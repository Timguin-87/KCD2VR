#include "vrik/weapon_sweep112.h"
#include "vrik/arm_constraints.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::physical;
namespace m=kcd2vr::pose_core;
static int count=0;
static void ck(bool v,const char* name){++count;if(!v){std::fprintf(stderr,"FAIL %s\n",name);std::exit(1);}}
static bool has(const Output& o,CommandKind k){for(unsigned j=0;j<o.commandCount;++j)if(o.commands[j].kind==k)return true;return false;}
static void next(Input& i){++i.sample;i.tick+=10;}
static Input input(){Input i{};i.allowed=i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
 i.sample=i.epoch=i.bodyEpoch=1;i.tick=1000;i.dynamics=false;i.contact=false;i.attacks=false;
 i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};return i;}
static auto clear=[](const Profile&,Pose,Pose,bool){return Contact103{};};
int main(){
 {auto i=input();i.contact=true;i.requirePrimaryGrip114=true;i.grip[0]=1;i.shoulders[0]={0,0,1.3};i.hands[0].p={0,.64,1.3};
  Profile p{};p.key=1;p.generation=1;p.side=0;p.classId=4;p.primary=p.geometry=p.nativeSeed=true;p.grip[0].p={.30,0,0};p.tip={0,1,0};
  p.nativeWeapon.q=m::from_rotation_vector({0,0,2.4});p.nativeWeapon.p=i.hands[0].p-m::rotate(p.nativeWeapon.q,p.grip[0].p);
  const auto desired=compose(i.hands[0],inverse(p.grip[0]));
  ck(m::length(point(blend(p.nativeWeapon,desired,.5),p.grip[0].p)-i.shoulders[0])>i.reach[0],"old origin blend reproduces unreachable intermediate grip");
  State s{};std::array<Profile,2> ps{p,Profile{}};bool reached=false,deferred=false;int rays=0;
  for(int n=0;n<20;++n){int work=0;auto o=step(s,i,ps,[&](const Profile& pr,Pose a,Pose b,bool acquire){return sweep_weapon112(pr,a,b,acquire,[&](Vec3,Vec3,double){++work;return Contact103{};});});
   rays=std::max(rays,work);ck(o.active[0]&&m::length(o.hands[0].p-i.hands[0].p)<1e-8,"offset grip remains reachable and owned throughout recovery");
   deferred|=o.sweepDeferred115[0];ck(!o.commandCount,"recovery emits no attack");
   if(o.contactKnown[0]&&m::angle(o.weapons[0].q,desired.q)<1e-6){reached=true;break;}next(i);}
  ck(reached&&deferred&&rays<=15,"offset native seed reaches controller through bounded checked prefixes");
  next(i);i.hands[0].p.x+=.01;const auto before=s.weapon[0].weapon;
  auto unknown=[](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=false;c.fraction=0;return c;};
  auto follow=step(s,i,ps,unknown);
  ck(!follow.contactKnown[0]&&m::length(before.p-follow.weapons[0].p)<1e-8,
     "unknown after CLEAR cannot authorize new movement");
  next(i);i.hands[0].p.x+=.01;
  (void)step(s,i,ps,[](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=true;c.fraction=0;return c;});
  next(i);i.hands[0].p.x+=.01;const auto held=s.weapon[0].weapon;
  auto blocked=step(s,i,ps,unknown);
  ck(!blocked.contactKnown[0]&&m::length(held.p-blocked.weapons[0].p)<1e-8,"unknown query after a known obstacle still cannot move weapon through it");
 }
 {auto i=input();i.hands[0].p={.2,.3,1.3};i.hands[1].p={-.03,.3,1.3};
  Profile p{};p.side=0;p.key=1;p.classId=4;p.primary=p.support=p.geometry=p.twoHanded=true;p.grip[1].p={-.1,0,0};p.tip={0,.8,0};
  std::array<Profile,2> ps{p,Profile{}};State s{};step(s,i,ps,clear);next(i);i.grip[1]=1;auto o=step(s,i,ps,clear);
  ck(!o.support[0],"near support grip needs dwell");for(int n=0;n<4;++n){next(i);o=step(s,i,ps,clear);}
  ck(o.support[0]&&o.owned[1],"13 cm support approach latches after deliberate 40 ms hold");
  next(i);i.grip[1]=0;o=step(s,i,ps,clear);ck(!o.support[0]&&o.active[0],"support release does not drop primary ownership");
 }
 {auto i=input();i.requirePrimaryGrip114=i.quiverGesture114=i.bowRegrab116=i.bowRetention=true;i.grip[1]=1;
  i.hands[1].p={0,.4,1.3};i.hands[0].p={0,.25,1.3};
  Profile p{};p.side=1;p.key=3;p.generation=1;p.classId=9;p.primary=p.support=p.geometry=true;p.grip[0].p={0,-.15,0};p.axis={0,-1,0};p.tip={0,0,.6};
  std::array<Profile,2> ps{Profile{},p};State s{};step(s,i,ps,clear);next(i);i.grip[0]=1;auto o=step(s,i,ps,clear);
  ck(o.bowHeld&&has(o,CommandKind::BowBegin),"initial native bow permits nock grab without redundant shoulder trip");
  next(i);i.hands[0].p={.10,0,1.3};o=step(s,i,ps,clear);
  ck(o.bowHeld&&o.bowFraction>.4&&m::length(o.hands[0].p-i.hands[0].p)<1e-8,"drawing wrist remains at tracked hand despite lateral offset");
  bool cancelled=false;for(int n=0;n<18;++n){next(i);i.hands[0].p.x=.31;o=step(s,i,ps,clear);cancelled|=has(o,CommandKind::BowCancel)&&o.bow116.reason==BowReason116::Deviation;}
  ck(cancelled&&!s.weapon[1].quiverSpent116,"sustained deviation cancels but does not consume retry ticket");
  next(i);i.grip[0]=0;i.hands[0].p={0,.25,1.3};step(s,i,ps,clear);next(i);i.grip[0]=1;o=step(s,i,ps,clear);
  ck(o.bowHeld&&has(o,CommandKind::BowBegin)&&!s.weapon[1].lateralSince,"cancelled draw can regrab and resets stale deviation timer");
  next(i);i.hands[0].p.y=0;step(s,i,ps,clear);next(i);i.grip[0]=0;o=step(s,i,ps,clear);
  ck(has(o,CommandKind::BowFire)&&s.weapon[1].quiverSpent116,"deliberate draw release requests fire and consumes gesture ticket");
  ck(!has(step(s,i,ps,clear),CommandKind::BowFire),"same sample never repeats fire request");
  next(i);i.hands[0].p={0,.25,1.3};step(s,i,ps,clear);next(i);i.grip[0]=1;o=step(s,i,ps,clear);
  ck(!o.bowHeld&&o.bow116.needsQuiver,"next shot requires quiver gesture while ownership remains continuous");
  next(i);i.grip[0]=0;i.hands[0].p=right_shoulder_zone(i);step(s,i,ps,clear);
  for(int n=0;n<6;++n){next(i);i.grip[0]=1;step(s,i,ps,clear);}next(i);i.hands[0].p={0,.25,1.3};o=step(s,i,ps,clear);
  ck(o.bowHeld&&has(o,CommandKind::BowBegin),"new shoulder grab permits next native draw");
  next(i);i.grip[1]=0;o=step(s,i,ps,clear);ck(has(o,CommandKind::BowCancel)&&!has(o,CommandKind::BowFire)&&!o.active[1],"primary grip release cancels rather than firing");
  Vec3 mount{};ck(shoulder_mount116(i,p,mount)&&m::length(mount-left_shoulder_zone_h42(i))<1e-9,"H42 bow uses LEFT shoulder gesture zone for left primary hand");
  p.classId=8;ck(shoulder_mount116(i,p,mount)&&mount.x<0,"R14B shield uses the LEFT shoulder zone");
  p.classId=16;ck(p.melee(),"retail hunting sword participates in ordinary melee input");
 }
 {for(int sign:{-1,1}){m::ForearmPronation s{};for(int n=0;n<=720;++n)m::continuous_pronation(s,m::from_rotation_vector({0,sign*n*m::armPi/180,0}),{0,1,0},1,n+1,1000+11*n);
   for(int n=0;n<100;++n)m::continuous_pronation(s,{}, {0,1,0},1,722+n,9000+11*n);
   ck(std::abs(s.allowed)<1e-8,"return to neutral unwinds either forearm after complete turns");}
 }
 {m::ForearmPronation s{};const auto roll=m::from_rotation_vector({0,m::armPi/2,0});
  for(int n=0;n<100;++n)m::continuous_pronation(s,roll,{0,1,0},1,n+1,1000+11*n);
  const double before=s.allowed;
  m::continuous_pronation(s,m::from_rotation_vector({m::armPi,0,0}),{0,1,0},1,101,2100);
  m::continuous_pronation(s,roll,{0,1,0},1,102,2111);
  ck(std::abs(s.allowed-before)<1e-8,"pronation share is not applied twice after an ill-conditioned sample");
 }
 std::printf("PASS %d V66.116 regression checks\n",count);
}
