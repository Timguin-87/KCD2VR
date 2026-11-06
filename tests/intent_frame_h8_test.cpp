// Host-only composition: actual tracked_frame51, a verbatim extracted ordinary
// foundation block and the complete physical kernel/normal recognizer. Upstream
// controller goals/orientations are synthetic calibrated poses; this does not
// execute the complete native skeleton/auto-palm adapter or native hit delivery.
#include "gesture_recovery_h30_fixture.h"
#include "vrik/tracked_frame51.h"
#include <cstdio>
namespace kcd2vr::station_ws_j27 {static bool captureD47(std::uint64_t){return false;}}
namespace diagnostic_trace {static void Write(const char*){}}
using V3=pm::Vec3;using Q4=pm::Quat;
static V3 Sub(V3 a,V3 b){return a-b;}static V3 Add(V3 a,V3 b){return a+b;}
static V3 VP(V3 a){return a;}static V3 PV(V3 a){return a;}static Q4 PQ(Q4 a){return a;}
struct FrameTxH8 {kcd2vr::tracked_frame51::State frameState51{};kcd2vr::tracked_frame51::Frame frame51{};};
struct ModelWorldH8 {V3 t{};Q4 q{};};
struct FrameRigH8 {
 RigH30 r{false};FrameTxH8 tx{};V3 base{},tracked{},anchor{},hmd{};int side=0;bool acquisitionClean=false;bool nativeStanceR93=false,nativeCrouchedR93=false;
 FrameRigH8(int hand=0):side(hand){r.in.hitTraceC54R3=true;
  if(side){r.profiles[1]=r.profiles[0];r.profiles[1].side=1;r.profiles[0]={};
   r.in.hands[1]=r.in.hands[0];r.in.intentHands[1]=r.in.intentHands[0];r.in.intentTracked[1]=true;r.in.grip[1]=1;}
  base=r.in.hands[side].p;r.state={};r.commands.clear();
  for(unsigned n=0;n<30;++n){step();if(n==0)acquisitionClean=r.hits()==0;}r.commands.clear();
 }
 void step(V3 controllerDelta={},V3 nativeDelta={},double angle=0,bool editGrip=false,V3 headDelta={}){
  tracked=tracked+controllerDelta;anchor=anchor+nativeDelta;hmd=hmd+headDelta;
  ++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;
  auto& pi=r.in;const Q4 frame=r.model;ModelWorldH8 frameWorld51{};
  kcd2vr::tracked_frame51::Input fi51{};fi51.stamp={pi.bodyEpoch,pi.sample,pi.tick,1};fi51.control=2;fi51.timeNs=pi.timeNs;
  fi51.nativePelvis=V3{0,0,.9}+anchor;fi51.nativeHead=V3{0,0,1.7}+anchor*.35;
  fi51.restHead={0,0,1.7};fi51.restPelvis={0,0,.9};fi51.calibratedHead={0,0,1.7};fi51.allowed=true;fi51.hmdHeight=true;fi51.nativeStanceR93=nativeStanceR93;fi51.nativeCrouchedR93=nativeCrouchedR93;
  V3 headAnchor123=fi51.nativeHead,headTarget=headAnchor123;
  // Production upstream equation, evaluated with synthetic valid calibration:
  // requested = headAnchor + R(controller - trackedBase).
  const V3 untouchedHeadOffset=fi51.nativeHead-fi51.restHead;
  V3 requestedGoals[2]{base+tracked+untouchedHeadOffset,base+tracked+untouchedHeadOffset};V3 goals[2]{requestedGoals[0],requestedGoals[1]};
// Ordinary-gameplay inputs: station movement is inactive; run the current adapter.
  kcd2vr::station_reposition_d53::Input moveD53{};
  kcd2vr::station_reposition_d53::Output movedD53{};
  struct {std::uint64_t sequence;} ts{pi.sample};
  V3 requestedGoalsRawH50[2]{requestedGoals[0],requestedGoals[1]};
#include "generated_frame_foundation_h8.inc"
#include "generated_frame_metadata_h8.inc"
  const Q4 rotation=pm::multiply(frame,Q4{0,0,std::sin(angle*.5),std::cos(angle*.5)});
  for(int n=0;n<2;++n){pi.hands[n]={rotation,goals[n]};pi.modelIntentHandsH30[n]={rotation,requestedGoals[n]};
   pi.modelIntentSourceH30[n]=pi.modelIntentTrackedH30[n]=true;pi.intentHands[n]={rotation,base+tracked-hmd};}
  if(editGrip){r.profiles[side].grip[side].p={.015,0,0};r.profiles[side].tip.x+=.025;}
  r.out=ph::step(r.state,pi,r.profiles,[](const ph::Profile&,ph::Pose,ph::Pose,bool){ph::Contact103 c{};c.known=true;return c;});
  for(unsigned n=0;n<r.out.commandCount;++n)r.commands.push_back(r.out.commands[n]);
 }
};
static void translationH8(int side,bool moving,bool backward=false,bool profileChange=false){
 FrameRigH8 t(side);for(unsigned n=0;n<12;++n)t.step({0,backward?-.035:.035,0},{0,moving?(backward?.024:-.024):0,0},0,profileChange&&n==4);
 const char* label=backward?"genuine backwards stroke remains windup":profileChange?"grip/mesh refresh preserves corrected stroke":
  side?"left controller stroke survives native anchor motion":moving?"right controller stroke survives native anchor motion":"stable-anchor air control commits once";
 std::printf("TRACE %s commits=%u guard=%d model=%.3f tracked=%.3f\n",label,t.r.hits(),t.r.out.guard117,t.r.out.hitTraceC54R3.modelTipSpeed,t.r.out.hitTraceC54R3.relativeTipSpeed);
 ckH30(t.r.hits()==(backward?0u:1u),label);
}
int main(){
 for(int cls:{4,10,14,15}){
  const int side=cls==4?0:1;FrameRigH8 t(side);t.r.profiles[side].classId=cls;
  t.nativeStanceR93=true;t.r.in.dynamics=true;t.r.in.controllerAnchor119=true;t.r.in.constraintDrive129=true;
  for(int n=0;n<20;++n)t.step();t.r.commands.clear();const auto before=t.r.out.weapons[side];
  for(bool crouch:{true,false}){
   t.nativeCrouchedR93=crouch;t.step();
   std::printf("CROUCH_KERNEL class=%d crouched=%d ready=%d owned=%d gap=%.6f vel=%.6f hits=%u reset=%u\n",cls,crouch,t.r.state.weapon[side].ready,t.r.out.owned[side],pm::length(t.r.out.hands[side].p-t.r.in.hands[side].p),pm::length(t.r.state.weapon[side].linearVelocity),t.r.hits(),t.r.out.resetH29[side].reason);
   ckH30(t.r.hits()==0&&t.r.out.owned[side]&&t.r.out.resetH29[side].reason==0,"native stance edge cannot fabricate attack or discard held weapon");
   ckH30(pm::length(t.r.out.hands[side].p-t.r.in.hands[side].p)<.002&&pm::length(t.r.state.weapon[side].linearVelocity)<1e-8,"shared native stance cannot add weapon compliance gap or velocity");
   ckH30(pm::angle(t.r.out.weapons[side].q,before.q)<1e-8,"native stance leaves held weapon aim orientation unchanged");
  }
  if(cls==4){for(int n=0;n<12;++n)t.step({0,.035,0});ckH30(t.r.hits()==1,"real weapon stroke remains accepted after native stance edges");}
 }

 translationH8(0,false);translationH8(0,true);translationH8(1,true);translationH8(0,true,true);translationH8(0,true,false,true);
 for(double native:{-.024,.024}){FrameRigH8 t;for(unsigned n=0;n<12;++n)t.step({},{0,native,0});ckH30(t.r.hits()==0,"native anchor alone cannot supply a controller attack");}
 for(bool moving:{false,true}){FrameRigH8 t;for(unsigned n=0;n<16;++n)t.step({},{0,moving?-.024:0,0},(n+1)*.07);
  ckH30(t.r.hits()==1,moving?"rotational tip cut survives native anchor motion":"rotational tip cut stable-anchor control");}
 for(double native:{-.024,0.,.024}){FrameRigH8 t;for(unsigned n=0;n<12;++n)t.step({0,.035,0},{0,native,0},0,false,{0,.035,0});
  ckH30(t.r.hits()==0,"common controller/HMD translation remains non-attack");}
#if KCD2VR_HAS_INTENT_FRAME_H8
 {
  FrameRigH8 air,moving;
  ckH30(air.acquisitionClean&&moving.acquisitionClean,"initial weapon acquisition seeds history without an attack");
  for(unsigned n=0;n<12;++n){air.step({0,.035,0});moving.step({0,.035,0},{0,-.024,0});
   if(n==0)ckH30(air.r.hits()==0&&moving.r.hits()==0,"first source increment cannot invent an immediate committed attack");}
  ckH30(air.r.first(ph::CommandKind::Prepare)==moving.r.first(ph::CommandKind::Prepare)&&
    air.r.first(ph::CommandKind::Slash)==moving.r.first(ph::CommandKind::Slash),"corrected native-frame stroke has the same prepare/commit source times as air control");
  const auto old=moving.r.state.weapon[0];auto duplicate=moving.r.in;duplicate.modelIntentFrameH8.anchor.y+=5;
  const auto out=ph::step(moving.r.state,duplicate,moving.r.profiles,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return ph::Contact103{};});
  const auto& now=moving.r.state.weapon[0];
  ckH30(out.commandCount==0&&now.sourceModelFrameH8.stamp.sample==old.sourceModelFrameH8.stamp.sample&&
    pm::length(now.sourceModelFrameH8.anchor-old.sourceModelFrameH8.anchor)<1e-12&&now.lastTick==old.lastTick,"duplicate XR source cannot advance frame ancestry or replay commands");
 }
 {
  FrameRigH8 t;t.step({0,.035,0},{0,-.024,0});
  auto& s=t.r.state.weapon[0];auto old=s;auto in=t.r.in;in.sample++;in.tick+=10;in.timeNs+=10000000;
  in.modelIntentFrameH8.stamp.sample=in.sample;in.modelIntentFrameH8.stamp.tick=in.tick;
  in.modelIntentFrameH8.anchor=in.modelIntentFrameH8.anchor+V3{.012,-.024,0};
  const V3 delta{.012,-.024,0};
  ph::rebase_model_intent_frame_h8(s,in,t.r.profiles[0],0);
  ckH30(pm::length(s.intentHand-old.intentHand-delta)<1e-12&&pm::length(s.intentTip-old.intentTip-delta)<1e-12,"only prior model hand/tip history translates");
  ckH30(pm::length(s.sourceOtherModelH30.p-old.sourceOtherModelH30.p-delta)<1e-12&&pm::length(s.supportIntentHand63-old.supportIntentHand63-delta)<1e-12,"other model/support history uses same translation");
  ckH30(pm::length(s.relativeIntentHand-old.relativeIntentHand)<1e-12&&pm::length(s.sourceTrackingH30.p-old.sourceTrackingH30.p)<1e-12,"relative controller history is unchanged");
  ckH30(pm::length(s.weapon.p-old.weapon.p)<1e-12&&pm::length(s.lastDesired.p-old.lastDesired.p)<1e-12&&sameImpactH30(s.impactRecord178r30,old.impactRecord178r30),"actual weapon and impact occurrence remain unchanged");
  ckH30(s.lastTick==old.lastTick&&s.timeNs==old.timeNs&&s.committedGestureH26.tick==old.committedGestureH26.tick&&pm::length(s.origin167-old.origin167)<1e-12,"original source/commit clocks and captured origin remain unchanged");
  for(unsigned change=0;change<13;++change){s=old;auto invalid=in;
   if(change==0)invalid.modelIntentFrameH8.stamp.actor++;
   if(change==1)invalid.modelIntentFrameH8.control++;
   if(change==2)invalid.modelIntentFrameH8.calibratedHead.x+=.01;
   if(change==3)invalid.modelIntentFrameH8.valid=false;
   if(change==4)invalid.modelIntentFrameH8.stamp.sample--;
   if(change==5)invalid.modelIntentFrameH8.acquisitionSerial=1;
   if(change==6)invalid.motionOwned59c5=true;
   if(change==7){invalid.tick+=100;invalid.modelIntentFrameH8.stamp.tick=invalid.tick;invalid.timeNs+=100000000;}
   if(change==8)invalid.modelIntentCalibrationH8={0,0,std::sin(.05),std::cos(.05)};
   if(change==9)invalid.modelIntentFrameH8.bow=true;
   if(change==10)invalid.modelIntentFrameH8.restOffset.x+=.01;
   if(change==11)invalid.modelIntentFrameH8.scale+=.1;
   if(change==12)invalid.bodyEpoch++;
   ph::rebase_model_intent_frame_h8(s,invalid,t.r.profiles[0],0);
   ckH30(pm::length(s.intentHand-old.intentHand)<1e-12,"changed owner/control/calibration/loss/clock or physical solver cannot borrow compensation");
  }
  ph::WeaponState restored{};ph::restore_normal_intent_h30(restored,old);
  ckH30(restored.sourceModelFrameH8.stamp.sample==old.sourceModelFrameH8.stamp.sample,"collision reset preserves exact source frame ancestry");
 }
#endif
 std::printf("SUMMARY checks=%u failures=%u\n",checksH30,failuresH30);return failuresH30?1:0;
}
