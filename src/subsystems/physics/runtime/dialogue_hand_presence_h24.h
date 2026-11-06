#pragma once
#include "../../../dialogue_state59c10r2.h"
#include "native_hand_proxy_t4.h"
namespace kcd2vr::physics::dialogue_hand_h24 {
// Separate physical-presence permission: never enables gameplay input, use,
// damage, NPC reactions, or the native camera. Preserve the native sample age.
struct Witness {
    dialogue_state59c10r2::State native{};
    std::uint64_t policy=0;
    bool presentation=false;
};
inline Witness capture(const dialogue_state59c10r2::State& state,bool presentation,
    std::uint64_t policy,std::uint64_t now) noexcept {
    if(!presentation||!policy||(policy&1)||!dialogue_state59c10r2::embodiedOwner(state,now))return {};
    return {state,policy,true};
}
inline bool current(const Witness& w,const native_hand_proxy_t4::Input& in,
    std::uint64_t policy,std::uint64_t now) noexcept {
    return w.presentation&&w.policy==policy&&!(policy&1)&&
        in.source.cancelGeneration==policy&&
        dialogue_state59c10r2::embodiedOwner(w.native,now)&&w.native.instance&&
        w.native.player==in.source.player&&w.native.entity==in.playerEntityH24;
}
}
