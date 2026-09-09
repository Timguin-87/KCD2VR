// camera_hook.cpp
//
// Candidate ladder for CCamera::UpdateFrustumPlanes reused, with attribution,
// from tkhquang/KCD2Tools (TPVCamera mod, MIT licensed), whose aob_resolver.hpp
// documents these three signatures as verified against the live retail
// WHGame.dll (P1 = full prologue, P2 = mid-prologue walk-back -3, P3 = body
// matrix-read run walk-back -0x1A).

#include "camera_hook.h"
#include "xr_manager.h"
#include "debug_trace.h"

#include <DetourModKit/scan.hpp>
#include <DetourModKit/hook.hpp>
#include <DetourModKit/logger.hpp>

#include <windows.h>
#include <algorithm>
#include <vector>
#include <mutex>
#include <atomic>
#include <string_view>
#include <cmath>
#include <cstddef>
#include <intrin.h>
#include <cstdint>
#include <cstdarg>
#include <cstdio>

namespace kcd2vr {

using DetourModKit::log;
namespace scan = DetourModKit::scan;
namespace hook = DetourModKit::hook;

std::mutex CameraHook::s_pose_mutex;
XrPose CameraHook::s_current_pose{};
XrPose CameraHook::s_origin_pose{};
std::atomic<float> CameraHook::s_current_scale{1.0f};
std::atomic<std::uint64_t> CameraHook::s_current_pose_frame_index{0};
std::atomic<std::uint64_t> CameraHook::s_last_applied_pose_frame_index{0};
std::atomic<bool> CameraHook::s_pose_received{false};
std::atomic<bool> CameraHook::s_origin_valid{false};
std::atomic<bool> CameraHook::s_recenter_requested{false};
std::atomic<bool> CameraHook::s_apply_rotation{true};
std::atomic<bool> CameraHook::s_apply_position{true};
std::atomic<bool> CameraHook::s_afr_enabled{false};
std::atomic<int> CameraHook::s_afr_eye{0};
std::atomic<float> CameraHook::s_afr_ipd_meters{0.064f};
std::atomic<float> CameraHook::s_afr_ipd_scale{1.0f};
std::atomic<float> CameraHook::s_cull_frustum_scale{1.0f};
std::atomic<float> CameraHook::s_camera_side_offset_m{0.0f};
std::atomic<float> CameraHook::s_camera_back_offset_m{0.0f};
std::atomic<float> CameraHook::s_camera_height_offset_m{0.0f};
std::atomic<int> CameraHook::s_injection_mode{1}; // V51 default: late reversible PublishViewCamera injection
std::atomic<int> CameraHook::s_pose_source_mode{0};
std::atomic<float> CameraHook::s_prediction_offset_ms{0.0f};
std::atomic<int> CameraHook::s_rotation_order{0};
std::atomic<int> CameraHook::s_rotation_filter_mode{0};
std::atomic<std::uint64_t> CameraHook::s_rotation_filter_generation{1};
std::atomic<float> CameraHook::s_worst_gap_deg{0.0f};
std::atomic<float> CameraHook::s_worst_hmd_deg{0.0f};
std::atomic<float> CameraHook::s_worst_final_deg{0.0f};
std::atomic<std::uint64_t> CameraHook::s_worst_n{0};
std::atomic<float> CameraHook::s_gameplay_fov_rad{0.0f};
std::atomic<std::uint64_t> CameraHook::s_gameplay_fov_samples{0};

void CameraHook::ResetWorstCaseAngular()
{
    s_worst_gap_deg.store(0.0f, std::memory_order_release);
    s_worst_hmd_deg.store(0.0f, std::memory_order_release);
    s_worst_final_deg.store(0.0f, std::memory_order_release);
    s_worst_n.store(0, std::memory_order_release);
}

void CameraHook::GetWorstCaseAngular(float& out_gap_deg, float& out_hmd_deg, float& out_final_deg, std::uint64_t& out_n)
{
    out_gap_deg = s_worst_gap_deg.load(std::memory_order_acquire);
    out_hmd_deg = s_worst_hmd_deg.load(std::memory_order_acquire);
    out_final_deg = s_worst_final_deg.load(std::memory_order_acquire);
    out_n = s_worst_n.load(std::memory_order_acquire);
}

void CameraHook::SetCullFrustumScale(float scale)
{
    s_cull_frustum_scale.store(std::clamp(scale, 1.0f, 1.60f), std::memory_order_release);
}

float CameraHook::GetCullFrustumScale()
{
    return s_cull_frustum_scale.load(std::memory_order_acquire);
}

void CameraHook::SetRotationFilterMode(int mode)
{
    // V56.1: V56 advertised modes 4..7 but this setter still clamped them to 3,
    // so the 6/8/10 ms and adaptive phases were not actually selected.
    // Keep the legacy 1/2 values available for diagnostics, and allow all V56 filters.
    if (mode < 0) mode = 0; if (mode > 7) mode = 7;
    s_rotation_filter_mode.store(mode, std::memory_order_release);
    s_rotation_filter_generation.fetch_add(1, std::memory_order_acq_rel);
    const char* name =
        mode==0 ? "RAW" :
        mode==1 ? "HOLD_45HZ" :
        mode==2 ? "HOLD_60HZ" :
        mode==3 ? "SMOOTH_TAU_4MS" :
        mode==4 ? "SMOOTH_TAU_6MS" :
        mode==5 ? "SMOOTH_TAU_8MS" :
        mode==6 ? "SMOOTH_TAU_10MS" : "SMOOTH_ADAPTIVE";
    char m[176]{}; std::snprintf(m,sizeof(m),"V56.1-ROT-FILTER mode=%d (%s)",mode,name); debug_trace::Write(m);
}
int CameraHook::GetRotationFilterMode(){ return s_rotation_filter_mode.load(std::memory_order_acquire); }

CameraHook::FrustumBuildFunc CameraHook::s_original = nullptr;

namespace {

// CryEngine 3x4 matrix: 3 rows x 4 columns, row-major. Column 3 is
// translation. The camera's matrix sits at OFFSET 0 of the pointer
// the hook receives directly.
struct Matrix34f
{
    float m[3][4];
};

} // namespace

bool CameraHook::Install()
{
    std::vector<scan::Candidate> ladder;

    auto p1 = scan::Pattern::compile(
        "48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 68 ?? 48 81 EC ?? ?? 00 00 F3 "
        "0F 10 09 48 8B D9");
    auto p2 = scan::Pattern::compile(
        "55 53 56 57 41 54 41 55 41 56 41 57 48 8D 68 ?? 48 81 EC ?? ?? 00 00 F3 0F 10 09 "
        "48 8B D9 F3 0F 10 59 08");
    auto p3 = scan::Pattern::compile(
        "F3 0F 10 09 48 8B D9 F3 0F 10 59 08 F3 0F 10 51 10 F3 0F 10 41 24 F3 0F 10 61 28");

    if (p1) ladder.push_back(scan::Candidate::direct("Frustum_P1_PrologueMatrixRead", *p1));
    if (p2) ladder.push_back(scan::Candidate::direct("Frustum_P2_PushChainMatrixRead", *p2, -3));
    if (p3) ladder.push_back(scan::Candidate::direct("Frustum_P3_MatrixReadBody", *p3, -0x1A));

    if (ladder.empty())
    {
        log().error("Camera hook: patterns failed to compile");
        return false;
    }

    // WHGame.dll, NOT the host launcher exe (KingdomCome.exe).
    DetourModKit::Region game_module{};
    for (int i = 0; i < 30 && game_module.size == 0; ++i)
    {
        game_module = DetourModKit::Region::module_named("WHGame.dll");
        if (game_module.size == 0)
            Sleep(100);
    }
    if (game_module.size == 0)
    {
        log().error("Camera hook: WHGame.dll not found in process after polling");
        return false;
    }

    scan::ScanRequest request = scan::borrow_code_target(ladder, "kcd2vr_frustum_hook", game_module);
    DetourModKit::Result<scan::Hit> resolved = scan::resolve(request);

    if (!resolved)
    {
        log().error("Camera hook: frustum-builder candidate ladder failed to resolve");
        return false;
    }

    log().info("Camera hook: frustum builder resolved via candidate '{}'", resolved->winning_name);

    hook::InlineRequest install_request{
        "kcd2vr_frustum_hook",
        hook::Target{resolved->address},
        hook::Options{}
    };

    DetourModKit::Result<hook::Hook> result = hook::inline_at(std::move(install_request), &CameraHook::FrustumDetour);
    if (!result)
    {
        log().error("Camera hook: inline_at failed to install");
        return false;
    }

    hook_ = std::move(*result);
    s_original = hook_->original<FrustumBuildFunc>();

    DetourModKit::Result<void> enable_result = hook_->enable();
    if (!enable_result)
    {
        log().error("Camera hook: failed to enable");
        s_original = nullptr;
        hook_.reset();
        return false;
    }

    log().info("Camera hook installed and enabled (frustum builder)");
    return true;
}

void CameraHook::Uninstall()
{
    if (hook_)
    {
        (void)hook_->disable();
        hook_.reset();
    }
    s_original = nullptr;
    s_pose_received.store(false, std::memory_order_relaxed);
}

void CameraHook::SetHeadPose(const XrPose& pose, float scale)
{
    SetHeadPoseForFrame(pose, scale, 0);
}

void CameraHook::SetHeadPoseForFrame(const XrPose& pose, float scale, std::uint64_t frame_index)
{
    std::lock_guard<std::mutex> lock(s_pose_mutex);
    s_current_pose = pose;
    if (frame_index != 0) s_current_pose_frame_index.store(frame_index, std::memory_order_release);
    s_current_scale.store(scale, std::memory_order_relaxed);

    if (!s_origin_valid.load(std::memory_order_relaxed) ||
        s_recenter_requested.exchange(false, std::memory_order_acq_rel))
    {
        s_origin_pose = pose;
        s_origin_valid.store(true, std::memory_order_release);
        log().info("Camera tracking origin captured/recentered");
    }

    s_pose_received.store(true, std::memory_order_release);
}

void CameraHook::SetInjectionMode(int mode)
{
    s_injection_mode.store(mode == 0 ? 0 : 1, std::memory_order_release);
}

int CameraHook::GetInjectionMode()
{
    return s_injection_mode.load(std::memory_order_acquire);
}

void CameraHook::SetPoseSourceMode(int mode, float prediction_offset_ms)
{
    if (mode < 0) mode=0; if (mode > 3) mode=3;
    s_pose_source_mode.store(mode,std::memory_order_release);
    if (!std::isfinite(prediction_offset_ms)) prediction_offset_ms=0.0f;
    if (prediction_offset_ms < -50.0f) prediction_offset_ms=-50.0f;
    if (prediction_offset_ms > 50.0f) prediction_offset_ms=50.0f;
    s_prediction_offset_ms.store(prediction_offset_ms,std::memory_order_release);
}

int CameraHook::GetPoseSourceMode() { return s_pose_source_mode.load(std::memory_order_acquire); }
void CameraHook::SetRotationOrder(int order) { s_rotation_order.store(order ? 1 : 0,std::memory_order_release); }
int CameraHook::GetRotationOrder() { return s_rotation_order.load(std::memory_order_acquire); }

void CameraHook::RequestRecenter()
{
    s_recenter_requested.store(true, std::memory_order_release);
}

void CameraHook::SetTrackingOptions(bool apply_rotation, bool apply_position)
{
    s_apply_rotation.store(apply_rotation, std::memory_order_relaxed);
    s_apply_position.store(apply_position, std::memory_order_relaxed);
}

void CameraHook::SetAfrStereoEnabled(bool enabled)
{
    s_afr_enabled.store(enabled, std::memory_order_release);
}

void CameraHook::SetAfrEye(int eye)
{
    s_afr_eye.store((eye & 1), std::memory_order_release);
}

int CameraHook::GetAfrEye()
{
    return s_afr_eye.load(std::memory_order_acquire) & 1;
}

void CameraHook::SetAfrIpd(float ipd_meters, float scale)
{
    if (std::isfinite(ipd_meters) && ipd_meters >= 0.040f && ipd_meters <= 0.090f)
        s_afr_ipd_meters.store(ipd_meters, std::memory_order_release);
    if (std::isfinite(scale) && scale >= -3.0f && scale <= 3.0f)
        s_afr_ipd_scale.store(scale, std::memory_order_release);
}

float CameraHook::GetAfrEyeOffsetMeters()
{
    if (!s_afr_enabled.load(std::memory_order_acquire)) return 0.0f;
    const int eye = s_afr_eye.load(std::memory_order_acquire) & 1;
    return (eye == 0 ? -0.5f : 0.5f) *
        s_afr_ipd_meters.load(std::memory_order_acquire) *
        s_afr_ipd_scale.load(std::memory_order_acquire);
}

void CameraHook::SetCameraCalibrationOffset(float side_meters, float back_meters, float height_meters)
{
    if (!std::isfinite(side_meters)) side_meters = 0.0f;
    if (!std::isfinite(back_meters)) back_meters = 0.0f;
    if (!std::isfinite(height_meters)) height_meters = 0.0f;
    side_meters = std::clamp(side_meters, -0.30f, 0.30f);
    back_meters = std::clamp(back_meters, -0.30f, 0.30f);
    height_meters = std::clamp(height_meters, -0.30f, 0.30f);
    s_camera_side_offset_m.store(side_meters, std::memory_order_release);
    s_camera_back_offset_m.store(back_meters, std::memory_order_release);
    s_camera_height_offset_m.store(height_meters, std::memory_order_release);
}

float CameraHook::GetCameraSideOffsetMeters()
{
    return s_camera_side_offset_m.load(std::memory_order_acquire);
}

float CameraHook::GetCameraBackOffsetMeters()
{
    return s_camera_back_offset_m.load(std::memory_order_acquire);
}

float CameraHook::GetCameraHeightOffsetMeters()
{
    return s_camera_height_offset_m.load(std::memory_order_acquire);
}

namespace {

constexpr ptrdiff_t kSViewParamsPositionOffset = 0x14;
constexpr ptrdiff_t kSViewParamsRotationOffset = 0x20;
constexpr ptrdiff_t kSViewParamsViewMatrixOffset = 0xE8;
constexpr std::uintptr_t kCViewVtableRvaA = 0x3A7A030ull;
constexpr std::uintptr_t kCViewVtableRvaB = 0x3A7A048ull;

struct Vec3f { float x, y, z; };
struct Quatf { float x, y, z, w; };

static Quatf Normalize(Quatf q)
{
    const float len2 = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
    if (len2 <= 1e-12f) return {0,0,0,1};
    const float inv = 1.0f / std::sqrt(len2);
    return {q.x*inv, q.y*inv, q.z*inv, q.w*inv};
}

static Quatf Conjugate(Quatf q) { return {-q.x, -q.y, -q.z, q.w}; }

static Quatf Mul(Quatf a, Quatf b)
{
    return {
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w,
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z
    };
}

static Quatf SlerpShortest(Quatf a, Quatf b, float t)
{
    a=Normalize(a); b=Normalize(b);
    float dot=a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;
    if(dot<0.0f){ b={-b.x,-b.y,-b.z,-b.w}; dot=-dot; }
    if(dot>0.9995f){ return Normalize({a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t,a.w+(b.w-a.w)*t}); }
    dot=std::clamp(dot,-1.0f,1.0f); const float th=std::acos(dot); const float st=std::sin(th);
    if(std::fabs(st)<1e-6f) return b;
    const float wa=std::sin((1.0f-t)*th)/st, wb=std::sin(t*th)/st;
    return Normalize({a.x*wa+b.x*wb,a.y*wa+b.y*wb,a.z*wa+b.z*wb,a.w*wa+b.w*wb});
}

static Quatf ApplyV55RotationFilter(Quatf target, int mode, std::uint64_t generation)
{
    // V55.2 crash isolation: raw/default mode is the pre-V55 path exactly.
    // Do not touch QPC or thread-local filter state until an F8 filter phase actually requests it.
    target = Normalize(target);
    if (mode == 0) return target;

    struct State { bool init=false; std::uint64_t gen=0; LARGE_INTEGER last{}; Quatf q{0,0,0,1}; };
    static thread_local State st{}; LARGE_INTEGER now{},freq{}; QueryPerformanceCounter(&now); QueryPerformanceFrequency(&freq);
    if(!st.init || st.gen!=generation){ st.init=true; st.gen=generation; st.last=now; st.q=Normalize(target); return st.q; }
    const double dt = freq.QuadPart>0 ? double(now.QuadPart-st.last.QuadPart)/double(freq.QuadPart) : 0.0;
    target=Normalize(target);
    // V56: the 45/60 Hz sample-and-hold experiments were subjectively worse.
    // Keep only continuous shortest-path quaternion smoothing around the useful 8 ms result.
    double tau = 0.0;
    if (mode==3) tau=0.004;
    else if (mode==4) tau=0.006;
    else if (mode==5) tau=0.008;
    else if (mode==6) tau=0.010;
    else if (mode==7) {
        // Adaptive: less smoothing on fast turns, more on tiny jitter.
        const Quatf aa=Normalize(st.q), bb=Normalize(target);
        float d=std::fabs(aa.x*bb.x+aa.y*bb.y+aa.z*bb.z+aa.w*bb.w);
        d=std::clamp(d,0.0f,1.0f);
        const double angle=2.0*std::acos(d);
        const double speed = dt>1e-5 ? angle/dt : 0.0; // rad/s
        const double fast = std::clamp(speed / 4.0, 0.0, 1.0);
        tau = 0.010 - fast * 0.007; // 10 ms stillness -> 3 ms fast turn
    }
    if (tau>0.0) {
        const float a=float(std::clamp(1.0-std::exp(-std::max(0.0,dt)/tau),0.0,1.0));
        st.q=SlerpShortest(st.q,target,a); st.last=now; return st.q;
    }
    return target;
}

static Vec3f Rotate(Quatf q, Vec3f v)
{
    const Quatf p{v.x, v.y, v.z, 0.0f};
    const Quatf r = Mul(Mul(q, p), Conjugate(q));
    return {r.x, r.y, r.z};
}

// Convert an OpenXR quaternion (+X right, +Y up, -Z forward) into the local
// CryEngine camera basis used by KCD2 (+X right, +Y forward, +Z up).
// This is the similarity transform C * R_xr * C^-1, implemented via basis vectors
// to avoid relying on a guessed component swap/sign trick.
static void XrQuatToCryMatrix(Quatf q, float out[3][3])
{
    q = Normalize(q);
    auto xr_rotate = [q](Vec3f v) { return Rotate(q, v); };
    auto cry_to_xr = [](Vec3f v) { return Vec3f{v.x, v.z, -v.y}; };
    auto xr_to_cry = [](Vec3f v) { return Vec3f{v.x, -v.z, v.y}; };

    const Vec3f cols[3] = {
        xr_to_cry(xr_rotate(cry_to_xr({1,0,0}))),
        xr_to_cry(xr_rotate(cry_to_xr({0,1,0}))),
        xr_to_cry(xr_rotate(cry_to_xr({0,0,1})))
    };
    for (int c = 0; c < 3; ++c) {
        out[0][c] = cols[c].x;
        out[1][c] = cols[c].y;
        out[2][c] = cols[c].z;
    }
}

static bool IsExactGameCView(uintptr_t camera)
{
    if (camera < static_cast<uintptr_t>(kSViewParamsViewMatrixOffset)) return false;
    const HMODULE module = GetModuleHandleA("WHGame.dll");
    if (!module) return false;
    const uintptr_t base = reinterpret_cast<uintptr_t>(module);
    const uintptr_t cview = camera - kSViewParamsViewMatrixOffset;
    const uintptr_t vtable = *reinterpret_cast<const uintptr_t*>(cview);
    if (vtable < base) return false;
    const uintptr_t rva = vtable - base;
    return rva == kCViewVtableRvaA || rva == kCViewVtableRvaB;
}

static bool ApplyTrackedPose(uintptr_t camera, const XrPose& pose, const XrPose& origin,
                             float world_scale, bool apply_rotation, bool apply_position,
                             int rotation_filter_mode, std::uint64_t rotation_filter_generation)
{
    // Exact RTTI-backed CView gate recovered from the user's retail WHGame.dll.
    // This replaces the old permissive "vtable lies somewhere inside WHGame" test.
    if (!IsExactGameCView(camera)) return false;

    const uintptr_t cview = camera - kSViewParamsViewMatrixOffset;
    auto* matrix = reinterpret_cast<Matrix34f*>(camera);
    const Vec3f base_pos = *reinterpret_cast<const Vec3f*>(cview + kSViewParamsPositionOffset);
    const Quatf base_q = Normalize(*reinterpret_cast<const Quatf*>(cview + kSViewParamsRotationOffset));

    const Quatf now_q{pose.qx, pose.qy, pose.qz, pose.qw};
    const Quatf origin_q{origin.qx, origin.qy, origin.qz, origin.qw};
    Quatf delta_xr = Normalize(Mul(Conjugate(Normalize(origin_q)), Normalize(now_q)));
    delta_xr = ApplyV55RotationFilter(delta_xr, rotation_filter_mode, rotation_filter_generation);

    float delta_local[3][3]{};
    XrQuatToCryMatrix(delta_xr, delta_local);

    // Matrix columns are the camera's local +X/+Y/+Z axes in world space.
    // Build them from the untouched CView eye quaternion so repeated frustum
    // rebuilds in the same frame do not accumulate our modification.
    const Vec3f base_right = Rotate(base_q, {1,0,0});
    const Vec3f base_forward = Rotate(base_q, {0,1,0});
    const Vec3f base_up = Rotate(base_q, {0,0,1});
    const Vec3f basis[3] = {base_right, base_forward, base_up};

    if (apply_rotation)
    {
        for (int c = 0; c < 3; ++c)
        {
            const Vec3f world{
                basis[0].x * delta_local[0][c] + basis[1].x * delta_local[1][c] + basis[2].x * delta_local[2][c],
                basis[0].y * delta_local[0][c] + basis[1].y * delta_local[1][c] + basis[2].y * delta_local[2][c],
                basis[0].z * delta_local[0][c] + basis[1].z * delta_local[1][c] + basis[2].z * delta_local[2][c]
            };
            matrix->m[0][c] = world.x;
            matrix->m[1][c] = world.y;
            matrix->m[2][c] = world.z;
        }
    }

    {
        // V60.8: tracked translation and fixed camera calibration are separate.
        // The fixed offset is intentionally applied even when positional tracking is disabled,
        // so it can test whether Henry/near-body scale is fundamentally a camera-distance issue.
        Vec3f cry_local{0.0f, 0.0f, 0.0f};
        if (apply_position)
        {
            const Vec3f xr_delta_world{
                pose.pos_x - origin.pos_x,
                pose.pos_y - origin.pos_y,
                pose.pos_z - origin.pos_z
            };
            const Vec3f xr_delta = Rotate(Conjugate(Normalize(origin_q)), xr_delta_world);
            cry_local = {xr_delta.x, -xr_delta.z, xr_delta.y};
        }
        cry_local.x += CameraHook::GetCameraSideOffsetMeters();
        cry_local.x += CameraHook::GetAfrEyeOffsetMeters();
        // Cry local +Y is camera forward and +Z is up. Positive menu 'back' therefore subtracts Y.
        cry_local.y -= CameraHook::GetCameraBackOffsetMeters();
        cry_local.z += CameraHook::GetCameraHeightOffsetMeters();
        const Vec3f world_delta{
            base_right.x * cry_local.x + base_forward.x * cry_local.y + base_up.x * cry_local.z,
            base_right.y * cry_local.x + base_forward.y * cry_local.y + base_up.y * cry_local.z,
            base_right.z * cry_local.x + base_forward.z * cry_local.y + base_up.z * cry_local.z
        };
        matrix->m[0][3] = base_pos.x + world_delta.x * world_scale;
        matrix->m[1][3] = base_pos.y + world_delta.y * world_scale;
        matrix->m[2][3] = base_pos.z + world_delta.z * world_scale;
    }

    return true;
}

} // namespace

// Isolated into its own function on purpose: MSVC's __try restriction (error
// C2712) applies to the WHOLE function that uses __try, not just the __try
// block - a lock_guard anywhere in FrustumDetour's frame, even textually
// before the __try, still triggers it (confirmed by hitting this exact
// error earlier - see ROADMAP.md). Keeping the mutex-guarded copy in a
// separate function keeps lock_guard's destructor in a different stack
// frame, which resolves it.
static void TraceCameraApplication() noexcept;

static void CopyCurrentPoseAndOrigin(std::mutex& mutex, const XrPose& src_pose, const XrPose& src_origin,
                                      std::atomic<float>& scale_atomic,
                                      XrPose& out_pose, XrPose& out_origin, float& out_scale)
{
    std::lock_guard<std::mutex> lock(mutex);
    out_pose = src_pose;
    out_origin = src_origin;
    out_scale = scale_atomic.load(std::memory_order_relaxed);
}

// V51: apply the current immutable XR snapshot directly to the CCamera matrix passed
// to PublishViewCamera. The caller backs up/restores the matrix around the engine copy,
// so KCD2's source camera remains untouched and the transform cannot accumulate.
bool CameraHook::ApplyCurrentPoseToPublishedCamera(void* camera, std::uint64_t* out_pose_frame, bool record_render_identity)
{
    if (!camera || !s_pose_received.load(std::memory_order_acquire) ||
        !s_origin_valid.load(std::memory_order_acquire)) return false;

    XrPose pose{}, origin{}; float world_scale=1.0f;
    CopyCurrentPoseAndOrigin(s_pose_mutex, s_current_pose, s_origin_pose, s_current_scale,
                             pose, origin, world_scale);
    const std::uint64_t pose_frame=s_current_pose_frame_index.load(std::memory_order_acquire);
    const int pose_source=s_pose_source_mode.load(std::memory_order_acquire);
    bool sync_ok=false; XrTime sampled_time=0;
    if (pose_source==1 || pose_source==3) {
        XrTime t=XrManager::Instance().GetLatestPredictedDisplayTime();
        if (pose_source==3) {
            const double off_ms=static_cast<double>(s_prediction_offset_ms.load(std::memory_order_acquire));
            t += static_cast<XrTime>(off_ms * 1000000.0);
        }
        sampled_time=t; sync_ok=XrManager::Instance().SampleHeadPoseAtTime(t,pose);
    } else if (pose_source==2) {
        sync_ok=XrManager::Instance().SampleHeadPoseNow(pose,&sampled_time);
    }
    if (pose_source!=0 && !sync_ok) {
        // Safe fallback keeps the test running while making failure visible in logs.
        static std::atomic<std::uint64_t> sync_fail{0};
        const auto n=sync_fail.fetch_add(1,std::memory_order_relaxed)+1;
        if (n<=8 || (n%300ull)==0) { char sm[192]{}; std::snprintf(sm,sizeof(sm),
            "V53-SYNC-POSE-FAIL n=%llu source=%d target=%lld fallback=CACHED",
            static_cast<unsigned long long>(n),pose_source,static_cast<long long>(sampled_time)); debug_trace::Write(sm); }
    }
    if (out_pose_frame) *out_pose_frame=pose_frame;

    __try {
        auto* matrix=reinterpret_cast<Matrix34f*>(camera);
        const Vec3f base_right{matrix->m[0][0],matrix->m[1][0],matrix->m[2][0]};
        const Vec3f base_forward{matrix->m[0][1],matrix->m[1][1],matrix->m[2][1]};
        const Vec3f base_up{matrix->m[0][2],matrix->m[1][2],matrix->m[2][2]};
        const Vec3f base_pos{matrix->m[0][3],matrix->m[1][3],matrix->m[2][3]};
        const Vec3f basis[3]={base_right,base_forward,base_up};

        const Quatf now_q{pose.qx,pose.qy,pose.qz,pose.qw};
        const Quatf origin_q{origin.qx,origin.qy,origin.qz,origin.qw};
        Quatf delta_xr=Normalize(Mul(Conjugate(Normalize(origin_q)),Normalize(now_q)));
        delta_xr=ApplyV55RotationFilter(delta_xr,s_rotation_filter_mode.load(std::memory_order_acquire),s_rotation_filter_generation.load(std::memory_order_acquire));
        float delta_local[3][3]{}; XrQuatToCryMatrix(delta_xr,delta_local);

        if (s_apply_rotation.load(std::memory_order_relaxed)) {
            const int order=s_rotation_order.load(std::memory_order_acquire);
            if (order==0) {
                // Local HMD rotation after the game's native/mouse camera: B * D.
                for(int c=0;c<3;++c){
                    const Vec3f world{
                        basis[0].x*delta_local[0][c]+basis[1].x*delta_local[1][c]+basis[2].x*delta_local[2][c],
                        basis[0].y*delta_local[0][c]+basis[1].y*delta_local[1][c]+basis[2].y*delta_local[2][c],
                        basis[0].z*delta_local[0][c]+basis[1].z*delta_local[1][c]+basis[2].z*delta_local[2][c]};
                    matrix->m[0][c]=world.x; matrix->m[1][c]=world.y; matrix->m[2][c]=world.z;
                }
            } else {
                // Diagnostic opposite composition: D * B in Cry world coordinates.
                float B[3][3]={{base_right.x,base_forward.x,base_up.x},
                               {base_right.y,base_forward.y,base_up.y},
                               {base_right.z,base_forward.z,base_up.z}};
                float O[3][3]{};
                for(int r=0;r<3;++r) for(int c=0;c<3;++c)
                    O[r][c]=delta_local[r][0]*B[0][c]+delta_local[r][1]*B[1][c]+delta_local[r][2]*B[2][c];
                for(int r=0;r<3;++r) for(int c=0;c<3;++c) matrix->m[r][c]=O[r][c];
            }
        }
        {
            Vec3f cry_local{0.0f,0.0f,0.0f};
            if (s_apply_position.load(std::memory_order_relaxed)) {
                const Vec3f xr_world{pose.pos_x-origin.pos_x,pose.pos_y-origin.pos_y,pose.pos_z-origin.pos_z};
                const Vec3f xr_delta=Rotate(Conjugate(Normalize(origin_q)),xr_world);
                cry_local={xr_delta.x,-xr_delta.z,xr_delta.y};
            }
            cry_local.x += GetCameraSideOffsetMeters();
            cry_local.x += GetAfrEyeOffsetMeters();
            cry_local.y -= GetCameraBackOffsetMeters();
            cry_local.z += GetCameraHeightOffsetMeters();
            const Vec3f wd{
                base_right.x*cry_local.x+base_forward.x*cry_local.y+base_up.x*cry_local.z,
                base_right.y*cry_local.x+base_forward.y*cry_local.y+base_up.y*cry_local.z,
                base_right.z*cry_local.x+base_forward.z*cry_local.y+base_up.z*cry_local.z};
            matrix->m[0][3]=base_pos.x+wd.x*world_scale;
            matrix->m[1][3]=base_pos.y+wd.y*world_scale;
            matrix->m[2][3]=base_pos.z+wd.z*world_scale;
        }
        // V61.44 FRAME IDENTITY FIX: the V61.12 culling path uses this same
        // matrix helper temporarily, then restores the CCamera. That temporary
        // apply must NOT overwrite the published-render pose identity used to
        // tag the final PRESENT. Otherwise a culling-only pose can be paired
        // with pixels rendered from a different PublishViewCamera pose.
        if (!record_render_identity) return true;

        s_last_applied_pose_frame_index.store(pose_frame,std::memory_order_release);
        // V53: quantify angular cadence at the exact publish application point.
        {
            static thread_local bool have_prev=false;
            static thread_local Quatf prev_hmd{0,0,0,1};
            static thread_local float prev_base[3][3]{};
            static thread_local float prev_final[3][3]{};
            static thread_local std::uint64_t diag_n=0;
            auto mat_delta_deg=[](const float A[3][3], const float Bm[3][3])->float {
                float tr=0.0f; for(int c=0;c<3;++c) for(int r=0;r<3;++r) tr += A[r][c]*Bm[r][c];
                float cs=(tr-1.0f)*0.5f; if(cs<-1.0f)cs=-1.0f; if(cs>1.0f)cs=1.0f; return std::acos(cs)*57.2957795f;
            };
            const Quatf hq=Normalize(now_q);
            float baseM[3][3]={{base_right.x,base_forward.x,base_up.x},{base_right.y,base_forward.y,base_up.y},{base_right.z,base_forward.z,base_up.z}};
            float finalM[3][3]={{matrix->m[0][0],matrix->m[0][1],matrix->m[0][2]},{matrix->m[1][0],matrix->m[1][1],matrix->m[1][2]},{matrix->m[2][0],matrix->m[2][1],matrix->m[2][2]}};
            ++diag_n;
            if(have_prev) {
                float dot=std::fabs(prev_hmd.x*hq.x+prev_hmd.y*hq.y+prev_hmd.z*hq.z+prev_hmd.w*hq.w); if(dot>1.0f)dot=1.0f;
                const float hmdDeg=2.0f*std::acos(dot)*57.2957795f;
                const float baseDeg=mat_delta_deg(prev_base,baseM); const float finalDeg=mat_delta_deg(prev_final,finalM);
                // V53 review finding: sampling only diag_n<=20 or every 30th
                // sample means ~97% of frames were never logged over a 10s
                // phase (~1000 frames at ~100fps -> ~50 logged) - easily
                // enough to miss a rare, transient spike, which is exactly
                // what this diagnostic exists to catch. Track worst-case
                // hmdDeg/finalDeg divergence EVERY frame (cheap - just a few
                // float compares, no snprintf/file I/O) in cross-thread-safe
                // atomics (mod.cpp's F8 phase state machine runs on a
                // different thread than this detour), reset at each phase
                // start and read+logged at phase end, so an outlier is never
                // silently missed even at the existing sparse per-frame log
                // rate.
                const float gapDeg = std::fabs(finalDeg - hmdDeg);
                if (gapDeg > s_worst_gap_deg.load(std::memory_order_relaxed)) {
                    s_worst_gap_deg.store(gapDeg, std::memory_order_relaxed);
                    s_worst_hmd_deg.store(hmdDeg, std::memory_order_relaxed);
                    s_worst_final_deg.store(finalDeg, std::memory_order_relaxed);
                    s_worst_n.store(diag_n, std::memory_order_relaxed);
                }
                if(diag_n<=20 || (diag_n%30ull)==0) {
                    char am[320]{}; std::snprintf(am,sizeof(am),
                        "V53-ANGULAR n=%llu source=%d order=%d sync=%s hmdDeg=%.4f baseDeg=%.4f finalDeg=%.4f poseFrame=%llu",
                        static_cast<unsigned long long>(diag_n),pose_source,s_rotation_order.load(std::memory_order_acquire),sync_ok?"yes":"cached",
                        hmdDeg,baseDeg,finalDeg,static_cast<unsigned long long>(pose_frame)); debug_trace::Write(am);
                }
            }
            prev_hmd=hq; std::memcpy(prev_base,baseM,sizeof(baseM)); std::memcpy(prev_final,finalM,sizeof(finalM)); have_prev=true;
        }
        TraceCameraApplication();
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}




namespace {

struct CensusCallerSlot
{
    std::atomic<std::uintptr_t> caller_rva{0};
    std::atomic<std::uint64_t> count{0};
    std::atomic<std::uintptr_t> sample_cview{0};
    std::atomic<std::uintptr_t> sample_vtable_rva{0};
};

struct CensusViewSlot
{
    std::atomic<std::uintptr_t> cview{0};
    std::atomic<std::uint64_t> count{0};
    std::atomic<std::uintptr_t> vtable_rva{0};
    std::atomic<std::uintptr_t> first_caller_rva{0};
};

static void CensusTracef(const char* fmt, ...) noexcept
{
    char buf[1024]{};
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    debug_trace::Write(buf);
}

CensusCallerSlot g_census_callers[48]{};
CensusViewSlot g_census_views[48]{};
std::atomic<std::uint64_t> g_census_total{0};
std::atomic<std::uint64_t> g_census_exact_cview_total{0};
std::atomic<std::uint64_t> g_census_non_cview_total{0};

static bool IsExactCViewVtable(std::uintptr_t vtable_rva) noexcept
{
    return vtable_rva == kCViewVtableRvaA || vtable_rva == kCViewVtableRvaB;
}

static void CensusCameraCall(std::uintptr_t camera, std::uintptr_t return_address) noexcept
{
    if (camera < static_cast<std::uintptr_t>(kSViewParamsViewMatrixOffset))
        return;

    const HMODULE wh = GetModuleHandleA("WHGame.dll");
    if (!wh) return;
    const auto base = reinterpret_cast<std::uintptr_t>(wh);

    const auto cview = camera - static_cast<std::uintptr_t>(kSViewParamsViewMatrixOffset);
    const auto vtable = *reinterpret_cast<const std::uintptr_t*>(cview);
    const auto vtable_rva = (vtable >= base) ? (vtable - base) : 0;
    const auto caller_rva = (return_address >= base) ? (return_address - base) : 0;
    const bool exact = IsExactCViewVtable(vtable_rva);

    const auto total = g_census_total.fetch_add(1, std::memory_order_relaxed) + 1;
    if (exact)
        g_census_exact_cview_total.fetch_add(1, std::memory_order_relaxed);
    else
        g_census_non_cview_total.fetch_add(1, std::memory_order_relaxed);

    for (auto& slot : g_census_callers)
    {
        auto key = slot.caller_rva.load(std::memory_order_relaxed);
        if (key == caller_rva && key != 0)
        {
            slot.count.fetch_add(1, std::memory_order_relaxed);
            break;
        }
        if (key == 0)
        {
            std::uintptr_t expected = 0;
            if (slot.caller_rva.compare_exchange_strong(expected, caller_rva, std::memory_order_relaxed))
            {
                slot.count.store(1, std::memory_order_relaxed);
                slot.sample_cview.store(cview, std::memory_order_relaxed);
                slot.sample_vtable_rva.store(vtable_rva, std::memory_order_relaxed);
                CensusTracef(
                    "CVIEW-CENSUS new-caller caller_rva=0x%llX cview=%p vtable_rva=0x%llX exact_cview=%s tid=%lu",
                    static_cast<unsigned long long>(caller_rva),
                    reinterpret_cast<void*>(cview),
                    static_cast<unsigned long long>(vtable_rva),
                    exact ? "true" : "false",
                    static_cast<unsigned long>(GetCurrentThreadId()));
                break;
            }
        }
    }

    if (exact)
    {
        for (auto& slot : g_census_views)
        {
            auto key = slot.cview.load(std::memory_order_relaxed);
            if (key == cview && key != 0)
            {
                const auto n = slot.count.fetch_add(1, std::memory_order_relaxed) + 1;
                if (n == 1000 || n == 10000)
                    CensusTracef("CVIEW-CENSUS view-milestone cview=%p count=%llu first_caller_rva=0x%llX",
                        reinterpret_cast<void*>(cview),
                        static_cast<unsigned long long>(n),
                        static_cast<unsigned long long>(slot.first_caller_rva.load(std::memory_order_relaxed)));
                break;
            }
            if (key == 0)
            {
                std::uintptr_t expected = 0;
                if (slot.cview.compare_exchange_strong(expected, cview, std::memory_order_relaxed))
                {
                    slot.count.store(1, std::memory_order_relaxed);
                    slot.vtable_rva.store(vtable_rva, std::memory_order_relaxed);
                    slot.first_caller_rva.store(caller_rva, std::memory_order_relaxed);
                    const auto* pos = reinterpret_cast<const Vec3f*>(cview + kSViewParamsPositionOffset);
                    CensusTracef(
                        "CVIEW-CENSUS NEW-EXACT-CVIEW cview=%p vtable_rva=0x%llX caller_rva=0x%llX pos=(%.3f,%.3f,%.3f) tid=%lu",
                        reinterpret_cast<void*>(cview),
                        static_cast<unsigned long long>(vtable_rva),
                        static_cast<unsigned long long>(caller_rva),
                        pos->x, pos->y, pos->z,
                        static_cast<unsigned long>(GetCurrentThreadId()));
                    break;
                }
            }
        }
    }

    if (total == 1 || total == 10000 || total == 50000 || (total % 100000ull) == 0)
    {
        CensusTracef(
            "CVIEW-CENSUS totals calls=%llu exact_cview_calls=%llu non_cview_calls=%llu",
            static_cast<unsigned long long>(total),
            static_cast<unsigned long long>(g_census_exact_cview_total.load(std::memory_order_relaxed)),
            static_cast<unsigned long long>(g_census_non_cview_total.load(std::memory_order_relaxed)));
    }
}

} // namespace

static std::atomic<std::uint64_t> g_last_camera_apply_tick_ms{0};

// V61.7 CULL-TIMING: a tiny seqlock snapshot of the exact camera orientation
// immediately before CCamera::UpdateFrustumPlanes consumes it.  This is
// observer-only and is intentionally independent of the old cull/FOV widening.
static std::atomic<std::uint64_t> g_frustum_rotation_seq{0};
static CameraRotationSample g_frustum_rotation_sample{};

// V61.44 exact FULL culling-pose latch is separate from the generic rotation
// observer above. CaptureFrustumRotationSample() is also called for stock/aux
// frusta; those must never overwrite the matrix eligible for PublishViewCamera.
static std::atomic<std::uint64_t> g_frustum_pose_seq{0};
static std::atomic<std::uint64_t> g_frustum_pose_consumed_seq{0};
static CameraRotationSample g_frustum_posed_sample{};
static Matrix34f g_frustum_posed_matrix{};

static void CaptureFrustumRotationSample(std::uintptr_t camera) noexcept
{
    if (!camera) return;
    CameraRotationSample sample{};
    bool ok=false;
    __try {
        const auto* matrix=reinterpret_cast<const Matrix34f*>(camera);
        sample.m[0]=matrix->m[0][0]; sample.m[1]=matrix->m[0][1]; sample.m[2]=matrix->m[0][2];
        sample.m[3]=matrix->m[1][0]; sample.m[4]=matrix->m[1][1]; sample.m[5]=matrix->m[1][2];
        sample.m[6]=matrix->m[2][0]; sample.m[7]=matrix->m[2][1]; sample.m[8]=matrix->m[2][2];
        ok=true;
    } __except(EXCEPTION_EXECUTE_HANDLER) { ok=false; }
    if (!ok) return;
    sample.pose_frame=CameraHook::CurrentPoseFrameIndex();
    sample.tick_ms=static_cast<std::uint64_t>(GetTickCount64());
    sample.camera=camera;
    const auto begin=g_frustum_rotation_seq.fetch_add(1,std::memory_order_acq_rel)+1; // odd
    (void)begin;
    g_frustum_rotation_sample=sample;
    g_frustum_rotation_seq.fetch_add(1,std::memory_order_release); // even
}

static void CaptureFrustumPosedMatrix(std::uintptr_t camera) noexcept
{
    if(!camera) return;
    CameraRotationSample sample{};
    Matrix34f full{};
    bool ok=false;
    __try {
        full=*reinterpret_cast<const Matrix34f*>(camera);
        sample.m[0]=full.m[0][0]; sample.m[1]=full.m[0][1]; sample.m[2]=full.m[0][2];
        sample.m[3]=full.m[1][0]; sample.m[4]=full.m[1][1]; sample.m[5]=full.m[1][2];
        sample.m[6]=full.m[2][0]; sample.m[7]=full.m[2][1]; sample.m[8]=full.m[2][2];
        ok=true;
    } __except(EXCEPTION_EXECUTE_HANDLER) { ok=false; }
    if(!ok) return;
    sample.pose_frame=CameraHook::CurrentPoseFrameIndex();
    sample.tick_ms=static_cast<std::uint64_t>(GetTickCount64());
    sample.camera=camera;
    g_frustum_pose_seq.fetch_add(1,std::memory_order_acq_rel); // odd
    g_frustum_posed_sample=sample;
    g_frustum_posed_matrix=full;
    g_frustum_pose_seq.fetch_add(1,std::memory_order_release); // even
}

bool CameraHook::GetLatestFrustumRotationSample(CameraRotationSample& out)
{
    for (int attempt=0; attempt<4; ++attempt) {
        const auto a=g_frustum_rotation_seq.load(std::memory_order_acquire);
        if (a==0 || (a&1ull)) continue;
        const CameraRotationSample local=g_frustum_rotation_sample;
        const auto b=g_frustum_rotation_seq.load(std::memory_order_acquire);
        if (a==b && !(b&1ull)) { out=local; return local.camera!=0; }
    }
    return false;
}

static void TraceCameraApplication() noexcept
{
    g_last_camera_apply_tick_ms.store(static_cast<std::uint64_t>(GetTickCount64()), std::memory_order_release);
    static std::atomic<std::uint64_t> count{0};
    const auto value = count.fetch_add(1, std::memory_order_relaxed) + 1;
    if (value == 1)
        debug_trace::Write("first tracked pose applied by camera frustum hook");
    if ((value % 600) == 0)
        debug_trace::WriteCounter("camera_pose_applications", value);
}

bool CameraHook::ApplyLatestCullingPoseToPublishedCamera(void* camera, std::uint64_t* out_pose_frame, std::uint64_t max_age_ms)
{
    if(!camera) return false;
    CameraRotationSample sample{};
    Matrix34f full{};
    bool got=false;
    std::uint64_t stable_seq=0;
    for(int attempt=0; attempt<4; ++attempt) {
        const auto a=g_frustum_pose_seq.load(std::memory_order_acquire);
        if(a==0 || (a&1ull)) continue;
        sample=g_frustum_posed_sample;
        full=g_frustum_posed_matrix;
        const auto b=g_frustum_pose_seq.load(std::memory_order_acquire);
        if(a==b && !(b&1ull)) { got=true; stable_seq=b; break; }
    }
    if(!got || sample.camera!=reinterpret_cast<std::uintptr_t>(camera) || sample.pose_frame==0) return false;
    if(stable_seq==0 || (stable_seq&1ull) || stable_seq==g_frustum_pose_consumed_seq.load(std::memory_order_acquire)) return false;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const auto age=now>=sample.tick_ms ? now-sample.tick_ms : UINT64_MAX;
    if(age>max_age_ms) return false;
    bool ok=false;
    __try { *reinterpret_cast<Matrix34f*>(camera)=full; ok=true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { ok=false; }
    if(!ok) return false;
    if(out_pose_frame) *out_pose_frame=sample.pose_frame;
    g_frustum_pose_consumed_seq.store(stable_seq,std::memory_order_release);
    s_last_applied_pose_frame_index.store(sample.pose_frame,std::memory_order_release);
    TraceCameraApplication();
    return true;
}

std::uint64_t CameraHook::LastPoseApplyTickMs()
{
    return g_last_camera_apply_tick_ms.load(std::memory_order_acquire);
}

std::uint64_t CameraHook::LastPoseFrameIndex()
{
    return s_last_applied_pose_frame_index.load(std::memory_order_acquire);
}

std::uint64_t CameraHook::CurrentPoseFrameIndex()
{
    return s_current_pose_frame_index.load(std::memory_order_acquire);
}

float CameraHook::LatestGameplayFovRadians()
{
    return s_gameplay_fov_rad.load(std::memory_order_acquire);
}

float CameraHook::LatestGameplayFovDegrees()
{
    return LatestGameplayFovRadians() * 57.29577951308232f;
}

std::uint64_t CameraHook::GameplayFovSampleCount()
{
    return s_gameplay_fov_samples.load(std::memory_order_acquire);
}

uintptr_t __fastcall CameraHook::FrustumDetour(uintptr_t camera) noexcept
{
    XrPose pose{};
    XrPose origin{};
    float world_scale = 1.0f;
    const bool have_pose = s_pose_received.load(std::memory_order_acquire) &&
                           s_origin_valid.load(std::memory_order_acquire);

    if (have_pose)
    {
        CopyCurrentPoseAndOrigin(s_pose_mutex, s_current_pose, s_origin_pose, s_current_scale,
                                  pose, origin, world_scale);
    }

    // V48: xrWaitFrame may be 72/90 Hz while KCD2 builds its camera much faster.
    // Refresh the head pose near camera consumption, but rate-limit duplicate
    // frustum calls so one game frame does not spam the runtime.
    if (have_pose && XrManager::Instance().GetLatePoseEnabled()) {
        static std::atomic<std::uint64_t> last_late_tick{0};
        const std::uint64_t now_tick = static_cast<std::uint64_t>(GetTickCount64());
        std::uint64_t previous = last_late_tick.load(std::memory_order_relaxed);
        if (now_tick >= previous + 2 && last_late_tick.compare_exchange_strong(previous, now_tick, std::memory_order_acq_rel)) {
            XrPose late{};
            if (XrManager::Instance().SampleLateHeadPose(late)) pose = late;
        }
    }

    __try
    {
        // Census every frustum call before modifying anything. _ReturnAddress()
        // identifies the engine call site; the census also validates exact CView RTTI vtables.
        // V42 lean runtime: retired broad CView census is disabled by default.
        // It generated tens of thousands of calls and is no longer needed for the renderer-camera proof.
        (void)camera;

        // TPVCamera applies its camera change BEFORE UpdateFrustumPlanes so the
        // cull planes are built from the modified matrix. Do the same here.
        if (camera != 0 && have_pose && s_injection_mode.load(std::memory_order_acquire) == 0)
        {
            const bool applied = ApplyTrackedPose(camera, pose, origin, world_scale,
                                                  s_apply_rotation.load(std::memory_order_relaxed),
                                                  s_apply_position.load(std::memory_order_relaxed),
                                                  s_rotation_filter_mode.load(std::memory_order_acquire),
                                                  s_rotation_filter_generation.load(std::memory_order_acquire));
            if (applied) {
                s_last_applied_pose_frame_index.store(s_current_pose_frame_index.load(std::memory_order_acquire), std::memory_order_release);
                TraceCameraApplication();
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // Fail closed. The game's original frustum builder still runs below.
    }

    if (camera == 0 || !s_original)
        return 0;

    // V61.28: recover the ACTUAL symmetric source VFOV from the exact gameplay
    // CView camera. Earlier PublishViewCamera calibration mixed inventory/dialogue/
    // auxiliary cameras. V61.12 already solved the identity problem, so reuse that
    // gate here and only READ CCamera+0x30. No FOV/cull field is changed.
    bool exact_game_fov_view = false;
    __try { exact_game_fov_view = IsExactGameCView(camera); }
    __except (EXCEPTION_EXECUTE_HANDLER) { exact_game_fov_view = false; }
    if (exact_game_fov_view) {
        float fov_rad = 0.0f;
        bool valid = false;
        __try {
            fov_rad = *reinterpret_cast<const float*>(camera + 0x30);
            valid = std::isfinite(fov_rad) && fov_rad > 0.50f && fov_rad < 2.70f;
        } __except (EXCEPTION_EXECUTE_HANDLER) { valid = false; }
        if (valid) {
            s_gameplay_fov_rad.store(fov_rad, std::memory_order_release);
            const auto fn = s_gameplay_fov_samples.fetch_add(1, std::memory_order_relaxed) + 1;
            if (fn <= 8 || (fn % 900ull) == 0) {
                char fm[256]{};
                std::snprintf(fm, sizeof(fm),
                    "V61.28-GAMEPLAY-FOV sample=%llu exactCView=yes camera=%p actualVFOV=%.3fdeg rad=%.6f readOnly=true",
                    static_cast<unsigned long long>(fn), reinterpret_cast<void*>(camera),
                    static_cast<double>(fov_rad * 57.29577951308232f), static_cast<double>(fov_rad));
                debug_trace::Write(fm);
            }
        }
    }

    // V61.8 CULL-POSE-SYNC TEST: mode 1 normally applies XR rotation later at
    // PublishViewCamera. The V61.7 correlation log proved the frustum matrix can
    // disagree by many degrees even with the same pose-frame tag. For this test,
    // temporarily apply the same current XR pose only while UpdateFrustumPlanes runs,
    // then restore the exact 3x4 camera matrix. No FOV/cullScale widening is used.
    if (have_pose && s_injection_mode.load(std::memory_order_acquire) == 1) {
        // V61.12: V61.8 proved the reversible frustum pose-sync fixes gameplay
        // culling, but it was too broad and also touched inventory/character/RTT
        // cameras. Gate the sync to the exact gameplay CView camera only.
        bool exact_game_view=false;
        __try { exact_game_view=IsExactGameCView(camera); }
        __except(EXCEPTION_EXECUTE_HANDLER) { exact_game_view=false; }
        if (exact_game_view) {
            Matrix34f backup{}; bool backed=false; bool applied=false;
            __try { backup=*reinterpret_cast<Matrix34f*>(camera); backed=true; }
            __except(EXCEPTION_EXECUTE_HANDLER) { backed=false; }
            if (backed) applied=CameraHook::ApplyCurrentPoseToPublishedCamera(
                reinterpret_cast<void*>(camera), nullptr, false /* culling only: never retag rendered PRESENT */);
            if (applied) {
                CaptureFrustumRotationSample(camera);
                CaptureFrustumPosedMatrix(camera);
                const uintptr_t result=s_original(camera);
                __try { *reinterpret_cast<Matrix34f*>(camera)=backup; }
                __except(EXCEPTION_EXECUTE_HANDLER) {}
                static std::atomic<std::uint64_t> n{0}; const auto c=n.fetch_add(1,std::memory_order_relaxed)+1;
                if(c<=8 || (c%900ull)==0) debug_trace::Write("V61.12-CULL-SYNC gameplay CView only; matrix restored; FOV untouched");
                return result;
            }
        } else {
            static std::atomic<std::uint64_t> skipped{0}; const auto c=skipped.fetch_add(1,std::memory_order_relaxed)+1;
            if(c<=4 || (c%6000ull)==0) debug_trace::Write("V61.12-CULL-SKIP auxiliary/non-gameplay camera left untouched");
        }
    }

    // V60.9 KCD2TOOLS CULL-SYNC TEST.
    // KCD2Tools TPVCamera documents SetFrustum's coupled CCamera fields:
    // projection FOV +0x30 and cull terms +0x50/+0x58/+0x60/+0x68/+0x70.
    // V60.4 changed +0x30 alone, which left these already-built cull terms inconsistent.
    // Here we temporarily widen all six fields coherently, call the original builder, then restore every field.
    // V61.4: failed TPVCamera-style persistent FOV/cull widening is retired.
    // Keep the setting parse-compatible, but never alter CCamera projection/cull fields.
    const float cull_scale = 1.0f;
    if (cull_scale <= 1.0001f) {
        CaptureFrustumRotationSample(camera);
        return s_original(camera);
    }

    // KCD2Tools' current TPVCamera implementation gates this exact manipulation
    // to cameras embedded in the gameplay CView. We already have the retail
    // CView RTTI/vtable gate recovered for this build, so do the same and leave
    // shadow/reflection/portal cameras untouched.
    //
    // V61.1 crash fix: IsExactGameCView() dereferences `camera` (a raw vtable
    // read) with no SEH protection at this call site. During a save-game
    // load, KCD2 tears down and recreates CView/CCamera objects, so the
    // engine can call this frustum builder with a momentarily-dangling
    // camera pointer. Every other camera/cview dereference in this file
    // (including this exact same IsExactGameCView call inside
    // ApplyTrackedPose above, and the cull-term reads just below) is wrapped
    // in __try/__except; this one was not, which produced an unhandled AV
    // inside KCD2VR.asi (BugSplat MFA: KCD2VR!00087994) right after loading
    // a save. Fail closed, same as everywhere else in this function.
    bool exact_view = false;
    __try { exact_view = IsExactGameCView(camera); }
    __except (EXCEPTION_EXECUTE_HANDLER) { exact_view = false; }
    if (!exact_view)
        return s_original(camera);

    constexpr std::uintptr_t kProjectionFovOffset = 0x30;
    constexpr std::uintptr_t kCull0Offset = 0x50;
    constexpr std::uintptr_t kCull1Offset = 0x58;
    constexpr std::uintptr_t kCullTanOffset = 0x60;
    constexpr std::uintptr_t kCull3Offset = 0x68;
    constexpr std::uintptr_t kCull4Offset = 0x70;
    float original_fov=0.0f, widened_fov=0.0f, ratio=1.0f;
    float c0=0.0f,c1=0.0f,ct=0.0f,c3=0.0f,c4=0.0f;
    bool wrote=false;
    __try {
        original_fov=*reinterpret_cast<float*>(camera+kProjectionFovOffset);
        c0=*reinterpret_cast<float*>(camera+kCull0Offset);
        c1=*reinterpret_cast<float*>(camera+kCull1Offset);
        ct=*reinterpret_cast<float*>(camera+kCullTanOffset);
        c3=*reinterpret_cast<float*>(camera+kCull3Offset);
        c4=*reinterpret_cast<float*>(camera+kCull4Offset);
        const bool plausible=std::isfinite(original_fov)&&original_fov>0.35f&&original_fov<2.95f&&
            std::isfinite(c0)&&std::isfinite(c1)&&std::isfinite(ct)&&std::isfinite(c3)&&std::isfinite(c4)&&std::fabs(ct)>1e-6f;
        if(plausible){
            const float old_tan=std::tan(original_fov*0.5f);
            widened_fov=std::clamp(2.0f*std::atan(old_tan*cull_scale),original_fov,3.05f);
            const float new_tan=std::tan(widened_fov*0.5f);
            ratio=(std::fabs(old_tan)>1e-6f)?(new_tan/old_tan):1.0f;
            *reinterpret_cast<float*>(camera+kProjectionFovOffset)=widened_fov;
            *reinterpret_cast<float*>(camera+kCull0Offset)=c0*ratio;
            *reinterpret_cast<float*>(camera+kCull1Offset)=c1*ratio;
            *reinterpret_cast<float*>(camera+kCullTanOffset)=ct/ratio;
            *reinterpret_cast<float*>(camera+kCull3Offset)=c3*ratio;
            *reinterpret_cast<float*>(camera+kCull4Offset)=c4*ratio;
            wrote=true;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER){ wrote=false; }

    // V61.1: KCD2Tools-faithful behavior.  Do NOT restore the FOV/cull tuple
    // after UpdateFrustumPlanes. SetFrustum refreshes these camera fields on the
    // next camera setup; restoring here made the widened planes transient and
    // defeated downstream render/cull consumers.
    const uintptr_t result=s_original(camera);

    if(wrote){
        static std::atomic<std::uint64_t> calls{0};
        const auto n=calls.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=12 || (n%3000ull)==0){
            char msg[512]{};
            std::snprintf(msg,sizeof(msg),
                "V60.9-CULL-SYNC exactCView=yes call=%llu camera=%p fov=%.2f->%.2f scale=%.2f ratio=%.4f terms={%.4f %.4f %.4f %.4f %.4f} persistent=true",
                static_cast<unsigned long long>(n),reinterpret_cast<void*>(camera),
                static_cast<double>(original_fov*57.29577951308232),static_cast<double>(widened_fov*57.29577951308232),
                static_cast<double>(cull_scale),static_cast<double>(ratio),
                static_cast<double>(c0),static_cast<double>(c1),static_cast<double>(ct),static_cast<double>(c3),static_cast<double>(c4));
            debug_trace::Write(msg);
        }
    } else {
        static std::atomic<std::uint64_t> bad{0};
        const auto n=bad.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=6) debug_trace::Write("V60.9-CULL-SYNC skipped: CCamera FOV/cull tuple not plausible/readable");
    }
    return result;

}

} // namespace kcd2vr