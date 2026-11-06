#pragma once
#include <atomic>
#include <cstdint>
namespace kcd2vr::inventory_lifecycle_t8 {
// Policy publication only. No native/VM/inventory call from the mod/menu thread.
// Low bit blocks delivery; upper bits are a monotonically changing load lease.
inline std::atomic<std::uint64_t> state{3};
// T18R2: POSITIVELY OBSERVED loading episode only. This is admission policy,
// NEVER proof that a native object has died or that cleanup may be discarded.
inline std::atomic<std::uint64_t> loadEpisode18{1};
inline std::atomic<bool> loadingObserved18{false};
inline std::atomic<int> daggers{-1},axes{-1};
inline std::atomic<std::uint64_t> recoveryLease{0};
inline void Publish(bool observed,bool loading) noexcept {
    if(observed&&loadingObserved18.exchange(loading,std::memory_order_acq_rel)!=loading&&loading)
        loadEpisode18.fetch_add(1,std::memory_order_acq_rel);
    const bool blocked=!observed||loading;
    auto prior=state.load(std::memory_order_acquire);
    while(bool(prior&1)!=blocked) {
        const auto next=((prior>>1)+1)*2+unsigned(blocked);
        if(state.compare_exchange_weak(prior,next,std::memory_order_acq_rel)) {
            // Unknown UI knowledge revokes admission, not native lifetime.
            daggers=-1;axes=-1;return;
        }
    }
}
inline void PublishUi(std::uint32_t known,std::uint32_t visible,bool installed,std::uint32_t loadMask) noexcept {
    Publish(installed&&loadMask&&(known&loadMask)==loadMask,(visible&loadMask)!=0);
}
// K34 explicit menu requests are separate from both durable save receipts.
// One atomic ticket binds the request serial to its observed load lease, so a
// menu/load race cannot move a previous click into the next save. Overflow
// refuses instead of recycling an old ticket. Only the script owner services it.
inline std::atomic<std::uint64_t> stockRevision34{0};
inline std::atomic<std::uint64_t> replenishSerial34{0},replenishRequest34{0},replenishDone34{0};
inline bool RequestReplenish() noexcept {
    const auto load=state.load(std::memory_order_acquire);
    if((load&1)||load>0xffffffffull)return false;
    auto serial=replenishSerial34.load(std::memory_order_acquire);
    do {if(serial>=0xffffffffull)return false;}
    while(!replenishSerial34.compare_exchange_weak(serial,serial+1,std::memory_order_acq_rel));
    ++serial;
    const auto ticket=(load<<32)|serial;
    auto prior=replenishRequest34.load(std::memory_order_acquire);
    while(prior<ticket&&!replenishRequest34.compare_exchange_weak(prior,ticket,std::memory_order_acq_rel)){}
    return prior<ticket&&state.load(std::memory_order_acquire)==load;
}
inline bool ReplenishWanted() noexcept {
    const auto load=state.load(std::memory_order_acquire);
    const auto ticket=replenishRequest34.load(std::memory_order_acquire);
    return !(load&1)&&ticket&&(ticket>>32)==load&&
        ticket!=replenishDone34.load(std::memory_order_acquire);
}
inline void RequestRecovery() noexcept {
    auto prior=state.load(std::memory_order_acquire);
    while(!(prior&1)) {
        recoveryLease.store(prior+2,std::memory_order_release);
        if(state.compare_exchange_weak(prior,prior+2,std::memory_order_acq_rel))return;
    }
}
inline void Recheck() noexcept {
    auto prior=state.load(std::memory_order_acquire);
    while(!(prior&1)&&!state.compare_exchange_weak(prior,prior+2,std::memory_order_acq_rel)){}
}
}
