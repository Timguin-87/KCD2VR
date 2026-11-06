#include "native_stage_bisect_v6644.h"
#include "native_stereo_outer_lab.h"
#include "diagnostic_trace.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <optional>
#include <utility>
#include <cstdio>

namespace kcd2vr::native_stage_bisect_v6644 { namespace {

// Static proof (retail WHGame.dll bdf8f9e4..., timestamp 0x6A350E20):
// stage classes/vtables from RTTI ".?AV<name>@@"; slot 9 = IsStageActive(stage,flags)
// (same slot as CShadowMapStage gate 0xF7516C that natively rejects selector 1),
// slot 4 = virtual Execute(stage). Every gate listed has exactly ONE vtable reference
// (stage-unique) and is called directly by the pipeline before Execute. Only functions
// with >=15 bytes of straight-line code before any branch target are patched.
// Stage+0x28 = CRenderView, selector at +0x8EE0 (0 LEFT, 1 RIGHT).
constexpr std::uint32_t kTimestamp = 0x6A350E20u;
enum Kind : int { Gate=0, Exec=1, External=2 };
struct Entry { const char* name; std::uintptr_t rva; int kind; unsigned char bytes[15]; };
constexpr Entry kEntries[] = {
    {"SHADOW-MASK",0x4EAB1CULL,External,{}}, // skip lives in the existing V66.39 ShadowMask Execute hook
    {"SSO-AO",0x5728DCULL,Gate,{0x48,0x83,0xEC,0x28,0x48,0x8B,0x49,0x20,0x33,0xD2,0x39,0x91,0xE4,0x01,0x00}},
    {"HEIGHTMAP-AO",0x572908ULL,Gate,{0x48,0x83,0xEC,0x28,0x48,0x8B,0x49,0x20,0x33,0xD2,0x39,0x91,0xD4,0x01,0x00}},
    {"SVOGI",0xFF33ECULL,Exec,{0x48,0x89,0x5C,0x24,0x18,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0x41,0x28,0x48}},
    {"VOLUMETRIC-FOG",0x7BB84CULL,Gate,{0x48,0x83,0xEC,0x28,0x4C,0x8B,0xC1,0x48,0x8B,0x49,0x20,0xE8,0xC4,0x1B,0x00}},
    {"FOG",0x7C2CBCULL,Gate,{0x48,0x8B,0x41,0x28,0x33,0xD2,0x38,0x90,0x18,0x8D,0x00,0x00,0x74,0x0E,0x48}},
    {"WATER",0x7C2D0CULL,Gate,{0x0F,0xBA,0xE2,0x0F,0x72,0x0C,0x48,0x8B,0x41,0x28,0x83,0x78,0x14,0x01,0x0F}},
    {"SCENE-FORWARD",0x7BC198ULL,Exec,{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18}},
    {"TILED-LIGHT-VOLUMES",0x10B2078ULL,Exec,{0x40,0x53,0x48,0x83,0xEC,0x40,0x48,0x8B,0xD9,0x48,0x8B,0x49,0x20,0xE8,0x56}},
    {"DEFERRED-DECALS",0x572994ULL,Gate,{0x48,0x8B,0x41,0x20,0x83,0xB8,0x84,0x00,0x00,0x00,0x00,0x0F,0x9F,0xC0,0xC3}},
    {"SCREEN-SPACE-SSS",0x57283CULL,Gate,{0x48,0x8B,0x41,0x20,0x83,0xB8,0x80,0x00,0x00,0x00,0x00,0x0F,0x9F,0xC0,0xC3}},
    {"SUN-SHAFTS",0x57292CULL,Gate,{0x48,0x83,0xEC,0x28,0x48,0x8B,0x49,0x20,0x33,0xD2,0x39,0x91,0x70,0x02,0x00}},
    {"RAIN",0xEA02CCULL,Gate,{0x48,0x8B,0x51,0x20,0x33,0xC0,0x39,0x82,0x84,0x03,0x00,0x00,0x7E,0x12,0x39}},
    {"BLOOM",0x7C2CF0ULL,Gate,{0x48,0x8B,0x51,0x20,0x33,0xC0,0x39,0x82,0xB4,0x00,0x00,0x00,0x74,0x0A,0x39}},
    {"MOTIONBLUR-PREP",0x1E14B50ULL,Gate,{0x48,0x83,0xEC,0x28,0x48,0x8B,0x49,0x20,0xE8,0x17,0x19,0x69,0xFE,0x84,0xC0}},
};
constexpr int kCount = int(sizeof(kEntries)/sizeof(kEntries[0]));
static_assert(kCount<=32,"mask is 32-bit");

using Fn = std::uintptr_t(__fastcall*)(void*,void*,void*,void*);
std::optional<DetourModKit::hook::Hook> g_hooks[kCount];
Fn g_orig[kCount]{};
std::atomic<std::uint32_t> g_mask{0};
std::atomic<std::uint32_t> g_installed{0};
std::atomic<std::uint64_t> g_skips[kCount]{};
std::atomic<std::uint64_t> g_right_calls[kCount]{};

int Selector(void* stage) noexcept {
    if(!stage) return -1;
    __try {
        const auto crv=*reinterpret_cast<const std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(stage)+0x28);
        if(crv<0x10000) return -1;
        return *reinterpret_cast<const std::int32_t*>(crv+0x8EE0);
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

template<int I>
std::uintptr_t __fastcall Detour(void* stage,void* a2,void* a3,void* a4) noexcept {
    if(Selector(stage)==1) {
        g_right_calls[I].fetch_add(1,std::memory_order_relaxed);
        if(g_mask.load(std::memory_order_relaxed)&(1u<<I)) {
            g_skips[I].fetch_add(1,std::memory_order_relaxed);
            return 0; // gate: "inactive"; Execute: do nothing (same as gate false)
        }
    }
    return g_orig[I]?g_orig[I](stage,a2,a3,a4):0;
}
template<int... Is> struct Table { static constexpr Fn fns[sizeof...(Is)]={&Detour<Is>...}; };
using DetourTable = Table<0,1,2,3,4,5,6,7,8,9,10,11,12,13,14>;
static_assert(sizeof(DetourTable::fns)/sizeof(Fn)==kCount,"detour table size");

bool ExactBuild(std::uintptr_t b) noexcept {
    if(!b) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(b);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(b+dos->e_lfanew);
        return nt->Signature==IMAGE_NT_SIGNATURE && nt->FileHeader.TimeDateStamp==kTimestamp;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
bool Matches(std::uintptr_t a,const unsigned char* bytes) noexcept {
    __try { return std::memcmp(reinterpret_cast<const void*>(a),bytes,15)==0; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

} // namespace

bool Install() noexcept {
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!ExactBuild(b)) { diagnostic_trace::Write("V66.44-BISECT install REFUSED exact WHGame mismatch"); return false; }
    std::uint32_t ok=1u; // index 0 (ShadowMask) is handled by the existing outer-lab hook
    for(int i=1;i<kCount;++i) {
        const auto& e=kEntries[i];
        if(!Matches(b+e.rva,e.bytes)) { char m[160]{}; std::snprintf(m,sizeof(m),"V66.44-BISECT skip-install %s entry-bytes-mismatch",e.name); diagnostic_trace::Write(m); continue; }
        char hn[64]{}; std::snprintf(hn,sizeof(hn),"kcd2vr_v6644_bisect_%d",i);
        DetourModKit::hook::InlineRequest req{hn,DetourModKit::hook::Target{DetourModKit::Address{b+e.rva}},DetourModKit::hook::Options{}};
        auto h=DetourModKit::hook::inline_at(std::move(req),DetourTable::fns[i]);
        if(!h) continue;
        g_hooks[i]=std::move(*h);
        g_orig[i]=g_hooks[i]->original<Fn>();
        if(!g_orig[i] || !g_hooks[i]->enable()) { g_orig[i]=nullptr; g_hooks[i].reset(); continue; }
        ok|=(1u<<i);
    }
    g_installed.store(ok,std::memory_order_release);
    char m[160]{}; std::snprintf(m,sizeof(m),"V66.44-BISECT installed mask=0x%05X of %d stages (RIGHT-pass skip only; default none)",unsigned(ok),kCount);
    diagnostic_trace::Write(m);
    return true;
}
int StageCount() noexcept { return kCount; }
const char* StageName(int i) noexcept { return (i>=0&&i<kCount)?kEntries[i].name:"?"; }
void LogStats(const char* reason) noexcept {
    char m[1200]{}; int n=std::snprintf(m,sizeof(m),"V66.44-BISECT-STATS reason=%s mask=0x%05X",reason?reason:"",unsigned(g_mask.load()));
    for(int i=1;i<kCount && n>0 && n<int(sizeof(m))-80;++i) {
        const auto rc=g_right_calls[i].exchange(0,std::memory_order_relaxed);
        const auto sk=g_skips[i].exchange(0,std::memory_order_relaxed);
        if(rc||sk) n+=std::snprintf(m+n,sizeof(m)-n," %s={rightCalls:%llu skipped:%llu}",kEntries[i].name,(unsigned long long)rc,(unsigned long long)sk);
    }
    if(n>0 && n<int(sizeof(m))-80) std::snprintf(m+n,sizeof(m)-n," SHADOW-MASK={skipped:%llu}",(unsigned long long)native_stereo_outer_lab::V6644RightShadowMaskSkips());
    diagnostic_trace::Write(m);
}

} // namespace kcd2vr::native_stage_bisect_v6644
