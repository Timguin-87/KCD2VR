// C23b OUTCOME_CHAIN receipt builder (production header diagnostics/outcome_chain23.h).
// Pure joiner/formatter only: no WHGame image, no native call, no forced result.
#include "diagnostics/outcome_chain23.h"
#include <cstdio>
#include <cstring>
namespace oc=kcd2vr::outcome23;
static int total=0,failed=0;
static void ck(bool b,const char* s){++total;failed+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",s);}
static bool has(const char* line,const char* needle){return std::strstr(line,needle)!=nullptr;}
static oc::Hit applied(int death=0,int combo=0,int master=0){oc::Hit h{};h.ok=h.apply=true;h.death=death;h.combo=combo;h.master=master;h.blocked=h.perfect=h.parry=h.comboEnd=h.ko=0;return h;}
int main(){
    // D4AD94 branch order and pre-read honesty.
    ck(oc::classify(true,false,false,0,false,false)==oc::Route::Ordinary,"ordinary: nothing set, returned 0");
    ck(oc::classify(true,false,false,2,true,true)==oc::Route::Master,"+7D8 wins over the combo slot (D4AD94 order)");
    ck(oc::classify(true,true,true,2,true,true)==oc::Route::Special830,"830 && heavy input is tested before +7D8");
    ck(oc::classify(true,true,false,2,false,true)==oc::Route::Master,"830 without heavy input falls through to +7D8");
    ck(oc::classify(true,false,false,0,true,true)==oc::Route::Combo,"combo slot route");
    ck(oc::classify(true,false,false,0,true,false)==oc::Route::Unknown,"predicted consuming route that returned 0 is UNKNOWN, not combo");
    ck(oc::classify(true,false,false,0,false,true)==oc::Route::OtherConsumed,"returned 1 with no pre-read cause = OTHER_CONSUMED");
    ck(oc::classify(false,false,false,1,false,true)==oc::Route::Unknown,"unreadable pre-read is UNKNOWN");
    ck(oc::classify(true,false,false,-1,false,true)==oc::Route::Unknown,"negative count is UNKNOWN");

    oc::Counters k{};oc::Core c{};char line[1600]{};oc::Chain out{};
    // Gesture 7: master window open at prepare, full observed chain, applied master hit.
    c.edge(7,1,true,2,true,true,1000,k);
    c.xi(true,2,1010,k);
    c.admission(true,oc::Route::Master,0,1010,k);
    c.factory(true,true,1011,k);c.dispatch(true,1011,k);c.kind5(true,1011,k);
    c.edge(7,1,false,0,true,true,1300,k);
    c.hit(0,applied(1,0,1),1500,k);
    ck(k.msOpenAtPrepare==1&&k.msFactoryQ==1&&k.msRef==1&&k.msAccepted==1&&k.kills==1,"counters: window/factory/ref/dispatch/kill counted once");
    ck(!c.pop(out),"edge-only: nothing emitted while the gesture is open");
    c.pump(1500+oc::Core::idleMs,k);ck(!c.pop(out),"pump keeps a chain exactly at the idle bound");
    c.pump(1501+oc::Core::idleMs,k);ck(c.pop(out)&&out.gesture==7,"idle chain closes once");
    ck(!c.pop(out),"closed exactly once");
    oc::format(out,5000,line,sizeof(line));
    ck(has(line,"C23 OUTCOME_CHAIN gesture=7 ")&&has(line,"master=APPLIED_MASTER_FLAG")&&has(line,"kill=DEATH_FLAG ")&&has(line,"ms7D8{prepare=2 commit=0 xiPrepare=2 xiRelease=-2}"),"format: joined master chain");
    ck(has(line,"row=UNOBSERVED")&&has(line,"actionEntry=UNOBSERVED")&&has(line,"writes=NONE"),"format: unobserved links named, no writes");

    // Gesture 8: window open, D4AD94 took another route -> stop at D4AD94.
    c.edge(8,1,true,1,true,true,6000,k);c.admission(true,oc::Route::Special830,0,6001,k);
    // Gesture 9 starts -> 8 closes (new gesture edge).
    c.edge(9,2,true,0,false,true,6200,k);
    ck(c.pop(out)&&out.gesture==8&&!std::strcmp(oc::masterVerdict(out),"STOP_AT_D4AD94_ROUTE"),"new gesture closes the previous; route stop named");
    // Gesture 9: no window; apply=0 evaluation must not become an outcome.
    oc::Hit eval{};eval.ok=true;eval.apply=false;eval.death=1;eval.master=1;
    c.hit(9,eval,6300,k);
    ck(k.evalSkipped==1&&k.kills==1,"apply=0 evaluation skipped: no kill, no master counted");
    oc::Hit refused{};refused.ok=false;refused.apply=true;refused.death=1;c.hit(9,refused,6301,k);
    ck(k.evalSkipped==1&&k.kills==1,"false D568B0 return is no result at all");
    c.hit(9,applied(0,1,0),6400,k);c.slot16(false,6410,k);
    c.pump(9000,k);ck(c.pop(out)&&out.gesture==9,"gesture 9 closed");
    ck(out.exact==1&&out.temporal==0&&out.evalSkipped==1&&out.applied==1,"exact direct-route join; evaluation recorded as skipped only");
    ck(!std::strcmp(oc::masterVerdict(out),"NO_WINDOW")&&!std::strcmp(oc::comboOutVerdict(out),"SLOT_NOT_REQUESTED_909A90_FALSE"),"combo-out stop at 909A90");
    ck(k.comboAdvance==1&&k.comboSlotOpen==0,"combo byte counted; slot not counted when not requested");

    // Factory gate unarmed vs factory refused.
    c.edge(10,1,true,1,false,true,10000,k);c.admission(true,oc::Route::Master,0,10001,k);c.kind5(false,10002,k);
    c.edge(11,1,true,1,true,true,10100,k);
    ck(c.pop(out)&&!std::strcmp(oc::masterVerdict(out),"FACTORY_GATE_UNARMED"),"unarmed factory observer is not a refusal");
    c.admission(true,oc::Route::Master,0,10101,k);c.kind5(false,10102,k);c.factory(false,false,10102,k);
    c.pump(20000,k);ck(c.pop(out)&&!std::strcmp(oc::masterVerdict(out),"STOP_FACTORY_GROUP_FALSE"),"factory group false named");

    // Combo-in: the NEXT press inside a slot opened earlier.
    c.edge(12,1,true,0,true,true,30000,k);c.admission(true,oc::Route::Combo,1,30001,k);c.admission(false,oc::Route::Combo,1,30200,k);
    c.pump(40000,k);ck(c.pop(out)&&!std::strcmp(oc::comboInVerdict(out),"C5AC2C_ROUTE_ADVANCE_UNOBSERVED")&&k.comboRoute==1,"combo route counted once per gesture, advance not claimed");

    // Temporal joins only inside the bound; unjoined is counted, never invented.
    c.hit(0,applied(),50000,k);ck(k.unjoined>=1,"hit with no open chain is unjoined");
    const auto before=k.unjoined.load();c.edge(13,1,true,0,true,true,60000,k);c.hit(0,applied(),60000+oc::Core::temporalMs+1,k);
    ck(k.unjoined==before+1,"temporal join refuses beyond the bound");
    c.hit(99,applied(),60010,k);ck(k.unjoined==before+2,"exact gesture mismatch is unjoined, not re-keyed");
    c.slot16(true,60020,k);ck(k.comboSlotOpen==1,"requested slot counted");

    // Death reaction row only when the reaction equals the native death kind.
    oc::Hit d=applied(1);d.reaction=5;d.deathKind=5;c.hit(13,d,60030,k);
    oc::Hit n=applied(0);n.reaction=4;n.deathKind=5;c.hit(13,n,60040,k);
    ck(k.deathRows==1,"deathRows only for reaction==deathKind");

    // Ring is bounded: overflow is counted, never grows.
    oc::Counters k2{};oc::Core r{};
    for(std::uint64_t g=1;g<=oc::Core::ringSize+3;++g)r.edge(g,1,true,0,true,true,g*10,k2);
    r.pump(1000000,k2);
    unsigned popped=0;while(r.pop(out))++popped;
    ck(popped==oc::Core::ringSize&&k2.dropped==3,"ring bounded at 8; extra closes counted as dropped");

    // Locked wrapper and same-stack scope.
    ck(!oc::onStack(0x10),"no stack outside D4AD94");
    {oc::StackScope s{true,0x10};ck(oc::onStack(0x10)&&!oc::onStack(0x20),"same-stack join only for the same actor");
     {oc::StackScope inner{false,0x20};ck(oc::onStack(0x10),"unowned nested scope keeps the outer owner");}}
    ck(!oc::onStack(0x10),"scope restored");
    bool ran=false;ck(oc::with([&](oc::Core&){ran=true;})&&ran,"uncontended update runs");
    {std::unique_lock<std::mutex> hold(oc::mutex);bool ran2=false;ck(!oc::with([&](oc::Core&){ran2=true;})&&!ran2&&oc::counters.busy==1,"contended update is dropped and counted, never waits");}
    char flow[600]{};oc::formatCounters(k,flow,sizeof(flow));
    ck(has(flow,"chain{msOpenAtPrepare=")&&has(flow,"deathRows=1")&&std::strlen(flow)<300,"COMBAT_FLOW chain{} group bounded");
    std::printf("%d/%d passed\n",total-failed,total);return failed?1:0;
}
