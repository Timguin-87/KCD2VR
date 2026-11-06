#pragma once
// Contact presentation policy only. No damage, native calls, material guesses,
// allocation, or pose writes. A successful final publication is the caller's gate.
#include "pose_math.h"
#include <cstdint>
#include <algorithm>
#include <cmath>
namespace kcd2vr::passive169r1 {
namespace m=pose_core;
enum class Source : unsigned { WeaponWorld, HandWorld, SelfTorso, OtherPalm };
enum class Status : unsigned { Clear, Contact, Unknown, Unavailable };
enum class Phase : unsigned { None, Begin, Impact, Slide, Rest, End, Suspended, Resumed };
inline const char* name(Source v) noexcept {switch(v){case Source::WeaponWorld:return "WEAPON_WORLD";case Source::HandWorld:return "HAND_WORLD";case Source::SelfTorso:return "SELF_TORSO_PROXY";default:return "OTHER_PALM_PROXY";}}
inline const char* name(Status v) noexcept {switch(v){case Status::Clear:return "CLEAR";case Status::Contact:return "CONTACT";case Status::Unknown:return "UNKNOWN";default:return "UNAVAILABLE";}}
inline const char* name(Phase v) noexcept {switch(v){case Phase::Begin:return "BEGIN";case Phase::Impact:return "IMPACT";case Phase::Slide:return "SLIDE";case Phase::Rest:return "REST";case Phase::End:return "END";case Phase::Suspended:return "SUSPENDED";case Phase::Resumed:return "RESUMED";default:return "NONE";}}
struct Motion {
    // Distances/speeds are in metres and seconds at the SAME feature on the
    // accepted swept path, not displacement accumulated against an obstacle.
    double incoming=0,slipSpeed=0,slipDistance=0;
    bool valid=false;
};
struct Sample {
    Source source=Source::WeaponWorld;Status status=Status::Unavailable;int side=0;
    std::uint64_t actor=0,key=0,generation=0,epoch=0,request=0,sequence=0,tick=0,witness=0;
    bool eligible=false,worldCoherent=false,measuredMotion=false;
    // Motion positions/normal share one frame: world for world contacts,
    // model-relative for self proxies. point is ALWAYS world-space for reporting.
    m::Vec3 requested{},allowed{},normal{},point{};
    Motion motion{};double gap=0;
    int feature=-1; // stable point identity; switching fingertips is not motion
    unsigned flags=0; // owner/finger-mask details are diagnostic, not permissions
};
struct Decision {Phase phase=Phase::None;bool changed=false;float amplitude=0;int durationMs=0;std::uint64_t episode=0;Motion motion{};};
struct State {
    std::uint64_t actor=0,key=0,generation=0,epoch=0,request=0,sequence=0,tick=0,episode=0;
    std::uint64_t lastPulse=0,clearSince=0,lastMoving=0;
    Source source=Source::WeaponWorld;int side=0,feature=-1;
    bool initialized=false,contact=false,suspended=false,motionReady=false;
    Phase phase=Phase::None;m::Vec3 requested{},allowed{},normal{};
    double slipSincePulse=0;
};
inline bool same(const State& s,const Sample& x) noexcept {
    return s.initialized&&s.actor==x.actor&&s.key==x.key&&s.generation==x.generation&&
        s.epoch==x.epoch&&s.request==x.request&&s.side==x.side&&s.source==x.source;
}
inline Decision update(State& s,const Sample& x) noexcept {
    Decision d{};
    const bool identity=x.actor&&x.key&&x.epoch&&x.request&&x.sequence&&x.tick&&x.side>=0&&x.side<2;
    if(!identity||!x.eligible){
        if(s.contact||s.suspended){d.phase=Phase::End;d.changed=true;d.episode=s.episode;}
        const auto episode=s.episode;s={};s.episode=episode;return d;
    }
    if(!same(s,x)){
        const auto episode=s.episode;s={};s.episode=episode;s.initialized=true;
        s.actor=x.actor;s.key=x.key;s.generation=x.generation;s.epoch=x.epoch;s.request=x.request;s.side=x.side;s.source=x.source;
    }
    d.episode=s.episode;
    if(x.sequence==s.sequence)return d; // two eyes/republication must not buzz twice
    const bool continuity=s.sequence&&x.sequence>s.sequence&&s.tick&&x.tick>s.tick&&x.tick-s.tick<=80;
    const bool finite=m::finite(x.requested)&&m::finite(x.allowed)&&m::finite(x.point)&&std::isfinite(x.gap);
    const auto unit=m::unit(x.normal);
    const bool plane=x.status!=Status::Contact||bool(unit);
    const bool known=x.worldCoherent&&finite&&plane&&x.status!=Status::Unknown&&x.status!=Status::Unavailable;
    Motion motion{};
    if(continuity&&known){
        if(x.measuredMotion)motion=x.motion;
        else if(s.motionReady&&s.feature==x.feature){
            const double dt=double(x.tick-s.tick)*.001;
            const auto da=x.allowed-s.allowed,dr=x.requested-s.requested;
            // The world query owns geometry. This only describes a committed
            // displacement; a pressure error alone is never a slide distance.
            if(unit){motion.incoming=std::max(0.0,-m::dot(dr,*unit)/dt);
                motion.slipDistance=m::length(da-*unit*m::dot(da,*unit));
                motion.slipSpeed=motion.slipDistance/dt;motion.valid=true;}
        }
    }
    if(!std::isfinite(motion.incoming)||!std::isfinite(motion.slipSpeed)||!std::isfinite(motion.slipDistance)||
       motion.incoming<0||motion.slipSpeed<0||motion.slipDistance<0||motion.incoming>20||motion.slipSpeed>20||motion.slipDistance>.25)motion={};
    // Discontinuity is not an impact. On a repeated/reversed timestamp, leave
    // motion unqualified; restarting at contact must not manufacture a strike.
    s.sequence=x.sequence;s.tick=x.tick;s.requested=x.requested;s.allowed=x.allowed;s.normal=x.normal;
    s.motionReady=known;s.feature=x.feature;
    d.motion=motion;
    if(!known){
        if(s.contact&&!s.suspended){d.phase=Phase::Suspended;d.changed=true;s.phase=d.phase;}
        s.suspended=s.contact;s.slipSincePulse=0;s.clearSince=0;return d;
    }
    if(x.status==Status::Clear){
        if(s.contact||s.suspended){
            if(!s.clearSince)s.clearSince=x.tick;
            if(x.tick>=s.clearSince&&x.tick-s.clearSince>=30){
                d.phase=Phase::End;d.changed=true;s.contact=s.suspended=false;s.phase=d.phase;s.slipSincePulse=0;
            }
        }
        return d;
    }
    s.clearSince=0;
    const bool resumed=s.suspended;s.suspended=false;
    const bool begin=!s.contact;s.contact=true;
    if(begin){++s.episode;d.episode=s.episode;s.slipSincePulse=0;d.phase=Phase::Begin;}
    else if(resumed){d.phase=Phase::Resumed;s.slipSincePulse=0;motion={};d.motion={};}
    else if(motion.valid&&motion.incoming>.25)d.phase=Phase::Impact;
    else if(motion.valid&&motion.slipSpeed>=.04&&motion.slipDistance>0){d.phase=Phase::Slide;s.lastMoving=x.tick;}
    else if(s.phase==Phase::Slide&&x.tick>=s.lastMoving&&x.tick-s.lastMoving<100)d.phase=Phase::Slide;
    else d.phase=Phase::Rest;
    d.changed=begin||resumed||d.phase!=s.phase;s.phase=d.phase;
    // Existing weapon impact feedback remains authoritative for impacts. New
    // feedback is a soft contact entry / bounded friction texture, not a hit.
    const bool pulseDue=!s.lastPulse||(x.tick>=s.lastPulse&&x.tick-s.lastPulse>=100);
    if(motion.valid&&!resumed){
        if(begin&&pulseDue&&(motion.incoming>.025||motion.slipSpeed>.04)){
            d.amplitude=x.source==Source::WeaponWorld?.14f:.10f;d.durationMs=18;
        }
        if(d.phase==Phase::Slide&&x.source==Source::WeaponWorld){
            s.slipSincePulse=std::min(.1,s.slipSincePulse+motion.slipDistance);
            if(pulseDue&&motion.slipSpeed>=.04&&s.slipSincePulse>=.008){
                d.amplitude=float(std::clamp(.06+motion.slipSpeed*.04,.07,.18));d.durationMs=20;s.slipSincePulse=0;
            }
        }
    }
    if(d.amplitude>0)s.lastPulse=x.tick;
    if(d.phase!=Phase::Slide)s.slipSincePulse=0;
    return d;
}
} // namespace kcd2vr::passive169r1
