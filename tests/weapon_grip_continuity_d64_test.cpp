#include "../src/vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical;
namespace m=kcd2vr::pose_core;
int main(){int fails=0,checks=0;auto ck=[&](bool b,const char* s){++checks;if(!b)++fails;std::printf("%s %s\n",b?"PASS":"FAIL",s);};
 auto clear=[](const Profile&,Pose,Pose,bool){return 1.;};
 Input in{};in.allowed=true;in.epoch=in.bodyEpoch=1;in.sample=1;in.tick=1000;
 in.tracked[0]=in.tracked[1]=in.intentTracked[0]=in.intentTracked[1]=true;
 in.hands[0].p={.2,.3,1.3};in.hands[1].p={-.2,.3,1.3};in.shoulders[0]=in.shoulders[1]={0,0,1.3};
 in.reach[0]=in.reach[1]=1.8;in.dynamics=false;in.contact=false;in.attacks=false;
 std::array<Profile,2> p{};auto& a=p[0];a.key=1;a.entity=42;a.generation=1;a.side=0;a.classId=7;
 a.primary=a.support=a.geometry=a.twoHanded=true;a.tip={0,.010,2.005};a.axis={0,0,1};a.extent=2;a.weight=3;
 a.grip[0].p={.024,-.066,.630};a.grip[1].p={-.036,-.026,1.233};
 State state{};step(state,in,p,clear);in.sample++;in.tick+=11;step(state,in,p,clear);
 auto prev=state.weapon[0].lastDesired;
 in.sample++;in.tick+=11;in.hands[0].q=m::from_rotation_vector({1.274,0,0});
 auto out=step(state,in,p,clear);
 ck(m::length(state.weapon[0].lastDesired.p-prev.p)>.40,"long model origin moves over40cm while grip stays fixed");
 ck(!(out.resetH29[0].reason&256),"stationary physical grip rotation does not reset translation history");
 ck(m::length(out.hands[0].p-in.hands[0].p)<1e-8,"weapon still pivots on the tracked primary grip");
 auto repeat=step(state,in,p,clear);ck(repeat.commandCount==0,"same source cannot repeat native commands");
 in.sample++;in.tick+=11;in.hands[0].p.y+=.45;out=step(state,in,p,clear);
 ck((out.resetH29[0].reason&256)!=0,"real grip translation above40cm retains reset");
 in.sample++;in.tick+=11;in.hands[0].q=m::from_rotation_vector({-1.,0,0});out=step(state,in,p,clear);
 ck((out.resetH29[0].reason&512)!=0,"gross orientation discontinuity retains reset");
 // A latched axis correction must survive a geometrically undefined coincident support hand.
 in.supportAxis59c16=true;in.autoSupport=false;in.grip[1]=1;in.sample++;in.tick+=11;
 state.weapon[0].support=true;state.weapon[0].supportAxis59c16=true;state.weapon[0].supportGripReady169r2=true;
 state.weapon[0].supportPrimary19c3=a.grip[0];state.weapon[0].supportGrip169r2=a.grip[1];
 state.weapon[0].supportCorrection=m::from_rotation_vector({0,.3,0});
 in.hands[1]=in.hands[0];const auto expected=*m::unit(m::multiply(state.weapon[0].supportCorrection,compose(in.hands[0],inverse(a.grip[0])).q));
 out=step(state,in,p,clear);
 ck(out.support[0]&&m::angle(out.weapons[0].q,expected)<1e-7,"coincident retained support preserves prior axis correction around primary grip");
 // The accepted bow path is deliberately outside this melee reset repair.
 State bowState{};a.classId=9;in.supportAxis59c16=false;in.grip[1]=0;in.bowActions=false;in.hands[0].q={};
 ++in.sample;in.tick+=11;step(bowState,in,p,clear);++in.sample;in.tick+=11;step(bowState,in,p,clear);
 ++in.sample;in.tick+=11;in.hands[0].q=m::from_rotation_vector({1.274,0,0});out=step(bowState,in,p,clear);
 ck((out.resetH29[0].reason&256)!=0,"bow retains its existing reset policy without new steering");
 std::printf("D64 weapon continuity %d/%d\n",checks-fails,checks);return fails?1:0;}
