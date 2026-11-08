#pragma once
#include <cstdint>

namespace kcd2vr::native_input_look_lab {

bool Install() noexcept;
void Uninstall() noexcept;
void SetActive(bool active) noexcept;
void SetInputGain(float gain) noexcept; // 0..1 diagnostic scaling of native mouse-look deltas

// Publish absolute HMD-local yaw/pitch (radians). The lab converts them to
// relative deltas and sends synthetic KCD2 mouse-look SInputEvents through
// the game's own CameraInputDispatch. No OS mouse input is generated.
void PublishHeadAngles(float yaw, float pitch, bool valid) noexcept;

bool Ready() noexcept;          // dispatcher + controller + mouse templates captured
bool HasYawTemplate() noexcept;
bool HasPitchTemplate() noexcept;
void ResetAnchor() noexcept;

// V64.56 DEBUG universal Touch->KCD2 gamepad bridge. Internal XI templates bootstrap from KCD2's own xi_* names/key IDs; genuine runtime XI events opportunistically replace them. Replay remains on KCD2's own dispatch thread.
void SetGamepadMode(int mode) noexcept; // 0 off, 1 discover, 2 sticks, 3 sticks+face/triggers
int GetGamepadMode() noexcept;
bool DiscoverNativeGamepadSymbolsDirect() noexcept;
// Cheap prerequisite only: deferred startup is not a failed discovery attempt.
bool GamepadDiscoveryContextReady() noexcept;
int NativeGamepadSymbolCount() noexcept;
void PublishGamepadState(float lx,float ly,float rx,float ry,float lt,float rt,
    bool a,bool b,bool x,bool y,bool lb,bool rb,bool l3,bool r3,bool start,bool back,
    bool dpad_up=false,bool dpad_down=false,bool dpad_left=false,bool dpad_right=false) noexcept;
// Current native grab_body HOLD availability, sampled only in the existing input callback.
// press=0 retires; positive occurrence returns -1 unknown, 0 absent, 1 present.
int QueryCarryContext25(std::uint32_t press) noexcept;
// R147: is the game's follow prompt (focus_follow_init_interactor_press) listed in the same native hint rows now?
// want=false retires the query. -1 unknown (no fresh answer / unreadable), 0 not listed, 1 listed. ride_follow_r147.h
int QueryFollowHint147(bool want) noexcept;
void SetStickDirectionKeys(bool enabled) noexcept; // V66.178R6 xi_thumb[lr]_{up,down,left,right}
bool NativeLeftStickReady() noexcept;
bool NativeRightStickReady() noexcept;
bool NativeGamepadCoreReady() noexcept;
bool NativeGamepadFullReady() noexcept;
void ArmTransportHeartbeat() noexcept;

} // namespace kcd2vr::native_input_look_lab
