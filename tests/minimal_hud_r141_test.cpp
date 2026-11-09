// R141 host test: Minimal HUD (VrMinimalHudR141). the cvar lease (hold 0, retry, give back), the R155 OFF repair (every
// cvar ever written back to 1), the hide masks with the palm glance, the native visible decision the SetDisplayInfo
// filter uses, the settings rows and the HUD.xml names.
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
        // R153: FancyEvent (quest / objective text) and SkillCheck (dialogue skill check text) are kept too
        const char* kept[]{"Compass","Stats","QAMWeapon","QAMFood","Subtitles","InfoText","Hints","DialogLeft","DialogRight","TutorialMessage","TutorialInDialog","Bubbles",
            "FancyEvent","SkillCheck"};
        for(unsigned i=0;i<mh::kCvars.size();++i){
            ck(std::strncmp(mh::kCvars[i],"wh_ui_Show",10)==0&&std::strcmp(mh::kCvars[i]+10,mh::kClips[i])==0,"cvar = wh_ui_Show + clip name");
            bool known=false;for(const char* g:game)known=known||std::strcmp(g,mh::kClips[i])==0;ck(known,"element is in the game's enum");
            for(const char* k:kept)ck(std::strcmp(k,mh::kClips[i])!=0,"prompts/dialogue/subtitles/palm elements are never in the hide list");
        }
        ck(std::strcmp(mh::kCursorClip,"Cursor")==0,"cursor clip = actioncursor");
        // R153: the status icons and the frequent popups stay in the list (the "UI does not constantly turn on" part)
        const char* hidden[]{"Buffs","Crime","Wanted","Trespassing","CommonEvent","GameLog","ItemTransfer"};
        ck(mh::kCvars.size()==7&&mh::kClips.size()==7,"7 held cvars / re-hidden clips");
        for(unsigned i=0;i<mh::kClips.size();++i)ck(std::strcmp(mh::kClips[i],hidden[i])==0,"status + popup list, same order");
        for(const char* cv:mh::kCvars)ck(std::strcmp(cv,"wh_ui_ShowFancyEvent")!=0&&std::strcmp(cv,"wh_ui_ShowSkillCheck")!=0,"quest / skill check cvar never written");
        // R155: the repair list = every cvar ever held (the 7 of today + the R141-R152 FancyEvent and SkillCheck)
        ck(mh::kRepairCvars.size()==9,"9 repaired cvars");
        for(const char* cv:mh::kCvars){bool in=false;for(const char* rc:mh::kRepairCvars)in=in||std::strcmp(cv,rc)==0;ck(in,"held cvar is repaired too");}
        bool fancy=false,skill=false;for(const char* rc:mh::kRepairCvars){fancy=fancy||std::strcmp(rc,"wh_ui_ShowFancyEvent")==0;skill=skill||std::strcmp(rc,"wh_ui_ShowSkillCheck")==0;}
        ck(fancy&&skill,"quest text + skill check cvars are repaired (R152 held them)");
    }
    {   // 2a. R155 repair: OFF puts a 0 back to 1 (persisted by the game), at start / OFF edge / lifetime change; never while ON
        struct Nine {std::array<int,9> v{};std::array<bool,9> unreadable{},refuse{};unsigned writes=0;Nine(){v.fill(1);}
            bool Read(unsigned i,int& o){if(unreadable[i])return false;o=v[i];return true;}
            bool Write(unsigned i,int x){++writes;if(refuse[i])return true;v[i]=x;return true;}} io;
        mh::CvarRepair rep;io.v[7]=0;io.v[8]=0; // FancyEvent + SkillCheck left at 0 by the R152 build, saved by the game
        auto r=rep.Update(io,false,1,1000);
        ck(r.ran&&r.zero==2&&r.repaired==2&&r.failed==0&&io.writes==2,"start: the two persisted zeros back to 1");
        for(int x:io.v)ck(x==1,"all 1");
        r=rep.Update(io,false,1,1500);ck(!r.ran&&io.writes==2,"same lifetime, nothing failed: no more cvar calls");
        r=rep.Update(io,false,1,5000);ck(!r.ran,"no timer while everything is fine");
        io.v[0]=0;r=rep.Update(io,false,2,5100);ck(r.ran&&r.repaired==1&&io.v[0]==1,"lifetime change (save load / new HUD): repaired again");
        io.v[3]=0;r=rep.Update(io,true,2,6000);ck(!r.ran&&io.v[3]==0,"ON: the repair never runs (the lease owns the zeros)");
        r=rep.Update(io,true,3,7000);ck(!r.ran,"still ON");
        r=rep.Update(io,false,3,8000);ck(r.ran&&r.repaired==1&&io.v[3]==1,"ON->OFF edge: a zero the lease did not give back is repaired");
        // the lease's own notOurs case end to end: the player's / persisted 0 at ON time, OFF leaves it, the repair fixes it
        mh::CvarLease lease;FakeCvars lio;lio.v[4]=0;
        auto lr=lease.Update(lio,true,4,9000);ck(lr.notOurs==(1u<<4),"lease: CommonEvent already 0 = notOurs");
        lr=lease.Update(lio,false,4,10000);ck(lr.restored==6&&lio.v[4]==0,"lease OFF: notOurs left at 0 (as before)");
        struct Bridge {FakeCvars& f;bool Read(unsigned i,int& o){return i<7?f.Read(i,o):(o=1,true);}bool Write(unsigned i,int x){return i<7?f.Write(i,x):true;}} bridge{lio};
        mh::CvarRepair rep2;rep2.Update(bridge,true,4,9000);auto r2=rep2.Update(bridge,false,4,10000);
        ck(r2.ran&&r2.zero==1&&r2.repaired==1&&lio.v[4]==1,"repair on the same OFF edge: CommonEvent back to 1");
        // failures: unreadable (console not ready) / refused write = retried after 1 s, not before; no latch
        Nine fio;fio.unreadable[1]=true;fio.v[2]=0;fio.refuse[2]=true;mh::CvarRepair rep3;
        r=rep3.Update(fio,false,1,0);ck(r.ran&&r.failed==2&&r.zero==1&&r.repaired==0,"unreadable + refused = 2 failed");
        r=rep3.Update(fio,false,1,500);ck(!r.ran,"not before 1 s");
        fio.unreadable[1]=false;fio.refuse[2]=false;r=rep3.Update(fio,false,1,1000);ck(r.ran&&r.failed==0&&r.repaired==1&&fio.v[2]==1,"1 s later: repaired");
        r=rep3.Update(fio,false,1,2000);ck(!r.ran,"fixed: quiet again");
    }
    {   // 2. lease: ON holds 0, original back on OFF; nothing owned = nothing touched
        mh::CvarLease lease;FakeCvars io;
        auto r=lease.Update(io,false,1,1000);ck(!r.ran&&io.writes==0,"OFF and nothing owned: no cvar call");
        io.v[2]=0; // the player's own wh_ui_ShowWanted 0
        r=lease.Update(io,true,1,1000);
        ck(r.ran&&r.held==7&&r.writes==6&&r.failed==0,"ON: 6 written, the player's own 0 counted as held");
        ck(r.notOurs==(1u<<2),"notOurs names the player's own 0");
        for(int x:io.v)ck(x==0,"every cvar 0");
        ck(lease.ZeroMask()==0x7Fu&&!lease.Owned(2)&&lease.Owned(0),"zero mask all, the player's 0 is not ours");
        r=lease.Update(io,true,1,1500);ck(!r.ran,"no recheck before 1 s");
        io.v[0]=1; // the game (profile/options load) set Buffs back to 1
        r=lease.Update(io,true,2,1600);ck(r.ran&&r.writes==1&&io.v[0]==0,"lifetime change: re-held at once");
        io.v[1]=1;r=lease.Update(io,true,2,2601);ck(r.ran&&io.v[1]==0,"1 s recheck re-holds too (no latch)");
        io.v[3]=5; // somebody else changed one after our write, while ON: we hold again and keep the first original
        r=lease.Update(io,true,2,3700);ck(io.v[3]==0,"re-held");
        r=lease.Update(io,false,2,3800);ck(r.ran&&r.restored==6&&r.notOurs==0,"OFF: 6 given back");
        ck(io.v[2]==0,"the player's own 0 stays");
        ck(io.v[3]==1,"first original (1) written back, not the later 5");
        for(unsigned i=0;i<io.v.size();++i)if(i!=2)ck(io.v[i]==1,"old value back");
        r=lease.Update(io,false,2,3900);ck(!r.ran,"OFF again: nothing to do");
    }
    {   // 3. refused write: retried on the next check, no latch; OFF leaves a value we never owned alone
        mh::CvarLease lease;FakeCvars io;io.refuse[4]=true;io.unreadable[5]=true;
        auto r=lease.Update(io,true,1,0);ck(r.held==5&&r.failed==2&&!lease.Owned(4)&&!(lease.ZeroMask()&(1u<<4)),"refused + unreadable = failed, not zero");
        io.refuse[4]=false;io.unreadable[5]=false;r=lease.Update(io,true,1,1000);ck(r.held==7&&r.failed==0&&lease.Owned(4)&&lease.Owned(5),"next check holds them");
        io.v[6]=1; // changed by someone else while ON and then we switch OFF before the recheck
        r=lease.Update(io,false,1,1100);ck(io.v[6]==1&&!lease.Owned(6)&&(r.notOurs&(1u<<6)),"OFF: a value somebody else set is theirs (notOurs bit)");
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
        io.v=0;r=lease.Update(io,true,4,40);r=lease.Update(io,false,4,50);ck(io.v==0,"lease alone: a 0 it never owned stays (as before)");
        // R155: the repair puts it back to 1 while SHOWN (hidden=false), never while hidden (held or given back in a menu)
        mh::TutorialRepair rep;auto rr=rep.Update(io,mh::TutorialsHidden(false,false),4,60);ck(rr.ran&&rr.repaired==1&&io.v==1,"SHOWN: persisted 0 back to 1");
        io.v=0;rr=rep.Update(io,mh::TutorialsHidden(true,false),5,70);ck(!rr.ran&&io.v==0,"HIDDEN (held 0 / menu): repair idle");
        rr=rep.Update(io,mh::TutorialsHidden(false,true),5,80);ck(!rr.ran&&io.v==0,"Minimal HUD hides them: repair idle");
        rr=rep.Update(io,mh::TutorialsHidden(false,false),5,90);ck(rr.ran&&io.v==1,"hidden->shown edge: 1 back");
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
