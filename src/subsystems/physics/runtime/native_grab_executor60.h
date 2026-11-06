#pragma once
#include "vrik/constraint_op60.h" // DS-F62: the Op type (file scope: namespace kcd2vr header)
#include "native_hand_proxy_t4.h"
#include <cstdint>
// DS-F62: the gated grab executor - the layer that JOINS the drain to the
// native leaf. Registration is explicitly OFF until (a) every pin passes,
// (b) the proxy client is registered, and (c) the source-connected fixture
// is reviewed. The executor runs on the GAME THREAD (the drain seam):
// - Create: the op's key.sourceBody is the prop's IPhysicalEntity (resolved
//   by the request producer through the pinned 8A49FC), the buddy is the
//   proxy's FRESH body handle from its achieved record (the real lease stays
//   in the owner record); packet flags 0x300 (retail baseline), identity
//   frames, pt zero, id unset (the returned positive handle feeds the
//   machine's typed-Present ACK);
// - Remove: the type-6 packet by the op's key.id; <=0 native status = false.
// No native call happens while unregistered (the drain drops ops with
// EXECUTOR_ABSENT).
namespace kcd2vr::physics::grab_executor60 {
bool Ready() noexcept; // pins + proxy client + probe presence: the registration gate
bool Exec(const kcd2vr::constraint_op60::Op& op) noexcept; // the drain's executor body
void Register() noexcept;  // sets the executor seam ONLY when Ready()
void Unregister() noexcept;
}
