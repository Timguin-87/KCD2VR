#pragma once
#include "hand_shape59c7f.h"
#include "motion_time.h"
#include <cstdint>

namespace kcd2vr::self_contact54 {
using namespace pose_core;
struct Blade {Vec3 a{},b{};double radius=.006;bool valid=false;};
inline bool valid(const Blade& b) noexcept {
    const double n=length(b.b-b.a);
    return b.valid&&finite(b.a)&&finite(b.b)&&std::isfinite(b.radius)&&b.radius>=.002&&b.radius<=.02&&n>=.12&&n<=2.5;
}
inline bool valid(const hand_shape59c7f::Shape& h) noexcept {
    if(!h.valid||!h.count||h.count>26)return false;
    for(unsigned i=0;i<h.count;++i){const auto& p=h.part[i];
        if(!finite(p.a)||!finite(p.b)||!std::isfinite(p.radius)||p.radius<0||p.radius>.03)return false;}
    return true;
}
struct Contact {Vec3 hand{},blade{},normal{};double gap=1e9,handU=0,bladeU=0;unsigned segment=0;bool valid=false;};
inline double coordinate(Vec3 p,Vec3 a,Vec3 b) noexcept {
    const auto d=b-a;const double n=dot(d,d);return n>1e-12?std::clamp(dot(p-a,d)/n,0.,1.):0.;
}
inline Contact nearest(const hand_shape59c7f::Shape& h,Vec3 wrist,const Blade& b) noexcept {
    Contact out{};if(!valid(h)||!valid(b)||!finite(wrist))return out;
    for(unsigned i=0;i<h.count;++i){const auto& p=h.part[i];
        const auto c=hand_shape59c7f::closest(wrist+p.a,wrist+p.b,b.a,b.b);
        const auto d=c.a-c.b;const double gap=length(d)-p.radius-b.radius;
        if(gap<out.gap){out={c.a,c.b,unit(d).value_or(Vec3{}),gap,
            coordinate(c.a,wrist+p.a,wrist+p.b),coordinate(c.b,b.a,b.b),i,true};}
    }
    return out;
}
struct Separation {Vec3 correction{};double residual=0;bool contact=false,resolved=false,valid=false;};
// Analytic visual capsules, never engine collision/hit evidence. Only the empty
// wrist moves. Two fixed passes of at most 26 finger/palm segments, <= 6 cm total.
inline Separation separate(const hand_shape59c7f::Shape& h,Vec3 wrist,const Blade& b,Vec3 fallback) noexcept {
    Separation out{};if(!valid(h)||!valid(b)||!finite(wrist))return out;
    const auto axis=unit(b.b-b.a);if(!axis)return out;
    fallback=fallback-*axis*dot(fallback,*axis);
    const auto safe=unit(fallback).value_or(unit(cross(*axis,Vec3{0,0,1})).value_or(Vec3{1,0,0}));
    for(unsigned pass=0;pass<2;++pass){const auto c=nearest(h,wrist+out.correction,b);
        if(!c.valid)return {};out.residual=c.gap;
        if(c.gap>=.002){out.resolved=true;break;}out.contact=true;
        const auto direction=unit(c.normal).value_or(safe);
        const auto proposed=out.correction+direction*(.002-c.gap);
        const double distance=length(proposed);
        out.correction=distance>.06?proposed*(.06/distance):proposed;
    }
    out.valid=finite(out.correction)&&length(out.correction)<=.06000001;
    return out;
}
struct Identity {
    std::uintptr_t character=0,player=0;std::uint64_t epoch=0,request=0,key=0,generation=0;
    std::uint32_t entity=0;int hand=-1,weapon=-1;
};
inline bool valid(const Identity& x) noexcept {
    return x.character&&x.player&&x.epoch&&x.request&&x.key&&x.generation&&x.entity&&
        x.hand>=0&&x.hand<2&&x.weapon==1-x.hand;
}
inline bool same(const Identity& a,const Identity& b) noexcept {
    return a.character==b.character&&a.player==b.player&&a.epoch==b.epoch&&a.request==b.request&&
        a.key==b.key&&a.generation==b.generation&&a.entity==b.entity&&a.hand==b.hand&&a.weapon==b.weapon;
}
struct Sample {
    Identity owner{};hand_shape59c7f::Shape shape{};Blade blade{};Vec3 wrist{};Quat orientation{};
    std::uint64_t tick=0,sample=0,timeNs=0;double scale=1;bool eligible=false;
};
enum class Source : unsigned char { EmptyHandSword, VisualShieldBounds };
struct Event : Identity {
    std::uint64_t serial=0,tick=0,sample=0;double relativeSpeed=0,approachSpeed=0;
    Source source=Source::EmptyHandSword;std::uint64_t shieldKey=0,shieldGeneration=0;std::uint32_t shieldEntity=0;
};
struct State {Sample previous{};bool have=false,touching=false;std::uint64_t lastPat=0;};
inline bool usable(const Sample& x) noexcept {
    return x.eligible&&valid(x.owner)&&valid(x.shape)&&valid(x.blade)&&finite(x.wrist)&&
        bool(unit(x.orientation))&&std::isfinite(x.scale)&&x.scale>=.1&&x.scale<=10&&x.tick&&x.sample;
}
inline void retire(State& s) noexcept {const auto cooldown=s.lastPat;s={};s.lastPat=cooldown;}
// Called only for completed same-owner body+weapon packets. A held/proximity
// pose cannot emit an event: a new contact requires inward relative motion of
// corresponding material points, from the hand OR the sword, and a cooldown.
inline Event committed(State& s,const Sample& x) noexcept {
    Event out{};if(!usable(x)){retire(s);return out;}
    const auto current=nearest(x.shape,x.wrist,x.blade);if(!current.valid){retire(s);return out;}
    const bool touching=current.gap<=.004;
    const bool keepTouch=current.gap<=.012;
    const auto& p=s.previous;
    const double dt=s.have?motion_time::elapsed(p.tick,x.tick,p.timeNs,x.timeNs):0.;
    const bool continuous=s.have&&same(x.owner,p.owner)&&x.sample>p.sample&&x.tick>=p.tick&&
        x.tick-p.tick<=80&&dt>=.001&&dt<=.080&&p.shape.count==x.shape.count&&std::abs(x.scale-p.scale)<1e-6;
    if(s.have&&same(x.owner,p.owner)&&x.sample==p.sample)return out; // duplicate native consumer
    if(continuous&&touching&&!s.touching){
        // Reject finger-curl-only contacts: the rigid wrist/hand or sword must
        // move. Reproject THIS contact point through the previous wrist basis.
        const auto handDelta=multiply(*unit(p.orientation),conjugate(*unit(x.orientation)));
        const Vec3 oldHand=p.wrist+rotate(handDelta,current.hand-x.wrist);
        const Vec3 oldBlade=p.blade.a+(p.blade.b-p.blade.a)*current.bladeU;
        const auto relative=((current.hand-current.blade)-(oldHand-oldBlade))*(x.scale/dt);
        const auto normal=unit(current.normal).value_or(unit(oldHand-oldBlade).value_or(Vec3{}));
        const double speed=length(relative),approach=-dot(relative,normal);
        if(std::isfinite(speed)&&std::isfinite(approach)&&speed>=.25&&approach>=.15&&
           (!s.lastPat||(x.tick>=s.lastPat&&x.tick-s.lastPat>=700))){
            static_cast<Identity&>(out)=x.owner;out.tick=x.tick;out.sample=x.sample;
            out.relativeSpeed=speed;out.approachSpeed=approach;s.lastPat=x.tick;
        }
    }
    s.touching=continuous?(s.touching?keepTouch:touching):touching;
    s.previous=x;s.have=true;return out;
}

// Shield gesture proxy uses the ACTUAL local model bounds and its current rigid
// transform. It is expressly NOT a native shield contact/block/damage witness.
struct Box {Vec3 lo{},hi{},position{};Quat rotation{};bool valid=false;};
inline bool valid(const Box& b) noexcept {
    const auto e=b.hi-b.lo;const double mn=std::min({e.x,e.y,e.z}),mx=std::max({e.x,e.y,e.z});
    return b.valid&&finite(b.lo)&&finite(b.hi)&&finite(b.position)&&bool(unit(b.rotation))&&
        mn>=.0005&&mn<=.18&&mx>=.20&&mx<=2.5&&(e.x+e.y+e.z-mn-mx)>=.15&&e.x<=2.5&&e.y<=2.5&&e.z<=2.5;
}
inline double component(Vec3 v,unsigned i) noexcept {return i==0?v.x:i==1?v.y:v.z;}
inline Vec3 clamped(Vec3 v,Vec3 a,Vec3 b) noexcept {
    return {std::clamp(v.x,a.x,b.x),std::clamp(v.y,a.y,b.y),std::clamp(v.z,a.z,b.z)};
}
struct BoxContact {Vec3 blade{},target{},targetLocal{},normal{};double gap=1e9,bladeU=0;bool valid=false;};
// Exact closest segment-to-local-AABB distance: at most eight plane breakpoints
// and seven quadratic intervals. No sampled/invented shield thickness.
inline BoxContact nearest(const Blade& blade,const Box& box) noexcept {
    BoxContact out{};if(!valid(blade)||!valid(box))return out;
    const auto q=*unit(box.rotation),inv=conjugate(q);
    const auto a=rotate(inv,blade.a-box.position),b=rotate(inv,blade.b-box.position),d=b-a;
    std::array<double,8> knots{};unsigned n=2;knots[0]=0;knots[1]=1;
    for(unsigned k=0;k<3;++k)if(std::abs(component(d,k))>1e-12)for(double edge:{component(box.lo,k),component(box.hi,k)}){
        const double t=(edge-component(a,k))/component(d,k);if(t>0&&t<1)knots[n++]=t;}
    std::sort(knots.begin(),knots.begin()+n);double best=1e99,tBest=0;Vec3 target{};
    auto consider=[&](double t){const auto p=a+d*t,c=clamped(p,box.lo,box.hi);const double distance=dot(p-c,p-c);
        if(distance<best){best=distance;tBest=t;target=c;}};
    for(unsigned k=1;k<n;++k){const double left=knots[k-1],right=knots[k],middle=(left+right)*.5;
        double aa=0,bb=0;const auto p=a+d*middle;
        for(unsigned axis=0;axis<3;++axis){const double value=component(p,axis),v=component(d,axis);
            if(value<component(box.lo,axis)||value>component(box.hi,axis)){
                const double bound=value<component(box.lo,axis)?component(box.lo,axis):component(box.hi,axis);
                aa+=v*v;bb+=v*(component(a,axis)-bound);}}
        consider(left);consider(right);if(aa>1e-12)consider(std::clamp(-bb/aa,left,right));
    }
    const auto point=a+d*tBest;Vec3 normal{};double distance=std::sqrt(std::max(0.,best));
    if(distance>1e-10)normal=(point-target)*(1./distance);
    else { // Initial overlap is observed but cannot itself emit a contact edge.
        double nearestFace=1e99;for(unsigned axis=0;axis<3;++axis)for(unsigned end=0;end<2;++end){
            const double boundary=end?component(box.hi,axis):component(box.lo,axis);
            const double depth=std::abs(component(point,axis)-boundary);
            if(depth<nearestFace){nearestFace=depth;target=point;normal={};
                if(axis==0){target.x=boundary;normal.x=end?1.:-1.;}
                else if(axis==1){target.y=boundary;normal.y=end?1.:-1.;}
                else {target.z=boundary;normal.z=end?1.:-1.;}}}
        distance=-nearestFace;
    }
    out.blade=blade.a+(blade.b-blade.a)*tBest;out.targetLocal=target;
    out.target=box.position+rotate(q,target);out.normal=rotate(q,normal);
    out.gap=distance-blade.radius;out.bladeU=tBest;out.valid=true;return out;
}
struct ShieldSample {
    Identity owner{};Blade blade{};Box box{};std::uint64_t shieldKey=0,shieldGeneration=0;
    std::uint32_t shieldEntity=0;std::uint64_t tick=0,sample=0,timeNs=0;double scale=1;bool eligible=false;
};
struct ShieldState {ShieldSample previous{};bool have=false,touching=false;std::uint64_t lastPat=0;};
inline void retire(ShieldState& s) noexcept {const auto cooldown=s.lastPat;s={};s.lastPat=cooldown;}
inline Event committed(ShieldState& s,const ShieldSample& x) noexcept {
    Event out{};if(!x.eligible||!valid(x.owner)||!x.shieldKey||!x.shieldGeneration||!x.shieldEntity||
        !x.tick||!x.sample||!valid(x.blade)||!valid(x.box)||!std::isfinite(x.scale)||x.scale<.1||x.scale>10){retire(s);return out;}
    const auto c=nearest(x.blade,x.box);if(!c.valid){retire(s);return out;}
    const auto& p=s.previous;const bool sameOwner=s.have&&same(x.owner,p.owner)&&x.shieldKey==p.shieldKey&&
        x.shieldGeneration==p.shieldGeneration&&x.shieldEntity==p.shieldEntity;
    if(sameOwner&&x.sample==p.sample)return out;
    const double dt=sameOwner?motion_time::elapsed(p.tick,x.tick,p.timeNs,x.timeNs):0;
    const bool continuous=sameOwner&&x.sample>p.sample&&x.tick>=p.tick&&x.tick-p.tick<=80&&
        dt>=.001&&dt<=.08&&std::abs(x.scale-p.scale)<1e-6;
    if(continuous&&c.gap<=.004&&!s.touching){
        const auto oldBlade=p.blade.a+(p.blade.b-p.blade.a)*c.bladeU;
        const auto oldTarget=p.box.position+rotate(*unit(p.box.rotation),c.targetLocal);
        const auto velocity=((c.blade-c.target)-(oldBlade-oldTarget))*(x.scale/dt);
        const double speed=length(velocity),approach=-dot(velocity,c.normal);
        if(std::isfinite(speed)&&std::isfinite(approach)&&speed>=.25&&approach>=.15&&
           (!s.lastPat||(x.tick>=s.lastPat&&x.tick-s.lastPat>=700))){
            static_cast<Identity&>(out)=x.owner;out.tick=x.tick;out.sample=x.sample;out.relativeSpeed=speed;out.approachSpeed=approach;
            out.source=Source::VisualShieldBounds;out.shieldKey=x.shieldKey;out.shieldGeneration=x.shieldGeneration;
            out.shieldEntity=x.shieldEntity;s.lastPat=x.tick;
        }
    }
    s.touching=continuous?(s.touching?c.gap<=.012:c.gap<=.004):c.gap<=.004;
    s.previous=x;s.have=true;return out;
}
} // namespace kcd2vr::self_contact54
