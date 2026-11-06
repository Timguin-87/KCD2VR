#pragma once
// R118 (Miki 6 Nov: "in SteamVR I could not open inventory. can we have hold X for inventory for steamvr?")
// Controller layout chosen in the start setup (Start KCD2VR.cmd) or VR CONFIG > CONTROLS > LAYOUT, saved as
// VrControllerLayoutR118 (0/1/2). Explicit choice only - never guessed from the runtime or headset name.
//  0 OPENXR NATIVE (Quest Touch on VDXR/Oculus): Left Menu = inventory, X tap talk, X hold torch, thumbrest modifier.
//  1 STEAMVR: X hold = inventory (SteamVR may keep the left Menu button), torch = left Trigger at the left shoulder.
//  2 PS VR2 (SteamVR): as STEAMVR, and the quick-access modifier is FACE TOUCH (touch Triangle/Circle).
// Left Menu keeps opening the inventory on every layout when the runtime delivers it.
#include <atomic>

namespace kcd2vr::controller_layout_r118 {
enum Layout : int {OpenXrNative=0,SteamVr=1,Psvr2=2,Count=3};
inline std::atomic<int> current{OpenXrNative};
constexpr int Clamp(int value) noexcept {return value>=0&&value<Count?value:OpenXrNative;}
constexpr int Next(int value) noexcept {return (Clamp(value)+1)%Count;}
// What the layout changes. One table for the native pad, the keyboard fallback, the menu and the setup dialog.
constexpr bool XHoldOpensInventory(int layout) noexcept {return Clamp(layout)!=OpenXrNative;}
constexpr bool XHoldLightsTorch(int layout) noexcept {return Clamp(layout)==OpenXrNative;}
constexpr bool LeftMenuOpensInventory(int) noexcept {return true;}
// Preset applied when the layout is CHOSEN (menu click / setup save). The QUICK ACCESS button still toggles it after.
constexpr bool FaceTouchFor(int layout) noexcept {return Clamp(layout)==Psvr2;}
inline void Set(int layout) noexcept {current.store(Clamp(layout),std::memory_order_release);}
inline int Get() noexcept {return Clamp(current.load(std::memory_order_acquire));}
constexpr const wchar_t* Name(int layout) noexcept {
    return Clamp(layout)==SteamVr?L"STEAMVR":Clamp(layout)==Psvr2?L"PS VR2":L"OPENXR";
}
constexpr const char* NameA(int layout) noexcept {
    return Clamp(layout)==SteamVr?"STEAMVR":Clamp(layout)==Psvr2?"PSVR2":"OPENXR";
}
// Setup dialog entries + the line that says what changes.
constexpr const wchar_t* SetupLabel(int layout) noexcept {
    return Clamp(layout)==SteamVr?L"SteamVR (Quest / Index / other on SteamVR)":
           Clamp(layout)==Psvr2?L"PlayStation VR2 (SteamVR)":L"OpenXR native (Quest Touch on VDXR / Oculus)";
}
constexpr const wchar_t* SetupInfo(int layout) noexcept {
    return Clamp(layout)==SteamVr?
        L"X tap = talk, X hold = INVENTORY (SteamVR keeps the left Menu button).\nTorch = left Trigger at the left shoulder. Modifier = right thumbrest.":
           Clamp(layout)==Psvr2?
        L"Square tap = talk, Square hold = INVENTORY. Torch = left Trigger at the left shoulder.\nModifier = touch Triangle / Circle (FACE TOUCH). L3+R3 hold = VR menu.":
        L"Left Menu = inventory, X tap = talk, X hold = torch.\nModifier = right thumbrest. Same as before R118.";
}
} // namespace kcd2vr::controller_layout_r118
