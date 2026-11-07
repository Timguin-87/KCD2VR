#include "subsystems/dialogues/lively_npc_d73.h"
#include "subsystems/vrmap/runtime/held_map_m1.h"
#include "subsystems/experimental/live_settings_d39.h"
#include "subsystems/ranged_reload/options_d65c.h"
#include "vrik/punch_travel_d65.h"
#include "vrik/two_hand_bias_d65.h"
#include "vrik/camera_hold_e1.h"
#include "vrik/knife_pouch_d66.h"
#include "core/transfer_clip_d65.h"
#include "vrik/motion_trials_d67.h"
#include "vrik/held_motion_d67.h"
#include "vrik/claude_motion_d69.h"
#include "vrik/weapon_trials_d69.h"
#include "experiments/claude_bundle_d70.h"
#include "experiments/contact_handover_d71.h"
#include "vrik/roomscale58.h"
#include "vrik/dodge_window_d70.h"
#include "subsystems/crafting/options_d67.h"
#include <cassert>
#include <cstdio>
#include <limits>
#include <string>
#include <unordered_map>
// Explicit native installation leaf: this fixture executes the production
// setting contributions, not Windows hook installation or headset animation.
static bool prepareAcceptedD81=true;
static unsigned prepareCallsD81=0;
namespace kcd2vr::lively_npc_d73 {
bool Prepare() noexcept {++prepareCallsD81;return prepareAcceptedD81;}
}
namespace kcd2vr::action_time_owner59c11 {
namespace transfer65=kcd2vr::transfer_clip_d65;
static std::atomic<float> g_alchemy_transfer_rate_d65{1.f};
static std::atomic<bool> g_unowned_pickup_clock_d65{false};
#include "d65_action_options.inc"
}
using namespace kcd2vr;
struct State {live_settings_d39::Settings live_d39{};float punch_minimum_m_d65=.10f;};
static std::unordered_map<std::string,double> ini;
static bool io=true;
static bool wb(const char* k,bool v){if(io)ini[k]=v;return io;}
static bool wi(const char* k,int v){if(io)ini[k]=v;return io;}
static bool wf(const char* k,float v){if(io)ini[k]=v;return io;}
static bool ReadIniBoolExplicit(const char*,const char*,const char* k,bool v){auto p=ini.find(k);return p==ini.end()?v:p->second!=0;}
static int ReadIniIntExplicit(const char*,const char*,const char* k,int v){auto p=ini.find(k);return p==ini.end()?v:int(p->second);}
static float ReadIniFloatExplicit(const char*,const char*,const char* k,float v){auto p=ini.find(k);return p==ini.end()?v:float(p->second);}
static void Read(State& out){
#include "d65_settings_read.inc"
}
static void Apply(const State& state){
#include "d65_settings_apply.inc"
}
static bool Save(){bool ok=true;
#include "d65_settings_save.inc"
return ok;}
static void Load(){const char* explicit_ini="fixture";
#include "d65_settings_load.inc"
}
static bool close(float a,float b){return std::abs(a-b)<.00001f;}
int main(){
 // Missing keys restore intended defaults: D81 NPC presentation and smith
 // anchoring ON, while the independent D69/D71 motion trials remain OFF.
 claude_motion_d69::SetHandLead(true);claude_motion_d69::SetElbowHysteresis(true);
 weapon_trials_d69::setSameFrame(true);weapon_trials_d69::setRigidGrip(true);
 claude_motion_d69::SetHandLeadMs(33);
 contact_handover_d71::SetContactContinuity(true);contact_handover_d71::SetItemHandover(true);
 lively_npc_d73::SetEnabled(false);lively_npc_d73::SetListeningGestures(false);
 crafting_d67::setSmithSemiAnchors(false);
 Load();State state{};Read(state);
 assert(state.live_d39.npcLifeD73&&state.live_d39.npcListeningD76&&state.live_d39.smithSemiD67);
 assert(prepareCallsD81==0); // load stores preference; qualified startup installs hooks
 // Every combination persists independently, including explicit saved OFF.
 // Change the process owners before reload so stale state cannot pass the test.
 for(unsigned bits=0;bits<8;++bits){
  state.live_d39.npcLifeD73=(bits&1)!=0;state.live_d39.npcListeningD76=(bits&2)!=0;
  state.live_d39.smithSemiD67=(bits&4)!=0;
  Apply(state);assert(Save());
  assert(ini.at("VrLivelyNpcD73")==double((bits&1)!=0));
  assert(ini.at("VrNpcListeningGesturesD76")==double((bits&2)!=0));
  assert(ini.at("VrSmithSemiAnchorsD67")==double((bits&4)!=0));
  lively_npc_d73::SetEnabled(!(bits&1));lively_npc_d73::SetListeningGestures(!(bits&2));
  crafting_d67::setSmithSemiAnchors(!(bits&4));
  Load();Read(state);
  assert(state.live_d39.npcLifeD73==bool(bits&1)&&state.live_d39.npcListeningD76==bool(bits&2));
  assert(state.live_d39.smithSemiD67==bool(bits&4));
 }
 assert(prepareCallsD81==4); // only four OFF-to-ON menu requests need installation
 state.live_d39.npcLifeD73=false;Apply(state);
 prepareAcceptedD81=false;state.live_d39.npcLifeD73=true;Apply(state);Read(state);
 assert(!state.live_d39.npcLifeD73&&state.live_d39.npcListeningD76&&state.live_d39.smithSemiD67);
 prepareAcceptedD81=true;state.live_d39.npcLifeD73=true;Apply(state);assert(Save());
 assert(!state.live_d39.contactContinuityD71&&!state.live_d39.itemHandoverD71);
 state.live_d39.contactContinuityD71=true;Apply(state);assert(Save());
 contact_handover_d71::SetContactContinuity(false);Load();Read(state);
 assert(state.live_d39.contactContinuityD71&&!state.live_d39.itemHandoverD71);
 state.live_d39.itemHandoverD71=true;Apply(state);assert(Save());
 contact_handover_d71::SetItemHandover(false);Load();Read(state);
 assert(state.live_d39.contactContinuityD71&&state.live_d39.itemHandoverD71);
 state.live_d39.contactContinuityD71=false;state.live_d39.itemHandoverD71=false;Apply(state);assert(Save());
 assert(!state.live_d39.stabFixD77&&!state.live_d39.unarmedFixD77&&!state.live_d39.masterstrikeFixD77&&!state.live_d39.hudWalkoffD70&&close(state.live_d39.leanHeightD70,.10f));
 state.live_d39.stabFixD77=state.live_d39.unarmedFixD77=state.live_d39.masterstrikeFixD77=state.live_d39.hudWalkoffD70=true;state.live_d39.leanHeightD70=.08f;state.live_d39.dodgeWindowD70=1.5f;
 assert(!state.live_d39.handLeadD69&&!state.live_d39.elbowHysteresisD69&&!state.live_d39.weaponSameFrameD69&&!state.live_d39.rigidGripD69);
 assert(close(state.live_d39.handLeadMsD69,11));
 state.live_d39.handLeadD69=state.live_d39.elbowHysteresisD69=state.live_d39.weaponSameFrameD69=state.live_d39.rigidGripD69=true;
 state.live_d39.handLeadMsD69=22;
 assert(!state.live_d39.knifeRightHipD66);state.live_d39.knifeRightHipD66=true;
 assert(close(state.punch_minimum_m_d65,.10f));
 assert(state.live_d39.npcLifeD73&&state.live_d39.npcListeningD76&&!state.live_d39.mapNativePaperM43);
 state.live_d39.npcLifeD73=true;state.live_d39.mapNativePaperM43=true;Apply(state);assert(Save());
 lively_npc_d73::SetEnabled(false);held_map_m1::SetNativePaperM43(false);Load();Read(state);
 assert(state.live_d39.npcLifeD73&&state.live_d39.mapNativePaperM43);
 assert(ini["VrLivelyNpcD73"]==1&&ini["VrMapNativePaperM43"]==1);
 assert(state.live_d39.pickupCameraBridgeD55&&state.live_d39.twoHandBiasD65);
 assert(state.live_d39.bowAimD65&&state.live_d39.crossbowReloadD65&&!state.live_d39.pickupClockD65);
 assert(close(state.live_d39.alchemySpeedD65,4.f)&&close(state.live_d39.crankTravelD66,.08f));
 state.punch_minimum_m_d65=.045f;state.live_d39.pickupCameraBridgeD55=false;
 state.live_d39.twoHandBiasD65=true;state.live_d39.twoHandPrimaryBiasD65=.3f;
 state.live_d39.bowAimD65=true;state.live_d39.crossbowReloadD65=true;
 state.live_d39.alchemySpeedD65=3;state.live_d39.pickupClockD65=true;state.live_d39.crankTravelD66=.14f;
 assert(state.live_d39.gripAnchorD67&&state.live_d39.arcCoverageD67&&state.live_d39.supportReleaseD67&&state.live_d39.contactFairnessD67);
 assert(state.live_d39.elbowContinuityD67&&state.live_d39.smithRecoveryD67&&state.live_d39.smithSemiD67&&state.live_d39.motionTrialD67==2);
 assert(close(state.live_d39.elbowOutwardD67,.35f)&&close(state.live_d39.elbowBackD67,.25f));
 state.live_d39.gripAnchorD67=state.live_d39.arcCoverageD67=state.live_d39.supportReleaseD67=state.live_d39.contactFairnessD67=false;
 state.live_d39.elbowContinuityD67=state.live_d39.smithRecoveryD67=false;state.live_d39.smithSemiD67=true;state.live_d39.motionTrialD67=3;
 state.live_d39.elbowOutwardD67=.45f;state.live_d39.elbowBackD67=.1f;
 Apply(state);assert(Save());assert(ini.size()==37);
 const auto revision=ranged_options_d65c::bowAimRevision.load();Apply(state);
 assert(ranged_options_d65c::bowAimRevision.load()==revision); // unrelated menu apply cannot retire a shot
 State other{};Apply(other);
 assert(!claude_motion_d69::HandLead()&&!claude_motion_d69::ElbowHysteresis()&&!weapon_trials_d69::sameFrame()&&!weapon_trials_d69::rigidGrip());
 Load();Read(other);
 assert(other.live_d39.stabFixD77&&other.live_d39.unarmedFixD77&&other.live_d39.masterstrikeFixD77&&other.live_d39.hudWalkoffD70&&close(other.live_d39.leanHeightD70,.08f)&&close(other.live_d39.dodgeWindowD70,1.5f));
 assert(other.live_d39.handLeadD69&&other.live_d39.elbowHysteresisD69&&other.live_d39.weaponSameFrameD69&&other.live_d39.rigidGripD69&&close(other.live_d39.handLeadMsD69,22));assert(close(other.live_d39.crankTravelD66,.14f)&&other.live_d39.knifeRightHipD66);
 assert(!other.live_d39.gripAnchorD67&&!other.live_d39.arcCoverageD67&&!other.live_d39.supportReleaseD67&&!other.live_d39.contactFairnessD67);
 assert(!other.live_d39.elbowContinuityD67&&!other.live_d39.smithRecoveryD67&&other.live_d39.smithSemiD67&&other.live_d39.motionTrialD67==3);
 assert(close(other.live_d39.elbowOutwardD67,.45f)&&close(other.live_d39.elbowBackD67,.1f));
 assert(close(other.punch_minimum_m_d65,.045f)&&!other.live_d39.pickupCameraBridgeD55);
 assert(other.live_d39.twoHandBiasD65&&close(other.live_d39.twoHandPrimaryBiasD65,.3f));
 assert(other.live_d39.bowAimD65&&other.live_d39.crossbowReloadD65&&other.live_d39.pickupClockD65&&close(other.live_d39.alchemySpeedD65,3));
 ini["VrPunchMinimumTravelMetersD65"]=std::numeric_limits<double>::quiet_NaN();
 ini["VrCrankTravelD66C"]=std::numeric_limits<double>::quiet_NaN();
 ini["VrTwoHandPrimaryBiasD65"]=9;ini["VrAlchemyTransferSpeedD65"]=-5;Load();Read(other);
 assert(close(other.live_d39.crankTravelD66,.08f));
 assert(close(other.punch_minimum_m_d65,.10f)&&close(other.live_d39.twoHandPrimaryBiasD65,.4f)&&close(other.live_d39.alchemySpeedD65,1));
 other.live_d39.knifeRightHipD66=false;other.punch_minimum_m_d65=.2f;other.live_d39.twoHandBiasD65=false;
 other.live_d39.bowAimD65=false;other.live_d39.crossbowReloadD65=false;other.live_d39.pickupClockD65=false;
 Apply(other);Save();Load();Read(other);
 assert(close(other.punch_minimum_m_d65,.2f)&&two_hand_d65::bias()==0);
 assert(!other.live_d39.bowAimD65&&!other.live_d39.crossbowReloadD65&&!other.live_d39.pickupClockD65);
 assert(!other.live_d39.knifeRightHipD66);
 assert(!other.live_d39.pickupCameraBridgeD55); // explicit OFF survives, no INI reset
 other.live_d39.stabFixD77=other.live_d39.unarmedFixD77=other.live_d39.masterstrikeFixD77=other.live_d39.hudWalkoffD70=false;other.live_d39.leanHeightD70=0;other.live_d39.dodgeWindowD70=0;Apply(other);assert(Save());Load();Read(other);
 assert(!other.live_d39.stabFixD77&&!other.live_d39.unarmedFixD77&&!other.live_d39.masterstrikeFixD77&&!other.live_d39.hudWalkoffD70&&other.live_d39.leanHeightD70==0&&other.live_d39.dodgeWindowD70==0);
 // Explicit OFF persists independently, and zero extra prediction is a real zero.
 other.live_d39.handLeadD69=other.live_d39.elbowHysteresisD69=other.live_d39.weaponSameFrameD69=other.live_d39.rigidGripD69=false;
 other.live_d39.handLeadMsD69=0;Apply(other);assert(Save());Load();Read(other);
 assert(!other.live_d39.handLeadD69&&!other.live_d39.elbowHysteresisD69&&!other.live_d39.weaponSameFrameD69&&!other.live_d39.rigidGripD69&&other.live_d39.handLeadMsD69==0);
 ini["VrHandLeadMsD69"]=99;Load();Read(other);assert(other.live_d39.handLeadMsD69==33&&!other.live_d39.handLeadD69);
 ini["VrHandLeadMsD69"]=-1;Load();Read(other);assert(other.live_d39.handLeadMsD69==0&&!other.live_d39.handLeadD69);
 ini["VrHandLeadMsD69"]=std::numeric_limits<double>::quiet_NaN();Load();Read(other);assert(other.live_d39.handLeadMsD69==0); // nonfinite value rejected, current finite setting retained
 // A single saved experiment cannot enable its three neighbors or old D67 options.
 ini["VrWeaponSameFrameD67C"]=1;Load();Read(other);
 assert(other.live_d39.weaponSameFrameD69&&!other.live_d39.handLeadD69&&!other.live_d39.elbowHysteresisD69&&!other.live_d39.rigidGripD69);
 assert(!other.live_d39.gripAnchorD67&&!other.live_d39.elbowContinuityD67&&other.live_d39.motionTrialD67==3);
 // Exact production load adopts only the inherited 1x once, not custom rates.
 ini.clear();ini["VrAlchemyTransferSpeedD65"]=1;Load();Read(other);assert(close(other.live_d39.alchemySpeedD65,4));
 ini["VrAlchemyTransferSpeedD65"]=3;Load();Read(other);assert(close(other.live_d39.alchemySpeedD65,3));
 other.live_d39.alchemySpeedD65=1;Apply(other);assert(Save());assert(ini["VrAlchemyTransferTuningD67Adopted"]==1);Load();Read(other);assert(close(other.live_d39.alchemySpeedD65,1));
 other.live_d39.motionTrialD67=2;other.live_d39.alchemySpeedD65=8;Apply(other);assert(Save());Load();Read(other);assert(other.live_d39.motionTrialD67==2&&close(other.live_d39.alchemySpeedD65,8));
 ini["VrMotionTrialD67"]=1;ini["VrElbowOutwardD67"]=std::numeric_limits<double>::quiet_NaN();ini["VrElbowBackD67"]=9;Load();Read(other);
 assert(other.live_d39.motionTrialD67==0&&close(other.live_d39.elbowOutwardD67,.35f)&&close(other.live_d39.elbowBackD67,.6f));
 // D77 executes the current production READ/APPLY/SAVE/LOAD contributions,
 // not just the standalone migration helper. Neighbor controls stay independent.
 for(unsigned bits=0;bits<8;++bits){
  other.live_d39.stabFixD77=(bits&1)!=0;other.live_d39.unarmedFixD77=(bits&2)!=0;other.live_d39.masterstrikeFixD77=(bits&4)!=0;
  Apply(other);assert(Save());claude_d70::SetCombatFix(false);Load();Read(other);
  assert(other.live_d39.stabFixD77==((bits&1)!=0)&&other.live_d39.unarmedFixD77==((bits&2)!=0)&&other.live_d39.masterstrikeFixD77==((bits&4)!=0));
 }
 ini.clear();ini["VrStabUnarmedMasterstrikeD70"]=1;Load();Read(other);
 assert(other.live_d39.stabFixD77&&other.live_d39.unarmedFixD77&&other.live_d39.masterstrikeFixD77);
 ini["VrUnarmedClinchD77"]=0;Load();Read(other);
 assert(other.live_d39.stabFixD77&&!other.live_d39.unarmedFixD77&&other.live_d39.masterstrikeFixD77);
 assert(Save()&&ini["VrStabUnarmedMasterstrikeD70"]==1);
 io=false;assert(!Save());
 std::puts("D81 actual production setting contributions: NPC life/listening + smith-anchor default ON and all eight independent persistence combinations, failed native preparation, independent changes, D77 three independent combat controls/legacy migration roundtrip + four default-OFF trials/zero extra prediction + exact 1x adoption/custom-rate/explicit1x persistence, explicit OFF, bounds, NaN, unchanged-shot revision and IO failure PASS");
}
