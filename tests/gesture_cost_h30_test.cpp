// Actual physical kernel + exact extracted production effort publisher. Native
// completion below is an explicit test leaf; no native game code is executed.
#include "gesture_recovery_h30_fixture.h"
#include "vrik/swing_effort59c14.h"
#include "subsystems/hybrid/contact_policy.h"

namespace mo=kcd2vr::motion59c4;
namespace se=kcd2vr::swing_effort59c14;
static bool synchronizedH30=true;
static bool CombatSynchronized59c10r9() noexcept {return synchronizedH30;}
struct PhysFrame6698 {ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};
 void* player=nullptr;std::uint64_t request=0;double publishWorldDeltaM122=0,publishWorldDeltaRad122=0;
 float world[8]{0,0,0,1,0,0,0,1};};
struct PhysEquipment6698 {bool valid=false;void* player=nullptr;std::uint64_t request=0,epoch=0,tick=0;
 int count=1;int heldIndex[2]{0,-1};bool handKnown[2]{true,true};std::uint64_t handKey[2]{};
 struct Item {ph::Profile profile{};bool held=true;};std::array<Item,2> items{};};
#include "effort_impact_publisher19.inc"

static PhysFrame6698 packetH30(const RigH30& r){PhysFrame6698 f{};
 f.input=r.in;f.output=r.out;f.profiles=r.profiles;f.player=reinterpret_cast<void*>(0x1234);f.request=r.in.epoch;return f;}
static PhysEquipment6698 equipmentH30(const PhysFrame6698& f){PhysEquipment6698 e{};
 e.valid=true;e.player=f.player;e.request=f.request;e.epoch=f.input.bodyEpoch;e.tick=f.input.tick;
 e.handKey[0]=f.profiles[0].key;e.items[0].profile=f.profiles[0];return e;}
static mo::Key identityH30(const ph::Command& c){return {c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch,
 kcd2vr::contact176::recoveryEpoch55c1.load(),0x1234};}
static bool completeNativeLeafH30(const mo::Key& key,std::uint64_t now){
 // Stand-in for the separately tested exact native start+dispatch completion.
 // This stores evidence only. It does not debit stamina or create effort.
 se::ledger.nativeObserved54(key,now-1);return se::ledger.nativeComplete54(key,now);
}
int main(){
 mo::selected=false;synchronizedH30=true;se::ledger.clear();
 kcd2vr::contact176::recoveryEpoch55c1=41;
 RigH30 r(false);r.in.intent59c16=false;r.in.dynamics=true;moveH30(r,10);gapH30(r,6);
 ckH30(ph::normal_hybrid_intent_h31(r.in)&&!r.in.intent59c16,"cost publisher test uses actual public HYBRID without legacy R16 route");
 const auto* source=firstH30(r,ph::CommandKind::Slash);
 ckH30(source&&r.hits()==1&&r.out.handlingOnly55c1[0],"actual normal cut commits once while collision output is unavailable");
 if(!source){std::printf("gesture_cost_h30=FAILED no soft commit fixture\n");return 1;}
 const auto commit=*source;const auto key=identityH30(commit);
 ckH30(!commit.effortKnown19,"soft commit does not manufacture achieved physical energy from controller intent");
 se::Record record{};
 ckH30(se::ledger.reserve(key,r.in.tick,record)==se::Claim::Missing,"qualified semantic commit alone cannot populate an effort record");
 auto unavailable=packetH30(r);auto unavailableGear=equipmentH30(unavailable);
 PhysPublishImpactEffort19(unavailable,unavailableGear);
 ckH30(se::ledger.reserve(key,r.in.tick,record)==se::Claim::Missing,"actual publisher refuses UNKNOWN display recovery");
 const auto nativeCompletion=commit.tick+2;
 ckH30(completeNativeLeafH30(key,nativeCompletion),"one exact-key native completion marker can precede physical energy publication");
 ckH30(se::ledger.contactProof54(key,r.in.tick)==se::ContactProof54::Refuse,"native marker without actual energy is not completed-contact proof");
 restoreH30(r,2);r.blocked=r.actor=true;r.step({0,.025,0});
 const auto actual=packetH30(r);const auto gear=equipmentH30(actual);const auto original=actual.output.impactRecord178r30[0];
 ckH30(original.valid&&original.gesture59c3==key.gesture&&original.tick==actual.input.tick&&original.sample==actual.input.sample&&
       original.collider==2&&original.entityId==13&&original.part==1,"later checked NPC impact keeps original same-gesture collider/part/sample/tick");
 ckH30(original.effortKnown19&&std::isfinite(original.effortScale19)&&original.effortMass19>0,
       "actual checked-impact branch records finite achieved approach energy");
 ckH30(ph::attack_authority55c1(actual.input,actual.output,0),"energy publication still requires current checked physical authority");
 PhysPublishImpactEffort19(actual,gear);
 ckH30(se::ledger.reserve(key,actual.input.tick,record)==se::Claim::Complete,
       "exact production publisher joins original actual energy to already completed native cost");
 ckH30(record.tick==nativeCompletion&&record.key.gesture==key.gesture&&record.key.recovery==key.recovery&&
       std::abs(record.factor-se::factor(original.effortScale19))<1e-10,
       "native completion retains its old payment clock and original impact energy factor");
 ckH30(kcd2vr::hybrid54::costProof54(se::ledger,key,original.tick,actual.input.tick,key.recovery)==kcd2vr::hybrid54::CostProof54::Ready,
       "original checked contact can continue through real hybrid cost gate without a second debit");
 const auto immutable=record;
 PhysPublishImpactEffort19(actual,gear);se::ledger.reserve(key,actual.input.tick,record);
 ckH30(record.tick==immutable.tick&&record.factor==immutable.factor&&record.status==immutable.status,
       "duplicate impact publication neither recharges nor renews original native cost");
 ckH30(!se::ledger.nativeComplete54(key,actual.input.tick),"same completed energy record cannot be completed a second time");
 ckH30(kcd2vr::hybrid54::costProof54(se::ledger,key,original.tick,original.tick+161,key.recovery)==kcd2vr::hybrid54::CostProof54::Refuse,
       "completed cost does not extend original contact beyond160ms");
 se::ledger.clear();PhysPublishImpactEffort19(actual,gear);
 ckH30(se::ledger.contactProof54(key,actual.input.tick)==se::ContactProof54::Refuse,
       "actual physical energy alone cannot claim completed native cost");
 se::ledger.nativeObserved54(key,actual.input.tick+1);
 ckH30(se::ledger.contactProof54(key,actual.input.tick+1)==se::ContactProof54::NativePending,
       "only exact observed native work makes the original checked impact wait");
 ckH30(se::ledger.nativeComplete54(key,actual.input.tick+2)&&
       kcd2vr::hybrid54::costProof54(se::ledger,key,original.tick,actual.input.tick+2,key.recovery)==kcd2vr::hybrid54::CostProof54::Ready,
       "physical energy first also joins later native completion without an extra debit");
 se::ledger.reserve(key,actual.input.tick+2,record);
 ckH30(record.tick==original.tick,"late native completion cannot renew original physical effort timestamp");
 se::ledger.clear();auto foreign=key;++foreign.gesture;completeNativeLeafH30(foreign,nativeCompletion);
 PhysPublishImpactEffort19(actual,gear);
 ckH30(se::ledger.contactProof54(key,actual.input.tick)==se::ContactProof54::Refuse,
       "another gesture's completed native work cannot credit this actual impact");
 for(unsigned bad=0;bad<14;++bad){se::ledger.clear();completeNativeLeafH30(key,nativeCompletion);
  auto f=actual;auto e=gear;
  switch(bad){case 0:f.output.impactRecord178r30[0].collider=0;break;
   case 1:f.output.impactRecord178r30[0].effortKnown19=false;break;
   case 2:f.output.handlingOnly55c1[0]=true;break;
   case 3:f.input.contactFaultH30=true;f.output.normalIntentH30[0].current=false;break;
   case 4:f.input.tracked[0]=false;break;
   case 5:f.input.allowed=false;break;
   case 6:f.output.contactKnown[0]=false;break;
   case 7:f.output.sweepDeferred115[0]=true;break;
   case 8:f.input.tick++;f.input.sample++;break;
   case 9:f.output.impactRecord178r30[0].generation++;break;
   case 10:e.items[0].held=false;break;
   case 11:synchronizedH30=false;break;
   case 12:f.output.normalIntentH30[0].current=false;break;
   case 13:f.output.ownShieldBlocked60[0]=true;break;}
  PhysPublishImpactEffort19(f,e);synchronizedH30=true;
  ckH30(se::ledger.contactProof54(key,actual.input.tick)==se::ContactProof54::Refuse,
        "missing actual contact/energy/owner/current clock cannot borrow native marker as a paid hit");
 }
 se::ledger.clear();completeNativeLeafH30(key,nativeCompletion);PhysPublishImpactEffort19(actual,gear);
 ckH30(kcd2vr::hybrid54::costProof54(se::ledger,key,original.tick,actual.input.tick,key.recovery+1)==kcd2vr::hybrid54::CostProof54::Refuse,
       "hard owner retirement still invalidates original physical/native cost join");
 std::printf("gesture_cost_h30=%u/%u actual_kernel=YES actual_publisher=YES native_completion=EXPLICIT_TEST_LEAF native_calls=NONE\n",
     checksH30-failuresH30,checksH30);return failuresH30?1:0;
}
