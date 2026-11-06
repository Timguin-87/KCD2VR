#pragma once
// E1 (Claude R8d lane E): the camera base never falls back to the game's own eye
// for a short gap of the VRIK body frame. The native eye follows the animated /
// arm-solved head bone (measured 5-15 cm forward/down of the tracked base in the
// H50 log). A rejected pose (extreme reach, hand tracking hold > 80 ms, stale
// packet) used to retire the tracked frame for a few frames, so camera_hook took
// the native eye: the "head jumps forward/down and back" when moving the arms.
// Now the LAST accepted base is held model-local and re-composed with the CURRENT
// character root (travel/turn stay live); the HMD delta is still applied once by
// camera_hook. Same actor/epoch/control/scale/calibration only, bounded age.
#include "pose_math.h"
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::camera_hold_e1 {
namespace m=pose_core;
inline std::atomic<std::uint32_t> maxHoldMs{1500}; // 0 = old behaviour (native eye at once)
inline std::atomic<std::uint64_t> heldFrames{0},expired{0};

// Switchable repair. An explicit saved OFF preserves the plain-gameplay gap policy.
// This retains an anatomical base, never old HMD tracking or old root travel.
inline std::atomic<bool> pickupBridgeD55{true};
struct ContextD55 {
    unsigned kind=0; // 0 native/unknown, 1 plain gameplay, 2 exact Picking
    std::uint64_t tracking=0,serial=0,generation=0;
    std::uintptr_t action=0,scope=0;
    std::uint32_t token=0;int side=-1;
};
inline bool same(const ContextD55& a,const ContextD55& b) noexcept {
    return a.kind==b.kind&&a.tracking==b.tracking&&a.serial==b.serial&&a.generation==b.generation&&
        a.action==b.action&&a.scope==b.scope&&a.token==b.token&&a.side==b.side;
}
inline bool compatible(const ContextD55& accepted,const ContextD55& current) noexcept {
    return accepted.kind&&current.kind&&accepted.tracking&&accepted.tracking==current.tracking&&
        (accepted.kind==1||same(accepted,current)||(accepted.kind==2&&current.kind==1));
}

// D64: pickup transitions use a tiny anatomical-base bridge. HMD/root remain current.
inline std::uint32_t bridgeLimitD64(const ContextD55& accepted,const ContextD55& current,std::uint32_t configured,
                                    bool pickupHandover=false) noexcept {
    return (pickupHandover||accepted.kind==2||current.kind==2)?(configured<60?configured:60):configured;
}

struct Sample {
    std::uintptr_t actor=0;std::uint64_t epoch=0,control=0,tick=0;double scale=1;
    m::Vec3 calibratedHead{},local{},headingLocal{};bool heading=false,valid=false;
    ContextD55 contextD55{};
};
inline bool finite3(m::Vec3 v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}
inline m::Quat inverse(m::Quat q) noexcept {return {-q.x,-q.y,-q.z,q.w};}
inline bool sameCal(m::Vec3 a,m::Vec3 b) noexcept {return std::fabs(a.x-b.x)<1e-6&&std::fabs(a.y-b.y)<1e-6&&std::fabs(a.z-b.z)<1e-6;}

// Called after an ACCEPTED tracked base: keep it in the character's local frame.
inline Sample remember(std::uintptr_t actor,std::uint64_t epoch,std::uint64_t control,std::uint64_t tick,double scale,
                       m::Vec3 calibratedHead,m::Vec3 root,m::Quat rotation,m::Vec3 eye,double hx,double hy) noexcept {
    Sample s{};
    if(!actor||!std::isfinite(scale)||scale<.1||scale>10||!finite3(root)||!finite3(eye))return s;
    const auto d=m::Vec3{eye.x-root.x,eye.y-root.y,eye.z-root.z};
    const auto l=m::rotate(inverse(rotation),d);
    s.local={l.x/scale,l.y/scale,l.z/scale};
    if(!finite3(s.local)||std::sqrt(s.local.x*s.local.x+s.local.y*s.local.y+s.local.z*s.local.z)>3)return Sample{};
    const double h=hx*hx+hy*hy;
    if(std::isfinite(h)&&h>.5){auto hl=m::rotate(inverse(rotation),m::Vec3{hx,hy,0});hl.z=0;
        const double n=std::sqrt(hl.x*hl.x+hl.y*hl.y);if(std::isfinite(n)&&n>1e-5){s.headingLocal={hl.x/n,hl.y/n,0};s.heading=true;}}
    s.actor=actor;s.epoch=epoch;s.control=control;s.tick=tick;s.scale=scale;s.calibratedHead=calibratedHead;s.valid=true;
    return s;
}
enum class Why:int {Held=0,None=1,Identity=2,Expired=3,Disabled=4,Invalid=5};
// The gap: same body identity, age <= limit -> base = CURRENT root + held local.
inline Why hold(const Sample& s,std::uintptr_t actor,std::uint64_t epoch,std::uint64_t control,double scale,
                m::Vec3 calibratedHead,m::Vec3 root,m::Quat rotation,std::uint64_t now,std::uint32_t limitMs,
                m::Vec3& eye,double& hx,double& hy) noexcept {
    eye={};hx=hy=0;
    if(!limitMs)return Why::Disabled;
    if(!s.valid)return Why::None;
    if(s.actor!=actor||s.epoch!=epoch||s.control!=control||std::fabs(s.scale-scale)>1e-6||!sameCal(s.calibratedHead,calibratedHead))return Why::Identity;
    if(now<s.tick||now-s.tick>limitMs)return Why::Expired;
    if(!finite3(root))return Why::Invalid;
    const auto w=m::rotate(rotation,m::Vec3{s.local.x*scale,s.local.y*scale,s.local.z*scale});
    eye={root.x+w.x,root.y+w.y,root.z+w.z};
    if(!finite3(eye))return Why::Invalid;
    if(s.heading){auto f=m::rotate(rotation,s.headingLocal);f.z=0;const double n=std::sqrt(f.x*f.x+f.y*f.y);
        if(std::isfinite(n)&&n>1e-5){hx=f.x/n;hy=f.y/n;}}
    return Why::Held;
}
inline const char* name(Why w) noexcept {
    switch(w){case Why::Held:return "HELD";case Why::None:return "NO_ACCEPTED_BASE";case Why::Identity:return "BODY_CHANGED";
    case Why::Expired:return "EXPIRED";case Why::Disabled:return "DISABLED";default:return "INVALID";}
}
// R8d2 (Codex P1): who may use a held base on a failed frame. admitted = the core's FINAL ownership check passed,
// plainBefore/plainAfter = plain gameplay owns the camera before and after the current world read.
struct Gap {bool useHeld=false,clearCache=false;};
inline Gap gap(bool admitted,bool plainBefore,bool plainAfter,Why why) noexcept {
    if(!admitted||!plainBefore)return {false,true};
    if(why!=Why::Held)return {false,false};
    if(!plainAfter)return {false,true};
    return {true,false};
}
}
