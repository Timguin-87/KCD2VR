// Execute the real coherent pose/physical kernel. Native scene/service leaves
// are the existing foundation fixture's explicit in-memory boundaries.
#include "../src/vrik/visual_selection59c7f.h"
#ifndef KCD2VR_H50_FOUNDATION
#define KCD2VR_H50_FOUNDATION "c54_foundation_restore_h26_test.cpp"
#endif
#define KCD2VR_H26_TEST_MAIN retained_h26_main
#include KCD2VR_H50_FOUNDATION
#undef KCD2VR_H26_TEST_MAIN
#include <fstream>
namespace hs=kcd2vr::hand_smooth_h50;
static std::vector<char> orderH50;
static bool OwnerOrderH50(int side)noexcept{orderH50.push_back(side?'L':'R');return false;}
static void ContactOrderH50(){orderH50.push_back('C');}
struct FrameH50 {
 std::array<QuatT28,Count> work{};ph::Pose requested[2]{},physicalHands[2]{},intent[2]{};
 pc::Vec3 anchor{},root{};QuatT28 head{};bool ok=false,physical=false,acquisition=false;
};
static FrameH50 FrameH50Run(TrackingSnapshot& t,int rung=3){
 ++t.sequence;t.tick=frameClock51+=10;t.timeNs=t.tick*1000000ull;
 PublishPoseContext55(poseContext55);CoherentTransaction6689 tx;
 FrameH50 out{};out.ok=BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,rung,t);
 if(out.ok){std::copy_n(g_pose6689.work.begin(),Count,out.work.begin());out.head=g_pose6689.work[rig.head];out.anchor=tx.frame51.anchor;out.root=tx.frame51.root;
  for(int side=0;side<2;++side){out.requested[side]=tx.propRequested5916c1[side];out.physicalHands[side]=tx.physicalInput.hands[side];out.intent[side]=tx.physicalInput.modelIntentHandsH30[side];}
  out.physical=tx.physicalActive;out.acquisition=tx.acquisitionRequired57;}
 tx.Restore();if(out.ok)PublishCommitted6689(tx);return out;
}
static bool SamePoseH50(const ph::Pose&a,const ph::Pose&b){return pc::length(a.p-b.p)<1e-8&&pc::angle(a.q,b.q)<1e-7;}
static void StartH50(bool enabled){InitFrame51();hs::enabled=enabled;hs::minCutoffHz=1.5f;hs::beta=20.f;hs::rotBeta=5.f;
 g_wristOwnedByController59c12=nullptr;mockContactHookH50=nullptr;pendingReady55=false;pickupWitnessReady54=false;
 g_doorActive66=nullptr;g_doorGoal66=nullptr;kcd2vr::reading_pose32::blocked=true;
 kcd2vr::prop5916c1::clearPresentation();workstation178r20c1::experimentArmed36=false;
 kcd2vr::combat_rules178r10::f5Rung=0;kcd2vr::combat_rules178r10::workstationEnabled=true;
 kcd2vr::minigame_hands45::witness=0;kcd2vr::minigame_hands45::mode=kcd2vr::minigame_hands45::SemiPassenger;
 for(auto& a:kcd2vr::activity_input163::witness175.active)a=0;
}
static void OwnerH50(int scenario,int n){
 if(n<6)return;
 if(scenario>=2&&scenario<=4){g_weapon_control_mode=4;mockPhysical=true;const int side=scenario==4?1:0;
  auto& p=mockProfiles[side];p.key=777;p.side=side;p.primary=true;p.classId=scenario==3?7:scenario==4?9:4;
  p.geometry=true;p.tip={0,scenario==3?1.7:.8,0};p.axis={0,1,0};p.generation=3;
  mockHeldKey164R1[side]=777;mockItemState164R1[side]=1;
 }
 if(scenario==5)g_native_use_until107[0]=frameClock51+100;
 if(scenario==6){mockHeldKey164R1[0]=888;mockItemState164R1[0]=1;}
 if(scenario==7){g_wristOwnedByController59c12=+[](int side)noexcept{return side==0;};}
 if(scenario==10){
  g_doorActive66=+[](int side,std::uint64_t,std::uint64_t,std::uint64_t&serial)noexcept{if(side!=0)return false;serial=77;return true;};
  g_doorGoal66=+[](int side,std::uint64_t,std::uint64_t,pc::LocalPose&pose,pc::Vec3&handle,std::uint64_t&serial,int&nativeSide)noexcept{pose={{0,0,0,1},{100.34,200.22,11.2}};handle=pose.fixed_offset;serial=77;nativeSide=0;return side==0;};
 }
 if(scenario==11){
  workstation178r20c1::experimentArmed36=true;kcd2vr::combat_rules178r10::f5Rung=2;
  kcd2vr::prop5916c1::Presentation p{};p.character=reinterpret_cast<std::uintptr_t>(character.data());p.body=0x7000;p.key=888;p.bodyEpoch=g_pose_epoch6689.load();p.experimentEpoch=workstation178r20c1::experimentEpoch36.load();p.side=0;p.observationTick=p.sourceTick=frameClock51+10;p.sequence=1;p.wristWorld={{0,0,0,1},{100.34,200.22,11.2}};
  kcd2vr::prop5916c1::presentation.publish(p);
 }
 if(scenario==12){
  kcd2vr::minigame_hands45::publish(kcd2vr::minigame_hands45::Smithing,frameClock51+10);
  kcd2vr::activity_input163::witness175.active[unsigned(kcd2vr::activity_input163::Kind175::Smithing)]=0x9000;
 }
 if(scenario==9){
  if(n==6){pickupWitness54={reinterpret_cast<std::uintptr_t>(character.data()),0x2000,0x3000,23,g_pose_epoch6689.load(),frameClock51+1,frameClock51+1,17,0,true};pickupWitness54.trackingEpoch55=kcd2vr::tracking59c6::epoch.load();pickupWitnessReady54=true;g_graspAcquisition54=PickupWitness54;}
 }
 if(scenario==8){pendingWitness55={reinterpret_cast<std::uintptr_t>(character.data()),g_pose_epoch6689.load(),55,frameClock51,0};pendingReady55=true;g_graspPendingUse55=PendingWitness55;}
}
static std::vector<FrameH50> SequenceH50(bool enabled,int scenario){
 StartH50(enabled);auto t=Tracking();t.source_epoch=kcd2vr::tracking59c6::epoch.load();
 if(scenario==1)g_arm_profile66100.publish(kcd2vr::calibration_profile::Arms{1.50,.75,.75,.20});
 g_wrist_from_grip59c7f=scenario%2?.03f:0.f;g_auto_palm59c8=scenario%3==0;
 std::vector<FrameH50> result;
 for(int n=0;n<24;++n){OwnerH50(scenario,n);const float v=float(n)*.19f;
  t.right.x=.36f+.0015f*std::sin(v);t.left.x=-.36f-.001f*std::sin(v*.7f);
  t.right.z=-.23f-.001f*std::cos(v);t.left.z=-.23f+.0008f*std::cos(v*.8f);
  t.rq=QP(pc::from_rotation_vector({0,.001*std::sin(v),0}));
  t.lq=QP(pc::from_rotation_vector({.001*std::cos(v),0,0}));
  t.right_grip=t.left_grip=1.f;
  result.push_back(FrameH50Run(t,scenario==1?2:3));
 }
 return result;
}
static void DumpH50(const char* path){std::ofstream out(path,std::ios::binary);
 for(int scenario=0;scenario<13;++scenario){auto frames=SequenceH50(false,scenario);
  for(const auto& f:frames){out.write(reinterpret_cast<const char*>(f.work.data()),sizeof(f.work));
   for(int side=0;side<2;++side)for(const auto* pose:{&f.requested[side],&f.physicalHands[side],&f.intent[side]}){
    const double p[]{pose->q.x,pose->q.y,pose->q.z,pose->q.w,pose->p.x,pose->p.y,pose->p.z};out.write(reinterpret_cast<const char*>(p),sizeof(p));}
   const double anchor[]{f.anchor.x,f.anchor.y,f.anchor.z,f.root.x,f.root.y,f.root.z};out.write(reinterpret_cast<const char*>(anchor),sizeof(anchor));
   const unsigned status[]{unsigned(f.ok),unsigned(f.physical),unsigned(f.acquisition)};out.write(reinterpret_cast<const char*>(status),sizeof(status));}}
}
#ifndef KCD2VR_H50_PARENT
struct ReachH50 {V3 requested[2]{},intent[2]{},shown[2]{};};
static ReachH50 ReachH50Run(const TrackingSnapshot& ts,const V3 visualController[2]){
 auto&s=g_pose6689;CoherentTransaction6689 tx;const Q4 frame{};const V3 headTarget{};const bool full=false,prepared169r2=false,manualPose169r2=false;
 const bool valid[2]{true,true};const V3 bowGoals169r2[2]{};
 // Equal requested goals but different controllers: H17 uses controller input
 // independently, so equality of these goals alone cannot skip the raw pass.
 const V3 requestedGoals[2]{{.3f,.2f,1.2f},{-.3f,.2f,1.2f}};
 const V3 requestedGoalsRawH50[2]{requestedGoals[0],requestedGoals[1]};
 const V3 visPH50[2]{visualController[0],visualController[1]};V3 goals[2]{requestedGoals[0],requestedGoals[1]};
#include "../src/subsystems/vrik/pose_reach_input.inl"
 ReachH50 result{};for(int side=0;side<2;++side){result.requested[side]=VP(tx.propRequested5916c1[side].p);result.intent[side]=modelIntentGoalsH30[side];result.shown[side]=goals[side];}return result;
}
#endif
int main(int argc,char**argv){
 if(argc==2&&std::strcmp(argv[1],"--retained-on")==0){hs::enabled=true;return retained_h26_main();}
 if(argc==2&&std::strcmp(argv[1],"--retained-off")==0){hs::enabled=false;return retained_h26_main();}
 if(argc==3&&std::strcmp(argv[1],"--dump")==0){DumpH50(argv[2]);return 0;}
#ifdef KCD2VR_H50_PARENT
 return 0;
#else
 tests=failures=0;bool allCommit=true,rawRequests=true,rawIntent=true,rawPhysical=true,head=true,changedFree=false,excludedOwners=true,actualAcquisition=true;
 for(int scenario=0;scenario<13;++scenario){const auto off=SequenceH50(false,scenario),on=SequenceH50(true,scenario);
  for(unsigned n=0;n<on.size();++n){const auto&a=off[n],&b=on[n];allCommit=allCommit&&a.ok&&b.ok;
   rawRequests=rawRequests&&SamePoseH50(a.requested[0],b.requested[0])&&SamePoseH50(a.requested[1],b.requested[1]);
   rawIntent=rawIntent&&SamePoseH50(a.intent[0],b.intent[0])&&SamePoseH50(a.intent[1],b.intent[1]);
   rawPhysical=rawPhysical&&SamePoseH50(a.physicalHands[0],b.physicalHands[0])&&SamePoseH50(a.physicalHands[1],b.physicalHands[1]);
   head=head&&pc::length(a.anchor-b.anchor)<1e-8&&pc::length(a.root-b.root)<1e-8&&Len(Sub(FromT(a.head),FromT(b.head)))<1e-5&&pc::angle(PQ(QFromQuatT(a.head)),PQ(QFromQuatT(b.head)))<1e-7;
   if(scenario==0&&n>3)changedFree=changedFree||Len(Sub(FromT(a.work[rig.hand_r]),FromT(b.work[rig.hand_r])))>.00001;
   if(scenario==9&&n>=6)actualAcquisition=actualAcquisition&&a.acquisition&&b.acquisition;
   if(scenario>=2&&n>=6){const int hand=scenario==4?rig.hand_l:rig.hand_r;excludedOwners=excludedOwners&&Len(Sub(FromT(a.work[hand]),FromT(b.work[hand])))<.0001;}
  }
 }
 Check(actualAcquisition,"acquisition case enters the actual current native-grasp owner rather than only a pending latch");
 Check(allCommit,"all actual shared/legacy/free/weapon/bow/native-owner pose frames commit");
 Check(changedFree,"enabled experiment changes a free-hand achieved wrist through the real pose pipeline");
 Check(rawRequests,"all actual H40/acquisition/prop requested poses retain independently raw reach mapping");
 Check(rawIntent,"all actual H30 model intent remains raw before reach clamping");
 Check(rawPhysical,"actual physical kernel targets remain raw including first sword/heavy/bow acquisition");
 Check(head,"actual head position/quaternion and shared body/head frame anchor stay unchanged");
 Check(excludedOwners,"newly current weapon/native-held/interaction owners keep their current raw wrist boundary");
 StartH50(true);auto t=Tracking();Check(FrameH50Run(t).ok,"filter establishes a committed free-hand owner");
 t.right.x+=.001f;Check(FrameH50Run(t).ok,"filter seeds on the next committed source sample");
 t.right.x+=.001f;Check(FrameH50Run(t).ok,"filter advances on slow motion");
 const auto before=g_pose6689.handSmoothH50[0];const auto freeBefore=g_pose6689.handFreeH50[0];
 failAt=rig.hand_r;failOnce=false;t.right.x+=.001f;Check(!FrameH50Run(t).ok,"actual failed native bone write rejects the staged frame");
 Check(std::memcmp(&before,&g_pose6689.handSmoothH50[0],sizeof(before))==0&&freeBefore==g_pose6689.handFreeH50[0],"rejected frame cannot advance committed filter history or free ownership");
 failAt=-1;t.right.x+=.001f;Check(FrameH50Run(t).ok,"a rejected frame does not block the next current pose");
 ++g_pose_epoch6689;t.right.x+=.001f;Check(FrameH50Run(t).ok,"current body epoch may reacquire without replaying old filtered state");
 Check(g_pose6689.handSmoothH50[0].epoch==g_pose_epoch6689&&std::fabs(g_pose6689.handSmoothH50[0].out[0]-t.right.x)<1e-7,"new body epoch restarts this visual filter at the current raw controller");
 StartH50(true);t=Tracking();FrameH50Run(t);t.right.x+=.001f;FrameH50Run(t);t.right.x+=.001f;FrameH50Run(t);
 g_pose6689.ready=false;g_pose6689.owner=nullptr;t.right.x+=.001f;Check(FrameH50Run(t).ok,"anatomical owner replacement reconstructs actual coherent state");
 Check(!g_pose6689.handSmoothH50[0].ready,"a replacement anatomy owner cannot inherit another body's visual history");
 StartH50(true);t=Tracking();FrameH50Run(t);g_arm_profile66100.publish(kcd2vr::calibration_profile::Arms{1.50,.75,.75,.20});
 const V3 raw[2]{t.right,t.left},visual[2]{Add(t.right,{.01f,0,0}),t.left};
 const auto reference=ReachH50Run(t,raw),split=ReachH50Run(t,visual);
 Check(Len(Sub(reference.requested[0],split.requested[0]))<1e-8&&Len(Sub(reference.intent[0],split.intent[0]))<1e-8,"actual H17 raw mapping is unchanged when equal requested goals conceal different controller inputs");
 Check(Len(Sub(reference.shown[0],split.shown[0]))>.001,"H17 counterexample actually exercises distinct visual controller input");
 StartH50(true);t=Tracking();FrameH50Run(t);t.right.x+=.001f;FrameH50Run(t);t.right.x+=.001f;FrameH50Run(t);
 hs::enabled=false;t.right.x+=.001f;Check(FrameH50Run(t).ok&&!g_pose6689.handSmoothH50[0].ready,"turning the experiment OFF retires only its visual history");
 hs::enabled=true;t.right.x+=.001f;const auto restarted=FrameH50Run(t);Check(restarted.ok&&!g_pose6689.handSmoothH50[0].ready&&Len(Sub(FromT(restarted.work[rig.hand_r]),VP(restarted.requested[0].p)))<.0001,"re-enabling within100ms starts at raw instead of resuming stale filtered output");
 for(bool enabled:{false,true}){
  StartH50(enabled);t=Tracking();mockCollision=true;g_wristOwnedByController59c12=OwnerOrderH50;mockContactHookH50=ContactOrderH50;orderH50.clear();
  Check(FrameH50Run(t,4).ok,"callback-order witness runs the actual contact and handover pipeline");
  const std::vector<char> expected=enabled?std::vector<char>{'R','L','C','C'}:std::vector<char>{'C','C','R','L'};
  Check(orderH50==expected,enabled?"enabled visual-only scan samples each existing owner once before contact":"OFF keeps both original owner callback calls after contact, with no extra early scan");
 }
 StartH50(true);t=Tracking();mockCollision=true;mockWallX=.35f;g_auto_palm59c8=true;
 bool contactBound=true;for(int n=0;n<8;++n){t.right.x=.38f+.0002f*n;const auto f=FrameH50Run(t,4);contactBound=contactBound&&f.ok&&FromT(f.work[rig.hand_r]).x<=mockWallX+.0001f;}
 Check(contactBound,"actual free-hand contact receives the filtered target and no post-contact filter reopens wall penetration");
 std::printf("H50_PRODUCTION_PIPELINE checks=%d failures=%d native_services=MODELED headset=NOT_RUN\n",tests,failures);
 return failures?1:0;
#endif
}
