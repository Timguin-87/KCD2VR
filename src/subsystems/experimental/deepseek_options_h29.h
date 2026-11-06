#pragma once
#include <atomic>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <limits>

namespace kcd2vr::deepseek_h29 {
// Session-only. Never serialized; opening the lab does not enable experiments.
inline std::atomic<bool> angularVignette{false};
inline std::atomic<bool> wallBackoutSpringH31{false};
inline std::atomic<bool> fingerContactSpringH31{false};
// The whole word is an immutable source identity, not just an enable bit.
// An OFF/ON cycle cannot authorize a packet from the earlier ON interval.
inline std::atomic<std::uint64_t> weaponNpcContactControlH5{0};
inline std::uint64_t ReadWeaponNpcContactControlH5() noexcept {
    return weaponNpcContactControlH5.load(std::memory_order_acquire);
}
inline bool WeaponNpcContactsEnabledH5() noexcept {return (ReadWeaponNpcContactControlH5()&1u)!=0;}
inline void SetWeaponNpcContactsH5(bool enabled) noexcept {
    auto prior=ReadWeaponNpcContactControlH5();
    for(;;){
        if(bool(prior&1u)==enabled)return;
        // Exhaustion permanently closes admission; never recycle an old word.
        const auto next=prior>=std::numeric_limits<std::uint64_t>::max()-2?
            (std::numeric_limits<std::uint64_t>::max()&~std::uint64_t{1}):((prior+2)&~std::uint64_t{1})|std::uint64_t(enabled);
        if(weaponNpcContactControlH5.compare_exchange_weak(prior,next,std::memory_order_acq_rel))return;
    }
}
template<class Input> inline void StampWeaponNpcContactControlH5(Input& in) noexcept {
    in.npcContactControlH5=ReadWeaponNpcContactControlH5();
}
inline void DisableAll() noexcept {
    angularVignette.store(false,std::memory_order_release);
    wallBackoutSpringH31.store(false,std::memory_order_release);
    fingerContactSpringH31.store(false,std::memory_order_release);
    SetWeaponNpcContactsH5(false);
}
// Constant31 already travels in one immutable presentation snapshot for both eyes.
// Sign selects the experiment; magnitude is the existing comfort envelope. This
// does not change the GPU constant layout, level setting, or UI/Theater policy.
inline float EncodeVignette(float strength,bool angular) noexcept {
    if(!std::isfinite(strength))return 0.f;
    strength=std::clamp(strength,0.f,1.f);
    return angular&&strength>0.f?-strength:strength;
}
}
