// The actual native-height producer; no synthetic impact or engine admission.
// The optional header override runs the same input sequence against D59.
#ifndef D59R1_SMITH_HEADER
#define D59R1_SMITH_HEADER "../src/vrik/semi_smith46.h"
#endif
#include D59R1_SMITH_HEADER
#include <cstdio>
#include <cstdlib>

namespace smith=kcd2vr::semi_smith46;
namespace sample=kcd2vr::workstation_sample46;
static unsigned checks=0;
static void ck(bool value,const char* why){
 ++checks;if(!value){std::fprintf(stderr,"FAIL: %s (check %u)\n",why,checks);std::exit(1);}
}
struct Driver {
 smith::Controller controller;
 smith::Context context{true,true,false,0x10000,7,11,true};
 sample::Frame frame{};
 Driver(){frame.allowed=frame.headValid=true;frame.poseEpoch=8;frame.sourceEpoch=9;
  frame.tick=1000;frame.timeNs=1000000000;frame.sequence=1;frame.head[1]=1.65f;
  for(auto& hand:frame.hand){hand.valid=true;hand.position[1]=1.1f;}}
 smith::Output step(int phase,float y,float manualRT=0){
  context.phase=phase;frame.hand[0].position[1]=y;
  auto out=controller.step(context,frame,frame.tick,0,0,manualRT);
  if(context.nativeHeight)ck(!out.clock.enabled&&!out.clock.request,"height mode never owns the legacy native clock");
  frame.tick+=20;frame.timeNs+=20000000;++frame.sequence;return out;
 }
 void prime(){ck(step(11,1.1f).reason==smith::Reason::Rebased,"station entry rebases");
  ck(!step(11,1.1f).requestRT,"one quiet sample is insufficient");
  ck(!step(11,1.1f).requestRT,"quiet arming does not press");}
 void raise(){prime();auto out=step(11,1.14f);
  ck(out.strokeStarted&&out.requestRT,"first deliberate lift starts native RT");
  ck(step(14,1.22f).requestRT,"native raise continues RT");}
 void release(){raise();auto out=step(14,1.19f);
  ck(out.strokeReleased&&!out.requestRT,"18 mm reversal releases before low point");}
 void fall(){release();ck(!step(16,1.13f).requestRT,"native fall has no gesture RT");
  ck(!step(17,1.10f).requestRT,"native completion has no gesture RT");}
};
static void continuousCycles(){
 Driver d;d.fall();ck(!d.step(11,1.11f).requestRT,"return to idle rebases without pressing");
 auto second=d.step(11,1.15f);
 ck(second.strokeStarted&&second.requestRT,"complete cycle permits continuous second lift");
 d.step(14,1.23f);auto release=d.step(14,1.20f);
 ck(release.strokeReleased&&!release.requestRT,"second physical reversal releases");
 d.step(16,1.14f);d.step(17,1.11f);d.step(11,1.12f);
 auto third=d.step(11,1.16f);
 ck(third.strokeStarted&&third.requestRT,"continuous third lift also starts");
}
static void freshLiftOnly(){
 Driver d;d.release();d.step(16,1.23f);d.step(17,1.27f);
 ck(!d.step(11,1.31f).requestRT,"lift while native busy is discarded at idle");
 auto held=d.frame;--held.sequence;held.timeNs-=20000000;held.tick-=20;
 auto duplicate=d.controller.step(d.context,held,d.frame.tick,0,0,0);
 ck(!duplicate.requestRT&&!duplicate.strokeStarted,"duplicate idle sample cannot replay busy movement");
 ck(!d.step(11,1.32f).requestRT,"subthreshold fresh lift stays idle");
 ck(d.step(11,1.35f).strokeStarted,"fresh post-idle 4 cm lift is eligible");
 Driver falling;falling.fall();falling.step(11,1.09f);
 ck(!falling.step(11,1.06f).requestRT,"further downward travel does not start a stroke");
 ck(falling.step(11,1.10f).strokeStarted,"fresh lift can begin at a lower new minimum");
 Driver autoComplete;autoComplete.raise();auto out=autoComplete.step(16,1.20f);
 ck(out.strokeReleased&&!out.requestRT,"native autonomous completion releases held trigger");
 autoComplete.step(17,1.14f);autoComplete.step(11,1.12f);
 ck(autoComplete.step(11,1.16f).strokeStarted,"observed native completion permits next fresh lift");
}
static void unobservedOrForeignCycles(){
 Driver foreign;foreign.prime();foreign.step(14,1.14f);foreign.step(16,1.18f);
 foreign.step(17,1.22f);foreign.step(11,1.26f);
 ck(!foreign.step(11,1.30f).requestRT,"foreign native cycle does not bypass quiet arming");
 Driver noFall;noFall.release();noFall.step(11,1.23f);
 ck(!noFall.step(11,1.27f).requestRT,"raise then idle without observed fall does not rearm");
 Driver noRaise;noRaise.prime();noRaise.step(11,1.14f);noRaise.step(11,1.11f);
 noRaise.step(16,1.07f);noRaise.step(17,1.03f);noRaise.step(11,1.07f);
 ck(!noRaise.step(11,1.11f).requestRT,"unacknowledged RT cannot claim a later fall");
 Driver restarted;restarted.fall();restarted.step(14,1.14f);restarted.step(16,1.18f);
 restarted.step(17,1.22f);restarted.step(11,1.26f);
 ck(!restarted.step(11,1.30f).requestRT,"intervening native raise revokes completed own cycle");
}
static void boundaryRetirement(){
 for(int boundary=0;boundary<8;++boundary){
  Driver d;d.fall();
  switch(boundary){
   case 0:++d.frame.poseEpoch;break;
   case 1:++d.frame.sourceEpoch;break;
   case 2:++d.context.lifetime;break;
   case 3:++d.context.instance;break;
   case 4:++kcd2vr::minigame_hands45::modeEpoch67;break;
   case 5:d.frame.hand[0].valid=false;d.step(17,1.11f);d.frame.hand[0].valid=true;break;
   case 6:d.step(17,1.11f,.8f);d.step(11,1.15f);break;
   case 7:d.context.enabled=false;d.step(17,1.11f);d.context.enabled=true;break;
  }
  d.step(11,1.19f);
  ck(!d.step(11,1.23f).requestRT,"identity, mode, tracking, manual or station loss revokes rapid rearm");
 }
 Driver stale;stale.fall();
 auto out=stale.controller.step(stale.context,stale.frame,stale.frame.tick+33,0,0,0);
 ck(!out.requestRT,"stale sample cannot hold a stroke");
 stale.step(11,1.14f);ck(!stale.step(11,1.18f).requestRT,"stale sampling revokes rapid rearm");
}
static void nativeHoldPreserved(){
 Driver d;d.raise();
 for(unsigned i=0;i<8;++i)ck(d.step(14,1.22f).requestRT,"stationary preparation keeps native RT held");
 ck(d.step(14,1.21f).requestRT,"small downward jitter does not release");
 auto out=d.step(14,1.19f);ck(!out.requestRT&&out.strokeReleased,"deliberate reversal releases once");
 ck(!d.step(14,1.19f).strokeReleased,"same reversal does not repeat release");
 Driver entry;entry.step(11,1.10f);entry.step(11,1.14f);
 ck(!entry.step(11,1.18f).requestRT,"initial moving entry still needs quiet baseline");
}
static void legacyClockUnchanged(){
 Driver d;d.context.nativeHeight=false;d.context.clockAvailable=true;d.prime();
 auto out=d.step(11,1.14f);const auto request=out.clock.request;
 ck(out.requestRT&&out.clock.enabled&&request&&out.clock.requestedPhase==14,"explicit legacy mode still requests its own raise clock");
 const auto ack=[&](int phase){smith::clockStatus.publish({d.context.instance,d.context.lifetime,request,d.frame.tick,phase,true});};
 ack(14);out=d.step(14,1.18f);const auto raised=out.clock.raiseWork;
 ck(out.requestRT&&out.clock.request==request&&raised>0,"legacy acknowledged raise earns displacement work");
 ack(14);out=d.step(14,1.18f);ck(out.clock.raiseWork==raised,"legacy stationary hand earns no extra work");
 ack(14);out=d.step(14,1.15f);
 ck(out.strokeReleased&&!out.requestRT&&out.clock.enabled&&out.clock.requestedPhase==16,"legacy reversal requests its own finish clock");
 const auto down=out.clock.finishWork;ack(16);out=d.step(16,1.11f);
 ck(out.clock.request==request&&out.clock.finishWork>down,"legacy fall consumes only current downward displacement");
 out=d.step(17,1.10f);ck(!out.clock.enabled&&!out.requestRT,"legacy native completion retires its lease");
 smith::clockStatus.publish({});
}
#ifndef KCD2VR_SMITH_HEIGHT_FIXTURE_ONLY_D62
int main(){kcd2vr::crafting_d67::setSmithRecovery(false);continuousCycles();freshLiftOnly();unobservedOrForeignCycles();boundaryRetirement();nativeHoldPreserved();legacyClockUnchanged();
 std::printf("D59R1 native-height smithing: %u checks PASS; continuous cycles rearm, native impact remains unmodified\n",checks);
 return 0;}
#endif
