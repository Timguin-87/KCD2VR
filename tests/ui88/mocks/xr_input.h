#pragma once
// TEST DOUBLE. ControllerState fields copied verbatim; pose is value-only for these tests.
namespace kcd2vr {
struct XrPose{float pos_x{},pos_y{},pos_z{},qx{},qy{},qz{},qw{1};};
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
    bool quick_access_touch{false};
    bool a_pressed{false};
    bool b_pressed{false};
    bool x_pressed{false};
    bool y_pressed{false};
    bool menu_pressed{false};
    bool thumbstick_pressed{false};
};
}
