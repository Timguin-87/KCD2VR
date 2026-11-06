#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
namespace kcd2vr::diagnostics::flowH12 {
// Event-only copied observations. No native reader, writer, timer or success award.
// These counters have the SAME coverage with the F3 recorder on and off.
struct Audit {
    std::atomic<std::uint64_t> admission{0},nativeTrue{0},unknown{0},slotAtInput{0},masterAtInput{0};
    std::atomic<std::uint64_t> prepare{0},commit{0},cut{0},thrust{0},positive{0},zero{0},windowUnknown{0};
    std::atomic<std::uint64_t> learned{0},unlearned{0},perkUnknown{0};
    std::atomic<std::uint64_t> msSamples{0},msUnknown{0},msPositive{0},msMax{0},msObservedOpens{0};
    std::array<std::atomic<std::uint64_t>,8> eligibilityReasons{};
    void input(bool known,int ms,bool slot,bool returned) noexcept {
        ++admission;nativeTrue+=returned; // D4AD94 Boolean only, not an accepted hit/recipe/dispatch receipt
        if(!known){++unknown;return;}
        slotAtInput+=slot;masterAtInput+=ms>0;
    }
    void attempt(bool prep,int stroke,int window,int perk,unsigned reason) noexcept {
        prep?++prepare:++commit;
        cut+=stroke==1;thrust+=stroke==2;
        window>0?++positive:window==0?++zero:++windowUnknown;
        perk==1?++learned:perk==0?++unlearned:++perkUnknown;
        if(reason<eligibilityReasons.size())++eligibilityReasons[reason];
    }
    void window(bool known,int count,bool observedOpen) noexcept {
        ++msSamples;
        if(!known){++msUnknown;return;}
        if(count>0){++msPositive;auto maximum=msMax.load(std::memory_order_relaxed);
            while(std::uint64_t(count)>maximum&&!msMax.compare_exchange_weak(maximum,std::uint64_t(count),std::memory_order_relaxed)) {}}
        msObservedOpens+=observedOpen; // exact existing observer edge, not a new counter opportunity
    }
};
inline Audit audit;
inline void format(const Audit& a,char* out,std::size_t n) noexcept {
    if(!out||!n)return;
    using U=unsigned long long;
    std::snprintf(out,n,"ownedWeaponInputs{calls=%llu nativeTrue=%llu unknown=%llu slotAtInput=%llu msAtInput=%llu} "
        "msEligibility{prepare=%llu commit=%llu cut=%llu thrust=%llu windowPos=%llu zero=%llu unknown=%llu perkYes=%llu no=%llu unknown=%llu reasonsQOffNoPerkNotThrustEnergyUnknownSideNotOpposite=%llu/%llu/%llu/%llu/%llu/%llu/%llu windowQualified=%llu} "
        "weaponMsSamples{calls=%llu unknown=%llu positive=%llu max=%llu observedOpens=%llu} detailScope=F3_ONLY",
        U(a.admission.load()),U(a.nativeTrue.load()),U(a.unknown.load()),U(a.slotAtInput.load()),U(a.masterAtInput.load()),
        U(a.prepare.load()),U(a.commit.load()),U(a.cut.load()),U(a.thrust.load()),U(a.positive.load()),U(a.zero.load()),U(a.windowUnknown.load()),
        U(a.learned.load()),U(a.unlearned.load()),U(a.perkUnknown.load()),
        U(a.eligibilityReasons[0].load()),U(a.eligibilityReasons[1].load()),U(a.eligibilityReasons[2].load()),U(a.eligibilityReasons[3].load()),
        U(a.eligibilityReasons[4].load()),U(a.eligibilityReasons[5].load()),U(a.eligibilityReasons[6].load()),U(a.eligibilityReasons[7].load()),
        U(a.msSamples.load()),U(a.msUnknown.load()),U(a.msPositive.load()),U(a.msMax.load()),U(a.msObservedOpens.load()));
}
}
