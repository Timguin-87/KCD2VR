#include <string>
#define main legacyTouchChecksDs1
#include "native_touch_h9_test.cpp"
#undef main
static void lightFixture(){fixture();lab::livingHitImpact=false;lab::livingTouch=true;}
#ifndef DS1_TEST_MAIN
#define DS1_TEST_MAIN main
#endif
int DS1_TEST_MAIN(){
 const int legacy=legacyTouchChecksDs1();if(legacy)return legacy;
 namespace policy=kcd2vr::lab_living_touch_ds;
 lightFixture();QueryState q{false,true,77};
 {tr::ScopedNpcQueryH9 scope(q,false);check(q.requested&&!q.prepared,"LivingTouch-only selects existing NPC hand query");}
 check(!q.requested&&q.prepared&&q.preparedOwner==77,"LivingTouch query restores outer native owner");
 auto touch=capsule(0,.5,-1,.9f);check(touch.grip==.9f,"actual committed publisher retains original Grip value");
 fp::Dispatch();check(capsuleCalls==1&&impactCalls==0,"LivingTouch-only native consumer accepts deliberate Grip contact with no invented limb");
 check(std::fabs(capsulePacket.impulse[2]+.1f)<1e-6,"actual native packet uses admitted bounded light impulse");
 fp::Dispatch();check(capsuleCalls==1,"same source sample never replays");
 auto newer=touch;newer.sample.sequence=++serial;newer.sample.tick=now+20;now+=20;send(newer);fp::Dispatch();
 check(capsuleCalls==1,"production entity cooldown limits repeated contacts");
 for(int kind=0;kind<12;++kind){lightFixture();auto e=capsule(0,.5,-1,.9f);
  switch(kind){
   case 0:e.grip=std::numeric_limits<float>::infinity();break;/* W5: grip 0 is the open-hand touch now, checked below */case 1:e.grip=std::numeric_limits<float>::quiet_NaN();break;
   case 2:e.sample.motion.incoming=.1;break;case 3:e.sample.motion.incoming=6.1;break;
   case 4:e.sample.tick=now-81;break;case 5:e.sample.tick=now+1;break;
   case 6:e.fist=true;break;case 7:e.sample.request=999;break;
   case 8:e.sample.worldCoherent=false;break;case 9:lab::rung=2;break;
   case 10:e.body=kcd2vr::npc_contact172::Body::Physical;break;
   case 11:e.sample.requested.z+=.6;break;
  }
  send(e);fp::Dispatch();check(capsuleCalls==0&&impactCalls==0,"invalid grip/motion/owner/rung/body/distance cannot submit light impulse");
 }
 {lightFixture();auto open=capsule(0,.5,-1,0.f);send(open);fp::Dispatch(); // W5: open moving hand on a living capsule = half-strength shove
  check(capsuleCalls==1&&impactCalls==0,"W5 open-hand touch submits the bounded light capsule impulse");
  check(std::fabs(capsulePacket.impulse[2]+.05f)<1e-6,"W5 open-hand touch is half the grip push");}
 {namespace policy=kcd2vr::lab_living_touch_ds;policy::Evidence y{};y.entity=9;y.bodyKind=int(kcd2vr::npc_contact172::Body::Living);y.speed=1;y.dist=.1;y.tick=y.now=1000;
  const auto r=policy::admit(y);check(r.push&&std::fabs(r.k-policy::impulseK(1)*.5)<1e-12&&std::string(r.why)=="OPEN_HAND_TOUCH","W5 open-hand policy");
  y.gripHeld=true;const auto g=policy::admit(y);check(g.push&&std::fabs(g.k-policy::impulseK(1))<1e-12,"W5 grip push keeps full strength");
  y.gripHeld=false;y.speed=.2;check(!policy::admit(y).push,"W5 slow open hand still refused (TOO_SLOW)");}
 lightFixture();capsule(0,5.9,-1,.9f);fp::Dispatch();check(capsuleCalls==1&&std::fabs(capsulePacket.impulse[2]+.6f)<1e-6,"light impulse is capped at0.6 in actual native packet");
 lightFixture();auto old=publish();fp::Dispatch();check(impactCalls==0&&capsuleCalls==0,"LivingTouch does not turn unqualified articulated contact into a soft pendulum");
 policy::Evidence x{};x.entity=456;x.part=-1;x.bodyKind=int(kcd2vr::npc_contact172::Body::Living);x.speed=1;x.dist=.1;x.gripHeld=true;x.tick=1000;x.now=1000;
 check(policy::admit(x).push,"policy uses actual Living enum2 and accepts capsule sentinel");
 x.bodyKind=int(kcd2vr::npc_contact172::Body::Physical);check(!policy::admit(x).push,"physical base enum1 is not living");
 x.bodyKind=int(kcd2vr::npc_contact172::Body::Living);x.henry=true;check(!policy::admit(x).push,"policy refuses Henry");
 policy::Budget b;for(unsigned i=1;i<=6;++i)check(b.admit(i,1000),"bounded global budget accepts six distinct initial contacts");
 check(!b.admit(7,1000),"seventh global contact refused");check(!b.admit(1,1100),"same entity cooldown authoritative");
 check(b.admit(1,2000),"budget recovers after source window");
 std::printf("living_touch_ds1: %u/%u assertions passed (legacy included)\n",checks-failed,checks);return failed?1:0;
}
