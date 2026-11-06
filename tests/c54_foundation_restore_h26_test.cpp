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

static void PresentationBasisChecksH20(){
 for(bool smooth:{false,true})for(bool nativeHeight:{false,true}){
  InitFrame51(smooth);if(nativeHeight)g_frame_control51.fetch_or(kNativeBodyHeightBit54);
  g_frame_neutral51=+[](std::uint64_t) noexcept{return true;};
  pickupWitnessReady54=pickupWitnessStale54=pendingReady55=false;
  auto t=Tracking();Check(TickFrame51(t),"basis: ordinary reference commits");
  tf::Frame initial{};g_tracked_frame51.read(initial);tf::EyeState eye{};pc::Vec3 camera{};
  const auto nativeEye=[](const tf::Frame& f){return f.root+pc::rotate(f.rotation,f.nativeHead*f.scale);};
  Check(tf::eye(eye,initial,initial.stamp,initial.control,frameClock51,nativeEye(initial),camera),
        "basis: actual neutral eye reference is measured once");
  const auto head=FromT(g_pose6689.work[rig.head]);
  const V3 wrists[]{FromT(g_pose6689.work[rig.hand_r]),FromT(g_pose6689.work[rig.hand_l])};
  pendingWitness55={initial.stamp.actor,initial.stamp.epoch,77,frameClock51+1,0};
  pendingReady55=true;g_graspPendingUse55=PendingWitness55;
  pickupWitness54={initial.stamp.actor,0x2000,0x3000,77,initial.stamp.epoch,pendingWitness55.requestedAt,frameClock51+1,17,0,true};
  pickupWitness54.trackingEpoch55=t.source_epoch=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
  g_graspAcquisition54=PickupWitness54;
  // Current witness states follow the separately composed complete native owner
  // schedule: pending Use -> installed Picking -> native Attach -> native cleanup
  // with held resource -> fast Drop/Stow -> ordinary -> another native request.
  // No old witness is kept active after cleanup/terminal in this body fixture.
  const V3 pelvis[]{ {.02f,.08f,-.02f},{.04f,.16f,-.03f},{.05f,.20f,-.03f},
                    {.05f,.20f,-.03f},{.05f,.20f,-.03f},{.025f,.10f,-.015f},
                    {0,0,0},{0,0,0},{-.04f,.12f,-.02f},{-.04f,.12f,-.02f},{0,0,0} };
  for(unsigned phase=0;phase<std::size(pelvis);++phase){
   const bool installed=phase==1||phase==2||phase==8;
   pickupWitnessReady54=installed;g_gameplay_allowed=!installed;g_ctx_scripted=installed;t.allowed=!installed;
   t.right_grip=phase==4||phase==10?0.f:1.f;
   if(phase==7){pendingWitness55={initial.stamp.actor,initial.stamp.epoch,78,frameClock51+1,0};
    pickupWitness54.serial=78;pickupWitness54.action=0x2100;pickupWitness54.scope=0x3100;
    pickupWitness54.startedAt=pendingWitness55.requestedAt;pickupWitness54.nativeToken=18;}
   NativePelvis51(pelvis[phase]);
   const float yaw=.04f*phase;float world[]{0,0,std::sin(yaw*.5f),std::cos(yaw*.5f),10.f+.03f*phase,20.f-.02f*phase,5.f+.01f*phase,1};
   std::memcpy(character.data()+0xA08,world,sizeof(world));
   Check(TickFrame51(t),"basis: complete pickup/held/fastterminal/return/reacquire pose commits");
   tf::Frame f{};CommittedFrame6695 packet{};const bool got=g_tracked_frame51.read(f)&&g_committed_frame6695.read(packet);
   Check(got&&f.valid&&std::abs(f.anchor.x-initial.anchor.x)<1e-6&&std::abs(f.anchor.y-initial.anchor.y)<1e-6,
         "basis: authored pelvis XY cannot move current tracked coordinate reference");
   Check(installed?(f.acquisitionSerial==pickupWitness54.serial&&f.acquisitionAction==pickupWitness54.action):
       (!f.acquisitionSerial&&!f.acquisitionAction&&!f.acquisitionScope&&!f.acquisitionToken),
         "basis: acquisition metadata follows CURRENT native action and retires on cleanup");
   bool lengths=true;for(int side=0;side<2;++side){
    const int u=side?rig.upper_l:rig.upper_r,e=side?rig.fore_l:rig.fore_r,h=side?rig.hand_l:rig.hand_r;
    lengths=lengths&&std::abs(Len(Sub(FromT(g_pose6689.work[e]),FromT(g_pose6689.work[u])))-g_pose6689.upper[side])<.0001&&
      std::abs(Len(Sub(FromT(g_pose6689.work[h]),FromT(g_pose6689.work[e])))-g_pose6689.fore[side])<.0001;
    const auto& leg=g_pose6689.leg[side];
    lengths=lengths&&std::abs(Len(Sub(FromT(g_pose6689.work[leg.knee]),FromT(g_pose6689.work[leg.hip])))-leg.thigh)<.0001&&
      std::abs(Len(Sub(FromT(g_pose6689.work[leg.ankle]),FromT(g_pose6689.work[leg.knee])))-leg.shin)<.0001;
   }
   Check(lengths&&Same(fixture.live,fixture.original),"basis: both hard arms/legs and native restoration remain unchanged");
   Check(packet.world[4]==world[4]&&packet.world[5]==world[5]&&packet.world[6]==world[6]&&
       pc::length(f.root-pc::Vec3{world[4],world[5],world[6]})<1e-6,
         "basis: actual current entity travel/stairs remains live without a frozen root");
   Check(tf::eye(eye,f,packet.stamp,f.control,frameClock51,nativeEye(f),camera)&&
       pc::length(camera-(f.root+pc::rotate(f.rotation,(f.anchor+eye.offset)*f.scale)))<1e-6,
         "basis: actual eye uses same current body foundation and entity turn");
   if(!nativeHeight){
    Check(Len(Sub(FromT(g_pose6689.work[rig.head]),head))<.001&&
      Len(Sub(FromT(g_pose6689.work[rig.hand_r]),wrists[0]))<.001&&
      Len(Sub(FromT(g_pose6689.work[rig.hand_l]),wrists[1]))<.001,
        "basis: default HMD height keeps head and both wrists through cleanup/fastterminal/pelvis return");
   }
   std::printf("BASIS_PHASE smooth=%d nativeHeight=%d phase=%u z=%.9f offsetZ=%.9f serial=%llu\n",smooth,nativeHeight,phase,f.anchor.z,f.bodyOffset.z,(unsigned long long)f.acquisitionSerial);
  }
  tf::Frame returned{};g_tracked_frame51.read(returned);
  Check(std::abs(returned.bodyOffset.x)<1e-6&&std::abs(returned.bodyOffset.y)<1e-6,
        "basis: completed native pelvis return leaves no permanent horizontal offset");
  for(float scale:{.8f,1.2f,1.f}){
   float world[8]{};std::memcpy(world,character.data()+0xA08,sizeof(world));world[7]=scale;
   std::memcpy(character.data()+0xA08,world,sizeof(world));
   Check(TickFrame51(t)&&g_tracked_frame51.read(returned)&&returned.valid&&std::abs(returned.scale-scale)<1e-6,
         "basis: current native model scale remains an actual body input");
   CommittedFrame6695 scaled{};g_committed_frame6695.read(scaled);
   Check(std::abs(scaled.world[7]-scale)<1e-6&&
       tf::eye(eye,returned,scaled.stamp,returned.control,frameClock51,nativeEye(returned),camera)&&
       pc::length(camera-(returned.root+pc::rotate(returned.rotation,(returned.anchor+eye.offset)*returned.scale)))<1e-6,
         "basis: current scaled entity transform is composed once by actual camera math");
  }
  const V3 priorHand=FromT(g_pose6689.work[rig.hand_r]);
  t.right.x+=.015f;t.left.x-=.015f;t.head.x+=.020f;
  Check(TickFrame51(t),"basis: new real tracked hand/HMD movement remains current");
  Check(Len(Sub(Sub(FromT(g_pose6689.work[rig.hand_r]),priorHand),{.015f,0,0}))<.002f,
        "basis: wrist follows new controller displacement without an added filter");
  CommittedFrame6695 moved{};g_committed_frame6695.read(moved);
  Check(std::abs((moved.targetHead122.x-head.x)-.020f)<.002f,
        "basis: real HMD displacement remains independent of anatomical base");
  // A repeated XR stamp with a changed action is a new native publication, not
  // permission to return the cached acquisition metadata from the first eye.
  pickupWitness54.serial=79;pickupWitness54.action=0x2200;pickupWitness54.scope=0x3200;
  pickupWitness54.startedAt=frameClock51;pickupWitnessReady54=true;pendingReady55=false;
  g_gameplay_allowed=false;g_ctx_scripted=true;t.allowed=false;t.right_grip=1;
  Check(Run(t,3,true),"basis: same-stamp current acquisition can publish without temporal credit");
  tf::Frame same{};g_tracked_frame51.read(same);
  Check(same.acquisitionSerial==79&&same.acquisitionAction==0x2200,
        "basis: same-stamp publication does not borrow previous native action identity");
  pickupWitnessReady54=false;g_gameplay_allowed=true;g_ctx_scripted=false;t.allowed=true;
  Check(Run(t,3,true)&&g_tracked_frame51.read(same)&&!same.acquisitionSerial&&!same.acquisitionAction,
        "basis: same-stamp cleanup revokes authority metadata without changing origin");
  animation_filter::nativeBodyOwned112=true;const int beforeNativeWrites=writeCalls;
  Check(!TickFrame51(t)&&g_tracked_frame51.read(same)&&!same.valid&&writeCalls==beforeNativeWrites&&Same(fixture.live,fixture.original),
        "basis: protected native body keeps its existing no-C51 owner");
  animation_filter::nativeBodyOwned112=false;
  g_gameplay_allowed=false;g_ctx_scripted=true;t.allowed=false;const int writes=writeCalls;
  Check(!TickFrame51(t)&&writeCalls==writes&&Same(fixture.live,fixture.original),
        "basis: without actual current gameplay/acquisition permission no old reference grants a pose");
  g_gameplay_allowed=true;g_ctx_scripted=false;t.allowed=true;g_frame_neutral51=nullptr;
  g_graspAcquisition54=nullptr;g_graspPendingUse55=nullptr;pendingReady55=false;
 }
}

static void PresentationGraceChecksH22(){
 namespace tr=kcd2vr::tracking59c6;
 for(int side=0;side<2;++side)for(bool smooth:{false,true})for(bool nativeHeight:{false,true}){
  InitFrame51(smooth);if(nativeHeight)g_frame_control51.fetch_or(kNativeBodyHeightBit54);
  g_frame_neutral51=+[](std::uint64_t)noexcept{return true;};
  g_graspEnabled13=+[]()noexcept{return true;};g_graspHands13=CaptureGraspHands55;
  auto base=Tracking();tr::SetCameraSource(2);tr::Configure(true);tr::Reset();base.source_epoch=tr::epoch.load();
  struct Observed {TrackingSnapshot tracking{};tf::Frame frame{},donor{};pc::Vec3 nativeFiltered{};V3 hand[2]{},head{};bool ok=false,acquisition=false;};
  const auto family=[&](std::uint64_t tick,unsigned tracked,unsigned valid){
   tr::Frame f{};f.tick=tick;f.ns=tick*1000000;f.epoch=base.source_epoch;f.locateTime=f.ns+60000000;f.controllersSampled=true;
   const auto set=[&](unsigned n,V3 p,Q4 q){f.poses[n]={p.x,p.y,p.z,q.x,q.y,q.z,q.w};f.valid[n]=(valid&(1u<<n))!=0;f.tracked[n]=(tracked&(1u<<n))!=0;};
   set(0,base.head,base.hq);set(1,base.left,base.lq);set(2,base.right,base.rq);return f;
  };
  const auto consume=[&](std::uint64_t tick,unsigned tracked=7,unsigned valid=7){
   auto f=family(tick,tracked,valid);Check(tr::Publish(f),"grace: actual serialized tracking publication");
   tr::Frame measured{};Check(tr::Read(measured),"grace: actual current tracking mailbox");
   Observed o{};o.tracking=base;o.tracking.input_tick=tick;
   Check(tr::Compose(o.tracking,measured,tick,f.ns)==tr::Choice::Current,"grace: actual original display grace composition");
   frameClock51=tick;PublishPoseContext55(poseContext55);CoherentTransaction6689 tx;
   o.ok=BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,3,o.tracking);
   o.acquisition=tx.acquisitionRequired57;o.frame=tx.frame51;o.nativeFiltered=tx.frameState51.filtered;
   o.head=FromT(g_pose6689.work[rig.head]);for(int h=0;h<2;++h)o.hand[h]=FromT(g_pose6689.work[h?rig.hand_l:rig.hand_r]);
   if(o.ok)for(int h=0;h<2;++h){const int u=h?rig.upper_l:rig.upper_r,e=h?rig.fore_l:rig.fore_r,w=h?rig.hand_l:rig.hand_r;
    Check(std::abs(Len(Sub(FromT(g_pose6689.work[e]),FromT(g_pose6689.work[u])))-g_pose6689.upper[h])<.0001&&
          std::abs(Len(Sub(FromT(g_pose6689.work[w]),FromT(g_pose6689.work[e])))-g_pose6689.fore[h])<.0001,"grace: fixed arm lengths remain exact");}
   tx.Restore();PublishCommitted6689(tx);o.donor=g_pose6689.ordinaryReference54;
   Check(Same(fixture.live,fixture.original),"grace: actual native buffers restored on every branch");return o;
  };
  const unsigned handBit=side?2u:4u;
  Check(tr::Publish(family(9990,7,7)),"grace: original first-locate reacquisition guard warms");
  const auto initial=consume(10000);Check(initial.ok&&initial.frame.valid&&initial.frame.neutral,"grace: all-fresh body admits original neutral reference");
  tf::EyeState eye{};pc::Vec3 camera{};
  const auto nativeEye=[](const tf::Frame& f){return f.root+pc::rotate(f.rotation,f.nativeHead*f.scale);};
  Check(tf::eye(eye,initial.frame,initial.frame.stamp,initial.frame.control,10000,nativeEye(initial.frame),camera),"grace: actual camera math learns only original fresh neutral anatomy");
  NativePelvis51({.12f,.03f,-.02f});const auto fresh=consume(10010);
  const auto held=consume(10020,7^handBit);
  Check(held.ok&&held.frame.valid&&(side?held.tracking.left_valid&&held.tracking.left_held:held.tracking.right_valid&&held.tracking.right_held),"grace: still-valid held hand retains current presentation frame only");
  Check(!held.frame.neutral&&held.donor.stamp.sample==fresh.donor.stamp.sample&&!held.acquisition,"grace: display-only publication cannot renew neutral or pre-request action provenance");
  Check(!publishedGraspHands55.tracked[side]&&publishedGraspHands55.tracked[1-side],"grace: actual grasp publisher keeps original held-side refusal and fresh opposite hand");
  Check(Len(Sub(held.hand[side],fresh.hand[side]))<.001f&&Len(Sub(held.head,fresh.head))<.001f,"grace: hand and HMD do not jump to native authored XY during display grace");
  tf::EyeState cold{};pc::Vec3 unused{};
  Check(!tf::eye(cold,held.frame,held.frame.stamp,held.frame.control,10020,nativeEye(held.frame),unused)&&!cold.ready,"grace: display frame cannot initialize a new camera neutral measurement");
  Check(tf::eye(eye,held.frame,held.frame.stamp,held.frame.control,10020,nativeEye(held.frame),camera)&&
       pc::length(camera-(held.frame.root+pc::rotate(held.frame.rotation,(held.frame.anchor+eye.offset)*held.frame.scale)))<1e-6,"grace: existing camera anatomy uses the same current body coordinate frame");
  const auto reacquire=consume(10030);const auto current=consume(10040);
  Check(reacquire.ok&&reacquire.frame.valid&&!reacquire.frame.neutral&&(side?reacquire.tracking.left_held:reacquire.tracking.right_held)&&
        reacquire.donor.stamp.sample==fresh.donor.stamp.sample,"grace: original first-fresh reacquisition guard remains display-only");
  Check(current.ok&&current.frame.valid&&current.frame.neutral&&!current.tracking.right_held&&!current.tracking.left_held&&
        current.donor.stamp.sample==current.frame.stamp.sample&&
        std::hypot(current.hand[side].x-fresh.hand[side].x,current.hand[side].y-fresh.hand[side].y)<.001f,"grace: actual fresh recovery resumes original neutral/action reference without an XY basis jump");
  Check(std::abs(held.frame.anchor.z-held.nativeFiltered.z)<1e-9&&held.frame.hmdHeight==!nativeHeight&&
        (nativeHeight?(held.frame.anchor.z<=fresh.frame.anchor.z&&held.frame.anchor.z>=held.frame.rawAnchor.z-1e-9):std::abs(held.frame.anchor.z-g_pose6689.rest[rig.head].t[2])<1e-6),"grace: native-Z/HMD-height selection remains original");
  base.right.x+=.015f;base.left.x-=.015f;base.head.x+=.02f;
  const auto moved=consume(10050,7^handBit);
  Check(moved.ok&&Len(Sub(Sub(moved.hand[side],current.hand[side]),{side?-.015f:.015f,0,0}))<.002f&&
        std::abs((moved.head.x-current.head.x)-.02f)<.002f,"grace: current controller and HMD displacement has no added temporal lag");
  float world[8]{};std::memcpy(world,character.data()+0xA08,sizeof(world));const float yaw=.3f;world[2]=std::sin(yaw*.5f);world[3]=std::cos(yaw*.5f);world[4]+=.2f;world[5]-=.1f;
  std::memcpy(character.data()+0xA08,world,sizeof(world));const auto traveled=consume(10060,7^handBit);
  Check(traveled.ok&&traveled.frame.valid&&std::abs(traveled.frame.root.x-world[4])<1e-6&&
        pc::angle(traveled.frame.rotation,{world[0],world[1],world[2],world[3]})<1e-6,"grace: current native world travel and turn remain live");
  const auto expired=consume(10140,7^handBit,7^handBit);
  Check(expired.ok&&!expired.frame.valid&&!(side?expired.tracking.left_valid:expired.tracking.right_valid),"grace: original80ms hand expiry still removes the presentation frame");
  const auto allLost=consume(10230,1,1);Check(!allLost.ok&&!allLost.frame.valid,"grace: both hands lost cannot retain the old display frame");
  (void)consume(10240);const auto freshAgain=consume(10250);Check(freshAgain.ok&&freshAgain.frame.valid,"grace: original reacquisition reestablishes current frame after loss");
  const auto headHeld=consume(10260,6);Check(headHeld.ok&&!headHeld.frame.valid&&headHeld.tracking.head_held,"grace: HMD-held policy remains the original native fallback");
  const auto headExpired=consume(10330,6,6);Check(!headExpired.ok&&!headExpired.frame.valid&&!headExpired.tracking.head_valid,"grace: original60ms HMD expiry cannot extend pose authority");
  (void)consume(10340);(void)consume(10350);
  base.allowed=false;g_gameplay_allowed=false;poseContext55.menu=true;const int writes=writeCalls;
  const auto menu=consume(10360,7^handBit);Check(!menu.ok&&!menu.frame.valid&&writeCalls==writes,"grace: menu/gameplay ownership revokes old presentation without native writes");
  base.allowed=true;g_gameplay_allowed=true;poseContext55.menu=false;
  animation_filter::nativeBodyOwned112=true;const auto protectedPose=consume(10370);Check(!protectedPose.ok&&!protectedPose.frame.valid,"grace: protected native body never borrows the display basis");
  animation_filter::nativeBodyOwned112=false;g_graspEnabled13=nullptr;g_graspHands13=nullptr;g_frame_neutral51=nullptr;
 }
 tr::Configure(false);tr::SetCameraSource(0);tr::Reset();
}

static void FreeWorkstationFoundationH51(){
 namespace ai=kcd2vr::activity_input163;namespace mh=kcd2vr::minigame_hands45;
 for(unsigned kind:{mh::Alchemy,mh::Smithing})for(int mode:{mh::Native,mh::SemiPassenger,mh::Gesture}){
  InitFrame51(false);g_gameplay_allowed=true;g_frame_neutral51=[](std::uint64_t) noexcept{return true;};
  ai::witness175.Clear();ai::witness175.ready=true;ai::witness175.transitionDepth175=0;
  workstation178r20c1::experimentArmed36=false;kcd2vr::combat_rules178r10::f5Rung=0;
  kcd2vr::combat_rules178r10::workstationEnabled=true;kcd2vr::alchemy_own_head_h51::enabled=false;
  mh::setMode(kind,mode);mh::publish(mh::None,frameClock51);auto t=Tracking();
  Check(TickFrame51(t),"FREE VRIK workstation control starts with actual ordinary body");
  ai::witness175.Begin(kind==mh::Alchemy?ai::Kind175::Alchemy:ai::Kind175::Smithing,0x51000+kind,frameClock51);
  mh::publish(kind,frameClock51);g_gameplay_allowed=false;g_ctx_scripted=true;
  for(int joint:{rig.neck,rig.head,46}){fixture.original[joint].t[0]+=.20f;fixture.original[joint].t[1]+=.10f;fixture.original[joint].t[2]-=.15f;}
  fixture.live=fixture.original;
  Check(TickFrame51(t),"native workstation semantic ownership retains the actual coherent transaction");
  tf::Frame f{};g_tracked_frame51.read(f);
  Check(f.valid==(mode==mh::Gesture),"only the saved FREE VRIK mode admits shared body foundation without an extra trial flag");
  if(mode==mh::Gesture){
   Check(f.workstationKindH51==kind&&f.workstationInstanceH51==0x51000+kind&&
       f.workstationLifetimeH51==ai::witness175.lifetime175.load()&&f.workstationModeEpochH51==mh::modeEpoch67.load(),
       "actual foundation carries the exact current native workstation and mode lifetime");
   Check(!f.neutral&&Len(Sub(FromT(g_pose6689.work[rig.head]),FromT(fixture.original[rig.head])))>.1,
       "FREE VRIK body removes authored head displacement without learning it as neutral");
  }
  Check(Same(fixture.live,fixture.original),"workstation display transaction restores original native pose storage");
  ai::witness175.End(0x51000+kind);mh::publish(mh::None,frameClock51);
 }
 ai::witness175.Clear();ai::witness175.ready=false;g_gameplay_allowed=true;g_ctx_scripted=false;
}
#ifndef KCD2VR_H26_TEST_MAIN
#define KCD2VR_H26_TEST_MAIN main
#endif
int KCD2VR_H26_TEST_MAIN(){
 tests=failures=0;
 for(bool smooth:{false,true})for(auto axis:{V3{1,0,0},V3{0,1,0},V3{0,0,1}}){
  InitFrame51(smooth);auto t=Tracking();Check(TickFrame51(t),"initial frame commits");
  RotateNativePelvis54(axis,15);Check(TickFrame51(t),"tilted native frame commits");
  Check(pc::angle(PQ(QFromQuatT(g_pose6689.work[rig.pelvis])),*kcd2vr::tracked_frame51::pelvis_foundation54(PQ(QFromQuatT(fixture.original[rig.pelvis])),PQ(QFromQuatT(g_pose6689.rest[rig.pelvis]))))<1e-5,"C54 tracked foundation removes native pelvis pitch-roll but retains yaw");
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
  // The held side may retain its visual pose, but contributes no fresh render
  // points. The independently current opposite arm must keep its own bounds.
  kcd2vr::render_bounds63::Envelope grace{};
  CommittedFrame6695 heldPacket{};
  const auto containsWrist=[](const kcd2vr::render_bounds63::Envelope& box,const QuatT28& wrist){
   const auto v=FromT(wrist);const float xyz[]{v.x,v.y,v.z};
   for(int k=0;k<3;++k)if(xyz[k]<box.box.v[k]-.0001f||xyz[k]>box.box.v[k+3]+.0001f)return false;
   return true;
  };
  Check(g_render_bounds63.read(grace)&&g_committed_frame6695.read(heldPacket)&&
      grace.freshTracked&&grace.sample==t.sequence&&grace.points==19&&
      containsWrist(grace,heldPacket.allowed[1])&&!containsWrist(grace,heldPacket.allowed[0]),
      "current opposite arm keeps its19 render points; held wrist contributes none");
  t.right_held=false;t.right_valid=false;commits=commits&&TickFrame51(t);
  t.right_valid=true;reacquires=reacquires&&TickFrame51(t)&&g_render_bounds63.read(grace)&&grace.freshTracked;
 }
 Check(commits,"forward backward lateral overreach and locomotion keep actual pose publications");
 Check(lengths,"extended and unreachable hand trajectories preserve anatomical arm lengths");
 Check(envelopes,"C63 committed render envelopes contain both shown wrists throughout reach and locomotion");
 Check(headStable,"tracked head remains within6cm of shared frame throughout reaches and locomotion");
 Check(reacquires,"tracking reacquisition restores fresh hand render envelope");

 bool headHeight=true,worldJoined=true;
 InitFrame51();auto hw=Tracking();Check(TickFrame51(hw),"head/world reference commits");
 for(int i=0;i<50;++i){
  RotateNativePelvis54({1,0,0},double(i%10)-5);
  for(int j=0;j<Count;++j)fixture.original[j].t[2]-=.30f;
  fixture.live=fixture.original;
  float world[8]{0,0,0,1,100.f+.04f*i,200.f+.02f*i,10.f+.01f*i,1};
  std::memcpy(character.data()+0xA08,world,sizeof(world));
  headHeight=headHeight&&TickFrame51(hw);
  tf::Frame shared{};CommittedFrame6695 packet{};
  headHeight=headHeight&&g_tracked_frame51.read(shared)&&shared.valid&&
      std::abs(FromT(g_pose6689.work[rig.head]).z-fixture.reference[rig.head].t[2])<.06f;
  worldJoined=worldJoined&&g_committed_frame6695.read(packet)&&
      std::abs(packet.world[4]-world[4])<.0001f&&std::abs(packet.world[5]-world[5])<.0001f&&std::abs(packet.world[6]-world[6])<.0001f;
 }
 Check(headHeight,"native gait/pelvis height and tilt cannot sink tracked head; HMD height foundation retained");
 Check(worldJoined,"moving native body-world root joined once to actual published head/wrist model frame");
    for(bool smooth:{false,true}){
        struct Arm55 {V3 elbow[2]{},hand[2]{};Q4 upper[2]{},fore[2]{};};
        std::array<Arm55,120> reference55{};bool all55=true;double worstElbow55=0,worstRoll55=0,worstWrist55=0;
        for(unsigned pass55=0;pass55<2;++pass55){
            InitFrame51(smooth);auto move55=Tracking();
            for(unsigned n55=0;n55<reference55.size();++n55){
                const float phase55=float(n55)*.035f;
                move55.right.x=.33f+.04f*std::sin(phase55);move55.left.x=-move55.right.x;
                move55.right.y=move55.left.y=1.22f-fixture.reference[rig.head].t[2]+.04f*std::sin(phase55*.7f);
                move55.right.z=move55.left.z=-.25f-.03f*std::cos(phase55);
                if(pass55){
                    RotateNativePelvis54({1,0,0},15*std::sin(phase55));
                    const float gait55=-.035f*(1+std::sin(phase55*2));
                    for(int j55=0;j55<Count;++j55){int parent55=j55;while(parent55>=0&&parent55!=rig.pelvis)parent55=fixture.parent[parent55];
                        if(parent55==rig.pelvis)fixture.original[j55].t[2]+=gait55;}
                    fixture.live=fixture.original;
                }
                all55=TickFrame51(move55)&&all55;
                for(int side55=0;side55<2;++side55){
                    const int upper55=side55?rig.upper_l:rig.upper_r,fore55=side55?rig.fore_l:rig.fore_r,hand55=side55?rig.hand_l:rig.hand_r;
                    const auto elbow55=FromT(g_pose6689.work[fore55]),wrist55=FromT(g_pose6689.work[hand55]);
                    const auto uq55=QFromQuatT(g_pose6689.work[upper55]),fq55=QFromQuatT(g_pose6689.work[fore55]);
                    if(!pass55){auto& r55=reference55[n55];r55.elbow[side55]=elbow55;r55.hand[side55]=wrist55;r55.upper[side55]=uq55;r55.fore[side55]=fq55;}
                    else {const auto& r55=reference55[n55];worstElbow55=std::max(worstElbow55,double(Len(Sub(elbow55,r55.elbow[side55]))));
                        worstWrist55=std::max(worstWrist55,double(Len(Sub(wrist55,r55.hand[side55]))));
                        worstRoll55=std::max({worstRoll55,pc::angle(PQ(uq55),PQ(r55.upper[side55])),pc::angle(PQ(fq55),PQ(r55.fore[side55]))});}
                    all55=all55&&std::fabs(Len(Sub(elbow55,FromT(g_pose6689.work[upper55])))-g_pose6689.upper[side55])<.0001&&
                        std::fabs(Len(Sub(wrist55,elbow55))-g_pose6689.fore[side55])<.0001;
                }
            }
        }
        std::printf("C55_ELBOW_CONTINUITY smooth=%d complete=%d elbowDelta=%.9f wristDelta=%.9f skinAngleDelta=%.9f\n",smooth,all55,worstElbow55,worstWrist55,worstRoll55);
        Check(all55&&worstElbow55<.0001&&worstWrist55<.0001&&worstRoll55<.001,
              "continuous native pelvis tilt/gait cannot spin or move elbows relative to the same moving tracked targets");
    }
 // Actual coherent owner: native GAME kick must keep the whole lower body
 // even while follow/crouch would otherwise request pelvis/leg reconstruction.
 for(bool smooth:{false,true}){
  InitFrame51(smooth);g_body_follow128=true;auto t=Tracking();Check(TickFrame51(t),"native-kick handover reference commits");
  RotateNativePelvis54({1,0,0},15);t.head.y-=.20f;
  animation_filter::mockNativeKickActive175=true;kcd2vr::kick26::style=0;kcd2vr::frik_pose_probe::mockCombatExperiment175=true;
  g_kick_busy176=123;kick176::Request request{};request.serial=123;request.tick=frameClock51;request.cancel=g_kick_cancel176.load();
  Check(g_kick_request176.publish(request),"native-kick test has an otherwise current procedural kick request");
  ++t.sequence;t.tick=frameClock51+=10;t.timeNs=t.tick*1000000;PublishPoseContext55(poseContext55);
  CoherentTransaction6689 tx;const bool ok=BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,3,t);
  Check(ok&&!tx.frame51.valid,"active native kick vetoes tracked frame foundation despite follow and physical crouch");
  Check(!tx.selected[rig.pelvis]&&pc::angle(PQ(QFromQuatT(g_pose6689.work[rig.pelvis])),PQ(QFromQuatT(fixture.original[rig.pelvis])))<1e-6,
        "active native kick keeps native pelvis pose instead of follow/reseed reconstruction");
  bool nativeLegs=true;for(int side=0;side<2;++side)for(int n=0;n<4;++n){const int j=47+side*4+n;
   nativeLegs=nativeLegs&&!tx.selected[j]&&Len(Sub(FromT(g_pose6689.work[j]),FromT(fixture.original[j])))<1e-6&&
       pc::angle(PQ(QFromQuatT(g_pose6689.work[j])),PQ(QFromQuatT(fixture.original[j])))<1e-6;}
  Check(nativeLegs&&!tx.kickFrame176.active,"GAME native kick leaves both native leg chains untouched and denies a procedural replacement");
  tx.Restore();Check(Same(fixture.live,fixture.original),"native-kick pose callback restores original native buffers");
  animation_filter::mockNativeKickActive175=false;kcd2vr::frik_pose_probe::mockCombatExperiment175=false;g_kick_busy176=0;
 }
 PresentationBasisChecksH20();
 PresentationGraceChecksH22();
 FreeWorkstationFoundationH51();
 std::printf("H26 actual C54 restored foundation checks=%d failures=%d\n",tests,failures);return failures?1:0;
}

