#pragma once
// R116 alchemy selection follows what you SEE and what your hand REACHES.
// Miki R115 (6 Nov): "the selection view is moved to extremes - I need to look like 30cm to the right
// to pick the left item with my right hand - my right hand also do not start the correct selection".
// R114 log: with idle hands the station selection came from the head path in OnScreenDetour, which
// keeps the NATIVE eye origin (authored alchemy camera) and, for every item outside its 10 deg cone,
// fell back to the native screen projection of that camera. nativeToVisual was 16-65 deg the whole
// session (V64.3-INTERACT rows 01:26:39-01:27:53), so the game picked what the native camera saw,
// not what the headset saw. the right hand's nominee (79392/7, gap 6-9 cm) was offered and every
// other item filtered (steer=637/0/8986) but the native result stayed 0 or the neighbour 79389/8:
// the last native gate is the visibility ray, which we started INSIDE the palm/fingers.
// pure helpers only (no native reads); used by interaction_native_selection68.inl,
// interaction_alignment.cpp (OnScreen) and interaction_alchemy_names_d75.inl (names).
// INI VrAlchemyViewSelectR116 (default true), Experiments SMITH page row 8. OFF = R115 behaviour.
#include <atomic>
#include <cmath>
#include "geometry68.h"

namespace kcd2vr::subsystems::alchemy::head_select_r116 {
inline std::atomic<bool> enabled{true};
namespace g=geometry68;
inline constexpr double Reach=2.0;     // same as the D75 name ray
inline constexpr double ConeDeg=6.0;   // no box hit: nearest box centre within 6 deg of the view centre
inline constexpr double LosBack=0.02;  // hand visibility ray starts 2 cm in front of the item box

// one candidate box against the VR eye ray. exact hits always beat cone picks.
// R118: among hits the box whose CENTRE is closest to the view centre wins (was: nearest hit).
// "nearest" let a neighbour box that the ray only clips at its edge, in front, steal the item you
// aim at - at the edge of the table you had to look past the item to get it.
struct Score {bool valid=false,hit=false;double key=0;};
inline double centreDeg(const g::Bounds& b,g::Point eye,g::Point forward) noexcept { // forward normalised
    const auto c=g::transform(b,{(double(b.local[0])+b.local[3])*.5,(double(b.local[1])+b.local[4])*.5,(double(b.local[2])+b.local[5])*.5});
    const auto d=g::sub(c,eye);const double len=std::sqrt(g::dot(d,d));
    if(!std::isfinite(len)||len<1.e-3)return 180.;
    return std::acos(std::clamp(g::dot(d,forward)/len,-1.,1.))*57.29577951308232;
}
inline Score score(const g::Bounds& b,g::Point eye,g::Point forward,double reach=Reach) noexcept {
    Score out{};
    if(!g::valid(b)||!g::finite(eye)||!g::finite(forward))return out;
    const double n=std::sqrt(g::dot(forward,forward));
    if(!std::isfinite(n)||n<.5)return out;
    forward=g::mul(forward,1./n);
    const auto hit=g::ray(b,eye,forward,reach);
    const double deg=centreDeg(b,eye,forward);
    if(hit.valid){out.valid=out.hit=true;out.key=deg;return out;}
    const auto c=g::transform(b,{(double(b.local[0])+b.local[3])*.5,(double(b.local[1])+b.local[4])*.5,(double(b.local[2])+b.local[5])*.5});
    const auto d=g::sub(c,eye);const double len=std::sqrt(g::dot(d,d));
    if(!std::isfinite(len)||len<1.e-3||len>reach)return out;
    if(!(deg<=ConeDeg))return out;
    out.valid=true;out.key=deg;return out;
}
inline bool better(const Score& a,const Score& b) noexcept { // a better than b
    if(!a.valid)return false;
    if(!b.valid)return true;
    if(a.hit!=b.hit)return a.hit;
    return a.key<b.key-1.e-9;
}
// origin for the native visibility ray of a HAND nominee: on the palm->endpoint line, 2 cm before
// it enters the item's own box. a palm inside the box keeps the palm. the box is only 5-14 cm away
// (near67/craft1 reach), so nothing but the item itself can be in the way - not our fingers.
inline g::Point losOrigin(const g::Bounds& b,g::Point palm,g::Point endpoint) noexcept {
    if(!g::valid(b)||!g::finite(palm)||!g::finite(endpoint))return palm;
    const auto d=g::sub(endpoint,palm);const double len=std::sqrt(g::dot(d,d));
    if(!std::isfinite(len)||len<1.e-4||len>Reach)return palm;
    const auto dir=g::mul(d,1./len);
    const auto hit=g::ray(b,palm,dir,len+.01);
    if(!hit.valid||!(hit.distance>LosBack))return palm;
    return g::add(palm,g::mul(dir,hit.distance-LosBack));
}
}
