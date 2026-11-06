#pragma once
#include "../native_weapon_body_t4.h"
#include "physical_weapon.h"
#include "native_held_intent_h28.h"
namespace kcd2vr::native_held_pose_t4 {
namespace b=native_weapon_body_t4;namespace p=physical;namespace m=pose_core;
// Optional observation of the actual failed branch; no alternate acceptance.
enum class ApplyReasonD42:unsigned {NotAttempted,Applied,State,Owner,Lease,Tracking,CancelGeneration,BodyIdentity,SourceOrder,Freshness,WorldFrame,Profile,WorldPose,Reach};
inline bool fresh(std::uint64_t now,std::uint64_t tick) noexcept {return tick&&now>=tick&&now-tick<=80;}
inline bool sameReceipt(const b::Achieved& a,const b::Achieved& c) noexcept {
    return a.owner==c.owner&&a.colliderLease==c.colliderLease&&a.worldGeneration==c.worldGeneration&&
        a.bodyGeneration&&a.bodyGeneration==c.bodyGeneration&&a.bodyId==c.bodyId&&a.trackingEpoch==c.trackingEpoch&&a.cancelGeneration==c.cancelGeneration;
}
// The native solver is the ONLY integrator. This path does not call ph::step,
// ray shortening, display latching, or the local own-shield presentation clamp.
inline bool apply(const b::Input& source,const b::Achieved& a,std::uint64_t now,
                  p::Input& in,const std::array<p::Profile,2>& profiles,p::Output& out,p::State& state,const b::ContactOwnerH24* contact=nullptr,std::uint64_t admission=0,ApplyReasonD42* reasonD42=nullptr) noexcept {
    const auto refuseD42=[&](ApplyReasonD42 why)noexcept {if(reasonD42)*reasonD42=why;return false;};
    const auto& w=profiles[0];const bool requestedAttacksH28=in.attacks;
    if(a.state!=b::State::Active||!a.poseValid||a.colliderRetired)return refuseD42(ApplyReasonD42::State);
    if(a.owner!=source.owner)return refuseD42(ApplyReasonD42::Owner);
    if(a.colliderLease!=source.colliderLease)return refuseD42(ApplyReasonD42::Lease);
    if(a.trackingEpoch!=source.trackingEpoch)return refuseD42(ApplyReasonD42::Tracking);
    if(a.cancelGeneration!=source.cancelGeneration)return refuseD42(ApplyReasonD42::CancelGeneration);
    if(!a.worldGeneration||!a.bodyGeneration||a.bodyId<0)return refuseD42(ApplyReasonD42::BodyIdentity);
    if(!a.sample||a.sample>in.sample)return refuseD42(ApplyReasonD42::SourceOrder);
    if(!fresh(now,a.sourceTick)||!fresh(now,a.observedTick)||a.sourceTick>in.tick)return refuseD42(ApplyReasonD42::Freshness);
    if(!in.haveWorld||!p::finite(in.world)||!std::isfinite(in.worldScale)||in.worldScale<.1||in.worldScale>10)return refuseD42(ApplyReasonD42::WorldFrame);
    if(!w.primary||!w.geometry||w.side!=0||w.key!=a.owner.key||w.generation!=a.owner.generation||w.entity!=a.owner.entityId||
       !p::finite(w.grip[0])||!m::finite(a.weaponWorld.p)||!m::finite(a.linear)||!m::finite(a.angular))return refuseD42(ApplyReasonD42::Profile);
    p::Pose world{a.weaponWorld.q,a.weaponWorld.p};
    if(!p::finite(world))return refuseD42(ApplyReasonD42::WorldPose);
    auto local=p::compose(p::inverse(in.world),world);local.p=local.p*(1/in.worldScale);
    const auto hand=p::compose(local,w.grip[0]);
    if(!p::finite(local)||!p::finite(hand)||!m::finite(in.shoulders[0])||!std::isfinite(in.reach[0])||
       m::length(hand.p-in.shoulders[0])>in.reach[0]-.002)return refuseD42(ApplyReasonD42::Reach);
    const bool continuous=state.epoch==in.epoch&&state.bodyEpoch==in.bodyEpoch&&
        state.weapon[0].key==w.key&&state.weapon[0].generation==w.generation&&state.cached.solverOwnedT4[0]&&
        state.cached.solverLeaseT4==a.colliderLease&&state.cached.solverWorldGenT4==a.worldGeneration&&
        state.cached.solverBodyGenT4==a.bodyGeneration&&state.cached.solverTrackingEpochH24==a.trackingEpoch&&
        state.cached.solverCancelGenerationH24==a.cancelGeneration&&a.sample>=state.cached.solverSourceSampleT4&&
        a.sourceTick>=state.cached.solverSourceTickT4;
    const auto previousSourceSample=state.cached.solverSourceSampleT4;
    const auto previous=continuous?state.weapon[0]:p::WeaponState{};
    out={};out.hands[0]=hand;out.weapons[0]=local;out.hands[1]=in.hands[1];
    out.owned[0]=out.active[0]=out.solverOwnedT4[0]=true;
    out.solverWorldT4=world;out.solverLeaseT4=a.colliderLease;out.solverWorldGenT4=a.worldGeneration;out.solverBodyGenT4=a.bodyGeneration;
    out.solverSourceTickT4=a.sourceTick;out.solverSourceSampleT4=a.sample;
    out.solverTrackingEpochH24=a.trackingEpoch;out.solverCancelGenerationH24=a.cancelGeneration;
    // Collision contacts do not yet have the exact native RPG actor/part join.
    // No ray fallback or authored swing is allowed to manufacture that join.
    out.contactKnown[0]=false;in.attacks=in.contact=in.dynamics=false;
    state={};state.epoch=in.epoch;state.bodyEpoch=in.bodyEpoch;state.sample=in.sample;
    auto& held=state.weapon[0];held.key=w.key;held.generation=w.generation;held.weapon=local;
    held.lastDesired=local;held.ready=true;held.lastSample=in.sample;held.lastTick=in.tick;
    // Input-only posture uses the actual achieved blade AND tracked intention.
    // Native solver remains the sole integrator; no ph::step/raycast/contact is
    // introduced. Raw moving pressure cannot become quiet because a body stops.
    const auto requested=p::compose(in.hands[0],p::inverse(w.grip[0]));
    const auto relative=p::compose(in.intentHands[0],p::inverse(w.grip[0]));
    const auto tip=p::point(requested,w.tip),relativeTip=p::point(relative,w.tip);
    const bool semantic=in.epoch==a.owner.request&&in.bodyEpoch==a.owner.epoch&&
        in.positionGuard176&&in.naturalCombat117&&in.coherentMelee119&&in.allowed&&
        in.tracked[0]&&in.intentTracked[0]&&p::finite(in.hands[0])&&p::finite(in.intentHands[0]);
    const bool posture=semantic&&p::natural_guard117(in,w,local,previous.guard117)&&
        p::natural_guard117(in,w,requested,previous.guard117);
    const double dt=previous.lastTick&&in.tick>previous.lastTick?double(in.tick-previous.lastTick)*.001:0;
    const bool history=continuous&&previous.intentReady&&previous.relativeIntentReady&&dt>0&&dt<=.08;
    double speed=std::numeric_limits<double>::infinity(),forward=-1;
    if(history){
        const auto modelHand=in.hands[0].p-previous.intentHand,relativeHand=in.intentHands[0].p-previous.relativeIntentHand;
        const auto modelTip=tip-previous.intentTip,relativeTipDelta=relativeTip-previous.relativeIntentTip;
        const double intentSpeed=std::min(std::max(m::length(modelHand),m::length(modelTip)),
            std::max(m::length(relativeHand),m::length(relativeTipDelta)))/dt;
        const auto worldTipOffset=m::rotate(world.q,w.tip*in.worldScale);
        const double achievedSpeed=m::length(a.linear+m::cross(a.angular,worldTipOffset))/in.worldScale;
        speed=std::max(intentSpeed,achievedSpeed);
        const auto fwd=p::body_axes(in).forward;
        const auto relativeForward=m::rotate(m::multiply(in.intentHands[0].q,m::conjugate(in.hands[0].q)),fwd);
        forward=std::max(0.,std::min(m::dot(modelTip,fwd),m::dot(relativeTipDelta,relativeForward))/dt);
    }
    held.guardEntry30c1=previous.guardEntry30c1;
    // A duplicated physics receipt cannot earn dwell or refresh an old guard.
    const bool newReceipt=!continuous||a.sample>previousSourceSample;
    held.guard117=posture&&(newReceipt?guard_parry176::enter(held.guardEntry30c1,true,previous.guard117,a.sourceTick,
        speed,in.guardEnterMaxSpeed178r29,release_combat_r92::guardDwell(in.guardEnterDwellMs178r29,w.shield176()),in.guardExitSpeed178r45,forward):
        (previous.guard117&&guard_parry176::enter(held.guardEntry30c1,true,true,a.sourceTick,
            speed,in.guardEnterMaxSpeed178r29,release_combat_r92::guardDwell(in.guardEnterDwellMs178r29,w.shield176()),in.guardExitSpeed178r45,forward)));
    if(!posture)held.guardEntry30c1={};
    out.guard117=out.guardBySide30c1[0]=held.guard117;
    held.intentHand=in.hands[0].p;held.intentTip=tip;held.relativeIntentHand=in.intentHands[0].p;held.relativeIntentTip=relativeTip;
    held.intentReady=held.relativeIntentReady=semantic;
        solver_command_h28::Stamp proof{};
        const auto& frameH28=a.intentFrameH28;
    const bool frameValidH28=frameH28.valid&&std::isfinite(frameH28.scale)&&frameH28.scale>=.1&&frameH28.scale<=10&&
        p::finite({frameH28.modelWorld.q,frameH28.modelWorld.p})&&p::finite({frameH28.trackingFromModel.q,frameH28.trackingFromModel.p});
    if(semantic&&frameValidH28&&contact&&solver_command_h28::make(a,*contact,admission,now,proof)){
        in.attacks=requestedAttacksH28;
        out.solverH28=proof;
        native_held_intent_h28::step(a,proof,in,w,previous,continuous,newReceipt,local,out,held);
    }
    const auto& committedH29=held.committedGestureH26;
    if(in.attacks&&!held.guard117&&committedH29.gesture&&committedH29.gesture==held.gesture59c3&&committedH29.key==w.key&&committedH29.generation==w.generation&&
       committedH29.epoch==in.bodyEpoch&&committedH29.request==in.epoch&&now>=committedH29.tick&&now-committedH29.tick<=160){
        out.solverGestureH29={committedH29.gesture,committedH29.tick,committedH29.gesture,
            held.solverCommitWorldH29,committedH29.kind,committedH29.origin};
    }
    held.solverTickH28=a.sourceTick;
    p::cache_profiles_r20(state,profiles);
    state.world=in.world;state.worldScale=in.worldScale;state.haveWorld=in.haveWorld;state.cached=out;state.cached.commandCount=0;
    if(reasonD42)*reasonD42=ApplyReasonD42::Applied;return true;
}
}
