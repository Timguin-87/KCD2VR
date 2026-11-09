#include "native_dlss_stereo_d82.h"
#include "native_dlss_aer_dump_d82.h"
#include "aer_warp_r154.h"
#include "native_stereo_preflight_v6602.h"
#include "diagnostic_trace.h"
#include "diagnostics/capture168.h"
#include "frame_sync.h"
#include "camera_hook.h"
#include "build_identity.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <optional>
#include <iterator>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace kcd2vr::native_dlss_stereo_d82 { namespace {
DWORD SaveNativeError() noexcept {return GetLastError();}
void RestoreNativeError(DWORD error) noexcept {SetLastError(error);}
void* ReadHandle(void** p) noexcept {
    if(!p)return nullptr;
    __try {return *p;} __except(EXCEPTION_EXECUTE_HANDLER) {return nullptr;}
}
std::uintptr_t CommandDevice(void* command) noexcept {
    if(!command)return 0;
    ID3D12Device* device=nullptr;
    const auto hr=static_cast<ID3D12GraphicsCommandList*>(command)->GetDevice(IID_PPV_ARGS(&device));
    if(FAILED(hr)||!device)return 0;
    const auto identity=reinterpret_cast<std::uintptr_t>(device);device->Release();return identity;
}
void ReportFailure(Fault reason,const char* label) noexcept {
    // Refuse calls this once per terminal fault; retain its cause without F2.
    const DWORD error=GetLastError();char line[256]{};
    std::snprintf(line,sizeof(line),"D82 DLSS_STEREO refused=%s code=%u stereoPair=WITHHELD nativeSettings=UNCHANGED",
        label?label:"unknown",static_cast<unsigned>(reason));
    diagnostic_trace::WriteStatus(line);SetLastError(error);
}
void ReportPairCreated(const void* left,const void* right) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,GetTickCount64()))return;
    const DWORD error=GetLastError();char line[256]{};
    std::snprintf(line,sizeof(line),"D82 DLSS_STEREO created left=%p right=%p owner=WHGame_NGX resources=NATIVE perEyeHistory=SEPARATE GPUverification=PENDING",left,right);
    diagnostic_trace::Write(line);SetLastError(error);
}
void ReportPairEvaluated(std::uint64_t epoch,std::uint32_t frame,const void* left,const void* right,std::uint64_t pairs) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,GetTickCount64()))return;
    if(pairs>3 && pairs%1024!=0)return;
    const DWORD error=GetLastError();char line[288]{};
    std::snprintf(line,sizeof(line),"D82 DLSS_STEREO evaluated epoch=%llu frame=%u pairs=%llu left=%p right=%p result=NGX_SUCCESS bothEyes=1 jitterAndResources=NATIVE_GPU_CONTENT_UNVERIFIED",
        static_cast<unsigned long long>(epoch),frame,static_cast<unsigned long long>(pairs),left,right);
    diagnostic_trace::Write(line);SetLastError(error);
}

// Called only for the first three cuts and then every 256th cut. These are
// scalar values already read for the exact rejected pair, not a new GPU probe.
void ReportResetCut(std::uint64_t epoch,std::uint32_t frame,std::uint32_t leftFrame,
    std::uint32_t previousRightFrame,int leftNative,int rightNative,bool leftEffective,
    bool rightEffective,std::uint64_t cuts) noexcept {
    const DWORD error=GetLastError();char line[448]{};
    std::snprintf(line,sizeof(line),
        "DLSS RESET_CUT epoch=%llu frame=%u cuts=%llu leftFrame=%u previousRightFrame=%u nativeReset=L%d/R%d effectiveReset=L%d/R%d pair=DISCARDED nextPair=RESET_BOTH terminalFault=0",
        (unsigned long long)epoch,frame,(unsigned long long)cuts,leftFrame,previousRightFrame,
        leftNative,rightNative,leftEffective?1:0,rightEffective?1:0);
    diagnostic_trace::WriteStatus(line);SetLastError(error);
}

void ReportEvaluationInputs(unsigned eye,std::uint64_t epoch,std::uint32_t frame,void* parameters,
    int nativeReset,bool effectiveReset) noexcept;

#include "native_dlss_stereo_d82_runtime.inl"
#include "native_dlss_aer_d82.inl"
#include "native_dlss_aer_dump_d82.inl"

void ReportEvaluationInputs(unsigned eye,std::uint64_t epoch,std::uint32_t frame,void* parameters,
    int nativeReset,bool effectiveReset) noexcept {
    const auto now=GetTickCount64();
    if(eye>1||!diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,now))return;
    // Select the pair at LEFT; RIGHT must match that exact epoch/frame. Two
    // independent eye timers could previously log unrelated frames as a pair.
    static std::uint64_t last=0,sampledEpoch=0;
    static std::uint32_t sampledFrame=0;
    if(eye==0){
        if(last&&now>=last&&now-last<1000)return;
        last=now;sampledEpoch=epoch;sampledFrame=frame;
    }else if(sampledEpoch!=epoch||sampledFrame!=frame)return;
    const DWORD error=GetLastError();
    const char* keys[]{"Color","Output","Depth","MotionVectors"};
    void* resource[4]{};D3D12_RESOURCE_DESC desc[4]{};
    for(unsigned i=0;i<4;++i)if(Success(g_get_resource(parameters,keys[i],&resource[i]))&&resource[i])
        desc[i]=static_cast<ID3D12Resource*>(resource[i])->GetDesc();
    float values[4]{};const char* names[]{"Jitter.Offset.X","Jitter.Offset.Y","MV.Scale.X","MV.Scale.Y"};
    for(unsigned i=0;i<4;++i)(void)g_get_f(parameters,names[i],&values[i]);
    char line[768]{};std::snprintf(line,sizeof(line),
        "D85R6 UPSCALER_INPUT tech=DLSS epoch=%llu frame=%u eye=%u color=%p:%llux%u output=%p:%llux%u depth=%p:%llux%u motion=%p:%llux%u jitter=%.6f/%.6f mvScale=%.6f/%.6f nativeReset=%d effectiveReset=%d dimensions=RESOURCE_ALLOCATION_NOT_ACTIVE_SUBRECT pixels=UNREAD",
        (unsigned long long)epoch,frame,eye,resource[0],(unsigned long long)desc[0].Width,desc[0].Height,
        resource[1],(unsigned long long)desc[1].Width,desc[1].Height,resource[2],(unsigned long long)desc[2].Width,desc[2].Height,
        resource[3],(unsigned long long)desc[3].Width,desc[3].Height,double(values[0]),double(values[1]),double(values[2]),double(values[3]),nativeReset,effectiveReset?1:0);
    diagnostic_trace::Write(line);SetLastError(error);
}

// Obtained from the supplied WHGame.dll, SHA256
// bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7.
// Resolve by export NAME, then verify module/RVA/entry. No nvngx_dlss private ABI.
struct Pin {const char* name;std::uintptr_t rva;unsigned char bytes[16];};
constexpr Pin kPins[]{
 {"NVSDK_NGX_D3D12_CreateFeature",0x1BE4840,{0x48,0x89,0x6C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x48,0x89,0x7C,0x24,0x20,0x41}},
 {"NVSDK_NGX_D3D12_EvaluateFeature",0x1BE4920,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x48}},
 {"NVSDK_NGX_D3D12_EvaluateFeature_C",0x1BE4990,{0x48,0x83,0xEC,0x38,0x48,0x8B,0xC1,0x48,0x8B,0x0D,0x52,0xCB,0x48,0x03,0x48,0x85}},
 {"NVSDK_NGX_D3D12_ReleaseFeature",0x1BE4D50,{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0x1D,0x97,0xC7,0x48}},
 {"NVSDK_NGX_D3D12_Shutdown1",0x1BE4DB0,{0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x48,0x89,0x7C,0x24,0x20,0x55}},
 {"NVSDK_NGX_Parameter_GetI",0x1BE3B80,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7C,0x24,0x18,0x41}},
 {"NVSDK_NGX_Parameter_SetI",0x1BE3E60,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7C,0x24,0x18,0x41}},
 {"NVSDK_NGX_Parameter_GetF",0x1BE3B20,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7C,0x24,0x18,0x41}},
 {"NVSDK_NGX_Parameter_GetD3d12Resource",0x1BE3AC0,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7C,0x24,0x18,0x41}},
 // R149 alternate-eye: scoped MotionVectors substitution and unsigned subrect reads.
 {"NVSDK_NGX_Parameter_SetD3d12Resource",0x1BE3DB0,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7C,0x24,0x18,0x41}},
 {"NVSDK_NGX_Parameter_GetUI",0x1BE3BE0,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7C,0x24,0x18,0x41}}
};
std::array<std::optional<DetourModKit::hook::Hook>,5> g_hooks;
std::mutex g_install_mutex;
bool g_attempted=false;
bool ExactModule(HMODULE module) noexcept {
    __try {
        const auto base=reinterpret_cast<std::uintptr_t>(module);
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE || dos->e_lfanew<0 || dos->e_lfanew>0x10000)return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        return nt->Signature==IMAGE_NT_SIGNATURE && nt->FileHeader.Machine==IMAGE_FILE_MACHINE_AMD64 &&
            nt->FileHeader.TimeDateStamp==0x6A350E20u && nt->OptionalHeader.Magic==IMAGE_NT_OPTIONAL_HDR64_MAGIC &&
            nt->OptionalHeader.SizeOfImage==0x5B2D000u;
    } __except(EXCEPTION_EXECUTE_HANDLER) {return false;}
}
void* ExactExport(HMODULE module,const Pin& pin) noexcept {
    const auto result=GetProcAddress(module,pin.name);
    if(reinterpret_cast<std::uintptr_t>(result)!=reinterpret_cast<std::uintptr_t>(module)+pin.rva)return nullptr;
    __try {
        if(std::memcmp(reinterpret_cast<const void*>(result),pin.bytes,sizeof(pin.bytes)))return nullptr;
    } __except(EXCEPTION_EXECUTE_HANDLER) {return nullptr;}
    return reinterpret_cast<void*>(result);
}
template<class Fn> bool Prepare(unsigned index,void* target,Fn detour,Fn& original) {
    DetourModKit::hook::InlineRequest request{kPins[index].name,
        DetourModKit::hook::Target{DetourModKit::Address{reinterpret_cast<std::uintptr_t>(target)}},DetourModKit::hook::Options{}};
    auto hook=DetourModKit::hook::inline_at(std::move(request),detour);
    if(!hook)return false;
    g_hooks[index]=std::move(*hook);original=g_hooks[index]->original<Fn>();return original!=nullptr;
}
} // namespace

bool Install() noexcept {
    std::lock_guard<std::mutex> install(g_install_mutex);
    if(Installed())return true;
    if(g_attempted)return false;
    const auto module=GetModuleHandleW(L"WHGame.dll");if(!module)return false;
    g_attempted=true;
    if(!ExactModule(module)) {diagnostic_trace::Write("D82 DLSS_STEREO install=REFUSED reason=retail-image");return false;}
    std::array<void*,std::size(kPins)> addresses{};
    for(unsigned i=0;i<addresses.size();++i) {
        addresses[i]=ExactExport(module,kPins[i]);
        if(!addresses[i]) {diagnostic_trace::Write("D82 DLSS_STEREO install=REFUSED reason=export-contract");return false;}
    }
    g_get_i=reinterpret_cast<GetIntFn>(addresses[5]);g_set_i=reinterpret_cast<SetIntFn>(addresses[6]);
    g_get_f=reinterpret_cast<GetFloatFn>(addresses[7]);g_get_resource=reinterpret_cast<GetResourceFn>(addresses[8]);
    g_set_resource=reinterpret_cast<SetResourceFn>(addresses[9]);g_get_ui=reinterpret_cast<GetUIntFn>(addresses[10]);
    g_wh_base=reinterpret_cast<std::uintptr_t>(module);
    const bool prepared=Prepare(0,addresses[0],static_cast<CreateFn>(&CreateDetour),g_create) &&
        Prepare(1,addresses[1],static_cast<EvaluateFn>(&EvaluateDetour),g_evaluate) &&
        Prepare(2,addresses[2],static_cast<EvaluateFn>(&EvaluateCDetour),g_evaluate_c) &&
        Prepare(3,addresses[3],static_cast<ReleaseFn>(&ReleaseDetour),g_release) &&
        Prepare(4,addresses[4],static_cast<ShutdownFn>(&ShutdownDetour),g_shutdown);
    // A racing Create detour waits here until the complete installation state
    // is published. Otherwise a successful native creation in the enable/store
    // interval would be permanently missed and could never acquire its twin.
    std::lock_guard<std::recursive_mutex> registry(g_mutex);
    bool enabled=prepared;
    // Lifecycle and evaluation precede creation. Native calls already inside
    // the old unhooked API remain uncaptured and must follow native recreation.
    if(prepared)for(unsigned index:{3u,4u,1u,2u,0u})if(!g_hooks[index]->enable()){enabled=false;break;}
    if(!enabled) {
        for(auto& hook:g_hooks)if(hook)(void)hook->disable();
        // Keep any created trampoline alive: a racing native pass-through may
        // still be returning. Failed installation is terminal for this process.
        diagnostic_trace::Write("D82 DLSS_STEREO install=FAILED reason=hook-transaction stereoPair=UNAVAILABLE");return false;
    }
    g_installed.store(true,std::memory_order_release);
    diagnostic_trace::Write("D82 DLSS_STEREO install=ARMED boundary=WHGame_NGX_EXPORTS views=TLS_REQUIRED renderTests=PENDING");return true;
}
bool Installed() noexcept {return g_installed.load(std::memory_order_acquire);}
void SetEnabled(bool enabled) noexcept {
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if(g_enabled.exchange(enabled,std::memory_order_acq_rel)!=enabled) {
        g_generation.fetch_add(1,std::memory_order_acq_rel);
        for(auto& p:g_pairs)Invalidate(p);
        for(auto& receipt:g_receipts)ClearReceipt(receipt);
        if(!enabled)g_fault.store(Fault::None,std::memory_order_release);
    }
}
bool ReadyForStereo() noexcept {
    return Installed() && g_live_pairs.load(std::memory_order_acquire)>0 && !Faulted();
}
bool PairReady(std::uint64_t epoch,std::uint32_t frame) noexcept {
    if(!ReadyForStereo() || !g_enabled.load(std::memory_order_acquire))return false;
    const auto generation=g_generation.load(std::memory_order_acquire);
    for(const auto& receipt:g_receipts) {
        const auto serial=receipt.serial.load(std::memory_order_seq_cst);if(!serial)continue;
        const bool matches=receipt.generation.load(std::memory_order_seq_cst)==generation &&
            receipt.epoch.load(std::memory_order_seq_cst)==epoch && receipt.frame.load(std::memory_order_seq_cst)==frame;
        if(matches && receipt.serial.load(std::memory_order_seq_cst)==serial &&
           generation==g_generation.load(std::memory_order_acquire) &&
           g_enabled.load(std::memory_order_acquire) && ReadyForStereo())return true;
    }
    return false;
}
bool RecoverableResetCut(std::uint64_t epoch,std::uint32_t frame,std::uintptr_t pipeline) noexcept {
    // This is the render-thread completion path, outside SDK evaluation. Unlike
    // PairReady (also read by queue callbacks), it may inspect the live registry.
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if(!epoch || !pipeline || !ReadyForStereo() || !g_enabled.load(std::memory_order_acquire))return false;
    const auto generation=g_generation.load(std::memory_order_acquire);
    for(const auto& p:g_pairs)if(p.native && p.right && p.native!=p.right &&
        p.feature==kSuperSampling && p.recoverableCut && p.cutFrame==frame &&
        p.generation==generation && p.epoch==epoch && p.pipeline==pipeline &&
        !p.have[0] && !p.have[1] && p.reset[0] && p.reset[1])return true;
    return false;
}
bool Faulted() noexcept {return g_fault.load(std::memory_order_acquire)!=Fault::None;}
Status GetStatus() noexcept {
    std::lock_guard<std::recursive_mutex> lock(g_mutex);Status out=g_stats;
    out.installed=Installed();out.enabled=g_enabled.load(std::memory_order_acquire);out.ready=ReadyForStereo();
    out.fault=g_fault.load(std::memory_order_acquire);out.livePairs=g_live_pairs.load(std::memory_order_acquire);return out;
}
void LogStatus(const char* reason) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,GetTickCount64()))return;
    const auto s=GetStatus();char line[512]{};
    std::snprintf(line,sizeof(line),"D82 DLSS_STEREO status=%s installed=%d enabled=%d ready=%d fault=%u livePairs=%u create=%llu twins=%llu evalLR=%llu/%llu pairs=%llu refused=%llu reset=%llu resetCuts=%llu unscoped=%llu last=%llu/%u",
        reason?reason:"status",s.installed?1:0,s.enabled?1:0,s.ready?1:0,static_cast<unsigned>(s.fault),s.livePairs,
        static_cast<unsigned long long>(s.creates),static_cast<unsigned long long>(s.twins),
        static_cast<unsigned long long>(s.leftEvaluations),static_cast<unsigned long long>(s.rightEvaluations),
        static_cast<unsigned long long>(s.pairs),static_cast<unsigned long long>(s.refused),
        static_cast<unsigned long long>(s.resets),static_cast<unsigned long long>(s.resetCuts),
        static_cast<unsigned long long>(s.unscoped),
        static_cast<unsigned long long>(s.lastEpoch),s.lastFrame);
    diagnostic_trace::Write(line);
}
void Stop() noexcept {SetEnabled(false);}

void SetAlternateEyeMode(bool enabled) noexcept {
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if(g_alternate_eye.exchange(enabled,std::memory_order_acq_rel)!=enabled) {
        // A new routing owner: no history, receipt or stereo-pair fault survives.
        g_generation.fetch_add(1,std::memory_order_acq_rel);
        for(auto& p:g_pairs)Invalidate(p);
        for(auto& receipt:g_receipts)ClearReceipt(receipt);
        g_fault.store(Fault::None,std::memory_order_release);
        char line[160]{};std::snprintf(line,sizeof(line),"R149 AER_DLSS routing=%s motionMode=%d",
            enabled?"ALTERNATE_EYE_PER_EYE_HISTORY":"OFF",g_aer_motion_mode.load(std::memory_order_acquire));
        diagnostic_trace::WriteStatus(line);
    }
}
void SetAlternateEyeMotionMode(int mode) noexcept {
    mode=std::clamp(mode,0,2);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if(g_aer_motion_mode.exchange(mode,std::memory_order_acq_rel)!=mode)
        for(auto& p:g_pairs)Invalidate(p);
}
int AlternateEyeMotionMode() noexcept {return g_aer_motion_mode.load(std::memory_order_acquire);}

bool RequestAerFrameDump(unsigned frames,unsigned delayMs) noexcept {
    if(!g_alternate_eye.load(std::memory_order_acquire)||!g_enabled.load(std::memory_order_acquire)){
        diagnostic_trace::WriteStatus("R149 AER_DUMP refused reason=ALTERNATE_EYE_DLSS_NOT_ACTIVE");
        return false;
    }
    return AerDumpRequestInternal(frames,delayMs);
}
bool AerFrameDumpBusy() noexcept {return g_dump.phase.load(std::memory_order_acquire)!=kAerDumpIdle;}
void AerFrameDumpRenderTick(void* directQueue,bool presentSubmitted) noexcept {
    AerDumpRenderTickInternal(static_cast<ID3D12CommandQueue*>(directQueue),presentSubmitted);
}

struct AerDumpXrCapture : AerDumpXrCaptureBody {};
bool AerDumpXrWanted() noexcept {return g_dump.xrWanted.load(std::memory_order_acquire);}
AerDumpXrCapture* AerDumpXrBegin(ID3D12Device* device) noexcept {
    if(!device||!AerDumpXrWanted())return nullptr;
    try{return new AerDumpXrCapture{};}catch(...){return nullptr;}
}
bool AerDumpXrCopy(AerDumpXrCapture* capture,ID3D12GraphicsCommandList* list,ID3D12Resource* resource,
                   D3D12_RESOURCE_STATES state,const char* name) noexcept {
    if(!capture||!list||!resource)return false;
    ID3D12Device* raw=nullptr;
    if(FAILED(list->GetDevice(IID_PPV_ARGS(&raw)))||!raw)return false;
    Microsoft::WRL::ComPtr<ID3D12Device> device;device.Attach(raw);
    char stateName[32]{};std::snprintf(stateName,sizeof(stateName),"0x%X",static_cast<unsigned>(state));
    try{return AerDumpDirect(device.Get(),list,resource,state,name?name:"xr",stateName,capture->record.textures);}
    catch(...){return false;}
}
void AerDumpXrSetJson(AerDumpXrCapture* capture,std::string json) noexcept {
    if(capture)capture->record.json=std::move(json);
}
void AerDumpXrCommit(AerDumpXrCapture* capture,bool executed,bool newFrame) noexcept {
    if(!capture)return;
    {
        std::lock_guard<std::mutex> lock(g_dump.mutex);
        auto& d=g_dump;
        if(!executed){
            // The list may still reference these readbacks; never release them.
            for(auto& t:capture->record.textures)if(t.readback)d.neverRelease.push_back(t.readback);
        }else if(d.phase.load()==kAerDumpCapturing&&d.xrWanted.load()){
            capture->record.kind="xr";capture->record.index=d.xrBlits++;
            if(newFrame)++d.xrNewFrames;
            if(capture->record.json.empty())capture->record.json="\"note\":\"no metadata\"";
            try{d.records.push_back(std::move(capture->record));}catch(...){}
            if(d.xrNewFrames>=d.framesWanted+1||d.xrBlits>=d.xrMaxBlits)d.xrWanted.store(false,std::memory_order_release);
        }
    }
    delete capture;
}
}
