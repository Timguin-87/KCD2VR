#pragma once
// R121 (Miki 6 Nov, R118 log): "the item selection in immersive mode is wonky a lot - sometimes not showing the
// interaction on some activities (build menu, cooking or eating soup, washing) - I need to reset orientation, then it
// works a little, but the interaction ray is not in my view (after activities my head stays in a different position)".
// the native look ray (interaction_alignment.cpp RayBuildDetour) took its DIRECTION from the headset view but kept the
// game's own eye as ORIGIN. the rendered eye is that game eye + your head offset from the recenter point. after a
// station you stand where you leaned to the table: R118 log 10:59:45 and 11:03:41 V66.178R58 ROOMSCALE remaining=
// (0.07,0.28)/(-0.02,0.27) = headset 27-28 cm forward of the recenter point (before the stations 2-7 cm, after his
// recenter at 11:04:11 -0.02..0.09). so the ray ran parallel to your view line but 25-30 cm behind/above it: close
// targets (pot, basin, bench) were missed or another object won, until a recenter put the two eyes together again.
// R118 fixed this only at the alchemy table (AlchemyEyeRayR118). now every head look ray starts at the rendered eye
// with the rendered forward, when that eye is a normal first-person eye (within kMaxShiftM of the game eye). a
// dialogue/cutscene/station camera further away keeps the game eye. reach, object masks, Use and all native checks
// stay native. INI VrInteractionEyeRayR121 (default ON), Experiments GEAR page 2. no latch: decided per ray.
#include <atomic>
#include <cmath>

namespace kcd2vr::eye_ray_r121 {
inline std::atomic<bool> enabled{true};
constexpr float kMaxShiftM=0.60f; // head offset from the game eye still taken as "your eye" (lean, step, crouch)
// true = the look ray starts at the rendered eye (eye) instead of the game eye (native). both world metres.
inline bool useRenderedEye(bool on,const float* native,const float* eye) noexcept {
    if(!on||!native||!eye)return false;
    double d2=0;
    for(int i=0;i<3;++i){
        if(!std::isfinite(native[i])||!std::isfinite(eye[i]))return false;
        const double d=double(eye[i])-double(native[i]);d2+=d*d;
    }
    return d2<=double(kMaxShiftM)*double(kMaxShiftM);
}
}
