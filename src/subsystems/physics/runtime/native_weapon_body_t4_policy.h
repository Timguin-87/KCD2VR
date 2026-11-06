#pragma once
#include "native_weapon_body_t4.h"
#include <algorithm>
#include <cmath>

namespace kcd2vr::native_weapon_body_t4::policy {
namespace m=pose_core;
inline bool finite(Pose p) noexcept {
    const double n=p.q.x*p.q.x+p.q.y*p.q.y+p.q.z*p.q.z+p.q.w*p.q.w;
    return m::finite(p.p)&&std::isfinite(n)&&std::abs(n-1)<.01&&m::length(p.p)<1000000;
}
inline Pose compose(Pose a,Pose b) noexcept {return {m::multiply(a.q,b.q),a.p+m::rotate(a.q,b.p)};}
inline Pose inverse(Pose a) noexcept {const auto q=m::conjugate(a.q);return {q,m::rotate(q,a.p*-1)};}
inline Vec3 limit(Vec3 a,double bound) noexcept {const double n=m::length(a);return n>bound&&n>0?a*(bound/n):a;}
inline bool owner(const Owner& o) noexcept {
    return o.key&&o.generation&&o.request&&o.epoch&&o.player&&o.manager&&o.character&&
        o.entityPointer&&o.entityId&&o.attachment&&o.attachmentObject&&o.side==0;
}
inline Reason eligible(const Input& in,std::uint64_t now) noexcept {
    if(!in.allowed||!in.tracked||!in.emptyLeft||
       !(in.classId==1||in.classId==2||in.classId==4||in.classId==16))return Reason::Unsupported;
    if(!owner(in.owner)||!in.sample||!in.tick||!in.trackingEpoch||!finite(in.weaponWorld)||!finite(in.targetWorld)||
       !finite(in.gripLocal)||!m::finite(in.boundsMin)||!m::finite(in.boundsMax)||
       !std::isfinite(in.scale)||in.scale<.1||in.scale>10||!std::isfinite(in.mass)||in.mass<.2||in.mass>12)return Reason::InvalidInput;
    if(now<in.tick||now-in.tick>80)return Reason::StaleInput;
    if(!in.nativeColliderExcluded||!in.colliderLease)return Reason::NativeCollider;
    return Reason::None;
}
struct Shape {
    Pose bodyLocal{};Vec3 axis{0,1,0};double radius=0,halfHeight=0,mass=0,inertiaUpper=0;bool valid=false;
};
// Circumscribe the WHOLE native bounds, including hilt, in one capsule. Its
// cylindrical segment spans the longest box dimension and the radius covers
// both other dimensions. This is conservative geometry, not a cloned mesh.
inline Shape shape(const Input& in) noexcept {
    Shape out{};const Vec3 d=(in.boundsMax-in.boundsMin)*in.scale;
    if(!m::finite(d)||d.x<=.002||d.y<=.002||d.z<=.002)return out;
    const Vec3 half=d*.5;const double h[3]{half.x,half.y,half.z};
    unsigned major=h[1]>h[0]?1:0;if(h[2]>h[major])major=2;
    const unsigned a=(major+1)%3,b=(major+2)%3;
    out.radius=std::sqrt(h[a]*h[a]+h[b]*h[b])+.003;
    out.halfHeight=h[major];out.mass=in.mass;
    out.axis={major==0?1.:0,major==1?1.:0,major==2?1.:0};
    out.bodyLocal.p=(in.boundsMin+in.boundsMax)*(.5*in.scale);
    // A deliberately conservative upper bound is used only to cap commanded
    // angular acceleration; the engine owns the actual mass/inertia integration.
    out.inertiaUpper=in.mass*(out.halfHeight*out.halfHeight+out.radius*out.radius);
    out.valid=out.radius>=.005&&out.radius<=.25&&out.halfHeight>=.05&&out.halfHeight<=1.25&&
        out.inertiaUpper>.0001&&std::isfinite(out.inertiaUpper);
    return out;
}
inline Pose bodyTarget(const Input& in,const Shape& s) noexcept {return compose(in.targetWorld,s.bodyLocal);}
inline Pose weaponPose(Pose body,const Shape& s) noexcept {return compose(body,inverse(s.bodyLocal));}
struct Motion {Vec3 linear{},angular{};bool valid=false;Reason reason=Reason::MotionUnknown;};
inline Motion motor(const Input& in,const Shape& s,Pose achieved,Vec3 v,Vec3 w,double actualMass,double dt) noexcept {
    Motion out{};
    if(!s.valid||!finite(achieved)||!m::finite(v)||!m::finite(w)||!std::isfinite(actualMass)||
       actualMass<.2||actualMass>12||!std::isfinite(dt)||dt<=0||dt>.05)return out;
    const Pose target=bodyTarget(in,s);const Vec3 error=target.p-achieved.p;
    if(!finite(target)||m::length(error)>.65||m::length(v)>12||m::length(w)>45){out.reason=Reason::ExcessError;return out;}
    // Velocity correction only. No mod-side position/mass integrator, no pose
    // teleports, and no resetting the accumulated collision response each frame.
    const Vec3 wanted=limit(error*(1/.055),3.0);
    const double maxDv=std::min(30.,180./actualMass)*dt;
    out.linear=v+limit(wanted-v,maxDv);
    auto delta=m::multiply(target.q,m::conjugate(achieved.q));
    if(delta.w<0){delta.x=-delta.x;delta.y=-delta.y;delta.z=-delta.z;delta.w=-delta.w;}
    const Vec3 qv{delta.x,delta.y,delta.z};const double qn=m::length(qv);
    const Vec3 wantedW=qn>1e-8?limit(qv*(2*std::atan2(qn,std::clamp(delta.w,-1.,1.))/(qn*.065)),12.):Vec3{};
    const double maxDw=std::min(70.,18./s.inertiaUpper)*dt;
    out.angular=w+limit(wantedW-w,maxDw);
    out.valid=m::finite(out.linear)&&m::finite(out.angular);out.reason=out.valid?Reason::None:Reason::MotionUnknown;
    return out;
}
}
