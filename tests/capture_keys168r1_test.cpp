#include "../src/diagnostics/hotkeys168r1.h"
#include "../src/diagnostics/capture168.h"
#include "../src/diagnostics/combat_capture.h"
#include <cassert>
#include <cstdio>
int main(){
 using namespace kcd2vr::diagnostics;
 keys168r1::Edges keys;
 assert(keys168r1::f10Route(true,false)==keys168r1::F10Route::HandsRecorder);
 assert(keys168r1::f10Route(true,true)==keys168r1::F10Route::HandsRecorder);
 assert(keys168r1::f10Route(false,true)==keys168r1::F10Route::NativeCapture);
 assert(keys168r1::f10Route(false,false)==keys168r1::F10Route::HandsMode);
 auto a=keys.poll({true,false,false},false);assert(a.hud&&!a.combat&&!a.interaction);
 a=keys.poll({true,false,false},false);assert(!a.hud);
 a=keys.poll({false,true,true},false);assert(!a.hud&&a.combat&&a.interaction);
 a=keys.poll({true,false,false},true);assert(!a.hud);
 a=keys.poll({true,false,false},false);assert(!a.hud);
 keys.poll({false,false,false},false);a=keys.poll({true,true,true},false);assert(a.hud&&a.combat&&a.interaction);
 namespace c=capture168;namespace cc=combat_capture;
 c::gate.mask=0;assert(!c::accept("HAND_F10 POSE fallback=FAULT UNKNOWN",100));
 c::gate.select(c::Domain::Combat,true,100);assert(!c::accept("HAND_F10 JOINT",101));
 c::gate.select(c::Domain::Hands,true,100);assert(!c::accept("HAND_F10 JOINT",101));
 const auto oldHandsSession=c::gate.session[unsigned(c::Domain::Hands)].load();
 c::gate.select(c::Domain::HandReference,true,100);assert(c::accept("HAND_F10 JOINT",101));
 c::gate.select(c::Domain::HandReference,false,102);assert(!c::accept("HAND_F10 JOINT",103)&&c::active(c::Domain::Hands,103)&&c::gate.session[unsigned(c::Domain::Hands)]==oldHandsSession);
 c::gate.select(c::Domain::HandReference,true,104);const auto refSession=c::gate.session[unsigned(c::Domain::HandReference)].load();
 c::gate.select(c::Domain::Hands,false,105);assert(c::active(c::Domain::HandReference,106)&&c::gate.session[unsigned(c::Domain::HandReference)]==refSession&&c::accept("HAND_F10 JOINT",106));
 c::gate.mask=0;assert(!c::accept("V66.168 POSE_VALIDITY ageMs=100",100));
 c::gate.select(c::Domain::Interaction,true,100);
 assert(c::accept("C10R18 NATIVE_GRASP ATTACH_NOT_ARMED reason=WRONG_THREAD",101));
 assert(c::accept("C19C3 NATIVE_GRASP ATTACH_NOT_ARMED reason=NO_FRESH_VR_HAND_USE",101));
 assert(c::accept("C10 HAND_HOLD_INPUT held=1",101));
 assert(c::accept("C10 HAND_USE hand=RIGHT",101));
 assert(c::essential("C10R18 NATIVE_GRASP READY exactRetail=SHA_AND_PINS"));
 assert(c::accept("V66.168 POSE_VALIDITY ageMs=100",101));
 assert(c::accept("V66.157 AUTHORITY",101));assert(c::accept("V66.168 INPUT_OWNER",101));
 assert(!c::accept("V66.168 MASTER_FACTORY result=0",101));
 cc::interactionOnlyUntil168r1=180100;
 assert(cc::interactionOnly168r1(101)&&cc::active(101));
 assert(cc::effectiveSources168r1(101)&cc::InteractionOnly168R1);
 assert(c::accept("V66.101R1-COMBAT_CAPTURE name=alchemy",101));
 assert(c::accept("V66.90-F5-CONTEXT",101));
 assert(!cc::interactionOnly168r1(99)&&!cc::interactionOnly168r1(180100));
 cc::interactionOnlyUntil168r1=0;assert(!cc::interactionOnly168r1(101));
 c::gate.select(c::Domain::Hud,true,100);
 assert(c::accept("V66.96-UI-INPUT owner=LASER",101));
 c::gate.select(c::Domain::Combat,true,100);cc::captureOnlyUntil168r1=180100;
 assert(cc::captureOnly168r1(101));assert(cc::active(101));
 assert(cc::effectiveSources168r1(101)&cc::CaptureOnly168R1);
 assert(!cc::captureOnly168r1(99));assert(!cc::captureOnly168r1(180100));
 c::gate.select(c::Domain::Combat,false,102);assert(!cc::captureOnly168r1(103));
 c::gate.mask=0;assert(!c::accept("V66.168 POSE_VALIDITY ageMs=100",105));
 assert(!c::accept("C10R18 NATIVE_GRASP ATTACH_NOT_ARMED reason=WRONG_THREAD",105));
 assert(!c::accept("C10 HAND_HOLD_INPUT held=1",105));
 c::gate.select(c::Domain::Combat,true,105);
 assert(!c::accept("C10R18 NATIVE_GRASP ATTACH_NOT_ARMED reason=WRONG_THREAD",106));
 std::puts("PASS plain-key edges, blocked-edge consumption, capture-only lifetime and cross-domain evidence routing");
}
