#pragma once
// D39: observe excessive shoulder travel without clamping an authoritative
// wrist. D38 capped this fit at 15 cm and the downstream fixed-length arm solve
// then moved the wrist. Body calibration/follow and shoulder skinning remain
// separate anatomical work; a warning must not masquerade as a hand constraint.
#include "pose_math.h"
#include <algorithm>
#include <atomic>
#include <cmath>
namespace kcd2vr::shoulder_reach_d38 {
namespace m=pose_core;
// D39: retain the D38 storage/API for INI compatibility, but this is a
// diagnostic threshold. An anatomical preference must never move the selected
// controller/contact endpoint. The shoulder fit below keeps both bone lengths.
inline std::atomic<float> maxShiftM{.15f}; // VrShoulderReachWarningM, .02 .. .40 m
inline std::atomic<unsigned long long> capped{0}; // legacy storage: warning observations, not caps
inline m::Vec3 observe(m::Vec3 natural,m::Vec3 fitted,double maxShift) noexcept {
    if(!m::finite(natural)||!m::finite(fitted)||!std::isfinite(maxShift)||maxShift<=0)return fitted;
    const m::Vec3 d=fitted-natural;const double l=m::length(d);
    if(!(l>maxShift))return fitted;
    capped.fetch_add(1,std::memory_order_relaxed);
    return fitted;
}
}
