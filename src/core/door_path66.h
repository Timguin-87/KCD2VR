#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

// Pure geometry only. The owner must prove the source and construct a RIGID
// door-leaf handle trajectory from the resident authored clip. An animated
// slt_lock reach/withdraw curve is not such a trajectory. Nothing here selects
// an entity, accepts a native Use, reads a model, or changes animation time.
namespace kcd2vr::door_path66 {
struct Point { double x=0,y=0,z=0; };
struct Rotation { double x=0,y=0,z=0,w=1; };
struct Pose { Point p{};Rotation q{}; };
inline Point add(Point a,Point b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline Point sub(Point a,Point b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline Point mul(Point a,double k){return {a.x*k,a.y*k,a.z*k};}
inline double dot(Point a,Point b){return a.x*b.x+a.y*b.y+a.z*b.z;}
inline bool finite(Point p){return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);}
inline bool rotation(Rotation q){
    const double n=q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;
    return std::isfinite(n)&&std::abs(n-1)<1e-4;
}
inline Rotation conjugate(Rotation q){return {-q.x,-q.y,-q.z,q.w};}
inline Point rotate(Rotation q,Point p){
    const Point u{q.x,q.y,q.z};
    const Point c{u.y*p.z-u.z*p.y,u.z*p.x-u.x*p.z,u.x*p.y-u.y*p.x};
    const Point cc{u.y*c.z-u.z*c.y,u.z*c.x-u.x*c.z,u.x*c.y-u.y*c.x};
    return add(p,add(mul(c,2*q.w),mul(cc,2)));
}
inline bool valid(Pose p){return finite(p.p)&&rotation(p.q);}
inline Point local(Pose world,Point p){return rotate(conjugate(world.q),sub(p,world.p));}
inline Point world(Pose frame,Point p){return add(frame.p,rotate(frame.q,p));}
inline Rotation blend(Rotation a,Rotation b,double t){
    if(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w<0)b={-b.x,-b.y,-b.z,-b.w};
    Rotation q{a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t,a.w+(b.w-a.w)*t};
    const double n=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
    return {q.x/n,q.y/n,q.z/n,q.w/n};
}
struct Source {
    std::uint64_t operation=0,clip=0,topology=0,curve=0;
    std::uint64_t press=0,poseEpoch=0,trackingEpoch=0,generation=0,settingsEpoch=0,experimentEpoch=0;
    std::uintptr_t player=0,entity=0;
    std::uint32_t target=0;int side=-1;
};
inline bool valid(const Source& s){
    return s.operation&&s.clip&&s.topology&&s.curve&&s.press&&s.poseEpoch&&s.trackingEpoch&&
        s.generation&&s.settingsEpoch&&s.player&&s.entity&&s.target&&s.target<=16777215u&&s.side>=0&&s.side<2;
}
inline bool same(const Source& a,const Source& b){
    return a.operation==b.operation&&a.clip==b.clip&&a.topology==b.topology&&a.curve==b.curve&&
        a.press==b.press&&a.poseEpoch==b.poseEpoch&&a.trackingEpoch==b.trackingEpoch&&a.generation==b.generation&&
        a.settingsEpoch==b.settingsEpoch&&a.experimentEpoch==b.experimentEpoch&&a.player==b.player&&
        a.entity==b.entity&&a.target==b.target&&a.side==b.side;
}
struct Sample {double phase=0;Pose handle{};};
struct Curve {Source source{};std::array<Sample,33> samples{};};
using Path=Curve;
inline bool valid(const Curve& c){
    if(!valid(c.source)||c.samples.front().phase!=0||c.samples.back().phase!=1)return false;
    double travel=0;
    for(std::size_t i=0;i<c.samples.size();++i){
        const auto& s=c.samples[i];
        if(!std::isfinite(s.phase)||!valid(s.handle)||(i&&s.phase<=c.samples[i-1].phase))return false;
        if(i){const auto d=sub(s.handle.p,c.samples[i-1].handle.p);travel+=dot(d,d);}
    }
    return std::isfinite(travel)&&travel>1e-12;
}
inline Pose at(const Curve& c,double phase){
    phase=std::clamp(phase,0.0,1.0);
    for(std::size_t i=1;i<c.samples.size();++i)if(phase<=c.samples[i].phase){
        const auto& a=c.samples[i-1];const auto& b=c.samples[i];const double t=(phase-a.phase)/(b.phase-a.phase);
        return {add(a.handle.p,mul(sub(b.handle.p,a.handle.p),t)),blend(a.handle.q,b.handle.q,t)};
    }
    return c.samples.back().handle;
}
struct Input {Source source{};Pose modelWorld{};Point handWorld{};std::uint64_t tick=0,poseTick=0;};
inline bool valid(const Input& in,std::uint64_t now){
    return valid(in.source)&&valid(in.modelWorld)&&finite(in.handWorld)&&in.tick&&in.poseTick&&
        now>=in.tick&&now-in.tick<=80&&now>=in.poseTick&&now-in.poseTick<=80;
}
struct Limits {
    // Explicit owner policy, not an inferred axis or animation length.
    double maxPhaseStep=1,maxDistance=.20,stopDistance=.001;
};
inline bool valid(Limits l){return std::isfinite(l.maxPhaseStep)&&l.maxPhaseStep>0&&l.maxPhaseStep<=1&&
    std::isfinite(l.maxDistance)&&l.maxDistance>0&&l.maxDistance<=.20&&
    std::isfinite(l.stopDistance)&&l.stopDistance>=0&&l.stopDistance<l.maxDistance;}
struct Result {double phase=0,rate=0,error=0;Pose handleWorld{};bool stopped=true;};
using Projection=Result;
struct State {Source source{};Point offset{},lastHand{};std::uint64_t tick=0;double phase=0;bool active=false;};
inline bool bind(State& s,const Curve& c,const Input& in,std::uint64_t now,double phase){
    s={};
    if(!valid(c)||!valid(in,now)||!same(c.source,in.source)||!std::isfinite(phase)||phase<0||phase>1)return false;
    const auto hand=local(in.modelWorld,in.handWorld);const auto handle=at(c,phase);
    const auto offset=rotate(conjugate(handle.q),sub(hand,handle.p));
    // The native eligible same-door surface/contact proof is still required by
    // the caller; this bound only refuses a geometrically detached binding.
    if(dot(offset,offset)>.20*.20)return false;
    s={in.source,offset,hand,now,phase,true};return true;
}
inline bool project(State& s,const Curve& c,const Input& in,std::uint64_t now,Limits limits,Result& out){
    out={};
    if(!s.active||!valid(c)||!valid(in,now)||!valid(limits)||!same(s.source,in.source)||
       !same(c.source,in.source)||now<=s.tick||now-s.tick>80){s={};return false;}
    const auto hand=local(in.modelWorld,in.handWorld);const auto movement=sub(hand,s.lastHand);
    double phase=s.phase,best=1e300;
    const double lo=std::max(0.0,s.phase-limits.maxPhaseStep),hi=std::min(1.0,s.phase+limits.maxPhaseStep);
    // Offset rotates with the retained handle frame, not with the camera or an
    // entity centre. Each segment is evaluated once; duplicate points are legal.
    for(std::size_t i=1;i<c.samples.size();++i){
        const auto& a=c.samples[i-1];const auto& b=c.samples[i];
        const double left=std::max(lo,a.phase),right=std::min(hi,b.phase);if(left>right)continue;
        const auto pa=add(a.handle.p,rotate(a.handle.q,s.offset));
        const auto pb=add(b.handle.p,rotate(b.handle.q,s.offset));const auto d=sub(pb,pa);const double dd=dot(d,d);
        const double tlo=(left-a.phase)/(b.phase-a.phase),thi=(right-a.phase)/(b.phase-a.phase);
        const double prior=std::clamp((s.phase-a.phase)/(b.phase-a.phase),tlo,thi);
        const double t=dd>1e-18?std::clamp(dot(sub(hand,pa),d)/dd,tlo,thi):prior;
        const double p=a.phase+(b.phase-a.phase)*t;const auto e=sub(hand,add(pa,mul(d,t)));const double err=dot(e,e);
        if(err<best-1e-12||(std::abs(err-best)<=1e-12&&std::abs(p-s.phase)<std::abs(phase-s.phase))){best=err;phase=p;}
    }
    if(!std::isfinite(best)||best>limits.maxDistance*limits.maxDistance){s={};return false;}
    // Sub-threshold noise does not autonomously progress the native clip. Keep
    // the last committed point so genuine slow motion can accumulate naturally.
    if(dot(movement,movement)<=limits.stopDistance*limits.stopDistance)phase=s.phase;
    else s.lastHand=hand;
    const double rate=(phase-s.phase)/(double(now-s.tick)*.001);s.phase=phase;s.tick=now;
    const auto handle=at(c,phase);
    // World orientation composition; both inputs already have validated unit q.
    const auto a=in.modelWorld.q,b=handle.q;
    const Rotation q{a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
        a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};
    out={phase,rate,std::sqrt(best),{world(in.modelWorld,handle.p),q},std::abs(rate)<1e-12};return true;
}
}
