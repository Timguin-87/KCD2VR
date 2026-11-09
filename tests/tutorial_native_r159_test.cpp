// R159 host test: "Tutorials: native hits only" (VrTutorialNativeHitsR159, tutorial_native_r159.h) - the ONE gate of the
// mod's direct hit route in the geometry contact drain (geometry_hit178r27.inl, drain_admission/rendezvous/live_delivery.inl)
// and its Experiments row (VRIK page 2 row 5, replaces the repeated "Loose scenery props" row that stays on OBJECTS page 3).
// g++ -std=c++20 -I src -I src/vrik tests/tutorial_native_r159_test.cpp -o tutorial_native_r159_test && ./tutorial_native_r159_test
#include "subsystems/direct/tutorial_native_r159.h"
#include "subsystems/experimental/live_settings_d39.h"
#include <cstdio>
#include <cstring>
#include <cwchar>
using namespace kcd2vr;
namespace T=tutorial_native_r159;namespace fb=npc_force_block153;namespace live=live_settings_d39;
// portable view of VrMenuState (same fields as minimal_hud_r141_test.cpp)
struct VrMenuState {
    live::Settings live_d39{};bool quick_access_face_touch_r112=false;
    bool vrik_body_follow=true,smooth_move51=false,precise_contacts44=true,exp_wall_slide=false,grab_hand_stays43=true;
    int roomscale_mode=1,vr_turn_mode=1,body_height_mode98=2;
    bool activity_hands59c7f[4]{true,true,true,false};
    int alchemy_mode67=1,smithing_mode67=1,smith_anvil_mode_a3=1;
    bool craft_trial_h41=false,smith_auto_hold36=true,user_shoulder_holsters=true,chest_dagger54=true;
    int weapon_release_mode_t1=0;bool throwing_supply_t2=false,gesture_talk=false,native_kill_nf=false;
    int exp_hide_stowed_bow=0,physical_f9_rung61=0,physical_f9_next_h6=1;float clinch_distance32=.55f;
    int comfort_vignette51=0,vr_pad_glyph_style=2,dialogue_camera_mode=0,cyclopean_mode=0,impale_hang=0,impale_hang_on45=3;
    bool contextual_hud=true,fast_block_r92=false,show_block_indicator_h24=false,native_activity_controls163=false;
    bool bloody_fights49=false,decapitation=false,dismemberment_d80=false,hmd_body_height54=false,native_crouch_r93=false;
    float npc_difficulty_percent=0,npc_defense_speed=1.3f;
};
static unsigned total=0,fail=0;
static void ck(bool b,const char* s){++total;if(!b){++fail;std::printf("FAIL %s\n",s);}}
// the drain's own use, written the same way as geometry_hit178r27.inl: decide per swing, then every direct branch is "&&ok"
struct Route {bool direct=false,live=false;};
static Route deliver(bool ok,bool overdue,bool intentDirect,bool strikeWindowOpen){
    Route r{};
    if(ok&&(overdue||intentDirect)&&!strikeWindowOpen){r.direct=true;return r;}
    if(strikeWindowOpen)r.live=true; // HYBRID_NATIVE_LIVE (C5235C, the game's own live attack action)
    return r;}
int main(){
    {   // 1. the gate table: only switch ON + tutorial turns the direct route off
        ck(T::directRouteAllowed(false,false)&&T::directRouteAllowed(false,true)&&T::directRouteAllowed(true,false),"switch off or no tutorial = direct route as set");
        ck(!T::directRouteAllowed(true,true),"switch on + tutorial = no direct route");
    }
    {   // 2. tutorial = practice weapon published fresh (R156 reader), stale after 1 s, no stamp = no tutorial
        ck(!T::tutorialActive(false,1000,1000),"real weapon");
        ck(T::tutorialActive(true,1000,1000)&&T::tutorialActive(true,1000,2000),"practice weapon, fresh (<= 1 s)");
        ck(!T::tutorialActive(true,1000,2001),"stale snapshot (combat off, no refresh) = not a tutorial");
        ck(!T::tutorialActive(true,0,5),"never published");
        ck(!T::tutorialActive(true,2000,1000),"clock before the stamp");
        fb::practiceWeapon.store(true);fb::practiceWeaponMs.store(5000);
        ck(T::mode.load()==0&&T::directRouteAllowedNow(5100)&&!T::nativeTimingOnlyNow(5100),"R162 runtime default = OFF: wooden sword changes nothing");
        T::mode.store(2);ck(T::directRouteAllowedNow(5100)&&T::nativeTimingOnlyNow(5100),"GAME TIMING + wooden sword: direct route as OFF, game's own attack speed");
        T::mode.store(1);ck(!T::directRouteAllowedNow(5100)&&!T::nativeTimingOnlyNow(5100),"NATIVE HITS + wooden sword = native only, mod timing as set");
        T::mode.store(0);ck(T::directRouteAllowedNow(5100)&&!T::nativeTimingOnlyNow(5100),"OFF = direct and mod timing as before (never refused)");
        T::mode.store(2);fb::practiceWeapon.store(false);ck(T::directRouteAllowedNow(5100)&&!T::nativeTimingOnlyNow(5100),"real sword = nothing changes in any mode");
        T::mode.store(1);ck(T::directRouteAllowedNow(5100),"real sword + NATIVE HITS = direct as before");
        fb::practiceWeapon.store(true);ck(T::directRouteAllowedNow(6200),"stale stamp = direct as before");
        T::mode.store(2);ck(!T::nativeTimingOnlyNow(6200),"stale stamp = mod timing as before");
        T::mode.store(7);ck(T::nativeTimingOnlyNow(5100)&&T::directRouteAllowedNow(5100),"out-of-range mode clamps to GAME TIMING");
        T::mode.store(-3);ck(!T::nativeTimingOnlyNow(5100)&&T::directRouteAllowedNow(5100),"negative mode clamps to OFF");
        T::mode.store(0); // back to the R162 default
    }
    {   // 3. once per swing: a change in the middle of a swing keeps that swing's route (no lost / doubled hit)
        T::SwingRoute s{};
        ck(!s.decide(11,false),"swing 11 first contact in a tutorial: native only");
        ck(!s.decide(11,true),"swing 11 later contact after the sword was stowed: still native (same swing)");
        ck(s.decide(12,true),"swing 12: new decision, direct allowed");
        ck(s.decide(12,false),"swing 12 keeps direct when the tutorial starts mid-swing");
        ck(!s.decide(13,false)&&s.decide(14,true)&&s.decide(15,true)&&s.decide(16,true),"four more swings");
        ck(!s.decide(0,false)&&s.decide(0,true),"no gesture id = decided per contact");
        ck(s.decide(11,true),"swing 11 aged out of the 4-entry memory = decided again (no latch)");
        // the drain: a contact in phase 4 (window not open) that would go direct
        ck(deliver(true,true,false,false).direct,"outside tutorial: overdue contact goes direct (unchanged)");
        const auto r=deliver(false,true,true,false);ck(!r.direct&&!r.live,"tutorial: overdue / intent contact does NOT go direct, it waits for the live window");
        ck(deliver(false,true,true,true).live&&!deliver(false,true,true,true).direct,"tutorial: contact in the strike window = live native hit");
    }
    {   // 4. one log row per edge, first OFF is not an edge
        T::Edge e{};ck(e.step(false)==-1,"start OFF: no row");ck(e.step(false)==-1,"still OFF: no row");
        ck(e.step(true)==1,"on row");ck(e.step(true)==-1,"no repeat");ck(e.step(false)==0,"off row");ck(e.step(false)==-1,"no repeat");
        T::Edge f{};ck(f.step(true)==1,"start ON = on row");
    }
    {   // 5. Experiments: VRIK page 2 row 5, default GAME TIMING (R161), cycles OFF / NATIVE HITS / GAME TIMING, never refused
        VrMenuState s{};ck(s.live_d39.tutorialNativeHitsR159==0,"Settings default OFF (R162) = the runtime default");
        s.live_d39.tutorialNativeHitsR159=1; // the old ON: the cycle below goes NATIVE HITS -> GAME TIMING -> OFF -> NATIVE HITS
        unsigned selected=live::kHandsPage2D71;
        ck(live::group(selected)==live::Group::Hands,"page 2 of the VRIK tab");
        const auto& rows=live::rows(selected);
        ck(rows[4].id==live::Id::TutorialNativeHitsR159&&!rows[4].readOnly&&!rows[4].session,"row 5 = the switch, saved");
        wchar_t text[200]{};live::label(s,rows[4],text,200);ck(std::wcsstr(text,L"NATIVE HITS")!=nullptr,"label NATIVE HITS");
        const auto r=live::rowRect(4);
        const auto click=[&]{return live::dispatch(selected,r.l+2,r.t+2,s,[](const VrMenuState&){return true;},[]{return true;});};
        ck(click()==live::Click::Changed&&s.live_d39.tutorialNativeHitsR159==2,"-> GAME TIMING (never refused)");
        live::label(s,rows[4],text,200);ck(std::wcsstr(text,L"GAME TIMING")!=nullptr,"label GAME TIMING");
        ck(click()==live::Click::Changed&&s.live_d39.tutorialNativeHitsR159==0,"-> OFF");
        live::label(s,rows[4],text,200);ck(std::wcscmp(text,L"OFF")==0||std::wcsstr(text,L"OFF")!=nullptr,"label OFF");
        ck(click()==live::Click::Changed&&s.live_d39.tutorialNativeHitsR159==1,"-> NATIVE HITS again");
        s.live_d39.tutorialNativeHitsR159=9;ck(click()==live::Click::Changed&&s.live_d39.tutorialNativeHitsR159==0,"bad stored value clamps, then cycles");
        bool swapStill=false;for(const auto& row:live::rows(live::kObjectsPage3R158))swapStill=swapStill||row.id==live::Id::SceneryPropSwapR100;
        ck(swapStill,"Loose scenery props still has its row (OBJECTS page 3)");
        unsigned ids=0;for(unsigned p=0;p<=live::kObjectsPage3R158;++p)for(const auto& row:live::rows(p))ids+=row.id==live::Id::TutorialNativeHitsR159;
        ck(ids==1,"exactly one row");
        ck(live::layoutValid(),"layout valid");
    }
    {   // 6. R161 INI: numbers 0..2, the old bool spelling, missing = 2
        ck(T::modeFromIniText("2",2)==2&&T::modeFromIniText("1",1)==1&&T::modeFromIniText("0",0)==0,"numbers");
        ck(T::modeFromIniText("true",2)==1&&T::modeFromIniText(" TRUE ",2)==1&&T::modeFromIniText("yes",2)==1&&T::modeFromIniText("on",2)==1,"old true = NATIVE HITS");
        ck(T::modeFromIniText("false",2)==0&&T::modeFromIniText("False",2)==0&&T::modeFromIniText("off",2)==0,"old false = OFF");
        ck(T::modeFromIniText("",0)==0&&T::modeFromIniText(nullptr,0)==0,"missing = the INI-int default (R162: 0 OFF)");
        ck(T::modeFromIniText("2",2)==2,"his INI 2 (ReadIniInt reads 2) stays GAME TIMING");
        ck(T::modeFromIniText("5",5)==2&&T::modeFromIniText("trueish",2)==2,"out of range clamps, junk = the number read");
        ck(T::nextMode(0)==1&&T::nextMode(1)==2&&T::nextMode(2)==0&&T::nextMode(9)==0,"cycle");
        ck(std::strcmp(T::modeName(2),"GAME_TIMING")==0&&std::strcmp(T::modeName(1),"NATIVE_HITS")==0&&std::strcmp(T::modeName(0),"OFF")==0,"names");
    }
    {   // 7. R161 GAME TIMING decided once per swing at its start (no latch, no change half way)
        T::SwingTiming t{};
        ck(t.decideAtStart(100,true)&&t.decided(100,false),"swing 100 started in the tutorial: native speed for all its frames, even after the sword is stowed");
        ck(!t.decideAtStart(101,false)&&!t.decided(101,true),"swing 101 started outside: mod timing for all its frames");
        ck(t.decided(102,true)&&!t.decided(103,false),"clock frame without a seen start = decided there once");
        ck(t.decided(102,false),"... and kept for that swing");
        ck(!t.decideAtStart(100,false),"a new start with the same key re-decides (no latch)");
        ck(t.decideAtStart(0,true)&&!t.decided(0,false),"no key = decided per call");
        T::TimingEdge e{};ck(e.step(false)==-1,"first OFF: no row");ck(e.step(true)==1,"on row");ck(e.step(true)==-1,"no repeat");
        ck(e.step(false)==0,"off row");ck(e.step(false)==-1,"no repeat");
    }
    std::printf("tutorial_native_r159_test: %u/%u passed\n",total-fail,total);
    return fail?1:0;
}
