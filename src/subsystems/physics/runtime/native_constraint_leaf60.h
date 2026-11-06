#pragma once
#include "vrik/constraint_packet60.h"
#include <cstdint>
// DS-F46 (after Codex F40 review): pinned native leaf v2.
// - The Action slot is pinned to the EXACT retail address (base+0xAC0D18,
//   the rigid vft 3A48DC0's +0x38 slot) - the 16-byte prologue alone occurs
//   177 times in the DLL and is only a secondary check. The body vft must be
//   the exact rigid (3A48DC0) or articulated (3A48B30) table.
// - Action returns the native int result; the outcome is TYPED
//   (Refused/Submitted/Exception + the returned id) - no invented bool from
//   SEH survival. The returned id is QUEUED work, never a Present ACK.
// - Game thread OUTSIDE the step only: never call Action(...,0) from inside
//   the T4 write-lock callback (Action(...,1) there is the step's own job).
// UNREGISTERED until the type-6 remove path and both body leases exist.
namespace kcd2vr::physics::constraint_leaf60 {
// DS-F50 (after Codex F42 review): the exact Action slot differs per table -
// RIGID 3A48DC0+0x38 = AC0D18 (this leaf, loose rigid props only);
// ARTICULATED 3A48B30+0x38 = A99DF0 (the corpse-push path's own handler).
// This leaf never points articulated bodies at the rigid Action.
inline constexpr std::uintptr_t kGetEntityPhysics=0x8A49FC;
inline constexpr std::uintptr_t kRigidVft=0x3A48DC0;
inline constexpr std::uintptr_t kArticulatedVft=0x3A48B30; // NOT accepted by this leaf (slot differs)
inline constexpr std::uintptr_t kActionSlot=0x38;
inline constexpr std::uintptr_t kActionRva=0xAC0D18;   // the RIGID table's Action entry
inline constexpr std::uintptr_t kArticulatedActionRva=0xA99DF0; // documented only; owned by the corpse-push path
inline constexpr unsigned char kGetEntityPhysicsPrologue[16]{0x40,0x53,0x48,0x83,0xEC,0x20,0xBA,0x01,0x00,0x00,0x00,0x48,0x8B,0xD9,0xE8,0xB5};
inline constexpr unsigned char kActionPrologue[16]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x18,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56};

struct Result {
    enum class Kind : unsigned char {Refused,Submitted,Exception};
    Kind kind=Kind::Refused;std::uint32_t returnedId=0;int nativeReturn=0; // raw signed native int preserved; >0 = handle per retail (eax/setg)
    bool submitted() const noexcept {return kind==Kind::Submitted;}
};
// Pins the getter only - NOT full service readiness (the Action class/slot,
// packet variant and remove path are separate prerequisites).
bool GetterPinned(std::uintptr_t base) noexcept;
bool ResolveBody(std::uintptr_t base,void* entity,void*& body) noexcept;   // SEH, exact getter pin, exact vft whitelist, output cleared on every rejection
Result ActionAddConstraint(std::uintptr_t base,void* body,const kcd2vr::constraint_packet60::Packet& packet) noexcept;
// DS-F49: the remove leaf - the SAME exact Action slot with the proven type-6
// packet (retail remove sequence: id + bRemove=1 + Action(...,0)).
Result ActionRemoveConstraint(std::uintptr_t base,void* body,const kcd2vr::constraint_packet60::RemovePacket& packet) noexcept;
}
