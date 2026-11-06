// Observable output from the actual Touch and native-pad adapters. No OS input.
#include <windows.h>
#include <vector>
#include <cstdio>
#include <cstdlib>
static std::vector<INPUT> eventsD37;
static UINT SpyD37(UINT n, INPUT* e, int) {
    eventsD37.insert(eventsD37.end(), e, e+n); return n;
}
#define SendInput SpyD37
#ifdef KCD2_SNAP_BASELINE
#include "../../../baseline/source/src/touch_bindings.cpp"
#include "../../../baseline/source/src/native_vr_pad_premap.cpp"
#else
#include "../src/touch_bindings.cpp"
#include "../src/native_vr_pad_premap.cpp"
#endif
#undef SendInput
namespace kcd2vr::diagnostic_trace { void Write(const char*) noexcept {} }
static unsigned checksD37=0,failuresD37=0;
static void checkD37(bool ok,const char* why) {
    ++checksD37;
    if(!ok){++failuresD37;std::fprintf(stderr,"FAIL %u: %s\n",checksD37,why);}
}
static unsigned pulsesD37(long dx) {
    unsigned n=0;
    for(const auto& e:eventsD37)
        if(e.type==INPUT_MOUSE && e.mi.dwFlags==MOUSEEVENTF_MOVE && e.mi.dx==dx && e.mi.dy==0)++n;
    return n;
}
int main() {
    using namespace kcd2vr;
    for(bool xi:{false,true}) {
        TouchBindings b; ControllerState l{},r{}; l.active=r.active=true;
        b.SetNativeGamepadActions(xi);b.SetNativeGamepadAxes(xi,xi);
        b.SetRightGripFreeLookEnabled(false);b.SetTurnMode(0);
        eventsD37.clear();hostTickCount64=1000;
        b.Update(l,r,false); // neutral arms the controller
        r.thumb_x=.9f;hostTickCount64+=11;b.Update(l,r,false);
        checkD37(pulsesD37(360)==1,"one right snap with native XI either ready or absent");
        for(int i=0;i<30;++i){hostTickCount64+=11;b.Update(l,r,false);}
        checkD37(pulsesD37(360)==1,"holding the stick never repeats snap");
        r.thumb_x=-.9f;hostTickCount64+=11;b.Update(l,r,false);
        checkD37(pulsesD37(-360)==0,"opposite deflection without neutral cannot retrigger");
        r.thumb_x=0;hostTickCount64+=11;b.Update(l,r,false);
        r.thumb_x=-.9f;hostTickCount64+=11;b.Update(l,r,false);
        checkD37(pulsesD37(-360)==1,"neutral rearms the opposite snap");
        b.Update(l,r,true); // menu owns this held deflection
        hostTickCount64+=11;b.Update(l,r,false);
        checkD37(pulsesD37(-360)==1,"closing a menu with stick held cannot snap");
        r.thumb_x=0;hostTickCount64+=11;b.Update(l,r,false);
        r.active=false;r.thumb_x=.9f;hostTickCount64+=11;b.Update(l,r,false);
        r.active=true;hostTickCount64+=11;b.Update(l,r,false);
        checkD37(pulsesD37(360)==1,"lost/reconnected right controller requires neutral");
        r.thumb_x=0;hostTickCount64+=11;b.Update(l,r,false);
        r.thumb_x=.9f;hostTickCount64+=11;b.Update(l,r,false);
        checkD37(pulsesD37(360)==2,"fresh neutral restores snap after tracking loss");
        r.thumb_x=0;hostTickCount64+=11;b.Update(l,r,false,false,false,true);
        r.thumb_x=.9f;hostTickCount64+=11;b.Update(l,r,false,false,false,true);
        checkD37(pulsesD37(360)==2,"dialogue/back context cannot emit a gameplay snap");
        hostTickCount64+=11;b.Update(l,r,false);
        checkD37(pulsesD37(360)==2,"leaving dialogue with stick held requires neutral");
        r.thumb_x=0;hostTickCount64+=11;b.Update(l,r,false);
        r.thumb_x=.9f;hostTickCount64+=11;b.Update(l,r,false);
        checkD37(pulsesD37(360)==3,"neutral restores snap after dialogue closes");
    }
    native_vr_pad_premap::Reset();
    ControllerState l{},r{};l.active=r.active=true;r.thumb_x=.9f;
    native_vr_pad_premap::Context c{};c.grip_weapon_comparison114=true;
#ifndef KCD2_SNAP_BASELINE
    c.snap_turn_d37=true;
#endif
    auto p=native_vr_pad_premap::Build(l,r,c);
    checkD37(p.rx==0,"snap has no simultaneous analog XI yaw");
    c.system_menu=true;p=native_vr_pad_premap::Build(l,r,c);
    checkD37(p.rx>0,"native menu right axis is unchanged");
    c.system_menu=false;c.semantic_minigame=true;p=native_vr_pad_premap::Build(l,r,c);
    checkD37(p.rx>0,"authored activity right axis is unchanged");
    c.semantic_minigame=false;
#ifndef KCD2_SNAP_BASELINE
    c.snap_turn_d37=false;
#endif
    p=native_vr_pad_premap::Build(l,r,c);
    checkD37(p.rx>0,"smooth mode retains analog native yaw");
    std::printf("D37 snap routing: %u checks, %u failures; Windows/game execution NOT_RUN\n",checksD37,failuresD37);
    return failuresD37?1:0;
}
