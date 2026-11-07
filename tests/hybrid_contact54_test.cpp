#include "../src/subsystems/hybrid/contact_policy.h"
#include "../src/vrik/motion_owned59c4.h"
#include "../src/vrik/swing_effort59c14.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
int main(){
 namespace h=hybrid54;namespace c=contact59c3;namespace m=motion59c4;
 c::Key joinKey{90,10,11,12,13,14};contact176::Ticket none{};
 assert(c::join(joinKey,none,1000,1078,14)==c::Join::Wait);
 assert(h::rendezvous(c::join(joinKey,none,1000,1078,14),true,true,1000,1078,70)==h::Rendezvous::Physical);
 assert(h::rendezvous(c::Join::Wait,true,true,1000,1069,70)==h::Rendezvous::Wait);
 assert(h::rendezvous(c::Join::Wait,true,true,1000,1160,70)==h::Rendezvous::Physical);
 assert(h::rendezvous(c::Join::Expired,true,true,1000,1161,70)==h::Rendezvous::Reject);
 assert(h::rendezvous(c::Join::Wait,true,true,1000,1161,70)==h::Rendezvous::Reject);
 assert(h::rendezvous(c::Join::Recovery,true,true,1000,1078,70)==h::Rendezvous::Reject);
 assert(h::rendezvous(c::Join::Identity,true,true,1000,1078,70)==h::Rendezvous::Reject);
 assert(h::rendezvous(c::Join::Wait,false,true,1000,1078,70)==h::Rendezvous::Wait);
 assert(h::rendezvous(c::Join::Wait,true,false,1000,1078,70)==h::Rendezvous::Wait);
 m::Attempts ledger; m::Key key{90,10,11,12,13,14,0x123};m::Attempt out{};
 // A missing native release does not mint a physical attempt.
 assert(ledger.contact(key,1000,1078,555,0,out)==m::Claim::Identity);
 assert(ledger.event(key,m::Event::Prepare,990,990,2,0));
 // Qualified early physical contact independently completes the preparation.
 assert(ledger.contact(key,1000,1078,555,0,out)==m::Claim::Ready);
 {m::ClaimReservationC2 rejectedPreflight;rejectedPreflight.hold(ledger,key);}
 assert(ledger.claim(key,1000,1080,555,out)==m::Claim::Ready);
 {m::ClaimReservationC2 entered;entered.hold(ledger,key);entered.nativeCallStarted();}
 assert(ledger.claim(key,1001,1081,666,out)==m::Claim::Duplicate);
 assert(ledger.application54(key,1120,out)==m::Claim::Duplicate); // late native release cannot hit twice
 // A native answer can reserve before physical commitment. A later native
 // preflight refusal must restore that real Prepared stage, not commit0.
 auto refused=key;refused.gesture=91;
 assert(ledger.event(refused,m::Event::Prepare,1130,1130,2,0));
 assert(ledger.application54(refused,1140,out)==m::Claim::Ready);
 {m::ClaimReservationC2 preflight;preflight.hold(ledger,refused);}
 assert(ledger.contact(refused,1150,1160,555,0,out)==m::Claim::Ready);
 {m::ClaimReservationC2 native;native.hold(ledger,refused);native.nativeCallStarted();}
 // Reverse scheduling: native application first also closes physical fallback.
 key.gesture=92;assert(ledger.event(key,m::Event::Prepare,1200,1200,1,1));
 assert(ledger.application54(key,1240,out)==m::Claim::Ready);
 {m::ClaimReservationC2 entered;entered.hold(ledger,key);entered.nativeCallStarted();}
 assert(ledger.contact(key,1230,1250,555,1,out)==m::Claim::Duplicate);
 // Hard context cancellation never revives under the old late-contact concession.
 ++key.gesture;assert(ledger.event(key,m::Event::Prepare,1300,1300,1,0));
 assert(ledger.event(key,m::Event::Cancel,1310,1310,1,0,false));
 assert(ledger.contact(key,1310,1320,555,0,out)==m::Claim::Cancelled);
 ++key.recovery;assert(ledger.application54(key,1320,out)==m::Claim::Identity);
 assert(!h::ordinaryPhase(64)&&!h::ordinaryPhase(128)&&!h::ordinaryPhase(256));
 assert(h::ordinaryPhase(1)&&h::ordinaryPhase(4)&&h::ordinaryPhase(8));
 // A terminal answer must survive rapid newer preparations for the full
 // native release cap, including a long held preparation before application.
 m::Attempts tombstones(1500);m::Key oldest{100,1,1,1,1,1,1};
 assert(tombstones.event(oldest,m::Event::Prepare,2000,2000,1,0));
 assert(tombstones.application54(oldest,4900,out)==m::Claim::Ready);
 for(unsigned n=1;n<=7;++n){auto next=oldest;next.gesture+=n;
  assert(tombstones.event(next,m::Event::Prepare,5000,5000,1,0));}
 auto overflow=oldest;overflow.gesture+=8;
 assert(!tombstones.event(overflow,m::Event::Prepare,5300,5300,1,0));
 assert(tombstones.application54(oldest,5300,out)==m::Claim::Duplicate);
 // Missing-release routing cannot silently grant a free strike. The proof
 // is keyed native completion, not a quote, elapsed time or latest cost value.
 swing_effort59c14::Ledger effort;
 assert(effort.publish(oldest,.8,1.6,false,5300));
 assert(!effort.paidFor54(oldest,5301));
 swing_effort59c14::Record cost{};
 assert(effort.reserve(oldest,5301,cost)==swing_effort59c14::Claim::Ready);
 assert(!effort.paidFor54(oldest,5301));
 assert(effort.finish(oldest,swing_effort59c14::Status::Complete));
 assert(effort.paidFor54(oldest,5302));
 auto foreign=oldest;++foreign.weapon;assert(!effort.paidFor54(foreign,5302));
 assert(!effort.paidFor54(oldest,6801));
 // Native ancestry credit never charges again and cannot turn factory-only,
 // partial, duplicate, wrong-site or failed native action work into payment.
 namespace se=swing_effort59c14;
 se::ledger.clear();auto nativeKey=oldest;nativeKey.gesture=200;
 const auto input=[&](std::uint64_t tick){se::NativeCost54 c{};c.key=nativeKey;c.actor=2;c.state=3;c.action=4;c.helper=5;c.row=6;c.type=1;c.hand=0;
  se::actionRegistry54.construct(4,2,true,tick);se::Input54 i{nativeKey,2,3,tick};se::Action54 a{};m::Key credit{};
  assert(se::actionRegistry54.bind(i,4,5,6,tick,a));assert(!se::actionRegistry54.dispatchFinish(a,true,tick,credit));
  assert(se::actionRegistry54.start(4,tick,c.causal54));return c;};
 assert(se::ledger.publish(nativeKey,.8,1.6,false,7000));
 {se::NativeCostScope54 s(input(7000));s.value.wuid=3;s.value.raised=1;
  assert(!s.finish(true,7001));assert(!se::ledger.paidFor54(nativeKey,7001));
  s.value.struck=1;assert(!s.finish(true,7001));} // a terminal partial start never reopens
 ++nativeKey.gesture;assert(se::ledger.publish(nativeKey,.8,1.6,false,7050));
 {se::NativeCostScope54 s(input(7050));s.value.wuid=3;s.value.raised=s.value.struck=1;assert(s.finish(true,7051));}
 assert(se::ledger.paidFor54(nativeKey,7052));
 ++nativeKey.gesture;assert(se::ledger.publish(nativeKey,.8,1.6,false,7100));
 {se::NativeCostScope54 s(input(7100));s.value.wuid=3;
  se::NativeWrapper54 w{&s.value,1,0,3,true};se::nativeWrapper54=&w;
  assert(!se::nativeEffect54(3,0xC64DCA,2,-10,true)); // wrong wrapper/site cannot credit
  assert(se::nativeEffect54(3,0xC5BBEB,1,-3,true));
  assert(!se::nativeEffect54(3,0xC5BBEB,1,-3,true)); // second effect poisons proof
  s.value.raised=s.value.struck=1;assert(!s.finish(true,7101));se::nativeWrapper54=nullptr;}
 assert(!se::ledger.paidFor54(nativeKey,7101));
 ++nativeKey.gesture;assert(se::ledger.publish(nativeKey,.8,1.6,false,7200));
 {auto prepared=input(7200);se::Record r{};assert(se::ledger.reserve(nativeKey,7201,r)==se::Claim::Ready);
  se::NativeCostScope54 s(prepared);s.value.wuid=3;s.value.raised=s.value.struck=1;
  assert(!s.finish(true,7201)); // an in-flight own debit cannot also receive native credit
  assert(se::ledger.finish(nativeKey,se::Status::Indeterminate));assert(!s.finish(true,7201));}
 // Native preparation may precede immutable physical energy publication. It
 // must quarantine the later same-key DIRECT debit without inventing energy.
 se::Ledger beforeEnergy;auto early=nativeKey;++early.gesture;se::Record r{};
 beforeEnergy.nativeObserved54(early,8000);assert(beforeEnergy.reserve(early,8000,r)==se::Claim::Missing);
 assert(beforeEnergy.publish(early,.72,2.2,true,8010));
 assert(beforeEnergy.reserve(early,8011,r)==se::Claim::Pending&&!beforeEnergy.paidFor54(early,8011));
 assert(std::abs(r.mass-2.2)<1e-9&&r.support); // only the real energy publication provides these values
 beforeEnergy.nativeUncertain54(early,8012);
 assert(beforeEnergy.reserve(early,8012,r)==se::Claim::Indeterminate&&!beforeEnergy.nativeComplete54(early,8012));
 auto completeEarly=early;++completeEarly.gesture;beforeEnergy.nativeObserved54(completeEarly,8100);
 assert(beforeEnergy.nativeComplete54(completeEarly,8101)&&!beforeEnergy.paidFor54(completeEarly,8101));
 assert(beforeEnergy.publish(completeEarly,.9,1.7,false,8110)&&beforeEnergy.paidFor54(completeEarly,8110));
 auto uncertainEarly=early;uncertainEarly.gesture+=2;beforeEnergy.nativeUncertain54(uncertainEarly,8200);
 assert(beforeEnergy.publish(uncertainEarly,.9,1.7,false,8210));assert(beforeEnergy.reserve(uncertainEarly,8210,r)==se::Claim::Indeterminate);
 // Marker pressure also fails closed rather than losing an already-entered native cost.
 se::Ledger pressure;for(unsigned n=0;n<33;++n){auto k=early;k.gesture+=100+n;pressure.nativeObserved54(k,8300);}
 auto unknown=early;unknown.gesture+=200;assert(pressure.publish(unknown,.9,1.7,false,8310));
 assert(pressure.reserve(unknown,8311,r)==se::Claim::Indeterminate);
 se::Ledger delayed;auto late=early;late.gesture+=300;
 assert(delayed.nativeComplete54(late,1000));assert(delayed.publish(late,.9,1.7,false,5000));
 assert(!delayed.paidFor54(late,5000)&&delayed.reserve(late,5000,r)==se::Claim::Indeterminate);
 ++late.gesture;assert(delayed.nativeComplete54(late,6000));assert(delayed.publish(late,.9,1.7,false,5900));
 assert(delayed.paidFor54(late,6001)); // completed pose timestamp precedes callback by existing100ms frame lease
 ++late.gesture;assert(delayed.nativeComplete54(late,7000));assert(delayed.publish(late,.9,1.7,false,6899));
 assert(!delayed.paidFor54(late,7001));
 for(std::uint64_t publication:{2501ull,4000ull}){se::Ledger withinAncestry;auto k=late;k.gesture+=publication;
  assert(withinAncestry.nativeComplete54(k,1000));assert(withinAncestry.publish(k,.9,1.7,false,publication));
  assert(!withinAncestry.paidFor54(k,publication));} // 3500ms ancestry is not a refreshed1500ms cost entitlement
 std::puts("hybrid_contact54: missing-release rendezvous, exact attempt authority, both orderings, preflight release, native-entry terminal PASS");
}
