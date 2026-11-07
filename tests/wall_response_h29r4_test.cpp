#include "vrik/weapon_sweep112.h"
#include "vrik/contact_budget.h"
#include <cstdio>
#include <vector>
using namespace kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
static unsigned checks=0,failures=0;
static void ck(bool value,const char* name){++checks;failures+=!value;std::printf("%s %s\n",value?"PASS":"FAIL",name);}
struct PlaneH4 {Vec3 normal{};double distance=0;}; // normal points into the allowed half-space
struct SlabH4 {double low=0,high=0;}; // world X slab, used only by the explicit ray leaf
struct WorldH4 {
 std::vector<PlaneH4> planes;std::vector<SlabH4> slabs;unsigned rays=0,sweeps=0;int failRay=0;bool unknown=false;
 Pose transform{};double scale=1;unsigned rayLimit=18,workUs=1;
 kcd2vr::npc_contact172::Witness witness{};
 kcd2vr::contact::Budget budget{};
 void begin(){rays=sweeps=0;budget.begin_work(rayLimit,1000);}
 Contact103 ray(Vec3 a,Vec3 b,double margin){
  a=point(transform,a*scale);b=point(transform,b*scale);margin*=scale;
  ++rays;Contact103 result{};
  if(!budget.take(rays*(workUs+1))||unknown||(failRay>0&&int(rays)==failRay)){
   result.known=false;result.fraction=0;result.coverage169r2.reason=kcd2vr::sweep169r2::Reason::RayCap;return result;}
  budget.finish(rays*(workUs+1)+workUs);
  const double length=pm::length(b-a);if(length<.0001)return result;
  const auto accept=[&](Vec3 normal,double hit){
   const double allowed=std::clamp(hit-margin/length,0.,1.);
   if(allowed<result.fraction){result.fraction=allowed;result.plane=true;result.normal=pm::rotate(pm::conjugate(transform.q),normal);
    result.point=point(inverse(transform),a+(b-a)*hit)*(1./scale);result.native172=witness;}
  };
  for(const auto& plane:planes){
   const double from=pm::dot(a,plane.normal)-plane.distance,to=pm::dot(b,plane.normal)-plane.distance;
   if(from>=-1e-9&&to<from&&to<=0){
    const double hit=std::clamp(from/(from-to),0.,1.);
    accept(plane.normal,hit);
   }
  }
  for(const auto& slab:slabs){
   // The real adapter has no proved overlap/depenetration-depth query. An
   // inside-solid origin is explicitly UNKNOWN here, never invented clearance.
   if(a.x>slab.low&&a.x<slab.high){result={};result.known=false;result.fraction=0;return result;}
   if(a.x<=slab.low&&b.x>slab.low)accept({-1,0,0},(slab.low-a.x)/(b.x-a.x));
   if(a.x>=slab.high&&b.x<slab.high)accept({1,0,0},(slab.high-a.x)/(b.x-a.x));
  }
  return result;
 }
 Contact103 operator()(const Profile& p,Pose a,Pose b,bool acquire){++sweeps;return sweep_weapon112(p,a,b,acquire,[&](Vec3 x,Vec3 y,double skin){return ray(x,y,skin);});}
};
struct RigH4 {
 Input in{};State state{};std::array<Profile,2> profiles{};Output out{};WorldH4 world{};
 RigH4(){in.epoch=in.bodyEpoch=1;in.tick=1000;in.timeNs=1000000000;in.allowed=true;
  in.controllerAnchor119=in.physicalCombat177=in.contact=in.tracked[0]=in.intentTracked[0]=true;in.attacks=in.dynamics=false;
  in.grip[0]=1;in.reach[0]=1.3;in.shoulders[0]={0,0,1};in.shoulders[1]={-.2,0,1};
  in.hands[0].p={.2,.2,1};in.intentHands[0]=in.hands[0];
  auto& p=profiles[0];p.side=0;p.key=p.generation=1;p.classId=4;p.primary=p.geometry=p.nativeSeed=true;
  p.tip={0,.6,0};p.axis={0,1,0};p.nativeWeapon=in.hands[0];profiles[1].side=1;
  next();}
 void next(){++in.sample;in.tick+=10;in.timeNs+=10000000;world.begin();in.intentHands[0]=in.hands[0];
  if(in.modelIntentSourceH30[0])in.modelIntentHandsH30[0]=in.hands[0];
  out=step(state,in,profiles,[&](const Profile& p,Pose a,Pose b,bool acquire){return world(p,a,b,acquire);});}
 void worldFrame(double scale){in.haveWorld=state.haveWorld=true;in.worldScale=state.worldScale=world.scale=scale;
  in.world=state.world=world.transform={pm::from_rotation_vector({0,0,.7}),{4,-3,2}};}
 void plane(Vec3 normal,double distance){const auto n=pm::rotate(world.transform.q,normal);
  world.planes.push_back({n,pm::dot(n,world.transform.p)+distance*world.scale});}
 bool safe()const {const auto& p=profiles[0];const auto pose=out.weapons[0];
  for(int i=0;i<=32;++i){const auto v=point(world.transform,point(pose,p.grip[0].p+(p.tip-p.grip[0].p)*(i/32.))*world.scale);
   for(const auto& wall:world.planes)if(pm::dot(v,wall.normal)<wall.distance-1e-7)return false;
   for(const auto& slab:world.slabs)if(v.x>slab.low+1e-7&&v.x<slab.high-1e-7)return false;}
  return finite(pose);}
};
int main(){
 RigH4 slide;slide.world.planes={{{-1,0,0},-.30}};
 slide.in.hands[0].p.x=.45;slide.next();
 const auto first=slide.out.weapons[0];
 ck(slide.out.blocked[0]&&first.p.x<=.300001&&first.p.x>=.28,"actual default hard sweep stops before the measured wall");
 slide.in.hands[0].p.z+=.08;slide.next();
 std::printf("REPRO wallSlide=%d requestedZ=%.6f allowedZ=%.6f heldX=%.6f rays=%u sweeps=%u\n",int(slide.in.wallSlide58),slide.in.hands[0].p.z,slide.out.weapons[0].p.z,slide.out.weapons[0].p.x,slide.world.rays,slide.world.sweeps);
 ck(slide.out.weapons[0].p.z-first.p.z>.075,"default contact follows a checked tangent while inward controller pressure remains constrained");
 RigH4 retreat;retreat.world.planes={{{-1,0,0},-.30}};retreat.in.hands[0].p.x=.45;retreat.next();
 const auto pinned=retreat.out.weapons[0];retreat.in.hands[0].p.x=.25;retreat.in.hands[0].q=pm::from_rotation_vector({0,0,-.8});retreat.next();
 std::printf("REPRO retreat requestedX=%.6f allowedX=%.6f initialX=%.6f requestedRad=.8 allowedRad=%.6f rays=%u\n",retreat.in.hands[0].p.x,retreat.out.weapons[0].p.x,pinned.p.x,pm::angle(retreat.out.weapons[0].q,{}),retreat.world.rays);
 ck(retreat.out.weapons[0].p.x<.255,"same-sample checked retreat releases immediately despite a simultaneous wall-directed rotation");
 ck(slide.out.commandCount==0&&retreat.out.commandCount==0&&!slide.out.impactRecord178r30[0].valid&&!retreat.out.impactRecord178r30[0].valid,"passive wall response creates no attack or target contact");
 for(double scale:{.25,1.,3.}){
  RigH4 corner;corner.worldFrame(scale);corner.plane({-1,0,0},-.30);corner.plane({0,0,-1},-1.10);
  corner.in.hands[0].p.x=.45;corner.next();const auto before=corner.out.weapons[0];
  corner.in.hands[0].p.y+=.15;corner.in.hands[0].p.z+=.25;corner.next();
  ck(corner.safe()&&corner.out.weapons[0].p.y-before.p.y>.14&&corner.out.weapons[0].p.z<=1.100001,
     "actual current two-plane manifold follows the corner crease under rotated translated nonunit world frame");
  ck(corner.world.budget.used<=18&&corner.world.budget.spent<=1000&&corner.world.sweeps<=3&&corner.out.checkedPath60[0].valid&&corner.out.checkedPath60[0].count<=4,
     "corner keeps every accepted segment and the unchanged shared18-ray/1ms budget");
  ck(corner.out.commandCount==0&&!corner.out.impactRecord178r30[0].valid,"corner projection never invents attack or actor impact");
  const auto queries=corner.world.rays;const auto repeated=step(corner.state,corner.in,corner.profiles,[&](const Profile& p,Pose a,Pose b,bool c){return corner.world(p,a,b,c);});
  ck(queries==corner.world.rays&&pm::length(repeated.weapons[0].p-corner.out.weapons[0].p)<1e-10,"same source sample never integrates or queries wall correction twice");
 }
 for(bool reverse:{false,true}){
  RigH4 thin;thin.world.slabs={{.30,.302}};
  if(reverse){thin.in.hands[0].p.x=.65;thin.profiles[0].nativeWeapon=thin.in.hands[0];thin.state={};thin.next();}
  for(int i=0;i<5;++i){thin.in.hands[0].p.x+=reverse?-.08:.08;thin.next();ck(thin.safe(),"2mm solid slab cannot be crossed by a swept blade from either side");}
  const auto pinnedThin=thin.out.weapons[0];thin.in.hands[0].p.x=reverse?.65:.20;thin.next();
  ck(thin.safe()&&pm::length(thin.out.weapons[0].p-thin.in.hands[0].p)<1e-8&&pm::length(thin.out.weapons[0].p-pinnedThin.p)>.05,"thin-wall contact releases on ordinary retreat without a dwell or equip cycle");
 }
 {RigH4 turn;turn.plane({-1,0,0},-.30);bool allSafe=true;
  for(int i=1;i<=14;++i){turn.in.hands[0].q=pm::from_rotation_vector({0,0,-.05*i});turn.next();allSafe=allSafe&&turn.safe()&&turn.world.budget.used<=18;}
  ck(allSafe&&pm::angle(turn.out.weapons[0].q,turn.in.hands[0].q)>.1,"stationary grip rotation retains a checked angular prefix rather than rotating the tip through a wall");
  turn.in.hands[0].p.x=.15;turn.in.hands[0].q={};turn.next();
  ck(turn.safe()&&pm::length(turn.out.hands[0].p-turn.in.hands[0].p)<1e-8&&pm::angle(turn.out.weapons[0].q,{})<1e-8,"rotation away and ordinary retreat regain the requested pose through checked paths");
 }
 for(unsigned rayLimit:{1u,3u,7u,18u}){
  RigH4 busy;busy.world.rayLimit=rayLimit;busy.plane({-1,0,0},-.30);busy.in.hands[0].p.x=.45;busy.next();
  const auto seed=busy.state.weapon[0].weapon;busy.in.hands[0].p.z+=.1;busy.next();
  ck(busy.safe()&&busy.world.budget.used<=rayLimit&&busy.world.sweeps<=3,"exhausted shared budget never authorizes an unchecked tangent or issues more native rays");
  busy.world.rayLimit=18;busy.in.hands[0].p.x=.20;busy.next();
  ck(busy.out.active[0]&&busy.out.owned[0]&&busy.out.contactKnown[0]&&pm::length(busy.out.hands[0].p-busy.in.hands[0].p)<1e-8&&busy.profiles[0].generation==1,
     "fresh complete query recovers the same equipped generation after budget refusal without reholstering");
 }
 {RigH4 latency;latency.plane({-1,0,0},-.30);latency.world.workUs=400;latency.in.hands[0].p.x=.45;latency.next();
  ck(latency.world.budget.used<=3&&latency.world.budget.spent==1000&&!latency.out.contactKnown[0],"existing1ms work budget is respected when individual ray work is expensive");}
 {RigH4 gap;gap.plane({-1,0,0},-.30);gap.in.hands[0].p.x=.45;gap.next();const auto original=gap.state.weapon[0].weapon;
  gap.world.unknown=true;gap.in.contactSuspended59c1=true;gap.next();
  ck(gap.state.weapon[0].acquiring124&&gap.out.handlingOnly55c1[0]&&!gap.out.contactKnown[0]&&pm::length(gap.state.weapon[0].weapon.p-original.p)<1e-10,
     "actual UNKNOWN handling keeps the same checked seed separate from unqualified display");
  gap.world.unknown=false;gap.in.contactSuspended59c1=false;gap.in.hands[0].p.x=.25;gap.in.hands[0].q=pm::from_rotation_vector({0,0,-.8});gap.next();
  ck(gap.out.active[0]&&gap.out.owned[0]&&!gap.out.handlingOnly55c1[0]&&gap.out.weapons[0].p.x<.255&&gap.state.weapon[0].key==1&&gap.state.weapon[0].generation==1,
     "same-item reacquisition regains checked outward translation even if requested rotation still touches the wall");
  gap.in.hands[0].q={};gap.in.hands[0].p.x=.2;gap.next();
  ck(gap.out.contactKnown[0]&&pm::length(gap.out.hands[0].p-gap.in.hands[0].p)<1e-8,"recovered collision stays live on following samples without release or re-equip");
 }
 for(int body=0;body<5;++body){RigH4 identity;identity.world.witness.mask=kcd2vr::npc_contact172::BodiesAndWorld;
  identity.world.witness.body=static_cast<kcd2vr::npc_contact172::Body>(body);identity.world.witness.collider=7;
  identity.world.witness.vft=body==1?0x3a578c0:0;identity.plane({-1,0,0},-.30);
  identity.in.hands[0].p.x=.45;identity.next();const auto old=identity.out.weapons[0];identity.in.hands[0].p.z+=.08;identity.next();
  ck(body==1?identity.out.weapons[0].p.z-old.p.z>.075:identity.out.weapons[0].p.z-old.p.z<.01,
     "new geometric response is restricted to the current exact base-world witness, never NPC rigid articulated or unclassified bodies");}
 {RigH4 moved;moved.plane({-1,0,0},-.30);moved.in.hands[0].p.x=.45;moved.next();
  moved.world.planes.clear();moved.next();ck(moved.out.contactKnown[0]&&!moved.out.blocked[0]&&pm::length(moved.out.hands[0].p-moved.in.hands[0].p)<1e-8,
     "removed current world surface cannot leave a cached plane or permanent weapon clamp");
  moved.plane({0,0,-1},-1.05);moved.in.hands[0].p.z=1.15;moved.next();moved.in.hands[0].p.x=.55;moved.next();
  ck(moved.safe()&&moved.out.weapons[0].p.x>.54,"changed current surface normal replaces the old one instead of retaining a former wall owner");}
 {RigH4 owned;owned.world.witness.mask=kcd2vr::npc_contact172::BodiesAndWorld;owned.world.witness.body=kcd2vr::npc_contact172::Body::Physical;
  owned.world.witness.vft=0x3a578c0;owned.world.witness.collider=9;owned.world.witness.ownerRead=true;owned.world.witness.owner=10;owned.world.witness.entityId=11;
  owned.plane({-1,0,0},-.30);owned.in.hands[0].p.x=.45;owned.next();const auto p=owned.out.weapons[0];owned.in.hands[0].p.z+=.08;owned.next();
  ck(owned.out.weapons[0].p.z-p.p.z<.01,"an owned base body is not reclassified as static world for mandatory geometric response");}
 {RigH4 overlap;overlap.world.slabs={{.19,.21}};overlap.in.contactSuspended59c1=true;overlap.in.hands[0].p.z+=.03;overlap.next();
  ck(!overlap.out.contactKnown[0]&&overlap.out.handlingOnly55c1[0]&&!attack_authority55c1(overlap.in,overlap.out,0),
     "unproved start overlap is UNKNOWN and cannot become collision or attack authority");
  overlap.world.slabs.clear();overlap.in.contactSuspended59c1=false;overlap.next();
  ck(overlap.out.contactKnown[0]&&overlap.out.active[0]&&!overlap.out.handlingOnly55c1[0]&&overlap.state.weapon[0].generation==1,
     "fresh clearance after a moved overlapping object restores the same weapon without re-equipping");}
 {RigH4 raw;raw.in.attacks=raw.in.coherentMelee119=true;select_forward_motion36(raw.in,false,true);
  raw.in.modelIntentSourceH30[0]=raw.in.modelIntentTrackedH30[0]=true;
  for(int i=0;i<15;++i)raw.next();raw.plane({-1,0,0},-.30);raw.in.hands[0].p.x=.45;raw.next();
  raw.world.rayLimit=1;raw.in.hands[0].p.z+=.08;raw.next();raw.world.rayLimit=18;
  const auto requested=raw.in.hands[0];unsigned actions=0;bool remainsSafe=true;
  for(int i=0;i<16;++i){raw.next();remainsSafe=remainsSafe&&raw.safe();for(unsigned j=0;j<raw.out.commandCount;++j){const auto k=raw.out.commands[j].kind;
    actions+=k==CommandKind::Prepare||k==CommandKind::Slash||k==CommandKind::Thrust;}}
  ck(normal_hybrid_intent_h31(raw.in)&&remainsSafe&&actions==0&&!raw.out.impactRecord178r30[0].valid&&pm::length(raw.in.hands[0].p-requested.p)<1e-10,
     "actual public HYBRID raw source stays stationary while checked corrective motion never becomes new prepare strike or target energy");}
 {RigH4 lost;lost.world.unknown=true;lost.next();lost.in.tracked[0]=false;lost.next();
  ck(!lost.out.owned[0]&&!lost.state.weapon[0].ready,"true tracking loss still retires weapon ownership without extending grace");
  lost.world.unknown=false;lost.in.tracked[0]=true;++lost.in.bodyEpoch;++lost.in.epoch;lost.profiles[0].generation=2;lost.profiles[0].nativeWeapon=lost.in.hands[0];lost.next();
  ck(lost.out.owned[0]&&lost.state.weapon[0].generation==2&&lost.state.bodyEpoch==lost.in.bodyEpoch,"new body request and item generation reacquire only through their current native seed");}
 std::printf("wall_response_h29r4=%u/%u production_step=YES production_sweep=YES native_calls=NONE\n",checks-failures,checks);
 return failures?1:0;
}
