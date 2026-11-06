#include "native_cloud_right_v6641.h"
#include "diagnostic_trace.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <optional>
#include <utility>
#include <cstdio>

namespace kcd2vr::native_cloud_right_v6641 { namespace {

// ---- Static proof (retail WHGame.dll, SHA-256 bdf8f9e4..., timestamp 0x6A350E20) ----
// Init 0x180741FD0 (vtable slot right before 0x180C1A1E0) creates texture objects:
//   eye0: Max0 +0x38, Max1 +0x40, Min0 +0x58, Min1 +0x60, PrevDepth +0x78
//   eye1: Max0_R +0x48, Max1_R +0x50, Min0_R +0x68, Min1_R +0x70, PrevDepth_R +0x80
// Update/resize 0x1809B94E8 (same vtable; its only reference is at DLL file offset 0x3D2BB38) loops eye 0..1 and calls
//   0x181E142E0(const u32 size[2], CTexture*, u8 fmt, u32 flags, u8 allowCreate)
//   with fmt 0x14 / flags 0x1101 for Max/Min, and fmt=[depth+0x96] flags=[depth+0x98]
//   for PrevDepth (depth = [[stage+0x10]+0x80]); allowCreate = (eye==0).
// 0x181E142E0: if tex already has backing (+0x80) at size (+0x90/+0x92) -> no-op;
//   else if !allowCreate -> return 0 (NO allocation); else create via 0x1808913F0.
// Cloud Execute 0x18056D778 selects textures by selector: [stage+0x38+eye*0x10+idx*8],
//   [stage+0x58+eye*0x10+idx*8], [stage+0x78+eye*8] -> RIGHT uses the unbacked set.
// Stereo reprojection flag params+0x32 is hard-coded 0 (0x1809BA22D), so KCD2 never
//   takes the L->R reprojection path; r_VolumetricCloudsStereoReprojection is inert.
constexpr std::uint32_t kTimestamp = 0x6A350E20u;
constexpr std::uintptr_t kUpdateRva = 0x9B94E8ull;
constexpr std::uintptr_t kAllocRva  = 0x1E142E0ull;
constexpr std::uintptr_t kLeftMax0 = 0x38, kLeftPrevDepth = 0x78;
constexpr std::uintptr_t kRightTex[4] = {0x48,0x50,0x68,0x70}; // Max0_R, Max1_R, Min0_R, Min1_R
constexpr std::uintptr_t kRightPrevDepth = 0x80;
constexpr std::uintptr_t kStageRenderer = 0x10, kRendererDepth = 0x80;
constexpr std::uintptr_t kTexBacking = 0x80, kTexW = 0x90, kTexH = 0x92, kTexFmt = 0x96, kTexFlags = 0x98;
constexpr std::uintptr_t kClearCounter = 0x56C8; // retail sets 4 after (re)allocation (0x1809B9891)
constexpr std::uint8_t  kCloudFmt = 0x14;
constexpr std::uint32_t kCloudFlags = 0x1101u;

// V66.42 A/B + timing. Gate 0x180578998 is called only by cloud-stage functions
// (Execute 0x18056D778, Update 0x1805732A8, 0x1809B907C); returning 0 skips that
// cloud work, which is exactly the retail RIGHT behaviour (unbacked -> nothing drawn).
constexpr std::uintptr_t kGateRva = 0x578998ull;
constexpr std::uintptr_t kExecuteRva = 0x56D778ull;
constexpr unsigned char kGateEntry[] = {
    0x48,0x8B,0x81,0x88,0x00,0x00,0x00,0x45,0x33,0xC9,0x41,0xB8,0x01,0x00,0x00,0x00,0x48,0x85,0xC0 };
constexpr unsigned char kExecuteEntry[] = {
    0x48,0x89,0x5C,0x24,0x08,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8D,0x6C,0x24,0xD9 };

constexpr unsigned char kUpdateEntry[] = {
    0x40,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8B,0xEC,0x48,0x83,0xEC,0x68 };
constexpr unsigned char kAllocEntry[] = {
    0x40,0x53,0x48,0x83,0xEC,0x40,0x48,0x8B,0xDA,0x48,0x85,0xD2,0x74,0x7E,0x0F,0xB7,0x83,0x90,0x00,0x00 };

// Pass-through signature: the original's argument registers and return value are
// forwarded untouched, so the detour is correct whatever the exact prototype is.
using UpdateFn = std::uintptr_t(__fastcall*)(void*,void*,void*,void*);
using AllocFn  = std::uint8_t(__fastcall*)(const std::uint32_t*,void*,std::uint8_t,std::uint32_t,std::uint8_t);

std::optional<DetourModKit::hook::Hook> g_hook;
UpdateFn g_original=nullptr;
AllocFn g_alloc=nullptr;
std::atomic<bool> g_installed{false};
std::atomic<bool> g_backed{false};
std::atomic<std::uint64_t> g_calls{0};
std::atomic<std::uint64_t> g_allocs{0};
std::atomic<std::uint64_t> g_refusals{0};

using GateFn = std::uint8_t(__fastcall*)(void*);
using ExecFn = std::uintptr_t(__fastcall*)(void*,void*,void*,void*);
std::optional<DetourModKit::hook::Hook> g_gate_hook, g_exec_hook;
GateFn g_gate_original=nullptr;
ExecFn g_exec_original=nullptr;
std::atomic<bool> g_right_enabled{true};
std::atomic<std::uint64_t> g_gate_skips{0};
std::uintptr_t g_last_left_backing=0; // render thread only
// timing (render thread only)
double g_qpc_to_ms=0.0;
std::uint64_t g_t_calls[2]{}, g_t_last_log=0;
double g_t_sum[2]{}, g_t_max[2]{};
std::uint64_t g_update_calls_window=0;

bool Readable(const void* p,std::size_t n=1) noexcept {
    if(!p || n==0) return false;
    MEMORY_BASIC_INFORMATION m{};
    if(!VirtualQuery(p,&m,sizeof(m)) || m.State!=MEM_COMMIT || (m.Protect&(PAGE_NOACCESS|PAGE_GUARD))) return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p);
    const auto e=reinterpret_cast<std::uintptr_t>(m.BaseAddress)+m.RegionSize;
    return a<=e && n<=e-a;
}
template<class T> bool SafeRead(std::uintptr_t p,T& out) noexcept {
    // V66.42: SEH-only (the VirtualQuery pre-check ran ~20 syscalls per cloud update).
    if(p<0x10000) return false;
    __try { out=*reinterpret_cast<const T*>(p); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
int SelectorOf(void* stage) noexcept {
    std::uintptr_t crv=0; std::int32_t sel=-1;
    if(!SafeRead(reinterpret_cast<std::uintptr_t>(stage)+0x28,crv) || !crv) return -1;
    if(!SafeRead(crv+0x8EE0,sel)) return -1;
    return sel;
}

bool EntryMatches(std::uintptr_t a,const unsigned char* bytes,std::size_t n) noexcept {
    if(!Readable(reinterpret_cast<void*>(a),n)) return false;
    __try { return std::memcmp(reinterpret_cast<const void*>(a),bytes,n)==0; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
bool ExactBuild(std::uintptr_t b) noexcept {
    if(!b) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(b);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(b+dos->e_lfanew);
        if(nt->Signature!=IMAGE_NT_SIGNATURE || nt->FileHeader.TimeDateStamp!=kTimestamp) return false;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    return EntryMatches(b+kUpdateRva,kUpdateEntry,sizeof(kUpdateEntry)) &&
           EntryMatches(b+kAllocRva,kAllocEntry,sizeof(kAllocEntry));
}

struct TexInfo { std::uintptr_t obj=0, backing=0; std::uint16_t w=0,h=0; std::uint8_t fmt=0; std::uint32_t flags=0; };
bool ReadTex(std::uintptr_t stage,std::uintptr_t off,TexInfo& t) noexcept {
    t={};
    if(!SafeRead(stage+off,t.obj) || !t.obj) return false;
    return SafeRead(t.obj+kTexBacking,t.backing) && SafeRead(t.obj+kTexW,t.w) && SafeRead(t.obj+kTexH,t.h);
}

void Refuse(const char* why,void* stage) noexcept {
    g_backed.store(false,std::memory_order_release);
    const auto n=g_refusals.fetch_add(1,std::memory_order_relaxed)+1;
    if(n<=8 || (n%600ull)==0) { char m[256]{}; std::snprintf(m,sizeof(m),
        "V66.41-CLOUD-RIGHT REFUSED n=%llu reason=%s stage=%p",(unsigned long long)n,why,stage); diagnostic_trace::Write(m); }
}

void ProvisionRight(void* stage_ptr) noexcept {
    if(!stage_ptr || !g_alloc) return;
    const auto stage=reinterpret_cast<std::uintptr_t>(stage_ptr);
    // Mirror exactly what retail did for eye 0 this call: only act once LEFT is backed.
    TexInfo lmax{}, ldepth{};
    if(!ReadTex(stage,kLeftMax0,lmax) || !lmax.backing || !lmax.w || !lmax.h) return; // clouds off / not sized yet
    if(g_backed.load(std::memory_order_relaxed) && lmax.backing==g_last_left_backing) return; // V66.42 steady state
    g_last_left_backing=lmax.backing;
    if(!ReadTex(stage,kLeftPrevDepth,ldepth) || !ldepth.backing) return;
    if(ldepth.w!=lmax.w || ldepth.h!=lmax.h) { Refuse("left-size-mismatch",stage_ptr); return; }
    const std::uint32_t size[2]={lmax.w,lmax.h};

    // PrevDepth format/flags come from the scene depth texture, exactly as retail reads them.
    std::uintptr_t renderer=0, depth=0; std::uint8_t dfmt=0; std::uint32_t dflags=0;
    if(!SafeRead(stage+kStageRenderer,renderer) || !renderer ||
       !SafeRead(renderer+kRendererDepth,depth) || !depth ||
       !SafeRead(depth+kTexFmt,dfmt) || !SafeRead(depth+kTexFlags,dflags)) { Refuse("depth-desc-unreadable",stage_ptr); return; }

    bool all=true; unsigned allocated=0;
    for(const auto off:kRightTex) {
        TexInfo r{}; if(!ReadTex(stage,off,r)) { Refuse("right-object-missing",stage_ptr); return; }
        if(r.backing && r.w==lmax.w && r.h==lmax.h) continue;
        const bool ok=g_alloc(size,reinterpret_cast<void*>(r.obj),kCloudFmt,kCloudFlags,1)!=0;
        if(ok) ++allocated;
        TexInfo after{}; all=ReadTex(stage,off,after) && after.backing && all;
    }
    {
        TexInfo r{}; if(!ReadTex(stage,kRightPrevDepth,r)) { Refuse("right-depth-object-missing",stage_ptr); return; }
        if(!(r.backing && r.w==lmax.w && r.h==lmax.h)) {
            if(g_alloc(size,reinterpret_cast<void*>(r.obj),dfmt,dflags,1)!=0) ++allocated;
        }
        TexInfo after{}; all=ReadTex(stage,kRightPrevDepth,after) && after.backing && all;
    }
    if(allocated) {
        // Retail marks freshly (re)allocated histories for clearing (Execute clears while >0).
        std::int32_t clear=0;
        if(SafeRead(stage+kClearCounter,clear) && clear<4) {
            __try { *reinterpret_cast<std::int32_t*>(stage+kClearCounter)=4; } __except(EXCEPTION_EXECUTE_HANDLER) {}
        }
        g_allocs.fetch_add(allocated,std::memory_order_relaxed);
        char m[320]{}; std::snprintf(m,sizeof(m),
            "V66.41-CLOUD-RIGHT allocated=%u stage=%p size=%ux%u fmt=0x%02X flags=0x%X depthFmt=0x%02X depthFlags=0x%X allBacked=%d clearCounter=4",
            allocated,stage_ptr,unsigned(lmax.w),unsigned(lmax.h),unsigned(kCloudFmt),unsigned(kCloudFlags),unsigned(dfmt),unsigned(dflags),all?1:0);
        diagnostic_trace::Write(m);
    }
    const bool was=g_backed.exchange(all,std::memory_order_acq_rel);
    if(was!=all) diagnostic_trace::Write(all?"V66.41-CLOUD-RIGHT state=BACKED (Max_R/Min_R/PrevDepth_R)":"V66.41-CLOUD-RIGHT state=NOT-BACKED");
}

std::uint8_t __fastcall GateDetour(void* stage) noexcept {
    if(!g_right_enabled.load(std::memory_order_relaxed) && SelectorOf(stage)==1) {
        g_gate_skips.fetch_add(1,std::memory_order_relaxed);
        return 0;
    }
    return g_gate_original?g_gate_original(stage):0;
}

std::uintptr_t __fastcall ExecuteDetour(void* stage,void* a2,void* a3,void* a4) noexcept {
    const int sel=SelectorOf(stage);
    LARGE_INTEGER t0{},t1{}; QueryPerformanceCounter(&t0);
    const auto r=g_exec_original?g_exec_original(stage,a2,a3,a4):0;
    QueryPerformanceCounter(&t1);
    if(sel==0 || sel==1) {
        const double ms=double(t1.QuadPart-t0.QuadPart)*g_qpc_to_ms;
        g_t_calls[sel]++; g_t_sum[sel]+=ms; if(ms>g_t_max[sel]) g_t_max[sel]=ms;
    }
    const auto now=static_cast<std::uint64_t>(t1.QuadPart);
    if(!g_t_last_log) g_t_last_log=now;
    if(double(now-g_t_last_log)*g_qpc_to_ms>=5000.0) {
        char m[384]{}; std::snprintf(m,sizeof(m),
            "V66.42-CLOUD-TIME 5s cpuMs L={calls:%llu avg:%.3f max:%.3f} R={calls:%llu avg:%.3f max:%.3f} updateCalls=%llu rightClouds=%s backed=%d gateSkips=%llu",
            (unsigned long long)g_t_calls[0],g_t_calls[0]?g_t_sum[0]/double(g_t_calls[0]):0.0,g_t_max[0],
            (unsigned long long)g_t_calls[1],g_t_calls[1]?g_t_sum[1]/double(g_t_calls[1]):0.0,g_t_max[1],
            (unsigned long long)g_update_calls_window,g_right_enabled.load()?"ON":"OFF(F4)",g_backed.load()?1:0,
            (unsigned long long)g_gate_skips.load(std::memory_order_relaxed));
        diagnostic_trace::Write(m);
        g_t_calls[0]=g_t_calls[1]=0; g_t_sum[0]=g_t_sum[1]=0.0; g_t_max[0]=g_t_max[1]=0.0; g_update_calls_window=0;
        g_t_last_log=now;
    }
    return r;
}

std::uintptr_t __fastcall UpdateDetour(void* stage,void* a2,void* a3,void* a4) noexcept {
    const auto r=g_original?g_original(stage,a2,a3,a4):0;
    g_calls.fetch_add(1,std::memory_order_relaxed);
    ++g_update_calls_window;
    ProvisionRight(stage);
    return r;
}

} // namespace

bool Install() noexcept {
    if(g_installed.load(std::memory_order_acquire)) return true;
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!ExactBuild(b)) { diagnostic_trace::Write("V66.41-CLOUD-RIGHT install REFUSED exact WHGame/prologue contract mismatch"); return false; }
    g_alloc=reinterpret_cast<AllocFn>(b+kAllocRva);
    DetourModKit::hook::InlineRequest req{
        "kcd2vr_v6641_cloud_right_backing",
        DetourModKit::hook::Target{DetourModKit::Address{b+kUpdateRva}},
        DetourModKit::hook::Options{}
    };
    auto h=DetourModKit::hook::inline_at(std::move(req),&UpdateDetour);
    if(!h) { g_alloc=nullptr; diagnostic_trace::Write("V66.41-CLOUD-RIGHT install FAILED hook-create"); return false; }
    g_hook=std::move(*h);
    g_original=g_hook->original<UpdateFn>();
    if(!g_original || !g_hook->enable()) { g_original=nullptr; g_alloc=nullptr; g_hook.reset(); diagnostic_trace::Write("V66.41-CLOUD-RIGHT install FAILED hook-enable"); return false; }
    { LARGE_INTEGER f{}; QueryPerformanceFrequency(&f); g_qpc_to_ms=f.QuadPart?1000.0/double(f.QuadPart):0.0; }
    // V66.42 optional hooks: A/B gate + CPU timing. Failure leaves the fix itself active.
    if(EntryMatches(b+kGateRva,kGateEntry,sizeof(kGateEntry))) {
        DetourModKit::hook::InlineRequest gr{"kcd2vr_v6642_cloud_gate_ab",DetourModKit::hook::Target{DetourModKit::Address{b+kGateRva}},DetourModKit::hook::Options{}};
        if(auto gh=DetourModKit::hook::inline_at(std::move(gr),&GateDetour)) {
            g_gate_hook=std::move(*gh); g_gate_original=g_gate_hook->original<GateFn>();
            if(!g_gate_original || !g_gate_hook->enable()) { g_gate_original=nullptr; g_gate_hook.reset(); }
        }
    }
    if(EntryMatches(b+kExecuteRva,kExecuteEntry,sizeof(kExecuteEntry))) {
        DetourModKit::hook::InlineRequest er{"kcd2vr_v6642_cloud_execute_timing",DetourModKit::hook::Target{DetourModKit::Address{b+kExecuteRva}},DetourModKit::hook::Options{}};
        if(auto eh=DetourModKit::hook::inline_at(std::move(er),&ExecuteDetour)) {
            g_exec_hook=std::move(*eh); g_exec_original=g_exec_hook->original<ExecFn>();
            if(!g_exec_original || !g_exec_hook->enable()) { g_exec_original=nullptr; g_exec_hook.reset(); }
        }
    }
    { char m[160]{}; std::snprintf(m,sizeof(m),"V66.42-CLOUD-RIGHT extras gateAB=%d executeTiming=%d",g_gate_hook?1:0,g_exec_hook?1:0); diagnostic_trace::Write(m); }
    g_installed.store(true,std::memory_order_release);
    diagnostic_trace::Write("V66.41-CLOUD-RIGHT installed: CVolumetricCloudsStage update post-hook; allocates retail-created RIGHT cloud histories with retail format/flags");
    return true;
}
void Uninstall() noexcept {
    g_installed.store(false,std::memory_order_release);
    if(g_exec_hook) { (void)g_exec_hook->disable(); g_exec_hook.reset(); }
    if(g_gate_hook) { (void)g_gate_hook->disable(); g_gate_hook.reset(); }
    if(g_hook) { (void)g_hook->disable(); g_hook.reset(); }
    g_original=nullptr; g_alloc=nullptr; g_gate_original=nullptr; g_exec_original=nullptr;
}
bool Installed() noexcept { return g_installed.load(std::memory_order_acquire); }
void SetRightCloudsEnabled(bool on) noexcept { g_right_enabled.store(on,std::memory_order_release); }

} // namespace kcd2vr::native_cloud_right_v6641
