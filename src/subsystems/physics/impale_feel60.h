#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
// DS-F108: the IMPALE FEEL LAYER (from the ImmersiveWeaponPenetrationVR design
// blueprint) - pure, host-tested. The constraint hold pins the corpse part to
// the blade; this policy models the LODGED-BLADE FEEL around it:
//   - slide resistance + the exit-grab (the final tug before the blade frees);
//   - wrist stiffness + the max twist (the grind limit while lodged);
//   - the ragdoll-freefall gate (a stationary blade releases the pin so GRAVITY
//     owns the corpse between drags - the stabbed-ragdoll float fix);
//   - the drag-strength split (a ragdoll is dragged at a fraction of a living
//     victim's full strength).
// Nothing writes natively; the hold's motor consumes the outputs.
namespace kcd2vr::impale_feel60 {
struct Params {
    float slideResistance=70.f;  // 0..100: in/out friction (the blade follows 1:1 at 0)
    float exitGrab=55.f;         // 0..100: the final tug before the blade exits
    float wristStiffness=70.f;   // 0..100: the twist grind while lodged
    float wristMaxDegrees=30.f;  // the hard twist limit (degrees)
    float ragdollDrag=35.f;      // % of full drag strength on a ragdoll
    float maxStretch=35.f;       // units: the hand leash before the blade slides free
    std::uint64_t freefallAfterMs=900; // a stationary blade releases the pin after this
};
struct Blade {
    float pos[3]{};float motion[3]{}; // the world position + the per-frame velocity
    std::uint64_t tick=0,serial=0;bool ragdoll=true,gripHeld=true;
};
struct Out {
    float slideFactor=1.f;    // the in/out motion scale (1 = free, <1 = resisted)
    float exitTug=0.f;        // the exit-grab commitment (0 = clean slide-out)
    float twistAllow=0.f;     // the remaining twist allowance (degrees, signed budget)
    float dragScale=1.f;      // the body-drag strength scale
    bool freefall=false;      // released grip; caller retires its motor
    bool valid=false;float leash=35.f;         // the hand leash this frame
};
inline float clamp01(float v) noexcept {return v<0?0:v>1?1:v;}
struct State {std::uint64_t stillSince=0,serial=0;float exitDepth=0;};
inline Out plan(const Params& p,const Blade& b,State& s,std::uint64_t now) noexcept {
    Out o{};
    if(!b.serial||!b.tick||now<b.tick||now-b.tick>80||!std::isfinite(p.maxStretch)||p.maxStretch<=0||
       !std::isfinite(p.slideResistance)||!std::isfinite(p.exitGrab)||!std::isfinite(p.ragdollDrag)||
       !std::isfinite(p.wristStiffness)||!std::isfinite(p.wristMaxDegrees)||p.wristMaxDegrees<0||
       !std::isfinite(b.motion[0])||!std::isfinite(b.motion[1])||!std::isfinite(b.motion[2])){s={};return o;}
    if(s.serial!=b.serial){s={};s.serial=b.serial;}
    o.valid=true;o.leash=p.maxStretch;
    o.dragScale=b.ragdoll?std::clamp(p.ragdollDrag/100.f,.05f,1.f):1.f;
    const float resist=clamp01(p.slideResistance/100.f);
    // slide: the blade follows the hand 1:1 at 0 resistance; at 100 the in/out
    // motion is damped hard. The exit-grab adds the FINAL tug as the blade nears
    // the exit (the caller feeds exitDepth 0..1, 1 = about to free).
    o.slideFactor=1.f-resist*.75f;
    o.exitTug=clamp01(p.exitGrab/100.f)*clamp01(std::isfinite(s.exitDepth)?s.exitDepth:0.f);
    // wrist: the twist budget in degrees, consumed by the caller per frame.
    o.twistAllow=p.wristMaxDegrees;
    (void)o.twistAllow;
    // H21: holding the weapon grip owns the lodged blade even while stationary.
    // Speed never releases/re-adds a constraint. Releasing grip yields native
    // gravity/retirement; no opposite-hand movement participates in this rule.
    o.freefall=!b.gripHeld;s.stillSince=0;
    return o;
}
}
