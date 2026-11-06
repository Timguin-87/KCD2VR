// Literal outer early-return/call sites + actual N2 producer + physical kernel + native coordinator.
#include "impale_native_eligibility_r93_fixture.h"
#include "core/impact_identity59c6.h"
#define main retained_native_handoff_main
#include "impale_native_handoff59_test.cpp"
#undef main
#define main retained_hang_main
#include "impale_hang44_test.cpp"
#undef main
static std::atomic<void*>g_last_player_entity{nullptr};
namespace frik_pose_probe {static void PublishVisibleFailure(const char*){}}
#include "subsystems/gore/corpse_impale_producer_r93.inl"
static bool combatOuterR93=false;static unsigned producersOuterR93=0,coordinatorsOuterR93=0,labOuterR93=0;
static std::atomic<std::uint64_t>g_native_busy_tick178{0};
static bool PhysicalCombat177Enabled(){return combatOuterR93;}
namespace lab58n {static void Tick(std::uint64_t){++labOuterR93;}}
static void producerOuterR93(){++producersOuterR93;CorpseImpaleProducerR93();}
static void coordinatorOuterR93(){++coordinatorsOuterR93;ImpalePartTick59c7g();}
#define CorpseImpaleProducerR93 producerOuterR93
#define ImpalePartTick59c7g coordinatorOuterR93
#include "actual_impale_outer_r93.inc"
#undef CorpseImpaleProducerR93
#undef ImpalePartTick59c7g
static std::atomic<bool>g_impale_enabled178r15{false};
static bool CombatExperiment175Enabled(){return false;}
static void optionsOuterR93(ph::Input& pi){
#include "actual_impale_options_r93.inc"
}
static void nativeOuterR93(Replay& r){
 frame59={};frame59.input=r.in;frame59.output=r.out;frame59.profiles=r.profiles;frame59.request=3;
 frame59.world[3]=frame59.world[7]=1;now59=r.in.tick;
 g_phys_frame6698.publish(frame59);g_phys_ready_sample6698=r.in.sample;
 const auto p=producersOuterR93,c=coordinatorsOuterR93,l=labOuterR93;actualOuterR93();
 ck(producersOuterR93==p+1&&coordinatorsOuterR93==c+1,"actual outer branch dispatches producer/coordinator exactly once regardless of physical combat");
 ck(labOuterR93==l+1,"existing independent lab tick remains exactly once");
}
static void resetOuterR93(Replay& r,unsigned iteration){
 g_gameplay_allowed=false;nativeOuterR93(r);g_gameplay_allowed=true;
 kcd2vr::impale_corpse_r15::latest.publish({});for(auto& e:kcd2vr::gore32::knownDead)e.id=0;
 r.time=20+iteration*10;r.in.epoch=3;r.in.bodyEpoch=2;r.in.physicalCombat177=false;r.move({});
 kcd2vr::gore32::impaleHang=true;kcd2vr::impale_corpse_r15::enabled=false;optionsOuterR93(r.in);
 poll59=kcd2vr::impale47::Poll::Ready;root59=0x440;partPoint59=r.tip();integratePart59=true;
}
static void impactOuterR93(Replay& r,bool dead=true,bool owned=true,int kind=1){
 if(dead)kcd2vr::gore32::NoteDead(42,0x420,r.in.tick);
 auto& h=r.out.impactRecord178r30[0];h={};h.valid=true;h.entityId=42;h.collider=0x440;h.part=12;
 h.kind=kind;h.tick=r.in.tick;h.key=h.generation=1;h.epoch=2;h.request=3;h.sample=r.in.sample;
 h.feature59c7g=int(kcd2vr::bodycombo59c7g::Feature::Tip);h.body=int(kcd2vr::npc_contact172::Body::Articulated);
 h.speed=3.18;h.point=r.tip();r.out.owned[0]=owned;
}
int main(){
 kcd2vr::gore32::hangMassKg=0;kcd2vr::impale40::mode=0;unsigned iteration=0;
 for(bool combat:{false,true})for(int f9=0;f9<4;++f9){
  combatOuterR93=combat;kcd2vr::collab_h21::setPhysicsMode(f9);Replay r;resetOuterR93(r,++iteration);
  ck(r.in.impaleHang32&&r.in.impaleEnabled178r15&&!r.in.physicalCombat177,"actual single-option producer ignores unrelated combat and legacy flags");
  impactOuterR93(r);nativeOuterR93(r);
  ck(kcd2vr::impale_corpse_r15::latest.read(r.in.corpseStabR15)&&r.in.corpseStabR15.valid,"literal N2 publishes exact game-dead owned corpse candidate with legacy VrImpaleCorpse OFF");
  r.move({});nativeOuterR93(r);ImpalePin178r15 pin{};
  ck(r.out.impaleLatchRequest&&g_impale_pin178r15.read(pin)&&pin.valid,"actual outer dispatch reaches first corpse pin without physical combat/F9 requirement");
  const auto before=drive59;
  for(unsigned n=0;n<12;++n){r.in.impalePinValid178r15=pin.valid;r.in.impalePinSerial178r19=pin.serial;r.in.impalePinWorld178r15={pin.x,pin.y,pin.z};
   r.move({.002,0,0});nativeOuterR93(r);g_impale_pin178r15.read(pin);}
  ck(pin.valid&&drive59>before,"same outer path retains active corpse drive through repeated frames");
  const auto off=drive59;kcd2vr::gore32::impaleHang=false;optionsOuterR93(r.in);r.move({});nativeOuterR93(r);g_impale_pin178r15.read(pin);
  ck(!pin.valid&&drive59==off,"single option OFF reaches coordinator retirement without a corpse write");
  ck(kcd2vr::collab_h21::physicsMode.load()==f9&&!kcd2vr::impale_corpse_r15::enabled.load(),"no unrelated option is switched on");
 }
 combatOuterR93=false;kcd2vr::collab_h21::setPhysicsMode(0);
 for(unsigned bad=0;bad<3;++bad){Replay r;resetOuterR93(r,++iteration);impactOuterR93(r,bad!=0,bad!=1,bad==2?0:1);nativeOuterR93(r);
  kcd2vr::impale_corpse_r15::Candidate c{};kcd2vr::impale_corpse_r15::latest.read(c);
  ck(!c.valid,"actual N2 rejects missing game-death, lost blade ownership and non-thrust independently of config");}
 std::printf("PASS %u outer/source/physical/native checks\n",checks);return 0;
}
