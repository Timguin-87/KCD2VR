#include "door_signal19c4r4.h"
#include "core/door_channel19c4r4.h"
#include "retail_build112.h"
#include "vrik/lab_prop58.h"
#include "diagnostic_trace.h"
#include "diagnostics/pickup_prepare_d85r5r1.h"
#include "vrik/brush_swap_r100.h"
#include "diagnostics/interaction_receipt59c10.h"
#include <cstdio>
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <optional>
#include <cstring>

namespace kcd2vr::door_signal19c4r4 {namespace {
using GetCVar= int(__fastcall*)(void*,void*);
using ReadString= bool(__fastcall*)(void*,int,const char**);
using EndInteger= int(__fastcall*)(void*,const std::uint32_t*);
static std::uintptr_t g_base19c4r4=0;
static GetCVar g_get_cvar_original19c4r4=nullptr;
static door_channel19c4r4::Channel g_channel19c4r4;
static std::optional<DetourModKit::hook::Hook> g_hook19c4r4;
static std::atomic<bool> g_installed19c4r4{false};
static std::atomic<bool> g_retained19c4r4{false};
static std::atomic_flag g_installing19c4r4=ATOMIC_FLAG_INIT;
static std::atomic<door_curve_signal66::Handler> g_curve_handler66{nullptr};
static std::atomic<SmallPickupReaderR24> g_small_pickup_readerR24{nullptr};
#include "core/door_curve_message66.inl"
static bool RemoveHook19c4r4() noexcept {
    // frik Install begins with Uninstall. Before any hook/original existed,
    // that cold cleanup must not permanently forbid the FIRST door install.
    // Once a hook has existed, retain its trampoline for process lifetime.
    if(!g_hook19c4r4&&!g_get_cvar_original19c4r4){g_installed19c4r4.store(false);return true;}
    g_retained19c4r4.store(true);
    if(g_hook19c4r4){
        if(g_hook19c4r4->is_enabled()&&!g_hook19c4r4->disable())return false;
        if(g_hook19c4r4->is_enabled())return false;
    }
    g_installed19c4r4.store(false);return true;
}
static bool DoorPin19c4r4(std::uintptr_t rva,const unsigned char* bytes,unsigned size) noexcept {
    unsigned char actual[32]{};if(size>sizeof(actual))return false;
    __try{std::memcpy(actual,reinterpret_cast<void*>(g_base19c4r4+rva),size);return !std::memcmp(actual,bytes,size);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool DoorNativeName19c4r4(void* handler,char* out,std::size_t cap) noexcept {
    if(!handler||!out||cap<2)return false;out[0]=0;
    __try{
        // Exact native GetCVar41C388 checks handler+vft28 before parsing param1.
        auto** vft=*reinterpret_cast<void***>(handler);
        using Count=int(__fastcall*)(void*);
        if(!vft||!vft[0x28/8]||reinterpret_cast<Count>(vft[0x28/8])(handler)!=1)return false;
        const char* name=nullptr;
        if(!reinterpret_cast<ReadString>(g_base19c4r4+0x41DC20)(handler,1,&name)||!name)return false;
        for(std::size_t i=0;i<cap-1;++i){out[i]=name[i];if(!out[i])return true;}
        out[cap-1]=0;return false;
    }__except(EXCEPTION_EXECUTE_HANDLER){out[0]=0;return false;}
}
static int DoorNativeEndInteger19c4r4(void* handler,std::uint32_t value) noexcept {
    // This exact helper converts uint32 to Lua's native float-number value.
    // All protocol values are capped to the exact integer range of float32.
    __try{return reinterpret_cast<EndInteger>(g_base19c4r4+0x2AB4C2C)(handler,&value);}
    __except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
static std::uint64_t DoorClock19c4r4() noexcept {return GetTickCount64();}
static void DoorPickupNoteD85R5R1(unsigned reason) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    namespace prep=kcd2vr::diagnostics::pickup_prepare_d85r5r1;
    const auto now=DoorClock19c4r4();
    if(!cap::active(cap::Domain::Interaction,now)||reason>=sizeof(prep::reasons)/sizeof(prep::reasons[0]))return;
    static thread_local kcd2vr::diagnostics::interaction_receipt59c10::Gate gate;
    const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire);
    if(!gate.observe(session,now,{0,0,-1,int(reason)},100,128,1000))return;
    char line[240]{};
    std::snprintf(line,sizeof(line),"D85R5R1 F5_PICKUP_PREPARE_SCRIPT reason=%s nativeUse=ORIGINAL_ONCE physicsHold=UNOBSERVED",
        prep::reasons[reason]);
    diagnostic_trace::Write(line);
}
#define KCD2VR_PICKUP_PREPARE_NOTE_D85R5R1 1
#define KCD2VR_SMALL_PICKUP_R24 1
#define KCD2VR_DOOR_CURVE_SIGNAL66 1
#define KCD2VR_SWAP_NOTE_R100 1
#include "core/door_signal_hook19c4r4.inl"
#undef KCD2VR_SWAP_NOTE_R100
#undef KCD2VR_DOOR_CURVE_SIGNAL66
#undef KCD2VR_SMALL_PICKUP_R24
#undef KCD2VR_PICKUP_PREPARE_NOTE_D85R5R1
static bool Install19c4r4() noexcept {
    if(g_retained19c4r4.load())return false;
    if(g_installed19c4r4.load())return true;
    if(g_installing19c4r4.test_and_set())return false;
    struct Done{~Done(){g_installing19c4r4.clear();}} done;
    if(g_retained19c4r4)return false;
    const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!base||!retail_build112::Verify(base))return false;
    g_base19c4r4=base;
    constexpr unsigned char entry[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x30,0x48,0x8B,0x02,0x48,0x8B,0xCA};
    constexpr unsigned char parser[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x40,0x48,0x8B,0x01,0x49,0x8B,0xF8};
    constexpr unsigned char end[]{0x40,0x53,0x48,0x83,0xEC,0x40,0x8B,0x02,0x0F,0x57,0xC0};
    constexpr unsigned char registration[]{0x4C,0x8D,0x05,0xAF,0x03,0xFD,0xFE};
    if(!DoorPin19c4r4(0x41C388,entry,sizeof(entry))||!DoorPin19c4r4(0x41DC20,parser,sizeof(parser))||
       !DoorPin19c4r4(0x2AB4C2C,end,sizeof(end))||!DoorPin19c4r4(0x144BFD2,registration,sizeof(registration))){
        diagnostic_trace::Write("C4R4 DOOR_SIGNAL rejected exact ScriptBind_System/GetCVar/parser/end pins");return false;
    }
    DetourModKit::hook::InlineRequest request{"kcd2vr_c4r4_private_door_getter",DetourModKit::hook::Target{DetourModKit::Address{base+0x41C388}},DetourModKit::hook::Options{}};
    auto hook=DetourModKit::hook::inline_at(std::move(request),&DoorGetCVarDetour19c4r4);
    if(!hook)return false;
    g_get_cvar_original19c4r4=hook->template original<GetCVar>();
    g_hook19c4r4.emplace(std::move(*hook));
    if(!g_get_cvar_original19c4r4||!g_hook19c4r4->enable()||!g_hook19c4r4->is_enabled()){
        if(!RemoveHook19c4r4()){
            g_retained19c4r4=true;
            diagnostic_trace::Write("C4R4 DOOR_SIGNAL retained failed hook; channel disabled; reinstall refused");
        }
        return false;
    }
    g_installed19c4r4.store(true);
    diagnostic_trace::Write("C4R4 DOOR_SIGNAL ready exact private getter; Lua-owned scheduler; no CVar registration or cross-thread Lua entry");
    return true;
}
}
bool SetEnabled(bool on) noexcept {
    if(on&&!Install19c4r4())return false;
    if(g_channel19c4r4.enabled()!=on)g_channel19c4r4.enabled(on);
    return !on||g_installed19c4r4.load();
}
bool Ready() noexcept {
    static std::atomic<bool> reported46{false};
    if(!reported46.load(std::memory_order_relaxed)&&g_channel19c4r4.companion46()&&!reported46.exchange(true))
        diagnostic_trace::Write("C66 DOOR_COMPANION observed revision=66 protocol=1966 source=ACTUAL_LUA_PRIVATE_GETTER");
    return g_installed19c4r4.load()&&g_channel19c4r4.enabled();
}
bool Publish(const door_script19c4r4::Command& command,std::uintptr_t entity) noexcept {return g_channel19c4r4.publish(command,GetTickCount64(),entity);}
bool ReadCleanupIdentity(door_script19c4r4::CleanupIdentity& out) noexcept {return g_channel19c4r4.cleanup(out);}
bool RetireKnownAbsent(const door_script19c4r4::CleanupIdentity& expected,std::uintptr_t currentEntity) noexcept {
    if(!g_channel19c4r4.retireKnownAbsent(expected,currentEntity))return false;
    diagnostic_trace::Write("C4R4 DOOR_SIGNAL cleanup retired: native registry old identity absent/replaced; restored=UNOBSERVED; old command epoch invalidated");
    return true;
}
bool ReadAck(door_script19c4r4::Ack& out) noexcept {return g_channel19c4r4.ack(out,GetTickCount64());}
bool HasCleanupLease() noexcept {return g_channel19c4r4.pending();}
void SetCurveHandler66(door_curve_signal66::Handler handler) noexcept {g_curve_handler66.store(handler,std::memory_order_release);}
bool SetSmallPickupReaderR24(SmallPickupReaderR24 reader) noexcept {
    if(reader&&!Install19c4r4())return false;
    g_small_pickup_readerR24.store(reader,std::memory_order_release);return true;
}
// R100: the swap note needs only the installed getter hook (no reader, no channel).
bool ArmSwapNoteR100() noexcept {return Install19c4r4();}
void Stop() noexcept {(void)SetEnabled(false);}
bool Shutdown() noexcept {
    g_small_pickup_readerR24.store(nullptr,std::memory_order_release);
    Stop();if(g_channel19c4r4.pending()){
        diagnostic_trace::Write("C4R4 DOOR_SIGNAL shutdown deferred: exact Lua cleanup acknowledgment pending");return false;}
    if(!RemoveHook19c4r4()){
        g_retained19c4r4=true;
        diagnostic_trace::Write("C4R4 DOOR_SIGNAL shutdown retained hook: disable unconfirmed");return false;
    }
    return true;
}
}
