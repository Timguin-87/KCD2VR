// HS (27 Oct): holster regression replay. H3 (19 Oct) drew/holstered bow, two-handed, hip sword, knife and shield
// with one simple rule: hand in the zone, close Grip, pull out. Every later owner (H39 fist, HS2 volume, R12 back
// point, R15 release) must keep these reaches working. Positions are the H3 / J27 headset reaches in body space.
#include "vrik/physical_weapon.h"
#include "vrik/equipment_gesture114.h"
#include <cmath>
#include <cstdio>
namespace p=kcd2vr::physical;namespace e=kcd2vr::equipment114;namespace m=kcd2vr::pose_core;
static unsigned tests=0,failures=0;
static void check(bool b,const char* t){++tests;failures+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",t);}
// Replays the modern zone-grip gesture: hand arrives already closed (Grip+Trigger fist chord), dwells, pulls out.
static bool drawWithClosedHand(const p::Input& in,const p::Profile& prof,m::Vec3 hand,m::Vec3 pullDir,double radius){
 e::State s{};e::Input i{};i.zoneGripR15=true;i.key=7;i.generation=i.epoch=1;i.tick=1000;i.eligible=true;i.radius=radius;
 auto step=[&](m::Vec3 h,double grip){p::Vec3 mount{};
  i.mountKnown=p::shoulder_holster_closest131(in,prof,h,mount)||p::holster_mount126(in,prof,mount);
  if(i.mountKnown&&!p::draw_volume_ok_hs2(in,prof,h)&&!s.latched)i.mountKnown=false; // producer hides the mount outside the volume
  i.mount=mount;i.hand=h;i.grip=grip;i.tick+=12;return e::update(s,i);};
 step(hand,1.0);step(hand,1.0);
 for(int k=0;k<10;++k)step(hand,1.0);              // dwell 120 ms
 e::Request r=e::Request::None;
 for(int k=1;k<=16&&r==e::Request::None;++k)r=step(hand+pullDir*(.02*k),1.0); // pull out up to 32 cm (H3 draws logged at gaps .24-.38)
 return r==e::Request::Draw;
}
int main(){
 for(double yaw:{0.,.6,-1.4}){
  p::Input in{};const m::Vec3 right{std::cos(yaw),std::sin(yaw),0},forward{-std::sin(yaw),std::cos(yaw),0},up{0,0,1};
  in.shoulders[0]=right*.2+m::Vec3{.07,-.09,1.35};in.shoulders[1]=right*-.2+m::Vec3{.07,-.09,1.35};
  const auto chest=(in.shoulders[0]+in.shoulders[1])*.5;
  p::Profile bow{};bow.classId=9;p::Profile pole{};pole.classId=7;p::Profile shield{};shield.classId=8;p::Profile knife{};knife.classId=0;
  const double rShoulder=.32,rBelt=.28;
  // -- bow: LEFT hand to the RIGHT shoulder (H3 zone 1 / R12 RightShoulderBowR12)
  const auto bowReach=in.shoulders[0]+up*.04+forward*.03-right*.06;   // J27-like reach: shoulder height, slightly in front
  p::Vec3 bm{};check(p::shoulder_holster_closest131(in,bow,bowReach,bm)&&m::length(bowReach-bm)<rShoulder,"bow: shoulder-height reach is inside the bow zone");
  check(p::draw_volume_ok_hs2(in,bow,bowReach),"bow: shoulder-height reach is inside the bow volume");
  check(p::holster_grab_hs(in,bow,bowReach,bm,rShoulder),"bow: closed hand there is a holster grab, not a fist");
  check(drawWithClosedHand(in,bow,bowReach,up*.5+forward*.5,rShoulder),"bow: closed-hand reach + pull draws (no release demanded)");
  // -- two-handed back weapon: RIGHT hand behind the RIGHT shoulder
  const auto poleReach=in.shoulders[0]-up*.08-forward*.08;
  p::Vec3 pm{};check(p::shoulder_holster_closest131(in,pole,poleReach,pm)&&m::length(poleReach-pm)<rShoulder,"back weapon: reach just behind the shoulder is inside the zone");
  check(p::draw_volume_ok_hs2(in,pole,poleReach),"back weapon: reach just behind the shoulder is inside the volume");
  check(p::holster_grab_hs(in,pole,poleReach,pm,rShoulder),"back weapon: closed hand there is a holster grab");
  check(drawWithClosedHand(in,pole,poleReach,up*.6+forward*.4,rShoulder),"back weapon: closed-hand reach + pull draws");
  // -- the exact J27 failure shapes (R14..R28 refused these as OUTSIDE_VOLUME / OUTSIDE_ZONE, H3 drew them)
  check(p::draw_volume_ok_hs2(in,pole,in.shoulders[0]-up*.10-forward*.03),"back weapon: hand at the side, 3 cm behind the shoulder, is in the volume");
  check(p::draw_volume_ok_hs2(in,bow,in.shoulders[0]-up*.10-right*.05),"bow: hand 10 cm under the shoulder top is in the volume (H3 capsule -10 cm)");
  {p::Vec3 m2{};const auto low=in.shoulders[0]-up*.10-right*.05;check(p::shoulder_holster_closest131(in,bow,low,m2)&&m::length(low-m2)<rShoulder,"bow: that hand is inside the bow zone radius");}
  // -- shield over the LEFT shoulder, knife in the chest pouch: plain spheres, also grabs
  p::Vec3 sm{};p::holster_mount126(in,shield,sm);check(p::holster_grab_hs(in,shield,sm+forward*.05,sm,rShoulder),"shield: closed hand at the left shoulder is a grab");
  p::Vec3 km{};p::holster_mount126(in,knife,km);check(p::holster_grab_hs(in,knife,km,km,rBelt),"knife: closed hand at the chest pouch is a grab");
  // -- negatives: the fist keeps its hand
  const auto guard=chest+forward*.30+up*.15;
  check(!p::holster_grab_hs(in,bow,guard,bm,rShoulder)&&!p::holster_grab_hs(in,pole,guard,pm,rShoulder),"guard in front of the face stays a fist");
  check(!p::holster_grab_hs(in,bow,in.shoulders[0]+forward*.30,bm,rShoulder),"forward punch from the shoulder stays a fist");
  check(!p::draw_volume_ok_hs2(in,bow,p::chest_knife_pouch_k32(in))&&!p::draw_volume_ok_hs2(in,pole,p::chest_knife_pouch_k32(in)),"chest knife pouch is not in the bow/back volume");
  check(!p::draw_volume_ok_hs2(in,pole,in.shoulders[0]+forward*.10),"back weapon never from in front of the shoulder");
  p::Profile none{};none.classId=11;check(!p::holster_grab_hs(in,none,bowReach,bm,rShoulder),"non-holster item never grabs");
  check(!p::holster_grab_hs(in,bow,m::Vec3{NAN,0,0},bm,rShoulder)&&!p::holster_grab_hs(in,bow,bowReach,bm,NAN),"non-finite refuses");
 }
 std::printf("holster_history_hs %u tests, %u failures\n",tests,failures);return failures?1:0;}
