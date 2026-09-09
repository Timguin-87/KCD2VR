#include "frame_sync.h"
#include "camera_hook.h"
#include "recursive_view_probe.h"
#include "debug_trace.h"

#include <DetourModKit/logger.hpp>
#include <windows.h>
#include <cstdio>
#include <cstddef>
#include <utility>
#include <cstring>
#include <cmath>
#include <algorithm>

namespace kcd2vr {

using DetourModKit::log;
namespace hook = DetourModKit::hook;

namespace {
constexpr std::uintptr_t kPublishViewCameraRva = 0x7F0AE0ull;
constexpr std::uintptr_t kGraphicsPipelineExecuteRva = 0x57278Cull;

static std::uintptr_t SafeReadPtr(const void* base, std::ptrdiff_t off) noexcept
{
    if (!base) return 0;
    __try { return *reinterpret_cast<const std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(base) + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}
static std::uint32_t SafeReadU32(const void* base, std::ptrdiff_t off) noexcept
{
    if (!base) return 0xFFFFFFFFu;
    __try { return *reinterpret_cast<const std::uint32_t*>(reinterpret_cast<std::uintptr_t>(base) + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0xFFFFFFFFu; }
}
static float SafeReadF32(const void* base, std::ptrdiff_t off) noexcept
{
    if (!base) return 0.0f;
    __try { return *reinterpret_cast<const float*>(reinterpret_cast<std::uintptr_t>(base) + off); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return 0.0f; }
}
static float RotationDeltaDegrees(const CameraRotationSample& a, const void* b) noexcept
{
    if (!b) return -1.0f;
    // CCamera Matrix34 uses columns 0/1/2 as right/forward/up. Normalize each
    // basis so a tiny scale/shear difference does not masquerade as rotation.
    float A[3][3]={{a.m[0],a.m[1],a.m[2]},
                   {a.m[3],a.m[4],a.m[5]},
                   {a.m[6],a.m[7],a.m[8]}};
    float B[3][3]={{SafeReadF32(b,0x00),SafeReadF32(b,0x04),SafeReadF32(b,0x08)},
                   {SafeReadF32(b,0x10),SafeReadF32(b,0x14),SafeReadF32(b,0x18)},
                   {SafeReadF32(b,0x20),SafeReadF32(b,0x24),SafeReadF32(b,0x28)}};
    float trace=0.0f;
    for(int c=0;c<3;++c){
        float an=0.0f,bn=0.0f,dot=0.0f;
        for(int r=0;r<3;++r){ an+=A[r][c]*A[r][c]; bn+=B[r][c]*B[r][c]; }
        an=std::sqrt(an); bn=std::sqrt(bn);
        if (!(an>1e-6f) || !(bn>1e-6f)) return -1.0f;
        for(int r=0;r<3;++r) dot+=(A[r][c]/an)*(B[r][c]/bn);
        trace+=dot;
    }
    const float cos_angle=std::clamp((trace-1.0f)*0.5f,-1.0f,1.0f);
    return std::acos(cos_angle)*57.29577951308232f;
}
const char* ViewTypeName(std::uint32_t t) noexcept
{
    switch (t) {
    case 0: return "Normal";
    case 1: return "Recursive";
    case 2: return "Shadow";
    case 3: return "BillboardGen";
    default: return "Unknown";
    }
}
}

FrameSync::PublishViewCameraFunc FrameSync::s_publish_original = nullptr;
FrameSync::GraphicsPipelineExecuteFunc FrameSync::s_pipeline_original = nullptr;
std::optional<hook::Hook> FrameSync::s_publish_hook;
std::optional<hook::Hook> FrameSync::s_pipeline_hook;
std::atomic<std::uint64_t> FrameSync::s_frame_serial{0};
std::atomic<int> FrameSync::s_eye{0};
std::atomic<int> FrameSync::s_mode{0};
std::atomic<std::uint64_t> FrameSync::s_pipeline_counts[4]{};
std::atomic<int> FrameSync::s_native_stereo_reentry_mode{0};
std::atomic<float> FrameSync::s_native_stereo_ipd{0.064f};
std::atomic<std::uint64_t> FrameSync::s_native_stereo_reentry_passes{0};
std::atomic<float> FrameSync::s_latest_published_fov_rad{0.0f};
std::atomic<std::uint64_t> FrameSync::s_published_fov_samples{0};
std::atomic<bool> FrameSync::s_dense_rotation_probe{false};
thread_local int FrameSync::s_pipeline_view_type = -1;
thread_local bool FrameSync::s_native_stereo_reentry_active = false;
thread_local int FrameSync::s_native_stereo_pass_eye = -1;

bool FrameSync::Install()
{
    const auto wh = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
    if (!wh) {
        log().error("V45-FRAME-SYNC: WHGame.dll unavailable");
        return false;
    }

    hook::InlineRequest pub_req{
        "kcd2vr_v45_publish_correlation",
        hook::Target{DetourModKit::Address{wh + kPublishViewCameraRva}},
        hook::Options{}
    };
    auto pub = hook::inline_at(std::move(pub_req), &FrameSync::PublishViewCameraDetour);
    if (!pub) {
        log().error("V45-FRAME-SYNC: ViewCamera publish hook install failed");
        return false;
    }
    s_publish_hook = std::move(*pub);
    s_publish_original = s_publish_hook->original<PublishViewCameraFunc>();
    if (!s_publish_hook->enable()) {
        log().error("V45-FRAME-SYNC: ViewCamera publish hook enable failed");
        s_publish_hook.reset(); s_publish_original = nullptr;
        return false;
    }

    hook::InlineRequest pipe_req{
        "kcd2vr_v45_recursive_pipeline_probe",
        hook::Target{DetourModKit::Address{wh + kGraphicsPipelineExecuteRva}},
        hook::Options{}
    };
    auto pipe = hook::inline_at(std::move(pipe_req), &FrameSync::GraphicsPipelineExecuteDetour);
    if (!pipe) {
        debug_trace::Write("V45-RECURSIVE pipeline hook install failed");
        return false;
    }
    s_pipeline_hook = std::move(*pipe);
    s_pipeline_original = s_pipeline_hook->original<GraphicsPipelineExecuteFunc>();
    if (!s_pipeline_hook->enable()) {
        debug_trace::Write("V45-RECURSIVE pipeline hook enable failed");
        s_pipeline_hook.reset(); s_pipeline_original = nullptr;
        return false;
    }

    s_frame_serial.store(0, std::memory_order_release);
    s_eye.store(0, std::memory_order_release);
    for (auto& c : s_pipeline_counts) c.store(0, std::memory_order_release);
    s_latest_published_fov_rad.store(0.0f, std::memory_order_release);
    s_published_fov_samples.store(0, std::memory_order_release);
    CameraHook::SetAfrEye(0);
    CameraHook::SetAfrStereoEnabled(false);
    debug_trace::Write("V45-PIPELINE armed: view type is CRenderView+0x14 (0 Normal,1 Recursive,2 Shadow,3 BillboardGen); +0x8EE0 is camera selector, NOT view type");
    return true;
}

void FrameSync::Uninstall()
{
    if (s_pipeline_hook) { (void)s_pipeline_hook->disable(); s_pipeline_hook.reset(); }
    s_pipeline_original = nullptr;
    if (s_publish_hook) { (void)s_publish_hook->disable(); s_publish_hook.reset(); }
    s_publish_original = nullptr;
}

void FrameSync::SetStereoMode(int mode)
{
    if (mode < 0) mode = 0;
    if (mode > 3) mode = 3;
    s_mode.store(mode, std::memory_order_release);
    // V46.5: mode 1 is the explicit AFR diagnostic. Keep the camera eye and
    // PRESENT tag on the same frame serial so the two cache images are truly
    // separated views rather than two temporally displaced mono images.
    CameraHook::SetAfrStereoEnabled(mode == 1);
    if (mode != 1) { s_eye.store(0, std::memory_order_release); CameraHook::SetAfrEye(0); }
}

std::uint64_t FrameSync::CurrentFrameSerial() { return s_frame_serial.load(std::memory_order_acquire); }
int FrameSync::CurrentEye() { return s_eye.load(std::memory_order_acquire) & 1; }

void FrameSync::SetNativeStereoReentryMode(int mode, float ipd_meters)
{
    if (mode < 0 || mode > 2) mode = 0;
    // V59.6 proof ladder deliberately permits tiny IPD values (10 mm total) so
    // scene-color separation can be proven without large culling/shadow disruption.
    if (!(ipd_meters >= 0.004f && ipd_meters < 0.10f)) ipd_meters = 0.064f;
    s_native_stereo_ipd.store(ipd_meters, std::memory_order_release);
    s_native_stereo_reentry_mode.store(mode, std::memory_order_release);
    char m[256]{};
    std::snprintf(m,sizeof(m),"V54-NATIVE-STEREO reentry mode=%d ipd=%.6f",mode,(double)ipd_meters);
    debug_trace::Write(m);
}
int FrameSync::NativeStereoReentryMode() { return s_native_stereo_reentry_mode.load(std::memory_order_acquire); }
std::uint64_t FrameSync::NativeStereoReentryPasses() { return s_native_stereo_reentry_passes.load(std::memory_order_acquire); }
int FrameSync::CurrentNativeStereoPassEye() { return s_native_stereo_pass_eye; }
int FrameSync::CurrentPipelineViewType() { return s_pipeline_view_type; }

std::uint64_t FrameSync::PipelineCount(int view_type)
{
    if (view_type < 0 || view_type > 3) return 0;
    return s_pipeline_counts[view_type].load(std::memory_order_acquire);
}

float FrameSync::LatestPublishedFovRadians()
{
    return s_latest_published_fov_rad.load(std::memory_order_acquire);
}

std::uint64_t FrameSync::PublishedFovSampleCount()
{
    return s_published_fov_samples.load(std::memory_order_acquire);
}

void FrameSync::SetDenseRotationProbe(bool enabled)
{
    s_dense_rotation_probe.store(enabled,std::memory_order_release);
    debug_trace::Write(enabled ? "V50-ROTATION-TRACE dense PublishViewCamera logging ON" : "V50-ROTATION-TRACE dense PublishViewCamera logging OFF");
}

void __fastcall FrameSync::PublishViewCameraDetour(void* system, void* camera) noexcept
{
    // V51 tracking rebuild: apply the immutable XR snapshot at the actual camera
    // publish boundary, then restore the source camera immediately after KCD2 copies it.
    // This replaces repeated frustum-hook mutation as the default path.
    float camera_backup[12]{};
    bool have_backup=false, publish_pose_applied=false;
    std::uint64_t publish_pose_frame=0;
    // Preselect the AFR eye for the camera about to be published so V51's
    // reversible publish transform uses the same eye offset that will tag it.
    if (s_mode.load(std::memory_order_acquire) == 1) {
        const auto next_serial=s_frame_serial.load(std::memory_order_acquire)+1ull;
        const int next_eye=static_cast<int>((next_serial-1ull)&1ull);
        s_eye.store(next_eye,std::memory_order_release); CameraHook::SetAfrEye(next_eye);
    }
    if (camera && CameraHook::GetInjectionMode() == 1) {
        __try { std::memcpy(camera_backup,camera,sizeof(camera_backup)); have_backup=true; }
        __except(EXCEPTION_EXECUTE_HANDLER) { have_backup=false; }
        if (have_backup) {
            // V61.44 FRAME NEXUS: culling and rendered camera must consume one
            // immutable pose. Reuse the exact gameplay CView matrix that V61.12
            // just handed to UpdateFrustumPlanes; only fall back to a fresh pose
            // sample if no same-camera, recent cull latch exists.
            publish_pose_applied=CameraHook::ApplyLatestCullingPoseToPublishedCamera(camera,&publish_pose_frame,24);
            if(!publish_pose_applied)
                publish_pose_applied=CameraHook::ApplyCurrentPoseToPublishedCamera(camera,&publish_pose_frame);
        }
    }

    // V61.7: compare the orientation that the frustum builder most recently
    // consumed against the XR-posed orientation about to be published/rendered.
    // If this delta grows with head motion, edge culling is being built from a
    // stale orientation rather than merely a too-narrow FOV.
    if (camera && publish_pose_applied) {
        CameraRotationSample fr{};
        if (CameraHook::GetLatestFrustumRotationSample(fr)) {
            const auto now=static_cast<std::uint64_t>(GetTickCount64());
            const auto age=(now>=fr.tick_ms)?(now-fr.tick_ms):0ull;
            const bool same_camera=fr.camera==reinterpret_cast<std::uintptr_t>(camera);
            const float delta_deg=RotationDeltaDegrees(fr,camera);
            static std::atomic<std::uint64_t> corr_n{0};
            static std::atomic<std::uint64_t> last_alert_ms{0};
            const auto n=corr_n.fetch_add(1,std::memory_order_relaxed)+1;
            bool log_now=(n<=16 || (n%300ull)==0);
            if (delta_deg>=0.50f && age<=50ull) {
                auto last=last_alert_ms.load(std::memory_order_relaxed);
                if (now>=last+100ull && last_alert_ms.compare_exchange_strong(last,now,std::memory_order_relaxed)) log_now=true;
            }
            if (log_now) {
                char cm[352]{};
                std::snprintf(cm,sizeof(cm),
                    "V61.7-CULL-TIMING n=%llu sameCamera=%s ageMs=%llu frustumPoseFrame=%llu publishPoseFrame=%llu deltaDeg=%.4f camera=%p tid=%lu",
                    static_cast<unsigned long long>(n),same_camera?"true":"false",
                    static_cast<unsigned long long>(age),
                    static_cast<unsigned long long>(fr.pose_frame),
                    static_cast<unsigned long long>(publish_pose_frame),delta_deg,camera,
                    static_cast<unsigned long>(GetCurrentThreadId()));
                debug_trace::Write(cm);
            }
        }
    }
    if (s_publish_original) s_publish_original(system, camera);
    if (have_backup && publish_pose_applied) {
        __try { std::memcpy(camera,camera_backup,sizeof(camera_backup)); }
        __except(EXCEPTION_EXECUTE_HANDLER) {}
    }
    const auto serial = s_frame_serial.fetch_add(1, std::memory_order_acq_rel) + 1;
    if (publish_pose_applied && (serial <= 8 || (serial % 900ull)==0)) {
        char pm[240]{}; std::snprintf(pm,sizeof(pm),
            "V51-TRACK-PUBLISH serial=%llu poseFrame=%llu reversible=true tid=%lu",
            static_cast<unsigned long long>(serial),static_cast<unsigned long long>(publish_pose_frame),
            static_cast<unsigned long>(GetCurrentThreadId())); debug_trace::Write(pm);
    }
    if (s_mode.load(std::memory_order_acquire) == 1) {
        const int eye = static_cast<int>((serial - 1ull) & 1ull);
        s_eye.store(eye, std::memory_order_release);
        CameraHook::SetAfrEye(eye);
    } else {
        s_eye.store(0, std::memory_order_release);
    }
    // CCamera's legacy matrix/front block stores the live symmetric FOV at
    // float[12] / +0x30 in this build. Historical camera summaries measured
    // ~1.309 rad (75 deg). V46.1 uses this only as an observer so cl_fov
    // requests can be verified against the camera actually published.
    const float fov_rad = SafeReadF32(camera, 0x30);
    if (fov_rad > 0.10f && fov_rad < 3.10f) {
        s_latest_published_fov_rad.store(fov_rad, std::memory_order_release);
        s_published_fov_samples.fetch_add(1, std::memory_order_relaxed);
    }
    if (serial <= 6 || (serial % 1200ull) == 0) {
        char msg[224]{};
        std::snprintf(msg, sizeof(msg),
            "V46.1-VIEWCAMERA-PUBLISH serial=%llu system=%p camera=%p fovRad=%.6f tid=%lu",
            static_cast<unsigned long long>(serial), system, camera, fov_rad,
            static_cast<unsigned long>(GetCurrentThreadId()));
        debug_trace::Write(msg);
    }
    if (s_dense_rotation_probe.load(std::memory_order_acquire) && camera) {
        const float m00=SafeReadF32(camera,0x00), m01=SafeReadF32(camera,0x04), m02=SafeReadF32(camera,0x08);
        const float m10=SafeReadF32(camera,0x10), m11=SafeReadF32(camera,0x14), m12=SafeReadF32(camera,0x18);
        const float m20=SafeReadF32(camera,0x20), m21=SafeReadF32(camera,0x24), m22=SafeReadF32(camera,0x28);
        const auto now=static_cast<std::uint64_t>(GetTickCount64()); const auto apply=CameraHook::LastPoseApplyTickMs();
        char dm[448]{}; std::snprintf(dm,sizeof(dm),
            "V50-ROTATION-PUBLISH serial=%llu poseFrame=%llu poseApplyAgeMs=%llu m=[%.5f %.5f %.5f|%.5f %.5f %.5f|%.5f %.5f %.5f] tid=%lu",
            static_cast<unsigned long long>(serial),static_cast<unsigned long long>(CameraHook::LastPoseFrameIndex()),
            static_cast<unsigned long long>(apply?now-apply:0),m00,m01,m02,m10,m11,m12,m20,m21,m22,static_cast<unsigned long>(GetCurrentThreadId()));
        debug_trace::Write(dm);
    }
}

void __fastcall FrameSync::GraphicsPipelineExecuteDetour(void* self) noexcept
{
    static std::atomic<std::uint64_t> calls{0};
    static std::atomic<std::uint64_t> normal_calls{0};
    static std::atomic<std::uint64_t> shadow_calls{0};
    static std::atomic<std::uint64_t> recursive_calls{0};
    const auto n = calls.fetch_add(1, std::memory_order_relaxed) + 1;

    // The pipeline's current render view was runtime-proven at +0x1978 in V29/V41.
    const auto crv = reinterpret_cast<void*>(SafeReadPtr(self, 0x1978));
    const auto view_type = SafeReadU32(crv, 0x14);       // ctor EViewType, proven statically in V45
    const auto selector = SafeReadU32(crv, 0x8EE0);     // active camera-bank selector, not EViewType
    const auto count = SafeReadU32(crv, 0xA330);
    const auto source = reinterpret_cast<void*>(SafeReadPtr(crv, 0x38));
    if (view_type <= 3u)
        s_pipeline_counts[view_type].fetch_add(1, std::memory_order_relaxed);

    bool should_log = false;
    std::uint64_t class_n = 0;
    if (view_type == 1u) {
        class_n = recursive_calls.fetch_add(1, std::memory_order_relaxed) + 1;
        should_log = class_n <= 96 || (class_n % 300ull) == 0;
    } else if (view_type == 0u) {
        class_n = normal_calls.fetch_add(1, std::memory_order_relaxed) + 1;
        should_log = class_n <= 6 || (class_n % 1200ull) == 0;
    } else if (view_type == 2u) {
        class_n = shadow_calls.fetch_add(1, std::memory_order_relaxed) + 1;
        should_log = class_n <= 6 || (class_n % 1200ull) == 0;
    } else if (n <= 12) {
        should_log = true;
    }

    if (should_log) {
        char msg[336]{};
        std::snprintf(msg, sizeof(msg),
            "V45-PIPELINE call=%llu classCall=%llu pipeline=%p crv=%p viewType=%u(%s) selector=%u source=%p records=%u recursivePasses=%llu tid=%lu",
            static_cast<unsigned long long>(n), static_cast<unsigned long long>(class_n),
            self, crv, view_type, ViewTypeName(view_type), selector, source, count,
            static_cast<unsigned long long>(RecursiveViewProbe::RecursivePassCount()),
            static_cast<unsigned long>(GetCurrentThreadId()));
        debug_trace::Write(msg);
    }

    const int previous_type = s_pipeline_view_type;
    s_pipeline_view_type = static_cast<int>(view_type);

    // V54: same-epoch dual scene-render experiment. This is deliberately gated to
    // Normal CRenderView only and never recursively re-enters itself. The camera
    // bank at +0x8EE8 was repeatedly observed as the renderer-consumed camera.
    // Writes are restored after each pass. Whether KCD2 routes the two executions
    // to distinct native stereo surfaces is what this experiment is testing.
    const int v54mode = s_native_stereo_reentry_mode.load(std::memory_order_acquire);
    if (s_pipeline_original && view_type == 0 && v54mode != 0 && !s_native_stereo_reentry_active && crv) {
        s_native_stereo_reentry_active = true;
        float saved[12]{};
        std::uint32_t saved_selector = 0;
        bool readable = false;
        __try {
            std::memcpy(saved, reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(crv)+0x8EE8), sizeof(saved));
            saved_selector = *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(crv)+0x8EE0);
            readable = true;
        } __except(EXCEPTION_EXECUTE_HANDLER) { readable = false; }
        if (readable) {
            const float half = 0.5f * s_native_stereo_ipd.load(std::memory_order_acquire);
            const int saved_eye = s_eye.load(std::memory_order_acquire) & 1;
            auto run_eye = [&](float eye, std::uint32_t selector) {
                // V58.8: expose the currently executing re-entry eye to the
                // existing PRESENT-tagging diagnostics. This does NOT itself
                // create a second target; it only makes any target KCD2 emits
                // during this pass attributable to L or R.
                s_eye.store(static_cast<int>(selector & 1u), std::memory_order_release);
                CameraHook::SetAfrEye(static_cast<int>(selector & 1u));
                s_native_stereo_pass_eye = static_cast<int>(selector & 1u);
                __try {
                    auto* cam = reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(crv)+0x8EE8);
                    std::memcpy(cam,saved,sizeof(saved));
                    // Matrix layout used throughout the existing camera probes: translation
                    // is [3,7,11], local right basis is [0,4,8].
                    cam[3]  += saved[0] * eye;
                    cam[7]  += saved[4] * eye;
                    cam[11] += saved[8] * eye;
                    if (v54mode == 2) *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(crv)+0x8EE0)=selector;
                } __except(EXCEPTION_EXECUTE_HANDLER) {}
                static std::atomic<std::uint64_t> native_eye_logs{0};
                const auto ln = native_eye_logs.fetch_add(1,std::memory_order_relaxed)+1;
                if (ln <= 12 || (ln % 600ull)==0) {
                    char nm[256]{}; std::snprintf(nm,sizeof(nm),
                        "V58.8-NATIVE-EYE-PASS n=%llu eye=%c mode=%d selector=%u offsetM=%.6f crv=%p",
                        static_cast<unsigned long long>(ln),selector==0?'L':'R',v54mode,selector,(double)eye,crv);
                    debug_trace::Write(nm);
                }
                s_pipeline_original(self);
                s_native_stereo_reentry_passes.fetch_add(1,std::memory_order_relaxed);
                s_native_stereo_pass_eye = -1;
            };
            run_eye(-half,0);
            run_eye(+half,1);
            s_eye.store(saved_eye,std::memory_order_release);
            CameraHook::SetAfrEye(saved_eye);
            __try {
                std::memcpy(reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(crv)+0x8EE8),saved,sizeof(saved));
                *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(crv)+0x8EE0)=saved_selector;
            } __except(EXCEPTION_EXECUTE_HANDLER) {}
        } else {
            s_pipeline_original(self);
        }
        s_native_stereo_pass_eye = -1;
        s_native_stereo_reentry_active = false;
    } else if (s_pipeline_original) {
        s_pipeline_original(self);
    }
    s_pipeline_view_type = previous_type;
}

} // namespace kcd2vr
