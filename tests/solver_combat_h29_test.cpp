#include "../src/vrik/native_weapon_combat_h29.h"
#include "../src/vrik/swing_effort59c14.h"
#include <cstdio>
using namespace kcd2vr;namespace sc=solver_command_h28;namespace se=swing_effort59c14;namespace b=native_weapon_body_t4;
static unsigned checks=0,fails=0;
#define CK(x) do{++checks;if(!(x)){++fails;std::printf("FAIL line=%u %s\n",__LINE__,#x);}}while(0)
int main(){
 b::Achieved a{};a.owner={1,2,3,4,5,6,7,8,9,10,11,0};a.sample=100;a.sourceTick=1000;a.observedTick=1005;
 a.worldGeneration=12;a.bodyGeneration=13;a.trackingEpoch=14;a.cancelGeneration=16;a.colliderLease=17;a.bodyId=18;
 a.state=b::State::Active;a.poseValid=true;a.colliderRetired=false;
 b::ContactOwnerH24 live{};live.owner=a.owner;live.body=19;live.foreign=20;live.foreignType=0x5634;live.bodyId=18;
 live.worldGeneration=12;live.bodyGeneration=13;live.trackingEpoch=14;live.cancelGeneration=16;live.colliderLease=17;
 live.sourceSample=100;live.sourceTick=1000;live.observedTick=1005;live.active=true;
 sc::Stamp original{};CK(sc::make(a,live,21,1010,original));
 se::Input54 input{{90,1,2,4,3,7,5},6,22,1000};input.solverH28=original;
 se::InputRegistry54 inputs;const auto nonce=inputs.publish(input,1010,3,1);CK(nonce);
 se::Input54 transported{};CK(inputs.read(nonce,1015,transported));CK(solver_rpg_h29::same(transported.solverH28,original));
 se::ActionRegistry54 actions;CK(actions.construct(30,6,true,1015));se::Action54 binding{},start{};
 CK(actions.bind(transported,30,31,32,1020,binding));CK(solver_rpg_h29::same(binding.solverH28,original));
 CK(actions.start(30,1020,start));motion59c4::Key credit{};
 CK(!actions.startFinish(start,true,1020,credit)); // native submission alone is not completed cost
 unsigned legacyCalls=0,typedCalls=0;bool current=true;
 auto legacy=[&](auto...){++legacyCalls;return true;};
 auto typed=[&](const sc::Stamp& p,auto...){++typedCalls;return current&&solver_rpg_h29::same(p,original);};
 CK(solver_rpg_h29::costOwned(binding,legacy,typed));CK(legacyCalls==0&&typedCalls==1);
 current=false;CK(!solver_rpg_h29::costOwned(binding,legacy,typed));CK(legacyCalls==0);current=true;
 auto changed=binding;++changed.solverH28.colliderLease;CK(!actions.dispatchFinish(changed,true,1025,credit));
 CK(actions.dispatchFinish(binding,true,1025,credit));CK(motion59c4::same(credit,input.key));
 CK(!actions.dispatchFinish(binding,true,1025,credit)); // one completion only
 auto malformed=binding;malformed.solverH28={};malformed.solverH28.bodyId=0;
 CK(!solver_rpg_h29::costOwned(malformed,legacy,typed));CK(legacyCalls==0);
 auto ordinary=binding;ordinary.solverH28={};CK(solver_rpg_h29::costOwned(ordinary,legacy,typed));CK(legacyCalls==1);
 // H30 normal ancestry chooses the current gesture-source callback, not the
 // latest displayed pose. No new cost/damage or contact is manufactured here.
 bool intentCurrent=true;unsigned normalCostReads=0;
 auto normalCost=[&](std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request){
  ++normalCostReads;return intentCurrent&&key==ordinary.key.weapon&&generation==ordinary.key.generation&&
   epoch==ordinary.key.body&&request==ordinary.key.request;
 };
 CK(solver_rpg_h29::costOwned(ordinary,normalCost,typed));CK(normalCostReads==1);
 intentCurrent=false;CK(!solver_rpg_h29::costOwned(ordinary,normalCost,typed));intentCurrent=true;
 auto wrongNormal=ordinary;++wrongNormal.key.generation;
 CK(!solver_rpg_h29::costOwned(wrongNormal,normalCost,typed));
 for(unsigned failure=0;failure<7;++failure){auto missing=ordinary;const auto readsBefore=normalCostReads;
  switch(failure){case 0:missing.key.gesture=0;break;case 1:missing.actor=0;break;case 2:missing.key.weapon=0;break;
   case 3:missing.key.generation=0;break;case 4:missing.key.body=0;break;case 5:missing.key.request=0;break;case 6:missing.key.player=0;break;}
  CK(!solver_rpg_h29::costOwned(missing,normalCost,typed));CK(normalCostReads==readsBefore);
 }
 // Original normal events keep their exact collider, sample, gesture and age.
 // A successful tracked source cannot rescue missing/unclocked contact proof.
 contact59c7g::Metadata normalContact{};normalContact.collider=50;normalContact.bodyMaterial=3;
 normalContact.feature=4;normalContact.weaponClass=4;normalContact.originZone47=2;
 unsigned originalReads=0;
 auto originalContactOwner=[&](std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,
                   std::uint64_t sample,std::uint64_t gesture,std::uint64_t tick,std::uint64_t now){
  ++originalReads;return intentCurrent&&key==1&&generation==2&&epoch==4&&request==3&&
   sample==100&&gesture==90&&tick==1000&&now>=tick;
 };
 const auto normalBefore=normalContact;
 CK(solver_rpg_h29::originalContactOwnedH30(normalContact,1,2,4,3,100,90,1000,1160,legacy,originalContactOwner,typed));
 CK(normalContact.collider==normalBefore.collider&&normalContact.bodyMaterial==3&&normalContact.originZone47==2);
 intentCurrent=false;
 CK(!solver_rpg_h29::originalContactOwnedH30(normalContact,1,2,4,3,100,90,1000,1010,legacy,originalContactOwner,typed));
 intentCurrent=true;
 for(unsigned failure=0;failure<11;++failure){auto metadata=normalContact;auto key=1ull,generation=2ull,epoch=4ull,request=3ull;
  auto sample=100ull,gesture=90ull,tick=1000ull,now=1010ull;
  switch(failure){case 0:metadata.collider=0;break;case 1:key=0;break;case 2:generation=0;break;
   case 3:epoch=0;break;case 4:request=0;break;case 5:sample=0;break;case 6:gesture=0;break;
   case 7:tick=0;break;case 8:now=1161;break;case 9:now=999;break;case 10:generation++;break;}
  CK(!solver_rpg_h29::originalContactOwnedH30(metadata,key,generation,epoch,request,sample,gesture,tick,now,legacy,originalContactOwner,typed));
 }
 // Production physical occurrence -> committed achieved gesture -> metadata.
 native_weapon_actor_join_h28::Contact joined{};auto& e=joined.occurrence;e.serial=44;e.tick=1010;e.source=live;
 e.otherBody=50;e.otherForeign=51;e.otherPart=3;e.selfVelocity={0,4,0};e.normal={0,1,0};
 auto& w=joined.witness;w.ownerRead=true;w.owner=51;w.entityId=52;w.collider=50;w.part=3;w.body=npc_contact172::Body::Living;
 physical::Profile weapon{};weapon.key=1;weapon.generation=2;weapon.classId=4;
 physical::Output::SolverGestureH29 gesture{90,1000,90,{0,1,0},0,1};native_weapon_combat_h29::Contact result{};
 CK(native_weapon_combat_h29::bind(joined,original,gesture,weapon,1020,result));
 CK(result.impact.gesture59c3==90&&result.impact.part==3&&result.impact.tick==1010&&result.impact.sample==100);
 CK(result.impact.surfaceIdx==-1&&result.metadata.bodyMaterial==-1&&result.metadata.occurrenceSerialH28==44);
 CK(solver_rpg_h29::contactOwned(result.metadata,1,2,4,3,100,90,1010,1020,legacy,typed));
 const auto originalBeforeTyped=originalReads;
 CK(solver_rpg_h29::originalContactOwnedH30(result.metadata,1,2,4,3,100,90,1010,1020,legacy,originalContactOwner,typed));
 CK(originalReads==originalBeforeTyped); // F9 never downgrades to a normal source.
 const auto legacyBefore=legacyCalls;
 CK(!solver_rpg_h29::contactOwned(result.metadata,1,2,4,3,0,90,1010,1020,legacy,typed));
 CK(!solver_rpg_h29::contactOwned(result.metadata,1,2,4,3,100,0,1010,1020,legacy,typed));
 CK(legacyCalls==legacyBefore);

 for(unsigned cause=0;cause<9;++cause){auto bad=result.metadata;auto sample=100ull;auto tick=1010ull;
  switch(cause){case 0:bad.occurrenceSerialH28=0;break;case 1:bad.sourceSolverH28={};break;case 2:bad.sourceSolverH28.owner.key++;break;
   case 3:sample++;break;case 4:tick=900;break;case 5:tick=1021;break;case 6:bad.sourceSolverH28.sourceTick=1011;break;
   case 7:bad.sourceSolverH28.observedTick=1011;break;case 8:bad.sourceSolverH28.colliderLease++;break;}
  CK(!solver_rpg_h29::contactOwned(bad,1,2,4,3,sample,90,tick,1020,legacy,typed));CK(legacyCalls==legacyBefore);
 }
 for(unsigned cause=0;cause<11;++cause){auto je=joined;auto g=gesture;auto proof=original;
  switch(cause){case 0:g.gesture=0;break;case 1:g.tick=1011;break;case 2:g.tick=800;break;case 3:g.onsetSample=101;break;
   case 4:je.occurrence.selfVelocity={0,-4,0};break;case 5:je.occurrence.selfVelocity={};break;
   case 6:je.occurrence.tick=900;break;case 7:proof.sourceSample++;break;case 8:je.witness.part++;break;
   case 9:je.witness.body=npc_contact172::Body::Unknown;break;case 10:je.occurrence.selfVelocity.x=std::numeric_limits<float>::quiet_NaN();break;}
  CK(!native_weapon_combat_h29::bind(je,proof,g,weapon,1020,result));CK(!result.impact.valid);
 }
 std::printf("solver_combat_h29 %u checks %u failures; original registry and physical event paths; native headset acceptance UNCLAIMED\n",checks,fails);
 return fails?1:0;
}
