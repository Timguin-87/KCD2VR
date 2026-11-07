// Actual gesture producer; the legacy displacement clock remains disabled.
#define KCD2VR_SMITH_HEIGHT_FIXTURE_ONLY_D62
#include "smith_native_height_d59r1_test.cpp"
#include <limits>
namespace fall=kcd2vr::subsystems::smithing::fall_d81;
int main(){
 kcd2vr::crafting_d67::setSmithRecovery(true);
 Driver d;d.release();auto out=d.step(16,1.15f);const auto r=out.clock.fallD81;
 ck(r.request&&r.released&&r.phase==16&&fall::fresh(r,r.tick),"observed raise/reversal grants exactly one native fall");
 ck(!out.clock.enabled&&!out.clock.request,"new receipt cannot activate the legacy charge clock");
 out=d.step(16,1.12f);ck(out.clock.fallD81.request==r.request,"same fall retains its occurrence");
 out=d.step(17,1.10f);ck(!out.clock.fallD81.request,"native completion revokes timing permission");
 for(int fault=0;fault<8;++fault){Driver x;x.release();
  switch(fault){case 0:++x.frame.poseEpoch;break;case 1:++x.frame.sourceEpoch;break;
   case 2:++x.context.lifetime;break;case 3:x.frame.hand[0].valid=false;break;
   case 4:x.frame.allowed=false;break;case 5:++kcd2vr::minigame_hands45::modeEpoch67;break;
   case 6:x.context.nativeHeight=false;break;case 7:x.frame.timeNs+=90000000;break;}
  ck(!x.step(16,1.15f).clock.fallD81.request,"identity, tracking and mode loss revoke fall timing");}
 Driver manual;manual.raise();ck(!manual.step(16,1.15f,.8f).clock.fallD81.request,"manual RT cannot borrow a physical release");
 Driver automatic;automatic.raise();ck(!automatic.step(16,1.15f).clock.fallD81.request,"native autonomous fall has no physical reversal receipt");
 Driver unack;unack.prime();unack.step(11,1.14f);unack.step(11,1.10f);
 ck(!unack.step(16,1.08f).clock.fallD81.request,"unacknowledged raise cannot claim a later native fall");
 Driver foreign;foreign.prime();foreign.step(14,1.14f);
 ck(!foreign.step(16,1.10f).clock.fallD81.request,"foreign native raise/fall stays native");
 ck(fall::rate(.01f,1.f)==4.f,"native fall4x at normal frame time");
 ck(std::fabs(fall::rate(.033f,1.f)*.033f-.08f)<1e-6f,"long frame cannot skip over80ms authored time");
 ck(fall::rate(.2f,1.f)==1.f&&fall::rate(.01f,0.f)==1.f&&fall::rate(.01f,std::numeric_limits<float>::quiet_NaN())==1.f,"invalid frame uses native rate");
 std::printf("D81 actual smith gesture/fall timing: %u checks PASS; native impact unexecuted\n",checks);
}
