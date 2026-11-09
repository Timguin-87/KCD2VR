#pragma once

#include <windows.h>
#include <d3d12.h>
#include <dxgi.h>
#include <cstdint>
#include <atomic>
#include "diagnostics/block_indicator_h24.h"
#include "vrik/melee_tuning_c54.h"
#include "vrik/shield_block_r120.h"
#include "vrik/thrust_tuning_d67.h"
#include "melee_tuning_ui_c54.h"
#include "subsystems/ranged_reload/options_xr32.h"
#include "subsystems/experimental/live_settings_d39.h"

namespace kcd2vr {

struct VrMenuState
{
    live_settings_d39::Settings live_d39{};
    std::array<melee_tuning_c54::Values,17> melee_types_c54{};
    std::array<thrust_tuning_d67::Values,17> thrust_d67{};bool independent_thrust_d67=true;
    float punch_sensitivity_c54=1.f,block_sensitivity_h29=1.f,blade_axis_cosine_h41=.80f;
    bool free_hand_present_d61=false,native_finisher_test_d61=false;int vrik_comparison_d61=0; // Menu choice: baseline, fresh pose, uncapped, elbow.
    bool craft_trial_h41=false,bypass_mod_vetoes_h41=false,archer_trial_h41=false,native_kill_nf=false;float chest_trial_h41=0.f; // D51: existing native paired playback backup, default OFF
    float punch_minimum_m_d65=.10f;
    float punch_travel_h41=1.f,guard_width_h41=.5f,guard_height_h41=.2f,guard_depth_d61=.5f,guard_body_distance_d56=.65f,pb_window_h41=2.f,ms_window_h41=1.5f;
    std::array<float,shield_block_r120::Count> shield_box_r120=shield_block_r120::defaults(); // R120 SHIELD BOX page sliders (VrShieldBox*R120)
    float finger_middle_lateral_c54[2][5]{};
    float finger_middle_shift_mm_h31[2]{15.f,15.f};
    float finger_middle_distal_medial_h31[2]{10.f,10.f};
#define KCD2VR_M2_HOST_SLOT 249 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=249
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    bool fast_block_r92 = false;
    bool enabled = true;                 // restart-only master config
    bool bridge_enabled = true;
    bool head_rotation = true;
    bool head_position = true;
    bool diagnostic_rendering = false;
    bool swap_eyes = false;
    int rotation_order = 0;               // V61.13: 0 = B*D, 1 = D*B diagnostic/recenter frame test
    bool fake_dof_active = false;          // V61.11 manual comfort prototype; no engine depth input
    float fake_dof_scale = 0.95f;          // live global pseudo strength from fake sweep
    bool engine_dof_active = false;        // V61.12 adaptive pseudo driven by KCD2 DOF focus CVar when available
    bool engine_dof_available = false;
    float engine_dof_focus_m = 0.0f;
    float engine_dof_scale = 0.95f;
    float depth_stereo_strength = 0.30f;      // V64.28 0..1: near asymmetry 0.70 -> 0.55; far remains 1.00

    float head_pose_scale = 1.0f;          // positional/world translation scale
    float camera_side_offset_m = 0.0f;      // V61.0 TPV local X; positive = right
    float camera_back_offset_m = 0.0f;      // V60.8 positive = backward from local camera forward
    float camera_height_offset_m = 0.035f;  // V66.110 default eye clearance above Henry collar; positive = local camera up
    float game_fov_target = 95.0f;          // pending native KCD2 cl_fov; Apply button commits
    float horizontal_fov_target = 95.0f;    // V60.9 native KCD2 cl_hfov diagnostic; Apply button commits
    bool fps_cap_enabled = false;             // V65.03a optional simple sys_MaxFPS limiter; OFF by default
    float fps_cap_target = 90.0f;            // manual target when the optional limiter is enabled
    float merged_mesh_target = 120.0f;      // V61.32 unified view/vegetation/merged distance target
    bool auto_projection_active = false;
    float auto_fov_trim_deg = 0.0f;
    float auto_center_x_trim = 0.00f;
    float auto_center_y_trim = 0.00f;
    bool auto_projection_center = true;
    float projection_pitch_trim_deg = 0.0f;
    float projection_pitch_effective_deg = 0.0f;
    int presentation_mode = 1;           // 0 stretch, 1 fit, 2 crop, 3 OpenXR FOV
    float presentation_zoom = 0.95f;
    float source_vertical_fov_deg = 95.0f;      // Normal/Pseudo source-FOV calibration only
    float corrected_center_scale_fov_deg = 95.0f; // legacy INI compatibility; ignored by V65.17 projective REMAP
    float presentation_center_x = 0.0f;
    float presentation_center_y = 0.0f;          // V61.27 live presentation-only common Y offset; never touches CView/culling
    float required_source_vfov_deg = 0.0f;       // V61.27 read-only XR coverage diagnostic for current captured source aspect
    float cull_frustum_scale = 1.60f;           // culling-only frustum tangent expansion
#define KCD2VR_M2_HOST_SLOT 250 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=250
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 251 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=251
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 252 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=252
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    float flat_screen_scale = 1.70f;         // V61.33 F6 virtual-screen physical scale, kept separate from native HUD
    float passive_full_frame_scale = 1.56f; // R160 UI default (his INI, was 1.20). V66.97 Loading/Cutscene-only Theater multiplier
    float hud_distance_r110 = 1.2f; // R160 UI default (his INI, was 3.0). R110 readable HUD plane distance, metres (same angular size)
    float combat_crosshair_scale_r112 = 0.25f; // R112 big directional combat crosshair size (VrCombatCrosshairScaleR112); R160 UI default (his INI, was 0.50)
    float vr_ui_unified_scale_r160 = 1.0f; // R160 UI: VrUiUnifiedScaleR160, one icon size on top of every per-element size (ui_hud_r160.h)
    float vr_ui_tutorial_y_r160 = 0.0f; // R160 UI: VrUiTutorialYR160, tutorial box vertical offset, stage px, + = down (dev page)
    float pause_overlay_size = 1.00f;        // V66.97 readable Pause content; background/DoF excluded
    float transient_zoom_retain = 0.00f;     // V61.45 retired compatibility field; no runtime zoom writer
#define KCD2VR_M2_HOST_SLOT 253 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=253
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR

    int stereo_mode = 3;                 // legacy diagnostic stereo mode; production remains 0
    int cyclopean_mode = 0;              // V61.37: 0 V65.01 mono; 1 projective FOV REMAP; 2 fixed pseudo .90; 3 depth pseudo near .70->.55 / far 1.00
    float afr_ipd_scale = 0.0f;

    bool balanced_graphics = true;
    bool fix_head_sync = false;             // V115/V163: fresh SYNC_NOW; OFF is cached XR A/B
    bool f2_ownership_fix = false;         // V65.04 exact renderer-owned pose token used for PRESENT pairing
#define KCD2VR_M2_HOST_SLOT 254 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=254
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 255 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=255
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 256 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=256
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 257 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=257
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 258 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=258
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 259 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=259
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 260 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=260
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // HUD groups retained for native/event-driven persistence work. Values are relative
    // to the current HUD design-space baseline; no Lua lifecycle helper is active.
#define KCD2VR_M2_HOST_SLOT 261 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=261
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    bool graphics_profile_applied = false;
    float render_scale = 1.0f;             // OpenXR eye swapchain scale; can be rebuilt live from F7
    int source_width = 1920;                // KCD2 source/backbuffer width persisted in KCD2VR.ini
    int source_height = 1800;               // KCD2 source/backbuffer height persisted in KCD2VR.ini
    float engine_clarity = 0.18f;          // bridge-side XR sharpening strength; engine r_Sharpening remains a balanced-profile extra
    float gamma = 1.0f;                    // V62.4 direct renderer color tuning
    float brightness = 0.0f;               // V62.4 direct renderer color tuning
    float contrast = 1.0f;                 // V62.4 direct renderer color tuning
#define KCD2VR_M2_HOST_SLOT 262 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=262
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 263 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=263
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 264 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=264
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 265 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=265
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 266 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=266
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    bool native_head_look = false;           // retired compatibility field
    bool activity_hands59c7f[4]{true,true,true,false}; // alchemy, smithing, sharpening, reading
#define KCD2VR_M2_HOST_SLOT 267 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=267
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    bool calibration_auto_restore43=true; // C43 VrCalibrationAutoRestore: owner exits/recenter keep the F8 calibration (view height, head anchor)
    bool precise_contacts44=true; // hand/finger and weapon surface contacts; no extra physics bodies
#define KCD2VR_M2_HOST_SLOT 268 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=268
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    int smith_anvil_mode_a3=1; // A3 anvil contact: 0 off, 1 real anvil (game physics), 2 learned (A2 experiment)
    bool smith_auto_hold36=true; // C46R1: hand-to-native smith input in Semi mode; retained VrSmithingAutoHold INI name.
    int  minigame_hands45=1; // VrMinigameHands: Native=0, gesture-paced native work=1.
    int alchemy_mode67=3,smithing_mode67=3; // R124: 3 FREE 6DOF (Miki's INI). G1: independent Native / Semi / Gesture owners.
#define KCD2VR_M2_HOST_SLOT 269 // M2_SOURCE_SELECTOR
#include "subsystems/doors/host.inl" // M2_OWNED_SOURCE slot=269
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    int grab_mode60=0; // C62: Native(0) or Staged native Hold(2); explicitly armed F5 physical test is independent
    bool grab_hand_stays43=true; // C43: VR grab: the hand stays on the controller from the Use to the native Attach (INI VrGrabHandStaysOnController)
    bool native_activity_controls163 = false; // session-only, full authored pad mappings
#define KCD2VR_M2_HOST_SLOT 270 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=270
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    int vr_turn_mode = 1;                 // V64.17: 0 snap, 1 smooth
#define KCD2VR_M2_HOST_SLOT 271 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=271
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    int roomscale_mode = 3;                 // V66.178R59: 0 off, 1 follow, 2 follow + dodge (follow needs 6DOF BODY), 3 dodge only (R59.16C5)
    bool smooth_move51 = false;             // C51: shared body movement smoothing, not tracked hand ownership.
    bool native_crouch_r93 = false; // accepted native crouch while keeping HMD-height walking
    int body_height_mode98 = 2;             // R98: 0 HMD HEIGHT, 1 NATIVE HEIGHT, 2 HMD + NATIVE (RIDE/CROUCH). INI VrBodyHeightModeR98 (old bool VrHmdBodyHeight).
#define KCD2VR_M2_HOST_SLOT 272 // M2_SOURCE_SELECTOR
#include "subsystems/throw_drop/host.inl" // M2_OWNED_SOURCE slot=272
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    bool chest_dagger54 = true;             // C54 cosmetic actual carried dagger; no right-hand draw/physics claim.
    int  comfort_vignette51 = 0;             // C51 VrComfortVignette 0..3
    bool gesture_talk = true;               // Optional index-finger point holds native Quick Talk choices; X stays available
    int reading_pages45 = 0;                // C45: book pages 0 modifier + flick, 1 modifier only, 2 native (C42 stick + grips)
    bool quick_slots_on_selection_r115 = true; // R115: VrHudQuickSlotFadeR112 - weapon/potion slots only while selecting (3 s)
    bool quick_access_face_touch_r112 = false; // R113: VrQuickAccessFaceTouchR112 - Y/B touch is the modifier (no thumbrest)
    unsigned quick_access_available_hands_r112 = 0; // R113: selected touch action live: left=1, right=2; menu text only
    int phys_bodies = 0;                    // R59.16C10R8: hand/blade physics bodies 0 off, 1 follow, 2 collide
#define KCD2VR_M2_HOST_SLOT 273 // M2_SOURCE_SELECTOR
#include "subsystems/vrmap/host.inl" // M2_OWNED_SOURCE slot=273
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 274 // M2_SOURCE_SELECTOR
#include "subsystems/experimental/host.inl" // M2_OWNED_SOURCE slot=274
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 275 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=275
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    int physical_f9_next_h6 = 1; // Session cursor: after refused install, next click exits safely.
    int physical_f9_rung61 = 0; // C61 session-only: OFF, OBSERVE, PHYSICAL WEAPON, PHYSICAL WEAPON + LAB
#define KCD2VR_M2_HOST_SLOT 276 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=276
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 277 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=277
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    bool auto_palm = true;                  // R59.16C8: palm centre on the controller from Henry's finger skeleton (INI VrAutoPalmAlign)
    bool riposte_native = false;            // R59.16C8 final: retired (never a native press); kept for state layout
    float target_dwell_s = 0.4f;            // R59.16C8: look-away seconds before the opponent lock switches (INI VrTargetSwitchDwell)
#define KCD2VR_M2_HOST_SLOT 278 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=278
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 279 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=279
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // R59.16C10d EXPERIMENTS page (Miki: "I am lost in constantly checking ini settings"): every live combat/guard/telegraph INI
    // switch in one tab, applied on change and saved to the INI (the INI keys stay the source on the next start).
#define KCD2VR_M2_HOST_SLOT 280 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=280
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 281 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=281
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    bool exp_windup_hold = true;            // VrWindupHold
    bool exp_guard_auto_windup = false; // VrGuardAutoWindupRefresh: native timing still decides the outcome.
    bool exp_guard_counter_motion = true;   // VrGuardRequireCounterMotion
    float exp_guard_enter_speed = 2.5f;     // VrGuardEnterMaxSpeed (m/s, 0 = off; C4R1 restored entry profile)
    int   exp_guard_enter_dwell_ms = 0;    // VrGuardEnterDwellMs (C4R1 restored immediate entry)
    float exp_guard_exit_speed = 3.5f;      // VrGuardExitSpeed (m/s, 0 = position only)
    int   exp_guard_after_attack_ms = 400;  // VrGuardAfterAttackMs
    float exp_strike_reference_j = 75.f;    // VrStrikeReferenceJ
    float exp_strike_max_power = 1.10f;     // VrStrikeMaxPower
    float exp_strike_two_handed = 1.25f;    // VrStrikeTwoHandedBonus
    // R109 MISC pages (COMBAT EXPERIMENTS > MISC 1/2/3): saved keys that had a runtime owner but no reachable menu row
    float npc_block_anim_speed109 = 1.4f;   // VrNpcBlockAnimSpeed 1..3 (acts only with the raise experiment)
    float npc_parry_anim_speed109 = 1.33f;  // VrNpcParryAnimSpeed 1..3 (acts only with the raise experiment)
    float npc_dodge_anim_speed109 = 1.25f;  // VrNpcDodgeAnimSpeed 1..3
    bool  npc_defense_raise109 = false;     // VrNpcDefenseRaiseExperiment: the enemy block/parry clip peak moves to the shortened ETA
    int   win_router_mode109 = 0;           // VrCombatRouter 0 off / 1 observe / 2 native windows written (same switch as F12)
    float win_combo_s109 = 3.f, win_riposte_s109 = 1.f, win_standard_s109 = 1.5f, win_ai_mod109 = .2f, win_pl_mod109 = .2f, win_block_speed_mod109 = 1.4f; // VrWin_* (wh_cs_* window globals, written only in router mode 2)
    int   riposte_window_ms109 = 700;       // VrRiposteWindowMs 100..3000: riposte window after Henry's perfect block
    bool  strike_power_on109 = true;        // VrStrikePower: the mod scales the native swing by the stroke
    float strike_min_power109 = .30f;       // VrStrikeMinPower 0.05..1: floor of the scaled strike
    int   strike_repeat_ms109 = 2000;       // VrStrikeRepeatMs 200..10000: repeat-penalty window for weak strokes
#define KCD2VR_M2_HOST_SLOT 282 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=282
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 283 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=283
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 284 // M2_SOURCE_SELECTOR
#include "subsystems/experimental/host.inl" // M2_OWNED_SOURCE slot=284
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 285 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=285
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 286 // M2_SOURCE_SELECTOR
#include "subsystems/hybrid/host.inl" // M2_OWNED_SOURCE slot=286
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 287 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=287
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 288 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=288
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // R59.16C10R14 WEAPON FEEL page
#define KCD2VR_M2_HOST_SLOT 289 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=289
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    float exp_swing_stamina_scale = 1.0f;   // VrSwingStaminaScale 0..2: native preparation cost x energy/mass (1..4 charges)
    float exp_strike_min_scale = 0.35f;     // VrStrikeMinScale 0..1
    int   exp_strike_min_interval_ms = 0;   // VrStrikeMinIntervalMs 0..1000
    bool  exp_strike_min_gate = false;      // VrStrikeMinScaleGate: a WEAK stroke does not attack (R15: off again - it dropped real swings)
#define KCD2VR_M2_HOST_SLOT 290 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=290
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 291 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=291
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 292 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=292
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // R59.16C10R17 COMBAT FLOW page (+ the RPG-stats slider on WEAPON FEEL)
    bool  exp_guard_strict = false;          // VrGuardStrictEntry
    bool show_block_indicator_h24 = block_indicator_h24::defaultEnabled; // Optional native BLOCK message, saved; default ON since R160 UI (his INI)
    bool  exp_no_block_cooldown = true;     // VrNoBlockCooldownF4
#define KCD2VR_M2_HOST_SLOT 293 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=293
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 294 // M2_SOURCE_SELECTOR
#include "subsystems/hybrid/host.inl" // M2_OWNED_SOURCE slot=294
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    bool  exp_hide_swing_lean38 = true;     // C38 VrHideSwingLean
#define KCD2VR_M2_HOST_SLOT 295 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=295
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 296 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=296
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 297 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=297
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 298 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=298
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 299 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=299
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 300 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=300
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 301 // M2_SOURCE_SELECTOR
#include "subsystems/hybrid/host.inl" // M2_OWNED_SOURCE slot=301
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 302 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=302
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 303 // M2_SOURCE_SELECTOR
#include "subsystems/hybrid/host.inl" // M2_OWNED_SOURCE slot=303
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 304 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=304
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    int   vr_shadow_quality49 = 0;          // C49 VrShadowQuality 0 LOW (VR2 cascades only) / 1 MEDIUM (C26 fix, 2048 sun map) / 2 HIGH (C26 fix, 4096); was C26d VrShadowTweaks
    bool  eye_head26 = true;                // C26d VrEyeAdaptHeadSteady (brightness adapts only with a steady head)
#define KCD2VR_M2_HOST_SLOT 305 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=305
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 306 // M2_SOURCE_SELECTOR
#include "subsystems/experimental/host.inl" // M2_OWNED_SOURCE slot=306
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 307 // M2_SOURCE_SELECTOR
#include "subsystems/gore/host.inl" // M2_OWNED_SOURCE slot=307
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 308 // M2_SOURCE_SELECTOR
#include "subsystems/experimental/host.inl" // M2_OWNED_SOURCE slot=308
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 309 // M2_SOURCE_SELECTOR
#include "subsystems/masterstrike_combos/host.inl" // M2_OWNED_SOURCE slot=309
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 310 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=310
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 311 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=311
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 312 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=312
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 313 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=313
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR

    bool xr_running = false;
    bool matrix_running = false;
#define KCD2VR_M2_HOST_SLOT 314 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=314
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    int eye_width = 0;
    int eye_height = 0;
    float raw_ipd_m = 0.0f;

    std::uint64_t xr_frames = 0;
    std::uint64_t valid_snapshots = 0;
    std::uint64_t present_captures = 0;
    std::uint64_t recursive_requests = 0;
    std::uint64_t recursive_passes = 0;
    std::uint64_t recursive_pipeline = 0;
    std::uint64_t recursive_rt = 0;
};

// V46.2: lightweight immediate-mode VR control menu. It deliberately uses the
// game's existing D3D12 device/queue and renders after the game submission but
// before XrManager copies the PRESENT source into OpenXR. No dummy/second
// D3D12 device and no DXGI Present hook are introduced.
class VrMenu
{
public:
    struct Callbacks
    {
        void* context = nullptr;
        void (*read_state)(void* context, VrMenuState* out) = nullptr;
        bool (*apply_state)(void* context, const VrMenuState* state) = nullptr;
        void (*request_matrix)(void* context) = nullptr;
        void (*request_safe_mono)(void* context) = nullptr;
        bool (*save_settings)(void* context) = nullptr;
        void (*request_record)(void* context, int category) = nullptr;
        void (*request_resolution_recreate)(void* context, float render_scale) = nullptr;
        bool (*apply_custom_resolution)(void* context, int width, int height) = nullptr;
        bool (*apply_hud_scale)(void* context, float scale) = nullptr;
        bool (*apply_dialogue_text_scale)(void* context, float scale) = nullptr;
        bool (*apply_vr_ui_layout)(void* context, const VrMenuState* state) = nullptr;
        void (*apply_game_fov)(void* context, float value) = nullptr;
        void (*apply_horizontal_fov)(void* context, float value) = nullptr;
        void (*apply_fps_cap)(void* context, float value) = nullptr;
        void (*apply_merged_mesh)(void* context, float value) = nullptr; // V61.32 applies all 3 view-distance CVars together
        int (*read_graphics_preset)(void* context) = nullptr;
        bool (*apply_graphics_preset)(void* context, int preset) = nullptr;
        bool (*apply_graphics_color)(void* context, float gamma, float brightness, float contrast) = nullptr;
        bool (*apply_hud_visibility)(void* context, bool crosshair, bool lower_icons, bool compass) = nullptr;
        void (*request_vrik_cycle)(void* context) = nullptr; // V63.0 WndProc edge -> main-loop ladder cycle
        void (*set_vrik_mode)(void* context, bool enabled) = nullptr; // V65.09 shortcut: OFF / F10 rung 1
        void (*request_vrik_recalibrate)(void* context) = nullptr;
        void (*request_vrik_restore)(void* context) = nullptr; // C43 RESTORE CALIBRATION (stored F8 profile, no T-pose)
        void (*request_recenter)(void* context) = nullptr;
        bool (*recalculate_projection)(void* context) = nullptr; // V65.07 commit current Game FOV/View FOV/framing into corrected mapper; never writes game FOV
        bool (*center_projection)(void* context) = nullptr;      // V65.07 one-shot residual vertical angular centering
        void (*toggle_fake_dof)(void* context) = nullptr;
        void (*toggle_engine_dof)(void* context) = nullptr;
        bool (*write_hud_xml_mod)(void* context) = nullptr;
        bool (*reset_hud_xml_mod)(void* context) = nullptr;
        // V66.69 Graphics: stage native KCD2 upscaler settings, commit only on Apply.
        bool (*read_upscaler)(void* context, int* tech, int* quality, int* preset) = nullptr;
        // Requested Stereo may be saved before resources exist; activation is
        // still guarded by the worker. This read-only callback explains restart.
        bool (*stereo_ready)(void* context) = nullptr;
        bool (*apply_upscaler)(void* context, int tech, int quality, int preset) = nullptr;
    };

    VrMenu() = default;
    ~VrMenu() { Shutdown(); }

    VrMenu(const VrMenu&) = delete;
    VrMenu& operator=(const VrMenu&) = delete;

    bool Initialize(ID3D12Device* game_device, HWND game_window, const Callbacks& callbacks);
    void Shutdown();

    // Called from Mod::OnRenderTick before XrManager::OnRenderThreadTick.
    void OnRenderThreadTick(ID3D12CommandQueue* queue);
    // V58 hybrid presentation: after XrManager has copied the clean KCD2
    // backbuffer into XR, draw the same book texture onto the desktop view.
    void OnDesktopOverlayTick(ID3D12CommandQueue* queue);
    // V46.5: poll mouse/keyboard from the mod control thread, not the D3D12
    // render callback, so UI actions never execute engine CVar setters inside
    // an ExecuteCommandLists hook.
    void PumpInput();
    // V52: direct OpenXR-controller navigation for the head-locked quad.
    void PumpVrInput(bool active, int pointer_x, int pointer_y, bool pointer_valid, float trigger, float thumb_x, float thumb_y, bool a_pressed, bool b_pressed);

    void Toggle();
    void ToggleDeepSeekLabH29();
    void SetVisible(bool visible);
    bool IsVisible() const { return visible_.load(std::memory_order_acquire); }
    // Calibration countdown remains visible after the normal menu is dismissed.
    bool IsLayerVisible() const { return IsVisible() || capture_countdown_.load(std::memory_order_acquire) || IsDiagnosticOnlyH24(); }
    ID3D12Resource* GetQuadTexture() const { return ui_texture_ready_ ? ui_texture_ : nullptr; }
    // Calibration-only text; 3..1 countdown, 0 capture, -1 VRIK active, -2 failed.
    void SetCaptureCountdown(bool active, int seconds_remaining);
    void SetBlockIndicatorH24(bool active);
    bool IsDiagnosticOnlyH24() const { return block_indicator_h24::visible(true,block_indicator_h24_.load(std::memory_order_acquire),IsVisible(),capture_countdown_.load(std::memory_order_acquire)); }

private:
    enum class MenuPage : int { Home=0, VrView, VrHud, VrHudElementControls, VrHudElements, Dialogue, VrActions, VrControls, Graphics, ViewModes, Experiments, ImmersionExperiments, Experiments2, WeaponFeel59c14, CombatFlow59c17, Kicks26, RetiredFingerPositions40, CombatAdvanced40, CombatTuning49, CounterStab40, Crafting67, ClaudeExperimentsH8, MeleeSensitivityH29, DeepSeekLabH29, ExperimentalFeaturesH5, FistSensitivityH41, GuardSettingsH41, DefenceWindowsH41, RangedOptions32, NpcTuning, LiveExperimentsD39, VrikExperimentsD61, StabSensitivityD67,ReleaseImmersionR92, MiscCombat1R109,MiscCombat2R109,MiscCombat3R109, ShieldBoxR120 }; // R109: COMBAT EXPERIMENTS > MISC 1/2/3
    enum class MenuTab : int { None = -1, Resolution = 0, Render, Fov, Ui, HeadPosition, Controls, Dialogue, OptimizedDefaults };
    std::atomic<int> meleeTypeH29_{4};
    std::atomic<unsigned> shieldSlotR120_{0}; // R120: which SHIELD BOX slider is being dragged
    std::atomic<int> fingerSide40_{0},fingerDigit40_{0};
    enum class DragControl : int { None, StabSpeedD67,StabTravelD67, H29Cut,H29Stab,H29Punch,H29Block,H29TravelR8,FistTravelH41,GuardWidthH41,GuardHeightH41,GuardDistanceD56,GuardEntryH41,GuardReleaseH41,PbWindowH41,MsWindowH41,FingerMiddleShiftH31,FingerMiddleMedialH31,FingerMiddleLateralC54, FingerOpen40,FingerClosed40,FingerLateral40,FingerProximal55, ExpWeightFeel59c14, ExpFatigueRamp59c14, ExpPoleOneHand59c14, ExpSwingStamina59c14, ExpStrikeMinScale59c14, ExpStrikeMinInterval59c14, ExpGuardEnterSpeed59c10, ExpGuardEnterDwell59c10, ExpGuardExitSpeed59c10, ExpGuardAfterAttack59c10, ExpStrikeRefJ59c10, ExpStrikeMaxPower59c10, ExpStrikeTwoHanded59c10, ExpHeadRaise59c10, ExpNpcEtaPrepare59c10, ExpNpcProximity36, ExpAttackReach59c10, WeaponGrip59c7f,TargetDwell59c8, SupportSword59c7f, SupportPole59c7f, CameraSide, CameraBack, CameraHeight, Zoom, GameFov, AutoFovTrim, AutoCenterYTrim, HorizontalFov, SourceFov, CenterX, CenterY, CorrectedPlanarY, CullScale, NativeUiScale, DialogueTextScale, DepthStereoStrength, TransientZoomRetain, HudXmlScale, HudXmlX, HudXmlY, FlatUiIpd, FlatScreenScale, PassiveFullFrameScale, HudDistanceR110, CombatCrosshairR112, PauseOverlaySize, FpsCap, EngineClarity, MergedMesh, Gamma, Brightness, Contrast, UiSubtitleScale, UiSubtitleY, UiInteractionMenuScale, UiInteractionMenuX, UiInteractionMenuY, UiItemPanelScale, UiItemPanelX, UiItemPanelY, UiDialogueScale, UiDialogueY, UiDialogueInset, DialogueZoomCompensation, DialogueFov, UiHintsScale, UiHintsInset, UiHintsY, UiControlPromptScale, UiControlPromptX, UiControlPromptY, UiCompactness, UiSafeWidth, UiSafeHeight, UiTopBoundary, UiBottomBoundary, UiUniformCenterX, UiHudOffsetX, UiHudOffsetY, UiCompassScale, UiCompassY, UiStatsScale, UiStatsX, UiStatsY, UiLowerScale, UiLowerY, UiLowerSpread, ProjectionPitchTrim, ExpNativeHitTime59c17, ExpComboWindow59c17, ExpIntentLiveMs59c17, ExpWeaponStatEffect59c17,  ExpMsHandoff23, KickReach26, KickReach52, ExpDirectBlockLo39, ExpDirectBlockHi39, ExpProximity49, ExpBlockSpeed49, NpcDifficulty, NpcDefenseSpeed, NpcUnarmedSpacing, ExpWeight49, ExpSkill49, CounterEnergy40, CounterMul40, CounterWindow40, HangMass44, KillEnergy44, MsStabEnergy47, KickAssistH8, BladeAxisH41, GuardDepthD61, M1DifficultyR109,M1DefenseSpeedR109,M1BlockAnimR109,M1ParryAnimR109,M1DodgeAnimR109,M1SpacingR109,M1BlockSpeed18R109,M1DirectLoR109,M1DirectHiR109,M1EtaPrepareR109,M1EtaStrikeR109,M2CounterEnergyR109,M2CounterMulR109,M2CounterWindowR109,M2IntentWaitR109,M3WinComboR109,M3WinRiposteR109,M3WinStandardR109,M3WinAiR109,M3WinPlR109,M3WinBlockR109,M3PbScaleR109,M3MsScaleR109,M3RiposteMsR109,M3MinPowerR109,M3RepeatMsR109,M3HangMassR109,M3KillEnergyR109, ShieldBoxR120, CraftUiSizeR145, UnifiedScaleR160, TutorialYR160 }; // R160 UI: one icon size + tutorial position. R120: ShieldBoxR120 + slider index (shieldSlotR120_) // R109 MISC sliders (M1..M3 stay contiguous: CommitDrag checks the range)

    static LRESULT CALLBACK WindowSubclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                               UINT_PTR subclass_id, DWORD_PTR ref_data);
    bool HandleWindowMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& result);

    bool EnsureGpuResources(ID3D12Resource* backbuffer); // R111: nullptr = texture/upload objects only
    bool CreateCoreResourcesR111();
    bool EnsureDesktopPipelineR111(DXGI_FORMAT format);
    void ReleaseGpuResources();
    bool RenderOverlay(ID3D12CommandQueue* queue);
    bool RenderDesktopOverlay(ID3D12CommandQueue* queue, ID3D12Resource* backbuffer);
    bool UpdateCpuBitmap();
    bool UploadBitmap(ID3D12GraphicsCommandList* list);

    void ReadState(VrMenuState& out) const;
    bool ApplyState(const VrMenuState& state);
    void HandleClick(int x, int y);
    void HandleDrag(int x, int y);
    void CommitDrag(DragControl released);
    void BeginTabAnimation(MenuTab tab, bool open);
    void UpdateTabAnimation();
    static bool IsDeveloperPage(MenuPage page) noexcept;
    static bool IsMiscPageR109(MenuPage page) noexcept; // R109 MISC 1/2/3
    void GoToPage(MenuPage page, int direction);
    void UpdatePageAnimation();
    void CyclePage(int direction);
    void PollInput();
    void MarkDirty();

    void DrawPanel(HDC dc, const VrMenuState& state);

    ID3D12Device* device_ = nullptr; // non-owning; DeviceHook owns game device lifetime
    HWND game_window_ = nullptr;
    bool input_hook_installed_ = false;
    Callbacks callbacks_{};

    std::atomic<bool> initialized_{false};
    std::atomic<bool> visible_{false};
    std::atomic<bool> dirty_{true};
    std::atomic<bool> block_indicator_h24_{false};
    std::atomic<bool> block_indicator_shield_r120_{false}; // R120: indicator says SHIELD BLOCK
    std::atomic<bool> capture_countdown_{false};
    std::atomic<int> capture_countdown_seconds_{0};
    std::atomic<int> drag_control_{static_cast<int>(DragControl::None)};
    std::atomic<int> active_tab_{static_cast<int>(MenuTab::None)};
    std::atomic<float> tab_open_progress_{0.0f};
    std::atomic<float> tab_anim_from_{0.0f};
    std::atomic<float> tab_anim_to_{0.0f};
    std::atomic<std::uint64_t> tab_anim_start_ms_{0};
    std::atomic<bool> tab_animating_{false};
    std::atomic<int> up_tech_{-1}, up_quality_{1}, up_preset_{0};
    std::atomic<int> up_status_{0}; // 0 staged, 1 readback OK, 2 save rejected, 3 saved/restart, 4 select supported stereo technology, 5 restart to create paired contexts
    std::atomic<bool> up_seeded_{false};
    std::atomic<int> xr_scale_index_{2}; // .50/.75/runtime1.00/1.25/1.50
    std::atomic<bool> xr_scale_seeded_{false};
    std::atomic<int> resolution_index_{2}; // 1920x1080 release baseline; seeded from persisted source size when available.
    std::atomic<bool> vr_pointer_valid_{false};
    int keyboard_selection_ = 0;
    bool mouse_left_prev_ = false;
    bool key_up_prev_ = false;
    bool key_down_prev_ = false;
    bool key_left_prev_ = false;
    bool key_right_prev_ = false;
    bool key_enter_prev_ = false;
    bool vr_trigger_prev_ = false;
    std::uint64_t mouse_down_started_ms_ = 0;
    std::uint64_t vr_trigger_started_ms_ = 0;
    std::atomic<bool> preview_transparent_{false};
    std::atomic<bool> ui_tuning_page_{false}; // legacy compatibility; V62.4 uses MenuPage
    std::atomic<int> active_page_{static_cast<int>(MenuPage::Home)};
    std::atomic<unsigned> liveGroupD39_{0};
    std::atomic<unsigned> releasePageR92_{0};
    std::atomic<int> graphicsPresetR92_{-2}, graphicsPresetStatusR92_{0};
    std::atomic<int> liveStatusD39_{0}; // 0 applied/saved, 1 apply refused, 2 save failed
    std::atomic<unsigned> experimentalGroupH5_{0}; // Navigation only; never saved or a runtime enable.
    std::atomic<bool> page_animating_{false};
    std::atomic<int> page_slide_dir_{1};
    std::atomic<std::uint64_t> page_anim_start_ms_{0};
    bool vr_stick_up_prev_ = false;
    bool vr_stick_down_prev_ = false;
    bool vr_stick_left_prev_ = false;
    bool vr_stick_right_prev_ = false;
    std::uint64_t vr_cursor_last_ms_ = 0;
    int software_cursor_x_ = 550;
    int software_cursor_y_ = 100;
    bool software_cursor_active_ = false;

    // Cursor visibility is reference counted by Win32; record only increments
    // performed by this menu so close restores exactly what open changed.
    int cursor_show_increments_ = 0;
    RECT old_clip_rect_{};
    bool old_clip_valid_ = false;

    // CPU panel bitmap, rendered through GDI into BGRA8 then uploaded.
    static constexpr int kPanelWidth = 1100;
    static constexpr int kPanelHeight = 680;
    static constexpr int kPanelX = 20;
    static constexpr int kPanelY = 20;
    HDC memory_dc_ = nullptr;
    HBITMAP dib_ = nullptr;
    HGDIOBJ old_bitmap_ = nullptr;
    void* dib_bits_ = nullptr;
    HFONT font_normal_ = nullptr;
    HFONT font_small_ = nullptr;
    HFONT font_tiny_ = nullptr;
    HFONT font_title_ = nullptr;
    HFONT font_hero_ = nullptr;

    ID3D12CommandAllocator* allocator_ = nullptr;
    ID3D12GraphicsCommandList* command_list_ = nullptr;
    ID3D12Fence* fence_ = nullptr;
    HANDLE fence_event_ = nullptr;
    std::uint64_t fence_value_ = 0;

    ID3D12DescriptorHeap* rtv_heap_ = nullptr;
    ID3D12DescriptorHeap* srv_heap_ = nullptr;
    ID3D12RootSignature* root_signature_ = nullptr;
    ID3D12PipelineState* pipeline_state_ = nullptr;
    ID3D12Resource* ui_texture_ = nullptr;
    ID3D12Resource* upload_buffer_ = nullptr;
    void* upload_mapped_ = nullptr;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT upload_footprint_{};
    UINT upload_num_rows_ = 0;
    UINT64 upload_row_size_ = 0;
    UINT64 upload_total_bytes_ = 0;
    bool ui_texture_ready_ = false;
    DXGI_FORMAT render_target_format_ = DXGI_FORMAT_UNKNOWN;
    std::uint64_t last_present_serial_ = 0;
    std::uint64_t last_upload_ms_r111_ = 0;
    std::uint64_t last_desktop_serial_ = 0;
    std::atomic<std::uint32_t> last_backbuffer_width_{0};
    std::atomic<std::uint32_t> last_backbuffer_height_{0};
};

} // namespace kcd2vr
