#include "held_map_m1.h"
#include "subsystems/direct/combat_router_h3.h"
#include "vrik/user_modes175.h"
#include "vrik/calibration_restore43.h" // C43
#include "ui_hud_experiment36c5.h"
#include "vrik/combat_rules178r10.h"
#include "vrik/workstation178r20c1.h"
#include "diagnostics/hotkeys168r1.h"
#include "diagnostics/capture_control168.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <limits>
#include <vector>

// Actual mod.cpp clauses below are regenerated before compilation. Only native,
// renderer and worker leaves are mocked; there is no copied hotkey state machine.
namespace kcd2vr::held_map_m1 {bool Install() noexcept {return true;}}
static std::atomic<bool> g_hybrid_default59{false}; // legacy-policy tests use explicit opt-out; new default covered below
static std::atomic<bool> g_held_map59c10r10{false}; // C10R10: normal-play Map off in this fixture (F2 rung 4 path unchanged)
namespace sim175 {
using Mode=kcd2vr::user175::Mode;
using Config=kcd2vr::user175::Configuration;
std::uint64_t now=1000;
Config config{};
std::vector<Mode> applied;
unsigned ensureCalls=0,beeps=0,cues=0,cancelCalibration=0,fingerCaptures=0;
unsigned hudWrites=0,hudSnapshots=0,traceWrites=0,observationWrites=0;
unsigned beginCalibration=0,completeCalibration=0,guidedPose=0,calibrationSamples=0,recenters=0;
unsigned modeAttempts=0;
int lastCue=-1;
std::uint64_t savedCalibrationRevision=0x123456789ull;
bool shiftH3=false;
bool ensureOk=true,ctrl=false,hudRepair=false,hudRepairAvailable=true,combatObserver=false,combatProducer=false,hitProducer=false;
bool calibrationSucceeds=false,calibrationStartedOff=false,calibrationReady=true;
bool interactionReady=true;
bool experimentCorrected=false,desktopMirror=false;
bool equal(Config a,Config b){return a.combat==b.combat&&a.interaction==b.interaction&&a.pose==b.pose&&a.weapon==b.weapon&&a.experiment==b.experiment&&a.physical177==b.physical177&&a.full178r10==b.full178r10&&a.rung178r15==b.rung178r15;}
}
std::uint64_t GetTickCount64() noexcept {return sim175::now;}
static constexpr int VK_SHIFT=16,VK_CONTROL=17,MB_ICONASTERISK=64,MB_OK=0;
short GetAsyncKeyState(int key) noexcept {return ((key==VK_CONTROL&&sim175::ctrl)||(key==VK_SHIFT&&sim175::shiftH3))?short(0x8000):0;}
void* GetModuleHandleW(const wchar_t*) noexcept {return nullptr;}
int MessageBeep(unsigned) noexcept {++sim175::beeps;return 1;}
int Beep(unsigned,unsigned) noexcept {++sim175::beeps;return 1;}
void* SafeGetCryConsole() noexcept {return nullptr;}
void* SafeConsoleGetCVar(void*,const char*) noexcept {return nullptr;}
float SafeCVarGetFloat(void*,float fallback) noexcept {return fallback;}
int SafeCVarGetInt(void*,int fallback) noexcept {return fallback;}
bool SafeCVarSetFloat(void*,float) noexcept {return false;}
bool SafeCVarSetInt(void*,int) noexcept {return false;}

namespace kcd2vr {
namespace diagnostic_trace {void Write(const char*) noexcept {++sim175::traceWrites;}}
namespace retail_build112 {bool Verify(std::uintptr_t) noexcept {return false;}}
namespace activity_fader47c1 {bool Install() noexcept {return false;}}
namespace flashui_probe {
void SetHudExperiment36(bool corrected,bool) noexcept {sim175::experimentCorrected=corrected;}
bool HudRepair175() noexcept {return sim175::hudRepair;}
bool SetHudRepair175(bool on) noexcept {
    ++sim175::hudWrites;
    if(on&&!sim175::hudRepairAvailable)return false;
    sim175::hudRepair=on;return true;
}
void WriteHudCoverage168r1(const char*) noexcept {}
}
namespace hud_layer_v6664 {void SetDesktopMirror36(bool on) noexcept {sim175::desktopMirror=on;}void WriteSnapshot() noexcept {++sim175::hudSnapshots;}}
namespace combat_diag_probe {
bool Install() noexcept {return true;}
unsigned ReadinessMask168r1() noexcept {return 1;}
void SetEnabled(bool on) noexcept {sim175::combatProducer=on;}
void SetVrWeaponHitLogging136(bool on) noexcept {sim175::hitProducer=on;}
}
namespace frik_pose_probe {
bool BodyFollow128() noexcept {return false;}
int HolsterCombatDiagMode() noexcept {return sim175::config.combat;}
int MiscInteractionDiagMode() noexcept {return sim175::config.interaction;}
int CombatCensusMode() noexcept {return sim175::config.pose;}
int WeaponControlMode() noexcept {return sim175::config.weapon;}
bool CombatExperiment175Enabled() noexcept {return sim175::config.experiment;}
bool CombatIntegrated178r22c2Enabled() noexcept {return sim175::config.experiment&&sim175::config.rung178r15==1;} // R28: F4 rung 1
inline bool g_sim_sync59c10r9=false;
bool CombatSynchronized59c10r9() noexcept {return CombatIntegrated178r22c2Enabled()&&g_sim_sync59c10r9;} // R59.16C10R9
inline bool g_sim_intent59c16=false;
bool CombatIntent59c16() noexcept {return CombatSynchronized59c10r9()&&g_sim_intent59c16;} // R59.16C10R16
bool PhysicalCombat177Enabled() noexcept {return sim175::config.experiment&&sim175::config.physical177;}
bool CombatFull178r10Enabled() noexcept {return PhysicalCombat177Enabled()&&sim175::config.full178r10;}
int F4Rung178r10() noexcept {if(CombatIntegrated178r22c2Enabled())return 1;const int r=user175::f4Rung(CombatExperiment175Enabled(),PhysicalCombat177Enabled(),CombatFull178r10Enabled());return r>=3?(sim175::config.rung178r15==7?3:sim175::config.rung178r15==6?2:std::max(3,sim175::config.rung178r15)):r;}
bool CalibrationReady() noexcept {return sim175::calibrationReady;}
inline bool g_sim_stored43=false;inline unsigned g_sim_restores43=0; // C43
bool CalibrationStored43() noexcept {return g_sim_stored43;}
bool RestoreCalibration43(const char*) noexcept {++g_sim_restores43;return true;}
bool ApplyUserMode175(user175::Mode mode) noexcept {
    ++sim175::modeAttempts;
    if(mode!=user175::Mode::Off&&!CalibrationReady())return false;
    if(mode==user175::Mode::Off)workstation178r20c1::setExperiment36(false);
    g_sim_sync59c10r9=mode==user175::Mode::CombatSynchronized||mode==user175::Mode::CombatIntent; // R59.16C10R9 / R16
    g_sim_intent59c16=mode==user175::Mode::CombatIntent;
    sim175::applied.push_back(mode);sim175::config=user175::configuration(mode);return true;
}
unsigned SetCombatObservation168r1(bool on) noexcept {
    ++sim175::observationWrites;sim175::combatObserver=on;return on?1:0;
}
bool InteractionLifecycleActive168() noexcept {return diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64());}
bool SetInteractionLifecycle165(bool on) noexcept {
    diagnostics::capture168::select(diagnostics::capture168::Domain::Interaction,on,GetTickCount64(),"F5");return true;
}
unsigned InteractionCoverage175() noexcept {return 1;}
bool InteractionLifecycleReady36() noexcept {return sim175::interactionReady;}
bool SetInteractionExperiment36(bool on) noexcept {
    const bool armed=on&&InteractionLifecycleReady36()&&CalibrationReady()&&CombatCensusMode()>=3;
    workstation178r20c1::setExperiment36(armed);return !on||armed;
}
// Real API cancellation/gate behavior is also executed by diagnostic_modes175_bridge.
void ServiceInteractionExperiment36() noexcept {
    if(combat_rules178r10::f5Rung==1&&!InteractionLifecycleActive168()){
        combat_rules178r10::f5Rung=0;SetInteractionExperiment36(false);SetInteractionLifecycle165(false);
    }
}
bool ReloadDiagnostics175() noexcept {return diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,GetTickCount64());}
bool SetReloadDiagnostics175(bool on) noexcept {
    diagnostics::capture168::select(diagnostics::capture168::Domain::Reload,on,GetTickCount64(),"F11");return true;
}
// Native mode application is a leaf in this hotkey fixture. The actual physical
// draw/input ownership is executed separately in bow_grip47c2_test.
int reloadMode47c2=0;
int ReloadMode47C2() noexcept {return reloadMode47c2;}
bool SetReloadMode47C2(int mode) noexcept {
    if(mode==2&&!CalibrationReady())return false;
    reloadMode47c2=mode;return SetReloadDiagnostics175(mode!=0);
}
int HandsMode59c7f() noexcept {return hands59c7f::rung.load();}
bool SetHandsMode59c7f(int rung) noexcept {
    const bool wasActive=hands59c7f::active();hands59c7f::clear();hands59c7f::rung=rung;
    diagnostics::capture168::select(diagnostics::capture168::Domain::Hands,rung==1,GetTickCount64(),"F10");
    if(rung==2){SetInteractionExperiment36(false);const bool ok=ApplyUserMode175(user175::Mode::Hands);hands59c7f::admitted=ok;return ok;}
    if(wasActive)ApplyUserMode175(user175::Mode::Off);return true;
}
void CancelArmLengthCalibration() noexcept {++sim175::cancelCalibration;}
void BeginArmLengthCalibration() noexcept {
    ++sim175::beginCalibration;
    sim175::calibrationStartedOff=sim175::equal(sim175::config,user175::configuration(user175::Mode::Off));
}
void BeginGuidedCalibrationPose(int) noexcept {++sim175::guidedPose;}
void RequestCalibrationFromRecenter() noexcept {++sim175::calibrationSamples;}
bool CompleteArmLengthCalibration() noexcept {++sim175::completeCalibration;return sim175::calibrationSucceeds;}
void SetCombatCensusMode(int mode) noexcept {sim175::config.pose=mode;}
void BeginFingerCapture() noexcept {++sim175::fingerCaptures;}
}
inline std::atomic<bool> g_roomscale_follow58{false},g_roomscale_dodge58{false};
struct CameraHook {static void SetRoomscaleFollow(bool) noexcept {} static void RequestRecenter() noexcept {++sim175::recenters;}};
}
using namespace kcd2vr;
namespace cap=diagnostics::capture168;
using Mode=user175::Mode;
struct FakeLog175 {template<class... A> void warning(const char*,A&&...) noexcept {}};
struct FakeMenu175 {
    bool countdown=false,visible=false;
    void SetCaptureCountdown(bool on,int) noexcept {countdown=on;}
    void SetVisible(bool on) noexcept {visible=on;}
};
struct FakeTouch175 {
    int labStage=-1;bool rightGripFreeLook=false;
    void SetVrikInputLabStage(int stage) noexcept {labStage=stage;}
    void SetRightGripFreeLookEnabled(bool on) noexcept {rightGripFreeLook=on;}
};
struct Keys175 {unsigned mask=0;bool shift=false;bool down(unsigned key)const noexcept{return (mask&(1u<<key))!=0;}};
struct Fixture175 {
    ui_hud_experiment36c5::Ladder hudLadder36;
    std::atomic<unsigned> hud_session_rung57c1_{0};
    std::atomic<bool> vr_hud_reapply_requested_{false};
    std::atomic<bool> vr_auto_correct_hud_{false}; // actual Mod worker preference, not a copied clause
    bool f4_down_prev=false,f6_down_prev=false,f7_down_prev=false,f9_down_prev=false;
    bool f10_down_prev=false,f11_down_prev=false,f12_down_prev=false;
    bool f8_cal_key_prev=false;
    bool combat_diag_probe_installed=false;
    int f8_cal_stage=0,vrik_auto_enable_after_cal=0;
    int f8_cal_beep_index=-1,observedF2Requested175=0;
    std::uint64_t f8_cal_stage_ms=0,vrik_cal_complete_until_ms=0;
    std::atomic<int> vrik_calibration_ui_state_{0};
    std::atomic<bool> vrik_recalibrate_requested_{false};
    std::atomic<int> vrik_menu_mode_requested_{-1};
    FakeMenu175 menu{};FakeMenu175* vr_menu_=&menu;
    FakeTouch175 touch_bindings_{};FakeLog175 logger{};
    FakeLog175& log() noexcept {return logger;}
    bool ensure_vrik_lab() noexcept {++sim175::ensureCalls;return sim175::ensureOk;}
    void LabLadderCue(int cue) noexcept {++sim175::cues;sim175::lastCue=cue;}
    void poll(Keys175 keys={},bool matrix_active=false) {
        sim175::shiftH3=keys.shift;
        ++sim175::now;
        const auto ladder_now_ms=GetTickCount64();
        const bool f2_down=keys.down(2),f3_down=keys.down(3),f4_down=keys.down(4),f5_down=keys.down(5);
        const bool f6_down=keys.down(6),f7_down=keys.down(7),f8_down=keys.down(8),f9_down=keys.down(9),shift_down=keys.shift;
        const bool f10_down=keys.down(10),f11_down=keys.down(11),f12_down=keys.down(12);
#include "generated_user_controls175.inc"
        observedF2Requested175=int(hudLadder36.Rung()); // recorder only, no gameplay/HUD repair rung
    }
    void press(unsigned key,bool shift=false){poll({1u<<key,shift});poll();}
    void reset(Mode mode=Mode::Off){
        kcd2vr::routerH3::mode=0;
        poll(); // consume the real function-static edge state before each scenario
        // Return the real function-static F2 cycle to OFF through its own dispatch.
        // No fixture write may hide a retained-rung production regression.
        for(unsigned i=0;i<3&&observedF2Requested175;++i)press(2); // C26: 4-step F2 cycle
        kcd2vr::frik_pose_probe::SetReloadMode47C2(0);
        kcd2vr::combat_rules178r10::f5Rung.store(0); // V66.178R13: F5 rung state follows the recorder; reset with it
        hands59c7f::clear();hands59c7f::rung=0;
        vr_auto_correct_hud_=false;sim175::experimentCorrected=sim175::desktopMirror=false;
        sim175::now=1000;sim175::config=user175::configuration(mode);sim175::applied.clear();
        frik_pose_probe::g_sim_sync59c10r9=mode==Mode::CombatSynchronized||mode==Mode::CombatIntent;
        frik_pose_probe::g_sim_intent59c16=mode==Mode::CombatIntent;

        sim175::ensureCalls=sim175::beeps=sim175::cues=sim175::cancelCalibration=sim175::fingerCaptures=0;
        sim175::hudWrites=sim175::hudSnapshots=sim175::traceWrites=sim175::observationWrites=0;
        sim175::beginCalibration=sim175::completeCalibration=sim175::guidedPose=sim175::calibrationSamples=sim175::recenters=0;
        sim175::modeAttempts=0;sim175::calibrationReady=true;
        sim175::interactionReady=true;workstation178r20c1::setExperiment36(false);
        sim175::calibrationSucceeds=sim175::calibrationStartedOff=false;sim175::lastCue=-1;
        sim175::savedCalibrationRevision=0x123456789ull;sim175::hudRepairAvailable=true;
        sim175::ensureOk=true;sim175::ctrl=sim175::hudRepair=sim175::combatObserver=sim175::combatProducer=sim175::hitProducer=false;
        for(unsigned d=0;d<cap::DomainCount;++d){cap::gate.select(cap::Domain(d),false,sim175::now);cap::gate.session[d]=0;}
        f4_down_prev=f6_down_prev=f7_down_prev=f9_down_prev=f10_down_prev=f11_down_prev=f12_down_prev=false;
        f8_cal_key_prev=false;f8_cal_stage_ms=vrik_cal_complete_until_ms=0;f8_cal_beep_index=-1;vrik_recalibrate_requested_=false;vrik_menu_mode_requested_=-1;
        combat_diag_probe_installed=false;f8_cal_stage=vrik_auto_enable_after_cal=0;vrik_calibration_ui_state_=0;menu.countdown=false;
    }
};
static unsigned checks=0,failures=0;
static void ck(bool ok,const char* name){++checks;if(!ok)++failures;std::printf("%s %s\n",ok?"PASS":"FAIL",name);}
static bool modeIs(Mode mode){
    const bool synchronized=mode==Mode::CombatSynchronized||mode==Mode::CombatIntent;
    return sim175::equal(sim175::config,user175::configuration(mode))&&
        frik_pose_probe::g_sim_sync59c10r9==synchronized&&
        frik_pose_probe::g_sim_intent59c16==(mode==Mode::CombatIntent);
}
static bool active(cap::Domain d){return cap::active(d,sim175::now);}
int main(){
    Fixture175 f;
    const Mode modes[]{Mode::Off,Mode::Calibrated,Mode::Hands,Mode::Weapons,Mode::Combat,Mode::CombatIntegrated};
    for(auto mode:modes)for(bool preferences:{false,true}){
        f.reset(mode);sim175::hudRepair=preferences;f.press(2);
        ck(active(cap::Domain::Hud)&&sim175::hudRepair==preferences&&modeIs(mode)&&sim175::applied.empty(),"F2 first press records without changing saved manual HUD ownership");
        for(int j=0;j<8;++j)f.poll({1u<<2});f.poll(); // fresh edge off, held key stays off
        ck(active(cap::Domain::Hud)&&f.hudLadder36.Rung()==2&&sim175::hudRepair==preferences&&sim175::hudWrites==0,"F2 held press advances once to corrected HUD without changing saved preferences");
        f.press(2);ck(active(cap::Domain::Hud)&&f.hudLadder36.Rung()==3&&kcd2vr::held_map_m1::Requested()&&modeIs(mode),"F2 third edge selects palm HUD and requests the VR map without changing VRIK (C26)");
        f.press(2);ck(!active(cap::Domain::Hud)&&f.hudLadder36.Rung()==0&&!kcd2vr::held_map_m1::Requested(),"F2 fourth edge restores baseline and stops map and recorder (C26)");
        f.press(2);sim175::now=cap::gate.until[unsigned(cap::Domain::Hud)].load();f.poll();
        ck(!active(cap::Domain::Hud)&&sim175::hudRepair==preferences&&sim175::hudWrites==0&&modeIs(mode),"F2 timeout cannot disable the production visibility writer");
    }
    // Execute the actual F2 dispatch with both saved AutoHUD preferences.
    // Rung 1 recording and rung 0 OFF must not clear a retained HUD owner.
    for(bool preference:{false,true}){
        f.reset();f.vr_auto_correct_hud_=preference;
        for(unsigned expected:{1u,2u,3u,0u}){
            f.press(2);const bool corrected=true;(void)preference; /* C34: corrected HUD on every rung (rung 1 used to drop it -> black square at startup mirror) */
            ck(f.hudLadder36.Rung()==expected&&sim175::experimentCorrected==corrected&&sim175::desktopMirror==corrected,
               "actual F2 clause keeps the corrected HUD on every rung (C34)");
        }
    }
    const unsigned diagnosticKeys[]{3,5,11};
    const cap::Domain domains[]{cap::Domain::Combat,cap::Domain::Interaction,cap::Domain::Reload};
    for(auto mode:modes)for(unsigned d=0;d<3;++d){
        f.reset(mode);const auto key=diagnosticKeys[d];const auto domain=domains[d];
        f.poll({1u<<key});const auto session=cap::gate.session[unsigned(domain)].load();
        for(unsigned repeat=0;repeat<12;++repeat)f.poll({1u<<key});
        ck(active(domain)&&cap::gate.session[unsigned(domain)]==session&&modeIs(mode)&&sim175::applied.empty(),"F3/F5/F11 held key starts one recorder session without changing gameplay");
        f.poll();if(key==5){f.press(5);ck(active(domain)&&kcd2vr::combat_rules178r10::f5Rung==2&&modeIs(mode)&&sim175::applied.empty(),"R36C4 second F5 press selects gated rung 2 with recorder on and VRIK selection unchanged");}
        if(key==5){f.press(5);ck(active(domain)&&kcd2vr::combat_rules178r10::f5Rung==3&&modeIs(mode)&&sim175::applied.empty(),"C55 third F5 edge selects guided crafting without changing VRIK or starting another recorder");}
        if(key==11){f.press(11);ck(active(domain)&&frik_pose_probe::ReloadMode47C2()==2,"F11 second deliberate edge selects Grip draw experiment");}
        f.press(key);
        ck(!active(domain)&&modeIs(mode)&&sim175::applied.empty(),"F3/F5/F11 explicit off leaves current gameplay untouched");
        f.press(key);sim175::now=cap::gate.until[unsigned(domain)].load();f.poll();
        ck(!active(domain)&&!(cap::gate.mask.load()&(1u<<unsigned(domain)))&&modeIs(mode)&&sim175::applied.empty(),"F3/F5/F11 expiry cannot restore an old gameplay tuple");
        if(key==3)ck(!sim175::combatProducer&&!sim175::hitProducer,"expired F3 disables production observation logging on the same control poll");
    }
    for(auto mode:modes){
        f.reset(mode);f.press(10);
        ck(modeIs(mode)&&active(cap::Domain::Hands)&&sim175::applied.empty(),"F10 rung 1 records without changing gameplay");
        f.press(10);ck(modeIs(Mode::Hands)&&cap::gate.mask==0,"F10 rung 2 selects hands and ends its own recorder");
        f.press(10);ck(modeIs(Mode::Off)&&cap::gate.mask==0&&frik_pose_probe::HandsMode59c7f()==0,"third F10 edge turns off");
    }
    f.reset();f.poll({1u<<10});for(int j=0;j<15;++j)f.poll({1u<<10});
    ck(modeIs(Mode::Off)&&sim175::applied.empty()&&active(cap::Domain::Hands),"held F10 starts only one diagnostic rung");
    f.poll();f.press(10);ck(modeIs(Mode::Hands)&&sim175::applied.size()==1,"second edge enters hands once");
    sim175::ensureOk=false;f.press(10);ck(modeIs(Mode::Off)&&f.touch_bindings_.rightGripFreeLook,"F10 off works with install unavailable and releases Grip");
    f.reset();sim175::calibrationReady=false;f.press(10);f.press(10);
    ck(modeIs(Mode::Off)&&frik_pose_probe::HandsMode59c7f()==2&&!hands59c7f::admitted,"failed calibration never admits hands");
    f.press(10);ck(frik_pose_probe::HandsMode59c7f()==0,"blocked rung 2 still cycles off");

    f.reset();f.press(10);f.press(10);
    f.press(10,true);ck(modeIs(Mode::Hands)&&frik_pose_probe::HandsMode59c7f()==2&&active(cap::Domain::Hands)&&!active(cap::Domain::Combat),"Shift F10 records actual hands experiment without F3");
    f.press(10,true);ck(modeIs(Mode::Hands)&&!active(cap::Domain::Hands),"Shift F10 stops only its own recorder");

    f.reset(Mode::Weapons);f.press(12);
    ck(kcd2vr::routerH3::mode==1&&modeIs(Mode::Weapons)&&sim175::applied.empty(),"H3 F12 observation keeps VRIK/weapon ownership");
    f.press(12);ck(kcd2vr::routerH3::mode==2&&modeIs(Mode::Weapons),"H3 F12 second edge selects native timing only");
    f.press(12);ck(kcd2vr::routerH3::mode==0&&modeIs(Mode::Weapons),"H3 F12 third edge turns experiment off without dropping weapon");
    f.reset(Mode::Hands);f.press(12,true);ck(modeIs(Mode::Weapons)&&cap::gate.mask==0,"F12 explicitly selects physical weapons from hands");
    f.press(12,true);ck(modeIs(Mode::WeaponsHands40)&&cap::gate.mask==0,"F12 second press adds existing free-hand contacts to physical weapons");
    f.press(12,true);ck(modeIs(Mode::Calibrated)&&cap::gate.mask==0,"F12 third press returns calibrated handling without restoring prior hands");
    // C4R2: retain only the user-accepted synchronized hybrid and direct lab.
    f.press(10);f.press(10);f.press(4);ck(modeIs(Mode::CombatSynchronized)&&cap::gate.mask==0&&sim175::lastCue==1&&sim175::config.rung178r15==1&&!frik_pose_probe::CombatIntent59c16(),"first F4 selects synchronized hybrid (old press3) without recording");
    f.press(4);ck(modeIs(Mode::CombatContactLab)&&cap::gate.mask==0&&sim175::lastCue==2,"second F4 selects direct lab (old press4) without recording");
    f.press(4);ck(modeIs(Mode::Weapons)&&cap::gate.mask==0&&sim175::lastCue==0,"third F4 releases combat and retains normal VRIK weapons");
    f.press(4,true);ck(modeIs(Mode::CombatSynchronized)&&sim175::lastCue==1,"Shift F4 cannot revive removed native comparisons");
    f.press(4,true);ck(modeIs(Mode::CombatContactLab)&&sim175::lastCue==2,"Shift F4 follows same two-mode cycle");
    f.press(4,true);ck(modeIs(Mode::Weapons)&&sim175::lastCue==0,"Shift F4 also leaves normal handling on");
    f.reset(Mode::CombatIntegrated);f.press(4);ck(modeIs(Mode::CombatSynchronized),"legacy state enters retained hybrid rather than an obsolete rung");
    f.press(4);f.press(4);ck(modeIs(Mode::Weapons),"short cycle ends with combat off and handling on");
    f.press(4);f.press(12,true);ck(modeIs(Mode::Weapons),"F12 after F4 replaces the experiment with explicit physical weapons");
    f.press(12,true);ck(modeIs(Mode::WeaponsHands40),"interleaved F4/F12 state selects weapon plus hand contacts, not old combat");
    f.press(12,true);ck(modeIs(Mode::Calibrated),"F12 third press cannot resurrect earlier experiment");

    for(unsigned cycle=0;cycle<3;++cycle){
        f.reset();f.press(4);f.press(4); // C4R2: hybrid -> direct, held F4 below -> OFF
        sim175::ensureOk=false;sim175::calibrationReady=false;const auto installs=sim175::ensureCalls;
        f.poll({1u<<4});for(int i=0;i<10;++i)f.poll({1u<<4});f.poll();
        ck(modeIs(Mode::Off)&&sim175::lastCue==0&&sim175::ensureCalls==installs,
           "F4 OFF bypasses failed installation/calibration and a held key cannot restart it");
    }
    for(bool ready:{false,true})for(auto mode:{Mode::Off,Mode::Calibrated}){
        f.reset(mode);sim175::interactionReady=ready;f.press(5);f.press(5);
        ck(combat_rules178r10::f5Rung==2&&workstation178r20c1::ownsWrist(sim175::now)==(ready&&mode!=Mode::Off),
           "F5 rung 2 has a separate gate: no missing coverage or VRIK-off takeover");
        sim175::interactionReady=true;f.poll();
        ck(workstation178r20c1::ownsWrist(sim175::now)==(ready&&mode!=Mode::Off),"later readiness cannot silently arm a blocked F5 selection");
        sim175::ensureOk=false;sim175::calibrationReady=false;f.press(5);
        ck(combat_rules178r10::f5Rung==3&&!workstation178r20c1::ownsWrist(sim175::now)&&modeIs(mode),
           "C55 blocked guided rung cannot bypass calibration or change VRIK");
        f.press(5);
        ck(combat_rules178r10::f5Rung==0&&!active(cap::Domain::Interaction)&&!workstation178r20c1::ownsWrist(sim175::now)&&modeIs(mode),
           "C55 fourth F5 edge stops even a blocked experiment, without installing or changing VRIK");
    }
    f.reset(Mode::Calibrated);f.press(5);f.press(5);
    combat_rules178r10::useHeld=true;combat_rules178r10::strokeUntil=sim175::now+140;
    sim175::now=cap::gate.until[unsigned(cap::Domain::Interaction)].load();
    ck(workstation178r20c1::ownsWrist(sim175::now),"F5 gameplay survives the recorder deadline");
    f.poll();ck(combat_rules178r10::f5Rung==2&&workstation178r20c1::ownsWrist(sim175::now),
        "expired recorder preserves explicitly armed F5 gameplay");
    f.press(5);ck(combat_rules178r10::f5Rung==3&&modeIs(Mode::Calibrated),"C55 explicit guided selection remains reachable after recorder expiry");
    f.press(5);ck(combat_rules178r10::f5Rung==0&&!workstation178r20c1::ownsWrist(sim175::now)&&
        !combat_rules178r10::useHeld&&!combat_rules178r10::strokeUntil,"F5 after recorder expiry still turns gameplay off and releases outputs");
    f.press(5);ck(combat_rules178r10::f5Rung==1&&!workstation178r20c1::ownsWrist(sim175::now),"next F5 edge starts diagnostics only");

    // V66.178R10: F11 needs two edges for its recorder (rung 1 = workstation hands).
    auto pressRecorders=[&](){for(auto key:diagnosticKeys)f.press(key);};
    f.reset(Mode::Hands);pressRecorders();
    const auto recordingMask=cap::gate.mask.load();f.press(4);f.press(12,true);f.press(10);f.press(10);
    ck(modeIs(Mode::Hands)&&cap::gate.mask==recordingMask,"gameplay shortcuts leave all armed recorder leases unchanged");
    sim175::now+=cap::Duration+1;f.poll();
    ck(modeIs(Mode::Hands)&&cap::gate.mask==0,"simultaneous recorder expiry preserves a gameplay choice made after recording began");

    const unsigned keys[]{2,3,4,5,10,11,12};
    for(auto key:keys){
        f.reset();f.poll({1u<<key},true);f.poll({1u<<key},false);
        ck(modeIs(Mode::Off)&&sim175::applied.empty()&&cap::gate.mask==0&&!sim175::hudRepair,"modal-consumed key edge is not replayed when the modal owner closes");
        f.poll();f.poll({1u<<key});
        ck(!sim175::applied.empty()||cap::gate.mask!=0||sim175::hudRepair||kcd2vr::routerH3::mode.load()!=0,"release and deliberate new key edge still work after modal ownership");
    }
    for(auto key:diagnosticKeys){
        f.reset(Mode::Weapons);sim175::ensureOk=false;f.press(key);
        ck(modeIs(Mode::Weapons)&&sim175::applied.empty(),"diagnostic installation failure cannot alter existing gameplay ownership");
    }
    for(auto key:{4u,10u,12u}){
        f.reset();f.f8_cal_stage=2;f.vrik_auto_enable_after_cal=1;f.vrik_calibration_ui_state_=4;f.menu.countdown=true;f.press(key,key==12);
        ck(sim175::cancelCalibration==1&&!f.f8_cal_stage&&!f.vrik_auto_enable_after_cal&&!f.vrik_calibration_ui_state_&&!f.menu.countdown,
           "explicit gameplay shortcut cancels pending calibration without a delayed stale mode write");
    }
    const auto leases=[](){std::array<std::uint64_t,cap::DomainCount> value{};
        for(unsigned i=0;i<cap::DomainCount;++i)value[i]=cap::gate.until[i].load();return value;};
    for(bool cancel:{true,false}){
        f.reset(Mode::Weapons);pressRecorders();f.press(2);f.press(4,true);
        const auto existingLeases=leases();const auto saved=sim175::savedCalibrationRevision;
        ck(modeIs(Mode::CombatSynchronized)&&sim175::config.experiment,"calibration regression enters the retained synchronized F4 backend even with Shift");
        f.press(8,true);
        ck(modeIs(Mode::Off)&&!sim175::config.experiment&&f.f8_cal_stage==1&&sim175::beginCalibration==1&&sim175::calibrationStartedOff,
           "F4 to Shift+F8 applies all-off before the shared calibration begin leaf");
        ck(leases()==existingLeases&&cap::gate.mask==15&&sim175::savedCalibrationRevision==saved,
           "calibration start preserves all recorder leases and saved calibration sentinel");
        if(cancel){
            f.press(8,true);
            ck(modeIs(Mode::Off)&&!sim175::config.experiment&&!f.f8_cal_stage&&!f.menu.countdown&&sim175::cancelCalibration==1,
               "Shift+F8 cancel after F4 leaves experiment and all internal gameplay modes off");
        }else{
            sim175::now=f.f8_cal_stage_ms+3000;f.poll();
            ck(f.f8_cal_stage==2&&modeIs(Mode::Off)&&sim175::guidedPose==1&&sim175::calibrationSamples==1,
               "actual calibration countdown requests capture while F4 remains off");
            sim175::now=f.f8_cal_stage_ms+2000;f.poll();
            ck(modeIs(Mode::Off)&&!sim175::config.experiment&&!f.f8_cal_stage&&sim175::completeCalibration==1&&f.vrik_calibration_ui_state_==6,
               "failed calibration after F4 preserves all-off instead of retaining the old combat tuple");
            sim175::now=f.vrik_cal_complete_until_ms;f.poll();
            ck(modeIs(Mode::Off)&&f.vrik_calibration_ui_state_==0,"failed calibration UI expiry cannot resurrect F4 gameplay");
        }
        ck(leases()==existingLeases&&cap::gate.mask==15&&sim175::savedCalibrationRevision==saved,
           "calibration cancel/failure preserves existing recorder leases and saved calibration sentinel");
    }
    f.reset(Mode::Combat);f.vrik_recalibrate_requested_=true;f.poll();
    ck(modeIs(Mode::Off)&&f.f8_cal_stage==1&&sim175::beginCalibration==1&&sim175::calibrationStartedOff&&sim175::applied.size()==1,
       "menu recalibration uses the same all-off begin lambda without a duplicate mode mutation");
    f.reset(Mode::Combat);f.press(8);
    ck(modeIs(Mode::Combat)&&sim175::beginCalibration==0,"plain F8 remains reserved and cannot start calibration");
    f.poll({1u<<8,true},true);f.poll({1u<<8,true},false);
    ck(modeIs(Mode::Combat)&&sim175::beginCalibration==0,"modal Shift+F8 is consumed and cannot replay calibration on close");
    f.poll();f.press(8,true);
    ck(modeIs(Mode::Off)&&sim175::beginCalibration==1,"fresh Shift+F8 edge after modal ownership still begins calibration safely");
    f.reset();sim175::calibrationReady=false;f.press(3);f.press(11);
    const auto menuLeases=leases();f.vrik_menu_mode_requested_=5;f.poll();
    ck(modeIs(Mode::Off)&&sim175::modeAttempts==1&&sim175::applied.empty()&&sim175::beeps==1&&
       sim175::beginCalibration==0&&!f.f8_cal_stage&&f.vrik_calibration_ui_state_==0&&leases()==menuLeases,
       "positive menu VRIK choice without calibration reports blocked and never starts hidden capture");
    f.press(8,true);
    ck(modeIs(Mode::Off)&&sim175::beginCalibration==1&&f.f8_cal_stage==1&&leases()==menuLeases,
       "missing calibration begins only after a separate explicit Shift+F8 request");
    // C43 (Miki: "switch to VRIK ... loads the calibration from the .ini"): a stored profile is applied, no T-pose.
    f.reset();frik_pose_probe::g_sim_stored43=true;const auto restores43=frik_pose_probe::g_sim_restores43;f.vrik_menu_mode_requested_=2;f.poll();
    ck(modeIs(Mode::Calibrated)&&sim175::beginCalibration==0&&!f.f8_cal_stage&&sim175::recenters==1&&frik_pose_probe::g_sim_restores43==restores43+1,
       "C43: menu VRIK with a stored calibration applies CALIBRATED_HANDLING at once (recenter + restore receipt), no T-pose");
    f.reset();frik_pose_probe::g_sim_stored43=false;f.vrik_menu_mode_requested_=2;f.poll();
    ck(modeIs(Mode::Off)&&sim175::beginCalibration==1&&f.f8_cal_stage==1,"C43: menu VRIK without a stored calibration still starts the T-pose");
    // C59 executes the actual calibration/menu entry + F4 clauses. New policy
    // is a default, not a recording lease or unconditional calibration bypass.
    f.reset();g_hybrid_default59=true;frik_pose_probe::g_sim_stored43=true;f.vrik_menu_mode_requested_=2;f.poll();
    ck(modeIs(Mode::CombatSynchronized),"C59 VRIK enable enters approved HYBRID default");
    f.press(3);const auto capture59=leases();f.press(4);
    ck(modeIs(Mode::CombatContactLab)&&leases()==capture59,"C59 F4 comparison enables DIRECT without touching diagnostics");
    f.press(4);ck(modeIs(Mode::CombatSynchronized)&&leases()==capture59,"C59 F4 comparison OFF restores HYBRID and VRIK");
    g_hybrid_default59=false;
    std::printf("user_controls175=%u/%u source=EXTRACTED_MOD_CPP native_leaves=MOCKED\n",checks-failures,checks);
    return failures?1:0;
}
