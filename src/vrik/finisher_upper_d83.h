#pragma once
// D83R1: tracked head orientation during native paired finishers.
// The game animation keeps arms, hands, weapon, pelvis and legs so the paired move still lands and
// stays aligned with the other actor. Head, neck and upper torso (spine2/spine3) follow the headset,
// clamped against the native pose. Clinches are never touched (they stay whole-body native).
// No game files, no extra logs: settings live in KCD2VR.ini, status goes to KCD2VR.log.
#include <atomic>
#include <cmath>
#include <cstring>
#include <initializer_list>

namespace kcd2vr::finisher_upper_d83 {
// INI VrFinisherUpperBodyPlayerD83 / VrFinisherUpperBodyNpcD83 retain Claude D83's ON defaults.
// This is a bounded orientation overlay. Arms, weapons and HMD translation are not driven here.
inline std::atomic<bool> player{true}; // Henry does the finisher (alias ..._master)
inline std::atomic<bool> npc{true};    // an NPC finishes / combos Henry (alias ..._slave)

enum Side : unsigned {None=0,Player=1,Npc=2};
inline constexpr float kYawLimitDeg=45.f,kSwingLimitDeg=30.f,kBlendMs=150.f;
// Cumulative share of the clamped head correction carried by each joint, spine2 -> head.
// Small torso share keeps the native shoulders/clavicles (arms stay native) from stretching.
inline constexpr float kShareSpine2=.10f,kShareSpine3=.25f,kShareNeck=.60f,kShareHead=1.f;

inline bool Has(const char* n,const char* s) noexcept {return std::strstr(n,s)!=nullptr;}
inline bool EndsWith(const char* n,const char* s) noexcept {
    const auto a=std::strlen(n),b=std::strlen(s);return a>=b&&std::strcmp(n+a-b,s)==0;
}
// Name-only classification of ONE protected (whole-body native) clip on Henry.
// Returns Player/Npc for a finisher-type paired clip, None for everything else
// (clinch, stealth kill, takedown, grab, horse, death of Henry, unknown names).
inline Side Classify(const char* n,int layer) noexcept {
    // Any FIFO layer: the alias name decides (the D83 log shows masterstrike/combo clips on layer 0).
    if(!n||layer<0||std::strncmp(n,"combat_",7)!=0)return None;
    for(const char* no:{"clinch","stealth","takedown","grab","horse","mount","rider","ragdoll","knock","dialog","carry"})
        if(Has(n,no))return None;
    const bool finisher=Has(n,"masterstrike")||std::strncmp(n,"combat_combo_",13)==0||
        Has(n,"finish")||Has(n,"execut")||Has(n,"_kill");
    if(!finisher)return None;
    if(EndsWith(n,"_master"))return Player;
    // Henry is the victim. A clip where Henry himself dies stays native.
    if(EndsWith(n,"_slave"))return Has(n,"death")||Has(n,"_kill")||Has(n,"execut")?None:Npc;
    return None;
}
inline bool Enabled(Side s) noexcept {
    return s==Player?player.load(std::memory_order_acquire):s==Npc?npc.load(std::memory_order_acquire):false;
}

// Plain quaternion math (x,y,z,w), model space, Z up. Pure so it can be unit tested.
struct Q {float x=0,y=0,z=0,w=1;};
inline bool Valid(Q q) noexcept {
    const double n=double(q.x)*q.x+double(q.y)*q.y+double(q.z)*q.z+double(q.w)*q.w;
    return std::isfinite(n)&&n>.90&&n<1.10;
}
inline Q Norm(Q q) noexcept {const float n=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
    return n>1e-6f&&std::isfinite(n)?Q{q.x/n,q.y/n,q.z/n,q.w/n}:Q{};}
inline Q Mul(Q a,Q b) noexcept {
    return {a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
            a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};
}
inline Q Conj(Q q) noexcept {return {-q.x,-q.y,-q.z,q.w};}
// Scale the rotation angle of q by t (t in [0,1]); shortest arc.
inline Q Scale(Q q,float t) noexcept {
    q=Norm(q);if(q.w<0)q={-q.x,-q.y,-q.z,-q.w};
    const float s=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z);if(s<1e-7f)return {};
    const float a=2.f*std::atan2(s,q.w)*t,k=std::sin(a*.5f)/s;
    return Norm({q.x*k,q.y*k,q.z*k,std::cos(a*.5f)});
}
// Split into yaw about model Z (twist) and the remaining swing; clamp each.
inline Q Clamp(Q r,float yawDeg=kYawLimitDeg,float swingDeg=kSwingLimitDeg) noexcept {
    r=Norm(r);if(r.w<0)r={-r.x,-r.y,-r.z,-r.w};
    constexpr float d2r=3.14159265f/180.f;
    Q twist=Norm({0,0,r.z,r.w});
    if(twist.w<0)twist={-twist.x,-twist.y,-twist.z,-twist.w};
    Q swing=Norm(Mul(r,Conj(twist)));
    const float yaw=2.f*std::atan2(twist.z,twist.w);
    if(std::fabs(yaw)>yawDeg*d2r)twist=Scale(twist,yawDeg*d2r/std::fabs(yaw));
    if(swing.w<0)swing={-swing.x,-swing.y,-swing.z,-swing.w};
    const float sv=std::sqrt(swing.x*swing.x+swing.y*swing.y+swing.z*swing.z);
    const float sa=2.f*std::atan2(sv,swing.w);
    if(sa>swingDeg*d2r)swing=Scale(swing,swingDeg*d2r/sa);
    return Norm(Mul(swing,twist));
}
}
