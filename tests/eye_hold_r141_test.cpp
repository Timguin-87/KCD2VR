// R141 host test: the eye-image hold (xr_eye_hold_r141.h) used by XrManager::RunFrame when a frame wrote no eye
// image, and its Experiments switch (GEAR page 3, row 4, VrEyeHoldR141).
// g++ -std=c++20 -I src -I tests -I tests/syntax_shims -I tests/openxr_sdk tests/eye_hold_r141_test.cpp
#include "subsystems/experimental/live_settings_d39.h"
#include "xr_eye_hold_r141.h"
#include <cstdio>
#include <cwchar>
using namespace kcd2vr;
namespace live=live_settings_d39;namespace h=xr_eye_hold_r141;
static unsigned total=0,fail=0;
static void ck(bool b,const char* s){++total;if(!b){++fail;std::printf("FAIL %s\n",s);}}
struct MenuState { // portable view of VrMenuState (same as hud_infinity_r125_test)
    live::Settings live_d39{};bool quick_access_face_touch_r112=false;
    bool vrik_body_follow=true,smooth_move51=false,precise_contacts44=true,exp_wall_slide=false,grab_hand_stays43=true;
    int roomscale_mode=1,vr_turn_mode=1,body_height_mode98=2;
    bool activity_hands59c7f[4]{true,true,true,false};
    int alchemy_mode67=1,smithing_mode67=1,smith_anvil_mode_a3=1;
    bool craft_trial_h41=false,smith_auto_hold36=true,user_shoulder_holsters=true,chest_dagger54=true;
    int weapon_release_mode_t1=0;bool throwing_supply_t2=false,gesture_talk=false,native_kill_nf=false;
    int exp_hide_stowed_bow=0,physical_f9_rung61=0,physical_f9_next_h6=1;float clinch_distance32=.55f;
};
static XrPosef pose(float x){XrPosef p{};p.orientation.w=1;p.position.x=x;return p;}
static XrFovf fov(float a){return XrFovf{-a,a,a,-a};}
int main(){
    {   // 1. a swapchain ring of 3: frames 1..3 written, frame 4 copy failed -> image 0 (frame 1) re-shown with frame 1's pose
        h::Images L,R;XrCompositionLayerProjectionView v[2]{};
        for(std::uint32_t f=1;f<=3;++f){h::Record(L,f-1,0,pose(-.03f*f),fov(.9f),f,1000+11*f);h::Record(R,f-1,1,pose(.03f*f),fov(.8f),f,1000+11*f);}
        ck(h::Pick(L,0,R,0,false,1044,v)==h::Result::Held,"held");
        ck(v[0].pose.position.x==-.03f&&v[1].pose.position.x==.03f&&v[0].fov.angleRight==.9f&&v[1].fov.angleRight==.8f,"pose/fov of the pixels (frame 1), not of now");
        ck(h::Pick(L,1,R,1,false,1000+22+h::kHoldMs+1,v)==h::Result::TooOld,"bounded: older than 2 s = not held (black as before)");
        ck(h::Pick(L,2,R,2,false,1033+h::kHoldMs,v)==h::Result::Held,"exactly 2 s still held");
        h::Forget(L,0);ck(h::Pick(L,0,R,0,false,1044,v)==h::Result::Missing,"theater/map wrote image 0 -> never re-shown");
        ck(h::Pick(L,7,R,7,false,1044,v)==h::Result::Missing,"never written index");
        h::Images L2=L;h::Record(L2,1,0,pose(0),fov(1),99,1040);ck(h::Pick(L2,1,R,1,false,1044,v)==h::Result::Mismatch,"eyes from different frames -> no hold (never a mixed pair)");
        ck(h::Pick(L,1,R,1,true,1044,v)==h::Result::Mismatch,"eye swap changed since the picture -> no hold");
        h::Images A,B;h::Record(A,0,1,pose(1),fov(1),5,10);h::Record(B,0,0,pose(2),fov(1),5,10);
        ck(h::Pick(A,0,B,0,true,20,v)==h::Result::Held&&v[0].pose.position.x==2&&v[1].pose.position.x==1,"swapped eyes recorded swapped -> held with the same mapping");
        h::Images C,D;h::Record(C,0,0,pose(0),fov(1),1,500);h::Record(D,0,1,pose(0),fov(1),1,500);
        ck(h::Pick(C,0,D,0,false,400,v)==h::Result::TooOld,"clock went backwards -> no hold");
        h::Record(C,200,0,pose(0),fov(1),1,1);ck(C.size()==1,"bogus index ignored");
    }
    {   // 2. switch: default ON, GEAR page 3 row 4 toggles (never refused), label
        ck(h::enabled.load(),"default ON");
        MenuState s{};unsigned selected=unsigned(live::Group::Gear);
        auto click=[&](live::Rect r){return live::dispatch(selected,r.l+2,r.t+2,s,[](const MenuState&){return true;},[]{return true;});};
        ck(s.live_d39.eyeHoldR141,"settings default ON");
        ck(click(live::kMotionNextD69)==live::Click::Changed&&click(live::kMotionNextD69)==live::Click::Changed&&selected==live::kGearPage3R125,"GEAR 1 > 2 > 3");
        ck(live::rows(selected)[4].id==live::Id::EyeHoldR141,"row 5 = eye hold (merged R141)");
        wchar_t text[160]{};live::label(s,live::rows(selected)[4],text,160);ck(std::wcsstr(text,L"ON")!=nullptr,"label ON");
        ck(click(live::rowRect(4))==live::Click::Changed&&!s.live_d39.eyeHoldR141,"OFF");
        live::label(s,live::rows(selected)[4],text,160);ck(std::wcsstr(text,L"OFF")!=nullptr,"label OFF");
        ck(click(live::rowRect(4))==live::Click::Changed&&s.live_d39.eyeHoldR141,"ON again");
        ck(live::rows(selected)[0].id==live::Id::HudInfinityR125&&live::rows(selected)[2].id==live::Id::ImpaleFullDepthR128,"R125/R128 rows unchanged");
    }
    {   // 3. R153 Theater hold: a stalled Theater frame re-shows the left image's theater picture of the same scene
        h::Images L,R;XrCompositionLayerProjectionView v[2]{};
        for(std::uint32_t i=0;i<3;++i){h::Forget(L,i);h::Forget(R,i);h::RecordTheater(L,i,7,1000+11*i);} // 3 theater frames, scene 7
        ck(h::PickTheater(L,0,7,1040)==h::Result::Held,"theater stall -> last theater picture again (was: no layer = black)");
        ck(h::PickTheater(L,0,8,1040)==h::Result::Mismatch,"other scene (ticket changed) -> not held");
        ck(h::PickTheater(L,1,7,1011+h::kHoldMs+1)==h::Result::TooOld,"theater hold bounded by 2 s too");
        ck(h::PickTheater(L,2,7,1022+h::kHoldMs)==h::Result::Held,"exactly 2 s still held");
        ck(h::PickTheater(L,0,7,900)==h::Result::TooOld,"clock backwards -> no hold");
        ck(h::PickTheater(L,5,7,1040)==h::Result::Missing,"never written index");
        ck(h::Pick(L,0,R,0,false,1040,v)!=h::Result::Held&&h::Pick(L,0,L,0,false,1040,v)==h::Result::Mismatch,
           "a theater picture is never re-shown as a projection");
        h::Record(L,1,0,pose(0),fov(1),3,1030);
        ck(h::PickTheater(L,1,7,1040)==h::Result::Mismatch,"a projection picture is never re-shown on the theater quad");
        h::Forget(L,2);ck(h::PickTheater(L,2,7,1040)==h::Result::Missing,"physical map / written-but-cancelled frame forgot it");
        h::Images P,Q;h::Record(P,0,0,pose(1),fov(1),4,10);h::Record(Q,0,1,pose(2),fov(1),4,10);
        ck(h::Pick(P,0,Q,0,false,20,v)==h::Result::Held,"projection hold unchanged");
        h::RecordTheater(P,100,7,1);ck(P.size()==1,"bogus index ignored");
    }
    std::printf("eye_hold_r141_test %u/%u\n",total-fail,total);
    return fail?1:0;
}
