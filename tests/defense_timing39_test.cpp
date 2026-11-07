// C39 VR DIRECT defense timing: skill-scaled announced contact ETA for a waiting reciprocal opponent (pure logic).
#include "../src/vrik/defense_timing39.h"
#include <cmath>
#include <cstdio>
#include <limits>
#include <string>
namespace t=kcd2vr::defense_timing39;
static int failures=0,checks=0;
#define CHECK(c,msg) do{++checks;if(!(c)){++failures;std::printf("FAIL %s\n",msg);}else std::printf("PASS %s\n",msg);}while(0)
static bool near(float a,float b){return std::fabs(a-b)<1e-4f;}
int main(){
    // phases: only idle/guard/block-preparing/block-ready count as waiting
    CHECK(t::waitingPhase(1)&&t::waitingPhase(2)&&t::waitingPhase(128)&&t::waitingPhase(256),"idle, guard, block preparing and block ready are waiting/guarding");
    CHECK(!t::waitingPhase(4)&&!t::waitingPhase(8)&&!t::waitingPhase(16)&&!t::waitingPhase(32)&&!t::waitingPhase(64)&&!t::waitingPhase(512)&&!t::waitingPhase(0)&&!t::waitingPhase(-99),
          "own attack wind-up/strike/withdraw/recover, hit reaction, other and unknown phases are never waiting");
    // skill mapping
    int none[5]{-1,-1,-1,-1,-1},mixed[5]{3,12,-1,41,7},master[5]{30,0,0,0,0};
    CHECK(t::bestSkill(none)==-1&&t::bestSkill(mixed)==12&&t::bestSkill(master)==30,"best melee skill ignores unread and out-of-range values");
    CHECK(near(t::skillFactor(0,2.f,3.f),2.f)&&near(t::skillFactor(15,2.f,3.f),2.5f)&&near(t::skillFactor(30,2.f,3.f),3.f)&&near(t::skillFactor(40,2.f,3.f),3.f),
          "skill 0 -> low factor, 15 -> half way, 30 and above -> high factor");
    CHECK(near(t::skillFactor(-1,2.f,3.f),2.f),"unread skill uses the low (menu minimum) factor");
    CHECK(near(t::skillFactor(30,5.f,9.f),3.f)&&near(t::skillFactor(0,.2f,.5f),1.f)&&near(t::skillFactor(10,std::numeric_limits<float>::quiet_NaN(),3.f),1.f+2.f/3.f),
          "factors are clamped to 1..3, NaN means native speed");
    CHECK(near(t::skillFactor(30,3.f,1.f),1.f)&&near(t::skillFactor(0,3.f,1.f),3.f),"an inverted low/high pair is honoured, still inside 1..3");
    // effective ETA
    CHECK(near(t::effectiveEta(.25f,1.f),.25f)&&near(t::effectiveEta(.30f,2.f),.15f)&&near(t::effectiveEta(.30f,3.f),.10f),"seed divided by the factor");
    CHECK(near(t::effectiveEta(.20f,3.f),.08f)&&near(t::effectiveEta(.10f,3.f),.08f),"never below the 80 ms floor");
    CHECK(near(t::effectiveEta(.06f,3.f),.06f),"a seed already under the floor is not lengthened");
    CHECK(near(t::effectiveEta(.25f,.5f),.25f)&&near(t::effectiveEta(.25f,std::numeric_limits<float>::infinity()),.25f),"never longer than the configured seed");
    CHECK(t::effectiveEta(0.f,2.f)==0.f&&!(t::effectiveEta(-1.f,2.f)>0.f),"no seed stays no seed");
    // decisions
    auto d=t::decide(true,.25f,true,true,2,30,2.f,3.f);
    CHECK(d.reason==t::Reason::Applied&&near(d.factor,3.f)&&near(d.eta,.0833333f)&&d.phase==2&&d.skill==30,"guarding master: 250 ms -> 83 ms (3x)");
    d=t::decide(true,.25f,true,true,256,0,2.f,3.f);
    CHECK(d.reason==t::Reason::Applied&&near(d.eta,.125f),"block already up, unskilled: 250 ms -> 125 ms (2x)");
    d=t::decide(true,.25f,true,true,8,30,2.f,3.f);
    CHECK(d.reason==t::Reason::Attacking&&near(d.eta,.25f)&&near(d.factor,1.f),"his own strike is running: announcement unchanged (Miki rule)");
    d=t::decide(true,.25f,true,true,64,30,2.f,3.f);
    CHECK(d.reason==t::Reason::Attacking&&near(d.eta,.25f),"hit reaction running: unchanged");
    d=t::decide(true,.25f,true,false,2,30,2.f,3.f);
    CHECK(d.reason==t::Reason::NotReciprocal&&near(d.eta,.25f),"opponent not targeting Henry: unchanged");
    d=t::decide(true,.25f,false,false,-99,-1,2.f,3.f);
    CHECK(d.reason==t::Reason::NoOpponent&&near(d.eta,.25f),"no exact opponent: unchanged");
    d=t::decide(true,.25f,true,true,-99,30,2.f,3.f);
    CHECK(d.reason==t::Reason::PhaseUnknown&&near(d.eta,.25f),"unreadable phase fails closed");
    d=t::decide(false,.25f,true,true,2,30,2.f,3.f);
    CHECK(d.reason==t::Reason::Disabled&&near(d.eta,.25f)&&near(d.factor,1.f),"menu off: unchanged");
    d=t::decide(true,0.f,true,true,2,30,2.f,3.f);
    CHECK(d.reason==t::Reason::NoSeed&&d.eta==0.f,"missing provenance (zero seed) stays zero");
    d=t::decide(true,.25f,true,true,2,30,1.f,1.f);
    CHECK(d.reason==t::Reason::Applied&&near(d.eta,.25f),"1.0/1.0 is the native speed even when applied");
    // C37C2 evidence: the four still-preparing contacts (lead 113/117/136/83 ms after the offer of 269 ms)
    const float leads[4]{.155f,.161f,.182f,.121f};int covered=0;
    for(const float lead:leads){const auto e=t::decide(true,.30f,true,true,2,0,2.f,3.f);if(e.eta<lead)++covered;}
    CHECK(covered==3,"seed 0.30 s, unskilled (2x = 150 ms): 3 of the 4 real C37C2 still-preparing arrivals come after the announced contact");
    covered=0;for(const float lead:leads){const auto e=t::decide(true,.30f,true,true,2,30,2.f,3.f);if(e.eta<lead)++covered;}
    CHECK(covered==4,"seed 0.30 s, master (3x = 100 ms): all 4 come after the announced contact");
    CHECK(std::string(t::name(t::Reason::Applied))=="APPLIED"&&std::string(t::name(t::Reason::Attacking))=="NPC_BUSY_NOT_WAITING","receipt names");
    std::printf("defense_timing39: %d/%d checks passed\n",checks-failures,checks);
    return failures?1:0;
}
