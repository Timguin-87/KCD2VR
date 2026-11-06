#include "vrik/weapon_sweep112.h"
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace kcd2vr::physical;
int main(){
 Input in{};State state{};std::array<Profile,2> profiles{};Output out{};
 in.epoch=51;in.bodyEpoch=42;in.sample=1;in.tick=1000;in.timeNs=1000000000;
 in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.grip[0]=1;
 in.dynamics=false;in.contact=true;in.naturalCombat117=in.heldMotionAttacks=in.requirePrimaryGrip114=true;
 in.coherentMelee119=in.controllerAnchor119=true;
 in.shoulders[0]={0,0,1.3};in.shoulders[1]={-.2,0,1.3};in.reach[0]=1.3;
 in.hands[0].p={-.25,.2,1.2};in.intentHands[0]=in.hands[0];
 auto& p=profiles[0];p.key=123;p.generation=456;p.classId=4;p.primary=p.geometry=true;p.axis={0,1,0};p.tip={0,.6,0};p.weight=1.7;
 auto sample=[&]{out=step(state,in,profiles,[](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=true;c.fraction=1.;return c;});};
 auto move=[&](Vec3 d){in.timeNs+=10000000;in.tick=in.timeNs/1000000;++in.sample;in.hands[0].p=in.hands[0].p+d;in.intentHands[0].p=in.intentHands[0].p+d;sample();};
 sample();for(int n=0;n<12;++n)move({});
 in.physicalCombat177=in.constraintDrive129=in.dynamics=true;in.nativeStateKnown178=true;in.nativeAttackBusy178=false;
 in.strikeMinScale178=0;in.strikeMinIntervalMs178=550;
 Command captured{};unsigned commits=0;double measured=0;
 for(int n=0;n<11;++n){move({0,.02,0});for(unsigned j=0;j<out.commandCount;++j){const auto& c=out.commands[j];
   if(c.kind==CommandKind::Thrust||c.kind==CommandKind::Slash){captured=c;++commits;measured=out.strike178.scale;}
   else assert(!c.effortKnown19); // preparations/guards never reserve a full-stroke debit
 }}
 assert(commits==1&&captured.kind==CommandKind::Thrust&&captured.gesture59c3&&captured.effortKnown19);
 assert(captured.key==123&&captured.generation==456&&captured.bodyEpoch==42&&captured.epoch==51);
 assert(captured.effortScale19>0&&std::abs(captured.effortScale19-measured)<1.e-9&&captured.effortMass19>0);
 const auto frozen=captured;
 for(int n=0;n<8;++n)move({}); // later output and world motion cannot rewrite the captured packet
 assert(captured.tick==frozen.tick&&captured.effortScale19==frozen.effortScale19&&captured.gesture59c3==frozen.gesture59c3);
 // Actual contact packet producer: a checked living contact carries energy from
 // before the solver stops the weapon, frozen with its own source sample.
 state={};in.sample=1;in.tick=1000;in.timeNs=1000000000;in.motionOwned59c5=true;
 in.hands[0].p={-.25,.2,1.2};in.intentHands[0]=in.hands[0];in.physicalCombat177=false;
 sample();for(int n=0;n<12;++n)move({});in.physicalCombat177=true;
 bool wall=false;
 auto approach=[&]{out=step(state,in,profiles,[&](const Profile& profile,Pose,Pose,bool){Contact103 c{};
   c.known=true;c.fraction=wall?.5:1.;c.plane=wall;c.normal={0,-1,0};c.point={-.25,.7,1.2};c.feature=profile.tip;
   if(wall){c.native172.mask=kcd2vr::npc_contact172::BodiesAndWorld;c.native172.ownerRead=true;
    c.native172.owner=11;c.native172.collider=12;c.native172.entityId=13;c.native172.part=4;c.native172.body=kcd2vr::npc_contact172::Body::Living;}
   return c;});};
 auto forward=[&]{in.timeNs+=10000000;in.tick=in.timeNs/1000000;++in.sample;
   in.hands[0].p.y+=.02;in.intentHands[0].p.y+=.02;approach();};
 forward();forward();wall=true;forward();
 const auto impact=out.impactRecord178r30[0];
 assert(impact.valid&&impact.sample==in.sample&&impact.tick==in.tick&&impact.gesture59c3&&impact.effortKnown19);
 assert(impact.effortScale19>0&&impact.effortMass19>0&&impact.key==p.key&&impact.generation==p.generation);
 const auto conserved=impact;for(int n=0;n<4;++n)forward();
 assert(conserved.effortScale19==impact.effortScale19&&conserved.tick==impact.tick);
 // Technique-useful energy excludes wrist roll and negative thrust motion.
 ImpactRecord178r30 roll{};roll.gesture59c3=1;roll.kind=0;impact_effort19(roll,p,0,{}, {},{0,9,0},1.);
 assert(roll.effortKnown19&&roll.effortScale19==0);
 roll={};roll.gesture59c3=1;roll.kind=1;impact_effort19(roll,p,0,{}, {0,-2,0},{},1.);
 assert(roll.effortKnown19&&roll.effortScale19==0);
 std::puts("PASS actual physical producer: one committed thrust carries immutable gesture/item/body/request/energy/mass/support; no full debit on preparations");
}
