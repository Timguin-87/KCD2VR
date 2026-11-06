#pragma once
#include <cmath>
#include <cstdint>
namespace kcd2vr::quiver114 {
// This is an INPUT gesture ticket, not an arrow/inventory object. Native bow
// preparation still validates ammunition and creates the real charge action.
struct State { bool neutral=false,down=false,carrying=false;std::uint64_t since=0,taken=0,tick=0; };
inline bool update(State& s,bool eligible,double grip,double shoulderDistance,std::uint64_t now,double radius=.22) noexcept {
    if(!std::isfinite(radius)||radius<.10||radius>.45)radius=.22; // V66.146R1 configurable zone
    if(!eligible||!now||!std::isfinite(grip)||!std::isfinite(shoulderDistance)||shoulderDistance<0||
       (s.tick&&(now<s.tick||now-s.tick>80))){s={};return false;}
    if(now==s.tick)return s.carrying;
    s.tick=now;
    if(grip<.25){s.neutral=true;s.down=false;s.carrying=false;s.since=0;return false;}
    if(grip>.55&&!s.down){s.down=true;if(s.neutral&&shoulderDistance<radius)s.since=now;s.neutral=false;}
    if(s.since&&!s.carrying){
        if(shoulderDistance>=radius||grip<=.55)s.since=0;
        else if(now-s.since>=40){s.carrying=true;s.taken=now;s.since=0;}
    }
    if(s.carrying&&now-s.taken>10000){s.carrying=false;s.since=0;}
    return s.carrying;
}
inline void consume(State& s) noexcept {s.carrying=false;s.since=0;s.neutral=false;}
// +28A1157 branches at >=1.0 to the game's charge-complete transition. Never
// cap at .995: that prevents the native hold/fatigue state from being reached.
inline float chargeTarget(float physical,float nativeTimeCap) noexcept {
    return std::fmax(.001f,std::fmin(1.f,std::fmin(physical,nativeTimeCap)));
}
}
