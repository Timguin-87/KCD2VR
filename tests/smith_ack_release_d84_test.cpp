// Exercises the actual native-height producer and its production fall tracker.
// Phase observations are fixtures, not a claim of native game acceptance.
#define KCD2VR_SMITH_HEIGHT_FIXTURE_ONLY_D62
#include "smith_native_height_d59r1_test.cpp"

int main(){
 kcd2vr::crafting_d67::setSmithRecovery(true);
 Driver samePacket;samePacket.prime();
 auto out=samePacket.step(11,1.14f);
 ck(out.strokeStarted&&out.requestRT,"deliberate lift starts the exact native request");
 out=samePacket.step(14,1.11f);
 ck(out.strokeReleased&&!out.requestRT,"first observed native ACK and physical reversal release together");
 ck(out.clock.fallD81.request&&out.clock.fallD81.released&&out.clock.fallD81.phase==14,
    "same-packet native ACK and own reversal retain the native fall receipt");
 const auto request=out.clock.fallD81.request;
 out=samePacket.step(16,1.08f);
 ck(out.clock.fallD81.request==request,"native falling phase keeps the same request");
 ck(!samePacket.step(17,1.07f).clock.fallD81.request,"native completion ends that request");

 Driver unack;unack.prime();unack.step(11,1.14f);
 out=unack.step(11,1.11f);
 ck(out.strokeReleased&&!out.clock.fallD81.request,"release without native ACK cannot earn a fall receipt");
 ck(!unack.step(14,1.10f).clock.fallD81.request,"later native phase cannot revive an already released unacknowledged request");
 ck(!unack.step(16,1.08f).clock.fallD81.request,"unowned native fall remains native");

 Driver foreign;foreign.prime();
 ck(!foreign.step(14,1.14f).clock.fallD81.request,"native raise without our press cannot be adopted");
 ck(!foreign.step(14,1.11f).clock.fallD81.request,"foreign phase14 reversal cannot earn permission");

 for(int fault=0;fault<5;++fault){
  Driver d;d.prime();d.step(11,1.14f);
  switch(fault){case 0:++d.frame.poseEpoch;break;case 1:++d.frame.sourceEpoch;break;
   case 2:++d.context.lifetime;break;case 3:d.frame.hand[0].valid=false;break;
   case 4:d.frame.allowed=false;break;}
  ck(!d.step(14,1.11f).clock.fallD81.request,"owner or tracking loss at ACK/release cannot inherit the old stroke");
 }
 Driver manual;manual.prime();manual.step(11,1.14f);
 ck(!manual.step(14,1.11f,.8f).clock.fallD81.request,"manual RT takeover cannot borrow our same-packet release");
 std::printf("D84 smith ACK/release: %u checks PASS; native impact and headset outcomes NOT_RUN\n",checks);
}
