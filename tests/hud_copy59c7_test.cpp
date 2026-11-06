
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cassert>
#include <mutex>
#include <vector>
using UINT=unsigned;using UINT64=std::uint64_t;using HRESULT=int;
using XrSession=std::uint64_t;using XrSwapchain=std::uint64_t;
constexpr int XR_NULL_HANDLE=0,XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO=1,XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO=2,
 XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO=3,XR_TIMEOUT_EXPIRED=1;
inline bool XR_FAILED(int r){return r<0;}inline bool SUCCEEDED(int r){return r>=0;}
constexpr int D3D12_RESOURCE_STATE_COMMON=0,D3D12_RESOURCE_STATE_COPY_SOURCE=1,
 D3D12_RESOURCE_STATE_RENDER_TARGET=2,D3D12_RESOURCE_STATE_COPY_DEST=3;
struct Desc {UINT64 Width=1920;UINT Height=1800;int Format=28;};
struct ID3D12Resource {Desc d{};int refs=1;Desc GetDesc(){return d;}void AddRef(){++refs;}void Release(){--refs;}};
struct ID3D12Device {};
extern int resetResult;struct Alloc {HRESULT Reset(){return resetResult;}};
struct ID3D12CommandList {};
int waitResult=0,releaseResult=0,acquireResult=0,resetResult=0,signalResult=0;bool submitResult=true;
unsigned copies=0,submits=0,acquires=0,releases=0,waits=0;
struct List:ID3D12CommandList{HRESULT Reset(Alloc*,void*){return 0;}HRESULT Close(){return 0;}
 void CopyResource(ID3D12Resource*,ID3D12Resource*){++copies;}};
struct Fence {UINT64 done=0;UINT64 GetCompletedValue(){return done;}};
struct ID3D12CommandQueue {HRESULT Signal(Fence*,UINT64){return signalResult;}};
struct XrHudFrame {bool ok=false;XrSwapchain swapchain=0;int width=0,height=0;};
struct XrSwapchainImageWaitInfo {int type;std::int64_t timeout=0;};
struct XrSwapchainImageReleaseInfo {int type;};
struct XrSwapchainImageAcquireInfo {int type;};
int xrWaitSwapchainImage(XrSwapchain,XrSwapchainImageWaitInfo*){++waits;return waitResult;}
int xrReleaseSwapchainImage(XrSwapchain,XrSwapchainImageReleaseInfo*){++releases;return releaseResult;}
int xrAcquireSwapchainImage(XrSwapchain,XrSwapchainImageAcquireInfo*,unsigned*i){++acquires;*i=0;return acquireResult;}
std::uint64_t tick=1000;
std::uint64_t GetTickCount64(){return tick;}
void Barrier(List*,ID3D12Resource*,int,int){}
namespace DeviceHook{bool ExecuteCommandListsBypass(ID3D12CommandQueue*,unsigned,ID3D12CommandList**){++submits;return submitResult;}}
std::atomic<bool> g_enabled{true};
bool x_failed=false,x_has_released=false,x_acquired=false;
XrSwapchain x_sc=5;std::uint64_t x_released_ms=0,x_fv=0,g_pub_ms=1000,g_pub_serial=1;
int x_w=1920,x_h=1800,x_fmt=28;unsigned x_index=0;
unsigned s_busy_skips=0,s_wait_timeouts=0,s_xr_copy_fail=0,s_age_expired162r1=0,s_xr_shown=0;
ID3D12Resource cache{},image{};ID3D12Resource*g_pub_cache=&cache,*x_inflight_source=nullptr;
Fence fence{};Fence*x_fence=&fence;Alloc allocator{};Alloc*x_alloc=&allocator;List list{};List*x_list=&list;
std::vector<ID3D12Resource*> imgs{&image};auto*x_imgs=&imgs;
std::mutex g_pub_mutex;
std::atomic<std::uint64_t>g_source_epoch59c7{1};std::uint64_t g_pub_epoch59c7=1;
std::uint64_t x_released_serial59c7=0,x_released_epoch59c7=0;unsigned s_same_reused59c7=0;
XrSession x_session59c7=1;ID3D12Device*x_device59c7=nullptr;ID3D12CommandQueue*x_queue59c7=nullptr;
bool XrEnsure(XrSession s,ID3D12Device*d,const Desc&c){
 if(x_w!=int(c.Width)||x_h!=int(c.Height)||x_fmt!=c.Format||x_sc==XR_NULL_HANDLE){
  x_has_released=false;x_released_serial59c7=x_released_epoch59c7=0;++x_sc;
  x_w=int(c.Width);x_h=int(c.Height);x_fmt=c.Format;
 }
 x_session59c7=s;x_device59c7=d;return true;
}

#include "generated_hud_copy59c7.inc"
int main(){
 ID3D12Device device,otherDevice;ID3D12CommandQueue queue,otherQueue;x_device59c7=&device;
 auto ready=[&](){fence.done=x_fv;};
 for(int i=0;i<10;++i){tick=1000+11*i;ready();assert(XrPrepare(1,&device,&queue).ok);}
 assert(copies==1&&submits==1&&acquires==1&&releases==1&&s_same_reused59c7==9);
 puts("PASS one publication / ten uses: one copy-submit-acquire-release, nine reuses");
 ++g_pub_serial;g_pub_ms=tick;ready();assert(XrPrepare(1,&device,&queue).ok&&copies==2);
 auto n=copies;++g_pub_serial;g_pub_ms=tick;fence.done=0;
 assert(XrPrepare(1,&device,&queue).ok&&copies==n);ready();
 waitResult=XR_TIMEOUT_EXPIRED;assert(XrPrepare(1,&device,&queue).ok&&x_acquired);auto a=acquires,r=releases;
 assert(XrPrepare(1,&device,&queue).ok&&acquires==a&&releases==r&&copies==n);
 waitResult=0;assert(XrPrepare(1,&device,&queue).ok&&copies==n+1&&!x_acquired);
 ready();n=copies;assert(!XrPrepare(2,&device,&queue).ok&&copies==n);
 assert(!XrPrepare(1,&otherDevice,&queue).ok&&copies==n);
 assert(!XrPrepare(1,&device,&otherQueue).ok&&copies==n);
 tick=g_pub_ms+251;assert(!XrPrepare(1,&device,&queue).ok&&copies==n);
 ++g_pub_serial;g_pub_ms=tick;assert(XrPrepare(1,&device,&queue).ok&&copies==n+1);ready();
 ++g_source_epoch59c7;n=copies;assert(!XrPrepare(1,&device,&queue).ok&&copies==n);
 g_pub_epoch59c7=g_source_epoch59c7.load();++g_pub_serial;g_pub_ms=tick;
 assert(XrPrepare(1,&device,&queue).ok&&copies==n+1);ready();
 auto serial=x_released_serial59c7;++g_pub_serial;g_pub_ms=tick;releaseResult=-1;
 assert(!XrPrepare(1,&device,&queue).ok&&x_failed&&!x_has_released&&x_released_serial59c7==serial);
 x_failed=false;releaseResult=0;ready();assert(XrPrepare(1,&device,&queue).ok);ready();
 ++g_pub_serial;g_pub_ms=tick;resetResult=-1;assert(!XrPrepare(1,&device,&queue).ok&&!x_has_released);
 resetResult=0;ready();assert(XrPrepare(1,&device,&queue).ok);ready();
 n=copies;cache.d.Width=1024;assert(XrPrepare(1,&device,&queue).ok&&copies==n+1&&x_w==1024);ready();
 // Backward clock cannot mint freshness or reuse cached content.
 tick=g_pub_ms-1;assert(!XrPrepare(1,&device,&queue).ok);
 puts("PASS new serial, busy fence, timeout ownership, failed release/reset, session/device/queue, epoch, resize and source age");
}
