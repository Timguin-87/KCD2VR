// Exact retail native master opportunity diagnosis. No eligibility function is
// called twice. Mid-hooks only copy the AL/EAX/EBX values produced by the real
// call. No registers, flags, branch destinations or native results are changed.
#include "../diagnostics/master_eligibility_h21.h"
namespace me21=kcd2vr::master_eligibility_h21;
using MasterEligibilityFnH21=bool(__fastcall*)(void*);
static MasterEligibilityFnH21 g_master_eligibility_original_h21=nullptr,g_master_context_original_h21=nullptr;
static std::optional<DetourModKit::hook::Hook> g_master_eligibility_hook_h21,g_master_context_hook_h21;
static std::array<std::optional<DetourModKit::hook::Hook>,9> g_master_terms_h21;
// DMK inline original() has no rundown. One preallocated, deliberately retained
// bucket keeps both callable trampolines alive after any inline enable attempt,
// including an error AFTER patch publication. Never recycle this observer in
// the same process. The provider DLL must also remain mapped (normal mod life).
struct MasterInlineRetirementH21 {
    std::optional<DetourModKit::hook::Hook> outer,context;
};
static MasterInlineRetirementH21* g_master_inline_retirement_h21=nullptr;
static bool g_master_inline_enable_attempted_h21=false,g_master_inline_retired_h21=false;
static std::atomic<bool> g_master_eligibility_ready_h21{false};
static std::atomic<std::uint64_t> g_master_eligibility_serial_h21{0};
struct MasterEligibilityScopeH21 {me21::Attempt attempt{};MasterEligibilityScopeH21* previous=nullptr;bool active=false;};
static thread_local MasterEligibilityScopeH21* g_master_scope_h21=nullptr;
static void MasterTermH21(DetourModKit::hook::MidContext& ctx,unsigned site) noexcept {
    auto* scope=g_master_scope_h21;if(!scope||!scope->active)return;
    using namespace DetourModKit::hook;
    if(gpr(ctx,Gpr::Rdi)!=scope->attempt.trigger)return;
    auto& a=scope->attempt;const bool al=(gpr(ctx,Gpr::Rax)&255)!=0;
    switch(site){
    case 0:a.observed(me21::Term::Ability,al);break;
    case 1:a.observed(me21::Term::OpponentPhase,al);break;
    case 2:a.observed(me21::Term::MutualPair,al);break;
    case 3:a.observed(me21::Term::DodgeExclusion,!al);break;
    case 4:if(al)a.observed(me21::Term::OwnPhase,true);else a.pending(me21::Term::OwnPhase);break;
    case 5:a.observed(me21::Term::OwnPhase,true);break; // shared +90A663 entry: native phase branch already passed
    case 6:a.observed(me21::Term::Zone,al);break;
    case 7:if(al)a.observed(me21::Term::OwnerActionExclusion,true);else a.pending(me21::Term::OwnerActionExclusion);break;
    case 8:a.observed(me21::Term::OwnerActionExclusion,true);break; // shared +90A6E9 entry, after either accepted owner-action branch
    }
}
template<unsigned Site> static void MasterTermHookH21(DetourModKit::hook::MidContext& ctx) noexcept {MasterTermH21(ctx,Site);}
static bool __fastcall MasterContextHookH21(void* actor) noexcept {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());auto* scope=g_master_scope_h21;
    const bool capture=scope&&scope->active&&scope->attempt.actor==reinterpret_cast<std::uintptr_t>(actor)&&caller==WhBase()+0x90A6B5;
    const bool result=g_master_context_original_h21?g_master_context_original_h21(actor):false;
    if(capture&&g_master_scope_h21==scope)scope->attempt.observed(me21::Term::Context,result);
    return result;
}
static bool __fastcall MasterEligibilityHookH21(void* trigger) noexcept {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    MasterEligibilityScopeH21 scope{};scope.previous=g_master_scope_h21;
    const auto now=std::uint64_t(GetTickCount64());
    if(g_master_eligibility_ready_h21.load(std::memory_order_acquire)&&
       diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
        ReadScope101R2 reads;void* actor=nullptr;void* opponent=nullptr;void* player=nullptr;NativeOwner104 owner{};
        std::uintptr_t vft=0;
        if(trigger&&Read(trigger,vft)&&vft==WhBase()+0x3B81528&&
           Read(static_cast<unsigned char*>(trigger)+8,actor)&&actor&&
           Read(static_cast<unsigned char*>(actor)+0x1F8,player)&&player&&NativeResolve104(player,owner)&&owner.actor==actor&&
           Read(static_cast<unsigned char*>(trigger)+0x38,opponent)){
            scope.active=true;auto& a=scope.attempt;a.trigger=reinterpret_cast<std::uintptr_t>(trigger);
            a.actor=reinterpret_cast<std::uintptr_t>(actor);a.opponent=reinterpret_cast<std::uintptr_t>(opponent);
            a.state=reinterpret_cast<std::uintptr_t>(owner.state);
            a.caller=caller>=WhBase()?caller-WhBase():0;a.tick=now;
            a.serial=g_master_eligibility_serial_h21.fetch_add(1,std::memory_order_relaxed)+1;
            (void)Read(static_cast<unsigned char*>(owner.state)+0x7D8,a.countBefore);
            (void)Read(static_cast<unsigned char*>(owner.state)+0xF8,a.stance);
            void* os=nullptr;if(opponent&&Read(static_cast<unsigned char*>(opponent)+0x210,os)&&os)
                (void)Read(static_cast<unsigned char*>(os)+0x158,a.offered);
        }
    }
    g_master_scope_h21=&scope;
    const bool result=g_master_eligibility_original_h21?g_master_eligibility_original_h21(trigger):false;
    g_master_scope_h21=scope.previous;
    if(scope.active){
        auto& a=scope.attempt;a.complete(result);
        ReadScope101R2 reads;void* currentActor=nullptr;void* currentState=nullptr;
        if(Read(static_cast<unsigned char*>(trigger)+8,currentActor)&&reinterpret_cast<std::uintptr_t>(currentActor)==a.actor&&
           Read(static_cast<unsigned char*>(currentActor)+0x210,currentState)&&reinterpret_cast<std::uintptr_t>(currentState)==a.state)
            (void)Read(static_cast<unsigned char*>(currentState)+0x7D8,a.countAfter);
        // Authored attempts always appear; active-tick repeats are collapsed to
        // changes, bounded to 256 records per F3 session on this native thread.
        static thread_local me21::Attempt last{};static thread_local std::uint64_t session=0;static thread_local unsigned lines=0;
        const auto currentSession=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
        if(session!=currentSession){session=currentSession;lines=0;last={};}
        if(lines<256&&(a.caller==0x909C6D||!me21::sameVerdict(a,last))){
            char message[1100]{};me21::format(a,message,sizeof(message));diagnostic_trace::Write(message);last=a;++lines;
        }
    }
    return result;
}
static void ShutdownMasterEligibilityH21() noexcept {
    g_master_eligibility_ready_h21.store(false,std::memory_order_release);
    if(g_master_inline_enable_attempted_h21)g_master_inline_retired_h21=true;
    // Stop outer admission first. A successful disable is NOT an in-flight
    // original completion witness. Retain even cleanly disabled inline leases;
    // no wait here, especially when shutdown is re-entered from a callback.
    const auto disarm=[](auto& hook){if(hook&&hook->is_enabled())(void)hook->disable();};
    disarm(g_master_eligibility_hook_h21);disarm(g_master_context_hook_h21);
    if(g_master_inline_retired_h21){
        // Allocated before the first factory/enable. This bucket is never
        // destroyed by static/DLL teardown and has capacity for exactly two.
        auto& retired=*g_master_inline_retirement_h21;
        if(g_master_eligibility_hook_h21){retired.outer=std::move(g_master_eligibility_hook_h21);g_master_eligibility_hook_h21.reset();}
        if(g_master_context_hook_h21){retired.context=std::move(g_master_context_hook_h21);g_master_context_hook_h21.reset();}
        disarm(retired.outer);disarm(retired.context); // retry an uncertain detach
    }
    // A failed detach still owns executable code and its trampoline. Keep both
    // alive; a subsequent shutdown may retry, never overwrite that lease.
    for(auto& hook:g_master_terms_h21)if(hook&&(!hook->is_enabled()||hook->disable()))hook.reset();
    // No inline enable was attempted: these disabled factories were never
    // executable and may be reclaimed after a pre-publication failure.
    if(!g_master_inline_retired_h21){
        if(g_master_context_hook_h21&&!g_master_context_hook_h21->is_enabled())g_master_context_hook_h21.reset();
        if(g_master_eligibility_hook_h21&&!g_master_eligibility_hook_h21->is_enabled())g_master_eligibility_hook_h21.reset();
    }
    bool pending=g_master_context_hook_h21.has_value()||g_master_eligibility_hook_h21.has_value();
    for(const auto& hook:g_master_terms_h21)pending=pending||hook.has_value();
    if(!pending&&!g_master_inline_retired_h21){g_master_context_original_h21=nullptr;g_master_eligibility_original_h21=nullptr;}
}
// Hook shared branch destinations, never an earlier compare whose displaced
// bytes contain another native entry. Rejected own-phase/owner-action paths may
// remain UNKNOWN; the recorder must not manufacture a verdict to fill that gap.
// Pin table and all-site 5/14-byte inbound-edge checks use the exact retail PE.
#include "master_eligibility_pins_h21.h"
static bool ArmMasterEligibilityH21() noexcept {
    if(g_master_inline_retired_h21)return false; // optional recorder needs restart, never duplicate old ownership
    if(g_master_eligibility_ready_h21.load())return true;
    ShutdownMasterEligibilityH21();
    if(g_master_inline_retired_h21)return false;
    if(g_master_context_hook_h21||g_master_eligibility_hook_h21)return false;
    for(const auto& hook:g_master_terms_h21)if(hook)return false;
    const auto base=WhBase();if(!retail_build112::Verify(base))return false;
    for(const auto& pin:kMasterEligibilityPinsH21){unsigned char bytes[16]{};
        if(!Read(reinterpret_cast<void*>(base+pin.rva),bytes)||std::memcmp(bytes,pin.bytes.data(),16))return false;}
    if(!g_master_inline_retirement_h21)g_master_inline_retirement_h21=new(std::nothrow) MasterInlineRetirementH21;
    if(!g_master_inline_retirement_h21)return false; // no hooks published without retirement capacity
    using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
    auto outer=inline_at(InlineRequest{"kcd2vr_master_eligibility_h21",Target{DetourModKit::Address{base+0x90A59C}},options},MasterEligibilityHookH21);
    if(!outer)return false;g_master_eligibility_hook_h21=std::move(*outer);
    g_master_eligibility_original_h21=g_master_eligibility_hook_h21->original<MasterEligibilityFnH21>();
    auto context=inline_at(InlineRequest{"kcd2vr_master_context_h21",Target{DetourModKit::Address{base+0x17FD570}},options},MasterContextHookH21);
    if(!context){ShutdownMasterEligibilityH21();return false;}g_master_context_hook_h21=std::move(*context);
    g_master_context_original_h21=g_master_context_hook_h21->original<MasterEligibilityFnH21>();
    constexpr std::uintptr_t sites[]{0x90A5DC,0x90A5FD,0x90A61B,0x90A62C,0x90A64C,0x90A663,0x90A6A8,0x90A6C9,0x90A6E9};
    constexpr const char* names[]{"kcd2vr_ms_ability_h21","kcd2vr_ms_opponent_phase_h21","kcd2vr_ms_pair_h21",
        "kcd2vr_ms_dodge_h21","kcd2vr_ms_candidate_h21","kcd2vr_ms_own_phase_h21","kcd2vr_ms_zone_h21",
        "kcd2vr_ms_owner_action_h21","kcd2vr_ms_opponent_action_h21"};
    const auto install=[&](unsigned i,auto callback){auto hook=mid_at(MidRequest{names[i],Target{DetourModKit::Address{base+sites[i]}},options},callback);
        if(!hook)return false;g_master_terms_h21[i]=std::move(*hook);return true;};
    const bool installed=install(0,MasterTermHookH21<0>)&&install(1,MasterTermHookH21<1>)&&install(2,MasterTermHookH21<2>)&&
        install(3,MasterTermHookH21<3>)&&install(4,MasterTermHookH21<4>)&&install(5,MasterTermHookH21<5>)&&
        install(6,MasterTermHookH21<6>)&&install(7,MasterTermHookH21<7>)&&install(8,MasterTermHookH21<8>);
    if(!installed||!g_master_eligibility_original_h21||!g_master_context_original_h21){ShutdownMasterEligibilityH21();return false;}
    for(auto& hook:g_master_terms_h21)if(!hook->enable()){ShutdownMasterEligibilityH21();return false;}
    g_master_inline_enable_attempted_h21=true; // before enable: DMK error may still mean ACTIVE
    if(!g_master_context_hook_h21->enable()||!g_master_eligibility_hook_h21->enable()){ShutdownMasterEligibilityH21();return false;}
    g_master_eligibility_ready_h21.store(true,std::memory_order_release);
    diagnostic_trace::Write("H21 MS_ELIGIBILITY arm=READY terms=9 source=NATIVE_90A59C F3_ONLY nativeWrites=NONE");return true;
}
