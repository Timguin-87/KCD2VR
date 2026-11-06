#include "activity_input163.h"
#include "diagnostics/capture168.h"
#include "diagnostics/interaction_receipt59c10.h"
#include "diagnostic_trace.h"
#include "retail_build112.h"
#include "vrik/alchemy_input19c4.h"
#include "subsystems/alchemy/bridge67.h"
#include "frik_pose_probe.h"
#include "subsystems/smithing/native_contact68.inl"
#include "subsystems/dice/hand_select_d85.h" // D85 dice
#include "subsystems/dice/selection_marker_r92.h"
#include "subsystems/dice/hud_depth_r92r3.h"
#include "vrik/visible_failure.h"
#include "core/native_door_pins19c4.h"
#include <intrin.h>
#include <windows.h>
#include <DetourModKit/hook.hpp>
#include <array>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <optional>
#include <utility>

namespace kcd2vr::activity_input163 {
namespace {
// Exact retail evidence and script/RTTI call chains:
// verification/UI_NATIVE_BOUNDARIES_V175.md and tools/audit_ui175.py.
std::atomic<std::uintptr_t> base175{0};
// All 12 direct callers discard RAX before another call/assignment. Start's
// 32-bit EDX is stored at +0x28; its common return sets no result. See audit.
using Start175=void(__fastcall*)(void*,unsigned);
using End175=void(__fastcall*)(void*);
Start175 originalStart175=nullptr;
End175 originalStop175=nullptr,originalDestroy175=nullptr;
std::array<std::optional<DetourModKit::hook::Hook>,3> hooks175;
End175 originalSmithMapC1=nullptr;
std::optional<DetourModKit::hook::Hook> smithHookC1;
std::optional<DetourModKit::hook::Hook> alchemyHoldHook19c4;
template<class T>bool AlchemyInputRead19c4(std::uintptr_t at,T& value) noexcept {
    __try {value=*reinterpret_cast<const T*>(at);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// C37 (F5 rung 1 / Interaction recorder only): alchemy stage receipts for the
// tilt-pour design. Sampled on the existing IsUsableByHold callback; read-only;
// formats ONLY on an edge (alchemy_input19c4::Edge37). Proven layout reused from
// alchemy_tool18: owner+304 selected type, owner+78 director (3BFA5B0) with live
// slot vector +B0/+B8 (slot 1 = current tool action), action+28 started, and for
// C_UseResourceAction (3A5C670) only, child +78 (3A47948) status +28 / token +54.
#define KCD2VR_ALCHEMY_STAGE37 1
static void AlchemyStageReceipt37(std::uintptr_t self,std::uint64_t lifetime,unsigned type,std::uint32_t entity,
                                  std::uintptr_t text,bool hold,std::uint32_t selectedEntity) noexcept {
    using namespace diagnostics::capture168;
    const auto now=std::uint64_t(GetTickCount64());
    if(type>20||!active(Domain::Interaction,now))return;
    const auto b=base175.load(std::memory_order_acquire);if(!b)return;
    static std::atomic_flag busy=ATOMIC_FLAG_INIT;if(busy.test_and_set(std::memory_order_acquire))return; // diagnostic only: skip, never wait
    struct Release{std::atomic_flag& f;~Release(){f.clear(std::memory_order_release);}} release{busy};
    static kcd2vr::alchemy_input19c4::Edge37 edge{};
    static kcd2vr::alchemy_input19c4::ReceiptBudget38 budget{};static std::uint64_t previousSession=0;
    const auto session=gate.session[unsigned(Domain::Interaction)].load(std::memory_order_acquire);
    if(session!=previousSession){edge={};budget={};previousSession=session;}
    // C71 retains bounded state heartbeats after the older edge-only budget is exhausted.
    struct Sample50 {std::uint64_t session=0,tick=0;std::uint32_t entity=0;bool hold=false;};
    static Sample50 sampled[21];auto& prior50=sampled[type];
    if(prior50.session==session&&prior50.entity==entity&&prior50.hold==hold&&
       now>=prior50.tick&&now-prior50.tick<100)return;
    prior50={session,now,entity,hold}; // read-only diagnostics, original native query already ran

    kcd2vr::alchemy_input19c4::Stage37 stage{};stage.selectedEntity=selectedEntity;
    (void)AlchemyInputRead19c4(self+0x304,stage.selectedType);
    std::uintptr_t director=0,first=0,last=0;unsigned char started=0;
    if(AlchemyInputRead19c4(self+0x78,director)&&director==b+0x3BFA5B0&&AlchemyInputRead19c4(self+0xB0,first)&&
       AlchemyInputRead19c4(self+0xB8,last)&&first&&last>=first+16&&last-first<=256&&!((last-first)%8)&&
       AlchemyInputRead19c4(first+8,stage.action)&&stage.action&&AlchemyInputRead19c4(stage.action,stage.actionVft)){
        std::uintptr_t slot0=0;if(AlchemyInputRead19c4(first,slot0)&&slot0&&AlchemyInputRead19c4(slot0,stage.slot0Vft)&&stage.slot0Vft>=b)stage.slot0Vft-=b;else stage.slot0Vft=0;
        if(AlchemyInputRead19c4(stage.action+0x28,started))stage.started=started;
        std::uintptr_t childVft=0;
        if(stage.actionVft==b+0x3A5C670&&AlchemyInputRead19c4(stage.action+0x78,stage.child)&&stage.child&&
           AlchemyInputRead19c4(stage.child,childVft)&&childVft==b+0x3A47948){
            (void)AlchemyInputRead19c4(stage.child+0x28,stage.childStatus);(void)AlchemyInputRead19c4(stage.child+0x54,stage.childToken);
        }else stage.child=0;
        if(stage.actionVft>=b)stage.actionVft-=b;else stage.actionVft=0; // RVA, comparable across runs
    }else{stage.action=0;stage.actionVft=0;}
    kcd2vr::alchemy_input19c4::publishStage37(self,stage.actionVft,now); // R8b: alchemy stage for C10/C71/recorder
    char name[33]{};unsigned length=0;
    for(;text&&length+1<sizeof(name);++length){char c=0;if(!AlchemyInputRead19c4(text+length,c)||!c)break;name[length]=(c>=32&&c<127)?c:'?';}
    const kcd2vr::alchemy_input19c4::Hint37 hint{entity,kcd2vr::alchemy_input19c4::textHash37(name,length),hold};
    kcd2vr::alchemy_input19c4::Stage37 before{};
    const unsigned changed=budget.admit(edge.update(self,lifetime,type,hint,stage,before));
    if(changed&4)diagnostic_trace::Write("C37I1 F5_ALCHEMY_CAPTURE_LIMIT hintBudget=256 stageBudget=512 legacyEdges=SUPPRESSED C71_heartbeats=ACTIVE nativeState=UNCHANGED");

    static diagnostics::interaction_receipt59c10::Gate causal;
    if(causal.observe(session,now,{stage.action,selectedEntity,int(type),int(stage.childStatus)},100,8192,500)){
        char line[1000]{};std::snprintf(line,sizeof(line),"C71 F5_CRAFT_NATIVE session=%llu tick=%llu kind=ALCHEMY instance=%llX life=%llu queriedEntity=%u queriedType=%u selectedEntity=%u selectedType=%u hold=%d actionName=%s action=%llX actionClass=%s started=%d child=%llX childStatus=%d token=%u effectCompletion=UNOBSERVED",
            (unsigned long long)session,(unsigned long long)now,(unsigned long long)self,(unsigned long long)lifetime,entity,type,selectedEntity,stage.selectedType,hold,name,
            (unsigned long long)stage.action,kcd2vr::alchemy_input19c4::actionName37(stage.actionVft),stage.started,(unsigned long long)stage.child,stage.childStatus,stage.childToken);diagnostic_trace::Write(line);
    }
    if(changed&1){char m[256]{};std::snprintf(m,sizeof(m),
        "C37 F5_ALCHEMY_HINT type=%u entity=%u hold=%d action='%s' selectedType=%u selectedEntity=%u lifetime=%llu source=NATIVE_IsUsableByHold writes=NONE",
        type,entity,hold?1:0,name,stage.selectedType,selectedEntity,(unsigned long long)lifetime);diagnostic_trace::Write(m);}
    if(changed&2){char m[360]{};std::snprintf(m,sizeof(m),
        "C37 F5_ALCHEMY_STAGE event=%s selectedType=%u selectedEntity=%u action=%p class=%s actionVft=0x%llX started=%d slot0=%s child=%p childStatus=%d childToken=%u prevAction=%p prevClass=%s prevChildStatus=%d lifetime=%llu sampled=NATIVE_HINT_QUERY writes=NONE",
        kcd2vr::alchemy_input19c4::stageEvent37(before,stage),stage.selectedType,selectedEntity,reinterpret_cast<void*>(stage.action),
        kcd2vr::alchemy_input19c4::actionName37(stage.actionVft),(unsigned long long)stage.actionVft,stage.started,
        kcd2vr::alchemy_input19c4::actionName37(stage.slot0Vft),reinterpret_cast<void*>(stage.child),stage.childStatus,stage.childToken,
        reinterpret_cast<void*>(before.action),kcd2vr::alchemy_input19c4::actionName37(before.actionVft),before.childStatus,(unsigned long long)lifetime);diagnostic_trace::Write(m);}
}
#include "vrik/alchemy_input_observer19c4.inl"
#include "vrik/alchemy_selection38.inl"
#include "vrik/alchemy_book45.inl"
bool VerifyAlchemyInput19c4(std::uintptr_t b) noexcept {
    __try {
        constexpr unsigned char entry[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x41,0x8B,0xC0,0x48,0x8B,0xDA,0x83,0xF8,0x11};
        constexpr unsigned char caller[]{0xE8,0x3A,0x00,0x00,0x00};
        constexpr unsigned char holdResult[]{0x48,0x8B,0x08,0x40,0x38,0x31};
        constexpr unsigned char actionString[]{0xFF,0x90,0xD8,0x00,0x00,0x00};
        constexpr unsigned char nativeSelection[]{0x48,0x8b,0x47,0x18,0x48,0x8b,0x90,0x08,0x0b,0x00,0x00,0x48,0x8b,0x01,0x8b,0x92,0xf4,0x00,0x00,0x00,0xff,0x50,0x70};
        constexpr unsigned char interactorOwner[]{0x48,0x8b,0x41,0x08,0x8b,0xea,0x48,0x8b,0x88,0x90,0x09,0x00,0x00};
        return !std::memcmp(reinterpret_cast<void*>(b+0xD5AD38),nativeSelection,sizeof(nativeSelection))&&
            !std::memcmp(reinterpret_cast<void*>(b+0x82CF28),interactorOwner,sizeof(interactorOwner))&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3A6A0C8)==b+0x19D2578&&
            !std::memcmp(reinterpret_cast<void*>(b+0x8D1ECC),entry,sizeof(entry))&&
            !std::memcmp(reinterpret_cast<void*>(b+0x8D1E8D),caller,sizeof(caller))&&
            !std::memcmp(reinterpret_cast<void*>(b+0x8D1E97),holdResult,sizeof(holdResult))&&
            !std::memcmp(reinterpret_cast<void*>(b+0x8D1EFF),actionString,sizeof(actionString));
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
Kind175 ActiveKind175(void*self) noexcept {
    const auto b=base175.load(std::memory_order_acquire);if(!b||!self)return Kind175::Unknown;
    __try{
        const auto p=reinterpret_cast<std::uintptr_t>(self);
        if(*reinterpret_cast<const unsigned char*>(p+0x2C)!=1)return Kind175::Unknown;
        const auto vt=*reinterpret_cast<const std::uintptr_t*>(p);
        Kind175 kind=Kind175::Unknown;
        if(vt==b+0x3F60B08)kind=Kind175::Alchemy;
        else if(vt==b+0x3A6A390)kind=Kind175::Smithing;
        else if(vt==b+0x3F749E0)kind=Kind175::Reading;
        // C7F: C_Sharpening primary subobject; +2EB13D7 passes self to
        // C_Minigame::Start (+9F0970). The +18 player and +2C active fields are
        // inherited from that exact base. No SDK slot or camera write added.
        else if(vt==b+0x3F74BA0)kind=Kind175::Sharpening;
        // C_ForgeBuilder primary RTTI/vft. Its 2027960 Start calls this same
        // C_Minigame::Start at 20279DA; Stop2027AF0 calls 9F1150 at 2027B8C.
        // The inherited +18 player/+2C running contract is checked below.
        else if(vt==b+0x3F6FA30)kind=Kind175::ForgeBuilder;
        // C_Dice primary RTTI offset0; Starts 2E972B5/2E975AC/2E97C3C
        // pass this unchanged to the existing base Start hook. Stop2E98638
        // reaches the same base Stop through 9F0160 -> 9F1150.
        else if(vt==b+0x3F72098)kind=Kind175::Dice;
        if(kind==Kind175::Unknown)return kind;
        const auto player=*reinterpret_cast<const std::uintptr_t*>(p+0x18);
        // C_Player is the same supported local-player primary vft used by the
        // native input/combat adapters. NPC minigame instances are excluded.
        if(!player||*reinterpret_cast<const std::uintptr_t*>(player)!=b+0x3AA4B98||
           !*reinterpret_cast<const std::uintptr_t*>(player+0x38))return Kind175::Unknown;
        return kind;
    }__except(EXCEPTION_EXECUTE_HANDLER){return Kind175::Unknown;}
}
void Trace175(const char*phase,void*self,Kind175 kind) noexcept {
    using namespace diagnostics::capture168;
    if(!(gate.mask.load(std::memory_order_relaxed)&(1u<<unsigned(Domain::Interaction)))||
       !active(Domain::Interaction,GetTickCount64()))return;
    char message[224]{};
    std::snprintf(message,sizeof(message),"V66.175 ACTIVITY_NATIVE phase=%s instance=%p kind=%u qualified=%d source=MINIGAME_NATIVE_LIFECYCLE authoredControls=%d actionMapHook=RETIRED",
        phase,self,unsigned(kind),kind!=Kind175::Unknown,witness175.Active());
    diagnostic_trace::Write(message);
}
// C10R19: Start can synchronously update a FirstPerson view before its
// post-original witness is published. Do not train an anatomical eye inside
// that authored transition. Every scope unwinds its own depth, including nested
// lifecycle calls; no frame timer, native poll or persistent guessed flag.
struct TransitionScope19 {
    TransitionScope19() noexcept {witness175.transitionDepth175.fetch_add(1,std::memory_order_acq_rel);}
    ~TransitionScope19(){witness175.transitionDepth175.fetch_sub(1,std::memory_order_acq_rel);}
};
void __fastcall NativeStart175(void*self,unsigned target){
    TransitionScope19 transition;
    const auto enteredAt=std::uint64_t(GetTickCount64());
    originalStart175(self,target); // exact original, same arguments, once
    if(!witness175.ready.load(std::memory_order_acquire))return;
    const auto kind=ActiveKind175(self);
    // Retail restart calls the derived Stop (vft+28 -> 9F1150) INSIDE
    // originalStart175. Preserve that witnessed End and the new Begin as two
    // lifecycle events; pointer equality alone cannot authorize continuation.
    witness175.End(reinterpret_cast<std::uintptr_t>(self));
    kcd2vr::alchemy_input19c4::clear();
    if(kind!=Kind175::Unknown)witness175.Begin(kind,reinterpret_cast<std::uintptr_t>(self),enteredAt);
    Trace175("START_RETURN",self,kind);
}
void __fastcall NativeStop175(void*self){
    TransitionScope19 transition;
    const auto kind=witness175.End(reinterpret_cast<std::uintptr_t>(self));
    if(kind==Kind175::Alchemy)kcd2vr::alchemy_input19c4::clear();
    originalStop175(self);
    Trace175("STOP",self,kind);
}
void __fastcall NativeDestroy175(void*self){
    TransitionScope19 transition;
    const auto kind=witness175.End(reinterpret_cast<std::uintptr_t>(self));
    if(kind==Kind175::Alchemy)kcd2vr::alchemy_input19c4::clear();
    originalDestroy175(self); // no dereference after the destructor
    Trace175("DESTROY",self,kind);
}
// R20C1: the smithing listener's own map-refresh callback, not the retired global
// action-map-enable observer. RCX is the exact listener; its tail thunk at
// 0x85786C forwards RCX, and 0x858188 consumes no other arguments/result.
// Read-only after original; never call an SDK vtable or poll from the worker.
std::uintptr_t SmithListenerOwnerC1(void*listener) noexcept {
    const auto b=base175.load(std::memory_order_acquire);if(!b||!listener)return 0;
    __try{
        const auto l=reinterpret_cast<std::uintptr_t>(listener);
        if(*reinterpret_cast<const std::uintptr_t*>(l)!=b+0x3F67880)return 0;
        const auto owner=*reinterpret_cast<const std::uintptr_t*>(l+8);
        return owner>0x68?owner-0x68:0;
    }__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
int SmithPhaseFromListenerC1(void*listener,std::uintptr_t instance) noexcept {
    const auto b=base175.load(std::memory_order_acquire);if(!b||!listener||!instance)return -1;
    __try{
        const auto l=reinterpret_cast<std::uintptr_t>(listener);
        if(*reinterpret_cast<const std::uintptr_t*>(l)!=b+0x3F67880||
           *reinterpret_cast<const std::uintptr_t*>(l+8)!=instance+0x68||
           ActiveKind175(reinterpret_cast<void*>(instance))!=Kind175::Smithing||
           *reinterpret_cast<const std::uintptr_t*>(instance+0x68)!=b+0x3A6A320)return -1;
        // Retail secondary getter 0x72DD20 is mov rax,[rcx+30h]; rcx=self+68h.
        const auto context=*reinterpret_cast<const std::uintptr_t*>(instance+0x98);
        if(!context||*reinterpret_cast<const std::uintptr_t*>(context)!=b+0x3A6A280||
           *reinterpret_cast<const std::uintptr_t*>(context+0x80)!=b+0x3A6A208)return -1;
        // Property getter 0x1A7DE10 is movzx eax,byte ptr [rcx+8].
        const int phase=*reinterpret_cast<const unsigned char*>(context+0x88);
        return phase<=20?phase:-1;
    }__except(EXCEPTION_EXECUTE_HANDLER){return -1;}
}
void __fastcall NativeSmithMapC1(void*self){
    originalSmithMapC1(self);
    if(!witness175.ready.load(std::memory_order_acquire))return;
    const auto epoch=witness175.smithEpochC1.load(std::memory_order_acquire);
    const auto instance=witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire);
    if(!instance||SmithListenerOwnerC1(self)!=instance)return;
    const int phase=SmithPhaseFromListenerC1(self,instance);
    // Unknown phase/layout of the CURRENT instance must revoke old permission.
    witness175.smithSampleC1.store((epoch<<8)|unsigned(phase<0?255:phase),std::memory_order_release);
}
bool VerifySmithC1(std::uintptr_t b) noexcept {
    __try{
        constexpr unsigned char refresh[]={0x48,0x89,0x5C,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0x33,0xD2};
        constexpr unsigned char getter[]={0x48,0x8B,0x41,0x30,0xC3};
        constexpr unsigned char property[]={0x0F,0xB6,0x41,0x08,0xC3};
        return !std::memcmp(reinterpret_cast<void*>(b+0x858188),refresh,sizeof(refresh))&&
            !std::memcmp(reinterpret_cast<void*>(b+0x72DD20),getter,sizeof(getter))&&
            !std::memcmp(reinterpret_cast<void*>(b+0x1A7DE10),property,sizeof(property))&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3A6A380)==b+0x72DD20&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3A6A210)==b+0x1A7DE10;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool Verify175(std::uintptr_t b) noexcept {
    __try{
        constexpr unsigned char start[]={0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x40,0x80,0x79,0x2C,0x00,0x8B,0xFA};
        constexpr unsigned char stop[]={0x40,0x53,0x48,0x83,0xEC,0x40,0x48,0x8B,0x41,0x18,0x48,0x8D,0x54,0x24,0x20,0x48,0x8B,0xD9};
        constexpr unsigned char destroy[]={0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x48,0x8D,0x05,0xF2,0x96,0x07,0x03};
        return std::memcmp(reinterpret_cast<void*>(b+0x9F0970),start,sizeof(start))==0&&
            std::memcmp(reinterpret_cast<void*>(b+0x9F1150),stop,sizeof(stop))==0&&
            std::memcmp(reinterpret_cast<void*>(b+0x9F0E60),destroy,sizeof(destroy))==0&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3F60B08)==b+0x1A78EB0&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3A6A390)==b+0x1A99DC0&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3F749E0)==b+0x1A74A40&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3F74BA0)==b+0x41A6A0&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3F74C90)==b+0x9F0A44&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3F72098)==b+0x1A7F810&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3F720C0)==b+0x2E98638&&
            !std::memcmp(reinterpret_cast<void*>(b+0x2E972B5),"\xE8\xB6\x96\xB5\xFD",5)&&
            !std::memcmp(reinterpret_cast<void*>(b+0x2E98A2E),"\xE8\x2D\x77\xB5\xFD",5)&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3F6FA30)==b+0x1A86DD0&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3F6FA58)==b+0x2027AF0&&
            *reinterpret_cast<const std::uintptr_t*>(b+0x3F6FB20)==b+0x9F0A44&&
            !std::memcmp(reinterpret_cast<void*>(b+0x20279DA),"\xE8\x91\x8F\x9C\xFE",5)&&
            !std::memcmp(reinterpret_cast<void*>(b+0x2027B8C),"\xE8\xBF\x95\x9C\xFE",5)&&
            !std::memcmp(reinterpret_cast<void*>(b+0x2EB13D7),"\xE8\x94\xF5\xB3\xFD",5);
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class Fn>bool Hook175(unsigned slot,std::uintptr_t rva,Fn detour,Fn&original,const char*name){
    DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
    DetourModKit::hook::InlineRequest request{name,DetourModKit::hook::Target{
        DetourModKit::Address{base175.load()+rva}},options};
    auto hook=DetourModKit::hook::inline_at(std::move(request),detour);if(!hook)return false;
    hooks175[slot]=std::move(*hook);original=hooks175[slot]->original<Fn>();
    return original&&bool(hooks175[slot]->enable());
}
#include "subsystems/dice/diagnostic_d84.inl"
#include "subsystems/dice/hand_select_native_d85.inl" // D85 dice
}
void AlchemySelectionNativeFrameEnd38() noexcept {
    AlchemySelectionNativeFrameEndImpl38();
    AlchemyBookNativeFrameEndImpl45(); // C45: same completed-frame pump, no new hook
    DiceNativeFrameEndD84(); // F5-only read-only native dice state; no cup control
    DiceHandsNativeFrameEndD85(); // D85 dice: point to choose (VrDiceHandsD85, OFF by default)
}
bool InstallNative175() noexcept {
    if(witness175.ready.load(std::memory_order_acquire))return true;
    // A failed optional-hook disarm retains its trampoline and vetoes reinstall.
    if(alchemyHoldHook19c4)return false;
    const auto b=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!retail_build112::Verify(b)||!Verify175(b))return false;
    base175.store(b,std::memory_order_release);witness175.Clear();
    if(!Hook175(0,0x9F0E60,&NativeDestroy175,originalDestroy175,"kcd2vr_minigame_destroy175")||
       !Hook175(1,0x9F1150,&NativeStop175,originalStop175,"kcd2vr_minigame_stop175")||
       !Hook175(2,0x9F0970,&NativeStart175,originalStart175,"kcd2vr_minigame_start175")){
        UninstallNative175();return false;
    }
    // Optional phase observation: failure disables only smithing gestures.
    // Ordinary lifecycle/native controls and alchemy continue to function.
    if(VerifySmithC1(b)){
        DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
        DetourModKit::hook::InlineRequest request{"kcd2vr_smith_phase_r20c1",
            DetourModKit::hook::Target{DetourModKit::Address{b+0x858188}},options};
        auto hook=DetourModKit::hook::inline_at(std::move(request),&NativeSmithMapC1);
        if(hook){smithHookC1=std::move(*hook);originalSmithMapC1=smithHookC1->original<End175>();
            if(!originalSmithMapC1||!smithHookC1->enable())smithHookC1.reset();}
    }
    diagnostic_trace::Write(smithHookC1?"R20C1 SMITH_PHASE observer=READY scope=EXACT_NATIVE_LISTENER readOnly=1":
        "R20C1 SMITH_PHASE observer=UNAVAILABLE hammerGesture=DISABLED nativeControls=UNCHANGED");
    if(VerifyAlchemyInput19c4(b)){
        DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
        DetourModKit::hook::InlineRequest request{"kcd2vr_alchemy_hold_hint19c4",
            DetourModKit::hook::Target{DetourModKit::Address{b+0x8D1ECC}},options};
        auto hook=DetourModKit::hook::inline_at(std::move(request),&NativeAlchemyHold19c4);
        if(hook){alchemyHoldHook19c4=std::move(*hook);originalAlchemyHold19c4=alchemyHoldHook19c4->original<AlchemyHold19c4>();
            if(originalAlchemyHold19c4&&alchemyHoldHook19c4->enable())alchemyHoldReady19c4.store(true,std::memory_order_release);
            else if(alchemyHoldHook19c4->disable()){alchemyHoldHook19c4.reset();originalAlchemyHold19c4=nullptr;}}
    }
    diagnostic_trace::Write(alchemyHoldReady19c4.load()?"C19C4 ALCHEMY_INPUT observer=READY source=NATIVE_IsUsableByHold scope=F5_2_RELEASE_HINT_PULSE":
        "C19C4 ALCHEMY_INPUT observer=UNAVAILABLE mapping=NATIVE_LEGACY_HOLD");
    subsystems::smithing::contact68::readHammer=&frik_pose_probe::ReadSmithContactHammer68;
    subsystems::smithing::contact68::findEntity=&frik_pose_probe::FindSmithContactEntity68;
    diagnostic_trace::Write(subsystems::smithing::contact68::install(b)?
        "G1R1 SMITH_CONTACT68 observer=READY scope=F5_1_NATIVE_STROKEHIT writes=NONE":
        "G1R1 SMITH_CONTACT68 observer=UNAVAILABLE nativeSmithing=UNCHANGED");
    witness175.ready.store(true,std::memory_order_release);return true;
}
void UninstallNative175() noexcept {
    witness175.ready.store(false,std::memory_order_release);witness175.Clear();
    (void)subsystems::smithing::contact68::uninstall();
    alchemyHoldReady19c4.store(false,std::memory_order_release);kcd2vr::alchemy_input19c4::clear();
    if(alchemyHoldHook19c4&&alchemyHoldHook19c4->disable()){alchemyHoldHook19c4.reset();originalAlchemyHold19c4=nullptr;}
    if(smithHookC1){(void)smithHookC1->disable();smithHookC1.reset();}
    for(auto&hook:hooks175)if(hook){(void)hook->disable();hook.reset();}
}
} // namespace kcd2vr::activity_input163
