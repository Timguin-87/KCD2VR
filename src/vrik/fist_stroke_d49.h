#pragma once
#include <atomic>

namespace kcd2vr::fist_stroke_d49 {
// Optional detector-only comparison. No native combat/damage ownership changes.
// Snapshot this value into fists::Sample at the existing pose publication.
inline std::atomic<bool> enabled{false};
}
