#include "native_taa_pair_epoch_v6661.h"
#include "diagnostic_trace.h"
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <limits>

namespace kcd2vr::native_taa_pair_epoch_v6661 { namespace {

constexpr std::uint32_t kTimestamp = 0x6A350E20u;
constexpr std::uintptr_t kHistoryGetterRva = 0xFFDCACull;
constexpr std::uintptr_t kGlobalFrameRva = 0x547DB20ull;
constexpr std::size_t kGetterSize = 0x31;
constexpr unsigned char kGetterWhole[kGetterSize] = {
    0x8B,0x05,0x6E,0xFE,0x47,0x04,0x4C,0x8B,0xC9,0x45,0x84,0xC0,0x75,0x02,0xFF,0xC0,
    0x48,0x63,0x8A,0xE0,0x8E,0x00,0x00,0x25,0x01,0x00,0x00,0x80,0x7D,0x07,0xFF,0xC8,
    0x83,0xC8,0xFE,0xFF,0xC0,0x48,0x98,0x48,0x8D,0x04,0x48,0x49,0x8B,0x44,0xC1,0x40,
    0xC3
};
constexpr std::size_t kPatchLen = 6;
constexpr std::size_t kStubSize = 64;
constexpr std::size_t kControlOffset = 63;

std::atomic<bool> g_installed{false};
std::uintptr_t g_base=0;
std::uint8_t* g_stub=nullptr;
volatile char* g_control=nullptr;
unsigned char g_saved[kPatchLen]{};

bool ExactBuild(std::uintptr_t b) noexcept {
    if(!b) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(b);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(b+dos->e_lfanew);
        if(nt->Signature!=IMAGE_NT_SIGNATURE || nt->FileHeader.TimeDateStamp!=kTimestamp) return false;
        return std::memcmp(reinterpret_cast<const void*>(b+kHistoryGetterRva),kGetterWhole,kGetterSize)==0;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool FitsRel32(std::uintptr_t from_after, std::uintptr_t to) noexcept {
    const auto d=static_cast<std::int64_t>(to)-static_cast<std::int64_t>(from_after);
    return d>=std::numeric_limits<std::int32_t>::min() && d<=std::numeric_limits<std::int32_t>::max();
}

std::uint8_t* AllocNear(std::uintptr_t target) noexcept {
    SYSTEM_INFO si{}; GetSystemInfo(&si);
    const std::uintptr_t gran=si.dwAllocationGranularity?si.dwAllocationGranularity:0x10000u;
    const std::uintptr_t base=target & ~(gran-1u);
    constexpr std::uintptr_t kMax=0x70000000ull;
    for(std::uintptr_t d=0; d<=kMax; d+=gran) {
        const std::uintptr_t cand[2]={base+d,(base>d)?base-d:0};
        for(auto a:cand) {
            if(!a || !FitsRel32(target+5,a)) continue;
            void* p=VirtualAlloc(reinterpret_cast<void*>(a),0x1000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE);
            if(p && FitsRel32(target+5,reinterpret_cast<std::uintptr_t>(p))) return static_cast<std::uint8_t*>(p);
            if(p) VirtualFree(p,0,MEM_RELEASE);
        }
    }
    return nullptr;
}

void Put32(std::uint8_t* p,std::int32_t v) noexcept { std::memcpy(p,&v,sizeof(v)); }

bool BuildStub(std::uintptr_t target,std::uintptr_t global_frame) noexcept {
    g_stub=AllocNear(target);
    if(!g_stub) return false;
    std::memset(g_stub,0xCC,0x1000);

    // The retail caller at WHGame+0x661E74 intentionally reuses RDX for the second getter call.
    // Therefore the replacement MUST preserve the getter's real register footprint. This stub
    // uses only RAX/RCX/R9/flags, exactly like retail, and never touches RDX/R10/R8.
    //
    // 00: cmp byte ptr [rip+control],0
    // 07: je retail_global
    // 09: mov eax,[rdx+0x2c]         ; CRenderView game/pair frame
    // 0c: jmp common
    // 0e: mov eax,[rip+globalFrame]  ; exact retail global render-pass frame
    // 14: common = original getter bytes from +6 through RET
    // 3f: control byte (F2 toggles this byte only)
    std::size_t o=0;
    g_stub[o++]=0x80; g_stub[o++]=0x3D;
    const auto control_addr=reinterpret_cast<std::uintptr_t>(g_stub)+kControlOffset;
    const auto cmp_next=reinterpret_cast<std::uintptr_t>(g_stub)+7;
    if(!FitsRel32(cmp_next,control_addr)) return false;
    Put32(g_stub+o,static_cast<std::int32_t>(static_cast<std::int64_t>(control_addr)-static_cast<std::int64_t>(cmp_next))); o+=4;
    g_stub[o++]=0x00;
    g_stub[o++]=0x74; g_stub[o++]=0x05;
    g_stub[o++]=0x8B; g_stub[o++]=0x42; g_stub[o++]=0x2C;
    g_stub[o++]=0xEB; g_stub[o++]=0x06;
    g_stub[o++]=0x8B; g_stub[o++]=0x05;
    const auto mov_next=reinterpret_cast<std::uintptr_t>(g_stub)+20;
    if(!FitsRel32(mov_next,global_frame)) return false;
    Put32(g_stub+o,static_cast<std::int32_t>(static_cast<std::int64_t>(global_frame)-static_cast<std::int64_t>(mov_next))); o+=4;
    if(o!=20) return false;
    std::memcpy(g_stub+o,kGetterWhole+6,kGetterSize-6); o+=kGetterSize-6;
    if(o!=63) return false;
    g_stub[kControlOffset]=0;
    g_control=reinterpret_cast<volatile char*>(g_stub+kControlOffset);
    FlushInstructionCache(GetCurrentProcess(),g_stub,kStubSize);
    return true;
}

bool PatchEntry(std::uintptr_t target) noexcept {
    if(!g_stub || !FitsRel32(target+5,reinterpret_cast<std::uintptr_t>(g_stub))) return false;
    std::memcpy(g_saved,reinterpret_cast<const void*>(target),kPatchLen);
    unsigned char patch[kPatchLen]{0xE9,0,0,0,0,0x90};
    Put32(patch+1,static_cast<std::int32_t>(static_cast<std::int64_t>(reinterpret_cast<std::uintptr_t>(g_stub))-static_cast<std::int64_t>(target+5)));
    DWORD old=0;
    if(!VirtualProtect(reinterpret_cast<void*>(target),kPatchLen,PAGE_EXECUTE_READWRITE,&old)) return false;
    std::memcpy(reinterpret_cast<void*>(target),patch,kPatchLen);
    FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(target),kPatchLen);
    DWORD dummy=0; (void)VirtualProtect(reinterpret_cast<void*>(target),kPatchLen,old,&dummy);
    return true;
}

void RestoreEntry() noexcept {
    if(!g_base) return;
    const auto target=g_base+kHistoryGetterRva;
    DWORD old=0;
    if(VirtualProtect(reinterpret_cast<void*>(target),kPatchLen,PAGE_EXECUTE_READWRITE,&old)) {
        std::memcpy(reinterpret_cast<void*>(target),g_saved,kPatchLen);
        FlushInstructionCache(GetCurrentProcess(),reinterpret_cast<void*>(target),kPatchLen);
        DWORD dummy=0; (void)VirtualProtect(reinterpret_cast<void*>(target),kPatchLen,old,&dummy);
    }
}

} // namespace

bool Install() noexcept {
    if(g_installed.load(std::memory_order_acquire)) return true;
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!ExactBuild(b)) { diagnostic_trace::Write("V66.62-TAA-PAIR-EPOCH install REFUSED exact getter mismatch"); return false; }
    g_base=b;
    if(!BuildStub(b+kHistoryGetterRva,b+kGlobalFrameRva) || !PatchEntry(b+kHistoryGetterRva)) {
        if(g_stub) { VirtualFree(g_stub,0,MEM_RELEASE); g_stub=nullptr; }
        g_control=nullptr; g_base=0;
        diagnostic_trace::Write("V66.62-TAA-PAIR-EPOCH install FAILED near-stub/entry-patch");
        return false;
    }
    g_installed.store(true,std::memory_order_release);
    diagnostic_trace::Write("V66.62-TAA-PAIR-EPOCH installed DEFAULT=OFF implementation=REGISTER-PRESERVING-STUB F2=CONTROL-BYTE-ONLY");
    return true;
}

void Uninstall() noexcept {
    if(!g_installed.exchange(false,std::memory_order_acq_rel)) return;
    if(g_control) InterlockedExchange8(g_control,0);
    RestoreEntry();
    g_control=nullptr;
    if(g_stub) { VirtualFree(g_stub,0,MEM_RELEASE); g_stub=nullptr; }
    g_base=0;
}

bool Installed() noexcept { return g_installed.load(std::memory_order_acquire); }
void SetEnabled(bool enabled) noexcept {
    if(g_control) InterlockedExchange8(g_control,enabled?1:0);
}
bool Enabled() noexcept { return g_control && (*g_control!=0); }

void LogStats(const char* reason) noexcept {
    char m[320]{};
    std::snprintf(m,sizeof(m),
        "V66.62-TAA-PAIR-EPOCH-STATS reason=%s installed=%d enabled=%d implementation=REGISTER-PRESERVING-STUB control=%p",
        reason?reason:"periodic",Installed()?1:0,Enabled()?1:0,(const void*)g_control);
    diagnostic_trace::Write(m);
}

} // namespace kcd2vr::native_taa_pair_epoch_v6661
