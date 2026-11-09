// R158 host test: VrSceneryBigPropsR158 "Scenery props: size" (brush_swap_r100.h + static_grab_r89.h) and its
// Experiments row (OBJECTS page 3). default = the old 65 cm filter.
// g++ -std=c++20 -I src -I src/vrik tests/scenery_big_props_r158_test.cpp -o scenery_big_props_r158_test && ./scenery_big_props_r158_test
#include "vrik/static_grab_r89.h"
#include "subsystems/experimental/live_settings_d39.h"
#include "vrik/self_collision_r103.h"
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <limits>
using namespace kcd2vr;
namespace B=brush_swap_r100;namespace G=static_grab_r89;namespace live=live_settings_d39;
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
static bool near(double a,double b,double e=1e-6){return a>b-e&&a<b+e;}
static B::Candidate box(std::uintptr_t body,float sx,float sy,float sz){
    B::Candidate c{};c.body=body;c.owner=body+1;c.foreign=B::kForeignStatic;
    c.bb[0]=-sx/2;c.bb[1]=-sy/2;c.bb[2]=0;c.bb[3]=sx/2;c.bb[4]=sy/2;c.bb[5]=sz;return c;}
int main(){
    const auto all=[](unsigned,B::Pick){return true;};
    const double palm[3]{0,0,.05};
    {   // 1. default OFF = the old filter, unchanged numbers
        B::bigPropsR158.store(false);
        ck(near(B::maxDiameter(),.65),"default 65 cm");
        const B::Candidate c[]{box(10,.5f,.5f,.45f)}; // stool: 0.81 m across
        const auto pick=B::choose(palm,c,1,false,true,all);
        ck(pick.kind==B::Pick::None&&!std::strcmp(pick.why,"MESH_BIGGER_THAN_65CM_OR_FURNITURE"),"stool refused, old reason text");
        B::Brush b{};std::memcpy(b.bb,c[0].bb,sizeof(b.bb));ck(B::brushRule(b)&&!std::strcmp(B::brushRule(b),"MESH_BIGGER_THAN_65CM_OR_FURNITURE"),"brushRule 65 cm");
        ck(near(G::maxReachR158(.81),G::kMaxReach)&&near(G::maxReachR158(.2),.30),"reach stays 30 cm");
        namespace sc=self_collision_r103;
        ck(near(sc::clearRadius(sc::Kind::Prop),.6)&&near(sc::clearRadius(sc::Kind::Corpse),1.0),"Henry clear radius 0.6 m (R103)");
    }
    {   // 2. ON = up to 1 m
        B::bigPropsR158.store(true);
        ck(near(B::maxDiameter(),1.0),"big: 1 m");
        const B::Candidate c[]{box(10,.5f,.5f,.45f)};
        auto pick=B::choose(palm,c,1,false,true,all);ck(pick.kind==B::Pick::Brush&&pick.index==0,"stool admitted");
        B::Brush b{};std::memcpy(b.bb,c[0].bb,sizeof(b.bb));ck(B::brushRule(b)==nullptr,"brushRule admits 0.81 m");
        const B::Candidate t[]{box(20,1.2f,.8f,.8f)}; // table 1.6 m
        pick=B::choose(palm,t,1,false,true,all);ck(pick.kind==B::Pick::None&&!std::strcmp(pick.why,"MESH_BIGGER_THAN_1M_OR_FURNITURE"),"table still refused (new reason)");
        const B::Candidate m[]{box(30,.6f,.6f,.5f),box(31,.1f,.1f,.12f),box(32,1.2f,.8f,.8f)}; // crate, mug on it, table
        pick=B::choose(palm,m,3,false,true,all);ck(pick.kind==B::Pick::Brush&&pick.index==1,"the mug still wins over the crate under it (smallest box)");
        ck(near(G::maxReachR158(.2),.30)&&near(G::maxReachR158(.5),.30),"small props keep 30 cm");
        ck(near(G::maxReachR158(.8),.45)&&near(G::maxReachR158(1.0),.55)&&near(G::maxReachR158(3.0),.55),"edge grip of a big prop: half + 5 cm, max 55 cm");
        ck(near(G::maxReachR158(std::numeric_limits<double>::quiet_NaN()),.30),"NaN size: old reach");
        float bb[6]{-.25f,-.25f,0,.25f,.25f,.7f};ck(B::massFor(bb)<=B::kMaxMass,"loose copy mass stays capped (1.5 kg)");
        namespace sc=self_collision_r103;
        ck(near(sc::clearRadius(sc::Kind::Prop),1.0)&&near(sc::clearRadius(sc::Kind::Corpse),1.0),"big props: Henry collides again at 1 m (or 4 s)");
        sc::Lease l{};l.used=true;l.kind=sc::Kind::Prop;l.released=1000;
        ck(sc::due(l,1500,true,.8)==sc::Due::Wait&&sc::due(l,1500,true,1.01)==sc::Due::Restore&&sc::due(l,5000,true,.1)==sc::Due::Restore,"0.8 m waits, 1 m restores, 4 s always");
        B::bigPropsR158.store(false);
    }
    {   // 3. Experiments: OBJECTS page 3 row 1, default SMALL, never refused, page navigation 2 <-> 3
        VrMenuState s{};ck(!s.live_d39.sceneryBigPropsR158,"default OFF");
        ck(live::group(live::kObjectsPage3R158)==live::Group::Objects,"page 3 belongs to OBJECTS");
        const auto& rows=live::rows(live::kObjectsPage3R158);
        ck(rows[0].id==live::Id::SceneryBigPropsR158&&!rows[0].readOnly&&!rows[0].session,"row 1 = the size switch, saved");
        ck(rows[1].id==live::Id::StaticGrabR89&&rows[2].id==live::Id::SceneryPropSwapR100&&rows[3].id==live::Id::PropFirmHoldR116,"held-prop switches together");
        unsigned selected=live::kObjectsPage2D72;
        ck(live::dispatch(selected,live::kMotionNextD69.l+2,live::kMotionNextD69.t+2,s,[](const VrMenuState&){return true;},[]{return true;})==live::Click::Changed&&selected==live::kObjectsPage3R158,"OBJECTS 2 > 3");
        const auto r=live::rowRect(0);
        ck(live::dispatch(selected,r.l+2,r.t+2,s,[](const VrMenuState&){return true;},[]{return true;})==live::Click::Changed&&s.live_d39.sceneryBigPropsR158,"ON (never refused)");
        wchar_t text[200]{};live::label(s,rows[0],text,200);ck(std::wcsstr(text,L"UP TO 1 M")!=nullptr,"label UP TO 1 M");
        ck(live::dispatch(selected,r.l+2,r.t+2,s,[](const VrMenuState&){return true;},[]{return true;})==live::Click::Changed&&!s.live_d39.sceneryBigPropsR158,"OFF again");
        live::label(s,rows[0],text,200);ck(std::wcsstr(text,L"SMALL")!=nullptr,"label SMALL");
        const auto r1=live::rowRect(1);const bool was=s.live_d39.staticGrabR89;
        ck(live::dispatch(selected,r1.l+2,r1.t+2,s,[](const VrMenuState&){return true;},[]{return true;})==live::Click::Changed&&s.live_d39.staticGrabR89!=was,"row 2 is the same Small props switch");
        ck(live::dispatch(selected,live::kMotionNextD69.l+2,live::kMotionNextD69.t+2,s,[](const VrMenuState&){return true;},[]{return true;})==live::Click::Changed&&selected==live::kObjectsPage3R158,"3 > stays 3");
        ck(live::dispatch(selected,live::kMotionPrevD69.l+2,live::kMotionPrevD69.t+2,s,[](const VrMenuState&){return true;},[]{return true;})==live::Click::Changed&&selected==live::kObjectsPage2D72,"OBJECTS 3 < 2");
        ck(live::dispatch(selected,live::kMotionPrevD69.l+2,live::kMotionPrevD69.t+2,s,[](const VrMenuState&){return true;},[]{return true;})==live::Click::Changed&&selected==unsigned(live::Group::Objects),"OBJECTS 2 < 1");
        unsigned gear=live::kGearPage2R118;
        ck(live::dispatch(gear,live::kMotionNextD69.l+2,live::kMotionNextD69.t+2,s,[](const VrMenuState&){return true;},[]{return true;})==live::Click::Changed&&gear==live::kGearPage3R125,"GEAR 2 > 3 unchanged");
    }
    std::printf("scenery_big_props_r158_test: %u/%u passed\n",total-fail,total);
    return fail?1:0;
}
