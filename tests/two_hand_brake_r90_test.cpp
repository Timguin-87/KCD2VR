// R90 (R88 ranged 00200c6): two hands in the hybrid (V66.129, no F4) drive.
// A hard stop after a fast turn must not shoot past the hands and spring back;
// option VrTwoHandWeightFeelR90 makes the weight slider slow two-hand turns.
// Same kernel replay as tests/two_hand_damping_d86_test.cpp, F4 physics OFF.
// g++ -std=c++20 -O2 -w -I src -I src/vrik tests/two_hand_brake_r90_test.cpp
#include "vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical;
static int checks=0,fail=0;
static void ck(bool ok,const char* n){++checks;if(!ok){++fail;std::printf("FAIL %s\n",n);}}
struct Replay {
 Input in{};State state{};std::array<Profile,2> profiles{};Output out{};
 Replay(int cls,double weight,double extent,bool option,double feel){
  in.allowed=true;in.epoch=in.bodyEpoch=1;in.tick=1000;in.sample=1;
  in.physicalCombat177=false;in.controllerAnchor119=in.constraintDrive129=true;
  in.weaponAuthorityScale178r44=feel;in.twoHandWeightFeelR90=option;
  in.tracked[0]=in.tracked[1]=true;in.grip[0]=1;in.grip[1]=1;in.supportConstraint130=true;
  in.hands[0].p={.2,.3,1.3};in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=in.reach[1]=1.5;
  auto& p=profiles[0];p.key=p.generation=1;p.classId=cls;p.primary=p.geometry=p.support=p.twoHanded=true;
  p.weight=weight;p.extent=extent;p.tip={0,extent,0};p.grip[1].p={-.12,0,0};profiles[1].side=1;
  for(int i=0;i<20;++i)next();
 }
 void next(unsigned ms=11){in.tick+=ms;++in.sample;in.timeNs=in.tick*1000000;in.hands[1]=compose(in.hands[0],profiles[0].grip[1]);
  out=step(state,in,profiles,[&](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=true;return c;});}
};
struct Result{double overshootDeg=0,maxLagDeg=0;bool settled=false,active=false;};
// turn the hands about z at `speed` rad/s for `travel` rad, then hold still.
static Result swingStop(int cls,double weight,double extent,double speed,bool option=false,double feel=.6,unsigned ms=11){
 Replay r(cls,weight,extent,option,feel);Result out{};out.active=r.out.active[0];double angle=0;const double travel=1.0;
 while(angle<travel){angle=std::min(travel,angle+speed*ms/1000.);r.in.hands[0].q=m::from_rotation_vector({0,0,angle});r.next(ms);
  out.maxLagDeg=std::max(out.maxLagDeg,m::angle(r.out.weapons[0].q,r.in.hands[0].q)*180/3.14159265358979323846);}
 for(int i=0;i<60;++i){r.next(ms);
  // signed z rotation of weapon vs the (now still) hands: positive = past them
  const auto rel=m::rotation_vector(m::multiply(r.out.weapons[0].q,m::conjugate(r.in.hands[0].q)));
  out.overshootDeg=std::max(out.overshootDeg,rel.z*180/3.14159265358979323846);}
 out.settled=m::angle(r.out.weapons[0].q,r.in.hands[0].q)<.001;
 return out;
}
int main(){
 kcd2vr::two_hand_d65::enabled=false;
 kcd2vr::held_motion_d67::setGripAnchored(true);
 kcd2vr::held_motion_d67::setSupportRelease(false);
 // R2 kernel (no brake) overshoots exactly these fast two-hand stops (sweep, R90 review):
 // halberd 8 rad/s @14 ms 1.0 deg, 12 rad/s @11/14 ms 1.15/2.5 deg; longsword 12 rad/s @14 ms 2.1 deg.
 struct Case{const char* name;int cls;double w,ext,speed;unsigned ms;};
 for(const Case c:{Case{"halberd 8/14",7,9.6,1.8,8.,14},Case{"halberd 12/11",7,9.6,1.8,12.,11},Case{"halberd 12/14",7,9.6,1.8,12.,14},
                   Case{"longsword 12/14",4,2.2,1.06,12.,14},Case{"longsword 4/11",4,2.2,1.06,4.,11}}){
  const auto r=swingStop(c.cls,c.w,c.ext,c.speed,false,.6,c.ms);
  std::printf("%-16s overshoot=%.3f deg maxLag=%.3f deg settled=%d\n",c.name,r.overshootDeg,r.maxLagDeg,int(r.settled));
  ck(r.active,"two-hand weapon owned by the hybrid drive");
  ck(r.overshootDeg<.05,"hard stop: no spring-back past the hands");
  ck(r.settled,"weapon settles on the hands after the stop");
 }
 // option: weight slider 0.3 (heavy) must trail more than 1.0 with the option ON, and do nothing with it OFF
 const auto heavyOn=swingStop(7,9.6,1.8,1.,true,.3),lightOn=swingStop(7,9.6,1.8,1.,true,1.);
 const auto heavyOff=swingStop(7,9.6,1.8,1.,false,.3),lightOff=swingStop(7,9.6,1.8,1.,false,1.);
 std::printf("lag option ON  feel .3=%.3f 1.0=%.3f | OFF feel .3=%.3f 1.0=%.3f deg\n",heavyOn.maxLagDeg,lightOn.maxLagDeg,heavyOff.maxLagDeg,lightOff.maxLagDeg);
 ck(heavyOn.maxLagDeg>lightOn.maxLagDeg+1e-3,"option ON: lower weight feel = heavier two-hand turn");
 ck(std::abs(heavyOff.maxLagDeg-lightOff.maxLagDeg)<1e-9,"option OFF: weight slider does not touch the two-hand hybrid drive (R2)");
 ck(heavyOn.overshootDeg<.05&&heavyOn.settled,"option ON: still no spring-back");
 std::printf("two_hand_brake_r90=%d/%d\n",checks-fail,checks);return fail?1:0;
}
