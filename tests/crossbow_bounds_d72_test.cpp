// Actual kernel and production sweep: copied native bounds, exact first owner,
// transverse obstacles and shared query budget. No headset/native mesh claim.
#include "vrik/weapon_sweep112.h"
#include "vrik/contact_budget.h"
#include <cstdio>
#include <limits>
using namespace kcd2vr::physical;
static unsigned checks=0,failed=0;
static void check(bool value,const char* what){++checks;if(!value){++failed;std::printf("FAIL %s\n",what);}}
static Profile profile(int cls=10,int side=1){
 Profile p{};p.key=19;p.generation=3;p.entity=47;p.classId=cls;p.side=side;
 p.primary=p.geometry=p.nativeSeed=p.diagnosticBounds59c6=p.initialAcquireSweepD44=true;
 p.diagnosticBoundsMin59c6={-.45,-.25,-.04};p.diagnosticBoundsMax59c6={.45,.8,.04};p.tip={0,.8,0};return p;
}
int main(){
 for(int cls:{10,14,15}){
  auto p=profile(cls);unsigned rays=0;bool bothEnds=false;
  const auto clear=[&](Vec3 a,Vec3 b,double){++rays;bothEnds|=a.x==-.45&&b.x==.45;return Contact103{};};
  check(initial_target_clear_d70(p,Pose{},clear)&&rays==54&&bothEnds,"every crossbow class checks measured prod and stock on all axes");
  check(initial_target_ray_count_d72(p)==rays,"production receipt counts the actual broad target probes");
 }
 for(int axis=0;axis<3;++axis){
  auto p=profile();if(axis==1){p.diagnosticBoundsMin59c6={-.45,-.04,-.25};p.diagnosticBoundsMax59c6={.45,.04,.8};}
  if(axis==2){p.diagnosticBoundsMin59c6={-.04,-.45,-.25};p.diagnosticBoundsMax59c6={.04,.45,.8};}
  std::array<Vec3,9> f{};check(crossbow_features_d72(p,f),"native bounds determine transverse plane without model axis guess");
  const auto lo=p.diagnosticBoundsMin59c6,hi=p.diagnosticBoundsMax59c6;bool endpoints=false;
  for(const auto v:f)endpoints|=(axis==0?v.x==hi.x&&v.y==hi.y:axis==1?v.x==hi.x&&v.z==hi.z:v.y==hi.y&&v.z==hi.z);
  check(endpoints,"broad motion features contain actual outer transverse endpoint");
 }
 for(int bad=0;bad<10;++bad){
  auto p=profile();switch(bad){case 0:p.diagnosticBounds59c6=false;break;case 1:p.geometry=false;break;
   case 2:p.diagnosticBoundsMax59c6.x=p.diagnosticBoundsMin59c6.x;break;
   case 3:p.diagnosticBoundsMin59c6.y=2;break;case 4:p.diagnosticBoundsMax59c6.z=5;break;
   case 5:p.diagnosticBoundsMax59c6.x=std::numeric_limits<double>::quiet_NaN();break;
   case 6:p.side=2;break;case 7:p.nativeSeed=false;break;case 8:p.entity=0;break;case 9:p.initialAcquireSweepD44=false;break;}
  unsigned rays=0;check(!initial_target_clear_d70(p,Pose{},[&](Vec3,Vec3,double){++rays;return Contact103{};})&&rays==0,"class alone, malformed bounds or missing identity never grant target clearance");
 }
 // A wall intersects only the prod's outer end at its forward edge. Every old
 // grip-to-tip shaft probe has x near zero and misses this concrete obstacle.
 const auto prodWall=[](Vec3 a,Vec3 b,double){Contact103 c;
  if(std::abs(a.y-.8)<.0001&&std::abs(a.z)<.0001&&std::abs(a.x-b.x)>.0001&&(a.x-.3)*(b.x-.3)<=0){
   c.fraction=std::clamp((.3-a.x)/(b.x-a.x),0.,1.);c.plane=true;c.normal={1,0,0};c.point={.3,.8,0};}
  return c;};
 auto p=profile();check(!initial_target_clear_d70(p,Pose{},prodWall),"actual prod-only obstacle prevents instantaneous broad placement");
 auto thin=p;thin.classId=4;check(initial_target_clear_d70(thin,Pose{},prodWall),"counterexample confirms former shaft-only test missed the prod obstacle");
 for(int cls:{10,14,15}){
  p=profile(cls);p.initialAcquireSweepD44=false;Pose from{},to{};to.p.y=.5;unsigned rays=0;
  const auto motion=[&](Vec3 a,Vec3 b,double){++rays;Contact103 c;
   if(std::abs(a.x)>.3&&std::abs(a.y-b.y)>.0001&&(a.y-1)*(b.y-1)<=0){c.fraction=std::clamp((1-a.y)/(b.y-a.y),0.,1.);c.plane=true;c.normal={0,-1,0};c.point={a.x,1,a.z};}return c;};
  const auto hit=sweep_weapon112(p,from,to,false,motion);
  check(hit.known&&hit.plane&&hit.fraction<1&&std::abs(hit.feature.x)>.3&&rays<=18,"ordinary crossbow path retains transverse contact instead of falling back to thin shaft");
 }
 // Exact kernel handover for both main-hand assignments, including native
 // left-held crossbows. No button, inventory action or strike is generated.
 for(int side:{0,1})for(int cls:{10,14,15}){
  State state{};Input in{};in.allowed=in.contact=in.controllerAnchor119=true;
  in.epoch=in.bodyEpoch=7;in.tick=1000;in.timeNs=1000000000;in.sample=1;
  in.tracked[side]=in.intentTracked[side]=true;in.hands[side].p={.2,.6,1.3};in.intentHands[side]=in.hands[side];
  in.shoulders[side]={.2,0,1.3};in.reach[side]=1.6;
  std::array<Profile,2> pp{};pp[0].side=0;pp[1].side=1;pp[side]=profile(cls,side);pp[side].nativeWeapon.p={.2,.1,1.3};
  unsigned rays=0;const auto ray=[&](Vec3,Vec3,double){++rays;return Contact103{};};
  const auto out=step(state,in,pp,[&](const Profile& q,Pose a,Pose b,bool acquire){
   if(!acquire&&initial_target_clear_d70(q,b,ray)){Contact103 c;c.targetPlacementD70=true;return c;}
   return sweep_weapon112(q,a,b,acquire,ray);});
  check(out.initialAcquireSweepD44[side]&&out.contact119[side].targetPlacementD70&&rays==54,"actual new owner uses broad first-binding correction");
  check(out.owned[side]&&m::length(out.hands[side].p-in.hands[side].p)<1e-8&&out.commandCount==0&&!out.impactRecord178r30[side].valid,"first cleared crossbow binds exact requested grip without synthesizing a combat action");
 }
 for(unsigned cap:{0u,17u,53u}){
  kcd2vr::contact::Budget budget;budget.begin_work(cap,1000);std::uint64_t clock=0;unsigned rays=0;
  auto limited=[&](Vec3,Vec3,double){Contact103 c;if(!budget.take(clock)){c.known=false;c.fraction=0;return c;}
   ++rays;++clock;budget.finish(clock);return c;};
  p=profile();check(!initial_target_clear_d70(p,Pose{},limited)&&rays==cap,"incomplete shared allowance cannot certify broad placement");
 }
 kcd2vr::contact::Budget shared;shared.begin_work(18,1000);unsigned rays=0;std::uint64_t clock=0;
 {
  kcd2vr::contact::AcquisitionBurstD45 burst(shared,true);
  const auto query=[&](Vec3,Vec3,double){Contact103 c;if(!shared.take(clock)){c.known=false;c.fraction=0;return c;}
   ++rays;clock+=20;shared.finish(clock);return c;};
  p=profile();const bool clear=initial_target_clear_d70(p,Pose{},query);
  Pose to{};to.q=m::from_rotation_vector({0,0,1});const auto follow=sweep_weapon112(p,Pose{},to,false,query);
  check(!clear&&(!follow.known||follow.limited115)&&rays<=50&&shared.used<=72&&shared.spent==1000,"target and fallback motion share original one-millisecond ceiling and cannot replenish it");
 }
 std::printf("crossbow_bounds_d72: %u/%u PASS; bounded native-bounds proxy, no full mesh/runtime claim\n",checks-failed,checks);return failed?1:0;
}
