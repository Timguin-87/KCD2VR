#pragma once

#include <DetourModKit/hook.hpp>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <optional>
#include "vrik/roomscale58.h"
#include "vrik/tracking_origin125.h"
#include "vrik/visual_ray_d75.h"

namespace kcd2vr {
namespace hj1 {struct Sample;}

struct CameraRotationSample {
    float m[9]{};
    std::uint64_t pose_frame{0};
    std::uint64_t tick_ms{0};
    std::uintptr_t camera{0};
};

struct XrPose {
    float pos_x{0.0f};
    float pos_y{0.0f};
    float pos_z{0.0f};
    float qx{0.0f};
    float qy{0.0f};
    float qz{0.0f};
    float qw{1.0f};
};

class CameraHook {
public:
    using FrustumBuildFunc = uintptr_t(__fastcall*)(uintptr_t);

    CameraHook() = default;
    ~CameraHook() { Uninstall(); }

    bool Install();
    void Uninstall();

    static void SetHeadPose(const XrPose& pose, float scale = 1.0f);
    static void SetHeadPoseForFrame(const XrPose& pose, float scale, std::uint64_t frame_index, const hj1::Sample* family = nullptr);
    static std::uint64_t CurrentFamilyGenerationR24() noexcept;
    static void RequestRecenter();
    // R112: the raw OpenXR LOCAL-space head pose last handed to the camera (the runtime center is 0,0,0). false = none yet.
    static bool CopyHeadPoseR112(XrPose& out) noexcept;
    // Copy the same room reference used by the view; never a final/previous camera pose.
    static bool ReadTrackingOrigin125(tracking125::OriginVisitorH46 visitor,void* context) noexcept;
    // V66.178R58 roomscale follow (vrik/roomscale58.h): the entity walks under the player; the anchor keeps the view still.
    static void SetRoomscaleFollow(bool enabled);
    static bool RoomscaleRemaining(float& x,float& y,std::uint64_t& tick);
    static kcd2vr::roomscale58::Drive RoomscaleDrive(std::uint64_t now,bool follow,bool combatLocked,bool dodgeEnabled);
    static void RoomscaleCounters(std::uint64_t& consumed,std::uint64_t& dodges,std::uint64_t& injected,float& anchorX,float& anchorY);
    static void SetTrackingOptions(bool apply_rotation, bool apply_position);
    static void SetNativeHeadRotationOwner(bool active);
    static void SetAfrStereoEnabled(bool enabled);
    static void SetAfrEye(int eye);
    static int GetAfrEye();
    static void SetAfrIpd(float ipd_meters, float scale = 1.0f);
    // V60.4: widen ONLY the visibility frustum while CCamera::UpdateFrustumPlanes runs.
    // The camera FOV field is restored immediately after the original function returns.
    static void SetCullFrustumScale(float scale);
    // V60.8: fixed local-space camera calibration independent of tracked head translation.
    // TPV-style local offsets: positive side=right, positive back=opposite local forward, positive height=up.
    static void SetCameraCalibrationOffset(float side_meters, float back_meters, float height_meters);
    static float GetCameraSideOffsetMeters();
    static float GetCameraBackOffsetMeters();
    static float GetCameraHeightOffsetMeters();
    static float GetAfrEyeOffsetMeters();
    // R150: eye whose AFR IPD offset was actually written into the last camera
    // posed on THIS thread (fresh pose, or the reused culling pose), -1 if none.
    // The culling pose is posed before the publish detour runs, so the AFR eye
    // selected at publish time is not necessarily the eye of the rendered camera.
    static int LastAppliedAfrEye() noexcept;
    static void ClearAppliedAfrEye() noexcept;
    // R154: signed world offset along camera right written with that eye (0 if none).
    static float LastAppliedAfrEyeOffsetWorld() noexcept;
    // GetAfrEyeOffsetMeters() that also records the eye for LastAppliedAfrEye().
    static float GetAfrEyeOffsetMetersApplied() noexcept;
    // V46.9 timing audit: last tick when the frustum hook consumed an XR pose.
    static std::uint64_t LastPoseApplyTickMs();
    static std::uint64_t LastPoseFrameIndex();
    static std::uint64_t CurrentPoseFrameIndex();
    // V61.7 read-only culling timing probe: latest orientation actually handed to UpdateFrustumPlanes.
    static bool GetLatestFrustumRotationSample(CameraRotationSample& out);
    // Exact final camera basis recorded with a rendered XR pose frame.
    static bool GetCameraRotationForPoseFrame(std::uint64_t pose_frame,CameraRotationSample& out);
    // V64.38: exact world-forward direction of the final published OpenXR-composed camera.
    // Used by interaction alignment so gameplay queries can follow the headset without
    // making native camera timing part of the rendered view. Fails closed when stale.
    static bool GetLatestVisualForwardWorld(float& x, float& y, float& z, std::uint64_t max_age_ms = 100);
    // V66.161: pitch of the untouched NATIVE camera (aim), radians up-positive. Invisible in VR
    // because the visual view strips native pitch; used to steer native aim pitch in VRIK.
    static bool GetNativeCameraPitch(float& pitch_rad, std::uint64_t max_age_ms = 100);
    // V65.09 F5: exact final published OpenXR camera/HMD world position. This is
    // the same matrix translation rendered by PublishViewCamera, not a native
    // Henry head-bone approximation.
    static bool GetLatestVisualPositionWorld(float& x, float& y, float& z, std::uint64_t max_age_ms = 100);
    // R142 direct hands use the placed cyclopean camera center: it includes
    // calibration offsets and roomscale, while excluding per-eye IPD.
    static bool GetLatestVisualCenterPositionWorld(float& x,float& y,float& z,std::uint64_t max_age_ms=100);
    // Read-only completed center-head ray for station labels, copied together
    // with its tracking/camera provenance. Never a native action selector.
    static bool GetLatestVisualRayD75(visual_ray_d75::Sample& out,std::uint64_t max_age_ms = 80);
    static void PublishVisualForwardFromCameraMatrix(const void* camera_matrix);
    static void PublishVisualPositionFromCameraMatrix(const void* camera_matrix);
    static void PublishVisualCenterPositionWorld(const float* xyz) noexcept;
    // V61.44 FRAME NEXUS: if the exact gameplay CView was just posed for the
    // culling frustum, reuse that exact full Matrix34 at PublishViewCamera. This
    // guarantees culling and rendered pixels use one immutable pose snapshot even
    // if xrWaitFrame publishes a newer HMD pose between the two engine calls.
    static bool ApplyLatestCullingPoseToPublishedCamera(void* camera, std::uint64_t* out_pose_frame = nullptr,
                                                        std::uint64_t max_age_ms = 24, hj1::Sample* out_family = nullptr);
    // V61.28: exact gameplay-CView FOV calibration. Captured only at the proven
    // gameplay frustum boundary; auxiliary/dialogue/RTT cameras are excluded.
    static float LatestGameplayFovRadians();
    static float LatestGameplayFovDegrees();
    static std::uint64_t GameplayFovSampleCount();
    // V51 tracking rebuild: 0 = legacy frustum injection, 1 = reversible PublishViewCamera injection.
    static void SetInjectionMode(int mode);
    static int GetInjectionMode();
    // V61.44: record_render_identity must be false for the temporary V61.12 culling-only pose apply.
    // Only the real PublishViewCamera path may advance LastPoseFrameIndex()/LastPoseApplyTickMs().
    static bool ApplyCurrentPoseToPublishedCamera(void* camera, std::uint64_t* out_pose_frame = nullptr,
                                                  bool record_render_identity = true, hj1::Sample* out_family = nullptr);
    // V53 diagnostics: pose source 0=cached XR snapshot, 1=sync latest predictedDisplayTime,
    // 2=sync QPC-now, 3=sync latest predictedDisplayTime + offset ms.
    static void SetPoseSourceMode(int mode, float prediction_offset_ms = 0.0f);
    static int GetPoseSourceMode();
    // Rotation composition: 0=base*HMD(local), 1=HMD*base(world diagnostic).
    static void SetRotationOrder(int order);
    // V64.37 F3 view-marriage lab: keep native aim/interactions active while
    // choosing how the published VR view removes the native head-look component.
    // 0=off/native behavior, 1=shadow native + OpenXR (no cancellation),
    // 2=cancel estimated native yaw, 3=cancel estimated native yaw+pitch.
    static void SetViewMarriageMode(int mode, float native_gain = 1.0f);
    // V64.39: F3=1 fresh-publish test. When enabled, PublishViewCamera skips
    // the up-to-24ms culling-pose latch and samples the configured XR pose source
    // exactly at the publish boundary.
    static void SetFreshPublishPose(bool enabled);
    static bool GetFreshPublishPose();
    // V64.41 F3=3 history-state lab: persist the OpenXR-composed pose into the
    // exact gameplay CView SViewParams rotation/position fields as well as its
    // camera matrix. Guarded restoration prevents HMD accumulation across frames.
    static void SetHistoryPersistentPose(bool enabled);
    // V56 rotation filter diagnostics: 0 raw, 3/4/5/6 = 4/6/8/10ms SLERP smoothing, 7 = adaptive smoothing.
    static void SetRotationFilterMode(int mode);
    // V65.03 F2 only: enables the old per-frame angular instrumentation.
    // OFF in normal play so diagnostics cannot perturb head timing.
    static void SetAngularDiagnosticsEnabled(bool enabled);
    // V64.22B native head-aim bridge: CameraHook publishes the recentered local
    // HMD yaw/pitch and accepts the portion transferred into KCD2's native
    // look controller. The transferred rotation is removed from the HMD camera
    // delta so visual head tracking remains one-to-one (no double-rotation).
    static bool GetHeadAimLocalAngles(float& yaw_rad, float& pitch_rad);
    // V64.19 gated F5 dialogue camera lab. mode -1/0 does not alter the published
    // camera. Modes 1..3 use the last real pre-dialogue PublishViewCamera base
    // as a reversible VR anchor while KCD2 dialogue logic keeps running.
    static void SetDialogueLabState(int mode, bool dialogue_active, int native_camera_type);
    static void SetDialogueEmbodiment59c10r2(bool allowed,std::uint64_t now);
    static void ReportDialogueEmbodiment59c10r2(std::uint64_t now);
    static bool DialogueEmbodimentApplied59c13(std::uint64_t now); // R59.16C10R13: the last render publish rebuilt the eye from Henry's head
    static void SetDialogueDefaultEye59c13(bool on);
    static void SetDialogueSmooth38(bool on); // C38 INI/menu VrDialogueSmoothCamera (dialogue anchor low-pass + late-sample hold)
    static bool DialogueSmooth38();
    // V53 review addition: worst-case angular divergence tracking, since the
    // existing V53-ANGULAR log only samples ~3% of frames after the first 20
    // - easily enough to miss a rare, transient spike. Updated every frame
    // in FrustumDetour (cheap - a few float compares), reset at the start of
    // each F8 phase, read+logged at phase end so an outlier is captured
    // regardless of whether its specific frame happened to be logged.
    static void ResetWorstCaseAngular();
    static void GetWorstCaseAngular(float& out_gap_deg, float& out_hmd_deg, float& out_final_deg, std::uint64_t& out_n);

private:
    static uintptr_t __fastcall FrustumDetour(uintptr_t camera) noexcept;

    std::optional<DetourModKit::hook::Hook> hook_;
    static FrustumBuildFunc s_original;
    static std::mutex s_pose_mutex;
    static XrPose s_current_pose;
    static hj1::Sample s_current_family_r24;
    static XrPose s_origin_pose;
    static std::atomic<float> s_current_scale;
    static std::atomic<std::uint64_t> s_current_pose_frame_index;
    static std::atomic<std::uint64_t> s_last_applied_pose_frame_index;
    static std::atomic<bool> s_pose_received;
    static std::atomic<bool> s_origin_valid;
    static std::atomic<bool> s_recenter_requested;
    static std::atomic<bool> s_apply_rotation;
    static std::atomic<bool> s_native_head_rotation_owner;
    static std::atomic<bool> s_apply_position;
    static std::atomic<bool> s_afr_enabled;
    static std::atomic<int> s_afr_eye;
    static std::atomic<float> s_afr_ipd_meters;
    static std::atomic<float> s_afr_ipd_scale;
    static std::atomic<float> s_cull_frustum_scale;
public:
private:
    static std::atomic<float> s_camera_side_offset_m;
    static std::atomic<float> s_camera_back_offset_m;
    static std::atomic<float> s_camera_height_offset_m;
    static std::atomic<int> s_injection_mode;
    static std::atomic<int> s_pose_source_mode;
    static std::atomic<float> s_prediction_offset_ms;
    static std::atomic<int> s_rotation_order;
    static std::atomic<int> s_view_marriage_mode;
    static std::atomic<bool> s_fresh_publish_pose;
    static std::atomic<float> s_view_marriage_native_gain;
    static std::atomic<int> s_rotation_filter_mode;
    static std::atomic<float> s_head_aim_local_yaw_rad;
    static std::atomic<float> s_head_aim_local_pitch_rad;
    static std::atomic<bool> s_head_aim_local_valid;
    static std::atomic<float> s_head_aim_transferred_yaw_rad;
    static std::atomic<float> s_head_aim_transferred_pitch_rad;
    static std::atomic<int> s_dialogue_lab_mode;
    static std::atomic<bool> s_dialogue_lab_active;
    static std::atomic<int> s_dialogue_native_camera_type;
    static std::atomic<std::uint64_t> s_rotation_filter_generation;
    static std::atomic<float> s_worst_gap_deg;
    static std::atomic<float> s_worst_hmd_deg;
    static std::atomic<float> s_worst_final_deg;
    static std::atomic<std::uint64_t> s_worst_n;
    static std::atomic<float> s_gameplay_fov_rad;
    static std::atomic<std::uint64_t> s_gameplay_fov_samples;
    static std::atomic<float> s_visual_forward_x;
    static std::atomic<float> s_visual_forward_y;
    static std::atomic<float> s_visual_forward_z;
    static std::atomic<std::uint64_t> s_visual_forward_tick_ms;
    static std::atomic<bool> s_visual_forward_valid;
    static std::atomic<float> s_visual_position_x;
    static std::atomic<float> s_visual_position_y;
    static std::atomic<float> s_visual_position_z;
    static std::atomic<std::uint64_t> s_visual_position_tick_ms;
    static std::atomic<bool> s_visual_position_valid;
    static std::atomic<float> s_visual_center_x;
    static std::atomic<float> s_visual_center_y;
    static std::atomic<float> s_visual_center_z;
    static std::atomic<std::uint64_t> s_visual_center_tick_ms;
    static std::atomic<bool> s_visual_center_valid;
};

} // namespace kcd2vr
