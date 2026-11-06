#pragma once
// A2 SMITHING ANVIL CONTACT (Miki 27 Oct: "only the anvil needs to be collided with - iron and hammer").
// Replaces the rejected J28-B cylinder. Pure logic + one coherent mailbox, host tested (tests/anvil_a2_test.cpp).
//
// Source: no anvil entity/bounds pin exists in the retail tables we have (C_Blacksmithing instance+18 player,
// +2C active, +98 model, model+2E0..2F8 tool holders; none of them is the anvil). The only CURRENT station-owned
// geometry fact is the native StrokeHit point (C51060, caller 859A76): where the native minigame lands a blow on the
// workpiece lying on the anvil. So:
//  - every native blow is published with its full identity: smith instance, native lifetime (witness175), smith
//    epoch (witness175.smithEpochC1, bumps on every station enter/exit/Clear = save/load), Henry entity and Henry's
//    native root WORLD (the station-aligned actor root, IEntity+58) at that blow. One SnapshotMailbox, no torn reads.
//  - the anvil is LEARNED once per visit from the first kLockBlows blows and then LOCKED: top = lowest blow height
//    minus the workpiece half thickness (native local bounds), footprint = blow centroid with a radius from the blow
//    spread (+margin, clamped). Later blows never move it. A blow far outside the locked footprint, a different
//    instance/lifetime/epoch/Henry, a moved native root, or a new pose/body epoch retires it (relearn).
//  - until locked there is no collision (one visible line says so).
// Solve: from the LAST ALLOWED tool transform to the CURRENT DESIRED one (translation + rotation together), the
// largest fraction that keeps every probe out of the anvil; then a horizontal slide toward the desired xy at that
// height if that is also clear. A last-allowed pose that already penetrates (start inside) is never corrected: the
// hand follows the controller until it is outside, so there is no jump. Turning the feature off / losing the anvil
// while corrected returns to the controller at most 2 cm and 5 deg per frame.
#include "../../core/snapshot_mailbox.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::subsystems::smithing::anvil_a2 {
inline std::atomic<bool> enabled{false}; // any anvil contact on (A3: derived from mode)
// A3 (Miki: "Try it, if you can find it"): VrSmithAnvilContact 0 OFF, 1 REAL ANVIL (the game's own static physics
// entity under the first native blow, its world AABB from CPhysicalPlaceholder m_BBox, see anvil_physics_a3.h),
// 2 LEARNED (Codex A2 experiment above). REAL never falls back to LEARNED.
enum class Mode : int {Off=0,Real=1,Learned=2};
inline std::atomic<int> mode{int(Mode::Off)};
inline std::atomic<std::uint64_t> modeRevisionD39{1};
inline int clampMode(int m) noexcept {return m<0||m>2?int(Mode::Real):m;}
inline void setMode(int m) noexcept {
    m=clampMode(m);
    if(mode.exchange(m,std::memory_order_acq_rel)!=m)modeRevisionD39.fetch_add(1,std::memory_order_acq_rel);
    enabled.store(m!=0,std::memory_order_release);
}
inline bool physicsMode() noexcept {return mode.load(std::memory_order_acquire)==int(Mode::Real);}
inline constexpr unsigned kLockBlows=3,kRing=8;
inline constexpr double kRadiusMin=.15,kRadiusMax=.30,kRadiusMargin=.08,kDepth=.80,kRelearnOutside=.15,
    kRootMove=.10,kRootYawCos=.9962 /*5 deg*/,kMaxStep=.02,kMaxStepRad=.0872665 /*5 deg*/;

struct D3 {double x=0,y=0,z=0;};
struct DQ {double x=0,y=0,z=0,w=1;};
inline D3 add(D3 a,D3 b) noexcept {return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline D3 sub(D3 a,D3 b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline D3 mul(D3 a,double s) noexcept {return {a.x*s,a.y*s,a.z*s};}
inline double dot(D3 a,D3 b) noexcept {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline double len(D3 a) noexcept {return std::sqrt(dot(a,a));}
inline bool finite(D3 a) noexcept {return std::isfinite(a.x)&&std::isfinite(a.y)&&std::isfinite(a.z);}
inline DQ qnorm(DQ q) noexcept {const double n=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
    if(!(n>1e-9)||!std::isfinite(n))return {};return {q.x/n,q.y/n,q.z/n,q.w/n};}
inline DQ qmul(DQ a,DQ b) noexcept {return {a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
    a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};}
inline DQ qconj(DQ q) noexcept {return {-q.x,-q.y,-q.z,q.w};}
inline D3 rot(DQ q,D3 v) noexcept {const DQ p{v.x,v.y,v.z,0};const DQ r=qmul(qmul(q,p),qconj(q));return {r.x,r.y,r.z};}
inline double qangle(DQ a,DQ b) noexcept {double d=std::fabs(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w);d=std::min(1.0,d);return 2*std::acos(d);}
inline DQ slerp(DQ a,DQ b,double t) noexcept {
    a=qnorm(a);b=qnorm(b);double d=a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;if(d<0){b={-b.x,-b.y,-b.z,-b.w};d=-d;}
    if(d>.9995)return qnorm({a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t,a.w+(b.w-a.w)*t});
    const double th=std::acos(d),s=std::sin(th),wa=std::sin((1-t)*th)/s,wb=std::sin(t*th)/s;
    return {a.x*wa+b.x*wb,a.y*wa+b.y*wb,a.z*wa+b.z*wb,a.w*wa+b.w*wb};
}

// ---------------------------------------------------------------- native blow publication (hook thread)
struct Identity {
    std::uintptr_t instance=0,henry=0;std::uint64_t lifetime=0,smithEpoch=0;
    bool operator==(const Identity& o) const noexcept {return instance==o.instance&&henry==o.henry&&lifetime==o.lifetime&&smithEpoch==o.smithEpoch;}
    explicit operator bool() const noexcept {return instance&&henry&&lifetime&&smithEpoch;}
};
struct Blow {Identity id{};D3 point{};D3 rootPos{};D3 rootFwd{};std::uint64_t serial=0,tick=0;double workpieceHalf=-1;
    bool boxValid=false;D3 boxMin{},boxMax{};std::uintptr_t phys=0;std::uint64_t modeRevisionD39=0;}; // A3: the physics anvil found for this blow
struct Ring {std::array<Blow,kRing> blow{};std::uint64_t serial=0;};
inline core::SnapshotMailbox<Ring> mailbox{};
inline std::atomic<std::uint64_t> published{0};
// the native root matrix is CryEngine Matrix34 row major (IEntity+58): column 1 = forward, column 3 = position
inline bool rootFrom(const float (&m)[12],D3& pos,D3& fwd) noexcept {
    for(float v:m)if(!std::isfinite(v)||std::fabs(v)>1e6f)return false;
    pos={m[3],m[7],m[11]};fwd={m[1],m[5],m[9]};const double l=len({fwd.x,fwd.y,0});
    if(!(l>.2))return false;fwd={fwd.x/l,fwd.y/l,0};return true;
}
// single producer (the native StrokeHit callback); readers copy a coherent ring
inline Ring producerRing{};
inline bool publish(const Blow& in) noexcept {
    if(!bool(in.id)||!finite(in.point)||!finite(in.rootPos))return false;
    Ring r=producerRing;Blow b=in;b.serial=++r.serial;r.blow[(b.serial-1)%kRing]=b;
    if(!mailbox.publish(r))return false;
    producerRing=r;published.store(r.serial,std::memory_order_release);return true;
}

// ---------------------------------------------------------------- learned anvil (pose owner only)
struct Anvil {
    Identity id{};std::uint64_t poseEpoch=0,lastSerial=0;D3 rootPos{},rootFwd{};
    D3 sum{};double minZ=1e9,half=0;D3 first[kLockBlows]{};unsigned blows=0;
    bool locked=false;D3 center{};double top=0,radius=0;
    bool box=false;D3 boxMin{},boxMax{};std::uintptr_t phys=0; // A3: REAL anvil = world AABB of its static physics entity
    std::uint64_t modeRevisionD39=0;int sourceModeD39=int(Mode::Off);
};
inline void retireGeometryD39(Anvil& a) noexcept {
    const auto serial=a.lastSerial,revision=a.modeRevisionD39;const auto source=a.sourceModeD39;
    a=Anvil{};a.lastSerial=serial;a.modeRevisionD39=revision;a.sourceModeD39=source;
}
// Runs on the pose owner even if no new native blow arrives. A live switch may
// never leave LEARNED geometry active under REAL, or revive an OFF-era box.
inline bool syncModeD39(Anvil& a,int source,std::uint64_t revision) noexcept {
    if(a.sourceModeD39==source&&a.modeRevisionD39==revision)return false;
    const bool retired=a.blows||a.locked;retireGeometryD39(a);
    a.sourceModeD39=source;a.modeRevisionD39=revision;return retired;
}
enum class Retire {None,Identity,PoseEpoch,RootMoved,OutsideFootprint};
inline const char* retireName(Retire r) noexcept {
    return r==Retire::Identity?"STATION_OR_SAVE_CHANGED":r==Retire::PoseEpoch?"BODY_POSE_EPOCH":r==Retire::RootMoved?"NATIVE_ROOT_MOVED":
        r==Retire::OutsideFootprint?"BLOW_OUTSIDE_ANVIL":"NONE";}
inline bool rootSame(const Anvil& a,D3 pos,D3 fwd) noexcept {
    return len(sub(pos,a.rootPos))<=kRootMove&&dot(fwd,a.rootFwd)>=kRootYawCos;
}
// current = identity the consumer sees NOW (witness/socket); returns why it was retired (None = kept)
inline Retire validate(Anvil& a,const Identity& current,std::uint64_t poseEpoch,D3 rootPos,D3 rootFwd) noexcept {
    if(!a.blows)return Retire::None;
    Retire r=Retire::None;
    if(!(a.id==current))r=Retire::Identity;else if(a.poseEpoch!=poseEpoch)r=Retire::PoseEpoch;
    else if(!rootSame(a,rootPos,rootFwd))r=Retire::RootMoved;
    if(r!=Retire::None){retireGeometryD39(a);}
    return r;
}
inline void lock(Anvil& a) noexcept {
    D3 c=mul(a.sum,1.0/a.blows);double spread=0;
    for(unsigned i=0;i<a.blows&&i<kLockBlows;++i)spread=std::max(spread,len({a.first[i].x-c.x,a.first[i].y-c.y,0}));
    a.center={c.x,c.y,0};a.radius=std::clamp(spread+kRadiusMargin,kRadiusMin,kRadiusMax);
    a.top=a.minZ-std::clamp(a.half,0.0,.05);a.locked=true;
}
// integrate every new blow of the ring once; a blow of another identity never joins
inline bool outsideBox(const Anvil& a,D3 p,double margin) noexcept {
    return p.x<a.boxMin.x-margin||p.x>a.boxMax.x+margin||p.y<a.boxMin.y-margin||p.y>a.boxMax.y+margin;}
inline void lockBox(Anvil& a,const Blow& b) noexcept {
    a.box=true;a.boxMin=b.boxMin;a.boxMax=b.boxMax;a.phys=b.phys;a.top=b.boxMax.z;
    a.center={.5*(b.boxMin.x+b.boxMax.x),.5*(b.boxMin.y+b.boxMax.y),0};
    a.radius=.5*len({b.boxMax.x-b.boxMin.x,b.boxMax.y-b.boxMin.y,0});a.locked=true;
}
inline Retire integrate(Anvil& a,const Ring& r,const Identity& current,std::uint64_t poseEpoch,double currentHalf=-1,bool physics=false,std::uint64_t modeRevision=0) noexcept {
    Retire out=Retire::None;
    for(std::uint64_t s=a.lastSerial+1;s<=r.serial;++s){
        if(r.serial-s>=kRing)continue; // overwritten, skip (never borrow a later blow under its serial)
        const Blow& b=r.blow[(s-1)%kRing];if(b.serial!=s)continue;
        a.lastSerial=s;
        if(!(b.id==current)||(modeRevision&&b.modeRevisionD39!=modeRevision))continue;
        if(a.blows&&!(a.id==b.id)){retireGeometryD39(a);out=Retire::Identity;}
        if(a.blows&&!rootSame(a,b.rootPos,b.rootFwd)){retireGeometryD39(a);out=Retire::RootMoved;}
        if(physics){ // A3 REAL: lock at once from the physics box this blow carries; a blow without one never teaches
            if(a.locked&&a.box&&!outsideBox(a,b.point,kRelearnOutside))continue;
            if(a.locked){retireGeometryD39(a);out=Retire::OutsideFootprint;}
            if(!b.boxValid)continue;
            a.id=b.id;a.poseEpoch=poseEpoch;a.rootPos=b.rootPos;a.rootFwd=b.rootFwd;a.blows=1;a.first[0]=b.point;a.sum=b.point;
            a.minZ=b.point.z;lockBox(a,b);continue;
        }
        if(a.locked&&a.box){retireGeometryD39(a);out=Retire::OutsideFootprint;} // mode switched
        if(a.locked){
            if(len({b.point.x-a.center.x,b.point.y-a.center.y,0})>a.radius+kRelearnOutside){
                retireGeometryD39(a);out=Retire::OutsideFootprint;}
            else continue; // stable: a locked anvil is never moved by later blows
        }
        if(!a.blows){a.id=b.id;a.poseEpoch=poseEpoch;a.rootPos=b.rootPos;a.rootFwd=b.rootFwd;}
        if(a.blows<kLockBlows)a.first[a.blows]=b.point;
        a.sum=add(a.sum,b.point);a.minZ=std::min(a.minZ,b.point.z);
        const double half=b.workpieceHalf>=0?b.workpieceHalf:currentHalf;if(half>=0)a.half=std::max(a.half,half);
        ++a.blows;if(a.blows>=kLockBlows)lock(a);
    }
    return out;
}

// ---------------------------------------------------------------- penetration + solve (world space)
struct Probe {D3 local{};double radius=.01;};           // in the tool's own frame
struct Pose {D3 p{};DQ q{};};                            // tool WORLD transform
inline double penetration(const Anvil& a,const Pose& pose,const Probe* probes,unsigned n) noexcept {
    if(!a.locked)return 0;double worst=0;
    for(unsigned i=0;i<n;++i){
        const D3 w=add(pose.p,rot(pose.q,probes[i].local));
        if(a.box){ // A3 REAL: box footprint + full physics height
            const double r=probes[i].radius;
            if(w.x<=a.boxMin.x-r||w.x>=a.boxMax.x+r||w.y<=a.boxMin.y-r||w.y>=a.boxMax.y+r)continue;
            const double bottom=w.z-r;if(w.z<a.boxMin.z||bottom>=a.top)continue;
            worst=std::max(worst,a.top-bottom);continue;
        }
        const double r=len({w.x-a.center.x,w.y-a.center.y,0});
        if(r>=a.radius+probes[i].radius)continue;
        const double bottom=w.z-probes[i].radius;
        if(w.z<a.top-kDepth||bottom>=a.top)continue;
        worst=std::max(worst,a.top-bottom);
    }
    return worst;
}
inline Pose lerp(const Pose& a,const Pose& b,double t) noexcept {return {add(a.p,mul(sub(b.p,a.p),t)),slerp(a.q,b.q,t)};}
struct Solve {Pose pose{};double fraction=1;double depth=0;bool corrected=false,startInside=false,slid=false;};
inline constexpr double kClear=.001;
// Continuous translation and conservative quaternion arc enclosure. Each arc is
// enclosed by its chord plus r*(1-cos(theta/2)); exact segment/cylinder intervals
// cover endpoint-clear crossings. At most90 segments,3 probes; no native queries.
inline bool segmentEntry(D3 a,D3 b,double radius,double bottom,double top,double& enter) noexcept {
    const D3 d=sub(b,a);double lo=0,hi=1;
    const double aa=d.x*d.x+d.y*d.y,bb=2*(a.x*d.x+a.y*d.y),cc=a.x*a.x+a.y*a.y-radius*radius;
    if(aa<1e-20){if(cc>=0)return false;}
    else {const double det=bb*bb-4*aa*cc;if(det<=0)return false;
        const double root=std::sqrt(det);lo=std::max(lo,(-bb-root)/(2*aa));hi=std::min(hi,(-bb+root)/(2*aa));}
    if(std::fabs(d.z)<1e-12){if(a.z<=bottom||a.z>=top)return false;}
    else {double p=(bottom-a.z)/d.z,q=(top-a.z)/d.z;if(p>q)std::swap(p,q);lo=std::max(lo,p);hi=std::min(hi,q);}
    if(!(hi>lo+1e-12)||hi<=0||lo>=1)return false;
    enter=std::clamp(lo,0.0,1.0);return true;
}
// A3: slab test of segment a->b against the box [mn,mx]; enter = first parameter inside (open box)
inline bool segmentEntryBox(D3 a,D3 b,D3 mn,D3 mx,double& enter) noexcept {
    const double s[3]={a.x,a.y,a.z},e[3]={b.x,b.y,b.z},lo3[3]={mn.x,mn.y,mn.z},hi3[3]={mx.x,mx.y,mx.z};double lo=0,hi=1;
    for(int k=0;k<3;++k){const double d=e[k]-s[k];
        if(std::fabs(d)<1e-12){if(s[k]<=lo3[k]||s[k]>=hi3[k])return false;continue;}
        double p=(lo3[k]-s[k])/d,q=(hi3[k]-s[k])/d;if(p>q)std::swap(p,q);lo=std::max(lo,p);hi=std::min(hi,q);if(!(hi>lo))return false;}
    if(!(hi>lo+1e-12)||hi<=0||lo>=1)return false;
    enter=std::clamp(lo,0.0,1.0);return true;
}
// Exact minimum of a probe's rotated Z over the shortest quaternion arc.
// Both slerp and its small-angle nlerp fast path traverse this same arc. Taking
// the minimum translation Z separately is conservative for every time along
// the motion; it does not assume rotation and translation have equal timing.
inline double minimumRotatedZD39(DQ from,DQ to,D3 local) noexcept {
    from=qnorm(from);to=qnorm(to);
    if(from.x*to.x+from.y*to.y+from.z*to.z+from.w*to.w<0)to={-to.x,-to.y,-to.z,-to.w};
    const D3 start=rot(from,local),end=rot(to,local);double minimum=std::min(start.z,end.z);
    const DQ delta=qnorm(qmul(to,qconj(from)));const double sine=len({delta.x,delta.y,delta.z});
    if(sine<1e-12)return minimum;
    const D3 axis{delta.x/sine,delta.y/sine,delta.z/sine};
    const double angle=2*std::atan2(sine,std::max(0.0,delta.w));
    const double constant=axis.z*dot(axis,start),cosine=start.z-constant,sinus=axis.x*start.y-axis.y*start.x;
    constexpr double pi=3.14159265358979323846;
    double atMinimum=std::atan2(sinus,cosine)+pi;if(atMinimum>=2*pi)atMinimum-=2*pi;
    if(atMinimum>=0&&atMinimum<=angle)minimum=std::min(minimum,constant-std::hypot(cosine,sinus));
    return minimum;
}
inline bool staysAboveTopD39(const Anvil& a,const Pose& from,const Pose& to,const Probe& probe) noexcept {
    const double top=a.top+probe.radius-kClear-1e-12;
    const double lower=std::min(from.p.z,to.p.z)+minimumRotatedZD39(from.q,to.q,probe.local);
    if(lower>=top)return true;
    // D70: do not combine the LOWEST translation at the start with a rotated
    // tip's LOWEST height at the end. That independent bound glues a resting
    // hammer at t=0 even when upward translation outruns every rotational dip.
    // Prove monotonic separation along the SAME segment. The nlerp angular
    // speed bound2*tan(angle/2) also bounds slerp; no endpoint-only escape.
    const auto q0=qnorm(from.q),q1=qnorm(to.q);const double angle=qangle(q0,q1);
    const auto local0=rot(q0,probe.local);if(from.p.z+local0.z<top||to.p.z<from.p.z)return false;
    if(angle<1.e-12)return true;
    if(angle>=3.141592653589793)return false;
    auto delta=qnorm(qmul(q1,qconj(q0)));if(delta.w<0)delta={-delta.x,-delta.y,-delta.z,-delta.w};
    const double sine=len({delta.x,delta.y,delta.z});if(sine<1.e-12)return true;
    const D3 axis{delta.x/sine,delta.y/sine,delta.z/sine};
    const double constant=axis.z*dot(axis,local0),cosine=local0.z-constant,
        sinus=axis.x*local0.y-axis.y*local0.x;
    const double dropBound=2*std::tan(angle*.5)*std::hypot(cosine,sinus);
    return to.p.z-from.p.z+1.e-12>=dropBound;
}
inline bool firstEntry(const Anvil& a,const Pose& from,const Pose& to,const Probe* probes,unsigned n,double& first) noexcept {
    first=1;const double angle=qangle(qnorm(from.q),qnorm(to.q));
    const unsigned segments=std::max(1u,std::min(90u,unsigned(std::ceil(angle/.03490658503988659))));
    const double arc=1.0-std::cos(angle/(2*segments));
    for(unsigned j=0;j<segments;++j){
        const double t0=double(j)/segments,t1=double(j+1)/segments;
        const auto p0=lerp(from,to,t0),p1=lerp(from,to,t1);double hit=1;bool found=false;
        for(unsigned i=0;i<n;++i){
            // The chord's symmetric arc padding otherwise puts a resting
            // probe inside at t=0 even while its entire true arc lifts or turns above
            // the top. This proven separating-plane path must not glue the hammer down.
            if(staysAboveTopD39(a,p0,p1,probes[i]))continue;
            const D3 w0=add(p0.p,rot(p0.q,probes[i].local)),w1=add(p1.p,rot(p1.q,probes[i].local));
            const double error=len(probes[i].local)*arc;
            if(a.box){const double g=probes[i].radius+error;double t=1;
                if(segmentEntryBox(w0,w1,{a.boxMin.x-g,a.boxMin.y-g,a.boxMin.z-error},{a.boxMax.x+g,a.boxMax.y+g,a.top+probes[i].radius-kClear+error},t)){
                    hit=std::min(hit,t);found=true;}
                continue;}
            const D3 c0{w0.x-a.center.x,w0.y-a.center.y,w0.z},c1{w1.x-a.center.x,w1.y-a.center.y,w1.z};
            double t=1;if(segmentEntry(c0,c1,a.radius+probes[i].radius+error,a.top-kDepth-error,a.top+probes[i].radius-kClear+error,t)){
                hit=std::min(hit,t);found=true;}
        }
        if(found){first=t0+(t1-t0)*hit;return true;}
    }
    return false;
}
inline Solve solve(const Anvil& a,const Pose& lastAllowed,const Pose& desired,const Probe* probes,unsigned n) noexcept {
    Solve s{};s.pose=desired;if(!a.locked||!n)return s;
    s.depth=penetration(a,desired,probes,n);
    if(penetration(a,lastAllowed,probes,n)>kClear){s.startInside=true;return s;}
    double first=1;if(!firstEntry(a,lastAllowed,desired,probes,n,first))return s;
    s.fraction=std::max(0.0,first-1e-7);s.pose=lerp(lastAllowed,desired,s.fraction);s.corrected=true;
    Pose slide=s.pose;slide.p.x=desired.p.x;slide.p.y=desired.p.y;double slideEntry=1;
    if(!firstEntry(a,s.pose,slide,probes,n,slideEntry)&&penetration(a,slide,probes,n)<=kClear){s.pose=slide;s.slid=true;}
    return s;
}
// bounded return to the controller (feature off / anvil lost while a correction was active)
inline Pose recover(const Pose& from,const Pose& to,bool& done) noexcept {
    const D3 d=sub(to.p,from.p);const double dl=len(d),ang=qangle(from.q,to.q);
    const double t=std::min(1.0,std::min(dl>kMaxStep?kMaxStep/dl:1.0,ang>kMaxStepRad?kMaxStepRad/ang:1.0));
    done=t>=1.0;return done?to:lerp(from,to,t);
}
}
