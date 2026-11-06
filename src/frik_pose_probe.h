#pragma once
#include "subsystems/physics/corpse_grip_cg1.h"
#include <atomic>
#include "subsystems/physics/runtime/committed_hand_push_h21.h"
#include "core/collab_test_h21.h"
#include "diagnostics/combat_flow_h12.h"
#include "vrik/workstation_sample46.h"
#include "vrik/user_modes175.h"
#include "vrik/calibration_restore43.h" // C43
#include "vrik/native_held_owner_t4_status.h"
#include <cstdint>
#include "vrik/original_contact_wait_h8.h"
#include "dialogue_state59c10r2.h"

#include "vrik/tracking_origin125.h"
namespace kcd2vr::native_roomscale_d76 {struct Origin;}
namespace kcd2vr::self_contact54 {struct Event;}
namespace kcd2vr::contact176 {struct Live;struct Ticket;}
namespace kcd2vr::answer59c4r2 {struct Lineage;}
namespace kcd2vr::subsystems::smithing::contact68 {struct HammerFrame;}
namespace kcd2vr::constraint_op60 {struct Op;}
namespace kcd2vr::solver_command_h28 {struct Stamp;}
namespace kcd2vr::frik_pose_probe {
void SetGrabMode60(int mode) noexcept;
int GrabMode60() noexcept;
void SetLab58(bool livingHitImpact,bool corpseDrag,bool propWake,float livingK) noexcept;
void SetLab58LivingTouch(bool on) noexcept;
// C54 completed exact empty-hand/sword moving-contact edge; voice input only.
bool ReadSelfPat54(kcd2vr::self_contact54::Event&,std::uint64_t now) noexcept;
// Exact Henry, pre-VRIK head/model transform from the existing skinning callback.
// False after 60 ms, character/identity change, calibration or recenter epoch change.
bool NativeDialogueHead59c10r2(dialogue_state59c10r2::NativeHead&,std::uint64_t now) noexcept;
// Current original-player pose copied at the existing native FirstPerson view
// callback. It is independent of whether Henry was visible/skinned this frame.
void ObserveNativeDialogueHead28(const dialogue_state59c10r2::State&,std::uint64_t now) noexcept;
bool NativeDialogueViewHead28(dialogue_state59c10r2::NativeHead&,
    const dialogue_state59c10r2::State&,std::uint64_t now) noexcept;
struct DialogueVisual28 {
    std::uint64_t epoch=0,skinTick=0,frameTick=0,sample=0;
    std::uintptr_t skinCharacter=0,frameCharacter=0;
    bool bodyAdmitted=false,gameplay=false,poseValid=false,haveFrame=false,frameFresh=false;
    bool right=false,left=false,nativeBody=false;
};
DialogueVisual28 DialogueVisualSnapshot28(std::uintptr_t character,std::uint64_t now) noexcept;
void SetNativeSpecialContext168(bool allowed) noexcept;
bool RequestNativeSpecial168() noexcept;
// Read the current exact Henry main action, only for an active retail free-kick alias.
bool NativeFreeKickVisualR24() noexcept;
enum class FistKickResultH35 {NotThisLane,Refused,Queued};
FistKickResultH35 RequestFistKickStatusH35(int side) noexcept;
bool FistKickEnabledH35() noexcept;
void SetFistKickEnabledH35(bool on) noexcept; // H41: default-OFF authored R3 kick; callers use tri-state so refusal cannot choose a different action
// F4 Route B visual/contact probe only. Native kick damage/stamina remain gated.
bool RequestProceduralKick176() noexcept;
// C37: VR LEG KICK style - the leg plus ONE kick transaction bound to Henry/epoch (at most one native type-4 hit).
bool RequestVrLegKick34() noexcept;
void CancelProceduralKick176() noexcept;
void SetMotionClock50C2(bool on) noexcept;
void SetCombatAdmissionAssist51(bool enabled) noexcept;
bool ReadF4ContactLive176(kcd2vr::contact176::Live&) noexcept;
bool GeometryAdmittedRelease51(const kcd2vr::contact176::Ticket&,const void* receiver=nullptr) noexcept;
bool GeometryNativeAnswer19c4r2(const void* actor,const void* target,int attackType,kcd2vr::answer59c4r2::Lineage&) noexcept;

// F5 bounded lifecycle capture / separately gated rung-2 experiment; control thread only.
bool SetInteractionLifecycle165(bool on) noexcept;
bool InteractionLifecycleReady36() noexcept;
bool SetInteractionExperiment36(bool on) noexcept;
void ServiceInteractionExperiment36() noexcept;
unsigned InteractionCoverage175() noexcept;
bool ApplyUserMode175(kcd2vr::user175::Mode mode) noexcept;
bool CombatExperiment175Enabled() noexcept;
void SetClinchDistanceMeters32(float meters) noexcept; // F4 initiation-only radius; zero=retail, default0.55m
void SetClinchModeD40(int mode) noexcept; // 0=distance/legacy, 1=current contact + fresh grip, 2=automatic off
int ClinchModeD40() noexcept;
bool ClinchGateReadyD40() noexcept; // native hook installed; not an accepted clinch/physical hold
void SetNpcProximityRadius36(float meters) noexcept; // F3 reference probe,20..60cm; no NPC guard write
float NpcProximityRadius36() noexcept;
float ClinchDistanceMeters32() noexcept; // copied preference, no native IO
bool CombatIntegrated178r22c2Enabled() noexcept; // V66.178R28: F4 rung 1 (R22C2 integrated native combat)
bool CombatSynchronized59c10r9() noexcept; // R59.16C10R9: F4 rung 3 = rung 1 + VR contact authority
bool CombatIntent59c16() noexcept; // R59.16C10R16: F4 press 1 = SYNCHRONIZED + VR intent rules
int F4ContactGateMode178r26() noexcept; // V66.178R26
bool PhysicalCombat177Enabled() noexcept;
void SetContactStartsAttack178r41(bool on) noexcept; // V66.178R41
void SetAttackAtPrepare178r42(bool on) noexcept; // V66.178R42
void SetWeaponAuthorityScale178r44(float k) noexcept; // V66.178R44: 1.0 = R177 authority, lower = heavier blade
void SetWeaponFeel59c14(float fatigueRamp,float poleOneHand,bool strikeMinScaleGate,bool liveAttributesAllRungs) noexcept; // R59.16C10R14
void SetPerfectBlockWindow178r42(int value) noexcept; // R44C1: compatibility only; native window is unchanged.
void SetPlayerCombatWindowScaleH26(float scale) noexcept; // 1=native allowance, bounded1..2; exact Henry PB/MS only.
bool CombatFull178r10Enabled() noexcept; // V66.178R10 F4 rung 3
int F4Rung178r10() noexcept;             // 0 off, 1..5 (R15: 4 combo/master coach, 5 impale latch)
// C46R1: one coherent, fresh XR input observation; no native pointers leave the owner thread.
bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept;
void SetF4ContactGate178r26(int mode) noexcept;   // V66.178R26: 0 pass-through (V174), 1 release-ticket gate (V176)
void SetSupportHiltRegion178r26(bool on) noexcept; // V66.178R26: longsword hilt sliding region (V169R2) on/off
void SetTwoHandV120_178r27(bool on) noexcept;      // V66.178R27: V66.120/123 two-hand socket (toward the blade, authored frame)
void SetTwoHandConstraintV120_178r29(bool on) noexcept; // V66.178R29: 8 deg nudge instead of the V130 constraint (default off)
void SetGuardEntry178r29(float maxSpeed,unsigned dwellMs) noexcept; // V66.178R29: F4 block entry needs a slow blade for this long
void SetGuardAfterAttackMs49(unsigned ms) noexcept; // V66.178R49: no guard entry this long after a committed stroke
void SetGuardStrict59c17(bool on) noexcept; // R59.16C10R17: strict guard entry (blade in front, not after a feint, forward-only exit)
void SetGuardExitSpeed178r45(float speed) noexcept; // V66.178R45: a held guard releases above this blade-centre speed (0 = position only)
void SetMasterHandoff23(bool enabled,float forwardSpeed) noexcept; // C23: guard->strike handoff only inside the live native +7D8 window (speed <0 = VrGuardExitSpeed)
void SetGuardCounterMotion57C1(bool on) noexcept;
void SetGuardAutoWindup19c4r1(bool on) noexcept;
void SetBlockCooldownLease59c17(bool leased) noexcept;
void SetCombatFlow59c17(float nativeHitTime,bool masterKeepStance,float weaponStatEffect) noexcept; // R59.16C10R17: NPC reaction time, MS stance keep, RPG stat drag
void CombatFlowReceipts59c17(char* out,std::size_t cap) noexcept; // R59.16C10R17: cumulative receipts for the normal log // R59.16C10R17: mod.cpp leases wh_cs_BlockCooldown=0 in F4 (receipt in GUARD_R17)
void SetGuardIncoming178r45(bool repress,int phaseMask,unsigned cooldownMs) noexcept; // V66.178R45: one block re-press per incoming native attack phase
void SetThrustAsSwing55(bool on) noexcept;
void SetWindupHold57(bool on) noexcept;
void SetTelegraphWindup59c10(bool on) noexcept; // R59.16C10
void SetWallFriction57(float friction) noexcept;
void SetWallSlide59c10(bool on) noexcept; // R59.16C10d
void SetAttackOpponentGate59c10(bool requireOpponent,float reachMeters) noexcept; // R59.16C10R4F: native attack request only toward an opponent in reach
bool RequestKick59() noexcept; // V66.178R59
bool KickDrop59C2(int& reason,std::uint64_t& tick) noexcept; // R59.16C2: expired R3 kick + reason
void SetMasterStrikeAllWeapons59(bool on) noexcept; // V66.178R59
void SetMasterStrikeVeto59c14(bool veto) noexcept; // R59.16C10R14: false = the game decides master strikes (no VR ticket veto)
void SetTwoHandAxis59c16(bool on) noexcept; // R59.16C10R16: off hand latches where it closes on the handle/shaft
void SetHideStowedBow59c14(int mode,float pitchDeg) noexcept; // R59.16C10R14: 0 native, 1 hidden when looking up, 2 always hidden while stowed
void SetHeadPitch59c14(float pitchDeg) noexcept; // R59.16C10R14: recentred HMD pitch (deg, up = positive), per frame from mod.cpp
void* HenryItemAttachManager59c15() noexcept; // R59.16C10R15: Henry's C_ItemAttachmentManager (player+0x9D0, vtable-checked when stored)
void SetHideStowedWeaponsHW(bool atStations,bool back) noexcept; // D33: VrHideStowedAtStations / VrHideBackWeapons
void SetStowedBowAttachments59c15(const char* csv) noexcept; // R59.16C10R15: Henry's back attachments hidden by VrHideStowedBow
void SetCombatNoStep55(bool on) noexcept;        // V66.178R55: skip the native attack alignment (step/turn) in F4       // V66.178R55: thrust = ordinary attack from its origin side (default on)
bool CombatNoStep55() noexcept; // D40: live value of existing VrCombatNoStep policy.
// D40: early ordinary Henry self-rotation only; independent from the late
// NoStep query. Readiness is the verified installed hook, not the preference.
void SetCombatNoTurnD40(bool on) noexcept;
bool CombatNoTurnD40() noexcept;
bool CombatNoTurnReadyD40() noexcept;
bool CombatFreeViewContextD42() noexcept;
std::uint64_t CombatNoTurnVetoCountD40() noexcept;
void SetDirectionalStabs178r29(bool on) noexcept;  // V66.178R29: V167 directional stabs (default off: thrust = stab zone)
void SetGeometryAutoLock178r29(bool on) noexcept;  // V66.178R29: rung 2 auto opponent lock when not in combat
bool GeometryAutoLock178r29() noexcept;
void SetAutoPalm59c8(bool on) noexcept;
void SetHeadRaiseMax59c9(float cm) noexcept;
void SetStrikeReferenceJ59c9(float joules) noexcept; // R59.16C9C: strike energy that counts as power 1.0 // R59.16C9C: VRIK head may rise this much above the calibrated anchor (INI VrHeadRaiseMaxCm, default 18) // R59.16C8: wrist goal = grip pose - half wrist->knuckle bar (palm centre on the controller)
void SetWristFromGrip59c7f(float cm) noexcept; // R59.16C7F: IK hand goal moved down the controller handle (INI VrWristFromGripCm, default 0)
bool GuardOwnedByUs59c7e(const void* state) noexcept; // R59.16C7F: our posture block input is held (or released <=250 ms ago) on THIS Henry state object
bool HybridQuietGuardOwnedH20(const void* actor,const void* state) noexcept; // exact own quiet128/256 only; not contact/cost authority
int GeometryOwnerStage49() noexcept; // V66.178R49: 0 ok, 1 frame, 2 fresh, 3 pose epoch, 4 request epoch, 5 allowed/mode, 6 native body owns Henry (hit reaction/clip), 7 menu/dialogue, 8 weapon key/generation
bool GeometryOwnerValid178r30(std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept; // Strict completed physical pose/new-contact owner, never display fallback.
// H30: current tracked gesture source for original semantic commands/native
// action ancestry. This never qualifies a new collision or a displayed pose.
bool GeometryIntentOwnerH30(std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept;
// 0=lost, 1=complete current owner, 2=same current owner publishing commands.
// State2 is not input/contact authority and cannot start or extend a combo slot.
unsigned GeometryIntentOwnerStateH30(std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept;
// Original checked contact only; original sample/gesture/impact clock are never
// refreshed by a temporary presentation recovery.
bool OriginalGeometryContactOwnerH30(std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,
    std::uint64_t sample,std::uint64_t gesture,std::uint64_t impactTick,std::uint64_t now) noexcept;
// Bounded postponement only for an already checked pre-quarantine occurrence.
// It does not satisfy either current command or contact ownership predicate.
kcd2vr::original_contact_h8::Decision WaitOriginalGeometryContactH8(
    const kcd2vr::original_contact_h8::Source&,const kcd2vr::contact176::Ticket&,std::uint64_t now) noexcept;
bool GeometrySolverOwnerValidH28(const kcd2vr::solver_command_h28::Stamp&,std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,std::uint64_t sample,std::uint64_t gesture) noexcept;
bool GeometrySolverCostOwnerValidH28(const kcd2vr::solver_command_h28::Stamp& proof,std::uint64_t gesture,std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,std::uintptr_t actor) noexcept;
void SetFistNativeArm178r15(bool on) noexcept; // retired compatibility setting; tracked fist remains controller-owned
void SetImpaleLatch178r15(bool on) noexcept;
void SetKick26(int style,int legSide,int reachMs,bool freesImpale) noexcept; // C26 KICKS page
void SetStatSim26(int mode) noexcept; // C26c SIMULATE HENRY (weapon feel only)
void NoteKickRequest26(bool toGame) noexcept;
void ServiceKick26(std::uint64_t now) noexcept;   // V66.178R15: F4 rung 5 impale latch experiment
void RequestGoreVisualRestore59() noexcept; // request only; exact native owner performs visual cleanup
void SetNpcFlinch178r11(bool on) noexcept; // V66.178R11 experimental visual NPC flinch (F4 rung 3)
void SetContactEffects178r12(bool on) noexcept;
void SetHoverAffordance178r14(int side,bool on) noexcept; // V66.178R14: fingers open a little over a usable target
void SetNativeHandover178r14(bool on) noexcept;
void SetGrabHandStays43(bool on) noexcept; // C43: ACTIONS > GRAB: HAND STAYS ON CONTROLLER (INI VrGrabHandStaysOnController)
bool GrabHandStays43() noexcept;
void NoteVrPickupUse43(int side,std::uint32_t entity,bool pickableItem,std::uint64_t nowMs) noexcept; // C43: a VR hand Use pulse was sent for this candidate
bool HoverHaptic178r14(int side,std::uint64_t key) noexcept; // one short tick when a new usable target comes under the hand         // V66.178R14: the using hand follows the game's animated hand during an accepted action // V66.178R12 experimental native material effects on our contacts (F4 rung 3)
void SetVrCombatCore178(float minScale,unsigned minIntervalMs,bool blockWhileActive) noexcept;
bool SetReloadDiagnostics175(bool on) noexcept;
bool ReloadDiagnostics175() noexcept;
int ReloadMode47C2() noexcept;
bool ReloadExperiment47C2() noexcept;
// Accepted F11=2 draw policy, independent of the finite reload recorder.
void SetBowGripDrawDefault596C1(bool on) noexcept;
// Same exact native staged quiver/nock policy as F11=2, independent of recording.
void SetStagedBowDefault59(bool on) noexcept;
bool StagedBowDefault59() noexcept; // saved requested policy, independent of temporary F11 override
bool PrepareStagedBowDefault59() noexcept;
bool PrepareGameplayResourcesD84() noexcept;
bool StagedBowEnabled59() noexcept;
bool BowGripDrawEnabled596C1() noexcept;
bool SetReloadMode47C2(int mode) noexcept;
bool InteractionLifecycleActive168() noexcept;
int HandsMode59c7f() noexcept;
bool SetHandsMode59c7f(int rung) noexcept;
// Capture-only: never calls gameplay mode setters. Bit mask: retail/input/sweep/contact/material/animation.
unsigned SetCombatObservation168r1(bool on) noexcept;

inline bool FullWeaponMode120(int mode) noexcept { return mode==6||mode==7; }
inline bool BowHandlingMode59c9(int mode) noexcept {
    return FullWeaponMode120(mode)||((mode==4||mode==5)&&BowGripDrawEnabled596C1());
}

// V66.98 native-frame-owned physical weapon extension. F12 0..3 remain comparisons.
bool ArmPhysicalWeapons6698() noexcept;
bool FistInputOwned101() noexcept;
bool F4GestureInputOwned176() noexcept; // copied equipment only; F4 melee/unknown excludes native LT/RT
float FistTransportTrigger101(int side,float raw,bool tracked,bool menu,float grip=0.f) noexcept;
void ObserveR3Combat101(bool down) noexcept;
// Copied diagnostic payloads only; sole consumer is the mod worker.
void DrainCopiedDiagnosticsH47(std::uint64_t now) noexcept;
void ServiceDrawDiagnosticsD77() noexcept;
void PhysicalNativeFrameTick6698() noexcept;
void PhysicalNativeFrameEnd104() noexcept;
enum class PhysicalAttachmentResult101 {NotMine,Waiting,Placed,Restored,Busy,Fault};
PhysicalAttachmentResult101 PhysicalAttachmentUpdate101(void* self,void* attachment) noexcept;
bool PhysicalAttachmentPending6698() noexcept;
bool PhysicalWeaponInputOwned6698() noexcept;
bool PhysicalWeaponConsumesRightTrigger139() noexcept;
void ShutdownPhysicalWeapons6698() noexcept;

struct TrackingPose {
    float px=0.0f, py=0.0f, pz=0.0f;
    float qx=0.0f, qy=0.0f, qz=0.0f, qw=1.0f;
};

// V64.19C: Dynamic Detector-inspired native game-state correlation.
// agency_known is populated only when the exact-build CCryAction ->
// CActionMapManager -> IsFilterEnabled path validates at runtime. Any mismatch
// remains fail-closed with agency_known=false.
struct GameStateContext {
    bool dialogue=false;
    bool cutscene=false;
    bool menu=false;
    // Native activity controls block gameplay, but are not a real UI menu.
    bool workstation_controls_only=false;
    bool tool_pose_ui_blocked=false;
    bool native_reading=false; // Exact native Reading lifecycle, never HUD visibility.
    bool hud_hidden=false; // Diagnostic presentation state only.
    bool agency_known=false;
    int agency_count=-1;
    bool scripted=false;
};

enum class VrikTestMode : int {
    Off = 0,
    RightHandVisibleProof = 1,
    RightTouchPosition = 2,
    RightTouchPositionWrist = 3,
    RightTwoBonePosition = 4,
    RightTwoBoneWrist = 5,
};

struct ProbeStatus {
    int mode=0;
    bool rig_valid=false;
    bool calibrated=false;
    bool tracking_valid=false;
    bool gameplay_allowed=false;
    std::uint32_t joint_count=0;
    int hand_r=-1;
    int hand_l=-1;
    int head=-1;
    bool named_joint_api_valid=false;
    int named_hand_r=-1;
    int named_hand_l=-1;
    int weapon_slot_r=-1;
    int weapon_slot_l=-1;
    int weapon_slot_r_parent=-2;
    int weapon_slot_l_parent=-2;
    bool weapon_slot_r_resolved=false;
    bool weapon_slot_l_resolved=false;
    bool weapon_solver_recent=false;
    int solver_joint_c=-1;
    int solver_joint_e=-1;
    std::uint64_t correlated_calls=0;
    std::uint64_t setik_calls=0;
    std::uint64_t rig_maps=0;
    std::uint64_t final_output_writes=0;
    std::uint64_t prebuild_writes=0;
    std::uint64_t setik_upstream_writes=0;
    std::uint64_t setik_hold_writes=0;
    bool pending_hold_active=false;
    std::uint64_t rejects=0;
    float upper_length=0.0f;     // direct selector-1 U->F length
    float fore_length=0.0f;      // direct selector-1 F->H length
    float ref_upper_length=0.0f;
    float ref_fore_length=0.0f;
    float controller_delta=0.0f;
    int fail_stage=0;
};

struct VrikDiagnostics {
    bool valid=false;
    std::uint64_t sample=0;
    int upper=-1, fore=-1, elbow=-1, hand=-1;
    int parent_upper=-2, parent_fore=-2, parent_elbow=-2, parent_hand=-2;
    int solver_joint_c=-1, solver_joint_e=-1;
    float b0_upper[3]{}, b0_fore[3]{}, b0_elbow[3]{}, b0_hand[3]{};
    float b1_upper[3]{}, b1_fore[3]{}, b1_elbow[3]{}, b1_hand[3]{};
    float ref_upper[3]{}, ref_fore[3]{}, ref_elbow[3]{}, ref_hand[3]{};
    float out_upper[8]{}, out_fore[8]{}, out_elbow[8]{}, out_hand[8]{};
    // Four common DQ translation interpretations. V62.9 uses the actual semantic
    // UPPERARM_R -> FOREARM_R -> HAND_R chain (403 -> 405 -> 408), not ELBOW_R helper 406.
    float cand_l1[4]{}, cand_l2[4]{};
    float b1_l1=0.0f, b1_l2=0.0f;
    float ref_l1=0.0f, ref_l2=0.0f;
    float controller_delta_cry[3]{};
    float arm_goal_r[3]{};          // V64.0 semantic RgtArm01 absolute goal in Henry model space
    float arm_goal_l[3]{};          // V64.0 semantic LftArm01 absolute goal in Henry model space
    float arm_effector_r[3]{};      // solved HAND_R skeleton effector; not the target
    float arm_effector_l[3]{};      // solved HAND_L skeleton effector; not the target
    float arm_goal_error_r=0.0f;    // target minus reachable solved effector distance
    float arm_goal_error_l=0.0f;
    int weapon_slot_r=-1, weapon_slot_l=-1;
    float weapon_slot_r_native[3]{};
    float weapon_slot_l_native[3]{};
    float weapon_slot_r_goal[3]{};
    float weapon_slot_l_goal[3]{};
};

bool Install() noexcept;
void Uninstall() noexcept;
void PublishTrackingSnapshot(const TrackingPose& head, bool head_valid,
                             const TrackingPose& left, bool left_valid,
                             const TrackingPose& right, bool right_valid,
                             float left_grip, float right_grip,
                             float left_trigger, float right_trigger,
                             bool gameplay_allowed, std::uint64_t sourceTick=0, std::uint64_t sourceNs=0, std::uint64_t sourceEpoch=0, std::int64_t poseTime=0,
                             bool dialogueFingersD78=false, float leftVisualTriggerD78=0.f, float rightVisualTriggerD78=0.f) noexcept;
void PublishGameStateContext(const GameStateContext& state) noexcept;
// V66.100: F8 uses the CURRENT three-second/2s T-pose path. Only measured
// player reach is persisted; session origin/alignment is rebuilt automatically.
void ConfigureCalibrationPersistence(const char* iniPath) noexcept;
void BeginArmLengthCalibration() noexcept;
bool CompleteArmLengthCalibration() noexcept;
void CancelArmLengthCalibration() noexcept;
void RequestTrackingReanchor() noexcept;
// C43 calibration restore (calibration_restore43.h). AutoRestore: INI VrCalibrationAutoRestore / menu checkbox (default on).
void SetCalibrationAutoRestore43(bool on) noexcept;
bool CalibrationAutoRestore43() noexcept;
// Vertical correction a re-seat applies: how far the headset is above the calibrated standing eye height now (0 = none known).
float StandingEyeShift43() noexcept;
// Edge action at an owner exit (dialogue, activity, native body, recenter, mode on, menu button): re-asserts the calibrated
// head anchor, writes the C43 CALIBRATION receipt (restored / drift detected) once per edge. Returns true when restored.
bool RestoreCalibration43(const char* owner) noexcept;
void SetTrackingOriginProbeH46(kcd2vr::tracking125::OriginReaderH46 probe) noexcept;
// Menu RESTORE CALIBRATION: the next recenter re-seats with the standing correction even when auto restore is off.
void ForceCalibrationRestore43() noexcept;
// Receipt helper: the published VR camera position (mod.cpp passes CameraHook::GetLatestVisualPositionWorld).
void SetViewPositionProbe43(bool(*probe)(float&,float&,float&,std::uint64_t)) noexcept;
// A stored calibration exists (arm profile from INI or this session's F8) and can be applied without a new T-pose.
bool CalibrationStored43() noexcept;
void BeginFingerCapture() noexcept;
bool FingerCaptureActive() noexcept;
int FingerCaptureStatus() noexcept;
int CalibrationPersistenceStatus() noexcept;
const char* FingerCaptureStatusName(int stage) noexcept;
const char* CombatCensusRungLabel(int mode) noexcept;

void ResetCalibration() noexcept;
// Current guided capture arming helper. Ordinary recenter now uses
// RequestTrackingReanchor; it does not demand another T-pose. Controller-owned
// wrist targets and zero learned wrist offsets remain authoritative.
void RequestCalibrationFromRecenter() noexcept;
bool CalibrationReady() noexcept;
// Current guided T-pose accumulator; only pose=1 is active. No wrist-offset learning.
void BeginGuidedCalibrationPose(int pose) noexcept;
void EndGuidedCalibrationPose(int pose) noexcept;
void SetFinalSkinTestMode(int mode) noexcept;
int FinalSkinTestMode() noexcept;
const char* ModeName(int mode) noexcept;

// Current F10: 0 OFF; 1 bounded native reference; 2 tracked arms and saved/native
// curve fingers; 3 connected upper body; 4 opt-in static contact. One
// staging/commit/restore owner. Integer mode IDs remain 0..4.
// F10 1.5 is an arms-only mode-2 substage with native fingers; F8 selects it.
void SetCombatCensusMode(int mode) noexcept;
void CycleCombatCensusMode() noexcept;
int CombatCensusMode() noexcept;
const char* CombatCensusModeName(int mode) noexcept;

// V65.68 F9 read-only upstream weapon ownership ladder. Never changes input or writes game state.
void SetWeaponUpstreamDiagMode(int mode) noexcept;
void CycleWeaponUpstreamDiagMode() noexcept;
int WeaponUpstreamDiagMode() noexcept;
const char* WeaponUpstreamDiagModeName(int mode) noexcept;

// V66.89 F12: 0 OFF, 1 exact right-item latch, 2 bounded +20cm world-X injection
// through BOTH native attachment consumers + readback/restore, 3 explicit hand-follow
// request requiring current-item proof/readback/restoration. Visibility is user-verified.
void SetWeaponControlMode(int mode) noexcept;
// V66.128 body follow / chest twist / bow string
void SetBodyFollow128(bool on) noexcept;
bool BodyFollow128() noexcept;
void SetChestTwist128(bool natural,bool handBias) noexcept;
void SetBowStringFollow128(bool on) noexcept;
void SetWeaponConstraints134(bool drive,bool twoHand) noexcept;
void SetPreciseContacts44(bool on) noexcept;
void SetBloodyFights49(bool on) noexcept; // C49 VrBloodyFights (IMMERSION BLOODY FIGHTS)
bool BloodyFights49() noexcept;
bool PreciseContacts44() noexcept;
int HandsModeDs2() noexcept;            // DS2: 0 NORMAL, 1 PRECISE, 2 PERFORMANCE
void SetHandsModeDs2(int mode) noexcept; // also sets PreciseContacts44(mode==1)
void SetHandSeparationDs1(bool on) noexcept; // DS1 free-hand self separation
bool HandSeparationDs1() noexcept;
void SetShoulderReachD38(float warningMeters) noexcept; // D39 diagnostic threshold; never limits controller authority
float GetShoulderReachD38() noexcept;
void SetHolsterPolicy136(float radius,bool releaseAnywhere) noexcept;
void SetLegacyHolsterLayout147(bool legacy) noexcept;
void SetBowReload146(bool manual,bool nativeGrab) noexcept;
void SetBowStringTrigger160(bool on) noexcept;
// V66.136R2: presentation-only haptic for a KCD2-native accepted collision hit.
// Never creates damage/contact and is ignored without a fresh physically owned weapon.
void NotifyNativeWeaponImpact136R2(float damage,float stamina,float sampleEnergy,bool blocked,bool perfectBlock,bool masterStrike,std::uint32_t targetId) noexcept;
int WeaponControlMode() noexcept;
void WeaponControlTick() noexcept;


// V66.89 F5 ON/OFF: exact actor/action/target request + successful name/ID starts.
// Observation only. Native lifecycle completion and physical gesture control are not
// fabricated from timers, pointer scans or unrelated last-seen item snapshots.
void SetMiscInteractionDiagMode(int mode) noexcept;
void CycleMiscInteractionDiagMode() noexcept;
int MiscInteractionDiagMode() noexcept;
// V66.107: F5 hand-use started on this hand -> use KCD2's own evaluated finger pose for it
// (like a held weapon) for a short window while the native interaction plays.
void NotifyNativeHandUse(int side,unsigned durationMs) noexcept;
void SetSupportSocket178r9(bool synthetic,float swordCm,float poleCm) noexcept; // V66.178R9 two-hand second-slot socket
void SetSupportSlot34(bool on) noexcept; bool SupportSlot34() noexcept; // C34b/C37 VrSupportFromWeaponSlot: slt_1 may replace the synthetic fallback (native frame wins)
void SetBowStringGlue178r9(bool enabled) noexcept;                                 // V66.178R9 string joint follows the string hand
bool HeldInteractionVisual43() noexcept; // exact current held-prop visual lease; no gameplay authority
bool HeldInteractionVisual43(int side) noexcept; // exact current native grasp on this hand only
void SetDialogueBody178r9(bool active) noexcept;                                   // V66.178R9 VRIK body in dialogue (pose only)
void PublishDialogueHandPresenceH24(const dialogue_state59c10r2::State&,bool presentation) noexcept;
void SetReadPageMemoMs(unsigned ms) noexcept;      // V66.178R7 INI VrReadPageMemoMs (0..150)
// V66.107: HMD eye height above the floor (OpenXR STAGE space), published every frame.
void SetEyeHeightAboveFloor(bool valid,float meters) noexcept;
void MiscInteractionDiagTick() noexcept;

// V65.48 lightweight F11 WUID/entity recorder. One press starts one read-only session;
// the next press stops it. Samples core XR/native transforms at 400 ms, WeaponSolver C/E,
// exact-name grip candidate IDs, game-state, directly validates playerModule+0x9D0 as
// C_ItemAttachmentManager, and logs only changed manager fields every 200 ms. Broad scans remain disabled.
// V167 plain F11: 0 baseline, 1 hand contact + boxing, 2 directional stab + native sweep.
// Shift+F11 retains the old reference recorder. Explicit OFF restores the leased modes.
int FocusedTestMode167() noexcept;
void CancelFocusedTests167(bool restore) noexcept;
ProbeStatus GetStatus() noexcept;

// V65.09 F5: reuse the exact WeaponSolver -> C_Player -> +0x38 CEntity
// ownership proof solely to identify Henry as a LookAt target. Observation only.
void SetPlayerIdentityObservationEnabled(bool enabled) noexcept;
bool BlockIndicatorGameplayH24(std::uint64_t now) noexcept; // cheap semantic owner/tracking/calibration gate
std::uint64_t PoseEpochH24() noexcept; // cheap current calibration/reanchor generation
enum class ThrowAuthorityT22 : unsigned char { Unknown, Allowed, Denied };
// Current native authority only: no pose/latest-publication read or lease renewal.
// Exact same-module installed MID observer witness; no native call or hook mutation.
bool NativeReloadHandRemoveEntryQualifiedT30(std::uintptr_t whBase) noexcept;
ThrowAuthorityT22 ThrowNativeAuthorityT22(int side,std::uintptr_t character,std::uint64_t body,
    std::uint64_t request,std::uint64_t tracking,std::uint64_t now) noexcept;
// New utility admission only. Original prepared/waiting fists retire themselves;
// this read-only getter never aborts them or gates native cleanup.
ThrowAuthorityT22 ThrowHandAdmissionT22(int side,std::uintptr_t player,std::uintptr_t character,
    std::uint64_t body,std::uint64_t request) noexcept;
// New admission only; original LEFT Draw/Stow debt remains until its native ACK.
ThrowAuthorityT22 ThrowEquipmentAdmissionT25(int side,std::uintptr_t player,std::uintptr_t manager,std::uintptr_t character,std::uint64_t body,std::uint64_t request) noexcept;
bool ReadThrowModelGripT18(void*,float (&)[7]) noexcept;
void* LastPlayerEntity() noexcept;
void* WeaponControlEntity(int side) noexcept;
// Exact Henry item-manager state: -1 UNKNOWN, 0 EMPTY, 1 OCCUPIED. UNKNOWN is never treated as empty.
int ExactHandItemState(int side) noexcept;
// C55: 1=current primary/support/string owner or retained held support intent ("occupied"), 0=known no physical
// owner, -1=unknown. Combine with ExactHandItemState before empty-hand gestures.
// Copied POD only; no additional native polling.
int ReadPhysicalHandHeld55(int side,std::uint64_t now) noexcept;
bool ReadHolsterGripOwnerR15(int side,std::uint64_t now) noexcept;
// T24 per-hand competing-owner exclusion; no native hand-key mutation.
bool HandClaimBlocksT24(int side) noexcept;
int UtilityLeftItem54() noexcept; // 0 exact empty, 11 exact held torch, -1 unavailable/reserved
int UtilityLeftItemR114(std::uint64_t now) noexcept; // R114: as 54, a torch seen held in the last 400 ms stays 11 while the copy is unknown
bool RequestTorch62(bool shoulderGesture,std::uint64_t tick) noexcept; // full filtered native callback; X does not require VRIK
bool RequestVisor54(bool close,std::uint64_t tick) noexcept; // native-frame one-shot, no inferred visor toggle state
unsigned PhysicalHandReservations114() noexcept; // bit0 right, bit1 left; stale = both
unsigned PhysicalHandReservationsPhysicsD41() noexcept; // copied POD/atomics only; no native equipment or late XR reads
int NativeHeadAimEquipment120R3() noexcept; // 0 empty/melee; 1 non-melee; -1 unknown/stale
// Cached committed VRIK endpoint only; no new native calls.
void StartNativeDoor19c4() noexcept;
bool StopNativeDoor19c4() noexcept; // retains update hook until native-thread cleanup
void TickNativeDoor19c4() noexcept; // exact Henry native-controller thread only
bool ReadInteractionHandWorld123R1(int side,float& x,float& y,float& z) noexcept;
// DS-F88: the narrow grab reader - committed world wrist + palm + epochs, no census gate.
bool ReadGrabHandWorld60(int side,float palm[3],float quat[4],std::uint64_t& poseEpoch,std::uint64_t& trackingEpoch,std::uintptr_t& player,std::uint64_t& tick) noexcept;
// D59: copied completed palms for opt-in misc gestures. No pose/physics writer.
struct MiscGestureBodyD59 {
    std::uintptr_t player=0,character=0;
    std::uint32_t playerId=0;
    std::uint64_t tick=0,body=0,tracking=0;
    float palm[2][3]{};bool valid[2]{};
};
bool ReadMiscGestureBodyD59(MiscGestureBodyD59&) noexcept;
// Palm centre and outward finger axis from the same committed skeleton packet.
void SetTouchReact43(bool on,float mass) noexcept; // C43T
void SetChestDagger54(bool enabled) noexcept; // C54 cosmetic actual carried retail dagger only; no physical draw
bool ChestDagger54() noexcept;
void SetLuaExecutor43(bool(*fn)(const char*) noexcept) noexcept; // C43D: game Lua via console ExecuteString (deferred)
void PublishVisibleFailure(const char* text) noexcept; // DS-F17: thread-safe publication (any thread), pure mailbox traffic
void DrainVisibleFailure(std::uint64_t now) noexcept;  // DS-F17: game-thread drain (NativeRhythmTick178) - the ONLY Lua exec
// DS-F20: constraint op drain (game thread). The executor is registered only
// when the native op path is proven; while absent, ops are dropped explicitly.
void DrainConstraintOps60(std::uint64_t now) noexcept;
void SetSmoothBody51(bool on) noexcept; // C51
// CL-B (Claude): INI VrChestPitchShare 0..1, default 0 (trial OFF) - looking down, the chest/abdomen bend with the head, the neck rides along.
inline std::atomic<float> g_chest_pitch_share_cb{0.f};
inline void SetChestPitchShareCB(float v) noexcept {g_chest_pitch_share_cb.store((v>=0.f&&v<=1.f)?v:0.f,std::memory_order_relaxed);}
inline float ChestPitchShareCB() noexcept {return g_chest_pitch_share_cb.load(std::memory_order_relaxed);}
// CL-C (Claude): INI VrArcherStance 1 = V66.127/128 archer stance (left shoulder to the target), 0 = 169R2 shoulder participation.
inline std::atomic<bool> g_archer_stance_cb{false};
inline void SetArcherStanceCB(bool on) noexcept {g_archer_stance_cb.store(on,std::memory_order_relaxed);}
inline bool ArcherStanceCB() noexcept {return g_archer_stance_cb.load(std::memory_order_relaxed);}
bool SmoothBody51() noexcept;
void SetHmdBodyHeight54(bool) noexcept;
void SetBodyHeightModeR98(int mode) noexcept; // R98: 0 HMD, 1 NATIVE, 2 HMD + NATIVE (ride/crouch)
int BodyHeightModeR98() noexcept;
void SetNativeCrouchR93(bool) noexcept;
bool NativeCrouchR93() noexcept;
bool HmdBodyHeight54() noexcept;
// C51R1: one exact completed head/controller/body frame; HMD/IPD are composed afterwards.
bool TrackedFrameCamera51(float,float,float,float&,float&,float&) noexcept;
// C55 joined ordinary frame; nonzero output heading is qualified bow-only root-relative neutral yaw.
bool TrackedFrameCamera55(float,float,float,float,float,float,float&,float&,float&,float&,float&,native_roomscale_d76::Origin* = nullptr) noexcept;
bool ReadCameraRoomscaleOriginD76(native_roomscale_d76::Origin&) noexcept;
bool ReadInteractionPalm59c9(int side,float& x,float& y,float& z,float& dx,float& dy,float& dz,std::uint64_t& sourceTick,
    std::uint64_t* sourceEpoch=nullptr,std::uintptr_t* sourcePlayer=nullptr) noexcept;
bool ReadWorkstationPalm67(int side,float& x,float& y,float& z,std::uint64_t& tick,
    std::uint64_t& poseEpoch,std::uint64_t& sourceEpoch,std::uintptr_t& playerEntity) noexcept;
bool ReadWorkstationPalmAimH21(int side,float& x,float& y,float& z,std::uint64_t& tick,
    std::uint64_t& poseEpoch,std::uint64_t& sourceEpoch,std::uintptr_t& playerEntity,float (&direction)[3]) noexcept;
// F5=1 recorded native StrokeHit join only. No contact or quality is generated.
bool ReadSmithContactHammer68(kcd2vr::subsystems::smithing::contact68::HammerFrame&,std::uint64_t now) noexcept;
std::uintptr_t FindSmithContactEntity68(std::uint32_t entity) noexcept;
void SetPhysBodies59c10r8(int mode,float handMass,float bladeMass) noexcept; // R59.16C10R8: 0 off, 1 follow, 2 collide
void TickPhysBodies59c10r8() noexcept; // R59.16C10R8: mod main loop, once per iteration
bool LegacyPhysBodiesRetiredT4() noexcept; // Atomic native cleanup acknowledgement.
// C7FM1: same committed/fresh epoch proof, complete LEFT wrist quaternion and world position.
// refusal: 0 usable, 1 pose mode, 2 reference/calibration, 3 native body,
// 4 nonempty/unknown left hand, 5 ranged, 6 packet/hand tracking, 7 age, 8 invalid pose.
bool ReadCorpseGrabCandidateCg1(kcd2vr::corpse_grip_cg1::Candidate& out,std::uint64_t now,bool& inputCurrent) noexcept;
bool ReadCorpseGrabCandidateSideCg1(int side,kcd2vr::corpse_grip_cg1::Candidate& out,std::uint64_t now,bool& inputCurrent) noexcept; // W5: 0 RIGHT 1 LEFT
// M39P (ChatGPT review source3 #3): the fold distance needs BOTH ends from ONE completed
// publication. The left reader reports the exact publication it used; the right reader
// refuses any other one (pairMismatch=true) instead of mixing two packets.
// CB1: Henry's current primary weapon point (tip, or the pommel/butt end), world, led ~45 ms along its last frame
// motion (physics frame <=120 ms old).
// CB4: the same measured striking point now and one physics frame earlier (world), for the stroke's contact line.
bool WeaponPathWorldCB4(bool pommel,float (&now)[3],float (&previous)[3]) noexcept;
struct HeldMapPairM39 {bool valid=false;std::uintptr_t actor=0;std::uint64_t epoch=0,sample=0,tick=0,publicationR24=0;float world[8]{};};
// One late committed publication for the RIGHT paper anchor and optional LEFT support.
// Fresh ownership is still rechecked immediately before/after native presentation.
struct HeldMapSnapshotD78R1 {
    float right[8]{},left[8]{},leftGrip=0;bool haveLeft=false;
    std::uintptr_t sourceEntity=0;std::uint64_t sourceEpoch=0,sourceTick=0;
    HeldMapPairM39 pair{};
};
bool ReadHeldMapSnapshotD78R1(HeldMapSnapshotD78R1&,bool includeLeft,unsigned* refusal=nullptr) noexcept;
bool ReadHeldMapFreeRightPoseM39(float (&out)[8],float& grip,const HeldMapPairM39* pair=nullptr,bool* pairMismatch=nullptr) noexcept;
bool ReadHeldMapLeftPoseM1(float (&out)[8],unsigned* refusal=nullptr,
    std::uintptr_t* sourceEntity=nullptr,std::uint64_t* sourceEpoch=nullptr,std::uint64_t* sourceTick=nullptr,
    HeldMapPairM39* pair=nullptr) noexcept;
// M41 static parchment: current empty RIGHT hand/socket, never borrowed LEFT.
bool ReadHeldMapRightPoseM41(float (&out)[8],unsigned* refusal=nullptr,
    std::uintptr_t* sourceEntity=nullptr,std::uint64_t* sourceEpoch=nullptr,std::uint64_t* sourceTick=nullptr,HeldMapPairM39* pair=nullptr) noexcept;
void SetInteractionHandContext158(int side,bool passive,float x,float y,float z,float maxReach) noexcept;
}

namespace kcd2vr::frik_pose_probe {
void SetHolsterCombatDiagMode(int mode) noexcept;
void CycleHolsterCombatDiagMode() noexcept;
int HolsterCombatDiagMode() noexcept;
void ObserveCollisionAnimationCharacter(void* character,std::uint32_t id,std::uint64_t contact_sequence) noexcept;
}

namespace kcd2vr::frik_pose_probe { bool ArmMotionCombatInput101() noexcept; }
namespace kcd2vr::part_namespace62 { struct Ticket; }
namespace kcd2vr::frik_pose_probe {
// Finite F3 read-only current-identity witness, called before native submission.
void ObserveHitPartNamespace62(const kcd2vr::part_namespace62::Ticket&) noexcept;
// Read-only SAME current skeleton-body/raw-part check. Caller separately proves
// current-life native death; this does not admit living impacts or successors.
bool GeometryCorpseBodyH7(std::uint32_t target,std::uintptr_t entity,std::uintptr_t collider,int rawPart,bool nativeDeathQualified) noexcept;
bool GeometryCorpseContactFaultH7() noexcept;
// DS-H22: the hand's corpse-push entry - the SAME proven weapon corpse-impulse
// chain (the geometry request -> the death ledger -> the articulated Action).
// Returns false with `why` when the chain refuses; never invents a body.
bool CorpseImpulseRequestH22(std::uint32_t entity,int part,std::uintptr_t collider,
    const float pos[3],const float normal[3],float speed,std::uint64_t sourceTick,
    std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,
    const char*& why) noexcept;
}
namespace kcd2vr::frik_pose_probe {
// Observation of the already transformed immersive eye camera. No engine reads/writes.
void ObserveImmersiveCamera122(float x,float y,float z,int eye,std::uint64_t xrFrame) noexcept;
void ObserveBowCamera126(float x,float y,float z,float hmdX,float hmdY,float hmdZ,int eye,std::uint64_t xrFrame) noexcept;
// C35: while a bow is drawn, cancel authored XYZ camera movement
// (aim animation) relative to Henry's body. Returns a world-space correction for the
// native camera base position (zero when not drawing / unknown).
bool BowCameraCorrection127(float baseX,float baseY,float baseZ,float& cx,float& cy,float& cz) noexcept;
// Existing completed physical input only; no native query. Unknown is not neutral.
bool BowCameraNeutral35(std::uint64_t now) noexcept;
}

namespace kcd2vr::frik_pose_probe {
// H11: -1 outside tracked boxing ownership, 0 no current palm contact, 1
// current native contact point overlaps the tracked palm sweep. No hit award.
int FistContactAdmissionH11(const void*,const float*,int) noexcept;
int FistContactAdmissionH12(const void*,const float*,int,const void*) noexcept;
// C47: copied current Henry world + calibrated anatomical eye; no native calls.
bool MotionOwnedCamera47(float,float,float,float,float,float,float&,float&,float&,float&,float&) noexcept;
}

namespace kcd2vr::frik_pose_probe {bool NativeHandPhysicsH24() noexcept;void SetNativeHandPhysicsH24(bool) noexcept;bool PrepareNativeHandPhysicsH24() noexcept;}
namespace kcd2vr::frik_pose_probe {
// Copied current empty-hand quick-talk input claim only; no native calls/pose writes.
void PublishQuickTalkOwnerH4(unsigned mask,std::uint64_t tick) noexcept;
}

namespace kcd2vr::frik_pose_probe {
// Same committed packet/actor/time as the primary right paper grip; no native writes.
bool ReadHeldMapSupportLeftD64(float (&out)[8],float& grip,const HeldMapPairM39& primary) noexcept;
}
