// Actual production dispatch/locates with controlled runtime and ownership doubles.
// No Windows DLL, driver, compositor, eye image, or headset is exercised here.
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <future>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "vrik/motion_trials_d67.h"
#include "tracking_sample59c6.h"
#include "vrik/fresh_hand_target_r94.h"

using XrTime=std::int64_t;
using XrSpaceLocationFlags=std::uint64_t;
using XrResult=int;
constexpr int XR_NULL_HANDLE=0,XR_TYPE_SPACE_LOCATION=1;
constexpr XrSpaceLocationFlags XR_SPACE_LOCATION_POSITION_VALID_BIT=1,
    XR_SPACE_LOCATION_ORIENTATION_VALID_BIT=2,XR_SPACE_LOCATION_POSITION_TRACKED_BIT=4,
    XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT=8;
constexpr bool XR_FAILED(int r){return r<0;}
constexpr bool XR_SUCCEEDED(int r){return r>=0;}
struct XrVector3f {float x=0,y=0,z=0;};
struct XrQuaternionf {float x=0,y=0,z=0,w=1;};
struct XrPosef {XrQuaternionf orientation{};XrVector3f position{};};
struct XrSpaceVelocity {};
struct XrSpaceLocation {int type;XrPosef pose{};XrSpaceLocationFlags locationFlags=0;};
struct LARGE_INTEGER {std::int64_t QuadPart=0;};
namespace fixture {
inline std::uint64_t tick=1000,nativeFrame=1;
inline XrTime now=10000000000ll;
inline bool qpcGood=true,convertGood=true,headGood=true,headValid=true;
inline unsigned headLocates=0,gripLocates=0,extraCalls=0,waits=0,observations=0;
    inline unsigned diagnosticWrites=0,diagnosticClocks=0;
inline std::vector<std::string> messages;
inline std::function<void()> duringHead,duringGrips,duringQpc;
inline XrTime lastHeadTarget=0,lastGripTarget=0;
inline int failures=0,checks=0;
void check(bool yes,const char* label){++checks;if(!yes){++failures;std::printf("FAIL %s\n",label);}}
void advance(){++tick;now+=1000000;++nativeFrame;}
}
std::uint64_t GetTickCount64(){return fixture::tick;}
unsigned GetCurrentThreadId(){return 7;}
bool QueryPerformanceCounter(LARGE_INTEGER* out){out->QuadPart=fixture::now;if(fixture::duringQpc)fixture::duringQpc();return fixture::qpcGood;}
XrResult ConvertQpc(int,const LARGE_INTEGER* qpc,XrTime* out){*out=qpc->QuadPart;return fixture::convertGood?0:-1;}
XrResult xrLocateSpace(int,int,XrTime time,XrSpaceLocation* out){
    ++fixture::headLocates;fixture::lastHeadTarget=time;
    out->pose.position={.1f,1.7f,.2f};out->locationFlags=fixture::headValid?15u:0u;
    if(fixture::duringHead)fixture::duringHead();
    return fixture::headGood?0:-1;
}

namespace kcd2vr {
struct XrPose {float pos_x=0,pos_y=0,pos_z=0,qx=0,qy=0,qz=0,qw=1;};
struct CameraHook {
    inline static int source=2,injection=1;
    static int GetPoseSourceMode(){return source;}
    static int GetInjectionMode(){return injection;}
};
struct XrInput {
    std::mutex mutex;
    bool available=true;
    unsigned flags[2]{15,15};
    bool TryLocateGripPair(XrTime target,XrPosef (&poses)[2],XrSpaceLocationFlags (&out)[2],XrSpaceVelocity* =nullptr){
        ++fixture::gripLocates;fixture::lastGripTarget=target;
        std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);
        if(!lock.owns_lock()||!available)return false;
        for(unsigned i=0;i<2;++i){poses[i].position={float(i+1),1.4f,-.4f};out[i]=flags[i];}
        if(fixture::duringGrips)fixture::duringGrips();
        return true;
    }
};
namespace diagnostics::xr165 {enum {SyncPose};struct Timer {explicit Timer(int){}};}
namespace hand_smooth_h50 {inline std::atomic<std::uint64_t> policyEpoch{1};}
namespace claude_motion_d69 {inline std::int64_t extra=0;inline auto ExtraNs(){return extra;}}
static void ApplyExtraHandsD69(XrInput&,tracking59c6::Frame& frame,const XrSpaceVelocity (&)[2],std::int64_t extra){
    ++fixture::extraCalls;
    // Controlled stand-in for the existing unchanged prediction helper. This
    // fixture proves the new route uses that helper/policy, not its maths.
    frame.handPolicyD69=std::uint64_t(extra);
    for(unsigned h=0;h<2;++h){frame.handTargetD69[h]=frame.locateTime+extra;frame.handLeadSourceD69[h]=1;}
}
struct XrManager {
    #include "fresh_uncapped_receipt.inc"
    std::atomic<bool> session_running_{true};
    bool qpc_time_extension_enabled_=true;
    XrResult (*qpc_to_xr_time_)(int,const LARGE_INTEGER*,XrTime*)=ConvertQpc;
    int instance_=1,view_space_=1,local_space_=2,xrGate=0;
    std::mutex pose_locate_mutex_;
    std::atomic<std::int64_t> latest_predicted_display_time_{10010000000ll},
        hj1_predicted_now_{10000000000ll},hj1_period_ns_{11111111};
    XrInput input_;
    static XrManager& Instance(){static XrManager m;return m;}
    FreshPoseUncappedReceipt RefreshUncappedPoseFamily() noexcept;
    bool SampleHeadPoseAtTime(XrTime,XrPose&,hj1::Sample* =nullptr,FreshPoseUncappedReceipt* =nullptr);
    bool SampleHeadPoseNow(XrPose&,XrTime* =nullptr,hj1::Sample* =nullptr);
    XrTime GetLatestPredictedDisplayTime() const{return latest_predicted_display_time_.load();}
    int GetXrRenderGateMode(){return xrGate;}
    std::int64_t Hj1PeriodNs(){return hj1_period_ns_.load();}
    bool WaitForNextRenderFrameToken(std::uint64_t& token,unsigned,bool=true,bool=false){++fixture::waits;++token;return true;}
};
#include "fresh_uncapped_manager.inc"

namespace smooth_lab {
struct PacingFrameGate {std::uint64_t attempted=0;};
inline thread_local PacingFrameGate pacingFrameGateD45{};
inline thread_local std::uint64_t pacingTokenD64=0;
inline std::atomic<std::uint64_t> pacingOutsideScope{0},pacingRepeatedFrame{0};
#include "fresh_uncapped_claim.inc"
inline bool earlySelected=false,lateSelected=false;
inline int pacing=0;
inline std::atomic_flag pacingLock=ATOMIC_FLAG_INIT;
std::uint64_t SteadyNs(){++fixture::diagnosticClocks;return tracking59c6::NowNs();}
bool EarlyPacingSelectedD45(){return earlySelected;}
bool PacingSelected(){return lateSelected;}
bool PacingWantedAtD45(std::uint64_t,bool early){return early?earlySelected:lateSelected;}
void ObserveAcceptedTimingD72(int&,bool,std::uint64_t,std::uint64_t,std::uint64_t){++fixture::observations;}
void PacingObserved(bool,std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t){++fixture::observations;}
}
namespace native_stereo_outer_lab {std::uint64_t CurrentNativeFrameHJ1(){return fixture::nativeFrame;}}

namespace diagnostic_trace {void Write(const char* s){++fixture::diagnosticWrites;fixture::messages.emplace_back(s);}}
struct FrameSync {static void PaceGameplayBeforeUpdateD45() noexcept;};
#include "vrik/early_frame_pacing_d45.inl"
#include "fresh_uncapped_predicates.inc"
}

template<class F> double whileOtherThreadHolds(std::mutex& mutex,F&& call){
    std::promise<void> ready;
    auto acquired=ready.get_future();
    std::thread holder([&]{std::unique_lock<std::mutex> lock(mutex);ready.set_value();std::this_thread::sleep_for(std::chrono::milliseconds(120));});
    acquired.wait();auto start=std::chrono::steady_clock::now();call();
    auto elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
    holder.join();return elapsed;
}

int main(){
    using fixture::check;
    { // Actual runtime horizon from the failed R94 run, independent of age.
        using namespace kcd2vr::fresh_hands_r94;
        std::int64_t target=0;
        Anchor a{1050000000,1000000000,11111111,7};
        check(Target(a,1001000000,7,target)&&target==1051000000,
            "50ms runtime forecast at90Hz accepts a fresh hand locate");
        check(!Target(a,1050000000,7,target),"forecast does not renew an expired anchor");
        check(!Target(a,1001000000,8,target),"world/tracking epoch still invalidates old anchor");
        a.epoch=8;a.observed=1060000000;a.target=1110000000;
        check(Target(a,1061000000,8,target),"new current runtime anchor recovers after epoch change");
    }

    using namespace kcd2vr;
    using fixture::check;
    namespace tr=tracking59c6;namespace cap=diagnostics::capture168;namespace mt=motion_trials_d67;
    auto& manager=XrManager::Instance();
    using Receipt=XrManager::FreshPoseUncappedReceipt;
    mt::SetMenuMode(4);tr::Configure(true);tr::SetCameraSource(2);
    XrPose initialCamera{};manager.SampleHeadPoseNow(initialCamera);
    tr::Frame cameraBefore{};check(tr::ReadHead(cameraBefore),"camera establishes its own independent head snapshot");
    auto fresh=manager.RefreshUncappedPoseFamily();
    check(fresh.published&&fresh.result==Receipt::Published&&fresh.sequence!=0,"mode4 publishes an actual pose family");
    check(fixture::lastHeadTarget==fixture::now&&fixture::lastGripTarget==fixture::now&&fresh.target==fixture::now,"NOW head and grips share the exact selected target");
    check(fresh.validMask==7&&fresh.trackedMask==7,"successful receipt reports actual tracked members");
    check(fixture::waits==0&&fixture::diagnosticWrites==0&&diagnostics::ownership48::order.load()==0,"pose refresh has no frame wait or idle diagnostics");
    fixture::advance();fresh=manager.RefreshUncappedPoseFamily();
    const auto good=fresh;

    auto held=good;const auto headCalls=fixture::headLocates;
    double elapsed=whileOtherThreadHolds(manager.pose_locate_mutex_,[&]{held=manager.RefreshUncappedPoseFamily();});
    check(elapsed<60&&held.result==Receipt::PoseBusy&&!held.published,"pose-lock contention returns without waiting");
    check(fixture::headLocates==headCalls&&held.sequence==good.sequence&&held.sourceNs==good.sourceNs&&held.sourceTick==good.sourceTick&&held.target==good.target,"pose contention preserves complete identity and original age");
    tr::Frame oldHead{};check(tr::ReadHead(oldHead),"complete head snapshot exists before grip contention");
    fixture::advance();elapsed=whileOtherThreadHolds(manager.input_.mutex,[&]{held=manager.RefreshUncappedPoseFamily();});
    tr::Frame afterHead{};tr::ReadHead(afterHead);
    check(elapsed<60&&held.result==Receipt::GripBusy&&!held.published,"grip lock contention is not a blocking action/input read");
    check(held.sequence==good.sequence&&held.sourceNs==good.sourceNs&&afterHead.ns==oldHead.ns,"grip contention publishes neither a fake family nor a newer head half");

    manager.input_.flags[0]=0;fixture::advance();auto partial=manager.RefreshUncappedPoseFamily();
    tr::Frame state{};tr::Read(state);
    check(partial.published&&partial.result==Receipt::Published&&partial.trackedMask==5,"attempted invalid left hand publishes invalidity instead of reporting busy");
    check(state.held[1]&&state.goodTick[1]==good.sourceTick&&state.goodTick[2]==partial.sourceTick,"invalid left tracking does not renew its last-good age");
    manager.input_.flags[0]=15;fixture::advance();fresh=manager.RefreshUncappedPoseFamily();

    fixture::headGood=false;fixture::advance();auto invalidHead=manager.RefreshUncappedPoseFamily();
    tr::Read(state);tr::ReadHead(afterHead);
    check(invalidHead.published&&!(invalidHead.trackedMask&1)&&state.held[0],"failed head locate publishes held/invalid family state");
    check(afterHead.ns==cameraBefore.ns,"fresh hands never renew camera-head age even on a failed reference locate");
    fixture::headGood=true;

    const auto beforeReset=tr::published.load();fixture::advance();
    fixture::duringHead=[] {CameraHook::source=1;tr::SetCameraSource(1);};
    auto changed=manager.RefreshUncappedPoseFamily();fixture::duringHead={};
    check(!changed.published&&changed.result==Receipt::TrackingDisabled&&tr::published.load()==beforeReset,"source change during locate rejects the old-target family");
    CameraHook::source=2;tr::SetCameraSource(2);fixture::advance();fresh=manager.RefreshUncappedPoseFamily();
    fixture::duringHead=[] {CameraHook::source=3;tr::SetCameraSource(3);};
    changed=manager.RefreshUncappedPoseFamily();fixture::duringHead={};
    check(!changed.published&&changed.result==Receipt::TrackingDisabled,"coherent tracking disabled during locate cannot publish a new head half or claim session failure");
    CameraHook::source=2;tr::SetCameraSource(2);fixture::advance();fresh=manager.RefreshUncappedPoseFamily();
    fixture::duringGrips=[] {tr::Reset();};
    changed=manager.RefreshUncappedPoseFamily();fixture::duringGrips={};
    check(!changed.published&&changed.result==Receipt::TrackingDisabled,"epoch reset during grip acquisition cannot stamp the old family with the new epoch");
    fixture::advance();fresh=manager.RefreshUncappedPoseFamily();

    const auto beforeMode0=fixture::headLocates;
    CameraHook::source=0;tr::SetCameraSource(0);
    const auto anchorTime=fixture::now;
    fresh_hands_r94::anchors.publish({anchorTime+10000000,anchorTime,11111111,tr::epoch.load()});
    auto cached=manager.RefreshUncappedPoseFamily();
    check(cached.published&&cached.result==Receipt::Published&&fixture::headLocates==beforeMode0+1,"cached camera now acquires independent current hands");
    fixture::advance();auto cachedNext=manager.RefreshUncappedPoseFamily();
    check(cachedNext.published&&cachedNext.target-cached.target==1000000,"hand target advances continuously between XR publications");
    tr::Frame lane{};check(tr::Read(lane)&&lane.refreshGenerationR94!=0,"uncapped selects the separate hands lane");
    unsigned workerReadsR94=0;
    const auto missingHandR94=[](tr::Frame&){return false;};
    const auto workerR94=[&](tr::Frame& out){++workerReadsR94;out=lane;out.refreshGenerationR94=0;return true;};
    tr::Frame selectedR94{};
    check(!tr::ReadFamilyR94(selectedR94,lane.epoch,lane.refreshGenerationR94,true,missingHandR94,workerR94)&&workerReadsR94==0,
        "contended independent read never falls back to a differently timed worker after activation");
    check(tr::ReadFamilyR94(selectedR94,lane.epoch,lane.refreshGenerationR94+1,true,missingHandR94,workerR94)&&workerReadsR94==1,
        "new unactivated mode generation can use its current worker family");
    check(tr::ReadFamilyR94(selectedR94,lane.epoch,lane.refreshGenerationR94,false,missingHandR94,workerR94)&&workerReadsR94==2,
        "ordinary paced choice still selects worker regardless of earlier independent ownership");
    // Use an actual acquisition clock, never a fabricated future timestamp.
    // A future worker ns makes the following new-visit test depend on CPU speed.
    auto worker=lane;worker.refreshGenerationR94=0;worker.ns=tr::NowNs();worker.locateTime-=1000000;worker.poses[2].x=99;
    check(tr::Publish(worker),"original worker remains free to publish its own completed family");
    tr::Read(state);check(state.sequence==lane.sequence&&state.poses[2].x!=99,"later worker publication cannot overwrite uncapped hands with another target");
    const auto handGeneration=mt::handRefreshGenerationR94.load();
    mt::SetMode(2);mt::SetElbowPreferenceR92(!mt::ElbowPreferenceR92());mt::SetContinuityRepair(!mt::ContinuityRepair());
    tr::Read(state);
    check(mt::handRefreshGenerationR94.load()==handGeneration&&state.sequence==lane.sequence&&state.tick==lane.tick,
        "diagnostic and elbow policy changes cannot switch fresh hands back to an older worker pose");
    mt::SetMode(0);mt::SetElbowPreferenceR92(false);mt::SetContinuityRepair(true);
    fixture::advance();
    fresh_hands_r94::anchors.publish({cachedNext.target-1000000,fixture::now,11111111,tr::epoch.load()});
    const auto reversed=manager.RefreshUncappedPoseFamily();
    check(!reversed.published&&reversed.result==Receipt::PublicationRejected&&reversed.sequence==cachedNext.sequence&&
        reversed.sourceNs==cachedNext.sourceNs,"shorter runtime prediction horizon cannot publish backwards hands or renew age");
    fixture::advance();fixture::advance();
    const auto recovered=manager.RefreshUncappedPoseFamily();
    check(recovered.published&&recovered.target>cachedNext.target,"independent hands resume when the runtime target advances again");
    mt::SetMenuMode(2);mt::SetMenuMode(4);tr::Read(state);
    check(mt::handRefreshGenerationR94.load()!=handGeneration&&state.refreshGenerationR94==0,
        "leaving and reentering uncapped mode cannot revive its old hand family");
    fixture::advance();const auto newVisit=manager.RefreshUncappedPoseFamily();tr::Read(state);
    check(newVisit.published&&state.refreshGenerationR94==mt::handRefreshGenerationR94.load(),
        "new uncapped visit obtains a freshly acquired family");
    tr::Frame noHead{};check(!tr::ReadHead(noHead),"independent hands cannot create a head-cache publication after an epoch change");
    const auto beforeOffset=fixture::headLocates;
    CameraHook::source=3;tr::SetCameraSource(3);
    auto offset=manager.RefreshUncappedPoseFamily();
    check(!offset.published&&offset.result==Receipt::SourceOffset&&!tr::enabled.load()&&fixture::headLocates==beforeOffset,"offset source retains its existing disabled coherent-family policy");

    CameraHook::source=1;tr::SetCameraSource(1);
    manager.latest_predicted_display_time_=fixture::now+10000000;manager.hj1_predicted_now_=fixture::now;
    fresh_hands_r94::anchors.publish({fixture::now+10000000,fixture::now,11111111,tr::epoch.load()});
    fixture::duringQpc=[] {
        // The worker publishes immediately after the caller reads QPC. The
        // previously acquired valid anchor must not be replaced by this one.
        fresh_hands_r94::anchors.publish({fixture::now+12000000,fixture::now+1000000,11111111,tr::epoch.load()});
    };
    auto predicted=manager.RefreshUncappedPoseFamily();
    fixture::duringQpc={};
    check(predicted.published,"concurrent newer anchor does not falsely reject fresh hands");
    check(predicted.published&&predicted.target==manager.GetLatestPredictedDisplayTime()&&fixture::lastHeadTarget==fixture::lastGripTarget,"mode1 hands share one prediction reference with both controllers");
    fixture::now+=100000000;const auto beforeStale=fixture::headLocates;
    auto stale=manager.RefreshUncappedPoseFamily();
    check(!stale.published&&stale.result==Receipt::StalePrediction&&fixture::headLocates==beforeStale,"stopped XR publication cannot relabel an obsolete target as fresh");
    check(stale.sequence==predicted.sequence&&stale.sourceNs==predicted.sourceNs&&stale.sourceTick==predicted.sourceTick,"stale predicted target retains actual original sample age");
    manager.hj1_predicted_now_=fixture::now;
    check(manager.RefreshUncappedPoseFamily().result==Receipt::StalePrediction&&fixture::headLocates==beforeStale,"unrelated atomic supplier update cannot manufacture a coherent anchor");
    fresh_hands_r94::anchors.publish({fixture::now+10000000,fixture::now,11111111,tr::epoch.load()+1});
    check(manager.RefreshUncappedPoseFamily().result==Receipt::StalePrediction,"reference-space epoch mismatch is refused");
    manager.qpc_time_extension_enabled_=false;
    check(manager.RefreshUncappedPoseFamily().result==Receipt::ClockUnavailable,"missing clock conversion cannot invent a fresh sample");
    manager.qpc_time_extension_enabled_=true;

    CameraHook::source=2;tr::SetCameraSource(2);fixture::advance();
    claude_motion_d69::extra=5000000;
    const auto extraBefore=fixture::extraCalls;auto optional=manager.RefreshUncappedPoseFamily();tr::Read(state);
    check(optional.published&&state.handPolicyD69==5000000&&state.handTargetD69[0]==optional.target+5000000,"early refresh preserves the user's existing optional D69 policy");
    XrPose camera{};manager.SampleHeadPoseNow(camera);tr::Read(state);
    check(state.handPolicyD69==5000000&&fixture::extraCalls==extraBefore+2,"later camera route uses the same existing prediction helper/policy");
    claude_motion_d69::extra=0;

    hj1::enabled=true;hj1::Sample complete{};
    fixture::advance();check(manager.SampleHeadPoseNow(camera,nullptr,&complete)&&complete.ok,"mode4 late camera keeps its own fresh head and completed-family lookup");
    const auto beforeCamera=fixture::headLocates;XrPose unchanged=camera;
    elapsed=whileOtherThreadHolds(manager.pose_locate_mutex_,[&]{check(!manager.SampleHeadPoseNow(camera,nullptr,&complete),"mode4 camera locate honestly fails when pose mutex is busy");});
    check(elapsed<60&&!complete.ok&&fixture::headLocates==beforeCamera&&camera.pos_y==unchanged.pos_y,"mode4 camera contention does not fabricate success or renew a pose");
    mt::SetMenuMode(2);
    elapsed=whileOtherThreadHolds(manager.pose_locate_mutex_,[&]{check(manager.SampleHeadPoseNow(camera),"existing paced camera lookup retains its prior successful semantics");});
    check(elapsed>=80,"existing paced mode lock policy remains unchanged");
    hj1::enabled=false;

    mt::SetMenuMode(4);smooth_lab::earlySelected=smooth_lab::lateSelected=true;
    const auto startWaits=fixture::waits;const auto firstLocates=fixture::headLocates;
    fixture::advance();FrameSync::PaceGameplayBeforeUpdateD45();
    const auto oneLocates=fixture::headLocates;
    FrameSync::PaceGameplayBeforeUpdateD45();PaceGameplayAtD64(false);
    check(oneLocates==firstLocates+1&&fixture::headLocates==oneLocates,"new early refresh runs once per native frame and not at late camera dispatch");
    check(fixture::waits==startWaits,"uncapped dispatch bypasses all legacy smooth-lab waits");
    check(fixture::diagnosticClocks==0,"diagnostics off avoids extra timing clocks in the uncapped dispatch");
    check(!LegacyCameraWaitSelectedFixture(1)&&!LegacyCameraWaitSelectedFixture(2)&&!LegacyCameraWaitSelectedFixture(3),"uncapped also bypasses the separate legacy camera token gate");
    check(!CameraFailureReceiptSelectedFixture(2,false)&&fixture::diagnosticWrites==0&&diagnostics::ownership48::order.load()==0,"diagnostics off generates no mode4 failure or timing receipt");
    const auto priorFrame=fixture::nativeFrame;fixture::nativeFrame=0;FrameSync::PaceGameplayBeforeUpdateD45();
    check(fixture::headLocates==oneLocates,"refresh outside a native gameplay frame is refused");fixture::nativeFrame=priorFrame;
    CameraHook::injection=0;fixture::advance();FrameSync::PaceGameplayBeforeUpdateD45();
    check(fixture::headLocates==oneLocates,"inactive injection does not add pose acquisition");CameraHook::injection=1;
    const auto beforeBusyGrips=fixture::gripLocates;fixture::advance();
    whileOtherThreadHolds(manager.input_.mutex,[&]{
        FrameSync::PaceGameplayBeforeUpdateD45();FrameSync::PaceGameplayBeforeUpdateD45();PaceGameplayAtD64(false);
    });
    check(fixture::gripLocates==beforeBusyGrips+1&&fixture::waits==startWaits,"a contended early refresh is attempted only once in its native frame");

    cap::gate.select(cap::Domain::Vrik,true,fixture::tick);
    for(unsigned rung=0;rung<=3;++rung){
        mt::SetMode(rung);fixture::tick+=600;fixture::advance();
        FrameSync::PaceGameplayBeforeUpdateD45();PaceGameplayAtD64(false);
        check(fixture::waits==startWaits&&!mt::TimingSelectedAt(fixture::tick),"F7 comparison cannot re-enable pacing under persistent mode4");
    }
    check(fixture::diagnosticWrites==4&&fixture::messages.back().find("waitNs=0")!=std::string::npos&&fixture::messages.back().find("PUBLISHED")!=std::string::npos,"F7 receipt exposes actual publication and zero token wait");
    check(CameraFailureReceiptSelectedFixture(2,false),"active F7 may record mode4 camera contention");
    cap::gate.select(cap::Domain::Vrik,false,fixture::tick);mt::SetMode(0);
    cap::gate.select(cap::Domain::Bug,true,fixture::tick);fixture::advance();FrameSync::PaceGameplayBeforeUpdateD45();
    check(diagnostics::ownership48::order.load()==1,"active F2 receives the bounded structured uncapped event");
    cap::gate.select(cap::Domain::Bug,false,fixture::tick);
    const auto messages=fixture::diagnosticWrites;const auto events=diagnostics::ownership48::order.load();
    fixture::advance();FrameSync::PaceGameplayBeforeUpdateD45();
    check(fixture::diagnosticWrites==messages&&diagnostics::ownership48::order.load()==events,"turning recorders off stops mode4 diagnostic production");

    mt::SetMenuMode(2);fixture::advance();FrameSync::PaceGameplayBeforeUpdateD45();PaceGameplayAtD64(false);
    check(fixture::waits==startWaits+1&&LegacyCameraWaitSelectedFixture(2),"existing fresh-pose mode keeps its accepted late wait and explicit gate");
    mt::SetMenuMode(4);fixture::advance();FrameSync::PaceGameplayBeforeUpdateD45();PaceGameplayAtD64(false);
    check(fixture::waits==startWaits+1,"live switch back to uncapped stops token pacing immediately");
    mt::SetMenuMode(0);fixture::advance();FrameSync::PaceGameplayBeforeUpdateD45();PaceGameplayAtD64(false);
    check(fixture::waits==startWaits+2,"baseline legacy comparison remains on its existing one-claim-per-frame path");
    mt::SetMenuMode(3);fixture::advance();FrameSync::PaceGameplayBeforeUpdateD45();PaceGameplayAtD64(false);
    check(fixture::waits==startWaits+3,"existing elbow comparison retains its prior paced behavior");
    std::printf("fresh_pose_uncapped_core: %u/%u checks PASS; headset=NOT_RUN native_dll=NOT_BUILT\n",fixture::checks-fixture::failures,fixture::checks);
    return fixture::failures?1:0;
}
