#include "vrik/physical_weapon.h"
#include "vrik/weapon_sweep112.h"
#include "vrik/contact_budget.h"
#include <cstdio>
using namespace kcd2vr::physical;
namespace cfg=kcd2vr::held_motion_d67;
static int count=0,failed=0;
static void ck(bool b,const char*n){++count;if(!b)++failed;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
struct Fixture {
 Input in{};State state{};std::array<Profile,2> p{};Output out{};bool wall=false,unknown=false;
 Fixture(int cls=7){in.allowed=in.dynamics=in.controllerAnchor119=in.physicalCombat177=in.constraintDrive129=true;
  in.epoch=in.bodyEpoch=1;in.tick=1000;in.sample=1;in.timeNs=1000000000;
  in.tracked[0]=in.tracked[1]=true;in.grip[0]=1;
  in.hands[0].p={.2,.3,1.3};in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=in.reach[1]=1.5;
  auto&a=p[0];a.key=a.generation=1;a.classId=cls;a.primary=a.geometry=true;a.weight=cls==7?9.6:3.4;
  a.grip[0].p={0,.627,0};a.extent=cls==7?1.8:.9;a.tip={0,a.extent,0};p[1].side=1;next();}
 void next(){in.tick+=11;in.timeNs+=11000000;++in.sample;
  in.intentHands[0]=in.hands[0];in.intentHands[1]=in.hands[1];
  out=step(state,in,p,[&](const Profile&,Pose a,Pose b,bool){Contact103 c{};
   if(unknown){c.known=false;c.fraction=0;return c;}
   const auto from=point(a,p[0].grip[0].p),to=point(b,p[0].grip[0].p);
   if(wall&&to.y>.4){c.fraction=std::clamp((.4-from.y)/std::max(1e-9,to.y-from.y),0.,1.);c.plane=true;c.point={.2,.4,1.3};c.normal={0,-1,0};c.feature=p[0].grip[0].p;}
   return c;});}
};
static double release(bool enabled,double deg,bool wall=false,bool moving=false){
 cfg::setSupportRelease(enabled);Fixture f;f.in.supportConstraint130=f.in.supportAxis59c16=true;
 f.p[0].twoHanded=true;f.p[0].diagnosticBounds59c6=true;f.p[0].diagnosticBoundsMin59c6={-.03,-.7,-.03};f.p[0].diagnosticBoundsMax59c6={.03,1.8,.03};
 f.in.hands[1].p=point(f.out.weapons[0],f.p[0].grip[0].p+Vec3{0,-.25,0});f.in.grip[1]=1;
 for(int n=0;n<12;++n)f.next();ck(f.out.support[0],"support fixture acquired actual shaft grasp");
 f.in.hands[1].p=f.in.hands[0].p+m::rotate(m::from_rotation_vector({0,0,deg/57.295779513}),{0,-.25,0});
 for(int n=0;n<60;++n)f.next();
 // Enable actual raw two-family attack recognition before the ownership edge.
 f.in.attacks=f.in.requirePrimaryGrip114=f.in.heldMotionAttacks=f.in.coherentMelee119=f.in.naturalCombat117=f.in.forwardOnly19=true;
 f.in.intentTracked[0]=f.in.intentTracked[1]=true;
 for(int n=0;n<4;++n)f.next();
 const auto before=f.out.weapons[0];f.in.grip[1]=0;f.wall=wall;f.in.contact=wall;f.next();
 const double delta=m::angle(before.q,f.out.weapons[0].q)*57.295779513;
 ck(!f.out.owned[1]&&!f.out.support[0]&&f.out.owned[0],"support release immediately frees other hand");
 ck(f.out.commandCount==0,"support release does not synthesize native combat action");
 ck(m::length(compose(f.out.weapons[0],f.p[0].grip[0]).p-f.out.hands[0].p)<1e-8,"released primary wrist and weapon share exact grip");
 if(enabled){ck(delta<=8*.011*57.295779513+.0001,"basis release cannot bypass bounded angular motor");
  unsigned semanticCommands=0,releaseSamples=0;bool currentTarget=true;
  for(int n=0;n<100;++n){if(moving)f.in.hands[0].q=m::multiply(m::from_rotation_vector({0,0,.18}),f.in.hands[0].q);
   f.next();semanticCommands+=f.out.commandCount;releaseSamples+=f.state.weapon[0].supportReleaseD67;
   const auto raw=compose(f.in.hands[0],inverse(f.p[0].grip[0]));
   const auto expected=m::multiply(f.state.weapon[0].supportReleaseOffsetD67,raw.q);
   currentTarget&=m::angle(expected,f.state.weapon[0].lastDesired.q)<1e-7;
  }
  ck(moving||semanticCommands==0,"settling visual motion never becomes attack intent");
  ck(!moving||(currentTarget&&releaseSamples<36),"fast current wrist still drives target and cannot prolong released basis");
  ck(!f.state.weapon[0].supportReleaseD67,"one-hand motor settles without stale offhand owner");}
 std::printf("release enabled%d inputDeg%.1f firstDelta%.6f\n",enabled,deg,delta);return delta;
}
int main(){
 // Historical-axis fixture; the accepted 0.2 bias has its own two_hand_bias_d65 suite.
 kcd2vr::two_hand_d65::enabled=false;

 for(int cls:{4,7})for(bool on:{false,true}){cfg::setGripAnchored(on);Fixture f(cls);
  f.in.hands[0].p.y+=.04;f.in.hands[0].q=m::from_rotation_vector({0,0,.15});f.next();
  const double gap=m::length(f.out.hands[0].p-f.in.hands[0].p);
  ck(on?gap<1e-8:gap>.001,"clear candidate follows grip only when enabled");
  ck(m::angle(f.out.hands[0].q,f.in.hands[0].q)>.001,"angular resistance retained while translation follows grip");
  ck(m::length(compose(f.out.weapons[0],f.p[0].grip[0]).p-f.out.hands[0].p)<1e-9,"nonzero native grip defines pivot, never model origin");
 }
 cfg::setGripAnchored(true);
 {Fixture strong,weak;strong.in.attributes={20,24,1,true};weak.in.attributes={20,24,0,true};
  strong.in.hands[0].p.y+=.03;weak.in.hands[0].p.y+=.03;
  strong.in.hands[0].q=weak.in.hands[0].q=m::from_rotation_vector({0,0,.1});strong.next();weak.next();
  ck(m::length(strong.out.hands[0].p-strong.in.hands[0].p)<1e-8&&m::length(weak.out.hands[0].p-weak.in.hands[0].p)<1e-8,"clear primary grip stays attached regardless of stamina");
  ck(m::angle(weak.out.weapons[0].q,Quat{})<m::angle(strong.out.weapons[0].q,Quat{}),"fatigue still reduces angular acceleration with anchored translation");}
 for(int mode:{0,1}){Fixture f;f.in.contact=true;f.wall=mode==0;f.unknown=mode==1;f.in.hands[0].p.y=.55;f.next();
  ck(f.out.blocked[0]&&f.out.hands[0].p.y<=.4000001,"wall or unknown query still prevents anchored candidate crossing");
  const auto prior=f.out;f.next();ck(f.out.hands[0].p.y<=.4000001,"persistent blocked contact retains checked yield");
 }
 for(double deg:{63.115,54.833}){const double old=release(false,deg);const double repaired=release(true,deg);ck(old>30&&repaired<6,"recorded-size release discontinuity reduced against D66 control");}
 release(true,63.115,false,true);
 for(bool on:{false,true}){cfg::setCurrentArcCoverage(on);Profile p{};p.side=0;p.classId=4;p.primary=p.geometry=true;p.tip={0,.915,0};Pose from{},to{};to.q=m::from_rotation_vector({0,0,42.8077/57.295779513});
  unsigned calls=0;kcd2vr::contact::Budget budget;budget.begin_work();g_sweep_coverage_scale178r6=1;
  const auto c=sweep_weapon112(p,from,to,false,[&](Vec3,Vec3,double){Contact103 h;ck(budget.take(calls),"sweep respects18ray budget");++calls;budget.finish(calls);return h;});
  ck(on?c.fraction>.999:c.fraction<.8,"current fast arc expands immediately rather than alternating next-frame coverage");
  ck(calls<=18,"current-arc repair keeps total ray cap");
 }
 cfg::setCurrentArcCoverage(true);
 for(int failure:{1,2}){Profile p{};p.classId=4;p.primary=p.geometry=true;p.tip={0,.915,0};Pose from{},to{};to.q=m::from_rotation_vector({0,0,.75});unsigned calls=0;
  const auto c=sweep_weapon112(p,from,to,false,[&](Vec3,Vec3,double){Contact103 h;++calls;if(calls==4){if(failure==1){h.known=false;h.fraction=0;}else{h.plane=true;h.fraction=0;h.normal={0,-1,0};}}return h;});
  ck(c.fraction<1,"current arc cannot call unknown or hit segments clear");}
 std::printf("D67 held motion %d/%d\n",count-failed,count);return failed?1:0;
}
