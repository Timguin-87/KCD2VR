#ifndef KCD2VR_INPUT67_HEADER
#define KCD2VR_INPUT67_HEADER "../../../src/subsystems/alchemy/input67.h"
#endif
#include KCD2VR_INPUT67_HEADER
#include <cassert>
#include <cstdio>
#include <limits>
namespace a=kcd2vr::subsystems::alchemy::input67;
namespace n=kcd2vr::subsystems::alchemy::near67;
namespace g=kcd2vr::subsystems::alchemy::gesture67;
namespace s=kcd2vr::alchemy_semi46;
using Frame=kcd2vr::workstation_sample46::Frame;
static unsigned checks=0;
static void checkAt(bool b,int line){++checks;if(!b)std::fprintf(stderr,"input67 check%u line%d FAILED\n",checks,line);assert(b);}
#define check(v) checkAt((v),__LINE__)
struct Fixture {
 a::State state{};a::Input in{};Frame f{};
 Fixture(){in.enabled=in.qualified=true;in.now=1000;in.modeEpoch=6;in.context={1,2,3,4,5,6,7};
  auto& t=in.nativeFrame.table;t.instance=1;t.player=2;t.playerEntity=9;t.lifetime=3;t.tick=1000;t.sequence=20;
  for(int k=0;k<21;++k)t.refs[k]={k,unsigned(100+k)};
  in.nativeFrame.offers[14]={{14,114},{0,0,0},1000,true};in.nativeFrame.offers[6]={{6,106},{.15,0,0},1000,true};
  for(int h=0;h<2;++h){in.palm[h]={{h?0.:2.,0,0},1000,4,5,6,h,0,true};
   in.world[h]={true,{float(h?0.:2.),0,0},1000,4,5};f.hand[h].valid=true;}
  f.sequence=10;f.tick=1000;f.timeNs=1000000000;f.poseEpoch=4;f.sourceEpoch=5;f.allowed=f.headValid=true;
  in.action={1,0,3,1000,true};}
 void advance(unsigned ms=10){in.now+=ms;f.tick=in.now;f.timeNs+=std::uint64_t(ms)*1000000;++f.sequence;
  auto& t=in.nativeFrame.table;t.tick=in.now;++t.sequence;
  for(auto& o:in.nativeFrame.offers)if(o.valid)o.tick=in.now;
  for(int h=0;h<2;++h){in.palm[h].tick=in.world[h].tick=in.now;if(in.tool[h].joined)in.tool[h].tick=in.now;}
  in.action.tick=in.now;if(in.work.parent)in.work.tick=in.now;if(in.nativeSelectedKnown)in.nativeSelected.tick=in.now;}
 void grip(int side,float value){in.palm[side].grip=f.hand[side].grip=value;}
 void selected(int type=14,bool hold=false){in.nativeSelectedKnown=true;in.nativeSelected={1,3,in.now,unsigned(100+type),unsigned(type),hold};}
 a::Output step(){return state.step(in,f);}
 void admit(int type=14,int side=1,bool hold=false){
  in.nativeFrame.offers[14].valid=false;in.nativeFrame.offers[type]={{type,unsigned(100+type)},{0,0,0},in.now,true};
  in.palm[side].point={0,0,0};in.palm[1-side].point={2,0,0};grip(side,1);step();advance();selected(type,hold);
  check(step().admitted);}
 void tool(int side,int type){in.tool[side]={1,0,888,3,in.now,4,unsigned(100+type),side,type,true};}
 void resource(int phase=1,bool working=true){in.action.parent=999;in.work={1,999,555,3,in.now,44,s::Resource,phase,2,working,false};tool(1,14);}
};
static void nativeResourceR17(Fixture& q,int type,int phase=0,bool attached=true){
 q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Resource,phase,2,phase==1,false};
 if(attached){q.tool(1,type);q.in.tool[1].parent=999;
  auto& b=q.in.tool[1].binding;b={1,2,888,777,unsigned(100+type),1,type,999,0x3A5C670,666,3};}
 else q.in.tool[1]={};
}
static void waitingResourceR17(Fixture& q,int type=15,bool hold=true){
 q.admit(type,1,hold);q.advance();nativeResourceR17(q,type,0,false);q.step();
 q.advance();q.grip(1,0);auto o=q.step();check(!o.use&&!o.control.held[1]&&!o.gesture.releaseRequested);
}
static void joinedWhileOpenR17(Fixture& q,int type=15){
 waitingResourceR17(q,type);q.advance();nativeResourceR17(q,type);auto o=q.step();
 check(!o.use&&!o.control.held[1]&&!o.gesture.releaseRequested&&o.gesture.totalSeconds==0);
}
static void runResourceRegripR17(bool corrected){
 // Actual wine order: release before Attach, close again before the current
 // entity_left publication. Native Resource phase0 remains non-working.
 for(int type=13;type<=16;++type)for(bool hold:{false,true}){
  Fixture q;waitingResourceR17(q,type,hold);q.advance();q.grip(1,1);
  auto o=q.step();check(!o.control.held[1]&&!o.use&&!o.gesture.releaseRequested);
  q.advance();nativeResourceR17(q,type);o=q.step();
  check(o.admitted&&o.control.held[1]==corrected&&!o.control.held[0]);
  check(!o.use&&!o.semiControl.manualUse&&!o.gesture.releaseRequested&&o.gesture.totalSeconds==0);
  check(o.gesture.status==(corrected?g::Status::NativeOwnerRequired:g::Status::GripRequired));
  for(int k=0;k<4;++k){o=q.step();check(!o.use&&o.control.held[1]==corrected&&o.gesture.totalSeconds==0);}
  // A genuine same-child interval is still required; regrip emits no A, recipe
  // effect, return request or clock credit. Only a later physical tilt works.
  q.advance();nativeResourceR17(q,type,1);o=q.step();check(!o.use&&o.gesture.totalSeconds==0);
  check(o.control.held[1]==corrected&&o.gesture.releaseRequested==!corrected);
  q.advance();q.f.hand[1].rotation[2]=q.f.hand[1].rotation[3]=.70710678f;o=q.step();
  check((o.gesture.totalSeconds>0)==corrected&&!o.use);
  const double amount=o.gesture.totalSeconds;o=q.step();check(o.gesture.totalSeconds==amount&&!o.use);
  q.advance();q.grip(1,0);o=q.step();check(!o.use&&!o.control.held[1]&&o.gesture.releaseRequested);
  q.advance();q.grip(1,1);o=q.step();check(!o.use&&!o.control.held[1]&&o.gesture.releaseRequested);
 }
 // Current tool, context, native parent, original side and real Grip are all
 // required. These mutations exercise the actual full State::step call path.
 for(int fault=0;fault<38;++fault){
  Fixture q;joinedWhileOpenR17(q);q.advance();q.grip(1,1);
  switch(fault){
  case 0:q.grip(1,.54f);break;
  case 1:q.grip(1,0);q.grip(0,1);break;
  case 2:q.f.hand[1].grip=.54f;break;
  case 3:q.in.tool[1].joined=false;break;
  case 4:q.in.tool[1].tick=q.in.now-81;break;
  case 5:q.in.tool[1].tick=q.in.now+1;break;
  case 6:q.in.tool[1].entity++;break;
  case 7:q.in.tool[1].side=0;break;
  case 8:q.in.tool[1].type=14;break;
  case 9:q.in.tool[1].parent++;break;
  case 10:q.in.tool[1].lifetime++;break;
  case 11:q.in.tool[1].poseEpoch++;break;
  case 12:q.in.tool[1].character=0;break;
  case 13:q.in.tool[1].binding.player++;break;
  case 14:q.in.tool[1].binding.instance++;break;
  case 15:q.in.tool[1].binding.character++;break;
  case 16:q.in.tool[1].binding.attachment++;break;
  case 17:q.in.tool[1].binding.object++;break;
  case 18:q.in.tool[1].binding.lifetime++;break;
  case 19:q.in.tool[1].binding.entity++;break;
  case 20:q.in.tool[1].binding.side=0;break;
  case 21:q.in.tool[1].binding.type++;break;
  case 22:q.in.tool[1].binding.action++;break;
  case 23:q.in.tool[1].binding.actionClass++;break;
  case 24:q.in.work.child++;break;
  case 25:q.in.work.token++;break;
  case 26:q.in.work.tick=q.in.now-81;break;
  case 27:q.in.work.parent++;break;
  case 28:q.in.work.child=0;break;
  case 29:q.in.work.token=0;break;
  case 30:q.in.action.parent++;break;
  case 31:q.in.action.valid=false;break;
  case 32:q.f.hand[1].valid=false;break;
  case 33:q.f.allowed=false;break;
  case 34:q.in.modeEpoch++;break;
  case 35:q.in.context.sourceEpoch++;break;
  case 36:q.in.nativeFrame.table.refs[15].entity++;break;
  case 37:q.in.enabled=false;break;
  }
  auto o=q.step();
  // Baseline consumes the old decision again after retiring a changed parent.
  if(fault==30&&!corrected){check(o.admitted&&o.control.held[1]&&o.use);continue;}
  if(o.control.held[0]||o.control.held[1]||o.use||o.gesture.totalSeconds!=0)std::fprintf(stderr,"FAULT=%d admitted=%d held=%d,%d use=%d status=%d\n",fault,o.admitted,o.control.held[0],o.control.held[1],o.use,int(o.gesture.status));
  check(!o.control.held[0]&&!o.control.held[1]&&!o.use&&o.gesture.totalSeconds==0);
 }
 // A remembered replacement, real effect/return, or previously requested
 // native cleanup cannot be undone by a subsequent apparently old phase0.
 for(int fault=0;fault<7;++fault){
  Fixture q;joinedWhileOpenR17(q);q.advance();
  const auto work=q.in.work;const auto tool=q.in.tool[1];
  switch(fault){
  case 0:q.in.work.child++;break;
  case 1:q.in.work.token++;break;
  case 2:q.in.tool[1].binding.object++;break;
  case 3:q.in.tool[1].binding.attachment++;break;
  case 4:q.in.work.phase=2;break;
  case 5:q.in.work.phase=1;q.in.work.working=true;break;
  case 6:q.in.nativeReturnAuthorized=true;break;
  }
  q.step();q.advance();q.in.work=work;q.in.work.tick=q.in.now;q.in.tool[1]=tool;q.in.tool[1].tick=q.in.now;
  q.in.nativeReturnAuthorized=false;q.grip(1,1);auto o=q.step();check(!o.control.held[1]&&!o.use&&o.gesture.totalSeconds==0);
 }
 // New observation of a replaced child before any Attach must also close
 // regrip, rather than forgetting the already observed native work identity.
 Fixture replaced;waitingResourceR17(replaced);replaced.advance();replaced.in.work.child++;
 replaced.step();replaced.advance();replaced.grip(1,1);nativeResourceR17(replaced,15);auto o=replaced.step();
 check(!o.control.held[1]&&!o.use&&o.gesture.totalSeconds==0);
 // Native A remains untouched passenger policy; no gesture/clock takeover.
 Fixture passenger;joinedWhileOpenR17(passenger);passenger.advance();passenger.grip(1,1);passenger.in.nativeA=true;
 o=passenger.step();check(o.passenger&&o.semiControl.manualUse&&!o.control.held[1]&&!o.use);
 // If the exact same phase0 tool is released and regripped again, continuity
 // alone permits another hand lease. No effect or return ever occurred.
 Fixture again;joinedWhileOpenR17(again);again.advance();again.grip(1,1);again.step();again.advance();again.grip(1,0);again.step();
 again.advance();again.grip(1,1);o=again.step();check(o.control.held[1]==corrected&&!o.use&&o.gesture.totalSeconds==0);
 // Phase0 is also observed after the genuine native Use marker. It must not
 // manufacture work credit or another press in that state. Actual Detach can
 // revoke the current binding immediately; the parent then ends normally.
 Fixture nativeUsed;joinedWhileOpenR17(nativeUsed);nativeUsed.advance();nativeUsed.grip(1,1);
 o=nativeUsed.step();check(o.control.held[1]==corrected&&!o.use&&o.gesture.totalSeconds==0&&!o.gesture.releaseRequested);
 nativeUsed.advance();nativeUsed.in.tool[1].joined=false;o=nativeUsed.step();
 check(!o.control.held[1]&&!o.use&&!o.gesture.releaseRequested&&o.gesture.totalSeconds==0);
 nativeUsed.advance();nativeUsed.in.action.parent=0;nativeUsed.in.work={};o=nativeUsed.step();
 check(corrected?(!o.admitted&&!o.use&&!o.control.held[1]):(o.admitted&&o.use&&!o.control.held[1]));
 // Changed-parent retirement may not reuse its prior decision on this sample.
 // The next genuinely open -> closed Grip remains able to request a new A.
 Fixture changed;joinedWhileOpenR17(changed);changed.advance();changed.grip(1,1);changed.in.action.parent=1000;
 o=changed.step();check(corrected?(!o.admitted&&!o.use):(o.admitted&&o.use));
 if(corrected){
  o=changed.step();check(!o.admitted&&!o.use);
  changed.advance();changed.grip(1,0);changed.in.action.parent=0;changed.in.work={};changed.in.tool[1]={};changed.step();
  changed.advance();changed.grip(1,1);changed.selected(15);o=changed.step();check(o.admitted&&o.use);
 }
 for(int fault=0;fault<6;++fault){Fixture q;joinedWhileOpenR17(q);q.advance();q.grip(1,1);
  switch(fault){case 0:q.f.tick=q.in.now-81;break;case 1:q.f.tick=q.in.now+1;break;
  case 2:q.in.palm[1].tick=q.in.now-81;break;case 3:q.in.palm[1].tick=q.in.now+1;break;
  case 4:q.f.sourceEpoch++;break;case 5:q.in.cancel=true;break;}
  o=q.step();check(!o.use&&!o.control.held[1]&&o.gesture.totalSeconds==0);
 }
}

#ifndef KCD2VR_INPUT67_NO_MAIN
int main(){
 Fixture z;auto o=z.step();check(!o.use&&!o.admitted&&o.nomination.decision.source==n::Source::Left);
 check(o.semiControl.enabled&&o.semiControl.manualUse&&!o.control.held[0]&&!o.control.held[1]);
 z.grip(1,1);o=z.step();check(!o.use&&!o.admitted&&o.nomination.decision.gripping);
 z.advance();z.selected();o=z.step();check(o.use&&o.admitted&&!o.control.held[1]&&!o.semiControl.manualUse);
 check(o.control.entity==114&&o.control.type==14&&o.control.side==1); // queue-kind witness, not a pose grant
 z.advance(81);o=z.step();check(!o.use&&o.admitted); // native release-hint pulse consumed once
 z.resource(0,false);o=z.step();check(o.control.held[1]&&!o.control.held[0]);
 check(o.gesture.status==g::Status::NativeOwnerRequired&&o.gesture.totalSeconds==0); // hold through native prepare
 z.advance();z.resource();o=z.step();check(o.gesture.enabled&&o.gesture.totalSeconds==0&&o.control.held[1]);
 z.advance();z.f.hand[1].rotation[2]=.70710678f;z.f.hand[1].rotation[3]=.70710678f;
 o=z.step();check(o.gesture.totalSeconds>0&&o.gesture.status==g::Status::Pouring);
 const double credit=o.gesture.totalSeconds;o=z.step();check(o.gesture.totalSeconds==credit); // no second-eye credit
 z.advance();z.grip(1,0);o=z.step();check(!o.use&&!o.control.held[1]&&o.gesture.releaseRequested);
 check(o.gesture.enabled&&o.gesture.status==g::Status::ReleaseAwaitingNativeReturn&&!o.semiControl.manualUse);
 z.advance();z.in.nativeReturnAuthorized=true;o=z.step();check(o.gesture.status==g::Status::NativeReturn&&o.semiControl.manualUse);
 // A real completed native use stays held until release. It is not an invented effect.
 Fixture completed;completed.admit();completed.resource();completed.step();completed.advance();completed.in.work.phase=2;
 o=completed.step();check(o.gesture.completedHeld&&o.control.held[1]&&!o.gesture.releaseRequested);
 completed.advance();completed.grip(1,0);o=completed.step();check(o.gesture.status==g::Status::NativeReturn&&!o.control.held[1]);
 // Unknown or contradictory native hold hints never fall through to Legacy Use.
 for(int fault=0;fault<6;++fault){Fixture t;t.grip(1,1);t.step();t.advance();t.selected();
  switch(fault){case 0:t.in.nativeSelectedKnown=false;break;case 1:++t.in.nativeSelected.instance;break;
   case 2:++t.in.nativeSelected.lifetime;break;case 3:++t.in.nativeSelected.entity;break;
   case 4:t.in.nativeSelected.type=21;break;case 5:t.in.nativeSelected.tick=t.in.now-81;break;}
  o=t.step();check(!o.use&&!o.admitted&&!o.control.held[1]);}
 Fixture hold;hold.admit(14,1,true);hold.advance(81);o=hold.step();check(o.use&&o.admitted);
 hold.in.nativeSelectedKnown=false;o=hold.step();check(!o.use); // held input cannot target an unknown selection
 hold.grip(1,0);o=hold.step();check(!o.use&&!o.control.held[1]);
 Fixture wrongHand;wrongHand.in.palm[0].point={0,0,0};wrongHand.in.palm[1].point={2,0,0};wrongHand.grip(0,1);
 wrongHand.step();wrongHand.advance();wrongHand.selected();o=wrongHand.step();check(!o.use&&!o.admitted&&wrongHand.state.gripTick==0);
 Fixture wrongMortar;wrongMortar.in.nativeFrame.offers[14].valid=false;wrongMortar.in.nativeFrame.offers[2]={{2,102},{0,0,0},1000,true};
 wrongMortar.grip(0,1);wrongMortar.in.palm[0].point={0,0,0};wrongMortar.in.palm[1].point={2,0,0};
 wrongMortar.step();wrongMortar.advance();wrongMortar.selected(2);o=wrongMortar.step();check(!o.use&&!o.admitted);
 // Actual tool identity is mandatory, not a nomination or convenient native parent.
 for(int fault=0;fault<10;++fault){Fixture t;t.admit();t.resource();
  switch(fault){case 0:t.in.tool[1].joined=false;break;case 1:t.in.tool[1].side=0;break;
   case 2:t.in.tool[1].type=13;break;case 3:++t.in.tool[1].entity;break;case 4:++t.in.tool[1].instance;break;
   case 5:++t.in.tool[1].lifetime;break;case 6:++t.in.tool[1].poseEpoch;break;case 7:t.in.tool[1].tick=t.in.now-81;break;
   case 8:t.in.tool[1].parent=123;break;case 9:t.in.tool[1].character=0;break;}
  o=t.step();check(!o.control.held[0]&&!o.control.held[1]&&o.gesture.totalSeconds==0);}
 Fixture foreign;foreign.admit();foreign.resource();foreign.in.work.kind=s::Mortar;
 o=foreign.step();check(o.semiControl.manualUse&&!o.control.held[1]);
 Fixture topology;topology.admit();topology.resource();check(topology.step().control.held[1]);topology.advance();
 ++topology.in.nativeFrame.table.refs[20].entity;o=topology.step();check(!o.admitted&&!o.use&&!o.control.held[1]);
 // Native parent end cannot replay the same still-held Grip admission.
 Fixture end;end.admit();end.resource();end.step();end.advance();end.in.action.parent=0;end.in.work={};
 o=end.step();check(!o.use&&!o.admitted&&!o.control.held[1]);end.advance();o=end.step();check(!o.use&&!o.admitted);
 end.grip(1,0);end.step();end.advance();end.grip(1,1);end.selected();o=end.step();check(o.admitted&&o.use);
 // Native A stays passenger through pending admission and exact native parent.
 Fixture pass;pass.in.nativeA=true;o=pass.step();check(o.passenger&&o.semiControl.manualUse);
 pass.advance();pass.in.nativeA=false;pass.in.action.parent=700;o=pass.step();check(o.passenger&&o.control.parent==700);
 pass.advance();pass.in.action.valid=false;o=pass.step();check(o.passenger); // missing is not ended
 pass.advance();pass.in.action.valid=true;o=pass.step();check(o.passenger); // book/page flag has no input here
 pass.advance();pass.in.action.parent=0;o=pass.step();check(!o.passenger&&!o.use);
 Fixture existing;existing.resource();existing.in.nativeA=true;o=existing.step();check(o.passenger&&o.control.parent==999);
 existing.advance();existing.in.nativeA=false;o=existing.step();check(o.passenger);
 existing.advance();existing.in.action.parent=1000;o=existing.step();check(!o.passenger); // previous real parent ended
 Fixture refused;refused.in.nativeA=true;o=refused.step();check(o.passenger);
 refused.advance(251);refused.in.nativeA=false;refused.in.action.valid=false;o=refused.step();check(o.passenger);
 refused.advance();refused.in.action.valid=true;refused.in.action.parent=0;o=refused.step();check(!o.passenger);
 // Mode/source/context loss retires controller ownership; held input cannot
 // regain admission until an actual valid open Grip sample is seen.
 for(int fault=0;fault<6;++fault){Fixture t;t.admit();t.resource();check(t.step().control.held[1]);
  switch(fault){case 0:t.in.enabled=false;break;case 1:t.in.qualified=false;break;case 2:t.in.cancel=true;break;
   case 3:t.in.modeEpoch++;break;case 4:t.f.allowed=false;break;case 5:t.in.palm[1].valid=false;break;}
  o=t.step();check(!o.use&&!o.control.held[1]);t.advance();t.in.enabled=t.in.qualified=t.f.allowed=true;t.in.cancel=false;
  t.in.modeEpoch=6;t.in.palm[1].valid=true;o=t.step();check(!o.admitted&&!o.use);}
 // Pot proof and world palms are not raw XR substitutes.
 Fixture geom;geom.admit();geom.resource();geom.step();geom.advance();geom.in.nativeFrame.offers[6].valid=false;
 geom.f.hand[1].rotation[2]=geom.f.hand[1].rotation[3]=.70710678f;o=geom.step();check(o.gesture.status==g::Status::GeometryRequired&&o.gesture.totalSeconds==0);
 Fixture mortar;mortar.admit(2,1);mortar.in.action.parent=999;mortar.in.work={1,999,555,3,mortar.in.now,44,s::Mortar,3,3,true,false};
 mortar.tool(0,3);mortar.tool(1,2);mortar.grip(0,1);mortar.in.world[0].point[0]=.02f;
 o=mortar.step();check(o.control.held[0]&&o.control.held[1]&&o.gesture.enabled);
 // Each physical hand keeps its own presentation. Releasing the mortar
 // requests native return, but does not revoke the still-gripped pestle.
 mortar.advance();mortar.grip(0,0);o=mortar.step();check(!o.control.held[0]&&o.control.held[1]&&o.gesture.releaseRequested);
 // An ordinary native held tool needs no special work child to follow its
 // controller. Ending the native Take action does not end real held membership.
 // Slot5 is a carried empty phial. Slot4 is the native retort.
 Fixture bowl;bowl.admit(5,0);bowl.tool(0,5);bowl.in.action.parent=700;
 o=bowl.step();check(o.ordinaryHeld&&o.control.held[0]&&o.semiControl.manualUse);
 bowl.advance(40);bowl.in.action.parent=0;o=bowl.step();check(o.admitted&&o.ordinaryHeld&&o.control.held[0]);
 bowl.advance(40);bowl.in.nativeFrame.offers[6].point={.05,0,0};o=bowl.step();
 check(o.nomination.decision.ref.slot==6&&!o.nomination.decision.gripping&&o.ordinaryHeld); // ray grip0 only, real Grip still holds
 bowl.advance();bowl.selected(6);bowl.grip(0,0);o=bowl.step();
 check(o.use&&o.passenger&&o.ordinaryReturnRequested&&!o.control.held[0]&&!o.gesture.releaseRequested);
 bowl.advance();bowl.in.action.parent=900;o=bowl.step();check(o.passenger&&!o.ordinaryReturnObserved);
 bowl.advance(81);bowl.in.action.parent=0;o=bowl.step();check(o.ordinaryReturnObserved&&!o.admitted&&!o.use);
 bowl.advance();o=bowl.step();check(!o.use&&!o.ordinaryReturnRequested); // no second Return pulse
 // Unknown/mismatched final native selection cannot turn release into Use.
 for(int fault=0;fault<3;++fault){Fixture t;t.admit(5,0);t.tool(0,5);t.step();t.advance(81);
  t.in.nativeFrame.offers[6].point={.05,0,0};t.selected(6);t.step();t.advance();
  if(fault==0)t.in.nativeSelectedKnown=false;else if(fault==1)t.selected(7);else t.in.nativeFrame.offers[6].point={.101,0,0};
  t.grip(0,0);o=t.step();check(!o.use&&!o.ordinaryReturnRequested&&!o.control.held[0]);
  t.advance();t.selected(6);t.in.nativeFrame.offers[6].point={.05,0,0};o=t.step();
  check(o.use==(fault<2)); // one pending physical release survives native observation lag; a distant release cannot retarget
 }
 // A known native source-A return context may be used without guessing its verb.
 Fixture own;own.admit(5,0);own.tool(0,5);own.step();own.advance(81);own.step();own.advance();own.grip(0,0);o=own.step();
 check(o.use&&o.ordinaryReturnRequested&&!o.control.held[0]);
 // If a held-hint admission still held A, insert one real release frame before
 // the one new contextual Return press, never two indistinguishable held Uses.
 Fixture edge;edge.admit(5,0,true);edge.tool(0,5);edge.step();edge.advance();edge.grip(0,0);o=edge.step();
 check(!o.use&&o.ordinaryReturnRequested);edge.advance();o=edge.step();check(o.use&&o.passenger);
 // An already tilted hand at a delayed native attachment keeps the original
 // physical grip's quaternion as its neutral, not the late attach orientation.
 Fixture reference;reference.admit();reference.f.hand[1].rotation[2]=reference.f.hand[1].rotation[3]=.70710678f;
 reference.resource();reference.step();check(reference.state.work.neutralUp[1]>.99f);
 reference.advance();o=reference.step();check(o.gesture.totalSeconds>0);
 // Release BEFORE the native Attach cannot leave a subsequently paused real
 // resource child waiting forever for an acquisition which can no longer occur.
 Fixture early;early.admit();early.advance();early.grip(1,0);o=early.step();
 check(!o.gesture.releaseRequested&&!o.use&&!o.control.held[1]); // no native action, no synthetic Return
 early.advance();early.resource();o=early.step();
 check(o.gesture.status==g::Status::ReleaseAwaitingNativeReturn&&o.gesture.releaseRequested&&o.gesture.enabled);
 check(o.gesture.parent==999&&o.gesture.child==555&&o.gesture.token==44&&o.gesture.totalSeconds==0&&!o.control.held[1]&&!o.use);
 early.advance();early.grip(1,1);o=early.step();check(o.gesture.releaseRequested&&!o.use&&o.gesture.totalSeconds==0); // no reacquire
 // Another hand near B must not return a remotely carried object to B.
 Fixture remote;remote.admit(5,0);remote.tool(0,5);remote.step();remote.advance(81);remote.step();remote.advance();
 remote.in.palm[0].point={1,0,0};remote.in.palm[1].point={0,0,0};remote.in.nativeFrame.offers[6].point={.05,0,0};
 remote.selected(6);remote.step();remote.advance();remote.grip(0,0);o=remote.step();
 check(!o.use&&!o.ordinaryReturnRequested&&!o.control.held[0]);
 // G1R1: actual copied native bounds, rather than the native gaze-dependent
 // sample point, determine both the initial grab and the carrying palm's Put.
 const auto bounds=[](n::Offer& offer,float x){
  offer.bounds={};offer.boundsValid=true;
  for(int j=0;j<3;++j){offer.bounds.local[j]=-.02f;offer.bounds.local[j+3]=.02f;}
  offer.bounds.world[0]=offer.bounds.world[5]=offer.bounds.world[10]=1.f;
  offer.bounds.world[3]=x;offer.point={50,50,50};
 };
 Fixture actual;bounds(actual.in.nativeFrame.offers[14],0.f);
 actual.grip(1,1);o=actual.step();check(o.nomination.decision.ref.slot==14&&!o.use);
 actual.advance();actual.selected();o=actual.step();check(o.admitted&&o.use);
 actual.resource();bounds(actual.in.nativeFrame.offers[6],.15f);actual.step();actual.advance();
 actual.f.hand[1].rotation[2]=actual.f.hand[1].rotation[3]=.70710678f;
 o=actual.step();check(o.gesture.totalSeconds>0&&o.gesture.status==g::Status::Pouring);
 const auto actualCredit=o.gesture.totalSeconds;
 actual.advance();actual.in.nativeFrame.offers[6].bounds.world[3]=1.f;
 actual.in.nativeFrame.offers[6].point={0,0,0}; // stale gaze point cannot authorize a remote pour
 o=actual.step();check(o.gesture.totalSeconds==actualCredit&&o.gesture.status==g::Status::MoveToPot);
 Fixture actualPut;actualPut.admit(5,0);actualPut.tool(0,5);actualPut.step();actualPut.advance(81);actualPut.step();
 bounds(actualPut.in.nativeFrame.offers[6],.05f);actualPut.advance();actualPut.selected(6);actualPut.step();
 actualPut.advance();actualPut.grip(0,0);o=actualPut.step();
 check(o.use&&o.ordinaryReturnRequested&&o.passenger);
 // A native gaze-selection that arrives after the hand has left10cm does not
 // turn an old physical latch into a remote grab/Use.
 Fixture remoteAdmit;remoteAdmit.grip(1,1);remoteAdmit.step();remoteAdmit.advance();
 remoteAdmit.in.palm[1].point={.3,0,0};remoteAdmit.selected();o=remoteAdmit.step();
 check(!o.admitted&&!o.use&&o.nomination.decision.gap>.1);
 // Mounted Bellows uses its actual native LEFT membership18, never a fake
 // NativeTool attach. Actual bounds and the same parent own a contact lease.
 const auto bellows=[](Fixture& q){q.admit(18,1,true);auto& t=q.in.nativeFrame.table;
  t.actionKnown=true;t.action=999;t.handTypesKnown68=true;t.handType68[1]=18;
  q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Bellows,2,2,true,false};
  auto& v=q.in.nativeFrame.offers[18];v.boundsValid=true;for(int j=0;j<3;++j){v.bounds.local[j]=-.02f;v.bounds.local[j+3]=.02f;v.bounds.world[j*4+j]=1;}
 };
 Fixture pump;bellows(pump);o=pump.step();check(o.gesture.enabled&&!o.control.held[0]&&!o.control.held[1]&&o.gesture.totalSeconds==0);
 pump.advance();pump.f.hand[1].position[1]=pump.in.world[1].point[2]=-.02f;o=pump.step();
 check(o.gesture.totalSeconds>0&&o.gesture.status==g::Status::Pumping&&!o.control.held[1]);
 const auto pumpCredit=o.gesture.totalSeconds;
 pump.advance();pump.in.nativeFrame.offers[18].valid=false;pump.f.hand[1].position[1]=pump.in.world[1].point[2]=-.04f;o=pump.step();
 check(o.gesture.enabled&&o.gesture.clockStop&&o.gesture.totalSeconds==pumpCredit);
 pump.advance();pump.grip(1,0);o=pump.step();check(o.semiControl.manualUse&&o.gesture.status==g::Status::NativeReturn&&!o.use);
 Fixture earlyPump;bellows(earlyPump);earlyPump.in.work={};earlyPump.in.action.parent=0;earlyPump.in.nativeFrame.table.action=0;
 earlyPump.grip(1,0);earlyPump.step();earlyPump.advance();earlyPump.in.action.parent=999;earlyPump.in.nativeFrame.table.action=999;
 earlyPump.in.work={1,999,555,3,earlyPump.in.now,44,s::Bellows,2,2,true,false};o=earlyPump.step();
 check(o.semiControl.manualUse&&o.gesture.status==g::Status::NativeReturn&&!o.use&&!o.control.held[1]);
 earlyPump.advance();earlyPump.grip(1,1);o=earlyPump.step();check(o.semiControl.manualUse&&!o.use&&o.gesture.totalSeconds==0); // no squeeze replay before real native return
 for(unsigned fault=0;fault<6;++fault){Fixture q;bellows(q);
  switch(fault){case 0:q.in.nativeFrame.table.handTypesKnown68=false;break;case 1:q.in.nativeFrame.table.handType68[1]=21;break;
  case 2:q.in.nativeFrame.table.actionKnown=false;break;case 3:q.in.nativeFrame.table.action++;break;
  case 4:q.in.nativeFrame.offers[18].boundsValid=false;break;case 5:q.in.nativeFrame.offers[18].bounds.world[3]=1;break;}
  o=q.step();check(o.gesture.totalSeconds==0&&o.gesture.clockStop&&!o.control.held[0]&&!o.control.held[1]);}
 runResourceRegripR17(true);
 std::printf("alchemy input67: %u checks PASS (native final selection, attachment and work remain required)\n",checks);
 return 0;
}

#endif
