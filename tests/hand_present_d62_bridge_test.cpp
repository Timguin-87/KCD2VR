// Current production late-display adapter and completed-palm readers execute
// verbatim. Native identity/query/FK leaves are explicitly in-memory fixtures.
// This is not an engine/OpenXR/headset acceptance test.
#include "vrik/hand_present_d62.h"
#include "vrik/hand_shape59c7f.h"
#include "vrik/upper_body.h"
#include "vrik/user_modes175.h"
#include "vrik/npc_contact172.h"
#include "vrik/physical_weapon.h"
#include "subsystems/masterstrike_combos/draw_signal_d63.h"
#include "tracking_sample59c6.h"
#include "subsystems/interactions/washing_gesture_d59.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <limits>
namespace pc=kcd2vr::pose_core;
namespace hp=kcd2vr::hand_present_d61;
namespace hd=kcd2vr::hand_present_d62;
namespace ph=kcd2vr::physical;
namespace tracking59c6=kcd2vr::tracking59c6;
namespace kcd2vr::diagnostics {}
namespace diagnostics=kcd2vr::diagnostics;
struct V3 {float x=0,y=0,z=0;};struct Q4 {float x=0,y=0,z=0,w=1;};
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
static pc::Vec3 PV(V3 p){return {p.x,p.y,p.z};}
static V3 VP(pc::Vec3 p){return {float(p.x),float(p.y),float(p.z)};}
static pc::Quat PQ(Q4 q){return {q.x,q.y,q.z,q.w};}
static Q4 QP(pc::Quat q){return {float(q.x),float(q.y),float(q.z),float(q.w)};}
static V3 Add(V3 a,V3 b){return VP(PV(a)+PV(b));}
static V3 Sub(V3 a,V3 b){return VP(PV(a)-PV(b));}
static V3 Mul(V3 a,float s){return VP(PV(a)*s);}
static float Len(V3 p){return float(pc::length(PV(p)));}
static V3 QRotateV(Q4 q,V3 p){return VP(pc::rotate(PQ(q),PV(p)));}
static V3 XrToCryV(V3 p){return {p.x,-p.z,p.y};}
static Q4 QFromQuatT(const QuatT28& p){return {p.q[0],p.q[1],p.q[2],p.q[3]};}
static V3 FromT(const QuatT28& p){return {p.t[0],p.t[1],p.t[2]};}
static QuatT28 Pose(pc::Vec3 p,pc::Quat q={}){return {{float(q.x),float(q.y),float(q.z),float(q.w)},{float(p.x),float(p.y),float(p.z)}};}
static bool ValidPose6689(const QuatT28& p){return pc::finite(PV(FromT(p)))&&bool(pc::unit(PQ(QFromQuatT(p))));}
static unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::printf("FAIL line=%d %s\n",__LINE__,#x);std::fflush(stdout);std::abort();}}while(0)
namespace kcd2vr::diagnostics::lab {static bool reference=false;static bool NativeReference(){return reference;}}
namespace kcd2vr::activity_input163 {struct W{bool active=false;std::atomic<unsigned>transitionDepth175{0};bool Active(){return active;}};static W witness175;}
namespace animation_filter {static std::atomic<bool>nativeBodyOwned112{false};static bool kick=false;static bool NativeKickActive175(){return kick;}}
namespace kcd2vr::frik_pose_probe {static int item[2]{};static int ExactHandItemState(int side){return item[side];}}
namespace wc {enum class Result{Clear,Hit,Busy,Invalid};}
namespace kcd2vr::touch_react_service60 {struct ScopedNpcQueryH9 {ScopedNpcQueryH9(int,bool){}};}
static std::atomic<bool>g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_reading{false},g_ctx_scripted{false};
static std::atomic<bool>g_auto_palm59c8{false};
static std::atomic<std::uint64_t>g_native_use_until107[2]{},g_weapon_request_epoch6694{1},g_pose_epoch6689{1};
static std::atomic<int>g_census_mode{4};
static std::atomic<void*>g_last_player_entity{reinterpret_cast<void*>(0x100000)};
static bool wristOwner[2]{},claim[2]{};
static bool (*g_wristOwnedByController59c12)(int)=+[](int side){return wristOwner[side];};
static bool HandClaimBlockedT24(int side,std::uintptr_t,std::uint64_t,std::uint64_t){return claim[side];}
static std::uint64_t clockD62=100;
static std::uint64_t GetTickCount64(){return clockD62;}
#include "display_context_d62.inc"
// The equipment/grip mailbox leaves below are modeled. The admission and
// profile assignment are extracted from actual PhysPrepare6698; ph::step is
// the production kernel. Empty-equipment tests never call the fill leaf.
struct PhysEquipmentItem6698{ph::Profile profile{};bool held=false;};
struct PhysEquipment6698 {
 std::array<PhysEquipmentItem6698,8>items{};int heldIndex[2]{-1,-1};
 std::uint64_t request=1,epoch=1,tick=100,handKey[2]{};
 bool valid=true,handKnown[2]{true,true};
};
struct PhysGrip6698{std::uint64_t key=0,generation=0;int nativeStamp=0,nativeSocket126=0;};
struct PhysGripBank6698{std::array<PhysGrip6698,8>grip{};};
struct PhysCorrelation126{int stamp=0,socket=0;};
static kcd2vr::core::SnapshotMailbox<PhysEquipment6698>g_phys_equipment6698;
static kcd2vr::core::SnapshotMailbox<PhysGripBank6698>g_phys_grips6698;
static std::atomic<bool>g_phys_armed6698{true},g_phys_fault6698{false};
static std::atomic<int>g_weapon_control_mode{7};
static std::atomic<unsigned>g_physical_gates101R2[2]{};
static unsigned fillProfileCalls=0;
static void PhysFillProfile6698(const PhysEquipmentItem6698& item,const PhysGrip6698&,ph::Profile& profile){++fillProfileCalls;profile=item.profile;}
#include "display_profile_d62.inc"
static ph::State displayPhysicalState;
static ph::Output displayPhysicalOutput;
static std::array<ph::Profile,2>displayPhysicalProfiles;
struct TrackingSnapshot {
 V3 right{.5f,.2f,-.4f},left{-.5f,.2f,-.4f};Q4 rq{},lq{};
 float right_grip=0,left_grip=0,right_trigger=0,left_trigger=0;
 bool allowed=true,head_valid=true,head_held=false,right_valid=true,left_valid=true,right_held=false,left_held=false;
 std::uint64_t sequence=2,tick=100,timeNs=100000000,source_epoch=1;std::int64_t poseTime=200000000;
};
struct Rig{int pelvis=0,spine3=1,head=2,upper_r=3,fore_r=4,hand_r=5,upper_l=6,fore_l=7,hand_l=8,item_r=9,item_l=10;};
struct Chain{int count=1;std::array<int,1>id{};};
struct State {
 static constexpr int Count=19;Rig rig{};unsigned count=Count;void*character=reinterpret_cast<void*>(0x200000);void*owner=reinterpret_cast<void*>(0x300000);
 std::array<QuatT28,Count>work{},local{},original{},restLocal{},baselineLocal{};std::array<int,Count>parent{};
 double upper[2]{.3,.3},fore[2]{.32,.32};bool deferPassengers137=true,dirtyAll=false;
 struct{unsigned serial=0;}pendingUse55;
 bool fingerMap[2]{true,true};Chain fingers[2][5]{};
 int fingerCount[2]{4,4};std::array<std::array<int,32>,2>fingerIds{};
 hd::Cache handDisplayD62{};
};
static State g_pose6689;
struct Tx {
 bool physicalActive=false,reloadWorkingHandXR34=false,acquisitionRequired57=false;
 ph::State& physicalState=displayPhysicalState;
 ph::Output& physicalOutput=displayPhysicalOutput;
 std::array<ph::Profile,2>& physicalProfiles=displayPhysicalProfiles;
 struct{bool active=false;}kickFrame176;
 std::uint64_t doorSerial66[2]{},epoch=1,policyGeneration57=2,control51=2;
 bool fingerTrialH7[2]{};float fingerOpenBiasH7[2]{};int smithFixture[2]{};
 bool handSemanticReadyD62=false,handSemanticSlotValidD62[2]{},handPresentOnlyD61[2]{};
 QuatT28 handSemanticD62[2]{},handSemanticSlotD62[2]{};
 std::uint64_t handPresentPolicyD61=0;std::int64_t handPresentTimeD62=0;
};
static bool OwnerCurrent(const Tx& tx){
#include "display_owner_d62.inc"
 return true;
}
struct ModelWorld6692{Q4 q{};V3 t{};float scale=1;};
static bool worldAvailable=true;static unsigned worldReads=0,fkCalls=0,rayCalls=0;static int failFkAt=-1;
static bool ReadModelWorld6692(void*,ModelWorld6692& x){++worldReads;x={};return worldAvailable;}
static V3 ModelToWorld6692(const ModelWorld6692& x,V3 p){return Add(QRotateV(x.q,Mul(p,x.scale)),x.t);}
static bool ComposeFull137(State& s){
 ++fkCalls;if(failFkAt>=0&&int(fkCalls)==failFkAt)return false;
 for(unsigned j=0;j<s.count;++j){
  const int parent=s.parent[j];
  if(parent<0)s.work[j]=s.local[j];
  else {const auto q=PQ(QFromQuatT(s.work[parent]));s.work[j]=Pose(PV(FromT(s.work[parent]))+pc::rotate(q,PV(FromT(s.local[j]))),pc::multiply(q,PQ(QFromQuatT(s.local[j]))));}
 }s.dirtyAll=false;return true;
}
static bool Compose6689(State& s){return ComposeFull137(s);}
static void SetWorldRotation6689(State& s,int j,Q4 q){const int p=s.parent[j];const auto local=p<0?PQ(q):pc::multiply(pc::conjugate(PQ(QFromQuatT(s.work[p]))),PQ(q));for(unsigned n=0;n<4;++n)s.local[j].q[n]=n==0?float(local.x):n==1?float(local.y):n==2?float(local.z):float(local.w);}
static bool RejectCoherent6693(Tx&,int){return false;}
struct FingerContactGeometry103{};
static Q4 DesiredHand6689(const State&,int,Q4,Q4 q){return q;}
static bool geometryAvailable=true;static float geometryBias=0;
static bool PalmGeometry103(const State&,int,Q4 q,float,float,int,V3(&p)[6],FingerContactGeometry103&,float=0,bool=false){
 if(!geometryAvailable)return false;
 const V3 local[6]{{0,.06f,0},{-.025f,.12f,0},{-.01f,.14f,0},{.008f,.14f,0},{.025f,.12f,0},{-.04f,.06f,0}};
 for(int j=0;j<6;++j)p[j]=QRotateV(q,Add(local[j],{geometryBias,0,0}));return true;
}
static bool SimplePalmGeometryR24(const State& s,int side,Q4 q,V3(&p)[6]){FingerContactGeometry103 g;return PalmGeometry103(s,side,q,0,0,0,p,g);}
struct Contact{bool fresh169r1=true,blocked=false,completeClear49=true;int planeCount=0;};
static bool g_contact_pass6695=true,g_contact_budget_active6695=true;
static bool contactSuspended=false;
static bool ContactSuspended59c1(){return contactSuspended;}
static struct{unsigned remaining=18;}g_contact_budget6695;
static int g_npc_query172=0;static wc::Result queryResult=wc::Result::Clear;static float wallX=100;
struct Ray{wc::Result state;};
static Ray WorldRayDistance6692(V3 from,V3 motion){++rayCalls;if(!g_contact_budget6695.remaining)return {wc::Result::Busy};--g_contact_budget6695.remaining;if(queryResult!=wc::Result::Clear)return {queryResult};return {from.x+motion.x>wallX?wc::Result::Hit:wc::Result::Clear};}
struct CommittedFrame6695 {
 struct{std::uint64_t epoch=1,sample=2,tick=100;std::uintptr_t actor=0x200000;}stamp;
 QuatT28 native[2]{},allowed[2]{},displayedD62[2]{},fistSlotH31[2]{};
 bool valid[2]{true,true},predictedDisplayD62[2]{},fistSlotValidH31[2]{},palmValid59c9[2]{};
 V3 palm59c9[2]{},palmDirection59c9[2]{};int smithFixture[2]{},fistWristBoneH31[2]{},fistSlotBoneH31[2]{};
 std::uintptr_t fistSkeletonH31=0,doorPlayer38=0x100000;unsigned fistJointCountH31=0;
 std::uint64_t displayPolicyD62=0,doorTrackingEpoch66=1,publicationR24=2;std::int64_t displayTargetTimeD62=0;
 float world[8]{0,0,0,1,0,0,0,1};
};
static CommittedFrame6695 latestPacket;static TrackingSnapshot live;
struct MiscGestureBodyD59 {std::uintptr_t player=0,character=0;std::uint32_t playerId=0;std::uint64_t tick=0,body=0,tracking=0;float palm[2][3]{};bool valid[2]{};};
static std::uintptr_t WhBase(){return 0x180000000;}
static bool CalibrationReady(){return true;}
namespace retail_build112 {static bool Ready(std::uintptr_t){return true;}}
static bool NativePassengerCurrentA34(std::uint64_t){return false;}
static bool ReadCommittedPacket6695(CommittedFrame6695& p){p=latestPacket;return true;}
static bool ReadTrackingSnapshot(TrackingSnapshot& t){t=live;return true;}
static bool TrackingHandUsable111(int side,std::uint64_t){return side?live.left_valid&&!live.left_held:live.right_valid&&!live.right_held;}
static std::uint64_t ReadCompletedPublicationT23(){return 2;}
static QuatT28 HandWorld6694(const QuatT28& p,const float*){return p;}
template<class T> static bool Read(const void* p,T& out){auto a=reinterpret_cast<std::uintptr_t>(p);if(a==0x100000){out=T(WhBase()+0x4072C78);return true;}if(a==0x10000C){out=T(42);return true;}return false;}
#include "../src/subsystems/interactions/hand_pose_service.inl"
struct Options {bool collision=true,performance=false,precise=true,smooth=false,reading=false,nativeCapture=false;int owner=-1;bool blocked=false,fresh=true,clear=true;};
static bool Run(Tx& tx,const TrackingSnapshot& ts,const Options& o={}){
 auto& s=g_pose6689;const auto& rig=s.rig;void*character=s.character;
 const bool smoothEnabledH50=o.smooth,readingVisual32=o.reading,nativeFingerCapture=o.nativeCapture;
 const bool collisionMode=o.collision,perfContactModeDs2=o.performance,preciseContactModeR24=o.precise;
 const bool valid[2]{ts.right_valid,ts.left_valid};const std::uint64_t contactControl=2;
 const V3 headAnchor123{0,0,1},trackedBase{};const Q4 frame{};const float wristBack59c7f=0;
 unsigned heldFingerKey[2]{};bool propHeld5916c1[2]{},presenceOwnedH24[2]{},nativePassengerHand46[2]{},doorPose66[2]{},doorPoseH40[2]{};
 float stagedHandoverD47[2]{};Contact stagedContact[2]{};
 V3 gestureShouldersR38[2]{FromT(s.work[rig.upper_r]),FromT(s.work[rig.upper_l])};
 if(o.owner>=0)heldFingerKey[o.owner]=17;
 for(auto& contact:stagedContact){contact.fresh169r1=o.fresh;contact.completeClear49=o.clear;}
 if(o.blocked){stagedContact[0].blocked=true;stagedContact[0].planeCount=1;stagedContact[0].completeClear49=false;}
 #include "../src/subsystems/vrik/pose_hand_display_d62.inl"
 return true;
}
static CommittedFrame6695 Publish(const Tx& tx,const TrackingSnapshot& ts){
 auto&s=g_pose6689;CommittedFrame6695 packet;const int hands[2]{s.rig.hand_r,s.rig.hand_l};
 #include "display_packet_d62.inc"
 packet.stamp.tick=ts.tick;packet.stamp.sample=ts.sequence;packet.doorTrackingEpoch66=ts.source_epoch;
 latestPacket=packet;live=ts;return packet;
}
static hp::Packet Packet(const TrackingSnapshot& ts,float shift=.025f){hp::Packet p;p.sourceEpoch=ts.source_epoch;p.sequence=ts.sequence;p.tick=ts.tick;p.ns=ts.timeNs;p.policyEpoch=hp::policy;p.headTime=p.baseHandTime=ts.poseTime;p.presentHandTime=ts.poseTime+22222222;p.tracked[0]=p.tracked[1]=true;p.hand[0]={{ts.left.x-shift,ts.left.y,ts.left.z},{0,0,0,1}};p.hand[1]={{ts.right.x+shift,ts.right.y,ts.right.z},{0,0,0,1}};return p;}
static void Init(bool enabled=true,float rightOffset=0){
 hp::SetEnabled(false);hp::SetEnabled(enabled);g_pose6689=State{};auto&s=g_pose6689;
 s.parent.fill(-1);s.local[0]=Pose({0,0,1});s.parent[1]=0;s.local[1]=Pose({0,0,.4});s.parent[2]=1;s.local[2]=Pose({0,0,.2});
 for(int side=0;side<2;++side){
  const int u=side?6:3,f=u+1,h=u+2,slot=side?10:9;const double sign=side?-1:1;
  const pc::Vec3 shoulder{sign*.2,0,1.4},hand{sign*.5+(side?0:rightOffset),.4,1.2};auto a=pc::solve_arm(shoulder,hand,.3,.32,{0,0,-1},{0,0,-1});assert(a);
  const auto uq=pc::from_to({1,0,0},a->elbow-shoulder),fq=pc::from_to({1,0,0},hand-a->elbow);
  s.parent[u]=1;s.local[u]=Pose(shoulder-pc::Vec3{0,0,1.4},uq);s.parent[f]=u;s.local[f]=Pose({.3,0,0},pc::multiply(pc::conjugate(uq),fq));
  s.parent[h]=f;s.local[h]=Pose({.32,0,0},pc::conjugate(fq));s.parent[slot]=h;s.local[slot]=Pose({0,.01,0});
  for(int finger=1;finger<5;++finger){int j=11+side*4+finger-1;s.parent[j]=h;s.local[j]=Pose({0,.06,0});s.fingers[side][finger].id[0]=j;s.fingerIds[side][finger-1]=j;}
 }
 failFkAt=-1;CHECK(ComposeFull137(s));s.original=s.work;s.restLocal=s.local;s.baselineLocal=s.local;
 worldAvailable=geometryAvailable=true;geometryBias=0;worldReads=fkCalls=rayCalls=0;
 displayPhysicalState=ph::State{};displayPhysicalOutput=ph::Output{};displayPhysicalProfiles={};fillProfileCalls=0;
 g_phys_armed6698=true;g_phys_fault6698=false;g_weapon_control_mode=7;
 for(int side=0;side<2;++side){kcd2vr::frik_pose_probe::item[side]=0;claim[side]=wristOwner[side]=false;g_native_use_until107[side]=0;}
 g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_reading=g_ctx_scripted=false;animation_filter::nativeBodyOwned112=false;animation_filter::kick=false;
 kcd2vr::activity_input163::witness175.active=false;kcd2vr::activity_input163::witness175.transitionDepth175=0;diagnostics::lab::reference=false;
 g_contact_pass6695=g_contact_budget_active6695=true;contactSuspended=false;g_contact_budget6695.remaining=18;queryResult=wc::Result::Clear;wallX=100;
 clockD62=100;tracking59c6::epoch=1;g_auto_palm59c8=false;
}
static void RestoreBaseline(){auto&s=g_pose6689;s.local=s.baselineLocal;CHECK(ComposeFull137(s));}
int main(){
 TrackingSnapshot t;Init(false);Tx off;CHECK(Run(off,t));auto baseline=Publish(off,t);CHECK(!off.handSemanticReadyD62&&fkCalls==0&&rayCalls==0&&worldReads==0);
 Init();CHECK(hp::Publish(Packet(t)));Tx on;CHECK(Run(on,t));auto shown=Publish(on,t);CHECK(on.handPresentOnlyD61[0]&&on.handPresentOnlyD61[1]);
 CHECK(Len(Sub(FromT(shown.displayedD62[0]),FromT(baseline.allowed[0])))>.024f);
 for(int side=0;side<2;++side){CHECK(Len(Sub(FromT(shown.allowed[side]),FromT(baseline.allowed[side])))<1e-7f);CHECK(pc::angle(PQ(QFromQuatT(shown.allowed[side])),PQ(QFromQuatT(baseline.allowed[side])))<1e-7);CHECK(Len(Sub(shown.palm59c9[side],baseline.palm59c9[side]))<1e-7f);CHECK(Len(Sub(FromT(shown.fistSlotH31[side]),FromT(baseline.fistSlotH31[side])))<1e-7f);}
 CHECK(shown.displayPolicyD62==hp::policy&&shown.displayTargetTimeD62>t.poseTime);CHECK(rayCalls==12);
 MiscGestureBodyD59 body;CHECK(ReadMiscGestureBodyD59(body));float x=0,y=0,z=0,dx=0,dy=0,dz=0;std::uint64_t tick=0;
 CHECK(ReadInteractionPalm59c9(0,x,y,z,dx,dy,dz,tick,nullptr,nullptr));CHECK(std::abs(x-baseline.palm59c9[0].x)<1e-7f&&tick==t.tick);
 CHECK(ReadInteractionHandWorld123R1(0,x,y,z));CHECK(std::abs(x-FromT(baseline.allowed[0]).x)<1e-7f);
 kcd2vr::washing_d59::Frame wash{};wash.forward[1]=1;wash.hmd[0]=body.palm[0][0]+.28f;wash.hmd[1]=body.palm[0][1]-.12f;wash.hmd[2]=body.palm[0][2]+.06f;
 CHECK(!kcd2vr::washing_d59::NearFace(body.palm[0],wash));
 // Same-family accepted reuse performs no more query work.
 auto expected=shown;RestoreBaseline();g_contact_budget6695.remaining=0;g_contact_pass6695=false;const auto rays=rayCalls;Tx repeated;Options repeatOptions;repeatOptions.fresh=false;CHECK(Run(repeated,t,repeatOptions));auto again=Publish(repeated,t);CHECK(repeated.handPresentOnlyD61[0]&&repeated.handPresentOnlyD61[1]);CHECK(rayCalls==rays);CHECK(Len(Sub(FromT(again.displayedD62[0]),FromT(expected.displayedD62[0])))<1e-7f);CHECK(hp::WantsSample(t.source_epoch,clockD62));
 // First absence/insufficient/busy decision is sealed: never promote later.
 for(int kind=0;kind<3;++kind){Init();if(kind)CHECK(hp::Publish(Packet(t)));if(kind==1)g_contact_budget6695.remaining=0;if(kind==2)queryResult=wc::Result::Busy;Tx a;CHECK(Run(a,t));CHECK(!a.handPresentPolicyD61);CHECK(hp::Publish(Packet(t)));g_contact_budget6695.remaining=18;queryResult=wc::Result::Clear;Tx b;CHECK(Run(b,t));CHECK(!b.handPresentPolicyD61);}
 // Every native owner and stale/invalid source yields independently per hand.
 for(int kind=0;kind<17;++kind){Init();auto active=t;Options o;Tx a;
  if(kind==0)active.right_grip=.2f;if(kind==1)active.right_trigger=.2f;if(kind==2)active.right_held=true;if(kind==3)active.right_valid=false;
  if(kind==4)kcd2vr::frik_pose_probe::item[0]=1;if(kind==5)kcd2vr::frik_pose_probe::item[0]=-1;if(kind==6)claim[0]=true;
  if(kind==7)wristOwner[0]=true;if(kind==8)g_native_use_until107[0]=101;if(kind==9)o.owner=0;if(kind==10)o.blocked=true;
  if(kind==11)o.smooth=true;if(kind==12)o.reading=true;if(kind==13){a.physicalActive=true;a.physicalOutput.owned[0]=true;}if(kind==14)clockD62=181;
  if(kind==15)active.head_held=true;if(kind==16)active.right_grip=std::numeric_limits<float>::quiet_NaN();
  CHECK(hp::Publish(Packet(active)));CHECK(Run(a,active,o));CHECK(!a.handPresentOnlyD61[0]);CHECK(Len(Sub(FromT(g_pose6689.work[5]),FromT(g_pose6689.original[5])))<1e-7f);
 }
 Init();wallX=.51f;CHECK(hp::Publish(Packet(t)));Tx wall;CHECK(Run(wall,t));CHECK(!wall.handPresentOnlyD61[0]);CHECK(Len(Sub(FromT(g_pose6689.work[5]),FromT(g_pose6689.original[5])))<1e-7f);
 Init();CHECK(hp::Publish(Packet(t,0)));Tx still;CHECK(Run(still,t));CHECK(!still.handPresentPolicyD61&&rayCalls==0&&fkCalls==0);
 Init();CHECK(hp::Publish(Packet(t,.06f)));Tx jump;CHECK(Run(jump,t));CHECK(!jump.handPresentPolicyD61&&rayCalls==0);
 Init();worldAvailable=false;CHECK(hp::Publish(Packet(t)));Tx noWorld;CHECK(Run(noWorld,t));CHECK(!noWorld.handPresentPolicyD61&&rayCalls==0&&!hp::WantsSample(t.source_epoch,clockD62));
 Init();geometryAvailable=false;CHECK(hp::Publish(Packet(t)));Tx noShape;CHECK(Run(noShape,t));CHECK(!noShape.handPresentPolicyD61&&rayCalls==0&&!hp::WantsSample(t.source_epoch,clockD62));
 Init();CHECK(hp::Publish(Packet(t)));failFkAt=1;Tx fail;CHECK(Run(fail,t));CHECK(!fail.handPresentOnlyD61[0]);CHECK(Len(Sub(FromT(g_pose6689.work[5]),FromT(g_pose6689.original[5])))<1e-7f);
 Init();CHECK(hp::Publish(Packet(t)));Tx own;CHECK(Run(own,t));CHECK(OwnerCurrent(own));wristOwner[0]=true;CHECK(!OwnerCurrent(own));wristOwner[0]=false;hp::SetEnabled(false);CHECK(!OwnerCurrent(own));
 // Native start/stop runs nested callbacks inside transition scope while
 // Active() can still be false. Both staging and final publication must yield.
 Init();CHECK(hp::Publish(Packet(t)));Tx beforeTransition;CHECK(Run(beforeTransition,t));CHECK(OwnerCurrent(beforeTransition));
 kcd2vr::activity_input163::witness175.transitionDepth175=1;CHECK(!kcd2vr::activity_input163::witness175.Active());CHECK(!OwnerCurrent(beforeTransition));
 RestoreBaseline();const auto beforeTransitionRays=rayCalls;Tx inTransition;CHECK(Run(inTransition,t));CHECK(!inTransition.handPresentPolicyD61&&rayCalls==beforeTransitionRays);
 // Once a source family loses current geometry/ownership it stays declined.
 Init();CHECK(hp::Publish(Packet(t)));Tx first;CHECK(Run(first,t));RestoreBaseline();wristOwner[0]=true;Tx revoke;CHECK(Run(revoke,t));CHECK(!revoke.handPresentOnlyD61[0]);RestoreBaseline();wristOwner[0]=false;Tx freeAgain;CHECK(Run(freeAgain,t));CHECK(!freeAgain.handPresentOnlyD61[0]);
 // Collision-qualified semantic endpoint remains distinct from raw intent.
 Init(true,-.02f);CHECK(hp::Publish(Packet(t)));Tx clipped;Options clippedOptions;clippedOptions.blocked=true;
 CHECK(Run(clipped,t,clippedOptions));auto collisionPacket=Publish(clipped,t);
 CHECK(std::abs(FromT(collisionPacket.allowed[0]).x-.48f)<1e-6f);
 CHECK(std::abs(FromT(collisionPacket.allowed[0]).x-t.right.x)>.019f);
 CHECK(!collisionPacket.predictedDisplayD62[0]);
 // Leftover credits can admit one hand, never promote the other on a repeat.
 Init();g_contact_budget6695.remaining=6;CHECK(hp::Publish(Packet(t)));Tx one;CHECK(Run(one,t));CHECK(one.handPresentOnlyD61[0]&&!one.handPresentOnlyD61[1]&&rayCalls==6);
 RestoreBaseline();g_contact_budget6695.remaining=18;Tx stillOne;CHECK(Run(stillOne,t));CHECK(stillOne.handPresentOnlyD61[0]&&!stillOne.handPresentOnlyD61[1]&&rayCalls==6);
 // Pose/world geometry changing within a family revokes its optional decision.
 Init();CHECK(hp::Publish(Packet(t)));Tx firstGeometry;CHECK(Run(firstGeometry,t));RestoreBaseline();g_pose6689.local[0].t[0]+=.001f;CHECK(ComposeFull137(g_pose6689));Tx changedGeometry;CHECK(Run(changedGeometry,t));CHECK(!changedGeometry.handPresentPolicyD61);RestoreBaseline();Tx restoredGeometry;CHECK(Run(restoredGeometry,t));CHECK(!restoredGeometry.handPresentPolicyD61);
 // Identical arm endpoints do not hide changed fingers/curve/contact results.
 for(int kind=0;kind<7;++kind){
  Init();CHECK(hp::Publish(Packet(t)));Tx initial;CHECK(Run(initial,t));CHECK(initial.handPresentOnlyD61[0]);RestoreBaseline();const auto beforeRays=rayCalls;
  Options altered;
  if(kind==0){const auto q=pc::from_rotation_vector({.1,0,0});g_pose6689.local[11]=Pose({0,.06,0},q);CHECK(ComposeFull137(g_pose6689));}
  if(kind==1)geometryBias=.001f;if(kind==2)altered.blocked=true;if(kind==3)altered.clear=false;
  if(kind==4)geometryAvailable=false;if(kind==5)contactSuspended=true;if(kind==6)worldAvailable=false;
  Tx changed;CHECK(Run(changed,t,altered));CHECK(!changed.handPresentOnlyD61[0]&&rayCalls==beforeRays);CHECK(!hp::WantsSample(t.source_epoch,clockD62));
  RestoreBaseline();geometryBias=0;geometryAvailable=worldAvailable=true;contactSuspended=false;g_contact_budget6695.remaining=18;
  Tx restored;CHECK(Run(restored,t));CHECK(!restored.handPresentOnlyD61[0]&&rayCalls==beforeRays);
 }
 // Global source serials may evict a cache slot within the 80ms freshness
 // window. The old family cannot reopen after packet/budget become available.
 Init();Tx declined;CHECK(Run(declined,t));CHECK(!declined.handPresentPolicyD61);
 auto colliding=t;colliding.sequence+=2*hp::kSlots;Tx newer;CHECK(Run(newer,colliding));CHECK(!newer.handPresentPolicyD61);
 CHECK(hp::Publish(Packet(t)));Tx delayed;CHECK(Run(delayed,t));CHECK(!delayed.handPresentPolicyD61&&rayCalls==0);
 // Retained entries may reuse their sealed acceptance after a newer family;
 // changed policy/owner identity at the old serial cannot buy a new decision.
 Init();CHECK(hp::Publish(Packet(t)));Tx retained;CHECK(Run(retained,t));RestoreBaseline();
 auto following=t;following.sequence+=2;Tx later;CHECK(Run(later,following));const auto retainedRays=rayCalls;
 Tx oldRetained;CHECK(Run(oldRetained,t));CHECK(oldRetained.handPresentOnlyD61[0]&&rayCalls==retainedRays);
 RestoreBaseline();hp::SetEnabled(false);hp::SetEnabled(true);CHECK(hp::Publish(Packet(t)));Tx policyChanged;CHECK(Run(policyChanged,t));CHECK(!policyChanged.handPresentPolicyD61&&rayCalls==retainedRays);
 // Existing auto-palm and wrist orientation mapping remains before the late solve.
 Init();g_auto_palm59c8=true;auto autoPalm=t;autoPalm.right.z=autoPalm.left.z=-.43f;CHECK(hp::Publish(Packet(autoPalm)));Tx palm;CHECK(Run(palm,autoPalm));auto palmPacket=Publish(palm,autoPalm);CHECK(palm.handPresentOnlyD61[0]);CHECK(Len(Sub(FromT(palmPacket.allowed[0]),FromT(baseline.allowed[0])))<1e-7f);
 Init();auto rotated=Packet(t);const auto turn=pc::from_rotation_vector({0,0,.1});rotated.hand[1].q[0]=float(turn.x);rotated.hand[1].q[1]=float(turn.y);rotated.hand[1].q[2]=float(turn.z);rotated.hand[1].q[3]=float(turn.w);CHECK(hp::Publish(rotated));Tx turnTx;CHECK(Run(turnTx,t));auto turnPacket=Publish(turnTx,t);CHECK(turnTx.handPresentOnlyD61[0]);CHECK(pc::angle(PQ(QFromQuatT(turnPacket.allowed[0])),PQ(QFromQuatT(baseline.allowed[0])))<1e-7);CHECK(pc::angle(PQ(QFromQuatT(turnPacket.displayedD62[0])),turn)<1e-5);
 // Context / tracking faults cannot start optional queries or extra FK.
 for(int kind=0;kind<6;++kind){Init();CHECK(hp::Publish(Packet(t)));if(kind==0)g_ctx_menu=true;if(kind==1)g_ctx_dialogue=true;if(kind==2)g_ctx_scripted=true;if(kind==3)animation_filter::nativeBodyOwned112=true;if(kind==4)kcd2vr::activity_input163::witness175.active=true;if(kind==5)diagnostics::lab::reference=true;Tx prohibited;CHECK(Run(prohibited,t));CHECK(!prohibited.handPresentPolicyD61&&rayCalls==0&&fkCalls==0&&worldReads==0);}
 // Public-mode composition executes actual profile admission/assignment,
 // then the real physical kernel, before entering the actual late adapter.
 for(unsigned mode=1;mode<=14;++mode){
  Init();const auto c=kcd2vr::user175::configuration(kcd2vr::user175::Mode(mode));
  CHECK(kcd2vr::npc_contact172::selected(0,c.weapon,false,c.pose));g_census_mode=c.pose;g_weapon_control_mode=c.weapon;
  CHECK(g_phys_equipment6698.publish(PhysEquipment6698{}));CHECK(g_phys_grips6698.publish(PhysGripBank6698{}));
  ph::Input input;input.epoch=input.bodyEpoch=1;input.sample=t.sequence;input.tick=t.tick;input.timeNs=t.timeNs;
  input.allowed=input.tracked[0]=input.tracked[1]=true;
  for(int side=0;side<2;++side){input.hands[side]={PQ(QFromQuatT(g_pose6689.work[side?8:5])),PV(FromT(g_pose6689.work[side?8:5]))};input.shoulders[side]=PV(FromT(g_pose6689.work[side?6:3]));}
  PhysEquipment6698 equipment;Tx composed;
  composed.physicalActive=PhysPrepare6698(input,composed.physicalProfiles,equipment);
  CHECK(composed.physicalActive==(c.weapon>=4));unsigned physicalQueries=0;
  if(composed.physicalActive)composed.physicalOutput=ph::step(composed.physicalState,input,composed.physicalProfiles,
    [&](const ph::Profile&,ph::Pose,ph::Pose,bool){++physicalQueries;return ph::Contact103{};});
  CHECK(fillProfileCalls==0&&physicalQueries==0&&composed.physicalOutput.commandCount==0);
  CHECK(hd::IdlePhysical(composed.physicalActive,composed.physicalProfiles,composed.physicalState,composed.physicalOutput));
  CHECK(hp::Publish(Packet(t)));CHECK(Run(composed,t));CHECK(composed.handPresentOnlyD61[0]&&composed.handPresentOnlyD61[1]);
 }
 // Every actual physical ownership/transition component vetoes the display
 // experiment while leaving all semantic/kernel data untouched.
 for(int kind=0;kind<10;++kind){
  Init();Tx busy;busy.physicalActive=true;
  if(kind==0)busy.physicalProfiles[0].key=17;if(kind==1)busy.physicalState.weapon[1].key=17;
  if(kind==2)busy.physicalOutput.owned[0]=true;if(kind==3)busy.physicalOutput.active[1]=true;
  if(kind==4)busy.physicalOutput.support[0]=true;if(kind==5)busy.physicalOutput.solverOwnedT4[1]=true;
  if(kind==6)busy.physicalOutput.bowHand169r2=true;if(kind==7)busy.physicalOutput.commandCount=1;
  if(kind==8)busy.physicalOutput.displayLatched[0]=true;if(kind==9)busy.physicalOutput.displayRejoinR13[1]=true;
  CHECK(hp::Publish(Packet(t)));CHECK(Run(busy,t));CHECK(!busy.handPresentPolicyD61&&rayCalls==0&&worldReads==0&&fkCalls==0);
 }
 // Unknown native membership is a nonzero sentinel profile, never EMPTY.
 Init();PhysEquipment6698 uncertain;uncertain.handKnown[0]=false;CHECK(g_phys_equipment6698.publish(uncertain));
 ph::Input uncertainInput;Tx uncertainTx;CHECK(PhysPrepare6698(uncertainInput,uncertainTx.physicalProfiles,uncertain));
 uncertainTx.physicalActive=true;CHECK(uncertainTx.physicalProfiles[0].key==~std::uint64_t(0));
 CHECK(hp::Publish(Packet(t)));CHECK(Run(uncertainTx,t));CHECK(!uncertainTx.handPresentPolicyD61&&rayCalls==0);
 // Producer interest is explicitly current, not a permanent menu selection.
 Init();hp::NoteConsumer(1,100,3);CHECK(hp::WantsSample(1,180));CHECK(!hp::WantsSample(1,181));CHECK(!hp::WantsSample(2,110));hp::SetEnabled(false);CHECK(!hp::WantsSample(1,110));
 // Actual raw publication: optional receipt is sealed BEFORE raw visibility.
 Init();tracking59c6::Configure(true);tracking59c6::Frame raw{};raw.epoch=tracking59c6::epoch.load();raw.tick=90;raw.ns=90000000;raw.locateTime=200000000;raw.controllersSampled=true;raw.valid.fill(true);raw.tracked.fill(true);CHECK(tracking59c6::Publish(raw));
 raw.tick=100;raw.ns=100000000;tracking59c6::Frame complete{};bool sealed=false;
 CHECK(tracking59c6::PublishPrepared(raw,&complete,[&](const tracking59c6::Frame& prepared){tracking59c6::Frame visible{};CHECK(tracking59c6::Read(visible));CHECK(visible.sequence!=prepared.sequence);auto p=Packet(t);p.sourceEpoch=prepared.epoch;p.sequence=prepared.sequence;p.tick=prepared.tick;p.ns=prepared.ns;CHECK(hp::Publish(p));sealed=true;}));
 CHECK(sealed);hp::Packet receipt{};CHECK(hp::Read(complete.epoch,complete.sequence,complete.tick,complete.ns,complete.locateTime,100,receipt));
 for(unsigned i=0;i<3;++i)CHECK(std::memcmp(&complete.poses[i],&raw.poses[i],sizeof(raw.poses[i]))==0);
 hp::SetEnabled(false);
 std::printf("D62_HAND_DISPLAY_BRIDGE PASS checks=%u adapter=VERBATIM readers=VERBATIM native_query_fk_leaves=MODELED game_openxr_headset=NOT_RUN\n",checks);
}
