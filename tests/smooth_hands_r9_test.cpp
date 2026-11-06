// R9 lane D: smooth hands default (VrSmoothHands), F2 rung 2 returns to the default, F7 finger block throttle.
#include "vrik/smooth_lab_f2.h"
#include "diagnostics/vrik_trace_h29.h"
#include <cstdio>
#include <cmath>
#include <string>
namespace sl=kcd2vr::smooth_lab;namespace hs=kcd2vr::hand_smooth_h50;namespace h=kcd2vr::hj1;namespace v7=kcd2vr::diagnostics::vrik_h29;
static int total=0,failed=0;
static void check(bool v,const char* n){++total;if(!v)++failed;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
int main(){
    const float frik=sl::CutoffHzForRetention(.7f);
    auto d=sl::DefaultsFor(true,false,1.5f,20.f);
    check(d.hj1&&d.damping&&std::fabs(d.minHz-frik)<1e-4f&&d.beta>=20.f,"default on: same-frame hands + FRIK damping (5.1 Hz)");
    d=sl::DefaultsFor(false,true,2.f,7.f);
    check(!d.hj1&&d.damping&&d.minHz==2.f&&d.beta==7.f,"VrSmoothHands=false: the user's own H50R2c values, no HJ1");
    // startup with the user's INI (H50R2c off) then default on
    hs::enabled.store(false);hs::minCutoffHz.store(1.5f);hs::beta.store(20.f);h::enabled.store(false);
    sl::ApplyDefaults(true);
    check(h::enabled.load()&&hs::enabled.load()&&std::fabs(hs::minCutoffHz.load()-frik)<1e-4f,"startup: hands smooth without any F-key");
    sl::SetEnabled(true);check(sl::Enabled()&&h::enabled.load()&&hs::enabled.load(),"F2 rung 2: lab on top of the default");
    sl::SetEnabled(false);
    check(h::enabled.load()&&hs::enabled.load()&&std::fabs(hs::minCutoffHz.load()-frik)<1e-4f,"leaving rung 2 goes back to smooth hands, not raw");
    hs::enabled.store(false);hs::minCutoffHz.store(1.5f);sl::ApplyDefaults(false);
    check(!h::enabled.load()&&!hs::enabled.load()&&hs::minCutoffHz.load()==1.5f,"VrSmoothHands=false = old raw hands");
    sl::SetEnabled(true);sl::SetEnabled(false);check(!h::enabled.load()&&!hs::enabled.load(),"default off: rung 2 off restores raw");
    sl::ApplyDefaults(true);sl::SetEnabled(true);sl::ApplyDefaults(false);
    check(h::enabled.load(),"INI reload during the lab does not tear it down");
    sl::SetEnabled(false);check(!h::enabled.load(),"...and leaving the lab then applies the reloaded default");
    sl::ApplyDefaults(true);
    check(std::string(sl::RungText(2)).find("smooth lab ON")!=std::string::npos&&std::string(sl::RungText(0)).find("smooth hands on")!=std::string::npos,"every F2 rung says what it is");
    // F7 finger throttle
    v7::Arm a{};a.fingers=15;for(unsigned i=0;i<15;++i){a.finger[i].valid=true;a.finger[i].localRotation={0,0,0,1};}
    v7::FingerMemoR9 m{};
    check(v7::fingerDueR9(m,a,1,1000),"first block always written");
    check(!v7::fingerDueR9(m,a,1,1025),"still fingers 25 ms later: no block");
    check(!v7::fingerDueR9(m,a,1,1249),"still fingers up to 249 ms: no block");
    check(v7::fingerDueR9(m,a,1,1250),"250 ms heartbeat block");
    auto b=a;const float s=std::sin(.06f);b.finger[7].localRotation={s,0,0,std::cos(.06f)}; // 0.12 rad
    check(v7::fingerDueR9(m,b,1,1260),"a curling finger (0.12 rad) is written at once");
    check(!v7::fingerDueR9(m,b,1,1285),"same pose again: no block");
    auto c=b;c.finger[3].localPosition={0,0,.006f};check(v7::fingerDueR9(m,c,1,1290),"5 mm joint move written");
    check(v7::fingerDueR9(m,c,2,1291),"new F7 session written");
    auto e=c;e.fingers=10;check(v7::fingerDueR9(m,e,2,1292),"finger map change written");
    // 90 records/s of a still hand for 10 s: blocks ~4/s instead of 40/s
    v7::FingerMemoR9 m2{};int blocks=0;for(int i=0;i<400;++i)blocks+=v7::fingerDueR9(m2,a,3,5000+std::uint64_t(i)*25)?1:0;
    check(blocks<=41,"still hand: 4 blocks/s instead of 40 (one per record)");
    std::printf("smooth_hands_r9 %d/%d\n",total-failed,total);return failed?1:0;
}
