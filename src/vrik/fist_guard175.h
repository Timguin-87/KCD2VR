#pragma once
// Raised empty hands request the existing native block input. This policy never
// decides hit eligibility, perfect blocks, damage, or an NPC's reaction.
#include "fist_motion.h"
namespace kcd2vr::fist_guard175 {
struct State {
    std::uint64_t actor=0,epoch=0,sequence=0,tick=0,raisedSince=0;
    bool guarding=false;pose_core::Vec3 relative[2]{};
};
struct Decision {bool block=false;int zone=-1;};
inline Decision update(State& s,const fists::Sample& in,bool attacking) noexcept {
    namespace m=pose_core;
    const auto head=m::unit(in.headOrientation);
    if(!in.valid||!in.empty[0]||!in.empty[1]||!in.actor||!in.epoch||!in.sequence||!in.tick||!head||
       !m::finite(in.head)||!m::finite(in.controller[0])||!m::finite(in.controller[1])||attacking){s={};return {};}
    const bool same=s.actor==in.actor&&s.epoch==in.epoch&&in.sequence>=s.sequence&&
        in.tick>=s.tick&&in.tick-s.tick<=80;
    if(!same){s={};s.actor=in.actor;s.epoch=in.epoch;}
    // Original XR pose: Y up, -Z forward. Head-relative coordinates reject root
    // locomotion as a gesture and scale independently of the native avatar size.
    const auto r=m::rotate(m::conjugate(*head),in.controller[0]-in.head);
    const auto l=m::rotate(m::conjugate(*head),in.controller[1]-in.head);
    const double sensitivity=melee_tuning_c54::blockH29();
    // Sensitivity changes dwell only. Preserve the face envelope; the separate
    // compact sternum envelope below selects native body zone5, never face zone0.
    // Closed fists protect the face in a smaller region than a weapon blade.
    // These are fist-only caps; the user's weapon plane and native block rules
    // remain unchanged. A narrower configured height remains authoritative.
    const double pad=s.guarding?.03:0.;
    const double width=std::min(.25,double(melee_tuning_c54::bound(melee_tuning_c54::guardHalfWidth.load(),.25f,.75f,.50f)));
    const double height=std::min(.18,double(melee_tuning_c54::bound(melee_tuning_c54::guardHalfHeight.load(),.10f,.35f,.20f)));
    const auto inFront=[&](m::Vec3 p){return p.z<=-.06+pad&&p.z>=-.36-pad&&std::abs(p.x)<=width;};
    const auto raised=[&](m::Vec3 p){return inFront(p)&&p.y>=-height&&p.y<=height;};
    // D65: continuous face/body bands; only an existing guard gets the hold pad.
    const auto body=[&](m::Vec3 p){return inFront(p)&&p.y>=-.48-pad&&p.y<-height;};
    const bool facePosture=raised(r)&&raised(l),bodyPosture=body(r)&&body(l);
    const double separation=m::length(r-l);
    // R114: in a live unarmed fight (native hands empty, opponent owner = responsiveCombatH27) a leftover T24
    // hand claim does not take the guard away. R113 log 00:42:15-00:43:28: all 86 H29 FIST_BLOCK rows said
    // COMPETING_HAND_CLAIM_T24 (left claim left over from the 00:41:02 knife draw that died at PREVIEW), so the
    // raised fists never blocked. Raising a block only presses the native block input; it never touches the
    // claimed hand's item. Outside combat the claim still refuses the guard as before.
    const auto closedR114=[&](int side){return in.grip[side]>double(.60f)&&in.trigger[side]>double(.60f)&&
        (!in.claimedT24[side]||in.responsiveCombatH27);};
    const bool closed=closedR114(0)&&closedR114(1);
    const double dt=same&&in.tick>s.tick?double(in.tick-s.tick)/1000.:0;
    double speed=0,forward=0;
    if(dt>0){speed=std::max(m::length(r-s.relative[0]),m::length(l-s.relative[1]))/dt;
        forward=std::max(-(r.z-s.relative[0].z),-(l.z-s.relative[1].z))/dt;}
    const double entry=melee_tuning_c54::guardEntrySpeed.load(),release=melee_tuning_c54::guardReleaseSpeed.load();
    const bool movingOut=s.guarding&&release>0&&forward>release;
    const bool movingIn=!s.guarding&&entry>0&&dt>0&&speed>entry;
    const bool posture=closed&&(facePosture||bodyPosture)&&separation>=.09&&separation<=.50+pad&&!movingOut&&!movingIn;
    if(!posture){s.raisedSince=0;s.guarding=false;}
    else {
        if(!s.raisedSince)s.raisedSince=in.tick;
        if(in.tick>=s.raisedSince&&double(in.tick-s.raisedSince)>=80./sensitivity)s.guarding=true;
    }
    s.relative[0]=r;s.relative[1]=l;s.sequence=in.sequence;s.tick=in.tick;
    const int zone=facePosture?0:5; // native sector follows the envelope that qualified
    return {s.guarding,s.guarding?zone:-1};
}
} // namespace kcd2vr::fist_guard175
