#pragma once
namespace kcd2vr::head_aim120r3 {
// V66.163: retired after headset nausea/rotation regressions. An old INI or
// legacy diagnostic rung must never transfer HMD rotation to mouse input.
inline constexpr bool kEnabled = false;
inline bool RangedHeadAim(int equipment,float trigger) noexcept { return equipment==1 && trigger>0.55f; }
inline bool Allowed(bool requested,bool retail,bool blocked,bool trackingFresh,bool anglesValid,bool cameraLab,int equipment,float trigger) noexcept {
    return kEnabled&&requested&&retail&&!blocked&&trackingFresh&&anglesValid&&!cameraLab&&RangedHeadAim(equipment,trigger);
}
}
