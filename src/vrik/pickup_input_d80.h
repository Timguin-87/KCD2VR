#pragma once
#include <cmath>
// Included by the real-world pickup adapters and their focused fixtures.
// The fields belong to one completed pose publication, not a newer XR read.
namespace pickup_input_d80 {
template<class Frame> inline const char* refusal(const Frame& f,int side,bool selectedFists) noexcept {
    if(side<0||side>1||!std::isfinite(f.mapGripM39[side])||!std::isfinite(f.pickupTriggerD80[side])||
       f.mapGripM39[side]<0.f||f.mapGripM39[side]>1.f||f.pickupTriggerD80[side]<0.f||f.pickupTriggerD80[side]>1.f)
        return "PICKUP_INPUT_INVALID";
    // Selected unarmed mode is not an occupied hand. The other controller's
    // squeeze cannot veto this hand's Grip-only native pickup.
    return selectedFists&&f.mapGripM39[side]>.60f&&f.pickupTriggerD80[side]>.60f?"FIST_OWNER":nullptr;
}
}
