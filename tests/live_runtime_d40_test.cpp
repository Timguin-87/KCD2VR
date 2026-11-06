#include "subsystems/dialogues/lively_npc_d73.h"
// Executes the actual Mod contributions and extracted actual preference owners.
// OS clock, INI IO, hook publication and prop-status read are explicit leaves.
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif
#include "vrik/motion_trials_d67.h"
#include "vrik/held_motion_d67.h"
#include "subsystems/crafting/options_d67.h"
#include "subsystems/experimental/live_settings_d39.h"
#include "ui_hud_r112.h" // R112 VrHudQuickSlotFadeR112
#include "vrik/swing_whoosh_r116.h" // R116 VrSwingWhooshR116
#include "controller_layout_r118.h" // R118 VrControllerLayoutR118
#include "subsystems/experimental/free_targeting_d42.h"
#include "subsystems/ranged_reload/arrow_hold_d48.h"
#include "vrik/fist_stroke_d49.h"
#include "vrik/camera_hold_e1.h"
#include "vrik/alchemy_tool_contact_d55.h"
#include "npc_look_policy63.h"
#include "vrik/frik_elbow_lab.h"
#include "vrik/smooth_lab_f2.h"
#include "subsystems/physics/runtime/native_grab_producer60.h"
#include "vrik/pickup_use19c3.h"
#include "vrik/clinch_intent_d40.h"
#include "vrik/lab58.h"
#include "subsystems/masterstrike_combos/body_recipe_policy.h"
#include "subsystems/masterstrike_combos/physical_finisher_cb1.h"
#include "subsystems/masterstrike_combos/native_kill_anim_nf.h"
#include <cstdio>
#include <string>
#include <unordered_map>
static std::uint64_t clockD40=1000;
static std::uint64_t GetTickCount64(){return clockD40;}
namespace kcd2vr {
namespace diagnostic_trace {static unsigned writes=0;void Write(const char*){++writes;}}
namespace frik_pose_probe {
std::atomic<bool> g_no_step55{true},g_no_turn_d40{true},g_close_combat_ready175{false};
std::atomic<std::uintptr_t> g_motion_contract112{0},g_rotation_contract_d40{0};
static std::uintptr_t baseD40=0;
static std::uintptr_t WhBase(){return baseD40;}
#include "live_owners_d40_production.inc"
}
namespace physics::grab_producer60 {
static Status status[2]{};static bool available[2]{};
bool ReadStatus(int side,Status& out) noexcept {out=status[side];return available[side];}
}
struct MenuD40 {live_settings_d39::Settings live_d39{};bool native_kill_nf=false;};
static void Fill(MenuD40& out){
#define KCD2VR_D40_LIVE_READ
#include "subsystems/experimental/live_runtime_d40.inl"
#undef KCD2VR_D40_LIVE_READ
}
static void Apply(const MenuD40& state){
#define KCD2VR_D40_LIVE_APPLY
#include "subsystems/experimental/live_runtime_d40.inl"
#undef KCD2VR_D40_LIVE_APPLY
}
static std::unordered_map<std::string,double> iniD40;
static bool ReadIniBoolExplicit(const char*,const char*,const char* k,bool d){auto i=iniD40.find(k);return i==iniD40.end()?d:i->second!=0;}
static int ReadIniIntExplicit(const char*,const char*,const char* k,int d){auto i=iniD40.find(k);return i==iniD40.end()?d:int(i->second);}
static float ReadIniFloatExplicit(const char*,const char*,const char* k,float d){auto i=iniD40.find(k);return i==iniD40.end()?d:float(i->second);}
static void Load(){const char* explicit_ini="explicit-test.ini";
#include "live_legacy_load_d40_production.inc"
#include "live_combat_load_d41_production.inc"
#define KCD2VR_D40_LIVE_LOAD
#include "subsystems/experimental/live_runtime_d40.inl"
#undef KCD2VR_D40_LIVE_LOAD
#define KCD2VR_M2_HOST_SLOT 154
#include "subsystems/masterstrike_combos/host.inl"
#undef KCD2VR_M2_HOST_SLOT
}
static bool Save(bool ioOk=true){bool ok=true;
    auto wb=[&](const char* k,bool v){if(ioOk)iniD40[k]=v?1:0;return ioOk;};
    auto wi=[&](const char* k,int v){if(ioOk)iniD40[k]=v;return ioOk;};
#define KCD2VR_D40_LIVE_SAVE
#include "subsystems/experimental/live_runtime_d40.inl"
#undef KCD2VR_D40_LIVE_SAVE
#include "live_legacy_save_d40_production.inc"
    return ok;
}
}
int main(){
    using namespace kcd2vr;namespace f=frik_pose_probe;namespace p=physics::grab_producer60;
    unsigned checks=0,failures=0;
    auto check=[&](bool ok,const char* what){++checks;if(!ok){++failures;std::printf("FAIL %s\n",what);}};
    Load();MenuD40 menu{};Fill(menu);
    check(!menu.live_d39.pickupCameraBridgeD55&&!menu.live_d39.alchemyToolContactD55,"D55 experiments default OFF");
    menu.live_d39.pickupCameraBridgeD55=true;Apply(menu);Fill(menu);
    check(menu.live_d39.pickupCameraBridgeD55&&!menu.live_d39.alchemyToolContactD55,"D55 camera toggle independent");
    menu.live_d39.alchemyToolContactD55=true;Apply(menu);check(Save(),"D55 experiment save");
    camera_hold_e1::pickupBridgeD55=false;alchemy_tool_contact_d55::enabled=false;Load();Fill(menu);
    check(menu.live_d39.pickupCameraBridgeD55&&menu.live_d39.alchemyToolContactD55,"D55 experiment roundtrip");
    check(iniD40["VrPickupCameraBridgeD55"]==1&&iniD40["VrAlchemyToolContactD55"]==1,"D55 canonical keys persisted");
    menu.live_d39.pickupCameraBridgeD55=false;menu.live_d39.alchemyToolContactD55=false;Apply(menu);Save();
    check(!camera_hold_e1::pickupBridgeD55.load()&&!alchemy_tool_contact_d55::enabled.load(),"D55 toggle OFF reaches actual runtime");
    check(menu.live_d39.npcGazeHmdD52,"D52 retains legacy FollowHMD default");
    menu.live_d39.npcGazeHmdD52=false;Apply(menu);Fill(menu);
    check(!npc_look_policy63::followHmd()&&!menu.live_d39.npcGazeHmdD52,"D52 Native live preference");
    const auto gazeMode=npc_look_policy63::modeD52.load();Apply(menu);
    check(gazeMode==npc_look_policy63::modeD52.load(),"D52 unrelated menu apply preserves mode generation");
    check(Save()&&iniD40.count("VrDialogueNpcLookHmd")==1&&iniD40["VrDialogueNpcLookHmd"]==0,"D52 canonical key saves Native");
    npc_look_policy63::setFollowHmd(true);Load();Fill(menu);
    check(!menu.live_d39.npcGazeHmdD52,"D52 Native survives reload");
    npc_look_policy63::hookReadyD52=true;npc_look_policy63::sceneAdmittedD52=true;npc_look_policy63::sceneTickD52=clockD40;
    Fill(menu);check(menu.live_d39.npcGazeReadyD52&&menu.live_d39.npcGazeSceneD52,"D52 actual readiness readback");
    clockD40+=81;Fill(menu);check(!menu.live_d39.npcGazeSceneD52,"D52 stale admission is not shown as current");clockD40-=81;
    check(!menu.native_kill_nf&&menu.live_d39.finisherCandidateD41,"D51 backups default to native OFF and physical finisher ON");
    menu.native_kill_nf=true;Apply(menu);Fill(menu);
    check(menu.native_kill_nf&&native_kill_nf::enabled.load()&&menu.live_d39.finisherCandidateD41,"D51 native backup live owner is independent of combo requirement");
    const auto nfGeneration=native_kill_nf::BeginScan();
    native_kill_nf::Observe(nfGeneration,0x1234,true,0,3,clockD40);
    const auto nfSerial=native_kill_nf::liveSerial.load();Apply(menu);
    check(native_kill_nf::StandDown(0x1234,clockD40)&&native_kill_nf::liveSerial.load()==nfSerial&&native_kill_nf::generation.load()==nfGeneration,
          "D51 unrelated menu apply does not retire native playback");
    check(Save()&&iniD40["VrNativeKillAnimations"]==1,"D51 native backup saves its existing canonical key");
    native_kill_nf::SetEnabled(false);Load();Fill(menu);
    check(menu.native_kill_nf&&!native_kill_nf::StandDown(0x1234,clockD40),"D51 reload selects preference without reviving a retired occurrence");
    menu.native_kill_nf=false;Apply(menu);Save();
    check(!menu.live_d39.fistStrokeD49&&!fist_stroke_d49::enabled.load(),"D49 punch trial defaults OFF");
    menu.live_d39.fistStrokeD49=true;Apply(menu);Fill(menu);
    check(menu.live_d39.fistStrokeD49&&fist_stroke_d49::enabled.load(),"D49 punch trial live owner roundtrip");
    check(Save()&&iniD40["VrFistStrokeTrialD49"]==1,"D49 punch trial canonical save");
    fist_stroke_d49::enabled=false;Load();Fill(menu);check(menu.live_d39.fistStrokeD49,"D49 punch trial canonical reload");
    menu.live_d39.fistStrokeD49=false;Apply(menu);Save();
    check(menu.live_d39.arrowNativeTurnD48&&!menu.live_d39.frikElbowsD48,"D56 missing keys select requested native arrow orientation and continuous elbows");
    menu.live_d39.arrowNativeTurnD48=true;menu.live_d39.frikElbowsD48=true;Apply(menu);Fill(menu);
    check(arrow_hold_d48::nativeTurn.load()&&frik_elbow_lab::Active()&&menu.live_d39.arrowNativeTurnD48&&menu.live_d39.frikElbowsD48,"D48 menu changes both actual owners immediately");
    check(Save()&&iniD40["VrArrowHoldNativeTurn"]==1&&iniD40["VrFrikElbows"]==1,"D48 existing canonical keys persist without restart");
    arrow_hold_d48::nativeTurn.store(false);frik_elbow_lab::enabled.store(false);Load();Fill(menu);
    check(menu.live_d39.arrowNativeTurnD48&&menu.live_d39.frikElbowsD48,"D48 arrow and elbow canonical settings reload");
    menu.live_d39.arrowNativeTurnD48=false;menu.live_d39.frikElbowsD48=false;Apply(menu);
    check(Save()&&iniD40["VrArrowHoldNativeTurnD56Adopted"]==1,"D56 arrow preference adoption is persisted");
    Load();Fill(menu);check(!menu.live_d39.arrowNativeTurnD48,"D56 later stable-grip choice survives restart");
    iniD40.erase("VrArrowHoldNativeTurnD56Adopted");Load();Fill(menu);
    check(menu.live_d39.arrowNativeTurnD48,"D56 adopts preferred native orientation over inherited legacy false once");
    menu.live_d39.arrowNativeTurnD48=false;Apply(menu);Save();
    frik_elbow_lab::rung2.store(true);
    check(!arrow_hold_d48::nativeTurn.load()&&!frik_elbow_lab::Active(),"D48 recording rung cannot enable a gameplay solver or arrow turn");
    frik_elbow_lab::rung2.store(false);Save();
    // The F2 comparison is independent of the saved elbow preference. Merely
    // applying another menu setting cannot clear or manufacture its status.
    for(unsigned mode=0;mode<4;++mode)for(bool frik:{false,true}){
        smooth_lab::SetMode(mode);frik_elbow_lab::enabled.store(frik);Fill(menu);
        check(menu.live_d39.elbowF2OverrideD48==(mode==3)&&menu.live_d39.frikElbowsD48==frik,
              "D48 live elbow preference reports the actual F2 override independently");
        menu.live_d39.elbowF2OverrideD48=!menu.live_d39.elbowF2OverrideD48;Apply(menu);Fill(menu);
        check(smooth_lab::Mode()==mode&&menu.live_d39.elbowF2OverrideD48==(mode==3),
              "D48 menu apply cannot change F2 comparison through read-only status");
    }
    smooth_lab::SetMode(0);frik_elbow_lab::enabled.store(false);Fill(menu);
    check(!menu.live_d39.elbowF2OverrideD48&&!menu.live_d39.frikElbowsD48,
          "D48 returning from F2 comparison immediately shows the saved elbow preference");
    check(menu.live_d39.freeTargetingD42,"free-targeting preserves the D54 human-requested default when the canonical key is absent");
    menu.live_d39.freeTargetingD42=true;Apply(menu);Fill(menu);
    check(free_targeting_d42::enabled.load()&&menu.live_d39.freeTargetingD42,"free-targeting menu writes and reads actual owner");
    free_targeting_d42::contextReady=true;free_targeting_d42::cameraLeased=true;Fill(menu);
    check(menu.live_d39.freeTargetingContextD42&&menu.live_d39.freeTargetingLeaseD42,"free-targeting status is separate from selected preference");
    check(Save()&&iniD40["VrFreeTargeting"]==1,"free-targeting canonical key persists");
    free_targeting_d42::set(false);Load();Fill(menu);
    check(menu.live_d39.freeTargetingD42,"free-targeting canonical key reloads");
    check(menu.live_d39.pickupNearOnlyD40&&menu.live_d39.combatNoStepD40&&menu.live_d39.combatNoTurnD40,"missing-key direct pickup and attack defaults");
    check(menu.live_d39.clinchModeD40==2,"missing-key qualified automatic clinch default OFF");
    check(!menu.live_d39.combatNoTurnReadyD40&&!menu.live_d39.clinchReadyD40,"preference cannot manufacture hook readiness");
    check(!menu.live_d39.propStatusKnownD40[0]&&!menu.live_d39.propStatusKnownD40[1],"absent native status stays unknown");
    check(menu.live_d39.npcContactReactionD41&&menu.live_d39.npcStaminaReactionD41&&menu.live_d39.finisherCandidateD41,"existing combat option defaults retained");
    const auto reach=pickup_use19c3::reachEpochD40.load(),clinch=clinch_intent_d40::generation.load();
    menu.live_d39.pickupNearOnlyD40=false;menu.live_d39.combatNoStepD40=false;menu.live_d39.combatNoTurnD40=false;
    menu.live_d39.clinchModeD40=1;menu.live_d39.labLivingTouchD40=false;menu.live_d39.labLivingImpactD40=false;
    menu.live_d39.labCorpseDragD40=false;menu.live_d39.labPropWake=false;lab58::livingHitK.store(12.f);
    menu.live_d39.npcContactReactionD41=false;menu.live_d39.npcStaminaReactionD41=false;menu.live_d39.finisherCandidateD41=false;
    Apply(menu);Fill(menu);
    check(!menu.live_d39.pickupNearOnlyD40&&!menu.live_d39.combatNoStepD40&&!menu.live_d39.combatNoTurnD40,"menu updates actual pickup and attack owners");
    check(menu.live_d39.clinchModeD40==1&&pickup_use19c3::reachEpochD40.load()==reach+1&&clinch_intent_d40::generation.load()==clinch+2,"policy changes retire old gesture epochs");
    check(!bodycombo59c7g::npcReactsToBlade.load()&&!bodycombo59c7g::staminaHits.load()&&!finisher_cb1::enabled.load(),"combat menu updates actual runtime owners immediately");
    check(!lab58::livingTouch.load()&&!lab58::livingHitImpact.load()&&!lab58::corpseDrag.load()&&!lab58::propWake.load()&&lab58::livingHitK.load()==12.f,"lab apply preserves force setting and writes real switches");
    const auto traces=diagnostic_trace::writes;Apply(menu);
    check(diagnostic_trace::writes==traces&&pickup_use19c3::reachEpochD40.load()==reach+1&&clinch_intent_d40::generation.load()==clinch+2,"unchanged apply neither logs lab nor advances policy");
    check(Save(),"INI write success propagated");
    for(const char* key:{"VrPickupNearOnlyD40","VrCombatNoStep","VrCombatNoTurnD40","VrClinchModeD40","VrLabLivingTouch58","VrLabLivingHitImpact58","VrLabCorpseDrag58","VrLabPropWake58"})check(iniD40.count(key)==1,key);
    check(!Save(false),"failed INI write is not reported as persisted");
    for(const char* key:{"VrNpcReactsToBlade","VrNpcReactsStaminaHits","VrPhysicalComboFinisher"})check(iniD40.count(key)==1&&iniD40[key]==0,key);
    bodycombo59c7g::npcReactsToBlade.store(true);bodycombo59c7g::staminaHits.store(true);finisher_cb1::enabled.store(true);
    pickup_use19c3::setNearOnlyD40(true);f::SetCombatNoStep55(true);f::SetCombatNoTurnD40(true);f::SetClinchModeD40(0);
    f::SetLab58(true,true,true,4);f::SetLab58LivingTouch(true);Load();Fill(menu);
    check(!menu.live_d39.pickupNearOnlyD40&&!menu.live_d39.combatNoStepD40&&!menu.live_d39.combatNoTurnD40&&menu.live_d39.clinchModeD40==1,"new and old canonical keys roundtrip");
    check(!menu.live_d39.labLivingTouchD40&&!menu.live_d39.labLivingImpactD40&&!menu.live_d39.labCorpseDragD40&&!lab58::propWake.load(),"lab switches roundtrip through existing host loader");
    check(!menu.live_d39.npcContactReactionD41&&!menu.live_d39.npcStaminaReactionD41&&!menu.live_d39.finisherCandidateD41,"combat options roundtrip through unchanged canonical host loader");
    iniD40["VrClinchModeD40"]=99;Load();check(f::ClinchModeD40()==2,"invalid clinch mode resolves OFF");
    f::baseD40=0x10000;f::g_motion_contract112.store(f::baseD40);f::g_rotation_contract_d40.store(f::baseD40);f::g_close_combat_ready175.store(true);Fill(menu);
    check(menu.live_d39.combatNoTurnReadyD40&&menu.live_d39.clinchReadyD40&&!menu.live_d39.combatNoTurnD40,"hook readiness and selected preference are independent");
    f::g_rotation_contract_d40.store(f::baseD40+16);Fill(menu);check(!menu.live_d39.combatNoTurnReadyD40,"old module hook publication does not qualify");
    p::available[0]=p::available[1]=true;p::status[0].state=p::State::Held;p::status[0].tick=950;
    p::status[1].state=p::State::Quarantined;p::status[1].tick=1000;Fill(menu);
    check(menu.live_d39.propStatusKnownD40[0]&&menu.live_d39.propStateD40[0]==3,"right Held status reflects actual enum");
    check(menu.live_d39.propStatusKnownD40[1]&&menu.live_d39.propStateD40[1]==6,"left Quarantined is independent");
    p::status[0].tick=499;p::status[1].tick=1001;Fill(menu);
    check(!menu.live_d39.propStatusKnownD40[0]&&!menu.live_d39.propStatusKnownD40[1],"stale and future prop status rejected");
    p::status[0].tick=500;p::status[1].tick=0;Fill(menu);
    check(menu.live_d39.propStatusKnownD40[0]&&!menu.live_d39.propStatusKnownD40[1],"status freshness boundary and zero timestamp");
    static_assert(unsigned(p::State::Idle)==0&&unsigned(p::State::Preparing)==1&&unsigned(p::State::WaitingPresent)==2&&unsigned(p::State::Held)==3&&unsigned(p::State::Releasing)==4&&unsigned(p::State::Retiring)==5&&unsigned(p::State::Quarantined)==6&&unsigned(p::State::Fault)==7);
    std::printf("D51 actual live runtime: %u checks, %u failures; native game and GDI NOT RUN\n",checks,failures);
    return failures?1:0;
}
