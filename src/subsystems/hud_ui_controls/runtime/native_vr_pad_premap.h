#pragma once

#include "xr_input.h"
#include <cstdint>

namespace kcd2vr::native_vr_pad_premap {

struct Context {
    bool vr_menu{false};
    bool theater{false};
    bool contextual_ui{false};
    bool dialogue_navigation{false};
    bool back_context{false};
    bool apse_hub{false};
    bool apse_ui{false};
    bool system_menu{false};
    bool passive_back_only{false};
    bool semantic_minigame{false};
    std::uint64_t back_epoch{0};
    int pointer_hand{-1}; // -1 none, 0 left, 1 right; pointer trigger is withheld from XI
    bool grip_weapon_comparison114{false}; // Grip holds equipment; stick remains locomotion input
    bool quick_talk_open54{false}; // witnessed native chat map, not generic minigame or cursor inference
    bool hand_claim_blocked_t24[2]{}; // right0/left1; gameplay utilities only
    // Optional current native hint query. Zero retires the query; -1 means unavailable.
    int (*carry_query_r25)(std::uint32_t) noexcept {nullptr};
    bool quick_talk_claim_h4{false}; // current pending/opener/chooser gesture; quick-weapon chord re-arms at neutral
    bool snap_turn_d37{false}; // Gameplay yaw belongs to the latched Touch pulse; native menu/activity axes stay unchanged.
};

struct State {
    float lx{0}, ly{0}, rx{0}, ry{0}, lt{0}, rt{0};
    bool gameplay_r133{false}; // R133: built by the immersive gameplay branch (left stick = walking, right X = turning)
    bool a{false}, b{false}, x{false}, y{false};
    bool lb{false}, rb{false}, l3{false}, r3{false};
    bool start{false}, back{false};
    bool dpad_up{false}, dpad_down{false}, dpad_left{false}, dpad_right{false};
    // R59.16C10 (B-exit receipt, Codex HUD handoff 5c): one bounded B transaction edge per Build, host-testable
    // without the diagnostics sink. 0 none, 1 physical press, 2 physical release, 3 hold rearm pulse (release
    // 50 ms after an acknowledged stage epoch change), 4 menu-close (left Menu) press. backOwned = the press was
    // born inside a menu/minigame/back context (owned); a gameplay-born press is quarantined (RB) instead.
    unsigned char backEvent59c10{0}; bool backOwned59c10{false};
    // One native-frame torch request, never also XI D-pad Down. Original edge time.
    std::uint64_t torchRequestTick62{0};
    // Physical X press only: 1 talk/torch, 2 left-rest visor, 3 right-rest Wait,
    // 4 native UI/activity, 5 quarantined, 6 inactive controller.
    unsigned char xUtilityPress62{0};
    // R118: one physical X/Square hold on the STEAMVR / PS VR2 layout = native Inventory request (instead of torch).
    bool inventoryRequestR118{false};
};

void Reset() noexcept;
void SetXQuickTalk58(bool on) noexcept; // V66.178R58 (INI VrXQuickTalk; needs the companion pak's defaultProfile)
void SetBQuickTalk45(bool on) noexcept; // C45 (INI VrBQuickTalk, default off): gameplay B as a second Quick Speak (RB)
State Build(const ControllerState& left, const ControllerState& right, const Context& context) noexcept;

} // namespace kcd2vr::native_vr_pad_premap
