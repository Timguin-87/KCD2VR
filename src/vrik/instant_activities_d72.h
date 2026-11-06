#pragma once
// D72: user-requested activity policies. Read these only when admitting a new
// occurrence. An admitted owner retains its own completion/cancellation debt;
// switching a preference must never replay or abandon that occurrence.
#include <atomic>
#include <cstdint>

namespace kcd2vr::instant_d72 {
enum class Family : std::uint8_t { Interactions, Crafting, Reloads, Combat, Count };
enum class State : std::uint8_t { Native, Available, Active, Retiring, Unsupported };
inline std::atomic<bool> activities{true};
inline std::atomic<bool> interactions{true};
inline std::atomic<bool> crafting{true};
inline std::atomic<bool> reloads{true};
inline std::atomic<bool> combat{true};
inline std::atomic<State> state[4]{};
inline bool Activities() noexcept { return activities.load(std::memory_order_relaxed); }
inline bool Interactions() noexcept { return Activities() && interactions.load(std::memory_order_relaxed); }
inline bool Crafting() noexcept { return Activities() && crafting.load(std::memory_order_relaxed); }
inline bool Reloads() noexcept { return Activities() && reloads.load(std::memory_order_relaxed); }
inline bool Combat() noexcept { return Activities() && combat.load(std::memory_order_relaxed); }
inline bool Requested(Family f) noexcept {
    switch(f) {
    case Family::Interactions:return Interactions();
    case Family::Crafting:return Crafting();
    case Family::Reloads:return Reloads();
    case Family::Combat:return Combat();
    default:return false;
    }
}
inline void Publish(Family f, State s) noexcept {
    const auto i=static_cast<unsigned>(f);
    if(i<4)state[i].store(s,std::memory_order_relaxed);
}
inline State Current(Family f) noexcept {
    const auto i=static_cast<unsigned>(f);
    return i<4?state[i].load(std::memory_order_relaxed):State::Unsupported;
}
}
