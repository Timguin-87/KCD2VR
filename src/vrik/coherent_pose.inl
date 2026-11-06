#include "prop_drive5916c1.h"
#include "finger_tuning40.h"
#include "handling_profile59c10r2.h"
#include "kick_style26.h" // C26 KICKS page
#include "reading_pose32.h"
#include "grab_reach43.h" // C43: VR grab keeps the wrist on the controller from the Use to the native Attach
#include "tracked_frame51.h"
#include "self_contact54.h"
#include "hand_present_d62.h"
#include "pose_restore_debt_r96.h"
#include "../core/observed_load_recovery_r96.h"
// R59.16C10R12B: set by frik_pose_probe at install (action_time_owner59c11::WristOwnedByController); null in host tests.
inline bool (*g_wristOwnedByController59c12)(int) noexcept=nullptr;
inline bool (*g_readReloadPassengerXR34)(kcd2vr::crossbow_native_xr34::Passenger&,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept=nullptr;
inline bool (*g_currentReloadPassengerXR34)(const kcd2vr::crossbow_native_xr34::Passenger&,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept=nullptr;
inline bool (*g_prepareReloadPoseXR34)(kcd2vr::crossbow_native_xr34::Passenger&,CommittedFrame6695&,const QuatT28*,std::size_t,const TrackingSnapshot&) noexcept=nullptr;
inline bool (*g_graspAcquisition54)(kcd2vr::grasp13::Acquisition54&) noexcept=nullptr;
inline bool (*g_graspPendingUse55)(kcd2vr::grasp13::PendingUse55&) noexcept=nullptr;
// Door-only copied lease seams; existing pose fixtures keep their null no-op.
inline bool (*g_doorActive66)(int,std::uint64_t,std::uint64_t,std::uint64_t&) noexcept=nullptr;
inline bool (*g_doorGoal66)(int,std::uint64_t,std::uint64_t,kcd2vr::pose_core::LocalPose&,kcd2vr::pose_core::Vec3&,std::uint64_t&,int&) noexcept=nullptr;
inline bool (*g_doorFingers66)(int,std::uint64_t,std::uint64_t) noexcept=nullptr;
// V66.89: sole production F10 pose writer. Included in frik_pose_probe's private namespace.
// Input: one validated native model-space pose + one XR snapshot. Output: a staged,
// length-preserving model pose. Legs remain native except configured body follow
// and the F4-only procedural kick probe; camera, inventory and native hit rules are untouched.
// Never import live attack elbows, partial cache readiness, or per-joint measured lengths.
namespace pc = kcd2vr::pose_core;
namespace kick176 = kcd2vr::kick176;
static kcd2vr::core::SnapshotMailbox<kick176::Request> g_kick_request176;
static std::atomic<std::uint64_t> g_kick_serial176{0},g_kick_cancel176{1},g_kick_busy176{0};
static std::atomic<unsigned> g_kick_status176{unsigned(kick176::Status::Idle)};
static pc::Vec3 PV(V3 v) { return {v.x,v.y,v.z}; }
static V3 VP(pc::Vec3 v) { return {float(v.x),float(v.y),float(v.z)}; }
static pc::Quat PQ(Q4 q) { return {q.x,q.y,q.z,q.w}; }
static Q4 QP(pc::Quat q) { return {float(q.x),float(q.y),float(q.z),float(q.w)}; }
static Q4 DeltaRotation6689(V3 a,V3 b) noexcept {return QP(pc::from_to(PV(a),PV(b)));}
static bool ValidPose6689(const QuatT28& p) noexcept {
    double norm=0;
    for(float v:p.q) {if(!std::isfinite(v))return false;norm+=double(v)*v;}
    for(float v:p.t)if(!std::isfinite(v)||std::fabs(v)>10000.0f)return false;
    return norm>0.25&&norm<4.0;
}
constexpr std::size_t kPoseCapacity6689=1024;
struct FingerEndpoint6689 {
    std::array<QuatT28,32> local{};
    std::uint64_t since=0,sequence=0,lastSample=0;
    bool ready=false,have=false;
    const char* issue="NO_QUALIFIED_REFERENCE";int rejectedJoint=-1;
};
struct CoherentState6689 {
    void* owner=nullptr; void* character=nullptr; std::uint32_t count=0;
    RigMap rig{}; bool ready=false; int rightHandSlotR26=-1;
    std::array<int,10> stowedJointsD63{}; // initialized to -1 before name resolution
    std::array<int,10> stowedAnchorsR112{}; // nearest validated trunk/pelvis, never an arm/head owner
    std::array<QuatT28,10> stowedReferenceR112{}; // companion socket relative to that trunk
    std::array<int,kPoseCapacity6689> parent{},order{};
    std::array<bool,kPoseCapacity6689> torso{},armR{},armL{},isFinger{},trunk{};
    std::array<QuatT28,kPoseCapacity6689> rest{},restLocal{},original{},local{},work{},neutral{};
    std::array<std::array<FingerChainV6528,5>,2> fingers{};
    bool fingerMap[2]{false,false};
    std::array<std::array<int,32>,2> fingerIds{};
    int fingerCount[2]{};
    double upper[2]{},fore[2]{};
    V3 poleLocal[2]{},lastBend[2]{},sideDownLocal[2]{},hingeNormalLocal[2]{};
    V3 handForwardLocalR89[2]{},thumbLocalR89[2]{}; // R90 (R89 elbow option): hand-bone local wrist->fingers / palm->thumb (rest pose)
    pc::ElbowContinuity elbow[2]{};
    kcd2vr::elbow_shoulder_d45::ShoulderState shoulderFitD45[2]{};
    std::uint64_t elbowPolicyD45[2]{};
    float handoverW178r14[2]{},hoverW178r14[2]{};std::uint64_t naturalTick178r14[2]{}; // V66.178R14 (R18: per-side clock, review F07)
    kcd2vr::grab43::Tracker grab43[2]{}; // C43: per-Use grab window receipts
    kcd2vr::archer169r2::State archer169r2{};
    kcd2vr::hand_smooth_h50::State handSmoothH50[2]{};
    bool handFreeH50[2]{}; // Only this coherent body owner carries committed visual history.
    kcd2vr::hand_present_d62::Cache handDisplayD62{}; // optional display decisions; never gameplay history
    kcd2vr::tracked_frame51::State trackedFrame51{};
    kcd2vr::station_body_h41::State stationBodyH41{};
    // Anatomical-only pre-Queue reference, never old hand/HMD tracking. Bound
    // to one acquisition at its exact native Queue time, then retired on release.
    kcd2vr::tracked_frame51::Frame ordinaryReference54{},acquisitionReference54{};
    std::uint64_t acquisitionSerial54=0,acquisitionAction54=0;
    kcd2vr::grasp13::PendingUse55 pendingUse55{};
    kcd2vr::tracked_frame51::Frame pendingReference55{};
    pc::ReachAssist chestAssist[2]{},clavicleAssist[2]{};
    pc::ShoulderState103 shoulder[2]{};V3 chestRightLocal{},chestUpLocal{};
    Q4 boneToAnatomy[2]{}; bool anatomyHand[2]{false,false};
    FingerEndpoint6689 open[2]{},closed[2]{};
    // Exact mapped asset-decoder bank, independent of saved/default profiles.
    FingerEndpoint6689 nativeCurveClosedH7[2]{};
    bool fingerTrialRecipeH7[2]{}; // Last contact recipe, not action permission.
    pc::ForearmPronation pronation[2]{};
    // Object grip is a native evaluated parent-local rotation cache, separate
    // from the empty-hand relaxed/fist recipe and never a wrist correction.
    std::uint64_t objectFingerKey[2]{},objectFingerGeneration[2]{};
    std::array<std::array<QuatT28,32>,2> objectFinger{};
    kcd2vr::grasp13::NativeFinger54 graspFingers54[2]{};
    std::uint64_t graspMappingGeneration55=0;
    std::uint64_t doorFingerSerial66[2]{};
    std::array<std::array<QuatT28,32>,2> doorFingerLocal66{};
    unsigned doorFingerCount66[2]{};
    kcd2vr::self_contact54::State selfPat54{};
    kcd2vr::self_contact54::ShieldState shieldPat54{};
    FingerEndpoint6689 objectCandidate[2]{};
    bool objectObserved28[2]{},objectFollow28[2]{},freeObserved28[2]{}; // current diagnostic episode only
    double freeCurlObserved28[2][5]{};bool freeClampObserved28[2][5]{};
    bool dirtyAll=true;std::array<bool,kPoseCapacity6689> dirty{};
    // V66.137: mid-solve FK only needs a small ancestor-closed set. Passenger
    // descendants are deferred and settled once before invariants/commit.
    std::array<bool,kPoseCapacity6689> essential137{},pending137{};
    bool deferPassengers137=false;std::uint32_t essentialCount137=0;
    std::uint64_t composedJoints=0,initRuns137=0;
    // V66.92 requested/allowed are intentionally distinct. Collision never
    // overwrites controller intent; only the virtual endpoint is constrained.
    V3 allowedGoal[2]{},stableReq[2]{},stableSolved[2]{},stableCtrl[2]{};
    Q4 stableHmd{},stableCtrlQ[2]{};
    bool allowedReady[2]{false,false},handBlocked[2]{false,false},armBlocked[2]{false,false},armSaturated[2]{false,false},reachLimited[2]{false,false};
    bool stableWindow=false;
    std::uint64_t collisionEpoch=0,lastInvariantLog=0,lastHierarchyLog=0,lastHmdInvariantLog=0,stableSince=0,lastArmCollisionTick[2]{};
    std::uint64_t lastLog=0,lastFingerStatus=0,lastContactPass=0,contactControl=0;
    HandContact6695 contact[2]{};
    kcd2vr::hand_pair_contact_d39::State handPairD39{};
    // V66.128 (a) 6DOF body follow: leg chains found from the validated rest
    // skeleton (topology + rest geometry, no guessed bone names), and the per-foot
    // planted offset (state committed only with a successful pose transaction).
    struct Leg128 {int hip=-1,knee=-1,ankle=-1;double thigh=0,shin=0;bool ok=false;} leg[2]{};
    bool legsReady128=false;std::array<bool,kPoseCapacity6689> pelvisTree128{};
    kcd2vr::body_follow_r38::State bodyFollowR38{};
    std::uint64_t lastFollowLog128=0;
    kick176::State kick176{};
};
static CoherentState6689 g_pose6689;
static std::atomic<std::uint64_t> g_grasp_mapping_generation55{0};
static kcd2vr::core::SnapshotMailbox<kcd2vr::self_contact54::Event> g_selfPat54;
static std::atomic<std::uint64_t> g_selfPatSerial54{0};
// One atomic token: generation in high bits, smooth mode in bit 0. A toggle
// cannot pair an old mode value with a new generation in a pose transaction.
static std::atomic<std::uint64_t> g_frame_control51{2};
inline bool (*g_frame_neutral51)(std::uint64_t) noexcept=nullptr; // bound at Install; copied native/equipment proof only
static kcd2vr::core::SnapshotMailbox<kcd2vr::tracked_frame51::Frame> g_tracked_frame51;
static kcd2vr::core::SnapshotMailbox<kcd2vr::render_bounds63::Envelope> g_render_bounds63;
// Exact successful native-pose publication, separate from the XR sample which
// may be reused by multiple native callbacks. All payloads stay in race-free
// mailboxes; this token joins the two publications without copying live state.
static std::atomic<std::uint64_t> g_frame_publication51{0}; // odd while either packet can be replaced
static std::uint64_t ReadCompletedPublicationT23() noexcept {return g_frame_publication51.load(std::memory_order_acquire);}
static void RetireTrackedFrame51() noexcept {
    g_frame_publication51.fetch_add(2,std::memory_order_acq_rel);
    g_pose6689.handPairD39={};g_pose6689.trackedFrame51={};g_pose6689.stationBodyH41={};g_pose6689.bodyFollowR38=kcd2vr::body_follow_r38::State{};(void)g_tracked_frame51.publish({});
}
struct FramePublicationScope51 {
    std::uint64_t value=0;bool active=false;
    FramePublicationScope51() noexcept {
        auto previous=g_frame_publication51.load(std::memory_order_acquire);
        if(previous&1)return;
        active=g_frame_publication51.compare_exchange_strong(previous,previous+1,std::memory_order_acq_rel);
        if(active)value=previous+2;
    }
    void finish() noexcept {
        if(active){g_frame_publication51.fetch_add(1,std::memory_order_release);active=false;}
    }
    ~FramePublicationScope51(){finish();}
};

static bool FistContactFresh174(int side) noexcept {
    if(side<0||side>1)return false;const auto& c=g_pose6689.contact[side];
    return c.epoch==g_pose_epoch6689.load()&&c.fresh169r1&&c.blocked&&c.planeCount>0;
}
// Presentation tuning from the supplied V115 proposal, NOT measured WHGame eye
// geometry. It changes only the full-upper-body target, never camera or hands.
#include "../subsystems/vrik/eye_body_follow_state.inl" // M2_OWNED_SOURCE
#include "../subsystems/physics/constraint_mode_state.inl" // M2_OWNED_SOURCE
#include "../subsystems/vrik/compose_height_policy.inl" // M2_OWNED_SOURCE
#include "../subsystems/vrik/chest_policy_state.inl" // M2_OWNED_SOURCE
static std::recursive_mutex g_pose_mutex6689;
#include "../subsystems/vrik/pose_stowed_reference_r112.inl" // M2_OWNED_SOURCE
#include "../subsystems/vrik/pose_stowed_extra_r117.inl" // M2_OWNED_SOURCE
#include "../subsystems/vrik/pose_anatomy.inl" // M2_OWNED_SOURCE
#include "finger_reference.inl"
#include "calibration_runtime.inl"

#include "../subsystems/vrik/pose_fingers.inl" // M2_OWNED_SOURCE
#include "../subsystems/interactions/pose_grasp_fingers.inl" // M2_OWNED_SOURCE
struct CoherentTransaction6689;
static void ObserveVrikH29(const CoherentTransaction6689&,bool,int) noexcept;
static void ObserveFrameD30(const CoherentTransaction6689&,bool) noexcept; // D30 F2 FRAME / F7 SUMMARY numbers
static void ObserveHeldF5D30(const CoherentTransaction6689&) noexcept; // D30 F5 HELD item dynamics
// D47: temporal motion belongs to a completed pose/frame pair, not a
// temporary native-buffer write. A rejected skinning/publication transaction
// must not become the prior elbow, shoulder, handover, or visual-filter sample.
struct CoherentMotionHistoryD47 {
    bool ready=false,published=false,valid[2]{},contactMode=false;
    V3 bend[2]{};
    pc::ElbowContinuity elbow[2]{};
    pc::ForearmPronation pronation[2]{};
    kcd2vr::elbow_shoulder_d45::ShoulderState shoulderFit[2]{};
    std::uint64_t elbowPolicy[2]{},naturalTick[2]{};
    float handover[2]{};
    HandContact6695 contact[2]{};
    pc::ReachAssist clavicle[2]{};
    pc::ShoulderState103 shoulder[2]{};
    kcd2vr::archer169r2::State archer{};
    kcd2vr::hand_smooth_h50::State smooth[2]{};
    bool free[2]{};
};
static std::uint64_t CoherentClockNsD47() noexcept {
    return std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
}
static bool HandPresentContextD62() noexcept {
    return !diagnostics::lab::NativeReference()&&!g_ctx_menu.load(std::memory_order_acquire)&&
        !g_ctx_dialogue.load(std::memory_order_acquire)&&!g_ctx_cutscene.load(std::memory_order_acquire)&&
        !g_ctx_reading.load(std::memory_order_acquire)&&!g_ctx_scripted.load(std::memory_order_acquire)&&
        !animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)&&
        !animation_filter::NativeKickActive175()&&!kcd2vr::activity_input163::witness175.Active()&&
        !kcd2vr::activity_input163::witness175.transitionDepth175.load(std::memory_order_acquire);
}
// R96: failed native restoration is an exact outstanding obligation, not a
// process-wide permanent setting. The existing Henry callback holds the pose
// mutex, validates current native identity/rig/API and observes recovery before
// any VRIK admission check. No old native pointer is dereferenced to recover.
namespace restoreR96=kcd2vr::pose_restore_debt_r96;
namespace restoreLoadR96=kcd2vr::observed_load_recovery_r96;
static restoreR96::Debt<QuatT28,kPoseCapacity6689> g_pose_restore_debt_r96;
static std::array<QuatT28,kPoseCapacity6689> g_pose_restore_readback_r96;
static restoreLoadR96::Witness g_pose_restore_load_r96{};
static std::uint64_t g_pose_restore_retry_episode_r96=0;
static restoreR96::Owner PoseRestoreOwnerR96(void* owner,void* character,void* pose,std::uint32_t count) noexcept {
    return {reinterpret_cast<std::uintptr_t>(character),reinterpret_cast<std::uintptr_t>(owner),
        reinterpret_cast<std::uintptr_t>(pose),g_player_lifetime_d44.load(std::memory_order_acquire),count};
}
static void RecordPoseRestoreDebtR96(const restoreR96::Owner& owner,const QuatT28* original,
    const std::array<bool,kPoseCapacity6689>& failed) noexcept {
    // R97 guard: our R96 load re-arm cleared only the flag (removed again in the R97 merge, frik_pose_probe.cpp
    // RearmAfterLoadR96). If anything clears the flag but not this record, the old debt is retired; drop it
    // here, under the pose mutex, so a new failure records ITS own buffer/joints/C37 lifetime and is repaid by
    // readback, instead of being 'forgiven' as a replaced player or blocked until another load.
    if(g_pose_restore_debt_r96.active&&!g_pose_restore_fault6689.load(std::memory_order_acquire))g_pose_restore_debt_r96={};
    if(!g_pose_restore_debt_r96.active){
        g_pose_restore_load_r96=restoreLoadR96::observe();g_pose_restore_retry_episode_r96=0;
    }
    g_pose_restore_debt_r96.remember(owner,original,failed);
    g_pose_restore_fault6689.store(true,std::memory_order_release);
    g_committed_pose_valid6689.store(false,std::memory_order_release);
}
static bool ObservePoseRestoreRecoveryR96(void* owner,void* character,void* pose,PoseGetFn get,
    std::uint32_t count,bool nativeQualified) noexcept {
    if(!g_pose_restore_fault6689.load(std::memory_order_acquire))return true;
    const auto current=PoseRestoreOwnerR96(owner,character,pose,count);
    const auto qualifiedNow=[&]() noexcept {
        return current.playerLifetime==g_player_lifetime_d44.load(std::memory_order_acquire)&&
            PlayerIdentityAccepts37(character,std::uint64_t(GetTickCount64()))&&
            current.playerLifetime==g_player_lifetime_d44.load(std::memory_order_acquire);
    };
    if(!nativeQualified||!restoreR96::valid(current)||count>kPoseCapacity6689||!qualifiedNow()||
       !ReadNativeBuffer6689(pose,get,count,g_pose_restore_readback_r96.data()))return false;
    for(std::uint32_t j=0;j<count;++j)if(!ValidPose6689(g_pose_restore_readback_r96[j]))return false;
    if(!qualifiedNow())return false;
    auto result=g_pose_restore_debt_r96.observe(current,g_pose_restore_readback_r96.data(),true);
    bool repaid=false;
    const auto load=restoreLoadR96::observe();
    if(result==restoreR96::Resolution::Pending&&g_pose_restore_debt_r96.active&&
       restoreR96::sameBuffer(g_pose_restore_debt_r96.owner,current)&&
       restoreLoadR96::completedSince(g_pose_restore_load_r96,load)&&
       g_pose_restore_retry_episode_r96!=load.episode){
        // Loading admits one freshly qualified repair attempt, never debt
        // forgiveness. Restore only the retained failed joints on this EXACT
        // current native buffer; no saved pointer is called or dereferenced.
        // These older native values may be visible on this repair callback.
        // A failed attempt remains quarantined until another real load.
        g_pose_restore_retry_episode_r96=load.episode;
        const auto currentAdmission=[&]() noexcept {
            const auto nowLoad=restoreLoadR96::observe();
            return nowLoad.admitted&&nowLoad.episode==load.episode&&qualifiedNow();
        };
        PoseSetFn setter=nullptr;
        if(!ValidateBuffer1Setter(pose,setter)||!setter||!currentAdmission()){
            diagnostic_trace::Write("R96 POSE_RESTORE_RETRY pending=SETTER_OR_CURRENT_OWNER loadAttemptConsumed=1");return false;
        }
        bool wrote=false,writeOk=true;
        for(std::size_t j=count;j-->0;)if(g_pose_restore_debt_r96.failed[j]){
            if(!ValidPose6689(g_pose_restore_debt_r96.original[j])||!currentAdmission()){
                diagnostic_trace::Write("R96 POSE_RESTORE_RETRY pending=OWNER_CHANGED loadAttemptConsumed=1");return false;
            }
            wrote=true;
            writeOk=WriteQuat(setter,pose,int(j),g_pose_restore_debt_r96.original[j])&&writeOk;
        }
        if(!wrote||!currentAdmission()||!ReadNativeBuffer6689(pose,get,count,g_pose_restore_readback_r96.data())||!currentAdmission())return false;
        for(std::uint32_t j=0;j<count;++j)if(!ValidPose6689(g_pose_restore_readback_r96[j]))return false;
        result=g_pose_restore_debt_r96.observe(current,g_pose_restore_readback_r96.data(),true);
        repaid=result==restoreR96::Resolution::RestoredReadback;
        if(!repaid){diagnostic_trace::Write(writeOk?
            "R96 POSE_RESTORE_RETRY pending=READBACK_MISMATCH loadAttemptConsumed=1":
            "R96 POSE_RESTORE_RETRY pending=WRITE_FAILED loadAttemptConsumed=1");return false;}
    }
    if(result!=restoreR96::Resolution::RestoredReadback&&result!=restoreR96::Resolution::ReplacedNativeOwner)return false;
    // New pose work must rebuild its temporal/anatomical caches from this
    // current native owner. Saved calibration, head and camera stay untouched.
    g_pose6689.ready=false;RetireTrackedFrame51();
    g_pose_restore_fault6689.store(false,std::memory_order_release);
    diagnostic_trace::Write(repaid?"R96 POSE_RESTORE_RECOVERED proof=CURRENT_OWNER_RESTITUTION_AND_EXACT_READBACK loadAttempt=1":
        result==restoreR96::Resolution::RestoredReadback?
        "R96 POSE_RESTORE_RECOVERED proof=EXACT_NATIVE_READBACK oldPointerWrites=0":
        "R96 POSE_RESTORE_RECOVERED proof=QUALIFIED_REPLACEMENT_PLAYER oldPointerWrites=0");
    return true;
}
struct CoherentTransaction6689;
// R96R1: a setter may restore the original joint and then report an exception.
// Observe that exact current value before the native callback releases this
// rotating pose buffer. This path performs no writes and is called only after
// this callback's publication and final consumers have already seen the fault.
static bool ObservePoseRestoreAfterCallbackR96R1(void* descriptor,void* expectedPose,
    void* expectedOwner,void* expectedCharPlus8,void* expectedCharacter,std::uint32_t expectedCount) noexcept {
    const auto exact=PoseRestoreOwnerR96(expectedOwner,expectedCharacter,expectedPose,expectedCount);
    if(!g_pose_restore_fault6689.load(std::memory_order_acquire)||!g_pose_restore_debt_r96.active||
       !restoreR96::valid(exact)||expectedCount>kPoseCapacity6689||
       !restoreR96::sameBuffer(g_pose_restore_debt_r96.owner,exact))return false;
    // Native skinning may have changed mappings. Qualify the current descriptor,
    // player, pose owner, joint count and pinned getter on BOTH sides of the read;
    // earlier callback admission is not authority to read a saved buffer.
    const auto qualify=[&](PoseGetFn& current) noexcept {
        current=nullptr;
        if(exact.playerLifetime!=g_player_lifetime_d44.load(std::memory_order_acquire))return false;
        void* pose=nullptr;void* owner=nullptr;void* charPlus8=nullptr;
        if(!descriptor||!Read(static_cast<unsigned char*>(descriptor)+0x00,pose)||pose!=expectedPose||
           !Read(static_cast<unsigned char*>(descriptor)+0x08,owner)||owner!=expectedOwner||
           !Read(static_cast<unsigned char*>(descriptor)+0x10,charPlus8)||charPlus8!=expectedCharPlus8)return false;
        void* player=g_last_player_pose.load(std::memory_order_acquire);
        void* playerChar=nullptr;void* playerAnim=nullptr;void* skinChar=nullptr;void* skinAnim=nullptr;void* poseOwner=nullptr;
        if(!player||!ReadValidatedCharInstanceFromPlayerPose(player,playerChar,playerAnim)||
           !ReadValidatedCharInstanceFromSkinning(charPlus8,skinChar,skinAnim)||
           skinChar!=expectedCharacter||skinChar!=playerChar||skinAnim!=playerAnim||
           !ReadValidatedDefaultSkeletonFromPose(pose,poseOwner)||poseOwner!=owner)return false;
        std::uint32_t count=0;PoseGetFn reference=nullptr;
        return ReadJointCount(owner,count)&&count==expectedCount&&
            ValidateRawPoseApi(pose,reference,current)&&current&&
            PlayerIdentityAccepts37(skinChar,std::uint64_t(GetTickCount64()))&&
            exact.playerLifetime==g_player_lifetime_d44.load(std::memory_order_acquire);
    };
    PoseGetFn getter=nullptr;
    if(!qualify(getter)||!ReadNativeBuffer6689(expectedPose,getter,expectedCount,g_pose_restore_readback_r96.data()))return false;
    for(std::uint32_t j=0;j<expectedCount;++j)if(!ValidPose6689(g_pose_restore_readback_r96[j]))return false;
    PoseGetFn after=nullptr;
    if(!qualify(after)||after!=getter||!restoreR96::sameBuffer(g_pose_restore_debt_r96.owner,exact))return false;
    // No load witness, setter, retry or replacement-owner resolution exists on
    // this path. Exact readback repays the retained failed joints themselves.
    if(g_pose_restore_debt_r96.observe(exact,g_pose_restore_readback_r96.data(),true)!=
       restoreR96::Resolution::RestoredReadback)return false;
    g_pose6689.ready=false;RetireTrackedFrame51();
    g_committed_pose_valid6689.store(false,std::memory_order_release);
    g_pose_restore_fault6689.store(false,std::memory_order_release);
    diagnostic_trace::Write("R96R1 POSE_RESTORE_RECOVERED proof=SAME_CALLBACK_EXACT_NATIVE_READBACK additionalWrites=0 currentTransactionPublished=0");
    return true;
}
static void MotionPoseCompleteD68(const CoherentTransaction6689&) noexcept;
struct CoherentTransaction6689 {
    // Declaration order deliberately timestamps before acquiring the mutex.
    // With F2/F7 off, neither timestamp nor any per-solve clock is sampled.
    std::uint64_t lockStartNsD47=kcd2vr::diagnostics::frame_d30::anyOn()?CoherentClockNsD47():0;
    std::unique_lock<std::recursive_mutex> lock;
    void* pose=nullptr; PoseSetFn setter=nullptr;
    restoreR96::Owner restoreOwnerR96{};
    std::array<bool,kPoseCapacity6689> selected{},attempted{};
    bool committed=false,restored=false,acquisitionRequired57=false,adoptedVisualRequiredR95=false;
    kcd2vr::grasp13::FinishingPresentationR94 adoptedVisualR95{};
    bool fingerDiagnostic28=false,fingerPublished28=false,fingerContext28[2]{};
    bool fingerSpringH31=false,fingerTrialRequestedH7=false,fingerTrialH7[2]{};float fingerOpenBiasH7[2]{};
    bool reloadWorkingHandXR34=false; kcd2vr::crossbow_native_xr34::Passenger reloadPassengerXR34{};
    bool ridingGaitD76=false;std::uint64_t ridingModeD76=0,ridingLifetimeR95=0;
    bool diceBodyD81=false;kcd2vr::dice_body_d81::Lease diceLeaseD81{};
    bool instantPairHandsD72=false;kcd2vr::paired_hands_d72::Owner pairHandsD72{};
    std::uint64_t policyGeneration57=0;
    bool handPresentOnlyD61[2]{};std::uint64_t handPresentPolicyD61=0;
    bool handSemanticReadyD62=false,handSemanticSlotValidD62[2]{};
    QuatT28 handSemanticD62[2]{},handSemanticSlotD62[2]{};
    std::int64_t handPresentTimeD62=0;
    std::uint64_t kickSerial176=0,kickCancel176=0;
    kick176::Frame kickFrame176{};
    const char* stage="NOT_ATTEMPTED";int failureLine=0;
    // F7 receipt only. Original clocks remain in tracking; all status is staged
    // in this transaction and observed once after Restore/publication finishes.
    bool traceH29=false,traceStartedH29=false,traceAnatomyH29=false,traceRestoreOkH29=true;
    // Finite Shift+F2 burst identity only; no native read or hot-thread format when off.
    std::uint64_t motionSessionD68=0,motionCycleD68=0,motionSourceNsD68=0,motionSourceTickD68=0,motionSampleD68=0,motionTrackingEpochD68=0,motionInputTickD68=0;
    std::int64_t motionTargetXrNsD68=0;unsigned motionHandFlagsD68[2]{};bool motionCommittedD68=false;
    std::uintptr_t motionActorD68=0,motionDescriptorD68=0,motionOutputD68=0,motionPoseD68=0;
    bool traceNativeCalledH29=false,traceNativeReturnedH29=false,tracePublishAttemptedH29=false;
    bool tracePacketPublishedH29=false,tracePublishedH29=false,traceEnvelopePublishedH29=false;
    // Cheap publication receipts; formatting/copying stays behind F7/F2 leases.
    // Entered consumers are not undoable merely because a later owner retires.
    std::uint64_t publicationD49=0;unsigned consumersD49=0;bool pairedPublishedD49=false;
    unsigned traceRestoreWritesH29=0;int tracePublishLineH29=0;std::uintptr_t traceCharacterH29=0;
    std::uint64_t startNsD30=0,lockWaitNsD47=0,prepareNsD47=0; // F2/F7 only
    CoherentMotionHistoryD47 motionD47{};
    float traceWorldH29[8]{};bool traceWorldKnownH29=false;
    std::uint64_t traceSessionH29=0;
    kcd2vr::tracked_frame51::Frame traceFrameH29{};bool traceFrameCapturedH29=false;
    kcd2vr::diagnostics::vrik_h29::ArmContext traceArmH29[2]{};
    kcd2vr::native_roomscale_d76::Origin roomOriginD76{};bool roomOriginCapturedD76=false;
    TrackingSnapshot tracking{}; std::uint64_t epoch=0,doorRequestH40=0;
    kcd2vr::subsystems::smithing::bridge67::FixturePose smithFixture[2]{};
    SmithContactHistoryR38 smithContactR38{};
    AlchemyContactResultD80 alchemyContactD80[2]{};
    kcd2vr::hand_pair_contact_d39::State handPairD39{};
    kcd2vr::hand_only_door_h40::Admission doorHolsterAdmissionH40[2];
    V3 headTarget122{};bool fullHead122=false;
    bool stowedFullD63=false;
    kcd2vr::tracked_frame51::State frameState51{};
    kcd2vr::station_body_h41::State stationBodyH41{};
    kcd2vr::body_follow_r38::State bodyFollowR38{};
    bool stationLeaseH41=false; // Retain mode3 frame ownership when only its root step falls back.
    kcd2vr::tracked_frame51::Frame frame51{};std::uint64_t control51=0;
    kcd2vr::grasp13::Acquisition54 acquisition54{};kcd2vr::tracked_frame51::Frame acquisitionReference54{};
    std::array<PhysCorrelation126,2> physicalCorrelation126{};
    std::array<kcd2vr::passive169r1::Sample,6> passive169r1{};
    std::array<kcd2vr::physics::committed_push_h21::Target,2> passiveTargetsH21{};
    std::array<std::uint64_t,2> passiveQueryTickR11{};
    ModelWorld6692 passiveWorld169r1{};bool passiveWorldValid169r1=false;
    kcd2vr::diagnostics::bow169r2::Record bowEvidence169r2{};bool bowContext169r2=false;
    ph::Pose propRequested5916c1[2]{};bool propRequestedValid5916c1[2]{};
    handPolicyH24::Input handPresenceH24[2]{};bool handPresenceValidH24[2]{};
    std::uint64_t doorSerial66[2]{};
    std::uint64_t doorDisplayH40[2]{};std::uint32_t doorInputOwnerMaskH40[2]{};
    std::uint32_t mapPresentationOwnerM34[2]{kcd2vr::map_hand_owner_m34::Unknown,kcd2vr::map_hand_owner_m34::Unknown};
    bool physicalActive=false;ph::Input physicalInput{};ph::Output physicalOutput{};
    bool nativeHeldActiveT4=false;heldT4::Input nativeHeldInputT4{};heldT4::Achieved nativeHeldReceiptT4{};
    std::uint64_t nativeHeldAdmissionT4=0;
    kcd2vr::self_contact54::Sample selfContact54{};
    kcd2vr::self_contact54::ShieldSample shieldContact54{};
    ph::State physicalState{};std::array<ph::Profile,2> physicalProfiles{};PhysEquipment6698 physicalEquipment{};
    std::array<ph::HolsterState,8> physicalHolsters{};
    explicit CoherentTransaction6689():lock(g_pose_mutex6689){
        // Latch before calibration reads the camera reference. Existing epoch
        // checks retire this transaction if recenter interrupts it later.
        epoch=g_pose_epoch6689.load(std::memory_order_acquire);
        if(kcd2vr::diagnostics::motion_d68::requested()){
            namespace md=kcd2vr::diagnostics::motion_d68;md::Event e{};
            static md::Cadence cadence; // g_pose_mutex6689 owns this observer only.
            if(md::begin(e,md::CoreBefore)&&md::inWindow(e)){
                if(cadence.due(e.session,e.ns)){motionSessionD68=e.session;motionCycleD68=md::nextCycle();}
                else md::noteCadenceSkip(md::CoreBefore);
            }
        }
        if(lockStartNsD47){startNsD30=CoherentClockNsD47();
            lockWaitNsD47=startNsD30>=lockStartNsD47?startNsD30-lockStartNsD47:0;}
        if(kcd2vr::diagnostics::capture168::gate.mask.load(std::memory_order_relaxed)&(1u<<unsigned(kcd2vr::diagnostics::capture168::Domain::HandReference)))
            fingerDiagnostic28=kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::HandReference,GetTickCount64());
        if(kcd2vr::diagnostics::vrik_h29::selected()){
            const auto session=kcd2vr::diagnostics::capture168::gate.session[unsigned(kcd2vr::diagnostics::capture168::Domain::Vrik)].load(std::memory_order_acquire);
            traceH29=kcd2vr::diagnostics::vrik_h29::active(GetTickCount64())&&
                session==kcd2vr::diagnostics::capture168::gate.session[unsigned(kcd2vr::diagnostics::capture168::Domain::Vrik)].load(std::memory_order_acquire);
            traceSessionH29=session;
            // D67: F7 no longer selects a finger reference. The saved captured-fist
            // preference below remains the sole owner of this existing recipe.
        }
        // W1 (Miki: "fists from the codex fixes as the default fists"): the captured native punch CLOSED endpoint is the
        // normal fist in every rung; F7 rung 2 only records. Open hand, Grip/Trigger split and held-object hands unchanged.
        if(kcd2vr::finger_rig_w1::capturedFistDefault.load(std::memory_order_relaxed))fingerTrialRequestedH7=true;
        fingerSpringH31=kcd2vr::deepseek_h29::fingerContactSpringH31.load(std::memory_order_acquire);
        nativeHeldAdmissionT4=heldT4::AdmissionSerial();
        policyGeneration57=g_pose_policy_generation57.load(std::memory_order_acquire);
        kickCancel176=g_kick_cancel176.load(std::memory_order_acquire);
        kickSerial176=g_kick_busy176.load(std::memory_order_acquire);
    }
    void Restore() noexcept {
        if(restored)return;bool ok=true;
        std::array<bool,kPoseCapacity6689> failed{};
        if(setter&&pose)for(std::size_t i=g_pose6689.count;i-->0;)if(attempted[i]){
            if(traceH29)++traceRestoreWritesH29;
            const bool restoredJoint=WriteQuat(setter,pose,int(i),g_pose6689.original[i]);
            failed[i]=!restoredJoint;ok=restoredJoint&&ok;
        }
        if(traceH29||motionSessionD68)traceRestoreOkH29=ok;
        if(!ok){RecordPoseRestoreDebtR96(restoreOwnerR96,g_pose6689.original.data(),failed);
            diagnostic_trace::Write("V66.89-POSE RESTORE_FAILED poseWritesSuspended=1 debt=EXACT_NATIVE_OWNER");}
        restored=true;
    }
    ~CoherentTransaction6689(){Restore();MotionPoseCompleteD68(*this);kcd2vr::diagnostics::perf_d61::poseAge(tracking.timeNs,committed);if(traceH29)ObserveVrikH29(*this,committed,failureLine);if(startNsD30)ObserveFrameD30(*this,committed);
        if(physicalActive&&(kcd2vr::diagnostics::capture168::gate.mask.load(std::memory_order_relaxed)&(1u<<unsigned(kcd2vr::diagnostics::capture168::Domain::Interaction))))ObserveHeldF5D30(*this);}
};
static bool MotionEventD68(const CoherentTransaction6689& tx,kcd2vr::diagnostics::motion_d68::Event& e,
    kcd2vr::diagnostics::motion_d68::Stage stage) noexcept {
    namespace md=kcd2vr::diagnostics::motion_d68;
    if(!tx.motionSessionD68||!md::begin(e,stage,false)||e.session!=tx.motionSessionD68)return false;
    e.cycle=tx.motionCycleD68;e.actor=tx.motionActorD68;e.descriptor=tx.motionDescriptorD68;
    e.pose=tx.motionPoseD68;e.output=tx.motionOutputD68;e.poseEpoch=tx.epoch;
    e.sourceNs=tx.motionSourceNsD68;e.sourceTick=tx.motionSourceTickD68;e.sample=tx.motionSampleD68;
    e.trackingEpoch=tx.motionTrackingEpochD68;e.inputTick=tx.motionInputTickD68;e.targetXrNs=tx.motionTargetXrNsD68;
    e.publication=tx.publicationD49;e.thread=GetCurrentThreadId();return true;
}
static void MotionPoseCompleteD68(const CoherentTransaction6689& tx) noexcept {
    if(!tx.motionSessionD68)return;
    namespace md=kcd2vr::diagnostics::motion_d68;md::Event e{};
    if(!MotionEventD68(tx,e,md::PosePublished))return;
    e.count=8;e.v[0]=float(tx.traceNativeReturnedH29);e.v[1]=float(tx.restored);e.v[2]=float(tx.traceRestoreOkH29);
    e.v[3]=float(tx.motionCommittedD68);e.v[4]=float(tx.tracePacketPublishedH29);e.v[5]=float(tx.pairedPublishedD49);
    e.v[6]=float(tx.tracePublishedH29);e.v[7]=float(tx.consumersD49);e.flags=tx.failureLine?1u:0u;
    (void)md::push(e);
}
// Re-read the original raw-input-backed acquisition, not the composed/held pose.
// Used before the first write, before native skinning and before publication.
// Ordinary owners use copied mailboxes. The exact Reloading passenger also
// revalidates its installed native child, root and current CAF through the
// read-only provider; it never cancels or changes their semantic owner.
static bool CoherentOwnerCurrent57(const CoherentTransaction6689& tx) noexcept {
    if(tx.diceBodyD81&&(!kcd2vr::dice_body_d81::current(tx.diceLeaseD81,GetTickCount64())||
       WorkstationContextRefusal19c4r2(GetTickCount64(),true)||
       animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)))return false;
    if(tx.ridingGaitD76&&(!(tx.ridingModeD76&1)||
       tx.ridingModeD76!=kcd2vr::riding_vrik_d76::mode.load(std::memory_order_acquire)||
        tx.ridingLifetimeR95!=kcd2vr::riding_vrik_d76::lifetimeR95.load(std::memory_order_acquire)||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||
       animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)))return false;
    if(tx.roomOriginCapturedD76&&(kcd2vr::roomscale_d76::recenterPending.load(std::memory_order_acquire)||
       (tx.roomOriginD76.owner.epoch&&tx.roomOriginD76.owner.epoch!=g_cal_room_origin_d76.load(std::memory_order_acquire))))return false;
    if(tx.roomOriginCapturedD76&&!kcd2vr::native_roomscale_d76::SameSerial(
        kcd2vr::native_roomscale_d76::Surface::Character,reinterpret_cast<std::uintptr_t>(g_pose6689.character),tx.roomOriginD76.serial))return false;
    // Only a staged optional display experiment is revoked by this policy.
    // Ordinary tracking, action admission and collision modes keep their rules.
    if(tx.handPresentPolicyD61&&(!kcd2vr::hand_present_d61::CurrentPolicy(tx.handPresentPolicyD61)||
       !HandPresentContextD62()))return false;
    if(tx.handPresentPolicyD61)for(int side=0;side<2;++side)if(tx.handPresentOnlyD61[side]){
        if(kcd2vr::frik_pose_probe::ExactHandItemState(side)!=0||
           HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(g_pose6689.character),tx.epoch,g_weapon_request_epoch6694.load())||
           (g_wristOwnedByController59c12&&g_wristOwnedByController59c12(side))||
           std::uint64_t(GetTickCount64())<g_native_use_until107[side].load(std::memory_order_acquire))return false;
    }
    if(tx.instantPairHandsD72){kcd2vr::paired_hands_d72::Owner current{};
        if(!ReadInstantPairHandsD72(current,tx.pairHandsD72.character,tx.epoch,tx.tracking.source_epoch,GetTickCount64())||
           !kcd2vr::paired_hands_d72::same(current,tx.pairHandsD72))return false;}
    if(tx.reloadWorkingHandXR34&&(!g_currentReloadPassengerXR34||
       !g_currentReloadPassengerXR34(tx.reloadPassengerXR34,tx.reloadPassengerXR34.frame.character,tx.epoch,GetTickCount64())||
       g_cal_state.load()!=2||g_pose_restore_fault6689.load()||
       tx.policyGeneration57!=g_pose_policy_generation57.load()||tx.control51!=g_frame_control51.load()||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||
       tx.tracking.head_held||tx.tracking.right_held||tx.tracking.left_held||
       tx.tracking.source_epoch!=tx.reloadPassengerXR34.frame.tracking))return false;
    for(int side=0;side<2;++side)if(tx.doorSerial66[side]){std::uint64_t current=0;
        if(!g_doorActive66||!g_doorActive66(side,tx.epoch,GetTickCount64(),current)||current!=tx.doorSerial66[side])return false;}
    if(tx.nativeHeldAdmissionT4!=heldT4::AdmissionSerial())return false;
    if(tx.nativeHeldActiveT4){
        heldT4::Achieved current{};
        if(!NativeHeldSourceCurrentT4(tx.nativeHeldInputT4)||!heldT4::Read(current)||
           current.state!=heldT4::State::Active||!current.poseValid||current.colliderRetired||
           !kcd2vr::native_held_pose_t4::sameReceipt(tx.nativeHeldReceiptT4,current)||
           !kcd2vr::native_held_pose_t4::fresh(GetTickCount64(),tx.nativeHeldReceiptT4.sourceTick))return false;
    }else if(NativeHeldBusyT4())return false;
    if(tx.stationLeaseH41){
        // R9C: the same station conditions, named (first failure) and fed to the per-visit fallback.
        namespace sb=kcd2vr::station_body_h41;
        kcd2vr::workstation178r20c1::WorkstationOwnerH51 current{};sb::GateInput g{};
        g.frameValid=tx.frame51.valid;
        g.ownerNow=g.frameValid&&workstation178r20c1::workstationOwnerH51(GetTickCount64(),current);
        g.modeOk=int(kcd2vr::minigame_hands45::modeFor(current.kind))==sb::kMode;
        g.identityOk=current.kind==tx.frame51.workstationKindH51&&current.modeEpoch==tx.frame51.workstationModeEpochH51&&
           current.lifetime==tx.frame51.workstationLifetimeH51&&current.instance==tx.frame51.workstationInstanceH51;
        g.epochOk=tx.epoch==g_pose_epoch6689.load()&&tx.policyGeneration57==g_pose_policy_generation57.load()&&
           tx.control51==g_frame_control51.load()&&g_cal_state.load()==2&&!g_pose_restore_fault6689.load();
        g.trackingOk=tx.tracking.source_epoch&&tx.tracking.source_epoch==kcd2vr::tracking59c6::epoch.load();
        g.notHeld=!tx.tracking.head_held&&!tx.tracking.right_held&&!tx.tracking.left_held;
        // Native workstation controls share the menu flag but are not a real UI owner.
        // Reuse the existing coherent context word; true menus/dialogue/cutscenes still deny.
        g.contextOk=WorkstationContextRefusal19c4r2(GetTickCount64(),true)==nullptr&&
            !g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&(!g_ctx_reading.load()||current.alchemyBookAS1);
        g.bodyOk=!animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175();
        const auto why=sb::classify(g);
        sb::lastRefuse.store(unsigned(why),std::memory_order_relaxed);
        if(sb::note(sb::fallback,tx.frame51.workstationInstanceH51,tx.frame51.workstationLifetimeH51,tx.tracking.sequence,why)&&
           sb::fallbackEnabled.load(std::memory_order_relaxed)){
            sb::fallbackVisits.fetch_add(1,std::memory_order_relaxed);sb::fallbackWhy.store(unsigned(why),std::memory_order_relaxed);
            const auto now=std::uint64_t(GetTickCount64());
            char m[200]{};std::snprintf(m,sizeof(m),"R9C STATION_FALLBACK kind=%u instance=%llX lifetime=%llu why=%s refusals=%llu body=FREE_GESTURE",
                tx.frame51.workstationKindH51,(unsigned long long)tx.frame51.workstationInstanceH51,(unsigned long long)tx.frame51.workstationLifetimeH51,
                sb::refuseName(why),(unsigned long long)sb::refusals.load(std::memory_order_relaxed));
            diagnostic_trace::Write(m);
            char t[kcd2vr::visible_failure::kMaxText+1]{};
            std::snprintf(t,sizeof(t),"6DOF station: body step refused (%s) - VR hands, no room step",sb::refuseName(why));
            (void)kcd2vr::visible_failure::publish(t,now);
        }
        sb::fallbackNow.store(sb::fallbackFor(sb::fallback,tx.frame51.workstationInstanceH51,tx.frame51.workstationLifetimeH51),std::memory_order_relaxed);
        if(why!=sb::Refuse::None){sb::refusals.fetch_add(1,std::memory_order_relaxed);return false;}
    }
    // Free mode 2 has no optional station root-step lease, but its shared
    // body frame still belongs to one current native workstation. Rejoin the
    // same identity/context that the camera requires before publishing bones.
    // Tool holding and controller grace do not revoke this frame by themselves.
    if(tx.frame51.workstationKindH51&&!tx.stationLeaseH41){
        kcd2vr::workstation178r20c1::WorkstationOwnerH51 current{};
        const auto now=std::uint64_t(GetTickCount64());
        if(!tx.frame51.valid||!workstation178r20c1::workstationOwnerH51(now,current)||
           current.kind!=tx.frame51.workstationKindH51||current.modeEpoch!=tx.frame51.workstationModeEpochH51||
           current.lifetime!=tx.frame51.workstationLifetimeH51||current.instance!=tx.frame51.workstationInstanceH51||
           WorkstationContextRefusal19c4r2(now,true)!=nullptr||g_ctx_dialogue.load()||g_ctx_cutscene.load()||
           (g_ctx_reading.load()&&!current.alchemyBookAS1))return false;
    }
    // D47: ordinary arms also belong to the generation they solved. A live
    // preference/recenter may happen during native skinning; absence of a
    // pickup lease cannot authorize publishing that obsolete pose. This check
    // has no retained latch: the next transaction reads the new generation.
    if(tx.adoptedVisualRequiredR95){
        const auto& held=tx.adoptedVisualR95;const auto now=std::uint64_t(GetTickCount64());
        if(!action_time_owner59c11::AdoptedAttachmentPresentationCurrentR95(held)||
           held.binding.character!=reinterpret_cast<std::uintptr_t>(g_pose6689.character)||held.binding.epoch!=tx.epoch||
           held.trackingEpoch!=tx.tracking.source_epoch||held.trackingEpoch!=tracking59c6::epoch.load()||
           g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||
           WorkstationContextRefusal19c4r2(now,true)!=nullptr||animation_filter::nativeBodyOwned112.load()||
           animation_filter::NativeKickActive175()||g_kick_busy176.load()||diagnostics::lab::NativeReference()||
           HandClaimBlockedT24(held.binding.side,held.binding.character,tx.epoch,g_weapon_request_epoch6694.load()))return false;
    }
    if(!tx.acquisitionRequired57){
        const auto currentPolicyD47=g_pose_policy_generation57.load(std::memory_order_acquire);
        return !(currentPolicyD47&1)&&currentPolicyD47==tx.policyGeneration57&&
            tx.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
            tx.control51==g_frame_control51.load(std::memory_order_acquire)&&
            g_cal_state.load(std::memory_order_acquire)==2&&!g_pose_restore_fault6689.load(std::memory_order_acquire);
    }
    const auto policy=g_pose_policy_generation57.load(std::memory_order_acquire);
    const auto now=std::uint64_t(GetTickCount64());
    if((policy&1)||policy!=tx.policyGeneration57||g_pose_restore_fault6689.load(std::memory_order_acquire)||
       tx.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||g_cal_state.load(std::memory_order_acquire)!=2||
       tx.control51!=g_frame_control51.load(std::memory_order_acquire)||
       g_ctx_menu.load(std::memory_order_acquire)||g_ctx_dialogue.load(std::memory_order_acquire)||
       g_ctx_cutscene.load(std::memory_order_acquire)||g_ctx_reading.load(std::memory_order_acquire)||
       WorkstationContextRefusal19c4r2(now,true)!=nullptr||
       animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)||animation_filter::NativeKickActive175()||
       !tx.tracking.source_epoch||tx.tracking.source_epoch!=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire)||
       tx.tracking.head_held||tx.tracking.right_held||tx.tracking.left_held)return false;
    kcd2vr::grasp13::Acquisition54 current{};const auto& original=tx.acquisition54;
    if(!kcd2vr::grasp13::readFoundationD56(g_graspAcquisition54,current)||!current.active||
       !kcd2vr::grasp13::freshAcquisition54(current,now)||current.serial!=original.serial||
       current.action!=original.action||current.scope!=original.scope||current.nativeToken!=original.nativeToken||
       current.side!=original.side||current.character!=original.character||current.epoch!=original.epoch||
       current.startedAt!=original.startedAt||current.generation55!=original.generation55||
       current.trackingEpoch55!=original.trackingEpoch55||current.trackingEpoch55!=tx.tracking.source_epoch||
       (original.graspToken&&(current.graspToken!=original.graspToken||current.graspKey!=original.graspKey||
        current.graspItem!=original.graspItem||current.graspEntity!=original.graspEntity)))return false;
    // The getter checks fresh original Grip/source and irreversible retirement.
    // Recheck publication identity after it, so callback-side revocation cannot
    // pair a new policy/body/source with the transaction's older witness.
    return policy==g_pose_policy_generation57.load(std::memory_order_acquire)&&
        tx.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&g_cal_state.load(std::memory_order_acquire)==2&&
        tx.control51==g_frame_control51.load(std::memory_order_acquire)&&
        current.trackingEpoch55==kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
}
// Only the coherent pose owner, under g_pose_mutex6689, may mutate this latch
// or g_phys_state. A body can create+retire without ANY achieved publication.
#include "../subsystems/physics/pose_held_solver_stage.inl" // M2_OWNED_SOURCE
#include "passive_hand_evidence169r1.inl"

// Bounds-derived visual blade capsule. This is NOT a native collision/block
// witness. Sword classes only; an axe/mace/shield's longest bounds axis does not
// establish its contacting surface. No new engine read/call or body obstacle.
#include "../subsystems/physics/pose_self_contact.inl" // M2_OWNED_SOURCE
#include "../subsystems/physics/own_shield_clang58.inl"
#include "../subsystems/physics/pose_self_contact_publish.inl" // M2_OWNED_SOURCE
#include "../subsystems/vrik/pose_leg_solver.inl" // M2_OWNED_SOURCE
static void ObserveVrikH29(const CoherentTransaction6689& tx,bool committed,int line) noexcept {
    namespace vd=kcd2vr::diagnostics::vrik_h29;if(!tx.traceH29||!vd::selected())return;
    if(tx.traceSessionH29!=kcd2vr::diagnostics::capture168::gate.session[unsigned(kcd2vr::diagnostics::capture168::Domain::Vrik)].load(std::memory_order_acquire))return;
    const auto now=std::uint64_t(GetTickCount64());if(!vd::active(now))return;
    const auto& s=g_pose6689;vd::Record r{};r.kind=tx.traceStartedH29?vd::Kind::Cycle:vd::Kind::Skip;
    r.session=tx.traceSessionH29;r.tick=now;r.sample=tx.tracking.sequence;r.epoch=tx.epoch;r.trackingEpoch=tx.tracking.source_epoch;
    r.character=tx.traceCharacterH29;r.publication=g_frame_publication51.load();r.committed=committed;r.line=tx.tracePublishLineH29?tx.tracePublishLineH29:line;
    r.sourceTick=tx.tracking.tick;r.inputTick=tx.tracking.input_tick;r.timeNs=tx.tracking.timeNs;
    r.locateTargetXrNs=tx.tracking.poseTime;
    r.heldInputTargetXrNs[0]=tx.physicalInput.poseTime[0];r.heldInputTargetXrNs[1]=tx.physicalInput.poseTime[1];
    r.heldMotorPath=tx.physicalActive?(tx.nativeHeldActiveT4?2u:tx.physicalInput.dynamics?1u:3u):0u;
    r.goodHead=tx.tracking.head_good_tick;r.goodRight=tx.tracking.right_good_tick;r.goodLeft=tx.tracking.left_good_tick;
    r.currentTrackingEpoch=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);r.allowed=tx.tracking.allowed;
    r.rawPosition[0]=PV(tx.tracking.head);r.rawPosition[1]=PV(tx.tracking.right);r.rawPosition[2]=PV(tx.tracking.left);
    r.rawRotation[0]=PQ(tx.tracking.hq);r.rawRotation[1]=PQ(tx.tracking.rq);r.rawRotation[2]=PQ(tx.tracking.lq);
    const auto& frame=tx.traceFrameCapturedH29?tx.traceFrameH29:tx.frame51;
    r.frameValid=frame.valid;r.frameSample=frame.stamp.sample;r.frameTick=frame.stamp.tick;r.framePublication=frame.publication;
    r.frameRoot=frame.root;r.frameRotation=frame.rotation;r.frameScale=frame.scale;r.frameAnchor=frame.anchor;r.frameOffset=frame.bodyOffset;
    r.worldKnown=tx.traceWorldKnownH29;std::copy_n(tx.traceWorldH29,8,r.modelWorld.begin());
    r.policy=tx.policyGeneration57;r.control=tx.control51;
    r.context=(g_ctx_menu.load()?1u:0u)|(g_ctx_dialogue.load()?2u:0u)|(g_ctx_cutscene.load()?4u:0u)|
        (g_ctx_reading.load()?8u:0u)|(animation_filter::nativeBodyOwned112.load()?16u:0u)|(g_gameplay_allowed.load()?32u:0u);
    r.started=tx.traceStartedH29;r.nativeCalled=tx.traceNativeCalledH29;r.nativeReturned=tx.traceNativeReturnedH29;
    r.restored=tx.restored;r.restoreOk=tx.traceRestoreOkH29;r.restoreWrites=tx.traceRestoreWritesH29;
    r.publishAttempted=tx.tracePublishAttemptedH29;r.packetPublished=tx.tracePacketPublishedH29;
    r.published=tx.tracePublishedH29;r.envelopePublished=tx.traceEnvelopePublishedH29;
    r.proposedPublicationD49=tx.publicationD49;r.pairedPublishedD49=tx.pairedPublishedD49;r.consumersD49=tx.consumersD49;
    const char* end=!r.restoreOk?"RESTORE_FAILED":!r.started?"BEGIN_NOT_ENTERED":!committed?"SOLVE_OR_WRITE_REFUSED":
        r.published?"PUBLISHED":r.publishAttempted?"PUBLICATION_REFUSED":"PUBLICATION_NOT_ATTEMPTED";
    std::strncpy(r.end,end,sizeof(r.end)-1);
    std::strncpy(r.stage,tx.stage?tx.stage:"UNKNOWN",sizeof(r.stage)-1);r.tracking=(tx.tracking.head_valid?1u:0u)|(tx.tracking.right_valid?2u:0u)|(tx.tracking.left_valid?4u:0u)|(tx.tracking.head_held?8u:0u)|(tx.tracking.right_held?16u:0u)|(tx.tracking.left_held?32u:0u);
    r.poseData=tx.traceAnatomyH29&&r.character==reinterpret_cast<std::uintptr_t>(s.character)&&s.count>0&&s.count<=kPoseCapacity6689;
    const auto joint=[&](int id){vd::Joint j{};j.id=id;if(id<0||id>=int(s.count)||id>=int(kPoseCapacity6689))return j;j.parent=s.parent[id];j.valid=ValidPose6689(s.work[id])&&ValidPose6689(s.local[id]);j.selected=tx.selected[id];j.attempted=tx.attempted[id];j.position=PV(FromT(s.work[id]));j.rotation=PQ(QFromQuatT(s.work[id]));j.localPosition=PV(FromT(s.local[id]));j.localRotation=PQ(QFromQuatT(s.local[id]));return j;};
    if(r.poseData&&s.rig.pelvis>=0&&unsigned(s.rig.pelvis)<s.count){
        r.nativePelvis=PQ(QFromQuatT(s.original[s.rig.pelvis]));r.solvedPelvis=PQ(QFromQuatT(s.work[s.rig.pelvis]));r.pelvisKnown=true;
    }
    if(r.poseData)for(int side=0;side<2;++side){auto& a=r.arms[side];a.upper=joint(side?s.rig.upper_l:s.rig.upper_r);a.elbow=joint(side?s.rig.fore_l:s.rig.fore_r);a.wrist=joint(side?s.rig.hand_l:s.rig.hand_r);a.bend=s.elbow[side].bend;a.axis=s.elbow[side].axis;a.history=s.elbow[side].ready;a.historyTick=s.elbow[side].tick;a.upperLength=s.upper[side];a.foreLength=s.fore[side];a.mapped=s.fingerMap[side];a.open=s.open[side].ready;a.closed=s.closed[side].ready;a.fingers=unsigned(std::clamp(s.fingerCount[side],0,32));for(unsigned i=0;i<a.fingers;++i){auto& j=a.finger[i];j=joint(s.fingerIds[side][i]);for(int digit=0;digit<5;++digit)for(int k=0;k<s.fingers[side][digit].count;++k)if(s.fingers[side][digit].id[k]==j.id){j.digit=digit;j.segment=k;}}}
    if(r.poseData)for(int side=0;side<2;++side)r.arms[side].context=tx.traceArmH29[side];
    vd::push(r);
    // Four receipts/second, only in the current F7 session. The ordinary pose
    // record already contains the full requested/allowed/composed chain. This
    // adds the missing acceptance boundary: a failed pose cannot donate history.
    struct MotionReceiptD47 {
        std::uint64_t session=0,tick=0,seen=0,accepted=0,refused=0,reused=0;
    };
    static thread_local MotionReceiptD47 receiptD47;
    if(receiptD47.session!=tx.traceSessionH29){receiptD47={};receiptD47.session=tx.traceSessionH29;}
    ++receiptD47.seen;if(tx.motionD47.published)++receiptD47.accepted;else ++receiptD47.refused;
    if(tx.traceArmH29[0].priorReady&&tx.traceArmH29[0].priorSample==tx.tracking.sequence)++receiptD47.reused;
    if(!receiptD47.tick||now<receiptD47.tick||now-receiptD47.tick>=250){
        char m[576]{};std::snprintf(m,sizeof(m),
            "VRIK_F7 ARM_HISTORY_D47 sample=%llu staged=%d accepted=%d historySampleR/L=%llu/%llu updatesR/L=%llu/%llu handoverR/L=%.4f/%.4f windowSeen=%llu accepted=%llu refused=%llu reusedRight=%llu finiteSwing=1 nativeConsumer=UNPROVEN",
            (unsigned long long)tx.tracking.sequence,tx.motionD47.ready,tx.motionD47.published,
            (unsigned long long)s.elbow[0].sample,(unsigned long long)s.elbow[1].sample,
            (unsigned long long)s.elbow[0].updates,(unsigned long long)s.elbow[1].updates,
            double(s.handoverW178r14[0]),double(s.handoverW178r14[1]),
            (unsigned long long)receiptD47.seen,(unsigned long long)receiptD47.accepted,
            (unsigned long long)receiptD47.refused,(unsigned long long)receiptD47.reused);
        diagnostic_trace::Write(m);receiptD47.tick=now;
        receiptD47.seen=receiptD47.accepted=receiptD47.refused=receiptD47.reused=0;
    }
}
// D30: copy of numbers this transaction already holds (raw tracking, final model-space wrists after the
// scoped write, F7 arm context). No native read; the frame_d30 windows are filled only under F2 / F7.
static void ObserveFrameD30(const CoherentTransaction6689& tx,bool committed) noexcept {
    namespace fd=kcd2vr::diagnostics::frame_d30;fd::PoseSample p{};p.tick=std::uint64_t(GetTickCount64());p.sequence=tx.tracking.sequence;p.committed=committed;
    p.timeNs=tx.tracking.timeNs;
    p.motionContext={tx.epoch,tx.tracking.source_epoch,std::uint64_t(reinterpret_cast<std::uintptr_t>(g_pose6689.character)),
        tx.policyGeneration57,tx.control51,kcd2vr::hand_smooth_h50::policyEpoch.load(std::memory_order_acquire),
        (std::uint64_t(kcd2vr::smooth_lab::Mode())<<1)|(kcd2vr::frik_elbow_lab::Active()?1ull:0ull)};
    const auto nowNs=std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
    p.transactionUs=tx.startNsD30&&nowNs>=tx.startNsD30?double(nowNs-tx.startNsD30)/1000.:-1.;
    p.lockWaitUs=tx.startNsD30?double(tx.lockWaitNsD47)/1000.:-1.;
    p.prepareUs=tx.prepareNsD47?double(tx.prepareNsD47)/1000.:-1.;
    const V3 raw[3]{tx.tracking.head,tx.tracking.right,tx.tracking.left};const bool ok[3]{tx.tracking.head_valid&&!tx.tracking.head_held,tx.tracking.right_valid&&!tx.tracking.right_held,tx.tracking.left_valid&&!tx.tracking.left_held};
    for(int i=0;i<3;++i){p.rawValid[i]=ok[i];p.raw[i][0]=raw[i].x;p.raw[i][1]=raw[i].y;p.raw[i][2]=raw[i].z;}
    const auto& s=g_pose6689;
    if(committed&&s.character&&s.count>0&&s.count<=kPoseCapacity6689)for(int side=0;side<2;++side){
        const int id=side?s.rig.hand_l:s.rig.hand_r;
        if(id>=0&&id<int(s.count)&&ValidPose6689(s.work[id])){const auto w=FromT(s.work[id]);p.finalValid[side]=true;p.finalWrist[side][0]=w.x;p.finalWrist[side][1]=w.y;p.finalWrist[side][2]=w.z;}
    }
    p.frameValid=tx.frame51.valid;p.traced=tx.traceH29;
    if(tx.traceH29)for(int side=0;side<2;++side){const auto& a=tx.traceArmH29[side];
        const double dx=a.goal.x-a.requested.x,dy=a.goal.y-a.requested.y,dz=a.goal.z-a.requested.z;const double mm=std::sqrt(dx*dx+dy*dy+dz*dz)*1000.;
        p.goalValid[side]=a.entered&&std::isfinite(mm);p.goalOffsetMm[side]=mm;p.planes[side]=a.planes;p.elbowOk[side]=!a.entered||a.solveOk;p.fingerOk[side]=a.fingerCall&&a.fingerOk;}
    fd::pose(p);
}
// D30 F5 HELD (Miki: "better diagnostics for ... items held and their dynamics"): per hand that holds a physical
// profile, 20 Hz + key edges, model space (Henry root removed): requested controller hand, final hand, item pose,
// item-in-hand offset (mm / deg), linear + angular speed of item and controller from real time steps.
static void ObserveHeldF5D30(const CoherentTransaction6689& tx) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;namespace pc=kcd2vr::pose_core;
    const auto now=std::uint64_t(GetTickCount64());if(!cap::active(cap::Domain::Interaction,now))return;
    struct Prev {std::uint64_t key=0,ns=0,printed=0,session=0;ph::Pose item{},hand{};bool have=false;};
    static thread_local Prev prev[2];
    const auto ns=std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
    const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire);
    for(int side=0;side<2;++side){
        const auto& pr=tx.physicalProfiles[side];auto& p=prev[side];
        if(!pr.key){if(p.key&&p.session==session){char m[160]{};std::snprintf(m,sizeof(m),"F5 HELD side=%d event=RELEASED key=%016llX",side,(unsigned long long)p.key);diagnostic_trace::Write(m);}p={};p.session=session;continue;}
        const auto& item=tx.physicalOutput.weapons[side];const auto& hand=tx.physicalOutput.hands[side];const auto& req=tx.physicalInput.hands[side];
        if(!ph::finite(item)||!ph::finite(hand))continue;
        const bool edge=p.key!=pr.key||p.session!=session;
        double v=0,w=0,hv=0;
        if(p.have&&!edge&&ns>p.ns){const double dt=double(ns-p.ns)/1e9;if(dt>0&&dt<.25){v=pc::length(item.p-p.item.p)/dt;w=pc::angle(item.q,p.item.q)*57.2958/dt;hv=pc::length(req.p-p.hand.p)/dt;}}
        p.item=item;p.hand=req;p.ns=ns;p.have=true;
        if(!edge&&p.printed&&now>=p.printed&&now-p.printed<50){continue;}
        p.key=pr.key;p.session=session;p.printed=now;
        const auto rel=ph::compose(ph::inverse(hand),item);
        char m[720]{};std::snprintf(m,sizeof(m),
            "F5 HELD side=%d event=%s key=%016llX generation=%llu entity=%u class=%d primary=%d support=%d owned=%d active=%d blocked=%d native=%d "
            "itemModel=(%.3f,%.3f,%.3f) handFinal=(%.3f,%.3f,%.3f) handRequested=(%.3f,%.3f,%.3f) handLagMm=%.1f itemInHandMm=(%.1f,%.1f,%.1f) itemInHandDeg=%.1f "
            "itemSpeedMs=%.2f itemAngDegS=%.0f controllerSpeedMs=%.2f weightProxy=%.2f space=MODEL",
            side,edge?"ACQUIRED_OR_NEW_SESSION":"SAMPLE",(unsigned long long)pr.key,(unsigned long long)pr.generation,pr.entity,pr.classId,pr.primary?1:0,pr.support?1:0,
            tx.physicalOutput.owned[side]?1:0,tx.physicalOutput.active[side]?1:0,tx.physicalOutput.blocked[side]?1:0,tx.nativeHeldActiveT4?1:0,
            item.p.x,item.p.y,item.p.z,hand.p.x,hand.p.y,hand.p.z,req.p.x,req.p.y,req.p.z,pc::length(hand.p-req.p)*1000.,
            rel.p.x*1000.,rel.p.y*1000.,rel.p.z*1000.,pc::angle(rel.q,pc::Quat{})*57.2958,v,w,hv,pr.weight);
        diagnostic_trace::Write(m);
    }
}
// Called only on the existing exact-Henry callback, with its already-copied
// snapshot and eligibility result. No extra native read or context query.
static void ObserveVrikSkipH29(CoherentTransaction6689& tx,const TrackingSnapshot& ts,void* character,
        const char* reason,int line) noexcept {
    if(!tx.traceH29)return;tx.tracking=ts;tx.epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    tx.traceCharacterH29=reinterpret_cast<std::uintptr_t>(character);tx.stage=reason;tx.failureLine=line;
}
static bool RejectCoherent6693(CoherentTransaction6689& tx,int line) noexcept {
    tx.failureLine=line;
    if(diagnostics::ownership48::active(GetTickCount64())){
        diagnostics::ownership48::Event e{};e.stage=diagnostics::ownership48::PoseSkip;e.tick=GetTickCount64();e.thread=GetCurrentThreadId();e.sample=tx.tracking.sequence;e.epoch=tx.epoch;e.sourceEpoch=tx.tracking.source_epoch;e.sourceNs=tx.tracking.timeNs;e.poseTime=tx.tracking.poseTime;e.code=line;
        diagnostics::ownership48::name(e,tx.stage);(void)diagnostics::ownership48::push(e);
    }

    for(int side=0;side<2;++side){(void)g_hand_presence_source_h24[side].publish({});handPresenceH24::CancelPresenceH24(side);}
    // Only a real achieved transaction may revoke its solver on a later IK/
    // publication failure. Pending create/config waits retain their live intent.
    if(tx.nativeHeldActiveT4)heldT4::Retire(tx.nativeHeldReceiptT4.owner);
    RetireTrackedFrame51();
    // A failed pose must not replay a pending kick later. State remains staged;
    // the request serial CAS cannot clear a cancel/requeue from another thread.
    if(tx.kickSerial176){auto own=tx.kickSerial176;
        if(g_kick_busy176.compare_exchange_strong(own,0,std::memory_order_acq_rel))
            g_kick_status176.store(unsigned(kick176::Status::PoseRejected),std::memory_order_release);
    }
    if(tx.bowContext169r2){auto r=tx.bowEvidence169r2;r.stage=1;r.line=unsigned(line);r.committed=tx.committed;
        kcd2vr::diagnostics::bow169r2::push(r);}
    return false;
}
#ifdef KCD2VR_REACH_VIEW38_H
static kcd2vr::core::SnapshotMailbox<kcd2vr::reach_view38::Evidence> g_reach_view38; // C38: last committed frame, F5 rung 1 only
#endif
static bool BeginCoherent6689(CoherentTransaction6689& tx,void* pose,void* owner,void* character,void* anim,
                              std::uint32_t count,const RigMap& rig,PoseGetFn get,int rung,const TrackingSnapshot& ts) noexcept {
    // Exact preparation scope: anatomy/input reads, solver/contact and temporary
    // bone writes. Original native skinning, Restore and publication are outside.
    struct PrepareClockD47 {
        CoherentTransaction6689& tx;std::uint64_t begin;
        explicit PrepareClockD47(CoherentTransaction6689& v) noexcept:tx(v),begin(v.startNsD30?CoherentClockNsD47():0){}
        ~PrepareClockD47(){if(begin){const auto end=CoherentClockNsD47();if(end>=begin)tx.prepareNsD47+=end-begin;}}
    } prepareClockD47(tx);
    auto& s=g_pose6689;s.deferPassengers137=false;s.pending137.fill(false);
    kcd2vr::reading_pose32::Lease readingLease32{};
    kcd2vr::workstation178r20c1::WorkstationOwnerH51 bookOwnerAS1{};
    const bool alchemyBookBodyAS1=kcd2vr::workstation178r20c1::workstationOwnerH51(GetTickCount64(),bookOwnerAS1)&&bookOwnerAS1.alchemyBookAS1;
    const bool readingVisual32=!alchemyBookBodyAS1&&kcd2vr::reading_pose32::read(readingLease32,GetTickCount64());
    tx.stage="ANATOMY_OR_NATIVE_SNAPSHOT";tx.pose=pose;tx.tracking=ts;
    if(tx.motionSessionD68){tx.motionSourceNsD68=ts.timeNs;tx.motionSourceTickD68=ts.tick;tx.motionSampleD68=ts.sequence;
        tx.motionTrackingEpochD68=ts.source_epoch;tx.motionInputTickD68=ts.input_tick;tx.motionTargetXrNsD68=ts.poseTime;}
    tx.doorRequestH40=g_weapon_request_epoch6694.load(std::memory_order_acquire); // original BEFORE solve
    if(tx.traceH29){tx.traceStartedH29=true;tx.traceCharacterH29=reinterpret_cast<std::uintptr_t>(character);}
    tx.reloadWorkingHandXR34=g_readReloadPassengerXR34&&
        g_readReloadPassengerXR34(tx.reloadPassengerXR34,reinterpret_cast<std::uintptr_t>(character),tx.epoch,GetTickCount64());
    const auto reloadHandPolicyR95=kcd2vr::crossbow_native_xr34::handPolicyR95(tx.reloadPassengerXR34);
    tx.instantPairHandsD72=ReadInstantPairHandsD72(tx.pairHandsD72,reinterpret_cast<std::uintptr_t>(character),
        tx.epoch,ts.source_epoch,GetTickCount64());
    tx.diceBodyD81=kcd2vr::dice_body_d81::read(tx.diceLeaseD81,GetTickCount64())&&
        tx.diceLeaseD81.character==reinterpret_cast<std::uintptr_t>(character)&&tx.diceLeaseD81.bodyEpoch==tx.epoch&&
        !WorkstationContextRefusal19c4r2(GetTickCount64(),true);
    if(activity_input163::witness175.dice.load(std::memory_order_acquire)&&!tx.diceBodyD81){
        tx.stage="DICE_WAIT_CURRENT_CAMERA_BODY_ANCHOR";return RejectCoherent6693(tx,__LINE__);
    }
    if(animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)&&!tx.reloadWorkingHandXR34&&!tx.instantPairHandsD72){
        tx.stage="NATIVE_BODY_NO_CURRENT_RELOADING_PASSENGER";return RejectCoherent6693(tx,__LINE__);
    }
    if(g_pose_restore_fault6689.load()||!InitCoherent6689(s,owner,character,pose,get,count,rig)||!ReadNativeBuffer6689(pose,get,count,s.original.data()))return RejectCoherent6693(tx,__LINE__);
    const std::uint64_t composeStart137=s.composedJoints;
    for(std::uint32_t j=0;j<count;++j)if(!ValidPose6689(s.original[j]))return RejectCoherent6693(tx,__LINE__);
    tx.restoreOwnerR96=PoseRestoreOwnerR96(owner,character,pose,count);
    if(tx.traceH29)tx.traceAnatomyH29=true;
    PhysCaptureSamePose101(character,s.original.data(),count,rig,ts,s.rest.data(),s.parent.data(),s.rightHandSlotR26);
    if(tx.fingerDiagnostic28){s.objectObserved28[0]=s.objectObserved28[1]=false;s.freeObserved28[0]=s.freeObserved28[1]=false;}
#include "../subsystems/vrik/pose_reference_stage.inl" // M2_OWNED_SOURCE
    tx.stage="TRACKING_OR_SETTER";
    const auto validTrackedQ=[](Q4 q){const double n=double(q.x)*q.x+double(q.y)*q.y+double(q.z)*q.z+double(q.w)*q.w;return std::isfinite(n)&&n>.5&&n<1.5;};
    if(!validTrackedQ(ts.hq)||(ts.right_valid&&(!validTrackedQ(ts.rq)||!pc::finite(PV(ts.right))||!std::isfinite(ts.right_grip)||!std::isfinite(ts.right_trigger)))||
       (ts.left_valid&&(!validTrackedQ(ts.lq)||!pc::finite(PV(ts.left))||!std::isfinite(ts.left_grip)||!std::isfinite(ts.left_trigger)))||!pc::finite(PV(ts.head)))return RejectCoherent6693(tx,__LINE__);
    if(g_cal_state.load(std::memory_order_acquire)!=2){tx.stage="CALIBRATION_NOT_READY";return RejectCoherent6693(tx,__LINE__);}
#include "../subsystems/interactions/pose_acquisition_admission.inl" // M2_OWNED_SOURCE
#include "../subsystems/vrik/pose_tracking_goals.inl" // M2_OWNED_SOURCE
    // Alchemy passenger preserves the authored torso/head that the camera
    // follows. Only free arms use tracked endpoints; exact work grips remain
    // native below. F5=2 explicitly retains the full tracked-body experiment.
    const bool passengerAlchemy63=workstation178r20c1::passengerAlchemy63(acquisitionNow54);
    // D81/R97: right after native mounting (no extra delay since R97), riding
    // enters this existing normal body path. The flag only identifies the
    // mounted coordinate context; it does not remove contacts, weapons or fingers.
    const bool full=kcd2vr::riding_vrik_d76::UpperBodyD80(rung,passengerAlchemy63,tx.reloadWorkingHandXR34,tx.instantPairHandsD72);
    tx.stowedFullD63=full;
    // Normal full VRIK uses the same free-hand surface solver as F10.
    // In particular weapon/reload return to full pose3 no longer drops contacts.
    // Input/fists/weapon ownership is independent; do not enter exclusive Hands.
    // The primary weapon grip and active working-hand mechanism retain
    // their exact reload owner. Only an actually free working hand uses the
    // ordinary bounded contact solver; the native reload clock stays separate.
    const bool collisionMode=rung>=3&&(!tx.reloadWorkingHandXR34||
        reloadHandPolicyR95.worldContact[0]||reloadHandPolicyR95.worldContact[1]);
    // D75: a current native FREE/6DOF station uses the bounded palm path.
    // Native mode, ordinary gameplay, disabled contacts and held tools keep
    // their existing owners. Contact selection does not depend on F2/F5/F7.
    kcd2vr::workstation178r20c1::WorkstationOwnerH51 stationContactOwnerD75{};
    const bool stationContactD75=kcd2vr::station_hand_contact_d75::selected(
        !nativeFingerCapture&&workstation178r20c1::workstationOwnerH51(GetTickCount64(),stationContactOwnerD75),
        diagnostics::lab::NativeReference(),collisionMode);
    // Balanced mode adds two stable input-driven tips to the palm. Stations
    // retain their five-point permissive policy; no mode skips world queries.
    const bool balancedContactD79=kcd2vr::contact_balanced_d79::selected(
        ph::handsModeDs2.load(std::memory_order_relaxed),stationContactD75);
    // A legacy precise toggle can coexist with saved mode2. Balanced owns the
    // complete recipe, including self/own-blade contact, until another mode is chosen.
    const bool preciseContactModeR24=ph::preciseContacts44.load(std::memory_order_relaxed)&&!stationContactD75&&!balancedContactD79;
    const bool perfContactModeDs2=false; // mode2 now queries bounded palm+tips; no world-skip mode
    const auto contactControl=g_contact_control_epoch6695.load(std::memory_order_acquire);
    if(contactControl!=s.contactControl){s.contactControl=contactControl;s.lastContactPass=0;s.contact[0]={};s.contact[1]={};}
    for(int side=0;side<2;++side){
        tx.fingerTrialH7[side]=tx.fingerTrialRequestedH7&&s.nativeCurveClosedH7[side].ready;
        // Proximity is an interaction hint, never a free-finger pose owner.
        // Clear before both contact FK and the final writer; Grip/Trigger and
        // the accepted fist recipe remain authoritative until actual contact.
        s.hoverW178r14[side]=0.f;
        tx.fingerOpenBiasH7[side]=0.f;
        if(s.fingerTrialRecipeH7[side]!=tx.fingerTrialH7[side]){
            s.fingerTrialRecipeH7[side]=tx.fingerTrialH7[side];s.contact[side]={};s.lastContactPass=0;
        }
    }
    // Visual-only dialogue/activity poses cannot run weapon/holster intent.
    const bool physicalMode=!tx.reloadWorkingHandXR34&&g_weapon_control_mode.load()>=4&&!nativeFingerCapture&&g_gameplay_allowed.load(std::memory_order_acquire);
    // C55: held physical weapons always keep bounded world sweeps. The optional
    // hand/finger setting and F4 experiment do not revoke weapon geometry.
    const bool smithContactPassR38=kcd2vr::station_ws_j27::toolCollision.load(std::memory_order_relaxed)&&
        workstation178r20c1::gestureSmith67(ts.tick);
    const bool alchemyContactPassD55=kcd2vr::alchemy_tool_contact_d55::enabled.load(std::memory_order_relaxed)&&
        workstation178r20c1::fullVrAlchemy63(ts.tick)&&
        minigame_hands45::modeFor(minigame_hands45::Alchemy)==minigame_hands45::SixDofVrik;
    ContactPass6695 contactPass(collisionMode||physicalMode||smithContactPassR38||alchemyContactPassD55,ts.tick,s.lastContactPass,ts.sequence,ts.source_epoch,ts.timeNs);
    // Stage the relatively large contact payload in-place; no second per-frame
    // array/copy is needed merely to keep it alive through native skinning.
    auto& stagedContact=tx.motionD47.contact;
    std::copy_n(s.contact,2,stagedContact);
    std::uint64_t heldFingerKey[2]{};
    for(int side=0;side<2;++side)if(valid[side])NativeHeldFingerKey101(side,heldFingerKey[side]);
    if(!collisionMode){
        s.allowedReady[0]=s.allowedReady[1]=false;s.collisionEpoch=0;if(!physicalMode&&!smithContactPassR38&&!alchemyContactPassD55)s.lastContactPass=0;s.contact[0]={};s.contact[1]={};
        s.handBlocked[0]=s.handBlocked[1]=s.armBlocked[0]=s.armBlocked[1]=s.armSaturated[0]=s.armSaturated[1]=false;
    }
    s.dirtyAll=true;
    for(std::uint32_t j=0;j<count;++j) {
        const int p=s.parent[j];s.local[j]=p>=0?RelativeFromAbsolute(s.original[p],s.original[j]):s.original[j];
        tx.selected[j]=full?s.torso[j]:(s.armR[j]&&valid[0])||(s.armL[j]&&valid[1]);
        // Neutral ownership of torso/arm helpers; authored fingers and facial motion
        // remain native until separately overridden by a complete endpoint cache.
        const bool trunk=s.trunk[j];
        const bool ownArm=(s.armR[j]&&valid[0])||(s.armL[j]&&valid[1]);
        if((full&&trunk&&s.torso[j])||(ownArm&&!s.isFinger[j])||(full&&(int(j)==rig.clav_l||int(j)==rig.clav_r)))s.local[j]=s.restLocal[j];
    }
    if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);s.neutral=s.work;
    // One current qualified equipment/profile snapshot for both the torso and
    // physical kernel. Never read an unqualified older published bow frame.
    tx.physicalInput.epoch=tx.epoch;tx.physicalInput.bodyEpoch=tx.epoch;
    tx.physicalInput.sample=ts.sequence;tx.physicalInput.tick=ts.tick;
    const bool prepared169r2=physicalMode&&PhysPrepare6698(tx.physicalInput,tx.physicalProfiles,tx.physicalEquipment,&tx.physicalCorrelation126);
    if(NativeHeldBusyT4()&&!prepared169r2){heldT4::Cancel();tx.stage="T4_RETIRING_NO_CURRENT_NATIVE_SOURCE";return RejectCoherent6693(tx,__LINE__);}
    const auto preparedRequest169r2=tx.physicalInput.epoch;
#include "../subsystems/ranged_reload/pose_bow_admission.inl" // M2_OWNED_SOURCE
#include "../subsystems/vrik/pose_frame_foundation.inl" // M2_OWNED_SOURCE
#include "../subsystems/vrik/pose_mapping_trace_d68.inl" // Shift+F2 copied values only
    if(tx.traceH29){tx.traceFrameH29=tx.frame51;tx.traceFrameCapturedH29=true;}
#include "../subsystems/ranged_reload/pose_archer_input.inl" // M2_OWNED_SOURCE
    // Neutral must always be complete. Only subsequent solver edits may defer
    // passenger descendants; the final settle occurs before any invariant/commit.
    s.deferPassengers137=g_compose_defer137.load(std::memory_order_relaxed)&&s.essentialCount137>0&&s.essentialCount137<s.count;
#include "../subsystems/vrik/pose_follow_input.inl" // M2_OWNED_SOURCE
    auto stagedKick176=s.kick176; kick176::Input kickInput176{};
    const auto kickBusy176=g_kick_busy176.load(std::memory_order_acquire);
    kickInput176.cancel=g_kick_cancel176.load(std::memory_order_acquire);
    kickInput176.epoch=tx.epoch;kickInput176.tick=ts.tick;kickInput176.sample=ts.sequence;
    kickInput176.now=std::uint64_t(GetTickCount64());
    kick176::Request kickRequest176{};
    if(g_kick_request176.read(kickRequest176)&&kickRequest176.serial==kickBusy176&&
       kickRequest176.cancel==kickInput176.cancel)kickInput176.request=kickRequest176;
    if(stagedKick176.active&&stagedKick176.serial!=kickBusy176){stagedKick176.active=false;stagedKick176.havePreviousWorld=false;}
    kickInput176.eligible=full&&CombatExperiment175Enabled()&&kickBusy176&&
        !kcd2vr::kick26::NativeBlocksVrLeg(animation_filter::NativeKickActive175())&&ts.head_valid&&ts.right_valid&&ts.left_valid&&
        !ts.head_held&&!ts.right_held&&!ts.left_held;
    kickInput176.legsReady=s.legsReady128;kickInput176.forceSide=kcd2vr::kick26::ForcedSide(); // C26
    kickInput176.profile=kcd2vr::kick26::Profile52();kickInput176.reachScale=kcd2vr::kick26::ReachScale52(); // C52 KICKS page
    if(!stagedKick176.active&&kickInput176.request.serial&&kickInput176.request.haveTarget){ // C52: pending request -> opponent belly into model space (one transform read, only until the leg starts)
        ModelWorld6692 mw52{};const auto& w=kickInput176.request.targetWorld;
        if(ReadModelWorld6692(character,mw52)&&std::isfinite(w.x)&&std::isfinite(w.y)&&std::isfinite(w.z)){
            kickInput176.target=PV(WorldToModel6692(mw52,V3{float(w.x),float(w.y),float(w.z)}));kickInput176.haveTarget=true;}
    }
    for(int side=0;side<2;++side)if(s.leg[side].ok){const auto& leg=s.leg[side];
        kickInput176.hip[side]=PV(FromT(s.work[leg.hip]));
        kickInput176.ankle[side]=PV(FromT(s.original[leg.ankle]));
        kickInput176.reach[side]=leg.thigh+leg.shin;
    }
    tx.bodyFollowR38=s.bodyFollowR38;
    if(!full)tx.bodyFollowR38=kcd2vr::body_follow_r38::State{};
    // State is transactional too: failed or repeated pose consumers do not feed
    // another temporal update back into the next eye/frame.
    pc::ElbowContinuity stagedElbow[2]{s.elbow[0],s.elbow[1]};
    float stagedHandoverD47[2]{s.handoverW178r14[0],s.handoverW178r14[1]};
    std::uint64_t stagedNaturalTickD47[2]{s.naturalTick178r14[0],s.naturalTick178r14[1]};
    kcd2vr::elbow_shoulder_d45::ShoulderState stagedShoulderFitD45[2]{s.shoulderFitD45[0],s.shoulderFitD45[1]};
    std::uint64_t stagedElbowPolicyD45[2]{s.elbowPolicyD45[0],s.elbowPolicyD45[1]};
    const auto motionModeD67=kcd2vr::motion_trials_d67::ModeAt(std::uint64_t(GetTickCount64()));
    const bool elbowTrialD67=kcd2vr::motion_trials_d67::ElbowSelectedForModeR92(motionModeD67);
    const bool elbowTrialD45=!elbowTrialD67&&!motionModeD67&&kcd2vr::smooth_lab::ElbowShoulderSelectedD45();
    const bool elbowHysteresisD69=kcd2vr::claude_motion_d69::ElbowHysteresis();
    const bool elbowR89=kcd2vr::elbow_r89::Active(); // R90: optional R89 hand-guided swivel (Experiments > Elbow preference)
    const bool continuityD67=kcd2vr::motion_trials_d67::ContinuityRepair();
    const auto elbowPolicyBaseD45=(elbowTrialD67||elbowTrialD45||continuityD67)?
        kcd2vr::motion_trials_d67::Policy(motionModeD67)^(elbowTrialD45?(kcd2vr::hand_smooth_h50::policyEpoch.load(std::memory_order_acquire)<<32):0ull):0ull;
    const auto elbowPolicyD45=elbowPolicyBaseD45^(elbowHysteresisD69?(kcd2vr::claude_motion_d69::elbowPolicy.load(std::memory_order_acquire)<<40):0ull);
    pc::ForearmPronation stagedPronation[2]{s.pronation[0],s.pronation[1]};
    pc::ReachAssist stagedClavicle[2]{s.clavicleAssist[0],s.clavicleAssist[1]};
    pc::ShoulderState103 stagedShoulder[2]{s.shoulder[0],s.shoulder[1]};
#include "../subsystems/vrik/pose_torso_stage.inl" // M2_OWNED_SOURCE
#include "../subsystems/vrik/pose_reach_input.inl" // M2_OWNED_SOURCE
#include "../subsystems/base/pose_physical_executor.inl" // M2_OWNED_SOURCE
    bool propHeld5916c1[2]{};ph::Pose propWrist5916c1[2]{};
    if((workstation178r20c1::ownsWrist(GetTickCount64())||
        kcd2vr::instant_pickup_d72::active.load(std::memory_order_acquire))&&!nativeFingerCapture){
        ModelWorld6692 currentWorld{};
        if(ReadModelWorld6692(character,currentWorld)&&std::isfinite(currentWorld.scale)&&currentWorld.scale>.1f){
            const ph::Pose world{PQ(currentWorld.q),PV(currentWorld.t)};
            for(int side=0;side<2;++side){
                if(!valid[side]||ts.head_held||(side?ts.left_held:ts.right_held)||heldFingerKey[side]||
                   (tx.physicalActive&&tx.physicalOutput.owned[side]))continue;
                ph::Pose wrist{};
                if(!kcd2vr::prop5916c1::select(reinterpret_cast<std::uintptr_t>(character),tx.epoch,
                    workstation178r20c1::experimentEpoch36.load(),GetTickCount64(),side,wrist,
                    kcd2vr::mutual_admission_t25::pickupClaimsD72[side].load(std::memory_order_acquire)))continue;
                wrist=ph::compose(ph::inverse(world),wrist);wrist.p=wrist.p*(1./currentWorld.scale);
                const auto shoulder=PV(FromT(s.work[side?rig.upper_l:rig.upper_r]));
                if(pc::length(wrist.p-PV(rawGoalsH50[side]))>.35||pc::length(wrist.p-shoulder)>ph::reach_max(s.upper[side],s.fore[side]))continue;
                propHeld5916c1[side]=true;propWrist5916c1[side]=wrist;goals[side]=VP(wrist.p);
            }
        }
    }
#include "../subsystems/doors/pose_handle_stage.inl" // M2_OWNED_SOURCE
    // One free-hand contact owner for world, fingers and the F5 body proxy.
    // An equipped item still owns its grip when the physical solver has no frame.
    // Unknown occupancy cannot authorize a free-hand correction. No new native calls.
    // A native tool return owns the attachment/action, not the tracked wrist in
    // Free/6DOF alchemy. Keep native and semi-passenger wrist ownership separate.
    const auto alchemyOwnerNowR26=std::uint64_t(GetTickCount64());
    const bool alchemyPassengerScene42=!workstation178r20c1::fullVrAlchemy63(alchemyOwnerNowR26)&&
        (workstation178r20c1::nativeAlchemy42(alchemyOwnerNowR26)||
         kcd2vr::subsystems::alchemy::bridge67::visualPassenger71(alchemyOwnerNowR26))&&!readingVisual32;
    bool alchemyHeldPassenger42[2]{},nativePassengerHand46[2]{},nativeSmithFixtureH21[2]{},nativeHeatingHandD56[2]{};
    const auto passengerNow46=std::uint64_t(GetTickCount64());
    const bool trackedSmith46=workstation178r20c1::ownsActivityWrist45(passengerNow46)&&
        kcd2vr::activity_input163::witness175.active[unsigned(kcd2vr::activity_input163::Kind175::Smithing)].load(std::memory_order_acquire);
    ModelWorld6692 smithWorld{};CommittedFrame6695 previousSmith{};
    const auto smithMode=kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Smithing);
    const bool smithWorldKnown=trackedSmith46&&(smithMode==1||smithMode==2)&&ReadModelWorld6692(character,smithWorld)&&
        std::isfinite(smithWorld.scale)&&smithWorld.scale>.1f;
    if(smithWorldKnown)(void)g_committed_frame6695.read(previousSmith);
    for(int side=0;side<2;++side)if(valid[side]){
        if(alchemyPassengerScene42)alchemyHeldPassenger42[side]=
            kcd2vr::subsystems::alchemy::bridge67::nativeRetortD56(passengerNow46)||
            NativeAlchemyFingers18(reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,side,passengerNow46)||
            kcd2vr::alchemy_semi46::nativeHandOwned(passengerNow46,side);
        // Proximity guidance is separate from exact native tool/finger identity.
        // Both positions use THIS character world transform and scale; raw XR
        // movement separately prevents authored motion from re-grabbing a free hand.
        kcd2vr::smith_tool36::Binding smithBinding{};
        if(smithWorldKnown&&!nativeFingerCapture&&!ts.head_held&&!(side?ts.left_held:ts.right_held)&&
           NativeActivityHandOwned46(false,reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,side,passengerNow46,&smithBinding)){
            const auto nativeWorld=ModelToWorld6692(smithWorld,FromT(s.original[side?rig.hand_l:rig.hand_r]));
            const auto trackedWorld=ModelToWorld6692(smithWorld,rawGoalsH50[side]);
            const auto& rawTracking=side?ts.left:ts.right;
            const double tracking[3]{rawTracking.x,rawTracking.y,rawTracking.z};
            tx.smithFixture[side]=kcd2vr::subsystems::smithing::bridge67::fixtureStep(
                previousSmith.smithFixture[side],smithBinding,tx.epoch,ts.source_epoch,
                kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire),
                ts.sequence,ts.tick,Len(Sub(trackedWorld,nativeWorld)),tracking);
        }
        nativeSmithFixtureH21[side]=tx.smithFixture[side].valid&&tx.smithFixture[side].weight>0.f;
        // Preserve the separately armed legacy guided-native experiment.
        // Saved modes1-3 use the controller/proximity owners above.
        const bool nativeSmithPassenger=trackedSmith46&&smithMode==kcd2vr::minigame_hands45::Native&&
            NativeActivityHandOwned46(false,reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,side,passengerNow46,nullptr,false);
        // Detached forge workpiece remains native; attached tongs and wrist stay tracked.
        nativeHeatingHandD56[side]=false;
        nativePassengerHand46[side]=alchemyHeldPassenger42[side]||nativeSmithFixtureH21[side]||nativeSmithPassenger||nativeHeatingHandD56[side];
    }
    // Raw reachable controller target -> native motor. Only the measured
    // achieved wrist feeds this existing single arm solve; never feed an
    // already collision-corrected wrist back as the next motor target.
    bool presenceOwnedH24[2]{};ph::Pose presenceWristH24[2]{};
    ModelWorld6692 handWorldH24{};handPolicyH24::Input identityH24{};
    const auto handNowH24=std::uint64_t(GetTickCount64());
    const bool presenceBaseH24=g_native_hand_physics_h24.load(std::memory_order_acquire)&&
        !nativeFingerCapture&&!readingVisual32&&!tx.reloadWorkingHandXR34&&
        ReadModelWorld6692(character,handWorldH24)&&HandPresenceIdentityH24(character,tx.epoch,identityH24);
    for(int side=0;side<2;++side){
        auto in=identityH24;in.tick=handNowH24;
        in.source.trackingEpoch=ts.source_epoch;in.source.sourceGeneration=tx.epoch;
        in.source.cancelGeneration=tx.policyGeneration57;in.source.poseTick=ts.tick;
        in.source.side=side;in.source.physicalPresenceH24=true;
        const bool occupiedH24=HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,g_weapon_request_epoch6694.load())||heldFingerKey[side]||propHeld5916c1[side]||doorPose66[side]||doorPoseH40[side]||nativePassengerHand46[side]||
            (tx.physicalActive&&tx.physicalOutput.owned[side]);
        const bool usableH24=presenceBaseH24&&valid[side]&&tx.propRequestedValid5916c1[side]&&!occupiedH24&&
            !ts.head_held&&!(side?ts.left_held:ts.right_held);
        if(usableH24){
            const ph::Pose world{PQ(handWorldH24.q),PV(handWorldH24.t)};
            auto raw=tx.propRequested5916c1[side];raw.p=raw.p*handWorldH24.scale;
            const auto desired=ph::compose(world,raw);in.target={desired.q,desired.p};
            tx.handPresenceValidH24[side]=HandPresenceContextH24(in);
            if(tx.handPresenceValidH24[side]){
                tx.handPresenceH24[side]=in;handPresenceH24::PresenceFeedbackH24 measured{};handPolicyH24::Pose local{};
                const auto shoulder=PV(FromT(s.work[side?rig.upper_l:rig.upper_r]));
                if(handPresenceH24::ReadPresenceFeedbackH24(side,measured)&&
                   kcd2vr::physics::hand_presence_h24::allowed(in,measured,handNowH24,{world.q,world.p},handWorldH24.scale,
                       shoulder,ph::reach_max(s.upper[side],s.fore[side]),local)){
                    presenceOwnedH24[side]=true;presenceWristH24[side]={local.q,local.p};goals[side]=VP(local.p);
                }else if(measured.valid&&measured.source==in.source){
                    // A measured collider outside the current anatomical/model
                    // contract must retire, not drive an invisible second hand.
                    tx.handPresenceValidH24[side]=false;
                }
            }
        }
    }
    // A newly current owner may arrive after visual tracking was staged. Restore
    // its RAW requested input before free-hand contact, never after a contact
    // correction. Existing achieved weapon/prop/door/presence goals stay intact.
    bool wristOwnedH50[2]{},otherOwnerH50[2]{};
    if(visualPolicyH50)for(int side=0;side<2;++side)if(valid[side]){
        wristOwnedH50[side]=g_wristOwnedByController59c12&&g_wristOwnedByController59c12(side);
        otherOwnerH50[side]=heldFingerKey[side]||readingVisual32||tx.reloadWorkingHandXR34||nativeFingerCapture||
            (tx.acquisitionRequired57&&tx.acquisition54.side==side)||(s.pendingUse55.serial&&s.pendingUse55.side==side)||
            nativePassengerHand46[side]||doorPoseH40[side]||wristOwnedH50[side]||
            ts.tick<g_native_use_until107[side].load(std::memory_order_acquire)||
            HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,g_weapon_request_epoch6694.load());
        const bool achievedH50=(tx.physicalActive&&tx.physicalOutput.owned[side])||
            propHeld5916c1[side]||doorPose66[side]||doorPoseH40[side]||presenceOwnedH24[side];
        if(otherOwnerH50[side]){
            if(!achievedH50)goals[side]=rawGoalsH50[side];
            visQH50[side]=side?ts.lq:ts.rq;stagedH50[side].ready=false;
        }
    }
    int contactItemState164R1[2]{-1,-1};unsigned contactYield164R1[2]{};bool freeContact164R1[2]{};
    const auto contactNow164R1=std::uint64_t(GetTickCount64());
    // D52: pose-only activity/dialogue hands must not depend on the gameplay
    // equipment pump. This observation grants contact only, never input/use.
    kcd2vr::contact_occupancy_d52::Observation contactOccupancyD52{};
    if(collisionMode&&(valid[0]||valid[1])){
        contactOccupancyD52=ReadContactOccupancyD52(s.character,tx.epoch,ts.source_epoch,tx.policyGeneration57,contactNow164R1);
        TraceContactOccupancyD52(contactOccupancyD52,contactNow164R1);
    }
    kcd2vr::contact_occupancy_d52::QueryScopeD53 contactQueryOwnerD53(contactOccupancyD52);
    for(int side=0;side<2;++side)if(collisionMode&&valid[side]){
        const int item=contactItemState164R1[side]=contactOccupancyD52.hand[side].state;
        const bool physical=(tx.physicalActive&&tx.physicalOutput.owned[side])||presenceOwnedH24[side];
        const bool nativeUse=contactNow164R1<g_native_use_until107[side].load(std::memory_order_acquire);
        const bool bowString=side==0&&tx.physicalActive&&tx.physicalOutput.bowHand169r2;
        contactYield164R1[side]=((item>0||heldFingerKey[side])?1u:0u)|(physical?2u:0u)|(nativeUse?4u:0u)|
            (nativeFingerCapture?8u:0u)|(item<0?16u:0u)|(bowString?32u:0u)|(propHeld5916c1[side]?64u:0u)|(nativePassengerHand46[side]?128u:0u)|(HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,g_weapon_request_epoch6694.load())?256u:0u);
        if(tx.reloadWorkingHandXR34&&!reloadHandPolicyR95.worldContact[side])contactYield164R1[side]|=512u;
        freeContact164R1[side]=contactYield164R1[side]==0&&!doorPose66[side]&&!doorPoseH40[side];
    }
    tx.smithContactR38=ReadSmithContactHistoryR38(); // last published tool pose, staged for this transaction
#include "../subsystems/physics/pose_free_hand_contact.inl" // M2_OWNED_SOURCE
    // D39 hand-pair correction is staged before world sweeps in pose_hand_pair_d39.inl.
    V3 stagedBend[2]{s.lastBend[0],s.lastBend[1]};
    V3 ikAllowed113[2]{goals[0],goals[1]}; // diagnostics: actual fixed-length IK targets
    bool freeNowH50[2]{false,false}; // H50R2c: free this frame (no weapon/prop/presence, door/reading/work wrist, reload, passenger, lease)
    for(int side=0;side<2;++side)if(valid[side]) {
        const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r,h=side?rig.hand_l:rig.hand_r;
        const Q4 chest=QFromQuatT(s.work[rig.spine3]);
        V3 sideDown=QRotateV(chest,s.sideDownLocal[side]);
        V3 shoulder=FromT(s.work[u]);
        // Qualified native interaction ownership may blend the wrist toward the
        // authored hand (150 ms in / 200 ms out). Mere target proximity never
        // changes the free fingers; physical weapon ownership excludes this blend.
        {
            // V66.178R16 (review F02): the blend advances only on a NEW tracking tick (a re-evaluated
            // sample adds zero time), and a hand the physical weapon owns has weight 0 at once - one
            // exclusive owner per hand, no residual pull on a physical grip.
            // V66.178R18 (review F07): one clock per side - an invalid other hand (skipped by the loop) can no
            // longer leave this side on the 16 ms fallback step every evaluation.
            std::uint64_t& nt=stagedNaturalTickD47[side];
            const float dtN=(nt&&ts.tick>nt)?std::min(.1f,float(ts.tick-nt)*.001f):(nt?0.f:.016f);
            nt=ts.tick;
            const bool physicalOwned=(tx.physicalActive&&tx.physicalOutput.owned[side])||propHeld5916c1[side]||presenceOwnedH24[side];
            // F5 rung 2: native action/fingers remain alive, but the wrist stays
            // controller-owned. No residual blend may pull it down to an authored reach.
            // R59.16C10R12B (Miki): an owned pickup/alchemy/dog action keeps the wrist on the controller - the game's grab pose holds the
            // fingers and the item rides the hand bone; the R14 blend toward Henry's authored reach never applies to it.
            const bool alchemyPassengerContext42=alchemyPassengerScene42;
            const bool alchemyToolHeld42=alchemyHeldPassenger42[side];
            const bool wristOwned43=visualPolicyH50?wristOwnedH50[side]:(g_wristOwnedByController59c12&&g_wristOwnedByController59c12(side));
            const bool trackedInteraction=tx.instantPairHandsD72||tx.doorSerial66[side]||doorPoseH40[side]||(!alchemyPassengerContext42&&!nativePassengerHand46[side]&&
                (readingVisual32||workstation178r20c1::ownsActivityWrist45(GetTickCount64())||wristOwned43));
            // C43/D44: a sent native PickableItem Use opens a bounded presentation window in which this wrist stays on the
            // controller until the native Attach adopts it (R12B). Since R18 arms at Attach, the R14 blend used to pull the VR
            // hand along Henry's authored 3d_pick reach for the whole Use->Attach approach. Doors/chests never open a window.
            kcd2vr::grab43::Use grabUse43{};const auto grabNow43=std::uint64_t(GetTickCount64());
            const bool grabFresh43=!alchemyPassengerContext42&&!nativePassengerHand46[side]&&kcd2vr::grab43::read(side,grabUse43)&&kcd2vr::grab43::fresh(grabUse43,grabNow43)&&
                kcd2vr::grab43::current(grabUse43,{reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,ts.source_epoch});
            const bool grabOwner43=action_time_owner59c11::NativeGraspAvailable63(); // capability, not a still-pending Attach owner
            const bool grabSetting43=kcd2vr::grab43::handStays.load(std::memory_order_relaxed);
            const bool grabVeto43=kcd2vr::grab43::veto(grabSetting43,grabOwner43,grabFresh43,trackedInteraction);
            const bool leaseBase43=!tx.reloadWorkingHandXR34&&(g_native_handover178r14.load(std::memory_order_relaxed)||alchemyPassengerContext42||nativePassengerHand46[side])&&!physicalOwned&&!trackedInteraction&&
                (alchemyToolHeld42||nativePassengerHand46[side]||
                 (g_native_use_serial107[side].load(std::memory_order_acquire)>g_native_wrist_retired107[side].load(std::memory_order_acquire)&&
                  ts.tick<g_native_wrist_until107[side].load(std::memory_order_acquire)))&&ValidPose6689(s.original[h]);
            const bool lease=leaseBase43&&!grabVeto43;
            if(grabFresh43&&leaseBase43){ // edge-only F5 rung 1 receipt: what the native reach would have pulled this hand
                const float pull43=Len(Sub(FromT(s.original[h]),goals[side]));
                if(s.grab43[side].reach(grabUse43,grabVeto43,pull43)&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,grabNow43)){
                    char m[300]{};std::snprintf(m,sizeof(m),"C43 GRAB_ALIGN edge=%s side=%d target=%u serial=%u setting=%s graspAvailable=%s nativeHandPullM=%.3f mover=R14_HANDOVER_BLEND(our own) nativePicking=UNCHANGED",
                        grabVeto43?"REACH_VETOED":"REACH_KEPT",side,grabUse43.entity,grabUse43.serial,grabSetting43?"ON":"OFF",grabOwner43?"ON":"OFF",pull43);
                    diagnostic_trace::Write(m);}
            }
            float& hw=stagedHandoverD47[side];const float ht=lease?1.f:0.f;
            if(tx.reloadWorkingHandXR34||physicalOwned||trackedInteraction||grabVeto43)hw=0.f;
            else if(tx.smithFixture[side].valid)hw=tx.smithFixture[side].weight;
            else if(nativePassengerHand46[side]&&lease)hw=1.f; // other native passenger owners unchanged
            else hw+=std::clamp(ht-hw,-dtN/.2f,dtN/.15f);
            freeNowH50[side]=visualPolicyH50&&!otherOwnerH50[side]&&!physicalOwned&&!trackedInteraction&&!tx.reloadWorkingHandXR34&&!nativePassengerHand46[side]&&!lease&&!grabVeto43;
            if(hw>.001f)goals[side]=Add(goals[side],Mul(Sub(FromT(s.original[h]),goals[side]),hw));
        }
#include "../subsystems/vrik/pose_final_wrist_d52.inl" // M2_OWNED_SOURCE
        // Reload progress still reads the immutable controller intent. Only the
        // working hand's presentation follows the qualified string/mechanism.
        if(tx.reloadWorkingHandXR34&&side==tx.reloadPassengerXR34.workingHand){
            ph::Pose constrainedD78{};
            if(ConstrainReloadWristD78(tx.reloadPassengerXR34,tx.propRequested5916c1,
                tx.propRequestedValid5916c1,ts,constrainedD78)){
                goals[side]=VP(constrainedD78.p);handDesired=QP(constrainedD78.q);
            }
        }
#include "station_hand_admission_d75.inl"
        // R38: the final endpoint (including actual world/weapon/door contact)
        // is authoritative. Accommodate its shoulder before choosing an elbow;
        // elbow preference and flexion policy cannot move this endpoint.
        const bool ordinaryElbowD67=!bowOwnsReachH17&&!nativeFingerCapture&&
            !nativePassengerHand46[side]&&!readingVisual32&&
            (!tx.reloadWorkingHandXR34||reloadHandPolicyR95.tracked[side])&&stagedHandoverD47[side]<=.001f;
        const auto& crossbowProfileR93=tx.physicalProfiles[1];
        const auto& crossbowStateR93=tx.physicalState.weapon[1];
        const bool crossbowSupportR93=side==0&&ordinaryElbowD67&&tx.physicalActive&&
            crossbowProfileR93.crossbowD72()&&crossbowProfileR93.primary&&crossbowProfileR93.key&&
            crossbowStateR93.key==crossbowProfileR93.key&&crossbowStateR93.generation==crossbowProfileR93.generation&&
            tx.physicalOutput.active[1]&&tx.physicalOutput.owned[1]&&tx.physicalOutput.owned[0]&&
            tx.physicalOutput.support[1]&&crossbowStateR93.support;
        const bool elbowExperimentD45=(elbowTrialD67||elbowTrialD45)&&ordinaryElbowD67;
        const bool torsoContinuousD67=(continuityD67||elbowExperimentD45)&&ordinaryElbowD67;
        // D86: the committed bend belongs to the current body geometry. A
        // bow/native-hand ownership change must not cold-start the same arm;
        // only an actual policy generation (or the existing epoch/age guards)
        // retires it. Otherwise every bow admission can bypass continuity once.
        const auto sidePolicyD45=elbowPolicyD45;
        if(elbowTrialD67&&ordinaryElbowD67)sideDown=VP(kcd2vr::elbow_motion_d67::reachPole(PV(sideDown),PV(QRotateV(chest,s.chestRightLocal)),PV(QRotateV(chest,s.chestUpLocal)),PV(shoulder),PV(goals[side]),side,kcd2vr::motion_trials_d67::ElbowOutward(),kcd2vr::motion_trials_d67::ElbowBack()));
        if(stagedElbowPolicyD45[side]!=sidePolicyD45){
            // R90 (R89 option): a real policy generation change (F2/F7 comparison rung) keeps the hand-guided
            // history - wiping it snapped the elbow 10-29 cm in one frame. only the trial destination state restarts.
            if(elbowR89){auto& e=stagedElbow[side];e.destinationReadyD67C=false;e.alternativeSinceD67C=0;e.alternativeSideD69=0;}
            else stagedElbow[side]={};
            stagedShoulderFitD45[side]={};stagedElbowPolicyD45[side]=sidePolicyD45;
        }
        // Latch one legacy policy for shoulder fit, search and the arm solve.
        const bool torsoSearchLabD45=kcd2vr::frik_elbow_lab::Active();
        // Only an unchanged free-world-contact endpoint enters this extra
        // anatomical admission. Held props/weapons, native leases, smithing,
        // doors and legitimate raw-controller reach retain their own owners.
        const V3* collisionRawD74=freeContact164R1[side]&&s.allowedReady[side]&&
            Len(Sub(goals[side],rawGoalsH50[side]))>.0001f&&
            Len(Sub(goals[side],s.allowedGoal[side]))<.00001f?&rawGoalsH50[side]:nullptr;
        if(!accommodateShoulderR38(side,goals[side],torsoSearchLabD45&&!bowOwnsReachH17,false,elbowExperimentD45,collisionRawD74))
            return RejectCoherent6693(tx,__LINE__);
        shoulder=FromT(s.work[u]);
        // B3: select the hinge plane for the FINAL wrist endpoint. Native
        // passenger handover above may replace the tracked goal; a pole selected
        // for the old goal can send the elbow through the torso during that handover.
        const double torsoRadius=pc::torso_radius103(PV(gestureShouldersR38[0]),PV(gestureShouldersR38[1]));
        const auto torsoA_D45=PV(FromT(s.work[rig.pelvis])),torsoB_D45=PV(FromT(s.work[rig.spine3]));
        std::optional<pc::Vec3> torsoSeedD45{};
        if(torsoContinuousD67&&pc::elbow_history_current(stagedElbow[side],tx.epoch,ts.sequence,ts.tick)){
            if(const auto axisD45=pc::unit(PV(Sub(goals[side],shoulder))))
                torsoSeedD45=kcd2vr::elbow_shoulder_d45::transport(stagedElbow[side],*axisD45);
        }
        // D86: one body/hand destination reaches ordinary tracked arms AND
        // the bow. Native passengers retain their existing authored policy.
        kcd2vr::elbow_destination_d86::Input anatomicalD86{};
        kcd2vr::elbow_destination_d86::Result destinationD86{};
        if(ordinaryElbowD67||bowOwnsReachH17){
            anatomicalD86.crossbowSupportR93=crossbowSupportR93;
            anatomicalD86.shoulder=PV(shoulder);anatomicalD86.hand=PV(goals[side]);
            anatomicalD86.upper=s.upper[side];anatomicalD86.fore=s.fore[side];
            anatomicalD86.relaxed=PV(sideDown);
            anatomicalD86.up=PV(QRotateV(chest,s.chestUpLocal));
            anatomicalD86.outward=PV(QRotateV(chest,s.chestRightLocal))*(side?-1.:1.);
            anatomicalD86.backward=pc::cross(anatomicalD86.up,anatomicalD86.outward)*(side?1.:-1.);
            anatomicalD86.torsoA=torsoA_D45;anatomicalD86.torsoB=torsoB_D45;anatomicalD86.torsoRadius=torsoRadius;
            anatomicalD86.head=PV(FromT(s.work[rig.head]));anatomicalD86.haveHead=pc::finite(anatomicalD86.head);
            const auto forearmCueD86=kcd2vr::elbow_destination_d86::forearm_direction(
                PV(FromT(s.rest[f])),PV(FromT(s.rest[h])),PQ(QFromQuatT(s.rest[h])),PQ(handDesired));
            anatomicalD86.haveForearm=bool(forearmCueD86);
            if(forearmCueD86)anatomicalD86.forearmDirection=*forearmCueD86;
            if(pc::elbow_history_current(stagedElbow[side],tx.epoch,ts.sequence,ts.tick)){
                if(const auto axisD86=pc::unit(anatomicalD86.hand-anatomicalD86.shoulder))
                    anatomicalD86.prior=pc::transported_elbow_bend(stagedElbow[side],*axisD86);
            }
            if(side==0&&manualPose169r2&&tx.physicalActive&&tx.physicalOutput.bowHand169r2&&!tx.physicalState.weapon[1].carry146){
                anatomicalD86.bowAway=PV(Sub(goals[0],goals[1]));anatomicalD86.drawWeight=archerWeight169r2;
            }
            destinationD86=kcd2vr::elbow_destination_d86::choose(anatomicalD86);
        }
        auto torsoPreference=destinationD86.valid?destinationD86.pole:
            kcd2vr::elbow_motion_d69::preference(elbowHysteresisD69&&ordinaryElbowD67,
                PV(shoulder),PV(goals[side]),s.upper[side],s.fore[side],PV(sideDown),torsoA_D45,torsoB_D45,torsoRadius,
                stagedElbow[side],tx.epoch,ts.sequence,ts.tick,ts.timeNs,torsoContinuousD67,torsoSearchLabD45);
        if(tx.traceH29){auto& d=tx.traceArmH29[side];d.torsoInput=true;
            d.torsoA=torsoA_D45;d.torsoB=torsoB_D45;d.torsoRadius=torsoRadius;
            d.torsoSeedPresent=destinationD86.valid?bool(anatomicalD86.prior):bool(torsoSeedD45);
            d.torsoSeed=destinationD86.valid?anatomicalD86.prior.value_or(pc::Vec3{}):torsoSeedD45.value_or(pc::Vec3{});
            d.torsoSearchLab=torsoSearchLabD45;d.continuousTorsoD45=torsoContinuousD67;d.torsoResult=torsoPreference;
            d.anatomicalD86=destinationD86.valid;d.wristCueValidD86=anatomicalD86.haveForearm;
            d.wristCueD86=anatomicalD86.forearmDirection;d.headD86=anatomicalD86.head;
            d.outwardD86=anatomicalD86.outward;d.upD86=anatomicalD86.up;d.bowAwayD86=anatomicalD86.bowAway;
            d.drawWeightD86=anatomicalD86.drawWeight;d.intrusionD86=destinationD86.intrusion;d.costD86=destinationD86.objective;
            d.evaluationsD86=destinationD86.evaluations;
            // Same finite F7 record, copied from this transaction. No engine
            // read or inference from the settings eventually saved at exit.
            auto& basis=d.basisD86;basis.available=ordinaryElbowD67||bowOwnsReachH17;
            if(basis.available){basis.upper=u;basis.forearm=f;basis.hand=h;basis.elbowPolicy=sidePolicyD45;
                basis.handConversionKnown=s.anatomyHand[side];basis.restUpper=PV(FromT(s.rest[u]));
                basis.restForearm=PV(FromT(s.rest[f]));basis.restHand=PV(FromT(s.rest[h]));
                basis.restHandQ=PQ(QFromQuatT(s.rest[h]));basis.desiredHandQ=PQ(handDesired);
                basis.boneToAnatomy=PQ(s.boneToAnatomy[side]);basis.chestQ=PQ(chest);
                basis.chestRightLocal=PV(s.chestRightLocal);basis.chestUpLocal=PV(s.chestUpLocal);
                basis.hingeNormalLocal=PV(s.hingeNormalLocal[side]);}}
        if(!destinationD86.valid&&side==0&&manualPose169r2&&tx.physicalActive&&tx.physicalOutput.bowHand169r2&&!tx.physicalState.weapon[1].carry146){
            const auto archer=kcd2vr::archer169r2::elbow_preference(PV(shoulder),PV(goals[0]),PV(goals[1]),s.upper[0],s.fore[0],
                torsoPreference,PV(FromT(s.work[rig.pelvis])),PV(FromT(s.work[rig.spine3])),torsoRadius,PV(FromT(s.work[rig.head])));
            const auto blended=pc::unit(torsoPreference*(1-archerWeight169r2)+archer*archerWeight169r2);
            if(blended)torsoPreference=*blended;
        }
#include "../subsystems/vrik/pose_arm_solver.inl" // M2_OWNED_SOURCE
        // The native book/parchment grip already exists in this transaction's
        // untouched parent-local finger pose. Keep it; only the wrist/arms are
        // controller-owned. Do not turn menu Grip/Trigger into an empty fist.
        const bool claimedFingerT24=HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,g_weapon_request_epoch6694.load());
        bool objectFingers=(tx.reloadWorkingHandXR34&&(reloadHandPolicyR95.nativeFingers[side]||contactItemState164R1[side]!=0))||claimedFingerT24||(!nativeFingerCapture&&(readingVisual32||nativePassengerHand46[side]));
        bool combatFistH34=false;
        const bool stringFingers169r2=side==0&&tx.physicalActive&&tx.physicalOutput.bowHand169r2;
        if(!claimedFingerT24){
        if(stringFingers169r2){
            const bool carry=tx.physicalState.weapon[1].carry146;
            const std::uint64_t phaseKey=tx.physicalProfiles[1].key^(carry?0xA220000000000000ull:0xB057000000000000ull);
            objectFingers=ApplyObjectFingerLocals6698(s,0,phaseKey,tx.physicalProfiles[1].generation,ts.sequence,ts.tick);
            // While the native finger reference is becoming ready, retain the
            // unmodified native locals; do not turn Trigger into an empty-hand fist.
            if(!objectFingers)objectFingers=true;
        }else if(tx.physicalActive&&tx.physicalOutput.owned[side]){
            const int itemSide=tx.physicalOutput.active[side]?side:1-side;
            const auto& profile=tx.physicalProfiles[itemSide];
            objectFingers=ApplyObjectFingerLocals6698(s,side,profile.key,profile.generation,ts.sequence,ts.tick);
        }
        if(!tx.reloadWorkingHandXR34){
        if(nativeFingerCapture)s.graspFingers54[side]={};
        else if(ApplyPublishedGraspFingers55(s,side,tx.epoch,std::uint64_t(GetTickCount64())))objectFingers=true;
        if(!nativeFingerCapture&&tx.doorSerial66[side]&&g_doorFingers66&&
           g_doorFingers66(side,tx.epoch,GetTickCount64()))objectFingers=true;
        if(!nativeFingerCapture&&!objectFingers&&NativeAlchemyFingers18(reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,side,
            std::uint64_t(GetTickCount64())))objectFingers=true; // exact currently native-held alchemy entity; keep its own finger grip
        if(!nativeFingerCapture&&!objectFingers&&NativeSmithFingers36(reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,side,
            std::uint64_t(GetTickCount64())))objectFingers=true; // C37: auto-held native smithing tool keeps its native grip without Grip
        if(!nativeFingerCapture&&!objectFingers){
            const std::uint64_t heldKey=heldFingerKey[side];
            if(heldKey)objectFingers=ApplyObjectFingerLocals6698(s,side,heldKey,tx.epoch,ts.sequence,ts.tick);
            else if(std::uint64_t(GetTickCount64())<g_native_use_until107[side].load(std::memory_order_acquire)){
                // V66.107: F5 native interaction (door, chest, pick up, eat): KCD2's own
                // evaluated finger pose drives this hand exactly like a held weapon.
                objectFingers=ApplyObjectFingerLocals6698(s,side,0xF5F5000000000000ull|std::uint64_t(side),
                    g_native_use_serial107[side].load(std::memory_order_acquire),ts.sequence,ts.tick,true);
            }
            else {s.objectFingerKey[side]=0;s.objectCandidate[side]={};
                // Exact occupied hand, but no usable keyed grip yet: retain the
                // native locals already read at transaction start. Do not apply
                // the free-hand curl or cache an invented item identity.
                objectFingers=contactItemState164R1[side]>0;}
        }
        } // reload objects/constraints retain native fingers; only a verified free working hand curls below.
        if(preciseContactModeR24&&!nativeFingerCapture&&!objectFingers&&freeContact164R1[side]){
            const kcd2vr::finger_contact_h31::Source source{reinterpret_cast<std::uintptr_t>(character),tx.epoch,tx.epoch,
                contactControl,ts.sequence,ts.tick,tx.fingerSpringH31&&kcd2vr::deepseek_h29::fingerContactSpringH31.load(std::memory_order_acquire)};
            ApplyFingerContactSpringH31(s,side,stagedContact[side],source,side?ts.left_grip:ts.right_grip,side?ts.left_trigger:ts.right_trigger,tx.fingerTrialH7[side]?tx.fingerOpenBiasH7[side]:s.hoverW178r14[side],GetTickCount64(),tx.fingerTrialH7[side]);
        }else stagedContact[side].springH31={};
        if(tx.motionSessionD68){
            namespace md=kcd2vr::diagnostics::motion_d68;const auto& c=stagedContact[side];
            bool fingerTouchD68=false;for(bool touch:c.fingerContact)fingerTouchD68|=touch;
            tx.motionHandFlagsD68[side]=md::StateKnown|(fingerTouchD68?md::FingerContact:0u)|
                ((tx.physicalActive&&tx.physicalOutput.owned[side])?md::PhysicalOwner:0u)|
                (propHeld5916c1[side]?md::PropOwner:0u)|(presenceOwnedH24[side]?md::PresenceOwner:0u)|
                ((doorPose66[side]||doorPoseH40[side])?md::DoorOwner:0u)|
                (nativePassengerHand46[side]?md::PassengerOwner:0u)|(readingVisual32?md::ReadingOwner:0u)|
                (nativeFingerCapture?md::CaptureOwner:0u)|(heldFingerKey[side]?md::HeldKey:0u)|
                (contactItemState164R1[side]>0?md::ItemOccupied:0u)|(contactItemState164R1[side]<0?md::ItemUnknown:0u)|
                (freeContact164R1[side]?md::FreeContactEligible:0u)|(c.fresh169r1?md::ContactFresh:0u)|
                (c.blocked?md::ContactBlocked:0u)|(c.planeCount>0?md::ContactPlanes:0u)|
                (selfMask164[side]?md::SelfContact:0u)|(tx.handPairD39.touching?md::PairContact:0u)|
                (stagedHandoverD47[side]>.001f?md::Handover:0u)|
                ((otherOwnerH50[side]||claimedFingerT24||(tx.acquisitionRequired57&&tx.acquisition54.side==side)||
                  (s.pendingUse55.serial&&s.pendingUse55.side==side))?md::OtherOwner:0u)|(c.releaseActiveD45?md::ContactRelease:0u)|
                ((ts.head_held||(side?ts.left_held:ts.right_held))?md::TrackingHeld:0u)|
                ((tx.physicalActive&&tx.physicalOutput.blocked[side])?md::PhysicalBlocked:0u)|
                ((tx.physicalActive&&tx.physicalOutput.owned[side]&&!tx.physicalOutput.contactKnown[side])?md::PhysicalContactUnknown:0u);
        }
        const float freeFingerTriggerD78=!nativeFingerCapture&&!objectFingers?FreeFingerTriggerD78(ts,side):(side?ts.left_trigger:ts.right_trigger);
        if(tx.traceH29||tx.fingerDiagnostic28){auto& d=tx.traceArmH29[side];const auto& contact=stagedContact[side];
        kcd2vr::diagnostics::contact_d77::copy(d.contactD77,tx.traceH29,contact,collisionMode,freeContact164R1[side],preciseContactModeR24,perfContactModeDs2,g_contact_pass6695,g_contact_budget_sample65,g_contact_budget6695);
            if(tx.fingerDiagnostic28)tx.fingerContext28[side]=true;
            d.ownerMask=(tx.physicalActive&&tx.physicalOutput.owned[side]?1u:0u)|(presenceOwnedH24[side]?2u:0u)|
                (propHeld5916c1[side]?4u:0u)|((doorPose66[side]||doorPoseH40[side])?8u:0u)|(nativePassengerHand46[side]?16u:0u)|
                (readingVisual32?32u:0u)|(stringFingers169r2?64u:0u)|(nativeFingerCapture?128u:0u);
            d.heldKey=heldFingerKey[side];d.objectKey=s.objectFingerKey[side];d.objectGeneration=s.objectFingerGeneration[side];
            d.doorSerial=doorPoseH40[side]?tx.doorDisplayH40[side]:tx.doorSerial66[side];d.itemState=contactItemState164R1[side];d.contactYield=contactYield164R1[side];
            d.d71Trial=contact_handover_d71::ContactContinuity();d.d71LocalSeed=contact.bodyLocalSeedD71;
            d.d71PartRequeryMask=contact.bodyPartRequeryMaskD71;d.d71BodyRelease=contact.releaseBodySurfaceD71;
            d.contactFresh=contact.fresh169r1;d.contactTick=contact.lastCompleteTickH18;d.planes=contact.planeCount;
            for(int f=0;f<5;++f)if(contact.fingerContact[f])d.fingerContact|=1u<<f;
            d.grip=side?ts.left_grip:ts.right_grip;d.trigger=freeFingerTriggerD78;
            d.curveTrialRequestedH7=tx.fingerTrialRequestedH7;d.curveTrialAvailableH7=tx.fingerTrialH7[side];
            using F=kcd2vr::diagnostics::vrik_h29::FingerOwner;
            d.finger=nativeFingerCapture?F::NativeCapture:objectFingers?F::NativeOrObject:
                s.fingerMap[side]&&s.open[side].ready&&s.closed[side].ready?F::FreeRecipe:F::NativeMissingReference;
        }
        if(!nativeFingerCapture&&!objectFingers){
            combatFistH34=QualifiedCombatFistH34(ts,tx.epoch,side,GetTickCount64());
            if(tx.traceH29||tx.fingerDiagnostic28)tx.traceArmH29[side].fingerCall=true;
            if(!ApplyFingerLocals6689(s,side,side?ts.left_grip:ts.right_grip,freeFingerTriggerD78,freeContact164R1[side]&&!stationContactD75?&stagedContact[side]:nullptr,tx.fingerTrialH7[side]?tx.fingerOpenBiasH7[side]:s.hoverW178r14[side],tx.fingerTrialH7[side]))return RejectCoherent6693(tx,__LINE__);
            if(tx.traceH29||tx.fingerDiagnostic28)tx.traceArmH29[side].fingerOk=true;
        }
        }else stagedContact[side].springH31={}; // T24: keep native locals; arm/map/invariants below still execute.
        // Snapshot only the owners already selected for this hand. No new
        // combat/interaction gate: only the optional paper yields to this mask.
        const auto& supportOwnerM34=tx.physicalState.weapon[1-side];
        const auto& supportProfileM34=tx.physicalProfiles[1-side];
        const bool retainedSupportM34=tx.physicalActive&&supportProfileM34.key&&
            supportOwnerM34.key==supportProfileM34.key&&supportOwnerM34.generation==supportProfileM34.generation&&
            supportOwnerM34.support&&supportOwnerM34.supportGripReady169r2;
        tx.mapPresentationOwnerM34[side]=kcd2vr::map_hand_owner_m34::mask(
            tx.physicalActive&&tx.physicalOutput.owned[side],propHeld5916c1[side],
            doorPose66[side]||tx.doorSerial66[side],nativePassengerHand46[side],
            readingVisual32,stringFingers169r2,nativeFingerCapture,objectFingers,combatFistH34,retainedSupportM34);
        // H40 presentation is not another gameplay permission. Preserve the
        // original semantic input mask; only the optional map yields to it.
        tx.doorInputOwnerMaskH40[side]=tx.mapPresentationOwnerM34[side];
        if(doorPoseH40[side])tx.mapPresentationOwnerM34[side]|=
            kcd2vr::map_hand_owner_m34::mask(false,false,true,false,false,false,false,false,false,false);
        if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);
        if(std::fabs(Len(Sub(FromT(s.work[f]),FromT(s.work[u])))-s.upper[side])>.002||
           std::fabs(Len(Sub(FromT(s.work[h]),FromT(s.work[f])))-s.fore[side])>.002)return RejectCoherent6693(tx,__LINE__);
    }
#include "../subsystems/vrik/pose_invariants.inl" // M2_OWNED_SOURCE
    if(tx.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||g_cal_state.load()!=2||
       tx.control51!=g_frame_control51.load(std::memory_order_acquire))return RejectCoherent6693(tx,__LINE__);
    // V169R1: sample the final composed joints, NOT the earlier self-projection
    // success flag. A later world/reach solve can invalidate a self contact.
    BuildPassiveHandEvidence169R1(tx,collisionMode,freeContact164R1,requestedGoals,stagedContact);
#include "../subsystems/ranged_reload/pose_archer_result.inl" // M2_OWNED_SOURCE
#include "../subsystems/vrik/pose_hand_display_d62.inl" // raw semantic solve has completed
    if(tx.kickFrame176.active&&(g_kick_cancel176.load(std::memory_order_acquire)!=tx.kickCancel176||
       g_kick_busy176.load(std::memory_order_acquire)!=tx.kickSerial176||!CombatExperiment175Enabled()))
        return RejectCoherent6693(tx,__LINE__);
    tx.stage="NATIVE_WRITE_TRANSACTION";
    if(readingVisual32&&!kcd2vr::reading_pose32::valid(readingLease32,GetTickCount64()))return RejectCoherent6693(tx,__LINE__);
    // Cancellation, a real menu/modal transition or raw Grip release during
    // the solve revokes Picking before any scoped native bone write.
    if(!CoherentOwnerCurrent57(tx)){
        const auto stationWhyR9C=kcd2vr::station_body_h41::Refuse(kcd2vr::station_body_h41::lastRefuse.load(std::memory_order_relaxed));
        tx.stage=tx.stationLeaseH41&&stationWhyR9C!=kcd2vr::station_body_h41::Refuse::None?
            kcd2vr::station_body_h41::stageName(stationWhyR9C):"ACQUISITION_REVOKED_BEFORE_WRITE";return RejectCoherent6693(tx,__LINE__);}
    tx.committed=pc::commit(s.original,s.work,tx.selected,count,[&](std::size_t j,const QuatT28& p){return WriteQuat(tx.setter,pose,int(j),p);},tx.attempted);
    if(!tx.committed){tx.Restore();g_committed_pose_valid6689.store(false,std::memory_order_release);return RejectCoherent6693(tx,__LINE__);}
    if(!CoherentOwnerCurrent57(tx)){tx.Restore();tx.stage="ACQUISITION_REVOKED_DURING_WRITE";return RejectCoherent6693(tx,__LINE__);}
    if(tx.kickFrame176.active&&(g_kick_cancel176.load(std::memory_order_acquire)!=tx.kickCancel176||
       g_kick_busy176.load(std::memory_order_acquire)!=tx.kickSerial176||!CombatExperiment175Enabled())){
        tx.Restore();return RejectCoherent6693(tx,__LINE__);
    }
    // Only a committed pose can publish a trajectory or consume query budget.
    // The previous endpoint is world-space from the previous committed sample,
    // not an old model point transformed using a newer actor transform.
    kick176::Contact kickContact176{};int kickMaterial37=-1;std::uintptr_t kickCollider37=0; // C37: the foot ray's own idmatOrg/collider
    const auto& kickFrame176=tx.kickFrame176;
    if(kickFrame176.active&&kickFrame176.newSample){
        ModelWorld6692 world176{};
        if(ReadModelWorld6692(character,world176)){
            const auto foot=PV(ModelToWorld6692(world176,FromT(s.work[s.leg[kickFrame176.side].ankle])));
            if(stagedKick176.havePreviousWorld)
            {
                float gap72=-1.f;
                kickContact176=ProbeKickContact176(character,stagedKick176.previousWorld,foot,&kickMaterial37,&kickCollider37,
                    kickFrame176.status==kcd2vr::kick176::Status::Extend?kcd2vr::kick26::AssistM72():0.f,&gap72);
                if(gap72>=0.f&&kickContact176.kind==kick176::ContactKind::Hit){char l72[160]{};
                    std::snprintf(l72,sizeof(l72),"C72 KICK_ASSIST_GAP visibleGapM=%.3f assistCm=%d",double(gap72),kcd2vr::kick26::assistCm72.load());
                    diagnostic_trace::Write(l72);}
            }
            stagedKick176.previousWorld=foot;stagedKick176.havePreviousWorld=true;
        }else stagedKick176.havePreviousWorld=false;
    }
    if(kickFrame176.active&&(g_kick_cancel176.load(std::memory_order_acquire)!=tx.kickCancel176||
       g_kick_busy176.load(std::memory_order_acquire)!=tx.kickSerial176||!CombatExperiment175Enabled())){
        tx.Restore();return RejectCoherent6693(tx,__LINE__);
    }
    const bool firstKickContact176=kickContact176.kind==kick176::ContactKind::Hit&&!stagedKick176.contactObserved;
    if(firstKickContact176)stagedKick176.contactObserved=true;
#ifdef KCD2VR_KICK34_H
    // C37: a body contact of the kicking foot is OFFERED to this click's kick transaction (copied data, no native call).
    // Only the snap/hold of the foot (EXTEND) on a living/ragdoll body counts; the ledger takes at most one per click
    // and binds it to the click's pose epoch. Two atomic loads when no transaction waits for this kick.
    if(kickContact176.kind==kick176::ContactKind::Hit&&kickContact176.entity&&kickFrame176.status==kick176::Status::Extend&&
       (kickContact176.body==unsigned(kcd2vr::npc_contact172::Body::Living)||kickContact176.body==unsigned(kcd2vr::npc_contact172::Body::Articulated))&&
       kcd2vr::kick34::ledger.wants(kickFrame176.serial)){
        kcd2vr::kick34::Contact k37{};k37.serial=kickFrame176.serial;k37.tick=ts.tick;k37.poseEpoch=tx.epoch;
        k37.entity=kickContact176.entity;k37.part=kickContact176.part;k37.body=kickContact176.body;k37.bodyMaterial=kickMaterial37;k37.collider=kickCollider37;
        k37.pos[0]=float(kickContact176.point.x);k37.pos[1]=float(kickContact176.point.y);k37.pos[2]=float(kickContact176.point.z);
        k37.extend=true;k37.living=true;
        (void)kcd2vr::kick34::ledger.offer(k37,std::uint64_t(GetTickCount64()));
    }
#endif
    s.kick176=stagedKick176;
    if(kickFrame176.serial&&g_kick_cancel176.load(std::memory_order_acquire)==tx.kickCancel176){
        const bool phaseChanged176=g_kick_status176.exchange(unsigned(kickFrame176.status),std::memory_order_acq_rel)!=unsigned(kickFrame176.status);
        if(kickFrame176.terminal){auto own=kickFrame176.serial;
            (void)g_kick_busy176.compare_exchange_strong(own,0,std::memory_order_acq_rel);}
        if((kickFrame176.started||kickFrame176.terminal||phaseChanged176||firstKickContact176)&&
           kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Combat,ts.tick)){
            char text176[512]{};std::snprintf(text176,sizeof(text176),
                "V66.176 KICK_VISUAL_CONTACT_PROBE serial=%llu sample=%llu phase=%s leg=%c contact=%s entity=%u part=%d body=%u owner=%d queryAttempts=%u damage=%s contactShape=ANKLE_3RAY",
                (unsigned long long)kickFrame176.serial,(unsigned long long)ts.sequence,kick176::name(kickFrame176.status),
                kickFrame176.side?'L':'R',kickContact176.kind==kick176::ContactKind::Hit?"KNOWN_PLANE":kickContact176.kind==kick176::ContactKind::Clear?"CLEAR":"UNKNOWN",
                kickContact176.entity,kickContact176.part,kickContact176.body,kickContact176.bodyOwner,kickContact176.queries,
                kcd2vr::kick26::Current()==kcd2vr::kick26::Style::VrLegOnly?"NONE(native kick admission unproven; contact observation only)":"NATIVE_ACTION_ONLY(no procedural contact damage)");
            diagnostic_trace::Write(text176);
        }
    }
    auto& historyD47=tx.motionD47;
    historyD47.ready=true;historyD47.archer=stagedArcher169r2;historyD47.contactMode=collisionMode;
    for(int side=0;side<2;++side){
        historyD47.valid[side]=valid[side];historyD47.smooth[side]=stagedH50[side];historyD47.free[side]=freeNowH50[side];
        if(!valid[side])continue;
        historyD47.bend[side]=stagedBend[side];historyD47.elbow[side]=stagedElbow[side];historyD47.pronation[side]=stagedPronation[side];
        historyD47.shoulderFit[side]=stagedShoulderFitD45[side];historyD47.elbowPolicy[side]=stagedElbowPolicyD45[side];
        historyD47.naturalTick[side]=stagedNaturalTickD47[side];historyD47.handover[side]=stagedHandoverD47[side];
        historyD47.clavicle[side]=stagedClavicle[side];historyD47.shoulder[side]=stagedShoulder[side];
    }
#ifdef KCD2VR_REACH_VIEW38_H
    // C38 F5 rung 1 only: copy where the eye, the solved hands and the NATIVE body box were in this committed frame.
    // O(joint count) min/max, no native call, no write; consumed edge-only by ReachView38Tick on the tracking thread.
    if(kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,ts.tick)){
        namespace rv=kcd2vr::reach_view38;rv::Evidence e{};auto RV=[](V3 v){return rv::V{v.x,v.y,v.z};};
        e.tick=ts.tick;e.sample=ts.sequence;e.epoch=tx.epoch;e.full=full;e.liveAnchor=liveHeadOk123;e.raiseCapped=raiseCapped38;
        e.workstation=kcd2vr::workstation178r20c1::ownsActivityWrist45(GetTickCount64())||kcd2vr::hands59c7f::activityVisual.load(std::memory_order_relaxed);
        e.dialogueBody=!ts.allowed?false:!g_gameplay_allowed.load(std::memory_order_relaxed);
        for(std::uint32_t j=0;j<count;++j)rv::grow(e.native,RV(FromT(s.original[j])));
        const Q4 nc38=QFromQuatT(s.neutral[rig.spine3]);
        const auto ax38=pc::chest_axes103(PV(QRotateV(nc38,s.chestRightLocal)),PV(QRotateV(nc38,s.chestUpLocal)));
        const V3 fwd38=ax38?QRotateV(delta,VP(ax38->forward)):V3{0,1,0};
        const V3 eyeOff38=ax38?VP(ax38->forward*g_eye_forward115.load()+ax38->up*g_eye_up115.load()):V3{};
        const V3 eye38=Add(headTarget,QRotateV(delta,eyeOff38));
        e.eye=RV(eye38);e.view=RV(fwd38);e.headTarget=RV(headTarget);e.solvedHead=RV(FromT(s.work[rig.head]));e.chest=RV(FromT(s.work[rig.spine3]));
        e.headErr=Len(Sub(FromT(s.work[rig.head]),headTarget));
        e.nativeHeadFromCal=RV(Sub(liveNativeHead123,baseHead));e.hmdDelta=RV(QRotateV(frame,XrToCryV(Sub(ts.head,trackedBase))));
        e.eyeOutside=rv::outside(e.native,e.eye);e.nativeBehindEye=rv::behindEye(e.native,e.eye,e.view);
        for(int side=0;side<2;++side){e.valid[side]=valid[side];if(!valid[side])continue;
            const V3 hand38=FromT(s.work[side?rig.hand_l:rig.hand_r]);e.hand[side]=RV(hand38);
            e.reachAsk[side]=reachAsk38[side];e.reachLimited[side]=s.reachLimited[side];
            e.handAhead[side]=rv::ahead(e.eye,e.view,e.hand[side]);e.handOutside[side]=rv::outside(e.native,e.hand[side]);}
        g_reach_view38.publish(e);
    }
#endif
    if(tx.traceH29&&ts.tick>=s.lastLog&&ts.tick-s.lastLog>=10000){s.lastLog=ts.tick;const auto composed137=s.composedJoints-composeStart137;char m[576]{};std::snprintf(m,sizeof(m),"V66.89-POSE TEMP_WRITE sample=%llu epoch=%llu fullUpper=%d handValidR/L=%d/%d fingersReadyR/L=%d/%d fixedLengths=1 nativeAttackPole=0 acceptedPairPending=1 composedPerSolve=%llu essential=%u/%u defer137=%d initRuns137=%llu",(unsigned long long)ts.sequence,(unsigned long long)tx.epoch,full,valid[0],valid[1],s.open[0].ready&&s.closed[0].ready,s.open[1].ready&&s.closed[1].ready,(unsigned long long)composed137,unsigned(s.essentialCount137),unsigned(s.count),g_compose_defer137.load()?1:0,(unsigned long long)s.initRuns137);diagnostic_trace::Write(m);}
    return true;
}
// C55 capture-only closure: distinguish a wrong learned wrist/finger grasp
// from an attachment/solver disagreement. Long-axis distance is an approximate
// geometry observation, never a new hard-coded grip or collision authority.
static void TraceCommittedGrip55(const CoherentTransaction6689& tx,const CommittedFrame6695& packet,bool published) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!published||!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    static kcd2vr::handling59c10r2::GripTraceGate gate;
    const auto& s=g_pose6689;
    for(int side=0;side<2;++side){
        if(!packet.valid[side]||!tx.physicalOutput.owned[side])continue;
        const int owner=tx.physicalOutput.active[side]?side:1-side;
        const auto& p=tx.physicalProfiles[owner];
        if((p.classId!=4&&p.classId!=7)||!p.primary||!tx.physicalOutput.active[owner]||
           !gate.due(side,p.key,tx.physicalOutput.support[owner]?2u:1u,now))continue;
        const auto weapon=(tx.physicalOutput.displayLatched[owner]||tx.physicalOutput.displayRejoinR13[owner])?tx.physicalOutput.displayWeapons[owner]:tx.physicalOutput.weapons[owner];
        const auto grasp=side==owner?p.grip[side]:tx.physicalState.weapon[owner].supportGrip169r2;
        const auto wanted=ph::compose(weapon,grasp);const ph::Pose actual{PQ(QFromQuatT(packet.allowed[side])),PV(FromT(packet.allowed[side]))};
        V3 six[6]{};FingerContactGeometry103 fingers{};
        const bool geometry=PalmGeometry103(s,side,QFromQuatT(packet.allowed[side]),0,0,p.key,six,fingers);
        const auto palm=actual.p+(geometry?PV(six[0])*.5:pc::Vec3{});
        const auto root=ph::point(weapon,p.grip[owner].p),tip=ph::point(weapon,p.tip);
        const auto line=pc::unit(tip-root);const auto delta=palm-root;
        const double radial=line?pc::length(delta-(*line)*pc::dot(delta,*line)):-1.;
        char m[640]{};std::snprintf(m,sizeof(m),
            "C55 GRIP_CLOSURE side=%c role=%s key=%016llX sample=%llu class=%d nativeSource=%u wristClosureCm=%.3f wristClosureDeg=%.3f palmFromWristCm=%.3f palmToLongAxisCm=%.3f localWrist=(%.4f,%.4f,%.4f) nativeFingers=%d frame=COMPLETED_SHARED_POSE meshClearance=UNPROVEN writes=NONE",
            side?'L':'R',side==owner?"PRIMARY":"SUPPORT",(unsigned long long)p.key,(unsigned long long)packet.stamp.sample,p.classId,
            unsigned(p.supportSource37),pc::length(actual.p-wanted.p)*100,pc::angle(actual.q,wanted.q)*57.2957795,
            geometry?Len(six[0])*50:-1.,radial*100,grasp.p.x,grasp.p.y,grasp.p.z,geometry?1:0);
        diagnostic_trace::Write(m);
    }
}
#include "../subsystems/doors/hand_only_h40_pose.inl"
static void PublishCommitted6689(CoherentTransaction6689& tx) noexcept {
    if(tx.traceH29||tx.motionSessionD68)tx.tracePublishAttemptedH29=true;
    const auto rejectPublish169r2=[&](unsigned line){
        g_native_rejoin_r28.clear();
        if(tx.traceH29)tx.tracePublishLineH29=int(line);
        if(diagnostics::ownership48::active(GetTickCount64())){
            diagnostics::ownership48::Event e{};e.stage=diagnostics::ownership48::PoseSkip;
            e.tick=GetTickCount64();e.thread=GetCurrentThreadId();e.sample=tx.tracking.sequence;
            e.publication=tx.publicationD49;e.epoch=tx.epoch;e.actor=reinterpret_cast<std::uintptr_t>(g_pose6689.character);
            e.sourceEpoch=tx.tracking.source_epoch;e.sourceNs=tx.tracking.timeNs;e.poseTime=tx.tracking.poseTime;
            e.code=int(line);e.flags=tx.consumersD49;e.v[0]=float(tx.tracePacketPublishedH29);e.v[1]=float(tx.pairedPublishedD49);
            diagnostics::ownership48::name(e,tx.consumersD49?"OWNER_RETIRED_AFTER_CONSUMERS_D49":
                tx.tracePacketPublishedH29&&!tx.pairedPublishedD49?"PAIRED_FRAME_REFUSED_D49":"POSE_PUBLICATION_REFUSED_D49");
            (void)diagnostics::ownership48::push(e);
        }
        g_committed_pose_valid6689.store(false,std::memory_order_release);
        kcd2vr::hand_only_door_h40::RevokeCompleted();
        for(int side=0;side<2;++side){(void)g_hand_presence_source_h24[side].publish({});handPresenceH24::CancelPresenceH24(side);}
        RetireTrackedFrame51();
        if(tx.bowContext169r2&&tx.committed){auto r=tx.bowEvidence169r2;r.stage=2;r.line=line;r.committed=true;
            kcd2vr::diagnostics::bow169r2::push(r);}
    };
    if(!tx.committed||!tx.restored||g_pose_restore_fault6689.load()||tx.epoch!=g_pose_epoch6689.load()||
       !CoherentOwnerCurrent57(tx)){rejectPublish169r2(__LINE__);return;}
    auto& s=g_pose6689;CommittedFrame6695 packet{};
    // Door-only identity annotation. Capture before packet construction and
    // compare the ledger publication again immediately before publication.
    const auto doorIdentityVersion38=g_player_publication37.load(std::memory_order_acquire);
    const auto doorPlayer38=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire));
    const auto doorCharacter38=g_player_char37.load(std::memory_order_acquire);
    const auto doorLease38=g_player_lease37.load(std::memory_order_acquire);
    void* vft=nullptr;void* update=nullptr;
    if(!Read(s.character,vft)||!vft||!Read(static_cast<unsigned char*>(vft)+0x190,update)||update!=reinterpret_cast<void*>(WhBase()+0x423C30ull)||!Read(static_cast<unsigned char*>(s.character)+0xA08,packet.world)){rejectPublish169r2(__LINE__);return;}
    double norm=0;for(float f:packet.world)if(!std::isfinite(f)){rejectPublish169r2(__LINE__);return;}
    for(int i=0;i<4;++i)norm+=double(packet.world[i])*packet.world[i];if(norm<.8||norm>1.2||packet.world[7]<.1f||packet.world[7]>10){rejectPublish169r2(__LINE__);return;}
    if(tx.traceH29){std::copy_n(packet.world,8,tx.traceWorldH29);tx.traceWorldKnownH29=true;}
    const int hands[2]{s.rig.hand_r,s.rig.hand_l};
    packet.headEvidence122=tx.fullHead122;
    if(packet.headEvidence122){packet.nativeHead122=FromT(s.original[s.rig.head]);packet.solvedHead122=FromT(s.work[s.rig.head]);
        packet.targetHead122=tx.headTarget122;packet.pelvis122=FromT(s.original[s.rig.pelvis]);}
    packet.valid[0]=tx.tracking.right_valid;packet.valid[1]=tx.tracking.left_valid;
    packet.mapGripM39[0]=tx.tracking.right_grip;packet.mapGripM39[1]=tx.tracking.left_grip;
    packet.pickupTriggerD80[0]=tx.tracking.right_trigger;packet.pickupTriggerD80[1]=tx.tracking.left_trigger;
    packet.mapFoldTrackedM39=tx.tracking.allowed&&tx.tracking.head_valid&&
        tx.tracking.right_valid&&tx.tracking.left_valid&&!tx.tracking.head_held&&
        !tx.tracking.right_held&&!tx.tracking.left_held;
    for(int side=0;side<2;++side)packet.mapPresentationOwnerM34[side]=tx.mapPresentationOwnerM34[side];
    for(int side=0;side<2;++side){packet.native[side]=s.original[hands[side]];
        packet.allowed[side]=tx.handSemanticReadyD62?tx.handSemanticD62[side]:s.work[hands[side]];
        packet.displayedD62[side]=s.work[hands[side]];packet.predictedDisplayD62[side]=tx.handPresentOnlyD61[side];
        packet.smithFixture[side]=tx.smithFixture[side];}
    packet.displayPolicyD62=tx.handPresentPolicyD61;packet.displayTargetTimeD62=tx.handPresentTimeD62;
    packet.handTimeNsR118=tx.tracking.poseTime; // R118: firm prop grip derives hand velocity on this clock
    // Skinning restores native bones before publication. Attachment rendering
    // needs these completed bones explicitly, not the later native cache.
    // Only joints owned by THIS successful full-body transaction are eligible.
    packet.stowedPolicyD63=tx.policyGeneration57;
    if(tx.stowedFullD63)for(std::size_t i=0;i<std::size(kStowedSlotsD63);++i){
        const int j=s.stowedJointsD63[i];
        if(j>=0&&j<int(s.count)&&tx.selected[j]&&ValidPose6689(s.work[j])){
            QuatT28 model=s.work[j];
            // R112: final trunk ownership must not import a native attack's
            // animated stowed socket/helper after correcting the body itself.
            // The native attachment relative/additional transforms stay native.
            (void)StowedReferencePoseR112(s,i,tx.selected,model);
            packet.stowedD63[i]={model,j,true};
        }
    }
    PublishStowedExtraR117(s,tx.selected,tx.stowedFullD63,packet); // R117: back polearm etc. (unnamed sockets)
    // Original socket and native wrist share this completed transaction.
    // Qualified attachment bootstrap reads these copies, never already solved bones.
    packet.itemHandoverRevisionD71=contact_handover_d71::ItemHandover()?contact_handover_d71::ItemRevision():0;
    packet.fistSkeletonH31=reinterpret_cast<std::uintptr_t>(s.owner);packet.fistJointCountH31=s.count;
    for(int side=0;side<2;++side){
        const int slot=side?s.rig.item_l:s.rig.item_r;packet.fistWristBoneH31[side]=hands[side];packet.fistSlotBoneH31[side]=slot;
        if(slot>=0&&slot<int(s.count)&&s.parent[slot]==hands[side]&&ValidPose6689(s.work[slot])){
            // Two immutable originals are cheap and also serve first native
            // minigame and ordinary pickup placement. Only other legacy tools
            // require the optional handover revision in OriginalToolGripD71.
            if(ValidPose6689(s.original[slot])){
                packet.originalToolSlotD71[side]=s.original[slot];packet.originalToolSlotValidD71[side]=true;}
            packet.fistSlotH31[side]=tx.handSemanticReadyD62?tx.handSemanticSlotD62[side]:s.work[slot];
            packet.fistSlotValidH31[side]=!tx.handSemanticReadyD62||tx.handSemanticSlotValidD62[side];
        }
    }
    // Four metacarpal bases define the palm without evaluating finger curves a
    // second time. Fixed anatomy, transformed by the committed wrist rotation.
    for(int side=0;side<2;++side)if(s.fingerMap[side]){
        V3 centre{};bool valid=true;
        for(int finger=1;finger<5;++finger){const auto& chain=s.fingers[side][finger];
            if(chain.count<1||chain.id[0]<0||chain.id[0]>=s.count){valid=false;break;}
            centre=Add(centre,FromT(s.restLocal[chain.id[0]]));}
        centre=Mul(centre,.25f);const float length=Len(centre);
        if(valid&&std::isfinite(length)&&length>.015f&&length<.20f){
            const V3 outward=QRotateV(QFromQuatT(packet.allowed[side]),centre);
            packet.palm59c9[side]=Add(FromT(packet.allowed[side]),Mul(outward,.5f));
            packet.palmDirection59c9[side]=Mul(outward,1.f/length);packet.palmValid59c9[side]=true;}
    }
    for(int side=0;side<2;++side){
        QToQuatT(packet.propRequested5916c1[side],QP(tx.propRequested5916c1[side].q));
        ToT(packet.propRequested5916c1[side],VP(tx.propRequested5916c1[side].p));
        packet.propRequestedValid5916c1[side]=tx.propRequestedValid5916c1[side];
    }
    packet.stamp={tx.epoch,tx.tracking.sequence,tx.tracking.tick,reinterpret_cast<std::uintptr_t>(s.character)};
    packet.doorTrackingEpoch66=tx.tracking.source_epoch;
    for(int side=0;side<2;++side){ // C43: grab window open/close receipts (edge-only, F5 rung 1); Henry's entity world delta answers "did he lunge"
        kcd2vr::grab43::Use u43{};const auto now43=std::uint64_t(GetTickCount64());
        if(!kcd2vr::grab43::read(side,u43)||(!s.grab43[side].open&&!kcd2vr::grab43::fresh(u43,now43)))continue;
        const float henry43[3]{packet.world[4],packet.world[5],packet.world[6]};float moved43=0;const char* end43="";
        const bool wristOwned43=g_wristOwnedByController59c12&&g_wristOwnedByController59c12(side);
        const int edge43=s.grab43[side].frame(u43,kcd2vr::grab43::fresh(u43,now43)&&
            kcd2vr::grab43::current(u43,{packet.stamp.actor,tx.epoch,tx.tracking.source_epoch}),wristOwned43,henry43,now43,moved43,end43);
        if(edge43==2&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now43)){
            char m[320]{};std::snprintf(m,sizeof(m),"C43 GRAB_ALIGN edge=WINDOW_END side=%d target=%u serial=%u end=%s vetoed=%d nativeActionSeen=%d maxNativeHandPullM=%.3f henryMovedM=%.3f ms=%llu entityMover=NATIVE_UNOBSERVED(no position veto)",
                side,u43.entity,u43.serial,end43,s.grab43[side].vetoed?1:0,s.grab43[side].nativeSeen?1:0,s.grab43[side].maxPull,moved43,
                (unsigned long long)(now43>=s.grab43[side].openedAt?now43-s.grab43[side].openedAt:0));
            diagnostic_trace::Write(m);}
    }
    kcd2vr::grasp13::FingerReference55 graspFingers55[2]{};kcd2vr::grasp13::Binding graspReference55{};
    if(g_graspBinding13&&g_graspBinding13(graspReference55)&&!graspReference55.gripReady55&&
       graspReference55.side>=0&&graspReference55.side<2)
        (void)OriginalGraspFingers55(s,graspReference55.side,graspReference55,tx.epoch,packet.stamp.sample,
            packet.stamp.tick,std::uint64_t(GetTickCount64()),graspFingers55[graspReference55.side],&tx.acquisition54);
    // Carry original fingers even when the new socket has not reached the mailbox.
    // First-presentation adoption must not suppress the canonical finger capture.
    // R94: this payload was captured only for the exact ordinary grasp above.
    // Carry it with its original wrist/slot transaction even when the legacy
    // tool trial is off; first callback capture still validates the full pair.
    for(int side=0;side<2;++side)
        packet.tools59c10[side].fingers55=graspFingers55[side];
    BuildToolPoses59c10(packet,s.original.data(),s.count,graspFingers55);
    for(int side=0;side<2;++side)packet.alchemyContactD80[side]=tx.alchemyContactD80[side];
    BuildManualArrowPoses64(packet,s.original.data(),s.count,tx.physicalInput,tx.physicalOutput,tx.physicalProfiles[1],s.rest.data(),s.rig.hand_r);
    BuildCrossbowPosesXR34(packet,s.original.data(),s.count,s.rest.data(),s.rig.hand_r,s.rig.hand_l);
    packet.doorPlayer38=kcd2vr::door_candidates38::framePlayer(doorIdentityVersion38,
        g_player_publication37.load(std::memory_order_acquire),packet.stamp.epoch,g_pose_epoch6689.load(std::memory_order_acquire),
        packet.stamp.actor,doorCharacter38,doorPlayer38,doorLease38,GetTickCount64());
    // One completed transaction supplies the bow fraction, weapon and wrists.
    // Previously body publication preceded PhysPublish: the draw callback could
    // read body N and physical N-1 and yield the whole string/arrow to native.
    bool physicalCoherentR93=tx.physicalActive;
    for(int side=0;side<2&&physicalCoherentR93;++side)if(tx.physicalOutput.owned[side])
        physicalCoherentR93=pc::length(PV(FromT(packet.allowed[side]))-((tx.physicalOutput.displayLatched[side]||tx.physicalOutput.displayRejoinR13[side])?tx.physicalOutput.displayHands[side]:tx.physicalOutput.hands[side]).p)<.004;
    packet.bowDrawR93=kcd2vr::bow_draw_publication_r93::prepare(tx.physicalInput,tx.physicalOutput,
        tx.physicalProfiles[1],packet.stamp,tx.physicalEquipment.request,
        physicalCoherentR93&&g_weapon_control_mode.load()>=4,ph::physical_bow175(tx.physicalInput));
    if(tx.reloadWorkingHandXR34){
        // Reload input is auxiliary to the completed body. A missing native
        // socket/clock may decline reload ownership, never revoke head/hands.
        if(g_prepareReloadPoseXR34&&g_prepareReloadPoseXR34(tx.reloadPassengerXR34,packet,s.original.data(),s.count,tx.tracking))
            tx.consumersD49|=256u;
    }
    // Native restoration has succeeded. Mark the WHOLE paired publication as
    // in-flight before changing the completed body packet, including callbacks
    // with exactly the same XR sample and model transform as their predecessor.
    FramePublicationScope51 publication51;
    if(!publication51.active||!CoherentOwnerCurrent57(tx)){rejectPublish169r2(__LINE__);return;}
    packet.nativeCrouchOffsetR93=tx.frame51.valid?kcd2vr::tracked_frame51::shared_height_offset_r98(tx.frame51):0; // R98: + native height share
    packet.publicationR24=publication51.value;tx.publicationD49=publication51.value;
    packet.reloadSourceXR34.bodyPublicationR95=packet.publicationR24;
    packet.reloadTargetXR34.publicationR95=packet.publicationR24;
    if(!g_committed_frame6695.publish(packet)){rejectPublish169r2(__LINE__);return;}
    tx.tracePacketPublishedH29=true; // Also needed by the bounded F2 refusal receipt.
    // Rejoin the same completed packet after native restoration. Camera readers
    // require this publication serial as well as stamp/world/control, so a failed
    // pose, stale packet or overlapping publication cannot borrow another frame.
    const bool frameWorldSame51=tx.frame51.valid&&
        pc::length(tx.frame51.root-pc::Vec3{packet.world[4],packet.world[5],packet.world[6]})<=.002&&
        pc::angle(tx.frame51.rotation,pc::Quat{packet.world[0],packet.world[1],packet.world[2],packet.world[3]})<=.002&&
        std::fabs(tx.frame51.scale-packet.world[7])<1e-6;
    if(!frameWorldSame51||tx.control51!=g_frame_control51.load(std::memory_order_acquire))tx.frame51={};
    else{tx.frame51.stamp=packet.stamp;tx.frame51.publication=publication51.value;
        tx.frame51.root={packet.world[4],packet.world[5],packet.world[6]};
        tx.frame51.rotation={packet.world[0],packet.world[1],packet.world[2],packet.world[3]};}
    const bool pairedFramePublishedR38=g_tracked_frame51.publish(tx.frame51);
    tx.pairedPublishedD49=pairedFramePublishedR38;
    // Mailbox contention is a publication failure, not an intentionally empty
    // hand-only smith frame. Do not dispatch tools/grasp/physics on half a pair.
    // Keep the serial odd through the current-owner check so readers cannot
    // acquire a pair already invalidated while either mailbox was written.
    if(!pairedFramePublishedR38){rejectPublish169r2(__LINE__);return;}
    if(!CoherentOwnerCurrent57(tx)){rejectPublish169r2(__LINE__);return;}
    // Reload input and bow draw data are already inside this completed packet.
    publication51.finish(); // both payload writes and pre-consumer owner check succeeded
    if(diagnostics::ownership48::active(GetTickCount64()))for(int side48=0;side48<2;++side48){
        diagnostics::ownership48::Event e{};e.stage=diagnostics::ownership48::Pose;e.tick=GetTickCount64();e.thread=GetCurrentThreadId();
        e.sample=packet.stamp.sample;e.publication=publication51.value;e.epoch=packet.stamp.epoch;e.actor=packet.stamp.actor;e.side=side48;
        e.sourceEpoch=tx.tracking.source_epoch;e.sourceNs=tx.tracking.timeNs;e.poseTime=tx.tracking.poseTime;
        const auto raw=side48?tx.tracking.left:tx.tracking.right;const auto rq=side48?tx.tracking.lq:tx.tracking.rq;const auto allowed=FromT(packet.allowed[side48]);const auto aq=QFromQuatT(packet.allowed[side48]);const auto native=FromT(packet.native[side48]);
        const float data[]{raw.x,raw.y,raw.z,rq.x,rq.y,rq.z,rq.w,allowed.x,allowed.y,allowed.z,aq.x,aq.y,aq.z,aq.w,native.x,native.y,native.z,
            packet.world[4],packet.world[5],packet.world[6],packet.world[0],packet.world[1],packet.world[2],packet.world[3],tx.tracking.head.x,tx.tracking.head.y,tx.tracking.head.z,
            side48?tx.tracking.left_grip:tx.tracking.right_grip,side48?tx.tracking.left_trigger:tx.tracking.right_trigger,float(tx.tracking.allowed),float(packet.valid[side48]),float(tx.frame51.valid)};
        std::copy_n(data,32,e.v);e.flags=(tx.tracking.head_held?1u:0u)|(tx.tracking.right_held?2u:0u)|(tx.tracking.left_held?4u:0u);
        diagnostics::ownership48::name(e,"COMPLETED_MODEL_POSE_RAW_IS_XR_SPACE");(void)diagnostics::ownership48::push(e);
        if(tx.physicalOutput.active[side48]){
            e.stage=7;e.item=tx.physicalProfiles[side48].key;e.generation=tx.physicalProfiles[side48].generation;
            const auto a=tx.physicalInput.hands[side48],b=tx.physicalOutput.hands[side48],c=tx.physicalOutput.weapons[side48],d=tx.physicalOutput.displayWeapons[side48];
            const float motor[]{float(a.p.x),float(a.p.y),float(a.p.z),float(a.q.x),float(a.q.y),float(a.q.z),float(a.q.w),float(b.p.x),float(b.p.y),float(b.p.z),float(b.q.x),float(b.q.y),float(b.q.z),float(b.q.w),float(c.p.x),float(c.p.y),float(c.p.z),float(c.q.x),float(c.q.y),float(c.q.z),float(c.q.w),float(d.p.x),float(d.p.y),float(d.p.z),float(d.q.x),float(d.q.y),float(d.q.z),float(d.q.w),float(tx.physicalInput.controllerAnchor119),float(tx.physicalInput.constraintDrive129),float(tx.physicalInput.physicalCombat177),float(tx.physicalOutput.support[side48])};
            std::copy_n(motor,32,e.v);e.flags=(tx.physicalOutput.blocked[side48]?1u:0u)|(tx.physicalOutput.displayLatched[side48]?2u:0u)|(tx.physicalOutput.nativeRejoinSweepR28[side48]?4u:0u)|(tx.physicalOutput.initialAcquireSweepD44[side48]?8u:0u)|(tx.physicalOutput.contact119[side48].targetPlacementD70?16u:0u);diagnostics::ownership48::name(e,tx.physicalOutput.contact119[side48].targetPlacementD70?"MOTOR_INITIAL_TARGET_PROXY_CLEAR_D70":tx.physicalOutput.initialAcquireSweepD44[side48]?"MOTOR_INITIAL_ACQUIRE_CHECKED_SWEEP_D44":tx.physicalOutput.nativeRejoinSweepR28[side48]?"MOTOR_NATIVE_RETURN_CHECKED_SWEEP_R28":"MOTOR_REQUEST_ALLOWED_WEAPON_DISPLAY_ALL_MODEL");(void)diagnostics::ownership48::push(e);
        }

    }

    tx.consumersD49|=1u; // presence dispatch entered (not an acceptance claim)
    for(int side=0;side<2;++side){
        const bool active=tx.handPresenceValidH24[side]&&HandPresenceContextH24(tx.handPresenceH24[side]);
        (void)g_hand_presence_source_h24[side].publish({tx.handPresenceH24[side],active});
        if(active)(void)handPresenceH24::SubmitPresenceH24(side,tx.handPresenceH24[side]);
        else handPresenceH24::CancelPresenceH24(side);
    }
    tx.consumersD49|=2u;PublishGuidedTools55(packet,tx.tracking.source_epoch);
    if(!tx.reloadWorkingHandXR34&&g_graspHands13&&g_graspEnabled13&&g_graspEnabled13()){
        kcd2vr::grasp13::HandFrame h{};h.character=packet.stamp.actor;h.epoch=tx.epoch;
        h.sample=tx.tracking.sequence;h.tick=tx.tracking.tick;
        for(int side=0;side<2;++side){
            const bool acquisitionHand55=tx.frame51.valid&&tx.frame51.acquisitionSerial&&side==tx.acquisition54.side;
            h.tracked[side]=((tx.tracking.allowed&&g_gameplay_allowed.load(std::memory_order_acquire))||acquisitionHand55)&&
                tx.tracking.head_valid&&!tx.tracking.head_held&&
                (side?tx.tracking.left_valid&&!tx.tracking.left_held:tx.tracking.right_valid&&!tx.tracking.right_held);
            h.grip[side]=side?tx.tracking.left_grip:tx.tracking.right_grip;
            const V3 semanticHandD61=FromT(packet.allowed[side]);
            h.chestDistance[side]=Len(Sub(semanticHandD61,FromT(s.work[s.rig.spine3])));
        }
        tx.consumersD49|=4u;g_graspHands13(h);
    }
    const bool contactWorldSame169r1=tx.passiveWorldValid169r1&&
        pc::length(PV(tx.passiveWorld169r1.t)-pc::Vec3{packet.world[4],packet.world[5],packet.world[6]})<=.002&&
        pc::angle(PQ(tx.passiveWorld169r1.q),pc::Quat{packet.world[0],packet.world[1],packet.world[2],packet.world[3]})<=.002&&
        std::abs(tx.passiveWorld169r1.scale-packet.world[7])<1e-5;
    tx.consumersD49|=8u;
    for(auto sample:tx.passive169r1){
        sample.worldCoherent=sample.worldCoherent&&contactWorldSame169r1;
        if(sample.eligible&&!sample.worldCoherent)sample.status=kcd2vr::passive169r1::Status::Unknown;
        // D62: these witnesses were produced by the raw semantic solve, before
        // optional display. They never contain future-display contact queries.
        if(sample.source==kcd2vr::passive169r1::Source::HandWorld&&sample.side>=0&&sample.side<2){
            const auto& target=tx.passiveTargetsH21[sample.side];
            kcd2vr::physics::committed_push_h21::PublishCommitted(sample,target);
            const bool fistH21=(sample.side?tx.tracking.left_grip:tx.tracking.right_grip)>.55f&&
                (sample.side?tx.tracking.left_trigger:tx.tracking.right_trigger)>.55f;
            kcd2vr::touch_react_service60::publishPlan(sample,target.contact,fistH21,
                sample.side?tx.tracking.left_grip:tx.tracking.right_grip,packet.publicationR24);
            // DS-2026-10-23: the free-hand touch feed. The living contact publishes the
            // near-living flag (the grab-over-talk) AND the touch record; the F9 rung-3
            // physics lab's own switches gate the native reactions in its tick.
            {
                const auto& w=target.contact;
                const bool living=w.body==kcd2vr::npc_contact172::Body::Living;
                if(living&&sample.motion.valid&&sample.motion.incoming>0){
                    const auto now=std::uint64_t(GetTickCount64());
                    kcd2vr::grab_over_talk_ds::PublishLivingContact(sample.side,now);
                    kcd2vr::grab_over_talk_ds::HandTouch ht{};
                    ht.entity=w.entityId;ht.part=w.part;ht.side=sample.side;ht.tick=sample.tick;ht.sample=sample.sequence;
                    ht.point[0]=float(sample.point.x);ht.point[1]=float(sample.point.y);ht.point[2]=float(sample.point.z);
                    ht.normal[0]=float(sample.normal.x);ht.normal[1]=float(sample.normal.y);ht.normal[2]=float(sample.normal.z);
                    ht.speed=float(sample.motion.incoming);
                    (void)kcd2vr::grab_over_talk_ds::handTouch.publish(ht);
                }
            }
        }
        // These samples are admitted only after the same successful pose packet.
        // The haptic queue avoids replacing F11's stronger contact-entry cue.
        kcd2vr::diagnostics::passive169r1::observe(sample,sample.source!=kcd2vr::passive169r1::Source::SelfTorso); // C22b: touching your own body is not feedback
    }
    bool physicalPublishAttempted169r2=false;
    const auto physicalPublication54=g_physical_gates101R2[2].load(std::memory_order_acquire);
    if(tx.physicalActive){
        bool coherent=true;for(int side=0;side<2;++side)if(tx.physicalOutput.owned[side])
            coherent=coherent&&pc::length(PV(FromT(packet.allowed[side]))-((tx.physicalOutput.displayLatched[side]||tx.physicalOutput.displayRejoinR13[side])?tx.physicalOutput.displayHands[side]:tx.physicalOutput.hands[side]).p)<.004;
        if(coherent){physicalPublishAttempted169r2=true;tx.consumersD49|=16u;PhysPublish6698(tx.physicalInput,tx.physicalOutput,tx.physicalProfiles,tx.physicalEquipment,packet.world,tx.physicalState,tx.physicalHolsters,&tx.physicalCorrelation126,tx.tracking.source_epoch,packet.stamp.actor,publication51.value);}
        else {g_physical_gates101R2[3].fetch_add(1,std::memory_order_relaxed);} // Skip this contact publication, not the existing native strike.
    }
    if(PhysicalCombat177Enabled()&&!physicalPublishAttempted169r2)g_phys_ready_sample6698.store(0,std::memory_order_release);
    TraceCommittedGrip55(tx,packet,physicalPublishAttempted169r2&&physicalPublication54!=g_physical_gates101R2[2].load(std::memory_order_acquire));
    tx.consumersD49|=32u;
    PublishOwnBladeContact54(tx,packet,physicalPublishAttempted169r2&&
        physicalPublication54!=g_physical_gates101R2[2].load(std::memory_order_acquire));
    PublishShieldPat54(tx,packet,physicalPublishAttempted169r2&&
        physicalPublication54!=g_physical_gates101R2[2].load(std::memory_order_acquire));
    if(tx.bowContext169r2){auto r=tx.bowEvidence169r2;r.committed=tx.committed;r.posePacketPublished=true;r.physicalPublishAttempted=physicalPublishAttempted169r2;r.worldSame=contactWorldSame169r1;
        kcd2vr::diagnostics::bow169r2::push(r);}
    if(!CoherentOwnerCurrent57(tx)){rejectPublish169r2(__LINE__);return;}
    if(physicalPublishAttempted169r2)PhysSameFrameD69(publication51.value,packet.stamp.sample,tx.motionSessionD68,tx.motionCycleD68);
    if(!CoherentOwnerCurrent57(tx)){rejectPublish169r2(__LINE__);return;}
    // Render-only: publish a bounded envelope of the FINAL allowed model-space
    // arms and actual composed finger chains, after native restoration and the
    // final owner check. No native calls, extra rig walk or bone/cache writes.
    kcd2vr::render_bounds63::Envelope bounds63{};
    bounds63.character=packet.stamp.actor;bounds63.epoch=packet.stamp.epoch;
    bounds63.sample=packet.stamp.sample;bounds63.tick=packet.stamp.tick;
    bounds63.publication=publication51.value;bounds63.policy=tx.policyGeneration57;
    bounds63.control=tx.control51;std::copy_n(packet.world,8,bounds63.world);
    // A lost/held controller cannot revoke the CURRENT opposite arm's
    // culling correction. Include only that side's committed selected bones;
    // the untouched native box still supplies the rest of Henry's bounds.
    // This grants no pose, contact, input or tracking authority to the lost hand.
    const bool currentHand63[2]{packet.valid[0]&&tx.tracking.right_valid&&!tx.tracking.right_held,
        packet.valid[1]&&tx.tracking.left_valid&&!tx.tracking.left_held};
    bounds63.freshTracked=tx.tracking.head_valid&&!tx.tracking.head_held&&(currentHand63[0]||currentHand63[1]);
    if(bounds63.freshTracked){
        const auto point63=[&](int id){if(id>=0&&id<int(s.count)&&tx.selected[id]){
            const auto p=FromT(s.work[id]);kcd2vr::render_bounds63::add(bounds63,p.x,p.y,p.z);}};
        for(int side=0;side<2;++side)if(currentHand63[side]){
            point63(side?s.rig.clav_l:s.rig.clav_r);point63(side?s.rig.upper_l:s.rig.upper_r);
            point63(side?s.rig.fore_l:s.rig.fore_r);point63(side?s.rig.hand_l:s.rig.hand_r);
            if(s.fingerMap[side])for(const auto& chain:s.fingers[side])
                for(int k=0;k<chain.count&&k<int(chain.id.size());++k)point63(chain.id[k]);
        }
    }
    const bool envelopePublishedH29=g_render_bounds63.publish(bounds63);
    if(tx.traceH29||tx.motionSessionD68)tx.traceEnvelopePublishedH29=envelopePublishedH29;
    g_committed_character6689.store(s.character,std::memory_order_release);
    // All rejection paths, including the late owner recheck, precede this.
    // An intentionally empty tracked frame is still a successful paired publish
    // for hand-only smithing; the native tool/socket gates remain authoritative.
    if(pairedFramePublishedR38){
        s.trackedFrame51=tx.frame51.valid?tx.frameState51:kcd2vr::tracked_frame51::State{};
        s.stationBodyH41=tx.frame51.valid?tx.stationBodyH41:kcd2vr::station_body_h41::State{};
        s.bodyFollowR38=tx.frame51.valid?tx.bodyFollowR38:kcd2vr::body_follow_r38::State{};
        // Display grace is not a new pre-request acquisition reference.
        if(tx.frame51.valid&&!tx.frame51.acquisitionSerial){
            if(kcd2vr::tracked_frame51::ordinary_reference_d86(tx.frame51)){
                if(!tx.tracking.head_held&&!tx.tracking.right_held&&!tx.tracking.left_held)s.ordinaryReference54=tx.frame51;
            }else s.ordinaryReference54={}; // dice/rider/station cannot donate a later pickup reference
        }
        if(tx.frame51.valid&&tx.frame51.acquisitionSerial){s.acquisitionReference54=tx.acquisitionReference54;
            s.acquisitionSerial54=tx.acquisition54.serial;s.acquisitionAction54=tx.acquisition54.action;
        }else{s.acquisitionReference54={};s.acquisitionSerial54=s.acquisitionAction54=0;}
    }else{s.trackedFrame51={};s.stationBodyH41={};s.bodyFollowR38=kcd2vr::body_follow_r38::State{};s.acquisitionReference54={};s.acquisitionSerial54=s.acquisitionAction54=0;}
    if(pairedFramePublishedR38&&tx.motionD47.ready){
        auto& historyD47=tx.motionD47;s.archer169r2=historyD47.archer;
        for(int side=0;side<2;++side)if(historyD47.valid[side]){
            s.lastBend[side]=historyD47.bend[side];s.elbow[side]=historyD47.elbow[side];s.pronation[side]=historyD47.pronation[side];
            s.shoulderFitD45[side]=historyD47.shoulderFit[side];s.elbowPolicyD45[side]=historyD47.elbowPolicy[side];
            s.naturalTick178r14[side]=historyD47.naturalTick[side];s.handoverW178r14[side]=historyD47.handover[side];
            if(historyD47.contactMode)s.contact[side]=historyD47.contact[side];
            s.chestAssist[side]={};s.clavicleAssist[side]=historyD47.clavicle[side];s.shoulder[side]=historyD47.shoulder[side];
        }
        kcd2vr::hand_smooth_h50::Commit(s.handSmoothH50,s.handFreeH50,historyD47.smooth,historyD47.free);
        historyD47.published=true;
    }
    if(pairedFramePublishedR38&&!tx.reloadWorkingHandXR34){
        // D62: allowed wrist, slot and contact history are the raw semantic pass.
        tx.consumersD49|=64u;FistPublish101(tx.tracking,packet,s.handBlocked);
        // D54: contact joins this completed fist frame, never the previous
        // mailbox sample. Preserve the same queried WORLD and native guards.
        for(auto sample:tx.passive169r1){
            sample.worldCoherent=sample.worldCoherent&&contactWorldSame169r1;
            if(sample.eligible&&!sample.worldCoherent)sample.status=kcd2vr::passive169r1::Status::Unknown;
            if(sample.source==kcd2vr::passive169r1::Source::HandWorld&&sample.side>=0&&sample.side<2)
                FistPublishContactR11(sample,tx.passiveTargetsH21[sample.side].contact,tx.passiveQueryTickR11[sample.side]);
        }
    }
    if(pairedFramePublishedR38)CommitSmithContactHistoryR38(tx.smithContactR38);
    // A free-pair witness is published only with this completed body, after
    // every rejection path. Live policy/ownership changes retire it immediately.
    const bool pairOwnerCurrentD39=tx.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
        tx.policyGeneration57==g_pose_policy_generation57.load(std::memory_order_acquire)&&
        tx.control51==g_frame_control51.load(std::memory_order_acquire)&&
        tx.tracking.source_epoch==kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire)&&
        tx.handPairD39.key.contactControl==g_contact_control_epoch6695.load(std::memory_order_acquire)&&
        tx.nativeHeldAdmissionT4==heldT4::AdmissionSerial()&&ph::handSeparationDs1.load(std::memory_order_relaxed)&&
        kcd2vr::frik_pose_probe::ExactHandItemState(0)==0&&kcd2vr::frik_pose_probe::ExactHandItemState(1)==0;
    if(pairedFramePublishedR38&&pairOwnerCurrentD39)
        kcd2vr::hand_pair_contact_d39::commit(s.handPairD39,tx.handPairD39,true,true);
    else s.handPairD39={};

    g_committed_pose_valid6689.store(true,std::memory_order_release);
    if(tx.traceH29&&packet.displayPolicyD62){
        static std::uint64_t nextDisplayReceiptD62=0;
        if(packet.stamp.tick>=nextDisplayReceiptD62){nextDisplayReceiptD62=packet.stamp.tick+1000;
            char line[420]{};std::snprintf(line,sizeof(line),
                "D62 HAND_DISPLAY_FINAL sample=%llu publication=%llu policy=%llu right=%d left=%d deltaM=%.5f/%.5f displayTime=%lld semanticTime=%lld semantic=RAW_COLLISION_IK input=UNCHANGED scope=FREE_OPEN_HANDS",
                (unsigned long long)packet.stamp.sample,(unsigned long long)packet.publicationR24,
                (unsigned long long)packet.displayPolicyD62,packet.predictedDisplayD62[0],packet.predictedDisplayD62[1],
                double(Len(Sub(FromT(packet.displayedD62[0]),FromT(packet.allowed[0])))),
                double(Len(Sub(FromT(packet.displayedD62[1]),FromT(packet.allowed[1])))),
                (long long)packet.displayTargetTimeD62,(long long)tx.tracking.poseTime);
            diagnostic_trace::Write(line);
        }
    }
    if(tx.traceH29||tx.motionSessionD68)tx.tracePublishedH29=true;
    if(tx.fingerDiagnostic28)tx.fingerPublished28=true;
    tx.consumersD49|=128u;PublishDoorSurface65(packet,tx.tracking.source_epoch);
    PublishHandOnlyDoorH40(tx,packet);
    animation_filter::combatPoseTick112.store(tx.tracking.allowed&&tx.tracking.head_valid&&
        !tx.tracking.head_held&&!tx.tracking.left_held&&!tx.tracking.right_held&&
        tx.tracking.left_valid&&tx.tracking.right_valid?tx.tracking.tick:0,std::memory_order_release);
    if(!g_f12_hand_delta_lock.test_and_set(std::memory_order_acquire)){
        std::memcpy(g_committed_model_world6689,packet.world,sizeof(packet.world));g_committed_stamp6689=packet.stamp;
        for(int side=0;side<2;++side){g_f12_native_hand[side]=packet.native[side];g_f12_vr_hand[side]=packet.allowed[side];g_f12_hand_delta_valid[side]=packet.valid[side];}
        g_f12_hand_delta_lock.clear(std::memory_order_release);
    }
    if(kcd2vr::smooth_lab::NativeRejoinPolicyR28()){
        bool raw[2]{};
        for(int side=0;side<2;++side){
            const auto& input=tx.physicalInput;
            const bool current=side?tx.tracking.left_valid&&!tx.tracking.left_held:tx.tracking.right_valid&&!tx.tracking.right_held;
            raw[side]=!tx.reloadWorkingHandXR34&&packet.valid[side]&&input.allowed&&input.tracked[side]&&
                input.intentTracked[side]&&tx.tracking.head_valid&&!tx.tracking.head_held&&current&&
                !tx.physicalOutput.owned[side]&&ph::finite(input.hands[side])&&
                pc::length(PV(FromT(packet.allowed[side]))-input.hands[side].p)<.004&&
                pc::angle(PQ(QFromQuatT(packet.allowed[side])),input.hands[side].q)<.01;
        }
        g_native_rejoin_r28.complete(PhysRejoinContextR28(s.character,tx.epoch,tx.tracking.source_epoch),
            packet.stamp.sample,packet.stamp.tick,packet.publicationR24,std::uint64_t(GetTickCount64()),raw);
    }
}
