// Actual native-height producer driven by fixture tracking/phase observations.
// No synthetic impact, native callback or claimed game acceptance.
#define KCD2VR_SMITH_HEIGHT_FIXTURE_ONLY_D62
#include "smith_native_height_d59r1_test.cpp"

static sample::Frame lastPublished(const Driver& d){
 auto f=d.frame;--f.sequence;f.timeNs-=20000000;f.tick-=20;return f;
}
static void liftDuringOwnedRecovery(Driver& d){
 d.release();d.step(16,1.23f);d.step(17,1.27f);
}
static smith::Output duplicateIdle(Driver& d,unsigned ageMs=10){
 d.context.phase=11;
 const auto f=lastPublished(d);
 return d.controller.step(d.context,f,f.tick+ageMs,0,0,0);
}

int main(){
 kcd2vr::crafting_d67::setSmithRecovery(true);
 {
  Driver d;liftDuringOwnedRecovery(d);
  auto o=duplicateIdle(d);
  ck(!o.requestRT&&!o.strokeStarted,"duplicate native idle cannot issue a stroke");
  o=duplicateIdle(d,15);
  ck(!o.requestRT&&!o.strokeStarted,"another fresh duplicate still cannot issue a stroke");
  o=d.step(11,1.29f);
  ck(o.requestRT&&o.strokeStarted,"next fresh packet consumes retained own recovery lift");
  o=duplicateIdle(d);
  ck(o.requestRT&&!o.strokeStarted,"duplicate after consumption holds but never starts twice");
  o=d.step(14,1.29f);
  ck(o.requestRT&&!o.strokeStarted,"native acknowledgement retains exactly one RT transaction");
  o=d.step(14,1.26f);
  ck(!o.requestRT&&o.strokeReleased,"normal downward reversal still releases that one transaction");
 }
 {
  Driver d;liftDuringOwnedRecovery(d);duplicateIdle(d);
  auto o=d.step(11,1.25f);
  ck(!o.requestRT&&!o.strokeStarted,"fresh reversal cancels the retained pending lift");
  ck(!d.step(11,1.27f).requestRT,"cancelled pending lift cannot return on subthreshold upward travel");
  ck(d.step(11,1.29f).strokeStarted,"a later genuinely new lift can still begin");
 }
 {
  Driver d;liftDuringOwnedRecovery(d);
  for(int i=0;i<4;++i)d.step(17,1.27f);
  ck(!duplicateIdle(d).requestRT,"old recovery lift cannot survive duplicate native idle past its age limit");
  ck(!d.step(11,1.29f).requestRT,"expired lift is not restored by the next fresh sample");
 }
 for(int boundary=0;boundary<8;++boundary){
  Driver d;liftDuringOwnedRecovery(d);duplicateIdle(d);
  switch(boundary){
   case 0:++d.context.instance;break;
   case 1:++d.context.lifetime;break;
   case 2:++d.frame.poseEpoch;break;
   case 3:++d.frame.sourceEpoch;break;
   case 4:++kcd2vr::minigame_hands45::modeEpoch67;break;
   case 5:d.frame.hand[0].valid=false;break;
   case 6:d.context.enabled=false;break;
   case 7:kcd2vr::crafting_d67::setSmithRecovery(false);break;
  }
  const auto o=d.step(11,1.29f);
  ck(!o.requestRT&&!o.strokeStarted,"owner, mode, tracking or option loss cannot consume retained lift");
  kcd2vr::crafting_d67::setSmithRecovery(true);
 }
 {
  Driver d;liftDuringOwnedRecovery(d);duplicateIdle(d);
  auto o=d.step(11,1.29f,.8f);
  ck(!o.requestRT&&!o.strokeStarted,"manual trigger takeover revokes retained gesture lift");
 }
 {
  Driver d;liftDuringOwnedRecovery(d);duplicateIdle(d);
  d.context.phase=14;d.controller.step(d.context,lastPublished(d),d.frame.tick,0,0,0);
  ck(!d.step(11,1.29f).requestRT,"intervening foreign native raise revokes the retained cycle");
 }
 {
  Driver d;liftDuringOwnedRecovery(d);duplicateIdle(d);
  auto o=duplicateIdle(d,33);
  ck(!o.requestRT&&!o.strokeStarted,"tracking age over 32ms still revokes pending input");
  ck(!d.step(11,1.29f).requestRT,"freshness revocation is not revived by a later packet");
 }
 kcd2vr::crafting_d67::setSmithRecovery(false);
 {
  Driver d;liftDuringOwnedRecovery(d);duplicateIdle(d);
  ck(!d.step(11,1.29f).requestRT,"disabled D67 recovery preserves native-idle rebase behavior");
 }
 kcd2vr::crafting_d67::setSmithRecovery(true);
 std::printf("D67R1 smith recovery lifetime %u checks PASS; one owned lift, no synthetic impact\n",checks);
}
