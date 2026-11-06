// Actual physical kernel plus exact extracted native-release preflight. Native
// execution is NONE; the native charge acknowledgement is an explicit input.
#include "vrik/physical_weapon.h"
#include <cstdio>
namespace ph=kcd2vr::physical;namespace m=kcd2vr::pose_core;
struct PhysNativeOwner6698 {bool bowHeld=false,physicalBow175=false,bowChargeObserved114=false;
 std::uint64_t key=0,bowGeneration175=0,request=0,epoch=0,bowBeginSample175=0;};
struct PhysFrame6698 {ph::Input input{};std::array<ph::Profile,2> profiles{};std::uint64_t request=0;};
#include "bow_release63.inc"
static unsigned checks=0,failed=0;
static void ck(bool yes,const char* name){++checks;if(!yes)++failed;std::printf("%s %s\n",yes?"PASS":"FAIL",name);}
struct Bow63 {
 ph::Input i{};ph::State s{};std::array<ph::Profile,2> p{};ph::Output o{};ph::Command release{};
 unsigned begin=0,fire=0,cancel=0;std::uint64_t beginSample=0;double maxFraction=0;
 Bow63(double lateral,double rest=.15){
  i.epoch=3;i.bodyEpoch=4;i.allowed=i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
  i.bowActions=i.bowRegrab116=i.bowRetention=true;i.reach[0]=i.reach[1]=1.1;
  i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.hands[1].p={0,.5,1.3};
  i.hands[0].p={lateral,.5-rest,1.3};i.grip[1]=1;
  auto&b=p[1];b.key=7;b.generation=8;b.side=1;b.classId=9;b.primary=b.support=b.geometry=true;
  b.bowNockQualified40=true;b.grip[0].p={0,-rest,0};b.axis={0,-1,0};b.tip={0,0,.6};b.bowDrawLength166=.55;
  ph::select_bow_policy175(i,b,true,true,true,false,false,true);
 }
 void next(){++i.sample;i.tick=1000+10*i.sample;i.timeNs=i.tick*1000000;
  i.intentHands[0]=i.hands[0];i.intentHands[1]=i.hands[1];
  o=ph::step(s,i,p,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return ph::Contact103{};});
  maxFraction=std::max(maxFraction,o.bowFraction);
  for(unsigned n=0;n<o.commandCount;++n){const auto& c=o.commands[n];
   if(c.kind==ph::CommandKind::BowBegin){++begin;beginSample=c.sample;}
   if(c.kind==ph::CommandKind::BowFire){++fire;release=c;}
   if(c.kind==ph::CommandKind::BowCancel)++cancel;}
 }
 void latch(){for(int n=0;n<8;++n)next();i.grip[0]=1;for(int n=0;n<12;++n)next();}
 void letGo(){i.grip[0]=0;next();}
 bool nativeReady(bool ack){PhysNativeOwner6698 n{};n.bowHeld=n.physicalBow175=true;n.bowChargeObserved114=ack;
  n.key=7;n.bowGeneration175=8;n.request=3;n.epoch=4;n.bowBeginSample175=beginSample;
  PhysFrame6698 f{};f.request=3;f.input=i;f.profiles=p;
  return PhysBowReleaseReady175(n,release,f,i.tick,true,true);}
};
int main(){
 for(double rest:{.05,.15,.30})for(double lateral:{0.,.05,.10,.15,.159}){
  Bow63 b(lateral,rest);b.latch();b.letGo();
  ck(b.begin==1&&b.fire==0&&b.cancel==1,"allowed stationary off-axis latch and release cancels, never fires");
  ck(b.maxFraction<1e-8,"initial acquisition/steering offset never charges the bow");
 }
 {Bow63 b(.15);b.latch();const auto target=b.i.hands[0].p-b.i.hands[1].p;
  for(int n=0;n<20;++n){b.i.hands[0].p=b.i.hands[1].p+m::rotate(m::from_rotation_vector({0,0,.005*(n+1)}),target);b.next();}
  b.letGo();ck(b.fire==0&&b.cancel==1&&b.maxFraction<1e-8,"constant-span lateral aiming after latch is not string pull");}
 {Bow63 b(.10);b.latch();for(int n=0;n<20;++n){
   const ph::Pose common{m::from_rotation_vector({0,0,.01}),{.001,0,0}};
   b.i.hands[0]=ph::compose(common,b.i.hands[0]);b.i.hands[1]=ph::compose(common,b.i.hands[1]);b.next();}
  b.letGo();ck(b.fire==0&&b.cancel==1&&b.maxFraction<1e-8,"rigid common hand translation and rotation cannot manufacture draw");}
 {Bow63 b(0);b.latch();for(int n=0;n<15;++n){b.i.hands[0].p.y-=.02;b.next();}
  ck(std::abs(b.o.bowFraction-.30/.55)<1e-8,"normal axial hand pull retains its exact physical fraction");
  const auto aim=b.o.weapons[1].q;b.letGo();
  ck(b.fire==1&&b.cancel==0&&b.nativeReady(true)&&!b.nativeReady(false),"real post-latch pull fires once only with current native charge ACK");
  ck(m::angle(b.o.weapons[1].q,aim)<1e-7,"release preserves coupled geometric aim");
  b.next();ck(b.fire==1,"release cannot replay the native fire request");}
 {Bow63 b(0);b.latch();for(int n=0;n<15;++n){b.i.hands[1].p.y+=.02;b.next();}
  ck(std::abs(b.o.bowFraction-.30/.55)<1e-8,"pushing the bow away with the left hand is equivalent axial draw");}
 {Bow63 b(.05);b.latch();for(int n=0;n<15;++n){b.i.hands[0].p.y-=.02;b.next();}
  b.i.tracked[0]=false;b.letGo();ck(b.fire==0&&b.cancel==1,"real draw followed by tracking loss still cancels");}
 {Bow63 b(.15);b.latch();++b.i.bodyEpoch;b.next();b.letGo();ck(b.fire==0,"recenter cannot inherit another body epoch's draw");}
 for(double rest:{.05,.15,.30})for(double lateral:{0.,.10,.15,.159}){
  Bow63 b(lateral,rest);b.latch();const auto begin=b.o.weapons[1];
  for(int n=1;n<=20;++n){b.i.hands[1].q=m::from_rotation_vector({0,0,.04*n});b.next();}
  b.letGo();ck(b.fire==0&&b.cancel==1&&b.maxFraction<1e-8,"fixed hand positions plus primary-wrist orientation cannot manufacture draw or fire");
  ck(m::length(ph::point(b.o.weapons[1],b.p[1].grip[1].p)-b.i.hands[1].p)<1e-8&&
     (rest<.15||(m::angle(begin.q,b.o.weapons[1].q)<1e-7&&m::length(begin.p-b.o.weapons[1].p)<1e-8)),
     "orientation-only replay preserves the primary pivot and existing bounded coupling geometry");
 }
 {Bow63 b(.10);b.latch();for(int n=0;n<15;++n){b.i.hands[0].p.y-=.02;b.next();}
  const double fraction=b.o.bowFraction;b.p[1].grip[0].p.y-=.02;b.p[1].bowGeometryRank48=2;b.next();
  ck(std::abs(b.o.bowFraction-fraction)<1e-8,"a better native rest measurement rebases without gaining or losing charge");}
 std::printf("bow_pull63=%u/%u native_execution=NONE\n",checks-failed,checks);return failed?1:0;
}
