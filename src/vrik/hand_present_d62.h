#pragma once
// D62: optional late display never owns a semantic hand. The ordinary contact,
// arm solve and contact history complete first. These helpers contain no engine
// calls; the adapter uses only remaining existing collision-query credits.
#include "hand_present_d61.h"
#include "pose_math.h"
#include <array>

namespace kcd2vr::hand_present_d62 {
namespace m=pose_core;
constexpr double kMaxShiftM=.05,kMaxTurnRad=.2617993877991494;
struct Key {
    std::uint64_t sourceEpoch=0,sample=0,tick=0,ns=0,policy=0;
    std::uint64_t poseEpoch=0,posePolicy=0,frameControl=0,contactControl=0;
};
inline bool Same(const Key& a,const Key& b) noexcept {
    return a.sourceEpoch==b.sourceEpoch&&a.sample==b.sample&&a.tick==b.tick&&a.ns==b.ns&&a.policy==b.policy&&
        a.poseEpoch==b.poseEpoch&&a.posePolicy==b.posePolicy&&a.frameControl==b.frameControl&&a.contactControl==b.contactControl;
}
inline bool Same(m::Vec3 a,m::Vec3 b) noexcept {return a.x==b.x&&a.y==b.y&&a.z==b.z;}
inline bool Same(m::Quat a,m::Quat b) noexcept {return a.x==b.x&&a.y==b.y&&a.z==b.z&&a.w==b.w;}
inline bool Same(const m::LocalPose& a,const m::LocalPose& b) noexcept {
    return Same(a.rotation,b.rotation)&&Same(a.fixed_offset,b.fixed_offset);
}
struct Basis {
    m::LocalPose arm[2][3]{},pelvis{},chest{},head{},world{};
    // Exact copied geometry, not a lossy hash. Finger/hover poses and the
    // contact recipe can change while all three arm endpoints stay identical.
    hand_present_d61::Pose finger[2][32]{};
    int fingerId[2][32]{};
    unsigned fingerCount[2]{},palmCount[2]{},geometryPolicy=0;
    m::Vec3 palm[2][7]{};
    bool clear[2]{};
    double scale=1;
};
inline bool Same(const Basis& a,const Basis& b) noexcept {
    if(a.scale!=b.scale||a.geometryPolicy!=b.geometryPolicy||!Same(a.pelvis,b.pelvis)||
       !Same(a.chest,b.chest)||!Same(a.head,b.head)||!Same(a.world,b.world))return false;
    for(unsigned side=0;side<2;++side){
        if(a.fingerCount[side]!=b.fingerCount[side]||a.palmCount[side]!=b.palmCount[side]||a.clear[side]!=b.clear[side])return false;
        if(a.fingerCount[side]>32||a.palmCount[side]>7)return false;
        for(unsigned j=0;j<3;++j)if(!Same(a.arm[side][j],b.arm[side][j]))return false;
        for(unsigned j=0;j<a.palmCount[side];++j)if(!Same(a.palm[side][j],b.palm[side][j]))return false;
        for(unsigned j=0;j<a.fingerCount[side];++j){
            if(a.fingerId[side][j]!=b.fingerId[side][j])return false;
            for(unsigned n=0;n<3;++n)if(a.finger[side][j].p[n]!=b.finger[side][j].p[n])return false;
            for(unsigned n=0;n<4;++n)if(a.finger[side][j].q[n]!=b.finger[side][j].q[n])return false;
        }
    }
    return true;
}
struct Decision {
    Key key{};Basis basis{};m::Quat rotation[2][3]{};
    m::Vec3 target[2]{};std::int64_t displayTime=0;
    unsigned mask=0,interest=0;bool sealed=false;
};
struct Cache {
    std::array<Decision,hand_present_d61::kSlots> slots{};
    Decision retired{};
    std::uint64_t highWater=0;
};
// Seal a decline before doing optional work. A late packet, extra budget or a
// second native callback can never promote that same family into prediction.
// Changed raw geometry can revoke an old display decision, never renew it.
inline Decision& Begin(Cache& cache,const Key& key,const Basis& basis,bool& first) noexcept {
    auto& d=cache.slots[(key.sample/2)%cache.slots.size()];
    first=false;
    if(d.sealed&&Same(d.key,key)){
        if(!Same(d.basis,basis))d.mask=d.interest=0;
        return d;
    }
    // The global source serial also counts non-XR publishers. Eight slots do
    // not imply eight fresh XR frames. An evicted or changed-owner old family
    // must never buy a second first decision after a delayed native callback.
    if(key.sample<=cache.highWater){cache.retired.mask=cache.retired.interest=0;return cache.retired;}
    cache.highWater=key.sample;first=true;
    d={};d.key=key;d.basis=basis;d.sealed=true;
    return d;
}
inline void Revoke(Decision& d,unsigned allowed) noexcept {d.mask&=allowed;d.interest&=allowed;}
// physicalActive means the kernel ran; it is not a held-hand ownership bit.
// Public weapon modes run it for positive-empty equipment too. Permit that
// case only when both current profiles, retained owners and output are empty.
template<class Profiles,class State,class Output>
inline bool IdlePhysical(bool active,const Profiles& profiles,const State& state,const Output& out) noexcept {
    if(!active)return true;
    if(out.bowHand169r2||out.bowHandlingOnlyR12||out.commandCount)return false;
    for(unsigned side=0;side<2;++side)
        if(profiles[side].key||state.weapon[side].key||out.owned[side]||out.active[side]||
           out.support[side]||out.solverOwnedT4[side]||out.displayLatched[side]||out.displayRejoinR13[side])return false;
    return true;
}
// Query subsystem selection (including public VRIK/NPC/fist modes) is not
// ownership. The adapter supplies current positive emptiness and real owners.
inline bool Free(bool context,bool tracked,bool empty,bool owned,float grip,float trigger) noexcept {
    return context&&tracked&&empty&&!owned&&std::isfinite(grip)&&std::isfinite(trigger)&&
        grip>=0&&trigger>=0&&grip<=.05f&&trigger<=.05f;
}
struct Arm {m::Quat rotation[3]{};m::Vec3 elbow{},hand{};bool valid=false;};
inline Arm Solve(const m::LocalPose (&raw)[3],m::Vec3 target,m::Quat desired,double upper,double fore,double scale) noexcept {
    Arm out{};
    const auto travel=m::length(target-raw[2].fixed_offset)*scale;
    const auto turn=m::angle(raw[2].rotation,desired);
    if(!std::isfinite(scale)||scale<=.1||!m::finite(target)||
       travel>kMaxShiftM||turn>kMaxTurnRad||(!std::isfinite(travel))||
       (travel<.0001&&turn<.0001))return out;
    for(const auto& p:raw)if(!m::finite(p.fixed_offset)||!m::unit(p.rotation))return out;
    const auto axis=m::unit(raw[2].fixed_offset-raw[0].fixed_offset);if(!axis)return out;
    const auto bend=m::unit(raw[1].fixed_offset-raw[0].fixed_offset-
        *axis*m::dot(raw[1].fixed_offset-raw[0].fixed_offset,*axis));if(!bend)return out;
    const auto arm=m::solve_arm(raw[0].fixed_offset,target,upper,fore,*bend,*bend);
    // The display experiment cannot move a shoulder, clamp a raw request or
    // change anatomy to manufacture reach. Unreachable lead simply yields.
    if(!arm||arm->reach_limited||m::length(arm->hand-target)>.00001)return out;
    const auto upperDelta=m::from_to(raw[1].fixed_offset-raw[0].fixed_offset,arm->elbow-raw[0].fixed_offset);
    const auto oldFore=m::rotate(upperDelta,raw[2].fixed_offset-raw[1].fixed_offset);
    const auto foreDelta=m::from_to(oldFore,arm->hand-arm->elbow);
    const auto u=m::unit(m::multiply(upperDelta,raw[0].rotation));
    const auto f=m::unit(m::multiply(foreDelta,m::multiply(upperDelta,raw[1].rotation)));
    const auto h=m::unit(desired);if(!u||!f||!h)return out;
    out.rotation[0]=*u;out.rotation[1]=*f;out.rotation[2]=*h;
    out.elbow=arm->elbow;out.hand=arm->hand;out.valid=true;return out;
}
} // namespace kcd2vr::hand_present_d62
