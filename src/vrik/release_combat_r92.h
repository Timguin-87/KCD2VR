#pragma once
#include <atomic>
namespace kcd2vr::release_combat_r92 {
// Optional100ms sword/polearm posture entry. Only input timing changes;
// native stamina, block outcome and prepared/committed attack ownership remain.
inline std::atomic<bool> fastBlock{false}; // VrFastBlockR92
inline unsigned guardDwell(unsigned configured,bool shield) noexcept {
    return !shield&&fastBlock.load(std::memory_order_relaxed)?100u:configured;
}
}
