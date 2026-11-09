// R163 host test: "Training fights: vanilla NPCs" (VrTrainingVanillaNpcR163, training_vanilla_r163.h) - the gate every NPC-writing
// feature reads (held = switch ON + practice / quest weapon fresh in Henry's hand), the per-write / per-scene skip counts, the
// always-on tutor count rows, the F2 edge rows, the force-block lease given back (production slot 5008, fake console as in
// npc_force_block153_test.cpp), and the Experiments row (VRIK page 2 row 4 next to R159; "F2 comparison boundary" moved to
// VRIK MOTION page 2 in place of the REPEATED "Forward lean height" row, which stays on BODY page 2).
// g++ -std=c++20 -I src -I src/vrik tests/training_vanilla_r163_test.cpp -o training_vanilla_r163_test && ./training_vanilla_r163_test
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <set>
#include <string>
#include "subsystems/direct/training_vanilla_r163.h"
#include "subsystems/experimental/live_settings_d39.h"
static unsigned total=0,fail=0;
static void ck(bool b,const char* s){++total;if(!b){++fail;std::printf("FAIL %s\n",s);}}
// ---- fake console / mod.cpp services used by the production force-block lease (same leaves as npc_force_block153_test) ----
static int cvarValue=0;static int fakeCvar;static std::string lastLine,lastStatus;
void* SafeGetCryConsole() noexcept {return &fakeCvar;}
void* SafeConsoleGetCVar(void*,const char* n) noexcept {return std::strcmp(n,"wh_cs_ForceBlock")==0?&fakeCvar:nullptr;}
int SafeCVarGetInt(void*,int) noexcept {return cvarValue;}
static bool CheatCVarSetInt143(void*,int v) noexcept {cvarValue=v;return true;}
static bool CheatCVarSetFloat143(void*,float) noexcept {return true;}
namespace diagnostic_trace {void Write(const char* m) noexcept {lastLine=m;} void WriteStatus(const char* m) noexcept {lastStatus=m;}}
namespace npc_defense {inline std::atomic<bool> bySkill{true};}
struct RagdollConsole59 {bool Read(const char*,float&) noexcept {return false;} bool Write(const char*,float) noexcept {return false;}};
namespace retail_build112 {inline bool Ready(std::uintptr_t){return true;}}
inline void* GetModuleHandleW(const wchar_t*){return nullptr;}
namespace kcd2vr::routerH3 {struct Windows{};}
namespace kcd2vr::dodge_window_d70 {struct Lease{};}
#define KCD2VR_M2_HOST_SLOT 5008
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
using namespace kcd2vr;
namespace tv=training_vanilla_r163;namespace T=tutorial_native_r159;namespace fb=npc_force_block153;namespace live=live_settings_d39;
// portable view of VrMenuState (same fields as tutorial_native_r159_test.cpp)
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
// the service in mod.cpp, written the same way: publish held, step the edge, give back the force block at the ON edge
static tv::Edge edge;
static int service(std::uint64_t now,NpcBlock39& lease){
    const bool want=tv::wanted(tv::enabled.load(),T::tutorialNow(now));tv::held.store(want);
    const int e=edge.step(want);
    if(e==1){(void)tv::take();tv::period.fetch_add(1);lease.Restore();}
    return e;}
int main(){
    {   // 1. defaults + the gate table
        ck(tv::enabled.load()&&!tv::held.load(),"runtime default: switch ON, nothing held before the first tick");
        VrMenuState s{};ck(s.live_d39.trainingVanillaNpcR163,"Settings default ON (= INI default true)");
        ck(!tv::wanted(false,false)&&!tv::wanted(false,true)&&!tv::wanted(true,false)&&tv::wanted(true,true),"held only with switch ON and a training fight");
        ck(std::strcmp(tv::kKey,"VrTrainingVanillaNpcR163")==0,"INI key");
    }
    {   // 2. the training fight = the R156/R159 practice-weapon detector, fresh (<= 1 s), independent of the R159 mode
        NpcBlock39 lease{};
        fb::practiceWeapon.store(false);fb::practiceWeaponMs.store(1000);
        ck(service(1000,lease)==-1&&!tv::holdNow(),"real weapon: nothing held, first OFF is no edge");
        fb::practiceWeapon.store(true);fb::practiceWeaponMs.store(2000);
        ck(T::mode.load()==0,"R159 Tutorials row OFF (his R162 default)");
        ck(service(2100,lease)==1&&tv::holdNow(),"wooden sword: held (ON edge) even with the R159 row OFF");
        ck(service(2200,lease)==-1&&tv::holdNow(),"no repeated edge");
        ck(service(3100,lease)==0&&!tv::holdNow(),"stale snapshot (> 1 s) = no training fight: everything runs again");
        fb::practiceWeaponMs.store(3200);ck(service(3200,lease)==1&&tv::holdNow(),"fresh again = held again (no latch)");
        tv::enabled.store(false);ck(service(3300,lease)==0&&!tv::holdNow(),"switch OFF: never refused, released at once");
        tv::enabled.store(true);ck(service(3400,lease)==1,"switch ON again");
        fb::practiceWeapon.store(false);fb::practiceWeaponMs.store(3500);ck(service(3500,lease)==0&&!tv::holdNow(),"practice weapon gone: released");
    }
    {   // 3. one gate per write: skip + counts (all / tutor = Henry's reciprocal opponent), nothing counted when not held
        (void)tv::take();tv::held.store(false);
        ck(!tv::skip(tv::NpcFlinch)&&!tv::skip(tv::NpcDefenseSpeed,0xA0),"not held: the write runs, nothing counted");
        ck(!tv::any(tv::take()),"no counts outside a training fight");
        tv::held.store(true);fb::opponent.store(0xA0);
        ck(tv::skip(tv::NpcDefenseSpeed,0xA0)&&tv::skip(tv::NpcDefenseSpeed,0xB0)&&tv::skip(tv::NpcFlinch),"held: every write skipped");
        const auto c=tv::take();
        ck(c.all[tv::NpcDefenseSpeed]==2&&c.tutor[tv::NpcDefenseSpeed]==1,"defence speed: 2 writes, 1 on the tutor");
        ck(c.all[tv::NpcFlinch]==1&&c.tutor[tv::NpcFlinch]==0,"flinch: entity id only, no tutor identity");
        char text[400]{};tv::formatCounts(text,sizeof(text),c);
        ck(std::strcmp(text,"npcDefenseSpeed=2/1,npcFlinch=1/-")==0,"row text: all/tutor, '-' = no identity");
        ck(!tv::any(tv::take()),"take() empties the counters");
        tv::formatCounts(text,sizeof(text),tv::Counts{});ck(std::strcmp(text,"none")==0,"no counts = none");
        char held[400]{};tv::formatHeld(held,sizeof(held));
        ck(std::strstr(held,"npcDefenseSpeed,forceBlock,npcFlinch")==held&&std::strstr(held,"sharedRules")!=nullptr,"held list = every gated feature");
        for(unsigned i=0;i<tv::Count;++i)ck(std::strcmp(tv::featureName(i),"?")!=0,"every feature has a name");
    }
    {   // 4. per-frame writers count once per scene / NPC key and period, but skip every call
        std::atomic<std::uintptr_t> last{0};(void)tv::take();tv::held.store(true);
        for(int i=0;i<50;++i)ck(tv::skipOnce(tv::DialogueLook,last,0x5CE),"look point skipped every frame");
        ck(tv::take().all[tv::DialogueLook]==1,"50 frames of one scene = 1 count");
        ck(tv::skipOnce(tv::DialogueLook,last,0x5CF)&&tv::take().all[tv::DialogueLook]==1,"another scene = 1 more");
        tv::period.fetch_add(1);ck(tv::skipOnce(tv::DialogueLook,last,0x5CF)&&tv::take().all[tv::DialogueLook]==1,"same scene in the next held period counts again");
        tv::held.store(false);ck(!tv::skipOnce(tv::DialogueLook,last,0x5CF),"not held: writes again");
    }
    {   // 5. always-on tutor rows: at most one per 30 s while held, only with new counts
        ck(!tv::tutorRowDue(false,0,100),"no counts = no row");
        ck(tv::tutorRowDue(true,0,100),"first row");
        ck(!tv::tutorRowDue(true,1000,30999)&&tv::tutorRowDue(true,1000,31000),"next row after 30 s");
        ck(tv::tutorRowDue(true,5000,100),"clock before the last row = due (no stuck timer)");
    }
    {   // 6. the production force-block lease: held = the original value back (R153 wh_cs_ForceBlock), resumes after
        NpcBlock39 lease{};RagdollConsole59 io;cvarValue=0;std::uint64_t now=10000;
        const auto publish=[&](int phase){fb::opponent=0xA0;fb::reciprocal=true;fb::skill=30;fb::henryPhase=phase;fb::publishedMs=now;};
        fb::practiceWeapon.store(false);tv::held.store(false);
        for(int i=0;i<300&&cvarValue==0;++i){now+=10;publish(1);lease.Service(true,2,now,io);publish(4);lease.Service(true,2,now,io);}
        ck(cvarValue==1&&lease.owned,"real fight: lease written in Henry's swing (mode 2 always)");
        tv::held.store(true);publish(8);lease.Service(true,2,now,io);
        ck(cvarValue==0&&!lease.owned,"training vanilla held: original back on the same tick, even mid-swing");
        for(int i=0;i<20;++i){now+=10;publish(1);lease.Service(true,2,now,io);publish(4);lease.Service(true,2,now,io);}
        ck(cvarValue==0&&!lease.owned,"held: never leased again");
        tv::held.store(false);
        for(int i=0;i<300&&cvarValue==0;++i){now+=10;publish(1);lease.Service(true,2,now,io);publish(4);lease.Service(true,2,now,io);}
        ck(cvarValue==1,"released: the roll and lease run again (no latch)");
        lease.Restore();ck(cvarValue==0,"restore");
        // the service's ON edge gives it back too (Restore), independent of Service
        for(int i=0;i<300&&cvarValue==0;++i){now+=10;publish(1);lease.Service(true,2,now,io);publish(4);lease.Service(true,2,now,io);}
        fb::practiceWeapon.store(true);fb::practiceWeaponMs.store(now);edge=tv::Edge{};
        ck(service(now,lease)==1&&cvarValue==0&&!lease.owned,"ON edge: force block given back at once");
        fb::practiceWeapon.store(false);tv::held.store(false);
    }
    {   // 7. Experiments: VRIK page 2 row 4 (next to R159 row 5), ON/OFF, never refused; the moved row and the repeated row
        VrMenuState s{};unsigned selected=live::kHandsPage2D71;
        const auto& rows=live::rows(selected);
        ck(rows[3].id==live::Id::TrainingVanillaNpcR163&&!rows[3].readOnly&&!rows[3].session,"row 4 = the switch, saved");
        ck(rows[4].id==live::Id::TutorialNativeHitsR159,"row 5 = R159 Tutorials, unchanged");
        wchar_t text[200]{};live::label(s,rows[3],text,200);ck(std::wcsstr(text,L": ON (NPCs vanilla")!=nullptr&&std::wcslen(text)<100,"label ON (fits the 418 px row)");
        const auto r=live::rowRect(3);int saved=0;
        const auto click=[&]{return live::dispatch(selected,r.l+2,r.t+2,s,[](const VrMenuState&){return true;},[&]{++saved;return true;});};
        ck(click()==live::Click::Changed&&!s.live_d39.trainingVanillaNpcR163&&saved==1,"-> OFF (never refused, saved)");
        live::label(s,rows[3],text,200);ck(std::wcsstr(text,L": OFF (all NPC features run)")!=nullptr&&std::wcslen(text)<100,"label OFF");
        ck(click()==live::Click::Changed&&s.live_d39.trainingVanillaNpcR163&&saved==2,"-> ON again");
        bool timingOnMotion2=false,leanOnMotion2=false,leanOnBody2=false,timingOnHands2=false;
        for(const auto& row:live::kMotionTrialsD69){timingOnMotion2|=row.id==live::Id::TimingLocationD64;leanOnMotion2|=row.id==live::Id::LeanHeightD70;}
        for(const auto& row:live::kBodyTrialsD76)leanOnBody2|=row.id==live::Id::LeanHeightD70;
        for(const auto& row:live::kHandTrialsD71)timingOnHands2|=row.id==live::Id::TimingLocationD64;
        ck(timingOnMotion2&&!timingOnHands2,"F2 comparison boundary moved to VRIK MOTION page 2");
        ck(!leanOnMotion2&&leanOnBody2,"the REPEATED Forward lean height row left MOTION page 2, still on BODY page 2");
        // no switch lost: every Id that had a row still has one
        std::set<unsigned> ids;for(unsigned n=0;n<=live::kObjectsPage3R158;++n)for(const auto& row:live::rows(n))ids.insert(unsigned(row.id));
        ck(ids.count(unsigned(live::Id::TimingLocationD64))&&ids.count(unsigned(live::Id::LeanHeightD70))&&ids.count(unsigned(live::Id::TrainingVanillaNpcR163))&&
           ids.count(unsigned(live::Id::TutorialNativeHitsR159)),"every switch still has a row");
    }
    std::printf("training_vanilla_r163_test: %u/%u passed\n",total-fail,total);
    return fail?1:0;
}
