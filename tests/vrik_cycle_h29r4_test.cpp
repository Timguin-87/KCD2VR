// Real full coherent transaction, finger writer and elbow caller. Existing
// in-memory native getter/setter/world leaves do not emulate the retail engine.
static unsigned long long f7NowH29R4=10000;
#define KCD2VR_POSE_FIXTURE_CLOCK f7NowH29R4
#define main retained_pose_adapter_main_f7_h29r4
#include "pose_adapter_test.cpp"
#undef main
namespace vd4=kcd2vr::diagnostics::vrik_h29;
namespace cap4=kcd2vr::diagnostics::capture168;
static std::vector<vd4::Record> records4(){std::vector<vd4::Record> out;vd4::Record r{};while(vd4::pop(r))out.push_back(r);return out;}
static void arm4(bool on=true){cap4::gate.select(cap4::Domain::Vrik,on,f7NowH29R4);records4();}
static TrackingSnapshot source4(){auto t=Tracking();t.source_epoch=kcd2vr::tracking59c6::epoch.load();
 t.tick=f7NowH29R4;t.input_tick=t.tick-3;t.head_good_tick=t.tick-1;t.right_good_tick=t.tick-2;t.left_good_tick=t.tick-4;t.timeNs=t.tick*1000000ull;return t;}
static bool run4(TrackingSnapshot t,bool publish=true,int owner=0,bool nativeReturned=true){
 CoherentTransaction6689 tx;const bool ok=BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,3,t);
 tx.traceNativeCalledH29=tx.traceNativeReturnedH29=nativeReturned; // explicit original-body fixture leaf
 tx.Restore();if(owner)g_pose_epoch6689.fetch_add(1);if(publish)PublishCommitted6689(tx);return ok;
}
int main(){tests=failures=0;Init();arm4(false);auto t=source4();Check(run4(t),"F7 OFF real pose publishes");
 const auto offPose=g_pose6689.work,offLocal=g_pose6689.local;const auto offWrites=writeCalls;
 Check(records4().empty(),"F7 OFF emits no copied receipt");
 Init();arm4();t=source4();Check(run4(t),"F7 ON same actual transaction publishes");auto events=records4();
 Check(events.size()==1,"one complete-cycle receipt after transaction ends");auto r=events.at(0);
 Check(r.started&&r.committed&&r.nativeCalled&&r.nativeReturned&&r.restored&&r.restoreOk&&r.restoreWrites>0&&
  r.publishAttempted&&r.packetPublished&&r.published&&r.envelopePublished,"scoped write, original, restore, body and render publication separately proven");
 Check(std::strcmp(r.end,"PUBLISHED")==0&&r.sourceTick==t.tick&&r.inputTick==t.input_tick&&r.timeNs==t.timeNs&&
  r.goodHead==t.head_good_tick&&r.goodRight==t.right_good_tick&&r.goodLeft==t.left_good_tick,"all original source/good/input clocks survive complete cycle");
 Check(pc::length(r.rawPosition[0]-PV(t.head))==0&&pc::length(r.rawPosition[1]-PV(t.right))==0&&pc::length(r.rawPosition[2]-PV(t.left))==0&&
  pc::angle(r.rawRotation[1],PQ(t.rq))<1e-12&&r.worldKnown&&r.modelWorld[4]==100&&r.modelWorld[5]==200&&r.modelWorld[6]==10,
  "original raw XR poses and validated current model-world copied without new native read");
 Check(std::memcmp(offPose.data(),g_pose6689.work.data(),sizeof(offPose))==0&&std::memcmp(offLocal.data(),g_pose6689.local.data(),sizeof(offLocal))==0&&offWrites==writeCalls&&Same(fixture.live,fixture.original),"F7 ON/OFF byte-identical solved pose, finger locals, writes and native restoration");
 for(int side=0;side<2;++side){const auto& a=r.arms[side];
  Check(a.context.entered&&a.context.conditionAttemptedD44&&a.context.conditionedOk&&a.context.continuityOk&&a.context.solveOk,"actual bilateral elbow branch and solve outcomes observed");
  Check(a.context.finger==vd4::FingerOwner::NativeMissingReference&&a.context.fingerCall&&a.context.fingerOk,
   "true-but-native missing finger reference is explicit, not a fabricated finger write");
 }
 // Captured readiness, ordinary free curls, owner transition and prior continuity.
 f7NowH29R4+=300;Init();arm4();t=source4();Check(run4(t),"prime actual topology");records4();
 for(int side=0;side<2;++side){auto& s=g_pose6689;s.open[side].ready=s.closed[side].ready=true;
  for(int i=0;i<s.fingerCount[side];++i){const int id=s.fingerIds[side][i];s.open[side].local[i]=s.closed[side].local[i]=s.restLocal[id];}}
 f7NowH29R4+=40;++t.sequence;t.tick=f7NowH29R4;t.right_grip=.8f;t.right_trigger=.9f;
 Check(run4(t),"qualified actual free-finger writer succeeds");events=records4();r=events.at(0);
 Check(r.arms[0].context.historyCurrent&&r.arms[0].context.priorSample==t.sequence-1&&r.arms[0].context.priorTick<t.tick,
  "prior committed elbow lineage kept separate from current source");
 Check(r.arms[0].context.finger==vd4::FingerOwner::FreeRecipe&&r.arms[0].open&&r.arms[0].closed&&r.arms[0].finger[0].attempted,
  "actual free recipe and scoped finger write are joined");
 Check(r.arms[0].context.grip==double(t.right_grip)&&r.arms[0].context.trigger==double(t.right_trigger),"raw squeeze copied without qualification change");
 f7NowH29R4+=40;++t.sequence;t.tick=f7NowH29R4;mockHeldKey164R1[0]=901;
 Check(run4(t),"actual held-object recipe remains available");events=records4();r=events.at(0);
 Check(r.arms[0].context.finger==vd4::FingerOwner::NativeOrObject&&r.arms[0].context.heldKey==901&&!r.arms[0].context.fingerCall,
  "native held owner excludes free recipe and carries actual key");
 f7NowH29R4+=300;Init();arm4();t=source4();failAt=rig.hand_r;
 Check(!run4(t),"actual native setter failure remains rejected");events=records4();r=events.at(0);
 Check(!r.committed&&r.restored&&r.restoreOk&&!r.packetPublished&&!r.published&&r.line>0&&r.poseData,
  "failed write carries precise stage/line and restoration, no publication claim");
 f7NowH29R4+=300;Init();arm4();t=source4();
 Check(run4(t,true,1),"owner replacement occurs after real scoped write");events=records4();r=events.at(0);
 Check(r.committed&&r.restored&&r.restoreOk&&r.publishAttempted&&!r.packetPublished&&!r.published&&std::strcmp(r.end,"PUBLICATION_REFUSED")==0,
  "late owner failure distinguished from successful write and final publication");
 f7NowH29R4+=300;Init();arm4();t=source4();t.head_held=t.right_held=true;
 Check(run4(t),"existing tiny held display policy remains intact");events=records4();r=events.at(0);
 Check((r.tracking&24u)==24u,"source held flags remain explicit original snapshot bits");
 Check(r.goodHead==t.head_good_tick&&r.goodRight==t.right_good_tick,"held display never refreshes good clocks");
 f7NowH29R4+=300;Init();arm4();t=source4();t.hq={0,0,0,0};
 Check(!run4(t),"actual invalid tracked quaternion rejects");events=records4();r=events.at(0);
 Check(std::strcmp(r.stage,"TRACKING_OR_SETTER")==0&&!r.published&&r.line>0,"precise tracking failure stage precedes writer");
 // Current mismatched native snapshot must never borrow the prior actor joints.
 f7NowH29R4+=300;Init();arm4();t=source4();g_pose_restore_fault6689=true;
 Check(!run4(t),"restore fault prevents actual transaction");events=records4();r=events.at(0);
 Check(!r.poseData&&!r.committed&&!r.published,"pre-anatomy failure emits no stale prior-actor geometry");
 g_pose_restore_fault6689=false;
 f7NowH29R4+=300;Init();arm4();t=source4();{
  CoherentTransaction6689 tx;Check(BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,3,t),"real pose begins in original F7 session");
  arm4(false);arm4(true);tx.Restore();PublishCommitted6689(tx);
 }
 Check(records4().empty()&&g_committed_pose_valid6689,"F7 OFF/ON during actual transaction cannot relabel old source into new capture session");
 arm4(false);f7NowH29R4+=300;Init();t=source4();{
  CoherentTransaction6689 tx;arm4(true);Check(BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,3,t),"pose begun before F7 still behaves normally");tx.Restore();PublishCommitted6689(tx);
 }
 Check(records4().empty(),"arming F7 mid-transaction never retroactively borrows prior capture start");
 // Queue format and finite channel controls, including no visibility fiction.
 f7NowH29R4+=300;Init();arm4();t=source4();run4(t);std::vector<std::string> lines;
 vd4::drain(f7NowH29R4,[&](const char* p){lines.emplace_back(p);});bool source=false,cycle=false,elbow=false,owner=false,health=false,unknown=false,fit=true;
 for(const auto& s:lines){source|=s.find("VRIK_F7 SOURCE ")==0;cycle|=s.find("VRIK_F7 CYCLE ")==0;elbow|=s.find("VRIK_F7 ELBOW ")==0;
  owner|=s.find("VRIK_F7 HAND_OWNER ")==0;health|=s.find("VRIK_F7 HEALTH ")==0;unknown|=s.find("renderedVisibility=UNPROVEN")!=s.npos;fit&=s.size()<1499&&cap4::accept(s.c_str(),f7NowH29R4);}
 Check(source&&cycle&&elbow&&owner&&health&&unknown&&fit,"bounded complete-cycle F7 lines fit queue and retain honest visibility unknown");
 arm4(false);const auto before=vd4::copied.load();run4(t);Check(before==vd4::copied.load(),"OFF after ON revokes producer without pose change");
 arm4();run4(t);cap4::gate.select(cap4::Domain::Vrik,false,f7NowH29R4);lines.clear();vd4::drain(f7NowH29R4,[&](const char* p){lines.emplace_back(p);});Check(lines.empty(),"OFF discards queued old-session details");
 Check(!cap4::active(cap4::Domain::Combat,f7NowH29R4)&&!cap4::active(cap4::Domain::Interaction,f7NowH29R4),"F7 does not arm combat or interaction channels");
 arm4();Check(!cap4::active(cap4::Domain::Vrik,f7NowH29R4+cap4::Duration),"existing180second recorder expiry remains exact");
 std::printf("VRIK_CYCLE_H29R4 %d/%d host checks; retail/render/headset=NOT_EXECUTED\n",tests-failures,tests);return failures?1:0;
}
