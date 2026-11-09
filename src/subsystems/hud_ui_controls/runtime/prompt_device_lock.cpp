#include "prompt_device_lock.h"
#include "diagnostic_trace.h"
#include "debug_trace.h"
#include "ui_cursor_lease.h"
#include "prompt_family_d49.h"
#include "prompt_refresh_d56.h"
#include "glyph_apply_r129.h"
#include "../prompt_policy_d80.h"
#include "ui_hud_r160.h" // R160 UI
#include "flashui_probe.h"
#include "native_script_t2.h"
#include "interaction_alignment.h"
#include "retail_build112.h"
#include "diagnostics/capture168.h"
#include <mutex>

#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <intrin.h>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <utility>

namespace kcd2vr::prompt_device_lock {
namespace {
constexpr std::uint32_t kSupportedWhTimestamp = 0x6A350E20u;
constexpr std::uintptr_t kInputDeviceManagerSetRva = 0x00C3C034ull;
std::atomic<int> g_forced_device{2};
std::atomic<bool> g_suspended{false};
std::atomic<std::uint64_t> g_flash_request_d56{0},g_flash_done_d56{0};
std::atomic<bool> g_flash_ready_d56{false};
prompt_refresh_d56::State g_flash_state_d56;
std::atomic<int> g_applied_style_r129{0},g_glyph_path_r129{0}; // R129: confirmed art style, path of the next dispatch
// R129: one always-on KCD2VR.log row (WriteStatus, no screen text) per change of (style, applied).
void ReportGlyphR129(int style,bool dispatched,const char* reply,unsigned attempts) noexcept {
    namespace ga=glyph_apply_r129;static ga::Report report;
    const auto path=ga::Path(g_glyph_path_r129.exchange(0,std::memory_order_acq_rel));
    if(!report.take(style,dispatched))return;
    char line[300]{};
    if(dispatched)std::snprintf(line,sizeof(line),"R129 GLYPHS style=%s path=%s applied=1 reply=%.48s (button icons sent to the game's movies)",
        ga::label(style),ga::name(path),reply?reply:"");
    else std::snprintf(line,sizeof(line),"R129 GLYPHS style=%s path=%s applied=0 attempts=%u reply=%.48s reason=%s retry=NEXT_UI_CHANGE_OR_MENU_CLICK",
        ga::label(style),ga::name(path),attempts,reply?reply:"",
        !reply||!*reply?"GAME_SCRIPT_NOT_READY":"NO_MOVIE_TOOK_IT (KCD2VR.pak missing or old)");
    diagnostic_trace::WriteStatus(line);
}
std::atomic<std::uint64_t> g_station_request_d80{0},g_station_done_d80{0};
prompt_refresh_d56::State g_station_dispatch_d80;
bool StationPromptWantedD80() noexcept {
    return g_flash_ready_d56.load(std::memory_order_acquire)&&
        g_station_request_d80.load(std::memory_order_acquire)!=g_station_done_d80.load(std::memory_order_acquire);
}

using SetInputDeviceFn = void(__fastcall*)(void*, int);
std::optional<DetourModKit::hook::Hook> g_hook;
SetInputDeviceFn g_original=nullptr;
using GetCVarFnD53=void*(__fastcall*)(void*,const char*);
std::optional<DetourModKit::hook::Hook> g_hint_hook_d53;
GetCVarFnD53 g_hint_original_d53=nullptr;
std::atomic<bool> g_hint_ready_d53{false};
std::atomic<bool> g_installed{false};
std::atomic<std::uint64_t> g_forced_calls{0};
// Independently implemented from the supplied With Cursor variant's behavior:
// classifier, native registration and OS SetCursor are separate responsibilities.
using RegistrationFn=void(__fastcall*)(void*,bool);
using ListenerFn=void(__fastcall*)(void*,int,int);
using CursorFn=HCURSOR(WINAPI*)(HCURSOR);
std::optional<DetourModKit::hook::Hook> g_registration_hook,g_cursor_hook;
RegistrationFn g_registration=nullptr;ListenerFn g_listener=nullptr;CursorFn g_cursor=nullptr;
std::recursive_mutex g_cursor_mutex;
ui_cursor_lease::State g_cursor_state;
POINT g_cursor_position{};bool g_cursor_position_valid=false;
std::atomic<bool> g_cursor_ready{false},g_external{false};
std::atomic<std::uint64_t> g_navigation{0},g_cursor_changes{0},g_cursor_errors{0};
std::uint64_t g_navigation_seen=0;
thread_local bool g_in_cursor_callback=false;

// F2/F5 own optional prompt/cursor diagnostics. The input correction and
// cursor lifecycle still run with captures off; even the clock read stays gated.
bool PromptCaptureActiveD50() noexcept {
    namespace cap=diagnostics::capture168;
    constexpr unsigned mask=(1u<<unsigned(cap::Domain::Bug))|(1u<<unsigned(cap::Domain::Interaction));
    if(!(cap::gate.mask.load(std::memory_order_acquire)&mask))return false;
    const auto now=GetTickCount64();
    return cap::active(cap::Domain::Bug,now)||cap::active(cap::Domain::Interaction,now);
}

bool FlashGlyphWantedD56() noexcept {
    return g_flash_ready_d56.load(std::memory_order_acquire)&&
        !g_suspended.load(std::memory_order_acquire)&&
        g_flash_request_d56.load(std::memory_order_acquire)!=g_flash_done_d56.load(std::memory_order_acquire);
}
// Uses the already registered original-first script client: no ninth scheduler
// slot, foreign-thread Lua call, invented name, or global HUD visibility write.
static std::atomic<bool> g_name_cleanup_d72{false};
static interaction_alignment::AlchemyHandNameFocusD72 g_name_focus_d72{};
static std::uint64_t g_name_tick_d72=0,g_name_generation_d72=0,g_name_serial_d72=0;
static bool g_name_requested_d72=false,g_name_attempted_d72=false;
bool FlashWantedD56() noexcept {
    return FlashGlyphWantedD56()||StationPromptWantedD80()||interaction_alignment::AlchemyHandNameWantedD72()||
        g_name_cleanup_d72.load(std::memory_order_acquire);
}
void AlchemyNameServiceD72(const native_script_t2::Context& context) noexcept {
    using namespace interaction_alignment;
    AlchemyHandNameFocusD72 focus{};
    const bool active=!g_suspended.load(std::memory_order_acquire)&&ReadAlchemyHandNameFocusD72(context.now,focus);
    const bool generation=context.generation!=g_name_generation_d72;
    const bool changed=!g_name_attempted_d72||generation||active!=g_name_requested_d72||(active&&focus!=g_name_focus_d72);
    if(!active&&!g_name_cleanup_d72.load(std::memory_order_acquire))return;
    if(!changed&&context.now>=g_name_tick_d72&&context.now-g_name_tick_d72<500)return;
    if(!context.Current())return;
    if(active){AlchemyHandNameFocusD72 again{};
        if(!ReadAlchemyHandNameFocusD72(context.now,again)||again!=focus)return;}
    g_name_tick_d72=context.now;g_name_generation_d72=context.generation;
    g_name_attempted_d72=true;g_name_requested_d72=active;g_name_focus_d72=focus;
    char input[128]{},output[96]{},expected[96]{};
    const auto serial=++g_name_serial_d72;
    std::snprintf(input,sizeof(input),"N72|%llu|%u|%u|%d|%d",(unsigned long long)serial,
        active?focus.player:0,active?focus.entity:0,active?focus.type:0,active?1:0);
    // Even an uncertain UI dispatch may have installed a name. Retain clear
    // debt until a later script receipt confirms restoration of native focus.
    if(active)g_name_cleanup_d72.store(true,std::memory_order_release);
    const bool called=context.CallPacket("kcd2vr_alchemy_name_d72",input,output,sizeof(output));
    if(called){
        std::snprintf(expected,sizeof(expected),"N72|%llu|CLEARED",(unsigned long long)serial);
        if(std::strcmp(output,expected)==0)g_name_cleanup_d72.store(false,std::memory_order_release);
    }
}
void StationPromptServiceD80(const native_script_t2::Context& context) noexcept {
    const auto ticket=g_station_request_d80.load(std::memory_order_acquire);
    if(!StationPromptWantedD80()||!g_station_dispatch_d80.begin(ticket,context.generation,context.now))return;
    const auto serial=ticket>>16;const unsigned percent=unsigned((ticket>>8)&255);
    const bool active=(ticket&1)!=0,visible=(ticket&2)!=0;
    char input[96]{},output[96]{},expected[96]{};
    std::snprintf(input,sizeof(input),"H80|%llu|%d|%u|%d",(unsigned long long)serial,active,percent,visible);
    const auto leaseEpoch=prompt_policy_d80::movieLease.Epoch();
    const bool called=prompt_policy_d80::movieLease.CanDispatch(active)&&
        ticket==g_station_request_d80.load(std::memory_order_acquire)&&context.Current()&&
        context.CallPacket("kcd2vr_prompt_style_d80",input,output,sizeof(output));
    std::snprintf(expected,sizeof(expected),"H80|%llu|DISPATCHED",(unsigned long long)serial);
    const bool dispatched=called&&std::strcmp(output,expected)==0;
    prompt_policy_d80::movieLease.Completed(leaseEpoch,active,dispatched,g_station_dispatch_d80.attempts>=4);
    // A successful script dispatch does not prove a freshly constructed child
    // existed. Four bounded event retries cover delayed creation, without an
    // always-on Flash poll. HUD recreation publishes a new request separately.
    if(g_station_dispatch_d80.attempts>=4)g_station_done_d80.store(ticket,std::memory_order_release);
    // R145: the request is re-sent every second at a station; log a row only when what we sent or its result changed.
    static unsigned loggedR145=~0u;const unsigned signatureR145=(percent<<4)|(active?1u:0u)|(visible?2u:0u)|(dispatched?4u:0u);
    if(PromptCaptureActiveD50()&&(g_station_dispatch_d80.attempts==1||g_station_dispatch_d80.attempts==4)&&
       signatureR145!=loggedR145){loggedR145=signatureR145;
        char line[256]{};std::snprintf(line,sizeof(line),
            "D80 STATION_PROMPT active=%d percent=%u visible=%d attempt=%u dispatched=%d delivery=MOVIE_CHILD_EVENT pixels=UNOBSERVED",
            active,percent,visible,g_station_dispatch_d80.attempts,dispatched);diagnostic_trace::Write(line);
    }
}
void FlashServiceD56(const native_script_t2::Context& context) noexcept {
    AlchemyNameServiceD72(context);
    StationPromptServiceD80(context);
    const auto ticket=g_flash_request_d56.load(std::memory_order_acquire);
    if(!FlashGlyphWantedD56()||!g_flash_state_d56.begin(ticket,context.generation,context.now))return;
    const auto serial=ticket>>3;const int style=int(ticket&7);
    char input[64]{},output[96]{},expected[80]{};
    std::snprintf(input,sizeof(input),"P56|%llu|%d",(unsigned long long)serial,style);
    const bool called=context.CallPacket("kcd2vr_prompt_d56",input,output,sizeof(output));
    std::snprintf(expected,sizeof(expected),"P56|%llu|%d|",(unsigned long long)serial,style);
    const auto n=std::strlen(expected);
    const bool dispatched=called&&std::strncmp(output,expected,n)==0&&
        output[n]>='1'&&output[n]<='8'&&output[n+1]==0;
    const bool finished=g_flash_state_d56.terminal(dispatched);
    if(finished)g_flash_done_d56.store(ticket,std::memory_order_release);
    if(finished){ // R129: remember what the movies took; a failed one is re-sent on the next UI change / menu click
        g_applied_style_r129.store(dispatched?glyph_apply_r129::style(style):0,std::memory_order_release);
        ReportGlyphR129(style,dispatched,called?output:"",g_flash_state_d56.attempts);
    }
    if((finished||g_flash_state_d56.attempts==1)&&PromptCaptureActiveD50()){
        char line[240]{};std::snprintf(line,sizeof(line),
            "D56 PROMPT_FLASH serial=%llu family=%d attempt=%u dispatched=%d reply=%.90s nativeVm=1 pixelReadback=NOT_OBSERVED",
            (unsigned long long)serial,style,g_flash_state_d56.attempts,dispatched,called?output:"CALLBACK_UNAVAILABLE");
        diagnostic_trace::Write(line);
    }
}

// Leaf SEH helpers avoid the MSVC C2712 RAII/SEH combination.
bool SameBytes(std::uintptr_t p,const unsigned char* expected,std::size_t n) noexcept {
    __try{return std::memcmp(reinterpret_cast<const void*>(p),expected,n)==0;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool CurrentDevice(void* self,int& device) noexcept {
    __try{if(!self)return false;device=*reinterpret_cast<const int*>(static_cast<const unsigned char*>(self)+0x20);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool NativeCursorDelta(ui_cursor_lease::Delta d) noexcept {
    if(d==ui_cursor_lease::Delta::None)return true;
    __try{if(!g_listener)return false;
        g_listener(nullptr,d==ui_cursor_lease::Delta::Show?2:1,d==ui_cursor_lease::Delta::Show?1:2);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void ApplyCursorDelta(ui_cursor_lease::Delta d) noexcept {
    if(d==ui_cursor_lease::Delta::None)return;
    if(NativeCursorDelta(d)){if(PromptCaptureActiveD50())g_cursor_changes.fetch_add(1,std::memory_order_relaxed);}
    else {if(PromptCaptureActiveD50())g_cursor_errors.fetch_add(1,std::memory_order_relaxed);g_cursor_ready.store(false,std::memory_order_release);}
}
void BaselineCursor() noexcept {POINT p{};g_cursor_position_valid=GetCursorPos(&p)!=0;if(g_cursor_position_valid)g_cursor_position=p;}
void NativeNavigation() noexcept {
    if(!g_cursor_ready.load(std::memory_order_acquire)||g_in_cursor_callback)return;
    std::lock_guard<std::recursive_mutex> lock(g_cursor_mutex);
    g_in_cursor_callback=true;BaselineCursor();ApplyCursorDelta(g_cursor_state.Hide());g_in_cursor_callback=false;
}
void __fastcall RegisterCursor(void* self,bool on) {
    // Always preserve the game's registration operation exactly once.
    if(g_registration)g_registration(self,on);
    if(g_in_cursor_callback||!g_cursor_ready.load(std::memory_order_acquire))return;
    std::lock_guard<std::recursive_mutex> lock(g_cursor_mutex);
    g_in_cursor_callback=true;const auto before=g_cursor_state.Count();
    ApplyCursorDelta(g_cursor_state.Register(reinterpret_cast<std::uintptr_t>(self),on));
    if(!before&&g_cursor_state.Count())BaselineCursor();
    if(!g_cursor_state.Count())g_cursor_position_valid=false;
    if(g_cursor_state.Overflow()&&PromptCaptureActiveD50())g_cursor_errors.fetch_add(1,std::memory_order_relaxed);
    g_in_cursor_callback=false;
}
HCURSOR WINAPI SetCursorDetour(HCURSOR cursor) {
    if(cursor&&g_cursor_ready.load(std::memory_order_acquire)&&!g_in_cursor_callback){
        std::lock_guard<std::recursive_mutex> lock(g_cursor_mutex);
        g_in_cursor_callback=true;
        if(g_cursor_state.Count()){
            POINT p{};if(GetCursorPos(&p)){
                const bool moved=g_cursor_position_valid&&(p.x!=g_cursor_position.x||p.y!=g_cursor_position.y);
                g_cursor_position=p;g_cursor_position_valid=true;
                if(moved)ApplyCursorDelta(g_cursor_state.MouseMoved());
            }
        }
        g_in_cursor_callback=false;
    }
    return g_cursor?g_cursor(cursor):nullptr;
}

bool ReadTimestamp(std::uintptr_t base,std::uint32_t& out) noexcept {
    out=0; if(!base) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        if(nt->Signature!=IMAGE_NT_SIGNATURE) return false;
        out=nt->FileHeader.TimeDateStamp; return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool MatchEntry(std::uintptr_t p) noexcept {
    static const unsigned char expected[]={
        0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x55,
        0x48,0x8D,0x68,0xA1,0x48,0x81,0xEC,0xB0,0x00,0x00,0x00,0x8B,0x71,0x20,0x8B,0xDA,
        0x48,0x8B,0xF9,0x3B,0xD6,0x0F,0x84
    };
    __try { return std::memcmp(reinterpret_cast<const void*>(p),expected,sizeof(expected))==0; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

// Claude M57, independently requalified against retail SHA bdf8f9e4...479f7.
// ActionHint +8E6B05 reads wh_ui_InputDevice through CXConsole::GetCVar and
// ICVar::GetIVal. Its callback +2BBA0E0 then calls the manager setter below.
// Forcing only that manager left the CVar (and therefore glyphs) on keyboard.
constexpr std::uintptr_t kConsolePtrD49=0x492D8A8,kConsoleVtD49=0x3DCF850,kGetCVarD49=0x9D8D38;
constexpr std::uintptr_t kIntVtD49=0x3A7F0B8,kRefVtD49=0x3DD0020;
std::uintptr_t g_prompt_base_d49=0;
thread_local bool g_in_cvar_d49=false;
std::atomic<std::uint64_t> g_cvar_changed_d49{0},g_cvar_rejected_d49{0},g_cvar_unavailable_d49{0};
bool CVarPinsD49(std::uintptr_t b) noexcept {
    static const unsigned char get[]={0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x57,0x48,0x83,0xEC,0x40};
    static const unsigned char setInt[]={0x48,0x89,0x5C,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57,0x48,0x81,0xEC,0x50,0x02,0x00,0x00};
    static const unsigned char setRef[]={0x48,0x89,0x5C,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57,0x48,0x81,0xEC,0xB0,0x00,0x00,0x00};
    static const unsigned char getInt[]={0x8B,0x41,0x48,0xC3},getRef[]={0x48,0x8B,0x41,0x48,0x8B,0x00,0xC3};
    return retail_build112::Ready(b)&&SameBytes(b+kGetCVarD49,get,sizeof(get))&&
        SameBytes(b+0x4D857C,setInt,sizeof(setInt))&&SameBytes(b+0xC05DB8,setRef,sizeof(setRef))&&
        SameBytes(b+0x1A727A0,getInt,sizeof(getInt))&&SameBytes(b+0x4A48E0,getRef,sizeof(getRef));
}
bool QualifiedInputCVarD53(void* var) noexcept {
    __try {
        const auto b=g_prompt_base_d49;if(!b||!var||!retail_build112::Ready(b))return false;
        const auto* vt=*reinterpret_cast<const std::uintptr_t**>(var);
        const auto v=reinterpret_cast<std::uintptr_t>(vt);
        const bool plain=v==b+kIntVtD49,ref=v==b+kRefVtD49;
        return (plain||ref)&&vt[0x10/8]==b+(plain?0x1A727A0:0x4A48E0)&&vt[0x38/8]==b+(plain?0x4D857C:0xC05DB8);
    } __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void* InputDeviceCVarD49() noexcept {
    __try {
        const auto b=g_prompt_base_d49;if(!b||!retail_build112::Ready(b))return nullptr;
        void* con=*reinterpret_cast<void**>(b+kConsolePtrD49);if(!con)return nullptr;
        const auto* cvt=*reinterpret_cast<const std::uintptr_t**>(con);
        if(reinterpret_cast<std::uintptr_t>(cvt)!=b+kConsoleVtD49||cvt[0xB8/8]!=b+kGetCVarD49)return nullptr;
        using Get=void*(__fastcall*)(void*,const char*);
        void* var=reinterpret_cast<Get>(cvt[0xB8/8])(con,"wh_ui_InputDevice");
        return QualifiedInputCVarD53(var)?var:nullptr;
    } __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
bool CVarReadD49(void* var,int& value) noexcept {
    __try {if(!var)return false;using Fn=int(__fastcall*)(void*);
        value=(*reinterpret_cast<Fn**>(var))[0x10/8](var);return true;
    } __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool CVarWriteD49(void* var,int value) noexcept {
    __try {if(!var)return false;using Fn=void(__fastcall*)(void*,int);
        (*reinterpret_cast<Fn**>(var))[0x38/8](var,value);return true;
    } __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void ReassertPromptFamilyD49(int forced) noexcept {
    if(g_in_cvar_d49)return;
    g_in_cvar_d49=true;
    void* var=InputDeviceCVarD49();
    const auto result=prompt_family_d49::reassert(forced,
        [&](int& v) noexcept {return CVarReadD49(var,v);},
        [&](int v) noexcept {return CVarWriteD49(var,v);});
    g_in_cvar_d49=false;
    if(!PromptCaptureActiveD50())return;
    using R=prompt_family_d49::Result;
    if(result==R::Changed)g_cvar_changed_d49.fetch_add(1,std::memory_order_relaxed);
    else if(result==R::Rejected)g_cvar_rejected_d49.fetch_add(1,std::memory_order_relaxed);
    else if(result==R::Unavailable)g_cvar_unavailable_d49.fetch_add(1,std::memory_order_relaxed);
}
// D53: ActionHint reads the CVar directly even when no manager transition is
// delivered. Repair at that exact native read, after the original lookup. The
// return site includes the literal-name load and GetCVar slot; all other CVars,
// callers and pointer-owned menus pass through untouched. No worker polling.
bool ActionHintPinsD53(std::uintptr_t b) noexcept {
    static const unsigned char caller[]{0x48,0x8B,0x0D,0x9C,0x6D,0x04,0x04,0x48,0x8D,0x15,0x15,0xD7,0x2D,0x03,
        0x48,0x8B,0x01,0xFF,0x90,0xB8,0,0,0,0x48,0x8B,0x08};
    static const unsigned char name[]{'w','h','_','u','i','_','I','n','p','u','t','D','e','v','i','c','e',0};
    return CVarPinsD49(b)&&SameBytes(b+0x8E6B05,caller,sizeof(caller))&&SameBytes(b+0x3BC4228,name,sizeof(name));
}
void ObserveHintFamilyD53(int forced,int before,int after,prompt_family_d49::Result result) noexcept {
    if(!PromptCaptureActiveD50())return;
    static thread_local std::uint64_t last=0;static thread_local int lastBefore=-999,lastAfter=-999;
    const auto now=GetTickCount64();
    if(before==lastBefore&&after==lastAfter&&now>=last&&now-last<5000)return;
    last=now;lastBefore=before;lastAfter=after;
    char line[240]{};std::snprintf(line,sizeof(line),"D53 PROMPT_CONSUMER boundary=ACTION_HINT forced=%d before=%d after=%d result=%u qualified=1 workerPolling=0",
        forced,before,after,unsigned(result));diagnostic_trace::Write(line);
}
void* __fastcall InputDeviceGetD53(void* console,const char* name) {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    void* var=g_hint_original_d53?g_hint_original_d53(console,name):nullptr;
    if(caller!=g_prompt_base_d49+0x8E6B1C||!g_hint_ready_d53.load(std::memory_order_acquire)||
       g_suspended.load(std::memory_order_acquire)||g_in_cvar_d49||!QualifiedInputCVarD53(var))return var;
    const int forced=prompt_family_d49::semanticFamilyD68(g_forced_device.load(std::memory_order_acquire));
    int before=-1,after=-1;g_in_cvar_d49=true;
    const auto result=prompt_family_d49::reassert(forced,
        [&](int& v) noexcept {const bool ok=CVarReadD49(var,v);if(ok){if(before<0)before=v;after=v;}return ok;},
        [&](int v) noexcept {return CVarWriteD49(var,v);});
    g_in_cvar_d49=false;ObserveHintFamilyD53(forced,before,after,result);return var;
}
void __fastcall Detour(void* self,int requested) {
    if(!g_original)return;
    const auto navigation=g_navigation.load(std::memory_order_acquire);
    bool wantsNavigation=requested==2||requested==4;
    {std::lock_guard<std::recursive_mutex> lock(g_cursor_mutex);
     if(navigation!=g_navigation_seen){g_navigation_seen=navigation;wantsNavigation=true;}}
    if(wantsNavigation)NativeNavigation();
    if(g_suspended.load(std::memory_order_acquire)){g_original(self,requested);return;}
    const int forced=prompt_family_d49::semanticFamilyD68(g_forced_device.load(std::memory_order_acquire));
    if(requested!=forced&&PromptCaptureActiveD50())g_forced_calls.fetch_add(1,std::memory_order_relaxed);
    int current=0;
    if(!CurrentDevice(self,current)||current!=forced)g_original(self,forced);
    // Native callback only: no worker polling, cursor injection or input remap.
    ReassertPromptFamilyD49(forced);
}

}

bool Install() noexcept {
    if(g_installed.load(std::memory_order_acquire)) return true;
    // Do not layer two providers onto the same native setter/refcount lifecycle.
    if(GetModuleHandleW(L"KCD2XboxPromptLock.asi")){
        g_external.store(true);g_installed.store(true);
        diagnostic_trace::Write("V66.90R1-PROMPTS external KCD2XboxPromptLock detected; internal hooks NOT installed; external cursor/version coverage unverified");
        return true;
    }
    HMODULE wh=GetModuleHandleW(L"WHGame.dll");
    if(!wh){ diagnostic_trace::Write("V64.20-PROMPT-LOCK unavailable: WHGame.dll missing"); return false; }
    const auto base=reinterpret_cast<std::uintptr_t>(wh);
    std::uint32_t ts=0;
    if(!ReadTimestamp(base,ts) || ts!=kSupportedWhTimestamp){
        diagnostic_trace::Write("V64.20-PROMPT-LOCK fail-closed: unsupported WHGame timestamp"); return false;
    }
    if(!retail_build112::Verify(base)){
        diagnostic_trace::Write("D49 PROMPT_LOCK fail-closed: retail SHA mismatch");return false;
    }
    g_prompt_base_d49=CVarPinsD49(base)?base:0;
    const auto target=base+kInputDeviceManagerSetRva;
    if(!MatchEntry(target)){
        diagnostic_trace::Write("V64.20-PROMPT-LOCK fail-closed: InputDeviceManagerSet signature mismatch"); return false;
    }
    DetourModKit::hook::InlineRequest req{"kcd2vr_prompt_device_lock",DetourModKit::hook::Target{DetourModKit::Address{target}},DetourModKit::hook::Options{}};
    auto r=DetourModKit::hook::inline_at(std::move(req),&Detour);
    if(!r){ diagnostic_trace::Write("V64.20-PROMPT-LOCK fail-closed: hook install failed"); return false; }
    g_hook=std::move(*r); g_original=g_hook->original<SetInputDeviceFn>();
    if(!g_original){ g_hook.reset(); diagnostic_trace::Write("V64.20-PROMPT-LOCK fail-closed: original unavailable"); return false; }
    auto en=g_hook->enable();
    if(!en){
        g_original=nullptr; g_hook.reset();
        diagnostic_trace::Write("V64.20-PROMPT-LOCK fail-closed: hook enable failed"); return false;
    }
    HMODULE hintOwnerD53=nullptr;
    const bool hintPinnedD53=GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&InputDeviceGetD53),&hintOwnerD53)&&hintOwnerD53;
    if(!g_hint_hook_d53&&hintPinnedD53&&g_prompt_base_d49&&ActionHintPinsD53(base)){
        auto hint=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{"d53_action_hint_family",
            DetourModKit::hook::Target{DetourModKit::Address{base+kGetCVarD49}},DetourModKit::hook::Options{}},&InputDeviceGetD53);
        if(hint){g_hint_hook_d53=std::move(*hint);g_hint_original_d53=g_hint_hook_d53->original<GetCVarFnD53>();
            if(g_hint_original_d53){g_hint_ready_d53.store(true,std::memory_order_release);
                if(!g_hint_hook_d53->enable())g_hint_ready_d53.store(false,std::memory_order_release);}}
        // Failed enable may still have crossed the patch boundary. Keep both
        // hook storage and trampoline for original-only pass-through until exit.
        // A missing original was never enabled and can release its disabled hook.
        if(!g_hint_original_d53)g_hint_hook_d53.reset();
    }
    // Exact supported registration/listener contracts. No wildcard scan or
    // cursor-manager lookup in a frame loop. All calls occur inside native/OS callbacks.
    static const unsigned char registration[]={0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x30,0x48,0x8B,0xF9,0x84,0xD2,0x74,0x5B};
    static const unsigned char listener[]={0x48,0x83,0xEC,0x28,0x83,0xFA,0x01,0x75,0x12,0x48,0x8B,0x0D};
    bool cursorOk=SameBytes(base+0x8EABA4,registration,sizeof(registration))&&SameBytes(base+0x2B139D4,listener,sizeof(listener));
    if(cursorOk){
        g_listener=reinterpret_cast<ListenerFn>(base+0x2B139D4);
        auto registerHook=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{"ui90r1_cursor_registration",DetourModKit::hook::Target{DetourModKit::Address{base+0x8EABA4}},DetourModKit::hook::Options{}},&RegisterCursor);
        const auto user=GetModuleHandleW(L"user32.dll");
        const auto cursorAddress=user?reinterpret_cast<std::uintptr_t>(GetProcAddress(user,"SetCursor")):0;
        cursorOk=bool(registerHook)&&cursorAddress!=0;
        if(cursorOk){
            g_registration_hook=std::move(*registerHook);g_registration=g_registration_hook->original<RegistrationFn>();
            auto cursorHook=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{"ui90r1_set_cursor",DetourModKit::hook::Target{DetourModKit::Address{cursorAddress}},DetourModKit::hook::Options{}},&SetCursorDetour);
            cursorOk=bool(cursorHook);
            if(cursorOk){g_cursor_hook=std::move(*cursorHook);g_cursor=g_cursor_hook->original<CursorFn>();
                cursorOk=g_registration&&g_cursor&&bool(g_cursor_hook->enable());
                if(cursorOk){g_cursor_ready.store(true,std::memory_order_release);cursorOk=bool(g_registration_hook->enable());}}
        }
    }
    if(!cursorOk){
        g_cursor_ready.store(false,std::memory_order_release);
        if(g_cursor_hook){(void)g_cursor_hook->disable();g_cursor_hook.reset();}
        if(g_registration_hook){(void)g_registration_hook->disable();g_registration_hook.reset();}
        g_registration=nullptr;g_cursor=nullptr;g_listener=nullptr;
    }
    diagnostic_trace::Write(cursorOk?"V66.90R1-PROMPTS classifier + balanced native cursor lifecycle READY; mixed input; no polling":
        "V66.90R1-PROMPTS cursor bridge UNAVAILABLE; pointer-only device unlock fallback, not full mixed-input success");
    g_installed.store(true,std::memory_order_release);
    { char m[256]{}; const int forced=prompt_family_d49::semanticFamilyD68(g_forced_device.load(std::memory_order_acquire));
      std::snprintf(m,sizeof(m),"D68 PROMPT_LOCK semanticFamily=%d(%s) glyphOnly=1 keyboard/mouse execution retained",forced,forced==4?"PlayStation":"Xbox");
      diagnostic_trace::Write(m); }
    return true;
}

void Uninstall() noexcept {
    g_flash_ready_d56.store(false,std::memory_order_release);
    g_hint_ready_d53.store(false,std::memory_order_release);
    if(g_hint_hook_d53)(void)g_hint_hook_d53->disable();
    // Retain trampoline/handle even on successful disarm: a callback admitted
    // immediately before shutdown may still be inside original GetCVar. This
    // optional hook has process lifetime (module pinned before enable).
    g_cursor_ready.store(false,std::memory_order_release);
    if(g_cursor_hook){(void)g_cursor_hook->disable();g_cursor_hook.reset();}
    if(g_registration_hook){(void)g_registration_hook->disable();g_registration_hook.reset();}
    // Called at mod shutdown, not a supported hot-unload/reload operation.
    // Never call a game cursor manager from the worker just to balance shutdown.
    if(g_hook){ (void)g_hook->disable(); g_hook.reset(); }
    g_original=nullptr; g_installed.store(false,std::memory_order_release);
}

bool Installed() noexcept { return g_installed.load(std::memory_order_acquire); }
bool RequestFlashStyleD56(int device) noexcept {
    if(!g_flash_ready_d56.load(std::memory_order_acquire)){
        const bool ok=native_script_t2::Register(&FlashServiceD56,&FlashWantedD56)&&native_script_t2::Install();
        if(!ok)return false;
        g_flash_ready_d56.store(true,std::memory_order_release);
    }
    auto before=g_flash_request_d56.load(std::memory_order_acquire);
    for(;;){const auto after=((before&~std::uint64_t(7))+8)|std::uint64_t(prompt_refresh_d56::family(device));
        if(g_flash_request_d56.compare_exchange_weak(before,after,std::memory_order_acq_rel))return true;}
}
int AppliedGlyphStyleR129() noexcept {return g_applied_style_r129.load(std::memory_order_acquire);}
void NoteGlyphPathR129(int path,bool override) noexcept {
    if(override){g_glyph_path_r129.store(path,std::memory_order_release);return;}
    int none=0;(void)g_glyph_path_r129.compare_exchange_strong(none,path,std::memory_order_acq_rel);
}
void PublishMinigamePromptsD80(bool active,std::uint64_t lifetime,unsigned phase,bool openPalm,std::uint64_t now,bool alchemy) noexcept {
    static prompt_policy_d80::State state;
    static prompt_policy_d80::Publication publication;
    const prompt_policy_d80::Frame frame{active,openPalm,prompt_policy_d80::timed.load(std::memory_order_acquire),
        lifetime,now,phase,ui_hud_r160::StationScale(prompt_policy_d80::minigameScale.load(std::memory_order_acquire),ui_hud_r160::Unified()),alchemy}; // R160 UI: the parents carry the one icon size, crafting keeps its own
    const auto next=state.update(frame);const auto generation=flashui_probe::HudGeneration();
    const auto revision=prompt_policy_d80::refreshRevision.load(std::memory_order_acquire);
    if(!publication.Needed(next,generation,revision)&&!publication.Reassert(next,now))return; // R145: + every 1 s at a station
    if(!g_flash_ready_d56.load(std::memory_order_acquire)&&!RequestFlashStyleD56(g_forced_device.load(std::memory_order_acquire)))return;
    publication.Published(next,generation,revision,now);
    auto before=g_station_request_d80.load(std::memory_order_acquire);
    for(;;){const auto ticket=((before&~std::uint64_t(65535))+65536)|
        (std::uint64_t(next.percent)<<8)|(next.active?1:0)|(next.visible?2:0);
        if(g_station_request_d80.compare_exchange_weak(before,ticket,std::memory_order_acq_rel))break;}
}
void SetForcedDevice(int device) noexcept {
    const int forced=device==4?4:2;
    const int previous=g_forced_device.exchange(forced,std::memory_order_acq_rel);
    if(previous!=forced) {
        char m[192]{}; std::snprintf(m,sizeof(m),"D68 PROMPT_LOCK glyphStyle=%s savedStyle=%d semanticFamily=2",forced==4?"PLAYSTATION":"XBOX",forced); diagnostic_trace::Write(m);
    }
}
int ForcedDevice() noexcept { return prompt_family_d49::semanticFamilyD68(g_forced_device.load(std::memory_order_acquire)); }
void SetSuspended(bool suspended) noexcept {
    if(g_external.load())return;
    const bool old=g_suspended.exchange(suspended,std::memory_order_acq_rel);
    if(old!=suspended) diagnostic_trace::Write(suspended
        ? "V66.90R1-PROMPT-LOCK pointer-only KBM fallback (cursor bridge unavailable)"
        : "V66.90R1-PROMPT-LOCK controller family restored on next native device update");
}
bool Suspended() noexcept { return g_suspended.load(std::memory_order_acquire); }

bool MixedCursorReady() noexcept{return g_cursor_ready.load(std::memory_order_acquire);}
void NotifyNavigation() noexcept {g_navigation.fetch_add(1,std::memory_order_release);}
void WriteCursorSnapshot() noexcept {
    if(!PromptCaptureActiveD50())return;
    unsigned registrations=0;bool lease=false;
    {std::lock_guard<std::recursive_mutex> lock(g_cursor_mutex);registrations=g_cursor_state.Count();lease=g_cursor_state.Visible();}
    char text[440]{};std::snprintf(text,sizeof(text),"V66.90R1-CURSOR ready=%d external=%d registrations=%u extraLease=%d changes=%llu errors=%llu cvarQualified=%d cvarChanged=%llu cvarRejected=%llu cvarUnavailable=%llu hintConsumerReady=%d",
        g_cursor_ready.load()?1:0,g_external.load()?1:0,registrations,lease?1:0,
        (unsigned long long)g_cursor_changes.load(),(unsigned long long)g_cursor_errors.load(),g_prompt_base_d49?1:0,
        (unsigned long long)g_cvar_changed_d49.load(),(unsigned long long)g_cvar_rejected_d49.load(),
        (unsigned long long)g_cvar_unavailable_d49.load(),g_hint_ready_d53.load()?1:0);diagnostic_trace::Write(text);
}

} // namespace kcd2vr::prompt_device_lock
