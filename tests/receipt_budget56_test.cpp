// C56 review (Claude): the F3 combat recorder's rate cap must be per call SITE, never per category.
//
// Defect (C55 baseline 22b79808, src/diagnostics/native_hit_probe134.inl:57-62): Allow(category,perSecond)
// kept one per-second counter per category. 31 call sites share category 4 with limits 2..30, so after a
// chatty site (HENRY_COMBAT_EDGE 30/s, NPC_PHASE 20/s, GEOM_LIVE_HIT 20/s) wrote 4 lines in a second, the
// refusal receipts (GEOM_HIT rejected=... 4/s, VR_HIT_SKIPPED 2/s) were refused for the rest of that second
// although THEY had written nothing. A refused contact then looks like a silent drop in the log.
//
// The repro below runs the baseline function (copied verbatim, only GetTickCount64 replaced by the host
// clock) against the per-site production header and shows the before/after difference on the same call
// pattern. The baseline block is a counterexample; it is not a production target.
#include "../src/diagnostics/receipt_budget56.h"
#include <cassert>
#include <cstdio>
static std::uint64_t hostNowMs=5000;
namespace baseline55 {
// src/diagnostics/native_hit_probe134.inl @22b79808 lines 57-62, GetTickCount64() -> hostNowMs.
bool Allow(int category,unsigned perSecond) noexcept {
    static std::atomic<std::uint64_t> second[8]{};static std::atomic<unsigned> used[8]{};
    const auto now=hostNowMs/1000;
    if(second[category].exchange(now)!=now)used[category].store(0);
    return used[category].fetch_add(1)<perSecond;
}
}
int main(){
    unsigned checks=0;auto ck=[&](bool ok,const char* what){++checks;std::printf("%s %s\n",ok?"PASS":"FAIL",what);assert(ok);};
    // The C54 15:56:16 second: three HENRY_COMBAT_EDGE lines (Allow(4,30)) and one NPC_PHASE line (Allow(4,20))
    // before a GEOM_HIT rejected=... receipt (Allow(4,4)) that had written nothing yet in that second.
    {bool refusalWritten=false;
     for(int i=0;i<3;++i)(void)baseline55::Allow(4,30);
     (void)baseline55::Allow(4,20);
     refusalWritten=baseline55::Allow(4,4);
     ck(!refusalWritten,"baseline (shared category counter): the refusal receipt is starved by other sites in the same second");}
    {using namespace kcd2vr::receipt56;
     static Site combatEdge,npcPhase,geomRejected,vrSkipped;
     for(int i=0;i<3;++i)ck(allow(combatEdge,30,hostNowMs),"chatty site writes within its own budget");
     ck(allow(npcPhase,20,hostNowMs),"second chatty site writes within its own budget");
     ck(allow(geomRejected,4,hostNowMs),"per-site: the refusal receipt still has its own 4/s budget");
     // A site's own cap still holds: 4 allowed, the 5th refused and counted.
     for(int i=0;i<3;++i)ck(allow(geomRejected,4,hostNowMs),"own budget 2..4");
     ck(!allow(geomRejected,4,hostNowMs),"own cap reached: 5th line in the same second refused");
     ck(geomRejected.allowed.load()==4&&geomRejected.refused.load()==1,"allowed/refused receipts count the site's own calls");
     ck(combatEdge.refused.load()==0&&npcPhase.refused.load()==0,"other sites are untouched by that refusal");
     // A 2/s site is independent too.
     ck(allow(vrSkipped,2,hostNowMs)&&allow(vrSkipped,2,hostNowMs)&&!allow(vrSkipped,2,hostNowMs),"2/s site: two lines then refused");
     // Next wall-clock second: every site starts fresh, counters never reset.
     hostNowMs+=1000;
     ck(allow(geomRejected,4,hostNowMs),"new second: budget renewed");
     ck(geomRejected.allowed.load()==5&&geomRejected.refused.load()==1,"receipt counters are cumulative");
     // Same second again (clock does not move backwards inside the second): still per-site.
     for(int i=0;i<40;++i)(void)allow(combatEdge,30,hostNowMs);
     ck(allow(geomRejected,4,hostNowMs),"40 chatty calls in this second do not touch the refusal site");
     ck(combatEdge.refused.load()==10,"the chatty site's own overflow is counted on itself (40 calls, 30 allowed)");}
    std::printf("receipt_budget56: %u checks\n",checks);
    return 0;
}
