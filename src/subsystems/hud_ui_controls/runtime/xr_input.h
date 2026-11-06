#pragma once

#include <openxr/openxr.h>
#include "camera_hook.h"
#include <mutex>

namespace kcd2vr {

struct ControllerState {
    bool active{false};
    XrPose pose{};
    XrSpaceLocationFlags pose_flags{0}; // exact grip locate provenance; active retains its valid-bit semantics
    XrSpaceVelocity velocityD69{XR_TYPE_SPACE_VELOCITY};
    XrPose aim_pose{};
    bool aim_active{false};
    float trigger{0.0f};
    float grip{0.0f};
    float thumb_x{0.0f};
    float thumb_y{0.0f};
    bool a_down{false};
    bool b_down{false};
    bool x_down{false};
    bool y_down{false};
    bool menu_down{false};
    bool thumbstick_down{false};
    bool a_pressed{false};
    bool b_pressed{false};
    bool x_pressed{false};
    bool y_pressed{false};
    bool menu_pressed{false};
    bool thumbstick_pressed{false};
    // Selected capacitive modifier: default thumbrest, or explicit secondary-face touch.
    bool quick_access_touch{false};
    bool quick_access_touch_valid{false}; // successful current active selected touch action
    // R59.16C5: index finger resting on the trigger (capacitive). Defaults to TRUE so a runtime without the
    // binding can never produce a false 'pointing' gesture.
    bool trigger_touch{true};
    bool trigger_touch_valid{false}; // current optional capacitive action, not its fallback value
    bool b_valid{false}; // current B action read; only the explicit 2s Escape recovery requires this
    bool grip_valid{false}; // CG1 only: successful current active squeeze action, independent of pose
    bool trigger_valid{false}; // D59: validity metadata only; existing trigger values/mappings are unchanged.
};

class XrInput {
public:
    bool Initialize(XrInstance instance, XrSession session, XrSpace reference_space);
    void Shutdown();
    bool Sync(XrTime display_time);
    // R105: retire an unavailable frame without rebuilding its attached actions.
    // No OpenXR calls; the next successful Sync restores current input directly.
    void InvalidateFrameR105();
    // Retire local output receipts before an ended/lost session; no OpenXR calls.
    void OnSessionStoppedR105();
    // Pose-only read: no xrSyncActions, button mutation, recenter, or haptics.
    // false means the action-space lock was busy/unavailable; locations can
    // individually fail and are then returned with zero validity flags.
    bool TryLocateGripPair(XrTime time, XrPosef (&poses)[2], XrSpaceLocationFlags (&flags)[2], XrSpaceVelocity* velocitiesD69=nullptr);

    const ControllerState& Left() const { return left_; }
    const ControllerState& Right() const { return right_; }
    bool RecenterRequested() const { return recenter_requested_; }
    void ClearRecenterRequest() { recenter_requested_ = false; }

private:
    bool CreateActions();
    void ServiceHaptics(bool focused);
    bool SuggestBindings();
    bool CreateSpaces();
    bool CreateActionSpace(XrAction action, XrPath subaction_path, XrSpace* out_space);
    bool GetPose(XrSpace space, XrTime display_time, XrPose& out_pose, bool& active, XrSpaceLocationFlags* flags=nullptr, XrSpaceVelocity* velocityD69=nullptr);
    bool GetFloat(XrAction action, XrPath subaction_path, float& value, bool* active_out=nullptr);
    bool GetVec2(XrAction action, XrPath subaction_path, float& x, float& y);
    bool GetBoolState(XrAction action, XrPath subaction_path, bool& down, bool& pressed, bool* active_out=nullptr);
    XrPath Path(const char* value) const;

    XrInstance instance_{XR_NULL_HANDLE};
    XrSession session_{XR_NULL_HANDLE};
    XrSpace reference_space_{XR_NULL_HANDLE};
    XrActionSet action_set_{XR_NULL_HANDLE};
    XrAction haptic_action_{XR_NULL_HANDLE};
    std::uint64_t haptic_seen_[2]{},haptic_epoch_[2]{};
    bool haptic_active_[2]{};
    XrAction pose_action_{XR_NULL_HANDLE};
    XrAction aim_pose_action_{XR_NULL_HANDLE};
    XrAction trigger_action_{XR_NULL_HANDLE};
    XrAction grip_action_{XR_NULL_HANDLE};
    XrAction thumbstick_action_{XR_NULL_HANDLE};
    XrAction recenter_action_{XR_NULL_HANDLE};
    XrAction a_action_{XR_NULL_HANDLE};
    XrAction b_action_{XR_NULL_HANDLE};
    XrAction x_action_{XR_NULL_HANDLE};
    XrAction y_action_{XR_NULL_HANDLE};
    XrAction menu_action_{XR_NULL_HANDLE};
    XrAction thumbstick_click_action_{XR_NULL_HANDLE};
    XrAction quick_access_touch_action_{XR_NULL_HANDLE};
    XrAction quick_access_face_touch_action_r112_{XR_NULL_HANDLE}; // optional Y/B or Index B touch
    XrAction trigger_touch_action_{XR_NULL_HANDLE}; // R59.16C5 optional

    XrPath left_path_{XR_NULL_PATH};
    XrPath right_path_{XR_NULL_PATH};
    XrSpace left_space_{XR_NULL_HANDLE};
    XrSpace right_space_{XR_NULL_HANDLE};
    XrSpace left_aim_space_{XR_NULL_HANDLE};
    XrSpace right_aim_space_{XR_NULL_HANDLE};

    ControllerState left_{};
    ControllerState right_{};
    bool recenter_requested_{false};
    bool initialized_{false};
    bool focused_{false};
    // R102: interaction profile actually bound by the runtime (logged once per change).
    XrPath bound_profile_r102_[2]{XR_NULL_PATH,XR_NULL_PATH};
    XrResult profile_result_r112_[2]{XR_SUCCESS,XR_SUCCESS};
    unsigned long long profile_check_tick_r102_{0};
    int profile_reports_r102_{0};
    std::mutex action_space_mutex_;
};

} // namespace kcd2vr
