#include "../src/subsystems/hybrid/contact_policy.h"
#include <atomic>
#include <cstdio>
#include <cassert>
using namespace kcd2vr;
static std::atomic<unsigned> g_drop_nonordinary_phase56{0};
static constexpr unsigned kActionPhaseProp39=0x30;
static int hostPhase=1;static bool owned=false;static unsigned ownerReads=0;
static int I(const void*,unsigned){return hostPhase;}
static bool Allow(int,int){return false;}
namespace diagnostic_trace {static void Write(const char*){}}
namespace frik_pose_probe {static bool HybridQuietGuardOwnedH20(const void*,const void*){++ownerReads;return owned;}}
struct Pending {std::uint32_t serial=1,entityId=55;std::uint64_t tick=1000;};
static unsigned total=0;
static void check(bool ok){++total;assert(ok);}
int main(){
 namespace mo=kcd2vr::motion59c4;namespace se=kcd2vr::swing_effort59c14;
 const void* hs=reinterpret_cast<void*>(1);const void* henryActor=reinterpret_cast<void*>(2);
 const mo::Key key{90,10,11,12,13,14,15};Pending p{};const bool noRelease54=true;
 contact176::recoveryEpoch55c1=14;
 struct Lease{unsigned defers=0;void defer(const Pending&){++defers;}}pendingLease;
 unsigned nativeEntries=0;std::uint64_t now=1078;
 const auto submit=[&](){
  mo::Attempt attempt{};auto result=mo::hybridAttempts54.claim(key,p.tick,now,p.entityId,attempt);
  if(result==mo::Claim::Wait)result=mo::hybridAttempts54.contact(key,p.tick,now,p.entityId,0,attempt);
  if(result!=mo::Claim::Ready)return;
  mo::ClaimReservationC2 reservation;reservation.hold(mo::hybridAttempts54,key);
#include "generated_hybrid_quiet_guard_h20.inc"
  reservation.nativeCallStarted();++nativeEntries; // counted native leaf; no damage is emulated
 };
 const auto reset=[&](){mo::hybridAttempts54.clear();se::ledger.clear();g_drop_nonordinary_phase56=0;ownerReads=0;nativeEntries=0;pendingLease.defers=0;now=1078;
  check(mo::hybridAttempts54.event(key,mo::Event::Prepare,990,990,2,0));};
 const auto paid=[&](){check(se::ledger.publish(key,.8,1.6,false,1000));check(se::ledger.nativeComplete54(key,1001));};
 for(int phase:{128,256}){
  reset();hostPhase=phase;owned=true;paid();submit();submit();
  check(nativeEntries==1&&ownerReads==1); // second same physical gesture cannot adjudicate twice
  reset();hostPhase=phase;owned=false;paid();submit();check(nativeEntries==0&&g_drop_nonordinary_phase56==1);
  reset();hostPhase=phase;owned=true;submit();check(nativeEntries==0); // ownership is not native stamina proof
  check(se::ledger.publish(key,.8,1.6,false,1000));se::ledger.nativeObserved54(key,1000);submit();
  check(nativeEntries==0&&pendingLease.defers==1); // unfinished native work defers, no free hit
  check(se::ledger.nativeComplete54(key,1080));now=1081;submit();check(nativeEntries==1);
  reset();hostPhase=phase;owned=true;paid();now=1161;submit();check(nativeEntries==0); // old contact not renewed by guard witness
  reset();hostPhase=phase;owned=true;paid();contact176::recoveryEpoch55c1=15;submit();check(nativeEntries==0);contact176::recoveryEpoch55c1=14;
 }
 for(int phase:{64,512,1024}){reset();hostPhase=phase;owned=true;paid();submit();check(nativeEntries==0&&ownerReads==0);}
 for(int phase:{0,1,2,4,8,16,32}){reset();hostPhase=phase;owned=false;paid();submit();check(nativeEntries==1&&ownerReads==0);}
 std::printf("hybrid_quiet_guard_h20: %u checks PASS; actual production phase/cost block, real ledgers, explicit native owner/effect leaves\n",total);
}
