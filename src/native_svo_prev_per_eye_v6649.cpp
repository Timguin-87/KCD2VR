#include "native_svo_prev_per_eye_v6649.h"
#include "diagnostic_trace.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <optional>
#include <utility>
#include <cstdio>

namespace kcd2vr::native_svo_prev_per_eye_v6649 { namespace {

// ---- Static proof (retail WHGame.dll bdf8f9e4..., timestamp 0x6A350E20) ----
// 0x181DF83A0 = reprojection helper(curView,curProj,..,prevView*,prevProj*). Exactly two callers:
//  A 0x180579D64 (SVOGI constants; rcx=stage, rbx=[rcx+0x28]=CRenderView; uploads
//    SVO_ViewProjPrev from stage+0xA0 and the helper result to stage+0x60):
//      static prevView @0x185679550 (64 B), static prevProj @0x1856795A0 (64 B)
//      (thread-safe static guards 0x185679544 / 0x185679590), overwritten with the current
//      matrices at 0x180579FF8..0x18057A02F right after the helper call.
//  B 0x18057B764 (second SVO path, also references SVO_ViewProjPrev; rbx=[rcx+0x28]):
//      static prevView @0x18549F5E0, static prevProj @0x18549F630 (64 B each),
//      overwritten at 0x18057C526..0x18057C566.
// Both statics live in .data (writable). Neither function reads stack arguments.
constexpr std::uint32_t kTimestamp = 0x6A350E20u;
struct Site {
    const char* name; std::uintptr_t fn; std::uintptr_t view; std::uintptr_t proj; unsigned char entry[16];
};
constexpr Site kSites[2] = {
    {"SVO-A",0x579D64ULL,0x5679550ULL,0x56795A0ULL,{0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56}},
    {"SVO-B",0x57B764ULL,0x549F5E0ULL,0x549F630ULL,{0x48,0x8B,0xC4,0x48,0x89,0x58,0x20,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56}},
};
// Proof that the static stores are really there (movaps [rip+x],xmm13 / xmm6 at the
// exact addresses): first store of view and last store of proj for each site.
struct StoreCheck { std::uintptr_t at; unsigned char bytes[9]; };
constexpr StoreCheck kStores[4] = {
    {0x579FF8ULL,{0x44,0x0F,0x29,0x2D,0x50,0xF5,0x0F,0x05,0x00}}, // movaps [0x185679550],xmm13 (needs 8 bytes)
    {0x57A02FULL,{0x0F,0x29,0x35,0x9A,0xF5,0x0F,0x05,0x00,0x00}}, // movaps [0x1856795D0],xmm6  (7 bytes)
    {0x57C537ULL,{0x44,0x0F,0x29,0x2D,0xA1,0x30,0xF2,0x04,0x00}}, // movaps [0x18549F5E0],xmm13
    {0x57C566ULL,{0x0F,0x29,0x35,0xF3,0x30,0xF2,0x04,0x00,0x00}}, // movaps [0x18549F660],xmm6
};
constexpr std::size_t kStoreLen[4] = {8,7,8,7};

struct EyeState { unsigned char view[64]; unsigned char proj[64]; bool valid=false; };
EyeState g_eye[2][2]{}; // [site][eye] - render thread only

using Fn = std::uintptr_t(__fastcall*)(void*,void*,void*,void*);
std::optional<DetourModKit::hook::Hook> g_hooks[2];
Fn g_orig[2]{};
std::uintptr_t g_base=0;
std::atomic<bool> g_installed{false};
std::atomic<bool> g_enabled{true};
std::atomic<std::uint64_t> g_swaps[2][2]{};
std::atomic<std::uint64_t> g_passthrough[2]{};

int EyeOf(void* stage) noexcept {
    if(!stage) return -1;
    __try {
        const auto crv=*reinterpret_cast<const std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(stage)+0x28);
        if(crv<0x10000) return -1;
        if(*reinterpret_cast<const std::uint32_t*>(crv+0x14)!=0u) return -1; // Normal CRenderView only
        const auto sel=*reinterpret_cast<const std::int32_t*>(crv+0x8EE0);
        return (sel==0||sel==1)?sel:-1;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

template<int S>
std::uintptr_t __fastcall Detour(void* stage,void* a2,void* a3,void* a4) noexcept {
    const int eye=g_enabled.load(std::memory_order_relaxed)?EyeOf(stage):-1;
    auto* view=reinterpret_cast<unsigned char*>(g_base+kSites[S].view);
    auto* proj=reinterpret_cast<unsigned char*>(g_base+kSites[S].proj);
    if(eye<0) { g_passthrough[S].fetch_add(1,std::memory_order_relaxed); return g_orig[S]?g_orig[S](stage,a2,a3,a4):0; }
    auto& st=g_eye[S][eye];
    if(st.valid) { std::memcpy(view,st.view,64); std::memcpy(proj,st.proj,64); } // this eye's own previous camera
    const auto r=g_orig[S]?g_orig[S](stage,a2,a3,a4):0;
    std::memcpy(st.view,view,64); std::memcpy(st.proj,proj,64); st.valid=true;    // now holds this eye's current
    g_swaps[S][eye].fetch_add(1,std::memory_order_relaxed);
    return r;
}

bool Matches(std::uintptr_t a,const unsigned char* b,std::size_t n) noexcept {
    __try { return std::memcmp(reinterpret_cast<const void*>(a),b,n)==0; }
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
    for(const auto& s:kSites) if(!Matches(b+s.fn,s.entry,16)) return false;
    for(int i=0;i<4;++i) if(!Matches(b+kStores[i].at,kStores[i].bytes,kStoreLen[i])) return false;
    return true;
}

} // namespace

bool Install() noexcept {
    if(g_installed.load(std::memory_order_acquire)) return true;
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!ExactBuild(b)) { diagnostic_trace::Write("V66.49-SVO-PREV install REFUSED exact WHGame / static-store contract mismatch"); return false; }
    g_base=b;
    constexpr Fn fns[2]={&Detour<0>,&Detour<1>};
    for(int i=0;i<2;++i){
        char hn[64]{}; std::snprintf(hn,sizeof(hn),"kcd2vr_v6649_svo_prev_%d",i);
        DetourModKit::hook::InlineRequest req{hn,DetourModKit::hook::Target{DetourModKit::Address{b+kSites[i].fn}},DetourModKit::hook::Options{}};
        auto h=DetourModKit::hook::inline_at(std::move(req),fns[i]);
        if(!h){ diagnostic_trace::Write("V66.49-SVO-PREV install FAILED hook-create"); return false; }
        g_hooks[i]=std::move(*h); g_orig[i]=g_hooks[i]->original<Fn>();
        if(!g_orig[i] || !g_hooks[i]->enable()){ diagnostic_trace::Write("V66.49-SVO-PREV install FAILED hook-enable"); return false; }
    }
    g_installed.store(true,std::memory_order_release);
    diagnostic_trace::Write("V66.49-SVO-PREV installed: per-eye previous view/projection for SVO reprojection (2 sites), default ON");
    return true;
}
bool Installed() noexcept { return g_installed.load(std::memory_order_acquire); }
void SetEnabled(bool on) noexcept {
    g_enabled.store(on,std::memory_order_release);
    // Invalidate saved per-eye state so re-enabling starts cleanly (render thread may be mid-call;
    // a stale 'valid' flag only costs one frame of old previous-camera data).
    for(auto& s:g_eye) for(auto& e:s) e.valid=false;
}
bool Enabled() noexcept { return g_enabled.load(std::memory_order_acquire); }
void LogStats() noexcept {
    char m[320]{}; std::snprintf(m,sizeof(m),
        "V66.49-SVO-PREV enabled=%d A={L:%llu R:%llu other:%llu} B={L:%llu R:%llu other:%llu}",
        g_enabled.load()?1:0,
        (unsigned long long)g_swaps[0][0].load(),(unsigned long long)g_swaps[0][1].load(),(unsigned long long)g_passthrough[0].load(),
        (unsigned long long)g_swaps[1][0].load(),(unsigned long long)g_swaps[1][1].load(),(unsigned long long)g_passthrough[1].load());
    diagnostic_trace::Write(m);
}

} // namespace kcd2vr::native_svo_prev_per_eye_v6649
