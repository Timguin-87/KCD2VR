#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdio>
#include "vrik/user_modes175.h"
#include "vrik/hands_mode59c7f.h"
#include "vrik/calibration_restore43.h"

// The extracted public mode and F10 functions are production. Native hook
// readiness and the unrelated full combat-mode installer are test doubles.
using Mode=kcd2vr::user175::Mode;
static int weapon=7,pose=4,coreCalls=0,prepareCalls=0,weaponEpoch=19;
static bool calibrated=true,nativeReady=true,resources=true;
static std::atomic<bool> g_normal_fists44{false},g_synchronized59c10r9{true},
    g_intent59c16{true},g_intent_pi59c16{true};
namespace diagnostic_trace {void Write(const char*){}}
namespace combat_diag_probe {void SetSynchronized59c10r9(bool){} void SetIntent59c16(bool){}}
static bool FullWeaponMode120(int m){return m==6||m==7;}
static int WeaponControlMode(){return weapon;}
static int CombatCensusMode(){return pose;}
static void SetCombatCensusMode(int m){pose=m;}
static bool CalibrationReady(){return calibrated;}
static bool NativeArm104(){return nativeReady;}
static bool ArmCombatInput101(bool){return nativeReady;}
static bool ArmCloseCombat175(){return nativeReady;}
static bool PrepareInteractionResources59c7f(){++prepareCalls;return resources;}
static bool ApplyUserModeCore175(Mode m,Mode){
    ++coreCalls;
    if(!calibrated)return false;
    const auto c=kcd2vr::user175::configuration(m);
    pose=c.pose;weapon=c.weapon;++weaponEpoch;return true;
}
#include "hand_mode_functions_r95.inc"
static void reset(){weapon=7;pose=4;coreCalls=prepareCalls=0;weaponEpoch=19;
    calibrated=nativeReady=resources=true;g_normal_fists44=false;
    g_synchronized59c10r9=g_intent59c16=g_intent_pi59c16=true;
    kcd2vr::hands59c7f::clear();kcd2vr::hands59c7f::rung=0;}
int main(){
    using namespace kcd2vr;
    for(auto m:{Mode::Hands,Mode::WeaponsHands40}){
        auto c=user175::configuration(m);
        assert(c.pose==4&&FullWeaponMode120(c.weapon));
    }
    assert(calibration_restore43::selectionFromTuple(4,6,false)==Mode::Calibrated);
    for(int owner:{3,4,5,6,7}){
        reset();weapon=owner;
        assert(SetHandsMode59c7f(2));
        assert(weapon==owner&&pose==4&&coreCalls==0&&weaponEpoch==19);
        assert(g_normal_fists44&&g_synchronized59c10r9&&g_intent59c16&&g_intent_pi59c16);
        assert(hands59c7f::active()&&hands59c7f::pickupReady&&prepareCalls==1);
        hands59c7f::grabMask=3;hands59c7f::sampleTick=100;
        assert(SetHandsMode59c7f(0));
        assert(weapon==owner&&pose==4&&coreCalls==0&&weaponEpoch==19);
        assert(!hands59c7f::active()&&hands59c7f::grabMask==0);
        assert(g_synchronized59c10r9&&g_intent59c16);
    }
    reset();nativeReady=false;
    assert(!SetHandsMode59c7f(2));
    assert(weapon==7&&pose==4&&weaponEpoch==19&&coreCalls==0);
    assert(!hands59c7f::active());
    nativeReady=true;assert(SetHandsMode59c7f(2));
    assert(hands59c7f::active()&&weaponEpoch==19);
    reset();resources=false;
    assert(SetHandsMode59c7f(2));
    assert(hands59c7f::active()&&!hands59c7f::pickupReady&&weapon==7);
    resources=true;assert(SetHandsMode59c7f(2));assert(hands59c7f::pickupReady);
    reset();weapon=pose=0;
    assert(SetHandsMode59c7f(2));assert(pose==4&&weapon==6&&coreCalls==1);
    // Changing/clearing a hand experiment invalidates its old grip identity.
    auto epoch=hands59c7f::epoch.load();hands59c7f::grabMask=1;
    assert(SetHandsMode59c7f(1));
    assert(hands59c7f::epoch>epoch&&hands59c7f::grabMask==0&&weapon==6);
    std::puts("PASS: production hand selection preserves active holster/combat owner, OFF scope and retry");
}
