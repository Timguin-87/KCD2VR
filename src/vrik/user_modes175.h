#pragma once
#include <cstdint>

// Public controls describe gameplay, never diagnostic leases.  Legacy internal
// numeric values remain adapters for the proven native boundaries only.
namespace kcd2vr::user175 {
enum class Mode : unsigned { Off, Calibrated, Hands, Weapons, Combat, CombatPhysical, CombatFull, CombatCombo, CombatImpale, CombatGeometry, CombatIntegrated, CombatContactLab, CombatSynchronized, CombatIntent, WeaponsHands40 };
struct Configuration {
    int combat=0, interaction=0, pose=0, weapon=0;
    bool experiment=false;
    bool physical177=false;
    bool full178r10=false; // V66.178R10 F4 rung 3: VR combat rules (lock policy, native kick request)
    int rung178r15=0;      // V66.178R15: 4 = combo/master-strike coach, 5 = + impale latch experiment, 6 = V66.178R27 geometry hits (hybrid)
};
inline constexpr Configuration configuration(Mode mode) noexcept {
    switch(mode) {
    case Mode::Calibrated: return {3,2,4,6,false}; // C44: normal free-hand surface contacts, not the exclusive F10 boxing mode
    case Mode::Hands: return {3,2,4,6,false}; // Hand behavior includes ordinary holsters/weapon handling; never boxing-only ownership.
    case Mode::Weapons: return {3,2,4,7,false};
    case Mode::WeaponsHands40: return {3,2,4,7,false}; // normal weapon owner plus existing free-hand contacts
    case Mode::CombatIntegrated: return {3,2,4,7,true,true,true,1};
    case Mode::Combat: return {3,2,4,7,true};
    case Mode::CombatPhysical: return {3,2,4,7,true,true};
    case Mode::CombatFull: return {3,2,4,7,true,true,true,3};
    case Mode::CombatCombo: return {3,2,4,7,true,true,true,4};
    case Mode::CombatImpale: return {3,2,4,7,true,true,true,5};
    case Mode::CombatGeometry: return {3,2,4,7,true,true,true,6};
    case Mode::CombatContactLab: return {3,2,4,7,true,true,true,7};
    case Mode::CombatSynchronized: return {3,2,4,7,true,true,true,1}; // R59.16C10R9: rung 1 configuration + contact authority
    case Mode::CombatIntent: return {3,2,4,7,true,true,true,1};       // R59.16C10R16: SYNCHRONIZED configuration + VR intent rules
    default: return {};
    }
}
inline constexpr Mode integrated(bool experiment) noexcept {
    return experiment ? Mode::Calibrated : Mode::CombatIntegrated;
}
// V66.178R28: F4 cycles OFF -> 1 (R22C2 integrated native combat) -> 2 (R27 hybrid geometry hits) -> OFF.
// R59.16C7E-DECAP1 (Miki): plain F4 cycles OFF -> rung 2 (hybrid geometry hits) -> rung 3 (VR backend, direct hits) -> OFF.
// The old rung 1 (integrated native combat) is reached only with Shift+F4 (toggle), so a test session never lands in it by accident.
// R59.16C10R9 (Miki: "rung 3 = 1+2"): plain F4 cycles OFF -> rung 2 (hybrid geometry) -> rung 3 SYNCHRONIZED (native
// admission/defense/combos of rung 1 + the VR contact decides the hit moment) -> rung 4 VR DIRECT (the former rung 3) -> OFF.
inline constexpr Mode hybridPlain59c10r9(bool experiment,bool integrated,bool synchronized,int rung) noexcept {
    return !experiment ? Mode::CombatGeometry : synchronized ? Mode::CombatContactLab : integrated ? Mode::CombatGeometry :
        rung==2 ? Mode::CombatSynchronized : Mode::Off;
}
// R59.16C10R16 (Miki, 15 Oct: "make it the first F4 rung (I am confused from the naming)"). The log names now follow the
// F4 PRESS that selects the mode: plain F4 cycles OFF -> 1 VR INTENT -> 2 HYBRID GEOMETRY -> 3 SYNCHRONIZED -> 4 VR DIRECT
// (contact lab) -> OFF. Shift+F4 still toggles the integrated native combat (old rung 1), outside the plain cycle.
inline constexpr Mode hybridPlain59c16(bool experiment,bool integrated,bool synchronized,bool intent,int rung) noexcept {
    return !experiment ? Mode::CombatIntent : intent ? Mode::CombatGeometry : synchronized ? Mode::CombatContactLab :
        integrated ? Mode::CombatIntent : rung==2 ? Mode::CombatSynchronized : Mode::Off;
}
// Beep count = F4 press number (Shift+F4 integrated = 5 short beeps, OFF = one long low tone).
inline constexpr int press59c16(Mode mode) noexcept {
    return mode==Mode::CombatIntent?1:mode==Mode::CombatGeometry?2:mode==Mode::CombatSynchronized?3:mode==Mode::CombatContactLab?4:
        mode==Mode::CombatIntegrated?5:0;
}
inline constexpr Mode hybridShift59c7e(bool experiment,bool integrated) noexcept {
    return experiment&&integrated ? Mode::Off : Mode::CombatIntegrated;
}
// C4R2: public F4 now has only the two retained backends. Their native/internal
// configuration values above stay unchanged; do not renumber combat policies.
inline constexpr Mode hybridCurrent19c4r2(bool experiment,bool synchronized,bool direct) noexcept {
    return !experiment?Mode::CombatSynchronized:synchronized?Mode::CombatContactLab:
        direct?Mode::Weapons:Mode::CombatSynchronized; // C44: combat OFF retains normal VRIK/bow/weapon ownership
}
// C59 user-approved baseline is synchronized hybrid; F4 is DIRECT comparison
// ON/OFF (off returns to that hybrid baseline, never disables VRIK).
inline constexpr Mode comparison59(bool experiment,bool synchronized) noexcept {
    return experiment&&!synchronized?Mode::CombatSynchronized:Mode::CombatContactLab;
}
inline constexpr Mode default59(Mode requested,bool hybrid) noexcept {
    return hybrid&&requested==Mode::Calibrated?Mode::CombatSynchronized:requested;
}
inline constexpr int pressCurrent19c4r2(Mode mode) noexcept {
    return mode==Mode::CombatSynchronized?1:mode==Mode::CombatContactLab?2:0;
}
inline constexpr Mode hands(int pose,int weapon) noexcept {
    // R96: Hands is pose4/weapon6 since R95 (was boxing 5/0). The menu VRIK cycle must still reach OFF from
    // Hands/Calibrated (4/6) and from the old boxing tuple; weapons/combat tuples go to Hands as before.
    return (pose==5&&weapon==0)||(pose==4&&weapon==6) ? Mode::Off : Mode::Hands;
}
inline constexpr Mode weapons(int weapon,bool experiment,int pose=0) noexcept {
    return weapon==7&&!experiment ? (pose==4?Mode::WeaponsHands40:Mode::Calibrated) : Mode::Weapons;
}
inline constexpr Mode combat(bool experiment) noexcept {
    return experiment ? Mode::Calibrated : Mode::Combat;
}
inline constexpr Mode combat(bool experiment,bool physical) noexcept {
    return !experiment?Mode::Combat:!physical?Mode::CombatPhysical:Mode::Calibrated;
}
// V66.178R10: F4 cycles OFF -> 1 (controller weapon) -> 2 (physical drive) -> 3 (VR rules) -> OFF.
inline constexpr Mode combat(bool experiment,bool physical,bool full) noexcept {
    return !experiment?Mode::Combat:!physical?Mode::CombatPhysical:!full?Mode::CombatFull:Mode::Calibrated;
}
inline constexpr int f4Rung(bool experiment,bool physical,bool full) noexcept {
    return !experiment?0:!physical?1:!full?2:3;
}
// V66.178R15/R27: F4 cycles OFF -> 1 -> 2 -> 3 -> 4 (combo/master coach) -> 5 (impale latch) -> 6 (geometry hits) -> OFF.
inline constexpr Mode combat(bool experiment,bool physical,bool full,int rung) noexcept {
    return !experiment?Mode::Combat:!physical?Mode::CombatPhysical:!full?Mode::CombatFull:rung<4?Mode::CombatCombo:rung<5?Mode::CombatImpale:rung<6?Mode::CombatGeometry:Mode::Calibrated;
}
inline constexpr const char* name(Mode mode) noexcept {
    switch(mode) {
    case Mode::Calibrated: return "CALIBRATED_HANDLING";
    case Mode::Hands: return "VRIK_HAND_CONTACT_FISTS";
    case Mode::Weapons: return "VR_WEAPONS";
    case Mode::WeaponsHands40: return "VR_WEAPONS_AND_FREE_HAND_CONTACTS";
    case Mode::CombatIntegrated: return "SHIFT_F4_INTEGRATED_NATIVE_COMBAT";
    case Mode::Combat: return "COMBAT_EXPERIMENT";
    case Mode::CombatPhysical: return "F4_RUNG2_PHYSICAL_COMPARISON";
    case Mode::CombatFull: return "F4_RUNG3_VR_RULES";
    case Mode::CombatCombo: return "F4_RUNG4_COMBO_MASTER_COACH";
    case Mode::CombatImpale: return "F4_RUNG5_IMPALE_LATCH";
    case Mode::CombatGeometry: return "F4_PRESS2_HYBRID_GEOMETRY_HITS";
    case Mode::CombatContactLab: return "F4_PRESS2_VR_DIRECT_CONTACT_LAB";
    case Mode::CombatSynchronized: return "F4_PRESS1_SYNCHRONIZED_NATIVE_ACTION_VR_CONTACT";
    case Mode::CombatIntent: return "F4_PRESS1_VR_INTENT";
    default: return "ALL_VRIK_OFF";
    }
}
}
