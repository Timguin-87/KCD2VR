#pragma once
// Isolated, allocation-free gesture policy for a single native attached item.
// Its state machine is owned by action_time_owner59c11; NEVER infer a native
// detach or inventory transfer from a controller event alone.
#include <cmath>
namespace kcd2vr::grasp_transaction59c12c {
enum class Intent { KeepHolding, Drop, Stow };
inline float SafeHoldAt(float requested) noexcept {
    // The audited 3d_pick_rhand clip attaches at .452 and pockets at .617.
    // Do not permit tuning beyond that outcome; reject NaN as well.
    if(!std::isfinite(requested)) return 0.50f;
    return requested < 0.47f ? 0.47f : requested > 0.55f ? 0.55f : requested;
}
inline Intent Decide(float grip,float currentDistance,float acquisitionDistance,float pullThreshold,float releaseThreshold) noexcept {
    if(!std::isfinite(grip)||!std::isfinite(currentDistance)||!std::isfinite(acquisitionDistance))return Intent::KeepHolding;
    if(grip<releaseThreshold)return Intent::Drop;
    if(acquisitionDistance-currentDistance>=pullThreshold)return Intent::Stow;
    return Intent::KeepHolding;
}
}
