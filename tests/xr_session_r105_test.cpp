// Actual EnsureInitialized, lifecycle helpers, RunFrame admission/event block,
// and scheduled reference-change block are inserted verbatim by the runner.
// D3D creation, OpenXR calls, sleep/clock, input and camera are explicit leaves.
// Full rendering, native threading, SDK ABI and headset behavior are not tested.
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <mutex>
#include <string>
#include <vector>
using XrInstance=std::uint64_t;using XrSession=std::uint64_t;using XrSpace=std::uint64_t;
using XrTime=std::int64_t;using XrResult=int;
constexpr std::uint64_t XR_NULL_HANDLE=0;
constexpr XrResult XR_SUCCESS=0,XR_EVENT_UNAVAILABLE=4,XR_ERROR_RUNTIME_FAILURE=-2,
    XR_ERROR_SESSION_RUNNING=-14,XR_ERROR_SESSION_NOT_RUNNING=-16,
    XR_ERROR_SESSION_NOT_READY=-28,XR_ERROR_SESSION_NOT_STOPPING=-29;
enum {XR_TYPE_EVENT_DATA_BUFFER=1,XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED,
    XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING,XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING,
    XR_TYPE_SESSION_BEGIN_INFO};
enum {XR_SESSION_STATE_IDLE=1,XR_SESSION_STATE_READY,XR_SESSION_STATE_SYNCHRONIZED,
    XR_SESSION_STATE_VISIBLE,XR_SESSION_STATE_FOCUSED,XR_SESSION_STATE_STOPPING,
    XR_SESSION_STATE_LOSS_PENDING,XR_SESSION_STATE_EXITING};
constexpr int XR_REFERENCE_SPACE_TYPE_LOCAL=2,XR_REFERENCE_SPACE_TYPE_STAGE=3,
    XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO=2;
#define XR_FAILED(x) ((x)<0)
#define XR_SUCCEEDED(x) ((x)>=0)
// Identical aliases keep these host event buffers free of fixture aliasing UB;
// their layout is deliberately not offered as an OpenXR ABI declaration.
struct XrEventDataBuffer {int type;XrSession session=0;int state=0,referenceSpaceType=0;XrTime changeTime=0;};
using XrEventDataSessionStateChanged=XrEventDataBuffer;
using XrEventDataReferenceSpaceChangePending=XrEventDataBuffer;
struct XrSessionBeginInfo {int type;int primaryViewConfigurationType=0;};
struct ID3D12Device {};struct ID3D12CommandQueue {};
namespace fixture {
struct Event {XrEventDataBuffer data;unsigned afterSleeps=0;};
struct Runtime {
    std::deque<Event> events;
    unsigned sleeps=0,polls=0,begins=0,ends=0,recenters=0;
    int nativeState=XR_SESSION_STATE_IDLE;bool nativeRunning=false;
    unsigned cancelAt=~0u;std::atomic<bool>* cancel=nullptr;
    XrResult pollResult=XR_EVENT_UNAVAILABLE,beginResult=XR_SUCCESS,endResult=XR_SUCCESS;
    std::vector<std::string> status;
    void event(int state,XrSession session=2,unsigned after=0){events.push_back({{XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED,session,state,0,0},after});}
    void reference(XrTime when,int type=XR_REFERENCE_SPACE_TYPE_LOCAL,XrSession session=2){events.push_back({{XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING,session,0,type,when},sleeps});}
    void loss(){events.push_back({{XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING,0,0,0,0},sleeps});}
} rt;
}
static XrResult xrPollEvent(XrInstance,XrEventDataBuffer* event){auto& r=fixture::rt;++r.polls;if(r.events.empty()||r.events.front().afterSleeps>r.sleeps)return r.pollResult;*event=r.events.front().data;r.events.pop_front();if(event->type==XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED&&event->session==2)r.nativeState=event->state;return XR_SUCCESS;}
static XrResult xrBeginSession(XrSession,const XrSessionBeginInfo*){auto& r=fixture::rt;++r.begins;if(r.nativeRunning)return XR_ERROR_SESSION_RUNNING;if(r.nativeState!=XR_SESSION_STATE_READY)return XR_ERROR_SESSION_NOT_READY;if(XR_SUCCEEDED(r.beginResult))r.nativeRunning=true;return r.beginResult;}
static XrResult xrEndSession(XrSession){auto& r=fixture::rt;++r.ends;if(!r.nativeRunning)return XR_ERROR_SESSION_NOT_RUNNING;if(r.nativeState!=XR_SESSION_STATE_STOPPING)return XR_ERROR_SESSION_NOT_STOPPING;if(XR_SUCCEEDED(r.endResult))r.nativeRunning=false;return r.endResult;}
static void Sleep(unsigned ms){auto& r=fixture::rt;if(ms==8)return; /* R106: stopped-session idle pause is not a startup poll */ if(ms!=25)std::abort();++r.sleeps;if(r.cancel&&r.sleeps>=r.cancelAt)r.cancel->store(false);if(r.sleeps>10000)std::abort();}
#define KCD2VR_DIAG_LOG(...) ((void)0)
namespace kcd2vr {
struct XrPose {};
namespace debug_trace {inline void Write(const char*){}}
namespace diagnostic_trace {inline void WriteStatus(const char* text){fixture::rt.status.emplace_back(text);}inline void Write(const char* text){WriteStatus(text);}}
namespace diagnostics::xr165 {enum Kind {Frame};struct Timer {explicit Timer(Kind){}};}
struct CameraHook {static void RequestRecenter(){++fixture::rt.recenters;}};
struct InputLeaf {
    unsigned initializes=0,invalidates=0,stops=0;bool active=true,level=true;
    bool Initialize(XrInstance,XrSession,XrSpace){++initializes;return true;}
    void InvalidateFrameR105(){++invalidates;active=level=false;}
    void OnSessionStoppedR105(){++stops;active=level=false;}
};
class XrManager {
public:
    XrInstance instance_=1;XrSession session_=2;XrSpace local_space_=3;
    std::atomic<bool> session_running_{false};bool init_attempted_=false;
    std::atomic<bool> diagnostic_rendering_{false},active_vr_bridge_{false};
    void* qpc_to_xr_time_=reinterpret_cast<void*>(1);bool qpc_time_extension_enabled_=true;
    InputLeaf input_;
    struct Comfort {unsigned resets=0;void Reset(){++resets;}} comfort_envelope51_;
    std::mutex flat_ui_mutex_;bool flat_ui_anchor_valid_=true,ui_layer_test_anchor_valid_=true,hud_lag_valid_=true,head_receipt_pending_r105_=false;
    XrTime previous_predicted_display_time_=999;
    std::vector<XrTime> pending_reference_changes125_;
    bool CreateInstance(){return true;}bool GetSystem(){return true;}
    bool CreateSession(ID3D12Device*,ID3D12CommandQueue*){return true;}
    bool CreateReferenceSpace(){return true;}bool CreateD3D12Objects(){return true;}
    bool CreateSwapchains(){return true;}bool CreateBlitPipeline(){return true;}
    @INIT_DECLARATION@;
#if HAS_SESSION_R105
    void ResetUiAnchorsR105() noexcept;
    void PollSessionEventsR105();
#endif
    bool RunFrame(XrPose& out_head_pose,float head_pose_scale);
    bool Start(std::atomic<bool>& keep){
#if HAS_INIT_CANCEL_R105
        return EnsureInitialized(nullptr,nullptr,&keep);
#else
        (void)keep;return EnsureInitialized(nullptr,nullptr);
#endif
    }
    void ApplyReference(XrTime predicted){struct {XrTime predictedDisplayTime;} frame_state{predicted};
        @REFERENCE_BLOCK@
    }
};
@ENSURE_INITIALIZED@
@LIFECYCLE_HELPERS@
@FRAME_ADMISSION@
} // namespace kcd2vr

namespace {
unsigned checks=0;std::vector<std::string> failures;
void check(bool ok,const char* why){++checks;if(!ok){failures.emplace_back(why);std::fprintf(stderr,"FAIL: %s\n",why);}}
struct Case {
    kcd2vr::XrManager manager;std::atomic<bool> keep{true};
    Case(){fixture::rt={};fixture::rt.cancel=&keep;}
    bool start(){return manager.Start(keep);}
    bool frame(){kcd2vr::XrPose ignored;return manager.RunFrame(ignored,1);}
    void running(){fixture::rt.event(XR_SESSION_STATE_READY);check(start(),"initialize running session for lifecycle case");}
};
}
int main(){
    using fixture::rt;
    {Case c;rt.event(XR_SESSION_STATE_READY);check(c.start(),"immediate READY starts session");check(rt.sleeps==0&&rt.begins==1&&c.manager.input_.initializes==1,"healthy startup adds no wait or extra action initialization");}
    {Case c;rt.event(XR_SESSION_STATE_READY,2,1000);check(c.start()&&rt.sleeps==1000&&rt.begins==1,"READY after 25 seconds starts without timeout latch");}
    {Case c;rt.cancelAt=30;check(!c.start()&&!rt.begins&&!c.manager.input_.initializes,"cancelled startup never starts native session or actions");check(rt.sleeps==30,"startup responds to caller cancellation");}
    {Case c;rt.event(XR_SESSION_STATE_EXITING);check(!c.start()&&rt.sleeps==0&&rt.begins==0,"terminal startup session exits immediately");}
    {Case c;rt.loss();check(!c.start()&&rt.sleeps==0&&rt.begins==0,"instance loss exits startup immediately");}
    {Case c;rt.pollResult=XR_ERROR_RUNTIME_FAILURE;check(!c.start()&&rt.sleeps==0&&rt.begins==0,"startup poll error exits without deadline wait");}
    {Case c;rt.event(XR_SESSION_STATE_EXITING,99);rt.event(XR_SESSION_STATE_READY,2,2);check(c.start()&&rt.sleeps==2&&rt.begins==1,"other-session terminal event cannot cancel current startup");}
    {Case c;rt.event(XR_SESSION_STATE_READY,99);rt.event(XR_SESSION_STATE_READY,2,2);check(c.start()&&rt.sleeps==2&&rt.begins==1,"other-session READY cannot prematurely begin current session");}
    {
        Case c;c.running();auto& m=c.manager;
        rt.event(XR_SESSION_STATE_STOPPING);
        check(!c.frame()&&!m.session_running_.load()&&!m.input_.active&&!m.input_.level,"STOPPING stops current-frame admission and clears input");
        check(rt.ends==1&&!rt.nativeRunning&&m.input_.stops==1,"STOPPING ends native session and retires local output receipts");
        check(!m.flat_ui_anchor_valid_&&!m.ui_layer_test_anchor_valid_&&!m.hud_lag_valid_,"STOPPING discards anchors tied to old visit");
        const auto polls=rt.polls;check(!c.frame()&&rt.polls>polls,"stopped session continues polling events");
        rt.event(XR_SESSION_STATE_READY);
        check(c.frame()&&m.session_running_.load()&&rt.nativeRunning&&rt.begins==2,"READY resumes same stopped native session");
        check(m.input_.initializes==1,"resume retains already attached action set");
        check(m.qpc_to_xr_time_&&m.qpc_time_extension_enabled_,"STOPPING resume retains established OpenXR clock converter");
        check(rt.recenters==1&&m.previous_predicted_display_time_==0&&m.head_receipt_pending_r105_,"resume clears prediction cadence and requests origin recapture");
        const auto begins=rt.begins;rt.event(XR_SESSION_STATE_READY);check(c.frame()&&rt.begins==begins,"duplicate READY cannot begin already running session");
        rt.event(XR_SESSION_STATE_VISIBLE);rt.event(XR_SESSION_STATE_FOCUSED);
        check(c.frame()&&rt.begins==begins&&rt.ends==1&&m.input_.initializes==1,"focus changes do not restart session or attach actions");
        rt.event(XR_SESSION_STATE_STOPPING,99);
        check(c.frame()&&m.session_running_.load()&&rt.ends==1,"other-session STOPPING cannot stop this session");
    }
    {Case c;c.running();rt.event(XR_SESSION_STATE_LOSS_PENDING);check(!c.frame()&&!c.manager.input_.active&&c.manager.input_.stops==1&&rt.ends==0,"session loss clears input without illegal EndSession");}
    {Case c;c.running();rt.loss();check(!c.frame()&&!c.manager.input_.active&&c.manager.input_.stops==1&&rt.ends==0,"instance loss clears input without illegal EndSession");}
    {Case c;c.running();rt.event(XR_SESSION_STATE_EXITING);check(!c.frame()&&!c.manager.input_.active&&rt.ends==0,"EXITING stops admission without spontaneous restart");}
    {
        Case c;c.running();auto& m=c.manager;
        rt.reference(500);rt.reference(700,XR_REFERENCE_SPACE_TYPE_STAGE);rt.reference(800,XR_REFERENCE_SPACE_TYPE_LOCAL,99);
        check(c.frame()&&m.pending_reference_changes125_.size()==1,"queue only this session LOCAL reference changes");
        m.ApplyReference(499);
        check(m.pending_reference_changes125_.size()==1&&rt.recenters==0&&m.flat_ui_anchor_valid_,"future reference change leaves present origin and anchors intact");
        m.ApplyReference(500);
        check(m.pending_reference_changes125_.empty()&&rt.recenters==1,"reference change applied at its display-time boundary");
        check(!m.flat_ui_anchor_valid_&&!m.ui_layer_test_anchor_valid_&&!m.hud_lag_valid_&&m.head_receipt_pending_r105_,"due reference change discards stale UI anchors");
    }
#if HAS_SESSION_R105
    {Case c;c.running();rt.event(XR_SESSION_STATE_STOPPING);c.frame();rt.beginResult=XR_ERROR_RUNTIME_FAILURE;rt.event(XR_SESSION_STATE_READY);check(!c.frame()&&!c.manager.session_running_.load()&&!c.manager.input_.active,"failed resume never claims active session or input");}
    {Case c;c.manager.instance_=0;const auto before=rt.polls;check(!c.frame()&&rt.polls==before,"unavailable instance does not poll invalid handle");}
#endif
    std::printf("{\"checks\":%u,\"failures\":%zu,\"has_lifecycle_helpers\":%s,\"failed_checks\":[",checks,failures.size(),HAS_SESSION_R105?"true":"false");
    for(std::size_t i=0;i<failures.size();++i)std::printf("%s\"%s\"",i?",":"",failures[i].c_str());
    std::puts("]}");return failures.empty()?0:1;
}
