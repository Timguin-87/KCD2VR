#include "vrik/fist_lt_r138.h" // R138: closed empty left fist is never native LT
#include "vrik/impale_death_speed_r106.h" // R106: impale fast death (used by native_impale_part.inl)
#include "vrik/start_r111.h" // R111: start at Henry's eyes (StandingEyeShift43, session re-seat)
#include "core/load_rearm_r96.h" // R96: save-load re-arm of session latches
#include "diagnostics/capture_ticket_d84.h"
#include "diagnostics/reload_capture_d84.h"
#include "diagnostics/draw_native_d77.h"
#include "vrik/melee_grip_r123.h" // R123: fixed melee grip switch (physical_native.inl PhysCaptureGrip6698)
#include "subsystems/physics/saved_corpse_drag_d72.h"
#include "vrik/elbow_hysteresis_d69.h"
#include "vrik/pose_restore_debt_r96.h"
#include "core/observed_load_recovery_r96.h"
#include "vrik/elbow_destination_d86.h"
#include "vrik/bow_draw_publication_r93.h"
#include "vrik/claude_motion_d69.h"
#include "subsystems/masterstrike_combos/draw_signal_d63.h" // D63 exact native draw ownership
#include "subsystems/masterstrike_combos/native_kill_anim_nf.h" // I10/NF native kill animations switch
#include "core/page_memo_pf.h" // D30-PERF O(1) resident page memo
#include "subsystems/ranged_reload/crossbow_rifle_reload_drive60.h"
#include "core/initial_draw_completion_h36r3.h"
#include "core/instant_pickup_d72.h"
#include "vrik/owned_interaction_body_entry_r93.h"
#include "subsystems/ranged_reload/crossbow_native_xr34.h"
#include "subsystems/ranged_reload/crossbow_hand_d89.h" // R89 xbow
#include "subsystems/ranged_reload/reload_throw_claim_t25.h"
#include "subsystems/ranged_reload/options_xr32.h"
#include "subsystems/ranged_reload/ranged_draw_policy_d72.h"
#include "subsystems/ranged_reload/weapon_draw_scope_d75.h"
#include "subsystems/ranged_reload/draw_guard_r120.h" // R120: drawn weapon hidden until our first VR placement
#include "subsystems/ranged_reload/entity_hide_pin_r125.h" // R125: CEntity::Hide pin, hook tolerant, checked every call
#include "subsystems/masterstrike_combos/paired_hands_d72.h"
#include "subsystems/ranged_reload/bow_physical_aim_d65c.h"
#include "vrik/gesture_recovery_h30.h"
#include "vrik/gore_eligibility_d56.h"
#include "native_script_t2.h"
#include <new> // optional master observer's bounded process-lifetime retirement bucket
#include "diagnostics/receipt_budget56.h"
#include "diagnostics/master_eligibility_h21.h"
#include "core/player_window_h26.h"
#include "diagnostics/interaction_cycle_h4.h"
#include "diagnostics/hmd_dodge_context_h24.h" // Namespace-owning headers stay at global scope.
#include "vrik/weapon_parry_contact_h24.h"
#include "vrik/native_weapon_combat_h29.h"
#include "subsystems/doors/module.h"
#include "subsystems/physics/native_grab_input60.h" // DS-F88: grip-evidence resolver (file scope: namespace kcd2vr header)
#include "vrik/impale_hold60.h" // DS-F91: impale constraint hold (file scope: namespace kcd2vr header)
#include "lb_prompt_x_r153.h" // R155: the OnAction detour notes infinite_hold_interaction_* dispatches (hold to look prompt offered)
#include "subsystems/direct/tutorial_native_r159.h" // R161: tutorial GAME TIMING (native hit time + native attack clock with the practice weapon)
#include "subsystems/direct/training_vanilla_r163.h" // R163: training fights - flinch / NPC writes held off
#include "subsystems/direct/npc_force_block153.h" // R156: the equipment snapshot publishes "practice / quest weapon in hand" (no NPC force-block roll)
#include "subsystems/gore/impale_constraint_h21.h"
#include "subsystems/physics/touch_react_service60.h" // DS-F97/F99: touch-reaction service + flesh seam (file scope: namespace kcd2vr header)
#include "subsystems/physics/runtime/committed_hand_push_h21.h"
#include "subsystems/physics/grab_over_talk_ds.h" // DS-2026-10-23: the grab-over-talk (file scope) // DS-F106: the hand-push (HIGGS) policy + mailbox (file scope: namespace kcd2vr header)
#include "vrik/direct_response36.h"
#include "vrik/defense_observe37.h" // C37 read-only defense receipts
#include "vrik/live_attributes5912.h"
#include "vrik/motion_owned59c4.h"
#include "subsystems/direct/combat_router_h3.h"
#include "subsystems/experimental/deepseek_options_h29.h"
#include "diagnostics/capture_control168.h"
#include "activity_input163.h"
#include "subsystems/ranged_reload/stowed_hide_hw.h" // D33 stowed weapon hiding policy
#include "vrik/shoulder_reach_d38.h" // D39 shoulder-travel diagnostic; wrist-authoritative fit
#include "vrik/hand_separation_ds1.h" // DS1 free-hand self separation (file scope: namespace kcd2vr header)
#include "subsystems/throw_drop/inventory_lifecycle_t8.h"
#include "subsystems/physics/load_fault_recovery_r96.h"
#include "vrik/reading_pose32.h"
#include "vrik/grab_reach43.h" // C43: VR grab window (file scope: coherent_pose.inl is included inside the private namespace)
#include "activity_eye59c19.h" // H51: alchemy own-head switch (file scope, same reason)
#include "vrik/visual_selection59c7f.h"
#include "vrik/smooth_lab_f2.h" // F2 rung2 visual policy only; physical targets remain raw.
#include "vrik/presentation_context55.h"
#include "vrik/alchemy_tool_contact_d55.h" // Namespace-owning trial state must be declared at file scope.
#include "action_time_owner59c11.h" // R59.16C10R12B: owned-action wrist policy
#include "vrik/swing_whoosh_r116.h" // R116: weapon whoosh at the physical release (native_input117.inl)
#include "diagnostics/hook_cost178r22.h" // V66.178R22b per-hook cost
#include "diagnostics/pose_cost_d60.h" // D60 finite F2 pose substage costs
#include "diagnostics/passive_contact169r1.h"
#include "vrik/world_query_contract.h"
#include "core/snapshot_mailbox.h"
#include "vrik/stowed_render_pose_d63.h"
#include "vrik/fist_action_h12.h"
#include "vrik/contact_budget.h"
#include <chrono>
#include "frik_pose_probe.h"
#include "vrik/quick_talk_owner_h4.h"
#include "subsystems/doors/hand_only_h40.h"
#include "subsystems/alchemy/finish_observer_cb1.h"
#include "subsystems/throw_drop/grip_stow_cb1.h"
#include "subsystems/alchemy/bridge67.h"
#include "vrik/dialogue_native_head28.h"
#include "interaction_alignment.h"
#include "core/door_candidates38.h"
#include "core/native_door19c4.h"
#include "core/door_use46.h"
#include "core/door_surface65.h"
#include "core/native_door_start19c4.h"
#include "core/native_door_pins19c4.h"
#include "core/native_door_script19c4r4.h"
#include "core/native_door_hold65.h"
#include "core/native_door_motion66.h"
#include "core/door_motion_native66.h"
#include "core/native_door_fingers66.h"
#include "core/door_path66.h"
#include "door_signal19c4r4.h"
#include "held_map_m1.h"
#include "subsystems/vrmap/native_paper_m43.h"
#include "subsystems/vrmap/map_hand_owner_m34.h"
#include "tracking_sample59c6.h"
#include "subsystems/throw_drop/pending_reach_t23.h"
#include "subsystems/throw_drop/native_ammo_service_t22.h"
#include "subsystems/throw_drop/hand_claim_t24.h"
#include "combat_diag_probe.h"
#include "diagnostics/outcome_chain23.h" // C23b OUTCOME_CHAIN receipts (read-only)
#include "vrik/pose_math.h"
#include "vrik/physical_weapon.h"
#include "vrik/native_rejoin_r28.h"
#include "diagnostics/hit_chain_c54r3.h"
#include "diagnostics/vrik_trace_h29.h"
#include "diagnostics/native_palette_d67r1.h"
#include "diagnostics/performance_metrics_d61.h"
#include "diagnostics/motion_trace_d68.h"
#include "diagnostics/frame_summary_d30.h" // D30 F2 FRAME / F7 SUMMARY
#include "diagnostics/physics_stage_d42.h"
#include "subsystems/experimental/free_targeting_d42.h"
#include "subsystems/ranged_reload/arrow_hold_d48.h"
#include "diagnostics/interaction_snapshot_d42.h"
#include "vrik/lab58.h"
#include "vrik/lab_prop58.h"
#include "vrik/static_grab_r89.h" // R90 (R89 pickup): small scenery props in the hand (file scope)
#include "vrik/pickup_use_fallback_r120.h" // R120 pickup Use fallback
#include "vrik/self_collision_r103.h" // R103: held prop / dragged corpse do not collide with Henry
#include "vrik/blood49.h" // C49 BLOODY FIGHTS (pure)
#include "vrik/gorepak50.h" // C50 separate optional gore pak (pure)
#include "vrik/gore_skin_path_d52.h" // D52 exact attachment model, no pointer scan
#include "vrik/combo_test_ds.h" // DS-F6 VrComboTestForce (file scope: never inside the native namespace)
#include "vrik/visible_failure.h" // DS-F12 visible failure line policy (file scope)
#include "subsystems/ranged_reload/rifle_hint_d32.h" // D32 rifle reload hint/haptic/deadline (file scope)
#include "subsystems/masterstrike_combos/combo_zone_assist_cz.h" // I12 VrComboZoneAssist (file scope)
#include "vrik/contact_glide_w5.h" // W5 expired contact glide (file scope)
#include "vrik/contact_hold_r11.h" // R11K: clear free hand follows the request when its query is late
#include "vrik/finger_rig_w1.h" // W1: finger chains by name crc on any Henry rig + captured fist default
#include "vrik/elbow_pole_w1.h" // W1: relaxed-arm elbow pole (back, not sideways)
#include "vrik/constraint_op60.h" // DS-F20 constraint op queue (file scope: namespace kcd2vr header)
#include "core/door_pre_press66.h" // DS-F16 pre-press door descriptor cache (file scope: namespace kcd2vr header)
#include <filesystem> // C50 gore pak presence check
#include "vrik/bow_camera35.h"
#include "vrik/bow_camera_identity_d37b.h"
#include "vrik/recovery_retire65h.h"
#include "vrik/motion_camera47.h"
#include "vrik/tracked_frame51.h"
#include "vrik/native_crouch_r93.h"
#include "subsystems/roomscale/settings_d76.h"
#include "subsystems/roomscale/tracked_step_d76.h"
#include "vrik/roomscale58.h"
#include "vrik/riding_vrik_d76.h"
#include "vrik/finisher_upper_d83.h" // D83 finisher
#include "vrik/station_body_h41.h"
#include "vrik/station_ws_j27.h" // J27
#include "vrik/body_follow_r38.h" // controller-authoritative head-led body posture
#include "subsystems/smithing/anvil_a2.h" // optional A2 learned anvil
#include "subsystems/smithing/anvil_physics_a3.h" // R96: GetEntitiesInBox contract reused by static_grab_r89.inl (read only)
#include "vrik/dagger_chest54.h"
#include "vrik/utility_gestures54.h"
#include "vrik/native_torch62.h"
#include "vrik/phys_bodies59c10r8.h" // R59.16C10R8
#include "vrik/part_anchor59c7g.h"
#include "vrik/carry_snapshot59c7g.h"
#include "core/impact_identity59c6.h"
#include "core/player_identity37.h" // C37 exact player identity ledger
#include "vrik/feature_probe59c6.h"
#include "vrik/bow_grip178r36c1.h"
#include "vrik/bow_geometry59c10r2.h"
#include "vrik/environment_fx178r22c1.h"
#include "vrik/own_shield_contact58.h"
#include "vrik/support_grip166.h"
#include "vrik/handling_profile59c10r2.h"
#include "vrik/grip_binding59c10r3.h"
#include "vrik/native_transform166.h"
#include "vrik/f11_tests167.h"
#include "vrik/physical_handoff132.h"
#include "vrik/impact_haptics133.h"
#include "retail_build112.h"
#include "vrik/weapon_sweep112.h"
#include "vrik/attack_xi178r8.h"
#include "vrik/combat_rules178r10.h" // V66.178R17
#include "vrik/vr_combat_ownership111.h" // V66.178R28: clip hiding scoped to the geometry rung
#include "subsystems/base/counter_step_r160.h" // R160 VrCombatNoStep counter-step rules (used by native_motion112.inl)
#include "vrik/workstation178r20c1.h"
#include "vrik/reach_view38.h" // C38 F5 rung 1 reach/visibility receipt (pure)
#include "vrik/alchemy_tool18.h"
#include "vrik/smith_tool36.h"
#include "subsystems/smithing/native_floor_d56.h" // namespace-bearing policy must enter at file scope
#include "subsystems/smithing/bridge67.h"
#include "subsystems/smithing/smith_contact68.h"
#include "vrik/alchemy_stage19c4.h"
#include "vrik/alchemy_semi46.h"
#include "vrik/hand_shape59c7f.h"
#include "vrik/contact_occupancy_d52.h"
#include "vrik/hand_pair_contact_d39.h"
#include "vrik/self_contact54.h"
#include "vrik/physical_guard112.h"
#include "vrik/guard_motion168.h"
#include "vrik/guard_beat_h5.h"
#include "vrik/native_bridges168.h"
#include "vrik/master_gate47.h" // C47 master-strike gate policy
#include "subsystems/masterstrike_combos/preopen_stance_h14.h" // H14 retain an already measured pre-open stance
#include "subsystems/masterstrike_combos/lineage_policy.h" // H14 observe an exact native-open combo lease at input
#include "subsystems/masterstrike_combos/physical_finisher_cb1.h" // CB1 strike kind for the physical combo finisher
#include "diagnostics/intent168.h"
#include "vrik/physical_native_contract.h"
#include "native_weapon_body_t4.h"
#include "subsystems/physics/runtime/native_weapon_contact_event_h24.h"
#include "vrik/native_weapon_parry_join_h24.h"
#include "vrik/native_held_pose_t4.h"
#include "vrik/native_held_owner_t4_status.h"
#include "vrik/hybrid_serial59.h"
#include "vrik/tracking_grace111.h"
#include "vrik/dialogue_fingers_d78.h"
#include "native_stereo_outer_lab.h"
#include "vrik/elbow_continuity.h"
#include "vrik/arm_constraints.h"
#include "vrik/upper_body.h"
#include "vrik/elbow_shoulder_d45.h"
#include "vrik/contact_admission_d74.h"
#include "vrik/archer_pose169r2.h"
#include "diagnostics/bow_contact169r2.h"
#include "vrik/finger_surface.h"
#include "vrik/contact_release_d45.h" // namespace header: file scope, before private native includes
#include "vrik/contact_balanced_d79.h" // namespace header: file scope
#include "vrik/finger_contact_h31.h"
#include "vrik/station_hand_contact_d75.h"
#include "vrik/finger_tuning40.h"
#include "vrik/physical_hand_owner55.h"
#include "vrik/knuckle_contact174.h"
#include "vrik/body_contact164.h"
#include "vrik/belt_layout164.h"
#include "diagnostics/combat_trace164.h"
#include "diagnostics/interaction_lifecycle165.h"
#include "diagnostics/interaction_receipt59c10.h"
#include "diagnostics/reload_lifecycle178r36c2.h"
#include "vrik/staged_hold596c2.h"
#include "vrik/prop_drive5916c1.h"
#include "vrik/prop_parent_r153.h" // R153: top level (staged_pickup596c2.inl is included inside a namespace)
#include "vrik/instant_drop_r158.h" // R158: top level too (dropped instant item falls; staged_pickup596c2.inl + static_grab_r89.inl)
#include "diagnostics/optional_groups175.h"
#include "diagnostics/parry_trace176.h"
#include "vrik/contact_authority176.h"
#include "vrik/contact_handoff59c3.h"
#include "vrik/native_answer59c4r2.h"
#include "vrik/kick176.h"
#include "vrik/kick_style26.h" // C26 KICKS page
#include "vrik/kick_hit34.h" // C37 VR leg kick transaction
#include "vrik/fist_motion.h"
#include "diagnostics/punch_path_c54r3.h"
#include "vrik/contact_projection44.h"
#include "vrik/rigid_contact_d67r1.h"
#include "experiments/contact_handover_d71.h"
#include "vrik/native_combat104.h"
#include "vrik/align_veto_receipts24.h" // C25 alignment-veto receipts (used by native_motion112.inl)
#include "vrik/attack_timing178r22c2.h"
#include "vrik/attack_clock50c2.h"
#include "subsystems/masterstrike_combos/combo_recovery_hold_r14.h" // N2 (I12): combo recovery hold
#include "subsystems/masterstrike_combos/combo_carrier_cb.h" // D30-CMB: N2 publishes the carrier window
#include "vrik/native_admission147.h"
#include "core/haptics.h"
#include "diagnostics/combat_capture.h"
#include "vrik/calibration_profile.h"
#include "vrik/default_profile_r149.h" // R149: default arm profile when there is no saved F8 T-pose
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include "core/config_file_guard_d62.h"
#include "core/resident_pages126.h"
#include <intrin.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <optional>
#include <mutex>
#include <utility>
#include <vector>
#include "diagnostic_trace.h"
#include "debug_trace.h"
#include "animation_filter.h"
#include "subsystems/interactions/native_passenger_a34.h"
#include "diagnostics/lab_state.h"
#include "vrik/part_namespace62.h"
#include "vrik/weapon_exit_d63.h"
#include "subsystems/gore/impale_observe_h7.h"
#include "subsystems/experimental/claude_options_h8.h"
#include "subsystems/gore/corpse_contact_h7.h"

#include "subsystems/physics/runtime/native_hand_presence_pose_h24.h"
#include "subsystems/physics/runtime/dialogue_hand_presence_h24.h"
namespace kcd2vr::frik_pose_probe {
// Shared by private physics consumers and the later body-observer definition.
static bool InteractionBodyEntryQualifiedR93(std::uintptr_t base) noexcept;
static void CancelKickServiceForR3H24(std::uint64_t,std::uint64_t) noexcept;
namespace {
static std::atomic<int> g_f11_test167{0};
static kcd2vr::f11_tests167::Lease g_f11_lease167; // mod control thread only
static bool F11Capture167(int rung) noexcept {
    namespace capture=kcd2vr::diagnostics::combat_capture;
    return FocusedTestMode167()==rung&&capture::active(GetTickCount64())&&
        (capture::sourceMask.load()&(rung==1?capture::F11Hands167:capture::F11Stabs167));
}
constexpr std::uint32_t  kSupportedWhTimestamp       = 0x6A350E20u;
constexpr std::uintptr_t kSetIkTargetsRva            = 0xA7F9CCull;
constexpr std::uintptr_t kPoseGetterBuffer0Rva       = 0x53F550ull;
constexpr std::uintptr_t kPoseGetterBuffer1Rva       = 0x53D830ull;
constexpr std::uintptr_t kPoseSetterBuffer0Rva       = 0x53F510ull;
constexpr std::uintptr_t kPoseSetterBuffer1Rva       = 0x53F4D0ull;
constexpr std::uintptr_t kPoseBufferSelectorRva      = 0x53F580ull;
constexpr std::uintptr_t kSkinningTransformCoreRva   = 0x5FE2F0ull;
// V65.05 exact-WHGame native procedural IKTarget runtime path.
// Recovered statically from C_ProceduralClipIKTargetContext RTTI/vtable and
// direct callsites in the supplied WHGame.dll. All hooks below are observation-only.
constexpr std::uintptr_t kIkTargetContextUpdateRva    = 0x145DCBCull;
constexpr std::uintptr_t kIkTargetBridgeRva           = 0x145E558ull;
constexpr std::uintptr_t kIkTargetPoseWriterRva       = 0xA7F6D0ull;
constexpr std::uintptr_t kIkTargetBlendWriterRva      = 0xA80150ull;
constexpr std::uintptr_t kIkTargetContextVftRva       = 0x3BD6180ull;
constexpr std::uint32_t  kRgtArm01Crc                 = 0xFE062C68u;
constexpr std::uint32_t  kLftArm01Crc                 = 0x314004C5u;
constexpr std::ptrdiff_t kSemanticJointTableOffset   = 0x17C;
constexpr std::ptrdiff_t kPoseDefaultSkeletonOffset   = 0x190; // V64.7 runtime: pose+400 decimal repeatedly points to CDefaultSkeleton
constexpr std::uintptr_t kCDefaultSkeletonVftRva      = 0x406B930ull; // exact retail RTTI primary vftable
constexpr std::uintptr_t kCDefaultJointCountRva        = 0x1AAF020ull; // static: returns count from skeleton joint array
constexpr std::uintptr_t kCDefaultJointByNameRva       = 0x78BC4Cull;  // exact retail CDefaultSkeleton name-hash lookup
constexpr std::uintptr_t kCCharInstanceVftRva          = 0x40602F8ull;
constexpr std::uintptr_t kCSkeletonAnimVftRva          = 0x3A711E0ull;
// V65.28: exact primary vtable recovered statically for retail C_ItemAttachmentManager.
// F10 rung 3 only scans for this owner; it performs zero item/weapon writes.
constexpr std::uintptr_t kCItemAttachmentManagerVftRva = 0x3EC9958ull;
// V65.83 libKCD2/script-informed non-combat Henry identity chain.
constexpr std::uintptr_t kCryActionGlobalRva          = 0x549D328ull;
constexpr std::uintptr_t kCryActionVftRva             = 0x40472D0ull;
constexpr std::uintptr_t kActionGameVftRva            = 0x3AA5988ull;
constexpr std::uintptr_t kPlayerModuleVftRva          = 0x3AA4B98ull;
constexpr std::uintptr_t kCEntityVftRva               = 0x4072C78ull;
constexpr std::uintptr_t kCEntityGetCharacterRva      = 0x6411ACull;
// V65.47 exact-build item/weapon path recovered from supplied WHGame.dll.
// +0x8D00C4 consumes a concrete item object in RDX plus a side/type in R8D.
// +0xA80E68 consumes Mannequin animation events including AttachRight/AttachLeft.
constexpr std::uintptr_t kItemAttachOperationRva       = 0x8D00C4ull;
constexpr std::uintptr_t kAttachEventDispatchRva      = 0xA80E68ull;
// V65.48: native global WUID registry lookup used by GetEntityIdByWUID Lua binding.
constexpr std::uintptr_t kWuidResolverRva              = 0x47C1F8ull;
// V65.73: native attachment-helper transform resolver. Retail callers pass
// (CAttachmentManager*, helperName, QuatT28*, bool). The supplied WHGame.dll
// has direct runtime slt_0 callers that immediately feed this result into
// object placement. This is the downstream hand->slt_0 boundary F12 should own.
constexpr std::uintptr_t kAttachmentHelperTransformRva  = 0xB309DCull;
// V66.88 exact retail CEntityAttachment::UpdateAttachment boundary. Static RTTI/vtable
// recovery from the supplied WHGame.dll proves vtbl[1]=GetAttachmentType(4) and
// vtbl[2]=this update method. It resolves self+0x8 entityId, reads the native
// attachment QuatTS and applies position/quaternion/scale to the bound entity.
constexpr std::uintptr_t kEntityAttachmentUpdateRva     = 0x4238B8ull;
constexpr std::ptrdiff_t kSkeletonAnimOffset           = 0x110;
constexpr std::ptrdiff_t kPoseInCharInstanceOffset     = 0x8E0; // V64.49/V64.50 runtime: validated Henry CPoseData - CCharInstance
constexpr float kMaxControllerDeltaMeters            = 0.30f;

using SetIkFn = void(__fastcall*)(void*, void*);
using SkinningTransformCoreFn = void(__fastcall*)(void*);
using JointCountFn = std::uint32_t(__fastcall*)(void*);
using JointIdByNameExactFn = int(__fastcall*)(void*, const char*);
using PoseBufferSelectorFn = void* (__fastcall*)(void*, std::uintptr_t);
using IkTargetContextUpdateFn = void (__fastcall*)(void*, float);
using ItemAttachOperationFn = bool (__fastcall*)(void*, void*, int);
using AttachEventDispatchFn = void (__fastcall*)(void*, void*);
using WuidResolverFn = void* (__fastcall*)(void*, const void*);
using EntityIdGetterFn = std::uint32_t (__fastcall*)(void*);
struct Matrix34V6550 { float m[12]; };
using EntityWorldTmGetterFn = const Matrix34V6550* (__fastcall*)(void*);
using EntityTmSetterFn = void (__fastcall*)(void*, const Matrix34V6550*, std::uint32_t);
// MSVC x64 large-struct member return: hidden return buffer first, this second.
using EntityLocalTmGetterFn = void (__fastcall*)(Matrix34V6550*, void*);
static bool GetEntityWorldTmV6552(void* ent,std::uint32_t expected,Matrix34V6550& out) noexcept;
static bool GetEntityLocalTmV6552(void* ent,std::uint32_t expected,Matrix34V6550& out) noexcept;

#pragma pack(push,1)
struct QuatT28 { float q[4]; float t[3]; };
struct Dq32 { float a[4]; float b[4]; };
#pragma pack(pop)
static_assert(sizeof(QuatT28)==0x1C);
using AttachmentHelperTransformFn = bool (__fastcall*)(void*, const char*, QuatT28*, bool);
using EntityAttachmentUpdateFn = void (__fastcall*)(void*, void*);
using AttachmentWorldQuatTsFn = void (__fastcall*)(void*, float*);
using EntitySetPosRotScaleFn = void (__fastcall*)(void*, const float*, const float*, const float*, std::uint32_t);
static_assert(sizeof(Dq32)==0x20);
using PoseGetFn = const QuatT28* (__fastcall*)(void*, int);
using PoseSetFn = void (__fastcall*)(void*, int, const QuatT28*);
using IkTargetBridgeFn = void (__fastcall*)(void*, void*, const QuatT28*, float);
using IkTargetPoseWriterFn = bool (__fastcall*)(void*, const char*, const std::int64_t*, const QuatT28*, bool, bool);
using IkTargetBlendWriterFn = bool (__fastcall*)(void*, const char*, const std::int64_t*, float, bool);

struct RigMap {
    int pelvis{-1}, spine{-1}, spine1{-1}, spine2{-1}, spine3{-1}, neck{-1}, head{-1};
    int clav_l{-1}, upper_l{-1}, fore_l{-1}, elbow_l{-1}, hand_l{-1}, item_l{-1};
    int clav_r{-1}, upper_r{-1}, fore_r{-1}, elbow_r{-1}, hand_r{-1}, item_r{-1};
};

enum SemanticBoneId : int {
    BONE_PELVIS=2, BONE_SPINE=3, BONE_SPINE1=4, BONE_SPINE2=5, BONE_SPINE3=6,
    BONE_NECK=9, BONE_HEAD=10,
    BONE_HAND_L=18, BONE_HAND_R=19,
    BONE_ITEM_L=20, BONE_ITEM_R=21, // retail BONEID vocabulary: immediately follows HAND_L/R
    BONE_CLAVICLE_L=32, BONE_CLAVICLE_R=33,
    BONE_UPPERARM_L=36, BONE_UPPERARM_R=37,
    BONE_FOREARM_L=40, BONE_FOREARM_R=41,
    BONE_ELBOW_L=42, BONE_ELBOW_R=43
};

struct V3 { float x{},y{},z{}; };
struct Q4 { float x{},y{},z{},w{1.0f}; };
struct TrackingSnapshot {
    std::uint64_t sequence=0,tick=0,timeNs=0;
    std::int64_t poseTime=0; // XR locate target; never used as input/freshness time
    std::uint64_t input_tick=0,source_epoch=0;
    std::uint64_t head_good_tick=0,left_good_tick=0,right_good_tick=0;
    V3 head{}, left{}, right{};
    Q4 hq{}, lq{}, rq{};
    float left_grip=0.0f,right_grip=0.0f;
    float left_trigger=0.0f,right_trigger=0.0f;
    kcd2vr::dialogue_fingers_d78::VisualInput dialogueFingersD78{}; // presentation only
    bool head_valid=false,left_valid=false,right_valid=false,allowed=false;
    bool head_held=false,left_held=false,right_held=false; // V66.86 VRIK-only short dropout hold
    std::int64_t handTargetD69[2]{};unsigned handLeadSourceD69[2]{};std::uint64_t handPolicyD69=0;

};

static V3 Add(V3 a,V3 b) noexcept { return {a.x+b.x,a.y+b.y,a.z+b.z}; }
static V3 Sub(V3 a,V3 b) noexcept { return {a.x-b.x,a.y-b.y,a.z-b.z}; }
static V3 Mul(V3 a,float s) noexcept { return {a.x*s,a.y*s,a.z*s}; }
static float Dot(V3 a,V3 b) noexcept { return a.x*b.x+a.y*b.y+a.z*b.z; }
static V3 Cross(V3 a,V3 b) noexcept { return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; }
static float Len(V3 a) noexcept { return std::sqrt(std::max(0.0f,Dot(a,a))); }
static V3 Normalize(V3 a) noexcept { const float l=Len(a); return l>1.0e-6f?Mul(a,1.0f/l):V3{}; }
// Retired unused implementation archived: research_archive/retired_pose_implementations

static V3 FromT(const QuatT28& q) noexcept { return {q.t[0],q.t[1],q.t[2]}; }
static void ToT(QuatT28& q,V3 v) noexcept { q.t[0]=v.x;q.t[1]=v.y;q.t[2]=v.z; }
static Q4 QConj(Q4 q) noexcept { return {-q.x,-q.y,-q.z,q.w}; }
static Q4 QMul(Q4 a,Q4 b) noexcept {
    return {a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,
            a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
            a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,
            a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};
}
static Q4 QNormalize(Q4 q) noexcept { const float n=std::sqrt(std::max(0.0f,q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w)); return n>1.0e-6f?Q4{q.x/n,q.y/n,q.z/n,q.w/n}:Q4{}; }
static Q4 QNlerpIdentity(Q4 q,float t) noexcept { q=QNormalize(q); if(q.w<0.0f) q={-q.x,-q.y,-q.z,-q.w}; return QNormalize(Q4{q.x*t,q.y*t,q.z*t,1.0f+(q.w-1.0f)*t}); }
static Q4 QFromQuatT(const QuatT28& q) noexcept { return QNormalize(Q4{q.q[0],q.q[1],q.q[2],q.q[3]}); }
static void QToQuatT(QuatT28& q,Q4 v) noexcept { v=QNormalize(v); q.q[0]=v.x;q.q[1]=v.y;q.q[2]=v.z;q.q[3]=v.w; }
static Q4 XrToCryQ(Q4 q) noexcept { const Q4 basis{0.70710678118f,0,0,0.70710678118f}; return QNormalize(QMul(QMul(basis,QNormalize(q)),QConj(basis))); }
static V3 XrToCryV(V3 v) noexcept { return {v.x,-v.z,v.y}; }
static Q4 QFromBasis(V3 x,V3 y,V3 z) noexcept {
    // Columns are the local +X/+Y/+Z axes expressed in model space.
    x=Normalize(x); y=Normalize(y); z=Normalize(z);
    const float m00=x.x,m01=y.x,m02=z.x;
    const float m10=x.y,m11=y.y,m12=z.y;
    const float m20=x.z,m21=y.z,m22=z.z;
    Q4 q{}; const float tr=m00+m11+m22;
    if(tr>0.0f){const float s=std::sqrt(tr+1.0f)*2.0f;q.w=0.25f*s;q.x=(m21-m12)/s;q.y=(m02-m20)/s;q.z=(m10-m01)/s;}
    else if(m00>m11&&m00>m22){const float s=std::sqrt(std::max(0.0f,1.0f+m00-m11-m22))*2.0f;q.w=(m21-m12)/s;q.x=0.25f*s;q.y=(m01+m10)/s;q.z=(m02+m20)/s;}
    else if(m11>m22){const float s=std::sqrt(std::max(0.0f,1.0f+m11-m00-m22))*2.0f;q.w=(m02-m20)/s;q.x=(m01+m10)/s;q.y=0.25f*s;q.z=(m12+m21)/s;}
    else {const float s=std::sqrt(std::max(0.0f,1.0f+m22-m00-m11))*2.0f;q.w=(m10-m01)/s;q.x=(m02+m20)/s;q.y=(m12+m21)/s;q.z=0.25f*s;}
    return QNormalize(q);
}
static V3 QRotateV(Q4 q,V3 v) noexcept {
    q=QNormalize(q);
    const V3 u{q.x,q.y,q.z};
    const float ss=q.w;
    return Add(Add(Mul(u,2.0f*Dot(u,v)),Mul(v,ss*ss-Dot(u,u))),Mul(Cross(u,v),2.0f*ss));
}
// Retired unused implementation archived: research_archive/retired_pose_implementations

static QuatT28 ApplyRigidHandDelta(const QuatT28& handNative,const QuatT28& handGoal,const QuatT28& targetNative) noexcept {
    const Q4 delta=QNormalize(QMul(QFromQuatT(handGoal),QConj(QFromQuatT(handNative))));
    QuatT28 out=targetNative; QToQuatT(out,QMul(delta,QFromQuatT(targetNative)));
    const V3 rel=Sub(FromT(targetNative),FromT(handNative)); ToT(out,Add(FromT(handGoal),QRotateV(delta,rel))); return out;
}
static QuatT28 RelativeFromAbsolute(const QuatT28& parent,const QuatT28& child) noexcept {
    const Q4 ip=QConj(QFromQuatT(parent));
    QuatT28 out=child;
    QToQuatT(out,QMul(ip,QFromQuatT(child)));
    ToT(out,QRotateV(ip,Sub(FromT(child),FromT(parent))));
    return out;
}
static QuatT28 AbsoluteFromRelative(const QuatT28& parent,const QuatT28& rel) noexcept {
    QuatT28 out=rel; const Q4 pq=QFromQuatT(parent);
    QToQuatT(out,QMul(pq,QFromQuatT(rel)));
    ToT(out,Add(FromT(parent),QRotateV(pq,FromT(rel))));
    return out;
}

std::optional<DetourModKit::hook::Hook> g_setik_hook;
std::optional<DetourModKit::hook::Hook> g_skinning_core_hook;
std::optional<DetourModKit::hook::Hook> g_iktarget_context_hook;
std::optional<DetourModKit::hook::Hook> g_iktarget_bridge_hook;
std::optional<DetourModKit::hook::Hook> g_iktarget_pose_writer_hook;
std::optional<DetourModKit::hook::Hook> g_iktarget_blend_writer_hook;
std::optional<DetourModKit::hook::Hook> g_item_attach_operation_hook;
std::optional<DetourModKit::hook::Hook> g_attach_event_dispatch_hook;
std::optional<DetourModKit::hook::Hook> g_wuid_resolver_hook;
std::optional<DetourModKit::hook::Hook> g_attachment_helper_transform_hook;
std::optional<DetourModKit::hook::Hook> g_entity_attachment_update_hook;
SetIkFn g_setik_original=nullptr;
SkinningTransformCoreFn g_skinning_core_original=nullptr;
IkTargetContextUpdateFn g_iktarget_context_original=nullptr;
IkTargetBridgeFn g_iktarget_bridge_original=nullptr;
IkTargetPoseWriterFn g_iktarget_pose_writer_original=nullptr;
IkTargetBlendWriterFn g_iktarget_blend_writer_original=nullptr;
ItemAttachOperationFn g_item_attach_operation_original=nullptr;
AttachEventDispatchFn g_attach_event_dispatch_original=nullptr;
WuidResolverFn g_wuid_resolver_original=nullptr;
AttachmentHelperTransformFn g_attachment_helper_transform_original=nullptr;
EntityAttachmentUpdateFn g_entity_attachment_update_original=nullptr;
std::atomic<void*> g_last_concrete_item_object{nullptr};
std::atomic<void*> g_last_item_attach_owner{nullptr};
std::atomic<int> g_last_item_attach_side{-1};

// V65.49: recent concrete-item WUID ring. C_Item stores its 16-byte runtime WUID
// at +0x30..+0x3F; the global WUID resolver is extremely hot, so F11 only
// performs RTTI/entity work when a resolver input exactly matches one of these
// recently observed item WUIDs.
constexpr std::size_t kRecentItemWuidSlots=16;
std::atomic<std::uint64_t> g_recent_item_wuid0[kRecentItemWuidSlots]{};
std::atomic<std::uint64_t> g_recent_item_wuid1[kRecentItemWuidSlots]{};
std::atomic<std::uint64_t> g_recent_item_wuid_tick[kRecentItemWuidSlots]{};
std::atomic<std::uint32_t> g_recent_item_wuid_cursor{0};
std::atomic<std::uint64_t> g_item_attach_calls{0},g_attach_event_calls{0},g_wuid_resolve_calls{0};
std::atomic<std::uint32_t> g_last_wuid_entity_id{0};
std::atomic<void*> g_last_wuid_object{nullptr},g_last_wuid_entity_iface{nullptr};
// V65.57 F12 held-chain ownership ladder. Side 0/1 follows the native item-operation side.
// Rung 1 reuses the proven F10 two-arm/hand writer; later rungs ADD held-item layers.
// A missing later-layer target must never cancel an earlier successful hand/item write.
std::atomic<int> g_weapon_control_mode{0}; // V66.85: attachment-owner ladder only; retired slt_0/ITEM/CEntity/sword_pos writes remain disabled
std::atomic<void*> g_henry_item_attach_manager{nullptr};
std::atomic<std::uint64_t> g_weapon_control_wuid0[2]{};
std::atomic<std::uint64_t> g_weapon_control_wuid1[2]{};
std::atomic<void*> g_weapon_control_entity[2]{};
std::atomic<std::uint32_t> g_weapon_control_entity_id[2]{};
std::atomic<std::uint64_t> g_weapon_control_generation[2]{};
// V65.57 continuously remember the latest native side-qualified item operation so
// entering F12 rung 3 does not require the user to redraw/re-attach the weapon.
std::atomic<void*> g_native_side_item[2]{nullptr,nullptr};
#include "subsystems/ranged_reload/bow_character_state.inl" // M2_OWNED_SOURCE
#include "subsystems/dialogues/body_state.inl" // M2_OWNED_SOURCE
#include "subsystems/ranged_reload/reload_observer_state.inl" // M2_OWNED_SOURCE
std::atomic<void*> g_native_side_owner[2]{nullptr,nullptr};
std::atomic<std::uint64_t> g_native_side_wuid0[2]{};
std::atomic<std::uint64_t> g_native_side_wuid1[2]{};
std::atomic<int> g_native_side_item_state[2]{{-1},{-1}}; // -1 UNKNOWN, 0 EMPTY, 1 OCCUPIED (exact Henry manager only)
std::atomic<std::uint64_t> g_native_side_item_tick[2]{};
// V66.86: correlate the attach-event object immediately preceding an exact Henry
// C_ItemAttachmentManager operation. Read-only until a downstream ABI is proven.
std::atomic<void*> g_last_attach_event_self{nullptr},g_last_attach_event_obj{nullptr};
std::atomic<std::uint32_t> g_last_attach_event_hash{0};
std::atomic<std::uint64_t> g_last_attach_event_tick{0};
std::atomic<void*> g_exact_attach_event_self[2]{nullptr,nullptr};
std::atomic<void*> g_exact_attach_event_obj[2]{nullptr,nullptr};
std::atomic<std::uint32_t> g_exact_attach_event_hash[2]{};
std::atomic<void*> g_weapon_control_registry{nullptr};
// V66.88 final visible-child attachment proof state. The native CEntityAttachment
// update always runs first, so every proof frame starts from KCD2's fresh placement
// and cannot accumulate. Leaving rung 2 restores native placement on the next update.
std::atomic<std::uint64_t> g_f12_attachment_proof_until_ms{0};
std::atomic<std::uint64_t> g_f12_attachment_proof_writes{0};
std::atomic<bool> g_f12_readback_ok6689{false},g_f12_restore_ok6689{false};
std::atomic<std::uint64_t> g_f12_verified_generation6689{0};
std::atomic<bool> g_f12_follow_requested6689{false};
float g_committed_model_world6689[8]{};
// V66.89 actor/calibration generation and committed-pose publication. The stamp
// is copied only under g_f12_hand_delta_lock; atomics below never authorize a stale copy.
std::atomic<std::uint64_t> g_pose_epoch6689{1},g_tracking_tick6689{0};

// F5=1 only: actual successful native smith-tool placement, never a requested
// wrist or an empty-hand proxy. The native impact observer only reads this POD.
static kcd2vr::core::SnapshotMailbox<kcd2vr::subsystems::smithing::contact68::HammerFrame> g_smith_placed68;
std::atomic<bool> g_pose_restore_fault6689{false},g_committed_pose_valid6689{false};
std::atomic<void*> g_committed_character6689{nullptr};
kcd2vr::pose_core::Stamp g_committed_stamp6689{};
std::atomic_flag g_f12_hand_delta_lock=ATOMIC_FLAG_INIT;
QuatT28 g_f12_native_hand[2]{};
QuatT28 g_f12_vr_hand[2]{};
bool g_f12_hand_delta_valid[2]{false,false};
// Null seams let the existing isolated pose tests retain their original native-tool path.
static bool (*g_graspEnabled13)() noexcept=nullptr;
// C61 safe C60 subset: 0 Native, 2 physical staged hold; unsafe Instant(1) deliberately refused.
static std::atomic<int> g_grab_mode60{0};
static bool (*g_graspBinding13)(kcd2vr::grasp13::Binding&) noexcept=nullptr;
static void (*g_graspHands13)(const kcd2vr::grasp13::HandFrame&) noexcept=nullptr;
static void (*g_graspInput55)(const kcd2vr::grasp13::AcquisitionInput55&) noexcept=nullptr;
static void (*g_graspPlaced13)(const kcd2vr::grasp13::Placement&) noexcept=nullptr;
static void (*g_graspSocket28)(const kcd2vr::grasp13::SocketProof28&) noexcept=nullptr;
struct ToolSocket59c10 {
    void* self=nullptr;void* attachment=nullptr;void* character=nullptr;void* entity=nullptr;
    std::uint32_t id=0;int side=-1,joint=-1;QuatT28 relative{};
    std::uint64_t epoch=0,tick=0,request=0;bool nativeGrasp13=false;
    kcd2vr::alchemy_tool18::Binding alchemy18{};
    kcd2vr::smith_tool36::Binding smith36{}; // C37: exact native smithing tool, controller auto-hold
    QuatT28 grip67{};bool gripReady67=false; // exact first native tool grip, for a current Gesture lease only.
    std::uint64_t modeEpoch67=0;
};
struct ToolPose59c10 {ToolSocket59c10 socket{};QuatT28 model{};bool valid=false;
    QuatT28 parkedWorldD47{};bool parkedD47=false;
    QuatT28 nativeObserved28{};bool nativeObservedKnown28=false; // diagnostic data only, never placement authority
    kcd2vr::grasp13::FingerReference55 fingers55{};};
#include "vrik/manual_arrow_types64.inl"
// D63: named equipment sockets only. Runtime joint indices are resolved once
// per actual skeleton; compiled-asset indices are not runtime indices.
struct StowedSlotD63 { const char* attachment; const char* bone; };
static constexpr StowedSlotD63 kStowedSlotsD63[]{
    {"weapon_holster_right","rightweaponslot"},
    {"weapon_holster_left","leftweaponslot"},
    {"weapon_dagger","DaggerSlot"},
    {"weapon_bow","BackSlot_bow"},
    {"weapon_crossbow","BackSlot_crossbow"},
    {"weapon_cannon","BackSlot_cannon"},
    {"weapon_shield","BackSlot_shield"},
    {"weapon_quiver","BackSlot_quiver"},
    {"weapon_quiver_ammo","BackSlot_quiver"},
    {"scabbard_left","LeftWeaponSheath"}
};
struct StowedPoseD63 { QuatT28 model{}; int joint=-1; bool valid=false; };
// R117: Henry BONE attachments drawn under OTHER names than the 10 above (the
// polearm / two-hander on the back: R116 log 02:47-02:59 and the 5 Nov log 00:46-00:49 carried
// one and no named slot drew it). The render leaf records the joint it draws;
// the completed body publishes that joint on the solved torso. Reset per
// skeleton (initRuns137) and character, so every load/VRIK restart registers again.
#include "vrik/stowed_extra_r117.h"
static StowedExtraTableR117 g_stowed_extraR117;
#include "vrik/alchemy_contact_result_d80.h"
// R95: reload prop target and source are copied in the same completed body.
struct RangedSocketXR34 {
    kcd2vr::crossbow_native_xr34::Owner owner{};void* self=nullptr;void* attachment=nullptr;void* entity=nullptr;void* character=nullptr;
    std::uint64_t toolKey=0;void* toolItem=nullptr;std::uint32_t id=0;int side=-1,joint=-1;QuatT28 relative{},helper{};bool bolt=false;
};
struct RangedTargetXR34 {RangedSocketXR34 socket{};kcd2vr::pose_core::Stamp stamp{};QuatT28 model{},grip{};V3 wrist{};std::uint64_t publicationR95=0;bool valid=false;};
struct CommittedFrame6695 {
    kcd2vr::bow_draw_publication_r93::Source bowDrawR93{};
    kcd2vr::crossbow_native_xr34::Source reloadSourceXR34{};
    RangedTargetXR34 reloadTargetXR34{};
    double nativeCrouchOffsetR93=0; // exact stance applied to this completed model pose
    float mapGripM39[2]{}; // exact completed tracking packet; map gesture and pickup admission
    float pickupTriggerD80[2]{}; // same packet, per-hand fist ownership (never selected-mode-only)
    bool mapFoldTrackedM39=false; // optional gesture authority, never display grace
    kcd2vr::pose_core::Stamp stamp{};
    QuatT28 native[2]{},allowed[2]{},propRequested5916c1[2]{};float world[8]{};bool valid[2]{},propRequestedValid5916c1[2]{};
    // D62: allowed/palm/fist slots are the completed semantic collision/IK pass.
    // Future display has its own explicit provenance and is never action input.
    QuatT28 displayedD62[2]{};bool predictedDisplayD62[2]{};
    std::uint64_t displayPolicyD62=0;std::int64_t displayTargetTimeD62=0;
    std::int64_t handTimeNsR118=0; // R118: headset time (XrTime ns) this sample's hands were located for; 0 unknown
    V3 palm59c9[2]{},palmDirection59c9[2]{};bool palmValid59c9[2]{};
    V3 nativeHead122{},solvedHead122{},targetHead122{},pelvis122{};bool headEvidence122=false;
    ToolPose59c10 tools59c10[2]{};
    AlchemyContactResultD80 alchemyContactD80[2]{};
    kcd2vr::subsystems::smithing::bridge67::FixturePose smithFixture[2]{};
    std::uint32_t mapPresentationOwnerM34[2]{kcd2vr::map_hand_owner_m34::Unknown,kcd2vr::map_hand_owner_m34::Unknown};
    ManualArrowPose64 arrow64{};
    std::uintptr_t doorPlayer38=0; // producing CEntity paired with stamp.actor/epoch, otherwise unknown
    std::uint64_t doorTrackingEpoch66=0;
    // Exact semantic hand/item topology of THIS completed character pose.
    // Used only to map the original fist collision bone to its own hand.
    std::uintptr_t fistSkeletonH31=0;std::uint32_t fistJointCountH31=0;
    int fistWristBoneH31[2]{-1,-1},fistSlotBoneH31[2]{-1,-1};
    QuatT28 fistSlotH31[2]{};bool fistSlotValidH31[2]{};
    QuatT28 originalToolSlotD71[2]{};bool originalToolSlotValidD71[2]{};std::uint64_t itemHandoverRevisionD71=0;
    // D63: final, successfully committed model-space socket bones travel WITH
    // their body publication. No persistent native bone/cache write is needed.
    StowedPoseD63 stowedD63[10]{};
    StowedPoseD63 stowedExtraR117[kStowedExtraR117]{}; // R117: same rule for unnamed torso sockets
    std::uint64_t stowedPolicyD63=0;
    std::uint64_t publicationR24=0; // exact completed body identity; visual attachment join only
};
#include "subsystems/dialogues/view_head_state.inl" // M2_OWNED_SOURCE
static kcd2vr::core::SnapshotMailbox<CommittedFrame6695> g_committed_frame6695;
static kcd2vr::core::SnapshotMailbox<ToolSocket59c10> g_tool_socket59c10[2];
static bool ToolAttachment59c10(void*,void*) noexcept;
static void WeaponDrawBeforeAttachmentD75(void*,void*) noexcept;
static void WeaponDrawAfterNativeAttachD75(const kcd2vr::weapon_draw_d75::NativeAttachScope&) noexcept;
static void CraftAttachmentReceipt36b(void*,void*,std::uint64_t) noexcept;
static void HandAttachmentReceipt28(void*,void*,std::uint64_t) noexcept;
// Observe after whichever existing attachment owner actually completed. This
// never places an object or implies that the original callback ran.
struct AttachmentObservation15 {
    void* self;void* attachment;
    ~AttachmentObservation15() noexcept {
        if(diagnostics::capture168::gate.mask.load(std::memory_order_relaxed)&(1u<<unsigned(diagnostics::capture168::Domain::HandReference)))
            HandAttachmentReceipt28(self,attachment,std::uint64_t(GetTickCount64()));
        if(!(diagnostics::capture168::gate.mask.load(std::memory_order_relaxed)&
             (1u<<unsigned(diagnostics::capture168::Domain::Interaction))))return;
        CraftAttachmentReceipt36b(self,attachment,std::uint64_t(GetTickCount64()));
    }
};
static bool CrossbowAttachmentXR34(void*,void*) noexcept;
static bool ObserveNativeActivityHand46(void*,void*) noexcept;
static void PublishGuidedTools55(const CommittedFrame6695&,std::uint64_t) noexcept;

// V65.73: exact Henry character attachment-manager correlation captured from the
// already-proven CHARACTER-MATCH skinning path. slt_0 helper calls matching this
// owner are safe local-player attachment propagation candidates. The thread/tick
// pair is a bounded fallback for native helper work performed immediately after
// Henry skinning on the same animation thread.
std::atomic<void*> g_henry_char_attachment_manager{nullptr};
std::atomic<DWORD> g_henry_skinning_thread{0};
std::atomic<std::uint64_t> g_henry_skinning_tick{0};
std::atomic<std::uint64_t> g_slt0_helper_calls{0},g_slt0_helper_owned_calls{0},g_slt0_helper_writes{0};
std::atomic<std::uint64_t> g_iktarget_context_calls{0},g_iktarget_bridge_calls{0},g_iktarget_pose_calls{0},g_iktarget_blend_calls{0};
std::atomic<std::uint64_t> g_iktarget_context_last_log{0},g_iktarget_bridge_last_log{0},g_iktarget_pose_last_log{0},g_iktarget_blend_last_log{0};

std::atomic<int> g_mode{0}; // V65.02 F9: read-only ownership/animation diagnostic ladder
std::atomic<std::uint64_t> g_contact_control_epoch6695{1};
static std::atomic<bool> g_combat_experiment175{false};
static std::atomic<bool> g_physical_combat177{false};
std::atomic<bool> g_contact_starts_attack178r41{true}; // V66.178R41 INI VrContactStartsAttack
std::atomic<bool> g_attack_at_prepare178r42{true}; // V66.178R42 INI VrAttackAtPrepare
std::atomic<float> g_weapon_authority_scale178r44{0.6f}; // V66.178R44 INI VrWeaponWeightFeel (authority scale)
static std::atomic<bool> g_combat_full178r10{false}; // V66.178R10 F4 rung 3
static std::atomic<bool> g_combat_integrated178r22c2{false};
static std::atomic<int> g_combat_rung178r15{0};        // V66.178R15: 3,4,5 inside F4 rung 3+
// VrFistNativeArm retired in R58C2: punch preparation cannot seize the visible arm.
// V66.178R15 impale latch glue: the kernel (pose thread) asks for a pin on an entity; the game thread
// resolves the entity transform each tick and publishes the pin in world space.
struct ImpalePin178r15 {bool valid=false;std::uint64_t serial=0,tick=0;float x=0,y=0,z=0;int mode40=0;float depth40=0;}; // C40: 1 STUCK (depth cap m) / 2 HANG
static kcd2vr::core::SnapshotMailbox<ImpalePin178r15> g_impale_pin178r15;
// C40: the impaled victim's own entity/body while a C40 latch holds the blade in him. Added ONLY to our physical blade's
// ray-query skip list (the native RayWorldIntersection skip-entities argument of THIS query), so the simulated blade is
// not pushed out of the wound. No native collision flag, part flag or entity state is changed; stale (>200 ms) = gone.
// R128: hangR128 = a CORPSE hanging on the blade (VrImpaleFullDepthR128); only that one is read (npc_contact172.inl).
struct ImpaleSkip40 {std::uintptr_t entity=0,body=0;std::uint64_t tick=0;std::uint32_t target=0;bool hangR128=false;};
static kcd2vr::core::SnapshotMailbox<ImpaleSkip40> g_impale_skip40;
static thread_local bool g_kick_query40=false; // C40: the R3 kick foot query still sees the impaled victim
static std::atomic<int> g_f4_contact_gate178r26{0};          // V66.178R26 (see SetF4ContactGate178r26)
static std::atomic<bool> g_support_hilt_region178r26{false}; // V66.178R26 (see SetSupportHiltRegion178r26)
static std::atomic<bool> g_two_hand_v120_178r27{true};       // V66.178R27: V66.120/123 two-hand socket (see SetTwoHandV120_178r27)
static std::atomic<bool> g_two_hand_constraint_v120_178r29{false}; // V66.178R29: 8 deg nudge (V120) instead of the V130 55 deg two-hand constraint
static std::atomic<float> g_guard_enter_speed178r29{2.5f};static std::atomic<unsigned> g_guard_enter_dwell178r29{0}; // C19C4R1 easy normal guard entry
static std::atomic<bool> g_guard_strict59c17{false}; // Optional stricter entry; held exit remains direction-aware
static std::atomic<float> g_weapon_stat_effect59c17{1.f}; // R59.16C10R17 VrWeaponStatEffect (0 = R16 handling)
static std::atomic<float> g_guard_exit_speed178r45{3.5f};static std::atomic<unsigned> g_guard_after_attack_ms49{400}; // V66.178R49: no guard entry this long after a commit // V66.178R45: a held guard releases above this blade-centre speed (a strike from guard)
static std::atomic<bool> g_thrust_as_swing55{true};        // V66.178R55: a thrust is sent as the ordinary attack from its origin side (INI VrThrustAsSwing)
static std::atomic<std::uint64_t> g_kick_request59{0};      // V66.178R59: R3 kick request tick (armed melee, F4 physical combat)
static std::atomic<bool> g_wall_slide59c10{false};      // H18: tangential scrape is explicit opt-in; checked impacts/outward recovery remain independent
static std::atomic<bool> g_telegraph_windup59c10{true};  // R59.16C10: the backswing arms the held preparation (INI VrTelegraphWindup)
static std::atomic<bool> g_windup_hold57{true};             // V66.178R57: a cocked, resting melee hand is the native prepare (INI VrWindupHold)
static std::atomic<bool> g_directional_stabs178r29{false};   // V66.178R29: V167 directional stabs (origin guard replaces the stab zone) off by default
static std::atomic<bool> g_geometry_auto_lock178r29{true};   // V66.178R29: rung 2 clicks R3 (opponent lock) when a hit lands while Henry is not in combat
static std::atomic<bool> g_impale_enabled178r15{true};   // INI VrImpaleLatch
std::atomic<int> g_census_mode{0}; // V64.50 F10: upper-body VRIK ladder
std::atomic<void*> g_last_player_pose{nullptr};
std::atomic<std::uint64_t> g_correlated_calls{0},g_setik_calls{0},g_rig_maps{0},g_rejects{0};
// V66.157 visible-pose authority diagnostics.  These counters never authorize a
// pose write; they only classify why the coherent VRIK transaction surrendered
// the rendered skeleton to native animation for a frame.
std::atomic<std::uint64_t> g_pose_authority_drop157[6]{}; // rig, raw API, no snapshot, age, native-body, transaction/other
std::atomic<std::uint64_t> g_phys_waiting_native157{0}; // native frame shown only when sole physical owner has no placement yet
std::atomic<std::uint64_t> g_phys_rebase_r98{0}; // R98: in-flight body frames where the held weapon was re-placed at the current world
struct InteractionHandContext158 {int side=-1;bool passive=false,valid=false;float x=0,y=0,z=0,maxReach=0;std::uint64_t tick=0;};
kcd2vr::core::SnapshotMailbox<InteractionHandContext158> g_interaction_hand_context158;
std::atomic<std::uint64_t> g_prebuild_writes{0};
std::atomic<std::uint64_t> g_setik_upstream_writes{0},g_setik_hold_writes{0};
std::atomic<bool> g_rig_valid{false};
std::atomic<std::uint32_t> g_cached_count{0};
std::atomic<int> g_cached_hand_r{-1},g_cached_hand_l{-1},g_cached_head{-1},g_solver_joint_c{-1},g_solver_joint_e{-1};
std::atomic<bool> g_named_joint_api_valid{false};
std::atomic<int> g_named_api_attempts{0};
std::atomic<int> g_named_hand_r{-1},g_named_hand_l{-1},g_weapon_slot_r{-1},g_weapon_slot_l{-1};
std::atomic<int> g_slt0_joint{-1},g_slt1_joint{-1};
std::atomic<void*> g_named_probe_owner{nullptr};
std::atomic<int> g_weapon_slot_parent_r{-2},g_weapon_slot_parent_l{-2};
std::atomic<float> g_b1_l1{0},g_b1_l2{0},g_ref_l1{0},g_ref_l2{0},g_controller_delta_len{0};
std::atomic<float> g_goal_r_x{0},g_goal_r_y{0},g_goal_r_z{0};
std::atomic<float> g_goal_l_x{0},g_goal_l_y{0},g_goal_l_z{0};
std::atomic<float> g_goal_err_r{0},g_goal_err_l{0};
std::atomic<float> g_eff_r_x{0},g_eff_r_y{0},g_eff_r_z{0};
std::atomic<float> g_eff_l_x{0},g_eff_l_y{0},g_eff_l_z{0};
std::atomic<float> g_weapon_r_native_x{0},g_weapon_r_native_y{0},g_weapon_r_native_z{0},g_weapon_r_goal_x{0},g_weapon_r_goal_y{0},g_weapon_r_goal_z{0};
std::atomic<float> g_weapon_l_native_x{0},g_weapon_l_native_y{0},g_weapon_l_native_z{0},g_weapon_l_goal_x{0},g_weapon_l_goal_y{0},g_weapon_l_goal_z{0};
std::atomic<int> g_fail_stage{0};

// V64.11 F11 whole-rig recorder. Fixed storage only: no allocation, no writes into game memory.
constexpr int kJointRecordCap=2048;
std::atomic<bool> g_joint_recording{false};
std::atomic<int> g_f11_theory_phase{0}; // V65.67: 0 off, 1 core pose, 2 held-item observer, 3 native weapon anatomy tracker
std::atomic<int> g_weapon_upstream_diag_mode{0}; // V65.68 F9: read-only HAND->ITEM->C_Item->entity->attachment-owner ladder
std::atomic<std::uint64_t> g_joint_record_samples{0},g_joint_record_start_ms{0},g_joint_record_last_ms{0};
std::atomic<std::uint64_t> g_f11_session_id{0};
std::atomic_flag g_joint_record_lock=ATOMIC_FLAG_INIT;
QuatT28 g_joint_base[kJointRecordCap]{},g_joint_prev[kJointRecordCap]{};
float g_joint_max_pos[kJointRecordCap]{},g_joint_max_rot[kJointRecordCap]{};
int g_joint_parent[kJointRecordCap]{};
std::uint32_t g_joint_record_count=0;
bool g_joint_have_base=false;

// V64.22 focused right-hand subtree evidence. Measure relative geometry, not just
// world-space maxima, so a rigid helper/attachment can be distinguished from a
// normal articulated descendant. Edges: 417<-408, 419<-417, 420<-417, 414<-408, 415<-408.
constexpr int kFocusEdgeCount=5;
constexpr int g_focus_child[kFocusEdgeCount]={417,419,420,414,415};
constexpr int g_focus_parent[kFocusEdgeCount]={408,417,417,408,408};
float g_focus_base_dist[kFocusEdgeCount]{},g_focus_base_ang[kFocusEdgeCount]{};
float g_focus_max_dist_dev[kFocusEdgeCount]{},g_focus_max_ang_dev[kFocusEdgeCount]{};
bool g_focus_baseline_valid=false;

const char* F11ScenarioNameLocal(int phase) noexcept {
    switch(phase){
    case 0:return "OFF";
    case 1:return "DRAW/HOLSTER LIFECYCLE";
    case 2:return "BOW/TWO-HAND HELD ITEM";
    case 3:return "CROSSBOW/FIREARM RELOAD + WEAPON ANATOMY";
    default:return "UNKNOWN";
    }
}

std::atomic<std::uint64_t> g_track_seq{0};
std::atomic<float> g_hp_x{0},g_hp_y{0},g_hp_z{0},g_hq_x{0},g_hq_y{0},g_hq_z{0},g_hq_w{1};
std::atomic<float> g_lp_x{0},g_lp_y{0},g_lp_z{0},g_lq_x{0},g_lq_y{0},g_lq_z{0},g_lq_w{1};
std::atomic<float> g_rp_x{0},g_rp_y{0},g_rp_z{0},g_rq_x{0},g_rq_y{0},g_rq_z{0},g_rq_w{1};
std::atomic<float> g_left_grip{0.0f},g_right_grip{0.0f};
std::atomic<float> g_left_trigger{0.0f},g_right_trigger{0.0f};
std::atomic<int> g_misc_diag_mode{0}; // V66.88 F5: 0 off, 1 event-driven anim+attach+WUID+entity correlation
std::atomic<bool> g_head_valid{false},g_left_valid{false},g_right_valid{false},g_gameplay_allowed{false};
std::atomic<std::uint64_t> g_head_valid_tick{0},g_left_valid_tick{0},g_right_valid_tick{0};
// V66.88: V66.86 visual/free-look pitch compensation RETIRED after headset-proven frame oscillation.
std::atomic<int> g_upper_head_baseline_state{0};
std::atomic<float> g_upper_head_base_qx{0},g_upper_head_base_qy{0},g_upper_head_base_qz{0},g_upper_head_base_qw{1};
std::atomic<std::uint64_t> g_weapon_owner_trace_last_ms{0};
std::atomic<std::uint64_t> g_last_resolved_wuid0{0},g_last_resolved_wuid1{0};

// V64.19C Dynamic Detector-inspired correlation state. Published from the main
// thread using only already-proven native signals; consumed read-only by F11.
std::atomic<bool> g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_menu{false},g_ctx_reading{false};
std::atomic<std::uint64_t> g_pose_policy_generation57{2},g_pose_policy_flags57{0};
// One current flags+tick publication; the activity exception cannot mix old allow with new UI.
std::atomic<std::uint64_t> g_tool_context19c4r2{0};
std::atomic<bool> g_ctx_agency_known{false},g_ctx_scripted{false};
std::atomic<int> g_ctx_agency_count{-1};
std::atomic<std::uint64_t> g_ctx_packed_last_logged{~0ull};

#include "subsystems/vrik/calibration_state.inl" // M2_OWNED_SOURCE
// R133 FREE PLUS: the tracking->model frame turned by the body offset (free_targeting_plus_r133.h), so head,
// hands, weapon and room walking stay where they are in the world while the game turns Henry's root toward his
// opponent. offset 0 (every mode but FREE PLUS, and FREE PLUS outside a held fight) = the calibrated frame exactly.
// the calibration solver/save keep reading the raw g_cal_frame_q*.
struct CalFrameR133T {float x,y,z,w;template<class Q> operator Q() const noexcept {return Q{x,y,z,w};}};
static CalFrameR133T CalFrameR133() noexcept {
    float q[4]{g_cal_frame_qx.load(),g_cal_frame_qy.load(),g_cal_frame_qz.load(),g_cal_frame_qw.load()};
    kcd2vr::free_targeting_r133::yawFrame(q,kcd2vr::free_targeting_r133::offsetBody.load(std::memory_order_acquire));
    return {q[0],q[1],q[2],q[3]};
}
std::atomic<std::uint64_t> g_last_setik_tick{0};
std::atomic<bool> g_player_identity_observe{false};
std::atomic<void*> g_last_player_entity{nullptr};
#include "subsystems/vrik/body_fit_state.inl" // M2_OWNED_SOURCE

struct PendingPoseWrite {
    void* pose=nullptr; PoseGetFn getter=nullptr; PoseSetFn setter=nullptr; int buffer=-1;
    int ids[4]{-1,-1,-1,-1}; QuatT28 original[4]{},injected[4]{}; bool written[4]{}; int count=0;
    std::uint64_t tick=0; bool active=false;
};
PendingPoseWrite g_pending_setik{};
std::atomic_flag g_pending_lock=ATOMIC_FLAG_INIT;

// V65.11 F10 rung 2: keep the already-proven late selector-1 arm + complete
// hand_right subtree alive beyond the hierarchy builder, but only until the
// next exact-player SetIK / next matching skinning frame / mode change.  This
// tests lifetime ownership without inventing a new attachment or palette ABI.
constexpr int kF10HeldJointCap = 128;
struct PendingF10SubtreeEntry {
    int id=-1;
    QuatT28 original{};
    QuatT28 injected{};
    bool written=false;
};
struct PendingF10SubtreeHold {
    void* pose=nullptr;
    PoseGetFn getter=nullptr;
    PoseSetFn setter=nullptr;
    PendingF10SubtreeEntry entries[kF10HeldJointCap]{};
    int count=0;
    std::uint64_t tick=0;
    bool active=false;
};
PendingF10SubtreeHold g_f10_subtree_hold{};
std::atomic_flag g_f10_subtree_hold_lock=ATOMIC_FLAG_INIT;
std::atomic<std::uint64_t> g_f10_subtree_hold_writes{0};
std::atomic<std::uint64_t> g_f10_subtree_hold_restores{0};

std::atomic<std::uint64_t> g_diag_seq{0},g_diag_sample{0};
std::array<std::atomic<int>,8> g_di{{-1,-1,-1,-1,-2,-2,-2,-2}};
std::array<std::atomic<float>,96> g_df{};

// Resolve and pin once, instead of a loader lookup for every guarded field read.
// A failed lookup is NOT cached: the DLL may not have been loaded yet.
static std::atomic<std::uintptr_t> g_wh_pinned104{0};
std::uintptr_t WhBase() noexcept {
    if(const auto base=g_wh_pinned104.load(std::memory_order_acquire))return base;
    HMODULE module=nullptr;
    if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN,L"WHGame.dll",&module)&&module){
        const auto base=reinterpret_cast<std::uintptr_t>(module);
        g_wh_pinned104.store(base,std::memory_order_release);return base;
    }
    return reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
}
bool SupportedWhBuild(std::uintptr_t base) noexcept {
    if(!base) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        return nt->Signature==IMAGE_NT_SIGNATURE && nt->FileHeader.TimeDateStamp==kSupportedWhTimestamp;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
// Per-native-callback page checks, never a cache of item/actor/hand identity.
// Contents and vtables are still reread on every use; Read/Write retain SEH.
struct ReadRegion101R2 {std::uintptr_t first=0,end=0;};
struct ReadCache101R2 {std::array<ReadRegion101R2,16> region{};unsigned next=0,depth=0;std::uint64_t queries=0,hits=0,totalQueries122=0,totalHits122=0,totalQueryNs125=0;};
static thread_local ReadCache101R2 g_read_cache101R2;
static std::atomic<std::uint64_t> g_read_queries101R2{0},g_read_hits101R2{0};
struct ReadScope101R2 {
 ReadScope101R2() noexcept {auto& c=g_read_cache101R2;
    if(c.depth++==0){c.queries=c.hits=0;c.region={};c.next=0;}}
 ~ReadScope101R2(){auto& c=g_read_cache101R2;if(!--c.depth){
    c.region={};c.next=0;
    g_read_queries101R2.fetch_add(c.queries,std::memory_order_relaxed);g_read_hits101R2.fetch_add(c.hits,std::memory_order_relaxed);}}
 ReadScope101R2(const ReadScope101R2&)=delete;ReadScope101R2& operator=(const ReadScope101R2&)=delete;
};
static void InvalidateReadCache101R2() noexcept {g_read_cache101R2.region={};g_read_cache101R2.next=0;}
struct QueryTiming125 {
    const kcd2vr::hookcost178r22::Lease capture=kcd2vr::hookcost178r22::lease();
    const bool enabled=capture.serial!=0;
    std::chrono::steady_clock::time_point start=enabled?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{};
    ~QueryTiming125(){if(enabled&&capture.current())g_read_cache101R2.totalQueryNs125+=std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-start).count());}
};
struct PageMemo169 {struct R{std::uintptr_t first=0,end=0;std::uint64_t stamp=0;};std::array<R,32> region{};unsigned next=0;};
static thread_local PageMemo169 g_page_memo169;
// D30-PERF: O(1) resident-page memo (same 150 ms evidence window); the ring above keeps big VirtualQuery regions.
static thread_local kcd2vr::page_memo_pf::Table g_page_table_pf=[](){kcd2vr::page_memo_pf::Table t{};SYSTEM_INFO i{};GetSystemInfo(&i);if(i.dwPageSize)t.pageSize=i.dwPageSize;return t;}();
// V66.178R7 GUARDED-READ FAULT ATTRIBUTION. Every Read/SafeWrite is SEH-guarded, and the
// V169 page memo deliberately accepts that a page freed inside its 150 ms window "faults into
// the handler and fails closed". That fault is NOT free in this process: a first-chance access
// violation runs the kernel dispatch plus every vectored handler the game and its crash
// reporter (BugSplat64.dll sits next to WHGame.dll) installed before our __except sees it.
// The 2026-10-08 run shows six ~300 ms stalls inside the native weapon/rhythm ticks with all
// measured substages in the microseconds - exactly the signature of a hidden exception path.
// Faults were never counted; now they are, with the stage, address and cost, once per 250 ms.
static std::atomic<std::uint64_t> g_read_faults178r7{0},g_read_fault_log178r7{0};
static thread_local int g_perf_stage178r7=-1;
static std::atomic<unsigned> g_page_memo_ms178r7{150};
static void NoteReadFault178r7(const void* p,std::uint64_t ns) noexcept {
    const auto n=g_read_faults178r7.fetch_add(1,std::memory_order_relaxed)+1;
    const auto now=std::uint64_t(GetTickCount64());auto last=g_read_fault_log178r7.load(std::memory_order_relaxed);
    if(now-last<250||!g_read_fault_log178r7.compare_exchange_strong(last,now,std::memory_order_relaxed))return;
    static const char* stage[]{"POSE_SOLVE","NATIVE_SKINNING","PHYSICAL_PLACEMENT","NATIVE_FRAME","EQUIPMENT_READ","NATIVE_STATE_READBACK","EQUIPMENT_LIVE","EQUIPMENT_METADATA","NATIVE_BEGIN","NATIVE_SPECIAL","NATIVE_FIST","NATIVE_GUARD","NATIVE_WEAPON","NATIVE_RHYTHM","NATIVE_ACTION_PREPARE","NATIVE_ACTION_RELEASE","NATIVE_ACTION_CANCEL","NATIVE_ZONE"};
    const int s=g_perf_stage178r7;char m[224]{};
    std::snprintf(m,sizeof(m),"V66.178R7 GUARDED_READ_FAULT total=%llu stage=%s address=%p handlerMs=%.2f memoMs=%u (SEH fault inside the mod's guarded read; cost includes the game's vectored handlers)",
        (unsigned long long)n,s>=0&&s<18?stage[s]:"NONE",p,double(ns)/1e6,g_page_memo_ms178r7.load(std::memory_order_relaxed));
    diagnostic_trace::Write(m);
}
static void ResetPageMemo169() noexcept {g_page_memo169=PageMemo169{};g_page_table_pf.clear();} // tests / explicit invalidation
static void Memo169(std::uintptr_t first,std::uintptr_t end) noexcept {
    if(!first||end<=first)return;auto& m=g_page_memo169;const auto now=GetTickCount64();
    for(auto& r:m.region)if(r.first==first&&r.end==end){r.stamp=now;return;}
    m.region[m.next]={first,end,now};m.next=(m.next+1)%m.region.size();
}
bool Safe(const void* p,std::size_t need=1) noexcept {
    if(!p)return false;const auto address=reinterpret_cast<std::uintptr_t>(p);auto& c=g_read_cache101R2;
    if(c.depth)for(const auto& r:c.region)if(r.first&&address>=r.first&&address<r.end&&need<=r.end-address){++c.hits;++c.totalHits122;return true;}
    // V66.169 PERFORMANCE: in the V164R4 log the per-callback cache still issued
    // ~2,500 page queries/s at ~60 us each (40-70 ms CPU per second, ~85 % of the VRIK
    // solve). Readable regions are now also remembered for 150 ms across callbacks on
    // this thread. This is safe because every Read/SafeWrite remains SEH-guarded: a page
    // decommitted inside the window faults into the handler and fails closed, exactly as
    // before; identities/vtables are still re-read and re-validated on every use.
    {const auto now=GetTickCount64();
     {std::uintptr_t pf0=0,pf1=0;
      if(g_page_table_pf.lookup(address,need,now,g_page_memo_ms178r7.load(std::memory_order_relaxed),pf0,pf1)){
          ++c.hits;++c.totalHits122;if(c.depth){c.region[c.next]={pf0,pf1};c.next=(c.next+1)%c.region.size();}return true;}}
     for(const auto& r:g_page_memo169.region)if(r.first&&address>=r.first&&address<r.end&&need<=r.end-address&&now>=r.stamp&&now-r.stamp<=g_page_memo_ms178r7.load(std::memory_order_relaxed)){
         ++c.hits;++c.totalHits122;if(c.depth){c.region[c.next]={r.first,r.end};c.next=(c.next+1)%c.region.size();}return true;}}
    // Resident-page permission query is bounded even in a multi-GB committed
    // region. It never grants ownership or survives the callback read scope.
    std::uintptr_t pageFirst=0,pageEnd=0;bool resident=false;
    ++c.totalQueries122;if(c.depth)++c.queries;
    {QueryTiming125 queryTime;resident=kcd2vr::memory126::residentReadable(p,need,pageFirst,pageEnd);}
    if(resident){if(c.depth){c.region[c.next]={pageFirst,pageEnd};c.next=(c.next+1)%c.region.size();}g_page_table_pf.insert(pageFirst,pageEnd,GetTickCount64());return true;}
    MEMORY_BASIC_INFORMATION mbi{};
    std::size_t queried=0;{QueryTiming125 queryTime;queried=VirtualQuery(p,&mbi,sizeof(mbi));}
    if(!queried||mbi.State!=MEM_COMMIT||(mbi.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return false;
    const auto first=reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
    if(mbi.RegionSize>UINTPTR_MAX-first)return false;const auto end=first+mbi.RegionSize;
    if(address<first||address>=end||need>end-address)return false;
    if(c.depth){c.region[c.next]={first,end};c.next=(c.next+1)%c.region.size();}Memo169(first,end);return true;
}
struct PerfCounter101R2 {std::atomic<std::uint64_t> count{0},ns{0},maximum{0},queries122{0},reuse122{0},queryNs125{0};std::atomic<unsigned> modeMask125[4]{};};
// V66.107 globals
static std::atomic<std::uint64_t> g_native_use_until107[2]{},g_native_use_serial107[2]{};
// Accepted native animation provenance only. Attached-tool refreshes retain
// native fingers/contact ownership, but must not acquire the tracked wrist.
static std::atomic<std::uint64_t> g_native_wrist_until107[2]{},g_native_wrist_retired107[2]{};
static std::atomic<std::uint32_t> g_native_use_target59c10[2]{};
// V66.178R14 natural hands: hover affordance per side (fingers open a little), native hand handover while the game runs Henry's interactive action.
static std::atomic<bool> g_hover_affordance178r14[2]{},g_native_handover178r14{true};
static std::atomic<bool> g_eye_floor_valid107{false};static std::atomic<float> g_eye_floor_m107{0.f};
static std::atomic<float> g_standing_eye_floor_m107{0.f}; // from the F8 standing T-pose (persisted)
static std::atomic<bool> g_standing_eye_save107{false};
// C43 calibration restore (vrik/calibration_restore43.h): the calibrated head anchor (Henry's model-local head at the F8
// T-pose, persisted) is kept across re-seats; a re-seat corrects the vertical by the standing eye reference.
static std::atomic<bool> g_cal_auto_restore43{true},g_cal_anchor_saved43{false},g_cal_anchor_save43{false},g_cal_restore_force43{false};
static std::atomic<float> g_cal_anchor_x43{0},g_cal_anchor_y43{0},g_cal_anchor_z43{0};
static std::atomic<std::uint64_t> g_cal_restore_count43{0},g_cal_drift_count43{0};
static std::atomic<bool(*)(float&,float&,float&,std::uint64_t)> g_view_position_probe43{nullptr}; // mod.cpp: CameraHook::GetLatestVisualPositionWorld (receipt only)
static std::atomic<bool(*)(float&,float&,float&,std::uint64_t)> g_view_forward_probe43{nullptr};
static PerfCounter101R2 g_perf101R2[18]; // frame totals plus capture-only native substage attribution (R6: +rhythm, +native action prepare/release/cancel)
static std::atomic<std::uint64_t> g_perf_stall_log178r6{0};
struct PerfScope101R2 {
 int slot;bool enabled=false;kcd2vr::hookcost178r22::Lease timingCaptureR13{};std::chrono::steady_clock::time_point start{};std::uint64_t queries122=0,reuse122=0,queryNs125=0;unsigned modes125[4]{};
 int prevStage178r7=-1;
 kcd2vr::diagnostics::perf_d61::Scope lightMetricsD61;
 explicit PerfScope101R2(int index):slot(index),lightMetricsD61(unsigned(index)){
    prevStage178r7=g_perf_stage178r7;g_perf_stage178r7=index; // fault attribution even with capture off
    timingCaptureR13=kcd2vr::hookcost178r22::lease();enabled=timingCaptureR13.serial!=0;
    if(!enabled)return;
    start=std::chrono::steady_clock::now();queries122=g_read_cache101R2.totalQueries122;reuse122=g_read_cache101R2.totalHits122;queryNs125=g_read_cache101R2.totalQueryNs125;
    modes125[0]=1u<<std::clamp(g_census_mode.load(),0,15);modes125[1]=1u<<std::clamp(g_weapon_control_mode.load(),0,15);
    modes125[2]=1u<<std::clamp(diagnostics::lab::Get(diagnostics::lab::Key::F3),0u,15u);
    modes125[3]=1u<<std::clamp(diagnostics::lab::Get(diagnostics::lab::Key::F5),0u,15u);
 }
 ~PerfScope101R2(){g_perf_stage178r7=prevStage178r7;if(!enabled||!timingCaptureR13.current())return;const auto duration=std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-start).count());auto& p=g_perf101R2[slot];p.count.fetch_add(1,std::memory_order_relaxed);p.ns.fetch_add(duration,std::memory_order_relaxed);auto old=p.maximum.load(std::memory_order_relaxed);while(old<duration&&!p.maximum.compare_exchange_weak(old,duration,std::memory_order_relaxed)){}
    // V66.178R6: the 2026-10-08 run had six ~300 ms stalls inside the frame hook (NATIVE_FRAME
    // max 297-313 ms) that the 5 s summary could not attribute. Name the stage of any single
    // call over 20 ms at once (rate-limited; frame total slot 3 excluded, its parts report).
    if(duration>20000000ull&&slot!=3){const auto now=std::uint64_t(GetTickCount64());auto last=g_perf_stall_log178r6.load(std::memory_order_relaxed);
        if(now-last>=100&&g_perf_stall_log178r6.compare_exchange_strong(last,now,std::memory_order_relaxed)){
            static const char* stall[]{"POSE_SOLVE","NATIVE_SKINNING","PHYSICAL_PLACEMENT","NATIVE_FRAME","EQUIPMENT_READ","NATIVE_STATE_READBACK","EQUIPMENT_LIVE","EQUIPMENT_METADATA","NATIVE_BEGIN","NATIVE_SPECIAL","NATIVE_FIST","NATIVE_GUARD","NATIVE_WEAPON","NATIVE_RHYTHM","NATIVE_ACTION_PREPARE","NATIVE_ACTION_RELEASE","NATIVE_ACTION_CANCEL","NATIVE_ZONE"};
            char m[160]{};std::snprintf(m,sizeof(m),"V66.178R6 FRAME_STALL stage=%s ms=%.1f (single call inside the mod's game-thread hook)",stall[std::clamp(slot,0,17)],double(duration)/1e6);diagnostic_trace::Write(m);}}
    p.queries122.fetch_add(g_read_cache101R2.totalQueries122-queries122,std::memory_order_relaxed);
    p.reuse122.fetch_add(g_read_cache101R2.totalHits122-reuse122,std::memory_order_relaxed);
    p.queryNs125.fetch_add(g_read_cache101R2.totalQueryNs125-queryNs125,std::memory_order_relaxed);
    for(int i=0;i<4;++i)p.modeMask125[i].fetch_or(modes125[i],std::memory_order_relaxed); }
};
// V66.178R30 (R28 review perf 2): the per-read clock existed only to time a fault that almost never happens; a scalar
// read is ~20 ns and QueryPerformanceCounter ~20-30 ns, so it doubled the cost of every one of the ~10k reads per frame.
// The fault is still counted and SEH-guarded; its duration is measured only while a Combat/Interaction recorder runs.
template<class T> bool Read(const void* p,T& out) noexcept {
    static_assert(std::is_trivially_copyable_v<T>);
    if(!Safe(p,sizeof(T))) return false;
    __try { std::memcpy(&out,p,sizeof(T)); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { NoteReadFault178r7(p,0); return false; }
}
template<class T,std::size_t N> bool Read(const void* p,T (&out)[N]) noexcept {
    if(!Safe(p,sizeof(out)))return false;
    __try {std::memcpy(out,p,sizeof(out));return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class T> bool SafeWrite(void* p,const T& in) noexcept {
    static_assert(std::is_trivially_copyable_v<T>);
    if(!Safe(p,sizeof(T))) return false;
    __try { std::memcpy(p,&in,sizeof(T)); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { NoteReadFault178r7(p,0); return false; }
}
void Rtti(void* obj,char* out,std::size_t cap) noexcept {
    if(!out||!cap) return; out[0]=0; if(!obj) return;
    __try {
        const auto vt=*reinterpret_cast<std::uintptr_t*>(obj); if(!vt) return;
        const auto col=*reinterpret_cast<std::uintptr_t*>(vt-sizeof(std::uintptr_t)); if(!col) return;
        const auto base=WhBase(); if(!base) return;
        const auto typeRva=*reinterpret_cast<std::uint32_t*>(col+0x0C); if(!typeRva) return;
        const char* nm=reinterpret_cast<const char*>(base+typeRva+0x10); if(!Safe(nm,1)) return;
        std::size_t i=0; for(;i+1<cap&&Safe(nm+i,1)&&nm[i];++i) out[i]=nm[i]; out[i]=0;
    } __except(EXCEPTION_EXECUTE_HANDLER) { out[0]=0; }
}


bool CopyCString(const char* p,char* out,std::size_t cap) noexcept {
    if(!out||cap==0) return false; out[0]=0; if(!p) return false;
    __try {
        std::size_t i=0;
        for(;i+1<cap;++i){ if(!Safe(p+i,1)) break; const char c=p[i]; out[i]=c; if(!c) return i>0; }
        out[std::min(i,cap-1)]=0; return i>0;
    } __except(EXCEPTION_EXECUTE_HANDLER) { out[0]=0; return false; }
}
static bool IsNativeIkTargetType(const char* p) noexcept {
    char tmp[48]{}; return CopyCString(p,tmp,sizeof(tmp)) && std::strcmp(tmp,"IKTargetProceduralClip")==0;
}
static const char* ArmHandleFromKey(std::uint32_t key) noexcept {
    if(key==kRgtArm01Crc) return "RgtArm01";
    if(key==kLftArm01Crc) return "LftArm01";
    return "other";
}
static bool ArmHandleName(const char* s) noexcept {
    return s && (std::strcmp(s,"RgtArm01")==0 || std::strcmp(s,"LftArm01")==0);
}
static bool RateGate(std::atomic<std::uint64_t>& last,std::uint64_t periodMs) noexcept {
    const auto now=GetTickCount64(); auto old=last.load(std::memory_order_relaxed);
    return now>=old+periodMs && last.compare_exchange_strong(old,now,std::memory_order_acq_rel);
}

void __fastcall IkTargetContextUpdateDetour(void* ctx,float dt) {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::IkContext};
    const auto call=g_iktarget_context_calls.fetch_add(1,std::memory_order_relaxed)+1;
    if((g_mode.load(std::memory_order_acquire)>=5 || g_f11_theory_phase.load(std::memory_order_acquire)==99) && ctx){
        std::uintptr_t vt=0; (void)Read(ctx,vt);
        const auto expected=WhBase()+kIkTargetContextVftRva;
        if(vt==expected){
            const char* ikp=nullptr; const char* jointp=nullptr; void* owner8=nullptr; void* owner10=nullptr;
            int state=0; float blend=0,duration=0,ox=0,oy=0,oz=0;
            (void)Read(static_cast<const std::byte*>(ctx)+0x08,owner8);
            (void)Read(static_cast<const std::byte*>(ctx)+0x10,owner10);
            (void)Read(static_cast<const std::byte*>(ctx)+0x18,blend);
            (void)Read(static_cast<const std::byte*>(ctx)+0x1C,duration);
            (void)Read(static_cast<const std::byte*>(ctx)+0x20,state);
            (void)Read(static_cast<const std::byte*>(ctx)+0x30,ikp);
            (void)Read(static_cast<const std::byte*>(ctx)+0x38,jointp);
            (void)Read(static_cast<const std::byte*>(ctx)+0x40,ox);
            (void)Read(static_cast<const std::byte*>(ctx)+0x44,oy);
            (void)Read(static_cast<const std::byte*>(ctx)+0x48,oz);
            char ikh[48]{},joint[64]{}; CopyCString(ikp,ikh,sizeof(ikh)); CopyCString(jointp,joint,sizeof(joint));
            if((ArmHandleName(ikh)||call<16) && RateGate(g_iktarget_context_last_log,100)){
                char m[640]{}; std::snprintf(m,sizeof(m),
                    "V65.05-IKCTX call=%llu ctx=%p vtMatch=1 owner8=%p owner10=%p state=%d blend=%.4f duration=%.4f dt=%.5f IKHandle=%s JointId=%s Offset=(%.4f,%.4f,%.4f) READ-ONLY",
                    (unsigned long long)call,ctx,owner8,owner10,state,blend,duration,dt,ikh[0]?ikh:"<none>",joint[0]?joint:"<none>",ox,oy,oz);
                diagnostic_trace::Write(m);
            }
        }
    }
    if(g_iktarget_context_original) g_iktarget_context_original(ctx,dt);
}

void __fastcall IkTargetBridgeDetour(void* ctx,void* targetIface,const QuatT28* pose,float weight) {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::IkBridge};
    const auto call=g_iktarget_bridge_calls.fetch_add(1,std::memory_order_relaxed)+1;
    if((g_mode.load(std::memory_order_acquire)>=5 || g_f11_theory_phase.load(std::memory_order_acquire)==99) && ctx){
        const char* ikp=nullptr; const char* jointp=nullptr; (void)Read(static_cast<const std::byte*>(ctx)+0x30,ikp); (void)Read(static_cast<const std::byte*>(ctx)+0x38,jointp);
        char ikh[48]{},joint[64]{}; CopyCString(ikp,ikh,sizeof(ikh)); CopyCString(jointp,joint,sizeof(joint));
        QuatT28 q{}; const bool poseOk=pose&&Read(pose,q);
        if((ArmHandleName(ikh)||call<16) && RateGate(g_iktarget_bridge_last_log,75)){
            char m[700]{}; std::snprintf(m,sizeof(m),
                "V65.05-IKBRIDGE call=%llu ctx=%p targetIface=%p IKHandle=%s JointId=%s weight=%.4f poseOk=%d q=(%.4f,%.4f,%.4f,%.4f) t=(%.4f,%.4f,%.4f) READ-ONLY",
                (unsigned long long)call,ctx,targetIface,ikh[0]?ikh:"<none>",joint[0]?joint:"<none>",weight,poseOk?1:0,
                q.q[0],q.q[1],q.q[2],q.q[3],q.t[0],q.t[1],q.t[2]); diagnostic_trace::Write(m);
        }
    }
    if(g_iktarget_bridge_original) g_iktarget_bridge_original(ctx,targetIface,pose,weight);
}

bool __fastcall IkTargetPoseWriterDetour(void* owner,const char* typeName,const std::int64_t* keyPtr,const QuatT28* pose,bool active,bool flag) {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::IkPoseWriter};
    std::int64_t key64=0; if(keyPtr) (void)Read(keyPtr,key64); const std::uint32_t key32=static_cast<std::uint32_t>(key64);
    const bool relevant=(g_mode.load(std::memory_order_acquire)>=5 || g_f11_theory_phase.load(std::memory_order_acquire)==99) && IsNativeIkTargetType(typeName);
    QuatT28 q{}; const bool poseOk=relevant&&pose&&Read(pose,q);
    const bool ok=g_iktarget_pose_writer_original?g_iktarget_pose_writer_original(owner,typeName,keyPtr,pose,active,flag):false;
    const auto call=g_iktarget_pose_calls.fetch_add(1,std::memory_order_relaxed)+1;
    if(relevant && RateGate(g_iktarget_pose_last_log,75)){
        char m[720]{}; std::snprintf(m,sizeof(m),
            "V65.05-IKPOSE call=%llu owner=%p key64=0x%016llX key32=0x%08X handle=%s poseOk=%d active=%d flag=%d found=%d t=(%.4f,%.4f,%.4f) READ-ONLY",
            (unsigned long long)call,owner,(unsigned long long)key64,(unsigned)key32,ArmHandleFromKey(key32),poseOk?1:0,active?1:0,flag?1:0,ok?1:0,q.t[0],q.t[1],q.t[2]); diagnostic_trace::Write(m);
    }
    return ok;
}

bool __fastcall IkTargetBlendWriterDetour(void* owner,const char* typeName,const std::int64_t* keyPtr,float weight,bool enabled) {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::IkBlendWriter};
    std::int64_t key64=0; if(keyPtr) (void)Read(keyPtr,key64); const std::uint32_t key32=static_cast<std::uint32_t>(key64);
    const bool relevant=(g_mode.load(std::memory_order_acquire)>=5 || g_f11_theory_phase.load(std::memory_order_acquire)==99) && IsNativeIkTargetType(typeName);
    const bool ok=g_iktarget_blend_writer_original?g_iktarget_blend_writer_original(owner,typeName,keyPtr,weight,enabled):false;
    const auto call=g_iktarget_blend_calls.fetch_add(1,std::memory_order_relaxed)+1;
    if(relevant && RateGate(g_iktarget_blend_last_log,75)){
        char m[560]{}; std::snprintf(m,sizeof(m),
            "V65.05-IKBLEND call=%llu owner=%p key64=0x%016llX key32=0x%08X handle=%s weight=%.4f enabled=%d found=%d READ-ONLY",
            (unsigned long long)call,owner,(unsigned long long)key64,(unsigned)key32,ArmHandleFromKey(key32),weight,enabled?1:0,ok?1:0); diagnostic_trace::Write(m);
    }
    return ok;
}

static bool DiscoveryRttiInteresting(const char* rt,int mode) noexcept {
    if(!rt||!*rt) return false;
    if(mode==5) return std::strstr(rt,"Attachment")||std::strstr(rt,"Weapon")||std::strstr(rt,"Character")||std::strstr(rt,"Skeleton");
    if(mode==3) return std::strstr(rt,"Operator")||std::strstr(rt,"PoseModifier")||std::strstr(rt,"Animation")||std::strstr(rt,"Skeleton")||std::strstr(rt,"Character");
    return std::strstr(rt,"Pose")||std::strstr(rt,"Skeleton")||std::strstr(rt,"Character")||std::strstr(rt,"Animation")||std::strstr(rt,"Attachment")||std::strstr(rt,"Weapon");
}

void TraceDiscoveryObject(const char* label,void* root,int mode) noexcept {
    if(!root||mode<=0||mode>5) return;
    char rootRt[128]{}; Rtti(root,rootRt,sizeof(rootRt));
    char hdr[320]{}; std::snprintf(hdr,sizeof(hdr),"V64.3-VRIK-DISC mode=%d(%s) root=%s ptr=%p rtti=%s",mode,ModeName(mode),label?label:"?",root,rootRt[0]?rootRt:"<none>");
    diagnostic_trace::Write(hdr);
    int emitted=0;
    for(std::ptrdiff_t off=0;off<=0x300 && emitted<16;off+=static_cast<std::ptrdiff_t>(sizeof(void*))){
        void* child=nullptr;
        if(!Read(static_cast<unsigned char*>(root)+off,child)||!child||child==root||!Safe(child,sizeof(void*))) continue;
        char rt[128]{}; Rtti(child,rt,sizeof(rt));
        if(!DiscoveryRttiInteresting(rt,mode)) continue;
        char m[384]{}; std::snprintf(m,sizeof(m),"V64.3-VRIK-DISC root=%s+0x%llX child=%p rtti=%s",label?label:"?",(unsigned long long)off,child,rt);
        diagnostic_trace::Write(m); ++emitted;
    }
    if(!emitted) { char m[224]{}; std::snprintf(m,sizeof(m),"V64.3-VRIK-DISC root=%s interestingChildren=0 scan=0x0..0x300",label?label:"?"); diagnostic_trace::Write(m); }
}

void MaybeTraceDiscoveryObject(const char* label,void* root,int mode,int slot) noexcept {
    if(!diagnostics::vrik_h29::selected()||!diagnostics::vrik_h29::active(GetTickCount64()))return;
    static std::atomic<std::uint64_t> last[18]{};
    if(mode<=0||mode>5||slot<0||slot>=3) return;
    const int ix=mode*3+slot; const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto prev=last[ix].load(std::memory_order_relaxed);
    if(now-prev<1500ull || !last[ix].compare_exchange_strong(prev,now,std::memory_order_acq_rel)) return;
    TraceDiscoveryObject(label,root,mode);
}


static bool MannequinRttiInteresting(const char* rt) noexcept {
    if(!rt||!*rt) return false;
    return std::strstr(rt,"Animation")||std::strstr(rt,"Mannequin")||std::strstr(rt,"Action")||
           std::strstr(rt,"Scope")||std::strstr(rt,"Context")||std::strstr(rt,"Controller")||
           std::strstr(rt,"Attachment");
}

static void TraceMannequinCandidates(const char* label,void* root) noexcept {
    if(!root||g_f11_theory_phase.load(std::memory_order_acquire)!=99) return;
    char rootRt[128]{}; Rtti(root,rootRt,sizeof(rootRt));
    char h[384]{};
    std::snprintf(h,sizeof(h),
        "V64.18-F11-MANNEQUIN root=%s ptr=%p rtti=%s scan=0x0..0x600 READ-ONLY NO-VTABLE-CALLS expectations=HoldItem/AimPose/WeaponEquip/combat",
        label?label:"?",root,rootRt[0]?rootRt:"<none>");
    diagnostic_trace::Write(h);
    int emitted=0;
    for(std::ptrdiff_t off=0;off<=0x600&&emitted<32;off+=static_cast<std::ptrdiff_t>(sizeof(void*))){
        void* child=nullptr;
        if(!Read(static_cast<unsigned char*>(root)+off,child)||!child||child==root||!Safe(child,sizeof(void*))) continue;
        char rt[128]{}; Rtti(child,rt,sizeof(rt));
        if(!MannequinRttiInteresting(rt)) continue;
        char m[448]{};
        std::snprintf(m,sizeof(m),
            "V64.18-F11-MANNEQUIN candidate root=%s+0x%llX child=%p rtti=%s",
            label?label:"?",(unsigned long long)off,child,rt);
        diagnostic_trace::Write(m); ++emitted;
    }
    if(!emitted){
        char m[256]{}; std::snprintf(m,sizeof(m),
            "V64.18-F11-MANNEQUIN root=%s candidates=0 (hypothesis unresolved, no guessed offsets)",label?label:"?");
        diagnostic_trace::Write(m);
    }
}

static void MaybeTraceMannequinTheory(void* charPlus8) noexcept {
    if(g_f11_theory_phase.load(std::memory_order_acquire)!=99||!charPlus8) return;
    static std::atomic<std::uint64_t> last{0};
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto prev=last.load(std::memory_order_relaxed);
    if(now-prev<1500ull||!last.compare_exchange_strong(prev,now,std::memory_order_acq_rel)) return;
    TraceMannequinCandidates("Henry.CAttachmentManager",charPlus8);
}

static bool TargetRttiInteresting(const char* rt) noexcept {
    if(!rt||!*rt)return false;
    return std::strstr(rt,"Attachment")||std::strstr(rt,"Weapon")||std::strstr(rt,"Item")||std::strstr(rt,"Character")||
           std::strstr(rt,"Skeleton")||std::strstr(rt,"Animation")||std::strstr(rt,"Operator")||std::strstr(rt,"PoseModifier")||std::strstr(rt,"Entity");
}
static void TraceTargetObject(const char* label,void* root,int mode) noexcept {
    if(!root||mode<=0)return;
    char rootRt[128]{};Rtti(root,rootRt,sizeof(rootRt));
    char h[320]{};std::snprintf(h,sizeof(h),"V64.13-F10-TARGETSCAN mode=%d(%s) root=%s ptr=%p rtti=%s range=0x0..0x600 READ-ONLY",mode,CombatCensusModeName(mode),label?label:"?",root,rootRt[0]?rootRt:"<none>");diagnostic_trace::Write(h);
    int emitted=0;
    for(std::ptrdiff_t off=0;off<=0x600&&emitted<24;off+=static_cast<std::ptrdiff_t>(sizeof(void*))){
        void* child=nullptr;if(!Read(static_cast<unsigned char*>(root)+off,child)||!child||child==root||!Safe(child,sizeof(void*)))continue;
        char rt[128]{};Rtti(child,rt,sizeof(rt));if(!TargetRttiInteresting(rt))continue;
        char m[384]{};std::snprintf(m,sizeof(m),"V64.13-F10-TARGETSCAN root=%s+0x%llX child=%p rtti=%s",label?label:"?",(unsigned long long)off,child,rt);diagnostic_trace::Write(m);++emitted;
    }
    if(!emitted){char m[224]{};std::snprintf(m,sizeof(m),"V64.13-F10-TARGETSCAN root=%s candidates=0",label?label:"?");diagnostic_trace::Write(m);}
}


static void TracePlayerOwnershipChain(void* weaponSolver, int mode) noexcept {
    if(!diagnostics::vrik_h29::selected()||!diagnostics::vrik_h29::active(GetTickCount64()))return;
    if(!weaponSolver||mode<=0)return;
    void* playerModule=nullptr;
    if(!Read(weaponSolver,playerModule)||!playerModule||!Safe(playerModule,sizeof(void*)))return;
    char prt[128]{};Rtti(playerModule,prt,sizeof(prt));
    void* playerEntity=nullptr;
    const bool entityOk=Read(static_cast<unsigned char*>(playerModule)+0x38,playerEntity)&&playerEntity&&Safe(playerEntity,sizeof(void*));
    char ert[128]{};if(entityOk)Rtti(playerEntity,ert,sizeof(ert));
    char m[512]{};std::snprintf(m,sizeof(m),
        "V65.02-OWNER level=%d solver=%p playerModule=%p/%s playerPlus38=%p/%s READ-ONLY",
        mode,weaponSolver,playerModule,prt[0]?prt:"<none>",playerEntity,ert[0]?ert:"<none>");
    diagnostic_trace::Write(m);
    // The runtime WUID value itself is not guessed here. These targeted scans start from the
    // proven player/entity roots behind GetWeaponsFromWeaponSet's known hand_left/hand_right/weapon
    // identifier path and look only for live item/weapon/attachment ownership candidates.
    if(mode>=2)TraceTargetObject("C_Player",playerModule,mode);
    if(mode>=3&&entityOk)TraceTargetObject("C_Player.Entity",playerEntity,mode);
}

static bool ResolveExactPlayerSolverOwner(void* weaponSolver, void*& playerModule, void*& playerEntity) noexcept {
    playerModule=nullptr; playerEntity=nullptr;
    if(!weaponSolver||!Read(weaponSolver,playerModule)||!playerModule||!Safe(playerModule,sizeof(void*))) return false;
    char prt[128]{}; Rtti(playerModule,prt,sizeof(prt));
    if(std::strstr(prt,"C_Player@entitymodule@wh")==nullptr) return false;
    if(!Read(static_cast<unsigned char*>(playerModule)+0x38,playerEntity)||!playerEntity||!Safe(playerEntity,sizeof(void*))) return false;
    char ert[96]{}; Rtti(playerEntity,ert,sizeof(ert));
    return std::strstr(ert,"CEntity")!=nullptr;
}
// V64.3R safety-first carry-forward from the V64.7/V64.9 diagnostics.
// Runtime repeatedly found pose+0x190 -> a real CDefaultSkeleton instance.
// This routine performs READS ONLY: no CDefaultSkeleton virtual method is called.
void MaybeTraceDefaultSkeletonAnchor(void* pose,int mode) noexcept {
    if(!pose||mode<=0) return;
    static std::atomic<std::uint64_t> last{0};
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto prev=last.load(std::memory_order_relaxed);
    if(now-prev<1200ull || !last.compare_exchange_strong(prev,now,std::memory_order_acq_rel)) return;
    void* skel=nullptr; std::uintptr_t vft=0; const auto base=WhBase();
    const bool ptrOk=Read(static_cast<unsigned char*>(pose)+kPoseDefaultSkeletonOffset,skel)&&skel&&Safe(skel,sizeof(void*));
    const bool vftOk=ptrOk&&Read(skel,vft)&&vft!=0;
    char rt[128]{}; if(ptrOk) Rtti(skel,rt,sizeof(rt));
    const std::uintptr_t expected=base?base+kCDefaultSkeletonVftRva:0;
    const bool match=vftOk&&expected&&vft==expected;
    char m[448]{};
    std::snprintf(m,sizeof(m),
        "V64.3R-VRIK-SKELANCHOR pose=%p offset=0x190 ptr=%p vft=%p expected=%p vftMatch=%d rtti=%s READ-ONLY NO-VTABLE-CALLS",
        pose,skel,reinterpret_cast<void*>(vft),reinterpret_cast<void*>(expected),match?1:0,rt[0]?rt:"<none>");
    diagnostic_trace::Write(m);
}
// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations


bool ReadValidatedDefaultSkeletonFromPose(void* pose, void*& out) noexcept {
    out=nullptr;
    if(!pose||!Safe(pose,sizeof(void*))) return false;
    char rt[96]{}; Rtti(pose,rt,sizeof(rt));
    if(std::strstr(rt,"CPoseData")==nullptr) return false;
    void* skel=nullptr;
    if(!Read(static_cast<unsigned char*>(pose)+kPoseDefaultSkeletonOffset,skel)||!skel||!Safe(skel,sizeof(void*))) return false;
    const auto base=WhBase(); std::uintptr_t vt=0;
    if(!SupportedWhBuild(base)||!Read(skel,vt)||vt!=base+kCDefaultSkeletonVftRva) return false;
    out=skel;
    return true;
}

bool ValidateCharInstance(void* ci, void*& skeletonAnim) noexcept {
    skeletonAnim=nullptr;
    if(!ci||!Safe(ci,static_cast<std::size_t>(kSkeletonAnimOffset)+sizeof(void*))) return false;
    const auto base=WhBase();
    if(!SupportedWhBuild(base)) return false;
    std::uintptr_t charVft=0, animVft=0;
    auto* sa=static_cast<unsigned char*>(ci)+kSkeletonAnimOffset;
    if(!Read(ci,charVft)||charVft!=base+kCCharInstanceVftRva) return false;
    if(!Read(sa,animVft)||animVft!=base+kCSkeletonAnimVftRva) return false;
    skeletonAnim=sa;
    return true;
}

bool ReadValidatedCharInstanceFromPlayerPose(void* pose, void*& ci, void*& skeletonAnim) noexcept {
    ci=nullptr; skeletonAnim=nullptr;
    if(!pose||!Safe(pose,sizeof(void*))) return false;
    char rt[96]{}; Rtti(pose,rt,sizeof(rt));
    if(std::strstr(rt,"CPoseData")==nullptr) return false;
    const auto p=reinterpret_cast<std::uintptr_t>(pose);
    if(p<static_cast<std::uintptr_t>(kPoseInCharInstanceOffset)) return false;
    ci=reinterpret_cast<void*>(p-static_cast<std::uintptr_t>(kPoseInCharInstanceOffset));
    if(!ValidateCharInstance(ci,skeletonAnim)){ ci=nullptr; skeletonAnim=nullptr; return false; }
    return true;
}

bool ReadValidatedCharInstanceFromSkinning(void* charPlus8, void*& ci, void*& skeletonAnim) noexcept {
    ci=nullptr; skeletonAnim=nullptr;
    if(!charPlus8) return false;
    const auto p=reinterpret_cast<std::uintptr_t>(charPlus8);
    if(p<8u) return false;
    ci=reinterpret_cast<void*>(p-8u);
    if(!ValidateCharInstance(ci,skeletonAnim)){ ci=nullptr; skeletonAnim=nullptr; return false; }
    return true;
}
bool ReadJointCount(void* owner,std::uint32_t& count) noexcept {
    count=0; if(!owner||!Safe(owner,sizeof(void*))) return false;
    const auto base=WhBase(); std::uintptr_t vt=0;
    if(!SupportedWhBuild(base)||!Read(owner,vt)||vt!=base+kCDefaultSkeletonVftRva) return false;
    // V64.10: this is no longer a guessed virtual slot. WHGame+0x1AAF020 was
    // statically identified as CDefaultSkeleton's joint-count accessor.
    __try { count=reinterpret_cast<JointCountFn>(base+kCDefaultJointCountRva)(owner); }
    __except(EXCEPTION_EXECUTE_HANDLER) { count=0; return false; }
    return count>=8&&count<=2048;
}

// Historical note: the old guessed/vtable name lookup was retired after returning unreliable hand names.
// V65.45 does NOT restore that path; it uses the separately statically-audited exact retail
// CDefaultSkeleton name-hash function at WHGame+0x78BC4C below.
// Retired unused implementation archived: research_archive/retired_pose_implementations

// Safety rule retained: blind CDefaultSkeleton vtable/name calls remain forbidden. V65.45 only
// permits the exact prologue-validated direct retail function below and a tiny fixed name set.

bool ReadParentDirect(void* owner,int joint,int& parent) noexcept;

// V65.45 weapon diagnostic: use the statically audited direct retail name-hash
// function at WHGame+0x78BC4C. This is not a blind vtable sweep. It is invoked only
// for the already-validated Henry CDefaultSkeleton and a small fixed name set.
bool ResolveJointByNameExactV6545(void* owner,const char* name,int& out) noexcept {
    out=-1; if(!owner||!name||!*name) return false;
    const auto base=WhBase(); if(!SupportedWhBuild(base)) return false;
    const unsigned char expected[]{0x4C,0x8B,0xC2,0x4C,0x8B,0xC9,0x83,0xCA,0xFF,0x41,0x8A,0x00};
    const auto* target=reinterpret_cast<const unsigned char*>(base+kCDefaultJointByNameRva);
    if(!Safe(target,sizeof(expected))||std::memcmp(target,expected,sizeof(expected))!=0) return false;
    int id=-1;
    __try { id=reinterpret_cast<JointIdByNameExactFn>(base+kCDefaultJointByNameRva)(owner,name); }
    __except(EXCEPTION_EXECUTE_HANDLER) { id=-1; }
    out=id; return true;
}

static bool ValidateExactWeaponHooksV6547(std::uintptr_t base) noexcept {
    if(!SupportedWhBuild(base)) return false;
    const unsigned char attachOp[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57};
    const unsigned char eventDispatch[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0xFA,0x48,0x8B,0xD9};
    const auto* a=reinterpret_cast<const unsigned char*>(base+kItemAttachOperationRva);
    const auto* e=reinterpret_cast<const unsigned char*>(base+kAttachEventDispatchRva);
    return Safe(a,sizeof(attachOp))&&Safe(e,sizeof(eventDispatch))&&
           std::memcmp(a,attachOp,sizeof(attachOp))==0&&std::memcmp(e,eventDispatch,sizeof(eventDispatch))==0;
}



static bool ValidateWuidResolverV6548(std::uintptr_t base) noexcept {
    if(!SupportedWhBuild(base))return false;
    const unsigned char pro[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0x41,0xB8,0x08,0x00,0x00,0x00};
    const auto* p=reinterpret_cast<const unsigned char*>(base+kWuidResolverRva);
    return Safe(p,sizeof(pro))&&std::memcmp(p,pro,sizeof(pro))==0;
}

static bool ValidateAttachmentHelperTransformV6573(std::uintptr_t base) noexcept {
    if(!SupportedWhBuild(base)) return false;
    const unsigned char pro[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,0x44,0x88,0x48,0x20};
    const auto* p=reinterpret_cast<const unsigned char*>(base+kAttachmentHelperTransformRva);
    return Safe(p,sizeof(pro))&&std::memcmp(p,pro,sizeof(pro))==0;
}

static bool ValidateEntityAttachmentUpdateV6688(std::uintptr_t base) noexcept {
    if(!SupportedWhBuild(base)) return false;
    // Exact retail prologue from supplied WHGame.dll (timestamp 0x6A350E20).
    const unsigned char pro[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x60,0x48,0x8B,0xF9,0x48,0x8B,0xF2,0x48,0x8B,0x0D};
    const auto* p=reinterpret_cast<const unsigned char*>(base+kEntityAttachmentUpdateRva);
    return Safe(p,sizeof(pro))&&std::memcmp(p,pro,sizeof(pro))==0;
}

#include "subsystems/vrik/rig_resolver.inl" // M2_OWNED_SOURCE
void Store3(std::size_t& k,const float v[3]) noexcept { for(int i=0;i<3;++i) g_df[k++].store(v[i],std::memory_order_relaxed); }
void Store8(std::size_t& k,const Dq32& d) noexcept { for(float v:d.a) g_df[k++].store(v,std::memory_order_relaxed); for(float v:d.b) g_df[k++].store(v,std::memory_order_relaxed); }

#include "subsystems/vrik/tracking_reader.inl" // M2_OWNED_SOURCE
// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


#include "subsystems/vrik/calibration_solver.inl" // M2_OWNED_SOURCE

// V65.12 ladder progressively owns more of Henry's validated upper skeleton.
// 1 remains the visually-proven right-hand reference path below.
enum class WriteStrategy : int { None=0, HandOnly=1, TwoBone=2, UpperBody=3, ArmsAndUpperBody=4 };
static WriteStrategy CurrentWriteStrategy(int mode,int census) noexcept {
    (void)census;
    if(mode>0) return WriteStrategy::None; // F9 stays read-only.
    // V65.16 is deliberately a pre-calibration ownership build. Rungs 1..4 are
    // calibration-independent V65.11 recovery/A-B tests; rung 5 is LOCKED until
    // the requested six-pose calibration is implemented rather than reusing the
    // obsolete single-T-pose gate.
    return WriteStrategy::None;
}
static bool StrategyNeedsHands(WriteStrategy s) noexcept { return s==WriteStrategy::HandOnly||s==WriteStrategy::TwoBone||s==WriteStrategy::ArmsAndUpperBody; }
// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations


void PublishDiag(const RigMap& r,int pu,int pf,int pe,int ph,
                 const QuatT28 b0[4],const QuatT28 b1[4],const QuatT28 ref[4],
                 const Dq32& outU,const Dq32& outF,const Dq32& outE,const Dq32& outH) noexcept {
    g_diag_seq.fetch_add(1,std::memory_order_acq_rel);
    g_di[0].store(r.upper_r,std::memory_order_relaxed); g_di[1].store(r.fore_r,std::memory_order_relaxed);
    g_di[2].store(r.elbow_r,std::memory_order_relaxed); g_di[3].store(r.hand_r,std::memory_order_relaxed);
    g_di[4].store(pu,std::memory_order_relaxed);g_di[5].store(pf,std::memory_order_relaxed);g_di[6].store(pe,std::memory_order_relaxed);g_di[7].store(ph,std::memory_order_relaxed);
    std::size_t k=0; for(int j=0;j<4;++j) Store3(k,b0[j].t); for(int j=0;j<4;++j) Store3(k,b1[j].t); for(int j=0;j<4;++j) Store3(k,ref[j].t);
    Store8(k,outU);Store8(k,outF);Store8(k,outE);Store8(k,outH);
    for(int c=0;c<4;++c){
        const bool wxyz=c>=2,conjFirst=(c&1)!=0;
        const V3 u=DecodeDqTranslation(outU,wxyz,conjFirst),f=DecodeDqTranslation(outF,wxyz,conjFirst),h=DecodeDqTranslation(outH,wxyz,conjFirst);
        g_df[k++].store(Len(Sub(f,u)),std::memory_order_relaxed);g_df[k++].store(Len(Sub(h,f)),std::memory_order_relaxed);
    }
    const float b1l1=Len(Sub(FromT(b1[1]),FromT(b1[0]))),b1l2=Len(Sub(FromT(b1[3]),FromT(b1[1])));
    const float refl1=Len(Sub(FromT(ref[1]),FromT(ref[0]))),refl2=Len(Sub(FromT(ref[3]),FromT(ref[1])));
    g_b1_l1.store(b1l1,std::memory_order_relaxed);g_b1_l2.store(b1l2,std::memory_order_relaxed);g_ref_l1.store(refl1,std::memory_order_relaxed);g_ref_l2.store(refl2,std::memory_order_relaxed);
    g_diag_sample.fetch_add(1,std::memory_order_relaxed);
    g_diag_seq.fetch_add(1,std::memory_order_release);
}

struct GripTargetSnapshot {
    bool hr=false,hl=false,wr=false,wl=false;
    QuatT28 hr0{},hr1{},hl0{},hl1{},wr0{},wr1{},wl0{},wl1{};
};
void PublishGripTargets(const QuatT28* hr0,const QuatT28* hr1,const QuatT28* hl0,const QuatT28* hl1,
                        const QuatT28* wr0,const QuatT28* wr1,const QuatT28* wl0,const QuatT28* wl1) noexcept {
    g_grip_seq.fetch_add(1,std::memory_order_acq_rel);
    if(hr0&&hr1){g_cached_goal0_hand_r=*hr0;g_cached_goal1_hand_r=*hr1;g_cached_goal_hand_r_valid.store(true,std::memory_order_relaxed);} else g_cached_goal_hand_r_valid.store(false,std::memory_order_relaxed);
    if(hl0&&hl1){g_cached_goal0_hand_l=*hl0;g_cached_goal1_hand_l=*hl1;g_cached_goal_hand_l_valid.store(true,std::memory_order_relaxed);} else g_cached_goal_hand_l_valid.store(false,std::memory_order_relaxed);
    if(wr0&&wr1){g_cached_goal0_weapon_r=*wr0;g_cached_goal1_weapon_r=*wr1;g_cached_goal_weapon_r_valid.store(true,std::memory_order_relaxed);} else g_cached_goal_weapon_r_valid.store(false,std::memory_order_relaxed);
    if(wl0&&wl1){g_cached_goal0_weapon_l=*wl0;g_cached_goal1_weapon_l=*wl1;g_cached_goal_weapon_l_valid.store(true,std::memory_order_relaxed);} else g_cached_goal_weapon_l_valid.store(false,std::memory_order_relaxed);
    g_grip_seq.fetch_add(1,std::memory_order_release);
}
// Retired unused implementation archived: research_archive/retired_pose_implementations

bool SameQuatT(const QuatT28& a,const QuatT28& b,float eps=1.0e-4f) noexcept {
    for(int i=0;i<4;++i)if(std::fabs(a.q[i]-b.q[i])>eps)return false;
    for(int i=0;i<3;++i)if(std::fabs(a.t[i]-b.t[i])>eps)return false;
    return true;
}
struct UpstreamBatch {
    PoseGetFn getter=nullptr; PoseSetFn setter=nullptr; int buffer=-1;
    int ids[4]{-1,-1,-1,-1}; QuatT28 original[4]{},injected[4]{}; bool written[4]{}; int count=0;
};
struct SubtreeEntry { int id=-1; QuatT28 original{},injected{}; bool written=false; };
struct SubtreeBatch {
    PoseGetFn getter=nullptr; PoseSetFn setter=nullptr;
    std::vector<SubtreeEntry> entries;
};
// Retired unused implementation archived: research_archive/retired_pose_implementations

// Retired unused implementation archived: research_archive/retired_pose_implementations


void RestoreF10SubtreeHoldIfPossible(void* matchingPose=nullptr) noexcept {
    if(g_f10_subtree_hold_lock.test_and_set(std::memory_order_acquire)) return;
    auto& h=g_f10_subtree_hold;
    if(h.active && (!matchingPose || h.pose==matchingPose)) {
        if(h.pose && h.getter && h.setter) {
            for(int i=h.count-1;i>=0;--i) {
                const auto& e=h.entries[i];
                if(!e.written) continue;
                QuatT28 cur{};
                // Native combat/attachment changes win. We restore only records
                // that are still bit-semantically ours within the existing epsilon.
                if(ReadQuat(h.getter,h.pose,e.id,cur) && SameQuatT(cur,e.injected))
                    (void)WriteQuat(h.setter,h.pose,e.id,e.original);
            }
        }
        h=PendingF10SubtreeHold{};
        g_f10_subtree_hold_restores.fetch_add(1,std::memory_order_relaxed);
    }
    g_f10_subtree_hold_lock.clear(std::memory_order_release);
}

// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations

void RestorePendingIfPossible(void* matchingPose=nullptr) noexcept {
    if(g_pending_lock.test_and_set(std::memory_order_acquire)) return;
    if(g_pending_setik.active && (!matchingPose||g_pending_setik.pose==matchingPose)){
        if(g_pending_setik.getter&&g_pending_setik.setter&&g_pending_setik.pose){
            for(int i=g_pending_setik.count-1;i>=0;--i){
                if(!g_pending_setik.written[i])continue; QuatT28 cur{};
                // Restore only a value that is still exactly ours. If native IK or
                // attachment code changed it while the hold was active, that native
                // result wins and is deliberately not clobbered.
                if(ReadQuat(g_pending_setik.getter,g_pending_setik.pose,g_pending_setik.ids[i],cur)&&
                   SameQuatT(cur,g_pending_setik.injected[i]))
                    (void)WriteQuat(g_pending_setik.setter,g_pending_setik.pose,g_pending_setik.ids[i],g_pending_setik.original[i]);
            }
        }
        g_pending_setik=PendingPoseWrite{};
    }
    g_pending_lock.clear(std::memory_order_release);
}
// Retired unused implementation archived: research_archive/retired_pose_implementations

bool PendingHoldActive() noexcept {
    bool yes=false;
    if(g_pending_lock.test_and_set(std::memory_order_acquire)) return false;
    yes=g_pending_setik.active;
    g_pending_lock.clear(std::memory_order_release);
    return yes;
}

// Retired unused implementation archived: research_archive/retired_pose_implementations


// V65.09 F10: one canonical corrected right-arm solve used at every timing rung.
// Only the injection lifetime changes between rungs; geometry/math stays identical.
// Retired unused implementation archived: research_archive/retired_pose_implementations



// Retired unused implementation archived: research_archive/retired_pose_implementations


// V65.10 F10 rung 1: the hand itself is not a sufficient attachment owner.
// Rigidly carry every runtime descendant of hand_right (408) by the exact
// old-hand -> corrected-hand transform. The descendant set is derived from the
// retail parent table each call; no finger/helper ID list is hard-coded.
// Retired unused implementation archived: research_archive/retired_pose_implementations



// V65.25 F10 whole-arm helper ownership test. The latest runtime neighborhood proved
// that 404/406/407 are direct children of UPPERARM_R=403 and 409/410/411 are direct
// children of FOREARM_R=405. The previous 403->405->408 solve left those branches
// under native animation ownership, producing exactly the visible twist/forearm warp.
// This rung rigidly carries those proven helper branches with their corrected parent
// transform, then carries the entire HAND_R=408 descendant subtree. It is still a
// late-pose diagnostic write, not the final native-IK architecture.

#include "subsystems/vrik/hand_anatomy_cache.inl" // M2_OWNED_SOURCE
static float QuatAngleDeg(const QuatT28& a,const QuatT28& b) noexcept {
    const Q4 qa=QFromQuatT(a),qb=QFromQuatT(b);
    const float d=std::clamp(std::fabs(qa.x*qb.x+qa.y*qb.y+qa.z*qb.z+qa.w*qb.w),0.0f,1.0f);
    return 2.0f*std::acos(d)*57.29577951308232f;
}
#include "subsystems/vrik/joint_recorder.inl" // M2_OWNED_SOURCE
#include "subsystems/physics/native_grip_anatomy.inl" // M2_OWNED_SOURCE
struct ItemAttachmentOwnerHit {
    void* object{};
    std::ptrdiff_t offset{-1};
    bool embedded{};
};

// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


// Retired unused implementation archived: research_archive/retired_pose_implementations


void TraceWeaponAttachmentOwnerV6528(void* playerModule,void* playerEntity) noexcept {
    (void)playerEntity;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const auto base=WhBase();void* manager=nullptr;bool valid=false;std::uintptr_t vft=0;
    if(playerModule&&SupportedWhBuild(base)&&Read(static_cast<unsigned char*>(playerModule)+0x9D0,manager)&&manager&&Safe(manager,sizeof(void*))&&Read(manager,vft)) valid=(vft==base+kCItemAttachmentManagerVftRva);
    if(valid) g_henry_item_attach_manager.store(manager,std::memory_order_release);
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now)&&
       !diagnostics::capture168::active(diagnostics::capture168::Domain::Vrik,now))return;
    const bool f11=g_f11_theory_phase.load(std::memory_order_acquire)>0;
    // V65.47: retired flat +0x000..0x300 manager delta scan. Exact attach/event hooks own weapon identification.
    auto prev=g_weapon_owner_trace_last_ms.load(std::memory_order_relaxed);
    if(now-prev<1200ull || !g_weapon_owner_trace_last_ms.compare_exchange_strong(prev,now,std::memory_order_acq_rel)) return;
    static void* lastManager=nullptr;static bool lastValid=false;
    if(manager!=lastManager||valid!=lastValid||f11||g_census_mode.load()==1||g_weapon_control_mode.load()==1){
        char rt[128]{};if(manager&&Safe(manager,sizeof(void*)))Rtti(manager,rt,sizeof(rt));
        char m[640]{};std::snprintf(m,sizeof(m),
            "V65.47-WEAPON-OWNER LIGHT module=%p +0x9D0=%p validItemMgr=%d vft=%p expected=WHGame+0x3EC9958 rtti=%s slt0=%d RightWeaponSlot=%d ITEM_R=%d",
            playerModule,manager,valid?1:0,reinterpret_cast<void*>(vft),rt[0]?rt:"<none>",g_slt0_joint.load(),g_weapon_slot_r.load(),g_cached_hand_r.load()>=0?417:-1);diagnostic_trace::Write(m);
        lastManager=manager;lastValid=valid;
    }
}

// C37: every publication of g_last_player_pose/g_last_player_entity goes through this ledger. Generic SetIK
// CPoseData callbacks (NPCs) are counted as candidates and never published; the C_Player solver callback can only
// refresh what the exact CCryAction prime qualified. Correlation additionally needs a fresh lease, so a stale
// pointer that survives unload/body replacement cannot authorize a reused address. Logs are edge-only.
namespace pid37=kcd2vr::player_identity37;
static pid37::Ledger g_player_ledger37;
static std::atomic_flag g_player_ledger_lock37=ATOMIC_FLAG_INIT;
static std::atomic<std::uint64_t> g_player_publication37{0};
// D44 online: true C37 owner edges, independent of pose/F4 refreshes.
static std::atomic<std::uint64_t> g_player_lifetime_d44{1};
static std::atomic<std::uintptr_t> g_player_char37{0};
// D76: exact local C_Player from the existing identity prime, independent of F12/diagnostic equipment discovery.
static std::atomic<void*> g_player_module_d76{nullptr};
static std::atomic<std::uint64_t> g_player_lease37{0};
static bool PlayerIdentityAccepts37(const void* character,std::uint64_t now) noexcept {
    const auto version=g_player_publication37.load(std::memory_order_acquire);
    if(version&1)return false;
    const auto owner=g_player_char37.load(std::memory_order_acquire),tick=g_player_lease37.load(std::memory_order_acquire);
    return version==g_player_publication37.load(std::memory_order_acquire)&&
        pid37::accepts(owner,tick,reinterpret_cast<std::uintptr_t>(character),now);
}
// Copied presentation owner only: no native calls or stored action-pointer
// dereferences. Attachment/native pumps can run between native Update and the
// next skinning callback, before nativeBodyOwned112 has been mirrored.
static bool NativePassengerCurrentA34(std::uint64_t now) noexcept {
    return kcd2vr::native_passenger_a34::current(g_player_char37.load(std::memory_order_acquire),
        g_pose_epoch6689.load(std::memory_order_acquire),now);
}
// D72: read-only native relationship and exact sole-CAF exclusion. A stale
// publication cannot lend these wrists to a replacement pair or protected action.
static bool ReadInstantPairHandsD72(kcd2vr::paired_hands_d72::Owner& out,std::uintptr_t character,
    std::uint64_t epoch,std::uint64_t tracking,std::uint64_t now) noexcept {
    namespace hands=kcd2vr::paired_hands_d72;out={};hands::Owner r{};
    if(!hands::witness.read(r)||!hands::current(r,character,epoch,tracking,now)||
       diagnostics::lab::NativeReference()||g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||
       g_ctx_reading.load()||g_ctx_scripted.load()||kcd2vr::activity_input163::witness175.Active()||
       g_cal_state.load()!=2||!retail_build112::Ready(WhBase()))return false;
    const auto rd=[](std::uintptr_t p,auto& v) noexcept{return Read(reinterpret_cast<void*>(p),v);};
    kcd2vr::native_pair_d72::Pair live{};
    std::uintptr_t p=0;unsigned token=0,entity=0,layers=0;int layer=-1;
    if(!kcd2vr::native_pair_d72::read(rd,WhBase(),r.pair.attackFragment,r.pair.combo,true,live)||
       !kcd2vr::native_pair_d72::same(live,r.pair)||
       !rd(r.controller+0x30,entity)||entity!=r.entity||!rd(r.controller+0x28,p)||!p||
       !rd(p+0x10,p)||p!=character||!rd(r.pair.attackFragment+0x30,p)||p!=r.scope||
       !rd(r.scope+0x148,p)||p!=r.pair.attackFragment||!rd(r.pair.attackFragment+0x54,token)||token!=r.token||
       !rd(r.scope+0x30,layer)||layer!=r.layer||!rd(r.scope+0x34,layers)||layers!=1||
       !animation_filter::AdmittedNativeClipOnlyD72(reinterpret_cast<void*>(character+0x110),r.clip,r.animation))return false;
    // Armed pairs retain native arm + weapon presentation together. The native
    // body owner intentionally retires physical item placement, so moving only
    // the wrist here would leave a floating native weapon behind. Require both
    // current empty memberships AND observed empty attachment bindings.
    void* manager=g_henry_item_attach_manager.load(std::memory_order_acquire);
    const auto emptyVisuals=[&]() noexcept {
        for(int side=0;side<2;++side)if(g_native_side_item_state[side].load(std::memory_order_acquire)!=0||
            g_native_side_owner[side].load(std::memory_order_acquire)!=manager||
            g_native_side_item[side].load(std::memory_order_acquire)||
            g_native_side_wuid0[side].load(std::memory_order_acquire))return false;
        return true;
    };
    if(!emptyVisuals()||!hands::empty_native_hands(rd,WhBase(),reinterpret_cast<std::uintptr_t>(manager),r.entity)||
       manager!=g_henry_item_attach_manager.load(std::memory_order_acquire)||!emptyVisuals())return false;
    hands::Owner again{};if(!hands::witness.read(again)||!hands::same(r,again)||
       !hands::current(again,character,epoch,tracking,now))return false;
    out=r;return true;
}
static pid37::Verdict ObservePlayerIdentity37(pid37::Source source,bool exactOwner,void* entity,void* character,void* pose,std::uint64_t now,void* playerModuleD76=nullptr) noexcept {
    pid37::Verdict v=pid37::Verdict::Ignored;pid37::Record r{};std::uintptr_t prevChar=0;std::uint64_t cand=0,flips=0,mism=0;bool flipped=false;
    if(g_player_ledger_lock37.test_and_set(std::memory_order_acquire))return pid37::Verdict::Ignored;
    auto& L=g_player_ledger37;prevChar=L.current.valid?L.current.character:0;const auto flipsBefore=L.candidateChanges;
    const auto e=reinterpret_cast<std::uintptr_t>(entity),c=reinterpret_cast<std::uintptr_t>(character);
    if(source==pid37::Source::Prime)v=(e&&c)?L.prime(e,c,now):L.primeAbsent(now);
    else v=L.solver(exactOwner,e,c,now);
    r=L.current;cand=L.rejectedCandidates;flips=L.candidateChanges;mism=L.rejectedMismatches;flipped=flips!=flipsBefore;
    g_player_publication37.fetch_add(1,std::memory_order_acq_rel); // odd: paired publication in progress
    if(v==pid37::Verdict::Qualified||v==pid37::Verdict::Cleared)g_player_lifetime_d44.fetch_add(1,std::memory_order_acq_rel);
    if(v==pid37::Verdict::Refreshed||v==pid37::Verdict::Qualified){
        if(source==pid37::Source::Prime||v==pid37::Verdict::Qualified)g_player_module_d76.store(playerModuleD76,std::memory_order_release);
        g_last_player_pose.store(pose,std::memory_order_release);g_last_player_entity.store(entity,std::memory_order_release);
        g_player_char37.store(r.character,std::memory_order_release);g_player_lease37.store(now,std::memory_order_release);
        if(v==pid37::Verdict::Qualified&&prevChar&&prevChar!=r.character){g_henry_char_attachment_manager.store(nullptr,std::memory_order_release);g_henry_skinning_tick.store(0,std::memory_order_release);}
    } else if(v==pid37::Verdict::Cleared){
        g_player_module_d76.store(nullptr,std::memory_order_release);
        g_player_char37.store(0,std::memory_order_release);g_player_lease37.store(0,std::memory_order_release);
        g_last_player_pose.store(nullptr,std::memory_order_release);g_last_player_entity.store(nullptr,std::memory_order_release);
        g_henry_char_attachment_manager.store(nullptr,std::memory_order_release);g_henry_skinning_thread.store(0,std::memory_order_release);g_henry_skinning_tick.store(0,std::memory_order_release);
    }
    g_player_publication37.fetch_add(1,std::memory_order_release);
    g_player_ledger_lock37.clear(std::memory_order_release);
    if(v==pid37::Verdict::Cleared)animation_filter::ObservePlayerSkinning(nullptr,nullptr,nullptr); // drops the validated Henry CSkeletonAnim
    const bool pow2Mismatch=v==pid37::Verdict::RejectedMismatch&&mism&&!(mism&(mism-1));
    const bool pow2Flip=v==pid37::Verdict::RejectedCandidate&&flipped&&flips&&!(flips&(flips-1));
    if(v==pid37::Verdict::Qualified||v==pid37::Verdict::Cleared||pow2Mismatch){
        char m[448]{};std::snprintf(m,sizeof(m),"C37 PLAYER-OWNER %s gen=%llu source=%s entity=%p char=%p prevChar=%p offeredEntity=%p offeredChar=%p rejectedCandidates=%llu candidateFlips=%llu rejectedMismatches=%llu leaseMs=%llu",
            pid37::name(v),(unsigned long long)r.generation,pid37::name(source),reinterpret_cast<void*>(r.entity),reinterpret_cast<void*>(r.character),reinterpret_cast<void*>(prevChar),entity,character,
            (unsigned long long)cand,(unsigned long long)flips,(unsigned long long)mism,(unsigned long long)pid37::kLeaseMs);diagnostic_trace::Write(m);
    } else if(pow2Flip){
        // Bounded (log2) evidence of generic, unqualified CPoseData SetIK callbacks. Never an owner.
        char m[320]{};std::snprintf(m,sizeof(m),"C37 PLAYER-CHAR-CANDIDATE rejected=NOT_C_PLAYER_SOLVER char=%p pose=%p candidateFlips=%llu rejectedCandidates=%llu qualifiedChar=%p gen=%llu",
            character,pose,(unsigned long long)flips,(unsigned long long)cand,reinterpret_cast<void*>(r.character),(unsigned long long)r.generation);diagnostic_trace::Write(m);
    }
    return v;
}
static void ResetPlayerIdentity37() noexcept {
    while(g_player_ledger_lock37.test_and_set(std::memory_order_acquire)){}
    g_player_publication37.fetch_add(1,std::memory_order_acq_rel);
    g_player_lifetime_d44.fetch_add(1,std::memory_order_acq_rel); // explicit reset retires same-address owners
    g_player_module_d76.store(nullptr,std::memory_order_release);
    g_player_ledger37=pid37::Ledger{};g_player_char37.store(0,std::memory_order_release);g_player_lease37.store(0,std::memory_order_release);
    g_player_publication37.fetch_add(1,std::memory_order_release);
    g_player_ledger_lock37.clear(std::memory_order_release);
}
static bool PrimeLocalPlayerPoseV6583(bool trace) noexcept {
    const auto base=WhBase(); if(!SupportedWhBuild(base)) return false;
    __try {
        void* cryAction=nullptr;if(!Read(reinterpret_cast<void*>(base+kCryActionGlobalRva),cryAction)||!cryAction||!Safe(cryAction,0x90))return false;
        std::uintptr_t vt=0;if(!Read(cryAction,vt)||vt!=base+kCryActionVftRva)return false;
        void* actionGame=nullptr;if(!Read(static_cast<unsigned char*>(cryAction)+0x88,actionGame)||!actionGame||!Safe(actionGame,0xA48))return false;
        if(!Read(actionGame,vt)||vt!=base+kActionGameVftRva)return false;
        void* playerModule=nullptr;if(!Read(static_cast<unsigned char*>(actionGame)+0xA40,playerModule))return false;
        if(!playerModule){(void)ObservePlayerIdentity37(pid37::Source::Prime,true,nullptr,nullptr,nullptr,std::uint64_t(GetTickCount64()));return false;} // C37: validated ActionGame, no local player
        if(!Safe(playerModule,0x9D8))return false;
        if(!Read(playerModule,vt)||vt!=base+kPlayerModuleVftRva)return false;
        void* entity=nullptr;if(!Read(static_cast<unsigned char*>(playerModule)+0x38,entity))return false;
        if(!entity){(void)ObservePlayerIdentity37(pid37::Source::Prime,true,nullptr,nullptr,nullptr,std::uint64_t(GetTickCount64()));return false;}
        if(!Safe(entity,sizeof(void*)))return false;
        if(!Read(entity,vt)||vt!=base+kCEntityVftRva)return false;
        void* fn=nullptr;if(!Read(reinterpret_cast<unsigned char*>(vt)+0x328,fn)||fn!=reinterpret_cast<void*>(base+kCEntityGetCharacterRva))return false;
        using GetCharacterFn=void*(__fastcall*)(void*,int);
        void* ci=reinterpret_cast<GetCharacterFn>(fn)(entity,0);void* anim=nullptr;if(!ValidateCharInstance(ci,anim))return false;
        void* pose=static_cast<unsigned char*>(ci)+kPoseInCharInstanceOffset;void* ci2=nullptr;void* anim2=nullptr;
        if(!ReadValidatedCharInstanceFromPlayerPose(pose,ci2,anim2)||ci2!=ci)return false;
        (void)ObservePlayerIdentity37(pid37::Source::Prime,true,entity,ci,pose,std::uint64_t(GetTickCount64()),playerModule);
        if(trace)TraceWeaponAttachmentOwnerV6528(playerModule,entity);
        static std::atomic<bool> logged{false};bool expected=false;if(logged.compare_exchange_strong(expected,true))diagnostic_trace::Write("V65.83-VRIK PLAYER-PRIME READY source=CCryAction->ActionGame->PlayerModule->CEntity->GetCharacter; combat/punch bootstrap not required");
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER){ return false; }
}

static void TraceConcreteItemObjectV6547(const char* source,void* owner,void* item,int side) noexcept {
    if(g_f11_theory_phase.load(std::memory_order_acquire)<=0 && int(diagnostics::lab::Get(diagnostics::lab::Key::F5))<1) return;
    char ort[128]{},irt[128]{}; if(owner&&Safe(owner,sizeof(void*)))Rtti(owner,ort,sizeof(ort)); if(item&&Safe(item,sizeof(void*)))Rtti(item,irt,sizeof(irt));
    std::uintptr_t q0=0,q28=0,q30=0,q38=0; if(item&&Safe(item,0x40)){(void)Read((unsigned char*)item+0x0,q0);(void)Read((unsigned char*)item+0x28,q28);(void)Read((unsigned char*)item+0x30,q30);(void)Read((unsigned char*)item+0x38,q38);}
    char m[896]{}; std::snprintf(m,sizeof(m),
        "V65.56-%s-CONCRETE-ITEM source=%s owner=%p/%s item=%p/%s side=%d itemFields={+0:%p +28:%p +30:%p +38:%p} calls=%llu",
        (g_f11_theory_phase.load(std::memory_order_acquire)>0?"F11":"F5-MISC"),source?source:"?",owner,ort[0]?ort:"-",item,irt[0]?irt:"-",side,reinterpret_cast<void*>(q0),reinterpret_cast<void*>(q28),reinterpret_cast<void*>(q30),reinterpret_cast<void*>(q38),
        (unsigned long long)g_item_attach_calls.load(std::memory_order_relaxed)); diagnostic_trace::Write(m);
}

#include "vrik/weapon_tracking.inl"

// H36R1: observed real Henry LEFT membership transitions, not input requests.
// This reuses the already installed exact8D00C4 hook; no new native hook/write.
static std::atomic<std::uint64_t> g_left_native_membership_h36{0};
static bool ObserveLeftNativeDrawH36(void* owner,void* item,int side,bool nativeOk,
                                     bool beforeKnown,std::uint64_t beforeKey) noexcept {
    if(!nativeOk||!beforeKnown||side!=1||!owner||owner!=g_henry_item_attach_manager.load(std::memory_order_acquire)||
       !item||!retail_build112::Ready(WhBase()))return false;
    std::uintptr_t vf=0;std::uint64_t key=0,current=0;
    if(!Read(item,vf)||vf!=WhBase()+0x3A2DA88||!Read(static_cast<unsigned char*>(item)+0x30,key)||!key||
       beforeKey==key||!NativeHandKey6697(owner,1,current)||current!=key)return false;
    g_left_native_membership_h36.fetch_add(1,std::memory_order_acq_rel);return true;
}
static bool TryStagedPickup596(void*,std::uint32_t) noexcept;
static int InstantPickupRequestD72(void*,std::uint32_t) noexcept;
static bool __fastcall DeferredPickup596(void*,std::uint32_t) noexcept;
static bool ManualAction596(std::uintptr_t) noexcept;
static bool ManualTransfer596(void*) noexcept;
static void __fastcall DeferredReload596(void*,void*) noexcept;
#include "diagnostics/interaction_lifecycle165.inl"
#include "diagnostics/reload_lifecycle178r36c2.inl"

// C10R19 (review of BC2): native minigame controls keep gameplay authority OFF, so the BC1 alchemy tool route could never read its
// body. Only an admitted workstation body (WorkstationToolPose18, alchemy/smithing tool placement) may read it then; every other
// consumer still requires gameplay authority.
static const char* WorkstationContextRefusal19c4r2(std::uint64_t,bool) noexcept;
static std::uint64_t ReadCompletedPublicationT23() noexcept;
static bool ReadCommittedPacketFor6695(CommittedFrame6695& p,bool workstationBody,
    const kcd2vr::grasp13::Acquisition54* picking55=nullptr,
    const kcd2vr::grasp13::FinishingPresentationR94* finishingR94=nullptr) noexcept {
    // D50: the previous successful validity flag can remain true while the next
    // body/frame pair is being written. Every body reader must join the same
    // completed serial, including tool/hand readers without a paired-frame read.
    const auto publicationD50=ReadCompletedPublicationT23();
    if(!publicationD50||(publicationD50&1))return false;
    const auto now=std::uint64_t(GetTickCount64());
    const auto actor=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire));
    const auto epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    // C55: an exact installed Picking presentation is already allowed to
    // produce this pose before Attach. It need not borrow the later held-prop
    // or workstation flag. This typed exception grants only a copied pose read.
    const bool picking=picking55&&picking55->active&&
        kcd2vr::grasp13::freshAcquisition54(*picking55,now)&&picking55->character==actor&&picking55->epoch==epoch&&
        picking55->trackingEpoch55&&picking55->trackingEpoch55==tracking59c6::epoch.load(std::memory_order_acquire)&&
        actor+8==reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load(std::memory_order_acquire))&&
        !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_reading.load()&&
        !WorkstationContextRefusal19c4r2(now,true)&&
        !animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175();
    if(picking55&&!picking)return false;
    const auto finishingCurrentR94=[&]() noexcept {
        return finishingR94&&!picking55&&finishingR94->binding.character==actor&&finishingR94->binding.epoch==epoch&&
            finishingR94->trackingEpoch==tracking59c6::epoch.load(std::memory_order_acquire)&&
            actor+8==reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load(std::memory_order_acquire))&&
            !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_reading.load()&&
            !WorkstationContextRefusal19c4r2(std::uint64_t(GetTickCount64()),true)&&
            !animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175()&&
            action_time_owner59c11::FinishingOrHeldPresentationCurrentR94(*finishingR94);
    };
    const bool finishing=finishingCurrentR94();
    if(finishingR94&&!finishing)return false;
    if(!g_committed_pose_valid6689.load(std::memory_order_acquire)||g_pose_restore_fault6689.load()||
       !(g_gameplay_allowed.load()||(workstationBody&&g_dialogue_body178r9.load(std::memory_order_acquire))||picking||finishing)||
       !TrackingHeadUsable111()||!g_committed_frame6695.read(p))return false;
    return kcd2vr::pose_core::fresh(p.stamp,epoch,actor,now)&&
        (!picking||(p.stamp.actor==picking55->character&&p.stamp.epoch==picking55->epoch))&&
        (!finishing||finishingCurrentR94())&&p.publicationR24==publicationD50&&
        actor==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire))&&
        epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
        g_committed_pose_valid6689.load(std::memory_order_acquire)&&!g_pose_restore_fault6689.load(std::memory_order_acquire)&&
        publicationD50==ReadCompletedPublicationT23();
}
static bool ReadCommittedPacket6695(CommittedFrame6695& p) noexcept {return ReadCommittedPacketFor6695(p,false);}
static bool ReadF12HandDeltaV6557(int side,QuatT28& nativeHand,QuatT28& vrHand) noexcept {
    CommittedFrame6695 p{};if(side<0||side>1||!ReadCommittedPacket6695(p)||!p.valid[side]||
       !TrackingHandUsable111(side))return false;
    nativeHand=p.native[side];vrHand=p.allowed[side];return true;
}
static bool ReadCommittedHandFrame6689(QuatT28& nativeHand,QuatT28& vrHand,float (&world)[8],
                                      kcd2vr::pose_core::Stamp* stamp=nullptr) noexcept {
    CommittedFrame6695 p{};if(!ReadCommittedPacket6695(p)||!p.valid[0]||!TrackingHandUsable111(0))return false;
    nativeHand=p.native[0];vrHand=p.allowed[0];std::memcpy(world,p.world,sizeof(world));if(stamp)*stamp=p.stamp;return true;
}

static bool ReadCStringEqV6573(const char* s,const char* wanted) noexcept {
    if(!s||!wanted) return false;
    bool eq=false;
    __try { eq=std::strcmp(s,wanted)==0; } __except(EXCEPTION_EXECUTE_HANDLER) { eq=false; }
    return eq;
}

#include "subsystems/ranged_reload/native_nock_observer.inl" // M2_OWNED_SOURCE
static bool ResolveWeaponControlTargetV6553(int side) noexcept;
static thread_local std::uint32_t g_dispatch_hash6690=0;
static thread_local std::uintptr_t g_dispatch_self6690=0;
static std::atomic<std::uint64_t> g_holster_event_sequence6690{0};
// No RAII locals in this SEH helper: MSVC /EHsc does not run an outer C++
// scope destructor for a native structured exception handled outside it.
// Restore only this stack-local TLS candidate; preserve the original exception.
static bool InvokeItemAttachOperationD78(kcd2vr::weapon_draw_d75::NativeAttachScope& scope) {
    bool ok=false;
    __try {
        ok=g_item_attach_operation_original?g_item_attach_operation_original(scope.owner,scope.item,scope.side):false;
        if(ok)WeaponDrawAfterNativeAttachD75(scope);
    } __finally {scope.leave();}
    return ok;
}

bool __fastcall ItemAttachOperationDetour(void* owner,void* item,int side) {
    // V66.87: capture the REAL native caller of the exact item operation. Once the
    // owner is Henry's manager and side==0, this callsite is the cleanest runtime
    // breadcrumb we have for statically following AttachRight downstream without
    // reviving generic entity/helper guesses.
    const auto callRet=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const auto callBase=WhBase();
    const auto callerRva=(callBase&&callRet>=callBase)?(callRet-callBase):0ull;
    const bool recording=diagnostics::lab::Get(diagnostics::lab::Key::F3)>0;
    const int beforeState=(side>=0&&side<2)?g_native_side_item_state[side].load():-1;
    const std::uint64_t beforeKey=(side>=0&&side<2)?g_native_side_wuid0[side].load():0;
    std::uint64_t beforeLeftH36=0;const bool beforeLeftKnownH36=owner&&owner==g_henry_item_attach_manager.load(std::memory_order_acquire)&&NativeHandKey6697(owner,1,beforeLeftH36);
    const auto attachObservation36=LifecycleAttachBegin36(owner,item,side,g_dispatch_hash6690);
    const bool nativeOk=[&]() {
        kcd2vr::weapon_draw_d75::NativeAttachScope drawScopeD75(owner,item,side);
        return InvokeItemAttachOperationD78(drawScopeD75);
    }();
    // Native side 2 means choose this item's class-defined hand; it is not a
    // third hand. Cache only after the native operation and live-key agreement.
    if(nativeOk&&item&&side==2){
        void* def=nullptr;void* cls=nullptr;std::uintptr_t v=0;unsigned char right=2;
        std::uint64_t itemKey=0,liveKey=0;
        // R134: an S_ItemClassAlias definition (vtable 0x3BAEBF0, quest/tutorial weapons) forwards to its source at +0x68.
        if(Read(static_cast<unsigned char*>(item)+0x48,def)&&def&&Read(def,v)&&v==WhBase()+0x3BAEBF0ull&&
           (!Read(static_cast<unsigned char*>(def)+0x68,def)||!def||!Read(def,v)))def=nullptr;
        if(def&&Read(def,v)&&
           (v==WhBase()+0x3A4E390ull||v==WhBase()+0x3A4D890ull)&&
           Read(static_cast<unsigned char*>(def)+0xE8,cls)&&cls&&Read(static_cast<unsigned char*>(cls)+0x25,right)&&right<=1&&
           Read(static_cast<unsigned char*>(item)+0x30,itemKey)&&NativeHandKey6697(owner,right?0:1,liveKey)&&itemKey==liveKey)side=right?0:1;
    }
    (void)ObserveLeftNativeDrawH36(owner,item,side,nativeOk,beforeLeftKnownH36,beforeLeftH36);
    LifecycleAttach165(attachObservation36,side,nativeOk);
    g_item_attach_calls.fetch_add(1,std::memory_order_relaxed);
    const auto henryMgr=g_henry_item_attach_manager.load(std::memory_order_acquire);
    const bool ownerMatch=nativeOk&&owner&&henryMgr&&owner==henryMgr;
    if(ownerMatch && side>=0 && side<2 && !item){
        // Exact native Henry-manager success with a null item is the only fail-closed EMPTY proof used by F4.
        g_weapon_control_generation[side].fetch_add(1,std::memory_order_acq_rel);
        g_native_side_item[side].store(nullptr,std::memory_order_release);
        g_native_side_owner[side].store(owner,std::memory_order_release);
        g_native_side_wuid0[side].store(0,std::memory_order_release);
        g_native_side_wuid1[side].store(0,std::memory_order_release);
        g_native_side_item_tick[side].store((std::uint64_t)GetTickCount64(),std::memory_order_release);
        g_native_side_item_state[side].store(0,std::memory_order_release);
        g_weapon_control_wuid0[side].store(0,std::memory_order_release);
        g_weapon_control_wuid1[side].store(0,std::memory_order_release);
        g_weapon_control_entity[side].store(nullptr,std::memory_order_release);
        g_weapon_control_entity_id[side].store(0,std::memory_order_release);
    }
    if(nativeOk&&item&&Safe(item,sizeof(void*))){
        g_last_concrete_item_object.store(item,std::memory_order_release);g_last_item_attach_owner.store(owner,std::memory_order_release);g_last_item_attach_side.store(side,std::memory_order_release);
        if(Safe((unsigned char*)item+0x30,16)){
            std::uint64_t w0=0,w1=0;(void)Read((unsigned char*)item+0x30,w0);(void)Read((unsigned char*)item+0x38,w1);
            if(w0){
                const auto slot=g_recent_item_wuid_cursor.fetch_add(1,std::memory_order_relaxed)%kRecentItemWuidSlots;g_recent_item_wuid0[slot].store(w0,std::memory_order_release);g_recent_item_wuid1[slot].store(w1,std::memory_order_release);g_recent_item_wuid_tick[slot].store((std::uint64_t)GetTickCount64(),std::memory_order_release);
                const int f12Mode=g_weapon_control_mode.load(std::memory_order_acquire);
                if(f12Mode>0&&ownerMatch){char cm[768]{};std::snprintf(cm,sizeof(cm),"V66.88-F12-C_ITEM rung=%d nativeOk=%d owner=%p henryMgr=%p ownerMatch=%d side=%d item=%p wuidKey=%016llX meta=%016llX publish=%d callerRva=0x%llX",f12Mode,nativeOk?1:0,owner,henryMgr,ownerMatch?1:0,side,item,(unsigned long long)w0,(unsigned long long)w1,(ownerMatch&&side>=0&&side<2)?1:0,(unsigned long long)callerRva);diagnostic_trace::Write(cm);}
                if(ownerMatch&&side>=0&&side<2){
                    const auto exactNow=(std::uint64_t)GetTickCount64();
                    g_native_side_item[side].store(item,std::memory_order_release);g_native_side_owner[side].store(owner,std::memory_order_release);g_native_side_wuid0[side].store(w0,std::memory_order_release);g_native_side_wuid1[side].store(w1,std::memory_order_release);g_native_side_item_tick[side].store(exactNow,std::memory_order_release);
                    g_native_side_item_state[side].store(1,std::memory_order_release);
                    const auto evTick=g_last_attach_event_tick.load(std::memory_order_acquire);
                    if(evTick&&exactNow>=evTick&&exactNow-evTick<=25ull){
                        g_exact_attach_event_self[side].store(g_last_attach_event_self.load(std::memory_order_acquire),std::memory_order_release);
                        g_exact_attach_event_obj[side].store(g_last_attach_event_obj.load(std::memory_order_acquire),std::memory_order_release);
                        g_exact_attach_event_hash[side].store(g_last_attach_event_hash.load(std::memory_order_acquire),std::memory_order_release);
                        char em[640]{};char srt[128]{};void* es=g_exact_attach_event_self[side].load();if(es&&Safe(es,sizeof(void*)))Rtti(es,srt,sizeof(srt));std::snprintf(em,sizeof(em),"V66.87-F12-EXACT-EVENT-JOIN side=%d item=%p wuid={%016llX %016llX} eventSelf=%p/%s event=%p hash=%08X dtMs=%llu writes=NONE",side,item,(unsigned long long)w0,(unsigned long long)w1,es,srt[0]?srt:"-",g_exact_attach_event_obj[side].load(),g_exact_attach_event_hash[side].load(),(unsigned long long)(exactNow-evTick));diagnostic_trace::Write(em);
                    }
                    const auto prevW0=g_weapon_control_wuid0[side].exchange(w0,std::memory_order_acq_rel);
                    g_weapon_control_wuid1[side].store(w1,std::memory_order_release);
                    if(prevW0!=w0){
                        g_weapon_control_entity[side].store(nullptr,std::memory_order_release);
                        g_weapon_control_entity_id[side].store(0,std::memory_order_release);
                        g_weapon_control_generation[side].fetch_add(1,std::memory_order_acq_rel);
                    }
                }
            }
        }
    }
    if(g_f11_theory_phase.load(std::memory_order_acquire)>0 || int(diagnostics::lab::Get(diagnostics::lab::Key::F5))>=1){
        const auto f11Phase=g_f11_theory_phase.load(std::memory_order_acquire);
        if(f11Phase>0){
            static std::atomic<std::uint64_t> lastLog{0}; const auto now=(std::uint64_t)GetTickCount64(); auto prev=lastLog.load(std::memory_order_relaxed);
            const std::uint64_t minInterval=(f11Phase==3)?250ull:80ull; if(now-prev>=minInterval&&lastLog.compare_exchange_strong(prev,now,std::memory_order_acq_rel)) TraceConcreteItemObjectV6547("WHGame+8D00C4",owner,item,side);
        } else {
            static std::atomic<std::uint64_t> lastF5Sig{0}; std::uint64_t w0=0,w1=0;if(item&&Safe((unsigned char*)item+0x40,1)){(void)Read((unsigned char*)item+0x30,w0);(void)Read((unsigned char*)item+0x38,w1);}
            const std::uint64_t sig=(std::uint64_t)(std::uintptr_t)item ^ ((std::uint64_t)(std::uintptr_t)owner<<7) ^ ((std::uint64_t)(std::uint32_t)side<<56) ^ w0 ^ (w1<<1);
            const auto prev=lastF5Sig.exchange(sig,std::memory_order_acq_rel); if(ownerMatch&&sig!=prev) TraceConcreteItemObjectV6547("WHGame+8D00C4",owner,item,side);
        }
    }
    if(ownerMatch&&item&&side>=0&&side<2) (void)ResolveWeaponControlTargetV6553(side);
    if(recording&&owner&&owner==henryMgr){
        std::uint64_t key=0;bool keyRead=item&&Read(static_cast<unsigned char*>(item)+0x30,key);
        const auto stamp=animation_filter::DiagnosticPlaybackStamp();
        const auto seq=g_holster_event_sequence6690.fetch_add(1)+1;
        char m[900]{};std::snprintf(m,sizeof(m),
            "V66.90-F3-EQUIPMENT tick=%llu epoch=%llu eventSeq=%llu nativeReturned=%d exactHenryOwner=1 side=%d item=%p keyRead=%d key=%016llX beforeState=%d beforeKey=%016llX afterState=%d callerRva=0x%llX dispatchHash=0x%08X dispatchSelf=%p sameCallStack=%d playbackSeq=%llu playbackTick=%llu visibleHolsterPose=UNKNOWN drawAccepted=UNCLAIMED",
            (unsigned long long)GetTickCount64(),(unsigned long long)(diagnostics::lab::Snapshot()>>4),(unsigned long long)seq,nativeOk,side,item,keyRead,(unsigned long long)key,beforeState,(unsigned long long)beforeKey,
            (side>=0&&side<2)?g_native_side_item_state[side].load():-1,(unsigned long long)callerRva,g_dispatch_hash6690,reinterpret_cast<void*>(g_dispatch_self6690),g_dispatch_self6690!=0,(unsigned long long)stamp.serial,(unsigned long long)stamp.tick);
        diagnostic_trace::Write(m);
    }
    return nativeOk;
}


// V66.79: the WUID resolver returns a game item object (C_AIItem / C_LinkableObject / ...),
// NOT a CEntity. V65.53 blindly used [obj+0x18] and called vtable+0x08 on it; the result then
// failed the exact CEntity gate forever ("NO_WORLD_OR_LOCAL_TM"). Find the real CEntity instead:
// obj itself, then the first qword members of obj whose object has the exact CEntity vtable
// (single-inheritance class, RTTI .?AVCEntity@@, vtable WHGame+0x4072C78, GetId = [this+0xC]).

void* __fastcall WuidResolverDetour(void* registry,const void* wuid) {
    // V66.94: retain native behavior, not the failed nearby-CEntity scan. The
    // exact held C_Item is resolved by its own linked ID at the placement boundary.
    if(registry)g_weapon_control_registry.store(registry,std::memory_order_release);
    void* obj=g_wuid_resolver_original?g_wuid_resolver_original(registry,wuid):nullptr;
    g_wuid_resolve_calls.fetch_add(1,std::memory_order_relaxed);
    return obj;
}

void __fastcall AttachEventDispatchDetour(void* self,void* eventObj) {
    g_attach_event_calls.fetch_add(1,std::memory_order_relaxed);
    const auto reloadNow136=(std::uint64_t)GetTickCount64();int reloadWeapon136=0;
    const bool reloadObserve136=ReloadDiagnostics175()&&
        animation_filter::ReloadDiagnosticWindow136R3(reloadNow136,reloadWeapon136);
    if((g_f11_theory_phase.load(std::memory_order_acquire)>0||g_weapon_control_mode.load(std::memory_order_acquire)==1||
        diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,reloadNow136)||reloadObserve136)&&eventObj&&Safe(eventObj,0x30)){
        std::uint32_t hash=0; (void)Read((unsigned char*)eventObj+0x1c,hash);
        const auto base=WhBase(); std::uint32_t hA=0,hB=0,hC=0,hD=0;
        if(base){(void)Read((void*)(base+0x548A788ull),hA);(void)Read((void*)(base+0x548A7A8ull),hB);(void)Read((void*)(base+0x548A7ACull),hC);(void)Read((void*)(base+0x548A7A4ull),hD);}
        const char* tag=hash==hA?"ATTACH-EVENT-A":hash==hB?"ATTACH-EVENT-B":hash==hC?"ATTACH-EVENT-C":hash==hD?"ATTACH-EVENT-D":nullptr;
        if(tag){
            g_last_attach_event_self.store(self,std::memory_order_release);g_last_attach_event_obj.store(eventObj,std::memory_order_release);g_last_attach_event_hash.store(hash,std::memory_order_release);g_last_attach_event_tick.store((std::uint64_t)GetTickCount64(),std::memory_order_release);
            std::uintptr_t q0=0,q8=0,q10=0;(void)Read((unsigned char*)eventObj+0x0,q0);(void)Read((unsigned char*)eventObj+0x8,q8);(void)Read((unsigned char*)eventObj+0x10,q10);
            const bool f12=g_weapon_control_mode.load(std::memory_order_acquire)>0, f11=g_f11_theory_phase.load(std::memory_order_acquire)>0;
            static std::atomic<std::uint64_t> lastF5EventSig{0}; const std::uint64_t evSig=(std::uint64_t)(std::uintptr_t)self ^ ((std::uint64_t)hash<<32) ^ (std::uint64_t)(std::uintptr_t)eventObj;
            const bool reloadEmit=reloadObserve136&&g_reload_attach_event_logs136r3.fetch_add(1,std::memory_order_relaxed)<100;
            const bool emit=f12||f11||reloadEmit||(lastF5EventSig.exchange(evSig,std::memory_order_acq_rel)!=evSig);
            if(emit){char rt[128]{},srt[128]{};Rtti(eventObj,rt,sizeof(rt));if(self&&Safe(self,sizeof(void*)))Rtti(self,srt,sizeof(srt)); char m[1120]{};std::snprintf(m,sizeof(m),
                "V66.88-%s-ATTACH-EVENT tag=%s self=%p/%s event=%p/%s hash=0x%08X knownHashes={A:%08X B:%08X C:%08X D:%08X} fields={+0:%p +8:%p +10:%p} eventCalls=%llu reloadWeapon=%s semantics=UNCLASSIFIED",
                reloadEmit?"F11-RELOAD":(f12?"F12":(f11?"F11":"F5-MISC")),tag,self,srt[0]?srt:"-",eventObj,rt[0]?rt:"-",hash,hA,hB,hC,hD,reinterpret_cast<void*>(q0),reinterpret_cast<void*>(q8),reinterpret_cast<void*>(q10),(unsigned long long)g_attach_event_calls.load(),
                reloadEmit?animation_filter::ReloadDiagnosticWeaponName136R3(reloadWeapon136):"-");diagnostic_trace::Write(m);}}
    }
    {   // F5 discovery: distinct dispatch hashes during a crafting lifetime.
        // The observed self and printable fields are raw evidence, not a proven
        // Henry attribution, semantic event name or completed tool operation.
        using namespace kcd2vr::activity_input163;
        const auto craftNow=(std::uint64_t)GetTickCount64();
        const bool smith=witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire)!=0;
        const bool alch=witness175.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire)!=0;
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,craftNow)&&(smith||alch)&&eventObj&&Safe(eventObj,0x30)){
            static std::atomic_flag busy=ATOMIC_FLAG_INIT; // several animation threads: one writer at a time, others skip (diagnostic only)
            struct Release{std::atomic_flag& f;bool own;~Release(){if(own)f.clear(std::memory_order_release);}} rel{busy,!busy.test_and_set(std::memory_order_acquire)};
            std::uint32_t hash=0;(void)Read((unsigned char*)eventObj+0x1c,hash);
            static std::uint32_t seen[48]{};static unsigned seenCount=0;static std::uint64_t seenLife=0;
            if(rel.own){
            const auto life=witness175.lifetime175.load(std::memory_order_acquire);
            if(seenLife!=life){seenLife=life;seenCount=0;}
            bool known=false;for(unsigned i=0;i<seenCount;++i)if(seen[i]==hash){known=true;break;}
            if(!known&&seenCount<48){seen[seenCount++]=hash;
                char txt[3][48]{};
                for(int f=0;f<3;++f){const char* p=nullptr;if(!Read((unsigned char*)eventObj+f*8,p)||!p)continue;
                    unsigned n=0;for(;n+1<sizeof(txt[f]);++n){char c=0;if(!Read(p+n,c)||!c)break;if(c<32||c>126){n=0;break;}txt[f][n]=c;}txt[f][n]=0;}
                char rt[96]{},srt[96]{};Rtti(eventObj,rt,sizeof(rt));if(self&&Safe(self,sizeof(void*)))Rtti(self,srt,sizeof(srt));
                char m[704]{};std::snprintf(m,sizeof(m),"C36B F5_CRAFT_EVENT kind=%s smithPhase=%d hash=0x%08X self=%p/%s event=%s str0='%s' str8='%s' str10='%s' distinct=%u actorAttribution=UNVERIFIED fieldSemantics=RAW_PRINTABLE_POINTERS writes=NONE",
                    smith?"SMITHING":"ALCHEMY",smith?witness175.SmithPhaseC1():-1,hash,self,srt[0]?srt:"-",rt[0]?rt:"-",txt[0],txt[1],txt[2],seenCount);
                diagnostic_trace::Write(m);}}
        }
    }
    // Correlation metadata only. This does not classify hash values as attacks or
    // attach/detach success, and never suppresses the native event.
    const auto oldHash=g_dispatch_hash6690;const auto oldSelf=g_dispatch_self6690;
    std::uint32_t eventHash=0;
    if(diagnostics::lab::Get(diagnostics::lab::Key::F3)>0&&eventObj)
        (void)Read(static_cast<unsigned char*>(eventObj)+0x1c,eventHash);
    g_dispatch_hash6690=eventHash;g_dispatch_self6690=reinterpret_cast<std::uintptr_t>(self);
    if(g_attach_event_dispatch_original)g_attach_event_dispatch_original(self,eventObj);
    g_dispatch_hash6690=oldHash;g_dispatch_self6690=oldSelf;
}


static bool FiniteTmV6552(const Matrix34V6550& tm) noexcept {
    for(float f:tm.m) if(!std::isfinite(f)||std::fabs(f)>1000000.0f) return false;
    return true;
}

static bool ValidateEntityV6583(void* ent,std::uint32_t expected,void*& vft) noexcept {
    vft=nullptr;if(!ent||!expected||!Safe(ent,sizeof(void*)))return false;if(!Read(ent,vft)||!vft)return false;
    const auto base=WhBase();if(!base||reinterpret_cast<std::uintptr_t>(vft)!=base+kCEntityVftRva)return false;
    EntityIdGetterFn idfn=nullptr;if(!Read((unsigned char*)vft+0x08,idfn)||!idfn)return false;std::uint32_t id=0;__try{id=idfn(ent);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}return id==expected;
}
static bool GetEntityWorldTmV6552(void* ent,std::uint32_t expected,Matrix34V6550& out) noexcept {
    void* vft=nullptr;if(!ValidateEntityV6583(ent,expected,vft))return false;
    // Exact KCD2 CEntity vtable: +0x100 returns the cached world Matrix34 pointer
    // (lea rax,[rcx+0x58]; ret). +0xE8 is a hierarchy query, not GetWorldTM.
    EntityWorldTmGetterFn get=nullptr;if(!Read((unsigned char*)vft+0x100,get)||!get)return false;const Matrix34V6550* p=nullptr;__try{p=get(ent);}__except(EXCEPTION_EXECUTE_HANDLER){p=nullptr;}if(!p||!Safe(p,sizeof(out))||!Read(p,out))return false;return FiniteTmV6552(out);
}
static bool GetEntityLocalTmV6552(void* ent,std::uint32_t expected,Matrix34V6550& out) noexcept {
    void* vft=nullptr;if(!ValidateEntityV6583(ent,expected,vft))return false;
    EntityLocalTmGetterFn get=nullptr;if(!Read((unsigned char*)vft+0x108,get)||!get)return false;Matrix34V6550 tmp{};__try{get(&tmp,ent);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}if(!FiniteTmV6552(tmp))return false;out=tmp;return true;
}
// Retired unused implementation archived: research_archive/retired_pose_implementations


struct WeaponControlBaselineV6552 {
    void* ent=nullptr;
    std::uint32_t id=0;
    std::uint64_t gen=0;
    bool worldValid=false,localValid=false,oneShotApplied=false,persistComplete=false;
    std::uint64_t proofStartMs=0;
    Matrix34V6550 world{},local{};
};
static WeaponControlBaselineV6552 g_weapon_control_base[2]{};

// Retired unused implementation archived: research_archive/retired_pose_implementations


static std::atomic<std::uint64_t> g_weapon_contended6695{0},g_weapon_lost6695{0},g_weapon_restored6695{0};
static const char* WeaponControlModeNameImpl(int mode) noexcept;

static bool ArmPrediction120() noexcept;
static bool ArmBowZoom121() noexcept;
static void StartPredictionCapture120() noexcept;
static bool PredictionCapture120() noexcept;
static bool PredictionSweep120(std::uintptr_t,void*,int,int) noexcept;
static void NativeHeldInvalidateT4() noexcept;
static void SetWeaponControlModeImpl(int mode) noexcept {
    mode=std::clamp(mode,0,7);
    // Failure must never trap Cycle() at 3 -> failed 4 -> 3 forever.
    // Use the ordinary OFF path so epochs, pending restoration and native
    // cancellation see the same mode transition as a deliberate OFF request.
    if((mode>=4&&!ArmPhysicalWeapons6698())||(FullWeaponMode120(mode)&&!ArmMotionCombatInput101())||(mode==7&&!ArmPrediction120())){
        diagnostic_trace::Write("V66.135R1 PHYSICAL enable rejected: returning F12 to NATIVE/OFF; next USER press=BASELINE_3; native guards retained");
        mode=0;
    }
    if(BowHandlingMode59c9(mode)&&!ArmBowZoom121())diagnostic_trace::Write("V66.121 BOW_ZOOM unavailable: native camera value retained");
    g_contact_control_epoch6695.fetch_add(1,std::memory_order_acq_rel);
    if(g_weapon_control_mode.load()!=0){char end[448]{};
        std::snprintf(end,sizeof(end),"V66.95-F12 SESSION_END rung=%d next=%d applied=%llu contentionSkips=%llu sourceLosses=%llu nativeRestores=%llu restorePending=%d finalVisibility=USER_REPORT",
            g_weapon_control_mode.load(),mode,(unsigned long long)g_weapon_applied6694.load(),(unsigned long long)g_weapon_contended6695.load(),(unsigned long long)g_weapon_lost6695.load(),(unsigned long long)g_weapon_restored6695.load(),diagnostics::lab::weaponRestorePending.load());diagnostic_trace::Write(end);}
    g_weapon_contended6695.store(0);g_weapon_lost6695.store(0);g_weapon_restored6695.store(0);
    g_weapon_request_epoch6694.fetch_add(1,std::memory_order_acq_rel);
    NativeHeldInvalidateT4(); // retire the old collider even when attachment/source receipts are contended
    g_weapon_fault6694.store(false,std::memory_order_release);g_weapon_applied6694.store(0);
    if(mode==7&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64()))StartPredictionCapture120();
    g_weapon_reason6694.store(mode?1:0);g_weapon_control_mode.store(mode,std::memory_order_release);kcd2vr::haptics101::select(mode>=4||g_census_mode.load()>=4,std::uint64_t(GetTickCount64()));
    g_f12_attachment_proof_until_ms.store(0);g_f12_follow_requested6689.store(mode>=2);
    if(mode>0&&g_census_mode.load()==5)SetCombatCensusMode(3);
    if(mode>=3&&g_census_mode.load()<3)SetCombatCensusMode(3); // one deliberate combined-mode request, not a repeated override
    char m[512]{};std::snprintf(m,sizeof(m),"V66.94-F12 rung=%d %s previousProofRequired=0 fingerReferenceRequired=0 weaponOnlyNeedsVrik=0 nativeOwnershipRequired=1",mode,WeaponControlModeNameImpl(mode));diagnostic_trace::Write(m);
}
static void CycleWeaponControlModeImpl() noexcept {SetWeaponControlModeImpl((g_weapon_control_mode.load()+1)%8);}
static int WeaponControlModeImpl() noexcept {return g_weapon_control_mode.load(std::memory_order_acquire);}
static const char* WeaponControlModeNameImpl(int mode) noexcept {switch(mode){case 0:return "NATIVE / OFF";case 1:return "NATIVE ITEM/ATTACHMENT DIAGNOSTIC";case 2:return "CONTROLLER WEAPON ONLY / SKELETON SUSPENDED";case 3:return "CONTROLLER WEAPON + HANDS + UPPER BODY";case 4:return "PHYSICAL HANDLING / TWO HANDS / MOUNTS (NO ATTACKS)";case 5:return "TRACKED WEAPON + STATIC CONTACT (NO ATTACKS)";case 6:return "FULL WEAPON TEST / HOLD GRIP / SWINGS / GUARD / BOW / HOLSTERS";case 7:return "TRACKED NATIVE SWEEP EXPERIMENT + FULL WEAPON HANDLING";default:return "INVALID";}} }

// Retired unused implementation archived: research_archive/retired_pose_implementations



namespace {
static bool ResolveWeaponControlTargetV6553(int side) noexcept {
    std::uint32_t id=0;if(!NativeHeldItemId6694(side,id))return false;
    void* ent=EntityById6694(id);if(!ent)return false;
    g_weapon_control_entity_id[side].store(id,std::memory_order_release);
    g_weapon_control_entity[side].store(ent,std::memory_order_release);return true;
}

} // private exact-target resolver; same namespace as its declaration

// V66.89 qualified native placement transaction. The exact +0x4238B8 routine
// writes entity PRS THEN CCharInstance location (+0x423C30 -> instance+0xA08).
// Both receive ONE QuatTS. All other attachments call native unchanged.
struct AttachmentState6689 {
    void* self=nullptr;void* attachment=nullptr;void* entity=nullptr;std::uint32_t id=0;
    std::uint64_t generation=0;bool changed=false;std::uint64_t requestEpoch=0,lastLog=0;
    bool gripValid=false,usedCommitted=false;QuatT28 grip{};
    kcd2vr::pose_core::Stamp lastSource{};float lastWanted[8]{};int lastMode=0;
};
static AttachmentState6689 g_attachment6689;
static std::atomic_flag g_attachment_lock6689=ATOMIC_FLAG_INIT;
static std::atomic<void*> g_attachment_pending_self6695{nullptr},g_attachment_pending_object6695{nullptr};
struct AttachmentLock6689 { bool owned=!g_attachment_lock6689.test_and_set(std::memory_order_acquire);~AttachmentLock6689(){if(owned)g_attachment_lock6689.clear(std::memory_order_release);} };
static void MarkWeaponPending6695(void* self,void* attachment) noexcept {
    g_attachment_pending_object6695.store(attachment,std::memory_order_relaxed);
    g_attachment_pending_self6695.store(self,std::memory_order_release);
    diagnostics::lab::weaponRestorePending.store(self!=nullptr,std::memory_order_release);
}
static bool ReadAttachmentQs6689(void* attachment,float (&qs)[8]) noexcept {
    void* vft=nullptr;AttachmentWorldQuatTsFn fn=nullptr;
    if(!Read(attachment,vft)||!vft||!Read(static_cast<unsigned char*>(vft)+0x70,fn)||!fn)return false;
    __try {fn(attachment,qs);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    double qn=0;for(int i=0;i<8;++i)if(!std::isfinite(qs[i]))return false;
    for(int i=0;i<4;++i)qn+=double(qs[i])*qs[i];
    return qn>.8&&qn<1.2&&qs[7]>.001f&&qs[7]<100;
}
static void* CharacterForEntity6689(void* ent) noexcept {
    void* vf=nullptr;void* fn=nullptr;if(!Read(ent,vf)||!vf||!Read(static_cast<unsigned char*>(vf)+0x328,fn)||!fn)return nullptr;
    __try{return reinterpret_cast<void*(__fastcall*)(void*,int)>(fn)(ent,0);}__except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
// V66.128: capture the held bow's character. The entity vtable slot must be the exact
// retail CEntity::GetCharacter and the result must be a validated CCharInstance.
#include "subsystems/ranged_reload/bow_character_capture.inl" // M2_OWNED_SOURCE
static bool PlaceBothConsumers6689(void* self,void* ent,const float (&qs)[8],bool& hasCharacter) noexcept {
    void* vf=nullptr;EntitySetPosRotScaleFn prs=nullptr;float scale[3]{};
    if(!Read(ent,vf)||!vf||!Read(static_cast<unsigned char*>(vf)+0x168,prs)||!prs)return false;
    for(int i=0;i<3;++i){if(!Read(static_cast<unsigned char*>(self)+0x0C+i*4,scale[i])||!std::isfinite(scale[i]))return false;scale[i]*=qs[7];}
    // Exceptions are isolated in this leaf; no RAII object crosses an MSVC SEH block.
    __try {prs(ent,qs+4,qs,scale,0x10u);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    void* child=CharacterForEntity6689(ent);hasCharacter=child!=nullptr;
    if(child){
        void* cv=nullptr;void* update=nullptr;
        if(!Read(child,cv)||!cv||!Read(static_cast<unsigned char*>(cv)+0x190,update)||update!=reinterpret_cast<void*>(WhBase()+0x423C30ull))return false;
        __try{reinterpret_cast<void(__fastcall*)(void*,const float*)>(update)(child,qs);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    }
    {std::uint32_t guardIdR120=0;if(Read(static_cast<unsigned char*>(ent)+0x0C,guardIdR120))kcd2vr::draw_guard_r120::markPlaced(guardIdR120);} // R120
    return true;
}
static bool PlacementReadback6689(void* self,void* ent,std::uint32_t id,const float (&qs)[8],bool& characterChecked) noexcept {
    Matrix34V6550 tm{};if(!GetEntityWorldTmV6552(ent,id,tm))return false;
    const V3 delta{tm.m[3]-qs[4],tm.m[7]-qs[5],tm.m[11]-qs[6]};if(Len(delta)>.008f)return false;
    // Verify the native per-axis scale as well as position and quaternion.
    for(int c=0;c<3;++c){float scale=0;if(!Read(static_cast<unsigned char*>(self)+0x0C+c*4,scale))return false;
        const float expected=std::fabs(scale*qs[7]);const float actual=Len(V3{tm.m[c],tm.m[4+c],tm.m[8+c]});
        if(!std::isfinite(actual)||expected<.0001f||std::fabs(actual-expected)>.001f*std::max(1.f,expected))return false;}
    // Check rotation independently of scale; never call position readback full-pose proof.
    const V3 x=Normalize({tm.m[0],tm.m[4],tm.m[8]}),y=Normalize({tm.m[1],tm.m[5],tm.m[9]}),z=Normalize({tm.m[2],tm.m[6],tm.m[10]});
    const Q4 q=QFromBasis(x,y,z);const double dot=std::fabs(q.x*qs[0]+q.y*qs[1]+q.z*qs[2]+q.w*qs[3]);if(dot<.999)return false;
    void* child=CharacterForEntity6689(ent);characterChecked=child!=nullptr;
    if(child){float actual[8]{};if(!Read(static_cast<unsigned char*>(child)+0xA08,actual))return false;
        for(int i=0;i<8;++i)if(!std::isfinite(actual[i])||std::fabs(actual[i]-qs[i])>.001f)return false;}
    return true;
}
#include "subsystems/physics/controller_attachment_bridge.inl" // M2_OWNED_SOURCE

namespace {static PhysicalAttachmentResult101 NativeHeldAttachmentT4(void*,void*) noexcept;}
namespace {static void StationWeaponAfterAttachmentD56(void*,void*) noexcept;}
namespace {static void DrawGuardAfterCallbackR120(void*,void*) noexcept;}
static void __fastcall EntityAttachmentUpdateDetourV6688(void* self,void* attachment) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::AttachmentUpdate};
    struct StationVisibilityD56 {void* self;void* attachment;
        ~StationVisibilityD56(){StationWeaponAfterAttachmentD56(self,attachment);DrawGuardAfterCallbackR120(self,attachment);}} stationVisibilityD56{self,attachment};
    const AttachmentObservation15 observation{self,attachment};
    const auto native=[&](){InvalidateReadCache101R2();if(g_entity_attachment_update_original){kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::AttachmentUpdate};g_entity_attachment_update_original(self,attachment);}InvalidateReadCache101R2();};
    if(!self||!attachment){native();return;}
    // A retired/unknown solver body can still collide. No native/controller,
    // activity, grace or tool writer resumes before exact collider retirement.
    WeaponDrawBeforeAttachmentD75(self,attachment);
    const auto heldT4=NativeHeldAttachmentT4(self,attachment);
    if(heldT4==PhysicalAttachmentResult101::Placed||heldT4==PhysicalAttachmentResult101::Busy)return;
    if(CrossbowAttachmentXR34(self,attachment))return;
    // C46R1: semi-passenger tools use their real native attachment transform.
#include "vrik/native_activity_passthrough46.inl"
    // Exact native-held tools have a separate F5 owner; weapons still take their
    // existing physical path. OFF/reference is a cheap no-op in this function.
    if(ManualArrowAttachment64(self,attachment))return;
    if(ToolAttachment59c10(self,attachment))return;
    const int mode=g_weapon_control_mode.load(std::memory_order_acquire);
    const bool pendingCandidate=g_attachment_pending_self6695.load(std::memory_order_acquire)==self &&
        g_attachment_pending_object6695.load(std::memory_order_relaxed)==attachment;
    if(mode==0&&!pendingCandidate&&!PhysicalAttachmentPending6698()){native();return;}
    ReadScope101R2 pages;
    // Placement is an explicit result, not the old ambiguous "handled" bool.
    if(mode>=4||PhysicalAttachmentPending6698()){
        const auto result=PhysicalAttachmentUpdate101(self,attachment);
        if(result==PhysicalAttachmentResult101::Placed){
            AttachmentLock6689 old;if(old.owned&&g_attachment6689.self==self&&g_attachment6689.attachment==attachment){
                // Keep the qualified backup grip, but no second active writer.
                g_attachment6689.changed=false;MarkWeaponPending6695(nullptr,nullptr);
            }
            return;
        }
        if(result==PhysicalAttachmentResult101::Busy)return;
        if(result==PhysicalAttachmentResult101::Restored||result==PhysicalAttachmentResult101::Fault){
            AttachmentLock6689 old;if(old.owned&&g_attachment6689.self==self&&g_attachment6689.attachment==attachment){g_attachment6689={};MarkWeaponPending6695(nullptr,nullptr);}return;
        }
        if(mode>=4){
            // V66.157: keep ONE authoritative full-physical writer.  The retired
            // controller bridge is deliberately NOT revived here because it does
            // not carry the physical contact/reach result.  Count this native
            // WAITING fallback so headset logs tell us exactly how often the
            // physical producer failed to publish before we change ownership.
            if(result==PhysicalAttachmentResult101::Waiting)g_phys_waiting_native157.fetch_add(1,std::memory_order_relaxed);
            native();
            AttachmentLock6689 old;if(old.owned&&g_attachment6689.self==self&&g_attachment6689.attachment==attachment){
                g_attachment6689.changed=false;MarkWeaponPending6695(nullptr,nullptr);
            }
            return;
        }
        // A lower rung may resume its own writer after physical pending work.
    }
    const int controllerMode=mode>=3?3:mode;
    // Filter unrelated traffic BEFORE entering the exact-target transaction.
    // Only the same currently-owned/pending attachment may skip duplicate
    // placement when another callback already owns that transaction.
    std::uint32_t bound=0,id=0;
    // Candidate lookup is NOT authorization. Unrelated world attachments must
    // not each walk Henry's manager/player/hand chain (each Read uses VirtualQuery).
    // Exact current hand membership is still required here AND before placement.
    std::uint32_t candidateId=0;
    const bool match=Read(static_cast<unsigned char*>(self)+8,bound)&&bound&&
        NativeCandidateItemId6699(0,candidateId)&&bound==candidateId&&
        NativeHeldItemId6694(0,id)&&id&&bound==id;
    if(!match&&!pendingCandidate){native();return;}
    AttachmentLock6689 guard;
    if(!guard.owned){g_weapon_contended6695.fetch_add(1,std::memory_order_relaxed);return;}
    auto& state=g_attachment6689;
    const auto restore=[&](){
        native();if(state.changed)g_weapon_restored6695.fetch_add(1,std::memory_order_relaxed);
        state.changed=false;state.lastSource={};MarkWeaponPending6695(nullptr,nullptr);
    };
    if(!match){
        const bool relinquished=state.changed;restore();state={};
        g_weapon_control_entity_id[0].store(0);g_weapon_control_entity[0].store(nullptr);
        g_weapon_reason6694.store(2);
        if(relinquished)diagnostic_trace::Write("V66.97-F12 HAND_ROLE_REVOKED gripCleared=1 destination=CURRENT_NATIVE noHistoricalHandRestore=1");
        return;
    }
    if(mode<2||diagnostics::lab::NativeReference()||NativePassengerCurrentA34(GetTickCount64())||g_weapon_fault6694.load(std::memory_order_acquire)){
        const bool changed=state.changed;restore();state.gripValid=false;
        g_weapon_reason6694.store(changed?8:diagnostics::lab::NativeReference()?5:g_weapon_fault6694.load()?6:3);
        if(mode==1){const auto now=static_cast<std::uint64_t>(GetTickCount64());if(now-state.lastLog>=2000){state.lastLog=now;char m[384]{};
            std::snprintf(m,sizeof(m),"V66.95-F12 BOUND item=%p id=%u attachment=%p key=%016llX route=C_ITEM_LINKED_ID_NATIVE_ENTITY_SYSTEM writes=NONE",g_native_side_item[0].load(),id,attachment,(unsigned long long)g_native_side_wuid0[0].load());diagnostic_trace::Write(m);}}
        return;
    }
    void* entity=EntityById6694(id);if(!entity){restore();state={};g_weapon_reason6694.store(3);return;}
    g_weapon_control_entity_id[0].store(id,std::memory_order_release);g_weapon_control_entity[0].store(entity,std::memory_order_release);
    const auto generation=g_weapon_control_generation[0].load(std::memory_order_acquire);
    const auto request=g_weapon_request_epoch6694.load(std::memory_order_acquire);
    if(state.self!=self||state.attachment!=attachment||state.generation!=generation||state.id!=id){
        restore();state={};state.self=self;state.attachment=attachment;state.entity=entity;state.id=id;state.generation=generation;state.requestEpoch=request;
    }else if(state.requestEpoch!=request){
        // Preserve only this exact item's already-qualified combined grip across
        // 3->physical->3 transitions. Never carry raw/committed bases interchangeably.
        if(controllerMode!=state.lastMode)state.gripValid=false;
        state.requestEpoch=request;
    }
    float original[8]{},wanted[8]{};
    if(!ReadAttachmentQs6689(attachment,original)){restore();state.gripValid=false;g_weapon_reason6694.store(3);return;}
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(!ControllerWeaponTransform6694(state,controllerMode,original,wanted)){
        // A new combined hand is unavailable: do not substitute unconstrained
        // controller coordinates. Retain this SAME source only until its original
        // capture deadline, never extending its age by using it again.
        const bool usable=state.changed&&state.lastMode==controllerMode&&g_gameplay_allowed.load()&&TrackingHeadUsable111(now)&&TrackingHandUsable111(0,now)&&
            kcd2vr::pose_core::fresh(state.lastSource,g_pose_epoch6689.load(),controllerMode==3?reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()):state.lastSource.actor,now)&&
            (controllerMode!=3||g_committed_pose_valid6689.load());
        if(!usable){if(state.changed)g_weapon_lost6695.fetch_add(1);restore();g_weapon_reason6694.store(controllerMode==3?9:4);return;}
        std::copy_n(state.lastWanted,8,wanted);
    }
    if(generation!=g_weapon_control_generation[0].load()||request!=g_weapon_request_epoch6694.load()||
       mode!=g_weapon_control_mode.load()||diagnostics::lab::NativeReference()||NativePassengerCurrentA34(now)||
       !g_gameplay_allowed.load()||!TrackingHeadUsable111(now)||!TrackingHandUsable111(0,now)) {restore();return;}
    std::uint32_t finalHandId=0;
    if(!NativeHeldItemId6694(0,finalHandId)||finalHandId!=id){restore();state={};g_weapon_reason6694.store(2);return;}
    bool child=false;if(!PlaceBothConsumers6689(self,entity,wanted,child)){
        restore();state.gripValid=false;g_weapon_fault6694.store(true);g_weapon_reason6694.store(6);
        diagnostic_trace::Write("V66.95-F12 PLACEMENT_FAULT nativeRestorationRequested=1 faultLatched=1");return;
    }
    state.changed=true;MarkWeaponPending6695(self,attachment);g_weapon_reason6694.store(7);
    const auto n=g_weapon_applied6694.fetch_add(1)+1;g_f12_attachment_proof_writes.fetch_add(1);
    if(n==1){bool childCheck=false;const bool readback=PlacementReadback6689(self,entity,id,wanted,childCheck);
        char m[448]{};std::snprintf(m,sizeof(m),"V66.95-F12 CONTROL_STARTED mode=%d id=%u nativeReadback=%d childConsumer=%d handSource=%s visibleProof=USER_REQUIRED timerProofGate=REMOVED",mode,id,readback,child,state.usedCommitted?"COMMITTED_HAND":"RAW_TRACKING");diagnostic_trace::Write(m);}
}

static void WeaponControlTickImpl() noexcept {
    static int previousMode=-1,previousReason=-1;static std::uint64_t last=0;
    const int mode=g_weapon_control_mode.load(),reason=g_weapon_reason6694.load();const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(mode==previousMode&&now-last<1000)return;
    if(mode==previousMode&&reason==previousReason&&(mode!=1||now-last<2000))return;
    previousMode=mode;previousReason=reason;last=now;if(!mode)return;
    char m[384]{};std::snprintf(m,sizeof(m),"V66.94-F12 STATUS rung=%d state=%s id=%u applied=%llu restorePending=%d noWorkerNativeCalls=1",mode,WeaponReason6694(reason),g_weapon_control_entity_id[0].load(),(unsigned long long)g_weapon_applied6694.load(),diagnostics::lab::weaponRestorePending.load());diagnostic_trace::Write(m);
}



#include "subsystems/vrik/native_ik_observer.inl" // M2_OWNED_SOURCE
// V66.92R1: corrected world pointer and fault containment. Runtime contact and
// safe native scheduling remain unproven. F10 rung 4 is an explicit experiment.
namespace wc = kcd2vr::world_query_contract;
namespace npc172 = kcd2vr::npc_contact172;
#include "vrik/npc_query_contract172.inl"
struct NpcQuery172 {
    bool requested=false,prepared=false,ready=false;
    npc172::Gate gate=npc172::Gate::Off;
    npc172::Exclusions skip{};
};
static thread_local NpcQuery172 g_npc_query172;
static std::atomic<npc172::Gate> g_npc_gate172{npc172::Gate::Off};
static std::atomic<std::uint64_t> g_npc_queries172{0},g_npc_blocked172{0},g_npc_self172{0};
static std::array<std::atomic<std::uint64_t>,unsigned(npc172::Body::Count)> g_npc_hits172{};
static bool PrepareNpcQuery172() noexcept;
static bool NpcQuerySelected172() noexcept {
    return npc172::selected(FocusedTestMode167(),g_weapon_control_mode.load(),diagnostics::lab::NativeReference(),g_census_mode.load());
}
constexpr std::uintptr_t kRayWorldIntersectionRva6692=wc::kHelperRva;
constexpr int kWorldOnlyObjTypes6692=0x101;
constexpr unsigned kWorldOnlyFlags6692=0x8002040Fu;
using RayWorldIntersectionFn6692=int(__fastcall*)(void*,const V3*,const V3*,int,unsigned,void*,int,void*,int,void*,int,const char*);
static std::atomic<int> g_collision_binding6692{0}; // prefix/caller contract only, NOT live success
static wc::Circuit g_collision_circuit6693;
static std::atomic<std::uint64_t> g_collision_hits6692{0},g_collision_last_status6692{0};
static std::atomic<int> g_collision_live6693{0}; // 0 untested/waiting, 1 query returned, -1 wrong live type
static thread_local bool g_collision_fan_valid6693=false;
static thread_local wc::Result g_collision_fan_result6695=wc::Result::Disabled;

static bool MatchRayHelper6692(std::uintptr_t base) noexcept {
    static constexpr unsigned char kPrefix[]{
        0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,0x4C,
        0x89,0x70,0x20,0x55
    };
    unsigned char prefix[sizeof(kPrefix)]{};
    if(!Read(reinterpret_cast<void*>(base+wc::kHelperRva),prefix)||std::memcmp(prefix,kPrefix,sizeof(kPrefix))!=0)return false;
    // Validate the RIP-relative pointer source, not merely a function prologue.
    for(auto rva:wc::kNativeWorldLoadRvas){
        unsigned char op[7]{};if(!Read(reinterpret_cast<void*>(base+rva),op))return false;
        if(op[0]!=0x48||op[1]!=0x8B||op[2]!=0x0D)return false;
        std::int32_t displacement=0;std::memcpy(&displacement,op+3,4);
        if(std::int64_t(rva)+7+displacement!=std::int64_t(wc::kWorldSlotRva))return false;
    }
    std::uintptr_t method=0;
    return Read(reinterpret_cast<void*>(base+wc::kWorldVtableRva+wc::kQueryVtableOffset),method)&&method==base+wc::kQueryMethodRva;
}
static bool CollisionBinding6692(std::uintptr_t base) noexcept {
    if(!g_collision_circuit6693.enabled())return false;
    int state=g_collision_binding6692.load(std::memory_order_acquire);if(state)return state>0;
    const bool ok=SupportedWhBuild(base)&&MatchRayHelper6692(base);
    int expected=0;if(g_collision_binding6692.compare_exchange_strong(expected,ok?1:-1,std::memory_order_acq_rel)){
        char m[384]{};std::snprintf(m,sizeof(m),
            "V66.92R1-COLLISION BINDING %s helperRva=0x%llX worldPointerSlotRva=0x%llX callerLoads=3 liveQuery=UNTESTED",
            ok?"CONTRACT_MATCH":"BLOCKED_CONTRACT",(unsigned long long)wc::kHelperRva,(unsigned long long)wc::kWorldSlotRva);diagnostic_trace::Write(m);
    }
    return ok;
}
// SEH is confined to this leaf. -1 is a fault, never an empty ray result.
static int RayWorldGuarded6692(RayWorldIntersectionFn6692 fn,void* world,const V3* origin,const V3* dir,void* hit,
                              bool bodies,void** skip,int count) noexcept {
    __try{return fn(world,origin,dir,bodies?npc172::BodiesAndWorld:kWorldOnlyObjTypes6692,
        bodies?npc172::BodyFlags:kWorldOnlyFlags6692,hit,1,skip,count,nullptr,0,"KCD2VR_VRIK");}
    __except(EXCEPTION_EXECUTE_HANDLER){return -1;}
}
static std::uint64_t ContactMicros6695() noexcept {
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}
static thread_local kcd2vr::contact::Budget g_contact_budget6695;
static thread_local std::uint64_t g_contact_budget_sample65=0;
static thread_local std::uint64_t g_contact_query_tick_h18=0;
static thread_local std::uint64_t g_contact_query_sample_d75=0;
static thread_local std::uint64_t g_contact_query_ns_d79=0;
static thread_local std::uint64_t g_contact_tracking_d53=0;
static thread_local bool g_contact_budget_active6695=false,g_contact_pass6695=false;
static wc::LatencyBackoff59c1 g_contact_latency59c1;
static bool ContactSuspended59c1() noexcept {
    return !g_collision_circuit6693.enabled()||!g_contact_latency59c1.ready(ContactMicros6695());
}
static bool ContactFaultH30() noexcept {return !g_collision_circuit6693.enabled();}
static std::atomic<std::uint64_t> g_contact_budget_skips6695{0},g_contact_max_us6695{0};
#include "vrik/door_surface_budget65.inl"
struct ContactPass6695 {
    bool active=false;
    ContactPass6695(bool enabled,std::uint64_t tick,std::uint64_t& previous,std::uint64_t sample=0,std::uint64_t trackingD53=0,std::uint64_t timeNsD79=0) noexcept {
        active=enabled;g_contact_budget_active6695=enabled;g_contact_query_tick_h18=enabled?tick:0;
        g_contact_tracking_d53=enabled?trackingD53:0;
        g_contact_query_ns_d79=enabled?timeNsD79:0;
        g_contact_query_sample_d75=enabled?(sample?sample:tick):0;
        g_npc_query172={};g_npc_query172.requested=enabled&&NpcQuerySelected172();
        // One budget per NEW shared XR sample, not a 16ms sample-and-hold.
        // Repeated eye/skinning callbacks keep the committed constraint result.
        const auto key=sample?sample:tick;
        // A rejected solve can leave this exact thread/sample budget unused.
        // Resume only that virgin budget; never replenish consumed rays/time or
        // borrow another worker's budget. The physical result remains staged.
        const bool unusedSameSampleR16=key&&key==previous&&g_contact_budget_sample65==key&&
            g_contact_budget6695.workOnly&&g_contact_budget6695.used==0&&g_contact_budget6695.spent==0&&
            g_contact_budget6695.remaining>0&&g_contact_budget6695.limit>0&&!g_contact_budget6695.inFlight;
        g_contact_pass6695=enabled&&g_contact_latency59c1.ready(ContactMicros6695())&&key&&(key!=previous||unusedSameSampleR16);
        if(g_contact_pass6695){previous=key;BeginContactBudget65(key,GetTickCount64());}
    }
    ~ContactPass6695(){g_npc_query172={};g_contact_budget_active6695=false;g_contact_pass6695=false;g_contact_query_tick_h18=0;g_contact_tracking_d53=0;g_contact_query_sample_d75=0;g_contact_query_ns_d79=0;}
};
static std::atomic<int> g_rigid_contact_contract_d67r1{0};
static thread_local bool g_hand_rigid_contact_d67r1=false;
struct HandRigidContactScopeD67R1 {
    bool previous=g_hand_rigid_contact_d67r1;
    HandRigidContactScopeD67R1() noexcept {g_hand_rigid_contact_d67r1=true;}
    ~HandRigidContactScopeD67R1(){g_hand_rigid_contact_d67r1=previous;}
};
static bool RigidContactContractD67R1(std::uintptr_t base) noexcept {
    // A pending full-image lease is not a permanent instruction-pin failure.
    // Recheck before the cache so its positive result cannot outlive that lease.
    if(!base||!retail_build112::Ready(base))return false;
    const int cached=g_rigid_contact_contract_d67r1.load(std::memory_order_acquire);if(cached)return cached>0;
    const bool ok=SupportedWhBuild(base)&&npc172::rigidSurfaceContractD67R1(base,
        [](std::uintptr_t address,auto& value){return Read(reinterpret_cast<const void*>(address),value);});
    int expected=0;g_rigid_contact_contract_d67r1.compare_exchange_strong(expected,ok?1:-1,std::memory_order_acq_rel);
    return ok;
}
static bool CurrentRigidPlaneD67R1(const npc172::Witness& witness) noexcept {
    return g_contact_pass6695&&npc172::currentRigidSurfaceD67R1(witness,g_contact_budget_sample65,g_contact_query_tick_h18);
}
static bool CurrentBodyPlaneD71(const npc172::Witness& witness) noexcept {
    return kcd2vr::contact_handover_d71::ContactContinuity()&&!diagnostics::lab::NativeReference()&&
        g_contact_pass6695&&npc172::currentBodySurfaceD71(witness,g_contact_budget_sample65,g_contact_query_tick_h18);
}
struct WorldRayResult6693 {npc172::Witness native172{};kcd2vr::sweep169r2::Reason reason169r2=kcd2vr::sweep169r2::Reason::BindingUnavailable;wc::Result state=wc::Result::Disabled;float distance=0;V3 point{},normal{},nativeNormalD63{};bool plane=false;std::uint64_t witness169r1=0;};
static WorldRayResult6693 WorldRayDistance6692(V3 origin,V3 sweep) noexcept {
    WorldRayResult6693 out{};
    if(!g_contact_latency59c1.ready(ContactMicros6695())){
        out.state=wc::Result::Busy;out.reason169r2=kcd2vr::sweep169r2::Reason::QueryBusy;return out;
    }
    if(g_contact_budget_active6695&&(!g_contact_pass6695||!g_contact_budget6695.take(ContactMicros6695()))){
        g_contact_budget_skips6695.fetch_add(1,std::memory_order_relaxed);out.state=wc::Result::Busy;
        using R=kcd2vr::sweep169r2::Reason;
        out.reason169r2=!g_contact_pass6695?(!g_contact_latency59c1.ready(ContactMicros6695())?R::QueryBusy:R::SameSample):
            !g_contact_budget6695.remaining?R::RayCap:g_contact_budget6695.spent>=g_contact_budget6695.limit?R::TimeBudget:
            g_contact_budget6695.inFlight?R::QueryBusy:R::InvalidQuery;
        return out;}
    // Charge only the query adapter's actual work. Owner, shoulder and pose work
    // between queries cannot starve the very first ray of its own budget.
    struct QueryCost104 {bool active;~QueryCost104(){if(active)g_contact_budget6695.finish(ContactMicros6695());}} queryCost{g_contact_budget_active6695};
    const float length=Len(sweep);if(!std::isfinite(length)||length<1.0e-4f)return out;
    const auto base=WhBase();if(!CollisionBinding6692(base))return out;
    const bool bodies=g_npc_query172.requested;
    if(bodies&&!PrepareNpcQuery172()){
        ++g_npc_blocked172;g_npc_gate172.store(g_npc_query172.gate);
        // Missing self exclusions or an unqualified body never means NPC clearance.
        out.state=wc::Result::Busy;return out;
    }
    void* world=nullptr;std::uintptr_t vft=0,method=0;
    if(!Read(reinterpret_cast<void*>(base+wc::kWorldSlotRva),world)||!world){g_collision_live6693.store(0);return out;}
    if(!Read(world,vft)||vft!=base+wc::kWorldVtableRva||
       !Read(reinterpret_cast<void*>(vft+wc::kQueryVtableOffset),method)||method!=base+wc::kQueryMethodRva){
        g_collision_live6693.store(-1);g_collision_circuit6693.invalidate();return out;
    }
    alignas(16) std::array<std::byte,0x50> hit{};
    npc172::Witness witness{};if(bodies)witness.mask=npc172::BodiesAndWorld;
    auto fn=reinterpret_cast<RayWorldIntersectionFn6692>(base+wc::kHelperRva);
    out.state=g_collision_circuit6693.invoke([&]() noexcept {
        // Another callback can finish a slow query after our outer check.
        if(!g_contact_latency59c1.ready(ContactMicros6695()))return wc::Result::Busy;
        const auto begin=ContactMicros6695();
        const int n=RayWorldGuarded6692(fn,world,&origin,&sweep,hit.data(),bodies,
            bodies?g_npc_query172.skip.bodies.data():nullptr,bodies?int(g_npc_query172.skip.bodyCount):0);
        if(bodies){++g_npc_queries172;g_npc_gate172.store(npc172::Gate::Ready);}
        const auto elapsed=ContactMicros6695()-begin;
        auto prior=g_contact_max_us6695.load();while(prior<elapsed&&!g_contact_max_us6695.compare_exchange_weak(prior,elapsed)){}
        if(n<0)return wc::Result::Fault;
        if(n==0)return g_contact_latency59c1.observe(wc::Result::Clear,elapsed,ContactMicros6695());
        if(n!=1)return wc::Result::Invalid;
        std::memcpy(&out.distance,hit.data(),sizeof(float));
        if(!std::isfinite(out.distance)||out.distance<0||out.distance>length+.05f)return wc::Result::Invalid;
        // Pinned 0x50 ray_hit: point +0x24 (retail +50B374), normal
        // +0x30 (Cry layout). Additional fields are accepted only after checking
        // the point against THIS ray and a finite near-unit normal.
        std::memcpy(&out.point,hit.data()+0x24,sizeof(V3));
        std::memcpy(&out.normal,hit.data()+0x30,sizeof(V3));
        const V3 expected=Add(origin,Mul(sweep,out.distance/length));
        const float nn=Len(out.normal),error=Len(Sub(out.point,expected));
        out.plane=std::isfinite(nn)&&nn>.8f&&nn<1.2f&&std::isfinite(error)&&error<.025f;
        if(out.plane){out.normal=Mul(out.normal,1.f/nn);out.nativeNormalD63=out.normal;
            if(Dot(out.normal,Sub(origin,out.point))<0)out.normal=Mul(out.normal,-1.f);}
        {std::int16_t surface=-1;std::memcpy(&surface,hit.data()+0x18,sizeof(surface));witness.surfaceIdx=surface>=0?int(surface):-1;} // V66.178R12 (Cry ray_hit::surface_idx)
        {std::int16_t mat=-1;std::memcpy(&mat,hit.data()+0x1A,sizeof(mat));witness.bodyMaterial=mat>=0?int(mat):-1;} // R59.16C7E (Cry ray_hit::idmatOrg)
        if(bodies){
            void* collider=nullptr;void* owner=nullptr;std::uintptr_t vf=0,foreignMethod=0,ownerVf=0;
            std::memcpy(&collider,hit.data()+8,sizeof(collider));
            std::memcpy(&witness.part,hit.data()+0x14,sizeof(witness.part));
            witness.collider=reinterpret_cast<std::uintptr_t>(collider);
            // Returned bodies are borrowed for this synchronous query only. The
            // exact GetForeignData leaf proves +20; do not invoke an SDK slot.
            if(collider&&Read(collider,vf)&&vf>=base&&vf-base<0x6000000){
                witness.vft=vf-base;witness.body=npc172::classify(witness.vft);
                if(Read(reinterpret_cast<void*>(vf+0x50),foreignMethod)&&foreignMethod==base+0x5D6F44){
                    const bool foreignReadD62=Read(static_cast<unsigned char*>(collider)+0x20,owner);
                    witness.foreignNullD62=foreignReadD62&&!owner;
                    if(foreignReadD62&&owner&&Read(owner,ownerVf)&&ownerVf==base+0x4072C78&&
                       Read(static_cast<unsigned char*>(owner)+0xC,witness.entityId)){
                        witness.owner=reinterpret_cast<std::uintptr_t>(owner);witness.ownerRead=true;
                    }else owner=nullptr;
                }
            }
            if(g_npc_query172.skip.excludes(owner,collider)){
                ++g_npc_self172;g_npc_gate172.store(npc172::Gate::SelfHit);
                out.plane=false;return wc::Result::Busy;
            }
            if(out.plane&&g_hand_rigid_contact_d67r1&&g_contact_pass6695&&witness.body==npc172::Body::Rigid){
                int partIndex=-1;std::memcpy(&partIndex,hit.data()+0x10,sizeof(partIndex));
                // This copied receipt authorizes only locality against freshly
                // queried planes. Failure retains ordinary collision handling;
                // it never excludes the item or calls missing geometry clear.
                (void)npc172::qualifyRigidSurfaceD67R1(witness,base,partIndex,g_contact_budget_sample65,g_contact_query_tick_h18,RigidContactContractD67R1(base),
                    [](std::uintptr_t address,auto& value){return Read(reinterpret_cast<const void*>(address),value);});
            }
            // D71: never use the ray-facing (possibly flipped) normal to
            // claim an outside-origin witness. Only the native front face of
            // the exact current body/part can opt into local proxy projection.
            // No entity/world transform is substituted for an articulated part.
            if(out.plane&&g_hand_rigid_contact_d67r1&&g_contact_pass6695&&
               kcd2vr::contact_handover_d71::ContactContinuity()&&!diagnostics::lab::NativeReference()&&
               retail_build112::Ready(base)&&npc172::bodySurfaceIdentityD71(witness)&&
               Dot(out.nativeNormalD63,Sub(origin,out.point))>.0001f){
                witness.bodyCurrentSampleD71=g_contact_budget_sample65;
                witness.bodyCurrentTickD71=g_contact_query_tick_h18;
            }
            ++g_npc_hits172[unsigned(witness.body)];
        }
        return g_contact_latency59c1.observe(wc::Result::Hit,elapsed,ContactMicros6695());
    });
    out.reason169r2=out.state==wc::Result::Clear?kcd2vr::sweep169r2::Reason::FullClear:
        out.state==wc::Result::Hit?kcd2vr::sweep169r2::Reason::SurfaceHit:
        out.state==wc::Result::Fault?kcd2vr::sweep169r2::Reason::CallFault:kcd2vr::sweep169r2::Reason::InvalidQuery;
    if(out.state==wc::Result::Clear||out.state==wc::Result::Hit)g_collision_live6693.store(1);
    if(out.state==wc::Result::Hit){
        out.native172=witness; // qualified synchronous non-self contact, independent of recording
        g_collision_hits6692.fetch_add(1,std::memory_order_relaxed);
        if(out.plane)out.witness169r1=kcd2vr::diagnostics::passive169r1::ray(hit.data(),
            {origin.x,origin.y,origin.z},{sweep.x,sweep.y,sweep.z},out.distance,witness);
    }
    return out;
}
static std::optional<float> WorldRayFan6692(V3 origin,V3 sweep,float radius) noexcept {
    g_collision_fan_valid6693=false;g_collision_fan_result6695=wc::Result::Disabled;
    const float length=Len(sweep);if(length<1.0e-4f||!std::isfinite(radius)||radius<0)return std::nullopt;
    const V3 dir=Mul(sweep,1.0f/length);
    const V3 seed=std::fabs(dir.z)<.9f?V3{0,0,1}:V3{1,0,0};
    const V3 right=Normalize(Cross(dir,seed)),up=Normalize(Cross(right,dir));
    if(Len(right)<.5f)return std::nullopt;
    const V3 offs[5]{{0,0,0},Mul(right,radius),Mul(right,-radius),Mul(up,radius),Mul(up,-radius)};
    std::optional<float> best;
    const int rays=radius>0?5:1;
    for(int oi=0;oi<rays;++oi){const auto off=offs[oi];
        const auto h=WorldRayDistance6692(Add(origin,off),sweep);
        g_collision_fan_result6695=h.state;
        if(h.state!=wc::Result::Hit&&h.state!=wc::Result::Clear)return std::nullopt; // unknown, NOT clear
        if(h.state==wc::Result::Hit&&(!best||h.distance<*best))best=h.distance;
    }
    g_collision_fan_valid6693=true;return best;
}
struct ModelWorld6692{Q4 q{};V3 t{};float scale=1;kcd2vr::native_roomscale_d76::Origin roomOriginD76{};};
#include "subsystems/dialogues/head_mailbox.inl" // M2_OWNED_SOURCE
static bool ReadModelWorld6692(void* character,ModelWorld6692& out) noexcept {
    namespace nr=kcd2vr::native_roomscale_d76;
    nr::Origin roomOrigin{};
    if(!character||!nr::ReadCharacterOrigin(reinterpret_cast<std::uintptr_t>(character),roomOrigin))return false;
    float v[8]{};if(!Read(static_cast<unsigned char*>(character)+0xA08,v)||
       !nr::SameSerial(nr::Surface::Character,reinterpret_cast<std::uintptr_t>(character),roomOrigin.serial))return false;
    double n=0;for(float f:v)if(!std::isfinite(f))return false;for(int i=0;i<4;++i)n+=double(v[i])*v[i];
    if(n<.8||n>1.2||v[7]<.1f||v[7]>10)return false;
    out.q=QNormalize({v[0],v[1],v[2],v[3]});out.t={v[4],v[5],v[6]};out.scale=v[7];out.roomOriginD76=roomOrigin;return true;
}
#include "subsystems/roomscale/pose_producer_d76.inl"
static V3 ModelToWorld6692(const ModelWorld6692& x,V3 p) noexcept{return Add(x.t,QRotateV(x.q,Mul(p,x.scale)));}
static V3 WorldToModel6692(const ModelWorld6692& x,V3 p) noexcept{return Mul(QRotateV(QConj(x.q),Sub(p,x.t)),1.0f/x.scale);}

static bool ConstrainHandWorld6692(void* character,V3 previousAllowed,V3 requested,V3& allowed,float& hitDistance) noexcept {
    allowed=requested;hitDistance=0;ModelWorld6692 x{};if(!ReadModelWorld6692(character,x)||!CollisionBinding6692(WhBase()))return false;
    const V3 a=ModelToWorld6692(x,previousAllowed),b=ModelToWorld6692(x,requested),sweep=Sub(b,a);const float n=Len(sweep);
    if(n<.001f)return true;
    const auto h=WorldRayFan6692(a,sweep,.055f);if(!g_collision_fan_valid6693)return false;
    if(!h||*h>=n)return true;
    hitDistance=*h;const float safe=std::max(0.0f,*h-.012f);allowed=WorldToModel6692(x,Add(a,Mul(sweep,safe/n)));return true;
}
static bool SegmentBlockedWorld6692(void* character,V3 aModel,V3 bModel,float radius,float& hitDistance) noexcept {
    g_collision_fan_valid6693=false;hitDistance=0;ModelWorld6692 x{};if(!ReadModelWorld6692(character,x)||!CollisionBinding6692(WhBase()))return false;
    const V3 a=ModelToWorld6692(x,aModel),b=ModelToWorld6692(x,bModel),sweep=Sub(b,a);const float n=Len(sweep);
    if(n<.04f)return false;const auto h=WorldRayFan6692(a,sweep,radius);
    if(!g_collision_fan_valid6693)return false; // no collision claim; circuit/status retain unavailable state
    if(h&&*h<n-.018f){hitDistance=*h;return true;}return false;
}
// The hand proxy follows Henry's actual palm/finger points and controller
// rotation. It is six point sweeps plus bounded finger-plane fitting, NOT a solid convex shape or rigidbody.
#include "vrik/hand_contact_types.inl"
// A continuing surface may genuinely move into the hand. Only its measured
// normal translation (same feature, owner and nearly identical normal) can
// push a spatial discovery contact; accumulated fingertip overlap cannot.
static float SpatialContactPushD81(const HandContact6695& before,const PalmPlane6697& hit) noexcept {
    if(!before.lastCompleteTickH18||g_contact_query_tick_h18<before.lastCompleteTickH18||
       g_contact_query_tick_h18-before.lastCompleteTickH18>80)return 0;
    for(int p=0;p<before.planeCount;++p){const auto& old=before.planes[p];
        const auto& a=old.native172;const auto& b=hit.native172;
        if(old.feature!=hit.feature||a.collider!=b.collider||a.vft!=b.vft||a.owner!=b.owner||
           a.entityId!=b.entityId||a.body!=b.body||a.part!=b.part||a.mask!=b.mask||
           Dot(old.normal,hit.normal)<.9999f)continue;
        return std::max(0.f,Dot(Sub(hit.point,old.point),hit.normal));
    }
    return 0;
}
static V3 ProjectPalmPlanes6697(const HandContact6695& c,V3 wrist,const V3* offsets,float skin,const V3* rawRequestD63=nullptr,bool* bodyLocalSeedD71=nullptr,bool* resolvedD74=nullptr) noexcept {
    if(resolvedD74)*resolvedD74=false;
    if(bodyLocalSeedD71)*bodyLocalSeedD71=false;
    const auto cv=[](V3 v){return kcd2vr::pose_core::Vec3{v.x,v.y,v.z};};
    kcd2vr::contact_projection44::Plane planes[7]{};int count=0;
    for(int p=0;p<c.planeCount&&p<7;++p){
        const auto unit=kcd2vr::pose_core::unit(cv(c.planes[p].normal));if(!unit)continue;
        double offset=std::numeric_limits<double>::infinity();
        for(int i=0;i<c.featureCount;++i)if(c.planes[p].feature<0||c.planes[p].feature==i)
            offset=std::min(offset,kcd2vr::pose_core::dot(cv(offsets[i]),*unit));
        if(!std::isfinite(offset))continue;
        double minimum=double(skin)+kcd2vr::pose_core::dot(cv(c.planes[p].point),*unit)-offset;
        // A wrist-to-tip discovery ray is not a temporal feature sweep. Its
        // hit can already lie inside the accepted hand. Do not depenetrate the
        // entire wrist by that overlap: stop further entry, allow withdrawal
        // and tangent motion. Only a real swept feature owns full push-out.
        // This bounds geometry, not controller speed; no pose filter is added.
        if(c.have&&c.planes[p].spatialProbeD81)
            minimum=std::min(minimum,kcd2vr::pose_core::dot(cv(c.worldAllowed),*unit)+c.planes[p].spatialPushD81);
        planes[count++]={*unit,minimum};
    }
    // D63: re-solving a deep controller request is not contact continuity. A
    // newly measured edge (or one float-ULP shift at world coordinates) can
    // make the previous point just infeasible; the old nearest-to-RAW solve
    // then jumped sideways by the entire stored penetration. Start from the
    // accepted wrist plus actual input displacement and project THAT point
    // against every CURRENT plane. This is geometric proxy motion, not a rate
    // cap: a moving current surface still pushes out its full penetration.
    auto target=cv(wrist);
    bool proxyD63=rawRequestD63&&c.have&&c.rawFeatureReady55&&count>0&&
        c.lastCompleteTickH18&&g_contact_query_tick_h18>=c.lastCompleteTickH18&&
        g_contact_query_tick_h18-c.lastCompleteTickH18<=80;
    bool bodySeedD71=false;
    for(int p=0;p<c.planeCount&&proxyD63;++p){
        const bool body=CurrentBodyPlaneD71(c.planes[p].native172);bodySeedD71|=body;
        proxyD63=npc172::worldOnlyWitnessD62(c.planes[p].native172)||
            npc172::boundedEntitySurfaceD63(c.planes[p].native172)||CurrentRigidPlaneD67R1(c.planes[p].native172)||body;
    }
    if(proxyD63){
        const auto displacement=cv(*rawRequestD63)-cv(c.releaseRawWorldD45);
        if(kcd2vr::pose_core::finite(displacement))target=cv(c.worldAllowed)+displacement;
        else proxyD63=false;
    }
    if(bodyLocalSeedD71)*bodyLocalSeedD71=proxyD63&&bodySeedD71;
    kcd2vr::pose_core::Vec3 result{};
    if(kcd2vr::contact_projection44::project(target,planes,count,result)){
        // Feature rays establish local tangent planes, not an infinite convex
        // solid. Near-opposing tangents can intersect far outside every current
        // sweep and send a slowly moving wrist across the body. Keep their
        // nearest-point solution inside the current feature-motion envelope.
        // Position travel, feature rotation/curl, current normal penetration
        // and the two surface skins are
        // measured in the same world units; there is no frame-time smoothing.
        double featureTravel=0,normalTravel=0;
        for(int i=0;i<count;++i)
            normalTravel=std::max(normalTravel,planes[i].minimum-kcd2vr::pose_core::dot(cv(proxyD63?c.worldAllowed:wrist),planes[i].normal));
        for(int i=0;i<c.featureCount&&i<7;++i)
            featureTravel=std::max(featureTravel,kcd2vr::pose_core::length(cv(offsets[i])-cv(c.offset[i])));
        // D64: accumulated raw penetration is not fresh motion. It must not
        // fund a nonlocal tangent intersection when current intent barely
        // moves. Measure current qualified-proxy penetration from the accepted
        // wrist; unknown/articulated coverage keeps its previous policy.
        const auto previousRaw=proxyD63?cv(c.rawFeatureWorld55[0])-cv(c.offset[0]):cv(c.worldAllowed);
        const double inputTravel=kcd2vr::pose_core::length((proxyD63?cv(*rawRequestD63):cv(wrist))-previousRaw);
        const double envelope=inputTravel+featureTravel+normalTravel+2.0*std::max(0.f,skin);
        if(!c.have||kcd2vr::pose_core::length(result-cv(c.worldAllowed))<=envelope+1e-6){
            // D56: a newly visible corner plane can change the nearest point
            // by centimetres while raw intent moves only millimetres. If the
            // accepted wrist is feasible against EVERY current plane, move
            // along that feasible segment by current intent/shape travel only.
            // This neither retains old planes nor smooths through an obstacle.
            if(!proxyD63&&c.have&&c.rawFeatureReady55&&c.featureCount>0){
                bool priorFeasible=true;
                for(int p=0;p<count;++p)priorFeasible=priorFeasible&&
                    kcd2vr::pose_core::dot(cv(c.worldAllowed),planes[p].normal)+1e-6>=planes[p].minimum;
                const auto previousRaw=cv(c.rawFeatureWorld55[0])-cv(c.offset[0]);
                const double budget=kcd2vr::pose_core::length(cv(wrist)-previousRaw)+featureTravel;
                const auto delta=result-cv(c.worldAllowed);const double distance=kcd2vr::pose_core::length(delta);
                if(priorFeasible&&std::isfinite(budget)&&distance>budget&&distance>1e-9)
                    result=cv(c.worldAllowed)+delta*(budget/distance);
            }
            if(resolvedD74)*resolvedD74=true;
            return V3{float(result.x),float(result.y),float(result.z)};
        }
    }
    // An infeasible or nonlocal tangent intersection does not authorize new
    // travel. The current measured contact remains; a clear query releases it
    // immediately. No old plane or deadline is added or renewed here.
    return c.worldAllowed;
}
// One incomplete-query presentation policy for both mandatory sweeps and
// optional same-pass recovery. It never renews geometry or its deadline.
static bool HoldIncompleteHandContact6695(HandContact6695& c,const ModelWorld6692&,V3 requested,V3& result) noexcept {
    // D80: unavailable coverage is not a physical clamp. The old all-axis80ms
    // pin could ignore withdrawal and tangential controller movement whenever
    // a mandatory query ran out of budget or a corner could not be resolved.
    // Retire its planes, digit fits and correction deadline. A moving door/body
    // cannot lend old geometry to this new request. Keep only the accepted
    // presentation origin for a later fresh sweep; it proves no clearance.
    // Current valid tracking/hand ownership are checked by the caller unchanged.
    // Brief clipping while native coverage is unavailable is intentionally
    // preferable to an invented fixed hand. No query, filter or grace is added.
    const bool station=c.stationPermissiveD75,balanced=c.balancedD79;
    ResetHandGeometryD56(c);c.stationPermissiveD75=station;c.balancedD79=balanced;
    result=requested;return false;
}

// A prior curl can select a connected feasible branch only after THIS digit
// has a new native hit on the exact same owner/body/part. Old geometry never
// participates, and another finger or replacement entity cannot inherit it.
static bool SameFingerSurfaceD72(const HandContact6695& before,const PalmPlane6697& current,int finger) noexcept {
    if(finger<0||finger>=5||!before.fingerContact[finger]||!before.lastCompleteTickH18||
       g_contact_query_tick_h18<before.lastCompleteTickH18||g_contact_query_tick_h18-before.lastCompleteTickH18>80)return false;
    const auto& a=current.native172;
    if(!a.collider||a.part<0)return false;
    for(int p=0;p<before.planeCount;++p){const auto& old=before.planes[p];const auto& b=old.native172;
        if(old.feature==finger+1&&a.collider==b.collider&&a.vft==b.vft&&a.owner==b.owner&&
           a.entityId==b.entityId&&a.body==b.body&&a.part==b.part&&a.mask==b.mask)return true;
    }
    return false;
}
static kcd2vr::pose_core::FingerFit103 FitFingerContactD72(const HandContact6695& before,
    const HandContact6695& current,int finger,const FingerContactGeometry103& fingers,
    const ModelWorld6692& world,V3 wrist,float skin) noexcept {
    namespace fp=kcd2vr::pose_core;fp::FingerSurface103 planes[7]{};unsigned count=0;bool continuing=false;
    for(int p=0;p<current.planeCount&&p<7;++p){const auto& plane=current.planes[p];if(plane.feature!=finger+1)continue;
        planes[count++]={{plane.point.x,plane.point.y,plane.point.z},{plane.normal.x,plane.normal.y,plane.normal.z}};
        continuing|=SameFingerSurfaceD72(before,plane,finger);
    }
    if(!count)return {};
    auto curve=fingers.curve[finger];for(unsigned k=0;k<curve.count;++k)curve.offset[k]=curve.offset[k]*world.scale;
    const Q4 rotation=QMul(world.q,fingers.orientation);
    return fp::fit_finger_surfaces_d72(curve,fingers.input[finger],continuing?before.hardCurlH31[finger]:-1.,
        {rotation.x,rotation.y,rotation.z,rotation.w},{wrist.x,wrist.y,wrist.z},planes,count,skin);
}
static bool CompleteHandContact6695(HandContact6695& c,HandContactPending6695& pending,V3& result) noexcept {
    kcd2vr::diagnostics::pose_cost_d60::Scope costD60(kcd2vr::diagnostics::pose_cost_d60::ContactRecovery);
    if(!pending.ready)return false;
    HandRigidContactScopeD67R1 rigidReceiptD67R1;
    pending.ready=false;
    kcd2vr::touch_react_service60::ScopedNpcQueryH9 npcHandH9(g_npc_query172,diagnostics::lab::NativeReference());
    const auto& before=pending.before;auto& queried=pending.queried;
    const ModelWorld6692 x{pending.worldRotation,pending.worldTranslation,pending.worldScale};const auto desired=pending.desired;
    auto* offsets=pending.offsets;const auto* fingers=pending.fingers;const float skin=pending.skin;
    const auto unavailable=[&](){ResetHandContact6695(c);c.epoch=before.epoch;c.character=before.character;return false;};
    const auto incomplete=[&](){
        c=before;c.fresh169r1=false;c.completeClear49=false;
        return HoldIncompleteHandContact6695(c,x,pending.requested,result);
    };
    // An observed plane may suggest another feature is inside its surface, but
    // cannot constrain that other feature. Ask native geometry for that exact
    // point instead. Original temporal feature rays have already run unchanged.
    bool tested[7]{};
    // Full original plane sets keep their current solved result. Optional
    // recovery cannot invalidate them merely because no extra slot is available.
    if(g_npc_query172.requested&&!before.stationPermissiveD75&&!before.balancedD79)for(int extra=0;extra<queried.featureCount&&queried.planeCount<7;++extra){
        const V3 proposal=queried.planeCount?ProjectPalmPlanes6697(queried,desired,offsets,skin,&pending.rawDesiredD45):desired;
        int feature=-1;float worst=-.000001f;
        if(!queried.planeCount&&before.blocked&&!queried.blocked&&before.rawFeatureReady55&&!tested[0])feature=0;
        // D63: losing the palm ray does not clear a previously contacting
        // fingertip. At the recorded alchemy bottle, the palm was outside but
        // the index was already107mm beyond its surface before contact returned.
        // Requalify each LOST exact feature from the accepted wrist. Old planes
        // only nominate a ray; neither their geometry nor owner becomes a new
        // contact unless the current native query supplies it. A moving BASE
        // surface can overtake a tip without ever reaching the palm centre.
        if(feature<0&&before.blocked&&before.rawFeatureReady55&&
           before.lastCompleteTickH18&&g_contact_query_tick_h18>=before.lastCompleteTickH18&&
           g_contact_query_tick_h18-before.lastCompleteTickH18<=80){
            for(int p=0;p<before.planeCount&&feature<0;++p){const auto& old=before.planes[p];
                if(old.feature<0||old.feature>=queried.featureCount||tested[old.feature]||
                   (!npc172::worldOnlyWitnessD62(old.native172)&&!npc172::boundedEntitySurfaceD63(old.native172)&&
                    !npc172::rigidSurfaceReceiptD67R1(old.native172)&&
                    !(kcd2vr::contact_handover_d71::ContactContinuity()&&!diagnostics::lab::NativeReference()&&
                      npc172::bodySurfaceReceiptD71(old.native172))))continue;
                bool current=false;for(int q=0;q<queried.planeCount;++q)
                    current|=queried.planes[q].feature==old.feature;
                if(!current)feature=old.feature;
            }
        }
        if(feature<0)for(int p=0;p<queried.planeCount;++p)for(int i=0;i<queried.featureCount;++i){
            if(tested[i]||queried.planes[p].feature<0||queried.planes[p].feature==i)continue;
            const float gap=Dot(queried.planes[p].normal,Sub(Add(proposal,offsets[i]),queried.planes[p].point))-skin;
            if(gap<worst){worst=gap;feature=i;}
        }
        if(feature<0)break;
        tested[feature]=true;
        const V3 sweep=Sub(Add(proposal,offsets[feature]),before.worldAllowed);
        const float length=Len(sweep);
        if(!std::isfinite(length)||length>2.f)return unavailable();
        if(length<.0001f)continue;
        const auto h=WorldRayDistance6692(before.worldAllowed,sweep);
        if(h.state==wc::Result::Busy){
            // D81: all mandatory feature sweeps already completed. Optional
            // recovery may add a current surface, but budget loss here must not
            // throw away the current measured planes and jump back through them.
            // With no current plane there is still no proven stopping surface.
            if(queried.planeCount>0)break;
            return incomplete();
        }
        if(h.state!=wc::Result::Hit&&h.state!=wc::Result::Clear)return unavailable();
        if(h.state==wc::Result::Clear)continue;
        if(!h.plane)return incomplete();
        queried.planes[queried.planeCount++]={h.point,h.normal,feature,h.witness169r1,h.native172,true};
        queried.planes[queried.planeCount-1].spatialPushD81=SpatialContactPushD81(before,queried.planes[queried.planeCount-1]);
        queried.blocked=true;
        if(fingers&&feature>0&&feature<=5&&fingers->enabled[feature-1]){
            const auto fit=FitFingerContactD72(before,queried,feature-1,*fingers,x,proposal,skin);
            if(fit.valid&&fit.fits){
                queried.curl[feature-1]=fit.curl;queried.fingerContact[feature-1]=true;
                offsets[feature]=Sub({float(fit.tip.x),float(fit.tip.y),float(fit.tip.z)},proposal);
            }
        }
    }
    V3 allowed=Add(before.worldAllowed,Mul(Sub(desired,before.worldAllowed),pending.fallbackFraction));
    // A current hit without a valid plane owns fallbackFraction. Its limited
    // endpoint must not be replaced by the proxy's full raw displacement.
    // Other current planes still project it; ordinary qualified contact keeps
    // the D63 continuity path and all feature queries unchanged.
    if(queried.planeCount){
        // Balanced contact cannot turn a current articulated back-face hit into
        // a remote wrist push. It remains UNKNOWN with the original bounded
        // incomplete-query policy; native front-face qualification is unchanged.
        if(before.balancedD79)for(int p=0;p<queried.planeCount;++p)
            if(npc172::bodySurfaceIdentityD71(queried.planes[p].native172)&&
               !CurrentBodyPlaneD71(queried.planes[p].native172))return incomplete();
        bool resolvedD74=false;
        allowed=ProjectPalmPlanes6697(queried,allowed,offsets,skin,
            pending.unqualifiedHitD62?nullptr:&pending.rawDesiredD45,&queried.bodyLocalSeedD71,&resolvedD74);
        // D74: the old wrist is a conservative fallback, NOT a newly solved
        // current endpoint. An infeasible/nonlocal corner must not renew the
        // completed-query stamp forever. The D80 incomplete-result policy
        // releases geometry to the current wrist; it does not pin an old contact.
        if(!resolvedD74)return incomplete();
    }
    if(!std::isfinite(Len(allowed))||Len(Sub(allowed,desired))>2.f)return incomplete();
    // Intermediate palm/corner projection can move the wrist after an earlier
    // digit fit. Refit every measured digit at the final wrist against only its
    // OWN current surfaces. This prevents a curl from the abandoned wrist pose
    // being published, and never applies one finger's plane to its neighbours.
    if(fingers)for(int finger=0;finger<5;++finger)if(fingers->enabled[finger]){
        // Intermediate fits belong to an earlier wrist. Retire them before
        // testing the final wrist; a failed refit must not publish that curl.
        queried.fingerContact[finger]=false;
        queried.curl[finger]=fingers->input[finger];
        offsets[finger+1]=Sub(pending.rawFeatures[finger+1],pending.rawDesiredD45);
        const auto fit=FitFingerContactD72(before,queried,finger,*fingers,x,allowed,skin);
        if(fit.valid&&fit.fits){queried.curl[finger]=fit.curl;queried.fingerContact[finger]=true;
            offsets[finger+1]=Sub({float(fit.tip.x),float(fit.tip.y),float(fit.tip.z)},allowed);}
    }
    queried.worldAllowed=allowed;queried.fresh169r1=true;queried.completeClear49=!queried.blocked;queried.glideTickR8b=0; // R11K: a measured point ends any glide
    queried.stationPresentationLimitedD81=false;
    queried.lastCompleteTickH18=g_contact_query_tick_h18;
    queried.completeSampleD80=g_contact_query_sample_d75;queried.completeTrackingD80=g_contact_tracking_d53;
    queried.stationSampleD75=queried.stationPermissiveD75?g_contact_query_sample_d75:0;
    queried.stationTrackingD75=queried.stationPermissiveD75?g_contact_tracking_d53:0;
    queried.releaseRawWorldD45=pending.rawDesiredD45;
    queried.releaseCorrectionWorldD45=Sub(allowed,pending.rawDesiredD45);
    queried.releaseTickD45=g_contact_query_tick_h18;queried.releaseNsD79=g_contact_query_ns_d79;
    queried.releaseActiveD45=!queried.stationPermissiveD75&&(before.releaseActiveD45||queried.blocked)&&
        Len(queried.releaseCorrectionWorldD45)>kcd2vr::contact_release_d45::kDoneM*x.scale;
    if(queried.blocked){
        queried.releaseWorldD62=queried.planeCount>0&&!pending.unqualifiedHitD62;
        for(int p=0;p<queried.planeCount;++p)
            queried.releaseWorldD62=queried.releaseWorldD62&&npc172::worldOnlyWitnessD62(queried.planes[p].native172);
    }else queried.releaseWorldD62=before.releaseWorldD62;
    if(queried.blocked){
        queried.releaseEntitySurfaceD63=queried.planeCount>0&&!pending.unqualifiedHitD62;
        for(int p=0;p<queried.planeCount;++p)
            queried.releaseEntitySurfaceD63=queried.releaseEntitySurfaceD63&&
                (npc172::boundedEntitySurfaceD63(queried.planes[p].native172)||
                 npc172::worldOnlyWitnessD62(queried.planes[p].native172)||CurrentRigidPlaneD67R1(queried.planes[p].native172));
    }else queried.releaseEntitySurfaceD63=before.releaseEntitySurfaceD63;
    if(queried.blocked){
        queried.releaseBodySurfaceD71=kcd2vr::contact_handover_d71::ContactContinuity()&&
            !diagnostics::lab::NativeReference()&&queried.planeCount>0&&!pending.unqualifiedHitD62;
        for(int p=0;p<queried.planeCount;++p)
            queried.releaseBodySurfaceD71=queried.releaseBodySurfaceD71&&
                (npc172::worldOnlyWitnessD62(queried.planes[p].native172)||
                 npc172::boundedEntitySurfaceD63(queried.planes[p].native172)||
                 CurrentRigidPlaneD67R1(queried.planes[p].native172)||CurrentBodyPlaneD71(queried.planes[p].native172));
    }else queried.releaseBodySurfaceD71=before.releaseBodySurfaceD71;
    if(!queried.releaseActiveD45){queried.releaseWorldD62=false;queried.releaseEntitySurfaceD63=false;queried.releaseBodySurfaceD71=false;}
    queried.springMeasuredH31=fingers?fingers->springSourceH31:kcd2vr::finger_contact_h31::Source{};
    for(int finger=0;finger<5;++finger)queried.hardCurlH31[finger]=queried.curl[finger];
    const bool unchangedClearD64=!queried.blocked&&allowed.x==pending.rawDesiredD45.x&&
        allowed.y==pending.rawDesiredD45.y&&allowed.z==pending.rawDesiredD45.z;
    const V3 finalModelD64=unchangedClearD64?pending.requested:WorldToModel6692(x,allowed);
    queried.clearCorrection49=Sub(finalModelD64,pending.requested);
    std::copy_n(offsets,queried.featureCount,queried.offset);
    std::copy_n(pending.rawFeatures,queried.featureCount,queried.rawFeatureWorld55);queried.rawFeatureReady55=true;c=queried;
    result=finalModelD64;return true;
}
static bool ContactHand6695(HandContact6695& c,void* character,std::uint64_t epoch,
                            V3 nativeHand,V3 requested,V3& result,const V3* palmOffsets=nullptr,const FingerContactGeometry103* fingers=nullptr,HandContactPending6695* deferred=nullptr,bool stationPermissiveD75=false,bool balancedD79=false) noexcept {
    kcd2vr::diagnostics::pose_cost_d60::Scope costD60(kcd2vr::diagnostics::pose_cost_d60::ContactSweep);
    HandRigidContactScopeD67R1 rigidReceiptD67R1;
    kcd2vr::touch_react_service60::ScopedNpcQueryH9 npcHandH9(g_npc_query172,diagnostics::lab::NativeReference());
    c.fresh169r1=false;
    const auto unavailableH18=[&](){ResetHandContact6695(c);c.epoch=epoch;c.character=character;return false;};
    result=requested;ModelWorld6692 x{};if(!ReadModelWorld6692(character,x))return unavailableH18();
    if(c.epoch!=epoch||c.character!=character){ResetHandContact6695(c);c.epoch=epoch;c.character=character;}
    if(c.stationPermissiveD75!=stationPermissiveD75){
        ResetHandGeometryD56(c);c.stationPermissiveD75=stationPermissiveD75;
    }
    balancedD79=balancedD79&&!stationPermissiveD75;
    if(c.balancedD79!=balancedD79){
        const V3 accepted=c.worldAllowed;const bool have=c.have;
        ResetHandGeometryD56(c);c.worldAllowed=accepted;c.have=have;
    }
    c.balancedD79=balancedD79;
    // The caller supplies the existing five measured palm points. A station
    // never imports precise fingertip fits/springs into this separate layout.
    if(stationPermissiveD75||balancedD79)fingers=nullptr;
    const auto holdIncompleteH18=[&](){return HoldIncompleteHandContact6695(c,x,requested,result);};
    const auto knuckle=fingers?kcd2vr::knuckle174::offset(fingers->curve,fingers->input,
        {fingers->orientation.x,fingers->orientation.y,fingers->orientation.z,fingers->orientation.w}):std::nullopt;
    const int count=balancedD79?int(kcd2vr::contact_balanced_d79::kFeatures):fingers?(knuckle?7:6):5;
    const int previousCount=c.featureCount;
    if(previousCount!=count){
        // D72: adding/removing the optional knuckle probe does not replace the
        // palm or five digit identities. Preserve their nomination/history for
        // current requery; only the removed feature loses its old record.
        if(previousCount>=6&&count>=6){int kept=0;
            for(int p=0;p<c.planeCount;++p)if(c.planes[p].feature<count)c.planes[kept++]=c.planes[p];
            c.planeCount=kept;
        }else {
            // D74: coarse palm points and precise digits are different feature
            // layouts, even at the same array index. Keep only the accepted
            // wrist as a sweep origin; retire planes, raw feature history and
            // digit springs before assigning the new layout's offsets below.
            const V3 accepted=c.worldAllowed;const bool have=c.have;
            ResetHandGeometryD56(c);c.worldAllowed=accepted;c.have=have;
        }
    }
    // Layout retirement also clears policy metadata; restore this current call's
    // selected owner after every reset, before copying pending receipts.
    c.stationPermissiveD75=stationPermissiveD75;c.balancedD79=balancedD79;
    c.stationWorldScaleD75=stationPermissiveD75?x.scale:0;
    c.featureCount=count;
    V3 offsets[7]{};if(palmOffsets)for(int i=0;i<(balancedD79?7:fingers?6:5);++i)offsets[i]=QRotateV(x.q,Mul(palmOffsets[i],x.scale));
    if(knuckle)offsets[6]=QRotateV(x.q,Mul({float(knuckle->x),float(knuckle->y),float(knuckle->z)},x.scale));
    if(c.have&&previousCount!=count&&(previousCount<6||count<6))std::copy_n(offsets,count,c.offset);
    else if(c.have&&count>previousCount)for(int i=previousCount;i<count;++i)c.offset[i]=offsets[i];
    if(!c.have){
        const bool displayedD53=c.presentationTickD53&&g_contact_query_tick_h18>=c.presentationTickD53&&
            g_contact_query_tick_h18-c.presentationTickD53<=80&&c.presentationTrackingD53==g_contact_tracking_d53&&
            std::isfinite(Len(c.presentationWorldD53));
        // Station entry without an accepted displayed wrist has no sweep origin.
        // Follow this controller now; ContactCommit records presentation only.
        if(stationPermissiveD75&&!displayedD53)return unavailableH18();
        c.worldAllowed=displayedD53?c.presentationWorldD53:ModelToWorld6692(x,nativeHand);
        std::copy_n(offsets,count,c.offset);c.have=true;
    }
    if(!g_collision_circuit6693.enabled())return unavailableH18();
    if(!g_contact_latency59c1.ready(ContactMicros6695()))return holdIncompleteH18();
    const V3 rawDesiredD45=ModelToWorld6692(x,requested);const float travel=Len(Sub(rawDesiredD45,c.worldAllowed));
    if(!std::isfinite(travel)||travel>2.f)return unavailableH18();
    V3 rawFeatures55[7]{};for(int i=0;i<count;++i)rawFeatures55[i]=Add(rawDesiredD45,offsets[i]);
    // INVESTIGATE_D67R1: this8mm is positive surface clearance for measured
    // feature points, not a ray radius or collider/render-mesh error bound.
    // A2mm permitted penetration policy needs the actual proxy-to-visible-tip
    // gap for each finger and tool. Do not implement it by shortening temporal
    // travel rays, negative skin, or treating every nearby station tool clear.
    const float skin=(balancedD79?kcd2vr::contact_balanced_d79::kSkinM:kcd2vr::station_hand_contact_d75::skin(stationPermissiveD75))*x.scale;
    // D45: the first CLEAR after a shelf edge used to snap the entire stored
    // contact offset away. Sweep a bounded release candidate instead. These are
    // still the same mandatory feature rays, and no stale plane constrains it.
    // Moving-body coverage uses raw intent to discover an overtaken feature.
    // Keep that independently tested path unchanged; this repair is static.
    // D63: a completed current feature query owns the contact correction,
    // including entity-backed tables and tools. Their CPhysicalEntity foreign
    // owner is not permission to snap the wrist back by the full correction.
    // This is a candidate, never retained geometry: ALL mandatory feature rays
    // still test it below. Raw feature intent remains separate for stationary
    // and overtaken moving-body coverage; no body is reclassified as static.
    const V3 desired=!stationPermissiveD75&&g_contact_pass6695&&(!g_npc_query172.requested||c.releaseWorldD62||c.releaseEntitySurfaceD63||
        (kcd2vr::contact_handover_d71::ContactContinuity()&&!diagnostics::lab::NativeReference()&&c.releaseBodySurfaceD71))&&c.releaseActiveD45?
        kcd2vr::contact_release_d45::candidate(rawDesiredD45,c.releaseRawWorldD45,c.releaseCorrectionWorldD45,
            x.scale,g_contact_query_tick_h18,c.releaseTickD45,g_contact_query_ns_d79,c.releaseNsD79):rawDesiredD45;
    if(!g_contact_pass6695){
        // Repeated eye/skinning callbacks of ONE completed publication retain
        // its already measured result and spend no additional budget. A newer
        // publication with no coverage must not borrow that old fixed endpoint.
        bool sameCompletedD80=c.have&&c.rawFeatureReady55&&previousCount==count&&
            c.completeSampleD80&&c.completeTrackingD80&&
            c.completeSampleD80==g_contact_query_sample_d75&&
            c.completeTrackingD80==g_contact_tracking_d53&&c.lastCompleteTickH18&&
            g_contact_query_tick_h18>=c.lastCompleteTickH18&&
            g_contact_query_tick_h18-c.lastCompleteTickH18<=80;
        // The same XR ID can be observed after a changed model/root basis or
        // fingertip orientation. Reuse requires the SAME world-space request
        // for every feature, not merely the sequence number.
        for(int i=0;i<count&&sameCompletedD80;++i)
            sameCompletedD80=Len(Sub(rawFeatures55[i],c.rawFeatureWorld55[i]))<=.000001f;
        if(!sameCompletedD80)return holdIncompleteH18();
        if(c.planeCount)result=WorldToModel6692(x,c.worldAllowed);
        else if(c.completeClear49&&!c.blocked)result=Add(requested,c.clearCorrection49);
        else result=WorldToModel6692(x,c.worldAllowed);
        return true;
    }

    HandContact6695 queried=c;queried.planeCount=0;queried.blocked=false;
    queried.bodyLocalSeedD71=false;queried.bodyPartRequeryMaskD71=0;
    for(int finger=0;finger<5;++finger)queried.fingerContact[finger]=false;
    bool busy=false,unqualifiedHitD62=false;float fallbackFraction=1.f;
    for(int i=0;i<count;++i){
        // C55: every native ray returns only its FIRST surface. Once this
        // pass has freshly measured a wall, cast the remaining features toward
        // the nearest wrist already feasible against that measured wall. If all
        // rays aim through the original blocked request, the first wall hides
        // the second side of a corner and the hand can tunnel around it.
        // This uses only this pass's new planes, never an old-world plane grace,
        // and spends the same bounded feature-ray count.
        const V3 queryDesired=queried.planeCount?ProjectPalmPlanes6697(queried,desired,offsets,skin,&rawDesiredD45):desired;
        bool spatialProbeD81=false;
        V3 start=Add(c.worldAllowed,c.offset[i]);const V3 finish=Add(queryDesired,offsets[i]);V3 motion=Sub(finish,start);float length=Len(motion);
        if(!std::isfinite(length)||length>2.f)return unavailableH18();
        // In the body-contact experiment an NPC can enter a stationary hand.
        // Spend this feature's existing ray on the wrist-to-feature segment when
        // there is no temporal sweep. This is bounded surface coverage, not an
        // overlap/inside-body test or a new rigid physics shape.
        // Test stationarity against the real unprojected feature intent. A
        // moving body can already contain the old feature; the projected
        // escape path must not replace this wrist-to-feature coverage with a
        // backward ray from INSIDE that body (opposite face/normal).
        const bool stationaryFeature55=c.rawFeatureReady55&&i<std::min(previousCount,count)&&
            Len(Sub(rawFeatures55[i],c.rawFeatureWorld55[i]))<.0001f;
        // A freshly measured moving palm surface can overtake the previous
        // fingertip even while the controller moves tangentially. Its temporal
        // ray would then start inside that body and report the opposite face.
        // Use the current outside wrist for this feature's existing ray only
        // when the same body/part contact continues and no different previous
        // surface owns this feature. No previous plane is kept as a constraint.
        bool overtakenFeature55=false;
        if(i>0&&g_npc_query172.requested&&i<std::min(previousCount,count)){
            const auto sameBody55=[](const npc172::Witness& a,const npc172::Witness& b){
                return a.collider&&a.collider==b.collider&&a.vft==b.vft&&a.owner==b.owner&&
                    a.entityId==b.entityId&&a.body==b.body&&a.part==b.part;};
            for(int p=0;p<queried.planeCount&&!overtakenFeature55;++p){const auto& palm=queried.planes[p];
                if(palm.feature!=0||(palm.native172.body!=npc172::Body::Living&&palm.native172.body!=npc172::Body::Articulated&&
                   !npc172::boundedEntitySurfaceD63(palm.native172)&&!CurrentRigidPlaneD67R1(palm.native172))||
                   Dot(palm.normal,Sub(queryDesired,palm.point))<0||Dot(palm.normal,Sub(start,palm.point))>=0)continue;
                bool continuing=false,differentFeature=false,changedPartD71=false;
                for(int old=0;old<c.planeCount;++old){
                    // D71: a fresh front-facing palm query can nominate the
                    // same body's NEW part for a new feature ray. The old part
                    // supplies identity only, never a plane or transform.
                    const bool changed=CurrentBodyPlaneD71(palm.native172)&&
                        npc172::sameBodyIdentityD71(palm.native172,c.planes[old].native172)&&
                        palm.native172.part!=c.planes[old].native172.part;
                    changedPartD71|=changed;
                    const bool same=sameBody55(palm.native172,c.planes[old].native172)||changed;
                    continuing|=same;
                    if((c.planes[old].feature<0||c.planes[old].feature==i)&&!same)differentFeature=true;}
                overtakenFeature55=continuing&&!differentFeature;
                if(overtakenFeature55&&changedPartD71)queried.bodyPartRequeryMaskD71|=1u<<i;
            }
        }
        // A stationary feature can be overtaken by a moving body. Its old
        // feature origin and current raw wrist can BOTH be inside. Use the
        // previously achieved wrist for the palm's existing spatial ray; only
        // the current native hit below can establish a surface or body owner.
        if((stationaryFeature55&&g_npc_query172.requested)||overtakenFeature55){
            spatialProbeD81=true;start=stationaryFeature55&&i==0?c.worldAllowed:queryDesired;
            motion=Sub(finish,start);length=Len(motion);}
        // Balanced mode has no optional recovery rays. A previously measured
        // selected tip can be overtaken while the palm is clear. Nominate that
        // tip's SAME mandatory ray from the last accepted wrist; only the new
        // native hit owns geometry. Old planes supply identity, never a stop.
        if(balancedD79&&i>=5){
            bool tipRequeryD79=false;
            if(c.rawFeatureReady55&&c.lastCompleteTickH18&&g_contact_query_tick_h18>=c.lastCompleteTickH18&&
               g_contact_query_tick_h18-c.lastCompleteTickH18<=80)
                for(int p=0;p<c.planeCount&&!tipRequeryD79;++p){const auto& old=c.planes[p];
                    tipRequeryD79=old.feature==i&&(npc172::worldOnlyWitnessD62(old.native172)||
                        npc172::boundedEntitySurfaceD63(old.native172)||npc172::rigidSurfaceReceiptD67R1(old.native172)||
                        npc172::bodySurfaceReceiptD71(old.native172));}
            // Another selected tip can obtain the first current surface while
            // this tip's old origin is already behind it. The current plane
            // only nominates a new ray from a demonstrably outside wrist; it
            // is never applied to this other feature without its own hit.
            for(int p=0;p<queried.planeCount&&!tipRequeryD79;++p){const auto& current=queried.planes[p];
                const bool qualified=npc172::worldOnlyWitnessD62(current.native172)||
                    npc172::boundedEntitySurfaceD63(current.native172)||CurrentRigidPlaneD67R1(current.native172)||
                    CurrentBodyPlaneD71(current.native172);
                tipRequeryD79=qualified&&Dot(current.normal,Sub(c.worldAllowed,current.point))>skin&&
                    Dot(current.normal,Sub(start,current.point))<0;
            }
            if(tipRequeryD79){spatialProbeD81=true;start=c.worldAllowed;motion=Sub(finish,start);length=Len(motion);}
        }
        // A spatial probe may leave a tip inside a newly encountered part.
        // Sweeping OUT from that tip sees a back face which the adapter orients
        // toward its origin. Requery from the outside accepted wrist instead:
        // old geometry nominates the ray, only its CURRENT result owns contact.
        for(int p=0;p<c.planeCount&&!spatialProbeD81;++p){const auto& old=c.planes[p];
            if(!old.spatialProbeD81||old.feature!=i||!c.lastCompleteTickH18||
               g_contact_query_tick_h18<c.lastCompleteTickH18||g_contact_query_tick_h18-c.lastCompleteTickH18>80)continue;
            if(Dot(old.normal,Sub(start,old.point))<skin&&
               Dot(old.normal,Sub(finish,old.point))>=skin&&
               Dot(old.normal,Sub(c.worldAllowed,old.point))>=skin){
                spatialProbeD81=true;start=c.worldAllowed;motion=Sub(finish,start);length=Len(motion);
            }
        }
        if(length<.0001f)continue;
        // The same ray includes positive clearance while a measured contact
        // returns to the controller. No minimum16mm positional step, extra ray,
        // old-plane constraint or free-motion filter is needed.
        const auto queryMotionD79=kcd2vr::contact_balanced_d79::clearanceSweep(motion,skin,
            !stationPermissiveD75&&c.releaseActiveD45);
        const auto h=WorldRayDistance6692(start,queryMotionD79);
        if(h.state==wc::Result::Busy){busy=true;break;}
        if(h.state!=wc::Result::Hit&&h.state!=wc::Result::Clear)return unavailableH18();
        if(h.state==wc::Result::Hit){
            queried.blocked=true;
            if(h.plane&&queried.planeCount<7){
                queried.planes[queried.planeCount++]={h.point,h.normal,i,h.witness169r1,h.native172,spatialProbeD81};
                if(spatialProbeD81)queried.planes[queried.planeCount-1].spatialPushD81=SpatialContactPushD81(c,queried.planes[queried.planeCount-1]);
                // Fit at the same current wrist used by this feature ray.
                // The deep raw request can be inside the palm surface and
                // falsely make every curl infeasible, pushing the whole arm.
                if(fingers&&i>0&&i<=5&&fingers->enabled[i-1]){
                    const auto fit=FitFingerContactD72(c,queried,i-1,*fingers,x,queryDesired,skin);
                    if(fit.valid&&fit.fits){
                        queried.curl[i-1]=fit.curl;queried.fingerContact[i-1]=true;
                        offsets[i]=Sub({float(fit.tip.x),float(fit.tip.y),float(fit.tip.z)},queryDesired);
                    }
                }
            }
            else {unqualifiedHitD62=true;
                fallbackFraction=std::min(fallbackFraction,std::clamp((h.distance-skin)/length,0.f,1.f));}
        }
    }
    if(busy){
        // A positive hit in an incomplete pass revokes prior clearance across
        // later callbacks too; a merely busy clear hand keeps its last classification.
        c.completeClear49=c.completeClear49&&!queried.blocked;
        // An incomplete mandatory set is not a clear scene. D80 retires its
        // geometric authority and follows the current controller immediately;
        // it neither pins the hand nor claims an unchecked obstacle is clear.
        return holdIncompleteH18();
    }
    HandContactPending6695 pending{};pending.before=c;pending.queried=queried;pending.unqualifiedHitD62=unqualifiedHitD62;
    pending.worldRotation=x.q;pending.worldTranslation=x.t;pending.worldScale=x.scale;
    pending.desired=desired;pending.rawDesiredD45=rawDesiredD45;pending.requested=requested;pending.skin=skin;pending.fallbackFraction=fallbackFraction;pending.fingers=fingers;pending.ready=true;
    std::copy_n(offsets,count,pending.offsets);std::copy_n(rawFeatures55,count,pending.rawFeatures);
    if(deferred){*deferred=pending;result=WorldToModel6692(x,c.worldAllowed);return true;}
    return CompleteHandContact6695(c,pending,result);
}
static void ContactCommit6695(HandContact6695& c,void* character,V3 solved,std::uint64_t presentationTickD53=0) noexcept {
    ModelWorld6692 x{};if(c.character==character&&((c.have&&c.fresh169r1)||presentationTickD53)&&ReadModelWorld6692(character,x)){
        const auto world=ModelToWorld6692(x,solved);
        if(c.have&&c.fresh169r1&&!c.stationPresentationLimitedD81){
            c.clearCorrection49=Add(c.clearCorrection49,Sub(solved,WorldToModel6692(x,c.worldAllowed)));
            c.worldAllowed=world;
        }
        if(presentationTickD53&&std::isfinite(Len(world))){
            c.presentationWorldD53=world;c.presentationTickD53=presentationTickD53;c.presentationTrackingD53=g_contact_tracking_d53;
        }
    }
}

static void CollisionStatus6692(std::uint64_t now,const char*) noexcept {
    auto prev=g_collision_last_status6692.load(std::memory_order_relaxed);if(now-prev<2000||!g_collision_last_status6692.compare_exchange_strong(prev,now,std::memory_order_acq_rel))return;
    const int reason=g_collision_circuit6693.reason(),live=g_collision_live6693.load();
    const char* status=reason==1?"BLOCKED_NATIVE_FAULT":reason==2?"BLOCKED_INVALID_TYPE_OR_RESULT":
        g_contact_latency59c1.pending()?"TEMPORARY_SLOW_QUERY_BACKOFF":
        g_collision_binding6692.load()<0?"BLOCKED_CONTRACT":live==1?"QUERY_RETURNED_CONTACT_UNPROVEN":"WAITING_VALID_WORLD";
    char m[512]{};std::snprintf(m,sizeof(m),
        "V66.92R1-COLLISION STATUS state=%s contract=%d queries=%llu hits=%llu faults=%llu collisionGuaranteed=0 scope=STATIC+TERRAIN failureFallback=HANDLING_ONLY rearm=%s slowEvents=%llu",
        status,g_collision_binding6692.load(),(unsigned long long)g_collision_circuit6693.calls(),
        (unsigned long long)g_collision_hits6692.load(),(unsigned long long)g_collision_circuit6693.faults(),
        reason?"PROCESS_RESTART_ONLY":"AUTOMATIC_VALIDATED_QUERY",
        (unsigned long long)g_contact_latency59c1.events());diagnostic_trace::Write(m);
    char budget[320]{};std::snprintf(budget,sizeof(budget),"V66.97-CONTACT BUDGET raysPerPass=18 freshSharedTrackingSample=1 queryWorkTargetUs=1000 excludesOwnerAndBodyWork=1 maxQueryUs=%llu budgetSkips=%llu singleCallStopUs=8000 behavior=PALM_PLUS_FIVE_FINGERS_CACHED_PLANES approximation=SPARSE_POINTS_AND_PLANE_FIT_NOT_SOLID_VOLUME",
        (unsigned long long)g_contact_max_us6695.load(),(unsigned long long)g_contact_budget_skips6695.load());diagnostic_trace::Write(budget);
}
// R155: the instant-held pickup item (staged_pickup596c2.inl, included below inside the private namespace) uses the
// same R103 "do not collide with Henry" lease as the scenery props. self_collision_native_r103.inl is included
// after that namespace (it needs PropBody596 and the gore part helpers), so its two entry points are declared here.
namespace self_collision_r103n {
static void Exclude(unsigned owner,kcd2vr::self_collision_r103::Kind kind,std::uint32_t id,std::uintptr_t entity,std::uintptr_t body,int part,std::uint64_t now) noexcept;
static void Release(unsigned owner,std::uint64_t now) noexcept;
}

namespace {
using CombatInputFn101=void(__fastcall*)(void*,const std::uint64_t*,int,float);
static std::atomic<CombatInputFn101> g_combat_bypass101{nullptr};
static std::atomic<std::uintptr_t> g_combat_contract101{0},g_combat_hook_base101{0};
static std::array<unsigned char,16> g_combat_own_patch101{};
static bool OwnCombatInputHook101(std::uintptr_t base) noexcept {
    if(!base||g_combat_hook_base101.load(std::memory_order_acquire)!=base||!g_combat_bypass101.load())return false;
    unsigned char live[16]{};return Read(reinterpret_cast<void*>(base+0x8E5324),live)&&!std::memcmp(live,g_combat_own_patch101.data(),16);
}
static bool NativeArm104() noexcept;
static bool NativeSubmit104(void*,kcd2vr::native_combat104::Gesture,int,std::uint64_t,kcd2vr::pose_core::Vec3) noexcept;
static void NativeRelease104(const char*) noexcept;
static void NativeTimingCancel178r22c2() noexcept;
static void NativeTimingCancelPending59c3() noexcept;
static void NativeTimingFist178r22c2(void*,std::uint64_t,std::uint64_t,int,double) noexcept;
static void ServiceAttackTiming178r22c2() noexcept;
static void AdmissionSummary51() noexcept;
struct PhysFrame6698;
static void TracePhysicalItem28(const PhysFrame6698&,int,void*,void*,void*,std::uint32_t,std::uint64_t,const ModelWorld6692&,const float*,bool,bool) noexcept;
static void TraceToolItem28(const CommittedFrame6695&,int,void*,void*,const ModelWorld6692&,const float*,const kcd2vr::grasp13::Binding*) noexcept;
static bool HandClaimBlockedT24(int,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept;
static bool DrawNativeActiveD77() noexcept;
static void DrawObserveExistingD77(std::uint64_t) noexcept;
static void DrawTokenSubmittedD77(DetourModKit::hook::MidContext&) noexcept;
static void DrawInvokeWorldOriginalD77(void*,void(__fastcall*)(void*));
#include "vrik/physical_native.inl"
#include "subsystems/throw_drop/initial_draw_callback_h36r3.inl"
#include "diagnostics/draw_native_d77.inl"
#include "vrik/alchemy_tool18.inl"
#include "vrik/smith_tool36.inl"
#define KCD2VR_CRAFT36B 1
// C37I1 discovery, F5 recorder only (Domain::Interaction lease; nothing written or changed).
// Henry's entity_right/entity_left hold alchemy props and the sword workpiece;
// hand_right/hand_left hold smithing inventory tools (table at 0x40AC298).
// Bounded edges and heartbeats report each relevant attachment, its local hand
// helpers, native smith phase and exact tool-membership refusal.
static void ReadCraftHelpers15(void* entity,QuatT28& h0,QuatT28& h1,bool& ok0,bool& ok1) noexcept {
    if(!entity||!g_attachment_helper_transform_original)return;
    __try{ok0=g_attachment_helper_transform_original(entity,"slt_0",&h0,true);ok1=g_attachment_helper_transform_original(entity,"slt_1",&h1,true);}
    __except(EXCEPTION_EXECUTE_HANDLER){ok0=ok1=false;}
}
static void CraftAttachmentReceipt36b(void* self,void* attachment,std::uint64_t now) noexcept {
    using namespace kcd2vr::activity_input163;
    const bool smith=witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire)!=0;
    const bool alch=witness175.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire)!=0;
    if(!self||!attachment||!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return;
    void* am=nullptr;const void* henryAm=g_henry_char_attachment_manager.load(std::memory_order_acquire);
    if(!henryAm||!PhysVft6698(attachment,0x3A657C0)||!Read(static_cast<unsigned char*>(attachment)+0x28,am)||am!=henryAm)return; // Henry only
    const char* namePtr=nullptr;char name[40]{};
    if(!Read(static_cast<unsigned char*>(attachment)+0x10,namePtr)||!namePtr)return; // IAttachment::GetName 0x638480 = [this+0x10]
    for(unsigned i=0;i+1<sizeof(name);++i){char c=0;if(!Read(namePtr+i,c)||!c)break;name[i]=(c>=32&&c<127)?c:'?';}
    // C37: hammer/tongs are inventory items in hand_right/hand_left (193F148 -> 8CF6FC -> AttachOnly), the
    // sword workpiece is entity_left; each name keeps its own edge slot so other Henry attachments cannot flap it.
    const int slot=std::strcmp(name,"entity_right")==0?0:std::strcmp(name,"entity_left")==0?1:
        std::strcmp(name,"hand_right")==0?2:std::strcmp(name,"hand_left")==0?3:4;
    // Unrelated holster/bow/quiver attachments used to share slot 4 and replace
    // one another on EVERY callback (31,493 irrelevant smithing lines in the
    // supplied run). They cannot identify a workstation tool. Refuse them
    // before helper reads or logging instead of treating that churn as edges.
    if(slot==4)return;
    const int side=slot==0||slot==2?0:slot==1||slot==3?1:2;
    std::uint32_t bound=0;(void)Read(static_cast<unsigned char*>(self)+8,bound);
    const auto life=witness175.lifetime175.load(std::memory_order_acquire);
    const int phase=smith?witness175.SmithPhaseC1():-1;
    // Gate before native helper/holder reads. A bounded heartbeat re-observes
    // an attachment initially seen before native attach completed, and a new
    // F5 session gets its own first sample. No recorder lease means no work.
    static std::mutex receiptMutex;
    std::unique_lock<std::mutex> receiptLock(receiptMutex,std::try_to_lock);
    if(!receiptLock.owns_lock())return; // diagnostic contention never stalls gameplay
    static kcd2vr::diagnostics::interaction_receipt59c10::Gate receipts[4];
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
    if(!receipts[slot].observe(session,now,{reinterpret_cast<std::uintptr_t>(attachment)^life,bound,side,phase},500,768,500))return;
    // Actual entity cache and attachment socket, read only after the current
    // callback's placement path. A missing/stale measurement is not an input veto.
    void* observedCharacter=nullptr;(void)Read(static_cast<unsigned char*>(am)+0x70,observedCharacter);
    void* observedEntity=bound?EntityById6694(bound):nullptr;
    Matrix34V6550 itemWorld{};float socketWorld[8]{};
    const bool itemKnown=observedEntity&&GetEntityWorldTmV6552(observedEntity,bound,itemWorld);
    const bool socketKnown=SupportedWhBuild(WhBase())&&ReadAttachmentQs6689(attachment,socketWorld);
    CommittedFrame6695 packet{};const bool copied=g_committed_frame6695.read(packet);
    const bool poseKnown=copied&&packet.valid[side]&&kcd2vr::pose_core::fresh(packet.stamp,g_pose_epoch6689.load(),reinterpret_cast<std::uintptr_t>(observedCharacter),now);
    char pose[1824]{};std::snprintf(pose,sizeof(pose),
        "C70 F5_HELD_POSE session=%llu tick=%llu kind=%s activityLife=%llu instance=%llX slot=%s side=%d item=%u itemPtr=%p attachment=%p manager=%p character=%p source=POST_ATTACHMENT_CALLBACK itemWorldKnown=%d itemWorld12=(%.9g,%.9g,%.9g,%.9g;%.9g,%.9g,%.9g,%.9g;%.9g,%.9g,%.9g,%.9g) socketKnown=%d socketWorld8=(%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g) wristKnown=%d poseEpoch=%llu poseSample=%llu poseTick=%llu poseAgeMs=%llu nativeWristModel=(%.9g,%.9g,%.9g) allowedWristModel=(%.9g,%.9g,%.9g) modelWorld8=(%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g) actualMeshContact=UNKNOWN originalCallbackRan=UNCLAIMED writes=NONE",
        (unsigned long long)session,(unsigned long long)now,smith?"SMITHING":alch?"ALCHEMY":"INTERACTION",(unsigned long long)life,(unsigned long long)witness175.active[unsigned(smith?kcd2vr::activity_input163::Kind175::Smithing:kcd2vr::activity_input163::Kind175::Alchemy)].load(),name,side,bound,observedEntity,attachment,am,observedCharacter,
        itemKnown,itemWorld.m[0],itemWorld.m[1],itemWorld.m[2],itemWorld.m[3],itemWorld.m[4],itemWorld.m[5],itemWorld.m[6],itemWorld.m[7],itemWorld.m[8],itemWorld.m[9],itemWorld.m[10],itemWorld.m[11],
        socketKnown,socketWorld[0],socketWorld[1],socketWorld[2],socketWorld[3],socketWorld[4],socketWorld[5],socketWorld[6],socketWorld[7],poseKnown,
        (unsigned long long)packet.stamp.epoch,(unsigned long long)packet.stamp.sample,(unsigned long long)packet.stamp.tick,
        (unsigned long long)kcd2vr::diagnostics::interaction_receipt59c10::age(now,packet.stamp.tick),
        packet.native[side].t[0],packet.native[side].t[1],packet.native[side].t[2],packet.allowed[side].t[0],packet.allowed[side].t[1],packet.allowed[side].t[2],
        packet.world[0],packet.world[1],packet.world[2],packet.world[3],packet.world[4],packet.world[5],packet.world[6],packet.world[7]);
    diagnostic_trace::Write(pose);
    if(!smith&&!alch)return;
    kcd2vr::smith_tool36::Binding smith36{};kcd2vr::smith_tool36::ReadEvidenceI1 smithEvidence{};void* character36=nullptr;
    bool smithBound36=false;
    if(smith){
        smithEvidence.boundEntity=bound;
        if(!bound)smithEvidence.refusal="ATTACHMENT_BOUND_ENTITY_MISSING";
        else if(!Read(static_cast<unsigned char*>(am)+0x70,character36)||!character36)
            smithEvidence.refusal="ATTACHMENT_CHARACTER_UNAVAILABLE";
        else smithBound36=ReadSmithTool36(attachment,bound,reinterpret_cast<std::uintptr_t>(character36),smith36,&smithEvidence);
    }
    QuatT28 h0{},h1{};bool ok0=false,ok1=false;void* entity=bound?EntityById6694(bound):nullptr;
    ReadCraftHelpers15(entity,h0,h1,ok0,ok1);
    char rt[96]{};if(entity)Rtti(entity,rt,sizeof(rt));
    char m[1152]{};std::snprintf(m,sizeof(m),
        "C37I1 F5_CRAFT_ATTACH kind=%s smithPhase=%d attachment=%s hand=%s entity=%u rtti=%s slt_0=%d(%.3f,%.3f,%.3f q=%.3f,%.3f,%.3f,%.3f) slt_1=%d(%.3f,%.3f,%.3f) owner=%p smithTool36=%s smithHolder36=%p smithRefusal=%s evidenceHolder=%p holderVft=%p objectVft=%p holderFields=%d holderHand=%d holderAttached=%u linkedEntity=%u boundEntity=%u holderHost=%u playerEntity=%u keysRead=%d holderKey=%016llX handKey=%016llX observed=%llu coalesced=%llu writes=NONE",
        smith?"SMITHING":"ALCHEMY",phase,name,side==0?"R":side==1?"L":"-",bound,rt[0]?rt:"-",
        ok0?1:0,h0.t[0],h0.t[1],h0.t[2],h0.q[0],h0.q[1],h0.q[2],h0.q[3],ok1?1:0,h1.t[0],h1.t[1],h1.t[2],self,
        smithBound36?kcd2vr::smith_tool36::name(smith36.tool):"NONE",reinterpret_cast<void*>(smith36.holder),
        smith?smithEvidence.refusal:"NOT_SMITHING",reinterpret_cast<void*>(smithEvidence.holder),
        reinterpret_cast<void*>(smithEvidence.holderVft),reinterpret_cast<void*>(smithEvidence.objectVft),
        smithEvidence.holderFields,smithEvidence.holderHand,unsigned(smithEvidence.attached),smithEvidence.linkedEntity,
        smithEvidence.boundEntity,smithEvidence.host,smithEvidence.playerEntity,smithEvidence.keysRead,
        (unsigned long long)smithEvidence.holderKey,(unsigned long long)smithEvidence.handKey,
        (unsigned long long)receipts[slot].seen,(unsigned long long)receipts[slot].coalesced);
    diagnostic_trace::Write(m);
}
#include "vrik/tool_attachment59c10.inl"
#include "vrik/alchemy_tool_contact_d53.inl" // D55 opt-in held-alchemy bounds contact
#include "vrik/smith_tool_contact_j27.inl" // J27 hammer/tongs/workpiece world contact
#include "vrik/smith_anvil_contact_a2.inl" // optional learned anvil; J27 remains off-experiment
static void ServiceBodyEvidence122() noexcept;
static void ObserveF11Native167(void*,const char*,int,std::uint64_t,int,pm::Vec3,int,std::uint64_t,bool,bool,int) noexcept;
#include "vrik/passive_contact_service169r1.inl"
static bool FistContactFresh174(int side) noexcept;
static void NativeManualBlockEdge176(void*,std::uint64_t,std::uint64_t) noexcept;
static bool HandClaimBlockedT24(int side,std::uintptr_t character,std::uint64_t body,std::uint64_t request) noexcept {
    // Normal OFF/free path uses only the existing copied claim publication.
    // Busy/partial/unknown observations still require the qualified owner join.
    if(kcd2vr::hand_claim_t24::observe(side,{}).state==kcd2vr::hand_claim_t24::State::Free)return false;
    kcd2vr::hand_claim_t24::Owner owner{};owner.character=character;owner.body=body;owner.request=request;
    void* manager=g_henry_item_attach_manager.load(std::memory_order_acquire);void* player=nullptr;void* back=nullptr;
    if(PhysVft6698(manager,0x3EC9958)&&Read(static_cast<unsigned char*>(manager)+0x38,player)&&PhysVft6698(player,0x3AA4B98)&&Read(static_cast<unsigned char*>(player)+0x9D0,back)&&back==manager&&manager==g_henry_item_attach_manager.load(std::memory_order_acquire)){
        owner.player=reinterpret_cast<std::uintptr_t>(player);owner.manager=reinterpret_cast<std::uintptr_t>(manager);
    }
    return kcd2vr::hand_claim_t24::competing(side,owner);
}
#include "vrik/fist_native.inl"
#include "vrik/native_combat104.inl"
static bool ExternalIdentity176(ExternalPrepare176& out,void* input,void* player,void* manager) noexcept {
    void* exact=nullptr;void* equipment=nullptr;void* currentInput=nullptr;NativeOwner104 owner{};
    if(!FistExactInput101(input,exact,equipment)||exact!=player||equipment!=manager||
       !Read(static_cast<unsigned char*>(player)+0xB88,currentInput)||currentInput!=input||
       !NativeResolve104(player,owner))return false;
    out.input=input;out.player=player;out.manager=manager;out.actor=owner.actor;
    out.state=owner.state;out.entity=owner.entity;out.epoch=owner.epoch;
    for(int side=0;side<2;++side){if(!NativeHandKey6697(manager,side,out.key[side]))return false;
        if(PhysEmpty6698(out.key[side]))out.key[side]=0;}
    return true;
}
static bool ExternalSame176(const ExternalPrepare176& a,const ExternalPrepare176& b) noexcept {
    return a.input==b.input&&a.player==b.player&&a.manager==b.manager&&a.actor==b.actor&&
        a.state==b.state&&a.entity==b.entity&&a.epoch==b.epoch&&a.key[0]==b.key[0]&&a.key[1]==b.key[1];
}
// 0: qualified melee/empty context; 1: ranged; -1: wait for known equipment.
// The native pump refreshes this existing publication independently of pose.
static int ExternalEquipment176(const ExternalPrepare176& owner) noexcept {
    if(!owner.key[0]&&!owner.key[1])return 0;
    PhysEquipment6698 e{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_phys_equipment6698.read(e)||!e.valid||!e.tick||now<e.tick||now-e.tick>80||
       e.count<0||e.count>8||e.manager!=owner.manager||e.player!=owner.player||e.epoch!=owner.epoch||
       e.request!=g_weapon_request_epoch6694.load())return -1;
    bool unknown=false;
    for(int side=0;side<2;++side){
        if(!e.handKnown[side]||e.handKey[side]!=owner.key[side])return -1;
        if(!owner.key[side])continue;
        const int i=e.heldIndex[side];if(i<0||i>=e.count){unknown=true;continue;}
        const auto& p=e.items[i].profile;
        if(p.key!=owner.key[side]||p.side!=side){unknown=true;continue;}
        const int c=p.classId;if(c==9||c==10||c==13||c==14||c==15)return 1;
        if(!(p.melee()||c==8||c==11||c==12||c==17))unknown=true;
    }
    return unknown?-1:0;
}
static ExternalPrepare176 ExternalBefore176(void* input,void* player,void* manager,const char* name,int mode,bool ownedXi178r20) noexcept {
    ExternalPrepare176 ticket{};
    // V66.178R20: a prepare/release/abort the mod itself published on the XI pad is owned by the attack state
    // machine (abort = A then RT-up). Ticketing it as a raw RT and draining it (abort, release, abort) made the
    // release re-introduce RT, which the game saw as a NEW prepare: a self-sustaining prepare/attack loop every
    // ~300 ms (the 18:14 window of the 2026-10-09 log: 180 native swings with the hand idle).
    if(ownedXi178r20)return ticket;
    const bool prepare=mode==1&&!std::strcmp(name,"prepare_to_attack");
    const bool retire=(mode==2&&(!std::strcmp(name,"attack_primary")||!std::strcmp(name,"attack_primary_mouse")))||
        (mode==1&&!std::strcmp(name,"attack_abort"));
    if(!player||(!prepare&&!retire))return ticket;
    // Advance before the original can reenter with a later release/abort/prepare.
    const auto serial=g_external_prepare_generation176.fetch_add(1,std::memory_order_acq_rel)+1;
    std::unique_lock<std::recursive_mutex> lock(g_external_prepare_mutex176,std::try_to_lock);
    if(!lock.owns_lock())return ticket;
    ExternalClear176();
    if(!prepare||!g_gameplay_allowed.load()||animation_filter::nativeBodyOwned112.load()||
       !retail_build112::Ready(WhBase())||!ExternalIdentity176(ticket,input,player,manager))return {};
    ticket.equipment=ExternalEquipment176(ticket);if(ticket.equipment==1)return {};
    ticket.serial=serial;ticket.takeover=g_external_prepare_takeover176.load(std::memory_order_acquire);
    return ticket;
}
static void ExternalAfter176(const ExternalPrepare176& ticket) noexcept {
    if(!ticket.serial)return;
    std::unique_lock<std::recursive_mutex> lock(g_external_prepare_mutex176,std::try_to_lock);
    if(!lock.owns_lock()||g_external_prepare_generation176.load(std::memory_order_acquire)!=ticket.serial)return;
    ExternalPrepare176 current{};
    if(ExternalIdentity176(current,ticket.input,ticket.player,ticket.manager)&&ExternalSame176(ticket,current)){
        g_external_prepare176=ticket; // callback returned; native acceptance remains unknown
        g_external_prepare_observed_takeover176.store(ticket.takeover,std::memory_order_release);
        g_external_prepare_serial176.store(ticket.serial,std::memory_order_release);
    }
}
static bool ExternalPending176() noexcept {
    return g_external_prepare_serial176.load(std::memory_order_acquire)&&
        (g_external_prepare_observed_takeover176.load(std::memory_order_acquire)!=
         g_external_prepare_takeover176.load(std::memory_order_acquire)||CombatExperiment175Enabled());
}
static bool ExternalMustWait176(void* player,unsigned group) noexcept {
    if(group!=1||!g_gameplay_allowed.load()||!ExternalPending176())return false;
    std::unique_lock<std::recursive_mutex> lock(g_external_prepare_mutex176,std::try_to_lock);
    if(!lock.owns_lock())return true; // never race a qualified pending abort with a raw release
    const auto& lease=g_external_prepare176;
    return lease.serial&&lease.serial==g_external_prepare_generation176.load(std::memory_order_acquire)&&
        lease.player==player&&lease.equipment!=1;
}
static void ExternalPrepareDrain176() noexcept {
    // Normal all-off holds and no-lease frames perform no locks or owner reads.
    if(!ExternalPending176()||!g_gameplay_allowed.load()||animation_filter::nativeBodyOwned112.load()||
       animation_filter::NativeKickActive175()||diagnostics::lab::NativeReference())return;
    std::unique_lock<std::recursive_mutex> physical(g_phys_native_mutex6698,std::try_to_lock);
    if(!physical.owns_lock())return;
    std::unique_lock<std::recursive_mutex> lock(g_external_prepare_mutex176,std::try_to_lock);
    if(!lock.owns_lock()||!g_external_prepare176.serial)return;
    auto& pending=g_external_prepare176;
    const auto lease=pending;ExternalPrepare176 current{};
    if(lease.serial!=g_external_prepare_generation176.load(std::memory_order_acquire)||
       !ExternalIdentity176(current,lease.input,lease.player,lease.manager)||!ExternalSame176(lease,current)){
        ExternalClear176();return;
    }
    // A faulted invocation is not evidence of a reset latch. Retain quarantine
    // only while its exact identity survives; never retry its native sequence.
    if(pending.attempted)return;
    // A later physical/native-owned request is never aborted as an old raw RT.
    if(g_phys_native6698.attackPrepared||g_fist_native101.prepared||g_native_pending104.held){
        ExternalClear176();return;
    }
    if(!retail_build112::Ready(WhBase())||g_phys_fault6698.load())return;
    // Immutable class metadata was captured with these exact live hand keys.
    // Keep that qualification across mode/request changes; Off does not replace
    // the observed native preparation. Unknown equipment still fails closed.
    const int equipment=lease.equipment>=0?lease.equipment:ExternalEquipment176(lease);
    if(equipment<0)return;
    pending.equipment=equipment;
    if(equipment==1){ExternalClear176();return;} // explicit ranged exemption
    pending.attempted=true; // set before a native callback can reenter
    // V66.178R20: the drain retires a RAW (non-mod) prepare; it goes straight to the handler so it can never
    // feed the XI state machine.
    const bool abortInvoked=PhysCallAction6698(lease.player,2,1,true);
    bool primaryReleased=false,abortReleased=false;
    const auto stillCurrent=[&]() noexcept {
        ExternalPrepare176 after{};
        return g_external_prepare176.serial==lease.serial&&
            g_external_prepare_generation176.load(std::memory_order_acquire)==lease.serial&&
            ExternalIdentity176(after,lease.input,lease.player,lease.manager)&&ExternalSame176(lease,after);
    };
    if(abortInvoked&&stillCurrent()){
        primaryReleased=PhysCallAction6698(lease.player,1,2,true);
        if(stillCurrent())abortReleased=PhysCallAction6698(lease.player,2,2,true);
    }
    const bool returned=abortInvoked&&primaryReleased&&abortReleased;
    // A nested delivered edge may already have retired/replaced this lease.
    if(returned&&g_external_prepare176.serial==lease.serial&&
       g_external_prepare_generation176.load(std::memory_order_acquire)==lease.serial){
        ExternalClear176();g_external_prepare_generation176.fetch_add(1,std::memory_order_acq_rel);
    }
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64()))
        diagnostic_trace::Write(returned?
            "V66.176 EXTERNAL_PREPARE_DRAIN source=OBSERVED_RAW_PREPARE sequence=ABORT_THEN_RELEASE nativeSequence=RETURNED acceptedAttack=UNCLAIMED":
            "V66.176 EXTERNAL_PREPARE_DRAIN source=OBSERVED_RAW_PREPARE nativeSequence=FAILED quarantine=RETAINED retry=DISABLED acceptedAttack=UNCLAIMED");
}

static void RecordMasterIntent168(const NativeOwner104&,const ph::Command&,const PhysFrame6698&) noexcept;
static void RevokeMasterIntent168() noexcept;
static bool MasterGateEdge47(const NativeOwner104&,const ph::Command&,ph::CommandKind,bool) noexcept; // C47 master_gate47.inl
static bool ArmMasterGate47() noexcept;static void ShutdownMasterGate47() noexcept;
static bool ArmMasterEligibilityH21() noexcept;static void ShutdownMasterEligibilityH21() noexcept;
static bool ArmPlayerWindowH26() noexcept;static void ShutdownPlayerWindowH26() noexcept;
#include "vrik/native_attack_timing178r22c2.inl"
#include "vrik/native_input117.inl"
#include "vrik/native_bridges168.inl"
#include "vrik/master_gate47.inl" // C47 VrMasterStrikeStabOnly
#include "vrik/master_eligibility_h21.inl"
#include "vrik/player_window_h26.inl"
#include "vrik/native_motion112.inl"
#include "vrik/door_motion_native66.inl"
#include "vrik/native_prediction120.inl"
#include "vrik/bow_zoom121.inl"
#include "vrik/diagnostics122.inl"
#include "vrik/material_observer116.inl"
static bool PredictionCapture120() noexcept {
    const auto now=std::uint64_t(GetTickCount64()),until=g_contact_until114.load();
    return g_weapon_control_mode.load()==7&&until&&now<until&&
        diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now);
}
static void StartPredictionCapture120() noexcept {
    g_contact_until114=GetTickCount64()+180000;
    diagnostics::trace164::until.store(g_contact_until114.load(),std::memory_order_release);
    diagnostic_trace::Write(ArmNativeContact114()&&ArmMaterial116()?
        "V66.120 F12=7 CONTACT_MATERIAL_CAPTURE=READY durationMs=180000 HenryOnly=1":
        "V66.120 F12=7 CONTACT_MATERIAL_CAPTURE=UNAVAILABLE predictionHasSeparateCounters=1");
}
static bool NativeHeldFingerKey101(int side,std::uint64_t& key) noexcept {
    key=0;if(g_weapon_control_mode.load()<3)return false;
    void* manager=g_henry_item_attach_manager.load();
    return NativeHandKey6697(manager,side,key)&&key&&!PhysEmpty6698(key);
}
#include "vrik/kick_contact176.inl"
#include "vrik/staged_leaf_pins596c2.inl"
#include "vrik/prop_native_pins5916c1.inl"
#include "vrik/staged_pickup596c2.inl"
#include "vrik/staged_reload596c2.inl"
static std::atomic<bool> g_reload_pose_ready_xr34{false};
#include "subsystems/ranged_reload/crossbow_rifle_request_h21.inl"
#include "subsystems/ranged_reload/reload_passenger_xr34.inl"
#include "vrik/manual_arrow64.inl"
#include "subsystems/ranged_reload/crossbow_hand_d89.inl" // R89 xbow
#include "vrik/hand_presence_h24.inl"
#include "vrik/contact_occupancy_d52.inl"
#define KCD2VR_POSE_COST_D60(stage) kcd2vr::diagnostics::pose_cost_d60::Scope poseCostD60(kcd2vr::diagnostics::pose_cost_d60::stage)
#include "vrik/coherent_pose.inl"
#include "vrik/motion_skin_d68.inl"
#include "vrik/stowed_render_native_d63.inl"
#undef KCD2VR_POSE_COST_D60
#include "vrik/render_bounds63.inl"
#include "vrik/bow_string128.inl"
#include "vrik/npc_flinch178r11.inl"
#include "vrik/finisher_upper_d83.inl" // D83 finisher
#include "vrik/item_census178r19.inl"
#include "vrik/contact_fx178r12.inl"
} // sole private production pose adapter
// H41: external adapters stay outside the private fist implementation namespace.
void SetFistKickEnabledH35(bool on) noexcept {SetFistKickEnabledImplH35(on);}
bool FistKickEnabledH35() noexcept {return FistKickEnabledImplH35();}
FistKickResultH35 RequestFistKickStatusH35(int side) noexcept {return RequestFistKickStatusImplH35(side);}
#include "vrik/rpg_param178r42.inl" // Public compatibility API; unsafe R42 writer retired.
#include "vrik/npc_contact172.inl"
#include "vrik/part_namespace62.inl"
#include "subsystems/gore/native_part_anchor.inl"
#include "subsystems/gore/native_gore.inl" // C32 impale hang + decapitation (native side)
#include "subsystems/gore/native_blood.inl" // C49 BLOODY FIGHTS (native side)
#include "vrik/touch_react43.inl" // C43T npc touch reaction (native AddImpact)
#include "vrik/visible_failure.inl" // DS-F12 visible failure line (needs gore32n::g_lua_exec43)
#include "vrik/dagger_chest54.inl" // C54 actual carried dagger cosmetic; no right-hand draw
#include "vrik/utility_gestures54.inl"
#include "subsystems/gore/native_impale_part.inl"
#include "vrik/self_collision_native_r103.inl" // R103: held prop / dragged corpse vs Henry (before lab58 + static grab)
#include "vrik/lab58.inl"
#include "vrik/lab_prop58.inl"
#include "vrik/brush_swap_native_r100.inl" // R100: brush -> loose entity swap native leaves (pinned)
#include "vrik/static_grab_r89.inl" // R90 (R89 pickup): carry small scenery props
#include "vrik/pickup_use_fallback_r120.inl" // R120: item Use when the game interactor did not run it
#include "vrik/feature_probe59c6.inl"
void SetMotionClock50C2(bool on) noexcept {kcd2vr::attackclock50::enabled.store(on);}
bool ArmPhysicalWeapons6698() noexcept {return PhysArmImpl6698();}
bool ReadF4ContactLive176(kcd2vr::contact176::Live& out) noexcept {return CurrentF4ContactLive176(out);}
void SetCombatAdmissionAssist51(bool enabled) noexcept {g_admission_enabled51.store(enabled,std::memory_order_release);}
bool GeometryAdmittedRelease51(const kcd2vr::contact176::Ticket& t,const void* receiver) noexcept {
    if(!MotionClockSelected50())return false;
    ac50::Binding b{};ac50::View v{};
    if(!g_motion_clock_binding50.read(b)||g_motion_clock_actor50.load()!=b.intent.actor||
       b.intent.policy!=g_attack_timing_policy22.load()||!ac50::sameRelease(b,t)||
       !kcd2vr::contact176::fresh(t.tick,GetTickCount64(),1500)||
       !GeometryIntentOwnerH30(t.key,t.generation,t.epoch,t.request))return false;
    if(!ac50::inspect(b,WhBase(),ClockRead50,v))return false;
    std::uintptr_t current=0;
    return !receiver||(ClockRead50(b.helper+0x18,&current,8)&&current==reinterpret_cast<std::uintptr_t>(receiver));
}
bool GeometryNativeAnswer19c4r2(const void* actor,const void* target,int attackType,kcd2vr::answer59c4r2::Lineage& out) noexcept {
    out={};
    if(!actor||!target||(attackType!=0&&attackType!=1)||!CombatSynchronized59c10r9()||!MotionClockSelected50())return false;
    ac50::Binding b{};ac50::View v{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_motion_clock_binding50.read(b)||b.intent.actor!=reinterpret_cast<std::uintptr_t>(actor)||
       g_motion_clock_actor50.load()!=b.intent.actor||b.intent.policy!=g_attack_timing_policy22.load()||
       b.intent.epoch!=g_pose_epoch6689.load()||b.intent.request!=g_weapon_request_epoch6694.load()||
       !kcd2vr::contact176::fresh(b.intent.tick,now,1500)||!ac50::inspect(b,WhBase(),ClockRead50,v)||v.phase!=8)return false;
    // The exact native FOLLOWUP_C constructs its target from current CF8 and
    // attack type from current state+210 (C55C58..8F,173BDA5..C3). The row and
    // CURRENT main slot must describe that same ordinary action before submit.
    std::uintptr_t opponent=0,type=0;int rowType=-1,stateType=-1;
    if(!ClockRead50(b.intent.state+0xCF8,&opponent,8)||opponent!=reinterpret_cast<std::uintptr_t>(target)||
       !ClockRead50(b.row+0x148,&type,8)||!type||!ClockRead50(type+8,&rowType,4)||rowType!=attackType||
       !ClockRead50(b.intent.state+0x218,&stateType,4)||stateType!=attackType||
       !GeometryIntentOwnerH30(b.intent.key,b.intent.generation,b.intent.epoch,b.intent.request))return false;
    const auto recovery=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
    kcd2vr::contact176::Ticket ticket{};
    const kcd2vr::contact59c3::Key key{b.intent.gesture59c3,b.intent.key,b.intent.generation,b.intent.epoch,b.intent.request,recovery};
    if(!kcd2vr::contact59c3::releases.find(key,ticket)||!ac50::sameRelease(b,ticket)||
       ticket.recovery59c3!=recovery||!kcd2vr::contact176::fresh(ticket.tick,now,1500))return false;
    // Revalidate publication/identity after the bounded history read. No new
    // request, native call, global polling or borrowed-pointer post-submit use.
    ac50::Binding again{};ac50::View checked{};std::uintptr_t opponentAgain=0;int typeAgain=-1;
    if(!g_motion_clock_binding50.read(again)||again.action!=b.action||again.intent.serial!=b.intent.serial||
       again.intent.policy!=b.intent.policy||again.intent.actor!=b.intent.actor||again.intent.state!=b.intent.state||
       again.helper!=b.helper||again.row!=b.row||again.manager!=b.manager||g_attack_timing_policy22.load()!=b.intent.policy||
       g_motion_clock_actor50.load()!=b.intent.actor||kcd2vr::contact176::recoveryEpoch55c1.load()!=recovery||
       !ac50::sameRelease(again,ticket)||!ac50::inspect(again,WhBase(),ClockRead50,checked)||checked.phase!=8||
       !ClockRead50(b.intent.state+0xCF8,&opponentAgain,8)||opponentAgain!=opponent||
       !ClockRead50(b.intent.state+0x218,&typeAgain,4)||typeAgain!=attackType)return false;
    out={ticket,b.action};return true;
}
// H17: the delayed native-clip/fallback service belongs to the ORIGINAL R3
// context. Cancellation retires its correlation as well as an already drawn leg.
struct KickServiceRequestH17 {
    std::uint64_t tick=0,generation=0,context=0,pose=0,tracking=0,clipAtRequest=0,nativeSerial=0;
    kcd2vr::kick26::Style style=kcd2vr::kick26::Style::Game;bool combat=false;
};
static kcd2vr::core::SnapshotMailbox<KickServiceRequestH17> g_kick_service_request_h17;
static std::atomic<std::uint64_t> g_kick_service_generation_h17{1};
static void CancelKickServiceForR3H24(std::uint64_t serial,std::uint64_t tick) noexcept {
    KickServiceRequestH17 service{};
    if(g_kick_service_request_h17.read(service)&&service.nativeSerial==serial&&service.tick==tick)CancelProceduralKick176();
}
static void RetireKickServiceH17() noexcept {
    // A still-queued original R3 must not survive tracking/context recovery
    // inside its120ms native deadline. This retires input, never an admitted
    // native animation; the visual-only handover below deliberately skips it.
    g_special_context168.fetch_add(1,std::memory_order_acq_rel);
    g_special_pending168.store(false,std::memory_order_release);
    g_kick_service_generation_h17.fetch_add(1,std::memory_order_acq_rel);
    kcd2vr::kick26::requestTick.store(0,std::memory_order_release);
    kcd2vr::kick26::reachTick.store(0,std::memory_order_release);
}
static bool KickServiceTrackingH17(TrackingSnapshot& t,std::uint64_t now) noexcept {
    return g_gameplay_allowed.load()&&!animation_filter::nativeBodyOwned112.load()&&
        ReadTrackingSnapshot(t)&&t.allowed&&t.head_valid&&t.left_valid&&t.right_valid&&
        !t.head_held&&!t.left_held&&!t.right_held&&t.tick&&now>=t.tick&&now-t.tick<=80&&
        t.source_epoch&&t.source_epoch==tracking59c6::epoch.load(std::memory_order_acquire);
}
static void CancelKickVisualH17() noexcept {
    g_kick_cancel176.fetch_add(1,std::memory_order_acq_rel);
    g_kick_busy176.exchange(0,std::memory_order_acq_rel);
    kcd2vr::kick34::ledger.retire(kcd2vr::kick34::Why::Cancelled); // C37: a cancelled leg never applies
}
void CancelProceduralKick176() noexcept {
    RetireKickServiceH17();CancelKickVisualH17();
}
// C52 (Miki 18 Oct): the kick aims at the opponent. ONE read at the R3 click (XR thread), never per frame: Henry's
// combat state +0xCF8 (his current opponent actor, the V168 field) -> the opponent's C_Player (+0x1F8, back link +0x278)
// -> its entity (+0x38) -> world origin (the same reads clinch_distance32 makes). Target = origin + 0.95 m (belly).
// No native call, no retained pointer; a kick without an opponent goes straight forward as before.
static bool KickTargetAtClick52(kcd2vr::pose_core::Vec3& world,float& distance) noexcept {
    world={};distance=0;
    ReadScope101R2 reads;void* manager=g_henry_item_attach_manager.load();void* player=nullptr;NativeOwner104 henry{};
    void *opponent=nullptr,*enemyState=nullptr,*owner=nullptr,*back=nullptr,*entity=nullptr;
    if(!PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,player)||!NativeResolve104(player,henry)||!henry.state||!henry.entity)return false;
    if(!Read(static_cast<unsigned char*>(henry.state)+0xCF8,opponent)||!opponent||opponent==henry.actor||!CurrentOpponent168(opponent,enemyState))return false;
    if(!Read(static_cast<unsigned char*>(opponent)+0x1F8,owner)||!owner||!Read(static_cast<unsigned char*>(owner)+0x278,back)||back!=opponent||
       !Read(static_cast<unsigned char*>(owner)+0x38,entity)||!entity||entity==henry.entity)return false;
    std::uint32_t hid=0,oid=0;pm::Vec3 hp{},op{};
    if(!ClinchEntityPosition32(henry.entity,hid,hp)||!ClinchEntityPosition32(entity,oid,op)||hid==oid)return false;
    const double d=pm::length(op-hp);
    if(!std::isfinite(d)||d<.2||d>3.0)return false; // not a kick target: standing in him or out of any kick's reach
    // Visual aim estimate only: +0.95 m is not a native belly hitbox or a
    // damage location. Revalidate the same identities after the bounded read.
    NativeOwner104 again{};void *currentOpponent=nullptr,*currentOwner=nullptr,*currentEntity=nullptr;
    if(!NativeResolve104(player,again)||again.actor!=henry.actor||again.state!=henry.state||again.entity!=henry.entity||again.epoch!=henry.epoch||
       !Read(static_cast<unsigned char*>(henry.state)+0xCF8,currentOpponent)||currentOpponent!=opponent||
       !Read(static_cast<unsigned char*>(opponent)+0x1F8,currentOwner)||currentOwner!=owner||
       !Read(static_cast<unsigned char*>(owner)+0x38,currentEntity)||currentEntity!=entity)return false;
    world={op.x,op.y,op.z+.95};distance=float(d);return true;
}
static bool RequestProceduralKick176(std::uint64_t& accepted) noexcept {accepted=0;
    const auto now=std::uint64_t(GetTickCount64());TrackingSnapshot tracking{};
    if(!CombatExperiment175Enabled()||!g_special_allowed168.load(std::memory_order_acquire)||
       !g_gameplay_allowed.load()||!CalibrationReady()||diagnostics::lab::NativeReference()||
       animation_filter::nativeBodyOwned112.load()||kcd2vr::kick26::NativeBlocksVrLeg(animation_filter::NativeKickActive175())||
       !ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||!tracking.left_valid||!tracking.right_valid||
       tracking.head_held||tracking.left_held||tracking.right_held||now<tracking.tick||now-tracking.tick>80)return false;
    const auto serial=g_kick_serial176.fetch_add(1,std::memory_order_acq_rel)+1;
    if(!serial)return false;
    const auto cancel=g_kick_cancel176.load(std::memory_order_acquire);std::uint64_t empty=0;
    if(!g_kick_busy176.compare_exchange_strong(empty,serial,std::memory_order_acq_rel))return false;
    kcd2vr::kick176::Request request{serial,now,cancel};
    request.haveTarget=KickTargetAtClick52(request.targetWorld,request.targetDistance);
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
        char m[280]{};std::snprintf(m,sizeof(m),"C54 KICK_AIM serial=%llu profile=%s target=%s distance=%.2f referenceHitMs=%llu reach=%d%% nativeAdmission=UNCHANGED damage=UNCHANGED",
            (unsigned long long)serial,kcd2vr::kick52::name(kcd2vr::kick26::Profile52()),request.haveTarget?"OPPONENT_ORIGIN_HEIGHT_ESTIMATE":"NONE(forward)",
            double(request.targetDistance),(unsigned long long)kcd2vr::kick26::LegLeadMs(),kcd2vr::kick26::reachPercent52.load());diagnostic_trace::Write(m);}

    if(!g_kick_request176.publish(request)||g_kick_cancel176.load(std::memory_order_acquire)!=cancel||
       !CombatExperiment175Enabled()||!g_special_allowed168.load(std::memory_order_acquire)){
        auto own=serial;(void)g_kick_busy176.compare_exchange_strong(own,0,std::memory_order_acq_rel);return false;
    }
    accepted=serial;return true;
}
bool RequestProceduralKick176() noexcept {std::uint64_t serial=0;return RequestProceduralKick176(serial);}
// C37 (review: no procedural damage bypass): R3 in the VR LEG KICK style issues the leg request and, on the same
// click, opens ONE kick transaction with that request serial, bound to the exact Henry (manager -> C_Player ->
// C_CombatPlayer actor + combat state, the same resolution as the native special route) and to the pose epoch.
// The leg is drawn even when no transaction opens (then the kick is only a picture, said in the receipt).
bool RequestVrLegKick34() noexcept {
    std::uint64_t serial=0;if(!RequestProceduralKick176(serial)||!serial)return false;
    namespace k=kcd2vr::kick34;const auto now=std::uint64_t(GetTickCount64());
    k::Binding b{};b.serial=serial;b.tick=now;b.poseEpoch=g_pose_epoch6689.load(std::memory_order_acquire);
    const char* why="OPEN";NativeOwner104 owner{};void* player=nullptr;
    {ReadScope101R2 reads;void* manager=g_henry_item_attach_manager.load();
     if(!combat_diag_probe::GeometryHitsEnabled178r27())why="HIT_ROUTE_OFF(direct native hits disabled in this F4 mode)";
     else if(!retail_build112::Verify(WhBase()))why="RETAIL_BUILD_UNVERIFIED";
     else if(!PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,player)||!NativeResolve104(player,owner))why="HENRY_UNRESOLVED";
     else if(owner.epoch!=b.poseEpoch)why="POSE_EPOCH_MOVED";}
    if(!std::strcmp(why,"OPEN")){b.actor=reinterpret_cast<std::uintptr_t>(owner.actor);b.state=reinterpret_cast<std::uintptr_t>(owner.state);
        b.player=reinterpret_cast<std::uintptr_t>(player);if(!k::ledger.open(b))why="REFUSED_BY_LEDGER";}
    char m[260]{};std::snprintf(m,sizeof(m),"C37 KICK_TX serial=%llu stage=%s henry=%p state=%p epoch=%llu expiresMs=%llu applications<=1 hit=%s",
        (unsigned long long)serial,why,owner.actor,owner.state,(unsigned long long)b.poseEpoch,(unsigned long long)k::kExpiryMs,
        std::strcmp(why,"OPEN")?"NONE(leg only)":"OBSERVE_ONLY_NATIVE_ADMISSION_UNPROVEN");
    diagnostic_trace::Write(m);return true;
}
// C26 KICKS page. Called once per XR frame (two atomic loads when idle). No hook, no timer.
void SetKick26(int style,int legSide,int reachMs,bool freesImpale) noexcept {
    namespace k=kcd2vr::kick26;
    const int st=std::clamp(style,0,2),side=std::clamp(legSide,0,2),reach=std::clamp(reachMs,300,1200);
    const bool styleChanged=k::style.exchange(st)!=st;
    const bool changed=styleChanged|k::legSide.exchange(side)!=side|k::reachMs.exchange(reach)!=reach|k::freesImpale.exchange(freesImpale)!=freesImpale;
    if(styleChanged){++g_special_context168;g_special_pending168=false;CancelProceduralKick176();}
    kcd2vr::combat_rules178r10::kickTransport.store(k::TransportFor(k::Style(st),kcd2vr::combat_rules178r10::kickTransportIni26.load()));
    if(!changed)return;
    char m[220]{};std::snprintf(m,sizeof(m),"C26 KICK_STYLE style=%s leg=%s reachMs=%d freesImpale=%d transport=%d rules=NATIVE_KICK_ACTION(style 0/1) damage=NATIVE",
        k::Name(k::Style(st)),side==0?"RIGHT":side==1?"LEFT":"ALTERNATE",reach,freesImpale?1:0,kcd2vr::combat_rules178r10::kickTransport.load());
    diagnostic_trace::Write(m);
}
void NoteKickRequest26(bool toGame) noexcept {
    namespace k=kcd2vr::kick26;const auto now=std::uint64_t(GetTickCount64());
    TrackingSnapshot tracking{};KickServiceRequestH17 request{};
    request.tick=now;request.generation=g_kick_service_generation_h17.load(std::memory_order_acquire);
    request.context=g_special_context168.load();request.pose=g_pose_epoch6689.load();
    request.style=k::Current();request.combat=CombatExperiment175Enabled();
    request.clipAtRequest=k::nativeClipStart.load(std::memory_order_acquire);
    if(!g_special_allowed168.load()||!KickServiceTrackingH17(tracking,now))return;
    if(toGame){
        // Reuse the queued original native R3 identity/time, not a new delayed intent.
        b168::SpecialRequest original{};
        if(!g_special_request168.read(original)||!original.serial||
           !b168::fresh(original,now,request.context,request.pose))return;
        request.tick=original.tick;request.nativeSerial=original.serial;
    }
    request.tracking=tracking.source_epoch;
    if(!g_kick_service_request_h17.publish(request)||
       request.generation!=g_kick_service_generation_h17.load(std::memory_order_acquire))return;
    k::requestTick.store(request.tick,std::memory_order_release);
    k::reachTick.store(toGame?0:now+k::kLegExtendMs,std::memory_order_release); // style 2: our leg reaches out at 250 ms
    if(request.generation!=g_kick_service_generation_h17.load(std::memory_order_acquire))RetireKickServiceH17();
}
void ServiceKick26(std::uint64_t now) noexcept {
    {   // C37: transactions that closed outside the game-thread drain (no contact in 900 ms, recenter, cancel, new click).
        kcd2vr::kick34::ledger.sweep(now);static std::uint64_t seen37=0;kcd2vr::kick34::Note n{};
        if(kcd2vr::kick34::ledger.takeNote(seen37,n)&&(n.why==kcd2vr::kick34::Why::Expired||n.why==kcd2vr::kick34::Why::EpochChanged||
           n.why==kcd2vr::kick34::Why::Cancelled||n.why==kcd2vr::kick34::Why::Superseded)){
            char m[160]{};std::snprintf(m,sizeof(m),"C37 KICK_TX serial=%llu stage=CLOSED reason=%s applied=0",(unsigned long long)n.serial,kcd2vr::kick34::name(n.why));
            diagnostic_trace::Write(m);}
    }
    namespace k=kcd2vr::kick26;static k::Service service{};
    const auto request=k::requestTick.load(std::memory_order_acquire);
    if(!request)return;
    KickServiceRequestH17 lease{};TrackingSnapshot tracking{};b168::SpecialRequest original{};
    if(!g_kick_service_request_h17.read(lease)||lease.tick!=request||
       lease.generation!=g_kick_service_generation_h17.load(std::memory_order_acquire)||
       lease.context!=g_special_context168.load()||lease.pose!=g_pose_epoch6689.load()||
       lease.style!=k::Current()||
       lease.combat!=CombatExperiment175Enabled()||!g_special_allowed168.load()||
       !KickServiceTrackingH17(tracking,now)||lease.tracking!=tracking.source_epoch||
       (lease.nativeSerial&&(!g_special_request168.read(original)||original.serial!=lease.nativeSerial||
        original.tick!=lease.tick||original.context!=lease.context||original.epoch!=lease.pose))){
        g_special_pending168=false;CancelProceduralKick176();return;
    }
    const auto nativeClip=k::nativeClipStart.load(std::memory_order_acquire);
    const auto clip=nativeClip!=lease.clipAtRequest?nativeClip:0;
    if(clip==service.handledClip&&(request==0||request==service.reportedRequest))return;
    if(CombatExperiment175Enabled()&&k::Current()==k::Style::VrLegOnly){service.handledClip=clip;service.reportedRequest=request;service.legRequest=request;return;} // U16: only immediate contact-probe route already owns the leg
    const auto style=k::Current();
    const auto d=k::Step(service,style,now,clip,request,std::uint64_t(k::reachMs.load(std::memory_order_relaxed)));
    if(lease.generation!=g_kick_service_generation_h17.load(std::memory_order_acquire))return;
    if(d.reach)k::reachTick.store(d.reach,std::memory_order_release);
    bool leg=false;if(d.requestLeg)leg=RequestProceduralKick176();
    if(lease.generation!=g_kick_service_generation_h17.load(std::memory_order_acquire)){CancelProceduralKick176();return;}
    if(d.clipSeen||d.clipMissing||d.requestLeg){char m[240]{};
        std::snprintf(m,sizeof(m),"C26 KICK style=%s nativeClip=%s afterMs=%llu reachMs=%d vrLeg=%s",k::Name(style),
            d.fallback?"NO_MATCHING_ACTIVE_CLIP_YET(VR leg fallback)":d.clipMissing?"NO_MATCHING_ACTIVE_CLIP_OBSERVED":d.clipSeen?"STARTED":"STARTED_EARLIER",
            (unsigned long long)d.afterMs,k::reachMs.load(),d.requestLeg?(leg?"DRAWN":"REFUSED(tracking/context)"):"NO");
        diagnostic_trace::Write(m);}
}
void SetNativeSpecialContext168(bool allowed) noexcept {
    if(g_special_allowed168.exchange(allowed,std::memory_order_acq_rel)!=allowed)++g_special_context168;
    if(!allowed){g_special_pending168=false;if(g_kick_busy176.load()||kcd2vr::kick26::requestTick.load())CancelProceduralKick176();}
}
static std::atomic<std::uint64_t> g_r3_last_ingress_h24{0};
bool RequestNativeSpecial168() noexcept {
    const bool direct17=kcd2vr::combat_rules178r10::kickTransport.load(std::memory_order_relaxed)==3; // V66.178R17: allowed inside F4 too
    if((CombatExperiment175Enabled()&&!direct17)||!g_special_allowed168.load()||g_special_pending168.load()||diagnostics::lab::NativeReference()||
       !retail_build112::Verify(WhBase())||!ArmCombatInput101(true))return false;
    ReadScope101R2 reads;void* manager=g_henry_item_attach_manager.load();void* player=nullptr;NativeOwner104 owner{};
    if(!PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,player)||!NativeResolve104(player,owner))return false;
    const auto nowH24=std::uint64_t(GetTickCount64());
    const auto previousH24=g_r3_last_ingress_h24.load(std::memory_order_acquire);
    if(previousH24&&(nowH24<previousH24||nowH24-previousH24<600))return false;
    b168::SpecialRequest r{++g_bridge_serial168,nowH24,g_special_context168.load(),owner.epoch,
        reinterpret_cast<std::uintptr_t>(player),reinterpret_cast<std::uintptr_t>(owner.actor),reinterpret_cast<std::uintptr_t>(owner.state)};
    if(!g_special_request168.publish(r))return false;
    g_r3_last_ingress_h24.store(nowH24,std::memory_order_release);
    g_special_pending168.store(true,std::memory_order_release);return true;
}
bool NativeFreeKickVisualR24() noexcept {return NativeFreeKickVisualCurrentR24();}
bool ArmMotionCombatInput101() noexcept {return (retail_build112::Ready(WhBase())||retail_build112::Verify(WhBase()))&&NativeArm104()&&NativeInputArm117()&&ArmCombatInput101(false);}
// V66.178 (game thread): publish whether Henry's native attack is still requested /
// prepared, and log the strike qualification of the latest physical frame (F4 rung 2).
// V66.178R2 READ-ONLY probe of KCD's own combat collision targets (the shapes the
// native weapon sweep A7A124 tests against). Static facts (WHGame 0x6A350E20):
// collisions = C_CombatActor+0x218; per hand h: target list head at collisions+0x58+16h,
// count at collisions+0x60+16h (D510C4); node+0x20 = target object; target vfunc +0x48
// writes its bounds {min Vec3, max Vec3} (used by D510C4 itself). No scene query, no
// collision publication, no writes. Answers: is the list filled outside native attacks,
// and do the bounds sit on the enemy body? Logged every 500 ms in F4 rung 2 only.
static bool TargetBounds178(void* target,float out[6]) noexcept {
    const auto b=WhBase();std::uintptr_t vt=0,fn=0;
    if(!target||!Read(target,vt)||!vt||!Read(reinterpret_cast<void*>(vt+0x48),fn)||fn<b+0x1000||fn>=b+0x3A03000)return false;
    using BoundsFn=void(__fastcall*)(void*,float*);float tmp[8]{};
    __try{reinterpret_cast<BoundsFn>(fn)(target,tmp);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    for(int i=0;i<6;++i){if(!std::isfinite(tmp[i]))return false;out[i]=tmp[i];}
    return tmp[3]>=tmp[0]&&tmp[4]>=tmp[1]&&tmp[5]>=tmp[2];
}
static void CombatTargetProbe178(const NativeOwner104& owner) noexcept {
    static std::uint64_t next=0;const auto now=std::uint64_t(GetTickCount64());if(now<next)return;next=now+500;
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return; // D30: F3 only, no native reads/formatting otherwise
    void* collisions=nullptr;if(!owner.actor||!Read(static_cast<unsigned char*>(owner.actor)+0x218,collisions)||!collisions)return;
    PhysFrame6698 f{};float hx=0,hy=0,hz=0;
    if(g_phys_frame6698.read(f)&&f.output.active[0]){const auto w=ph::point(f.input.world,f.output.weapons[0].p*f.input.worldScale);hx=float(w.x);hy=float(w.y);hz=float(w.z);}
    for(int hand=0;hand<2;++hand){
        const auto base=static_cast<unsigned char*>(collisions)+0x58+16*hand;std::uint64_t count=0;void* head=nullptr;
        if(!Read(static_cast<unsigned char*>(collisions)+0x60+16*hand,count)||!Read(base,head)||!head)continue;
        char m[900]{};int pos=std::snprintf(m,sizeof(m),"V66.178R2 COMBAT_TARGETS hand=%d count=%llu weaponWorld=(%.2f,%.2f,%.2f) attackBusy=%d",
            hand,(unsigned long long)count,hx,hy,hz,g_native_busy178.load()?1:0);
        // MSVC std::set/map tree (A7A124 checks node+0x19 "isnil" and steps with the
        // successor 0x9462B0): node {left+0,parent+8,right+0x10,color+0x18,isnil+0x19,value+0x20}.
        auto isnil=[](void* n){unsigned char v=1;return !n||!Read(static_cast<unsigned char*>(n)+0x19,v)||v!=0;};
        auto successor=[&](void* n)->void*{
            void* r=nullptr;if(!Read(static_cast<unsigned char*>(n)+0x10,r))return nullptr;
            if(!isnil(r)){void* l=nullptr;for(int g=0;g<64&&Read(r,l)&&!isnil(l);++g)r=l;return r;}
            void* p=nullptr;for(int g=0;g<64;++g){if(!Read(static_cast<unsigned char*>(n)+8,p)||!p)return nullptr;
                void* pr=nullptr;if(isnil(p)||!Read(static_cast<unsigned char*>(p)+0x10,pr)||pr!=n)return p;n=p;}
            return nullptr;};
        void* node=nullptr;if(!Read(head,node))continue; // head->left = leftmost
        for(int i=0;i<6&&node&&node!=head&&!isnil(node)&&pos>0&&pos<int(sizeof(m))-140;++i){
            void* target=nullptr;std::uintptr_t vt=0;float bb[6]{};
            if(!Read(static_cast<unsigned char*>(node)+0x20,target)||!target)break;(void)Read(target,vt);
            const bool ok=TargetBounds178(target,bb);
            pos+=std::snprintf(m+pos,sizeof(m)-pos," [%d vft=+%llX bounds=%s min=(%.2f,%.2f,%.2f) max=(%.2f,%.2f,%.2f)]",i,
                (unsigned long long)(vt?vt-WhBase():0),ok?"OK":"NO",bb[0],bb[1],bb[2],bb[3],bb[4],bb[5]);
            node=successor(node);
        }
        diagnostic_trace::Write(m);
    }
}
// V66.178R27 stage 3: enemy weapon/shield identity from the entity NAME (CEntity vtable +0x90), cached per entity id.
static const char* WeaponClash178r27(std::uint32_t entityId) noexcept {
    if(!entityId)return nullptr;
    struct Entry {std::uint32_t id=0;const char* cls=nullptr;std::uint64_t seen=0;};static Entry cache[16]{};static unsigned next=0;
    const auto now=std::uint64_t(GetTickCount64());
    for(auto& e:cache)if(e.id==entityId&&now-e.seen<10000)return e.cls;
    void* ent=EntityById6694(entityId);char name[64]{};const char* cls=nullptr;
    if(ent&&EntityNameV178r19(ent,entityId,name,sizeof(name))){
        char lower[64]{};for(std::size_t i=0;i<sizeof(lower)-1&&name[i];++i)lower[i]=char((name[i]>='A'&&name[i]<='Z')?name[i]+32:name[i]);
        static const char* shields[]{"shield","buckler","pavise"};
        static const char* weapons[]{"sword","sabre","saber","axe","mace","hammer","spear","polearm","halberd","dagger","club","flail","cudgel","falchion","messer","bardiche","glaive","weapon","staff"};
        for(const char* k:shields)if(std::strstr(lower,k)){cls="SHIELD";break;}
        if(!cls)for(const char* k:weapons)if(std::strstr(lower,k)){cls="WEAPON";break;}
        char m[200]{};std::snprintf(m,sizeof(m),"V66.178R27 GEOM_CLASH_IDENTITY entity=%u name=%s class=%s",entityId,name,cls?cls:"OTHER");diagnostic_trace::Write(m);
    }
    auto& e=cache[next++%16];e.id=entityId;e.cls=cls;e.seen=now;return cls;
}
// D41: the physical prop's copied allowed bit cannot outlive current gameplay.
// Only existing atomics and coherent POD snapshots are read on the physics
// callback. No native call, wait, object lookup or actor mutation occurs here.
static bool GrabSourceCurrentD41(const kcd2vr::physics::grab_producer60::Intent& in) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    const auto policy=g_pose_policy_generation57.load(std::memory_order_acquire);
    const auto context=[&]() noexcept {
        return !(policy&1)&&policy==g_pose_policy_generation57.load(std::memory_order_acquire)&&
            in.side>=0&&in.side<2&&in.player&&in.poseEpoch&&in.trackingEpoch&&
            in.poseEpoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
            in.trackingEpoch==tracking59c6::epoch.load(std::memory_order_acquire)&&
            in.player==reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire))&&
            g_cal_state.load(std::memory_order_acquire)==2&&g_gameplay_allowed.load(std::memory_order_acquire)&&
            !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_reading.load()&&!g_ctx_scripted.load()&&
            !animation_filter::nativeBodyOwned112.load()&&!NativePassengerCurrentA34(now)&&
            !diagnostics::lab::NativeReference()&&!kcd2vr::workstation178r20c1::ownsWrist(now)&&
            TrackingHeadUsable111(now)&&TrackingHandUsable111(in.side,now);
    };
    if(!context())return false;
    CommittedFrame6695 packet{};
    if(!ReadCommittedPacket6695(packet)||!packet.valid[in.side]||packet.tools59c10[in.side].valid||
       packet.doorPlayer38!=in.player||packet.stamp.epoch!=in.poseEpoch||packet.doorTrackingEpoch66!=in.trackingEpoch||
       !packet.stamp.tick||now<packet.stamp.tick||now-packet.stamp.tick>80||
       (frik_pose_probe::PhysicalHandReservationsPhysicsD41()&(1u<<in.side)))return false;
    return context();
}
#include "subsystems/gore/corpse_impale_producer_r93.inl"
// R96 (complaint 5, no permanent blockers): a native SEH fault used to switch these features off until restart.
// They are exceptions on a stale native object (a body/actor freed between the check and the call), not a code
// mismatch: pins/vtable/retail checks still run before every call and stay authoritative. A save/level load builds
// new objects, so the latch is cleared here, on the same game thread that sets it, once per load. One log line each.
// R97 merge: the load edge is the T8 observed load episode (load_rearm_r96::Seen). Latches that are now Codex
// load_fault_recovery_r96::Fault objects clear themselves when that same load completes and are NOT listed here (no
// double reset): gore32n::g_drive_fault33 (impale hang/hold, corpse drag - lab58n::g_dragFault is now an alias of it,
// decap impulses), g_corpse_contact_fault_h7, g_prop_fault5916c1 (physical pickups). g_pose_restore_fault6689 is owned
// by the Codex pose-restore debt (coherent_pose.inl ObservePoseRestoreRecoveryR96: one exact restore attempt per
// completed load, or a new player lifetime) - clearing only the flag here would leave that debt active, skip the
// restore of the failed joints and block the next debt from being recorded.
static void RearmAfterLoadR96() noexcept {
    static kcd2vr::load_rearm_r96::Seen seen;if(!seen.take())return;
    namespace lr=kcd2vr::load_rearm_r96;char m[420]{};int at=0;
    const auto note=[&](bool was,const char* what){if(was&&at>=0&&at<int(sizeof(m)))at+=std::snprintf(m+at,sizeof(m)-at," %s",what);};
    note(lr::rearm(gore32n::g_joint_fault44),"DECAP_LOOSE_NECK");
    note(lr::rearm(touch43::g_latched),"NPC_TOUCH");
    note(lr::rearm(g_contact_fx_fault178r12),"BLOOD_CONTACT_FX");
    note(lr::rearm(g_flinch_fault178r11),"NPC_FLINCH");
    note(lr::rearm(g_fist_fault101),"FISTS");
    note(lr::rearm(g_native_fault104),"NATIVE_COMBAT_REQUEST");
    note(lr::rearm(g_finisher_fault_d83),"FINISHER_UPPER");
    note(lr::rearm(g_motion_clock_fault50),"ATTACK_CLOCK_COMBO_HOLD");          // R14 combo HOLD + attack clock
    note(lr::rearm(g_prediction_fault120),"WEAPON_PREDICTION");
    note(lr::rearm(g_torch_fault62),"TORCH_GESTURE");
    note(lr::rearm(g_phys_fault6698),"PHYSICAL_WEAPONS_HOLSTERS");               // its input owner is re-published by the next frame
    if(at>0){char line[480]{};std::snprintf(line,sizeof(line),"R96 LOAD_REARM after save/level load:%s",m);diagnostic_trace::Write(line);}
}
static void NativeRhythmTick178() noexcept {
    RearmAfterLoadR96(); // R96: first native frame after a load
    // Service setup/retirement even when the native combat tick returns early.
    // Register on instant-pickup readiness or an explicit F9 activation edge;
    // failed setup is not retried every frame. Each synchronous Use rechecks
    // current admission, with no Lua boolean/timestamp permission lease.
    UpdateSmallPickupReaderR24((kcd2vr::static_grab_r89::enabled.load(std::memory_order_acquire)||
        (kcd2vr::lab58::active(kcd2vr::lab58::Sub::PropWake)&&kcd2vr::lab58::f9Rung.load()==3))&&
        !kcd2vr::lab58::uiBlocked.load());
    SmallContactTickR24(std::uint64_t(GetTickCount64())); // fresh bare-hand static contact; no impulse/pickup
    static_grab_r89n::Tick(std::uint64_t(GetTickCount64())); // R90 (R89 pickup): small props in the hand
    pickup_use_fallback_r120n::Tick(std::uint64_t(GetTickCount64())); // R120: instant pickup when the game did not run Use
    // C59 cleanup runs on this same native owner even after combat is OFF.
    (void)gore32n::ServiceVisualLeases59(std::uint64_t(GetTickCount64()));
    featureProbe59c6::Tick();
    DrainVisibleFailure(std::uint64_t(GetTickCount64())); // DS-F17: game-thread visible-failure drain (bounded, one Lua call max)
    DrainConstraintOps60(std::uint64_t(GetTickCount64())); // DS-F20: one physics op per tick, executor seam
    (void)kcd2vr::physics::grab_producer60::SetSourcePredicateD41(&GrabSourceCurrentD41);
    kcd2vr::physics::grab_input60::resolveEntity=&EntityById6694;
    kcd2vr::physics::grab_input60::GameThreadTick(std::uint64_t(GetTickCount64())); // current target entity, never Henry's owner pointer
    // DS-F99: the touch-reaction flesh seam (one-time fill; the gore's own
    // MaterialEffects route at the touch point - the same exact pins as the
    // impale blood; never called when retail/pins fail).
    {static bool touchSeamFilled99=false;
     if(!touchSeamFilled99){kcd2vr::touch_react_service60::fleshEffect=&gore32n::TouchFleshFill61;touchSeamFilled99=true;}}
    kcd2vr::touch_react_service60::drain(std::uint64_t(GetTickCount64())); // DS-F105: one touch fire per native frame (haptic + flesh effect on the game thread)
    // C61: static wall/prop effects are not ordinary melee damage and must drain even when F4 is disabled.
    ConsumeEnvironmentFx178r22c1();
    if(!PhysicalCombat177Enabled()){
        CorpseImpaleProducerR93();ImpalePartTick59c7g(); // R93R3: the single impale option owns its eligible current-part path.
        lab58n::Tick(std::uint64_t(GetTickCount64())); // D84: corpse selection and physics recording are independent of combat.
        g_native_busy_tick178.store(0,std::memory_order_release);return;}
    ReadScope101R2 reads;void* manager=g_henry_item_attach_manager.load();void* player=nullptr;NativeOwner104 owner{};nc104::Observed st{};
    const bool ok=PhysVft6698(manager,0x3EC9958)&&Read(static_cast<unsigned char*>(manager)+0x38,player)&&NativeResolve104(player,owner)&&NativeObserve104(owner,st);
    // V66.178R4: every observation (also a FAILED one) is published with tick + epoch, so a stale
    // idle value can never look current. R44C1: ordinary attack/recovery is not
    // an input veto. Preserve a held request and protected/unclassified actions;
    // the original input selector arbitrates an ordinary follow-up attempt.
    g_native_state_ok178.store(ok,std::memory_order_release);g_native_state_epoch178.store(g_pose_epoch6689.load(),std::memory_order_release);
    g_native_phase178.store(ok?st.phase:-1);g_native_type178.store(ok?st.type:-1);
    if(!ok)g_native_busy_tick178.store(GetTickCount64(),std::memory_order_release);
    if(ok){
        const bool guardOwnerH14=nc104::blockMetadataH14(st)?NativeHeldGuardOwnerH14(owner):GuardOwnedByUs59c7e(owner.state);
        g_native_busy178.store(kcd2vr::native_admission147::preparationBlockedOwnedGuard56(st,guardOwnerH14),std::memory_order_release);
        g_native_busy_tick178.store(GetTickCount64(),std::memory_order_release);
        if(cc::active(GetTickCount64()))CombatTargetProbe178(owner);
    }
    static std::uint64_t lastSerial=0;PhysFrame6698 f{};
    if(g_phys_frame6698.read(f)&&f.output.strike178.serial&&f.output.strike178.serial!=lastSerial){
        lastSerial=f.output.strike178.serial;const auto& k=f.output.strike178;
        static const char* reasons[]{"ACCEPTED","RHYTHM_NATIVE_BUSY","RHYTHM_MIN_INTERVAL","TOO_WEAK","NATIVE_STATE_UNKNOWN"};
        int cSide=-1;for(int s=0;s<2;++s)if(f.output.blocked[s]&&f.output.contact119[s].known){cSide=s;break;}
        const auto* cw=cSide>=0?&f.output.contact119[cSide].native172:nullptr; // V66.178R13: what the blade was stopped by
        char m[560]{};std::snprintf(m,sizeof(m),"V66.178 STRIKE serial=%llu kind=%d zone=%d accepted=%d reason=%s usefulSpeed=%.2f energyJ=%.2f refJ=%.2f scale=%.2f nativeOk=%d phase=%d type=%d busy=%d contactSide=%d contactBody=%s contactPart=%d contactSurface=%d contactEntity=%u opponent=%d units=MOD_DESIGN_ESTIMATES nativeDamage=NOT_CHANGED rateLimiter=DESIGN_NOT_NATIVE_COMBO_TIMING",
            (unsigned long long)k.serial,k.kind,k.zone,k.accepted?1:0,reasons[std::clamp(k.reason,0,4)],k.tipSpeed,k.energy,k.reference,k.scale,
            g_native_state_ok178.load()?1:0,g_native_phase178.load(),g_native_type178.load(),g_native_busy178.load()?1:0,
            cSide,cw?kcd2vr::npc_contact172::name(cw->body):"NONE",cw?cw->part:-1,cw?cw->surfaceIdx:-1,cw?cw->entityId:0u,cSide>=0&&f.output.contact119[cSide].currentOpponent175?1:0);
        diagnostic_trace::Write(m);
        if(k.accepted){int flinchSide=-1;for(int s=0;s<2;++s)if(f.output.blocked[s]&&f.output.contact119[s].native172.entityId){flinchSide=s;break;}
            if(flinchSide>=0&&!combat_diag_probe::GeometryHitsEnabled178r27())FlinchRequest178r11(f,flinchSide,k.scale); // rung 6: the native hit reaction replaces our flinch
            // World effects use completed contact events even when the wall
            // aborts the swing (R24 impact path / R22C1). Keep the body-effect path separate.
            int fxSide=-1;for(int s=0;s<2;++s){const auto& c=f.output.contact119[s];
                if(f.output.blocked[s]&&c.plane&&(c.native172.body==kcd2vr::npc_contact172::Body::Living||
                   c.native172.body==kcd2vr::npc_contact172::Body::Articulated)){fxSide=s;break;}}
            if(fxSide>=0)ContactFxRequest178r12(f,fxSide,k.kind,k.scale);}
    }
    // V66.178R30 (R28 review F02): gameplay contact admission is separate from optional effects. The geometry
    // hit consumes the IMMUTABLE impact record captured at the impact sample (entity/part/material/point/technique/
    // owner identity of THAT contact), never the current ray; it runs whether or not contact effects are enabled.
    // F9 observes fresh physical impacts; R4 delivers only to the same current
    // exact articulated collider, with no-point dispatch and a per-body budget.
    {static kcd2vr::impact59c6::Seen lastImpulse[2]{};PhysFrame6698 pf{};
     if(combat_diag_probe::ContactImpulse596()&&g_phys_frame6698.read(pf))for(int s=0;s<2;++s){
        const auto& r=pf.output.impactRecord178r30[s];if(!r.entityId||!r.collider||!lastImpulse[s].take(r))continue;
        const auto now=std::uint64_t(GetTickCount64());if(now<r.tick||now-r.tick>160||r.epoch!=g_pose_epoch6689.load()||r.request!=g_weapon_request_epoch6694.load()||
           pf.profiles[s].key!=r.key||pf.profiles[s].generation!=r.generation||!pf.output.active[s]||!pf.output.owned[s])continue;
        const float pos[3]{float(r.point.x),float(r.point.y),float(r.point.z)},nrm[3]{float(r.normal.x),float(r.normal.y),float(r.normal.z)};
        combat_diag_probe::ContactImpulseRequest596(r.entityId,r.part,r.body,pos,nrm,float(r.speed),r.tick,r.collider,r.key,r.generation,r.epoch,r.request);}}
    #include "subsystems/physics/pose_solver_combat_h29.inl"
    // C42: Articulated is a physics class, NOT a native death predicate.
    // Retire C41's unqualified corpse-cut producer. Only a confirmed native
    // lethal receipt may request head removal through decap59c7e::Evaluate.
    // C43T corpse cut (back, with a real death proof): a strong CUT on the head/neck surface of a body the GAME reported killed
    // (gore32::KnownDead from the native lethal receipt, same entity identity). body type is not checked - it never proved death.
    {static kcd2vr::impact59c6::Seen lastCorpse43[2]{};static kcd2vr::gore32::ReceiptBudget60 g_limb60_budget;PhysFrame6698 cf{};
     if(kcd2vr::gore32::decap.load(std::memory_order_relaxed)&&g_phys_frame6698.read(cf))for(int s=0;s<2;++s){
        const auto& r=cf.output.impactRecord178r30[s];
        if(!r.valid||!r.entityId||!r.collider||r.part<0||!lastCorpse43[s].take(r))continue;
        // D41: the corpse path has no native receipt to revalidate its owner.
        // Retired impact records must not cut after weapon/epoch/hand changes.
        if(r.epoch!=g_pose_epoch6689.load()||r.request!=g_weapon_request_epoch6694.load()||
           cf.profiles[s].key!=r.key||cf.profiles[s].generation!=r.generation||!cf.output.active[s]||!cf.output.owned[s])continue;
        const bool cuttingD41=kcd2vr::strike_feature_d41::canCut(r.feature59c7g,cf.profiles[s].classId);
        auto cutD56=r.goreCutD56;cutD56.power=kcd2vr::gore_cut_d56::powerRatio(cutD56,cf.profiles[s].weight,
            g_strike_reference_j59c9.load(std::memory_order_relaxed));
        const auto cutWhyD56=kcd2vr::gore_cut_d56::owned(cutD56,r.gesture59c3,r.key,r.generation,r.epoch,r.request,r.sample,r.tick)?
            kcd2vr::gore_cut_d56::qualify(cutD56,kcd2vr::gore32::decapMinPower.load()):kcd2vr::gore_cut_d56::Why::Identity;
        const auto now=std::uint64_t(GetTickCount64());if(now<r.tick||now-r.tick>200)continue;
        void* entity=FlinchEntityById178r11(r.entityId);const auto ident=reinterpret_cast<std::uintptr_t>(entity);
        if(!entity||!kcd2vr::gore32::KnownDead(r.entityId,ident,now))continue; // living (or unknown): never, silently
        if(kcd2vr::gore32::wasLethalGestureD56({r.entityId,ident,r.gesture59c3,r.key,r.generation,r.epoch,r.request}))continue;
        const bool seen=gore32n::CorpseCutAcked61(r.entityId,ident); // real head hide ACK, not candidate publication
        char m[560]{};const double need=kcd2vr::gore32::decapCorpseMinSpeed.load(std::memory_order_relaxed);
        // C47: the struck physics part names the joint (head / its parent the neck / named neck joints) first, the head/neck surface second
        const auto pj47=(!seen&&r.kind==0&&cuttingD41&&r.speed>=need)?gore32n::PartHeadNeck47(CharacterForEntity6689(entity),r.part):gore32n::PartJoint47{};
        const bool joint47=gore32n::IsHeadNeckJoint47(pj47);
        const bool limbD56=kcd2vr::gorepak50::dismember.load()&&kcd2vr::gore32::LimbFromMaterial60(r.bodyMaterial)!=kcd2vr::gorepak50::Cut::None;
        const char* why=seen&&!limbD56?"ALREADY_DONE":cutWhyD56!=kcd2vr::gore_cut_d56::Why::Ok?kcd2vr::gore_cut_d56::name(cutWhyD56):r.kind!=0?"NOT_A_CUT":!cuttingD41?"NO_PROVEN_CUTTING_FEATURE":!(r.speed>=need)?"TOO_SLOW":!joint47&&!kcd2vr::gore32::headMaterial(r.bodyMaterial)&&!limbD56?"NOT_HEAD_NECK_OR_QUALIFIED_LIMB_MATERIAL":nullptr;
        // C60: the legacy raw-part observer remains read-only; its unproven
        // namespace is never used by D56 material-based dispatch below.
        // The observer also reports an
        // already head-hidden corpse. F3 capture-gated, session-keyed saturating
        // budget; the classification is a CANDIDATE, never advertised anatomy.
        if(r.kind==0&&cuttingD41&&r.speed>=kcd2vr::gore32::limbCorpseMinSpeed.load(std::memory_order_relaxed)&&
           diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)&&
           g_limb60_budget.admit(diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load())){
            const auto limb60=gore32n::LimbCutReceipt60(CharacterForEntity6689(entity),r.part);
            if(limb60!=kcd2vr::gorepak50::Cut::None){std::snprintf(m,sizeof(m),"C60 LIMB_CORPSE candidateCut=%s living=0 target=%u speed=%.2f rawPart=%d key=%016llX generation=%llu epoch=%llu request=%llu sample=%llu tick=%llu collider=%016llX sourceMatch=%d candidateJoint=UNPROVEN event=NONE visual=UNPROVEN",
                kcd2vr::gorepak50::luaName(limb60),r.entityId,r.speed,r.part,(unsigned long long)r.key,(unsigned long long)r.generation,(unsigned long long)r.epoch,(unsigned long long)r.request,
                (unsigned long long)r.sample,(unsigned long long)r.tick,(unsigned long long)r.collider,
                (r.key==cf.profiles[s].key&&r.generation==cf.profiles[s].generation)?1:0);diagnostic_trace::Write(m);}
        }
        if(why){if(!seen){std::snprintf(m,sizeof(m),"C43T DECAP_CORPSE skipped target=%u reason=%s speed=%.2f need=%.2f bodyMaterial=%d part=%d joint=%s headJoint=%d neckJoint=%d",r.entityId,why,r.speed,need,r.bodyMaterial,r.part,pj47.joint,pj47.head,pj47.neck);diagnostic_trace::Write(m);}continue;}
        kcd2vr::gore32::CutCandidate c{};c.serial=0x43000000ull+now;c.tick=now;c.target=r.entityId;c.identity=ident;
        c.point[0]=float(r.point.x);c.point[1]=float(r.point.y);c.point[2]=float(r.point.z);c.power=float(cutD56.power);c.cutD56=cutD56;c.bodyMaterial=r.bodyMaterial;c.corpse=true;c.bodyEpoch42=r.epoch;c.request42=r.request;c.part44=r.part;c.rawPart47=r.part;
        (void)kcd2vr::gore32::lethalCut.publish(c); // tentative; overwritten/refused requests must not mark this target done
        diagnostic_trace::Write("C61 CORPSE_CUT_PENDING nativeAnswer=NOT_YET_CONSUMED");
        std::snprintf(m,sizeof(m),"C43T DECAP_CORPSE cut target=%u speed=%.2f bodyMaterial=%d part=%d joint=%s deadByGame=1 -> head removal",r.entityId,r.speed,r.bodyMaterial,r.part,pj47.joint);diagnostic_trace::Write(m);}}
    CorpseImpaleProducerR93(); // same N2 position in the ordinary combat path
    {static kcd2vr::impact59c6::Seen lastTouch43[2]{};PhysFrame6698 tf{}; // D44 online: raw-part receipts require a current mapped joint before skeletal injection
     if(touch43::Enabled()&&g_phys_frame6698.read(tf))for(int s=0;s<2;++s){const auto& r=tf.output.impactRecord178r30[s]; // W5: also on at F9 rung 3
        if(!r.valid||r.body!=int(kcd2vr::npc_contact172::Body::Living)||!lastTouch43[s].take(r))continue;
        const double p[3]{r.point.x,r.point.y,r.point.z},nrm[3]{r.normal.x,r.normal.y,r.normal.z};(void)touch43::OnContact(s,r.entityId,r.part,p,nrm,r.speed,r.tick);}}
    // F9 owns this mapped FREE-HAND touch request. The manual weapon-touch
    // checkbox does not own it; raw physics parts are never skeleton joints.
    touch43::HandContactsH9(std::uint64_t(GetTickCount64()));
    {static kcd2vr::impact59c6::Seen lastLimb60[2]{};static kcd2vr::gore32::ReceiptBudget60 g_limb60_living;PhysFrame6698 lf{}; // C60: strong cut on a LIVING/ARTICULATED limb -> receipt only (no event, no damage)
     if(kcd2vr::gore32::decap.load(std::memory_order_relaxed)&&g_phys_frame6698.read(lf))for(int s=0;s<2;++s){const auto& r=lf.output.impactRecord178r30[s];
        if(!r.valid||!r.entityId||!lastLimb60[s].take(r))continue;
        const bool living=r.body==int(kcd2vr::npc_contact172::Body::Living),articulated=r.body==int(kcd2vr::npc_contact172::Body::Articulated);
        if((!living&&!articulated)||r.kind!=0||r.speed<kcd2vr::gore32::limbCorpseMinSpeed.load(std::memory_order_relaxed))continue;
        const auto now=std::uint64_t(GetTickCount64());if(now<r.tick||now-r.tick>200)continue;
        if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)||
           !g_limb60_living.admit(diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load()))continue;
        void* entity=FlinchEntityById178r11(r.entityId);if(!entity||entity==g_last_player_entity.load(std::memory_order_acquire))continue;
        if(kcd2vr::gore32::KnownDead(r.entityId,reinterpret_cast<std::uintptr_t>(entity),now))continue; // game-reported dead: the corpse lane owns it
        void* ci=CharacterForEntity6689(entity);if(!ci)continue;
        const auto lc=gore32n::LimbCutReceipt60(ci,r.part);
        if(lc==kcd2vr::gorepak50::Cut::None)continue;
        char m60[420]{};std::snprintf(m60,sizeof(m60),"C60 LIMB_LIVING candidateCut=%s bodyKind=%s target=%u speed=%.2f rawPart=%d key=%016llX generation=%llu epoch=%llu request=%llu sample=%llu tick=%llu collider=%016llX sourceMatch=%d candidateJoint=UNPROVEN vitality=UNKNOWN nativeDeathEvidence=NONE event=NONE visual=UNPROVEN",
            kcd2vr::gorepak50::luaName(lc),living?"LIVING":"ARTICULATED",r.entityId,r.speed,r.part,(unsigned long long)r.key,(unsigned long long)r.generation,(unsigned long long)r.epoch,(unsigned long long)r.request,
            (unsigned long long)r.sample,(unsigned long long)r.tick,(unsigned long long)r.collider,
            (r.key==lf.profiles[s].key&&r.generation==lf.profiles[s].generation)?1:0);diagnostic_trace::Write(m60);}}
    {static kcd2vr::impact59c6::Seen lastGeom[2]{};PhysFrame6698 gf{};
     if(combat_diag_probe::GeometryHitsEnabled178r27()&&g_phys_frame6698.read(gf))for(int s=0;s<2;++s){
        if(gf.output.solverOwnedT4[s]||heldT4::HasUnretiredCollider())continue; // one physical contact owner, never a parallel predicted ray
        const auto& r=gf.output.impactRecord178r30[s];namespace n=kcd2vr::npc_contact172;
        if(!lastGeom[s].take(r))continue;
        const bool body=r.entityId&&(r.body==int(n::Body::Living)||r.body==int(n::Body::Articulated));
        // V66.178R43: the opponent's WEAPON/SHIELD item is a contact too - a blow into his block, adjudicated natively.
        const bool weaponItem43=!body&&r.entityId&&WeaponClash178r27(r.entityId)!=nullptr;
        // R59.13 (user): light pokes should still land, with small effect. The contact floor drops
        // 1.5 -> 0.8 m/s; below that a resting/sliding blade is not an attack.
        if((!body&&!weaponItem43)||r.speed<0.8)continue;
        if(body&&r.kind==1&&r.gesture59c3)kcd2vr::contact58c1::noteThrust96(r.gesture59c3,r.entityId,r.speed,r.tick,ImpactTipDepthR97(gf,s,r.point,r.normal),
            kcd2vr::impale_corpse_r15::axialCos(r.goreCutD56.totalSpeed,r.goreCutD56.transverseSpeed)); // R96: fastest touch of this thrust (impale threshold); R97: + tip depth (blade-contact pierce proof); R155: + along/total of the point at that touch
        const auto now=std::uint64_t(GetTickCount64());
        const bool fresh=now>=r.tick&&now-r.tick<=250&&r.key&&gf.profiles[s].key==r.key&&gf.profiles[s].generation==r.generation&&
            gf.input.bodyEpoch==r.epoch&&gf.request==r.request&&gf.output.active[s]&&gf.output.owned[s];
        if(fresh&&body){void* eD56=FlinchEntityById178r11(r.entityId);void* ciD56=eD56?CharacterForEntity6689(eD56):nullptr;
            if(eD56&&ciD56)(void)gore32n::EligibilityD56(r.entityId,reinterpret_cast<std::uintptr_t>(eD56),ciD56,now);}
        const float pos[3]{float(r.point.x),float(r.point.y),float(r.point.z)};// R59.13: strength curve from the contact speed. 0.8 m/s poke -> 0.05 (minimum native energy,
        // scratch damage), 3 m/s -> ~0.37, 7 m/s full stroke -> 1.0. Native armour/zone/perks still decide.
        // R59.16 energy from the weapon, not a speed curve: E = 1/2 m_eff v^2 at the struck feature. m_eff ~ 0.55 of the weapon
        // weight for a cut (mass distributed along a rotating blade), 0.9 for a thrust (axial). Reference: a 1.6 kg sword cut at
        // 7 m/s (~21.6 J) = full native strength 1.0; a light poke still lands at >= 0.05 (R59.13 policy). The game's RPG then
        // applies Henry's skills, the weapon stats and the armour - nothing is scaled twice here.
        const double weight5916=std::clamp(gf.profiles[s].weight,0.3,6.0);
        const double meff5916=weight5916*(r.kind==1?0.9:0.55);
        const double joules5916=0.5*meff5916*r.speed*r.speed;
        const float energy=float(std::clamp(joules5916/21.6,0.05,1.0));
        kcd2vr::contact59c7g::Metadata metadata59c7g{r.collider,r.bodyMaterial,r.feature59c7g,r.twoHands59c7g,gf.profiles[s].classId,gf.profiles[1-s].shield176(),r.originZone47};
        metadata59c7g.goreCutD56=r.goreCutD56;
        metadata59c7g.goreCutD56.power=kcd2vr::gore_cut_d56::powerRatio(r.goreCutD56,weight5916,
            g_strike_reference_j59c9.load(std::memory_order_relaxed));
        combat_diag_probe::NoteImpactPower59c8(r.sample,float(joules5916/double(g_strike_reference_j59c9.load(std::memory_order_relaxed))),r.twoHands59c7g); // R59.16C9C: reference = Miki's own full stroke (INI VrStrikeReferenceJ; 13 Oct median ratio 3.5 x 21.6 J) // C7G merge: support at this impact, not a later pose
        const bool queued=fresh&&(weaponItem43?combat_diag_probe::GeometryWeaponContactSubmit178r43(r.entityId,pos,energy,r.kind,r.part,r.surfaceIdx,r.tick,r.key,r.generation,r.epoch,r.request,r.sample,r.gesture59c3)
            :combat_diag_probe::GeometryHitSubmit178r27(r.entityId,pos,energy,r.kind,r.part,r.surfaceIdx,r.tick,r.key,r.generation,r.epoch,r.request,r.sample,r.gesture59c3,&metadata59c7g));
        char m[300]{};std::snprintf(m,sizeof(m),"V66.178R30 GEOM_IMPACT side=%c speed=%.2f energy=%.2f kind=%s entity=%u body=%s part=%d surface=%d pos=(%.2f,%.2f,%.2f) record=IMMUTABLE fresh=%d queued=%d",s?'L':'R',r.speed,energy,r.kind==0?"CUT":r.kind==1?"THRUST":"UNKNOWN",r.entityId,n::name(n::Body(r.body)),r.part,r.surfaceIdx,pos[0],pos[1],pos[2],fresh?1:0,queued?1:0);diagnostic_trace::Write(m);}}
    // V66.178R24: wall/object sparks from the IMPACT itself (speed >= 2 m/s against a known surface), not from an
    // accepted native strike. A sword hitting a wall is not an attack; the material effect is the game's own
    // row for that weapon class/surface. One effect per impact (dedupe by impact tick; 150 ms limiter inside).
    {static std::uint64_t lastImpact[2]{};PhysFrame6698 xf{};
     if(g_contact_fx178r12.load(std::memory_order_relaxed)&&g_phys_frame6698.read(xf))for(int s=0;s<2;++s){
        const auto t=xf.output.impactEventTick178r24[s];const double v=xf.output.impactEventSpeed178r24[s];
        if(!t||t==lastImpact[s]||v<2.0)continue;lastImpact[s]=t;
        const auto& rec=xf.output.impactRecord178r30[s];const auto& c=xf.output.contact119[s];namespace n=kcd2vr::npc_contact172;
        const bool body=rec.valid&&rec.tick==t?(rec.entityId&&(rec.body==int(n::Body::Living)||rec.body==int(n::Body::Articulated))):
            (c.native172.entityId&&(c.native172.body==n::Body::Living||c.native172.body==n::Body::Articulated));
        if(body&&!combat_diag_probe::VrStateConsequence596())continue; // bodies: geometry hit (rung 2) or the strike-driven native path (rung 1); effects come from the game's hit
        // R59.6: the VR backend's direct hit route plays no native material effect - the body impact gets the game's own blood row here.
        const int kind=int(xf.output.melee118.kind==melee118::Kind::Thrust?melee118::Kind::Thrust:melee118::Kind::Cut);
        // V66.178R27 hybrid stage 3: what did the blade hit? A non-body entity with a weapon/shield NAME is the enemy's
        // guard - a clash. The blade is already stopped physically; add the clash haptic (both hands when supported)
        // and name it in the log so blocks can be judged without guessing from the material row.
        const std::uint32_t hitEntity=rec.valid&&rec.tick==t?rec.entityId:c.native172.entityId;
        const char* clash27=WeaponClash178r27(hitEntity);
        if(clash27){const auto now27=std::uint64_t(GetTickCount64());const float amp27=float(std::clamp(.45+v/10.0,.45,1.0));
            (void)kcd2vr::haptics101::publish_event(s,xf.profiles[s].key,amp27,80,now27);
            if(xf.output.support[s]&&xf.output.owned[1-s])(void)kcd2vr::haptics101::publish_event(1-s,xf.profiles[s].key,amp27*.7f,80,now27);}
        char m[300]{};std::snprintf(m,sizeof(m),"V66.178R24 CONTACT_IMPACT side=%c speed=%.2f plane=%d surfaceIdx=%d entity=%u class=%s -> fx%s",s?'L':'R',v,c.plane?1:0,c.native172.surfaceIdx,hitEntity,clash27?clash27:"WORLD_OR_UNNAMED",clash27?" +CLASH_HAPTIC":"");diagnostic_trace::Write(m);
        ContactFxRequest178r12(xf,s,kind,std::clamp(v/8.0,0.25,1.0));}}
    // Bounded observation of the actual contacted actor, on the native thread.
    // Four read-only samples per accepted receipt; never an NPC/world scan and
    // never a physics command. A body-type transition is not a flinch outcome.
    {
        static kcd2vr::contact58c1::Receipt watched{};static void* identity=nullptr;static unsigned stage=4;
        constexpr unsigned offsets[]{0,100,300,750};const auto now=std::uint64_t(GetTickCount64());
        const bool on=kcd2vr::contact58c1::experiment.load(std::memory_order_acquire)&&g_gameplay_allowed.load()&&
            !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&NpcContract172(WhBase());
        if(!on){watched={};identity=nullptr;stage=4;}
        else{
            kcd2vr::contact58c1::Receipt r{};
            if(kcd2vr::contact58c1::latest.read(r)&&r.serial&&r.serial!=watched.serial&&r.tick&&now>=r.tick&&now-r.tick<=80&&
               r.accepted&&r.applied&&r.fields&&r.epoch==g_pose_epoch6689.load()&&r.request==g_weapon_request_epoch6694.load()){
                watched=r;identity=FlinchEntityById178r11(r.target);stage=identity?0:4;
            }
            if(stage<4){
                if(watched.epoch!=g_pose_epoch6689.load()||watched.request!=g_weapon_request_epoch6694.load()||
                   now<watched.tick||now-watched.tick>1000){stage=4;}
                else if(now-watched.tick>=offsets[stage]){
                    void* entity=FlinchEntityById178r11(watched.target);std::uintptr_t body=0,vft=0;
                    const bool same=entity&&entity==identity;
                    const auto read=[](std::uintptr_t p,auto& v){return Read(reinterpret_cast<void*>(p),v);};
                    const bool known=same&&kcd2vr::npc_contact172::bodyFromEntity(WhBase(),reinterpret_cast<std::uintptr_t>(entity),read,body)&&
                        body&&read(body,vft);
                    const auto type=known&&vft>=WhBase()?kcd2vr::npc_contact172::classify(vft-WhBase()):kcd2vr::npc_contact172::Body::Unknown;
                    char line[320]{};std::snprintf(line,sizeof(line),"V66.178R58C1 CONTACT_BODY serial=%llu target=%u stage=%u ageMs=%llu sameEntity=%d body=%s nativeDeath=%d nativeDispatch=%d physicsWrite=NONE activeRagdoll=UNPROVEN",
                        (unsigned long long)watched.serial,watched.target,stage,(unsigned long long)(now-watched.tick),same,
                        kcd2vr::npc_contact172::name(type),watched.death,watched.dispatch);diagnostic_trace::Write(line);
                    ++stage;if(!same)stage=4;
                }
            }
        }
    }
    // One read-only articulated-part query while the visual latch is active.
    ImpalePartTick59c7g();
    lab58n::Tick(std::uint64_t(GetTickCount64()));
    lab_prop58n::Tick(std::uint64_t(GetTickCount64()));
    gore32n::Tick(std::uint64_t(GetTickCount64())); // C32: decapitation (idle unless a head just came off)
    blood49n::Tick(std::uint64_t(GetTickCount64())); // C49 BLOODY FIGHTS (idle unless Henry just wounded/killed someone)

}
void RequestGoreVisualRestore59() noexcept {gore32n::RequestRestoreVisuals59();}
void SetNpcFlinch178r11(bool on) noexcept {g_flinch_enabled178r11.store(on,std::memory_order_relaxed);}
void SetHoverAffordance178r14(int side,bool on) noexcept {if(side>=0&&side<2)g_hover_affordance178r14[side].store(on,std::memory_order_relaxed);}
void SetNativeHandover178r14(bool on) noexcept {g_native_handover178r14.store(on,std::memory_order_relaxed);}
void SetGrabMode60(int mode) noexcept {
    const int selected=mode==2?2:0;
    g_grab_mode60.store(selected,std::memory_order_release);
    kcd2vr::grab43::clear(0);kcd2vr::grab43::clear(1); // retire presentation requests across grab-mode changes
    action_time_owner59c11::SetPickupHold62(selected==2);
}
int GrabMode60() noexcept {return g_grab_mode60.load(std::memory_order_acquire);}
void SetGrabHandStays43(bool on) noexcept {kcd2vr::grab43::handStays.store(on,std::memory_order_relaxed);} // C43
bool GrabHandStays43() noexcept {return kcd2vr::grab43::handStays.load(std::memory_order_relaxed);}
#include "vrik/pickup_visual_use_d44.inl"

bool HoverHaptic178r14(int side,std::uint64_t key) noexcept {return kcd2vr::haptics101::publish_event(side,key,.25f,25,std::uint64_t(GetTickCount64()));}
void SetContactEffects178r12(bool on) noexcept {
    // Effects must not depend on whether the F3 recorder has ever been enabled.
    const bool ready=on&&(retail_build112::Ready(WhBase())||retail_build112::Verify(WhBase()))&&ArmMaterial116();
    g_contact_fx178r12.store(on,std::memory_order_relaxed);
    if(on&&!ready)diagnostic_trace::Write("V66.178R22C1 CONTACT_FX unavailable until exact material service is armed; setting retained, calls fail closed");
}
// C49 BLOODY FIGHTS: the extra blood uses the game's MaterialEffects service exactly as the contact effects do, so it needs
// the same read-only material contract armed (normally already armed by VrContactEffects, default on). Fails closed otherwise.
void SetBloodyFights49(bool on) noexcept {
    const bool was=kcd2vr::blood49::on.exchange(on,std::memory_order_relaxed);
    const bool ready=on&&(retail_build112::Ready(WhBase())||retail_build112::Verify(WhBase()))&&ArmMaterial116();
    if(on&&!ready)diagnostic_trace::Write("C49 BLOOD route=NO_MATERIAL_EFFECTS (material service not armed); setting kept, extra particles fail closed");
    if(was!=on){char m[120]{};std::snprintf(m,sizeof(m),"C49 BLOODY_FIGHTS=%s materialEffects=%s",on?"ON":"OFF",ready?"READY":on?"NOT_READY":"-");diagnostic_trace::Write(m);}
}
bool BloodyFights49() noexcept {return kcd2vr::blood49::on.load(std::memory_order_relaxed);}
void SetVrCombatCore178(float minScale,unsigned minIntervalMs,bool blockWhileActive) noexcept {
    g_strike_block_active178.store(blockWhileActive);
    g_strike_min_scale178.store(std::clamp(minScale,0.0f,1.0f));g_strike_min_interval178.store(std::min(minIntervalMs,3000u));
    char m[160]{};std::snprintf(m,sizeof(m),"V66.178 VR_COMBAT_CORE strikeMinScale=%.2f strikeMinIntervalMs=%u (F4 rung 2)",double(g_strike_min_scale178.load()),g_strike_min_interval178.load());
    diagnostic_trace::Write(m);
}
void ServiceDrawDiagnosticsD77() noexcept { DrawNativeServiceD77(); }
void DrainCopiedDiagnosticsH47(std::uint64_t now) noexcept {
    // These queues contain value copies only: no native calls, live object
    // dereferences, tracking acquisition or gameplay writes on this worker.
    unsigned lines=0;
    const auto write=[&](const char* line){
        diagnostic_trace::Write(line);
        // Keep the shared queue moving while a complete F7 skeleton expands;
        // do not thin finger records or defer native input/cost processing.
        if(++lines%64==0)diagnostic_trace::Drain();
    };
    kcd2vr::diagnostics::vrik_h29::drain(now,write);
    kcd2vr::diagnostics::hit_chain_c54r3::drain(now,write);
}
void PhysicalNativeFrameTick6698() noexcept {
    kcd2vr::held_map_m1::NativeFrameTick(); // own paper only; no work when experiment is off
    g_stage_thread596=GetCurrentThreadId();g_stage_tick596=GetTickCount64();
    if(kcd2vr::diagnostics::motion_d68::requested()){
        namespace md=kcd2vr::diagnostics::motion_d68;md::Event e{};
        static thread_local md::Cadence cadence;
        if(md::begin(e,md::NativeFrame)&&md::inWindow(e)){
            if(cadence.due(e.session,e.ns)){
                e.sourceTick=g_stage_tick596;e.thread=g_stage_thread596;
                // No renderer/native frame identifier is proven at this callback.
                // frame=0 is UNKNOWN, not a combat dispatch counter or XR ordinal.
                e.flags=1u;(void)md::push(e);
            }else md::noteCadenceSkip(md::NativeFrame);
        }
    }
    ReadScope101R2 pages;PerfScope101R2 timing(3);
    const auto censusNow59=std::uint64_t(GetTickCount64());
    if(kcd2vr::diagnostics::frame_d30::anyOn())kcd2vr::diagnostics::frame_d30::gameFrame(std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()),censusNow59); // D30 F2 FRAME
    ItemCensusImpl178r19(diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,censusNow59),
        diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,censusNow59));
    IntegratedFistSelection178r22c2();
    {PerfScope101R2 stage(8);NativeFrameBegin104();ExternalPrepareDrain176();}
    {PerfScope101R2 stage(9);NativeSpecialTick168();}
    {PerfScope101R2 stage(13);NativeRhythmTick178();}
    {PerfScope101R2 stage(10);FistNativeTick101();}
    {NativeBlockGuardFrameH14 blockGuardFrameH14;
     {PerfScope101R2 stage(11);NativeGuardTick112();}
     {PerfScope101R2 stage(12);PhysNativeTickImpl6698();}}
    {std::unique_lock<std::recursive_mutex> arrow(g_manual_arrow_mutex64,std::try_to_lock);
     if(arrow.owns_lock())ManualBowTick596();}
    CrossbowRifleTickH21(); // F11=2 parked native ranged request; bow route is independent
    if(g_hidden_bow_entity59c14.load()&&(g_weapon_control_mode.load()<4||g_hide_stowed_bow59c14.load(std::memory_order_relaxed)==0||!StowedBowImmersive54())){
        PhysTryLock6698 visibilityLock;if(visibilityLock.owned)StowedBowShow59c14(nullptr,std::uint64_t(GetTickCount64())); // this preference owns only the stowed bow hide
    }
    RangedDrawTickD72(std::uint64_t(GetTickCount64())); // current native ownership and exact visibility-debt retirement
    DrawGuardTickR120(std::uint64_t(GetTickCount64())); // R120: drawn weapon shown at its first VR placement or after 150 ms
    UtilityNativeTick54(std::uint64_t(GetTickCount64()));
    ChestDaggerTick54(std::uint64_t(GetTickCount64())); // edge-only carried-item discovery
    StowedBowAttachmentTick59c15(std::uint64_t(GetTickCount64())); // R59.16C10R15: the stowed bow is a character attachment, not an entity
    if(g_actionOwnerNativeTick59c15)g_actionOwnerNativeTick59c15(); // R59.16C10R15
}
#include "subsystems/physics/self_contact_service.inl" // M2_OWNED_SOURCE
#include "subsystems/ranged_reload/stowed_bow_controls.inl" // M2_OWNED_SOURCE
#include "subsystems/smithing/native_sources_h42.inl"
#include "subsystems/vrik/native_crouch_r93.inl"
void PhysicalNativeFrameEnd104() noexcept {
    PublishNativeCrouchR93();
    kcd2vr::activity_input163::AlchemySelectionNativeFrameEnd38(); // C37I1: publish only completed native selection
    {ReadScope101R2 pages;PublishMinigameSourcesH42();StagedPropTick596();} // station reads share current frame cache
    if(!NativeSelected104()&&!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64())){g_native_owner104.publish(NativeOwner104{});return;}
    ReadScope101R2 pages;PerfScope101R2 timing(5);NativeFrameEnd104();
}
HeldPhysicsSourceT4 HeldPhysicsSourceReasonT4() noexcept {return g_native_held_reason_t4.load(std::memory_order_acquire);}
const char* HeldPhysicsSourceNameT4(HeldPhysicsSourceT4 reason) noexcept {
    static constexpr const char* names[]{"DISABLED","NO_EQUIPMENT","LEFT_OCCUPIED","NO_HELD_SWORD","ITEM_LINK","NATIVE_PROFILE",
        "ATTACHMENT","NATIVE_PROXY_SUSPENSION","HENRY_PROXY","NATIVE_WORLD_POSE","CURRENT_CONTEXT","LEGACY_RETIRING","AWAIT_GRIP","READY"};
    const auto i=static_cast<unsigned>(reason);return i<std::size(names)?names[i]:"UNKNOWN";
}
PhysicalAttachmentResult101 PhysicalAttachmentUpdate101(void* self,void* attachment) noexcept {return PhysAttachmentResultImpl101(self,attachment);}
bool FistInputOwned101() noexcept {return FistSelected101();}
bool F4GestureInputOwned176() noexcept {
    if(!CombatExperiment175Enabled()||diagnostics::lab::NativeReference()||kcd2vr::vr_combat111::ClinchInput55c1(GetTickCount64()))return false;
    // Read the existing copied equipment publication only. A cold/lost physical
    // frame must not reopen stock melee input while F4 is selected. Ranged
    // classes retain their established native/physical-bow trigger policy.
    PhysEquipment6698 e{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_phys_equipment6698.read(e)||!e.valid||!e.tick||now<e.tick||now-e.tick>80||
       e.request!=g_weapon_request_epoch6694.load()||e.epoch!=g_pose_epoch6689.load()||
       !e.manager||e.manager!=g_henry_item_attach_manager.load()||e.count<0||e.count>8)return true;
    for(unsigned side=0;side<2;++side){
        if(!e.handKnown[side]||!e.handKey[side])continue;
        const int i=e.heldIndex[side];if(i<0||i>=e.count)continue;
        const int c=e.items[i].profile.classId;
        // Exact existing retail class IDs: bow, crossbow and firearm families.
        // Shields (8/17) remain in the gesture-controlled melee category.
        if(c==9||c==10||c==13||c==14||c==15)return false;
    }
    return true;
}
// R138: Henry's LEFT hand has no item in the copied equipment publication (stale/unknown = free: an empty-hand fist is
// the common case; a weapon in the left hand keeps the R122 LT rule unchanged).
static bool LeftHandItemFreeR138() noexcept {
    PhysEquipment6698 e{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_phys_equipment6698.read(e)||!e.valid||!e.tick||now<e.tick||now-e.tick>80)return true;
    return !(e.handKnown[1]&&e.handKey[1]);
}
float FistTransportTrigger101(int side,float raw,bool tracked,bool menu,float grip) noexcept {
    // Only called by the mod input worker. During UI, clear no held state and
    // let UI receive its trigger; gameplay resumes only after a tracked release.
    static bool latch[2]{};if(side<0||side>1)return raw;
    // Both physical weapon rungs require a fresh melee/bow owner. Merely
    // selecting rung 6 must not swallow gun/crossbow or native-reference input
    // now that the same policy precedes XI as well as the fallback transport.
    const int mode=g_weapon_control_mode.load();
    // H4: physical boxing owns BOTH triggers, including the approach to a
    // closed fist. Waiting for left Grip>.70 let LT cross the native .4 input
    // threshold first, arming attack_special/kick before the fist was closed.
    // Native reference, menus and authored clinches retain the existing exits;
    // explicit R3 has its independent kick route. Positional fists own guard.
    const bool fistConsumes=FistSelected101()&&FistModePermits178r22c2();
    const bool consume=!diagnostics::lab::NativeReference()&&
        (F4GestureInputOwned176()||fistConsumes||(side==0&&FullWeaponMode120(mode)&&PhysicalWeaponInputOwned6698()));
    if(consume)latch[side]=true;
    // R122 (Miki 6 Nov: "hold LT for stealth - choke out or stab from behind - LT is not working at all"): the
    // consume above (F4 gesture melee / boxing) ate the physical LT in ALL gameplay, so the game never saw
    // xi_triggerl_btn and its LT-hold actions outside a fight (combat_base stealth_kill, knock_out, butcher,
    // horse_pulldown: holdTriggerDelay 0.25 s) could not fire. A LEFT trigger press that STARTS while Henry is
    // known out of combat (native inCombat flag = 0) now reaches the game as LT until that press is released.
    // Fights (inCombat 1 or unknown) keep the old ownership: positional guard, fist guard and the LT-hold kick
    // stay mod-owned. Block/attack_special from a raw LT stay suppressed in F4 by CombatInputDetour101.
    // If the mod's own block/kick takes LT during such a press, the raw LT is cut for the rest of that press
    // (attack_xi178r8 publishes one LT-up first), so the mod's block release is never hidden. Per press only.
    // R138 (vrik/fist_lt_r138.h): a closed EMPTY left fist is never native LT - R122 handed the fist-closing press
    // to the game (its block), every punch then became the game's unarmed kick (log 7 Nov 04:14:41 / 04:15:29:
    // R122 LT_PRESS native=1, then all punches phase=256 type=4). open-hand LT outside a fight still passes after 100 ms.
    static kcd2vr::fist_lt_r138::State ltR138{};
    bool passR122=false;
    if(side==1){
        namespace fl=kcd2vr::fist_lt_r138;
        fl::Input in{};in.raw=raw;in.grip=grip;in.now=std::uint64_t(GetTickCount64());in.leftHandFree=LeftHandItemFreeR138();
        in.consume=consume;in.menu=menu;in.tracked=tracked;in.ltPublished=kcd2vr::attack_xi178r8::ltPublished.load(std::memory_order_acquire);
        const bool fresh=raw>=fl::kDown&&(!ltR138.down||ltR138.pending);
        in.inCombat=fresh?combat_diag_probe::HenryInCombat178r31(in.now):-1; // read only while a press is being decided
        const auto ev=fl::step(ltR138,in);
        if(ev==fl::Event::Native||ev==fl::Event::Fist){char m[240]{};std::snprintf(m,sizeof(m),
            "R122 LT_PRESS native=%d inCombat=%d modOwnedLt=%d fist=%d (native LT outside a fight: stealth kill / knock out / butcher; R138 a closed empty fist stays the mod's)",
            ev==fl::Event::Native?1:0,in.inCombat,in.ltPublished?1:0,(in.grip>=fl::kFistGrip&&in.leftHandFree)?1:0);diagnostic_trace::Write(m);}
        else if(ev==fl::Event::Cut)diagnostic_trace::Write("R138 LT_PRESS native=0 reason=FIST_CLOSED_DURING_NATIVE_LT (rest of this press is the mod's: fist guard / punches)");
        passR122=ltR138.pass;
        kcd2vr::attack_xi178r8::rawLtR122.store(passR122&&!menu,std::memory_order_release);
    }
    if(menu)return raw;
    // Paired clinch owns both presentation and native controls. Do not retain
    // the F4 trigger quarantine after handing the visible body to that action.
    if(CombatExperiment175Enabled()&&kcd2vr::vr_combat111::ClinchInput55c1(GetTickCount64())){latch[side]=false;return raw;}
    if(side==1&&passR122)return raw; // R122: native LT outside a fight (latch stays; it retires as before)
    if(latch[side]){if(tracked&&raw<.20f&&!consume)latch[side]=false;return 0;}
    return raw;
}
void ObserveR3Combat101(bool down) noexcept {
    if(g_kick_r3_down101.exchange(down)==down)return;
    const auto now=std::uint64_t(GetTickCount64());
    if(!cc::active(now))return;
    cc::Event e{};e.kind=cc::Kind::R3;e.tick=now;e.flags=down?1u:0u;
    e.mode=HolsterCombatDiagMode();e.id=g_census_mode.load();e.layer=g_weapon_control_mode.load();cc::push(e);
}
bool PhysicalAttachmentPending6698() noexcept {return g_phys_pending6698.load()!=0;}
bool PhysicalWeaponInputOwned6698() noexcept {
    const auto now=std::uint64_t(GetTickCount64()),tick=g_phys_input_tick6698.load();
    return BowHandlingMode59c9(g_weapon_control_mode.load())&&!g_phys_fault6698.load()&&g_gameplay_allowed.load()&&g_phys_input6698.load()&&now>=tick&&now-tick<=100;
}
bool PhysicalWeaponConsumesRightTrigger139() noexcept {
    // V66.139: trigger transport ownership must be decided identically for
    // native-XI and scan-code/mouse fallback.  The physical melee solver does
    // not use RT; only the physical bow lifecycle consumes the right trigger.
    // This keeps RT usable for stock/native semantics on swords, crossbows and
    // firearms while preventing a physical bow press from leaking into KCD2 a
    // second time.  Native-reference diagnostics never consume gameplay RT.
    if(!PhysicalWeaponInputOwned6698()||diagnostics::lab::NativeReference())return false;
    PhysFrame6698 f{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_phys_frame6698.read(f)||!f.input.sample||now<f.input.tick||now-f.input.tick>100)return false;
    // V66.160: the manual bow (quiver gesture) also owns RT so a physical string
    // pull can never leak a second native draw/fire through the game transport.
    return f.output.active[1]&&f.profiles[1].bow()&&(ph::physical_bow175(f.input)||ph::bow_uses_trigger175(f.input));
}
static void CancelComparisonLease114() noexcept;
void ShutdownPhysicalWeapons6698() noexcept {
    ExternalPrepareForget176();CancelProceduralKick176();diagnostics::parry176::Clear();
    kcd2vr::contact176::policy.fetch_add(1,std::memory_order_acq_rel);
    g_f11_test167.store(0,std::memory_order_release);g_f11_lease167={};
    // No gameplay getters/cancel calls on the unload/Mod worker. Mode-off is
    // consumed by the retained native pump; final process teardown is native.
    g_phys_abort6698=true;g_phys_input6698=false;g_phys_armed6698=false;
    if(g_phys_charge_hook6698){(void)g_phys_charge_hook6698->disable();g_phys_charge_hook6698.reset();}
    g_phys_charge_original6698=nullptr;
    if(g_phys_native_sweep_hook110){(void)g_phys_native_sweep_hook110->disable();g_phys_native_sweep_hook110.reset();}
    g_phys_native_sweep_original110=nullptr;
    combat_diag_probe::ClearContactObserver115(&ObserveNativeContact114);
}

static thread_local bool g_pose_callback_active6689=false;
struct PoseCallbackScope6689 { PoseCallbackScope6689(){g_pose_callback_active6689=true;} ~PoseCallbackScope6689(){g_pose_callback_active6689=false;} };

// Whole incoming native hands are sampled before the sole VR writer. A final
// counterpart is copied only from this same completed/restored transaction.
namespace {
static kcd2vr::diagnostics::hands28::Local FingerLocal28(const QuatT28& q) noexcept {
    return {PQ(QFromQuatT(q)),PV(FromT(q))};
}


static void ReadItemWorld28(kcd2vr::diagnostics::hands28::Record& r,void* entity,const ph::Pose* wrist) noexcept {
    Matrix34V6550 matrix{};
    if(!GetEntityWorldTmV6552(entity,r.entity,matrix)||!PhysEntityIdentity135(entity,r.entity)||
        !kcd2vr::diagnostics::hands28::current(r,GetTickCount64())||r.epoch!=g_pose_epoch6689.load())return;
    r.actualMatrixKnown=true;std::copy_n(matrix.m,12,r.actualMatrix.begin());
    kcd2vr::grasp13::Pose pose{};float m[12]{};std::copy_n(matrix.m,12,m);
    if(!kcd2vr::grasp13::fromMatrix(m,pose))return;
    const ph::Pose actual{{pose.q[0],pose.q[1],pose.q[2],pose.q[3]},{pose.p[0],pose.p[1],pose.p[2]}};
    r.actualItemKnown=true;r.actualItem={actual.q,actual.p};
    if(wrist){const auto relative=ph::compose(ph::inverse(*wrist),actual);r.actualWrist={relative.q,relative.p};}
}
static void HandAttachmentReceipt28(void* self,void* attachment,std::uint64_t now) noexcept {
    namespace d=kcd2vr::diagnostics::hands28;if(!d::active(now)||!self||!attachment)return;
    void* manager=nullptr;const char* name=nullptr;char label[40]{};
    if(!PhysVft6698(attachment,0x3A657C0)||!Read(static_cast<unsigned char*>(attachment)+0x28,manager)||!manager||
        manager!=g_henry_char_attachment_manager.load(std::memory_order_acquire)||!Read(static_cast<unsigned char*>(attachment)+0x10,name)||!name)return;
    for(unsigned i=0;i+1<sizeof(label);++i){char c=0;if(!Read(name+i,c)||!c)break;label[i]=c;}
    const int slot=!std::strcmp(label,"entity_right")?0:!std::strcmp(label,"entity_left")?1:!std::strcmp(label,"hand_right")?2:!std::strcmp(label,"hand_left")?3:-1;
    if(slot<0)return;
    static std::mutex mutex;std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);if(!lock.owns_lock())return;
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::HandReference)].load();
    static d::Gate gates[4];if(!gates[slot].due(session,now,reinterpret_cast<std::uintptr_t>(self)^reinterpret_cast<std::uintptr_t>(attachment)))return;
    d::Record r{};r.kind=d::Record::Kind::Item;r.itemRoute=3;r.tick=now;r.session=session;r.side=slot&1;r.epoch=g_pose_epoch6689.load();r.request=g_weapon_request_epoch6694.load();
    void* character=nullptr;if(!Read(static_cast<unsigned char*>(manager)+0x70,character)||!PhysVft6698(character,0x40602F8)||!Read(static_cast<unsigned char*>(self)+8,r.entity)||!r.entity)return;
    r.character=reinterpret_cast<std::uintptr_t>(character);r.attachment=reinterpret_cast<std::uintptr_t>(attachment);
    void* entity=EntityById6694(r.entity);if(!entity)return;r.item=reinterpret_cast<std::uintptr_t>(entity);
    kcd2vr::grasp13::Binding grasp{};
    if(g_graspBinding13&&g_graspBinding13(grasp)&&grasp.attached&&grasp.entity==r.entity&&grasp.side==r.side&&grasp.character==r.character&&grasp.epoch==r.epoch){r.key=grasp.key;r.keyKnown=r.key!=0;r.token=grasp.token;r.nativeItemPointer=grasp.item;}
    CommittedFrame6695 body{};ph::Pose wrist{};const ph::Pose* pairedWrist=nullptr;
    if(g_committed_pose_valid6689.load(std::memory_order_acquire)&&g_committed_frame6695.read(body)&&body.valid[r.side]&&kcd2vr::pose_core::fresh(body.stamp,r.epoch,r.character,now)){
        const auto& tool=body.tools59c10[r.side];QuatT28 relative{};std::int16_t joint=-1;
        if(tool.valid&&tool.nativeObservedKnown28&&tool.socket.self==self&&tool.socket.attachment==attachment&&tool.socket.id==r.entity&&tool.socket.character==character&&
            Read(static_cast<unsigned char*>(attachment)+0x78,joint)&&joint==tool.socket.joint&&Read(static_cast<unsigned char*>(attachment)+0x9C,relative)&&!std::memcmp(&relative,&tool.socket.relative,sizeof(relative))){
            r.sample=body.stamp.sample;r.sourceTick=body.stamp.tick;r.joint=joint;r.nativeItem=FingerLocal28(tool.nativeObserved28);r.itemPair=true;
            ModelWorld6692 world{};if(PhysAttachmentWorld164(attachment,r.character,world)){
                r.nativeWrist=FingerLocal28(RelativeFromAbsolute(body.native[r.side],tool.nativeObserved28));r.nativeWrist.p=r.nativeWrist.p*world.scale;
                wrist=PhysCurrentWorld164(PhysPose6698(body.allowed[r.side]),world);pairedWrist=&wrist;
            }else r.itemPair=false;
        }
    }
    ReadItemWorld28(r,entity,pairedWrist);
    std::uint32_t stillBound=0;void* stillCharacter=nullptr;
    if(!Read(static_cast<unsigned char*>(self)+8,stillBound)||stillBound!=r.entity||!Read(static_cast<unsigned char*>(manager)+0x70,stillCharacter)||stillCharacter!=character||
       r.epoch!=g_pose_epoch6689.load()||r.request!=g_weapon_request_epoch6694.load())return;
    if(r.keyKnown){kcd2vr::grasp13::Binding live{};if(!g_graspBinding13||!g_graspBinding13(live)||!kcd2vr::grasp13::same(grasp,live)||!live.attached){r.key=0;r.keyKnown=false;r.token=0;r.nativeItemPointer=0;}}
    (void)d::push(r,GetTickCount64());
}

static void TraceToolItem28(const CommittedFrame6695& body,int side,void* self,void* attachment,const ModelWorld6692& world,const float* wanted,const kcd2vr::grasp13::Binding* binding) noexcept {
    namespace d=kcd2vr::diagnostics::hands28;if(!d::selected())return;const auto now=std::uint64_t(GetTickCount64());if(!d::active(now)||side<0||side>1)return;
    const auto& tool=body.tools59c10[side];const auto& socket=tool.socket;
    if(!tool.valid||socket.self!=self||socket.attachment!=attachment||!tool.nativeObservedKnown28||!body.valid[side]||
       !kcd2vr::pose_core::fresh(body.stamp,g_pose_epoch6689.load(),reinterpret_cast<std::uintptr_t>(socket.character),now))return;
    static std::mutex mutex;std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);if(!lock.owns_lock())return;
    static d::Gate gates[2];const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::HandReference)].load();
    if(!gates[side].due(session,now,socket.request^socket.id))return;
    d::Record r{};r.kind=d::Record::Kind::Item;r.itemRoute=1;r.tick=now;r.session=session;r.character=body.stamp.actor;r.epoch=body.stamp.epoch;r.request=socket.request;
    r.sample=body.stamp.sample;r.sourceTick=body.stamp.tick;r.side=side;r.entity=socket.id;r.item=reinterpret_cast<std::uintptr_t>(socket.entity);r.attachment=reinterpret_cast<std::uintptr_t>(attachment);r.token=socket.request;r.joint=socket.joint;
    r.key=binding?binding->key:socket.smith36.key;r.keyKnown=r.key!=0;r.generation=0;r.generationKnown=false;
    r.nativeItemPointer=binding?binding->item:0;
    r.nativeItem=FingerLocal28(tool.nativeObserved28);r.nativeWrist=FingerLocal28(RelativeFromAbsolute(body.native[side],tool.nativeObserved28));r.nativeWrist.p=r.nativeWrist.p*world.scale;
    const ph::Pose placedWorld=PhysWorld6698(wanted),wristWorld=PhysCurrentWorld164(PhysPose6698(body.allowed[side]),world);
    const auto relative=ph::compose(ph::inverse(wristWorld),placedWorld);
    r.finalItem={placedWorld.q,placedWorld.p};r.finalWrist={relative.q,relative.p};r.itemPair=true;r.placed=true;
    ReadItemWorld28(r,socket.entity,&wristWorld);
    if(binding){kcd2vr::grasp13::Binding live{};if(!g_graspBinding13||!g_graspBinding13(live)||!kcd2vr::grasp13::same(*binding,live)||!live.attached)return;}
    if(r.epoch!=g_pose_epoch6689.load())return;(void)d::push(r,GetTickCount64());
}
static void TracePhysicalItem28(const PhysFrame6698& frame,int side,void* self,void* attachment,void* entity,std::uint32_t id,
    std::uint64_t generation,const ModelWorld6692& world,const float* wanted,bool checked,bool readback) noexcept {
    namespace d=kcd2vr::diagnostics::hands28;if(!d::selected())return;const auto now=std::uint64_t(GetTickCount64());if(!d::active(now)||side<0||side>1)return;
    static std::mutex mutex;std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);if(!lock.owns_lock())return;
    static d::Gate gates[2];const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::HandReference)].load();
    const auto key=frame.profiles[side].key;if(!gates[side].due(session,now,key^generation))return;
    d::Record r{};r.kind=d::Record::Kind::Item;r.itemRoute=2;r.tick=now;r.session=session;r.character=frame.characterT23;r.epoch=frame.input.bodyEpoch;r.request=frame.request;
    r.sample=frame.input.sample;r.sourceTick=frame.input.tick;r.side=side;r.entity=id;r.key=key;r.keyKnown=key!=0;r.generation=generation;r.generationKnown=true;r.item=reinterpret_cast<std::uintptr_t>(entity);r.attachment=reinterpret_cast<std::uintptr_t>(attachment);
    r.placed=true;r.readbackKnown=checked;r.readback=readback;
    CommittedFrame6695 body{};PhysGripBank6698 grips{};
    if(g_committed_pose_valid6689.load(std::memory_order_acquire)&&g_committed_frame6695.read(body)&&body.valid[side]&&body.stamp.actor==r.character&&body.stamp.epoch==r.epoch&&body.stamp.sample==r.sample&&body.stamp.tick==r.sourceTick&&
       kcd2vr::pose_core::fresh(body.stamp,g_pose_epoch6689.load(),r.character,now)&&g_phys_grips6698.read(grips)){
        for(const auto& g:grips.grip)if(g.key==key&&g.generation==generation&&g.nativeStamp.actor==r.character&&g.nativeStamp.epoch==r.epoch&&g.nativeStamp.sample==r.sample&&g.nativeStamp.tick==r.sourceTick&&g.nativeSocket126.valid&&g.nativeSocket126.key==key&&g.nativeSocket126.generation==generation&&g.nativeSocket126.character==reinterpret_cast<void*>(r.character)){
            r.joint=g.nativeSocket126.joint;r.nativeItem={g.nativeWeapon.q,g.nativeWeapon.p};
            const auto relative=ph::compose(ph::inverse(PhysPose6698(body.native[side])),g.nativeWeapon);r.nativeWrist={relative.q,relative.p*world.scale};
            const auto wrist=PhysCurrentWorld164(PhysPose6698(body.allowed[side]),world);const auto allowed=ph::compose(ph::inverse(wrist),PhysWorld6698(wanted));r.finalWrist={allowed.q,allowed.p};r.itemPair=true;break;
        }
    }
    const auto final=PhysWorld6698(wanted);r.finalItem={final.q,final.p};
    const auto actualWrist=ph::compose(final,ph::inverse({r.finalWrist.q,r.finalWrist.p}));
    ReadItemWorld28(r,entity,r.itemPair?&actualWrist:nullptr);
    if(r.epoch!=g_pose_epoch6689.load()||r.request!=g_weapon_request_epoch6694.load())return;(void)d::push(r,GetTickCount64());(void)self;
}

} // copied item diagnostics share the existing private attachment owners
static std::optional<kcd2vr::diagnostics::hands28::Record> BeginFingerOutput28(
    void* pose,void* owner,void* character,void* anim,std::uint32_t count,const RigMap& rig,
    PoseGetFn get,const TrackingSnapshot& ts,std::uint64_t now) noexcept {
    namespace d=kcd2vr::diagnostics::hands28;namespace cap=kcd2vr::diagnostics::capture168;
    if(!d::active(now))return std::nullopt;
    const auto session=cap::gate.session[unsigned(cap::Domain::HandReference)].load(std::memory_order_acquire);
    animation_filter::HandPlaybackOutput28 clips{};const bool clipKnown=animation_filter::ReadHandPlaybackOutput28(anim,clips);
    static d::Gate cadence; // existing pose callback mutex owns this observer only
    const auto signature=clips.signature^reinterpret_cast<std::uintptr_t>(character)^g_pose_epoch6689.load();
    const auto reason=cadence.due(session,now,signature);if(!reason)return std::nullopt;
    d::Record r{};r.tick=now;r.session=session;r.epoch=g_pose_epoch6689.load();r.request=g_weapon_request_epoch6694.load();
    r.sample=ts.sequence;r.sourceTick=ts.tick;r.timeNs=ts.timeNs;r.trackingEpoch=ts.source_epoch;
    r.character=reinterpret_cast<std::uintptr_t>(character);r.skeleton=reinterpret_cast<std::uintptr_t>(owner);r.pose=reinterpret_cast<std::uintptr_t>(pose);r.anim=reinterpret_cast<std::uintptr_t>(anim);r.reason=reason;
    r.clipKnown=clipKnown;r.clipTick=clips.tick;r.clipSerial=clips.serial;r.clipCount=clips.count;r.clipTruncated=clips.truncated;
    for(unsigned i=0;i<r.clipCount&&i<12;++i){const auto& a=clips.clips[i];auto& b=r.clips[i];b.id=a.id;b.layer=a.layer;b.fifo=a.fifo;b.segment=a.segment;b.phase=a.segmentTime;std::memcpy(b.name,a.name,sizeof(b.name));}
    if(!get||!pose||!owner||!character||!count||count>kPoseCapacity6689)return r;
    r.rig=14695981039346656037ull;
    for(int side=0;side<2;++side){auto& h=r.hand[side];h.wrist=side?rig.hand_l:rig.hand_r;
        h.grip=side?ts.left_grip:ts.right_grip;h.trigger=side?ts.left_trigger:ts.right_trigger;
        std::array<FingerChainV6528,5> chains{};
        h.map=GetCachedFingerChainsV6546(pose,get,owner,count,rig,side!=0,chains);
        QuatT28 wrist{};if(h.wrist<0||unsigned(h.wrist)>=count||!ReadQuat(get,pose,h.wrist,wrist)||!ValidPose6689(wrist)){h.map=false;continue;}
        h.nativeWrist=FingerLocal28(wrist);
        if(!h.map)continue;
        for(int f=0;f<5;++f)for(int k=0;k<chains[f].count&&h.count<32;++k){const int id=chains[f].id[k];auto& j=h.joints[h.count++];j.id=id;j.digit=f;j.segment=k;
            QuatT28 absolute{},parent{};j.nativeKnown=id>=0&&unsigned(id)<count&&ReadParentDirect(owner,id,j.parent)&&j.parent>=0&&unsigned(j.parent)<count&&
                ReadQuat(get,pose,id,absolute)&&ReadQuat(get,pose,j.parent,parent)&&ValidPose6689(absolute)&&ValidPose6689(parent);
            if(j.nativeKnown)j.native=FingerLocal28(RelativeFromAbsolute(parent,absolute));
            r.rig^=std::uint64_t(std::uint32_t(id))|(std::uint64_t(std::uint32_t(j.parent))<<32);r.rig*=1099511628211ull;
        }
    }
    return r;
}
static void FinishFingerOutput28(kcd2vr::diagnostics::hands28::Record& r,const CoherentTransaction6689& tx) noexcept {
    namespace d=kcd2vr::diagnostics::hands28;
    if(!d::current(r,GetTickCount64()))return;
    const auto& s=g_pose6689;
    const bool same=r.character==reinterpret_cast<std::uintptr_t>(s.character)&&r.skeleton==reinterpret_cast<std::uintptr_t>(s.owner)&&r.epoch==g_pose_epoch6689.load()&&r.request==g_weapon_request_epoch6694.load();
    r.committed=tx.committed;r.restored=tx.restored;r.published=same&&r.originalReturned&&r.epoch==tx.epoch&&tx.committed&&tx.restored&&tx.fingerPublished28&&!g_pose_restore_fault6689.load();
    const bool refs=same&&s.ready;
    const auto& a=g_finger_acquire6694;const bool acquisition=refs&&a.owner==s.owner&&a.character==s.character&&a.count==s.count&&a.bodyEpoch59==r.epoch;
    for(int side=0;side<2;++side){auto& h=r.hand[side];h.referenceOwner=refs&&s.fingerMap[side]&&s.fingerCount[side]==h.count;
        h.trialRequested=tx.fingerTrialRequestedH7;h.writer=r.published&&s.freeObserved28[side]&&tx.traceArmH29[side].fingerCall&&tx.traceArmH29[side].fingerOk;
        if(h.writer&&tx.fingerContext28[side])h.trigger=tx.traceArmH29[side].trigger; // actual free-writer input, including visual-only dialogue
        h.objectWriter=r.published&&s.objectObserved28[side];h.objectFollow=h.objectWriter&&s.objectFollow28[side];
        if(refs){const auto& e=s.objectCandidate[side];h.objectReady=e.ready;h.objectSince=e.since;h.objectLastSample=e.lastSample;h.objectSequence=e.sequence;}
        h.trialActive=h.writer&&tx.fingerTrialH7[side];h.heldKnown=tx.fingerContext28[side]&&tx.traceArmH29[side].itemState>=0;h.heldKey=tx.traceArmH29[side].heldKey;h.objectKey=refs?s.objectFingerKey[side]:0;h.generation=refs?s.objectFingerGeneration[side]:0;
        if(h.referenceOwner){h.open=s.open[side].ready;h.closed=s.closed[side].ready;h.trialReady=s.nativeCurveClosedH7[side].ready;
            h.openSequence=s.open[side].sequence;h.closedSequence=s.closed[side].sequence;h.trialSequence=s.nativeCurveClosedH7[side].sequence;
            if(r.published&&h.wrist>=0&&h.wrist<int(s.count))h.finalWrist=FingerLocal28(s.work[h.wrist]);
        }
        if(acquisition){h.cacheOnly=a.loadedCacheH7;h.selectedPhase=a.selected[side];const int ids[]{0,1+side,3+side};
            for(int i=0;i<3;++i){h.curveId[i]=a.clip[ids[i]].id;h.curveFlags[i]=a.clip[ids[i]].flags;}}
        for(int f=0;f<5;++f){h.inputCurl[f]=FreeFingerInputH7(f,float(h.grip),float(h.trigger),h.trialActive);
            h.contact[f]=h.writer&&s.freeClampObserved28[side][f];h.appliedCurl[f]=h.writer?s.freeCurlObserved28[side][f]:h.inputCurl[f];}
        for(int n=0;n<h.count;++n){auto& j=h.joints[n];if(!h.referenceOwner||s.fingerIds[side][n]!=j.id||s.parent[j.id]!=j.parent){h.referenceOwner=false;continue;}
            if(acquisition)j.crc=a.crc[side][n];
            if(h.open)j.open=FingerLocal28(s.open[side].local[n]);if(h.closed)j.closed=FingerLocal28(s.closed[side].local[n]);if(h.trialReady)j.trial=FingerLocal28(s.nativeCurveClosedH7[side].local[n]);
            j.finalKnown=r.published&&j.id>=0&&j.id<int(s.count)&&j.parent>=0&&j.parent<int(s.count);
            if(j.finalKnown)j.final=FingerLocal28(RelativeFromAbsolute(s.work[j.parent],s.work[j.id]));
            if(h.writer&&h.open&&h.closed){pc::FingerCurve103 curve{};FreeFingerCurveH7(s,side,j.digit,tx.fingerTrialH7[side]?tx.fingerOpenBiasH7[side]:s.hoverW178r14[side],h.trialActive,curve);
                j.curveOpen={curve.open[j.segment],curve.offset[j.segment]};j.curveClosed={curve.closed[j.segment],pc::finger_offset103(curve,unsigned(j.segment),1.)};}
        }
    }
    // Candidates retain one complete observed hand, never per-joint extrema.
    static std::uint64_t peakOwner=0,peakClip=0;static double peak[2]{};
    const auto owner=r.session^r.character^r.skeleton^r.epoch^r.rig;
    std::uint64_t clipIdentity=0;for(unsigned i=0;i<r.clipCount;++i)clipIdentity=clipIdentity*131+std::uint32_t(r.clips[i].id)+std::uint64_t(r.clips[i].layer)*65537;
    if(peakOwner!=owner||peakClip!=clipIdentity){peak[0]=peak[1]=0;peakOwner=owner;peakClip=clipIdentity;}
    for(auto& h:r.hand){unsigned n=0;double score=0;
        if(h.open&&h.referenceOwner)for(int j=0;j<h.count;++j)if(h.joints[j].nativeKnown){score+=pc::angle(h.joints[j].native.q,h.joints[j].open.q);++n;}
        h.nativeClosureScore=n?score/n:0;const auto side=&h-r.hand;
        if(n&&h.nativeClosureScore>peak[side]+.005){h.peakCandidate=true;peak[side]=h.nativeClosureScore;}
    }
    (void)d::push(r,GetTickCount64());
}

// F3-only observation at the exact-Henry callback. Native output is opaque bytes.
struct KickOutputProbe23 {
    bool mapKnown=false,called=false,returned=false;
    std::uint64_t tick=0,session=0,epoch=0;void* playerPose=nullptr;void* pose=nullptr;
    void* owner=nullptr;void* character=nullptr;void* anim=nullptr;void* output=nullptr;
    std::uint32_t count=0;int ids[7]{-1,-1,-1,-1,-1,-1,-1};
    QuatT28 joints[3][7]{};unsigned jointMask[3]{};
    std::uint64_t outputHash[2][7]{};unsigned outputMask[2]{};bool descriptorSame[2]{};
    animation_filter::KickPlaybackOutput23 clip{};bool clipKnown=false;
    b168::SpecialRequest request{};bool requestKnown=false;
};
static bool KickOutputActive23(const KickOutputProbe23& r) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    return cap::active(cap::Domain::Combat,std::uint64_t(GetTickCount64()))&&
        cap::gate.session[unsigned(cap::Domain::Combat)].load(std::memory_order_acquire)==r.session&&
        g_pose_epoch6689.load(std::memory_order_acquire)==r.epoch&&
        g_last_player_pose.load(std::memory_order_acquire)==r.playerPose;
}
static void KickOutputJoints23(KickOutputProbe23& r,unsigned phase,PoseGetFn get) noexcept {
    if(!KickOutputActive23(r)||!r.mapKnown||!get)return;
    for(unsigned j=0;j<7;++j){
        const QuatT28* value=nullptr;
        __try {value=get(r.pose,r.ids[j]);} __except(EXCEPTION_EXECUTE_HANDLER){value=nullptr;}
        QuatT28 q{};if(!value||!Read(value,q))continue;
        bool finite=true;for(float v:q.q)finite=finite&&std::isfinite(v);for(float v:q.t)finite=finite&&std::isfinite(v);
        if(finite){r.joints[phase][j]=q;r.jointMask[phase]|=1u<<j;}
    }
}
static void KickOutputPalette23(KickOutputProbe23& r,unsigned phase,void* desc) noexcept {
    if(!KickOutputActive23(r)||!r.mapKnown||!desc)return;
    void *pose=nullptr,*owner=nullptr,*charPlus8=nullptr,*out=nullptr;
    r.descriptorSame[phase]=Read(static_cast<unsigned char*>(desc),pose)&&
        Read(static_cast<unsigned char*>(desc)+8,owner)&&Read(static_cast<unsigned char*>(desc)+0x10,charPlus8)&&
        Read(static_cast<unsigned char*>(desc)+0x20,out)&&pose==r.pose&&owner==r.owner&&
        charPlus8==static_cast<unsigned char*>(r.character)+8&&out&&out==r.output;
    if(!r.descriptorSame[phase])return;
    // Exact 5FE2F0 writes 32 bytes per joint. This hashes bytes only; no DQ layout,
    // mesh-instance visibility or successful GPU consumption is inferred.
    for(unsigned j=0;j<7;++j){unsigned char bytes[32]{};
        if(!Read(static_cast<unsigned char*>(out)+std::size_t(r.ids[j])*32,bytes))continue;
        std::uint64_t h=14695981039346656037ull;
        for(unsigned char b:bytes){h^=b;h*=1099511628211ull;}
        r.outputHash[phase][j]=h;r.outputMask[phase]|=1u<<j;
    }
}
static std::optional<KickOutputProbe23> BeginKickOutput23(void* pose,void* owner,void* character,void* anim,
                                          void* output,std::uint32_t count,const RigMap& rig,PoseGetFn get,std::uint64_t now) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    // All additional pose/cache/palette reads and formatting stay behind F3.
    if(!cap::active(cap::Domain::Combat,now)||!animation_filter::NativeKickActive175())return std::nullopt;
    const auto session=cap::gate.session[unsigned(cap::Domain::Combat)].load(std::memory_order_acquire);
    static std::uint64_t last=0,lastSession=0; // existing callback's pose mutex owns cadence
    if(lastSession==session&&last&&now>=last&&now-last<500)return std::nullopt;
    last=now;lastSession=session;
    KickOutputProbe23 r{};r.tick=now;r.session=session;r.epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    r.playerPose=g_last_player_pose.load(std::memory_order_acquire);r.pose=pose;r.owner=owner;
    r.character=character;r.anim=anim;r.output=output;r.count=count;
    const auto& s=g_pose6689;
    r.mapKnown=s.ready&&s.owner==owner&&s.character==character&&s.count==count&&s.legsReady128&&
        s.rig.pelvis==rig.pelvis&&count<=kPoseCapacity6689;
    if(r.mapKnown){
        const int ids[]{rig.pelvis,s.leg[0].hip,s.leg[0].knee,s.leg[0].ankle,s.leg[1].hip,s.leg[1].knee,s.leg[1].ankle};
        for(unsigned j=0;j<7;++j){r.ids[j]=ids[j];if(ids[j]<0||unsigned(ids[j])>=count)r.mapKnown=false;}
    }
    r.clipKnown=animation_filter::ReadKickPlaybackOutput23(anim,r.clip);
    r.requestKnown=g_special_request168.read(r.request);
    KickOutputJoints23(r,0,get);return r;
}
static void FinishKickOutput23(const KickOutputProbe23& r,bool committed) noexcept {
    if(!KickOutputActive23(r))return;
    char line[2048]{};std::snprintf(line,sizeof(line),
        "V66.90-PLAYBACK KICK_OUTPUT tick=%llu F3session=%llu epoch=%llu playerPose=%p pose=%p CI=%p anim=%p skeleton=%p joints=%u mapKnown=%d committed=%d originalCalled=%d originalReturned=%d output=%p descSame=%d/%d jointMasks=%02X/%02X/%02X paletteMasks=%02X/%02X clipKnown=%d clipTick=%llu clipSeq=%llu id=%d layer=%d fifo=%d segment=%d segmentTime=%.6f name=\"%s\" lastR3Known=%d lastR3Serial=%llu lastR3Tick=%llu lastR3Actor=%p lastR3Player=%p lastR3Epoch=%llu requestAssociation=UNPROVEN clipWeight=UNKNOWN meshVisibility=UNOBSERVED",
        (unsigned long long)r.tick,(unsigned long long)r.session,(unsigned long long)r.epoch,r.playerPose,r.pose,r.character,r.anim,r.owner,r.count,r.mapKnown,committed,r.called,r.returned,r.output,
        r.descriptorSame[0],r.descriptorSame[1],r.jointMask[0],r.jointMask[1],r.jointMask[2],r.outputMask[0],r.outputMask[1],
        r.clipKnown,(unsigned long long)r.clip.tick,(unsigned long long)r.clip.serial,r.clip.id,r.clip.layer,r.clip.fifo,r.clip.segment,double(r.clip.segmentTime),r.clipKnown?r.clip.name:"UNKNOWN",
        r.requestKnown,(unsigned long long)r.request.serial,(unsigned long long)r.request.tick,reinterpret_cast<void*>(r.request.actor),reinterpret_cast<void*>(r.request.player),(unsigned long long)r.request.epoch);
    diagnostic_trace::Write(line);
    if(!r.mapKnown)return;
    const char* names[]{"PELVIS","R_HIP","R_KNEE","R_ANKLE","L_HIP","L_KNEE","L_ANKLE"};
    for(unsigned j=0;j<7;++j){const auto& a=r.joints[0][j];const auto& b=r.joints[1][j];const auto& c=r.joints[2][j];
        std::snprintf(line,sizeof(line),
            "V66.90-PLAYBACK KICK_JOINT tick=%llu F3session=%llu CI=%p joint=%d role=%s known=%d/%d/%d preSolveB1=(%.6f,%.6f,%.6f;%.6f,%.6f,%.6f,%.6f) preCoreB1=(%.6f,%.6f,%.6f;%.6f,%.6f,%.6f,%.6f) postCoreB1=(%.6f,%.6f,%.6f;%.6f,%.6f,%.6f,%.6f) paletteKnown=%d/%d raw32Hash=%016llX/%016llX space=MODEL quaternion=RAW_NATIVE outputDecode=NONE",
            (unsigned long long)r.tick,(unsigned long long)r.session,r.character,r.ids[j],names[j],bool(r.jointMask[0]&(1u<<j)),bool(r.jointMask[1]&(1u<<j)),bool(r.jointMask[2]&(1u<<j)),
            double(a.t[0]),double(a.t[1]),double(a.t[2]),double(a.q[0]),double(a.q[1]),double(a.q[2]),double(a.q[3]),
            double(b.t[0]),double(b.t[1]),double(b.t[2]),double(b.q[0]),double(b.q[1]),double(b.q[2]),double(b.q[3]),
            double(c.t[0]),double(c.t[1]),double(c.t[2]),double(c.q[0]),double(c.q[1]),double(c.q[2]),double(c.q[3]),
            bool(r.outputMask[0]&(1u<<j)),bool(r.outputMask[1]&(1u<<j)),(unsigned long long)r.outputHash[0][j],(unsigned long long)r.outputHash[1][j]);
        diagnostic_trace::Write(line);
    }
}

#include "vrik/native_palette_d67r1.inl"

void __fastcall SkinningTransformCoreDetour(void* desc) {
    if(g_pose_callback_active6689){if(g_skinning_core_original)g_skinning_core_original(desc);return;}
    const int mode=g_mode.load(std::memory_order_acquire);
    const int census=g_census_mode.load(std::memory_order_acquire);
    const int f12=g_weapon_control_mode.load(std::memory_order_acquire);
    if(!desc||(mode==0&&census==0&&f12==0&&!g_joint_recording.load(std::memory_order_acquire)&&!g_vrik_calibration_requested.load(std::memory_order_acquire)&&!diagnostics::lab::Any()&&!diagnostics::capture168::any(GetTickCount64()))){if(g_skinning_core_original)g_skinning_core_original(desc);return;}

    ReadScope101R2 pages;
    void* pose=nullptr;void* owner=nullptr;void* charPlus8=nullptr;void* output=nullptr;
    const bool descOk=Read(static_cast<unsigned char*>(desc)+0x00,pose)&&Read(static_cast<unsigned char*>(desc)+0x08,owner)&&Read(static_cast<unsigned char*>(desc)+0x10,charPlus8)&&Read(static_cast<unsigned char*>(desc)+0x20,output);
    // Capture the untouched native lever before any attachment/bow hierarchy override.
    if(descOk)CrossbowMechanismSkinningD78(pose,owner,charPlus8);
    if(descOk&&(mode>0||census==1||g_joint_recording.load(std::memory_order_acquire))) MaybeTraceDefaultSkeletonAnchor(pose,mode>0?mode:1);
    // V66.128 (c)/(d): Henry's held bow is skinned through this same boundary. Exact
    // pointer identity (equipment-qualified bow character + 8) gates it; it never
    // reaches the Henry writer and never widens Henry's correlation filter.
    if(descOk&&BowCandidate128(charPlus8)){BowSkinning128(pose,owner,charPlus8);if(g_skinning_core_original)g_skinning_core_original(desc);return;}
    // V66.178R11: experimental visual flinch on the struck NPC's character (same boundary as the bow).
    if(descOk){FlinchActive178r11 fa{};if(FlinchCandidate178r11(charPlus8,fa)){FlinchSkinning178r11(pose,owner,charPlus8,fa);if(g_skinning_core_original)g_skinning_core_original(desc);return;}}

    // CHAIN5's upstream pose-buffer write exists only between native SetIK and
    // this correlated hierarchy-build boundary. Restore it before reading the
    // pose for our final anatomical solve so malformed local data cannot leak
    // into the rendered skeleton.

    // V65.83: F10/F12 no longer wait for a combat SetIK event to learn Henry.
    // Prime through the validated CCryAction/player lifecycle at low frequency.
    // C37: the prime is the identity authority and renews the correlation lease, so it runs (1 Hz) whenever this
    // detour is active; the owner trace keeps its previous gate.
    {
        const bool primeTrace=mode>0||census>0||f12>0||diagnostics::capture168::any(GetTickCount64());
        static std::atomic<std::uint64_t> lastPrime{0};const auto pn=(std::uint64_t)GetTickCount64();auto pp=lastPrime.load(std::memory_order_relaxed);
        if(pn-pp>=1000ull&&lastPrime.compare_exchange_strong(pp,pn,std::memory_order_acq_rel)) (void)PrimeLocalPlayerPoseV6583(primeTrace);
    }
    // Only observers which actually consume non-Henry animation need its identity
    // walk. F2 performance / Henry-only F7 must not expand work to every NPC.
    const auto diagnosticSkinningNowR13=std::uint64_t(GetTickCount64());
    const bool observeOtherSkinningR13=cc::active(diagnosticSkinningNowR13)||ReloadDiagnostics175()||
        diagnostics::capture168::active(diagnostics::capture168::Domain::HandReference,diagnosticSkinningNowR13);
    if(descOk&&mode==0&&(census>=2||f12>=2)&&!observeOtherSkinningR13){
        const auto known=g_henry_char_attachment_manager.load(std::memory_order_acquire);
        const auto hintTick=g_henry_skinning_tick.load(std::memory_order_acquire),hintNow=std::uint64_t(GetTickCount64());
        if(known&&charPlus8!=known&&hintNow>=hintTick&&hintNow-hintTick<=100){InvalidateReadCache101R2();if(g_skinning_core_original)g_skinning_core_original(desc);return;}
    }
    void* player=g_last_player_pose.load(std::memory_order_acquire);
    const auto nowTick=static_cast<std::uint64_t>(GetTickCount64());
    const auto lastSetIk=g_last_setik_tick.load(std::memory_order_relaxed);
    void* playerChar=nullptr; void* playerAnim=nullptr;
    void* skinChar=nullptr; void* skinAnim=nullptr;
    void* poseSkeleton=nullptr;
    const bool playerCharValid=player&&ReadValidatedCharInstanceFromPlayerPose(player,playerChar,playerAnim);
    const bool skinCharValid=descOk&&ReadValidatedCharInstanceFromSkinning(charPlus8,skinChar,skinAnim);
    const bool poseOwnerValid=descOk&&ReadValidatedDefaultSkeletonFromPose(pose,poseSkeleton)&&poseSkeleton==owner;
    // V64.53: CPoseData and CDefaultSkeleton addresses rotate between animation/skinning buffers.
    // The owning CCharInstance is stable. Require exact, independently vft-validated character
    // identity plus the current pose->owner link. The cached player identity may outlive one SetIK
    // burst, but cannot authorize another character because current skinChar must equal playerChar.
    // C37: plus the ledger lease: the qualified character must still be the one the prime/solver renewed recently.
    const bool correlated=descOk&&playerCharValid&&skinCharValid&&poseOwnerValid&&skinChar==playerChar&&skinAnim==playerAnim&&PlayerIdentityAccepts37(skinChar,nowTick);
    if(correlated) {
        g_correlated_calls.fetch_add(1,std::memory_order_relaxed);
        g_henry_char_attachment_manager.store(charPlus8,std::memory_order_release);
        g_henry_skinning_thread.store(GetCurrentThreadId(),std::memory_order_release);
        g_henry_skinning_tick.store(nowTick,std::memory_order_release);
        static std::atomic<std::uint64_t> lastCorrOk{0}; auto prev=lastCorrOk.load(std::memory_order_relaxed);
        if((census==1||mode>0)&&nowTick-prev>=2000ull && lastCorrOk.compare_exchange_strong(prev,nowTick,std::memory_order_acq_rel)) {
            char cm[448]{}; std::snprintf(cm,sizeof(cm),"V64.53-VRIK CHAR-MATCH pose=%p owner=%p playerPose=%p char=%p skelAnim=%p setIkAgeMs=%llu mode=%d f10=%d poseSuppressed=%d IDENTITY-QUALIFIED",pose,owner,player,skinChar,skinAnim,(unsigned long long)(lastSetIk&&nowTick>=lastSetIk?nowTick-lastSetIk:0),mode,census,diagnostics::lab::NativeReference()); diagnostic_trace::Write(cm);
        }
    } else if((mode>0||census==1) && descOk) {
        static std::atomic<std::uint64_t> lastCorrMiss{0}; auto prev=lastCorrMiss.load(std::memory_order_relaxed);
        if(nowTick-prev>=1000ull && lastCorrMiss.compare_exchange_strong(prev,nowTick,std::memory_order_acq_rel)) {
            char cm[640]{}; std::snprintf(cm,sizeof(cm),"V64.53-VRIK CHAR-MISS pose=%p owner=%p charPlus8=%p lastPlayer=%p playerChar=%p skinChar=%p playerAnim=%p skinAnim=%p playerCharValid=%d skinCharValid=%d poseOwnerValid=%d charMatch=%d animMatch=%d setIkAgeMs=%llu mode=%d f10=%d FAIL-CLOSED",pose,owner,charPlus8,player,playerChar,skinChar,playerAnim,skinAnim,playerCharValid?1:0,skinCharValid?1:0,poseOwnerValid?1:0,(playerChar&&skinChar&&playerChar==skinChar)?1:0,(playerAnim&&skinAnim&&playerAnim==skinAnim)?1:0,(unsigned long long)(lastSetIk&&nowTick>=lastSetIk?nowTick-lastSetIk:0),mode,census); diagnostic_trace::Write(cm);
        }
    }

    // Observe selected non-Henry characters only at THEIR natural callback. Never
    // widen the pose-write correlation filter or read a stored character on the worker.
    if(skinCharValid&&poseOwnerValid&&observeOtherSkinningR13)
        animation_filter::ObserveDiagnosticSkinning(skinAnim,correlated);
    if(!correlated){if(g_skinning_core_original)g_skinning_core_original(desc);return;}
    hookcost178r22::Scope poseCostD48(hookcost178r22::PoseTransaction);
    CoherentTransaction6689 transaction;
    if(transaction.motionSessionD68){transaction.motionActorD68=reinterpret_cast<std::uintptr_t>(skinChar);
        transaction.motionDescriptorD68=reinterpret_cast<std::uintptr_t>(desc);transaction.motionOutputD68=reinterpret_cast<std::uintptr_t>(output);
        transaction.motionPoseD68=reinterpret_cast<std::uintptr_t>(pose);}
    PoseCallbackScope6689 callbackScope;
    TrackingSnapshot poseSnapshot{};bool haveSnapshot=ReadTrackingSnapshot(poseSnapshot);
    std::uint32_t count=0;RigMap rig{};bool rigOk=correlated&&ReadJointCount(owner,count)&&ResolveRig(owner,count,rig);
    if(rigOk){
        if(!g_rig_valid.exchange(true,std::memory_order_acq_rel))g_rig_maps.fetch_add(1,std::memory_order_relaxed);
        animation_filter::ObservePlayerSkinning(pose,owner,charPlus8);
        MaybeTraceMannequinTheory(charPlus8);
        if(census==1){
            static std::atomic<std::uint64_t> lastTarget{0};const auto tn=static_cast<std::uint64_t>(GetTickCount64());auto tp=lastTarget.load(std::memory_order_relaxed);
            if(tn-tp>=1200ull&&lastTarget.compare_exchange_strong(tp,tn,std::memory_order_acq_rel)){
                char ort[96]{},crt[96]{},prt[96]{};Rtti(owner,ort,sizeof(ort));Rtti(charPlus8,crt,sizeof(crt));Rtti(pose,prt,sizeof(prt));
                char m[512]{};std::snprintf(m,sizeof(m),"V64.13-F10 TARGET mode=%d(%s) pose=%p/%s skeleton=%p/%s charPlus8=%p/%s joints=%u solverC/E=%d/%d setIK=%llu writer=SELECTOR1",census,CombatCensusModeName(census),pose,prt[0]?prt:"?",owner,ort[0]?ort:"?",charPlus8,crt[0]?crt:"?",count,g_solver_joint_c.load(),g_solver_joint_e.load(),(unsigned long long)g_setik_calls.load());diagnostic_trace::Write(m);
            }
        }
        g_cached_count.store(count,std::memory_order_relaxed);g_cached_hand_r.store(rig.hand_r,std::memory_order_relaxed);g_cached_hand_l.store(rig.hand_l,std::memory_order_relaxed);g_cached_head.store(rig.head,std::memory_order_relaxed);
        if(census>=6 || g_f11_theory_phase.load(std::memory_order_acquire)>0) (void)ResolveNamedGripCandidatesV6545(owner,count,rig,false);
        if(mode>0) MaybeTraceDiscoveryObject("SkeletonOwner",owner,mode,2);
        if(mode==2 || mode==4){
            static std::atomic<std::uint64_t> lastAnat{0}; const auto now=static_cast<std::uint64_t>(GetTickCount64()); auto prev=lastAnat.load(std::memory_order_relaxed);
            if(now-prev>=1000ull && lastAnat.compare_exchange_strong(prev,now,std::memory_order_acq_rel)){
                TrackingSnapshot ts{}; const bool tr=ReadTrackingSnapshot(ts);
                char m[480]{}; std::snprintf(m,sizeof(m),
                    "V64.3-VRIK-ANATOMY mode=%d joints=%u pelvis=%d spine=%d/%d/%d/%d neck=%d head=%d handL/R=%d/%d upperL/R=%d/%d foreL/R=%d/%d track=%d HMD=(%.3f,%.3f,%.3f)",
                    mode,count,rig.pelvis,rig.spine,rig.spine1,rig.spine2,rig.spine3,rig.neck,rig.head,rig.hand_l,rig.hand_r,rig.upper_l,rig.upper_r,rig.fore_l,rig.fore_r,tr?1:0,ts.head.x,ts.head.y,ts.head.z);
                diagnostic_trace::Write(m);
            }
        }
    }

    // V64.10: weapon-slot name fields remain unresolved/read-only. The only runtime
    // CDefaultSkeleton call below is the statically identified name-hash lookup at +0x78BC4C.

    PoseGetFn b0fn=nullptr,b1fn=nullptr;QuatT28 b0[4]{},b1[4]{},ref[4]{};int parents[4]{-2,-2,-2,-2};bool rawOk=false;
    if(!descOk)g_fail_stage.store(1,std::memory_order_relaxed);
    else if(!rigOk)g_fail_stage.store(2,std::memory_order_relaxed);
    else {
        const int ids[4]{rig.upper_r,rig.fore_r,rig.elbow_r,rig.hand_r}; rawOk=ValidateRawPoseApi(pose,b0fn,b1fn);
        if(census==1||mode>0)for(int j=0;j<4&&rawOk;++j) rawOk=ReadQuat(b0fn,pose,ids[j],b0[j])&&ReadQuat(b1fn,pose,ids[j],b1[j])&&ReadCompanion(owner,ids[j],ref[j])&&ReadParentDirect(owner,ids[j],parents[j]);
        if(!rawOk)g_fail_stage.store(3,std::memory_order_relaxed);
    }

    // R96: restoration debt is observed from an already qualified native
    // buffer before any VRIK/attachment gate can return because of that debt.
    if(rigOk&&rawOk&&g_pose_restore_fault6689.load(std::memory_order_acquire))
        (void)ObservePoseRestoreRecoveryR96(owner,skinChar,pose,b1fn,count,
            correlated&&PlayerIdentityAccepts37(skinChar,std::uint64_t(GetTickCount64())));
    if(rigOk&&rawOk) {
        if(g_weapon_upstream_diag_mode.load(std::memory_order_acquire)>0) SampleWeaponUpstreamV6568(pose,b1fn,rig);
        if(g_f11_theory_phase.load(std::memory_order_acquire)>0) SampleFocusedF11Recorder(pose,b1fn,rig);
        if(g_f11_theory_phase.load(std::memory_order_acquire)==3) SampleNativeWeaponAnatomyV6567(pose,b1fn,rig);
        else SampleJointRecorder(owner,pose,b1fn,count);
        if(haveSnapshot) {
            QuatT28 nativeHead{};
            if(ReadQuat(b1fn,pose,rig.head,nativeHead)) RestoreSessionCalibration66100(poseSnapshot,nativeHead,skinChar);
            if(!g_arm_capture_active66100.load()) (void)CaptureTposeCalibration(pose,b1fn,rig,poseSnapshot);
        }
    }

    // V65.02 read-only ownership ladder. F9 never writes selector-1 or SetIK state.
    if(mode>=3 && rigOk && rawOk && diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64())) {
        static std::atomic<std::uint64_t> lastHandOwn{0}; const auto now=static_cast<std::uint64_t>(GetTickCount64()); auto prev=lastHandOwn.load(std::memory_order_relaxed);
        if(now-prev>=1500ull && lastHandOwn.compare_exchange_strong(prev,now,std::memory_order_acq_rel)) {
            auto readPos=[&](int id,V3& out)->bool { QuatT28 q{}; if(id<0||static_cast<std::uint32_t>(id)>=count||!ReadQuat(b1fn,pose,id,q))return false; out=FromT(q); return true; };
            V3 h{},j417{},j414{},j415{},j419{},j420{}; const bool oh=readPos(rig.hand_r,h),o417=readPos(417,j417),o414=readPos(414,j414),o415=readPos(415,j415),o419=readPos(419,j419),o420=readPos(420,j420);
            TrackingSnapshot ts{}; const bool tr=ReadTrackingSnapshot(ts);
            char line[1280]{}; std::snprintf(line,sizeof(line),
                "V65.02-HAND level=%d pose=%p handR=%d ok=%d(%.4f,%.4f,%.4f) "
                "j417=%d(%.4f,%.4f,%.4f) j414=%d(%.4f,%.4f,%.4f) j415=%d(%.4f,%.4f,%.4f) j419=%d(%.4f,%.4f,%.4f) j420=%d(%.4f,%.4f,%.4f) "
                "xrR=%d(%.4f,%.4f,%.4f) xrL=%d(%.4f,%.4f,%.4f) READ-ONLY",
                mode,pose,rig.hand_r,oh?1:0,h.x,h.y,h.z,o417?1:0,j417.x,j417.y,j417.z,o414?1:0,j414.x,j414.y,j414.z,o415?1:0,j415.x,j415.y,j415.z,o419?1:0,j419.x,j419.y,j419.z,o420?1:0,j420.x,j420.y,j420.z,
                (tr&&ts.right_valid)?1:0,ts.right.x,ts.right.y,ts.right.z,(tr&&ts.left_valid)?1:0,ts.left.x,ts.left.y,ts.left.z); diagnostic_trace::Write(line);
        }
    }

    if(mode>=5 && rigOk && diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64())) {
        static std::atomic<std::uint64_t> lastSubtree{0}; const auto now=static_cast<std::uint64_t>(GetTickCount64()); auto prev=lastSubtree.load(std::memory_order_relaxed);
        if(now-prev>=1500ull && lastSubtree.compare_exchange_strong(prev,now,std::memory_order_acq_rel)) {
            int rids[40]{},lids[40]{},rn=0,ln=0;
            for(std::uint32_t jid=0;jid<count;++jid){
                int cur=static_cast<int>(jid); bool rr=false,ll=false;
                for(int depth=0;depth<8 && cur>=0;++depth){ if(cur==rig.hand_r){rr=true;break;} if(cur==rig.hand_l){ll=true;break;} int par=-2; if(!ReadParentDirect(owner,cur,par)||par==cur)break; cur=par; }
                if(rr&&rn<40)rids[rn++]=static_cast<int>(jid); if(ll&&ln<40)lids[ln++]=static_cast<int>(jid);
            }
            char rs[320]{},ls[320]{}; int ro=0,lo=0;
            for(int i=0;i<rn&&ro<(int)sizeof(rs)-16;++i)ro+=std::snprintf(rs+ro,sizeof(rs)-ro,"%s%d",i?",":"",rids[i]);
            for(int i=0;i<ln&&lo<(int)sizeof(ls)-16;++i)lo+=std::snprintf(ls+lo,sizeof(ls)-lo,"%s%d",i?",":"",lids[i]);
            char line[960]{}; std::snprintf(line,sizeof(line),"V65.02-HAND-SUBTREE handR=%d descendants=%d ids=[%s] handL=%d descendants=%d ids=[%s] NOTE=finger-candidate-census-no-name-ABI READ-ONLY",rig.hand_r,rn,rs,rig.hand_l,ln,ls); diagnostic_trace::Write(line);
        }
    }

    // V65.62: preserve a safe copy of Henry's latest validated native head position.
    // This decouples guided F8 capture completion from the timing of the next SetIK call.
    if(rigOk && rawOk && correlated && rig.head>=0) {
        QuatT28 latestHead{};
        if(ReadQuat(b1fn,pose,rig.head,latestHead)) {
            const V3 hp=FromT(latestHead);
            g_latest_native_head_x.store(hp.x,std::memory_order_relaxed);g_latest_native_head_y.store(hp.y,std::memory_order_relaxed);g_latest_native_head_z.store(hp.z,std::memory_order_relaxed);
            g_latest_native_head_valid.store(true,std::memory_order_release);
#include "subsystems/dialogues/head_publication.inl" // M2_OWNED_SOURCE
        }
    }


    // Read the actual latest coherent tracking packet after identity/rig work.
    // Never advance an old packet's timestamp or fall back to raw wrists in combined mode.
    if(rigOk&&rawOk){
        MotionReadScopeD68 readTrace(transaction.motionSessionD68,transaction.motionCycleD68,
            transaction.motionActorD68,transaction.motionDescriptorD68,transaction.motionPoseD68,transaction.motionOutputD68);
        TrackingSnapshot latest{};haveSnapshot=ReadTrackingSnapshot(latest);
        poseSnapshot=haveSnapshot?latest:TrackingSnapshot{};
    }
    const bool roomLifecycleD76=kcd2vr::roomscale_d76::Enabled()||kcd2vr::native_roomscale_d76::Enrolled();
    if(!roomLifecycleD76&&f12>0&&rigOk&&rawOk&&haveSnapshot)PublishWeaponTracking6694(skinChar,owner,pose,b1fn,rig,poseSnapshot);

    // V64.82 F9 rung 1 is validation-only. The old +25 cm HAND_R proof was useful
    // archaeology but is exactly the kind of isolated fist warp we no longer want
    // in normal testing. No pose writes happen in rung 1.
    if(mode==1 && rigOk && rawOk) {
        g_fail_stage.store(0,std::memory_order_relaxed);
        static std::atomic<std::uint64_t> lastProofLog{0};
        const auto now=static_cast<std::uint64_t>(GetTickCount64()); auto prev=lastProofLog.load(std::memory_order_relaxed);
        if(now-prev>=1000ull && lastProofLog.compare_exchange_strong(prev,now,std::memory_order_acq_rel))
            diagnostic_trace::Write("V64.82-VRIK F9=1 READ-ONLY chain validation; zero pose writes");
    }


    // V66.89 single production owner. Build and validate the complete model pose
    // before writing. No current-attack elbow, name timer or partial per-arm batch.
    bool committed=false;
    // The snapshot is read AFTER the callback-entry timestamp and pose lock.
    // Compare to a fresh validation time; a genuinely newer sample is not a future timestamp.
    const auto validationTick=static_cast<std::uint64_t>(GetTickCount64());
    kcd2vr::crossbow_native_xr34::Passenger reloadPassengerXR34{};
    const bool reloadWorkingHandXR34=g_readReloadPassengerXR34&&
        g_readReloadPassengerXR34(reloadPassengerXR34,reinterpret_cast<std::uintptr_t>(skinChar),g_pose_epoch6689.load(),validationTick);
    kcd2vr::paired_hands_d72::Owner pairHandsOwnerD72{};
    const bool pairHandsD72=ReadInstantPairHandsD72(pairHandsOwnerD72,reinterpret_cast<std::uintptr_t>(skinChar),
        g_pose_epoch6689.load(),poseSnapshot.source_epoch,validationTick);
    const auto ridingModeD76=kcd2vr::riding_vrik_d76::mode.load(std::memory_order_acquire);
    bool ridingGaitOnlyD76=false;
    const bool nativeBody=animation_filter::NativeBodyRequired112(skinAnim,&ridingGaitOnlyD76,&transaction.ridingLifetimeR95)||reloadWorkingHandXR34;
    // D83 finisher: copy the classification now, before anything else re-inspects the FIFO on this thread.
    char finisherAliasD83[192]{};unsigned finisherSideD83=0;
    if(nativeBody&&!reloadWorkingHandXR34&&!pairHandsD72){const char* a=nullptr;finisherSideD83=animation_filter::FinisherUpperSideD83(&a);
        if(finisherSideD83&&a)std::snprintf(finisherAliasD83,sizeof(finisherAliasD83),"%s",a);}
    transaction.ridingGaitD76=ridingGaitOnlyD76&&!reloadWorkingHandXR34&&!pairHandsD72;
    transaction.ridingModeD76=ridingModeD76;
    animation_filter::nativeBodyOwned112.store(nativeBody,std::memory_order_release);
    if(rigOk&&rawOk&&haveSnapshot)PublishRoomscaleStepD76(skinChar,poseSnapshot);
    // Once enrolled, replace a save/player origin before publishing raw weapons.
    // Never-enrolled OFF keeps the exact D75 publication order above.
    if(roomLifecycleD76&&f12>0&&rigOk&&rawOk&&haveSnapshot)PublishWeaponTracking6694(skinChar,owner,pose,b1fn,rig,poseSnapshot);

    if(diagnostics::ownership48::active(validationTick)){
        static thread_local bool previousNative48=false;static thread_local std::uint64_t lastSkip48=0;
        if(nativeBody!=previousNative48||(nativeBody&&(validationTick<lastSkip48||validationTick-lastSkip48>=100))){
            diagnostics::ownership48::Event e{};e.stage=diagnostics::ownership48::PoseSkip;e.tick=validationTick;e.thread=GetCurrentThreadId();e.actor=reinterpret_cast<std::uintptr_t>(skinChar);
            e.sample=poseSnapshot.sequence;e.epoch=g_pose_epoch6689.load();e.sourceEpoch=poseSnapshot.source_epoch;e.sourceNs=poseSnapshot.timeNs;e.flags=nativeBody?1u:0u;
            diagnostics::ownership48::name(e,nativeBody?"RETIRE_VR_BODY_WEAPON_HOLSTERS":"ALLOW_VR_BODY_AGAIN");(void)diagnostics::ownership48::push(e);lastSkip48=validationTick;
        }previousNative48=nativeBody;
    }

    PhysObserveNativeRejoinR28(nativeBody,!reloadWorkingHandXR34&&!diagnostics::lab::NativeReference()&&
        f12!=2&&census>0&&rigOk&&rawOk&&haveSnapshot,skinChar,g_pose_epoch6689.load(),poseSnapshot,validationTick);
    if(nativeBody){
        kcd2vr::physics::grab_producer60::Cancel(0);kcd2vr::physics::grab_producer60::Cancel(1);
        animation_filter::combatPoseTick112.store(0,std::memory_order_release);g_phys_abort6698=true;
        // R30C1: native hit/paired actions move the body and held item. Recovery
        // must acquire from the fresh native seed, never the pre-hit world pose
        // or an old prepared gesture. This callback owns the pose mutex.
        PhysRetireMotion30c1("NATIVE_BODY_REQUIRED_LOCKED_ACTION",__LINE__);g_phys_holsters6698={};
        g_phys_ready_sample6698.store(0,std::memory_order_release);
    }
    auto fingerOutput28=BeginFingerOutput28(pose,owner,skinChar,skinAnim,rigOk?count:0,rig,rawOk?b1fn:nullptr,poseSnapshot,validationTick);
    auto kickOutput23=BeginKickOutput23(pose,owner,skinChar,skinAnim,output,count,rig,rawOk?b1fn:nullptr,validationTick);
    if((!nativeBody||reloadWorkingHandXR34||pairHandsD72||transaction.ridingGaitD76)&&!diagnostics::lab::NativeReference()&&f12!=2&&census>0&&rigOk&&rawOk&&haveSnapshot&&poseSnapshot.tick&&validationTick>=poseSnapshot.tick&&validationTick-poseSnapshot.tick<=100)
        {PerfScope101R2 timing(0);committed=BeginCoherent6689(transaction,pose,owner,skinChar,skinAnim,count,rig,b1fn,census,poseSnapshot);}
    if(transaction.traceH29&&!transaction.traceStartedH29){
        const char* reason=nativeBody?"SKIP_NATIVE_BODY":diagnostics::lab::NativeReference()?"SKIP_NATIVE_REFERENCE":
            f12==2?"SKIP_NATIVE_WEAPON_MODE":census<=0?"SKIP_VRIK_OFF":!rigOk?"SKIP_RIG":!rawOk?"SKIP_RAW_POSE_API":
            !haveSnapshot?"SKIP_NO_TRACKING_SNAPSHOT":"SKIP_TRACKING_AGE";
        ObserveVrikSkipH29(transaction,poseSnapshot,skinChar,reason,__LINE__);
    }
    if(!committed){
        g_committed_pose_valid6689.store(false,std::memory_order_release);
        if(!nativeBody)g_native_rejoin_r28.clear(); // failed return cannot qualify a later acquisition
    }
    InvalidateReadCache101R2();
    auto paletteD67R1=BeginNativePaletteD67R1(desc,pose,owner,skinChar,output,rig,rigOk?count:0,poseSnapshot,transaction,committed);
#include "vrik/native_skinning_acquisition57.inl"
    if(paletteD67R1&&!committed)paletteD67R1->flags&=~unsigned(kcd2vr::diagnostics::native_palette_d67r1::ScopedPose);
    if(kickOutput23){KickOutputJoints23(*kickOutput23,1,b1fn);KickOutputPalette23(*kickOutput23,0,desc);}
    MotionCoreBeforeD68(transaction,desc,pose,owner,skinChar,output,rig,count,rawOk?b1fn:nullptr,poseSnapshot,committed);
    // D83 finisher: native body keeps arms/legs/weapon; head, neck and upper torso follow the headset.
    FinisherWriteD83 finisherWriteD83{};
    if(finisherSideD83&&!committed)
        FinisherBeginD83(finisherWriteD83,finisherSideD83,finisherAliasD83,pose,owner,skinChar,rig,rigOk,rawOk?b1fn:nullptr,count,
            poseSnapshot,haveSnapshot,validationTick,census>0&&f12!=2&&!diagnostics::lab::NativeReference());
    else FinisherIdleD83();
    {PerfScope101R2 timing(1);if(g_skinning_core_original){
        hookcost178r22::Original nativeCostD48(hookcost178r22::PoseTransaction);
        if(kickOutput23)kickOutput23->called=true;
        if(transaction.traceH29||transaction.motionSessionD68)transaction.traceNativeCalledH29=true;
        if(paletteD67R1)paletteD67R1->beforeNativeNs=kcd2vr::diagnostics::native_palette_d67r1::clockNs();
        g_skinning_core_original(desc);
        if(paletteD67R1){paletteD67R1->afterNativeNs=kcd2vr::diagnostics::native_palette_d67r1::clockNs();paletteD67R1->flags|=kcd2vr::diagnostics::native_palette_d67r1::NativeReturned;}
        if(kickOutput23)kickOutput23->returned=true;
        if(fingerOutput28)fingerOutput28->originalReturned=true;
        if(transaction.traceH29||transaction.motionSessionD68)transaction.traceNativeReturnedH29=true;
    }}
    MotionCoreAfterD68(transaction,desc,pose,owner,skinChar,output,rig,count,committed);
    FinisherRestoreD83(finisherWriteD83); // D83 finisher
    InvalidateReadCache101R2(); // native code may change mappings
    if(paletteD67R1&&(paletteD67R1->flags&kcd2vr::diagnostics::native_palette_d67r1::NativeReturned))ReadNativePaletteD67R1(*paletteD67R1,1,rigOk?count:0);
    if(kickOutput23){
        if(kickOutput23->returned){KickOutputJoints23(*kickOutput23,2,b1fn);KickOutputPalette23(*kickOutput23,1,desc);}
        FinishKickOutput23(*kickOutput23,committed);
    }
    transaction.Restore();
    if(committed){PublishCommitted6689(transaction);
        FinisherLatchCommittedD83(skinChar,transaction.tracking,transaction.epoch,transaction.policyGeneration57); // D83R1: only the current published pose may seed the overlay
        g_prebuild_writes.fetch_add(1,std::memory_order_relaxed);g_fail_stage.store(0,std::memory_order_relaxed);}
    else if(census>=2&&f12!=2&&!diagnostics::lab::NativeReference()){
        g_rejects.fetch_add(1,std::memory_order_relaxed);g_fail_stage.store(89,std::memory_order_relaxed);
        const unsigned authorityReason157=!rigOk?0:!rawOk?1:!haveSnapshot?2:
            (!poseSnapshot.tick||validationTick<poseSnapshot.tick||validationTick-poseSnapshot.tick>100)?3:nativeBody?4:5;
        g_pose_authority_drop157[authorityReason157].fetch_add(1,std::memory_order_relaxed);
        static std::atomic<std::uint64_t> lastReject{0};auto old=lastReject.load(std::memory_order_relaxed);
        if(nowTick>=old&&nowTick-old>=2000&&lastReject.compare_exchange_strong(old,nowTick)){ // V66.178R29: one line per 2 s without a recorder (hands fall back to the native pose here)
            const char* reason=!rigOk?"RIG":!rawOk?"RAW_POSE_API":!haveSnapshot?"NO_TRACKING_SNAPSHOT":
                (!poseSnapshot.tick||validationTick<poseSnapshot.tick||validationTick-poseSnapshot.tick>100)?"TRACKING_AGE":nativeBody?"NATIVE_BODY_HANDOFF":transaction.stage;
            { char detail[512]{};std::snprintf(detail,sizeof(detail),
                "V66.168 POSE_VALIDITY sample=%llu tick=%llu validate=%llu head=%d right=%d left=%d heldHRL=%d/%d/%d nativeBody=%d stage=%s writes=UNCHANGED agePolicy=UNCHANGED",
                (unsigned long long)poseSnapshot.sequence,(unsigned long long)poseSnapshot.tick,(unsigned long long)validationTick,
                poseSnapshot.head_valid,poseSnapshot.right_valid,poseSnapshot.left_valid,poseSnapshot.head_held,poseSnapshot.right_held,poseSnapshot.left_held,nativeBody,transaction.stage);
                diagnostic_trace::Write(detail); }
            char m[512]{};std::snprintf(m,sizeof(m),"V66.92R1-POSE-REJECT rung=%d reason=%s sourceLine=%d sample=%llu ageMs=%llu rejects=%llu result=NATIVE_POSE_FALLBACK meshVisibility=UNOBSERVED",
                census,reason,transaction.failureLine,(unsigned long long)poseSnapshot.sequence,
                (unsigned long long)(poseSnapshot.tick&&validationTick>=poseSnapshot.tick?validationTick-poseSnapshot.tick:UINT64_MAX),
                (unsigned long long)g_rejects.load());diagnostic_trace::Write(m);
        }
    }

    if(paletteD67R1)FinishNativePaletteD67R1(*paletteD67R1,transaction);
    if(fingerOutput28)FinishFingerOutput28(*fingerOutput28,transaction);

    if(rigOk&&rawOk&&output&&(census==1||mode>0)){
        // V66.85: old ITEM_R/ITEM_L final-skin and controller-follow F12 proofs are retired.
        // The new F12 ladder stays downstream of exact Henry AttachRight qualification and fails closed
        // until each attachment-object ABI is proven.
        Dq32 u{},f{},e{},h{};
        const bool outOk=Read(static_cast<unsigned char*>(output)+static_cast<std::ptrdiff_t>(rig.upper_r)*0x20,u)&&
                         Read(static_cast<unsigned char*>(output)+static_cast<std::ptrdiff_t>(rig.fore_r)*0x20,f)&&
                         Read(static_cast<unsigned char*>(output)+static_cast<std::ptrdiff_t>(rig.elbow_r)*0x20,e)&&
                         Read(static_cast<unsigned char*>(output)+static_cast<std::ptrdiff_t>(rig.hand_r)*0x20,h);
        if(outOk){
            PublishDiag(rig,parents[0],parents[1],parents[2],parents[3],b0,b1,ref,u,f,e,h);
            if(mode>=3){
                static std::atomic<std::uint64_t> lastFinal{0}; const auto now=static_cast<std::uint64_t>(GetTickCount64()); auto prev=lastFinal.load(std::memory_order_relaxed);
                if(now-prev>=1500ull && lastFinal.compare_exchange_strong(prev,now,std::memory_order_acq_rel)){
                    const V3 c0=DecodeDqTranslation(h,false,false),c1=DecodeDqTranslation(h,false,true),c2=DecodeDqTranslation(h,true,false),c3=DecodeDqTranslation(h,true,true);
                    char line[1050]{}; std::snprintf(line,sizeof(line),
                        "V65.02-FINAL-HAND level=%d handR=%d dq=[%.5f %.5f %.5f %.5f | %.5f %.5f %.5f %.5f] "
                        "decode0=(%.4f,%.4f,%.4f) decode1=(%.4f,%.4f,%.4f) decode2=(%.4f,%.4f,%.4f) decode3=(%.4f,%.4f,%.4f) READ-ONLY",
                        mode,rig.hand_r,h.a[0],h.a[1],h.a[2],h.a[3],h.b[0],h.b[1],h.b[2],h.b[3],
                        c0.x,c0.y,c0.z,c1.x,c1.y,c1.z,c2.x,c2.y,c2.z,c3.x,c3.y,c3.z); diagnostic_trace::Write(line);
                }
            }
            if(committed)g_fail_stage.store(0,std::memory_order_relaxed);
        }
        else {g_fail_stage.store(4,std::memory_order_relaxed);g_rejects.fetch_add(1,std::memory_order_relaxed);}
    }

    // R96R1: publication and final consumers above still reject this failed
    // transaction. Exact current-buffer readback reopens future work only;
    // no extra restore write or replacement-buffer forgiveness occurs here.
    if(g_pose_restore_fault6689.load(std::memory_order_acquire))
        (void)ObservePoseRestoreAfterCallbackR96R1(desc,pose,owner,charPlus8,skinChar,count);
}

void SetWeaponControlMode(int mode) noexcept { SetWeaponControlModeImpl(mode); }
#include "subsystems/interactions/hand_context_control.inl" // M2_OWNED_SOURCE
#include "subsystems/base/combat_mode_controls.inl" // M2_OWNED_SOURCE
// R160 VrCombatNoStep counter-step (native_motion_owner.inl, private namespace there).
void PublishMoveStick160(float lx,float ly) noexcept {PublishMoveStickImpl160(lx,ly);}
void CounterStepAfterUpdate160(void* combatActor) noexcept {CounterStepAfterUpdateImpl160(combatActor);}
#include "subsystems/physics/handling_controls.inl" // M2_OWNED_SOURCE
bool CombatFull178r10Enabled() noexcept {return PhysicalCombat177Enabled()&&g_combat_full178r10.load(std::memory_order_acquire);}
int F4Rung178r10() noexcept {if(CombatExperiment175Enabled()&&g_combat_integrated178r22c2.load())return 1;const int r=kcd2vr::user175::f4Rung(CombatExperiment175Enabled(),PhysicalCombat177Enabled(),CombatFull178r10Enabled());return r>=3?(g_combat_rung178r15.load()==7?3:g_combat_rung178r15.load()==6?2:std::max(3,g_combat_rung178r15.load())):r;}
// C37C4: the hammer stroke needs the sample's nanosecond time. GetTickCount64 advances every ~15.6 ms, so at ~90 Hz
// half of the consecutive samples had dt=0 and the rest a 16 ms dt for an 11 ms move - the measured speed was 30 %
// low and noisy, and the 1.6 m/s stroke threshold was almost never reached (Miki, 17 Oct: 47 s at the anvil, 0 strokes).
bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept {
    // F5 read-only copied-body snapshot; stale native-passenger gaps are explicit.
    {const auto now=std::uint64_t(GetTickCount64());
     if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){
      static std::atomic_flag busy=ATOMIC_FLAG_INIT;
      if(!busy.test_and_set(std::memory_order_acquire)){
       const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
       static diagnostics::interaction_receipt59c10::Gate receipt;
       auto& w=kcd2vr::activity_input163::witness175;const auto alch=w.active[unsigned(kcd2vr::activity_input163::Kind175::Alchemy)].load();
       const auto smith=w.active[unsigned(kcd2vr::activity_input163::Kind175::Smithing)].load();const auto life=w.lifetime175.load();
       if((alch||smith)&&receipt.observe(session,now,{alch?alch:smith,unsigned(life),-1,w.SmithPhaseC1()},500,4096,500)){
        CommittedFrame6695 body{};const bool copied=g_committed_frame6695.read(body);
        const bool fresh=copied&&body.stamp.epoch==g_pose_epoch6689.load()&&body.stamp.tick&&now>=body.stamp.tick&&now-body.stamp.tick<=80;
        char line[1500]{};std::snprintf(line,sizeof(line),"C71 F5_CRAFT_BODY session=%llu tick=%llu instance=%llX life=%llu actor=%llX bodyCopied=%d bodyFresh=%d bodyAgeMs=%llu poseEpoch=%llu sample=%llu rootSource=LAST_COMMITTED_MODEL_WORLD rootQ=%.7g,%.7g,%.7g,%.7g rootWorld=%.7g,%.7g,%.7g scale=%.7g allowedWristModelR=%.7g,%.7g,%.7g allowedWristModelL=%.7g,%.7g,%.7g valid=%d,%d currentNativeRoot=UNOBSERVED_WHEN_BODY_STALE",
            (unsigned long long)session,(unsigned long long)now,(unsigned long long)(alch?alch:smith),(unsigned long long)life,(unsigned long long)body.stamp.actor,copied,fresh,
            (unsigned long long)diagnostics::interaction_receipt59c10::age(now,body.stamp.tick),(unsigned long long)body.stamp.epoch,(unsigned long long)body.stamp.sample,
            body.world[0],body.world[1],body.world[2],body.world[3],body.world[4],body.world[5],body.world[6],body.world[7],
            body.allowed[0].t[0],body.allowed[0].t[1],body.allowed[0].t[2],body.allowed[1].t[0],body.allowed[1].t[1],body.allowed[1].t[2],body.valid[0],body.valid[1]);diagnostic_trace::Write(line);
       }
       busy.clear(std::memory_order_release);
      }
     }}

    const workstation_sample46::Frame empty{};
    out=empty;const auto epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    const auto source=tracking59c6::epoch.load(std::memory_order_acquire);
    TrackingSnapshot t{};
    if(!ReadTrackingSnapshot(t)||!workstation_sample46::copy(t,epoch,source,std::uint64_t(GetTickCount64()),out))return false;
    if(epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||source!=tracking59c6::epoch.load(std::memory_order_acquire)){out=empty;return false;}
    return true;
}
#include "subsystems/experimental/contact_gate_control.inl" // M2_OWNED_SOURCE
#include "subsystems/physics/support_mode_controls.inl" // M2_OWNED_SOURCE
#include "subsystems/base/guard_entry_controls.inl" // M2_OWNED_SOURCE
#include "subsystems/masterstrike_combos/handoff_controls.inl" // M2_OWNED_SOURCE
#include "subsystems/base/guard_receipt_controls.inl" // M2_OWNED_SOURCE
#include "subsystems/physics/stat_sim_control.inl" // M2_OWNED_SOURCE
#include "subsystems/masterstrike_combos/native_flow_controls.inl" // M2_OWNED_SOURCE
// CB1 physical combo finisher: where the blade is going (kept out of native_flow_controls.inl, whose
// fixture test mocks PhysFrame6698).
// CB4: measured point now + the same point in the previous physics frame (no extrapolation here).
bool WeaponPathWorldCB4(bool pommel,float (&now)[3],float (&previous)[3]) noexcept {
    now[0]=now[1]=now[2]=0;previous[0]=previous[1]=previous[2]=0;
    PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if(!ready||!g_phys_frame6698.read(f)||f.input.sample!=ready||!f.input.haveWorld||!f.output.active[0]||!f.profiles[0].melee()||
       !ph::finite(f.input.world)||!ph::finite(f.output.weapons[0])||!ph::finite(f.previousWeapon177)||!pm::finite(f.profiles[0].tip)||
       !std::isfinite(f.input.worldScale)||f.input.worldScale<.1||f.input.worldScale>10)return false;
    const auto tick=std::uint64_t(GetTickCount64());
    if(!f.input.tick||tick<f.input.tick||tick-f.input.tick>120)return false;
    ph::Vec3 local=f.profiles[0].tip;
    if(pommel){const auto line=ph::shaft_line39(f.profiles[0]);
        if(!line.valid)return false;local=line.o-line.u*line.butt;}
    const ph::Vec3 a=ph::point(f.input.world,ph::point(f.output.weapons[0],local)*f.input.worldScale);
    const ph::Vec3 b=ph::point(f.input.world,ph::point(f.previousWeapon177,local)*f.input.worldScale);
    now[0]=float(a.x);now[1]=float(a.y);now[2]=float(a.z);previous[0]=float(b.x);previous[1]=float(b.y);previous[2]=float(b.z);
    for(int i=0;i<3;++i)if(!std::isfinite(now[i])||!std::isfinite(previous[i]))return false;
    return true;
}
#include "subsystems/base/melee_policy_controls.inl" // M2_OWNED_SOURCE
#include "subsystems/masterstrike_combos/strike_policy_controls.inl" // M2_OWNED_SOURCE
bool RequestKick59() noexcept {
    if(!CombatExperiment175Enabled()||!PhysicalCombat177Enabled()||!g_gameplay_allowed.load())return false;
    const auto now=std::uint64_t(GetTickCount64()),ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    PhysFrame6698 f{};
    if(!ready||!g_phys_frame6698.read(f)||ready!=f.input.sample||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       !f.player||!f.input.tick||now<f.input.tick||now-f.input.tick>80||!f.input.allowed||
       f.request!=g_weapon_request_epoch6694.load()||f.input.bodyEpoch!=g_pose_epoch6689.load()||
       !f.input.tracked[0]||!f.profiles[0].melee())return false; // R59.16C4: sword Grip/ownership not required for R3
    return kcd2vr::kick5916c3::publish({reinterpret_cast<std::uintptr_t>(f.player),f.profiles[0].key,
        f.profiles[0].generation,f.input.bodyEpoch,f.request},now);
}
// R59.16C2: last R3 kick request that expired without a prepare (reason codes in physical_weapon.h).
bool KickDrop59C2(int& reason,std::uint64_t& tick) noexcept {
    PhysFrame6698 f{};if(!g_phys_frame6698.read(f))return false;
    reason=f.output.kickDrop59c2;tick=f.output.kickDropTick59c2;return tick!=0;
}
void SetCombatNoStep55(bool on) noexcept {g_no_step55.store(on,std::memory_order_relaxed);}
bool CombatNoStep55() noexcept {return g_no_step55.load(std::memory_order_relaxed);}
void SetCombatNoTurnD40(bool on) noexcept {g_no_turn_d40.store(on,std::memory_order_release);}
bool CombatNoTurnD40() noexcept {return g_no_turn_d40.load(std::memory_order_acquire);}
bool CombatNoTurnReadyD40() noexcept {const auto b=WhBase();return b&&g_motion_contract112.load(std::memory_order_acquire)==b&&g_rotation_contract_d40.load(std::memory_order_acquire)==b;}
bool CombatFreeViewContextD42() noexcept {return NativeFreeViewContextD42();}
std::uint64_t CombatNoTurnVetoCountD40() noexcept {return g_rotation_veto_d40.load(std::memory_order_relaxed);}
void SetNpcProximityRadius36(float meters) noexcept {g_proximity_radius36.store(std::isfinite(meters)?std::clamp(meters,.2f,.6f):.4f);}
float NpcProximityRadius36() noexcept {return g_proximity_radius36.load();}
void SetClinchDistanceMeters32(float meters) noexcept {ClinchDistanceConfigure32(meters);}
void SetClinchModeD40(int mode) noexcept {kcd2vr::clinch_intent_d40::setMode(mode);}
int ClinchModeD40() noexcept {return kcd2vr::clinch_intent_d40::getMode();}
bool ClinchGateReadyD40() noexcept {return g_close_combat_ready175.load(std::memory_order_acquire);}
float ClinchDistanceMeters32() noexcept {return g_clinch_distance_meters32.load(std::memory_order_acquire);}
void SetGeometryAutoLock178r29(bool on) noexcept {g_geometry_auto_lock178r29.store(on,std::memory_order_relaxed);}
bool GeometryAutoLock178r29() noexcept {return g_geometry_auto_lock178r29.load(std::memory_order_relaxed);}
std::atomic<int> g_geometry_owner_stage49{0}; // V66.178R49: which gate rejected the last owner check (GEOM_HIT STALE_OR_OWNER_CHANGED)
int GeometryOwnerStage49() noexcept {return g_geometry_owner_stage49.load(std::memory_order_relaxed);}
// R59.16C7E-DECAP1: true while OUR posture block input is held or was released within the last 250 ms (the native
// block action may still show phase 128/256 for a few frames after the release).
bool GuardOwnedByUs59c7e(const void* state) noexcept {
    if(!state)return false;
    // Identity-bound (Codex review): the block input must be ours on THIS Henry state object; a held input qualifies,
    // a release qualifies for 250 ms (the native block action winds down over a few frames). Context loss resets both.
    if(g_guard_physical112.load()&&g_native_guard112.owner.state==state)return true;
    const auto t=g_guard_release_tick59c7e.load(std::memory_order_relaxed);const auto now=std::uint64_t(GetTickCount64());
    return t&&now>=t&&now-t<=250&&g_guard_release_state59c7e.load(std::memory_order_relaxed)==state;
}
bool HybridQuietGuardOwnedH20(const void* actor,const void* state) noexcept {
    // The native block can linger after our strike handoff. This is only an
    // owner classification: the caller still needs the original fresh contact,
    // exact motion claim and completed native cost; no phase is changed here.
    if(!actor||!state||!g_gameplay_allowed.load()||g_ctx_menu.load()||g_ctx_dialogue.load()||
       g_ctx_cutscene.load()||g_ctx_scripted.load()||animation_filter::nativeBodyOwned112.load()||
       g_phys_fault6698.load()||!g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load())return false;
    void* player=nullptr;NativeOwner104 owner{};nc104::Observed native{};
    if(!Read(static_cast<const unsigned char*>(actor)+0x1F8,player)||!player||
       !NativeResolve104(player,owner)||owner.actor!=actor||owner.state!=state||
       owner.epoch!=g_pose_epoch6689.load()||!GuardOwnedByUs59c7e(state)||
       (g_guard_manual_owner112.load()==player&&g_guard_manual112.load())||
       (g_native_guard112.held&&!NativeHeldGuardOwnerH14(owner))||!NativeObserve104(owner,native))return false;
    return (native.phase==128||native.phase==256)&&(native.type==-1||native.type==1)&&
        !native.requested&&!native.requestCandidate&&!native.blocking;
}
bool GeometryOwnerValid178r30(std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept { // V66.178R30 (R28 review F03)
    PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    const auto fail=[](int s){g_geometry_owner_stage49.store(s,std::memory_order_relaxed);return false;};
    if(!NativeHeldContactsCurrentT4())return fail(10);
    if(!key||!ready||!g_phys_frame6698.read(f)||f.input.sample!=ready||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire))return fail(1);
    if(!kcd2vr::contact176::fresh(f.input.tick,GetTickCount64(),80))return fail(2);
    if(epoch!=g_pose_epoch6689.load()||f.input.bodyEpoch!=epoch)return fail(3);
    if(request!=g_weapon_request_epoch6694.load()||f.request!=request)return fail(4);
    if(!f.input.allowed||!f.input.physicalCombat177||!g_gameplay_allowed.load())return fail(5);
    if(animation_filter::nativeBodyOwned112.load())return fail(6);
    if(g_ctx_menu.load()||g_ctx_dialogue.load())return fail(7);
    for(int s=0;s<2;++s)if(f.profiles[s].key==key&&f.profiles[s].generation==generation){
        if(!ph::attack_authority55c1(f.input,f.output,s))return fail(9);
        g_geometry_owner_stage49.store(0,std::memory_order_relaxed);return true;}
    return fail(8);}
unsigned GeometryIntentOwnerStateH30(std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept {
    PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    const auto now=std::uint64_t(GetTickCount64());
    const auto fail=[](int s)->unsigned{g_geometry_owner_stage49.store(s,std::memory_order_relaxed);return 0;};
    // F9 keeps its typed body/lease path. A raw tracked owner can never borrow
    // an unretired physical body or an unobserved admission generation.
    if(!NativeHeldContactsCurrentT4())return fail(10);
    if(!key||!generation||!ready||!g_phys_frame6698.read(f)||f.input.sample<ready)return fail(1);
    if(!kcd2vr::contact176::fresh(f.input.tick,now,80)||!TrackingHeadUsable111(now))return fail(2);
    if(epoch!=g_pose_epoch6689.load()||f.input.bodyEpoch!=epoch)return fail(3);
    if(request!=g_weapon_request_epoch6694.load()||f.request!=request)return fail(4);
    if(!f.input.allowed||!f.input.physicalCombat177||!g_gameplay_allowed.load())return fail(5);
    if(animation_filter::nativeBodyOwned112.load())return fail(6);
    if(g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_scripted.load()||g_ctx_reading.load())return fail(7);
    for(int side=0;side<2;++side)if(f.profiles[side].key==key&&f.profiles[side].generation==generation){
        // The H30 proof is emitted by the qualified once-per-sample recognizer,
        // independently of reach/collision presentation. No proof is inferred
        // from handlingOnly/active/owned, and checked-contact authority remains
        // the separate attack_authority55c1 predicate above.
        if(!TrackingHandUsable111(side,now)||!ph::command_authority_h30(f.input,f.output,f.profiles[side],side))return fail(9);
        // PhysPublish6698 publishes the new frame before completing its command
        // queue and ready-sample store. That same-owner interval is not owner
        // loss. It grants no input/contact authority: only an already-earned
        // native combo slot may wait on its original finite clock.
        const auto completed=g_phys_ready_sample6698.load(std::memory_order_acquire);
        if(completed!=ready&&completed!=f.input.sample)return fail(1);
        if(completed!=f.input.sample){g_geometry_owner_stage49.store(1,std::memory_order_relaxed);return 2;}
        g_geometry_owner_stage49.store(0,std::memory_order_relaxed);return 1;
    }
    return fail(8);
}
bool GeometryIntentOwnerH30(std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept {
    return GeometryIntentOwnerStateH30(key,generation,epoch,request)==1;
}
bool OriginalGeometryContactOwnerH30(std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,
    std::uint64_t sample,std::uint64_t gesture,std::uint64_t impactTick,std::uint64_t now) noexcept {
    if(!sample||!gesture||!kcd2vr::contact176::fresh(impactTick,now,kcd2vr::contact59c3::maximumContactAge)||
       !GeometryIntentOwnerH30(key,generation,epoch,request))return false;
    PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    // Recheck after the source-owner callback: the original event may be older
    // than this pose, but cannot come from a future sample or replacement item.
    return ready&&g_phys_frame6698.read(f)&&f.input.sample==ready&&
        ready==g_phys_ready_sample6698.load(std::memory_order_acquire)&&sample<=f.input.sample&&impactTick<=f.input.tick&&
        f.input.bodyEpoch==epoch&&f.request==request&&epoch==g_pose_epoch6689.load()&&request==g_weapon_request_epoch6694.load()&&
        ((f.profiles[0].key==key&&f.profiles[0].generation==generation&&
          ph::command_authority_h30(f.input,f.output,f.profiles[0],0))||
         (f.profiles[1].key==key&&f.profiles[1].generation==generation&&
          ph::command_authority_h30(f.input,f.output,f.profiles[1],1)));
}
kcd2vr::original_contact_h8::Decision WaitOriginalGeometryContactH8(
    const kcd2vr::original_contact_h8::Source& original,const kcd2vr::contact176::Ticket& copiedRelease,std::uint64_t now) noexcept {
    using Decision=kcd2vr::original_contact_h8::Decision;
    PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if(!original.player||!original.entity||!original.collider||!ready||!NativeHeldContactsCurrentT4()||
       !g_phys_frame6698.read(f)||f.input.sample!=ready||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       !kcd2vr::contact176::fresh(f.input.tick,now,80)||!TrackingHeadUsable111(now)||!TrackingHandUsable111(0,now)||
       !kcd2vr::contact176::fresh(original.tick,now,kcd2vr::contact59c3::maximumContactAge)||
       original.player!=reinterpret_cast<std::uintptr_t>(f.player)||original.entity!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())||
       original.body!=g_pose_epoch6689.load()||original.body!=f.input.bodyEpoch||
       original.request!=g_weapon_request_epoch6694.load()||original.request!=f.request||
       original.recovery!=kcd2vr::contact176::recoveryEpoch55c1.load()||
       !g_gameplay_allowed.load()||g_phys_fault6698.load()||animation_filter::nativeBodyOwned112.load()||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_scripted.load()||g_ctx_reading.load()||
       !ph::pre_quarantine_contact_h8(f.input,f.output,f.profiles[0],original)||
       !kcd2vr::contact176::fresh(f.output.intentQuarantineH8[0].since,now,100))return Decision::Reject;
    const kcd2vr::motion59c4::Key key{original.gesture,original.key,original.generation,original.body,
        original.request,original.recovery,original.player};
    const auto queued=PhysQueuedOriginalCommitH8(original,f,now);
    // A contended inspection may postpone only this independently proven old
    // occurrence, and only inside the ORIGINAL queued-command100ms clock.
    // It is not evidence of a native cost or authority to dispatch anything.
    const bool boundedBusy=queued==PhysQueuedCommitH8::Busy&&
        kcd2vr::contact176::fresh(f.output.intentQuarantineH8[0].commit,now,100);
    if(queued!=PhysQueuedCommitH8::ExactQueued&&!boundedBusy){
    if(kcd2vr::swing_effort59c14::ledger.contactProof54(key,now)==kcd2vr::swing_effort59c14::ContactProof54::Refuse)return Decision::Reject;
    const kcd2vr::contact59c3::Key releaseKey{original.gesture,original.key,original.generation,original.body,original.request,original.recovery};
    auto release=copiedRelease;if(!release.serial)(void)kcd2vr::contact59c3::releases.find(releaseKey,release);
    if(kcd2vr::contact59c3::join(releaseKey,release,original.tick,now,original.recovery)!=kcd2vr::contact59c3::Join::Bound||
       release.player!=original.player||release.entity!=original.entity)return Decision::Reject;
    }
    // The result cannot enter native code. Recheck the immutable source clocks
    // and live epoch after the bounded read-only cost/release snapshots.
    return ready==g_phys_ready_sample6698.load(std::memory_order_acquire)&&
        original.recovery==kcd2vr::contact176::recoveryEpoch55c1.load()&&
        original.body==g_pose_epoch6689.load()&&original.request==g_weapon_request_epoch6694.load()?Decision::Wait:Decision::Reject;
}
// Typed solver variants do not weaken either legacy predicate above.
bool GeometrySolverOwnerValidH28(const kcd2vr::solver_command_h28::Stamp& proof,std::uint64_t key,
 std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,std::uint64_t sample,std::uint64_t gesture) noexcept {
 PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);NativeSolverLineageH28 lineage{};
 if(!ready||!g_phys_frame6698.read(f)||f.input.sample!=ready||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
    !gesture||sample!=proof.sourceSample||sample>f.input.sample||proof.owner.key!=key||proof.owner.generation!=generation||
    proof.owner.epoch!=epoch||proof.owner.request!=request||
    !NativeSolverLineageForKeyH28(gesture,key,generation,epoch,request,lineage))return false;
 const auto now=std::uint64_t(GetTickCount64());
 return NativeSolverLineageOwnerCurrentH28(lineage,f,now)&&NativeSolverOwnerCurrentH28(proof,f,now)&&
    proof.owner==lineage.proof.owner&&proof.worldGeneration==lineage.proof.worldGeneration&&
    proof.bodyGeneration==lineage.proof.bodyGeneration&&proof.colliderLease==lineage.proof.colliderLease&&
    proof.body==lineage.proof.body&&proof.foreign==lineage.proof.foreign&&proof.foreignType==lineage.proof.foreignType&&
    proof.admissionSerial==lineage.proof.admissionSerial;
}
bool GeometrySolverCostOwnerValidH28(const kcd2vr::solver_command_h28::Stamp& proof,std::uint64_t gesture,std::uint64_t key,std::uint64_t generation,
 std::uint64_t epoch,std::uint64_t request,std::uintptr_t actor) noexcept {
 PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);NativeSolverLineageH28 lineage{};
 if(!actor||!ready||!g_phys_frame6698.read(f)||f.input.sample!=ready||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
    !NativeSolverLineageForKeyH28(gesture,key,generation,epoch,request,lineage)||
    reinterpret_cast<std::uintptr_t>(lineage.owner.actor)!=actor)return false;
 // Original action-registry lineage is proven by the caller; only CURRENT
 // tracking/body/contact clocks must be <=80ms. An aged original Prepare is
 // not new command permission, and cannot extend tracking or collider life.
 return proof.owner==lineage.proof.owner&&proof.worldGeneration==lineage.proof.worldGeneration&&
    proof.bodyGeneration==lineage.proof.bodyGeneration&&proof.colliderLease==lineage.proof.colliderLease&&
    proof.trackingEpoch==lineage.proof.trackingEpoch&&proof.cancelGeneration==lineage.proof.cancelGeneration&&
    proof.admissionSerial==lineage.proof.admissionSerial&&proof.body==lineage.proof.body&&
    proof.foreign==lineage.proof.foreign&&proof.bodyId==lineage.proof.bodyId&&proof.foreignType==lineage.proof.foreignType&&
    proof.sourceSample&&proof.sourceSample<=lineage.proof.sourceSample&&proof.sourceTick<=lineage.proof.sourceTick&&
    proof.observedTick<=lineage.proof.observedTick&&NativeSolverLineageOwnerCurrentH28(lineage,f,GetTickCount64());
}
void SetFistNativeArm178r15(bool) noexcept {} // compatibility only; delayed native arm replay retired
#include "subsystems/gore/pose_impale_control.inl" // M2_OWNED_SOURCE
static std::atomic<bool> g_synchronized59c10r9{false}; // R59.16C10R9 F4 rung 3 = rung 1 + VR contact authority
static std::atomic<bool> g_intent59c16{false};         // R59.16C10R16 F4 press 1 = SYNCHRONIZED + VR intent rules
static bool ApplyUserModeCore175(kcd2vr::user175::Mode mode,kcd2vr::user175::Mode label) noexcept;
bool CombatSynchronized59c10r9() noexcept {return CombatIntegrated178r22c2Enabled()&&g_synchronized59c10r9.load(std::memory_order_acquire);}
bool CombatIntent59c16() noexcept {return CombatSynchronized59c10r9()&&g_intent59c16.load(std::memory_order_acquire);}
bool ApplyUserMode175(kcd2vr::user175::Mode mode) noexcept {
    using namespace kcd2vr::user175;
    // Hand/contact selection is not a request to retire the weapon or combat
    // owner. The normal full-handling path already supports empty-hand fists.
    // In particular, preserve its current request epoch and in-flight draw.
    if(mode==Mode::Hands&&WeaponControlMode()>=3){
        const bool ready=CalibrationReady()&&NativeArm104()&&ArmCombatInput101(true)&&ArmCloseCombat175();
        if(!ready){diagnostic_trace::Write("R95 HAND_MODE readiness=UNAVAILABLE currentHandling=PRESERVED retry=NEXT_SELECTION");return false;}
        g_normal_fists44.store(true,std::memory_order_release);
        if(CombatCensusMode()!=4)SetCombatCensusMode(4);
        diagnostic_trace::Write("R95 HAND_MODE contacts=ON weaponAndCombatOwner=PRESERVED");
        return true;
    }
    // Retired public direct route cannot be revived through an old numeric menu request.
    if(mode==Mode::CombatContactLab)mode=Mode::CombatSynchronized;
    // R59.16C10R9: SYNCHRONIZED is the complete rung-1 configuration (native admission, defense, combos, clock) plus one
    // switch in the contact path: a contact the native strike window has not reached in time is delivered through the
    // direct native hit instead of being held until it expires. Every other mode clears the switch first.
    // R59.16C10R16: VR INTENT (F4 press 1) is SYNCHRONIZED plus the intent rules (Miki 15 Oct: "WE SHOULD TELL THE GAME what we
    // are doing ... but never play the real strokes"): the native attack is announced once, when the VR stroke commits (zone +
    // prepare + release together, never cancelled, no rhythm gate), the blade contact is the hit at once (energy-scaled native
    // damage), and a perfect block needs the guard to move against the incoming side.
    const bool intent=mode==Mode::CombatIntent;
    const bool sync=mode==Mode::CombatSynchronized||intent;
    g_synchronized59c10r9.store(false,std::memory_order_release);combat_diag_probe::SetSynchronized59c10r9(false);
    g_intent59c16.store(false,std::memory_order_release);combat_diag_probe::SetIntent59c16(false);g_intent_pi59c16.store(false,std::memory_order_release);
    const bool ok=ApplyUserModeCore175(sync?Mode::CombatIntegrated:mode,mode);
    if(ok&&sync){g_synchronized59c10r9.store(true,std::memory_order_release);combat_diag_probe::SetSynchronized59c10r9(true);
        if(intent){g_intent59c16.store(true,std::memory_order_release);combat_diag_probe::SetIntent59c16(true);g_intent_pi59c16.store(true,std::memory_order_release);
            diagnostic_trace::Write("R59.16C10R16 F4_PRESS1 VR_INTENT announce=AT_COMMIT(zone+prepare+release, never cancelled, no rhythm gate) "
                "hit=CONTACT_NOW(direct native hit, energy 30..120 %, once per release) heldWindup=NATIVE_HOLD/FEINT guard=NATIVE_BLOCK "
                "perfectBlock=GUARD_MOVES_AGAINST_INCOMING masterStrike=NATIVE_FACTORY");}
        else diagnostic_trace::Write("C19C4R2 F4_PRESS1 SYNCHRONIZED admission=NATIVE_RUNG1 defense=NATIVE clock=OWNED_ETA contact=VR_AUTHORITY "
            "(strike window first; overdue contact -> direct native hit once; no second producer)");}
    return ok;
}
static bool ApplyUserModeCore175(kcd2vr::user175::Mode mode,kcd2vr::user175::Mode label) noexcept {
    using namespace kcd2vr::user175;
    if(mode!=Mode::Hands&&kcd2vr::hands59c7f::rung.load()==2){kcd2vr::hands59c7f::clear();kcd2vr::hands59c7f::rung=0;}
    if(mode==Mode::Off)(void)SetInteractionExperiment36(false);
    if(mode!=Mode::Off&&!CalibrationReady()){
        diagnostic_trace::Write("V66.175 MODE state=BLOCKED reason=CALIBRATION_REQUIRED recorders=UNCHANGED");return false;
    }
    const auto wanted=configuration(mode);
    g_normal_fists44.store(false,std::memory_order_release);
    kcd2vr::motion59c4::selected.store(false,std::memory_order_release);
    kcd2vr::motion59c4::attempts.clear();
    // Release ownership on the native pump. Do not invoke combat methods on this worker.
    g_combat_integrated178r22c2=false;g_f4_fists_tick178r22c2=0;NativeTimingCancel178r22c2();
    g_combat_experiment175=false;g_physical_combat177=false;g_combat_full178r10=false;g_combat_rung178r15=0;g_experiments169=false;RevokeMasterIntent168();
    kcd2vr::contact58c1::latest.publish({});
    kcd2vr::contact58c1::experiment.store(false,std::memory_order_release);
    g_flinch_active178r11.publish({});g_impale_pin178r15.publish({}); // V66.178R16: no flinch/latch candidate survives a mode change
    combat_diag_probe::SetGeometryHits178r27(false); // V66.178R27: geometry hits belong to the geometry rung only
    kcd2vr::vr_combat111::hideNativeClipsActive178r28.store(false,std::memory_order_relaxed);
    kcd2vr::attack_xi178r8::cancel(std::uint64_t(GetTickCount64())); // V66.178R18 (review F05): a prepare still down leaves as abort (A, then RT-up), nothing stays pending
    diagnostics::parry176::Clear();CancelProceduralKick176();
    kcd2vr::contact176::policy.fetch_add(1,std::memory_order_acq_rel);
    g_special_pending168=false;g_phys_abort6698=true;g_phys_input6698=false;
    g_fist_epoch101.fetch_add(1,std::memory_order_acq_rel);
    g_f11_test167=0;g_f11_theory_phase=0;
    SetWeaponControlMode(0);SetCombatCensusMode(0);
    SetMiscInteractionDiagMode(0);SetHolsterCombatDiagMode(0);
    if(mode!=Mode::Off){
        SetHolsterCombatDiagMode(wanted.combat);SetMiscInteractionDiagMode(wanted.interaction);
        SetCombatCensusMode(wanted.pose);
        if(wanted.weapon)SetWeaponControlMode(wanted.weapon);
        if(CombatCensusMode()!=wanted.pose||WeaponControlMode()!=wanted.weapon||
           HolsterCombatDiagMode()!=wanted.combat||MiscInteractionDiagMode()!=wanted.interaction){
            (void)ApplyUserMode175(Mode::Off);
            diagnostic_trace::Write("V66.175 MODE state=BLOCKED reason=NATIVE_PREREQUISITE recorders=UNCHANGED");return false;
        }
        if((wanted.pose==5||mode==Mode::CombatIntegrated||mode==Mode::CombatGeometry||mode==Mode::CombatContactLab)&&(!NativeArm104()||!ArmCombatInput101(true)||!ArmCloseCombat175())){ // C44: exact-empty hands may box without enabling an F4 combat experiment
            (void)ApplyUserMode175(Mode::Off);
            diagnostic_trace::Write("V66.175 MODE state=BLOCKED reason=CLOSE_COMBAT_CONTRACT recorders=UNCHANGED");return false;
        }
        if(!wanted.experiment&&wanted.pose>=4&&FullWeaponMode120(wanted.weapon)){
            const bool ready44=NativeArm104()&&ArmCombatInput101(true)&&ArmCloseCombat175();
            g_normal_fists44.store(ready44,std::memory_order_release);
            if(!ready44)diagnostic_trace::Write("C44 EMPTY_HAND_COMBAT unavailable=NATIVE_CONTRACT normal_weapon_handling=RETAINED");
        }
        if(wanted.experiment&&!ArmMasterGate168()){
            (void)ApplyUserMode175(Mode::Calibrated);
            diagnostic_trace::Write("V66.175 MODE state=BLOCKED reason=COMBAT_EXPERIMENT_CONTRACT baseline=RETAINED recorders=UNCHANGED");return false;
        }
        if(wanted.experiment&&!combat_diag_probe::ArmF4ContactGate176()){
            (void)ApplyUserMode175(Mode::Calibrated);
            diagnostic_trace::Write("V66.176 MODE state=BLOCKED reason=F4_CONTACT_CONTRACT baseline=CALIBRATED recorders=UNCHANGED");return false;
        }
        if((mode==Mode::CombatIntegrated||mode==Mode::CombatGeometry||mode==Mode::CombatContactLab)&&!ArmAttackTiming178r22c2()){
            (void)ApplyUserMode175(Mode::Calibrated);
            diagnostic_trace::Write("V66.178R22C2 MODE state=BLOCKED reason=ATTACK_TIMING_CONTRACT");return false;
        }
        if(wanted.experiment)ExternalPrepareTakeover176();
        g_physical_combat177.store(wanted.physical177,std::memory_order_release);
        g_combat_full178r10.store(wanted.full178r10,std::memory_order_release);g_combat_rung178r15.store(wanted.rung178r15,std::memory_order_release);
        if(wanted.rung178r15==4)diagnostic_trace::Write("V66.178R15 F4_RUNG4 coach=MASTER_PERMIT_HAPTIC combos=NATIVE_STATE_MACHINE(zone sequence from our strikes) window=NATIVE");
        if(wanted.rung178r15==5)diagnostic_trace::Write("V66.178R15 F4_RUNG5 impale=LATCH_OURS(tip pinned to the struck body, pull-out releases) ragdoll=NATIVE_DEATH_ROWS_ONLY damage=UNMODIFIED");
        if(mode==Mode::CombatIntegrated||wanted.rung178r15==6||wanted.rung178r15==7){combat_diag_probe::SetGeometryLiveAction178r35(true);combat_diag_probe::SetGeometryHits178r27(true);kcd2vr::vr_combat111::hideNativeClipsActive178r28.store(true,std::memory_order_relaxed);diagnostic_trace::Write("R59C3 F4_CONTACT_POLICY hits=GESTURE_BOUND_CONTACT_ONLY timing=PHYSICAL_CONTACT_NATIVE_COORDINATOR authoredSweep=DENIED guard=POSTURE_NATIVE_WINDOW paired=NATIVE");}
        // V66.178R59.6: F4 rung 3 IS the VR backend (no INI needed): blade-driven attack state + direct native hits.
        // Rung 2 keeps ChatGPT's preview, with consequences only when VrAttackBackend=vr.
        const bool rung3vr596=mode==Mode::CombatContactLab;
        const bool motionPreview59c4=(mode==Mode::CombatGeometry&&kcd2vr::motion59c4::previewRequested.load())||rung3vr596;
        combat_diag_probe::SelectVrBackendRung596(rung3vr596);
        kcd2vr::motion59c4::selected.store(motionPreview59c4,std::memory_order_release);
        kcd2vr::motion59c4::nativeStarts.store(0);
        if(motionPreview59c4)diagnostic_trace::Write("R59C4 MOTION_OWNED PREVIEW_ONLY timing=VR_ATTEMPT_CONTACT ordinaryInput=SUPPRESSED damage=DISABLED effort=UNIMPLEMENTED threat=UNIMPLEMENTED hostility=UNIMPLEMENTED defense=UNIMPLEMENTED nativeFallback=NONE");
        g_combat_integrated178r22c2.store(mode==Mode::CombatIntegrated,std::memory_order_release);
        g_combat_experiment175.store(wanted.experiment,std::memory_order_release);
        kcd2vr::contact58c1::experiment.store(mode==Mode::CombatContactLab,std::memory_order_release);
        if(mode==Mode::CombatIntegrated)diagnostic_trace::Write("R59C3 F4_RUNG1 integrated=WEAPONS_BOXING_BOW_HOLSTERS ordinaryMelee=CONTACT_ONLY timing=CONTACT_COORDINATOR rules=NATIVE HUD=R59C2");
        if(wanted.full178r10&&mode!=Mode::CombatIntegrated)diagnostic_trace::Write("V66.178R10 F4_RUNG3 rules=VR autoLock=POLICY_CVARS kick=NATIVE_ATTACK_SPECIAL_LT_HOLD damageUnits=UNMODIFIED");
        if(wanted.physical177&&mode!=Mode::CombatIntegrated)diagnostic_trace::Write("V66.177R1 F4_RUNG2 drive=BOUNDED_MASS_INERTIA_GRAVITY interval=COMPLETED_PAIR nativeWindow=REQUIRED damageUnits=UNMODIFIED material=UNQUALIFIED ragdoll=UNIMPLEMENTED");
        if(wanted.experiment)diagnostic_trace::Write("V66.176 F4_READY block=NATIVE_TIMING motionParry=QUALIFIED_REPRESS contactGate=ORDINARY_HENRY_ONLY R3=PROCEDURAL_VISUAL_CONTACT damageKick=DISABLED nativeKickAdmission=MISSING staminaKick=UNPROVEN");
    }
    char m[320]{};std::snprintf(m,sizeof(m),
        "V66.175 MODE state=%s pose=%d weapon=%d combat=%d interaction=%d F4=%d recorders=UNCHANGED calibration=PRESERVED",
        name(label),CombatCensusMode(),WeaponControlMode(),HolsterCombatDiagMode(),MiscInteractionDiagMode(),CombatExperiment175Enabled()); // R16: the requested name
    diagnostic_trace::Write(m);
    (void)PrepareGameplayResourcesD84();
    return true;
}

#include "subsystems/vrik/body_follow_controls.inl" // M2_OWNED_SOURCE
#include "subsystems/ranged_reload/bow_string_follow_control.inl" // M2_OWNED_SOURCE
#include "subsystems/ranged_reload/bow_reload_controls.inl" // M2_OWNED_SOURCE
#include "subsystems/throw_drop/holster_controls.inl" // M2_OWNED_SOURCE
void NotifyNativeWeaponImpact136R2(float damage,float stamina,float sampleEnergy,bool blocked,bool perfectBlock,bool masterStrike,std::uint32_t targetId) noexcept {
    if(g_weapon_control_mode.load(std::memory_order_acquire)<6||!targetId||!kcd2vr::haptics101::enabled.load()||!kcd2vr::haptics101::allowed.load())return;
    const auto now=std::uint64_t(GetTickCount64());PhysFrame6698 frame{};
    if(!g_phys_frame6698.read(frame)||!frame.input.allowed||!frame.input.tick||now<frame.input.tick||now-frame.input.tick>120)return;
    int side=-1;for(int s=0;s<2;++s)if(frame.output.owned[s]&&frame.profiles[s].key&&frame.profiles[s].melee()&&frame.input.tracked[s]){side=s;break;}
    if(side<0)return;
    static std::atomic<std::uint64_t> lastTick{0};static std::atomic<std::uint32_t> lastTarget{0};
    const auto prev=lastTick.load(std::memory_order_relaxed);const auto oldTarget=lastTarget.load(std::memory_order_relaxed);
    if(prev&&now>=prev&&now-prev<45&&oldTarget==targetId)return;
    lastTick.store(now,std::memory_order_relaxed);lastTarget.store(targetId,std::memory_order_relaxed);
    const double e=std::isfinite(sampleEnergy)?std::clamp(double(sampleEnergy),0.0,8.0):0.0;
    const double d=std::isfinite(damage)?std::clamp(double(damage),0.0,150.0):0.0;
    const double st=std::isfinite(stamina)?std::clamp(double(stamina),0.0,150.0):0.0;
    double amp=.18+.045*e+.0018*d+.0007*st;if(blocked)amp=std::max(amp,.30);if(perfectBlock)amp=std::max(amp,.42);if(masterStrike)amp=std::max(amp,.52);
    amp=std::clamp(amp,.16,.68);const int ms=std::clamp(int(std::lround(28+e*2.5+(blocked?6:0)+(perfectBlock?5:0))),28,55);
    const auto key=frame.profiles[side].key; const bool sent=kcd2vr::haptics101::publish_event(side,key,float(amp),ms,now);
    const int other=1-side;const bool support=frame.output.support[side]&&frame.output.owned[other]&&frame.input.tracked[other];
    if(support)(void)kcd2vr::haptics101::publish_event(other,key,float(std::clamp(amp*.68,.10,.42)),ms,now);
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    static std::atomic<std::uint64_t> count{0},lastLog{0};const auto n=count.fetch_add(sent?1:0,std::memory_order_relaxed)+(sent?1:0);
    auto prevLog=lastLog.load(std::memory_order_relaxed);
    if(sent&&(n<=20||now-prevLog>=5000)&&lastLog.compare_exchange_strong(prevLog,now,std::memory_order_acq_rel)){
        char m[320]{};std::snprintf(m,sizeof(m),"V66.136R2 NATIVE_HAPTIC target=%u side=%c amp=%.2f ms=%d damage=%.2f stamina=%.2f energy=%.2f blocked=%d perfect=%d master=%d support=%d count=%llu",
            targetId,side?'L':'R',amp,ms,double(d),double(st),double(e),blocked?1:0,perfectBlock?1:0,masterStrike?1:0,support?1:0,(unsigned long long)n);diagnostic_trace::Write(m);
    }
}

bool NativeHandPhysicsH24() noexcept {return g_native_hand_physics_h24.load(std::memory_order_acquire);}
void SetNativeHandPhysicsH24(bool on) noexcept {
    // Installation follows an explicit request. Retain that request if startup
    // is not ready yet; the startup owner retries only when it is selected.
    if(on&&!g_native_hand_physics_h24.load(std::memory_order_acquire))(void)PrepareNativeHandPhysicsH24();
    g_native_hand_physics_h24.store(on,std::memory_order_release);
    if(!on)for(int side=0;side<2;++side)handPresenceH24::CancelPresenceH24(side);
}
bool PrepareNativeHandPhysicsH24() noexcept {
    (void)handPresenceH24::SetPresencePredicateH24(&HandPresenceCurrentH24);
    const bool phaseReady=kcd2vr::native_weapon_body_t4::Install(); // weapon control stays OFF; independent of F9
    const bool handsReady=phaseReady&&handPresenceH24::PresenceReadyH24();
    if(!handsReady)diagnostic_trace::Write("NATIVE_HAND_PHYSICS unavailable: qualified phase/hand body byte-pins or clearance preflight did not install; no physical hand authority granted");
    return handsReady;
}
bool PreciseContacts44() noexcept {return ph::preciseContacts44.load(std::memory_order_relaxed);}
void SetPreciseContacts44(bool on) noexcept {if(ph::preciseContacts44.exchange(on)!=on)g_contact_control_epoch6695.fetch_add(1,std::memory_order_acq_rel);
    if(ph::handsModeDs2.load(std::memory_order_relaxed)!=2)ph::handsModeDs2.store(on?1:0,std::memory_order_release);}
// D39: this threshold reports shoulder travel; it must never clamp a tracked wrist.
void SetShoulderReachD38(float warningMeters) noexcept {kcd2vr::shoulder_reach_d38::maxShiftM.store(std::isfinite(warningMeters)?std::clamp(warningMeters,.02f,.40f):.15f,std::memory_order_relaxed);}
float GetShoulderReachD38() noexcept {return kcd2vr::shoulder_reach_d38::maxShiftM.load(std::memory_order_relaxed);}
bool HandSeparationDs1() noexcept {return ph::handSeparationDs1.load(std::memory_order_acquire);}
void SetHandSeparationDs1(bool on) noexcept {if(ph::handSeparationDs1.exchange(on,std::memory_order_acq_rel)!=on)g_contact_control_epoch6695.fetch_add(1,std::memory_order_acq_rel);}
int HandsModeDs2() noexcept {const int m=ph::handsModeDs2.load(std::memory_order_acquire);return m==2?2:(ph::preciseContacts44.load(std::memory_order_relaxed)?1:0);}
void SetHandsModeDs2(int mode) noexcept {mode=mode<0||mode>2?1:mode;
    if(ph::handsModeDs2.exchange(mode,std::memory_order_acq_rel)!=mode)g_contact_control_epoch6695.fetch_add(1,std::memory_order_acq_rel);
    SetPreciseContacts44(mode==1);}
void SetWeaponConstraints134(bool drive,bool twoHand) noexcept {g_weapon_constraint_drive134.store(drive);g_weapon_two_hand_constraint134.store(twoHand);
    char m[160]{};std::snprintf(m,sizeof(m),"V66.134 WEAPON_CONSTRAINTS drive129=%d twoHand130=%d (INI)",drive?1:0,twoHand?1:0);diagnostic_trace::Write(m);}
int WeaponControlMode() noexcept { return WeaponControlModeImpl(); }
#include "subsystems/interactions/lifecycle_controls.inl" // M2_OWNED_SOURCE
#include "subsystems/ranged_reload/reload_policy.inl" // M2_OWNED_SOURCE
void WeaponControlTick() noexcept {
    ServiceBridges168();ServiceLifecycle165();ServiceReloadDiagnostics175();
    if(const auto until=g_lifecycle_until165.load();until&&(GetTickCount64()>=until||!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64())))
        EndLifecycle165(GetTickCount64()>=until?"TIME_LIMIT":"RECORDER_OFF"); NativeService104(); ServiceAlignVetoReceipts25(); ServiceRootMotionArm125(); ServiceCombatCapture101(); animation_filter::ServiceAliasCatalog(g_arm_capture_active66100.load()||g_census_mode.load()>=2||g_weapon_control_mode.load()>=4); ServiceFingerReferenceIo6694(); ServiceCalibrationIo66100(); WeaponControlTickImpl(); }

// F5 exact actor action/target observer; signature includes the fifth stack float
// at entry RSP+0x28, confirmed in +0x702288. Opaque return bits are passed unchanged.
#include "vrik/native_door19c4.inl"
using InteractiveActionFn6689=std::uintptr_t(__fastcall*)(void*,const char*,std::uint32_t,bool,float);
static std::uintptr_t DoorNativeCall66(InteractiveActionFn6689 original,void* actor,const char* name,std::uint32_t target,
                                     bool force,float speed,const DoorRequestWitness38& witness,int side,std::uint64_t now) noexcept {
    const auto saved=g_door_entry66;g_door_entry66=DoorEntry66(witness,target,side,now);
    const auto result=original?original(actor,name,target,force,speed):0;g_door_entry66=saved;return result;
}
static std::optional<DetourModKit::hook::Hook> g_interaction_hook6689;
static InteractiveActionFn6689 g_interaction_original6689=nullptr;
static bool InteractionActionReady175() noexcept {return g_interaction_original6689!=nullptr;}
static std::atomic<std::uint64_t> g_interaction_sequence6689{0};
#include "vrik/interaction_body122.inl"
static std::uint64_t __fastcall InteractiveActionDetour6689Body(void* actor,const char* name,std::uint32_t targetId,bool force,float speed) noexcept;
// MSVC C2712: the cost scope (a destructor) lives in a wrapper; the SEH body is its own function.
static std::uint64_t __fastcall InteractiveActionDetour6689(void* actor,const char* name,std::uint32_t targetId,bool force,float speed) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::InteractiveAction};return InteractiveActionDetour6689Body(actor,name,targetId,force,speed);}
static std::uint64_t __fastcall InteractiveActionDetour6689Body(void* actor,const char* name,std::uint32_t targetId,bool force,float speed) noexcept {
    const auto captureNow175=std::uint64_t(GetTickCount64());
    const bool interactionCapture175=diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,captureNow175);
    // Snapshot source while native owns the arguments. There is no worker polling.
    void* actorEntity=nullptr;bool henry=actor&&Read(static_cast<unsigned char*>(actor)+0x38,actorEntity)&&actorEntity&&actorEntity==g_last_player_entity.load(std::memory_order_acquire);
    // C38: bind the source before the synchronous native call. Its nested work
    // may publish another hand's nearer candidate before returning acceptance.
    InteractionHandContext158 requestHand38{};
    DoorRequestWitness38 requestDoor38{};
    requestDoor38.epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    requestDoor38.player=reinterpret_cast<std::uintptr_t>(actorEntity);
    if(henry&&g_interaction_hand_context158.read(requestHand38)&&requestHand38.valid&&
       captureNow175>=requestHand38.tick&&captureNow175-requestHand38.tick<=80)
        (void)interaction_alignment::ExactDoorCandidate38(targetId,requestHand38.side,captureNow175,requestDoor38.candidate);
    if(henry&&requestHand38.valid&&captureNow175>=requestHand38.tick&&captureNow175-requestHand38.tick<=80)
        CaptureDoorRequest46(requestDoor38,targetId,requestHand38.side,captureNow175);
    // V66.178R10/R13: KCD2's own finger pose owns the using hand only once the game ACCEPTED Henry's
    // interactive action (non-zero return = the action object; the R7 log shows 0x18A57CE7A00 for doors),
    // for the hand that published the hover/use ray just before. A rejected request grants nothing.
    // (V66.178R7-R9 granted it on every sent Use pulse, i.e. on every Grip squeeze of an empty hand.)
    auto grantHandUse178r13=[&](std::uint64_t returned){
        if(!henry||!returned)return;const auto& hc=requestHand38;const auto hcNow=std::uint64_t(GetTickCount64());
        if(hc.valid&&hcNow>=hc.tick&&hcNow-hc.tick<=80){
            NotifyNativeHandUse(hc.side,1800);
            ArmDoor19c4(targetId,returned,name,hc.side,hcNow,&requestDoor38);
            if(hc.side>=0&&hc.side<2)g_native_use_target59c10[hc.side].store(targetId,std::memory_order_release);
            if(interactionCapture175&&kcd2vr::workstation178r20c1::ownsWrist(hcNow)){
                char label[96]{};if(name)for(unsigned i=0;i<sizeof(label)-1;++i){if(!Read(name+i,label[i])||!label[i])break;}
                char line[304]{};std::snprintf(line,sizeof(line),
                    "C10 F5_HAND_HOLD_ACTION side=%d object=%u action=%s nativeReturn=%p fingers=NATIVE wrist=CONTROLLER elementDrive=NATIVE_AUTHORED",
                    hc.side,targetId,label,reinterpret_cast<void*>(returned));diagnostic_trace::Write(line);
            }
        }else if(interactionCapture175){
            // Accepted native door, but no fresh hand context: receipt only.
            ArmDoor19c4(targetId,returned,name,-1,hcNow);
        }};
    if(!interactionCapture175&&!ReloadDiagnostics175()){
        const auto fast=DoorNativeCall66(g_interaction_original6689,actor,name,targetId,force,speed,requestDoor38,requestHand38.side,captureNow175);
        grantHandUse178r13(fast);return fast;}
    char action[192]{};bool nameOk=false;
    if(henry&&name){for(std::size_t i=0;i<sizeof(action)-1;++i){char c=0;if(!Read(name+i,c))break;action[i]=c;if(!c){nameOk=true;break;}}}
    const bool reloadObserve175=ReloadDiagnostics175();
    const bool observe=henry&&nameOk&&interactionCapture175;
    std::uint64_t seq=0;il165::Event actionObservation36{};
    if(observe){
        seq=g_interaction_sequence6689.fetch_add(1)+1;
        const auto stamp=animation_filter::DiagnosticPlaybackStamp();char context[448]{};
        std::snprintf(context,sizeof(context),"V66.90-F5-CONTEXT tick=%llu epoch=%llu requestSeq=%llu playbackSeq=%llu playbackTick=%llu posePolicy=%s requestOnly=1 completion=UNKNOWN",
            (unsigned long long)GetTickCount64(),(unsigned long long)(diagnostics::lab::Snapshot()>>4),(unsigned long long)seq,(unsigned long long)stamp.serial,(unsigned long long)stamp.tick,diagnostics::lab::NativeReference()?"NATIVE_REFERENCE":"VRIK_REQUESTED");diagnostic_trace::Write(context);
        void* system=nullptr;void* vf=nullptr;void* get=nullptr;void* target=nullptr;
        if(targetId&&Read(reinterpret_cast<void*>(WhBase()+0x492D8A0ull),system)&&system&&Read(system,vf)&&vf&&Read(static_cast<unsigned char*>(vf)+0x70,get)&&get) {
            __try{target=reinterpret_cast<void*(__fastcall*)(void*,std::uint32_t)>(get)(system,targetId);}__except(EXCEPTION_EXECUTE_HANDLER){target=nullptr;}
        }
        actionObservation36=LifecycleEvent165(il165::Kind::ActionRequest,0x702288);
        actionObservation36.call=++g_lifecycle_call165;actionObservation36.related=seq;
        actionObservation36.entity=reinterpret_cast<std::uintptr_t>(target);
        if(!LifecycleEntity165(actionObservation36.entity,actionObservation36.target)||actionObservation36.target!=targetId){actionObservation36.entity=0;actionObservation36.target=0;}
        std::snprintf(actionObservation36.label,sizeof(actionObservation36.label),"%.63s",action);
        InteractionHandContext158 hc36{};
        if(g_interaction_hand_context158.read(hc36)){actionObservation36.sourceTick=hc36.tick;
            if(hc36.valid&&captureNow175>=hc36.tick&&captureNow175-hc36.tick<=120){
                actionObservation36.side=hc36.side;actionObservation36.valid=1;
                actionObservation36.point[0]=hc36.x;actionObservation36.point[1]=hc36.y;actionObservation36.point[2]=hc36.z;}}
        requestDoor38.diagnosticActionSeqH4=seq;
        {namespace h4=diagnostics::interaction_cycle_h4;h4::Event e{};e.stage=h4::Stage::ActionRequest;e.tick=captureNow175;e.sourceTick=requestHand38.tick;e.poseEpoch=requestDoor38.epoch;e.player=requestDoor38.player;e.actor=reinterpret_cast<std::uintptr_t>(actor);e.target=targetId;e.side=requestHand38.valid?requestHand38.side:-1;e.actionSeq=seq;
         e.token=requestDoor38.input46.token;e.press=requestDoor38.receiptInput63.press;e.physicalUse=requestDoor38.receiptPhysical63.serial;e.trackingEpoch=requestDoor38.receiptInput63.trackingEpoch;e.generation=requestDoor38.receiptInput63.sourceGeneration;
         h4::copy(e.reason,sizeof(e.reason),"EXACT_702288_REQUEST_NOT_ACCEPTANCE");h4::copy(e.label,sizeof(e.label),action);h4::observe(e,captureNow175);}
        LifecycleWatch165(actionObservation36);LifecycleEmit165(actionObservation36);
        WatchInteractionBody122(target,targetId,seq,action);
        ProbeInteractionBody147(target,targetId,seq);
        void* character=target?CharacterForEntity6689(target):nullptr;void* anim=nullptr;
        if(character&&!ValidateCharInstance(character,anim))anim=nullptr;
        animation_filter::SetInteractionAnimationTarget(anim,targetId,seq);
        char m[600]{};std::snprintf(m,sizeof(m),"V66.90-F5-ACTION REQUEST seq=%llu Henry=1 actor=%p entity=%p targetId=%u targetEntity=%p objectAnim=%p name='%s' force=%d speed=%.3f lifecycle=REQUEST_NOT_COMPLETION handoff=NONE",(unsigned long long)seq,actor,actorEntity,targetId,target,anim,action,force,double(speed));diagnostic_trace::Write(m);
    }
    const auto result=DoorNativeCall66(g_interaction_original6689,actor,name,targetId,force,speed,requestDoor38,requestHand38.side,captureNow175);
    grantHandUse178r13(result);
    if(reloadObserve175&&henry&&nameOk){char reload[512]{};std::snprintf(reload,sizeof(reload),
        "V66.175 F11-RELOAD RECOVERY_ACTION target=%u action=%s nativeReturn=0x%llX recovery=REQUEST_RETURN_ONLY ammoDelta=UNKNOWN writes=NONE",
        targetId,action,(unsigned long long)result);diagnostic_trace::Write(reload);}
    if(observe){
        actionObservation36.kind=il165::Kind::ActionReturn;actionObservation36.tick=GetTickCount64();
        actionObservation36.returnBits=result;LifecycleEmit165(actionObservation36);
        {namespace h4=diagnostics::interaction_cycle_h4;h4::Event e{};e.stage=h4::Stage::ActionReturn;e.tick=actionObservation36.tick;e.sourceTick=requestHand38.tick;e.poseEpoch=requestDoor38.epoch;e.player=requestDoor38.player;e.actor=reinterpret_cast<std::uintptr_t>(actor);e.target=targetId;e.side=requestHand38.valid?requestHand38.side:-1;e.actionSeq=seq;e.action=std::uintptr_t(result);e.code=result?1:0;
         e.token=requestDoor38.input46.token;e.press=requestDoor38.receiptInput63.press;e.physicalUse=requestDoor38.receiptPhysical63.serial;
         h4::copy(e.reason,sizeof(e.reason),result?"NATIVE_ACTION_OBJECT_RETURNED_NOT_COMPLETION":"NATIVE_ACTION_RETURN_ZERO");h4::copy(e.label,sizeof(e.label),action);h4::observe(e,e.tick);}
        void* state=nullptr;std::uint32_t stored=0;const bool have=Read(static_cast<unsigned char*>(actor)+0x990,state)&&state&&Read(static_cast<unsigned char*>(state)+0x2F0,stored);
        char m[384]{};std::snprintf(m,sizeof(m),"V66.90-F5-ACTION RETURN seq=%llu targetRequested=%u targetStored=%u exactTargetReadback=%d returnBits=0x%llX nativeCompletion=UNKNOWN",(unsigned long long)seq,targetId,stored,have&&stored==targetId,(unsigned long long)result);diagnostic_trace::Write(m);
    }
    return result;
}
// V66.178R13 F5 item-cycle observers: CarryableItemBind CanPickUp (+174C170) and PickUp (+2AC04E0),
// read-only, logged while the interaction recorder runs. Both prologues are 18 clean bytes
// (mov [rsp+8],rbx; mov [rsp+10h],rsi; push rdi; sub rsp,30h; mov rbx,r9 ...), unlike the V175
// PICKUP mid-sites that were 6 bytes apart. Arguments are opaque: an argument is reported as an
// entity id only when it carries the exact CEntity vtable.
using PickupFn178r13=std::uint64_t(__fastcall*)(void*,void*,void*,void*);
static std::optional<DetourModKit::hook::Hook> g_pickup_hook178r13,g_canpickup_hook178r13;
static PickupFn178r13 g_pickup_original178r13=nullptr,g_canpickup_original178r13=nullptr;
static std::atomic<std::uint64_t> g_pickup_serial178r13{0};
static void PickupLog178r13(const char* fn,void* a,void* b,void* c,void* d,std::uint64_t result) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return;
    // Review F04 (static): R8D/R9D are 32-bit registry keys (routes +0x180/+0x190), not entity pointers; the
    // wrapper's return is the script-marshalling result (+0x711B5C), not the inner boolean nor an inventory delta.
    void* args[2]{a,b};std::uint32_t ids[2]{};bool henry[2]{};ReadScope101R2 reads;
    for(int i=0;i<2;++i){if(args[i]&&PhysVft6698(args[i],0x4072C78)){(void)Read(static_cast<unsigned char*>(args[i])+0xC,ids[i]);henry[i]=args[i]==g_last_player_entity.load(std::memory_order_acquire);}}
    const std::uint32_t keyC=std::uint32_t(reinterpret_cast<std::uintptr_t>(c)&0xFFFFFFFFu),keyD=std::uint32_t(reinterpret_cast<std::uintptr_t>(d)&0xFFFFFFFFu);
    InteractionHandContext158 hc{};const bool hand=g_interaction_hand_context158.read(hc)&&hc.valid&&now>=hc.tick&&now-hc.tick<=400;
    char m[420]{};std::snprintf(m,sizeof(m),"V66.178R13 ITEM_CYCLE stage=%s serial=%llu tick=%llu self=%p arg1=%p entityIds=%u/%u henryArg=%d%d key8=%u key9=%u scriptMarshalReturned=0x%llX innerBool=NOT_OBSERVED hoverHand=%d inventoryChange=NOT_READ constraint=NONE",
        fn,(unsigned long long)g_pickup_serial178r13.fetch_add(1),(unsigned long long)now,a,b,ids[0],ids[1],henry[0],henry[1],keyC,keyD,(unsigned long long)result,hand?hc.side:-1);
    diagnostic_trace::Write(m);
}
static std::uint64_t __fastcall PickupDetour178r13(void* a,void* b,void* c,void* d) noexcept {
    const auto r=g_pickup_original178r13?g_pickup_original178r13(a,b,c,d):0;PickupLog178r13("PICKUP",a,b,c,d,r);return r;}
static std::uint64_t __fastcall CanPickupDetour178r13(void* a,void* b,void* c,void* d) noexcept {
    const auto r=g_canpickup_original178r13?g_canpickup_original178r13(a,b,c,d):0;PickupLog178r13("CAN_PICKUP",a,b,c,d,r);return r;}
static bool InstallPickupObservers178r13(std::uintptr_t base) noexcept {
    constexpr unsigned char prologue[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x30,0x49,0x8B,0xD9,0x49,0x8B,0xF8,0x48,0x8B,0xF2};
    struct Site{const char* name;std::uintptr_t rva;std::optional<DetourModKit::hook::Hook>* hook;PickupFn178r13* original;void* detour;};
    Site sites[]{{"kcd2vr_pickup178r13",0x2AC04E0,&g_pickup_hook178r13,&g_pickup_original178r13,reinterpret_cast<void*>(&PickupDetour178r13)},
                 {"kcd2vr_canpickup178r13",0x174C170,&g_canpickup_hook178r13,&g_canpickup_original178r13,reinterpret_cast<void*>(&CanPickupDetour178r13)}};
    bool any=false;
    for(auto& s:sites){
        if(*s.hook)continue;
        if(!Safe(reinterpret_cast<void*>(base+s.rva),sizeof(prologue))||std::memcmp(reinterpret_cast<void*>(base+s.rva),prologue,sizeof(prologue))){
            char m[160]{};std::snprintf(m,sizeof(m),"V66.178R13 ITEM_CYCLE observer=%s result=PROLOGUE_MISMATCH (not installed)",s.name);diagnostic_trace::Write(m);continue;}
        DetourModKit::hook::InlineRequest req{s.name,DetourModKit::hook::Target{DetourModKit::Address{base+s.rva}},DetourModKit::hook::Options{}};
        auto r=DetourModKit::hook::inline_at(std::move(req),s.detour);if(!r)continue;
        *s.hook=std::move(*r);*s.original=(*s.hook)->original<PickupFn178r13>();
        if(!*s.original||!(*s.hook)->enable()){s.hook->reset();*s.original=nullptr;continue;}
        any=true;
    }
    return any;
}
static bool InstallInteractionObserver6689(std::uintptr_t base) noexcept {
    constexpr unsigned char expected[]{0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x6C,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57,0x48,0x81,0xEC,0xB0};
    if(!Safe(reinterpret_cast<void*>(base+0x702288),sizeof(expected))||std::memcmp(reinterpret_cast<void*>(base+0x702288),expected,sizeof(expected)))return false;
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6689_actor_interactive_action",DetourModKit::hook::Target{DetourModKit::Address{base+0x702288}},DetourModKit::hook::Options{}};
    auto r=DetourModKit::hook::inline_at(std::move(req),&InteractiveActionDetour6689);if(!r)return false;
    g_interaction_hook6689=std::move(*r);g_interaction_original6689=g_interaction_hook6689->original<InteractiveActionFn6689>();if(!g_interaction_original6689||!g_interaction_hook6689->enable()){g_interaction_hook6689.reset();g_interaction_original6689=nullptr;return false;}
    DoorBindingsInstall66();(void)DoorMotionNativeArm66();return true;
}

#include "subsystems/doors/hand_only_h40_current.inl"
#include "subsystems/interactions/native_item_context_h41.inl"
bool Install() noexcept {
    kcd2vr::alchemy_finish_cb1::SetCurrentReader(&AlchemyFinishCurrentH41);
    kcd2vr::grip_stow_cb1::SetCurrentReader(&GripStowCurrentH41);
    kcd2vr::hand_only_door_h40::currentReader.store(&HandOnlyDoorCurrentH40,std::memory_order_release);
    kcd2vr::hand_only_door_h40::inputIdentityReader.store(&HandOnlyDoorInputIdentityH40,std::memory_order_release);
    g_wristOwnedByController59c12=&action_time_owner59c11::WristOwnedByController;
    g_readReloadPassengerXR34=&ReadReloadPassengerXR34;
    g_currentReloadPassengerXR34=&ReloadPassengerCurrentXR34;
    g_prepareReloadPoseXR34=&PrepareReloadPoseXR34;
    kcd2vr::crossbow_native_xr34::identityReader.store(&ReloadNativeIdentityReaderXR34,std::memory_order_release);
    kcd2vr::crossbow_native_xr34::bindingReader.store(&RangedNativeBindingXR34,std::memory_order_release);
    kcd2vr::crossbow_native_xr34::registrationReader.store(&RangedRegistrationReadyXR34,std::memory_order_release);
    g_frame_neutral51=+[](std::uint64_t now) noexcept {
        // BowCameraNeutral35 historically treats a disabled bow mode as neutral.
        // This anatomical measurement still needs known empty/melee equipment.
        return BowHandlingMode59c9(g_weapon_control_mode.load())?BowCameraNeutral35(now):NativeHeadAimEquipment120R3()==0;
    };
    g_graspEnabled13=&action_time_owner59c11::NativeGraspEnabled;
    g_graspBinding13=&action_time_owner59c11::ReadGraspBinding;
    g_graspAcquisition54=&action_time_owner59c11::ReadAcquisition54;
    kcd2vr::grasp13::foundationReaderD56=&action_time_owner59c11::ReadAcquisitionFoundationD56;
    g_graspPendingUse55=&action_time_owner59c11::ReadPendingUse55;
    g_graspHands13=&action_time_owner59c11::PublishHands;
    g_graspInput55=&action_time_owner59c11::PublishAcquisitionInput55;
    g_graspPlaced13=&action_time_owner59c11::PublishGraspPlacement; // R59.16C10R12B
    g_graspSocket28=&action_time_owner59c11::PublishGraspSocket28;
    const auto base=WhBase();if(!SupportedWhBuild(base))return false;Uninstall();
    if(kcd2vr::instant_pickup_d72::active.load(std::memory_order_acquire))return false;
    if(kcd2vr::roomscale_d76::Enabled()){(void)kcd2vr::native_roomscale_d76::Install();} // New hooks only for the selected trial.
    if(!InstallInteractionObserver6689(base))diagnostic_trace::Write("V66.89-F5 exact action hook unavailable; generic snapshots remain retired");
    if(!InstallPickupObservers178r13(base))diagnostic_trace::Write("V66.178R13 ITEM_CYCLE pickup observers unavailable");
    {DetourModKit::hook::InlineRequest req{"kcd2vr_v640_weapon_solver_correlation",DetourModKit::hook::Target{DetourModKit::Address{base+kSetIkTargetsRva}},DetourModKit::hook::Options{}};auto r=DetourModKit::hook::inline_at(std::move(req),&SetIkDetour);if(!r){Uninstall();return false;}g_setik_hook=std::move(*r);g_setik_original=g_setik_hook->original<SetIkFn>();if(!g_setik_hook->enable()){Uninstall();return false;}}
    {DetourModKit::hook::InlineRequest req{"kcd2vr_v640_skinning_vrik_grip_chain_lab",DetourModKit::hook::Target{DetourModKit::Address{base+kSkinningTransformCoreRva}},DetourModKit::hook::Options{}};auto r=DetourModKit::hook::inline_at(std::move(req),&SkinningTransformCoreDetour);if(!r){Uninstall();return false;}g_skinning_core_hook=std::move(*r);g_skinning_core_original=g_skinning_core_hook->original<SkinningTransformCoreFn>();if(!g_skinning_core_hook->enable()){Uninstall();return false;}}
    diagnostic_trace::Write(InstallStowedRenderD63(base)?
        "D63 STOWED_RENDER exact native draw boundary READY; scoped final-body parent; native actions/cache unchanged":
        "D63 STOWED_RENDER unavailable; original attachment rendering unchanged");
    diagnostic_trace::Write(InstallHenryBounds63(base)?
        "C63 HENRY_RENDER_BOUNDS exact getter/caller READY; fresh committed arms/fingers union; no native bounds writes":
        "C63 HENRY_RENDER_BOUNDS unavailable; native getter unchanged");
    if(!ValidateExactWeaponHooksV6547(base)){diagnostic_trace::Write("V65.47 exact weapon hooks FAIL-CLOSED: prologue mismatch");Uninstall();return false;}
    if(ValidateAttachmentHelperTransformV6573(base)){
        DetourModKit::hook::InlineRequest req{"kcd2vr_v6573_attachment_helper_transform",DetourModKit::hook::Target{DetourModKit::Address{base+kAttachmentHelperTransformRva}},DetourModKit::hook::Options{}};
        auto r=DetourModKit::hook::inline_at(std::move(req),&AttachmentHelperTransformDetourV6573);
        if(r){g_attachment_helper_transform_hook=std::move(*r);g_attachment_helper_transform_original=g_attachment_helper_transform_hook->original<AttachmentHelperTransformFn>();if(g_attachment_helper_transform_hook->enable())diagnostic_trace::Write("V65.73-F12 slt_0 native attachment-helper hook READY WHGame+0xB309DC");else{g_attachment_helper_transform_hook.reset();g_attachment_helper_transform_original=nullptr;}}
    } else diagnostic_trace::Write("V65.73-F12 slt_0 helper hook FAIL-CLOSED prologue mismatch");
    {DetourModKit::hook::InlineRequest req{"kcd2vr_v6547_item_attach_operation",DetourModKit::hook::Target{DetourModKit::Address{base+kItemAttachOperationRva}},DetourModKit::hook::Options{}};auto r=DetourModKit::hook::inline_at(std::move(req),&ItemAttachOperationDetour);if(!r){Uninstall();return false;}g_item_attach_operation_hook=std::move(*r);g_item_attach_operation_original=g_item_attach_operation_hook->original<ItemAttachOperationFn>();if(!g_item_attach_operation_hook->enable()){Uninstall();return false;}}
    (void)InstallInitialDrawCallbackH36r3(base); // optional capability; no inferred completion if unavailable
    {DetourModKit::hook::InlineRequest req{"kcd2vr_v6547_attach_event_dispatch",DetourModKit::hook::Target{DetourModKit::Address{base+kAttachEventDispatchRva}},DetourModKit::hook::Options{}};auto r=DetourModKit::hook::inline_at(std::move(req),&AttachEventDispatchDetour);if(!r){Uninstall();return false;}g_attach_event_dispatch_hook=std::move(*r);g_attach_event_dispatch_original=g_attach_event_dispatch_hook->original<AttachEventDispatchFn>();if(!g_attach_event_dispatch_hook->enable()){Uninstall();return false;}}
    if(ValidateWuidResolverV6548(base)){DetourModKit::hook::InlineRequest req{"kcd2vr_v6548_wuid_entity_resolver",DetourModKit::hook::Target{DetourModKit::Address{base+kWuidResolverRva}},DetourModKit::hook::Options{}};auto r=DetourModKit::hook::inline_at(std::move(req),&WuidResolverDetour);if(r){g_wuid_resolver_hook=std::move(*r);g_wuid_resolver_original=g_wuid_resolver_hook->original<WuidResolverFn>();if(g_wuid_resolver_hook->enable())diagnostic_trace::Write("V65.51-WUID-ENTITY hook ready exact WHGame+0x47C1F8 FILTERED ITEM->ENTITY + ABI-GATED GetWorldTM@+0x100 READ-ONLY");else{g_wuid_resolver_hook.reset();g_wuid_resolver_original=nullptr;}}}else diagnostic_trace::Write("V65.48-WUID-ENTITY hook FAIL-CLOSED prologue mismatch");
    if(ValidateEntityAttachmentUpdateV6688(base)){
        DetourModKit::hook::InlineRequest req{"kcd2vr_v6688_entity_attachment_update",DetourModKit::hook::Target{DetourModKit::Address{base+kEntityAttachmentUpdateRva}},DetourModKit::hook::Options{}};
        auto r=DetourModKit::hook::inline_at(std::move(req),&EntityAttachmentUpdateDetourV6688);
        if(r){g_entity_attachment_update_hook=std::move(*r);g_entity_attachment_update_original=g_entity_attachment_update_hook->original<EntityAttachmentUpdateFn>();if(g_entity_attachment_update_original&&g_entity_attachment_update_hook->enable())diagnostic_trace::Write("V66.88-F12 CEntityAttachment::UpdateAttachment exact hook READY WHGame+0x4238B8; rung2 entity+character injection/readback/restore; visibility UNVERIFIED");else{g_entity_attachment_update_hook.reset();g_entity_attachment_update_original=nullptr;diagnostic_trace::Write("V66.88-F12 CEntityAttachment hook enable failed; rung2 proof FAIL-CLOSED");}}
        else diagnostic_trace::Write("V66.88-F12 CEntityAttachment hook creation failed; rung2 proof FAIL-CLOSED");
    } else diagnostic_trace::Write("V66.88-F12 CEntityAttachment hook refused: exact +0x4238B8 prologue mismatch");
    // V65.05: exact-build native IKTarget runtime observers. Unlike the two core
    // VRIK hooks above, each of these is optional/fail-closed: failure to attach a
    // diagnostic hook must never disable the established player-pose diagnostics.
    auto tryContext=[&]() -> bool {DetourModKit::hook::InlineRequest req{"kcd2vr_v6505_iktarget_context_update",DetourModKit::hook::Target{DetourModKit::Address{base+kIkTargetContextUpdateRva}},DetourModKit::hook::Options{}};auto r=DetourModKit::hook::inline_at(std::move(req),&IkTargetContextUpdateDetour);if(!r)return false;g_iktarget_context_hook=std::move(*r);g_iktarget_context_original=g_iktarget_context_hook->original<IkTargetContextUpdateFn>();auto e=g_iktarget_context_hook->enable();return static_cast<bool>(e);};
    auto tryBridge=[&]() -> bool {DetourModKit::hook::InlineRequest req{"kcd2vr_v6505_iktarget_bridge",DetourModKit::hook::Target{DetourModKit::Address{base+kIkTargetBridgeRva}},DetourModKit::hook::Options{}};auto r=DetourModKit::hook::inline_at(std::move(req),&IkTargetBridgeDetour);if(!r)return false;g_iktarget_bridge_hook=std::move(*r);g_iktarget_bridge_original=g_iktarget_bridge_hook->original<IkTargetBridgeFn>();auto e=g_iktarget_bridge_hook->enable();return static_cast<bool>(e);};
    auto tryPose=[&]() -> bool {DetourModKit::hook::InlineRequest req{"kcd2vr_v6505_iktarget_pose_writer",DetourModKit::hook::Target{DetourModKit::Address{base+kIkTargetPoseWriterRva}},DetourModKit::hook::Options{}};auto r=DetourModKit::hook::inline_at(std::move(req),&IkTargetPoseWriterDetour);if(!r)return false;g_iktarget_pose_writer_hook=std::move(*r);g_iktarget_pose_writer_original=g_iktarget_pose_writer_hook->original<IkTargetPoseWriterFn>();auto e=g_iktarget_pose_writer_hook->enable();return static_cast<bool>(e);};
    auto tryBlend=[&]() -> bool {DetourModKit::hook::InlineRequest req{"kcd2vr_v6505_iktarget_blend_writer",DetourModKit::hook::Target{DetourModKit::Address{base+kIkTargetBlendWriterRva}},DetourModKit::hook::Options{}};auto r=DetourModKit::hook::inline_at(std::move(req),&IkTargetBlendWriterDetour);if(!r)return false;g_iktarget_blend_writer_hook=std::move(*r);g_iktarget_blend_writer_original=g_iktarget_blend_writer_hook->original<IkTargetBlendWriterFn>();auto e=g_iktarget_blend_writer_hook->enable();return static_cast<bool>(e);};
    const bool c=tryContext(),b=tryBridge(),p=tryPose(),w=tryBlend();
    char im[420]{};std::snprintf(im,sizeof(im),"V65.05-IKRUNTIME hooks context=%d bridge=%d poseWriter=%d blendWriter=%d exactBuild=1 READ-ONLY",c?1:0,b?1:0,p?1:0,w?1:0);diagnostic_trace::Write(im);
    g_reload_pose_ready_xr34.store(g_skinning_core_original&&g_skinning_core_hook&&g_skinning_core_hook->is_enabled()&&
        g_entity_attachment_update_original&&g_entity_attachment_update_hook&&g_entity_attachment_update_hook->is_enabled()&&
        g_readReloadPassengerXR34&&g_currentReloadPassengerXR34&&g_prepareReloadPoseXR34,std::memory_order_release);
    // Uninstall performs the cold cleanup at Install entry. Permit preparation
    // on the next native frame only after the complete VRIK installation won.
    kcd2vr::instant_pickup_d72::stopping.store(false,std::memory_order_release);
    SetFinalSkinTestMode(0);return true;
}
void Uninstall() noexcept {
    if(!ShutdownInstantPickupD72()){
        diagnostic_trace::Write("D72 INSTANT_PICKUP shutdown deferred until native frame releases world item");return;
    }
    g_reload_pose_ready_xr34.store(false,std::memory_order_release);
    kcd2vr::crossbow_native_xr34::WithdrawRegistration();
    DoorMotionNativeShutdown66();
    g_door_live_serial66.store(0,std::memory_order_release);
    kcd2vr::door_signal19c4r4::SetCurveHandler66(nullptr);
    UpdateSmallPickupReaderR24(false);
    ShutdownStowedRenderD63();
    ShutdownHenryBounds63();
    // Request only: never call native attachment setters from teardown/loader lock.
    // A still-running native frame may acknowledge; if callbacks already stopped,
    // exact identity/expiry cleanup is not claimed to have executed.
    gore32n::RequestRestoreVisuals59();
    // Stop new script claims before removing the accepted-action producer.
    // A pending Lua cleanup keeps its getter/trampoline process-owned.
    (void)StopNativeDoor19c4();
    (void)kcd2vr::door_signal19c4r4::Shutdown();
    ShutdownBridges168();
    EndLifecycle165("SHUTDOWN");ShutdownLifecycle165();ShutdownReloadNative36();
    CancelComparisonLease114(); // shutdown must not auto-rearm the previous F12 mode
    kcd2vr::haptics101::select(false);cc::select(false,std::uint64_t(GetTickCount64()));
    // Shutdown is not a native gameplay phase. OFF cancellation is processed by
    // the existing frame pump during normal play; no foreign-thread game calls.
    g_native_owner104.publish(NativeOwner104{});
    ShutdownMaterial116();
    ShutdownInteractionBody122();
    ShutdownAttackTiming178r22c2();
    ShutdownNativeInput117();
    g_rotation_contract_d40=0;g_motion_contract112=0;
    if(g_rotation_hook_d40){(void)g_rotation_hook_d40->disable();g_rotation_hook_d40.reset();}
    g_rotation_original_d40=nullptr;
    if(g_position_hook36){(void)g_position_hook36->disable();g_position_hook36.reset();}
    g_position_original36=nullptr;
    if(g_alignment_hook112){(void)g_alignment_hook112->disable();g_alignment_hook112.reset();}
    g_alignment_original112=nullptr;
    if(g_native_damage_hook104){(void)g_native_damage_hook104->disable();g_native_damage_hook104.reset();}
    g_native_damage_original104=nullptr;g_native_contract104=0;g_native_request104=nullptr;g_native_zone104=nullptr;
    if(g_combat_input_hook101){(void)g_combat_input_hook101->disable();g_combat_input_hook101.reset();}
    g_combat_hook_base101=0;g_combat_contract101=0;g_combat_bypass101=nullptr;g_combat_input_original101=nullptr;
    ShutdownPhysicalWeapons6698();
    if(g_interaction_hook6689){(void)g_interaction_hook6689->disable();g_interaction_hook6689.reset();}g_interaction_original6689=nullptr;
    if(g_pickup_hook178r13){(void)g_pickup_hook178r13->disable();g_pickup_hook178r13.reset();}g_pickup_original178r13=nullptr;
    if(g_canpickup_hook178r13){(void)g_canpickup_hook178r13->disable();g_canpickup_hook178r13.reset();}g_canpickup_original178r13=nullptr;
    animation_filter::SetInteractionAnimationTarget(nullptr,0,0);g_committed_pose_valid6689.store(false,std::memory_order_release);
StopDrawNativeD77();StopInitialDrawCallbackH36r3();SetHolsterCombatDiagMode(0);SetMiscInteractionDiagMode(0);SetWeaponUpstreamDiagMode(0);SetWeaponControlMode(0);animation_filter::SetStartDiagnosticsLevel(0);if(g_entity_attachment_update_hook){(void)g_entity_attachment_update_hook->disable();g_entity_attachment_update_hook.reset();}g_entity_attachment_update_original=nullptr;g_player_identity_observe.store(false,std::memory_order_release);g_last_player_entity.store(nullptr,std::memory_order_release);ResetPlayerIdentity37();g_f11_theory_phase.store(0,std::memory_order_release);g_joint_recording.store(false,std::memory_order_release);SetCombatCensusMode(0);SetFinalSkinTestMode(0);if(g_attachment_helper_transform_hook){(void)g_attachment_helper_transform_hook->disable();g_attachment_helper_transform_hook.reset();}g_attachment_helper_transform_original=nullptr;if(g_wuid_resolver_hook){(void)g_wuid_resolver_hook->disable();g_wuid_resolver_hook.reset();}g_wuid_resolver_original=nullptr;if(g_attach_event_dispatch_hook){(void)g_attach_event_dispatch_hook->disable();g_attach_event_dispatch_hook.reset();}g_attach_event_dispatch_original=nullptr;if(g_item_attach_operation_hook){(void)g_item_attach_operation_hook->disable();g_item_attach_operation_hook.reset();}g_item_attach_operation_original=nullptr;if(g_iktarget_blend_writer_hook){(void)g_iktarget_blend_writer_hook->disable();g_iktarget_blend_writer_hook.reset();}g_iktarget_blend_writer_original=nullptr;if(g_iktarget_pose_writer_hook){(void)g_iktarget_pose_writer_hook->disable();g_iktarget_pose_writer_hook.reset();}g_iktarget_pose_writer_original=nullptr;if(g_iktarget_bridge_hook){(void)g_iktarget_bridge_hook->disable();g_iktarget_bridge_hook.reset();}g_iktarget_bridge_original=nullptr;if(g_iktarget_context_hook){(void)g_iktarget_context_hook->disable();g_iktarget_context_hook.reset();}g_iktarget_context_original=nullptr;if(g_skinning_core_hook){(void)g_skinning_core_hook->disable();g_skinning_core_hook.reset();}g_skinning_core_original=nullptr;if(g_setik_hook){(void)g_setik_hook->disable();g_setik_hook.reset();}g_setik_original=nullptr;g_last_player_pose.store(nullptr,std::memory_order_release);g_henry_char_attachment_manager.store(nullptr,std::memory_order_release);g_henry_skinning_thread.store(0,std::memory_order_release);g_henry_skinning_tick.store(0,std::memory_order_release);}
void PublishGameStateContext(const GameStateContext& state) noexcept {
    // C57R1: detect UI/context ABA during a solve without a frame counter or scan.
    // Hidden HUD is intentionally absent: it is not an ownership transition.
    const std::uint64_t policy57=(state.dialogue?1ull:0ull)|(state.cutscene?2ull:0ull)|
        (state.menu?4ull:0ull)|(state.native_reading?8ull:0ull)|
        (state.tool_pose_ui_blocked?16ull:0ull)|(state.workstation_controls_only?32ull:0ull)|
        (state.agency_known?64ull:0ull)|((state.agency_known&&state.scripted)?128ull:0ull);
    const bool policyChanged57=g_pose_policy_flags57.load(std::memory_order_acquire)!=policy57;
    if(policyChanged57){
        g_pose_policy_generation57.fetch_add(1,std::memory_order_acq_rel); // odd: in progress
        // Atomic cancellation also catches menu/cutscene OFF/ON between physics
        // callbacks; the old squeeze cannot be re-labelled as a new request.
        kcd2vr::physics::grab_producer60::Cancel(0);kcd2vr::physics::grab_producer60::Cancel(1);
    }
#include "vrik/workstation_pose_context19c4r2_publish.inl"
    g_ctx_dialogue.store(state.dialogue,std::memory_order_release);
    g_ctx_cutscene.store(state.cutscene,std::memory_order_release);
    g_ctx_menu.store(state.menu,std::memory_order_release);
    g_ctx_reading.store(state.native_reading,std::memory_order_release);
    g_ctx_agency_known.store(state.agency_known,std::memory_order_release);
    g_ctx_agency_count.store(state.agency_known?state.agency_count:-1,std::memory_order_release);
    g_ctx_scripted.store(state.agency_known&&state.scripted,std::memory_order_release);
    if(policyChanged57){g_pose_policy_flags57.store(policy57,std::memory_order_release);
        g_pose_policy_generation57.fetch_add(1,std::memory_order_release);} // even: published

    // Transition-only correlation while F11 is active. The agency field explicitly
    // reports unresolved instead of fabricating a native ActionMapManager ABI.
    if(g_f11_theory_phase.load(std::memory_order_acquire)>0){
        const std::uint64_t packed=(state.dialogue?1ull:0ull)|(state.cutscene?2ull:0ull)|
            (state.menu?4ull:0ull)|(state.native_reading?8ull:0ull)|
            (state.agency_known?16ull:0ull)|((state.agency_known&&state.scripted)?32ull:0ull)|
            (static_cast<std::uint64_t>(state.agency_known?std::clamp(state.agency_count,0,255):255)<<8);
        auto prev=g_ctx_packed_last_logged.load(std::memory_order_relaxed);
        if(prev!=packed && g_ctx_packed_last_logged.compare_exchange_strong(prev,packed,std::memory_order_acq_rel)){
            char m[384]{};
            std::snprintf(m,sizeof(m),
                "V64.19C-F11-STATE dialogue=%d cutscene=%d menu=%d nativeReading=%d agencyKnown=%d agencyCount=%d scripted=%d source=native-proven agencyABI=%s",
                state.dialogue?1:0,state.cutscene?1:0,state.menu?1:0,state.native_reading?1:0,
                state.agency_known?1:0,state.agency_known?state.agency_count:-1,(state.agency_known&&state.scripted)?1:0,
                state.agency_known?"PROVEN-CCRYACTION":"UNAVAILABLE-FAIL-CLOSED");
            diagnostic_trace::Write(m);
        }
    }
}

// C38 F5 rung 1 (Miki 17 Oct: hands vanish when both are stretched forward / over an edge / in dialogue; alchemy head
// "finicky"). Edge-only: one line when a condition starts and one when it ends. Reads the last committed VRIK frame copied by
// BeginCoherent6689 plus two atomics; no native call, no pose or camera change. Mod main thread only (tracking publisher).
#include "subsystems/interactions/reach_view_diagnostics.inl" // M2_OWNED_SOURCE
#include "subsystems/vrik/tracking_producer.inl" // M2_OWNED_SOURCE
#include "subsystems/vrik/calibration_storage.inl" // M2_OWNED_SOURCE
void SetBowStringGlue178r9(bool enabled) noexcept {g_bow_string_glue178r9.store(enabled,std::memory_order_relaxed);}
#include "subsystems/dialogues/body_control.inl" // M2_OWNED_SOURCE
// C43: accepted ordinary held props are a visual body owner, like workstations.
// Native Picking can suspend gameplay while the exact native attachment stays
// in the tracked hand. Dropping all visual authority there retracts both arms.
// Copied lease/identity only: no native lookup, no gameplay/input permission.
#include "subsystems/interactions/held_visual_policy.inl" // M2_OWNED_SOURCE
// V66.178R9 two-hand support socket controls
void SetStrikeReferenceJ59c9(float j) noexcept {g_strike_reference_j59c9.store(std::isfinite(j)?std::clamp(j,5.f,500.f):75.f,std::memory_order_relaxed);} // R59.16C9C
#include "subsystems/vrik/body_settings.inl" // M2_OWNED_SOURCE
#include "subsystems/physics/support_socket_controls.inl" // M2_OWNED_SOURCE
#include "subsystems/interactions/native_hand_use_control.inl" // M2_OWNED_SOURCE
#include "subsystems/vrik/calibration_controls.inl" // M2_OWNED_SOURCE

namespace {
// Retired unused implementation archived: research_archive/retired_pose_implementations

}
static diagnostics::lab::PoseLease g_lab_pose_lease6690;
static thread_local bool g_lab_internal_pose6690=false;
static int g_comparison_weapon_base114=-1;
static std::uint64_t g_comparison_weapon_request114=0;
static void CancelComparisonLease114() noexcept {g_comparison_weapon_base114=-1;g_comparison_weapon_request114=0;}
static void SetDiagnosticLab6690(diagnostics::lab::Key key,int mode) noexcept {
    mode=std::clamp(mode,0,3);
    if(diagnostics::lab::Get(key)==unsigned(mode))return;
    g_contact_control_epoch6695.fetch_add(1,std::memory_order_acq_rel);
    g_committed_pose_valid6689.store(false,std::memory_order_release);
    diagnostics::lab::Set(key,unsigned(mode));
    if(key==diagnostics::lab::Key::F5&&!mode)animation_filter::SetInteractionAnimationTarget(nullptr,0,0);
    // No saved tuple, automatic pose restoration or diagnostic lease mutation.
}

unsigned SetCombatObservation168r1(bool on) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!on){cc::captureOnlyUntil168r1=0;g_contact_until114=0;diagnostics::trace164::until=0;return 0;}
    if(!retail_build112::Verify(WhBase()))return 0;
    unsigned mask=1;
    if(g_weapon_control_mode.load()==7)StartPredictionCapture120();
    if(ArmCombatInput101(false))mask|=2;
    if(ArmNativeSweep113())mask|=4;
    if(ArmNativeContact114())mask|=8;
    if(ArmMaterial116())mask|=16;
    if(animation_filter::Ready()||animation_filter::Install())mask|=32;
    cc::session.fetch_add(1);
    cc::captureOnlyUntil168r1.store(now+diagnostics::capture168::Duration,std::memory_order_release);
    // Provenance collectors only. Do not arm F12, change hand ownership or renew physics grace.
    g_contact_until114=now+diagnostics::capture168::Duration;
    diagnostics::trace164::until.store(now+diagnostics::capture168::Duration,std::memory_order_release);
    return mask;
}
void SetHolsterCombatDiagMode(int mode) noexcept {
    // Compatibility adapter for explicit gameplay policy; public F3 is recorder-only.
    const int previous=HolsterCombatDiagMode();
    if(mode>=2&&!retail_build112::Verify(WhBase()))
        diagnostic_trace::Write("V66.112 F3 full SHA verification FAILED: combat visual suppression and physical guard remain native");
    if(mode==2||mode==3)diagnostic_trace::Write(NativeInputArm117()?
        "V66.117 INPUT contracts=READY cut=HEAVY thrust=LIGHT pommel=CONTEXTUAL_SPECIAL guard=POSTURE_ENTRY timing=NATIVE":
        "V66.117 INPUT contracts=FAILED_CLOSED typed melee disabled");
    if(mode==3){
        const bool armed=retail_build112::Ready(WhBase())&&NativeMotionArm112();
        diagnostic_trace::Write(armed?
            "V66.112-F3=3 contracts=READY alignment=QUALIFIED_HENRY_QUERY trajectory=LIVE_BONE_REQUIRES_3_CORRELATIONS directDamage=NONE enemyContactParry=UNRESOLVED":
            "V66.112-F3=3 contracts=FAILED_CLOSED alignment=NATIVE trajectory=NATIVE");
        diagnostic_trace::Write(ArmNativeContact114()?"V66.114 CONTACT_OBSERVER armed Henry-only participation no NPC enumeration no native mutation":"V66.114 CONTACT_OBSERVER unavailable; original native callback retained");
    }
    SetDiagnosticLab6690(diagnostics::lab::Key::F3,mode);
    const int current=HolsterCombatDiagMode();
    if(current>0){
        const bool semanticReady=NativeArm104();
        const bool pumpReady=semanticReady&&native_stereo_outer_lab::EnsurePhysicalFramePump6698();
        char status[256]{};std::snprintf(status,sizeof(status),"V66.104 F3 native contract=%d statePump=%d damageObserver=%d noAttacksFromObserver=1",semanticReady?1:0,pumpReady?1:0,g_native_damage_original104?1:0);diagnostic_trace::Write(status);
    }
    if(current>=2&&previous<2){
        if(!animation_filter::Ready())animation_filter::Install();
        animation_filter::SetMode(animation_filter::SuppressionMode::AllSelectedCombat);
        diagnostic_trace::Write("V66.114-F3 ownership: rung2=qualified NAME+ID enqueue suppression; rung3=native clip events plus final VR pose; SHA and live gates required; alignment/trajectory only rung3");
    }else if(previous>=2&&current<2){
        animation_filter::SetMode(animation_filter::SuppressionMode::Off);
        diagnostic_trace::Write("V66.111-F3 VR_COMBAT_OWNERSHIP OFF native Henry combat visuals restored");
    }
    if(current==1){
        const bool inputReady=retail_build112::Verify(WhBase())&&ArmCombatInput101(false);
        const bool sweepReady=ArmNativeSweep113();
        (void)ArmNativeContact114();
        if(!animation_filter::Ready())animation_filter::Install();
        char status[320]{};std::snprintf(status,sizeof(status),
            "V66.113 REFERENCE_ARM source=F3 input=%d sweep=%d durationMs=180000 F12Required=0 mutation=NONE",inputReady?1:0,sweepReady?1:0);
        diagnostic_trace::Write(status);
    }
    cc::selectSources(cc::sources(current,MiscInteractionDiagMode(),g_census_mode.load()),GetTickCount64(),current==1);
    if(current==1||current==3)diagnostic_trace::Write(ArmMaterial116()?
        "V66.116 MATERIAL_OBSERVER armed Henry-qualified combat dispatch->surface lookup->effect execution READ_ONLY":
        "V66.116 MATERIAL_OBSERVER unavailable; original effects retained");

}
void CycleHolsterCombatDiagMode() noexcept {SetHolsterCombatDiagMode((HolsterCombatDiagMode()+1)%4);}
int HolsterCombatDiagMode() noexcept {return int(diagnostics::lab::Get(diagnostics::lab::Key::F3));}
#include "subsystems/interactions/diagnostic_control.inl" // M2_OWNED_SOURCE
void SetCombatCensusMode(int mode) noexcept {
    if(g_finger_capture66100.exchange(false,std::memory_order_acq_rel)){
        g_capture_session66100.fetch_add(1,std::memory_order_acq_rel);g_finger_capture_status66100=0;
    }
    g_contact_control_epoch6695.fetch_add(1,std::memory_order_acq_rel);
    if(!g_lab_internal_pose6690)g_lab_pose_lease6690.ExternalPoseChoice(std::clamp(mode,0,5));
    g_committed_pose_valid6689.store(false,std::memory_order_release);
    RestorePendingIfPossible(nullptr);
    RestoreF10SubtreeHoldIfPossible(nullptr);
    mode=std::clamp(mode,0,5);
    if(mode==5){
        if((!retail_build112::Ready(WhBase())&&!retail_build112::Verify(WhBase()))||!NativeArm104()||!ArmCombatInput101(true)){diagnostic_trace::Write("V66.104-FISTS ARM_FAILED F10=OFF native guards retained");mode=0;}
        else {SetWeaponControlMode(0);
            if(!ArmAttackTiming178r22c2())diagnostic_trace::Write("H11 BOXING timing=UNAVAILABLE retailPins=REFUSED nativeInput=RETAINED");
        }
    }
    g_fist_epoch101.fetch_add(1,std::memory_order_acq_rel);
    cc::selectSources(cc::sources(HolsterCombatDiagMode(),MiscInteractionDiagMode(),mode),GetTickCount64(),mode==1);
    if(mode==1){SetWeaponControlMode(0);const bool hook=ArmCombatInput101(false);if(!animation_filter::Ready())animation_filter::Install();
        diagnostic_trace::Write(hook?"V66.101R1-KICK_CAPTURE ARMED durationMs=60000 nativeInputs=READ_ONLY animation=READ_ONLY R3=EXISTING_BINDING":"V66.101R1-KICK_CAPTURE INPUT_HOOK_UNAVAILABLE animationOnly=1 noNativeKickClaim=1");}
    g_hard_body_epoch.fetch_add(1,std::memory_order_acq_rel); // V65.77: next rung-5 frame captures a fresh idle/base pose
    if(mode>0) g_mode.store(0,std::memory_order_release); // mutually exclusive A/B lab
    g_census_mode.store(mode,std::memory_order_release);
    {const bool haptics=mode>=4||g_weapon_control_mode.load()>=4;
     if(kcd2vr::haptics101::enabled.load()!=haptics)kcd2vr::haptics101::select(haptics,GetTickCount64());}
    g_legacy_cal_state.store(0,std::memory_order_release); g_upper_head_baseline_state.store(0,std::memory_order_release);
    // V66.111 F3=2: calibrated VRIK remains the final Henry combat-pose owner while
    // the native combat action/state continues underneath. F3=0/1 are untouched.
    animation_filter::SetMode(diagnostics::lab::Get(diagnostics::lab::Key::F3)>=2
        ? animation_filter::SuppressionMode::AllSelectedCombat
        : animation_filter::SuppressionMode::Off);
    if(mode>=2 && !CalibrationReady()) { g_vrik_calibration_requested.store(true,std::memory_order_release); g_vrik_calibration_requests.fetch_add(1,std::memory_order_relaxed); }
    g_setik_calls.store(0,std::memory_order_relaxed);g_correlated_calls.store(0,std::memory_order_relaxed);g_prebuild_writes.store(0);g_rejects.store(0);g_rig_valid.store(false);
    g_named_joint_api_valid.store(false,std::memory_order_release);g_named_api_attempts.store(0);g_named_hand_r.store(-1);g_named_hand_l.store(-1);g_slt0_joint.store(-1);g_slt1_joint.store(-1);g_weapon_slot_r.store(-1);g_weapon_slot_l.store(-1);g_weapon_slot_parent_r.store(-2);g_weapon_slot_parent_l.store(-2);g_named_probe_owner.store(nullptr);
    const char* calStatus=(mode>=2&&mode<=5)?(CalibrationReady()?"READY":"REQUIRED-NOT-READY"):(mode==1?"READ-ONLY":"OFF");
    char m[768]{};std::snprintf(m,sizeof(m),
         "V66.94-VRIK F10 mode=%d %s calibration=%s hmdAnchoredFrame=1 canonicalArms=R403>405>408 L438>440>443 concreteItem=%p attachSide=%d itemWrites=NONE identity=%s animations=NATIVE collision=%s",
        mode,CombatCensusModeName(mode),calStatus,g_last_concrete_item_object.load(std::memory_order_acquire),g_last_item_attach_side.load(std::memory_order_acquire),
        g_last_player_pose.load(std::memory_order_acquire)?"PRIMED":"WAITING-FOR-HENRY-SETIK",mode>=4?"STATIC_CONTACT_OPT_IN":"OFF_IN_CONTROLLER_CORE");diagnostic_trace::Write(m);
}
void CycleCombatCensusMode() noexcept {
    if(FingerCaptureActive()){
        FingerPacket66100 candidate{};const auto now=static_cast<std::uint64_t>(GetTickCount64());
        const bool qualified=g_finger_candidate_ready66100.load(std::memory_order_acquire)&&g_finger_candidate66100.read(candidate)&&
            candidate.session==g_capture_session66100.load()&&candidate.tick&&now>=candidate.tick&&now-candidate.tick<=150&&fc100::valid(candidate.pair);
        const bool offered=qualified&&g_finger_offer66100.publish(candidate);
        // No native callback is required to leave capture, reach preview, or OFF.
        g_finger_capture66100=false;g_finger_candidate_ready66100=false;
        SetCombatCensusMode(2);g_finger_capture_status66100=offered?5:0;
        diagnostic_trace::Write(offered?"V66.100-F10 rung=2 preview=QUALIFIED_NATIVE_PAIR":"V66.100-F10 rung=2 preview=PREVIOUS_REFERENCES captureIncomplete=1 oldPairRetained=1");return;
    }
    const int current=g_census_mode.load(std::memory_order_acquire);
    // Finger capture is an explicit advanced command, never part of cycling.
    // F8 changes anthropometry only; accepted finger banks remain authoritative.
    SetCombatCensusMode((current+1)%6);
}
int CombatCensusMode() noexcept {return g_census_mode.load(std::memory_order_acquire);}
static kcd2vr::f11_tests167::Modes F11Modes167() noexcept {
    return {HolsterCombatDiagMode(),MiscInteractionDiagMode(),CombatCensusMode(),WeaponControlMode()};
}
int FocusedTestMode167() noexcept {
    const int mode=g_f11_test167.load(std::memory_order_acquire);
    return mode&&kcd2vr::f11_tests167::same(F11Modes167(),kcd2vr::f11_tests167::preset(mode))?mode:0;
}
static void ApplyF11Modes167(kcd2vr::f11_tests167::Modes mode) noexcept {
    // F3=3 selects F12=6 and F12 may select pose mode 3. Apply the saved pose
    // last so OFF can restore the exact preceding tuple, including pose OFF.
    SetMiscInteractionDiagMode(mode.interaction);SetHolsterCombatDiagMode(mode.combat);
    SetWeaponControlMode(0);SetWeaponControlMode(mode.weapon);SetCombatCensusMode(mode.pose);
}
void CancelFocusedTests167(bool restore) noexcept {
    RevokeMasterIntent168();
    const auto current=F11Modes167();const int old=g_f11_test167.exchange(0,std::memory_order_acq_rel);
    kcd2vr::f11_tests167::Modes base{};const bool owned=g_f11_lease167.end(current,base);
    if(!old)return;
    const auto sources=cc::sourceMask.load();
    if(sources&(cc::F11Hands167|cc::F11Stabs167))
        cc::selectSources(cc::sources(current.combat,current.interaction,current.pose),GetTickCount64());
    if(restore&&owned)ApplyF11Modes167(base);
    kcd2vr::haptics101::select(WeaponControlMode()>=4||CombatCensusMode()>=4,GetTickCount64());
    char m[240]{};std::snprintf(m,sizeof(m),"V66.167 F11 OFF previous=%d baselineRestored=%d reason=%s calibration=PRESERVED",
        old,restore&&owned,restore?"USER_OFF":"OTHER_MODE_SELECTED");diagnostic_trace::Write(m);
}

void SetWeaponUpstreamDiagMode(int mode) noexcept { g_weapon_upstream_diag_mode.store(std::clamp(mode,0,5),std::memory_order_release); }
void CycleWeaponUpstreamDiagMode() noexcept { SetWeaponUpstreamDiagMode((g_weapon_upstream_diag_mode.load(std::memory_order_acquire)+1)%6); }
int WeaponUpstreamDiagMode() noexcept { return g_weapon_upstream_diag_mode.load(std::memory_order_acquire); }
const char* WeaponUpstreamDiagModeName(int mode) noexcept { switch(mode){case 0:return "OFF";case 1:return "HENRY HAND BONES";case 2:return "+ ITEM_R/ITEM_L";case 3:return "+ CONCRETE C_ITEM/WUID";case 4:return "+ RESOLVED ENTITY WORLD/LOCAL";case 5:return "+ ATTACHMENT OWNER/MANAGER";default:return "UNKNOWN";} }



ProbeStatus GetStatus() noexcept {
    ProbeStatus s{};
    s.mode=FinalSkinTestMode();s.rig_valid=g_rig_valid.load();s.calibrated=g_cal_state.load(std::memory_order_acquire)==2;
    const auto strategy=CurrentWriteStrategy(s.mode,g_census_mode.load(std::memory_order_acquire));
    s.tracking_valid=g_head_valid.load()&&(!StrategyNeedsHands(strategy)||(g_right_valid.load()&&g_left_valid.load()));
    s.gameplay_allowed=g_gameplay_allowed.load();s.joint_count=g_cached_count.load();
    s.hand_r=g_cached_hand_r.load();s.hand_l=g_cached_hand_l.load();s.head=g_cached_head.load();
    s.named_joint_api_valid=g_named_joint_api_valid.load(std::memory_order_acquire);
    s.named_hand_r=g_named_hand_r.load();s.named_hand_l=g_named_hand_l.load();
    s.weapon_slot_r=g_weapon_slot_r.load();s.weapon_slot_l=g_weapon_slot_l.load();s.weapon_slot_r_parent=g_weapon_slot_parent_r.load();s.weapon_slot_l_parent=g_weapon_slot_parent_l.load();s.weapon_slot_r_resolved=s.weapon_slot_r>=0;s.weapon_slot_l_resolved=s.weapon_slot_l>=0;
    s.weapon_solver_recent=(static_cast<std::uint64_t>(GetTickCount64())-g_last_setik_tick.load(std::memory_order_relaxed))<=1500ull;
    s.solver_joint_c=g_solver_joint_c.load();s.solver_joint_e=g_solver_joint_e.load();
    s.correlated_calls=g_correlated_calls.load();s.setik_calls=g_setik_calls.load();s.rig_maps=g_rig_maps.load();
    s.prebuild_writes=g_prebuild_writes.load();s.setik_upstream_writes=g_setik_upstream_writes.load();s.setik_hold_writes=g_setik_hold_writes.load();
    s.pending_hold_active=PendingHoldActive();s.rejects=g_rejects.load();
    s.upper_length=g_b1_l1.load();s.fore_length=g_b1_l2.load();s.ref_upper_length=g_ref_l1.load();s.ref_fore_length=g_ref_l2.load();
    s.controller_delta=g_controller_delta_len.load();s.fail_stage=g_fail_stage.load();return s;
}
#include "subsystems/vrik/diagnostic_snapshot.inl" // M2_OWNED_SOURCE
void SetPlayerIdentityObservationEnabled(bool enabled) noexcept {
    g_player_identity_observe.store(enabled,std::memory_order_release);
    if(!enabled) g_last_player_entity.store(nullptr,std::memory_order_release);
}

void* LastPlayerEntity() noexcept {
    return g_last_player_entity.load(std::memory_order_acquire);
}
#include "subsystems/dialogues/native_head_service.inl" // M2_OWNED_SOURCE
bool NativeReloadHandRemoveEntryQualifiedT30(std::uintptr_t base) noexcept {
    return base==WhBase()&&retail_build112::Ready(base)&&
        g_reload_hand_entry_t30.qualifies(base,reinterpret_cast<std::uintptr_t>(&ReloadHandRemove36),CopyReloadHandEntryT30);
}
ThrowAuthorityT22 ThrowNativeAuthorityT22(int side,std::uintptr_t character,std::uint64_t body,
    std::uint64_t request,std::uint64_t tracking,std::uint64_t now) noexcept {
    if(side<0||side>1||!character||!body||!request||!tracking||!now)return ThrowAuthorityT22::Unknown;
    const auto currentBody=g_pose_epoch6689.load(std::memory_order_acquire);
    const auto currentRequest=g_weapon_request_epoch6694.load(std::memory_order_acquire);
    const auto currentTracking=tracking59c6::epoch.load(std::memory_order_acquire);
    const auto currentCharacter=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire));
    if(currentBody!=body||currentRequest!=request||currentTracking!=tracking||currentCharacter!=character)
        return ThrowAuthorityT22::Unknown;
    const auto denied=[]() noexcept {
        return g_weapon_control_mode.load(std::memory_order_acquire)<4||!g_gameplay_allowed.load(std::memory_order_acquire)||
            g_ctx_menu.load(std::memory_order_acquire)||g_ctx_dialogue.load(std::memory_order_acquire)||
            g_ctx_cutscene.load(std::memory_order_acquire)||g_ctx_reading.load(std::memory_order_acquire)||
            g_ctx_scripted.load(std::memory_order_acquire)||animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)||
            diagnostics::lab::NativeReference();
    };
    const bool deniedBefore=denied();
    const bool tracked=TrackingHeadUsable111(now)&&TrackingHandUsable111(side,now);
    const bool fault=g_pose_restore_fault6689.load(std::memory_order_acquire)||g_phys_fault6698.load(std::memory_order_acquire);
    if(currentBody!=g_pose_epoch6689.load(std::memory_order_acquire)||
       currentRequest!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||
       currentTracking!=tracking59c6::epoch.load(std::memory_order_acquire)||
       currentCharacter!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire)))
        return ThrowAuthorityT22::Unknown;
    if(deniedBefore||denied())return ThrowAuthorityT22::Denied;
    return tracked&&!fault?ThrowAuthorityT22::Allowed:ThrowAuthorityT22::Unknown;
}
ThrowAuthorityT22 ThrowHandAdmissionT22(int side,std::uintptr_t player,std::uintptr_t character,
    std::uint64_t body,std::uint64_t request) noexcept {
    if(g_fist_native_tick_active_t22||side<0||side>1||!player||!character||!body||!request)return ThrowAuthorityT22::Unknown;
    const auto current=[&]() noexcept {
        return body==g_pose_epoch6689.load(std::memory_order_acquire)&&
            request==g_weapon_request_epoch6694.load(std::memory_order_acquire)&&
            character==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire));
    };
    const auto reloadAdmission=[&]() noexcept {
        using A=kcd2vr::reload_throw_claim_t25::Admission;
        const auto a=kcd2vr::reload_throw_claim_t25::observeReloadDebt(side,player);
        return a==A::Allowed?ThrowAuthorityT22::Allowed:a==A::Denied?ThrowAuthorityT22::Denied:ThrowAuthorityT22::Unknown;
    };
    if(!current())return ThrowAuthorityT22::Unknown;
    const auto reloadBefore=reloadAdmission();if(reloadBefore!=ThrowAuthorityT22::Allowed)return reloadBefore;
    const auto finish=[&]() noexcept {return current()?reloadAdmission():ThrowAuthorityT22::Unknown;};
    std::unique_lock<std::recursive_mutex> lock(g_fist_native_mutex101,std::try_to_lock);
    if(!lock.owns_lock()||!current())return ThrowAuthorityT22::Unknown;
    const auto& native=g_fist_native101;
    const bool fault=g_fist_fault101.load(std::memory_order_acquire);
    if(!native.prepared&&!native.waitingGuardU16&&!fault)return finish();
    if(!native.player)return ThrowAuthorityT22::Unknown;
    // A positively different original player owns its own retirement. Never
    // abort/clear it or retag it to the current actor's body/request.
    if(reinterpret_cast<std::uintptr_t>(native.player)!=player)return finish();
    if(fault)return ThrowAuthorityT22::Unknown;
    if(native.prepared){
        if(native.side<0||native.side>1)return ThrowAuthorityT22::Unknown;
        if(native.side==side)return ThrowAuthorityT22::Denied;
    }
    if(native.waitingGuardU16){
        if(!native.guardPunchU16.valid||native.guardPunchU16.side<0||native.guardPunchU16.side>1)
            return ThrowAuthorityT22::Unknown;
        if(native.guardPunchU16.side==side)return ThrowAuthorityT22::Denied;
    }
    return finish();
}
// Read-only LEFT equipment retirement observation. New-admission only:
// the caller supplies its ORIGINAL owner; cleanup must never depend on it.
ThrowAuthorityT22 ThrowEquipmentAdmissionT25(int side,std::uintptr_t player,std::uintptr_t manager,
    std::uintptr_t character,std::uint64_t body,std::uint64_t request) noexcept {
    if(side<0||side>1||!player||!manager||!character||!body||!request)return ThrowAuthorityT22::Unknown;
    const auto current=[&]() noexcept {
        return manager==reinterpret_cast<std::uintptr_t>(g_henry_item_attach_manager.load(std::memory_order_acquire))&&
            character==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire))&&
            character==g_player_char37.load(std::memory_order_acquire)&&
            body==g_pose_epoch6689.load(std::memory_order_acquire)&&request==g_weapon_request_epoch6694.load(std::memory_order_acquire);
    };
    if(!current())return ThrowAuthorityT22::Unknown;
    // H36 currently owns LEFT Draw/Stow only; do not mask the opposite hand.
    if(side==0)return current()?ThrowAuthorityT22::Allowed:ThrowAuthorityT22::Unknown;
    std::unique_lock<std::recursive_mutex> lock(g_phys_native_mutex6698,std::try_to_lock);
    if(!lock.owns_lock()||!current())return ThrowAuthorityT22::Unknown;
    bool debt=false,unknown=g_left_equipment_overflow_h36;
    const auto observe=[&](const LeftEquipmentObligationH36& o) noexcept {
        if(!o.active)return;
        if(o.command.side!=1||!o.player||!o.manager||!o.character){unknown=true;return;}
        if(reinterpret_cast<std::uintptr_t>(o.player)==player&&reinterpret_cast<std::uintptr_t>(o.manager)==manager)debt=true;
        else unknown=true;
    };
    observe(g_left_equipment_obligation_h36);for(const auto& o:g_left_equipment_successors_h36)observe(o);
    const bool pending=g_native_held_equipment_pending_h36.load(std::memory_order_acquire);
    if(!current())return ThrowAuthorityT22::Unknown;
    if(debt)return ThrowAuthorityT22::Denied; // old body/request cannot retire original work
    if(unknown||pending)return ThrowAuthorityT22::Unknown;
    return ThrowAuthorityT22::Allowed;
}
bool ReadThrowModelGripT18(void* entity,float (&out)[7]) noexcept {
    ph::Pose grip{};if(!PhysVft6698(entity,0x4072C78)||!PhysHelper34(entity,"slt_0",grip,false))return false;
    out[0]=float(grip.q.x);out[1]=float(grip.q.y);out[2]=float(grip.q.z);out[3]=float(grip.q.w);
    out[4]=float(grip.p.x);out[5]=float(grip.p.y);out[6]=float(grip.p.z);return true;
}
void* WeaponControlEntity(int side) noexcept {
    return (side>=0&&side<2)?g_weapon_control_entity[side].load(std::memory_order_acquire):nullptr;
}
bool HandClaimBlocksT24(int side) noexcept {
    return HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),g_pose_epoch6689.load(),g_weapon_request_epoch6694.load());
}
#include "subsystems/interactions/hand_item_state.inl" // M2_OWNED_SOURCE
#include "subsystems/doors/native_door_service.inl" // M2_OWNED_SOURCE
#include "subsystems/vrmap/committed_pose_service.inl" // M2_OWNED_SOURCE
#include "subsystems/vrmap/paired_pose_snapshot_d78r1.inl" // M2_OWNED_SOURCE
#include "subsystems/interactions/hand_pose_service.inl" // M2_OWNED_SOURCE
// DS-F88 (Claude's helper note): the narrow grab reader - the committed world
// wrist + quaternion + epochs, WITHOUT the g_census_mode>=2 diagnostics gate.
// Public namespace scope: the anonymous-namespace helpers are visible here via
// the using-directive.
bool ReadGrabHandWorld60(int side,float palm[3],float quat[4],std::uint64_t& poseEpoch,std::uint64_t& trackingEpoch,std::uintptr_t& player,std::uint64_t& tick) noexcept {
    palm[0]=palm[1]=palm[2]=0;quat[0]=quat[1]=quat[2]=0;quat[3]=1;
    poseEpoch=trackingEpoch=0;player=0;tick=0;
    if(side<0||side>1)return false;
    CommittedFrame6695 p{};const auto now=std::uint64_t(GetTickCount64());
    if(!ReadCommittedPacket6695(p)||!p.valid[side]||!TrackingHandUsable111(side,now)||
       !p.stamp.tick||now<p.stamp.tick||now-p.stamp.tick>80)return false;
    const auto wrist=HandWorld6694(p.allowed[side],p.world);
    const auto pos=FromT(wrist);
    if(!std::isfinite(pos.x)||!std::isfinite(pos.y)||!std::isfinite(pos.z))return false;
    palm[0]=pos.x;palm[1]=pos.y;palm[2]=pos.z;
    const auto q=QFromQuatT(wrist);
    quat[0]=q.x;quat[1]=q.y;quat[2]=q.z;quat[3]=q.w;
    poseEpoch=p.stamp.epoch;trackingEpoch=p.doorTrackingEpoch66;
    player=p.doorPlayer38;tick=p.stamp.tick;
    return true;
}
#include "vrik/phys_bodies59c10r8.inl" // R59.16C10R8 hand/blade physics bodies
bool ReadSmithContactHammer68(kcd2vr::subsystems::smithing::contact68::HammerFrame& out,std::uint64_t now) noexcept {
    out={};kcd2vr::subsystems::smithing::contact68::HammerFrame h{};workstation_sample46::Frame frame{};
    if(!g_smith_placed68.read(h)||!h.placed||!h.tick||now<h.tick||now-h.tick>80||
       h.poseEpoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       h.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire)||
       !WorkstationFrame46(frame)||!frame.allowed||!workstation_sample46::fresh(frame,now)||
       !frame.hand[0].valid||frame.poseEpoch!=h.poseEpoch||frame.sourceEpoch!=h.sourceEpoch||
       h.sourceEpoch!=tracking59c6::epoch.load(std::memory_order_acquire)||
       !TrackingHeadUsable111(now)||!TrackingHandUsable111(0,now)||
       h.binding.character!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire)))return false;
    if(h.poseEpoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       h.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire)||
       h.sourceEpoch!=tracking59c6::epoch.load(std::memory_order_acquire)||
       h.binding.character!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire)))return false;
    out=h;return true;
}
std::uintptr_t FindSmithContactEntity68(std::uint32_t id) noexcept {
    if(!id||!retail_build112::Ready(WhBase()))return 0;
    void* entity=EntityById6694(id);
    return entity&&PhysEntityIdentity135(entity,id)?reinterpret_cast<std::uintptr_t>(entity):0;
}
static bool ReadWorkstationPalmImplH21(int side,float& x,float& y,float& z,std::uint64_t& tick,
    std::uint64_t& poseEpoch,std::uint64_t& sourceEpoch,std::uintptr_t& playerEntity,float* direction) noexcept {
    if(direction)direction[0]=direction[1]=direction[2]=0;tick=poseEpoch=sourceEpoch=0;playerEntity=0;const auto now=std::uint64_t(GetTickCount64());
    if(side<0||side>1||!kcd2vr::workstation178r20c1::ownsActivityWrist45(now)||
       WorkstationToolReason19c4r2(now)||diagnostics::lab::NativeReference()||!CalibrationReady())return false;
    CommittedFrame6695 p{};
    if(!ReadCommittedPacketFor6695(p,true)||!p.valid[side]||!p.palmValid59c9[side]||
       !TrackingHeadUsable111(now)||!TrackingHandUsable111(side,now)||
       !p.stamp.tick||now<p.stamp.tick||now-p.stamp.tick>80||p.stamp.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       !p.doorTrackingEpoch66||!p.doorPlayer38||p.doorPlayer38!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire)))return false;
    const Q4 q{p.world[0],p.world[1],p.world[2],p.world[3]};
    V3 palm=p.palm59c9[side],palmDirection=p.palmDirection59c9[side];
    // D72: selecting a native ingredient/source is controller intention.
    // Contact may displace the displayed wrist but cannot move the pointing
    // ray to another item. Reuse the same completed packet's calibrated raw
    // wrist and fixed anatomical palm offset; no newer/unpaired tracking read.
    const bool trackedIntent=kcd2vr::minigame_hands45::freeTrackedMode(kcd2vr::minigame_hands45::currentMode());
    if(trackedIntent){
        if(!p.propRequestedValid5916c1[side]||!ValidPose6689(p.propRequested5916c1[side]))return false;
        const auto achieved=QFromQuatT(p.allowed[side]);
        const auto requested=QFromQuatT(p.propRequested5916c1[side]);
        const auto local=QRotateV(QConj(achieved),Sub(palm,FromT(p.allowed[side])));
        palm=Add(FromT(p.propRequested5916c1[side]),QRotateV(requested,local));
        palmDirection=QRotateV(requested,QRotateV(QConj(achieved),palmDirection));
    }
    const V3 origin=Add(QRotateV(q,Mul(palm,p.world[7])),{p.world[4],p.world[5],p.world[6]});
    if(!pc::finite(PV(origin)))return false;
    if(direction){const V3 aim=QRotateV(q,palmDirection);
        const double norm=std::sqrt(double(aim.x)*aim.x+double(aim.y)*aim.y+double(aim.z)*aim.z);
        if(!pc::finite(PV(aim))||!std::isfinite(norm)||norm<.5||norm>1.5)return false;
        direction[0]=float(aim.x/norm);direction[1]=float(aim.y/norm);direction[2]=float(aim.z/norm);}

    x=origin.x;y=origin.y;z=origin.z;tick=p.stamp.tick;poseEpoch=p.stamp.epoch;sourceEpoch=p.doorTrackingEpoch66;playerEntity=p.doorPlayer38;return true;
}
bool ReadWorkstationPalmAimH21(int side,float& x,float& y,float& z,std::uint64_t& tick,
    std::uint64_t& poseEpoch,std::uint64_t& sourceEpoch,std::uintptr_t& playerEntity,float (&direction)[3]) noexcept {
    return ReadWorkstationPalmImplH21(side,x,y,z,tick,poseEpoch,sourceEpoch,playerEntity,direction);
}

bool ReadWorkstationPalm67(int side,float& x,float& y,float& z,std::uint64_t& tick,
    std::uint64_t& poseEpoch,std::uint64_t& sourceEpoch,std::uintptr_t& playerEntity) noexcept {
    return ReadWorkstationPalmImplH21(side,x,y,z,tick,poseEpoch,sourceEpoch,playerEntity,nullptr);
}

int NativeHeadAimEquipment120R3() noexcept {
    PhysEquipment6698 e{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_phys_equipment6698.read(e)||!e.valid||!e.tick||now<e.tick||now-e.tick>80||
       e.request!=g_weapon_request_epoch6694.load()||e.epoch!=g_pose_epoch6689.load()||
       !e.manager||e.manager!=g_henry_item_attach_manager.load())return -1;
    bool unknown=false;
    for(unsigned side=0;side<2;++side){
        if(!e.handKnown[side]){unknown=true;continue;}
        if(!e.handKey[side])continue;
        const int i=e.heldIndex[side];
        if(i<0||i>=e.count){unknown=true;continue;}
        const auto& p=e.items[i].profile;
        if(p.classId<0){unknown=true;continue;}
        if(!p.melee()&&p.classId!=0)return 1; // conservative non-melee/ranged exemption
    }
    return unknown?-1:0;
}
static int FistContactFrameAdmissionH31(const void* actor,const float* point,int side,const FistContactFrameH11* original,bool replayR10=false,bool nativeShapeH31=false) noexcept {
    namespace fh=kcd2vr::fist_action_h12;auto& receipt=fh::receiptH31;receipt.side=side;
    const auto fail=[&](fh::FailureH31 why){receipt.failure=why;return 0;};
    if(diagnostics::lab::NativeReference())return -1;
    if(!FistSelected101())return FistContactRequestedH31()?fail(fh::FailureH31::GameplayOwner):-1;
    if(!actor||!point||side<0||side>1)return fail(fh::FailureH31::InvalidInput);
    if(!g_gameplay_allowed.load()||animation_filter::nativeBodyOwned112.load())return fail(fh::FailureH31::GameplayOwner);
    const auto now=std::uint64_t(GetTickCount64());
    FistContactFrameH11 f{};FistOwner101 hands{};NativeOwner104 owner{};
    if(original)f=*original;
    else if(!g_fist_contact_h11.read(f))return fail(fh::FailureH31::MissingPalm);
    receipt.palmSample=f.sample;receipt.palmAge=now>=f.tick?now-f.tick:UINT64_MAX;
    if(!f.valid[side])return fail(fh::FailureH31::PalmInvalid);
    // A new preview claim can arrive after this original palm/sweep was published.
    if(HandClaimBlockedT24(side,f.character,f.epoch,g_weapon_request_epoch6694.load()))return fail(fh::FailureH31::CompetingHandClaimT24);
    if(f.epoch!=g_pose_epoch6689.load())return fail(fh::FailureH31::PalmEpoch);
    if(f.request!=g_fist_epoch101.load())return fail(fh::FailureH31::PalmRequest);
    if(now<f.tick||now-f.tick>kcd2vr::fist_reach_r10::palmAgeLimitMs(replayR10&&original))return fail(fh::FailureH31::PalmAge); // R10/H2H: a replayed peak is up to 150 ms old
    if(!FistReadOwner101(hands)||!hands.empty||hands.player!=f.player)return fail(fh::FailureH31::EmptyHandOwner);
    if(!NativeResolve104(f.player,owner)||owner.actor!=actor||owner.epoch!=f.epoch)return fail(fh::FailureH31::NativeActor);
    CommittedFrame6695 committedH11{};
    if(!f.completedH31||!f.sample||!f.character)return fail(fh::FailureH31::UncommittedPalm);
    if(!g_committed_frame6695.read(committedH11))return fail(fh::FailureH31::CommittedOwner);
    receipt.committedSample=committedH11.stamp.sample;
    // The palm packet was made from a WHOLE completed pose. A newer pose can be
    // committed before its palm publication; that must not erase this unchanged
    // <=60 ms physical segment. Ownership/epoch and a usable current hand must
    // still agree. The original segment/sample is never retimed or reprojected.
    if(committedH11.stamp.epoch!=f.epoch||committedH11.stamp.sample<f.sample||committedH11.stamp.actor!=f.character||
       !committedH11.valid[side])return fail(fh::FailureH31::CommittedOwner);
    TrackingSnapshot live{};
    if(!ReadTrackingSnapshot(live)||!live.allowed||!live.head_valid||!live.right_valid||!live.left_valid||
       live.head_held||live.right_held||live.left_held||now<live.tick||now-live.tick>60)return fail(fh::FailureH31::Tracking);
    receipt.grip=side?live.left_grip:live.right_grip;receipt.trigger=side?live.left_trigger:live.right_trigger;
    if(!(receipt.grip>.60f&&receipt.trigger>.60f))return fail(fh::FailureH31::Squeeze);
    const pm::Vec3 p{point[0],point[1],point[2]};if(!pm::finite(p)||!pm::finite(f.palm[side])||!pm::finite(f.previous[side]))return fail(fh::FailureH31::NonFinite);
    const auto delta=f.palm[side]-f.previous[side];const double length2=pm::dot(delta,delta);
    const double along=length2>1.e-10?std::clamp(pm::dot(p-f.previous[side],delta)/length2,0.,1.):0.;
    // Bounded palm/knuckle volume; the candidate point and target/body are still
    // constructed and validated by the original native collision producer.
    receipt.distance=pm::length(p-(f.previous[side]+delta*along));
    // The exact synchronous redirected query already used KCD2's unchanged
    // native fist primitive at this committed slot. A second 12 cm palm sphere
    // is not that shape: it rejected real native contacts at 121.3/121.9 mm.
    // Retain the old bound for every other path, including experimental replay.
    receipt.nativeShapeH31=nativeShapeH31;
    if(!nativeShapeH31&&receipt.distance>.12)return fail(fh::FailureH31::OutsidePalm);
    receipt.failure=fh::FailureH31::None;return 1;
}
int FistContactAdmissionH11(const void* actor,const float* point,int side) noexcept {return FistContactFrameAdmissionH31(actor,point,side,nullptr);}
int FistContactAdmissionH12(const void* actor,const float* point,int nativeHand,const void* details) noexcept {
    namespace fh=kcd2vr::fist_action_h12;auto& receipt=fh::receiptH31;receipt={};receipt.nativeHand=nativeHand;
    if(diagnostics::lab::NativeReference())return -1;
    if(!FistSelected101()){receipt.failure=fh::FailureH31::GameplayOwner;return FistContactRequestedH31()?0:-1;}
    fh::Binding b{};
    const auto now=std::uint64_t(GetTickCount64());
    if(!fh::context(reinterpret_cast<std::uintptr_t>(actor),reinterpret_cast<std::uintptr_t>(details),nativeHand,now,b,&receipt.failure))return 0;
    receipt.serial=b.intent.serial;receipt.gestureSample=b.intent.sample;receipt.side=b.intent.side;
    if(b.intent.policy!=g_attack_timing_policy22.load()){receipt.failure=fh::FailureH31::PolicyChanged;return 0;}
    if(b.intent.epoch!=g_pose_epoch6689.load()){receipt.failure=fh::FailureH31::EpochChanged;return 0;}
    if(b.intent.request!=g_fist_epoch101.load()){receipt.failure=fh::FailureH31::RequestChanged;return 0;}
    if(!retail_build112::Ready(WhBase())){receipt.failure=fh::FailureH31::RetailMismatch;return 0;}
    if(!fh::receiver(b,fh::Scope::live->callback,WhBase(),ClockRead50,&receipt.failure))return 0;
    const FistContactFrameH11* originalH31=nullptr;bool nativeShapeH31=false;
    if(g_weapon_control_mode.load()==7){
        const auto* sweep=g_prediction_scope120;
        std::uintptr_t currentCollisionsH31=0;(void)ClockRead50(b.intent.actor+0x218,&currentCollisionsH31,8);
        if(g_prediction_contract120.load()!=WhBase()||g_prediction_fault120.load()||!sweep||!sweep->fistH31||!sweep->redirected||sweep->fistGenerationH31!=b.generation||
           sweep->fistGestureSampleH31!=b.intent.sample||sweep->hand!=b.nativeHand||
           currentCollisionsH31!=reinterpret_cast<std::uintptr_t>(sweep->collisions)||
           details!=static_cast<unsigned char*>(sweep->collisions)+0x170){
            receipt.failure=fh::FailureH31::NoTrackedNativeSweep;return 0;
        }
        originalH31=&sweep->fistContactH31;
        const auto* shape=kcd2vr::contact176::current;
        nativeShapeH31=!sweep->fistReplayR10&&shape&&shape->nativeFistShapeH31&&shape->redirected&&
            shape->identity.player==b.intent.player&&shape->identity.collisions==currentCollisionsH31&&
            shape->identity.generation==b.generation&&shape->identity.epoch==b.intent.epoch&&shape->identity.request==b.intent.request&&
            shape->identity.sample==originalH31->sample&&shape->identity.tick==originalH31->tick&&
            shape->wrapper==reinterpret_cast<std::uintptr_t>(sweep->wrapper)&&shape->action==reinterpret_cast<std::uintptr_t>(sweep->action)&&
            shape->hand==b.nativeHand&&shape->bone==sweep->bone;
    }
    // The native chosen row/hand remain unchanged. Only this admitted action's
    // actual VR fist owns contact; the latest proposed punch is never consulted.
    return FistContactFrameAdmissionH31(actor,point,b.intent.side,originalH31,originalH31&&g_prediction_scope120&&g_prediction_scope120->fistReplayR10,nativeShapeH31);
}
// Physics callbacks may consume copied tracking, but cannot enter optional HJ1
// late-location/native-frame leaves through ReadTrackingSnapshot.
static unsigned FistHandMaskD48() noexcept {
    TrackingSnapshot t{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_tracking_frames6695.read(t)||!t.allowed||!t.head_valid||t.head_held||
       !t.tick||now<t.tick||now-t.tick>80||
       (t.source_epoch&&t.source_epoch!=tracking59c6::epoch.load(std::memory_order_acquire)))return 0u;
    // A lost/held opposite controller cannot revoke this current fist's owner.
    // Each bit belongs to its own valid controller in the same copied sample.
    return (t.right_valid&&!t.right_held&&t.right_grip>.60f&&t.right_trigger>.60f?1u:0u)|
           (t.left_valid&&!t.left_held&&t.left_grip>.60f&&t.left_trigger>.60f?2u:0u);
}
static unsigned PhysicalHandReservationsBaseH40(bool allowNativeReadsD41=true) noexcept {
    if(animation_filter::nativeBodyOwned112.load()||!g_gameplay_allowed.load()||
       !TrackingHeadUsable111()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_menu.load()||g_ctx_scripted.load())return 3;
    // D48: a fist reserves its own hand. It cannot monopolize the free hand.
    const unsigned fistsD48=FistSelected101()&&!kcd2vr::hands59c7f::active()?FistHandMaskD48():0u;
    // F5 explicitly owns verified empty hands even while the F4/F10 mode remains
    // selected. Unknown equipment, a held weapon, support and bow still use the
    // original reservations. ExactHandItemState can fall back to native slots
    // in lower modes. The physics callback uses only copied equipment below,
    // even if workstation ownership or mode changes after its context check.
    if(allowNativeReadsD41&&kcd2vr::workstation178r20c1::ownsWrist(GetTickCount64())&&
       ExactHandItemState(0)==0&&ExactHandItemState(1)==0)return fistsD48;
    const int mode=g_weapon_control_mode.load();if(!mode)return fistsD48;if(mode<4)return 3;
    PhysFrame6698 f{};const auto now=std::uint64_t(GetTickCount64());
    static std::atomic<unsigned> lastMask178r10{0};static std::atomic<std::uint64_t> lastMaskTick178r10{0},lastMaskKey178r10{0};
    const std::uint64_t maskKey178r13=(g_weapon_request_epoch6694.load()<<32)^g_pose_epoch6689.load(); // V66.178R13: an epoch change cannot reuse the old mask
    if(!g_phys_frame6698.read(f)||!f.input.allowed||!f.input.tick||now<f.input.tick||now-f.input.tick>150||
       f.request!=g_weapon_request_epoch6694.load()||f.input.bodyEpoch!=g_pose_epoch6689.load()){
        // V66.178R10: a stale physics frame (a stalled game frame) no longer reserves both hands outright.
        // Carry the last computed mask for 400 ms; with both hands known empty nothing is reserved.
        const auto t=lastMaskTick178r10.load(std::memory_order_acquire);
        if(t&&now>=t&&now-t<=400&&lastMaskKey178r10.load(std::memory_order_acquire)==maskKey178r13)return lastMask178r10.load(std::memory_order_acquire)|fistsD48;
        PhysEquipment6698 e{};
        if(g_phys_equipment6698.read(e)&&e.valid&&e.tick&&now>=e.tick&&now-e.tick<=400&&e.manager&&e.manager==g_henry_item_attach_manager.load()&&
           e.request==g_weapon_request_epoch6694.load()&&e.handKnown[0]&&e.handKnown[1]&&!e.handKey[0]&&!e.handKey[1])return fistsD48;
        return 3;
    }
    unsigned mask=0;
    for(int side=0;side<2;++side){
        // Empty-hand render ownership is not interaction ownership. Only a
        // real equipped profile/socket reserves this hand outside a closed fist.
        if(f.profiles[side].key)mask|=1u<<side;
        if(f.output.support[side]||f.profiles[side].bow())mask|=3u;
        const int other=1-side;const auto& profile=f.profiles[side];
        if(profile.key&&profile.twoHanded&&profile.support&&f.input.tracked[other]&&
           f.output.active[side]&&ph::finite(profile.grip[other])&&
           pm::length(f.input.hands[other].p-ph::compose(f.output.weapons[side],profile.grip[other]).p)<.24)
            mask|=1u<<other; // reserve the support socket before the F12 dwell completes
    }
    lastMask178r10.store(mask,std::memory_order_release);lastMaskTick178r10.store(now,std::memory_order_release);lastMaskKey178r10.store(maskKey178r13,std::memory_order_release);
    return mask|fistsD48;
}
unsigned PhysicalHandReservations114() noexcept {return PhysicalHandReservationsBaseH40()|kcd2vr::hand_only_door_h40::Reserved();}
unsigned PhysicalHandReservationsPhysicsD41() noexcept {return PhysicalHandReservationsBaseH40(false)|kcd2vr::hand_only_door_h40::Reserved();}

}

namespace kcd2vr::frik_pose_probe {
void ObserveImmersiveCamera122(float x,float y,float z,int eye,std::uint64_t xrFrame) noexcept {
    if(g_census_mode.load()<3||!g_gameplay_allowed.load())return;
    static std::atomic<std::uint64_t> next{0};const auto now=std::uint64_t(GetTickCount64());auto due=next.load();
    if(now<due||!next.compare_exchange_strong(due,now+1000))return;
    (void)CaptureCameraBody122({x,y,z},eye,xrFrame);
}
#include "subsystems/vrik/dodge_epoch_h24.inl"
#include "subsystems/vrik/block_indicator_context_h24.inl"
#include "subsystems/vrik/tracked_camera_service.inl" // M2_OWNED_SOURCE
#include "subsystems/ranged_reload/bow_camera_service.inl" // M2_OWNED_SOURCE
}

namespace kcd2vr::door66 {
bool OwnedMotionFor(std::uintptr_t actor,std::uint32_t target,std::uintptr_t entity,std::uint64_t now,std::uint64_t* serial) noexcept {
    return frik_pose_probe::DoorNativeMotionOwned66(actor,target,entity,now,serial);
}
}
namespace kcd2vr::frik_pose_probe {
void PublishQuickTalkOwnerH4(unsigned mask,std::uint64_t tick) noexcept {
 namespace q=kcd2vr::quick_talk_owner_h4;q::Stamp s{};PhysEquipment6698 e{};
 const auto now=std::uint64_t(GetTickCount64());
 if(g_phys_equipment6698.read(e)&&e.valid&&e.player&&e.tick&&now>=e.tick&&now-e.tick<=100&&e.epoch==g_pose_epoch6689.load()&&e.request==g_weapon_request_epoch6694.load()){
  s.tick=tick;s.body=e.epoch;s.request=e.request;s.player=reinterpret_cast<std::uintptr_t>(e.player);
  if(g_gameplay_allowed.load()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&tick&&now>=tick&&now-tick<=80)
   for(unsigned side=0;side<2;++side)if((mask&(1u<<side))&&e.handKnown[side]&&!e.handKey[side])s.mask|=1u<<side;
 }
 (void)q::mailbox.publish(s);
}
}
