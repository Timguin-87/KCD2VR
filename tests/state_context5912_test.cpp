#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <mutex>
#include <cassert>
#include <limits>
#include <string>
#include <thread>
#include "vrik/motion_owned59c4.h"
#include "vrik/contact_authority176.h"
static std::uint64_t hostNow=1000;
static bool hostVerified=true,hostOwner=true,hostEffortOK=true;
static unsigned hostFailProp=~0u;
static unsigned hostExceptionProp=~0u,hostMismatchProp=~0u;
static int hostMismatchInt=0;static float hostMismatchFloat=0.f;
static bool hostMismatchOnce=false;
static unsigned hostPhasePublications=0;
static unsigned hostEffortCalls=0;
static unsigned hostOfferReads=0,hostOfferReadMask=7;
static bool hostCombatCapture=false;
std::uint64_t GetTickCount64(){return hostNow;}
std::map<std::pair<const void*,unsigned>,int> hostInts;
std::map<std::pair<const void*,unsigned>,float> hostFloats;
static const void* actor=reinterpret_cast<void*>(0x2000);
static const void* player=reinterpret_cast<void*>(0x1000);
static int I(const void* s,unsigned p){return hostInts[{s,p}];}
static float F(const void* s,unsigned p){return hostFloats[{s,p}];}
// Optional synthetic native memory/notification leaves for the actual H21 Tick
// regression. The complete production driver and offer qualifier stay intact.
#define KCD2VR_DIRECT_PREPARE_H21_TEST
static bool hostGraphH21=false,hostNativeEligibleH21=false;
static const void* hostGraphStateH21=nullptr;
static std::map<std::uintptr_t,unsigned char> hostMemoryH21;
template<class T>static void PutH21(std::uintptr_t p,T v){const auto* bytes=reinterpret_cast<const unsigned char*>(&v);for(std::size_t n=0;n<sizeof(v);++n)hostMemoryH21[p+n]=bytes[n];}
static bool HostDefenseIntentReadH21(std::uintptr_t p,void* out,std::size_t size){
 if(!hostGraphH21)return false;auto* bytes=static_cast<unsigned char*>(out);
 for(std::size_t n=0;n<size;++n){const auto it=hostMemoryH21.find(p+n);if(it==hostMemoryH21.end())return false;bytes[n]=it->second;}return true;}
static bool HostDefenseIntentSetH21(const void* s,unsigned prop,bool v){
 if(!hostGraphH21)return false;PutH21(std::uintptr_t(s)+prop+8,static_cast<unsigned char>(v));return true;}
static void HostPhaseNotifyH21(const void*,unsigned,int);
template<class T>T Rd(const void* s,std::size_t p,T d){
 if(hostGraphH21){T out{};return HostDefenseIntentReadH21(std::uintptr_t(s)+p,&out,sizeof(out))?out:d;}
 if(p==0xCE0)return reinterpret_cast<T>(actor);if(p==0x1F8)return reinterpret_cast<T>(player);return d;}
constexpr unsigned kActionPhaseProp39=0x30,kAttackType=0x210,kAttackStrength=0x1e0,kPendingZone=0x120,kAttackZone=0xf0,kCommittedZone=0x180,kExecutedZone=0x270,kAttackingHand=0x1b0,kDefenseZone=0x150;
static bool hostRowAvailable=true,hostQueryTakeover=false,hostRowMinusOne=false,hostRowFallback=false; // R12: fallback case publishes without a defense write
static unsigned hostQueries=0;
static bool HostResolveDefense(const void* hs,int committed,const void*& row,int& value){
 ++hostQueries;assert(I(hs,kAttackZone)==committed&&I(hs,kCommittedZone)==committed);
 if(hostQueryTakeover){hostInts[{hs,kActionPhaseProp39}]=32;hostInts[{hs,kDefenseZone}]=42;}
 row=hostRowAvailable?reinterpret_cast<void*>(std::uintptr_t(0x6000+committed*0x100)):nullptr;
 value=hostRowMinusOne?-1:(committed==5?0:1);return hostRowAvailable;
}
static void HostWatchPublication(const void* hs,unsigned prop,int value){
 if(prop==kActionPhaseProp39&&(value==4||value==8)){
  ++hostPhasePublications;
  if(!hostRowFallback)assert(I(hs,kDefenseZone)==(hostRowMinusOne?-1:(I(hs,kCommittedZone)==5?0:1)));
  assert(F(hs,0x450)>=0.f&&F(hs,0x450)<=.600001f); // expired/missing predictions stay zero
 }
}
static bool hostGuardOwned=false; // R59.16C10R16: our posture guard holds the block input
namespace frik_pose_probe {static bool GuardOwnedByUs59c7e(const void*){return hostGuardOwned;}static int GeometryOwnerStage49(){return 0;}static bool GeometryOwnerValid178r30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t){return hostOwner;}}
namespace diagnostics::capture168 {enum class Domain {Combat};static bool active(Domain,std::uint64_t){return hostCombatCapture;}}
namespace diagnostic_trace {static std::string last,offerRetired;static void Write(const char* s){last=s;if(last.find("FORWARD_OFFER_RETIRED")!=std::string::npos)offerRetired=last;}}
// This extracted state-controller fixture has no live native actor graph.
// Fail closed at these external leaves; quiet_hit_feedback168 exercises the complete
// production follow-up ownership helper with an explicit actor/opponent fixture.
static bool CombatActor(const void* a,const void*& e,const void*& s,bool) noexcept {
 if(!hostGraphH21||a!=actor)return false;e=player;s=hostGraphStateH21;return true;}
static bool IsPlayer(const void* e) noexcept {return hostGraphH21&&e==player;}
static const void* hostNativeAction=nullptr;
static const void* LiveAttackAction178r35(const void*) noexcept {return hostNativeAction;}
#include "state_driver5912.inc"
static void HostPhaseNotifyH21(const void*,unsigned prop,int phase){
 if(!hostGraphH21||prop!=kActionPhaseProp39||phase!=4)return;
 auto* scope=vrstate596::t_defense_intent19c4;
 assert(scope&&scope->qualified);
 // Native outcome is a leaf, never awarded by the production driver. The
 // separate defense-intent fixture exercises actual CaptureOffer as well.
 scope->offerObserved=true;scope->active=hostNativeEligibleH21?1:0;
 scope->trigger=reinterpret_cast<void*>(0x6000);
 PutH21(0x6019,static_cast<unsigned char>(hostNativeEligibleH21));
}
int main(){
 namespace mo=kcd2vr::motion59c4;namespace vs=vrstate596;
 vs::g_npc_block_speed18=1.f; // compatibility input does not disable the direct contact prediction
 // Exact byte-property receipts must not interpret the three padding bytes as
 // part of the bool. These are the raw 32-bit values seen in the C10R3 log.
 {std::array<unsigned char,0x700> memory{};constexpr std::uintptr_t vft=0x3B0A6A0;
  unsigned reads=0;const void* fail=nullptr;
  auto copy=[&](const void* from,void* to,std::size_t n){++reads;if(from==fail)return false;std::memcpy(to,from,n);return true;};
  for(const auto prop:{0x300u,0x680u}){
   std::memcpy(memory.data()+prop,&vft,sizeof(vft));
   const std::uint32_t padded=prop==0x300?0x43360000u:0x45300000u;
   std::memcpy(memory.data()+prop+8,&padded,4);
   assert(std::strcmp(vs::TypedDefenseBoolDiagnostic(memory.data(),prop,vft,true,copy),"0")==0);
   memory[prop+8]=1;
   assert(std::strcmp(vs::TypedDefenseBoolDiagnostic(memory.data(),prop,vft,true,copy),"1")==0);
   assert(std::strcmp(vs::TypedDefenseBoolDiagnostic(memory.data(),prop,vft+8,true,copy),"UNKNOWN")==0);
   fail=memory.data()+prop+8;
   assert(std::strcmp(vs::TypedDefenseBoolDiagnostic(memory.data(),prop,vft,true,copy),"UNKNOWN")==0);fail=nullptr;
  }
  reads=0;
  assert(std::strcmp(vs::TypedDefenseBoolDiagnostic(memory.data(),0x300,vft,false,copy),"UNKNOWN")==0&&reads==0);
  assert(std::strcmp(vs::TypedDefenseBoolDiagnostic(nullptr,0x300,vft,true,copy),"UNKNOWN")==0&&reads==0);
  assert(std::strcmp(vs::TypedDefenseBoolDiagnostic(memory.data(),0x390,vft,true,copy),"UNKNOWN")==0&&reads==0);
  std::puts("Typed defense receipts ignore padding and preserve UNKNOWN for unsupported types/reads/retail.");}
 int dummy=0;const void* hs=&dummy;mo::selected=true;vs::g_consequence=true;
 mo::Key a{1,77,4,1,1,kcd2vr::contact176::recoveryEpoch55c1.load(),std::uintptr_t(player)},b=a;b.gesture=2;
 hostInts[{hs,kActionPhaseProp39}]=1;
 auto event=[&](const mo::Key& k,mo::Event e,int kind,int z,int tech,std::uint64_t t){hostNow=t;
  assert(mo::attempts.event(k,e,t,t,z,tech));assert(vs::Event(k,t,kind,z,tech));vs::Tick(hs,t);};
 event(a,mo::Event::Prepare,1,2,0,1000);event(a,mo::Event::Commit,2,2,0,1010);
 assert(I(hs,kDefenseZone)==1&&I(hs,kAttackZone)==2); // native row, not F0 copy
 event(b,mo::Event::Prepare,1,5,1,1560);event(b,mo::Event::Commit,2,5,1,1570);
 const auto oldClock=vs::g_since.load();hostNow=1590;mo::Attempt claimed;
 assert(mo::attempts.claim(a,1540,hostNow,55,claimed)==mo::Claim::Ready);
 {vs::ContactScope5912 contact;assert(contact.begin(hs,claimed));
  assert(vs::g_gesture==1&&I(hs,kExecutedZone)==2&&vs::g_technique==0);
  assert(I(hs,kDefenseZone)==1);
  assert(contact.setStrength(.2f)&&contact.current()&&F(hs,kAttackStrength)==.2f);
  contact.afterHit();assert(vs::g_hit_at5910c1==1590);
 }
 assert(vs::g_gesture==2&&I(hs,kExecutedZone)==5&&vs::g_technique==1&&vs::g_hit_at5910c1==0&&vs::g_since==oldClock);
 assert(I(hs,kDefenseZone)==0&&vs::g_written_defense==0); // B's row restored
 assert(F(hs,kAttackStrength)==1.f);
 assert(mo::attempts.claim(a,1540,hostNow,55,claimed)==mo::Claim::Duplicate);
 std::puts("A contact uses A context; B restored with original clock and no false hit.");
 // A committed cancellation remains cancelled even with a fresh collision.
 assert(mo::attempts.event(b,mo::Event::Cancel,1591,1591,-1,1));
 assert(mo::attempts.contact(b,1591,1592,55,1,claimed)==mo::Claim::Cancelled);
 mo::Key c=a;c.gesture=3;assert(mo::attempts.event(c,mo::Event::Prepare,1600,1600,2,0));
 // R59.16C7A: the CONTACT decides the technique (a thrust from a cut wind-up is the same swing), still once.
 assert(mo::attempts.contact(c,1610,1612,55,1,claimed)==mo::Claim::Ready&&claimed.technique==1);
 assert(mo::attempts.contact(c,1610,1612,55,0,claimed)==mo::Claim::Duplicate);
 // A claim that could not be delivered is given back once; the next contact of the same swing lands.
 mo::attempts.release(c);assert(mo::attempts.contact(c,1611,1612,55,1,claimed)==mo::Claim::Identity); // released to Committed: contact() needs Prepared
 assert(mo::attempts.claim(c,1611,1612,55,claimed)==mo::Claim::Ready);
 assert(mo::attempts.claim(c,1611,1612,55,claimed)==mo::Claim::Duplicate);
 // Codex review: a Cancel that arrives while the claim is reserved survives release().
 mo::Key dk=a;dk.gesture=4;assert(mo::attempts.event(dk,mo::Event::Prepare,1620,1620,2,0));
 assert(mo::attempts.contact(dk,1625,1626,55,0,claimed)==mo::Claim::Ready);
 assert(!mo::attempts.event(dk,mo::Event::Cancel,1627,1627,-1,0));
 mo::attempts.release(dk);assert(mo::attempts.claim(dk,1628,1629,55,claimed)==mo::Claim::Cancelled);

 mo::Key unknown=c;unknown.gesture=20;assert(mo::attempts.contact(unknown,1610,1612,55,0,claimed)==mo::Claim::Identity);
 std::puts("Early contact commits a prepared attempt once; unknown/cancelled/wrong-technique remain invalid.");
 // Native takeover during a borrowed context must win.
 hostNow=1612;
 mo::Attempt borrowed{c,mo::Stage::Consumed,1600,1610,2,0,55};
 {vs::ContactScope5912 contact;assert(contact.begin(hs,borrowed));assert(contact.setStrength(1.1f)&&contact.current());
  hostInts[{hs,kActionPhaseProp39}]=32;assert(!contact.setStrength(.2f));}
 assert(I(hs,kActionPhaseProp39)==32&&vs::g_stage==0);
 // Protected native state cannot be converted into offensive authority.
 // R59.16C10R16: Henry's hit reaction (64) stays protected (VrStrikeDuringStagger off); 32 (recovery after a hit/block) is a start
 // phase only while VrDirectStrikeInRecovery is on.
 hostInts[{hs,kActionPhaseProp39}]=64;
 {vs::ContactScope5912 contact;assert(!contact.begin(hs,borrowed));assert(I(hs,kActionPhaseProp39)==64);}
 hostInts[{hs,kActionPhaseProp39}]=32;vs::g_accept_recovery59c16=false;
 {vs::ContactScope5912 contact;assert(!contact.begin(hs,borrowed));assert(I(hs,kActionPhaseProp39)==32);}
 vs::g_accept_recovery59c16=true;
 // Wrong player cannot borrow this actor.
 borrowed.key.player=0x3000;hostInts[{hs,kActionPhaseProp39}]=1;
 {vs::ContactScope5912 contact;assert(!contact.begin(hs,borrowed));}borrowed.key.player=std::uintptr_t(player);
 // Setter failure disables the driver. No damage caller may proceed after this.
 hostFailProp=kExecutedZone;
 {vs::ContactScope5912 contact;assert(!contact.begin(hs,borrowed));}assert(vs::g_fault);
 std::puts("Native takeover preserved; foreign player and setter failure rejected.");
 // A later successful begin followed by partial restore must remain latched beyond the old 3 s grace.
 vs::g_fault=false;hostFailProp=~0u;vs::Forget();hostInts[{hs,kActionPhaseProp39}]=1;
 mo::Key e=a;e.gesture=30;event(e,mo::Event::Prepare,1,5,1,5000);event(e,mo::Event::Commit,2,5,1,5010);
 borrowed.commit=5005;
 {vs::ContactScope5912 contact;assert(contact.begin(hs,borrowed));hostFailProp=kExecutedZone;}
 assert(vs::g_fault&&vs::g_stage==0);hostFailProp=~0u;hostNow=9010;vs::Tick(hs,hostNow);
 assert(vs::g_fault&&vs::g_stage==0);
 // A failed preparation-cost adapter does not revoke otherwise qualified contact admission (C7D policy).
 vs::g_fault=false;vs::Forget();hostInts[{hs,kActionPhaseProp39}]=1;hostNow=10000;
 mo::Key failed=a;failed.gesture=31;
 assert(mo::attempts.event(failed,mo::Event::Prepare,10000,10000,2,0));
 assert(mo::attempts.contact(failed,10001,10001,55,0,claimed)==mo::Claim::Ready);
 hostNow=10001;
 hostEffortOK=false;
  {vs::ContactScope5912 contact;assert(contact.begin(hs,claimed));assert(contact.setStrength(1.1f)&&contact.current());
   contact.afterHit();assert(vs::g_hit_at5910c1==hostNow);}
 assert(mo::attempts.claim(failed,10001,10002,55,claimed)==mo::Claim::Duplicate);
 assert(!vs::g_fault);
 // A fresh gesture is allowed to try again; zero resource is not simulated as an ABI failure.
 hostEffortOK=true;
 // Queue is bounded and preserves existing events rather than overwriting.
 vs::g_fault=false;vs::Forget();vs::Reset();hostFailProp=~0u;hostInts[{hs,kActionPhaseProp39}]=1;hostNow=1700;vs::Tick(hs,hostNow);
 for(int i=0;i<8;++i)assert(vs::Event(a,hostNow,1,2,0));
 assert(!vs::Event(b,hostNow,4,5,1));assert(vs::g_event_drops5912==1);
 // R59.16C10R12: with the row fallback ON (default) a missing row publishes the phase WITHOUT writing the defense zone.
 vs::Reset();vs::Tick(hs,hostNow);vs::Forget();hostRowAvailable=false;hostRowFallback=true;vs::g_no_row_fallback59c12=true;
 hostInts[{hs,kActionPhaseProp39}]=1;hostInts[{hs,kDefenseZone}]=7;
 hostInts[{hs,kAttackZone}]=3;hostInts[{hs,kCommittedZone}]=3;
 hostInts[{hs,kPendingZone}]=-1;hostInts[{hs,kAttackType}]=-1;hostFloats[{hs,kAttackStrength}]=0.f;
 vs::g_key5912=a;vs::g_gesture=a.gesture;
 assert(vs::Publish(hs,4,2,0,hostNow+600)&&!vs::g_fault&&vs::g_stage==4);
 assert(I(hs,kActionPhaseProp39)==4&&I(hs,kDefenseZone)==7&&I(hs,kAttackZone)==2&&!vs::g_defense_owned);
 assert(vs::Publish(hs,0,2,0,0)&&vs::g_stage==0&&I(hs,kDefenseZone)==7);
 hostRowFallback=false;vs::g_no_row_fallback59c12=false;
 // C10C1: missing table rows do not publish phase4 or leak a partial context (fallback OFF).
 vs::Reset();vs::Tick(hs,hostNow);vs::Forget();hostRowAvailable=false;
 hostInts[{hs,kActionPhaseProp39}]=1;hostInts[{hs,kDefenseZone}]=7;
 hostInts[{hs,kAttackZone}]=3;hostInts[{hs,kCommittedZone}]=3;
 hostInts[{hs,kPendingZone}]=-1;hostInts[{hs,kAttackType}]=-1;hostFloats[{hs,kAttackStrength}]=0.f;
 vs::g_key5912=a;vs::g_gesture=a.gesture;
 assert(!vs::Publish(hs,4,2,0,hostNow+600)&&!vs::g_fault&&vs::g_stage==0);
 assert(I(hs,kActionPhaseProp39)==1&&I(hs,kDefenseZone)==7&&I(hs,kAttackZone)==3&&I(hs,kCommittedZone)==3);
 assert(I(hs,kPendingZone)==-1&&I(hs,kAttackType)==-1&&F(hs,kAttackStrength)==0.f);
 // The native table may deliberately return -1; do not fabricate its meaning.
 hostRowAvailable=true;hostRowMinusOne=true;vs::g_key5912=a;
 assert(vs::Publish(hs,4,2,0,hostNow+600)&&I(hs,kDefenseZone)==-1);
 assert(vs::Publish(hs,0,-1,0)&&I(hs,kDefenseZone)==7);
 // No repeated query is needed on ordinary idle/tick paths.
 hostRowMinusOne=false;vs::g_key5912=a;assert(vs::Publish(hs,4,2,0,hostNow+600));
 auto queries=hostQueries;vs::Tick(hs,hostNow+1);assert(hostQueries==queries);
 // If native changes +150, cleanup must not overwrite that new value.
 hostInts[{hs,kDefenseZone}]=4;assert(vs::Publish(hs,0,-1,0)&&I(hs,kDefenseZone)==4);
 // A native phase takeover during the query wins over our pending publication.
 vs::Forget();hostInts[{hs,kActionPhaseProp39}]=1;hostQueryTakeover=true;vs::g_key5912=a;
 assert(!vs::Publish(hs,4,2,0,hostNow+600)&&I(hs,kActionPhaseProp39)==32&&I(hs,kDefenseZone)==42&&vs::g_stage==0);
 hostQueryTakeover=false;
 std::puts("Native defense row precedes phase publication; missing row rollback, borrowed row restoration and native takeover verified.");
 // C10R2: the offer clock is bounded by the VR attempt lifetime; normal exit restores -1.
 // R59.16C10R4B (S33): the published value is the time to CONTACT (prepare 0.25 s, strike 0.12 s, counting down to a
 // zero with no floor), because the native follower requires a fixed predicted sync point.
 vs::g_eta_prepare59c10.store(.25f);vs::g_eta_strike59c10.store(.12f);
 vs::Forget();hostInts[{hs,kActionPhaseProp39}]=1;hostFloats[{hs,0x450}]=-1.f;hostNow=20000;a.gesture=50;mo::attempts.clear();assert(mo::attempts.event(a,mo::Event::Prepare,hostNow,hostNow,2,0));vs::g_key5912=a;
 assert(vs::AttemptDeadline(20000,20000)==20600&&vs::AttemptDeadline(19400,20000)==0);
 assert(vs::Publish(hs,4,2,0,20600)&&F(hs,0x450)>.249f&&F(hs,0x450)<.251f);
 auto guardContext=a;guardContext.gesture=0;assert(vs::QueryAttackOwnership(guardContext,hostNow)==mo::AttackOwnership::Owned);
 auto wrongContext=guardContext;++wrongContext.weapon;assert(vs::QueryAttackOwnership(wrongContext,hostNow)==mo::AttackOwnership::None);
 assert(vs::QueryAttackOwnership(guardContext,hostNow+151)==mo::AttackOwnership::None);
 vs::g_attack_snapshot.revision.fetch_add(1); // deterministic writer-in-progress sample
 assert(vs::QueryAttackOwnership(guardContext,hostNow)==mo::AttackOwnership::Unknown);
 vs::g_attack_snapshot.revision.fetch_add(1);
 assert(vs::QueryAttackOwnership(guardContext,hostNow)==mo::AttackOwnership::Owned);
 // A concurrent refresh of an unchanged live owner may be Owned or Unknown,
 // but must never grant guard admission by reporting a certain None.
 std::atomic<bool> startRefresh{false};
 std::thread refresher([&]{while(!startRefresh.load()){}for(unsigned i=0;i<10000;++i)vs::SnapshotAttack(hostNow);});
 startRefresh=true;for(unsigned i=0;i<10000;++i)assert(vs::QueryAttackOwnership(guardContext,hostNow)!=mo::AttackOwnership::None);
 refresher.join();
 hostNow=20100;vs::Tick(hs,hostNow);assert(F(hs,0x450)>.149f&&F(hs,0x450)<.151f);
 hostNow=20200;assert(vs::Publish(hs,4,5,1)&&vs::g_deadline==20600&&F(hs,0x450)<.401f);
 hostNow=20600;vs::Tick(hs,hostNow);assert(vs::g_stage==0&&F(hs,0x450)==-1.f);
 assert(vs::QueryAttackOwnership(guardContext,hostNow)==mo::AttackOwnership::None);
 assert(!vs::Publish(hs,4,2,0,20600)&&I(hs,kActionPhaseProp39)==1);
 // Actual commit grants exactly the existing commit lifetime once; a delayed
 // contact borrows its own remaining time and returns B's elapsed deadline.
 hostNow=21000;++a.gesture;mo::attempts.clear();assert(mo::attempts.event(a,mo::Event::Prepare,hostNow,hostNow,2,0));vs::g_key5912=a;assert(vs::Publish(hs,4,2,0,21600));
 hostNow=21100;assert(vs::Publish(hs,8,2,0,21700));
 hostNow=21200;vs::Tick(hs,hostNow);assert(F(hs,0x450)>.049f&&F(hs,0x450)<.051f); // original onset 21000 + .25, never re-anchored at commit
 const auto savedDeadline=vs::g_deadline;borrowed.key=b;borrowed.commit=21100;borrowed.origin=5;borrowed.technique=1;
 {vs::ContactScope5912 contact;assert(contact.begin(hs,borrowed));hostNow=21240;assert(contact.current());}
 assert(vs::g_deadline==savedDeadline&&vs::g_key5912.gesture==a.gesture&&F(hs,0x450)>.009f&&F(hs,0x450)<.011f);
 assert(vs::QueryAttackOwnership(guardContext,hostNow)==mo::AttackOwnership::Owned);
 assert(vs::Publish(hs,0,-1,0)&&F(hs,0x450)==-1.f);
 // A changed native countdown is preserved; no stale lease overwrites it.
 hostNow=22000;vs::g_key5912=a;assert(vs::Publish(hs,4,2,0,22600));
 hostFloats[{hs,0x450}]=.123f;assert(vs::Publish(hs,0,-1,0)&&F(hs,0x450)==.123f);
 vs::Forget();vs::g_key5912=a;assert(vs::Publish(hs,4,2,0,22600));
 hostInts[{hs,kActionPhaseProp39}]=32;vs::Tick(hs,hostNow);
 assert(diagnostic_trace::last.find("reason=NATIVE_TOOK_PHASE livePhase=32 writtenPhase=4")!=std::string::npos);
 assert(vs::g_stage==0&&I(hs,kActionPhaseProp39)==32&&F(hs,0x450)==.123f);
 // A native timing write wins even when its phase changes on the same tick.
 hostInts[{hs,kActionPhaseProp39}]=1;vs::g_key5912=a;assert(vs::Publish(hs,4,2,0,22600));
 hostFloats[{hs,0x450}]=.217f;hostInts[{hs,kActionPhaseProp39}]=256;vs::Tick(hs,hostNow);
 assert(F(hs,0x450)==.217f&&I(hs,kActionPhaseProp39)==256&&vs::g_stage==0);
 assert(vs::QueryAttackOwnership(guardContext,hostNow)==mo::AttackOwnership::None);
 std::puts("Finite offer countdown, non-renewing feints, A/B restoration, expiry and takeover receipts verified.");
 // Candidate E review: a readable synchronous takeover does not permanently
 // disable combat, but it cancels THIS attempt and restores only owned leases.
 vs::Forget();mo::attempts.clear();hostNow=23000;hostInts[{hs,kActionPhaseProp39}]=1;
 hostFloats[{hs,0x450}]=-1.f;hostInts[{hs,kDefenseZone}]=3;
 mo::Key next=a;next.gesture=100;assert(mo::attempts.event(next,mo::Event::Prepare,hostNow,hostNow,2,0));
 vs::g_key5912=next;hostMismatchProp=kActionPhaseProp39;hostMismatchInt=32;
 assert(!vs::Publish(hs,4,2,0,23600)&&!vs::g_fault&&vs::g_stage==0);
 assert(I(hs,kActionPhaseProp39)==32&&I(hs,kDefenseZone)==3&&F(hs,0x450)==-1.f);
 assert(mo::attempts.contact(next,23001,23001,55,0,claimed)==mo::Claim::Cancelled);
 hostMismatchProp=~0u;hostInts[{hs,kActionPhaseProp39}]=1;hostNow=23010;++next.gesture;
 assert(mo::attempts.event(next,mo::Event::Prepare,hostNow,hostNow,2,0));vs::g_key5912=next;
 assert(vs::Publish(hs,4,2,0,23610)&&!vs::g_fault);assert(vs::Publish(hs,0,-1,0));
 // Failed memory reads are not mismatches (even if the fallback might equal a
 // valid native phase), and an exception remains latched after arbitrary time.
 ++next.gesture;vs::g_key5912=next;hostFailProp=kPendingZone;
 assert(!vs::Publish(hs,4,2,0,23610)&&vs::g_fault);hostFailProp=~0u;
 vs::g_fault=false;vs::Forget();vs::g_key5912=next;hostExceptionProp=kPendingZone;
 assert(!vs::Publish(hs,4,2,0,23610)&&vs::g_fault&&vs::g_setter_exception);
 hostExceptionProp=~0u;hostNow=27000;vs::Tick(hs,hostNow);assert(vs::g_fault);
 std::puts("Readable native setter takeover cancels one attempt and permits a fresh one; unreadable/exception stay latched.");
 // Producer queued both events before the game thread. Prepare yields to an
 // idle native phase (not a protected phase), so only the ledger tombstone can
 // prevent the queued Commit from creating a second/ghost native publication.
 hostNow=28002;hostInts[{hs,kActionPhaseProp39}]=1;hostFloats[{hs,0x450}]=-1.f;
 vs::g_fault=false;vs::g_setter_exception=false;vs::Forget();mo::attempts.clear();
 next.gesture=200;
 assert(mo::attempts.event(next,mo::Event::Prepare,28000,28000,2,0));
 assert(mo::attempts.event(next,mo::Event::Commit,28001,28001,2,0));
 assert(vs::Event(next,28000,1,2,0)&&vs::Event(next,28001,2,2,0));
 hostMismatchProp=kActionPhaseProp39;hostMismatchInt=1;hostMismatchOnce=true;
 const auto publications=hostPhasePublications;vs::Tick(hs,hostNow);
 assert(!vs::g_fault&&vs::g_stage==0&&I(hs,kActionPhaseProp39)==1&&hostPhasePublications==publications+1);
 assert(!mo::attempts.publicationAllowed(next,mo::Event::Commit,28001,hostNow));
 assert(mo::attempts.claim(next,28002,28002,55,claimed)==mo::Claim::Cancelled);
 assert(vs::QueryAttackOwnership(guardContext,hostNow)==mo::AttackOwnership::None);
 // A different attempt can still publish through the real event queue.
 hostMismatchOnce=false;++next.gesture;event(next,mo::Event::Prepare,1,2,0,28100);
 assert(vs::g_stage==4&&I(hs,kActionPhaseProp39)==4&&!vs::g_fault);
 assert(vs::Publish(hs,0,-1,0));
 std::puts("Same-batch cancelled Commit cannot republish; concurrent snapshot refresh never masquerades as no owner.");
 #include "state_telegraph59c10_cases.inl"
 {// R59.16C10R16: Henry's own guard phase over a published attack is reclaimed (3x), a native +450 rewrite is adopted,
  // a foreign phase still wins, and the reclaim switch restores the old yield.
  vs::g_fault=false;vs::g_setter_exception=false;hostMismatchProp=~0u;hostFailProp=~0u;hostExceptionProp=~0u;vs::Forget();mo::attempts.clear();
  hostNow=40000;hostInts[{hs,kActionPhaseProp39}]=1;hostFloats[{hs,0x450}]=-1.f;vs::g_telegraph_strike59c10=false; // R16 default
  hostGuardOwned=true;
  mo::Key r=a;r.gesture=400;event(r,mo::Event::Prepare,1,2,0,40000);
  assert(vs::g_stage==4&&I(hs,kActionPhaseProp39)==4);
  for(int i=0;i<3;++i){hostInts[{hs,kActionPhaseProp39}]=2;hostNow+=10;vs::Tick(hs,hostNow);
   assert(vs::g_stage==4&&I(hs,kActionPhaseProp39)==4&&!vs::g_fault);}
  hostInts[{hs,kActionPhaseProp39}]=2;hostNow+=10;vs::Tick(hs,hostNow);
  assert(vs::g_stage==0&&I(hs,kActionPhaseProp39)==2); // fourth takeover in one attempt: the guard wins
  std::puts("R16: own guard phase reclaimed three times per attempt, then yielded.");
  hostInts[{hs,kActionPhaseProp39}]=1;mo::Key q=a;q.gesture=401;event(q,mo::Event::Prepare,1,2,0,hostNow+10);
  assert(vs::g_stage==4);hostInts[{hs,kActionPhaseProp39}]=64;hostNow+=10;vs::Tick(hs,hostNow);
  assert(vs::g_stage==0&&I(hs,kActionPhaseProp39)==64); // a foreign phase (hit reaction) is never reclaimed
  hostInts[{hs,kActionPhaseProp39}]=1;mo::Key w=a;w.gesture=402;event(w,mo::Event::Prepare,1,2,0,hostNow+10);
  assert(vs::g_stage==4);hostFloats[{hs,0x450}]=.33f;hostNow+=10;vs::Tick(hs,hostNow);
  assert(vs::g_stage==0&&I(hs,kActionPhaseProp39)==4&&F(hs,0x450)==.33f); // native timing takeover wins without stale overwrite
  hostInts[{hs,kActionPhaseProp39}]=1;
  vs::g_reclaim_guard59c16=false;mo::Key y=a;y.gesture=403;event(y,mo::Event::Prepare,1,2,0,hostNow+10);
  assert(vs::g_stage==4);hostInts[{hs,kActionPhaseProp39}]=2;hostNow+=10;vs::Tick(hs,hostNow);
  assert(vs::g_stage==0&&I(hs,kActionPhaseProp39)==2);vs::g_reclaim_guard59c16=true;
  // Without our guard holding the block input, the same phase 2 is a native writer: yield, never overwrite.
  hostGuardOwned=false;hostInts[{hs,kActionPhaseProp39}]=1;mo::Key z=a;z.gesture=404;event(z,mo::Event::Prepare,1,2,0,hostNow+10);
  assert(vs::g_stage==4);hostInts[{hs,kActionPhaseProp39}]=2;hostNow+=10;vs::Tick(hs,hostNow);
  assert(vs::g_stage==0&&I(hs,kActionPhaseProp39)==2);
  // A cancelled attempt is never reclaimed: the guard keeps its phase.
  hostGuardOwned=true;hostInts[{hs,kActionPhaseProp39}]=1;mo::Key c2=a;c2.gesture=405;event(c2,mo::Event::Prepare,1,2,0,hostNow+10);
  assert(vs::g_stage==4);hostInts[{hs,kActionPhaseProp39}]=2;hostNow+=10;
  assert(mo::attempts.event(c2,mo::Event::Cancel,hostNow,hostNow,2,0)&&vs::Event(c2,hostNow,4,2,0));vs::Tick(hs,hostNow);
  assert(vs::g_stage==0&&I(hs,kActionPhaseProp39)==2);hostGuardOwned=false;
  std::puts("R16: foreign phase wins, +450 rewrite preserved, switch off / native phase 2 / cancelled attempt all yield.");}
 {// C10R18: a real contact whose published attempt lost its phase to a non-reclaimable guard/idle phase still lands (the R14B
  // CONTEXT_NOT_OWNED skips); with VrDirectOwnsHits off the old skip remains. One block-speed constant for the whole attempt.
  vs::g_fault=false;vs::g_setter_exception=false;hostMismatchProp=~0u;hostFailProp=~0u;hostExceptionProp=~0u;vs::Forget();mo::attempts.clear();
  hostNow=50000;hostInts[{hs,kActionPhaseProp39}]=1;hostFloats[{hs,0x450}]=-1.f;hostGuardOwned=false;vs::g_npc_block_speed18=1.5f;
  vs::g_telegraph_strike59c10=true;vs::g_own_hits18=true;
  mo::Key k18=a;k18.gesture=500;event(k18,mo::Event::Prepare,1,2,0,50000);
  assert(vs::g_stage==4&&F(hs,0x450)>.249f&&F(hs,0x450)<.251f);
  hostNow=50012;vs::Tick(hs,hostNow);assert(vs::g_stage==4&&F(hs,0x450)>.237f&&F(hs,0x450)<.239f); // H21: real Commit owns8, original deadline
  event(k18,mo::Event::Commit,2,2,0,50030);assert(vs::g_stage==8&&F(hs,0x450)>.219f&&F(hs,0x450)<.221f);
  hostInts[{hs,kActionPhaseProp39}]=2;hostNow=50060;mo::Attempt c18;assert(mo::attempts.claim(k18,50060,hostNow,55,c18)==mo::Claim::Ready);
  const auto refreshed=vs::g_contact_refreshed18.load();
  {vs::ContactScope5912 scope;assert(scope.begin(hs,c18));assert(I(hs,kActionPhaseProp39)==8&&scope.current());scope.afterHit();}
  assert(vs::g_contact_refreshed18.load()==refreshed+1);assert(vs::Publish(hs,16,2,0)&&vs::Publish(hs,0,-1,0));
  vs::g_own_hits18=false;hostInts[{hs,kActionPhaseProp39}]=1;mo::Key k19=a;k19.gesture=501;hostNow=51000;event(k19,mo::Event::Prepare,1,2,0,51000);
  event(k19,mo::Event::Commit,2,2,0,51030);assert(vs::g_stage==8);hostInts[{hs,kActionPhaseProp39}]=2;hostNow=51060;
  assert(mo::attempts.claim(k19,51060,hostNow,55,c18)==mo::Claim::Ready);
  {vs::ContactScope5912 scope;assert(!scope.begin(hs,c18));}assert(I(hs,kActionPhaseProp39)==2&&vs::g_stage==0);
  vs::g_own_hits18=true;vs::g_npc_block_speed18=0.f;
  std::puts("C10R18: a contact lost to a guard/idle phase is published fresh and lands; switch off keeps the old skip.");}
 #include "state_prediction19c3_cases.inl"
 { // C35: a legacy rear signal is not admission; an owned withdrawn key is not a new onset.
  vs::g_fault=false;vs::g_setter_exception=false;vs::Forget();vs::Reset();mo::attempts.clear();
  vs::g_prediction_history19c3={};hostNativeAction=nullptr;hostGuardOwned=false;
  hostFailProp=hostExceptionProp=hostMismatchProp=~0u;hostNow=100000;
  hostInts[{hs,kActionPhaseProp39}]=1;hostFloats[{hs,0x450}]=-1.f;vs::Tick(hs,hostNow);
  mo::Key k=a;k.gesture=1200;vs::g_telegraph_strike59c10=false;
  assert(!vs::Event(k,hostNow,5,2,0)&&vs::g_ev_count==0&&vs::g_stage==0);
  event(k,mo::Event::Prepare,1,2,0,hostNow);const auto prediction=vs::g_prediction19c3;
  hostNow=100050;assert(vs::Publish(hs,16,2,0));const auto phaseWrites=hostPhasePublications;
  event(k,mo::Event::Commit,2,2,0,100100);
  assert(vs::g_stage==16&&hostPhasePublications==phaseWrites&&
         vs::g_prediction19c3.onset==prediction.onset&&vs::g_prediction19c3.deadline==prediction.deadline);
  // A genuinely different forward gesture still starts normally, with its own original time.
  ++k.gesture;event(k,mo::Event::Prepare,1,2,0,100110);event(k,mo::Event::Commit,2,2,0,100150);
  assert(vs::g_stage==8&&vs::g_prediction19c3.onset==100110&&vs::g_prediction19c3.deadline==100360);
  std::puts("C35 actual driver: kind5 refused before admission, withdrawn key never reopened, next genuine forward key eligible.");
 }
 // H21 causal regression through actual production Event -> Tick -> Publish:
 // both ACTIVE and refused native offers keep4 despite legacy true; subsequent
 // Tick cannot mint8; actual Commit and checked contact still can publish8.
 for(bool eligible:{false,true})for(bool legacy:{false,true})for(bool contact:{false,true}){
  vs::Forget();vs::g_fault=false;vs::g_reset=false;mo::attempts.clear();
  vs::g_prediction_history19c3={};vs::g_defense_transitions19c4={};
  hostNow=200000+(eligible?10000:0)+(legacy?2000:0)+(contact?1000:0);
  hostGuardOwned=false;hostNativeAction=nullptr;hostOwner=true;
  hostGraphH21=true;hostNativeEligibleH21=eligible;hostGraphStateH21=hs;hostMemoryH21.clear();
  vs::g_defense_intent_hooks19c4=true;vs::g_henry_actor=actor;vs::g_telegraph_strike59c10=legacy;
  constexpr std::uintptr_t base=0x180000000ull,ta=0x4000,ts=0x5000,tr=0x6000;
  PutH21(std::uintptr_t(actor),base+0x3A80610);PutH21(std::uintptr_t(player),base+0x3AA4B98);
  PutH21(std::uintptr_t(actor)+0x210,std::uintptr_t(hs));PutH21(std::uintptr_t(actor)+0x1F8,std::uintptr_t(player));
  PutH21(std::uintptr_t(player)+0x278,std::uintptr_t(actor));PutH21(std::uintptr_t(hs)+0xCE0,std::uintptr_t(actor));
  PutH21(std::uintptr_t(hs)+0xCF8,ta);PutH21(ta,base+0x3E3FAC8);PutH21(ta+0x210,ts);
  PutH21(ts+0xCE0,ta);PutH21(ts+0xCF8,std::uintptr_t(actor));
  PutH21(tr,base+0x3B813F8);PutH21(tr+0x38,std::uintptr_t(actor));PutH21(tr+8,ta);
  for(unsigned prop:{0x4B0u,0x4E0u,0x5A0u}){
   PutH21(std::uintptr_t(hs)+prop,base+0x3B0A6A0);PutH21(std::uintptr_t(hs)+prop+8,static_cast<unsigned char>(prop==0x4B0));}
  hostInts[{hs,kActionPhaseProp39}]=1;hostFloats[{hs,0x450}]=-1.f;
  mo::Key key=a;key.gesture=hostNow;
  const auto before=hostPhasePublications;event(key,mo::Event::Prepare,1,2,0,hostNow);
  assert(vs::g_last_offer19c4.offerObserved&&vs::g_last_offer19c4.active==int(eligible));
  assert(vs::g_stage==4&&I(hs,kActionPhaseProp39)==4&&hostPhasePublications==before+1&&!vs::g_telegraph_offer.open);
  assert(kcd2vr::defense_intent19c4::Bool(HostDefenseIntentReadH21,base,std::uintptr_t(hs),0x5A0)==0);
  const auto deadline=vs::g_deadline,onset=vs::g_prediction19c3.onset,predictionDeadline=vs::g_prediction19c3.deadline;
  const auto effort=hostEffortCalls;hostNow+=12;vs::Tick(hs,hostNow);
  assert(vs::g_stage==4&&hostPhasePublications==before+1&&!vs::g_telegraph_offer.open);
  assert(vs::g_deadline==deadline&&vs::g_prediction19c3.onset==onset&&vs::g_prediction19c3.deadline==predictionDeadline&&hostEffortCalls==effort);
  hostNow+=18;
  if(contact){mo::Attempt claim;assert(mo::attempts.contact(key,hostNow,hostNow,55,0,claim)==mo::Claim::Ready);
   vs::ContactScope5912 scope;assert(scope.begin(hs,claim)&&I(hs,kActionPhaseProp39)==8&&scope.current());scope.afterHit();}
  else event(key,mo::Event::Commit,2,2,0,hostNow);
  assert(vs::g_stage==8&&hostPhasePublications==before+2&&vs::g_deadline==deadline&&vs::g_prediction19c3.onset==onset&&vs::g_prediction19c3.deadline==predictionDeadline);
  std::printf("H21_ACTUAL_TICK nativeActive=%d legacy=%d contact=%d prepare=4 idleTick=4 realStrike=8 originalDeadline=UNCHANGED\n",eligible,legacy,contact);
  vs::Forget();hostGraphH21=false;vs::g_defense_intent_hooks19c4=false;
 }
 std::puts("State/context/ledger regression tests passed; native operations are host substitutes.");
}
