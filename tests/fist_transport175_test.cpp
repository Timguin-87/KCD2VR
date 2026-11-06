// Regression of the production pretransport filter through the actual mod call
// clauses and BOTH complete input adapters. No copied trigger decision is used.
#include <windows.h>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>
#include <vector>
#include "../src/frik_pose_probe.h"
#include "../src/diagnostics/lab_state.h"
#include "../src/ui_menu_router.h"
#include "../src/ui_menu_input.h"
#include "../src/activity_input163.h"
#include "../src/core/reading_pages42.h"
#include "../src/core/book_pages45.h" // C45: page arbiter used by the extracted XI block
#include "../src/core/book_gesture46.h"
#include "../src/vrik/book_input46.h"
#include "../src/vrik/semi_workstations46.h"
#include "../src/diagnostics/interaction_receipt59c10.h"
#include "../src/diagnostics/capture168.h"
#include "../src/action_map_probe.h"
#include "../src/subsystems/hud_ui_controls/runtime/quick_talk_native_h19.h"
#include "../src/vrik/attack_xi178r8.h" // V66.178R8: the extracted XI payload block applies the attack transport
#include "../src/vrik/combat_rules178r10.h" // V66.178R10: R3 special hold / F11 workstation inputs on the XI pad
#include "../src/vrik/workstation178r20c1.h"
#include "../src/vrik/vr_combat_ownership111.h"

static std::vector<INPUT> sent175;
static UINT InputSpy175(UINT n, INPUT* inputs, int) {
    sent175.insert(sent175.end(), inputs, inputs + n);
    return n;
}
constexpr unsigned VK_F24 = 0x87;
#define SendInput InputSpy175
#include "../src/touch_bindings.cpp"
#include "../src/native_vr_pad_premap.cpp"

static kcd2vr::native_vr_pad_premap::State published175;
static unsigned publishes175 = 0;
static bool bowOwns175 = false;
static bool specialAllowed175 = false;
static bool f4GestureOwned176 = false;
namespace kcd2vr::diagnostic_trace { void Write(const char*) noexcept {} }
namespace kcd2vr::interaction_alignment { bool LastHandCandidate(HandCandidate& out) noexcept {out={};return false;} } // combat-only fixture: real offer pipeline is exercised by guided_crafting55_test
static std::uint64_t g_stallCp178r30[5]{}; // R30: loop checkpoint array referenced by the extracted pad block
namespace kcd2vr::combat_diag_probe { bool GeometryLockClick178r29(std::uint64_t) noexcept {return false;} bool GeometryRipostePress59c8(std::uint64_t) noexcept {return false;} } // R29: no geometry engage in this fixture
static void ApplyRoomscalePad58(kcd2vr::native_vr_pad_premap::State&,const kcd2vr::native_vr_pad_premap::Context&,bool) noexcept {} // Roomscale has its own production-kernel tests.
static void ApplyUtilityGestures54(kcd2vr::native_vr_pad_premap::State&,const kcd2vr::native_vr_pad_premap::Context&,bool) noexcept {} // Utility producer and native-frame boundary have separate focused tests.
#include "../src/vrik/gesture_talk59c5.h"
static kcd2vr::gesture_talk59c5::Output PrepareGestureTalkH4(const kcd2vr::native_vr_pad_premap::Context&,bool) noexcept {return {};} // No chat in this transport fixture; actual adapter+holster composed in quick_talk_h4 tests.
namespace kcd2vr::frik_pose_probe {void PublishQuickTalkOwnerH4(unsigned,std::uint64_t) noexcept {}}
struct GestureFrame59c5 {bool valid=false;kcd2vr::XrPose head{};kcd2vr::ControllerState hands[2]{};}; // C45: surrounding mod.cpp frame read by the page flick; book_pages45_test covers the recognizer
static GestureFrame59c5 g_gesture_frame59c5{};
// Combat fixture has no native chat map; preserve the real Open predicate.
namespace kcd2vr::quick_talk_native_h19 { Snapshot Read() noexcept { return {}; } }
namespace kcd2vr::action_map_probe {
bool BackContext() noexcept { return false; }
bool ChatOpen59c5() noexcept { return false; } // this fixture has no native quick-talk map
std::uint64_t Transitions() noexcept { return 0; }
}
namespace kcd2vr::native_input_look_lab {
void PublishGamepadState(float lx,float ly,float rx,float ry,float lt,float rt,
    bool a,bool b,bool x,bool y,bool lb,bool rb,bool l3,bool r3,bool start,bool back,
    bool up,bool down,bool left,bool right) noexcept {
    published175 = {lx,ly,rx,ry,lt,rt,a,b,x,y,lb,rb,l3,r3,start,back,up,down,left,right};
    ++publishes175;
}
bool NativeLeftStickReady() noexcept { return true; }
bool NativeRightStickReady() noexcept { return true; }
void ArmTransportHeartbeat() noexcept {}
}

namespace kcd2vr::frik_pose_probe {
// Combat-only fixture: no committed crafting palm or haptic endpoint. The
// actual source bridge has its own complete-TU and alchemy tests.
bool ReadWorkstationPalm67(int,float& x,float& y,float& z,std::uint64_t& tick,
    std::uint64_t& pose,std::uint64_t& source,std::uintptr_t& player) noexcept {
    x=y=z=0;tick=pose=source=0;player=0;return false;
}
bool ReadWorkstationPalmAimH21(int,float& x,float& y,float& z,std::uint64_t& tick,
    std::uint64_t& pose,std::uint64_t& source,std::uintptr_t& player,float (&direction)[3]) noexcept {
    direction[0]=direction[1]=direction[2]=0;x=y=z=0;tick=pose=source=0;player=0;return false;
}
bool HoverHaptic178r14(int,std::uint64_t) noexcept {return false;}
// This combat-only caller fixture has no native torch owner. The actual
// request/consumer is exercised separately by native_torch62_test.
bool RequestTorch62(bool,std::uint64_t) noexcept {return false;}
// Mutable surrounding engine state, not substitute ownership predicates.
static std::atomic<bool> g_normal_fists44{false};
static std::atomic<int> g_census_mode{0}, g_weapon_control_mode{0};
static std::atomic<bool> g_combat_integrated178r22c2{false},g_combat_experiment175{false}; // R28: fists permitted in every F4 rung
static std::atomic<std::uint64_t> g_f4_fists_tick178r22c2{0},g_f4_fists_epoch178r22c2{0},g_pose_epoch6689{1};
static std::atomic<std::uint64_t> g_phys_input_tick6698{0};
static std::atomic<bool> g_phys_fault6698{false}, g_gameplay_allowed{true}, g_phys_input6698{false};
#include "generated_fist_transport175.inc"
// External physical bow lifecycle and native request leaves; not under test.
bool PhysicalWeaponConsumesRightTrigger139() noexcept { return bowOwns175; }
bool BowGripDrawEnabled596C1() noexcept { return true; } // configured default; exact mode admission below
// Copied equipment ownership is a leaf here; complete-TU F4 tests exercise it.
bool F4GestureInputOwned176() noexcept { return f4GestureOwned176; }
void SetNativeSpecialContext168(bool allowed) noexcept { specialAllowed175 = allowed; }
}

using namespace kcd2vr;
using CleanMenuOwner = ui_menu_router::Owner;

static unsigned Mouse175(DWORD flags) {
    unsigned n=0;
    for(const auto& event:sent175)
        if(event.type==INPUT_MOUSE && event.mi.dwFlags==flags) ++n;
    return n;
}
static unsigned Key175(unsigned scan,bool down) {
    unsigned n=0;
    for(const auto& event:sent175)
        if(event.type==INPUT_KEYBOARD && event.ki.wScan==scan &&
           !(event.ki.dwFlags&KEYEVENTF_KEYUP)==down) ++n;
    return n;
}

struct Output175 {
    ControllerState left{}, right{};
    native_vr_pad_premap::State pad{};
    TouchBindings::Events events{};
};
static std::atomic<std::uint64_t> g_apse_close_press_ms{0}; // C25: surrounding mod.cpp state written by the extracted pad block
struct Session175 {
    bool xr_head_valid=true;
    struct { bool gameplay=true; } palmInput36;
    TouchBindings touch_bindings_;
    workstation178r20c1::Controller workstationC1; // surrounding worker state used by the real extracted XI caller
    semi_workstations46::State semiWorkstations46;
    workstation_sample46::Frame semiFrame46{}; // explicit absent workstation tracking in combat transport cases
    bool native_xi_full=false, mis_menu_after_frame=false, immersive_cursor_menu=false;
    bool handUseAllowed59c10=false; // surrounding F5 selection owner; exercised by input_repair175
    bool dialogue_active=false, policy_theater_scene=false;
    activity_input163::State activity163{};
    ui_menu_router::Decision ui89_policy{};
    ui_menu_input::Result ui88_input_result{};
    std::uint64_t native_xi_heartbeat_ms=0;

    explicit Session175(bool xi):native_xi_full(xi) {
        using namespace frik_pose_probe;
        g_census_mode=0;g_weapon_control_mode=0;g_combat_integrated178r22c2=false;g_combat_experiment175=false;g_f4_fists_tick178r22c2=0;g_pose_epoch6689=1;
        g_phys_fault6698=false;g_gameplay_allowed=true;g_phys_input6698=false;
        bowOwns175=false;f4GestureOwned176=false;diagnostics::lab::Set(diagnostics::lab::Key::F3,0);
        // Rearm the actual function-local latches through their production API.
        FistTransportTrigger101(0,0,true,false,0);
        FistTransportTrigger101(1,0,true,false,0);
        native_vr_pad_premap::Reset();
        sent175.clear();
    }
    Output175 Tick(ControllerState left,ControllerState right) {
        hostTickCount64+=20;
        sent175.clear();published175={};publishes175=0;
        auto routeL=left,routeR=right;
#include "generated_fist_transport_caller175.inc"
        assert(publishes175==(native_xi_full?1u:0u));
        return {routeL,routeR,published175,touch_events};
    }
};
#undef SendInput

static ControllerState Hand175(){ControllerState hand{};hand.active=true;return hand;}
static void NoMouse175(){
    assert(Mouse175(MOUSEEVENTF_LEFTDOWN)==0&&Mouse175(MOUSEEVENTF_LEFTUP)==0);
    assert(Mouse175(MOUSEEVENTF_RIGHTDOWN)==0&&Mouse175(MOUSEEVENTF_RIGHTUP)==0);
}
static void Block175(const Output175& out,bool xi,bool down,bool edge) {
    assert(out.left.trigger==(down?1.f:0.f));
    if(xi){assert(out.pad.lt==(down?1.f:0.f));NoMouse175();}
    else {
        assert(Mouse175(down?MOUSEEVENTF_RIGHTDOWN:MOUSEEVENTF_RIGHTUP)==(edge?1u:0u));
        assert(Mouse175(down?MOUSEEVENTF_RIGHTUP:MOUSEEVENTF_RIGHTDOWN)==0);
        assert(Mouse175(MOUSEEVENTF_LEFTDOWN)==0&&Mouse175(MOUSEEVENTF_LEFTUP)==0);
    }
}
static void Suppressed175(const Output175& out,bool xi) {
    assert(out.left.trigger==0&&out.right.trigger==0);
    if(xi)assert(out.pad.lt==0&&out.pad.rt==0);
    NoMouse175();
}

int main() {
    assert(frik_pose_probe::BowHandlingMode59c9(4)&&frik_pose_probe::BowHandlingMode59c9(5));
    assert(!frik_pose_probe::FullWeaponMode120(4)&&!frik_pose_probe::FullWeaponMode120(5));
    assert(!frik_pose_probe::BowHandlingMode59c9(0)&&!frik_pose_probe::BowHandlingMode59c9(3));
    using namespace frik_pose_probe;
    for(bool xi:{false,true}) {
        { // Fresh exact-Henry clinch playback restores both real input transports.
            Session175 session(xi);auto l=Hand175(),r=Hand175();
            g_weapon_control_mode=7;g_combat_experiment175=true;f4GestureOwned176=true;
            l.trigger=r.trigger=1;
            Suppressed175(session.Tick(l,r),xi);
            vr_combat111::clinchInputTick55c1=hostTickCount64;
            const auto out=session.Tick(l,r);
            assert(out.left.trigger==1&&out.right.trigger==1);
            if(xi)assert(out.pad.lt==1&&out.pad.rt==1);
            else assert(Mouse175(MOUSEEVENTF_LEFTDOWN)==1);
            vr_combat111::clinchInputTick55c1=0;
            const auto ended=session.Tick(l,r);
            assert(ended.left.trigger==0&&ended.right.trigger==0);
            l.trigger=r.trigger=0;session.Tick(l,r);
        }
        { // Integrated boxing retains weapon/holster mode; real neutral rearm still required.
            Session175 session(xi);auto l=Hand175(),r=Hand175();
            g_census_mode=4;g_weapon_control_mode=7;g_combat_integrated178r22c2=true;g_combat_experiment175=true;
            g_f4_fists_epoch178r22c2=1;g_f4_fists_tick178r22c2=hostTickCount64;
            session.Tick(l,r);l.grip=l.trigger=r.trigger=1;
            Suppressed175(session.Tick(l,r),xi);
            hostTickCount64+=81; // a stale empty-hand observation cannot keep owning input
            Suppressed175(session.Tick(l,r),xi); // held input quarantined
            l.grip=l.trigger=r.trigger=0;session.Tick(l,r);
            r.trigger=1;auto out=session.Tick(l,r);assert(out.right.trigger==1);
            r.trigger=0;session.Tick(l,r);
        }

        { // Explicit F5=2 yields boxing input without disabling its contact solver.
            Session175 session(xi);g_census_mode=5;
            assert(FistSelected101());
            combat_rules178r10::f5Rung=2;combat_rules178r10::workstationEnabled=true;
            workstation178r20c1::setExperiment36(true);assert(!FistSelected101());
            workstation178r20c1::setExperiment36(false);assert(FistSelected101());
            combat_rules178r10::f5Rung=0;
        }
        // Native fallback LT alone is a native block in both transports. Right Grip is not
        // left Grip: exercise the real mod arguments, hold, release, and repeat.
        {
            Session175 session(xi);auto l=Hand175(),r=Hand175();
            g_census_mode=0;session.Tick(l,r);
            l.trigger=1;r.grip=1;
            Block175(session.Tick(l,r),xi,true,true);
            for(int i=0;i<8;++i)Block175(session.Tick(l,r),xi,true,false);
            l.trigger=0;Block175(session.Tick(l,r),xi,false,true);
            Block175(session.Tick(l,r),xi,false,false);
            l.trigger=1;Block175(session.Tick(l,r),xi,true,true);
            l.trigger=0;Block175(session.Tick(l,r),xi,false,true);
        }
        // H4 owns both analog triggers before Grip closes: otherwise LT arms a
        // native kick during the approach to a deliberate trigger+Grip punch.
        {
            Session175 session(xi);auto l=Hand175(),r=Hand175();g_census_mode=5;
            session.Tick(l,r);l.trigger=1;
            for(int i=0;i<8;++i)Suppressed175(session.Tick(l,r),xi);
        }
        // Deliberate left Grip+LT and RT belong to fists. Dropping Grip while LT
        // remains held must not invent a native block press on either transport.
        {
            Session175 session(xi);auto l=Hand175(),r=Hand175();
            g_census_mode=5;session.Tick(l,r);
            l.grip=1;l.trigger=1;r.trigger=1;
            Suppressed175(session.Tick(l,r),xi);
            l.grip=0;
            for(int i=0;i<8;++i)Suppressed175(session.Tick(l,r),xi);
            g_census_mode=0; // Exit physical boxing before native trigger rearm.
            l.trigger=0;Suppressed175(session.Tick(l,r),xi);
            l.trigger=1;Block175(session.Tick(l,r),xi,true,true);
            l.trigger=0;Block175(session.Tick(l,r),xi,false,true);
        }
        // Tracking loss is not a trigger release. A real tracked neutral sample
        // must precede a fresh LT block; left tracking is the caller argument.
        {
            Session175 session(xi);auto l=Hand175(),r=Hand175();
            g_census_mode=5;l.grip=l.trigger=1;
            Suppressed175(session.Tick(l,r),xi);
            l.grip=0;l.trigger=0;l.active=false;
            for(int i=0;i<3;++i)Suppressed175(session.Tick(l,r),xi);
            l.active=true;l.trigger=1;
            Suppressed175(session.Tick(l,r),xi);
            g_census_mode=0; // Exit physical boxing before native trigger rearm.
            l.trigger=0;Suppressed175(session.Tick(l,r),xi);
            l.trigger=1;Block175(session.Tick(l,r),xi,true,true);
            l.trigger=0;Block175(session.Tick(l,r),xi,false,true);
        }
        // Mode exit cannot convert an already held fist gesture to stock block
        // or stock attack, even after an untracked zero sample.
        {
            Session175 session(xi);auto l=Hand175(),r=Hand175();
            g_census_mode=5;l.grip=l.trigger=r.trigger=1;
            Suppressed175(session.Tick(l,r),xi);
            g_census_mode=0;l.grip=0;
            Suppressed175(session.Tick(l,r),xi);
            l.active=r.active=false;l.trigger=r.trigger=0;
            Suppressed175(session.Tick(l,r),xi);
            l.active=r.active=true;l.trigger=r.trigger=1;
            Suppressed175(session.Tick(l,r),xi);
            l.trigger=r.trigger=0;Suppressed175(session.Tick(l,r),xi);
            l.trigger=r.trigger=1;
            const auto out=session.Tick(l,r);
            assert(out.left.trigger==1&&out.right.trigger==1);
            if(xi){assert(out.pad.lt==1&&out.pad.rt==1);NoMouse175();}
            else assert(Mouse175(MOUSEEVENTF_RIGHTDOWN)==1&&Mouse175(MOUSEEVENTF_LEFTDOWN)==1);
            l.trigger=r.trigger=0;session.Tick(l,r);
            if(!xi)assert(Mouse175(MOUSEEVENTF_RIGHTUP)==1&&Mouse175(MOUSEEVENTF_LEFTUP)==1);
        }
        // Real native menu and mod menu bypass the boxing filter. The real pad
        // adapter still owns pointer suppression and mod-menu blanking.
        for(bool modMenu:{false,true}) {
            Session175 session(xi);auto l=Hand175(),r=Hand175();
            g_census_mode=5;l.grip=l.trigger=r.trigger=1;
            Suppressed175(session.Tick(l,r),xi);
            session.mis_menu_after_frame=modMenu;
            session.ui88_input_result.blocks_gameplay=!modMenu;
            if(!modMenu){session.ui89_policy.owner=CleanMenuOwner::InventoryMap;
                session.ui89_policy.input=ui_menu_router::Input::MenuNavigation;}
            auto out=session.Tick(l,r);
            assert(out.left.trigger==1&&out.right.trigger==1&&!specialAllowed175);
            NoMouse175();
            if(xi)assert(out.pad.lt==(modMenu?0.f:1.f)&&out.pad.rt==(modMenu?0.f:1.f));
            if(!modMenu&&xi){session.ui88_input_result.hand=0;out=session.Tick(l,r);
                assert(out.left.trigger==1&&out.pad.lt==0&&out.pad.rt==1);}
            session.mis_menu_after_frame=false;session.ui89_policy={};session.ui88_input_result={};
            l.grip=0;Suppressed175(session.Tick(l,r),xi);
            g_census_mode=0; // Exit physical boxing before native trigger rearm.
            l.trigger=0;Suppressed175(session.Tick(l,r),xi);
            l.trigger=1;Block175(session.Tick(l,r),xi,true,true);
            l.trigger=0;Block175(session.Tick(l,r),xi,false,true);
        }
        // Accepted authored activity owns raw analog triggers/Grips despite F10.
        // Fallback has no claimed XI parity; it must not emit combat mouse/Q.
        {
            Session175 session(xi);auto l=Hand175(),r=Hand175();
            g_census_mode=5;session.activity163.nativeControls=true;
            l.grip=r.grip=1;l.trigger=.8f;r.trigger=.9f;
            for(int i=0;i<8;++i){const auto out=session.Tick(l,r);
                assert(out.left.trigger==.8f&&out.right.trigger==.9f&&!specialAllowed175);
                NoMouse175();assert(Key175('Q',true)==0);
                if(xi)assert(out.pad.lt==.8f&&out.pad.rt==.9f&&out.pad.lb&&out.pad.rb);}
            session.activity163.nativeControls=false;l.grip=0;
            Suppressed175(session.Tick(l,r),xi);
            g_census_mode=0; // Exit physical boxing before native trigger rearm.
            l.trigger=0;Suppressed175(session.Tick(l,r),xi);
            l.trigger=1;Block175(session.Tick(l,r),xi,true,true);
            l.trigger=0;Block175(session.Tick(l,r),xi,false,true);
        }
        // F4 uses physical melee posture/motion even before a fresh producer
        // frame. Both actual game transports receive zero native LT/RT; no Grip
        // is needed for that exclusion, and tracking loss cannot reopen it.
        {
            Session175 session(xi);auto l=Hand175(),r=Hand175();
            g_weapon_control_mode=7;f4GestureOwned176=true;
            l.trigger=r.trigger=1;
            for(int i=0;i<8;++i)Suppressed175(session.Tick(l,r),xi);
            l.active=r.active=false;l.trigger=r.trigger=0;
            Suppressed175(session.Tick(l,r),xi);
            l.active=r.active=true;l.trigger=r.trigger=1;
            Suppressed175(session.Tick(l,r),xi);
            // F4 exit does not replay the held controller input as a stock hit.
            f4GestureOwned176=false;
            Suppressed175(session.Tick(l,r),xi);
            l.trigger=r.trigger=0;Suppressed175(session.Tick(l,r),xi);
            l.trigger=r.trigger=1;const auto out=session.Tick(l,r);
            assert(out.left.trigger==1&&out.right.trigger==1);
            if(xi){assert(out.pad.lt==1&&out.pad.rt==1);NoMouse175();}
            else assert(Mouse175(MOUSEEVENTF_RIGHTDOWN)==1&&Mouse175(MOUSEEVENTF_LEFTDOWN)==1);
            l.trigger=r.trigger=0;session.Tick(l,r);
        }
        // F4 does not replace authored menu or minigame controls. Returning to
        // gameplay remains gesture-only, independent of the recorder state.
        for(bool activity:{false,true}) {
            Session175 session(xi);auto l=Hand175(),r=Hand175();
            g_weapon_control_mode=7;f4GestureOwned176=true;
            l.trigger=.8f;r.trigger=.9f;
            if(activity)session.activity163.nativeControls=true;
            else {session.ui89_policy.owner=CleanMenuOwner::InventoryMap;
                session.ui89_policy.input=ui_menu_router::Input::MenuNavigation;
                session.ui88_input_result.blocks_gameplay=true;}
            auto out=session.Tick(l,r);
            assert(out.left.trigger==.8f&&out.right.trigger==.9f);
            if(xi)assert(out.pad.lt==.8f&&out.pad.rt==.9f);
            NoMouse175();
            session.activity163={};session.ui89_policy={};session.ui88_input_result={};
            Suppressed175(session.Tick(l,r),xi);
        }
        // Physical-weapon selection alone must not swallow stock crossbow/gun
        // RT. The extracted owner freshness check applies equally to both rungs.
        for(int mode:{6,7}) {
            Session175 session(xi);auto l=Hand175(),r=Hand175();
            g_weapon_control_mode=mode;session.Tick(l,r);
            r.trigger=1;auto out=session.Tick(l,r);
            assert(out.right.trigger==1);
            if(xi){assert(out.pad.rt==1);NoMouse175();}
            else assert(Mouse175(MOUSEEVENTF_LEFTDOWN)==1);
            r.trigger=0;session.Tick(l,r);
            g_phys_input6698=true;g_phys_input_tick6698=hostTickCount64;
            r.trigger=1;out=session.Tick(l,r);
            assert(out.right.trigger==0);NoMouse175();
            // A stale owner stops consuming, but held input still needs release.
            hostTickCount64+=200;out=session.Tick(l,r);
            assert(out.right.trigger==0);NoMouse175();
            r.trigger=0;session.Tick(l,r);r.trigger=1;out=session.Tick(l,r);
            assert(out.right.trigger==1);
            if(xi)assert(out.pad.rt==1);else assert(Mouse175(MOUSEEVENTF_LEFTDOWN)==1);
            r.trigger=0;session.Tick(l,r);
        }
    }
    std::puts("PASS V175 actual fist trigger/filter + mod callers through real XI and TouchBindings: native-fallback LT block, physical-boxing LT quarantine, tracking, mode exit, UI/activity bypass fresh weapon ownership and F4 gesture-only pretransport exclusion; no retail/headset claim");
}
