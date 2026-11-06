// R118 host test: controller layout table, the real native pad premap (X hold = torch vs inventory), the
// Experiments GEAR page 2 reducer and the default INI writer (never over an existing file).
// g++ -std=c++20 -I src -I tests -I tests/syntax_shims -I tests/openxr_sdk tests/controller_layout_r118_test.cpp src/native_vr_pad_premap.cpp
#include <windows.h>
#include "native_vr_pad_premap.h"
#include "controller_layout_r118.h"
#include "subsystems/experimental/live_settings_d39.h"
#include "core/default_ini_r118.h"
#include <cstdio>
#include <cwchar>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
using namespace kcd2vr;
namespace cl=controller_layout_r118;namespace pm=native_vr_pad_premap;namespace live=live_settings_d39;
static unsigned total=0,fail=0;
static void ck(bool b,const char* s){++total;if(!b){++fail;std::printf("FAIL %s\n",s);}}
struct MenuState { // portable view of VrMenuState (the reducer only touches these fields)
    live::Settings live_d39{};bool quick_access_face_touch_r112=false;
    bool vrik_body_follow=true,smooth_move51=false,precise_contacts44=true,exp_wall_slide=false,grab_hand_stays43=true;
    int roomscale_mode=1,vr_turn_mode=1,body_height_mode98=2;
    bool activity_hands59c7f[4]{true,true,true,false};
    int alchemy_mode67=1,smithing_mode67=1,smith_anvil_mode_a3=1;
    bool craft_trial_h41=false,smith_auto_hold36=true,user_shoulder_holsters=true,chest_dagger54=true;
    int weapon_release_mode_t1=0;bool throwing_supply_t2=false,gesture_talk=false,native_kill_nf=false;
    int exp_hide_stowed_bow=0,physical_f9_rung61=0,physical_f9_next_h6=1;float clinch_distance32=.55f;
};
struct Hold {bool inventory=false,talk=false,dpadUp=false;std::uint64_t torch=0;};
// Hold physical X for ms (16 ms steps) in gameplay, then release; collect what the pad published.
static Hold PressX(unsigned ms,bool ui=false){
    ControllerState l{},r{};l.active=r.active=true;pm::Context c{};c.theater=ui;Hold h{};
    pm::Build(l,r,c);
    l.x_down=true;
    for(unsigned t=0;t<=ms;t+=16){hostTickCount64+=16;const auto p=pm::Build(l,r,c);h.inventory|=p.inventoryRequestR118;h.talk|=p.x;h.dpadUp|=p.dpad_up;if(p.torchRequestTick62)h.torch=p.torchRequestTick62;}
    l.x_down=false;
    for(unsigned t=0;t<200;t+=16){hostTickCount64+=16;const auto p=pm::Build(l,r,c);h.inventory|=p.inventoryRequestR118;h.talk|=p.x;h.dpadUp|=p.dpad_up;if(p.torchRequestTick62)h.torch=p.torchRequestTick62;}
    return h;
}
int main(){
    // 1. layout table
    ck(!cl::XHoldOpensInventory(0)&&cl::XHoldLightsTorch(0),"OPENXR: X hold = torch");
    ck(cl::XHoldOpensInventory(1)&&!cl::XHoldLightsTorch(1),"STEAMVR: X hold = inventory");
    ck(cl::XHoldOpensInventory(2)&&!cl::XHoldLightsTorch(2),"PS VR2: Square hold = inventory");
    ck(cl::LeftMenuOpensInventory(0)&&cl::LeftMenuOpensInventory(1)&&cl::LeftMenuOpensInventory(2),"Left Menu stays inventory on every layout");
    ck(!cl::FaceTouchFor(0)&&!cl::FaceTouchFor(1)&&cl::FaceTouchFor(2),"only PS VR2 presets face touch");
    ck(cl::Next(0)==1&&cl::Next(1)==2&&cl::Next(2)==0&&cl::Clamp(-1)==0&&cl::Clamp(7)==0,"cycle + clamp of bad INI values");
    ck(std::wcscmp(cl::Name(1),L"STEAMVR")==0&&std::wcscmp(cl::Name(2),L"PS VR2")==0&&std::wcscmp(cl::Name(9),L"OPENXR")==0,"menu names");
    for(int i=0;i<3;++i)ck(std::wcslen(cl::SetupInfo(i))>20&&std::wcslen(cl::SetupLabel(i))>5,"setup text per layout");

    // 2. the real premap, quick-talk (companion pak) mode
    pm::Reset();cl::Set(0);
    auto h=PressX(600);ck(h.torch!=0&&!h.inventory&&!h.talk,"OPENXR: X 0.6 s = torch, no talk, no inventory");
    h=PressX(100);ck(!h.torch&&!h.inventory&&h.talk,"OPENXR: X tap = Quick Speak (xi_x pulse)");
    cl::Set(1);
    h=PressX(600);ck(h.inventory&&!h.torch&&!h.talk,"STEAMVR: X 0.6 s = inventory once, no torch, no talk");
    h=PressX(100);ck(!h.torch&&!h.inventory&&h.talk,"STEAMVR: X tap still Quick Speak");
    cl::Set(2);
    h=PressX(600);ck(h.inventory&&!h.torch&&!h.talk,"PS VR2: Square hold = inventory");
    {   // the press that opened the Inventory is not passed to the opened Inventory as xi_x
        pm::Reset();cl::Set(1);ControllerState l{},r{};l.active=r.active=true;pm::Context c{};l.x_down=true;bool inv=false;
        for(unsigned t=0;t<600;t+=16){hostTickCount64+=16;inv|=pm::Build(l,r,c).inventoryRequestR118;}
        c.theater=true;bool leaked=false;for(unsigned t=0;t<300;t+=16){hostTickCount64+=16;leaked|=pm::Build(l,r,c).x;}
        ck(inv&&!leaked,"held X after the inventory opened stays out of the inventory");
        l.x_down=false;hostTickCount64+=16;pm::Build(l,r,c);l.x_down=true;hostTickCount64+=16;
        ck(pm::Build(l,r,c).x,"a NEW X press inside the inventory reaches it normally");
    }
    {   // left thumbrest + X hold stays visor on every layout
        pm::Reset();cl::Set(1);ControllerState l{},r{};l.active=r.active=true;l.quick_access_touch=true;pm::Context c{};
        pm::Build(l,r,c);l.x_down=true;bool inv=false,up=false;
        for(unsigned t=0;t<700;t+=16){hostTickCount64+=16;const auto p=pm::Build(l,r,c);inv|=p.inventoryRequestR118;up|=p.dpad_up;}
        ck(!inv&&up,"STEAMVR: left modifier + X hold = visor, not inventory");
    }
    // 3. legacy mode (VrXQuickTalk=false): tap torch, hold visor (OPENXR) / inventory (STEAMVR)
    pm::SetXQuickTalk58(false);pm::Reset();cl::Set(0);
    h=PressX(900);ck(h.dpadUp&&!h.inventory&&!h.torch,"legacy OPENXR: X hold = visor (D-pad up)");
    cl::Set(1);h=PressX(900);ck(h.inventory&&!h.dpadUp&&!h.torch,"legacy STEAMVR: X hold = inventory, no visor");
    h=PressX(100);ck(h.torch&&!h.inventory,"legacy STEAMVR: X tap = torch");
    pm::SetXQuickTalk58(true);cl::Set(0);

    // 4. Experiments GEAR page 2 (same reducer the VR menu calls)
    {
        MenuState s{};unsigned selected=unsigned(live::Group::Gear);
        auto click=[&](live::Rect r){return live::dispatch(selected,r.l+2,r.t+2,s,[](const MenuState&){return true;},[]{return true;});};
        ck(click(live::kMotionNextD69)==live::Click::Changed&&selected==live::kGearPage2R118,"GEAR has a PAGE 2");
        ck(live::group(selected)==live::Group::Gear&&live::rows(selected)[0].id==live::Id::ControllerLayoutR118,"page 2 row 1 = controller layout");
        ck(click(live::rowRect(0))==live::Click::Changed&&s.live_d39.controllerLayoutR118==1&&!s.quick_access_face_touch_r112,"OPENXR -> STEAMVR, thumbrest");
        ck(click(live::rowRect(0))==live::Click::Changed&&s.live_d39.controllerLayoutR118==2&&s.quick_access_face_touch_r112,"STEAMVR -> PS VR2 sets face touch");
        ck(click(live::rowRect(1))==live::Click::Changed&&!s.quick_access_face_touch_r112,"modifier row still toggles (never refused)");
        ck(click(live::rowRect(0))==live::Click::Changed&&s.live_d39.controllerLayoutR118==0&&!s.quick_access_face_touch_r112,"PS VR2 -> OPENXR, thumbrest");
        wchar_t text[160]{};live::label(s,live::rows(selected)[0],text,160);ck(std::wcsstr(text,L"OPENXR")!=nullptr,"label shows the layout");
        ck(click(live::rowRect(3))==live::Click::None,"empty rows do nothing");
        ck(click(live::kMotionPrevD69)==live::Click::Changed&&selected==unsigned(live::Group::Gear),"back to GEAR page 1");
    }
    // 5. default INI: written only when missing
    {
        namespace di=core::default_ini_r118;namespace fs=std::filesystem;
        const auto dir=fs::temp_directory_path()/"kcd2vr_r118_ini_test";fs::remove_all(dir);fs::create_directories(dir);
        const auto ini=dir/"KCD2VR.ini";const std::string def="[Settings]\r\nVrControllerLayoutR118=0\r\n";
        auto read=[&]{std::ifstream f(ini,std::ios::binary);return std::string(std::istreambuf_iterator<char>(f),{});};
        ck(di::WriteIfMissing(ini,"")==di::Result::NoDefault&&!fs::exists(ini),"no embedded default = nothing written");
        ck(di::WriteIfMissing(ini,def)==di::Result::Created&&read()==def,"missing INI = default written");
        { std::ofstream f(ini,std::ios::binary|std::ios::trunc);f<<"[Settings]\r\nMine=1\r\n"; }
        ck(di::WriteIfMissing(ini,def)==di::Result::Exists&&read()=="[Settings]\r\nMine=1\r\n","existing INI never replaced");
        { std::ofstream f(ini,std::ios::binary|std::ios::trunc); }
        ck(di::WriteIfMissing(ini,def)==di::Result::Exists&&read().empty(),"existing EMPTY INI also never replaced");
        std::size_t files=0;for(auto& e:fs::directory_iterator(dir)){(void)e;++files;}
        ck(files==1,"no temp/backup files beside it");
        fs::remove_all(dir);
    }
    std::printf("controller_layout_r118_test %u/%u\n",total-fail,total);
    return fail?1:0;
}
