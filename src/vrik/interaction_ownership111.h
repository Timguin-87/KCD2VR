#pragma once
namespace kcd2vr::interaction_ownership111 {

inline bool Live(unsigned f3,unsigned f5,bool uiBlocked,bool calibrated,bool physicalCombatBusy=false) noexcept {
    (void)f3; // V66.135: F3 owns combat diagnostics, never the interaction lab.
    if(uiBlocked||!calibrated||physicalCombatBusy)return false;
    return f5==0u||f5==2u;
}
// F5=2 is the live rung; 3 is retained for old saved diagnostic fixtures. Fist
// ownership and stale/unknown weapon reservations are rejected by the caller.
inline bool Comparison114(unsigned f3,unsigned f5,bool uiBlocked,bool calibrated) noexcept {
    (void)f3;
    return (f5==2u||f5==3u)&&!uiBlocked&&calibrated;
}
inline bool HandEligible(bool tracked,int itemState,bool neutral,bool uiGripOwned) noexcept {
    return tracked&&itemState==0&&neutral&&!uiGripOwned;
}

} // namespace kcd2vr::interaction_ownership111
