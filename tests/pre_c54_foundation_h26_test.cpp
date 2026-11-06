// Actual production pose fixture. Only unavailable native/service leaves are OFF.
#include "vrik/npc_contact172.h"
#include "subsystems/physics/runtime/native_hand_presence_pose_h24.h"
#include "core/snapshot_mailbox.h"
namespace npc172=kcd2vr::npc_contact172;
namespace handPresenceH24=kcd2vr::physics::hand_proxy60;
namespace handPolicyH24=kcd2vr::native_hand_proxy_t4;
struct HandPresenceSourceH24 {handPolicyH24::Input input{};bool valid=false;};
static kcd2vr::core::SnapshotMailbox<HandPresenceSourceH24> g_hand_presence_source_h24[2];
static bool HandPresenceContextH24(const handPolicyH24::Input&) noexcept{return false;}
static bool HandPresenceIdentityH24(void*,std::uint64_t,handPolicyH24::Input&) noexcept{return false;}
namespace kcd2vr::physics::hand_proxy60 {
void CancelPresenceH24(int) noexcept{}
bool ReadPresenceFeedbackH24(int,PresenceFeedbackH24&) noexcept{return false;}
bool SubmitPresenceH24(int,const native_hand_proxy_t4::Input&) noexcept{return false;}
}
template<class... A> static bool NativeActivityHandOwned46(A...) noexcept{return false;}
namespace kcd2vr::touch_react_service60 {template<class... A> void publishPlan(A&&...) noexcept{}}
// Actual coherent_pose.inl and physical kernel, retaining the existing native
// getter/setter leaves; this fixture adds real two-leg topology and direct
// endpoint expectations instead of the retired shoulder-ratio contract.
#include <cstdint>
static std::uint64_t frameClock51=10000;
#define KCD2VR_POSE_FIXTURE_CLOCK frameClock51
#define KCD2VR_POSE_FIXTURE_COUNT 64
#define main historical_pose_adapter51
#define KCD2VR_POSE_FIXTURE_EXTERNAL_PRESENCE_H24
#include "pose_adapter_test.cpp"
#undef main

namespace tf=kcd2vr::tracked_frame51;
static kcd2vr::frik_pose_probe::GameStateContext poseContext55{};
static bool refreshPoseContext55=true;
static void InitFrame51(bool smooth=false){
    Init();g_frame_control51=smooth?3:2;g_body_follow128=false;
    animation_filter::nativeBodyOwned112=false;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_ctx_reading=false;
    poseContext55={};refreshPoseContext55=true;g_tool_context19c4r2=0;mockModelWorldHook55=nullptr;
    g_eye_forward115=g_eye_up115=0;g_head_raise_max59c9=.40;frameClock51=10000;
    for(int side=0;side<2;++side){const int hip=47+side*4;const float sign=side?-1.f:1.f;
        fixture.parent[hip]=rig.pelvis;fixture.local[hip].t[0]=sign*.09f;fixture.local[hip].t[1]=0;fixture.local[hip].t[2]=-.04f;
        for(int n=1;n<=3;++n){fixture.parent[hip+n]=hip+n-1;fixture.local[hip+n].t[0]=0;
            fixture.local[hip+n].t[1]=n==1?.025f:n==2?-.025f:.06f;fixture.local[hip+n].t[2]=n==3?-.07f:-.36f;}
    }
    for(int j=0;j<Count;++j)fixture.reference[j]=fixture.parent[j]<0?fixture.local[j]:AbsoluteFromRelative(fixture.reference[fixture.parent[j]],fixture.local[j]);
    fixture.original=fixture.live=fixture.reference;
    g_cal_native_head_x=fixture.reference[rig.head].t[0];g_cal_native_head_y=fixture.reference[rig.head].t[1];g_cal_native_head_z=fixture.reference[rig.head].t[2];
    g_pose6689.ready=false;g_graspAcquisition54=nullptr;g_graspPendingUse55=nullptr;(void)g_tracked_frame51.publish({});
}
static void NativePelvis51(V3 offset){
    fixture.original=fixture.reference;
    for(int j=0;j<Count;++j){int p=j;while(p>=0&&p!=rig.pelvis)p=fixture.parent[p];
        if(p==rig.pelvis){for(int c=0;c<3;++c)fixture.original[j].t[c]+=c==0?offset.x:c==1?offset.y:offset.z;}}
    fixture.live=fixture.original;
}
static bool TickFrame51(TrackingSnapshot& t,bool publish=true){++t.sequence;t.tick=frameClock51+=10;t.timeNs=t.tick*1000000;
    if(refreshPoseContext55)PublishPoseContext55(poseContext55);
    return Run(t,3,publish);}
static kcd2vr::grasp13::Acquisition54 pickupWitness54{};
static bool pickupWitnessReady54=false,pickupWitnessStale54=false;
static bool PickupWitness54(kcd2vr::grasp13::Acquisition54& out) noexcept {
    out=pickupWitness54;if(!pickupWitnessStale54)out.tick=frameClock51;return pickupWitnessReady54;
}
static kcd2vr::grasp13::PendingUse55 pendingWitness55{};
static bool pendingReady55=false;
static bool PendingWitness55(kcd2vr::grasp13::PendingUse55& out) noexcept {out=pendingWitness55;return pendingReady55;}
static kcd2vr::grasp13::HandFrame publishedGraspHands55{};
static unsigned graspHandsPublications55=0,resetWorldReads55=0;
static void CaptureGraspHands55(const kcd2vr::grasp13::HandFrame& hands) noexcept {publishedGraspHands55=hands;++graspHandsPublications55;}
static void RotateNativePelvis54(V3 axis,double degrees){
    fixture.original=fixture.reference;auto locals=fixture.local;
    const double half=degrees*3.14159265358979323846/360;
    locals[rig.pelvis].q[0]=float(axis.x*std::sin(half));
    locals[rig.pelvis].q[1]=float(axis.y*std::sin(half));
    locals[rig.pelvis].q[2]=float(axis.z*std::sin(half));
    locals[rig.pelvis].q[3]=float(std::cos(half));
    for(int j=0;j<Count;++j)fixture.original[j]=fixture.parent[j]<0?locals[j]:AbsoluteFromRelative(fixture.original[fixture.parent[j]],locals[j]);
    fixture.live=fixture.original;
}

int main(){
 tests=failures=0;
 for(bool smooth:{false,true})for(auto axis:{V3{1,0,0},V3{0,1,0},V3{0,0,1}}){
  InitFrame51(smooth);auto t=Tracking();Check(TickFrame51(t),"initial frame commits");
  RotateNativePelvis54(axis,15);Check(TickFrame51(t),"tilted native frame commits");
  Check(pc::angle(PQ(QFromQuatT(g_pose6689.work[rig.pelvis])),PQ(QFromQuatT(fixture.original[rig.pelvis])))<1e-5,"pre-C54 native pelvis rotation retained");
  for(int side=0;side<2;++side){
   const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r,h=side?rig.hand_l:rig.hand_r;
   Check(std::fabs(Len(Sub(FromT(g_pose6689.work[f]),FromT(g_pose6689.work[u])))-g_pose6689.upper[side])<.0001,"fixed upper arm length");
   Check(std::fabs(Len(Sub(FromT(g_pose6689.work[h]),FromT(g_pose6689.work[f])))-g_pose6689.fore[side])<.0001,"fixed forearm length");
  }
  Check(Same(fixture.live,fixture.original),"native buffers restored after tracked publication");
 }
 for(bool follow:{false,true}){
  InitFrame51();g_body_follow128=follow;auto t=Tracking();Check(TickFrame51(t),"crouch reference commits");
  t.head.y-=.30f;Check(TickFrame51(t),"tracked crouch commits");
  Check(FromT(g_pose6689.work[rig.pelvis]).z<fixture.original[rig.pelvis].t[2]-.25f,"physical crouch preserved");
 }

 // Non-disappearing hands: execute the actual publication and check the
 // retained C63 render envelope, not only the IK endpoint calculation.
 bool commits=true,lengths=true,envelopes=true,headStable=true,reacquires=true;
 for(int direction=0;direction<3;++direction){
  InitFrame51();auto t=Tracking();
  for(int i=0;i<40;++i){
   const float reach=.2f+float(i%20)*.065f;
   t.right={.28f,-.35f,direction==0?-reach:direction==1?reach:-.3f};
   t.left={-.28f,-.35f,direction==0?-reach:direction==1?reach:-.3f};
   if(direction==2){t.right.x=reach;t.left.x=-reach;}
   NativePelvis51({.02f*float(i),.01f*float(i),0});
   const bool ok=TickFrame51(t);commits=commits&&ok;
   CommittedFrame6695 packet{};kcd2vr::render_bounds63::Envelope e{};
   envelopes=envelopes&&g_committed_frame6695.read(packet)&&g_render_bounds63.read(e)&&
       e.freshTracked&&e.points>=4&&kcd2vr::render_bounds63::valid(e.box)&&e.sample==t.sequence;
   for(int side=0;side<2;++side){
    const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r,h=side?rig.hand_l:rig.hand_r;
    lengths=lengths&&std::fabs(Len(Sub(FromT(g_pose6689.work[f]),FromT(g_pose6689.work[u])))-g_pose6689.upper[side])<.0001&&
       std::fabs(Len(Sub(FromT(g_pose6689.work[h]),FromT(g_pose6689.work[f])))-g_pose6689.fore[side])<.0001;
    const auto wrist=FromT(packet.allowed[side]);const float xyz[]{wrist.x,wrist.y,wrist.z};
    for(int k=0;k<3;++k)envelopes=envelopes&&xyz[k]>=e.box.v[k]-.0001f&&xyz[k]<=e.box.v[k+3]+.0001f;
   }
   tf::Frame frame{};headStable=headStable&&g_tracked_frame51.read(frame)&&frame.valid&&
       Len(Sub(FromT(g_pose6689.work[rig.head]),VP(frame.anchor)))<.06f;
  }
  t.right_held=true;commits=commits&&TickFrame51(t);
  // A valid 10ms held snapshot may keep the visual pose, but must not
  // manufacture a fresh collision/render-envelope witness.
  kcd2vr::render_bounds63::Envelope grace{};
  Check(g_render_bounds63.read(grace)&&!grace.freshTracked,"held tracking does not mint fresh render evidence");
  t.right_held=false;t.right_valid=false;commits=commits&&TickFrame51(t);
  t.right_valid=true;reacquires=reacquires&&TickFrame51(t)&&g_render_bounds63.read(grace)&&grace.freshTracked;
 }
 Check(commits,"forward backward lateral overreach and locomotion keep actual pose publications");
 Check(lengths,"extended and unreachable hand trajectories preserve anatomical arm lengths");
 Check(envelopes,"C63 committed render envelopes contain both shown wrists throughout reach and locomotion");
 Check(headStable,"tracked head remains within6cm of shared frame throughout reaches and locomotion");
 Check(reacquires,"tracking reacquisition restores fresh hand render envelope");
 std::printf("H26 actual pre-C54 foundation checks=%d failures=%d\n",tests,failures);return failures?1:0;
}

