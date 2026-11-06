#pragma once
#include "native_grab_producer60.h" // DS-F70: the contract types (Intent/Owner/.../Backend) live here, owned by Codex's producer
#include "native_hand_proxy_owner60.h"
#include <cstdint>
// DS-F70 (Codex's grab contract): the backend bridge. The producer owns
// native_grab_producer60.{h,inl} + grab_request60.h (frozen for Codex); this
// is the backend's disjoint landing. FillBackend wires the same-T4
// implementations into the producer's Backend; the backend retains the prop
// lease + proxy by requestToken until retire ACKs actual retirement.
namespace kcd2vr::physics::native_grab_backend60 {
void FillBackend(grab_producer60::Backend& out) noexcept;
}
