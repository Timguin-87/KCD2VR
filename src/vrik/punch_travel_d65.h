#pragma once
// User-selected base travel. Native admission and existing sensitivity/travel
// multipliers remain unchanged; 4.5 cm is the exact D64 reference.
#include <atomic>
#include <algorithm>
#include <cmath>
namespace kcd2vr::punch_travel_d65 {
inline constexpr float kDefault=.10f,kMin=.045f,kMax=.20f;
inline std::atomic<float> minimumMeters{kDefault};
inline float setting(float v) noexcept {return std::isfinite(v)?std::clamp(v,kMin,kMax):kDefault;}
inline double value() noexcept {return double(setting(minimumMeters.load(std::memory_order_relaxed)));}
}
