#pragma once

#include "xr_input.h"
#include "pause_chord58c2.h"
#include "ui_back_escape_m1.h"
#include <cstdint>

namespace kcd2vr {

// V58.6 provisional Quest/Touch -> vanilla KCD2 keyboard/mouse bridge.
// Layout follows the requested test scheme while the native/ViGEm path is researched.
class TouchBindings {
public:
    struct Config {
        unsigned short move_forward='W', move_left='A', move_back='S', move_right='D';
        unsigned short sprint=160, crouch='C', jump=32, interact='E';
        unsigned short torch='R', visor='Z', horse_dog='X', inventory='I', pause=27, map='M', quick_reply='Q', skip_time='T';
        unsigned short quick_left='5', quick_right='6', weapon_slot1='1', weapon_slot2='2';
    };
    struct Events {
        bool toggle_mis_menu = false;
        bool toggle_inventory_comfort = false;
        bool toggle_map_comfort = false; // V66.108: quick-access touch + LS-Up held 1.0s -> physical/native map toggle
        bool back_cancel = false;
        bool toggle_pause_comfort = false;
        bool cycle_display_mode = false;
        bool theater_ui_input = false; // V64.89: presentation-only glyph refresh trigger after keyboard-style theater input
        // V64.80: semantic controller output requested by the VR control layer.
        // The mod translates this to genuine KCD2 XI instead of a keyboard key.
        bool native_jump_pulse = false;
        bool special_attack_requested = false; // request only; KCD2 owns selection/acceptance
    };

    void SetEnabled(bool enabled);
    bool Enabled() const { return enabled_; }
    void SetAttackDirectionMouseEmulation(bool enabled) { attack_direction_mouse_ = enabled; }
    void SetTurnMode(int mode) { turn_mode_ = mode == 0 ? 0 : 1; }
    void SetNativeGamepadAxes(bool left_ready,bool right_ready){ native_left_stick_=left_ready; native_right_stick_=right_ready; }
    void SetNativeSpecialRoute168(bool enabled){native_special168_=enabled;}
    void SetNativeGamepadActions(bool enabled);
    void SetNativeMinigameOwnership162r1(bool enabled){native_minigame162r1_=enabled;}
    void SetAuthoredMinigameContext(bool alchemy,bool reading){alchemy_context_=alchemy;alchemy_reading_context_=reading;}
    // V65.43 VRIK input-ownership lab. Gameplay-only; UI trigger semantics remain native.
    // 0 native, 1 suppress RT->attack, 2 suppress RT+LT combat, 3 also suppress quick weapon toggle/sheath.
    void SetVrikInputLabStage(int stage);
    // Diagnostic: invoke the same primary quick-weapon path used by quick-access touch + LS-Left.
    // If holstered this enters the native player draw scheduler/DrawAction; if drawn it sheathes.
    // V66.106 F5: one normal native Use/E pulse after the hand ray has been scoped to the VR hand.
    // Returns false if ordinary A/E is already held; never releases a user-owned held interaction.
    // true means queued XI input, or both keyboard events accepted by Windows.
    // It never means the native pickup/action accepted the request.
    bool RequestPhysicalInteractionUse(std::uint64_t originalPressD48=0);
    void ValidatePhysicalInteractionPressD48(std::uint64_t originalPress,bool held) noexcept;
    bool SetPhysicalInteractionHeld114(bool held);
    bool PhysicalInteractionGamepad59c10(std::uint64_t now,bool allowed) noexcept;
    void SetGripWeaponComparison114(bool enabled) { grip_weapon_comparison114_=enabled; }
    // V66.138: explicit ownership. With VRIK OFF, Right Grip always exposes the
    // old two-axis free-look/precision mouse path. With VRIK active, Grip belongs
    // to the tracked hand/weapon and must never steer the game camera.
    void SetRightGripFreeLookEnabled(bool enabled) { right_grip_free_look_enabled_=enabled; }
    bool RightGripFreeLookEnabled() const { return right_grip_free_look_enabled_; }
    // V64.20: closed-loop native head-aim servo. The mod computes the angular
    // difference between KCD2's live native interactor/aim ray and the VR
    // visual-forward ray; this bridge feeds only the residual error back
    // through KCD2's already-proven mouse-look path.
    void SetHeadAim(bool enabled, float yaw_error_deg, float pitch_error_deg, bool valid) {
        head_aim_enabled_=enabled; head_aim_yaw_error_deg_=yaw_error_deg;
        head_aim_pitch_error_deg_=pitch_error_deg; head_aim_valid_=valid;
    }
    Events Update(const ControllerState& left,const ControllerState& right,bool mis_menu_visible,bool theater_ui_visible=false,bool contextual_ui_visible=false,bool back_context_visible=false);
    void SetConfig(const Config& config) { config_=config; }
    const Config& GetConfig() const noexcept { return config_; }
    void Reset();
    // Called once before native XI/fallback routing, including native pause/menu ownership.
    bool UpdatePauseChord58c2(ControllerState& left,const ControllerState& right,bool allowed);
    // Once before all UI/XI routes; allowed includes current source/focus ownership.
    bool UpdateBackEscapeM1(const ControllerState& right,bool allowed,std::uint64_t input_generation);
    bool BackEscapeConsumedM1() const noexcept {return back_escape_m1_.Consumed();}

    // Native menu owns this click; a held close press must not reopen inventory
    // on the gameplay side of the transition, including a Reset at UI exit.
    void ConsumeMenuUntilRelease174r2() noexcept { menu_consumed174r2_=true; }
    // R118: the native pad saw an X/Square hold on the STEAMVR / PS VR2 layout. The next gameplay Update sends
    // the same Inventory key + comfort request as a Left Menu tap. false drops a request no gameplay Update took.
    void QueueInventoryR118(bool request) noexcept { inventory_request118_=request; }

private:
    enum class Dir : int { None=0, Up=1, Down=2, Left=3, Right=4 };

    static Dir StickDir(float x, float y, float threshold = 0.62f);
    static void KeyDown(unsigned short vk);
    static void KeyUp(unsigned short vk);
    static void KeyTap(unsigned short vk);
    static void MouseButton(bool left, bool down);
    static void MouseMoveRelative(long dx, long dy);
    static void MouseWheel(int delta);
    static void SetKeyHeld(unsigned short vk, bool want, bool& held);
    void SetManualInteractionHeld114(bool held);
    void ReleaseInteractionOwners114();

    pause_chord58c2::State pause_chord58c2_{}; // deliberately survives Reset/UI transitions
    ui_back_escape_m1::State back_escape_m1_{}; // same physical hold survives route Reset, never rearms there
    Config config_{};
    bool enabled_ = true;
    bool physical_interact114_=false,manual_interact114_=false,grip_weapon_comparison114_=false;
    bool physical_interact_xi59c10_=false;
    std::uint64_t physical_interact_pulse59c10_=0,physical_interact_press_d48_=0;
    bool right_grip_free_look_enabled_=true;
    bool attack_direction_mouse_ = true;
    int turn_mode_ = 1; // 0 snap, 1 smooth
    bool native_left_stick_=false, native_right_stick_=false;
    bool native_gamepad_actions_=false,native_special168_=false;
    bool native_minigame162r1_=false;
    bool alchemy_context_=false,alchemy_reading_context_=false;
    bool minigame_lt_held_=false,minigame_rt_held_=false;
    bool minigame_x_held_=false,minigame_y_held_=false;
    Dir minigame_dpad_prev_=Dir::None,minigame_page_prev_=Dir::None;
    int vrik_input_lab_stage_=0;
    bool w_=false,a_=false,s_=false,d_=false,shift_=false,lmb_=false,rmb_=false,interact_held_=false;
    bool menu_prev_=false, y_prev_=false, x_prev_=false;
    bool x_prev_down_=false;
    std::uint64_t x_down_since_=0;
    bool x_long_sent_=false;
    bool y_prev_down_=false;
    std::uint64_t y_down_since_=0;
    bool y_long_sent_=false;
    bool b_prev_down_=false;
    bool b_quick_reply_eligible175_=false;
    bool b_context_back_held_=false;
    std::uint64_t b_down_since_=0;
    bool b_long_sent_=false;
    bool b_x_held_=false; // reused for Y-hold companion X after V61.32 B/Y family swap
    bool right_grip_prev_down_=false;
    std::uint64_t right_grip_since_=0;
    bool right_grip_used_as_modifier_=false;
    bool torch_chord_prev_=false;
    bool vr_menu_chord_prev_=false;
    std::uint64_t vr_menu_chord_since_=0;
    bool sticks_menu_chord_prev_r112_=false;        // R112: L3+R3 hold = VR menu (no thumbrest)
    std::uint64_t sticks_menu_chord_since_r112_=0;
    bool map_chord_latched_=false;
    std::uint64_t map_chord_since_=0;
    bool target_switch_latched_=false;
    bool r3_prev_down_=false;
    bool snap_turn_latched_=true; // D37: a new/reconnected controller must first be neutral
    std::uint64_t last_combat_input_ms_=0;
    bool left_menu_prev_down_=false;
    bool menu_consumed174r2_=false;
    bool inventory_request118_=false; // R118: one-shot, consumed by the next Update
    std::uint64_t left_menu_since_=0;
    bool left_menu_long_sent_=false;
    Dir quick_dir_prev_=Dir::None;
    std::uint64_t utility_dir_since_=0;
    bool utility_long_sent_=false;
    Dir quick_consumable_dir_=Dir::None;
    unsigned short quick_consumable_key_=0;
    unsigned short quick_consumable_pending_key_=0;
    std::uint64_t quick_consumable_hold_at_ms_=0;
    Dir attack_dir_prev_=Dir::None;
    Dir jump_dir_prev_=Dir::None;
    Dir crouch_dir_prev_=Dir::None;
    std::uint64_t turn_last_ms_=0;
    std::uint64_t turn_last_log_ms_=0;
    float turn_fraction_=0.0f;
    float bow_fraction_y_=0.0f;
    std::uint64_t bow_last_log_ms_=0;
    bool head_aim_enabled_=false;
    bool head_aim_valid_=false;
    float head_aim_yaw_error_deg_=0.0f;
    float head_aim_pitch_error_deg_=0.0f;
    int weapon_slot_toggle_=1;

    // V64.14 theater navigation: sticks are digital arrow-key channels independent of the laser.
    Dir menu_nav_l_=Dir::None, menu_nav_r_=Dir::None;
    std::uint64_t menu_nav_l_next_repeat_ms_=0, menu_nav_r_next_repeat_ms_=0;
    unsigned short quick_selected_key_=0; // provisional keyboard bridge only; native QAM/WUID path remains unresolved
};

} // namespace kcd2vr
