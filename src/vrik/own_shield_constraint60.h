#pragma once
#include "self_contact54.h"

// C60: a local visual constraint, not a native collider/block/damage witness.
// Exact item bounds provide an oriented BOX approximation of the held shield.
// The supported sword blade is the existing hilt-excluding capsule. Both entire
// rigid poses move on their already world-checked paths; no new native query.
namespace kcd2vr::own_shield_constraint60 {
namespace m=pose_core;namespace sc=self_contact54;
struct Result {
    bool usable=false,limited=false,escaping=false;
    double fraction=1,gap=0;unsigned queries=0;m::Vec3 normal{};
};
template<class Profile> inline bool sword(const Profile& p) noexcept {
    return p.classId==1||p.classId==2||p.classId==4||p.classId==16;
}
template<class Profile,class Path> inline auto at(const Profile& p,const Path& path,double t) noexcept {
    const unsigned n=path.count-1;
    const double a=std::clamp(t,0.,1.)*n;
    const unsigned i=std::min(n-1,unsigned(a));
    return blend_weapon116(p,path.point[i],path.point[i+1],std::clamp(a-i,0.,1.));
}
template<class Profile,class Path> inline bool valid(const Profile& p,const Path& path) noexcept {
    if(!p.key||!p.generation||!p.entity||!p.primary||!p.geometry||!p.diagnosticBounds59c6||
       p.side<0||p.side>1||!path.valid||path.count<2||path.count>path.point.size())return false;
    if(!finite(p.grip[p.side]))return false;
    for(unsigned i=0;i<path.count;++i)if(!finite(path.point[i]))return false;
    return true;
}
template<class Profile,class Pose> inline sc::Blade blade(const Profile& p,Pose pose) noexcept {
    sc::Blade b{};if(!sword(p))return b;
    const auto axis=m::unit(p.tip-p.grip[p.side].p);if(!axis)return b;
    b.a=point(pose,p.grip[p.side].p+*axis*.08);b.b=point(pose,p.tip);b.radius=.006;b.valid=true;return b;
}
template<class Profile,class Pose> inline sc::Box box(const Profile& p,Pose pose) noexcept {
    return {p.diagnosticBoundsMin59c6,p.diagnosticBoundsMax59c6,pose.p,pose.q,true};
}
template<class Profile> inline double radius(const Profile& p,bool shield) noexcept {
    const auto g=p.grip[p.side].p;
    if(!shield)return std::max(.08,m::length(p.tip-g));
    double r=0;for(unsigned c=0;c<8;++c){const m::Vec3 x{
        c&1?p.diagnosticBoundsMax59c6.x:p.diagnosticBoundsMin59c6.x,
        c&2?p.diagnosticBoundsMax59c6.y:p.diagnosticBoundsMin59c6.y,
        c&4?p.diagnosticBoundsMax59c6.z:p.diagnosticBoundsMin59c6.z};r=std::max(r,m::length(x-g));}
    return r;
}
// Unit-parameter speed bound for EVERY point on every checked segment. Distance
// between two moving convex sets is Lipschitz with the sum of these bounds.
template<class Profile,class Path> inline double speedBound(const Profile& p,const Path& path,double r) noexcept {
    double v=0;for(unsigned i=1;i<path.count;++i)v=std::max(v,
        m::length(point(path.point[i],p.grip[p.side].p)-point(path.point[i-1],p.grip[p.side].p))+
        m::angle(path.point[i-1].q,path.point[i].q)*r);
    return v*(path.count-1);
}
template<class Profile,class Path> inline double minimumProjectionRate(const Profile& p,const Path& path,double r,m::Vec3 n) noexcept {
    double v=1e99;for(unsigned i=1;i<path.count;++i)v=std::min(v,
        m::dot(point(path.point[i],p.grip[p.side].p)-point(path.point[i-1],p.grip[p.side].p),n)-
        m::angle(path.point[i-1].q,path.point[i].q)*r);
    return v*(path.count-1);
}
inline double planeGap(const sc::Blade& blade,const sc::Box& box,m::Vec3 normal) noexcept {
    const auto n=m::rotate(m::conjugate(*m::unit(box.rotation)),normal);
    const double support=std::max(box.lo.x*n.x,box.hi.x*n.x)+std::max(box.lo.y*n.y,box.hi.y*n.y)+
        std::max(box.lo.z*n.z,box.hi.z*n.z)+m::dot(box.position,normal);
    return std::min(m::dot(blade.a,normal),m::dot(blade.b,normal))-blade.radius-support;
}
inline m::Vec3 escapePlane(const sc::Blade& blade,const sc::Box& box,const sc::BoxContact& nearest) noexcept {
    // The first segment/box intersection may lie on a rim face whose normal
    // does NOT separate the whole blade. Use the shallowest WHOLE capsule face
    // plane for an overlap. The exact closest normal is safe only when separated.
    double best=-1e99;m::Vec3 normal{};
    const auto q=*m::unit(box.rotation);
    auto consider=[&](m::Vec3 n){const double g=planeGap(blade,box,n);if(g>best){best=g;normal=n;}};
    for(unsigned i=0;i<3;++i)for(int sign:{-1,1}){
        m::Vec3 n{};if(i==0)n.x=sign;else if(i==1)n.y=sign;else n.z=sign;consider(m::rotate(q,n));}
    if(nearest.gap>0)consider(nearest.normal);
    return normal;
}
template<class Profiles,class Paths> inline Result sweep(const Profiles& p,const Paths& paths,int weapon,int shield,double scale) noexcept {
    Result out{};
    if(weapon<0||weapon>1||shield!=1-weapon||!sword(p[weapon])||!p[shield].shield176()||
       !valid(p[weapon],paths[weapon])||!valid(p[shield],paths[shield])||
       !std::isfinite(scale)||scale<.1||scale>10)return out;
    const auto b0=blade(p[weapon],paths[weapon].point[0]);
    const auto s0=box(p[shield],paths[shield].point[0]);
    if(!sc::valid(b0)||!sc::valid(s0))return out;
    const double rw=radius(p[weapon],false),rs=radius(p[shield],true);
    const double bound=speedBound(p[weapon],paths[weapon],rw)+speedBound(p[shield],paths[shield],rs);
    if(!std::isfinite(bound)||bound>100)return out;
    out.usable=true;const double skin=.002/scale,tolerance=.00001/scale;
    auto contact=[&](double t){++out.queries;return sc::nearest(blade(p[weapon],at(p[weapon],paths[weapon],t)),box(p[shield],at(p[shield],paths[shield],t)));};
    auto c=contact(0);if(!c.valid){out.usable=false;return out;}
    out.gap=c.gap;out.normal=c.normal;
    if(bound<1e-12)return out;
    if(c.gap<=skin+tolerance){
        // A supporting plane certifies monotonic separation, including tangent
        // translation. Initial overlap cannot invent a depenetration teleport:
        // only a path that never moves deeper through this plane is permitted.
        const auto normal=escapePlane(b0,s0,c);out.normal=normal;
        const double outward=minimumProjectionRate(p[weapon],paths[weapon],rw,normal)+
            minimumProjectionRate(p[shield],paths[shield],rs,normal*-1.);
        if(outward>=-1e-10){out.escaping=true;return out;}
        out.fraction=0;out.limited=true;return out;
    }
    double t=0;
    // Conservative advancement certifies the WHOLE interval, including a fast
    // crossing with separated endpoints. Cap exhaustion returns a checked prefix.
    for(unsigned i=0;i<64;++i){
        const double free=c.gap-skin;
        if(free>=bound*(1.-t))return out;
        if(free<=tolerance)break;
        const double advance=.85*free/bound;if(advance<1e-9)break;
        t=std::min(1.,t+advance);c=contact(t);
        if(!c.valid){out.usable=false;return out;}
        out.gap=c.gap;out.normal=c.normal;
    }
    out.fraction=t;out.limited=t<1.-1e-9;return out;
}

// Apply before the coherent hands/body/native-contact publication. Every new
// pose is a prefix of an existing world-checked path. Never feed this OBB contact
// into native damage/material/block code, nor retain an attack beyond the stop.
template<class Input,class Profiles,class State,class Output>
inline Result apply(const Input& in,const Profiles& profiles,State& state,Output& out,int weapon,int shield) noexcept {
    auto result=sweep(profiles,out.checkedPath60,weapon,shield,in.haveWorld?in.worldScale:1.);
    if(!result.usable||!result.limited)return result;
    const auto oldPrepared=state.weapon[weapon].prepared;
    bool removed=false,haveAbort=false;
    using Kind=decltype(out.commands[0].kind);
    unsigned count=0;for(unsigned i=0;i<out.commandCount&&i<out.commands.size();++i){const auto& c=out.commands[i];
        const bool attack=c.kind==Kind::Prepare||c.kind==Kind::Slash||c.kind==Kind::Thrust||c.kind==Kind::Pommel||c.kind==Kind::Zone57;
        if(c.side==weapon&&!c.kick59&&attack){removed=true;continue;}
        haveAbort|=c.side==weapon&&c.kind==Kind::Abort;out.commands[count++]=c;
    }out.commandCount=count;
    const bool candidate=state.weapon[weapon].stroke118.active||state.weapon[weapon].released44c1;
    for(int side=0;side<2;++side){const auto& p=profiles[side];auto& s=state.weapon[side];
        const auto pose=at(p,out.checkedPath60[side],result.fraction);
        auto& path=out.checkedPath60[side];const unsigned segments=path.count-1;
        const unsigned segment=std::min(segments-1,unsigned(result.fraction*segments));
        path.point[segment+1]=pose;path.count=segment+2;
        out.weapons[side]=out.displayWeapons[side]=pose;
        out.hands[side]=out.displayHands[side]=compose(pose,p.grip[side]);
        out.ownShieldBlocked60[side]=true;out.blocked[side]=true;
        out.contact119[side]={};out.contact119[side].known=false;out.contactKnown[side]=false;
        out.contactPrefixKnown126[side]=false;out.sweepDeferred115[side]=false;
        out.contactMotion169r1[side]={};out.impact177[side]={};
        out.impactRecord178r30[side]={};out.impactSpeed[side]=0;out.impactEventTick178r24[side]=0;out.impactEventSpeed178r24[side]=0;
        out.wallBlocked58[side]=out.wallSlid58[side]=out.wallBackout58[side]=out.wallScraped58[side]=false;out.scrapeSpeed58[side]=0;
        s.weapon=pose;s.lastTip=point(pose,p.tip);s.linearVelocity={};s.angularVelocity={};
        s.recoil={};s.recoilVelocity={};s.angularRecoil={};s.angularRecoilVelocity={};
        // A previously checked full-clear native path still covers its suffix
        // from this accepted prefix. Preserve that existing static-world cache;
        // moving-body/world-root changes already disable reuse in the kernel.
        s.wasBlocked=true;s.impactRecord178r30={};s.lastImpactTick=0;
        s.freeLinear178r5={};s.freeAngular178r5={};s.freeTick178r5=0;
        // A shortened path cannot gain a posture that the full solve refused.
        out.guardBySide30c1[side]=out.guardBySide30c1[side]&&natural_guard117(in,p,pose,s.guard117);
        s.guard117=out.guardBySide30c1[side];
    }
    auto& s=state.weapon[weapon];s.prepared=false;s.stroke118={};s.guardStroke36={};s.cocked57={};
    s.spent=true;s.released44c1=false;s.waitingPrepare44c1=false;s.travel=s.thrustTravel=s.reversalTravel=0;s.intentSamples=0;
    out.strokeActive119=false;out.primaryStrokeGesture19=0;out.melee118={};out.strike178={};out.masterHandoff23={};out.arrival39={};
    out.guard117=out.guardBySide30c1[0]||out.guardBySide30c1[1];
    if((oldPrepared||removed||candidate)&&!haveAbort)out.add(Kind::Abort,in,profiles[weapon]);
    state.cached=out;state.cached.commandCount=0;state.cached.impactSpeed[0]=state.cached.impactSpeed[1]=0;
    return result;
}
} // namespace kcd2vr::own_shield_constraint60
