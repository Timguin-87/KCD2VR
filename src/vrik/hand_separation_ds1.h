#pragma once
// DS1 (DeepSeek, 27 Oct): hand-hand / hand-weapon self-separation, pure math.
// Miki: "hands can still clip through each other or the sword through them".
// This is a SOFT post-solve pass: it splits the remaining overlap between the
// two wrists along their world axis, never teleports, never fights the arm IK
// (each push is half the overlap and bounded by the palm radius). It does not
// create rigid bodies, does not read native state and never authorizes hits.
#include "pose_math.h"
#include <cmath>
namespace kcd2vr::hand_separation_ds1 {
namespace m=pose_core;
struct Limits {
    double palmRadius=.06;   // palms closer than 2x this are considered overlapping
    double weaponSkin=.02;   // off-hand palm stops this far from the held segment
    double maxPush=.05;      // per-hand push cap per frame (no teleport feel)
};
struct Result {
    m::Vec3 delta[2]{};      // world-space push for each hand (model-scale caller applies)
    bool pushedHands=false, pushedWeapon=false;
};
// Split a hand-hand overlap: if the two world palm positions are closer than
// 2*palmRadius, push each palm half the overlap away along the connecting axis.
// A zero-length axis (exact same point) falls back to +X for the right hand.
inline Result separateHands(m::Vec3 palmL,m::Vec3 palmR,const Limits& lim) noexcept {
    Result out{};
    if(!m::finite(palmL)||!m::finite(palmR))return out;
    const m::Vec3 axis=palmR-palmL;
    const double d=m::length(axis);
    const double minD=2.0*lim.palmRadius;
    if(d>=minD)return out;
    m::Vec3 dir{};
    if(d>1e-9)dir=axis*(1.0/d);else dir={1.0,0.0,0.0};
    const double push=std::min((minD-d)*.5,lim.maxPush);
    out.delta[0]=dir*-push;out.delta[1]=dir*push;
    out.pushedHands=true;
    return out;
}
// Push the off-hand palm off a held weapon segment (the handle line in world
// space). Only the CLOSEST point's normal is used; a palm already farther than
// weaponSkin is untouched. segmentA/B must be distinct, finite points.
inline Result separateWeapon(m::Vec3 palm,m::Vec3 segA,m::Vec3 segB,const Limits& lim) noexcept {
    Result out{};
    if(!m::finite(palm)||!m::finite(segA)||!m::finite(segB))return out;
    const m::Vec3 ab=segB-segA;
    const double len2=m::dot(ab,ab);
    if(len2<=1e-12)return out;
    const double t=std::clamp(m::dot(palm-segA,ab)/len2,0.0,1.0);
    const m::Vec3 closest=segA+ab*t;
    const m::Vec3 diff=palm-closest;
    const double d=m::length(diff);
    if(d>=lim.weaponSkin)return out;
    m::Vec3 n{};
    if(d>1e-9)n=diff*(1.0/d);else n={0.0,0.0,1.0};
    const double push=std::min(lim.weaponSkin-d,lim.maxPush);
    out.delta[0]=n*push;
    out.pushedWeapon=true;
    return out;
}
}
