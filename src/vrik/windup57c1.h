#pragma once
#include "pose_math.h"
#include <cstdint>
namespace kcd2vr::windup57c1 {
namespace m=pose_core;
// Gesture input only. No native state, damage, or artificial charged-attack flag.
// Both deltas use the calibrated body axes; one is model motion, the other
// comes from the independently tracked head-relative hand. Neither alone arms.
struct Sample {std::uint64_t tick=0;double dt=0;m::Vec3 position{},modelDelta{},relativeDelta{};bool eligible=false;};
inline bool region(m::Vec3 p) noexcept {
    return m::finite(p)&&p.y<.08&&p.z>-.15&&(std::abs(p.x)>.14||p.z>.22);
}
// R59.16C10R14: same convention as stroke_zone117 / thrust_origin167 - the zone is the side the stroke STARTS from
// (hand on Henry's left, x<0 -> 1 LEFT; x>=0 -> 2 RIGHT). Before R14 this was inverted against the cut rule.
inline int zone(m::Vec3 p) noexcept {return p.z>.20?0:(p.x<0?1:2);}
struct Arm {
    std::uint64_t last=0,start=0,settled=0;double backward=0;
    void reset() noexcept {*this={};}
    bool update(Sample s) noexcept {
        if(!s.eligible||!s.tick||!std::isfinite(s.dt)||s.dt<=0||s.dt>.08||
           !m::finite(s.position)||!m::finite(s.modelDelta)||!m::finite(s.relativeDelta)) {reset();return false;}
        if(last&&s.tick==last)return false;
        if(last&&(s.tick<last||s.tick-last>80))reset();
        last=s.tick;
        const double a=m::length(s.modelDelta)/s.dt,b=m::length(s.relativeDelta)/s.dt;
        if(a>8||b>8){reset();return false;}
        const double back=std::min(-s.modelDelta.y,-s.relativeDelta.y);
        const bool deliberate=back/s.dt>=.20&&back>=.65*std::max(m::length(s.modelDelta),m::length(s.relativeDelta));
        if(deliberate){
            if(!start||s.tick-start>700){start=s.tick;backward=0;}
            backward+=back;settled=0;
        }else {
            if(start&&s.tick-start>900){reset();return false;}
            // Measure net backward progress: small forward reversals must not
            // accumulate into a wind-up by repeatedly shaking in place.
            backward=std::max(0.0,backward-std::max(0.0,std::max(s.modelDelta.y,s.relativeDelta.y)));
        }
        if(backward>=.12&&region(s.position)&&a<.35&&b<.35){
            if(!settled)settled=s.tick;
            if(s.tick-settled>=150){reset();return true;}
        }else settled=0;
        if(s.modelDelta.y>.02||s.relativeDelta.y>.02){reset();return false;}
        return false;
    }
};
}
