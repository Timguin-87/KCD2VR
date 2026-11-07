#include "vrik/combo_lineage19.h"
#include "vrik/contact_handoff59c3.h"
#include "vrik/contact_metadata59c7g.h"
#include "diagnostics/contact_lease47c2.h"
#include "core/contact_backlog59c6.h"
#include "subsystems/hybrid/contact_policy.h"
#include "vrik/recovery_retire65h.h"
#include "diagnostics/capture168.h"
#include <cstdio>
#include <cassert>
#include <cstring>
static std::uint64_t now=1000;inline std::uint64_t GetTickCount64(){return now;}
namespace handoff59c3=kcd2vr::contact59c3;
namespace diagnostics=kcd2vr::diagnostics;
namespace victim5916c3{void Clear(){}}namespace vrstate596{void Reset(){}}
std::atomic<std::uint64_t> g_stats[8]{},g_coalesced59c3{0},g_henry_seen178r31{0},g_native_owned_until178r33{0};
std::atomic<int> g_henry_in_combat178r31{-1},g_tg_phase178r34{0};
bool Allow(int,unsigned){return false;}namespace diagnostic_trace{void Write(const char*){}}
// Explicit capture-off diagnostic leaf; connected capture-on retirement tested in contact_queue59c6.
template<class P> void RecoveryRetired65h(const P& p,std::uint64_t epoch,std::uint64_t tick,kcd2vr::recovery65h::Site site){kcd2vr::recovery65h::retired(p,epoch,tick,site,false);}
#include "hybrid_order59.inc"
int main(){
 using namespace kcd2vr;namespace c=contact176;namespace mo=motion59c4;
 c::recoveryEpoch55c1=1;c::policy=3;c::claimedSerial=0;Clear();
 Pending a{};a.gesture59c3=10;a.key=20;a.generation=1;a.epoch=2;a.request=3;a.sample=100;a.tick=1000;a.entityId=50;
 Pending b=a;b.gesture59c3=11;b.sample=101;b.tick=1030;b.entityId=51;
 mo::Key ka{10,20,1,2,3,1,0x1234},kb=ka;kb.gesture=11;
 assert(mo::hybridAttempts54.event(ka,mo::Event::Prepare,950,1000,1,0));
 assert(mo::hybridAttempts54.event(ka,mo::Event::Commit,990,1000,1,0));
 assert(mo::hybridAttempts54.event(kb,mo::Event::Prepare,1030,1030,2,0));
 assert(mo::hybridAttempts54.event(kb,mo::Event::Commit,1060,1060,2,0));
 swing_effort59c14::Ledger cost;assert(cost.publish(ka,.9,1.5,false,1000));cost.nativeObserved54(ka,1000);
 now=1030;assert(Submit(a));assert(Submit(b));
 now=1070;DrainBooking59([&](Pending& p,auto& lease){assert(p.gesture59c3==10);
   assert(hybrid54::costProof54(cost,ka,p.tick,now,1)==hybrid54::CostProof54::Wait);lease.defer(p);});
 assert(g_pending.gesture59c3==11&&g_backlog59c6.count==1);
 // A's original released ticket appears while A waits in the tail. B follows.
 auto ticket=[&](const Pending& p,std::uint64_t serial,std::uint64_t tick){c::Ticket t{};
  t.player=0x1234;t.entity=0x5678;t.serial=serial;t.tick=tick;t.sample=p.sample;t.epoch=p.epoch;t.request=p.request;
  t.key=p.key;t.generation=p.generation;t.policy=serial+1;t.gesture59c3=p.gesture59c3;t.recovery59c3=1;return t;};
 const auto ta=ticket(a,1,990),tb=ticket(b,2,1060);
 assert(handoff59c3::releases.publish(ta));
 now=1075;assert(cost.nativeComplete54(ka,now)); // completed while A was still the native action
 assert(handoff59c3::releases.publish(tb));
 unsigned entered=0;now=1090;
 DrainBooking59([&](Pending& p,auto&){assert(p.gesture59c3==11);mo::Attempt witness{};
  assert(mo::hybridAttempts54.claim(kb,p.tick,now,p.entityId,witness)==mo::Claim::Ready);
  mo::ClaimReservationC2 claim;claim.hold(mo::hybridAttempts54,kb);
  kcd2vr::hybrid_serial59::Reservation serial;assert(serial.reserve(tb,now));assert(serial.tryEnter());claim.nativeCallStarted();++entered;});
 assert(g_pending.gesture59c3==10&&g_pending.tick==1000&&c::claimedSerial==2);
 now=1095;bool refused=false;
 DrainBooking59([&](Pending& p,auto&){
  handoff59c3::Key key{p.gesture59c3,p.key,p.generation,p.epoch,p.request,p.recovery55c1};c::Ticket found{};
  assert(handoff59c3::releases.find(key,found)&&found.serial==1);
  assert(handoff59c3::join(key,found,p.tick,now,1)==handoff59c3::Join::Bound);
  assert(hybrid54::costProof54(cost,ka,p.tick,now,1)==hybrid54::CostProof54::Ready);
  mo::Attempt witness{};assert(mo::hybridAttempts54.claim(ka,p.tick,now,p.entityId,witness)==mo::Claim::Ready);
  mo::ClaimReservationC2 claim;claim.hold(mo::hybridAttempts54,ka);
  kcd2vr::hybrid_serial59::Reservation serial;refused=!serial.reserve(found,now);assert(!refused);assert(serial.tryEnter());claim.nativeCallStarted();++entered;
 });
 assert(!refused&&!g_has&&entered==2);mo::Attempt witness{};
 assert(mo::hybridAttempts54.claim(ka,1000,1095,50,witness)==mo::Claim::Duplicate);
 // Actual queue lease + exact reservation: reentrant mode clear between
 // preflight and entry must prevent the call and leave the attempt rollbackable.
 now=1200;Clear();auto d=a;d.gesture59c3=12;d.tick=1200;d.sample=102;d.entityId=52;
 auto kd=ka;kd.gesture=12;assert(mo::hybridAttempts54.event(kd,mo::Event::Prepare,1190,1200,1,0));
 assert(mo::hybridAttempts54.event(kd,mo::Event::Commit,1195,1200,1,0));assert(Submit(d));
 DrainBooking59([&](Pending& p,auto&){mo::Attempt witness{};
  assert(mo::hybridAttempts54.claim(kd,p.tick,now,p.entityId,witness)==mo::Claim::Ready);
  mo::ClaimReservationC2 claim;claim.hold(mo::hybridAttempts54,kd);
  kcd2vr::hybrid_serial59::Reservation serial;auto td=ticket(d,3,1200);assert(serial.reserve(td,now));
  Clear();assert(!serial.tryEnter()); // no native call, no committed claim
 });assert(!g_has&&entered==2);
 std::puts("PASS: actual Submit/Drain queue defers fresh A behind B, then both exact releases enter once; no native game execution");
}
