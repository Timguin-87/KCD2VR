// Actual coherent pose transaction. No native engine execution.
static unsigned long long f7NowH6=10000;
#define KCD2VR_POSE_FIXTURE_CLOCK f7NowH6
#define main retained_pose_adapter_main_f7_h6
#include "pose_adapter_test.cpp"
#undef main
namespace vh6=kcd2vr::diagnostics::vrik_h29;
namespace ch6=kcd2vr::diagnostics::capture168;
static std::vector<vh6::Record> takeH6(){std::vector<vh6::Record> out;vh6::Record r{};while(vh6::pop(r))out.push_back(r);return out;}
static TrackingSnapshot sourceH6(){auto t=Tracking();t.source_epoch=kcd2vr::tracking59c6::epoch.load();t.tick=f7NowH6;t.input_tick=t.tick-3;t.head_good_tick=t.tick-1;t.right_good_tick=t.tick-2;t.left_good_tick=t.tick-4;t.timeNs=t.tick*1000000ull;return t;}
static bool runH6(TrackingSnapshot t,bool changeOwner=false){CoherentTransaction6689 tx;const bool ok=BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,3,t);tx.traceNativeCalledH29=tx.traceNativeReturnedH29=true;tx.Restore();if(changeOwner)g_pose_epoch6689.fetch_add(1);PublishCommitted6689(tx);return ok;}
static std::string drainH6(){std::string out;vh6::drain(f7NowH6,[&](const char* x){out+=x;out+='\n';});return out;}
static bool containsH6(const std::string& s,const char* x){return s.find(x)!=s.npos;}
int main(){tests=failures=0;Init();ch6::gate.select(ch6::Domain::Vrik,false,f7NowH6);auto t=sourceH6();Check(runH6(t),"OFF actual solve succeeds");auto offPose=g_pose6689.work,offLocal=g_pose6689.local;auto offWrites=writeCalls;Check(takeH6().empty()&&drainH6().empty(),"OFF no receipt or delivery");
 Init();ch6::gate.select(ch6::Domain::Vrik,true,f7NowH6);t=sourceH6();Check(runH6(t),"ON original actual solve succeeds");Check(std::memcmp(offPose.data(),g_pose6689.work.data(),sizeof(offPose))==0&&std::memcmp(offLocal.data(),g_pose6689.local.data(),sizeof(offLocal))==0&&offWrites==writeCalls,"ON byte-identical pose/locals/write count");
 f7NowH6+=5;++t.sequence;t.tick=f7NowH6;t.timeNs+=5000000;Check(runH6(t,true),"actual late owner refusal occurs after scoped solve");Check(!g_committed_pose_valid6689,"actual changed-owner publication refused");
 f7NowH6+=5;++t.sequence;t.tick=f7NowH6;t.timeNs+=5000000;Check(runH6(t),"new current-owner actual solve recovers");Check(g_committed_pose_valid6689,"current original source publishes again");
 auto out=drainH6();Check(containsH6(out,"VRIK_F7 ACCOUNT kind=CYCLE observed=3 throttled=2"),"all3 actual outcomes accounted despite unchanged25ms detail throttle");Check(containsH6(out,"publicationRefused=1"),"throttled real refusal remains counted");Check(containsH6(out,"VRIK_F7 LAST_FAILURE kind=CYCLE")&&containsH6(out,"end=PUBLICATION_REFUSED")&&containsH6(out,"sourceTick=10005"),"exact failed operation and original clock survive throttling");
 Check(std::count(out.begin(),out.end(),'\n')<100,"bounded existing drain cannot unthrottle3 full poses");Check(containsH6(out,"renderedVisibility=UNPROVEN"),"never invent visibility");
 f7NowH6+=300;++t.sequence;t.tick=f7NowH6;t.timeNs+=300000000;Check(runH6(t),"later actual pose still accepted");auto events=takeH6();Check(events.size()==1&&events[0].sourceTick==t.tick&&events[0].inputTick==t.input_tick&&events[0].timeNs==t.timeNs,"new accounting never relabels source/input/ns clocks");
 f7NowH6+=300;Init();t=sourceH6();{CoherentTransaction6689 tx;Check(BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,3,t),"actual ABA source begins");ch6::gate.select(ch6::Domain::Vrik,false,f7NowH6);ch6::gate.select(ch6::Domain::Vrik,true,f7NowH6);tx.Restore();PublishCommitted6689(tx);}Check(takeH6().empty(),"existing recorder lease still excludes old-scope ABA receipt");
 f7NowH6+=300;Init();t=sourceH6();t.poseTime=9876543210123ll;t.right_held=true;mockPhysical=true;g_weapon_control_mode=6;
 auto& weapon=mockProfiles[0];weapon.key=weapon.generation=1;weapon.primary=weapon.geometry=true;weapon.classId=4;weapon.side=0;weapon.tip={0,.8,0};
 Check(runH6(t),"actual held-grace physical pose remains accepted while recording timing");events=takeH6();
 Check(events.size()==1&&events[0].locateTargetXrNs==t.poseTime&&events[0].heldInputTargetXrNs[0]==0&&events[0].heldInputTargetXrNs[1]==t.poseTime&&events[0].heldMotorPath==1,"existing gated F7 record copies actual locate and per-side physical-input target selection");
 ch6::gate.select(ch6::Domain::Vrik,false,f7NowH6);Check(drainH6().empty(),"OFF summary/detail delivery zero");std::printf("vrik_cycle_h6 %d/%d actual_coherent; native_leaves_explicit; pose_bytes_unchanged\n",tests-failures,tests);return failures?1:0;}
