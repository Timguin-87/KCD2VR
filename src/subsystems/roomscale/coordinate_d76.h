#pragma once
#include "native_roomscale_d76.h"

namespace kcd2vr::roomscale_d76 {
// Optional current-body bases must belong to the XR-origin snapshot they use.
// Zero is legitimate only before enrollment, never an unknown replacement for C.
inline bool compatible(const native_roomscale_d76::Origin& canonical,
    const native_roomscale_d76::Origin& candidate,std::uint64_t revision) noexcept {
    const auto&a=canonical.owner;const auto&b=candidate.owner;
    if(b.epoch&&b.epoch!=revision)return false;
    if(!a.entity)return true;
    return a.player==b.player&&a.entity==b.entity&&a.character==b.character&&
        a.entityId==b.entityId&&a.lifetime==b.lifetime&&b.epoch==revision;
}
// Apply before the existing XR-to-camera/yaw conversion. The native eye base
// stays unchanged; consumedWorld is requested displacement, NOT an accepted
// collision result, and must never be added to the camera.
inline native_roomscale_d76::Vec3 residual(
    native_roomscale_d76::Vec3 raw, const native_roomscale_d76::Origin& origin) noexcept {
    return {raw.x-origin.consumedTracking.x,raw.y-origin.consumedTracking.y,
        raw.z-origin.consumedTracking.z};
}
}
