#pragma once
#include <atomic>
#include <cstdint>
namespace kcd2vr::diagnostics::hud168r1 {
// Cumulative *instrumented* observations; start/end snapshots allow bounded interval comparisons.
// Individual atomic counters are not one atomic multi-counter snapshot.
// Increment only inside an explicit HUD capture. Not delivered FPS/GPU timing.
inline std::atomic<std::uint64_t> draws{0},fast{0},masked{0},contended{0},nativeTicks{0},resolves{0};
}
