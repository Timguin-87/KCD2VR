#include "vrik/live_attributes5912.h"
#include "vrik/weapon_dynamics177.h"
#include "vrik/motion_owned59c4.h"
#include "diagnostics/capture168.h"
#include <cassert>
#include <cstdio>
#include <limits>
int main(){
 namespace ls=kcd2vr::live5912;namespace mo=kcd2vr::motion59c4;
 ls::Snapshot s{};s.player=1;s.entity=2;s.soul=3;s.tick=1000;s.recovery=4;
 s.valid=true;s.strength=21;s.agility=19;s.current=20;s.maximum=100;s.stamina=.2;
 assert(ls::usable(s,1,2,1050,4));assert(!ls::usable(s,1,2,1101,4));
 assert(!ls::usable(s,2,2,1050,4));assert(!ls::usable(s,1,3,1050,4));assert(!ls::usable(s,1,2,1050,5));
 auto invalid=s;invalid.stamina=std::numeric_limits<double>::quiet_NaN();assert(!ls::usable(invalid,1,2,1050,4));
 ls::latest.publish(s);ls::Snapshot read{};assert(ls::latest.read(read)&&read.stamina==.2);
 ls::latest.publish({});assert(ls::latest.read(read)&&!read.valid);
 const auto full=kcd2vr::dynamics177::authority(21,19,1,true,false);
 const auto tired=kcd2vr::dynamics177::authority(21,19,.2,true,false);
 const auto both=kcd2vr::dynamics177::authority(21,19,1,true,true);
 assert(tired.force<full.force&&tired.torque<full.torque);
 assert(both.force>full.force&&both.torque>full.torque);
 mo::Attempts bank;mo::Key k{1,2,3,4,5,6,7};
 assert(bank.event(k,mo::Event::Prepare,1000,1000,2,0));
 assert(bank.claimEffort(k,1000)==mo::EffortClaim::Invalid); // held windup is not a full swing
 assert(bank.event(k,mo::Event::Commit,1010,1010,2,0));
 assert(bank.claimEffort(k,1010)==mo::EffortClaim::Ready);
 assert(bank.claimEffort(k,1010)==mo::EffortClaim::Pending);
 assert(bank.finishEffort(k,true));
 mo::Attempt a{};assert(bank.claim(k,1020,1030,77,a)==mo::Claim::Ready);
 assert(bank.claimEffort(k,1030)==mo::EffortClaim::Duplicate); // contact cannot charge twice
 mo::Key miss=k;miss.gesture=2;assert(bank.event(miss,mo::Event::Commit,1040,1040,5,1));
 assert(bank.claimEffort(miss,1040)==mo::EffortClaim::Ready);
 assert(bank.claimEffort(miss,1040)==mo::EffortClaim::Pending);
 assert(bank.finishEffort(miss,true)); // miss costs at commit
 assert(bank.event(miss,mo::Event::Cancel,1045,1045,-1,1));
 assert(bank.claimEffort(miss,1045)==mo::EffortClaim::Duplicate);
 mo::Key early=k;early.gesture=3;assert(bank.event(early,mo::Event::Prepare,1050,1050,2,0));
 assert(bank.contact(early,1060,1061,77,0,a)==mo::Claim::Ready);
 assert(bank.claimEffort(early,1061)==mo::EffortClaim::Ready);
 assert(bank.claimEffort(early,1061)==mo::EffortClaim::Pending);
 assert(bank.finishEffort(early,true));
 assert(bank.claimEffort(early,1062)==mo::EffortClaim::Duplicate);
 bank.clear();assert(bank.claimEffort(early,1063)==mo::EffortClaim::Invalid);
 // R59.16C7E-DECAP1: per-swing stamina lines need the F3 recorder; the exhaustion edge stays essential.
 assert(!kcd2vr::diagnostics::capture168::accept("R59.16C1 SWING_STAMINA attempt=1",1063));
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,1063);
 assert(kcd2vr::diagnostics::capture168::accept("R59.16C1 SWING_STAMINA attempt=1",1064));
 assert(kcd2vr::diagnostics::capture168::essential("R59.16C1 SWING_STAMINA_EXHAUSTION ON stamina=0.010"));
 std::puts("Live snapshot, owner/age revocation, fatigue authority, and once-only native-effort policy passed.");
}
