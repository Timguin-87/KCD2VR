#include "native_vr_control_plane_lab.h"
#include "debug_trace.h"
#include "diagnostic_trace.h"

#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <intrin.h>
#include <atomic>
#include <array>
#include <optional>
#include <utility>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdarg>
#include <cstring>

namespace kcd2vr::native_vr_control_plane_lab {
namespace {

constexpr std::uint32_t kSupportedTimestamp = 0x6A350E20u;
constexpr std::uintptr_t kThreeMonConsumerRva = 0x254DE70ull;
constexpr std::uintptr_t kThreeMonMathConsumerRva = 0x2557764ull;
constexpr std::uintptr_t kThreeMonRegisterRootRva = 0x743ED0ull;
constexpr std::uintptr_t kVrProjectionRegisterRootRva = 0x1011768ull;
constexpr std::uintptr_t kOculusTouchActionMapRootRva = 0x3573BFCull;
constexpr std::uintptr_t kDemoHmdRegisterRootRva = 0x100F9A0ull;
constexpr std::uintptr_t kStereoNodeAutoRegRva = 0x19E668Cull;
constexpr std::uintptr_t kReadStereoNodeAutoRegRva = 0x19E6D94ull;
// V65.25 offline exact-WHGame xrefs into StereoParameters/ReadStereoParameters port metadata.
constexpr std::uintptr_t kStereoEyeDistancePortSiteA = 0x36661E8ull;
constexpr std::uintptr_t kStereoEyeDistancePortSiteB = 0x366A0FDull;
constexpr std::uintptr_t kStereoCurrentEyeDistancePortSite = 0x366A401ull;
// V65.28 exact RTTI/vtable recovery from the supplied retail DLL. These are the
// runtime FlowNode ProcessEvent implementations, not registration/config stubs.
constexpr std::uintptr_t kReadStereoProcessEventRva = 0x368BA1Cull;
constexpr std::uintptr_t kStereoProcessEventRva     = 0x368CF9Cull;

// Previously recovered live renderer stereo backing stores. These reads are
// observation-only and are useful as control-plane witnesses while the old
// public r_StereoMode frontend is absent.
constexpr std::uintptr_t kStereoScaleCoefficientStorageRva = 0x50DBDECull;
constexpr std::uintptr_t kStereoStrengthStorageRva = 0x50DBDE8ull;
constexpr std::uintptr_t kStereoEyeDistStorageRva = 0x50DBDCCull;
constexpr std::uintptr_t kStereoScreenDistStorageRva = 0x50DBDC8ull;
constexpr std::uintptr_t kStereoNearGeoScaleStorageRva = 0x50DBDC4ull;
constexpr std::uintptr_t kStereoHudScreenDistStorageRva = 0x50DBDC0ull;
constexpr std::uintptr_t kStereoGammaAdjustmentStorageRva = 0x50DBDD4ull;
constexpr std::uintptr_t kStereoEnableMgpuStorageRva = 0x5571030ull;
constexpr std::uintptr_t kStereoFlipEyesStorageRva = 0x5571034ull;

constexpr unsigned char kThreeMonConsumerPrologue[] = {
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x57,0x48,0x83,0xEC,0x60
};

struct Target {
    const char* label;
    const char* text;
    std::uintptr_t expected_rva; // 0 means discover only.
};

constexpr Target kTargets[] = {
    {"3MON", "r_3MonHack", 0x3D26770ull},
    {"3MON_RIGHT", "r_3MonHackRightCGFOffsetX", 0x3D31FD0ull},
    {"VR_PROJ_TYPE", "r_VrProjectionType", 0x3DD9500ull},
    {"VR_PROJ_PRESET", "r_VrProjectionPreset", 0x3DD95D0ull},
    {"OCULUS_TOUCH", "oculustouch", 0x4779790ull},
    {"DEMO_HMD_ROT", "demo_use_hmd_rotation", 0x405D530ull},
    {"STEREO_NODE", "Stereo:StereoParameters", 0x3D1C9D0ull},
    {"READ_STEREO_NODE", "Stereo:ReadStereoParameters", 0x3D1D418ull},
    {"CURRENT_EYE_DISTANCE", "CurrentEyeDistance", 0x47872E8ull},
    {"EYE_DISTANCE", "EyeDistance", 0x4787298ull},
    {"OCULUS", "Oculus", 0},
    {"HTC_VIVE", "HTC Vive", 0},
    {"STEREO_HUD_DIST", "r_StereoHudScreenDist", 0},
    {"STEREO_EYE_DIST", "r_StereoEyeDist", 0},
    {"PS5PAD", "ps5pad", 0},
};

using ThreeMonConsumerFn = void(__fastcall*)(void* self);
using FlowNodeProcessEventFn = void(__fastcall*)(void* self, int event, void* activationInfo);
std::optional<DetourModKit::hook::Hook> g_three_mon_hook;
std::optional<DetourModKit::hook::Hook> g_read_stereo_process_hook;
std::optional<DetourModKit::hook::Hook> g_stereo_process_hook;
ThreeMonConsumerFn g_three_mon_original = nullptr;
FlowNodeProcessEventFn g_read_stereo_process_original = nullptr;
FlowNodeProcessEventFn g_stereo_process_original = nullptr;
std::atomic<int> g_mode{0};
std::atomic<bool> g_supported{false};
std::atomic<bool> g_three_mon_installed{false};
std::atomic<std::uint64_t> g_three_mon_calls{0};
std::atomic<std::uintptr_t> g_three_mon_last_self{0};
std::atomic<std::uintptr_t> g_three_mon_last_caller{0};
std::atomic<float> g_m1820{0},g_m1834{0},g_m1840{0},g_m1850{0};
std::atomic<bool> g_flow_process_hooks_installed{false};
std::atomic<std::uint64_t> g_read_process_calls{0},g_stereo_process_calls{0};
std::atomic<int> g_read_last_event{-1},g_stereo_last_event{-1};
std::atomic<std::uintptr_t> g_read_last_self{0},g_stereo_last_self{0};
std::atomic<std::uintptr_t> g_read_last_activation{0},g_stereo_last_activation{0};
std::atomic<std::uint64_t> g_read_last_log_ms{0},g_stereo_last_log_ms{0};
std::atomic<std::uint64_t> g_last_tick_ms{0};
std::atomic<float> g_openxr_ipd{0.064f};
std::atomic<bool> g_bridge_active{false};
float g_bridge_original_eye_dist=0.0f;
bool g_bridge_original_valid=false;


std::uintptr_t Base() noexcept { return reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")); }

bool Readable(const void* p,std::size_t n) noexcept {
    if(!p || !n) return false;
    MEMORY_BASIC_INFORMATION mbi{};
    if(!VirtualQuery(p,&mbi,sizeof(mbi)) || mbi.State!=MEM_COMMIT || (mbi.Protect&PAGE_GUARD) || mbi.Protect==PAGE_NOACCESS) return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p), e=a+n, r=reinterpret_cast<std::uintptr_t>(mbi.BaseAddress)+mbi.RegionSize;
    return e>=a && e<=r;
}

template<class T> bool SafeRead(std::uintptr_t addr,T& out) noexcept {
    if(!Readable(reinterpret_cast<const void*>(addr),sizeof(T))) return false;
    __try { out=*reinterpret_cast<const T*>(addr); return true; } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

template<class T> bool SafeWrite(std::uintptr_t addr,const T& value) noexcept {
    if(!Readable(reinterpret_cast<const void*>(addr),sizeof(T))) return false;
    __try { *reinterpret_cast<T*>(addr)=value; return true; } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool GetImageRange(std::uintptr_t& base,std::size_t& size,std::uint32_t& timestamp) noexcept {
    base=Base(); size=0; timestamp=0; if(!base) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        if(nt->Signature!=IMAGE_NT_SIGNATURE) return false;
        size=nt->OptionalHeader.SizeOfImage; timestamp=nt->FileHeader.TimeDateStamp; return size>0;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool IsExecutable(std::uintptr_t p) noexcept {
    MEMORY_BASIC_INFORMATION mbi{}; if(!p || !VirtualQuery(reinterpret_cast<void*>(p),&mbi,sizeof(mbi))) return false;
    const DWORD x=mbi.Protect & 0xffu;
    return mbi.State==MEM_COMMIT && (x==PAGE_EXECUTE || x==PAGE_EXECUTE_READ || x==PAGE_EXECUTE_READWRITE || x==PAGE_EXECUTE_WRITECOPY);
}

std::uintptr_t FindAscii(const char* s) noexcept {
    std::uintptr_t base{}; std::size_t size{}; std::uint32_t ts{}; if(!GetImageRange(base,size,ts)||!s||!*s) return 0;
    const std::size_t n=std::strlen(s);
    __try {
        const auto* b=reinterpret_cast<const unsigned char*>(base);
        for(std::size_t i=0;i+n<=size;++i) if(b[i]==static_cast<unsigned char>(s[0]) && std::memcmp(b+i,s,n)==0) return base+i;
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
    return 0;
}

std::uintptr_t ResolveTarget(const Target& t) noexcept {
    const auto base=Base();
    if(base && t.expected_rva) {
        const auto p=base+t.expected_rva; const auto n=std::strlen(t.text);
        if(Readable(reinterpret_cast<const void*>(p),n)) {
            __try { if(std::memcmp(reinterpret_cast<const void*>(p),t.text,n)==0) return p; } __except(EXCEPTION_EXECUTE_HANDLER) {}
        }
    }
    return FindAscii(t.text);
}

void SnapshotStereoState(const char* why) noexcept;

void Trace(const char* fmt,...) noexcept {
    char b[1024]{}; va_list ap; va_start(ap,fmt); std::vsnprintf(b,sizeof(b),fmt,ap); va_end(ap);
    diagnostic_trace::Write(b); debug_trace::Write(b);
}


// V65.30 startup-time stereo archaeology. CryEngine historically constructed
// the stereo device/renderer during renderer initialization, so runtime CVar
// writes can be too late. These probes are strictly READ-ONLY: they inspect
// known renderer/frame globals and nearby polymorphic child objects without
// calling any unknown virtual functions.
constexpr std::uintptr_t kFrameNexusGlobalRva = 0x492D808ull;
constexpr std::uintptr_t kRendererGlobalsRva  = 0x547DB60ull;

bool SafePtr(std::uintptr_t addr,std::uintptr_t& out) noexcept { return SafeRead(addr,out) && out!=0; }

void DumpStartupStereoFrontEnd() noexcept {
    Trace("V65.30-STARTUP-STEREO frontend census begin (presence only; no CVar writes)");
    const char* names[]={
        "sys_vr_support","r_StereoMode","r_StereoDevice","r_StereoOutput",
        "r_overrideDXGIOutput","IStereoRenderer","StereoRenderer",
        "r_StereoEyeDist","r_StereoStrength","r_StereoScreenDist",
        "demo_use_hmd_rotation","Oculus","oculustouch"};
    const auto b=Base();
    for(const char* n:names){
        const auto p=FindAscii(n);
        Trace("V65.30-STARTUP-STEREO STRING %-24s present=%d rva=+0x%llX",n,p?1:0,(unsigned long long)(p&&b?p-b:0));
    }
    SnapshotStereoState("V65.30-startup-front-end");
}

void DumpObjectVtable(const char* label,std::uintptr_t obj,std::size_t max_slots=160) noexcept {
    if(!obj){ Trace("V65.30-STARTUP-STEREO OBJECT %s null",label); return; }
    std::uintptr_t vt=0;
    if(!SafePtr(obj,vt)){ Trace("V65.30-STARTUP-STEREO OBJECT %s obj=%p vtable=UNREADABLE",label,(void*)obj); return; }
    const auto b=Base();
    Trace("V65.30-STARTUP-STEREO OBJECT %s obj=%p vtable=%p vtableRva=+0x%llX",label,(void*)obj,(void*)vt,(unsigned long long)(b&&vt>=b?vt-b:0));
    std::size_t exec_count=0;
    for(std::size_t i=0;i<max_slots;++i){
        std::uintptr_t fn=0; if(!SafeRead(vt+i*sizeof(void*),fn)) break;
        if(!IsExecutable(fn)) continue;
        ++exec_count;
        if(i<64 || (i%8)==0)
            Trace("V65.30-STARTUP-STEREO VSLOT %s slot=0x%llX target=+0x%llX",label,(unsigned long long)(i*sizeof(void*)),(unsigned long long)(b&&fn>=b?fn-b:0));
    }
    Trace("V65.30-STARTUP-STEREO OBJECT %s executableSlots=%llu scanned=%llu",label,(unsigned long long)exec_count,(unsigned long long)max_slots);
}

void DumpRendererStartupObjects() noexcept {
    const auto b=Base(); if(!b) return;
    std::uintptr_t frameObj=0, rendererGlobals=0;
    SafeRead(b+kFrameNexusGlobalRva,frameObj);
    SafeRead(b+kRendererGlobalsRva,rendererGlobals);
    Trace("V65.30-STARTUP-STEREO GLOBALS frameNexusGlobal=+0x%llX->%p rendererGlobals=+0x%llX->%p",
          (unsigned long long)kFrameNexusGlobalRva,(void*)frameObj,(unsigned long long)kRendererGlobalsRva,(void*)rendererGlobals);
    DumpObjectVtable("FRAME_NEXUS_OBJECT",frameObj,192);
    DumpObjectVtable("RENDERER_GLOBALS_OBJECT",rendererGlobals,192);
}

void DumpChildPolymorphicCensus(std::uintptr_t obj,const char* label) noexcept {
    if(!obj) return;
    const auto b=Base();
    Trace("V65.30-STARTUP-STEREO CHILD-CENSUS root=%s obj=%p range=0x0..0x1000",label,(void*)obj);
    unsigned hits=0;
    for(std::uintptr_t off=0;off<0x1000;off+=sizeof(void*)){
        std::uintptr_t child=0,vt=0,fn0=0;
        if(!SafeRead(obj+off,child) || !child || child==obj) continue;
        if(!SafeRead(child,vt) || !vt) continue;
        if(!SafeRead(vt,fn0) || !IsExecutable(fn0)) continue;
        if(b && vt>=b && vt<b+0x6000000ull){
            Trace("V65.30-STARTUP-STEREO CHILD root=%s off=0x%llX child=%p vtRva=+0x%llX fn0=+0x%llX",
                  label,(unsigned long long)off,(void*)child,(unsigned long long)(vt-b),(unsigned long long)(fn0>=b?fn0-b:0));
            if(++hits>=96) break;
        }
    }
    Trace("V65.30-STARTUP-STEREO CHILD-CENSUS root=%s hits=%u",label,hits);
}

void DumpRendererChildObjects() noexcept {
    const auto b=Base(); if(!b) return;
    std::uintptr_t frameObj=0,rendererGlobals=0;
    SafeRead(b+kFrameNexusGlobalRva,frameObj);
    SafeRead(b+kRendererGlobalsRva,rendererGlobals);
    DumpChildPolymorphicCensus(frameObj,"FRAME_NEXUS_OBJECT");
    DumpChildPolymorphicCensus(rendererGlobals,"RENDERER_GLOBALS_OBJECT");
}
void DumpStringCensus() noexcept {
    const auto base=Base();
    Trace("V65.21-NATIVEVR R1 SURVIVOR-CENSUS begin");
    for(const auto& t:kTargets) {
        const auto p=ResolveTarget(t); const auto rva=p&&base?p-base:0;
        Trace("V65.21-NATIVEVR SURVIVOR %-20s present=%d rva=+0x%llX expected=+0x%llX exact=%d text=%s",
              t.label,p?1:0,(unsigned long long)rva,(unsigned long long)t.expected_rva,
              (t.expected_rva==0 || rva==t.expected_rva)?1:0,t.text);
    }
    Trace("V65.21-NATIVEVR ROOTS 3monReg=+0x%llX 3monConsumer=+0x%llX 3monMath=+0x%llX vrProjectionReg=+0x%llX oculusTouchActionMap=+0x%llX demoHmdReg=+0x%llX stereoNodeAutoReg=+0x%llX readStereoNodeAutoReg=+0x%llX",
          (unsigned long long)kThreeMonRegisterRootRva,(unsigned long long)kThreeMonConsumerRva,(unsigned long long)kThreeMonMathConsumerRva,
          (unsigned long long)kVrProjectionRegisterRootRva,(unsigned long long)kOculusTouchActionMapRootRva,(unsigned long long)kDemoHmdRegisterRootRva,
          (unsigned long long)kStereoNodeAutoRegRva,(unsigned long long)kReadStereoNodeAutoRegRva);
}

// Find direct RIP-relative LEA/MOV references to a target. This is deliberately
// a tiny decoder for the two instruction forms used by the recovered anchors;
// it never executes or patches the referenced code.




void DumpFlowNodeImplementationCandidates() noexcept {
    const auto b=Base(); if(!b) return;
    struct Site { const char* name; std::uintptr_t rva; } sites[]={
        {"STEREO_EYEDIST_PORT_A",kStereoEyeDistancePortSiteA},
        {"STEREO_EYEDIST_PORT_B",kStereoEyeDistancePortSiteB},
        {"READ_CURRENT_EYEDIST_PORT",kStereoCurrentEyeDistancePortSite},
    };
    Trace("V65.25-NATIVEVR FLOWNODE-CANDIDATES begin: these are NEW executable xrefs to EyeDistance/CurrentEyeDistance, not autoreg stubs");
    for(const auto& s:sites){
        unsigned char bytes[48]{}; bool ok=false;
        if(Readable(reinterpret_cast<void*>(b+s.rva-16),sizeof(bytes))){
            __try { std::memcpy(bytes,reinterpret_cast<void*>(b+s.rva-16),sizeof(bytes)); ok=true; } __except(EXCEPTION_EXECUTE_HANDLER){}
        }
        char hex[48*3+1]{}; std::size_t pos=0;
        if(ok) for(unsigned char c:bytes) pos+=std::snprintf(hex+pos,sizeof(hex)-pos,"%02X ",c);
        Trace("V65.25-NATIVEVR FLOWNODE %-28s rva=+0x%llX exec=%d bytes[-16..+31]=%s",s.name,(unsigned long long)s.rva,IsExecutable(b+s.rva)?1:0,ok?hex:"UNREADABLE");
    }
    Trace("V65.25-NATIVEVR FLOWNODE next-target=identify enclosing ctor/config + vtable/ProcessEvent; ZERO WRITES");
}

void SnapshotStereoState(const char* why) noexcept {
    const auto b=Base(); if(!b) return;
    float scale{},strength{},eye{},screen{},nearGeo{},hud{},gamma{}; std::uint32_t mgpu{},flip{};
    const bool ok=SafeRead(b+kStereoScaleCoefficientStorageRva,scale)&SafeRead(b+kStereoStrengthStorageRva,strength)&SafeRead(b+kStereoEyeDistStorageRva,eye)&SafeRead(b+kStereoScreenDistStorageRva,screen)&SafeRead(b+kStereoNearGeoScaleStorageRva,nearGeo)&SafeRead(b+kStereoHudScreenDistStorageRva,hud)&SafeRead(b+kStereoGammaAdjustmentStorageRva,gamma)&SafeRead(b+kStereoEnableMgpuStorageRva,mgpu)&SafeRead(b+kStereoFlipEyesStorageRva,flip);
    Trace("V65.21-NATIVEVR STATE why=%s ok=%d scale=%.6f strength=%.6f eyeDist=%.6f screenDist=%.6f nearGeo=%.6f hudScreenDist=%.6f gamma=%.6f mgpu=%u flip=%u 3monCalls=%llu lastSelf=0x%llX caller=+0x%llX m=[%.6f,%.6f,%.6f,%.6f]",
          why,ok?1:0,double(scale),double(strength),double(eye),double(screen),double(nearGeo),double(hud),double(gamma),mgpu,flip,
          (unsigned long long)g_three_mon_calls.load(),(unsigned long long)g_three_mon_last_self.load(),(unsigned long long)g_three_mon_last_caller.load(),
          double(g_m1820.load()),double(g_m1834.load()),double(g_m1840.load()),double(g_m1850.load()));
}






void DisableFlowProcessHooks() noexcept {
    if(g_read_stereo_process_hook){(void)g_read_stereo_process_hook->disable();g_read_stereo_process_hook.reset();}
    if(g_stereo_process_hook){(void)g_stereo_process_hook->disable();g_stereo_process_hook.reset();}
    g_read_stereo_process_original=nullptr;g_stereo_process_original=nullptr;g_flow_process_hooks_installed.store(false,std::memory_order_release);
}



void DisableThreeMonHook() noexcept {
    if(g_three_mon_hook) { (void)g_three_mon_hook->disable(); g_three_mon_hook.reset(); }
    g_three_mon_original=nullptr;
    g_three_mon_installed.store(false,std::memory_order_release);
}

void RestoreBridge() noexcept {
    // V65.25: previous IPD-write experiment is retired. Nothing is written here;
    // clear stale bookkeeping only so F8 is strictly read-only in this build.
    g_bridge_active.store(false,std::memory_order_release);
    g_bridge_original_valid=false;
}


} // namespace

bool Initialize() noexcept {
    std::uintptr_t b{}; std::size_t s{}; std::uint32_t ts{}; if(!GetImageRange(b,s,ts)) return false;
    const bool ok=ts==kSupportedTimestamp; g_supported.store(ok);
    Trace("V65.21-NATIVEVR init base=0x%llX size=0x%llX timestamp=0x%08X supported=%d",(unsigned long long)b,(unsigned long long)s,ts,ok?1:0);
    return ok;
}

void Shutdown() noexcept { RestoreBridge(); DisableFlowProcessHooks(); DisableThreeMonHook(); g_mode.store(0); }


void SetMode(int mode) noexcept {
    mode=std::clamp(mode,0,5);
    // V65.31: startup/static census remains here; the revived frame/render-view
    // observer ladder is owned by native_stereo_outer_lab and is strictly read-only.
    DisableThreeMonHook(); DisableFlowProcessHooks(); RestoreBridge();
    g_mode.store(mode);
    Trace("V65.32-CRV-TYPE F8 mode=%d name=%s WRITE_POLICY=READ-ONLY",mode,ModeName(mode));
    if(mode==1){
        DumpStartupStereoFrontEnd(); DumpStringCensus();
        DumpRendererStartupObjects(); DumpRendererChildObjects();
        DumpFlowNodeImplementationCandidates();
        Trace("V65.32-R1 STATIC-PROOF renderer=CD3D9Renderer vft=+0x3DFC008 GetOrCreateRenderView=+0x43EB60 vslot=0x1E8 CRenderViewVft=+0x3DF25C0 normalCaller=+0x43E9BC type=0 recursionCaller=+0x25254B1 type=1 recursionString=e_RecursionViewDistRatio; type2+ owner still UNKNOWN; shader targets=PostStereo/_RT_SECONDARY_VIEW");
    }
    if(mode==2) Trace("V65.32-R2 FRAME-NEXUS + GetOrCreateRenderView READ-ONLY; watch requestedType!=0 and render-view token epochs");
    if(mode==3) Trace("V65.32-R3 + CAMERA HANDOFF + ENGINE PHASES READ-ONLY; no camera-bank or IPD writes");
    if(mode==4) Trace("V65.32-R4 + SAME-FRAME RTV RESOURCE PROOF READ-ONLY; correlate paired resources with secondary-view activity / PostStereo");
    if(mode==5) Trace("V65.32-R5 + CSystem::Render BOUNDARY READ-ONLY; duplicate render remains LOCKED");
}

void Cycle() noexcept { const int m=g_mode.load(); SetMode(m>=5?0:m+1); }

void Tick() noexcept {
    const int m=g_mode.load(); if(m<=0) return;
    const auto now=static_cast<std::uint64_t>(GetTickCount64()); std::uint64_t last=g_last_tick_ms.load();
    if(now-last<1000 || !g_last_tick_ms.compare_exchange_strong(last,now)) return;
    SnapshotStereoState("F8-read-only-live");
}

int Mode() noexcept { return g_mode.load(); }
const char* ModeName(int m) noexcept {
    switch(m){
    case 0:return "OFF";
    case 1:return "STARTUP+RENDERER+SHADER-BACKEND-CENSUS";
    case 2:return "FRAME-NEXUS + RENDERVIEW OBSERVER";
    case 3:return "+ CAMERA HANDOFF + ENGINE PHASES";
    case 4:return "+ SAME-FRAME RTV / POSTSTEREO RESOURCE PROOF";
    case 5:return "+ CSYSTEM RENDER BOUNDARY (READ-ONLY)";
    default:return "INVALID";
    }
}
Status GetStatus() noexcept {
    Status s{}; s.mode=g_mode.load();s.supported_build=g_supported.load();s.three_mon_hook_installed=false;s.three_mon_calls=g_three_mon_calls.load();s.three_mon_last_self=g_three_mon_last_self.load();s.three_mon_last_caller_rva=g_three_mon_last_caller.load();s.matrix_1820=g_m1820.load();s.matrix_1834=g_m1834.load();s.matrix_1840=g_m1840.load();s.matrix_1850=g_m1850.load();
    s.bridge_active=g_bridge_active.load();s.bridge_openxr_ipd=g_openxr_ipd.load();s.bridge_original_eye_dist=g_bridge_original_eye_dist;
    s.flow_process_hooks_installed=g_flow_process_hooks_installed.load();
    s.read_process_calls=g_read_process_calls.load();s.stereo_process_calls=g_stereo_process_calls.load();
    s.read_last_event=g_read_last_event.load();s.stereo_last_event=g_stereo_last_event.load();
    s.read_last_self=g_read_last_self.load();s.stereo_last_self=g_stereo_last_self.load();
    float cur{}; if(Base()) SafeRead(Base()+kStereoEyeDistStorageRva,cur); s.bridge_written_eye_dist=cur; return s;
}

} // namespace kcd2vr::native_vr_control_plane_lab
