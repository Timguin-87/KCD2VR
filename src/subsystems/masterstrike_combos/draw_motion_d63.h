#pragma once
#define KCD2VR_DRAW_MOTION_D63 1
// Input admission only. The actual Draw transports its original controller
// source. Pending raw sources may prove extraction/settle before geometry ACK,
// but only a later positively held same-item source may hand over to combat.
#include "vrik/pose_math.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace kcd2vr::draw_motion_d63 {
namespace m=kcd2vr::pose_core;
struct Source {
    std::uint64_t sample=0,tick=0,timeNs=0,tracking=0,body=0,request=0,frameActor=0,frameControl=0;
    m::Vec3 hand{},modelHand{},extractionDirection{},modelExtractionDirection{};m::Quat rotation{},modelRotation{},calibration{};
    double scale=1;int side=-1;bool valid=false,anchored=false,extracted=false;
};
struct Ticket {std::uint64_t serial=0,key=0,generation=0,body=0,request=0,sample=0,tick=0;int side=-1;Source source{};};
struct Owner {std::uint64_t key=0,generation=0,body=0,request=0,sample=0,tick=0;int side=-1;};
struct State {
    Ticket ticket{};Source last{};bool initialized=false,armed=false,moved=false,qualified=false,handoffPending=false,acknowledged=false;
    m::Vec3 direction{},modelDirection{},back{},modelBack{};double quiet=0,backTime=0;
};
struct Sample {Source source{};bool tracked=false,held=false;};
struct Decision {bool blocked=false,released=false;};
inline bool same(const Ticket& t,const Owner& o) noexcept {
    return t.serial&&t.key&&t.generation&&t.body&&t.request&&t.sample&&t.key==o.key&&
        t.generation==o.generation&&t.body==o.body&&t.request==o.request&&t.side==o.side;
}
inline bool valid(const Source& s) noexcept {
    return s.valid&&s.sample&&s.tick&&s.body&&s.request&&s.side>=0&&s.side<2&&
        m::finite(s.hand)&&m::finite(s.modelHand)&&m::unit(s.rotation)&&m::unit(s.modelRotation)&&m::unit(s.calibration)&&
        std::isfinite(s.scale)&&s.scale>=.1&&s.scale<=10&&(!s.anchored||(s.frameActor&&s.frameControl));
}
inline bool sameBasis(const Source& a,const Source& b) noexcept {
    return valid(a)&&valid(b)&&a.body==b.body&&a.request==b.request&&a.side==b.side&&a.tracking==b.tracking&&
        a.anchored==b.anchored&&a.frameActor==b.frameActor&&a.frameControl==b.frameControl&&
        std::abs(a.scale-b.scale)<1e-6&&m::angle(a.calibration,b.calibration)<1e-7;
}
// The legacy holster emits Draw after a physical pull. Retain both RAW
// controller deltas from its own latch, not hand-minus-moving-mount geometry.
// Missing or changed-basis evidence falls back to ordinary extraction handling.
inline Source extractedSource(Source current,const Source& origin) noexcept {
    current.extracted=false;current.extractionDirection=current.modelExtractionDirection={};
    if(!sameBasis(origin,current)||current.sample<=origin.sample||current.tick<origin.tick||
       current.tick-origin.tick>2000)return current;
    const auto hand=current.hand-origin.hand,model=current.modelHand-origin.modelHand;
    if(std::min(m::length(hand),m::length(model))<.06)return current;
    const auto h=m::unit(hand),mh=m::unit(model);if(!h||!mh)return current;
    current.extractionDirection=*h;current.modelExtractionDirection=*mh;current.extracted=true;return current;
}
inline bool sourceFor(const Ticket& t) noexcept {
    const auto& s=t.source;return valid(s)&&s.body==t.body&&s.request==t.request&&s.side==t.side&&
        s.sample<=t.sample&&s.tick<=t.tick&&t.tick-s.tick<=3000;
}
inline Decision update(State& s,const Ticket& offered,const Owner& owner,const Sample& pose) noexcept {
    const auto& current=pose.source;
    if(s.ticket.serial&&!same(s.ticket,owner))s={};
    if(same(offered,owner)&&sourceFor(offered)&&offered.serial>s.ticket.serial&&owner.sample>offered.sample&&
       owner.tick>=offered.tick&&owner.tick-offered.tick<=2000&&sameBasis(offered.source,current)){
        s={};s.ticket=offered;
        // extracted=true by itself carries no direction proof. Never let an
        // invalid/absent direction turn continued extraction into a fresh cut.
        const auto h=m::unit(offered.source.extractionDirection),mh=m::unit(offered.source.modelExtractionDirection);
        if(offered.source.extracted&&h&&mh){s.moved=true;s.direction=*h;s.modelDirection=*mh;}
    }
    if(!same(s.ticket,owner))return {};
    // An unacknowledged old operation cannot disarm a weapon drawn later.
    if(!s.acknowledged&&owner.tick>=s.ticket.tick&&owner.tick-s.ticket.tick>2000){s={};return {};}
    if(s.armed&&!s.handoffPending)return {};
    if(!sameBasis(s.ticket.source,current)){s={};return {};}
    if(s.armed)return {pose.held&&s.handoffPending,pose.held&&s.handoffPending};
    Decision out{pose.held,false};
    if(!pose.tracked||current.sample!=owner.sample||current.tick!=owner.tick){
        s.initialized=false;s.qualified=false;s.quiet=s.backTime=0;s.back=s.modelBack={};return out;
    }
    if(s.initialized&&current.sample<=s.last.sample)return out;
    if(pose.held)s.acknowledged=true;
    const auto previous=s.last;const bool hadPrevious=s.initialized;
    const double dt=hadPrevious?(current.timeNs&&previous.timeNs?
        (current.timeNs>=previous.timeNs?double(current.timeNs-previous.timeNs)*1e-9:-1):
        (current.tick>=previous.tick?double(current.tick-previous.tick)*.001:-1)):0;
    s.last=current;s.initialized=true;
    const auto travel=current.hand-s.ticket.source.hand,modelTravel=current.modelHand-s.ticket.source.modelHand;
    if(!s.moved&&std::min(m::length(travel),m::length(modelTravel))>=.06){
        s.moved=true;s.direction=m::unit(travel).value_or(m::Vec3{});s.modelDirection=m::unit(modelTravel).value_or(m::Vec3{});
    }
    // Qualification can be recorded while native geometry is pending, but a
    // same/older controller source can never acknowledge or arm that request.
    if(!hadPrevious||!std::isfinite(dt)||dt<=0||dt>.120000001){s.qualified=false;s.quiet=s.backTime=0;s.back=s.modelBack={};return out;}
    if(s.qualified&&pose.held){s.armed=s.handoffPending=true;out.released=true;return out;}
    if(!s.moved){s.quiet=s.backTime=0;return out;}
    const auto hand=current.hand-previous.hand,modelHand=current.modelHand-previous.modelHand;
    const bool quiet=std::max(m::length(hand),m::length(modelHand))/dt<.30&&
        std::max(m::angle(current.rotation,previous.rotation),m::angle(current.modelRotation,previous.modelRotation))/dt<3.;
    s.quiet=quiet?s.quiet+dt:0;
    const auto h=m::unit(hand),mh=m::unit(modelHand);
    if(h&&mh&&m::dot(*h,s.direction)<-.35&&m::dot(*mh,s.modelDirection)<-.35){
        s.back=s.back+hand;s.modelBack=s.modelBack+modelHand;s.backTime+=dt;
    }else{s.back=s.modelBack={};s.backTime=0;}
    if(s.quiet>=.05-1e-9||(s.backTime>=.035&&std::min(m::length(s.back),m::length(s.modelBack))>=.04)){
        s.qualified=true;if(pose.held){s.armed=s.handoffPending=true;out.released=true;}
    }
    // The actual ACK/handoff sample itself remains handling-only. A subsequent
    // fresh sample starts the ordinary recognizer from the hand's current pose.
    return out;
}
}
