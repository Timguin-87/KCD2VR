#pragma once
// R106 (Miki: "kill blow impale: nearly instant ragdoll, he stays on my blade"): the game only hands the body to
// ragdoll physics when its death animation has played; the impale servo (native_impale_part.inl) can't pin the
// weapon before that, so a killing stab waits ~1-2 s with the blade "painted" on a still animated man. We can't
// skip the animation, but we can play it faster: while the servo waits for THIS victim's ragdoll, his
// CActionController time scale (+0x9C, the exact value native Update multiplies by) is raised, so his death clip
// runs `factor` times faster and the ragdoll comes that much sooner. Restored to the saved value when the pin is
// there, the servo gives up, or after 5 s. Only the victim of an admitted lethal thrust, never Henry.
// Switch: Experiments > Hand trials "Impale: fast death", INI VrImpaleFastDeathR106 (ON = runs).
#include <atomic>
#include <cstdint>
namespace kcd2vr::impale_death_speed_r106 {
inline std::atomic<bool> enabled{true};
inline constexpr float kFactor=3.0f;
inline constexpr std::uint64_t kWindowMs=5000;
inline std::atomic<std::uint32_t> victim{0};   // entity id of the waiting lethal-thrust victim (0 = none)
inline std::atomic<std::uint64_t> since{0};    // when the wait began (GetTickCount64)
inline std::atomic<std::uint64_t> begun{0},applied{0},restored{0}; // receipts
// servo (game thread): a lethal thrust on `entity` is admitted and the servo now waits for his ragdoll
inline void Begin(std::uint32_t entity,std::uint64_t now) noexcept {
    if(!entity)return;since.store(now?now:1,std::memory_order_relaxed);victim.store(entity,std::memory_order_release);begun.fetch_add(1,std::memory_order_relaxed);
}
// servo: pinned, gave up, released - the clip speed goes back
inline void End() noexcept { victim.store(0,std::memory_order_release); }
// Update detour: should THIS controller (entity id) run fast right now?
inline bool Wants(std::uint32_t entity,std::uint64_t now) noexcept {
    if(!entity||!enabled.load(std::memory_order_relaxed))return false;
    if(victim.load(std::memory_order_acquire)!=entity)return false;
    const auto s=since.load(std::memory_order_relaxed);
    if(!s||now<s||now-s>kWindowMs){victim.store(0,std::memory_order_release);return false;}
    return true;
}
}
