#pragma once
#include "../experiments/claude_bundle_d70.h"
#include "held_motion_d67.h"
#include "weapon_trials_d69.h"
#include "holster_body_d70.h"
#include "holster_body_d85.h"
#include "knife_pouch_d66.h"
#include "original_contact_wait_h8.h"
#include "../subsystems/masterstrike_combos/draw_motion_d63.h"
#define KCD2VR_PHYSICAL_DRAW_MOTION_D63 1
#include "../subsystems/physics/corpse_grip_cg1.h"
#include <atomic>
#include "thrust_origin167.h"
#include "kick_request5916c3.h"
#include "hilt_grip169r2.h"
#include "sweep_evidence169r2.h"
#include "passive_contact169r1.h"
// V66.98: active engine-independent physical weapon policy. All geometry is in
// Henry model space; no camera movement, health/damage writes or native ABI here.
#include "pose_math.h"
#include "melee_tuning_c54.h"
#include "two_hand_bias_d65.h"
#include "two_hand_weight_r90.h" // R90 option
#include "solver_command_h28.h"
#include "guard_parry176.h"
#include "gore_cut_d56.h"
#include "windup57c1.h"
#include "blade_zone_d84.h" // D84 blade zone
#include "../diagnostics/combat_state_zone_d85.h" // D85 zone (sent zone for the F3 ZONE row)
#include "contact_receipt58c1.h"
#include "gore32.h"
#include "decap_joint44.h" // C44: pure loose-neck part (layout, markers, parameter build), used by gore32.inl
#include "body_combo59c7g.h"
#include "weapon_dynamics177.h"
#include "swing_effort59c14.h"
#include "motion_time.h"
#include "tracked_frame51.h"
#include "melee_stroke118.h"
#include "equipment_gesture114.h"
#include "holster_diag_r8d.h" // R8d: pure F2 rung 1 holster attempt observer (no behaviour)
#include "quiver_gesture114.h"
#include "bow_arm_aim_r9.h" // R9 quiver arbitration: right hand = arrow hand while a bow is held
#include "../subsystems/throw_drop/hip_selection_t10.h"
#include "npc_contact172.h"
#include "dirblock39.h" // C39 DIRBLOCK approach sector (pure)
#include "feint_hybrid_fe.h" // D34 FE VrFeintHybrid (pure)
#include "impale40.h" // C40 IMPALE BY ARMOUR candidate thrust (pure)
#include "impale_pending59.h"
#include "impale47.h" // C47 stuck point on the blade for a killing thrust (pure)
#include "impale_corpse_r15.h" // I10/N2: stab into a game-killed corpse latches the same hang (pure)
#include <array>
#include <cstdint>
#include <algorithm>
#include <limits>
#include <cmath>
namespace kcd2vr::physical {
inline std::atomic<double> g_wall_friction57{.35}; // Runtime setting; Input stays explicit and defaults to geometric sliding.
inline std::atomic<bool> preciseContacts44{true}; // Immersion: bounded surface contacts, not rigid-body creation
// Saved contact modes:0=coarse palm,1=precise fingers,2=balanced palm+thumb/index.
// Balanced retains bounded world queries without collision-driven finger curls.
inline std::atomic<int> handsModeDs2{1};
inline std::atomic<bool> handSeparationDs1{true}; // DS1: INI VrHandSeparationDs1
// V66.178R6: step-time sweep coverage scale, set by the weapon kernel from its own dt
// (1 at <=16.6 ms, up to 4 at >=66 ms). At 1 the sweep is byte-for-byte the V169R2
// 3x4 mm-chord prefix; only slow frames buy a coarser, longer checked prefix.
inline std::atomic<double> g_sweep_coverage_scale178r6{1.0};

namespace m=pose_core;
inline constexpr std::uint64_t kKickPendingMs59c2=600; // R59.16C2: R3 waits this long for a wind-up/stroke/rhythm gate
using m::Vec3; using m::Quat;
struct Pose { Quat q{}; Vec3 p{}; };
// C60: the exact accepted main sweep, then optional checked tangent. The
// self-shield constraint may shorten these paths, never take a new world chord.
struct CheckedPath60 {
    std::array<Pose,4> point{};unsigned count=0;bool valid=false;
    void begin(Pose a,Pose b,bool checked) noexcept {point[0]=a;point[1]=b;count=2;valid=checked;}
    void append(Pose p) noexcept {if(count<point.size())point[count++]=p;else valid=false;}
};
inline Pose compose(Pose a,Pose b){return {m::multiply(a.q,b.q),a.p+m::rotate(a.q,b.p)};}
inline Pose inverse(Pose a){auto q=m::conjugate(a.q);return {q,m::rotate(q,a.p*-1)};}
inline Pose blend(Pose a,Pose b,double f){return {*m::slerp(a.q,b.q,f),a.p+(b.p-a.p)*f};}
inline Vec3 point(Pose a,Vec3 p){return a.p+m::rotate(a.q,p);}
inline bool finite(Pose p){
    const double norm=p.q.x*p.q.x+p.q.y*p.q.y+p.q.z*p.q.z+p.q.w*p.q.w;
    return m::finite(p.p)&&std::isfinite(norm)&&std::abs(norm-1.0)<.01;
}
inline Vec3 limited(Vec3 v,double n){double l=m::length(v);return l>n&&l>0?v*(n/l):v;}
inline double reach_max(double a,double b){return std::sqrt(a*a+b*b+2*a*b*std::cos(2*3.141592653589793/180));}
inline Vec3 reachable(Vec3 p,Vec3 shoulder,double maximum){return shoulder+limited(p-shoulder,maximum);}
// Contact carries only validated geometry. Scalar callbacks remain compatible
// but cannot invent a normal, material, target identity or an impact response.
struct Contact103 {
    double fraction=1;bool known=true,plane=false;Vec3 point{},normal{},feature{};
    // A proven clear prefix, with the remainder deferred by the ray cap.
    // This may advance presentation but never authorizes attack/contact intent.
    bool limited115=false;
    kcd2vr::sweep169r2::Evidence coverage169r2{};
    std::uint64_t witness169r1=0; // exact optional raw-ray record, never collision permission
    kcd2vr::npc_contact172::Witness native172{}; // copied synchronous owner evidence; independent of recording
    bool currentOpponent175=false; // independently revalidated exact native combat actor and ray owner
    bool targetPlacementD70=false; // complete current proxy only; NEVER native-to-target motion proof
};
inline Contact103 contact103(double fraction) {
    Contact103 c{};c.known=std::isfinite(fraction);c.fraction=c.known?std::clamp(fraction,0.0,1.0):0;return c;
}
inline Contact103 contact103(Contact103 c) {
    if(!c.known||!std::isfinite(c.fraction)){
        const auto evidence=c.coverage169r2;c={};c.coverage169r2=evidence;
        if(c.coverage169r2.reason==kcd2vr::sweep169r2::Reason::FullClear)c.coverage169r2.reason=kcd2vr::sweep169r2::Reason::InvalidQuery;
        c.known=false;c.fraction=0;return c;
    }
    c.fraction=std::clamp(c.fraction,0.0,1.0);const auto n=m::unit(c.normal);
    c.plane=c.plane&&bool(n)&&m::finite(c.point)&&m::finite(c.feature);if(c.plane)c.normal=*n;
    return c;
}
struct Attributes { double strength=10,agility=10,stamina=1;bool live=false;double strengthEffective=-1,agilityEffective=-1;int skills26[5]{-1,-1,-1,-1,-1}; }; // C26c: sword, heavy, large, dagger, unarmed (-1 = unread) // R59.16C10R17: effective levels (-1 = base only)
// R59.16C7B (Miki): an exhausted Henry still attacks and blocks, but the weapon handles 5x heavier.
// Full effect at <=2 % stamina, none at >=15 %, linear in between (no on/off jump). Handling only:
// inventory weight, damage inputs and the estimated physical body are unchanged.
inline double exhaustionScale59c7b(const Attributes& a) noexcept {
    if(!a.live||!std::isfinite(a.stamina))return 1.0;
    const double t=std::clamp((0.15-a.stamina)/(0.15-0.02),0.0,1.0);
    return 1.0+4.0*t;
}
struct Profile {
    std::uint64_t key=0,generation=0;
    std::uint32_t entity=0;int side=0,classId=-1;
    int holsterSetR12=-1; // current native equipment set; only selects the virtual grab zone
    kcd2vr::hilt169r2::Region supportRegion169r2{};
    unsigned bowGeometryRank48=0; // structural0, native partial-axis1, native full2
    bool bowNockQualified40=false; // current measured named-string rest point, not fallback grip
    double bowDrawLength166=0; // zero until exact current bow skeleton/attachment correlation
    double weight=1,extent=.7; // inventory weight is explicitly a tuning proxy, not measured rigid-body mass
    int strReq59c17=-1,agiReq59c17=-1; // R59.16C10R17: melee def +0x118 / +0x11C (the game's StrReq / AgiReq); -1 = unknown
    Pose grip[2]; // hands in weapon-local space, taken from native evaluated poses
    Vec3 tip{},axis{0,1,0};
    // Entity-local blade-face normal from a qualified visual-model helper,
    // independent of the round native weapon collision proxy.
    Vec3 guardFaceNormalH24{};bool guardFaceReadyH24=false;
    Vec3 diagnosticBoundsMin59c6{},diagnosticBoundsMax59c6{};bool diagnosticBounds59c6=false;
    Pose nativeWeapon{};bool nativeSeed=false;
    bool nativeRejoinSweepR28=false; // query-only copied annotation; never cached as item authority
    bool initialAcquireSweepD44=false; // query-only fresh binding burst; no new item/contact authority
    double rootTurnAngleD44=0; // query-only current same-owner root delta, never collision clearance
    bool primary=false,support=false,geometry=false,twoHanded=false;
    bool supportPositionOnly174r2=false; // synthesized socket has no opposite-wrist rotation proof
    bool supportSlot34=false; // C34b/C37: support frame taken from the weapon's slt_1 helper (only when no native frame was captured)
    unsigned char supportSource37=0; // C37: 0 none, 1 native animation frame, 2 weapon slot slt_1, 3 synthetic fallback (sliders)
    unsigned char slotCheck37=0;     // C37: 0 not evaluated, 1 measured+used, 2 right wrist does not reproduce slt_0, 3 slots closer than a hand, 4 slt_1 outside bounds, 5 measured but native frame wins
    bool supportV120_178r27=false;       // V66.178R27: socket built the V66.120/123 way (see physical_native PhysFillProfile)
    bool legacyHolsters147=false; // one mapping shared by zones, gestures and haptics
    bool shield176()const{return classId==8||classId==17;}
    bool bow()const{return classId==9;}
    bool crossbowD72()const{return classId==10||classId==14||classId==15;}
    bool torch40()const{return classId==11;} // native weapon_class torch, left hand; no attack type
    bool melee()const{return (classId>=1&&classId<=7&&classId!=6)||classId==16;}
};
// D72: a crossbow's measured whole-model bounds contain its transverse prod.
// Do not enable its new-binding correction from a class number or guessed
// grip-to-tip shaft alone. Coordinates have already received native item scale.
inline bool crossbow_bounds_d72(const Profile& p) noexcept {
    if(!p.crossbowD72()||!p.geometry||!p.diagnosticBounds59c6||p.side<0||p.side>1||
       !finite(p.grip[p.side])||!m::finite(p.diagnosticBoundsMin59c6)||!m::finite(p.diagnosticBoundsMax59c6))return false;
    const auto size=p.diagnosticBoundsMax59c6-p.diagnosticBoundsMin59c6;
    for(const auto span:{size.x,size.y,size.z})if(!(span>=.001&&span<=2.4))return false;
    return true;
}
inline bool initial_acquire_shape_d72(const Profile& p) noexcept {
    return p.melee()||p.bow()||p.torch40()||crossbow_bounds_d72(p);
}
// H24: measured whole-model planar proxy, not an inferred blade mesh normal.
// Native render-proxy bounds must expose an unambiguous thin transverse axis.
inline bool planar_face24(const Profile& p,Vec3& normal) noexcept {
    if(p.side<0||p.side>1)return false;
    const auto axis=m::unit(p.tip-p.grip[p.side].p);if(!axis)return false;
    if(p.guardFaceReadyH24){
        const auto face=m::unit(p.guardFaceNormalH24);
        if(!face||std::abs(m::dot(*face,*axis))>.20)return false;
        normal=*face;return true;
    }
    if(!p.diagnosticBounds59c6||!m::finite(p.diagnosticBoundsMin59c6)||!m::finite(p.diagnosticBoundsMax59c6))return false;
    const Vec3 size=p.diagnosticBoundsMax59c6-p.diagnosticBoundsMin59c6;
    const double d[3]{size.x,size.y,size.z},a[3]{std::abs(axis->x),std::abs(axis->y),std::abs(axis->z)};
    int along=0;if(a[1]>a[along])along=1;if(a[2]>a[along])along=2;
    if(a[along]<.95)return false;
    const int u=(along+1)%3,v=(along+2)%3,thin=d[u]<d[v]?u:v,wide=thin==u?v:u;
    if(!(d[thin]>.001&&d[wide]>=2.5*d[thin]&&d[along]>=2.*d[wide]))return false;
    normal={};if(thin==0)normal.x=1;else if(thin==1)normal.y=1;else normal.z=1;return true;
}
// Interpolate around the actual primary grip. Origin interpolation can push a
// nonzero local grip outside arm reach during a clear rotation, reset the state,
// and restart the same native seed forever. Sweeps and their consumer must agree.
inline Pose blend_weapon116(const Profile& p,Pose a,Pose b,double t){
    const auto grip=p.grip[p.side].p;
    const auto q=*m::slerp(a.q,b.q,t);
    const auto pivot=point(a,grip)+(point(b,grip)-point(a,grip))*t;
    return {q,pivot-m::rotate(q,grip)};
}
enum class BowDrawPolicy175 : unsigned char { Legacy, NativeTrigger, PhysicalString };
enum class BowLoadPolicy175 : unsigned char { NativeAutomatic, ManualQuiver };
// Immutable native nock attachment acknowledgment, F11=2 only. This is a
// one-use permission for a NEW Grip after a proven release-to-nock transfer;
// it is neither ammunition state nor a shot request or held-carry continuation.
struct BowNockAck19c4r4 {
    std::uint64_t serial=0,key=0,generation=0,request=0,body=0,sample=0,tick=0;
    // C56: this native transfer followed an actual open-Grip nock release.
    // The removed hand entity is immutable source identity, never dereferenced.
    std::uint64_t releaseSample56=0,releaseTick56=0,handIdentity56=0;
    std::uint32_t handEntity56=0;bool releaseToNock56=false;
};
struct Input {
    bool hitTraceC54R3=false; // F3 observation only; preserves already-computed pre-recognizer measurements.
    std::uint64_t epoch=0,sample=0,tick=0,bodyEpoch=0;
    kcd2vr::draw_motion_d63::Ticket drawMotionD63[2]{};bool drawHeldD63[2]{};
    Pose hands[2];Vec3 guardHead{};bool guardHeadKnown=false;Vec3 shoulders[2]{};double reach[2]{.65,.65};
    // Optional HMD-relative controller poses used ONLY for attack intent.  The
    // visible weapon still follows hands[] in Henry model space.  When present,
    // common room/body/HMD translation cannot masquerade as an arm swing.
    Pose intentHands[2];bool intentTracked[2]{};
    // Calibrated requested model wrist captured BEFORE anatomical reach caps.
    // Physics/visible arms continue to use hands[]. A published but invalid
    // source fails closed; legacy callers with no publication use hands[].
    Pose modelIntentHandsH30[2]{};bool modelIntentSourceH30[2]{},modelIntentTrackedH30[2]{};
    // Coordinate provenance only, never attack/contact permission. The ordinary
    // native frame is also present in the requested model wrists; its XY motion
    // must not subtract from the controller stroke measured between samples.
    tracked_frame51::Frame modelIntentFrameH8{};Quat modelIntentCalibrationH8{};
    double grip[2]{};bool tracked[2]{};bool allowed=false;
    bool dynamics=true,contact=true,attacks=true,bowActions=true; // explicitly gated by F12, defaults preserve old callers
    bool nativeRejoinR28[2]{}; // exact post-native completed-body receipt, consumed by first seed sweep only
    bool contactFaultH30=false; // circuit fault is hard loss; latency backoff is only contact uncertainty
    bool contactSuspended59c1=false; // native latency cooldown/fault, not a per-frame ray-budget miss
    Attributes attributes{};Pose world{};double worldScale=1;bool haveWorld=false;
    // V66.102 production options (defaults = old contract for older callers/tests)
    bool releasePresentation45=false; // bounded current-owner visual bridge while a sheath request awaits ACK
    bool autoSupport=false;
    bool supportV120_178r27=false; // V66.178R27: V66.120/123 two-hand handling (8 deg nudge, authored frame, no twist release)      // two-hand support latches by proximity+dwell, no grip button
    bool arrowFromShoulder=false;// right hand must take an arrow over the right shoulder before nocking
    bool sweepGate=false;        // bounded static-query reuse; world feature motion is checked
    std::uint64_t timeNs=0;       // producer monotonic timestamp, NEVER an eye-consumer timestamp
    // Held-body integration only. Existing achieved-motion consumers use its
    // resulting velocities; occurrence/freshness clocks remain timeNs/tick.
    std::int64_t poseTime[2]{};std::uint64_t poseTimeEpoch=0;
    std::uint64_t handPredictionPolicyD69=0; // actual extra-target basis, separate from tracking epoch
    bool bowRetention=false;     // independent of the optional quiver experiment
    bool heldMotionAttacks=false;// positively held item supplies intent eligibility; grip is not a second attack button
    double trigger[2]{};bool bowTrigger=false; // Bow uses RT only when selected by the production adapter.
    bool nativeBowTrigger126=false; // RT edges only; native game owns charge and release eligibility.
    bool requirePrimaryGrip114=false; // full F12 handling / F3=3; release cannot emit a final attack.
    bool quiverGesture114=false; // input ticket only; native charge acknowledgement required to fire
    bool bowRegrab116=false; // first native draw/retry needs no new quiver gesture; fire consumes the ticket
    bool bowNativeGrab146=false; // V66.146R1: native load (arrow appears in the real hand) starts at the quiver grab
    // V66.160: manual bow with TRIGGER on the string: Grip takes/carries the arrow,
    // Trigger holds the string (index finger), pull distance/angle is physical and
    // releasing the Trigger shoots. Grip mode (false) keeps V146R1 behavior.
    bool bowStringTrigger160=false;
    // Explicit production policy: unavailable geometry never changes who draws.
    BowDrawPolicy175 bowDrawPolicy175=BowDrawPolicy175::Legacy;
    BowLoadPolicy175 bowLoadPolicy175=BowLoadPolicy175::NativeAutomatic;
    bool bowGeometryReady175=false;
    BowNockAck19c4r4 manualNockAck19c4r4{};
    // V66.169R1: retained opt-in BOW-DISTANCE comparison only; UNKNOWN never moves/latches.
    bool experiments169=false;
    double quiverRadius146=.22; // V66.146R1: quiver zone radius over the right shoulder
    bool positionGuard176=false; // F4-only positional block; never a Trigger requirement
    double guardEnterMaxSpeed178r29=0; // V66.178R29: a block is ENTERED only below this blade-centre speed (m/s, 0 = no ceiling); holding is unaffected
    unsigned guardEnterDwellMs178r29=0; // V66.178R29: the posture must persist this long before the native block edge (0 = immediate)
    double guardExitSpeed178r45=0; // V66.178R45: a held guard releases above this blade-centre speed (m/s, 0 = position only) so a strike can start from guard
    unsigned guardAfterAttackMs178r49=400; // V66.178R49: no guard ENTRY this long after a committed stroke, and never while the blade is stopped in a living actor
    double statEffect59c17=0; // R59.16C10R17 VrWeaponStatEffect (0 = R16 handling, 1 = default, 1.5 max)
    int statSim26=0; // C26c SIMULATE HENRY: 0 real stats, 1 low, 2 medium, 3 high (feel only)
    bool guardStrict59c17=false;   // Optional stricter NEW entry. Held directional exit is independent (C19C4R1).
    unsigned guardFeintMs59c17=250; // R59.16C10R17: no guard ENTRY this long after a withdrawn/short/incoherent/timed-out candidate (150 ms after a bare wind-up sample)
    bool naturalCombat117=false; // one guard/attack owner, direction captured with the stroke
    bool coherentMelee119=false; // every live F12=6 uses deliberate intent + typed native input, independently of F3 visuals
    bool directionalStabs167=false; // F4 only: latch the physical origin as a native guard request
    kcd2vr::kick5916c3::Ticket kickTicket59c3{};
    std::uint64_t kickRequest59=0;  // V66.178R59: tick of the last R3 kick request (armed melee); the producer turns it into prepare(zone 3)+release
    bool windupHold57=false;        // V66.178R57 (coherent_pose sets it from INI VrWindupHold, default ON): a hand cocked beside/behind/above the shoulder and held still IS the native prepare (RT held) - the opponent sees the zone
    bool telegraphWindup59c10=true; // optional rearward preparation; forward onset remains independent
    bool earlyWarning33=false; // C35 compatibility hint only: a rearward observation cannot own DIRECT admission or its forward clock
    bool wallSlide58=false; // optional scrape/friction feel; checked static-world geometric response is independent
    bool wallBackoutSpringH31=false; // session experiment; checked extra outward offset only, OFF unchanged
    double wallFriction57=0; // Tangential displacement attenuation [0,.95]; invalid values preserve geometric sliding.
    bool thrustAsSwing55=false;     // V66.178R55 (coherent_pose sets it from INI VrThrustAsSwing, default ON in the game) (the user: "the stabs should be taken by the enemy as attacks, swings - only where it originates and where it goes matters"): a thrust is dispatched as the ordinary attack from its origin side, never as the native stab input
    bool controllerAnchor119=false; // free-space primary pivot/orientation track the hand; verified contact still constrains it
    bool constraintDrive129=false; // bounded acceleration/torque drive: near-1:1 normally, weight only under abrupt motion
    bool forwardOnly19=false; // no automatic backward-pommel or rear-facing thrust gesture
    bool pommelStrike69=false; // C71: allow the deliberate FORWARD end-first push inside the forward-only policies
    bool motionOwned59c5=false; // developer backend only; does not alter native comparison
    bool physicalCombat177=false; // F4 rung 2 only, same final pose and collision owner
    bool contactStartsAttack178r41=false; // legacy setting retained for fixture compatibility; contact never bypasses gesture qualification
    bool attackAtPrepare178r42=false; // early release only after full deliberate travel + quality, never merely PREPARE
    double weaponAuthorityScale178r44=1.0; // Scales bounded force/torque authority; two-hand support multiplies on top.
    bool twoHandWeightFeelR90=false; // R90 (R88 ranged) option VrTwoHandWeightFeelR90: VrWeaponWeightFeel also scales the two-hand hybrid rotation drive
    double fatigueRamp59c14=.6;   // R59.16C10R14: drag grows as stamina falls from full (0 = C7B floor only, 1 = x4 at empty)
    double poleOneHand59c14=.40;  // R59.16C10R14: one-handed polearm authority fraction (two hands restore it)
    bool intent59c16=false; // legacy R16 route selector; native hit semantics stay independent
    bool strikeMinScaleGate59c14=false; // R59.16C10R14: a stroke under VrStrikeMinScale is WEAK and does NOT commit (abort instead); INI VrStrikeMinScaleGate (default true), host default off = R59.13 semantics
    // R44C1: a held request/protected state can defer preparation; an ordinary
    // action's presence alone cannot. The optional extra mod interval defaults off.
    bool nativeAttackBusy178=false,nativeStateKnown178=false;double strikeMinScale178=.12;std::uint64_t strikeMinIntervalMs178=0;
    bool bowGripDraw47c2=false; // Normal Grip option or F11=2 override; native autoload remains.
    bool supportConstraint130=false; // secondary Grip constrains the same rigid weapon, never becomes a second independent writer
    // V66.178R15 impale latch (F4 rung 5): the pin point of an active latch in WORLD space, resolved on the
    // game thread from the struck entity's transform; stale/invalid = release.
    bool impaleHang32=false; // C32: the dead body hangs on the blade (game thread drives it, display is never pinned)
    bool impaleEnabled178r15=false,impalePinValid178r15=false;Vec3 impalePinWorld178r15{};std::uint64_t impalePinSerial178r19=0; // R19: the pin must belong to THIS latch
    bool impale40=false;int impalePinMode40=0;double impalePinDepth40=0; // C40 IMPALE BY ARMOUR: any undefended native thrust may latch; pin mode 1 STUCK (living, display keeps the blade in the wound, depth cap m), 2 HANG
    bool kickHoldImpale26=false,kickReleaseImpale26=false; // C26: an R3 kick on its way keeps the latch (max 2.5 s); at foot reach it frees the blade
    kcd2vr::contact58c1::Receipt receipt58c1{};
    kcd2vr::impale_corpse_r15::Candidate corpseStabR15{}; // I10/N2: weapon contact on a game-killed corpse (no native receipt exists for it)
    double supportMaxAngle=8.0*3.14159265358979323846/180.0;
    bool supportAxis59c16=false; // C19C3 explicit held grasp: sword hilt first; otherwise select shaft once, both hands steer (INI VrTwoHandAxisGrip)
    // C23 guard->strike handoff. Nonzero serial only while the exact-owner guard pump observes the LIVE native
    // master-strike count (Henry state+0x7D8 > 0) for the same mutually linked opponent within reach/ahead, with
    // the window's own item/generation/body/request copied at its OPEN edge. Zero = unchanged held-guard behaviour.
    bool masterHandoff23=false;double masterHandoffSpeed23=0; // VrMasterWindowHandoff / VrMasterHandoffForwardSpeed (default = VrGuardExitSpeed)
    std::uint64_t masterWindowSerial23=0,masterWindowKey23=0,masterWindowGeneration23=0,masterWindowBody23=0,masterWindowRequest23=0;
    // R38: shoulders[] are per-arm reach roots and may accommodate a tracked
    // endpoint. Body axes, guards and equipment zones use the pre-fit torso.
    // Appended/defaulted so older pure callers keep their existing body basis.
    Vec3 torsoShouldersR38[2]{};bool torsoShouldersKnownR38=false;
    // D85 holster: the PLAYER's shoulders (head pivot behind the headset eyes, holster_body_d85.h).
    // Only the over-shoulder / back acquisition zones read them; belt, hip and chest zones keep the torso anchors.
    Vec3 playerShouldersD85[2]{};bool playerShouldersKnownD85=false;
};
inline Vec3 body_shoulder_r38(const Input& in,int side) noexcept {
    return in.torsoShouldersKnownR38?in.torsoShouldersR38[side]:in.shoulders[side];
}
// D85 holster: shoulder anchor of the bow / shield / back zones. Unknown = the old torso anchor (callers without a headset frame).
inline Vec3 holster_shoulder_d85(const Input& in,int side) noexcept {
    return in.playerShouldersKnownD85?in.playerShouldersD85[side]:body_shoulder_r38(in,side);
}
inline Vec3 body_center_r38(const Input& in) noexcept {
    return (body_shoulder_r38(in,0)+body_shoulder_r38(in,1))*.5;
}
// Normal HYBRID intent follows its existing physical/forward policy directly.
// There is no separate enable flag, recorder dependency or opt-in to forget.
// The legacy R16 native contact route remains independently selected.
inline bool normal_hybrid_intent_h31(const Input& in) noexcept {
    return in.physicalCombat177&&in.coherentMelee119&&in.forwardOnly19&&!in.motionOwned59c5;
}
inline bool normal_intent_enabled_h30(const Input& in) noexcept {
    return normal_hybrid_intent_h31(in)||in.intent59c16;
}
inline bool model_intent_selected_h30(const Input& in,int side) noexcept {
    return side>=0&&side<2&&normal_intent_enabled_h30(in)&&in.coherentMelee119&&in.forwardOnly19&&!in.motionOwned59c5&&in.modelIntentSourceH30[side];
}
inline Pose model_intent_hand_h30(const Input& in,int side) noexcept {
    return model_intent_selected_h30(in,side)?in.modelIntentHandsH30[side]:in.hands[side];
}
inline bool model_intent_current_h30(const Input& in,int side) noexcept {
    return !model_intent_selected_h30(in,side)||(in.modelIntentTrackedH30[side]&&finite(in.modelIntentHandsH30[side]));
}
// Exact raw Draw source. Model XY is expressed relative to the same validated
// native anchor as ordinary intent, so root translation cannot count as pull.
inline kcd2vr::draw_motion_d63::Source draw_source_d63(const Input& in,int side,bool extracted=false) noexcept {
    kcd2vr::draw_motion_d63::Source source{};if(side<0||side>1)return source;
    const auto model=model_intent_hand_h30(in,side);const auto& frame=in.modelIntentFrameH8;
    source.sample=in.sample;source.tick=in.tick;source.timeNs=in.timeNs;source.tracking=in.poseTimeEpoch;
    source.body=in.bodyEpoch;source.request=in.epoch;source.side=side;
    source.hand=in.intentHands[side].p;source.modelHand=model.p;source.rotation=in.intentHands[side].q;
    source.modelRotation=model.q;source.calibration=in.modelIntentCalibrationH8;source.extracted=extracted;
    source.valid=in.allowed&&in.tracked[side]&&in.intentTracked[side]&&model_intent_current_h30(in,side);
    if(frame.valid){
        if(frame.stamp.sample!=in.sample||frame.stamp.tick!=in.tick||frame.stamp.epoch!=in.bodyEpoch||frame.bow||frame.acquisitionSerial){source.valid=false;return source;}
        source.anchored=true;source.frameActor=frame.stamp.actor;source.frameControl=frame.control;source.scale=frame.scale;
        source.modelHand.x-=frame.anchor.x;source.modelHand.y-=frame.anchor.y;
        source.modelHand.z-=tracked_frame51::shared_height_offset_r98(frame); // shared stance/native height (R98), never physical HMD Z
    }
    return source;
}
// C36 shared production policy, factored without changing its mode semantics.
// Current HYBRID and DIRECT configurations both enable physical177; do not
// infer mode tuples from historical fixture comments.
inline std::atomic<bool> pommelStrikeDefault69{true}; // C71 INI VrPommelStrike (default ON, Miki)
inline void select_forward_motion36(Input& in,bool directSelected,bool synchronized) noexcept {
    in.motionOwned59c5=in.physicalCombat177&&directSelected;
    in.forwardOnly19=in.physicalCombat177&&(in.motionOwned59c5||synchronized);
    in.pommelStrike69=in.forwardOnly19&&pommelStrikeDefault69.load(std::memory_order_relaxed);
}

inline bool manual_bow_geometry146(const Profile& p){
    return p.bow()&&p.support&&finite(p.grip[0])&&bool(m::unit(p.axis));
}
// The named resting string joint plus this bow's captured primary grip and
// actual long axis qualifies a structural draw direction before any full shot.
// No guessed joint, prior calibration shot, camera or controller axis is used.
inline bool structural_bow_axis175(const Profile& p,Vec3 nock,Vec3& axis) noexcept {
    if(!p.bow()||!p.primary||!p.geometry||!finite(p.grip[1])||!m::finite(nock))return false;
    const auto longitudinal=m::unit(p.tip-p.grip[1].p);if(!longitudinal)return false;
    Vec3 draw=nock-p.grip[1].p;const double span=m::length(draw);
    if(span<.045||span>.85)return false;
    draw=draw-*longitudinal*m::dot(draw,*longitudinal);
    if(m::length(draw)<.025)return false;
    const auto unit=m::unit(draw);if(!unit)return false;axis=*unit;return true;
}
inline bool physical_bow175(const Input& in) noexcept {
    return in.bowDrawPolicy175==BowDrawPolicy175::PhysicalString;
}
inline bool bow_uses_trigger175(const Input& in) noexcept {
    if(physical_bow175(in)&&in.bowGripDraw47c2)return false;
    return physical_bow175(in)||in.nativeBowTrigger126||in.bowStringTrigger160||
        (in.bowTrigger&&!in.arrowFromShoulder&&!in.quiverGesture114);
}
inline bool bow_requires_charge_ack175(const Input& in) noexcept {
    return physical_bow175(in)||in.bowStringTrigger160||in.quiverGesture114||in.bowTrigger;
}
// Called before coherent_pose fills in.tracked[]. Use the current transaction's
// valid[] arguments; neither zero-initialized output flags nor geometry choose RT.
inline void select_bow_policy175(Input& in,const Profile& bow,bool selectedPhysical,
        bool currentRight,bool currentLeft,bool manualReload=false,bool nativeGrab=false,bool gripDraw=false) noexcept {
    in.bowDrawPolicy175=selectedPhysical&&bow.bow()?BowDrawPolicy175::PhysicalString:BowDrawPolicy175::NativeTrigger;
    in.bowLoadPolicy175=manualReload&&!gripDraw?BowLoadPolicy175::ManualQuiver:BowLoadPolicy175::NativeAutomatic;
    const bool physical=physical_bow175(in);
    in.bowGeometryReady175=physical&&manual_bow_geometry146(bow)&&currentRight&&currentLeft;
    in.bowGripDraw47c2=physical&&gripDraw;
    in.quiverGesture114=physical&&manualReload&&!gripDraw;
    in.bowNativeGrab146=in.quiverGesture114&&nativeGrab;
    in.bowRegrab116=physical;
    in.bowStringTrigger160=physical;
    in.bowTrigger=in.nativeBowTrigger126=!physical;
    // Accepted hand-distance draw belongs to physical mode, not F2 diagnostics.
    in.experiments169=physical;
    if(physical&&!in.bowGeometryReady175)in.bowActions=false;
}
// Input admission only. Native target selection and hit eligibility remain in
// KCD2. A body label alone never substitutes for the live contact identity.
inline bool actor_approach32c1(const Contact103& c) noexcept {
    namespace n=kcd2vr::npc_contact172;
    const auto& w=c.native172;
    return c.known&&!c.limited115&&c.plane&&std::isfinite(c.fraction)&&c.fraction>=0&&c.fraction<1&&w.mask==n::BodiesAndWorld&&
        w.ownerRead&&w.owner&&w.collider&&w.entityId&&w.part>=0&&
        (w.body==n::Body::Living||w.body==n::Body::Articulated);
}
inline bool actor_contact175(const Contact103& c) noexcept {
    return c.currentOpponent175&&actor_approach32c1(c);
}
enum class CommandKind : unsigned char { None,Prepare,Slash,Thrust,Abort,BowBegin,BowFire,BowCancel,Draw,Holster,Pommel,Zone57,DropT1 };
// Zone57 (V66.178R57): re-announce the attack zone of a HELD preparation (the feint: the cocked hand moved to the other side).
// Consumed as a native zone call only; it never presses or releases anything.
struct Command { CommandKind kind{};int side=0,set=-1;std::uint64_t key=0,epoch=0,sample=0,tick=0;double fraction=0;Vec3 direction{};std::uint64_t bodyEpoch=0,generation=0;int zone117=-1;CommandKind stroke117=CommandKind::None; int originZone167=-1;Vec3 origin167{};double timingSpeed178r22c2=0;bool controlledHoldH26=false; bool held57=false; /* R57: prepare from a cocked, resting hand */ bool cutFeint62=false; /* same prepared cut, new purposeful translated segment */ std::uint64_t gesture59c3=0; bool recognizerAbort18=false; /* intentional classifier abandonment only; context loss stays hard */ double effortScale19=0.,effortMass19=1.1;bool effortSupport19=false,effortKnown19=false; bool kick59=false; kcd2vr::kick5916c3::Ticket kickTicket59c3{}; /* explicit companion-dependent native kick exception */ std::uint64_t masterHandoff23=0; /* C23: native window serial that released guard for THIS gesture; the consumer rechecks the live count */ std::uint64_t throwEpochH21=0,physicsEpochH21=0; kcd2vr::solver_command_h28::Stamp solverH28{}; std::uint64_t recoveryEpochH30=0; /* producer-stamped original semantic owner; never retag queued attacks after hard loss */ kcd2vr::corpse_grip_cg1::DrawOccurrence corpseClaimCg1{}; bool optionalBladeZoneD84=false; bool feintFE=false; /* D34 FE: post-commit HYBRID feint request; the consumer never cancels on refusal */ kcd2vr::draw_motion_d63::Source drawSourceD63{}; Vec3 aimDirWorldD65C{},aimUpWorldD65C{},aimNockWorldD65C{};bool aimWorldD65C=false; /* BowFire only, same-command world-space aim; native consumer requalifies */ };
enum class BowReason116 : unsigned char { None, Waiting, Acquired, Drawing, PrimaryReleased, Unreachable, HandUnavailable, Deviation, HardSeparation, ShortRelease, FireRequested, GeometryUnavailable };
// R155: a HELD, nocked physical draw rides through this much bowActions/bowGeometryReady175 dropout (ms of tracking
// ticks) while the Grip stays closed and both hands are still valid; longer is the ordinary HandUnavailable cancel.
inline constexpr std::uint64_t kBowDropoutMs155=200;
struct BowObservation116 { BowReason116 reason{};double gap=0,along=0,lateral=0,maxDraw=0;bool needsQuiver=false,rearm=false; };
// V66.178R30 (R28 review F02): one immutable completed-impact record (see Output::impactRecord178r30).
struct ImpactRecord178r30 {std::uintptr_t collider=0;std::uint64_t gesture59c3=0;std::uint64_t tick=0,key=0,generation=0,epoch=0,request=0,sample=0;double speed=0;std::uint32_t entityId=0;int body=0,part=-1,surfaceIdx=-1,kind=-1;int bodyMaterial=-1;Vec3 point{},normal{};bool valid=false;int feature59c7g=0;bool twoHands59c7g=false;
    double effortScale19=0,effortMass19=0;bool effortKnown19=false;int originZone47=-1;
    gore_cut_d56::Evidence goreCutD56{};
}; // C7G: copied struck feature, never inferred from the current later pose
// Same technique-useful energy model as the recognizer's commit. Capture the
// physical velocity BEFORE collision removes it; neither desired-controller
// pressure nor a later global strike can supply this contact's cost factor.
inline void impact_effort19(ImpactRecord178r30& impact,const Profile& p,int side,Pose pose,
        Vec3 linear,Vec3 angular,double scale,Vec3 feature={}) noexcept {
    if(side<0||side>1||!impact.gesture59c3||!finite(pose)||!m::finite(linear)||!m::finite(angular)||
       !std::isfinite(scale)||scale<=0||(impact.kind!=0&&impact.kind!=1))return;
    const Vec3 rTip=(point(pose,p.tip)-point(pose,p.grip[side].p))*scale;
    const auto axis=m::unit(rTip);if(!axis)return;
    const auto body=kcd2vr::dynamics177::body(p.classId,p.weight,m::length(rTip));
    const Vec3 v=linear*scale,vCom=v+m::cross(angular,*axis*body.com);
    const Vec3 vPerp=vCom-*axis*m::dot(vCom,*axis),wPerp=angular-*axis*m::dot(angular,*axis);
    const double forward=std::max(0.,m::dot(vCom,*axis));
    const double energy=impact.kind==1?.5*body.mass*forward*forward:
        .5*body.mass*m::dot(vPerp,vPerp)+.5*body.transverse*m::dot(wPerp,wPerp);
    const double reference=impact.kind==1?.5*body.mass*3.*3.:.5*body.gripInertia*12.*12.;
    if(!std::isfinite(energy)||!std::isfinite(reference)||reference<=1e-6||!std::isfinite(body.mass))return;
    impact.effortScale19=std::clamp(std::sqrt(std::max(0.,energy)/reference)*melee_tuning_c54::read(p.classId).sensitivityH29,0.,1.25);
    impact.effortMass19=body.mass;impact.effortKnown19=true;
    // Actual pre-collision feature velocity, never controller pressure or NPC approach.
    const Vec3 lever=(point(pose,feature)-point(pose,p.grip[side].p))*scale;
    const Vec3 atContact=v+m::cross(angular,lever);
    impact.goreCutD56.totalSpeed=m::length(atContact);
    impact.goreCutD56.transverseSpeed=m::length(atContact-*axis*m::dot(atContact,*axis));
}
struct Output {
    struct IntentOwnerH30 {std::uint64_t key=0,generation=0,bodyEpoch=0,request=0,sample=0,tick=0;bool current=false;};
    IntentOwnerH30 normalIntentH30[2]{};
    // An admitted stroke may retain ancestry through a bounded finite motion
    // outlier. This certificate grants neither command nor contact authority.
    struct IntentQuarantineH8 {IntentOwnerH30 owner{};std::uint64_t gesture=0,commit=0,since=0;
        unsigned failed=0;double sourceDt=0,modelTip=0,trackingTip=0,modelHand=0,trackingHand=0;
        std::uint64_t afterSample=0;};
    IntentQuarantineH8 intentQuarantineH8[2]{};
    struct ResetH29 {unsigned reason=0;std::uint64_t priorGesture=0,priorCommit=0;double seconds=0,positionJump=0,angleJump=0;bool retained=false;};
    ResetH29 resetH29[2]{};
    bool reachClampedR4[2]{}; // R4: weapon pulled back to Henry's reach this frame (swing kept)
    kcd2vr::solver_command_h28::Stamp solverH28{};
    struct SolverGestureH29 {std::uint64_t gesture=0,tick=0,onsetSample=0;Vec3 worldDirection{};int kind=0,origin=-1;} solverGestureH29{};
    bool solverOwnedT4[2]{}; // achieved solver pose; no local-ray melee authority
    Pose solverWorldT4{};std::uint64_t solverLeaseT4=0,solverWorldGenT4=0,solverBodyGenT4=0,solverSourceTickT4=0,solverSourceSampleT4=0;
    std::uint64_t solverTrackingEpochH24=0,solverCancelGenerationH24=0;
    CheckedPath60 checkedPath60[2];
    bool ownShieldBlocked60[2]{}; // local presentation stop; never native block/hit evidence
    struct TraceC54R3 {bool observed=false,attackIntent=false,continuous=false,coherent=false;
        double dt=0,modelTipSpeed=0,relativeTipSpeed=0,modelHandSpeed=0,relativeHandSpeed=0;};
    TraceC54R3 hitTraceC54R3{};
    // 1 profile/tracking, 2 primary release, 3 unreachable seed, 4 unreachable contact.
    // Presentation rejection only; never treated as collision/hit evidence.
    std::array<unsigned,2> releaseReason121{}, invalidMask55c1{};
    std::array<bool,2> handlingOnly55c1{};
    std::array<double,2> acquisitionReachError124{}; // unowned seed distance beyond hard reach
    Pose hands[2],weapons[2];bool owned[2]{},active[2]{},support[2]{},blocked[2]{},contactKnown[2]{true,true},sweepDeferred115[2]{};
    // The displayed endpoint may be a fully checked prefix of a large arc.
    // The remainder is still untested; only this applied prefix may feed F12=7.
    bool contactPrefixKnown126[2]{};
    bool bowHand169r2=false; // effective manual carry/string owner (including F2)
    bool bowHandlingOnlyR12=false; // current bow presentation, without checked contact/throw authority
    double bowFraction=0;bool bowHeld=false;double impactSpeed[2]{};std::uint64_t cueKey[2]{};double cueGap[2]{1000,1000};double cueRadiusD48[2]{};
    // V66.178R58 wall feel: scrape speed (m/s of grip slip along a known surface while blocked), and per-frame flags.
    double scrapeSpeed58[2]{};bool wallBlocked58[2]{},wallSlid58[2]{},wallBackout58[2]{},wallStuck58[2]{};
    bool wallScraped58[2]{}; // Fresh checked non-backout tangent travel >.5mm world; not a rigid-body/contact impulse.
    std::array<Command,8> commands{};unsigned commandCount=0;
    int kickDrop59c2=0;std::uint64_t kickDropTick59c2=0; // R59.16C2: copied from the right-hand state; consumer dedupes by tick
    // V66.178R15 impale latch: a new latch request (entity + tip world point) and the active state.
    std::uint64_t impactEventTick178r24[2]{};double impactEventSpeed178r24[2]{}; // V66.178R24 latched impact event (survives frame skipping; dedupe by tick)
    // V66.178R30 (R28 review F02): ONE immutable completed-impact record per side, captured at the impact sample
    // with the contact that produced it, the technique at that moment and the owner identity; re-published 250 ms.
    std::array<ImpactRecord178r30,2> impactRecord178r30{};
    bool impaleCorpseR15=false; // I10/N2: this latch came from a corpse stab (game-killed body, own contact), not a native receipt
    kcd2vr::impale_corpse_r15::Candidate impaleCorpseCandidateR15{}; // I10/N2: the admitted candidate, by value, with every (re)publication
    bool impaleLatchRequest=false,impaleActive=false,impaleReleased=false;std::uint32_t impaleEntity=0;std::uint64_t impaleSerial=0;Vec3 impaleTipWorld{};double impalePull=0;
    int impaleSide32=-1;Vec3 impaleLocal32{}; // C32: latch side and the stuck point in weapon-local model units
    int impaleRelease40=0; // C40: weapon-side release cause of a STUCK latch (impale40::Release; 8 = WEAPON_SIDE display/reach/sweep limit)
    // C47: the admitted killing-thrust receipt travels with the request (the game thread no longer re-checks an 80 ms old copy),
    // and a refused killing thrust says why (impale47::refusalName, sticky 500 ms so the game thread sees it once).
    kcd2vr::contact58c1::Receipt impaleHit47{};int impalePick47=-1;double impaleTipM47=-1,impaleDepth47=0;
    int impaleRefusal47=0,impaleRefusalSide47=-1,impaleRefusalReason47=0;std::uint64_t impaleRefusalSerial47=0;double impaleRefusalTipM47=-1;
    // V66.178R19 (review F02): DISPLAY-ONLY latch packet. weapons[]/hands[] stay the physical (collision-resolved,
    // unlatched) poses that feed the native sweep predictor, contact samples and the retained state; the pinned
    // pose lives here and is consumed only by the weapon placement writer and the hand IK goals.
    Pose displayWeapons[2],displayHands[2];bool displayLatched[2]{},displayRejoinR13[2]{};
    bool nativeRejoinSweepR28[2]{}; // observation of the actual qualified acquisition query
    bool initialAcquireSweepD44[2]{}; // distinct from post-native return; same bounded sweep policy
    bool acquisitionPresentationD48[2]{}; // tracked presentation only; checked native seed grants no hand endpoint
    BowObservation116 bow116{}; // copied with the committed frame; no logging on pose thread
    bool guard117=false;
    // R59.16C10 (Miki: "two-hand holding not possible outside combat"): per-frame support-latch probe of the primary
    // side's off hand. Observation only; the latch rules above are unchanged. reason: 0 latched, 1 no two-handed
    // profile/support socket, 2 other hand holds a profile (shield/torch/bow), 3 other hand untracked/unreachable,
    // 4 rearm pending (Grip must open below .20 first), 5 Grip below .55, 6 gap to the socket >= 14 cm, 7 twist /
    // region / reach rule, 8 dwell (<20 ms) or sweep blocked, 9 auto-support path.
    struct SupportProbe59c10 {int side=-1;unsigned char reason=0;bool twoHanded=false,otherKeyed=false,otherTracked=false,profileSupport=false,rearm=false,v120=false,positionOnly=false,axis59c16=false;double gap=0,grip=0,reachExcess=0,along59c16=0;
        // C39 GRIP: shaft-centre geometry of this frame's candidate (edge-only F3 receipt C39 SUPPORT_GRIP)
        unsigned char mode39=0;bool centre39=false,socketNear39=false;double perp39=1e3,tMin39=0,tMax39=0,perpLimit39=0,radius39=0,socketGap39=1e3;} supportProbe59c10{};
    bool guardBySide30c1[2]{}; // completed-pose decision; native transport must use this same entry
    std::uint64_t guardFeints59c17=0,guardForwardVetoes59c17=0; // R59.16C10R17: cumulative frames a NEW guard was refused (feint/wind-up, forward hand)
    bool strokeActive119=false;
    std::uint64_t primaryStrokeGesture19=0; // current live primary candidate; never a remembered contact
    melee118::Observation melee118{};
    kcd2vr::passive169r1::Motion contactMotion169r1[2]; // geometry-only, never damage
    Contact103 contact119[2]; // already-computed query result; no extra rays or material guesses
    kcd2vr::dynamics177::Impact impact177[2]; // design SI estimates, never native damage units
    // V66.178 strike qualification (design estimates; never native damage units).
    struct Strike178 {std::uint64_t serial=0,tick=0;int kind=0,zone=-1,reason=0;bool accepted=false;
        double tipSpeed=0,energy=0,reference=0,scale=0;} strike178{};
    // C23: one receipt per guard->strike handoff (same tick as the released guard and the recognizer sample).
    struct MasterHandoff23 {std::uint64_t serial=0,tick=0,sample=0;double forward=0,tipSpeed=0,threshold=0;int kind=0;bool heldGuard=false;} masterHandoff23{};
    // C39 DIRBLOCK: last confident approach sector (combat_zone.xml angle table) of the primary committed cut, this sample only.
    struct Arrival39 {std::uint64_t gesture=0,tick=0;int zone=-1,angle=-1;bool nearBoundary=false;} arrival39{};
    kcd2vr::hip_t10::Grant hipGrantT10{}; // same completed-pose selection for ordinary draw and stock
    void add(CommandKind kind,const Input& in,const Profile& p,double f=0,Vec3 d={},int zone=-1,CommandKind stroke=CommandKind::None,int originZone=-1,Vec3 origin={}){
        if(commandCount<commands.size())commands[commandCount++]={kind,p.side,-1,p.key,in.epoch,in.sample,in.tick,f,d,in.bodyEpoch,p.generation,zone,stroke,originZone,origin};
    }
};
// Display ownership is insufficient for combat. All consumers, including queued
// contacts, must exclude the direct-handling recovery frame.
inline bool attack_authority55c1(const Input& in,const Output& out,int side) noexcept {
    return side>=0&&side<2&&in.allowed&&in.attacks&&in.tracked[side]&&
                out.active[side]&&out.owned[side]&&(!out.solverOwnedT4[side]||
          (side==0&&!kcd2vr::solver_command_h28::empty(out.solverH28)&&out.solverH28.owner.key&&
           out.solverH28.owner.request==in.epoch&&out.solverH28.owner.epoch==in.bodyEpoch&&
           out.solverH28.sourceSample==out.solverSourceSampleT4&&out.solverH28.sourceTick==out.solverSourceTickT4&&
           out.solverH28.colliderLease==out.solverLeaseT4&&out.solverH28.worldGeneration==out.solverWorldGenT4&&
           out.solverH28.bodyGeneration==out.solverBodyGenT4&&out.solverH28.trackingEpoch==out.solverTrackingEpochH24&&
           out.solverH28.cancelGeneration==out.solverCancelGenerationH24&&
           kcd2vr::solver_command_h28::fresh(out.solverH28.sourceTick,in.tick)))&&!out.ownShieldBlocked60[side]&&
        !out.handlingOnly55c1[0]&&!out.handlingOnly55c1[1];
}
// Input/native command ownership is distinct from permission to publish a new
// collision. The explicit proof is stamped by the current normal recognizer only.
inline bool command_authority_h30(const Input& in,const Output& out,const Profile& p,int side) noexcept {
    if(side<0||side>=2)return false;
    if((in.contactFaultH30||!model_intent_current_h30(in,side))&&!out.solverOwnedT4[side])return false;
    const bool normalH30=normal_intent_enabled_h30(in)&&!in.motionOwned59c5&&!out.solverOwnedT4[side]&&
        p.primary&&p.melee()&&!p.shield176();
    // A known collision pose cannot restore a discontinuous raw source. Normal
    // intent must own this sample explicitly even when its display is healthy.
    if(!normalH30&&attack_authority55c1(in,out,side))return true;
    const auto& q=out.normalIntentH30[side];
    return q.current&&q.key==p.key&&q.generation==p.generation&&q.bodyEpoch==in.bodyEpoch&&
        q.request==in.epoch&&q.sample==in.sample&&q.tick==in.tick&&in.allowed&&in.attacks&&
        in.physicalCombat177&&in.coherentMelee119&&in.tracked[side]&&in.intentTracked[side]&&
        finite(in.hands[side])&&finite(model_intent_hand_h30(in,side))&&finite(in.intentHands[side])&&p.primary&&p.side==side&&p.melee()&&
        !p.shield176()&&!in.contactFaultH30&&!out.solverOwnedT4[side]&&!out.ownShieldBlocked60[0]&&!out.ownShieldBlocked60[1];
}
inline bool quarantined_intent_h8(const Input& in,const Output& out,const Profile& p,int side) noexcept {
    if(side<0||side>=2)return false;
    const auto& q=out.intentQuarantineH8[side];const auto& o=q.owner;
    return q.gesture&&q.commit&&q.since&&q.failed&&o.current&&o.key==p.key&&o.generation==p.generation&&
        o.bodyEpoch==in.bodyEpoch&&o.request==in.epoch&&o.sample==in.sample&&o.tick==in.tick&&
        in.tick>=q.since&&in.tick-q.since<=80&&normal_hybrid_intent_h31(in)&&
        in.allowed&&in.attacks&&in.physicalCombat177&&in.coherentMelee119&&in.tracked[side]&&in.intentTracked[side]&&
        finite(in.hands[side])&&finite(model_intent_hand_h30(in,side))&&finite(in.intentHands[side])&&
        model_intent_current_h30(in,side)&&p.primary&&p.side==side&&p.melee()&&!p.shield176()&&
        !in.contactFaultH30&&!out.solverOwnedT4[side]&&!out.ownShieldBlocked60[0]&&!out.ownShieldBlocked60[1];
}
inline bool pre_quarantine_contact_h8(const Input& in,const Output& out,const Profile& p,
        const kcd2vr::original_contact_h8::Source& original) noexcept {
    if(!quarantined_intent_h8(in,out,p,0))return false;
    const auto& q=out.intentQuarantineH8[0];
    return original.collider&&original.sample&&original.tick&&q.afterSample&&original.gesture==q.gesture&&
        original.key==q.owner.key&&original.generation==q.owner.generation&&
        original.body==q.owner.bodyEpoch&&original.request==q.owner.request&&
        original.sample<=q.afterSample&&original.tick<=q.since;
}
// A previously queued input can wait for a readable current motion sample; its
// original 100ms deadline and recovery identity are checked by the consumer.
inline bool quarantined_command_h8(const Input& in,const Output& out,const Profile& p,const Command& c,std::uint64_t now) noexcept {
    if(c.side<0||c.side>=2||!quarantined_intent_h8(in,out,p,c.side)||c.kick59||c.held57||
       !(c.kind==CommandKind::Prepare||c.kind==CommandKind::Slash||c.kind==CommandKind::Thrust))return false;
    const auto& q=out.intentQuarantineH8[c.side];
    return c.gesture59c3==q.gesture&&c.key==q.owner.key&&c.generation==q.owner.generation&&
        c.bodyEpoch==q.owner.bodyEpoch&&c.epoch==q.owner.request&&c.sample<=in.sample&&
        c.tick<=in.tick&&now>=c.tick&&now-c.tick<=100;
}
struct WeaponState {
    Pose solverModelH28{},solverRelativeH28{};std::uint64_t solverTickH28=0;
    Quat lastTrackingRotationH29{};
    // Raw source poses let a native grip/mesh-basis refresh re-express the old
    // sample without treating that profile edit as physical weapon motion.
    Pose sourceModelH30{},sourceTrackingH30{},sourceOtherModelH30{},sourceOtherTrackingH30{},sourceGripH30{};
    Vec3 sourceTipH30{};bool sourcePoseH30=false;
    tracked_frame51::Frame stanceFrameR93{}; // prior current-source stance; separate from recognizer admission
    tracked_frame51::Frame sourceModelFrameH8{};Quat sourceModelCalibrationH8{};
    std::uint64_t gesture59c3=0;
    // A committed command outlives the next provisional recognizer candidate.
    // This is source identity only: the consumer still requires its native cost,
    // current attack/target and one-hit ledger. Contacts never renew this lease.
    struct CommittedGestureH26 {std::uint64_t gesture=0,tick=0,key=0,generation=0,epoch=0,request=0;
        Vec3 modelDirection{},trackingDirection{};int kind=0,origin=-1;
        std::uint64_t forwardTickH8=0,forwardSampleH8=0;
        std::uint64_t uncertainSinceH8=0,uncertainNsH8=0,uncertainSampleH8=0,uncertainAfterSampleH8=0;
        unsigned uncertainFailedH8=0;double goreNetTravelD56=0,goreFastTravelD56=0;} committedGestureH26{};
    Vec3 solverCommitWorldH29{};
    std::uint64_t lastCorpseStabR15=0;bool impaleCorpseR15=false; // I10/N2
    kcd2vr::impale_corpse_r15::Candidate impaleCorpseCandidateR15{}; // I10/N2: request-owned copy (not refreshed)
    std::uint64_t lastImpaleReceipt58c1=0;double hangSag44=0; // C44: 0..1 ramp of the hanging-body sag on the weapon target
    kcd2vr::contact58c1::Receipt impaleHit47{};int impalePick47=-1;double impaleTipM47=-1,impaleDepth47=0; // C47
    std::uint64_t refusedSerial47=0,refusalTick47=0;int refusal47=0,refusalReason47=0;double refusalTipM47=-1; // C47
    bool impale178r15=false,impalePinned178r18=false;std::uint64_t impaleSince178r15=0,impaleSerial178r15=0;std::uint32_t impaleEntity178r15=0;Vec3 impaleTipWorld178r18{};Vec3 impaleLocal32{}; // V66.178R15/R18, C32
    std::uint64_t key=0,generation=0,lastTick=0,lastSample=0,quietSince=0,lastAttack=0;
    std::uint64_t handPredictionPolicyD69=0;
    bool rigidGripD69=false; // sampled on fresh source only; no cached future pose
    Pose weapon{},lastDesired{};Vec3 angularVelocity{},linearVelocity{};std::uint64_t prepareTick=0;
    Pose primaryDesiredH42{};bool primaryDesiredReadyH42=false; // current-owner pre-support request, not motor/contact output
    std::uint64_t releaseVisualSince45=0;
    bool ready=false,support=false,supportRearm=false,gripRearm=true,prepared=false,spent=false,released44c1=false,waitingPrepare44c1=false;
    kcd2vr::kick5916c3::Ticket kickTicket59c3{};
    std::uint64_t kickSeen59=0,kickRelease59=0; // V66.178R59 R3 kick: request consumed / release pending (next frame)
    int kickDrop59c2=0;std::uint64_t kickDropTick59c2=0; // R59.16C2: last expired kick (1 held wind-up, 2 stroke, 3 rhythm, 4 command buffer, 5 stale)
    bool acquiring124=false; // checked seed progress only; no collision/native-combat ownership yet
    bool acquisitionPresentationD48=false; // new binding: display current grip while checked seed catches up
    bool displayRejoinR13=false; // current handling already shown; checked seed catches up independently
    std::uint64_t lastBowRelease166=0,manualNockSeen19c4r4=0;
    bool nativeBowMode146=false,bowGripMode47c2=false;
    guard_parry176::ThrustExit49 guardThrust49{};std::uint64_t guardAttackUntil49=0;
    std::uint64_t guardExitTick32=0; // current item only: postguard HYBRID onset uses normal preparation evidence
    std::uint64_t guardFeintUntil59c17=0; // R59.16C10R17: tick until which a NEW guard cannot be entered (feint / wind-up)
    std::uint64_t guardFeints59c17=0,guardForwardVetoes59c17=0; // receipts
    bool bowHeld=false,bowRearm=true;double travel=0,thrustTravel=0,bowFraction=0;
    Vec3 lastTip{},nock{},drawAxis{},bowAcquireOffset121{};double maxDraw=.55;
    unsigned bowGeometryRank48=0;
    bool bowNockQualified43=false; // measured rest geometry may arrive after first latch
    bool bowGrip38=false;Quat bowHandLocal38{},bowCorrection38{};double bowPull38=0;
    // C63 charge starts at zero at the actual latch, independently of the
    // geometric hand/string placement. A fixed acquisition offset, wrist-only
    // reorientation or common rigid hand movement cannot supply shot charge.
    double bowPullRest63=0;bool bowPullReady63=false,bowPullRebase63=false;
    // V66.102
    Vec3 recoil{},recoilVelocity{};          // model-space contact rebound (decays back to the hand)
    std::uint64_t supportCandidateSince=0,arrowSince=0,lateralSince=0;
    std::uint64_t bowDropoutSince155=0; // R155: first tick of the current readiness dropout of a held draw, 0 = none
    // C10R2: only a distance-only release may rearm on a deliberate return.
    std::uint64_t supportRearmReturnSince59c10=0;bool supportDistanceRelease59c10=false;
    double supportSlide=0;                   // polearm support position along the weapon axis (m)
    bool arrowInHand=false;
    Pose supportGrip169r2{},supportPrimary19c3{};Vec3 supportCandidate169r2{};
    bool supportGripReady169r2=false,supportCandidateReady169r2=false;
    bool supportPositionOnly175=false,supportRegionBound175=false;
    bool supportAxis59c16=false; // R59.16C10R16: the current latch is an axis (sliding) grip
    bool quiverSpent116=false;
    bool carry146=false;std::uint64_t carrySince146=0;
    quiver114::State quiver{};
    Pose lastSwept{};bool sweptReady=false,lastClear=false;
    std::uint64_t timeNs=0,lastSweepTick=0,supportTwistSince=0,lastImpactTick=0,lastStrike178=0,strikeSerial178=0;
    std::int64_t poseTime=0;std::uint64_t poseTimeEpoch=0; // belongs to lastDesired, not unrelated hand samples

    bool weakStrike178=false; // R59.13: last stroke was light (small energy), still a telegraphed attack
    std::uint64_t wallStuckSince58=0; // V66.178R58: first tick of a zero-progress contact
    double lastImpactSpeed178r24=0;std::uint64_t contactAttackCount178r41=0;
    ImpactRecord178r30 impactRecord178r30{}; // V66.178R30
    std::uint64_t unpublishedImpactTickH8=0; // suppress an uncertain semantic event, never the physical recoil clock
    // V66.178R5: last FREE-FLIGHT body velocity (tick-stamped). Contact resolution zeroes the body
    // velocity before the strike is measured, so a stroke committed while touching the enemy must
    // be scored from the approach, not from the stopped blade.
    Vec3 freeLinear178r5{},freeAngular178r5{};std::uint64_t freeTick178r5=0;
    bool catchUp178r8=false; // previous sweep was a clear prefix: the blade is behind the hand and must catch up
    Quat supportCorrection{};
    bool supportReleaseD67=false;Quat supportReleaseOffsetD67{}; // live one-hand basis handover; never attack input
    bool wasBlocked=false,contactRejoinR9=false;Vec3 angularRecoil{},angularRecoilVelocity{};
    Vec3 intentTip{},intentHand{},relativeIntentTip{},relativeIntentHand{},lastStrokeDirection{};
    bool intentReady=false,relativeIntentReady=false;unsigned intentSamples=0;
    Vec3 supportIntentHand63{},supportRelativeIntentHand63{};bool supportIntent63=false;
    double supportPrimaryBiasD65=0; // one fresh-source policy for visible/model/relative steering
    double reversalTravel=0;CommandKind lastStrokeKind=CommandKind::None;
    bool guard117=false;int zone117=-1;CommandKind stroke117=CommandKind::None;
    kcd2vr::guard_parry176::Entry guardEntry30c1{};
    Vec3 origin167{};int originZone167=-1;
    struct Cocked57 {bool active=false;std::uint64_t since=0,lastZoneTick=0;int zone=-1;windup57c1::Arm arm{};bool windup59c10=false,controlledH26=false,feintH26=false;Vec3 originH26{};} cocked57{};bool holdRearmH26=false; // V66.178R57 wind-up hold
    kcd2vr::feint_fe::Track feintFE{}; // D34 FE: live sector of the committed cut
    melee118::State stroke118{};bool natural118=false;
    // C36: observation-only ordinary stroke while positional guard still owns
    // the blade. Native Prepare is withheld until NORMAL preparation evidence.
    melee118::State guardStroke36{};Vec3 guardOrigin36{};bool guardDirect36=false;
    std::uint64_t masterHandoffSerial23=0,masterHandoffGesture23=0; // C23: last native window serial spent by a handoff, and the gesture it started
};
// R59.16C10R16 two-handed AXIS grip (Miki 15 Oct: "two handed grip is still not working outside the combat and inside the combat
// it is very bad ... it was a mistake to make the right dominant"). The 14 Oct log: 174 SUPPORT_LATCH lines, 0 latched - the left
// hand closed 26-29 cm from the computed socket (the V120 socket lies 7.5-12 cm toward the BLADE, the hand goes below the right
// hand toward the pommel), and outside a fight there is no native off-hand frame at all. Now the off hand latches WHERE IT CLOSES:
// its position is projected onto the weapon's long axis (grip -> tip), anywhere from just past the pommel to 85 % of the length
// (half-sword, halberd shaft), within 10 cm of the axis. C19C3 retains that chosen local grasp while Grip is held;
// a qualified sword hilt takes priority. Both hands steer one weapon and the primary wrist keeps the roll.
// Socket-less shaft acquisition remains available outside combat; it is not native grip/handle metadata.
// C39 GRIP (Miki 17 Oct: "the left hand was not gripping" the halberd, "the same problem as with the longsword"). The R16 axis
// ran through the RIGHT WRIST, not through the shaft: a wrist sits 6-11 cm beside the wood (17 Oct halberd: native right wrist
// 10.9 cm, native LEFT wrist 6.1 cm from the bounds centre line), so the game's own left-hand frame was 15.0 cm off that line and
// failed the 10 cm gate (21 GAP_OVER refusals with Grip held, 10.2-23.5 cm), and a latch snapped the left wrist onto the right
// wrist's side of the shaft - the closed fist hung beside the pole. Now the line is the weapon's bounds centre line (longest
// box axis, through the primary grip's station), the hand is measured wrist-to-centre, and the latched wrist keeps the side the
// player closed on at the wrist radius the native frames show (native left frame when captured, else the primary wrist).
// Without valid bounds the R16 line is used unchanged.
// Upgrade a provisional first-draw seed only when its measurement rank increases.
// No input edge, native charge, fraction, reload or release state is changed.
inline bool upgrade_bow_nock43(WeaponState& s,const Profile& p) noexcept {
    if(!s.ready||!s.bowHeld||s.carry146||(s.bowNockQualified43&&p.bowGeometryRank48<=s.bowGeometryRank48)||!p.bowNockQualified40||
       !p.bow()||s.key!=p.key||s.generation!=p.generation||!m::finite(p.grip[0].p))return false;
    const auto axis=m::unit(p.axis);if(!axis)return false;
    s.nock=p.grip[0].p;s.drawAxis=*axis;
    s.bowAcquireOffset121=*axis*m::dot(s.bowAcquireOffset121,*axis);
    if(s.bowNockQualified43){s.bowGrip38=false;s.bowCorrection38={};} // reacquire wrist attitude against the refined current root
    s.bowGeometryRank48=p.bowGeometryRank48;
    if(p.bowGeometryRank48==2&&p.bowDrawLength166>=.12&&p.bowDrawLength166<=.95)s.maxDraw=p.bowDrawLength166;
    s.bowNockQualified43=true;s.bowPullRebase63=true;return true;
}
struct ShaftLine39 {bool valid=false;Vec3 o{},u{};double butt=0,len=0;};
inline ShaftLine39 shaft_line39(const Profile& p) noexcept {
    ShaftLine39 s{};if(!p.diagnosticBounds59c6||!finite(p.grip[p.side])||!m::finite(p.tip))return s;
    const Vec3 lo=p.diagnosticBoundsMin59c6,hi=p.diagnosticBoundsMax59c6;const double e[3]{hi.x-lo.x,hi.y-lo.y,hi.z-lo.z};
    int k=0;for(int j=1;j<3;++j)if(e[j]>e[k])k=j;
    Vec3 u{};if(k==0)u.x=1;else if(k==1)u.y=1;else u.z=1;
    const Vec3 c{(lo.x+hi.x)*.5,(lo.y+hi.y)*.5,(lo.z+hi.z)*.5},g=p.grip[p.side].p;
    if(m::dot(p.tip-g,u)<0)u=u*-1.0;
    const double station=m::dot(g-c,u),half=e[k]*.5;
    s.o=c+u*station;s.u=u;s.butt=half+station;s.len=half-station;
    // the primary wrist must actually hold this line (a wrist is never 15 cm from the wood it grips)
    s.valid=std::isfinite(station)&&s.len>.10&&s.butt>=-.02&&m::length(g-s.o)<.15&&m::finite(s.o);return s;
}
// D72: classify the ACTUAL collision feature in the same immutable impact.
// A measured model rear end is a pommel/handle; the gesture label alone never is.
inline bodycombo59c7g::Feature impact_feature_d72(const Profile& p,int side,Vec3 feature,double scale) noexcept {
    namespace bc=bodycombo59c7g;
    if(side<0||side>1||p.side!=side||!p.geometry||!m::finite(feature)||!std::isfinite(scale)||scale<=0||
       !finite(p.grip[side])||!m::finite(p.tip))return bc::Feature::Unknown;
    const bool sword=p.classId==1||p.classId==2||p.classId==4||p.classId==16;
    const auto shaft=shaft_line39(p);
    if(shaft.valid&&shaft.butt*scale>.035&&shaft.len*scale>.15&&
       m::length(feature-(shaft.o-shaft.u*shaft.butt))*scale<=.06)
        return sword?bc::Feature::Pommel:(p.classId==3||p.classId==5||p.classId==7)?bc::Feature::Handle:bc::Feature::Unknown;
    if(!sword)return bc::Feature::Unknown; // forward axe/mace proxy cannot certify its head/tip surface
    const double length=m::length(p.tip-p.grip[side].p);
    const auto axis=m::unit(p.tip-p.grip[side].p);if(!axis||length*scale<.1)return bc::Feature::Unknown;
    if(m::length(feature-p.tip)*scale<=.06)return bc::Feature::Tip;
    const double along=m::dot(feature-p.grip[side].p,*axis);
    return along>length*.25&&along<=length?bc::Feature::Blade:bc::Feature::Unknown;
}
struct AxisGrip59c16 {bool eligible=false;double t=0,tc=0,perp=1e3,tMin=0,tMax=0;Vec3 local{};
    bool centre39=false;double radius39=0,perpLimit39=.10;Vec3 base39{},u39{};};
inline AxisGrip59c16 axis_grip59c16(const Input& in,const Profile& p,const Pose& weapon,Vec3 hand,double side=0) noexcept {
    AxisGrip59c16 a{};
    if(!in.supportAxis59c16||in.autoSupport||!p.twoHanded||!p.melee()||p.bow()||p.shield176()||!p.geometry||
       !finite(p.grip[p.side])||!m::finite(hand)||!finite(weapon))return a;
    // C39: a qualified sword wrist frame still wins - the caller prefers it whenever the hand closes within 14 cm of it;
    // the axis only answers a grasp elsewhere on the hilt/blade (it was disabled for every socketed sword before).
    const Vec3 g=p.grip[p.side].p;const auto ax=m::unit(p.tip-g);if(!ax)return a;
    double len=m::length(p.tip-g);Vec3 base=g,u=*ax;
    const auto line=shaft_line39(p);
    if(line.valid){
        base=line.o;u=line.u;len=line.len;a.centre39=true;a.perpLimit39=.15;
        a.tMin=-std::clamp(line.butt+.02,.12,1.0); // down to the butt / pommel end (a halberd's left hand sits up to 60 cm below)
        const int other=1-p.side;
        const auto radial=[&](Vec3 v){const Vec3 d=v-base;return m::length(d-u*m::dot(d,u));};
        double r=radial(g);if(p.support&&!p.supportPositionOnly174r2&&finite(p.grip[other]))r=radial(p.grip[other].p);
        a.radius39=std::clamp(std::isfinite(r)?r:.07,.03,.11);
    }else{
        const double extent=std::isfinite(p.extent)&&p.extent>len?p.extent:len;
        a.tMin=-std::clamp(extent-len+.06,.12,.45);
    }
    a.tMax=std::max(.10,len*.85);a.base39=base;a.u39=u;
    const Vec3 local=point(inverse(weapon),hand)-base;
    a.t=m::dot(local,u);const Vec3 radialV=local-u*a.t;a.perp=m::length(radialV);
    a.tc=std::clamp(a.t,a.tMin,a.tMax);
    // The two grips must stay apart for the axis to be defined; keep the side the hand latched on.
    if(std::abs(a.tc)<.06)a.tc=(side>0||(side==0&&a.tc>0))?.06:-.06;
    a.local=base+u*a.tc;
    if(a.centre39&&a.perp>1e-4)a.local=a.local+radialV*(a.radius39/a.perp); // the wrist on the side the player closed, palm on the wood
    a.eligible=std::isfinite(a.t)&&std::isfinite(a.perp)&&m::finite(a.local);return a;
}
inline bool axis_grip_near59c16(const AxisGrip59c16& a) noexcept {
    return a.eligible&&a.perp<a.perpLimit39&&a.t>=a.tMin-.05&&a.t<=a.tMax+.05&&std::abs(a.t)>=.06;
}
// R59.10C1: a collision reacquisition is not a new hand grasp. Carry only
// the proven weapon-local grip, never motion/impact/attack history. No stale
// tracking lease: this is evaluated against both CURRENT tracked hands.
struct SupportContinuity5910c1 {
    bool keep=false,rearm=false,positionOnly=false,regionBound=false,axis59c16=false;
    bool distanceRelease=false;std::uint64_t returnSince=0;
    Pose local{},primary{};Quat correction{};std::uint64_t twistSince=0;
    void restore(WeaponState& s) const noexcept {
        s.support=keep;s.supportRearm=rearm;s.supportGripReady169r2=keep;
        s.supportDistanceRelease59c10=distanceRelease;s.supportRearmReturnSince59c10=returnSince;
        if(keep){s.supportGrip169r2=local;s.supportPrimary19c3=primary;s.supportCorrection=correction;
            s.supportPositionOnly175=positionOnly;s.supportRegionBound175=regionBound;s.supportTwistSince=twistSince;s.supportAxis59c16=axis59c16;}
    }
};
inline SupportContinuity5910c1 support_continuity5910c1(const WeaponState& s,
        const Input& in,const Profile& p,bool otherFree,Pose primary) noexcept {
    SupportContinuity5910c1 c{};const int other=1-p.side;
    const double age=kcd2vr::motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs);
    if(in.autoSupport||!s.ready||s.key!=p.key||s.generation!=p.generation)return c;
    const bool neutral=std::isfinite(in.grip[other])&&in.grip[other]<.25;
    if(age<0||age>.080000001){c.rearm=in.supportAxis59c16&&(s.support||s.supportRearm)&&!neutral;return c;}
    c.rearm=s.supportRearm&&!neutral;
    const bool axis59c16=s.supportAxis59c16&&in.supportAxis59c16; // acquired shaft grip has no native socket
    if(c.rearm&&s.supportDistanceRelease59c10&&otherFree&&p.twoHanded&&(p.support||axis59c16)&&!p.bow()){
        c.distanceRelease=true;c.returnSince=s.supportRearmReturnSince59c10;
    }
    if(!s.support)return c;
    c.rearm=!neutral; // loss of this grasp cannot silently reacquire while held
    if(!otherFree||neutral||!p.twoHanded||p.bow()||(!p.support&&!axis59c16)||!s.supportGripReady169r2||
       !in.intentTracked[p.side]||!in.intentTracked[other]||!finite(s.supportGrip169r2)||!m::unit(s.supportCorrection)||
       (!in.supportAxis59c16&&!axis59c16&&s.supportPositionOnly175!=p.supportPositionOnly174r2))return c;
    const bool latched=in.supportAxis59c16&&p.melee()&&finite(s.supportPrimary19c3)&&
        m::length(s.supportPrimary19c3.p-p.grip[p.side].p)<.001&&m::angle(s.supportPrimary19c3.q,p.grip[p.side].q)<.001;
    if(in.supportAxis59c16&&!latched)return c;
    const bool region=p.supportPositionOnly174r2&&p.classId==4&&p.supportRegion169r2.valid;
    if(!latched&&!axis59c16&&(s.supportRegionBound175?(!region||!kcd2vr::hilt169r2::contains(p.supportRegion169r2,s.supportGrip169r2.p)):
       m::length(s.supportGrip169r2.p-p.grip[other].p)>=.001))return c;
    Pose weapon=compose(primary,inverse(p.grip[p.side]));
    weapon.q=*m::unit(m::multiply(s.supportCorrection,weapon.q));
    weapon.p=primary.p-m::rotate(weapon.q,p.grip[p.side].p);
    const auto hand=compose(weapon,s.supportGrip169r2);
    if(!latched&&(m::length(hand.p-in.hands[other].p)>.24||m::length(hand.p-in.shoulders[other])>in.reach[other]+.001))return c;
    c.keep=true;c.rearm=false;c.local=s.supportGrip169r2;c.primary=s.supportPrimary19c3;c.correction=s.supportCorrection;
    c.positionOnly=s.supportPositionOnly175;c.regionBound=s.supportRegionBound175;c.twistSince=s.supportTwistSince;c.axis59c16=axis59c16;return c;
}
// The tracking sequence can repeat after native equipment/grip publication.
// Cache the binding used by the solve, not the changing authored native seed.
struct CachedProfileR20 {
    std::uint64_t key=0,generation=0;std::uint32_t entity=0;int side=0,classId=-1;
    unsigned bowRank=0;bool bowNock=false;double bowDraw=0;
    bool primary=false,geometry=false,support=false,twoHanded=false,positionOnly=false;
    Pose grip[2]{};Vec3 tip{},axis{};
    void assign(const Profile& p) noexcept {
        key=p.key;generation=p.generation;entity=p.entity;side=p.side;classId=p.classId;
        bowRank=p.bowGeometryRank48;bowNock=p.bowNockQualified40;bowDraw=p.bowDrawLength166;
        primary=p.primary;geometry=p.geometry;support=p.support;twoHanded=p.twoHanded;
        positionOnly=p.supportPositionOnly174r2;grip[0]=p.grip[0];grip[1]=p.grip[1];tip=p.tip;axis=p.axis;
    }
};
struct State {std::uint64_t kickSeen59c3=0;
    // R59.16C4: open-hand R3 kick. The per-weapon state is reset every frame while the sword Grip is open,
    // so its one-frame-later release is owned here (cleared on epoch/body change with the whole state).
    std::uint64_t kickOpenRelease59c4=0;kcd2vr::kick5916c3::Ticket kickOpenTicket59c4{};
    // Separate from recoverable WeaponState: UNKNOWN/pose resets must not
    // repeatedly buy a fresh-acquisition ray burst for the same held binding.
    struct AcquisitionAttemptD45 {std::uint64_t key=0,generation=0;unsigned entity=0;};
    std::array<AcquisitionAttemptD45,2> acquisitionAttemptD45{};
    std::uint64_t epoch=0,sample=0,bodyEpoch=0;std::array<WeaponState,2> weapon{};std::array<kcd2vr::draw_motion_d63::State,2> drawMotionD63{};std::array<kcd2vr::draw_motion_d63::Decision,2> drawDecisionD63{};Output cached{};std::array<CachedProfileR20,2> cachedProfilesR20{};Pose world{};double worldScale=1;bool haveWorld=false;};
inline void cache_profiles_r20(State& state,const std::array<Profile,2>& profiles) noexcept {
    for(int side=0;side<2;++side)state.cachedProfilesR20[side].assign(profiles[side]);
}
struct RejectedBowRelease166 {std::uint64_t key=0,generation=0,epoch=0,bodyEpoch=0,sample=0;};
// Only explicit failure to dispatch the SAME release restores its input
// ticket. This does not create an arrow or claim that a returned request fired.
inline bool restore_rejected_bow166(State& state,std::uint64_t key,std::uint64_t generation,
    std::uint64_t epoch,std::uint64_t bodyEpoch,std::uint64_t sample){
    auto& bow=state.weapon[1];
    if(!sample||state.epoch!=epoch||state.bodyEpoch!=bodyEpoch||bow.key!=key||bow.generation!=generation||
       bow.lastBowRelease166!=sample||!bow.quiverSpent116||bow.bowHeld)return false;
    bow.quiverSpent116=false;bow.lastBowRelease166=0;return true;
}
// One effective mode for gesture, release and charge consumers. Keep an active
// trigger draw in trigger mode if geometry arrives halfway through that draw.
inline void resolve_bow_mode146(Input& in,const Profile& p,const State* previous=nullptr){
    const auto* old=previous?&previous->weapon[1]:nullptr;
    const bool held=old&&old->bowHeld&&old->key==p.key&&old->generation==p.generation&&
        previous->epoch==in.epoch&&previous->bodyEpoch==in.bodyEpoch;
    if(physical_bow175(in)){
        in.nativeBowTrigger126=in.bowTrigger=false;in.bowStringTrigger160=true;
        if(!manual_bow_geometry146(p)||!in.bowGeometryReady175||
           (held&&old->nativeBowMode146))in.bowActions=false;
        return; // policy transition cancels the old draw; it never inherits RT fire
    }
    if(held&&!old->nativeBowMode146&&!manual_bow_geometry146(p)){
        in.bowActions=false;return; // loss of manual geometry cancels, never fires
    }
    const bool native=held?old->nativeBowMode146:
        in.nativeBowTrigger126||(in.quiverGesture114&&!manual_bow_geometry146(p));
    if(native){in.quiverGesture114=in.bowRegrab116=in.bowNativeGrab146=false;
        in.nativeBowTrigger126=in.bowTrigger=true;}
}
// Caller gives a conservative allowed fraction for the old->candidate weapon
// feature sweeps. 0=blocked/unknown, 1=clear; same result drives both virtual hands.
// Only call this for a new tracking sample; repetitions return the committed result
// without re-emitting commands or integrating a second time.
// V66.102: body axes in Henry model space. CryEngine model space is Z-up; Henry's
// forward follows from the shoulder line (right shoulder = side 0).
struct BodyAxes { Vec3 up{0,0,1},forward{0,1,0},right{1,0,0}; };
inline BodyAxes body_axes(const Input& in){
    BodyAxes a{};auto r=m::unit(body_shoulder_r38(in,0)-body_shoulder_r38(in,1));if(!r)return a;
    Vec3 rr=*r-a.up*m::dot(*r,a.up);auto rn=m::unit(rr);if(!rn)return a;
    a.right=*rn;auto f=m::unit(m::cross(a.up,a.right));if(f)a.forward=*f;return a;
}
inline Vec3 body_vector117(const Input& in,Vec3 v){
    const auto a=body_axes(in);return {m::dot(v,a.right),m::dot(v,a.forward),m::dot(v,a.up)};
}
inline bool windup_cue_motion59c10(const Input& in,Vec3 modelHand,Vec3 relativeHand,Vec3 modelTip,Vec3 relativeTip,double dt) noexcept {
    if(!std::isfinite(dt)||dt<=0||dt>.120000001||!m::finite(modelHand)||!m::finite(relativeHand)||
       !m::finite(modelTip)||!m::finite(relativeTip))return false;
    const auto mh=body_vector117(in,modelHand),rh=body_vector117(in,relativeHand);
    const auto mt=body_vector117(in,modelTip),rt=body_vector117(in,relativeTip);
    // Same two independent motion frames and body basis as the established
    // settled wind-up. A model-only locomotion/turn cannot advertise a threat.
    // Reuse deliberate-cut speed floors; a slow backward reach remains idle.
    const bool hand=mh.y<-melee118::kCutWindupInward51*dt&&rh.y<-melee118::kCutWindupInward51*dt&&
        std::min(m::length(modelHand),m::length(relativeHand))>=.8*dt;
    const bool tip=mt.y<-melee118::kCutWindupTip55*dt&&rt.y<-melee118::kCutWindupTip55*dt&&
        std::min(m::length(modelTip),m::length(relativeTip))>=2.0*dt&&
        std::min(m::length(modelTip-modelHand),m::length(relativeTip-relativeHand))>=melee118::kCutRotSpeed46*dt;
    return hand||tip;
}
// Shared geometry for attack arbitration and the native block edge. No speed
// ceiling: raising a guard quickly must not delay a timed native block. The
// center can lie exactly on the chest's vertical axis; that is a valid guard.
inline bool natural_guard117(const Input& in,const Profile& p,Pose weapon,bool held){
    const bool shield=in.positionGuard176&&p.side==1&&p.shield176();
    if(p.side<0||p.side>1||!in.allowed||!in.tracked[p.side]||(!shield&&(!p.melee()||p.side!=0))||!p.geometry||!finite(weapon)||
       !m::finite(body_shoulder_r38(in,0))||!m::finite(body_shoulder_r38(in,1))||
       (in.requirePrimaryGrip114&&(!std::isfinite(in.grip[p.side])||in.grip[p.side]<.25)))return false;
    const auto chest=body_center_r38(in);
    const auto grip=point(weapon,p.grip[p.side].p),tip=point(weapon,p.tip);
    if(in.positionGuard176){
        Vec3 localFace{};const bool planar=shield||planar_face24(p,localFace);
        // Ambiguous shape cannot establish the requested automatic broad-face
        // posture. Native/manual controls and real weapon contacts are separate.
        return kcd2vr::guard_parry176::position({grip,tip,chest,body_axes(in).right,shield,
            shield?Vec3{}:m::rotate(weapon.q,localFace),!shield&&planar,in.guardHead,in.guardHeadKnown},held,in.guardStrict59c17);
    }
    const auto axis=m::unit(body_vector117(in,tip-grip));
    const auto offset=body_vector117(in,(grip+tip)*.5-chest);
    return axis&&std::abs(axis->y)<(held?.75:.60)&&std::abs(axis->z)<(held?.88:.78)&&
        offset.y>(held?.07:.10)&&offset.y<(held?.90:.85)&&
        std::abs(offset.x)<(held?.65:.55)&&offset.z>(held?-.25:-.20)&&offset.z<(held?.42:.35)&&m::length(grip-chest)<.85;
}
inline int stroke_zone117(CommandKind kind,Vec3 bodyVelocity){
    if(!m::finite(bodyVelocity))return -1;
    if(kind==CommandKind::Pommel)return -1; // contextual special input owns direction
    if(kind==CommandKind::Thrust)return 5;
    // D84 up cut: a cut whose blade moves UP is the native low attack (5) when the switch is on (combos).
    if(upCutLowD84Enabled()&&bodyVelocity.z>0&&std::abs(bodyVelocity.z)>std::abs(bodyVelocity.x)*1.25)return 5;
    // Native cuts use the side the stroke starts from. Upward cuts retain a
    // lateral sector; the disabled table sectors 3/4 are never synthesized.
    if(bodyVelocity.z<0&&std::abs(bodyVelocity.z)>std::abs(bodyVelocity.x)*1.25)return 0;
    return bodyVelocity.x>=0?1:2;
}
// Right shoulder keeps the quiver (right_shoulder_zone is the arrow pick, unchanged).
inline Vec3 right_shoulder_zone(const Input& in){const auto a=body_axes(in);return body_shoulder_r38(in,0)+a.up*.10-a.forward*.10;}
// Explicit grab layout: left hand reaches the right shoulder for the bow;
// left shoulder for a shield. Third-loadout and heavy weapons use the right
// hand behind the right side; crossbows use the left hand there. These anchors never alter an in-hand weapon pose.
inline Vec3 right_shoulder_bow_zone_r12(const Input& in){const auto a=body_axes(in);return holster_shoulder_d85(in,0)+a.up*.10-a.forward*.10;}
inline Vec3 left_shoulder_shield_zone_hs2(const Input& in){const auto a=body_axes(in);return holster_shoulder_d85(in,1)+a.up*.10-a.forward*.10;}
// HS (27 Oct): H3 drew two-handed weapons at the right shoulder itself. The R12 back point (18 cm down, 22 cm behind)
// was out of reach with a tracked controller (J27: 36 tries, 0 drawn, gaps .23-.68 m). Keep it behind the
// shoulder, but where a hand actually gets: 12 cm down, 14 cm back.
inline Vec3 right_back_weapon_zone_r12(const Input& in){const auto a=body_axes(in);return holster_shoulder_d85(in,0)-a.up*.12-a.forward*.14;}
// Historical callers use this name only for the bow acquisition point.
inline Vec3 left_shoulder_zone_h42(const Input& in){return right_shoulder_bow_zone_r12(in);}
inline bool shoulder_mount116(const Input& in,const Profile& p,Vec3& mount){
    if(p.bow()){mount=right_shoulder_bow_zone_r12(in);return true;}
    if(p.shield176()){mount=left_shoulder_shield_zone_hs2(in);return true;}
    return false;
}
// Equipment gestures use requested tracking before visible anatomical compression.
// This copy changes no physics pose, clocks, eligibility or opposite-hand owner.
inline Input holster_tracking_input_h43(const Input& physical,Vec3 right,Vec3 left) {
    Input result=physical;result.hands[0].p=right;result.hands[1].p=left;return result;
}
enum class HolsterZone126 { None, RightShoulder, LeftShoulder, NativeBelt, RightHip45, LeftBack45, LeftChest45, LeftShoulderBow, RightShoulderBowR12, RightBackR12 };
inline HolsterZone126 holster_zone126(const Profile& p){
    if(p.bow())return HolsterZone126::RightShoulderBowR12;
    if(p.shield176())return HolsterZone126::LeftShoulder;
    if(p.classId==0)return HolsterZone126::LeftChest45;
    if((p.holsterSetR12==2&&p.classId>0&&p.classId!=11&&p.classId!=12)||
       p.classId==7||p.classId==10||p.classId==13||p.classId==14||p.classId==15)
        return HolsterZone126::RightBackR12;
    if(p.classId==3||p.classId==5||p.classId==6)return HolsterZone126::RightHip45;
    if(p.classId==1||p.classId==2||p.classId==4||p.classId==16)return HolsterZone126::NativeBelt;
    return HolsterZone126::None;
}
inline int holster_hand126(const Profile& p){
    if(holster_zone126(p)==HolsterZone126::None)return -1;
    if(p.classId==9||p.classId==8||p.classId==17)return 1;
    if(p.classId==10||p.classId==14||p.classId==15)return 1; // crossbow: left hand reaches behind RIGHT back
    return 0;
}
inline bool holster_mount126(const Input& in,const Profile& p,Vec3& mount){
    const auto a=body_axes(in);const auto zone=holster_zone126(p);
    const auto centre=body_center_r38(in);
    if(zone==HolsterZone126::RightBackR12){mount=right_back_weapon_zone_r12(in);return true;}
    if(zone==HolsterZone126::RightShoulderBowR12){mount=right_shoulder_bow_zone_r12(in);return true;}
    if(zone==HolsterZone126::LeftShoulder){mount=left_shoulder_shield_zone_hs2(in);return true;}
    if(zone==HolsterZone126::RightHip45){mount=body_shoulder_r38(in,0)-a.up*.50+a.right*.03;return true;}
    if(zone==HolsterZone126::LeftChest45){mount=centre-a.right*.12-a.up*.18+a.forward*.12;return true;}
    return false; // swords retain their current native scabbard mount
}
// V66.131: shoulder holsters are body-space capsules, not mathematical points.
// Return the closest point on a short vertical segment through the selected
// shoulder zone. Existing gesture radii then form the capsule. Belt mounts stay
// exact native attachment points in the adapter.
inline bool shoulder_holster_closest131(const Input& in,const Profile& p,Vec3 hand,Vec3& mount){
    if(!m::finite(hand))return false;
    const auto zone=holster_zone126(p);
    if(zone!=HolsterZone126::RightBackR12&&zone!=HolsterZone126::LeftShoulder&&zone!=HolsterZone126::RightShoulderBowR12)return false;
    if(!holster_mount126(in,p,mount))return false;
    const auto a=body_axes(in);
    // Keep the acquisition anchor ABOVE the shoulder instead of letting
    // closest-point projection cancel its entire vertical placement.
    // HS: H3 capsule (-10..+14 cm) for bow/shield again; R14 made it 0..12 cm = the hand had to be ABOVE the zone point.
    const double along=zone==HolsterZone126::RightBackR12?std::clamp(m::dot(hand-mount,a.up),-.14,.12):std::clamp(m::dot(hand-mount,a.up),-.10,.14);
    mount=mount+a.up*along;return m::finite(mount);
}
// The bow requires an over-shoulder reach. Back weapons accept a reach
// below the shoulder as well, but never a hand in front of the chest.
// D48: one stored-item origin for rendered front hardware and its acquisition
// point. A measured grip offset is retained; before it exists, the actual model
// origin is an explicit approximate grip point, never a measured native socket.
inline bool front_stowed_pose_d48(const Input& in,const Profile& p,Pose natural,
                                  Pose localGrip,bool gripKnown,Pose& placed) noexcept {
    if(holster_zone126(p)!=HolsterZone126::RightHip45||!finite(natural)||
       (gripKnown&&!finite(localGrip)))return false;
    Vec3 mount{};if(!holster_mount126(in,p,mount)||!m::finite(mount))return false;
    placed=natural;
    placed.p=mount-m::rotate(placed.q,gripKnown?localGrip.p:Vec3{});
    return finite(placed);
}
inline bool over_shoulder_zone_hs2(HolsterZone126 zone) noexcept {
    return zone==HolsterZone126::RightShoulderBowR12||zone==HolsterZone126::RightBackR12;
}
inline bool draw_volume_ok_hs2(const Input& in,const Profile& p,Vec3 hand){
    const auto zone=holster_zone126(p);if(!over_shoulder_zone_hs2(zone))return true;
    if(!m::finite(hand)||!m::finite(holster_shoulder_d85(in,0)))return false;
    const auto a=body_axes(in);const auto d=hand-holster_shoulder_d85(in,0); // D85: the player's shoulder plane, not Henry's
    // HS: R14's volume demanded the hand 6 cm BEHIND the shoulder plane (back weapon) or above it (bow). H3 had no
    // volume at all. Keep the intent (never a hand in front of the chest - the chest knife pouch K32 and the guard
    // stay separate) with tracked-controller tolerance.
    if(zone==HolsterZone126::RightBackR12)return m::dot(d,a.up)>-.60&&m::dot(d,a.up)<.20&&m::dot(d,a.forward)<.05&&m::dot(d,a.right)>-.18; // D38: wrist point, see below
    // the chest knife pouch (K32, 12 cm left of the sternum) lies well left of the right shoulder: d.right > -.14 keeps them apart.
    // D38: the hand point here is the WRIST goal (VrAutoPalmAlign: a palm-length behind the grip), so the HS
    // limits refused real over-the-shoulder reaches that H3 (no volume) drew. back: 5 cm in front / 18 cm inward,
    // D53: the tested shoulder capsule stays generous vertically and inward, but its
    // acquisition stops at the shoulder plane. The prior +15 cm front slab
    // advertised a bow while the left hand was still in front of the shoulder.
    // This same boundary governs haptics, admission and chest-pouch exclusion.
    return m::dot(d,a.up)>-.15&&m::dot(d,a.forward)<0.&&m::dot(d,a.right)>-.24;
}
// A closed hand in an available shoulder holster volume can acquire that item.
// HS: the same priority for EVERY holster zone. A closed hand (Grip, with or without Trigger) inside an available
// holster's own acquisition zone is a holster grab, not a combat fist. Over-shoulder zones also need their volume.
inline bool holster_grab_hs(const Input& in,const Profile& p,Vec3 hand,Vec3 mount,double radius){
    if(holster_zone126(p)==HolsterZone126::None||!m::finite(hand)||!m::finite(mount)||!std::isfinite(radius)||radius<=0)return false;
    return draw_volume_ok_hs2(in,p,hand)&&m::length(hand-mount)<radius;
}
// K35: LEFT chest (12 cm left of the sternum), with the existing 16 cm acquisition radius. Never accepts a hand that the
// over-the-shoulder bow/heavy volume holds, so the pouch and the bow can't both claim one hand.
inline Vec3 chest_knife_pouch_k32(const Input& in){const auto a=body_axes(in);return body_center_r38(in)-a.right*.12-a.up*.18+a.forward*.12;}
inline bool chest_knife_pouch_holds_k32(const Input& in,Vec3 hand){
    if(!m::finite(hand))return false;const auto p=chest_knife_pouch_k32(in);if(!m::finite(p)||m::length(hand-p)>=.16)return false;
    Profile bow{};bow.classId=9;return !draw_volume_ok_hs2(in,bow,hand);
}
// D66 location trial: reuse T15's invisible right-hip-front anchor only.
// Acquisition remains the independent LEFT-hand service, never the old RIGHT
// equipment Draw route. Legacy chest geometry is returned exactly when OFF.
inline Vec3 knife_pouch_d66(const Input& in,bool rightHip){
    if(!rightHip)return chest_knife_pouch_k32(in);
    const auto a=body_axes(in);
    return body_shoulder_r38(in,0)-a.up*.50+a.right*.03+a.forward*.13;
}
inline bool knife_pouch_holds_d66(const Input& in,Vec3 hand,bool rightHip){
    if(!rightHip)return chest_knife_pouch_holds_k32(in,hand);
    const auto p=knife_pouch_d66(in,true);
    return m::finite(hand)&&m::finite(p)&&m::length(hand-p)<.16;
}
// Knife acquisition follows the tracked player's left hip, independent of
// native upper-body animation and old chest/right-hip trial settings.
inline Vec3 knife_left_hip_d85r5(const Input& in) noexcept {
    const auto a=body_axes(in);
    return holster_shoulder_d85(in,1)-a.up*.50-a.right*.03+a.forward*.08;
}
inline bool knife_left_hip_holds_d85r5(const Input& in,Vec3 hand) noexcept {
    const auto center=knife_left_hip_d85r5(in);
    return m::finite(hand)&&m::finite(center)&&m::length(hand-center)<.16;
}
// R109: the knife zone is the left hip (D85R5) AND, with VrKnifeChestZoneR109 on, the K32 chest
// pouch where the VrChestDagger visual sits. one 16 cm radius each. the chest pouch keeps its
// bow-volume guard (chest_knife_pouch_holds_k32) so the bow hand and the pouch never share a hand.
inline bool knife_pouch_holds_r109(const Input& in,Vec3 hand) noexcept {
    if(knife_left_hip_holds_d85r5(in,hand))return true;
    return knife_pouch_d66::chestZoneSetting()&&chest_knife_pouch_holds_k32(in,hand);
}
// the zone centre nearest to the hand: offsets, the empty-pouch line and the haptic cue measure from it.
inline Vec3 knife_pouch_centre_r109(const Input& in,Vec3 hand) noexcept {
    const auto hip=knife_left_hip_d85r5(in);
    if(!knife_pouch_d66::chestZoneSetting())return hip;
    const auto chest=chest_knife_pouch_k32(in);
    if(!m::finite(hand)||!m::finite(chest))return hip;
    if(!m::finite(hip))return chest;
    return m::length(hand-chest)<m::length(hand-hip)?chest:hip;
}
// Rotate 'from' toward 'to' by at most 'maxAngle' radians.
inline Quat limit_toward(Quat from,Quat to,double maxAngle){
    const double ang=m::angle(from,to);if(!(ang>maxAngle)||ang<1e-9)return to;
    auto r=m::slerp(from,to,maxAngle/ang);return r?*r:to;
}
// Propose translation along/out of an observed surface. This is NOT collision
// permission: both call sites must sweep the candidate before retaining it.
inline Vec3 wall_recovery_step59c10r2(const Contact103& c,Vec3 remaining,bool allowBackout,bool& backout){
    backout=false;
    if(!c.known||!c.plane||c.limited115||!m::finite(remaining))return {};
    const double outward=m::dot(remaining,c.normal);
    backout=allowBackout&&outward>.001;
    return backout?c.normal*outward:remaining-c.normal*outward;
}
// Wall experiment: propose an EXTRA MODEL offset capped in WORLD metres.
// This intentionally exceeds requested outward travel by <=25%, <=2.5cm world.
// It is not native rigid-body physics, an impulse or hit/effort authority.
inline bool wall_spring_world_h31(const Contact103& c) noexcept {
    namespace n=kcd2vr::npc_contact172;const auto& w=c.native172;
    if(c.currentOpponent175||w.body==n::Body::Living||w.body==n::Body::Articulated||w.body==n::Body::Rigid)return false;
    if(w.mask==n::StaticTerrain)return true;
    // Exact base-physical world witness, NOT a guessed static/GetType claim.
    return w.mask==n::BodiesAndWorld&&w.body==n::Body::Physical&&w.vft==0x3a578c0&&w.collider&&
        !w.ownerRead&&!w.owner&&!w.entityId;
}
template<class Sweep>
inline bool checked_wall_spring_h31(const Input& in,const Profile& p,const Contact103& c,
    Pose from,Pose desired,Vec3 base,bool backout,Pose& result,Sweep&& sweep){
    if(!in.wallBackoutSpringH31||!backout||!in.contact||in.contactFaultH30||in.contactSuspended59c1||
       p.side<0||p.side>1||!p.melee()||!p.geometry||!c.known||!c.plane||c.limited115||!wall_spring_world_h31(c)||
       !finite(from)||!finite(desired)||!m::finite(base)||!m::finite(c.normal)||std::abs(m::length(c.normal)-1.)>.001)return false;
    const double scale=in.haveWorld?in.worldScale:1.;
    if(!std::isfinite(scale)||scale<.1||scale>10)return false;
    const double outward=m::dot(base,c.normal),extra=std::min(.025/scale,outward*.25);
    if(!std::isfinite(extra)||extra*scale<.0005)return false;
    Pose candidate=from;candidate.p=candidate.p+base+c.normal*extra;
    if(!finite(candidate)||m::length(point(candidate,p.grip[p.side].p)-in.shoulders[p.side])>in.reach[p.side]+.001)return false;
    const auto clear=[](auto raw){const auto hit=contact103(raw);return hit.known&&!hit.plane&&!hit.limited115&&hit.fraction>=1.-1e-9;};
    // First prove the whole retained translation at the old checked orientation.
    if(!clear(sweep(p,from,candidate,false)))return false;
    // Independent final-pose/rotation veto: check the extra tail from the
    // requested unassisted grip, at the requested orientation, to the retained
    // candidate. This is a real feature sweep, not a zero-motion clearance test.
    Pose retention{desired.q,{}};const Vec3 grip=point(from,p.grip[p.side].p)+base;
    retention.p=grip-m::rotate(retention.q,p.grip[p.side].p);
    if(!finite(retention)||!clear(sweep(p,retention,candidate,false)))return false;
    result=candidate;return true;
}
#include "wall_response_h29r4.inl"
// Copy only tracked input/semantic ancestry across a collision-pose reset.
// Actual contact, body velocity and checked path are never copied as permission.
inline void restore_normal_intent_h30(WeaponState& s,const WeaponState& source) noexcept {
    s.gesture59c3=source.gesture59c3;
    s.feintFE=source.feintFE;
    s.committedGestureH26=source.committedGestureH26;
    s.lastTrackingRotationH29=source.lastTrackingRotationH29;
    s.sourceModelH30=source.sourceModelH30;s.sourceTrackingH30=source.sourceTrackingH30;
    s.sourceOtherModelH30=source.sourceOtherModelH30;s.sourceOtherTrackingH30=source.sourceOtherTrackingH30;
    s.sourceGripH30=source.sourceGripH30;s.sourceTipH30=source.sourceTipH30;s.sourcePoseH30=source.sourcePoseH30;
    s.sourceModelFrameH8=source.sourceModelFrameH8;s.sourceModelCalibrationH8=source.sourceModelCalibrationH8;
    s.prepared=source.prepared;
    s.prepareTick=source.prepareTick;
    s.spent=source.spent;
    s.released44c1=source.released44c1;
    s.waitingPrepare44c1=source.waitingPrepare44c1;
    s.gripRearm=source.gripRearm;
    s.quietSince=source.quietSince;
    s.lastAttack=source.lastAttack;
    s.lastStrike178=source.lastStrike178;
    s.strikeSerial178=source.strikeSerial178;
    s.weakStrike178=source.weakStrike178;
    s.intentTip=source.intentTip;
    s.intentHand=source.intentHand;
    s.relativeIntentTip=source.relativeIntentTip;
    s.relativeIntentHand=source.relativeIntentHand;
    s.lastStrokeDirection=source.lastStrokeDirection;
    s.intentReady=source.intentReady;
    s.relativeIntentReady=source.relativeIntentReady;
    s.intentSamples=source.intentSamples;
    s.travel=source.travel;
    s.thrustTravel=source.thrustTravel;
    s.reversalTravel=source.reversalTravel;
    s.lastStrokeKind=source.lastStrokeKind;
    s.guard117=source.guard117;
    s.zone117=source.zone117;
    s.stroke117=source.stroke117;
    s.guardEntry30c1=source.guardEntry30c1;
    s.guardThrust49=source.guardThrust49;
    s.guardAttackUntil49=source.guardAttackUntil49;
    s.guardExitTick32=source.guardExitTick32;
    s.guardFeintUntil59c17=source.guardFeintUntil59c17;
    s.guardFeints59c17=source.guardFeints59c17;
    s.guardForwardVetoes59c17=source.guardForwardVetoes59c17;
    s.origin167=source.origin167;
    s.originZone167=source.originZone167;
    s.cocked57=source.cocked57;
    s.holdRearmH26=source.holdRearmH26;
    s.stroke118=source.stroke118;
    s.natural118=source.natural118;
    s.guardStroke36=source.guardStroke36;
    s.guardOrigin36=source.guardOrigin36;
    s.guardDirect36=source.guardDirect36;
    s.masterHandoffSerial23=source.masterHandoffSerial23;
    s.masterHandoffGesture23=source.masterHandoffGesture23;
    s.lastTick=source.lastTick;
    s.timeNs=source.timeNs;
    s.supportIntent63=source.supportIntent63;
    s.supportPrimaryBiasD65=source.supportPrimaryBiasD65;
    s.supportIntentHand63=source.supportIntentHand63;
    s.supportRelativeIntentHand63=source.supportRelativeIntentHand63;
    // Retain the immutable original occurrence for the existing bounded queue.
    // This is not current collision permission and its tick/sample are unchanged.
    s.impactRecord178r30=source.impactRecord178r30;s.lastImpactTick=source.lastImpactTick;
    s.unpublishedImpactTickH8=source.unpublishedImpactTickH8;
}
#include "intent_frame_rebase_h8.inl"
#include "committed_stroke_h8.inl"
// Final publication boundary for the normal-HYBRID uncertainty certificate.
// Geometry may still stop the blade and produce physical haptics. An unreadable
// motion increment must not mint a semantic impact or replace a prior checked
// occurrence that the slower native consumer has not yet received.
struct QuarantinedImpactPublicationH8 {
    const Input& in;const Profile& profile;WeaponState& state;Output& output;int side;
    ImpactRecord178r30 before;
    QuarantinedImpactPublicationH8(const Input& i,const Profile& p,WeaponState& s,Output& o,int n) noexcept:
        in(i),profile(p),state(s),output(o),side(n),before(s.impactRecord178r30){}
    ~QuarantinedImpactPublicationH8() noexcept {
        const auto& q=output.intentQuarantineH8[side];
        if(q.gesture&&q.owner.current&&q.owner.sample==in.sample&&q.owner.tick==in.tick){
            if(output.impactEventTick178r24[side]==in.tick)state.unpublishedImpactTickH8=in.tick;
            const bool retained=before.valid&&before.key==profile.key&&before.generation==profile.generation&&
                before.epoch==in.bodyEpoch&&before.request==in.epoch&&before.sample<in.sample&&
                in.tick>=before.tick&&in.tick-before.tick<=250;
            state.impactRecord178r30=retained?before:ImpactRecord178r30{};
            output.impactRecord178r30[side]={};
            output.impactEventTick178r24[side]=0;output.impactEventSpeed178r24[side]=0;
        }
        // The physical recoil timestamp is deliberately untouched. Do not turn
        // its cached event into semantic FX on the following clear frame.
        if(state.unpublishedImpactTickH8&&output.impactEventTick178r24[side]==state.unpublishedImpactTickH8){
            output.impactEventTick178r24[side]=0;output.impactEventSpeed178r24[side]=0;
        }
    }
};
#include "normal_intent_phase_h30.inl"
#include "prediction_intent_rebase_d69.inl"
inline bool profile_input_valid_r20(const Input& in,const Profile& p,int side) noexcept {
    return p.key&&p.primary&&p.side==side&&in.tracked[side]&&finite(in.hands[side])&&finite(p.grip[side])&&
            m::finite(in.shoulders[side])&&std::isfinite(in.reach[side])&&in.reach[side]>.05&&in.reach[side]<2&&
            std::isfinite(in.grip[side])&&(!p.bow()||!in.bowGripDraw47c2||std::isfinite(in.grip[0]))&&(!bow_uses_trigger175(in)||!p.bow()||std::isfinite(in.trigger[0]))&&std::isfinite(p.weight)&&p.weight>0&&std::isfinite(p.extent)&&p.extent>=0&&m::finite(p.tip)&&m::finite(p.axis)&&
            (!in.attributes.live||(std::isfinite(in.attributes.strength)&&std::isfinite(in.attributes.agility)&&std::isfinite(in.attributes.stamina)));
}
inline bool same_grip_r20(Pose a,Pose b) noexcept {
    // Round-off only (one micron / 0.0006 degrees), not a new grasp tolerance.
    // Quaternion sign is not a different binding.
    return finite(a)&&finite(b)&&m::dot(a.p-b.p,a.p-b.p)<=1e-12&&m::angle(a.q,b.q)<=1e-5;
}
inline bool cached_profile_current_r20(const CachedProfileR20& old,const Profile& p,int side,bool coupled) noexcept {
    return old.key==p.key&&old.generation==p.generation&&old.entity==p.entity&&old.side==p.side&&old.classId==p.classId&&
        old.primary==p.primary&&old.geometry==p.geometry&&same_grip_r20(old.grip[side],p.grip[side])&&
        m::dot(old.tip-p.tip,old.tip-p.tip)<=1e-12&&
        // A bow's axis/nock/draw span belong to the string, not the left-hand
        // attachment. A geometry publication between two skin callbacks must
        // not restore the whole bow to its native pose on a repeated XR sample.
        (p.bow()||m::dot(old.axis-p.axis,old.axis-p.axis)<=1e-12)&&
        (p.bow()||!coupled||(old.support==p.support&&old.twoHanded==p.twoHanded&&old.positionOnly==p.supportPositionOnly174r2&&
                   same_grip_r20(old.grip[1-side],p.grip[1-side])));
}
inline void retire_cached_side_r20(Output& out,const Input& in,int side) noexcept {
    out.normalIntentH30[side]={};out.intentQuarantineH8[side]={};out.resetH29[side]={};
    out.reachClampedR4[side]=out.solverOwnedT4[side]=false;out.checkedPath60[side]={};out.ownShieldBlocked60[side]=false;
    out.releaseReason121[side]=out.invalidMask55c1[side]=0;out.handlingOnly55c1[side]=false;out.acquisitionReachError124[side]=0;
    out.hands[side]=out.displayHands[side]=in.hands[side];out.weapons[side]=out.displayWeapons[side]={};
    out.owned[side]=out.active[side]=out.support[side]=out.blocked[side]=out.contactKnown[side]=out.sweepDeferred115[side]=false;
    out.contactPrefixKnown126[side]=false;out.impactSpeed[side]=0;out.cueKey[side]=0;out.cueGap[side]=1000;
    out.scrapeSpeed58[side]=0;out.wallBlocked58[side]=out.wallSlid58[side]=out.wallBackout58[side]=out.wallStuck58[side]=out.wallScraped58[side]=false;
    out.impactEventTick178r24[side]=0;out.impactEventSpeed178r24[side]=0;out.impactRecord178r30[side]={};
    out.displayLatched[side]=out.displayRejoinR13[side]=out.guardBySide30c1[side]=false;
    out.contactMotion169r1[side]={};out.contact119[side]={};out.contact119[side].known=false;out.impact177[side]={};
    if(side==0){
        out.solverH28={};out.solverGestureH29={};out.solverWorldT4={};out.solverLeaseT4=out.solverWorldGenT4=out.solverBodyGenT4=0;
        out.solverSourceTickT4=out.solverSourceSampleT4=out.solverTrackingEpochH24=out.solverCancelGenerationH24=0;
        out.hitTraceC54R3={};out.kickDrop59c2=0;out.kickDropTick59c2=0;out.guardFeints59c17=out.guardForwardVetoes59c17=0;
        out.strokeActive119=false;out.primaryStrokeGesture19=0;out.melee118={};out.strike178={};out.masterHandoff23={};out.arrival39={};
    }
    if(out.impaleSide32==side){
        out.impaleLatchRequest=out.impaleActive=out.impaleReleased=false;out.impaleEntity=0;out.impaleSerial=0;
        out.impaleTipWorld={};out.impalePull=0;out.impaleSide32=-1;out.impaleLocal32={};out.impaleRelease40=0;
        out.impaleHit47={};out.impalePick47=-1;out.impaleTipM47=-1;out.impaleDepth47=0;
    }
    if(out.impaleRefusalSide47==side){out.impaleRefusal47=out.impaleRefusalReason47=0;out.impaleRefusalSide47=-1;out.impaleRefusalSerial47=0;out.impaleRefusalTipM47=-1;}
}
inline Output repeated_output_r20(const State& state,const Input& in,const std::array<Profile,2>& profiles) noexcept {
    Output out=state.cached;out.commandCount=0;out.impactSpeed[0]=out.impactSpeed[1]=0;
    out.wallScraped58[0]=out.wallScraped58[1]=false;
    bool retire[2]{};
    for(int side=0;side<2;++side){
        const auto& p=profiles[side];const auto& old=state.cachedProfilesR20[side];
        const auto& held=state.weapon[side];
        // A manually latched grasp owns its stored local frame; a later native
        // off-hand socket refresh does not replace that accepted grasp.
        const bool storedSupport=out.support[side]&&in.supportAxis59c16&&!in.autoSupport&&p.twoHanded&&p.melee()&&!p.bow()&&
            held.supportGripReady169r2&&same_grip_r20(held.supportPrimary19c3,p.grip[side]);
        const bool coupled=(out.support[side]&&!storedSupport)||(side==1&&(out.bowHeld||out.bowHand169r2));
        // Empty/support-only hands have no independent physical owner. A new
        // profile on that side cannot inherit the other weapon's support wrist.
        if(out.active[side]||state.weapon[side].key){
            retire[side]=!cached_profile_current_r20(old,p,side,coupled)||!profile_input_valid_r20(in,p,side);
            if(!retire[side]&&in.requirePrimaryGrip114&&!p.torch40()&&in.grip[side]<.25){
                const auto& held=state.weapon[side];
                retire[side]=!(out.releaseReason121[side]==2&&in.releasePresentation45&&in.intentTracked[side]&&
                    held.releaseVisualSince45&&in.tick>=held.releaseVisualSince45&&in.tick-held.releaseVisualSince45<=250);
            }
        }
    }
    for(int primary=0;primary<2;++primary){const int other=1-primary;
        if(!out.support[primary]&&!(primary==1&&(out.bowHand169r2||out.bowHeld)))continue;
        const bool otherCurrent=!profiles[other].key&&in.tracked[other]&&finite(in.hands[other])&&
            m::finite(in.shoulders[other])&&std::isfinite(in.reach[other])&&in.reach[other]>.05&&std::isfinite(in.grip[other]);
        const bool supportReleased=out.support[primary]&&!in.autoSupport&&in.grip[other]<.25;
        if(retire[primary]||!otherCurrent||supportReleased){
            retire[other]=true;out.support[primary]=false;out.supportProbe59c10={};
        }
    }
    if(profiles[1].bow()&&(out.bowHand169r2||out.bowHeld)){
        const auto& old=state.cachedProfilesR20[1];const auto& bow=profiles[1];
        // An invalidated or materially changed string relinquishes only its
        // right-hand coupling. It cannot revoke an unchanged primary grasp.
        // The measured float scale may differ by round-off between callbacks.
        const bool stringCurrent=old.bowRank==bow.bowGeometryRank48&&old.bowNock==bow.bowNockQualified40&&
            std::isfinite(old.bowDraw)&&std::isfinite(bow.bowDrawLength166)&&
            std::abs(old.bowDraw-bow.bowDrawLength166)<=1e-6&&old.support==bow.support&&
            same_grip_r20(old.grip[0],bow.grip[0])&&m::dot(old.axis-bow.axis,old.axis-bow.axis)<=1e-12;
        if(!stringCurrent)retire[0]=true;
    }
    if(retire[1]||((out.bowHand169r2||out.bowHeld)&&retire[0])){
        out.bowHand169r2=out.bowHandlingOnlyR12=out.bowHeld=false;out.bowFraction=0;out.bow116={};
    }
    for(int side=0;side<2;++side)if(retire[side]){
        if(out.supportProbe59c10.side==side)out.supportProbe59c10={};
        if(out.hipGrantT10.key==state.cachedProfilesR20[side].key)out.hipGrantT10={};
        retire_cached_side_r20(out,in,side);
    }
    if(retire[0]||retire[1])out.guard117=out.guardBySide30c1[0]||out.guardBySide30c1[1];
    // No integration, new command, state reset, or sample consumption. The next
    // real source sample retains the existing owner/acquisition/rebase policy.
    return out;
}
template<class Sweep>
Output step(State& state,const Input& in,const std::array<Profile,2>& profiles,Sweep&& sweep){
    Output out{};out.hands[0]=in.hands[0];out.hands[1]=in.hands[1];
    if(!in.allowed||!in.sample||!in.tick||(in.haveWorld&&(!finite(in.world)||!std::isfinite(in.worldScale)||in.worldScale<.1||in.worldScale>10))){state={};return out;}
    if(state.epoch!=in.epoch||state.bodyEpoch!=in.bodyEpoch){state={};state.epoch=in.epoch;state.bodyEpoch=in.bodyEpoch;}
    if(in.sample==state.sample)return repeated_output_r20(state,in,profiles);
    if(in.sample<state.sample)return out;
    state.sample=in.sample;
    for(int side=0;side<2;++side){
        const auto commandBegin59c3=out.commandCount;
        const auto& p=profiles[side];auto& s=state.weapon[side];const int other=1-side;
        // Observe pending extraction on the existing raw source even while the
        // native held profile/geometry has not arrived. This metadata grants no
        // pose, collision or attack; only drawHeldD63 plus exact profile may ACK.
        {namespace dm=kcd2vr::draw_motion_d63;const auto& ticket=in.drawMotionD63[side];
         const bool held=in.drawHeldD63[side]&&p.primary&&p.side==side&&p.key==ticket.key&&p.generation==ticket.generation;
         const bool otherHeld=p.primary&&p.key&&(p.key!=ticket.key||p.generation!=ticket.generation);
         const dm::Owner owner{otherHeld?p.key:ticket.key,otherHeld?p.generation:ticket.generation,in.bodyEpoch,in.epoch,in.sample,in.tick,side};
         state.drawDecisionD63[side]={};
         if(ticket.serial||state.drawMotionD63[side].ticket.serial){const auto source=draw_source_d63(in,side);
             state.drawDecisionD63[side]=dm::update(state.drawMotionD63[side],ticket,owner,{source,source.valid,held});}}
        if(!p.key)state.acquisitionAttemptD45[side]={}; // actual no-item interval ends the binding
        QuarantinedImpactPublicationH8 quarantinePublicationH8(in,p,s,out,side);
        const bool valid=profile_input_valid_r20(in,p,side);
        // R155 (Miki 8 Nov: bow arrow "jumps back to the unpulled position" / "disables itself"; 8 Nov log 11:08:52.763 and
        // 11:09:05.6: BOW_STATE HAND_UNAVAILABLE gap=0 rearm=1 with gripRL=1.000 at full draw, 11:08:53.754 the Grip opened
        // a second later): ONE solve with bowActions or bowGeometryReady175 false (a tracking sample the runtime reported
        // held with the string hand at the face, an 80 ms geometry/nock receipt that expired for one solve) cancelled the
        // whole held draw: native uncharge (the string and arrow snap back), rearm (open and close the Grip again).
        // a HELD, nocked physical draw now keeps its state through such a dropout for kBowDropoutMs155 while the string
        // hand still squeezes and both hands are valid; a longer dropout, tracking loss or an open hand cancel as before.
        // no latch: the clock clears as soon as readiness is back or the draw ends. new draws still need full readiness.
        bool bowActionsR155=in.bowActions,bowGeometryReadyR155=in.bowGeometryReady175;
        if(p.bow()&&side==1&&physical_bow175(in)&&s.bowHeld&&!s.carry146&&(!in.bowActions||!in.bowGeometryReady175)&&
           in.allowed&&in.tracked[0]&&in.tracked[1]&&manual_bow_geometry146(p)){
            const double holdR155=bow_uses_trigger175(in)?in.trigger[0]:in.grip[0];
            if(!std::isfinite(holdR155)||holdR155<.25)s.bowDropoutSince155=0;
            else{
                if(!s.bowDropoutSince155||in.tick<s.bowDropoutSince155)s.bowDropoutSince155=in.tick;
                if(in.tick-s.bowDropoutSince155<=kBowDropoutMs155)bowActionsR155=bowGeometryReadyR155=true;
            }
        }else s.bowDropoutSince155=0;
        if(!valid){out.resetH29[side]={1,s.gesture59c3,s.committedGestureH26.tick};out.releaseReason121[side]=1;
            out.invalidMask55c1[side]=(!p.key?1u:0u)|(!p.primary?2u:0u)|(p.side!=side?4u:0u)|
                (!in.tracked[side]?8u:0u)|(!finite(in.hands[side])?16u:0u)|(!finite(p.grip[side])?32u:0u)|
                ((!m::finite(in.shoulders[side])||!std::isfinite(in.reach[side])||in.reach[side]<=.05||in.reach[side]>=2)?64u:0u)|
                (!std::isfinite(in.grip[side])?128u:0u)|
                ((!std::isfinite(p.weight)||p.weight<=0||!std::isfinite(p.extent)||p.extent<0||!m::finite(p.tip)||!m::finite(p.axis))?256u:0u)|
                (in.attributes.live&&(!std::isfinite(in.attributes.strength)||!std::isfinite(in.attributes.agility)||!std::isfinite(in.attributes.stamina))?512u:0u);
            if(s.prepared)out.add(CommandKind::Abort,in,p);s={};continue;}
        // R59.16C4: a pending open-hand kick is released one frame after its prepare, whatever the Grip does now.
        if(side==0&&state.kickOpenRelease59c4&&in.tick>state.kickOpenRelease59c4){
            const auto beforeRelease=out.commandCount;
            out.add(CommandKind::Slash,in,p,1.0,{},3,CommandKind::Slash,-1,{});
            if(out.commandCount>beforeRelease){out.commands[out.commandCount-1].kick59=true;
                out.commands[out.commandCount-1].kickTicket59c3=state.kickOpenTicket59c4;}
            state.kickOpenRelease59c4=0;state.kickOpenTicket59c4={};
        }
        if(in.requirePrimaryGrip114&&!p.torch40()&&in.grip[side]<.25){
            out.resetH29[side]={2,s.gesture59c3,s.committedGestureH26.tick};out.releaseReason121[side]=2;
            // R59.16C4 (user decision): R3 does not need the sword Grip. With the sword hand open the weapon pose
            // is released as before, but a fresh, owner-matching kick request is still scheduled (prepare now,
            // release next frame). Same once-only/deadline/owner rules as the gripped path.
            if(side==0&&p.melee()&&in.physicalCombat177&&in.kickRequest59&&in.kickRequest59!=state.kickSeen59c3&&
               !state.kickOpenRelease59c4&&!s.prepared){
                const std::uint64_t age=in.tick>=in.kickRequest59?in.tick-in.kickRequest59:~std::uint64_t(0);
                const bool wrongOwner=in.kickTicket59c3.id&&(in.kickTicket59c3.owner.key!=p.key||
                    in.kickTicket59c3.owner.generation!=p.generation||in.kickTicket59c3.owner.body!=in.bodyEpoch);
                state.kickSeen59c3=in.kickRequest59;
                if(age<=kKickPendingMs59c2&&!wrongOwner){
                    const auto beforeKick=out.commandCount;
                    out.add(CommandKind::Prepare,in,p,0,{},3,CommandKind::Slash,-1,{});
                    if(out.commandCount>beforeKick){out.commands[out.commandCount-1].kick59=true;
                        out.commands[out.commandCount-1].kickTicket59c3=in.kickTicket59c3;
                        state.kickOpenRelease59c4=in.tick;state.kickOpenTicket59c4=in.kickTicket59c3;}
                    else{s.kickDrop59c2=4;s.kickDropTick59c2=in.tick;}
                }else{out.kickDrop59c2=wrongOwner?6:5;out.kickDropTick59c2=in.tick;}
            }
            if(s.prepared){out.add(CommandKind::Abort,in,p);s.prepared=false;}
            if(s.bowHeld){out.add(CommandKind::BowCancel,in,p);out.bow116.reason=BowReason116::PrimaryReleased;s.bowHeld=false;}
            // No attack/contact publication: retain only the current item's last
            // model-relative visual pose while native sheath ACK is pending.
            // Fresh tracking only; invalid owner/tracking/context already fails above.
            if(in.releasePresentation45&&in.intentTracked[side]&&s.ready&&s.key==p.key&&s.generation==p.generation&&finite(s.weapon)){
                if(!s.releaseVisualSince45)s.releaseVisualSince45=in.tick;
                if(in.tick>=s.releaseVisualSince45&&in.tick-s.releaseVisualSince45<=250){
                    out.active[side]=out.owned[side]=true;out.weapons[side]=s.weapon;
                    out.hands[side]=compose(s.weapon,p.grip[side]);continue;
                }
            }
            s={};continue;
        }
        s.releaseVisualSince45=0;
        const bool predictionChangedD69=s.ready&&s.handPredictionPolicyD69!=in.handPredictionPolicyD69;
        s.handPredictionPolicyD69=in.handPredictionPolicyD69;
        if(predictionChangedD69&&!s.prepared&&!s.committedGestureH26.gesture){
            // Only an unentered local nomination belongs to the old target
            // basis. Never abort or renew a native prepared/committed action.
            s.stroke118={};s.guardStroke36={};s.cocked57={};s.waitingPrepare44c1=false;
        }
        const double supportPrimaryBiasD65=p.melee()&&!p.bow()&&!p.shield176()?kcd2vr::two_hand_d65::bias():0.;
        // A live steering-policy change is a new intent basis, never a swing.
        // Keep the physical grip/collision owner; retire only its old nomination.
        if(s.support&&s.supportPrimaryBiasD65!=supportPrimaryBiasD65){
            if(s.prepared)out.add(CommandKind::Abort,in,p);
            s.prepared=false;s.waitingPrepare44c1=false;s.stroke118={};s.guardStroke36={};s.committedGestureH26={};
            s.intentReady=s.relativeIntentReady=false;s.sourcePoseH30=false;s.impactRecord178r30={};s.lastImpactTick=0;
        }
        // A native stance edge shifts the whole tracked body, not the hand's
        // physical speed. Re-express only motor request history; the previous
        // checked weapon/contact seed stays where it actually was for the sweep.
        const auto stanceDeltaR93=s.ready&&s.key==p.key&&s.generation==p.generation&&
            s.stanceFrameR93.stamp.sample==s.lastSample&&s.stanceFrameR93.stamp.tick==s.lastTick&&
            in.modelIntentFrameH8.stamp.sample==in.sample&&in.modelIntentFrameH8.stamp.tick==in.tick&&
            in.modelIntentFrameH8.stamp.epoch==in.bodyEpoch?
            tracked_frame51::shared_frame_delta_r98(in.modelIntentFrameH8,s.stanceFrameR93):Vec3{}; // R98: same frame as the hands
        s.stanceFrameR93=in.modelIntentFrameH8;
        s.lastDesired.p=s.lastDesired.p+stanceDeltaR93;
        if(s.primaryDesiredReadyH42)s.primaryDesiredH42.p=s.primaryDesiredH42.p+stanceDeltaR93;
        s.lastTip=s.lastTip+stanceDeltaR93;
        s.supportPrimaryBiasD65=supportPrimaryBiasD65;
        rebase_model_intent_frame_h8(s,in,p,side);
        const Pose intentModelHandH30=p.melee()&&!p.shield176()?model_intent_hand_h30(in,side):in.hands[side];
        const Pose intentOtherModelH30=model_intent_hand_h30(in,other);
        const Pose rawIntentWeapon104=compose(intentModelHandH30,inverse(p.grip[side]));
        Pose intentWeapon104=rawIntentWeapon104;
        Pose primary=in.hands[side];primary.p=reachable(primary.p,in.shoulders[side],in.reach[side]);
        Pose desired=compose(primary,inverse(p.grip[side]));
        // The captured native grip defines the bow-to-controller relation.
        // Preserve controller rotation here; string coupling retains its existing owner.
        const bool otherFree=!profiles[other].key&&in.tracked[other]&&finite(in.hands[other])&&m::finite(in.shoulders[other])&&std::isfinite(in.grip[other])&&std::isfinite(in.reach[other])&&in.reach[other]>.05;
        // C19C3: explicit two-hand Grip is a grasp, not a proximity lease.
        // Current tracking/context/item authority still gates every frame.
        const bool manualLatch19c3=in.supportAxis59c16&&!in.autoSupport&&p.twoHanded&&p.melee()&&!p.bow();
        if(manualLatch19c3&&s.support){
            const double graspAge=kcd2vr::motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs);
            if(graspAge<0||graspAge>.080000001){s.support=false;s.supportRearm=true;s.supportGripReady169r2=false;}
        }
        const auto supportContinuity=support_continuity5910c1(s,in,p,otherFree,primary);
        const auto impalePending59=kcd2vr::impale_pending59::Pending::capture(s,in,p,side);
        const auto retainSupportPose=[&](){
            if(!supportContinuity.keep)return;
            desired.q=*m::unit(m::multiply(supportContinuity.correction,desired.q));
            desired.p=primary.p-m::rotate(desired.q,p.grip[side].p);
        };
        const auto publishRetainedSupport=[&](Pose weapon){
            if(!s.support||!s.supportGripReady169r2)return;
            const auto hand=compose(weapon,s.supportGrip169r2);
            if(m::length(hand.p-in.shoulders[other])<=in.reach[other]+.001&&(manualLatch19c3||m::length(hand.p-in.hands[other].p)<=.24)){
                out.hands[other]=out.displayHands[other]=hand;out.owned[other]=true;out.support[side]=true;
            }else if(!manualLatch19c3){s.support=false;s.supportRearm=true;s.supportGripReady169r2=false;}
        };
        // A clear prefix after display recovery is progress for the collision
        // owner, not a newly observed obstacle. Keep the current tracked visual
        // until that checked path rejoins it. Native weapons[]/hands[] and their
        // permission flags stay on the original checked result throughout.
        const auto publishDisplayRejoinR13=[&](const Contact103& contact){
            if(!s.displayRejoinR13)return;
            if(out.displayLatched[side]||!in.contact||in.contactFaultH30||!in.intentTracked[side]||
               !finite(in.intentHands[side])||contact.plane||(contact.known&&!contact.limited115)){
                s.displayRejoinR13=false;return;
            }
            out.displayRejoinR13[side]=true;out.displayWeapons[side]=desired;
            out.displayHands[side]=compose(desired,p.grip[side]);
            if(out.owned[other]&&out.support[side]&&s.support&&s.supportGripReady169r2){
                const auto hand=compose(desired,s.supportGrip169r2);
                if(m::length(hand.p-in.shoulders[other])<=in.reach[other]+.001){
                    out.displayRejoinR13[other]=true;out.displayHands[other]=hand;
                }
            }
        };
        struct RecoveryBasisH30 {Pose model{},relative{};bool paired=false,stable=false;Vec3 supportModel{},supportRelative{};};
        const auto recoveryBasisH30=[&](){
            Pose relativeH30=compose(in.intentHands[side],inverse(p.grip[side]));
            Pose modelH30=rawIntentWeapon104;
            bool pairH30=false;
            const bool pairedH30=manualLatch19c3&&supportContinuity.keep&&in.intentTracked[other]&&
                finite(in.intentHands[other])&&m::length(s.supportGrip169r2.p-p.grip[side].p)>=.035&&
                m::length(intentOtherModelH30.p-intentModelHandH30.p)>=.035&&
                m::length(in.intentHands[other].p-in.intentHands[side].p)>=.035;
            if(pairedH30){
                const Vec3 axisH30=s.supportGrip169r2.p-p.grip[side].p;
                const double maximumH30=s.supportAxis59c16?3.0:std::clamp(in.supportConstraint130?std::max(in.supportMaxAngle,55.0*3.14159265358979323846/180.0):in.supportMaxAngle,0.0,1.05);
                const auto steerH30=[&](Pose root,Pose wrist,Vec3 otherHand){
                    const auto rawAxis=m::rotate(root.q,axisH30);
                    const auto align=m::from_to(rawAxis,kcd2vr::two_hand_d65::target(rawAxis,otherHand-wrist.p,s.supportPrimaryBiasD65));
                    root.q=*m::unit(m::multiply(limit_toward({},align,maximumH30),root.q));
                    root.p=wrist.p-m::rotate(root.q,p.grip[side].p);return root;};
                modelH30=steerH30(modelH30,intentModelHandH30,intentOtherModelH30.p);
                relativeH30=steerH30(relativeH30,in.intentHands[side],in.intentHands[other].p);
                pairH30=s.supportIntent63;
            }
            // A support-family edge is a new basis, not a strike increment.
            if(pairedH30!=s.supportIntent63&&s.intentReady&&s.relativeIntentReady){
                s.intentTip=point(modelH30,p.tip);s.relativeIntentTip=point(relativeH30,p.tip);
                s.intentHand=intentModelHandH30.p;s.relativeIntentHand=in.intentHands[side].p;
                s.committedGestureH26={};}
            const Vec3 supportModelH30=pairH30?intentOtherModelH30.p-s.supportIntentHand63:Vec3{};
            const Vec3 supportRelativeH30=pairH30?in.intentHands[other].p-s.supportRelativeIntentHand63:Vec3{};
            return RecoveryBasisH30{modelH30,relativeH30,pairedH30,pairH30,supportModelH30,supportRelativeH30};
        };
        // Valid tracking + exact grip, but no anatomically feasible collision
        // pose: publish current direct handling only. Keep the collision seed
        // separate so re-entry must still traverse a checked path. No stale pose,
        // impact, guard, or damage authority is granted by this display fallback.
        const auto handlingRecovery55c1=[&](bool unknownRecovery59c1=false){
            if((!in.physicalCombat177&&!unknownRecovery59c1)||!in.controllerAnchor119||!in.contact||!p.melee()||!in.intentTracked[side])return false;
            const bool normalH30=normal_intent_enabled_h30(in)&&in.coherentMelee119&&in.forwardOnly19&&!in.motionOwned59c5;
            out.resetH29[side]={128,s.gesture59c3,s.committedGestureH26.tick};
            if(!normalH30){
                if(s.prepared)out.add(CommandKind::Abort,in,p);
                s.prepared=false;s.stroke118={};s.spent=true;s.released44c1=false;s.waitingPrepare44c1=false;s.committedGestureH26={};
                s.guard117=false;s.guardEntry30c1={};s.intentReady=s.relativeIntentReady=false;
            }
            supportContinuity.restore(s);
            if(!normalH30||in.contactFaultH30||!s.impactRecord178r30.valid||
               in.tick<s.impactRecord178r30.tick||in.tick-s.impactRecord178r30.tick>250){
                s.impactRecord178r30={};s.lastImpactTick=0;}
            // UNKNOWN/unreachable display ownership cannot supply actor impact,
            // contact-led commit, haptics, or a new native hit. Only raw intent
            // advances below; the prior checked collision seed remains untouched.
            out.contactKnown[side]=false;out.contact119[side]={};out.blocked[side]=false;
            out.impactRecord178r30[side]={};out.impactSpeed[side]=0;out.impactEventTick178r24[side]=0;
            if(normalH30){
                const double sourceDtH30=kcd2vr::motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs);
                const auto basisH30=recoveryBasisH30();
                normal_intent_prediction_d69(state,in,p,s,out,side,std::clamp(sourceDtH30,0.,.05),rawIntentWeapon104,
                    basisH30.model,basisH30.relative,desired,basisH30.stable,basisH30.supportModel,basisH30.supportRelative,predictionChangedD69,true);
                s.supportIntent63=basisH30.paired;s.supportIntentHand63=intentOtherModelH30.p;
                s.supportRelativeIntentHand63=in.intentHands[other].p;
                if(!out.normalIntentH30[side].current){s.impactRecord178r30={};s.lastImpactTick=0;}
            }
            s.acquiring124=true;s.displayRejoinR13=true;s.lastTick=in.tick;s.timeNs=in.timeNs;s.lastDesired=desired;s.lastSample=in.sample;
            s.poseTime=in.poseTime[side];s.poseTimeEpoch=in.poseTimeEpoch;s.handPredictionPolicyD69=in.handPredictionPolicyD69;
            out.handlingOnly55c1[side]=true;out.active[side]=out.owned[side]=true;
            out.weapons[side]=out.displayWeapons[side]=desired;
            out.hands[side]=out.displayHands[side]=compose(desired,p.grip[side]);
            publishRetainedSupport(desired);
            // No contact-qualified pose here. Retain only the already admitted
            // unpinned request so native death-body observation can finish;
            // the native side refuses anchor acquisition/drive in this mode.
            impalePending59.publish(out,side);
            return true;
        };
        // H42: lastDesired is a support-/load-steered motor target, not a
        // primary tracking sample. Compare the two pre-support requests instead.
        // This also covers a retained grasp whose support wrist is temporarily
        // unreachable: that frame's motor target correctly falls back to primary.
        const Pose priorPrimaryH42=manualLatch19c3&&s.primaryDesiredReadyH42?s.primaryDesiredH42:s.lastDesired;
        // D64: melee continuity belongs to the captured hand grip, not the
        // mesh origin. A long shaft rotates its origin by >40cm with a still
        // wrist. Preserve the accepted bow/shield reset policy unchanged.
        const double positionJumpH42=p.melee()&&!p.shield176()?
            m::length(point(desired,p.grip[side].p)-point(priorPrimaryH42,p.grip[side].p)):
            m::length(desired.p-priorPrimaryH42.p);
        const double angleJumpH42=m::angle(desired.q,priorPrimaryH42.q);
        struct PrimaryRequestHistoryH42 {
            WeaponState& state;Pose value;std::uint64_t key,generation;
            ~PrimaryRequestHistoryH42(){
                if(state.ready&&state.key==key&&state.generation==generation){
                    state.primaryDesiredH42=value;state.primaryDesiredReadyH42=true;
                }
            }
        } primaryHistoryH42{s,desired,p.key,p.generation};
        const bool reset=!s.ready||s.acquiring124||s.key!=p.key||s.generation!=p.generation||kcd2vr::motion_time::discontinuity(s.lastTick,in.tick,s.timeNs,in.timeNs)||
            positionJumpH42>.40||angleJumpH42>1.65;
        if(reset){
            const WeaponState intentBeforeH30=s;
            const double intentDtH30=kcd2vr::motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs);
            const bool preserveIntentH30=normal_intent_enabled_h30(in)&&in.coherentMelee119&&in.forwardOnly19&&!in.motionOwned59c5&&
                !in.contactFaultH30&&model_intent_current_h30(in,side)&&in.attacks&&in.intentTracked[side]&&finite(in.intentHands[side])&&
                s.key==p.key&&s.generation==p.generation&&s.ready&&s.intentReady&&s.relativeIntentReady&&
                intentDtH30>0&&intentDtH30<=.080000001&&
                finite(intentModelHandH30);
            const auto committedBeforeH29=s.committedGestureH26;const auto strokeBeforeH29=s.stroke118;
            const double sourceDtH29=kcd2vr::motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs);
            const double positionJumpH29=positionJumpH42,angleJumpH29=angleJumpH42;
            const bool sourceContinuousH29=sourceDtH29>0&&sourceDtH29<=.080000001&&in.intentTracked[side]&&finite(in.intentHands[side])&&
                s.intentReady&&s.relativeIntentReady&&m::length(intentModelHandH30.p-s.intentHand)<.25&&
                m::length(in.intentHands[side].p-s.relativeIntentHand)<.25&&m::angle(in.intentHands[side].q,s.lastTrackingRotationH29)<1.;
            const bool retainCommitH29=committedBeforeH29.gesture&&s.ready&&!s.acquiring124&&!s.guard117&&p.melee()&&in.attacks&&
                s.key==p.key&&s.generation==p.generation&&!strokeBeforeH29.active&&!s.prepared&&sourceContinuousH29&&
                committedBeforeH29.key==p.key&&committedBeforeH29.generation==p.generation&&committedBeforeH29.epoch==in.bodyEpoch&&committedBeforeH29.request==in.epoch&&
                in.tick>=committedBeforeH29.tick&&in.tick-committedBeforeH29.tick<=160;
            out.resetH29[side]={unsigned((!s.ready?4:0)|(s.acquiring124?8:0)|(s.key!=p.key?16:0)|(s.generation!=p.generation?32:0)|
                (kcd2vr::motion_time::discontinuity(s.lastTick,in.tick,s.timeNs,in.timeNs)?64:0)|(positionJumpH29>.40?256:0)|(angleJumpH29>1.65?512:0)),s.gesture59c3,committedBeforeH29.tick,sourceDtH29,positionJumpH29,angleJumpH29,false};
            retainSupportPose();
            // A timing/pose discontinuity cancels gesture history, but must not
            // pull an already-owned weapon back to a newly animated native pose.
            // Re-enter contact from its last allowed WORLD pose. A new item or
            // actual ownership/tracking loss still requires the native seed.
            const double age124=kcd2vr::motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs);
            const bool retained119=in.controllerAnchor119&&s.ready&&s.key==p.key&&s.generation==p.generation&&finite(s.weapon)&&in.haveWorld==state.haveWorld&&
                (!s.acquiring124||(age124>=0&&age124<=.080000001));
            Pose seed119=retained119?s.weapon:p.nativeWeapon;
            if(retained119&&in.haveWorld){seed119.p=seed119.p*state.worldScale;
                seed119=compose(inverse(in.world),compose(state.world,seed119));seed119.p=seed119.p*(1.0/in.worldScale);}
            s={};s.displayRejoinR13=retained119&&intentBeforeH30.displayRejoinR13;
            s.acquisitionPresentationD48=retained119&&intentBeforeH30.acquisitionPresentationD48;s.supportPrimaryBiasD65=supportPrimaryBiasD65;s.ready=true;s.nativeBowMode146=in.nativeBowTrigger126;s.bowGripMode47c2=in.bowGripDraw47c2;s.key=p.key;s.generation=p.generation;s.weapon=s.lastDesired=desired;
            s.poseTime=in.poseTime[side];s.poseTimeEpoch=in.poseTimeEpoch;s.handPredictionPolicyD69=in.handPredictionPolicyD69;
            if(in.intentTracked[side]&&finite(in.intentHands[side]))s.lastTrackingRotationH29=in.intentHands[side].q;
        s.lastTick=in.tick;s.timeNs=in.timeNs;s.lastSample=in.sample;s.lastTip=point(desired,p.tip);
            supportContinuity.restore(s);
            impalePending59.restore(s);
            if(preserveIntentH30)restore_normal_intent_h30(s,intentBeforeH30);
            if(in.contact&&(retained119||p.nativeSeed)&&finite(seed119)&&p.geometry){
                // R28 F2/7: complete the existing bounded sweep when a same-item
                // controller return was already published after native ownership.
                // No desired/display pose is promoted without actual query coverage.
                Profile seedProfileR28=p;
                seedProfileR28.nativeRejoinSweepR28=in.nativeRejoinR28[side]&&!retained119&&
                    !intentBeforeH30.ready&&p.melee()&&in.controllerAnchor119&&
                    in.intentTracked[side]&&finite(in.intentHands[side])&&!in.contactFaultH30;
                // D45: query the native-to-grip arc with the original 4mm chord
                // on a fresh held binding, at most 24 segments/72 shared rays and
                // the unchanged 1000us ceiling. Ordinary moves retain 18 rays.
                // This is one attempt per binding; UNKNOWN cannot renew it.
                const bool newIdentityD44=(!intentBeforeH30.ready&&!intentBeforeH30.acquiring124)||
                    intentBeforeH30.key!=p.key||intentBeforeH30.generation!=p.generation;
                auto& attemptD45=state.acquisitionAttemptD45[side];
                const bool notAttemptedD45=attemptD45.key!=p.key||attemptD45.generation!=p.generation||attemptD45.entity!=p.entity;
                seedProfileR28.initialAcquireSweepD44=notAttemptedD45&&!seedProfileR28.nativeRejoinSweepR28&&!retained119&&newIdentityD44&&
                    p.key&&p.generation&&p.entity&&p.nativeSeed&&initial_acquire_shape_d72(p)&&!p.shield176()&&in.epoch&&in.bodyEpoch&&
                    in.controllerAnchor119&&in.intentTracked[side]&&finite(in.intentHands[side])&&!in.contactFaultH30;
                if(seedProfileR28.initialAcquireSweepD44)attemptD45={p.key,p.generation,p.entity};
                out.nativeRejoinSweepR28[side]=seedProfileR28.nativeRejoinSweepR28;
                out.initialAcquireSweepD44[side]=seedProfileR28.initialAcquireSweepD44;
                const auto seedContact=contact103(sweep(seedProfileR28,seed119,desired,false));
                out.contact119[side]=seedContact;
                out.contactKnown[side]=seedContact.known&&!seedContact.limited115;out.blocked[side]=seedContact.fraction<.999;
                out.sweepDeferred115[side]=seedContact.limited115;
                out.contactPrefixKnown126[side]=seedContact.known&&seedContact.limited115&&seedContact.fraction>0;
                double allowed=seedContact.fraction;if(!std::isfinite(allowed))allowed=0;
                s.weapon=seedContact.targetPlacementD70?desired:blend_weapon116(p,seed119,desired,std::clamp(allowed,0.0,1.0));
                out.checkedPath60[side].begin(seedContact.targetPlacementD70?desired:seed119,s.weapon,seedContact.known);
                primary=compose(s.weapon,p.grip[side]);s.lastTip=point(s.weapon,p.tip);
                // D48: query-budget prefixes and UNKNOWN do not describe a wall
                // at the native animation's wrist. Keep that checked seed for
                // collision acquisition, but never publish it as the VR wrist.
                // The new binding may present its exact current grip; no attack,
                // guard, impact or contact authority comes from this display.
                const bool firstBindingD48=(!retained119&&p.nativeSeed)||s.acquisitionPresentationD48;
                if(firstBindingD48&&p.key&&p.generation&&p.entity&&in.epoch&&in.bodyEpoch&&
                   in.controllerAnchor119&&in.intentTracked[side]&&finite(in.intentHands[side])&&!in.contactFaultH30&&!seedContact.plane&&
                   (!seedContact.known||seedContact.limited115)&&initial_acquire_shape_d72(p)){
                    s.acquiring124=s.acquisitionPresentationD48=s.displayRejoinR13=true;
                    out.acquisitionPresentationD48[side]=out.displayRejoinR13[side]=true;
                    out.handlingOnly55c1[side]=true;out.active[side]=out.owned[side]=true;
                    out.weapons[side]=s.weapon;out.displayWeapons[side]=desired;
                    out.hands[side]=out.displayHands[side]=compose(desired,p.grip[side]);
                    out.contactKnown[side]=false;out.blocked[side]=false;
                    out.impactSpeed[side]=0;out.impactRecord178r30[side]={};
                    out.releaseReason121[side]=seedContact.known?6u:5u;
                    continue;
                }
                s.acquisitionPresentationD48=false;
                if(!seedContact.known&&handlingRecovery55c1(true)){
                    out.releaseReason121[side]=5;continue;
                }
                // C10R2: an old checked WORLD seed can become unreachable after
                // Henry moves. Repeatedly rotating that seed toward the hand
                // can hit a wall at fraction zero forever. The ordinary path
                // already tries checked outward/tangential translation; allow
                // one such attempt during melee reacquisition too. The retained
                // seed is never replaced by the unswept display/controller pose.
                const auto wallResponseH4=retained119?checked_wall_response_h4(in,p,seedContact,desired,s.weapon,out.checkedPath60[side],1,sweep):WallResponseH4{};
                if(wallResponseH4.handled){
                    primary=compose(s.weapon,p.grip[side]);s.lastTip=point(s.weapon,p.tip);
                    out.wallBackout58[side]=wallResponseH4.backout;out.wallSlid58[side]=wallResponseH4.slid;out.wallBlocked58[side]=true;
                    if(wallResponseH4.limited){out.contactKnown[side]=false;out.sweepDeferred115[side]=true;out.contactPrefixKnown126[side]=true;out.contact119[side].limited115=true;}
                }
                if(!wallResponseH4.handled&&retained119&&in.physicalCombat177&&p.melee()&&
                   m::length(primary.p-in.shoulders[side])>in.reach[side]+.001){
                    bool backout=false;
                    const auto delta=wall_recovery_step59c10r2(seedContact,
                        point(desired,p.grip[side].p)-primary.p,true,backout);
                    if(m::length(delta)>.001){
                        Pose candidate=s.weapon;candidate.p=candidate.p+delta;Pose sprungH31{};
                        const bool springH31=checked_wall_spring_h31(in,p,seedContact,s.weapon,desired,delta,backout,sprungH31,sweep);
                        const auto checked=springH31?Contact103{}:contact103(sweep(p,s.weapon,candidate,false));
                        if(springH31)candidate=sprungH31;
                        if(checked.known&&checked.fraction>0){
                            s.weapon=blend_weapon116(p,s.weapon,candidate,checked.fraction);
                            out.checkedPath60[side].append(s.weapon);
                            primary=compose(s.weapon,p.grip[side]);s.lastTip=point(s.weapon,p.tip);
                            out.wallBackout58[side]=backout&&checked.fraction>.01;
                            out.wallSlid58[side]=!backout&&checked.fraction>.01;
                            out.wallBlocked58[side]=true;
                            if(checked.limited115){
                                out.contactKnown[side]=false;out.sweepDeferred115[side]=true;
                                out.contactPrefixKnown126[side]=true;out.contact119[side].limited115=true;
                            }
                        }
                    }
                }
                if(m::length(primary.p-in.shoulders[side])>in.reach[side]+.001){
                    out.releaseReason121[side]=3;
                    out.acquisitionReachError124[side]=m::length(primary.p-in.shoulders[side])-in.reach[side];
                    // A clear bounded prefix may still be outside the VR arm's
                    // reach. Preserve ONLY that checked progress for the next
                    // fresh sample instead of restarting from native forever.
                    // This grants no visible ownership, intent or hit. Unknown
                    // queries and a real wall never advance the seed.
                    if(!handlingRecovery55c1()){if(in.controllerAnchor119&&seedContact.known){s.acquiring124=true;}else s={};}
                    continue;
                }
            }
            if(!preserveIntentH30){
            s.holdRearmH26=in.trigger[side]>.20;s.wasBlocked=out.blocked[side];s.gripRearm=in.grip[side]>.2;s.bowRearm=(bow_uses_trigger175(in)?in.trigger[other]:in.grip[other])>.2;s.spent=in.heldMotionAttacks;
            s.intentTip=point(intentWeapon104,p.tip);s.intentHand=intentModelHandH30.p;s.intentReady=true;
            const Pose relHand=(in.intentTracked[side]&&finite(in.intentHands[side]))?in.intentHands[side]:in.hands[side];
            const Pose relWeapon=compose(relHand,inverse(p.grip[side]));
            s.relativeIntentTip=point(relWeapon,p.tip);s.relativeIntentHand=relHand.p;s.relativeIntentReady=true;
            s.lastTrackingRotationH29=relHand.q;
            s.sourceModelFrameH8=in.modelIntentFrameH8;s.sourceModelCalibrationH8=in.modelIntentCalibrationH8;
            s.guard117=in.naturalCombat117&&natural_guard117(in,p,s.weapon,false);
            if(in.positionGuard176&&(release_combat_r92::guardDwell(in.guardEnterDwellMs178r29,p.shield176())||in.guardEnterMaxSpeed178r29>0)){s.guard117=false;s.guardEntry30c1={};} // fresh acquisition must earn posture dwell
            out.guardBySide30c1[side]=s.guard117;
            s.natural118=in.naturalCombat117||in.coherentMelee119;s.stroke118.ready=s.guard117;
            if(side==0)out.guard117=s.guard117;else if(in.positionGuard176&&p.shield176())out.guard117=out.guard117||s.guard117;
            }
            // Legacy checked rebase retains only an already committed identity.
            // Normal intent has its separate source continuity above and advances
            // once below; collision recovery never supplies a new hit.
            if(retainCommitH29&&out.contactKnown[side]&&!out.sweepDeferred115[side]&&!s.guard117){
                s.committedGestureH26=committedBeforeH29;s.gesture59c3=committedBeforeH29.gesture;
                s.lastAttack=committedBeforeH29.tick;s.released44c1=true;s.stroke118=strokeBeforeH29;
                s.spent=true;out.resetH29[side].retained=true;
            }
            out.weapons[side]=s.weapon;out.active[side]=out.owned[side]=true;out.hands[side]=primary;publishRetainedSupport(s.weapon);impalePending59.publish(out,side);
            if(preserveIntentH30){
                const auto basisH30=recoveryBasisH30();
                normal_intent_prediction_d69(state,in,p,s,out,side,std::clamp(intentDtH30,0.,.05),rawIntentWeapon104,
                    basisH30.model,basisH30.relative,s.weapon,basisH30.stable,basisH30.supportModel,basisH30.supportRelative,predictionChangedD69,true);
                s.supportIntent63=basisH30.paired;s.supportIntentHand63=intentOtherModelH30.p;
                s.supportRelativeIntentHand63=in.intentHands[other].p;
                out.resetH29[side].retained=true;
                // The old actual occurrence may have missed native drain during
                // recovery. Expose it unchanged under the existing 250ms window;
                // no current ray, impact or clock is invented by reacquisition.
                if(s.impactRecord178r30.valid&&in.tick>=s.impactRecord178r30.tick&&in.tick-s.impactRecord178r30.tick<=250)
                    out.impactRecord178r30[side]=s.impactRecord178r30;
            }
            publishDisplayRejoinR13(out.contact119[side]);
            s.lastTick=in.tick;s.timeNs=in.timeNs;s.lastSample=in.sample;
            continue;}
        if(!s.bowHeld)s.nativeBowMode146=in.nativeBowTrigger126;
        const double dt=std::clamp(kcd2vr::motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs),0.0,.05);
        if(in.grip[side]<.2)s.gripRearm=false;
        if(p.bow()&&s.bowGripMode47c2!=in.bowGripDraw47c2){
            if(s.bowHeld)out.add(CommandKind::BowCancel,in,p);
            s.bowHeld=s.carry146=s.bowGrip38=false;s.bowFraction=0;s.bowCorrection38={};
            s.bowGripMode47c2=in.bowGripDraw47c2;s.bowRearm=true;
        }
        if(in.tracked[other]&&std::isfinite(in.grip[other])&&in.grip[other]<.2){if(!in.autoSupport)s.supportRearm=false;if(!bow_uses_trigger175(in))s.bowRearm=false;}
        if(bow_uses_trigger175(in)&&in.tracked[other]&&std::isfinite(in.trigger[other])&&in.trigger[other]<.20)s.bowRearm=false;
        // Retain the qualified support socket, or a bounded longsword hilt point.
        // A latched local point is immutable; no blade/shaft sliding or UNKNOWN approach.
        Pose supportLocal=p.grip[other];
        // C19C3: choose a qualified hilt/shaft point once; physical release chooses a new one.
        if(manualLatch19c3&&s.support&&(!finite(s.supportPrimary19c3)||
           m::length(s.supportPrimary19c3.p-p.grip[side].p)>=.001||m::angle(s.supportPrimary19c3.q,p.grip[side].q)>=.001)){
            s.support=false;s.supportRearm=true;s.supportGripReady169r2=false;
        }
        if(!s.support)s.supportAxis59c16=false;
        const double axisSide59c16=(s.support&&s.supportAxis59c16&&finite(s.supportGrip169r2))?m::dot(s.supportGrip169r2.p-p.grip[side].p,p.tip-p.grip[side].p):0.0;
        const AxisGrip59c16 axis59c16=axis_grip59c16(in,p,s.weapon,in.hands[other].p,axisSide59c16);
        // C39 native-first: a hand that closes within 14 cm of a sword socket or of a captured NATIVE animation frame (or inside
        // the qualified hilt region) takes that frame; anywhere else on the hilt/shaft the axis grip answers. A held socket stays a socket.
        const bool socketHeld39=s.support&&!s.supportAxis59c16;
        double socketGap39=1e3;bool socketNear39=false;
        if(!s.support&&otherFree&&p.support&&finite(p.grip[other])&&(p.classId==4||p.supportSource37==1||!axis59c16.eligible)){
            socketGap39=m::length(in.hands[other].p-point(s.weapon,p.grip[other].p));
            socketNear39=socketGap39<.14||(p.classId==4&&p.supportPositionOnly174r2&&p.supportRegion169r2.valid&&!in.autoSupport&&
                bool(kcd2vr::hilt169r2::select(p.supportRegion169r2,point(inverse(s.weapon),in.hands[other].p))));
        }
        const bool axisMode59c16=otherFree&&((s.support&&s.supportAxis59c16&&in.supportAxis59c16)||(axis59c16.eligible&&!socketHeld39&&!socketNear39));
        if(axisMode59c16){supportLocal.p=axis59c16.local;supportLocal.q=(s.support&&s.supportAxis59c16&&finite(s.supportGrip169r2))?s.supportGrip169r2.q:p.grip[side].q;}
        bool slide39=false;
        if(axisMode59c16&&s.support&&s.supportAxis59c16){
            // C42: Grip locks the selected shaft station and wrist radial offset.
            // Reposition by releasing and re-gripping; controller separation steers
            // the weapon, never silently slides the hand along it.
            supportLocal=s.supportGrip169r2; // immutable until physical release
        }
        bool regionCandidate169r2=false;
        const bool region169r2=!axisMode59c16&&!in.autoSupport&&p.supportPositionOnly174r2&&p.classId==4&&p.twoHanded&&p.supportRegion169r2.valid;
        if(s.support&&s.supportGripReady169r2&&!(axisMode59c16&&s.supportAxis59c16)){
            const bool retainedBinding=manualLatch19c3&&finite(s.supportPrimary19c3)&&
                m::length(s.supportPrimary19c3.p-p.grip[side].p)<.001&&m::angle(s.supportPrimary19c3.q,p.grip[side].q)<.001;
            const bool sameSocket=finite(s.supportGrip169r2)&&(retainedBinding||(p.support&&
                s.supportPositionOnly175==p.supportPositionOnly174r2&&
                (s.supportRegionBound175?(region169r2&&kcd2vr::hilt169r2::contains(p.supportRegion169r2,s.supportGrip169r2.p)):
                 m::length(s.supportGrip169r2.p-p.grip[other].p)<.001)));
            if(sameSocket)supportLocal=s.supportGrip169r2;
            else {s.support=false;s.supportRearm=true;s.supportGripReady169r2=false;s.supportTwistSince=0;}
        }
        if(!s.support&&region169r2&&otherFree){
            const auto selected=kcd2vr::hilt169r2::select(p.supportRegion169r2,point(inverse(s.weapon),in.hands[other].p));
            if(selected){
                // Keep the chosen hilt point throughout the short Grip dwell.
                // Hand jitter along a valid handle must not restart acquisition.
                // Leaving the qualified handle still cancels the candidate below.
                supportLocal.p=s.supportCandidateReady169r2&&kcd2vr::hilt169r2::contains(p.supportRegion169r2,s.supportCandidate169r2)?
                    s.supportCandidate169r2:*selected;
                regionCandidate169r2=true;
            }
        }
        const Pose supportPoint=compose(s.weapon,supportLocal);
        const double gap=m::length(in.hands[other].p-supportPoint.p);
        // V66.163/R2: acquire around the socket or qualified handle interval. Holding uses
        // one wider release limit before AND after the rigid constraint solve;
        // the old 26 cm / 18 cm conflict dropped a grip immediately after latch.
        constexpr double manualAcquire163=.14,manualRelease163=.24;
        if(in.autoSupport||!otherFree||!p.twoHanded||(!p.support&&!axisMode59c16)||p.bow()||in.grip[other]<.25){
            s.supportDistanceRelease59c10=false;s.supportRearmReturnSince59c10=0;
        }
        if(in.autoSupport){
            const double spacingError=std::abs(m::length(in.hands[other].p-primary.p)-m::length(supportLocal.p-p.grip[side].p));
            const bool eligible=otherFree&&p.twoHanded&&p.support&&finite(supportLocal)&&!p.bow();
            const double wristError=eligible?m::angle(supportPoint.q,in.hands[other].q):0.0;
            if(s.support&&eligible&&wristError>1.92){
                if(!s.supportTwistSince)s.supportTwistSince=in.tick;
            }else s.supportTwistSince=0;
            const bool incompatible=s.supportTwistSince&&in.tick-s.supportTwistSince>=150;
            if(s.support&&(!eligible||gap>.18||spacingError>.18||incompatible)){
                s.support=false;s.supportRearm=incompatible;s.supportCandidateSince=0;
            }
            // A rejected over-twist cannot automatically reacquire without leaving
            // the helper. Grip remains entirely available to the D-pad modifier.
            if(gap>.18)s.supportRearm=false;
            const bool withinGripZone=eligible&&!s.supportRearm&&gap<.10&&
                m::length(supportPoint.p-in.shoulders[other])<=in.reach[other];
            if(!s.support){
                if(!withinGripZone)s.supportCandidateSince=0;
                else {
                    if(!s.supportCandidateSince)s.supportCandidateSince=in.tick;
                    if(in.tick-s.supportCandidateSince>=80){
                        Pose approach=s.weapon;approach.p=approach.p+(in.hands[other].p-supportPoint.p);
                        {const auto ac=contact103(sweep(p,s.weapon,approach,true));if(!in.contact||(ac.known&&!ac.limited115&&ac.fraction>=.999))s.support=true;}
                    }
                }
            }
        }
        if(!in.autoSupport&&s.support){
            // Legacy fixed comparison keeps anti-twist/proximity release. The
            // explicit held-grasp mode releases only real ownership/Grip loss.
            const double wristError=(otherFree&&!s.supportAxis59c16)?m::angle(supportPoint.q,in.hands[other].q):0; // R16: the axis grip has no wrist socket
            if(otherFree&&wristError>1.92){if(!s.supportTwistSince)s.supportTwistSince=in.tick;}
            else s.supportTwistSince=0;
            const bool incompatible=s.supportTwistSince&&in.tick>=s.supportTwistSince&&in.tick-s.supportTwistSince>=150;
            if(!otherFree||in.grip[other]<.25||(!manualLatch19c3&&(gap>manualRelease163||(incompatible&&!in.supportV120_178r27)))){ // V120: release by Grip/distance only
                s.support=false;s.supportRearm=!(in.tracked[other]&&in.grip[other]<.25);
                s.supportDistanceRelease59c10=otherFree&&in.grip[other]>.55&&gap>manualRelease163&&
                    !(incompatible&&!in.supportV120_178r27);
                s.supportRearmReturnSince59c10=0;
                s.supportCandidateSince=0;s.supportGripReady169r2=false;s.supportTwistSince=0;
            }
        }
        if(!in.autoSupport&&!s.support&&s.supportRearm&&s.supportDistanceRelease59c10&&otherFree&&
           p.twoHanded&&!p.bow()&&in.grip[other]>.55&&
           (axisMode59c16?axis_grip_near59c16(axis59c16):(p.support&&gap<manualAcquire163&&
            (p.supportPositionOnly174r2||in.supportV120_178r27||m::angle(supportPoint.q,in.hands[other].q)<=1.92)))&&
           m::length(supportPoint.p-in.shoulders[other])<=in.reach[other]){
            // Carry-forward from Claude C10b, narrowed to distance-only release.
            // The hand returning while still squeezing must settle for150ms;
            // normal acquisition then still needs its dwell and checked path.
            if(!s.supportRearmReturnSince59c10)s.supportRearmReturnSince59c10=in.tick;
            else if(in.tick>=s.supportRearmReturnSince59c10&&in.tick-s.supportRearmReturnSince59c10>=150){
                s.supportRearm=false;s.supportRearmReturnSince59c10=0;s.supportDistanceRelease59c10=false;
            }
        }else s.supportRearmReturnSince59c10=0;
        if(!in.autoSupport&&!s.support){
            const bool manualEligible=!s.supportRearm&&otherFree&&p.twoHanded&&!p.bow()&&in.grip[other]>.55&&
                (axisMode59c16?axis_grip_near59c16(axis59c16):(p.support&&finite(p.grip[other])&&
                 (!region169r2||regionCandidate169r2)&&gap<manualAcquire163&&
                 (p.supportPositionOnly174r2||in.supportV120_178r27||m::angle(supportPoint.q,in.hands[other].q)<=1.92)))&&
                m::length(supportPoint.p-in.shoulders[other])<=in.reach[other];
            if(!manualEligible){s.supportCandidateSince=0;s.supportCandidateReady169r2=false;}
            else {
                if(region169r2&&!s.supportCandidateReady169r2){
                    s.supportCandidate169r2=supportLocal.p;s.supportCandidateReady169r2=true;s.supportCandidateSince=in.tick;
                }
                if(!s.supportCandidateSince)s.supportCandidateSince=in.tick;
                if(in.tick-s.supportCandidateSince>=20){ // V66.146R1: 40 -> 20 ms dwell
                    // Explicit Grip is a latch, not a second primary hand. Acquire only
                    // after a short stable dwell and a clear approach.
                    Pose approach=s.weapon;approach.p=approach.p+(in.hands[other].p-supportPoint.p);
                    auto acquisitionProfile=p;acquisitionProfile.grip[other]=supportLocal;
                    const auto ac=contact103(sweep(acquisitionProfile,s.weapon,approach,true));
                    if(!in.contact||(ac.known&&!ac.limited115&&ac.fraction>=.999)){
                        s.support=true;s.supportCandidateSince=0;s.supportTwistSince=0;
                        // Synthesized geometry proves position, so capture THIS
                        // opposite wrist's local orientation once at acquisition.
                        // Native opposite-hand frames keep their authored rotation.
                        if(p.supportPositionOnly174r2||axisMode59c16)
                            supportLocal.q=*m::unit(m::multiply(m::conjugate(s.weapon.q),in.hands[other].q));
                        s.supportGrip169r2=supportLocal;s.supportPrimary19c3=p.grip[side];s.supportGripReady169r2=true;
                        s.supportPositionOnly175=p.supportPositionOnly174r2;s.supportRegionBound175=region169r2;
                        s.supportAxis59c16=axisMode59c16; // R59.16C10R16
                    }
                }
            }
        }
        {   // R59.16C10: support-latch probe (observation only). Filled for the primary two-handed melee side.
            auto& pr=out.supportProbe59c10;
            if(pr.side<0&&p.twoHanded&&p.melee()&&!p.bow()){
                pr.side=side;pr.twoHanded=true;pr.otherKeyed=profiles[other].key!=0;pr.otherTracked=in.tracked[other]&&finite(in.hands[other]);
                pr.profileSupport=p.support;pr.rearm=s.supportRearm; // after this frame's release/acquire decisions
                pr.v120=in.supportV120_178r27;pr.positionOnly=p.supportPositionOnly174r2;pr.axis59c16=axisMode59c16;pr.along59c16=axis59c16.t;
                pr.mode39=s.support?(s.supportAxis59c16?(p.classId==7&&axis59c16.centre39?3:2):1):axisMode59c16?2:(p.support?1:0);(void)slide39;
                pr.centre39=axis59c16.centre39;pr.socketNear39=socketNear39;pr.perp39=axis59c16.perp;pr.tMin39=axis59c16.tMin;pr.tMax39=axis59c16.tMax;
                pr.perpLimit39=axis59c16.perpLimit39;pr.radius39=axis59c16.radius39;pr.socketGap39=socketGap39;
                pr.gap=std::isfinite(gap)?gap:1e3;pr.grip=std::isfinite(in.grip[other])?double(in.grip[other]):-1;
                pr.reachExcess=m::finite(in.shoulders[other])&&std::isfinite(in.reach[other])?m::length(supportPoint.p-in.shoulders[other])-in.reach[other]:1e3;
                if(s.support)pr.reason=0;
                else if(in.autoSupport)pr.reason=9;
                else if(!axisMode59c16&&(!p.support||!finite(p.grip[other])))pr.reason=1;
                else if(pr.otherKeyed)pr.reason=2;
                else if(!otherFree)pr.reason=3;
                else if(s.supportRearm)pr.reason=4;
                else if(!(in.grip[other]>.55))pr.reason=5;
                else if(axisMode59c16?!axis_grip_near59c16(axis59c16):!(gap<manualAcquire163))pr.reason=6;
                else if((!axisMode59c16&&((region169r2&&!regionCandidate169r2)||!(p.supportPositionOnly174r2||in.supportV120_178r27||m::angle(supportPoint.q,in.hands[other].q)<=1.92)))||pr.reachExcess>0)pr.reason=7;
                else pr.reason=8;
            }
        }
        if(s.support){
            const Vec3 localAxis=supportLocal.p-p.grip[side].p;
            const Vec3 targetAxis=kcd2vr::two_hand_d65::target(m::rotate(desired.q,localAxis),
                in.hands[other].p-primary.p,s.supportPrimaryBiasD65);
            if(m::length(localAxis)<.035||m::length(targetAxis)<.035){
                if(!manualLatch19c3){s.support=false;s.supportRearm=true;}
                else {
                    // D64: coincident tracked hands cannot define a new shaft
                    // axis. Keep the already admitted correction around the
                    // primary grip; do not snap to the unsteered wrist basis.
                    desired.q=*m::unit(m::multiply(s.supportCorrection,desired.q));
                    desired.p=primary.p-m::rotate(desired.q,p.grip[side].p);
                }
            }
            else {
                // R59.16C10R16: an axis grip is BOTH hands on one rigid weapon - no 55-degree cap, fast slew (jitter filter only).
                // V66.130: the secondary hand constrains ONE rigid weapon. It owns
                // the long-axis direction while primary wrist roll remains the roll
                // reference. This is not a second weapon writer and cannot shear the
                // hands apart. The correction is slew-limited rather than permanently
                // limited to the old eight-degree nudge.
                if(s.supportAxis59c16){
                    // A held shaft grip owns its long-axis direction continuously.
                    // Transport the preceding corrected basis onto the current
                    // target direction; an absolute shortest-arc correction at
                    // 180 degrees otherwise flips +/-172 degrees under tiny
                    // wrist changes. Current primary wrist roll still enters
                    // through desired.q; the saved grip point never slides.
                    const Quat priorSteered=*m::unit(m::multiply(s.supportCorrection,desired.q));
                    const Quat residual=m::from_to(m::rotate(priorSteered,localAxis),targetAxis);
                    const Quat next=*m::unit(m::multiply(residual,s.supportCorrection));
                    s.supportCorrection=limit_toward(s.supportCorrection,next,30.0*dt);
                }else {
                    const auto align=m::from_to(m::rotate(desired.q,localAxis),targetAxis);
                    const double maximum=std::clamp(in.supportConstraint130?std::max(in.supportMaxAngle,55.0*3.14159265358979323846/180.0):in.supportMaxAngle,0.0,1.05);
                    const Quat bounded=limit_toward({},align,maximum);
                    s.supportCorrection=limit_toward(s.supportCorrection,bounded,(in.supportConstraint130?8.0:3.0)*dt);
                }
                desired.q=*m::unit(m::multiply(s.supportCorrection,desired.q));
                // Rotate around the primary grip first. With the V130 constraint,
                // share at most 18 mm of residual secondary positional error so both
                // physical hands influence one object without midpoint rubber-banding.
                desired.p=primary.p-m::rotate(desired.q,p.grip[side].p);
                auto support=compose(desired,supportLocal);
                if(in.supportConstraint130){
                    const Vec3 shared=limited((in.hands[other].p-support.p)*.25,.018);
                    desired.p=desired.p+shared;support=compose(desired,supportLocal);
                }
                if(!manualLatch19c3&&(m::length(support.p-in.shoulders[other])>in.reach[other]||m::length(support.p-in.hands[other].p)>(in.autoSupport?.18:manualRelease163))){s.support=false;s.supportRearm=!in.autoSupport;s.supportCandidateSince=0;desired=compose(primary,inverse(p.grip[side]));}
            }
        }
        // D67: removing a shaft constraint changes the desired basis, not
        // the controller motion. Start the one-hand motor at its last CHECKED
        // pose and do not let its compliance clamp teleport to the new basis.
        const bool releaseSupportD67=kcd2vr::held_motion_d67::supportRelease()&&p.melee()&&
            !s.support&&m::angle(s.supportCorrection,Quat{})>1e-6;
        if(releaseSupportD67){
            s.supportReleaseD67=true;s.lastDesired=s.weapon;s.angularVelocity={};
            s.supportReleaseOffsetD67=*m::unit(m::multiply(s.weapon.q,m::conjugate(desired.q)));
        }
        if(s.support||!kcd2vr::held_motion_d67::supportRelease()){
            s.supportReleaseD67=false;s.supportReleaseOffsetD67={};
        }
        if(s.supportReleaseD67){
            // Retire only the old support basis. CURRENT controller rotation
            // still enters this target immediately; even continued fast input
            // cannot prolong an old correction past pi/rate seconds.
            s.supportReleaseOffsetD67=limit_toward(s.supportReleaseOffsetD67,{},
                kcd2vr::held_motion_d67::supportReleaseRate*dt);
            desired.q=*m::unit(m::multiply(s.supportReleaseOffsetD67,desired.q));
            desired.p=primary.p-m::rotate(desired.q,p.grip[side].p);
            if(m::angle(s.supportReleaseOffsetD67,Quat{})<1e-6)s.supportReleaseD67=false;
        }
        if(!s.support)s.supportCorrection={};
        const bool supportEngaged19c3=s.support&&(!manualLatch19c3||
            m::length(compose(desired,supportLocal).p-in.shoulders[other])<=in.reach[other]+.001);
        // An unreachable retained grasp is identity only: no two-hand force,
        // effort, or steering until its rigid wrist is anatomically usable.
        double supportReachWeightD85R4=supportEngaged19c3?1.:0.;
        if(s.support&&manualLatch19c3){
            const Pose single=compose(primary,inverse(p.grip[side]));
            const double margin=in.reach[other]+.001-m::length(compose(desired,supportLocal).p-in.shoulders[other]);
            // The old binary reach veto replaced a held two-hand basis with
            // the main wrist in one sample. Approach that same boundary
            // geometrically; no timer, stale pose or extra arm reach.
            const double fraction=std::clamp(margin/.04,0.,1.);
            const double weight=fraction*fraction*(3.-2.*fraction);
            supportReachWeightD85R4=weight;
            if(weight<1.){
                const Vec3 pivot=primary.p+(point(desired,p.grip[side].p)-primary.p)*weight;
                desired.q=m::slerp(single.q,desired.q,weight).value_or(single.q);
                desired.p=pivot-m::rotate(desired.q,p.grip[side].p);
            }
        }else if(s.support&&!supportEngaged19c3)desired=compose(primary,inverse(p.grip[side]));

        // C63: the visible two-hand weapon can rotate through a fast real cut
        // while the primary wrist barely rotates. Its intent must include the
        // latched support lever in BOTH independent controller families. Build
        // these poses from raw tracked wrists, before dynamics/contact, never
        // from the collision-stopped or smoothed output weapon.
        const Pose relativeHand63=(in.intentTracked[side]&&finite(in.intentHands[side]))?in.intentHands[side]:intentModelHandH30;
        Pose relativeWeapon63=compose(relativeHand63,inverse(p.grip[side]));
        const Vec3 intentLocalAxis63=supportLocal.p-p.grip[side].p;
        const bool pairedIntent63=manualLatch19c3&&supportEngaged19c3&&s.supportGripReady169r2&&
            in.intentTracked[side]&&in.intentTracked[other]&&finite(in.intentHands[other])&&
            m::length(intentLocalAxis63)>=.035&&m::length(intentOtherModelH30.p-intentModelHandH30.p)>=.035&&
            m::length(in.intentHands[other].p-relativeHand63.p)>=.035;
        const bool stablePair63=pairedIntent63&&s.supportIntent63;
        if(pairedIntent63){
            const double maximum=s.supportAxis59c16?3.0:std::clamp(in.supportConstraint130?std::max(in.supportMaxAngle,55.0*3.14159265358979323846/180.0):in.supportMaxAngle,0.0,1.05);
            const auto steer63=[&](Pose root,Pose wrist,Vec3 otherHand){
                const auto rawAxis=m::rotate(root.q,intentLocalAxis63);
                const auto align=m::from_to(rawAxis,kcd2vr::two_hand_d65::target(rawAxis,otherHand-wrist.p,s.supportPrimaryBiasD65));
                root.q=*m::unit(m::multiply(limit_toward({},align,maximum),root.q));
                root.p=wrist.p-m::rotate(root.q,p.grip[side].p);return root;
            };
            intentWeapon104=steer63(intentWeapon104,intentModelHandH30,intentOtherModelH30.p);
            relativeWeapon63=steer63(relativeWeapon63,relativeHand63,in.intentHands[other].p);
        }
        // Acquisition/release changes which raw inputs steer the blade, but
        // that family change itself is never movement or an attack increment.
        if(pairedIntent63!=s.supportIntent63&&s.intentReady&&s.relativeIntentReady){
            s.intentTip=point(intentWeapon104,p.tip);s.relativeIntentTip=point(relativeWeapon63,p.tip);
            s.intentHand=intentModelHandH30.p;s.relativeIntentHand=relativeHand63.p;
            s.committedGestureH26={};
        }
        if(predictionChangedD69){
            // Basis changes are not physical increments in either raw family.
            s.intentTip=point(intentWeapon104,p.tip);s.relativeIntentTip=point(relativeWeapon63,p.tip);
            s.intentHand=intentModelHandH30.p;s.relativeIntentHand=relativeHand63.p;
        }
        auto& committedH26=s.committedGestureH26;
        if(!in.attacks||!p.melee()||!in.intentTracked[side]||
           !(in.naturalCombat117||in.coherentMelee119)||
           committedH26.key!=p.key||committedH26.generation!=p.generation||
           committedH26.epoch!=in.bodyEpoch||committedH26.request!=in.epoch||
           in.tick<committedH26.tick||(!normal_hybrid_intent_h31(in)&&in.tick-committedH26.tick>160))committedH26={};
        const double committedDtH8=motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs);
        if(!predictionChangedD69)AdvanceCommittedGestureH8(in,p,s,side,intentWeapon104,relativeWeapon63,
            s.intentReady&&s.relativeIntentReady&&committedDtH8>0&&committedDtH8<=.080000001);
        const Vec3 modelMotionH26=point(intentWeapon104,p.tip)-s.intentTip;
        const Vec3 trackingMotionH26=point(relativeWeapon63,p.tip)-s.relativeIntentTip;
        const double modelSpeedH26=dt>0?m::length(modelMotionH26)/dt:0;
        const double trackingSpeedH26=dt>0?m::length(trackingMotionH26)/dt:0;
        const bool retainedGestureH26=committedH26.gesture&&s.intentReady&&s.relativeIntentReady&&
            !s.guard117&&(!in.requirePrimaryGrip114||in.grip[side]>.25)&&
            modelSpeedH26>.25&&std::isfinite(modelSpeedH26)&&trackingSpeedH26>.25&&std::isfinite(trackingSpeedH26)&&
            (normal_hybrid_intent_h31(in)?(committedH26.forwardTickH8==in.tick&&committedH26.forwardSampleH8==in.sample):
             (m::dot(m::unit(modelMotionH26).value_or(Vec3{}),committedH26.modelDirection)>.25&&
              m::dot(m::unit(trackingMotionH26).value_or(Vec3{}),committedH26.trackingDirection)>.25));
        const Vec3 supportHandDelta63=stablePair63?intentOtherModelH30.p-s.supportIntentHand63:Vec3{};
        const Vec3 supportRelativeDelta63=stablePair63?in.intentHands[other].p-s.supportRelativeIntentHand63:Vec3{};
        s.supportIntent63=pairedIntent63;s.supportIntentHand63=intentOtherModelH30.p;
        s.supportRelativeIntentHand63=in.intentHands[other].p;

        // R39R1C1: right Grip couples the drawing hand to ONE bow, like the
        // support grip of a weapon. Left wrist retains pivot and roll authority.
        // Solve distance geometrically; lateral hand motion steers the bow rather
        // than translating/rotating the nocked arrow independently of its rest.
        bool bowGripAcquired19c4r2=false;
        (void)upgrade_bow_nock43(s,p);
        const bool bowCouple38=p.bow()&&physical_bow175(in)&&s.bowHeld&&!s.carry146&&
            bowActionsR155&&otherFree&&(in.grip[0]>=.25||(in.bowGripDraw47c2&&s.bowGrip38));
        if(!bowCouple38){s.bowGrip38=false;s.bowCorrection38={};}
        else {
            if(!s.bowGrip38&&in.grip[0]>=.55){
                s.bowGrip38=true;bowGripAcquired19c4r2=true;
                // Capture only after this frame's allowed root is solved below.
                // The prior root can differ during the initial draw/aim motion.
            }
            if(s.bowGrip38){
                const Vec3 base=s.nock+s.bowAcquireOffset121-p.grip[side].p;
                const auto axis=m::unit(s.drawAxis);
                const Vec3 target=in.hands[0].p-primary.p;
                const double span=m::length(target),a=axis?m::dot(base,*axis):0;
                const double square=span*span-(m::dot(base,base)-a*a);
                if(!axis||span<.04||square<0){s.bowGrip38=false;s.bowCorrection38={};}
                else {
                    s.bowPull38=std::max(0.0,std::sqrt(square)-a);
                    const Vec3 local=base+*axis*s.bowPull38;
                    const Quat correction=m::from_to(m::rotate(desired.q,local),target);
                    // Primary hand dominates; no abrupt snap on acquisition.
                    const Quat bounded=limit_toward({},correction,1.05);
                    s.bowCorrection38=limit_toward(s.bowCorrection38,bounded,8.0*dt);
                    desired.q=*m::unit(m::multiply(s.bowCorrection38,desired.q));
                    desired.p=primary.p-m::rotate(desired.q,p.grip[side].p);
                }
            }
        }
        if(predictionChangedD69){s.lastDesired=desired;s.linearVelocity={};s.angularVelocity={};}
        Pose wanted=desired;
        bool anchoredRejoinD70=false;Vec3 rejoinControllerVelocityD70{};
        if(in.dynamics&&p.geometry&&!p.bow()&&!p.torch40()){
            const double targetDt=motion_time::target_elapsed(s.poseTime,in.poseTime[side],s.poseTimeEpoch,in.poseTimeEpoch);
            const double driveDt=targetDt>0?std::min(targetDt,.05):dt;
            // V66.102 "a little drag": the weapon trails the hand by a few centimetres
            // (critically damped, NO overshoot, no board-on-a-pole rotation). Heavier
            // weapons / low strength / low stamina trail more; a two-handed grip halves it.
            // Model-relative drive; world contact still includes locomotion. Coherent timing
            // is independently required and walking is not an attack signal.
            const double exhaustion59c14=std::max(exhaustionScale59c7b(in.attributes),
                kcd2vr::dynamics177::fatigueRamp59c14(in.attributes.stamina,in.attributes.live,in.fatigueRamp59c14)); // R59.16C10R14 ramp + C7B floor
            const auto handling59c14=kcd2vr::dynamics177::handling59c14(p.classId,supportEngaged19c3,in.poleOneHand59c14);
            const bool physical177=in.physicalCombat177&&p.melee()&&in.controllerAnchor119&&in.constraintDrive129;
            const bool rigidPolicyD69=kcd2vr::weapon_trials_d69::rigidGrip()&&p.melee()&&!p.shield176()&&!p.bow();
            if(rigidPolicyD69!=s.rigidGripD69){
                // Policy handover keeps the checked pose/contact/support owner.
                // Only obsolete motor momentum is retired, never raw intent.
                s.rigidGripD69=rigidPolicyD69;s.linearVelocity={};s.angularVelocity={};s.lastDesired=desired;
            }
            const bool rigidClearD69=rigidPolicyD69&&!(in.contact&&(s.wasBlocked||s.contactRejoinR9));
            // The physical path already uses the full Body below: never apply
            // these equivalent loads twice or add more work to that drive.
            const auto loadsH2=physical177?kcd2vr::dynamics177::HandlingLoadsH2{p.weight,p.weight}:
                kcd2vr::dynamics177::handlingLoadsH2(p.classId,p.weight,
                    m::length(p.tip-p.grip[side].p)*(in.haveWorld?in.worldScale:1.));
            const double mass=std::clamp(loadsH2.linear*exhaustion59c14/handling59c14.authority,.15,15.0);
            const double angularMassH2=std::clamp(loadsH2.angular*exhaustion59c14/handling59c14.authority,.15,15.0);
            const double strength=in.attributes.live?std::clamp(in.attributes.strength/10.0,.35,3.0):1;
            const double dex=in.attributes.live?std::clamp(in.attributes.agility/10.0,.5,2.0):1;
            const double stamina=in.attributes.live?std::clamp(in.attributes.stamina,0.0,1.0):1;
            const double load=mass/(strength*(.55+.45*stamina));
            double tau=std::clamp(.004+.002*load,.004,.025);if(supportEngaged19c3)tau*=.5; // ~3 cm behind at 2 m/s for a 2 kg sword
            double angularTauH2=std::clamp(.004+.002*angularMassH2/(strength*(.55+.45*stamina)),.004,.025);
            if(supportEngaged19c3)angularTauH2*=.5;
            const double tauRot=std::clamp(angularTauH2*.30/dex,.004,.018);
            const double w=1.0/tau;
            const double metricScale177=in.haveWorld?in.worldScale:1;
            kcd2vr::dynamics177::Body body177{};kcd2vr::dynamics177::Authority authority177{};
            Vec3 axis177{},gravity177{},previousPivot177{},targetVelocity177{},targetOmega177{};
            // R59.16C10R17: requirement-relative RPG stats (see dynamics177::stats59c17). With VrWeaponStatEffect > 0 the base
            // authority is the neutral design profile (stamina is NOT counted twice: only the exhaustion divisor below).
            kcd2vr::dynamics177::Sim26 sim26{};const bool simOn26=kcd2vr::dynamics177::sim26(in.statSim26,sim26); // C26c
            const int slot26=kcd2vr::dynamics177::skillSlot26(p.classId);
            const int skill26=simOn26?sim26.skill:(slot26>=0&&in.attributes.live?in.attributes.skills26[slot26]:-1);
            const auto stats59c17=kcd2vr::dynamics177::rpg26(kcd2vr::dynamics177::stats59c17(
                simOn26?sim26.strength:in.attributes.strengthEffective>=0?in.attributes.strengthEffective:in.attributes.strength,
                simOn26?sim26.agility:in.attributes.agilityEffective>=0?in.attributes.agilityEffective:in.attributes.agility,
                p.strReq59c17,p.agiReq59c17,in.statEffect59c17,in.attributes.live||simOn26),skill26);
            if(physical177){
                body177=kcd2vr::dynamics177::body(p.classId,p.weight,m::length(p.tip-p.grip[side].p)*metricScale177);
                // C32: a body hanging on the blade = a point mass at the stuck point (moves the centre of mass toward the tip)
                const bool hanging44=in.impaleHang32&&in.impalePinMode40!=1&&s.impale178r15&&s.impalePinned178r18;
                if(const double load32=kcd2vr::gore32::hangLoadKg(p.weight,hanging44);load32>0){
                    const double d=std::clamp(m::length(s.impaleLocal32-p.grip[side].p)*metricScale177,.05,2.5),mt=body177.mass+load32;
                    body177.com=(body177.mass*body177.com+load32*d)/mt;body177.gripInertia+=load32*d*d;body177.transverse+=load32*.01;body177.mass=mt;}
                // C44: his weight sags the TARGET (what the hand asks for): the grip drops and the tip droops toward down until Henry lifts
                // and tilts the hand up (VrImpaleHangMassKg). Ramped over ~300 ms in and out; nothing native, display only follows the sim.
                s.hangSag44=kcd2vr::gore32::sagRamp(s.hangSag44,hanging44,driveDt);
                if(s.hangSag44>0){
                    const Vec3 downM=m::unit(in.haveWorld?m::rotate(m::conjugate(in.world.q),Vec3{0,0,-1}):Vec3{0,0,-1}).value_or(Vec3{0,0,-1});
                    const auto sag44=kcd2vr::gore32::hangSag(double(kcd2vr::gore32::hangMassKg.load(std::memory_order_relaxed)),
                        stats59c17.active?kcd2vr::dynamics177::statAuthority59c17(kcd2vr::dynamics177::authority(10,10,1,false,supportEngaged19c3),stats59c17).force:
                        kcd2vr::dynamics177::authority(in.attributes.strength,in.attributes.agility,in.attributes.stamina,in.attributes.live,supportEngaged19c3).force);
                    const Vec3 bladeM=m::unit(m::rotate(desired.q,p.tip-p.grip[side].p)).value_or(Vec3{0,1,0});
                    if(const auto hinge=m::unit(m::cross(bladeM,downM))){
                        const Quat droop=m::from_rotation_vector(*hinge*(sag44.droop*s.hangSag44));
                        desired.q=m::unit(m::multiply(droop,desired.q)).value_or(desired.q);}
                    primary.p=primary.p+downM*(sag44.drop*s.hangSag44/metricScale177);
                    desired.p=primary.p-m::rotate(desired.q,p.grip[side].p);
                }
                authority177=stats59c17.active?kcd2vr::dynamics177::statAuthority59c17(kcd2vr::dynamics177::authority(10,10,1,false,supportEngaged19c3),stats59c17):
                    kcd2vr::dynamics177::authority(in.attributes.strength,in.attributes.agility,in.attributes.stamina,in.attributes.live,supportEngaged19c3);
                // Same weapon body; lower motion authority, with static guard
                // support retained. The bounded final pose is not a 5x slowdown.
                authority177=kcd2vr::dynamics177::fatiguedAuthority59c10r2(authority177,in.weaponAuthorityScale178r44,exhaustion59c14);
                authority177=kcd2vr::dynamics177::classAuthority59c14(authority177,handling59c14); // C10R14: one-handed halberd = 10-pole stick
                axis177=m::unit(p.tip-p.grip[side].p).value_or(p.axis);
                gravity177=in.haveWorld?m::rotate(m::conjugate(in.world.q),{0,0,-9.81}):Vec3{0,0,-9.81};
                previousPivot177=point(s.lastDesired,p.grip[side].p);
                targetVelocity177=driveDt>0?limited((point(desired,p.grip[side].p)-previousPivot177)*(metricScale177/driveDt),10):Vec3{};
                targetOmega177=driveDt>0&&!releaseSupportD67?limited(m::rotation_vector(m::multiply(desired.q,m::conjugate(s.lastDesired.q)))*(1/driveDt),28):Vec3{};
            }
            Vec3 pivot=point(s.weapon,p.grip[side].p)+stanceDeltaR93;Quat q=s.weapon.q;
            if(in.controllerAnchor119&&in.haveWorld&&state.haveWorld){
                // Contact rebound is a world direction, even when Henry turns.
                const Quat turn=m::multiply(m::conjugate(in.world.q),state.world.q);
                s.recoil=m::rotate(turn,s.recoil);s.recoilVelocity=m::rotate(turn,s.recoilVelocity);
                s.angularRecoil=m::rotate(turn,s.angularRecoil);s.angularRecoilVelocity=m::rotate(turn,s.angularRecoilVelocity);
            }
            const int n=std::max(1,int(std::ceil(driveDt/.004)));const double h=driveDt/n;
            double brakeAngularR90=0; // R90 (R88 ranged): two-hand hybrid brake budget (set by the V66.129 drive)
            for(int i=0;i<n;++i){
                // D86: two hands damp a contact rebound at critical damping
                // (2*sqrt(900)); its old27 coefficient rang like a spring.
                // Keep the one-hand response and the same admitted contact impulse.
                const double reboundDampingD86=supportEngaged19c3?60.:27.;
                const Vec3 ra=s.recoil*(-900.0)-s.recoilVelocity*reboundDampingD86;
                s.recoilVelocity=limited(s.recoilVelocity+ra*h,2.5);s.recoil=limited(s.recoil+s.recoilVelocity*h,.12);
                const Vec3 aa=s.angularRecoil*(-900.0)-s.angularRecoilVelocity*reboundDampingD86;
                s.angularRecoilVelocity=limited(s.angularRecoilVelocity+aa*h,3.0);
                s.angularRecoil=limited(s.angularRecoil+s.angularRecoilVelocity*h,6.0*3.141592653589793/180.0);
                const Vec3 requestedPivot=(in.supportConstraint130&&supportEngaged19c3)?point(desired,p.grip[side].p):primary.p;
                const Vec3 target=requestedPivot+s.recoil;
                // EXACT critically damped step (unconditionally stable). A semi-implicit
                // Euler step of this stiff spring alternates every frame = visible jitter.
                if(physical177){
                    const double fraction=double(i+1)/n;
                    const auto target177=previousPivot177+(requestedPivot-previousPivot177)*fraction+s.recoil;
                    const auto targetQ177=m::multiply(m::from_rotation_vector(s.angularRecoil),*m::slerp(s.lastDesired.q,desired.q,fraction));
                    Vec3 metricPivot=pivot*metricScale177,metricVelocity=s.linearVelocity*metricScale177;
                    if(rigidClearD69){
                        metricPivot=requestedPivot*metricScale177;metricVelocity=targetVelocity177;
                        kcd2vr::dynamics177::advanceAngularD69(body177,authority177,axis177,gravity177,targetQ177,
                            targetOmega177,h,q,s.angularVelocity,stats59c17.active?stats59c17.A:1.);
                    }else kcd2vr::dynamics177::advance(body177,authority177,axis177,gravity177,target177*metricScale177,targetQ177,
                        targetVelocity177,targetOmega177,h,metricPivot,q,metricVelocity,s.angularVelocity,stats59c17.active?stats59c17.A:1.);
                    pivot=metricPivot*(1/metricScale177);s.linearVelocity=metricVelocity*(1/metricScale177);
                }else if(in.controllerAnchor119&&in.constraintDrive129){
                    // V66.129: HIGGS-style *authority*, not low-pass drag. The hand asks
                    // for a rigid grip pose; bounded acceleration/torque only permits a
                    // small separation during abrupt motion. Slow motion remains nearly
                    // one-to-one. Two hands increase authority instead of changing the
                    // weapon into a different interpolation mode.
                    const double staminaDrive=.55+.45*stamina;
                    const double handBoost=supportEngaged19c3?1.75:1.0;
                    // R90 (R88 ranged): option VrTwoHandWeightFeelR90 (default OFF = R2 heft):
                    // VrWeaponWeightFeel (lower = heavier) also scales this hybrid-mode
                    // ROTATION drive while both hands hold the weapon (otherwise only
                    // the F4 VR-direct drive reads it). Translation is unchanged.
                    const double weightR90=(supportEngaged19c3&&in.twoHandWeightFeelR90)?std::clamp(in.weaponAuthorityScale178r44,.2,2.):1.;
                    const double linearAuthority=std::clamp(240.0*strength*staminaDrive*handBoost/(.65+.55*mass),65.0,420.0);
                    const double maxLinearSpeed=std::clamp(7.0*std::sqrt(strength*staminaDrive*handBoost/(.7+.35*mass)),3.0,10.0);
                    const Vec3 e=target-pivot;
                    const Vec3 wantedVelocity=limited(e*42.0,maxLinearSpeed);
                    s.linearVelocity=limited(s.linearVelocity+limited((wantedVelocity-s.linearVelocity)*(1.0/h),linearAuthority)*h,maxLinearSpeed);
                    pivot=pivot+s.linearVelocity*h;

                    const Quat targetQ=m::multiply(m::from_rotation_vector(s.angularRecoil),desired.q);
                    const Vec3 angularError=m::rotation_vector(m::multiply(targetQ,m::conjugate(q)));
                    const double angularAuthority=std::clamp(150.0*dex*std::sqrt(strength*staminaDrive*handBoost/(.7+.30*angularMassH2)),45.0,360.0)*weightR90;
                    const double maxAngularSpeed=std::clamp(18.0*dex*std::sqrt(strength*staminaDrive*handBoost/(.8+.25*angularMassH2)),6.0,28.0)*std::sqrt(weightR90);
                    // R90 (R88 ranged): two hands = the wanted turn speed is bounded by what
                    // the arm can still brake before the target (no overshoot at a stop).
                    if(supportEngaged19c3)brakeAngularR90=angularAuthority;
                    const Vec3 wantedAngularVelocity=limited(angularError*34.0,supportEngaged19c3?
                        std::min(maxAngularSpeed,std::sqrt(2.0*angularAuthority*m::length(angularError))):maxAngularSpeed);
                    s.angularVelocity=limited(s.angularVelocity+limited((wantedAngularVelocity-s.angularVelocity)*(1.0/h),angularAuthority)*h,maxAngularSpeed);
                    q=m::multiply(m::from_rotation_vector(s.angularVelocity*h),q);
                }else if(in.controllerAnchor119){
                    pivot=target;s.linearVelocity={};s.angularVelocity={};
                    q=m::multiply(m::from_rotation_vector(s.angularRecoil),desired.q);
                }else{
                    const Vec3 e=pivot-target;const Vec3 c=s.linearVelocity+e*w;const double ex=std::exp(-w*h);
                    const Vec3 eNew=(e+c*h)*ex;
                    s.linearVelocity=limited((c-(e+c*h)*w)*ex,6.0);pivot=target+eNew;
                    const double f=1.0-std::exp(-h/tauRot);
                    if(auto sq=m::slerp(q,m::multiply(m::from_rotation_vector(s.angularRecoil),desired.q),f))q=*sq;
                    s.angularVelocity={};
                }
            }
            if(rigidClearD69){
                pivot=(in.supportConstraint130&&supportEngaged19c3)?point(desired,p.grip[side].p):primary.p;
                s.linearVelocity=driveDt>0?limited((pivot-point(s.lastDesired,p.grip[side].p))*(1./driveDt),10):Vec3{};
            }
            const Quat driveTarget=m::multiply(m::from_rotation_vector(s.angularRecoil),desired.q);
            auto compliance177=kcd2vr::dynamics177::statCompliance59c17(kcd2vr::dynamics177::classCompliance59c14(
                kcd2vr::dynamics177::compliance59c10r2(exhaustion59c14,supportEngaged19c3),handling59c14),stats59c17); // C10R14 weight/fatigue + R17 stats
            if(s.support&&manualLatch19c3&&supportReachWeightD85R4<1.){
                const auto one=kcd2vr::dynamics177::statCompliance59c17(kcd2vr::dynamics177::classCompliance59c14(
                    kcd2vr::dynamics177::compliance59c10r2(exhaustion59c14,false),
                    kcd2vr::dynamics177::handling59c14(p.classId,false,in.poleOneHand59c14)),stats59c17);
                compliance177.radians=one.radians+(compliance177.radians-one.radians)*supportReachWeightD85R4;
                compliance177.metres=one.metres+(compliance177.metres-one.metres)*supportReachWeightD85R4;
            }
            const double maxRotError=physical177?compliance177.radians:
                (in.controllerAnchor119&&in.constraintDrive129?(5.0-2.5*supportReachWeightD85R4):4.0)*3.141592653589793/180.0;
            const auto integratedQ177=q;const auto integratedPivot177=pivot;
            q=limit_toward(driveTarget,q,maxRotError); // current target includes only bounded release-basis handover
            const double maxLinearError=physical177?compliance177.metres/metricScale177:
                (in.controllerAnchor119&&in.constraintDrive129)?(.055-.025*supportReachWeightD85R4):(.065-.030*supportReachWeightD85R4);
            // Contact can leave the checked wrist outside free-space compliance.
            // Rejoin through the existing bounded motor and fresh sweep rather
            // than snapping to the envelope on the second clear frame.
            const bool contactRejoinR9=in.contact&&(s.wasBlocked||s.contactRejoinR9);
            Vec3 rejoinStopAxisR9{};
            const Vec3 rejoinTargetVelocityR9=physical177?targetVelocity177*(1/metricScale177):
                (driveDt>0?limited((primary.p-point(s.lastDesired,p.grip[side].p))*(1/driveDt),10):Vec3{});
            if(contactRejoinR9){
                const Vec3 gap=primary.p-point(s.weapon,p.grip[side].p);
                const double gap2=m::dot(gap,gap),ahead=m::dot(pivot-primary.p,gap);
                if(gap2>1e-12&&ahead>0){
                    // A returning body may reach its controller within a substep.
                    // Remove only motion past that arrival plane; retain lateral
                    // motion and let the normal sweep validate this candidate.
                    pivot=pivot-gap*(ahead/gap2);rejoinStopAxisR9=gap*(1/std::sqrt(gap2));
                }
                s.contactRejoinR9=m::length(pivot-primary.p)>maxLinearError||m::length(s.linearVelocity-rejoinTargetVelocityR9)>.05;
            }else s.contactRejoinR9=false;
            // D70: an old blocking flag cannot permanently turn the anchored
            // grip into a velocity-matching spring. Ask for the CURRENT pivot,
            // including after contact; the same full world sweep below still
            // stops it at a wall/UNKNOWN/prefix. No clearance is assumed here.
            const bool anchoredCandidateD70=rigidPolicyD69||(p.melee()&&kcd2vr::held_motion_d67::gripAnchored());
            if(anchoredCandidateD70){
                pivot=(in.supportConstraint130&&supportEngaged19c3)?point(desired,p.grip[side].p):primary.p;
                anchoredRejoinD70=contactRejoinR9;
                rejoinControllerVelocityD70=rejoinTargetVelocityR9;
            }else if(!s.wasBlocked&&!contactRejoinR9) pivot=primary.p+limited(pivot-primary.p,maxLinearError);
            // Anchoring above changes the requested candidate only. The SAME
            // world sweep below can stop it and retains genuine contact yield.
            pivot=reachable(pivot,in.shoulders[side],in.reach[side]);
            if(physical177&&driveDt>0){
                // A safety projection moves the displayed body. Do not retain
                // an unrelated pre-projection velocity as next-frame momentum.
                if(m::length(pivot-integratedPivot177)>1e-8)
                    s.linearVelocity=(pivot-point(s.weapon,p.grip[side].p)-stanceDeltaR93)*(1./driveDt);
                if(m::angle(q,integratedQ177)>1e-8)
                    s.angularVelocity=m::rotation_vector(m::multiply(q,m::conjugate(s.weapon.q)))*(1./driveDt);
            }
            if(m::dot(rejoinStopAxisR9,rejoinStopAxisR9)>.5){
                s.linearVelocity=s.linearVelocity-rejoinStopAxisR9*std::max(0.,m::dot(s.linearVelocity,rejoinStopAxisR9));
                if(m::length(pivot-primary.p)<=maxLinearError&&m::length(s.linearVelocity-rejoinTargetVelocityR9)<=.05)s.contactRejoinR9=false;
            }
            if(!physical177&&brakeAngularR90>0){
                // R90 (R88 ranged): the 2.5 deg envelope drags the blade along
                // without touching its motor speed, so on every stop it shot
                // past the hands and sprang back (D85R5 two-hand "springy";
                // no contact involved, so D86's rebound damping does not cover it).
                // Keep only the speed the arm can still brake before the target.
                const Vec3 restA=m::rotation_vector(m::multiply(driveTarget,m::conjugate(q)));
                s.angularVelocity=limited(s.angularVelocity,std::sqrt(2.0*brakeAngularR90*m::length(restA)));
            }
            wanted.q=q;wanted.p=pivot-m::rotate(q,p.grip[side].p);
        }
        if(predictionChangedD69){
            // A checked position can move to the new basis, but that setting
            // offset is not a velocity/impact-energy sample for the next motor.
            s.linearVelocity={};s.angularVelocity={};
        }
        // Inertia is model-relative (locomotion is not a swing), but contact
        // starts at the last actual WORLD pose. Express it in the current model
        // frame before interpolation so walking into a wall cannot tunnel.
        Pose contactFrom=s.weapon;
        if(in.haveWorld&&state.haveWorld){
            Pose previous=s.weapon;previous.p=previous.p*state.worldScale;
            contactFrom=compose(inverse(in.world),compose(state.world,previous));
            contactFrom.p=contactFrom.p*(1.0/in.worldScale);
        }
        double allowed=1.0;Contact103 contact{};
        if(in.contact&&p.geometry){
            // V66.102: a still, previously clear weapon does not need a new static query.
            const double scale=in.haveWorld?in.worldScale:1.0;
            const bool rootStable=!in.haveWorld||(state.haveWorld&&
                m::length(in.world.p-state.world.p)<1e-9&&m::angle(in.world.q,state.world.q)<1e-7&&
                std::abs(in.worldScale-state.worldScale)<1e-9);
            const bool still=in.sweepGate&&s.sweptReady&&s.lastClear&&rootStable&&
                in.tick>=s.lastSweepTick&&in.tick-s.lastSweepTick<50&&
                m::length(point(wanted,p.tip)-point(s.lastSwept,p.tip))*scale<.0005&&
                m::length(point(wanted,p.grip[side].p)-point(s.lastSwept,p.grip[side].p))*scale<.0005&&
                m::angle(wanted.q,s.lastSwept.q)<.0001;
            // Reuse is spatially qualified by 'still' above. A clear result for
            // a different trajectory never clears a new unknown path.
            // V66.178R6: tell the sweep how long this step was (slow frames need more coverage).
            // V66.178R8: once a step was prefix-limited the next request from the pinned pose is larger
            // still (R7 log: requestedDeg up to 164 with checkedDeg 30 at ordinary 15 ms steps), so the
            // coverage must follow the backlog, not only the step time, until the blade has caught up.
            g_sweep_coverage_scale178r6.store(std::max(s.catchUp178r8?4.0:1.0,dt>0?std::clamp(dt/.0166,1.0,4.0):1.0),std::memory_order_relaxed);
            if(!still){
                Profile queryProfileD44=p;queryProfileD44.rootTurnAngleD44=0;
                // D44: the checked seed has already been transformed from its
                // prior WORLD frame above. A fast native root rotation can ask
                // for a large world arc even while the tracked model hand is
                // nearly still. Let the existing bounded sweep choose its
                // six-segment coverage for that arc on this same current item;
                // do not move the seed to the live attachment/display pose.
                const double sourceDtD44=motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs);
                if(in.controllerAnchor119&&in.haveWorld&&state.haveWorld&&!in.contactFaultH30&&
                   in.intentTracked[side]&&finite(in.intentHands[side])&&
                   sourceDtD44>0&&sourceDtD44<=.080000001&&
                   std::abs(in.worldScale-state.worldScale)<1e-6&&
                   cached_profile_current_r20(state.cachedProfilesR20[side],p,side,false))
                    queryProfileD44.rootTurnAngleD44=m::angle(state.world.q,in.world.q);
                contact=contact103(sweep(queryProfileD44,contactFrom,wanted,false));
                // V66.162: UNKNOWN cannot authorize a new path, even immediately
                // after CLEAR. contact103 normalizes unknown to fraction zero;
                // a known, fully checked partial sweep may still advance its prefix.
                allowed=contact.fraction;
                s.lastSwept=wanted;s.lastSweepTick=in.tick;s.sweptReady=true;
                s.lastClear=contact.known&&!contact.limited115&&allowed>=.999;}
        }
        if(in.contact&&in.contactSuspended59c1&&!contact.known&&handlingRecovery55c1(true)){
            // Keep this frame's transformed checked seed; never advance it to
            // the displayed controller pose or back to a native shoulder pose.
            s.weapon=contactFrom;out.releaseReason121[side]=5;continue;
        }
        s.catchUp178r8=contact.known&&contact.limited115&&!contact.plane;
        out.contactKnown[side]=contact.known&&!contact.limited115;
        out.contact119[side]=contact;
        out.sweepDeferred115[side]=contact.limited115;
        out.contactPrefixKnown126[side]=contact.known&&contact.limited115&&contact.fraction>0;
        if(anchoredRejoinD70){
            // Corrective displacement is not kinetic input, including when a
            // NEW wall or unknown/prefix query restricts the rejoin. Preserve
            // actual current controller velocity before capturing impact effort;
            // normal contact response below still removes blocked momentum.
            s.linearVelocity=rejoinControllerVelocityD70;
            if(contact.known&&!contact.limited115&&allowed>=1.)s.contactRejoinR9=false;
        }
        const Vec3 preImpactLinear19=s.linearVelocity,preImpactAngular19=s.angularVelocity;
        Pose final=blend_weapon116(p,contactFrom,wanted,allowed);out.blocked[side]=allowed<.999;
        out.checkedPath60[side].begin(contactFrom,final,in.contact&&p.geometry&&contact.known);
        if(!out.blocked[side]){s.freeLinear178r5=s.linearVelocity;s.freeAngular178r5=s.angularVelocity;s.freeTick178r5=in.tick;s.wallStuckSince58=0;}
        if(out.blocked[side]){
            if(contact.plane&&contact.known){
                const Vec3 remaining=point(wanted,p.grip[side].p)-point(final,p.grip[side].p);
                if(allowed<.01){if(!s.wallStuckSince58)s.wallStuckSince58=in.tick;}
                else s.wallStuckSince58=0;
                const bool stuck=s.wallStuckSince58&&in.tick>=s.wallStuckSince58&&in.tick-s.wallStuckSince58>=60;
                out.wallStuck58[side]=stuck;
                const auto wallResponseH4=checked_wall_response_h4(in,p,contact,wanted,final,out.checkedPath60[side],2,sweep);
                if(wallResponseH4.handled){
                    out.wallBackout58[side]=wallResponseH4.backout;out.wallSlid58[side]=wallResponseH4.slid;
                    out.wallScraped58[side]=in.wallSlide58&&wallResponseH4.slid&&!wallResponseH4.limited;
                    if(wallResponseH4.limited){out.contactKnown[side]=false;out.sweepDeferred115[side]=true;out.contactPrefixKnown126[side]=true;out.contact119[side].limited115=true;}
                }
                // R59: once stuck, ANY outward component is a back-out request (a diagonal pull is the normal way out);
                // the tangential remainder is the slide. Both are swept (R58C1) and the back-out is tried first.
                bool backout=false;
                const Vec3 step=wall_recovery_step59c10r2(contact,remaining,stuck,backout);
                if(!wallResponseH4.handled&&(backout||in.wallSlide58)&&m::length(step)>.001){
                    // Attenuate only the tangent proposal. The original hard stop
                    // and a fresh second sweep still own every accepted prefix;
                    // friction cannot damp outward escape or authorize an unknown path.
                    const double friction=!backout&&std::isfinite(in.wallFriction57)?std::clamp(in.wallFriction57,0.,.95):0.;
                    Pose candidate=final;candidate.p=candidate.p+step*(1.-friction);Pose sprungH31{};
                    const bool springH31=checked_wall_spring_h31(in,p,contact,final,wanted,step,backout,sprungH31,sweep);
                    const auto second=springH31?Contact103{}:contact103(sweep(p,final,candidate,false));
                    if(springH31)candidate=sprungH31;
                    if(second.known){
                        const Vec3 beforeSlide=final.p;
                        final=blend_weapon116(p,final,candidate,second.fraction);
                        out.checkedPath60[side].append(final);
                        out.wallBackout58[side]=backout&&second.fraction>.01;
                        out.wallSlid58[side]=!backout&&second.fraction>.01;
                        out.wallScraped58[side]=!backout&&!second.limited115&&
                            m::length(final.p-beforeSlide)*(in.haveWorld?in.worldScale:1.)>.0005;
                    }
                }
                // R59C1: similar normals cannot authorize an unswept 2 cm move.
                out.wallBlocked58[side]=true;
                // Rebound is driven by new desired feature motion, NOT accumulated
                // spring error against a wall. Holding pressure cannot pump energy.
                Pose oldDesired=s.lastDesired;
                if(in.haveWorld&&state.haveWorld){oldDesired.p=oldDesired.p*state.worldScale;
                    oldDesired=compose(inverse(in.world),compose(state.world,oldDesired));oldDesired.p=oldDesired.p*(1.0/in.worldScale);}
                const Vec3 motion=point(desired,contact.feature)-point(oldDesired,contact.feature);
                const double incoming=dt>0?std::max(0.0,-m::dot(motion,contact.normal)/dt):0.0;
                const bool recoilDueH5=in.dynamics&&dt>0&&incoming>.25&&
                    (!s.lastImpactTick||in.tick-s.lastImpactTick>=120);
                if(recoilDueH5){
                    const Vec3 lever=point(final,contact.feature)-point(final,p.grip[side].p);
                    const double energy=std::min(incoming,6.0)*(supportEngaged19c3?.12:.18);
                    s.recoilVelocity=limited(contact.normal*energy,1.2);
                    s.angularRecoilVelocity=limited(m::cross(lever,contact.normal)*(energy/(m::dot(lever,lever)+.04)),3.0);
                    if(in.physicalCombat177&&p.melee()&&!contact.limited115){
                        const double scale=in.haveWorld?in.worldScale:1;
                        const auto body=kcd2vr::dynamics177::body(p.classId,p.weight,m::length(p.tip-p.grip[side].p)*scale);
                        const auto axis=m::rotate(final.q,m::unit(p.tip-p.grip[side].p).value_or(p.axis));
                        auto& response=out.impact177[side];
                        response=kcd2vr::dynamics177::impact(body,axis,lever*scale,contact.normal,motion*(scale/dt));
                        s.recoilVelocity=response.linear*(1/scale);s.angularRecoilVelocity=response.angular;
                    }
                    s.lastImpactTick=in.tick;s.lastImpactSpeed178r24=incoming;out.impactSpeed[side]=incoming;
                    // V66.178R24: an impact lives one sample; the game thread reads frames at its own rate, so the
                    // impact is also published as a latched event (tick + speed) that a slower reader cannot miss.
                    out.impactEventTick178r24[side]=in.tick;out.impactEventSpeed178r24[side]=incoming;
                }
                // H5: an actual actor contact is combat evidence, not a wall
                // recoil effect. A cosmetic 120ms throttle must not hide the
                // next gesture/target. No new gesture is minted here; downstream
                // exact-attempt/native settlement still decides every result.
                const bool actorImpactH5=in.physicalCombat177&&p.melee()&&dt>0&&incoming>.25&&
                    !contact.limited115&&contact.native172.entityId&&
                    (contact.native172.body==kcd2vr::npc_contact172::Body::Living||
                     contact.native172.body==kcd2vr::npc_contact172::Body::Articulated);
                if(recoilDueH5||actorImpactH5){
                    {auto& r=s.impactRecord178r30;r={};r.valid=true;r.tick=in.tick;r.speed=incoming;r.key=p.key;r.generation=p.generation;r.epoch=in.bodyEpoch;r.request=in.epoch;r.sample=in.sample;
                     // C54: remember the deliberate gesture at the actual impact,
                     // BEFORE this sample can end/cancel the recognizer. A later
                     // state reset cannot turn a real same-gesture contact into id0.
                     const bool releasedH5=normal_hybrid_intent_h31(in)?retainedGestureH26:
                         (s.released44c1&&s.lastAttack&&in.tick>=s.lastAttack&&in.tick-s.lastAttack<=160);
                     if(retainedGestureH26){r.gesture59c3=committedH26.gesture;r.originZone47=committedH26.origin;}
                     else if(s.stroke118.active||releasedH5){r.gesture59c3=s.gesture59c3;r.originZone47=s.originZone167;}
                     r.collider=contact.native172.collider;r.entityId=contact.native172.entityId;r.body=int(contact.native172.body);r.part=contact.native172.part;r.bodyMaterial=contact.native172.bodyMaterial;r.surfaceIdx=contact.native172.surfaceIdx;
                     {namespace bc=kcd2vr::bodycombo59c7g;
                      r.feature59c7g=int(impact_feature_d72(p,side,contact.feature,in.haveWorld?in.worldScale:1.));
                      r.twoHands59c7g=supportEngaged19c3;}
                     r.point=in.haveWorld?point(in.world,contact.point*in.worldScale):contact.point;r.normal=in.haveWorld?m::rotate(in.world.q,contact.normal):contact.normal;
                     r.kind=!r.gesture59c3?-1:retainedGestureH26?committedH26.kind:
                         s.stroke118.active?(s.stroke118.kind==melee118::Kind::Thrust?1:(s.stroke118.kind==melee118::Kind::Cut||s.stroke118.kind==melee118::Kind::Pommel)?0:-1):
                         releasedH5?(s.stroke117==CommandKind::Thrust?1:(s.stroke117==CommandKind::Slash||s.stroke117==CommandKind::Pommel)?0:-1):-1;
                     out.impactRecord178r30[side]=r;
                    } // Deliberate contact-assisted commits are handled by melee118 below.
                }
            }
            // V66.178R6: only a real stop (a known surface) kills the body's motion. A clear
            // prefix-limited sweep (ANGULAR_PREFIX, no plane) means the blade has not been
            // verified that far yet; it keeps its velocity and catches up on the next step
            // instead of being frozen at the prefix pose with zero velocity.
            const bool clearPrefix178r6=contact.known&&contact.limited115&&!contact.plane;
            if(!clearPrefix178r6)s.angularVelocity={};
            if(contact.known&&contact.plane)
                s.linearVelocity=s.linearVelocity-contact.normal*std::min(0.0,m::dot(s.linearVelocity,contact.normal));
            else if(!clearPrefix178r6)s.linearVelocity={};
            // V66.144: do NOT turn an unknown/deferred obstruction into impact
            // haptics.  The headset run proved that this fires while merely
            // swinging near geometry.  Only a qualified plane/normal above or
            // an accepted native melee/material hit may produce strike feedback.
            // The deferred constraint can still limit the weapon pose; it simply
            // carries no invented impact magnitude.
            if(contact.known&&contact.limited115&&allowed<.999){
                out.impactSpeed[side]=0.0;
            }
        }
        // Avoid a constraint demanding impossible anatomy. Releasing support is
        // preferable to stretching either arm or moving the headset.
        Pose hand=compose(final,p.grip[side]);
        // R4 (Miki 23 oct: overhead swings and wide swings never register, DIRECT stopped connecting): when the VR hand is further
        // from Henry's shoulder than his arm reaches, the old path threw the weapon into display-only handling and (DIRECT,
        // non-normal intent) aborted and cleared the swing - 2521 such resets mid-swing in the 12:35 log, 271 in 13:01 (physics off).
        // now the weapon is pulled back along the arm to Henry's reach (toward his own body, so no new wall crossing) and the
        // swing, its contacts and its hits continue. the bow too (Miki: "the bow should also never cancel"): a full draw or a
        // stretched bow arm is pulled back to Henry's reach instead of cancelling the draw.
        if(m::length(hand.p-in.shoulders[side])>in.reach[side]+.001){
            const Vec3 armR4=hand.p-in.shoulders[side];const double lenR4=m::length(armR4);
            if(lenR4>1e-6&&std::isfinite(lenR4)){
                const Vec3 clampedR4=in.shoulders[side]+armR4*(in.reach[side]/lenR4);
                final.p=final.p+(clampedR4-hand.p);hand=compose(final,p.grip[side]);
                out.reachClampedR4[side]=true;}
        }
        if(m::length(hand.p-in.shoulders[side])>in.reach[side]+.001){
            out.releaseReason121[side]=4;
            // There is no verified feasible contact/reach solution here. Do not
            // translate an already collision-limited weapon through a wall.
            if(s.bowHeld){out.add(CommandKind::BowCancel,in,p);out.bow116.reason=BowReason116::Unreachable;}
            s.weapon=final;
            if(!handlingRecovery55c1()){
                if(s.prepared)out.add(CommandKind::Abort,in,p);
                s={};
            }continue;
        }
        // Observe the SAME feature on the checked start/final path. Inward
        // controller pressure is not tangential movement. Runs even when native
        // attack requests are disabled, and never requests a native hit/effect.
        if(in.contact&&contact.known&&contact.plane&&!contact.limited115&&out.blocked[side]&&dt>0&&dt<=.080){
            Pose previousDesired=s.lastDesired;
            if(in.haveWorld&&state.haveWorld){previousDesired.p=previousDesired.p*state.worldScale;
                previousDesired=compose(inverse(in.world),compose(state.world,previousDesired));previousDesired.p=previousDesired.p*(1.0/in.worldScale);}
            const double scale=in.haveWorld?in.worldScale:1.0;
            const Vec3 requestedMotion=(point(desired,contact.feature)-point(previousDesired,contact.feature))*scale;
            const Vec3 allowedMotion=(point(final,contact.feature)-point(contactFrom,contact.feature))*scale;
            auto& evidence=out.contactMotion169r1[side];
            evidence.incoming=std::max(0.0,-m::dot(requestedMotion,contact.normal)/dt);
            evidence.slipDistance=m::length(allowedMotion-contact.normal*m::dot(allowedMotion,contact.normal));
            evidence.slipSpeed=evidence.slipDistance/dt;evidence.valid=true;
            out.scrapeSpeed58[side]=evidence.slipSpeed; // R58: the blade dragging along the surface
        }
        // F4=3: lethal presentation latch, or the explicitly selected C40
        // native-result/armour display experiment. Never invent native damage,
        // force a living victim into ragdoll, or pull an NPC by the display pose.
        const auto& receipt=in.receipt58c1;
        // C47: every NEW killing thrust of THIS weapon is either latched or refused with a reason (C46: one lethal thrust, nothing happened, no line).
        if(in.impaleEnabled178r15&&p.melee()&&!s.impale178r15&&in.haveWorld&&in.worldScale>1e-6&&
           receipt.serial&&receipt.serial!=s.lastImpaleReceipt58c1&&receipt.serial!=s.refusedSerial47&&receipt.serial!=out.impaleSerial&&(receipt.death==1||in.impale40)&&receipt.kind==1&&receipt.key==p.key&&
           !(receipt.tick&&in.tick<receipt.tick)){ // C63: completion may postdate this input sample; wait, do not consume it as a permanent future-time refusal.
            const bool contract=kcd2vr::contact58c1::lethalThrust(receipt,in.tick,p.key,p.generation,in.bodyEpoch,in.epoch)||
                (in.impale40&&kcd2vr::impale40::candidateThrust(receipt,in.tick,p.key,p.generation,in.bodyEpoch,in.epoch)); // C40: the game thread decides depth/refusal; C44: dead bodies only
            int why=0;double tipM=-1;
            if(!contract)why=1+int(kcd2vr::contact58c1::lethalThrustReasonC2(receipt,in.tick,p.key,p.generation,in.bodyEpoch,in.epoch));
            // IT1 (ChatGPT review source3 #4): VrImpaleKillEnergy is the KILLING-impale threshold. It applies
            // to a lethal (death=1) or unknown (-1) outcome only; a native-confirmed nonlethal (death=0) C40
            // candidate keeps every other contract check but no longer inherits the kill setting.
            else if(receipt.death!=0&&!kcd2vr::contact58c1::strongEnough44(receipt))why=kcd2vr::impale47::kRefuseTooWeak; // C44: a killing poke below VrImpaleKillEnergy stays a plain native kill
            if(!why){
                const Vec3 hit{receipt.point[0],receipt.point[1],receipt.point[2]};
                const Vec3 tipWorld=point(in.world,point(final,p.tip)*in.worldScale),gripWorld=point(in.world,point(final,p.grip[side].p)*in.worldScale);
                const double g3[3]{gripWorld.x,gripWorld.y,gripWorld.z},t3[3]{tipWorld.x,tipWorld.y,tipWorld.z},h3[3]{hit.x,hit.y,hit.z};
                // C47: hang mode takes the blade point nearest to the contact (the receipt is 60..80 ms late, the blade moved on); the F4 visual latch keeps 12 cm
                const auto pk=kcd2vr::impale47::pickStuckPoint(g3,t3,h3,in.impaleHang32);tipM=pk.tipDistance;
                if(pk.ok()){
                    s.lastImpaleReceipt58c1=receipt.serial;
                    const Vec3 stuck{pk.point[0],pk.point[1],pk.point[2]};
                    s.impale178r15=true;s.impaleSince178r15=in.tick;s.impaleEntity178r15=receipt.target;s.impaleCorpseR15=false;s.impaleCorpseCandidateR15={};
                    s.impalePinned178r18=false;s.impaleSerial178r15=receipt.serial;s.impaleTipWorld178r18=hit; // the wound = the native contact point on HIS body
                    s.impaleHit47=receipt;s.impalePick47=int(pk.pick);s.impaleTipM47=pk.tipDistance;s.impaleDepth47=pk.depth;
                    out.impaleLatchRequest=true;out.impaleEntity=receipt.target;out.impaleSerial=receipt.serial;out.impaleTipWorld=hit;
                    s.impaleLocal32=point(inverse(final),point(inverse(in.world),stuck)*(1.0/in.worldScale));out.impaleSide32=side;out.impaleLocal32=s.impaleLocal32;
                    out.impaleHit47=s.impaleHit47;out.impalePick47=s.impalePick47;out.impaleTipM47=s.impaleTipM47;out.impaleDepth47=s.impaleDepth47;
                }else why=pk.pick==kcd2vr::impale47::Pick::OffBlade?kcd2vr::impale47::kRefuseOffBlade:pk.pick==kcd2vr::impale47::Pick::TipFar?kcd2vr::impale47::kRefuseTipFar:kcd2vr::impale47::kRefuseInvalid;
            }
            if(why){s.refusedSerial47=receipt.serial;s.refusalTick47=in.tick;s.refusal47=why;s.refusalReason47=why<20?why-1:0;s.refusalTipM47=tipM;}
        }
        // N2 (Miki: impalements): a THRUST of this weapon into a body the game itself killed this lifetime (own completed
        // contact record + gore32::KnownDead, checked on the game thread) latches the same corpse HANG. Hang mode only;
        // no native receipt is invented (impaleHit47 carries identity only, death stays unknown).
        if(in.impaleEnabled178r15&&in.impaleHang32&&p.melee()&&!s.impale178r15&&!out.impaleLatchRequest&&in.haveWorld&&in.worldScale>1e-6&&
           in.corpseStabR15.side==side&&
           kcd2vr::impale_corpse_r15::admissible(in.corpseStabR15,in.tick,p.key,p.generation,in.bodyEpoch,in.epoch,s.lastCorpseStabR15)){
            const auto& c=in.corpseStabR15;s.lastCorpseStabR15=c.serial;
            const Vec3 hit{c.point[0],c.point[1],c.point[2]};
            const Vec3 tipWorld=point(in.world,point(final,p.tip)*in.worldScale),gripWorld=point(in.world,point(final,p.grip[side].p)*in.worldScale);
            const double g3[3]{gripWorld.x,gripWorld.y,gripWorld.z},t3[3]{tipWorld.x,tipWorld.y,tipWorld.z},h3[3]{hit.x,hit.y,hit.z};
            const auto pk=kcd2vr::impale47::pickStuckPoint(g3,t3,h3,true);
            if(pk.ok()){
                const Vec3 stuck{pk.point[0],pk.point[1],pk.point[2]};
                s.impale178r15=true;s.impaleSince178r15=in.tick;s.impaleEntity178r15=c.target;s.impaleCorpseR15=true;s.impaleCorpseCandidateR15=c;
                s.impalePinned178r18=false;s.impaleSerial178r15=c.serial;s.impaleTipWorld178r18=hit;
                s.impaleHit47=kcd2vr::contact58c1::Receipt{};s.impaleHit47.serial=c.serial;s.impaleHit47.target=c.target;s.impaleHit47.tick=c.tick;
                s.impaleHit47.targetIdentity=c.identity;s.impaleHit47.collider=c.collider;s.impaleHit47.rawPart=c.part;s.impaleHit47.kind=1;
                s.impaleHit47.bodyMaterial=c.bodyMaterial;s.impaleHit47.energy=float(c.power);
                for(int i=0;i<3;++i)s.impaleHit47.point[i]=float(c.point[i]);
                s.impalePick47=int(pk.pick);s.impaleTipM47=pk.tipDistance;s.impaleDepth47=pk.depth;
                out.impaleLatchRequest=true;out.impaleCorpseR15=true;out.impaleCorpseCandidateR15=c;out.impaleEntity=c.target;out.impaleSerial=c.serial;out.impaleTipWorld=hit;
                s.impaleLocal32=point(inverse(final),point(inverse(in.world),stuck)*(1.0/in.worldScale));out.impaleSide32=side;out.impaleLocal32=s.impaleLocal32;
                out.impaleHit47=s.impaleHit47;out.impalePick47=s.impalePick47;out.impaleTipM47=s.impaleTipM47;out.impaleDepth47=s.impaleDepth47;
            }
        }
        if(s.refusal47&&in.tick>=s.refusalTick47&&in.tick-s.refusalTick47<=500&&!out.impaleRefusal47){
            out.impaleRefusal47=s.refusal47;out.impaleRefusalSide47=side;out.impaleRefusalSerial47=s.refusedSerial47;out.impaleRefusalReason47=s.refusalReason47;out.impaleRefusalTipM47=s.refusalTipM47;}
        if(in.impaleEnabled178r15&&p.melee()&&s.impale178r15){
            const bool fresh=in.impalePinValid178r15&&in.impalePinSerial178r19==s.impaleSerial178r15&&in.haveWorld&&in.worldScale>1e-6; // R19: serial-matched pin only
            const Vec3 axisM=m::unit(point(final,p.tip)-point(final,p.grip[side].p)).value_or(Vec3{0,1,0});
            // R19: the pull is measured against the PINNED grip (display), not the physical weapon that now follows the hand.
            const Vec3 pinShift=fresh?(point(inverse(in.world),in.impalePinWorld178r15)*(1.0/in.worldScale)-point(final,p.tip)):Vec3{};
            const Vec3 gripP=point(final,p.grip[side].p)+pinShift;
            const double pull=m::dot(gripP-in.hands[side].p,axisM); // hand behind the pinned grip = pulling out
            out.impalePull=pull;
            // V66.178R18 (review F01): REQUESTED -> (first pin published by the game thread) -> ACTIVE. The
            // first pin may arrive a few frames after the request; waiting for it is not a lost pin.
            // C32 hang: the corpse may still blend into its ragdoll, so the first pin can take up to 1.2 s
            // H18: the immutable lethal-victim wait is4500ms; this original
            // request accepts a first pin for4700ms. No source/contact grace.
            const bool waiting=!fresh&&!s.impalePinned178r18&&in.tick>=s.impaleSince178r15&&in.tick-s.impaleSince178r15<=(in.impaleHang32?kcd2vr::impale47::kWeaponWaitH18:in.impale40?1200u:250u);
            out.impaleSide32=side;out.impaleLocal32=s.impaleLocal32;
            if(waiting){out.impaleLatchRequest=true;out.impaleCorpseR15=s.impaleCorpseR15;if(s.impaleCorpseR15)out.impaleCorpseCandidateR15=s.impaleCorpseCandidateR15;out.impaleSerial=s.impaleSerial178r15;out.impaleEntity=s.impaleEntity178r15;out.impaleTipWorld=s.impaleTipWorld178r18;
                out.impaleHit47=s.impaleHit47;out.impalePick47=s.impalePick47;out.impaleTipM47=s.impaleTipM47;out.impaleDepth47=s.impaleDepth47;} // the request is re-published until the first pin arrives (a missed frame is not a lost latch)
            else if(in.impale40&&in.impalePinMode40==1){ // C40 STUCK (living victim): the game thread owns pull-out/kick/palm/torn; the display keeps the blade in the wound
                const Vec3 pinModel=fresh?point(inverse(in.world),in.impalePinWorld178r15)*(1.0/in.worldScale):Vec3{};
                const Vec3 tipM=point(final,p.tip),toPin=pinModel-tipM;
                const double pen=-m::dot(toPin,axisM),cap=std::clamp(in.impalePinDepth40,.05,.80)/in.worldScale; // model units
                const Vec3 shift=toPin+axisM*(pen-std::max(0.0,pen-cap)); // lateral into the wound line + push back past the depth cap
                Pose shown=final;shown.p=final.p+shift;
                const Contact103 checked=fresh?contact103(sweep(p,final,shown,false)):Contact103{};
                const Pose shownHand=compose(shown,p.grip[side]);
                if(fresh&&m::length(shift)*in.worldScale<=.35&&checked.known&&checked.fraction>=.999&&m::length(shownHand.p-in.shoulders[side])<=in.reach[side]+.001){
                    s.impalePinned178r18=true;out.displayWeapons[side]=shown;out.displayHands[side]=shownHand;out.displayLatched[side]=true;
                    out.impaleActive=true;out.impaleEntity=s.impaleEntity178r15;out.impaleSerial=s.impaleSerial178r15;
                }else{s.impale178r15=false;s.impalePinned178r18=false;out.impaleReleased=true;out.impaleSerial=s.impaleSerial178r15;out.impaleRelease40=fresh?8:7;}
            }
            else if(in.impaleHang32){ // C32: the game thread owns pull-out / kick / hand push / stuck; a missing pin = released there
                if(!fresh){s.impale178r15=false;s.impalePinned178r18=false;out.impaleReleased=true;out.impaleSerial=s.impaleSerial178r15;out.impaleRelease40=7;} // R93: distinguish pin loss from generic weapon-side release.
                else{s.impalePinned178r18=true;out.impaleActive=true;out.impaleEntity=s.impaleEntity178r15;out.impaleSerial=s.impaleSerial178r15;}
            }
            else if(!fresh||pull>.14||m::length(pinShift)*in.worldScale>.20||in.tick<s.impaleSince178r15||in.tick-s.impaleSince178r15>(in.kickHoldImpale26?2500u:1500u)||in.kickReleaseImpale26){s.impale178r15=false;s.impalePinned178r18=false;out.impaleReleased=true;out.impaleSerial=s.impaleSerial178r15;}
            else{s.impalePinned178r18=true;
                const Vec3 pinModel=point(inverse(in.world),in.impalePinWorld178r15)*(1.0/in.worldScale);
                Pose shown=final;shown.p=final.p+(pinModel-point(final,p.tip)); // R19: display copy only; `final` (physics/native) is untouched
                const auto checked=contact103(sweep(p,final,shown,false));
                const Pose shownHand=compose(shown,p.grip[side]);
                if(checked.known&&checked.fraction>=.999&&m::length(shownHand.p-in.shoulders[side])<=in.reach[side]+.001){
                    out.displayWeapons[side]=shown;out.displayHands[side]=shownHand;out.displayLatched[side]=true;
                    out.impaleActive=true;out.impaleEntity=s.impaleEntity178r15;out.impaleSerial=s.impaleSerial178r15;
                }else{s.impale178r15=false;s.impalePinned178r18=false;out.impaleReleased=true;out.impaleSerial=s.impaleSerial178r15;}
            }
        }else if(!in.impaleEnabled178r15)s.impale178r15=false;
        if(!out.impactEventTick178r24[side]&&s.lastImpactTick&&in.tick>=s.lastImpactTick&&in.tick-s.lastImpactTick<=250){out.impactEventTick178r24[side]=s.lastImpactTick;out.impactEventSpeed178r24[side]=s.lastImpactSpeed178r24;} // R24: re-publish for 250 ms
        if(!out.impactRecord178r30[side].valid&&s.impactRecord178r30.valid&&in.tick>=s.impactRecord178r30.tick&&in.tick-s.impactRecord178r30.tick<=250)out.impactRecord178r30[side]=s.impactRecord178r30; // R30: the record, not the current ray
        // A native ray-latency backoff is not a constraint on the tracked bow.
        // Keep its checked collision seed below, but publish one current
        // handling pose for the wrist, native attachment, arrow and string.
        // Continue the original charge/release machine; query scheduling may
        // neither cancel a shot nor manufacture draw against a frozen bow.
        const Pose checkedBowPoseR12=contactFrom;
        const bool bowHandlingOnlyR12=side==1&&p.bow()&&in.controllerAnchor119&&in.contact&&
            in.contactSuspended59c1&&!in.contactFaultH30&&!contact.known&&
            in.intentTracked[side]&&finite(in.intentHands[side])&&finite(desired);
        if(bowHandlingOnlyR12){
            final=desired;out.bowHandlingOnlyR12=true;
            out.impactRecord178r30[side]={};out.impactSpeed[side]=0;
            out.impactEventTick178r24[side]=0;out.impactEventSpeed178r24[side]=0;
        }
        out.hands[side]=compose(final,p.grip[side]);out.weapons[side]=final;out.active[side]=out.owned[side]=true;
        if(!out.displayLatched[side]){out.displayWeapons[side]=final;out.displayHands[side]=out.hands[side];}
        if(s.support){
            auto second=compose(final,supportLocal);
            // Both wrists derive from one retained weapon-local grasp. A
            // position-only hilt uses the opposite wrist captured at acquisition,
            // never the primary wrist or a new raw-controller writer per solve.
            if(m::length(second.p-in.shoulders[other])<=in.reach[other]+.001){out.hands[other]=second;out.owned[other]=true;out.support[side]=true;}
            else if(!manualLatch19c3){s.support=false;s.supportRearm=true;}
            // If the rigid grasp cannot be reached anatomically this frame,
            // leave out.hands[other] at its current controller-owned fallback.
            // Keep only the grasp identity; never stretch or hide the hand.
        }
        publishDisplayRejoinR13(contact);
        if(p.bow()&&s.bowHeld&&!bowActionsR155){
            out.add(CommandKind::BowCancel,in,p);s.bowHeld=false;s.bowRearm=true;
            out.bow116.reason=BowReason116::HandUnavailable;
        }
        // A selected physical bow stays not-ready until geometry/tracking returns
        // and its Trigger has been neutral. Grip cannot rearm a held Trigger.
        const bool manualBowGeometry146=manual_bow_geometry146(p);
        if(p.bow()&&physical_bow175(in)&&(!manualBowGeometry146||!bowGeometryReadyR155||!bowActionsR155)){
            if(s.bowHeld)out.add(CommandKind::BowCancel,in,p);
            s.bowHeld=s.carry146=false;s.bowFraction=0;s.bowRearm=true;
            out.bow116.reason=manualBowGeometry146?BowReason116::HandUnavailable:BowReason116::GeometryUnavailable;
            out.bow116.rearm=true;
        }
        if(in.bowActions&&p.bow()&&side==1&&!physical_bow175(in)&&
           (in.nativeBowTrigger126||(in.quiverGesture114&&!manualBowGeometry146))){
            // The tracked string geometry is not a verified arrow socket. Do not
            // make a native trigger shot depend on reaching that point or on a
            // guessed draw axis. Primary grip/ownership and live tracking remain
            // mandatory; loss cancels, never synthesizes a release.
            auto& observation=out.bow116;observation.reason=BowReason116::Waiting;
            observation.rearm=s.bowRearm;
            if(!otherFree||!in.intentTracked[0]||!in.intentTracked[1]||!std::isfinite(in.trigger[0])){
                if(s.bowHeld)out.add(CommandKind::BowCancel,in,p);
                s.bowHeld=false;s.bowRearm=true;observation.reason=BowReason116::HandUnavailable;
            }else if(s.bowHeld&&in.trigger[0]<.25){
                out.add(CommandKind::BowFire,in,p); // fraction is deliberately unused in native mode
                s.bowHeld=false;s.bowRearm=true;observation.reason=BowReason116::FireRequested;
            }else if(!s.bowHeld&&!s.bowRearm&&in.trigger[0]>.55){
                s.bowHeld=true;out.add(CommandKind::BowBegin,in,p);observation.reason=BowReason116::Acquired;
            }else if(s.bowHeld)observation.reason=BowReason116::Drawing;
            out.bowHeld=s.bowHeld;s.bowFraction=0;
            // Leave the wrist at its tracked endpoint. Native string/arrow
            // attachment is separate from the input edge and remains unverified.
        }else if(bowActionsR155&&p.bow()&&side==1&&p.support&&finite(p.grip[0])&&m::unit(p.axis)){
            // V66.107: with the over-the-shoulder quiver the RIGHT GRIP is the arrow hand
            // (take the arrow with grip over the right shoulder, keep holding, it nocks at the
            // string, pull back, release grip to shoot). Without the quiver, RT owns the draw.
            const double bowHold=bow_uses_trigger175(in)?in.trigger[0]:in.grip[0];
            const double arrowHold160=in.grip[0]; // the arrow itself is always held with Grip
            const Pose nock=compose(final,p.grip[0]);
            auto& observation=out.bow116;observation.reason=BowReason116::Waiting;
            observation.gap=m::length(in.hands[0].p-nock.p);observation.rearm=s.bowRearm;
            if(s.bowHeld&&(!otherFree||!in.tracked[0])){out.add(CommandKind::BowCancel,in,p);s.bowHeld=false;s.carry146=false;s.bowRearm=true;observation.reason=BowReason116::HandUnavailable;}
            // V66.102: Skyrim-VR style quiver over the right shoulder.
            if(in.arrowFromShoulder&&!s.bowHeld){
                const double zone=m::length(in.hands[0].p-right_shoulder_zone(in));
                if(!s.arrowInHand){
                    if(bowHold>.55&&zone<.22){if(!s.arrowSince)s.arrowSince=in.tick;if(in.tick-s.arrowSince>=40){s.arrowInHand=true;s.bowRearm=false;}}
                    else s.arrowSince=0;
                } else if(bowHold<.15){s.arrowInHand=false;s.arrowSince=0;} // arrow dropped before nocking
            }
            const bool quiverReady=quiver114::update(s.quiver,in.quiverGesture114&&otherFree&&
                in.intentTracked[0]&&in.intentTracked[1]&&!s.bowHeld,in.bowStringTrigger160?arrowHold160:bowHold, // V66.160 arrow = Grip
                m::length(in.hands[0].p-right_shoulder_zone(in)),in.tick,in.quiverRadius146);
            const bool arrowReady=in.quiverGesture114?(quiverReady||(in.bowRegrab116&&!s.quiverSpent116)):(!in.arrowFromShoulder||s.arrowInHand);
            observation.needsQuiver=!arrowReady;
            // C56: an actual release-to-nock transfer may bridge a NEW Grip
            // press that arrived before the native-ready publication. A held
            // carry Grip without the real release can never use this edge.
            // The producer claims once, including refused solves; no replay.
            const auto& ack=in.manualNockAck19c4r4;
            if(ack.serial&&ack.serial!=s.manualNockSeen19c4r4){
                s.manualNockSeen19c4r4=ack.serial;
                if(!s.bowHeld&&physical_bow175(in)&&in.bowGripDraw47c2&&in.bowActions&&in.bowGeometryReady175&&
                   ack.key==p.key&&ack.generation==p.generation&&ack.request==in.epoch&&ack.body==in.bodyEpoch&&
                   ack.sample&&ack.sample<=in.sample&&ack.sample>=s.lastBowRelease166&&ack.tick&&ack.tick<=in.tick&&in.tick-ack.tick<=80&&
                   ack.releaseToNock56&&ack.handEntity56&&ack.handIdentity56&&ack.releaseSample56&&ack.releaseTick56&&
                   ack.sample>=ack.releaseSample56&&ack.tick>=ack.releaseTick56&&in.sample>ack.releaseSample56&&
                   in.tick>ack.releaseTick56&&in.tick-ack.releaseTick56<=80&&
                   otherFree&&in.intentTracked[0]&&in.intentTracked[1]&&in.tracked[0]&&in.tracked[1]&&
                   bowHold>.55&&m::length(in.hands[0].p-nock.p)<(in.bowRetention?.16:.13))s.bowRearm=false;
            }
            if(!s.bowHeld&&!s.bowRearm&&arrowReady&&otherFree&&bowHold>.55&&m::length(in.hands[0].p-nock.p)<(in.bowRetention?.16:.13)){
                s.bowHeld=true;s.lastBowRelease166=0;s.nock=p.grip[0].p;s.drawAxis=p.axis;s.bowNockQualified43=p.bowNockQualified40;s.bowGeometryRank48=p.bowGeometryRank48;
                s.bowPullReady63=s.bowPullRebase63=false;
                // Acquisition offset is neither draw travel nor lateral deviation.
                s.bowAcquireOffset121=m::rotate(m::conjugate(final.q),in.hands[0].p-nock.p);
                // C43 measured nock: don't retain a lateral first-draw seed error.
                if(p.bowNockQualified40){const auto axis=m::unit(s.drawAxis);
                    if(axis)s.bowAcquireOffset121=*axis*m::dot(s.bowAcquireOffset121,*axis);}
                s.maxDraw=p.bowDrawLength166>=.12&&p.bowDrawLength166<=.95?p.bowDrawLength166:std::clamp(in.reach[0]*.85,.35,.65);s.bowFraction=0;s.lateralSince=0;
                if(in.bowRegrab116)s.quiverSpent116=false; // an acquired ticket survives an uncharge/regrab
                out.add(CommandKind::BowBegin,in,p);
                observation.reason=BowReason116::Acquired;
                if(in.quiverGesture114)quiver114::consume(s.quiver);
            }
            // V66.146R1 TRUE RELOAD with native grab: taking the arrow over the right
            // shoulder starts the NATIVE load/draw at once, so the game's own arrow
            // appears in Henry's (tracked) right hand like in the flat game. The arrow
            // is then CARRIED to the string; draw evaluation starts only there.
            if(in.bowNativeGrab146&&in.quiverGesture114&&quiverReady&&!s.bowHeld&&!s.bowRearm&&otherFree&&(in.bowStringTrigger160?arrowHold160:bowHold)>.55){
                s.bowHeld=true;s.lastBowRelease166=0;s.carry146=true;s.carrySince146=in.tick;s.nock=p.grip[0].p;s.drawAxis=p.axis;s.bowNockQualified43=p.bowNockQualified40;s.bowGeometryRank48=p.bowGeometryRank48;s.bowAcquireOffset121={};
                s.bowPullReady63=s.bowPullRebase63=false; // quiver carry has not latched the string yet
                s.maxDraw=p.bowDrawLength166>=.12&&p.bowDrawLength166<=.95?p.bowDrawLength166:std::clamp(in.reach[0]*.85,.35,.65);s.bowFraction=0;s.lateralSince=0;s.quiverSpent116=false;
                out.add(CommandKind::BowBegin,in,p);observation.reason=BowReason116::Acquired;quiver114::consume(s.quiver);
            }
            if(s.bowHeld&&s.carry146){
                out.bowHeld=true;out.bowFraction=0;
                // V66.160: in trigger mode the CARRIED arrow is held by Grip; the
                // string is taken when the Trigger closes at the nock.
                const double carryHold160=in.bowStringTrigger160?std::max(arrowHold160,bowHold):bowHold;
                if(carryHold160<.25){ // arrow let go before the string: cancel, never a shot
                    out.add(CommandKind::BowCancel,in,p);s.bowHeld=false;s.carry146=false;s.bowRearm=true;observation.reason=BowReason116::ShortRelease;
                }else if(m::length(in.hands[0].p-nock.p)<(in.bowRetention?.16:.13)&&(!in.bowStringTrigger160||bowHold>.55)){
                    s.carry146=false;s.lateralSince=0;
                    s.bowPullReady63=s.bowPullRebase63=false;
                    s.bowAcquireOffset121=m::rotate(m::conjugate(final.q),in.hands[0].p-nock.p);
                // C43 measured nock: don't retain a lateral first-draw seed error.
                if(p.bowNockQualified40){const auto axis=m::unit(s.drawAxis);
                    if(axis)s.bowAcquireOffset121=*axis*m::dot(s.bowAcquireOffset121,*axis);}
                    // Legacy Grip carry auto-nocks on entering the sphere. Its
                    // approach distance is not a deliberate Trigger latch offset.
                    if(!in.bowStringTrigger160)s.bowAcquireOffset121=s.bowAcquireOffset121-s.drawAxis*m::dot(s.bowAcquireOffset121,s.drawAxis);
                    observation.reason=BowReason116::Drawing; // nocked: physical draw begins
                }else if(in.tick>=s.carrySince146&&in.tick-s.carrySince146>4000){
                    out.add(CommandKind::BowCancel,in,p);s.bowHeld=false;s.carry146=false;s.bowRearm=true;observation.reason=BowReason116::HandUnavailable;
                }else observation.reason=BowReason116::Waiting;
            }
            if(s.bowHeld&&s.carry146){
                if(otherFree){ // R4: a string hand beyond Henry's reach is pulled back to it, never a cancel
                    Pose carryR4=in.hands[0];const Vec3 armR4=carryR4.p-in.shoulders[0];const double lenR4=m::length(armR4);
                    if(lenR4>in.reach[0]+.001&&lenR4>1e-6)carryR4.p=in.shoulders[0]+armR4*(in.reach[0]/lenR4);
                    out.hands[0]=carryR4;out.owned[0]=true;out.bowHand169r2=true;
                }else{
                    out.add(CommandKind::BowCancel,in,p);s.bowHeld=false;s.carry146=false;s.bowRearm=true;
                    out.bowHeld=false;observation.reason=BowReason116::Unreachable;
                }
            }
            if(s.bowHeld&&!s.carry146){
                const Vec3 origin=point(final,s.nock),draw=m::rotate(final.q,s.drawAxis);
                const Vec3 acquisition=m::rotate(final.q,s.bowAcquireOffset121);
                const Vec3 v=in.hands[0].p-origin-acquisition;double along=m::dot(v,draw);
                double lateral=m::length(v-draw*along);
                // V66.169 EXPERIMENT: the V164R4 log cancelled every manual draw as
                // DEVIATION with lateral ~0.8 x along: the bow's native axis is not the
                // real pull line. The pull is now the string-hand DISTANCE from the nock,
                // valid while the hand stays on the body side of the bow (toward the
                // shoulders); shot direction stays native. Only absurd poses cancel.
                if(physical_bow175(in)||in.experiments169){
                    const Vec3 body=body_center_r38(in)-origin;const double bl=m::length(body);
                    const double toward=bl>.05?m::dot(v,body*(1.0/bl)):0.0;
                    along=toward>-.05?m::length(v):-m::length(v);lateral=0.0;
                }
                if(s.bowGrip38){
                    along=s.bowPull38;
                    // A wall/large mismatch may constrain the hand, but cannot
                    // silently stretch the string through the bow or the body.
                    lateral=m::length(in.hands[0].p-(origin+acquisition+draw*along));
                }
                // The coupled geometric solution includes an allowed initial
                // offset. Measure its signed CHANGE since latch, not an absolute
                // length or a changing raw wrist basis. Use the current allowed
                // bow's primary grip and actual local string geometry; rotating
                // the wrist while that grip and hand stay fixed is zero travel.
                double chargeAlong63=along;
                if(physical_bow175(in)){
                    const auto axis=m::unit(s.drawAxis);
                    const Vec3 base=s.nock+s.bowAcquireOffset121-p.grip[side].p;
                    const Vec3 target=in.hands[0].p-point(final,p.grip[side].p);
                    const double a=axis?m::dot(base,*axis):0;
                    const double square=m::dot(target,target)-(m::dot(base,base)-a*a);
                    if(axis&&square>=0&&m::dot(target,m::rotate(final.q,*axis))>=0){
                        const double geometricPull=std::max(0.0,std::sqrt(square)-a);
                        if(!s.bowPullReady63||s.bowPullRebase63){
                            // New native geometry may improve placement; it may
                            // neither add charge nor erase already-earned draw.
                            s.bowPullRest63=geometricPull-(s.bowPullRebase63?s.bowFraction*s.maxDraw:0.0);
                            s.bowPullReady63=true;s.bowPullRebase63=false;
                        }
                        chargeAlong63=geometricPull-s.bowPullRest63;
                    }else chargeAlong63=0;
                }
                observation.along=chargeAlong63;observation.lateral=lateral;observation.maxDraw=s.maxDraw;
                s.bowFraction=std::clamp(chargeAlong63/s.maxDraw,0.0,1.0);
                // V66.102: a short lateral wobble while drawing no longer drops the arrow;
                // only a sustained (150 ms) large deviation cancels.
                const double lateralLimit=in.bowRetention?.30:.22;
                if(lateral>lateralLimit||along<-.15){if(!s.lateralSince)s.lateralSince=in.tick;}else s.lateralSince=0;
                const bool lost=(lateral>lateralLimit||along<-.15)&&(!in.bowRetention||in.tick-s.lateralSince>=150);
                // No indefinite hold in an impossible pose. Ordinary short deviation
                // is tolerated; large separation is a cancel, never a fire.
                const bool hardLost=lateral>.55||along<-.30||along>s.maxDraw+.30;
                if(lost||hardLost){out.add(CommandKind::BowCancel,in,p);s.bowHeld=false;s.bowRearm=true;s.arrowInHand=false;observation.reason=hardLost?BowReason116::HardSeparation:BowReason116::Deviation;}
                else if(bowHold<.25){
                    const auto beforeFireD65=out.commandCount;
                    out.add(s.bowFraction>.12?CommandKind::BowFire:CommandKind::BowCancel,in,p,s.bowFraction,draw*-1);
                    observation.reason=s.bowFraction>.12?BowReason116::FireRequested:BowReason116::ShortRelease;
                    // Only the newly appended Fire receives this payload. A full
                    // command buffer must not overwrite an unrelated old command.
                    if(s.bowFraction>.12&&out.commandCount>beforeFireD65&&in.haveWorld&&
                       finite(in.world)&&std::isfinite(in.worldScale)&&in.worldScale>0){
                        auto& fire=out.commands[out.commandCount-1];
                        const auto forward=m::unit(draw*-1),limb=m::unit(p.tip-p.grip[side].p);
                        if(fire.kind==CommandKind::BowFire&&forward&&limb){
                            const Vec3 nockPlay=origin+draw*std::clamp(chargeAlong63,0.0,s.maxDraw+.15);
                            fire.aimDirWorldD65C=m::rotate(in.world.q,*forward);
                            fire.aimUpWorldD65C=m::rotate(in.world.q,m::rotate(final.q,*limb));
                            fire.aimNockWorldD65C=point(in.world,nockPlay*in.worldScale);
                            fire.aimWorldD65C=m::finite(fire.aimDirWorldD65C)&&m::finite(fire.aimUpWorldD65C)&&m::finite(fire.aimNockWorldD65C);
                        }
                    }
                    if(in.bowRegrab116&&s.bowFraction>.12){s.quiverSpent116=true;s.lastBowRelease166=in.sample;}
                    s.bowHeld=false;s.bowRearm=true;s.arrowInHand=false;
                }else{
                    // Legacy projection remains outside the F3=3 comparison.
                    // In comparison the tracked wrist stays at the controller;
                    // the native bow still owns its arrow/string geometry.
                    Pose drawing=in.hands[0];
                    if(s.bowGrip38){
                        drawing.p=origin+acquisition+draw*along; // retain the coupled geometric placement, separate from charge
                        if(bowGripAcquired19c4r2)
                            s.bowHandLocal38=*m::unit(m::multiply(m::conjugate(final.q),in.hands[0].q));
                        drawing.q=*m::unit(m::multiply(final.q,s.bowHandLocal38));
                    }else if(!in.bowRegrab116&&!in.quiverGesture114)drawing.p=origin+draw*(s.bowFraction*s.maxDraw);
                    { // R4: a full draw past Henry's reach keeps the draw - the string hand stops at his reach, no cancel
                        const Vec3 armR4=drawing.p-in.shoulders[0];const double lenR4=m::length(armR4);
                        if(lenR4>in.reach[0]+.001&&lenR4>1e-6)drawing.p=in.shoulders[0]+armR4*(in.reach[0]/lenR4);}
                    if(m::length(drawing.p-in.shoulders[0])<=in.reach[0]+.002){
                        out.hands[0]=drawing;out.owned[0]=true;
                        // Physical Trigger/string ownership is independent of
                        // the optional quiver gesture. Otherwise auto-loaded
                        // arrows lose their string-hand exclusion/retarget flag.
                        out.bowHand169r2=physical_bow175(in)||in.quiverGesture114||in.bowStringTrigger160;
                        out.bowHeld=true;out.bowFraction=s.bowFraction;
                    }else{
                        out.add(CommandKind::BowCancel,in,p);s.bowHeld=false;s.bowRearm=true;s.carry146=false;
                        observation.reason=BowReason116::Unreachable; // cancel, never a fire or hidden-owner draw
                    }
                    if(s.bowHeld&&observation.reason!=BowReason116::Acquired)observation.reason=BowReason116::Drawing;
                }
            }
        }
        if(p.bow()&&!s.bowHeld){out.bowHeld=false;out.bowHand169r2=false;out.bowFraction=0;s.bowPullReady63=false;}
        normal_intent_prediction_d69(state,in,p,s,out,side,dt,rawIntentWeapon104,
            intentWeapon104,relativeWeapon63,final,stablePair63,supportHandDelta63,supportRelativeDelta63,predictionChangedD69);
        // Join only events produced by this deliberate gesture, including an
        // impact in the same sample that completes it. Never borrow the previous
        // swing's global release just because it is the latest published value.
        for(auto n=commandBegin59c3;n<out.commandCount;++n){auto& cmd=out.commands[n];
            if(cmd.kind==CommandKind::Prepare||cmd.kind==CommandKind::Slash||cmd.kind==CommandKind::Thrust||
               cmd.kind==CommandKind::Pommel||cmd.kind==CommandKind::Zone57||cmd.kind==CommandKind::Abort)
                cmd.gesture59c3=cmd.kick59?0:s.gesture59c3;
        }
        if(s.impactRecord178r30.valid&&s.impactRecord178r30.sample==in.sample&&s.impactRecord178r30.tick==in.tick){
            auto& impact=s.impactRecord178r30;
            // A new same-sample forward candidate may supply identity; otherwise
            // retain the value captured BEFORE contact changed recognizer state.
            // Unknown contacts remain id0; never borrow an arbitrary old release.
            if(!impact.gesture59c3&&(s.stroke118.active||
               (normal_hybrid_intent_h31(in)?(s.committedGestureH26.gesture&&s.committedGestureH26.forwardTickH8==in.tick&&
                    s.committedGestureH26.forwardSampleH8==in.sample):
                 (s.released44c1&&s.lastAttack&&in.tick>=s.lastAttack&&in.tick-s.lastAttack<=160)))){impact.gesture59c3=s.gesture59c3;
                impact.kind=s.stroke118.active?(s.stroke118.kind==melee118::Kind::Thrust?1:
                    (s.stroke118.kind==melee118::Kind::Cut||s.stroke118.kind==melee118::Kind::Pommel)?0:-1):
                    s.stroke117==CommandKind::Thrust?1:(s.stroke117==CommandKind::Slash||s.stroke117==CommandKind::Pommel)?0:-1;impact.originZone47=s.originZone167;}
            if((in.motionOwned59c5||(normal_intent_enabled_h30(in)&&!in.motionOwned59c5&&out.normalIntentH30[side].current))&&
               in.physicalCombat177&&contact.known&&contact.plane&&!contact.limited115)
                impact_effort19(impact,p,side,contactFrom,preImpactLinear19,preImpactAngular19,in.haveWorld?in.worldScale:1.,contact.feature);
            auto& cutD56=impact.goreCutD56;
            cutD56.gesture=impact.gesture59c3;cutD56.key=impact.key;cutD56.generation=impact.generation;
            cutD56.epoch=impact.epoch;cutD56.request=impact.request;cutD56.sample=impact.sample;cutD56.tick=impact.tick;cutD56.kind=impact.kind;
            const auto& committedD56=s.committedGestureH26;
            cutD56.committed=committedD56.gesture&&committedD56.gesture==impact.gesture59c3&&committedD56.key==impact.key&&
                committedD56.generation==impact.generation&&committedD56.epoch==impact.epoch&&committedD56.request==impact.request&&
                committedD56.kind==0&&s.stroke117==CommandKind::Slash&&in.tick>=committedD56.tick&&in.tick-committedD56.tick<=500;
            if(cutD56.committed){cutD56.netTravel=committedD56.goreNetTravelD56;cutD56.fastTravel=committedD56.goreFastTravelD56;}
            // Always publish the actual current contact, never replay a previous
            // target/part/tick to compensate for recognizer identity loss.
            out.impactRecord178r30[side]=impact;
        }
        // Display handling never establishes collision clearance. The next
        // fresh query resumes at the last actually checked WORLD pose.
        const Pose checkedFinalR12=bowHandlingOnlyR12?checkedBowPoseR12:final;
        const Vec3 tip=point(checkedFinalR12,p.tip);
        s.weapon=checkedFinalR12;s.lastDesired=desired;s.wasBlocked=out.blocked[side];if(dt>0)s.lastTip=tip;s.lastTick=in.tick;s.timeNs=in.timeNs;s.lastSample=in.sample;
        s.poseTime=in.poseTime[side];s.poseTimeEpoch=in.poseTimeEpoch;s.handPredictionPolicyD69=in.handPredictionPolicyD69;
    }
    cache_profiles_r20(state,profiles);
    state.world=in.world;state.worldScale=in.worldScale;state.haveWorld=in.haveWorld;state.cached=out;state.cached.commandCount=0;state.cached.impactSpeed[0]=state.cached.impactSpeed[1]=0;return out;
}
// HS2b (Codex R8 review 4): the pending half of a weapon switch lives in the SAME pose-transaction copy as the commands
// that start/finish it (states[0], like hipStateT10). a failed publication drops both together.
struct WeaponSwitchHS2 {std::uint64_t key=0,generation=0,body=0,tick=0;int hand=-1;bool active=false;kcd2vr::corpse_grip_cg1::DrawOccurrence corpseClaimCg1{};kcd2vr::draw_motion_d63::Source drawSourceD63{};};
enum class SwitchStepHS2 {None,Clear,Timeout,Wait,Draw};
// pure: what the pending switch does this pose. Clear = forget silently (new body/epoch, item gone or already held),
// Timeout = 3 s without free hands (say why), Draw = hands free + drawable + Grip still closed on the switch hand.
inline SwitchStepHS2 switch_step_hs2(const WeaponSwitchHS2& s,std::uint64_t body,std::uint64_t tick,bool wantedFound,bool wantedHeld,
    bool handsFree,bool drawable,double grip,bool allowed,bool tracked){
    if(!s.active)return SwitchStepHS2::None;
    if(s.body!=body||tick<s.tick||!wantedFound||wantedHeld)return SwitchStepHS2::Clear;
    if(tick-s.tick>3000)return SwitchStepHS2::Timeout;
    if(handsFree&&drawable&&s.hand>=0&&s.hand<2&&grip>.55&&allowed&&tracked)return SwitchStepHS2::Draw;
    return SwitchStepHS2::Wait;
}
struct HolsterState {std::uint64_t key=0,since=0,last=0,epoch=0,lastTick=0;Vec3 origin{};bool latched=false,rearm=true,wasGrip=false,pending=false,expectedDrawn=false;equipment114::State comparison114{};Vec3 beltMount140{};std::uint64_t beltKey140=0,beltGeneration140=0;bool beltMountReady140=false;kcd2vr::hip_t10::State hipStateT10{};WeaponSwitchHS2 switchHS2{};kcd2vr::corpse_grip_cg1::DrawOccurrence acquisitionCg1{};kcd2vr::draw_motion_d63::Source drawOriginD63{};};
// Mount is the actual equipped item's native body-relative attachment/grip, not
// a phantom body zone. A held item releases only in its own previously seen mount.
inline CommandKind holster(HolsterState& s,std::uint64_t key,Vec3 hand,Vec3 mount,double grip,bool drawn,bool eligible,std::uint64_t tick){
    if(!eligible||!key||!m::finite(hand)||!m::finite(mount)){s={};return CommandKind::None;}
    if(s.key!=key){s={};s.key=key;s.rearm=grip>.25;}
    if(s.pending){
        if(drawn==s.expectedDrawn){s.pending=false;s.rearm=true;s.wasGrip=grip>.55;}
        else if(tick>=s.last&&tick-s.last<2000)return CommandKind::None;
        else {s.pending=false;s.rearm=true;}
    }
    const double gap=m::length(hand-mount);const bool held=grip>.55;
    if(grip<.2)s.rearm=false;
    if(drawn){if(held)s.wasGrip=true;const bool release=s.wasGrip&&grip<.25;if(release)s.wasGrip=false;if(release&&gap<.18&&tick-s.last>600){s.last=tick;s.rearm=true;s.pending=true;s.expectedDrawn=false;return CommandKind::Holster;}return CommandKind::None;}
    s.wasGrip=held;
    if(grip<.25){s.latched=false;return CommandKind::None;}
    if(held&&!s.rearm&&!s.latched&&gap<.14){s.latched=true;s.origin=hand-mount;s.since=tick;}
    if(s.latched&&tick>=s.since+80&&gap>.20&&m::length((hand-mount)-s.origin)>.12){s.latched=false;s.rearm=true;s.last=tick;s.pending=true;s.expectedDrawn=true;return CommandKind::Draw;}
    if(s.latched&&tick-s.since>2000){s.latched=false;s.rearm=true;}
    return CommandKind::None;
}
} // namespace kcd2vr::physical
#include "own_shield_constraint60.h"
