#include "vrik/dice_body_d81.h"
#include "diagnostics/log_sink_d48.h"
// camera_hook.cpp
//
// Candidate ladder for CCamera::UpdateFrustumPlanes reused, with attribution,
// from tkhquang/KCD2Tools (TPVCamera mod, MIT licensed), whose aob_resolver.hpp
// documents these three signatures as verified against the live retail
// WHGame.dll (P1 = full prologue, P2 = mid-prologue walk-back -3, P3 = body
// matrix-read run walk-back -0x1A).

#include "camera_hook.h"
#include <chrono>
#include "xr_manager.h"
#include "tracking_sample59c6.h"
#include "debug_trace.h"
#include "diagnostic_trace.h"
#include "frik_pose_probe.h"
#include "activity_lens59c10r1.h"
#include "dialogue_smooth38.h"
#include "core/snapshot_mailbox.h"
#include "diagnostics/capture168.h"
#include "vrik/motion_trials_d67.h"
#include "vrik/tracking_origin125.h"
#include "vrik/roomscale58.h"
#include "subsystems/roomscale/settings_d76.h"
#include "subsystems/roomscale/coordinate_d76.h"
#include "vrik/workstation178r20c1.h"
#include "diagnostics/xr_timing165.h"
#include "vrik/hand_same_frame_hj1.h"
#include "vrik/visual_selection59c7f.h"
#include "subsystems/experimental/free_targeting_d42.h" // R133 FREE PLUS view hold
#include "vrik/riding_vrik_d76.h" // R133: never hold on a horse (the horse turns with the left stick)

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

namespace {
static void RoomscaleXrSample47(const XrPose&,const XrPose&);
// D76R1: restore the read-only D75 head-name sidecar. The pose lifetime is
// copied under s_pose_mutex; one publication contains both origin and direction.
static std::uint64_t g_current_pose_source_epoch_d75=0;
static core::SnapshotMailbox<visual_ray_d75::Sample> g_visual_ray_d75;
}
std::mutex CameraHook::s_pose_mutex;
XrPose CameraHook::s_current_pose{};
hj1::Sample CameraHook::s_current_family_r24{};
XrPose CameraHook::s_origin_pose{};
std::atomic<float> CameraHook::s_current_scale{1.0f};
std::atomic<std::uint64_t> CameraHook::s_current_pose_frame_index{0};
std::atomic<std::uint64_t> CameraHook::s_last_applied_pose_frame_index{0};
std::atomic<bool> CameraHook::s_pose_received{false};
std::atomic<bool> CameraHook::s_origin_valid{false};
std::atomic<bool> CameraHook::s_recenter_requested{false};
std::atomic<bool> CameraHook::s_apply_rotation{true};
std::atomic<bool> CameraHook::s_native_head_rotation_owner{false};
void CameraHook::SetNativeHeadRotationOwner(bool active){s_native_head_rotation_owner.store(active,std::memory_order_release);}
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
std::atomic<int> CameraHook::s_view_marriage_mode{0};
std::atomic<bool> CameraHook::s_fresh_publish_pose{false};
std::atomic<float> CameraHook::s_view_marriage_native_gain{1.0f};
std::atomic<int> CameraHook::s_rotation_filter_mode{0};
std::atomic<float> CameraHook::s_head_aim_local_yaw_rad{0.0f};
std::atomic<float> CameraHook::s_head_aim_local_pitch_rad{0.0f};
std::atomic<bool> CameraHook::s_head_aim_local_valid{false};

namespace {
// Recenter seats the tracked origin at the native camera, plus explicit user
// offsets. It must not preserve or accumulate previous roomscale displacement.
std::atomic<float> g_recenter_anchor_x{0.0f};
std::atomic<float> g_recenter_anchor_y{0.0f};
std::atomic<float> g_recenter_anchor_z{0.0f};
// V66.178R58 roomscale follow: the anchor is the distance the ENTITY has already walked under the player
// (camera-local, x right / y forward), subtracted from the HMD offset so the view never slides. Written on
// the render-identity apply only; read by the pad injection through CameraHook::RoomscaleRemaining.
std::atomic<bool> g_roomscale_enabled58{false};
kcd2vr::roomscale58::State g_roomscale58{};
std::mutex g_roomscale_mutex58;
std::atomic<float> g_roomscale_rem_x58{0.f},g_roomscale_rem_y58{0.f};std::atomic<std::uint64_t> g_roomscale_tick58{0};
bool g_roomscale_have_base58=false;float g_roomscale_base58[3]{};
std::atomic<bool> g_angular_diagnostics_enabled{false};
static XrPose YawOnlyOrigin(const XrPose& pose,const XrPose& previous) noexcept
{
    const auto reference=tracking125::recenter({pose.pos_x,pose.pos_y,pose.pos_z},
        {pose.qx,pose.qy,pose.qz,pose.qw},{previous.qx,previous.qy,previous.qz,previous.qw});
    XrPose out=pose;
    out.pos_x=float(reference.position.x);out.pos_y=float(reference.position.y);out.pos_z=float(reference.position.z);
    out.qx=float(reference.yaw.x);out.qy=float(reference.yaw.y);out.qz=float(reference.yaw.z);out.qw=float(reference.yaw.w);
    return out;
}
}
std::atomic<float> CameraHook::s_head_aim_transferred_yaw_rad{0.0f};
std::atomic<float> CameraHook::s_head_aim_transferred_pitch_rad{0.0f};
std::atomic<int> CameraHook::s_dialogue_lab_mode{-1};
std::atomic<bool> CameraHook::s_dialogue_lab_active{false};
std::atomic<int> CameraHook::s_dialogue_native_camera_type{0};
std::atomic<std::uint64_t> CameraHook::s_rotation_filter_generation{1};
std::atomic<float> CameraHook::s_worst_gap_deg{0.0f};
std::atomic<float> CameraHook::s_worst_hmd_deg{0.0f};
std::atomic<float> CameraHook::s_worst_final_deg{0.0f};
std::atomic<std::uint64_t> CameraHook::s_worst_n{0};
std::atomic<float> CameraHook::s_gameplay_fov_rad{0.0f};
std::atomic<std::uint64_t> CameraHook::s_gameplay_fov_samples{0};
std::atomic<float> CameraHook::s_visual_forward_x{0.0f};
std::atomic<float> CameraHook::s_visual_forward_y{1.0f};
std::atomic<float> CameraHook::s_visual_forward_z{0.0f};
std::atomic<std::uint64_t> CameraHook::s_visual_forward_tick_ms{0};
std::atomic<bool> CameraHook::s_visual_forward_valid{false};
std::atomic<float> CameraHook::s_visual_position_x{0.0f};
std::atomic<float> CameraHook::s_visual_position_y{0.0f};
std::atomic<float> CameraHook::s_visual_position_z{0.0f};
std::atomic<std::uint64_t> CameraHook::s_visual_position_tick_ms{0};
std::atomic<bool> CameraHook::s_visual_position_valid{false};
static std::atomic<bool> g_history_persistent_pose{false};

namespace {
std::mutex g_dialogue_base_mutex;
float g_dialogue_pre_base[12]{};
bool g_dialogue_pre_base_valid=false;
std::atomic<std::uint64_t> g_dialogue_takeover_frames{0};
std::atomic<std::uint64_t> g_dialogue_last_log_ms{0};
}

void CameraHook::SetDialogueLabState(int mode, bool dialogue_active, int native_camera_type)
{
    s_dialogue_lab_mode.store((mode>=0&&mode<=3)?mode:-1,std::memory_order_release);
    s_dialogue_lab_active.store(dialogue_active,std::memory_order_release);
    s_dialogue_native_camera_type.store((native_camera_type>=0&&native_camera_type<=4)?native_camera_type:0,std::memory_order_release);
}


static std::atomic<float> s_native_pitch161{0.0f};
static std::atomic<std::uint64_t> s_native_pitch_tick161{0};
bool CameraHook::GetNativeCameraPitch(float& pitch_rad, std::uint64_t max_age_ms)
{
    const auto t=s_native_pitch_tick161.load(std::memory_order_acquire);const auto now=GetTickCount64();
    if(!t||now<t||now-t>max_age_ms)return false;
    pitch_rad=s_native_pitch161.load(std::memory_order_relaxed);return std::isfinite(pitch_rad);
}

bool CameraHook::GetHeadAimLocalAngles(float& yaw_rad, float& pitch_rad)
{
    if (!s_head_aim_local_valid.load(std::memory_order_acquire)) return false;
    yaw_rad=s_head_aim_local_yaw_rad.load(std::memory_order_acquire);
    pitch_rad=s_head_aim_local_pitch_rad.load(std::memory_order_acquire);
    return std::isfinite(yaw_rad)&&std::isfinite(pitch_rad);
}


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

// V66.59 TRANSIENT CULL WIDENING (F3). Widen the gameplay CView camera's FOV (+0x30) and the
// coupled cull terms (+0x50/+0x58/+0x60/+0x68/+0x70, KCD2Tools TPVCamera layout) ONLY while
// the frustum builder runs, then restore them. The frustum planes (culling, incl. shadow
// casters) become wider; the projection the eyes are rendered with is unchanged.
// (V60.9 kept the widened FOV persistently, which also changed the rendered image.)
static std::atomic<std::uint64_t> g_v6659_widened{0};
static std::uintptr_t V6659CallWidened(std::uintptr_t camera, float scale, std::uintptr_t (*original)(std::uintptr_t)) noexcept
{
    constexpr std::uintptr_t kFov=0x30, k0=0x50, k1=0x58, kT=0x60, k3=0x68, k4=0x70;
    float fov=0,c0=0,c1=0,ct=0,c3=0,c4=0; bool wrote=false;
    if(scale>1.0001f) {
        __try {
            fov=*reinterpret_cast<float*>(camera+kFov); c0=*reinterpret_cast<float*>(camera+k0);
            c1=*reinterpret_cast<float*>(camera+k1); ct=*reinterpret_cast<float*>(camera+kT);
            c3=*reinterpret_cast<float*>(camera+k3); c4=*reinterpret_cast<float*>(camera+k4);
            const bool ok=std::isfinite(fov)&&fov>0.35f&&fov<2.95f&&std::isfinite(c0)&&std::isfinite(c1)&&
                std::isfinite(ct)&&std::isfinite(c3)&&std::isfinite(c4)&&std::fabs(ct)>1e-6f;
            if(ok) {
                const float ot=std::tan(fov*0.5f);
                const float nf=std::clamp(2.0f*std::atan(ot*scale),fov,3.05f);
                const float r=std::tan(nf*0.5f)/ot;
                *reinterpret_cast<float*>(camera+kFov)=nf;
                *reinterpret_cast<float*>(camera+k0)=c0*r; *reinterpret_cast<float*>(camera+k1)=c1*r;
                *reinterpret_cast<float*>(camera+kT)=ct/r; *reinterpret_cast<float*>(camera+k3)=c3*r;
                *reinterpret_cast<float*>(camera+k4)=c4*r;
                wrote=true;
            }
        } __except(EXCEPTION_EXECUTE_HANDLER) { wrote=false; }
    }
    const std::uintptr_t result=original(camera);
    if(wrote) {
        __try {
            *reinterpret_cast<float*>(camera+kFov)=fov; *reinterpret_cast<float*>(camera+k0)=c0;
            *reinterpret_cast<float*>(camera+k1)=c1; *reinterpret_cast<float*>(camera+kT)=ct;
            *reinterpret_cast<float*>(camera+k3)=c3; *reinterpret_cast<float*>(camera+k4)=c4;
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
        g_v6659_widened.fetch_add(1,std::memory_order_relaxed);
    }
    return result;
}

void CameraHook::SetCullFrustumScale(float scale)
{
    s_cull_frustum_scale.store(std::clamp(scale, 1.0f, 1.60f), std::memory_order_release);
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

CameraHook::FrustumBuildFunc CameraHook::s_original = nullptr;

namespace {

// CryEngine 3x4 matrix: 3 rows x 4 columns, row-major. Column 3 is
// translation. The camera's matrix sits at OFFSET 0 of the pointer
// the hook receives directly.
struct Matrix34f
{
    float m[3][4];
};

static void PublishVisualRayD75(const Matrix34f& matrix,std::uint64_t sourceEpoch,
    std::uint64_t cameraGeneration,std::uint64_t poseFrame,const float* displayOrigin=nullptr,float worldScaleR92=1.f) noexcept {
    // An older camera worker must not relabel its matrix after a recenter or
    // tracking reset. No render matrix, camera latch or tracking state is changed.
    if(roomscale_d76::recenterPending.load(std::memory_order_acquire)||!sourceEpoch||sourceEpoch!=tracking59c6::epoch.load(std::memory_order_acquire)||
       !cameraGeneration||cameraGeneration!=CameraHook::CurrentFamilyGenerationR24())return;
    const auto sample=visual_ray_d75::make(matrix.m,std::uint64_t(GetTickCount64()),sourceEpoch,cameraGeneration,poseFrame,displayOrigin,worldScaleR92);
    if(!roomscale_d76::recenterPending.load(std::memory_order_acquire)&&
       sourceEpoch==tracking59c6::epoch.load(std::memory_order_acquire)&&
       cameraGeneration==CameraHook::CurrentFamilyGenerationR24())(void)g_visual_ray_d75.publish(sample);
}

namespace ds=dialogue_state59c10r2;
namespace ae19=activity_eye59c19;
core::SnapshotMailbox<ae19::Neutral> g_activity_neutral59c19;
core::SnapshotMailbox<ae19::Anchor> g_activity_anchor59c19;
std::atomic_flag g_activity_cache_writer43=ATOMIC_FLAG_INIT;
bool ActivityEyeBase59c19(Matrix34f& matrix,bool render,native_roomscale_d76::Origin* roomOriginD76=nullptr,std::uint64_t roomRevisionD76=0) noexcept {
    // Cull and publish may overlap. Serialize the small cache transaction
    // before taking any state/head snapshot; a busy attempt keeps its native
    // base. Never block a camera thread or let an old clear erase a new latch.
    if(g_activity_cache_writer43.test_and_set(std::memory_order_acquire))return false;
    struct Unlock43 {~Unlock43(){g_activity_cache_writer43.clear(std::memory_order_release);}} unlock43;
    const auto now=std::uint64_t(GetTickCount64());
    const auto state=activity_lens59c10r1::EyeState(now);
    // FREE VRIK consumes its exact completed C51 body below. Do not first
    // replace the native eye with the separate passenger anchor and then skip
    // that consumer. A missing/protected C51 frame retains the native fallback.
    workstation178r20c1::WorkstationOwnerH51 workstationOwner{};
    if(workstation178r20c1::workstationOwnerH51(now,workstationOwner)){
        g_activity_anchor59c19.publish({});dice_body_d81::retire();return false;
    }
    if(ae19::nativeAlchemy63(state,now)){
        // Preserve the exact current native eye, including the authored reach
        // to the book. Normal HMD rotation/recenter/IPD still compose below.
        // No old anchor can survive this native-owned alchemy interval.
        g_activity_anchor59c19.publish({});
        const ds::V native{matrix.m[0][3],matrix.m[1][3],matrix.m[2][3]};
        activity_lens59c10r1::NoteEye59c19({now,state,ae19::Reason::NativeAlchemy,native,native,{},false,false,!render});
        return false;
    }
    if(ae19::nativeSharpeningR92(state,now)){
        // Keep this callback's seated native eye, not the cached standing
        // anchor. No camera replay, smoothing, FOV or HMD rotation change.
        g_activity_anchor59c19.publish({});dice_body_d81::retire();
        const ds::V native{matrix.m[0][3],matrix.m[1][3],matrix.m[2][3]};
        activity_lens59c10r1::NoteEye59c19({now,state,ae19::Reason::NativeSharpening,native,native,{},false,false,!render});
        return false;
    }
    ds::NativeHead head{};const bool haveHead=frik_pose_probe::NativeDialogueHead59c10r2(head,now);
    ae19::Neutral neutral{};ae19::Anchor anchor{};
    (void)g_activity_neutral59c19.read(neutral);(void)g_activity_anchor59c19.read(anchor);
    ds::Eye native{};native.position={matrix.m[0][3],matrix.m[1][3],matrix.m[2][3]};
    for(unsigned i=0;i<3;++i)native.basis[i]={matrix.m[0][i],matrix.m[1][i],matrix.m[2][i]};
    // The ordinary publish route reuses the exact composed culling matrix.
    // Learn/latch from that pre-HMD base too: there may be no later render=true
    // call. Existing native owner, body, freshness and lifecycle gates remain.
    if(haveHead&&ae19::learn(state,head,native,now,neutral))g_activity_neutral59c19.publish(neutral);
    if(ae19::fresh(state,now)&&!state.active){anchor={};g_activity_anchor59c19.publish(anchor);}
    if(haveHead&&ae19::latch(state,head,neutral,now,anchor)==ae19::Reason::Applied)
        g_activity_anchor59c19.publish(anchor);
    // No world-space replay when ownership/body publication is missing. An
    // anatomical model-local anchor can survive recenter, but applying it still
    // requires the CURRENT qualified body; unavailable is not permission.
    ds::Eye eye{};const auto reason=ae19::resolveCurrent(state,head,haveHead,anchor,now,eye);
    if(haveHead&&roomOriginD76&&roomRevisionD76&&!roomscale_d76::compatible(*roomOriginD76,head.roomOriginD76,roomRevisionD76)){
        dice_body_d81::retire();return false;
    }
    const bool applied=reason==ae19::Reason::Applied;
    dice_body_d81::publish(state,head,anchor,now,applied);
    if(applied&&roomOriginD76)*roomOriginD76=head.roomOriginD76;
    if(applied){
        matrix.m[0][3]=eye.position.x;matrix.m[1][3]=eye.position.y;matrix.m[2][3]=eye.position.z;
        for(unsigned i=0;i<3;++i){matrix.m[0][i]=eye.basis[i].x;matrix.m[1][i]=eye.basis[i].y;matrix.m[2][i]=eye.basis[i].z;}
    }
    // Attempt receipt only: a cull can be discarded without being rendered.
    if(state.active)activity_lens59c10r1::NoteEye59c19({now,state,reason,native.position,
        applied?eye.position:native.position,head.world,haveHead,neutral.ready,!render});
    return applied;
}
namespace dsm38=dialogue_smooth38;
// C38: steady high-resolution time for the dialogue anchor low-pass (GetTickCount64 is 15.6 ms coarse).
std::uint64_t DialogueSmoothNowNs38() noexcept {return std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());}
core::SnapshotMailbox<ds::Policy> g_dialogue_policy59c10r2;
core::SnapshotMailbox<ds::EyeOffset> g_dialogue_eye59c10r2;
struct DialogueReceipt59c10r2 {std::uint64_t tick=0;unsigned reason=0;ds::State state{};bool head=false,learned=false,allowed=false;
    bool defaultEye=false,fromCull=false,headFromView28=false;std::uintptr_t headCharacter28=0;
    ds::V headWorld{},nativeView{},eye{};
    bool smooth38=false,held38=false;unsigned heldCause38=0;std::uint32_t holds38=0,snaps38=0;float lagM38=0;}; // A camera attempt, not a claim that this cull was published.
core::SnapshotMailbox<DialogueReceipt59c10r2> g_dialogue_receipt59c10r2;
std::atomic<std::uint64_t> g_dialogue_applied_tick59c13{0};
std::atomic<bool> g_dialogue_default_eye59c13{true};
core::SnapshotMailbox<dsm38::State> g_dialogue_smooth38; // C38 anchor low-pass + late-sample hold (visual base only)
std::atomic<bool> g_dialogue_smooth_on38{true}; // INI/menu VrDialogueSmoothCamera; false = exact C37C2 per-sample base
bool DialogueEyeBase59c10r2(Matrix34f& matrix,bool render,std::uint64_t roomRevisionD76=0,const native_roomscale_d76::Origin* canonicalD76=nullptr) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    const auto state=activity_lens59c10r1::Dialogue(now);
    ds::NativeHead head{};ds::EyeOffset learned{};ds::Policy policy{};
    const bool headFromView28=frik_pose_probe::NativeDialogueViewHead28(head,state,now);
    const bool haveHead=headFromView28||frik_pose_probe::NativeDialogueHead59c10r2(head,now);
    if(haveHead&&canonicalD76&&roomRevisionD76&&!roomscale_d76::compatible(*canonicalD76,head.roomOriginD76,roomRevisionD76))return false;
    (void)g_dialogue_eye59c10r2.read(learned);(void)g_dialogue_policy59c10r2.read(policy);
    ds::Eye native{};native.position={matrix.m[0][3],matrix.m[1][3],matrix.m[2][3]};
    for(unsigned i=0;i<3;++i)native.basis[i]={matrix.m[0][i],matrix.m[1][i],matrix.m[2][i]};
    // A loaded/replaced Henry character can enter dialogue before a neutral
    // gameplay eye is available. A ready calibration for another body must not
    // prevent the existing opted-in default eye from serving this fresh body.
    // Unknown/mismatched owners and missing head data never retire calibration;
    // recenter alone preserves it because the same body's identity is stable.
    if(haveHead&&ds::fresh(head,now)&&ds::fresh(state,now)&&state.entity==head.entity&&learned.ready&&
       (learned.entity!=head.entity||learned.character!=head.character)){
        learned=ds::EyeOffset{};g_dialogue_eye59c10r2.publish(learned);
    }
    const auto activityState19=activity_lens59c10r1::EyeState(now);
    // An activity may use the FirstPerson camera while moving its eye. Such a
    // sample must never become the neutral dialogue eye calibration.
    // Both callers supply the untouched native base, before the ONE HMD/IPD
    // composition below. The common render route reuses this exact culling
    // matrix, so render-only learning silently missed ordinary gameplay.
    // POD publication does not advance rendered-frame identity or query native/XR.
    if(haveHead&&activityState19.known&&!activityState19.active&&frik_pose_probe::BowCameraNeutral35(now)&&
       ds::learn(state,head,native,now,learned))g_dialogue_eye59c10r2.publish(learned);
    ds::Eye eye{};bool applied=haveHead&&ds::reconstruct(state,head,learned,policy,now,eye);bool viaDefault=false;
    // R59.16C10R13 (Miki: "in dialogues we replace the Henry actor with us"): when no first-person eye offset was learned yet, the
    // view still comes from Henry's own head - the measured gameplay relationship (V66.127: the camera sits ~5.6 cm above the head
    // bone, looking along the body) stands in. Never the dialogue camera entity.
    if(!applied&&haveHead&&!learned.ready&&g_dialogue_default_eye59c13.load(std::memory_order_relaxed)){
        ds::EyeOffset def{};def.entity=head.entity;def.character=head.character;def.local={0.f,0.f,0.056f};
        def.basis[0]={1,0,0};def.basis[1]={0,1,0};def.basis[2]={0,0,1};def.ready=true;
        applied=ds::reconstruct(state,head,def,policy,now,eye);viaDefault=applied;}
    const unsigned rawReason=applied?0u:!state.known?1u:!state.active?2u:!ds::embodiedOwner(state,now)?3u:
        !policy.enabled||!ds::age(policy.tick,now,80)?4u:!haveHead?5u:!learned.ready?6u:7u;
    // C38: the native anchor arrives at game/animation rate and sometimes late; follow it with a short
    // time-based low-pass and bridge a LATE sample of the same active dialogue with the last smoothed eye.
    // The HMD is still composed once afterwards on every publication. OFF = exact C37C2 behaviour.
    const bool smooth38=g_dialogue_smooth_on38.load(std::memory_order_relaxed);bool held38=false;dsm38::State sm38{};
    if(smooth38){(void)g_dialogue_smooth38.read(sm38);const dsm38::Config c38{};
        if(applied){(void)dsm38::follow(sm38,eye,head.entity,head.character,state.instance,DialogueSmoothNowNs38(),now,c38,eye);(void)g_dialogue_smooth38.publish(sm38);}
        else if((rawReason==4u||rawReason==5u||rawReason==3u)&&dsm38::hold(sm38,state,policy,now,c38,eye)){held38=applied=true;(void)g_dialogue_smooth38.publish(sm38);}}
    if(applied)g_dialogue_applied_tick59c13.store(now,std::memory_order_release);
    if(applied){
        matrix.m[0][3]=eye.position.x;matrix.m[1][3]=eye.position.y;matrix.m[2][3]=eye.position.z;
        for(unsigned i=0;i<3;++i){matrix.m[0][i]=eye.basis[i].x;matrix.m[1][i]=eye.basis[i].y;matrix.m[2][i]=eye.basis[i].z;}
    }
    // POD publication only. Formatting/file IO stay on the control thread.
    // Include the culling attempt: a later publish may reuse that composed
    // matrix without invoking this function again. This does not assert that
    // the culling matrix was consumed; the receipt explicitly names its stage.
    if(state.active||applied){
        DialogueReceipt59c10r2 r{};r.tick=now;r.reason=held38?8u:rawReason;
        r.smooth38=smooth38;r.held38=held38;r.heldCause38=held38?rawReason:0u;r.holds38=sm38.holds;r.snaps38=sm38.snaps;r.lagM38=sm38.lagM;
        r.state=state;r.head=haveHead;r.learned=learned.ready;r.allowed=policy.enabled;r.defaultEye=viaDefault;r.fromCull=!render;
        r.headFromView28=headFromView28;r.headCharacter28=haveHead?head.character:0;
        r.headWorld=head.world;r.nativeView=native.position;r.eye=applied?eye.position:native.position;
        g_dialogue_receipt59c10r2.publish(r);}
    return applied;
}

// Keep mutex RAII outside functions containing MSVC __try blocks (C2712).
void StoreDialoguePreBase(const Matrix34f& src) noexcept
{
    std::lock_guard<std::mutex> lk(g_dialogue_base_mutex);
    int k=0; for(int r=0;r<3;++r) for(int c=0;c<4;++c) g_dialogue_pre_base[k++]=src.m[r][c];
    g_dialogue_pre_base_valid=true;
}

bool LoadDialoguePreBase(Matrix34f& dst) noexcept
{
    std::lock_guard<std::mutex> lk(g_dialogue_base_mutex);
    if(!g_dialogue_pre_base_valid) return false;
    int k=0; for(int r=0;r<3;++r) for(int c=0;c<4;++c) dst.m[r][c]=g_dialogue_pre_base[k++];
    return true;
}

} // namespace

void CameraHook::SetDialogueEmbodiment59c10r2(bool allowed,std::uint64_t now) {
    g_dialogue_policy59c10r2.publish({now,allowed});
}
bool CameraHook::DialogueEmbodimentApplied59c13(std::uint64_t now) {
    const auto t=g_dialogue_applied_tick59c13.load(std::memory_order_acquire);return t&&now>=t&&now-t<=120;
}
void CameraHook::SetDialogueDefaultEye59c13(bool on) {g_dialogue_default_eye59c13.store(on,std::memory_order_relaxed);}
void CameraHook::SetDialogueSmooth38(bool on) {g_dialogue_smooth_on38.store(on,std::memory_order_relaxed);}
bool CameraHook::DialogueSmooth38() {return g_dialogue_smooth_on38.load(std::memory_order_relaxed);}
void CameraHook::ReportDialogueEmbodiment59c10r2(std::uint64_t now) {
    // R59.16C10R13: no capture-domain gate any more - every dialogue writes this line on entry, on every change of its outcome and
    // every 2 s while it lasts (the 14 Oct review: no log ever showed the camera-vs-head distance inside a dialogue).
    static std::uint64_t last=0;static unsigned lastReason=~0u;static bool lastActive=false;
    DialogueReceipt59c10r2 r{};const bool have=g_dialogue_receipt59c10r2.read(r)&&ds::age(r.tick,now,200);
    const bool activeNow=have&&r.state.active;
    if(!activeNow&&!lastActive)return;
    const unsigned reasonClass=r.reason==8u?0u:r.reason; // C38: a bridged late sample is still the embodied eye (counted, not a change)
    const bool changed=activeNow!=lastActive||(have&&reasonClass!=lastReason);
    if(!changed&&now>=last&&now-last<2000)return;
    last=now;lastActive=activeNow;lastReason=have?reasonClass:~0u;
    if(!activeNow){diagnostic_trace::Write("R59.16C10R13 DIALOGUE_VIEW active=0 (dialogue ended; gameplay eye)");return;}
    static const char* kReason[]{"EMBODIED","WITNESS_UNKNOWN","NOT_ACTIVE","CAMERA_OWNER_NOT_FP_OR_DIALOG(bone/ui/rider/external)","NOT_ALLOWED(policy: VRIK/mode2/UI/activity gate)","NO_FRESH_HEAD(skinning head >60ms or epoch)","NO_LEARNED_OFFSET","RECONSTRUCT_INVALID","HELD_LATE_SAMPLE"};
    const float dx=r.eye.x-r.headWorld.x,dy=r.eye.y-r.headWorld.y,dz=r.eye.z-r.headWorld.z,nx=r.nativeView.x-r.headWorld.x,ny=r.nativeView.y-r.headWorld.y,nz=r.nativeView.z-r.headWorld.z;
    const float ex=r.eye.x-r.nativeView.x,ey=r.eye.y-r.nativeView.y,ez=r.eye.z-r.nativeView.z;
    char line[800]{};std::snprintf(line,sizeof(line),
        "R59.16C10R13 DIALOGUE_VIEW active=1 camera=%s eye=%s%s head=%d learned=%d allowed=%d headWorld=(%.2f,%.2f,%.2f) nativeView=(%.2f,%.2f,%.2f) nativeToHeadM=%.2f viewToHeadM=%.2f entity=%p instance=%p stage=%s"
        " C38{smooth=%d heldCause=%s holds=%u snaps=%u lagCm=%.1f eyeToNativeM=%.2f}",
        r.state.camera==ds::Camera::FirstPerson?"FIRST_PERSON":r.state.camera==ds::Camera::Dialogue?"C_CameraDialog":"OTHER",kReason[r.reason<9?r.reason:7],r.defaultEye?"(DEFAULT_EYE +5.6cm)":"",
        r.head,r.learned,r.allowed,double(r.headWorld.x),double(r.headWorld.y),double(r.headWorld.z),double(r.nativeView.x),double(r.nativeView.y),double(r.nativeView.z),
        double(std::sqrt(nx*nx+ny*ny+nz*nz)),double(std::sqrt(dx*dx+dy*dy+dz*dz)),reinterpret_cast<void*>(r.state.entity),reinterpret_cast<void*>(r.state.instance),r.fromCull?"CULL_ATTEMPT":"PUBLISH_ATTEMPT",
        r.smooth38?1:0,r.held38?kReason[r.heldCause38<8?r.heldCause38:7]:"-",unsigned(r.holds38),unsigned(r.snaps38),double(r.lagM38*100.f),double(std::sqrt(ex*ex+ey*ey+ez*ez)));
    diagnostic_trace::Write(line);
    using namespace diagnostics::capture168;
    if(active(Domain::Hud,now)||active(Domain::Interaction,now)){
        const auto body=frik_pose_probe::DialogueVisualSnapshot28(r.headCharacter28,now);
        char detail[700]{};std::snprintf(detail,sizeof(detail),
            "C28 HUD_DIALOGUE_BODY entity=%p instance=%p headSource=%s headCharacter=%p bodyAdmitted=%d gameplay=%d nativeBody=%d skinCharacter=%p skinAgeMs=%llu frameCharacter=%p frameAgeMs=%llu frameSample=%llu poseValid=%d frameFreshSameBody=%d handsRL=%d/%d epoch=%llu snapshotOnly=1",
            reinterpret_cast<void*>(r.state.entity),reinterpret_cast<void*>(r.state.instance),
            r.headFromView28?"NATIVE_VIEW":r.head?"SKINNING":"NONE",reinterpret_cast<void*>(r.headCharacter28),
            body.bodyAdmitted,body.gameplay,body.nativeBody,reinterpret_cast<void*>(body.skinCharacter),
            (unsigned long long)(body.skinTick&&now>=body.skinTick?now-body.skinTick:~0ull),
            reinterpret_cast<void*>(body.frameCharacter),
            (unsigned long long)(body.frameTick&&now>=body.frameTick?now-body.frameTick:~0ull),
            (unsigned long long)body.sample,body.poseValid,body.frameFresh,body.right,body.left,(unsigned long long)body.epoch);
        diagnostic_trace::Write(detail);
    }
}

void CameraHook::SetAngularDiagnosticsEnabled(bool enabled)
{
    g_angular_diagnostics_enabled.store(enabled,std::memory_order_release);
    if(!enabled) ResetWorstCaseAngular();
    debug_trace::Write(enabled?"V65.03-F2 angular diagnostics ON":"V65.03-F2 angular diagnostics OFF");
}


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
        KCD2VR_DIAG_LOG(Bug,error,"Camera hook: patterns failed to compile");
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
        KCD2VR_DIAG_LOG(Bug,error,"Camera hook: WHGame.dll not found in process after polling");
        return false;
    }

    scan::ScanRequest request = scan::borrow_code_target(ladder, "kcd2vr_frustum_hook", game_module);
    DetourModKit::Result<scan::Hit> resolved = scan::resolve(request);

    if (!resolved)
    {
        KCD2VR_DIAG_LOG(Bug,error,"Camera hook: frustum-builder candidate ladder failed to resolve");
        return false;
    }

    KCD2VR_DIAG_LOG(Bug,info,"Camera hook: frustum builder resolved via candidate '{}'", resolved->winning_name);

    hook::InlineRequest install_request{
        "kcd2vr_frustum_hook",
        hook::Target{resolved->address},
        hook::Options{}
    };

    DetourModKit::Result<hook::Hook> result = hook::inline_at(std::move(install_request), &CameraHook::FrustumDetour);
    if (!result)
    {
        KCD2VR_DIAG_LOG(Bug,error,"Camera hook: inline_at failed to install");
        return false;
    }

    hook_ = std::move(*result);
    s_original = hook_->original<FrustumBuildFunc>();

    DetourModKit::Result<void> enable_result = hook_->enable();
    if (!enable_result)
    {
        KCD2VR_DIAG_LOG(Bug,error,"Camera hook: failed to enable");
        s_original = nullptr;
        hook_.reset();
        return false;
    }

    KCD2VR_DIAG_LOG(Bug,info,"Camera hook installed and enabled (frustum builder)");
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

bool CameraHook::ReadTrackingOrigin125(tracking125::OriginVisitorH46 visitor,void* context) noexcept {
    std::lock_guard<std::mutex> lock(s_pose_mutex);
    if(!visitor||!s_origin_valid.load(std::memory_order_acquire))return false;
    const tracking125::Origin out={{s_origin_pose.pos_x,s_origin_pose.pos_y,s_origin_pose.pos_z},
         {s_origin_pose.qx,s_origin_pose.qy,s_origin_pose.qz,s_origin_pose.qw}};
    return visitor(out,context);
}

void CameraHook::SetHeadPose(const XrPose& pose, float scale)
{
    SetHeadPoseForFrame(pose, scale, 0);
}

// Additional consistency check on the already copied producer receipt;
// equality alone never creates provenance.
static bool CameraFamilyMatchesPoseR24(const hj1::Sample& family,const XrPose& pose) noexcept {
    return family.head.p[0]==pose.pos_x&&family.head.p[1]==pose.pos_y&&family.head.p[2]==pose.pos_z&&
        family.head.q[0]==pose.qx&&family.head.q[1]==pose.qy&&family.head.q[2]==pose.qz&&family.head.q[3]==pose.qw;
}
static bool CameraFamilyCurrentR24(const hj1::Sample& family) noexcept {
    return tracking59c6::CameraFamilyCurrentR24(family,hand_smooth_h50::policyEpoch.load(std::memory_order_acquire),tracking59c6::NowNs());
}
std::uint64_t CameraHook::CurrentFamilyGenerationR24() noexcept {
    return s_rotation_filter_generation.load(std::memory_order_acquire);
}

static std::atomic<double> g_follow_consumed_x_d76{0},g_follow_consumed_y_d76{0};

void CameraHook::SetHeadPoseForFrame(const XrPose& pose, float scale, std::uint64_t frame_index,const hj1::Sample* family)
{
    std::lock_guard<std::mutex> lock(s_pose_mutex);
    const auto sourceEpochD75=tracking59c6::epoch.load(std::memory_order_acquire);
    s_current_pose = pose;
    if (frame_index != 0) s_current_pose_frame_index.store(frame_index, std::memory_order_release);
    s_current_scale.store(scale, std::memory_order_relaxed);

    const bool had_origin=s_origin_valid.load(std::memory_order_relaxed);
    const bool recenter=s_recenter_requested.exchange(false,std::memory_order_acq_rel);
    const bool physicalOriginD76=native_roomscale_d76::Enrolled();
    bool commitOriginD76=!had_origin||recenter;
    const auto nextOriginD76=roomscale_d76::originRevision.load(std::memory_order_acquire)+(had_origin&&recenter?1ull:0ull);
    if(commitOriginD76&&physicalOriginD76){
        roomscale_d76::recenterPending.store(true,std::memory_order_release);
        if(!native_roomscale_d76::TryRecenterOrigin(nextOriginD76)){
            s_recenter_requested.store(true,std::memory_order_release);commitOriginD76=false;
        }
    }
    if (commitOriginD76)
    {
        g_recenter_anchor_x.store(0.0f,std::memory_order_release);
        g_recenter_anchor_y.store(0.0f,std::memory_order_release);
        g_recenter_anchor_z.store(0.0f,std::memory_order_release);
        {std::lock_guard<std::mutex> rl(g_roomscale_mutex58);kcd2vr::roomscale58::reset(g_roomscale58);g_roomscale_have_base58=false;g_roomscale_tick58.store(0,std::memory_order_release);} // R58: a recenter seats the entity under the player again
        s_origin_pose = YawOnlyOrigin(pose,s_origin_pose);
        // C43 (VrCalibrationAutoRestore): the view height is the calibrated one. The origin is the headset corrected by
        // (current eye height - F8 standing eye height), so a recenter taken slouched/leaning (19:53:57 in the 17 Oct log:
        // currentEye=1.031 standingEye=1.128) or at session start does not move Henry's eyes 10 cm for the whole session.
        // The VRIK body base gets the same correction (calibration_runtime.inl BODY_REANCHOR). 0 without floor tracking.
        const float shift43=frik_pose_probe::StandingEyeShift43();
        s_origin_pose.pos_y-=shift43;
        roomscale_d76::originRevision.store(nextOriginD76,std::memory_order_release);
        if(physicalOriginD76)g_dialogue_smooth38.publish({});
        g_follow_consumed_x_d76.store(0,std::memory_order_release);g_follow_consumed_y_d76.store(0,std::memory_order_release);
        s_origin_valid.store(true, std::memory_order_release);
        s_rotation_filter_generation.fetch_add(1,std::memory_order_acq_rel);
        // Centralized here: menu, controller, OpenXR and Theater return all
        // reanchor VRIK when the camera actually consumes the new valid pose.
        if(recenter)frik_pose_probe::RequestTrackingReanchor();
        if(!physicalOriginD76||!recenter)roomscale_d76::recenterPending.store(false,std::memory_order_release); // no physical-origin debt
        // A real recenter remains pending until ApplySessionCalibrationH46
        // consumes this exact origin under this same pose mutex and acknowledges.
        char rm[400]{}; std::snprintf(rm,sizeof(rm),
            "V66.125 RECENTER kind=%s originXR=(%.4f,%.4f,%.4f) yawOnly=1 accumulatedOffset=ZERO bodyReanchor=%d userOffsetsSideBackUp=(%.3f,%.3f,%.3f) C43verticalCorrection=%+.3fm",
            had_origin?"RECENTER":"INITIAL",double(pose.pos_x),double(pose.pos_y),double(pose.pos_z),recenter?1:0,
            double(GetCameraSideOffsetMeters()),double(GetCameraBackOffsetMeters()),double(GetCameraHeightOffsetMeters()),double(-shift43));
        diagnostic_trace::Write(rm);
        KCD2VR_DIAG_LOG(Bug,info,"Camera tracking origin recentered at native view (V66.125; explicit user offsets retained)");
    }

    // The head and its exact completed family share the original pose mutex.
    // Stamp only after this call has consumed any recenter/origin generation.
    s_current_family_r24={};
    if(family&&frame_index&&CameraFamilyCurrentR24(*family)&&CameraFamilyMatchesPoseR24(*family,pose)){
        s_current_family_r24=*family;s_current_family_r24.cameraFrame=frame_index;
        s_current_family_r24.cameraGeneration=CurrentFamilyGenerationR24();
    }
    g_current_pose_source_epoch_d75=sourceEpochD75==tracking59c6::epoch.load(std::memory_order_acquire)&&
        (!family||family->sourceEpoch==sourceEpochD75)?sourceEpochD75:0;
    native_roomscale_d76::Origin followOriginD76{};
    if(native_roomscale_d76::Enrolled()&&native_roomscale_d76::ReadTrackingOriginD76(followOriginD76)){
        const auto& c=followOriginD76.consumedTracking;
        const auto local=tracking125::model_vector({c.x,c.y,c.z},{s_origin_pose.qx,s_origin_pose.qy,s_origin_pose.qz,s_origin_pose.qw});
        g_follow_consumed_x_d76.store(local.x,std::memory_order_release);g_follow_consumed_y_d76.store(local.y,std::memory_order_release);
    }
    if(g_roomscale_enabled58.load(std::memory_order_acquire)&&s_apply_position.load(std::memory_order_relaxed))
        RoomscaleXrSample47(pose,s_origin_pose);
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
    tracking59c6::SetCameraSource(mode);
    if (!std::isfinite(prediction_offset_ms)) prediction_offset_ms=0.0f;
    if (prediction_offset_ms < -50.0f) prediction_offset_ms=-50.0f;
    if (prediction_offset_ms > 50.0f) prediction_offset_ms=50.0f;
    s_prediction_offset_ms.store(prediction_offset_ms,std::memory_order_release);
}

int CameraHook::GetPoseSourceMode() { return s_pose_source_mode.load(std::memory_order_acquire); }
void CameraHook::SetRotationOrder(int order) { s_rotation_order.store(order ? 1 : 0,std::memory_order_release); }

void CameraHook::SetViewMarriageMode(int mode, float native_gain)
{
    if(mode<0) mode=0; if(mode>3) mode=3;
    if(!std::isfinite(native_gain)) native_gain=1.0f;
    native_gain=std::clamp(native_gain,0.0f,1.5f);
    s_view_marriage_native_gain.store(native_gain,std::memory_order_release);
    s_view_marriage_mode.store(mode,std::memory_order_release);
}


void CameraHook::SetFreshPublishPose(bool enabled)
{
    s_fresh_publish_pose.store(enabled,std::memory_order_release);
}

bool CameraHook::GetFreshPublishPose()
{
    return s_fresh_publish_pose.load(std::memory_order_acquire);
}

// V66.178R58 roomscale follow API (see vrik/roomscale58.h).
void CameraHook::SetRoomscaleFollow(bool enabled)
{
    if(g_roomscale_enabled58.exchange(enabled,std::memory_order_acq_rel)!=enabled){
        std::lock_guard<std::mutex> rl(g_roomscale_mutex58);kcd2vr::roomscale58::reset(g_roomscale58);g_roomscale_have_base58=false;
        g_recenter_anchor_x.store(0.f,std::memory_order_release);g_recenter_anchor_y.store(0.f,std::memory_order_release);g_roomscale_tick58.store(0,std::memory_order_release);
    }
}
bool CameraHook::RoomscaleRemaining(float& x,float& y,std::uint64_t& tick)
{
    tick=g_roomscale_tick58.load(std::memory_order_acquire);if(!tick)return false;
    x=g_roomscale_rem_x58.load(std::memory_order_relaxed);y=g_roomscale_rem_y58.load(std::memory_order_relaxed);return true;
}
kcd2vr::roomscale58::Drive CameraHook::RoomscaleDrive(std::uint64_t now,bool follow,bool combatLocked,bool dodgeEnabled)
{
    if(!g_roomscale_enabled58.load(std::memory_order_acquire))return {};
    std::lock_guard<std::mutex> rl(g_roomscale_mutex58);
    const float hx=g_roomscale_rem_x58.load(std::memory_order_relaxed)-float(g_roomscale58.anchorX);
    const float hy=g_roomscale_rem_y58.load(std::memory_order_relaxed)-float(g_roomscale58.anchorY);
    kcd2vr::roomscale58::observeHmd(g_roomscale58,g_roomscale_tick58.load(std::memory_order_acquire),hx,hy);
    kcd2vr::roomscale58::Config roomscaleConfigD70{};
    roomscaleConfigD70.forwardLeanHeightD70=kcd2vr::roomscale58::LeanHeightD70();
    roomscaleConfigD70.consumedPhysicalX_D76=g_follow_consumed_x_d76.load(std::memory_order_acquire);
    roomscaleConfigD70.consumedPhysicalY_D76=g_follow_consumed_y_d76.load(std::memory_order_acquire);
    follow=follow&&!roomscale_d76::Enabled()&&!roomscale_d76::recenterPending.load(std::memory_order_acquire);
    auto d=kcd2vr::roomscale58::drive(g_roomscale58,now,hx,hy,follow,combatLocked,dodgeEnabled,roomscaleConfigD70);
    if(d.dodge){g_recenter_anchor_x.store(float(g_roomscale58.anchorX),std::memory_order_release);g_recenter_anchor_y.store(float(g_roomscale58.anchorY),std::memory_order_release);}
    return d;
}
void CameraHook::RoomscaleCounters(std::uint64_t& consumed,std::uint64_t& dodges,std::uint64_t& injected,float& anchorX,float& anchorY)
{
    std::lock_guard<std::mutex> rl(g_roomscale_mutex58);
    consumed=g_roomscale58.consumed;dodges=g_roomscale58.dodges;injected=g_roomscale58.injectedFrames;anchorX=float(g_roomscale58.anchorX);anchorY=float(g_roomscale58.anchorY);
}

void CameraHook::RequestRecenter()
{
    tracking59c6::Reset();
    s_recenter_requested.store(true, std::memory_order_release);
}

bool CameraHook::CopyHeadPoseR112(XrPose& out) noexcept
{
    if(!s_pose_received.load(std::memory_order_acquire))return false;
    std::lock_guard<std::mutex> lock(s_pose_mutex);
    out=s_current_pose;
    return true;
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

static uintptr_t g_hist_cview=0;
static bool g_hist_valid=false;
static Quatf g_hist_native_q{0,0,0,1}, g_hist_written_q{0,0,0,1};
static Vec3f g_hist_native_p{0,0,0}, g_hist_written_p{0,0,0};

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

// C47: accepted XR input feeds history independently of culling-camera reuse.
static void RoomscaleXrSample47(const XrPose& pose,const XrPose& origin) {
    const Quatf oq{origin.qx,origin.qy,origin.qz,origin.qw};
    const Vec3f d=Rotate(Conjugate(Normalize(oq)),Vec3f{pose.pos_x-origin.pos_x,pose.pos_y-origin.pos_y,pose.pos_z-origin.pos_z});
    const auto tick=std::uint64_t(GetTickCount64());
    std::lock_guard<std::mutex> rl(g_roomscale_mutex58);
    g_roomscale_rem_x58.store(float(d.x+g_roomscale58.anchorX),std::memory_order_relaxed);
    g_roomscale_rem_y58.store(float(-d.z+g_roomscale58.anchorY),std::memory_order_relaxed);
    const Quatf pq{pose.qx,pose.qy,pose.qz,pose.qw};
    const Vec3f forward=Rotate(Conjugate(Normalize(oq)),Rotate(Normalize(pq),Vec3f{0,0,-1}));
    // Input history metadata only. Camera/view, follow translation and anchor
    // remain in the existing origin frame; dodge uses current horizontal facing.
    // D70: tracking-space vertical height metadata only; origin pitch must not
    // turn horizontal movement into apparent height. This does not alter view.
    kcd2vr::roomscale58::observeHmd(g_roomscale58,tick,d.x,-d.z,forward.x,-forward.z,
        double(pose.pos_y)-double(origin.pos_y),true);
    g_roomscale_tick58.store(tick,std::memory_order_release);
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

static Quatf MatrixToQuat(const Matrix34f& m)
{
    const float m00=m.m[0][0], m01=m.m[0][1], m02=m.m[0][2];
    const float m10=m.m[1][0], m11=m.m[1][1], m12=m.m[1][2];
    const float m20=m.m[2][0], m21=m.m[2][1], m22=m.m[2][2];
    Quatf q{}; const float tr=m00+m11+m22;
    if(tr>0.0f){ const float t=std::sqrt(tr+1.0f)*2.0f; q.w=0.25f*t; q.x=(m21-m12)/t; q.y=(m02-m20)/t; q.z=(m10-m01)/t; }
    else if(m00>m11 && m00>m22){ const float t=std::sqrt(1.0f+m00-m11-m22)*2.0f; q.w=(m21-m12)/t; q.x=0.25f*t; q.y=(m01+m10)/t; q.z=(m02+m20)/t; }
    else if(m11>m22){ const float t=std::sqrt(1.0f+m11-m00-m22)*2.0f; q.w=(m02-m20)/t; q.x=(m01+m10)/t; q.y=0.25f*t; q.z=(m12+m21)/t; }
    else { const float t=std::sqrt(1.0f+m22-m00-m11)*2.0f; q.w=(m10-m01)/t; q.x=(m02+m20)/t; q.y=(m12+m21)/t; q.z=0.25f*t; }
    return Normalize(q);
}

static bool ApplyTrackedPose(uintptr_t camera, const XrPose& pose, const XrPose& origin,
                             float world_scale, bool apply_rotation, bool apply_position,
                             int rotation_filter_mode, std::uint64_t rotation_filter_generation,
                             const native_roomscale_d76::Origin& roomOriginD76,
                             std::uint64_t visualSourceD75,std::uint64_t visualGenerationD75,std::uint64_t visualFrameD75)
{
    // Exact RTTI-backed CView gate recovered from the user's retail WHGame.dll.
    // This replaces the old permissive "vtable lies somewhere inside WHGame" test.
    if (!IsExactGameCView(camera)) return false;

    const uintptr_t cview = camera - kSViewParamsViewMatrixOffset;
    auto* matrix = reinterpret_cast<Matrix34f*>(camera);
    float displayCenterD75[3]{};
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
            const auto residualD76=roomscale_d76::residual({pose.pos_x-origin.pos_x,
                pose.pos_y-origin.pos_y,pose.pos_z-origin.pos_z},roomOriginD76);
            const Vec3f xr_delta_world{residualD76.x,residualD76.y,residualD76.z};
            const Vec3f xr_delta = Rotate(Conjugate(Normalize(origin_q)), xr_delta_world);
            cry_local = {xr_delta.x, -xr_delta.z, xr_delta.y};
        }
        cry_local.x += g_recenter_anchor_x.load(std::memory_order_acquire);
        cry_local.y += g_recenter_anchor_y.load(std::memory_order_acquire);
        cry_local.z += g_recenter_anchor_z.load(std::memory_order_acquire);
        cry_local.x += CameraHook::GetCameraSideOffsetMeters();
        const float centerLocalXD75=cry_local.x;
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
        // Display-only center before IPD, using D76's consumed-origin residual.
        // Keep every rendered position expression and eye-offset load intact.
        displayCenterD75[0]=base_pos.x+(base_right.x*centerLocalXD75+base_forward.x*cry_local.y+base_up.x*cry_local.z)*world_scale;
        displayCenterD75[1]=base_pos.y+(base_right.y*centerLocalXD75+base_forward.y*cry_local.y+base_up.y*cry_local.z)*world_scale;
        displayCenterD75[2]=base_pos.z+(base_right.z*centerLocalXD75+base_forward.z*cry_local.y+base_up.z*cry_local.z)*world_scale;
    }

    CameraHook::PublishVisualForwardFromCameraMatrix(matrix);
    CameraHook::PublishVisualPositionFromCameraMatrix(matrix);
    PublishVisualRayD75(*matrix,visualSourceD75,visualGenerationD75,visualFrameD75,displayCenterD75,world_scale);
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
                                      XrPose& out_pose, XrPose& out_origin, float& out_scale,
                                      const std::atomic<std::uint64_t>* frame_source=nullptr,
                                      std::uint64_t* out_frame=nullptr,
                                      const hj1::Sample* family_source=nullptr,hj1::Sample* out_family=nullptr,
                                      const std::atomic<std::uint64_t>* generation_source=nullptr,std::uint64_t* out_generation=nullptr,
                                      native_roomscale_d76::Origin* out_room_origin=nullptr,std::uint64_t* out_room_revision=nullptr,
                                      const std::uint64_t* source_epoch=nullptr,std::uint64_t* out_source_epoch=nullptr)
{
    std::lock_guard<std::mutex> lock(mutex);
    out_pose = src_pose;
    out_origin = src_origin;
    if(out_room_origin)(void)native_roomscale_d76::ReadTrackingOriginD76(*out_room_origin);
    if(out_room_revision)*out_room_revision=roomscale_d76::originRevision.load(std::memory_order_acquire);
    out_scale = scale_atomic.load(std::memory_order_relaxed);
    if(frame_source&&out_frame)*out_frame=frame_source->load(std::memory_order_relaxed);
    if(family_source&&out_family)*out_family=*family_source;
    if(generation_source&&out_generation)*out_generation=generation_source->load(std::memory_order_relaxed);
    if(source_epoch&&out_source_epoch)*out_source_epoch=*source_epoch;
}

// Keep RAII out of the SEH-bearing camera function (MSVC C2712).
static void RoomscaleBookkeeping58(const Vec3f& base_pos,const Vec3f& cry_local,
                                   const Vec3f& r58,const Vec3f& f58,float world_scale){
                std::lock_guard<std::mutex> rl(g_roomscale_mutex58);
                if(g_roomscale_have_base58){
                    const Vec3f d{base_pos.x-g_roomscale_base58[0],base_pos.y-g_roomscale_base58[1],base_pos.z-g_roomscale_base58[2]};
                    const double entX=(d.x*r58.x+d.y*r58.y+d.z*r58.z)/(world_scale>1e-6f?world_scale:1.f);
                    const double entY=(d.x*f58.x+d.y*f58.y+d.z*f58.z)/(world_scale>1e-6f?world_scale:1.f);
                    kcd2vr::roomscale58::consume(g_roomscale58,static_cast<std::uint64_t>(GetTickCount64()),cry_local.x,cry_local.y,entX,entY,kcd2vr::roomscale58::Config{});
                }
                g_roomscale_base58[0]=base_pos.x;g_roomscale_base58[1]=base_pos.y;g_roomscale_base58[2]=base_pos.z;g_roomscale_have_base58=true;
                g_recenter_anchor_x.store(float(g_roomscale58.anchorX),std::memory_order_release);
                g_recenter_anchor_y.store(float(g_roomscale58.anchorY),std::memory_order_release);
                g_roomscale_rem_x58.store(float(cry_local.x+g_roomscale58.anchorX),std::memory_order_relaxed);
                g_roomscale_rem_y58.store(float(cry_local.y+g_roomscale58.anchorY),std::memory_order_relaxed);
                g_roomscale_tick58.store(static_cast<std::uint64_t>(GetTickCount64()),std::memory_order_release);
}

// V51: apply the current immutable XR snapshot directly to the CCamera matrix passed
// to PublishViewCamera. The caller backs up/restores the matrix around the engine copy,
// so KCD2's source camera remains untouched and the transform cannot accumulate.
bool CameraHook::ApplyCurrentPoseToPublishedCamera(void* camera, std::uint64_t* out_pose_frame, bool record_render_identity,hj1::Sample* out_family)
{
    if(out_family)*out_family={};
    if (!camera || !s_pose_received.load(std::memory_order_acquire) ||
        !s_origin_valid.load(std::memory_order_acquire)) return false;

    kcd2vr::diagnostics::ownership48::cameraSample48=kcd2vr::diagnostics::ownership48::cameraPublication48=kcd2vr::diagnostics::ownership48::cameraEpoch48=kcd2vr::diagnostics::ownership48::cameraActor48=0;
    XrPose pose{}, origin{}; float world_scale=1.0f;
    std::uint64_t pose_frame=0,generationR24=0,visualSourceD75=0;
    native_roomscale_d76::Origin canonicalOriginD76{};std::uint64_t roomRevisionD76=0;
    hj1::Sample familyR24{};
    const bool wantFamilyR24=out_family&&hj1::enabled.load(std::memory_order_acquire);
    CopyCurrentPoseAndOrigin(s_pose_mutex, s_current_pose, s_origin_pose, s_current_scale,
                             pose, origin, world_scale,&s_current_pose_frame_index,&pose_frame,
                             &s_current_family_r24,wantFamilyR24?&familyR24:nullptr,
                             &s_rotation_filter_generation,&generationR24,&canonicalOriginD76,&roomRevisionD76,
                             &g_current_pose_source_epoch_d75,&visualSourceD75);
    const int pose_source=s_pose_source_mode.load(std::memory_order_acquire);
    bool sync_ok=false; XrTime sampled_time=0;
    if (pose_source==1 || pose_source==3) {
        XrTime t=XrManager::Instance().GetLatestPredictedDisplayTime();
        if (pose_source==3) {
            const double off_ms=static_cast<double>(s_prediction_offset_ms.load(std::memory_order_acquire));
            t += static_cast<XrTime>(off_ms * 1000000.0);
        }
        sampled_time=t; sync_ok=XrManager::Instance().SampleHeadPoseAtTime(t,pose,wantFamilyR24&&pose_source==1?&familyR24:nullptr);
    } else if (pose_source==2) {
        sync_ok=XrManager::Instance().SampleHeadPoseNow(pose,&sampled_time,wantFamilyR24?&familyR24:nullptr);
    }
    if(pose_source==3||(pose_source!=0&&!sync_ok))familyR24={};
    if(pose_source==0&&familyR24.cameraGeneration!=generationR24)familyR24={};
    if(pose_source==2&&!sync_ok){
        tracking59c6::Frame cached59c6{};
        if(tracking59c6::ReadHead(cached59c6)&&cached59c6.valid[0]&&tracking59c6::Valid(cached59c6.poses[0])&&
           tracking59c6::Age(cached59c6.ns,tracking59c6::NowNs(),20000000ull)){
            const auto& h=cached59c6.poses[0];pose={h.x,h.y,h.z,h.qx,h.qy,h.qz,h.qw};
            sampled_time=cached59c6.locateTime;sync_ok=true;
        }
    }
    if (pose_source!=0 && !sync_ok &&
        (!motion_trials_d67::UncappedSelectedAt(0)||
         diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,GetTickCount64())||
         diagnostics::capture168::active(diagnostics::capture168::Domain::Vrik,GetTickCount64()))) {
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

        // V66.110: Theater/cutscene ownership is a true camera-transform no-op.
        // The presentation router disables BOTH tracking options for these scenes;
        // do not still apply recenter/calibration offsets or the immersive stripped
        // basis behind the virtual screen. Publish the native camera for consumers.
        if(!s_apply_rotation.load(std::memory_order_relaxed) && !s_apply_position.load(std::memory_order_relaxed)){
            PublishVisualForwardFromCameraMatrix(matrix);PublishVisualPositionFromCameraMatrix(matrix);
            (void)g_visual_ray_d75.publish({}); // no tracked name ray in the native Theater path
            if(record_render_identity)s_last_applied_pose_frame_index.store(pose_frame,std::memory_order_release);
            return true;
        }

        // V64.19 F5 dialogue takeover lab. We continuously remember the last REAL
        // pre-dialogue native PublishViewCamera matrix. During modes 1..3 we use
        // that stable player-view base instead of KCD2's cinematic cut camera,
        // then apply the normal OpenXR head pose below. The engine source camera
        // is still restored by the caller after publication, so this is reversible.
        const int dialogue_lab_mode=s_dialogue_lab_mode.load(std::memory_order_acquire);
        const bool dialogue_lab_active=s_dialogue_lab_active.load(std::memory_order_acquire);
        if(record_render_identity && !dialogue_lab_active) StoreDialoguePreBase(*matrix);
        if(dialogue_lab_active && dialogue_lab_mode>=1) {
            const bool copied=LoadDialoguePreBase(*matrix);
            if(!copied) {
                const auto now=static_cast<std::uint64_t>(GetTickCount64());
                auto last=g_dialogue_last_log_ms.load(std::memory_order_relaxed);
                if(now-last>=1000ull && g_dialogue_last_log_ms.compare_exchange_strong(last,now,std::memory_order_relaxed))
                    debug_trace::Write("V64.19-F5 CAMERA-TAKEOVER fail-closed: no pre-dialogue PublishViewCamera base captured");
            } else {
                // Mode 2 adds a conservative 10 cm forward conversational bias.
                // Mode 3 derives only a small positional bias from the native shot
                // category; it never adopts the cinematic camera transform or FOV.
                float forward_bias_m=0.0f;
                if(dialogue_lab_mode==2) forward_bias_m=0.10f;
                if(dialogue_lab_mode==3) {
                    switch(s_dialogue_native_camera_type.load(std::memory_order_acquire)) {
                        case 1: forward_bias_m=0.05f; break; // Medium
                        case 2: forward_bias_m=0.10f; break; // CloseShot
                        case 3: forward_bias_m=0.15f; break; // Closeup
                        case 4: forward_bias_m=0.00f; break; // Master
                        default: forward_bias_m=0.08f; break;
                    }
                }
                if(forward_bias_m!=0.0f) {
                    matrix->m[0][3] += matrix->m[0][1]*forward_bias_m*world_scale;
                    matrix->m[1][3] += matrix->m[1][1]*forward_bias_m*world_scale;
                    matrix->m[2][3] += matrix->m[2][1]*forward_bias_m*world_scale;
                }
                const auto frame=g_dialogue_takeover_frames.fetch_add(1,std::memory_order_relaxed)+1;
                const auto now=static_cast<std::uint64_t>(GetTickCount64());
                auto last=g_dialogue_last_log_ms.load(std::memory_order_relaxed);
                if((frame<=4 || now-last>=1500ull) && g_dialogue_last_log_ms.compare_exchange_strong(last,now,std::memory_order_relaxed)) {
                    char dm[320]{}; std::snprintf(dm,sizeof(dm),
                        "V64.19-F5 CAMERA-TAKEOVER mode=%d nativeType=%d bias=%.2fm frame=%llu source=pre-dialogue-publish-base",
                        dialogue_lab_mode,s_dialogue_native_camera_type.load(std::memory_order_relaxed),double(forward_bias_m),(unsigned long long)frame);
                    debug_trace::Write(dm);
                }
            }
        }

        // Only the measured native-eye BASE changes. The existing single HMD/IPD
        // composition below remains authoritative; native Theater already returned.
        // Exact activity camera ownership replaces only the eye BASE. The
        // native update already ran; current HMD/recenter/IPD follows once.
        // Canonical consumed C was copied with this exact XR origin under the
        // existing pose mutex. Native/authored eye smoothing remains native.
        native_roomscale_d76::Origin roomOriginD76=canonicalOriginD76;
        const bool activityEye59c19=ActivityEyeBase59c19(*matrix,record_render_identity,&roomOriginD76,roomRevisionD76);
        const bool dialogueEye59c10r2=DialogueEyeBase59c10r2(*matrix,record_render_identity,roomRevisionD76,&canonicalOriginD76);
        if(dialogueEye59c10r2)roomOriginD76=canonicalOriginD76;
        Vec3f base_right{matrix->m[0][0],matrix->m[1][0],matrix->m[2][0]};
        Vec3f base_forward{matrix->m[0][1],matrix->m[1][1],matrix->m[2][1]};
        { const float fl=std::sqrt(base_forward.x*base_forward.x+base_forward.y*base_forward.y+base_forward.z*base_forward.z);
          if(fl>1e-4f&&std::isfinite(fl)){ s_native_pitch161.store(std::asin(std::clamp(base_forward.z/fl,-1.0f,1.0f)),std::memory_order_relaxed);
            s_native_pitch_tick161.store(GetTickCount64(),std::memory_order_release);} } // V66.161
        Vec3f base_up{matrix->m[0][2],matrix->m[1][2],matrix->m[2][2]};
        Vec3f base_pos{matrix->m[0][3],matrix->m[1][3],matrix->m[2][3]};
        const Vec3f nativeEye48=base_pos;bool trackedBase48=false;
        // C51R1: ordinary play uses the SAME committed frame as body/hands.
        // C55 pins the exact drawn bow foundation and removes native aim yaw;
        // current entity root travel/turn and fresh HMD/IPD follow once.
        float frameX51=0,frameY51=0,frameZ51=0,frameHx55=0,frameHy55=0;
        bool independentBowBasis64=false;native_roomscale_d76::Origin trackedOriginD76{};
        if(!dialogueEye59c10r2&&!activityEye59c19&&s_view_marriage_mode.load()==0&&
           !s_native_head_rotation_owner.load()&&frik_pose_probe::TrackedFrameCamera55(base_pos.x,base_pos.y,base_pos.z,
                base_forward.x,base_forward.y,base_forward.z,frameX51,frameY51,frameZ51,frameHx55,frameHy55,&trackedOriginD76)&&
           roomscale_d76::compatible(canonicalOriginD76,trackedOriginD76,roomRevisionD76)){
            trackedBase48=true;roomOriginD76=trackedOriginD76;
            base_pos={frameX51,frameY51,frameZ51};
            // Only a current exact bow frame with a positively neutral local
            // heading qualifies this. Native aim yaw must not rotate the view;
            // real character-world turns and the fresh HMD delta remain live.
            const float headingLengthSquared=frameHx55*frameHx55+frameHy55*frameHy55;
            if(std::isfinite(headingLengthSquared)&&headingLengthSquared>.5f){
                const float invHeadingLength=1.f/std::sqrt(headingLengthSquared);
                base_forward={frameHx55*invHeadingLength,frameHy55*invHeadingLength,0};
                // Cry camera columns are right/forward/up. Level ALL axes:
                // right = forward x world-up; keeping animated right/up here
                // made this foundation a shear for raw-basis consumers.
                base_right={base_forward.y,-base_forward.x,0};
                base_up={0,0,1};
                independentBowBasis64=true;
            }
        }
        if(roomOriginD76.owner.epoch&&roomOriginD76.owner.epoch!=roomRevisionD76)roomOriginD76=canonicalOriginD76;
        const Vec3f basis[3]={base_right,base_forward,base_up};

        const Quatf now_q{pose.qx,pose.qy,pose.qz,pose.qw};
        const Quatf origin_q{origin.qx,origin.qy,origin.qz,origin.qw};
        Quatf delta_xr=Normalize(Mul(Conjugate(Normalize(origin_q)),Normalize(now_q)));
        delta_xr=ApplyV55RotationFilter(delta_xr,s_rotation_filter_mode.load(std::memory_order_acquire),s_rotation_filter_generation.load(std::memory_order_acquire));
        float delta_local[3][3]{}; XrQuatToCryMatrix(delta_xr,delta_local);

        // V64.22B: publish the recentered LOCAL HMD angles before any native-aim
        // compensation. Cry local forward is matrix column 1 (+Y at identity).
        float hmd_local_yaw=0.0f, hmd_local_pitch=0.0f;
        bool hmd_local_angles_valid=false;
        {
            const float fx=delta_local[0][1], fy=delta_local[1][1], fz=delta_local[2][1];
            const float h=std::sqrt(fx*fx+fy*fy);
            const float hyaw=std::atan2(-fx,fy);
            const float hpitch=std::atan2(fz,h);
            const bool ok=std::isfinite(hyaw)&&std::isfinite(hpitch)&&h>1e-4f;
            hmd_local_yaw=ok?hyaw:0.0f; hmd_local_pitch=ok?hpitch:0.0f; hmd_local_angles_valid=ok;
            s_head_aim_local_yaw_rad.store(hmd_local_yaw,std::memory_order_release);
            s_head_aim_local_pitch_rad.store(hmd_local_pitch,std::memory_order_release);
            s_head_aim_local_valid.store(ok,std::memory_order_release);
        }

        // Part of the local head rotation may already have been transferred to
        // KCD2's real look-controller scalars. Remove exactly that portion from
        // the compositor/camera HMD delta: base*C * (C^-1*D) == base*D. This
        // prevents the old positive-feedback/double-turn spin while allowing the
        // bow/fists/native AimPose to ease toward the head direction.
        {
            const float cyaw=s_head_aim_transferred_yaw_rad.load(std::memory_order_acquire);
            const float cpitch=s_head_aim_transferred_pitch_rad.load(std::memory_order_acquire);
            if(!independentBowBasis64&&(std::fabs(cyaw)>1e-6f || std::fabs(cpitch)>1e-6f)) {
                const float cy=std::cos(cyaw), sy=std::sin(cyaw);
                const float cp=std::cos(cpitch), sp=std::sin(cpitch);
                // C = Rz(yaw)*Rx(pitch), column-vector convention.
                const float C[3][3]={{cy,-sy*cp, sy*sp},
                                     {sy, cy*cp,-cy*sp},
                                     {0.0f,sp,cp}};
                float residual[3][3]{};
                // residual = transpose(C) * D
                for(int r=0;r<3;++r) for(int c=0;c<3;++c)
                    residual[r][c]=C[0][r]*delta_local[0][c]+C[1][r]*delta_local[1][c]+C[2][r]*delta_local[2][c];
                for(int r=0;r<3;++r) for(int c=0;c<3;++c) delta_local[r][c]=residual[r][c];
            }
        }

        // V66.109: ordinary immersive VR has one final orientation authority: HMD.
        // KCD2 still supplies camera POSITION and its horizontal/root yaw, but
        // native animated pitch/roll (pickup, grab, authored head bob) is stripped
        // before the recentered HMD delta is applied.  This prevents an animation
        // from physically pitching/rolling the user's world. Diagnostic F3 marriage
        // modes retain their historical basis manipulation below.
        Vec3f visual_basis[3]={basis[0],basis[1],basis[2]};
        const int marriage_mode=s_view_marriage_mode.load(std::memory_order_acquire);
        const bool hmd_final_orientation_authority=(marriage_mode==0)&&!s_native_head_rotation_owner.load(std::memory_order_acquire);
        if(hmd_final_orientation_authority) {
            Vec3f f{base_forward.x,base_forward.y,0.0f};
            float fl=std::sqrt(f.x*f.x+f.y*f.y);
            if(fl<1e-5f) {
                // Degenerate vertical native camera: derive horizontal forward from right.
                f={-base_right.y,base_right.x,0.0f};
                fl=std::sqrt(f.x*f.x+f.y*f.y);
            }
            if(fl>=1e-5f) {
                f.x/=fl;f.y/=fl;
                const Vec3f r{f.y,-f.x,0.0f};
                const Vec3f u{0.0f,0.0f,1.0f};
                visual_basis[0]=r;visual_basis[1]=f;visual_basis[2]=u;
            }
        }
        // R133 FREE PLUS (Miki 7 Nov: "camera moving too much circling the enemy ... make my head view not move at
        // all"): while Henry has a combat opponent the game keeps turning his root/camera toward him (locked
        // locomotion, no hook; the queued turn command is already vetoed). that yaw is absorbed into a view offset
        // here and into the VR body frame (offsetBody, frik_pose_probe CalFrameR133), so head, hands and the room
        // stay where they are in the world and the lock stays underneath. your own stick/snap turns pass.
        // after the fight the pad turns Henry back under the view (offset runs out). recenter/load/dialogue reset it.
        {
            namespace r133=kcd2vr::free_targeting_r133;
            static r133::Hold holdR133{};static std::uint64_t genR133=0,epochR133=0;
            const bool plusR133=kcd2vr::free_targeting_d42::enabled.load(std::memory_order_acquire)&&
                kcd2vr::free_targeting_d42::mode()==r133::FreePlus;
            // R142: the hold also runs in the culling apply (record_render_identity=false). the rendered camera reuses that
            // culled matrix (ApplyLatestCullingPoseToPublishedCamera), so the record=true call never ran: R138 log 7 Nov
            // 08:51-08:53 FREE PLUS engaged=1 context=1 for 3 min with absorbFrames=0, W3 XR_TIMING cameraLatchUsed ~470/5 s
            // cameraLatchMissed=0 the whole log. one caller at a time (the two applies can run on different threads); a
            // skipped call loses nothing, the next one takes the whole yaw delta since the last call.
            static std::atomic<bool> busyR133{false};
            if((plusR133||holdR133.haveCamera||holdR133.view!=0||holdR133.body!=0)&& // the last call after leaving mode 2 forgets the old yaw
               !busyR133.exchange(true,std::memory_order_acquire)){
                const auto nowR133=std::uint64_t(GetTickCount64());
                const auto genNow=s_rotation_filter_generation.load(std::memory_order_acquire);
                const auto epochNow=frik_pose_probe::PoseEpochH24();
                r133::Input in{};in.modePlus=plusR133;in.now=nowR133;
                in.reset=(genR133&&genNow!=genR133)||(epochR133&&epochNow!=epochR133)||dialogueEye59c10r2||activityEye59c19||
                    !hmd_final_orientation_authority||kcd2vr::riding_vrik_d76::RiderRecentR98(nowR133);
                genR133=genNow;epochR133=epochNow;
                in.context=trackedBase48&&kcd2vr::free_targeting_d42::contextReady.load(std::memory_order_acquire);
                in.engaged=r133::engaged(nowR133);in.userTurn=r133::userTurning(nowR133);
                const float bfl=std::sqrt(base_forward.x*base_forward.x+base_forward.y*base_forward.y);
                in.cameraYaw=bfl>1e-4f?r133::yawOf(base_forward.x,base_forward.y):holdR133.lastCamera;
                const auto rootTick=r133::rootYawTick.load(std::memory_order_acquire);
                in.haveRoot=rootTick&&nowR133>=rootTick&&nowR133-rootTick<=100;
                in.rootYaw=r133::rootYaw.load(std::memory_order_relaxed);
                const auto clearsBefore=holdR133.clears;
                const auto stepR133=r133::step(holdR133,in);
                if(stepR133==r133::Step::Absorb){
                    r133::absorbFrames.fetch_add(1,std::memory_order_relaxed);
                    r133::absorbedDeg.store(float(holdR133.absorbedDeg),std::memory_order_relaxed);
                    r133::maxOffsetDeg.store(float(holdR133.maxOffsetDeg),std::memory_order_relaxed);
                }
                if(!in.engaged&&stepR133!=r133::Step::Absorb){holdR133.absorbedDeg=0;holdR133.maxOffsetDeg=0;}
                if(holdR133.clears!=clearsBefore)r133::clears.fetch_add(1,std::memory_order_relaxed);
                r133::offsetView.store(float(holdR133.view),std::memory_order_release);
                r133::offsetBody.store(float(holdR133.body),std::memory_order_release);
                r133::bleedSign.store(stepR133==r133::Step::Bleed?holdR133.bleedSign:0,std::memory_order_release);
                busyR133.store(false,std::memory_order_release);
            }
            const double phiR133=hmd_final_orientation_authority&&!dialogueEye59c10r2&&!activityEye59c19?double(r133::offsetView.load(std::memory_order_acquire)):0.; // atomic: culling applies can run on another thread
            if(std::fabs(phiR133)>1e-6){
                const float c=float(std::cos(phiR133)),s=float(std::sin(phiR133));
                for(auto& v:visual_basis){const float x=v.x,y=v.y;v.x=c*x-s*y;v.y=s*x+c*y;}
            }
        }
        // V64.37 F3 marriage lab. When native CameraInputDispatch is shadow-driving
        // KCD2 aim/interactions, the incoming camera base already contains some of the
        // HMD yaw/pitch. For modes 2/3 remove the estimated native component from that
        // base, then apply the absolute OpenXR local pose exactly once below. This keeps
        // KCD2 gameplay state native while making the visible camera OpenXR-authored.
        const float marriage_gain=s_view_marriage_native_gain.load(std::memory_order_acquire);
        if(marriage_mode>=2 && hmd_local_angles_valid) {
            const float cyaw=hmd_local_yaw*marriage_gain;
            const float cpitch=(marriage_mode>=3?hmd_local_pitch*marriage_gain:0.0f);
            const float cy=std::cos(cyaw), sy=std::sin(cyaw);
            const float cp=std::cos(cpitch), sp=std::sin(cpitch);
            const float C[3][3]={{cy,-sy*cp, sy*sp},
                                 {sy, cy*cp,-cy*sp},
                                 {0.0f,sp,cp}};
            // visual_basis = native_base * inverse(C) = native_base * transpose(C).
            for(int c=0;c<3;++c) {
                const Vec3f w{
                    basis[0].x*C[c][0]+basis[1].x*C[c][1]+basis[2].x*C[c][2],
                    basis[0].y*C[c][0]+basis[1].y*C[c][1]+basis[2].y*C[c][2],
                    basis[0].z*C[c][0]+basis[1].z*C[c][1]+basis[2].z*C[c][2]};
                visual_basis[c]=w;
            }
        }

        if (s_apply_rotation.load(std::memory_order_relaxed)) {
            const int order=s_rotation_order.load(std::memory_order_acquire);
            if (order==0) {
                // Local HMD rotation after the selected visual base: Bvisual * D.
                for(int c=0;c<3;++c){
                    const Vec3f world{
                        visual_basis[0].x*delta_local[0][c]+visual_basis[1].x*delta_local[1][c]+visual_basis[2].x*delta_local[2][c],
                        visual_basis[0].y*delta_local[0][c]+visual_basis[1].y*delta_local[1][c]+visual_basis[2].y*delta_local[2][c],
                        visual_basis[0].z*delta_local[0][c]+visual_basis[1].z*delta_local[1][c]+visual_basis[2].z*delta_local[2][c]};
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
        PublishVisualForwardFromCameraMatrix(matrix);
        if(hmd_final_orientation_authority && record_render_identity && s_apply_rotation.load(std::memory_order_relaxed)) {
            static std::atomic<std::uint64_t> next_hmd_owner_log{0};
            const auto now_ms=static_cast<std::uint64_t>(GetTickCount64());
            auto next=next_hmd_owner_log.load(std::memory_order_relaxed);
            if(now_ms>=next && next_hmd_owner_log.compare_exchange_strong(next,now_ms+5000ull,std::memory_order_relaxed))
                diagnostic_trace::Write("V66.109-CAMERA finalOrientation=HMD nativeContribution=POSITION+HORIZONTAL_YAW nativePitchRoll=STRIPPED");
        }

        float displayCenterD75[3]{};
        {
            Vec3f cry_local{0.0f,0.0f,0.0f};
            if (s_apply_position.load(std::memory_order_relaxed)) {
                const auto residualD76=roomscale_d76::residual({pose.pos_x-origin.pos_x,
                    pose.pos_y-origin.pos_y,pose.pos_z-origin.pos_z},roomOriginD76);
                const Vec3f xr_world{residualD76.x,residualD76.y,residualD76.z};
                const Vec3f xr_delta=Rotate(Conjugate(Normalize(origin_q)),xr_world);
                cry_local={xr_delta.x,-xr_delta.z,xr_delta.y};
            }
            // V66.178R58: roomscale follow bookkeeping, once per rendered frame. The entity movement is the native
            // camera base delta projected on the camera-local axes; only its component toward the player is consumed.
            if(record_render_identity&&!roomscale_d76::Enabled()&&g_roomscale_enabled58.load(std::memory_order_acquire)&&s_apply_position.load(std::memory_order_relaxed)){
                const bool visual58=hmd_final_orientation_authority||marriage_mode>=2;
                RoomscaleBookkeeping58(base_pos,cry_local,visual58?visual_basis[0]:base_right,
                    visual58?visual_basis[1]:base_forward,world_scale);
            }
            cry_local.x += g_recenter_anchor_x.load(std::memory_order_acquire);
            cry_local.y += g_recenter_anchor_y.load(std::memory_order_acquire);
            cry_local.z += g_recenter_anchor_z.load(std::memory_order_acquire);
            cry_local.x += GetCameraSideOffsetMeters();
            const float centerLocalXD75=cry_local.x;
            cry_local.x += GetAfrEyeOffsetMeters();
            cry_local.y -= GetCameraBackOffsetMeters();
            cry_local.z += GetCameraHeightOffsetMeters();
            const bool use_visual_position_basis=hmd_final_orientation_authority||marriage_mode>=2;
            const Vec3f& pos_right=(use_visual_position_basis?visual_basis[0]:base_right);
            const Vec3f& pos_forward=(use_visual_position_basis?visual_basis[1]:base_forward);
            const Vec3f& pos_up=(use_visual_position_basis?visual_basis[2]:base_up);
            const Vec3f wd{
                pos_right.x*cry_local.x+pos_forward.x*cry_local.y+pos_up.x*cry_local.z,
                pos_right.y*cry_local.x+pos_forward.y*cry_local.y+pos_up.y*cry_local.z,
                pos_right.z*cry_local.x+pos_forward.z*cry_local.y+pos_up.z*cry_local.z};
            matrix->m[0][3]=base_pos.x+wd.x*world_scale;
            matrix->m[1][3]=base_pos.y+wd.y*world_scale;
            matrix->m[2][3]=base_pos.z+wd.z*world_scale;
            // Alternating eyes share one center-head focus; roomscale and
            // activity bases above remain the exact D76 render calculation.
            displayCenterD75[0]=base_pos.x+(pos_right.x*centerLocalXD75+pos_forward.x*cry_local.y+pos_up.x*cry_local.z)*world_scale;
            displayCenterD75[1]=base_pos.y+(pos_right.y*centerLocalXD75+pos_forward.y*cry_local.y+pos_up.y*cry_local.z)*world_scale;
            displayCenterD75[2]=base_pos.z+(pos_right.z*centerLocalXD75+pos_forward.z*cry_local.y+pos_up.z*cry_local.z)*world_scale;

        }
        // V65.09 F5: publish position only after roomscale translation so the
        // sample can never transiently expose the pre-translation native camera.
        PublishVisualForwardFromCameraMatrix(matrix);
        PublishVisualPositionFromCameraMatrix(matrix);
        PublishVisualRayD75(*matrix,visualSourceD75,generationR24,pose_frame,displayCenterD75,world_scale);
        if(record_render_identity)frik_pose_probe::ObserveImmersiveCamera122(
            matrix->m[0][3],matrix->m[1][3],matrix->m[2][3],GetAfrEye(),pose_frame);
        if(record_render_identity)frik_pose_probe::ObserveBowCamera126(base_pos.x,base_pos.y,base_pos.z,
            pose.pos_x-origin.pos_x,pose.pos_y-origin.pos_y,pose.pos_z-origin.pos_z,GetAfrEye(),pose_frame);
        // Record only after successful immersive application. Native Theater
        // no-ops and failed camera paths cannot train the HJ1 prediction lead.
        if(kcd2vr::hj1::enabled.load(std::memory_order_relaxed)&&(record_render_identity||!GetFreshPublishPose())&&
           (pose_source==0||(pose_source==1&&sync_ok)))
            XrManager::Instance().NoteCameraApplyHJ1(pose_source==1?sampled_time:0,pose_frame);
        // V61.44 FRAME IDENTITY FIX: the V61.12 culling path uses this same
        // matrix helper temporarily, then restores the CCamera. That temporary
        // apply must NOT overwrite the published-render pose identity used to
        // tag the final PRESENT. Otherwise a culling-only pose can be paired
        // with pixels rendered from a different PublishViewCamera pose.
        if(record_render_identity&&kcd2vr::diagnostics::ownership48::active(GetTickCount64())){
            namespace oc=kcd2vr::diagnostics::ownership48;oc::Event e{};e.stage=oc::CameraFinal;e.tick=GetTickCount64();e.thread=GetCurrentThreadId();e.sample=pose_frame;e.poseTime=sampled_time;e.code=pose_source;e.object=oc::cameraSample48;e.publication=oc::cameraPublication48;e.epoch=oc::cameraEpoch48;e.actor=oc::cameraActor48;
            e.flags=(trackedBase48?1u:0u)|(s_apply_rotation.load()?2u:0u)|(s_apply_position.load()?4u:0u)|(s_native_head_rotation_owner.load()?8u:0u);
            const float data[]{nativeEye48.x,nativeEye48.y,nativeEye48.z,base_pos.x,base_pos.y,base_pos.z,matrix->m[0][3],matrix->m[1][3],matrix->m[2][3],pose.pos_x,pose.pos_y,pose.pos_z,pose.qx,pose.qy,pose.qz,pose.qw,
                matrix->m[0][0],matrix->m[1][0],matrix->m[2][0],matrix->m[0][1],matrix->m[1][1],matrix->m[2][1],matrix->m[0][2],matrix->m[1][2],matrix->m[2][2]};
            std::copy_n(data,25,e.v);oc::name(e,trackedBase48?"FINAL_CAMERA_TRACKED_BASE":"FINAL_CAMERA_NATIVE_BASE_OR_ACTIVITY");(void)oc::push(e);
        }
        // This output describes the selected complete XR family only. Native/
        // Theater/reference camera ownership and failed/incomplete locates stay unknown.
        if(wantFamilyR24&&hmd_final_orientation_authority&&s_apply_rotation.load(std::memory_order_acquire)&&
           s_apply_position.load(std::memory_order_acquire)&&generationR24==CurrentFamilyGenerationR24()&&
           CameraFamilyCurrentR24(familyR24)&&CameraFamilyMatchesPoseR24(familyR24,pose)){
            familyR24.cameraFrame=pose_frame;familyR24.cameraGeneration=generationR24;*out_family=familyR24;
        }
        if (!record_render_identity) return true;

        s_last_applied_pose_frame_index.store(pose_frame,std::memory_order_release);
        // V53 instrumentation is expensive enough to perturb the timing under test.
        // V65.03 runs it only at F2 rung 4.
        if(g_angular_diagnostics_enabled.load(std::memory_order_acquire)) {
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
    } __except(EXCEPTION_EXECUTE_HANDLER) { if(out_family)*out_family={};return false; }
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


CensusCallerSlot g_census_callers[48]{};
CensusViewSlot g_census_views[48]{};
std::atomic<std::uint64_t> g_census_total{0};
std::atomic<std::uint64_t> g_census_exact_cview_total{0};
std::atomic<std::uint64_t> g_census_non_cview_total{0};



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
// Receipt sidecar only: the legacy camera latch still selects/consumes the matrix.
// A newer or contended sidecar cannot relabel an older matrix's tracking family.
struct FrustumFamilyR24 {std::uint64_t latchSequence=0;hj1::Sample family{};};
static core::SnapshotMailbox<FrustumFamilyR24,4> g_frustum_family_r24;

static void CaptureFrustumRotationSample(std::uintptr_t camera,
    std::uint64_t pose_frame=CameraHook::CurrentPoseFrameIndex()) noexcept
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
    sample.pose_frame=pose_frame;
    sample.tick_ms=static_cast<std::uint64_t>(GetTickCount64());
    sample.camera=camera;
    const auto begin=g_frustum_rotation_seq.fetch_add(1,std::memory_order_acq_rel)+1; // odd
    (void)begin;
    g_frustum_rotation_sample=sample;
    g_frustum_rotation_seq.fetch_add(1,std::memory_order_release); // even
}

static void CaptureFrustumPosedMatrix(std::uintptr_t camera,std::uint64_t pose_frame,const hj1::Sample* family=nullptr) noexcept
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
    sample.pose_frame=pose_frame;
    sample.tick_ms=static_cast<std::uint64_t>(GetTickCount64());
    sample.camera=camera;
    const auto beginSeqR24=g_frustum_pose_seq.fetch_add(1,std::memory_order_acq_rel)+1; // odd
    g_frustum_posed_sample=sample;
    g_frustum_posed_matrix=full;
    const auto completedSeqR24=g_frustum_pose_seq.fetch_add(1,std::memory_order_release)+1; // even
    if(hj1::enabled.load(std::memory_order_acquire)){
        FrustumFamilyR24 receipt{};receipt.latchSequence=completedSeqR24;
        if(family&&(beginSeqR24&1ull)&&completedSeqR24==beginSeqR24+1)receipt.family=*family;
        (void)g_frustum_family_r24.publish(receipt);
    }
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

bool CameraHook::ApplyLatestCullingPoseToPublishedCamera(void* camera, std::uint64_t* out_pose_frame, std::uint64_t max_age_ms,hj1::Sample* out_family)
{
    if(out_family)*out_family={};
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
    diagnostics::xr165::LatchObserved(true,age); // W3 F2 r1: how old the reused culling pose really was (Bug recorder only)
    bool ok=false;
    __try { *reinterpret_cast<Matrix34f*>(camera)=full; ok=true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { ok=false; }
    if(!ok) return false;
    if(out_family&&hj1::enabled.load(std::memory_order_acquire)){
        FrustumFamilyR24 receipt{};
        if(g_frustum_family_r24.read(receipt)&&receipt.latchSequence==stable_seq&&
           receipt.family.cameraFrame==sample.pose_frame&&receipt.family.cameraGeneration==CurrentFamilyGenerationR24()&&
           s_apply_rotation.load(std::memory_order_acquire)&&s_apply_position.load(std::memory_order_acquire)&&
           !s_native_head_rotation_owner.load(std::memory_order_acquire)&&s_view_marriage_mode.load(std::memory_order_acquire)==0&&
           CameraFamilyCurrentR24(receipt.family))*out_family=receipt.family;
    }
    if(out_pose_frame) *out_pose_frame=sample.pose_frame;
    g_frustum_pose_consumed_seq.store(stable_seq,std::memory_order_release);
    s_last_applied_pose_frame_index.store(sample.pose_frame,std::memory_order_release);
    frik_pose_probe::ObserveImmersiveCamera122(full.m[0][3],full.m[1][3],full.m[2][3],GetAfrEye(),sample.pose_frame);
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

void CameraHook::PublishVisualForwardFromCameraMatrix(const void* camera_matrix)
{
    if(!camera_matrix){
        s_visual_forward_valid.store(false,std::memory_order_release);
        return;
    }
    const auto* m=reinterpret_cast<const Matrix34f*>(camera_matrix);
    const float fx=m->m[0][1], fy=m->m[1][1], fz=m->m[2][1];
    const float fl=std::sqrt(fx*fx+fy*fy+fz*fz);
    const bool ok=std::isfinite(fl)&&fl>1e-4f;
    const auto tick=static_cast<std::uint64_t>(GetTickCount64());
    if(ok){
        s_visual_forward_x.store(fx/fl,std::memory_order_relaxed);
        s_visual_forward_y.store(fy/fl,std::memory_order_relaxed);
        s_visual_forward_z.store(fz/fl,std::memory_order_relaxed);
        s_visual_forward_tick_ms.store(tick,std::memory_order_release);
    }
    s_visual_forward_valid.store(ok,std::memory_order_release);
}

void CameraHook::PublishVisualPositionFromCameraMatrix(const void* camera_matrix)
{
    if(!camera_matrix){
        s_visual_position_valid.store(false,std::memory_order_release);
        return;
    }
    const auto* m=reinterpret_cast<const Matrix34f*>(camera_matrix);
    const float px=m->m[0][3], py=m->m[1][3], pz=m->m[2][3];
    const bool ok=std::isfinite(px)&&std::isfinite(py)&&std::isfinite(pz);
    if(ok){
        s_visual_position_x.store(px,std::memory_order_relaxed);
        s_visual_position_y.store(py,std::memory_order_relaxed);
        s_visual_position_z.store(pz,std::memory_order_relaxed);
        s_visual_position_tick_ms.store(static_cast<std::uint64_t>(GetTickCount64()),std::memory_order_release);
    }
    s_visual_position_valid.store(ok,std::memory_order_release);
}

void CameraHook::SetHistoryPersistentPose(bool enabled)
{
    const bool was=g_history_persistent_pose.exchange(enabled,std::memory_order_acq_rel);
    if(was && !enabled && g_hist_valid && g_hist_cview) {
        __try {
            const Quatf curq=Normalize(*reinterpret_cast<const Quatf*>(g_hist_cview+kSViewParamsRotationOffset));
            const Vec3f curp=*reinterpret_cast<const Vec3f*>(g_hist_cview+kSViewParamsPositionOffset);
            const float dot=std::fabs(curq.x*g_hist_written_q.x+curq.y*g_hist_written_q.y+curq.z*g_hist_written_q.z+curq.w*g_hist_written_q.w);
            const bool pclose=std::fabs(curp.x-g_hist_written_p.x)<1e-4f&&std::fabs(curp.y-g_hist_written_p.y)<1e-4f&&std::fabs(curp.z-g_hist_written_p.z)<1e-4f;
            if(dot>0.99999f && pclose) {
                *reinterpret_cast<Quatf*>(g_hist_cview+kSViewParamsRotationOffset)=g_hist_native_q;
                *reinterpret_cast<Vec3f*>(g_hist_cview+kSViewParamsPositionOffset)=g_hist_native_p;
            }
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
        g_hist_valid=false; g_hist_cview=0;
    }
}


bool CameraHook::GetLatestVisualForwardWorld(float& x, float& y, float& z, std::uint64_t max_age_ms)
{
    if(!s_visual_forward_valid.load(std::memory_order_acquire)) return false;
    const auto tick=s_visual_forward_tick_ms.load(std::memory_order_acquire);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(!tick || now<tick || now-tick>max_age_ms) return false;
    const float lx=s_visual_forward_x.load(std::memory_order_relaxed);
    const float ly=s_visual_forward_y.load(std::memory_order_relaxed);
    const float lz=s_visual_forward_z.load(std::memory_order_relaxed);
    const float n2=lx*lx+ly*ly+lz*lz;
    if(!std::isfinite(n2)||n2<0.90f||n2>1.10f) return false;
    x=lx; y=ly; z=lz;
    return true;
}

bool CameraHook::GetLatestVisualPositionWorld(float& x, float& y, float& z, std::uint64_t max_age_ms)
{
    if(!s_visual_position_valid.load(std::memory_order_acquire)) return false;
    const auto tick=s_visual_position_tick_ms.load(std::memory_order_acquire);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(!tick || now<tick || now-tick>max_age_ms) return false;
    const float lx=s_visual_position_x.load(std::memory_order_relaxed);
    const float ly=s_visual_position_y.load(std::memory_order_relaxed);
    const float lz=s_visual_position_z.load(std::memory_order_relaxed);
    if(!std::isfinite(lx)||!std::isfinite(ly)||!std::isfinite(lz)) return false;
    x=lx; y=ly; z=lz;
    return true;
}

bool CameraHook::GetLatestVisualRayD75(visual_ray_d75::Sample& out,std::uint64_t max_age_ms)
{
    out={};visual_ray_d75::Sample sample{};
    const auto source=tracking59c6::epoch.load(std::memory_order_acquire);
    const auto generation=CurrentFamilyGenerationR24();
    if(!s_pose_received.load(std::memory_order_acquire)||!s_origin_valid.load(std::memory_order_acquire)||
       s_recenter_requested.load(std::memory_order_acquire)||
       roomscale_d76::recenterPending.load(std::memory_order_acquire)||!g_visual_ray_d75.read(sample)||
       !visual_ray_d75::current(sample,std::uint64_t(GetTickCount64()),source,generation,max_age_ms)||
       source!=tracking59c6::epoch.load(std::memory_order_acquire)||generation!=CurrentFamilyGenerationR24()||
       s_recenter_requested.load(std::memory_order_acquire)||roomscale_d76::recenterPending.load(std::memory_order_acquire))return false;
    out=sample;return true;
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
    XrPose origin{};native_roomscale_d76::Origin roomOriginD76{};
    float world_scale = 1.0f;
    std::uint64_t visualSourceD75=0,visualGenerationD75=0,visualFrameD75=0;
    const bool have_pose = s_pose_received.load(std::memory_order_acquire) &&
                           s_origin_valid.load(std::memory_order_acquire);

    if (have_pose)
    {
        CopyCurrentPoseAndOrigin(s_pose_mutex, s_current_pose, s_origin_pose, s_current_scale,
                                  pose, origin, world_scale,&s_current_pose_frame_index,&visualFrameD75,
                                  nullptr,nullptr,&s_rotation_filter_generation,&visualGenerationD75,&roomOriginD76,nullptr,
                                  &g_current_pose_source_epoch_d75,&visualSourceD75);
    }

    // V48: xrWaitFrame may be 72/90 Hz while KCD2 builds its camera much faster.
    // Refresh the head pose near camera consumption, but rate-limit duplicate
    // frustum calls so one game frame does not spam the runtime.
    if (have_pose && XrManager::Instance().GetLatePoseEnabled()) {
        // R59.16C5: the 2 ms rate limit used GetTickCount64, which only advances every ~15.6 ms. That capped the
        // late head refresh at ~64 per second however fast KCD2 rendered. A microsecond clock keeps the intended 2 ms.
        static std::atomic<std::uint64_t> last_late_us{0};
        const std::uint64_t now_us = static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
        std::uint64_t previous = last_late_us.load(std::memory_order_relaxed);
        if (now_us >= previous + 2000 && last_late_us.compare_exchange_strong(previous, now_us, std::memory_order_acq_rel)) {
            XrPose late{};
            if (XrManager::Instance().SampleLateHeadPose(late)) pose = late;
        }
        tracking59c6::Frame cached{};
        if(tracking59c6::ReadHead(cached)&&cached.valid[0]&&
           tracking59c6::Age(cached.ns,tracking59c6::NowNs(),20000000ull)){
            const auto& p=cached.poses[0];pose={p.x,p.y,p.z,p.qx,p.qy,p.qz,p.qw};
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
            // V64.41 F3=3: test the remaining difference versus native mouse look.
            // Matrix-only injection may leave CView's canonical SViewParams pose stale.
            // Persist the composed pose into rotation/position fields too, but before
            // reading the next native base restore only when the fields still exactly
            // match our previous write. If KCD2 changed them, accept that as the new
            // native base. This avoids blindly overwriting genuine game camera motion.
            const bool history_mode=g_history_persistent_pose.load(std::memory_order_acquire);
            bool history_exact=false;
            __try { history_exact=history_mode && IsExactGameCView(camera); }
            __except(EXCEPTION_EXECUTE_HANDLER) { history_exact=false; }
            const uintptr_t cv=history_exact && camera>=static_cast<uintptr_t>(kSViewParamsViewMatrixOffset)?camera-kSViewParamsViewMatrixOffset:0;
            if(history_exact && cv) {
                auto qclose=[](const Quatf&a,const Quatf&b){ const float d=std::fabs(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w); return d>0.99999f; };
                auto pclose=[](const Vec3f&a,const Vec3f&b){ return std::fabs(a.x-b.x)<1e-4f&&std::fabs(a.y-b.y)<1e-4f&&std::fabs(a.z-b.z)<1e-4f; };
                const Quatf curq=Normalize(*reinterpret_cast<const Quatf*>(cv+kSViewParamsRotationOffset));
                const Vec3f curp=*reinterpret_cast<const Vec3f*>(cv+kSViewParamsPositionOffset);
                if(g_hist_valid && g_hist_cview==cv && qclose(curq,g_hist_written_q) && pclose(curp,g_hist_written_p)) {
                    *reinterpret_cast<Quatf*>(cv+kSViewParamsRotationOffset)=g_hist_native_q;
                    *reinterpret_cast<Vec3f*>(cv+kSViewParamsPositionOffset)=g_hist_native_p;
                } else { g_hist_native_q=curq; g_hist_native_p=curp; g_hist_cview=cv; g_hist_valid=true; }
            }
            const bool applied = ApplyTrackedPose(camera, pose, origin, world_scale,
                                                  s_apply_rotation.load(std::memory_order_relaxed),
                                                  s_apply_position.load(std::memory_order_relaxed),
                                                  s_rotation_filter_mode.load(std::memory_order_acquire),
                                                  s_rotation_filter_generation.load(std::memory_order_acquire),roomOriginD76,
                                                  visualSourceD75,visualGenerationD75,visualFrameD75);
            if (applied) {
                if(history_exact && cv) {
                    auto* mm=reinterpret_cast<Matrix34f*>(camera);
                    g_hist_written_q=MatrixToQuat(*mm);
                    g_hist_written_p={mm->m[0][3],mm->m[1][3],mm->m[2][3]};
                    *reinterpret_cast<Quatf*>(cv+kSViewParamsRotationOffset)=g_hist_written_q;
                    *reinterpret_cast<Vec3f*>(cv+kSViewParamsPositionOffset)=g_hist_written_p;
                }
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
            std::uint64_t applied_pose_frame=0;hj1::Sample applied_family_r24{};
            __try { backup=*reinterpret_cast<Matrix34f*>(camera); backed=true; }
            __except(EXCEPTION_EXECUTE_HANDLER) { backed=false; }
            if (backed) applied=CameraHook::ApplyCurrentPoseToPublishedCamera(
                reinterpret_cast<void*>(camera), &applied_pose_frame, false /* culling only: never retag rendered PRESENT */,
                hj1::enabled.load(std::memory_order_acquire)?&applied_family_r24:nullptr);
            if (applied) {
                CaptureFrustumRotationSample(camera,applied_pose_frame);
                CaptureFrustumPosedMatrix(camera,applied_pose_frame,hj1::enabled.load(std::memory_order_acquire)?&applied_family_r24:nullptr);
                const uintptr_t result=V6659CallWidened(camera,s_cull_frustum_scale.load(std::memory_order_acquire),
                    [](std::uintptr_t c)->std::uintptr_t{ return s_original(c); });
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
        const float v6659=s_cull_frustum_scale.load(std::memory_order_acquire);
        if(v6659>1.0001f) {
            bool gv=false; __try { gv=IsExactGameCView(camera); } __except(EXCEPTION_EXECUTE_HANDLER) { gv=false; }
            if(gv) return V6659CallWidened(camera,v6659,[](std::uintptr_t c)->std::uintptr_t{ return s_original(c); });
        }
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
