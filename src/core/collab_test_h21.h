#pragma once
#include <atomic>
#include <cstdint>
// H21 requested control ownership: ordinary interactions/touch/VRIK/combat stay
// default; new rigid-body work follows F9, throwing follows F4 rung3. F11 remains
// reload diagnostics/tests and F12 remains the native combat-router experiment.
namespace kcd2vr::collab_h21 {
inline std::atomic<bool> enabled{false};
inline std::atomic<int> physicsMode{0}; // 0 OFF,1 observe,2 rigid props,3 NPC/corpse lab
inline std::atomic<std::uint64_t> epoch{1};
inline void setPhysicsMode(int mode) noexcept {
    mode=mode<0?0:mode>3?3:mode;
    if(physicsMode.load(std::memory_order_acquire)==mode)return;
    // OFF is immediately visible; an OFF/ON cycle also retires prior receipts.
    enabled.store(false,std::memory_order_release);
    epoch.fetch_add(1,std::memory_order_acq_rel);
    physicsMode.store(mode,std::memory_order_release);
    if(mode>=2)enabled.store(true,std::memory_order_release);
}
inline bool current(std::uint64_t captured) noexcept {
    return captured&&enabled.load(std::memory_order_acquire)&&
        captured==epoch.load(std::memory_order_acquire);
}
inline bool npcPhysics() noexcept {return enabled.load(std::memory_order_acquire)&&physicsMode.load(std::memory_order_acquire)>=3;}
inline std::atomic<bool> throwing{false};
inline std::atomic<std::uint64_t> throwEpoch{1};
inline void setThrowing(bool on) noexcept {
    if(throwing.exchange(on,std::memory_order_acq_rel)!=on)throwEpoch.fetch_add(1,std::memory_order_acq_rel);
}
}
