#pragma once
#include "native_script_t2.h"
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace kcd2vr::private_launcher_t21 {
// Disabled source contribution, not a runtime registration or a DLL boundary.
// The native source stays alive for delayed native causes. No inventory grant,
// source reconstruction, wrapper destructor, native flag or save hook is used.
inline constexpr std::size_t capacity=8,wrapperAllocationBytes=0x18;
alignas(16) inline constexpr unsigned char launcherGuid[16]{
    0x56,0x7F,0x87,0xDC,0xF3,0x16,0x89,0x58,0xB6,0x06,0x56,0x17,0x65,0x87,0x17,0xCC
}; // dc877f56-16f3-5889-b606-5617658717cc, actual native Windows GUID layout
enum class Status : unsigned char {Empty,Refused,Unknown,Ready,Absent,Quarantined,Capacity};
struct Lease {
    std::uint64_t token=0,key=0;
    std::uintptr_t itemManager=0,world=0,wrapper=0,definition=0;
    Status status=Status::Empty;
};
static_assert(wrapperAllocationBytes==24);
static_assert(std::is_trivially_copyable_v<Lease>);
// Existing action-owner lexical scope, after native_transient_ammo_t20.inl:
//   AmmoPrivateLauncherAcquireT21(currentContext,currentDefinition,lease)
//   AmmoPrivateLauncherLookupT21(currentContext,lease,currentItem)
// These routines require the ACTUAL same-stack script capability each time.
// A copied Context, load-policy episode, VM reset or absent item never grants a
// second allocation. Every observed original manager/world pair is spent once.
// The fixed records are process retained; capacity exhaustion refuses admission.
// Lease pointers are identity values only. Only Lookup returns a qualified item
// for immediate same-stack native use; callers must recheck context afterwards.
}
