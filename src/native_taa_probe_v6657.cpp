#include "native_taa_probe_v6657.h"
#include "diagnostic_trace.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <optional>
#include <utility>
#include <cstdio>

namespace kcd2vr::native_taa_probe_v6657 { namespace {

constexpr std::uint32_t kTimestamp = 0x6A350E20u;
constexpr std::uintptr_t kTaaPassRva = 0x661DECull;   // called from 0x1807EF20C (PostAA)
constexpr unsigned char kEntry[] = {0x48,0x8B,0xC4,0x48,0x89,0x58,0x18,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56};
constexpr std::uintptr_t kStageCrv = 0x28, kCrvType = 0x14, kCrvFrame = 0x2C, kCrvSel = 0x8EE0, kViewInfo = 0x9B30;
constexpr int kFloats = 192;   // 0x300 bytes of view info
constexpr int kFrames = 8;

using Fn = std::uintptr_t(__fastcall*)(void*,void*,void*);
std::optional<DetourModKit::hook::Hook> g_hook;
Fn g_orig = nullptr;
std::uintptr_t g_base = 0;

// render thread only
float g_v[2][kFrames][kFloats];
std::uint32_t g_frameId[kFrames];
bool g_have[2][kFrames];
// V66.58 extras: real jitter (crv+0xA720, written by CalculateJitter 0x1807C1A7C from the
// global frame counter 0x18547DB20), view-info flags (vi+0x3F4 bit 4 = jitter on, set by the
// camera build 0x180865550), pipeline AA state, and the CCamera used for culling (vi+0x0).
float g_jit[2][kFrames][2];
std::uint32_t g_viFlags[2][kFrames];
std::uint32_t g_globalFrame[2][kFrames];
struct Extra { std::uintptr_t cam=0; float camF[16]{}; std::uint32_t aaMode=0, pattern=0, w=0, h=0; bool ok=false; } g_extra[2];
int g_used = 0;
bool g_collect = false;
std::uint64_t g_nextMs = 0;
std::uint64_t g_calls[2]{};
int g_round = 0;

bool ReadBlock(std::uintptr_t src,float* dst) noexcept {
    __try { std::memcpy(dst,reinterpret_cast<const void*>(src),kFloats*sizeof(float)); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
template<class T> bool Rd(std::uintptr_t a,T& o) noexcept {
    if(a<0x10000) return false;
    __try { o=*reinterpret_cast<const T*>(a); return true; } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

void Analyze() noexcept {
    ++g_round;
    char m[1024]{};
    int n=std::snprintf(m,sizeof(m),"V66.57-TAA-PROBE round=%d frames=",g_round);
    for(int f=0;f<kFrames;++f) n+=std::snprintf(m+n,sizeof(m)-n,"%u%s",g_frameId[f],f+1<kFrames?",":"");
    std::snprintf(m+n,sizeof(m)-n," calls={L:%llu R:%llu} viewInfo=crv+0x9B30",(unsigned long long)g_calls[0],(unsigned long long)g_calls[1]);
    diagnostic_trace::Write(m);
    int printed=0;
    for(int i=0;i<kFloats && printed<16;++i){
        float mn[2]={1e30f,1e30f}, mx[2]={-1e30f,-1e30f};
        bool finite=true;
        for(int e=0;e<2;++e) for(int f=0;f<kFrames;++f){ const float x=g_v[e][f][i]; if(!std::isfinite(x)) finite=false; if(x<mn[e])mn[e]=x; if(x>mx[e])mx[e]=x; }
        if(!finite) continue;
        const float vl=mx[0]-mn[0], vr=mx[1]-mn[1];
        const bool moving=(vl>1e-7f||vr>1e-7f);
        const bool small=(vl<0.02f && vr<0.02f);
        if(!(moving&&small)) continue;
        int k=std::snprintf(m,sizeof(m),"V66.57-TAA-PROBE round=%d off=+0x%03X L=[",g_round,i*4);
        for(int f=0;f<kFrames;++f) k+=std::snprintf(m+k,sizeof(m)-k,"%.6f%s",double(g_v[0][f][i]),f+1<kFrames?" ":"");
        k+=std::snprintf(m+k,sizeof(m)-k,"] R=[");
        for(int f=0;f<kFrames;++f) k+=std::snprintf(m+k,sizeof(m)-k,"%.6f%s",double(g_v[1][f][i]),f+1<kFrames?" ":"");
        std::snprintf(m+k,sizeof(m)-k,"]");
        diagnostic_trace::Write(m); ++printed;
    }
    for(int e=0;e<2;++e){
        int k=std::snprintf(m,sizeof(m),"V66.58-TAA-JITTER round=%d eye=%s jitter(crv+0xA720)=",g_round,e?"R":"L");
        for(int f=0;f<kFrames;++f) k+=std::snprintf(m+k,sizeof(m)-k,"(%.6f,%.6f)",double(g_jit[e][f][0]),double(g_jit[e][f][1]));
        k+=std::snprintf(m+k,sizeof(m)-k," viFlags=");
        for(int f=0;f<kFrames;++f) k+=std::snprintf(m+k,sizeof(m)-k,"%X%s",g_viFlags[e][f],f+1<kFrames?",":"");
        k+=std::snprintf(m+k,sizeof(m)-k," globalFrame=");
        for(int f=0;f<kFrames;++f) k+=std::snprintf(m+k,sizeof(m)-k,"%u%s",g_globalFrame[e][f],f+1<kFrames?",":"");
        const Extra& x=g_extra[e];
        k+=std::snprintf(m+k,sizeof(m)-k," pipe{aaMode:%u pattern:%u w:%u h:%u}",x.aaMode,x.pattern,x.w,x.h);
        if(x.ok) {
            k+=std::snprintf(m+k,sizeof(m)-k," cullCamera=%p fov(+0x30)=%.2fdeg cam[+0x30..+0x6C]=",reinterpret_cast<void*>(x.cam),double(x.camF[0])*57.29577951308232);
            for(int i=0;i<16;++i) k+=std::snprintf(m+k,sizeof(m)-k,"%.4g%s",double(x.camF[i]),i<15?" ":"");
        }
        diagnostic_trace::Write(m);
    }
    if(!printed) {
        char z[200]{}; std::snprintf(z,sizeof(z),"V66.57-TAA-PROBE round=%d NO small frame-to-frame changing values in view info (no visible jitter)",g_round);
        diagnostic_trace::Write(z);
    }
    // Also print the projection-like 4x4 blocks' 3rd row (where jitter usually lives) for frame 0.
    for(int e=0;e<2;++e){
        int k=std::snprintf(m,sizeof(m),"V66.57-TAA-PROBE round=%d eye=%s frame0 floats[0x140..0x2FF]:",g_round,e?"R":"L");
        for(int i=0x140/4;i<kFloats && k<int(sizeof(m))-16;++i) k+=std::snprintf(m+k,sizeof(m)-k," %.4g",double(g_v[e][0][i]));
        diagnostic_trace::Write(m);
    }
}

std::uintptr_t __fastcall Detour(void* stage,void* a2,void* a3) noexcept {
    std::uintptr_t crv=0; std::uint32_t type=~0u, frame=0; std::int32_t sel=-1;
    const auto s=reinterpret_cast<std::uintptr_t>(stage);
    if(Rd(s+kStageCrv,crv) && Rd(crv+kCrvType,type) && type==0 && Rd(crv+kCrvSel,sel) && (sel==0||sel==1) && Rd(crv+kCrvFrame,frame)) {
        ++g_calls[sel];
        const auto now=GetTickCount64();
        if(!g_collect && now>=g_nextMs) { g_collect=true; g_used=0; std::memset(g_have,0,sizeof(g_have)); }
        if(g_collect) {
            int slot=-1;
            for(int f=0;f<g_used;++f) if(g_frameId[f]==frame) { slot=f; break; }
            if(slot<0 && g_used<kFrames && sel==0) { slot=g_used++; g_frameId[slot]=frame; }
            if(slot>=0 && !g_have[sel][slot] && ReadBlock(crv+kViewInfo,g_v[sel][slot])) {
                g_have[sel][slot]=true;
                (void)Rd(crv+0xA720,g_jit[sel][slot][0]); (void)Rd(crv+0xA724,g_jit[sel][slot][1]);
                (void)Rd(crv+kViewInfo+0x3F4,g_viFlags[sel][slot]);
                if(g_base) (void)Rd(g_base+0x547DB20,g_globalFrame[sel][slot]);
                if(slot==0) {
                    Extra& x=g_extra[sel]; x={};
                    std::uintptr_t pipe=0;
                    if(Rd(crv+0x8D60,pipe) && pipe) { (void)Rd(pipe+0x169C,x.aaMode); (void)Rd(pipe+0x16A4,x.pattern); (void)Rd(pipe+0x1988,x.w); (void)Rd(pipe+0x198C,x.h); }
                    if(Rd(crv+kViewInfo,x.cam) && x.cam) { for(int i=0;i<16;++i) (void)Rd(x.cam+0x30+i*4,x.camF[i]); x.ok=true; }
                }
            }
            bool done=g_used==kFrames;
            for(int f=0;f<kFrames&&done;++f) done=g_have[0][f]&&g_have[1][f];
            if(done) { Analyze(); g_collect=false; g_nextMs=now+20000; }
            else if(g_used==kFrames && slot<0 && sel==0) { g_collect=false; g_nextMs=now+2000; } // gap: retry soon
        }
    }
    return g_orig?g_orig(stage,a2,a3):0;
}

bool Exact(std::uintptr_t b) noexcept {
    if(!b) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(b);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(b+dos->e_lfanew);
        if(nt->Signature!=IMAGE_NT_SIGNATURE || nt->FileHeader.TimeDateStamp!=kTimestamp) return false;
        return std::memcmp(reinterpret_cast<const void*>(b+kTaaPassRva),kEntry,sizeof(kEntry))==0;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

} // namespace

bool Install() noexcept {
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!Exact(b)) { diagnostic_trace::Write("V66.57-TAA-PROBE install REFUSED exact WHGame contract mismatch"); return false; }
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6657_taa_probe",DetourModKit::hook::Target{DetourModKit::Address{b+kTaaPassRva}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&Detour);
    if(!h) { diagnostic_trace::Write("V66.57-TAA-PROBE install FAILED hook-create"); return false; }
    g_base=b;
    g_hook=std::move(*h); g_orig=g_hook->original<Fn>();
    if(!g_orig || !g_hook->enable()) { g_orig=nullptr; g_hook.reset(); diagnostic_trace::Write("V66.57-TAA-PROBE install FAILED hook-enable"); return false; }
    diagnostic_trace::Write("V66.57-TAA-PROBE installed (read-only; logs every 20 s while TAA runs)");
    return true;
}

} // namespace kcd2vr::native_taa_probe_v6657
