// R125 host test: Experiments GEAR page 3 (HUD at infinity + combat direction cross), the per-eye HUD offset math
// used by xr_manager (zero disparity), and the HUD.xml names of the cross art.
// g++ -std=c++20 -I src -I tests -I tests/syntax_shims -I tests/openxr_sdk tests/hud_infinity_r125_test.cpp
#include "subsystems/experimental/live_settings_d39.h"
#include "ui_surface_geometry.h"
#include "ui_hud_r112.h"
#include <cmath>
#include <cstdio>
#include <cwchar>
#include <fstream>
#include <iterator>
#include <string>
using namespace kcd2vr;
namespace live=live_settings_d39;namespace g=ui_surface_geometry;
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
int main(){
    {   // 1. GEAR 1 > 2 > 3, rows toggle (never refused), 3 < 2 < 1
        MenuState s{};unsigned selected=unsigned(live::Group::Gear);
        auto click=[&](live::Rect r){return live::dispatch(selected,r.l+2,r.t+2,s,[](const MenuState&){return true;},[]{return true;});};
        ck(!s.live_d39.hudInfinityR125&&s.live_d39.combatCrossR125,"defaults: HUD INF off (R126), cross shown (R160 UI = his INI)");
        ck(click(live::kMotionNextD69)==live::Click::Changed&&selected==live::kGearPage2R118,"GEAR 1 > 2");
        ck(click(live::kMotionNextD69)==live::Click::Changed&&selected==live::kGearPage3R125,"GEAR 2 > 3");
        ck(live::group(selected)==live::Group::Gear&&live::rows(selected)[0].id==live::Id::HudInfinityR125&&live::rows(selected)[1].id==live::Id::CombatCrossR125,"page 3 rows");
        ck(click(live::rowRect(0))==live::Click::Changed&&s.live_d39.hudInfinityR125,"INF on");
        ck(click(live::rowRect(0))==live::Click::Changed&&!s.live_d39.hudInfinityR125,"INF off again");
        ck(click(live::rowRect(1))==live::Click::Changed&&!s.live_d39.combatCrossR125,"cross hidden");
        ck(click(live::rowRect(1))==live::Click::Changed&&s.live_d39.combatCrossR125,"cross shown");
        wchar_t text[160]{};live::label(s,live::rows(selected)[1],text,160);ck(std::wcsstr(text,L"SHOWN")!=nullptr,"label SHOWN");
        ck(click(live::kMotionNextD69)==live::Click::Changed&&selected==live::kGearPage3R125,"GEAR 3 > stays 3");
        ck(click(live::kMotionPrevD69)==live::Click::Changed&&selected==live::kGearPage2R118,"GEAR 3 < 2");
        ck(click(live::kMotionPrevD69)==live::Click::Changed&&selected==unsigned(live::Group::Gear),"GEAR 2 < 1");
    }
    {   // 2. per-eye quad offset = zero disparity. eyes at +-IPD/2, quad centre point P at (x,y,-D) in head space.
        const float ipd=0.06923f,D=5.0f;const g::Vec P{0.4f,-0.3f,-D};
        const g::Vec eye[2]{{-ipd*.5f,0,0},{ipd*.5f,0,0}};
        auto dir=[](g::Vec a,g::Vec b){const g::Vec d{b.x-a.x,b.y-a.y,b.z-a.z};const float n=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z);return g::Vec{d.x/n,d.y/n,d.z/n};};
        const auto l1=dir(eye[0],P),r1=dir(eye[1],P); // R124: one quad for both eyes
        const float old=std::acos(std::fmin(1.f,l1.x*r1.x+l1.y*r1.y+l1.z*r1.z))*57.29578f;
        const auto l2=dir(eye[0],{P.x+eye[0].x,P.y+eye[0].y,P.z+eye[0].z}),r2=dir(eye[1],{P.x+eye[1].x,P.y+eye[1].y,P.z+eye[1].z});
        const float now=std::acos(std::fmin(1.f,l2.x*r2.x+l2.y*r2.y+l2.z*r2.z))*57.29578f;
        std::printf("disparity one quad at %.0f m = %.3f deg, per-eye quads = %.4f deg\n",double(D),double(old),double(now));
        ck(old>0.7f&&now<0.01f,"per-eye offset removes the disparity");
        ck(!g::hudInfinityR125.load(),"INF default off (R126)");
    }
    {   // 3. cross art names exist in the companion HUD.xml and in the runtime base XML; icons are not in the list
        auto read=[](const char* p){std::ifstream f(p,std::ios::binary);return std::string(std::istreambuf_iterator<char>(f),{});};
        const auto xml=read("companion/source/Libs/UI/UIElements/HUD.xml"),base=read("src/hud_xml_base.inc");
        for(const char* n:ui_hud_r112::kCombatCrossArtR125){
            const std::string key=std::string("name=\"")+n+"\" instancename=\"cccursor.actioncursor.CombatCursorCross.state"+std::string(n).substr(16)+"\"";
            ck(xml.find(key)!=std::string::npos,"companion HUD.xml has the cross art entry");
            ck(base.find(key)!=std::string::npos,"hud_xml_base.inc has the cross art entry");
            ck(std::string(n).find("lock")==std::string::npos&&std::string(n).find("pbslot")==std::string::npos,"icons are never hidden");
        }
        ck(ui_hud_r112::combatCrossR125.load(),"cross art shown by default (R160 UI = his INI)");
    }
    std::printf("hud_infinity_r125_test %u/%u\n",total-fail,total);
    return fail?1:0;
}
