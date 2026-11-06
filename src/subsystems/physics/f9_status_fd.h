#pragma once
// D30 F9 status (Miki: "working f9 for physics"). pure text helpers, host tested (tests/f9_status_fd_test.cpp).
// F9 1/2/3 used to say nothing in the headset: rung 2 refused silently (J27: source=NO_HELD_SWORD, no sword was
// drawn) and rung 3 needs a killed body under the left grip. one visible line per rung tells what is active and,
// for rung 2, why the held weapon is not physical yet. text only, no native state.
#include <cstdint>
namespace kcd2vr::f9_status_fd {
// D51: living actuators are paused even if a historical preference is true.
// Corpse and prop selections remain independent. No ON label for an unavailable
// skeletal/capsule reaction; the normal native reaction pipeline is preserved.
inline const char* rung3Text(bool grab,bool touch,bool hit,bool props) noexcept {
    (void)touch;(void)hit;
    static const char* const t[4]={
        "F9 3: corpse grab OFF, prop wake OFF; living reactions PAUSED",
        "F9 3: corpse grab ON, prop wake OFF; living reactions PAUSED",
        "F9 3: corpse grab OFF, prop wake ON; living reactions PAUSED",
        "F9 3: corpse grab ON, prop wake ON; living reactions PAUSED"};
    return t[(grab?1:0)|(props?2:0)];
}
// rung texts (<=96 chars, plain ascii, no quotes - visible_failure::safe)
inline const char* rungText(int rung,bool corpseDrag,bool livingTouch,bool livingHit) noexcept {
    switch(rung){
    case 1:return "F9 1: observe only - F3 records physics; nothing is pushed";
    case 2:return "F9 2: physical held weapon - right hand one weapon, left hand empty";
    case 3:return rung3Text(corpseDrag,livingTouch,livingHit,true);
    default:return "F9 off: physics experiments off";
    }
}
// Native installation refusal happens before rung 2 can acquire any owner.
inline const char* installRefusalText() noexcept {
    return "F9 2 unavailable: native physics did not initialize; previous mode kept";
}
// rung 2 refusal, indexed like HeldPhysicsSourceT4 (native_held_owner_t4_status.h). nullptr = nothing to say.
inline const char* heldRefusalText(unsigned source) noexcept {
    switch(source){
    case 1:return "F9 2: no equipment read yet";
    case 2:return "F9 2: left hand holds something - empty it for the physical weapon";
    case 3:return "F9 2: draw a one-hand weapon in your right hand";
    case 4:case 5:case 6:return "F9 2: this weapon is not supported as physical (native kept)";
    case 7:case 8:case 9:return "F9 2: native physics not ready for the weapon, retrying";
    case 10:return "F9 2: paused here (menu, station or scripted scene)";
    case 12:return "F9 2: grip the weapon to make it physical";
    case 13:return "F9 2: physical weapon active";
    default:return nullptr;
    }
}
// Legacy compatibility helper, not called by the current F9 host. F9 mode
// changes do not start, refresh or extend the finite F3 diagnostic recorder.
constexpr bool keepCapture(int nextRung) noexcept {return nextRung>=1&&nextRung<=3;}
}
