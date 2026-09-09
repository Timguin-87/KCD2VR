#include "xr_input.h"
#include <DetourModKit/logger.hpp>
#include <array>
#include <cstdio>

namespace kcd2vr {
using DetourModKit::log;

XrPath XrInput::Path(const char* value) const
{
    XrPath path = XR_NULL_PATH;
    if (XR_FAILED(xrStringToPath(instance_, value, &path))) return XR_NULL_PATH;
    return path;
}

bool XrInput::Initialize(XrInstance instance, XrSession session, XrSpace reference_space)
{
    instance_ = instance;
    session_ = session;
    reference_space_ = reference_space;
    left_path_ = Path("/user/hand/left");
    right_path_ = Path("/user/hand/right");
    if (left_path_ == XR_NULL_PATH || right_path_ == XR_NULL_PATH) return false;
    if (!CreateActions() || !SuggestBindings() || !CreateSpaces()) return false;

    XrSessionActionSetsAttachInfo attach{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};
    attach.countActionSets = 1;
    attach.actionSets = &action_set_;
    XrResult r = xrAttachSessionActionSets(session_, &attach);
    if (XR_FAILED(r)) {
        log().error("OpenXR: xrAttachSessionActionSets failed, code={}", static_cast<int>(r));
        return false;
    }
    initialized_ = true;
    log().info("OpenXR input initialized: grip + aim poses and left/right tracked controller actions are ready");
    return true;
}

bool XrInput::CreateActions()
{
    XrActionSetCreateInfo set{XR_TYPE_ACTION_SET_CREATE_INFO};
    std::snprintf(set.actionSetName, XR_MAX_ACTION_SET_NAME_SIZE, "kcd2vr");
    std::snprintf(set.localizedActionSetName, XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE, "KCD2VR");
    set.priority = 0;
    if (XR_FAILED(xrCreateActionSet(instance_, &set, &action_set_))) return false;

    const XrPath subpaths[] = {left_path_, right_path_};
    auto create = [&](const char* name, const char* localized, XrActionType type, XrAction* out) {
        XrActionCreateInfo info{XR_TYPE_ACTION_CREATE_INFO};
        std::snprintf(info.actionName, XR_MAX_ACTION_NAME_SIZE, "%s", name);
        std::snprintf(info.localizedActionName, XR_MAX_LOCALIZED_ACTION_NAME_SIZE, "%s", localized);
        info.actionType = type;
        info.countSubactionPaths = 2;
        info.subactionPaths = subpaths;
        return xrCreateAction(action_set_, &info, out) == XR_SUCCESS;
    };

    return create("hand_pose", "Hand Pose", XR_ACTION_TYPE_POSE_INPUT, &pose_action_) &&
           create("aim_pose", "Aim Pose", XR_ACTION_TYPE_POSE_INPUT, &aim_pose_action_) &&
           create("trigger", "Trigger", XR_ACTION_TYPE_FLOAT_INPUT, &trigger_action_) &&
           create("grip", "Grip", XR_ACTION_TYPE_FLOAT_INPUT, &grip_action_) &&
           create("thumbstick", "Thumbstick", XR_ACTION_TYPE_VECTOR2F_INPUT, &thumbstick_action_) &&
           create("recenter", "Recenter", XR_ACTION_TYPE_BOOLEAN_INPUT, &recenter_action_) &&
           create("button_a", "A Button", XR_ACTION_TYPE_BOOLEAN_INPUT, &a_action_) &&
           create("button_b", "B Button", XR_ACTION_TYPE_BOOLEAN_INPUT, &b_action_) &&
           create("button_x", "X Button", XR_ACTION_TYPE_BOOLEAN_INPUT, &x_action_) &&
           create("button_y", "Y Button", XR_ACTION_TYPE_BOOLEAN_INPUT, &y_action_) &&
           create("menu", "Menu Button", XR_ACTION_TYPE_BOOLEAN_INPUT, &menu_action_) &&
           create("thumbstick_click", "Thumbstick Click", XR_ACTION_TYPE_BOOLEAN_INPUT, &thumbstick_click_action_);
}

bool XrInput::SuggestBindings()
{
    const XrPath profile = Path("/interaction_profiles/oculus/touch_controller");
    if (profile == XR_NULL_PATH) return false;

    // V58 Touch profile. Keep locomotion and gameplay emulation separate from
    // OpenXR action acquisition: XrInput only publishes controller state; the
    // mod control thread decides whether/how to emulate KCD2 keyboard/mouse.
    const std::array<XrActionSuggestedBinding, 17> bindings{{
        {pose_action_, Path("/user/hand/left/input/grip/pose")},
        {pose_action_, Path("/user/hand/right/input/grip/pose")},
        {aim_pose_action_, Path("/user/hand/left/input/aim/pose")},
        {aim_pose_action_, Path("/user/hand/right/input/aim/pose")},
        {trigger_action_, Path("/user/hand/left/input/trigger/value")},
        {trigger_action_, Path("/user/hand/right/input/trigger/value")},
        {grip_action_, Path("/user/hand/left/input/squeeze/value")},
        {grip_action_, Path("/user/hand/right/input/squeeze/value")},
        {thumbstick_action_, Path("/user/hand/left/input/thumbstick")},
        {thumbstick_action_, Path("/user/hand/right/input/thumbstick")},
        {thumbstick_click_action_, Path("/user/hand/left/input/thumbstick/click")},
        {thumbstick_click_action_, Path("/user/hand/right/input/thumbstick/click")},
        {x_action_, Path("/user/hand/left/input/x/click")},
        {y_action_, Path("/user/hand/left/input/y/click")},
        {menu_action_, Path("/user/hand/left/input/menu/click")},
        {a_action_, Path("/user/hand/right/input/a/click")},
        {b_action_, Path("/user/hand/right/input/b/click")}
    }};
    for (const auto& b : bindings) if (b.binding == XR_NULL_PATH) return false;

    XrInteractionProfileSuggestedBinding info{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};
    info.interactionProfile = profile;
    info.suggestedBindings = bindings.data();
    info.countSuggestedBindings = static_cast<uint32_t>(bindings.size());
    XrResult r = xrSuggestInteractionProfileBindings(instance_, &info);
    if (XR_FAILED(r)) {
        log().warning("OpenXR: Oculus Touch bindings could not be suggested, code={}", static_cast<int>(r));
        // Do not fail VR just because a runtime chooses not to accept this optional profile.
    }
    return true;
}

bool XrInput::CreateActionSpace(XrAction action, XrPath subaction_path, XrSpace* out_space)
{
    XrActionSpaceCreateInfo info{XR_TYPE_ACTION_SPACE_CREATE_INFO};
    info.action = action;
    info.subactionPath = subaction_path;
    info.poseInActionSpace.orientation.w = 1.0f;
    return xrCreateActionSpace(session_, &info, out_space) == XR_SUCCESS;
}

bool XrInput::CreateSpaces()
{
    return CreateActionSpace(pose_action_, left_path_, &left_space_) &&
           CreateActionSpace(pose_action_, right_path_, &right_space_) &&
           CreateActionSpace(aim_pose_action_, left_path_, &left_aim_space_) &&
           CreateActionSpace(aim_pose_action_, right_path_, &right_aim_space_);
}

bool XrInput::GetPose(XrSpace space, XrTime display_time, XrPose& out_pose, bool& active)
{
    XrSpaceLocation location{XR_TYPE_SPACE_LOCATION};
    XrResult r = xrLocateSpace(space, reference_space_, display_time, &location);
    if (XR_FAILED(r)) return false;
    active = (location.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) &&
             (location.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT);
    if (!active) return true;
    out_pose.pos_x = location.pose.position.x;
    out_pose.pos_y = location.pose.position.y;
    out_pose.pos_z = location.pose.position.z;
    out_pose.qx = location.pose.orientation.x;
    out_pose.qy = location.pose.orientation.y;
    out_pose.qz = location.pose.orientation.z;
    out_pose.qw = location.pose.orientation.w;
    return true;
}

bool XrInput::GetFloat(XrAction action, XrPath subaction_path, float& value)
{
    XrActionStateGetInfo info{XR_TYPE_ACTION_STATE_GET_INFO};
    info.action = action; info.subactionPath = subaction_path;
    XrActionStateFloat state{XR_TYPE_ACTION_STATE_FLOAT};
    if (XR_FAILED(xrGetActionStateFloat(session_, &info, &state))) return false;
    value = state.isActive ? state.currentState : 0.0f;
    return true;
}

bool XrInput::GetVec2(XrAction action, XrPath subaction_path, float& x, float& y)
{
    XrActionStateGetInfo info{XR_TYPE_ACTION_STATE_GET_INFO};
    info.action = action; info.subactionPath = subaction_path;
    XrActionStateVector2f state{XR_TYPE_ACTION_STATE_VECTOR2F};
    if (XR_FAILED(xrGetActionStateVector2f(session_, &info, &state))) return false;
    x = state.isActive ? state.currentState.x : 0.0f;
    y = state.isActive ? state.currentState.y : 0.0f;
    return true;
}

bool XrInput::GetBool(XrAction action, XrPath subaction_path, bool& value)
{
    XrActionStateGetInfo info{XR_TYPE_ACTION_STATE_GET_INFO};
    info.action = action; info.subactionPath = subaction_path;
    XrActionStateBoolean state{XR_TYPE_ACTION_STATE_BOOLEAN};
    if (XR_FAILED(xrGetActionStateBoolean(session_, &info, &state))) return false;

    // Recenter is a one-shot action. Only report the rising edge instead of
    // returning true for every frame while the controller button is held.
    value = state.isActive && state.currentState && state.changedSinceLastSync;
    return true;
}

bool XrInput::GetBoolState(XrAction action, XrPath subaction_path, bool& down, bool& pressed)
{
    XrActionStateGetInfo info{XR_TYPE_ACTION_STATE_GET_INFO};
    info.action = action; info.subactionPath = subaction_path;
    XrActionStateBoolean state{XR_TYPE_ACTION_STATE_BOOLEAN};
    if (XR_FAILED(xrGetActionStateBoolean(session_, &info, &state))) return false;
    down = state.isActive && state.currentState;
    pressed = down && state.changedSinceLastSync;
    return true;
}

bool XrInput::Sync(XrTime display_time)
{
    if (!initialized_) return false;
    XrActiveActionSet active{action_set_, XR_NULL_PATH};
    XrActionsSyncInfo sync{XR_TYPE_ACTIONS_SYNC_INFO};
    sync.countActiveActionSets = 1;
    sync.activeActionSets = &active;
    if (XR_FAILED(xrSyncActions(session_, &sync))) return false;

    bool is_active = false;
    GetPose(left_space_, display_time, left_.pose, is_active); left_.active = is_active;
    GetPose(right_space_, display_time, right_.pose, is_active); right_.active = is_active;
    GetPose(left_aim_space_, display_time, left_.aim_pose, is_active); left_.aim_active = is_active;
    GetPose(right_aim_space_, display_time, right_.aim_pose, is_active); right_.aim_active = is_active;
    GetFloat(trigger_action_, left_path_, left_.trigger);
    GetFloat(trigger_action_, right_path_, right_.trigger);
    GetFloat(grip_action_, left_path_, left_.grip);
    GetFloat(grip_action_, right_path_, right_.grip);
    GetVec2(thumbstick_action_, left_path_, left_.thumb_x, left_.thumb_y);
    GetVec2(thumbstick_action_, right_path_, right_.thumb_x, right_.thumb_y);
    // V58: X/Y/Menu and stick-clicks are gameplay inputs now. Recenter is no
    // longer bound to X/Y; leave the explicit action available for a future
    // chord without stealing user buttons from the gameplay profile.
    recenter_requested_ = false;

    GetBoolState(a_action_, right_path_, right_.a_down, right_.a_pressed);
    GetBoolState(b_action_, right_path_, right_.b_down, right_.b_pressed);
    GetBoolState(x_action_, left_path_, left_.x_down, left_.x_pressed);
    GetBoolState(y_action_, left_path_, left_.y_down, left_.y_pressed);
    GetBoolState(menu_action_, left_path_, left_.menu_down, left_.menu_pressed);
    GetBoolState(thumbstick_click_action_, left_path_, left_.thumbstick_down, left_.thumbstick_pressed);
    GetBoolState(thumbstick_click_action_, right_path_, right_.thumbstick_down, right_.thumbstick_pressed);
    return true;
}

void XrInput::Shutdown()
{
    initialized_ = false;
    if (left_space_) { xrDestroySpace(left_space_); left_space_ = XR_NULL_HANDLE; }
    if (right_space_) { xrDestroySpace(right_space_); right_space_ = XR_NULL_HANDLE; }
    if (left_aim_space_) { xrDestroySpace(left_aim_space_); left_aim_space_ = XR_NULL_HANDLE; }
    if (right_aim_space_) { xrDestroySpace(right_aim_space_); right_aim_space_ = XR_NULL_HANDLE; }
    if (action_set_) { xrDestroyActionSet(action_set_); action_set_ = XR_NULL_HANDLE; }
    pose_action_ = aim_pose_action_ = trigger_action_ = grip_action_ = thumbstick_action_ = recenter_action_ = a_action_ = b_action_ = XR_NULL_HANDLE;
    x_action_ = y_action_ = menu_action_ = thumbstick_click_action_ = XR_NULL_HANDLE;
    instance_ = XR_NULL_HANDLE;
    session_ = XR_NULL_HANDLE;
    reference_space_ = XR_NULL_HANDLE;
    left_ = {}; right_ = {};
    recenter_requested_ = false;
}

} // namespace kcd2vr
