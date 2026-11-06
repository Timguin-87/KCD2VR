#include "diagnostics/log_sink_d48.h"
#include "frame_sync.h"
#include "camera_hook.h"
#include "recursive_view_probe.h"
#include "native_stereo_outer_lab.h"
#include "debug_trace.h"
#include "xr_manager.h"
#include "vrik/smooth_lab_f2.h"
#include "diagnostics/xr_timing165.h"

#include <DetourModKit/logger.hpp>
#include <windows.h>
#include <cstdio>
#include <cstddef>
#include <utility>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <array>
#include <limits>

namespace kcd2vr {

using DetourModKit::log;
namespace hook = DetourModKit::hook;

namespace {
constexpr std::uintptr_t kPublishViewCameraRva = 0x7F0AE0ull;
constexpr std::uintptr_t kGraphicsPipelineExecuteRva = 0x57278Cull;

// Exact retail setter7F0AE0 adds0x288 before CCamera assignment426E80.
// Only observed calls below produce metadata; unobserved writers are NOT
// authorized by matching the stored matrix. Native adapters remain private.
constexpr std::uintptr_t kSystemCameraOffsetR24=0x288;
std::atomic<std::uint64_t> g_camera_publication_r24{0};
std::atomic<bool> g_camera_publication_fault_r24{false};
core::SnapshotMailbox<CameraPublicationR24,4> g_camera_family_r24;
thread_local SystemRenderFamilyR24 t_system_render_family_r24{};
thread_local std::uint64_t t_system_render_serial_r24=0;
std::atomic<std::uint64_t> g_external_camera_writers_r24{0};
thread_local std::uint64_t t_external_camera_writer_r24=0;
static std::uint64_t InvalidateCameraFamilyR24() noexcept {
    const auto ticket=g_camera_publication_r24.fetch_add(1,std::memory_order_acq_rel)+1;
    if(!ticket)g_camera_publication_fault_r24.store(true,std::memory_order_release);
    return ticket;
}
static bool ReadCameraMatrixR24(const void* camera,float (&out)[12]) noexcept {
    if(!camera)return false;
    __try {std::memcpy(out,camera,sizeof(out));}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    for(float v:out)if(!std::isfinite(v))return false;
    return true;
}
static bool CurrentCameraReceiptR24(const CameraPublicationR24& receipt,std::uint64_t frame) noexcept {
    return !g_camera_publication_fault_r24.load(std::memory_order_acquire)&&
        !g_external_camera_writers_r24.load(std::memory_order_acquire)&&receipt.publication&&frame&&receipt.nativeFrame==frame&&
        receipt.family.nativeFrame==frame&&receipt.family.cameraGeneration==CameraHook::CurrentFamilyGenerationR24()&&
        tracking59c6::CameraFamilyCurrentR24(receipt.family,hand_smooth_h50::policyEpoch.load(std::memory_order_acquire),tracking59c6::NowNs());
}
static void CompleteCameraPublicationR24(void* system,const void* source,std::uint64_t ticket,
    std::uint64_t native_frame,const hj1::Sample* family,const float* expected,CameraPublicationR24* carried=nullptr) noexcept {
    if(carried)*carried={};
    if(!family||!family->ok||!system||!source||!expected||!ticket||!native_frame||
       !hj1::enabled.load(std::memory_order_acquire)||g_camera_publication_r24.load(std::memory_order_acquire)!=ticket)return;
    CameraPublicationR24 receipt{};receipt.family=*family;receipt.family.nativeFrame=native_frame;
    receipt.nativeFrame=native_frame;receipt.publication=ticket;
    receipt.system=reinterpret_cast<std::uintptr_t>(system);receipt.copiedCamera=receipt.system+kSystemCameraOffsetR24;
    receipt.sourceCamera=reinterpret_cast<std::uintptr_t>(source);
    if(!CurrentCameraReceiptR24(receipt,native_frame)||
       !ReadCameraMatrixR24(reinterpret_cast<const void*>(receipt.copiedCamera),receipt.matrix)||
       std::memcmp(receipt.matrix,expected,sizeof(receipt.matrix))||
       g_camera_publication_r24.load(std::memory_order_acquire)!=ticket)return;
    if(!g_camera_family_r24.publish(receipt))return;
    if(carried&&g_camera_publication_r24.load(std::memory_order_acquire)==ticket)*carried=receipt;
}

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

// V65.03 F2 render-ownership ring. Writer publishes the exact Matrix34 passed
// through PublishViewCamera after HMD composition. The renderer-side probe then
// matches the Normal CRenderView camera bank against these immutable samples.
// Each slot is published by sequence number last, so readers either see a whole
// sample or skip it. This is diagnostics only and never changes camera state.
struct HeadPublishSample {
    std::atomic<std::uint64_t> seq{0};
    std::uint64_t publish_serial{0};
    std::uint64_t pose_frame{0};
    std::uint64_t tick_ms{0};
    float m[12]{};
};
constexpr std::size_t kHeadPublishRing=32;
std::array<HeadPublishSample,kHeadPublishRing> g_head_publish_ring{};
std::atomic<int> g_head_probe_level{0};
std::atomic<std::uint64_t> g_head_publish_seq{0};
std::atomic<std::uint64_t> g_head_published_samples{0};
std::atomic<std::uint64_t> g_head_renderer_samples{0};
std::atomic<std::uint64_t> g_head_renderer_matches{0};
std::atomic<std::uint64_t> g_head_renderer_misses{0};
std::atomic<std::uint64_t> g_head_last_publish_serial{0};
std::atomic<std::uint64_t> g_head_last_publish_pose{0};
std::atomic<std::uint64_t> g_head_last_match_serial{0};
std::atomic<std::uint64_t> g_head_last_match_pose{0};
std::atomic<std::uint64_t> g_head_last_match_tick_ms{0};
std::atomic<bool> g_head_ownership_fix_enabled{false};
std::atomic<float> g_head_last_rot_error{-1.0f};
std::atomic<float> g_head_last_pos_error{-1.0f};

float MatrixRotationDeltaDegrees12(const float* a,const float* b) noexcept
{
    if(!a||!b) return -1.0f;
    float trace=0.0f;
    for(int c=0;c<3;++c){
        float an=0.0f,bn=0.0f,dot=0.0f;
        for(int r=0;r<3;++r){ const float av=a[r*4+c],bv=b[r*4+c]; an+=av*av; bn+=bv*bv; }
        an=std::sqrt(an); bn=std::sqrt(bn);
        if(!(an>1e-6f)||!(bn>1e-6f)) return -1.0f;
        for(int r=0;r<3;++r) dot+=(a[r*4+c]/an)*(b[r*4+c]/bn);
        trace+=dot;
    }
    const float cs=std::clamp((trace-1.0f)*0.5f,-1.0f,1.0f);
    return std::acos(cs)*57.29577951308232f;
}

void PublishHeadOwnershipSample(std::uint64_t serial,std::uint64_t pose_frame,const float matrix[12]) noexcept
{
    if(g_head_probe_level.load(std::memory_order_acquire)<=0 || !matrix) return;
    const auto seq=g_head_publish_seq.fetch_add(1,std::memory_order_acq_rel)+1;
    auto& slot=g_head_publish_ring[seq%kHeadPublishRing];
    slot.seq.store(0,std::memory_order_release);
    slot.publish_serial=serial; slot.pose_frame=pose_frame; slot.tick_ms=static_cast<std::uint64_t>(GetTickCount64());
    std::memcpy(slot.m,matrix,sizeof(slot.m));
    slot.seq.store(seq,std::memory_order_release);
    g_head_published_samples.fetch_add(1,std::memory_order_relaxed);
    g_head_last_publish_serial.store(serial,std::memory_order_release);
    g_head_last_publish_pose.store(pose_frame,std::memory_order_release);
}

float g_head_last_match_lateral_m=0.0f; // V66.46 diagnostics: eye offset seen by the matcher
bool MatchHeadOwnershipRendererCamera(const float renderer[12],std::uint64_t& out_serial,std::uint64_t& out_pose,float& out_rot,float& out_pos) noexcept
{
    out_serial=out_pose=0; out_rot=out_pos=-1.0f;
    float best=std::numeric_limits<float>::max();
    for(auto& slot:g_head_publish_ring){
        const auto a=slot.seq.load(std::memory_order_acquire); if(!a) continue;
        float m[12]{}; const auto serial=slot.publish_serial, pose=slot.pose_frame;
        std::memcpy(m,slot.m,sizeof(m));
        const auto b=slot.seq.load(std::memory_order_acquire); if(a!=b||!b) continue;
        const float rot=MatrixRotationDeltaDegrees12(m,renderer);
        // V66.46: native stereo renders each eye from the published CENTER camera shifted
        // by +-IPD/2 along the camera right axis (stereo_eye_control_model_v6604.h
        // ShiftCamera: t += right*meters, right=(m[0],m[4],m[8])). V65.04 compared full
        // translation with a 4 mm limit, so in stereo (~35 mm offset) NOTHING ever matched
        // and every pair fell back to the newest pose (wrong XR pose token -> head jitter).
        // Remove the component along the published right axis; keep the 4 mm limit on
        // what remains and allow up to 55 mm along the axis (any real IPD half).
        const float dx=renderer[3]-m[3], dy=renderer[7]-m[7], dz=renderer[11]-m[11];
        float rx=m[0], ry=m[4], rz=m[8];
        const float rl=std::sqrt(rx*rx+ry*ry+rz*rz);
        if(!(rl>0.5f && rl<2.0f)) continue;
        rx/=rl; ry/=rl; rz/=rl;
        const float lat=dx*rx+dy*ry+dz*rz;
        const float px=dx-lat*rx, py=dy-lat*ry, pz=dz-lat*rz;
        const float perp=std::sqrt(px*px+py*py+pz*pz);
        if(rot<0.0f||!std::isfinite(perp)||!std::isfinite(lat)) continue;
        if(std::fabs(lat)>0.055f) continue;
        const float score=rot + perp*40.0f;
        if(score<best){ best=score; out_serial=serial; out_pose=pose; out_rot=rot; out_pos=perp; g_head_last_match_lateral_m=lat; }
    }
    // Exact renderer copies land extremely close; allow a small tolerance so
    // harmless float normalization does not convert a real match into a miss.
    return out_serial!=0 && out_rot<=0.20f && out_pos<=0.004f;
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
std::atomic<bool> FrameSync::s_pipeline_probe_enabled{false};
thread_local int FrameSync::s_pipeline_view_type = -1;
thread_local bool FrameSync::s_native_stereo_reentry_active = false;
thread_local int FrameSync::s_native_stereo_pass_eye = -1;

bool FrameSync::Install()
{
    const auto wh = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
    if (!wh) {
        KCD2VR_DIAG_LOG(Bug,error,"V45-FRAME-SYNC: WHGame.dll unavailable");
        return false;
    }

    hook::InlineRequest pub_req{
        "kcd2vr_v45_publish_correlation",
        hook::Target{DetourModKit::Address{wh + kPublishViewCameraRva}},
        hook::Options{}
    };
    auto pub = hook::inline_at(std::move(pub_req), &FrameSync::PublishViewCameraDetour);
    if (!pub) {
        KCD2VR_DIAG_LOG(Bug,error,"V45-FRAME-SYNC: ViewCamera publish hook install failed");
        return false;
    }
    s_publish_hook = std::move(*pub);
    s_publish_original = s_publish_hook->original<PublishViewCameraFunc>();
    if (!s_publish_hook->enable()) {
        KCD2VR_DIAG_LOG(Bug,error,"V45-FRAME-SYNC: ViewCamera publish hook enable failed");
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
        debug_trace::Write("V65.04-PIPELINE diagnostic hook unavailable; production publish path remains active");
        s_pipeline_hook.reset(); s_pipeline_original=nullptr;
    } else {
        s_pipeline_hook = std::move(*pipe);
        s_pipeline_original = s_pipeline_hook->original<GraphicsPipelineExecuteFunc>();
        s_pipeline_probe_enabled.store(false, std::memory_order_release);
    }

    s_frame_serial.store(0, std::memory_order_release);
    s_eye.store(0, std::memory_order_release);
    for (auto& c : s_pipeline_counts) c.store(0, std::memory_order_release);
    s_latest_published_fov_rad.store(0.0f, std::memory_order_release);
    s_published_fov_samples.store(0, std::memory_order_release);
    CameraHook::SetAfrEye(0);
    CameraHook::SetAfrStereoEnabled(false);
    debug_trace::Write("V65.04-PIPELINE probe created DISABLED; F2/F8 enable it lazily");
    return true;
}

bool FrameSync::SetPipelineProbeEnabled(bool enabled)
{
    const bool current=s_pipeline_probe_enabled.load(std::memory_order_acquire);
    if(current==enabled) return true;
    if(!s_pipeline_hook) return !enabled;
    if(enabled){
        for(auto& c:s_pipeline_counts) c.store(0,std::memory_order_release);
        if(!s_pipeline_hook->enable()) return false;
        s_pipeline_probe_enabled.store(true,std::memory_order_release);
        debug_trace::Write("V65.04-PIPELINE probe ENABLED by active F2/F8 ladder");
        return true;
    }
    (void)s_pipeline_hook->disable();
    s_pipeline_probe_enabled.store(false,std::memory_order_release);
    debug_trace::Write("V65.04-PIPELINE probe DISABLED; normal play hot path restored");
    return true;
}

bool FrameSync::PipelineProbeEnabled(){ return s_pipeline_probe_enabled.load(std::memory_order_acquire); }

void FrameSync::Uninstall()
{
    (void)SetPipelineProbeEnabled(false);
    if (s_pipeline_hook) { s_pipeline_hook.reset(); }
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

void FrameSync::SetHeadOwnershipProbeLevel(int level)
{
    level=std::clamp(level,0,4);
    g_head_probe_level.store(level,std::memory_order_release);
    if(level==0){
        g_head_published_samples.store(0); g_head_renderer_samples.store(0);
        g_head_renderer_matches.store(0); g_head_renderer_misses.store(0);
        g_head_last_publish_serial.store(0); g_head_last_publish_pose.store(0);
        g_head_last_match_serial.store(0); g_head_last_match_pose.store(0);
        g_head_last_match_tick_ms.store(0);
        g_head_last_rot_error.store(-1.0f); g_head_last_pos_error.store(-1.0f);
        for(auto& slot:g_head_publish_ring) slot.seq.store(0,std::memory_order_release);
    }
    char m[256]{}; std::snprintf(m,sizeof(m),
        "V65.04-F2 MATCHER level=%d matcherReadOnly=1 cameraPoseWrites=0 presentTokenWriteEnabled=%d rawImmediateSwitch=0",
        level,HeadOwnershipFixEnabled()?1:0);
    debug_trace::Write(m);
}
int FrameSync::HeadOwnershipProbeLevel(){ return g_head_probe_level.load(std::memory_order_acquire); }

void FrameSync::SetHeadOwnershipFixEnabled(bool enabled)
{
    g_head_ownership_fix_enabled.store(enabled,std::memory_order_release);
    char m[224]{}; std::snprintf(m,sizeof(m),
        "V65.04-F2-FIX enabled=%d rendererMatchedPoseOwnsPresent=%d rawImmediateSwitch=0",
        enabled?1:0,enabled?1:0);
    debug_trace::Write(m);
}

bool FrameSync::HeadOwnershipFixEnabled()
{
    return g_head_ownership_fix_enabled.load(std::memory_order_acquire);
}

std::uint64_t FrameSync::LatestRendererMatchedPoseFrame(std::uint32_t max_age_ms)
{
    if(!HeadOwnershipFixEnabled()) return 0;
    const auto pose=g_head_last_match_pose.load(std::memory_order_acquire);
    const auto tick=g_head_last_match_tick_ms.load(std::memory_order_acquire);
    if(!pose || !tick) return 0;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(now<tick || now-tick>static_cast<std::uint64_t>(max_age_ms)) return 0;
    return pose;
}

bool FrameSync::PublishViewCameraHookInstalled()
{
    return s_publish_hook.has_value() && s_publish_original != nullptr;
}

bool FrameSync::ReadSystemCameraFamilyR24(void* system,const void* actual_camera,CameraPublicationR24& out) noexcept {
    // INVESTIGATE_D67R1: this optional camera-copy receipt is not a skeleton
    // receipt. Native skinData is cached by the renderer's own three-slot pool
    // counter (3D4E2C), then stored at renderObject+58 (3D4C49); armor/remapped
    // data can take 4A34FC. Join those exact consumers to the active eye before
    // transporting a family through async jobs. The currently disabled HJ1
    // registry has no installed native enqueue/callback adapter, so its presence
    // cannot establish worker ownership or justify advancing all input by N XR
    // periods. Acceptance: both eyes, palette/previous palette and held object
    // carry the same proved native draw identity and intended source family.
    out={};
    if(!system||!actual_camera||!hj1::enabled.load(std::memory_order_acquire))return false;
    const auto frame=native_stereo_outer_lab::CurrentNativeFrameHJ1();
    const auto ticket=g_camera_publication_r24.load(std::memory_order_acquire);
    CameraPublicationR24 receipt{};float matrix[12]{};
    if(!ticket||!g_camera_family_r24.read(receipt)||receipt.publication!=ticket||
       receipt.system!=reinterpret_cast<std::uintptr_t>(system)||receipt.copiedCamera!=reinterpret_cast<std::uintptr_t>(actual_camera)||
       receipt.copiedCamera!=receipt.system+kSystemCameraOffsetR24||!CurrentCameraReceiptR24(receipt,frame)||
       !ReadCameraMatrixR24(actual_camera,matrix)||std::memcmp(matrix,receipt.matrix,sizeof(matrix))||
       g_camera_publication_r24.load(std::memory_order_acquire)!=ticket)return false;
    out=receipt;return true;
}
SystemRenderMarkR24 FrameSync::BeginSystemRenderFamilyR24(void* system,std::uint64_t native_frame) noexcept {
    const SystemRenderMarkR24 mark{t_system_render_family_r24,++t_system_render_serial_r24};
    t_system_render_family_r24={};t_system_render_family_r24.active=true;t_system_render_family_r24.serial=mark.entered;
    t_system_render_family_r24.system=reinterpret_cast<std::uintptr_t>(system);t_system_render_family_r24.nativeFrame=native_frame;
    // A nested original CSystem invocation is not the proved primary lifetime.
    // Mask it; restoring an outer receipt still revalidates publication/currentness.
    if(!mark.entered||mark.previous.active){t_system_render_family_r24.poisoned=true;return mark;}
    if(native_frame&&native_frame==native_stereo_outer_lab::CurrentNativeFrameHJ1())
        (void)ReadSystemCameraFamilyR24(system,reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(system)+kSystemCameraOffsetR24),t_system_render_family_r24.camera);
    return mark;
}
void FrameSync::EndSystemRenderFamilyR24(const SystemRenderMarkR24& mark) noexcept {
    if(t_system_render_family_r24.serial==mark.entered)t_system_render_family_r24=mark.previous;
    else {t_system_render_family_r24={};t_system_render_family_r24.active=t_system_render_family_r24.poisoned=true;}
}

bool FrameSync::SetViewCameraRetail(void* system, void* camera,CameraPublicationR24* copied_family) noexcept
{
    // A stereo copy carries its captured family; a later/nested publication
    // cannot refresh or replace that family. Native setter behavior is unchanged.
    hj1::Sample family{};float expected[12]{};bool known=false;
    const auto frame=native_stereo_outer_lab::CurrentNativeFrameHJ1();
    if(copied_family&&hj1::enabled.load(std::memory_order_acquire)){
        CameraPublicationR24 current{};
        known=ReadSystemCameraFamilyR24(system,reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(system)+kSystemCameraOffsetR24),current)&&
            copied_family->publication==current.publication&&copied_family->system==current.system&&
            CurrentCameraReceiptR24(*copied_family,frame)&&ReadCameraMatrixR24(camera,expected);
        if(known)family=copied_family->family;
    }
    const auto ticket=InvalidateCameraFamilyR24();
    if(copied_family)*copied_family={};
    if(!system || !camera) return false;
    auto fn=s_publish_original;
    if(!fn) {
        const auto wh=reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
        if(!wh) return false;
        fn=reinterpret_cast<PublishViewCameraFunc>(wh+kPublishViewCameraRva);
    }
    __try {
        fn(system,camera);
        CompleteCameraPublicationR24(system,camera,ticket,frame,known?&family:nullptr,known?expected:nullptr,copied_family);
        return true;
    }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

#include "vrik/early_frame_pacing_d45.inl"

void __fastcall FrameSync::PublishViewCameraDetour(void* system, void* camera) noexcept
{
    // Invalidate at entry, including unknown/native-only publications and any
    // exceptional exit. Original camera copying and source restore stay in place.
    const auto familyTicketR24=InvalidateCameraFamilyR24();
    const auto familyFrameR24=native_stereo_outer_lab::CurrentNativeFrameHJ1();
    hj1::Sample publishFamilyR24{};float familyMatrixR24[12]{};bool familyMatrixKnownR24=false;
    const bool wantFamilyR24=hj1::enabled.load(std::memory_order_acquire);
    // V51 tracking rebuild: apply the immutable XR snapshot at the actual camera
    // publish boundary, then restore the source camera immediately after KCD2 copies it.
    // This replaces repeated frustum-hook mutation as the default path.
    float camera_backup[12]{};
    bool have_backup=false, publish_pose_applied=false;
    std::uint64_t publish_pose_frame=0;
    float ownership_publish_matrix[12]{}; bool ownership_publish_matrix_valid=false;
    // V64.48: approximate Crysis VR's xrWaitFrame-before-render architecture
    // without moving OpenXR ownership threads. In gated modes the gameplay
    // PublishViewCamera boundary waits for a NEW immutable XR frame token.
    const int xr_gate_mode=XrManager::Instance().GetXrRenderGateMode();
    static thread_local std::uint64_t gate_frame=0;
    static thread_local std::uint64_t gate_last_wait_tick=0;
    // D64 F2/2 late comparison: the accepted I10 publish boundary, sharing
    // the per-native-frame claim/token with the alternative early location.
    // Camera copy/restore and expected-Present ownership stay below. Mode4
    // bypasses explicit legacy token gating as well as the D67 pacing route.
    PaceGameplayAtD64(false);
    if (xr_gate_mode>0 && !motion_trials_d67::UncappedSelectedAt(0) && CameraHook::GetInjectionMode()==1) {
        const auto start=static_cast<std::uint64_t>(GetTickCount64());
        // CryEngine may publish auxiliary cameras in a tight burst. Treat calls
        // within 3 ms as part of the same game-frame publication so one KCD2
        // frame cannot accidentally consume multiple XR frame tokens.
        const bool burst_reuse=gate_last_wait_tick && start>=gate_last_wait_tick && (start-gate_last_wait_tick)<3ull;
        const bool gate_ok=burst_reuse ? true : XrManager::Instance().WaitForNextRenderFrameToken(gate_frame,20);
        if(!burst_reuse) gate_last_wait_tick=static_cast<std::uint64_t>(GetTickCount64());
        const auto waited=static_cast<std::uint64_t>(GetTickCount64())-start;
        static std::atomic<std::uint64_t> gate_n{0}; const auto gn=gate_n.fetch_add(1,std::memory_order_relaxed)+1;
        if(!gate_ok || gn<=8 || (gn%300ull)==0){ char gm[288]{}; std::snprintf(gm,sizeof(gm),
            "V64.48-XR-GATE camera n=%llu mode=%d ok=%d xrFrame=%llu waitMs=%llu burstReuse=%d",
            (unsigned long long)gn,xr_gate_mode,gate_ok?1:0,(unsigned long long)gate_frame,(unsigned long long)waited,burst_reuse?1:0); debug_trace::Write(gm); }
    }
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
            // V64.39: F3=1 can bypass the historical culling-pose reuse path.
            // That latch intentionally allowed a pose up to 24 ms old so culling and
            // rendering shared one matrix, but at 90 Hz that can be >2 headset frames
            // stale and is visible as micro-jitter/double imaging during head rotation.
            // In fresh-publish mode, sample once here at the actual render-camera
            // publication boundary. The source camera is still restored below.
            if(CameraHook::GetFreshPublishPose()) {
                publish_pose_applied=CameraHook::ApplyCurrentPoseToPublishedCamera(camera,&publish_pose_frame,true,wantFamilyR24?&publishFamilyR24:nullptr);
            } else {
                publish_pose_applied=CameraHook::ApplyLatestCullingPoseToPublishedCamera(camera,&publish_pose_frame,24,wantFamilyR24?&publishFamilyR24:nullptr);
                if(!publish_pose_applied)
                    kcd2vr::diagnostics::xr165::LatchObserved(false,0); // W3 F2 r1: latch not usable -> fresh sample below
                if(!publish_pose_applied)
                    publish_pose_applied=CameraHook::ApplyCurrentPoseToPublishedCamera(camera,&publish_pose_frame,true,wantFamilyR24?&publishFamilyR24:nullptr);
            }
        }
    }

    if (xr_gate_mode>=2 && publish_pose_applied && publish_pose_frame)
        XrManager::Instance().SetExpectedPresentCameraFrame(publish_pose_frame);

    // V61.7: compare the orientation that the frustum builder most recently
    // consumed against the XR-posed orientation about to be published/rendered.
    // If this delta grows with head motion, edge culling is being built from a
    // stale orientation rather than merely a too-narrow FOV.
    if (camera && publish_pose_applied && g_head_probe_level.load(std::memory_order_acquire)>=1) {
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
    if(camera && publish_pose_applied && g_head_probe_level.load(std::memory_order_acquire)>0){
        __try { std::memcpy(ownership_publish_matrix,camera,sizeof(ownership_publish_matrix)); ownership_publish_matrix_valid=true; }
        __except(EXCEPTION_EXECUTE_HANDLER) { ownership_publish_matrix_valid=false; }
    }
    if(wantFamilyR24&&publish_pose_applied&&publishFamilyR24.ok)familyMatrixKnownR24=ReadCameraMatrixR24(camera,familyMatrixR24);
    if (s_publish_original) {
        s_publish_original(system, camera);
        CompleteCameraPublicationR24(system,camera,familyTicketR24,familyFrameR24,
            familyMatrixKnownR24?&publishFamilyR24:nullptr,familyMatrixKnownR24?familyMatrixR24:nullptr);
    }
    if (have_backup && publish_pose_applied) {
        __try { std::memcpy(camera,camera_backup,sizeof(camera_backup)); }
        __except(EXCEPTION_EXECUTE_HANDLER) {}
    }
    const auto serial = s_frame_serial.fetch_add(1, std::memory_order_acq_rel) + 1;
    if(ownership_publish_matrix_valid) PublishHeadOwnershipSample(serial,publish_pose_frame,ownership_publish_matrix);
    if (publish_pose_applied && (serial <= 8 || (serial % 900ull)==0) && g_head_probe_level.load(std::memory_order_acquire)>0) {
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
    if (g_head_probe_level.load(std::memory_order_acquire)>0 && (serial <= 6 || (serial % 1200ull) == 0)) {
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

    // V65.04a F8: reuse this already-existing pipeline hook for frame-nexus
    // correlation instead of installing another detour on the same renderer
    // function. The F8 module records only counters/tokens and performs no writes.
    native_stereo_outer_lab::NotifyPipelineExecute(self, crv, view_type, selector);

    bool should_log = false;
    std::uint64_t class_n = 0;
    const int headProbe=g_head_probe_level.load(std::memory_order_acquire);
    if (view_type == 1u) {
        class_n = recursive_calls.fetch_add(1, std::memory_order_relaxed) + 1;
        should_log = headProbe>=4 && (class_n <= 8 || (class_n % 600ull) == 0);
    } else if (view_type == 0u) {
        class_n = normal_calls.fetch_add(1, std::memory_order_relaxed) + 1;
        should_log = headProbe>=3 && (class_n <= 6 || (class_n % 600ull) == 0);
    } else if (view_type == 2u) {
        class_n = shadow_calls.fetch_add(1, std::memory_order_relaxed) + 1;
        should_log = headProbe>=4 && (class_n <= 4 || (class_n % 1200ull) == 0);
    } else if (headProbe>=4 && n <= 12) {
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

    if(headProbe>=3 && view_type==0u && crv){
        float renderer[12]{}; bool ok=false;
        __try { std::memcpy(renderer,reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(crv)+0x8EE8),sizeof(renderer)); ok=true; }
        __except(EXCEPTION_EXECUTE_HANDLER){ ok=false; }
        if(ok){
            g_head_renderer_samples.fetch_add(1,std::memory_order_relaxed);
            std::uint64_t ms=0,mp=0; float rd=-1.0f,pd=-1.0f;
            const bool matched=MatchHeadOwnershipRendererCamera(renderer,ms,mp,rd,pd);
            if(matched) g_head_renderer_matches.fetch_add(1,std::memory_order_relaxed); else g_head_renderer_misses.fetch_add(1,std::memory_order_relaxed);
            g_head_last_rot_error.store(rd,std::memory_order_release); g_head_last_pos_error.store(pd,std::memory_order_release);
            if(matched){
                g_head_last_match_serial.store(ms,std::memory_order_release);
                g_head_last_match_pose.store(mp,std::memory_order_release);
                g_head_last_match_tick_ms.store(static_cast<std::uint64_t>(GetTickCount64()),std::memory_order_release);
            }
            static std::atomic<std::uint64_t> lastHeadLog{0}; const auto now=static_cast<std::uint64_t>(GetTickCount64()); auto prev=lastHeadLog.load();
            if((!matched || now>=prev+1000ull) && lastHeadLog.compare_exchange_strong(prev,now)){
                char hm[448]{}; std::snprintf(hm,sizeof(hm),
                    "V65.04-F2-RENDER level=%d matched=%d crv=%p selector=%u publishSerial=%llu poseFrame=%llu rotErrDeg=%.5f posErrM=%.6f lastAppliedPose=%llu",
                    headProbe,matched?1:0,crv,selector,(unsigned long long)ms,(unsigned long long)mp,double(rd),double(pd),
                    (unsigned long long)CameraHook::LastPoseFrameIndex()); debug_trace::Write(hm);
            }
        }
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
    native_stereo_outer_lab::NotifyPipelineExecuteReturn(self, crv, view_type, selector);
    s_pipeline_view_type = previous_type;
}

} // namespace kcd2vr
