#define main prior_reference_main
#include "held_motion_d67_test.cpp"
#undef main
#include "vrik/grip_binding59c10r3.h"
#include <cstring>
struct SocketD70 {bool valid=true;std::uint64_t key=11,generation=3;void* character=(void*)0x1000;int joint=23;Pose relative{};float scale[3]{1,1,1};};
struct GripD70 {bool heldReadyR27[2]{true,true};std::uint64_t heldBindingEpochR27=7;SocketD70 heldBindingR27{};float heldScaleR27[3]{1,1,1};};
int main(){
 const auto p=[](){Profile p{};p.key=11;p.generation=3;p.entity=42;p.primary=p.geometry=p.nativeSeed=p.initialAcquireSweepD44=true;p.classId=4;p.side=0;p.tip={0,.8,0};return p;}();
 for(int bad:{0,1,2}){unsigned rays=0;auto query=[&](Vec3,Vec3,double){++rays;Contact103 c;if(bad&&rays==11){c.fraction=0;if(bad==1){c.plane=true;c.normal={1,0,0};}else c.known=false;}return c;};
  ck(initial_target_clear_d70(p,Pose{},query)==(bad==0),"current complete target proxy refuses hit or unknown");ck(rays<=22,"target check bounded at22 rays");}
 for(int invalid=0;invalid<8;++invalid){auto x=p;switch(invalid){case 0:x.initialAcquireSweepD44=false;break;case 1:x.nativeRejoinSweepR28=true;break;case 2:x.entity=0;break;case 3:x.generation=0;break;case 4:x.key=0;break;case 5:x.geometry=false;break;case 6:x.nativeSeed=false;break;case 7:x.side=2;break;}unsigned rays=0;
  ck(!initial_target_clear_d70(x,Pose{},[&](Vec3,Vec3,double){++rays;return Contact103{};})&&rays==0,"noninitial/foreign/incomplete identity cannot run new query");}
 // Real kernel + real sweep. Wall intercepts irrelevant authored transport,
 // while the current target shaft is fully beyond it. No attack is invented.
 for(bool repair:{false,true})for(int targetMode:{0,1,2}){
  State state{};Input in{};in.allowed=in.controllerAnchor119=in.contact=true;in.epoch=in.bodyEpoch=7;in.tick=1000;in.sample=1;in.timeNs=1000000000;in.tracked[0]=in.intentTracked[0]=true;
  in.hands[0].p={.2,.6,1.3};in.intentHands[0]=in.hands[0];in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=1.5;
  std::array<Profile,2> profiles{};profiles[0]=p;profiles[0].nativeWeapon.p={.2,.1,1.3};profiles[1].side=1;
  unsigned queries=0;const double wall=targetMode==1?.9:.3;
  const auto ray=[&](Vec3 a,Vec3 b,double){++queries;Contact103 c;
   if(targetMode==2){c.known=false;c.fraction=0;return c;}
   if((a.y-wall)*(b.y-wall)<=0&&std::abs(a.y-b.y)>1e-9){c.fraction=std::clamp((wall-a.y)/(b.y-a.y),0.,1.);c.plane=true;c.point={a.x,wall,a.z};c.normal={0,a.y<wall?-1.:1.,0};}return c;};
  const auto out=step(state,in,profiles,[&](const Profile& q,Pose a,Pose b,bool acquire){if(repair&&initial_target_clear_d70(q,b,ray)){Contact103 c;c.targetPlacementD70=true;return c;}return sweep_weapon112(q,a,b,acquire,ray);});
  ck(out.commandCount==0,"new draw does not synthesize attack or inventory operation");
  if(targetMode==0){ck(repair?m::length(out.hands[0].p-in.hands[0].p)<1e-8:m::length(out.hands[0].p-in.hands[0].p)>.2,"target check removes native arc displacement only on complete clear");
   if(repair)ck(out.contact119[0].targetPlacementD70&&m::length(state.weapon[0].lastTip-point(out.weapons[0],p.tip))<1e-8,"new owner seeds derivative at target rather than fake traversed arc");}
  else ck(!out.contact119[0].targetPlacementD70,"blocked or unknown target retains legacy acquisition path");
 }
 // Calibrated head frame follows real translation/yaw, ignores native torso.
 std::array<Vec3,2> a{},b{};const Vec3 head{0,0,1.7},right{.2,0,1.4},left{-.2,0,1.4};
 ck(kcd2vr::holster_body_d70::shoulders(head,right,left,head,Quat{},a),"rest body qualifies shoulder zones");
 ck(kcd2vr::holster_body_d70::shoulders(head,right,left,head+Vec3{.4,.2,-.1},m::from_rotation_vector({.8,0,0}),b)&&m::length((b[0]-a[0])-Vec3{.4,.2,-.1})<1e-9,"head translation carried, pitch cannot drag shoulder zone");
 ck(kcd2vr::holster_body_d70::shoulders(head,right,left,head,m::from_rotation_vector({0,0,1.5707963267948966}),b)&&m::length(b[0]-Vec3{0,.2,1.4})<1e-8,"physical yaw turns calibrated shoulder layout");
 {Input in{};in.shoulders[0]=in.torsoShouldersR38[0]=b[0];in.shoulders[1]=in.torsoShouldersR38[1]=b[1];in.torsoShouldersKnownR38=true;Profile bow{};bow.classId=9;bow.side=1;bow.holsterSetR12=1;
 ck(holster_hand126(bow)==1&&m::length(left_shoulder_zone_h42(in)-right_shoulder_bow_zone_r12(in))<1e-12,"bow remains RIGHT shoulder acquired by LEFT hand");}
 GripD70 g{};SocketD70 socket{};using kcd2vr::grip_binding59c10r3::reuseBowD70;
 ck(reuseBowD70(g,socket,7,g.heldScaleR27,1,0,0),"same completed local bow relation reuses accepted grip immediately");
 for(int change=0;change<7;++change){auto s=socket;auto z=g;std::uint64_t epoch=7;double pos=0,angle=0;switch(change){case 0:s.joint++;break;case 1:s.generation++;break;case 2:s.relative.p.x=.1;break;case 3:epoch++;break;case 4:z.heldScaleR27[0]=2;break;case 5:pos=.001;break;case 6:angle=.02;break;}
 ck(!reuseBowD70(z,s,epoch,g.heldScaleR27,1,pos,angle),"changed bow socket/local/scale/epoch must settle again");}
 // Actual persistent contact -> moving clear counterexample. Neither a new
 // target nor an old blocking flag is itself clearance: every candidate queried.
 for(int later:{0,1,2}){cfg::setGripAnchored(true);Fixture f;f.in.contact=true;f.wall=true;
  f.in.hands[0].p.y=.55;f.next();ck(f.out.blocked[0],"initial wall genuinely constrains grip");
  f.wall=later==1;f.unknown=later==2;bool current=true,safe=true;unsigned actions=0;
  for(int i=0;i<12;++i){f.in.hands[0].p.y+=.035;f.next();actions+=f.out.commandCount;
   current&=m::length(f.out.hands[0].p-f.in.hands[0].p)<1e-7;
   safe&=f.out.hands[0].p.y<=.4000001;}
  ck(later==0?current:safe,"current clear rejoins moving controller; wall/unknown keeps checked boundary");
  ck(actions==0,"rejoin motion cannot invent attack or inventory action");
 }
 {cfg::setGripAnchored(true);Fixture f;f.in.contact=true;f.wall=true;f.in.intentTracked[0]=true;
  f.in.attacks=f.in.heldMotionAttacks=f.in.coherentMelee119=f.in.naturalCombat117=f.in.forwardOnly19=true;
  f.in.hands[0].p.y=.55;for(int i=0;i<12;++i)f.next();
  f.wall=false;unsigned actions=0;for(int i=0;i<8;++i){f.next();actions+=f.out.commandCount;}
  ck(actions==0,"stationary actual attack-enabled controller: releasingwall cannot create attack");
  ck(m::length(f.state.weapon[0].linearVelocity)<1e-8&&!f.state.weapon[0].contactRejoinR9,"clear rejoin retires spring without catchup energy");
  const auto prior=f.out;const auto repeated=step(f.state,f.in,f.p,[](const Profile&,Pose,Pose,bool){return Contact103{};});
  ck(m::length(repeated.hands[0].p-prior.hands[0].p)<1e-12&&repeated.commandCount==0,"repeated source cannot repeat rejoin or semantic action");
 }
 // A retained gap is positional correction, never kinetic effort. Seed an
 // already-existing deliberate gesture to exercise the actual impact channel;
 // the fixture does not infer a new attack from contact or the corrective move.
 for(int mode:{0,1,2,3}){cfg::setGripAnchored(true);Fixture f(4);f.in.contact=true;f.wall=true;
  f.in.hands[0].p.y=.7;for(int n=0;n<20;++n)f.next();
  auto& ws=f.state.weapon[0];const auto checked=ws.weapon;
  ws.gesture59c3=8;ws.stroke118.active=true;ws.stroke118.kind=kcd2vr::melee118::Kind::Thrust;ws.lastImpactTick=0;
  f.in.motionOwned59c5=f.in.attacks=f.in.intentTracked[0]=f.in.naturalCombat117=true;
  if(mode==0)f.in.hands[0].p.y+=.0055; // real 0.5m/s inward input, not the 30cm gap
  f.in.tick+=11;f.in.timeNs+=11000000;++f.in.sample;f.in.intentHands[0]=f.in.hands[0];
  const auto out=step(f.state,f.in,f.p,[&](const Profile&,Pose,Pose,bool){Contact103 c;
   if(mode==1){c.known=false;c.fraction=0;}
   else if(mode==2){c.limited115=true;c.fraction=.25;}
   else{c.plane=true;c.fraction=0;c.point={.2,.4,1.3};c.normal={0,-1,0};c.feature=f.p[0].grip[0].p;}return c;});
  const auto& r=out.impactRecord178r30[0];
  if(mode==0){auto expected=r;impact_effort19(expected,f.p[0],0,checked,{0,.5,0},{},1.,f.p[0].grip[0].p);
   ck(r.valid&&r.tick==f.in.tick&&r.gesture59c3==8&&r.effortKnown19&&std::abs(r.speed-.5)<1e-9,"real inward input still publishes current deliberate contact");
   ck(std::abs(r.effortScale19-expected.effortScale19)<1e-9&&std::abs(r.goreCutD56.totalSpeed-.5)<1e-9,"contact effort uses real current input, never accumulated rejoin gap");
   std::printf("D70 gap effort: input %.9f feature %.9f scale %.9f\n",r.speed,r.goreCutD56.totalSpeed,r.effortScale19);
  }else{
   ck(m::length(ws.linearVelocity)<1e-8,"stationary rejoin adds no momentum on UNKNOWN/prefix/wall");
   ck(!r.valid||r.tick!=f.in.tick,"stationary correction never creates fresh impact");
  }
  ck(out.blocked[0]&&out.hands[0].p.y<=(mode==2?.4750001:.4000001),"new wall/unknown/prefix retains its checked spatial boundary");
  ck(ws.contactRejoinR9&&out.commandCount==0,"uncleared gap retains recovery without semantic action");
 }
 std::printf("D70 draw/body %d/%d\n",count-failed,count);return failed?1:0;
}
