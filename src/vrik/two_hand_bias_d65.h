#pragma once
// Optional additional primary-wrist steering. Disabled = exact D64 axis.
// Bias is bounded below .5: even opposing axes cannot cancel the blended
// direction. One target construction is used for visible and raw intent poses.
#include "pose_math.h"
#include <atomic>
#include <algorithm>
#include <cmath>
namespace kcd2vr::two_hand_d65 {
inline constexpr float kDefault=.20f,kMin=0.f,kMax=.40f;
inline std::atomic<bool> enabled{true};
inline std::atomic<float> primaryBias{kDefault};
inline float setting(float v) noexcept {return std::isfinite(v)?std::clamp(v,kMin,kMax):kDefault;}
inline double bias() noexcept {return enabled.load(std::memory_order_relaxed)?double(setting(primaryBias.load(std::memory_order_relaxed))):0.;}
inline pose_core::Vec3 target(pose_core::Vec3 wristAxis,pose_core::Vec3 rawAxis,double amount) noexcept {
    namespace m=pose_core;
    if(!(amount>0)||!std::isfinite(amount)||m::length(rawAxis)<.035)return rawAxis;
    const auto a=m::unit(wristAxis),b=m::unit(rawAxis);if(!a||!b)return rawAxis;
    amount=std::min(amount,double(kMax));
    const auto blended=m::unit(*a*amount+*b*(1.-amount));
    return blended?*blended*m::length(rawAxis):rawAxis;
}
}
