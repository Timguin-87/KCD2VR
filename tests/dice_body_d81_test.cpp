#include "../src/vrik/dice_body_d81.h"
#include <cmath>
#include <cstdio>
namespace ae=kcd2vr::activity_eye59c19;
namespace db=kcd2vr::dice_body_d81;
namespace ds=kcd2vr::dialogue_state59c10r2;
namespace ai=kcd2vr::activity_input163;
static unsigned checks=0,failures=0;
static void check(bool good,const char* name){++checks;if(!good){++failures;std::printf("FAIL %s\n",name);}}
int main(){
 auto& w=ai::witness175;w.Clear();w.ready=true;
 const auto life=w.lifetime175.load();
 ds::NativeHead head{100,7,0x100,0x200,{10,20,1.70f},{},1,{10,20,0},true};
 ds::Eye eye{};eye.position={10,20.05f,1.756f};
 ae::State idle{};idle.tick=100;idle.lifetime=life;idle.entity=head.entity;idle.known=true;idle.neutral=true;
 ae::Neutral neutral{};check(ae::learn(idle,head,eye,100,neutral),"pre-entry native eye and head acquired together");
 check(neutral.headKnownD81&&std::abs(neutral.headLocalD81.z-1.7f)<.00001f,"neutral contains actual native model-local head");
 w.Begin(ai::Kind175::Dice,0x400,110);
 auto active=idle;active.tick=110;active.lifetime=w.lifetime175.load();active.enteredAt=110;
 active.kindMask=32;active.active=true;active.neutral=false;active.eligible=ae::eligibleKindsD81(32);head.tick=110;
 ae::Anchor anchor{};check(ae::latch(active,head,neutral,110,anchor)==ae::Reason::Applied,"exact dice lifecycle latches same camera/body anatomy");
 db::publish(active,head,anchor,110,true);db::Lease body{};
 check(db::read(body,110)&&body.instance==0x400&&body.character==head.character&&body.bodyEpoch==7,"dice body gets exact current camera-owned lease");
 check(std::abs(body.headLocal.y)<.00001f&&std::abs(body.headLocal.z-1.7f)<.00001f,"first dice body matches original head anchor");
 // Native inspect animation moves forward/down while Henry's entity translates
 // and turns. Both camera and body retain the same model-local anatomy; neither
 // adopts the animation's new head translation. HMD translation composes later.
 head.tick=120;active.tick=120;head.modelOrigin={30,40,0};head.world={30,40.8f,1.2f};
 const float q=std::sqrt(.5f);head.body={0,0,q,q};ds::Eye fixed{};
 check(ae::resolveCurrent(active,head,true,anchor,120,fixed)==ae::Reason::Applied,"current native inspection owner keeps anchored camera");
 db::publish(active,head,anchor,120,true);check(db::read(body,120),"inspection keeps exact body lease");
 const auto fixedHead=ds::add(head.modelOrigin,ds::rotate(head.body,ds::mul(body.headLocal,head.scale)));
 check(std::abs(fixedHead.x-30)<.0001f&&std::abs(fixedHead.y-40)<.0001f&&std::abs(fixedHead.z-1.7f)<.00001f,"body head ignores80cm native inspection lean");
 check(std::abs(fixed.position.x-fixedHead.x+.05f)<.0001f&&std::abs(fixed.position.z-fixedHead.z-.056f)<.0001f,"camera and VRIK head preserve measured eye/head separation under body turn");
 const auto captured=body;
 check(db::current(captured,200)&&!db::current(captured,201),"copied camera permission expires after80ms without being refreshed by reader");
 for(unsigned mismatch=0;mismatch<6;++mismatch){
  auto h=head;auto s=active;auto a=anchor;
  if(mismatch==0)h.character++;
  if(mismatch==1)h.scale=1.1f;
  if(mismatch==2)s.lifetime++;
  if(mismatch==3)s.kindMask=33;
  if(mismatch==4)a.eye.headKnownD81=false;
  if(mismatch==5)s.entity++;
  db::publish(s,h,a,120,true);check(!db::read(body,120),"body/lifetime/mixed-owner/missing-head mismatch refuses dice lease");
 }
 db::publish(active,head,anchor,120,false);check(!db::read(body,120),"camera refusal cannot grant body owner");
 db::publish(active,head,anchor,120,true);w.transitionDepth175=1;
 check(!db::read(body,120),"concurrent native start/stop transition revokes body");w.transitionDepth175=0;
 w.End(0x400);check(!db::current(captured,120),"native dice exit retires body immediately");
 w.Begin(ai::Kind175::Dice,0x400,125);check(!db::current(captured,125),"same-address new dice game cannot reuse old lifetime");
 check(ai::witness175.dice.load()==0x400,"presentation never changes native dice lifecycle or result");
 std::printf("D81 dice shared eye/head %u/%u; actual portable policy, no game/headset execution\n",checks-failures,checks);
 return failures?1:0;
}
