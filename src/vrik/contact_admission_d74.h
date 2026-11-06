#pragma once
// A collision receipt may constrain a hand, but cannot claim arbitrary body
// translation. This bounds only EXTRA shoulder participation introduced by
// contact. Legitimate raw-controller reach and fixed arm lengths are untouched.
#include "upper_body.h"
namespace kcd2vr::contact_admission_d74 {
namespace pc=pose_core;
inline bool shoulderAllowed(pc::Vec3 natural,pc::Vec3 rawEndpoint,pc::Vec3 collisionShoulder,
        double upper,double fore,pc::Vec3 fallback) noexcept {
    if(!pc::finite(collisionShoulder))return false;
    const auto raw=pc::controller_shoulder103(natural,rawEndpoint,upper,fore,fallback);
    if(!raw)return false;
    // Half a humerus (at most15cm) is a conservative presentation envelope for
    // scapular participation, not a new wrist reach cap or bone-length change.
    // Contact may always return toward the natural shoulder, including when
    // calibration already requires more accommodation for the raw controller.
    const double participation=std::min(.15,.5*upper);
    const double allowance=std::max(participation,pc::length(raw->shoulder-natural));
    return pc::length(collisionShoulder-natural)<=allowance+.002;
}
}
