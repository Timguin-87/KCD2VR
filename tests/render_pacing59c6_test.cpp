#include "core/render_pacing59c6.h"
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <thread>
struct ID3D12Resource{};struct ID3D12CommandQueue{};
namespace kcd2vr {
namespace debug_trace {inline void Write(const char*){}inline void WriteCounter(const char*,std::uint64_t){}}
inline ID3D12Resource* g_v6634_pub_left=nullptr;inline ID3D12Resource* g_v6634_pub_right=nullptr;
inline std::uint64_t g_v6634_pub_serial=0;
struct DeviceHook {
 inline static std::mutex s_bridge_capture_mutex;
 inline static std::atomic<std::uint64_t> s_bridge_complete_pair_serial{0},s_bridge_complete_pair_camera_frame{0},consumed{0};
 inline static bool presentKnown=true,presentSubmitted=false;
 inline static std::uint64_t present=0,presentCamera=0;inline static int presentReads=0;
 static bool ReadBridgePairIdentity59c6(std::uint64_t&,std::uint64_t&);
 static std::uint64_t V6634LastConsumedPairSerial(){return consumed.load();}
 static bool CurrentExecuteSubmittedPresent(){return presentSubmitted;}
 static bool GetLatestPresentIdentity(std::uint64_t*s,std::uint64_t*t,std::uint64_t*w,std::uint32_t*h,std::uint64_t*c){++presentReads;*s=present;*t=1000;*w=100;*h=100;*c=presentCamera;return presentKnown;}
};
struct XrManager {
 struct FramePresentation59c2{struct{int stereo=2;}policy;std::uint64_t ticket=3;};
 struct Production{bool current=true;bool Current(std::uint64_t){return current;}}production_presentation59c2_;
 FramePresentation59c2 render_request_presentation59c2_{},render_presentation59c2_{};
 inline static ID3D12CommandQueue queue{};
 ID3D12CommandQueue* d3d12_queue_=&queue;
 std::atomic<std::uint64_t> render_tick_count_{0},last_submitted_present_serial_{0},last_submitted_desktop_serial59c6_{0},expected_present_camera_frame_{0};
 std::atomic<std::uint64_t> present_pacing_fresh_count_{0},present_pacing_reuse_count_{0},present_pacing_last_wait_ms_{0},present_pacing_last_serial_{0};
 std::atomic<bool> diagnostic_rendering_{false},active_vr_bridge_{true};std::atomic<int> xr_render_gate_mode_{0},present_pacing_mode_{0};std::atomic<float> estimated_refresh_hz_{90};
 std::mutex render_mutex_;std::condition_variable render_cv_;
 bool render_request_pending_=true,render_request_in_progress_=false,render_request_done_=false,render_request_success_=false,render_request_copy_ui_=false,render_request_ui_copy_success_=false;
 ID3D12Resource* render_request_images_[2]{};ID3D12Resource* render_request_ui_image_=nullptr;std::atomic<ID3D12Resource*> menu_quad_source_{nullptr};
 std::uint64_t render_request_enqueue_ns59c6_=pacing59c6::NowNs();
 int copies=0,uiCopies=0;bool copyResult=true;
 bool SubmitBridgeImagesOnRenderThread(ID3D12Resource*,ID3D12Resource*){++copies;return copyResult;}
 bool SubmitDiagnosticImagesOnRenderThread(ID3D12Resource*,ID3D12Resource*){++copies;return copyResult;}
 bool CopyMenuTextureToUiImage(ID3D12Resource*,ID3D12Resource*){++uiCopies;return true;}
 void OnRenderThreadTick(ID3D12CommandQueue*);
};
#include "generated_render_pacing59c6.inc"
}
static unsigned n=0,f=0;static void check(bool ok,const char* s){++n;f+=!ok;std::printf("%s %s\n",ok?"PASS":"FAIL",s);}
int main(){using namespace kcd2vr;using namespace pacing59c6;
 const std::uint64_t pairA=(std::uint64_t(800)<<32)^6000,pairB=(std::uint64_t(801)<<32)^6010;
 check(!(std::uint64_t(200)>pairA),"old comparison: desktop counter is not greater than packed pair token");
 Input p{};p.paired=p.known=p.have=true;p.serial=pairB;p.last=pairA;p.camera=80;p.nowNs=10000000;p.enqueuedNs=9000000;
 check(Decide(p).result==Result::Fresh,"new certified pair eligible before desktop PRESENT");
 p.serial=99;check(Decide(p).fresh,"opaque pair inequality allows distinct numerically smaller token");
 p.serial=p.last;p.nowNs=12000000;check(Decide(p).result==Result::WaitGrace,"consumed pair waits original 4ms default grace");
 p.nowNs=13000000;check(Decide(p).result==Result::Reuse,"grace ends at actual 4ms without waiting 15.6ms clock tick");
 p.nowNs=p.enqueuedNs-1;check(Decide(p).result==Result::WaitClock,"reversed clock cannot justify cache reuse");
 p.known=false;check(Decide(p).result==Result::WaitIdentity,"busy metadata is not no-pair/clearance");
 p={};p.known=p.have=true;p.serial=12;p.last=11;p.nowNs=50000000;p.enqueuedNs=1;
 check(Decide(p).result==Result::WaitFuture,"unsubmitted desktop resource blocked after grace");
 p.submitted=true;check(Decide(p).fresh,"submitted desktop PRESENT uses its own last counter");
 p.gate=2;p.expected=15;p.camera=14;check(Decide(p).result==Result::WaitStrict,"strict comparator retains expected-camera exclusion");
 p.camera=15;check(Decide(p).fresh,"strict equal frame accepted");p.camera=16;check(Decide(p).fresh,"existing strict forward acceptance retained");
 check(Paired(2)&&Paired(6)&&Paired(7)&&!Paired(0)&&!Paired(1),"native pair scope excludes theater and AFR controls");
 ID3D12Resource l,r;g_v6634_pub_left=&l;g_v6634_pub_right=&r;g_v6634_pub_serial=pairB;
 DeviceHook::s_bridge_complete_pair_serial=pairB;DeviceHook::s_bridge_complete_pair_camera_frame=80;DeviceHook::consumed=pairA;
 std::uint64_t token=5,camera=5;check(DeviceHook::ReadBridgePairIdentity59c6(token,camera)&&token==pairB&&camera==80,"actual peek reads coherent published identity without claim");
 g_v6634_pub_serial=pairA;check(DeviceHook::ReadBridgePairIdentity59c6(token,camera)&&token==0&&camera==0,"mismatched publication cannot advertise pair readiness");g_v6634_pub_serial=pairB;
 {
  std::atomic<bool> locked{false},release{false};std::thread t([&]{std::lock_guard<std::mutex> lock(DeviceHook::s_bridge_capture_mutex);locked=true;while(!release)std::this_thread::yield();});while(!locked)std::this_thread::yield();
  check(!DeviceHook::ReadBridgePairIdentity59c6(token,camera)&&!token&&!camera,"actual peek cannot block behind busy producer");release=true;t.join();
 }
 DeviceHook::present=200;DeviceHook::presentSubmitted=false;DeviceHook::presentReads=0;
 XrManager m;m.last_submitted_present_serial_=pairA;m.OnRenderThreadTick(m.d3d12_queue_);
 check(m.copies==1&&m.render_request_done_&&m.render_request_success_&&!DeviceHook::presentReads,"actual handoff takes fresh pair without desktop lookup");
 check(m.render_presentation59c2_.ticket==3&&m.render_presentation59c2_.policy.stereo==2,"same request frame/layer policy reaches copy");
 DeviceHook::consumed=pairB;XrManager reuse;reuse.render_request_enqueue_ns59c6_=NowNs()-5000000;reuse.OnRenderThreadTick(reuse.d3d12_queue_);
 check(reuse.copies==1&&reuse.present_pacing_reuse_count_==1,"actual handoff releases expired high-resolution grace");
 XrManager obsolete;obsolete.production_presentation59c2_.current=false;obsolete.OnRenderThreadTick(obsolete.d3d12_queue_);
 check(obsolete.copies==0&&obsolete.render_request_done_&&!obsolete.render_request_pending_,"obsolete menu generation cancels before copy");
 XrManager desktop;desktop.render_request_presentation59c2_.policy.stereo=0;desktop.last_submitted_present_serial_=pairB;desktop.last_submitted_desktop_serial59c6_=199;DeviceHook::presentSubmitted=true;desktop.OnRenderThreadTick(desktop.d3d12_queue_);
 check(desktop.copies==1&&desktop.present_pacing_fresh_count_==1,"theater entry not gated by previous packed pair token");
 XrManager future;future.render_request_presentation59c2_.policy.stereo=0;future.last_submitted_desktop_serial59c6_=199;future.render_request_enqueue_ns59c6_=NowNs()-50000000;DeviceHook::presentSubmitted=false;future.OnRenderThreadTick(future.d3d12_queue_);
 check(future.copies==0&&future.render_request_pending_,"actual desktop future-PRESENT protection retained");
 DeviceHook::consumed=pairA;XrManager strict;strict.xr_render_gate_mode_=2;strict.expected_present_camera_frame_=81;strict.OnRenderThreadTick(strict.d3d12_queue_);
 check(strict.copies==0,"actual strict pair gate keeps camera identity checks");
 XrManager failed;failed.copyResult=false;failed.OnRenderThreadTick(failed.d3d12_queue_);check(failed.copies==1&&failed.render_request_done_&&!failed.render_request_success_,"copy failure completes false rather than replaying work");
 XrManager other;ID3D12CommandQueue wrong;other.OnRenderThreadTick(&wrong);check(other.copies==0,"unrelated queue cannot consume images");
 std::printf("render_pacing59c6=%u/%u GPU_AND_RUNTIME=MOCKED\n",n-f,n);return f?1:0;
}
