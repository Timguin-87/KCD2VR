#include "vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical;
namespace d=kcd2vr::dynamics177;
static int checks=0,fail=0;
void ck(bool ok,const char* n){++checks;if(!ok){++fail;std::printf("FAIL %s\n",n);}}
struct Replay {
 Input in{};State state{};std::array<Profile,2> profiles{};Output out{};
 bool support=true,unknown=false;
 Replay(bool two=true):support(two){
  in.allowed=true;in.epoch=in.bodyEpoch=1;in.tick=1000;in.sample=1;
  in.physicalCombat177=in.controllerAnchor119=in.constraintDrive129=true;
  in.tracked[0]=in.tracked[1]=true;in.grip[0]=1;in.grip[1]=two?1:0;in.supportConstraint130=true;
  in.hands[0].p={.2,.3,1.3};in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=in.reach[1]=1.5;
  auto& p=profiles[0];p.key=p.generation=1;p.classId=7;p.primary=p.geometry=p.support=p.twoHanded=true;
  p.weight=9.6;p.extent=1.8;p.tip={0,1.8,0};p.grip[1].p={-.12,0,0};profiles[1].side=1;
  for(int i=0;i<20;++i)next();
 }
 void next(unsigned ms=10){in.tick+=ms;++in.sample;in.timeNs=in.tick*1000000;in.hands[1]=compose(in.hands[0],profiles[0].grip[1]);
  out=step(state,in,profiles,[&](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=!unknown;if(unknown)c.fraction=0;return c;});}
};
int main(){
 kcd2vr::two_hand_d65::enabled=false;
 kcd2vr::held_motion_d67::setGripAnchored(true);
 kcd2vr::held_motion_d67::setSupportRelease(false);
 for(unsigned ms:{8u,11u,16u}){
  Replay two(true),one(false);
  ck(two.out.active[0]&&two.out.support[0]&&!one.out.support[0],"actual kernel acquires correct support owner");
  // A copied impulse isolates the existing contact-rebound integrator. It is
  // not a guessed collision receipt and never grants damage/attack permission.
  two.state.weapon[0].angularRecoil={0,0,.05};two.state.weapon[0].angularRecoilVelocity={};
  one.state.weapon[0].angularRecoil={0,0,.05};one.state.weapon[0].angularRecoilVelocity={};
  bool twoCrossed=false,oneCrossed=false;double twoMinimum=1,oneMinimum=1,maxPivot=0;
  for(unsigned t=0;t<600;t+=ms){two.next(ms);one.next(ms);
   twoMinimum=std::min(twoMinimum,two.state.weapon[0].angularRecoil.z);oneMinimum=std::min(oneMinimum,one.state.weapon[0].angularRecoil.z);
   twoCrossed|=two.state.weapon[0].angularRecoil.z<0;oneCrossed|=one.state.weapon[0].angularRecoil.z<0;
   maxPivot=std::max(maxPivot,m::length(two.out.hands[0].p-two.in.hands[0].p));}
  ck(!twoCrossed&&oneCrossed,"supported rebound returns without old oscillating overshoot");
  ck(std::abs(two.state.weapon[0].angularRecoil.z)<1e-5,"supported recoil settles after release");
  ck(maxPivot<1e-9,"primary grip remains exact while angular weight persists");
  double maxLag=0;
  for(unsigned t=ms;t<=400;t+=ms){double u=double(t)/400;two.in.hands[0].q=m::from_rotation_vector({0,0,.8*std::sin(u*3.14159265358979323846)});two.next(ms);
   maxLag=std::max(maxLag,m::angle(two.out.weapons[0].q,two.in.hands[0].q));
   ck(finite(two.out.weapons[0])&&two.out.active[0],"cut and reversal retain finite owned weapon");}
  ck(maxLag>1e-4&&maxLag<=6.001*3.14159265358979323846/180,"two-hand weight remains visible inside tighter envelope");
  for(int i=0;i<100;++i)two.next(ms);
  ck(m::angle(two.out.weapons[0].q,two.in.hands[0].q)<.001,"reversed pole settles without sustained spring motion");
  const auto checked=two.out.weapons[0];two.unknown=true;two.in.hands[0].p.y+=.2;two.next(ms);
  ck(m::length(two.out.weapons[0].p-checked.p)<1e-9&&!two.out.contactKnown[0],"unknown world contact retains existing checked-motion stop");
  std::printf("dt_ms=%u supported_recoil_min=%.9g one_hand_min=%.9g primary_error_m=%.9g max_lag_deg=%.9g\n",ms,twoMinimum,oneMinimum,maxPivot,maxLag*180/3.14159265358979323846);
 }
 const auto b=d::body(7,9.6,1.8);ck(b.mass==3&&b.gripInertia>4,"native tuning mass/length inertia remains heavy");
 const auto supported=d::classAuthority59c14(d::authority(10,10,1,false,true),d::handling59c14(7,true,.7));
 ck(std::abs(supported.torque-63)<1e-9,"support no longer loses its torque advantage to second class penalty");
 std::printf("two_hand_damping_d86=%d/%d\n",checks-fail,checks);return fail?1:0;
}
