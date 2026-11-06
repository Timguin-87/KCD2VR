#pragma once
#define KCD2VR_REACH_VIEW38_H 1
// C38 REACH VIEW (pure logic, host-tested; no native state, no pose writes).
// Miki (17 Oct): "both hands extended forward (over an edge, in dialogue) disappear". The VRIK arm path already clamps an
// over-reach at full extension along shoulder->controller (soft_reach_cap124 + solve_arm, see reach_view38_test), so the
// remaining causes are downstream of the pose: (a) the hand is behind the eye plane (pose/camera mismatch), or (b) Henry's
// mesh is not drawn because the game's visibility uses the NATIVE animated pose (we write at skinning time) and the
// headset moved the eye out of that native body. This file only measures both, edge-only, under F5 rung 1.
#include <cmath>
#include <cstdint>
#include <algorithm>

namespace kcd2vr::reach_view38 {
struct V {double x=0,y=0,z=0;};
inline V sub(V a,V b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline double dot(V a,V b) noexcept {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline bool finite(V a) noexcept {return std::isfinite(a.x)&&std::isfinite(a.y)&&std::isfinite(a.z);}
// Axis-aligned box of the native (pre-VRIK) bone positions in model space. Bones only: the skinned mesh and the game's own
// bounds are a few cm larger, so 'outside' is an upper estimate of how far the eye/hand left the native body.
struct Box {V lo{1e30,1e30,1e30},hi{-1e30,-1e30,-1e30};unsigned n=0;};
inline void grow(Box& b,V p) noexcept {
    if(!finite(p))return;
    b.lo={std::min(b.lo.x,p.x),std::min(b.lo.y,p.y),std::min(b.lo.z,p.z)};
    b.hi={std::max(b.hi.x,p.x),std::max(b.hi.y,p.y),std::max(b.hi.z,p.z)};++b.n;
}
inline bool valid(const Box& b) noexcept {return b.n>=2&&b.lo.x<=b.hi.x&&b.lo.y<=b.hi.y&&b.lo.z<=b.hi.z;}
// Euclidean distance of p outside the box (0 inside). -1 when unknown.
inline double outside(const Box& b,V p) noexcept {
    if(!valid(b)||!finite(p))return -1;
    const double dx=std::max({b.lo.x-p.x,0.0,p.x-b.hi.x}),dy=std::max({b.lo.y-p.y,0.0,p.y-b.hi.y}),dz=std::max({b.lo.z-p.z,0.0,p.z-b.hi.z});
    return std::sqrt(dx*dx+dy*dy+dz*dz);
}
// Metres of p in front of the eye along the view direction (negative = behind the eye plane, never visible).
inline double ahead(V eye,V view,V p) noexcept {
    const double n=std::sqrt(dot(view,view));
    if(!finite(eye)||!finite(view)||!finite(p)||!(n>1e-6))return 0;
    return dot(sub(p,eye),view)/n;
}
// True when the whole box lies behind the eye plane: then a view-frustum test on the native body fails for any field of view.
inline bool behindEye(const Box& b,V eye,V view) noexcept {
    if(!valid(b))return false;
    for(int i=0;i<8;++i){const V c{i&1?b.hi.x:b.lo.x,i&2?b.hi.y:b.lo.y,i&4?b.hi.z:b.lo.z};if(ahead(eye,view,c)>0)return false;}
    return true;
}
// Hysteresis flag: +1 when value rises to onAt, -1 when it falls below offAt (offAt<onAt), 0 otherwise.
struct Flag {bool on=false;std::uint64_t since=0;};
inline int edge(Flag& f,double value,double onAt,double offAt,std::uint64_t now) noexcept {
    if(!std::isfinite(value))return 0;
    if(!f.on&&value>=onAt){f.on=true;f.since=now;return 1;}
    if(f.on&&value<offAt){f.on=false;return -1;}
    return 0;
}
// Henry skinning gap while VRIK is otherwise running: if the native visibility culls Henry, the skinning callback that
// carries our pose write stops (shadow passes may still skin him, so absence of a gap is not proof of visibility).
struct Gap {bool open=false;std::uint64_t since=0,lastSkin=0;};
inline int gap(Gap& g,std::uint64_t now,std::uint64_t skinTick,bool eligible,std::uint64_t thresholdMs) noexcept {
    if(skinTick>g.lastSkin)g.lastSkin=skinTick; // the producer tick is monotonic; never let an older copy reopen a gap
    const bool stale=eligible&&g.lastSkin&&now>=g.lastSkin&&now-g.lastSkin>thresholdMs;
    if(!g.open&&stale){g.open=true;g.since=g.lastSkin;return 1;}
    if(g.open&&(!eligible||(g.lastSkin&&now>=g.lastSkin&&now-g.lastSkin<=thresholdMs))){g.open=false;return -1;}
    return 0;
}
// One committed VRIK frame, copied under F5 rung 1 only.
struct Evidence {
    std::uint64_t tick=0,sample=0,epoch=0;
    bool full=false,workstation=false,dialogueBody=false,liveAnchor=false,raiseCapped=false;
    bool valid[2]{},reachLimited[2]{};
    double reachAsk[2]{},handAhead[2]{},handOutside[2]{};
    double eyeOutside=-1,headErr=0;
    bool nativeBehindEye=false;
    V eye{},view{},hand[2]{},nativeHeadFromCal{},hmdDelta{},headTarget{},solvedHead{},chest{};
    Box native{};
};
}
