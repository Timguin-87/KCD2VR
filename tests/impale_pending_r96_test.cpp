// R96: exercise the production physical kernel's capture/recovery/rejoin path.
// The native lethal result and sweep response are fixture inputs; no game claim.
#define main ImpaleLegacyMainR96
#include "impale47_test.cpp"
#undef main
#include <cstring>
#include <limits>

namespace {
unsigned assertionsR96=0;
void requireR96(bool ok,const char* label) {
    ++assertionsR96;
    if(!ok){std::printf("FAIL_IMPALE_PENDING_R96 %s\n",label);std::exit(1);}
}
void admitR96(Replay& r,std::uint64_t serial) {
    r.move({});r.in.receipt58c1=r.receipt(serial,r.tip());r.move({});
    requireR96(r.out.impaleLatchRequest&&r.state.weapon[0].impale178r15,
        "actual kernel admits completed native lethal tip receipt");
    r.in.receipt58c1={}; // A subsequent frame cannot admit it again.
    r.in.sweepGate=false; // Ask the supplied sweep on every recovery frame.
}
void atAgeR96(Replay& r,std::uint64_t since,std::uint64_t age) {
    const auto target=since+age;
    requireR96(r.in.tick<=target,"test clock is monotonic");
    while(r.in.tick<target)r.move({},double(std::min<std::uint64_t>(11,target-r.in.tick))/1000.);
    requireR96(r.in.tick==target,"exact millisecond boundary reached");
}
void sameRequestR96(const Replay& r,const kcd2vr::contact58c1::Receipt& hit,std::uint64_t since) {
    requireR96(r.out.impaleLatchRequest&&r.out.impaleSerial==hit.serial,
        "late recovery retains original request after1700ms");
    requireR96(r.state.weapon[0].impaleSince178r15==since&&
        !std::memcmp(&r.out.impaleHit47,&hit,sizeof(hit)),
        "recovery neither renews admission time nor replaces immutable native receipt");
    requireR96(!r.out.impaleActive&&!r.out.displayLatched[0]&&!r.state.weapon[0].impalePinned178r18,
        "pending evidence grants no pin or display latch");
}
void unknownR96(Replay& r) {
    r.contactKnown59=false;r.in.contactSuspended59c1=true;r.move({},.001);
    requireR96(r.out.handlingOnly55c1[0]&&r.state.weapon[0].acquiring124&&
        !r.out.contactKnown[0],"actual unknown-contact handling recovery entered");
}
void rejoinR96(Replay& r) {
    r.contactKnown59=true;r.in.contactSuspended59c1=false;r.move({},.001);
    requireR96((r.out.resetH29[0].reason&8u)!=0&&r.out.contactKnown[0]&&
        !r.out.handlingOnly55c1[0]&&!r.state.weapon[0].acquiring124,
        "actual acquiring reset clears and restores state on checked rejoin");
}
void kernelLifetimeR96() {
    Replay r(true);admitR96(r,9601);
    const auto hit=r.state.weapon[0].impaleHit47;
    const auto since=r.state.weapon[0].impaleSince178r15;
    for(const auto age:{1701u,2000u,4500u,4699u}) {
        atAgeR96(r,since,age-1);unknownR96(r);sameRequestR96(r,hit,since);
        rejoinR96(r);sameRequestR96(r,hit,since);
    }
    requireR96(r.in.tick==since+kcd2vr::impale47::kWeaponWaitH18,
        "checked rejoin accepted at original4700ms boundary");
    r.move({},.001);
    requireR96(!r.out.impaleLatchRequest&&!r.state.weapon[0].impale178r15,
        "request expires at original4701ms despite repeated recovery");
    unknownR96(r);rejoinR96(r);
    requireR96(!r.out.impaleLatchRequest&&!r.out.impaleActive,
        "expired request cannot resurrect on later recovery");
}
void kernelRevocationsR96() {
    for(unsigned why=0;why<13;++why) {
        Replay r(true);admitR96(r,9620+why);
        const auto since=r.state.weapon[0].impaleSince178r15;
        atAgeR96(r,since,1999);unknownR96(r);
        requireR96(r.out.impaleLatchRequest,"late pending is live before exact revocation");
        r.contactKnown59=true;r.in.contactSuspended59c1=false;
        switch(why) {
        case 0:r.in.grip[0]=0;break;
        case 1:r.in.tracked[0]=false;break;
        case 2:r.in.intentTracked[0]=false;break;
        case 3:r.in.allowed=false;break;
        case 4:++r.in.epoch;break;
        case 5:++r.in.bodyEpoch;break;
        case 6:++r.profiles[0].key;break;
        case 7:++r.profiles[0].generation;break;
        case 8:r.in.impaleEnabled178r15=false;break;
        case 9:r.in.impaleHang32=false;break;
        case 10:r.profiles[0].primary=false;break;
        case 11:r.in.haveWorld=false;break;
        case 12:break; // A real81ms input gap is not covered by the request lifetime.
        }
        r.move({},why==12?.081:.001);
        requireR96(!r.out.impaleLatchRequest&&!r.out.impaleActive,
            "actual rejoin rejects owner context tracking grip option or freshness loss");
    }
}
void helperBoundariesR96() {
    Replay r(true);admitR96(r,9660);
    const auto original=r.state.weapon[0];const auto since=original.impaleSince178r15;
    for(const auto age:{1700u,1701u,4500u,4699u,4700u,4701u}) {
        auto s=original;auto in=r.in;in.tick=since+age;s.lastTick=in.tick-1;
        const auto pending=kcd2vr::impale_pending59::Pending::capture(s,in,r.profiles[0],0);
        requireR96(pending.valid==(age<=kcd2vr::impale47::kWeaponWaitH18),
            "actual Pending helper shares inclusive original H18 deadline");
        if(pending.valid) {
            WeaponState restored{};pending.restore(restored);Output out{};pending.publish(out,0);
            requireR96(restored.impaleSince178r15==since&&pending.since==since&&
                !std::memcmp(&out.impaleHit47,&original.impaleHit47,sizeof(original.impaleHit47)),
                "actual helper copies exact evidence and original admission time");
        }
    }
    for(unsigned why=0;why<15;++why) {
        auto s=original;auto in=r.in;in.tick=since+2000;s.lastTick=in.tick-1;
        switch(why) {
        case 0:s.lastTick=in.tick-81;break;
        case 1:s.lastTick=in.tick+1;break;
        case 2:s.impaleSince178r15=in.tick+1;break;
        case 3:s.impalePinned178r18=true;break;
        case 4:in.impalePinValid178r15=true;in.impalePinSerial178r19=s.impaleSerial178r15;break;
        case 5:++s.lastImpaleReceipt58c1;break;
        case 6:++s.impaleHit47.serial;break;
        case 7:++s.impaleHit47.target;break;
        case 8:s.impaleHit47.tick=since-81;break;
        case 9:s.impaleHit47.tick=since+1;break;
        case 10:s.impaleHit47.feature=int(kcd2vr::bodycombo59c7g::Feature::Blade);break;
        case 11:s.impaleHit47.death=0;break;
        case 12:s.impaleTipWorld178r18.x=std::numeric_limits<double>::quiet_NaN();break;
        case 13:s.impaleLocal32.x=std::numeric_limits<double>::quiet_NaN();break;
        case 14:in.grip[0]=std::numeric_limits<double>::quiet_NaN();break;
        }
        requireR96(!kcd2vr::impale_pending59::Pending::capture(s,in,r.profiles[0],0).valid,
            "request lifetime does not relax current or original receipt proof");
    }
    auto s=original;auto in=r.in;in.tick=since+2000;s.lastTick=in.tick-80;
    requireR96(kcd2vr::impale_pending59::Pending::capture(s,in,r.profiles[0],0).valid,
        "original current-input freshness boundary remains80ms");
}
}
int main() {
    kernelLifetimeR96();kernelRevocationsR96();helperBoundariesR96();
    std::printf("PASS_IMPALE_PENDING_R96 assertions=%u\n",assertionsR96);
    return 0;
}
