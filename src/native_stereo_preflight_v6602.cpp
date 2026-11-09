#include "native_stereo_preflight_v6602.h"
#include "native_upscaler_stereo_d82.h"
#include "native_svo_right_v6654.h" // V66.54 (must stay at file scope)
#include "native_stereo_outer_lab.h"
#include "frame_sync.h"
#include "stereo_preflight_model_v6602.h"
#include "stereo_resource_model_v6603.h"
#include "stereo_eye_control_model_v6604.h"
#include "stereo_design_capture_v6606.h"
#include <intrin.h>
#include "diagnostic_trace.h"
#include "diagnostics/native_palette_view_d67r1.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <array>
#include <optional>
#include <utility>
#include <cstdio>
#include <cstdarg>

namespace kcd2vr::native_stereo_preflight {
namespace {
namespace model=preflight_model;
using StandardFn=std::uint8_t(__fastcall*)(void*);
// The native caller consumes AL. Preserve the byte, rather than normalizing bool.
static_assert(sizeof(std::uint8_t)==1);
static_assert(sizeof(void*)==8,"V66.12 preflight requires the x64 target");
enum State : int { Idle, Armed, Running, Done, Cancelled, TimedOut, Faulted, Refused, Preparing, Allocating, Witnessing };
std::atomic<int> g_state{Idle};
std::atomic<bool> g_installed{false},g_setup_failed{false};
std::atomic<unsigned> g_inflight{0},g_samples{0},g_left_basic{0},g_right_basic{0},g_right_missing{0},g_unknown{0};
std::atomic<unsigned long> g_owner_tid{0};
std::atomic<std::uint64_t> g_started{0},g_seen{0},g_qualified{0},g_epoch{0};
std::atomic<int> g_first_fault{0}; // 0 empty, 1 reserved, 2 published; no spinning
// A returned-but-unconfirmed backed resize is not a native exception. This
// process-lifetime latch survives Stop/timeout racing the Allocating state.
std::atomic<bool> g_velocity_resize_failed{false};
// Initial unbacked creation budget. Backed size changes use the independently
// verified native resize path, not another name-registry allocation attempt.
std::atomic<bool> g_mutation_claimed{false};
std::atomic<unsigned> g_allocation_attempts{0},g_allocation_return{0};
std::atomic<int> g_allocation_outcome{0};
// V66.17: direct F9 must perform the one-time right-eye velocity provisioning that F11 used to own.
std::atomic<bool> g_direct_design_needs_provisioning{false};
// V66.18 run kind: 1=F11 zero-separation proof; F9 design stage N uses runKind=N+1.
// Direct F9 entry is now allowed because V66.15 already proved real native L/R publication.
// stage1=one real-IPD L/R CPU frame, stage2=one preserved/certified L/R frame,
// stage3=30-frame live preserved burst, stage4=continuous live preserved stereo.
std::atomic<int> g_run_kind{0};
std::atomic<bool> g_eye1_verified{false},g_last_run_passed{false};
std::atomic<unsigned> g_f11_outcome{0};
std::atomic<int> g_last_design_stage_passed{0};
std::atomic<float> g_design_ipd{0.064f};
std::atomic<bool> g_v6648_zero_ipd{false}; // V66.48 diagnostic: both eye passes use the center camera
// V66.12: an F9 request that arrives while the renderer is still unwinding the
// successful F11 command is latched instead of refused. Poll() arms it once the
// Standard/Pipeline inflight counters both reach zero. Only one pending design
// request can exist at a time.
std::atomic<int> g_pending_design_stage{0};
std::atomic<float> g_pending_design_ipd{0.064f};
std::atomic<std::uint64_t> g_design_pair_serial{0},g_design_serial_counter{0};
std::atomic<unsigned> g_repeat_remaining{0},g_repeat_completed{0};
constexpr unsigned kStereoBurstFrames=30;
std::optional<DetourModKit::hook::Hook> g_hook;
std::atomic<StandardFn> g_original{nullptr}; // release-published before hook enable
std::uintptr_t g_wh=0;
HANDLE g_fault_file=INVALID_HANDLE_VALUE; // process lifetime; opened outside render/SEH
model::Progress g_progress{}; // owning render thread only; never read by input
thread_local unsigned t_depth=0,t_native_stage=0;
thread_local std::uintptr_t t_pipeline=0,t_crv=0;
thread_local std::uint64_t t_epoch=0;
#include "native_stereo_preflight_signatures_v6602.inc"
#include "native_stereo_resource_signatures_v6603.inc"

bool Active(int s) noexcept {return s==Armed || s==Running || s==Allocating || s==Witnessing;}
const char* StateName(int s) noexcept {
    switch(s) {
    case Armed:return "ARMED";case Running:return "RUNNING";case Done:return "DONE";
    case Witnessing:return "EYE1-CONTROL";case Allocating:return "ALLOCATING";case Cancelled:return "STOPPED";case TimedOut:return "TIMED-OUT";case Faulted:return "FAILED";
    case Refused:return "REFUSED";default:return "IDLE";
    }
}
#if defined(__clang__) || defined(__GNUC__)
__attribute__((format(printf,1,2)))
#endif
void Log(const char* format,...) noexcept {
    // V66.18: once continuous stage 4 is proven alive, suppress the old per-eye
    // archaeology flood. Keep progress/error/allocation/completion lines only.
    if(g_run_kind.load(std::memory_order_relaxed)==5 && g_repeat_completed.load(std::memory_order_relaxed)>=3) {
        const auto contains=[](const char* hay,const char* needle) noexcept {
            if(!hay||!needle||!*needle)return false;
            for(const char* h=hay;*h;++h){const char* a=h;const char* b=needle;while(*a&&*b&&*a==*b){++a;++b;}if(!*b)return true;}
            return false;
        };
        const bool important=contains(format,"CONTINUOUS FRAME") || contains(format,"BLOCKED") ||
            contains(format,"FAULT") || contains(format,"REFUSED") || contains(format,"COMPLETE") || contains(format,"ALLOC");
        if(!important) return;
    }
    char buffer[3000]{};va_list args;va_start(args,format);
    const int n=std::vsnprintf(buffer,sizeof(buffer),format,args);va_end(args);
    if(n>0)diagnostic_trace::Write(buffer);
}
struct Reader {
    bool operator()(std::uintptr_t address,void* out,std::size_t bytes) const noexcept {
        if(!address || !out || !bytes || address>~std::uintptr_t{0}-(bytes-1))return false;
        SIZE_T copied=0;
        return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<const void*>(address),out,bytes,&copied)!=0 && copied==bytes;
    }
};
Reader g_read;

// No regular logger, allocation, loader calls, engine calls, or guessed unlock in
// the exception filter. The only I/O is a fixed-buffer write to an existing handle.
struct FaultLine {
    char bytes[1600]{};unsigned used{};
    void Text(const char* s) noexcept {while(*s && used+1<sizeof(bytes))bytes[used++]=*s++;}
    void Hex(std::uint64_t n) noexcept {
        Text("0x");const char* digits="0123456789ABCDEF";
        for(int shift=60;shift>=0;shift-=4) if(used+1<sizeof(bytes))bytes[used++]=digits[(n>>shift)&15u];
    }
    void Field(const char* name,std::uint64_t value) noexcept {Text(name);Hex(value);Text(" ");}
};
int RecordFault(EXCEPTION_POINTERS* info) noexcept {
    const DWORD lastError=GetLastError();
    int expected=0;
    if(info && info->ExceptionRecord && info->ContextRecord &&
       g_first_fault.compare_exchange_strong(expected,1,std::memory_order_acq_rel)) {
        const auto* e=info->ExceptionRecord;const auto* c=info->ContextRecord;
        const auto address=reinterpret_cast<std::uintptr_t>(e->ExceptionAddress);
        FaultLine line{};line.Text("V66.12-PREFLIGHT FIRST-FAULT ");
        line.Field("nativeStage=",t_native_stage);line.Field("epoch=",t_epoch);line.Field("tickMs=",GetTickCount64());
        line.Field("tid=",GetCurrentThreadId());line.Field("code=",e->ExceptionCode);
        line.Field("flags=",e->ExceptionFlags);line.Field("address=",address);
        const bool inWh=address>=g_wh && address-g_wh<model::kImageSize;
        line.Field("inWHGame=",inWh?1:0);line.Field("whRva=",inWh?address-g_wh:0);
        line.Field("pipeline=",t_pipeline);line.Field("crv=",t_crv);
        line.Field("rip=",c->Rip);line.Field("rsp=",c->Rsp);line.Field("rbp=",c->Rbp);
        line.Field("rax=",c->Rax);line.Field("rbx=",c->Rbx);line.Field("rcx=",c->Rcx);
        line.Field("rdx=",c->Rdx);line.Field("r8=",c->R8);line.Field("r9=",c->R9);
        if(e->NumberParameters>=2 && (e->ExceptionCode==EXCEPTION_ACCESS_VIOLATION ||
                                    e->ExceptionCode==EXCEPTION_IN_PAGE_ERROR)) {
            line.Field("accessKind=",e->ExceptionInformation[0]);
            line.Field("accessAddress=",e->ExceptionInformation[1]);
        }
        line.Text("policy=CONTINUE-SEARCH nativeRecovery=NOT-ATTEMPTED\r\n");
        // Terminal status is published before I/O; cancellation never waits for a
        // later render command. No claim is made that this repairs native state.
        g_state.store(Faulted,std::memory_order_release);
        g_first_fault.store(2,std::memory_order_release);
        if(g_fault_file!=INVALID_HANDLE_VALUE) {
            DWORD written=0;(void)WriteFile(g_fault_file,line.bytes,line.used,&written,nullptr);
        }
    }
    SetLastError(lastError);
    return EXCEPTION_CONTINUE_SEARCH; // never swallow an unexpected native fault
}

bool Qualify(std::uintptr_t p,std::uintptr_t& crv) noexcept {
    std::uintptr_t vt{},cvt{};std::uint32_t type=~0u;
    return model::Read(g_read,p,0,vt) && vt==g_wh+model::kStandardVtableRva &&
        model::Read(g_read,p,0x1978,crv) && model::Read(g_read,crv,0,cvt) && cvt==g_wh+model::kCrvVtableRva &&
        model::Read(g_read,crv,0x14,type) && type==0;
}
void PrintSnapshot(const model::Snapshot& s,unsigned sample,std::uint64_t epoch) noexcept {
    Log("V66.12-PREFLIGHT SAMPLE epoch=%llu sample=%u pipeline=%p crv=%p frame=%u selector=%u builtCount=%llu "
        "qualified=%d stateRead=%d resourceOwner=%p ownerMatches=%d output=%p outputRead=%d "
        "pipelinePhase=%u dispatchMode=%u scheduleBefore=%u deferSetting=%u normalSnapshotOnly=1",
        (unsigned long long)epoch,sample,reinterpret_cast<void*>(s.pipeline),reinterpret_cast<void*>(s.crv),s.frame,s.selector,
        (unsigned long long)s.builtCount,s.qualified?1:0,s.stateRead?1:0,reinterpret_cast<void*>(s.resourceOwner),s.ownerMatches?1:0,
        reinterpret_cast<void*>(s.output),s.outputRead?1:0,s.phase,s.dispatchMode,static_cast<unsigned>(s.scheduleEnabled),s.deferSetting);
    for(unsigned eye=0;eye<2;++eye) {
        const auto& t=s.velocity[eye];
        Log("V66.12-PREFLIGHT TARGET epoch=%llu sample=%u eye=%u status=%s object=%p objectRead=%d "
            "backing=%p backingRead=%d width=%u height=%u dimensionsRead=%d viewTable=%p viewsRead=%d stableRead=%d "
            "descriptorValidity=UNVERIFIED gpuLifetime=UNVERIFIED allocationAttemptsTotal=%u",
            (unsigned long long)epoch,sample,eye,model::Name(t.status),reinterpret_cast<void*>(t.object),t.objectRead?1:0,
            reinterpret_cast<void*>(t.backing),t.backingRead?1:0,static_cast<unsigned>(t.width),static_cast<unsigned>(t.height),t.dimensionsRead?1:0,
            reinterpret_cast<void*>(t.views),t.viewsRead?1:0,t.stable?1:0,g_allocation_attempts.load());
    }
    const auto& l=s.velocity[0];const auto& r=s.velocity[1];
    const bool left=l.status==model::ResourceStatus::BasicFieldsPresent;
    const bool right=r.status==model::ResourceStatus::BasicFieldsPresent;
    const bool missing=s.ownerMatches && r.status==model::ResourceStatus::NoBacking;
    if(left)g_left_basic.fetch_add(1,std::memory_order_relaxed);
    if(right)g_right_basic.fetch_add(1,std::memory_order_relaxed);
    if(missing)g_right_missing.fetch_add(1,std::memory_order_relaxed);
    if(!s.ownerMatches || r.status==model::ResourceStatus::Unreadable || r.status==model::ResourceStatus::Changed)
        g_unknown.fetch_add(1,std::memory_order_relaxed);
    Log("V66.12-PREFLIGHT PAIR epoch=%llu sample=%u leftControlBasic=%d rightBasic=%d rightNoBackingObserved=%d "
        "sameObject=%d sameBacking=%d dimensionsMatch=%d verdict=OBSERVATION-ONLY normalSnapshotOnly=1",
        (unsigned long long)epoch,sample,left?1:0,right?1:0,missing?1:0,
        l.object && l.object==r.object?1:0,l.backing && l.backing==r.backing?1:0,
        l.dimensionsRead && r.dimensionsRead && l.width==r.width && l.height==r.height?1:0);
    for(unsigned bank=0;bank<2;++bank) {
        const auto& a=s.camera[bank][0];const auto& b=s.camera[bank][1];
        Log("V66.12-PREFLIGHT CAMERAS epoch=%llu sample=%u bank=%s crv=%p frame=%u "
            "leftRead=%d leftStable=%d leftEyeTag=%u leftHash=%016llX rightRead=%d rightStable=%d rightEyeTag=%u rightHash=%016llX "
            "producerEpoch=UNOBSERVED freshness=UNPROVEN copiedOrShifted=0",
            (unsigned long long)epoch,sample,bank==0?"CURRENT":"PREVIOUS",reinterpret_cast<void*>(s.crv),s.frame,
            a.readable?1:0,a.stable?1:0,a.eyeTag,(unsigned long long)a.hash,
            b.readable?1:0,b.stable?1:0,b.eyeTag,(unsigned long long)b.hash);
    }
    Log("V66.12-PREFLIGHT BUILT epoch=%llu sample=%u count=%llu record0Read=%d hash0=%016llX record1Read=%d hash1=%016llX "
        "ordering=selected-eye-plus-record-mod2 GPUbinding=UNOBSERVED",
        (unsigned long long)epoch,sample,(unsigned long long)s.builtCount,s.builtRead[0]?1:0,(unsigned long long)s.builtHash[0],
        s.builtRead[1]?1:0,(unsigned long long)s.builtHash[1]);
}

#include "native_stereo_allocation_step_v6603.inc"
#include "native_stereo_eye_control_v6604.inc"

// Keep the native call in a leaf-like SEH scope with only trivial locals.
std::uint8_t CallNative(void* pipeline) {
    std::uint8_t result=0;
    const auto original=g_original.load(std::memory_order_acquire);
    t_native_stage=1;
    __try { result=original(pipeline); }
    __except(RecordFault(GetExceptionInformation())) { /* filter always continues search */ }
    t_native_stage=0;return result;
}
std::uint8_t __fastcall StandardDetour(void* pipeline) {
    const DWORD enteringError=GetLastError();
    ++t_depth;g_inflight.fetch_add(1,std::memory_order_acq_rel);
    const auto savedPipeline=t_pipeline,savedCrv=t_crv;const auto savedEpoch=t_epoch;const auto savedStage=t_native_stage;
    t_pipeline=reinterpret_cast<std::uintptr_t>(pipeline);t_crv=0;t_epoch=g_epoch.load(std::memory_order_acquire);
    bool returned=false,sampled=false;unsigned sample=0;
    DWORD nativeError=enteringError;std::uint8_t result=0;
    __try {
        const auto state=g_state.load(std::memory_order_acquire);
        if(t_depth==1 && Active(state) && !InEyeControl()) {
            g_seen.fetch_add(1,std::memory_order_relaxed);
            if(Qualify(t_pipeline,t_crv)) {
                const auto tid=static_cast<unsigned long>(GetCurrentThreadId());
                unsigned long zero=0;(void)g_owner_tid.compare_exchange_strong(zero,tid,std::memory_order_acq_rel);
                if(g_owner_tid.load(std::memory_order_acquire)==tid) {
                    g_qualified.fetch_add(1,std::memory_order_relaxed);
                    int expected=Armed;
                    if(g_state.compare_exchange_strong(expected,Running,std::memory_order_acq_rel)) {
                        g_progress.Reset(GetTickCount64());
                        Log("V66.12-PREFLIGHT BEGIN epoch=%llu ownerTid=%lu normalPipelineOnly=1 replay=0 selectorWrites=0 countWrites=0",
                            (unsigned long long)t_epoch,tid);
                    }
                    const auto now=static_cast<std::uint64_t>(GetTickCount64());
                    // D85R5R4: native LEFT can resize after initial provisioning.
                    // Repair only a verified backed RIGHT before either current
                    // eye pass; keep the later strict velocity-size admission.
                    if(g_state.load(std::memory_order_acquire)==Running &&
                       g_run_kind.load(std::memory_order_acquire)>=2 &&
                       g_pipeline_inflight.load(std::memory_order_acquire)==1)
                        ResizeBackedRightVelocity(t_pipeline);
                    if(g_state.load(std::memory_order_acquire)==Running && g_progress.Due(now)) {
                        const auto snapshot=model::Capture(g_read,t_pipeline,g_wh);
                        if(snapshot.qualified) {
                            sample=g_progress.samples+1;PrintSnapshot(snapshot,sample,t_epoch);
                            // Initial unbacked creation remains the sample-9 one-shot.
                            // Backed size changes are handled above before native rendering.
                            if(sample==9) {
                                const int kind=g_run_kind.load(std::memory_order_acquire);
                                if(kind==1 || (kind>=2 && g_direct_design_needs_provisioning.load(std::memory_order_acquire) &&
                                               g_allocation_attempts.load(std::memory_order_acquire)==0)) {
                                    AllocationStep(snapshot);
                                }
                            }
                            g_progress.Record(now);
                            // V66.17: for a direct F9 launch, keep the eye-control gate below its
                            // four-sample threshold until the one-time sample-9 provisioning call
                            // has completed. Once provisioned, publish the real sample count and
                            // the existing eye-control path can run unchanged.
                            unsigned publishedSample=sample;
                            if(g_direct_design_needs_provisioning.load(std::memory_order_acquire) &&
                               g_allocation_attempts.load(std::memory_order_acquire)==0 && sample<9) publishedSample=3;
                            g_samples.store(publishedSample,std::memory_order_release);sampled=true;
                        }
                    }
                }
            }
        }
        SetLastError(enteringError);
        ObserveStandard(pipeline,true,0);
        SetLastError(enteringError);
        result=CallNative(pipeline); // exactly once for each native invocation, including the single control
        nativeError=GetLastError();returned=true;
        ObserveStandard(pipeline,false,result);
    } __finally {
        if(returned) {
            if(sampled) {
                std::uint8_t schedule=0;std::uint32_t phase=0;
                const bool fields=model::Read(g_read,t_pipeline,0x1A0,schedule) && model::Read(g_read,t_pipeline,0x1974,phase);
                Log("V66.12-PREFLIGHT STANDARD-RETURN epoch=%llu sample=%u completed=1 returnByte=%u "
                    "returnPreserved=1 postStateRead=%d scheduleAfter=%u pipelinePhaseAfter=%u nativeRecovery=NOT-NEEDED",
                    (unsigned long long)t_epoch,sample,static_cast<unsigned>(result),fields?1:0,static_cast<unsigned>(schedule),phase);
                // Repeated stereo stages deliberately continue beyond the 64-sample
                // diagnostic window; the sample window becomes telemetry-only there.
                if(g_progress.Complete() && g_run_kind.load(std::memory_order_acquire)<4) {
                    int running=Running;(void)g_state.compare_exchange_strong(running,Done,std::memory_order_acq_rel);
                }
            }
        } else {
            // Actual unwind, not a successful STANDARD-OUT. The filter already
            // records the first fault before unwinding. No engine rollback here.
            g_state.store(Faulted,std::memory_order_release);
            const char message[]="V66.12-PREFLIGHT STANDARD-ABORT completed=0 returnByteValid=0 nativeRecovery=NOT-ATTEMPTED\r\n";
            if(g_fault_file!=INVALID_HANDLE_VALUE) {
                DWORD written=0;(void)WriteFile(g_fault_file,message,static_cast<DWORD>(sizeof(message)-1),&written,nullptr);
            }
        }
        t_pipeline=savedPipeline;t_crv=savedCrv;t_epoch=savedEpoch;t_native_stage=savedStage;
        g_inflight.fetch_sub(1,std::memory_order_acq_rel);--t_depth;
        if(returned)SetLastError(nativeError);
    }
    return result; // raw AL contract, preserved after all observer work
}

bool ExactImage() noexcept {
    auto h=GetModuleHandleW(L"WHGame.dll");if(!h)return false;
    g_wh=reinterpret_cast<std::uintptr_t>(h);
    std::uint16_t mz=0,machine=0,magic=0;std::uint32_t peOff=0,peSig=0,timestamp=0,imageSize=0;
    if(!model::Read(g_read,g_wh,0,mz) || mz!=0x5A4D || !model::Read(g_read,g_wh,0x3C,peOff) || peOff>0x10000 ||
       !model::Read(g_read,g_wh,peOff,peSig) || peSig!=0x4550 || !model::Read(g_read,g_wh,peOff+4,machine) || machine!=0x8664 ||
       !model::Read(g_read,g_wh,peOff+8,timestamp) || timestamp!=model::kTimestamp ||
       !model::Read(g_read,g_wh,peOff+24,magic) || magic!=0x20B ||
       !model::Read(g_read,g_wh,peOff+24+56,imageSize) || imageSize!=model::kImageSize) return false;
    for(const auto& signature:kSignatures) {
        unsigned char bytes[24]{};
        if(!g_read(g_wh+signature.rva,bytes,signature.size) || std::memcmp(bytes,signature.bytes,signature.size)!=0) {
            Log("V66.12-PREFLIGHT REFUSED reason=instruction-contract rva=0x%llX",(unsigned long long)signature.rva);return false;
        }
    }
    for(const auto& signature:kResourceSignatures) {
        unsigned char bytes[32]{};
        if(!g_read(g_wh+signature.rva,bytes,signature.size) || std::memcmp(bytes,signature.bytes,signature.size)!=0) {
            Log("V66.12-PREFLIGHT REFUSED reason=resource-instruction-contract rva=0x%llX",(unsigned long long)signature.rva);return false;
        }
    }
    for(const auto& range:kResourceRanges) {
        unsigned char bytes[4096]{};
        if(range.size>sizeof(bytes) || !g_read(g_wh+range.rva,bytes,range.size) || model::Hash(bytes,range.size)!=range.hash) {
            Log("V66.12-PREFLIGHT REFUSED reason=resource-code-range rva=0x%llX",(unsigned long long)range.rva);return false;
        }
    }
    std::uintptr_t textureNameGetter=0;
    if(!model::Read(g_read,g_wh,resource_model::kTextureVtableRva+0x20,textureNameGetter) || textureNameGetter!=g_wh+0x1A72CA0)return false;
    std::uintptr_t deviceTextureDestructor=0;
    if(!model::Read(g_read,g_wh,resource_model::kDeviceTextureVtableRva,deviceTextureDestructor) ||
       deviceTextureDestructor!=g_wh+0x1DFDB50)return false;
    std::uintptr_t target=0;
    return model::Read(g_read,g_wh,model::kStandardVtableRva+0x40,target) && target==g_wh+model::kStandardRva;
}
bool PrepareSinkAndPin() noexcept {
    HMODULE self=nullptr;wchar_t path[32768]{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
                          reinterpret_cast<LPCWSTR>(&StandardDetour),&self))return false;
    const DWORD n=GetModuleFileNameW(self,path,static_cast<DWORD>(sizeof(path)/sizeof(path[0])));
    if(!n || n>=sizeof(path)/sizeof(path[0]))return false;
    DWORD split=n;while(split && path[split-1]!=L'\\' && path[split-1]!=L'/')--split;
    if(!split)return false;
    const wchar_t name[]=L"KCD2VR.log";
    if(split+sizeof(name)/sizeof(name[0])>sizeof(path)/sizeof(path[0]))return false;
    for(std::size_t i=0;i<sizeof(name)/sizeof(name[0]);++i)path[split+i]=name[i];
    g_fault_file=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
                           nullptr,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    return g_fault_file!=INVALID_HANDLE_VALUE;
}
bool Install() noexcept {
    if(g_installed.load(std::memory_order_acquire))return true;
    if(g_setup_failed.load(std::memory_order_acquire))return false;
    if(GetModuleHandleW(L"KCD2StereoProbe.asi")) {
        Log("V66.12-PREFLIGHT REFUSED reason=companion-loaded remove-companion=1");return false;
    }
    if(!ExactImage() || !CheckEyeContracts()) {Log("V66.12-PREFLIGHT REFUSED reason=exact-retail-gate");return false;}
    if(!PrepareSinkAndPin()) {Log("V66.12-PREFLIGHT REFUSED reason=fault-sink-or-module-pin");return false;}
    // No live unhook/reset. A partial hook setup is terminal for this process.
    g_setup_failed.store(true,std::memory_order_release);
    DetourModKit::hook::InlineRequest request{"v6604_eye_control_standard_byte",
        DetourModKit::hook::Target{DetourModKit::Address{g_wh+model::kStandardRva}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(request),static_cast<StandardFn>(&StandardDetour));
    if(!h)return false;
    g_hook=std::move(*h);g_original.store(g_hook->original<StandardFn>(),std::memory_order_release);
    if(!g_original.load(std::memory_order_acquire) || !PrepareEyeHooks())return false;
    if(!static_cast<bool>(g_hook->enable()) || !EnableEyeHooks())return false;
    g_installed.store(true,std::memory_order_release);
    Log("V66.12-PREFLIGHT READY hooks=8 boundary=StandardExecute+57362C semanticFinalCompose=WHGame+8645F8 exactComposeCall=WHGame+2543A6C returnType=uint8 modulePinned=1 "
        "readMethod=ReadProcessMemory F11=ONE-GATED-ZERO-SEPARATION-PROOF F9=POST-PROOF-REAL-IPD-DESIGN "
        "selectorWrites=SCOPED countWrites=DISABLED initialCreation=ONE-GATED-NATIVE-CALL backedResize=PRE-STANDARD-VERIFIED");
    return true;
}
} // namespace

bool InUpscalerView() noexcept {
    const auto* f=t_eye_flow;
    if(f && IsStereoDesign(*f) && f->pass!=ControlPass::Restore)return true;
    // AFR has no native left/right re-entry scope. R149: FrameSync resolves the
    // exact eye of the camera this Normal execute renders (publication-ring
    // match); never the main thread's newer CurrentEye() outside that owner.
    return FrameSync::AlternateEyeUpscalerIdentity(nullptr,nullptr,nullptr,nullptr);
}
bool CurrentUpscalerView(UpscalerView& out) noexcept {
    out={};const auto* f=t_eye_flow;
    if(!InUpscalerView())return false;
    if(!(f && IsStereoDesign(*f) && f->pass!=ControlPass::Restore)){
        std::uint64_t epoch=0;std::uint32_t frame=0,eye=0;std::uintptr_t pipeline=0;
        if(!FrameSync::AlternateEyeUpscalerIdentity(&epoch,&frame,&eye,&pipeline))return false;
        out={epoch,frame,eye&1u,pipeline};
        return out.epoch!=0&&out.frame!=0&&out.pipeline!=0;
    }
    if(f->fallback)return false;
    const unsigned eye=f->pass==ControlPass::Eye1?1u:0u;
    const auto& work=eye?f->eye:f->normal;
    if((eye?!f->selected:!f->leftPrepared) || work.buildReturns!=1 ||
       !work.builtPointerValid || !work.cameraSourceValid || !work.standardStateValid)return false;
    std::uint32_t frame=0,selector=~0u;std::uintptr_t pipeline=0;
    if(!model::Read(g_read,f->crv,0x2C,frame) || frame!=f->frame ||
       !model::Read(g_read,f->crv,0x8EE0,selector) || selector!=eye ||
       !model::Read(g_read,f->crv,0x8D60,pipeline) || pipeline!=f->pipeline)return false;
    out={f->epoch,f->frame,eye,f->pipeline};return true;
}

bool Busy() noexcept {
    return Active(g_state.load(std::memory_order_acquire)) || g_pending_design_stage.load(std::memory_order_acquire)!=0;
}
int Phase() noexcept {
    if(g_velocity_resize_failed.load(std::memory_order_acquire))return -2;
    if(g_pending_design_stage.load(std::memory_order_acquire)!=0)return -1;
    const int s=g_state.load(std::memory_order_acquire);
    return s==Armed?-1:((s==Running || s==Allocating || s==Witnessing)?1:((s==Idle || s==Done)?0:-2));
}
bool Start() noexcept {
    if(RejectAfterVelocityResizeFailure("F11-start"))return false;
    if(Busy())return false;
    if(g_first_fault.load(std::memory_order_acquire)!=0 || g_inflight.load(std::memory_order_acquire)!=0 ||
       g_pipeline_inflight.load(std::memory_order_acquire)!=0) {
        Log("V66.12-PREFLIGHT REFUSED reason=earlier-fault-or-native-call-in-flight");return false;
    }
    // F11 is a one-shot proof gate. Once attempted, use its recorded verdict;
    // do not silently rerun a failed native mutation in the same process.
    if(g_eye_control_attempts.load(std::memory_order_acquire)!=0 || g_eye_decision_used.load(std::memory_order_acquire)) {
        Log("V66.12-PREFLIGHT REFUSED reason=f11-proof-already-consumed verified=%d outcome=%s",
            g_eye1_verified.load()?1:0,EyeOutcomeName(g_eye_outcome.load()));return false;
    }
    int previous=g_state.load(std::memory_order_acquire);
    if(Active(previous) || previous==Preparing || previous==Faulted ||
       !g_state.compare_exchange_strong(previous,Preparing,std::memory_order_acq_rel))return false;
    if(!Install()) {g_state.store(Refused,std::memory_order_release);Log("V66.12-PREFLIGHT REFUSED reason=setup see-previous-line=1");return false;}
    g_run_kind.store(1,std::memory_order_release);g_last_run_passed.store(false,std::memory_order_release);
    g_eye_outcome.store(0,std::memory_order_release);g_f11_outcome.store(0,std::memory_order_release);g_eye_decision_used.store(false,std::memory_order_release);
    g_samples.store(0);g_left_basic.store(0);g_right_basic.store(0);g_right_missing.store(0);g_unknown.store(0);
    g_seen.store(0);g_qualified.store(0);g_owner_tid.store(0);g_allocation_outcome.store(0);
    const auto epochD82=g_epoch.fetch_add(1,std::memory_order_acq_rel)+1;
    native_upscaler_stereo_d82::BeginEpoch(epochD82);
    g_started.store(GetTickCount64(),std::memory_order_release);
    Log("V66.12-PREFLIGHT ARMED samples=%u runKind=F11-PROOF launch=normal-exe environment=ignored "
        "cameraPolicy=ZERO-SEPARATION-NATIVE-SETTERS maxExtraPipelineCalls=1 selectorPolicy=SCOPED countWrites=0 "
        "allocationPolicy=ONE-ATTEMPT-PER-PROCESS eyeCapture=0 F9=LOCKED-UNTIL-VERIFIED",model::Progress::kSamples);
    // Re-check after the zero-inflight observations: a preceding native call
    // can publish an unconfirmed-resize latch while a worker starts this run.
    if(RejectAfterVelocityResizeFailure("arm")) {
        native_upscaler_stereo_d82::EndEpoch(epochD82);
        int preparing=Preparing;
        (void)g_state.compare_exchange_strong(preparing,Faulted,std::memory_order_acq_rel);
        return false;
    }
    int preparing=Preparing;
    return g_state.compare_exchange_strong(preparing,Armed,std::memory_order_acq_rel);
}

bool BeginStereoDesignNow(int stage,float ipd) noexcept {
    // Called only after the request was validated and the renderer is between
    // tracked native calls. Re-check all terminal/scheduling gates here because
    // a queued request can wait for more than one Poll() iteration.
    if(stage<1||stage>4)return false;
    if(RejectAfterVelocityResizeFailure("design-begin"))return false;
    if(Active(g_state.load(std::memory_order_acquire)) || g_state.load(std::memory_order_acquire)==Preparing ||
       g_state.load(std::memory_order_acquire)==Faulted || g_first_fault.load(std::memory_order_acquire)!=0 ||
       g_inflight.load(std::memory_order_acquire)!=0 || g_pipeline_inflight.load(std::memory_order_acquire)!=0)return false;
    // V66.17: V66.15 proved the real-IPD path, so direct stage-4 entry is valid.
    // The old F11-verified and stage1->2->3 progression gates are intentionally retired here.
    if(!(ipd>=0.03f&&ipd<=0.10f))ipd=0.064f;
    int previous=g_state.load(std::memory_order_acquire);
    if(Active(previous)||previous==Preparing||previous==Faulted||
       !g_state.compare_exchange_strong(previous,Preparing,std::memory_order_acq_rel))return false;
    if(!Install()){g_state.store(Refused,std::memory_order_release);return false;}
    const bool directNeedsProvision = !g_eye1_verified.load(std::memory_order_acquire) &&
                                      g_allocation_attempts.load(std::memory_order_acquire)==0;
    g_direct_design_needs_provisioning.store(directNeedsProvision,std::memory_order_release);
    g_run_kind.store(stage+1,std::memory_order_release);g_design_ipd.store(ipd,std::memory_order_release);
    g_eye_decision_used.store(false,std::memory_order_release);g_eye_outcome.store(0,std::memory_order_release);
    g_last_run_passed.store(false,std::memory_order_release);
    if(stage>=2)g_design_pair_serial.store(0,std::memory_order_release);
    g_repeat_completed.store(0,std::memory_order_release);
    g_repeat_remaining.store(stage==3?kStereoBurstFrames:0,std::memory_order_release);
    g_samples.store(0);g_left_basic.store(0);g_right_basic.store(0);g_right_missing.store(0);g_unknown.store(0);
    g_seen.store(0);g_qualified.store(0);g_owner_tid.store(0);
    const auto epochD82=g_epoch.fetch_add(1,std::memory_order_acq_rel)+1;
    native_upscaler_stereo_d82::BeginEpoch(epochD82);
    g_started.store(GetTickCount64(),std::memory_order_release);
    Log("V66.12-DESIGN ARMED stage=%d ipd=%.6f minimumNormalSamples=4 count2=0 outputCapture=%d "
        "repeatPolicy=%s repeatTarget=%u leftRightPolicy=SYMMETRIC-RUNTIME-IPD "
        "nativeCameraAssign=WHGame+0x426E80 launchBoundary=SAFE-ZERO-INFLIGHT directProvision=%d",
        stage,double(ipd),stage>=2?1:0,stage==3?"BURST-30":(stage==4?"CONTINUOUS":"ONE-SHOT"),
        stage==3?kStereoBurstFrames:0u,directNeedsProvision?1:0);
    // Re-check after the zero-inflight observations: a preceding native call
    // can publish an unconfirmed-resize latch while a worker starts this run.
    if(RejectAfterVelocityResizeFailure("arm")) {
        native_upscaler_stereo_d82::EndEpoch(epochD82);
        int preparing=Preparing;
        (void)g_state.compare_exchange_strong(preparing,Faulted,std::memory_order_acq_rel);
        return false;
    }
    int preparing=Preparing;
    return g_state.compare_exchange_strong(preparing,Armed,std::memory_order_acq_rel);
}

bool StartStereoDesign(int stage,float ipd) noexcept {
    if(stage<1||stage>4)return false;
    if(RejectAfterVelocityResizeFailure("design-start"))return false;
    /* V66.18: direct stage-4 start is allowed without prior F11 once the V66.15 native stereo proof succeeded on this game build. */
    if(g_first_fault.load(std::memory_order_acquire)!=0) {
        Log("V66.12-DESIGN REFUSED stage=%d reason=earlier-native-fault",stage);return false;
    }
    if(Active(g_state.load(std::memory_order_acquire)) || g_state.load(std::memory_order_acquire)==Preparing ||
       g_pending_design_stage.load(std::memory_order_acquire)!=0) {
        Log("V66.12-DESIGN REFUSED stage=%d reason=design-active-or-pending",stage);return false;
    }
    const int passed=g_last_design_stage_passed.load(std::memory_order_acquire);
    if(stage==1 && passed>=1){Log("V66.12-DESIGN REFUSED stage=1 reason=already-passed");return false;}    if(stage==2 && passed>=2){Log("V66.12-DESIGN REFUSED stage=2 reason=already-passed");return false;}
    if(stage==3 && passed>=3){Log("V66.12-DESIGN REFUSED stage=3 reason=already-passed");return false;}
    if(!(ipd>=0.03f&&ipd<=0.10f))ipd=0.064f;

    if(g_inflight.load(std::memory_order_acquire)!=0 || g_pipeline_inflight.load(std::memory_order_acquire)!=0) {
        int empty=0;
        if(!g_pending_design_stage.compare_exchange_strong(empty,stage,std::memory_order_acq_rel))return false;
        g_pending_design_ipd.store(ipd,std::memory_order_release);
        Log("V66.12-DESIGN QUEUED stage=%d ipd=%.6f reason=transient-native-unwind standardInflight=%u pipelineInflight=%u "
            "policy=AUTO-ARM-AT-NEXT-ZERO-INFLIGHT",
            stage,double(ipd),g_inflight.load(),g_pipeline_inflight.load());
        return true;
    }
    return BeginStereoDesignNow(stage,ipd);
}

void TryStartPendingDesign() noexcept {
    const int stage=g_pending_design_stage.load(std::memory_order_acquire);
    if(stage==0)return;
    if(RejectAfterVelocityResizeFailure("pending-design")) {
        int expected=stage;
        (void)g_pending_design_stage.compare_exchange_strong(expected,0,std::memory_order_acq_rel);
        return;
    }
    if(g_first_fault.load(std::memory_order_acquire)!=0) {
        int expected=stage;
        if(g_pending_design_stage.compare_exchange_strong(expected,0,std::memory_order_acq_rel))
            Log("V66.12-DESIGN PENDING-CANCELLED stage=%d reason=first-fault",stage);
        return;
    }
    if(Active(g_state.load(std::memory_order_acquire)) || g_state.load(std::memory_order_acquire)==Preparing ||
       g_inflight.load(std::memory_order_acquire)!=0 || g_pipeline_inflight.load(std::memory_order_acquire)!=0)return;
    const float ipd=g_pending_design_ipd.load(std::memory_order_acquire);
    // Keep the request latched while attempting the safe transition. A renderer
    // thread can enter between the zero-counter observation above and the CAS in
    // BeginStereoDesignNow(); if that happens, Begin returns false and Poll retries
    // later instead of losing the user's F9 request.
    if(BeginStereoDesignNow(stage,ipd)) {
        int expected=stage;
        (void)g_pending_design_stage.compare_exchange_strong(expected,0,std::memory_order_acq_rel);
        Log("V66.12-DESIGN DEQUEUED stage=%d ipd=%.6f action=ARMED-AT-ZERO-INFLIGHT",stage,double(ipd));
    }
}

bool Eye1Verified() noexcept {return g_eye1_verified.load(std::memory_order_acquire);}
unsigned Eye1Outcome() noexcept {return g_f11_outcome.load(std::memory_order_acquire);}
int LastRunKind() noexcept {return g_run_kind.load(std::memory_order_acquire);}
bool LastRunPassed() noexcept {return !g_velocity_resize_failed.load(std::memory_order_acquire) &&
    g_last_run_passed.load(std::memory_order_acquire);}
int LastDesignStagePassed() noexcept {return g_last_design_stage_passed.load(std::memory_order_acquire);}
std::uint64_t LastDesignPairSerial() noexcept {return g_velocity_resize_failed.load(std::memory_order_acquire)?0:
    g_design_pair_serial.load(std::memory_order_acquire);}

void Stop() noexcept {
    const int pending=g_pending_design_stage.exchange(0,std::memory_order_acq_rel);
    if(pending!=0) {
        g_state.store(Cancelled,std::memory_order_release);
        Log("V66.12-PREFLIGHT STOPPED source=F9-PENDING designStage=%d stopEffective=NOW nativeCallInterrupted=0",pending);
        return;
    }
    int s=g_state.load(std::memory_order_acquire);
    const int kind=g_run_kind.load(std::memory_order_acquire);
    // Continuous mode is user-owned once stage4 is running. F9 toggles it off
    // cleanly at a CPU boundary; do not label that intentional stop a failure.
    if(kind==5 && Active(s)) {
        while(Active(s)) if(g_state.compare_exchange_weak(s,Done,std::memory_order_acq_rel)) {
            g_last_run_passed.store(true,std::memory_order_release);
            g_last_design_stage_passed.store(4,std::memory_order_release);
            Log("V66.12-CONTINUOUS STOPPED clean=1 completedFrames=%u nativeCallInterrupted=0 lastPairSerial=%llu",
                g_repeat_completed.load(),(unsigned long long)g_design_pair_serial.load());return;
        }
    }
    while(Active(s)) if(g_state.compare_exchange_weak(s,Cancelled,std::memory_order_acq_rel)) {
        Log("V66.12-PREFLIGHT STOPPED source=%s runKind=%d stopEffective=NOW claimedNativeWorkInterrupted=0 resourceRelease=NOT-ATTEMPTED nativeCallInterrupted=0 nativeRecovery=NOT-ATTEMPTED",
            kind==1?"F11":"F9",kind);return;
    }
}

void Poll() noexcept {
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const auto start=g_started.load(std::memory_order_acquire);
    int s=g_state.load(std::memory_order_acquire);
    const bool unlimited=g_run_kind.load(std::memory_order_acquire)==5;
    if(!unlimited && Active(s) && now>=start && now-start>=model::Progress::kTimeoutMs) {
        while(Active(s)) if(g_state.compare_exchange_weak(s,TimedOut,std::memory_order_acq_rel))break;
    }
    TryStartPendingDesign();
    // Worker-owned notification cursor. No engine state, waits, or resets here.
    static std::uint64_t lastEpoch=0;static int lastState=Idle;
    const auto epoch=g_epoch.load(std::memory_order_acquire);s=g_state.load(std::memory_order_acquire);
    if(g_velocity_resize_failed.load(std::memory_order_acquire))s=Faulted; // Stop cannot mask the failure summary
    if((epoch!=lastEpoch || s!=lastState) && !Active(s) && s!=Idle) {
        const int kind=g_run_kind.load(std::memory_order_acquire);
        const int designStage=kind>=2?kind-1:0;
        const unsigned expectedSamples=designStage?4u:model::Progress::kSamples;
        Log("V66.12-PREFLIGHT SUMMARY epoch=%llu status=%s runKind=%d designStage=%d passed=%d eye1Verified=%d "
            "f11Outcome=%s samples=%u expectedAtLeast=%u callsSeen=%llu qualified=%llu "
            "leftBasic=%u rightBasic=%u rightNoBacking=%u unknown=%u standardInflight=%u pipelineInflight=%u "
            "countWrites=0 allocationAttemptsTotal=%u allocationOutcome=%s helperReturnByte=%u "
            "eyeDecisionUsed=%d eyeAttemptsTotal=%u extraPipelineCalls=%u extraPipelineReturns=%u "
            "eyeOutcome=%s eyeGbufferReturns=%u eyeOpaqueReturns=%u eyeNonemptyRequests=%u "
            "designStagePassed=%d designPairSerial=%llu nativeRecovery=NOT-ATTEMPTED completionMeans=%s",
            (unsigned long long)epoch,StateName(s),kind,designStage,LastRunPassed()?1:0,g_eye1_verified.load()?1:0,
            EyeOutcomeName(g_f11_outcome.load()),g_samples.load(),expectedSamples,(unsigned long long)g_seen.load(),(unsigned long long)g_qualified.load(),
            g_left_basic.load(),g_right_basic.load(),g_right_missing.load(),g_unknown.load(),g_inflight.load(),g_pipeline_inflight.load(),
            g_allocation_attempts.load(),AllocationName(g_allocation_outcome.load()),g_allocation_return.load(),
            g_eye_decision_used.load()?1:0,g_eye_control_attempts.load(),g_eye_control_calls.load(),g_eye_control_returns.load(),
            EyeOutcomeName(g_eye_outcome.load()),g_eye_gbuffer_returns.load(),g_eye_opaque_returns.load(),g_eye_nonempty_requests.load(),
            g_last_design_stage_passed.load(),(unsigned long long)g_design_pair_serial.load(),
            kind==1?"F11-proof-verdict-not-stereo":
            (designStage==1?"one-real-IPD-LR-CPU-frame":
            (designStage==2?"one-preserved-LR-frame":
            (designStage==3?"30-frame-live-stereo-burst":"continuous-live-stereo"))));
    }
    lastEpoch=epoch;lastState=s;
}

} // namespace kcd2vr::native_stereo_preflight
