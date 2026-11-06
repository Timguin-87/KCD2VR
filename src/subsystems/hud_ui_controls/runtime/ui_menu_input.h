#pragma once
#include "xr_input.h"
#include <windows.h>
#include <cstdint>
namespace kcd2vr::ui_menu_input {
void SetWindowedLaserImageSpaceR16(bool on) noexcept; // R16b: VrWindowedLaserImageSpace
struct Frame {
    HWND window=nullptr;
    bool menu=false,system_menu=false;
    bool native_gamepad=false; // native XI owns focus/actions; this adapter only adds pointer
    bool inventory_root=false,modal=false;
    std::uint8_t pointer_menu_kind=0; // 1 InventoryMap,2 Shop,3 Frontend; diagnostics and frontend device fallback only, never grants pointer ownership
    unsigned inventory_key='I';
    bool allow_pointer=true;
    bool passive_back_only=false; // Loading: whitelist native B / default KB Space, no navigation
    bool block_gameplay=false; // loading/progress, NOT every fullscreen-aligned movie
    bool consume_grips=false; // also contextual native interfaces / mod menu
    std::uint64_t context=0;
    ControllerState left{},right{};
    int ray_hand=-1;
    bool ray_hit=false;
    float u=.5f,v=.5f;
};
struct Result { int hand=-1; bool cursor=false; bool native_pad=false; bool blocks_gameplay=false; float u=.5f,v=.5f; std::uint8_t grip_rearm_mask=0; };
bool Install() noexcept;
void Uninstall() noexcept;
Result Update(const Frame&) noexcept;
int PointerHand() noexcept;
// Bit 0 left / bit 1 right. For semantic world interaction; never a pose/curl filter.
std::uint8_t WorldGripBlockedMask() noexcept;
void WriteSnapshot() noexcept;
// Prevent a held menu control from leaking into gameplay after native root closure.
} // namespace
