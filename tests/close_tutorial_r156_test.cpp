// R156 host test: while the game's close_tutorial action map is enabled (a full-screen tutorial screen is open) the
// right B (PS Circle) is sent as a REAL held xi_back in the real native pad premap (press, hold, release follow the
// button - the game makes its own 0.25 s hold-to-close from the held state); map not enabled = nothing changes.
// close_tutorial_r156.h. decided per frame from a 300 ms fresh tick (no latch).
// g++ -std=c++20 -D__fastcall= -D__cdecl= -D__stdcall= -D__thiscall= -I src -I tests -I tests/syntax_shims -I tests/openxr_sdk tests/close_tutorial_r156_test.cpp src/native_vr_pad_premap.cpp
#include <windows.h>
#include "native_vr_pad_premap.h"
#include "controller_layout_r118.h"
#include "close_tutorial_r156.h"
#include <cstdio>
using namespace kcd2vr;
namespace pm=native_vr_pad_premap;namespace ct=close_tutorial_r156;namespace cl=controller_layout_r118;
static unsigned total=0,fail=0;
static void ck(bool b,const char* s){++total;if(!b){++fail;std::printf("FAIL %s\n",s);}}
struct Out {unsigned backFrames=0,backEdges=0,backAfterRelease=0,heldFrames=0,bFrames=0,rbFrames=0,flagFrames=0;};
// one physical right-B press of ms, then 300 ms released; open = the mod reads the map enabled (fresh tick each frame)
static Out PressB(unsigned ms,bool open,pm::Context c=pm::Context{}){
    ControllerState l{},r{};l.active=r.active=true;Out o{};bool prev=false;
    auto frame=[&](bool down){hostTickCount64+=16;
        c.close_tutorial_r156=ct::Open(hostTickCount64,open?hostTickCount64:0);
        const auto p=pm::Build(l,r,c);
        o.backFrames+=p.back;o.backEdges+=(p.back&&!prev);prev=p.back;if(down)o.heldFrames+=p.back;else o.backAfterRelease+=p.back;
        o.bFrames+=p.b;o.rbFrames+=p.rb;o.flagFrames+=p.closeTutorialBackR156;};
    frame(false);
    r.b_down=true;for(unsigned t=0;t<=ms;t+=16)frame(true);
    r.b_down=false;for(unsigned t=0;t<300;t+=16)frame(false);
    return o;
}
int main(){
    {   // 1. pure decisions
        ck(ct::Open(10000,10000)&&ct::Open(10000,9700),"map read enabled within 300 ms = screen open");
        ck(!ct::Open(10000,9699)&&!ct::Open(10000,0),"older tick or never read = closed (no latch)");
        ck(!ct::Open(5000,9000),"clock behind the stamp: closed");
        ck(ct::Back(true,true)&&!ct::Back(true,false)&&!ct::Back(false,true),"xi_back only while open AND B is down");
        ck(ct::kEnabledMs==300,"300 ms freshness");
        ck(ct::kMapName[0]=='c'&&ct::kMapName[5]=='_'&&ct::kMapName[14]==0,"exact map name close_tutorial");
    }
    pm::Reset();cl::Set(0); // his INI: VrControllerLayoutR118=0 (OPENXR / Quest Touch)
    {   // 2. screen open: B is a held xi_back in gameplay (press, 2 s hold, release with the button)
        auto o=PressB(2000,true);
        ck(o.backEdges==1&&o.heldFrames>=120&&o.backAfterRelease==0,"open: xi_back down for the whole hold, up with the button");
        ck(o.flagFrames==o.backFrames&&o.bFrames==0&&o.rbFrames==0,"open: no xi_b / RB from this B, the receipt flag mirrors xi_back");
        o=PressB(60,true);ck(o.backEdges==1&&o.backAfterRelease==0&&o.heldFrames>=4,"open: short tap = one real press+release");
        cl::Set(2);o=PressB(800,true);ck(o.backEdges==1&&o.backAfterRelease==0,"PS VR2 layout: Circle hold = xi_back too");cl::Set(0);
        pm::SetBQuickTalk45(true);o=PressB(800,true);ck(o.rbFrames==0&&o.backEdges==1,"VrBQuickTalk=1: B closes the screen, no Quick Speak RB");
        o=PressB(300,false);ck(o.rbFrames>0&&o.backFrames==0,"VrBQuickTalk=1, screen closed: RB as before");pm::SetBQuickTalk45(false);
    }
    {   // 3. screen closed: exactly the old gameplay B (nothing published)
        auto o=PressB(2000,false);ck(o.backFrames==0&&o.bFrames==0&&o.rbFrames==0&&o.flagFrames==0,"closed: B publishes nothing (as before)");
        pm::Context c{};c.semantic_minigame=true;o=PressB(500,false,c);ck(o.backFrames==0,"closed, authored minigame: left Menu not pressed = no xi_back");
    }
    {   // 4. open inside other contexts, no latch
        pm::Context c{};c.semantic_minigame=true;auto o=PressB(500,true,c);ck(o.backEdges==1&&o.backAfterRelease==0,"authored minigame + open: B = xi_back as well");
        c=pm::Context{};c.theater=true;o=PressB(500,true,c);ck(o.backEdges==1&&o.backAfterRelease==0,"menu/theater + open: B = xi_back (and its native B as before)");
        c=pm::Context{};c.vr_menu=true;o=PressB(500,true,c);ck(o.backFrames==0,"VR menu open: nothing goes to the game");
        // the map goes disabled while B is still held: xi_back ends within 300 ms, B stays down
        ControllerState l{},r{};l.active=r.active=true;c=pm::Context{};unsigned after=0;std::uint64_t enabledTick=0;
        r.b_down=true;for(int i=0;i<10;++i){hostTickCount64+=16;enabledTick=hostTickCount64;c.close_tutorial_r156=ct::Open(hostTickCount64,enabledTick);(void)pm::Build(l,r,c);}
        for(int i=0;i<40;++i){hostTickCount64+=16;c.close_tutorial_r156=ct::Open(hostTickCount64,enabledTick);if(pm::Build(l,r,c).back)++after;}
        ck(after>0&&after<=20,"screen closed under a held B: xi_back released within 300 ms, no latch");
        r.b_down=false;hostTickCount64+=16;c.close_tutorial_r156=false;(void)pm::Build(l,r,c);
        o=PressB(300,false);ck(o.backFrames==0,"next press with the screen closed: nothing (no latch)");
        o=PressB(300,true);ck(o.backEdges==1,"next press with a screen open: xi_back again");
    }
    std::printf("close_tutorial_r156_test %u/%u\n",total-fail,total);
    return fail?1:0;
}
