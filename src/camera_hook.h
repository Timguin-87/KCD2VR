#pragma once

#include <DetourModKit/hook.hpp>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <optional>

namespace kcd2vr {

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
    static void SetHeadPoseForFrame(const XrPose& pose, float scale, std::uint64_t frame_index);
    static void RequestRecenter();
    static void SetTrackingOptions(bool apply_rotation, bool apply_position);
    static void SetAfrStereoEnabled(bool enabled);
    static void SetAfrEye(int eye);
    static int GetAfrEye();
    static void SetAfrIpd(float ipd_meters, float scale = 1.0f);
    // V60.4: widen ONLY the visibility frustum while CCamera::UpdateFrustumPlanes runs.
    // The camera FOV field is restored immediately after the original function returns.
    static void SetCullFrustumScale(float scale);
    static float GetCullFrustumScale();
    // V60.8: fixed local-space camera calibration independent of tracked head translation.
    // TPV-style local offsets: positive side=right, positive back=opposite local forward, positive height=up.
    static void SetCameraCalibrationOffset(float side_meters, float back_meters, float height_meters);
    static float GetCameraSideOffsetMeters();
    static float GetCameraBackOffsetMeters();
    static float GetCameraHeightOffsetMeters();
    static float GetAfrEyeOffsetMeters();
    // V46.9 timing audit: last tick when the frustum hook consumed an XR pose.
    static std::uint64_t LastPoseApplyTickMs();
    static std::uint64_t LastPoseFrameIndex();
    static std::uint64_t CurrentPoseFrameIndex();
    // V61.7 read-only culling timing probe: latest orientation actually handed to UpdateFrustumPlanes.
    static bool GetLatestFrustumRotationSample(CameraRotationSample& out);
    // V61.44 FRAME NEXUS: if the exact gameplay CView was just posed for the
    // culling frustum, reuse that exact full Matrix34 at PublishViewCamera. This
    // guarantees culling and rendered pixels use one immutable pose snapshot even
    // if xrWaitFrame publishes a newer HMD pose between the two engine calls.
    static bool ApplyLatestCullingPoseToPublishedCamera(void* camera, std::uint64_t* out_pose_frame = nullptr,
                                                        std::uint64_t max_age_ms = 24);
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
                                                  bool record_render_identity = true);
    // V53 diagnostics: pose source 0=cached XR snapshot, 1=sync latest predictedDisplayTime,
    // 2=sync QPC-now, 3=sync latest predictedDisplayTime + offset ms.
    static void SetPoseSourceMode(int mode, float prediction_offset_ms = 0.0f);
    static int GetPoseSourceMode();
    // Rotation composition: 0=base*HMD(local), 1=HMD*base(world diagnostic).
    static void SetRotationOrder(int order);
    static int GetRotationOrder();
    // V56 rotation filter diagnostics: 0 raw, 3/4/5/6 = 4/6/8/10ms SLERP smoothing, 7 = adaptive smoothing.
    static void SetRotationFilterMode(int mode);
    static int GetRotationFilterMode();
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
    static XrPose s_origin_pose;
    static std::atomic<float> s_current_scale;
    static std::atomic<std::uint64_t> s_current_pose_frame_index;
    static std::atomic<std::uint64_t> s_last_applied_pose_frame_index;
    static std::atomic<bool> s_pose_received;
    static std::atomic<bool> s_origin_valid;
    static std::atomic<bool> s_recenter_requested;
    static std::atomic<bool> s_apply_rotation;
    static std::atomic<bool> s_apply_position;
    static std::atomic<bool> s_afr_enabled;
    static std::atomic<int> s_afr_eye;
    static std::atomic<float> s_afr_ipd_meters;
    static std::atomic<float> s_afr_ipd_scale;
    static std::atomic<float> s_cull_frustum_scale;
    static std::atomic<float> s_camera_side_offset_m;
    static std::atomic<float> s_camera_back_offset_m;
    static std::atomic<float> s_camera_height_offset_m;
    static std::atomic<int> s_injection_mode;
    static std::atomic<int> s_pose_source_mode;
    static std::atomic<float> s_prediction_offset_ms;
    static std::atomic<int> s_rotation_order;
    static std::atomic<int> s_rotation_filter_mode;
    static std::atomic<std::uint64_t> s_rotation_filter_generation;
    static std::atomic<float> s_worst_gap_deg;
    static std::atomic<float> s_worst_hmd_deg;
    static std::atomic<float> s_worst_final_deg;
    static std::atomic<std::uint64_t> s_worst_n;
    static std::atomic<float> s_gameplay_fov_rad;
    static std::atomic<std::uint64_t> s_gameplay_fov_samples;
};

} // namespace kcd2vr
