#include "native_postaa_repair_v6632.h"
#include "diagnostic_trace.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <optional>
#include <utility>
#include <cstdio>

namespace kcd2vr::native_postaa_repair_v6632 { namespace {

constexpr std::uint32_t kTimestamp = 0x6A350E20u;
constexpr std::uintptr_t kPostAAVtableRva = 0x3C3FE80ull;
constexpr std::uintptr_t kResizeRva = 0xC1AF80ull;
constexpr std::uintptr_t kProvisionHelperRva = 0x73CEF4ull;
constexpr std::uintptr_t kLeft0 = 0x40ull;
constexpr std::uintptr_t kLeft1 = 0x48ull;
constexpr std::uintptr_t kRight0 = 0x50ull;
constexpr std::uintptr_t kRight1 = 0x58ull;
constexpr std::uintptr_t kTextureSourceFormat = 0xA4ull;
constexpr std::uintptr_t kTextureFlags = 0x98ull;
constexpr std::uintptr_t kTextureBacking = 0x80ull;
constexpr std::uint32_t kPostAAFlags = 0x00041001u;
// V66.33: retail Resize names each history "$PrevBackBuffer0" + pipelineName, where
// pipelineName is the MSVC std::string at [stage+0x08]+0x1990 (see 0x180C1AFEB..0x180C1B019).
// The texture creator (0x18073C634 -> 0x1806A0A10) is NAME-KEYED and resizes an existing
// texture with the same name, so RIGHT names must carry the same pipeline suffix.
constexpr std::uintptr_t kStagePipeline = 0x08ull;
constexpr std::uintptr_t kPipelineName = 0x1990ull;

constexpr unsigned char kResizeEntry[] = {
    0x40,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x56,0x41,0x57,0x48,
    0x8D,0x6C,0x24,0xD9,0x48,0x81,0xEC,0xB0,0x00,0x00,0x00,0x48
};
constexpr unsigned char kProvisionEntry[] = {
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,0x48,
    0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x41,0x56,0x48,0x83,0xEC
};

struct Color { float r=0.0f,g=0.0f,b=0.0f,a=0.0f; };
using ResizeFn = void(__fastcall*)(void*,std::int32_t,std::int32_t);
using ProvisionFn = std::uint8_t(__fastcall*)(const char*,void**,std::int32_t,std::int32_t,
                                             const Color*,std::uint8_t,std::uint8_t,
                                             std::uint8_t,std::int32_t,std::uint32_t);

std::optional<DetourModKit::hook::Hook> g_hook;
ResizeFn g_original=nullptr;
std::uintptr_t g_wh=0;
std::atomic<bool> g_installed{false};
std::atomic<bool> g_ready{false};
std::atomic<std::uint64_t> g_resize_calls{0};
std::atomic<std::uint64_t> g_provision_successes{0};

bool Readable(const void* p,std::size_t n=1) noexcept {
    if(!p || n==0) return false;
    MEMORY_BASIC_INFORMATION m{};
    if(!VirtualQuery(p,&m,sizeof(m)) || m.State!=MEM_COMMIT || (m.Protect&(PAGE_NOACCESS|PAGE_GUARD))) return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p);
    const auto e=reinterpret_cast<std::uintptr_t>(m.BaseAddress)+m.RegionSize;
    return a<=e && n<=e-a;
}

template<class T> bool SafeRead(std::uintptr_t p,T& out) noexcept {
    if(!Readable(reinterpret_cast<void*>(p),sizeof(T))) return false;
    __try { out=*reinterpret_cast<const T*>(p); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool ExactBuild(std::uintptr_t b) noexcept {
    if(!b) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(b);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(b+dos->e_lfanew);
        if(nt->Signature!=IMAGE_NT_SIGNATURE || nt->FileHeader.TimeDateStamp!=kTimestamp) return false;
        if(nt->OptionalHeader.Magic!=IMAGE_NT_OPTIONAL_HDR64_MAGIC) return false;
        if(!Readable(reinterpret_cast<void*>(b+kResizeRva),sizeof(kResizeEntry)) ||
           std::memcmp(reinterpret_cast<void*>(b+kResizeRva),kResizeEntry,sizeof(kResizeEntry))!=0) return false;
        if(!Readable(reinterpret_cast<void*>(b+kProvisionHelperRva),sizeof(kProvisionEntry)) ||
           std::memcmp(reinterpret_cast<void*>(b+kProvisionHelperRva),kProvisionEntry,sizeof(kProvisionEntry))!=0) return false;
        std::uintptr_t resize_slot=0;
        if(!SafeRead(b+kPostAAVtableRva+0x10,resize_slot) || resize_slot!=b+kResizeRva) return false;
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool SlotBacked(void* p) noexcept {
    if(!p || !Readable(p,0xA8)) return false;
    void* backing=nullptr;
    __try { backing=*reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(p)+kTextureBacking); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    return backing!=nullptr;
}

// Reads the pipeline's std::string name (MSVC layout: buf/ptr +0x00, size +0x10, cap +0x18).
bool ReadPipelineSuffix(void* self,char* out,std::size_t cap) noexcept {
    if(!self || !out || cap<2) return false;
    out[0]=0;
    std::uintptr_t pipe=0;
    if(!SafeRead(reinterpret_cast<std::uintptr_t>(self)+kStagePipeline,pipe) || !pipe) return false;
    const std::uintptr_t str=pipe+kPipelineName;
    std::uint64_t size=0,capacity=0;
    if(!SafeRead(str+0x10,size) || !SafeRead(str+0x18,capacity)) return false;
    if(size>=cap || size>64 || capacity<size) return false;
    std::uintptr_t chars=str;
    if(capacity>15) { if(!SafeRead(str,chars) || !chars) return false; }
    if(size==0) return true;
    if(!Readable(reinterpret_cast<void*>(chars),static_cast<std::size_t>(size))) return false;
    __try { std::memcpy(out,reinterpret_cast<const void*>(chars),static_cast<std::size_t>(size)); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    out[size]=0;
    for(std::uint64_t i=0;i<size;++i) { const unsigned char c=static_cast<unsigned char>(out[i]); if(c<0x20 || c>0x7E) return false; }
    return true;
}

bool ProvisionOne(void* self,std::uintptr_t slot_off,const char* name,std::int32_t w,std::int32_t h,std::uint8_t fmt) noexcept {
    if(!self || !g_wh || w<=0 || h<=0 || !name) return false;
    auto* slot=reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(self)+slot_off);
    if(!Readable(slot,sizeof(void*))) return false;
    void* existing=nullptr;
    __try { existing=*slot; } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    if(existing) return SlotBacked(existing);
    const Color clear{};
    const auto provision=reinterpret_cast<ProvisionFn>(g_wh+kProvisionHelperRva);
    std::uint8_t result=0;
    __try { result=provision(name,slot,w,h,&clear,1,0,fmt,-1,kPostAAFlags); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    void* after=nullptr;
    __try { after=*slot; } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    return result!=0 && SlotBacked(after);
}

void RepairRightHistories(void* self,std::int32_t w,std::int32_t h) noexcept {
    if(!self || w<=0 || h<=0) return;
    void* left0=nullptr; void* left1=nullptr;
    __try {
        left0=*reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(self)+kLeft0);
        left1=*reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(self)+kLeft1);
    } __except(EXCEPTION_EXECUTE_HANDLER) { return; }
    if(!left0 || !left1 || !SlotBacked(left0) || !SlotBacked(left1)) return;
    std::uint8_t fmt0=0,fmt1=0;
    std::uint32_t flags0=0;
    if(!SafeRead(reinterpret_cast<std::uintptr_t>(left0)+kTextureSourceFormat,fmt0) ||
       !SafeRead(reinterpret_cast<std::uintptr_t>(left1)+kTextureSourceFormat,fmt1) ||
       !SafeRead(reinterpret_cast<std::uintptr_t>(left0)+kTextureFlags,flags0)) return;
    // Retail Resize chooses one of these two source formats. Refuse unknown layouts.
    if((fmt0!=0x11 && fmt0!=0x14) || fmt1!=fmt0 || (flags0&0x41001u)!=0x41001u) {
        g_ready.store(false,std::memory_order_release);
        static std::atomic<std::uint64_t> refused{0}; const auto n=refused.fetch_add(1)+1;
        if(n<=8) { char m[256]{}; std::snprintf(m,sizeof(m),
            "V66.33-POSTAA-RIGHT REFUSED reason=left-history-contract fmt0=0x%02X fmt1=0x%02X flags0=0x%08X size=%dx%d",
            unsigned(fmt0),unsigned(fmt1),unsigned(flags0),int(w),int(h)); diagnostic_trace::Write(m); }
        return;
    }
    char suffix[80]{};
    if(!ReadPipelineSuffix(self,suffix,sizeof(suffix))) {
        g_ready.store(false,std::memory_order_release);
        static std::atomic<std::uint64_t> refused_name{0}; const auto rn=refused_name.fetch_add(1)+1;
        if(rn<=8) diagnostic_trace::Write("V66.33-POSTAA-RIGHT REFUSED reason=pipeline-name-unreadable (would risk shared name-keyed history)");
        return;
    }
    char name0[128]{}; char name1[128]{};
    std::snprintf(name0,sizeof(name0),"$PrevBackBuffer0%s_R",suffix);
    std::snprintf(name1,sizeof(name1),"$PrevBackBuffer1%s_R",suffix);
    const bool a=ProvisionOne(self,kRight0,name0,w,h,fmt0);
    const bool b=ProvisionOne(self,kRight1,name1,w,h,fmt0);
    const bool ready=a&&b;
    g_ready.store(ready,std::memory_order_release);
    if(ready) g_provision_successes.fetch_add(1,std::memory_order_relaxed);
    const auto n=g_resize_calls.load(std::memory_order_relaxed);
    if(n<=16 || !ready) { char m[448]{}; std::snprintf(m,sizeof(m),
        "V66.33-POSTAA-RIGHT resize=%llu stage=%p pipeline='%s' size=%dx%d fmt=0x%02X slots={R0:%s R1:%s} names={%s,%s} ready=%d owner=ENGINE-STAGE-SLOTS",
        static_cast<unsigned long long>(n),self,suffix,int(w),int(h),unsigned(fmt0),a?"BACKED":"MISSING",b?"BACKED":"MISSING",
        name0,name1,ready?1:0); diagnostic_trace::Write(m); }
}

void __fastcall ResizeDetour(void* self,std::int32_t w,std::int32_t h) noexcept {
    const auto n=g_resize_calls.fetch_add(1,std::memory_order_relaxed)+1;
    // V66.33: ready is decided by RepairRightHistories (success/refusal), not reset here.
    // Static proof: the per-eye getter 0x180FFDCAC returns [stage+0x40+(eye*2+frame&1)*8]
    // and the AA pass 0x180661E74.. returns early when either history is null, so a
    // transiently empty RIGHT slot is a no-AA RIGHT frame, never a null dereference.
    auto fn=g_original;
    if(fn) fn(self,w,h);
    // Original retail code has just created +0x40/+0x48 and explicitly cleared
    // +0x50/+0x58. Reconstruct only those removed right-eye histories.
    RepairRightHistories(self,w,h);
    (void)n;
}

} // namespace

bool Install() noexcept {
    if(g_installed.load(std::memory_order_acquire)) return true;
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!ExactBuild(b)) { diagnostic_trace::Write("V66.33-POSTAA-RIGHT install REFUSED exact WHGame/Resize/helper contract mismatch"); return false; }
    g_wh=b;
    DetourModKit::hook::InlineRequest req{
        "kcd2vr_v6632_postaa_resize_right_history",
        DetourModKit::hook::Target{DetourModKit::Address{b+kResizeRva}},
        DetourModKit::hook::Options{}
    };
    auto h=DetourModKit::hook::inline_at(std::move(req),&ResizeDetour);
    if(!h) { diagnostic_trace::Write("V66.33-POSTAA-RIGHT install FAILED hook-create"); return false; }
    g_hook=std::move(*h);
    g_original=g_hook->original<ResizeFn>();
    if(!g_original || !g_hook->enable()) { g_original=nullptr; g_hook.reset(); diagnostic_trace::Write("V66.33-POSTAA-RIGHT install FAILED hook-enable"); return false; }
    g_installed.store(true,std::memory_order_release);
    diagnostic_trace::Write("V66.33-POSTAA-RIGHT installed exact CPostAAStage::Resize hook; reconstructs removed +0x50/+0x58 right-eye histories only");
    return true;
}

void Uninstall() noexcept {
    g_installed.store(false,std::memory_order_release);
    g_ready.store(false,std::memory_order_release);
    if(g_hook) { (void)g_hook->disable(); g_hook.reset(); }
    g_original=nullptr; g_wh=0;
}

bool Installed() noexcept { return g_installed.load(std::memory_order_acquire); }

} // namespace kcd2vr::native_postaa_repair_v6632
