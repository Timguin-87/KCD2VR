#pragma once
#include "physical_weapon.h"
#include "solver_command_h28.h"
namespace kcd2vr::native_held_intent_h28 {
namespace p=physical;namespace m=pose_core;namespace b=native_weapon_body_t4;namespace sc=solver_command_h28;
// Only the already achieved rigid blade supplies these deltas. This function
// never integrates a body, casts a ray, predicts a hand hit, or creates a contact.
inline void step(const b::Achieved& a,const sc::Stamp& proof,const p::Input& in,
 const p::Profile& weapon,const p::WeaponState& previous,bool continuous,bool newReceipt,
 p::Pose actual,p::Output& out,p::WeaponState& held) noexcept {
 const auto& frame=a.intentFrameH28;
 if(!frame.valid||!std::isfinite(frame.scale)||frame.scale<.1||frame.scale>10)return;
 const p::Pose sourceWorld{frame.modelWorld.q,frame.modelWorld.p},trackingFromModel{frame.trackingFromModel.q,frame.trackingFromModel.p};
 if(!p::finite(sourceWorld)||!p::finite(trackingFromModel))return;
 auto model=p::compose(p::inverse(sourceWorld),p::Pose{a.weaponWorld.q,a.weaponWorld.p});model.p=model.p*(1/frame.scale);
 const auto relative=p::compose(trackingFromModel,model);
 if(!p::finite(model)||!p::finite(relative))return;
 actual=model;held.solverModelH28=model;held.solverRelativeH28=relative;
 if(!continuous||!newReceipt){
  if(continuous){held.solverModelH28=previous.solverModelH28;held.solverRelativeH28=previous.solverRelativeH28;held.stroke118=previous.stroke118;held.gesture59c3=previous.gesture59c3;
   held.prepared=previous.prepared;held.stroke117=previous.stroke117;held.zone117=previous.zone117;
   held.origin167=previous.origin167;held.originZone167=previous.originZone167;held.lastAttack=previous.lastAttack;
   held.committedGestureH26=previous.committedGestureH26;held.solverCommitWorldH29=previous.solverCommitWorldH29;}
  return;
 }
 held.stroke118=previous.stroke118;held.gesture59c3=previous.gesture59c3;
 held.prepared=previous.prepared;held.stroke117=previous.stroke117;held.zone117=previous.zone117;
 held.origin167=previous.origin167;held.originZone167=previous.originZone167;held.lastAttack=previous.lastAttack;
 held.committedGestureH26=previous.committedGestureH26;held.solverCommitWorldH29=previous.solverCommitWorldH29;
 const double dt=double(a.sourceTick-previous.solverTickH28)*.001;
 melee118::Input sample{};sample.dt=dt;sample.eligible=in.allowed&&in.attacks&&in.coherentMelee119&&
  in.naturalCombat117&&in.tracked[0]&&in.intentTracked[0]&&dt>0&&dt<=.08;
 sample.modelTip=p::point(actual,weapon.tip)-p::point(previous.solverModelH28,weapon.tip);
 sample.tip=p::point(relative,weapon.tip)-p::point(previous.solverRelativeH28,weapon.tip);
 sample.modelHand=p::point(actual,weapon.grip[0].p)-p::point(previous.solverModelH28,weapon.grip[0].p);
 sample.hand=p::point(relative,weapon.grip[0].p)-p::point(previous.solverRelativeH28,weapon.grip[0].p);
 sample.modelAxis=m::rotate(actual.q,weapon.axis);sample.axis=m::rotate(relative.q,weapon.axis);
 sample.modelForward=p::body_axes(in).forward;sample.forward=m::rotate(trackingFromModel.q,sample.modelForward);
 sample.guard=out.guardBySide30c1[0];sample.known=false; // no collision is asserted by a pose
 const auto tuningH29=melee_tuning_c54::read(weapon.classId);
 melee118::Policy policy{};policy.cutSensitivityH29=tuningH29.cut*tuningH29.sensitivityH29;policy.stabSensitivityH29=tuningH29.stab*tuningH29.sensitivityH29;policy.intentSensitivityH29=tuningH29.intentH29;policy.deliberate119=true;policy.handRearm178=true;policy.responsive44c2=true;
 thrust_tuning_d67::apply(weapon.classId,policy);
 policy.bladeAxisCosineH41=melee_tuning_c54::bladeAxisH41();
 policy.forwardOnly19=in.forwardOnly19;policy.purposefulCuts52=in.forwardOnly19;policy.deliberateCut42=in.forwardOnly19;
 policy.telegraphStart59c10=in.forwardOnly19&&!in.intent59c16&&!held.prepared;
 policy.c54PostGuardH27=in.forwardOnly19&&!sample.guard;policy.heavy60=weapon.classId==7;
 const bool wasActive=held.stroke118.active;const auto result=melee118::update(held.stroke118,sample,policy);
 if(!wasActive&&held.stroke118.active){held.gesture59c3=a.sample;
  held.origin167=p::body_vector117(in,p::point(actual,weapon.tip)-p::body_center_r38(in));
  held.originZone167=held.origin167.z>.20?0:held.origin167.x<0?1:2;}
 out.melee118=result.observation;out.strokeActive119=held.stroke118.active||result.commit;
 out.primaryStrokeGesture19=out.strokeActive119?held.gesture59c3:0;
 const auto append=[&](p::CommandKind kind){const auto before=out.commandCount;
  out.add(kind,in,weapon,0,result.modelDirection,held.zone117,held.stroke117,held.originZone167,held.origin167);
  if(out.commandCount==before)return false;auto& command=out.commands[before];
  command.gesture59c3=held.gesture59c3;command.solverH28=proof;return true;};
 if(result.abort){if(held.prepared)append(p::CommandKind::Abort);held.prepared=false;held.committedGestureH26={};held.solverCommitWorldH29={};return;}
 const bool busy=in.physicalCombat177&&!in.intent59c16&&in.nativeAttackBusy178;
 const bool offer=in.intent59c16?result.commit:result.prepare||result.commit;
 if(offer&&!held.prepared&&!busy&&held.gesture59c3){
  held.stroke117=result.kind==melee118::Kind::Thrust?p::CommandKind::Thrust:p::CommandKind::Slash;
  held.zone117=p::stroke_zone117(held.stroke117,p::body_vector117(in,result.modelDirection));
  if(in.thrustAsSwing55&&held.stroke117==p::CommandKind::Thrust)held.zone117=stabStartZoneD84(held.origin167,held.originZone167); // D84 stab start
  held.prepared=append(p::CommandKind::Prepare);
 }
 if(result.commit&&held.prepared&&!busy&&held.gesture59c3){
  p::ImpactRecord178r30 effort{};effort.gesture59c3=held.gesture59c3;effort.kind=held.stroke117==p::CommandKind::Thrust?1:0;
  const auto inverseWorld=m::conjugate(sourceWorld.q);
  p::impact_effort19(effort,weapon,0,actual,m::rotate(inverseWorld,a.linear)*(1/frame.scale),m::rotate(inverseWorld,a.angular),frame.scale);
  if(!effort.effortKnown19||(in.strikeMinScaleGate59c14&&effort.effortScale19<in.strikeMinScale178)){append(p::CommandKind::Abort);held.prepared=false;return;}
  const auto before=out.commandCount;
  if(append(held.stroke117)){auto& command=out.commands[before];command.effortKnown19=true;
   command.effortScale19=effort.effortScale19;command.effortMass19=effort.effortMass19;
   held.lastAttack=in.tick;held.committedGestureH26={held.gesture59c3,in.tick,weapon.key,weapon.generation,in.bodyEpoch,in.epoch,
    result.modelDirection,held.stroke118.lastDirection,effort.kind,held.originZone167};
   held.committedGestureH26.goreNetTravelD56=result.observation.progress;
   held.committedGestureH26.goreFastTravelD56=result.observation.fastTravel;
   held.solverCommitWorldH29=m::rotate(sourceWorld.q,result.modelDirection);}
  held.prepared=false;
 }
}
}
