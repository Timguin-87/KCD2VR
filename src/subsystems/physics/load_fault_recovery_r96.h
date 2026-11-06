#pragma once
#include "../../core/observed_load_recovery_r96.h"
#include <atomic>
#include <cstdint>

namespace kcd2vr::load_fault_recovery_r96 {
// A native exception stops this capability for the current observed load.
// Completion of a real load admits a new, fully qualified call. It never
// restores a held request, forgives a failed native cleanup, or proves death
// of an old engine object. Capture BEFORE the call so a late old exception
// cannot quarantine a newer load that was not involved in that call.
class Fault {
    std::atomic<std::uint64_t> failed_{0};
public:
    using Witness=observed_load_recovery_r96::Witness;
    static Witness capture() noexcept {return observed_load_recovery_r96::observe();}
    void record(const Witness& witness) noexcept {
        const auto value=(witness.episode<<1)|std::uint64_t(witness.loading);
        if(!value)return;
        auto prior=failed_.load(std::memory_order_acquire);
        while(prior<value&&!failed_.compare_exchange_weak(prior,value,std::memory_order_acq_rel)){}
    }
    bool load(std::memory_order order=std::memory_order_acquire) const noexcept {
        const auto value=failed_.load(order);
        return value&&!observed_load_recovery_r96::completedSince({value>>1,bool(value&1),false});
    }
    // Explicit reset is for construction/fixture isolation, not menu recovery.
    void clear() noexcept {failed_.store(0,std::memory_order_release);}
};
}
