#pragma once
// V66.89 active pose kernel. Shared verbatim by the retail adapter and executable tests.
// Fixed anatomy, parent-local finger rotations, explicit reach constraints. No engine ABI.
#include <algorithm>
#include <cmath>
#include <optional>
#include <array>
#include <cstdint>
#include <cstddef>
#include "render_bounds63.h"
namespace kcd2vr::pose_core {
struct Vec3 { double x{}, y{}, z{}; };
struct Quat { double x{}, y{}, z{}, w{1}; };
struct LocalPose { Quat rotation{}; Vec3 fixed_offset{}; };
inline Vec3 operator+(Vec3 a, Vec3 b) { return {a.x+b.x,a.y+b.y,a.z+b.z}; }
inline Vec3 operator-(Vec3 a, Vec3 b) { return {a.x-b.x,a.y-b.y,a.z-b.z}; }
inline Vec3 operator*(Vec3 a, double b) { return {a.x*b,a.y*b,a.z*b}; }
inline double dot(Vec3 a, Vec3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
inline double length(Vec3 a) { return std::sqrt(dot(a,a)); }
inline bool finite(Vec3 a) { return std::isfinite(a.x)&&std::isfinite(a.y)&&std::isfinite(a.z); }
inline std::optional<Vec3> unit(Vec3 a) {
    double n=length(a); if(!finite(a)||!std::isfinite(n)||n<1e-10) return std::nullopt;
    return a*(1/n);
}
inline std::optional<Quat> unit(Quat q) {
    double n=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
    if(!std::isfinite(n)||n<1e-10) return std::nullopt;
    return Quat{q.x/n,q.y/n,q.z/n,q.w/n};
}
inline Quat multiply(Quat a, Quat b) {
    return {a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,
            a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
            a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,
            a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};
}
inline Vec3 rotate(Quat q, Vec3 v) {
    Quat p=multiply(multiply(q, {v.x,v.y,v.z,0}), {-q.x,-q.y,-q.z,q.w});
    return {p.x,p.y,p.z};
}
inline std::optional<Quat> slerp(Quat a, Quat b, double t) {
    auto ua=unit(a),ub=unit(b); if(!ua||!ub||!std::isfinite(t)) return std::nullopt;
    a=*ua; b=*ub; t=std::clamp(t,0.0,1.0);
    double d=a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;
    if(d<0) { b={-b.x,-b.y,-b.z,-b.w}; d=-d; }
    d=std::clamp(d,0.0,1.0); double x=1-t,y=t;
    if(d<0.9995) { double theta=std::acos(d),sn=std::sin(theta);
        x=std::sin((1-t)*theta)/sn; y=std::sin(t*theta)/sn; }
    return unit(Quat{x*a.x+y*b.x,x*a.y+y*b.y,x*a.z+y*b.z,x*a.w+y*b.w});
}
// immediate-parent local rotations; the offset is anatomy, not an interpolated
// hand-relative endpoint. Caller must compose all joints in parent-first order.
inline std::optional<LocalPose> blend_finger(Quat relaxed, Quat fist, Vec3 offset, double curl) {
    auto q=slerp(relaxed,fist,curl); if(!q||!finite(offset)) return std::nullopt;
    return LocalPose{*q,offset};
}
struct ArmSolution { Vec3 elbow,hand,bend; bool reach_limited; };
// upper/fore lengths are immutable validated anatomy inputs. No live native elbow
// input exists. pole/previous_bend must already be in the same frame as the target.
inline std::optional<ArmSolution> solve_arm(Vec3 shoulder,Vec3 target,double upper,double fore,
                                          Vec3 pole,Vec3 previous_bend,double reserve=-1.0) {
    if(!finite(shoulder)||!finite(target)||!finite(pole)||!finite(previous_bend)||!std::isfinite(upper)||!std::isfinite(fore)||
       upper<=0||fore<=0||!std::isfinite(reserve)||reserve < -1.0) return std::nullopt;
    auto dir=unit(target-shoulder); if(!dir) return std::nullopt;
    // A fixed 8mm reserve implied ~20 degrees of elbow flexion. Use an angular
    // 2-degree singularity margin instead: anatomy is unchanged, radial loss <0.1mm.
    if(reserve<0)reserve=upper+fore-std::sqrt(upper*upper+fore*fore+2*upper*fore*std::cos(2.0*3.141592653589793/180.0));
    const double lo=std::abs(upper-fore)+1e-6,hi=upper+fore-reserve;
    if(hi<=lo) return std::nullopt;
    const Vec3 projected=pole-*dir*dot(pole,*dir);
    const Vec3 old=previous_bend-*dir*dot(previous_bend,*dir);
    auto bend=unit(projected), previous=unit(old);
    if(length(projected)<0.15 && previous) {
        if(bend && dot(*bend,*previous)<0) *bend=*bend*(-1);
        const double w=std::clamp(length(projected)/0.15,0.0,1.0);
        bend=unit(*previous*(1-w)+(bend?*bend:*previous)*w);
    }
    if(!bend) bend=previous;
    // Ambiguous pole is a rejected solve, not a new attack-driven pole.
    if(!bend) return std::nullopt;
    const double desired=length(target-shoulder),d=std::clamp(desired,lo,hi);
    const double x=(upper*upper-fore*fore+d*d)/(2*d);
    const double h=std::sqrt(std::max(0.0,upper*upper-x*x));
    return ArmSolution{shoulder+*dir*x+*bend*h,shoulder+*dir*d,*bend,desired!=d};
}
enum class Finger { Thumb,Index,Middle,Ring,Pinky };
inline double finger_input(Finger finger,double grip,double trigger) {
    return std::clamp(finger==Finger::Index?trigger:grip,0.0,1.0);
}

inline Quat conjugate(Quat q) { return {-q.x,-q.y,-q.z,q.w}; }
inline Vec3 cross(Vec3 a,Vec3 b) { return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; }
inline Quat from_to(Vec3 a, Vec3 b) {
    auto ua=unit(a),ub=unit(b); if(!ua||!ub)return {};
    const double d=std::clamp(dot(*ua,*ub),-1.0,1.0);
    if(d>0.999999) return {};
    if(d<-.999999) { auto u=unit(cross(*ua,std::abs(ua->z)<.8?Vec3{0,0,1}:Vec3{0,1,0}));
        return u?Quat{u->x,u->y,u->z,0}:Quat{}; }
    auto c=cross(*ua,*ub); return *unit(Quat{c.x,c.y,c.z,1+d});
}
// Distribute the axial component onto the actual forearm, not sibling helper bones.
// Swing stays at the wrist; twisting about the forearm axis cannot lengthen the limb.
inline double angle(Quat a,Quat b) {
    auto x=unit(a),y=unit(b); if(!x||!y)return 1e9;
    return 2*std::acos(std::clamp(std::abs(x->x*y->x+x->y*y->y+x->z*y->z+x->w*y->w),0.0,1.0));
}
// V66.92: distribute an HMD delta by model-axis rotation-vector components.
// Cry model axes here are X=pitch-like, Y=roll-like, Z=yaw-like after the
// established OpenXR->Cry basis conversion. This avoids applying the same
// scalar to yaw/pitch/roll while remaining free of Euler order singularities.
inline Vec3 rotation_vector(Quat q) {
    auto u=unit(q); if(!u)return {};
    q=*u; if(q.w<0)q={-q.x,-q.y,-q.z,-q.w};
    const double w=std::clamp(q.w,-1.0,1.0),a=2.0*std::acos(w);
    const double s=std::sqrt(std::max(0.0,1.0-w*w));
    if(s<1e-9||a<1e-9)return {};
    return {q.x*a/s,q.y*a/s,q.z*a/s};
}
inline Quat from_rotation_vector(Vec3 v) {
    if(!finite(v))return {};
    const double a=length(v); if(a<1e-10)return {};
    const double h=.5*a,s=std::sin(h)/a;
    auto q=unit(Quat{v.x*s,v.y*s,v.z*s,std::cos(h)});
    return q?*q:Quat{};
}
inline Quat axis_weighted_delta(Quat q,Vec3 weight,Vec3 max_abs) {
    Vec3 v=rotation_vector(q);
    v={std::clamp(v.x*weight.x,-std::abs(max_abs.x),std::abs(max_abs.x)),
       std::clamp(v.y*weight.y,-std::abs(max_abs.y),std::abs(max_abs.y)),
       std::clamp(v.z*weight.z,-std::abs(max_abs.z),std::abs(max_abs.z))};
    return from_rotation_vector(v);
}
// Pure collision clamp used by executable tests and the retail adapter.
// The query returns a world-distance to the first blocking surface or nullopt.
// Controller intent is never overwritten: only the allowed endpoint is clamped.
template<class Query>
inline Vec3 constrain_swept_point(Vec3 previous_allowed,Vec3 requested,double skin,Query&& query,bool& blocked) {
    blocked=false; const Vec3 sweep=requested-previous_allowed; const double n=length(sweep);
    if(!finite(previous_allowed)||!finite(requested)||!std::isfinite(skin)||skin<0||n<1e-7)return requested;
    const auto hit=query(previous_allowed,sweep);
    if(!hit||!std::isfinite(*hit)||*hit<=0||*hit>=n)return requested;
    blocked=true; const double d=std::max(0.0,*hit-skin);
    return previous_allowed+sweep*(d/n);
}
// Fixed-size chain solve. Zero-length helper links are retained; no segment scaling.
// The root is fixed and forward passes constrain bend away from the initial posture.
template<std::size_t N>
inline bool solve_spine(std::array<Vec3,N>& p,const std::array<double,N>& lengths,
                        std::size_t count,Vec3 goal,double max_bend=0.65) {
    if(count<2||count>N||!finite(goal)||!std::isfinite(max_bend)||max_bend<0)return false;
    for(std::size_t i=1;i<count;++i)if(!std::isfinite(lengths[i])||lengths[i]<0)return false;
    const auto initial=p; const Vec3 root=p[0];
    for(std::size_t i=0;i<count;++i)if(!finite(p[i]))return false;
    for(int pass=0;pass<8;++pass) {
        p[count-1]=goal;
        for(std::size_t i=count-1;i>0;--i) {
            auto d=unit(p[i-1]-p[i]); if(!d)d=unit(initial[i-1]-initial[i]);
            if(!d && lengths[i]>1e-7)return false;
            p[i-1]=p[i]+(d?*d:Vec3{})*lengths[i];
        }
        p[0]=root;
        for(std::size_t i=1;i<count;++i) {
            auto d=unit(p[i]-p[i-1]),ref=unit(initial[i]-initial[i-1]);
            if(!d)d=ref;
            if(!d && lengths[i]>1e-7)return false;
            if(d&&ref) {
                const double a=std::acos(std::clamp(dot(*d,*ref),-1.0,1.0));
                if(a>max_bend) { auto q=slerp({},from_to(*ref,*d),max_bend/a); if(q)d=unit(rotate(*q,*ref)); }
            }
            p[i]=p[i-1]+(d?*d:Vec3{})*lengths[i];
        }
    }
    return true;
}
struct Stamp { std::uint64_t epoch{},sample{},tick{}; std::uintptr_t actor{}; };
inline bool fresh(Stamp s,std::uint64_t epoch,std::uintptr_t actor,std::uint64_t now,
                  std::uint64_t maximum_age=100) {
    return s.actor && s.actor==actor && s.epoch==epoch && s.sample && s.tick && now>=s.tick && now-s.tick<=maximum_age;
}
// Commit failure must restore every attempted write, including the setter that failed.
// Getter/Setter operate on the same representation. No later success flag owns cleanup.
template<class Pose,class Setter,std::size_t N>
inline bool commit(const std::array<Pose,N>& original,const std::array<Pose,N>& desired,
                   const std::array<bool,N>& selected,std::size_t count,Setter&& setter,
                   std::array<bool,N>& attempted) {
    attempted.fill(false); if(count>N)return false;
    for(std::size_t i=0;i<count;++i)if(selected[i]) {
        attempted[i]=true;
        if(!setter(i,desired[i])) {
            for(std::size_t j=i+1;j-->0;)if(attempted[j])setter(j,original[j]);
            return false;
        }
    }
    return true;
}
} // namespace kcd2vr::pose_core
