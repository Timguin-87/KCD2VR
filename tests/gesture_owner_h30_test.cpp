#include "vrik/physical_weapon.h"
#include "vrik/gesture_recovery_h30.h"
#include <cassert>
#include <cstdio>
namespace ph=kcd2vr::physical;
struct Frame {ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};};
static Frame live(){
 Frame f;auto& i=f.input;i.allowed=i.attacks=i.physicalCombat177=i.coherentMelee119=i.intentTracked[0]=i.tracked[0]=true;
 i.epoch=3;i.bodyEpoch=4;i.sample=20;i.tick=1000;
 i.forwardOnly19=true;
 auto& p=f.profiles[0];p.key=77;p.generation=2;p.primary=p.geometry=true;p.classId=4;p.side=0;
 f.output.active[0]=f.output.owned[0]=true;
 f.output.normalIntentH30[0]={77,2,4,3,20,1000,true};return f;
}
int main(){
 const auto before=live();auto recovery=before;recovery.output.handlingOnly55c1[0]=true;
 auto hardSource=before;hardSource.output.normalIntentH30[0]={};
 assert(ph::attack_authority55c1(hardSource.input,hardSource.output,0));
 assert(!ph::command_authority_h30(hardSource.input,hardSource.output,hardSource.profiles[0],0));
 assert(ph::command_authority_h30(recovery.input,recovery.output,recovery.profiles[0],0));
 assert(!ph::attack_authority55c1(recovery.input,recovery.output,0));
 assert(!kcd2vr::gesture_recovery_h30::Retires(before,recovery,ph::command_authority_h30));
 auto lostContext=before;lostContext.input.physicalCombat177=lostContext.input.attacks=false;
 lostContext.output.normalIntentH30[0]={};
 assert(kcd2vr::gesture_recovery_h30::Retires(before,lostContext,ph::command_authority_h30));
 auto masked=recovery;masked.output.strokeActive119=true;masked.output.guard117=true;
 masked.output.guardBySide30c1[0]=masked.output.guardBySide30c1[1]=true;
 for(int side=0;side<2;++side){masked.output.impactRecord178r30[side].valid=true;
  masked.output.impactRecord178r30[side].tick=990;masked.output.impactSpeed[side]=3;
  masked.output.impactEventTick178r24[side]=990;masked.output.contact119[side].known=true;}
 kcd2vr::gesture_recovery_h30::MaskRecoveredContacts(masked.output);
 assert(masked.input.attacks&&masked.input.physicalCombat177&&masked.output.strokeActive119&&masked.output.normalIntentH30[0].current);
 assert(!masked.output.guard117&&!masked.output.guardBySide30c1[0]&&masked.output.guardBySide30c1[1]);
 assert(!masked.output.impactRecord178r30[0].valid&&!masked.output.impactSpeed[0]&&!masked.output.impactEventTick178r24[0]&&!masked.output.contact119[0].known);
 assert(masked.output.impactRecord178r30[1].valid&&masked.output.impactRecord178r30[1].tick==990&&masked.output.contact119[1].known);
 for(unsigned failure=0;failure<10;++failure){auto f=recovery;
  switch(failure){case 0:f.output.normalIntentH30[0].current=false;break;
   case 1:++f.output.normalIntentH30[0].sample;break;
   case 2:++f.output.normalIntentH30[0].generation;break;
   case 3:f.input.allowed=false;break;case 4:f.input.tracked[0]=false;break;
   case 5:f.input.intentTracked[0]=false;break;case 6:f.input.contactFaultH30=true;break;
   case 7:f.output.ownShieldBlocked60[1]=true;break;
   case 8:f.output.normalIntentH30[0].key=88;f.profiles[0].key=88;break;
   case 9:f.output.normalIntentH30[0].bodyEpoch=5;f.input.bodyEpoch=5;break;}
  assert(kcd2vr::gesture_recovery_h30::Retires(before,f,ph::command_authority_h30));
 }
 auto noProof=recovery;noProof.output.normalIntentH30[0]={};
 assert(!ph::command_authority_h30(noProof.input,noProof.output,noProof.profiles[0],0));
 auto old=before;old.output.normalIntentH30[0]={};
 assert(kcd2vr::gesture_recovery_h30::Retires(old,noProof,ph::command_authority_h30));
 // An already stopped offhand cannot mask a later primary stop.
 old.output.ownShieldBlocked60[1]=true;auto next=old;next.output.ownShieldBlocked60[0]=true;
 assert(kcd2vr::gesture_recovery_h30::Retires(old,next,ph::command_authority_h30));
 for(int toolClass:{11,8,17}){
  auto tool=before;auto& in=tool.input;auto& p=tool.profiles[1];
  p.side=1;p.classId=toolClass;p.key=90;p.generation=3;p.primary=p.geometry=true;
  in.tracked[1]=in.intentTracked[1]=in.modelIntentSourceH30[1]=in.modelIntentTrackedH30[1]=true;
  tool.output.active[1]=tool.output.owned[1]=true;
  auto soft=tool;soft.output.handlingOnly55c1[1]=true;soft.output.contactKnown[1]=false;
  assert(!kcd2vr::gesture_recovery_h30::Retires(tool,soft,ph::command_authority_h30));
  assert(ph::command_authority_h30(soft.input,soft.output,soft.profiles[0],0));
  assert(!ph::attack_authority55c1(soft.input,soft.output,0)&&!ph::attack_authority55c1(soft.input,soft.output,1));
  for(bool alreadyRecovering:{false,true})for(unsigned failure=0;failure<12;++failure){
   auto prior=alreadyRecovering?soft:tool;auto lost=soft;
   switch(failure){case 0:lost.input.tracked[1]=false;break;
    case 1:lost.input.intentTracked[1]=false;break;
    case 2:lost.input.modelIntentTrackedH30[1]=false;break;
    case 3:lost.input.modelIntentSourceH30[1]=false;break;
    case 4:++lost.profiles[1].key;break;case 5:++lost.profiles[1].generation;break;
    case 6:lost.profiles[1].classId=4;break;case 7:lost.profiles[1].side=0;break;
    case 8:lost.output.normalIntentH30[0]={};break;case 9:lost.output.ownShieldBlocked60[1]=true;break;
    case 10:lost.output.solverOwnedT4[1]=true;break;
    case 11:lost.output.normalIntentH30[1]={90,3,in.bodyEpoch,in.epoch,in.sample,in.tick,true};break;}
   assert(kcd2vr::gesture_recovery_h30::Retires(prior,lost,ph::command_authority_h30));
  }
 }
 std::puts("PASS tracked command survives soft recovery; independent primary=true left torch/shields retain right owner; 72 tool hard negatives retire; contacts stay strict");
}
