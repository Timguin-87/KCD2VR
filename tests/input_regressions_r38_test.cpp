// Production-function replay for the October 28 lost-input regressions.
#include "vrik/physical_weapon.h"
#include "vrik/utility_gestures54.h"
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <string>
#include <unordered_map>

namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
namespace equipment114=kcd2vr::equipment114;
static std::uint64_t nowR38=1000;
static std::uint64_t GetTickCount64(){return nowR38;}
static unsigned checks=0,failed=0;
static void check(bool ok,const char* label){++checks;if(!ok){++failed;std::printf("FAIL %s\n",label);}}

struct PhysEquipmentItem6698 {ph::Profile profile{};bool held=false;int set=0,slot126=0;};
struct PhysEquipment6698 {
    std::array<PhysEquipmentItem6698,8> items{};int count=0,selected=0;
    bool handKnown[2]{true,true};std::uint64_t handKey[2]{};
};
struct TestGripR38 {
    std::uint64_t key=0,generation=0;bool mountReady=false,ready[2]{};
    struct {std::uint64_t epoch=0,tick=0;}mountStamp126;
    ph::Pose mount{},hand[2]{};
};
struct PhysGripBank6698 {std::array<TestGripR38,8>grip{};};
static bool PhysSupplyHeldT2(const PhysEquipment6698&,const PhysEquipmentItem6698&,int){return false;}
static std::atomic<float>g_holster_radius136{.28f};
#include "r38_holster_helpers.inc"

static bool SwitchCandidate(const PhysEquipment6698&e,const ph::Output&out,int i){
    PhysEquipment6698 freeHS2=e;freeHS2.handKey[0]=freeHS2.handKey[1]=0;
    const bool anyHeld=e.handKey[0]||e.handKey[1];
    const int hand=ph::holster_hand126(e.items[i].profile);
    bool switchHS2[8]{};
#include "r38_switch_clause.inc"
    return switchHS2[i];
}
static bool SwitchLost(const PhysEquipment6698&e,const ph::Input&in,const ph::WeaponSwitchHS2&sw,
    bool rightFenceH36=false,bool leftFenceH36=false,unsigned doorAdmissionH40=3){
    const int h=sw.hand>=0&&sw.hand<2?sw.hand:0;
#include "r38_switch_lost.inc"
    return switchLostR15;
}
static PhysEquipment6698 SwordAndBow(){
    PhysEquipment6698 e{};e.count=2;e.selected=0;e.handKey[0]=100;
    auto&s=e.items[0];s.profile.key=100;s.profile.generation=1;s.profile.side=0;s.profile.classId=4;s.held=true;s.set=0;
    auto&b=e.items[1];b.profile.key=200;b.profile.generation=1;b.profile.side=1;b.profile.classId=9;b.set=1;
    return e;
}
static void Holsters(){
    auto e=SwordAndBow();ph::Output out{};
    check(!PhysDrawCandidate126(e,1),"normal bow draw cannot bypass a held sword");
    check(SwitchCandidate(e,out,1),"free left hand can start stow-then-draw while right holds represented sword");
    out.owned[1]=true;check(!SwitchCandidate(e,out,1),"occupied physical support hand cannot switch equipment");out.owned[1]=false;
    e.handKey[0]=999;check(!SwitchCandidate(e,out,1),"unrepresented held key cannot be stowed by a switch");e.handKey[0]=100;
    e.handKey[1]=999;check(!SwitchCandidate(e,out,1),"occupied grabbing hand cannot switch equipment");e.handKey[1]=0;
    e.handKnown[1]=false;check(!SwitchCandidate(e,out,1),"unknown grabbing hand cannot switch equipment");e.handKnown[1]=true;
    e.items[0].held=false;e.handKey[0]=0;
    check(PhysDrawCandidate126(e,1),"native stow acknowledgement admits the ordinary bow draw");
    check(!SwitchCandidate(e,out,1),"already free native hands do not stage unnecessary stows");

    e=SwordAndBow();ph::Input in{};in.allowed=true;in.epoch=in.bodyEpoch=1;in.tick=1000;
    in.tracked[0]=in.tracked[1]=in.intentTracked[0]=in.intentTracked[1]=true;
    in.shoulders[0]={.20,0,1.4};in.shoulders[1]={-.20,0,1.4};
    ph::Vec3 mount{};ph::holster_mount126(in,e.items[1].profile,mount);in.hands[1].p=mount;
    in.grip[1]=in.trigger[1]=1;in.requirePrimaryGrip114=true;
    std::array<ph::HolsterState,8> states{};PhysGripBank6698 bank{};bool prospective[8]{};
    prospective[1]=SwitchCandidate(e,out,1);
#if R38_HAVE_SWITCH_PRIORITY
    const auto owners=PhysOverShoulderGrabJ28(in,e,bank,states,prospective);
#else
    const auto owners=PhysOverShoulderGrabJ28(in,e,bank,states);
#endif
    check((owners&2u)!=0,"qualified switch acquisition keeps Grip+Trigger from becoming a fist");
    in.hands[1].p={-.2,.6,1.4};
#if R38_HAVE_SWITCH_PRIORITY
    check(!PhysOverShoulderGrabJ28(in,e,bank,states,prospective),"a switch candidate cannot claim a distant punching hand");
#else
    check(!PhysOverShoulderGrabJ28(in,e,bank,states),"a switch candidate cannot claim a distant punching hand");
#endif

    ph::WeaponSwitchHS2 sw{};sw.active=true;sw.key=200;sw.generation=1;sw.body=in.bodyEpoch;sw.tick=in.tick;sw.hand=1;
    kcd2vr::corpse_grip_cg1::claims.capture(3,sw.corpseClaimCg1);
    check(!SwitchLost(e,in,sw),"accepted switch retains Grip ownership when Trigger is also closed");
    check(ph::switch_step_hs2(sw,in.bodyEpoch,in.tick,true,false,false,false,1,true,true)==ph::SwitchStepHS2::Wait,
        "accepted switch waits for native stow acknowledgement");
    e.items[0].held=false;e.handKey[0]=0;
    check(ph::switch_step_hs2(sw,in.bodyEpoch,in.tick,true,false,true,PhysDrawCandidate126(e,1),1,true,true)==ph::SwitchStepHS2::Draw,
        "native empty hands plus current drawable key completes the switch");
    in.grip[1]=0;check(SwitchLost(e,in,sw),"real release cancels an accepted switch");in.grip[1]=1;
    in.intentTracked[1]=false;check(SwitchLost(e,in,sw),"source loss cancels an accepted switch");in.intentTracked[1]=true;
    check(SwitchLost(e,in,sw,false,false,1),"door ownership cancels a competing left-hand switch");
    check(SwitchLost(e,in,sw,false,true),"a current hand fence cancels the switch");
    kcd2vr::corpse_grip_cg1::claims.consumed[0].store(true);
    check(SwitchLost(e,in,sw),"new corpse ownership cancels the original switch claim");
    kcd2vr::corpse_grip_cg1::claims.consumed[0].store(false);
}

struct XrPose {float pos_x=0,pos_y=0,pos_z=0,qx=0,qy=0,qz=0,qw=1;};
struct ControllerState {XrPose pose{};bool active=true;float trigger=0,grip=0;};
namespace native_vr_pad_premap {
struct State {float lt=1;};
struct Context {bool vr_menu=false,theater=false,system_menu=false,dialogue_navigation=false,semantic_minigame=false,apse_ui=false,back_context=false;};
}
namespace diagnostics::capture168 {enum class Domain{Combat,Interaction};static bool active(Domain,std::uint64_t){return true;}}
namespace diagnostic_trace {static void Write(const char*){}}
namespace animation_filter {static std::atomic<bool>nativeBodyOwned112{false};}
static std::atomic<bool>g_gesture_talk59c5{false};
namespace frik_pose_probe {
static bool calibrated=true;static int leftItem=0,physicalHeld=0;static unsigned torch=0,visorOpen=0,visorClose=0;static bool shoulderSource=false;
static bool CalibrationReady(){return calibrated;}
static int UtilityLeftItem54(){return leftItem;}
static int UtilityLeftItemR114(std::uint64_t){return leftItem;}
static int ReadPhysicalHandHeld55(int,std::uint64_t){return physicalHeld;}
static bool RequestTorch62(bool shoulder,std::uint64_t){shoulderSource=shoulder;++torch;return true;}
static bool RequestVisor54(bool close,std::uint64_t){close?++visorClose:++visorOpen;return true;}
}
#include "r38_utility_host.inc"

static void Utilities(){
    native_vr_pad_premap::Context c{};g_gesture_frame59c5.valid=true;
    auto&hand=g_gesture_frame59c5.hands[1];
    auto step=[&](float x,float y,float z,float trigger,float grip=0.f){
        nowR38+=20;hand.pose.pos_x=x;hand.pose.pos_y=y;hand.pose.pos_z=z;hand.trigger=trigger;hand.grip=grip;
        native_vr_pad_premap::State pad{};ApplyUtilityGestures54(pad,c,true);return pad.lt;};
    step(-.2f,-.25f,.2f,0);step(-.2f,-.25f,.2f,1);
    for(int n=0;n<10;++n)step(-.2f,-.25f,.2f,1);
    check(frik_pose_probe::torch==1,"saved Quick Talk OFF does not disable shoulder torch gesture");
    check(frik_pose_probe::shoulderSource,"utility torch keeps its spatial source");
    const auto torchBefore=frik_pose_probe::torch;
    for(int n=0;n<10;++n)step(-.2f,-.25f,.2f,1);
    check(frik_pose_probe::torch==torchBefore,"a held trigger cannot toggle the torch twice");
    step(-.15f,0,0,0);step(-.15f,0,0,1);step(-.15f,.11f,0,1);
    check(frik_pose_probe::visorOpen==1,"saved Quick Talk OFF does not disable upward visor gesture");
    step(-.15f,0,0,0);step(-.15f,0,0,1);step(-.15f,-.11f,0,1);
    check(frik_pose_probe::visorClose==1,"downward visor gesture remains independent of Quick Talk");
    const auto visorBefore=frik_pose_probe::visorOpen;
    step(-.15f,0,0,0);step(-.15f,0,0,1,1);step(-.15f,.11f,0,1,1);
    check(frik_pose_probe::visorOpen==visorBefore,"combat Grip+Trigger is not stolen by visor gesture");
    step(-.15f,0,0,0);c.semantic_minigame=true;step(-.15f,0,0,1);step(-.15f,.11f,0,1);
    check(frik_pose_probe::visorOpen==visorBefore,"native crafting controls suppress unrelated utility gesture");
    c.semantic_minigame=false;step(-.15f,0,0,0);frik_pose_probe::physicalHeld=1;step(-.15f,0,0,1);step(-.15f,.11f,0,1);
    check(frik_pose_probe::visorOpen==visorBefore,"two-hand support cannot raise visor as an empty hand");frik_pose_probe::physicalHeld=0;
    check(!g_gesture_talk59c5.load(),"utility gestures leave saved Quick Talk preference untouched");
}

static std::unordered_map<std::string,bool>ini;
static std::atomic<bool>g_held_map59c10r10{true};
static bool ReadIniBoolExplicit(const char*,const char*,const char*key,bool fallback){auto i=ini.find(key);return i==ini.end()?fallback:i->second;}
static bool LoadMap(){const char*explicit_ini="test";
#include "r38_map_config.inc"
    return g_held_map59c10r10.load();
}
static void MapPreference(){
    ini.clear();ini["VrHeldMap"]=true;check(LoadMap(),"actual supplied VrHeldMap=true selects parchment after key rename");
    ini["VrHeldMap"]=false;check(!LoadMap(),"saved legacy map OFF is preserved");
    ini["VrHeldMap"]=true;ini["VrHeldParchmentMapDs7"]=false;check(!LoadMap(),"explicit newer map OFF takes precedence");
    ini["VrHeldMap"]=false;ini["VrHeldParchmentMapDs7"]=true;check(LoadMap(),"explicit newer map ON takes precedence");
}
static ph::Vec3 PV(ph::Vec3 value){return value;}
static void StableGestureAnchor(){
    ph::Input pi{};pi.shoulders[0]={.6,.2,1.4};pi.shoulders[1]={-.5,-.1,1.4};
    const ph::Vec3 gestureShouldersR38[2]{{.2,0,1.4},{-.2,0,1.4}};
    const ph::Vec3 modelIntentGoalsH30[2]{{.7,.2,1.4},{-.7,.2,1.4}};
#include "r38_holster_adapter.inc"
    check(pm::length(holsterInput.shoulders[0]-gestureShouldersR38[0])<1e-9&&pm::length(holsterInput.shoulders[1]-gestureShouldersR38[1])<1e-9,
        "shoulder reach accommodation cannot drag torso holster anchors");
    check(pm::length(holsterInput.hands[0].p-modelIntentGoalsH30[0])<1e-9,"gesture hand retains authoritative controller target");
}
int main(){Holsters();Utilities();MapPreference();StableGestureAnchor();std::printf("R38 input regressions: %u checks, %u failures\n",checks,failed);return failed?1:0;}
