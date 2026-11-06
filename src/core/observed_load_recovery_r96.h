#pragma once
// A positively observed loading episode can admit a NEW qualification attempt.
// This is never proof that a native object/constraint died, never a cleanup
// receipt, and never permission to replay the failed request or discard debt.
#include "../subsystems/throw_drop/inventory_lifecycle_t8.h"
#include <cstdint>
namespace kcd2vr::observed_load_recovery_r96 {
struct Witness {std::uint64_t episode=0;bool loading=false,admitted=false;};
inline Witness observe() noexcept {
    namespace life=inventory_lifecycle_t8;
    const auto episode=life::loadEpisode18.load(std::memory_order_acquire);
    const bool loading=life::loadingObserved18.load(std::memory_order_acquire);
    const auto state=life::state.load(std::memory_order_acquire);
    const bool stable=episode==life::loadEpisode18.load(std::memory_order_acquire)&&
        loading==life::loadingObserved18.load(std::memory_order_acquire)&&state==life::state.load(std::memory_order_acquire);
    // Even a mixed observation retains its epoch boundary; it cannot claim a
    // completed load or an in-progress episode on the basis of mixed copies.
    return {episode,stable&&loading,stable&&!loading&&!(state&1)};
}
inline bool completedSince(const Witness& before,const Witness& after) noexcept {
    return before.episode&&after.admitted&&
        (after.episode>before.episode||(before.loading&&after.episode==before.episode));
}
inline bool completedSince(const Witness& before) noexcept {return completedSince(before,observe());}
}
