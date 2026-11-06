#include "gesture_recovery_h30_fixture.h"
int main(){
 {using namespace kcd2vr::user175;
  const auto actual=default59(Mode::Calibrated,true);
  ckH30(actual==Mode::CombatSynchronized,"actual public default/F4first HYBRID mode resolves to CombatSynchronized");
  RigH30 r(false);
  ckH30(ph::normal_hybrid_intent_h31(r.in)&&ph::normal_intent_enabled_h30(r.in)&&!r.in.intent59c16,"actual HYBRID mode enables automatic normal producer while legacy R16 direct-contact route remains OFF");
  RigH30 direct(true);ckH30(!ph::normal_hybrid_intent_h31(direct.in)&&!direct.in.intent59c16,"DIRECT comparison does not inherit normal-hybrid policy or legacy-R16 intent flag");
  auto off=r.in;off.physicalCombat177=configuration(Mode::Off).physical177;ph::select_forward_motion36(off,false,false);
  ckH30(!ph::normal_hybrid_intent_h31(off)&&!off.intent59c16,"OFF removes structural normal-hybrid policy without switching legacy route");
  auto f9=r.out;f9.solverOwnedT4[0]=true;f9.normalIntentH30[0]={};f9.solverH28={};
  ckH30(!ph::command_authority_h30(r.in,f9,r.profiles[0],0),"F9 cannot borrow raw normal-hybrid owner; exact typed achieved-body proof remains mandatory");
 }
 {RigH30 r(false);r.in.intent59c16=false;moveH30(r,14);
  ckH30(r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Slash)==1,"control: unchanged real physical kernel qualifies one normal forward cut");}
 for(unsigned duration:{1u,3u,6u}){
  RigH30 r(false);r.in.intent59c16=false;moveH30(r,10);
  const auto* prepare=firstH30(r,ph::CommandKind::Prepare);const auto gesture=prepare?prepare->gesture59c3:0;const auto origin=prepare?prepare->origin167:ph::Vec3{};
  ckH30(gesture&&r.state.weapon[0].prepared&&r.state.weapon[0].stroke118.active&&!r.hits(),"recovery starts after one qualified preparation with live stroke");
  gapH30(r,duration);
  ckH30(r.out.handlingOnly55c1[0]&&r.out.releaseReason121[0]==5,"fixture executes actual suspended UNKNOWN handling-recovery branch");
  ckH30(r.count(ph::CommandKind::Prepare)==1&&!r.count(ph::CommandKind::Abort),"fresh tracked intent is not aborted by temporary collision backend recovery");
  ckH30(!r.out.impactRecord178r30[0].valid,"UNKNOWN recovery creates no impact or target evidence");
  ckH30(!ph::attack_authority55c1(r.in,r.out,0),"handling recovery never gains checked collision authority");
  const auto& owner=r.out.normalIntentH30[0];
  ckH30(owner.current&&owner.key==r.profiles[0].key&&owner.generation==r.profiles[0].generation&&owner.bodyEpoch==r.in.bodyEpoch&&owner.request==r.in.epoch&&owner.sample==r.in.sample&&owner.tick==r.in.tick,"normal intent owner carries current tracked source provenance during checked-pose recovery");
  restoreH30(r,6);
  const auto* commit=firstH30(r,ph::CommandKind::Slash);
  ckH30(r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Slash)==1&&!r.count(ph::CommandKind::Abort),"one continuous normal swing commits once across UNKNOWN and checked reacquisition");
  ckH30(commit&&commit->gesture59c3==gesture&&pm::length(commit->origin167-origin)<1e-9,"committed swing retains pre-recovery gesture and immutable origin");
  r.blocked=r.actor=true;r.step({0,.025,0});const auto impact=r.out.impactRecord178r30[0];
  ckH30(impact.valid&&impact.gesture59c3==gesture&&impact.tick==r.in.tick&&impact.sample==r.in.sample&&impact.entityId==13&&impact.collider==2,"later actual checked NPC contact inherits same physical gesture and actual occurrence clocks");
 }
 {RigH30 r(false);r.in.intent59c16=false;r.in.reach[0]=.7;moveH30(r,10);
  const auto* prepare=firstH30(r,ph::CommandKind::Prepare);const auto gesture=prepare?prepare->gesture59c3:0;const auto origin=prepare?prepare->origin167:ph::Vec3{};
  ckH30(gesture&&r.state.weapon[0].prepared,"reach recovery begins with same normally qualified preparation at fixed anatomical reach");
  r.blocked=true;r.in.shoulders[0].y-=.10;r.step({0,.025,0});
  ckH30(r.out.handlingOnly55c1[0]&&r.out.releaseReason121[0]==4&&!r.out.contactKnown[0],"fixture reaches known blocked final-wrist recovery without tracking loss or unknown geometry");
  r.step({0,.025,0});
  ckH30(r.out.handlingOnly55c1[0]&&r.out.releaseReason121[0]==3&&!r.out.contactKnown[0],"fixture reaches known unreachable seed recovery on next sample");
  ckH30(!r.count(ph::CommandKind::Abort)&&r.out.normalIntentH30[0].current,"known reach/seed recovery preserves current deliberate intent without inventing a collision owner");
  ckH30(!ph::attack_authority55c1(r.in,r.out,0)&&!r.out.impactRecord178r30[0].valid,"blocked unreachable presentation never supplies NPC contact evidence");
  r.blocked=false;moveH30(r,6);const auto* commit=firstH30(r,ph::CommandKind::Slash);
  ckH30(r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Slash)==1&&commit&&commit->gesture59c3==gesture&&pm::length(commit->origin167-origin)<1e-9,"one normal swing retains gesture/origin through known wrist and seed recovery, then checked reacquisition");
  ckH30(r.in.reach[0]==.7,"recovery fix never extends anatomical reach to rescue gesture");
 } {RigH30 r(false);r.in.intent59c16=false;moveH30(r,14);const auto* commit=firstH30(r,ph::CommandKind::Slash);const auto gesture=commit?commit->gesture59c3:0;const auto commitTick=commit?commit->tick:0;
  r.blocked=r.actor=true;r.step({0,.025,0});const auto original=r.out.impactRecord178r30[0];
  ckH30(original.valid&&original.gesture59c3==gesture&&original.tick==r.in.tick&&original.sample==r.in.sample,"checked actor contact captured before recovery");
  gapH30(r);
  ckH30(r.state.weapon[0].committedGestureH26.gesture==gesture&&r.state.weapon[0].committedGestureH26.tick==commitTick,"recovery retains original committed gesture timestamp without renewing it");
  ckH30(!r.out.impactRecord178r30[0].valid,"UNKNOWN recovery withholds collision output while retaining original checked event separately");
  ckH30(sameImpactH30(r.state.weapon[0].impactRecord178r30,original)&&std::memcmp(&r.state.weapon[0].impactRecord178r30,&original,sizeof original)==0,"STATE retains full byte-identical original checked impact through soft recovery");
  ckH30(r.in.tick-original.tick==10&&!ph::attack_authority55c1(r.in,r.out,0),"10ms original checked contact does not grant current UNKNOWN collision authority");
  ckH30(r.count(ph::CommandKind::Slash)==1,"recovery cannot replay attack commit/native cost request");
  r.blocked=r.actor=false;restoreH30(r,1);
  ckH30(sameImpactH30(r.state.weapon[0].impactRecord178r30,original)&&std::memcmp(&r.state.weapon[0].impactRecord178r30,&original,sizeof original)==0,"known clear reacquisition retains byte-identical original impact without retagging the old occurrence");
  ckH30(sameImpactH30(r.out.impactRecord178r30[0],original)&&std::memcmp(&r.out.impactRecord178r30[0],&original,sizeof original)==0,"known clear reacquisition republishes exact original occurrence for slower consumer, never a fabricated new contact");
  ckH30(r.out.impactRecord178r30[0].tick<r.in.tick&&r.out.impactRecord178r30[0].sample<r.in.sample,"republished original occurrence clocks remain older than current frame");
 }
 for(unsigned loss=0;loss<8;++loss){
  RigH30 r(false);r.in.intent59c16=false;moveH30(r,10);gapH30(r);
  if(loss==0)r.in.allowed=false;
  if(loss==1)r.in.tracked[0]=false;
  if(loss==2)++r.profiles[0].generation;
  if(loss==3)r.in.grip[0]=0;
  if(loss==4)r.in.intentTracked[0]=false;
  if(loss==5)r.in.attacks=false;
  if(loss==6){r.in.tick+=90;r.in.timeNs+=90000000;}
  if(loss==7)r.in.contactFaultH30=true;
  r.step({0,.025,0});
  ckH30(!r.state.weapon[0].prepared&&!r.state.weapon[0].stroke118.active&&!r.state.weapon[0].committedGestureH26.gesture&&!r.hits(),"real context/tracking/item/grip/intent/attack/source-clock/circuit-fault loss cancels normal intent instead of surviving recovery");
  ckH30(!r.out.normalIntentH30[0].current,"real hard source loss revokes current normal-intent provenance");
 }
 for(unsigned loss=0;loss<8;++loss){
  RigH30 r(false);r.in.intent59c16=false;moveH30(r,14);r.blocked=r.actor=true;r.step({0,.025,0});
  ckH30(r.out.impactRecord178r30[0].valid,"hard-loss contact negative begins with actual checked actor occurrence");
  gapH30(r);
  if(loss==0)r.in.allowed=false;
  if(loss==1)r.in.tracked[0]=false;
  if(loss==2)++r.profiles[0].generation;
  if(loss==3)r.in.grip[0]=0;
  if(loss==4)r.in.intentTracked[0]=false;
  if(loss==5)r.in.attacks=false;
  if(loss==6){r.in.tick+=90;r.in.timeNs+=90000000;}
  if(loss==7)r.in.contactFaultH30=true;
  r.step({0,.025,0});
  ckH30(!r.state.weapon[0].impactRecord178r30.valid&&!r.out.impactRecord178r30[0].valid&&!r.state.weapon[0].committedGestureH26.gesture&&!r.out.normalIntentH30[0].current,"hard current source loss retires retained original contact and committed owner, not merely its output");
  ckH30(r.count(ph::CommandKind::Slash)==1,"hard loss cannot replay committed attack from retained contact");
 } {RigH30 r(false);r.in.intent59c16=false;moveH30(r,10);gapH30(r);r.known=true;r.in.contactSuspended59c1=false;for(unsigned i=0;i<6;++i)r.step({0,-.025,0});
  ckH30(!r.hits(),"recovery cannot turn backward withdrawal into committed forward attack");}
 for(bool stab:{false,true}){
  RigH30 r(false,stab?ph::Vec3{0,1,0}:ph::Vec3{1,0,0});r.in.intent59c16=false;
  r.in.strikeMinScaleGate59c14=true;r.in.strikeMinScale178=.05;r.in.dynamics=true;
  for(unsigned i=0;i<25&&!r.count(ph::CommandKind::Prepare);++i)r.step({0,.025,0});
  const auto* first=firstH30(r,ph::CommandKind::Prepare);const auto gesture=first?first->gesture59c3:0;const auto origin=first?first->origin167:ph::Vec3{};
  ckH30(gesture&&!r.hits(),"energy gate .05: actual 2.5m/s cut/stab qualifies preparation before commit in normal hybrid");
  gapH30(r,12);
  const auto kind=stab?ph::CommandKind::Thrust:ph::CommandKind::Slash;const auto* release=firstH30(r,kind);
  ckH30(release&&release->gesture59c3==gesture&&!release->effortKnown19&&pm::length(release->origin167-origin)<1e-9,"energy gate .05: raw tracked recovery commits qualified cut/stab with original gesture, no invented achieved-impact effort");
  ckH30(!r.out.impactRecord178r30[0].valid&&!ph::attack_authority55c1(r.in,r.out,0),"energy gate .05: raw motion qualification never creates UNKNOWN collision evidence");
  restoreH30(r,2);r.blocked=r.actor=true;r.step({0,.025,0});const auto impact=r.out.impactRecord178r30[0];
  ckH30(r.count(kind)==1&&r.count(ph::CommandKind::Prepare)==1&&!r.count(ph::CommandKind::Abort),"energy gate .05: checked reacquisition never aborts or duplicates qualified cut/stab");
  ckH30(impact.valid&&impact.gesture59c3==gesture&&impact.tick==r.in.tick&&impact.sample==r.in.sample&&impact.effortKnown19&&std::isfinite(impact.effortScale19)&&impact.effortMass19>0,"energy gate .05: only later actual checked NPC impact publishes achieved-body effort with original gesture and current occurrence");
 } {RigH30 r(false);r.in.intent59c16=false;moveH30(r,14);
  const auto original=r.state.weapon[0].committedGestureH26;const auto* prepare=firstH30(r,ph::CommandKind::Prepare);const auto origin=prepare?prepare->origin167:ph::Vec3{};
  r.profiles[0].grip[0].q={0,0,std::sin(.9),std::cos(.9)};r.step({0,.025,0});
  ckH30(original.gesture&&r.state.weapon[0].committedGestureH26.gesture==original.gesture&&r.state.weapon[0].committedGestureH26.tick==original.tick,"native grip refresh after commit retains original gesture and immutable commit clock");
  ckH30(r.out.normalIntentH30[0].current&&r.count(ph::CommandKind::Prepare)==1&&r.count(ph::CommandKind::Slash)==1&&!r.count(ph::CommandKind::Abort),"native mount quaternion refresh with unchanged tracked input family never duplicates or aborts committed attack");
  const auto* same=firstH30(r,ph::CommandKind::Prepare);ckH30(same&&pm::length(same->origin167-origin)<1e-9,"native grip refresh preserves actual stroke origin");
 }
 {RigH30 r(false);r.in.intent59c16=false;moveH30(r,10);const auto* prepare=firstH30(r,ph::CommandKind::Prepare);const auto gesture=prepare?prepare->gesture59c3:0;
  r.profiles[0].grip[0].q={0,0,std::sin(.9),std::cos(.9)};r.step({0,.025,0});
  ckH30(gesture&&r.out.normalIntentH30[0].current&&r.state.weapon[0].gesture59c3==gesture,"native grip refresh during preparation keeps current source owner and original candidate identity");
  ckH30(!r.hits(),"native grip refresh alone cannot provide missing physical travel/energy to commit preparation");
 }
 {RigH30 r(false);r.in.intent59c16=false;
  r.profiles[0].grip[0].q={0,0,std::sin(.9),std::cos(.9)};for(unsigned i=0;i<5;++i)r.step();
  ckH30(!r.count(ph::CommandKind::Prepare)&&!r.hits(),"stationary tracked wrists cannot attack from native mount refresh angular jump");
 } for(bool stab:{false,true})for(double yaw:{0.,1.2}){
  RigH30 r(false,stab?ph::Vec3{0,1,0}:ph::Vec3{1,0,0},yaw,yaw+.4);r.in.intent59c16=false;
  r.in.modelIntentSourceH30[0]=r.in.modelIntentTrackedH30[0]=true;r.in.modelIntentHandsH30[0]=r.in.hands[0];
  r.in.strikeMinScaleGate59c14=true;r.in.strikeMinScale178=.05;const auto capped=r.in.hands[0];const auto weapon=r.out.weapons[0];
  for(unsigned i=0;i<20;++i){r.in.modelIntentHandsH30[0].p=r.in.modelIntentHandsH30[0].p+pm::rotate(r.model,{0,.026,0});r.step({0,.026,0},false,true);}
  const auto kind=stab?ph::CommandKind::Thrust:ph::CommandKind::Slash;const auto* prepare=firstH30(r,ph::CommandKind::Prepare);const auto* commit=firstH30(r,kind);
  ckH30(prepare&&commit&&r.count(ph::CommandKind::Prepare)==1&&r.count(kind)==1&&!r.count(ph::CommandKind::Abort),"pre-reach source: current2.6m/s raw model+tracking cut/stab qualifies once while anatomical hand remains capped, gate.05 enabled");
  ckH30(prepare&&commit&&prepare->gesture59c3==commit->gesture59c3&&prepare->gesture59c3&&pm::length(prepare->origin167-commit->origin167)<1e-9,"pre-reach source preserves same gesture and actual origin through capped presentation");
  ckH30(pm::length(r.in.hands[0].p-capped.p)<1e-12&&pm::length(r.out.weapons[0].p-weapon.p)<1e-9&&!r.out.impactRecord178r30[0].valid,"pre-reach intent never extends anatomical hand, physical weapon or invents checked NPC collision");
  ckH30(r.out.normalIntentH30[0].current,"pre-reach raw source emits current explicit normal command owner in rotated calibration frames");
 }
 for(unsigned negative=0;negative<5;++negative){
  RigH30 r(false);r.in.intent59c16=false;r.in.modelIntentSourceH30[0]=r.in.modelIntentTrackedH30[0]=true;r.in.modelIntentHandsH30[0]=r.in.hands[0];
  if(negative==3)r.in.modelIntentTrackedH30[0]=false;
  for(unsigned i=0;i<20;++i){
   if(negative==0||negative==3)r.in.modelIntentHandsH30[0].p.y+=.025;
   if(negative==2){r.in.haveWorld=true;r.in.world.p.x+=.025;}
   if(negative==4)r.in.modelIntentHandsH30[0].p.y-=.025;
   r.step(negative==4?ph::Vec3{0,-.025,0}:ph::Vec3{0,.025,0},false,negative==1||negative==3||negative==4);
  }
  ckH30(!r.count(ph::CommandKind::Prepare)&&!r.hits(),"pre-reach source: model-only, tracking-only, common world translation, invalid published model tracking and backward windup cannot manufacture an attack");
  if(negative==3)ckH30(!r.out.normalIntentH30[0].current,"published invalid raw model source fails closed instead of falling back to capped hand");
 } {RigH30 r(false);r.in.intent59c16=false;moveH30(r,10);
  r.in.tick+=90;r.in.timeNs+=90000000;r.step({0,.025,0});
  ckH30(r.out.contactKnown[0]&&ph::attack_authority55c1(r.in,r.out,0)&&!r.out.normalIntentH30[0].current,"hard100ms source gap leaves known checked presentation but no current tracked intent proof");
  ckH30(!ph::command_authority_h30(r.in,r.out,r.profiles[0],0),"known anatomical pose alone cannot restore normal semantic command authority after hard source-clock loss");
  ckH30(!r.state.weapon[0].prepared&&!r.state.weapon[0].committedGestureH26.gesture&&!r.hits(),"hard source gap retires actual normal preparation without replaying attack commit");
 }
 // Preserve the real native counter-window handoff under the public producer;
 // this tags an input ancestry and never fabricates a master-strike result.
 for(bool validWindow:{false,true}){
  RigH30 r(false);r.in.masterHandoff23=true;r.in.masterHandoffSpeed23=3.5;
  r.in.masterWindowSerial23=7;r.in.masterWindowKey23=r.profiles[0].key+(validWindow?0:1);
  r.in.masterWindowGeneration23=r.profiles[0].generation;r.in.masterWindowBody23=r.in.bodyEpoch;r.in.masterWindowRequest23=r.in.epoch;
  bool handed=false;for(unsigned i=0;i<14;++i){r.step({0,.04,0});handed|=r.out.masterHandoff23.serial==7;}
  unsigned tagged=0;for(const auto& c:r.commands)tagged+=c.kind==ph::CommandKind::Prepare&&c.masterHandoff23==7;
  ckH30(validWindow?(handed&&tagged==1):(!handed&&!tagged),"public HYBRID: only an exact current native counter-window owner tags one prepare; wrong owner cannot force a master strike");
 }
 {RigH30 r(false);r.in.windupHold57=r.in.thrustAsSwing55=true;
  r.in.hands[0].p={.3,-.06,1.25};r.in.intentHands[0]=r.in.hands[0];
  for(unsigned i=0;i<60;++i)r.step();r.commands.clear();
  r.in.trigger[0]=1;r.step();for(unsigned i=0;i<40;++i)r.step();
  ckH30(r.count(ph::CommandKind::Prepare)==1&&!r.hits()&&r.state.weapon[0].cocked57.controlledH26,"public HYBRID: explicit held Trigger prepares once while a stationary hold cannot attack");
  for(unsigned i=0;i<60;++i)r.step({-.01,0,0});for(unsigned i=0;i<34;++i)r.step();
  unsigned redirects=0;for(const auto& c:r.commands)redirects+=c.kind==ph::CommandKind::Zone57&&c.cutFeint62&&c.controlledHoldH26;
  ckH30(redirects==1&&r.count(ph::CommandKind::Prepare)==1&&!r.hits(),"public HYBRID: deliberate held cross-zone feint redirects same prepare once without native attack release");
  // Stay strictly above the accepted2.5m/s cut floor, rather than on its
  // floating-point boundary (the shared helper moves at exactly2.5m/s).
  for(unsigned i=0;i<20;++i)r.step({0,.03,0});unsigned heldRelease=0;for(const auto& c:r.commands)heldRelease+=(c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust)&&c.held57;
  ckH30(heldRelease==1&&r.count(ph::CommandKind::Prepare)==1,"public HYBRID: actual forward stroke releases held preparation once without duplicate native input");
 }
 std::printf("gesture_recovery_h30=%u/%u native_calls=NONE forced_outcomes=NONE\n",checksH30-failuresH30,checksH30);return failuresH30?1:0;
}
