#include "vrik/physical_weapon.h"
#include "vrik/equipment_gesture114.h"
#include <cstdio>
#include <cmath>
namespace p=kcd2vr::physical;namespace e=kcd2vr::equipment114;namespace m=kcd2vr::pose_core;
static unsigned tests=0,failures=0;
static void check(bool b,const char* text){++tests;failures+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",text);}
static int drawAtLeft(const p::Input& in,p::Profile bow){
 e::State state{};e::Input input{};input.key=12;input.generation=input.epoch=1;input.tick=1000;input.eligible=true;input.radius=.28;
 const auto a=p::body_axes(in);const auto hand=in.shoulders[1]+a.up*.10-a.forward*.10;
 p::Vec3 mount{};input.mountKnown=p::shoulder_holster_closest131(in,bow,hand,mount);input.mount=mount;input.hand=hand;input.grip=0;e::update(state,input);
 input.grip=1;input.tick+=10;e::update(state,input);input.hand=hand-a.forward*.40;input.tick+=90;
 return e::update(state,input)==e::Request::Draw;
}
int main(){
 for(double yaw:{0.,.6,-1.4}){
  p::Input in{};const m::Vec3 right{std::cos(yaw),std::sin(yaw),0},forward{-std::sin(yaw),std::cos(yaw),0};
  in.shoulders[0]=right*.2+m::Vec3{.07,-.09,1.35};in.shoulders[1]=right*-.2+m::Vec3{.07,-.09,1.35};
  p::Profile bow{};bow.classId=9;p::Profile pole{};pole.classId=7;p::Profile shield{};shield.classId=8;
  p::Vec3 bowMount{},poleMount{},closest{};p::holster_mount126(in,bow,bowMount);p::holster_mount126(in,pole,poleMount);
  check(p::holster_zone126(bow)==p::HolsterZone126::LeftShoulderBow&&p::holster_hand126(bow)==1,"HS2 bow side LEFT in its own zone, native primary hand LEFT");
  check(m::length(bowMount-(in.shoulders[1]+m::Vec3{0,0,.20}-forward*.14))<1e-8,"HS2 bow mount is own left shoulder +20cm up/14cm behind in current body axes");
  check(p::holster_zone126(pole)==p::HolsterZone126::RightShoulder&&p::holster_hand126(pole)==0,"heavy polearm remains right-shoulder/right-hand");
  check(m::length(poleMount-(in.shoulders[0]+m::Vec3{0,0,.14}-forward*.12))<1e-8&&m::length(p::right_shoulder_zone(in)-(in.shoulders[0]+m::Vec3{0,0,.10}-forward*.10))<1e-8,"HS2 polearm +14/-12 over the right shoulder, quiver pick unchanged");
  p::shoulder_holster_closest131(in,bow,in.shoulders[1]-m::Vec3{0,0,.4},closest);
  check(std::abs(closest.z-in.shoulders[1].z-.20)<1e-9,"low hand cannot drag the bow anchor down");
  p::shoulder_holster_closest131(in,pole,in.shoulders[0]-m::Vec3{0,0,.4},closest);
  check(std::abs(closest.z-in.shoulders[0].z-.14)<1e-9,"polearm anchor keeps its +14cm height");
  p::shoulder_holster_closest131(in,bow,bowMount+m::Vec3{0,0,.4},closest);
  check(std::abs(closest.z-in.shoulders[1].z-.32)<1e-9,"upper bow capsule bound is anchor +12cm");
  // HS2 volumes: the chest/collar must never take the bow, a hand over the shoulder must.
  const auto chestHand=in.shoulders[1]+right*.10+forward*.12-m::Vec3{0,0,.06};
  check(!p::draw_volume_ok_hs2(in,bow,chestHand),"HS2 left hand at the upper chest/collar does NOT acquire the bow");
  check(!p::draw_volume_ok_hs2(in,bow,in.shoulders[1]+m::Vec3{0,0,.05}+forward*.10),"HS2 hand above but in front of the chest plane does NOT acquire the bow");
  check(p::draw_volume_ok_hs2(in,bow,in.shoulders[1]+m::Vec3{0,0,.12}-forward*.08),"HS2 hand over/behind the left shoulder acquires the bow");
  check(p::draw_volume_ok_hs2(in,shield,chestHand),"HS2 shield keeps its plain sphere (no volume rule)");
  {p::Vec3 shieldMount{};p::holster_mount126(in,shield,shieldMount);check(m::length(shieldMount-bowMount)>.10,"HS2 bow and shield points are more than 10 cm apart");
   const auto overHand=in.shoulders[1]+m::Vec3{0,0,.17}-forward*.13;check(m::length(overHand-bowMount)<m::length(overHand-shieldMount),"HS2 arbitration: a hand over the shoulder is nearest the bow, not the shield");}
  {p::Profile heavy{};heavy.classId=10;p::Vec3 hm{};p::holster_mount126(in,heavy,hm);check(hm.z>in.shoulders[1].z&&m::length(hm-bowMount)>.12,"HS2 heavy back point above the shoulder line and 12+ cm from the bow");
   check(!p::draw_volume_ok_hs2(in,heavy,in.shoulders[1]-m::Vec3{0,0,.25}),"HS2 heavy is not taken from under the shoulder");}
  check(drawAtLeft(in,bow)==1,"actual reach/grip/pull FSM draws once from current left shoulder");
  check(p::holster_zone126(shield)==p::HolsterZone126::LeftShoulder&&p::holster_hand126(shield)==1,"shield side and hand retained");
 }
 for(int cls:{1,2,4,16}){p::Profile profile{};profile.classId=cls;p::Input input{};p::Vec3 mount{};
  check(p::holster_zone126(profile)==p::HolsterZone126::NativeBelt&&!p::holster_mount126(input,profile,mount),"native scabbard remains native; no invented global belt offset");}
 p::Input original{};original.sample=7;original.bodyEpoch=8;original.epoch=9;original.tick=1000;original.grip[0]=.8;original.trigger[1]=.4;original.allowed=true;original.hands[0].p={1,2,3};
 const auto copied=p::holster_tracking_input_h43(original,{4,5,6},{7,8,9});
 check(copied.sample==7&&copied.bodyEpoch==8&&copied.epoch==9&&copied.tick==1000&&copied.grip[0]==.8&&copied.trigger[1]==.4&&copied.allowed,"geometry copy retains original lifecycle/input; no changed clocks or ownership");
 check(original.hands[0].p.x==1&&copied.hands[0].p.x==4,"holster copy never migrates visible or physical controller wrist");
 std::printf("holster_location_h42 %u/%u\n",tests-failures,tests);return failures?1:0;
}
