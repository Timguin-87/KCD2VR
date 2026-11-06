// The Mod read/apply/save/load contributions and shared setters below are
// extracted verbatim from production by the D39 runner. No Windows/game calls.
#if defined(__GNUC__)
#pragma GCC diagnostic push
// Existing MSVC-targeted headers put multiple guarded statements on one line.
// Keep compiling them unchanged; new fixture/adapter code retains -Werror.
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif
#include "subsystems/experimental/live_settings_d39.h"
#include "subsystems/experimental/deepseek_options_h29.h"
#include "subsystems/alchemy/reach_policy_craft1.h"
#include "vrik/physical_weapon.h"
#include "vrik/shoulder_reach_d38.h"
#include "vrik/visual_selection59c7f.h"
#include "vrik/smooth_lab_f2.h"
#include "vrik/station_ws_j27.h"
#include "vrik/camera_hold_e1.h"
#include "vrik/bow_arm_aim_r9.h"
#include "vrik/lab58.h"
#include "vrik/utility_gestures54.h"
#include "vrik/weapon_release_t1.h"
#include "vrik/native_supply_t2.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#include <cassert>
#include <cstdio>
#include <string>
#include <unordered_map>

namespace kcd2vr {
namespace frik_pose_probe {
namespace ph=kcd2vr::physical;
std::atomic<std::uint64_t> g_contact_control_epoch6695{1};
std::atomic<bool> g_native_hand_physics_h24{false};
namespace handPresenceH24 {unsigned cancelled=0;void CancelPresenceH24(int){++cancelled;}}
#include "live_shared_setters_d39_production.inc"
}
namespace grip_stow_cb1 {
std::atomic<bool> enabled{false};std::atomic<int> button{0};std::atomic<std::uint64_t> generation{1};
#include "live_stow_setters_d39_production.inc"
}
namespace workstation178r20c1 {std::atomic<bool> gesturesEverywhere59c2{true};}
namespace alchemy_own_head_h51 {std::atomic<bool> enabled{false};}
namespace held_map_m1 {std::atomic<bool> selected{false};bool Selected(){return selected.load();}}
namespace action_time_owner59c11 {
#include "live_supply_setters_d39_production.inc"
}
std::atomic<bool> g_held_map59c10r10{true},g_held_map_apply_d39{false};
struct Menu {live_settings_d39::Settings live_d39{};int weapon_release_mode_t1=0;bool throwing_supply_t2=false;};
static void Fill(Menu& out){
#include "live_mod_read_d39_production.inc"
}
static void Apply(const Menu& state){
#include "live_mod_apply_d39_production.inc"
}
static void ApplyThrow(const Menu& state){
#define KCD2VR_M2_HOST_SLOT 76
#include "subsystems/throw_drop/host.inl"
#undef KCD2VR_M2_HOST_SLOT
}
using Ini=std::unordered_map<std::string,double>;
static Ini ini;
static bool ReadIniBoolExplicit(const char*,const char*,const char* key,bool fallback){const auto found=ini.find(key);return found==ini.end()?fallback:found->second!=0;}
static int ReadIniIntExplicit(const char*,const char*,const char* key,int fallback){const auto found=ini.find(key);return found==ini.end()?fallback:int(found->second);}
static float ReadIniFloatExplicit(const char*,const char*,const char* key,float fallback){const auto found=ini.find(key);return found==ini.end()?fallback:float(found->second);}
static void Load(){const char* explicit_ini="fixture.ini";
#include "live_mod_load_d39_production.inc"
}
static void Save(){bool ok=true;
    auto wb=[](const char* key,bool value){ini[key]=value?1:0;return true;};
    auto wi=[](const char* key,int value){ini[key]=value;return true;};
    auto wf=[](const char* key,float value){ini[key]=value;return true;};
#include "live_mod_save_d39_production.inc"
    assert(ok);
}
}
int main(){
    using namespace kcd2vr;namespace frik=frik_pose_probe;
    ini={{"VrPreciseSurfaceContacts",0}};Load();assert(frik::HandsModeDs2()==0);
    ini={{"VrHandsModeDs2",2},{"VrPreciseSurfaceContacts",1},{"VrShoulderReachMaxM",.1},{"VrShoulderReachWarningM",.3},
        {"VrTorchGestureD39",0},{"VrVisorGestureD39",1},{"VrAlchemyPickupReachD39",1}};
    Load();assert(frik::HandsModeDs2()==2&&!frik::PreciseContacts44());
    assert(std::fabs(frik::GetShoulderReachD38()-.3f)<1.e-5f);
    Menu read{};Fill(read);assert(read.live_d39.handsMode==2&&!read.live_d39.torchGesture&&read.live_d39.visorGesture&&read.live_d39.alchemyPickupReach==1);
    const auto contact=frik::g_contact_control_epoch6695.load();
    const auto smooth=hand_smooth_h50::policyEpoch.load();
    const auto stow=grip_stow_cb1::generation.load();
    read.live_d39.handSeparation=false;read.live_d39.handSmoothing=true;
    read.live_d39.gripStowTrial=true;read.live_d39.gripStowButton=1;
    read.live_d39.parchmentMap=false;read.live_d39.smithToolCollision=false;
    read.live_d39.cameraHoldMs=9000;read.live_d39.stationClearCm=99;
    Apply(read);
    smooth_lab::RefreshBaselineSmoothingD39(); // actual Run-owner refresh
    assert(frik::g_contact_control_epoch6695.load()==contact+1);
    assert(hand_smooth_h50::policyEpoch.load()==smooth+1);
    assert(grip_stow_cb1::generation.load()==stow+2); // enable and remap each retire their old receipt
    assert(g_held_map_apply_d39.exchange(false)&&!g_held_map59c10r10.load());
    Fill(read);assert(!read.live_d39.handSeparation&&read.live_d39.handSmoothing&&!read.live_d39.smithToolCollision);
    assert(read.live_d39.gripStowTrial&&read.live_d39.gripStowButton==1);
    assert(read.live_d39.cameraHoldMs==5000&&read.live_d39.stationClearCm==30);
    Apply(read);assert(frik::g_contact_control_epoch6695.load()==contact+1&&hand_smooth_h50::policyEpoch.load()==smooth+1);
    assert(grip_stow_cb1::generation.load()==stow+2&&!g_held_map_apply_d39.load());
    // A runtime hotkey owns the next visible value; no stale UI/INI shadow.
    frik::SetHandsModeDs2(0);Fill(read);assert(read.live_d39.handsMode==0);
    read.live_d39.handsMode=2;read.live_d39.torchGesture=true;read.live_d39.visorGesture=false;
    read.live_d39.alchemyPickupReach=0;read.live_d39.parchmentMap=true;Apply(read);
    assert(g_held_map_apply_d39.load());Fill(read);assert(read.live_d39.parchmentMap&&!read.live_d39.parchmentReady);
    Save();assert(ini.at("VrHandsModeDs2")==2&&ini.at("VrPreciseSurfaceContacts")==0);
    assert(ini.at("VrTorchGestureD39")==1&&ini.at("VrVisorGestureD39")==0);
    assert(ini.at("VrAlchemyPickupReachD39")==0&&ini.at("VrCameraBodyHoldMs")==5000);
    assert(ini.count("VrShoulderReachWarningM")&&ini.count("VrHandSeparationDs1"));
    frik::SetHandsModeDs2(1);utility54::SetTorchGestureD39(false);utility54::SetVisorGestureD39(true);
    Load();Fill(read);assert(read.live_d39.handsMode==2&&read.live_d39.torchGesture&&!read.live_d39.visorGesture);
    // F2 overrides effectiveness, never the stored user preference. Menu OFF
    // survives leaving a comparison and loading an older legacy ON key.
    smooth_lab::SetMode(3);Fill(read);assert(read.live_d39.handSmoothing&&read.live_d39.handSmoothingPaused&&!hand_smooth_h50::enabled.load());
    Save();assert(ini.at("VrHandSmoothH50")==1);
    read.live_d39.handSmoothing=false;Apply(read);smooth_lab::RefreshBaselineSmoothingD39();smooth_lab::SetMode(0);
    assert(!hand_smooth_h50::enabled.load()&&!smooth_lab::BaselineSmoothingD39());
    ini["VrHandSmoothH50"]=0;ini["VrSmoothHands"]=1;Load();assert(!smooth_lab::BaselineSmoothingD39());
    ini.erase("VrHandSmoothH50");Load();assert(smooth_lab::BaselineSmoothingD39());
    // Absent canonical warning key still accepts D38's numerical preference,
    // which is now diagnostics only. The saved canonical value wins otherwise.
    ini={{"VrShoulderReachMaxM",.2}};Load();assert(std::fabs(frik::GetShoulderReachD38()-.2f)<1.e-5f);
    // Stock refresh belongs to the shared apply owner, not just one old button.
    inventory_lifecycle_t8::state=4;read.throwing_supply_t2=true;read.weapon_release_mode_t1=1;
    ApplyThrow(read);assert(action_time_owner59c11::ThrowingSupplyT2()&&inventory_lifecycle_t8::state.load()==6);
    assert(weapon_release_t1::TestEnabledH21());
    ApplyThrow(read);assert(inventory_lifecycle_t8::state.load()==6);
    read.throwing_supply_t2=false;read.weapon_release_mode_t1=0;ApplyThrow(read);Fill(read);
    assert(!action_time_owner59c11::ThrowingSupplyT2()&&!weapon_release_t1::TestEnabledH21()&&inventory_lifecycle_t8::state.load()==6);
    std::puts("D39 actual Mod read/apply/save/load + shared setters: mode roundtrip, aliases, owner values, epochs, independent utilities, map intent queue PASS");
}
