#define KCD2VR_SMITH_HEIGHT_FIXTURE_ONLY_D62
#include "smith_native_height_d59r1_test.cpp"
int main(){
 kcd2vr::crafting_d67::setSmithRecovery(true);
 {Driver d;d.release();d.step(16,1.23f);d.step(17,1.27f);auto o=d.step(11,1.31f);ck(o.requestRT&&o.strokeStarted,"owned recovery lift survives native busy interval");auto copy=d.frame;--copy.sequence;copy.timeNs-=20000000;copy.tick-=20;auto dup=d.controller.step(d.context,copy,d.frame.tick,0,0,0);ck(!dup.strokeStarted,"recovery lift starts only once");}
 {Driver d;d.release();d.step(16,1.23f);d.step(17,1.27f);auto o=d.step(11,1.20f);ck(!o.requestRT,"reversed recovery lift is not queued");}
 {Driver d;d.raise();d.frame.hand[1].position[1]+=1;auto o=d.step(14,1.22f);ck(o.requestRT&&!o.strokeReleased,"left tool discontinuity cannot fire valid hammer");}
 {Driver d;d.raise();auto o=d.step(14,2.22f);ck(o.requestRT&&!o.strokeReleased&&o.reason==smith::Reason::DiscontinuityHeldD67,"one short hammer discontinuity preserves native raise without work");d.frame.tick+=81;d.frame.timeNs+=81000000;o=d.step(14,3.22f);ck(!o.requestRT,"sustained discontinuity never extends 80ms grace");}
 continuousCycles();unobservedOrForeignCycles();boundaryRetirement();nativeHoldPreserved();legacyClockUnchanged();
 std::printf("D67 smith recovery %u checks PASS; no synthetic hit/event\n",checks);
}
