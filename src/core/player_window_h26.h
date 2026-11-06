#pragma once
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
namespace kcd2vr::player_window_h26 {
// Player allowance only. Native trigger eligibility, impact cap and native
// authored close events remain authoritative; this never supplies an opening.
inline std::atomic<float> multiplier{1.5f};
inline std::atomic<float> perfectBlockMultiplier{2.f};
inline float widen(float original,bool qualified,float scale,bool exactScale=false) noexcept {
    if(!qualified||!std::isfinite(original)||original<=0||!std::isfinite(scale)||scale<=1||scale>2)return original;
    const double value=exactScale?double(original)*scale:std::min(double(original)*scale,double(original)+.5);
    return std::isfinite(value)?float(value):original;
}
inline bool fresh(std::uint64_t stamp,std::uint64_t now,std::uint64_t limit) noexcept {
    return stamp&&now>=stamp&&now-stamp<=limit;
}
}
