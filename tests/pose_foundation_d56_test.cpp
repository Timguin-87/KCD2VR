// Executes the complete production acquisition-admission and pose-foundation
// blocks. Native root/bone writes and unrelated equipment/station leaves are
// synthetic. It does not claim a complete native mesh solve or headset result.
#include "vrik/tracked_frame51.h"
#include "core/native_grasp13.h"
#include "vrik/minigame_hands45.h"
#include "vrik/bow_camera_identity_d37b.h"
#include <atomic>
#include <array>
#include <cstdio>
#include <cstdlib>
namespace pc=kcd2vr::pose_core;
using V3=pc::Vec3;using Q4=pc::Quat;
static unsigned checks=0;
static void ck(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",why);std::exit(1);}}
static bool near(V3 a,V3 b,double e=1e-8){return pc::length(a-b)<e;}
static std::uint64_t clockD56=1000;
static auto GetTickCount64(){return clockD56;}
namespace kcd2vr::tracking59c6 {static std::atomic<std::uint64_t> epoch{5};}
namespace kcd2vr::smooth_lab {static std::uint64_t NativeAnchorPolicy(){return 0;}}
namespace kcd2vr::station_ws_j27 {static bool captureD47(std::uint64_t){return false;}}
namespace diagnostic_trace {static void Write(const char*){}}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};static bool NativeKickActive175(){return false;}}
namespace kcd2vr::workstation178r20c1 {
struct WorkstationOwnerH51 {std::uint64_t modeEpoch=0,lifetime=0;std::uintptr_t instance=0;unsigned kind=0;bool alchemyBookAS1=false;std::uint64_t stationLifetimeH46=0;};
static bool workstationOwnerH51(std::uint64_t,WorkstationOwnerH51& out){out={};return false;}
}
namespace workstation178r20c1=kcd2vr::workstation178r20c1;
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_ctx_reading{false};
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1},g_pose_epoch6689{4};
static std::atomic<void*> g_henry_item_attach_manager{nullptr};
static constexpr std::uint64_t kNativeBodyHeightBit54=4;
static bool nativeNeutral=true,strongReady=false,foundationReady=false,claimBlocked=false;
static bool FrameNeutral(std::uint64_t){return nativeNeutral;}
static auto g_frame_neutral51=&FrameNeutral;
static kcd2vr::grasp13::Acquisition54 witness{};
static bool StrongReader(kcd2vr::grasp13::Acquisition54& out)noexcept{out=witness;out.tick=clockD56;return strongReady;}
static bool FoundationReader(kcd2vr::grasp13::Acquisition54& out)noexcept{out=witness;out.tick=clockD56;return foundationReady;}
static auto g_graspAcquisition54=&StrongReader;
static bool (*g_graspPendingUse55)(kcd2vr::grasp13::PendingUse55&)=nullptr;
static bool HandClaimBlockedT24(int,std::uintptr_t,std::uint64_t,std::uint64_t){return claimBlocked;}
static const char* WorkstationContextRefusal19c4r2(std::uint64_t,bool){return nullptr;}
static bool ValidateBuffer1Setter(void*,int& out){out=1;return true;}
static bool BowGripDrawEnabled596C1(){return false;}
struct Bone {Q4 q{};V3 t{};};
static V3 FromT(Bone b){return b.t;}
static Q4 QFromQuatT(Bone b){return b.q;}
static V3 PV(V3 a){return a;}static V3 VP(V3 a){return a;}
static Q4 PQ(Q4 q){return q;}static Q4 QP(Q4 q){return q;}
static V3 Sub(V3 a,V3 b){return a-b;}static V3 Add(V3 a,V3 b){return a+b;}
static V3 QRotateV(Q4 q,V3 v){return pc::rotate(q,v);}static V3 XrToCryV(V3 v){return {v.x,-v.z,v.y};}
struct ModelWorld6692 {Q4 q{};V3 t{};double scale=1;};
static ModelWorld6692 modelWorld{};
static bool modelReady=true;
static bool ReadModelWorld6692(void*,ModelWorld6692& out){out=modelWorld;return modelReady;}
struct EquipmentLeaf {
 struct Profile {int side=-1;std::uint64_t key=0,generation=0;bool bow()const{return false;}};
 struct Item {bool held=false;Profile profile{};};
 bool valid=false,player=false;void* manager=nullptr;std::uint64_t request=0,epoch=0,tick=0;
 bool handKnown[2]{};std::uint64_t handKey[2]{};int count=0,heldIndex[2]{-1,-1};std::array<Item,1> items{};
};
struct Tracking {std::uint64_t source_epoch=5,sequence=1,tick=1000,timeNs=1000000000;Q4 hq{};
 bool allowed=true,head_valid=true,right_valid=true,left_valid=true,head_held=false,right_held=false,left_held=false;
 float right_grip=1,left_grip=0,right_trigger=0;};
struct State {
 std::array<Bone,2> rest{{{{},{0,0,.9}},{{},{0,0,1.7}}}},original{},work{},neutral{};
 bool legsReady128=true;
 kcd2vr::tracked_frame51::State trackedFrame51{};
 kcd2vr::tracked_frame51::Frame ordinaryReference54{},acquisitionReference54{},pendingReference55{};
 kcd2vr::grasp13::PendingUse55 pendingUse55{};
 std::uint64_t acquisitionSerial54=0,acquisitionAction54=0;
};
struct Transaction {std::uint64_t epoch=4,control51=6;int setter=0;const char* stage="";
 bool acquisitionRequired57=false;EquipmentLeaf physicalEquipment{};Tracking tracking{};
 kcd2vr::grasp13::Acquisition54 acquisition54{};
 kcd2vr::tracked_frame51::Frame acquisitionReference54{},frame51{};
 kcd2vr::tracked_frame51::State frameState51{};};
static bool RejectCoherent6693(Transaction&,int){return false;}
static void SetWorldRotation6689(State& s,int id,Q4 q){s.work[id].q=q;}
static bool Compose6689(State&){return true;} // native bone composition leaf, not an IK test
static bool run(State& s,Transaction& tx,bool smooth){
 void* character=reinterpret_cast<void*>(0x2200);void* pose=reinterpret_cast<void*>(0x2250);
 auto& ts=tx.tracking;const bool full=true,readingVisual32=false,smoothBody51=smooth;
 struct {int pelvis=0,head=1;} rig;
 const V3 baseHead=s.rest[rig.head].t,liveNativeHead123=s.original[rig.head].t;Q4 frame{};
 V3 headAnchor123=baseHead,headTarget=baseHead,requestedGoals[2]{{.2,.1,1.3},{-.2,.1,1.3}};
 V3 requestedGoalsRawH50[2]{requestedGoals[0],requestedGoals[1]},goals[2]{requestedGoals[0],requestedGoals[1]};
 const bool prepared169r2=false,sameBow169r2=false;const std::uint64_t preparedRequest169r2=0;struct {bool bowHeld=false;} priorBow169r2;
 s.work=s.original;
 #include "subsystems/interactions/pose_acquisition_admission.inl"
 #include "subsystems/vrik/pose_frame_foundation.inl"
 ck(near(headTarget-baseHead,tx.frame51.valid?tx.frame51.anchor-baseHead:V3{}),"production head goal receives exact shared foundation delta");
 if(tx.frame51.valid)ck(near(goals[0]-V3{.2,.1,1.3},tx.frame51.anchor-baseHead),"production wrist goal receives identical foundation delta");
 return true;
}
static void commit(State& s,const Transaction& tx){
 s.trackedFrame51=tx.frame51.valid?tx.frameState51:kcd2vr::tracked_frame51::State{};
 if(tx.frame51.valid&&!tx.frame51.acquisitionSerial&&!tx.tracking.head_held&&!tx.tracking.right_held&&!tx.tracking.left_held)s.ordinaryReference54=tx.frame51;
 if(tx.frame51.valid&&tx.frame51.acquisitionSerial){s.acquisitionReference54=tx.acquisitionReference54;s.acquisitionSerial54=tx.acquisition54.serial;s.acquisitionAction54=tx.acquisition54.action;}
 else{s.acquisitionReference54={};s.acquisitionSerial54=s.acquisitionAction54=0;}
}
static void next(Transaction& tx){tx.acquisition54={};tx.frame51={};++tx.tracking.sequence;tx.tracking.tick=clockD56+=10;tx.tracking.timeNs=clockD56*1000000;}
static void reset(State& s,Transaction& tx){s={};s.original=s.work=s.rest;tx={};clockD56=1000;strongReady=foundationReady=claimBlocked=false;nativeNeutral=true;
 g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_ctx_reading=false;animation_filter::nativeBodyOwned112=false;
 modelReady=true;modelWorld={};kcd2vr::tracking59c6::epoch=5;kcd2vr::grasp13::foundationReaderD56=&FoundationReader;
 witness={0x2200,0x3300,0x4400,73,4,1001,1001,17,0,true};witness.trackingEpoch55=5;witness.generation55=9;}
int main(){
 for(bool smooth:{false,true}){State s{};Transaction tx{};reset(s,tx);ck(run(s,tx,smooth)&&tx.frame51.valid,"ordinary native-height reference is available");commit(s,tx);const auto anchor=tx.frame51.anchor;
  foundationReady=true;nativeNeutral=false;s.original[0].t.z+=.067842;
  for(unsigned n=0;n<16;++n){strongReady=!(n&1);next(tx);ck(run(s,tx,smooth)&&tx.frame51.acquisitionSerial==73,"same installed native Picking remains the presentation owner despite alternating physical eligibility");
   ck(near(tx.frame51.anchor,anchor)&&!tx.frame51.neutral,"67.842mm animated native pelvis change cannot switch shared pre-Use anchor");commit(s,tx);}
  g_gameplay_allowed=false;g_ctx_scripted=true;tx.tracking.allowed=false;next(tx);
  ck(run(s,tx,smooth)&&tx.frame51.acquisitionSerial==73&&near(tx.frame51.anchor,anchor),"scripted Picking context retains only its exact pre-Use foundation");commit(s,tx);
  foundationReady=false;next(tx);ck(!run(s,tx,smooth),"real acquisition exit while scripted revokes pose permission immediately");
  g_gameplay_allowed=true;g_ctx_scripted=false;tx.tracking.allowed=true;nativeNeutral=true;s.original[0].t.z=.86;next(tx);
  ck(run(s,tx,smooth)&&!tx.frame51.acquisitionSerial&&std::fabs(tx.frame51.anchor.z-1.66)<1e-8,"actual return to ordinary native crouch follows current height, not a frozen pickup anchor");
 }
 for(unsigned angle:{0u,15u,35u,90u}){State s{};Transaction tx{};reset(s,tx);const double r=angle*3.141592653589793/360;
  s.original[0].q={0,0,std::sin(r),std::cos(r)};modelWorld.t={12,23,5};modelWorld.q={0,0,std::sqrt(.5),std::sqrt(.5)};
  ck(run(s,tx,false)&&pc::angle(s.work[0].q,s.rest[0].q)<1e-8,"authored local pelvis yaw cannot rotate the ordinary shoulder foundation");
  ck(pc::angle(tx.frame51.rotation,modelWorld.q)<1e-8&&near(tx.frame51.root,modelWorld.t),"current entity world turn and travel remain in the completed frame");
  animation_filter::nativeBodyOwned112=true;next(tx);ck(run(s,tx,false)&&!tx.frame51.valid&&pc::angle(s.work[0].q,s.original[0].q)<1e-8,"protected native body keeps its authored pelvis and camera owner");
 }
 for(unsigned reason=0;reason<7;++reason){State s{};Transaction tx{};reset(s,tx);run(s,tx,false);commit(s,tx);foundationReady=true;g_gameplay_allowed=false;g_ctx_scripted=true;tx.tracking.allowed=false;next(tx);
  switch(reason){case 0:g_ctx_menu=true;break;case 1:g_ctx_dialogue=true;break;case 2:g_ctx_cutscene=true;break;case 3:tx.tracking.source_epoch=6;break;case 4:claimBlocked=true;break;case 5:tx.tracking.right_grip=0;break;case 6:animation_filter::nativeBodyOwned112=true;break;}
  ck(!run(s,tx,false),"actual context/source/claim/release/native-owner change cannot borrow acquisition foundation");
 }
 std::printf("D56 production foundation: %u/%u PASS; native root/bone/equipment leaves MOCKED\n",checks,checks);
}
