#pragma once
// C26d (Miki 16 Oct): native eye adaptation reacts to whatever is on screen, so looking down at dark ground or turning
// the head pumps the brightness. The game has no "adapt to where Henry stands" option. We only change HOW FAST the
// native adaptation runs (r_HDREyeAdaptationSpeed), on edges:
//   head turning fast (> 60 deg/s)            -> PAUSED (speed 0.01: the current exposure is kept)
//   looking far down/up (|pitch| > 35 deg)    -> SLOW   (speed x0.25)
//   head steady and roughly level for 300 ms  -> NATIVE (the captured original speed)
// Walking from shade into sun with a steady head still adapts at the native speed. Pure logic; the caller writes the
// cvar only when the returned level changes. No hook, no timer.
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace kcd2vr::exposure26 {
enum class Level {Native, Slow, Paused};
inline const char* name(Level l) noexcept {return l==Level::Paused?"PAUSED_HEAD_TURN":l==Level::Slow?"SLOW_LOOKING_DOWN_OR_UP":"NATIVE";}
struct Q {float x=0,y=0,z=0,w=1;};
inline bool Valid(Q q) noexcept {
    const double n=double(q.x)*q.x+double(q.y)*q.y+double(q.z)*q.z+double(q.w)*q.w;
    return std::isfinite(n)&&n>=.8&&n<=1.2;
}
struct V {double x=0,y=0,z=0;};
inline V forward(Q q) noexcept { // OpenXR view looks along -Z
    const double x=q.x,y=q.y,z=q.z,w=q.w;
    return {-(2*(x*z+w*y)),-(2*(y*z-w*x)),-(1-2*(x*x+y*y))};
}
struct State {bool have=false;V last{};std::uint64_t lastTick=0,calmSince=0;Level level=Level::Native;};
struct Tuning {double turnDegS=60,calmDegS=25,pitchDeg=35;std::uint64_t calmMs=300;};
inline Level Step(State& s,bool valid,Q q,std::uint64_t now,const Tuning& t={}) noexcept {
    if(!valid||!Valid(q)){s={};return Level::Native;}
    if(s.have&&now==s.lastTick)return s.level; // duplicate samples cannot mint a native-CVar edge
    const V f=forward(q);const double n=std::sqrt(f.x*f.x+f.y*f.y+f.z*f.z);if(n<1e-6){s={};return Level::Native;}
    const V u{f.x/n,f.y/n,f.z/n};
    const double pitch=std::asin(std::clamp(u.y,-1.,1.))*57.29577951308232;
    double speed=0;
    if(s.have&&now>s.lastTick&&now-s.lastTick<=100){
        const double d=std::clamp(u.x*s.last.x+u.y*s.last.y+u.z*s.last.z,-1.,1.);
        speed=std::acos(d)*57.29577951308232/(double(now-s.lastTick)/1000.0);
    }
    const bool fresh=s.have&&now>s.lastTick&&now-s.lastTick<=100;
    s.last=u;s.lastTick=now;s.have=true;
    if(!fresh){s.level=Level::Native;s.calmSince=0;return s.level;}
    if(speed>t.turnDegS){s.level=Level::Paused;s.calmSince=0;return s.level;}
    if(speed>t.calmDegS){s.calmSince=0;return s.level;}
    if(!s.calmSince)s.calmSince=now;
    if(now-s.calmSince<t.calmMs)return s.level;
    s.level=std::fabs(pitch)>t.pitchDeg?Level::Slow:Level::Native;
    return s.level;
}
inline float SpeedFor(Level l,float native) noexcept {
    const float n=std::isfinite(native)&&native>0?native:3.f;
    return l==Level::Paused?.01f:l==Level::Slow?n*.25f:n;
}
}
