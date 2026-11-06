#pragma once
// R9 combat (Claude lead, 25 Oct R8d2 headset log). three small rules, each with its own switch:
//  fist reach: the VR fist only redirected the native punch sweep where the real hand is. the native hook lands
//    0.40-0.60 m further forward (authoredToVrM, 16:54-16:55: 14/14 redirected sweeps missed, nativeResultNonNull=0),
//    so a VR punch at native combat distance could never touch the opponent. the redirected sweep is now moved along
//    YOUR punch direction toward the native contact point, at most VrFistReachAssistM. sideways/backward = no help.
//  combo slot: the manager phase 1/2 reset was skipped inside a live combo window, the trigger only skipped phase 1.
//    trigger phase 2 (attack end) went native 182x and every following press found no slot (NO_SLOT_AT_PRESS 119/119).
//  guard: the face-height guard took any upright blade crossing the face box, so a resting ready stance (hilt at the
//    belly, tip up) pressed block. entering a sword guard now needs the hilt raised (not more than
//    VrGuardHiltBelowHeadM under the eyes); a held guard keeps 8 cm hysteresis. shields unchanged.
// native rules decide every hit, block, combo and damage. nothing here creates an attack or a hit.
#include "pose_math.h"
#include "fist_clock_r11.h"
#include "fist_reach_r10.h" // R10: fists at Henry's real reach (replaces the R9 fist reach assist)
#include <algorithm>
#include <atomic>
#include <cmath>

namespace kcd2vr::combat_r9 {
namespace m=pose_core;
inline std::atomic<float> fistReachM{0.f};           // R10: retired (VrFistReachAssistM is ignored)
inline std::atomic<bool> keepSlotPhase2{false};       // VrComboKeepSlotPhase2
inline std::atomic<float> guardHiltBelowHeadM{.40f}; // VrGuardHiltBelowHeadM 0.2..0.9 (0 = off)
inline std::atomic<unsigned long long> fistAssisted{0},fistAssistNone{0};

inline bool finite3(m::Vec3 v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}
// R10: the R9 fist reach assist (fistShift) is retired - Miki: the fist reaches where Henry's arm reaches.
inline bool keepTriggerSlot(int phase,bool active,bool slotRequested,bool phase2Enabled) noexcept {
    if(!active||!slotRequested)return false;
    // R94: a slot after its action/listener ended can consume the next press without an attack.
    // Preserve explicit legacy opt-in; OFF lets the native idle close run in both phases.
    return phase2Enabled&&(phase==1||phase==2);
}
// hiltZ = hilt height minus eye height (world z up).
inline bool hiltRaised(double hiltZ,bool held,double belowM) noexcept {
    if(!(belowM>0))return true;
    if(!std::isfinite(hiltZ))return false;
    return hiltZ>=-(belowM+(held?.08:0.0));
}
}
