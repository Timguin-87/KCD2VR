#include "../../../diagnostics/log_sink_d48.h"
#include "native_input_look_lab.h"
#include "pad_dispatch_d84.h"
#include "vrik/attack_xi178r8.h" // V66.178R13: provenance of our own attack/guard/special symbols
#include "diagnostic_trace.h"
#include "retail_build112.h"

#include <DetourModKit/hook.hpp>
#include <DetourModKit/logger.hpp>
#include <DetourModKit/scan.hpp>

#include <windows.h>
#include <array>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>
#include <utility>
#include <string_view>

namespace kcd2vr::native_input_look_lab {
namespace {
using namespace DetourModKit;
using InputDispatchFn = void(__fastcall*)(std::uintptr_t, std::uintptr_t, char);

std::optional<hook::Hook> g_hook;
InputDispatchFn g_original = nullptr;
std::atomic<bool> g_active{false};
std::atomic<float> g_input_gain{1.0f};
std::atomic<std::uintptr_t> g_controller{0};
std::atomic<char> g_last_flag{0};

constexpr std::ptrdiff_t kStateOff = 0x04;
constexpr std::ptrdiff_t kKeyIdOff = 0x10;
constexpr std::ptrdiff_t kValueOff = 0x18;
constexpr int kChanged = 8;
constexpr int kMouseYaw = 0x10A;
constexpr int kMousePitch = 0x10B;
constexpr std::size_t kEventBytes = 0x30;

// Initial lab conversion. KCD2's mouse axis value is a relative delta, not an
// angle. 560 counts/radian ~= 9.77 counts/degree. This is intentionally a
// single bounded constant for the F4 lab; once the native path is proven we
// can calibrate it from observed native mouse deltas instead of guessing.
constexpr float kCountsPerRadian = 560.0f;
constexpr float kMaxCountsPerDispatch = 40.0f;

alignas(16) std::array<std::uint8_t,kEventBytes> g_yaw_template{};
alignas(16) std::array<std::uint8_t,kEventBytes> g_pitch_template{};
std::atomic<bool> g_yaw_valid{false};
std::atomic<bool> g_pitch_valid{false};
std::atomic_flag g_template_lock = ATOMIC_FLAG_INIT;

// V64.36-PHASE: publish an absolute HMD pose snapshot and derive the relative
// mouse-look delta on KCD2's own CameraInputDispatch thread.  The old bridge
// published one pending delta with store()/exchange(0), which could overwrite
// intermediate XR samples whenever producer and consumer cadences differed.
std::atomic<float> g_latest_yaw{0.0f};
std::atomic<float> g_latest_pitch{0.0f};
std::atomic<bool> g_latest_valid{false};
std::atomic<std::uint64_t> g_pose_seq{0};
std::atomic<bool> g_consumer_anchor{false};
std::atomic<float> g_consumed_yaw{0.0f};
std::atomic<float> g_consumed_pitch{0.0f};
std::atomic<std::uint64_t> g_consumed_seq{0};
std::atomic<std::uint64_t> g_published{0};
std::atomic<std::uint64_t> g_consumed{0};
std::atomic<std::uint64_t> g_coalesced{0};
std::atomic<std::uint64_t> g_injected{0};
std::atomic<std::uint64_t> g_last_inject_ms{0};
std::atomic<std::uint64_t> g_last_phase_log_ms{0};

std::atomic<int> g_gamepad_mode{0};
struct PadSlot {
    const char* name;
    int keyId;
    std::array<std::uint8_t,kEventBytes> templ{};
    std::atomic<bool> valid{false};
    std::atomic<bool> last_down{false};
    std::atomic<bool> have_down_state{false};
};
// V64.54: KCD2-retail XInput ids are the CryEngine XI block shifted by +4.
// 0x21A/0x21B (right stick) is independently verified in KCD2Tools.
PadSlot g_pad_slots[]={
    {"xi_dpad_up",0x204},{"xi_dpad_down",0x205},{"xi_dpad_left",0x206},{"xi_dpad_right",0x207},
    {"xi_start",0x208},{"xi_back",0x209},{"xi_thumbl",0x20A},{"xi_thumbr",0x20B},
    {"xi_shoulderl",0x20C},{"xi_shoulderr",0x20D},{"xi_a",0x20E},{"xi_b",0x20F},{"xi_x",0x210},{"xi_y",0x211},
    {"xi_triggerl",0x212},{"xi_triggerr",0x213},{"xi_thumblx",0x214},{"xi_thumbly",0x215},
    {"xi_thumbrx",0x21A},{"xi_thumbry",0x21B},
    // V66.178R6: the eight CryEngine stick-direction pseudo-keys. KCD2's own
    // defaultProfile.xml binds menu navigation (navigation_up/down/left/right,
    // apse_focus) to xi_dpad_* AND xi_thumbl_*/xi_thumbr_*; a physical pad emits
    // both, the analog axes alone never move a menu selection (2026-10-08 report:
    // sticks dead in Start/Theater/Inventory since the native XI transport). Ids
    // follow the proven +4 shift: ThumbLX 0x214, LY 0x215, L up/down/left/right
    // 0x216-0x219, RX 0x21A, RY 0x21B, R up/down/left/right 0x21C-0x21F. All
    // eight names exist in the retail WHGame.dll string table.
    {"xi_thumbl_up",0x216},{"xi_thumbl_down",0x217},{"xi_thumbl_left",0x218},{"xi_thumbl_right",0x219},
    {"xi_thumbr_up",0x21C},{"xi_thumbr_down",0x21D},{"xi_thumbr_left",0x21E},{"xi_thumbr_right",0x21F},
    // CryEngine IInput.h places TriggerLBtn/TriggerRBtn immediately after the
    // four right-stick direction pseudo-keys. KCD2's XI block is the proven
    // CryEngine block shifted by +4, so these exact retail ids are 0x220/0x221.
    {"xi_triggerl_btn",0x220},{"xi_triggerr_btn",0x221}
};
// D84: native dispatch reads one complete pad. Independent atomics allowed a
// new B edge to accompany an old forward stick, or analog/menu directions to
// disagree when the producer ran between emit_pad calls.
kcd2vr::core::SnapshotMailbox<kcd2vr::pad_dispatch_d84::Frame> g_pad_snapshotD84;
std::atomic<bool> g_pad_stick_keys{true}; // V66.178R6 INI VrPadStickDirectionKeys
bool g_stick_key_state[8]{};               // dispatcher-thread hysteresis latches
std::atomic<std::uintptr_t> g_pad_symbol_anchor{0};
alignas(16) std::array<std::uint8_t,kEventBytes> g_pad_shell{};
std::atomic<bool> g_pad_shell_valid{false};
// V64.56 DEBUG universal bootstrap. CryEngine SInputEvent is reconstructed from
// the retail layout already proven by the native dispatcher. The synthetic XI
// symbols live for the lifetime of the DLL and are replaced automatically by
// genuine runtime XI templates if KCD2 ever emits them.
constexpr std::size_t kSyntheticSymbolBytes=0x28;
alignas(16) std::array<std::array<std::uint8_t,kSyntheticSymbolBytes>, sizeof(g_pad_slots)/sizeof(g_pad_slots[0])> g_synth_symbols{};
std::atomic<bool> g_universal_bootstrap_done{false};
std::atomic<bool> g_universal_bootstrap_ok{false};
std::atomic<bool> g_pad_discovery_pending{false};
std::atomic<std::uint64_t> g_pad_last_emit{0};
std::atomic<std::uint64_t> g_pad_emit_events{0};
// V64.79: the F24 event is transport only. Consume it after native XI
// is flushed so it cannot become KCD2's last-active keyboard device.
std::atomic<std::uint64_t> g_transport_heartbeat_until_ms{0};
std::atomic<int> g_transport_heartbeat_key_id{-1};
std::atomic<int> g_transport_heartbeat_budget{0};
std::atomic<std::uint64_t> g_transport_heartbeat_consumed{0};
constexpr std::ptrdiff_t kKeyNameOff=0x08;
constexpr int kPressed=1, kReleased=2, kDown=4;

// Forward declarations for helpers used by the V64.47 native-gamepad bridge.
bool plausible(std::uintptr_t p) noexcept;
void lock_templates() noexcept;
void unlock_templates() noexcept;


bool writable_region(DWORD protect) noexcept {
    if(protect & (PAGE_NOACCESS|PAGE_GUARD)) return false;
    const DWORD p=protect & 0xff;
    return p==PAGE_READWRITE||p==PAGE_WRITECOPY||p==PAGE_EXECUTE_READWRITE||p==PAGE_EXECUTE_WRITECOPY;
}
bool safe_cstr_eq(const char* p,const char* expected) noexcept {
    if(!plausible(reinterpret_cast<std::uintptr_t>(p))||!expected) return false;
    __try {
        for(std::size_t i=0;i<40;++i){ const char a=p[i],b=expected[i]; if(a!=b)return false; if(!a)return true; }
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    return false;
}
std::uintptr_t module_end(HMODULE m) noexcept {
    if(!m) return 0;
    __try {
        auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(m); if(dos->e_magic!=IMAGE_DOS_SIGNATURE)return 0;
        auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(reinterpret_cast<const std::uint8_t*>(m)+dos->e_lfanew);
        if(nt->Signature!=IMAGE_NT_SIGNATURE)return 0;
        return reinterpret_cast<std::uintptr_t>(m)+nt->OptionalHeader.SizeOfImage;
    } __except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
std::uintptr_t find_module_string(HMODULE m,const char* text) noexcept {
    const auto b=reinterpret_cast<std::uintptr_t>(m),e=module_end(m); if(!b||!e||!text)return 0;
    const std::size_t n=std::strlen(text)+1;
    for(std::uintptr_t p=b;p+n<=e;++p){
        __try { if(std::memcmp(reinterpret_cast<const void*>(p),text,n)==0)return p; }
        __except(EXCEPTION_EXECUTE_HANDLER) { }
    }
    return 0;
}
// CryEngine SInputSymbol retail-compatible prefix inferred from the public CryEngine IInput.h:
// keyId@+0x00, name pointer@+0x08, devSpecId@+0x10, state@+0x14, type@+0x18,
// value@+0x1C, user@+0x20, deviceId@+0x24. Discovery is READ-ONLY until every
// exact field check succeeds; we never write the symbol itself.
bool validate_symbol_candidate(std::uintptr_t p,PadSlot& slot,std::uintptr_t nameAddr) noexcept {
    if(!plausible(p))return false;
    __try {
        const int key=*reinterpret_cast<const int*>(p+0x00);
        const auto name=*reinterpret_cast<const std::uintptr_t*>(p+0x08);
        const int type=*reinterpret_cast<const int*>(p+0x18);
        const int deviceId=*reinterpret_cast<const int*>(p+0x24);
        if(key!=slot.keyId || name!=nameAddr || type<0 || type>4 || deviceId<0 || deviceId>64) return false;
        if(!safe_cstr_eq(reinterpret_cast<const char*>(name),slot.name))return false;
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER){ return false; }
}
bool build_universal_template(PadSlot& slot,std::size_t slotIndex,std::uintptr_t nameAddr) noexcept {
    if(!nameAddr || slotIndex>=g_synth_symbols.size()) return false;
    if(!safe_cstr_eq(reinterpret_cast<const char*>(nameAddr),slot.name)) return false;

    // Public CryEngine CInputDevice::MapSymbol/AssignTo semantics: the symbol owns
    // key id, name, type, value and device id; AssignTo copies those into an
    // SInputEvent. KCD2's observed 64-bit retail offsets are already proven above.
    // eIDT_Gamepad is 2 in this CryInput lineage. We intentionally initialize only
    // fields consumed by the dispatcher/action-map path and leave user/device index 0.
    auto& raw=g_synth_symbols[slotIndex]; raw.fill(0);
    const bool axis = std::strcmp(slot.name,"xi_triggerl")==0 || std::strcmp(slot.name,"xi_triggerr")==0 ||
                      std::strncmp(slot.name,"xi_thumb",8)==0 && (slot.name[8]=='l'||slot.name[8]=='r') &&
                      (slot.name[9]=='x'||slot.name[9]=='y');
    __try {
        *reinterpret_cast<int*>(raw.data()+0x00)=slot.keyId;             // keyId
        *reinterpret_cast<std::uintptr_t*>(raw.data()+0x08)=nameAddr;  // name
        *reinterpret_cast<int*>(raw.data()+0x10)=slot.keyId;           // devSpecId (stable synthetic id)
        *reinterpret_cast<int*>(raw.data()+0x14)=kReleased;            // state
        *reinterpret_cast<int*>(raw.data()+0x18)=axis?1:0;             // symbol type: axis/button
        *reinterpret_cast<float*>(raw.data()+0x1C)=0.0f;               // value
        *reinterpret_cast<std::uintptr_t*>(raw.data()+0x20)=0;         // user
        *reinterpret_cast<int*>(raw.data()+0x24)=2;                    // eIDT_Gamepad
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }

    alignas(16) std::array<std::uint8_t,kEventBytes> ev{};
    ev.fill(0);
    __try {
        *reinterpret_cast<int*>(ev.data()+0x00)=2;                     // eIDT_Gamepad
        *reinterpret_cast<int*>(ev.data()+kStateOff)=kChanged;
        *reinterpret_cast<const char**>(ev.data()+kKeyNameOff)=reinterpret_cast<const char*>(nameAddr);
        *reinterpret_cast<int*>(ev.data()+kKeyIdOff)=slot.keyId;
        *reinterpret_cast<int*>(ev.data()+0x14)=0;                     // modifiers
        *reinterpret_cast<float*>(ev.data()+kValueOff)=0.0f;
        *reinterpret_cast<std::uintptr_t*>(ev.data()+0x20)=reinterpret_cast<std::uintptr_t>(raw.data());
        // +0x28 device index/remaining retail tail stays zero.
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }

    lock_templates(); std::memcpy(slot.templ.data(),ev.data(),kEventBytes); unlock_templates();
    slot.valid.store(true,std::memory_order_release);
    return true;
}

bool bootstrap_universal_gamepad() noexcept {
    if(g_universal_bootstrap_done.load(std::memory_order_acquire))
        return g_universal_bootstrap_ok.load(std::memory_order_acquire);
    HMODULE wh=GetModuleHandleW(L"WHGame.dll"); if(!wh) return false;
    // Do not arm until the proven KCD2 dispatcher/controller instance has actually
    // observed native input. This guarantees replay still happens on KCD2's own input thread.
    if(!g_original || !plausible(g_controller.load(std::memory_order_acquire))) return false;

    int built=0;
    for(std::size_t i=0;i<sizeof(g_pad_slots)/sizeof(g_pad_slots[0]);++i) {
        auto& slot=g_pad_slots[i];
        if(slot.valid.load(std::memory_order_acquire)){ ++built; continue; }
        const auto name=find_module_string(wh,slot.name);
        if(name && build_universal_template(slot,i,name)) ++built;
    }
    const bool ok=NativeGamepadCoreReady();
    g_universal_bootstrap_ok.store(ok,std::memory_order_release);
    g_universal_bootstrap_done.store(true,std::memory_order_release);
    char m[384]{};
    std::snprintf(m,sizeof(m),"V64.56-GAMEPAD UNIVERSAL-BOOTSTRAP built=%d/%zu coreReady=%d dispatcher=%p controller=%p source=internal-xi-no-runtime-dependency",
        built,(sizeof(g_pad_slots)/sizeof(g_pad_slots[0])),ok?1:0,reinterpret_cast<void*>(g_original),reinterpret_cast<void*>(g_controller.load()));
    diagnostic_trace::Write(m);
    return ok;
}

bool build_template_from_symbol(PadSlot& slot,std::uintptr_t symbol,std::uintptr_t nameAddr) noexcept {
    if(!validate_symbol_candidate(symbol,slot,nameAddr))return false;
    // Require a genuine native XI event shell. This preserves device type/index/unique-id
    // and unknown padding exactly as KCD2 emitted them; never derive gamepad events from mouse metadata.
    alignas(16) std::array<std::uint8_t,kEventBytes> ev{};
    lock_templates();
    const bool haveShell=g_pad_shell_valid.load(std::memory_order_relaxed);
    if(haveShell) std::memcpy(ev.data(),g_pad_shell.data(),kEventBytes);
    unlock_templates();
    if(!haveShell) return false;
    __try {
        *reinterpret_cast<int*>(ev.data()+kStateOff)=kChanged;
        *reinterpret_cast<const char**>(ev.data()+kKeyNameOff)=reinterpret_cast<const char*>(nameAddr);
        *reinterpret_cast<int*>(ev.data()+kKeyIdOff)=slot.keyId;
        *reinterpret_cast<float*>(ev.data()+kValueOff)=0.0f;
        *reinterpret_cast<std::uintptr_t*>(ev.data()+0x20)=symbol;
    } __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    lock_templates(); std::memcpy(slot.templ.data(),ev.data(),kEventBytes); unlock_templates();
    slot.valid.store(true,std::memory_order_release);
    char m[256]{}; std::snprintf(m,sizeof(m),"V64.56-GAMEPAD direct symbol %s keyId=0x%X symbol=%p",slot.name,slot.keyId,reinterpret_cast<void*>(symbol)); diagnostic_trace::Write(m);
    return true;
}

void capture_pad_template(std::uintptr_t ev) noexcept {
    if(!plausible(ev)) return;
    __try {
        const char* n=*reinterpret_cast<const char* const*>(ev+kKeyNameOff);
        if(!plausible(reinterpret_cast<std::uintptr_t>(n))) return;
        if(n[0]!='x'||n[1]!='i'||n[2]!='_') return;
        for(auto& slot:g_pad_slots){ if(std::strcmp(n,slot.name)==0 && *reinterpret_cast<const int*>(ev+kKeyIdOff)==slot.keyId){
            const bool first=!slot.valid.load(std::memory_order_relaxed);
            lock_templates();
            std::memcpy(slot.templ.data(),reinterpret_cast<const void*>(ev),kEventBytes);
            std::memcpy(g_pad_shell.data(),reinterpret_cast<const void*>(ev),kEventBytes);
            unlock_templates();
            g_pad_shell_valid.store(true,std::memory_order_release);
            slot.valid.store(true,std::memory_order_release);
            const auto sym=*reinterpret_cast<const std::uintptr_t*>(ev+0x20);
            if(plausible(sym)){ g_pad_symbol_anchor.store(sym,std::memory_order_release); g_pad_discovery_pending.store(true,std::memory_order_release); }
            if(first){ char m[256]{}; std::snprintf(m,sizeof(m),"V64.56-GAMEPAD captured genuine template %s keyId=0x%X symbol=%p",slot.name,*reinterpret_cast<const int*>(ev+kKeyIdOff),reinterpret_cast<void*>(sym)); diagnostic_trace::Write(m); }
            break;
        }}
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}
PadSlot* pad_slot(const char* n) noexcept { for(auto& s:g_pad_slots) if(std::strcmp(s.name,n)==0) return &s; return nullptr; }
void emit_pad(std::uintptr_t controller,char flag,const char* name,float value,bool digital=false,bool down=false,const kcd2vr::attack_xi178r8::Packet* owned18=nullptr) noexcept {
    auto* slot=pad_slot(name); if(!slot||!slot->valid.load(std::memory_order_acquire)||!g_original||!plausible(controller)) return;
    alignas(16) std::array<std::uint8_t,kEventBytes> ev{}; lock_templates(); std::memcpy(ev.data(),slot->templ.data(),kEventBytes); unlock_templates();
    if(digital){
        const bool had=slot->have_down_state.exchange(true,std::memory_order_acq_rel);
        const bool prev=slot->last_down.exchange(down,std::memory_order_acq_rel);
        int state=0;
        if(!had || down!=prev) state=down?kPressed:kReleased;
        else if(down) state=kDown;
        else {if(std::strcmp(name,"xi_triggerl_btn")==0)kcd2vr::attack_xi178r8::ackLtUp55c1();return;} // already released
        *reinterpret_cast<int*>(ev.data()+kStateOff)=state;
        *reinterpret_cast<float*>(ev.data()+kValueOff)=down?1.0f:0.0f;
    } else {
        *reinterpret_cast<int*>(ev.data()+kStateOff)=kChanged;
        *reinterpret_cast<float*>(ev.data()+kValueOff)=std::clamp(value,-1.0f,1.0f);
    }
    // V66.178R13/R16: our attack/guard/special symbols (and the releases of symbols we pressed) are
    // dispatched under a thread-local provenance so the mod's OnAction detour does not suppress them.
    // Ownership comes with the published pad (attack_xi178r8::ownedMask), not from timers at emit time.
    bool owned=false;int ownedIndex=-1;int gesture51=0;unsigned sequence51=0,costNonce54=0;
    {
        static const char* const ownedNames[6]{"xi_triggerr","xi_triggerr_btn","xi_triggerl","xi_triggerl_btn","xi_a","xi_shoulderl"};
        for(int k=0;k<6;++k)if(std::strcmp(name,ownedNames[k])==0){ownedIndex=k;break;}
        if(ownedIndex>=0){
            const unsigned mask=owned18?owned18->mask:kcd2vr::attack_xi178r8::ownedMask.load(std::memory_order_acquire); // V66.178R18: mask from the SAME packet as the values
            const bool oursNow=(ownedIndex<=1&&(mask&1u))||(ownedIndex>=2&&ownedIndex<=3&&(mask&2u))||(ownedIndex==4&&(mask&4u))||(ownedIndex==5&&(mask&8u));
            static std::atomic<bool> ownedPrev[6]{}; // one latch per exact symbol (analog and _btn are separate events)
            const bool active=digital?down:(value>0.2f);
            if(ownedIndex<=1){
                static kcd2vr::attack_xi178r8::GestureLatch51 gestures[2];
                gesture51=int(gestures[ownedIndex].update(oursNow,active,owned18?owned18->stroke:0,owned18?owned18->serial:0,owned18?owned18->costNonce54:0));
                sequence51=gestures[ownedIndex].serial;
                costNonce54=gestures[ownedIndex].costNonce54;
            }
            if(oursNow&&active){ownedPrev[ownedIndex].store(true,std::memory_order_relaxed);owned=true;}
            else if(!active&&ownedPrev[ownedIndex].load(std::memory_order_relaxed)){owned=true;ownedPrev[ownedIndex].store(false,std::memory_order_relaxed);} // our release
            else if(!active)ownedPrev[ownedIndex].store(false,std::memory_order_relaxed);
        }
    }
    {   // V66.178R8: time the game's own dispatch of each pad symbol (the direct combat handler call cost ~69 ms).
        const bool timeD48=diagnostic_trace::EnabledFor(diagnostics::capture168::Domain::Bug)||diagnostic_trace::EnabledFor(diagnostics::capture168::Domain::Combat);
        const auto t0=timeD48?std::chrono::steady_clock::now():std::chrono::steady_clock::time_point{};
        if(timeD48){kcd2vr::attack_xi178r8::detourUs=0;kcd2vr::attack_xi178r8::handlerUs=0;kcd2vr::attack_xi178r8::detourCalls=0;kcd2vr::attack_xi178r8::insideHooksUs=0;kcd2vr::attack_xi178r8::insideOriginalUs=0;kcd2vr::attack_xi178r8::dispatchRecordCount=0;}
        struct Owned{bool prev;~Owned(){kcd2vr::attack_xi178r8::ownedDispatch=prev;}} ownedScope{kcd2vr::attack_xi178r8::ownedDispatch};
        kcd2vr::attack_xi178r8::ownedDispatch=owned||ownedScope.prev;
        struct GestureScope51{int old;~GestureScope51(){kcd2vr::attack_xi178r8::dispatchedStroke51=old;}} gestureScope{kcd2vr::attack_xi178r8::dispatchedStroke51};
        kcd2vr::attack_xi178r8::dispatchedStroke51=owned?gesture51:0;
        struct CostScope54{unsigned old,serial;~CostScope54(){kcd2vr::attack_xi178r8::dispatchedCost54=old;kcd2vr::attack_xi178r8::dispatchedSerial54=serial;}} costScope54{kcd2vr::attack_xi178r8::dispatchedCost54,kcd2vr::attack_xi178r8::dispatchedSerial54};
        kcd2vr::attack_xi178r8::dispatchedCost54=owned?costNonce54:0;
        kcd2vr::attack_xi178r8::dispatchedSerial54=owned?sequence51:0;
        g_original(controller,reinterpret_cast<std::uintptr_t>(ev.data()),flag);
        if(owned){ // V66.178R16 acknowledgements: the game has now seen this edge
            const bool activeNow=digital?down:(value>0.2f);
            if(ownedIndex==0&&activeNow)kcd2vr::attack_xi178r8::ackRtDown(sequence51);
            if(ownedIndex==0&&!activeNow)kcd2vr::attack_xi178r8::ackRtUp51(sequence51);
            if(ownedIndex==3&&!activeNow)kcd2vr::attack_xi178r8::ackLtUp55c1();
            if(ownedIndex==4&&activeNow)kcd2vr::attack_xi178r8::ackADown();
        }
        const auto ms=timeD48?std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-t0).count()/1000.0:0.;
        if(timeD48&&ms>=20.0){static std::atomic<std::uint64_t> last{0};const auto now=std::uint64_t(GetTickCount64());auto l=last.load();
            if(now-l>=100&&last.compare_exchange_strong(l,now)){char m[400]{};std::snprintf(m,sizeof(m),"V66.178R22C1 XI_DISPATCH_STALL symbol=%s down=%d ms=%.1f detours=%llu detourInclusiveMs=%.1f handlerInclusiveMs=%.1f hookInclusiveMs=%.1f originalInclusiveMs=%.1f nestedScopes=1 exclusiveCost=UNKNOWN",name,down?1:0,ms,
                (unsigned long long)kcd2vr::attack_xi178r8::detourCalls,double(kcd2vr::attack_xi178r8::detourUs)/1000.0,double(kcd2vr::attack_xi178r8::handlerUs)/1000.0,double(kcd2vr::attack_xi178r8::insideHooksUs)/1000.0,double(kcd2vr::attack_xi178r8::insideOriginalUs)/1000.0);diagnostic_trace::Write(m);
            {char d[400]{};int o=std::snprintf(d,sizeof(d),"V66.178R22 XI_DISPATCH_ACTIONS symbol=%s originalInclusiveMs=%.1f",name,double(kcd2vr::attack_xi178r8::insideOriginalUs)/1000.0);
             for(unsigned i=0;i<kcd2vr::attack_xi178r8::dispatchRecordCount&&o<int(sizeof(d))-40;++i){const auto& r=kcd2vr::attack_xi178r8::dispatchRecords[i];o+=std::snprintf(d+o,sizeof(d)-o," [%s m%d %.1fms]",r.name,r.mode,double(r.handlerUs)/1000.0);}
             diagnostic_trace::Write(d);}}}
    }
    g_pad_emit_events.fetch_add(1,std::memory_order_relaxed);
}

// R25: metadata only, on the existing native dispatch thread and only while a
// physical X occurrence asks. Original action dispatch remains the authority.
std::atomic<std::uint64_t> g_carry_query25{0},g_carry_answer25{0};
int ReadCarryHint25(std::uintptr_t base) noexcept {
    if(!base||!retail_build112::Ready(base))return -1;
    __try {
        static const unsigned char graph[]={0x48,0x8b,0x88,0x28,0x01,0x00,0x00,0x4c,0x8b,0x79,0x58};
        static const unsigned char stride[]={0x48,0x81,0xc2,0xe0,0x00,0x00,0x00};
        static const unsigned char type[]={0x8a,0x41,0x08,0x38,0x42,0x14};
        if(std::memcmp(reinterpret_cast<void*>(base+0x7F402D),graph,sizeof(graph))||
           std::memcmp(reinterpret_cast<void*>(base+0x8E6521),stride,sizeof(stride))||
           std::memcmp(reinterpret_cast<void*>(base+0x8E6595),type,sizeof(type)))return -1;
        const auto game=*reinterpret_cast<const std::uintptr_t*>(base+0x549D388);
        if(!game)return -1;
        const auto service=*reinterpret_cast<const std::uintptr_t*>(game+0x128);
        if(!service)return -1;
        const auto manager=*reinterpret_cast<const std::uintptr_t*>(service+0x58);
        if(!manager||*reinterpret_cast<const std::uintptr_t*>(manager)!=base+0x3F60DC0||
           *reinterpret_cast<const std::uintptr_t*>(manager+8)!=base+0x3F60DD8||
           !*reinterpret_cast<const unsigned char*>(manager+0x124))return -1;
        const auto first=*reinterpret_cast<const std::uintptr_t*>(manager+0x50);
        const auto last=*reinterpret_cast<const std::uintptr_t*>(manager+0x58);
        if(last<first||(last-first)%0xE0||(last-first)/0xE0>64||(last!=first&&!first))return -1;
        unsigned matches=0;
        for(auto row=first;row!=last;row+=0xE0){
            const auto name=*reinterpret_cast<const char*const*>(row);
            bool same=name!=nullptr;
            for(unsigned k=0;k!=10&&same;++k)same=name[k]=="grab_body"[k];
            if(same&&*reinterpret_cast<const unsigned char*>(row+0x14)==2)++matches;
        }
        if(*reinterpret_cast<const std::uintptr_t*>(base+0x549D388)!=game||
           *reinterpret_cast<const std::uintptr_t*>(game+0x128)!=service||
           *reinterpret_cast<const std::uintptr_t*>(service+0x58)!=manager||
           *reinterpret_cast<const std::uintptr_t*>(manager)!=base+0x3F60DC0||
           *reinterpret_cast<const std::uintptr_t*>(manager+8)!=base+0x3F60DD8||
           !*reinterpret_cast<const unsigned char*>(manager+0x124)||
           *reinterpret_cast<const std::uintptr_t*>(manager+0x50)!=first||
           *reinterpret_cast<const std::uintptr_t*>(manager+0x58)!=last)return -1;
        return matches==1?1:matches? -1:0;
    } __except(EXCEPTION_EXECUTE_HANDLER){return -1;}
}
void UpdateCarryQuery25() noexcept {
    const auto q=g_carry_query25.load(std::memory_order_acquire);
    const auto press=std::uint32_t(q);if(!press)return;
    const auto now=std::uint32_t(GetTickCount64());
    if(now-std::uint32_t(q>>32)>80)return;
    const int value=ReadCarryHint25(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")));
    if(std::uint32_t(g_carry_query25.load(std::memory_order_acquire))!=press)return;
    const auto finish=std::uint32_t(GetTickCount64());
    if(finish-std::uint32_t(q>>32)>80)return;
    g_carry_answer25.store((std::uint64_t(finish)<<32)|(std::uint64_t(press)<<2)|unsigned(value+1),std::memory_order_release);
}

// R147: the same verified native hint rows as ReadCarryHint25 (BasicAIActions.lua AddInteractorAction rows, name =
// action at +0), looking for the follow prompt. metadata only, dispatch thread, only while a mounted X tap asks.
std::atomic<std::uint32_t> g_follow_want147{0};std::atomic<std::uint64_t> g_follow_answer147{0};
int ReadFollowHint147(std::uintptr_t base) noexcept {
    if(!base||!retail_build112::Ready(base))return -1;
    static const char kName[]="focus_follow_init_interactor_press";
    __try {
        static const unsigned char graph[]={0x48,0x8b,0x88,0x28,0x01,0x00,0x00,0x4c,0x8b,0x79,0x58};
        static const unsigned char stride[]={0x48,0x81,0xc2,0xe0,0x00,0x00,0x00};
        if(std::memcmp(reinterpret_cast<void*>(base+0x7F402D),graph,sizeof(graph))||
           std::memcmp(reinterpret_cast<void*>(base+0x8E6521),stride,sizeof(stride)))return -1;
        const auto game=*reinterpret_cast<const std::uintptr_t*>(base+0x549D388);
        if(!game)return -1;
        const auto service=*reinterpret_cast<const std::uintptr_t*>(game+0x128);
        if(!service)return -1;
        const auto manager=*reinterpret_cast<const std::uintptr_t*>(service+0x58);
        if(!manager||*reinterpret_cast<const std::uintptr_t*>(manager)!=base+0x3F60DC0||
           *reinterpret_cast<const std::uintptr_t*>(manager+8)!=base+0x3F60DD8||
           !*reinterpret_cast<const unsigned char*>(manager+0x124))return -1;
        const auto first=*reinterpret_cast<const std::uintptr_t*>(manager+0x50);
        const auto last=*reinterpret_cast<const std::uintptr_t*>(manager+0x58);
        if(last<first||(last-first)%0xE0||(last-first)/0xE0>64||(last!=first&&!first))return -1;
        bool found=false;
        for(auto row=first;row!=last&&!found;row+=0xE0){
            const auto name=*reinterpret_cast<const char*const*>(row);
            bool same=name!=nullptr;
            for(unsigned k=0;k!=sizeof(kName)&&same;++k)same=name[k]==kName[k];
            found=same;
        }
        if(*reinterpret_cast<const std::uintptr_t*>(manager+0x50)!=first||
           *reinterpret_cast<const std::uintptr_t*>(manager+0x58)!=last)return -1;
        return found?1:0;
    } __except(EXCEPTION_EXECUTE_HANDLER){return -1;}
}
void UpdateFollowQuery147() noexcept {
    const auto want=g_follow_want147.load(std::memory_order_acquire);if(!want)return;
    const auto now=std::uint32_t(GetTickCount64());if(now-want>80)return;
    const int value=ReadFollowHint147(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")));
    g_follow_answer147.store((std::uint64_t(std::uint32_t(GetTickCount64()))<<2)|unsigned(value+1),std::memory_order_release);
}

void flush_gamepad_same_dispatch_thread(std::uintptr_t controller,char flag) noexcept {
    const int mode=g_gamepad_mode.load(std::memory_order_acquire); if(mode<2||!g_original) return;
    kcd2vr::pad_dispatch_d84::Frame pad{};
    if(!g_pad_snapshotD84.read(pad))return; // no published coherent pad: wait for the next native dispatch
    const auto now=static_cast<std::uint64_t>(GetTickCount64()); auto last=g_pad_last_emit.load(std::memory_order_relaxed); if(now-last<8) return; g_pad_last_emit.store(now,std::memory_order_relaxed);
    emit_pad(controller,flag,"xi_thumblx",pad.lx); emit_pad(controller,flag,"xi_thumbly",pad.ly);
    emit_pad(controller,flag,"xi_thumbrx",pad.rx); emit_pad(controller,flag,"xi_thumbry",pad.ry);
    if(g_pad_stick_keys.load(std::memory_order_relaxed)){
        // V66.178R6: digital stick directions exactly like a physical XInput pad
        // (press above 0.5, release below 0.35; the game's own hold/repeat timing
        // does the scrolling). Published together with the axes in every context
        // the axes are published in, so gameplay/menu parity matches a real pad.
        const float ax[4]{pad.lx,pad.ly,pad.rx,pad.ry};
        static const char* names[8]{"xi_thumbl_up","xi_thumbl_down","xi_thumbl_left","xi_thumbl_right",
                                    "xi_thumbr_up","xi_thumbr_down","xi_thumbr_left","xi_thumbr_right"};
        const float dir[8]{ax[1],-ax[1],-ax[0],ax[0],ax[3],-ax[3],-ax[2],ax[2]};
        for(int i=0;i<8;++i){
            bool& on=g_stick_key_state[i];
            if(!on&&dir[i]>0.50f)on=true; else if(on&&dir[i]<0.35f)on=false;
            emit_pad(controller,flag,names[i],0,true,on);
        }
    }
    if(mode>=3){
        UpdateCarryQuery25();
        UpdateFollowQuery147(); // R147
        // V66.178R18 (review F05): combat symbols + ownership + serial come from ONE packet read once.
        const auto pk=kcd2vr::attack_xi178r8::unpack(pad.combat18);
        kcd2vr::attack_xi178r8::emitTriggers51c3(pk,[&](const char* name,float value,bool digital,bool held){
            emit_pad(controller,flag,name,value,digital,held,&pk);
        });
        emit_pad(controller,flag,"xi_a",0,true,pk.a,&pk); emit_pad(controller,flag,"xi_b",0,true,pad.b);
        emit_pad(controller,flag,"xi_x",0,true,pad.x); emit_pad(controller,flag,"xi_y",0,true,pad.y);
        emit_pad(controller,flag,"xi_shoulderl",0,true,pk.lb,&pk); emit_pad(controller,flag,"xi_shoulderr",0,true,pad.rb);
        emit_pad(controller,flag,"xi_thumbl",0,true,pad.l3); emit_pad(controller,flag,"xi_thumbr",0,true,pad.r3);
        emit_pad(controller,flag,"xi_start",0,true,pad.start); emit_pad(controller,flag,"xi_back",0,true,pad.back);
        emit_pad(controller,flag,"xi_dpad_up",0,true,pad.du); emit_pad(controller,flag,"xi_dpad_down",0,true,pad.dd);
        emit_pad(controller,flag,"xi_dpad_left",0,true,pad.dl); emit_pad(controller,flag,"xi_dpad_right",0,true,pad.dr);
    }
}



bool plausible(std::uintptr_t p) noexcept { return p > 0x10000ull && p < 0x0000800000000000ull; }

void lock_templates() noexcept { while(g_template_lock.test_and_set(std::memory_order_acquire)) YieldProcessor(); }
void unlock_templates() noexcept { g_template_lock.clear(std::memory_order_release); }

void capture_template(std::uintptr_t ev, int id) noexcept {
    if(!plausible(ev)) return;
    __try {
        const int state=*reinterpret_cast<const int*>(ev+kStateOff);
        const int key=*reinterpret_cast<const int*>(ev+kKeyIdOff);
        if(state!=kChanged || key!=id) return;
        lock_templates();
        if(id==kMouseYaw) {
            const bool first=!g_yaw_valid.load(std::memory_order_relaxed);
            std::memcpy(g_yaw_template.data(),reinterpret_cast<const void*>(ev),kEventBytes); g_yaw_valid.store(true,std::memory_order_release);
            if(first) diagnostic_trace::Write("V64.33-NATIVE-INPUT captured native MouseX template (0x10A)");
        } else if(id==kMousePitch) {
            const bool first=!g_pitch_valid.load(std::memory_order_relaxed);
            std::memcpy(g_pitch_template.data(),reinterpret_cast<const void*>(ev),kEventBytes); g_pitch_valid.store(true,std::memory_order_release);
            if(first) diagnostic_trace::Write("V64.33-NATIVE-INPUT captured native MouseY template (0x10B)");
        }
        unlock_templates();
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

void dispatch_one(std::uintptr_t controller, char flag, bool yaw_axis, float value) noexcept {
    if(!g_original || !plausible(controller) || !std::isfinite(value) || std::fabs(value)<0.0001f) return;
    const bool ready=yaw_axis?g_yaw_valid.load(std::memory_order_acquire):g_pitch_valid.load(std::memory_order_acquire);
    if(!ready) return;
    alignas(16) std::array<std::uint8_t,kEventBytes> ev{};
    lock_templates();
    std::memcpy(ev.data(), yaw_axis?g_yaw_template.data():g_pitch_template.data(), kEventBytes);
    unlock_templates();
    *reinterpret_cast<int*>(ev.data()+kStateOff)=kChanged;
    *reinterpret_cast<int*>(ev.data()+kKeyIdOff)=yaw_axis?kMouseYaw:kMousePitch;
    *reinterpret_cast<float*>(ev.data()+kValueOff)=value;
    g_original(controller,reinterpret_cast<std::uintptr_t>(ev.data()),flag);
}

void flush_pending_same_dispatch_thread(std::uintptr_t controller, char flag) noexcept {
    if(!g_active.load(std::memory_order_acquire) || !g_original) return;
    if(!g_yaw_valid.load(std::memory_order_acquire) || !g_pitch_valid.load(std::memory_order_acquire)) return;
    if(!g_latest_valid.load(std::memory_order_acquire)) { g_consumer_anchor.store(false,std::memory_order_release); return; }

    // Read a stable absolute HMD snapshot.  If XR publishes while we read,
    // retry once on the next native input event rather than mixing axes from
    // different samples.
    const std::uint64_t seq1=g_pose_seq.load(std::memory_order_acquire);
    const float yaw=g_latest_yaw.load(std::memory_order_acquire);
    const float pitch=g_latest_pitch.load(std::memory_order_acquire);
    const std::uint64_t seq2=g_pose_seq.load(std::memory_order_acquire);
    if(seq1==0 || seq1!=seq2 || !std::isfinite(yaw) || !std::isfinite(pitch)) return;

    const std::uint64_t prevSeq=g_consumed_seq.load(std::memory_order_acquire);
    if(g_consumer_anchor.load(std::memory_order_acquire) && seq2==prevSeq) return; // no new XR pose

    if(!g_consumer_anchor.exchange(true,std::memory_order_acq_rel)) {
        g_consumed_yaw.store(yaw,std::memory_order_release);
        g_consumed_pitch.store(pitch,std::memory_order_release);
        g_consumed_seq.store(seq2,std::memory_order_release);
        return;
    }

    const float py=g_consumed_yaw.exchange(yaw,std::memory_order_acq_rel);
    const float pp=g_consumed_pitch.exchange(pitch,std::memory_order_acq_rel);
    g_consumed_seq.store(seq2,std::memory_order_release);
    if(seq2>prevSeq+1) g_coalesced.fetch_add(seq2-prevSeq-1,std::memory_order_relaxed);
    g_consumed.fetch_add(1,std::memory_order_relaxed);

    const float dy=std::remainder(yaw-py,6.28318530718f);
    const float dp=std::remainder(pitch-pp,6.28318530718f);
    // Recenter/discontinuity: re-anchor without injecting a spin.
    if(std::fabs(dy)>0.35f || std::fabs(dp)>0.35f) return;

    const float gain=std::clamp(g_input_gain.load(std::memory_order_acquire),0.0f,1.0f);
    const float y=std::clamp(-dy*kCountsPerRadian*gain,-kMaxCountsPerDispatch,kMaxCountsPerDispatch);
    const float p=std::clamp(-dp*kCountsPerRadian*gain,-kMaxCountsPerDispatch,kMaxCountsPerDispatch);
    dispatch_one(controller,flag,true,y);
    dispatch_one(controller,flag,false,p);
    if(std::fabs(y)>0.0001f || std::fabs(p)>0.0001f) {
        g_injected.fetch_add(1,std::memory_order_relaxed);
        g_last_inject_ms.store(GetTickCount64(),std::memory_order_release);
    }

    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto last=g_last_phase_log_ms.load(std::memory_order_relaxed);
    if(now-last>=1000ull && g_last_phase_log_ms.compare_exchange_strong(last,now,std::memory_order_acq_rel)) {
        char m[320]{};
        std::snprintf(m,sizeof(m),"V64.36-PHASE native-look published=%llu consumed=%llu coalesced=%llu injected=%llu seqGap=%llu gain=%.2f",
            static_cast<unsigned long long>(g_published.load(std::memory_order_relaxed)),
            static_cast<unsigned long long>(g_consumed.load(std::memory_order_relaxed)),
            static_cast<unsigned long long>(g_coalesced.load(std::memory_order_relaxed)),
            static_cast<unsigned long long>(g_injected.load(std::memory_order_relaxed)),
            static_cast<unsigned long long>(seq2>prevSeq?seq2-prevSeq:0), double(gain));
        diagnostic_trace::Write(m);
    }
}

void __fastcall Detour(std::uintptr_t controller, std::uintptr_t input_event, char flag) noexcept {
    if(plausible(controller)) { g_controller.store(controller,std::memory_order_release); g_last_flag.store(flag,std::memory_order_release); }
    bool consumeTransportHeartbeat=false;
    if(plausible(input_event)) {
        capture_template(input_event,kMouseYaw);
        capture_template(input_event,kMousePitch);
        capture_pad_template(input_event);
        const auto now=static_cast<std::uint64_t>(GetTickCount64());
        const auto until=g_transport_heartbeat_until_ms.load(std::memory_order_acquire);
        int budget=g_transport_heartbeat_budget.load(std::memory_order_acquire);
        if(budget>0 && now<=until) {
            __try {
                const int deviceType=*reinterpret_cast<const int*>(input_event+0x00);
                const int keyId=*reinterpret_cast<const int*>(input_event+kKeyIdOff);
                if(deviceType!=2 && keyId!=kMouseYaw && keyId!=kMousePitch) {
                    int known=g_transport_heartbeat_key_id.load(std::memory_order_acquire);
                    if(known<0) {
                        g_transport_heartbeat_key_id.store(keyId,std::memory_order_release); known=keyId;
                        char m[192]{}; std::snprintf(m,sizeof(m),"V64.79-GAMEPAD heartbeat Cry keyId=0x%X learned and hidden",keyId); diagnostic_trace::Write(m);
                    }
                    if(keyId==known) {
                        consumeTransportHeartbeat=true;
                        g_transport_heartbeat_budget.fetch_sub(1,std::memory_order_acq_rel);
                        g_transport_heartbeat_consumed.fetch_add(1,std::memory_order_relaxed);
                    }
                }
            } __except(EXCEPTION_EXECUTE_HANDLER) {}
        }
    }
    flush_pending_same_dispatch_thread(controller,flag);
    flush_gamepad_same_dispatch_thread(controller,flag);
    if(g_original && !consumeTransportHeartbeat) g_original(controller,input_event,flag);
}
}

bool Install() noexcept {
    if(g_hook) return true;
    std::vector<scan::Candidate> ladder;
    auto p1=scan::Pattern::compile("44 38 81 D8 00 00 00 0F 84 ?? ?? ?? ?? 83 7A 10 FF 0F 84 ?? ?? ?? ??");
    auto p2=scan::Pattern::compile("83 7A 10 FF 0F 84 ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 8B 08 85 C9");
    auto p3=scan::Pattern::compile("0F B6 52 28 F3 0F 10 5B 18 48 8B 0D ?? ?? ?? ?? 4C 8B 43 08 0F 5A DB");
    if(p1) ladder.push_back(scan::Candidate::direct("Input_P1_BodyFlagCmpFarJz",*p1,-0x15));
    if(p2) ladder.push_back(scan::Candidate::direct("Input_P2_PitchCmpRipMov",*p2,-0x22));
    if(p3) ladder.push_back(scan::Candidate::direct("Input_P3_ChangedLogBlock",*p3,-0x50));
    if(ladder.empty()) return false;
    Region game=Region::module_named("WHGame.dll");
    if(game.size==0) return false;
    auto hit=scan::resolve(scan::borrow_code_target(ladder,"kcd2vr_v6432_input_dispatch",game));
    if(!hit) { KCD2VR_DIAG_LOG(Bug,warning,"V64.33 native-input lab: CameraInputDispatch AOB unresolved; F4 write mode will fail closed"); return false; }
    hook::InlineRequest req{"kcd2vr_v6432_input_dispatch",hook::Target{hit->address},hook::Options{}};
    auto h=hook::inline_at(std::move(req),&Detour);
    if(!h) { KCD2VR_DIAG_LOG(Bug,warning,"V64.33 native-input lab: CameraInputDispatch hook install failed"); return false; }
    g_hook=std::move(*h); g_original=g_hook->original<InputDispatchFn>();
    if(!g_original || !g_hook->enable()) { g_original=nullptr; g_hook.reset(); return false; }
    diagnostic_trace::Write("V64.33-NATIVE-INPUT installed CameraInputDispatch eventIds={yaw:0x10A pitch:0x10B}");
    KCD2VR_DIAG_LOG(Bug,warning,"V64.33 native-input lab installed CameraInputDispatch via candidate '{}'", hit->winning_name);
    return true;
}

void Uninstall() noexcept { g_active.store(false); if(g_hook){ (void)g_hook->disable(); g_hook.reset(); } g_original=nullptr; }
void SetActive(bool active) noexcept { const bool old=g_active.exchange(active,std::memory_order_acq_rel); if(old!=active){ ResetAnchor(); char m[256]{}; std::snprintf(m,sizeof(m),"V64.36-PHASE active=%d ready=%d templates={yaw:%d pitch:%d}",active?1:0,Ready()?1:0,HasYawTemplate()?1:0,HasPitchTemplate()?1:0); diagnostic_trace::Write(m);} }
void SetInputGain(float gain) noexcept { g_input_gain.store(std::clamp(gain,0.0f,1.0f),std::memory_order_release); }
bool HasYawTemplate() noexcept { return g_yaw_valid.load(std::memory_order_acquire); }
bool HasPitchTemplate() noexcept { return g_pitch_valid.load(std::memory_order_acquire); }
bool Ready() noexcept { return g_original && plausible(g_controller.load(std::memory_order_acquire)) && HasYawTemplate() && HasPitchTemplate(); }
void ResetAnchor() noexcept {
    g_consumer_anchor.store(false,std::memory_order_release);
    g_consumed_seq.store(0,std::memory_order_release);
}

void PublishHeadAngles(float yaw,float pitch,bool valid) noexcept {
    if(!g_active.load(std::memory_order_acquire) || !valid || !std::isfinite(yaw)||!std::isfinite(pitch)) {
        g_latest_valid.store(false,std::memory_order_release);
        ResetAnchor();
        return;
    }
    g_latest_yaw.store(yaw,std::memory_order_relaxed);
    g_latest_pitch.store(pitch,std::memory_order_relaxed);
    g_latest_valid.store(true,std::memory_order_release);
    g_pose_seq.fetch_add(1,std::memory_order_release);
    g_published.fetch_add(1,std::memory_order_relaxed);
}

} // namespace kcd2vr::native_input_look_lab


namespace kcd2vr::native_input_look_lab {
void SetGamepadMode(int mode) noexcept {
    const int clamped=std::clamp(mode,0,3);
    const int old=g_gamepad_mode.exchange(clamped,std::memory_order_acq_rel);
    if(clamped==0){ for(auto& s:g_pad_slots){ s.have_down_state.store(false); s.last_down.store(false); } }
    else g_pad_discovery_pending.store(true,std::memory_order_release);
    // V65.33: this setter is reasserted frequently by production input ownership.
    // Log only real state changes; repeated mode=0 lines previously dominated diagnostics.
    if(old!=clamped){ char m[320]{}; std::snprintf(m,sizeof(m),"V64.56-GAMEPAD mode=%d symbols=%d coreReady=%d fullReady=%d leftReady=%d rightReady=%d",GetGamepadMode(),NativeGamepadSymbolCount(),NativeGamepadCoreReady()?1:0,NativeGamepadFullReady()?1:0,NativeLeftStickReady()?1:0,NativeRightStickReady()?1:0); diagnostic_trace::Write(m); }
}
int GetGamepadMode() noexcept { return g_gamepad_mode.load(std::memory_order_acquire); }
bool DiscoverNativeGamepadSymbolsDirect() noexcept {
    // V64.56 universal first path: no Virtual Desktop/XInput seed is required.
    // A genuine XI event remains an optional refinement and overwrites synthetic
    // slot templates through capture_pad_template().
    const bool universalOk=bootstrap_universal_gamepad();
    const std::uintptr_t anchor=g_pad_symbol_anchor.load(std::memory_order_acquire);
    if(!plausible(anchor)){
        if(universalOk) {
            g_pad_discovery_pending.store(false,std::memory_order_release);
            diagnostic_trace::Write("V64.56-GAMEPAD universal native XI bridge ready without external gamepad seed");
            return true;
        }
        diagnostic_trace::Write("V64.56-GAMEPAD universal bootstrap deferred: waiting for KCD2 native input dispatcher/controller");
        return false;
    }

    HMODULE wh=GetModuleHandleW(L"WHGame.dll"); if(!wh)return false;
    std::uintptr_t names[sizeof(g_pad_slots)/sizeof(g_pad_slots[0])]{};
    for(std::size_t i=0;i<sizeof(g_pad_slots)/sizeof(g_pad_slots[0]);++i) names[i]=find_module_string(wh,g_pad_slots[i].name);

    // One bounded pass, not one scan per control. Runtime XI symbols normally live near the
    // already-captured genuine gamepad symbol in the input-device heap.
    const std::uintptr_t span=0x02000000ull;
    std::uintptr_t lo=anchor>span?anchor-span:0x10000ull, hi=anchor+span;
    SYSTEM_INFO si{};GetSystemInfo(&si); lo=std::max<std::uintptr_t>(lo,reinterpret_cast<std::uintptr_t>(si.lpMinimumApplicationAddress)); hi=std::min<std::uintptr_t>(hi,reinterpret_cast<std::uintptr_t>(si.lpMaximumApplicationAddress));
    MEMORY_BASIC_INFORMATION mbi{};
    for(std::uintptr_t cur=lo;cur<hi && NativeGamepadSymbolCount()<(int)(sizeof(g_pad_slots)/sizeof(g_pad_slots[0]));){
        if(!VirtualQuery(reinterpret_cast<void*>(cur),&mbi,sizeof(mbi)))break;
        const auto rb=reinterpret_cast<std::uintptr_t>(mbi.BaseAddress), re=rb+mbi.RegionSize;
        if(mbi.State==MEM_COMMIT && writable_region(mbi.Protect) && mbi.RegionSize<=0x4000000ull){
            const auto sb=std::max(rb,lo), se=std::min(re,hi);
            for(std::uintptr_t q=(sb+7ull)&~7ull;q+0x28<=se;q+=8){
                int key=-1; std::uintptr_t name=0;
                __try { key=*reinterpret_cast<const int*>(q); name=*reinterpret_cast<const std::uintptr_t*>(q+0x08); }
                __except(EXCEPTION_EXECUTE_HANDLER) { continue; }
                for(std::size_t i=0;i<sizeof(g_pad_slots)/sizeof(g_pad_slots[0]);++i){
                    auto& slot=g_pad_slots[i]; if(slot.valid.load(std::memory_order_acquire)||!names[i])continue;
                    if(key==slot.keyId && name==names[i]) (void)build_template_from_symbol(slot,q,names[i]);
                }
            }
        }
        if(re<=cur)break; cur=re;
    }
    const int found=NativeGamepadSymbolCount();
    for(std::size_t i=0;i<sizeof(g_pad_slots)/sizeof(g_pad_slots[0]);++i) if(!g_pad_slots[i].valid.load(std::memory_order_acquire)){
        char mm[224]{};std::snprintf(mm,sizeof(mm),"V64.56-GAMEPAD symbol MISS %s keyId=0x%X name=%p",g_pad_slots[i].name,g_pad_slots[i].keyId,reinterpret_cast<void*>(names[i]));diagnostic_trace::Write(mm);
    }
    g_pad_discovery_pending.store(false,std::memory_order_release);
    char m[320]{}; std::snprintf(m,sizeof(m),"V64.56-GAMEPAD discovery complete found=%d/%zu coreReady=%d fullReady=%d anchor=%p",found,(sizeof(g_pad_slots)/sizeof(g_pad_slots[0])),NativeGamepadCoreReady()?1:0,NativeGamepadFullReady()?1:0,reinterpret_cast<void*>(anchor)); diagnostic_trace::Write(m);
    return NativeGamepadCoreReady();
}
bool GamepadDiscoveryContextReady() noexcept {
    return g_original && plausible(g_controller.load(std::memory_order_acquire));
}
int NativeGamepadSymbolCount() noexcept { int n=0; for(auto& s:g_pad_slots) if(s.valid.load(std::memory_order_acquire))++n; return n; }
void PublishGamepadState(float lx,float ly,float rx,float ry,float lt,float rt,
    bool a,bool b,bool x,bool y,bool lb,bool rb,bool l3,bool r3,bool start,bool back,
    bool dpad_up,bool dpad_down,bool dpad_left,bool dpad_right) noexcept {
    kcd2vr::pad_dispatch_d84::Frame pad{};
    pad.lx=lx;pad.ly=ly;pad.rx=rx;pad.ry=ry;
    {namespace xi=kcd2vr::attack_xi178r8;const auto m=xi::ownedMask.load(std::memory_order_acquire);const auto serial=xi::prepareSerial.load();const auto stroke=unsigned(xi::pendingStroke.load());
     pad.combat18=xi::pack(rt,lt,a,lb,m,serial,stroke,xi::packetNonce54(serial,stroke));}
    pad.b=b;pad.x=x;pad.y=y;pad.rb=rb;pad.l3=l3;pad.r3=r3;pad.start=start;pad.back=back;
    pad.du=dpad_up;pad.dd=dpad_down;pad.dl=dpad_left;pad.dr=dpad_right;
    g_pad_snapshotD84.publish(pad);
}

int QueryCarryContext25(std::uint32_t press) noexcept {
    if(!press||press>0x3fffffffu||g_gamepad_mode.load(std::memory_order_acquire)!=3){
        g_carry_query25.store(0,std::memory_order_release);return -1;
    }
    const auto now=std::uint32_t(GetTickCount64());
    g_carry_query25.store((std::uint64_t(now)<<32)|press,std::memory_order_release);
    const auto answer=g_carry_answer25.load(std::memory_order_acquire);
    if((std::uint32_t(answer)>>2)!=press||now-std::uint32_t(answer>>32)>80)return -1;
    return int(answer&3)-1;
}

int QueryFollowHint147(bool want) noexcept {
    if(!want||g_gamepad_mode.load(std::memory_order_acquire)!=3){g_follow_want147.store(0,std::memory_order_release);return -1;}
    const auto now=std::uint32_t(GetTickCount64());
    g_follow_want147.store(now?now:1u,std::memory_order_release);
    const auto answer=g_follow_answer147.load(std::memory_order_acquire);
    if(!answer||now-std::uint32_t(answer>>2)>80)return -1;
    return int(answer&3)-1;
}

void SetStickDirectionKeys(bool enabled) noexcept { g_pad_stick_keys.store(enabled,std::memory_order_relaxed); }
bool NativeLeftStickReady() noexcept { auto*a=pad_slot("xi_thumblx"),*b=pad_slot("xi_thumbly"); return a&&b&&a->valid.load()&&b->valid.load(); }
bool NativeRightStickReady() noexcept { auto*a=pad_slot("xi_thumbrx"),*b=pad_slot("xi_thumbry"); return a&&b&&a->valid.load()&&b->valid.load(); }
bool NativeGamepadCoreReady() noexcept {
    const char* req[]={"xi_thumblx","xi_thumbly","xi_thumbrx","xi_thumbry","xi_triggerl","xi_triggerr","xi_a","xi_b","xi_x","xi_y"};
    for(const char* n:req){ auto*s=pad_slot(n); if(!s||!s->valid.load(std::memory_order_acquire)) return false; } return true;
}
bool NativeGamepadFullReady() noexcept {
    // V65.11 F11 rung 1 is allowed to take transport ownership only when every
    // symbol used by our semantic VR premap exists. This prevents a partial XI
    // bootstrap from silently dropping block, Skip Time, menus or LG chords.
    const char* req[]={
        "xi_thumblx","xi_thumbly","xi_thumbrx","xi_thumbry",
        "xi_triggerl","xi_triggerr","xi_triggerl_btn","xi_triggerr_btn",
        "xi_a","xi_b","xi_x","xi_y","xi_shoulderl","xi_shoulderr",
        "xi_thumbl","xi_thumbr","xi_start","xi_back",
        "xi_dpad_up","xi_dpad_down","xi_dpad_left","xi_dpad_right"
    };
    for(const char* n:req){ auto*s=pad_slot(n); if(!s||!s->valid.load(std::memory_order_acquire)) return false; }
    return true;
}
void ArmTransportHeartbeat() noexcept {
    g_transport_heartbeat_budget.store(2,std::memory_order_release);
    g_transport_heartbeat_until_ms.store(static_cast<std::uint64_t>(GetTickCount64())+25ull,std::memory_order_release);
}
}
