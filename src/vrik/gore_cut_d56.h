#pragma once
// Cosmetic severing only. Native hit recognition, damage and defence do not use this gate.
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace kcd2vr::gore_cut_d56 {
struct Evidence {
    std::uint64_t gesture=0,key=0,generation=0,epoch=0,request=0,sample=0,tick=0;
    double netTravel=0,fastTravel=0,transverseSpeed=0,totalSpeed=0,power=0;
    int kind=-1;bool committed=false;
};
inline double powerRatio(const Evidence& e,double mass,double reference) noexcept {
    if(!std::isfinite(e.transverseSpeed)||!std::isfinite(mass)||!std::isfinite(reference)||reference<=0)return 0;
    return .5*std::clamp(mass,.3,6.)*.55*e.transverseSpeed*e.transverseSpeed/std::max(1.,reference);
}
enum class Why {Ok,Identity,NotCut,NotCommitted,ShortStroke,Motion,Power};
inline const char* name(Why w) noexcept {
    constexpr const char* n[]{"FULL_TRANSVERSE_SWING","NO_EXACT_GESTURE","NOT_CUT","NOT_COMMITTED",
        "NOT_FULL_TRAVEL","NOT_TRANSVERSE_CONTACT","NOT_FULL_POWER"};return n[unsigned(w)];
}
inline Why qualify(const Evidence& e,double configuredPower) noexcept {
    if(!e.gesture||!e.key||!e.generation||!e.epoch||!e.request||!e.sample||!e.tick)return Why::Identity;
    if(e.kind!=0)return Why::NotCut;
    if(!e.committed)return Why::NotCommitted;
    // Deliberate full travel is independent of the permissive native contact-assisted attack.
    if(!std::isfinite(e.netTravel)||!std::isfinite(e.fastTravel)||e.netTravel<.22||e.fastTravel<.30)return Why::ShortStroke;
    if(!std::isfinite(e.transverseSpeed)||!std::isfinite(e.totalSpeed)||e.totalSpeed<2.5||
       e.transverseSpeed<2.5||e.transverseSpeed>e.totalSpeed+1e-6||e.transverseSpeed<.65*e.totalSpeed)return Why::Motion;
    const double need=std::isfinite(configuredPower)?std::max(.90,configuredPower):.90;
    if(!std::isfinite(e.power)||e.power<need)return Why::Power;
    return Why::Ok;
}
inline bool owned(const Evidence& e,std::uint64_t gesture,std::uint64_t key,std::uint64_t generation,
        std::uint64_t epoch,std::uint64_t request,std::uint64_t sample,std::uint64_t tick) noexcept {
    return e.gesture==gesture&&e.key==key&&e.generation==generation&&e.epoch==epoch&&e.request==request&&
        e.sample==sample&&e.tick==tick;
}
struct Claim {std::uint32_t target=0;std::uintptr_t identity=0;std::uint64_t gesture=0,key=0,generation=0,epoch=0,request=0;};
inline bool same(const Claim& a,const Claim& b) noexcept {
    return a.target&&a.identity&&a.gesture&&a.target==b.target&&a.identity==b.identity&&a.gesture==b.gesture&&
        a.key==b.key&&a.generation==b.generation&&a.epoch==b.epoch&&a.request==b.request;
}
template<unsigned N>struct Ledger {
    Claim entries[N]{};unsigned next=0;
    bool contains(const Claim& c) const noexcept {for(const auto& x:entries)if(same(x,c))return true;return false;}
    bool take(const Claim& c) noexcept {if(!c.target||!c.identity||!c.gesture||contains(c))return false;entries[next++%N]=c;return true;}
};
}
