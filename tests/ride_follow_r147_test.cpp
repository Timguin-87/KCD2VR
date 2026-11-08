// R147 host test: on the horse a short left X tap (PS VR2 Square) = one LB tap when the game lists the follow prompt
// (ride_follow_r147.h) in the REAL native pad premap, everything else unchanged, + the Experiments switch
// (GEAR page 3 row 8, VrRideFollowXR147).
// g++ -std=c++20 -D__fastcall= -D__cdecl= -D__stdcall= -D__thiscall= -I src -I tests -I tests/syntax_shims -I tests/openxr_sdk tests/ride_follow_r147_test.cpp src/native_vr_pad_premap.cpp
#include <windows.h>
#include "native_vr_pad_premap.h"
#include "controller_layout_r118.h"
#include "ride_follow_r147.h"
#include "subsystems/experimental/live_settings_d39.h"
#include <cstdio>
#include <cwchar>
#include <string_view>
using namespace kcd2vr;
namespace pm=native_vr_pad_premap;namespace rf=ride_follow_r147;namespace live=live_settings_d39;namespace cl=controller_layout_r118;
static unsigned total=0,fail=0;
static void ck(bool b,const char* s){++total;if(!b){++fail;std::printf("FAIL %s\n",s);}}
struct MenuState { // portable view of VrMenuState (same as eye_hold_r141_test)
    live::Settings live_d39{};bool quick_access_face_touch_r112=false;
    bool vrik_body_follow=true,smooth_move51=false,precise_contacts44=true,exp_wall_slide=false,grab_hand_stays43=true;
    int roomscale_mode=1,vr_turn_mode=1,body_height_mode98=2;
    bool activity_hands59c7f[4]{true,true,true,false};
    int alchemy_mode67=1,smithing_mode67=1,smith_anvil_mode_a3=1;
    bool craft_trial_h41=false,smith_auto_hold36=true,user_shoulder_holsters=true,chest_dagger54=true;
    int weapon_release_mode_t1=0;bool throwing_supply_t2=false,gesture_talk=false,native_kill_nf=false;
    int exp_hide_stowed_bow=0,physical_f9_rung61=0,physical_f9_next_h6=1;float clinch_distance32=.55f;
};
static int g_answer=-1;static unsigned g_asks=0,g_retires=0;
static int Query(bool want) noexcept {if(want)++g_asks;else ++g_retires;return want?g_answer:-1;}
struct Out {bool talk=false,lb=false,inventory=false,dpadUp=false,keptTalk=false;std::uint64_t torch=0;unsigned flag=0;};
static Out PressX(unsigned ms,bool mounted,bool withQuery=true,bool ui=false){
    ControllerState l{},r{};l.active=r.active=true;pm::Context c{};c.theater=ui;c.mounted_r147=mounted;
    c.follow_query_r147=withQuery?&Query:nullptr;Out o{};
    auto take=[&](const pm::State& p){o.talk|=p.x;o.lb|=p.lb;o.inventory|=p.inventoryRequestR118;o.dpadUp|=p.dpad_up;
        if(p.torchRequestTick62)o.torch=p.torchRequestTick62;if(p.rideFollowLbR147)o.flag=p.rideFollowLbR147;o.keptTalk|=p.rideFollowTalkR147;};
    take(pm::Build(l,r,c));
    l.x_down=true;
    for(unsigned t=0;t<=ms;t+=16){hostTickCount64+=16;take(pm::Build(l,r,c));}
    l.x_down=false;
    for(unsigned t=0;t<200;t+=16){hostTickCount64+=16;take(pm::Build(l,r,c));}
    return o;
}
int main(){
    {   // 1. the decision table
        ck(rf::SendLb(true,true,100,1)&&rf::SendLb(true,true,100,-1),"mounted tap: prompt listed or list unreadable = LB");
        ck(!rf::SendLb(true,true,100,0),"mounted tap, no follow prompt = old meaning (talk)");
        ck(!rf::SendLb(true,false,100,1),"not mounted = never (walking unchanged)");
        ck(!rf::SendLb(false,true,100,1),"switch OFF = never");
        ck(!rf::SendLb(true,true,rf::kTapMs,1)&&rf::SendLb(true,true,rf::kTapMs-1,1),"tap = shorter than 300 ms");
        ck(rf::Merge(-1,-1)==-1&&rf::Merge(-1,0)==0&&rf::Merge(0,1)==1&&rf::Merge(1,0)==1&&rf::Merge(0,-1)==0,"merge: listed wins, then not listed, then unknown");
        ck(rf::enabled.load()&&std::string_view(rf::kKey)=="VrRideFollowXR147","default ON, INI key");
    }
    pm::Reset();cl::Set(0);
    {   // 2. off the horse: exactly the old X (prompt state does not matter)
        g_answer=1;auto o=PressX(100,false);ck(o.talk&&!o.lb&&!o.flag&&!o.keptTalk,"walking X tap = Quick Speak, no LB even with Follow listed");
        g_asks=0;o=PressX(100,false);ck(g_asks==0,"walking: the prompt list is never asked");
        o=PressX(600,false);ck(o.torch&&!o.talk&&!o.lb,"walking X hold = torch");
    }
    {   // 3. on the horse
        g_answer=1;g_asks=0;auto o=PressX(100,true);ck(o.lb&&!o.talk&&o.flag==1&&g_asks>0,"mounted X tap + Follow listed = one LB tap, no talk");
        g_answer=0;o=PressX(100,true);ck(o.talk&&!o.lb&&!o.flag&&o.keptTalk,"mounted X tap, no Follow prompt = Quick Speak as before (+ one log line)");
        g_answer=-1;o=PressX(100,true);ck(o.lb&&!o.talk&&o.flag==2,"mounted, prompt list unreadable = LB (documented fallback)");
        o=PressX(100,true,false);ck(o.lb&&!o.talk&&o.flag==2,"mounted, no query (not native pad mode 3) = LB fallback");
        g_answer=1;o=PressX(400,true);ck(o.talk&&!o.lb,"mounted 0.4 s press (not a tap) = Quick Speak as before");
        o=PressX(600,true);ck(o.torch&&!o.talk&&!o.lb,"mounted X hold = torch unchanged (OPENXR)");
        cl::Set(1);o=PressX(600,true);ck(o.inventory&&!o.torch&&!o.lb,"STEAMVR: mounted X hold = inventory unchanged");
        o=PressX(100,true);ck(o.lb&&!o.talk,"STEAMVR: mounted X tap = LB");
        cl::Set(2);o=PressX(100,true);ck(o.lb&&!o.talk,"PS VR2: mounted Square tap = LB");
        cl::Set(0);
        g_answer=0;o=PressX(100,true);ck(o.talk&&!o.lb,"no latch: the next tap without Follow is talk again");
        g_answer=1;o=PressX(100,true,true,true);ck(!o.lb&&!o.flag,"menus/theater: no follow LB, X stays the native UI X");
        rf::enabled.store(false);o=PressX(100,true);ck(o.talk&&!o.lb,"switch OFF: mounted X tap = talk");
        rf::enabled.store(true);o=PressX(100,true);ck(o.lb&&!o.talk,"switch ON again (live, next press)");
    }
    {   // 4. LB pulse length = the talk pulse (120 ms), one press edge
        ControllerState l{},r{};l.active=r.active=true;pm::Context c{};c.mounted_r147=true;c.follow_query_r147=&Query;g_answer=1;
        pm::Build(l,r,c);l.x_down=true;for(int i=0;i<5;++i){hostTickCount64+=16;pm::Build(l,r,c);}
        l.x_down=false;unsigned frames=0,edges=0;bool prev=false;
        for(int i=0;i<30;++i){hostTickCount64+=16;const auto p=pm::Build(l,r,c);frames+=p.lb;edges+=(p.lb&&!prev);prev=p.lb;}
        ck(edges==1&&frames>=6&&frames<=9,"one LB press of ~120 ms");
    }
    {   // 5. switch: GEAR page 3 row 8, never refused, label
        MenuState s{};unsigned selected=unsigned(live::Group::Gear);
        auto click=[&](live::Rect r){return live::dispatch(selected,r.l+2,r.t+2,s,[](const MenuState&){return true;},[]{return true;});};
        ck(s.live_d39.rideFollowXR147,"settings default ON");
        ck(click(live::kMotionNextD69)==live::Click::Changed&&click(live::kMotionNextD69)==live::Click::Changed&&selected==live::kGearPage3R125,"GEAR 1 > 2 > 3");
        ck(live::rows(selected)[7].id==live::Id::RideFollowXR147,"row 8 = riding follow");
        wchar_t text[200]{};live::label(s,live::rows(selected)[7],text,200);ck(std::wcsstr(text,L"ON")!=nullptr,"label ON");
        ck(click(live::rowRect(7))==live::Click::Changed&&!s.live_d39.rideFollowXR147,"OFF");
        live::label(s,live::rows(selected)[7],text,200);ck(std::wcsstr(text,L"OFF")!=nullptr,"label OFF");
        ck(click(live::rowRect(7))==live::Click::Changed&&s.live_d39.rideFollowXR147,"ON again");
        ck(live::rows(selected)[4].id==live::Id::EyeHoldR141&&live::rows(selected)[6].id==live::Id::HideTutorialsR141,"rows 1-7 unchanged");
    }
    std::printf("ride_follow_r147_test %u/%u\n",total-fail,total);
    return fail?1:0;
}
