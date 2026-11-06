#include "native_water_reflection_right_v6663.h"
#include "diagnostic_trace.h"
#include "retail_build112.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <optional>
#include <utility>
#include <cstdio>

namespace kcd2vr::native_water_reflection_right_v6663 { namespace {

// Exact retail contract: WHGame.dll timestamp 0x6A350E20, SHA-256 bdf8f9e4...
// CWaterStage vtable 0x183D2C8B8; vslot +0x20 -> Execute 0x1807BBE70.
// Execute reads CRV frame (+0x2C), reduces it mod 2, and indexes texture registry
// [stage+0x10] at +0x288 + parity*8.  The water binding path 0x1807BCC58 uses
// 0x1807BD444(frame)=frame%2 and 0x181E14930(frame)=(frame+1)%2, so BOTH A/B
// textures participate as current/previous history for one temporal water pass.
// Retail allocation 0x180A61829/0x180A618B8 creates exactly:
//   +0x288 "$WaterVolumeRefl"      customId 0x24
//   +0x290 "$WaterVolumeReflPrev"  customId 0x25
// with no _R pair.  Since KCD2VR executes LEFT then RIGHT with the SAME CRV game
// frame, both eyes select the same current slot; RIGHT becomes the surviving
// previous-frame history.  This module gives RIGHT its own A/B pair but leaves
// the native water pass, SSR/cubemap work and temporal shader unchanged.
constexpr std::uint32_t kTimestamp=0x6A350E20u;
constexpr std::uintptr_t kWaterVtableRva=0x3D2C8B8ull;
constexpr std::uintptr_t kExecuteRva=0x7BBE70ull;
// R2: Execute only prepares pass targets/descriptors. The actual reflection draw
// is 0x7C1724, called by water-volume rendering 0x7C18B8 at 0x7C198A.
// That later draw AND subsequent water surface consumption must see the same pair.
constexpr std::uintptr_t kVolumesRva=0x7C18B8ull;
constexpr std::uintptr_t kLastReflectionFrame=0xD530ull; // read/updated by 0x7C1724
constexpr unsigned char kVolumesEntry[]={
    0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xEC,0x40};
constexpr unsigned char kGenerateCall[]={0x48,0x8B,0xCF,0xE8,0x95,0xFD,0xFF,0xFF};
constexpr std::uintptr_t kProvisionRva=0x73CEF4ull;
constexpr std::uintptr_t kModuloRva=0x7BD444ull;
constexpr std::uintptr_t kNextModuloRva=0x1E14930ull;
constexpr std::uintptr_t kBindProofRva=0x7BCC58ull;

constexpr std::uintptr_t kStageRegistry=0x10;
constexpr std::uintptr_t kStageCrv=0x28;
constexpr std::uintptr_t kCrvType=0x14;
constexpr std::uintptr_t kCrvFrame=0x2C;
constexpr std::uintptr_t kCrvSelector=0x8EE0;
constexpr std::uintptr_t kRefl0=0x288;
constexpr std::uintptr_t kRefl1=0x290;
constexpr std::uintptr_t kTexBacking=0x80;
constexpr std::uintptr_t kTexW=0x90;
constexpr std::uintptr_t kTexH=0x92;
constexpr std::uintptr_t kTexFlags=0x98;
constexpr std::uintptr_t kTexSourceFmt=0xA4;
constexpr std::uint32_t kWaterFlags=0x00010100u; // exact retail caller input
constexpr std::int32_t kWaterId0=0x24;
constexpr std::int32_t kWaterId1=0x25;

constexpr unsigned char kExecuteEntry[]={
    0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,
    0x7C,0x24,0x18,0x55,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57
};
constexpr unsigned char kProvisionEntry[]={
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,0x48,
    0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x41,0x56,0x48,0x83,0xEC
};
constexpr unsigned char kModuloBytes[]={0x8B,0xC2,0xB9,0x02,0x00,0x00,0x00,0x99,0xF7,0xF9,0x8B,0xC2,0xC3};
constexpr unsigned char kNextModuloBytes[]={0x8D,0x42,0x01,0x25,0x01,0x00,0x00,0x80};
constexpr unsigned char kBindProofBytes[]={0x4C,0x8B,0x4E,0x10,0xE8};

struct Color { float r=0,b=0,g=0,a=0; };
using ExecFn=std::uintptr_t(__fastcall*)(void*,void*,void*,void*);
using ProvisionFn=std::uint8_t(__fastcall*)(const char*,void**,std::int32_t,std::int32_t,
                                            const Color*,std::uint8_t,std::uint8_t,
                                            std::uint8_t,std::int32_t,std::uint32_t);
std::optional<DetourModKit::hook::Hook> g_hook, g_volumesHook;
using VolumesFn=void(__fastcall*)(void*);
VolumesFn g_volumesOriginal=nullptr;
std::int32_t g_rightLastReflectionFrame=-1;
std::uintptr_t g_rightLastStage=0;
std::atomic<std::uint64_t> g_volumeCalls[2]{}, g_volumeSwaps{0};
ExecFn g_original=nullptr;
ProvisionFn g_provision=nullptr;
std::atomic<bool> g_installed{false};
std::atomic<bool> g_ready{false};
void* g_right[2]{}; // references retained to process exit, matching other native repair modules
std::uint16_t g_lastW=0,g_lastH=0;
std::uint8_t g_lastFmt=0;
std::atomic<std::uint64_t> g_leftCalls{0},g_rightCalls{0},g_swaps{0},g_allocs{0},g_fail{0};
std::uint64_t g_lastLogMs=0;

bool MemEq(std::uintptr_t p,const unsigned char* b,std::size_t n) noexcept {
    __try { return std::memcmp(reinterpret_cast<const void*>(p),b,n)==0; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
template<class T> bool Rd(std::uintptr_t p,T& out) noexcept {
    if(p<0x10000) return false;
    __try { out=*reinterpret_cast<const T*>(p); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
template<class T> bool Wr(std::uintptr_t p,const T& v) noexcept {
    if(p<0x10000) return false;
    __try { *reinterpret_cast<T*>(p)=v; return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
struct Tex { void* obj=nullptr; void* backing=nullptr; std::uint16_t w=0,h=0; std::uint8_t fmt=0; std::uint32_t flags=0; bool ok=false; };
Tex ReadTex(void* p) noexcept {
    Tex t{}; t.obj=p; if(!p) return t; const auto a=reinterpret_cast<std::uintptr_t>(p);
    if(!Rd(a+kTexBacking,t.backing)||!Rd(a+kTexW,t.w)||!Rd(a+kTexH,t.h)||!Rd(a+kTexSourceFmt,t.fmt)||!Rd(a+kTexFlags,t.flags)) return t;
    t.ok=true; return t;
}

bool Exact(std::uintptr_t b) noexcept {
    if(!b || !retail_build112::Verify(b)) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(b);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(b+dos->e_lfanew);
        if(nt->Signature!=IMAGE_NT_SIGNATURE||nt->FileHeader.TimeDateStamp!=kTimestamp) return false;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    std::uintptr_t vt=0;
    return MemEq(b+kVolumesRva,kVolumesEntry,sizeof(kVolumesEntry)) &&
           MemEq(b+0x7C1987,kGenerateCall,sizeof(kGenerateCall)) &&
           MemEq(b+kExecuteRva,kExecuteEntry,sizeof(kExecuteEntry)) &&
           MemEq(b+kProvisionRva,kProvisionEntry,sizeof(kProvisionEntry)) &&
           MemEq(b+kModuloRva,kModuloBytes,sizeof(kModuloBytes)) &&
           MemEq(b+kNextModuloRva,kNextModuloBytes,sizeof(kNextModuloBytes)) &&
           MemEq(b+kBindProofRva,kBindProofBytes,sizeof(kBindProofBytes)) &&
           Rd(b+kWaterVtableRva+0x20,vt) && vt==b+kExecuteRva;
}

int EyeAndFrame(void* stage,std::uint32_t& frame,std::uintptr_t& registry) noexcept {
    frame=0; registry=0; std::uintptr_t crv=0; std::uint32_t type=~0u; std::int32_t sel=-1;
    const auto s=reinterpret_cast<std::uintptr_t>(stage);
    if(!Rd(s+kStageCrv,crv)||!crv||!Rd(crv+kCrvType,type)||type!=0u||!Rd(crv+kCrvSelector,sel)||!Rd(crv+kCrvFrame,frame)||!Rd(s+kStageRegistry,registry)||!registry) return -1;
    return (sel==0||sel==1)?sel:-1;
}

bool ProvisionOne(int i,const Tex& native) noexcept {
    if(!g_provision||!native.ok||!native.backing||!native.w||!native.h||!native.fmt) return false;
    Tex cur=ReadTex(g_right[i]);
    if(cur.ok&&cur.backing&&cur.w==native.w&&cur.h==native.h&&cur.fmt==native.fmt) return true;
    const Color clear{};
    const char* name=i==0?"$WaterVolumeRefl_R":"$WaterVolumeReflPrev_R";
    const std::int32_t id=i==0?kWaterId0:kWaterId1;
    std::uint8_t r=0;
    __try { r=g_provision(name,&g_right[i],native.w,native.h,&clear,1,1,native.fmt,id,kWaterFlags); }
    __except(EXCEPTION_EXECUTE_HANDLER) { r=0; }
    Tex after=ReadTex(g_right[i]);
    if(r&&after.ok&&after.backing&&after.w==native.w&&after.h==native.h&&after.fmt==native.fmt) {
        g_rightLastReflectionFrame=-1; g_rightLastStage=0;
        g_allocs.fetch_add(1,std::memory_order_relaxed); return true;
    }
    return false;
}

bool EnsureRight(std::uintptr_t registry) noexcept {
    void *n0=nullptr,*n1=nullptr;
    if(!Rd(registry+kRefl0,n0)||!Rd(registry+kRefl1,n1)) return false;
    const Tex a=ReadTex(n0),b=ReadTex(n1);
    if(!a.ok||!b.ok||!a.backing||!b.backing||a.w!=b.w||a.h!=b.h||a.fmt!=b.fmt) return false;
    const bool ok=ProvisionOne(0,a)&&ProvisionOne(1,b);
    const bool old=g_ready.exchange(ok,std::memory_order_acq_rel);
    if(ok && (!old||g_lastW!=a.w||g_lastH!=a.h||g_lastFmt!=a.fmt)) {
        g_lastW=a.w; g_lastH=a.h; g_lastFmt=a.fmt;
        char m[384]{}; std::snprintf(m,sizeof(m),
            "V66.63-WATER-REFL RIGHT-HISTORY ready=1 size=%ux%u fmt=0x%02X native={%p,%p} right={%p,%p} allocs=%llu contract=PER-EYE-A/B-NO-EXTRA-RENDER",
            unsigned(a.w),unsigned(a.h),unsigned(a.fmt),n0,n1,g_right[0],g_right[1],
            (unsigned long long)g_allocs.load(std::memory_order_relaxed)); diagnostic_trace::Write(m);
    }
    return ok;
}

void LogPeriodic(std::uint32_t frame,int eye,std::uintptr_t registry) noexcept {
    const auto now=GetTickCount64(); if(g_lastLogMs&&now-g_lastLogMs<5000) return; g_lastLogMs=now;
    void *n0=nullptr,*n1=nullptr; (void)Rd(registry+kRefl0,n0); (void)Rd(registry+kRefl1,n1);
    char m[420]{}; std::snprintf(m,sizeof(m),
        "V66.63-WATER-REFL 5s eye=%c frame=%u parity=%u ready=%d calls={L:%llu R:%llu} swaps=%llu fail=%llu nativePair={%p,%p} rightPair={%p,%p}",
        eye?'R':'L',frame,frame&1u,g_ready.load()?1:0,(unsigned long long)g_leftCalls.load(),(unsigned long long)g_rightCalls.load(),
        (unsigned long long)g_swaps.load(),(unsigned long long)g_fail.load(),n0,n1,g_right[0],g_right[1]); diagnostic_trace::Write(m);
}

std::uintptr_t __fastcall Detour(void* stage,void* a2,void* a3,void* a4) noexcept {
    std::uint32_t frame=0; std::uintptr_t registry=0; const int eye=EyeAndFrame(stage,frame,registry);
    if(eye==0) {
        g_leftCalls.fetch_add(1,std::memory_order_relaxed);
        const auto r=g_original?g_original(stage,a2,a3,a4):0;
        (void)EnsureRight(registry); LogPeriodic(frame,eye,registry); return r;
    }
    if(eye!=1) return g_original?g_original(stage,a2,a3,a4):0;
    g_rightCalls.fetch_add(1,std::memory_order_relaxed);
    if(!g_ready.load(std::memory_order_acquire) && !EnsureRight(registry)) {
        g_fail.fetch_add(1,std::memory_order_relaxed); return g_original?g_original(stage,a2,a3,a4):0;
    }
    void *native0=nullptr,*native1=nullptr;
    if(!Rd(registry+kRefl0,native0)||!Rd(registry+kRefl1,native1)||!native0||!native1||!g_right[0]||!g_right[1]) {
        g_fail.fetch_add(1,std::memory_order_relaxed); return g_original?g_original(stage,a2,a3,a4):0;
    }
    if(!Wr(registry+kRefl0,g_right[0])||!Wr(registry+kRefl1,g_right[1])) {
        (void)Wr(registry+kRefl0,native0); (void)Wr(registry+kRefl1,native1);
        g_fail.fetch_add(1,std::memory_order_relaxed); return g_original?g_original(stage,a2,a3,a4):0;
    }
    const auto r=g_original?g_original(stage,a2,a3,a4):0;
    const bool r0=Wr(registry+kRefl0,native0), r1=Wr(registry+kRefl1,native1);
    if(!r0||!r1) g_fail.fetch_add(1,std::memory_order_relaxed);
    g_swaps.fetch_add(1,std::memory_order_relaxed); LogPeriodic(frame,eye,registry); return r;
}

// Scope the private pair across actual reflection generation, mip generation and
// water surface draws. The earlier setup scope alone never populated this pair.
void __fastcall VolumesDetour(void* stage) noexcept {
    std::uint32_t frame=0; std::uintptr_t registry=0;
    const int eye=EyeAndFrame(stage,frame,registry);
    if(eye==0) g_volumeCalls[0].fetch_add(1,std::memory_order_relaxed);
    if(eye!=1) { if(g_volumesOriginal) g_volumesOriginal(stage); return; }
    g_volumeCalls[1].fetch_add(1,std::memory_order_relaxed);
    void *native[2]{}; std::int32_t nativeLast=-1;
    const auto st=reinterpret_cast<std::uintptr_t>(stage);
    const bool prepared=EnsureRight(registry) &&
        Rd(registry+kRefl0,native[0]) && Rd(registry+kRefl1,native[1]) &&
        Rd(st+kLastReflectionFrame,nativeLast);
    const Tex n0=ReadTex(native[0]), n1=ReadTex(native[1]);
    const Tex r0=ReadTex(g_right[0]), r1=ReadTex(g_right[1]);
    // Refuse aliased objects/backings: a private history must really be private.
    const bool distinct=n0.backing && n1.backing && r0.backing && r1.backing &&
        r0.backing!=r1.backing && r0.backing!=n0.backing && r0.backing!=n1.backing &&
        r1.backing!=n0.backing && r1.backing!=n1.backing;
    if(!prepared || !distinct) {
        g_fail.fetch_add(1,std::memory_order_relaxed);
        if(g_volumesOriginal) g_volumesOriginal(stage); return;
    }
    if(g_rightLastStage!=st) { g_rightLastStage=st; g_rightLastReflectionFrame=-1; }
    // Native +D530 suppresses previous-slot clear on consecutive frames. Sharing
    // it between eyes skips initialization of RIGHT's freshly allocated history.
    if(!Wr(registry+kRefl0,g_right[0]) || !Wr(registry+kRefl1,g_right[1]) ||
       !Wr(st+kLastReflectionFrame,g_rightLastReflectionFrame)) {
        (void)Wr(registry+kRefl0,native[0]); (void)Wr(registry+kRefl1,native[1]);
        (void)Wr(st+kLastReflectionFrame,nativeLast);
        g_fail.fetch_add(1,std::memory_order_relaxed);
        if(g_volumesOriginal) g_volumesOriginal(stage); return;
    }
    if(g_volumesOriginal) g_volumesOriginal(stage);
    const bool saved=Rd(st+kLastReflectionFrame,g_rightLastReflectionFrame);
    // All restores run, even when an earlier one fails.
    const bool restored0=Wr(registry+kRefl0,native[0]);
    const bool restored1=Wr(registry+kRefl1,native[1]);
    const bool restoredLast=Wr(st+kLastReflectionFrame,nativeLast);
    if(!saved || !restored0 || !restored1 || !restoredLast)
        g_fail.fetch_add(1,std::memory_order_relaxed);
    const auto n=g_volumeSwaps.fetch_add(1,std::memory_order_relaxed)+1;
    if(n<=4 || n%600==0) {
        char m[320]{}; std::snprintf(m,sizeof(m),
            "V66.124R2-WATER-PRODUCER frame=%u scopeCalls={L:%llu R:%llu} scopes=%llu lastRight=%d restore=%d privateBacking=1 scope=REFLECTION-GENERATION+MIPS+WATER-SURFACES",
            frame,(unsigned long long)g_volumeCalls[0].load(),(unsigned long long)g_volumeCalls[1].load(),
            (unsigned long long)n,g_rightLastReflectionFrame,restored0&&restored1&&restoredLast?1:0);
        diagnostic_trace::Write(m);
    }
}

} // namespace

bool Install() noexcept {
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!Exact(b)) { diagnostic_trace::Write("V66.63-WATER-REFL install REFUSED exact retail contract mismatch"); return false; }
    g_provision=reinterpret_cast<ProvisionFn>(b+kProvisionRva);
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6663_water_reflection_right",DetourModKit::hook::Target{DetourModKit::Address{b+kExecuteRva}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&Detour);
    if(!h) { diagnostic_trace::Write("V66.63-WATER-REFL install FAILED hook-create"); return false; }
    g_hook=std::move(*h); g_original=g_hook->original<ExecFn>();
    if(!g_original||!g_hook->enable()) { g_original=nullptr; g_hook.reset(); diagnostic_trace::Write("V66.63-WATER-REFL install FAILED hook-enable"); return false; }
    DetourModKit::hook::InlineRequest vr{"kcd2vr_v66124r2_water_volumes",DetourModKit::hook::Target{DetourModKit::Address{b+kVolumesRva}},DetourModKit::hook::Options{}};
    auto vh=DetourModKit::hook::inline_at(std::move(vr),&VolumesDetour);
    if(vh) { g_volumesHook=std::move(*vh); g_volumesOriginal=g_volumesHook->original<VolumesFn>(); }
    if(!g_volumesOriginal || !g_volumesHook->enable()) {
        g_volumesOriginal=nullptr; g_volumesHook.reset();
        (void)g_hook->disable(); g_hook.reset(); g_original=nullptr; g_provision=nullptr;
        diagnostic_trace::Write("V66.124R2-WATER-PRODUCER install FAILED; incomplete private-history repair disabled"); return false;
    }
    diagnostic_trace::Write("V66.124R2-WATER-PRODUCER installed exact SHA-qualified water setup + generation/surface scopes; independent RIGHT last-generation frame; no extra render");
    g_installed.store(true,std::memory_order_release);
    diagnostic_trace::Write("V66.63-WATER-REFL installed: CWaterStage RIGHT gets private $WaterVolumeRefl_R/$WaterVolumeReflPrev_R A/B history; native pass count unchanged");
    return true;
}
bool Installed() noexcept { return g_installed.load(std::memory_order_acquire); }

} // namespace kcd2vr::native_water_reflection_right_v6663
