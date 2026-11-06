#pragma once
#include <atomic>

namespace kcd2vr::quick_access_r112 {
// Explicit preference, never inferred from headset name or a released sensor.
// UI evidence only: bit 2 selects face touch, bits 0/1 mean left/right available.
// One atomic prevents an old-source publication overwriting a mode-change clear.
inline std::atomic<unsigned> state{0};
inline void SetFaceTouch(bool enabled) noexcept {
    unsigned previous=state.load(std::memory_order_acquire);
    const unsigned mode=enabled?4u:0u;
    while((previous&4u)!=mode&&!state.compare_exchange_weak(previous,mode,
        std::memory_order_acq_rel,std::memory_order_acquire)){}
}
inline bool FaceTouch() noexcept {return (state.load(std::memory_order_acquire)&4u)!=0;}
inline unsigned AvailableHands() noexcept {return state.load(std::memory_order_acquire)&3u;}
inline void ClearAvailable() noexcept {state.fetch_and(4u,std::memory_order_acq_rel);}
inline void PublishAvailable(bool selectedFaceTouch,unsigned mask) noexcept {
    unsigned previous=state.load(std::memory_order_acquire);
    const unsigned mode=selectedFaceTouch?4u:0u;
    while((previous&4u)==mode){
        if(state.compare_exchange_weak(previous,mode|(mask&3u),
            std::memory_order_acq_rel,std::memory_order_acquire))return;
    }
}
}
