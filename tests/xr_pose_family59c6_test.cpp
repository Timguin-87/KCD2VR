// Executes the actual xr_input.cpp, against Khronos headers retained in the
// supplied Crysis VR source; runtime calls below are mocks, not headset proof.
#include <windows.h>
#include "vrik/physical_weapon.h"
#include "vrik/tracking_grace111.h"
#include "../src/xr_input.cpp"
#include <map>
#include <string>
#include <vector>
namespace ph=kcd2vr::physical;
static unsigned nextHandle=10;template<class T>T handle(){return reinterpret_cast<T>(std::uintptr_t(++nextHandle));}
static std::map<std::string,XrPath> paths;static std::map<XrSpace,XrPath> spaces;
static XrResult syncResult=XR_SUCCESS,applyResult=XR_SUCCESS;static bool failHaptic=false,leftTracked=true,rightTracked=true;
static float translation59c6=0;static bool leftEstimated59c6=false;
static unsigned syncCalls59c6=0; static std::vector<XrTime> locateTimes59c6; static bool rightError59c6=false;
static unsigned calls=0,stops=0,bindCount=0;static XrPath lastHand=0;static XrHapticVibration lastV{};
extern "C" {
XRAPI_ATTR XrResult XRAPI_CALL xrStringToPath(XrInstance,const char*s,XrPath*p){auto&v=paths[s];if(!v)v=paths.size()+1;*p=v;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrCreateActionSet(XrInstance,const XrActionSetCreateInfo*,XrActionSet*p){*p=handle<XrActionSet>();return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrCreateAction(XrActionSet,const XrActionCreateInfo*i,XrAction*p){if(i->actionType==XR_ACTION_TYPE_VIBRATION_OUTPUT&&failHaptic)return XR_ERROR_ACTION_TYPE_MISMATCH;*p=handle<XrAction>();return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrSuggestInteractionProfileBindings(XrInstance,const XrInteractionProfileSuggestedBinding*i){bindCount=i->countSuggestedBindings;for(unsigned j=0;j<bindCount;++j)if(!i->suggestedBindings[j].action)return XR_ERROR_HANDLE_INVALID;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrCreateActionSpace(XrSession,const XrActionSpaceCreateInfo*i,XrSpace*p){*p=handle<XrSpace>();spaces[*p]=i->subactionPath;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrAttachSessionActionSets(XrSession,const XrSessionActionSetsAttachInfo*){return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrSyncActions(XrSession,const XrActionsSyncInfo*){++syncCalls59c6;return syncResult;}
XRAPI_ATTR XrResult XRAPI_CALL xrLocateSpace(XrSpace space,XrSpace,XrTime when,XrSpaceLocation*out){locateTimes59c6.push_back(when);if(rightError59c6&&spaces[space]==paths["/user/hand/right"])return XR_ERROR_RUNTIME_FAILURE;bool tracked=spaces[space]==paths["/user/hand/left"]?leftTracked:rightTracked;out->locationFlags=tracked?XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT|XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT:0;if(leftEstimated59c6&&spaces[space]==paths["/user/hand/left"])out->locationFlags=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;out->pose.position.x=translation59c6+(spaces[space]==paths["/user/hand/left"]?-.2f:.2f);out->pose.orientation.w=1;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrGetActionStateFloat(XrSession,const XrActionStateGetInfo*,XrActionStateFloat*out){out->isActive=XR_TRUE;out->currentState=0;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrGetActionStateVector2f(XrSession,const XrActionStateGetInfo*,XrActionStateVector2f*out){out->isActive=XR_TRUE;out->currentState={};return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrGetActionStateBoolean(XrSession,const XrActionStateGetInfo*,XrActionStateBoolean*out){out->isActive=XR_TRUE;out->currentState=0;out->changedSinceLastSync=0;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrApplyHapticFeedback(XrSession,const XrHapticActionInfo*i,const XrHapticBaseHeader*h){++calls;lastHand=i->subactionPath;lastV=*reinterpret_cast<const XrHapticVibration*>(h);return applyResult;}
XRAPI_ATTR XrResult XRAPI_CALL xrStopHapticFeedback(XrSession,const XrHapticActionInfo*){++stops;return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrDestroySpace(XrSpace){return XR_SUCCESS;}
XRAPI_ATTR XrResult XRAPI_CALL xrDestroyActionSet(XrActionSet){return XR_SUCCESS;}
}
static unsigned tests=0,failures=0;static void check(bool v,const char*n){++tests;failures+=!v;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
static int OriginalInputTests(){using namespace kcd2vr;namespace h=haptics101;
 XrInput input;check(input.Initialize(handle<XrInstance>(),handle<XrSession>(),handle<XrSpace>())&&bindCount==23&&paths.count("/user/hand/right/input/thumbrest/touch")&&paths.count("/user/hand/right/input/trigger/touch")/*R59.16C5 optional trigger touch*/,"actual input creates haptics plus per-hand Touch/thumbrest bindings");
 h::select(true);h::allowed=true;h::Cue cue{};hostTickCount64=1000;h::observe(cue,0,77,.1,true,1,1000);input.Sync(1);check(calls==0,"proximity must settle before readiness pulse");
 hostTickCount64=1070;h::observe(cue,0,77,.1,true,2,1070);input.Sync(2);check(calls==1&&lastHand==paths["/user/hand/right"]&&lastV.duration==70000000&&lastV.amplitude==.55f,"native-ready right mount queues configured 70ms proximity pulse through actual consumer");
 for(int i=0;i<100;++i){hostTickCount64++;h::observe(cue,0,77,.1,true,3+i,hostTickCount64);input.Sync(3+i);}check(calls==1,"standing near mount does not vibrate every frame");
 h::allowed=false;input.Sync(200);const auto stop=stops;check(stops==1,"menu/gameplay loss stops prior haptic");input.Sync(201);check(stops==stop,"stopped feedback is not repeatedly stopped");
 h::allowed=true;hostTickCount64=2000;h::queue[1].publish({h::epoch.load(),1000,2000,78,.12f,35});input.Sync(202);check(calls==2&&lastHand==paths["/user/hand/left"],"left hand uses distinct subaction path");
 leftTracked=false;hostTickCount64=2010;h::queue[1].publish({h::epoch.load(),1001,2010,78,.12f,35});input.Sync(203);const auto before=calls;leftTracked=true;input.Sync(204);check(calls==before,"tracking recovery does not replay discarded haptic");
 hostTickCount64=2300;h::queue[0].publish({h::epoch.load(),1002,2000,77,.12f,35});input.Sync(205);check(calls==before,"expired cue is never played");
 h::queue[0].publish({h::epoch.load(),1003,2300,77,.12f,35});syncResult=XR_SESSION_NOT_FOCUSED;check(!input.Sync(206),"positive NOT_FOCUSED result is not treated as usable action sync");syncResult=XR_SUCCESS;input.Sync(207);check(calls==before,"focus recovery does not replay old cue");
 h::queue[0].publish({h::epoch.load(),1004,2300,77,.12f,35});h::select(false);h::select(true);input.Sync(208);check(calls==before,"mode epoch rejects old readiness target");
 h::queue[0].publish({h::epoch.load(),1005,2300,77,.12f,35});applyResult=XR_SESSION_NOT_FOCUSED;input.Sync(209);const auto failed=calls;input.Sync(210);check(calls==failed,"failed haptic is not retried each frame");input.Shutdown();
 failHaptic=true;applyResult=XR_SUCCESS;XrInput other;check(other.Initialize(handle<XrInstance>(),handle<XrSession>(),handle<XrSpace>())&&bindCount==21/*R59.16C5: +2 trigger touch*/&&other.Sync(300),"unsupported haptic action does not disable tracking or thumbrest input");other.Shutdown();
 std::printf("xr_haptics=%u/%u runtime=MOCKED\n",tests-failures,tests);return failures?1:0;
}



// Complete production XrManager::SampleLateHeadPose, without Direct3D/native leaves.
struct LARGE_INTEGER{long long QuadPart=0;};
static long long fakeNow59c6=123456;
inline int QueryPerformanceCounter(LARGE_INTEGER* q){q->QuadPart=fakeNow59c6;return 1;}
namespace kcd2vr {
class XrManager { public:
 std::atomic<bool> late_pose_enabled_{true},session_running_{true};bool qpc_time_extension_enabled_=true;
 XrInstance instance_{};XrSpace view_space_{},local_space_{};XrInput input_;std::mutex pose_locate_mutex_;
 std::atomic<std::uint64_t> late_pose_samples_{0},late_pose_failures_{0};
 XrResult (*qpc_to_xr_time_)(XrInstance,const LARGE_INTEGER*,XrTime*)=[](XrInstance,const LARGE_INTEGER*q,XrTime*t){*t=q->QuadPart;return XR_SUCCESS;};
 bool SampleLateHeadPose(XrPose& out_pose);
 bool SampleHeadPoseAtTime(XrTime time,XrPose& out_pose);
 void PublishPredictedForTest(const tracking59c6::Stamp&,const XrPose&,XrSpaceLocationFlags,bool);
};
#include "generated_late_pose59c6.inc"
}
namespace kcd2vr::reading_pose32 {inline bool poseAllowed(bool allowed,bool workstation,std::uint64_t){return allowed||workstation;}}
namespace pipeline59c6 {
namespace tracking59c6=kcd2vr::tracking59c6;namespace tracking_grace111=kcd2vr::tracking_grace111;
using kcd2vr::XrPose;
struct TrackingPose{float px=0,py=0,pz=0,qx=0,qy=0,qz=0,qw=1;};
static std::atomic<bool>g_gameplay_allowed{true},g_dialogue_body178r9{false},g_head_valid{true},g_left_valid{true},g_right_valid{true};
static std::atomic<std::uint64_t>g_head_valid_tick{1000},g_left_valid_tick{1000},g_right_valid_tick{1000};
static std::atomic<float>g_left_grip{1},g_right_grip{1},g_left_trigger{.25f},g_right_trigger{.75f};
template<class S>static void LifecycleTracking36(const S&,bool){}
#define KCD2VR_HF1_PIPELINE_TEST
#include "generated_late_pose59c6.inc"
#undef KCD2VR_HF1_PIPELINE_TEST
static TrackingPose Convert(const XrPose& p){return {p.pos_x,p.pos_y,p.pos_z,p.qx,p.qy,p.qz,p.qw};}
static void Run(kcd2vr::XrManager& m){
 namespace t=tracking59c6;
 const XrSpaceLocationFlags flags=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT|XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
 const auto ns=t::NowNs()-50000000;const XrTime target=900000000000ll;
 t::Configure(true);t::SetCameraSource(0);t::Reset();CameraMini camera;
 auto publish=[&](unsigned n,bool lv=true){const auto& l=m.input_.Left();const auto& r=m.input_.Right();XrPose h{};h.pos_x=translation59c6;PublishActualPacket(Convert(h),true,Convert(l.pose),lv,Convert(r.pose),r.active,1000+10*n,ns+10000000*n,target+10000000*n);};
 auto early=[&](unsigned n){XrPose h{};h.pos_x=translation59c6;m.PublishPredictedForTest({1000+10*n,ns+10000000*n,t::epoch.load(),target+10000000*n},h,flags,true);camera.Publish(h,1,n+1);};
 leftTracked=rightTracked=true;leftEstimated59c6=false;rightError59c6=false;syncResult=XR_SUCCESS;hostTickCount64=1000;translation59c6=0;m.input_.Sync(target);publish(0);early(0);
 t::Frame f{};TrackingSnapshot packet{};check(t::Read(f)&&f.locateTime==target,"mode0 early publication contains existing predicted Sync family");
 hostTickCount64=1010;translation59c6=.1f;m.input_.Sync(target+10000000);const auto queries=locateTimes59c6.size();const auto syncs=syncCalls59c6;early(1);ReadTrackingSnapshot(packet);
 check(locateTimes59c6.size()==queries&&syncCalls59c6==syncs,"early publication performs no additional XR acquisition/action sync");
 check(camera.Consume().pos_x==packet.head.x&&std::abs(packet.left.x+.1f)<1e-6&&packet.poseTime==target+10000000,"actual reader consumes current head and hands before late packet: no100mm family gap");
 check(packet.input_tick==1000&&packet.left_grip==1&&packet.left_trigger==.25f&&packet.allowed,"early geometry retains original input clock/buttons/native permission");
 auto pi=Physical(packet);ph::State state{};std::array<ph::Profile,2> profiles{};auto& profile=profiles[1];profile.side=1;profile.key=1;profile.entity=2;profile.generation=1;profile.classId=4;profile.primary=profile.geometry=true;profile.tip={0,.8,0};profile.axis={0,1,0};profile.extent=.8;profile.weight=1;
 auto clear=[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.;};(void)ph::step(state,pi,profiles,clear);
 check(pi.intentTracked[1]&&pi.poseTime[1]==target+10000000&&state.weapon[1].lastSample==packet.sequence,"ordinary continuous same-family motion reaches actual physical kernel without dropped sample");
 const auto seq=packet.sequence;publish(1);ReadTrackingSnapshot(packet);auto same=Physical(packet);(void)ph::step(state,same,profiles,clear);check(packet.sequence==seq&&state.weapon[1].lastSample==seq,"late original packet cannot replay an already consumed early physical sample");
 hostTickCount64=1020;translation59c6=.2f;leftTracked=false;m.input_.Sync(target+20000000);early(2);ReadTrackingSnapshot(packet);pi=Physical(packet);
 check(packet.left_valid&&packet.left_held&&packet.left_good_tick==1010&&!pi.intentTracked[1]&&!pi.poseTime[1],"single lost hand retains original display age and cannot borrow new head motion clock");
 hostTickCount64=1030;leftTracked=true;m.input_.Sync(target+30000000);early(3);ReadTrackingSnapshot(packet);pi=Physical(packet);check(packet.left_held&&!pi.intentTracked[1],"first recovered hand stays nonoffensive under existing C6 policy");
 hostTickCount64=1031;m.input_.Sync(target+31000000);t::Stamp continuous{1031,ns+31000000,t::epoch.load(),target+31000000};XrPose continuousHead{};continuousHead.pos_x=translation59c6;m.PublishPredictedForTest(continuous,continuousHead,flags,true);ReadTrackingSnapshot(packet);pi=Physical(packet);check(!packet.left_held&&pi.intentTracked[1],"next genuinely tracked sample returns normal motion provenance without an extra delay");
 // Actual GetPose keeps active valid-only, while its separately copied flags
 // tell the existing C6 policy that an estimated hand is not tracked-good.
 leftEstimated59c6=true;hostTickCount64=1032;m.input_.Sync(target+32000000);m.PublishPredictedForTest({1032,ns+32000000,t::epoch.load(),target+32000000},continuousHead,flags,true);ReadTrackingSnapshot(packet);pi=Physical(packet);check(m.input_.Left().active&&packet.left_held&&packet.left_good_tick==1031&&!pi.intentTracked[1],"actual VALID-only input remains active but does not fabricate tracked-good motion");leftEstimated59c6=false;
 // Later schedules use fresh real source times rather than future host timestamps.
 t::Reset();hostTickCount64=1100;translation59c6=0;m.input_.Sync(target+40000000);publish(10);
 auto frame=[&](std::uint64_t tick,std::uint64_t dt,bool lh,bool rh){t::Frame in{};in.epoch=t::epoch.load();in.tick=tick;in.ns=t::NowNs()-10000000+dt;in.locateTime=target+40000000+std::int64_t(dt);in.controllersSampled=true;in.valid.fill(true);in.tracked.fill(true);in.valid[1]=in.tracked[1]=lh;in.valid[2]=in.tracked[2]=rh;return in;};
 check(t::Publish(frame(1100,0,true,true))&&t::Publish(frame(1101,1,true,true)),"current source can establish continuous family after actual epoch transition");
 t::Publish(frame(1181,2,false,false));hostTickCount64=1181;ReadTrackingSnapshot(packet);pi=Physical(packet);check(packet.left_valid&&packet.right_valid&&packet.left_held&&packet.right_held&&!pi.intentTracked[0]&&!pi.intentTracked[1],"both lost hands at80ms retain display only with no offensive intent");
 t::Publish(frame(1182,3,false,false));hostTickCount64=1182;ReadTrackingSnapshot(packet);check(!packet.left_valid&&!packet.right_valid,"both lost hands expire at81ms without renewing original good ticks");
 hostTickCount64=1201;ReadTrackingSnapshot(packet);check(!packet.head_valid&&!packet.left_valid,"new pose family cannot renew original100ms input permission age");
 g_gameplay_allowed=false;hostTickCount64=1182;ReadTrackingSnapshot(packet);check(!packet.allowed&&!Physical(packet).allowed,"current menu/gameplay ownership cannot be renewed by early pose publication");g_gameplay_allowed=true;
 t::SetCameraSource(2);check(!t::Read(f),"mode change rejects predicted family before NOW owner can publish");
 t::SetCameraSource(3);check(!t::enabled.load(),"offset comparison mode3 retains original late packet selection");
 t::SetCameraSource(0);t::Configure(false);check(!t::enabled.load(),"existing VrSynchronousHands=false comparison remains original packet path");t::Configure(true);
}
}
int main(){using namespace kcd2vr;
 check(OriginalInputTests()==0,"original input/haptic tests preserved");
 failHaptic=false;rightTracked=leftTracked=true;syncResult=XR_SUCCESS;applyResult=XR_SUCCESS;
 XrManager m; m.instance_=handle<XrInstance>();m.local_space_=handle<XrSpace>();m.view_space_=handle<XrSpace>();
 spaces[m.view_space_]=paths["/user/hand/left"];
 check(m.input_.Initialize(m.instance_,handle<XrSession>(),m.local_space_)&&m.input_.Sync(100),"actual input action spaces initialized/synced");
 tracking59c6::Configure(true);tracking59c6::SetCameraSource(2);tracking59c6::Reset();XrPose out{};
 m.late_pose_enabled_=false;auto activeCount=syncCalls59c6;locateTimes59c6.clear();
 check(m.SampleHeadPoseAtTime(fakeNow59c6,out)&&locateTimes59c6.size()==3&&syncCalls59c6==activeCount,
       "ACTIVE synchronous camera queries controllers even when legacy latePose is OFF");
 check(locateTimes59c6[0]==locateTimes59c6[1]&&locateTimes59c6[1]==locateTimes59c6[2],"active camera head/left/right share target XR time");
 tracking59c6::Configure(false);locateTimes59c6.clear();
 check(m.SampleHeadPoseAtTime(fakeNow59c6,out)&&locateTimes59c6.size()==1,"INI comparison disables extra grip locates without changing head route");
 tracking59c6::Configure(true);tracking59c6::SetCameraSource(0);locateTimes59c6.clear();
 check(m.SampleHeadPoseAtTime(fakeNow59c6,out)&&locateTimes59c6.size()==1,"mode0 head-only helper does not acquire a foreign current-time grip family");
 tracking59c6::SetCameraSource(1);locateTimes59c6.clear();check(m.SampleHeadPoseAtTime(fakeNow59c6,out)&&locateTimes59c6.size()==3&&locateTimes59c6[0]==locateTimes59c6[2],"mode1 actual resample publishes head/grips at its exact chosen predicted target");
 tracking59c6::SetCameraSource(2);m.late_pose_enabled_=true;
 auto syncs=syncCalls59c6,hs=calls;locateTimes59c6.clear();
 check(m.SampleLateHeadPose(out),"actual manager late family locates successfully");
 check(locateTimes59c6.size()==3&&locateTimes59c6[0]==fakeNow59c6&&locateTimes59c6[1]==fakeNow59c6&&locateTimes59c6[2]==fakeNow59c6,"head left right queried at exactly the same converted XR time");
 check(syncCalls59c6==syncs&&calls==hs,"late camera sampling does not resync actions or replay haptics");
 tracking59c6::Frame f{};check(tracking59c6::Read(f)&&f.controllersSampled&&f.valid[0]&&f.valid[1]&&f.valid[2],"actual sampler publishes a complete family");
 rightError59c6=true;fakeNow59c6+=1000;m.SampleLateHeadPose(out);tracking59c6::Read(f);
 check(!f.tracked[2]&&f.held[2]&&f.tracked[1],"right locate error clears right flags and cannot inherit left success");
 rightError59c6=false;syncResult=XR_SESSION_NOT_FOCUSED;m.input_.Sync(200);m.SampleLateHeadPose(out);tracking59c6::Read(f);
 check(!f.tracked[1]&&!f.tracked[2]&&!m.input_.Left().active&&!m.input_.Right().aim_active,"focus loss invalidates both active controller streams");
 XrPosef pair[2]{};XrSpaceLocationFlags flags[2]{123,123};m.input_.Shutdown();
 check(!m.input_.TryLocateGripPair(100,pair,flags)&&!flags[0]&&!flags[1],"destroyed action spaces are never located");
 m.late_pose_enabled_=false;auto n=locateTimes59c6.size();check(!m.SampleLateHeadPose(out)&&locateTimes59c6.size()==n,"disabled late comparison performs no pose calls");
 m.late_pose_enabled_=true;rightTracked=leftTracked=true;syncResult=XR_SUCCESS;rightError59c6=false;
 check(m.input_.Initialize(m.instance_,handle<XrSession>(),m.local_space_),"reinitialize actual action spaces for early-family composition");
 pipeline59c6::Run(m);
 tracking59c6::SetCameraSource(0);tracking59c6::Frame retained{};const auto retainedNs=tracking59c6::NowNs()-1000000;retained.epoch=tracking59c6::epoch.load();retained.tick=hostTickCount64;retained.ns=retainedNs;retained.locateTime=777;retained.controllersSampled=true;retained.valid.fill(true);retained.tracked.fill(true);tracking59c6::Publish(retained);m.SampleLateHeadPose(out);tracking59c6::Read(retained);check(retained.locateTime==777,"actual late frustum head refresh cannot overwrite mode0 predicted gameplay family");
 std::printf("xr_pose_family59c6=%u/%u runtime=MOCKED\n",tests-failures,tests);return failures?1:0;
}
namespace kcd2vr::diagnostic_trace { void Write(const char*) noexcept {}
void WriteReceiptFor(diagnostics::capture168::Domain,std::uint64_t,const char*) noexcept {}
// D62 fixture repair: current input diagnostics use the existing finite capture predicate.
bool EnabledFor(diagnostics::capture168::Domain d) noexcept {return diagnostics::capture168::active(d,GetTickCount64());}
}
