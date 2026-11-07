// Executes the CURRENT production shoulder admission, arm caller, FK,
// temporary-write/restore and motion-history acceptance blocks. The generated
// blocks are verbatim and their SHA receipt is retained with the host run.
// Native memory/contact and semantic owner providers are explicit fixture leaves.
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <string>
#include <vector>
#include "vrik/elbow_shoulder_d45.h"
#include "vrik/visual_selection59c7f.h"
#include "vrik/archer_pose169r2.h"
#include "vrik/shoulder_reach_d38.h"
#include "diagnostics/vrik_trace_h29.h"
namespace pc=kcd2vr::pose_core;
namespace diagnostics=kcd2vr::diagnostics;
constexpr std::size_t kPoseCapacity6689=9;
struct QuatT28 {float q[4]{0,0,0,1};float t[3]{};};
#include "arm_types_d47.inc"
#include "arm_math_d47.inc"
struct HandContact6695 {std::uint64_t epoch=0,sample=0;V3 allowed{};bool blocked=false;};
namespace ph {
struct Pose {pc::Quat q{};pc::Vec3 p{};};
struct Profile {bool isBow=false;bool bow()const{return isBow;}};
// This is the existing two-degree native admission limit, not a fixture solver.
inline double reach_max(double a,double b){return std::sqrt(a*a+b*b+2*a*b*std::cos(2*3.141592653589793/180));}
struct Output {bool owned[2]{},displayLatched[2]{},displayRejoinR13[2]{},bowHand169r2=false;Pose hands[2]{},displayHands[2]{};};
}
namespace kcd2vr::physics::hand_presence_h24 {
// Copied measured wrist provider: no native proxy exists in this host process.
inline pc::Quat wristRotation(bool owned,const ph::Pose& measured,pc::Quat current){return owned?measured.q:current;}
}
struct CoherentState6689 {
 std::uint32_t count=kPoseCapacity6689;std::array<int,kPoseCapacity6689> parent{},order{};
 std::array<QuatT28,kPoseCapacity6689> rest{},original{},work{},local{};
 std::array<bool,kPoseCapacity6689> dirty{},essential137{},pending137{};
 bool dirtyAll=true,deferPassengers137=false;std::uint64_t composedJoints=0,lastInvariantLog=0;
 V3 hingeNormalLocal[2]{},chestUpLocal{0,0,1},chestRightLocal{1,0,0},sideDownLocal[2]{};
 bool anatomyHand[2]{};Q4 boneToAnatomy[2]{};
 double upper[2]{.26628,.26628},fore[2]{.26153,.26153};
 bool reachLimited[2]{},armBlocked[2]{},armSaturated[2]{},allowedReady[2]{};V3 allowedGoal[2]{};
 float hoverW178r14[2]{},handoverW178r14[2]{};std::uint64_t naturalTick178r14[2]{},elbowPolicyD45[2]{};
 V3 lastBend[2]{};pc::ElbowContinuity elbow[2]{};pc::ForearmPronation pronation[2]{};
 kcd2vr::elbow_shoulder_d45::ShoulderState shoulderFitD45[2]{};
 pc::ReachAssist chestAssist[2]{},clavicleAssist[2]{};pc::ShoulderState103 shoulder[2]{};
 kcd2vr::archer169r2::State archer169r2{};HandContact6695 contact[2]{};
 kcd2vr::hand_smooth_h50::State handSmoothH50[2]{};bool handFreeH50[2]{};
};
static CoherentState6689 g_pose6689;
static std::array<QuatT28,kPoseCapacity6689> nativeBuffer;
static std::atomic<bool> g_pose_restore_fault6689{false},g_committed_pose_valid6689{false};
static std::atomic<std::uint64_t> g_pose_epoch6689{1},g_pose_policy_generation57{2},g_frame_control51{2};
static std::atomic<int> g_cal_state{2};
static std::uint64_t fixtureTick=10000;
static int failWrite=-1,writeCalls=0,probeQueries=0,ownerCalls=0,refuseOwnerCall=0;
static bool failOnce=false;
namespace diagnostic_trace {static std::vector<std::string> lines;static void Write(const char* m){lines.emplace_back(m);}}
static std::uint64_t GetTickCount64(){return fixtureTick;}
using PoseSetFn=void(*)(void*,int,const QuatT28*);
static void SetNative(void*,int id,const QuatT28* p){nativeBuffer[id]=*p;}
static bool WriteQuat(PoseSetFn fn,void* pose,int id,const QuatT28& p){++writeCalls;fn(pose,id,&p);if(id==failWrite&&!failOnce){failOnce=true;return false;}return true;}
#include "arm_history_type_d47.inc"
struct SmithContactHistoryR38 {}; // Native collision is a separate focused adapter fixture.
struct CoherentTransaction6689 {
 CoherentMotionHistoryD47 motionD47{};
 SmithContactHistoryR38 smithContactR38{};
 bool committed=false,restored=false,traceH29=false,traceRestoreOkH29=true;
 bool tracePublishAttemptedH29=false,reloadWorkingHandXR34=false,physicalActive=false,acquisitionRequired57=false;
 unsigned traceRestoreWritesH29=0;int line=0;const char* stage="HOST";
 std::uint64_t epoch=1,policyGeneration57=2,control51=2;void* pose=&nativeBuffer;PoseSetFn setter=SetNative;
 std::array<bool,kPoseCapacity6689> selected{},attempted{};
 diagnostics::vrik_h29::ArmContext traceArmH29[2]{};
 ph::Profile physicalProfiles[2]{};ph::Output physicalOutput{};
 struct {struct {bool carry146=false;} weapon[2];} physicalState;
 #include "arm_restore_d47.inc"
};
static bool CoherentOwnerCurrent57(const CoherentTransaction6689& tx){
 if(++ownerCalls==refuseOwnerCall)return false;
 #include "arm_ordinary_owner_d47.inc"
 return true; // Native acquired-owner provider is outside this arm fixture.
}
static bool RejectCoherent6693(CoherentTransaction6689& tx,int line){tx.line=line;return false;}
static bool g_contact_pass6695=true;
static bool SegmentBlockedWorld6692(void*,V3,V3,float,float&){++probeQueries;return false;}
static void ContactCommit6695(HandContact6695& c,void*,V3 v){c.allowed=v;}
static void SmithAnvilContactA2(void*,std::uint64_t,int,const CoherentState6689&,Q4,Q4&,V3&,SmithContactHistoryR38*,Q4*){}
#include "arm_fk_d47.inc"
static RigMap rig;
static int checks=0,failures=0;
static void ck(bool value,const char* name){++checks;if(!value){++failures;std::printf("FAIL %s\n",name);}}
static bool sameNative(){return std::memcmp(nativeBuffer.data(),g_pose6689.original.data(),sizeof(nativeBuffer))==0;}
static void init(Q4 world={}){
 g_pose6689=CoherentState6689{};auto& s=g_pose6689;rig={};
 rig.spine3=0;rig.head=1;rig.upper_r=2;rig.fore_r=3;rig.hand_r=4;
 rig.upper_l=5;rig.fore_l=6;rig.hand_l=7;rig.pelvis=8;
 s.parent={-1,0,0,2,3,0,5,6,-1};for(int i=0;i<9;++i)s.order[i]=i;
 const V3 positions[9]{{0,0,1.4f},{0,0,1.65f},{.18f,0,1.4f},
  {.44628f,0,1.4f},{.603198f,.209224f,1.4f},{-.18f,0,1.4f},
  {-.44628f,0,1.4f},{-.603198f,.209224f,1.4f},{0,0,.8f}};
 for(int i=0;i<9;++i){QToQuatT(s.rest[i],world);ToT(s.rest[i],QRotateV(world,positions[i]));}
 for(int side=0;side<2;++side){s.hingeNormalLocal[side]={0,0,side?-1.f:1.f};s.sideDownLocal[side]=Normalize({side?-.2f:.2f,-.45f,-1});}
 s.original=s.rest;nativeBuffer=s.rest;g_pose_restore_fault6689=false;g_pose_epoch6689=1;g_pose_policy_generation57=2;g_frame_control51=2;g_cal_state=2;fixtureTick=10000;
 failWrite=-1;failOnce=false;writeCalls=probeQueries=ownerCalls=refuseOwnerCall=0;
 kcd2vr::frik_elbow_lab::enabled=false; // Explicit preference; F7 is never needed.
}
// This boundary supplies already-admitted endpoints. It contains no duplicate
// arm math: all reach/plane/filter/FK/hand orientation code below is production.
static bool prepare(CoherentTransaction6689& tx,const TrackingSnapshot& ts,const V3 (&target)[2],int policy,
                    bool physical=false,bool prop=false,bool presence=false,bool door=false,bool contacts=false){
 auto& s=g_pose6689;void* character=&s;void* pose=tx.pose;const std::uint32_t count=s.count;
 s.original=s.rest;s.work=s.rest;nativeBuffer=s.original;s.dirtyAll=true;
 for(int i=0;i<int(count);++i)s.local[i]=s.parent[i]<0?s.rest[i]:RelativeFromAbsolute(s.rest[s.parent[i]],s.rest[i]);
 if(!Compose6689(s))return false;
 const bool valid[2]{true,true},collisionMode=contacts;const int rung=3;
 const Q4 frame{};Q4 visQH50[2]{ts.rq,ts.lq};const V3 requestedGoals[2]{target[0],target[1]};V3 goals[2]{target[0],target[1]};
 const bool prepared169r2=false,manualPose169r2=false,bowOwnsReachH17=false,nativeFingerCapture=false,readingVisual32=false;
 const bool nativePassengerHand46[2]{},propHeld5916c1[2]{prop,prop},presenceOwnedH24[2]{presence,presence},doorPose66[2]{door,door};
 ph::Pose propWrist5916c1[2]{},presenceWristH24[2]{};pc::LocalPose doorWrist66[2]{};
 const bool freeContact164R1[2]{contacts,contacts};const double archerWeight169r2=0;
 const auto desired=pc::from_rotation_vector({.27,-.31,.43});
 for(int side=0;side<2;++side){
  tx.physicalOutput.owned[side]=physical;tx.physicalOutput.hands[side].q=desired;
  propWrist5916c1[side].q=presenceWristH24[side].q=doorWrist66[side].rotation=desired;
  s.allowedReady[side]=contacts;
 }
 tx.physicalActive=physical;
 pc::ElbowContinuity stagedElbow[2]{s.elbow[0],s.elbow[1]};
 auto stagedArcher169r2=s.archer169r2;
 kcd2vr::elbow_shoulder_d45::ShoulderState stagedShoulderFitD45[2]{s.shoulderFitD45[0],s.shoulderFitD45[1]};
 std::uint64_t stagedElbowPolicyD45[2]{s.elbowPolicyD45[0],s.elbowPolicyD45[1]};
 const std::uint64_t elbowPolicyD45=policy==2?7:0;
 pc::ForearmPronation stagedPronation[2]{s.pronation[0],s.pronation[1]};
 pc::ReachAssist stagedClavicle[2]{s.clavicleAssist[0],s.clavicleAssist[1]};
 pc::ShoulderState103 stagedShoulder[2]{s.shoulder[0],s.shoulder[1]};
 auto& stagedContact=tx.motionD47.contact;std::copy_n(s.contact,2,stagedContact);
 kcd2vr::hand_smooth_h50::State stagedH50[2]{s.handSmoothH50[0],s.handSmoothH50[1]};
 bool freeNowH50[2]{true,true};V3 stagedBend[2]{s.lastBend[0],s.lastBend[1]},ikAllowed113[2]{};
 float stagedHandoverD47[2]{s.handoverW178r14[0],s.handoverW178r14[1]};
 std::uint64_t stagedNaturalTickD47[2]{ts.tick,ts.tick};
 #include "arm_reach_d47.inc"
 for(int side=0;side<2;++side){
  const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r,h=side?rig.hand_l:rig.hand_r;
  const Q4 chest=QFromQuatT(s.work[rig.spine3]);const V3 sideDown=QRotateV(chest,s.sideDownLocal[side]);V3 shoulder=FromT(s.work[u]);
  #include "arm_caller_d47.inc"
 }
 if(!Compose6689(s))return false;
 tx.selected.fill(true);tx.selected[rig.head]=tx.selected[rig.spine3]=tx.selected[rig.pelvis]=false;
 #include "arm_write_d47.inc"
 #include "arm_stage_d47.inc"
 return true;
}
static void publish(CoherentTransaction6689& tx,bool paired=true){
 const auto rejectPublish169r2=[&](unsigned line){tx.line=int(line);g_committed_pose_valid6689=false;};
 #include "arm_publish_guard_d47.inc"
 const bool pairedFramePublishedR38=paired;
 #include "arm_late_guard_d47.inc"
 auto& s=g_pose6689;
 #include "arm_publish_d47.inc"
}
static TrackingSnapshot sample(std::uint64_t sequence){TrackingSnapshot t{};t.sequence=sequence;t.tick=10000+11*sequence;t.timeNs=t.tick*1000000;fixtureTick=t.tick;t.right_valid=t.left_valid=t.head_valid=t.allowed=true;return t;}
static CoherentMotionHistoryD47 snapshot(){CoherentMotionHistoryD47 h{};auto& s=g_pose6689;
 for(int side=0;side<2;++side){h.bend[side]=s.lastBend[side];h.elbow[side]=s.elbow[side];h.pronation[side]=s.pronation[side];h.shoulderFit[side]=s.shoulderFitD45[side];h.elbowPolicy[side]=s.elbowPolicyD45[side];h.naturalTick[side]=s.naturalTick178r14[side];h.handover[side]=s.handoverW178r14[side];h.contact[side]=s.contact[side];h.clavicle[side]=s.clavicleAssist[side];h.shoulder[side]=s.shoulder[side];h.smooth[side]=s.handSmoothH50[side];h.free[side]=s.handFreeH50[side];}h.archer=s.archer169r2;return h;}
static bool sameHistory(const CoherentMotionHistoryD47& a){const auto b=snapshot();return std::memcmp(&a,&b,sizeof(a))==0;}
static void transactionCases(){
 const V3 target[2]{{.45f,.23f,1.1f},{-.45f,.23f,1.1f}};
 for(int failure=0;failure<12;++failure){
  init();auto first=sample(1);CoherentTransaction6689 accepted;
  ck(prepare(accepted,first,target,2),"initial actual arm/native write succeeds");accepted.Restore();ownerCalls=0;publish(accepted);
  ck(accepted.motionD47.published&&g_pose6689.elbow[0].sample==1,"history accepted only after restored paired publication");
  const auto history=snapshot();auto next=sample(2);next.rq={.1f,.2f,.05f,.9733961f};CoherentTransaction6689 tx;
  if(failure==1)failWrite=rig.hand_r;
  const bool ok=prepare(tx,next,target,2);
  ck(sameHistory(history),"temporary write and complete staged solve do not change persistent history");
  if(failure==1){ck(!ok&&sameNative(),"failed setter restores every attempted native bone");}
  else ck(ok,"host boundary reaches successful temporary native write");
  if(failure==2){failWrite=rig.hand_l;failOnce=false;}
  if(failure!=3)tx.Restore();
  ownerCalls=0;refuseOwnerCall=failure==4?1:failure==5?2:0;
  if(failure==7)g_pose_epoch6689=2;
  if(failure==8)g_pose_policy_generation57=3;
  if(failure==9)g_pose_policy_generation57=4;
  if(failure==10)g_frame_control51=4;
  if(failure==11)g_cal_state=1;
  publish(tx,failure!=6);
  if(failure==0)ck(tx.motionD47.published&&g_pose6689.elbow[0].sample==2,"successful current pair accepts staged temporal state");
  else ck(!tx.motionD47.published&&sameHistory(history),"write/restore/owner/pair/epoch/policy/control/calibration refusal preserves prior history");
  tx.Restore();ck(sameNative(),"native source buffer restored independently of publication result");
 }
}
static void geometricCases(){
 double maxWrist=0,maxLength=0,maxOrientation=0,maxRepeat=0;bool admitted=true,headSame=true,restored=true,historyOnce=true;
 for(int policy=0;policy<3;++policy)for(int worldMode=0;worldMode<2;++worldMode){
  const Q4 world=worldMode?QP(pc::from_rotation_vector({.13,-.21,.37})):Q4{};init(world);
  kcd2vr::frik_elbow_lab::enabled=policy==1;
  std::uint64_t sequence=0;
  for(double distance:{.0,.015,.08,.30,.523,.527,.75})for(int n=0;n<180;++n){
   ++sequence;auto t=sample(sequence);const double angle=2*pc::armPi*n/180;
   V3 target[2]{};
   for(int side=0;side<2;++side){const auto direction=*pc::unit(pc::Vec3{(side?-1:1)*std::cos(angle),std::sin(angle),.35*std::sin(2*angle)});
    target[side]=Add(FromT(g_pose6689.rest[side?rig.upper_l:rig.upper_r]),QRotateV(world,VP(direction*distance)));}
   CoherentTransaction6689 tx;const bool ok=prepare(tx,t,target,policy);admitted=admitted&&ok;if(!ok){std::printf("REFUSE policy=%d distance=%.6f n=%d line=%d\n",policy,distance,n,tx.line);continue;}
   const auto solved=g_pose6689.work;
   headSame=headSame&&std::memcmp(&solved[rig.head],&g_pose6689.original[rig.head],sizeof(QuatT28))==0;
   for(int side=0;side<2;++side){const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r,h=side?rig.hand_l:rig.hand_r;
    maxWrist=std::max(maxWrist,double(Len(Sub(FromT(solved[h]),target[side]))));
    maxLength=std::max({maxLength,std::abs(double(Len(Sub(FromT(solved[f]),FromT(solved[u]))))-g_pose6689.upper[side]),std::abs(double(Len(Sub(FromT(solved[h]),FromT(solved[f]))))-g_pose6689.fore[side])});
    maxOrientation=std::max(maxOrientation,pc::angle(PQ(QFromQuatT(solved[h])),PQ(DesiredHand6689(g_pose6689,side,{},side?t.lq:t.rq))));
   }
   tx.Restore();publish(tx);restored=restored&&sameNative();
   if(n%30==0){const auto updates=g_pose6689.elbow[0].updates;const auto before=snapshot();
    for(int repeat=0;repeat<3;++repeat){CoherentTransaction6689 again;const bool repeatOk=prepare(again,t,target,policy);admitted=admitted&&repeatOk;
     maxRepeat=std::max(maxRepeat,double(Len(Sub(FromT(g_pose6689.work[rig.hand_r]),FromT(solved[rig.hand_r])))));
     again.Restore();publish(again);historyOnce=historyOnce&&g_pose6689.elbow[0].updates==updates;}
    // Every duplicated source sample keeps the elbow integrator exactly once.
    historyOnce=historyOnce&&g_pose6689.elbow[0].sample==before.elbow[0].sample;
   }
  }
 }
 std::printf("D47 production arm sweep maxWristM=%.12g maxBoneLengthErrorM=%.12g maxWristOrientationRad=%.12g maxDuplicateWristM=%.12g\n",maxWrist,maxLength,maxOrientation,maxRepeat);
 ck(admitted,"all baseline/FRIK/D45 mirrored near/far/full-circle admitted endpoints solve");
 ck(maxWrist<.000002,"actual float FK wrist tracks admitted target within 2 micrometres without smoothing");
 ck(maxLength<.000001,"actual composed arms retain both fixed segment lengths");
 ck(maxOrientation<.000001,"actual composed wrist retains explicit controller orientation");
 ck(headSame,"all arm paths preserve head pose bytes");ck(restored,"every sampled native buffer restores");
 ck(historyOnce&&maxRepeat<.000002,"duplicate consumers do not integrate arm history or move wrist");
 ck(probeQueries==0,"F7-off arm solves perform zero diagnostic scene probes");
}
static void finiteAngleCases(){
 double maxTinyWrist=0,maxTinyElbow=0;bool accepted=true,head=true;
 // A legal native arm already close to the requested pose is the missing
 // case: an identity deadband in swing then leaves the wrist below its target.
 for(int policy=0;policy<3;++policy)for(double radians:{1e-8,.00005,.0005,.0013,.0014141,.0014143,.0016}){
  init();kcd2vr::frik_elbow_lab::enabled=policy==1;
  V3 target[2]{{.42f,.25f,1.1f},{-.42f,.25f,1.1f}};
  auto t=sample(1);CoherentTransaction6689 first;accepted=prepare(first,t,target,policy)&&accepted;
  first.Restore();publish(first);
  // Native boundary supplies a legal pose with exactly the same fixed lengths.
  // No calibration measurement or production state is synthesized from it.
  g_pose6689.rest=g_pose6689.work;
  const auto previousHead=g_pose6689.rest[rig.head];
  for(int side=0;side<2;++side){const auto shoulder=PV(FromT(g_pose6689.rest[side?rig.upper_l:rig.upper_r]));
   target[side]=VP(shoulder+pc::rotate(pc::from_rotation_vector({0,0,radians}),PV(target[side])-shoulder));}
  t=sample(2);CoherentTransaction6689 second;accepted=prepare(second,t,target,policy)&&accepted;
  for(int side=0;side<2;++side){const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r,h=side?rig.hand_l:rig.hand_r;
   maxTinyWrist=std::max(maxTinyWrist,double(Len(Sub(FromT(g_pose6689.work[h]),target[side]))));
   const auto solved=pc::solve_arm(PV(FromT(g_pose6689.work[u])),PV(target[side]),g_pose6689.upper[side],g_pose6689.fore[side],
       second.motionD47.elbow[side].bend,second.motionD47.elbow[side].bend,policy==0?-1.:kcd2vr::elbow_shoulder_d45::reserve(g_pose6689.upper[side],g_pose6689.fore[side]));
   if(solved)maxTinyElbow=std::max(maxTinyElbow,pc::length(PV(FromT(g_pose6689.work[f]))-solved->elbow));
  }
  head=head&&std::memcmp(&previousHead,&g_pose6689.work[rig.head],sizeof previousHead)==0;
  second.Restore();publish(second);
 }
 std::printf("D47 near-aligned production arm maxWristM=%.12g maxElbowM=%.12g\n",maxTinyWrist,maxTinyElbow);
 ck(accepted&&maxTinyWrist<.000002&&maxTinyElbow<.000002,"sub-0.081-degree arm rotations reach the actual composed controller wrist and elbow");
 ck(head,"finite-angle arm correction never changes HMD/head pose");
}
static void ownerCases(){
 const V3 target[2]{{.42f,.25f,1.1f},{-.42f,.25f,1.1f}};
 for(int owner=0;owner<5;++owner){init();auto t=sample(1);CoherentTransaction6689 tx;
  ck(prepare(tx,t,target,0,owner==1,owner==2,owner==3,owner==4,true),"actual arm solver accepts explicit current wrist owner");
  const auto wanted=owner?pc::from_rotation_vector({.27,-.31,.43}):PQ(DesiredHand6689(g_pose6689,0,{},t.rq));
  ck(Len(Sub(FromT(g_pose6689.work[rig.hand_r]),target[0]))<.000002&&pc::angle(PQ(QFromQuatT(g_pose6689.work[rig.hand_r])),wanted)<.000001,
      "controller/contact goal and physical/prop/presence/door orientation survive elbow reconstruction");
  tx.Restore();publish(tx);ck(sameNative(),"owned wrist path restores native animation buffer");
 }
}
int main(){transactionCases();geometricCases();finiteAngleCases();ownerCases();std::printf("D47 live arm/transaction adapter: %d/%d checks PASS; native ABI/render/headset not executed.\n",checks-failures,checks);return failures?1:0;}
