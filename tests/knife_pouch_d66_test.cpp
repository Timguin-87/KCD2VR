#include "vrik/physical_weapon.h"
#include "vrik/native_supply_t2.h"
#include "subsystems/throw_drop/pending_reach_t23.h"
#include "subsystems/throw_drop/hand_claim_t24.h"
#include <cstdio>
#include <cmath>
namespace p=kcd2vr::physical;namespace m=kcd2vr::pose_core;namespace s=kcd2vr::supply_t2;
namespace d=kcd2vr::knife_pouch_d66;namespace a=kcd2vr::throw_admission_t23;
namespace h=kcd2vr::hand_claim_t24;namespace ac=kcd2vr::ammo_carry_t18;
static int checks=0,failures=0;
static void ck(bool good,const char* reason){++checks;if(!good){++failures;std::printf("FAIL %s\n",reason);}}
static bool same(m::Vec3 x,m::Vec3 y){return x.x==y.x&&x.y==y.y&&x.z==y.z;}
static s::Input input(bool hip){s::Input i{};i.manager=1;i.player=2;i.character=3;i.entity=4;i.body=5;i.request=6;i.sample=i.tick=1000;i.allowed=i.tracked=true;i.chestKnivesT26=true;i.rightHipPouchD66=hip;i.pose18[0]=i.pose18[1]=true;i.pouchNear[0]=true;return i;}
int main(){
 ck(!d::rightHipSetting(),"trial OFF default");d::setRightHip(true);ck(d::rightHipSetting(),"setting enable");d::setRightHip(false);ck(!d::rightHipSetting(),"restore legacy without reset");
 for(double yaw:{0.,.65,-1.4,3.14}){
  p::Input in{};m::Vec3 right{std::cos(yaw),std::sin(yaw),0};const m::Vec3 root{1.7,-2.3,1.4};
  in.shoulders[0]=root+right*.2;in.shoulders[1]=root-right*.2;auto axes=p::body_axes(in);
  auto old=p::chest_knife_pouch_k32(in);auto hip=p::knife_pouch_d66(in,true);
  ck(same(p::knife_pouch_d66(in,false),old),"OFF exact legacy center");
  ck(same(hip,p::body_shoulder_r38(in,0)-axes.up*.50+axes.right*.03+axes.forward*.13),"ON historical right-hip-front center");
  ck(p::knife_pouch_holds_d66(in,hip,true),"hip center accepted");
  ck(!p::knife_pouch_holds_d66(in,hip+axes.right*.161,true),"bounded radius");
  ck(!p::knife_pouch_holds_d66(in,old,true)&&!p::knife_pouch_holds_d66(in,hip,false),"location choices distinct");
  ck(!p::knife_pouch_holds_d66(in,{NAN,0,0},true),"NaN refused");
  for(m::Vec3 point:{old,old+axes.forward*.12,old-axes.right*.17,hip,in.shoulders[0]})
   ck(p::knife_pouch_holds_d66(in,point,false)==p::chest_knife_pouch_holds_k32(in,point),"OFF exact legacy predicate");
 }
 for(bool hip:{false,true}){
  auto i=input(hip);s::Gesture left;auto step=[&](float grip,bool ready=true,bool reserved=false,int side=1){++i.sample;i.tick+=11;i.grip=i.grip18[side]=grip;return left.next(i,i.tick,true,reserved,side,ready);};
  ck(step(0)==-1,"open left");ck(step(1)==0,"left squeeze immediate without pull or cooldown");ck(step(1)==-1,"held occurrence no duplicate");
  ck(step(0)==-1&&step(1)==0,"actual opening immediate rearm");
  ck(step(0)==-1&&step(1,false)==-1&&step(1,true)==0,"pending eligible grip waits then draws");
  ck(step(0)==-1,"open before occupied");i.hand[0]=0x44;ck(step(1)==-1,"occupied selected hand refused");i.hand[0]=0;
  ck(step(0)==-1,"open before fist");i.trigger18[1]=1;ck(step(1)==-1,"left fist excludes utility");i.trigger18[1]=0;ck(step(1)==-1,"trigger release alone no replay");
  ck(step(0)==-1&&step(1,false)==-1,"undecided squeeze");i.rightHipPouchD66=!hip;ck(step(1,true)==-1,"location switch cancels undecided");ck(step(0)==-1&&step(1)==0,"new release after switch immediately rearms");
  s::Gesture right;i=input(hip);i.grip=0;ck(right.next(i,i.tick,true,false,0)==-1,"right open cannot arm");++i.sample;i.tick+=11;i.grip=i.grip18[0]=1;ck(right.next(i,i.tick,true,false,0)==-1,"right squeeze cannot draw");
  auto other=i;ck(a::SameFrame(i,other),"same packet matches");other.rightHipPouchD66=!i.rightHipPouchD66;ck(!a::SameFrame(i,other),"location cannot restamp old source");
 }
 // D66 hip-only occurrence contract; legacy OFF must keep its exact entry rule.
 for(bool hip:{false,true}){
  auto i=input(hip);s::Gesture g;auto step=[&](bool near,bool ready=true,bool reserved=false){++i.sample;i.tick+=11;i.pouchNear[0]=near;return g.next(i,i.tick,true,reserved,1,ready);};
  i.grip=i.grip18[1]=1;
  ck(step(false)==-1,"closed outside has no draw");
  ck(step(true)==(hip?0:-1),"closed entry acquires only in hip trial");
  ck(step(false)==-1&&step(true)==-1,"same held occurrence cannot leave/reenter to duplicate");
  ++i.request;ck(step(true)==-1,"source replacement cannot replay issued occurrence");
  i.grip=i.grip18[1]=0;ck(step(false)==-1,"actual opening clears occurrence");i.grip=i.grip18[1]=1;
  ck(step(false)==-1,"closing outside never draws");
  ck(step(true,false)==-1,"native admission unavailable retains eligible intent");
  ck(step(true,true)==(hip?0:-1),"hip held entry acquires once native admission is ready");
  i.grip=i.grip18[1]=0;ck(step(true)==-1,"open before occupied");i.grip=i.grip18[1]=1;i.hand[0]=0x55;
  ck(step(true)==-1,"occupied left hand never obtains knife");i.hand[0]=0;ck(step(true)==-1,"real hand ownership spent this occurrence");
  i.grip=i.grip18[1]=0;ck(step(false)==-1,"open before fist outside");i.grip=i.grip18[1]=1;i.trigger18[1]=1;
  ck(step(false)==-1,"fist spends utility occurrence outside pouch");i.trigger18[1]=0;
  ck(step(true)==-1,"dropping Trigger while Grip held cannot replay fist");
  ++i.request;ck(step(true)==-1,"source replacement cannot replay spent fist without Grip opening");
  i.grip=i.grip18[1]=0;ck(step(true)==-1,"open after fist");i.grip=i.grip18[1]=1;ck(step(true)==0,"new actual squeeze works immediately");
 }
 h::Owner owner{1,2,3,4,5};ac::Claim knife{1,2,3,0x77,11,0,4,5,1,1,9,ac::Stage::Held};
 h::Reads leftHeld{2u,2u,true,true,knife,knife};ck(h::blocks(h::classify(1,owner,leftHeld).state),"actual left knife owns left holsters");
 h::Reads rightFree{2u,2u,true,true,{},{}};ck(!h::blocks(h::classify(0,owner,rightFree).state),"left knife keeps right sword independent");
 ac::flights=1;h::Reads released{0u,0u,true,true,{},{}};ck(!h::blocks(h::classify(1,owner,released).state),"flight alone no hand block");ac::flights=0;
 std::printf("%d/%d passed\n",checks-failures,checks);return failures?1:0;
}
