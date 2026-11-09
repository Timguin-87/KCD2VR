// R146 (non-developer perf): diagnostic_trace::Write now asks capture168::gate.quiet(now) first.
// Compiles the REAL Write body (extracted by tools/run_quiet_write_r146.py) and proves, over random
// recorder start/stop/expiry/time sequences and receipt/ordinary rows, that it admits exactly the
// same rows as the R145 rule. Also: no latch (a recorder start ends quiet at once) and the
// COMBAT_FLOW/H12 rows that mod.cpp now skips when quiet are rows both of their writers refused.
#include "diagnostics/capture168.h"
#include <cstdio>
#include <cstdint>
#include <random>
#include <string>
#include <vector>
static std::uint64_t g_now=100000;
static std::uint64_t FakeTick() noexcept {return g_now;}
#define GetTickCount64 FakeTick
namespace kcd2vr::diagnostic_trace {
static bool Enabled() noexcept {return true;}
static int g_enqueued=0;
static void Enqueue(const char*) noexcept {++g_enqueued;}
#include "quiet_write_r146_production.inc"
}
#undef GetTickCount64
namespace cap=kcd2vr::diagnostics::capture168;
using D=cap::Domain;
static bool OldWriteR145(const char* m,std::uint64_t now) noexcept { // the R145 body, verbatim decision
    if(!m)return false;
    if(!cap::gate.mask.load(std::memory_order_relaxed)&&cap::receiptDomain(m)<0)return false;
    return cap::accept(m,now);
}
static bool OldEnabledForBug(std::uint64_t now) noexcept { // KCD2VR_DIAG_LOG(Bug,...) guard
    return (cap::gate.mask.load()&cap::members(D::Bug))&&cap::active(D::Bug,now);
}
static unsigned checks=0;
#define CK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(){
    const D domains[]{D::Bug,D::Combat,D::Interaction,D::Vrik,D::Reload,D::Physics,D::Hud,D::Hands,D::HandReference};
    std::mt19937_64 rng(146);
    std::uint64_t quietCalls=0,admitted=0,total=0;
    auto rows=[&](){
        std::vector<std::string> v{"R59.16C10R17 COMBAT_FLOW rung=1 guard{strict=1} hits{intentLive=3}",
            "H12 COMBAT_ELIGIBILITY rung=1 ownedWeaponInputs{calls=4}","V66.178 STRIKE serial=7 kind=1","V64.82-VRIK F9=1 READ-ONLY",
            "R133 FREE_TARGETING mode=1","C54R3 HIT_CHAIN x","x RELOAD_NATIVE_END y","F8_LIFECYCLE_END x"};
        for(auto d:domains){const auto s=cap::gate.session[unsigned(d)].load();
            for(std::uint64_t k:{s,s?s-1:0,s+1}){const auto n=std::to_string(k);
                v.push_back("D42_F2_END session="+n);v.push_back("D42_F3_END session="+n);v.push_back("D42_F5_END session="+n);
                v.push_back("D42_F7_END session="+n);v.push_back("F2_CAUSAL stage=4 session="+n);v.push_back("D48_F11 END session="+n);
                v.push_back("D42_COVERAGE domain=COMBAT session="+n);v.push_back("V66.168 CAPTURE domain=BUG session="+n);
                v.push_back("D42_COVERAGE domain=PHYSICS session="+n);v.push_back("x RELOAD_NATIVE_END session="+n);
                v.push_back("y F8_LIFECYCLE_END session="+n);v.push_back("H29R4 F5_END session="+n);}}
        return v;};
    for(unsigned step=0;step<60000;++step){
        const auto r=rng()%100;const D d=domains[rng()%9];
        if(r<12)cap::gate.select(d,true,g_now);
        else if(r<30)cap::gate.select(d,false,g_now);
        else if(r<36)(void)cap::gate.poll(g_now);
        const auto dt=rng()%4;g_now+=dt==0?1:dt==1?997:dt==2?4999:(rng()%200000);
        if(rng()%3==0)(void)cap::gate.poll(g_now);
        const bool quiet=cap::gate.quiet(g_now);
        if(quiet){++quietCalls;CK(!cap::gate.mask.load());
            for(auto e:domains){CK(!cap::active(e,g_now));CK(!cap::gate.receipt(e,cap::gate.session[unsigned(e)].load(),g_now));}
            CK(!OldEnabledForBug(g_now)); // KCD2VR_DIAG_LOG would print nothing
        }
        for(const auto& m:rows()){
            const bool before=OldWriteR145(m.c_str(),g_now);
            const int e0=kcd2vr::diagnostic_trace::g_enqueued;kcd2vr::diagnostic_trace::Write(m.c_str());
            const bool after=kcd2vr::diagnostic_trace::g_enqueued!=e0;
            CK(before==after);++total;admitted+=after;
            if(quiet)CK(!before); // what mod.cpp skips (COMBAT_FLOW/H12) was refused anyway
        }
    }
    CK(quietCalls>1000&&admitted>1000); // both paths really exercised
    // No latch: quiet ends the moment a recorder starts, and stays off for the 5 s end grace.
    for(auto d:{D::Bug,D::Combat,D::Interaction,D::Vrik,D::Reload,D::Physics}){
        for(auto e:domains)cap::gate.select(e,false,g_now);g_now+=6000;CK(cap::gate.quiet(g_now));
        cap::gate.select(d,true,g_now);CK(!cap::gate.quiet(g_now));
        const auto s=cap::gate.session[unsigned(cap::canonical(d))].load();
        cap::gate.select(d,false,g_now);CK(!cap::gate.quiet(g_now));
        g_now+=5000;CK(!cap::gate.quiet(g_now));CK(cap::gate.receipt(d,s,g_now));
        g_now+=1;CK(cap::gate.quiet(g_now));CK(!cap::gate.receipt(d,s,g_now));
        cap::gate.select(d,true,g_now);CK(!cap::gate.quiet(g_now));cap::gate.select(d,false,g_now);
    }
    // Expiry by poll() also opens the 5 s grace (closedAt), exactly like a stop.
    for(auto e:domains)cap::gate.select(e,false,g_now);g_now+=6000;
    cap::gate.select(D::Combat,true,g_now);g_now+=cap::Duration;CK(!cap::gate.quiet(g_now));
    CK(cap::gate.poll(g_now)!=0);CK(!cap::gate.quiet(g_now));g_now+=5001;CK(cap::gate.quiet(g_now));
    std::printf("quiet_write_r146: %u checks, %llu rows compared, %llu admitted, %llu quiet states: PASS\n",
        checks,(unsigned long long)total,(unsigned long long)admitted,(unsigned long long)quietCalls);
    return 0;
}
