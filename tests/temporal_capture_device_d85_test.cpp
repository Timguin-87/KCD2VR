#include "temporal_capture_transaction_d85.h"
#include "temporal_pair_watchdog_d85.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#define __stdcall
using UINT=unsigned;using DWORD=unsigned;using HRESULT=int;using HANDLE=void*;
constexpr HRESULT E_FAIL=-1;constexpr bool FALSE=false;
#define FAILED(x) ((x)<0)
#define SUCCEEDED(x) ((x)>=0)
#define IID_PPV_ARGS(x) x
using D3D12_RESOURCE_STATES=int;
constexpr int D3D12_RESOURCE_STATE_COMMON=0,D3D12_RESOURCE_STATE_PRESENT=0,D3D12_RESOURCE_STATE_RENDER_TARGET=4,
 D3D12_RESOURCE_STATE_COPY_SOURCE=2,D3D12_RESOURCE_STATE_COPY_DEST=3,D3D12_RESOURCE_DIMENSION_TEXTURE2D=2,
 D3D12_RESOURCE_BARRIER_TYPE_TRANSITION=1,D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES=-1,
 D3D12_HEAP_TYPE_DEFAULT=1,D3D12_HEAP_FLAG_NONE=0,D3D12_FENCE_FLAG_NONE=0,D3D12_COMMAND_LIST_TYPE_DIRECT=0;
static std::uint64_t fixtureClock=500;
DWORD GetLastError(){return 0;}void SetLastError(DWORD){}std::uint64_t GetTickCount64(){return fixtureClock;}HANDLE CreateEvent(void*,bool,bool,void*){return reinterpret_cast<void*>(1);}
struct D3D12_RESOURCE_DESC{int Dimension=2;std::uint64_t Width=1920;unsigned Height=1800,Format=28,Flags=0;struct{unsigned Count=1;}SampleDesc;};
struct D3D12_HEAP_PROPERTIES{unsigned Type{},CreationNodeMask{},VisibleNodeMask{};};
static unsigned gpuChecks=0;
struct ID3D12Resource{D3D12_RESOURCE_DESC desc{};int refs=1,state=0,pixel=0;void AddRef(){++refs;}void Release(){assert(refs>0);--refs;}D3D12_RESOURCE_DESC GetDesc(){assert(refs>0);return desc;}};
struct ID3D12Fence{int refs=1;std::uint64_t completed=0;void AddRef(){++refs;}void Release(){assert(refs>0);--refs;}std::uint64_t GetCompletedValue(){assert(refs>0);return completed;}};
struct D3D12_RESOURCE_BARRIER{int Type{};struct{ID3D12Resource*pResource{};int Subresource{},StateBefore{},StateAfter{};}Transition;};
struct ID3D12CommandAllocator{};struct ID3D12PipelineState{};
struct ID3D12GraphicsCommandList{
 std::vector<std::function<void()>> commands;
 HRESULT Reset(ID3D12CommandAllocator*,ID3D12PipelineState*){commands.clear();return 0;}HRESULT Close(){return 0;}
 void ResourceBarrier(UINT n,const D3D12_RESOURCE_BARRIER* b){for(unsigned i=0;i<n;++i){auto t=b[i].Transition;commands.push_back([t]{assert(t.pResource->refs>0);assert(t.pResource->state==t.StateBefore);t.pResource->state=t.StateAfter;++gpuChecks;});}}
 void CopyResource(ID3D12Resource*dst,ID3D12Resource*src){commands.push_back([=]{assert(src->refs>0&&dst->refs>0);assert(src->state==2&&dst->state==3);dst->pixel=src->pixel;++gpuChecks;});}
 void Draw(ID3D12Resource*r,int pixel){commands.push_back([=]{assert(r->refs>0&&r->state==4);r->pixel=pixel;++gpuChecks;});}
};
using ID3D12CommandList=ID3D12GraphicsCommandList;
struct ID3D12CommandQueue{
 bool failSignal=false,defer=false;ID3D12Fence* failFence=nullptr;std::vector<std::function<void()>> pending;
 void Push(std::function<void()> f){if(defer)pending.push_back(std::move(f));else f();}
 void Execute(UINT n,ID3D12CommandList*const* lists){for(unsigned i=0;i<n;++i)for(auto f:lists[i]->commands)Push(std::move(f));}
 HRESULT Signal(ID3D12Fence*f,std::uint64_t v){if(failSignal||failFence==f)return -1;Push([=]{assert(f->refs>0);f->completed=v;});return 0;}
 void Flush(){auto work=std::move(pending);pending.clear();for(auto&f:work)f();}
};
struct ID3D12Device{
 std::vector<std::unique_ptr<ID3D12Resource>> resources;std::vector<std::unique_ptr<ID3D12Fence>> fences;
 std::vector<std::unique_ptr<ID3D12CommandAllocator>> allocators;std::vector<std::unique_ptr<ID3D12GraphicsCommandList>> lists;
 HRESULT CreateCommittedResource(D3D12_HEAP_PROPERTIES*,int,const D3D12_RESOURCE_DESC*d,int state,void*,ID3D12Resource**out){auto p=std::make_unique<ID3D12Resource>();p->desc=*d;p->state=state;*out=p.get();resources.push_back(std::move(p));return 0;}
 HRESULT CreateFence(std::uint64_t,int,ID3D12Fence**out){auto p=std::make_unique<ID3D12Fence>();*out=p.get();fences.push_back(std::move(p));return 0;}
 HRESULT CreateCommandAllocator(int,ID3D12CommandAllocator**out){auto p=std::make_unique<ID3D12CommandAllocator>();*out=p.get();allocators.push_back(std::move(p));return 0;}
 HRESULT CreateCommandList(int,int,ID3D12CommandAllocator*,void*,ID3D12GraphicsCommandList**out){auto p=std::make_unique<ID3D12GraphicsCommandList>();*out=p.get();lists.push_back(std::move(p));return 0;}
};
namespace kcd2vr {
namespace diagnostics::capture168{
enum class Domain{Bug};bool enabled=false;
struct Gate{std::array<std::atomic<std::uint64_t>,1>session{};}gate;
bool active(Domain,std::uint64_t){return enabled;}
}
namespace diagnostic_trace{
std::vector<std::string> captureRows;
void Write(const char* s){if(std::strncmp(s,"D85R5R4 TEMPORAL_CAPTURE ",24)==0)captureRows.emplace_back(s);}
}namespace debug_trace{void Write(const char*){}}
namespace hud_layer_v6664{void OnSubmittedBatch(UINT,ID3D12CommandList*const*){}void OnLateWindowOpen(std::uint32_t){}}
namespace CameraHook{std::uint64_t LastPoseFrameIndex(){return 1000;}}
namespace FrameSync{std::uint64_t LatestRendererMatchedPoseFrame(int){return 900;}}
namespace native_upscaler_stereo_d82{
struct PairOwner{std::uint64_t epoch{};int technology{};};
std::uint64_t epoch=7;bool ready=true;std::uint64_t CurrentEpoch(){return epoch;}int BoundTechnology(){return 2;}
bool TemporalActive(){return true;}bool PairReady(std::uint64_t e,std::uint32_t){return ready&&e==epoch;}void NotifyPublished(std::uint64_t,std::uint32_t){}
}
std::recursive_mutex g_temporal_mutex_d85;temporal_capture_d85::Owner g_temporal_owner_d85;
temporal_pair_d85::Watchdog g_d85_watchdog;
std::atomic<bool>g_d82_temporal_tracking{true},g_d82_temporal_hooks_ready{true},g_v6624_late_window{true},g_v6615_right_copy_queued{false},g_v6615_pair_published{false},g_v6618_live_stream_started{false},g_v6615_left_copy_queued{false},g_v6615_active{true};
std::atomic<std::uint32_t>g_v6624_late_frame{100},g_v6613_pair_frame{100};
std::atomic<std::uint64_t>g_v6615_last_left_main_batch{1},g_v6638_left_pose_frame{0},g_v6638_right_pose_frame{0},g_v6638_shared_pose_frame{0};std::atomic<int>g_v6638_pair_pose_mode{0};
ID3D12Resource*g_v6634_pub_left=nullptr,*g_v6634_pub_right=nullptr,*g_v6634_xr_claim_left=nullptr;
native_upscaler_stereo_d82::PairOwner publishedOwner{};
void V6634PublishWriteSlotLocked(ID3D12Resource*l,ID3D12Resource*r,std::uint64_t,native_upscaler_stereo_d82::PairOwner owner={}){g_v6634_pub_left=l;g_v6634_pub_right=r;publishedOwner=owner;}
std::atomic<bool> pauseNative{false},nativeEntered{false},nativeRelease{false};ID3D12CommandList*pauseList=nullptr;
void NativeExecute(ID3D12CommandQueue*q,UINT n,ID3D12CommandList*const*l){
 if(pauseNative&&n==1&&l[0]==pauseList){nativeEntered=true;while(!nativeRelease)std::this_thread::yield();}
 q->Execute(n,l);
}
void NativeBarrier(ID3D12GraphicsCommandList*l,UINT n,const D3D12_RESOURCE_BARRIER*b){l->ResourceBarrier(n,b);}
bool resetSucceeds=true;HRESULT NativeReset(ID3D12GraphicsCommandList*l,ID3D12CommandAllocator*a,ID3D12PipelineState*p){return resetSucceeds?l->Reset(a,p):E_FAIL;}
void V6612DiscardTemporalRecordingD82(ID3D12GraphicsCommandList*){}
void V6612DiscardMainWriterD85(ID3D12GraphicsCommandList*) noexcept;
bool incomingRight=true;bool V6615IncomingLockedRightMainWriter(UINT,ID3D12CommandList*const*) noexcept;
temporal_compose_d82::Stamp V6615IncomingRightCompositionD82(UINT,ID3D12CommandList*const*){return {};}
ID3D12CommandAllocator* BridgeAcquireCopyAllocatorV6619(ID3D12Device*d,ID3D12Fence*){ID3D12CommandAllocator*a{};d->CreateCommandAllocator(0,&a);return a;}
void BridgeMarkCopyAllocatorSubmittedV6619(std::uint64_t){}
// State for the complete, literally extracted production phase function.
constexpr bool kV6647HistoryCensus=false;
struct Candidate {void*resource{};D3D12_RESOURCE_DESC desc{};bool via_uav{},same_main{};unsigned eye_mask{};
 unsigned pass_first[2]{},reads_other_eye[2]{},passes_seen[2]{},read_first[2]{},write_first[2]{},dsv_binds[2]{},binds[2]{},barriers[2]{},to_srv[2]{},from_srv[2]{},to_rt[2]{},last_frame[2]{};int last_writer{-1};};
std::mutex g_v6610_output_mutex;std::array<Candidate,4>g_v6610_output_candidates{};int g_v6610_output_count=0;
std::uint64_t g_v6647_pass_serial=0,g_v6647_last_log_ms=0,g_v6634_pub_serial=0;
std::atomic<bool>g_v6610_output_enabled{true},g_v6636_cadence_owner{false},g_v6613_pair_capture_active{false},g_v6614_right_submitted_seen{false},g_v6612_drain_requested{false};
std::atomic<int>g_v6610_output_eye{-1};std::atomic<std::uint32_t>g_v6610_output_frame{0},g_v6615_last_left_main_frame{0};
std::atomic<std::uint64_t>g_v6624_summary_serial{0},g_v6636_xr_opportunity{0},g_v6636_last_started_opportunity{0},g_v6636_cadence_skips{0},g_v6634_swaps{0},g_v6634_claim_skips{0},g_v6634_superseded{0},g_v6618_live_skip_unconsumed{0},g_v6618_live_rearm_count{0};
ID3D12Resource*g_v6634_spare_left=nullptr,*g_v6634_spare_right=nullptr;D3D12_RESOURCE_DESC g_v6634_spare_left_desc{},g_v6634_spare_right_desc{};
const char*V6647Fmt(unsigned){return "fixture";}
#include "temporal_capture_census_actual.inc"
void V6612ResetTracking(){g_v6612_live_lists={};g_v6612_live_count=0;g_v6615_left_copy_queued=false;g_v6615_right_copy_queued=false;g_v6615_pair_published=false;g_v6615_last_left_main_frame=0;g_v6615_last_left_main_batch=0;}
class DeviceHook{public:
 inline static std::atomic<bool>s_v6627_performance_quiet{true};
 static void SetV6610OutputCensusPhase(int,std::uint32_t);
 inline static std::mutex s_bridge_capture_mutex;
 inline static std::atomic<ID3D12Device*>s_captured_device{nullptr};inline static std::atomic<ID3D12CommandQueue*>s_captured_queue{nullptr};
 inline static ID3D12Resource*s_bridge_current_main_target=nullptr,*s_bridge_left_capture=nullptr,*s_bridge_right_capture=nullptr;
 inline static ID3D12GraphicsCommandList*s_bridge_current_main_list=nullptr,*s_bridge_copy_list=nullptr;
 inline static ID3D12CommandAllocator*s_bridge_copy_allocator=nullptr;inline static ID3D12Fence*s_bridge_copy_fence=nullptr;inline static HANDLE s_bridge_copy_fence_event=nullptr;
 inline static std::uint64_t s_bridge_copy_fence_value=0;inline static int s_bridge_current_main_state=4;
 inline static D3D12_RESOURCE_DESC s_bridge_left_desc{},s_bridge_right_desc{};
 inline static std::atomic<std::uint64_t>s_bridge_current_main_last_submit{1},s_bridge_copy_count{0},s_execute_submission_count{0},s_bridge_left_submitted_serial{0},s_bridge_right_submitted_serial{0},s_bridge_complete_pair_camera_frame{0},s_bridge_complete_pair_serial{0},s_bridge_complete_pair_count{0};
 inline static auto s_original_resource_barrier=&NativeBarrier;inline static auto s_original_execute_command_lists=&NativeExecute;
 inline static std::atomic<decltype(&NativeReset)>s_original_reset_command_list_d82{&NativeReset};
 static bool RecordTemporalFinalMainD85(ID3D12GraphicsCommandList*,ID3D12Resource*,D3D12_RESOURCE_STATES)noexcept;
 static void FinishTemporalMainSubmissionD85(ID3D12CommandQueue*,const temporal_capture_d85::Ticket&,std::uint64_t)noexcept;
 static bool BridgeCaptureCurrentMainTargetToLeft(std::uint64_t,ID3D12Resource* =nullptr,ID3D12Resource** =nullptr,const D3D12_RESOURCE_STATES* =nullptr,std::uint64_t=0);
 static HRESULT DetourResetCommandListD82(ID3D12GraphicsCommandList*,ID3D12CommandAllocator*,ID3D12PipelineState*);
 static void DetourExecuteCommandLists(ID3D12CommandQueue*,UINT,ID3D12CommandList*const*);
};
#include "temporal_capture_device_d85.inl"
#include "temporal_capture_adapter_actual.inc"
}
using namespace kcd2vr;
static int checks=0,failures=0;
#define CHECK(x) do{++checks;if(!(x)){++failures;std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);}}while(0)
static ID3D12Device device;static ID3D12CommandQueue queue;static ID3D12Resource mainTarget;static ID3D12GraphicsCommandList list;
void Fresh(std::uint32_t frame=100){
 queue.Flush();for(auto&p:g_temporal_pins_d85)if(p.left)ReleaseTemporalPinsD85(p);
 g_temporal_owner_d85={};g_v6612_live_lists={};g_v6612_live_count=0;g_v6634_pub_left=g_v6634_pub_right=g_v6634_xr_claim_left=nullptr;
 DeviceHook::s_captured_device=&device;DeviceHook::s_captured_queue=&queue;
 DeviceHook::s_bridge_current_main_target=&mainTarget;DeviceHook::s_bridge_current_main_list=&list;
 DeviceHook::s_bridge_left_capture=DeviceHook::s_bridge_right_capture=nullptr;
 DeviceHook::s_bridge_current_main_last_submit=1;DeviceHook::s_bridge_current_main_state=4;
 DeviceHook::s_bridge_complete_pair_serial=0;DeviceHook::s_bridge_complete_pair_count=0;
 list.commands.clear();mainTarget.refs=1;mainTarget.state=4;mainTarget.pixel=10;queue.failSignal=false;queue.failFence=nullptr;queue.defer=false;
 native_upscaler_stereo_d82::epoch=7;native_upscaler_stereo_d82::ready=true;
 g_v6624_late_frame=frame;g_v6613_pair_frame=frame;g_v6624_late_window=true;
 g_d82_temporal_tracking=true;g_d82_temporal_hooks_ready=true;g_v6615_active=true;g_v6615_last_left_main_batch=1;g_v6615_left_copy_queued=false;g_v6615_right_copy_queued=false;
 resetSucceeds=true;incomingRight=true;g_v6618_live_stream_started=false;g_v6613_pair_capture_active=false;g_v6610_output_eye=-1;g_v6634_spare_left=g_v6634_spare_right=nullptr;g_temporal_owner_d85.Begin(7,frame);g_temporal_owner_d85.NoteLeftBoundary(7,frame,reinterpret_cast<std::uintptr_t>(&mainTarget),4,801);
 g_temporal_owner_d85.CompleteRight(7,frame,true,802);
}
bool Record(){list.Draw(&mainTarget,20);return DeviceHook::RecordTemporalFinalMainD85(&list,&mainTarget,4);}
void DesktopFinish(){DeviceHook::s_bridge_current_main_state=0;list.Draw(&mainTarget,99);D3D12_RESOURCE_BARRIER b{};b.Transition={&mainTarget,-1,4,0};list.ResourceBarrier(1,&b);}
void Submit(){
 if(!g_v6612_live_count){auto&t=g_v6612_live_lists[0];t.list=&list;t.main_bind_mask=2;t.frame[1]=g_temporal_owner_d85.Current().frame;g_v6612_live_count=1;}
 // Native LEFT submission is a GPU/engine leaf; its actual admission policy is
 // extracted below and exercised with pending/mixed list controls.
 g_v6615_last_left_main_frame=incomingRight?g_temporal_owner_d85.Current().frame:0;g_v6615_last_left_main_batch=incomingRight?1:0;
 ID3D12CommandList*lists[]{&list};DeviceHook::DetourExecuteCommandLists(&queue,1,lists);
}
void TraceSession(){
 fixtureClock+=3000;diagnostics::capture168::enabled=true;
 ++diagnostics::capture168::gate.session[0];diagnostic_trace::captureRows.clear();
}
bool HasTrace(const char* stage,const char* reason){
 for(const auto& row:diagnostic_trace::captureRows)
  if(row.find(stage)!=std::string::npos&&row.find(reason)!=std::string::npos&&
     row.find("gpuContent=UNVERIFIED")!=std::string::npos)return true;
 return false;
}
int main(){
 Fresh();CHECK(Record());CHECK(g_temporal_owner_d85.Pending());CHECK(DeviceHook::s_bridge_complete_pair_serial==0);
 const auto first=g_temporal_owner_d85.Current();CHECK(!g_temporal_owner_d85.Begin(7,101));
 DesktopFinish();Submit();CHECK(!g_temporal_owner_d85.Pending());CHECK(DeviceHook::s_bridge_complete_pair_count==1);
 CHECK(g_v6634_pub_left->pixel==10);CHECK(g_v6634_pub_right->pixel==20);CHECK(mainTarget.pixel==99);
 CHECK(g_v6638_left_pose_frame==801&&g_v6638_right_pose_frame==802);CHECK((DeviceHook::s_bridge_complete_pair_serial.load()>>32)==100);
 // Optional native scaling blit never ran; actual final-main copy still published.
 CHECK(!g_temporal_owner_d85.Matches(first));
 Fresh();g_d82_temporal_hooks_ready=false;CHECK(!Record());CHECK(!g_temporal_owner_d85.Pending());
 Fresh();native_upscaler_stereo_d82::ready=false;CHECK(!Record());
 Fresh();CHECK(!DeviceHook::RecordTemporalFinalMainD85(&list,&mainTarget,0));
 Fresh();g_temporal_owner_d85.Begin(7,100);CHECK(!Record()); // no completed RIGHT
 Fresh();ID3D12Resource wrong;CHECK(!DeviceHook::RecordTemporalFinalMainD85(&list,&wrong,4));
 Fresh();CHECK(Record());const auto record=g_temporal_owner_d85.Current();ID3D12GraphicsCommandList other;
 CHECK(!g_temporal_owner_d85.Incoming(reinterpret_cast<std::uintptr_t>(&other)).recorded);
 CHECK(!g_temporal_owner_d85.Publishable(record,true,false));
 auto tampered=record;++tampered.recording;CHECK(!g_temporal_owner_d85.Matches(tampered));
 ++tampered.generation;CHECK(!g_temporal_owner_d85.Publishable(tampered,true,true));
 Fresh();CHECK(Record());const auto leftRefs=DeviceHook::s_bridge_left_capture->refs;
 resetSucceeds=false;CHECK(FAILED(DeviceHook::DetourResetCommandListD82(&list,nullptr,nullptr)));
 CHECK(g_temporal_owner_d85.Pending());CHECK(DeviceHook::s_bridge_left_capture->refs==leftRefs);
 resetSucceeds=true;CHECK(SUCCEEDED(DeviceHook::DetourResetCommandListD82(&list,nullptr,nullptr)));
 CHECK(!g_temporal_owner_d85.Pending());CHECK(DeviceHook::s_bridge_left_capture->refs==leftRefs-1);
 CHECK(g_temporal_owner_d85.Begin(7,101));CHECK(!g_temporal_owner_d85.Matches(record));
 Fresh();CHECK(Record());g_temporal_owner_d85.Cancel();DesktopFinish();Submit();CHECK(DeviceHook::s_bridge_complete_pair_count==0);CHECK(!g_temporal_owner_d85.Pending());
 Fresh();CHECK(Record());native_upscaler_stereo_d82::epoch=8;DesktopFinish();Submit();CHECK(DeviceHook::s_bridge_complete_pair_count==0);
 Fresh();CHECK(Record());incomingRight=false;DesktopFinish();Submit();CHECK(DeviceHook::s_bridge_complete_pair_count==0); // no LEFT preservation
 Fresh();CHECK(Record());queue.defer=true;DesktopFinish();Submit();CHECK(DeviceHook::s_bridge_complete_pair_count==1);
 auto*retainedL=DeviceHook::s_bridge_left_capture;auto*retainedR=DeviceHook::s_bridge_right_capture;
 retainedL->Release();retainedR->Release();CHECK(retainedL->refs>0&&retainedR->refs>0);
 ReapTemporalPinsD85();CHECK(retainedL->refs>0&&retainedR->refs>0);queue.Flush();ReapTemporalPinsD85();CHECK(retainedL->refs==0&&retainedR->refs==0);
 Fresh();CHECK(Record());queue.failSignal=true;DesktopFinish();Submit();CHECK(DeviceHook::s_bridge_complete_pair_count==0);
 CHECK(!g_temporal_owner_d85.Pending());auto*failedPin=DeviceHook::s_bridge_right_capture;failedPin->Release();
 ReapTemporalPinsD85();CHECK(failedPin->refs>0);g_temporal_fence_d85->completed=UINT64_MAX;ReapTemporalPinsD85();CHECK(failedPin->refs>0);
 Fresh();CHECK(Record());auto*s=DeviceHook::s_bridge_left_capture;
 CHECK(!g_temporal_owner_d85.MarkLeft(g_temporal_owner_d85.Current().generation,999,reinterpret_cast<std::uintptr_t>(s),true,801));
 DesktopFinish();Submit();CHECK(DeviceHook::s_bridge_complete_pair_count==0);
 // New attempts may re-arm after failed publication; no frozen first-frame target.
 Fresh();native_upscaler_stereo_d82::ready=false;CHECK(!Record());native_upscaler_stereo_d82::ready=true;
 CHECK(g_temporal_owner_d85.Begin(7,101));g_v6624_late_frame=101;g_v6613_pair_frame=101;
 g_temporal_owner_d85.NoteLeftBoundary(7,101,reinterpret_cast<std::uintptr_t>(&mainTarget),4,901);CHECK(g_temporal_owner_d85.CompleteRight(7,101,true,902));list.commands.clear();
 CHECK(Record());DesktopFinish();Submit();CHECK(DeviceHook::s_bridge_complete_pair_count==1);CHECK(g_v6638_left_pose_frame==901&&g_v6638_right_pose_frame==902);
 // Outstanding LEFT main writers forbid an early LEFT snapshot, including a
 // mixed L/R batch. The native submission is not split or reordered.
 Fresh();CHECK(Record());auto&mixed=g_v6612_live_lists[0];mixed.list=&list;mixed.main_bind_mask=3;mixed.frame[0]=mixed.frame[1]=100;g_v6612_live_count=1;
 DesktopFinish();Submit();CHECK(DeviceHook::s_bridge_complete_pair_count==0);CHECK(!g_temporal_owner_d85.Pending());
 Fresh();CHECK(Record());ID3D12GraphicsCommandList leftPending;auto&lp=g_v6612_live_lists[0];lp.list=&leftPending;lp.main_bind_mask=1;lp.frame[0]=100;g_v6612_live_count=1;
 auto&rp=g_v6612_live_lists[1];rp.list=&list;rp.main_bind_mask=2;rp.frame[1]=100;g_v6612_live_count=2;
 DesktopFinish();Submit();CHECK(DeviceHook::s_bridge_complete_pair_count==0);
 // Successful Reset also retires the corresponding main-writer generation.
 CHECK(SUCCEEDED(DeviceHook::DetourResetCommandListD82(&leftPending,nullptr,nullptr)));CHECK(g_v6612_live_lists[0].main_bind_mask==0);
 // Execute the actual phase function: pending reservations block bootstrap
 // and live rollover, and a later successful submission releases that hold.
 Fresh();DeviceHook::SetV6610OutputCensusPhase(0,100);DeviceHook::SetV6610OutputCensusPhase(1,100);
 DeviceHook::SetV6610OutputCensusPhase(-1,100);CHECK(Record());const auto pending=g_temporal_owner_d85.Current();
 auto*pendingLeft=DeviceHook::s_bridge_left_capture;auto*pendingRight=DeviceHook::s_bridge_right_capture;
 DeviceHook::SetV6610OutputCensusPhase(0,101);CHECK(g_v6613_pair_frame==100);
 CHECK(g_temporal_owner_d85.Matches(pending));CHECK(DeviceHook::s_bridge_left_capture==pendingLeft&&DeviceHook::s_bridge_right_capture==pendingRight);
 DesktopFinish();g_v6615_last_left_main_batch=1;Submit();CHECK(DeviceHook::s_bridge_complete_pair_count==1);
 DeviceHook::SetV6610OutputCensusPhase(0,102);CHECK(g_v6613_pair_frame==102);CHECK(g_temporal_owner_d85.Current().frame==102);
 CHECK(DeviceHook::s_bridge_left_capture!=g_v6634_pub_left);CHECK(DeviceHook::s_bridge_right_capture!=g_v6634_pub_right);
 // A phase arriving while the REAL queue leaf is paused cannot invalidate a
 // claim or replace buffers before publication. No SDK mutex is used here.
 Fresh();DeviceHook::SetV6610OutputCensusPhase(0,100);DeviceHook::SetV6610OutputCensusPhase(1,100);
 DeviceHook::SetV6610OutputCensusPhase(-1,100);CHECK(Record());DesktopFinish();g_v6615_last_left_main_batch=1;
 pauseList=&list;pauseNative=true;nativeEntered=false;nativeRelease=false;
 std::atomic<bool> phaseStarted{false},phaseReturned{false};
 std::thread submitter([]{Submit();});while(!nativeEntered)std::this_thread::yield();
 std::thread nextPhase([&]{phaseStarted=true;DeviceHook::SetV6610OutputCensusPhase(0,101);phaseReturned=true;});
 while(!phaseStarted)std::this_thread::yield();
 std::this_thread::sleep_for(std::chrono::milliseconds(20));CHECK(!phaseReturned);
 nativeRelease=true;submitter.join();nextPhase.join();pauseNative=false;
 CHECK(phaseReturned);CHECK(DeviceHook::s_bridge_complete_pair_count==1);CHECK(g_v6613_pair_frame==101);
 // Startup without the optional scaling-blit composition receipt: SDK calls
 // may already be complete while the FIRST RIGHT batch still has only an
 // intermediate image. Keep LEFT now, then copy RIGHT only after its final draw.
 Fresh();ID3D12GraphicsCommandList earlyRight;earlyRight.Draw(&mainTarget,15);
 ID3D12CommandList* earlyLists[]{&earlyRight};
 auto& earlyTag=g_v6612_live_lists[0];earlyTag.list=&earlyRight;earlyTag.main_bind_mask=2;earlyTag.frame[1]=100;g_v6612_live_count=1;
 g_v6615_last_left_main_frame=100; // engine census has submitted LEFT
 DeviceHook::DetourExecuteCommandLists(&queue,1,earlyLists);
 g_v6612_live_count=0; // post-submit census archival is outside this adapter prefix
 CHECK(mainTarget.pixel==15);CHECK(g_temporal_owner_d85.Current().leftQueued);
 CHECK(DeviceHook::s_bridge_left_capture->pixel==10);
 CHECK(DeviceHook::s_bridge_complete_pair_serial==0);CHECK(!g_v6615_right_copy_queued);
 CHECK(Record());DesktopFinish();Submit();
 CHECK(DeviceHook::s_bridge_complete_pair_count==1);
 CHECK(DeviceHook::s_bridge_left_capture->pixel==10);CHECK(DeviceHook::s_bridge_right_capture->pixel==20);
 CHECK(mainTarget.pixel==99); // desktop HUD was never copied into either eye

 // These receipts observe the production capture boundary; they neither alter
 // admission nor treat SDK success as final-image evidence. F2 off accumulated
 // no payloads or per-lane diagnostic state during all cases above.
 CHECK(diagnostic_trace::captureRows.empty());
 for(const auto& rate:g_temporal_capture_trace_rate_d85)CHECK(rate.session==0&&rate.lastMs==0);
 TraceSession();Fresh();CHECK(Record());DesktopFinish();Submit();
 CHECK(HasTrace("stage=RECORDED","reason=final-main-copy-recorded"));
 CHECK(HasTrace("stage=PUBLISHED","reason=final-main-copy-submitted"));
 CHECK(g_v6634_pub_left->pixel==10&&g_v6634_pub_right->pixel==20&&mainTarget.pixel==99);
 CHECK(publishedOwner.epoch==7&&publishedOwner.technology==2);
 TraceSession();Fresh();g_temporal_owner_d85.Begin(7,100);
 g_temporal_owner_d85.NoteLeftBoundary(7,100,reinterpret_cast<std::uintptr_t>(&mainTarget),4,801);
 CHECK(!Record());CHECK(HasTrace("stage=RECORD_REFUSED","reason=right-not-complete"));
 CHECK(HasTrace("reason=right-not-complete","pairSdkReady=1"));
 TraceSession();Fresh();native_upscaler_stereo_d82::ready=false;
 CHECK(!Record());CHECK(HasTrace("stage=RECORD_REFUSED","reason=pair-sdk-receipt-missing"));
 TraceSession();Fresh();g_v6624_late_window=false;
 CHECK(!Record());CHECK(HasTrace("stage=RECORD_REFUSED","reason=late-window-closed"));
 TraceSession();Fresh();g_d82_temporal_hooks_ready=false;
 CHECK(!Record());CHECK(HasTrace("stage=RECORD_REFUSED","reason=capture-hooks-not-ready"));
 bool repeatedRefusals=true;for(unsigned i=0;i<120;++i)repeatedRefusals=!Record()&&repeatedRefusals;
 CHECK(repeatedRefusals);
 CHECK(diagnostic_trace::captureRows.size()==1); // no per-frame refusal stream
 fixtureClock+=1999;CHECK(!Record());CHECK(diagnostic_trace::captureRows.size()==1);
 ++fixtureClock;CHECK(!Record());CHECK(diagnostic_trace::captureRows.size()==2);
 TraceSession();Fresh();CHECK(Record());CHECK(!Record());
 CHECK(HasTrace("stage=RECORDED","reason=final-main-copy-recorded"));
 CHECK(HasTrace("stage=RECORD_REFUSED","reason=previous-recording-pending"));
 TraceSession();Fresh();CHECK(Record());incomingRight=false;DesktopFinish();Submit();
 CHECK(DeviceHook::s_bridge_complete_pair_count==0);
 CHECK(HasTrace("stage=SUBMIT_WITHHELD","reason=left-copy-missing"));
 CHECK(HasTrace("reason=left-copy-missing","pairSdkReady=1"));
 TraceSession();Fresh();CHECK(Record());native_upscaler_stereo_d82::ready=false;DesktopFinish();Submit();
 CHECK(DeviceHook::s_bridge_complete_pair_count==0);
 CHECK(HasTrace("stage=SUBMIT_WITHHELD","reason=mode-or-sdk-not-ready"));
 TraceSession();Fresh();CHECK(Record());queue.failFence=g_temporal_fence_d85;DesktopFinish();Submit();
 CHECK(DeviceHook::s_bridge_complete_pair_count==0);
 CHECK(HasTrace("stage=SUBMIT_WITHHELD","reason=capture-fence-signal-failed"));
 TraceSession();Fresh();CHECK(Record());auto staleReceipt=g_temporal_owner_d85.Current();++staleReceipt.recording;
 DeviceHook::FinishTemporalMainSubmissionD85(&queue,staleReceipt,88);
 CHECK(HasTrace("stage=SUBMIT_WITHHELD","reason=recording-receipt-mismatch"));
 CHECK(DeviceHook::s_bridge_complete_pair_count==0&&g_temporal_owner_d85.Pending());
 // Turning F2 off leaves an old session number; it still must stop payloads
 // and budget updates, including after the rate interval has elapsed.
 const auto ratesBeforeOff=g_temporal_capture_trace_rate_d85;
 diagnostics::capture168::enabled=false;diagnostic_trace::captureRows.clear();fixtureClock+=3000;
 Fresh();g_d82_temporal_hooks_ready=false;CHECK(!Record());
 CHECK(diagnostic_trace::captureRows.empty());
 for(unsigned i=0;i<ratesBeforeOff.size();++i)
  CHECK(g_temporal_capture_trace_rate_d85[i].session==ratesBeforeOff[i].session&&
        g_temporal_capture_trace_rate_d85[i].lastMs==ratesBeforeOff[i].lastMs);
 std::printf("Temporal production adapter checks=%d GPU-leaf-order-checks=%u failures=%d\n",checks,gpuChecks,failures);return failures?1:0;
}
