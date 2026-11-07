#include "vrik/physical_weapon.h"
#include "vrik/fist_motion.h"
#include "vrik/upper_body.h"
#include "vrik/contact_budget.h"
#include "vrik/native_combat104.h"
#include <cstdio>
namespace p=kcd2vr::physical;namespace m=kcd2vr::pose_core;namespace f=kcd2vr::fists;namespace n=kcd2vr::native_combat104;
static int total=0,failures=0;void ck(bool x,const char*s){++total;failures+=!x;std::printf("%s %s\n",x?"PASS":"FAIL",s);}
int main(){
 kcd2vr::contact::Budget b;b.begin_work(3,1000);
 ck(b.take(999999),"pre-query preparation cannot expire a work-only budget");b.finish(1000199);
 ck(b.spent==200&&b.remaining==2,"actual first query charged exactly once");
 ck(b.take(9000000),"time spent in body solver between rays does not count as ray work");b.finish(9000750);
 ck(b.take(10000000),"last query admitted with 50 microseconds remaining");b.finish(10000099);
 ck(!b.take(10000200)&&b.spent==1000,"spent budget cannot admit further queries");
 b.begin_work(18,1000);ck(b.take(1000)&&!b.take(1001),"reentrant query cannot consume an in-flight slot");b.finish(900);ck(!b.take(1010),"backward clock invalidates query budget");
 ck(n::intent(n::Gesture::Slash,0,{1,0,0}).inputIndex==1,"directional swing requests retail heavy input, not attack-type ID");
 ck(n::intent(n::Gesture::Thrust,0,{0,1,0}).inputIndex==0&&n::intent(n::Gesture::Thrust,0,{}).zone==5,"thrust uses direct-attack input and lower attack zone");
 ck(n::intent(n::Gesture::Pommel,0,{}).inputIndex==2&&n::intent(n::Gesture::Pommel,0,{}).expectedType==-1,"pommel requests native special without pretending a variant is forced");
 n::Observed before{},after{};after.requested=true;ck(!n::entered(before,after),"request bit is not native action acceptance");after.phase=4;ck(n::entered(before,after),"native preparation-state entry is independently observed");after.phase=8;ck(n::entered(before,after),"native striking transition independently observed");
 // All coordinates below are a declared synthetic fixture, not exported Henry joints.
 p::Input in{};in.allowed=true;in.epoch=in.bodyEpoch=1;in.sample=1;in.tick=1000;in.timeNs=1000000000;in.tracked[0]=in.tracked[1]=true;in.reach[0]=in.reach[1]=1.8;in.dynamics=false;in.contact=false;in.attacks=false;in.bowActions=true;in.bowTrigger=true;in.bowRetention=true;
 in.shoulders[0]={.2,0,1.4};in.shoulders[1]={-.2,0,1.4};in.hands[1].p={0,.4,1.2};in.hands[0].p={0,.25,1.2};
 p::Profile bow{};bow.side=1;bow.key=5;bow.generation=1;bow.classId=9;bow.primary=bow.support=bow.geometry=true;bow.grip[0].p={0,-.15,0};bow.axis={0,-1,0};std::array<p::Profile,2> profiles{};profiles[1]=bow;p::State st;
 auto step=[&](){in.tick+=10;in.timeNs+=10000000;++in.sample;return p::step(st,in,profiles,[](auto const&,auto,auto,bool){return 1.;});};
 auto count=[](const p::Output&o,p::CommandKind k){int n=0;for(unsigned i=0;i<o.commandCount;++i)n+=o.commands[i].kind==k;return n;};
 step();step();in.grip[0]=1;auto o=step();ck(count(o,p::CommandKind::BowBegin)==0,"grip cannot nock a trigger-owned bow");
 in.trigger[0]=1;o=step();ck(count(o,p::CommandKind::BowBegin)==1&&o.bowHeld,"right trigger acquires the qualified nock once");
 for(int i=0;i<20;++i){in.hands[0].p.y-=.01;o=step();}ck(o.bowFraction>.25&&o.bowHeld,"right hand displacement rather than analog trigger controls draw");
 in.grip[0]=0;o=step();ck(o.bowHeld&&count(o,p::CommandKind::BowFire)==0,"grip release cannot drop or fire the trigger-owned arrow");
 in.trigger[0]=0;o=step();ck(count(o,p::CommandKind::BowFire)==1&&!o.bowHeld,"deliberate trigger release emits exactly one native fire request");
 ck(count(step(),p::CommandKind::BowFire)==0,"held released trigger does not refire");
 // Reach refinement can only rotate existing clavicle lever inside fixed limits.
 const auto axes=*m::chest_axes103({1,0,0},{0,0,1});int improved=0;
 for(int side=0;side<2;++side){double sign=side?-1:1;m::Vec3 pivot{sign*.03,0,1.45},shoulder{sign*.18,0,1.45},hand{sign*.18,.77,1.45};
  auto old=m::shoulder_goal103(axes,side,shoulder,hand,.65),now=m::shoulder_goal_reach104(axes,side,pivot,shoulder,hand,.65);
  auto distance=[&](auto a){auto q=m::multiply(m::from_rotation_vector(axes.up*(sign*a.protraction)),m::from_rotation_vector(axes.forward*(-sign*a.elevation)));return m::length(hand-(pivot+m::rotate(q,shoulder-pivot)));};
  improved+=distance(now)<distance(old)-.005;ck(now.protraction<=30*3.141592653589793/180+.000001&&now.elevation<=30*3.141592653589793/180+.000001,"residual shoulder reach obeys both anatomical policy limits");
 }
 ck(improved==2,"both shoulders recover forward reach without translating pivot or stretching arm");
 // Moderate deliberate fists, rather than only high-speed test motions.
 int recognized=0;for(double speed:{.55,.70,1.0,2.0}){f::State fs;f::Sample x{};x.valid=true;x.empty[0]=x.empty[1]=true;x.actor=1;x.epoch=1;x.tick=1000;x.sequence=1;x.head={0,1.6,0};x.controller[0]={.2,1.2,-.2};x.controller[1]={-.2,1.2,-.2};x.allowed[0]=x.controller[0];x.allowed[1]=x.controller[1];
  auto move=[&](double amount){x.tick+=10;++x.sequence;x.controller[0].z-=amount;x.allowed[0].z-=amount;return f::step(fs,x);};
  move(0);x.trigger[0]=x.grip[0]=1;for(int k=0;k<30;++k)move(0);int hits=0;for(int k=0;k<25;++k)hits+=move(speed*.01).valid;recognized+=hits==1;}
 ck(recognized==4,"0.55, 0.7, 1.0 and 2 m/s fist strokes each emit one intent");
 // Weapon motion recognition executes with production held-item eligibility.
 auto swordRun=[&](m::Vec3 velocity,bool reversed,bool contactUnknown,bool walking,bool backhand=false){
    p::Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.tick=1000;i.timeNs=1000000000;i.sample=1;
    i.tracked[0]=i.tracked[1]=true;i.hands[0].p={.2,.3,1.3};i.intentHands[0].p={.2,.3,-.2};i.intentHands[0].q=i.hands[0].q;i.intentTracked[0]=true;i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.reach[0]=i.reach[1]=1.8;
    i.contact=true;i.dynamics=false;i.attacks=true;i.bowActions=false;i.heldMotionAttacks=true;i.haveWorld=true;
    std::array<p::Profile,2> pp{};auto& weapon=pp[0];weapon.key=weapon.generation=1;weapon.side=0;weapon.classId=4;weapon.primary=weapon.geometry=true;weapon.tip={0,.65,0};weapon.axis={0,1,0};
    if(reversed){i.hands[0].q=m::from_rotation_vector({0,0,3.141592653589793});i.intentHands[0].q=i.hands[0].q;}p::State ws{};std::array<int,11> results{};
    auto advance=[&](m::Vec3 v,bool unknown){i.tick+=10;i.timeNs+=10000000;++i.sample;if(walking)i.world.p=i.world.p+v*.01;else {i.hands[0].p=i.hands[0].p+v*.01;i.intentHands[0].p=i.intentHands[0].p+v*.01;}
        auto output=p::step(ws,i,pp,[&](auto const&,auto,auto,bool){p::Contact103 contact{};contact.known=!unknown;contact.fraction=unknown?0:1;return contact;});
        for(unsigned j=0;j<output.commandCount;++j)++results[unsigned(output.commands[j].kind)];return output;};
    for(int k=0;k<30;++k)advance({},false);for(int k=0;k<30;++k)advance(velocity,contactUnknown);if(backhand)for(int k=0;k<30;++k)advance(velocity*-1,contactUnknown);return results;};
 auto slow=swordRun({.2,0,0},false,false,false);ck(slow[2]+slow[3]+slow[10]==0,"slow weapon repositioning emits no native attack intent");
 auto slash=swordRun({.6,0,0},false,false,false);ck(slash[2]==1&&slash[3]==0&&slash[10]==0,"ordinary 0.6m/s lateral swing emits one slash without holding grip");
 auto thrust=swordRun({0,.6,0},false,false,false);ck(thrust[3]==1&&thrust[2]==0,"ordinary outward blade-first motion emits a distinct thrust");
 auto withdraw=swordRun({0,-.6,0},false,false,false);ck(withdraw[2]+withdraw[3]+withdraw[10]==0,"pulling a forward blade back toward the chest is not a pommel attack");
 auto pommel=swordRun({0,.6,0},true,false,false);ck(pommel[10]==1&&pommel[2]==0&&pommel[3]==0,"outward butt-first movement with reversed weapon emits a special-intent candidate");
 auto unknown=swordRun({.6,0,0},false,true,false);ck(unknown[2]+unknown[3]+unknown[10]==0,"unavailable collision results cannot create an attack while the visible weapon is held still");
 auto reverse=swordRun({.6,0,0},false,false,false,true);ck(reverse[2]==2,"a deliberate backhand reversal can form a second cut without a compulsory pause");
 auto walk=swordRun({0,2,0},false,false,true);ck(walk[2]+walk[3]+walk[10]==0,"world/root locomotion alone never generates an attack");
 {p::Input ci{};ci.allowed=true;ci.epoch=ci.bodyEpoch=1;ci.sample=1;ci.tick=1000;ci.timeNs=1000000000;ci.tracked[0]=true;ci.intentTracked[0]=true;ci.hands[0].p={.2,.3,1.3};ci.intentHands[0].p={.2,.3,1.3};ci.shoulders[0]={.2,0,1.3};ci.reach[0]=1.8;ci.contact=false;ci.dynamics=false;ci.attacks=true;ci.heldMotionAttacks=true;
  std::array<p::Profile,2> cp{};cp[0].key=cp[0].generation=1;cp[0].side=0;cp[0].classId=4;cp[0].primary=cp[0].geometry=true;cp[0].tip={0,.65,0};cp[0].axis={0,1,0};p::State cs{};p::step(cs,ci,cp,[](auto const&,auto,auto,bool){return 1.;});int attacks=0;
  for(int k=0;k<30;++k){ci.tick+=10;ci.timeNs+=10000000;++ci.sample;ci.hands[0].p.y+=.02;auto co=p::step(cs,ci,cp,[](auto const&,auto,auto,bool){return 1.;});for(unsigned j=0;j<co.commandCount;++j)attacks+=co.commands[j].kind==p::CommandKind::Slash||co.commands[j].kind==p::CommandKind::Thrust||co.commands[j].kind==p::CommandKind::Pommel;}
  ck(attacks==0,"common body/room translation with fixed HMD-relative hand cannot become a sword attack");}
 std::printf("integration104=%d/%d host_policy_only=1\n",total-failures,total);return failures?1:0;
}
