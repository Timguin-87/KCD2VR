// Actual native-trigger producer, including asynchronous phase changes.
#define KCD2VR_SMITH_HEIGHT_FIXTURE_ONLY_D62
#include "smith_native_height_d59r1_test.cpp"
int main(){
 {Driver d;d.raise();auto out=d.step(11,1.22f);
  ck(out.strokeReleased&&!out.requestRT&&out.reason==smith::Reason::NativeContinuation,"cancelled native raise immediately drops our held trigger");
  ck(!d.step(11,1.26f).requestRT,"cancelled raise cannot become a deferred stroke without a new quiet baseline");
  d.step(11,1.26f);d.step(11,1.26f);ck(d.step(11,1.30f).strokeStarted,"deliberate new gesture after cancellation can start normally");}
 {Driver d;d.raise();auto sample=d.frame;--sample.sequence;sample.timeNs-=20000000;sample.tick-=20;
  d.context.phase=11;const auto out=d.controller.step(d.context,sample,d.frame.tick,0,0,0);
  ck(out.strokeReleased&&!out.requestRT,"native cancellation revokes trigger even before a new XR sample");}
 {Driver d;d.prime();auto out=d.step(11,1.14f);ck(out.strokeStarted,"unacknowledged fixture began a legitimate gesture");
  for(unsigned i=0;i<22;++i)ck(d.step(11,1.14f).requestRT,"bounded original admission window retained");
  out=d.step(11,1.14f);ck(!out.requestRT&&out.strokeReleased&&out.reason==smith::Reason::PressNotAccepted,"native refusal cannot hold trigger indefinitely");
  ck(!d.step(11,1.18f).requestRT,"expired request cannot replay when hand continues moving");}
 {Driver d;d.raise();for(unsigned i=0;i<80;++i)ck(d.step(14,1.22f).requestRT,"acknowledged native preparation keeps full genuine hold duration");
  auto out=d.step(14,1.19f);ck(out.strokeReleased&&!out.requestRT,"real physical reversal still releases at existing 18 mm threshold");}
 {Driver d;d.prime();d.step(11,1.14f);
  for(unsigned i=0;i<21;++i)d.step(11,1.14f);
  ck(d.step(14,1.14f).requestRT,"native acknowledgement inside original window retains current hold");
  for(unsigned i=0;i<5;++i)ck(d.step(14,1.14f).requestRT,"acknowledgement prevents later false timeout");}
 continuousCycles();kcd2vr::crafting_d67::setSmithRecovery(false);freshLiftOnly();kcd2vr::crafting_d67::setSmithRecovery(true);unobservedOrForeignCycles();boundaryRetirement();nativeHoldPreserved();legacyClockUnchanged();
 std::printf("D62 smith native-height retirement: %u checks PASS; native phase acceptance and impact are not synthesized\n",checks);return 0;
}
