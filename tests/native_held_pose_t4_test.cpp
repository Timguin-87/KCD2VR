#include "../src/vrik/native_held_pose_t4.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
int main(){
 namespace h=native_held_pose_t4;namespace b=native_weapon_body_t4;namespace p=physical;
 b::Input source{};source.owner={1,2,3,4,5,6,7,8,9,10,11,0};source.colliderLease=12;source.trackingEpoch=13;source.cancelGeneration=14;
 b::Achieved a{};a.owner=source.owner;a.colliderLease=12;a.trackingEpoch=13;a.cancelGeneration=14;
 a.worldGeneration=15;a.bodyGeneration=16;a.bodyId=17;a.state=b::State::Active;a.poseValid=true;a.colliderRetired=false;
 a.sample=20;a.sourceTick=995;a.observedTick=1000;a.weaponWorld.p={3,.2,0};
 p::Input in{};in.haveWorld=in.allowed=in.tracked[0]=true;in.world.p={3,0,0};in.sample=21;in.tick=1000;in.reach[0]=1;
 std::array<p::Profile,2> profiles{};auto& w=profiles[0];w.key=1;w.generation=2;w.entity=11;w.primary=w.geometry=true;w.grip[0].p={0,.1,0};
 p::Output out{};p::State state{};
 assert(h::apply(source,a,1000,in,profiles,out,state));
 assert(out.solverOwnedT4[0]&&std::abs(out.hands[0].p.y-.3)<1e-10&&out.solverSourceTickT4==995&&!in.dynamics);
 assert(!p::attack_authority55c1(in,out,0)&&out.commandCount==0&&!out.displayLatched[0]);
 for(unsigned cause=0;cause<13;++cause){auto bad=a;auto ask=in;auto profile=profiles;
  switch(cause){case 0:bad.bodyGeneration=0;break;case 1:bad.owner.request++;break;case 2:bad.colliderLease++;break;
   case 3:bad.trackingEpoch++;break;case 4:bad.cancelGeneration++;break;case 5:bad.sourceTick=919;break;
   case 6:bad.observedTick=1001;break;case 7:bad.sample=22;break;case 8:bad.state=b::State::Creating;break;
   case 9:bad.colliderRetired=true;break;case 10:ask.reach[0]=.1;break;case 11:profile[0].generation++;break;case 12:bad.poseValid=false;break;}
  assert(!h::apply(source,bad,1000,ask,profile,out,state));
 }
 auto recycled=a;recycled.bodyGeneration++;assert(!h::sameReceipt(a,recycled));
 // H24 connected achieved-pose posture path; contact/damage remains unknown.
 in={};in.haveWorld=in.allowed=in.tracked[0]=in.intentTracked[0]=true;in.sample=30;in.tick=1100;in.epoch=source.owner.request;in.bodyEpoch=source.owner.epoch;
 in.reach[0]=3;in.worldScale=1;in.positionGuard176=in.naturalCombat117=in.coherentMelee119=true;
 in.guardEnterDwellMs178r29=20;in.guardEnterMaxSpeed178r29=2.5;in.guardExitSpeed178r45=2;
 in.shoulders[0]={.15,0,1.3};in.shoulders[1]={-.15,0,1.3};
 w.classId=4;w.tip={0,0,1};w.axis={0,0,1};w.grip[0].p={};w.guardFaceReadyH24=true;w.guardFaceNormalH24={0,1,0};
 a.weaponWorld.p={0,.35,1.1};a.sourceTick=a.observedTick=in.tick;a.sample=30;
 in.hands[0]=in.intentHands[0]={a.weaponWorld.q,a.weaponWorld.p};state={};
 assert(h::apply(source,a,in.tick,in,profiles,out,state)&&!out.guard117&&in.positionGuard176&&!in.attacks);
 for(int i=0;i<4;++i){in.tick+=10;++in.sample;++a.sample;a.sourceTick=a.observedTick=in.tick;
  assert(h::apply(source,a,in.tick,in,profiles,out,state));}
 assert(out.guard117&&out.guardBySide30c1[0]&&out.solverOwnedT4[0]&&!out.contactKnown[0]&&out.commandCount==0);
 assert(!p::attack_authority55c1(in,out,0)); // Physics pose does not become synthetic RPG contact.
 // A repeated physical receipt cannot hide fresh tracked forward intention.
 in.tick+=10;++in.sample;a.observedTick=in.tick;in.hands[0].p.y+=.08;in.intentHands[0].p.y+=.08;
 assert(h::apply(source,a,in.tick,in,profiles,out,state)&&!out.guard117);
 in.hands[0].p.y-=.08;in.intentHands[0].p.y-=.08;
 // Edge-on intended/achieved blade releases native posture despite identical tip.
 a.weaponWorld.q={0,0,.7071067811865475,.7071067811865475};in.hands[0].q=in.intentHands[0].q=a.weaponWorld.q;
 in.tick+=10;++in.sample;++a.sample;a.sourceTick=a.observedTick=in.tick;
 assert(h::apply(source,a,in.tick,in,profiles,out,state)&&!out.guard117);
 // Reacquisition cannot inherit old dwell when the body incarnation changes.
 a.weaponWorld.q={};in.hands[0].q=in.intentHands[0].q={};++a.bodyGeneration;
 in.tick+=10;++in.sample;++a.sample;a.sourceTick=a.observedTick=in.tick;
 assert(h::apply(source,a,in.tick,in,profiles,out,state)&&!out.guard117);
 // Repeating an achieved receipt cannot earn the20ms physical-sample dwell.
 for(int i=0;i<3;++i){in.tick+=10;++in.sample;a.observedTick=in.tick;assert(h::apply(source,a,in.tick,in,profiles,out,state)&&!out.guard117);}
 // Raw hand pushing quickly while the body stays still cannot enter quiet guard.
 in.tick+=10;++in.sample;++a.sample;a.sourceTick=a.observedTick=in.tick;
 in.hands[0].p.y+=.06;in.intentHands[0].p.y+=.06;
 assert(h::apply(source,a,in.tick,in,profiles,out,state)&&!out.guard117);
 // A fresh still native body with an unintended tracked pose outside front is ready.
 in.hands[0].p.x=in.intentHands[0].p.x=1.2;
 in.tick+=10;++in.sample;++a.sample;a.sourceTick=a.observedTick=in.tick;
 assert(h::apply(source,a,in.tick,in,profiles,out,state)&&!out.guard117);
 std::puts("native_held_pose_t4: achieved pose, source-age/ABA refusals, real physical posture/dwell/intent-pressure and no synthetic RPG contact PASS");
}
