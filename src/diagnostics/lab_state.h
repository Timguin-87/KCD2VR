#pragma once
// V66.90. Sole temporary F3/F5 owner. No native calls, input, damage or calibration resets.
// Latest user mapping: F3 holster/equipment + combat; F5 miscellaneous interaction.
#include <atomic>
#include <cstdint>
namespace kcd2vr::diagnostics::lab {
enum class Key : unsigned { F3=0, F5=1 };
enum class Mode : unsigned { Off=0, NativeReference=1, VrikObserve=2, NativeMotionExperiment=3 };
inline std::atomic<bool> weaponRestorePending{false};
inline std::atomic<std::uint64_t> state{0}; // modes in low 4 bits, epoch above
inline unsigned Get(std::uint64_t s,Key key) noexcept {return unsigned((s>>(unsigned(key)*2))&3u);}
inline std::uint64_t Change(std::uint64_t s,Key key,unsigned mode) noexcept {
    mode=mode>3u?0:mode;const unsigned shift=unsigned(key)*2;
    unsigned bits=unsigned(s)&15u;bits=(bits&~(3u<<shift))|(mode<<shift);
    // V66.135: F3 and F5 are independent diagnostic domains. F3 may alter only
    // weapon/combat/collision ownership; F5 may alter only interaction ownership.
    // Hand reservations remain the sole arbitration when both labs are live.
    return (((s>>4)+1)<<4)|bits;
}
inline std::uint64_t Set(Key key,unsigned mode) noexcept {
    auto old=state.load(std::memory_order_acquire);
    for(;;){const auto next=Change(old,key,mode);if(state.compare_exchange_weak(old,next,std::memory_order_acq_rel))return next;}
}
inline std::uint64_t Snapshot() noexcept{return state.load(std::memory_order_acquire);}
inline unsigned Get(Key key) noexcept{return Get(Snapshot(),key);}
inline bool Any() noexcept{return (Snapshot()&15u)!=0;}
inline bool CombatReference() noexcept{return Get(Snapshot(),Key::F3)==1;}
inline bool InteractionReference() noexcept{return Get(Snapshot(),Key::F5)==1;}
inline bool AnyReference() noexcept{return CombatReference()||InteractionReference();}
// Historical name retained for weapon/body call sites: native combat reference is F3 only.
inline bool NativeReference() noexcept{return CombatReference();}
inline unsigned Mask() noexcept{const auto s=Snapshot();return (Get(s,Key::F3)?1u:0u)|(Get(s,Key::F5)?2u:0u);}
inline bool WantsVrik() noexcept{const auto s=Snapshot();return Get(s,Key::F3)>=2||Get(s,Key::F5)>=2;}
inline const char* Name(unsigned mode) noexcept {
    return mode==1?"NATIVE_REFERENCE":mode==2?"VRIK_OBSERVE":mode==3?"NATIVE_MOTION_EXPERIMENT":"OFF";
}
// 'Live' here means normal calibrated VRIK + observation, NOT a physical backend.
// No unsupported physical test is advertised as active.
inline constexpr bool PhysicalGrabImplemented=false;
inline constexpr bool HolsterGestureImplemented=true; // native draw/stow requests; runtime acceptance still required
inline constexpr bool NativeHitInjectionImplemented=false;
// Mode lease runs on the mod control thread, like the existing F10 setter. The
// render/animation callbacks only read atomically published modes. No game calls.
struct PoseLease {
    bool session=false, pendingLive=false, autoOwned=false;
    int base=0;
    void Begin(int current) noexcept { if(!session){session=true;base=current;autoOwned=false;} }
    void RequestLive() noexcept {if(session)pendingLive=true;}
    void ExternalPoseChoice(int current) noexcept {
        if(session){base=current;autoOwned=false;pendingLive=false;}
    }
    int Poll(bool wantsLive,bool calibrated,int current) noexcept {
        if(!session||!pendingLive||!wantsLive||!calibrated)return -1;
        pendingLive=false;
        if(current>=2)return -1;
        autoOwned=true;return 2; // V66.92R1 runtime-recovery default; never auto-enter collision
    }
    int End() noexcept { const int restore=autoOwned?base:-1;*this={};return restore; }
};

}
