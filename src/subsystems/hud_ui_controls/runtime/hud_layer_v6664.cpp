#include "hud_layer_v6664.h"
#include "xr_eye_srgb_r114.h" // R114
#include "ui_hud_mirror36.h"
#include "device_hook.h"
#include "diagnostic_trace.h"
#include <atomic>
#include <mutex>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>
#include <new>

namespace kcd2vr::hud_layer_v6664 { namespace {

std::atomic<bool> g_enabled{true};
// Publication identity includes capture lifetime; disabling/recreating cannot revive old pixels.
std::atomic<std::uint64_t> g_source_epoch59c7{1};
std::atomic<std::uint64_t> s_same_reused59c7{0};
std::atomic<bool> g_mirror_enabled36{false};
Mirror36* g_mirror36=nullptr;
bool g_mirror_attempted36=false;
ID3D12GraphicsCommandList* g_mirror_list36=nullptr;
ID3D12Resource* g_mirror_main36=nullptr;
D3D12_CPU_DESCRIPTOR_HANDLE g_desktop_rtv36{};
std::atomic<std::uint64_t> g_mirror_draws36{0};

// ---------------- render-thread state (single render thread) ----------------
ID3D12Resource* g_rt = nullptr;              // redirect target, lives in RENDER_TARGET
ID3D12Resource* g_cache[2] = {nullptr,nullptr}; // live in COMMON between uses
ID3D12DescriptorHeap* g_heap = nullptr;
D3D12_CPU_DESCRIPTOR_HANDLE g_rtv{};
UINT64 g_w = 0; UINT g_h = 0; DXGI_FORMAT g_fmt = DXGI_FORMAT_UNKNOWN;
bool g_create_failed = false;

std::mutex g_rt_mutex;                       // guards the render-side state below
bool g_frame_cleared = false;                // a redirect happened in this late window
std::uint32_t g_frame = 0;
int g_next_cache = 0;
struct PendingCopy { ID3D12GraphicsCommandList* list=nullptr; ID3D12Resource* cache=nullptr; std::uint64_t order=0, epoch=0; };
PendingCopy g_pending[2]{};std::uint64_t g_record_order=0;
std::atomic<bool> g_pending_any{false};
std::atomic<std::uint64_t> s_busy_skips{0},s_wait_timeouts{0},s_age_expired162r1{0};

// published for the XR thread
std::mutex g_pub_mutex;
ID3D12Resource* g_pub_cache = nullptr;       // AddRef'd
std::uint64_t g_pub_serial = 0;
std::uint64_t g_pub_ms = 0;
std::uint64_t g_pub_epoch59c7 = 0;

// stats (5 s summary)
std::atomic<std::uint64_t> s_windows{0}, s_redirected_windows{0}, s_binds{0}, s_draws{0}, s_indexed{0}, s_inst{0},
    s_small_inst{0}, s_published{0}, s_xr_shown{0}, s_xr_copy_fail{0};
std::atomic<std::uint32_t> s_hist[8]{};      // distinct draw counts seen (first 8)
std::atomic<std::uint32_t> s_hist_n[8]{};
// R111: a late window whose redirected lists received NO draw is not a HUD frame (R110 SteamVR: "HUD flickers a
// lot" while the layer was submitted every frame = the content alternated). Such a capture is neither copied nor
// published; the XR layer keeps the last capture that had draws. Draws are counted only on the redirected lists.
std::atomic<ID3D12GraphicsCommandList*> g_redirect_lists_r111[4]{};
std::atomic<std::uint32_t> g_window_draws_r111{0};
std::atomic<std::uint64_t> s_no_draw_skips_r111{0},s_recopies_r111{0};
std::uint64_t g_last_log_ms = 0;

void Log(const char* m) noexcept { diagnostic_trace::Write(m); }
// R104: HUD swapchain create/fail/retry rows are always in KCD2VR.log (max 40 per run) - under SteamVR no HUD was
// shown and nothing said why.
std::atomic<unsigned> s_status_rows_r104{0};
std::atomic<std::uint64_t> s_fail_events_r104{0},s_retries_r104{0};
void LogStatusR104(const char* m) noexcept { if(s_status_rows_r104.fetch_add(1,std::memory_order_relaxed)<40u)diagnostic_trace::WriteStatus(m); }

void ReleaseAll() noexcept {
    // Called only on size change; GPU may still reference the old objects for a frame or two,
    // so they are intentionally leaked (rare: resolution change) instead of released.
    for(auto& p:g_pending)p={};g_pending_any.store(false);
    g_rt=nullptr; g_cache[0]=g_cache[1]=nullptr; g_heap=nullptr;
    g_mirror36=nullptr;g_mirror_attempted36=false;g_mirror_main36=nullptr;g_mirror_list36=nullptr;
}

bool Ensure(ID3D12Device* dev, const D3D12_RESOURCE_DESC& md) noexcept {
    if(g_rt && g_w==md.Width && g_h==md.Height && g_fmt==md.Format) return true;
    if(g_create_failed || !dev) return false;
    if(g_rt) { ReleaseAll(); Log("V66.64-HUD backbuffer size/format changed -> recreating HUD textures"); }
    D3D12_HEAP_PROPERTIES hp{}; hp.Type=D3D12_HEAP_TYPE_DEFAULT;
    D3D12_RESOURCE_DESC d{}; d.Dimension=D3D12_RESOURCE_DIMENSION_TEXTURE2D; d.Width=md.Width; d.Height=md.Height;
    d.DepthOrArraySize=1; d.MipLevels=1; d.Format=md.Format; d.SampleDesc.Count=1; d.Layout=D3D12_TEXTURE_LAYOUT_UNKNOWN;
    d.Flags=D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    D3D12_CLEAR_VALUE cv{}; cv.Format=md.Format;
    ID3D12Resource* rt=nullptr; ID3D12Resource* c0=nullptr; ID3D12Resource* c1=nullptr; ID3D12DescriptorHeap* heap=nullptr;
    bool ok=SUCCEEDED(dev->CreateCommittedResource(&hp,D3D12_HEAP_FLAG_NONE,&d,D3D12_RESOURCE_STATE_RENDER_TARGET,&cv,IID_PPV_ARGS(&rt)));
    D3D12_RESOURCE_DESC dc=d; dc.Flags=D3D12_RESOURCE_FLAG_NONE;
    ok=ok&&SUCCEEDED(dev->CreateCommittedResource(&hp,D3D12_HEAP_FLAG_NONE,&dc,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&c0)));
    ok=ok&&SUCCEEDED(dev->CreateCommittedResource(&hp,D3D12_HEAP_FLAG_NONE,&dc,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&c1)));
    D3D12_DESCRIPTOR_HEAP_DESC hd{}; hd.Type=D3D12_DESCRIPTOR_HEAP_TYPE_RTV; hd.NumDescriptors=1;
    ok=ok&&SUCCEEDED(dev->CreateDescriptorHeap(&hd,IID_PPV_ARGS(&heap)));
    if(!ok) {
        if(rt) rt->Release();
        if(c0) c0->Release();
        if(c1) c1->Release();
        if(heap) heap->Release();
        g_create_failed=true; Log("V66.64-HUD create FAILED (HUD layer disabled for this session)"); return false;
    }
    g_rtv=heap->GetCPUDescriptorHandleForHeapStart();
    dev->CreateRenderTargetView(rt,nullptr,g_rtv);
    rt->SetName(L"KCD2VR_HUD_RT"); c0->SetName(L"KCD2VR_HUD_CACHE0"); c1->SetName(L"KCD2VR_HUD_CACHE1");
    g_source_epoch59c7.fetch_add(1,std::memory_order_acq_rel);
    g_rt=rt; g_cache[0]=c0; g_cache[1]=c1; g_heap=heap; g_w=md.Width; g_h=md.Height; g_fmt=md.Format;
    char m[200]{}; std::snprintf(m,sizeof(m),"V66.64-HUD textures ready %llux%u fmt=%u (RT + 2 cache)",(unsigned long long)g_w,g_h,(unsigned)g_fmt); Log(m);
    return true;
}

void NoteCount(UINT c) noexcept {
    for(int i=0;i<8;++i){ const auto v=s_hist[i].load(std::memory_order_relaxed);
        if(v==c){ s_hist_n[i].fetch_add(1,std::memory_order_relaxed); return; }
        if(v==0){ std::uint32_t z=0; if(s_hist[i].compare_exchange_strong(z,c)){ s_hist_n[i].fetch_add(1,std::memory_order_relaxed); return; } if(s_hist[i].load()==c){ s_hist_n[i].fetch_add(1); return; } } }
}

void MaybeSummary() noexcept {
    // UI88: no periodic file writes from a render callback. Use the manual UI marker.
    constexpr bool emitPeriodicSummary=false;
    if constexpr(!emitPeriodicSummary)return;
    const auto now=GetTickCount64();
    if(!g_last_log_ms){ g_last_log_ms=now; return; }
    if(now-g_last_log_ms<5000) return;
    g_last_log_ms=now;
    char m[900]{}; int n=std::snprintf(m,sizeof(m),
        "V66.64-HUD 5s enabled=%d lateWindows=%llu redirectedWindows=%llu binds=%llu draws=%llu (indexed=%llu instanced=%llu tinyInstanced<=4v=%llu) published=%llu xrShown=%llu xrCopyFail=%llu counts{",
        g_enabled.load()?1:0,(unsigned long long)s_windows.load(),(unsigned long long)s_redirected_windows.load(),(unsigned long long)s_binds.load(),
        (unsigned long long)s_draws.load(),(unsigned long long)s_indexed.load(),(unsigned long long)s_inst.load(),(unsigned long long)s_small_inst.load(),
        (unsigned long long)s_published.load(),(unsigned long long)s_xr_shown.load(),(unsigned long long)s_xr_copy_fail.load());
    for(int i=0;i<8 && n<int(sizeof(m))-40;++i){ const auto v=s_hist[i].load(); if(!v) break;
        n+=std::snprintf(m+n,sizeof(m)-n,"%s%s%u x%u",i?" ":"",(v&0x80000000u)?"verts":"idx",v&0x7FFFFFFFu,s_hist_n[i].load()); }
    if(n<int(sizeof(m))-4) std::snprintf(m+n,sizeof(m)-n,"}");
    Log(m);
}

// ---------------- XR thread state ----------------
XrSwapchain x_sc = XR_NULL_HANDLE;
std::vector<ID3D12Resource*>* x_imgs = nullptr;
int32_t x_w=0, x_h=0; DXGI_FORMAT x_fmt=DXGI_FORMAT_UNKNOWN; bool x_failed=false; std::uint64_t x_fail_ms_r104=0;
ID3D12CommandAllocator* x_alloc=nullptr; ID3D12GraphicsCommandList* x_list=nullptr; ID3D12Fence* x_fence=nullptr; UINT64 x_fv=0; HANDLE x_event=nullptr;
// R111: the HUD swapchain gets a fresh image EVERY XR frame (copy of the newest capture, like the eye swapchains)
// instead of re-showing the last released image on frames without a new capture. Copies wait behind the game's GPU
// work on the same queue, so 3 allocators (slot 0 = x_alloc) let a new copy start while older ones still run.
ID3D12CommandAllocator* x_alloc_more_r111[2]={nullptr,nullptr};
UINT64 x_slot_fv_r111[3]={0,0,0}; ID3D12Resource* x_slot_src_r111[3]={nullptr,nullptr,nullptr}; unsigned x_slot_r111=0;
ID3D12CommandAllocator* SlotAllocR111(unsigned i) noexcept { return i==0?x_alloc:x_alloc_more_r111[i-1]; }
void ReleaseSlotSourcesR111() noexcept { for(auto& r:x_slot_src_r111)if(r){r->Release();r=nullptr;} }
void ReleaseMoreAllocsR111() noexcept { for(auto& a:x_alloc_more_r111)if(a){a->Release();a=nullptr;} for(auto& v:x_slot_fv_r111)v=0; x_slot_r111=0; }
bool x_acquired=false,x_has_released=false;uint32_t x_index=0;std::uint64_t x_released_ms=0;
XrSession x_session59c7=XR_NULL_HANDLE;
ID3D12Device* x_device59c7=nullptr;ID3D12CommandQueue* x_queue59c7=nullptr;
std::uint64_t x_released_serial59c7=0,x_released_epoch59c7=0;
bool XrEnsure(XrSession s, ID3D12Device* dev, const D3D12_RESOURCE_DESC& cd) noexcept {
    if(x_sc!=XR_NULL_HANDLE && x_w==int32_t(cd.Width) && x_h==int32_t(cd.Height) && x_fmt==cd.Format) return true;
    if(x_failed) return false;
    if(x_acquired || (x_fence && x_fence->GetCompletedValue()<x_fv))return false;
    if(x_sc!=XR_NULL_HANDLE){ xrDestroySwapchain(x_sc); x_sc=XR_NULL_HANDLE; if(x_imgs) x_imgs->clear(); x_has_released=false; }
    if(!x_alloc){
        bool ok=SUCCEEDED(dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&x_alloc)));
        ok=ok&&SUCCEEDED(dev->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,x_alloc,nullptr,IID_PPV_ARGS(&x_list)));
        if(ok) { x_list->Close(); DeviceHook::SetInternalHudCommandList(x_list); }
        ok=ok&&SUCCEEDED(dev->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&x_fence)));
        x_event=CreateEventW(nullptr,FALSE,FALSE,nullptr);
        if(!ok || !x_event){ x_failed=true; LogStatusR104("V66.64-HUD XR command objects FAILED (R104: retried in 2 s)"); return false; }
    }
    // R108: the HUD copy is a CopyResource from the game's HUD texture (its format, DXGI 28 on his PC) into the
    // swapchain image; CopyResource allows the UNORM <-> UNORM_SRGB sibling of the same family. SteamVR (Meta
    // compatibility mode) refused 28 with -26 (FORMAT_UNSUPPORTED) every 2 s for the whole run = no HUD. Take the
    // texture's own format when the runtime lists it, else its sibling, else say which formats exist.
    int64_t fmtR108=static_cast<int64_t>(cd.Format);
    {
        uint32_t n=0;std::vector<int64_t> formats;
        if(XR_SUCCEEDED(xrEnumerateSwapchainFormats(s,0,&n,nullptr))&&n){formats.resize(n);if(XR_FAILED(xrEnumerateSwapchainFormats(s,n,&n,formats.data())))formats.clear();}
        auto has=[&](int64_t f){return std::find(formats.begin(),formats.end(),f)!=formats.end();};
        auto sibling=[](DXGI_FORMAT f)->int64_t{switch(f){
            case DXGI_FORMAT_R8G8B8A8_UNORM:return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:return DXGI_FORMAT_R8G8B8A8_UNORM;
            case DXGI_FORMAT_B8G8R8A8_UNORM:return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:return DXGI_FORMAT_B8G8R8A8_UNORM;
            case DXGI_FORMAT_R10G10B10A2_UNORM:return DXGI_FORMAT_R10G10B10A2_UNORM;default:return 0;}};
        // R114 (VrEyeSrgbR114): the HUD image is sRGB-encoded like the eyes; when the runtime offers the sRGB
        // sibling of the HUD texture format use it (bytes copied unchanged, the compositor decodes them) - on
        // VDXR the UNORM HUD was shown too bright/washed like the eyes.
        if(kcd2vr::xr_eye_srgb_r114::eyeSrgb.load(std::memory_order_acquire)&&!formats.empty()&&
           (cd.Format==DXGI_FORMAT_R8G8B8A8_UNORM||cd.Format==DXGI_FORMAT_B8G8R8A8_UNORM)&&has(sibling(cd.Format)))fmtR108=sibling(cd.Format);
        if(!formats.empty()&&!has(fmtR108)){
            const int64_t alt=sibling(cd.Format);
            if(alt&&has(alt)){char m[200]{};std::snprintf(m,sizeof(m),"R108 HUD_XR texture fmt=%u not offered by the runtime; using sibling %lld (same family, CopyResource ok)",(unsigned)cd.Format,(long long)alt);LogStatusR104(m);fmtR108=alt;}
            else{char m[300]{};int at=std::snprintf(m,sizeof(m),"R108 HUD_XR texture fmt=%u and its sibling are not offered; runtime formats:",(unsigned)cd.Format);
                for(size_t i=0;i<formats.size()&&at<int(sizeof(m))-12;++i)at+=std::snprintf(m+at,sizeof(m)-at," %lld",(long long)formats[i]);LogStatusR104(m);}
        }
    }
    XrSwapchainCreateInfo ci{XR_TYPE_SWAPCHAIN_CREATE_INFO};
    ci.usageFlags=XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT|XR_SWAPCHAIN_USAGE_SAMPLED_BIT|XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT;
    ci.format=fmtR108; ci.sampleCount=1; ci.width=uint32_t(cd.Width); ci.height=cd.Height; ci.faceCount=1; ci.arraySize=1; ci.mipCount=1;
    const XrResult createResultR105=xrCreateSwapchain(s,&ci,&x_sc);
    if(XR_FAILED(createResultR105)){ x_sc=XR_NULL_HANDLE; x_failed=true;
        char m[200]{}; std::snprintf(m,sizeof(m),"R105 HUD_XR swapchain create FAILED result=%d fmt=%u %llux%u retryMs=2000",int(createResultR105),(unsigned)cd.Format,(unsigned long long)cd.Width,cd.Height); LogStatusR104(m); return false; }
    uint32_t n=0; xrEnumerateSwapchainImages(x_sc,0,&n,nullptr);
    std::vector<XrSwapchainImageD3D12KHR> im(n); for(auto& i:im) i.type=XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR;
    if(!n || XR_FAILED(xrEnumerateSwapchainImages(x_sc,n,&n,reinterpret_cast<XrSwapchainImageBaseHeader*>(im.data())))){
        xrDestroySwapchain(x_sc); x_sc=XR_NULL_HANDLE; x_failed=true; LogStatusR104("V66.64-HUD XR swapchain images FAILED (R104: retried in 2 s)"); return false; }
    if(!x_imgs) x_imgs=new std::vector<ID3D12Resource*>();
    x_imgs->clear(); for(auto& i:im) x_imgs->push_back(i.texture);
    x_w=int32_t(cd.Width); x_h=int32_t(cd.Height); x_fmt=cd.Format;
    x_session59c7=s;x_device59c7=dev;x_has_released=false;
    x_released_serial59c7=x_released_epoch59c7=0;
    char m[200]{}; std::snprintf(m,sizeof(m),"V66.64-HUD XR swapchain ready %dx%d fmt=%u swapchainFmt=%lld images=%u readback=OFF",x_w,x_h,(unsigned)x_fmt,(long long)fmtR108,n); LogStatusR104(m);
    return true;
}

void Barrier(ID3D12GraphicsCommandList* l, ID3D12Resource* r, D3D12_RESOURCE_STATES a, D3D12_RESOURCE_STATES b) noexcept {
    D3D12_RESOURCE_BARRIER x{}; x.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; x.Transition.pResource=r;
    x.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; x.Transition.StateBefore=a; x.Transition.StateAfter=b; l->ResourceBarrier(1,&x);
}


} // namespace

void SetDesktopMirror36(bool on) noexcept {g_mirror_enabled36.store(on,std::memory_order_release);}
void SetEnabled(bool on) noexcept {
    if(g_enabled.exchange(on,std::memory_order_acq_rel)!=on)
        g_source_epoch59c7.fetch_add(1,std::memory_order_acq_rel);
}
bool Enabled() noexcept { return g_enabled.load(std::memory_order_acquire); }
void WriteSnapshot() noexcept {
    char m[512]{};std::snprintf(m,sizeof(m),
        "V66.88-HUD-SNAPSHOT enabled=%d windows=%llu redirects=%llu draws=%llu published=%llu xrShown=%llu copyFailures=%llu periodicReadback=OFF gpuBusySkips=%llu xrWaitTimeouts=%llu ageExpired250ms=%llu",
        g_enabled.load()?1:0,(unsigned long long)s_windows.load(),(unsigned long long)s_redirected_windows.load(),
        (unsigned long long)s_draws.load(),(unsigned long long)s_published.load(),(unsigned long long)s_xr_shown.load(),
        (unsigned long long)s_xr_copy_fail.load(),(unsigned long long)s_busy_skips.load(),(unsigned long long)s_wait_timeouts.load(),(unsigned long long)s_age_expired162r1.load());
    Log(m);
    char reuse[192]{};std::snprintf(reuse,sizeof(reuse),"R59.16C7 HUD_COPY reusedSamePublication=%llu agePolicyMs=250 content=NEW_SERIAL_ONLY fences=PRESERVED",(unsigned long long)s_same_reused59c7.load());Log(reuse);
    char mirror[192]{};std::snprintf(mirror,sizeof(mirror),"V66.178R36C5 HUD_MIRROR enabled=%d composites=%llu",g_mirror_enabled36.load(),(unsigned long long)g_mirror_draws36.load());Log(mirror);
}
bool RedirectActiveThisFrame() noexcept { return g_frame_cleared; }

void OnLateWindowOpen(std::uint32_t frame) noexcept {
    { std::lock_guard<std::mutex> lk(g_rt_mutex); g_frame_cleared=false; g_frame=frame;
      for(auto& l:g_redirect_lists_r111)l.store(nullptr,std::memory_order_relaxed); g_window_draws_r111.store(0,std::memory_order_release); }
    s_windows.fetch_add(1,std::memory_order_relaxed);
    MaybeSummary();
}

bool RedirectMainBind(ID3D12Device* device, ID3D12GraphicsCommandList* list, ID3D12Resource* main,
                      const D3D12_CPU_DESCRIPTOR_HANDLE* dsv, OmSetFn original,const D3D12_CPU_DESCRIPTOR_HANDLE* desktopRtv) noexcept {
    if(!g_enabled.load(std::memory_order_acquire) || !list || !main || !original) return false;
    std::lock_guard<std::mutex> lk(g_rt_mutex);
    const auto md=main->GetDesc();
    if(md.Dimension!=D3D12_RESOURCE_DIMENSION_TEXTURE2D || md.SampleDesc.Count!=1) return false;
    if(!Ensure(device,md)) return false;
    if(g_mirror_enabled36.load()&&!g_mirror_attempted36){
        g_mirror_attempted36=true;g_mirror36=new(std::nothrow) Mirror36;
        const bool ready=g_mirror36&&g_mirror36->Initialize(device,g_rt,md.Format);
        Log(ready?"V66.178R36C5 HUD_MIRROR READY single GPU composite; native Flash drawn once":"V66.178R36C5 HUD_MIRROR UNAVAILABLE initialization failed");
    }
    g_mirror_list36=list;g_mirror_main36=main;g_desktop_rtv36=desktopRtv?*desktopRtv:D3D12_CPU_DESCRIPTOR_HANDLE{};
    if(!g_frame_cleared){
        const float zero[4]={0,0,0,0};
        list->ClearRenderTargetView(g_rtv,zero,0,nullptr);
        g_frame_cleared=true;
        s_redirected_windows.fetch_add(1,std::memory_order_relaxed);
    }
    original(list,1,&g_rtv,FALSE,dsv);
    s_binds.fetch_add(1,std::memory_order_relaxed);
    for(auto& l:g_redirect_lists_r111){ID3D12GraphicsCommandList* cur=l.load(std::memory_order_relaxed);
        if(cur==list)break; if(!cur){l.store(list,std::memory_order_release);break;}}
    return true;
}

void OnLateDraw(bool indexed, UINT count, UINT instances, ID3D12GraphicsCommandList* list) noexcept {
    s_draws.fetch_add(1,std::memory_order_relaxed);
    if(list)for(auto& l:g_redirect_lists_r111)if(l.load(std::memory_order_acquire)==list){g_window_draws_r111.fetch_add(1,std::memory_order_relaxed);break;}
    (indexed?s_indexed:s_inst).fetch_add(1,std::memory_order_relaxed);
    if(!indexed && count<=4 && instances==1) s_small_inst.fetch_add(1,std::memory_order_relaxed);
    NoteCount(indexed?count:(0x80000000u|count));
}

void OnPresentBarrier(ID3D12GraphicsCommandList* list, BarrierFn ob,ID3D12Resource* main,D3D12_RESOURCE_STATES before,OmSetFn originalBind,MirrorDrawFn36 originalDraw) noexcept {
    std::lock_guard<std::mutex> lk(g_rt_mutex);
    if(!g_frame_cleared || !list || !g_rt || !ob) { g_frame_cleared=false; return; }
    g_frame_cleared=false;
    if(g_window_draws_r111.load(std::memory_order_acquire)==0){s_no_draw_skips_r111.fetch_add(1,std::memory_order_relaxed);return;} // R111: cleared but empty
    ID3D12Resource* c=g_cache[g_next_cache];
    D3D12_RESOURCE_BARRIER b[2]{};
    for(auto& x:b){ x.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; x.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; }
    b[0].Transition.pResource=g_rt; b[0].Transition.StateBefore=D3D12_RESOURCE_STATE_RENDER_TARGET; b[0].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
    b[1].Transition.pResource=c;    b[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON;        b[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
    ob(list,2,b);
    list->CopyResource(c,g_rt);
    std::swap(b[0].Transition.StateBefore,b[0].Transition.StateAfter);
    std::swap(b[1].Transition.StateBefore,b[1].Transition.StateAfter);
    ob(list,2,b);
    // Only the exact final backbuffer RT->PRESENT boundary can receive the mirror.
    // The two-eye preservation and HUD cache copy above have already been recorded.
    if(originalBind&&originalDraw&&g_mirror_enabled36.load()&&g_mirror36&&g_mirror36->ready&&list==g_mirror_list36&&main==g_mirror_main36&&g_desktop_rtv36.ptr&&before==D3D12_RESOURCE_STATE_RENDER_TARGET){
        D3D12_RESOURCE_BARRIER sample{};sample.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;sample.Transition.pResource=g_rt;sample.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        sample.Transition.StateBefore=D3D12_RESOURCE_STATE_RENDER_TARGET;sample.Transition.StateAfter=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;ob(list,1,&sample);
        g_mirror36->Draw(list,g_desktop_rtv36,float(g_w),float(g_h),originalBind,originalDraw);
        std::swap(sample.Transition.StateBefore,sample.Transition.StateAfter);ob(list,1,&sample);++g_mirror_draws36;
    }
    // No publication here: the game has only RECORDED this copy.
    g_pending[g_next_cache]={list,c,++g_record_order,g_source_epoch59c7.load(std::memory_order_acquire)};g_pending_any.store(true,std::memory_order_release);
    g_next_cache^=1;
}

void OnSubmittedBatch(UINT count, ID3D12CommandList* const* lists) noexcept {
    if(!lists||!g_pending_any.load(std::memory_order_acquire))return;
    std::lock_guard<std::mutex> lk(g_rt_mutex);
    PendingCopy newest{};
    for(auto& p:g_pending){
        if(!p.list)continue;
        for(UINT i=0;i<count;++i)if(lists[i]==static_cast<ID3D12CommandList*>(p.list)){
            if(p.order>newest.order)newest=p;p={};break;
        }
    }
    g_pending_any.store(g_pending[0].list||g_pending[1].list,std::memory_order_release);
    if(!newest.cache||newest.epoch!=g_source_epoch59c7.load(std::memory_order_acquire))return;
    std::lock_guard<std::mutex> plk(g_pub_mutex);
    if(g_pub_cache)g_pub_cache->Release();
    newest.cache->AddRef();g_pub_cache=newest.cache;++g_pub_serial;g_pub_ms=GetTickCount64();g_pub_epoch59c7=newest.epoch;
    s_published.fetch_add(1,std::memory_order_relaxed);
}

// Copies use the SAME D3D12 queue supplied in the OpenXR session binding.
// xrReleaseSwapchainImage publishes queued graphics work to the runtime; a CPU
// fence wait is not required here. The fence instead protects allocator reuse.
XrHudFrame XrPrepare(XrSession session, ID3D12Device* dev, ID3D12CommandQueue* q) noexcept {
    XrHudFrame out{};
    if(!g_enabled.load(std::memory_order_acquire)||session==XR_NULL_HANDLE||!dev||!q)return out;
    const auto now=GetTickCount64();
    if(x_failed){
        // R104: no HUD for the rest of the session after one failed XR call was a permanent latch (SteamVR answers
        // some calls differently than VDXR). Wait 2 s and until our last GPU copy retired, then start clean.
        if(!x_fail_ms_r104){x_fail_ms_r104=now?now:1;s_fail_events_r104.fetch_add(1,std::memory_order_relaxed);}
        if(now-x_fail_ms_r104<2000||(x_fence&&x_fence->GetCompletedValue()<x_fv))return out;
        ReleaseSlotSourcesR111();
        if(x_sc!=XR_NULL_HANDLE){xrDestroySwapchain(x_sc);x_sc=XR_NULL_HANDLE;}
        if(x_imgs)x_imgs->clear();
        x_acquired=false;x_has_released=false;x_released_serial59c7=x_released_epoch59c7=0;x_w=x_h=0;x_fmt=DXGI_FORMAT_UNKNOWN;
        if(!x_alloc||!x_list||!x_fence||!x_event){
            DeviceHook::SetInternalHudCommandList(nullptr);
            if(x_list){x_list->Release();x_list=nullptr;}if(x_alloc){x_alloc->Release();x_alloc=nullptr;}
            if(x_fence){x_fence->Release();x_fence=nullptr;}if(x_event){CloseHandle(x_event);x_event=nullptr;}x_fv=0;
            ReleaseMoreAllocsR111();
        }
        x_failed=false;x_fail_ms_r104=0;
        const auto r=s_retries_r104.fetch_add(1,std::memory_order_relaxed)+1;
        char m[160]{};std::snprintf(m,sizeof(m),"R104 HUD_XR retry=%llu after a failed OpenXR/D3D12 call (copyFailures=%llu)",(unsigned long long)r,(unsigned long long)s_xr_copy_fail.load());
        LogStatusR104(m);
    }
    // Context changes must go through normal shutdown. Do not reuse a swapchain,
    // command allocator or source from another runtime/device/queue.
    if(x_sc!=XR_NULL_HANDLE&&(x_session59c7!=session||x_device59c7!=dev||
       (x_queue59c7&&x_queue59c7!=q)))return out;
    const auto epoch=g_source_epoch59c7.load(std::memory_order_acquire);
    auto previous=[&]() noexcept { XrHudFrame f{};if(x_has_released && x_released_epoch59c7==epoch && now>=x_released_ms && now-x_released_ms<=250){f.ok=true;f.swapchain=x_sc;f.width=x_w;f.height=x_h;}return f; };
    // Never reset an allocator still referenced by GPU work. Reuse the last
    // RELEASED image instead of stalling the headset thread for up to 500 ms.
    // R111: ring of 3 allocators; the next slot is the oldest submitted copy.
    // A slot whose allocator could not be created is skipped (slot 0 alone = the old single-allocator behaviour);
    // creation is tried again next frame, nothing latches.
    if(x_alloc)for(auto& a:x_alloc_more_r111)if(!a&&FAILED(dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&a))))a=nullptr;
    unsigned slot=x_slot_r111%3u; if(!SlotAllocR111(slot))slot=0;
    if(x_fence && x_fence->GetCompletedValue()<x_slot_fv_r111[slot]){++s_busy_skips;return previous();}
    if(x_slot_src_r111[slot]){x_slot_src_r111[slot]->Release();x_slot_src_r111[slot]=nullptr;}
    ID3D12Resource* cache=nullptr;std::uint64_t ms=0,serial=0,sourceEpoch=0;
    {std::lock_guard<std::mutex> lk(g_pub_mutex);if(g_pub_cache){cache=g_pub_cache;cache->AddRef();ms=g_pub_ms;serial=g_pub_serial;sourceEpoch=g_pub_epoch59c7;}}
    if(!cache)return previous();
    struct Rel{ID3D12Resource*r;~Rel(){if(r)r->Release();}} rel{cache};
    if(sourceEpoch!=epoch||!serial)return previous();
    const auto cd=cache->GetDesc();
    const bool resize=x_sc!=XR_NULL_HANDLE && (x_w!=int32_t(cd.Width)||x_h!=int32_t(cd.Height)||x_fmt!=cd.Format);
    if(resize && x_acquired){
        XrSwapchainImageWaitInfo wi{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};wi.timeout=0;
        const auto wr=xrWaitSwapchainImage(x_sc,&wi);
        if(wr==XR_TIMEOUT_EXPIRED){++s_wait_timeouts;return previous();}
        if(XR_FAILED(wr)){x_failed=true;++s_xr_copy_fail;return out;}
        XrSwapchainImageReleaseInfo ri{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
        if(XR_FAILED(xrReleaseSwapchainImage(x_sc,&ri))){x_failed=true;++s_xr_copy_fail;return out;}
        x_acquired=false;x_has_released=false;
    }
    if(!XrEnsure(session,dev,cd))return previous();
    x_queue59c7=q;
    if(now<ms||now-ms>250){++s_age_expired162r1;return previous();}
    // Only a successful copy + release owns this identity. Never skip a pending
    // acquisition, and never extend the original content timestamp on reuse.
    // R111: same capture as last frame -> copy it again into a fresh image (no runtime "last released image" reuse).
    if(!x_acquired&&x_has_released&&x_released_serial59c7==serial&&x_released_epoch59c7==sourceEpoch){
        ++s_same_reused59c7;++s_recopies_r111;
    }
    if(!x_acquired){
        XrSwapchainImageAcquireInfo ai{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
        if(XR_FAILED(xrAcquireSwapchainImage(x_sc,&ai,&x_index))){++s_xr_copy_fail;return previous();}
        x_acquired=true;
    }
    // R104: 1 ms instead of 0. A zero timeout is legal but some runtimes (SteamVR) answer TIMEOUT for it while the
    // image is free a moment later; 1 ms on the XR thread is far below the frame budget. Still pending on timeout.
    XrSwapchainImageWaitInfo wi{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};wi.timeout=1000000;
    const auto wr=xrWaitSwapchainImage(x_sc,&wi);
    // XR_TIMEOUT_EXPIRED is a positive result, NOT an image ready for use.
    // Keep the acquisition pending and wait it again next frame. No second
    // acquire and no release/reset/copy are legal on this timeout path.
    if(wr==XR_TIMEOUT_EXPIRED){++s_wait_timeouts;return previous();}
    if(XR_FAILED(wr)){x_failed=true;++s_xr_copy_fail;return out;}
    bool ok=x_imgs&&x_index<x_imgs->size();
    ID3D12CommandAllocator* const slotAlloc=SlotAllocR111(slot);
    if(ok)ok=slotAlloc&&SUCCEEDED(slotAlloc->Reset())&&SUCCEEDED(x_list->Reset(slotAlloc,nullptr));
    if(ok){
        auto* img=(*x_imgs)[x_index];
        Barrier(x_list,cache,D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_COPY_SOURCE);
        Barrier(x_list,img,D3D12_RESOURCE_STATE_RENDER_TARGET,D3D12_RESOURCE_STATE_COPY_DEST);
        x_list->CopyResource(img,cache);
        Barrier(x_list,img,D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_STATE_RENDER_TARGET);
        Barrier(x_list,cache,D3D12_RESOURCE_STATE_COPY_SOURCE,D3D12_RESOURCE_STATE_COMMON);
        ok=SUCCEEDED(x_list->Close());
    }
    if(ok){ID3D12CommandList* ls[]={x_list};ok=DeviceHook::ExecuteCommandListsBypass(q,1,ls);}
    if(ok){
        // Retain the source reference until our submitted copy retires.
        cache->AddRef();x_slot_src_r111[slot]=cache;
        ++x_fv;ok=SUCCEEDED(q->Signal(x_fence,x_fv));x_slot_fv_r111[slot]=x_fv;x_slot_r111=slot+1u;
        if(!ok)x_failed=true; // no unproven allocator reuse after a failed signal
    }
    XrSwapchainImageReleaseInfo ri{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
    if(XR_FAILED(xrReleaseSwapchainImage(x_sc,&ri))){ok=false;x_failed=true;}
    x_acquired=false;
    if(!ok){x_has_released=false;++s_xr_copy_fail;return out;}
    x_released_serial59c7=serial;x_released_epoch59c7=sourceEpoch;
    x_has_released=true;x_released_ms=ms;++s_xr_shown;
    out.ok=true;out.swapchain=x_sc;out.width=x_w;out.height=x_h;return out;
}

ID3D12Resource* AcquirePublishedForBlit(std::uint64_t max_age_ms) noexcept {
    if(!g_enabled.load(std::memory_order_acquire)) return nullptr;
    std::lock_guard<std::mutex> lk(g_pub_mutex);
    if(!g_pub_cache) return nullptr;
    if(GetTickCount64()-g_pub_ms>max_age_ms) return nullptr;
    g_pub_cache->AddRef();
    s_xr_shown.fetch_add(1,std::memory_order_relaxed);
    return g_pub_cache;
}

void StatusR111(std::uint64_t* noDrawSkips,std::uint64_t* recopies,std::uint64_t* published,std::uint64_t* shown) noexcept {
    if(noDrawSkips)*noDrawSkips=s_no_draw_skips_r111.load(std::memory_order_relaxed);
    if(recopies)*recopies=s_recopies_r111.load(std::memory_order_relaxed);
    if(published)*published=s_published.load(std::memory_order_relaxed);
    if(shown)*shown=s_xr_shown.load(std::memory_order_relaxed);
}

void StatusR104(char* out,std::size_t n) noexcept {
    if(!out||!n)return;
    std::snprintf(out,n,"HUD{enabled:%d swapchain:%s %dx%d fmt:%u published:%llu shown:%llu copyFail:%llu waitTimeout:%llu gpuBusy:%llu aged:%llu failEvents:%llu retries:%llu}",
        g_enabled.load()?1:0,x_sc!=XR_NULL_HANDLE?"YES":(x_failed?"FAILED":"NO"),x_w,x_h,(unsigned)x_fmt,(unsigned long long)s_published.load(),
        (unsigned long long)s_xr_shown.load(),(unsigned long long)s_xr_copy_fail.load(),(unsigned long long)s_wait_timeouts.load(),
        (unsigned long long)s_busy_skips.load(),(unsigned long long)s_age_expired162r1.load(),(unsigned long long)s_fail_events_r104.load(),
        (unsigned long long)s_retries_r104.load());
}

void XrShutdown() noexcept {
    // Only shutdown may wait. Never free resources still referenced by our GPU
    // copy. On device/fence failure retain them until process exit instead.
    if(x_fence&&x_fence->GetCompletedValue()<x_fv){
        if(FAILED(x_fence->SetEventOnCompletion(x_fv,x_event))||WaitForSingleObject(x_event,2000)!=WAIT_OBJECT_0){x_failed=true;return;}
    }
    ReleaseSlotSourcesR111();
    if(x_sc!=XR_NULL_HANDLE){xrDestroySwapchain(x_sc);x_sc=XR_NULL_HANDLE;}
    x_acquired=false;x_has_released=false;
    DeviceHook::SetInternalHudCommandList(nullptr);
    if(x_list){x_list->Release();x_list=nullptr;}if(x_alloc){x_alloc->Release();x_alloc=nullptr;}
    ReleaseMoreAllocsR111();
    if(x_fence){x_fence->Release();x_fence=nullptr;}if(x_event){CloseHandle(x_event);x_event=nullptr;}
    x_fv=0;x_failed=false;x_fail_ms_r104=0;x_session59c7=XR_NULL_HANDLE;
    x_device59c7=nullptr;x_queue59c7=nullptr;x_released_serial59c7=x_released_epoch59c7=0;
}

} // namespace kcd2vr::hud_layer_v6664
