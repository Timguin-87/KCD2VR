#pragma once
#include "physical_weapon.h"
namespace kcd2vr::guard112 {
using namespace physical;
struct State {
    std::uint64_t key=0,generation=0,epoch=0,sample=0,tick=0,candidateSince=0,guardSince=0;
    int zone=-1,candidate=-1;bool block=false;Vec3 lastPoint{};
    std::uint64_t postureSince178r29=0; // V66.178R29: first tick of the current guard posture candidate
};
struct Decision {int zone=-1;bool block=false;};
// Retail combat_zone.xml: 0=head, 1=upper_left, 2=upper_right, 5=lower.
// This maps physical geometry to INPUT, never promises a native defense outcome.
inline int sector(Vec3 relative) noexcept {
    if(!m::finite(relative)||std::hypot(relative.x,relative.z)<.10)return -1;
    if(std::abs(relative.z)>std::abs(relative.x))return relative.z>=0?0:5;
    return relative.x<0?1:2;
}
inline Decision update(State& s,const Input& in,const Profile& p,Pose weapon,bool eligible) noexcept {
    const bool shield=in.positionGuard176&&p.side==1&&p.shield176();
    if(!eligible||p.side<0||p.side>1||!in.allowed||!in.tracked[p.side]||(!shield&&(!p.melee()||p.side!=0))||!p.geometry||
       !finite(weapon)||!in.sample||!in.tick||!m::finite(body_shoulder_r38(in,0))||!m::finite(body_shoulder_r38(in,1))){s={};return {};}
    const Vec3 chest=body_center_r38(in);
    const Vec3 grip=point(weapon,p.grip[p.side].p),tip=point(weapon,p.tip),center=shield?grip:(grip+tip)*.5;
    if(!s.sample||s.key!=p.key||s.generation!=p.generation||s.epoch!=in.epoch||
       in.tick<s.tick||in.tick-s.tick>80||in.sample<s.sample){
        s={};s.key=p.key;s.generation=p.generation;s.epoch=in.epoch;s.sample=in.sample;s.tick=in.tick;s.lastPoint=center;
        if(in.positionGuard176){const bool posture=natural_guard117(in,p,weapon,false);
            s.postureSince178r29=posture?in.tick:0;
            s.block=posture&&release_combat_r92::guardDwell(in.guardEnterDwellMs178r29,shield)==0&&in.guardEnterMaxSpeed178r29<=0;return {-1,s.block};}
        return {};
    }
    if(in.sample==s.sample)return {s.zone,s.block};
    const Vec3 offset=body_vector117(in,center-chest);
    int candidate=sector(offset);
    // Ten degrees of angular hysteresis around the four 45-degree boundaries.
    if(candidate>=0&&candidate!=s.zone&&s.zone>=0){
        const double major=std::max(std::abs(offset.x),std::abs(offset.z));
        const double minor=std::min(std::abs(offset.x),std::abs(offset.z));
        if(major<minor*1.43)candidate=s.zone;
    }
    if(candidate!=s.candidate){s.candidate=candidate;s.candidateSince=in.tick;}
    if(candidate<0)s.zone=-1;
    else if(in.tick>=s.candidateSince&&in.tick-s.candidateSince>=40)s.zone=candidate;
    // V66.117: posture entry is the block edge. Direction dwell is independent;
    // neither a centered weapon nor a fast raise may delay that edge. Holding
    // guard never re-presses block to manufacture a perfect-block timing window.
    // V66.178R29 ("blocking too sensitive"): the F4 position cone (+/-18 deg in front of the chest) fired on every
    // pass of the blade through it, pressing/releasing native block many times per swing and cancelling prepared
    // attacks. ENTERING a block now needs a slow blade centre (<= guardEnterMaxSpeed) that stays in the cone for
    // guardEnterDwellMs; an already held block keeps the old hysteresis so a timed raise still releases on exit.
    const bool posture=natural_guard117(in,p,weapon,s.block);
    if(!s.block&&posture){
        const double dt=double(in.tick-s.tick)*.001;const double speed=dt>0?m::length(center-s.lastPoint)/dt:0.0;
        if(!s.postureSince178r29)s.postureSince178r29=in.tick;
        const bool slow=std::isfinite(speed)&&(in.guardEnterMaxSpeed178r29<=0||speed<=in.guardEnterMaxSpeed178r29);
        if(!slow)s.postureSince178r29=in.tick; // a fast pass restarts the dwell
        const bool dwelt=in.tick>=s.postureSince178r29&&in.tick-s.postureSince178r29>=release_combat_r92::guardDwell(in.guardEnterDwellMs178r29,shield);
        s.block=slow&&dwelt;
    }else{s.block=posture;if(!posture)s.postureSince178r29=0;}
    s.sample=in.sample;s.tick=in.tick;s.lastPoint=center;
    return {s.zone,s.block};
}
}
