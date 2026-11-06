#include "diagnostics/hud_coverage168r1.h"
#include "experiments/claude_bundle_d70.h"
#include "diagnostics/hook_cost178r22.h" // V66.178R22c HUD cost slots
#include "diagnostics/capture168.h"
#include "ui_context_panel.h"
#include "ui_modal_overlay43.h"
#include "ui_helmet_overlay63.h"
#include "ui_contextual_hud162r1.h"
#include "ui_hud_transition170.h"
#include "ui_hud_elements171.h"
#include "ui_hud_work59c7.h"
#include "ui_hud_visibility175.h"
#include "action_map_probe.h"
#include <mutex>
#include "ui_prompt_scale120r3.h"
#include "ui_hud_r112.h" // R112 combat crosshair size + quick-slot fade
#include "ui_station_prompts_d74.h"
#include "../prompt_policy_d80.h"
#include "ui_hud_geometry.h"
#include "ui_native_canvas.h"
#include "ui_palm_alpha44c2.h"
#include "ui_pause_overlay.h"
#include "ui_pause_page59c19.h"
#include "ui_death_overlay135r2.h"
#include "ui_native_state.h"
#include "flashui_probe.h"
#include "subsystems/dice/selection_marker_r92.h"
#include "activity_input163.h" // R98: dice game lifetime for the dice ring hide
#include "retail_build112.h"
#include "debug_trace.h"
#include "diagnostic_trace.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include "core/readable_pages178r22c1.h"
#include <atomic>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <cmath>
#include <optional>
#include <utility>

namespace kcd2vr::flashui_probe {
namespace {
std::recursive_mutex g_hud_draw_mutex162r1;
ui_hud_transition170::Controller g_visibility170;
std::atomic<unsigned> g_user_hidden163{0};
std::atomic<void*> g_helmet_element63{nullptr};
std::atomic<std::uint64_t> g_helmet_generation63{0};
std::atomic<void*> g_wait_element_b2{nullptr};
std::atomic<std::uint64_t> g_wait_generation_b2{0};
std::array<std::atomic<void*>,2> g_activity_element_s1{};
std::array<std::atomic<std::uint64_t>,2> g_activity_generation_s1{};
std::atomic<std::uint64_t> g_transition_events170{0},g_transition_reads170{0},g_transition_writes170{0},g_transition_failures170{0};
std::atomic<unsigned> g_transition_hidden170{0};
std::atomic<bool> g_corrected36{false},g_contextual36{false};
ui_hud_work59c7::Children g_children59c7;
ui_hud_work59c7::SizedChildren<2> g_action_cursor_children_d69;
ui_hud_work59c7::SizedChildren<1> g_combat_cursor_children_r112; // R112 CombatCursorCross (scale only)
ui_hud_work59c7::SizedChildren<2> g_prompt_children_d74;
// Independent child epochs: notification replacement must not dirty HUD parents.
ui_hud_work59c7::SizedChildren<ui_prompt_scale120r3::Notifications::Names.size()> g_notification_children_r12;
constexpr auto kNotificationNamesR12=ui_prompt_scale120r3::Notifications::Names;
constexpr auto kNotificationPathsR12=ui_prompt_scale120r3::Notifications::Paths;
std::atomic<bool> g_vitals_pin59c7{false};
// One semantic snapshot: palm active/left/right and semi-alchemy gaze cursor.
std::atomic<unsigned> g_semantic_owner54{0};
ui_hud_work59c7::Vitals g_vitals59c7;
ui_hud_work59c7::Retry g_vitals_retry59c7,g_elements_retry59c7;
std::uint64_t g_parent_key59c7=0;bool g_parent_ready59c7=false;
bool g_display_verified170=false;
bool VerifyHudDisplay170(std::uintptr_t b) noexcept;
ui_hud_visibility175::Registry g_native_visibility175;
std::atomic<bool> g_hud_repair175{false},g_hud_repair_ready175{false};
std::atomic<std::uint64_t> g_hud_repair_epoch175{0};
std::atomic<std::uint64_t> g_native_vis_calls175{0},g_native_vis_qualified175{0},g_native_vis_suppressed175{0};
std::atomic<std::uintptr_t> g_hud_native_base175{0};
thread_local unsigned g_hud_owned_write175=0;
bool HudCaptureActive175() noexcept {
    using namespace diagnostics::capture168;
    return (gate.mask.load(std::memory_order_relaxed)&(1u<<unsigned(Domain::Hud)))&&
        active(Domain::Hud,GetTickCount64());
}
void RemoveHudRepairHooks175() noexcept;
bool ReadHudKey175(void*,void*,std::uint64_t,ui_hud_visibility175::Key&) noexcept;
constexpr std::uintptr_t kFlashUiGlobalRva = 0x492D938ull; // proven by HUD/cursor static xrefs
constexpr std::uintptr_t kUiHolderReleaseRva = 0x3968ACull;  // exact cleanup used after GetElement("hud") at WHGame+0x89B68C
constexpr std::uint32_t kSupportedWhTimestamp = 0x6A350E20u;
using GetElementFn = void*(__fastcall*)(void*, void*, const char*);
std::optional<DetourModKit::hook::Hook> g_hook;
GetElementFn g_original=nullptr;
std::atomic<std::uint64_t> g_calls{0};
std::atomic<std::uint64_t> g_hud_calls{0};
std::atomic<void*> g_last_hud{nullptr};
std::atomic<std::uint64_t> g_last_hud_tick{0};
std::atomic<std::uint64_t> g_hud_generation{0};
// V66.20 passive menu ownership hint. We only remember names WHGame itself
// requested through FlashUI::GetElement; no timer-driven probing is introduced.
std::atomic<std::uint32_t> g_recent_large_menu_mask{0};
// R59.16C10R14: the GAME's own GetElement("Menu") lookups (our AcquireElementDirect calls are excluded through the
// depth counter). Historical diagnostics only: actual page re-layout uses ShowPage in C10R19.
std::atomic<int> g_self_lookup_depth59c14{0};
std::atomic<std::uint64_t> g_menu_game_lookups59c14{0};
// C10R19: real native page completion, not GetElement/update traffic.
using ShowPageFn19=void(__fastcall*)(void*);
std::optional<DetourModKit::hook::Hook> g_page_hook19;
ShowPageFn19 g_page_original19=nullptr;
std::atomic<std::uint64_t> g_menu_page_events19{0};
bool InstallPageObserver19() noexcept;
void RemovePageObserver19() noexcept;

std::atomic<std::uint64_t> g_recent_large_menu_tick{0};
// V66.87 independent age per exact retail UIElement identity.
std::array<std::atomic<std::uint64_t>,12> g_recent_ui_ticks{};

// V64.22 F10 bounded UI/action discovery. We record unique GetElement names for
// the currently user-labelled scenario. Fixed storage only; no game-memory writes.
std::atomic<int> g_ui_discovery_mode{0};
std::atomic_flag g_ui_discovery_lock = ATOMIC_FLAG_INIT;
std::array<std::array<char,64>,64> g_ui_discovery_names{};
std::size_t g_ui_discovery_name_count=0;

const char* UiDiscoveryModeNameLocal(int mode) noexcept {
    switch(mode){
    case 0:return "OFF";
    case 1:return "ALCHEMY / CRAFTING";
    case 2:return "BUYING / SHOP";
    case 3:return "LOOT / TRANSFER";
    case 4:return "ITEM PICK / SELECTION";
    case 5:return "GENERAL UI / ACTION";
    default:return "UNKNOWN";
    }
}
void RecordUiDiscoveryName(const char* name) noexcept {
    const int mode=g_ui_discovery_mode.load(std::memory_order_acquire);
    if(mode<=0 || !name || !*name) return;
    if(g_ui_discovery_lock.test_and_set(std::memory_order_acquire)) return;
    for(std::size_t i=0;i<g_ui_discovery_name_count;++i){
        if(_stricmp(g_ui_discovery_names[i].data(),name)==0){g_ui_discovery_lock.clear(std::memory_order_release);return;}
    }
    if(g_ui_discovery_name_count<g_ui_discovery_names.size()){
        auto& dst=g_ui_discovery_names[g_ui_discovery_name_count++];
        std::strncpy(dst.data(),name,dst.size()-1);
        char m[256]{}; std::snprintf(m,sizeof(m),"V64.22-F10-UI mode=%d(%s) element='%s' UNIQUE READ-ONLY",mode,UiDiscoveryModeNameLocal(mode),dst.data());
        diagnostic_trace::Write(m); debug_trace::Write(m);
    }
    g_ui_discovery_lock.clear(std::memory_order_release);
}

// V64.15 F4 readback state. Main-loop owned; stored here so the read-only
// stability probe can compare later game/APSE layout passes to the exact targets.
struct VrHudTarget { const char* name; float x=0,y=0,scale=1; bool valid=false; float xs=100,ys=100; };
std::array<VrHudTarget,23> g_vrhud_targets{{
{"Subtitles"},{"DialogLeft"},{"DialogRight"},{"Hints"},{"Compass"},{"CombatCursorCross"},{"TutorialInDialog"},{"TutorialMessage"},{"Stats"},{"QAMWeapon"},{"QAMFood"},{"InfoText"},{"GameLog"},{"Crime"},{"Wanted"},{"Trespassing"},{"FancyEvent"},{"SkillCheck"},{"ItemTransfer"},{"Buffs"},{"CommonEvent"},{"ShootingContest"},{"DiceContainer"}
}};
struct VrHudBaseline {
    const char* name;
    std::array<unsigned char,0x38> info{};
    bool valid=false;
    std::uint64_t generation=0;
    void* clip=nullptr;
};
std::array<VrHudBaseline,23> g_vrhud_baselines{{
{"Subtitles"},{"DialogLeft"},{"DialogRight"},{"Hints"},{"Compass"},{"CombatCursorCross"},{"TutorialInDialog"},{"TutorialMessage"},{"Stats"},{"QAMWeapon"},{"QAMFood"},{"InfoText"},{"GameLog"},{"Crime"},{"Wanted"},{"Trespassing"},{"FancyEvent"},{"SkillCheck"},{"ItemTransfer"},{"Buffs"},{"CommonEvent"},{"ShootingContest"},{"DiceContainer"}
}};
std::uint64_t g_targets_generation=~0ull;
ui_native_canvas::Transaction g_canvas;
ui_native_canvas::Report g_canvas_report{};
ui_hud_elements171::Controller g_elements171;
bool g_canvas_requested=false;
std::atomic<void*> g_canvas_live_hud{nullptr};
std::atomic<std::uint64_t> g_canvas_lifetime{0};
bool g_safe_hud_active=false; // UI-worker owned; never forces native clip visibility.

enum class AutoStage : int { Ok=0, Busy=1, AcquireHud=2, FindMasterSwitch=3, GetDisplayInfo=4, BaselineInvalid=5, SetDisplayInfo=6, ReadbackMismatch=7 };
std::atomic<int> g_auto_stage{0};
void SetAutoStage(AutoStage s) noexcept { g_auto_stage.store(static_cast<int>(s),std::memory_order_release); }
const char* AutoStageName(int s) noexcept {
    switch(static_cast<AutoStage>(s)) {
    case AutoStage::Ok:return "ok"; case AutoStage::Busy:return "busy"; case AutoStage::AcquireHud:return "acquire_hud";
    case AutoStage::FindMasterSwitch:return "find_MasterSwitch"; case AutoStage::GetDisplayInfo:return "get_display_info";
    case AutoStage::BaselineInvalid:return "baseline_invalid"; case AutoStage::SetDisplayInfo:return "set_display_info";
    case AutoStage::ReadbackMismatch:return "readback_mismatch"; default:return "unknown";
    }
}

std::uintptr_t WhBase() noexcept { return reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")); }
bool SupportedBuild(std::uintptr_t base) noexcept {
    if(!base) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        return nt->Signature==IMAGE_NT_SIGNATURE && nt->FileHeader.TimeDateStamp==kSupportedWhTimestamp;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
bool SafePtr(const void* p) noexcept {
    return memory126::readableNow178r22c1(p,1);
}
template<class T> bool Read(const void* p,T& out) noexcept { if(!memory126::readableNow178r22c1(p,sizeof(T))) return false; __try{out=*reinterpret_cast<const T*>(p); return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;} }

bool ReadPointerSeh(const void* p, void*& out) noexcept {
    out = nullptr;
    if (!SafePtr(p)) return false;
    __try {
        out = *reinterpret_cast<void* const*>(p);
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        out = nullptr;
        return false;
    }
}

void* __fastcall DetourBody(void* self, void* out, const char* name);
// MSVC C2712: the cost scope (a destructor) lives in this wrapper; the SEH body is its own function.
void* __fastcall Detour(void* self, void* out, const char* name) {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::FlashGetElement};return DetourBody(self,out,name);}
void* __fastcall DetourBody(void* self, void* out, const char* name) {
    void* ret=nullptr;{const bool measured=kcd2vr::hookcost178r22::owner(kcd2vr::hookcost178r22::FlashGetElement)!=nullptr;const auto og0=measured?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{};ret = g_original ? g_original(self,out,name) : nullptr;if(measured)kcd2vr::hookcost178r22::addOriginal(kcd2vr::hookcost178r22::FlashGetElement,std::uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-og0).count()));} // no destructor objects in this SEH function
    void* observed_element=nullptr;if(out)Read(out,observed_element);
    if(observed_element)ui_native_state::ObserveElement(observed_element);
    const auto n=g_calls.fetch_add(1,std::memory_order_relaxed)+1;
    bool hud=false; char local[64]{};
    if(name && SafePtr(name)) { __try{ std::strncpy(local,name,sizeof(local)-1); } __except(EXCEPTION_EXECUTE_HANDLER){local[0]=0;} }
    if(local[0]) RecordUiDiscoveryName(local);
    if(local[0]) {
        std::uint32_t bit=0; int bitIndex=-1;
        if(_stricmp(local,"ApseInventoryList")==0 || _stricmp(local,"apse_inventory")==0) {bit=1u<<0;bitIndex=0;}
        else if(_stricmp(local,"ApseMap")==0) {bit=1u<<1;bitIndex=1;}
        else if(_stricmp(local,"Menu")==0) {bit=1u<<2;bitIndex=2;if(g_self_lookup_depth59c14.load(std::memory_order_relaxed)==0)g_menu_game_lookups59c14.fetch_add(1,std::memory_order_relaxed);}
        else if(_stricmp(local,"ItemSelection")==0) {bit=1u<<3;bitIndex=3;}
        else if(_stricmp(local,"ItemTransfer")==0) {bit=1u<<4;bitIndex=4;}
        else if(_stricmp(local,"AlchemyBook")==0) {bit=1u<<5;bitIndex=5;}
        else if(_stricmp(local,"ApseModalDialog")==0) {bit=1u<<6;bitIndex=6;}
        else if(_stricmp(local,"Overlay")==0) {bit=1u<<7;bitIndex=7;
            if(observed_element&&g_helmet_element63.exchange(observed_element)!=observed_element)g_helmet_generation63.fetch_add(1);}
        else if(_stricmp(local,"GameOver")==0) {bit=1u<<8;bitIndex=8;}
        else if(_stricmp(local,"LoadingScreen")==0) {bit=1u<<9;bitIndex=9;}
        else if(_stricmp(local,"Progress")==0) {bit=1u<<10;bitIndex=10;}
        else if(_stricmp(local,"SkipTime")==0) {bit=1u<<11;bitIndex=11;
            if(observed_element&&g_wait_element_b2.exchange(observed_element)!=observed_element)g_wait_generation_b2.fetch_add(1);}
        for(unsigned i=0;i<2;++i){
            const char* activity=i==0?"LockPicking":"Pickpocketing";
            if(observed_element&&_stricmp(local,activity)==0&&
               g_activity_element_s1[i].exchange(observed_element)!=observed_element)
                g_activity_generation_s1[i].fetch_add(1,std::memory_order_acq_rel);
        }
        if(bit) {
            const auto now=static_cast<std::uint64_t>(GetTickCount64());
            g_recent_large_menu_mask.fetch_or(bit,std::memory_order_acq_rel);
            g_recent_large_menu_tick.store(now,std::memory_order_release);
            if(bitIndex>=0) g_recent_ui_ticks[static_cast<std::size_t>(bitIndex)].store(now,std::memory_order_release);
        }
    }
    if(local[0] && (_stricmp(local,"hud")==0)) hud=true;
    if(hud) {
        const auto h=g_hud_calls.fetch_add(1,std::memory_order_relaxed)+1;
        void* out0=nullptr; if(out) Read(out,out0);
        if(out0 && SafePtr(out0)) {
            void* previous=g_last_hud.exchange(out0,std::memory_order_acq_rel);
            if(previous!=out0) g_hud_generation.fetch_add(1,std::memory_order_acq_rel);
            g_last_hud_tick.store(GetTickCount64(),std::memory_order_release);
        }
        if(h<=8 || h%120==0) {
            char msg[512]{}; std::snprintf(msg,sizeof(msg),
                "V61.21-FLASHUI HUD-LOOKUP call=%llu hudCall=%llu iface=%p out=%p out0=%p ret=%p name='%s'",
                (unsigned long long)n,(unsigned long long)h,self,out,out0,ret,local); debug_trace::Write(msg);
        }
    }
    return ret;
}
}

bool Install() noexcept {
    // V65.16: startup can occur before the retail FlashUI global is published.
    // Make installation idempotent so event-driven HUD/presentation transitions can
    // retry the observer later without stacking hooks or polling every frame.
    if(g_hook && g_original) return true;
    const auto base=WhBase(); if(!SupportedBuild(base)||!retail_build112::Verify(base)){debug_trace::Write("V61.43-FLASHUI install skipped: unsupported WHGame build/timestamp"); return false;}
    // This exact code/vtable hook does not need the later-created UI singleton.
    g_display_verified170=VerifyHudDisplay170(base);
    void* iface=nullptr; ReadPointerSeh(reinterpret_cast<const void*>(base+kFlashUiGlobalRva), iface);
    if(!iface || !SafePtr(iface)){debug_trace::Write("V61.21-FLASHUI global 0x492D938 unresolved/null"); return false;}
    void** vt=nullptr; if(!Read(iface,vt) || !vt || !SafePtr(vt)){debug_trace::Write("V61.21-FLASHUI global resolved but vtable unreadable"); return false;}
    void* target=nullptr; if(!Read(reinterpret_cast<unsigned char*>(vt)+0x70,target) || !target){debug_trace::Write("V61.21-FLASHUI vslot+0x70 target unreadable"); return false;}
    char pre[384]{}; std::snprintf(pre,sizeof(pre),"V61.21-FLASHUI static-xref confirmed global_rva=0x492D938 iface=%p vt=%p slot70=%p strings={hud@0x3A55B7C,CombatCursorCross@0x3C33BE0,CursorCrossPBSlot@0x3C33BC8}",iface,vt,target); debug_trace::Write(pre);
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6121_flashui_getelement",DetourModKit::hook::Target{DetourModKit::Address{reinterpret_cast<std::uintptr_t>(target)}},DetourModKit::hook::Options{}};
    auto result=DetourModKit::hook::inline_at(std::move(req),&Detour); if(!result){debug_trace::Write("V61.21-FLASHUI slot+0x70 passive hook failed"); return false;}
    g_hook=std::move(*result); g_original=g_hook->original<GetElementFn>(); auto e=g_hook->enable(); if(!e){g_original=nullptr;g_hook.reset();debug_trace::Write("V61.21-FLASHUI slot+0x70 enable failed");return false;}

    const bool pageObserver19=InstallPageObserver19();
    diagnostic_trace::Write(pageObserver19?"C10R19 PAUSE_PAGE observer=READY native=1F8DB80 scope=OWNED_MENU ShowPage=EVENT_ONLY":"C10R19 PAUSE_PAGE observer=UNAVAILABLE entryLayout=RETAINED periodicRefit=OFF");
    debug_trace::Write("V61.33-FLASHUI HUD observer enabled; F12 cursor proof retained; semantic MasterSwitch scale is user-triggered only"); return true;
}
std::uint64_t HudGeneration() noexcept { return g_hud_generation.load(std::memory_order_acquire); }

bool HasRecentHud() noexcept {
    void* hud=g_last_hud.load(std::memory_order_acquire);
    const auto seen=g_last_hud_tick.load(std::memory_order_acquire);
    const auto now=GetTickCount64();
    return hud && SafePtr(hud) && seen && now>=seen && now-seen<=2000ull;
}

std::uint64_t MenuPageEventSerial59c19() noexcept {return g_menu_page_events19.load(std::memory_order_relaxed);}
std::uint32_t RecentLargeMenuLookupMask(std::uint64_t maxAgeMs) noexcept {
    maxAgeMs=std::clamp<std::uint64_t>(maxAgeMs,50ull,5000ull);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    std::uint32_t live=0;
    for(std::size_t i=0;i<g_recent_ui_ticks.size();++i){
        const auto seen=g_recent_ui_ticks[i].load(std::memory_order_acquire);
        if(seen && now>=seen && now-seen<=maxAgeMs) live|=(1u<<static_cast<unsigned>(i));
    }
    return live;
}


namespace {

bool ExecPtr(const void* p) noexcept {
    if(!p) return false;
    MEMORY_BASIC_INFORMATION mbi{};
    if(!VirtualQuery(p,&mbi,sizeof(mbi)) || mbi.State!=MEM_COMMIT || (mbi.Protect&(PAGE_NOACCESS|PAGE_GUARD))) return false;
    const DWORD prot=mbi.Protect & 0xFFu;
    return prot==PAGE_EXECUTE || prot==PAGE_EXECUTE_READ || prot==PAGE_EXECUTE_READWRITE || prot==PAGE_EXECUTE_WRITECOPY;
}

// Retail WHGame uses a tiny smart-pointer holder for IUIElement::GetElement("hud"):
//   rcx = FlashUI global, rdx = &holder, r8 = "hud", call vtbl+0x70
//   hud = *returnValue; cleanup(&holder) @ WHGame+0x3968AC.
// Mirror that exact sequence and keep the HUD reference alive only for this call.
struct HudHolder {
    alignas(16) unsigned char bytes[0x20]{};
    bool acquired=false;
};

void ReleaseHudHolder(HudHolder& h) noexcept {
    if(!h.acquired) return;
    const auto base=WhBase();
    if(!SupportedBuild(base)) return;
    const auto addr=base+kUiHolderReleaseRva;
    if(!ExecPtr(reinterpret_cast<void*>(addr))) return;
    using ReleaseFn=void(__fastcall*)(void*);
    auto fn=reinterpret_cast<ReleaseFn>(addr);
    __try { fn(h.bytes); }
    __except(EXCEPTION_EXECUTE_HANDLER) { }
    h.acquired=false;
}

bool AcquireElementDirect(HudHolder& holder,const char* elementName,void*& element) noexcept {
    element=nullptr;
    if(!elementName || !*elementName) return false;
    const auto base=WhBase();
    if(!SupportedBuild(base)) return false;
    void* iface=nullptr;
    if(!ReadPointerSeh(reinterpret_cast<const void*>(base+kFlashUiGlobalRva),iface) || !iface || !SafePtr(iface)) return false;
    void** vt=nullptr;
    if(!Read(iface,vt) || !vt || !SafePtr(vt)) return false;
    void* target=nullptr;
    if(!Read(reinterpret_cast<unsigned char*>(vt)+0x70,target) || !ExecPtr(target)) return false;
    using DirectGetElementFn=void*(__fastcall*)(void*,void*,const char*);
    void* ret=nullptr;
    g_self_lookup_depth59c14.fetch_add(1,std::memory_order_relaxed); // R59.16C10R14: our own lookups are not menu page events
    __try { ret=reinterpret_cast<DirectGetElementFn>(target)(iface,holder.bytes,elementName); holder.acquired=true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { ret=nullptr; holder.acquired=false; }
    g_self_lookup_depth59c14.fetch_sub(1,std::memory_order_relaxed);
    if(!ret || !SafePtr(ret)) return false;
    if(!Read(ret,element) || !element || !SafePtr(element)) return false;
    return true;
}

bool AcquireHudDirect(HudHolder& holder, void*& hud) noexcept {
    return AcquireElementDirect(holder,"hud",hud);
}


bool FindMovieClip(void* hud,const char* name,void*& clip,void*& getTarget,void*& setTarget) noexcept {
    clip=nullptr; getTarget=nullptr; setTarget=nullptr;
    if(!hud || !name || !*name || !SafePtr(hud)) return false;
    void** hvt=nullptr;
    if(!Read(hud,hvt)||!hvt||!SafePtr(hvt)) return false;
    void* findTarget=nullptr;
    if(!Read(reinterpret_cast<unsigned char*>(hvt)+0x238,findTarget)||!ExecPtr(findTarget)) return false;
    using FindChildFn=void*(__fastcall*)(void*,const char*,int);
    __try { clip=reinterpret_cast<FindChildFn>(findTarget)(hud,name,0); }
    __except(EXCEPTION_EXECUTE_HANDLER) { clip=nullptr; }
    if(!clip||!SafePtr(clip)) return false;
    void** cvt=nullptr;
    if(!Read(clip,cvt)||!cvt||!SafePtr(cvt)) return false;
    if(!Read(reinterpret_cast<unsigned char*>(cvt)+0xC8,setTarget) || !ExecPtr(setTarget)) return false;
    if(!Read(reinterpret_cast<unsigned char*>(cvt)+0xD0,getTarget) || !ExecPtr(getTarget)) return false;
    return true;
}

// Exact retail proof is retained in verification/UI_NATIVE_BOUNDARIES_V175.md.
// Hooks observe actual native child resolution and one shared GFx visibility
// setter. They never acquire the worker HUD mutex or call back into FlashUI.
namespace visibility175 {
using Find=void*(__fastcall*)(void*,const char*,void*);
using Destroy=void(__fastcall*)(void*);
using Visible=void(__fastcall*)(void*,bool);
using Display54=bool(__fastcall*)(void*,void*);
Find find=nullptr;Destroy destroy=nullptr;Visible visible=nullptr;Display54 display54=nullptr;
std::array<std::optional<DetourModKit::hook::Hook>,4> hooks;
}
bool ReadHudKey175(void*wrapper,void*hud,std::uint64_t generation,ui_hud_visibility175::Key& out) noexcept {
    out={};const auto b=g_hud_native_base175.load(std::memory_order_acquire);
    if(!b||!wrapper||!hud)return false;
    __try {
        const auto w=reinterpret_cast<std::uintptr_t>(wrapper);
        if(*reinterpret_cast<const std::uintptr_t*>(w)!=b+0x3A55BA0)return false;
        if((*reinterpret_cast<const unsigned*>(w+0x10)&0x8fu)!=8u)return false;
        const auto handle=*reinterpret_cast<const std::uintptr_t*>(w+0x18);
        if(!handle)return false;
        const auto object=*reinterpret_cast<const std::uintptr_t*>(handle+8);
        if(!object||*reinterpret_cast<const std::uintptr_t*>(object+0xD0)!=handle)return false;
        const auto vt=*reinterpret_cast<const std::uintptr_t*>(object);
        if(!vt||*reinterpret_cast<const std::uintptr_t*>(vt+0x198)!=b+0x1C51240)return false;
        out={w,handle,object,reinterpret_cast<std::uintptr_t>(hud),generation};return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// R112: which ReadHudKey175 check refuses a child (0 = accepted). Every D74 HUD_PROMPT row since D79 (1 Nov) and all
// of R111 (21:36:25.592/.786/26.009, 21:41:08.243/.437/.649) shows resolved=0/0: the interaction-icon (Hints), control
// prompt (TutorialMessage) and action-cursor (CursorOutline/CursorCross) sizes were NEVER applied, although the same
// clips are found and read by HUD_VIS (CursorCross read=1 scale=100.00 in every snapshot) and the same
// NativeCanvasBackend resolves/writes Compass/Stats/QAM (HUD_ELEMENTS resolved=F). The only extra step those
// backends had is this native-key read, so its refusal no longer blocks the size; the reason goes to KCD2VR.log.
int HudKeyRefusalR112(void* wrapper,void* hud,unsigned& flags) noexcept {
    flags=0;const auto b=g_hud_native_base175.load(std::memory_order_acquire);
    if(!b)return 1;if(!wrapper||!hud)return 2;
    __try {
        const auto w=reinterpret_cast<std::uintptr_t>(wrapper);
        if(*reinterpret_cast<const std::uintptr_t*>(w)!=b+0x3A55BA0)return 3;
        flags=*reinterpret_cast<const unsigned*>(w+0x10);if((flags&0x8fu)!=8u)return 4;
        const auto handle=*reinterpret_cast<const std::uintptr_t*>(w+0x18);if(!handle)return 5;
        const auto object=*reinterpret_cast<const std::uintptr_t*>(handle+8);if(!object)return 6;
        if(*reinterpret_cast<const std::uintptr_t*>(object+0xD0)!=handle)return 7;
        const auto vt=*reinterpret_cast<const std::uintptr_t*>(object);if(!vt)return 8;
        if(*reinterpret_cast<const std::uintptr_t*>(vt+0x198)!=b+0x1C51240)return 9;
        return 0;
    }__except(EXCEPTION_EXECUTE_HANDLER){return 10;}
}
void NoteHudKeyRefusalR112(const char* name,void* wrapper,void* hud) noexcept {
    static std::uint64_t loggedGeneration=0;static unsigned loggedThisGeneration=0;
    const auto generation=HudGeneration();
    if(generation!=loggedGeneration){loggedGeneration=generation;loggedThisGeneration=0;}
    if(loggedThisGeneration>=8)return; // a few rows per HUD lifetime, never per frame
    ++loggedThisGeneration;unsigned flags=0;const int reason=HudKeyRefusalR112(wrapper,hud,flags);
    static constexpr const char* text[]{"ACCEPTED","NO_BASE","NO_CLIP","WRAPPER_VTABLE","WRAPPER_FLAGS","NO_HANDLE","NO_OBJECT","BACKLINK","NO_VTABLE","VISIBLE_SLOT","FAULT"};
    char m[260]{};std::snprintf(m,sizeof(m),"R112 HUD_CHILD_KEY clip=%s refused=%s flags=0x%X gen=%llu action=SCALE_WITH_VERIFIED_CLIP(native key not required)",
        name?name:"?",text[reason>=0&&reason<=10?reason:10],flags,(unsigned long long)generation);
    diagnostic_trace::Write(m);
}
bool ValidHudKey175(const ui_hud_visibility175::Key& key) noexcept {
    ui_hud_visibility175::Key current{};
    return ReadHudKey175(reinterpret_cast<void*>(key.wrapper),reinterpret_cast<void*>(key.hud),key.generation,current)&&
        ui_hud_visibility175::Same(current,key);
}
core::SnapshotMailbox<ui_hud_visibility175::Key> g_diceKeyR92;
std::atomic<bool> g_diceNativeVisibleR92{false};
bool DiceMarkerLeaseR92() noexcept {
    dice_marker_r92::Selection s{};dice_marker_r92::Presented p{},h{};
    return dice_marker_r92::flatOwnerReady.load()&&dice_marker_r92::selected.read(s)&&dice_marker_r92::prepared.read(p)&&
        dice_marker_r92::hidden.read(h)&&dice_marker_r92::ownsFlatCursor(s,h,GetTickCount64())&&
        dice_marker_r92::ownsFlatCursor(s,p,GetTickCount64());
}
bool DiceKeyR92(ui_hud_visibility175::Key& k) noexcept {
    return g_diceKeyR92.read(k)&&k.wrapper&&k.generation==HudGeneration()&&
        k.hud==reinterpret_cast<std::uintptr_t>(g_last_hud.load())&&ValidHudKey175(k);
}
void* __fastcall NativeFindHudChild175(void*self,const char*name,void*instance) {
    auto* result=visibility175::find(self,name,instance);
    if(!g_hud_repair_ready175.load(std::memory_order_acquire)||!result||
       self!=g_last_hud.load(std::memory_order_acquire))return result;
    char local[64]{};
    __try{if(name)std::strncpy(local,name,sizeof(local)-1);}__except(EXCEPTION_EXECUTE_HANDLER){return result;}
    int childIndex=-1,visibilityIndex=-1,notificationIndex=-1,promptIndex=-1,cursorIndex=-1,combatCursorR112=-1;
    constexpr const char* promptNamesD74[]{"Hints","TutorialMessage"};
    constexpr const char* promptPathsD74[]{"br.actionHints","tl.tutorial"};
    constexpr const char* cursorPathsD74[]{"cccursor.actioncursor.ActionCursorOutline","cccursor.actioncursor.ActionCursorCross"};
    for(unsigned i=0;i<2;++i){
        if(!std::strcmp(local,promptNamesD74[i])||!std::strcmp(local,promptPathsD74[i]))promptIndex=int(i);
        if(!std::strcmp(local,ui_prompt_scale120r3::ActionCursorD69::Names[i])||!std::strcmp(local,cursorPathsD74[i]))cursorIndex=int(i);
    }
    for(unsigned i=0;i<kNotificationNamesR12.size();++i)
        if(std::strcmp(local,kNotificationNamesR12[i])==0||std::strcmp(local,kNotificationPathsR12[i])==0){notificationIndex=int(i);break;}
    for(unsigned i=0;i<ui_hud_elements171::Names.size();++i)
        if(std::strcmp(local,ui_hud_elements171::Names[i])==0){childIndex=int(i);break;}
    for(unsigned i=0;i<ui_contextual_hud162r1::Clips.size();++i)
        if(ui_hud_visibility175::ExperimentGroup36(i,g_corrected36.load(),g_contextual36.load())&&
           std::strcmp(local,ui_contextual_hud162r1::Clips[i].name)==0){visibilityIndex=int(i);break;}
    if(std::strcmp(local,"bc.mc_StatsContainer")==0)childIndex=4;
    if(!std::strcmp(local,"CombatCursorCross")||!std::strcmp(local,"cccursor.actioncursor.CombatCursorCross"))combatCursorR112=0; // R112
    if(childIndex>=0||visibilityIndex>=0||notificationIndex>=0||promptIndex>=0||cursorIndex>=0||combatCursorR112>=0){
        ui_hud_visibility175::Key childKey{};
        if(ReadHudKey175(result,self,HudGeneration(),childKey)){
            if(childIndex>=0)g_children59c7.Observe(unsigned(childIndex),childKey);
            if(notificationIndex>=0)g_notification_children_r12.Observe(unsigned(notificationIndex),childKey);
            if(promptIndex>=0)g_prompt_children_d74.Observe(unsigned(promptIndex),childKey);
            if(cursorIndex>=0)g_action_cursor_children_d69.Observe(unsigned(cursorIndex),childKey);
            if(combatCursorR112>=0)g_combat_cursor_children_r112.Observe(0,childKey); // R112
            if(visibilityIndex>=0)g_native_visibility175.Bind(unsigned(visibilityIndex),childKey,g_corrected36.load(),g_contextual36.load());
        }
    }
    return result;
}
void __fastcall NativeDestroyHudWrapper175(void*self) {
    ui_hud_visibility175::Key diceKey{};
    if(g_diceKeyR92.read(diceKey)&&diceKey.wrapper==reinterpret_cast<std::uintptr_t>(self)){
        (void)g_diceKeyR92.publish({});dice_marker_r92::flatOwnerReady.store(false);
    }
    g_children59c7.Remove(reinterpret_cast<std::uintptr_t>(self));
    g_action_cursor_children_d69.Remove(reinterpret_cast<std::uintptr_t>(self));
    g_combat_cursor_children_r112.Remove(reinterpret_cast<std::uintptr_t>(self)); // R112
    g_prompt_children_d74.Remove(reinterpret_cast<std::uintptr_t>(self));
    g_notification_children_r12.Remove(reinterpret_cast<std::uintptr_t>(self));
    g_native_visibility175.RemoveWrapper(reinterpret_cast<std::uintptr_t>(self));
    visibility175::destroy(self);
}
// Same already-verified CMovieClip::SetDisplayInfo boundary used by our
// existing backend. Filter ONLY the alpha field of qualified semantic children;
// copy all position/scale/visibility/rotation fields unchanged to one original call.
// D59R1 also observes native scale writes to known notification identities, so
// the bounded worker can reapply after a same-child reset without a broad poll.
bool __fastcall NativeHudDisplay54(void*self,void* info) {
    if(g_hud_owned_write175||!g_hud_repair_ready175.load(std::memory_order_acquire)||!info)return visibility175::display54(self,info);
    // Native arguments are consumed synchronously under SEH, not a per-call
    // VirtualQuery or another Flash lookup while the movie lock may be held.
    alignas(16) unsigned char copy[0x38]{};
    __try{std::memcpy(copy,info,sizeof(copy));}__except(EXCEPTION_EXECUTE_HANDLER){return visibility175::display54(self,info);}
    std::uint16_t flags=0;std::memcpy(&flags,copy+0x34,2);
    ui_hud_visibility175::Key diceKey{};
    if((flags&0x400)&&DiceKeyR92(diceKey)&&diceKey.wrapper==reinterpret_cast<std::uintptr_t>(self)){
        g_diceNativeVisibleR92.store(copy[0x28]!=0);
        if(DiceMarkerLeaseR92()){copy[0x28]=0;return visibility175::display54(self,copy);}
    }
    if(!(flags&(0x20|0x18)))return visibility175::display54(self,info);
    // D60: a scale-only call on an unrelated Flash child has no notification
    // work to do. Avoid the native key read on that common animation path.
    if(!(flags&0x20)&&!g_notification_children_r12.HasWrapper(reinterpret_cast<std::uintptr_t>(self))&&
       !g_prompt_children_d74.HasWrapper(reinterpret_cast<std::uintptr_t>(self))&&
       !g_action_cursor_children_d69.HasWrapper(reinterpret_cast<std::uintptr_t>(self))&&
       !g_combat_cursor_children_r112.HasWrapper(reinterpret_cast<std::uintptr_t>(self))) // R112
        return visibility175::display54(self,info);
    ui_hud_visibility175::Key key{};
    if(!ReadHudKey175(self,g_last_hud.load(std::memory_order_acquire),HudGeneration(),key))return visibility175::display54(self,info);
    void* supplied=info;
    if(flags&0x20){
        float alpha=0;std::memcpy(&alpha,copy+0x14,4);
        const unsigned owner=g_semantic_owner54.load(std::memory_order_acquire);
        const auto decision=g_native_visibility175.NativeAlphaRequest54(key,alpha,g_transition_hidden170.load(std::memory_order_acquire),
            g_hud_repair175.load(std::memory_order_acquire),(owner&1)!=0,(owner&2)!=0,(owner&4)!=0,ValidHudKey175,(owner&8)!=0);
        if(decision.qualified&&decision.value!=alpha){std::memcpy(copy+0x14,&decision.value,4);supplied=copy;}
    }
    const bool result=visibility175::display54(self,supplied); // exactly once
    // Publish AFTER the original write. Otherwise the worker could consume the
    // retry epoch before the native reset actually happens and miss that reset.
    if(flags&0x18){
        (void)g_notification_children_r12.NativeScaleWrite(key);
        (void)g_prompt_children_d74.NativeScaleWrite(key);
        (void)g_action_cursor_children_d69.NativeScaleWrite(key);
        (void)g_combat_cursor_children_r112.NativeScaleWrite(key); // R112: native scale reset -> next service re-applies
    }
    return result;
}
void __fastcall NativeHudVisible175(void*self,bool requested) {
    bool target=requested;
    if(!g_hud_owned_write175&&g_hud_repair_ready175.load(std::memory_order_acquire)){
        const auto decision=g_native_visibility175.NativeRequest(reinterpret_cast<std::uintptr_t>(self),requested,
            reinterpret_cast<std::uintptr_t>(g_last_hud.load(std::memory_order_acquire)),HudGeneration(),
            g_transition_hidden170.load(std::memory_order_acquire),g_hud_repair175.load(std::memory_order_acquire),ValidHudKey175,g_corrected36.load(),g_contextual36.load());
        target=decision.visible;
        // Explicit ordinary/palm-gameplay vital display, not permission to reveal an
        // entire native HUD/menu. The native request is still retained for exit.
        if(decision.qualified&&g_hud_repair175.load(std::memory_order_acquire)&&decision.index==1&&g_vitals_pin59c7.load(std::memory_order_acquire)&&
           !(g_transition_hidden170.load(std::memory_order_acquire)&
             (ui_contextual_hud162r1::HideLower163|ui_contextual_hud162r1::VitalsGroupD84)))target=true;
        const unsigned owner54=g_semantic_owner54.load(std::memory_order_acquire);
        const unsigned hidden54=g_transition_hidden170.load(std::memory_order_acquire);
        if(decision.qualified&&g_hud_repair175.load(std::memory_order_acquire)&&((decision.index==0&&(owner54&2)&&!(hidden54&(ui_contextual_hud162r1::CompassGroupD84|ui_contextual_hud162r1::HideCompass163)))||
           (decision.index==19&&(owner54&8)&&!(hidden54&ui_contextual_hud162r1::HideCrosshair163))))target=true;
        if(HudCaptureActive175()){
            ++g_native_vis_calls175;if(decision.qualified)++g_native_vis_qualified175;
            if(decision.suppressed)++g_native_vis_suppressed175;
        }
    }
    ui_hud_visibility175::Key diceKey{};
    if(!g_hud_owned_write175&&DiceKeyR92(diceKey)&&diceKey.object==reinterpret_cast<std::uintptr_t>(self)){
        g_diceNativeVisibleR92.store(requested);if(DiceMarkerLeaseR92())target=false;
    }
    visibility175::visible(self,target); // exactly one original call; native content/animation continues
}
template<class Fn> bool InstallVisibilityHook175(unsigned slot,std::uintptr_t rva,Fn detour,Fn&original,const char*name) {
    const auto b=g_hud_native_base175.load();
    DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
    DetourModKit::hook::InlineRequest request{name,DetourModKit::hook::Target{DetourModKit::Address{b+rva}},options};
    auto hook=DetourModKit::hook::inline_at(std::move(request),detour);if(!hook)return false;
    visibility175::hooks[slot]=std::move(*hook);original=visibility175::hooks[slot]->original<Fn>();
    return original&&bool(visibility175::hooks[slot]->enable());
}
void RemoveHudRepairHooks175() noexcept {
    g_hud_repair175.store(false,std::memory_order_release);g_hud_repair_ready175.store(false,std::memory_order_release);
    for(auto&hook:visibility175::hooks)if(hook){(void)hook->disable();hook.reset();}
    g_native_visibility175.Invalidate();
}
bool VerifyHudWriter175(std::uintptr_t b) noexcept {
    __try {
        constexpr unsigned char finder[]={0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x20};
        constexpr unsigned char visible[]={0x44,0x8b,0x81,0xe8,0x00,0x00,0x00,0x41,0x8b,0xc0,0x83,0xe0,0xfe,0x41,0x83,0xc8,0x01};
        return VerifyHudDisplay170(b)&&std::memcmp(reinterpret_cast<void*>(b+0x7F4C18),finder,sizeof(finder))==0&&
            std::memcmp(reinterpret_cast<void*>(b+0x7F5FA0),finder,sizeof(finder))==0&&
            std::memcmp(reinterpret_cast<void*>(b+0x1C51240),visible,sizeof(visible))==0&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x4049BD0+0x238)==b+0x7F4C18&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3DA0440+0x198)==b+0x1C51240;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool InstallHudRepair175() noexcept {
    if(g_hud_repair_ready175.load(std::memory_order_acquire))return true;
    const auto b=WhBase();
    if(!SupportedBuild(b)||!retail_build112::Verify(b)||!VerifyHudWriter175(b))return false;
    g_hud_native_base175.store(b,std::memory_order_release);
    if(!InstallVisibilityHook175(0,0x7F5FA0,&NativeDestroyHudWrapper175,visibility175::destroy,"kcd2vr_hud_wrapper_destroy175")||
       !InstallVisibilityHook175(1,0x7F4C18,&NativeFindHudChild175,visibility175::find,"kcd2vr_hud_child_resolve175")||
       !InstallVisibilityHook175(2,0x1C51240,&NativeHudVisible175,visibility175::visible,"kcd2vr_hud_native_visible175")||
       !InstallVisibilityHook175(3,0x7F4520,&NativeHudDisplay54,visibility175::display54,"kcd2vr_hud_native_display54")){
        RemoveHudRepairHooks175();return false;
    }
    g_hud_repair_ready175.store(true,std::memory_order_release);return true;
}


bool ApplyMovieClipDisplay(void* hud,const char* name,float x,float y,float scale) noexcept {
    if(!hud || !name || !*name || !SafePtr(hud)) return false;
    void** hvt=nullptr;
    if(!Read(hud,hvt)||!hvt||!SafePtr(hvt)) return false;
    void* findTarget=nullptr;
    if(!Read(reinterpret_cast<unsigned char*>(hvt)+0x238,findTarget)||!ExecPtr(findTarget)) return false;
    using FindChildFn=void*(__fastcall*)(void*,const char*,int);
    void* clip=nullptr;
    __try { clip=reinterpret_cast<FindChildFn>(findTarget)(hud,name,0); }
    __except(EXCEPTION_EXECUTE_HANDLER) { clip=nullptr; }
    if(!clip||!SafePtr(clip)) return false;

    void** cvt=nullptr;
    if(!Read(clip,cvt)||!cvt||!SafePtr(cvt)) return false;
    void* setTarget=nullptr; void* getTarget=nullptr;
    if(!Read(reinterpret_cast<unsigned char*>(cvt)+0xC8,setTarget) || !ExecPtr(setTarget)) return false;
    if(!Read(reinterpret_cast<unsigned char*>(cvt)+0xD0,getTarget) || !ExecPtr(getTarget)) return false;

    // Recovered from WHGame+0x89B4D0 and +0x9A85D4:
    // SFlashDisplayInfo: x @ +0x00, y @ +0x04,
    // xScale @ +0x0C, yScale @ +0x10, flags uint16 @ +0x34.
    // Flags: X=0x01, Y=0x02, XScale=0x08, YScale=0x10.
    // WHGame writes scale as a percentage (50..100), not a 0..1 fraction.
    alignas(16) unsigned char info[0x38]{};
    using DisplayFn=void(__fastcall*)(void*,void*);
    __try { reinterpret_cast<DisplayFn>(getTarget)(clip,info); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }

    const float pct=std::clamp(scale,0.25f,2.00f)*100.0f;
    std::memcpy(info+0x00,&x,sizeof(float));
    std::memcpy(info+0x04,&y,sizeof(float));
    std::memcpy(info+0x0C,&pct,sizeof(float));
    std::memcpy(info+0x10,&pct,sizeof(float));
    std::uint16_t flags=0;
    std::memcpy(&flags,info+0x34,sizeof(flags));
    flags=static_cast<std::uint16_t>(flags|0x01u|0x02u|0x08u|0x10u);
    std::memcpy(info+0x34,&flags,sizeof(flags));
    ++g_hud_owned_write175;bool ok=true;
    __try { reinterpret_cast<DisplayFn>(setTarget)(clip,info); }
    __except(EXCEPTION_EXECUTE_HANDLER) { ok=false; }
    --g_hud_owned_write175;return ok;
}

bool CallFlashDisplayFnSEH(void* fn,void* clip,unsigned char* info) noexcept {
    // V64.1R: pulled out of ApplyAutoVrLayout - that function declares a Guard
    // object with a real destructor (RAII busy-flag release), and MSVC (C2712)
    // refuses to mix __try/__except with C++ object unwinding in the same
    // function. Same fix used elsewhere in this project: keep every raw
    // SEH-guarded pointer call in its own tiny leaf helper with nothing else
    // in scope. Covers the getter, setter, and post-write verify readback -
    // all three share this exact signature.
    using DisplayFn=void(__fastcall*)(void*,void*);
    const bool owned=reinterpret_cast<std::uintptr_t>(fn)==WhBase()+0x7F4520;
    if(owned)++g_hud_owned_write175;
    bool ok=true;
    __try { reinterpret_cast<DisplayFn>(fn)(clip,info); }
    __except(EXCEPTION_EXECUTE_HANDLER) { ok=false; }
    if(owned)--g_hud_owned_write175;return ok;
}

} // namespace


namespace {
std::atomic<void*> g_panel_live[2]{};
std::atomic<std::uint64_t> g_panel_life[2]{};
ui_context_panel::Transaction g_panel_fit[2];
std::atomic<std::uint64_t> g_panel_epoch{0};
struct ContextPanelBackend {
    void* element=nullptr;unsigned index=0;std::uint64_t generation=0;
    struct Handle{void*clip=nullptr;void*get=nullptr;void*set=nullptr;};Handle h[2]{};
    bool Alive()const noexcept{return g_panel_live[index].load()==element&&g_panel_life[index].load()==generation;}
    bool Resolve(std::size_t i,const char*name,std::uintptr_t& identity)noexcept {
        void**vt=nullptr;void*target=nullptr;const auto base=WhBase();
        if(i>=2||!Alive()||!Read(element,vt)||reinterpret_cast<std::uintptr_t>(vt)!=base+0x4049BD0ull||
           !Read(reinterpret_cast<unsigned char*>(vt)+0x238,target)||reinterpret_cast<std::uintptr_t>(target)!=base+0x7F4C18ull||
           !FindMovieClip(element,name,h[i].clip,h[i].get,h[i].set))return false;
        identity=reinterpret_cast<std::uintptr_t>(h[i].clip);return true;
    }
    bool Get(std::size_t i,ui_native_canvas::Geometry&value)noexcept {
        alignas(16) std::array<unsigned char,0x38> bytes{};
        if(!Alive()||!CallFlashDisplayFnSEH(h[i].get,h[i].clip,bytes.data()))return false;
        value=ui_native_canvas::Unpack(bytes);return Alive();
    }
    bool Set(std::size_t i,ui_native_canvas::Geometry value)noexcept {
        alignas(16)auto bytes=ui_native_canvas::Pack(value);
        return Alive()&&CallFlashDisplayFnSEH(h[i].set,h[i].clip,bytes.data())&&Alive();
    }
};
std::atomic<void*> g_pause_menu_live{nullptr};
std::atomic<std::uint64_t> g_pause_lifetime{0};
ui_pause_overlay::Transaction g_pause_fit;

// WHGame+1F8DB80 is a void ShowPage wrapper: one RCX UI wrapper; it
// constructs "ShowPage", calls 566BCC and destroys the temporary string. All
// 11 direct callers discard RAX; eight tail paths show pages, not frame updates.
// 566BE2 reads wrapper+48 as the IUIElement. See native verifier/report.
void __fastcall ShowPageDetour19(void* self) {
    const auto native=g_page_original19;if(!native)return;
    const bool observed=ui_pause_page59c19::Observe(reinterpret_cast<std::uintptr_t>(self),
        [](std::uintptr_t address,std::uintptr_t& value){return Read(reinterpret_cast<const void*>(address),value);},
        []{return ui_pause_page59c19::Owner{
            reinterpret_cast<std::uintptr_t>(g_pause_menu_live.load(std::memory_order_acquire)),
            g_pause_lifetime.load(std::memory_order_acquire)};},
        [&]{native(self);});
    if(observed)g_menu_page_events19.fetch_add(1,std::memory_order_relaxed);
}
bool PagePins19(std::uintptr_t b) noexcept {
    constexpr unsigned char entry[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0x48,0x8D,0x15,0xB0,0x26,0xF6,0x01};
    constexpr unsigned char invoke[]{0xE8,0x25,0x90,0x5D,0xFE};
    constexpr unsigned char element[]{0x48,0x8B,0x5B,0x48};
    std::array<unsigned char,sizeof(entry)> a{};std::array<unsigned char,sizeof(invoke)> c{};
    std::array<unsigned char,sizeof(element)> e{};std::array<char,9> name{};
    return Read(reinterpret_cast<const void*>(b+0x1F8DB80),a)&&!std::memcmp(a.data(),entry,sizeof(entry))&&
        Read(reinterpret_cast<const void*>(b+0x1F8DBA2),c)&&!std::memcmp(c.data(),invoke,sizeof(invoke))&&
        Read(reinterpret_cast<const void*>(b+0x566BE2),e)&&!std::memcmp(e.data(),element,sizeof(element))&&
        Read(reinterpret_cast<const void*>(b+0x3EF0240),name)&&!std::memcmp(name.data(),"ShowPage",9);
}
bool InstallPageObserver19() noexcept {
    if(g_page_hook19)return g_page_original19!=nullptr;
    const auto b=WhBase();if(!retail_build112::Ready(b)||!PagePins19(b))return false;
    DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
    auto hook=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{
        "c10r19_owned_menu_show_page",DetourModKit::hook::Target{DetourModKit::Address{b+0x1F8DB80}},options},&ShowPageDetour19);
    if(!hook)return false;g_page_hook19=std::move(*hook);g_page_original19=g_page_hook19->original<ShowPageFn19>();
    if(!g_page_original19){g_page_hook19.reset();return false;}
    if(!g_page_hook19->enable()){
        if(g_page_hook19->disable()){g_page_hook19.reset();g_page_original19=nullptr;}
        return false; // failed disarm retains handle AND original
    }
    return true;
}
void RemovePageObserver19() noexcept {
    if(g_page_hook19){
        if(!g_page_hook19->disable())return;
        g_page_hook19.reset();g_page_original19=nullptr;
    }
}

void* g_pause_background_owner36=nullptr;
bool g_pause_background_known36=false,g_pause_background_base36=true,g_pause_background_hidden36=false;
struct PauseOverlayBackend {
    void* element=nullptr;std::uint64_t generation=0;
    bool Alive()const noexcept{return element&&element==g_pause_menu_live.load()&&generation==g_pause_lifetime.load();}
    struct Handle{void*clip=nullptr;void*get=nullptr;void*set=nullptr;};
    std::array<Handle,ui_pause_overlay::Anchors.size()> h{};
    bool Resolve(std::size_t i,const char*name,std::uintptr_t& identity)noexcept {
        void** vt=nullptr;void* target=nullptr;
        const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
        if(i>=h.size()||!Alive()||!retail_build112::Ready(base)||!Read(element,vt)||!vt||
           !Read(reinterpret_cast<unsigned char*>(vt)+0x238,target)||reinterpret_cast<std::uintptr_t>(target)!=base+0x7F4C18ull||
           !FindMovieClip(element,name,h[i].clip,h[i].get,h[i].set)||!Alive())return false;
        identity=reinterpret_cast<std::uintptr_t>(h[i].clip);return true;
    }
    bool Get(std::size_t i,ui_native_canvas::Geometry&value)noexcept {
        alignas(16) std::array<unsigned char,0x38> bytes{};
        if(i>=h.size()||!Alive()||!CallFlashDisplayFnSEH(h[i].get,h[i].clip,bytes.data())||!Alive())return false;
        value=ui_native_canvas::Unpack(bytes);return true;
    }
    bool Set(std::size_t i,ui_native_canvas::Geometry value)noexcept {
        if(i>=h.size()||!Alive())return false;
        alignas(16) auto bytes=ui_native_canvas::Pack(value);
        return CallFlashDisplayFnSEH(h[i].set,h[i].clip,bytes.data())&&Alive();
    }
};
std::atomic<void*> g_death_live{nullptr};
std::atomic<std::uint64_t> g_death_lifetime{0};
ui_death_overlay135r2::Transaction g_death_fit;
struct DeathOverlayBackend {
    void* element=nullptr;std::uint64_t generation=0;
    bool Alive()const noexcept{return element&&element==g_death_live.load()&&generation==g_death_lifetime.load();}
    struct Handle{void*clip=nullptr;void*get=nullptr;void*set=nullptr;};
    std::array<Handle,ui_death_overlay135r2::Anchors.size()> h{};
    bool Parent(ui_native_canvas::Geometry& value)noexcept {
        void* clip=nullptr;void* get=nullptr;void* set=nullptr;
        alignas(16) std::array<unsigned char,0x38> bytes{};
        if(!Alive()||!FindMovieClip(element,"gameOverScreen",clip,get,set)||
           !CallFlashDisplayFnSEH(get,clip,bytes.data())||!Alive())return false;
        value=ui_native_canvas::Unpack(bytes);return true;
    }
    bool Resolve(std::size_t i,const char*name,std::uintptr_t& identity)noexcept {
        void** vt=nullptr;void* target=nullptr;
        const auto base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
        if(i>=h.size()||!Alive()||!retail_build112::Ready(base)||!Read(element,vt)||!vt||
           !Read(reinterpret_cast<unsigned char*>(vt)+0x238,target)||reinterpret_cast<std::uintptr_t>(target)!=base+0x7F4C18ull||
           !FindMovieClip(element,name,h[i].clip,h[i].get,h[i].set)||!Alive())return false;
        identity=reinterpret_cast<std::uintptr_t>(h[i].clip);return true;
    }
    bool Get(std::size_t i,ui_native_canvas::Geometry&value)noexcept {
        alignas(16) std::array<unsigned char,0x38> bytes{};
        if(i>=h.size()||!Alive()||!CallFlashDisplayFnSEH(h[i].get,h[i].clip,bytes.data())||!Alive())return false;
        value=ui_native_canvas::Unpack(bytes);return true;
    }
    bool Set(std::size_t i,ui_native_canvas::Geometry value)noexcept {
        if(i>=h.size()||!Alive())return false;
        alignas(16) auto bytes=ui_native_canvas::Pack(value);
        return CallFlashDisplayFnSEH(h[i].set,h[i].clip,bytes.data())&&Alive();
    }
};
bool SetElementChildVisible(void* element,const char*name,bool visible) noexcept {
    void*clip=nullptr;void*get=nullptr;void*set=nullptr;
    if(!element||!FindMovieClip(element,name,clip,get,set))return false;
    void**cvt=nullptr;void*visTarget=nullptr;
    if(!Read(clip,cvt)||!cvt||!SafePtr(cvt)||!Read(reinterpret_cast<unsigned char*>(cvt)+0x108,visTarget)||!ExecPtr(visTarget))return false;
    using SetVisibleFn=void(__fastcall*)(void*,bool);
    __try{reinterpret_cast<SetVisibleFn>(visTarget)(clip,visible);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}

// Handles exist only while this retained HUD transaction is alive. No persistent
// pointer cache is justified by a generation alone.
static std::atomic<std::uint64_t> g_canvas_resolve_us178r27{0},g_canvas_get_us178r27{0},g_canvas_set_us178r27{0},g_canvas_resolve_hits178r27{0},g_canvas_resolve_misses178r27{0};
// P1: optional phase timing follows the existing finite F2/HUD recorder lease.
// OFF performs no clock read/counter write. Expired/replaced sessions cannot
// receive a partly measured transaction. Native work and its result are unchanged.
struct CanvasPhaseCostP1 {
    hookcost178r22::Lease capture=hookcost178r22::lease();
    std::chrono::steady_clock::time_point begin{};
    CanvasPhaseCostP1() noexcept {if(capture.serial)begin=std::chrono::steady_clock::now();}
    bool Record(std::atomic<std::uint64_t>& counter) const noexcept {
        if(!capture.serial||!capture.current())return false;
        const auto us=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-begin).count();
        counter.fetch_add(std::uint64_t(us),std::memory_order_relaxed);return true;
    }
};
struct NativeCanvasBackend {
    void* hud=nullptr;
    std::uint64_t generation=0;
    struct Access {void* clip=nullptr;void* get=nullptr;void* set=nullptr;const char* name=nullptr;};
    std::array<Access,ui_native_canvas::Anchors.size()> handles{};
    std::uint64_t ElementRevision59c7(unsigned i) const noexcept{return g_children59c7.Revision(i);}
    bool Alive() const noexcept {return generation==g_canvas_lifetime.load(std::memory_order_acquire)&&hud==g_canvas_live_hud.load(std::memory_order_acquire);}
    bool Resolve(std::size_t i,const char* name,std::uintptr_t& identity) noexcept {
        auto&h=handles[i];
        // New raw-path use is limited to the exact retail element/getter shape.
        void** vt=nullptr;void* target=nullptr;const auto base=WhBase();const CanvasPhaseCostP1 phase;
        if(!Alive()||!Read(hud,vt)||reinterpret_cast<std::uintptr_t>(vt)!=base+0x4049BD0ull||
            !Read(reinterpret_cast<unsigned char*>(vt)+0x238,target)||reinterpret_cast<std::uintptr_t>(target)!=base+0x7F4C18ull)return false;
        // A repeated lookup within the SAME retained transaction is reusable.
        const bool reuse=h.clip&&h.name&&std::strcmp(h.name,name)==0;
        if(!reuse&&!FindMovieClip(hud,name,h.clip,h.get,h.set))return false;
        h.name=name;
        if(phase.Record(g_canvas_resolve_us178r27)){
            (reuse?g_canvas_resolve_hits178r27:g_canvas_resolve_misses178r27).fetch_add(1,std::memory_order_relaxed);
        }
        std::uintptr_t wrapper=0;
        if(!Read(h.clip,wrapper)||wrapper!=base+0x3A55BA0||
           reinterpret_cast<std::uintptr_t>(h.get)!=base+0x7F4784||reinterpret_cast<std::uintptr_t>(h.set)!=base+0x7F4520)return false;
        identity=reinterpret_cast<std::uintptr_t>(h.clip);return true;
    }
    bool Get(std::size_t i,ui_native_canvas::Geometry& out) noexcept {
        alignas(16) std::array<unsigned char,0x38> bytes{};
        const auto&h=handles[i];const CanvasPhaseCostP1 phase;
        const bool ok=Alive()&&CallFlashDisplayFnSEH(h.get,h.clip,bytes.data());
        (void)phase.Record(g_canvas_get_us178r27);
        if(!ok)return false;out=ui_native_canvas::Unpack(bytes);return Alive();
    }
    bool Set(std::size_t i,ui_native_canvas::Geometry value) noexcept {
        alignas(16) auto bytes=ui_native_canvas::Pack(value);
        const auto&h=handles[i];const CanvasPhaseCostP1 phase;
        const bool ok=Alive()&&CallFlashDisplayFnSEH(h.set,h.clip,bytes.data())&&Alive();
        (void)phase.Record(g_canvas_set_us178r27);return ok;
    }
};
}

bool NativeCanvasActive() noexcept {return g_canvas.Active();}
void CanvasPhaseCost178r27(double& resolveMs,double& getMs,double& setMs,std::uint64_t& hits,std::uint64_t& misses) noexcept { // V66.178R27: exchange-and-reset
    resolveMs=double(g_canvas_resolve_us178r27.exchange(0))/1000.0;getMs=double(g_canvas_get_us178r27.exchange(0))/1000.0;setMs=double(g_canvas_set_us178r27.exchange(0))/1000.0;hits=g_canvas_resolve_hits178r27.exchange(0);misses=g_canvas_resolve_misses178r27.exchange(0);}
ui_native_canvas::Report NativeCanvasReport() noexcept {return g_canvas_report;}
void WriteNativeCanvasSnapshot() noexcept {
    WriteHudCoverage168r1("SNAPSHOT");
    // UI-worker hotkey only: no Flash calls, no GPU readback, no native scans.
    const auto mask=g_canvas.ObservedMask();
    for(std::size_t i=0;i<ui_native_canvas::Anchors.size();++i){
        const auto&a=g_canvas.Observed()[i];const auto&b=g_canvas.Targets()[i];char line[288]{};
        std::snprintf(line,sizeof(line),"V66.90-HUD-ANCHOR cached=1 name=%s read=%d before=(%.2f,%.2f;%.3f,%.3f) requested=(%.2f,%.2f;%.3f,%.3f)",
            ui_native_canvas::Anchors[i],(mask&(1u<<i))?1:0,double(a.x),double(a.y),double(a.xs),double(a.ys),double(b.x),double(b.y),double(b.xs),double(b.ys));
        diagnostic_trace::Write(line);
    }
}

void NotifyHudDestroyed(void* element) noexcept {
    for(unsigned i=0;i<2;++i){void* expectedActivity=element;
        if(g_activity_element_s1[i].compare_exchange_strong(expectedActivity,nullptr))
            g_activity_generation_s1[i].fetch_add(1,std::memory_order_acq_rel);}
    void* waitExpected=element;if(g_wait_element_b2.compare_exchange_strong(waitExpected,nullptr))g_wait_generation_b2.fetch_add(1);
    void* helmetExpected=element;if(g_helmet_element63.compare_exchange_strong(helmetExpected,nullptr))g_helmet_generation63.fetch_add(1);
    void* deathExpected=element;if(g_death_live.compare_exchange_strong(deathExpected,nullptr))g_death_lifetime.fetch_add(1);
    void* pauseExpected=element;if(g_pause_menu_live.compare_exchange_strong(pauseExpected,nullptr))g_pause_lifetime.fetch_add(1);
    for(unsigned i=0;i<2;++i){void*expected=element;
        if(g_panel_live[i].compare_exchange_strong(expected,nullptr)){g_panel_life[i].fetch_add(1);g_panel_epoch.fetch_add(1);}}

    // No UI calls, logging, cache writes or dereference after destruction.
    void* expected=element;
    const bool observed=g_last_hud.compare_exchange_strong(expected,nullptr,std::memory_order_acq_rel);
    expected=element;
    const bool canvas=g_canvas_live_hud.compare_exchange_strong(expected,nullptr,std::memory_order_acq_rel);
    if(canvas)g_canvas_lifetime.fetch_add(1,std::memory_order_acq_rel);
    if(observed)g_last_hud_tick.store(0,std::memory_order_release);
    if(observed||canvas){g_hud_generation.fetch_add(1,std::memory_order_acq_rel);g_native_visibility175.Invalidate();}
}
namespace {
bool VerifyHudDisplay170(std::uintptr_t b) noexcept {
    __try {
        constexpr unsigned char setter[]={0x48,0x8b,0xc4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18};
        constexpr unsigned char getter[]={0x48,0x8b,0xc4,0x48,0x89,0x58,0x18,0x55,0x56,0x57};
        return std::memcmp(reinterpret_cast<void*>(b+0x7F4520),setter,sizeof(setter))==0&&
           std::memcmp(reinterpret_cast<void*>(b+0x7F4784),getter,sizeof(getter))==0&&
           *reinterpret_cast<const std::uintptr_t*>(b+0x3A55BA0+0xc8)==b+0x7F4520&&
           *reinterpret_cast<const std::uintptr_t*>(b+0x3A55BA0+0xd0)==b+0x7F4784;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
struct HudVisibilityBackend170 {
    HudHolder holder{};void* hud=nullptr;
    struct Handle {void* clip=nullptr;void* get=nullptr;void* set=nullptr;};
    std::array<Handle,ui_contextual_hud162r1::Clips.size()> handle{};
    std::array<bool,ui_contextual_hud162r1::Clips.size()> nativeRestricted175{};
    LARGE_INTEGER begin171{},end171{};
    bool Acquire(){if(HudCaptureActive175())QueryPerformanceCounter(&begin171);return g_display_verified170&&AcquireHudDirect(holder,hud);}
    std::uint64_t Generation(){return HudGeneration();}
    void Release(){ReleaseHudHolder(holder);if(begin171.QuadPart&&!end171.QuadPart)QueryPerformanceCounter(&end171);}
    double ElapsedUs171(){if(!begin171.QuadPart)return 0;Release();LARGE_INTEGER frequency{};
        return QueryPerformanceFrequency(&frequency)&&frequency.QuadPart?double(end171.QuadPart-begin171.QuadPart)*1000000.0/double(frequency.QuadPart):0;}
    ~HudVisibilityBackend170(){Release();}
    bool Resolve(unsigned i,const char*name,std::uintptr_t&id){auto&h=handle[i];std::uintptr_t vt=0;const auto b=WhBase();
        if(!FindMovieClip(hud,name,h.clip,h.get,h.set)||!Read(h.clip,vt)||vt!=b+0x3A55BA0||
           reinterpret_cast<std::uintptr_t>(h.get)!=b+0x7F4784||reinterpret_cast<std::uintptr_t>(h.set)!=b+0x7F4520)return false;
        id=reinterpret_cast<std::uintptr_t>(h.clip);return true;}
    static bool Call(void*fn,void*clip,void*info) noexcept {
        __try{return reinterpret_cast<bool(__fastcall*)(void*,void*)>(fn)(clip,info);}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    }
    bool Get(unsigned i,bool&visible){alignas(16) unsigned char info[0x38]{};
        if(!Call(handle[i].get,handle[i].clip,info))return false;
        std::uint16_t flags=0;std::memcpy(&flags,info+0x34,2);if(!(flags&0x400))return false;
        visible=info[0x28]!=0;return true;}
    bool NativeVisibility175(unsigned i,std::uintptr_t id,bool current,bool&native){
        if(!g_hud_repair_ready175.load(std::memory_order_acquire))return false;
        ui_hud_visibility175::Key key{};
        if(!ReadHudKey175(reinterpret_cast<void*>(id),hud,Generation(),key)||!g_native_visibility175.Bind(i,key,g_corrected36.load(),g_contextual36.load()))return false;
        return g_native_visibility175.NativeMeaning(i,key,current,native,&nativeRestricted175[i]);
    }
    bool NativeRestriction175(unsigned i)const{return nativeRestricted175[i];}
    bool Set(unsigned i,bool visible){alignas(16) unsigned char info[0x38]{};
        info[0x28]=visible?1:0;const std::uint16_t flags=0x400;std::memcpy(info+0x34,&flags,2);
        ++g_hud_owned_write175;const bool ok=Call(handle[i].set,handle[i].clip,info);--g_hud_owned_write175;return ok;}
};
}

namespace {
ui_palm_alpha44c2::Controller g_palm_alpha44c2;
struct PalmAlphaBackend44c2 {
    HudVisibilityBackend170 io;
    bool Acquire(){return io.Acquire();}
    void Release(){io.Release();}
    bool Resolve(unsigned i,const char* n){std::uintptr_t id=0;return io.Resolve(i,n,id);}
    bool Read(unsigned i,float& alpha){alignas(16) unsigned char data[0x38]{};
        if(!io.Call(io.handle[i].get,io.handle[i].clip,data))return false;
        std::uint16_t flags=0;std::memcpy(&flags,data+0x34,2);if(!(flags&0x20))return false;
        std::memcpy(&alpha,data+0x14,4);return std::isfinite(alpha)&&alpha>=0&&alpha<=100;}
    bool NativeAlpha54(unsigned i,float current,float& native){
        static constexpr unsigned indices[]{0,1,2,5,6,7};if(i>=6)return false;
        ui_hud_visibility175::Key key{};const auto index=indices[i];
        if(!ReadHudKey175(io.handle[i].clip,io.hud,io.Generation(),key)||
           !g_native_visibility175.Bind(index,key,g_corrected36.load(),g_contextual36.load()))return false;
        return g_native_visibility175.NativeAlphaMeaning54(index,key,current,native);
    }
    bool Write(unsigned i,float alpha){alignas(16) unsigned char data[0x38]{};
        const std::uint16_t flags=0x20;std::memcpy(data+0x34,&flags,2);std::memcpy(data+0x14,&alpha,4);
        ++g_hud_owned_write175;const bool ok=io.Call(io.handle[i].set,io.handle[i].clip,data);--g_hud_owned_write175;return ok;}
};
}
// Bounded snapshots on actual HUD permission/lifetime changes, never a frame
// recorder. Capture every ancestor of Stats: visible alone misses alpha=0 or
// an off-screen parent, which made the previous 'palm reveal' evidence ambiguous.
void TraceHudVisibility57c1(bool changed=true) noexcept {
    using namespace diagnostics::capture168;
    const auto now=GetTickCount64();const auto session=gate.session[unsigned(Domain::Hud)].load();
    static ui_hud_work59c7::CaptureBudget budget;
    if(!budget.Take(active(Domain::Hud,now),session,changed))return;
    HudHolder holder{};void* hud=nullptr;
    if(!g_display_verified170||!AcquireHudDirect(holder,hud)){ReleaseHudHolder(holder);return;}
    constexpr const char* names[]{"bc","bc.mc_StatsContainer","Stats","HealthBar","StaminaBar",
        "QAMWeapon","QAMFood","Cursor","CursorCross","CursorOutline","CombatCursorCross"};
    for(const auto* name:names){
        if(!active(Domain::Hud,GetTickCount64())||gate.session[unsigned(Domain::Hud)].load()!=session)break;
        void* clip=nullptr;void* get=nullptr;void* set=nullptr;alignas(16) unsigned char data[0x38]{};
        const bool ok=FindMovieClip(hud,name,clip,get,set)&&HudVisibilityBackend170::Call(get,clip,data);
        float x=0,y=0,xs=0,ys=0,a=0;std::uint16_t flags=0;
        if(ok){std::memcpy(&x,data,4);std::memcpy(&y,data+4,4);std::memcpy(&xs,data+12,4);std::memcpy(&ys,data+16,4);std::memcpy(&a,data+20,4);std::memcpy(&flags,data+52,2);}
        char m[420]{};std::snprintf(m,sizeof(m),"R59.16C7 HUD_VIS session=%llu snapshot=%u gen=%llu clip=%s read=%d flags=%X visible=%d alpha=%.2f xy=%.2f/%.2f scale=%.2f/%.2f hidden=%u vitalsPinned=%d",
            (unsigned long long)session,budget.Count(),(unsigned long long)HudGeneration(),name,ok,flags,ok&&(flags&0x400)?int(data[40]!=0):-1,double(a),double(x),double(y),double(xs),double(ys),g_transition_hidden170.load(),g_vitals_pin59c7.load());diagnostic_trace::Write(m);
    }
    ReleaseHudHolder(holder);
}

void SetHudSemanticOwner54(bool palm,bool left,bool right,bool gazeCursor) noexcept {
    const unsigned bits=(palm?1u:0u)|(palm&&left?2u:0u)|(palm&&right?4u:0u)|(gazeCursor?8u:0u);
    if(g_semantic_owner54.exchange(bits,std::memory_order_acq_rel)!=bits)++g_hud_repair_epoch175;
}
std::uint64_t HudAlphaGeneration54() noexcept {
    std::uint64_t key=1469598103934665603ull;ui_hud_work59c7::Mix(key,HudGeneration());
    ui_hud_work59c7::Mix(key,g_children59c7.Epoch());ui_hud_work59c7::Mix(key,g_native_visibility175.Epoch());
    ui_hud_work59c7::Mix(key,g_native_visibility175.Revision54());return key;
}
void UpdatePalmAlpha44c2(bool active,bool left,bool right,bool qam,std::uint64_t generation) noexcept {
    const ui_palm_alpha44c2::Input input{active,left,right,qam,generation,HudGeneration()};
    if(!g_palm_alpha44c2.Needs(input))return; // worker-owned state; no steady mutex or Flash calls
    std::lock_guard<std::recursive_mutex> lock(g_hud_draw_mutex162r1);
    PalmAlphaBackend44c2 io;
    const auto report=g_palm_alpha44c2.Update(io,input);
    if(report.changed){char m[256]{};std::snprintf(m,sizeof(m),"R44C2 PALM_ALPHA active=%d left=%d right=%d qam=%d reads=%u writes=%u failures=%u nativeVisible=UNTOUCHED steadyFlashCalls=0",active,left,right,qam,report.reads,report.writes,report.failures);diagnostic_trace::Write(m);}
}

void UpdateHudVisibility170(bool contextual,unsigned hidden,std::uint64_t lifecycle) noexcept {
    kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::HudVisibility};
    HudVisibilityBackend170 io{};
    g_transition_hidden170.store(hidden,std::memory_order_release);
    // The experiment/identity epoch is an explicit visibility event, not a HUD
    // layout or periodic native readback request.
    const auto repairEpoch=g_hud_repair_epoch175.load(std::memory_order_acquire);
    const auto registryEpoch=g_native_visibility175.Epoch();
    static std::uint64_t oldRepairEpoch=0,oldRegistryEpoch=0,oldRegistryRevision54=0,visibilityEpoch=0,oldLifecycle=~0ull,oldChildren=0;
    const auto registryRevision54=g_native_visibility175.Revision54();
    const auto children=g_children59c7.Epoch();
    if(oldRepairEpoch!=repairEpoch||oldRegistryEpoch!=registryEpoch||oldRegistryRevision54!=registryRevision54||oldLifecycle!=lifecycle||oldChildren!=children){
        ++visibilityEpoch;oldRepairEpoch=repairEpoch;oldRegistryEpoch=registryEpoch;oldRegistryRevision54=registryRevision54;oldLifecycle=lifecycle;oldChildren=children;
    }
    // D72: this experiment removes only unchanged worker/mutex work. Native
    // hide/reveal, lifecycle and restore transactions always keep their owner.
    static bool walkOffAppliedD70=false;
    const bool walkOffD70=kcd2vr::claude_d70::HudWalkoff();
    if(walkOffD70!=walkOffAppliedD70){
        walkOffAppliedD70=walkOffD70;++visibilityEpoch;
    }
    const ui_hud_transition170::Request request{contextual,hidden,visibilityEpoch,action_map_probe::WeaponQamOpen171(),action_map_probe::FoodQamOpen171(),g_corrected36.load(),g_vitals_pin59c7.load(),(g_semantic_owner54.load()&2)!=0,(g_semantic_owner54.load()&8)!=0};
    if(walkOffD70&&!g_visibility170.Needs(request,HudGeneration())&&!HudCaptureActive175())return;
    std::lock_guard<std::recursive_mutex> lock(g_hud_draw_mutex162r1);
    const auto report=g_visibility170.Update(io,request,HudGeneration());
    TraceHudVisibility57c1(report.changed); // fresh capture entry works even with unchanged gameplay
    if(!report.changed)return;
    if(!HudCaptureActive175())return;
    ++g_transition_events170;g_transition_reads170.fetch_add(report.reads);g_transition_writes170.fetch_add(report.writes);
    g_transition_failures170.fetch_add(report.failures);
    char m[384]{};std::snprintf(m,sizeof(m),"V66.175-HUD TRANSITION contextual=%d hidden=%u lifecycle=%llu reads=%u writes=%u failures=%u absent=%u costUs=%.1f qamWeapon=%d qamFood=%d steadyFlashCalls=0 renderHook=OFF",
        contextual,hidden,(unsigned long long)lifecycle,report.reads,report.writes,report.failures,report.absent,io.ElapsedUs171(),action_map_probe::WeaponQamOpen171(),action_map_probe::FoodQamOpen171());diagnostic_trace::Write(m);
}
void PublishHudVisibility163(bool crosshair,bool compass,bool lower,bool status) noexcept {
    using namespace ui_contextual_hud162r1;
    g_user_hidden163.store((crosshair?0u:HideCrosshair163)|(compass?0u:HideCompass163)|
        (lower?0u:HideLower163)|(status?0u:HideStatus163),std::memory_order_release);
}
void SetHudExperiment36(bool corrected,bool contextual) noexcept {
    const bool oldCorrected=g_corrected36.exchange(corrected);
    if(!corrected)g_vitals_pin59c7.store(false,std::memory_order_release);
    const bool oldContextual=g_contextual36.exchange(contextual);
    if(oldCorrected!=corrected||oldContextual!=contextual)++g_hud_repair_epoch175;
}
bool HudExperiment36Enabled() noexcept {return g_corrected36.load();}
bool SetHudRepair175(bool enabled) noexcept {
    if(enabled&&!InstallHudRepair175()){
        diagnostic_trace::Write("V66.175-HUD REPAIR state=UNAVAILABLE exactNativeWriterHooks=INCOMPLETE; one-shot HUD preferences remain available");return false;
    }
    if(g_hud_repair175.exchange(enabled,std::memory_order_acq_rel)!=enabled)++g_hud_repair_epoch175;
    diagnostic_trace::Write(enabled?
        "V66.175-HUD REPAIR state=ON scope=QUALIFIED_NATIVE_VISIBLE_WRITER layout=UNCHANGED immersive=USER_CHECKBOX_ONLY scriptTimelineCoverage=UNMEASURED":
        "V66.175-HUD REPAIR state=OFF nativeVisibilityRequest=PASSTHROUGH savedPreferences=PRESERVED");
    return true;
}
bool HudRepair175() noexcept{return g_hud_repair175.load(std::memory_order_acquire);}
void WriteHudCoverage168r1(const char* phase) noexcept {
    char m[640]{};std::snprintf(m,sizeof(m),"V66.175-HUD COVERAGE phase=%s transitions=%llu reads=%llu writes=%llu failures=%llu hidden=%u renderHook=OFF steadyFlashCalls=0 repair=%d writerReady=%d nativeWriterCalls=%llu qualifiedWrites=%llu suppressedNativeShows=%llu nativeVisibilityReassertion=QUALIFIED_GFX_VISIBLE_WRITER_ONLY scriptTimelineCoverage=UNMEASURED",
        phase,(unsigned long long)g_transition_events170.load(),(unsigned long long)g_transition_reads170.load(),
        (unsigned long long)g_transition_writes170.load(),(unsigned long long)g_transition_failures170.load(),g_transition_hidden170.load(),
        HudRepair175(),g_hud_repair_ready175.load(),(unsigned long long)g_native_vis_calls175.load(),
        (unsigned long long)g_native_vis_qualified175.load(),(unsigned long long)g_native_vis_suppressed175.load());diagnostic_trace::Write(m);
}
ui_prompt_scale120r3::Transaction g_prompt162r1;
ui_prompt_scale120r3::ActionCursorD69 g_action_cursor_d69;
ui_prompt_scale120r3::Notifications g_notifications_r12;

ui_hud_work59c7::ChildWork<ui_prompt_scale120r3::Notifications::Names.size()> g_notifications_work_p1;
bool g_notifications_requested_r12=false;
std::atomic<bool> g_notifications_need_service_d80{false};
// Caller holds the HUD mutex. Publish the summary with the request/ownership
// transition so an idle callback never takes the shared renderer lock.
void SetNotificationsRequestedD80(bool wanted) noexcept {
    g_notifications_requested_r12=wanted;
    g_notifications_need_service_d80.store(wanted||g_notifications_r12.Active(),std::memory_order_release);
}
// The existing worker service checks only copied epochs when idle. Native reads
// occur at most three times per child/HUD/request event; hooks only observe keys.
// A scale write wakes only that child; another child cannot renew its retry budget.
static bool ApplyNotificationScaleR12() noexcept {
    if(!g_notifications_need_service_d80.load(std::memory_order_acquire))return true;
    std::lock_guard<std::recursive_mutex> lock(g_hud_draw_mutex162r1);
    struct Summary {
        ~Summary(){g_notifications_need_service_d80.store(g_notifications_requested_r12||
            g_notifications_r12.Active(),std::memory_order_release);}
    } summary; // retain active restoration debt on every early return
    if(!g_notifications_requested_r12&&!g_notifications_r12.Active())return true;
    const auto keyFor=[&](std::uint64_t hudGeneration,std::uint64_t canvasLifetime){
        std::uint64_t key=1469598103934665603ull;
        ui_hud_work59c7::Mix(key,hudGeneration);ui_hud_work59c7::Mix(key,canvasLifetime);
        ui_hud_work59c7::Mix(key,g_notifications_requested_r12);return key;
    };
    const auto key=keyFor(HudGeneration(),g_canvas_lifetime.load());
    auto work=g_notifications_work_p1.Due(key,GetTickCount64(),g_notification_children_r12);
    if(!work)return g_notifications_work_p1.Ok();
    HudHolder holder{};void* hud=nullptr;
    if(!g_display_verified170||!AcquireHudDirect(holder,hud)){ReleaseHudHolder(holder);return false;}
    const auto previous=g_canvas_live_hud.exchange(hud,std::memory_order_acq_rel);
    if(previous!=hud)g_canvas_lifetime.fetch_add(1,std::memory_order_acq_rel);
    struct NotificationBackend : NativeCanvasBackend {
        bool Resolve(unsigned i,const char* name,std::uintptr_t& id) noexcept {
            if(!NativeCanvasBackend::Resolve(i,name,id))return false;
            ui_hud_visibility175::Key child{};
            if(!ReadHudKey175(handles[i].clip,hud,HudGeneration(),child))return false;
            g_notification_children_r12.Observe(i,child);id=child.object;return true;
        }
        bool Set(unsigned i,ui_native_canvas::Geometry value) noexcept {
            if(i>=handles.size()||!Alive())return false;
            alignas(16) auto bytes=ui_prompt_scale120r3::Notifications::PackScale(value);
            return CallFlashDisplayFnSEH(handles[i].set,handles[i].clip,bytes.data())&&Alive();
        }
        std::uint64_t Revision(unsigned i) const noexcept{return g_notification_children_r12.Revision(i);}
    }io;
    io.hud=hud;io.generation=g_canvas_lifetime.load(std::memory_order_acquire);
    work=g_notifications_work_p1.AfterAcquire(work,key,keyFor(HudGeneration(),io.generation),
        GetTickCount64(),g_notification_children_r12);
    (void)g_notifications_r12.Run(io,io.generation,g_notifications_requested_r12,work);
    const auto& report=g_notifications_r12.LastReport();
    g_notifications_work_p1.Complete(work,report.resolved,report.verified);
    // Existing finite HUD capture only; values came from this transaction.
    static ui_hud_work59c7::CaptureBudget notificationCaptureD59R1;
    const bool capture=HudCaptureActive175();
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Hud)].load();
    if(notificationCaptureD59R1.Take(capture,session,true)){
        for(unsigned i=0;i<kNotificationNamesR12.size();++i){if(!(work&(1u<<i)))continue;char line[360]{};
            std::snprintf(line,sizeof(line),"D59R1 HUD_NOTIFICATION name=%s resolved=%d written=%d verified=%d before=(%.3f,%.3f) target=(%.3f,%.3f) readback=(%.3f,%.3f) enabled=%d",
                kNotificationNamesR12[i],int((report.resolved>>i)&1),int((report.written>>i)&1),int((report.verified>>i)&1),
                double(report.before[i].xs),double(report.before[i].ys),double(report.target[i].xs),double(report.target[i].ys),
                double(report.after[i].xs),double(report.after[i].ys),g_notifications_requested_r12?1:0);
            diagnostic_trace::Write(line);
        }
    }
    ReleaseHudHolder(holder);return g_notifications_work_p1.Ok();
}

bool ApplyReadablePromptScale(bool enabled,float hints,float controls) noexcept {
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    enabled=prompt_policy_d80::movieLease.NativeEnabled(enabled);
    const auto leaseEpoch=prompt_policy_d80::movieLease.Epoch();
    if(!enabled&&!g_prompt162r1.Active()&&!g_action_cursor_d69.Active()){
        prompt_policy_d80::movieLease.NativeRelinquished(leaseEpoch);return true;
    }
    HudHolder holder{};void* hud=nullptr;
    if(!AcquireHudDirect(holder,hud)){ReleaseHudHolder(holder);return !enabled;}
    const auto previous=g_canvas_live_hud.exchange(hud,std::memory_order_acq_rel);
    if(previous!=hud)g_canvas_lifetime.fetch_add(1,std::memory_order_acq_rel);
    struct PromptBackendD74 : NativeCanvasBackend {
        bool Resolve(unsigned i,const char* name,std::uintptr_t& id) noexcept {
            if(i>=2||!NativeCanvasBackend::Resolve(i,name,id))return false;
            ui_hud_visibility175::Key child{};
            if(ReadHudKey175(handles[i].clip,hud,HudGeneration(),child)){g_prompt_children_d74.Observe(i,child);id=child.object;return true;}
            // R112: the clip passed the wrapper/getter/setter checks above (the ones every working HUD element
            // uses); a refused native key only loses the reset wake-up, not the size.
            NoteHudKeyRefusalR112(name,handles[i].clip,hud);id=reinterpret_cast<std::uintptr_t>(handles[i].clip);return true;
        }
        std::uint64_t Revision(unsigned i) const noexcept {return g_prompt_children_d74.Revision(i);}
        bool Set(unsigned i,ui_native_canvas::Geometry value) noexcept {
            if(i>=2||!Alive())return false;
            alignas(16) auto bytes=ui_prompt_scale120r3::Notifications::PackScale(value);
            return CallFlashDisplayFnSEH(handles[i].set,handles[i].clip,bytes.data())&&Alive();
        }
    }io;io.hud=hud;io.generation=g_canvas_lifetime.load(std::memory_order_acquire);
    const bool ok=g_prompt162r1.Run(io,io.generation,enabled,std::clamp(hints,.25f,1.5f),std::clamp(controls,.25f,1.5f));
    // Same retained HUD and settings/lifecycle event as the established prompt
    // owner. Only these semantic children's local scale is written; no timer,
    // visibility, whole-parent transform or new native call/hook is introduced.
    struct ActionCursorBackendD69 : NativeCanvasBackend {
        bool Resolve(unsigned i,const char* name,std::uintptr_t& id) noexcept {
            if(i>=ui_prompt_scale120r3::ActionCursorD69::Names.size()||!NativeCanvasBackend::Resolve(i,name,id))return false;
            ui_hud_visibility175::Key child{};
            if(ReadHudKey175(handles[i].clip,hud,HudGeneration(),child)){g_action_cursor_children_d69.Observe(i,child);id=child.object;return true;}
            NoteHudKeyRefusalR112(name,handles[i].clip,hud);id=reinterpret_cast<std::uintptr_t>(handles[i].clip);return true; // R112
        }
        std::uint64_t Revision(unsigned i) const noexcept {return g_action_cursor_children_d69.Revision(i);}
        bool Set(unsigned i,ui_native_canvas::Geometry value) noexcept {
            if(i>=ui_prompt_scale120r3::ActionCursorD69::Names.size()||!Alive())return false;
            alignas(16) auto bytes=ui_prompt_scale120r3::Notifications::PackScale(value);
            return CallFlashDisplayFnSEH(handles[i].set,handles[i].clip,bytes.data())&&Alive();
        }
    }cursor;
    cursor.hud=hud;cursor.generation=io.generation;
    const bool cursorOk=g_action_cursor_d69.Run(cursor,cursor.generation,enabled,std::clamp(controls,.25f,1.5f));
    if(!g_prompt162r1.Active()&&!g_action_cursor_d69.Active())
        prompt_policy_d80::movieLease.NativeRelinquished(leaseEpoch);
    ReleaseHudHolder(holder);return ok&&cursorOk;
}

// D74: the active WholeReadable owner uses this same semantic child service.
// Idle calls compare copied epochs/settings only; no native lookup or readback.
// A native child recreation/scale reset reopens three bounded attempts.
void UpdateReadablePromptsD74(bool enabled,bool station,float hints,float controls,std::uint64_t now) noexcept {
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    enabled=prompt_policy_d80::movieLease.NativeEnabled(enabled); // wake ordinary sizing only after movie restoration
    if(!g_prompt162r1.Active()&&!g_action_cursor_d69.Active())
        prompt_policy_d80::movieLease.NativeRelinquished(prompt_policy_d80::movieLease.Epoch());
    const auto scales=ui_station_prompts_d74::Choose(station,hints,controls);
    const auto keyFor=[&](){
        std::uint64_t key=1469598103934665603ull;
        for(auto value:{HudGeneration(),g_canvas_lifetime.load(),g_prompt_children_d74.Epoch(),g_action_cursor_children_d69.Epoch()})
            ui_hud_work59c7::Mix(key,value);
        ui_hud_work59c7::Mix(key,enabled);ui_hud_work59c7::Mix(key,station);
        for(float value:{scales.hints,scales.controls}){std::uint32_t bits=0;std::memcpy(&bits,&value,4);ui_hud_work59c7::Mix(key,bits);}
        return key;
    };
    static ui_hud_work59c7::Retry retry;
    const auto key=keyFor();if(!retry.Due(key,now))return;
    const bool ok=ApplyReadablePromptScale(enabled,scales.hints,scales.controls);
    const auto& hint=g_prompt162r1.LastReport();const auto& cursor=g_action_cursor_d69.LastReport();
    const bool complete=ok&&(!enabled||(hint.resolved==3&&hint.verified==3&&cursor.resolved==3&&cursor.verified==3));
    // First acquisition may bind newly observed children; next iteration seeds
    // that new event key. It does not let an old key hide a failed transaction.
    if(keyFor()==key)retry.Complete(complete);
    static ui_hud_work59c7::CaptureBudget capture;
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Hud)].load();
    if(capture.Take(HudCaptureActive175(),session,true)){char message[240]{};
        std::snprintf(message,sizeof(message),"D74 HUD_PROMPT station=%d enabled=%d hints=%.2f controls=%.2f resolved=%X/%X verified=%X/%X complete=%d position=NATIVE",
            station,enabled,double(scales.hints),double(scales.controls),hint.resolved,cursor.resolved,hint.verified,cursor.verified,complete);
        diagnostic_trace::Write(message);
    }
}

// R112: size of the big directional combat crosshair (CombatCursorCross). Same scale-only child owner as the
// D69 action cursor; the game keeps owning its position/rotation/alpha/visibility, so the directions stay right.
// Event driven (HUD generation, child identity, native scale reset, slider value); an incomplete transaction is
// tried again every 5 s and on the next event, never latched.
ui_hud_r112::CombatCrosshair g_combat_crosshair_r112;
void UpdateCombatCrosshairR112(bool enabled,std::uint64_t now) noexcept {
    const float scale=ui_hud_r112::CombatCrosshairScale();
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    if(!enabled&&!g_combat_crosshair_r112.Active())return;
    static bool lastComplete=true;
    std::uint64_t key=1469598103934665603ull;
    for(auto value:{HudGeneration(),g_canvas_lifetime.load(),g_combat_cursor_children_r112.Epoch()})ui_hud_work59c7::Mix(key,value);
    ui_hud_work59c7::Mix(key,enabled);{std::uint32_t bits=0;std::memcpy(&bits,&scale,4);ui_hud_work59c7::Mix(key,bits);}
    if(!lastComplete)ui_hud_work59c7::Mix(key,now/5000ull);
    // Without a native child key (see NoteHudKeyRefusalR112) a native scale reset cannot wake us: look every 2 s
    // (one Flash read; a write only when the size is not ours any more).
    else if(enabled&&g_combat_cursor_children_r112.Epoch()<=1)ui_hud_work59c7::Mix(key,now/2000ull);
    static ui_hud_work59c7::Retry retry;
    if(!retry.Due(key,now))return;
    HudHolder holder{};void* hud=nullptr;
    if(!AcquireHudDirect(holder,hud)){ReleaseHudHolder(holder);lastComplete=false;return;}
    const auto previous=g_canvas_live_hud.exchange(hud,std::memory_order_acq_rel);
    if(previous!=hud)g_canvas_lifetime.fetch_add(1,std::memory_order_acq_rel);
    struct CombatCrosshairBackendR112 : NativeCanvasBackend {
        bool Resolve(unsigned i,const char* name,std::uintptr_t& id) noexcept {
            if(i>=1||!NativeCanvasBackend::Resolve(i,name,id))return false;
            ui_hud_visibility175::Key child{};
            if(ReadHudKey175(handles[i].clip,hud,HudGeneration(),child)){g_combat_cursor_children_r112.Observe(0,child);id=child.object;return true;}
            NoteHudKeyRefusalR112(name,handles[i].clip,hud);id=reinterpret_cast<std::uintptr_t>(handles[i].clip);return true;
        }
        std::uint64_t Revision(unsigned i) const noexcept {return g_combat_cursor_children_r112.Revision(i);}
        bool Set(unsigned i,ui_native_canvas::Geometry value) noexcept {
            if(i>=1||!Alive())return false;
            alignas(16) auto bytes=ui_prompt_scale120r3::Notifications::PackScale(value); // flags 0x18: x/y scale only
            ++g_hud_owned_write175;const bool ok=CallFlashDisplayFnSEH(handles[i].set,handles[i].clip,bytes.data());--g_hud_owned_write175;
            return ok&&Alive();
        }
    }io;io.hud=hud;io.generation=g_canvas_lifetime.load(std::memory_order_acquire);
    const bool ok=g_combat_crosshair_r112.Run(io,io.generation,enabled,scale);
    const auto& r=g_combat_crosshair_r112.LastReport();
    const bool complete=ok&&(!enabled||(r.resolved==1u&&r.verified==1u));
    ReleaseHudHolder(holder);
    retry.Complete(complete);lastComplete=complete;
    static int loggedEnabled=-1;
    if(r.written||!complete||loggedEnabled!=int(enabled)){loggedEnabled=int(enabled);char m[300]{};
        std::snprintf(m,sizeof(m),"R112 COMBAT_CROSSHAIR enabled=%d scale=%.2f resolved=%u verified=%u written=%u native=(%.1f,%.1f) target=(%.1f,%.1f) readback=(%.1f,%.1f) complete=%d position=NATIVE",
            enabled?1:0,double(scale),r.resolved,r.verified,r.written,double(r.before[0].xs),double(r.before[0].ys),double(r.target[0].xs),double(r.target[0].ys),
            double(r.after[0].xs),double(r.after[0].ys),complete?1:0);
        diagnostic_trace::Write(m);}
}

// R112: quick weapon/potion selector fade (QAMWeapon/QAMFood alpha only, never _visible or position). The mod
// keeps the native adaptive HUD off for the palm HUD (R57C1 lease), and that adaptive HUD was what faded them,
// so they stayed on screen. Timing is ui_hud_r112::QuickFade; this is the Flash side.
// Flash work: nothing while not owned and not wanted; writes at most every 50 ms while the alpha must change
// (0.7 s fade = ~14 steps); while held faded, two reads every 250 ms catch the game re-showing a selector.
namespace {
ui_hud_r112::QuickFade g_quick_fade_r112;
struct QuickSlotR112 {float written=-1.f,native=100.f;bool owned=false,visible=false,nativeVisible=true;};
std::array<QuickSlotR112,2> g_quick_slots_r112{};
std::uint64_t g_quick_life_r112=~std::uint64_t{0},g_quick_io_r112=0;
}
void UpdateQuickSlotFadeR112(bool enabled,bool reveal,std::uint64_t now) noexcept {
    const ui_hud_r112::QuickInput in{enabled,reveal,
        {ui_hud_r112::weaponQuickInputMs.load(std::memory_order_acquire),ui_hud_r112::foodQuickInputMs.load(std::memory_order_acquire)},
        now,HudGeneration()};
    auto target=g_quick_fade_r112.Targets(in);
    // A new HUD has new selector clips at their native alpha; ownership of the old ones ended with them.
    if(in.hudLifetime!=g_quick_life_r112){g_quick_life_r112=in.hudLifetime;g_quick_slots_r112={};}
    bool owned=false,change=false;
    for(unsigned i=0;i<2;++i){const auto& s=g_quick_slots_r112[i];owned=owned||s.owned;
        if(target[i]>=0.f?(!s.owned||std::fabs(target[i]-s.written)>=1.f):s.owned)change=true;}
    if(!owned&&!change)return;
    const bool due=(change&&now-g_quick_io_r112>=50)||(owned&&now-g_quick_io_r112>=100); // R115: a native re-show is hidden again within 0.1 s
    if(!due)return;
    g_quick_io_r112=now;
    std::lock_guard<std::recursive_mutex> lock(g_hud_draw_mutex162r1);
    HudVisibilityBackend170 io{};
    if(!io.Acquire()){io.Release();return;}
    static constexpr const char* names[2]{"QAMWeapon","QAMFood"};
    unsigned writes=0,failures=0;bool edge=false,reshown=false;
    for(unsigned i=0;i<2;++i){auto& s=g_quick_slots_r112[i];std::uintptr_t id=0;
        if(!io.Resolve(i,names[i],id)){++failures;continue;}
        alignas(16) unsigned char data[0x38]{};
        if(!HudVisibilityBackend170::Call(io.handle[i].get,io.handle[i].clip,data)){++failures;continue;}
        std::uint16_t flags=0;std::memcpy(&flags,data+0x34,2);
        float alpha=-1.f;if(flags&0x20)std::memcpy(&alpha,data+0x14,4);
        if(!std::isfinite(alpha)||alpha<0.f||alpha>100.f){++failures;continue;}
        const bool visible=(flags&0x400)?data[0x28]!=0:true;
        float want=target[i];
        if(s.owned){
            // R115: a native re-show (visible again / alpha above ours) is NOT a selection any more - it is only
            // answered (hidden again below). before R115 it restarted the 3 s show.
            if((visible&&!s.visible)||alpha>s.written+1.f)reshown=true;
        }else{s.native=alpha;s.nativeVisible=visible;}
        s.visible=visible;
        // R115: hidden = alpha 0 AND _visible false (R114 log: alpha read 0 most of the time, yet he still saw the
        // icons); shown = _visible true + alpha. switched OFF: native alpha + native visibility back.
        auto write=[&](float value,bool vis)->bool{alignas(16) unsigned char out[0x38]{};const std::uint16_t f=0x20|0x400;
            std::memcpy(out+0x34,&f,2);std::memcpy(out+0x14,&value,4);out[0x28]=vis?1:0;
            ++g_hud_owned_write175;const bool ok=HudVisibilityBackend170::Call(io.handle[i].set,io.handle[i].clip,out);--g_hud_owned_write175;
            if(ok)++writes;else ++failures;return ok;};
        if(want<0.f){ // switched OFF: give the native alpha back once
            if(s.owned&&((std::fabs(alpha-s.native)<.5f&&visible==s.nativeVisible)||write(s.native,s.nativeVisible))){s.owned=false;edge=true;}
            continue;
        }
        const bool wantVisible=want>=.5f;
        if((std::fabs(alpha-want)>=.5f||visible!=wantVisible)&&!write(want,wantVisible))continue;
        s.visible=wantVisible;
        if(!s.owned||(s.written>=99.f)!=(want>=99.f))edge=true;
        s.owned=true;s.written=want;
    }
    io.Release();
    if(edge||reshown){char m[260]{};
        std::snprintf(m,sizeof(m),"R112 QUICK_SLOT_FADE enabled=%d reveal=%d weapon=%.0f food=%.0f reshown=%d writes=%u failures=%u holdMs=%llu fadeMs=%llu visible=NATIVE",
            enabled?1:0,reveal?1:0,double(g_quick_slots_r112[0].written),double(g_quick_slots_r112[1].written),reshown?1:0,writes,failures,
            (unsigned long long)ui_hud_r112::kQuickHoldMs,(unsigned long long)ui_hud_r112::kQuickFadeMs);
        diagnostic_trace::Write(m);}
}

bool ApplyWholeReadableHud44c2(bool enabled,const ui_surface_geometry::Hud& geometry,float size,float width,float upper,float lower,float height ,float iconScale) noexcept {
    std::lock_guard<std::recursive_mutex> lock(g_hud_draw_mutex162r1);
    const auto fit59c7=ui_native_canvas::ComputeAdjustableReadable49(geometry,size,width,height,upper,lower,iconScale);
    std::uint64_t key59c7=1469598103934665603ull;
    for(float v:{fit59c7.sx,fit59c7.sy,fit59c7.tx,fit59c7.ty,fit59c7.width49,fit59c7.height49,fit59c7.upper49,fit59c7.lower49,fit59c7.iconScale51c3}){
        std::uint32_t bits=0;std::memcpy(&bits,&v,4);ui_hud_work59c7::Mix(key59c7,bits);
    }
    ui_hud_work59c7::Mix(key59c7,HudGeneration());ui_hud_work59c7::Mix(key59c7,g_canvas_lifetime.load());
    // D63: the matching companion authors the six message containers at 50%.
    // Every dynamically attached message inherits that scale from its first frame.
    // Do not also apply the old deferred C++ factor to the same container.
    SetNotificationsRequestedD80(false);
    if(enabled&&fit59c7.valid&&g_parent_ready59c7&&g_parent_key59c7==key59c7)return ApplyNotificationScaleR12();
    g_parent_ready59c7=false;
    g_canvas_requested=enabled;
    HudHolder holder{};void* hud=nullptr;
    if(!AcquireHudDirect(holder,hud)){ReleaseHudHolder(holder);return false;}
    const auto previous=g_canvas_live_hud.exchange(hud);
    if(previous!=hud)++g_canvas_lifetime;
    NativeCanvasBackend io{};io.hud=hud;io.generation=g_canvas_lifetime.load();
    g_canvas_report=g_canvas.Run(io,io.generation,enabled,ui_native_canvas::ComputeAdjustableReadable49(geometry,size,width,height,upper,lower,iconScale));
    const bool parentsComplete=g_canvas_report.complete;
    const bool notificationsOk=ApplyNotificationScaleR12();
    g_canvas_report.complete=g_canvas_report.complete&&notificationsOk;
    if(HudCaptureActive175()){
        char m[256]{};std::snprintf(m,sizeof(m),"R44C2 READABLE_PARENTS complete=%d size=%.2f resolved=%X applied=%X effects=NATIVE lookup=NATIVE_ALIAS_OR_PATH retries=EVENT_ONLY",g_canvas_report.complete,double(size),g_canvas_report.resolved,g_canvas_report.written);
        diagnostic_trace::Write(m);
    }
    ReleaseHudHolder(holder);
    // Canvas lifetime can change on first acquisition; the next event may perform
    // one fresh check, but the cache never conceals a failed transaction.
    if(enabled&&parentsComplete){g_parent_key59c7=key59c7;g_parent_ready59c7=true;}
    return g_canvas_report.complete;
}
bool ApplyNativeCanvasLayout(bool enabled,const ui_surface_geometry::Hud& geometry,float elementScale,float spreadX,float spreadY) noexcept {
    g_parent_ready59c7=false;g_elements_retry59c7.Reset();
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    SetNotificationsRequestedD80(false);
    g_canvas_requested=enabled;

    if(!enabled&&!g_canvas.Active()&&!g_elements171.Active()&&!g_notifications_r12.Active()){g_canvas_report={};g_canvas_report.complete=true;return true;}
    HudHolder holder{};void* hud=nullptr;
    // Bounded event-driven reacquisition with a retained holder; no old observed
    // address fallback, and no worker polling of gameplay/managers.
    if(!AcquireHudDirect(holder,hud)){
        ReleaseHudHolder(holder);g_canvas_report={ui_native_canvas::Status::Missing};return false;
    }
    const auto previous=g_canvas_live_hud.exchange(hud,std::memory_order_acq_rel);
    if(previous!=hud)g_canvas_lifetime.fetch_add(1,std::memory_order_acq_rel);
    NativeCanvasBackend backend{};backend.hud=hud;
    backend.generation=g_canvas_lifetime.load(std::memory_order_acquire);
    const auto anchored=ui_native_canvas::ComputeAnchored(geometry,elementScale,spreadX,spreadY);
    g_canvas_report=g_canvas.Run(backend,backend.generation,enabled,{},anchored);
    NativeCanvasBackend elementBackend{};elementBackend.hud=hud;elementBackend.generation=backend.generation;
    const bool elementsRestored=g_elements171.Run(elementBackend,backend.generation,false,{});
    const bool notificationsRestored=ApplyNotificationScaleR12();
    const bool promptsRestored=ApplyReadablePromptScale(false,1.f,1.f)&&elementsRestored&&notificationsRestored;
    ReleaseHudHolder(holder);
    return g_canvas_report.complete&&promptsRestored;
}

bool ApplyUniformReadableHudLayout(bool enabled,const ui_surface_geometry::Hud& geometry,float size,float safeWidth,float topBoundary,float bottomBoundary,float offsetX,float offsetY,float dialogueScale,float hintsScale,float controlsScale,float subtitleScale) noexcept {
    g_parent_ready59c7=false;
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    SetNotificationsRequestedD80(false);
    const bool notificationsRestored=ApplyNotificationScaleR12();
    // V66.115R2 SAFE-UNIFORM HYBRID:
    // keep KCD2's readable-parent hierarchy and native relative layout, but compress
    // anchor POSITIONS into one binocular-safe rectangle while element scale stays
    // independent. This avoids the old low-resolution "scale the whole HUD texture"
    // look and keeps fader/vignette/ratioStrips/popUp outside the transaction.
    //
    // The optical safe rectangle can move slightly as runtime geometry changes.
    // For a HUD this is undesirable: center around the authored/game center
    // (960,540). Upper/lower boundaries never translate that center.
    g_canvas_requested=enabled;

    if(!enabled&&!g_canvas.Active()&&!g_elements171.Active()&&!g_notifications_r12.Active()){g_canvas_report={};g_canvas_report.complete=true;return true;}
    HudHolder holder{};void* hud=nullptr;
    if(!AcquireHudDirect(holder,hud)){
        ReleaseHudHolder(holder);g_canvas_report={ui_native_canvas::Status::Missing};return false;
    }
    const auto previous=g_canvas_live_hud.exchange(hud,std::memory_order_acq_rel);
    if(previous!=hud)g_canvas_lifetime.fetch_add(1,std::memory_order_acq_rel);
    NativeCanvasBackend backend{};backend.hud=hud;
    backend.generation=g_canvas_lifetime.load(std::memory_order_acquire);

    // V66.120R1: separate upper/lower radii about the authored center.
    // Old offsets never translate the center in this mode.
    (void)offsetX;(void)offsetY;
    auto fit=ui_native_canvas::ComputeCentered(geometry,
        std::clamp(size,.25f,1.20f),std::clamp(safeWidth,.60f,1.40f),
        std::clamp(topBoundary,.55f,3.00f),std::clamp(bottomBoundary,.55f,3.00f));
    fit.dialogueScale=std::isfinite(dialogueScale)?std::clamp(dialogueScale,.30f,1.50f):1.f;
    g_canvas_report=g_canvas.Run(backend,backend.generation,enabled,{},fit);

    const bool promptsOk=ApplyReadablePromptScale(enabled,hintsScale,controlsScale); // settings/lifecycle event only
    static float lastTop=-1.f,lastBottom=-1.f,lastSize=-1.f,lastDialogue=-1.f;
    if(enabled&&(lastTop!=topBoundary||lastBottom!=bottomBoundary||lastSize!=size||lastDialogue!=dialogueScale)){
        lastTop=topBoundary;lastBottom=bottomBoundary;lastSize=size;lastDialogue=dialogueScale;
        const auto& b=g_canvas.Targets()[7];
        char detail[384]{};
        std::snprintf(detail,sizeof(detail),"V66.120R2-HUD-GEOMETRY upper=%.2f lower=%.2f spans={%.1f,%.1f} size=%.2f dialogue=%.2f bc={x:%.1f y:%.1f scale:%.1f} result=%d complete=%d",
            double(topBoundary),double(bottomBoundary),double(fit.upperSpan),double(fit.lowerSpan),double(size),double(fit.dialogueScale),double(b.x),double(b.y),double(b.ys),int(g_canvas_report.status),g_canvas_report.complete?1:0);
        diagnostic_trace::Write(detail);
    }
    ReleaseHudHolder(holder);
    return g_canvas_report.complete&&promptsOk&&notificationsRestored;
}


bool ApplyUniformHudElements171(float compass,float compassY,float stats,float lower,float lowerY,float spread,float statsX,float statsY) noexcept {
    std::lock_guard<std::recursive_mutex> lock(g_hud_draw_mutex162r1);
    const std::array<ui_hud_elements171::Adjustment,4> targets{{
        {std::clamp(compass,.15f,1.50f),0,-compassY},{std::clamp(stats,.15f,1.50f),statsX,-statsY},
        {std::clamp(lower,.15f,2.f),-spread,-lowerY},{std::clamp(lower,.15f,2.f),spread,-lowerY}}};
    std::uint64_t key=1469598103934665603ull;
    ui_hud_work59c7::Mix(key,HudGeneration());ui_hud_work59c7::Mix(key,g_canvas_lifetime.load());
    ui_hud_work59c7::Mix(key,g_children59c7.Epoch());
    for(auto a:targets)for(float f:{a.scale,a.x,a.y}){std::uint32_t b=0;std::memcpy(&b,&f,4);ui_hud_work59c7::Mix(key,b);}
    static std::uint64_t resultKey=0;static bool resultOk=false;
    if(resultKey!=key){resultKey=key;resultOk=false;}
    if(!g_elements_retry59c7.Due(key,GetTickCount64()))return resultOk;
    HudHolder holder{};void* hud=nullptr;
    if(!g_display_verified170||!AcquireHudDirect(holder,hud)){ReleaseHudHolder(holder);return false;}
    NativeCanvasBackend backend{};backend.hud=hud;backend.generation=g_canvas_lifetime.load();
    const bool ok=g_elements171.Run(backend,backend.generation,true,targets);
    const auto report=g_elements171.LastReport();resultOk=ok;g_elements_retry59c7.Complete(ok&&!report.pending);
    char msg[380]{};std::snprintf(msg,sizeof(msg),"R59.16C7 HUD_ELEMENTS compass=%.2f health=%.2f quickSlots=%.2f result=%d resolved=%X applied=%X pending=%X queried=%X statsResolved=%d animation=NATIVE retries=BOUNDED_EVENT",
        double(compass),double(stats),double(lower),ok,report.resolved,report.applied,report.pending,report.queried,(report.resolved&2)!=0);
    diagnostic_trace::Write(msg);ReleaseHudHolder(holder);return ok;
}

void UpdateHudVitals59c7(bool requested,bool contextualRequested) noexcept {
    (void)ApplyNotificationScaleR12(); // child lifetime repair/restore, independent of parent layout cache
    const bool show=contextualRequested||(requested&&g_corrected36.load(std::memory_order_acquire));
    g_vitals_pin59c7.store(show,std::memory_order_release);
    std::uint64_t key=1469598103934665603ull;
    ui_hud_work59c7::Mix(key,show);ui_hud_work59c7::Mix(key,contextualRequested);ui_hud_work59c7::Mix(key,HudGeneration());
    ui_hud_work59c7::Mix(key,g_children59c7.Epoch());
    if(!g_vitals_retry59c7.Due(key,GetTickCount64()))return;
    std::lock_guard<std::recursive_mutex> lock(g_hud_draw_mutex162r1);
    struct IO {
        HudVisibilityBackend170 backend;
        std::uint64_t Revision(unsigned i){return g_children59c7.Revision(i?1:4);}
        bool Resolve(unsigned i,std::uintptr_t&id){
            if(!backend.Resolve(i,i?"Stats":"bc.mc_StatsContainer",id))return false;
            ui_hud_visibility175::Key key{};
            if(!ReadHudKey175(backend.handle[i].clip,backend.hud,backend.Generation(),key))return false;
            g_children59c7.Observe(i?1:4,key);id=key.object;return true;
        }
        bool Get(unsigned i,ui_hud_work59c7::Display&out){
            alignas(16) unsigned char bytes[0x38]{};
            if(!backend.Call(backend.handle[i].get,backend.handle[i].clip,bytes))return false;
            std::uint16_t flags=0;std::memcpy(&flags,bytes+0x34,2);
            if((flags&0x420)!=0x420)return false;
            out.visible=bytes[0x28]!=0;std::memcpy(&out.alpha,bytes+0x14,4);
            return std::isfinite(out.alpha)&&out.alpha>=0&&out.alpha<=100;
        }
        bool Set(unsigned i,ui_hud_work59c7::Display value,unsigned selectedFlags){
            alignas(16) unsigned char bytes[0x38]{};const std::uint16_t flags=std::uint16_t(selectedFlags&0x420u);
            std::memcpy(bytes+0x34,&flags,2);bytes[0x28]=value.visible?1:0;
            std::memcpy(bytes+0x14,&value.alpha,4);
            ++g_hud_owned_write175;const bool ok=backend.Call(backend.handle[i].set,backend.handle[i].clip,bytes);--g_hud_owned_write175;return ok;
        }
        bool NativeVisible(unsigned i,bool fallback){
            if(i!=1)return fallback;
            bool native=fallback;std::uintptr_t id=reinterpret_cast<std::uintptr_t>(backend.handle[i].clip);
            return backend.NativeVisibility175(1,id,fallback,native)?native:fallback;
        }
    }io;
    if(!io.backend.Acquire()){io.backend.Release();return;}
    const auto report=g_vitals59c7.Update(io,show,HudGeneration(),!contextualRequested);io.backend.Release();
    g_vitals_retry59c7.Complete(report.complete);
    char m[272]{};std::snprintf(m,sizeof(m),"R59.16C7 HUD_VITALS showRequested=%d resolved=%X written=%X failed=%X complete=%d scope=%s containers=StatsParent+Stats nativeValues=UNCHANGED",show,report.resolved,report.written,report.failed,report.complete,contextualRequested?"PALM_VISIBLE_ONLY":"ORDINARY_CORRECTED");diagnostic_trace::Write(m);
}

bool ApplyUniformDialogueLayout(bool enabled,float dialogueScale,float dialogueY,float dialogueInsetPct,
                                float subtitleScale,float subtitleY) noexcept {
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    dialogueScale=std::clamp(dialogueScale,0.30f,1.50f);
    dialogueY=std::clamp(dialogueY,-720.0f,720.0f);
    dialogueInsetPct=std::clamp(dialogueInsetPct,0.0f,80.0f);
    subtitleScale=std::clamp(subtitleScale,0.30f,1.25f);
    subtitleY=std::clamp(subtitleY,-600.0f,600.0f);
    const float ds=enabled?dialogueScale:1.0f;
    const float ss=enabled?subtitleScale:1.0f;
    const float inset=enabled?(960.0f*dialogueInsetPct*0.01f):0.0f;
    const float dy=540.0f+(enabled?dialogueY:0.0f);
    const float sy=-230.0f+(enabled?subtitleY:0.0f);
    HudHolder holder{};void* hud=nullptr;
    if(!AcquireHudDirect(holder,hud)){ReleaseHudHolder(holder);return false;}
    bool ok=true;
    ok=ApplyMovieClipDisplay(hud,"DialogLeft",inset,dy,ds)&&ok;
    ok=ApplyMovieClipDisplay(hud,"DialogRight",1920.0f-inset,dy,ds)&&ok;
    ok=ApplyMovieClipDisplay(hud,"Subtitles",-480.0f,sy,ss)&&ok;
    ReleaseHudHolder(holder);
    return ok;
}

std::uint64_t ContextPanelGeneration() noexcept {return g_panel_epoch.load();}
bool ApplyContextPanelLayout(unsigned index,bool enabled,const ui_surface_geometry::Hud& geometry,float size) noexcept {
    if(index>=2)return false;
    if(!enabled&&!g_panel_fit[index].Active())return true;
    if(!enabled&&!g_panel_live[index].load()){g_panel_fit[index].Forget(g_panel_life[index].load());return true;}
    HudHolder holder{};void* element=nullptr;
    const char*name=index==0?"ItemSelection":"ItemTransfer";
    if(!AcquireElementDirect(holder,name,element)){ReleaseHudHolder(holder);return false;}
    const auto prev=g_panel_live[index].exchange(element);
    if(prev!=element)g_panel_life[index].fetch_add(1);
    ContextPanelBackend io{};io.element=element;io.index=index;io.generation=g_panel_life[index].load();
    const bool ok=g_panel_fit[index].Run(io,io.generation,enabled,geometry.safe,size);
    ReleaseHudHolder(holder);return ok;
}

// C43: separate movie; no whole-HUD/root transform or steady Flash polling.
static ui_modal_overlay43::Transaction g_modal_fit43;
bool ApplyModalOverlayLayout43(bool enabled,const ui_surface_geometry::Hud& geometry,float size) noexcept {
    if(!retail_build112::Ready(WhBase()))return false;
    if(!enabled&&!g_modal_fit43.Active())return true;
    HudHolder holder{};void* element=nullptr;
    if(!AcquireElementDirect(holder,"ApseModalDialog",element)){
        ReleaseHudHolder(holder);if(!enabled)g_modal_fit43.Forget();return !enabled;
    }
    struct Backend {
        void* element=nullptr;void* clip=nullptr;void* get=nullptr;void* set=nullptr;
        bool Resolve(const char* name,std::uintptr_t& id)noexcept {
            if(!FindMovieClip(element,name,clip,get,set))return false;
            id=reinterpret_cast<std::uintptr_t>(clip);return true;
        }
        bool Get(ui_native_canvas::Geometry& value)noexcept {
            alignas(16) std::array<unsigned char,0x38> bytes{};
            if(!CallFlashDisplayFnSEH(get,clip,bytes.data()))return false;
            value=ui_native_canvas::Unpack(bytes);return true;
        }
        bool Set(ui_native_canvas::Geometry value)noexcept {
            alignas(16) auto bytes=ui_native_canvas::Pack(value);
            return CallFlashDisplayFnSEH(set,clip,bytes.data());
        }
    } io;io.element=element;
    const bool ok=g_modal_fit43.Run(io,reinterpret_cast<std::uintptr_t>(element),enabled,geometry,std::clamp(size,.45f,1.15f));
    ReleaseHudHolder(holder);return ok;
}

// Exact existing Flash holder/getter/setter ABI. No cached movie pointer or
// per-frame polling; root calls only a bounded Overlay lifetime/surface event.
std::uint64_t HelmetGeneration63() noexcept{return g_helmet_generation63.load(std::memory_order_acquire);}
bool ApplyHelmetOverlay63(bool enabled,float width) noexcept {
    static ui_helmet_overlay63::Transaction fit;
    static std::uint64_t generation=~0ull;
    if(!retail_build112::Ready(WhBase())||!g_display_verified170)return false;
    if(!enabled&&!fit.Active())return true;
    HudHolder holder{};void* element=nullptr;
    if(!AcquireElementDirect(holder,"Overlay",element)){ReleaseHudHolder(holder);return false;}
    // Acquire itself may publish the first observed movie generation. Capture
    // AFTER acquisition, or our first successful correction becomes a baseline
    // again next frame and shifts the helmet twice.
    const auto liveGeneration=HelmetGeneration63();
    if(generation!=liveGeneration){fit.Forget();generation=liveGeneration;}
    struct Backend {
      void* element=nullptr;void* clip=nullptr;void* get=nullptr;void* set=nullptr;
      std::uint64_t generation=0;
      bool Alive()const noexcept{return generation==HelmetGeneration63()&&element==g_helmet_element63.load(std::memory_order_acquire);}
      bool Resolve(const char* name,std::uintptr_t& id)noexcept {
        if(!Alive()||!FindMovieClip(element,name,clip,get,set)||!Alive())return false;
        id=reinterpret_cast<std::uintptr_t>(clip);return true;
      }
      bool Get(ui_native_canvas::Geometry& value)noexcept {
        alignas(16) std::array<unsigned char,0x38> bytes{};
        if(!Alive()||!CallFlashDisplayFnSEH(get,clip,bytes.data())||!Alive())return false;
        value=ui_native_canvas::Unpack(bytes);return true;
      }
      bool Set(ui_native_canvas::Geometry value)noexcept {
        // Only the existing helmet parent XY/scale channels. No full-field
        // movie, rotation, alpha, visibility or cinematic effect writes.
        alignas(16) auto bytes=ui_native_canvas::Pack(value);
        return Alive()&&CallFlashDisplayFnSEH(set,clip,bytes.data())&&Alive();
      }
    } io;io.element=element;io.generation=liveGeneration;
    const bool ok=fit.Run(io,reinterpret_cast<std::uintptr_t>(element),enabled,width);
    ReleaseHudHolder(holder);return ok;
}

std::uint64_t WaitGenerationB2() noexcept{return g_wait_generation_b2.load(std::memory_order_acquire);}
bool ApplyWaitOverlayB2(bool enabled,const ui_surface_geometry::Hud& geometry) noexcept {
    static ui_modal_overlay43::Transaction fit;
    static std::uint64_t generation=~0ull;
    if(!retail_build112::Ready(WhBase())||!g_display_verified170)return false;
    if(!enabled&&!fit.Active())return true;
    HudHolder holder{};void* element=nullptr;
    if(!AcquireElementDirect(holder,"SkipTime",element)){
        ReleaseHudHolder(holder);if(!enabled)fit.Forget();return !enabled;
    }
    const auto live=WaitGenerationB2();if(generation!=live){fit.Forget();generation=live;}
    struct Backend {
        void* element=nullptr;void* clip=nullptr;void* get=nullptr;void* set=nullptr;std::uint64_t generation=0;
        bool Alive()const noexcept{return generation==WaitGenerationB2()&&element==g_wait_element_b2.load(std::memory_order_acquire);}
        bool Resolve(const char* name,std::uintptr_t& id)noexcept {
            if(!Alive()||!FindMovieClip(element,name,clip,get,set)||!Alive())return false;
            id=reinterpret_cast<std::uintptr_t>(clip);return true;
        }
        bool Get(ui_native_canvas::Geometry& value)noexcept {
            alignas(16) std::array<unsigned char,0x38> bytes{};
            if(!Alive()||!CallFlashDisplayFnSEH(get,clip,bytes.data())||!Alive())return false;
            value=ui_native_canvas::Unpack(bytes);return true;
        }
        bool Set(ui_native_canvas::Geometry value)noexcept {
            alignas(16) auto bytes=ui_native_canvas::Pack(value);
            return Alive()&&CallFlashDisplayFnSEH(set,clip,bytes.data())&&Alive();
        }
    } io;io.element=element;io.generation=live;
    // Retail fc_addDialog attaches exactly mc_Dialog. Fader and fullscreen
    // siblings retain their native size. Unknown/non-centered geometry refuses.
    const bool ok=fit.Run(io,reinterpret_cast<std::uintptr_t>(element),enabled,geometry,.70f,"mc_Dialog");
    ReleaseHudHolder(holder);return ok;
}

#include "../activity_overlays_s1.inl"

std::uint64_t PauseGeneration() noexcept{return g_pause_lifetime.load(std::memory_order_acquire);}

template<class T> void LogOverlayGeometry163(const char* kind,const T& fit,const ui_surface_geometry::Hud& surface) noexcept {
    // Already captured transaction values: no extra native calls or frame polling.
    for(std::size_t i=0;i<fit.Baseline163().size();++i){
        const auto& b=fit.Baseline163()[i];const auto& a=fit.Applied163()[i];char m[384]{};
        std::snprintf(m,sizeof(m),"V66.163-OVERLAY kind=%s anchor=%zu native=(%.2f,%.2f,%.3f,%.3f) applied=(%.2f,%.2f,%.3f,%.3f) panelMeters=(%.4f,%.4f) units=XY_SCALE_PERCENT",
            kind,i,double(b.x),double(b.y),double(b.xs),double(b.ys),double(a.x),double(a.y),double(a.xs),double(a.ys),double(surface.surface.width),double(surface.surface.height));
        diagnostic_trace::Write(m);
    }
}
bool ApplyPauseOverlayLayout(bool enabled,const ui_surface_geometry::Hud& geometry,float size,float promptScale) noexcept {
    static bool reported163=false;if(!enabled)reported163=false;
    if(!retail_build112::Ready(WhBase()))return false;
    if(!enabled&&!g_pause_fit.Active())return true;
    HudHolder holder{};void* menu=nullptr;
    if(!AcquireElementDirect(holder,"Menu",menu)){
        ReleaseHudHolder(holder);
        if(!enabled){g_pause_fit.Forget();g_pause_menu_live.store(nullptr,std::memory_order_release);return true;}
        return false;
    }
    const auto old=g_pause_menu_live.exchange(menu,std::memory_order_acq_rel);
    if(old!=menu){g_pause_lifetime.fetch_add(1);g_pause_fit.Forget(reinterpret_cast<std::uintptr_t>(menu));}
    PauseOverlayBackend io{};io.element=menu;io.generation=g_pause_lifetime.load();
    size=std::clamp(size,.45f,1.15f);
    // Pause has its own semantic owner. Its readable-card fit must work with
    // AutoHUD/F2 OFF as well: otherwise absent optional footer clips abort the
    // whole fit and the exit/settings card stays at native full-canvas size.
    const bool layoutOk=g_pause_fit.Run(io,reinterpret_cast<std::uintptr_t>(menu),enabled,geometry,size,std::clamp(promptScale,.25f,1.5f),true);
    if(g_corrected36.load()){char detail[224]{};std::snprintf(detail,sizeof(detail),"V66.178R36C5 PAUSE_CONTENT_FIT enabled=%d ok=%d present=0x%X missing=0x%X background=FULL_FIELD",enabled,layoutOk,g_pause_fit.presentMask,g_pause_fit.failedMask);diagnostic_trace::Write(detail);}
    if(enabled&&layoutOk&&!reported163){reported163=true;LogOverlayGeometry163("PAUSE",g_pause_fit,geometry);}
    // The uploaded Clean Pause ASI proves Menu rendering and pause DoF are separate.
    // Keep native pause semantics and native DoF policy; do not force blur here.
    // Hide this root background only after a verified readable-content fit.
    if(g_pause_background_owner36!=menu){
        g_pause_background_owner36=menu;g_pause_background_known36=false;g_pause_background_hidden36=false;
    }
    if(!g_pause_background_known36){
        void* clip=nullptr;void* get=nullptr;void* set=nullptr;
        alignas(16) std::array<unsigned char,0x38> info{};
        if(io.Alive()&&FindMovieClip(menu,"background",clip,get,set)&&CallFlashDisplayFnSEH(get,clip,info.data())&&io.Alive()){
            g_pause_background_base36=info[0x28]!=0;g_pause_background_known36=true;
        }
    }
    bool bgOk=true;
    if(g_corrected36.load()){
        if(g_pause_background_hidden36&&g_pause_background_known36){
            bgOk=io.Alive()&&SetElementChildVisible(menu,"background",g_pause_background_base36)&&io.Alive();
            if(bgOk)g_pause_background_hidden36=false;
        }
    }else{
        bgOk=io.Alive()&&SetElementChildVisible(menu,"background",!(enabled&&layoutOk))&&io.Alive();
        if(bgOk)g_pause_background_hidden36=enabled&&layoutOk;
    }
    ReleaseHudHolder(holder);
    if(layoutOk){static bool last=false;if(last!=enabled){last=enabled;diagnostic_trace::Write(enabled
        ? "V66.178R59.16C10R2-PAUSE-OVERLAY readable Menu.gfx cards fitted; background follows HUD policy; effects=NATIVE"
        : "V66.97-PAUSE-OVERLAY native Menu.gfx geometry/background restored");}}
    return layoutOk&&bgOk;
}

std::uint64_t DeathGeneration() noexcept{return g_death_lifetime.load(std::memory_order_acquire);}
bool ApplyDeathOverlayLayout(bool enabled,const ui_surface_geometry::Hud& geometry,float size) noexcept {
    static bool reported163=false;if(!enabled)reported163=false;
    if(!retail_build112::Ready(WhBase()))return false;
    if(!enabled&&!g_death_fit.Active())return true;
    HudHolder holder{};void* death=nullptr;
    if(!AcquireElementDirect(holder,"GameOver",death)){
        ReleaseHudHolder(holder);
        if(!enabled){g_death_fit.Forget();g_death_live.store(nullptr);return true;}
        return false;
    }
    const auto old=g_death_live.exchange(death);
    if(old!=death){g_death_lifetime.fetch_add(1);g_death_fit.Forget(reinterpret_cast<std::uintptr_t>(death));}
    DeathOverlayBackend io{};io.element=death;io.generation=g_death_lifetime.load();
    const bool ok=g_death_fit.Run(io,reinterpret_cast<std::uintptr_t>(death),enabled,geometry,std::clamp(size,.45f,1.15f));
    if(enabled&&ok&&!reported163){reported163=true;LogOverlayGeometry163("DEATH",g_death_fit,geometry);}
    ReleaseHudHolder(holder);return ok;
}

bool ApplyAutoVrLayout(bool enabled,float compactness,float center_x,float center_y) noexcept {
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    static std::atomic_flag busy=ATOMIC_FLAG_INIT;
    struct Guard { std::atomic_flag& f; ~Guard(){f.clear(std::memory_order_release);} };
    if(busy.test_and_set(std::memory_order_acquire)) { SetAutoStage(AutoStage::Busy); return false; }
    Guard guard{busy};

    compactness=std::clamp(compactness,0.30f,1.25f);
    center_x=std::clamp(center_x,-0.20f,0.20f);
    center_y=std::clamp(center_y,-0.20f,0.20f);

    HudHolder holder{}; void* hud=nullptr;
    const auto seen=g_last_hud_tick.load(std::memory_order_acquire);
    const auto now=GetTickCount64();
    void* observed=g_last_hud.load(std::memory_order_acquire);
    if(observed && SafePtr(observed) && seen && now>=seen && now-seen<=30000ull) hud=observed;
    if(!hud && !AcquireHudDirect(holder,hud)) { SetAutoStage(AutoStage::AcquireHud); ReleaseHudHolder(holder); return false; }

    void* clip=nullptr; void* getTarget=nullptr; void* setTarget=nullptr;
    if(!FindMovieClip(hud,"MasterSwitch",clip,getTarget,setTarget)) { SetAutoStage(AutoStage::FindMasterSwitch); ReleaseHudHolder(holder); return false; }
    using DisplayFn=void(__fastcall*)(void*,void*);
    alignas(16) unsigned char current[0x38]{};
    if(!CallFlashDisplayFnSEH(getTarget,clip,current)) { SetAutoStage(AutoStage::GetDisplayInfo); ReleaseHudHolder(holder); return false; }

    static void* baseline_clip=nullptr;
    static std::array<unsigned char,0x38> baseline{};
    static bool baseline_valid=false;
    if(!baseline_valid || baseline_clip!=clip) {
        float x=0,y=0,xs=0,ys=0;
        std::memcpy(&x,current+0x00,sizeof(float)); std::memcpy(&y,current+0x04,sizeof(float));
        std::memcpy(&xs,current+0x0C,sizeof(float)); std::memcpy(&ys,current+0x10,sizeof(float));
        if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(xs)||!std::isfinite(ys)||std::fabs(xs)>1000.0f||std::fabs(ys)>1000.0f) {
            SetAutoStage(AutoStage::BaselineInvalid); ReleaseHudHolder(holder); return false;
        }
        std::memcpy(baseline.data(),current,sizeof(current));
        baseline_clip=clip; baseline_valid=true;
        debug_trace::Write("V64.1R-UI-AUTO captured exact MasterSwitch DisplayInfo baseline");
    }

    alignas(16) unsigned char target[0x38]{};
    if(!enabled) {
        if(baseline_valid && baseline_clip==clip) std::memcpy(target,baseline.data(),sizeof(target));
        else { SetAutoStage(AutoStage::Ok); ReleaseHudHolder(holder); return true; }
    } else {
        std::memcpy(target,baseline.data(),sizeof(target));
        float bx=0.0f,by=0.0f,bxs=100.0f,bys=100.0f;
        std::memcpy(&bx,baseline.data()+0x00,sizeof(float));
        std::memcpy(&by,baseline.data()+0x04,sizeof(float));
        std::memcpy(&bxs,baseline.data()+0x0C,sizeof(float));
        std::memcpy(&bys,baseline.data()+0x10,sizeof(float));
        const float gx=960.0f, gy=540.0f;
        const float cx=gx + center_x*1920.0f;
        const float cy=gy + center_y*1080.0f;
        const float nx=cx + compactness*(bx-gx);
        const float ny=cy + compactness*(by-gy);
        const float nxs=bxs*compactness, nys=bys*compactness;
        std::memcpy(target+0x00,&nx,sizeof(float)); std::memcpy(target+0x04,&ny,sizeof(float));
        std::memcpy(target+0x0C,&nxs,sizeof(float)); std::memcpy(target+0x10,&nys,sizeof(float));
        std::uint16_t flags=0; std::memcpy(&flags,target+0x34,sizeof(flags));
        flags=static_cast<std::uint16_t>(flags|0x01u|0x02u|0x08u|0x10u); std::memcpy(target+0x34,&flags,sizeof(flags));
    }
    bool ok=CallFlashDisplayFnSEH(setTarget,clip,target);
    if(!ok) { SetAutoStage(AutoStage::SetDisplayInfo); ReleaseHudHolder(holder); return false; }

    alignas(16) unsigned char verify[0x38]{};
    bool readback=CallFlashDisplayFnSEH(getTarget,clip,verify);
    if(readback) {
        float tx=0,ty=0,tsx=0,tsy=0,vx=0,vy=0,vsx=0,vsy=0;
        std::memcpy(&tx,target+0x00,4);std::memcpy(&ty,target+0x04,4);std::memcpy(&tsx,target+0x0C,4);std::memcpy(&tsy,target+0x10,4);
        std::memcpy(&vx,verify+0x00,4);std::memcpy(&vy,verify+0x04,4);std::memcpy(&vsx,verify+0x0C,4);std::memcpy(&vsy,verify+0x10,4);
        readback=std::isfinite(vx)&&std::isfinite(vy)&&std::isfinite(vsx)&&std::isfinite(vsy)&&
                 std::fabs(vx-tx)<=0.5f&&std::fabs(vy-ty)<=0.5f&&std::fabs(vsx-tsx)<=0.25f&&std::fabs(vsy-tsy)<=0.25f;
    }
    ReleaseHudHolder(holder);
    if(!readback) { SetAutoStage(AutoStage::ReadbackMismatch); return false; }
    SetAutoStage(AutoStage::Ok);
    char msg[256]{}; std::snprintf(msg,sizeof(msg),
        "V64.1R-UI-AUTO enabled=%d compact=%.2f center=(%+.4f,%+.4f) root=MasterSwitch readback=matched",
        enabled?1:0,double(compactness),double(center_x),double(center_y));
    debug_trace::Write(msg);
    return true;
}



bool ApplyCorrectedMonoGlobalUi(bool enabled,float scaleX,float scaleY,float centerX,float centerY) noexcept {
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    static std::atomic_flag busy=ATOMIC_FLAG_INIT;
    struct Guard { std::atomic_flag& f; ~Guard(){f.clear(std::memory_order_release);} };
    if(busy.test_and_set(std::memory_order_acquire)) return false;
    Guard guard{busy};
    scaleX=std::clamp(scaleX,0.30f,1.25f); scaleY=std::clamp(scaleY,0.30f,1.25f);
    centerX=std::clamp(centerX,200.0f,1720.0f); centerY=std::clamp(centerY,120.0f,960.0f);

    // V65.03: one mathematical NativeUI -> CorrectedMono transform, applied
    // independently to each retail UIElement movie root. HUD MasterSwitch is NOT
    // the parent of ItemTransfer/ItemSelection/AlchemyBook in retail.
    struct RootBaseline { void* clip=nullptr; std::array<unsigned char,0x38> info{}; bool valid=false; };
    static RootBaseline baseline[4]{};
    static constexpr const char* hudRoots[]={"MasterSwitch"};
    static constexpr const char* itemSelectionRoots[]={"_root","root","Root","ItemSelection"};
    static constexpr const char* itemTransferRoots[]={"_root","root","Root","ItemTransfer"};
    static constexpr const char* alchemyRoots[]={"_root","root","Root","AlchemyBook"};

    auto applyOne=[&](int slot,const char* element,const char* const* roots,std::size_t rootCount,bool required,unsigned bit,unsigned& mask)->bool {
        HudHolder holder{}; void* el=nullptr;
        if(slot==0) {
            const auto seen=g_last_hud_tick.load(std::memory_order_acquire);
            const auto now=GetTickCount64();
            void* observed=g_last_hud.load(std::memory_order_acquire);
            if(observed && SafePtr(observed) && seen && now>=seen && now-seen<=30000ull) el=observed;
            if(!el && !AcquireHudDirect(holder,el)) { ReleaseHudHolder(holder); return !required; }
        } else if(!AcquireElementDirect(holder,element,el)) { ReleaseHudHolder(holder); return !required; }
        void* clip=nullptr; void* getTarget=nullptr; void* setTarget=nullptr; const char* chosen=nullptr;
        for(std::size_t i=0;i<rootCount && !clip;++i) {
            if(FindMovieClip(el,roots[i],clip,getTarget,setTarget)) chosen=roots[i];
        }
        if(!clip) { ReleaseHudHolder(holder); return !required; }
        alignas(16) unsigned char current[0x38]{};
        if(!CallFlashDisplayFnSEH(getTarget,clip,current)) { ReleaseHudHolder(holder); return !required; }
        auto& b=baseline[slot];
        if(!b.valid || b.clip!=clip) {
            float x=0,y=0,xs=0,ys=0; std::memcpy(&x,current+0x00,4); std::memcpy(&y,current+0x04,4); std::memcpy(&xs,current+0x0C,4); std::memcpy(&ys,current+0x10,4);
            if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(xs)||!std::isfinite(ys)) { ReleaseHudHolder(holder); return !required; }
            std::memcpy(b.info.data(),current,sizeof(current)); b.clip=clip; b.valid=true;
            char bm[256]{}; std::snprintf(bm,sizeof(bm),"V65.03-GLOBAL-UI baseline element=%s root=%s clip=%p",element,chosen?chosen:"?",clip); diagnostic_trace::Write(bm);
        }
        alignas(16) unsigned char target[0x38]{}; std::memcpy(target,b.info.data(),sizeof(target));
        if(enabled) {
            float bx=0,by=0,bxs=100,bys=100; std::memcpy(&bx,b.info.data()+0x00,4); std::memcpy(&by,b.info.data()+0x04,4); std::memcpy(&bxs,b.info.data()+0x0C,4); std::memcpy(&bys,b.info.data()+0x10,4);
            const float nx=centerX+scaleX*(bx-960.0f), ny=centerY+scaleY*(by-540.0f);
            const float nxs=bxs*scaleX,nys=bys*scaleY;
            std::memcpy(target+0x00,&nx,4); std::memcpy(target+0x04,&ny,4); std::memcpy(target+0x0C,&nxs,4); std::memcpy(target+0x10,&nys,4);
            std::uint16_t flags=0; std::memcpy(&flags,target+0x34,2); flags=static_cast<std::uint16_t>(flags|0x01u|0x02u|0x08u|0x10u); std::memcpy(target+0x34,&flags,2);
        }
        const bool ok=CallFlashDisplayFnSEH(setTarget,clip,target); if(ok) mask|=bit;
        ReleaseHudHolder(holder); return ok || !required;
    };

    unsigned mask=0;
    const bool hudOk=applyOne(0,"hud",hudRoots,1,true,1u<<0,mask);
    (void)applyOne(1,"ItemSelection",itemSelectionRoots,4,false,1u<<1,mask);
    (void)applyOne(2,"ItemTransfer",itemTransferRoots,4,false,1u<<2,mask);
    (void)applyOne(3,"AlchemyBook",alchemyRoots,4,false,1u<<3,mask);
    if(hudOk) {
        char m[448]{}; std::snprintf(m,sizeof(m),"V65.03-GLOBAL-UI %s rootsMask=0x%X scale=(%.4f,%.4f) center=(%.1f,%.1f) sharedMath=1 exactBaselineRestore=1",
            enabled?"ON":"OFF/RESTORED",mask,double(scaleX),double(scaleY),double(centerX),double(centerY)); diagnostic_trace::Write(m); debug_trace::Write(m);
    }
    return hudOk;
}



bool ApplyCorrectedMonoGlobalUiOverlayExclusions(bool enabled,float uniformScale,float centerX,float centerY) noexcept {
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    // V66.118R1: preserve V65.22's one shared MasterSwitch transform, but
    // counter-transform only known fullscreen field effects.  Their apparent
    // world-space geometry therefore stays at the native HUD baseline while
    // readable HUD descendants inherit the historical AutoHUD fit.
    static std::atomic_flag busy=ATOMIC_FLAG_INIT;
    struct Guard { std::atomic_flag& f; ~Guard(){f.clear(std::memory_order_release);} };
    if(busy.test_and_set(std::memory_order_acquire)) return false;
    Guard guard{busy};

    uniformScale=std::clamp(uniformScale,0.30f,1.25f);
    centerX=std::clamp(centerX,200.0f,1720.0f);
    centerY=std::clamp(centerY,120.0f,960.0f);

    struct Baseline {
        const char* name=nullptr;
        void* clip=nullptr;
        std::array<unsigned char,0x38> info{};
        bool valid=false;
    };
    static Baseline b[]={
        {"Fader"},{"fader"},
        {"Vignette"},{"vignette"},
        {"RatioStrips"},{"ratioStrips"},
        {"PopUpBackground"},{"popUp"},{"PopUp"},{"popup"}
    };

    HudHolder holder{}; void* hud=nullptr;
    if(!AcquireHudDirect(holder,hud)) {
        const auto seen=g_last_hud_tick.load(std::memory_order_acquire);
        const auto now=GetTickCount64();
        void* observed=g_last_hud.load(std::memory_order_acquire);
        if(observed && SafePtr(observed) && seen && now>=seen && now-seen<=30000ull) hud=observed;
    }
    if(!hud) { ReleaseHudHolder(holder); return false; }

    bool any=false;
    unsigned applied=0;
    for(auto& e:b) {
        void* clip=nullptr; void* getTarget=nullptr; void* setTarget=nullptr;
        if(!FindMovieClip(hud,e.name,clip,getTarget,setTarget) || !clip) continue;
        alignas(16) unsigned char current[0x38]{};
        if(!CallFlashDisplayFnSEH(getTarget,clip,current)) continue;
        if(!e.valid || e.clip!=clip) {
            float x=0,y=0,xs=0,ys=0;
            std::memcpy(&x,current+0x00,4); std::memcpy(&y,current+0x04,4);
            std::memcpy(&xs,current+0x0C,4); std::memcpy(&ys,current+0x10,4);
            if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(xs)||!std::isfinite(ys)) continue;
            std::memcpy(e.info.data(),current,sizeof(current));
            e.clip=clip; e.valid=true;
        }
        alignas(16) unsigned char target[0x38]{};
        std::memcpy(target,e.info.data(),sizeof(target));
        if(enabled) {
            float bx=0,by=0,bxs=100,bys=100;
            std::memcpy(&bx,e.info.data()+0x00,4); std::memcpy(&by,e.info.data()+0x04,4);
            std::memcpy(&bxs,e.info.data()+0x0C,4); std::memcpy(&bys,e.info.data()+0x10,4);
            // Inverse of V65.22 parent map:
            // world = center + s * (local - authoredCenter).
            const float inv=1.0f/uniformScale;
            const float nx=960.0f + (bx-centerX)*inv;
            const float ny=540.0f + (by-centerY)*inv;
            const float nxs=bxs*inv, nys=bys*inv;
            std::memcpy(target+0x00,&nx,4); std::memcpy(target+0x04,&ny,4);
            std::memcpy(target+0x0C,&nxs,4); std::memcpy(target+0x10,&nys,4);
            std::uint16_t flags=0; std::memcpy(&flags,target+0x34,2);
            flags=static_cast<std::uint16_t>(flags|0x01u|0x02u|0x08u|0x10u);
            std::memcpy(target+0x34,&flags,2);
        }
        if(CallFlashDisplayFnSEH(setTarget,clip,target)) { any=true; ++applied; }
    }
    ReleaseHudHolder(holder);
    char m[320]{};
    std::snprintf(m,sizeof(m),
        "V66.118R1-AUTOHUD-OVERLAYS %s applied=%u scale=%.4f center=(%.1f,%.1f) policy=FULLSCREEN-NATIVE",
        enabled?"COUNTERTRANSFORM":"RESTORED",applied,double(uniformScale),double(centerX),double(centerY));
    diagnostic_trace::Write(m);
    return any || !enabled;
}


const char* LastAutoLayoutStageName() noexcept { return AutoStageName(g_auto_stage.load(std::memory_order_acquire)); }

bool ApplyHintsLayout(float hintsScale, float hintsInsetPct, float hintsY, float anchorOffsetX, float anchorOffsetY) noexcept {
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    // In native-canvas mode br owns the transform. New actionHints descendants
    // inherit it; a per-child reassert would double-transform or undo that fit.
    if(g_canvas_requested)return true;

    hintsScale=std::clamp(hintsScale,0.25f,1.50f);
    hintsInsetPct=std::clamp(hintsInsetPct,0.0f,75.0f);
    hintsY=std::clamp(hintsY,-600.0f,600.0f);
    const float hintInset=960.0f*hintsInsetPct*0.01f;
    const float hintYPos=-120.0f+hintsY+anchorOffsetY;
    HudHolder holder{}; void* hud=nullptr;
    if(!AcquireHudDirect(holder,hud)) {
        const auto seen=g_last_hud_tick.load(std::memory_order_acquire);
        const auto now=GetTickCount64();
        void* observed=g_last_hud.load(std::memory_order_acquire);
        if(observed && SafePtr(observed) && seen && now>=seen && now-seen<=30000ull) hud=observed;
    }
    if(!hud) { ReleaseHudHolder(holder); return false; }
    bool ok=false;
    if(g_safe_hud_active&&g_targets_generation==g_hud_generation.load(std::memory_order_acquire)){
        for(const auto& target:g_vrhud_targets)if(target.valid&&std::strcmp(target.name,"Hints")==0){
            void* clip=nullptr;void* get=nullptr;void* set=nullptr;
            if(FindMovieClip(hud,"Hints",clip,get,set)){
                auto info=ui_hud_geometry::Pack(target.x,target.y,target.xs,target.ys);
                ok=CallFlashDisplayFnSEH(set,clip,info.data());
            }
            break;
        }
    }else if(!g_safe_hud_active) ok=ApplyMovieClipDisplay(hud,"Hints",-35.0f-hintInset+anchorOffsetX,hintYPos,hintsScale);
    ReleaseHudHolder(holder);
    return ok;
}

bool ApplyVrLayout(bool enabled,
                   float subtitleScale,float subtitleY,
                   float dialogueScale,float dialogueY,float dialogueInsetPct,
                   float hintsScale,float hintsInsetPct,float hintsY) noexcept {
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    subtitleScale=std::clamp(subtitleScale,0.30f,1.25f);
    subtitleY=std::clamp(subtitleY,-600.0f,600.0f);
    dialogueScale=std::clamp(dialogueScale,0.30f,1.50f);
    dialogueY=std::clamp(dialogueY,-720.0f,720.0f);
    dialogueInsetPct=std::clamp(dialogueInsetPct,0.0f,80.0f);
    hintsScale=std::clamp(hintsScale,0.25f,1.50f);
    hintsInsetPct=std::clamp(hintsInsetPct,0.0f,75.0f);
    hintsY=std::clamp(hintsY,-600.0f,600.0f);

    const float ss=enabled?subtitleScale:1.0f;
    const float ds=enabled?dialogueScale:1.0f;
    const float hs=enabled?hintsScale:1.0f;
    const float subY=-230.0f+(enabled?subtitleY:0.0f);
    const float dialogInset=enabled?(960.0f*dialogueInsetPct*0.01f):0.0f;
    const float dialogYPos=540.0f+(enabled?dialogueY:0.0f);
    const float hintInset=enabled?(960.0f*hintsInsetPct*0.01f):0.0f;
    const float hintYPos=-120.0f+(enabled?hintsY:0.0f);

    HudHolder holder{}; void* hud=nullptr;
    // Prefer a HUD object observed on the game's own GetElement call path (the
    // same technique that already proved CombatCursorCross SetVisible). Fall
    // back to the exact retail GetElement/holder sequence only when no live HUD
    // has been observed yet.
    // V62.7: prefer a freshly acquired HUD instance. Inventory/Apse transitions
    // can destroy and recreate MovieClips while an observed pointer still looks
    // address-valid. Direct reacquisition prevents reapplying to a stale object.
    if(!AcquireHudDirect(holder,hud)) {
        const auto seen=g_last_hud_tick.load(std::memory_order_acquire);
        const auto now=GetTickCount64();
        void* observed=g_last_hud.load(std::memory_order_acquire);
        if(observed && SafePtr(observed) && seen && now>=seen && now-seen<=30000ull) hud=observed;
    }
    if(!hud) { ReleaseHudHolder(holder); return false; }
    bool ok=true;
    ok=ApplyMovieClipDisplay(hud,"Subtitles",-480.0f,subY,ss)&&ok;
    ok=ApplyMovieClipDisplay(hud,"DialogLeft",dialogInset,dialogYPos,ds)&&ok;
    ok=ApplyMovieClipDisplay(hud,"DialogRight",1920.0f-dialogInset,dialogYPos,ds)&&ok;
    ok=ApplyMovieClipDisplay(hud,"Hints",-35.0f-hintInset,hintYPos,hs)&&ok;
    ReleaseHudHolder(holder);
    return ok;
}



namespace {
struct ManualClip36 {const char* name=nullptr;void* clip=nullptr;void* get=nullptr;void* set=nullptr;ui_hud_visibility175::Key key{};};
std::array<ManualClip36,24> manualClips36{};
void* manualHud36=nullptr;std::uint64_t manualGeneration36=~0ull;
bool FindManualClip36(void* hud,std::uint64_t generation,const char* name,void*&clip,void*&get,void*&set) noexcept {
    if(!g_corrected36.load())return FindMovieClip(hud,name,clip,get,set);
    const CanvasPhaseCostP1 phase;
    if(manualHud36!=hud||manualGeneration36!=generation){manualClips36={};manualHud36=hud;manualGeneration36=generation;}
    ManualClip36* free=nullptr;
    for(auto&e:manualClips36){
        if(!e.name){if(!free)free=&e;continue;}
        if(std::strcmp(e.name,name)!=0)continue;
        if(ValidHudKey175(e.key)){clip=e.clip;get=e.get;set=e.set;
            if(phase.capture.serial&&phase.capture.current())g_canvas_resolve_hits178r27.fetch_add(1,std::memory_order_relaxed);return true;}
        e={};free=&e;break;
    }
    const bool ok=FindMovieClip(hud,name,clip,get,set);
    if(phase.Record(g_canvas_resolve_us178r27))g_canvas_resolve_misses178r27.fetch_add(1,std::memory_order_relaxed);
    ui_hud_visibility175::Key key{};
    if(ok&&free&&ReadHudKey175(clip,hud,generation,key))*free={name,clip,get,set,key};
    return ok;
}
}
bool ManualDisplay36(void* fn,void*clip,unsigned char*info,bool get) noexcept {
    if(!g_corrected36.load())return CallFlashDisplayFnSEH(fn,clip,info);
    const CanvasPhaseCostP1 phase;const bool ok=CallFlashDisplayFnSEH(fn,clip,info);
    (void)phase.Record(get?g_canvas_get_us178r27:g_canvas_set_us178r27);return ok;
}
bool ApplyVrAngularSafeLayout(bool enabled,float safeLeft,float safeTop,float safeRight,float safeBottom,float globalScale,float anchorOffsetX,float anchorOffsetY,
                              float subtitleY,float dialogueScale,float dialogueY,float dialogueInsetPct,
                              float hintsScale,float hintsInsetPct,float hintsY,
                              float controlPromptScale,float controlPromptX,float controlPromptY,
                              float compassScale,float compassY,float statsScale,float statsX,float statsY,
                              float lowerScale,float lowerY,float lowerSpread,float fixedCenterY) noexcept {
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    // V64.77: native-baseline semantic HUD ownership.  The OpenXR safe rectangle
    // defines only the VR comfort region; each KCD2 MovieClip keeps its live native
    // DisplayInfo and receives a local delta from that baseline.  Never reconstruct
    // absolute "vanilla" coordinates here: KCD2 is allowed to own/recreate layout.
    safeLeft=std::clamp(safeLeft,0.0f,960.0f);
    safeRight=std::clamp(safeRight,960.0f,1920.0f);
    safeTop=std::clamp(safeTop,-1080.0f,540.0f);
    safeBottom=std::clamp(safeBottom,540.0f,2160.0f);
    globalScale=std::clamp(globalScale,0.25f,1.20f);
    anchorOffsetX=std::clamp(anchorOffsetX,-500.0f,500.0f);
    anchorOffsetY=std::clamp(anchorOffsetY,-300.0f,300.0f);
    subtitleY=std::clamp(subtitleY,-600.0f,600.0f);
    dialogueScale=std::clamp(dialogueScale,0.30f,1.50f);
    dialogueY=std::clamp(dialogueY,-720.0f,720.0f);
    dialogueInsetPct=std::clamp(dialogueInsetPct,0.0f,80.0f);
    hintsScale=std::clamp(hintsScale,0.25f,1.50f);
    hintsInsetPct=std::clamp(hintsInsetPct,0.0f,75.0f);
    hintsY=std::clamp(hintsY,-600.0f,600.0f);
    controlPromptScale=std::clamp(controlPromptScale,0.30f,1.50f);
    controlPromptX=std::clamp(controlPromptX,-700.0f,700.0f);
    controlPromptY=std::clamp(controlPromptY,-600.0f,600.0f);
    compassScale=std::clamp(compassScale,0.15f,1.50f);
    compassY=std::clamp(compassY,-300.0f,300.0f);
    statsScale=std::clamp(statsScale,0.15f,1.50f);
    statsX=std::clamp(statsX,-700.0f,700.0f);
    statsY=std::clamp(statsY,-20.0f,60.0f);
    lowerScale=std::clamp(lowerScale,0.15f,2.00f);
    lowerY=std::clamp(lowerY,-500.0f,500.0f);
    lowerSpread=std::clamp(lowerSpread,-400.0f,400.0f);
    if(!(safeRight-safeLeft>200.0f) || !(safeBottom-safeTop>160.0f)) return false;

    HudHolder holder{}; void* hud=nullptr;
    if(!AcquireHudDirect(holder,hud)) {
        const auto seen=g_last_hud_tick.load(std::memory_order_acquire);
        const auto now=GetTickCount64();
        void* observed=g_last_hud.load(std::memory_order_acquire);
        if(observed && SafePtr(observed) && seen && now>=seen && now-seen<=30000ull) hud=observed;
    }
    if(!hud) { ReleaseHudHolder(holder); return false; }

    const std::uint64_t generation=g_hud_generation.load(std::memory_order_acquire);
    if(g_targets_generation!=generation){for(auto& t:g_vrhud_targets)t.valid=false;g_targets_generation=generation;g_safe_hud_active=false;}
    const float centerX=0.5f*(safeLeft+safeRight);
    const float centerY=std::isfinite(fixedCenterY)?std::clamp(fixedCenterY,0.f,1080.f):540.f;
    const float dxCenter=centerX-960.0f;
    const float dyCenter=centerY-540.0f;
    const float dxLeft=safeLeft;
    const float dxRight=safeRight-1920.0f;
    const float dyTop=safeTop;
    const float dyBottom=safeBottom-1080.0f;
    const float halfSafeW=0.5f*(safeRight-safeLeft);
    const float dialogInsetPx=halfSafeW*(dialogueInsetPct*0.01f);
    const float hintsInsetPx=halfSafeW*(hintsInsetPct*0.01f);
    const float compassYPx=-compassY; // V64.85 UI value is design px; positive = upward
    const float statsYPx=-1080.0f*(statsY*0.01f);
    const float lowerYPx=-lowerY; // V64.85 design px
    const float lowerSpreadPx=lowerSpread; // V64.85 design px, symmetric

    auto baseline_for=[&](const char* name)->VrHudBaseline* {
        for(auto& b:g_vrhud_baselines) if(std::strcmp(b.name,name)==0) return &b;
        return nullptr;
    };
    auto target_for=[&](const char* name)->VrHudTarget* {
        for(auto& t:g_vrhud_targets) if(std::strcmp(t.name,name)==0) return &t;
        return nullptr;
    };
    auto capture=[&](const char* name,void*& clip,void*& getTarget,void*& setTarget)->VrHudBaseline* {
        clip=nullptr; getTarget=nullptr; setTarget=nullptr;
        if(!FindManualClip36(hud,generation,name,clip,getTarget,setTarget)) return nullptr;
        auto* b=baseline_for(name); if(!b) return nullptr;
        if(!b->valid || b->generation!=generation || b->clip!=clip) {
            alignas(16) unsigned char current[0x38]{};
            if(!ManualDisplay36(getTarget,clip,current,true)) return nullptr;
            float x=0,y=0,xs=0,ys=0;
            std::memcpy(&x,current+0x00,4); std::memcpy(&y,current+0x04,4);
            std::memcpy(&xs,current+0x0C,4); std::memcpy(&ys,current+0x10,4);
            if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(xs)||!std::isfinite(ys)) return nullptr;
            // New wrapper identity is not automatically a new native baseline:
            // if it still carries our last target, keep the original geometry.
            const auto* prior=target_for(name);
            const bool still_ours=b->valid&&b->generation==generation&&prior&&prior->valid&&
                std::fabs(x-prior->x)<.05f&&std::fabs(y-prior->y)<.05f&&std::fabs(xs-prior->xs)<.05f&&std::fabs(ys-prior->ys)<.05f;
            if(!still_ours)std::memcpy(b->info.data(),current,sizeof(current));
            b->valid=true;b->generation=generation;b->clip=clip;

        }
        return b;
    };
    unsigned dispatchedMask=0;bool dispatchFailed=false;
    auto apply_from_native=[&](const char* name,float dx,float dy,float scaleMul,bool required)->bool {
        void* clip=nullptr; void* getTarget=nullptr; void* setTarget=nullptr;
        auto* b=capture(name,clip,getTarget,setTarget);
        if(!b) return !required;
        float bx=0,by=0,bxs=100,bys=100;
        std::memcpy(&bx,b->info.data()+0x00,4); std::memcpy(&by,b->info.data()+0x04,4);
        std::memcpy(&bxs,b->info.data()+0x0C,4); std::memcpy(&bys,b->info.data()+0x10,4);
        const float factor=std::clamp(scaleMul,.15f,2.f);
        const float finalScale=.01f*.5f*(std::fabs(bxs)+std::fabs(bys))*factor;
        const float x=bx+dx+anchorOffsetX,y=by+dy+anchorOffsetY;
        const float xs=bxs*factor,ys=bys*factor;
        auto target=ui_hud_geometry::Pack(x,y,xs,ys);
        const bool moved=ManualDisplay36(setTarget,clip,target.data(),false);
        if(moved){if(auto* t=target_for(name)){
            dispatchedMask|=1u<<static_cast<unsigned>(t-g_vrhud_targets.data());
            *t={t->name,x,y,finalScale,true,xs,ys};
        }}else dispatchFailed=true;
        return moved;
    };
    auto restore_native=[&](const char* name,bool required)->bool {
        void* clip=nullptr; void* getTarget=nullptr; void* setTarget=nullptr;
        if(!FindMovieClip(hud,name,clip,getTarget,setTarget)) return !required;
        auto* b=baseline_for(name);
        if(!b || !b->valid || b->generation!=generation) return !required;
        auto restore=ui_hud_geometry::Restore(b->info);
        const bool restored=ManualDisplay36(setTarget,clip,restore.data(),false);
        if(auto* t=target_for(name)) t->valid=false;
        return restored;
    };

    bool ok=true;
    if(enabled) {
        // Context-dependent core clips. Each correction is a delta from the exact native
        // DisplayInfo captured for this HUD generation.
        ok=apply_from_native("Subtitles",dxCenter,dyBottom+subtitleY,globalScale,false)&&ok;
        ok=apply_from_native("DialogLeft",dxLeft+dialogInsetPx,dyCenter+dialogueY,dialogueScale,false)&&ok;
        ok=apply_from_native("DialogRight",dxRight-dialogInsetPx,dyCenter+dialogueY,dialogueScale,false)&&ok;
        ok=apply_from_native("Hints",dxRight-hintsInsetPx,dyBottom+hintsY,hintsScale,false)&&ok;

        // Optional semantic groups.  Missing clips are normal for transient HUD
        // states; when KCD2 recreates the HUD they are captured on the next event/
        // settings reapply instead of inheriting stale absolute coordinates.
        apply_from_native("Compass",dxCenter,dyTop+compassYPx,compassScale,false);
        // Combat cursor retains its native geometry in every HUD layout.
        apply_from_native("TutorialInDialog",dxCenter,dyCenter+dialogueY,dialogueScale,false);
        // V64.86: separate tutorial/control prompt layer from contextual action hints.
        // This catches large help/control cards without coupling them to br.actionHints.
        apply_from_native("TutorialMessage",dxLeft+controlPromptX,dyTop+controlPromptY,controlPromptScale,false);
        apply_from_native("Stats",dxCenter+statsX,dyBottom+statsYPx,statsScale,false);
        // V64.79: lower quick-action icons are independent again.  Preserve each
        // native anchor and apply only the dedicated lower Y/scale/spread controls.
        apply_from_native("QAMWeapon",dxCenter-lowerSpreadPx,dyBottom+lowerYPx,lowerScale,false);
        apply_from_native("QAMFood",dxCenter+lowerSpreadPx,dyBottom+lowerYPx,lowerScale,false);

        // Sibling groups from retail hud.xml. Nested health/horse/opponent bars
        // inherit Stats; compass children inherit Compass. Never scale them twice.
        for(const char* n:{"Crime","Wanted","Trespassing","FancyEvent"})apply_from_native(n,dxCenter,dyTop,globalScale,false);
        for(const char* n:{"GameLog","CommonEvent"})apply_from_native(n,dxRight,dyTop,globalScale,false);
        for(const char* n:{"SkillCheck","Buffs"})apply_from_native(n,dxCenter,dyBottom,globalScale,false);
        for(const char* n:{"ItemTransfer","ShootingContest","DiceContainer"})apply_from_native(n,dxLeft,dyBottom,globalScale,false);
        apply_from_native("InfoText",dxCenter,dyCenter,globalScale,false);
        // DiceCursor follows native per-die projection in 1920x1080 coordinates.
        // Never cache its moving position or apply ordinary HUD scale/offsets.
        // MasterSwitch, Fader, Vignette, RatioStrips, PopUpBackground are untouched.
        // World-projected speech bubbles and the native action cursor retain their
        // native positioning; their descendants are not blindly forced visible.
        unsigned capturedMask=0, targetMask=0;
        for(std::size_t i=0;i<g_vrhud_baselines.size();++i) {
            if(g_vrhud_baselines[i].valid && g_vrhud_baselines[i].generation==generation) capturedMask|=(1u<<i);
            if(g_vrhud_targets[i].valid) targetMask|=(1u<<i);
        }
        ok=dispatchedMask!=0&&!dispatchFailed;
        if(g_corrected36.load()){char coverage[192]{};std::snprintf(coverage,sizeof(coverage),"V66.178R36C5 HUD_MANUAL_COVERAGE resolved=0x%X applied=0x%X expectedContext=OPTIONAL_NATIVE_CLIPS completeNotProven=1",capturedMask,dispatchedMask);diagnostic_trace::Write(coverage);}
        g_safe_hud_active=targetMask!=0;
        char m[640]{};
        std::snprintf(m,sizeof(m),
            "V66.88-SAFE-HUD GEOMETRY-DISPATCH gen=%llu safe=[%.1f,%.1f..%.1f,%.1f] center=(%.1f,%.1f) trim=(%+.0f,%+.0f) coreScale=%.2f hints={s:%.2f inset:%.0f y:%+.0f} prompts={s:%.2f x:%+.0f y:%+.0f} capturedMask=0x%03X targetMask=0x%03X thisPass=0x%06X dispatchResult=%s",
            static_cast<unsigned long long>(generation),double(safeLeft),double(safeTop),double(safeRight),double(safeBottom),
            double(centerX),double(centerY),double(anchorOffsetX),double(anchorOffsetY),double(globalScale),
            double(hintsScale),double(hintsInsetPct),double(hintsY),double(controlPromptScale),double(controlPromptX),double(controlPromptY),
            capturedMask,targetMask,dispatchedMask,ok?"ok":"partial");
        diagnostic_trace::Write(m); debug_trace::Write(m);
    } else {
        // Exact per-generation restoration.  We do not write guessed vanilla
        // coordinates when a baseline is absent or belongs to an older HUD.
        g_safe_hud_active=false;
        for(const auto& baseline:g_vrhud_baselines)ok=restore_native(baseline.name,false)&&ok;
        for(auto& t:g_vrhud_targets) t.valid=false;
        diagnostic_trace::Write(ok?"V64.79-SAFE-HUD exact native baselines restored":"V64.79-SAFE-HUD native baseline restore partial/failed");
    }
    ReleaseHudHolder(holder);
    return ok;
}








bool ApplyWristHudLayout(bool enabled,float statsX,float statsY,float compassX,float compassY,float scale,bool showStats,bool showCompass) noexcept {
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    scale=std::clamp(scale,0.35f,1.35f);
    HudHolder holder{}; void* hud=nullptr;
    if(!AcquireHudDirect(holder,hud)) {
        const auto seen=g_last_hud_tick.load(std::memory_order_acquire); const auto now=GetTickCount64();
        void* observed=g_last_hud.load(std::memory_order_acquire);
        if(observed && SafePtr(observed) && seen && now>=seen && now-seen<=30000ull) hud=observed;
    }
    if(!hud){ReleaseHudHolder(holder);return false;}
    const auto generation=g_hud_generation.load(std::memory_order_acquire);
    auto baseline_for=[&](const char* name)->VrHudBaseline*{for(auto& b:g_vrhud_baselines)if(std::strcmp(b.name,name)==0)return &b;return nullptr;};
    auto capture=[&](const char* name)->VrHudBaseline*{
        void* clip=nullptr;void* getTarget=nullptr;void* setTarget=nullptr;
        if(!FindMovieClip(hud,name,clip,getTarget,setTarget))return nullptr;
        auto* b=baseline_for(name);if(!b)return nullptr;
        if(!b->valid||b->generation!=generation){alignas(16) unsigned char cur[0x38]{};if(!CallFlashDisplayFnSEH(getTarget,clip,cur))return nullptr;std::memcpy(b->info.data(),cur,sizeof(cur));b->valid=true;b->generation=generation;}
        return b;
    };
    auto restore=[&](const char* name)->bool{void* clip=nullptr;void* gt=nullptr;void* st=nullptr;if(!FindMovieClip(hud,name,clip,gt,st))return false;auto* b=baseline_for(name);if(!b||!b->valid||b->generation!=generation)return false;auto info=ui_hud_geometry::Restore(b->info);return CallFlashDisplayFnSEH(st,clip,info.data());};
    bool ok=true;
    if(!enabled){ok=restore("Stats")&&ok;ok=restore("Compass")&&ok;ReleaseHudHolder(holder);return ok;}
    auto move=[&](const char* name,float x,float y,bool show)->bool{auto* b=capture(name);if(!b)return false;float bxs=100,bys=100;std::memcpy(&bxs,b->info.data()+0x0C,4);std::memcpy(&bys,b->info.data()+0x10,4);const float ns=0.01f*0.5f*(std::fabs(bxs)+std::fabs(bys));return ApplyMovieClipDisplay(hud,name,show?x:-5000.0f,show?y:-5000.0f,std::clamp(ns*scale,0.25f,1.6f));};
    ok=move("Stats",statsX,statsY,showStats)&&ok; ok=move("Compass",compassX,compassY,showCompass)&&ok;
    ReleaseHudHolder(holder);return ok;
}

bool SetHudMovieClipVisible(const char* name, bool visible,
    std::uint64_t expectedGeneration,std::uint64_t* appliedGeneration) noexcept {
    if(appliedGeneration)*appliedGeneration=0;
    if(!retail_build112::Ready(WhBase())||!name||!*name)return false;
    HudHolder holder{}; void* hud=nullptr;
    // V62.7: prefer a freshly acquired HUD instance. Inventory/Apse transitions
    // can destroy and recreate MovieClips while an observed pointer still looks
    // address-valid. Direct reacquisition prevents reapplying to a stale object.
    if(!AcquireHudDirect(holder,hud)) {
        const auto seen=g_last_hud_tick.load(std::memory_order_acquire);
        const auto now=GetTickCount64();
        void* observed=g_last_hud.load(std::memory_order_acquire);
        if(observed && SafePtr(observed) && seen && now>=seen && now-seen<=30000ull) hud=observed;
    }
    if(!hud) { ReleaseHudHolder(holder); return false; }
    const auto generationD79=HudGeneration();
    if((expectedGeneration&&expectedGeneration!=generationD79)||
       (appliedGeneration&&(!generationD79||g_last_hud.load(std::memory_order_acquire)!=hud))){
        ReleaseHudHolder(holder);return false;
    }

    void** hvt=nullptr; if(!Read(hud,hvt)||!hvt||!SafePtr(hvt)){ ReleaseHudHolder(holder); return false; }
    void* findTarget=nullptr;
    if(!Read(reinterpret_cast<unsigned char*>(hvt)+0x238,findTarget)||!ExecPtr(findTarget)){ ReleaseHudHolder(holder); return false; }
    using FindChildFn=void*(__fastcall*)(void*,const char*,int);
    void* clip=nullptr;
    __try { clip=reinterpret_cast<FindChildFn>(findTarget)(hud,name,0); }
    __except(EXCEPTION_EXECUTE_HANDLER){ clip=nullptr; }
    if(!clip||!SafePtr(clip)){ ReleaseHudHolder(holder); return false; }

    void** cvt=nullptr; if(!Read(clip,cvt)||!cvt||!SafePtr(cvt)){ ReleaseHudHolder(holder); return false; }
    void* visTarget=nullptr;
    if(!Read(reinterpret_cast<unsigned char*>(cvt)+0x108,visTarget)||!ExecPtr(visTarget)){ ReleaseHudHolder(holder); return false; }
    using SetVisibleFn=void(__fastcall*)(void*,bool);
    if(appliedGeneration&&(HudGeneration()!=generationD79||g_last_hud.load(std::memory_order_acquire)!=hud)){
        ReleaseHudHolder(holder);return false;
    }
    bool ok=true;++g_hud_owned_write175;
    __try { reinterpret_cast<SetVisibleFn>(visTarget)(clip,visible); }
    __except(EXCEPTION_EXECUTE_HANDLER){ ok=false; }
    --g_hud_owned_write175;
    if(appliedGeneration){
        ok=ok&&HudGeneration()==generationD79&&g_last_hud.load(std::memory_order_acquire)==hud;
        if(ok)*appliedGeneration=generationD79;
    }
    ReleaseHudHolder(holder);
    char msg[320]{}; std::snprintf(msg,sizeof(msg),
        "V62.4-FLASHUI visibility clip=%s visible=%s ok=%s",name,visible?"true":"false",ok?"true":"false");
    debug_trace::Write(msg);
    return ok;
}


// R98 DICE: the native DiceCursor clip (cccursor.dice) holds two children:
//   cursor = the round zoom-in ring around the die (fc_showDiceCursor gotoAndStop 2)
//   flag   = the description: dice-sides icon with the number + die name (fc_showDiceProperties)
// In VR the ring is drawn into the one flat HUD texture and shows double, so the
// ring child alone is hidden for the whole dice game. The description stays native.
// The R92 swap (hide the whole DiceCursor, draw an own XR ring) is retired: hiding
// the whole clip also hid the description. flatOwnerReady stays false, so the XR
// ring layer in xr_manager is never submitted. Joystick/hand die selection is untouched.
void ServiceDiceMarkerR92() noexcept {
    static bool ringHidden=false;static std::uint64_t ringGeneration=0,previous=0;static int logged=-1;
    const auto now=GetTickCount64();if(now>=previous&&now-previous<16)return;previous=now;
    dice_marker_r92::flatOwnerReady.store(false);
    (void)dice_marker_r92::hidden.publish({});
    const auto& w=kcd2vr::activity_input163::witness175;
    const bool dice=w.ready.load(std::memory_order_acquire)&&w.dice.load(std::memory_order_acquire)!=0&&
        w.diceStart.load(std::memory_order_acquire)!=0;
    const bool hudReady=g_hud_repair_ready175.load()&&retail_build112::Ready(WhBase());
    if(!dice||!hudReady){
        // Dice over (or HUD gone): give the ring back to the native movie.
        if(ringHidden&&(!hudReady||SetHudMovieClipVisible("DiceCursorRing",true,0)))ringHidden=false;
        if(!dice)logged=-1;
        return;
    }
    const auto generation=HudGeneration();
    if(ringHidden&&ringGeneration==generation)return; // only transitions; AS never writes _visible on the ring
    const bool ok=generation&&SetHudMovieClipVisible("DiceCursorRing",false,generation);
    if(ok){ringHidden=true;ringGeneration=generation;}
    if(logged!=int(ok)){logged=int(ok); // one KCD2VR.log line per result change, retried every frame
        diagnostic_trace::Write(ok?"R98 DICE_RING hidden=1 clip=DiceCursorRing(cccursor.dice.cursor) description=NATIVE":
            "R98 DICE_RING hidden=0 reason=HUD clip DiceCursorRing not found yet (companion HUD.xml entry), retrying each frame");
    }
}

// R125: the big combat direction cross (CombatCursorCross) without its direction art (ui_hud_r112::combatCrossR125).
// hide=true: state1/2/3/6/7 of cccursor.actioncursor.CombatCursorCross are set invisible (HUD.xml CombatCrossState*),
// lock + pbslot (opponent lock, perfect block / master strike icon) and the parent stay native, so the game keeps
// showing and moving them. Written again on every HUD generation and every 500 ms (the cross is driven by the game's
// ActionScript, a native re-show lasts at most 0.5 s). hide=false gives the art back once. Nothing latches: a clip not
// found yet (HUD not loaded, old companion HUD.xml) is simply tried again on the next call.
void UpdateCombatCrossArtR125(bool hide,std::uint64_t now) noexcept {
    static bool hidden=false;static std::uint64_t generation=0,last=0;static unsigned loggedMask=~0u;
    const auto& names=ui_hud_r112::kCombatCrossArtR125;
    const bool hudReady=g_hud_repair_ready175.load(std::memory_order_acquire)&&retail_build112::Ready(WhBase());
    const auto current=HudGeneration();
    std::lock_guard<std::recursive_mutex> drawLock(g_hud_draw_mutex162r1);
    if(!hide){
        if(!hidden)return;
        if(current!=generation){hidden=false;return;} // a new HUD movie starts with the native art
        if(!hudReady)return;
        bool all=true;for(const char* n:names)all=SetHudMovieClipVisible(n,true,current)&&all;
        if(all){hidden=false;loggedMask=~0u;diagnostic_trace::Write("R125 COMBAT_CROSS art=NATIVE (VrCombatCrossR125=1)");}
        return;
    }
    if(!hudReady||!current)return;
    if(hidden&&current==generation&&now>=last&&now-last<500ull)return;
    last=now;unsigned mask=0;
    for(unsigned i=0;i<names.size();++i)if(SetHudMovieClipVisible(names[i],false,current))mask|=1u<<i;
    hidden=mask!=0;generation=current;
    if(mask!=loggedMask){loggedMask=mask;char m[220]{};
        std::snprintf(m,sizeof(m),"R125 COMBAT_CROSS art=HIDDEN mask=%02X/1F lock+pbslot=NATIVE%s",mask,
            mask==0x1Fu?"":" (missing clips = old companion HUD.xml or HUD not loaded, retried every 0.5 s)");
        diagnostic_trace::Write(m);}
}

void Uninstall() noexcept {
    dice_marker_r92::flatOwnerReady.store(false);
    (void)dice_marker_r92::hidden.publish({});
    ui_hud_visibility175::Key diceKey{};
    if(DiceKeyR92(diceKey))(void)SetHudMovieClipVisible("DiceCursor",g_diceNativeVisibleR92.load(),diceKey.generation);
    (void)SetHudMovieClipVisible("DiceCursorRing",true,0); // R98: never leave the native dice ring hidden after unload
    for(const char* n:ui_hud_r112::kCombatCrossArtR125)(void)SetHudMovieClipVisible(n,true,0); // R125: cross art back
    (void)g_diceKeyR92.publish({});
    RemovePageObserver19();
    g_hud_repair175.store(false,std::memory_order_release);
    {std::lock_guard<std::recursive_mutex> lock(g_hud_draw_mutex162r1);HudVisibilityBackend170 io{};g_visibility170.Restore(io);}
    RemoveHudRepairHooks175();
    g_last_hud.store(nullptr,std::memory_order_release);
    g_last_hud_tick.store(0,std::memory_order_release);
    g_hud_generation.store(0,std::memory_order_release);
    g_recent_large_menu_mask.store(0,std::memory_order_release);
    g_recent_large_menu_tick.store(0,std::memory_order_release);
    for(auto& t:g_recent_ui_ticks) t.store(0,std::memory_order_release);
    if(g_hook){ (void)g_hook->disable(); g_hook.reset(); }
    g_original=nullptr;
}
}
