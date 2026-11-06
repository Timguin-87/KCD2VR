#pragma once
// R38 visible-body posture only. The input is the calibrated model-local HMD
// yaw delta; native/entity world turn is composed by the existing frame owner.
// Nothing here transforms an HMD/controller endpoint or moves the native actor.
// The caller stages State and publishes it only with the completed pose packet.
#include "pose_math.h"
#include "motion_time.h"

namespace kcd2vr::body_follow_r38 {
namespace m=pose_core;
constexpr double pi=3.14159265358979323846,rad=pi/180.;
inline double wrapped(double a) noexcept {return std::remainder(a,2*pi);}
inline m::Quat yaw(double a) noexcept {a=wrapped(a);return {0,0,std::sin(a*.5),std::cos(a*.5)};}
struct Owner {
    std::uintptr_t character=0,station=0;
    std::uint64_t pose=0,tracking=0,control=0,mode=0,lifetime=0;
    unsigned kind=0;
};
inline bool valid(const Owner& o) noexcept {
    return o.character&&o.pose&&o.tracking&&o.control&&
        (!o.kind||(o.station&&o.mode&&o.lifetime&&(o.kind==1||o.kind==2)));
}
inline bool same(const Owner& a,const Owner& b) noexcept {
    return valid(a)&&valid(b)&&a.character==b.character&&a.pose==b.pose&&a.tracking==b.tracking&&
        a.control==b.control&&a.station==b.station&&a.mode==b.mode&&a.lifetime==b.lifetime&&a.kind==b.kind;
}
struct Foot {
    m::Vec3 planted{},from{},current{};
    double plantedYaw=0,fromYaw=0,currentYaw=0;
    std::uint64_t sample=0,tick=0,start=0;
    bool have=false,stepping=false;
};
struct State {
    Owner owner{};
    m::Quat trackingBasis{};
    std::uint64_t sample=0,tick=0,timeNs=0;
    double previousWrappedHead=0,head=0,body=0;
    bool ready=false;
    Foot foot[2]{};
};
struct Input {
    Owner owner{};
    m::Quat trackingBasis{};
    std::uint64_t sample=0,tick=0,timeNs=0,now=0;
    double headYaw=0;
    bool active=false;
};
struct Heading {double body=0,head=0;bool active=false,continuous=false,newSample=false;};
inline Heading advance(State& s,const Input& in) noexcept {
    const auto basis=m::unit(in.trackingBasis);
    if(!in.active||!valid(in.owner)||!basis||!in.sample||!in.tick||in.now<in.tick||in.now-in.tick>80||!std::isfinite(in.headYaw)){
        s=State{};return {};
    }
    const bool owner=s.ready&&same(s.owner,in.owner)&&m::angle(s.trackingBasis,*basis)<1e-5;
    if(owner&&(in.sample<s.sample||in.tick<s.tick||(in.timeNs&&s.timeNs&&in.timeNs<s.timeNs))){s=State{};return {};}
    // Repeated native consumers of one XR sample may only read the posture.
    if(owner&&in.sample==s.sample)return {s.body,s.head,true,true,false};
    const double dt=owner?motion_time::elapsed(s.tick,in.tick,s.timeNs,in.timeNs):0;
    const bool continuous=owner&&dt>=0&&dt<=.080000001;
    constexpr double freeTurn=25*rad;
    const double raw=wrapped(in.headYaw);
    if(!continuous){
        s=State{};s.ready=true;s.owner=in.owner;s.trackingBasis=*basis;s.head=raw;
        // A new owner has no usable old heading. Start at this HMD's posture,
        // with a bounded comfortable neck/chest turn, never an old world's yaw.
        s.body=raw-freeTurn*std::tanh(raw/freeTurn);
    }else{
        s.head+=wrapped(raw-s.previousWrappedHead);
        // Smoothly let small head turns stay in the upper body; a sustained or
        // larger turn recruits hips. Only the inferred body is rate limited.
        const double relative=s.head-s.body;
        const double wanted=s.head-freeTurn*std::tanh(relative/freeTurn);
        const double step=(wanted-s.body)*(-std::expm1(-dt/.12));
        const double limit=360*rad*dt;
        s.body+=std::clamp(step,-limit,limit);
    }
    s.previousWrappedHead=raw;s.sample=in.sample;s.tick=in.tick;s.timeNs=in.timeNs;
    return {s.body,s.head,true,continuous,true};
}
// Rotate the current native footprint about the current native pelvis. The
// returned offset has no vertical component: native ground height is retained.
inline m::Vec3 foot_offset(m::Vec3 ankle,m::Vec3 pelvis,m::Vec3 displacement,double bodyYaw) noexcept {
    auto radius=ankle-pelvis;radius.z=0;displacement.z=0;
    return displacement+m::rotate(yaw(bodyYaw),radius)-radius;
}
inline Foot step_foot(Foot& s,m::Vec3 desired,double desiredYaw,std::uint64_t sample,std::uint64_t tick,
        bool continuous,bool otherStepping,double distanceTrigger=.16) noexcept {
    if(!m::finite(desired)||!std::isfinite(desiredYaw)||!sample||!tick){s=Foot{};return s;}
    if(!continuous||!s.have||sample<s.sample||tick<s.tick){
        s=Foot{};s.have=true;s.planted=s.current=desired;s.plantedYaw=s.currentYaw=desiredYaw;
    }else if(sample==s.sample)return s;
    else if(s.stepping){
        const double t=double(tick-s.start)/180.;
        if(t>=1.){s.planted=s.current=desired;s.plantedYaw=s.currentYaw=desiredYaw;s.stepping=false;}
        else{
            const double e=t*t*(3-2*t);
            s.current=s.from+(desired-s.from)*e;s.current.z+=.05*std::sin(pi*t);
            s.currentYaw=s.fromYaw+wrapped(desiredYaw-s.fromYaw)*e;
        }
    }else if(!otherStepping&&(m::length(desired-s.planted)>std::clamp(distanceTrigger,.05,.5)||
                            std::abs(wrapped(desiredYaw-s.plantedYaw))>25*rad)){
        s.stepping=true;s.from=s.planted;s.fromYaw=s.plantedYaw;s.start=tick;
    }
    s.sample=sample;s.tick=tick;return s;
}
} // namespace kcd2vr::body_follow_r38
