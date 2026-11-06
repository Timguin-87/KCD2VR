#pragma once
#include "pose_math.h"
#include "../core/snapshot_mailbox.h"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::station_reposition_d53 {
namespace m=pose_core;
// Virtual roomscale-style repositioning of one completed station presentation.
// No native player/capsule transform, workstation action, or recipe is written.
inline constexpr double kRadiusM=.40,kSpeedMps=.30,kDeadZone=.22;
struct Owner {
    std::uintptr_t character=0,instance=0;
    std::uint64_t pose=0,tracking=0,control=0,mode=0,stationLifetime=0;
    unsigned kind=0;
};
inline bool valid(const Owner& o) noexcept {
    return o.character&&o.instance&&o.pose&&o.tracking&&o.control&&o.mode&&o.stationLifetime&&(o.kind==1||o.kind==2);
}
inline bool same(const Owner& a,const Owner& b) noexcept {
    return valid(a)&&valid(b)&&a.character==b.character&&a.instance==b.instance&&a.pose==b.pose&&
        a.tracking==b.tracking&&a.control==b.control&&a.mode==b.mode&&a.stationLifetime==b.stationLifetime&&a.kind==b.kind;
}
struct Command {
    std::uintptr_t instance=0;
    std::uint64_t pose=0,tracking=0,mode=0,stationLifetime=0,sample=0,tick=0,timeNs=0;
    unsigned kind=0;
    float x=0,y=0;
    bool enabled=false;
};
inline core::SnapshotMailbox<Command> command;
struct Input {
    Owner owner{};Command command{};
    std::uint64_t sample=0,tick=0,now=0;
    m::Quat trackingBasis{};
    m::Vec3 forward{};
    double scale=1;
    bool active=false;
};
struct State {
    Owner owner{};
    m::Quat trackingBasis{};
    m::Vec3 offset{};
    double scale=1;
    std::uint64_t sample=0,tick=0,inputSample=0,inputTick=0,inputTimeNs=0;
};
struct Output {m::Vec3 offset{};bool active=false,moved=false;};
inline bool current(const Command& c,const Input& i) noexcept {
    return c.enabled&&c.instance==i.owner.instance&&c.pose==i.owner.pose&&c.tracking==i.owner.tracking&&
        c.mode==i.owner.mode&&c.stationLifetime==i.owner.stationLifetime&&c.kind==i.owner.kind&&
        // D74: this is current joystick intent, not a pose to splice into the
        // skeleton. Input can publish after the body transaction took its XR
        // sample. Ordering these independent streams permanently discarded
        // movement when the input publisher ran ahead of the pose consumer.
        // Exact owner/epoch, wall-clock age and per-command monotonicity remain.
        c.sample&&c.tick&&i.now>=c.tick&&i.now-c.tick<=80&&c.timeNs&&
        std::isfinite(c.x)&&std::isfinite(c.y)&&std::fabs(c.x)<=1.001f&&std::fabs(c.y)<=1.001f;
}
inline Output advance(State& s,const Input& i) noexcept {
    const auto basis=m::unit(i.trackingBasis);
    if(!i.active||!valid(i.owner)||!i.sample||!i.tick||i.now<i.tick||i.now-i.tick>80||
       !basis||!std::isfinite(i.scale)||i.scale<.1||i.scale>10||!m::finite(i.forward)){
        s={};return {};
    }
    const bool owner=same(s.owner,i.owner)&&std::fabs(s.scale-i.scale)<1.e-6&&
        m::angle(s.trackingBasis,*basis)<1.e-6;
    if(!owner||i.sample<s.sample||i.tick<s.tick){
        s={};s.owner=i.owner;s.trackingBasis=*basis;s.scale=i.scale;
    }
    auto f=i.forward;f.z=0;const double length=m::length(f);
    if(!std::isfinite(length)||length<.10){
        s.inputSample=s.inputTick=s.inputTimeNs=0;s.sample=i.sample;s.tick=i.tick;return {s.offset,true,false};
    }
    f=f*(1./length);
    // The offset survives a stationary input or a short input outage within
    // the same station visit; stale input never earns continued movement.
    // Nested recipe-reading changes the global activity lifetime, but not the
    // exact station's activity epoch used above.
    if(!current(i.command,i)){
        s.inputSample=s.inputTick=s.inputTimeNs=0;s.sample=i.sample;s.tick=i.tick;
        return {s.offset,true,false};
    }
    const auto& c=i.command;
    if(s.inputSample&&(c.sample<s.inputSample||c.tick<s.inputTick||c.timeNs<s.inputTimeNs)){
        s.sample=i.sample;s.tick=i.tick;return {s.offset,true,false};
    }
    const bool duplicate=i.sample==s.sample||c.sample==s.inputSample;
    const bool continuous=s.inputSample&&c.sample>s.inputSample&&c.tick>=s.inputTick&&c.tick-s.inputTick<=80&&
        c.timeNs>=s.inputTimeNs&&c.timeNs-s.inputTimeNs<=80000000;
    bool moved=false;
    if(!duplicate&&continuous){
        const double dt=std::min(.05,double(c.timeNs-s.inputTimeNs)*1.e-9);
        const double mag=std::hypot(double(c.x),double(c.y));
        if(mag>kDeadZone&&dt>0){
            const double speed=(std::min(mag,1.)-kDeadZone)/(1.-kDeadZone)*kSpeedMps;
            const m::Vec3 right{f.y,-f.x,0};
            const auto delta=(right*(double(c.x)/mag)+f*(double(c.y)/mag))*(speed*dt/i.scale);
            auto next=s.offset+delta;const double radius=m::length(next),limit=kRadiusM/i.scale;
            if(radius>limit)next=next*(limit/radius);
            moved=m::length(next-s.offset)>1.e-9;s.offset=next;
        }
    }
    if(!duplicate||!s.inputSample){s.inputSample=c.sample;s.inputTick=c.tick;s.inputTimeNs=c.timeNs;}
    s.sample=i.sample;s.tick=i.tick;
    return {s.offset,true,moved};
}
// Mode3 alchemy reserves the left stick for repositioning. Right-stick native
// page selection, thumbrest+D-pad pages and physical page gestures stay live.
inline float pageStick(bool mode3Alchemy,float left,float right) noexcept {
    return mode3Alchemy?right:(std::fabs(right)>=std::fabs(left)?right:left);
}
// Applied after the unshifted frame is built. The cached native foundation is
// deliberately untouched, so a repeated native callback cannot add it twice.
template<class Frame> inline void apply(Frame& f,const Output& out) noexcept {
    f.stationOffsetD53=out.active?out.offset:m::Vec3{};
    f.anchor=f.anchor+f.stationOffsetD53;f.bodyOffset=f.bodyOffset+f.stationOffsetD53;
}
}
