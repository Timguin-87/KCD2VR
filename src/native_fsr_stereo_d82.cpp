#include "native_fsr_stereo_d82.h"
#include "fsr_stereo_runtime_d82.h"
#include "native_stereo_preflight_v6602.h"
#include "diagnostic_trace.h"
#include "diagnostics/capture168.h"
#include <d3d12.h>
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstring>
#include <cstdio>
#include <optional>
#include <utility>

namespace kcd2vr::native_fsr_stereo_d82 { namespace {
namespace rt=fsr_stereo_runtime_d82;
rt::Runtime runtime;
std::mutex installMutex;
std::atomic<bool> installed{false};
std::atomic<std::uint64_t> completedPairs{0};
bool attempted=false; // Protected by installMutex; a hook transaction is never retried.
std::array<std::optional<DetourModKit::hook::Hook>,4> hooks;
rt::Api originals{};

bool ExactGame() noexcept {
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));if(!b)return false;
    // ABI witnesses from bdf8f9e4...9479f7: native create uses type 0x10000
    // and dispatch uses type 0x10001 with reset at +0x198.
    constexpr unsigned char create[]={0x4c,0x8b,0xdc,0x48,0x81,0xec,0x88,0,0,0,0x8b,0x42,0x08};
    constexpr unsigned char dispatch[]={0x48,0x89,0x5c,0x24,0x18,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57};
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(b);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE||dos->e_lfanew<0||dos->e_lfanew>0x10000)return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(b+dos->e_lfanew);
        return nt->Signature==IMAGE_NT_SIGNATURE&&nt->FileHeader.Machine==IMAGE_FILE_MACHINE_AMD64
            &&nt->FileHeader.TimeDateStamp==0x6A350E20u
            &&nt->OptionalHeader.Magic==IMAGE_NT_OPTIONAL_HDR64_MAGIC&&nt->OptionalHeader.SizeOfImage==0x5B2D000u
            &&std::memcmp(reinterpret_cast<const void*>(b+0x1DFEC30),create,sizeof(create))==0
            &&std::memcmp(reinterpret_cast<const void*>(b+0x1DFDFF0),dispatch,sizeof(dispatch))==0;
    } __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
ffxReturnCode_t Create(ffxContext* c,ffxCreateContextDescHeader* d,const ffxAllocationCallbacks* a) noexcept {return runtime.Create(c,d,a);}
ffxReturnCode_t Destroy(ffxContext* c,const ffxAllocationCallbacks* a) noexcept {return runtime.Destroy(c,a);}
ffxReturnCode_t Configure(ffxContext* c,const ffxConfigureDescHeader* d) noexcept {return runtime.Configure(c,d);}
ffxReturnCode_t Dispatch(ffxContext* c,const ffxDispatchDescHeader* d) noexcept {
    rt::View v{};v.active=native_stereo_preflight::InUpscalerView();
    native_stereo_preflight::UpscalerView native{};
    if(v.active&&native_stereo_preflight::CurrentUpscalerView(native)){
        v.valid=true;v.eye=native.eye;v.epoch=native.epoch;v.frame=native.frame;v.pipeline=native.pipeline;
    }
    const auto result=runtime.Dispatch(c,d,v);
    // Finite failure receipt, independent of F2; no resource readback or poll.
    if(result!=FFX_API_RETURN_OK&&v.valid&&runtime.RecoverableResetCut(v.epoch,v.frame,v.pipeline)){
        const auto cuts=runtime.GetStatus().resetCuts;
        if(cuts<=3||cuts%256==0){const auto error=GetLastError();char line[256]{};
            std::snprintf(line,sizeof(line),"FSR RESET_CUT epoch=%llu frame=%u cuts=%llu pair=DISCARDED nextPair=RESET_BOTH terminalFault=0",
                (unsigned long long)v.epoch,v.frame,(unsigned long long)cuts);
            diagnostic_trace::WriteStatus(line);SetLastError(error);
        }
    }
    const auto now=GetTickCount64();
    if(d&&d->type==FFX_API_DISPATCH_DESC_TYPE_UPSCALE&&v.valid&&v.eye<2&&
       result==FFX_API_RETURN_OK&&diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,now)){
        static std::atomic<std::uint64_t> last[2]{};
        auto previous=last[v.eye].load(std::memory_order_relaxed);
        if((!previous||now<previous||now-previous>=1000)&&last[v.eye].compare_exchange_strong(previous,now)){
            const DWORD error=GetLastError();const auto& input=*reinterpret_cast<const ffxDispatchDescUpscale*>(d);
            const auto* output=static_cast<ID3D12Resource*>(input.output.resource);
            const auto od=output?const_cast<ID3D12Resource*>(output)->GetDesc():D3D12_RESOURCE_DESC{};
            char line[640]{};std::snprintf(line,sizeof(line),
                "D85R6 UPSCALER_INPUT tech=FSR epoch=%llu frame=%u eye=%u render=%ux%u output=%p:%llux%u jitter=%.6f/%.6f mvScale=%.6f/%.6f nativeReset=%d pixels=UNREAD",
                (unsigned long long)v.epoch,v.frame,v.eye,input.renderSize.width,input.renderSize.height,input.output.resource,
                (unsigned long long)od.Width,od.Height,double(input.jitterOffset.x),double(input.jitterOffset.y),
                double(input.motionVectorScale.x),double(input.motionVectorScale.y),input.reset?1:0);
            diagnostic_trace::Write(line);SetLastError(error);
        }
    }
    if(d&&d->type==FFX_API_DISPATCH_DESC_TYPE_UPSCALE&&v.valid&&v.eye==1&&
       result==FFX_API_RETURN_OK&&runtime.PairReady(v.epoch,v.frame)){
        const auto n=completedPairs.fetch_add(1,std::memory_order_relaxed)+1;
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,now)&&(n<=3||n%1024==0)){
            const DWORD nativeError=GetLastError();
            char line[256]{};std::snprintf(line,sizeof(line),
                "D82 FSR_STEREO evaluated epoch=%llu frame=%u pairs=%llu bothEyes=1 histories=SEPARATE nativeInputs=PRESERVED GPU_CONTENT_UNVERIFIED",
                static_cast<unsigned long long>(v.epoch),v.frame,static_cast<unsigned long long>(n));
            diagnostic_trace::Write(line);
            SetLastError(nativeError);
        }
    }
    return result;
}
template<class Fn> bool Make(unsigned i,HMODULE module,const char* name,Fn detour,Fn& original) noexcept {
    auto address=GetProcAddress(module,name);if(!address)return false;
    DetourModKit::hook::InlineRequest request{name,DetourModKit::hook::Target{DetourModKit::Address{reinterpret_cast<std::uintptr_t>(address)}},DetourModKit::hook::Options{}};
    auto result=DetourModKit::hook::inline_at(std::move(request),detour);if(!result)return false;
    hooks[i]=std::move(*result);original=hooks[i]->original<Fn>();return original!=nullptr;
}
}
bool Install() noexcept {
    std::lock_guard lock(installMutex);if(installed.load(std::memory_order_acquire))return true;
    if(attempted)return false;
    const auto module=GetModuleHandleW(L"amd_fidelityfx_loader_dx12.dll");
    if(!module||!GetModuleHandleW(L"WHGame.dll"))return false;
    attempted=true;
    if(!ExactGame()){
        diagnostic_trace::Write("D82 FSR_STEREO install=REFUSED reason=retail-image");return false;
    }
    // Construct every trampoline before enabling any wrapper. All names are
    // actual game imports/loader exports; no scratch-buffer or context copying.
    const bool made=Make(0,module,"ffxDestroyContext",static_cast<PfnFfxDestroyContext>(&Destroy),originals.destroy)
        &&Make(1,module,"ffxDispatch",static_cast<PfnFfxDispatch>(&Dispatch),originals.dispatch)
        &&Make(2,module,"ffxConfigure",static_cast<PfnFfxConfigure>(&Configure),originals.configure)
        &&Make(3,module,"ffxCreateContext",static_cast<PfnFfxCreateContext>(&Create),originals.create);
    if(!made){
        diagnostic_trace::Write("D82 FSR_STEREO install=FAILED reason=hook-construction");return false;
    }
    runtime.SetApi(originals);
    // Lifecycle/evaluation hooks first, creation last: no twin can be created
    // while its destruction or dispatch path is still unprotected.
    for(auto& h:hooks)if(!h->enable()){
        for(auto& old:hooks)if(old)(void)old->disable();
        // Retain every trampoline and original pointer until process exit. A
        // native pass-through may still be returning on another thread. Creation
        // is enabled last, so a failed transaction has not created a twin.
        diagnostic_trace::Write("D82 FSR_STEREO install=FAILED reason=hook-enable stereoPair=UNAVAILABLE");return false;
    }
    installed.store(true,std::memory_order_release);
    diagnostic_trace::Write("D82 FSR_STEREO installed=1 api=FFX_LOADER_DX12 histories=INDEPENDENT_CONTEXTS nativeResources=PRESERVED validation=CPU_ROUTING_NOT_HEADSET");
    return true;
}
bool Installed() noexcept {return installed.load(std::memory_order_acquire);}
void SetEnabled(bool enabled) noexcept {runtime.SetEnabled(enabled);}
bool ReadyForStereo() noexcept {return Installed()&&runtime.Ready();}
bool PairReady(std::uint64_t epoch,std::uint32_t frame) noexcept {return Installed()&&runtime.PairReady(epoch,frame);}
bool RecoverableResetCut(std::uint64_t epoch,std::uint32_t frame,std::uintptr_t pipeline) noexcept {
    return Installed()&&runtime.RecoverableResetCut(epoch,frame,pipeline);
}
void Stop() noexcept {runtime.SetEnabled(false);} // native lifecycle owns safe GPU destruction
void LogStatus(const char* reason) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,GetTickCount64()))return;
    const auto s=runtime.GetStatus();char line[512]{};
    std::snprintf(line,sizeof(line),"D82 FSR_STEREO reason=%s installed=%d enabled=%d livePairs=%u created=%llu twins=%llu evaluatedL=%llu evaluatedR=%llu refused=%llu destroyedTwins=%llu",
        reason?reason:"status",Installed(),s.enabled,s.live,static_cast<unsigned long long>(s.creates),static_cast<unsigned long long>(s.twins),
        static_cast<unsigned long long>(s.left),static_cast<unsigned long long>(s.right),static_cast<unsigned long long>(s.refused),static_cast<unsigned long long>(s.destroyed));
    diagnostic_trace::Write(line);
}
}
