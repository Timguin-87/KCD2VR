#include <cstdlib>
#include "diagnostics/actor_lookup_h7.h"
#include "diagnostics/articulated_target_owner_h8.h"
// This observer-map fixture never enters the native ActorUpdate lookup scope.
static const void* NativeActorForEntityH7(std::uint32_t) noexcept {std::abort();}
// No retail image is supplied here; dedicated articulated_target_owner_h8
// exercises the exact-image positive and per-pin negative paths.
namespace retail_build112 {inline bool Ready(std::uintptr_t) noexcept{return false;}}
#include "vrik/combo_lineage19.h"
#include "vrik/contact_handoff59c3.h"
#include "vrik/contact_metadata59c7g.h"
#include "vrik/solver_rpg_provenance_h29.h"
#include "vrik/receipt_build59c7g.h"
#include "diagnostics/contact_lease47c2.h"
#include "core/contact_backlog59c6.h"
#include "core/impact_identity59c6.h"
#include "vrik/physical_weapon.h"
#include "vrik/npc_contact172.h"
#include "subsystems/hybrid/contact_policy.h"
#include "subsystems/hybrid/target_wait_policy.h"
#include "vrik/recovery_retire65h.h"
#include "diagnostics/capture168.h"
namespace diagnostics=kcd2vr::diagnostics;
#include <cstring>
#include <vector>
#include <atomic>
#include <mutex>
#include <cstdio>
#include <cassert>
#include <type_traits>
static std::uint64_t now59c6=1000;inline std::uint64_t GetTickCount64(){return now59c6;}
namespace handoff59c3=kcd2vr::contact59c3;
namespace victim5916c3{void Clear(){}} namespace vrstate596{void Reset(){}}
std::atomic<std::uint64_t> g_stats[8]{},g_coalesced59c3{0},g_henry_seen178r31{0},g_native_owned_until178r33{0};
std::atomic<int> g_henry_in_combat178r31{-1},g_tg_phase178r34{0};
bool Allow(int,int); namespace diagnostic_trace {void Write(const char*);}
#include "generated_contact_queue59c6.inc"
// Explicit native memory/identity leaves for the actual extracted map and gate.
namespace target_fixture19c3 {
struct Region {unsigned char* p;std::size_t n;};
std::vector<Region> regions;
struct Actor {const void *entity=nullptr,*state=nullptr;bool coherent=true;} henry,npc,other;
bool ownerValid=true;unsigned identityReads=0;
std::uint64_t key=20,generation=1,body=2,request=3;
}
constexpr std::uintptr_t g_base=0x180000000ull,kEntityVft=0x4072c78;
namespace impulse596 {constexpr std::uintptr_t kPhysicalProxyVft5916c3=0x3ac3478;}
bool SehCopy146(const void* from,void* to,std::size_t n){
 const auto p=reinterpret_cast<std::uintptr_t>(from);
 for(const auto& r:target_fixture19c3::regions){const auto base=reinterpret_cast<std::uintptr_t>(r.p);
  if(p>=base&&p-base<=r.n&&n<=r.n-(p-base)){std::memcpy(to,from,n);return true;}}
 return false;
}
template<class T>T Rd(const void* p,std::size_t off,T fallback){T value{};
 // Explicit native owner read leaf; no process memory is dereferenced.
 if constexpr(std::is_same_v<T,const void*>)if(p==&target_fixture19c3::henry&&off==0x1F8)return reinterpret_cast<const void*>(0x1234);
 return p&&SehCopy146(static_cast<const unsigned char*>(p)+off,&value,sizeof(value))?value:fallback;}
// Explicit read-only native property leaf for the production F3 trace in the extracted target resolver.
constexpr std::uint32_t kAttackZone=0xF0,kActionPhaseProp39=0x30,kAttackType=0x210,kPendingZone=0x120,kOpponentZone=0x5D0;
int I(const void* st,std::uint32_t prop) noexcept {return Rd<std::int32_t>(st,prop+8,-99);}
bool CombatActor(const void* p,const void*& entity,const void*& state,bool){
 using namespace target_fixture19c3;
 ++identityReads;const auto* a=static_cast<const Actor*>(p);
 if((a!=&henry&&a!=&npc&&a!=&other)||!a->coherent)return false;
 entity=a->entity;state=a->state;return entity&&state;
}
std::uint32_t EntityId(const void* e){return Rd<std::uint32_t>(e,0xc,0);}
namespace frik_pose_probe {bool GeometryOwnerValid178r30(std::uint64_t k,std::uint64_t g,std::uint64_t b,std::uint64_t r){
 using namespace target_fixture19c3;return ownerValid&&k==key&&g==generation&&b==body&&r==request;}
bool OriginalGeometryContactOwnerH30(std::uint64_t k,std::uint64_t g,std::uint64_t b,std::uint64_t r,
 std::uint64_t sample,std::uint64_t gesture,std::uint64_t tick,std::uint64_t now){
 return sample&&gesture&&kcd2vr::contact176::fresh(tick,now,kcd2vr::contact59c3::maximumContactAge)&&
  GeometryOwnerValid178r30(k,g,b,r);
}
// This legacy map fixture has no physical collider lease. Typed events may
// never inherit its successful hand-owner leaf (the typed positive chain is
// exercised by solver_combat_h29 and solver_command_h28).
bool GeometrySolverOwnerValidH28(const kcd2vr::solver_command_h28::Stamp&,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t){return false;}}

bool Allow(int,int){return false;} namespace diagnostic_trace {void Write(const char*){}}
// Explicit capture-only leaf; actual implementation executes in counter_capture36_test.
namespace counter36 {void ActorUpdated(const void*,std::uint64_t) noexcept {}}
#include "generated_target_arrival19c3.inc"
int main(){using namespace kcd2vr;unsigned tests=0;auto ck=[&](bool b,const char* n){++tests;std::printf("%s %s\n",b?"PASS":"FAIL",n);assert(b);};
 Pending a{};a.gesture59c3=10;a.key=20;a.generation=1;a.epoch=2;a.request=3;a.sample=100;a.tick=1000;a.entityId=50;
 a.metadata59c7g={0x9876,8,int(bodycombo59c7g::Feature::Tip),true,4,false};
 contact176::recoveryEpoch55c1=1;Clear();ck(Submit(a),"first contact accepted");const auto serial=g_pending.serial;
 auto b=a;b.gesture59c3=11;b.entityId=51;ck(Submit(b)&&g_backlog59c6.count==1,"distinct attempt/recipient retained instead of coalesced");
 auto duplicate=a;duplicate.sample++;duplicate.part=2;now59c6=1010;ck(!Submit(duplicate)&&g_pending.tick==1000&&g_pending.queued==1000,"repeated part sample cannot renew or replace first impact");
 unsigned calls=0;DrainBooking59c6([&](Pending& p,auto& lease){++calls;ck(p.serial==serial&&p.gesture59c3==10,"active contact stays original during native continuation");
  ck(p.metadata59c7g.collider==0x9876&&p.metadata59c7g.bodyMaterial==8&&p.metadata59c7g.twoHands,
     "queued contact retains original collider/material/support metadata");
  struct NativeResult {bool seen=true,ok=true,fields=true;std::uint32_t target=50;
   int nativePart=24,material=2,blocked=0,perfect=0,parry=0,master=0,death=1,stabbed=1,combo=0,comboEnd=0,dispatch=1;
   float dmg=12,stam=7;} native;
  const auto receipt=contact59c7g::makeReceipt(p,native,1010,50,0x1234,2,true,true,true);
  ck(receipt.direct&&receipt.continuation&&receipt.attempt==10&&receipt.origin==2&&receipt.bodyMaterial==8&&
     receipt.nativePart==24&&receipt.collider==0x9876&&receipt.damage==12&&receipt.targetIdentity==0x1234,
     "direct receipt preserves native result, physical region evidence and attack origin independently");
  native.target=51;
  ck(!contact59c7g::makeReceipt(p,native,1010,50,0x1234,2,true,true,true).continuation,
     "a different native victim cannot complete the queued contact");
  DrainBooking59c6([&](Pending&,auto&){++calls;});ck(calls==1,"reentrant drain cannot process another hit");
  auto c=a;c.gesture59c3=12;c.entityId=52;ck(Submit(c),"producer may append while original native call is active");});
 ck(g_has&&g_pending.gesture59c3==11&&g_backlog59c6.count==1,"completed reservation promotes first queued contact");
 DrainBooking59c6([&](Pending& p,auto& lease){lease.wait(p);});ck(g_has&&g_pending.gesture59c3==11&&g_pending.tick==1000,"explicit wait retains original time and FIFO head");
 DrainBooking59c6([&](Pending&,auto&){});ck(g_has&&g_pending.gesture59c3==12,"terminal rejection also frees reservation without replay");
 DrainBooking59c6([&](Pending&,auto&){});ck(!g_has&&!g_backlog59c6.count,"queue empty after every event consumed once");
 Clear();now59c6=1000;for(unsigned i=0;i<9;++i){auto p=a;p.gesture59c3+=i;ck(Submit(p),"bounded inbox entry accepted");}
 auto full=a;full.gesture59c3=99;ck(!Submit(full)&&g_backlogFull59c6==1,"capacity rejects explicitly without overwrite");
 now59c6=1161;full.tick=1161;ck(Submit(full)&&g_pending.gesture59c3==99&&!g_backlog59c6.count&&g_backlogExpired59c6>=9,"original contact-age expiry releases stale queued entries");
 Clear();now59c6=1200;a.tick=1200;ck(Submit(a),"start old generation");
 DrainBooking59c6([&](Pending&,auto&){contact176::recoveryEpoch55c1=2;Clear();b.tick=1200;
  ck(Submit(b)&&!g_has&&g_backlog59c6.count==1,"clear during continuation cannot reuse its active slot");});
 ck(g_has&&g_pending.recovery55c1==2&&g_pending.gesture59c3==b.gesture59c3,"old destructor does not erase newer recovery contact");
 Clear();auto bad=a;bad.tick=0;ck(!Submit(bad),"unclocked contact cannot enter inbox");bad=a;bad.gesture59c3=0;ck(!Submit(bad),"unowned contact cannot enter inbox");
 impact59c6::Seen seen;physical::ImpactRecord178r30 hit{};hit.valid=true;hit.tick=1200;hit.sample=20;hit.key=1;hit.generation=1;hit.entityId=50;
 ck(seen.take(hit)&&!seen.take(hit),"immutable impact repeated publication deduplicated");++hit.sample;ck(seen.take(hit),"second original sample inside same coarse millisecond is distinct");
 ++hit.generation;ck(seen.take(hit),"replacement generation does not inherit previous impact suppression");
#include "target_arrival19c3_cases.inl"
#include "hybrid_target65h_cases.inl"
#include "recovery_retire65h_cases.inl"
 // Production Drain booking + exact production cost decision + real ledger:
 // retain only known native partial work, never rejuvenate the physical hit.
 for(std::uint64_t completionAge:{80ull,150ull}){
  Clear();now59c6=2000;contact176::recoveryEpoch55c1=1;auto pending=a;pending.tick=2000;pending.gesture59c3=300+completionAge;
  auto other=pending;other.gesture59c3++;other.entityId++;
  const motion59c4::Key key{pending.gesture59c3,pending.key,pending.generation,pending.epoch,pending.request,1,0x1234};
  swing_effort59c14::Ledger cost;ck(cost.publish(key,.9,1.5,false,2000),"real contact energy published");cost.nativeObserved54(key,2000);
  ck(Submit(pending)&&Submit(other),"native-pending contact and another recipient coexist");
  now59c6=2070;unsigned delivered=0;
  DrainBooking59c6([&](Pending& p,auto& lease){ck(hybrid54::costProof54(cost,key,p.tick,now59c6,1)==hybrid54::CostProof54::Wait,"only exact NativePending waits after70ms");lease.defer(p);});
  ck(g_has&&g_pending.entityId==other.entityId&&g_backlog59c6.count==1,"pending cost yields head to other recipient without spinning");
  DrainBooking59c6([&](Pending& p,auto&){ck(p.entityId==other.entityId,"other recipient progresses before pending cost completes");});
  ck(g_has&&g_pending.tick==2000&&g_pending.queued==2000,"retained contact keeps original physical and queue timestamps");
  now59c6=2000+completionAge;ck(cost.nativeComplete54(key,now59c6),"synthetic native proof completes at80/150ms without added debit");
  DrainBooking59c6([&](Pending& p,auto&){if(hybrid54::costProof54(cost,key,p.tick,now59c6,1)==hybrid54::CostProof54::Ready)++delivered;});
  ck(delivered==1&&!g_has,"same original contact can continue once when exact proof completes inside160ms");
 }
 {Clear();now59c6=3000;auto p=a;p.tick=3000;p.gesture59c3=600;contact176::recoveryEpoch55c1=1;
  const motion59c4::Key key{600,p.key,p.generation,p.epoch,p.request,1,0x1234};swing_effort59c14::Ledger cost;
  ck(cost.publish(key,.9,1.5,false,3000),"expiry fixture has real energy");cost.nativeObserved54(key,3000);ck(Submit(p),"expiry fixture queued");
  now59c6=3070;DrainBooking59c6([&](Pending& x,auto& lease){lease.defer(x);});now59c6=3161;cost.nativeComplete54(key,3161);unsigned delivered=0;
  DrainBooking59c6([&](Pending& x,auto&){if(hybrid54::costProof54(cost,key,x.tick,now59c6,1)==hybrid54::CostProof54::Ready)++delivered;});
  ck(!delivered&&!g_has,"native completion after original161ms cannot revive expired contact");
  auto wrong=key;++wrong.body;ck(hybrid54::costProof54(cost,wrong,3000,3080,1)==hybrid54::CostProof54::Refuse,"different body/key cannot borrow pending cost");
  ck(hybrid54::costProof54(cost,key,3000,3080,2)==hybrid54::CostProof54::Refuse,"recovery change cannot retain old contact");
  swing_effort59c14::Ledger unknown;unknown.publish(key,.9,1.5,false,3000);
  ck(hybrid54::costProof54(unknown,key,3000,3080,1)==hybrid54::CostProof54::Refuse,"generic Pending is not native submission evidence");
  unknown.nativeUncertain54(key,3080);ck(hybrid54::costProof54(unknown,key,3000,3080,1)==hybrid54::CostProof54::Refuse,"Indeterminate refuses rather than waits");
 }
 std::printf("contact_queue59c6=%u; booking and native continuation explicitly separated\n",tests);
}
