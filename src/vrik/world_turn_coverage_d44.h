#pragma once
#include <algorithm>
#include <cmath>
namespace kcd2vr::world_turn_coverage_d44 {
// Reuse the existing bounded slow-frame sweep policy on the FIRST frame of a
// large bow root turn. The old policy waited for a limited prefix before raising
// coverage: D43 sample221042 then displaced the left wrist122.844mm for one frame.
// This changes query subdivision only, not the world trajectory, permission,
// hand endpoint, collision response, shared ray/time budget or actor/camera pose.
inline double scale(bool bow,double radius,double rootAngle,double ordinary) noexcept {
    if(!bow||ordinary>=2.0||!std::isfinite(radius)||radius<=.004||radius>3.0||
       !std::isfinite(rootAngle)||rootAngle<=0.0)return ordinary;
    const double fineStep=2.0*std::acos(std::clamp(1.0-.004/radius,-1.0,1.0));
    // Match the existing three-segment4mm-chord capacity exactly; small turns
    // retain the previous path. No new arbitrary angular threshold is used.
    return rootAngle>3.0*fineStep?std::max(ordinary,2.0):ordinary;
}
}
