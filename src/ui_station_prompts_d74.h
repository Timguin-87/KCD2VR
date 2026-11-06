#pragma once
#include <algorithm>
#include <cmath>
namespace kcd2vr::ui_station_prompts_d74 {
// Only semantic prompt children; native projected positions and visibility stay owned by KCD2.
// D76R1: the station must honor the same saved HUD sliders. D74 hardcoded 60%
// here, which silently discarded every in-menu size adjustment at the bench.
struct Scales { float hints=1.f,controls=1.f; };
inline Scales Choose(bool /*station*/,float hints,float controls) noexcept {
    return {std::isfinite(hints)?std::clamp(hints,.25f,1.5f):1.f,
            std::isfinite(controls)?std::clamp(controls,.25f,1.5f):1.f};
}
}
