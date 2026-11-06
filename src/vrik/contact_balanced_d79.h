#pragma once
#include <cmath>
// Saved hand-contact mode2: bounded measured palm + thumb/index features.
// This does not own finger curls, create bodies or increase native query budgets.
namespace kcd2vr::contact_balanced_d79 {
inline constexpr unsigned kFeatures=7;
inline constexpr float kSkinM=.002f;
inline constexpr bool selected(int mode,bool station) noexcept {return mode==2&&!station;}
// Keep stable feature identities across every controller pose; an argmax tip
// would silently replace geometry at an edge. Input features use the accepted
// free-finger recipe, never a collision-induced curl.
template<class V> void compose(const V* palm,const V* finger,V* out) noexcept {
    for(unsigned i=0;i<5;++i)out[i]=palm[i];
    out[5]=finger[1]; // thumb tip
    out[6]=finger[2]; // index tip
}
// A release candidate can stop inside positive clearance before the real
// surface. Probe through that clearance instead of forcing a16mm wrist jump.
// This extends the SAME current native query, not an old plane or extra ray.
template<class V> V clearanceSweep(V sweep,float skin,bool releasing) noexcept {
    if(!releasing||!std::isfinite(skin)||skin<=0)return sweep;
    const float length=std::sqrt(sweep.x*sweep.x+sweep.y*sweep.y+sweep.z*sweep.z);
    if(!std::isfinite(length)||length<.0001f)return sweep;
    const float scale=1.f+(skin+.0001f)/length;
    return {sweep.x*scale,sweep.y*scale,sweep.z*scale};
}
}
