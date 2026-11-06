#pragma once
// V66.178R10: F4 rung 3 "VR rules", native kick/back-off request, camera lock policy,
// F11 workstation hands. Mod-control-thread state only; no native work here.
#include <atomic>
#include <cstdint>
#include <cmath>
#include "attack_xi178r8.h"

namespace kcd2vr::combat_rules178r10 {

// ---- camera lock policy (KCD2 cvars, all verified present in the 2026-10-09 R7 run) ----
// VrCombatAutoLock: 0 = native (never touched), 1 = auto-lock off in F4 rung 3 only (default),
// 2 = auto-lock off in every F4 rung. Uses the four KCD1 "RemoveAutoCameraLockInCombat" cvars
// (wh_cs_PlayerInputLockAreaWidth=0, ...CombatUnlockDelay=0, ...LockingTolerance=0,
// ...CursorUnlockDistance=300); manual R3 opponent_lock stays available, so the
// combat_locked action map (attack_special = kick/back-off) can still activate.
// VrCombatLockDisabled=1 additionally writes wh_cs_PlayerLockDisabled=1 while F4 is on
// (hard switch; combat_locked never activates, so no native kick/back-off).
inline std::atomic<int> autoLockMode{0}; // R49C1: native opponent acquisition retained; old INI overrides ignored
inline std::atomic<bool> lockDisabled{false};

// ---- kick / special ----
// VrKickTransport: 0 = F4 procedural visual leg only (V176), 1 = native attack_special
// through the XI pad (default): LT held for kHoldMs so KCD2's own action map produces the
// onHold event (defaultProfile.xml combat_locked: attack_special xi_triggerl_btn
// holdTriggerDelay=0.25). Unarmed = kick rows (attack type 4); armed = back-off.
inline std::atomic<int> kickTransport{1};
inline std::atomic<int> kickTransportIni26{3}; // C26: VrKickTransport as read from the INI (style 2 = 0 overrides it)
inline constexpr std::uint64_t kHoldMs=attack_xi178r8::kSpecialHoldMs;
// V66.178R13: the hold lives in attack_xi178r8 (shared with the non-F4 attack_special route).
inline std::atomic<std::uint64_t>& specialUntil=attack_xi178r8::specialUntil;
inline bool requestSpecial(std::uint64_t now) noexcept {return attack_xi178r8::requestSpecial(now);}
inline bool specialHeld(std::uint64_t now) noexcept {return attack_xi178r8::specialHeld(now);}

// ---- F11 workstation hands ----
// F5 rung 2: alchemy -> either Grip (>=0.58) holds alch_use (xi_a, hold semantics native);
// smithing -> a downward hammer stroke of the right controller pulses blacksmith_stroke
// (xi_triggerr > 0.4) for kStrokeMs. Aim stays on the left stick (blacksmith_gamepad_x/y).
inline std::atomic<bool> workstationEnabled{true};   // INI VrWorkstationHands
inline std::atomic<int> f5Rung{0};                    // V66.178R13: F5 0 off, 1 interaction recording, 2 recording + workstation gestures (F11 = reload recorder ON/OFF only)
inline constexpr std::uint64_t kStrokeMs=140;
inline constexpr std::uint64_t kStrokeRefractoryMs=320;

struct HammerDetector {
    bool havePrev=false; float prevY=0; std::uint64_t prevMs=0; std::uint64_t lastStrokeMs=0;
    float fastestDown=0; // C37C4: most negative vy seen (receipt only; cleared by the reader)
    float peakDown=0;     // most negative vertical speed inside the current swing (m/s)
    bool swinging=false;
    // y = controller height in the XR reference space (metres, +up). Returns true once per
    // downward stroke: a swing faster than minSpeed that then decelerates (the hammer "lands").
    bool update(float y,std::uint64_t nowMs,float minSpeed=1.6f) noexcept {
        if(!std::isfinite(y))return false;
        bool stroke=false;
        if(havePrev&&nowMs>prevMs){
            const float dt=float(nowMs-prevMs)*.001f;
            if(dt>=.004f&&dt<=.12f){
                const float vy=(y-prevY)/dt;
                if(vy<fastestDown)fastestDown=vy; // C37C4 receipt
                if(vy<-minSpeed){swinging=true;if(vy<peakDown)peakDown=vy;}
                else if(swinging&&vy>peakDown*.5f){
                    // speed fell below half the peak: landing
                    if(!lastStrokeMs||nowMs-lastStrokeMs>=kStrokeRefractoryMs){stroke=true;lastStrokeMs=nowMs;}
                    swinging=false;peakDown=0;
                }
            }else if(dt>.12f){swinging=false;peakDown=0;}
        }
        havePrev=true;prevY=y;prevMs=nowMs;return stroke;
    }
    void reset() noexcept {*this=HammerDetector{};}
};

inline std::atomic<std::uint64_t> strokeUntil{0};
inline std::atomic<bool> useHeld{false};
inline void reset() noexcept {attack_xi178r8::specialUntil.store(0);strokeUntil.store(0);useHeld.store(false);}
}
