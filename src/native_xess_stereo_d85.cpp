// D85 UPSCALERS: Windows binding for the XeSS per-eye runtime. Same shape as
// the reviewed D82 FSR binding: hook the game's actual vendor imports, enable
// lifecycle/evaluation first and creation last, never retry a failed hook
// transaction, never initialize a second SDK or device.
#include "native_xess_stereo_d85.h"
#include "xess_stereo_runtime_d85.h"
#include "native_stereo_preflight_v6602.h"
#include "diagnostic_trace.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstring>
#include <cstdio>
#include <optional>
#include <utility>

namespace kcd2vr::native_xess_stereo_d85 { namespace {
namespace rt=xess_stereo_runtime_d85;
rt::Runtime runtime;
std::mutex installMutex;
std::atomic<bool> installed{false};
std::atomic<std::uint64_t> completedPairs{0};
bool attempted=false; // installMutex; a hook transaction is never retried.
std::array<std::optional<DetourModKit::hook::Hook>,5> hooks;
rt::Api originals{};
constexpr const char* kExports[5]{"xessDestroyContext","xessD3D12Execute","xessSetVelocityScale","xessD3D12Init","xessD3D12CreateContext"};
// WHGame 0x6A350E20 import address table slots for libxess.dll (lookup 0x48CAB58,
// IAT 0x3A052C0): DestroyContext +0x00, SetVelocityScale +0x08, Execute +0x18,
// CreateContext +0x28, Init +0x30. Each is called exactly once in the image:
// 0x181E11138, 0x181E00215, 0x181E00009, 0x181E0B774, 0x181E001F6.
constexpr std::uintptr_t kIat[5]{0x3A052C0,0x3A052D8,0x3A052C8,0x3A052F0,0x3A052E8};

bool ExactGame() noexcept {
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));if(!b)return false;
    struct Pin {std::uintptr_t rva;unsigned char bytes[16];};
    // Exact call sites and the execute-block layout witnesses quoted in
    // xess_stereo_runtime_d85.h. The image hash is bdf8f9e4...9479f7.
    static constexpr Pin pins[]{
        {0x1E00009,{0xff,0x15,0xc9,0x52,0xc0,0x01,0x8b,0xc8,0xe8,0xca,0xf1,0xff,0xff,0x84,0xc0,0x74}}, // call [Execute]
        {0x1E001F6,{0xff,0x15,0xf4,0x50,0xc0,0x01,0x85,0xc0,0x74,0x08,0x32,0xc0,0x48,0x83,0xc4,0x60}}, // call [Init]
        {0x1E00215,{0xff,0x15,0xad,0x50,0xc0,0x01,0x8b,0xc8,0xe8,0xbe,0xef,0xff,0xff,0x84,0xc0,0x74}}, // call [SetVelocityScale]
        {0x1E0B774,{0xff,0x15,0x6e,0x9b,0xbf,0x01,0x85,0xc0,0x74,0x1c,0x48,0x8b,0xcb,0xc6,0x83,0x10}}, // call [CreateContext]
        {0x1E11138,{0xff,0x15,0x82,0x41,0xbf,0x01,0x48,0xc7,0x83,0x18,0x07,0x00,0x00,0x00,0x00,0x00}}, // call [DestroyContext]; clear +0x718
        {0x1DFFF8E,{0x41,0x0f,0xb6,0x46,0x48,0x89,0x44,0x24,0x6c,0x48,0x8b,0x84,0x24,0x18,0x01,0x00}}, // reset -> block+0x3C
        {0x1DFFFC3,{0x0f,0x11,0x44,0x24,0x78,0x48,0x89,0x44,0x24,0x58,0x0f,0x11,0x84,0x24,0x88,0x00}}, // zero block+0x48..
    };
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(b);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE||dos->e_lfanew<0||dos->e_lfanew>0x10000)return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(b+dos->e_lfanew);
        if(!(nt->Signature==IMAGE_NT_SIGNATURE&&nt->FileHeader.Machine==IMAGE_FILE_MACHINE_AMD64
            &&nt->FileHeader.TimeDateStamp==0x6A350E20u
            &&nt->OptionalHeader.Magic==IMAGE_NT_OPTIONAL_HDR64_MAGIC&&nt->OptionalHeader.SizeOfImage==0x5B2D000u))return false;
        for(const auto& p:pins)if(std::memcmp(reinterpret_cast<const void*>(b+p.rva),p.bytes,sizeof(p.bytes)))return false;
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// The game's bound import slot must point at the export we hook; otherwise a
// proxy/overlay owns the call and we would hook the wrong function.
bool BoundImport(unsigned i,void* exported) noexcept {
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));if(!b||!exported)return false;
    __try {return *reinterpret_cast<void* const*>(b+kIat[i])==exported;} __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
rt::Result Create(void* device,rt::Context* out) noexcept {return runtime.Create(device,out);}
rt::Result Init(rt::Context c,const rt::InitParams* p) noexcept {return runtime.Init(c,p);}
rt::Result Velocity(rt::Context c,float x,float y) noexcept {return runtime.SetVelocityScale(c,x,y);}
rt::Result Destroy(rt::Context c) noexcept {return runtime.Destroy(c);}
rt::Result Execute(rt::Context c,void* list,const rt::ExecuteParams* p) noexcept {
    rt::View v{};v.active=native_stereo_preflight::InUpscalerView();
    native_stereo_preflight::UpscalerView native{};
    if(v.active&&native_stereo_preflight::CurrentUpscalerView(native)){
        v.valid=true;v.eye=native.eye;v.epoch=native.epoch;v.frame=native.frame;v.pipeline=native.pipeline;
    }
    const auto result=runtime.Execute(c,list,p,v);
    if(v.valid&&v.eye==1&&result==rt::kOk&&runtime.PairReady(v.epoch,v.frame)){
        const auto n=completedPairs.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=3||n%1024==0){
            const DWORD nativeError=GetLastError();
            char line[256]{};std::snprintf(line,sizeof(line),
                "D85 XESS_STEREO evaluated epoch=%llu frame=%u pairs=%llu bothEyes=1 histories=SEPARATE nativeInputs=PRESERVED GPU_CONTENT_UNVERIFIED",
                static_cast<unsigned long long>(v.epoch),v.frame,static_cast<unsigned long long>(n));
            diagnostic_trace::Write(line);
            SetLastError(nativeError);
        }
    }
    return result;
}
template<class Fn> bool Make(unsigned i,HMODULE module,const char* name,Fn detour,Fn& original) noexcept {
    auto address=GetProcAddress(module,name);if(!address||!BoundImport(i,reinterpret_cast<void*>(address)))return false;
    DetourModKit::hook::InlineRequest request{name,DetourModKit::hook::Target{DetourModKit::Address{reinterpret_cast<std::uintptr_t>(address)}},DetourModKit::hook::Options{}};
    auto result=DetourModKit::hook::inline_at(std::move(request),detour);if(!result)return false;
    hooks[i]=std::move(*result);original=hooks[i]->original<Fn>();return original!=nullptr;
}
}
bool Install() noexcept {
    std::lock_guard lock(installMutex);if(installed.load(std::memory_order_acquire))return true;
    if(attempted)return false;
    const auto module=GetModuleHandleW(L"libxess.dll");
    if(!module||!GetModuleHandleW(L"WHGame.dll"))return false;
    attempted=true;
    if(!ExactGame()){
        diagnostic_trace::Write("D85 XESS_STEREO install=REFUSED reason=retail-image");return false;
    }
    // Every trampoline exists before any wrapper is enabled.
    const bool made=Make(0,module,kExports[0],static_cast<rt::PfnDestroy>(&Destroy),originals.destroy)
        &&Make(1,module,kExports[1],static_cast<rt::PfnExecute>(&Execute),originals.execute)
        &&Make(2,module,kExports[2],static_cast<rt::PfnVelocityScale>(&Velocity),originals.velocity)
        &&Make(3,module,kExports[3],static_cast<rt::PfnInit>(&Init),originals.init)
        &&Make(4,module,kExports[4],static_cast<rt::PfnCreate>(&Create),originals.create);
    if(!made){
        diagnostic_trace::Write("D85 XESS_STEREO install=FAILED reason=hook-construction-or-import-binding");return false;
    }
    runtime.SetApi(originals);
    // Destroy, execute, velocity and init before create: no twin can exist
    // while a path that must mirror or release it is still unhooked.
    for(auto& h:hooks)if(!h->enable()){
        for(auto& old:hooks)if(old)(void)old->disable();
        // Keep trampolines and originals until process exit; a racing native
        // pass-through may still be returning. Create is enabled last.
        diagnostic_trace::Write("D85 XESS_STEREO install=FAILED reason=hook-enable stereoPair=UNAVAILABLE");return false;
    }
    installed.store(true,std::memory_order_release);
    diagnostic_trace::Write("D85 XESS_STEREO installed=1 api=LIBXESS_D3D12 histories=INDEPENDENT_CONTEXTS nativeResources=PRESERVED validation=CPU_ROUTING_NOT_HEADSET");
    return true;
}
bool Installed() noexcept {return installed.load(std::memory_order_acquire);}
void SetEnabled(bool enabled) noexcept {runtime.SetEnabled(enabled);}
bool ReadyForStereo() noexcept {return Installed()&&runtime.Ready();}
bool PairReady(std::uint64_t epoch,std::uint32_t frame) noexcept {return Installed()&&runtime.PairReady(epoch,frame);}
void Stop() noexcept {runtime.SetEnabled(false);} // native lifecycle owns destruction
void LogStatus(const char* reason) noexcept {
    const auto s=runtime.GetStatus();char line[512]{};
    std::snprintf(line,sizeof(line),"D85 XESS_STEREO reason=%s installed=%d enabled=%d livePairs=%u created=%llu twins=%llu inits=%llu evaluatedL=%llu evaluatedR=%llu refused=%llu destroyedTwins=%llu",
        reason?reason:"status",Installed(),s.enabled,s.live,static_cast<unsigned long long>(s.creates),static_cast<unsigned long long>(s.twins),
        static_cast<unsigned long long>(s.inits),static_cast<unsigned long long>(s.left),static_cast<unsigned long long>(s.right),
        static_cast<unsigned long long>(s.refused),static_cast<unsigned long long>(s.destroyed));
    diagnostic_trace::Write(line);
}
}
