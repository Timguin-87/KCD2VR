// Literal production publisher/dispatcher is extracted by run_pad_dispatch_d84.py.
// Only the native emitter and clock are controlled. An emit callback publishes
// the next complete pad at every possible interleave within the native pass.
#include "vrik/attack_xi178r8.h"
#if __has_include("subsystems/hud_ui_controls/runtime/pad_dispatch_d84.h")
#include "subsystems/hud_ui_controls/runtime/pad_dispatch_d84.h"
#endif
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace xi=kcd2vr::attack_xi178r8;
struct Event {std::string name;float value=0;bool digital=false,held=false,packet=false;xi::Packet p{};};
static std::vector<Event> events;
static std::function<void(unsigned)> afterEvent;
static std::uint64_t tick=100;
static int carryReads=0,followReads=0; // R147: follow prompt query rides the same mode-3 dispatch
static bool g_original=true;
static std::atomic<int> g_gamepad_mode{3};
static std::atomic<std::uint64_t> g_pad_last_emit{0};
static std::atomic<bool> g_pad_stick_keys{true};
static bool g_stick_key_state[8]{};
static std::uint64_t GetTickCount64(){return tick;}
static void UpdateCarryQuery25(){++carryReads;}
static void UpdateFollowQuery147(){++followReads;}
static void emit_pad(std::uintptr_t,char,const char* name,float value,bool digital=false,bool held=false,const xi::Packet* packet=nullptr){
    events.push_back({name,value,digital,held,packet!=nullptr,packet?*packet:xi::Packet{}});
    if(afterEvent)afterEvent(unsigned(events.size()));
}
#include "pad_dispatch_source.inc"

struct Input {
    float lx=0,ly=0,rx=0,ry=0,lt=0,rt=0;
    bool a=false,b=false,x=false,y=false,lb=false,rb=false,l3=false,r3=false,start=false,back=false;
    bool du=false,dd=false,dl=false,dr=false;
    unsigned mask=0,serial=0,stroke=0,nonce=0;
};
static void publish(const Input& p){
    xi::ownedMask=p.mask;xi::prepareSerial=p.serial;xi::pendingStroke=int(p.stroke);
    xi::costEnvelope54=std::uint64_t(p.nonce)|(std::uint64_t(p.serial)<<16)|(std::uint64_t(p.stroke)<<48);
    PublishGamepadState(p.lx,p.ly,p.rx,p.ry,p.lt,p.rt,p.a,p.b,p.x,p.y,p.lb,p.rb,p.l3,p.r3,p.start,p.back,p.du,p.dd,p.dl,p.dr);
}
static void dispatch(){tick+=8;flush_gamepad_same_dispatch_thread(123,0);}
static void reset(){events.clear();afterEvent={};carryReads=0;followReads=0;g_original=true;g_gamepad_mode=3;g_pad_stick_keys=true;for(auto& b:g_stick_key_state)b=false;tick+=20;}
static bool same(const Event& a,const Event& b){
    return a.name==b.name&&a.value==b.value&&a.digital==b.digital&&a.held==b.held&&a.packet==b.packet&&
        (!a.packet||(a.p.rt==b.p.rt&&a.p.lt==b.p.lt&&a.p.a==b.p.a&&a.p.lb==b.p.lb&&a.p.mask==b.p.mask&&a.p.serial==b.p.serial&&a.p.stroke==b.p.stroke&&a.p.costNonce54==b.p.costNonce54));
}
static bool same(const std::vector<Event>& a,const std::vector<Event>& b){
    if(a.size()!=b.size())return false;for(unsigned i=0;i<a.size();++i)if(!same(a[i],b[i]))return false;return true;
}
static int failures=0,checks=0;
static void check(bool ok,const char* name){++checks;if(!ok){++failures;std::printf("FAIL %s\n",name);}}
static std::vector<Event> reference(const Input& p){reset();publish(p);dispatch();return events;}
static void interleave(const Input& first,const Input& next,const char* name){
    const auto expected=reference(first),following=reference(next);
    for(unsigned at=1;at<=expected.size();++at){
        reset();publish(first);afterEvent=[&](unsigned n){if(n==at)publish(next);};dispatch();
        if(!same(events,expected)){++failures;std::printf("FAIL %s interleave=%u coherent-dispatch\n",name,at);}++checks;
        afterEvent={};events.clear();dispatch();check(same(events,following),"next-published-frame-emits-without-extra-delay");
    }
}
int main(){
    Input forward{};forward.ly=1;forward.rx=1;forward.ry=-1;forward.lt=.321f;forward.rt=.876f;
    forward.a=forward.x=forward.rb=forward.r3=forward.back=forward.du=forward.dl=true;
    forward.mask=7;forward.serial=17;forward.stroke=1;forward.nonce=49151;
    Input side{};side.lx=-1;side.rx=-1;side.ry=1;side.lt=.678f;side.rt=.123f;
    side.b=side.y=side.lb=side.l3=side.start=side.dd=side.dr=true;
    side.mask=11;side.serial=18;side.stroke=2;side.nonce=12345;
    Input back=side;back.lx=0;back.ly=-1;
    const Input neutral{};
    interleave(forward,side,"forward-to-side-B");
    interleave(forward,back,"forward-to-back-B");
    interleave(side,neutral,"dodge-and-combat-release");
    interleave(neutral,side,"neutral-to-dodge-and-combat-down");
    reset();publish(side);dispatch();const auto held=events;events.clear();dispatch();check(same(events,held),"held-frame-repeats-existing-native-semantics");check(carryReads==2&&followReads==2,"R147 follow query once per mode-3 dispatch, beside the carry query");
    events.clear();const auto last=tick;flush_gamepad_same_dispatch_thread(123,0);check(events.empty()&&tick==last,"eight-ms-throttle-unchanged");
    reset();publish(side);g_gamepad_mode=2;dispatch();check(events.size()==12&&carryReads==0&&followReads==0,"axes-only-mode");
    reset();publish(side);g_gamepad_mode=0;dispatch();check(events.empty()&&carryReads==0&&followReads==0,"disabled-mode-no-emission");
    reset();publish(side);g_original=false;dispatch();check(events.empty()&&carryReads==0,"missing-native-dispatch-no-emission");
    reset();publish(side);g_pad_stick_keys=false;dispatch();check(events.size()==22,"direction-key-option-preserved");
    std::printf("pad dispatch: %d checks, %d failures\n",checks,failures);
    return failures?1:0;
}
