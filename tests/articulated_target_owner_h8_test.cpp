#include <cstdlib>
#include "diagnostics/actor_lookup_h7.h"
#include "diagnostics/articulated_target_owner_h8.h"
static const void* NativeActorForEntityH7(std::uint32_t) noexcept {std::abort();}
namespace retail_build112 {inline bool exactH8=true;inline bool Ready(std::uintptr_t)noexcept{return exactH8;}}
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
static bool badPinH8=false;static int badPinIndexH8=-1;static std::uintptr_t unownedBodyH8=0;static unsigned unownedReadsH8=0;
static unsigned mainReadsH8=0;static std::uintptr_t mutateAtH8=0;
bool SehCopy146(const void* from,void* to,std::size_t n){
 const auto addr=reinterpret_cast<std::uintptr_t>(from);
 if(unownedBodyH8&&addr>=unownedBodyH8&&addr-unownedBodyH8<0x100){++unownedReadsH8;return false;}
 for(const auto& pin:kcd2vr::articulated_target_owner_h8::pins)if(addr==g_base+pin.rva&&n==pin.size){std::memcpy(to,pin.bytes,n);if((badPinH8&&pin.rva==0x2103EE0)||(badPinIndexH8>=0&&&pin==&kcd2vr::articulated_target_owner_h8::pins[badPinIndexH8]))static_cast<unsigned char*>(to)[0]^=1;return true;}
 for(auto slot:{std::pair<std::uintptr_t,std::uintptr_t>{0x40602F8+0x38,0x1AA6BC0},{0x3A71028+0x30,0x1A77FD0}})if(addr==g_base+slot.first&&n==sizeof(std::uintptr_t)){const auto value=g_base+slot.second;std::memcpy(to,&value,n);return true;}
 if(mutateAtH8&&addr==mutateAtH8&&++mainReadsH8==2)return false;
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

static unsigned checksH8=0,failuresH8=0;static void ckH8(bool b,const char* s){++checksH8;if(!b){++failuresH8;std::printf("FAIL %s\n",s);}}
int main(){using namespace kcd2vr;using namespace target_fixture19c3;
 std::array<unsigned char,0x100> he{},ne{},oe{},head{},node{},nodeRender{},physical{},slot{};
 std::array<unsigned char,0x180> render{},capsule{},articulated{};std::array<unsigned char,0xA00> character{};
 std::array<unsigned char,16> slotArray{};std::array<unsigned char,0xD20> hs{},ns{},os{};
 regions={{he.data(),he.size()},{ne.data(),ne.size()},{oe.data(),oe.size()},{head.data(),head.size()},{node.data(),node.size()},{nodeRender.data(),nodeRender.size()},{physical.data(),physical.size()},{render.data(),render.size()},{slot.data(),slot.size()},{slotArray.data(),slotArray.size()},{capsule.data(),capsule.size()},{articulated.data(),articulated.size()},{character.data(),character.size()},{hs.data(),hs.size()},{ns.data(),ns.size()},{os.data(),os.size()}};
 auto put=[](auto& v,std::size_t at,auto x){std::memcpy(v.data()+at,&x,sizeof x);};auto ptr=[](auto&v){return reinterpret_cast<std::uintptr_t>(v.data());};
 Pending a{};unsigned nativeEntries=0;bool doClaim=false;
 auto reset=[&](bool direct){Clear();g_actors={};contact176::recoveryEpoch55c1=20;ownerValid=true;key=20;generation=1;body=2;request=3;now59c6=2000;retail_build112::exactH8=true;badPinH8=false;badPinIndexH8=-1;unownedBodyH8=0;unownedReadsH8=0;mainReadsH8=0;mutateAtH8=0;nativeEntries=0;
  he={};ne={};oe={};head={};node={};nodeRender={};physical={};render={};slot={};slotArray={};capsule={};articulated={};character={};hs={};ns={};os={};
  put(he,0,g_base+kEntityVft);put(he,0xC,std::uint32_t(1));put(ne,0,g_base+kEntityVft);put(ne,0xC,std::uint32_t(50));put(oe,0,g_base+kEntityVft);put(oe,0xC,std::uint32_t(51));
  put(ne,0xA8,ptr(head));put(head,8,ptr(nodeRender));put(nodeRender,0x19,std::uint8_t(0));put(nodeRender,0x20,0u);put(nodeRender,0x28,ptr(render)+0x50);put(nodeRender,0x10,ptr(node));
  put(node,0x19,std::uint8_t(0));put(node,0x20,1u);put(node,0x28,ptr(physical));put(physical,0,g_base+impulse596::kPhysicalProxyVft5916c3);put(physical,0x30,ptr(ne));put(physical,0x38,ptr(capsule));
  put(render,0,g_base+0x3A49FD8);put(render,0x50,g_base+0x3A4A268);put(render,0x88,ptr(slotArray));put(render,0x90,ptr(slotArray)+8);put(slotArray,0,ptr(slot));put(slot,0x80,ptr(character));put(render,0x118,direct?ptr(character):std::uintptr_t(0));
  put(character,0,g_base+0x40602F8);put(character,0x698,g_base+0x3A71028);put(character,0x698+0x210,ptr(character));put(character,0x698+0x108,ptr(articulated));
  put(articulated,0,g_base+0x3A48B30);put(articulated,0x20,ptr(ne));put(articulated,0x28,std::int16_t(2));
  henry={he.data(),hs.data(),true};npc={ne.data(),ns.data(),true};other={oe.data(),os.data(),true};
  a={};a.gesture59c3=101;a.key=20;a.generation=1;a.epoch=2;a.request=3;a.sample=555;a.tick=2000;a.entityId=50;a.metadata59c7g.collider=ptr(articulated);g_hybrid_wait65h=true;};
 auto drain=[&](){DrainBooking59c6([&](Pending& p,auto& pendingLease){const void* henryActor=&henry;const void* hs=target_fixture19c3::henry.state;const auto now=now59c6;
#include "generated_hybrid_entry65h.inc"
  const bool liveRoute39=true,vrDirect596=false;const void*directTarget19c3=nullptr;const std::atomic<bool>g_sync59c10r9{true};
#include "generated_hybrid_target65h.inc"
  if(!doClaim){++nativeEntries;return;}
  motion59c4::ClaimReservationC2 hybridReservation54;const bool noRelease54=false;
#include "generated_hybrid_claim65h.inc"
#include "generated_hybrid_live65h.inc"
  hybridReservation54.nativeCallStarted();++nativeEntries; // explicit game-call leaf, not native acceptance
 });};
 for(bool direct:{false,true}){reset(direct);ckH8(RetainedContactBody19c3(a,ne.data()),"original articulated main is current despite distinct locomotion capsule");
  ckH8(Submit(a),"same original contact queues");const auto original=g_pending;drain();ckH8(g_has&&g_pending.targetWait19c3,"actual target stage waits with no selected/cached NPC");ActorSeen27(&npc);now59c6=2014;drain();ckH8(!g_has&&nativeEntries==1,"actual Hybrid target stage resumes original articulated contact once");ckH8(original.tick==2000&&original.sample==555&&original.key==20,"original impact/source clocks retained");
 }
 for(unsigned bad=0;bad<18;++bad){reset(false);Submit(a);drain();ActorSeen27(&npc);now59c6=2014;
  switch(bad){case 0:put(character,0x698+0x108,ptr(capsule));break;case 1:put(character,0x698+0x210,ptr(character)+8);break;case 2:put(articulated,0x20,ptr(oe));break;case 3:put(articulated,0x28,std::int16_t(1));break;case 4:put(articulated,0,g_base+0x3A82570);break;case 5:put(render,0x90,ptr(slotArray));break;case 6:put(render,0x118,ptr(character)+8);break;case 7:put(physical,0x30,ptr(oe));break;case 8:badPinH8=true;break;case 9:retail_build112::exactH8=false;break;case 10:++contact176::recoveryEpoch55c1;break;case 11:now59c6=2161;break;case 12:ownerValid=false;break;case 13:++generation;break;case 14:++request;break;case 15:put(ne,0xC,51u);break;case 16:mutateAtH8=ptr(character)+0x698+0x108;break;case 17:put(render,0,g_base+0x1234);break;}
  drain();ckH8(!nativeEntries&&!g_has,"changed actor/body/schema/source/original160ms refuses without native entry");
 }
 reset(false);Submit(a);drain();now59c6=2160;ActorSeen27(&npc);drain();ckH8(nativeEntries==1&&!g_has,"original160ms endpoint remains admissible without retimestamping");
 reset(false);Submit(a);drain();now59c6=1999;ActorSeen27(&npc);drain();ckH8(!nativeEntries&&!g_has,"future original contact cannot be repaired by current body ownership");
 for(int index=0;index<7;++index){reset(false);badPinIndexH8=index;ckH8(!RetainedContactBody19c3(a,ne.data()),"each exact retail schema mismatch refuses skeleton extension");}
 reset(false);put(ne,0xC,51u);ckH8(!RetainedContactBody19c3(a,ne.data()),"current entity identity cannot replace the original contact target");
 reset(false);a.metadata59c7g.collider=0x9876;unownedBodyH8=0x9876;ckH8(!RetainedContactBody19c3(a,ne.data())&&!unownedReadsH8,"saved collider not currently nominated is never dereferenced");
 reset(true);put(slot,0x80,std::uintptr_t(0x9876));ckH8(RetainedContactBody19c3(a,ne.data()),"valid native direct character retains priority over stale slot fallback");
 reset(false);ActorSeen27(&npc);a.targetWait19c3=true;const void *targetH8=nullptr,*entityH8=nullptr,*stateH8=nullptr;ckH8(ResolveDirectTarget19c3(a,&henry,hs.data(),now59c6,targetH8,entityH8,stateH8)==TargetResolution19c3::Ready,"same current skeleton relation also preserves direct target resolver");
 reset(false);a.metadata59c7g.collider=ptr(capsule);badPinH8=true;retail_build112::exactH8=false;ckH8(RetainedContactBody19c3(a,ne.data()),"legacy capsule relation unchanged and independent of skeleton extension pins");
 reset(false);doClaim=true;const motion59c4::Key k{a.gesture59c3,a.key,a.generation,a.epoch,a.request,20,0x1234};ckH8(motion59c4::hybridAttempts54.event(k,motion59c4::Event::Prepare,2000,2000,0,0),"actual native attempt ledger has original prepare");Submit(a);drain();now59c6=2014;ActorSeen27(&npc);drain();ckH8(nativeEntries==1,"current articulated owner reaches existing native-entry reservation exactly once");auto again=a;again.sample++;again.tick=2014;Submit(again);drain();ckH8(nativeEntries==1,"same gesture repeat cannot acquire another native-entry claim");
 std::printf("articulated_target_owner_h8=%u/%u actualQueueTargetClaim=EXTRACTED nativeExecution=NONE\n",checksH8-failuresH8,checksH8);return failuresH8?1:0;
}
