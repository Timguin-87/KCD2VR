#pragma once
#include <cstdint>
#include "subsystems/physics/runtime/native_grab_producer60.h"
namespace kcd2vr::grip_stow_cb1 {
struct Identity {
    std::uint32_t player=0; // Henry's numeric EntityId; Owner.player is his original CEntity pointer.
    std::uintptr_t actor=0,character=0,manager=0,nativePlayer=0;
    std::uint64_t body=0,tracking=0,request=0,policy=0;
};
using CurrentReader=bool(*)(const physics::grab_producer60::Owner&,std::uint64_t,Identity&) noexcept;
void SetCurrentReader(CurrentReader) noexcept;
bool Enabled() noexcept;
void SetEnabled(bool) noexcept;
int Button() noexcept;
void SetButton(int) noexcept;
// True owns this original button press, even if subsequent validation refuses.
// The caller MUST suppress its native/utility down+pressed routes until release.
bool Request(int side,std::uint64_t now) noexcept;
bool Install() noexcept;
}
