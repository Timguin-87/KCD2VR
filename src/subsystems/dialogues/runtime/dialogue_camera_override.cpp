#include "dialogue_camera_override.h"
#include "diagnostic_trace.h"
#include "retail_build112.h"
#include "dialogue_original43.h"
#include "dialogue_presentation19c2.h"
#include <intrin.h>
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <utility>

namespace kcd2vr::dialogue_camera_override {
namespace {
constexpr std::uint32_t kSupportedWhTimestamp=0x6A350E20u;
// Exact retail parser identified from the string decision tree. Note: PE RVA, not raw file offset:
// NoOverride -> 0, Medium -> 1, CloseShot -> 2, Closeup -> 3, Master -> 4.
constexpr std::uintptr_t kOverrideParserRva=0x27FD5E4ull;
using ParserFn=unsigned char(__fastcall*)(void*,void*);
std::optional<DetourModKit::hook::Hook> g_hook;
ParserFn g_original=nullptr;
std::atomic<bool> g_active{false};
std::atomic<int> g_type{0};
std::atomic<bool> g_force_no_override{false};
std::atomic<std::uint64_t> g_calls{0},g_overrides{0},g_last_log{0};
std::atomic<int> g_last_native{0};

using TwinFn43=bool(__fastcall*)(void*,void*,void**);
std::optional<DetourModKit::hook::Hook> g_twin_hook43;
TwinFn43 g_twin_original43=nullptr;
std::atomic<bool> g_twin_fault43{false},g_twin_enabled43{false};
static bool TwinPresentation43(std::uintptr_t caller,void* actor,void** output,bool created)noexcept {
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!g_force_no_override.load(std::memory_order_acquire)||g_twin_fault43.load()||!retail_build112::Ready(b))return false;
    __try {
        dialogue_original43::Pair pair{};
        auto read=[](std::uintptr_t p,std::uintptr_t& value)noexcept {__try{value=*reinterpret_cast<const std::uintptr_t*>(p);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}};
        if(!output||!dialogue_original43::pair(b,caller,true,created,reinterpret_cast<std::uintptr_t>(actor),
            reinterpret_cast<std::uintptr_t>(*output),read,pair))return false;
        // Native Hide(bool, propagateChildren) emits native HIDE/UNHIDE events.
        // Third arg is the child propagation branch at9DFCF0, not a physics slot.
        using Hide=void(__fastcall*)(void*,bool,bool);
        const auto hide=reinterpret_cast<Hide>(b+0x9DFC5C);
        hide(reinterpret_cast<void*>(pair.twin),true,true);
        // A native event can re-enter scene cleanup. Revalidate both receivers
        // after that call; never continue with an actor retired by its handler.
        dialogue_original43::Pair again{};
        if(!dialogue_original43::pair(b,caller,true,created,reinterpret_cast<std::uintptr_t>(actor),
            reinterpret_cast<std::uintptr_t>(*output),read,again)||again.original!=pair.original||again.twin!=pair.twin)return false;
        hide(reinterpret_cast<void*>(pair.original),false,true);
        diagnostic_trace::Write("C43 DIALOGUE_ORIGINAL Henry=ORIGINAL twin=HIDDEN npcController=NATIVE cleanup=NATIVE");
        return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){g_twin_fault43.store(true);diagnostic_trace::Write("C43 DIALOGUE_ORIGINAL fault=QUARANTINED noRetry=1");return false;}
}
static bool __fastcall TwinDetour43(void* controller,void* actor,void** output){
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const bool result=g_twin_original43(controller,actor,output); // all participant setup runs once
    (void)TwinPresentation43(caller,actor,output,result);return result;
}
static bool TwinPins43(std::uintptr_t b)noexcept {
    __try {
        constexpr unsigned char create[]{0x4C,0x89,0x44,0x24,0x18,0x48,0x89,0x4C,0x24,0x08,0x55,0x53,0x56,0x57};
        constexpr unsigned char hide[]{0x48,0x89,0x5C,0x24,0x18,0x55,0x56,0x57,0x48,0x83,0xEC,0x70};
        if(std::memcmp(reinterpret_cast<void*>(b+0xC8A4E0),create,sizeof(create))||
           std::memcmp(reinterpret_cast<void*>(b+0x9DFC5C),hide,sizeof(hide)))return false;
        constexpr unsigned char call0[]{0xE8,0xC5,0x08,0x00,0x00},call1[]{0xE8,0x99,0x08,0x00,0x00};
        return !std::memcmp(reinterpret_cast<void*>(b+0xC89C16),call0,sizeof(call0))&&
               !std::memcmp(reinterpret_cast<void*>(b+0xC89C42),call1,sizeof(call1));
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool InstallTwin43(std::uintptr_t b)noexcept {
    if(g_twin_hook43)return g_twin_enabled43.load(std::memory_order_acquire);
    if(!retail_build112::Ready(b)||!TwinPins43(b))return false;
    using namespace DetourModKit::hook;
    Options options{};options.fail_if_already_hooked=true;
    auto h=inline_at(InlineRequest{"c43_dialogue_Henry_original_presentation",Target{DetourModKit::Address{b+0xC8A4E0}},options},&TwinDetour43);
    if(!h)return false;g_twin_hook43=std::move(*h);g_twin_original43=g_twin_hook43->original<TwinFn43>();
    if(!g_twin_original43||!g_twin_hook43->enable()){
        // Keep the handle/trampoline even after disarm; a callback may be in flight.
        if(g_twin_hook43)(void)g_twin_hook43->disable();return false;
    }
    g_twin_enabled43.store(true,std::memory_order_release);
    diagnostic_trace::Write("C43 DIALOGUE_ORIGINAL installed creator=C8A4E0 playerOnly=1 nativeParticipants=RETAINED");return true;
}

bool Supported(std::uintptr_t base) noexcept {
    if(!base) return false;
    __try {
        auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        return nt->Signature==IMAGE_NT_SIGNATURE && nt->FileHeader.TimeDateStamp==kSupportedWhTimestamp;
    } __except(EXCEPTION_EXECUTE_HANDLER){ return false; }
}
bool EntryMatches(std::uintptr_t p) noexcept {
    static constexpr unsigned char sig[]={0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x4C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0xDA};
    __try { return std::memcmp(reinterpret_cast<const void*>(p),sig,sizeof(sig))==0; }
    __except(EXCEPTION_EXECUTE_HANDLER){ return false; }
}
unsigned char __fastcall Detour(void* a,void* b) noexcept {
    const unsigned char native=g_original?g_original(a,b):0;
    const auto call=g_calls.fetch_add(1,std::memory_order_relaxed)+1;
    g_last_native.store(static_cast<int>(native),std::memory_order_release);
    const int requested=g_type.load(std::memory_order_relaxed);
    const bool active=g_active.load(std::memory_order_relaxed);
    const bool forceNo=g_force_no_override.load(std::memory_order_relaxed);
    const unsigned char out=(active && forceNo)?0u:
        ((active && requested>=1 && requested<=4)?static_cast<unsigned char>(requested):native);
    if(out!=native) g_overrides.fetch_add(1,std::memory_order_relaxed);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto last=g_last_log.load(std::memory_order_relaxed);
    if((call<=6 || (active && now-last>=1000ull)) && g_last_log.compare_exchange_strong(last,now,std::memory_order_relaxed)){
        char m[256]{};
        std::snprintf(m,sizeof(m),"V64.3-DIALOG-CAM parser call=%llu active=%d requested=%d native=%u returned=%u overrides=%llu",
            (unsigned long long)call,active?1:0,forceNo?-1:requested,(unsigned)native,(unsigned)out,
            (unsigned long long)g_overrides.load(std::memory_order_relaxed));
        diagnostic_trace::Write(m);
    }
    return out;
}
}

bool Install() noexcept {
    const auto twinBase43=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!InstallTwin43(twinBase43))diagnostic_trace::Write("C43 DIALOGUE_ORIGINAL unavailable; native participant presentation retained");
    if(g_hook) return true;
    const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!Supported(base) || !EntryMatches(base+kOverrideParserRva)){
        diagnostic_trace::Write("V64.3-DIALOG-CAM install refused: retail parser RVA/signature mismatch");
        return false;
    }
    DetourModKit::hook::InlineRequest req{"kcd2vr_v643_dialogue_camera_type_override",
        DetourModKit::hook::Target{DetourModKit::Address{base+kOverrideParserRva}},DetourModKit::hook::Options{}};
    auto r=DetourModKit::hook::inline_at(std::move(req),&Detour);
    if(!r){ diagnostic_trace::Write("V64.3-DIALOG-CAM parser hook creation failed"); return false; }
    g_hook=std::move(*r); g_original=g_hook->original<ParserFn>();
    if(!g_hook->enable()){ g_original=nullptr; g_hook.reset(); diagnostic_trace::Write("V64.3-DIALOG-CAM parser hook enable failed"); return false; }
    diagnostic_trace::Write("V64.3-DIALOG-CAM parser hook ready mapping={0 NoOverride,1 Medium,2 CloseShot,3 Closeup,4 Master}; override is dialogue-active only");
    return true;
}
void Uninstall() noexcept { g_force_no_override.store(false);g_twin_enabled43.store(false);
    // New native factory trampoline stays retained for any in-flight forwarding
    // callback. A failed disable also remains a native-only forwarding hook.
    if(g_twin_hook43)(void)g_twin_hook43->disable();g_active.store(false);g_force_no_override.store(false);g_type.store(0);if(g_hook){(void)g_hook->disable();g_hook.reset();}g_original=nullptr; }
void SetDialogueActive(bool a) noexcept { g_active.store(a,std::memory_order_release); }
void SetOverrideType(int t) noexcept { g_type.store((t>=1&&t<=4)?t:0,std::memory_order_release); }
void SetForceNoOverride(bool enabled) noexcept { g_force_no_override.store(enabled,std::memory_order_release); }
int LastNativeType() noexcept { return g_last_native.load(std::memory_order_acquire); }
}
