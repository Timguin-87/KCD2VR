#pragma once
#include <atomic>
#include <cstdint>
#include "native_roomscale_d76.h"

namespace kcd2vr::roomscale_d76 {
inline std::atomic<bool> nativeSteps{false},movementContext{false},recenterPending{false};
inline std::atomic<std::uint64_t> selectionRevision{1},originRevision{1};
inline bool Enabled() noexcept { return nativeSteps.load(std::memory_order_acquire); }
inline void SetEnabled(bool on) noexcept {
    if(nativeSteps.exchange(on,std::memory_order_acq_rel)!=on){
        selectionRevision.fetch_add(1,std::memory_order_acq_rel);
        if(on){(void)native_roomscale_d76::Install();}
        // Turning the feature off stops new requests. It must NOT erase the
        // coordinate origin already consumed by native physics.
        native_roomscale_d76::CancelPending();
    }
    native_roomscale_d76::SetAdmission(on&&movementContext.load(std::memory_order_acquire)&&!recenterPending.load(std::memory_order_acquire));
}
inline void Context(bool allowed) noexcept {
    movementContext.store(allowed,std::memory_order_release);
    native_roomscale_d76::SetAdmission(allowed&&Enabled()&&!recenterPending.load(std::memory_order_acquire));
}
}
