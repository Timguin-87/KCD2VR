#pragma once

#include <openxr/openxr.h>
#include "camera_hook.h"

namespace kcd2vr {

struct ControllerState {
    bool active{false};
    XrPose pose{};
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
};

class XrInput {
public:
    bool Initialize(XrInstance instance, XrSession session, XrSpace reference_space);
    void Shutdown();
    bool Sync(XrTime display_time);

    const ControllerState& Left() const { return left_; }
    const ControllerState& Right() const { return right_; }
    bool RecenterRequested() const { return recenter_requested_; }
    void ClearRecenterRequest() { recenter_requested_ = false; }

private:
    bool CreateActions();
    bool SuggestBindings();
    bool CreateSpaces();
    bool CreateActionSpace(XrAction action, XrPath subaction_path, XrSpace* out_space);
    bool GetPose(XrSpace space, XrTime display_time, XrPose& out_pose, bool& active);
    bool GetFloat(XrAction action, XrPath subaction_path, float& value);
    bool GetVec2(XrAction action, XrPath subaction_path, float& x, float& y);
    bool GetBool(XrAction action, XrPath subaction_path, bool& value);
    bool GetBoolState(XrAction action, XrPath subaction_path, bool& down, bool& pressed);
    XrPath Path(const char* value) const;

    XrInstance instance_{XR_NULL_HANDLE};
    XrSession session_{XR_NULL_HANDLE};
    XrSpace reference_space_{XR_NULL_HANDLE};
    XrActionSet action_set_{XR_NULL_HANDLE};
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
};

} // namespace kcd2vr
