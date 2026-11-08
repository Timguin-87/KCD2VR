// R141 host test: Minimal HUD (VrMinimalHudR141). the cvar lease (hold 0, retry, give back), the hide masks with the
// palm glance, the native visible decision the SetDisplayInfo filter uses, the settings rows and the HUD.xml names.
// g++ -std=c++20 -I src -I tests -I tests/syntax_shims -I tests/openxr_sdk tests/minimal_hud_r141_test.cpp
#include "ui_minimal_hud_r141.h"
#include "ui_hud_experiment36c5.h"
#include "ui_hud_visibility175.h"
#include "ui_gameplay_hud54.h"
#include "subsystems/experimental/live_settings_d39.h"
#include "release_menu_r92.h"
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <fstream>
#include <iterator>
#include <string>
using namespace kcd2vr;
namespace mh=ui_minimal_hud_r141;namespace c=ui_contextual_hud162r1;namespace live=live_settings_d39;
// portable view of VrMenuState: only the fields the two reducers (live_settings_d39 + release_menu_r92) touch
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
struct FakeCvars {
    std::array<int,mh::kCvars.size()> v{};std::array<bool,mh::kCvars.size()> refuse{},unreadable{};unsigned writes=0;
    FakeCvars(){v.fill(1);}
    bool Read(unsigned i,int& out){if(unreadable[i])return false;out=v[i];return true;}
    bool Write(unsigned i,int x){++writes;if(refuse[i])return true;v[i]=x;return true;} // refused = Set() returns but value stays (const cvar)
};
int main(){
    {   // 1. the game's element names: wh_ui_Show + element, every element exists in the WHGame enum (0x555AC0)
        const char* game[]{"Compass","Stats","QAMWeapon","QAMFood","Subtitles","InfoText","GameLog","Hints","DialogLeft","DialogRight",
            "Cursor","Crime","Wanted","PopUpBackground","TutorialMessage","FancyEvent","SkillCheck","ItemTransfer","Buffs","CommonEvent",
            "DiceCursor","Trespassing","RatioStrips","ShootingContest","Bubbles","TutorialInDialog","DiceContainer","Vignette"};
        const char* kept[]{"Compass","Stats","QAMWeapon","QAMFood","Subtitles","InfoText","Hints","DialogLeft","DialogRight","TutorialMessage","TutorialInDialog","Bubbles"};
        for(unsigned i=0;i<mh::kCvars.size();++i){
            ck(std::strncmp(mh::kCvars[i],"wh_ui_Show",10)==0&&std::strcmp(mh::kCvars[i]+10,mh::kClips[i])==0,"cvar = wh_ui_Show + clip name");
            bool known=false;for(const char* g:game)known=known||std::strcmp(g,mh::kClips[i])==0;ck(known,"element is in the game's enum");
            for(const char* k:kept)ck(std::strcmp(k,mh::kClips[i])!=0,"prompts/dialogue/subtitles/palm elements are never in the hide list");
        }
        ck(std::strcmp(mh::kCursorClip,"Cursor")==0,"cursor clip = actioncursor");
    }
    {   // 2. lease: ON holds 0, original back on OFF; nothing owned = nothing touched
        mh::CvarLease lease;FakeCvars io;
        auto r=lease.Update(io,false,1,1000);ck(!r.ran&&io.writes==0,"OFF and nothing owned: no cvar call");
        io.v[2]=0; // the player's own wh_ui_ShowWanted 0
        r=lease.Update(io,true,1,1000);
        ck(r.ran&&r.held==9&&r.writes==8&&r.failed==0,"ON: 8 written, the player's own 0 counted as held");
        for(int x:io.v)ck(x==0,"every cvar 0");
        ck(lease.ZeroMask()==0x1FFu&&!lease.Owned(2)&&lease.Owned(0),"zero mask all, the player's 0 is not ours");
        r=lease.Update(io,true,1,1500);ck(!r.ran,"no recheck before 1 s");
        io.v[0]=1; // the game (profile/options load) set Buffs back to 1
        r=lease.Update(io,true,2,1600);ck(r.ran&&r.writes==1&&io.v[0]==0,"lifetime change: re-held at once");
        io.v[1]=1;r=lease.Update(io,true,2,2601);ck(r.ran&&io.v[1]==0,"1 s recheck re-holds too (no latch)");
        io.v[3]=5; // somebody else changed one after our write, while ON: we hold again and keep the first original
        r=lease.Update(io,true,2,3700);ck(io.v[3]==0,"re-held");
        r=lease.Update(io,false,2,3800);ck(r.ran&&r.restored==8,"OFF: 8 given back");
        ck(io.v[2]==0,"the player's own 0 stays");
        ck(io.v[3]==1,"first original (1) written back, not the later 5");
        for(unsigned i=0;i<io.v.size();++i)if(i!=2)ck(io.v[i]==1,"old value back");
        r=lease.Update(io,false,2,3900);ck(!r.ran,"OFF again: nothing to do");
    }
    {   // 3. refused write: retried on the next check, no latch; OFF leaves a value we never owned alone
        mh::CvarLease lease;FakeCvars io;io.refuse[4]=true;io.unreadable[5]=true;
        auto r=lease.Update(io,true,1,0);ck(r.held==7&&r.failed==2&&!lease.Owned(4)&&!(lease.ZeroMask()&(1u<<4)),"refused + unreadable = failed, not zero");
        io.refuse[4]=false;io.unreadable[5]=false;r=lease.Update(io,true,1,1000);ck(r.held==9&&r.failed==0&&lease.Owned(4)&&lease.Owned(5),"next check holds them");
        io.v[6]=1; // changed by someone else while ON and then we switch OFF before the recheck
        r=lease.Update(io,false,1,1100);ck(io.v[6]==1&&!lease.Owned(6),"OFF: a value somebody else set is theirs");
    }
    {   // 4. hide masks: minimal adds centre point, compass, vitals, status; the palm reveal still shows compass/vitals
        const unsigned saved=0; // the player's own choices: everything visible
        const unsigned prefs=ui_gameplay_hud54::Preferences(mh::Preferences(saved,true),true,false);
        ck((prefs&mh::kPreferences)==mh::kPreferences,"all four preference bits");
        ck(ui_gameplay_hud54::Preferences(mh::Preferences(saved,false),true,false)==c::HideCrosshair163,"OFF: exactly the old gameplay mask");
        ck(!(ui_gameplay_hud54::Preferences(mh::Preferences(saved,true),true,true)&c::HideCrosshair163),"alchemy gaze keeps its cursor");
        ck(mh::PalmEnabled(false,true)&&mh::PalmEnabled(true,false)&&!mh::PalmEnabled(false,false),"palm glance forced on by minimal");
        auto hidden=[&](bool left,bool right){return ui_hud_experiment36c5::VisibilityMask(prefs,{true,left,right});};
        auto shown=[&](unsigned clip,unsigned mask){return !(mask&ui_hud_visibility175::Group(clip));};
        ck(!shown(0,hidden(false,false))&&!shown(1,hidden(false,false)),"no palm look: compass + health hidden");
        ck(shown(0,hidden(true,false))&&!shown(1,hidden(true,false)),"left palm: compass");
        ck(shown(1,hidden(false,true))&&!shown(0,hidden(false,true)),"right palm: health/stamina");
        for(unsigned clip:{2u,5u,6u,7u})ck(!shown(clip,hidden(true,true)),"status icons stay hidden even on the palm");
        for(unsigned clip:{14u,15u,19u})ck(!shown(clip,hidden(true,true)),"centre point hidden");
        for(unsigned clip:{16u,17u,18u})ck(ui_hud_visibility175::Group(clip)==0||clip==16,"hints/tutorial/subtitles are no visibility owners");
        // palm off (dialogue / menus): plain preferences, compass + health hidden
        const unsigned dialogue=ui_hud_experiment36c5::VisibilityMask(ui_gameplay_hud54::Preferences(mh::Preferences(saved,true),false,false),{false,false,false});
        ck(!shown(0,dialogue)&&!shown(1,dialogue)&&!shown(2,dialogue),"dialogue: compass, health, status hidden");
    }
    {   // 5. the native visible decision (SetVisible hook and, with minimal, the SetDisplayInfo visible flag): a game re-show
        //    of a hidden element is turned into hide, an element the mask does not hide stays as the game asks
        ui_hud_visibility175::Registry reg;ui_hud_visibility175::Key k{0x10,0x20,0x30,0x40,7};
        auto valid=[](const ui_hud_visibility175::Key&){return true;};
        ck(reg.Bind(0,k,true,false),"compass bound");
        const unsigned prefs=ui_gameplay_hud54::Preferences(mh::Preferences(0,true),true,false);
        auto d=reg.NativeRequest(0x30,true,0x40,7,ui_hud_experiment36c5::VisibilityMask(prefs,{true,true,false}),true,valid,true,false);
        ck(d.qualified&&d.visible,"left palm: the game's show goes through");
        d=reg.NativeRequest(0x30,true,0x40,7,ui_hud_experiment36c5::VisibilityMask(prefs,{true,false,true}),true,valid,true,false);
        ck(d.qualified&&!d.visible&&d.suppressed,"not looking at the left palm: the game's re-show is suppressed");
        d=reg.NativeRequest(0x30,true,0x40,8,ui_hud_experiment36c5::VisibilityMask(prefs,{true,false,true}),true,valid,true,false);
        ck(!d.qualified&&d.visible,"another HUD generation: not ours, passes unchanged");
        d=reg.NativeRequest(0x30,true,0x40,7,0,true,valid,true,false);ck(d.visible,"minimal off (mask 0): unchanged");
    }
    {   // 5b. tutorials: own switch or Minimal HUD; given back while the options screen is reachable (main / pause menu)
        ck(!mh::TutorialsHidden(false,false)&&mh::TutorialsHidden(true,false)&&mh::TutorialsHidden(false,true),"own switch or minimal");
        ck(mh::TutorialHold(true,false)&&!mh::TutorialHold(true,true)&&!mh::TutorialHold(false,false),"no hold in main/pause menu");
        ck(std::strcmp(mh::kTutorialCvar[0],"wh_ui_TutorialsEnabled")==0,"the game's Tutorials option cvar");
        mh::TutorialLease lease;struct One{int v=1;unsigned w=0;bool Read(unsigned,int&o){o=v;return true;}bool Write(unsigned,int x){++w;v=x;return true;}} io;
        ck(!lease.Update(io,false,1,0).ran&&io.w==0,"shown: never touched");
        auto r=lease.Update(io,mh::TutorialHold(true,false),1,0);ck(r.ran&&io.v==0,"in game: held 0");
        r=lease.Update(io,mh::TutorialHold(true,true),2,10);ck(r.restored==1&&io.v==1,"pause menu: the player's 1 back (options screen saves 1)");
        r=lease.Update(io,mh::TutorialHold(true,false),3,20);ck(io.v==0,"back in game: held again (no latch)");
        r=lease.Update(io,false,3,30);ck(io.v==1&&!lease.AnyOwned(),"switched to SHOWN: 1 back");
        io.v=0;r=lease.Update(io,true,4,40);r=lease.Update(io,false,4,50);ck(io.v==0,"the player's own Tutorials OFF stays OFF");
    }
    {   // 6. re-hide schedule: new HUD at once, else every 500 ms
        mh::Rehide s;ck(s.Due(1,0),"first");s.Done(1,0);ck(!s.Due(1,499)&&s.Due(1,500)&&s.Due(2,10),"500 ms or new HUD");
        s.Reset();ck(s.Due(1,1),"reset");
    }
    {   // 7. switches: Experiments GEAR 3 row 4, release page 3 row 4, VR HUD page rect, default OFF
        VrMenuState s{};ck(!s.live_d39.minimalHudR141&&!mh::enabled.load(),"default OFF");
        const auto& rows=live::rows(live::kGearPage3R125);ck(rows[5].id==live::Id::MinimalHudR141&&rows[6].id==live::Id::HideTutorialsR141,"GEAR 3 rows 6 + 7");
        ck(rows[3].id==live::Id::LockpickOpaqueR141&&rows[4].id==live::Id::EyeHoldR141,"GEAR 3 rows 4 + 5 = lockpick + eye hold (merged R141)");
        unsigned selected=live::kGearPage3R125;const auto r=live::rowRect(5);
        ck(live::dispatch(selected,r.l+2,r.t+2,s,[](const VrMenuState&){return true;},[]{return true;})==live::Click::Changed&&s.live_d39.minimalHudR141,"toggle ON (never refused)");
        wchar_t text[160]{};live::label(s,rows[5],text,160);ck(std::wcsstr(text,L"ON")!=nullptr,"label ON");
        live::label(s,rows[6],text,160);ck(std::wcsstr(text,L"Minimal HUD hides")!=nullptr,"tutorial label tells Minimal HUD hides them");
        ck(live::dispatch(selected,r.l+2,r.t+2,s,[](const VrMenuState&){return true;},[]{return true;})==live::Click::Changed&&!s.live_d39.minimalHudR141,"toggle OFF");
        const auto t=live::rowRect(6);ck(!s.live_d39.hideTutorialsR141,"tutorials shown by default");
        ck(live::dispatch(selected,t.l+2,t.t+2,s,[](const VrMenuState&){return true;},[]{return true;})==live::Click::Changed&&s.live_d39.hideTutorialsR141,"tutorials HIDDEN");
        live::label(s,rows[6],text,160);ck(std::wcsstr(text,L"HIDDEN")!=nullptr,"label HIDDEN");
        ck(live::dispatch(selected,t.l+2,t.t+2,s,[](const VrMenuState&){return true;},[]{return true;})==live::Click::Changed&&!s.live_d39.hideTutorialsR141,"tutorials SHOWN");
        namespace rm=release_menu_r92;ck(rm::rows(2)[3].id==rm::Row::MinimalHud&&rm::rows(2)[4].id==rm::Row::Tutorials,"release page 3 rows 4 + 5");
        ck(rm::advance(s,rm::Row::Tutorials)&&s.live_d39.hideTutorialsR141&&std::wcscmp(rm::value(s,rm::Row::Tutorials),L"HIDDEN")==0,"release tutorials HIDDEN");
        ck(rm::advance(s,rm::Row::Tutorials)&&!s.live_d39.hideTutorialsR141&&std::wcscmp(rm::value(s,rm::Row::Tutorials),L"SHOWN")==0,"release tutorials SHOWN");
        ck(rm::advance(s,rm::Row::MinimalHud)&&s.live_d39.minimalHudR141&&std::wcscmp(rm::value(s,rm::Row::MinimalHud),L"ON")==0,"release toggle ON");
        ck(rm::advance(s,rm::Row::MinimalHud)&&!s.live_d39.minimalHudR141&&std::wcscmp(rm::value(s,rm::Row::MinimalHud),L"OFF")==0,"release toggle OFF");
    }
    {   // 8. names exist in our HUD.xml (companion pak + runtime base), the VR HUD page rects do not overlap, INI key present
        auto read=[](const char* p){std::ifstream f(p,std::ios::binary);return std::string(std::istreambuf_iterator<char>(f),{});};
        const auto xml=read("companion/source/Libs/UI/UIElements/HUD.xml"),base=read("src/hud_xml_base.inc"),ini=read("KCD2VR.ini");
        const auto menu=read("src/vr_menu.cpp");
        ck(!xml.empty()&&!base.empty(),"HUD.xml read (run from the tree root)");
        for(const char* n:mh::kClips){const std::string key=std::string("name=\"")+n+"\" instancename=";
            ck(xml.find(key)!=std::string::npos&&base.find(key)!=std::string::npos,"clip in both HUD.xml copies");}
        ck(xml.find("name=\"Cursor\" instancename=\"cccursor.actioncursor\"")!=std::string::npos,"Cursor = cccursor.actioncursor");
        ck(ini.find("\r\nVrMinimalHudR141=false\r\n")!=std::string::npos,"INI default OFF, CRLF");
        ck(ini.find("\r\nVrHideTutorialsR141=false\r\n")!=std::string::npos,"INI tutorials shown by default, CRLF");
        ck(menu.find("kHudBlockMsgR124{620,640,822,664}")!=std::string::npos&&menu.find("kHudTutorialsR141{832,640,1038,664}")!=std::string::npos,"VR HUD page: BLOCK | Tutorial messages");
        ck(menu.find("kContextualHud162r1{620,580,822,604}")!=std::string::npos&&menu.find("kMinimalHudR141{832,580,1038,604}")!=std::string::npos,"VR HUD page halves");
    }
    std::printf("minimal_hud_r141_test %u/%u\n",total-fail,total);
    return fail?1:0;
}
