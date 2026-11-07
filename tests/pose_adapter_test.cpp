#include <cstdlib> // abort if the immediate collision leaf ever stages deferred work
#include "../src/core/config_file_guard_d62.h"
#include "temp_ini_fixture_d62.h"
namespace kcd2vr::combat_diag_probe { inline bool HenryAttributes5916(double&,double&) noexcept {return false;} } // R59.16 host stub
#include "../src/frik_pose_probe.h"
#include "../src/subsystems/ranged_reload/crossbow_native_xr34.h"
#include "../src/core/native_grasp13.h"
#include "../src/subsystems/vrmap/map_hand_owner_m34.h"
#include "../src/vrik/motion_owned59c4.h"
using kcd2vr::frik_pose_probe::FullWeaponMode120;
using kcd2vr::frik_pose_probe::BowHandlingMode59c9;
using kcd2vr::frik_pose_probe::FocusedTestMode167;
namespace kcd2vr::frik_pose_probe { int FocusedTestMode167() noexcept {return 0;}
inline bool mockCombatExperiment175=false;
bool CombatExperiment175Enabled() noexcept {return mockCombatExperiment175;} }
using kcd2vr::frik_pose_probe::CombatExperiment175Enabled;
static bool mockGripDraw596c1=false;
namespace kcd2vr::frik_pose_probe { bool BowGripDrawEnabled596C1() noexcept {return mockGripDraw596c1;} }
using kcd2vr::frik_pose_probe::BowGripDrawEnabled596C1; // explicit input policy leaf
static bool StagedBowEnabled59() noexcept {return false;} // native staging is exercised separately
static bool ManualReadyFor596(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {return false;}
static bool CombatSynchronized59c10r9() noexcept {return false;} // neutral external mode; complete bridge exercises synchronized ownership
static bool PhysicalCombat177Enabled() noexcept {return false;} // neutral branch in this retained fixture
// Executes the ACTUAL production coherent_pose.inl, with native memory/getter/setter
// functions replaced by an in-memory 48-joint fixture. Not a game/ABI emulation.
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <mutex>
#include <optional>
#include <limits>
#include <new>
#include <string>
#include <vector>
#include "../src/vrik/pose_math.h"
#include "../src/vrik/elbow_shoulder_d45.h"
#include "../src/vrik/visual_selection59c7f.h" // Actual optional visual tracking dependency
#include "../src/vrik/smooth_lab_f2.h" // Same explicit visual-policy dependency as the production pose TU
#include "../src/vrik/npc_contact172.h"
namespace npc172=kcd2vr::npc_contact172;
#include "../src/subsystems/physics/runtime/native_hand_proxy_owner60.h"
#include "../src/subsystems/physics/runtime/native_hand_presence_pose_h24.h"
#include "../src/subsystems/physics/touch_react_service60.h"
namespace handPolicyH24=kcd2vr::native_hand_proxy_t4;
namespace handPresenceH24=kcd2vr::physics::hand_proxy60;
// Retained arm fixture has no native hand-body service. Real POD/helper types,
// with explicitly absent native services, match its existing T4-OFF contract.
#ifndef KCD2VR_POSE_FIXTURE_EXTERNAL_PRESENCE_H24
namespace kcd2vr::physics::hand_proxy60 {
inline bool SubmitPresenceH24(int,const kcd2vr::native_hand_proxy_t4::Input&) noexcept{return false;}
inline void CancelPresenceH24(int) noexcept{}
inline bool ReadPresenceFeedbackH24(int,PresenceFeedbackH24&) noexcept{return false;}
}
struct HandPresenceSourceH24 {handPolicyH24::Input input{};bool valid=false;};
static kcd2vr::core::SnapshotMailbox<HandPresenceSourceH24> g_hand_presence_source_h24[2];
static bool HandPresenceContextH24(const handPolicyH24::Input&) noexcept{return false;}
static bool HandPresenceIdentityH24(void*,std::uint64_t,handPolicyH24::Input&) noexcept{return false;}
#endif
static bool NativeActivityHandOwned46(bool,std::uintptr_t,std::uint64_t,int,std::uint64_t) noexcept{return false;}
#include "../src/subsystems/experimental/deepseek_options_h29.h"
#include "../src/diagnostics/vrik_trace_h29.h"
#include "../src/vrik/gesture_recovery_h30.h"
#include "../src/vrik/workstation178r20c1.h"
#include "../src/activity_eye59c19.h"
#include "../src/vrik/alchemy_tool18.h"
#include "../src/vrik/presentation_context55.h"
#include "../src/tracking_sample59c6.h"
namespace workstation178r20c1=kcd2vr::workstation178r20c1;
#include "../src/core/snapshot_mailbox.h"
#include "../src/native_weapon_body_t4.h"
#include "../src/vrik/native_held_pose_t4.h"
// This portable retained pose fixture has no native physics service. Keep its
// OFF contract explicit; the complete native_held_owner_t4_bridge executes the
// real source/ownership adapter and native_weapon_body_t4 exercises the backend.
namespace kcd2vr::native_weapon_body_t4 {
inline bool Enabled() noexcept {return false;}
inline bool HasUnretiredCollider() noexcept {return false;}
inline std::uint64_t AdmissionSerial() noexcept {return 0;}
inline bool Read(Achieved&) noexcept {return false;}
inline bool ReadContactOwnerH24(ContactOwnerH24&) noexcept {return false;}
inline void Submit(const Input&) noexcept {}
inline void Retire(const Owner&) noexcept {}
inline void Cancel() noexcept {}
}
#include "../src/core/door_candidates38.h"
#include "../src/vrik/kick176.h"
#include "../src/diagnostics/passive_contact169r1.h"
#include "../src/vrik/elbow_continuity.h"
#include "../src/vrik/arm_constraints.h"
#include "../src/vrik/upper_body.h"
#include "../src/vrik/archer_pose169r2.h"
#include "../src/diagnostics/bow_contact169r2.h"
#include "../src/vrik/finger_surface.h"
#include "../src/vrik/knuckle_contact174.h" // actual final-contact FK helper used by pose_fingers.inl
#include "../src/vrik/body_contact164.h"
#include "../src/vrik/hand_shape59c7f.h"
#include "../src/vrik/contact_occupancy_d52.h"
#include "../src/vrik/hand_pair_contact_d39.h"
static int mockItemState164R1[2]{};static std::uint64_t mockHeldKey164R1[2]{};
namespace kcd2vr::frik_pose_probe { int ExactHandItemState(int side) noexcept {return mockItemState164R1[side];} }
// This retained pose fixture has no native station/attachment observation.
// The production D52 reader is exercised by contact_occupancy_d52_test instead.
static kcd2vr::contact_occupancy_d52::Observation ReadContactOccupancyD52(void*,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {
    kcd2vr::contact_occupancy_d52::Observation o{};for(int side=0;side<2;++side)o.hand[side].state=mockItemState164R1[side];return o;
}
static void TraceContactOccupancyD52(const kcd2vr::contact_occupancy_d52::Observation&,std::uint64_t) noexcept {}
#include "../src/core/snapshot_mailbox.h"
// The real SHA gate is separately exercised by retail_hash112; this fixture
// has no retail module. Readiness is an explicit synthetic leaf, false unless a case requests it.
namespace retail_build112 {inline bool mockReady175=false;inline bool Ready(std::uintptr_t) noexcept {return mockReady175;}}
#define __try try
#define __except(x) catch(...)
struct QuatT28 {float q[4];float t[3];};
using PoseGetFn=const QuatT28*(*)(void*,int);using PoseSetFn=void(*)(void*,int,const QuatT28*);
#include "generated_adapter_types.h"
struct FingerChainV6528{std::array<int,6>id{};int count{};};
#ifndef KCD2VR_POSE_FIXTURE_COUNT
#define KCD2VR_POSE_FIXTURE_COUNT 48
#endif
constexpr int Count=KCD2VR_POSE_FIXTURE_COUNT;
struct Fixture{std::array<QuatT28,Count> original{},reference{},local{},live{};std::array<int,Count> parent{};};
static Fixture fixture;
static std::array<unsigned char,Count*0x30> nativeJointRecords;
static int failAt=-1,writeCalls=0;static bool failOnce=false;
static std::vector<std::string> logs;
namespace diagnostic_trace {void Write(const char*m){logs.emplace_back(m);}}
namespace animation_filter { inline bool mockNativeKickActive175=false;
inline bool NativeKickActive175() noexcept { return mockNativeKickActive175; } // Native playback leaf; exact alias/FIFO ownership is exercised by the full animation bridge.
inline std::atomic<bool> nativeBodyOwned112{false};
inline std::atomic<std::uint64_t> combatPoseTick112{0};struct PoseReferenceState{bool closed[2]{},relaxed[2]{};std::uint64_t serial[2]{};};PoseReferenceState refs{};bool referenceAvailable=false;
bool ReadPoseReferenceState(void*,PoseReferenceState& out){out=refs;return referenceAvailable;}}
namespace animation_filter {
struct CurveReferenceInfo {int id=-1;std::uint32_t flags=0;std::uint64_t alias=0;};
static bool curveAvailable=false;static int curveSamples=0,curveStatus=0;
bool ResolveCurveReference(void*,const char* n,CurveReferenceInfo&o){if(!curveAvailable)return false;
 if(std::strcmp(n,"relaxed_idle_both_player")==0)o.id=0;
 else if(std::strcmp(n,"combat_lg_sz2_idle_nwpn_player")==0)o.id=1;
 else if(std::strcmp(n,"combat_rg_sz1_idle_nwpn_player")==0)o.id=2;
 else if(std::strcmp(n,"combat_attack_lg_z2_nwpn_add")==0)o.id=3;else o.id=4;
 o.flags=o.id>=3?1:0;o.alias=o.id+100;return true;}
int SampleCurveReference(void*,int id,std::uint32_t,float phase,float*out){++curveSamples;if(curveStatus)return curveStatus;
 for(int i=0;i<7;++i)out[i]=0;const float a=id>=3?phase*.8f:0;out[2]=std::sin(a*.5f);out[3]=std::cos(a*.5f);return 0;}
}
static bool Safe(const void* p,std::size_t=1){return p!=nullptr;}
template<class T>bool Read(const void*p,T&out){
#ifdef KCD2VR_POSE_FIXTURE_READ_LEAF
 if(KCD2VR_POSE_FIXTURE_READ_LEAF(p,&out,sizeof(out)))return true;
#endif
 if(!p)return false;if(p==reinterpret_cast<const unsigned char*>(&fixture)+8&&sizeof(T)==sizeof(void*)){void* j=nativeJointRecords.data();std::memcpy(&out,&j,sizeof(j));return true;}std::memcpy(&out,p,sizeof(out));return true;}
static bool ReadParentDirect(void*,int j,int&p){p=fixture.parent.at(j);return true;}
static bool ReadCompanion(void*,int j,QuatT28&out){out=fixture.reference.at(j);return true;}
static bool mockFingerMapSwapH34=false; // native mapping leaf, default retained fixture unchanged
static bool GetCachedFingerChainsV6546(void*,PoseGetFn,void*,std::uint32_t,const RigMap&,bool left,std::array<FingerChainV6528,5>&out){
 for(int c=0;c<5;++c){out[c].count=3;for(int j=0;j<3;++j)out[c].id[j]=(left?30:15)+c*3+j;}
 if(mockFingerMapSwapH34)std::swap(out[0],out[1]);return true;}
static const QuatT28* MockGet(void*,int j){return &fixture.live.at(j);}
static void MockSet(void*,int j,const QuatT28*q){fixture.live.at(j)=*q;}
static bool ValidateBuffer1Setter(void*,PoseSetFn&set){set=MockSet;return true;}
static bool WriteQuat(PoseSetFn set,void* pose,int j,const QuatT28&value){++writeCalls;set(pose,j,&value);if(j==failAt&&!failOnce){failOnce=true;return false;}return true;}
static std::uintptr_t WhBase(){return 0;}
struct ModelWorld6692{Q4 q{};V3 t{};float scale=1;};
static void (*mockModelWorldHook55)()=nullptr;
static bool ReadModelWorld6692(void* character,ModelWorld6692& out) noexcept {
    float v[8]{};if(!character||!Read(static_cast<unsigned char*>(character)+0xA08,v))return false;
    double n=0;for(float f:v)if(!std::isfinite(f))return false;for(int i=0;i<4;++i)n+=double(v[i])*v[i];
    if(n<.8||n>1.2||v[7]<.1f||v[7]>10)return false;
    out.q=QNormalize({v[0],v[1],v[2],v[3]});out.t={v[4],v[5],v[6]};out.scale=v[7];
    if(mockModelWorldHook55)mockModelWorldHook55();
    return true;
}
static V3 ModelToWorld6692(const ModelWorld6692& x,V3 p) noexcept{return Add(x.t,QRotateV(x.q,Mul(p,x.scale)));}
static V3 WorldToModel6692(const ModelWorld6692& x,V3 p) noexcept{return Mul(QRotateV(QConj(x.q),Sub(p,x.t)),1.0f/x.scale);}


static bool mockCollision=false;static float mockWallX=.45f;
static bool ContactSuspended59c1() noexcept {return false;} // modeled native circuit state; actual circuit covered by world_adapter
static bool ContactFaultH30() noexcept {return false;}
static bool mockSharedBudget124=false;static int handQueries124=0,weaponQueries124=0;
static bool CollisionBinding6692(std::uintptr_t){return mockCollision;}
static bool ConstrainHandWorld6692(void*,V3,V3 requested,V3& allowed,float& hit){
 allowed=requested;hit=0;if(!mockCollision)return false;if(requested.x>mockWallX){allowed.x=mockWallX;hit=.1f;}return true;}
static bool SegmentBlockedWorld6692(void*,V3,V3,float,float& hit){hit=0;return false;}
static void CollisionStatus6692(std::uint64_t,const char*){}
#include "../src/vrik/hand_contact_types.inl"
static bool g_contact_pass6695=false,g_collision_fan_valid6693=true;
static struct {bool requested=false;} g_npc_query172;
struct ContactPass6695{ContactPass6695(bool enabled,std::uint64_t now,std::uint64_t&last,std::uint64_t=0){g_contact_pass6695=enabled;last=now;}~ContactPass6695(){g_contact_pass6695=false;}};
static void (*mockContactHookH50)()=nullptr; // Optional native query-leaf order witness.
static bool ContactHand6695(HandContact6695&,void* c,std::uint64_t,V3 native,V3 request,V3&allowed,const V3* =nullptr,const FingerContactGeometry103* =nullptr,HandContactPending6695* pending=nullptr){if(pending)pending->ready=false;if(mockContactHookH50)mockContactHookH50();if(mockSharedBudget124)handQueries124+=9;float hit=0;return ConstrainHandWorld6692(c,native,request,allowed,hit);}
// This pose-only fixture keeps its existing immediate native-collision leaf.
// The source-connected world adapter executes and tests real deferred work.
static bool CompleteHandContact6695(HandContact6695&,HandContactPending6695&,V3&){std::abort();}

static void ContactCommit6695(HandContact6695&,void*,V3){}

static std::atomic<bool>g_body_cal_ready{false},g_pose_restore_fault6689{false},g_committed_pose_valid6689{false};
static std::atomic<std::uint64_t>g_contact_control_epoch6695{1};
static std::atomic<std::uint64_t>g_pose_epoch6689{1};static std::atomic<int>g_cal_state{2};
// C37I1: synthetic native identity ledger leaves for the actual packet producer.
static std::atomic<std::uint64_t>g_player_publication37{0},g_player_lease37{0};
static std::atomic<std::uintptr_t>g_player_char37{0};
static std::atomic<void*>g_last_player_entity{nullptr};
static bool mockDoorIdentityChanges38=false;
static std::atomic<float> g_cal_frame_qx{0},g_cal_frame_qy{0},g_cal_frame_qz{0},g_cal_frame_qw{1},g_cal_hq_x{0},g_cal_hq_y{0},g_cal_hq_z{0},g_cal_hq_w{1};
static std::atomic<float>g_cal_native_head_x{0},g_cal_native_head_y{0},g_cal_native_head_z{0},g_cal_head_x{0},g_cal_head_y{0},g_cal_head_z{0};
static bool ControllerHeadAnchoredArmGoal(const TrackingSnapshot&,const QuatT28&,bool,V3&){return true;}
static std::atomic_flag g_f12_hand_delta_lock=ATOMIC_FLAG_INIT;
static float g_committed_model_world6689[8]{};static QuatT28 g_f12_native_hand[2]{},g_f12_vr_hand[2]{};static bool g_f12_hand_delta_valid[2]{};
static kcd2vr::pose_core::Stamp g_committed_stamp6689;static std::atomic<void*>g_committed_character6689{nullptr};
struct CommittedFrame6695{float mapGripM39[2]{};bool mapFoldTrackedM39=false;
 std::uint32_t mapPresentationOwnerM34[2]{~0u,~0u};kcd2vr::pose_core::Stamp stamp;QuatT28 native[2],allowed[2],propRequested5916c1[2]{};float world[8];bool valid[2],propRequestedValid5916c1[2]{};
 V3 palm59c9[2]{},palmDirection59c9[2]{};bool palmValid59c9[2]{};V3 nativeHead122{},solvedHead122{},targetHead122{},pelvis122{};bool headEvidence122=false;std::uintptr_t doorPlayer38=0;std::uint64_t doorTrackingEpoch66=0;
 std::uintptr_t fistSkeletonH31=0;std::uint32_t fistJointCountH31=0;int fistWristBoneH31[2]{-1,-1},fistSlotBoneH31[2]{-1,-1};QuatT28 fistSlotH31[2]{};bool fistSlotValidH31[2]{};};
static kcd2vr::core::SnapshotMailbox<CommittedFrame6695> g_committed_frame6695;
static void BuildCrossbowPosesXR34(CommittedFrame6695&,const QuatT28*,std::size_t) noexcept {} // external native tool-placement leaf
static void BuildToolPoses59c10(CommittedFrame6695&,const QuatT28*,std::size_t,
                              const kcd2vr::grasp13::FingerReference55* =nullptr) noexcept {
 // A native leaf changing identity after the producer's entry snapshot must
 // invalidate only the door metadata, without discarding the pose publication.
 if(mockDoorIdentityChanges38)g_player_publication37.fetch_add(2);
}
// Door-surface publication is an external native scene-query leaf. This pose
// fixture keeps it explicitly OFF; the door service fixture covers its writes.
static void PublishDoorSurface65(const CommittedFrame6695&,std::uint64_t) noexcept {}
static void PublishGuidedTools55(const CommittedFrame6695&,std::uint64_t) noexcept {} // exact native attachment producer tested by native_activity_hands46
static bool NativeSmithFingers36(std::uintptr_t,std::uint64_t,int,std::uint64_t) noexcept {return false;} // C37 leaf, tested by tool_attachment59c10/smith_tool36
static bool NativeAlchemyFingers18(std::uintptr_t,std::uint64_t,int,std::uint64_t) noexcept {return false;} // native tool ownership leaf, tested by tool_attachment59c10
// The native equipment/contact/publication edges are mocked. The physical
// kernel and its actual coherent-pose callsite are not replaced.
#include "../src/vrik/physical_weapon.h"
#include "../src/vrik/own_shield_contact58.h"
#include "../src/core/haptics.h"
#include "../src/diagnostics/combat_capture.h"
#include "../src/diagnostics/receipt_budget56.h"
namespace cc=kcd2vr::diagnostics::combat_capture;
namespace ph=kcd2vr::physical;
// Native ammo presentation is independently exercised in manual_arrow64_test.
static void BuildManualArrowPoses64(CommittedFrame6695&,const QuatT28*,std::size_t,const ph::Input&,const ph::Output&,const ph::Profile&,const QuatT28*,int) noexcept {}
// F11 is OFF in this surrounding coherent-pose fixture. It cannot fabricate a
// native nock acknowledgment; the actual producer/kernel join has its own test.
static void ManualNockInput19c4r4(ph::Input& in,const ph::Profile&,std::uint64_t) noexcept {in.manualNockAck19c4r4={};}
static kcd2vr::core::SnapshotMailbox<ph::RejectedBowRelease166> g_bow_release_rejected166;
// Copied POD shapes of the native equipment/socket publication boundary.
// Empty/default values deliberately provide NO ownership or contact witness;
// cases below must explicitly supply provenance before any scoped helper can act.
struct PhysEquipmentItem6698 {
 ph::Profile profile{};void* item=nullptr;void* entity=nullptr;float bounds[6]{};
 void* worldLink54=nullptr;
 int set=-1,slot126=-1;bool boundsValid=false,held=false;
 bool slotValid34[2]{};std::array<ph::Pose,2> slot34{};std::uint64_t slotRetry34=0;
};
struct PhysEquipment6698 {
 std::array<PhysEquipmentItem6698,8> items{};int count=0,selected=-1,heldIndex[2]{-1,-1};
 void* player=nullptr;void* manager=nullptr;std::uint64_t request=0,tick=0,epoch=0,handKey[2]{};
 bool valid=false,handKnown[2]{};
};
struct PhysSocket101 {
 std::uint64_t key=0,generation=0,tick=0,callback=0;void* character=nullptr;int joint=-1;
 QuatT28 relative{};float scale[3]{1,1,1};bool valid=false;
};
struct PhysCorrelation126 {kcd2vr::pose_core::Stamp stamp{};PhysSocket101 socket{};};
static ph::State g_phys_state6698;static std::array<ph::HolsterState,8> g_phys_holsters6698;
namespace heldT4=kcd2vr::native_weapon_body_t4;
struct NativeHeldSourceT4 {heldT4::Input input{};bool valid=false;};
static kcd2vr::core::SnapshotMailbox<NativeHeldSourceT4> g_native_held_source_t4;
static std::atomic<std::uintptr_t> g_native_held_attachment_self_t4{0},g_native_held_attachment_t4{0};
static std::atomic<bool> g_native_held_release_inhibit_t4{false};
static bool NativeHeldSourceCurrentT4(const heldT4::Input&) noexcept {return false;}
static bool NativeHeldBusyT4(int=-1) noexcept {return false;}
static void PhysRetireMotion30c1(const char* =nullptr,int =0) noexcept {g_phys_state6698={};}
static std::atomic<std::uint64_t> g_physical_gates101R2[10]{};
static std::atomic<bool> g_gameplay_allowed{true};
static std::atomic<bool> g_ctx_menu{false};
static std::atomic<bool> g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_ctx_reading{false};
static std::atomic<std::uint64_t> g_tool_context19c4r2{0};
static std::atomic<std::uint64_t> g_pose_policy_generation57{2};
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1}; // synthetic request leaf; full frik bridge exercises changes
static const char* WorkstationContextRefusal19c4r2(std::uint64_t now,bool requireFresh) noexcept {
    return kcd2vr::presentation_context55::refusal(g_tool_context19c4r2.load(std::memory_order_acquire),now,requireFresh);
}
static std::atomic<int>g_weapon_control_mode{0};static std::atomic<bool>g_phys_abort6698{false};
static std::array<ph::Profile,2> mockProfiles{};static bool mockPhysical=false;static int physicalPublications=0;
static ph::Output mockPhysicalOutput{};static ph::Input mockPhysicalInput175{};
static void PhysInputWorld6698(ph::Input&,void*){}
static bool PhysPrepare6698(ph::Input& i,std::array<ph::Profile,2>& p,PhysEquipment6698&,std::array<PhysCorrelation126,2>*){p=mockProfiles;return mockPhysical;}
static unsigned mockSweepCalls55=0;static bool mockSweepBudget55=false;
static ph::Contact103 PhysSweep6698(void*,const ph::Profile&,ph::Pose,ph::Pose,bool){
 ++mockSweepCalls55;mockSweepBudget55=g_contact_pass6695;ph::Contact103 c{};if(mockSharedBudget124){weaponQueries124=handQueries124+5;if(weaponQueries124>18){c.known=false;c.fraction=0;}}return c;}
static void PhysHolsterGestures6698(const ph::Input&,const PhysEquipment6698&,ph::Output&,std::array<ph::HolsterState,8>&,unsigned=0){}
static void PhysPublish6698(const ph::Input& input,const ph::Output& o,const std::array<ph::Profile,2>&,const PhysEquipment6698&,const float*,ph::State& s,std::array<ph::HolsterState,8>&,const std::array<PhysCorrelation126,2>*,std::uint64_t=0,std::uintptr_t=0,std::uint64_t=0){++physicalPublications;mockPhysicalInput175=input;mockPhysicalOutput=o;g_phys_state6698=s;}
#include "calibration_pose_shim.h"
static void PublishPoseContext55(const kcd2vr::frik_pose_probe::GameStateContext& state) noexcept {
#include "../src/vrik/workstation_pose_context19c4r2_publish.inl"
}
static void PhysCaptureSamePose101(void*,const QuatT28*,std::uint32_t,const RigMap&,const TrackingSnapshot&,const QuatT28*,const int*) noexcept {}
static void FistPublish101(const TrackingSnapshot&,const CommittedFrame6695&,const bool*) noexcept {}
static bool NativeHeldFingerKey101(int side,std::uint64_t& key) noexcept {key=mockHeldKey164R1[side];return key!=0;}
// V66.137: archer stance (V127) reads the published physical frame; this fixture has none.
struct PhysFrame6698{ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};};
struct PhysFrameMailboxStub{bool read(PhysFrame6698&)const noexcept{return false;}};static PhysFrameMailboxStub g_phys_frame6698;
// V66.146R1 globals owned by physical_native.inl (not linked into this fixture): simplified RT bow, default zone.
// Surrounding production settings added by R27-R29; the isolated pose TU must declare them.
static std::atomic<bool> g_combat_experiment175{false},g_two_hand_v120_178r27{true},g_two_hand_constraint_v120_178r29{false},g_directional_stabs178r29{false},g_thrust_as_swing55{true},g_windup_hold57{false},g_telegraph_windup59c10{false},g_wall_slide59c10{true},g_contact_starts_attack178r41{false},g_attack_at_prepare178r42{false};static std::atomic<float> g_weapon_authority_scale178r44{1.0f};static std::atomic<std::uint64_t> g_kick_request59{0}; // R59
static std::atomic<float> g_guard_enter_speed178r29{1.2f};static std::atomic<unsigned> g_guard_enter_dwell178r29{80};static std::atomic<float> g_guard_exit_speed178r45{3.5f};static std::atomic<unsigned> g_guard_after_attack_ms49{400};
static std::atomic<bool> g_guard_strict59c17{true};static std::atomic<float> g_weapon_stat_effect59c17{1.f}; // R59.16C10R17
static std::atomic<bool> g_bow_manual_reload146{false},g_bow_native_grab146{false},g_bow_string_trigger160{false},g_experiments169{false},g_combat_integrated178r22c2{false};static std::atomic<float> g_holster_radius136{.28f};
// Native ray leaf only; the actual coherent kick lifecycle/caller remains production.
static unsigned mockKickQueries176=0;
static kcd2vr::kick176::Contact mockKickContact176{};
static kcd2vr::pose_core::Vec3 mockKickFrom176{},mockKickTo176{};
static void (*mockKickQueryHook176)()=nullptr;
static kcd2vr::kick176::Contact ProbeKickContact176(void*,kcd2vr::pose_core::Vec3 from,kcd2vr::pose_core::Vec3 to,int* =nullptr,std::uintptr_t* =nullptr,float=0.f,float* =nullptr) noexcept { // H8 optional current-sweep assist leaves
    ++mockKickQueries176;mockKickFrom176=from;mockKickTo176=to;
    if(mockKickQueryHook176)mockKickQueryHook176();
    return mockKickContact176;
}
static std::atomic<bool> g_native_busy178{false};static std::atomic<std::uint64_t> g_native_busy_tick178{0};static std::atomic<float> g_strike_min_scale178{.35f};static std::atomic<unsigned> g_strike_min_interval178{550}; // V66.178
static std::atomic<float> g_fatigue_ramp59c14{.6f},g_pole_one_hand59c14{.40f};static std::atomic<bool> g_strike_min_scale_gate59c14{false}; // R59.16C10R14
static std::atomic<bool> g_intent_pi59c16{false},g_two_hand_axis59c16{false}; // R59.16C10R16
// R20C1: complete the surrounding state introduced in R4/R14/R15/R18.
// These native publication leaves are neutral here; the coherent pose code is real.
static std::atomic<bool> g_native_state_ok178{false},g_strike_block_active178{true};
static std::atomic<std::uint64_t> g_native_state_epoch178{0},g_phys_ready_sample6698{0};
static std::atomic<bool> g_hover_affordance178r14[2]{},g_native_handover178r14{true},g_impale_enabled178r15{true};
struct ImpalePin178r15 {bool valid=false;std::uint64_t serial=0,tick=0;float x=0,y=0,z=0;int mode40=0;float depth40=0;}; // C40 mirrors frik_pose_probe.cpp
static kcd2vr::core::SnapshotMailbox<ImpalePin178r15> g_impale_pin178r15;
static int F4Rung178r10() noexcept {return 0;}
static std::atomic<float> g_wrist_from_grip59c7f{0.f}; // R59.16C7F host
static std::atomic<bool> g_auto_palm59c8{false};static std::atomic<float> g_head_raise_max59c9{.05f};static std::atomic<float> g_auto_palm_last_cm59c8[2]{0.f,0.f}; // R59.16C8 host (off: these cases assert the wrist goal == the tracked point)
// C10R13: explicit neutral native-grasp publication leaves. Existing pose and
// kick cases keep these null; native-grasp orchestration/placement is covered
// by native_grasp_owner13 and tool_attachment59c10 using the production code.
#include "../src/core/native_grasp13.h"
static bool (*g_graspEnabled13)() noexcept=nullptr;
static bool (*g_graspBinding13)(kcd2vr::grasp13::Binding&) noexcept=nullptr;
static void (*g_graspHands13)(const kcd2vr::grasp13::HandFrame&) noexcept=nullptr;
// C23: guard-pump publication leaves owned by native_combat104.inl; neutral here (no published window).
#include "../src/vrik/native_master_window23.h"
static std::atomic<bool> g_master_handoff_enabled23{true};static std::atomic<float> g_master_handoff_speed23{-1.f};
static kcd2vr::core::SnapshotMailbox<kcd2vr::native_master23::Published> g_master_published23;
// Current production API dependencies for this retained native-services-OFF fixture.
// Types/helpers are real; no fist owner, item service or engine hand claim is invented.
#include "../src/subsystems/doors/hand_only_h40.h"
#include "../src/subsystems/physics/grab_over_talk_ds.h"
struct FistOwner101 {void* player=nullptr;void* manager=nullptr;std::uint64_t tick=0,epoch=0,request=0;bool empty=false,responsiveCombatH27=false;};
static kcd2vr::core::SnapshotMailbox<FistOwner101> g_fist_owner101;
static std::atomic<std::uint64_t> g_fist_epoch101{1};
static bool FistSelected101() noexcept{return false;}
static bool FistModePermits178r22c2() noexcept{return false;}
static bool HandClaimBlockedT24(int,std::uintptr_t,std::uint64_t,std::uint64_t) noexcept{return false;}
static double ChestPitchShareCB() noexcept{return 0.;}
static bool ArcherStanceCB() noexcept{return false;}
static kcd2vr::core::SnapshotMailbox<PhysEquipment6698> g_phys_equipment6698;
static QuatT28 HandWorld6694(const QuatT28& pose,const float*) noexcept{return pose;} // Door service stays OFF.
static int ExactHandItemState(int side) noexcept{return kcd2vr::frik_pose_probe::ExactHandItemState(side);}
static int ReadPhysicalHandHeld55(int,std::uint64_t) noexcept{return 0;}
#include "../src/vrik/coherent_pose.inl"
// Actual accepted-native-animation producer, distinct from an attached tool's
// short finger/contact refresh. The remaining controls are neutral storage.
static std::atomic<unsigned> g_page_memo_ms178r7{};
static std::atomic<std::uint64_t> g_read_faults178r7{};
#include "../src/subsystems/interactions/native_hand_use_control.inl"
static int tests=0,failures=0;
static void Check(bool p,const char*n){++tests;if(!p)++failures;std::printf("%s %s\n",p?"PASS":"FAIL",n);}
static RigMap rig;
static std::array<unsigned char,0xA40>character{};static std::array<std::uintptr_t,100>vtable{};
static bool Same(const std::array<QuatT28,Count>&a,const std::array<QuatT28,Count>&b){return std::memcmp(a.data(),b.data(),sizeof(a))==0;}
static void Init(){
 g_player_publication37=0;g_player_lease37=0;g_player_char37=0;g_last_player_entity=nullptr;mockDoorIdentityChanges38=false;
 retail_build112::mockReady175=false;kcd2vr::frik_pose_probe::mockCombatExperiment175=false;mockPhysicalInput175={};
 for(int side=0;side<2;++side){mockItemState164R1[side]=0;mockHeldKey164R1[side]=0;g_native_use_until107[side]=0;g_native_wrist_until107[side]=0;g_native_wrist_retired107[side]=0;}
 g_cal_frame_qx=g_cal_frame_qy=g_cal_frame_qz=0;g_cal_frame_qw=1;
 g_cal_hq_x=g_cal_hq_y=g_cal_hq_z=0;g_cal_hq_w=1;
 g_cal_head_x=g_cal_head_y=g_cal_head_z=0;
 g_finger_capture66100=false;g_finger_saved_generation66100=0;g_capture_runtime66100={};
 g_arm_profile66100.publish(kcd2vr::calibration_profile::Arms{}); // default: no valid retarget profile
 g_gameplay_allowed=true;g_weapon_control_mode=0;g_phys_abort6698=false;g_phys_state6698={};g_phys_holsters6698={};mockProfiles={};mockPhysical=false;physicalPublications=0;
 for(int j=0;j<Count;++j){std::uint32_t crc=j+1;std::memcpy(nativeJointRecords.data()+j*0x30+8,&crc,4);}
 fixture={};rig={};rig.pelvis=0;rig.spine=1;rig.spine1=2;rig.spine2=3;rig.spine3=4;rig.neck=5;rig.head=6;
 rig.clav_r=7;rig.upper_r=8;rig.fore_r=9;rig.hand_r=10;rig.elbow_r=9;rig.clav_l=11;rig.upper_l=12;rig.fore_l=13;rig.hand_l=14;rig.elbow_l=13;
 fixture.parent.fill(-1);for(auto&q:fixture.local)q.q[3]=1;
 fixture.local[0].t[2]=.8f;for(int i=1;i<=6;++i){fixture.parent[i]=i-1;fixture.local[i].t[2]=.14f;}
 for(int side=0;side<2;++side){const float sign=side?-1.f:1.f;const int c=side?11:7;
  fixture.parent[c]=4;fixture.local[c].t[0]=sign*.06f;
  for(int j=1;j<=3;++j){fixture.parent[c+j]=c+j-1;fixture.local[c+j].t[0]=sign*(j==1?.12f:.25f);}
  for(int finger=0;finger<5;++finger)for(int joint=0;joint<3;++joint){const int id=(side?30:15)+finger*3+joint;fixture.parent[id]=joint?id-1:c+3;fixture.local[id].t[0]=.03f*sign;if(!joint)fixture.local[id].t[1]=(2-finger)*.015f;}
 }
 fixture.parent[45]=4;fixture.local[45].t[1]=.04f;fixture.parent[46]=6;fixture.local[46].t[2]=.03f;
 fixture.parent[47]=0;fixture.local[47].t[2]=-.4f;
 for(int i=0;i<Count;++i)fixture.reference[i]=fixture.parent[i]<0?fixture.local[i]:AbsoluteFromRelative(fixture.reference[fixture.parent[i]],fixture.local[i]);
 fixture.original=fixture.live=fixture.reference;g_cal_native_head_z=fixture.reference[6].t[2];
 auto*vf=vtable.data();vtable[0x190/8]=0x423C30;std::memcpy(character.data(),&vf,sizeof(vf));const float world[8]{0,0,0,1,100,200,10,1};std::memcpy(character.data()+0xA08,world,sizeof(world));
 g_pose_policy_generation57=2;g_pose6689.~CoherentState6689(); new (&g_pose6689) CoherentState6689;g_cal_state=2;g_pose_epoch6689=1;g_pose_restore_fault6689=false;g_committed_pose_valid6689=false;failAt=-1;failOnce=false;writeCalls=0;
 animation_filter::referenceAvailable=false;animation_filter::curveAvailable=false;animation_filter::curveSamples=0;animation_filter::curveStatus=0;g_finger_acquire6694={};g_finger_loaded6694=0;g_finger_save_pending6694=false;mockCollision=false;mockWallX=.45f;
}
static TrackingSnapshot Tracking(){TrackingSnapshot t{};t.tick=10000;t.sequence=2;t.allowed=t.head_valid=t.left_valid=t.right_valid=true;
 // cry right=(.38,.22,1.20), left=(-.38,.22,1.20); headAtCalibration=1.64.
 const float z=1.20f-fixture.reference[6].t[2];t.right={.38f,z,-.22f};t.left={-.38f,z,-.22f};return t;}
static bool Run(TrackingSnapshot t,int rung=3,bool publish=false){CoherentTransaction6689 tx;bool ok=BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,rung,t);tx.Restore();if(publish)PublishCommitted6689(tx);return ok;}
static void NativeWristProvenanceChecks107(){
 // Replay the recorded ownership transition, not an animation or native action
 // emulator: the real grasp/terminal service is an explicit owner-read leaf.
 // The tool producer's exact now+80 refresh is separately executed by the
 // retained tool_attachment59c10 fixture; here it is the copied input boundary.
 static bool controllerOwned=false;
 const auto run=[&](TrackingSnapshot t,int rung){CoherentTransaction6689 tx;const bool ok=BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,rung,t);if(!ok)std::printf("PROVENANCE_FIXTURE_REFUSAL stage=%s line=%d\n",tx.stage,tx.failureLine);tx.Restore();return ok;};
 for(int side=0;side<2;++side)for(int earlyAction=0;earlyAction<2;++earlyAction){
  Init();auto t=Tracking();t.timeNs=t.tick*1000000ull;g_native_handover178r14=true;mockCollision=true;mockWallX=10;mockSharedBudget124=true;
  g_wristOwnedByController59c12=+[](int)noexcept{return controllerOwned;};controllerOwned=true;
  const int h=side?rig.hand_l:rig.hand_r;const float sign=side?-1.f:1.f;
  const V3 requested{sign*.077f,.075f,1.30f};const auto offset=Sub(requested,FromT(fixture.reference[rig.head]));
  (side?t.left:t.right)={offset.x,offset.z,-offset.y};
  Check(run(t,4),"post-tool wrist provenance reference frame commits");
  // Preserve established anatomical lengths while supplying the native idle
  // wrist observed after actual inventory commitment in the frozen F7 run.
  ToT(fixture.original[h],{sign*.246f,.043f,.855f});fixture.live=fixture.original;
  if(earlyAction){NotifyNativeHandUse(side,1800);
   // Copied successful-placement provenance; the actual ToolAttachment producer
   // and a newer Notify during placement run in tool_attachment59c10_test.
   g_native_wrist_retired107[side]=g_native_use_serial107[side].load();}
  g_native_use_until107[side]=t.tick+80;const auto originalDeadline=g_native_use_until107[side].load();
  const auto originalSerial=g_native_use_serial107[side].load();
  ++t.sequence;t.tick+=11;t.timeNs=t.tick*1000000ull;
  Check(run(t,4)&&g_pose6689.handoverW178r14[side]==0.f,"current controller-held item keeps wrist ownership before native terminal");
  const auto before=FromT(g_pose6689.work[h]);controllerOwned=false;handQueries124=0;
  ++t.sequence;t.tick+=11;t.timeNs=t.tick*1000000ull;
  Check(run(t,4),"completed native item terminal does not cancel the coherent pose");
  const auto after=FromT(g_pose6689.work[h]);
  std::printf("POST_TOOL_WRIST side=%d deltaM=%.9f handover=%.9f\n",side,Len(Sub(after,before)),g_pose6689.handoverW178r14[side]);
  Check(Len(Sub(after,before))<.00001f&&g_pose6689.handoverW178r14[side]==0.f,
        "tool-only finger/contact lease cannot reacquire tracked wrist after native inventory terminal");
  Check(g_native_use_until107[side]==originalDeadline&&g_native_use_serial107[side]==originalSerial&&handQueries124==9,
        "tool finger/contact deadline and serial remain unchanged while opposite hand retains contact");
  bool hard=true;for(int k=0;k<8;++k){++t.sequence;t.tick+=11;t.timeNs=t.tick*1000000ull;hard=hard&&run(t,4);
   const auto& s=g_pose6689;const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r;
   hard=hard&&std::abs(Len(Sub(FromT(s.work[f]),FromT(s.work[u])))-s.upper[side])<1e-6&&
    std::abs(Len(Sub(FromT(s.work[h]),FromT(s.work[f])))-s.fore[side])<1e-6&&
    Len(Sub(FromT(s.work[h]),before))<.00001f&&Same(fixture.live,fixture.original);}
  Check(hard,"post-tool wrist remains current through expiry with fixed lengths and native buffer restoration");
  const auto retiredBefore=g_native_wrist_retired107[side].load();NotifyNativeHandUse(side,1800);
  ++t.sequence;t.tick+=11;t.timeNs=t.tick*1000000ull;
  Check(run(t,4)&&g_pose6689.handoverW178r14[side]>0.f&&
        g_native_wrist_retired107[side]==retiredBefore&&g_native_use_serial107[side]>retiredBefore,
        "later accepted native animation reacquires wrist in same session after controller handoff retired earlier serial");
  g_wristOwnedByController59c12=nullptr;
  Init();t=Tracking();NotifyNativeHandUse(side,1800);
  Check(g_native_use_until107[side]==std::uint64_t(GetTickCount64())+1800&&
        g_native_wrist_until107[side]==g_native_use_until107[side],
        "actual accepted native animation producer publishes distinct matching wrist deadline");
  ++t.sequence;t.tick+=11;
  Check(run(t,4)&&g_pose6689.handoverW178r14[side]>0.f,
        "genuine accepted native animation retains existing wrist blend on its own hand");
 }
 Init();g_wristOwnedByController59c12=nullptr;
 {
  auto t=Tracking();namespace ai=kcd2vr::activity_input163;namespace mh=kcd2vr::minigame_hands45;
  const auto savedMode=mh::mode.load();const bool wasReady=ai::witness175.ready.load();
  mh::mode=mh::SemiPassenger;mh::publish(mh::Smithing,GetTickCount64());ai::witness175.ready=true;
  ai::witness175.Begin(ai::Kind175::Smithing,0x107);kcd2vr::combat_rules178r10::workstationEnabled=true;
  g_native_handover178r14=false;
  Check(run(t,4)&&g_pose6689.handoverW178r14[0]==1.f&&g_pose6689.handoverW178r14[1]==1.f,
        "actual native station witness retains both passenger wrists without any generic animation deadline");
  ai::witness175.End(0x107);ai::witness175.ready=wasReady;mh::mode=savedMode;mh::witness=0;
  g_native_handover178r14=true;Init();t=Tracking();const auto native=fixture.live;
  animation_filter::nativeBodyOwned112=true;
  Check(!run(t,4)&&Same(native,fixture.live),
        "existing native paired-body ownership remains a native pose no-op, not a tracked wrist takeover");
  animation_filter::nativeBodyOwned112=false;
 }
 Init();g_wristOwnedByController59c12=nullptr;
}
#ifndef KCD2VR_F7_TRIAL_HOST
#define KCD2VR_F7_TRIAL_HOST 1
#endif
static void F7NativeCurveTrialChecksH7(){
#if KCD2VR_F7_TRIAL_HOST
 namespace cap=kcd2vr::diagnostics::capture168;
 cap::gate.select(cap::Domain::Vrik,false,10000);
 Init();auto t=Tracking();const auto nowH7=std::uint64_t(GetTickCount64());Check(Run(t,3),"F7 reference trial begins with normal coherent body");
 const auto unavailableDefault=g_pose6689.work;
 cap::gate.cycleVrik(nowH7);cap::gate.cycleVrik(nowH7);
 Check(Run(t,3)&&!g_pose6689.nativeCurveClosedH7[0].ready&&!g_pose6689.nativeCurveClosedH7[1].ready,"actual F7 trial with unavailable asset reference never blocks the existing pose");
 Check(std::memcmp(unavailableDefault.data(),g_pose6689.work.data(),sizeof(unavailableDefault))==0,"unavailable F7 native reference transparently retains exact default body and fingers");
 cap::gate.select(cap::Domain::Vrik,false,nowH7);
 Init();Check(Run(t,3),"native decoder setup preserves ordinary body owner");
 auto& s=g_pose6689;animation_filter::curveAvailable=true;
 for(int n=0;n<100;++n)AcquireCurveReferences6694(s,reinterpret_cast<void*>(1),20000+n*12);
 Check(s.nativeCurveClosedH7[0].ready&&s.nativeCurveClosedH7[1].ready,"actual bounded native decoder populates independent mapped terminal bank");
 const auto actualNativeR=s.nativeCurveClosedH7[0].local,actualNativeL=s.nativeCurveClosedH7[1].local;
 for(int side=0;side<2;++side)for(int n=0;n<s.fingerCount[side];++n){
  QToQuatT(s.closed[side].local[n],QNormalize({.2f,.1f,.3f,.9f}));
 }
 const auto defaultsR=s.closed[0].local,defaultsL=s.closed[1].local;
 const auto openR=s.open[0].local,openL=s.open[1].local;
 s.nativeCurveClosedH7[0]={};s.nativeCurveClosedH7[1]={};
 g_capture_runtime66100.owner=s.owner;g_capture_runtime66100.applied=9;
 g_finger_acquire6694.loadedCacheH7=true;g_finger_acquire6694.done=true;animation_filter::curveSamples=0;
 // A valid accepted cache distinct from the decoder catches an accidental
 // reference-only publisher even if it republishes identical rig/CRC metadata.
 for(int side=0;side<2;++side)for(int n=0;n<s.fingerCount[side];++n)g_finger_file6694.fist[side][n]=s.closed[side].local[n];
 g_finger_file6694.checksum=0;g_finger_file6694.checksum=FingerHash6694(&g_finger_file6694,sizeof(g_finger_file6694));
 g_finger_load_request6694.store(g_finger_acquire6694.rig^1ull);
 g_finger_save_pending6694=false;g_finger_audit_pending6697=false;
 const auto cacheBeforeTrialH7=g_finger_file6694;
 const auto loadBeforeTrialH7=g_finger_load_request6694.load(),loadedBeforeTrialH7=g_finger_loaded6694.load();
 const bool saveBeforeTrialH7=g_finger_save_pending6694,auditBeforeTrialH7=g_finger_audit_pending6697;
 for(int n=0;n<100;++n)AcquireCurveReferences6694(s,reinterpret_cast<void*>(1),24000+n*12,true);
 Check(animation_filter::curveSamples==450&&s.nativeCurveClosedH7[0].ready&&s.nativeCurveClosedH7[1].ready,"trial bypasses unproved BIN endpoint and executes actual native asset sampler");
 Check(std::memcmp(defaultsR.data(),s.closed[0].local.data(),sizeof(defaultsR))==0&&std::memcmp(defaultsL.data(),s.closed[1].local.data(),sizeof(defaultsL))==0&&
       std::memcmp(openR.data(),s.open[0].local.data(),sizeof(openR))==0&&std::memcmp(openL.data(),s.open[1].local.data(),sizeof(openL))==0,"native trial acquisition never overwrites accepted saved open/fist profile");
 Check(std::memcmp(actualNativeR.data(),s.nativeCurveClosedH7[0].local.data(),sizeof(actualNativeR))==0&&std::memcmp(actualNativeL.data(),s.nativeCurveClosedH7[1].local.data(),sizeof(actualNativeL))==0,"independent acquisition reproduces the existing actual side-mapped curve decoder output");
 Check(std::memcmp(&cacheBeforeTrialH7,&g_finger_file6694,sizeof(cacheBeforeTrialH7))==0&&
       g_finger_load_request6694.load()==loadBeforeTrialH7&&g_finger_loaded6694.load()==loadedBeforeTrialH7&&
       g_finger_save_pending6694==saveBeforeTrialH7&&g_finger_audit_pending6697==auditBeforeTrialH7&&!g_finger_acquire6694.pendingPublish,
       "trial-only asset decode queues no BIN load audit save or default-cache publication");
 t.right_grip=t.left_grip=t.right_trigger=t.left_trigger=.91f;
 Check(Run(t,3),"F7 OFF leaves default free finger presentation reachable");const auto defaultPose=s.work;
 const auto samplesBefore=animation_filter::curveSamples;
 cap::gate.cycleVrik(nowH7);Check(cap::gate.vrikRung(nowH7)==1&&Run(t,3),"F7 rung1 is diagnostics only");
 Check(std::memcmp(defaultPose.data(),s.work.data(),sizeof(defaultPose))==0&&animation_filter::curveSamples==samplesBefore,"diagnostics rung1 preserves byte-identical full coherent pose and adds no curve sampling");
 const auto wristR=s.work[rig.hand_r],wristL=s.work[rig.hand_l],head=s.work[rig.head];
 const auto until=cap::gate.until[unsigned(cap::Domain::Vrik)].load();
 cap::gate.cycleVrik(nowH7);Check(cap::gate.vrikRung(nowH7)==2&&Run(t,3),"F7 rung2 reaches actual coherent terminal trial");
 bool raw=true,offsets=true;for(int side=0;side<2;++side)for(int n=0;n<s.fingerCount[side];++n){const int id=s.fingerIds[side][n];
  raw=raw&&pc::angle(PQ(QFromQuatT(s.local[id])),PQ(QFromQuatT(s.nativeCurveClosedH7[side].local[n])))<2e-6;
  offsets=offsets&&std::memcmp(s.local[id].t,s.restLocal[id].t,sizeof(s.local[id].t))==0;}
 Check(raw&&offsets,"actual coherent91percent writer uses raw native curve endpoint and immutable translations");
 Check(std::memcmp(&wristR,&s.work[rig.hand_r],sizeof(wristR))==0&&std::memcmp(&wristL,&s.work[rig.hand_l],sizeof(wristL))==0&&std::memcmp(&head,&s.work[rig.head],sizeof(head))==0,"terminal trial changes no solved wrist endpoint/rotation or head transform");
 Check(Same(fixture.live,fixture.original),"native buffer is restored after every trial transaction");
 mockHeldKey164R1[0]=1234;Check(Run(t,3),"genuinely held hand keeps existing native finger owner during trial");
 bool held=true;for(int n=0;n<s.fingerCount[0];++n){const int id=s.fingerIds[0][n],parent=s.parent[id];
  const auto native=RelativeFromAbsolute(s.original[parent],s.original[id]);held=held&&pc::angle(PQ(QFromQuatT(s.local[id])),PQ(QFromQuatT(native)))<2e-6;}
 Check(held,"F7 trial cannot replace native held finger rotations with a free fist");mockHeldKey164R1[0]=0;
 animation_filter::nativeBodyOwned112=true;const auto before=fixture.live;const auto calls=writeCalls;Check(!Run(t,3)&&writeCalls==calls&&Same(before,fixture.live),"protected native paired body remains a zero-write owner under F7trial");animation_filter::nativeBodyOwned112=false;
 cap::gate.poll(until);Check(cap::gate.vrikRung(until)==0&&Run(t,3),"finite F7 lease expiry immediately restores ordinary recipe");
 Check(std::memcmp(defaultPose.data(),s.work.data(),sizeof(defaultPose))==0,"expired trial returns exactly to full default pose without resetting profile");
 cap::gate.select(cap::Domain::Vrik,false,nowH7);
 s.ready=false;Check(InitCoherent6689(s,&fixture,character.data(),&fixture,MockGet,Count,rig)&&!s.nativeCurveClosedH7[0].ready&&!s.nativeCurveClosedH7[1].ready,"existing skeleton initialization retires both native curve banks");
 AcquireCurveReferences6694(s,reinterpret_cast<void*>(1),28000);
 bool originalCacheH7=g_finger_acquire6694.loadedCacheH7;
 for(int side=0;side<2;++side)for(int n=0;n<s.fingerCount[side];++n)
  originalCacheH7=originalCacheH7&&std::memcmp(&s.closed[side].local[n],&cacheBeforeTrialH7.fist[side][n],sizeof(QuatT28))==0;
 Check(originalCacheH7&&!s.nativeCurveClosedH7[0].ready&&!s.nativeCurveClosedH7[1].ready,
       "later normal body initialization restores original cached defaults not the private trial bank");
#else
 Check(false,"literal parent has no requested F7 terminal trial");
#endif
}
int main(){
 NativeWristProvenanceChecks107();
 // Exercise the real PublishCommitted6689: this optional door provenance must
 // never relabel an old character packet or alter existing pose consumers.
 {
  Init();const auto t=Tracking();CommittedFrame6695 baseline{};
  Check(Run(t,3,true)&&g_committed_frame6695.read(baseline)&&baseline.doorPlayer38==0,
        "door metadata defaults to zero while the existing coherent pose publishes");
  for(int kind=0;kind<6;++kind){
   Init();g_player_publication37=2;g_last_player_entity=&fixture;
   g_player_char37=reinterpret_cast<std::uintptr_t>(character.data());g_player_lease37=GetTickCount64();
   if(kind==1)g_player_publication37=3; // publication in progress
   if(kind==2)mockDoorIdentityChanges38=true; // replacement during packet build
   if(kind==3)g_player_char37=reinterpret_cast<std::uintptr_t>(vtable.data());
   if(kind==4)g_player_lease37=GetTickCount64()-3001;
   if(kind==5)g_last_player_entity=nullptr;
   CommittedFrame6695 packet{};
   Check(Run(t,3,true)&&g_committed_frame6695.read(packet),"identity annotation preserves successful actual pose publication");
   Check(packet.doorPlayer38==(kind==0?reinterpret_cast<std::uintptr_t>(&fixture):0),
         "only a stable fresh ledger for the packet actor supplies door player identity");
   Check(packet.stamp.actor==baseline.stamp.actor&&packet.stamp.epoch==baseline.stamp.epoch&&
         std::memcmp(packet.native,baseline.native,sizeof(packet.native))==0&&
         std::memcmp(packet.allowed,baseline.allowed,sizeof(packet.allowed))==0&&
         std::memcmp(packet.world,baseline.world,sizeof(packet.world))==0&&
         std::memcmp(packet.palm59c9,baseline.palm59c9,sizeof(packet.palm59c9))==0&&
         std::memcmp(packet.palmDirection59c9,baseline.palmDirection59c9,sizeof(packet.palmDirection59c9))==0&&
         Same(fixture.live,fixture.original),"door provenance leaves existing wrist, world, palm geometry and native restore unchanged");
  }
 }
 // The same physical T-pose must produce the same model-space wrists for any
 // tracking-space heading, including the degenerate 90-degree case.
 V3 reference125{};
 for(double yaw:{0.,.78539816339,1.57079632679,-1.57079632679,3.14159265359}){
  Init();g_arm_profile66100.publish(kcd2vr::calibration_profile::Arms{1.50,.75,.75,.20});
  g_cal_frame_qz=float(-std::sin(yaw/2));g_cal_frame_qw=float(std::cos(yaw/2));
  g_cal_hq_z=-g_cal_frame_qz.load();g_cal_hq_w=g_cal_frame_qw.load();
  const Q4 xrYaw{0,float(std::sin(yaw/2)),0,float(std::cos(yaw/2))};
  auto t=Tracking();t.hq=xrYaw;
  t.right=QRotateV(xrYaw,{float(std::sqrt(.75*.75-.20*.20-.10*.10)),-.20f,.10f});
  t.left=QRotateV(xrYaw,{-float(std::sqrt(.75*.75-.20*.20-.10*.10)),-.20f,.10f});
  Check(Run(t,2),"calibrated T-pose solves at rotated tracking origin");
  const auto wrist=FromT(g_pose6689.work[rig.hand_r]);
  if(yaw==0)reference125=wrist;
  Check(Len(Sub(wrist,reference125))<.001f,"tracking-space yaw cannot change calibrated T-pose endpoint");
 }
 // F10=4 must not spend the whole shared budget on fingers before a held blade.
 for(float depth:{-.15f,.10f,.22f}){
  Init();auto t=Tracking();t.right={.76f,-.09f,depth};t.left={-.75f,-.10f,depth};
  kcd2vr::calibration_profile::Window window;
  for(unsigned n=0;n<101;++n)window.add(PV(t.head),PV(t.left),PV(t.right),1000+20*n);
  kcd2vr::calibration_profile::Arms measured{};
  Check(window.measure(measured)&&g_arm_profile66100.publish(measured),"actual asymmetric T-pose measurement is usable");
  Check(Run(t,3),"measured T-pose solves in default upper-body mode");
  for(int side=0;side<2;++side){const int u=side?rig.upper_l:rig.upper_r,h=side?rig.hand_l:rig.hand_r;
   Check(Len(Sub(FromT(g_pose6689.work[h]),FromT(g_pose6689.work[u])))>.498f,
         "recorded full reach maps to full Henry reach regardless of T-pose forward/back offset");}
 }
 Init();mockCollision=true;mockWallX=10;mockSharedBudget124=true;handQueries124=weaponQueries124=0;
 mockPhysical=true;g_weapon_control_mode=6;
 auto& weapon124=mockProfiles[0];weapon124.key=weapon124.generation=1;weapon124.side=0;
 weapon124.primary=weapon124.geometry=weapon124.nativeSeed=true;weapon124.classId=4;weapon124.tip={0,.8,0};
 weapon124.nativeWeapon.p={.25f,.1f,1.3f};
 auto tracked124=Tracking();Check(Run(tracked124,4,true),"combined body and weapon contact commits");
 Check(mockPhysicalOutput.active[0]&&mockPhysicalOutput.contactKnown[0]&&weaponQueries124==5&&handQueries124==9,"held blade runs before free-hand contact and is not constrained twice");
 mockSharedBudget124=false;
 Init();g_arm_profile66100.publish(kcd2vr::calibration_profile::Arms{1.50,.75,.75,.20});
 auto fullReach124=Tracking();fullReach124.right={float(std::sqrt(.75*.75-.20*.20-.10*.10)),-.20f,.10f};
 fullReach124.left={-fullReach124.right.x,fullReach124.right.y,fullReach124.right.z};
 Check(Run(fullReach124,2),"calibrated T-pose solves with unchanged anatomical lengths");
 Check(Len(Sub(FromT(g_pose6689.work[rig.hand_r]),FromT(g_pose6689.work[rig.upper_r])))>.498f,
       "full calibrated reach exceeds 99.6 percent instead of retaining the V123 36 degree bend");
 const auto profiledHand124=FromT(g_pose6689.work[rig.hand_r]);
 fullReach124.hq={0,.5f,0,.8660254f};++fullReach124.sequence;fullReach124.tick+=16;
 Check(Run(fullReach124,2)&&Len(Sub(FromT(g_pose6689.work[rig.hand_r]),profiledHand124))<.001f,
       "head-only yaw cannot move a stationary calibrated controller in arms mode");
 Init();mockCollision=true;mockWallX=.25f;
 g_arm_profile66100.publish(kcd2vr::calibration_profile::Arms{1.50,.75,.75,.20});
 auto wallHand124=Tracking();wallHand124.right={.60f,-.20f,-.20f};
 Check(Run(wallHand124,4)&&FromT(g_pose6689.work[rig.hand_r]).x<=.251f,
       "calibrated reach retarget cannot move the wrist back through a tested wall");
 Init();auto t=Tracking();Check(Run(t,3,true),"production adapter full-body transaction succeeds on coherent anatomy");Check(Same(fixture.live,fixture.original),"every production write restored to byte-identical native pose");Check(g_committed_pose_valid6689&&g_committed_stamp6689.sample==t.sequence,"only committed/restored pose publishes stamped hand target");
 auto solved=g_pose6689.work;Check(std::fabs(Len(Sub(FromT(solved[9]),FromT(solved[8])))-.25f)<.0001&&std::fabs(Len(Sub(FromT(solved[10]),FromT(solved[9])))-.25f)<.0001,"active adapter both arm segments stay anatomical");
 Check(std::memcmp(&solved[0],&fixture.original[0],sizeof(QuatT28))==0&&std::memcmp(&solved[47],&fixture.original[47],sizeof(QuatT28))==0,"native pelvis and leg untouched");
 // Contact is deliberately layered around the accepted body solver.
 Init();mockCollision=true;mockWallX=.20f;t=Tracking();Check(Run(t,4,true),"opt-in contact mode uses actual full-body adapter");
 Check(FromT(g_pose6689.work[10]).x<.201f&&std::abs(Len(Sub(FromT(g_pose6689.work[9]),FromT(g_pose6689.work[8])))-.25f)<.0001,"allowed contact target feeds fixed-length arm rather than raw controller");
 Init();t=Tracking();Run(t,3,true);solved=g_pose6689.work;
 // A changed native attack arm must not change the production solution.
 const auto neutralSolved=solved;for(int id:{8,9,10,12,13,14})fixture.live[id].t[1]+=.18f;fixture.original=fixture.live;
 Check(Run(t),"native attack perturbation accepted without new anatomical lengths");bool invariant=true;for(int id:{7,8,9,10,11,12,13,14})invariant=invariant&&Len(Sub(FromT(g_pose6689.work[id]),FromT(neutralSolved[id])))<.0001f;Check(invariant,"stationary controllers invariant to native attack elbow/shoulder translation");

 Init();t=Tracking();t.right.x=1.2f;Check(Run(t),"unreachable controller uses bounded shoulder compensation then hard stop");Check(std::fabs(Len(Sub(FromT(g_pose6689.work[9]),FromT(g_pose6689.work[8])))-.25f)<.0001,"distant target never lengthens upper arm");
 // V66.123: profile changes shoulder-relative mapping inside soft-reach saturation.
 Init();t=Tracking();t.right.x=.40f;Check(Run(t),"far reach without saved profile solves");const float noProfileReach=Len(Sub(FromT(g_pose6689.work[rig.hand_r]),FromT(g_pose6689.work[rig.upper_r])));
 Init();g_arm_profile66100.publish(kcd2vr::calibration_profile::Arms{1.50,.75,.75,.20});t=Tracking();t.right.x=.40f;Check(Run(t),"far reach with valid saved profile solves");const float profiledReach=Len(Sub(FromT(g_pose6689.work[rig.hand_r]),FromT(g_pose6689.work[rig.upper_r])));
 Check(std::fabs(profiledReach-noProfileReach)>.005f,"saved F8 profile changes shoulder-relative reach below saturation");
 Init();t=Tracking();Check(Run(t,3),"rung 3 baseline coherent pose succeeds");const V3 baselineR=FromT(g_pose6689.work[10]);
 t.tick+=40;t.sequence++;t.hq={0,.25881905f,0,.96592583f};Check(Run(t,3),"HMD yaw with stationary controllers succeeds");Check(Len(Sub(FromT(g_pose6689.work[10]),baselineR))<.012f,"stationary controller keeps right hand stable through HMD yaw");
 t.tick+=40;t.sequence++;t.hq={.25881905f,0,0,.96592583f};Check(Run(t,3),"HMD pitch with stationary controllers succeeds");Check(Len(Sub(FromT(g_pose6689.work[10]),baselineR))<.012f,"stationary controller keeps right hand stable through HMD pitch");
 Init();mockCollision=true;mockWallX=.20f;t=Tracking();Check(Run(t,3),"live core remains available with experimental backend present");
 Check(!g_pose6689.handBlocked[0]&&FromT(g_pose6689.work[10]).x>.20f,"normal body rung makes no collision experiment calls");
 Init();t=Tracking();t.left_valid=false;Check(Run(t),"one lost controller does not disable valid opposite arm");
 Init();t=Tracking();t.hq.w=0;Check(!Run(t)&&writeCalls==0,"invalid tracking quaternion rejects before any write");
 Init();t=Tracking();Check(!Run(t,1)&&writeCalls==0,"rung 1 remains zero-write");
 for(int joint:{1,9,14,46}) {Init();t=Tracking();failAt=joint;bool ok=Run(t,3,true);Check(!ok&&Same(fixture.live,fixture.original)&&!g_committed_pose_valid6689,"injected setter failure restores all attempted joints, including failed joint");}
 Init();t=Tracking();Run(t);auto&s=g_pose6689;s.original.fill({});Check(!SampleFingerEndpoint6689(s,0,true,5,10000)&&!s.closed[0].ready,"invalid/incomplete donor never becomes ready");
 s.original.fill({});for(int i=0;i<Count;++i)s.original[i]=fixture.reference[i];
 SampleFingerEndpoint6689(s,0,true,5,10000);SampleFingerEndpoint6689(s,0,true,5,10500);Check(!s.closed[0].ready,"gap in pose samples resets stable donor interval");
 for(auto now:{10550u,10600u,10650u,10700u,10750u,10800u,10850u})SampleFingerEndpoint6689(s,0,true,5,now);
 Check(s.closed[0].ready,"every finger joint must qualify continuously before endpoint ready");
 s.open[0]=s.closed[0];for(int i=0;i<s.fingerCount[0];++i){QToQuatT(s.closed[0].local[i],{0,0,.70710678f,.70710678f});s.local[s.fingerIds[0][i]]=s.restLocal[s.fingerIds[0][i]];}
 ApplyFingerLocals6689(s,0,1,0);Check(pc::angle(PQ(QFromQuatT(s.local[15])),{0,0,.70710678,.70710678})<.001&&pc::angle(PQ(QFromQuatT(s.local[18])),{})<.001,"actual finger adapter: grip thumb/MRP, index relaxed");
 ApplyFingerLocals6689(s,0,0,1);Check(pc::angle(PQ(QFromQuatT(s.local[15])),{})<.001&&pc::angle(PQ(QFromQuatT(s.local[18])),{0,0,.70710678,.70710678})<.001,"actual finger adapter: trigger index only");
 bool fixed=true;for(int step=0;step<=100;++step){ApplyFingerLocals6689(s,0,step/100.f,step/100.f);Compose6689(s);for(int i=0;i<s.fingerCount[0];++i){int id=s.fingerIds[0][i];// H31: the middle-finger ROOT (palm-owned, parent=wrist) is shifted laterally by the configured mm with curl;
  // every phalanx keeps its anatomical length.
  const bool middleRootH31=id==s.fingers[0][2].id[0];
  const float wantLen=middleRootH31?float(pc::length(MiddleFingerOffsetH31(s,0,2,0,id,FreeFingerInputH7(2,step/100.f,step/100.f,false)))):Len(FromT(s.restLocal[id]));
  fixed=fixed&&std::fabs(Len(Sub(FromT(s.work[id]),FromT(s.work[s.parent[id]])))-wantLen)<.00001;}}
 Check(fixed,"actual full finger adapter preserves offsets across 101 curl values");

 // Exercise the real bounded curve-acquisition state machine. Only its native
 // curve API is replaced; the full finger arrays and FK scoring are production.
 Init();t=Tracking();Run(t);auto& cs=g_pose6689;
 std::array<unsigned char,Count*0x30> joints{};struct Owner{void*v;void*j;} owner{nullptr,joints.data()};
 for(int j=0;j<Count;++j){std::uint32_t crc=unsigned(j+1);std::memcpy(joints.data()+j*0x30+8,&crc,4);}
 cs.owner=&owner;animation_filter::curveAvailable=true;
 for(int i=0;i<100;++i)AcquireCurveReferences6694(cs,reinterpret_cast<void*>(1),20000+i*12);
 Check(cs.open[0].ready&&cs.closed[0].ready&&cs.open[1].ready&&cs.closed[1].ready,"direct curve acquisition completes both full hands without live FIFO");
 Check(animation_filter::curveSamples==450,"bounded acquisition visits all native open/base/clench curves at five complete phases");
 Check(g_finger_save_pending6694&&g_finger_file6694.count[0]==15&&g_finger_file6694.count[1]==15,"complete reference queued for worker persistence");
 const auto crcKey=g_finger_acquire6694.rig;cs.open[0].ready=cs.open[1].ready=cs.closed[0].ready=cs.closed[1].ready=false;
#if KCD2VR_F7_TRIAL_HOST
 cs.nativeCurveClosedH7[0]={};cs.nativeCurveClosedH7[1]={};
#endif
 g_finger_acquire6694={};g_finger_loaded6694=crcKey;animation_filter::curveSamples=0;
 AcquireCurveReferences6694(cs,reinterpret_cast<void*>(1),23000);
 Check(cs.open[0].ready&&cs.closed[1].ready&&animation_filter::curveSamples==0,"compatible reference reload does not call animation curves or wait for gameplay");
#if KCD2VR_F7_TRIAL_HOST
 Check(!cs.nativeCurveClosedH7[0].ready&&!cs.nativeCurveClosedH7[1].ready&&g_finger_acquire6694.loadedCacheH7,"same-rig BIN may restore defaults but never proves the native-curve trial bank");
#endif
 cs.open[0].ready=cs.open[1].ready=cs.closed[0].ready=cs.closed[1].ready=false;g_finger_acquire6694={};g_finger_file6694.crcs[0][0]^=1;
 animation_filter::curveAvailable=false;AcquireCurveReferences6694(cs,reinterpret_cast<void*>(1),25000);
 Check(!cs.open[0].ready&&!cs.closed[0].ready,"incompatible cached finger identity is rejected");
 // Independent position paths: no HMD rotation may move a reachable wrist goal.
 Init();t=Tracking();bool down=true;for(int i=0;i<200;++i){t.right={.25f,-.05f-i*.004f,-.03f};t.left={-.25f,-.05f-i*.004f,-.03f};t.tick=30000+i*12;t.sequence=100+i;
  down=Run(t,2)&&down;const auto&s=g_pose6689;const V3 target=Add({0,0,g_cal_native_head_z.load()},XrToCryV(t.right));const V3 shoulder=FromT(s.work[rig.upper_r]);
  const auto allowed=pc::limit_close_reach103(PV(shoulder),PV(target),s.upper[0],s.fore[0]);
  const auto hand=FromT(s.work[rig.hand_r]),elbow=FromT(s.work[rig.fore_r]);
  down=down&&std::fabs(Len(Sub(elbow,shoulder))-s.upper[0])<.0001f&&std::fabs(Len(Sub(hand,elbow))-s.fore[0])<.0001f;
  if(Len(Sub(target,shoulder))<.44f)down=down&&pc::length(PV(hand)-allowed)<.001f;
  else down=down&&Len(Sub(hand,shoulder))<.500f&&Len(Sub(hand,shoulder))<=Len(Sub(target,shoulder))+.001f;
 }Check(down,"actual arms adapter reaches down through 200 tracked goals with fixed lengths");

 // Additional production-adapter checks for the user's elbow/roll report.
 Init();t=Tracking();Run(t,2);const auto stableElbow=FromT(g_pose6689.work[rig.fore_r]);
 const auto beforeUpdates=g_pose6689.elbow[0].updates;
 bool duplicates=true;
 for(int eye=0;eye<20;++eye){duplicates=Run(t,2)&&duplicates;}
 Check(duplicates&&g_pose6689.elbow[0].updates==beforeUpdates,"same XR sample consumed 20 times does not integrate elbow smoothing again");
 bool rollStable=true,wristExact=true;
 for(int i=0;i<181;++i){const float angle=float(i)*6.28318530718f/180;
  t.rq={std::sin(angle/2),0,0,std::cos(angle/2)};t.sequence++;t.tick+=12;
  rollStable=Run(t,2)&&rollStable;
  rollStable=rollStable&&Len(Sub(FromT(g_pose6689.work[rig.fore_r]),stableElbow))<.0001f;
  auto q=DesiredHand6689(g_pose6689,0,{0,0,0,1},t.rq);
  wristExact=wristExact&&pc::angle(PQ(q),PQ(QFromQuatT(g_pose6689.work[rig.hand_r])))<.001;
 }
 Check(rollStable&&wristExact,"full controller roll does not swivel elbow or discard exact wrist orientation");
 const auto savedContinuity=g_pose6689.elbow[0];failAt=rig.hand_r;failOnce=false;t.sequence++;t.tick+=12;
 Check(!Run(t,2)&&g_pose6689.elbow[0].sample==savedContinuity.sample&&g_pose6689.elbow[0].updates==savedContinuity.updates,"failed native transaction cannot advance elbow temporal history");
 // Same physical skeleton, different local coordinate convention for chest.
 // Compensate immediate children's locals: rest world anatomy is unchanged.
 Init();const auto oldRef=fixture.reference;
 const Q4 chestBasis{.5f,.5f,.5f,.5f};QToQuatT(fixture.reference[rig.spine3],chestBasis);
 for(int j=0;j<Count;++j)fixture.local[j]=fixture.parent[j]<0?fixture.reference[j]:RelativeFromAbsolute(fixture.reference[fixture.parent[j]],fixture.reference[j]);
 fixture.live=fixture.original=fixture.reference;t=Tracking();bool correctAxes=Run(t,2);
 for(int side=0;side<2;++side){
  const auto& ps=g_pose6689;auto preference=QRotateV(QFromQuatT(ps.rest[rig.spine3]),ps.sideDownLocal[side]);
  correctAxes=correctAxes&&preference.z<-.8f&&((side==0&&preference.x>0)||(side==1&&preference.x<0));
 }
 Check(correctAxes,"anatomical down/outward survives a rotated chest BONE-local coordinate system");
 // Whole-hand phase selection uses orientation distances including thumb,
 // not quaternion signs, individual joint extrema, or compact fingertip radius.
 QuatT28 authored[5][32]{};
 for(int phase=0;phase<5;++phase)for(int j=0;j<3;++j){const float angle=phase==3?2.9f:.5f+phase*.01f;
  QToQuatT(authored[phase][j],{0,0,std::sin(angle/2),std::cos(angle/2)});}
 const int chosen=FingerClenchPhase6697(authored,3);
 Check(chosen>=0&&chosen!=3,"an extreme over-curled clench outlier cannot win just by being compact");
 for(int j=0;j<3;++j)for(float& c:authored[1][j].q)c=-c;
 Check(FingerClenchPhase6697(authored,3)==chosen,"whole-hand phase selection is invariant to quaternion sign");
 authored[1][0].q[0]=std::numeric_limits<float>::quiet_NaN();
 Check(FingerClenchPhase6697(authored,3)==-1,"nonfinite authored candidates are rejected, not persisted as a fist");
 Init();t=Tracking();Run(t);auto& exportState=g_pose6689;exportState.owner=&owner;animation_filter::curveAvailable=true;
 for(int i=0;i<100;++i)AcquireCurveReferences6694(exportState,reinterpret_cast<void*>(1),40000+i*12);
 Check(g_finger_audit_pending6697&&g_finger_audit6697.havePhases&&g_finger_audit6697.file.version==3,"automatic reference audit contains generated endpoints and all phases without another user recording");
 WriteFingerAudit6697(g_finger_audit6697); // ordinary host test working directory only
 // V66.98 runs THROUGH the real coherent pose transaction.
 Init();t=Tracking();Check(Run(t,3,true),"physical integration baseline publication");
 g_weapon_control_mode=4;mockPhysical=true;
 mockProfiles[0].key=777;mockProfiles[0].side=0;mockProfiles[0].primary=true;mockProfiles[0].classId=4;mockProfiles[0].geometry=true;mockProfiles[0].tip={0,.8,0};mockProfiles[0].axis={0,1,0};
 t.sequence++;t.tick+=11;Check(Run(t,3,true)&&physicalPublications==1,"successful native pose commit publishes physical result");
 const auto committedPhysical=g_phys_state6698.sample;
 t.sequence++;t.tick+=11;failAt=rig.hand_r;failOnce=false;
 Check(!Run(t,3,true)&&physicalPublications==1&&g_phys_state6698.sample==committedPhysical,"failed native bone write emits no physical pose or commands");
 failAt=-1;t.sequence++;t.tick+=11;Check(Run(t,3,true)&&physicalPublications==2,"physical path recovers after a nonfatal transaction failure");
 Check(Len(Sub(FromT(g_pose6689.work[rig.hand_r]),VP(mockPhysicalOutput.hands[0].p)))<.004,"rendered wrist and published weapon primary are coherent");
 // Current F4 must reach the actual physical input caller. Retired F11 is
 // always zero here; toggling independent recorders cannot select thrust origin.
 Init();t=Tracking();Run(t,3,true);g_weapon_control_mode=7;mockPhysical=true;
 mockProfiles[0].key=777;mockProfiles[0].side=0;mockProfiles[0].primary=true;mockProfiles[0].classId=4;
 mockProfiles[0].geometry=true;mockProfiles[0].tip={0,.8,0};mockProfiles[0].axis={0,1,0};
 retail_build112::mockReady175=true;
 ++t.sequence;t.tick+=11;
 Check(Run(t,3,true)&&mockPhysicalInput175.coherentMelee119&&!mockPhysicalInput175.directionalStabs167,
       "actual weapon caller keeps directional counter experiment off by default");
 kcd2vr::frik_pose_probe::mockCombatExperiment175=true;++t.sequence;t.tick+=11;
 Check(Run(t,3,true)&&mockPhysicalInput175.positionGuard176&&!mockPhysicalInput175.directionalStabs167,
       "R29 F4 positional guard preserves zone-five thrust default until directional stabs are explicitly enabled");
 g_directional_stabs178r29=true;++t.sequence;t.tick+=11;
 Check(Run(t,3,true)&&mockPhysicalInput175.directionalStabs167,
       "F4 reaches actual coherent physical input with the retired F11 preset off");
 kcd2vr::frik_pose_probe::mockCombatExperiment175=false;g_directional_stabs178r29=false;
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,t.tick);
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Reload,true,t.tick);
 ++t.sequence;t.tick+=11;
 Check(Run(t,3,true)&&!mockPhysicalInput175.directionalStabs167,
       "Combat and Reload recorders cannot enable the actual F4 thrust-origin caller");
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,false,t.tick);
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Reload,false,t.tick);
 // V164R1: execute contact ownership through the production transaction, including
 // the transient gap where native equipment exists but no physical frame does.
 Init();mockCollision=true;mockWallX=.20f;mockSharedBudget124=true;t=Tracking();
 handQueries124=0;Check(Run(t,4,true)&&handQueries124==18,"empty hands retain both existing world contact passes");
 g_pose6689.contact[0].planeCount=1;g_pose6689.contact[0].fingerContact[1]=true;g_pose6689.contact[0].curl[1]=.7;
 mockItemState164R1[0]=1;g_weapon_control_mode=6;mockPhysical=false;
 handQueries124=0;++t.sequence;t.tick+=16;
 Check(Run(t,4,true)&&handQueries124==9&&FromT(g_pose6689.work[rig.hand_r]).x>.21f,
       "occupied hand yields world collision even while physical frame is missing; other hand stays active");
 Check(!g_pose6689.contact[0].planeCount&&!g_pose6689.contact[0].fingerContact[1]&&!g_pose6689.allowedReady[0],
       "grasp clears stale world plane and finger contact caches");
 bool nativeGrip164R1=true;for(int n=0;n<g_pose6689.fingerCount[0];++n){const int id=g_pose6689.fingerIds[0][n];
  const auto native=RelativeFromAbsolute(g_pose6689.original[g_pose6689.parent[id]],g_pose6689.original[id]);
  nativeGrip164R1=nativeGrip164R1&&pc::angle(PQ(QFromQuatT(native)),PQ(QFromQuatT(g_pose6689.local[id])))<.001;}
 Check(nativeGrip164R1,"occupied hand without a keyed profile preserves native finger rotations");
 mockItemState164R1[0]=-1;handQueries124=0;++t.sequence;t.tick+=16;
 Check(Run(t,4)&&handQueries124==9,"unknown item membership cannot authorize free-hand collisions");
 mockItemState164R1[0]=0;mockHeldKey164R1[0]=123;handQueries124=0;++t.sequence;t.tick+=16;
 Check(Run(t,4)&&handQueries124==9,"live held key also excludes contact when empty snapshot disagrees");
 mockHeldKey164R1[0]=0;NotifyNativeHandUse(0,2500);
 handQueries124=0;++t.sequence;t.tick+=16;
 Check(Run(t,4)&&handQueries124==9,"native interaction hand excludes world/finger contact during its use window");
 kcd2vr::combat_rules178r10::f5Rung=2;g_pose6689.handoverW178r14[0]=1.f;
 handQueries124=0;++t.sequence;t.tick+=16;
 Check(Run(t,4)&&g_pose6689.handoverW178r14[0]>0.f,
       "selected F5 rung 2 without admission cannot take wrist ownership");
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,true,GetTickCount64());
 workstation178r20c1::setExperiment36(true); // admission leaf; real public gate checked in diagnostic_modes175_bridge
 handQueries124=0;++t.sequence;t.tick+=16;
 Check(Run(t,4)&&g_pose6689.handoverW178r14[0]==0.f,
       "F5 rung 2 immediately releases authored wrist pull while native action remains active");
 kcd2vr::combat_rules178r10::f5Rung=0;
 workstation178r20c1::setExperiment36(false);
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,false,GetTickCount64());
 g_native_use_until107[0]=0;g_native_wrist_until107[0]=0;handQueries124=0;++t.sequence;t.tick+=16;
 Check(Run(t,4)&&handQueries124==18&&FromT(g_pose6689.work[rig.hand_r]).x<.201f,
       "release to known-empty state resumes existing surface collision without recalibration");
 // C43: a VR pickup Use (PickableItem, NATIVE GRASP: ON) withholds the R14 reach blend until the native Attach adopts the wrist.
 static bool graspOn43=false;g_graspEnabled13=[]()noexcept{return graspOn43;};
 NotifyNativeHandUse(0,2500);g_pose6689.handoverW178r14[0]=0.f;
 ++t.sequence;t.tick+=16;Check(Run(t,4)&&g_pose6689.handoverW178r14[0]>0.f,"C43: without a grab window the native action keeps the R14 reach blend");
 graspOn43=true;kcd2vr::grab43::note(0,72,std::uint64_t(GetTickCount64()));
 ++t.sequence;t.tick+=16;Check(Run(t,4)&&g_pose6689.handoverW178r14[0]==0.f,"C43: a fresh VR pickup window under the grasp owner withholds the reach blend at once");
 ++t.sequence;t.tick+=16;Check(Run(t,4)&&g_pose6689.grab43[0].reachEdge&&g_pose6689.grab43[0].vetoed&&g_pose6689.grab43[0].maxPull>0.f,"C43: the window records the vetoed native hand pull");
 kcd2vr::grab43::handStays=false;++t.sequence;t.tick+=16;Check(Run(t,4)&&g_pose6689.handoverW178r14[0]>0.f,"C43: setting off restores the R14 blend inside the window");
 kcd2vr::grab43::handStays=true;graspOn43=false;++t.sequence;t.tick+=16;Check(Run(t,4)&&g_pose6689.handoverW178r14[0]>0.f,"C43: grasp owner off - the window has no authority (fail closed)");
 graspOn43=true;kcd2vr::grab43::useTick[0]=std::uint64_t(GetTickCount64())-5000;++t.sequence;t.tick+=16;Check(Run(t,4)&&g_pose6689.handoverW178r14[0]>0.f,"C43: an expired window has no authority");
 kcd2vr::grab43::useTick[0]=0;kcd2vr::grab43::useKey[0]=0;g_graspEnabled13=nullptr;g_native_use_until107[0]=0;g_native_wrist_until107[0]=0;g_pose6689.handoverW178r14[0]=0.f;g_pose6689.grab43[0]={};
 ++t.sequence;t.tick+=16;Check(Run(t,4),"C43: window state cleared");
 g_finger_capture66100=true;handQueries124=0;++t.sequence;t.tick+=16;
 Check(Run(t,4)&&handQueries124==0,"native finger calibration cannot be moved by free-hand contact");
 g_finger_capture66100=false;mockSharedBudget124=false;
 // Re-establish a normal transaction for the object-finger tests below.
 Init();t=Tracking();Check(Run(t,3),"normal pose ready after ownership comparison");
 // Actual object-finger adapter: native rotations with immutable anatomy.
 auto& fs=g_pose6689;const int fj=fs.fingerIds[0][0],fp=fs.parent[fj];
 const Q4 gripRotation{0,0,.5f,.8660254f};
 QToQuatT(fs.original[fj],QMul(QFromQuatT(fs.original[fp]),gripRotation));
 Check(ApplyObjectFingerLocals6698(fs,0,991,1,1,1000)&&pc::angle(PQ(QFromQuatT(fs.local[fj])),PQ(gripRotation))<.001,"owned weapon uses actual native object finger rotation");
 Check(Len(Sub(FromT(fs.local[fj]),FromT(fs.restLocal[fj])))<.000001f,"object grip keeps immutable anatomical finger offsets");
 for(int i=1;i<=20;++i)ApplyObjectFingerLocals6698(fs,0,991,1,i+1,1000+i*10);
 Check(fs.objectCandidate[0].ready,"held fingers qualify only after stable native window");
 QToQuatT(fs.original[fj],QFromQuatT(fs.original[fp]));
 ApplyObjectFingerLocals6698(fs,0,991,1);
 Check(pc::angle(PQ(QFromQuatT(fs.local[fj])),PQ(gripRotation))<.001,"native animation cannot recapture object finger pose every frame");
 ApplyObjectFingerLocals6698(fs,0,991,2);
 Check(pc::angle(PQ(QFromQuatT(fs.local[fj])),{})<.001,"new item generation reacquires native finger shape without player calibration");
 // An interactive action can stay open longer than the weapon-cache settling
 // interval, then close later. Its evaluated fingers must keep advancing.
 for(int i=0;i<30;++i)ApplyObjectFingerLocals6698(fs,0,0xF5F5,3,30+i,2000+10*i,true);
 Check(!fs.objectCandidate[0].ready,"interactive fingers do not latch an early open pose");
 QToQuatT(fs.original[fj],QMul(QFromQuatT(fs.original[fp]),gripRotation));
 Check(ApplyObjectFingerLocals6698(fs,0,0xF5F5,3,60,2300,true)&&
       pc::angle(PQ(QFromQuatT(fs.local[fj])),PQ(gripRotation))<.001,
       "native handle closure advances after a long open-hand approach");
 QToQuatT(fs.original[fj],QFromQuatT(fs.original[fp]));
 ApplyObjectFingerLocals6698(fs,0,0xF5F5,3,61,2310,true);
 Check(pc::angle(PQ(QFromQuatT(fs.local[fj])),{})<.001&&
       Len(Sub(FromT(fs.local[fj]),FromT(fs.restLocal[fj])))<.000001f,
       "native release reopens fingers with unchanged anatomy");
 // V66.100: exact coherent callsite follows tracked wrists but keeps all native
 // parent-local finger rotations and translations during 1.5 capture.
 Init();t=Tracking();g_finger_capture66100=true;g_capture_session66100.fetch_add(1);
 const Q4 nativeCurl{0,0,.35f,.9367497f};
 for(int side=0;side<2;++side)for(int digit=0;digit<5;++digit){const int id=(side?30:15)+digit*3+1;QToQuatT(fixture.local[id],nativeCurl);}
 for(int j=0;j<Count;++j)fixture.live[j]=fixture.parent[j]<0?fixture.local[j]:AbsoluteFromRelative(fixture.live[fixture.parent[j]],fixture.local[j]);
 fixture.original=fixture.live;
 Check(Run(t,2),"1.5 executes actual tracked-arms solver without finger references");
 bool nativeLocals=true;for(int side=0;side<2;++side)for(int n=0;n<g_pose6689.fingerCount[side];++n){const int id=g_pose6689.fingerIds[side][n],p=g_pose6689.parent[id];const auto local=RelativeFromAbsolute(g_pose6689.work[p],g_pose6689.work[id]);nativeLocals=nativeLocals&&pc::angle(PQ(QFromQuatT(local)),PQ(QFromQuatT(fixture.local[id])))<.001&&Len(Sub(FromT(local),FromT(fixture.local[id])))<.00001f;}
 Check(nativeLocals,"all native finger locals survive moving arm/wrist recomposition unchanged");
 g_weapon_control_mode=4;mockPhysical=true;t.sequence++;t.tick+=20;Check(Run(t,2)&&physicalPublications==0,"native-finger-only stage cannot run physical weapon writer even under conflicting mode flags");g_weapon_control_mode=0;mockPhysical=false;
 Check(Len(Sub(FromT(g_pose6689.work[rig.hand_r]),FromT(fixture.original[rig.hand_r])))>.1f,"native fingers remain attached to controller-driven wrist, not native arm placement");
 Check(g_capture_runtime66100.rig&&!g_finger_acquire6694.initialized,"manual capture resolves rig identity without invoking authored-curve acquisition");
 // Supply a real native local-pose sequence through the production reader.
 auto setCurl=[&](float angle){for(int side=0;side<2;++side)for(int digit=0;digit<5;++digit){const int id=(side?30:15)+digit*3+1;QToQuatT(fixture.local[id],{0,0,std::sin(angle*.5f),std::cos(angle*.5f)});}for(int j=0;j<Count;++j)fixture.live[j]=fixture.parent[j]<0?fixture.local[j]:AbsoluteFromRelative(fixture.live[fixture.parent[j]],fixture.local[j]);fixture.original=fixture.live;};
 setCurl(0);for(int n=0;n<10;++n){t.tick+=50;t.sequence++;Run(t,2);}
 t.right_trigger=t.left_trigger=1;t.tick+=50;t.sequence++;Run(t,2);setCurl(1.2f);
 for(int n=0;n<24;++n){t.tick+=50;t.sequence++;Run(t,2);}
 Check(FingerPacket66100{}.pair.rig==0&&g_finger_candidate_ready66100,"native-only pose reader produces complete geometry-qualified numeric candidate");
 t.right_trigger=t.left_trigger=0;t.tick+=50;t.sequence++;Run(t,2);
 TempIniFixtureD62 calibrationIniD62;
 std::strcpy(g_calibration_ini66100,calibrationIniD62.name.c_str());ServiceCalibrationIo66100();
 FingerPacket66100 manual{};g_finger_saved66100.read(manual);
 Check(g_finger_saved_generation66100&&manual.pair.rig==g_capture_runtime66100.rig&&fc100::valid(manual.pair),"observed native release sends pair through actual worker persistence");
 g_finger_capture66100=false;setCurl(0);t.right_trigger=t.left_trigger=t.right_grip=t.left_grip=1;t.tick+=50;t.sequence++;Run(t,2);
 bool preview=true;for(int side=0;side<2;++side)for(int n=0;n<g_pose6689.fingerCount[side];++n){const int id=g_pose6689.fingerIds[side][n];// H31 defaults (finger_tuning40.h): middle root shifted by getShiftMmH31, middle joints 1-2 turned by getDistalMedialDegH31.
  const int mid0H31=g_pose6689.fingers[side][0].count+g_pose6689.fingers[side][1].count,kH31=n-mid0H31;
  const bool midH31=kH31>=0&&kH31<g_pose6689.fingers[side][2].count;
  const double wantAngH31=midH31&&(kH31==1||kH31==2)?std::fabs(double(kcd2vr::finger40::getDistalMedialDegH31(side)))*3.14159265358979/180.:0.;
  const float wantShiftH31=midH31&&kH31==0?kcd2vr::finger40::getShiftMmH31(side)*.001f:0.f;
  preview=preview&&std::fabs(pc::angle(PQ(QFromQuatT(g_pose6689.local[id])),manual.pair.closed[side].joints[n].q)-wantAngH31)<.001&&
      std::fabs(Len(Sub(FromT(g_pose6689.local[id]),FromT(g_pose6689.restLocal[id])))-wantShiftH31)<.00001f;}
 Check(preview,"rung 2 uses captured fist rotations with immutable segment offsets");
 // Even a previously queued CAF publication must not overwrite manual references.
 g_finger_acquire6694.pendingPublish=true;animation_filter::curveAvailable=true;auto prior=manual.pair.closed[0].joints[1].q;
 t.tick+=50;t.sequence++;Run(t,2);
 Check(pc::angle(PQ(QFromQuatT(g_pose6689.closed[0].local[1])),prior)<.001,"pending automatic curve work cannot overwrite accepted native pair");
 t.right_trigger=t.left_trigger=t.right_grip=t.left_grip=0;t.tick+=50;t.sequence++;Run(t,2);
 Check(pc::angle(PQ(QFromQuatT(g_pose6689.local[g_pose6689.fingerIds[0][1]])),manual.pair.open[0].joints[1].q)<.001,"released controls reproduce captured relaxed native endpoint");
 g_pose6689.open[0].ready=false;g_pose6689.closed[0].ready=false;t.tick+=50;t.sequence++;Run(t,2);
 Check(g_pose6689.open[0].ready&&g_pose6689.closed[0].ready,"profile reapplies after a local reference reset without recalibration");

 for(int mode:{2,3}){
  Init();t=Tracking();bool below=true,endpoint=true,solves=true;
  for(int n=0;n<=160;++n){const float f=float(n)/160;t.sequence++;t.tick+=16;
   t.right={.675f-.55f*f,-.28f,-(.005f+.22f*f)};t.left={-t.right.x,t.right.y,t.right.z};
   solves=Run(t,mode)&&solves;
   for(int side=0;side<2;++side){const auto& state=g_pose6689;const int u=side?rig.upper_l:rig.upper_r,fore=side?rig.fore_l:rig.fore_r,hand=side?rig.hand_l:rig.hand_r;
    const auto a=FromT(state.work[u]),b=FromT(state.work[fore]),c=FromT(state.work[hand]);
    below=below&&b.z<=std::max(a.z,c.z)+.002f;
    endpoint=endpoint&&std::abs(Len(Sub(a,b))-.25f)<.0001f&&std::abs(Len(Sub(b,c))-.25f)<.0001f;
   }
  }
  Check(solves&&below&&endpoint,mode==2?"production arms T-to-center: both elbows stay below level hands with fixed lengths":"production full upper body T-to-center: anatomical elbow side and lengths retained");
 }
 Init();t=Tracking();Run(t,2);const auto firstKey=g_capture_runtime66100.rig;
 for(int id:{16,20,34})for(float& q:fixture.reference[id].q)q=-q;
 g_pose6689.~CoherentState6689(); new (&g_pose6689) CoherentState6689;g_capture_runtime66100={};t.sequence++;t.tick+=16;Run(t,2);
 Check(g_capture_runtime66100.rig==firstKey,"actual rig key is invariant under equivalent reference quaternion signs");
 // V115R1: execute the changed body target in the actual transaction.
 Init();t=Tracking();Run(t);const V3 baseChest=FromT(g_pose6689.work[rig.spine3]);
 const V3 baseHead=FromT(g_pose6689.work[rig.head]);
 t.head.z=-.14f;t.head.y=-.04f;t.tick+=16;++t.sequence;
 Check(Run(t),"HMD forward lean solves through actual body transaction");
 Check(FromT(g_pose6689.work[rig.spine3]).y>baseChest.y+.02f&&FromT(g_pose6689.work[rig.head]).y>baseHead.y+.10f,"chest and head follow forward HMD translation rather than counter-moving");
 Init();t=Tracking();t.hq={-.38268343f,0,0,.92387953f};Run(t);
 const auto pitched=g_pose6689.work;
 Check(pc::angle(PQ(QFromQuatT(pitched[rig.neck])),PQ(QMul(QNlerpIdentity(XrToCryQ(t.hq),.85f),QFromQuatT(g_pose6689.neutral[rig.neck]))))<.0001,"neck follows 85 percent of head rotation rather than bending back toward a fixed head target");
 bool lengthsPreserved=true;for(int j=1;j<=6;++j)lengthsPreserved=lengthsPreserved&&std::abs(Len(Sub(FromT(pitched[j]),FromT(pitched[j-1])))-Len(FromT(fixture.local[j])))<.0001;
 Check(lengthsPreserved&&std::memcmp(&pitched[rig.pelvis],&fixture.original[rig.pelvis],sizeof(QuatT28))==0,"head/neck change preserves every spine length and the native pelvis");
 // Poison last-frame neutral cache. Current composition must erase its influence.
 for(auto& q:g_pose6689.neutral){q.q[0]=1;q.q[3]=0;q.t[2]=99;}
 Check(Run(t)&&Len(Sub(FromT(g_pose6689.work[rig.head]),FromT(pitched[rig.head])))<.0001,"head pivot uses fresh neutral anatomy, not previous-frame cache");
 Init();t=Tracking();auto pelvisLocals115=fixture.local;pelvisLocals115[rig.pelvis].t[1]=.06f;pelvisLocals115[rig.pelvis].t[2]-=.08f;
 for(int j=0;j<Count;++j)fixture.original[j]=fixture.parent[j]<0?pelvisLocals115[j]:AbsoluteFromRelative(fixture.original[fixture.parent[j]],pelvisLocals115[j]);
 fixture.live=fixture.original;Check(Run(t),"body solves with non-bind native pelvis");
 const auto expectedSpine=AbsoluteFromRelative(fixture.original[rig.pelvis],fixture.local[rig.spine]);
 Check(Len(Sub(FromT(g_pose6689.work[rig.spine]),FromT(expectedSpine)))<.0001&&std::memcmp(&g_pose6689.work[rig.pelvis],&fixture.original[rig.pelvis],sizeof(QuatT28))==0,"staged torso starts at actual native pelvis, never an unwritten bind-pose pelvis");
 // V66.137: deferred passenger composition must not change the solved skeleton.
 std::array<std::array<QuatT28,Count>,9> full137{},deferred137{};
 bool eq137=true;
 for(int pass=0;pass<2;++pass){
  Init();g_arm_profile66100.publish(kcd2vr::calibration_profile::Arms{1.50,.75,.75,.20});
  g_compose_defer137=(pass==1);auto q=Tracking();
  for(int i=0;i<9;++i){q.sequence++;q.tick+=11;q.head.x=.04f*std::sin(float(i)*.31f);q.head.y=.02f*std::cos(float(i)*.23f);
   q.hq={0.f,std::sin(float(i)*.017f),0.f,std::cos(float(i)*.017f)};
   if(!Run(q,3)){eq137=false;break;}
   std::copy_n(g_pose6689.work.begin(),Count,(pass?deferred137:full137)[i].begin());
  }
 }
 for(int i=0;i<9&&eq137;++i)eq137=std::memcmp(full137[i].data(),deferred137[i].data(),sizeof(full137[i]))==0;
 Check(eq137,"V137 deferred passenger compose is bit-identical to full compose");
 g_compose_defer137=true;
 // V169R1: exercise the actual final-pose evidence builder (not the early
 // self projection flag). No native call and no skeleton mutation by observation.
 Init();t=Tracking();Check(Run(t,4),"contact-evidence setup uses production coherent solve");
 {
  namespace p=kcd2vr::passive169r1;CoherentTransaction6689 tx;tx.tracking=t;tx.epoch=1;
  bool free[2]{true,false};V3 requested[2]{};HandContact6695 contacts[2]{};
  auto& st=g_pose6689;const auto a=FromT(st.work[rig.pelvis]);
  const float radius=std::clamp(Len(Sub(FromT(st.work[rig.upper_r]),FromT(st.work[rig.upper_l])))*.32f,.08f,.16f);
  auto place=[&](float r){for(int f=0;f<5;++f)for(int j=0;j<st.fingers[0][f].count;++j){auto& bone=st.work[st.fingers[0][f].id[j]];
    bone.t[0]=a.x+r;bone.t[1]=a.y;bone.t[2]=a.z+.10f;}};
  place(radius+.005f);requested[0]=FromT(st.work[rig.hand_r]);
  const auto before=st.work;BuildPassiveHandEvidence169R1(tx,true,free,requested,contacts);
  Check(tx.passive169r1[2].status==p::Status::Contact&&tx.passive169r1[2].gap>.003,
    "final hand points near existing torso proxy qualify soft self touch");
  Check(std::memcmp(before.data(),st.work.data(),sizeof(before))==0,"contact evidence never writes final bones");
  Check(!tx.passive169r1[3].eligible&&tx.passive169r1[3].status==p::Status::Unavailable,
    "held opposite hand remains excluded from free-hand self feedback");
  Check(tx.passive169r1[0].status==p::Status::Unknown,"no fresh world query is UNKNOWN, not clear/contact");
  // H25: the production recovery can append finger0..5 at plane index6;
  // only feature6 is the knuckle, not every entry occupying the seventh slot.
  contacts[0].have=contacts[0].fresh169r1=contacts[0].blocked=true;
  contacts[0].planeCount=7;contacts[0].featureCount=6;
  ModelWorld6692 modelH25{};Check(ReadModelWorld6692(st.character,modelH25),"H25 passive fixture uses the actual completed world transform");
  const auto finalTipH25=ModelToWorld6692(modelH25,FromT(st.work[st.fingers[0][4].id[st.fingers[0][4].count-1]]));
  for(int j=0;j<7;++j){auto& plane=contacts[0].planes[j];plane.feature=j<6?j:5;plane.normal={-1,0,0};
   plane.point=Add(finalTipH25,{j<6?.10f:.008f,0,0});plane.witness169r1=100+j;
   plane.native172.collider=j<6?0x180004000ull:0x180007000ull;plane.native172.entityId=j<6?777:888;
   plane.native172.owner=j<6?0x180006000ull:0x180008000ull;plane.native172.part=j<6?200+j:207;
   plane.native172.body=kcd2vr::npc_contact172::Body::Articulated;}
  BuildPassiveHandEvidence169R1(tx,true,free,requested,contacts);
  Check(tx.passive169r1[0].status==p::Status::Contact&&tx.passive169r1[0].feature==5&&
    tx.passive169r1[0].witness==106&&tx.passiveTargetsH21[0].contact.entityId==888&&tx.passiveTargetsH21[0].contact.part==207,
    "H25 actual passive/F9 witness builder consumes a current recovery finger at plane index6");
  contacts[0].planes[6].feature=6;BuildPassiveHandEvidence169R1(tx,true,free,requested,contacts);
  Check(tx.passive169r1[0].status==p::Status::Unknown&&tx.passive169r1[0].witness!=106,
    "H25 scanning all plane slots preserves the original semantic knuckle exclusion");
  contacts[0]={};
  place(radius-.020f);BuildPassiveHandEvidence169R1(tx,true,free,requested,contacts);
  Check(tx.passive169r1[2].status==p::Status::Unknown&&(tx.passive169r1[2].flags&0x10000),
    "final self penetration rejects success even when earlier solve looked resolved");
  const auto oldRig=st.rig;st.rig.pelvis=st.rig.spine3=-1;
  BuildPassiveHandEvidence169R1(tx,false,free,requested,contacts);
  Check(!tx.passive169r1[2].eligible,"disabled contact never dereferences invalid torso indices");st.rig=oldRig;
 }

 for(int drawPolicy596c1=0;drawPolicy596c1<2;++drawPolicy596c1)
 for(int retargetCase169r2=0;retargetCase169r2<3;++retargetCase169r2){
 mockGripDraw596c1=drawPolicy596c1!=0;
 // V169R2: execute the ACTUAL body and physical bow kernel together. F2 alone
 // selects manual mode; the standalone INI manual flag deliberately stays OFF.
 Init();g_bow_manual_reload146=false;g_bow_native_grab146=false;g_bow_string_trigger160=false;g_experiments169=true;
 // The combined F4 mode must keep automatic nocking even when a saved INI
 // requests both experimental manual-arrow policies. Exercise the actual
 // coherent adapter and bow kernel, including draw/release and tracking loss.
 g_combat_integrated178r22c2=retargetCase169r2==2;
 if(g_combat_integrated178r22c2){g_bow_manual_reload146=true;g_bow_native_grab146=true;}
 g_chest_natural128=true;g_body_follow128=false;mockPhysical=true;g_weapon_control_mode=6;
 auto bowTracking169r2=Tracking();bowTracking169r2.tick+=10000*retargetCase169r2;bowTracking169r2.left_grip=1;bowTracking169r2.right_trigger=0;bowTracking169r2.right_grip=0;
 // At the nock, then a diagonal physical draw toward the right cheek. All
 // coordinates below are test geometry, not prescribed user/controller offsets.
 kcd2vr::calibration_profile::Arms profile169r2{};
 if(retargetCase169r2==1)profile169r2={1.50,.75,.75,.20};
 if(retargetCase169r2==2){profile169r2.leftOffset={-.73,-.12,-.20};profile169r2.rightOffset={.75,-.12,-.20};
     profile169r2.span=pc::length(profile169r2.rightOffset-profile169r2.leftOffset);profile169r2.leftReach=pc::length(profile169r2.leftOffset);
     profile169r2.rightReach=pc::length(profile169r2.rightOffset);profile169r2.headToHandPlane=.20;profile169r2.measuredOffsets=true;}
 if(retargetCase169r2)Check(g_arm_profile66100.publish(profile169r2)&&kcd2vr::calibration_profile::valid(profile169r2),"actual bow fixture uses a valid stored anthropometric profile");
 const V3 bowHand169r2{-.15f,.35f,1.42f},startHand169r2{-.15f,.25f,1.49f},endHand169r2{.15f,-.12f,1.54f};
 auto trackingHand169r2=[&](V3 p,int side=0){
     pc::Vec3 fromHead=PV(Sub(p,FromT(fixture.reference[rig.head])));
     // H17 (pose_reach_input.inl bowOwnsReachH17): a held bow keeps exact 1:1 endpoints; the stored profile
     // is installed but must NOT re-map them. The old profile pre-mapping below predates H17 and is kept off.
     constexpr bool profileRemapsBowH17=false;
     if(profileRemapsBowH17&&retargetCase169r2){const double half=.18,span=2*half+1.0,W=half*profile169r2.span/span,D=.20,B=.10;
         const pc::Vec3 u{side?-W:W,-B,-D};const auto off=side?profile169r2.leftOffset:profile169r2.rightOffset;
         const double reach=side?profile169r2.leftReach:profile169r2.rightReach;
         const double arm=profile169r2.measuredOffsets?pc::length(off-u):std::sqrt(reach*reach-D*D-B*B)-W;
         fromHead=u+(PV(p)-PV(FromT(fixture.reference[side?rig.upper_l:rig.upper_r])))*(arm/.50);}
     return V3{float(fromHead.x),float(fromHead.z),float(-fromHead.y)};
 };
 bowTracking169r2.left=trackingHand169r2(bowHand169r2,1);bowTracking169r2.right=trackingHand169r2(startHand169r2);
 // Initialize only the rig/anatomical hand bases before installing a profile.
 mockPhysical=false;Check(Run(bowTracking169r2,3),"bow fixture establishes actual rig hand bases");mockPhysical=true;
 auto& bow169r2=mockProfiles[1];bow169r2.key=7;bow169r2.generation=3;bow169r2.side=1;bow169r2.classId=9;
 bow169r2.primary=bow169r2.support=true;bow169r2.geometry=false;
 bow169r2.grip[1].q=PQ(DesiredHand6689(g_pose6689,1,{0,0,0,1},bowTracking169r2.lq));
 bow169r2.grip[0].p={0,-.10,0};bow169r2.axis={0,-1,0};
 bool bowAllCommitted169r2=true,bowAllOwned169r2=true,bowExact169r2=true;double bowMaxError169r2=0;
 auto tickBow169r2=[&](){++bowTracking169r2.sequence;bowTracking169r2.tick+=10;
     const bool ok=Run(bowTracking169r2,4,true);bowAllCommitted169r2=bowAllCommitted169r2&&ok;return ok;};
 tickBow169r2();tickBow169r2();
 if(mockGripDraw596c1)bowTracking169r2.right_grip=1;else bowTracking169r2.right_trigger=1;tickBow169r2();
 Check(!mockPhysicalOutput.bowHeld,"fallback animation wrist cannot become the first physical string nock");
 bowTracking169r2.right_grip=bowTracking169r2.right_trigger=0;tickBow169r2();
 bow169r2.bowNockQualified40=true;tickBow169r2(); // native geometry leaf is explicitly modeled in this pose fixture
 if(mockGripDraw596c1)bowTracking169r2.right_grip=1;else bowTracking169r2.right_trigger=1;tickBow169r2();
 Check(mockPhysicalOutput.bowHeld&&mockPhysicalOutput.bowHand169r2,"selected Grip/Trigger policy acquires tracked string with native automatic loading");
 Check(!mockPhysicalInput175.manualNockAck19c4r4.serial,"F11 off coherent pose never invents a manual native nock ticket");
 const auto headBeforeBow169r2=FromT(g_pose6689.work[rig.head]);
 for(int j=1;j<=50;++j){const float t=float(j)/50;bowTracking169r2.right=trackingHand169r2(Add(startHand169r2,Mul(Sub(endHand169r2,startHand169r2),t)));
     tickBow169r2();bowAllOwned169r2=bowAllOwned169r2&&mockPhysicalOutput.owned[0]&&mockPhysicalOutput.bowHeld;
     // Grip couples the wrist to the allowed string pose; the raw controller
     // endpoint is intentionally projected by that accepted coupling kernel.
     const auto allowed=mockPhysicalOutput.hands[0].p;
     const V3 expected=mockGripDraw596c1?V3{float(allowed.x),float(allowed.y),float(allowed.z)}:
         Add(startHand169r2,Mul(Sub(endHand169r2,startHand169r2),t));
     const double err=Len(Sub(FromT(g_pose6689.work[rig.hand_r]),expected));
     bowMaxError169r2=std::max(bowMaxError169r2,err);bowExact169r2=bowExact169r2&&err<.004;
 }
 Check(bowAllCommitted169r2,"actual bow draw sequence has no failed pose transactions");
 Check(bowAllOwned169r2,"actual bow draw retains right-hand ownership in every sampled step");
 Check(bowExact169r2,"torso participation does not add wrist translation to the allowed drawing target");
 Check(std::abs(g_pose6689.archer169r2.correction)>.1,"physical pull produces bounded torso participation");
 Check(Len(Sub(FromT(g_pose6689.work[rig.head]),headBeforeBow169r2))<.001,"bow torso twist preserves the existing HMD head target");
 const double fractionBeforeHold169r2=mockPhysicalOutput.bowFraction;
 const auto wristBeforeHold169r2=FromT(g_pose6689.work[rig.hand_r]);
 for(int j=0;j<20;++j)tickBow169r2();
 Check(std::abs(mockPhysicalOutput.bowFraction-fractionBeforeHold169r2)<.001&&Len(Sub(FromT(g_pose6689.work[rig.hand_r]),wristBeforeHold169r2))<.001,
     "settling chest/shoulder motion cannot manufacture more draw with fixed controllers");
 const auto yawBeforeDuplicate169r2=g_pose6689.archer169r2.correction;
 Check(Run(bowTracking169r2,4,true)&&g_pose6689.archer169r2.correction==yawBeforeDuplicate169r2,
     "duplicate eye/pose call leaves archer temporal state unchanged");
 // Head-only yaw must not move the two tracked endpoints or create charge.
 bool bowHeadTurn169r2=true,bowNativeRestored169r2=true;
 for(int j=1;j<=30;++j){const float a=.40f*float(j)/30;bowTracking169r2.hq={0,std::sin(a*.5f),0,std::cos(a*.5f)};
     const bool ok=tickBow169r2();bowHeadTurn169r2=bowHeadTurn169r2&&ok&&mockPhysicalOutput.bowHeld&&
         Len(Sub(FromT(g_pose6689.work[rig.hand_r]),wristBeforeHold169r2))<.004&&std::abs(mockPhysicalOutput.bowFraction-fractionBeforeHold169r2)<.001;
     bowNativeRestored169r2=bowNativeRestored169r2&&Same(fixture.live,fixture.original);
 }
 Check(bowHeadTurn169r2,"actual held bow plus head-only turn preserves wrist endpoint and draw fraction");
 Check(bowNativeRestored169r2,"bow pose writes still restore byte-identical native buffer each call");
 // Simulate a setter failure. A failed pose may neither publish nor advance
 // the archer state, even if the requested draw/head position has changed.
 const auto beforeFailure169r2=g_pose6689.archer169r2;const int pubs169r2=physicalPublications;
 failAt=rig.upper_r;failOnce=false;++bowTracking169r2.sequence;bowTracking169r2.tick+=10;
 Check(!Run(bowTracking169r2,4,true)&&physicalPublications==pubs169r2&&
     g_pose6689.archer169r2.sample==beforeFailure169r2.sample&&g_pose6689.archer169r2.correction==beforeFailure169r2.correction&&Same(fixture.live,fixture.original),
     "failed bow pose write rolls back native buffer and leaves posture/publication unchanged");
 failAt=-1;tickBow169r2();
 {
  namespace q=kcd2vr::diagnostics::bow169r2;namespace cap=kcd2vr::diagnostics::capture168;
  bowTracking169r2.tick+=200;++bowTracking169r2.sequence;cap::gate.select(cap::Domain::Combat,true,bowTracking169r2.tick);
  CoherentTransaction6689 tx;const bool ok=BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,4,bowTracking169r2);tx.Restore();
  const int before=physicalPublications;const auto saved=vtable[0x190/8];vtable[0x190/8]=0;PublishCommitted6689(tx);vtable[0x190/8]=saved;
  bool saw=false;q::Record r{};while(q::pop(r))saw=saw||(!r.kind&&r.stage==2&&r.committed&&!r.posePacketPublished&&!r.physicalPublishAttempted);
  Check(ok&&before==physicalPublications&&saw,"bow publication rejection is recorded separately from a successful skeletal commit");
  cap::gate.select(cap::Domain::Combat,false,bowTracking169r2.tick);tickBow169r2();
 }
 // A brief VALID held snapshot keeps the same retarget frame, but existing
 // native bow-action guards cancel instead of firing from extrapolated data.
 const auto wristBeforeGrace169r2=FromT(g_pose6689.work[rig.hand_r]);
 const auto yawBeforeGrace169r2=g_pose6689.archer169r2.correction;
 bowTracking169r2.right_held=true;tickBow169r2();
 bool graceNoFire169r2=true;for(unsigned c=0;c<mockPhysicalOutput.commandCount;++c)graceNoFire169r2=graceNoFire169r2&&mockPhysicalOutput.commands[c].kind!=ph::CommandKind::BowFire;
 Check(Len(Sub(FromT(g_pose6689.work[rig.hand_r]),wristBeforeGrace169r2))<.004&&g_pose6689.archer169r2.correction==yawBeforeGrace169r2&&graceNoFire169r2,
     "existing short tracking grace keeps visual retarget frame but never fabricates a bow fire or torso advance");
 bowTracking169r2.right_held=false;bowTracking169r2.right_trigger=0;bowTracking169r2.right_grip=0;tickBow169r2();
 Check(!mockPhysicalOutput.bowHeld&&!mockPhysicalOutput.bowHand169r2,"selected draw control release ends string ownership");
 std::printf("bow169r2_runtime retargetCase=%d maxWristErrorM=%.8f finalFractionBeforeRelease=%.6f\n",retargetCase169r2,bowMaxError169r2,fractionBeforeHold169r2);
 }
 mockGripDraw596c1=false;g_experiments169=false;g_combat_integrated178r22c2=false;g_bow_manual_reload146=false;g_bow_native_grab146=false;
 #include "melee_frame32c1_cases.inc"

 {
  namespace props=kcd2vr::prop5916c1;
  Init();auto t=Tracking();Check(Run(t,3,true),"prop baseline pose publishes");
  CommittedFrame6695 before{};g_committed_frame6695.read(before);
  const auto requested=FromT(before.allowed[0]);
  props::Presentation p{};p.character=reinterpret_cast<std::uintptr_t>(character.data());p.body=10;p.key=11;
  p.bodyEpoch=1;p.experimentEpoch=2;p.observationTick=p.sourceTick=10000;p.side=0;p.sequence=t.sequence;
  // Fixture model world is translation (100,200,10). The physical item is 2 cm behind the requested wrist.
  p.wristWorld={PQ(QFromQuatT(before.allowed[0])),{100+requested.x-.02,200+requested.y,10+requested.z}};
  props::presentation.publish(p);kcd2vr::combat_rules178r10::f5Rung=2;
  workstation178r20c1::experimentArmed36=true;workstation178r20c1::experimentEpoch36=2;
  Check(Run(t,3,true),"physics-held prop hand participates in the actual coherent solve");
  CommittedFrame6695 held{};g_committed_frame6695.read(held);
  Check(std::abs(FromT(held.allowed[0]).x-(requested.x-.02))<.004,"shown wrist follows achieved object, not the requested free hand");
  Check(std::abs(FromT(held.propRequested5916c1[0]).x-requested.x)<.004,"drive target remains independent of constrained visible wrist");
  Check(Same(fixture.live,fixture.original),"prop-hand pose restores native buffer like other coherent writes");
  props::clearPresentation();workstation178r20c1::experimentArmed36=false;kcd2vr::combat_rules178r10::f5Rung=0;
  Check(Run(t,3,true),"prop exit returns to ordinary tracked pose");
 }
 {
  Init();auto t=Tracking();Check(Run(t,3,true),"activity pose starts with valid visual owner");
  mockPhysical=true;g_weapon_control_mode=7;g_gameplay_allowed=false;++t.sequence;t.tick+=20;
  Check(Run(t,5,true)&&physicalPublications==0,"visual activity/dialogue pose cannot publish weapon or holster intent");
  g_gameplay_allowed=true;
 }
 {
  Init();auto t=Tracking();t.right={.25f,-.55f,.05f};t.left={-.25f,-.55f,.05f};
  Check(Run(t,3,true),"render envelope uses the actual restored coherent publication");
  kcd2vr::render_bounds63::Envelope e{};CommittedFrame6695 p{};
  Check(g_render_bounds63.read(e)&&g_committed_frame6695.read(p)&&
      e.character==p.stamp.actor&&e.sample==p.stamp.sample&&e.epoch==p.stamp.epoch&&e.tick==p.stamp.tick&&
      e.publication==g_frame_publication51.load()&&e.policy==g_pose_policy_generation57.load()&&e.control==g_frame_control51.load(),
      "actual envelope is joined to the completed frame/control/policy");
  bool contains=true;for(int side=0;side<2;++side){const auto h=FromT(p.allowed[side]);const float xyz[]{h.x,h.y,h.z};
      for(int k=0;k<3;++k)contains=contains&&xyz[k]>=e.box.v[k]&&xyz[k]<=e.box.v[k+3];}
  Check(e.freshTracked&&e.points>=4&&e.points<=68&&kcd2vr::render_bounds63::valid(e.box)&&contains,
      "render envelope contains actual allowed wrists without an extra rig walk or bone change");
  Check(Same(fixture.live,fixture.original),"render envelope publication never changes the restored native pose");
 }
 F7NativeCurveTrialChecksH7();
 std::printf("active_adapter=%d/%d; native functions=IN_MEMORY_MOCKS; retail/headset=NOT_TESTED\n",tests-failures,tests);

 return failures?1:0;
}
