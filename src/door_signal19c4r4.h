#pragma once
#include "core/native_door_script19c4r4.h"
#include "core/door_curve_signal66.h"
namespace kcd2vr::door_signal19c4r4 {
// Native ScriptBind_System::GetCVar private-name branch. All communication is
// copied POD; Lua enters on its own scheduler. No worker calls into Lua.
bool SetEnabled(bool on) noexcept;
bool Ready() noexcept;
bool Publish(const door_script19c4r4::Command&,std::uintptr_t acceptedEntity=0) noexcept;
bool ReadCleanupIdentity(door_script19c4r4::CleanupIdentity&) noexcept;
bool RetireKnownAbsent(const door_script19c4r4::CleanupIdentity&,std::uintptr_t currentEntity) noexcept;
bool ReadAck(door_script19c4r4::Ack&) noexcept;
bool HasCleanupLease() noexcept;
void SetCurveHandler66(door_curve_signal66::Handler) noexcept;
// Fixed read-only selected-item query: validated Item WUID, contact opcode bit.
// Contact requires the separate synchronous nominated contact; normal Use is unchanged.
using SmallPickupReaderR24=std::uint32_t(*)(std::uint64_t,bool) noexcept;
bool SetSmallPickupReaderR24(SmallPickupReaderR24) noexcept;
// R100 loose scenery swap: install the getter hook so the companion can name the spawned copy.
bool ArmSwapNoteR100() noexcept;
void Stop() noexcept;
bool Shutdown() noexcept;
}
