#include "../src/subsystems/direct/combat_router_h3.h"
#include "../src/subsystems/experimental/deepseek_options_h29.h"
#include <cassert>
#include <cstdio>
static constexpr int VK_SHIFT=16;
static bool shift=false;
short GetAsyncKeyState(int key){return key==VK_SHIFT&&shift?short(0x8000):0;}
namespace diagnostic_trace {void Write(const char*){}}
struct Log {template<class... A> void warning(const char*,A&&...){}};
struct Menu {unsigned calls=0;void ToggleDeepSeekLabH29(){++calls;}};
struct Fixture {
    Menu menu;Menu* vr_menu_=&menu;unsigned cues=0;
    bool f12_down_prev=false;
    Log logger;Log& log(){return logger;}
    void LabLadderCue(int){++cues;}
    void poll(bool f12_down,bool matrix_active=false){
#include "generated_shift_f12_h29.inc"
        f12_down_prev=f12_down;
    }
};
int main(){
    Fixture f;
    for(int mode=0;mode<3;++mode){
        kcd2vr::routerH3::mode.store(mode);
        shift=true;const auto before=f.menu.calls;
        f.poll(true);for(int i=0;i<10;++i)f.poll(true);f.poll(false);
        assert(f.menu.calls==before+1&&kcd2vr::routerH3::mode.load()==mode&&f.cues==0);
        assert(!kcd2vr::deepseek_h29::angularVignette.load());
        assert(!kcd2vr::deepseek_h29::wallBackoutSpringH31.load());
        assert(!kcd2vr::deepseek_h29::fingerContactSpringH31.load());
        f.poll(true,true);f.poll(false);
        assert(f.menu.calls==before+1);
    }
    kcd2vr::routerH3::mode.store(0);shift=false;
    for(int expected: {1,2,0}){f.poll(true);f.poll(false);assert(kcd2vr::routerH3::mode.load()==expected);}
    assert(f.cues==3);
    shift=true;f.vr_menu_=nullptr;f.poll(true);f.poll(false);
    assert(kcd2vr::routerH3::mode.load()==0);
    // Opening the session menu never enables a trial or resets an existing choice.
    f.vr_menu_=&f.menu;kcd2vr::deepseek_h29::fingerContactSpringH31=true;
    const auto kept=f.menu.calls;f.poll(true);f.poll(false);
    assert(f.menu.calls==kept+1&&kcd2vr::deepseek_h29::fingerContactSpringH31.load()&&
           !kcd2vr::deepseek_h29::wallBackoutSpringH31.load()&&!kcd2vr::deepseek_h29::angularVignette.load());
    kcd2vr::deepseek_h29::DisableAll();assert(!kcd2vr::deepseek_h29::fingerContactSpringH31.load());
    std::puts("PASS actual Shift F12 rising edge, matrix exclusion, no automatic trial activation, unchanged plain F12 cycle");
}
