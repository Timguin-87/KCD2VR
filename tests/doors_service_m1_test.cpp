// Actual moved Doors service + actual production host bridge. The legacy
// reference is the byte-for-byte pre-extraction block, SHA256 recorded below.
// Native leaves are explicit doubles; no game/DLL/VM is executed here.
#include "subsystems/doors/module.h"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstdint>
namespace doors=kcd2vr::subsystems::doors;
struct Source {
    std::uint64_t now=1000;
    int mode=2;
    bool calibration=true,action=true,retail=true,owner=true,
        ready=false,enable_result=false,stop_result=true;
};
static Source source;
static std::vector<std::string>* trace=nullptr;
static unsigned checks=0;
static void ck(bool yes,const char* why){
    ++checks;if(!yes){std::fprintf(stderr,"FAIL %s\n",why);std::abort();}
}
static void event(const char* s){trace->emplace_back(s);}
static std::uint64_t GetTickCount64() noexcept {event("Clock");return source.now;}
static int MiscInteractionDiagMode() noexcept {event("Mode");return source.mode;}
static bool CalibrationReady() noexcept {event("Calibration");return source.calibration;}
static bool InteractionActionReady175() noexcept {event("ActionReady");return source.action;}
static std::uintptr_t WhBase() noexcept {event("Base");return 1;}
namespace retail_build112 {
static bool Ready(std::uintptr_t) noexcept {event("Retail");return source.retail;}
}
namespace action_time_owner59c11 {
static bool Installed() noexcept {event("OwnerInstalled");return source.owner;}
}
static void StartNativeDoor19c4() noexcept {event("Start");}
static bool StopNativeDoor19c4() noexcept {event("Stop");return source.stop_result;}
namespace diagnostic_trace {
static void Write(const char* text) noexcept {trace->emplace_back(std::string("Log:")+text);}
}
namespace kcd2vr::door_use46 {static std::atomic<bool> enabled{true};}
namespace kcd2vr::door_signal19c4r4 {
static bool Ready() noexcept {event("Ready");return source.ready;}
static bool SetEnabled(bool on) noexcept {event(on?"Enable1":"Enable0");return source.enable_result;}
}
static void LegacyTick(){
// Frozen old production block SHA256:
// edfd86b5fb3b982087454e54b7fa49a5fb16402c5853b7e7f4d308cf31a9ce16
#include "fixtures/door_service46_legacy_m1.inc"
}
static void CurrentTick(){
#include "../src/vrik/door_service46.inl"
}
static unsigned occurrences(const std::vector<std::string>& rows,const char* text){
    unsigned n=0;for(const auto& x:rows)if(x==text)++n;return n;
}
static std::vector<std::string> step(){
    std::vector<std::string> old_trace,new_trace;
    // The legacy scheduler is now intentionally admitted for mode3 as well.
    // Compare the unchanged scheduling order with its mode2 reference.
    const int selected=source.mode;if(selected==3)source.mode=2;
    trace=&old_trace;LegacyTick();source.mode=selected;trace=&new_trace;CurrentTick();
    ck(old_trace==new_trace,"actual module+production bridge preserve old native/query/log order");
    ck(occurrences(new_trace,"Clock")==1,"original worker clock is sampled exactly once");
    return new_trace;
}
int main(){
    ck(doors::Descriptor().id==9&&doors::Descriptor().lifecycle==kcd2vr::subsystems::Lifecycle::HostOwned,
       "Doors descriptor retains explicit host-owned native lifecycle");
    kcd2vr::door_use46::enabled=false;
    auto rows=step();ck(rows==std::vector<std::string>{"Clock","Stop"},"disabled setting avoids all native/readiness discovery");
    kcd2vr::door_use46::enabled=true;source.mode=0;rows=step();
    ck(rows==std::vector<std::string>{"Clock","Mode","Stop"},"mode refusal retains short-circuit admission");
    source.mode=2;source.calibration=false;rows=step();
    ck(occurrences(rows,"ActionReady")==0&&occurrences(rows,"Stop")==1,"uncalibrated refusal does not query action");
    source.calibration=true;source.action=false;rows=step();
    ck(occurrences(rows,"Base")==0&&occurrences(rows,"Stop")==1,"action refusal does not query retail/module");
    source.action=true;source.retail=false;rows=step();
    ck(occurrences(rows,"OwnerInstalled")==0,"retail refusal does not query action owner");
    source.retail=true;source.owner=false;rows=step();
    ck(occurrences(rows,"Ready")==0&&occurrences(rows,"Stop")==1,"owner refusal never opens bridge");
    source.owner=true;source.now=1000;rows=step();
    ck(occurrences(rows,"Enable1")==1&&occurrences(rows,"Start")==0,"first unavailable bridge is attempted once without start");
    ck(occurrences(rows,"Log:C46 DOOR_SERVICE bridge=UNAVAILABLE exactPinsOrHook=REFUSED nativeControls=AVAILABLE")==1,
       "first unsuccessful bridge gets exact legacy edge receipt");
    source.now=1001;rows=step();ck(occurrences(rows,"Enable1")==0,"retry is suppressed before 1000ms deadline");
    source.now=1999;rows=step();ck(occurrences(rows,"Enable1")==0,"retry remains suppressed one millisecond before deadline");
    source.now=2000;rows=step();ck(occurrences(rows,"Enable1")==1&&occurrences(rows,"Start")==0,
       "exact deadline retries native enable but false return never starts");
    ck(occurrences(rows,"Log:C46 DOOR_SERVICE bridge=UNAVAILABLE exactPinsOrHook=REFUSED nativeControls=AVAILABLE")==0,
       "identical failure outcome does not repeat edge log");
    source.now=3000;source.enable_result=true;rows=step();
    ck(occurrences(rows,"Enable1")==1&&occurrences(rows,"Start")==1,"successful native enable starts once at same worker call");
    ck(occurrences(rows,"Log:C46 DOOR_SERVICE bridge=READY source=NORMAL_CALIBRATED_HANDLING companionAck=REQUIRED")==1,
       "changed native bridge outcome gets exact legacy receipt");
    source.now=3001;source.ready=true;rows=step();
    ck(occurrences(rows,"Start")==1&&occurrences(rows,"Enable1")==0,"already-ready bridge bypasses retry gate and never re-enables");
    source.now=3002;source.stop_result=false;kcd2vr::door_use46::enabled=false;rows=step();
    ck(occurrences(rows,"Stop")==1&&occurrences(rows,"Log:C46 DOOR_SERVICE mode=REVOKED cleanup=NATIVE_AND_LUA_OWNERS")==1,
       "refused native stop remains cleanup request and emits one revocation edge");
    source.now=3003;rows=step();
    ck(rows==std::vector<std::string>{"Clock","Stop"},"disabled service continues native cleanup without repeated revocation");
    kcd2vr::door_use46::enabled=true;source.ready=false;source.enable_result=false;source.now=3004;rows=step();
    ck(occurrences(rows,"Enable1")==1,"disable resets retry admission for genuine later re-enable");
    source.now=2800;rows=step();ck(occurrences(rows,"Enable1")==0,"clock reversal preserves old absolute retry behavior");

    // Differentially exercise many deterministic current inputs. This is not a
    // fake native lifecycle; it checks only exact scheduling/query side effects.
    std::uint32_t random=0x7D0019;
    for(unsigned i=0;i<4096;++i){
        random=random*1664525u+1013904223u;
        source.now+=random%1200;source.mode=(random&1)?((random&512)?3:2):0;
        source.calibration=(random&2)!=0;source.action=(random&4)!=0;
        source.retail=(random&8)!=0;source.owner=(random&16)!=0;
        source.ready=(random&32)!=0;source.enable_result=(random&64)!=0;
        source.stop_result=(random&128)!=0;kcd2vr::door_use46::enabled=(random&256)!=0;
        (void)step();
    }
    // The explicit-state entry is the exact function used by the production
    // owning overload. Missing providers cannot erase a pending service state.
    std::vector<std::string> state_trace;trace=&state_trace;
    const doors::ServiceLeaves leaves{&kcd2vr::door_signal19c4r4::Ready,
        &kcd2vr::door_signal19c4r4::SetEnabled,&StartNativeDoor19c4,
        &StopNativeDoor19c4,&diagnostic_trace::Write};
    for(unsigned missing=0;missing<5;++missing){
        auto absent=leaves;
        switch(missing){case 0:absent.ready=nullptr;break;case 1:absent.set_enabled=nullptr;break;
            case 2:absent.start=nullptr;break;case 3:absent.stop=nullptr;break;default:absent.write=nullptr;}
        doors::ServiceState state{true,12345,1};state_trace.clear();
        ck(!doors::TickService(state,{false},5000,absent),"missing typed leaf refuses service");
        ck(state.previous_wanted&&state.retry_after==12345&&state.reported_bridge==1&&state_trace.empty(),
           "provider refusal preserves lifecycle state and issues no partial native operation");
    }
    std::printf("doors_service_m1=%u checks PASS; actual module+host bridge; native_execution=NONE\n",checks);
}
