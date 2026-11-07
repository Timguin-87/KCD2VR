#include <cstdlib> // abort if the immediate collision leaf ever stages deferred work
#include "../src/frik_pose_probe.h"
using kcd2vr::frik_pose_probe::FullWeaponMode120;
using kcd2vr::frik_pose_probe::FocusedTestMode167;
namespace kcd2vr::frik_pose_probe { int FocusedTestMode167() noexcept {return 0;} }
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
#include "../src/vrik/kick176.h"
#include "../src/vrik/elbow_continuity.h"
#include "../src/vrik/arm_constraints.h"
#include "../src/vrik/upper_body.h"
#include "../src/vrik/finger_surface.h"
#include "../src/core/snapshot_mailbox.h"
// The real SHA gate is separately exercised by retail_hash112; this fixture
// has no retail module and must never enable the new comparison adapter.
namespace retail_build112 {inline bool Ready(std::uintptr_t) noexcept {return false;}}
#define __try try
#define __except(x) catch(...)
struct QuatT28 {float q[4];float t[3];};
using PoseGetFn=const QuatT28*(*)(void*,int);using PoseSetFn=void(*)(void*,int,const QuatT28*);
#include "generated_adapter_types.h"
struct FingerChainV6528{std::array<int,6>id{};int count{};};
constexpr int Count=720;
struct Fixture{std::array<QuatT28,Count> original{},reference{},local{},live{};std::array<int,Count> parent{};};
static Fixture fixture;
static std::array<unsigned char,Count*0x30> nativeJointRecords;
static int failAt=-1,writeCalls=0;static bool failOnce=false;
static std::vector<std::string> logs;
namespace diagnostic_trace {void Write(const char*m){logs.emplace_back(m);}}
namespace animation_filter { inline std::atomic<std::uint64_t> combatPoseTick112{0};struct PoseReferenceState{bool closed[2]{},relaxed[2]{};std::uint64_t serial[2]{};};PoseReferenceState refs{};bool referenceAvailable=false;
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
template<class T>bool Read(const void*p,T&out){if(!p)return false;if(p==reinterpret_cast<const unsigned char*>(&fixture)+8&&sizeof(T)==sizeof(void*)){void* j=nativeJointRecords.data();std::memcpy(&out,&j,sizeof(j));return true;}std::memcpy(&out,p,sizeof(out));return true;}
static bool ReadParentDirect(void*,int j,int&p){p=fixture.parent.at(j);return true;}
static bool ReadCompanion(void*,int j,QuatT28&out){out=fixture.reference.at(j);return true;}
static bool GetCachedFingerChainsV6546(void*,PoseGetFn,void*,std::uint32_t,const RigMap&,bool left,std::array<FingerChainV6528,5>&out){
 for(int c=0;c<5;++c){out[c].count=3;for(int j=0;j<3;++j)out[c].id[j]=(left?30:15)+c*3+j;}return true;}
static const QuatT28* MockGet(void*,int j){return &fixture.live.at(j);}
static void MockSet(void*,int j,const QuatT28*q){fixture.live.at(j)=*q;}
static bool ValidateBuffer1Setter(void*,PoseSetFn&set){set=MockSet;return true;}
static bool WriteQuat(PoseSetFn set,void* pose,int j,const QuatT28&value){++writeCalls;set(pose,j,&value);if(j==failAt&&!failOnce){failOnce=true;return false;}return true;}
static std::uintptr_t WhBase(){return 0;}
static bool mockCollision=false;static float mockWallX=.45f;
static bool mockSharedBudget124=false;static int handQueries124=0,weaponQueries124=0;
static bool CollisionBinding6692(std::uintptr_t){return mockCollision;}
static bool ConstrainHandWorld6692(void*,V3,V3 requested,V3& allowed,float& hit){
 allowed=requested;hit=0;if(!mockCollision)return false;if(requested.x>mockWallX){allowed.x=mockWallX;hit=.1f;}return true;}
static bool SegmentBlockedWorld6692(void*,V3,V3,float,float& hit){hit=0;return false;}
static void CollisionStatus6692(std::uint64_t,const char*){}
#include "../src/vrik/hand_contact_types.inl"
static bool g_contact_pass6695=false,g_collision_fan_valid6693=true;
struct ContactPass6695{ContactPass6695(bool enabled,std::uint64_t now,std::uint64_t&last,std::uint64_t=0){g_contact_pass6695=enabled;last=now;}~ContactPass6695(){g_contact_pass6695=false;}};
static bool ContactHand6695(HandContact6695&,void* c,std::uint64_t,V3 native,V3 request,V3&allowed,const V3* =nullptr,const FingerContactGeometry103* =nullptr,HandContactPending6695* pending=nullptr){if(pending)pending->ready=false;if(mockSharedBudget124)handQueries124+=9;float hit=0;return ConstrainHandWorld6692(c,native,request,allowed,hit);}
// This pose-only fixture keeps its existing immediate native-collision leaf.
// The source-connected world adapter executes and tests real deferred work.
static bool CompleteHandContact6695(HandContact6695&,HandContactPending6695&,V3&){std::abort();}

static void ContactCommit6695(HandContact6695&,void*,V3){}

static std::atomic<bool>g_body_cal_ready{false},g_pose_restore_fault6689{false},g_committed_pose_valid6689{false};
static std::atomic<std::uint64_t>g_contact_control_epoch6695{1};
static std::atomic<std::uint64_t>g_pose_epoch6689{1};static std::atomic<int>g_cal_state{2};
static std::atomic<float> g_cal_frame_qx{0},g_cal_frame_qy{0},g_cal_frame_qz{0},g_cal_frame_qw{1},g_cal_hq_x{0},g_cal_hq_y{0},g_cal_hq_z{0},g_cal_hq_w{1};
static std::atomic<float>g_cal_native_head_x{0},g_cal_native_head_y{0},g_cal_native_head_z{0},g_cal_head_x{0},g_cal_head_y{0},g_cal_head_z{0};
static bool ControllerHeadAnchoredArmGoal(const TrackingSnapshot&,const QuatT28&,bool,V3&){return true;}
static std::atomic_flag g_f12_hand_delta_lock=ATOMIC_FLAG_INIT;
static float g_committed_model_world6689[8]{};static QuatT28 g_f12_native_hand[2]{},g_f12_vr_hand[2]{};static bool g_f12_hand_delta_valid[2]{};
static kcd2vr::pose_core::Stamp g_committed_stamp6689;static std::atomic<void*>g_committed_character6689{nullptr};
struct CommittedFrame6695{kcd2vr::pose_core::Stamp stamp;QuatT28 native[2],allowed[2];float world[8];bool valid[2];
 V3 nativeHead122{},solvedHead122{},targetHead122{},pelvis122{};bool headEvidence122=false;};
static kcd2vr::core::SnapshotMailbox<CommittedFrame6695> g_committed_frame6695;
// The native equipment/contact/publication edges are mocked. The physical
// kernel and its actual coherent-pose callsite are not replaced.
#include "../src/vrik/physical_weapon.h"
namespace ph=kcd2vr::physical;
static kcd2vr::core::SnapshotMailbox<ph::RejectedBowRelease166> g_bow_release_rejected166;
struct PhysEquipment6698{};
struct PhysCorrelation126{}; // native provenance leaf is tested by the complete-TU fixture
static ph::State g_phys_state6698;static std::array<ph::HolsterState,8> g_phys_holsters6698;
static std::atomic<std::uint64_t> g_physical_gates101R2[10]{};
static std::atomic<int>g_weapon_control_mode{0};static std::atomic<bool>g_phys_abort6698{false};
static std::array<ph::Profile,2> mockProfiles{};static bool mockPhysical=false;static int physicalPublications=0;
static ph::Output mockPhysicalOutput{};
static void PhysInputWorld6698(ph::Input&,void*){}
static bool PhysPrepare6698(ph::Input& i,std::array<ph::Profile,2>& p,PhysEquipment6698&,std::array<PhysCorrelation126,2>*){p=mockProfiles;return mockPhysical;}
static ph::Contact103 PhysSweep6698(void*,const ph::Profile&,ph::Pose,ph::Pose,bool){ph::Contact103 c{};if(mockSharedBudget124){weaponQueries124=handQueries124+5;if(weaponQueries124>18){c.known=false;c.fraction=0;}}return c;}
static void PhysHolsterGestures6698(const ph::Input&,const PhysEquipment6698&,ph::Output&,std::array<ph::HolsterState,8>&){}
static void PhysPublish6698(const ph::Input&,const ph::Output& o,const std::array<ph::Profile,2>&,const PhysEquipment6698&,const float*,ph::State& s,std::array<ph::HolsterState,8>&,const std::array<PhysCorrelation126,2>*){++physicalPublications;mockPhysicalOutput=o;g_phys_state6698=s;}
#include "calibration_pose_shim.h"
static void PhysCaptureSamePose101(void*,const QuatT28*,std::uint32_t,const RigMap&,const TrackingSnapshot&,const QuatT28*,const int*) noexcept {}
static void FistPublish101(const TrackingSnapshot&,const CommittedFrame6695&,const bool*) noexcept {}
static bool NativeHeldFingerKey101(int,std::uint64_t& key) noexcept {key=0;return false;}
// V66.137: archer stance (V127) reads the published physical frame; the fixture has none.
struct PhysFrame6698{ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};};
struct PhysFrameMailboxStub{bool read(PhysFrame6698&)const noexcept{return false;}};static PhysFrameMailboxStub g_phys_frame6698;
// V66.146R1 globals owned by physical_native.inl (not linked into this fixture): simplified RT bow, default zone.
static std::atomic<bool> g_bow_manual_reload146{false},g_bow_native_grab146{false},g_bow_string_trigger160{false},g_experiments169{false};static std::atomic<float> g_holster_radius136{.28f};
// Native ray leaf only; the actual coherent kick lifecycle/caller remains production.
static unsigned mockKickQueries176=0;
static kcd2vr::kick176::Contact mockKickContact176{};
static kcd2vr::pose_core::Vec3 mockKickFrom176{},mockKickTo176{};
static void (*mockKickQueryHook176)()=nullptr;
static kcd2vr::kick176::Contact ProbeKickContact176(void*,kcd2vr::pose_core::Vec3 from,kcd2vr::pose_core::Vec3 to,int* =nullptr,std::uintptr_t* =nullptr) noexcept { // C37: + material/collider out
    ++mockKickQueries176;mockKickFrom176=from;mockKickTo176=to;
    if(mockKickQueryHook176)mockKickQueryHook176();
    return mockKickContact176;
}
static std::atomic<bool> g_native_busy178{false};static std::atomic<std::uint64_t> g_native_busy_tick178{0};static std::atomic<float> g_strike_min_scale178{.35f};static std::atomic<unsigned> g_strike_min_interval178{550}; // V66.178
static std::atomic<float> g_fatigue_ramp59c14{.6f},g_pole_one_hand59c14{.40f};static std::atomic<bool> g_strike_min_scale_gate59c14{false}; // R59.16C10R14
static std::atomic<bool> g_intent_pi59c16{false},g_two_hand_axis59c16{false}; // R59.16C10R16
static std::atomic<float> g_wrist_from_grip59c7f{0.f};static std::atomic<bool> g_auto_palm59c8{false};static std::atomic<float> g_head_raise_max59c9{.05f};static std::atomic<float> g_auto_palm_last_cm59c8[2]{0.f,0.f}; // R59.16C7F/C8 host
#include "../src/vrik/coherent_pose.inl"
static int tests=0,failures=0;
static void Check(bool p,const char*n){++tests;if(!p)++failures;std::printf("%s %s\n",p?"PASS":"FAIL",n);}
static RigMap rig;
static std::array<unsigned char,0xA40>character{};static std::array<std::uintptr_t,100>vtable{};
static bool Same(const std::array<QuatT28,Count>&a,const std::array<QuatT28,Count>&b){return std::memcmp(a.data(),b.data(),sizeof(a))==0;}
static void Init(){
 g_cal_frame_qx=g_cal_frame_qy=g_cal_frame_qz=0;g_cal_frame_qw=1;
 g_cal_hq_x=g_cal_hq_y=g_cal_hq_z=0;g_cal_hq_w=1;
 g_cal_head_x=g_cal_head_y=g_cal_head_z=0;
 g_finger_capture66100=false;g_finger_saved_generation66100=0;g_capture_runtime66100={};
 g_arm_profile66100.publish(kcd2vr::calibration_profile::Arms{}); // default: no valid retarget profile
 g_weapon_control_mode=0;g_phys_abort6698=false;g_phys_state6698={};g_phys_holsters6698={};mockProfiles={};mockPhysical=false;physicalPublications=0;
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
 for(int j=48;j<Count;++j){fixture.parent[j]=j<260?6:(j<560?(j%2?4:3):(j<640?0:j-1));fixture.local[j].t[0]=.001f*(j%7);fixture.local[j].t[2]=.002f;}
 for(int i=0;i<Count;++i)fixture.reference[i]=fixture.parent[i]<0?fixture.local[i]:AbsoluteFromRelative(fixture.reference[fixture.parent[i]],fixture.local[i]);
 fixture.original=fixture.live=fixture.reference;g_cal_native_head_z=fixture.reference[6].t[2];
 auto*vf=vtable.data();vtable[0x190/8]=0x423C30;std::memcpy(character.data(),&vf,sizeof(vf));const float world[8]{0,0,0,1,100,200,10,1};std::memcpy(character.data()+0xA08,world,sizeof(world));
 g_pose6689.~CoherentState6689(); new (&g_pose6689) CoherentState6689;g_cal_state=2;g_pose_epoch6689=1;g_pose_restore_fault6689=false;g_committed_pose_valid6689=false;failAt=-1;failOnce=false;writeCalls=0;
 animation_filter::referenceAvailable=false;animation_filter::curveAvailable=false;animation_filter::curveSamples=0;animation_filter::curveStatus=0;g_finger_acquire6694={};g_finger_loaded6694=0;g_finger_save_pending6694=false;mockCollision=false;mockWallX=.45f;
}
static TrackingSnapshot Tracking(){TrackingSnapshot t{};t.tick=10000;t.sequence=2;t.allowed=t.head_valid=t.left_valid=t.right_valid=true;
 // cry right=(.38,.22,1.20), left=(-.38,.22,1.20); headAtCalibration=1.64.
 const float z=1.20f-fixture.reference[6].t[2];t.right={.38f,z,-.22f};t.left={-.38f,z,-.22f};return t;}
static int lastFail=0;static bool Run(TrackingSnapshot t,int rung=3,bool publish=false){CoherentTransaction6689 tx;bool ok=BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,rung,t);lastFail=tx.failureLine;tx.Restore();if(publish)PublishCommitted6689(tx);return ok;}
#include <chrono>

int main(){
    std::array<std::array<QuatT28,Count>,9> solvedFull{}, solvedDeferred{};
    for(int pass=0;pass<2;++pass){
        Init();
        g_arm_profile66100.publish(kcd2vr::calibration_profile::Arms{1.50,.75,.75,.20});
        g_compose_defer137=(pass==1);
        auto t=Tracking();
        for(int i=0;i<9;++i){
            t.sequence++;t.tick+=11;
            t.head.x=.04f*std::sin(float(i)*.31f);
            t.head.y=.02f*std::cos(float(i)*.23f);
            t.hq={0.f,std::sin(float(i)*.017f),0.f,std::cos(float(i)*.017f)};
            t.right.x=.38f+.03f*std::sin(float(i)*.27f);
            t.left.x=-.38f+.02f*std::cos(float(i)*.19f);
            if(!Run(t,3)){std::printf("FAIL solve pass=%d i=%d line=%d\n",pass,i,lastFail);return 2;}
            std::copy_n(g_pose6689.work.begin(),Count,(pass?solvedDeferred:solvedFull)[i].begin());
        }
    }
    int mismatches=0;
    for(int i=0;i<9;++i){
        if(std::memcmp(solvedFull[i].data(),solvedDeferred[i].data(),sizeof(solvedFull[i]))!=0){
            ++mismatches;
            int first=-1;
            for(int j=0;j<Count;++j)if(std::memcmp(&solvedFull[i][j],&solvedDeferred[i][j],sizeof(QuatT28))!=0){first=j;break;}
            std::printf("MISMATCH pose=%d firstJoint=%d\n",i,first);
        }
    }
    std::printf("pose_equiv137=%d/9 byte_identical=%d\n",9-mismatches,mismatches==0);
    return mismatches?1:0;
}
