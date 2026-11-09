#pragma once
#include <atomic>

namespace kcd2vr::quick_access_r112 {
// Explicit preference, never inferred from headset name or a released sensor.
// State bits 0/1 report available sensors; bits 2-3 select right thumbrest,
// left thumbrest, or face touch.
// One atomic prevents an old-source publication overwriting a mode-change clear.
inline std::atomic<unsigned> state{0};
enum ModifierMode : int {RightThumbrest=0,LeftThumbrestMode=1,FaceTouchMode=2};
constexpr int ClampMode(int mode) noexcept {return mode>=0&&mode<=2?mode:RightThumbrest;}
inline void SetMode(int mode) noexcept {
    unsigned previous=state.load(std::memory_order_acquire);
    const unsigned bits=static_cast<unsigned>(ClampMode(mode))<<2;
    while((previous&12u)!=bits&&!state.compare_exchange_weak(previous,bits,
        std::memory_order_acq_rel,std::memory_order_acquire)){}
}
inline int Mode() noexcept {return int((state.load(std::memory_order_acquire)>>2)&3u);}
inline void SetFaceTouch(bool enabled) noexcept {SetMode(enabled?FaceTouchMode:RightThumbrest);}
inline bool FaceTouch() noexcept {return Mode()==FaceTouchMode;}
inline bool LeftThumbrest() noexcept {return Mode()==LeftThumbrestMode;}
inline unsigned AvailableHands() noexcept {return state.load(std::memory_order_acquire)&3u;}
inline void ClearAvailable() noexcept {state.fetch_and(12u,std::memory_order_acq_rel);}
inline void PublishAvailable(int selectedMode,unsigned mask) noexcept {
    unsigned previous=state.load(std::memory_order_acquire);
    const unsigned mode=static_cast<unsigned>(ClampMode(selectedMode))<<2;
    while((previous&12u)==mode){
        if(state.compare_exchange_weak(previous,mode|(mask&3u),
            std::memory_order_acq_rel,std::memory_order_acquire))return;
    }
}
}
