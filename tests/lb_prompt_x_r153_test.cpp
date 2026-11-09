// R153 host test: a listed infinite hold prompt (hold to look / watch) takes the left X press as a REAL held X in the
// real native pad premap (press, hold, release), no Quick Speak pulse / torch / inventory; not listed or unreadable =
// the old X exactly. decided per press (no latch). lb_prompt_x_r153.h. R155: a recent infinite_hold_interaction_* dispatch
// (OnAction detour stamp) counts as listed for 4 s.
// g++ -std=c++20 -D__fastcall= -D__cdecl= -D__stdcall= -D__thiscall= -I src -I tests -I tests/syntax_shims -I tests/openxr_sdk tests/lb_prompt_x_r153_test.cpp src/native_vr_pad_premap.cpp
#include <windows.h>
#include "native_vr_pad_premap.h"
#include "controller_layout_r118.h"
#include "ride_follow_r147.h"
#include "lb_prompt_x_r153.h"
#include <cstdio>
using namespace kcd2vr;
namespace pm=native_vr_pad_premap;namespace lx=lb_prompt_x_r153;namespace cl=controller_layout_r118;
static unsigned total=0,fail=0;
static void ck(bool b,const char* s){++total;if(!b){++fail;std::printf("FAIL %s\n",s);}}
static int g_lbx=-1,g_follow=-1;static unsigned g_lbxAsks=0,g_followRetires=0;static int g_lbxDelay=0,g_askFrames=0;
static int QueryLbx(bool want) noexcept {if(!want){g_askFrames=0;return -1;}++g_lbxAsks;return ++g_askFrames>g_lbxDelay?g_lbx:-1;}
static int QueryFollow(bool want) noexcept {if(!want)++g_followRetires;return want?g_follow:-1;}
struct Out {unsigned xFrames=0,xEdges=0,xAfterRelease=0,heldFrames=0;bool lb=false,inventory=false,dpadUp=false,pass=false;std::uint64_t torch=0;};
static Out PressX(unsigned ms,bool withQuery=true,bool mounted=false,bool ui=false){
    ControllerState l{},r{};l.active=r.active=true;pm::Context c{};c.theater=ui;c.mounted_r147=mounted;
    c.follow_query_r147=&QueryFollow;c.lb_prompt_query_r153=withQuery?&QueryLbx:nullptr;Out o{};bool prev=false;
    auto take=[&](const pm::State& p,bool down){o.xFrames+=p.x;o.xEdges+=(p.x&&!prev);prev=p.x;if(!down)o.xAfterRelease+=p.x;else o.heldFrames+=p.x;
        o.lb|=p.lb;o.inventory|=p.inventoryRequestR118;o.dpadUp|=p.dpad_up;o.pass|=p.lbPromptXR153;if(p.torchRequestTick62)o.torch=p.torchRequestTick62;};
    take(pm::Build(l,r,c),false);
    l.x_down=true;
    for(unsigned t=0;t<=ms;t+=16){hostTickCount64+=16;take(pm::Build(l,r,c),true);}
    l.x_down=false;
    for(unsigned t=0;t<300;t+=16){hostTickCount64+=16;take(pm::Build(l,r,c),false);}
    return o;
}
int main(){
    using R=lx::Route;
    {   // 1. decision table
        ck(lx::Next(R::Idle,true,true,0,1)==R::Pass,"new press + listed = pass");
        ck(lx::Next(R::Idle,true,true,0,-1)==R::Asking,"new press, no answer yet = asking");
        ck(lx::Next(R::Asking,true,false,100,1)==R::Pass,"answer inside 150 ms = pass");
        ck(lx::Next(R::Asking,true,false,100,0)==R::Asking&&lx::Next(R::Asking,true,false,lx::kDecideMs,0)==R::Normal,"not listed = old X after 150 ms");
        ck(lx::Next(R::Asking,true,false,lx::kDecideMs,1)==R::Normal,"listed too late = old X (nothing half-sent)");
        ck(lx::Next(R::Normal,true,false,200,1)==R::Normal,"a press already on the old path stays there");
        ck(lx::Next(R::Pass,false,false,0,-1)==R::Idle&&lx::Next(R::Normal,false,false,0,1)==R::Idle,"release = idle (no latch)");
        ck(lx::kDecideMs<ride_follow_r147::kTapMs&&lx::kDecideMs<500,"decided before the R147 tap and before torch/inventory");
        ck(lx::Matches("infinite_hold_interaction_init_press")&&lx::Matches("infinite_hold_interaction_inprogress_release"),"both infinite hold actions match");
        ck(!lx::Matches("focus_follow_init_interactor_press")&&!lx::Matches("infinite_hold")&&!lx::Matches(nullptr)&&!lx::Matches(""),"other rows do not match");
        // R155: second source = a recent infinite_hold_interaction_* dispatch seen by the OnAction detour
        ck(lx::Listed(1,10000,0)==1&&lx::Listed(0,10000,0)==0&&lx::Listed(-1,10000,0)==-1,"no dispatch seen: the hint row answer as before");
        ck(lx::Listed(0,10000,9000)==1&&lx::Listed(-1,10000,6000)==1,"dispatch within 4 s upgrades not listed / unknown to listed");
        ck(lx::Listed(0,10000,5999)==0&&lx::Listed(-1,10000,1)==-1,"older than 4 s: no effect (no latch)");
        ck(lx::Listed(0,5000,9000)==0&&lx::Listed(1,5000,9000)==1,"clock behind the stamp: ignored, rows still count");
        // R156: the hold-init action map read enabled within 300 ms = listed before any tap
        ck(lx::Listed(0,10000,0,9800)==1&&lx::Listed(-1,10000,0,9700)==1,"map enabled 200-300 ms ago upgrades not listed / unknown to listed");
        ck(lx::Listed(0,10000,0,9699)==0&&lx::Listed(-1,10000,0,1)==-1&&lx::Listed(0,10000,0,0)==0,"map not read enabled for 301 ms+ / never: no effect (no latch)");
        ck(lx::Listed(0,5000,0,9000)==0&&lx::Listed(1,5000,0,9000)==1,"clock behind the enabled stamp: ignored, rows still count");
        ck(lx::kMapSeenMs==4000,"4 s memory");
    }
    pm::Reset();cl::Set(1); // his INI: VrControllerLayoutR118=1 (STEAMVR, X hold = inventory)
    {   // 2. prompt listed: X is held through
        g_lbx=1;g_lbxDelay=1;auto o=PressX(2000);
        ck(o.pass&&o.xEdges==1&&o.heldFrames>=120&&o.xAfterRelease==0,"listed: X down for the whole 2 s hold, up with the button");
        ck(!o.inventory&&!o.torch&&!o.lb&&!o.dpadUp,"listed: no inventory, torch, LB or visor");
        o=PressX(60);ck(o.pass&&o.xEdges==1&&o.xAfterRelease==0,"listed: short tap = one real X press+release, no extra pulse");
        cl::Set(0);o=PressX(800);ck(o.pass&&!o.torch&&o.xAfterRelease==0,"OPENXR layout: listed hold = no torch");
        cl::Set(2);o=PressX(800);ck(o.pass&&!o.inventory,"PS VR2: listed Square hold = no inventory");
        cl::Set(1);
        g_lbxDelay=3;o=PressX(800);ck(o.pass&&!o.inventory&&o.xAfterRelease==0,"answer arrives 3 frames late: still pass");
        g_lbxDelay=1;g_followRetires=0;o=PressX(100,true,true);ck(o.pass&&!o.lb&&g_followRetires>0,"mounted + listed: held X, the R147 ask is retired, no LB");
    }
    {   // 3. not listed / unreadable: exactly the old X
        g_lbx=0;auto o=PressX(100);ck(!o.pass&&o.xAfterRelease>=6&&o.heldFrames==0,"not listed: tap = Quick Speak pulse after release (old)");
        o=PressX(700);ck(!o.pass&&o.inventory&&o.xFrames==0,"not listed: hold = inventory (old)");
        g_lbx=-1;o=PressX(100);ck(!o.pass&&o.xAfterRelease>=6,"unreadable: old tap");
        o=PressX(700);ck(!o.pass&&o.inventory,"unreadable: old hold");
        g_lbx=1;o=PressX(100,false);ck(!o.pass&&o.xAfterRelease>=6,"no query (not native pad mode 3): old tap");
        g_lbx=1;g_lbxDelay=20;o=PressX(700);ck(!o.pass&&o.inventory,"answer later than 150 ms: old hold, never both");
        g_lbxDelay=1;g_lbx=0;g_follow=1;o=PressX(100,true,true);ck(!o.pass&&o.lb,"mounted, follow listed, no hold prompt: R147 LB unchanged");
        g_follow=-1;
    }
    {   // 3b. R155: the prompt is not a hint row, but the game dispatched its action for the previous X tap
        g_lbx=0;g_lbxDelay=1;lx::holdMapSeenMs.store(hostTickCount64);
        auto o=PressX(2000);ck(o.pass&&o.xEdges==1&&o.heldFrames>=120&&o.xAfterRelease==0&&!o.inventory,"dispatch just seen + rows say no: X held through");
        o=PressX(100);ck(o.pass&&o.xAfterRelease==0,"second press within 4 s (hostTick moved ~2.6 s): still held through");
        hostTickCount64+=4100;o=PressX(100);ck(!o.pass&&o.xAfterRelease>=6,"4 s without a dispatch: old Quick Speak tap again (no latch)");
        g_lbx=-1;lx::holdMapSeenMs.store(hostTickCount64);o=PressX(100);ck(o.pass,"rows unreadable + dispatch seen: held through");
        lx::holdMapSeenMs.store(hostTickCount64);o=PressX(100,false);ck(!o.pass&&o.xAfterRelease>=6,"no query function (not native pad mode 3): the second source is not used");
        lx::holdMapSeenMs.store(0);g_lbx=0;o=PressX(100);ck(!o.pass&&o.xAfterRelease>=6,"stamp cleared: old tap");
    }
    {   // 3c. R156: the hold-init action map is enabled NOW (view lens) - the FIRST press holds, no wasted tap
        g_lbx=0;g_lbxDelay=1;lx::holdMapSeenMs.store(0);lx::holdMapEnabledMs.store(hostTickCount64);
        auto o=PressX(2000);ck(o.pass&&o.xEdges==1&&o.heldFrames>=120&&o.xAfterRelease==0&&!o.inventory&&!o.lb,"map enabled at the press + rows say no + no dispatch yet: X held through at once");
        o=PressX(100);ck(!o.pass&&o.xAfterRelease>=6,"next press 2.6 s later without a fresh enabled read: old Quick Speak tap (300 ms memory, no latch)");
        g_lbx=-1;lx::holdMapEnabledMs.store(hostTickCount64);o=PressX(100);ck(o.pass,"rows unreadable + map enabled: held through");
        lx::holdMapEnabledMs.store(hostTickCount64);o=PressX(100,false);ck(!o.pass&&o.xAfterRelease>=6,"no query function (not native pad mode 3): the third source is not used either");
        lx::holdMapEnabledMs.store(0);g_lbx=0;o=PressX(100);ck(!o.pass&&o.xAfterRelease>=6,"stamp cleared: old tap");
    }
    {   // 4. no latch, menus untouched
        g_lbx=1;auto o=PressX(100);ck(o.pass,"listed again");
        g_lbx=0;o=PressX(100);ck(!o.pass&&o.xAfterRelease>=6,"next press without the prompt = old Quick Speak again");
        g_lbx=1;g_lbxAsks=0;o=PressX(100,true,false,true);ck(!o.pass&&g_lbxAsks==0,"menus/theater: never asked, native UI X as before");
        g_lbx=1;g_lbxAsks=0;pm::Build({},{},{});unsigned before=g_lbxAsks;
        ControllerState l{},r{};l.active=r.active=true;pm::Context c{};c.lb_prompt_query_r153=&QueryLbx;
        for(int i=0;i<20;++i){hostTickCount64+=16;pm::Build(l,r,c);}
        ck(g_lbxAsks==before,"no X pressed: the prompt list is never asked");
    }
    std::printf("lb_prompt_x_r153_test %u/%u\n",total-fail,total);
    return fail?1:0;
}
