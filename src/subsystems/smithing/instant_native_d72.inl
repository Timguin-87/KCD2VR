// Included after semi_smith_clock46 in the private native owner namespace.
// Exact retail 858D6C is C_BlacksmithingProcess::Update(process,dt). The native
// bool setter857874(instance+68,on) is the bellows semantic leaf; it notifies
// model+108 after changing model+100. All native dt, thermal regions, cooling,
// quality/burn checks and feedback in the original Update still execute once.
namespace si72=kcd2vr::subsystems::smithing::instant_d72;
using SmithUpdateD72=void(__fastcall*)(void*,float);
static std::optional<DetourModKit::hook::Hook> g_smith_instant_hook_d72;
static SmithUpdateD72 g_smith_instant_original_d72=nullptr;
static std::atomic<bool> g_smith_instant_stop_d72{true},g_smith_pump_pending_d72{false};
static std::atomic<unsigned> g_smith_instant_inflight_d72{0};
struct SmithPumpOwnerD72 {
    std::uintptr_t instance=0,process=0,model=0,player=0;
    std::uint64_t lifetime=0;
    bool owned=false;
};
static SmithPumpOwnerD72 g_smith_pump_d72{};
struct SmithContextD72 {
    std::uintptr_t instance=0,process=0,model=0,player=0,holder=0;
    std::uint64_t lifetime=0;unsigned char phase=255,pumping=255;
};
static bool SmithContextCurrentD72(void* process,SmithContextD72& c) noexcept {
    namespace a=kcd2vr::activity_input163;const auto& w=a::witness175;
    c={};const auto b=g_base.load();c.process=reinterpret_cast<std::uintptr_t>(process);
    if(!process||!b||!retail_build112::Ready(b)||!w.ready.load()||w.transitionDepth175.load())return false;
    c.instance=w.active[unsigned(a::Kind175::Smithing)].load();c.lifetime=w.lifetime175.load();
    if(!c.instance||!c.lifetime||w.active[unsigned(a::Kind175::Alchemy)].load())return false;
    auto* p=reinterpret_cast<void*>(c.instance);
    c.player=Rd<std::uintptr_t>(p,0x18,0);c.model=Rd<std::uintptr_t>(p,0x98,0);
    auto* model=reinterpret_cast<void*>(c.model);auto* player=reinterpret_cast<void*>(c.player);
    if(Rd<std::uintptr_t>(p,0,0)!=b+0x3A6A390||Rd<unsigned char>(p,0x2C,0)!=1||
       Rd<std::uintptr_t>(process,0,0)!=c.instance||Rd<std::uintptr_t>(p,0xB0,0)!=c.process||
       Rd<std::uintptr_t>(p,0x68,0)!=b+0x3A6A320||Rd<std::uintptr_t>(player,0,0)!=b+0x3AA4B98||
       !Rd<unsigned>(player,0x30,0)||Rd<unsigned>(player,0x30,0)!=HenryEntityId18()||
       Rd<std::uintptr_t>(model,0,0)!=b+0x3A6A280)return false;
    c.phase=Rd<unsigned char>(model,0x88,255);c.pumping=Rd<unsigned char>(model,0x100,255);
    c.holder=Rd<std::uintptr_t>(model,0x2F0,0);
    if(c.phase>20||c.pumping>1||!c.holder||Rd<std::uintptr_t>(reinterpret_cast<void*>(c.holder),0,0)!=b+0x3A7BAA8)return false;
    return w.ready.load()&&!w.transitionDepth175.load()&&w.lifetime175.load()==c.lifetime&&
        w.active[unsigned(a::Kind175::Smithing)].load()==c.instance&&
        Rd<std::uintptr_t>(p,0xB0,0)==c.process&&Rd<std::uintptr_t>(p,0x98,0)==c.model;
}
static bool SmithSetPumpD72(const SmithContextD72& c,bool value) noexcept {
    SmithContextD72 after{};
    if(!SmithContextCurrentD72(reinterpret_cast<void*>(c.process),after)||after.instance!=c.instance||
       after.lifetime!=c.lifetime||after.model!=c.model||after.player!=c.player||after.holder!=c.holder)return false;
    __try {reinterpret_cast<void(__fastcall*)(void*,bool)>(g_base.load()+0x857874)(
        reinterpret_cast<void*>(c.instance+0x68),value);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    return SmithContextCurrentD72(reinterpret_cast<void*>(c.process),after)&&after.instance==c.instance&&
        after.lifetime==c.lifetime&&after.model==c.model&&after.pumping==unsigned(value);
}
static void ServiceSmithPumpD72(void* process) noexcept {
    namespace a=kcd2vr::activity_input163;const auto& w=a::witness175;
    OwnerTransaction19c4 transaction;if(!transaction)return;
    auto& owner=g_smith_pump_d72;
    // Positive lifecycle retirement discards the obligation without touching an
    // old allocation. Unknown reads retain it for its exact next callback.
    if(owner.owned&&w.ready.load()&&!w.transitionDepth175.load()&&
       (w.lifetime175.load()!=owner.lifetime||w.active[unsigned(a::Kind175::Smithing)].load()!=owner.instance)){
        owner={};g_smith_pump_pending_d72.store(false);
    }
    SmithContextD72 c{};if(!SmithContextCurrentD72(process,c))return;
    const bool same=owner.owned&&owner.process==c.process&&owner.instance==c.instance&&
        owner.model==c.model&&owner.player==c.player&&owner.lifetime==c.lifetime;
    if(owner.owned&&!same)return;
    const auto now=std::uint64_t(GetTickCount64());si72::Pump p{};kcd2vr::workstation_sample46::Frame f{};
    kcd2vr::subsystems::smithing::source_h42::Source handle{};
    const auto rd=[](std::uintptr_t at,auto& v){return SehCopy(reinterpret_cast<void*>(at),&v,sizeof(v));};
    const bool current=!g_smith_instant_stop_d72.load()&&si72::route()&&c.phase==8&&
        si72::pump.read(p)&&si72::current(p,c.instance,c.lifetime,kcd2vr::minigame_hands45::modeEpoch67.load(),now)&&
        kcd2vr::subsystems::smithing::source_h42::read(rd,g_base.load(),c.instance,c.player,2,handle)&&
        kcd2vr::subsystems::smithing::source_h42::sameIdentity(p.handle,handle)&&
        frik_pose_probe::WorkstationFrame46(f)&&f.allowed&&kcd2vr::workstation_sample46::fresh(f,now)&&
        f.poseEpoch==p.poseEpoch&&f.sourceEpoch==p.sourceEpoch&&f.hand[0].valid&&
        kcd2vr::workstation_sample46::finite(f.hand[0]);
    if(same&&(!current||!p.pressed)){
        // Compare-owned release: a native false already released our request.
        // A manual RT handoff can retain a true native request; do not overwrite
        // manual input, but relinquish our claim to it before normal native work.
        si72::Pump latest{};const bool manual=si72::pump.read(latest)&&latest.manual&&latest.handle.instance==c.instance&&
            latest.lifetime==c.lifetime&&latest.tick&&now>=latest.tick&&now-latest.tick<=80;
        if(!c.pumping||manual||SmithSetPumpD72(c,false)){owner={};g_smith_pump_pending_d72.store(false);}
        ip72::Publish(ip72::Family::Crafting,owner.owned?ip72::State::Retiring:
            (!ip72::Crafting()?ip72::State::Native:current?ip72::State::Available:ip72::State::Unsupported));
        return;
    }
    if(!current){if(!owner.owned)ip72::Publish(ip72::Family::Crafting,ip72::Crafting()?ip72::State::Unsupported:ip72::State::Native);return;}
    if(!p.pressed){ip72::Publish(ip72::Family::Crafting,ip72::State::Available);return;}
    if(!same&&c.pumping){ip72::Publish(ip72::Family::Crafting,ip72::State::Unsupported);return;}
    if(!same){owner={c.instance,c.process,c.model,c.player,c.lifetime,true};g_smith_pump_pending_d72.store(true);}
    // Publish ownership before the native call: a partial failure is not a
    // licence to repeat the event or abandon an on-state during shutdown.
    const bool applied=c.pumping||SmithSetPumpD72(c,true);
    ip72::Publish(ip72::Family::Crafting,applied?ip72::State::Active:ip72::State::Retiring);
}
static void __fastcall SmithUpdateDetourD72(void* process,float dt) {
    g_smith_instant_inflight_d72.fetch_add(1);
    ServiceSmithPumpD72(process);
#if defined(_MSC_VER)
    __try {g_smith_instant_original_d72(process,dt);}
    __finally {g_smith_instant_inflight_d72.fetch_sub(1);}
#else
    g_smith_instant_original_d72(process,dt);g_smith_instant_inflight_d72.fetch_sub(1);
#endif
}
static bool InstallInstantSmithD72() noexcept {
    if(g_smith_instant_hook_d72)return si72::ready.load();
    const auto b=g_base.load();if(!b||!retail_build112::Ready(b))return false;
    constexpr unsigned char update[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18,0x48};
    constexpr unsigned char pump[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0x01,0x8A,0xDA,0xFF,0x50,0x60,0x38,0x98};
    constexpr unsigned char getter[]{0x48,0x8B,0x41,0x30,0xC3};
    if(!MatchBytes(b+0x858D6C,update,sizeof(update))||!MatchBytes(b+0x857874,pump,sizeof(pump))||
       !MatchBytes(b+0x72DD20,getter,sizeof(getter))||Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3A6A320),0x60,0)!=b+0x72DD20)return false;
    DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
    DetourModKit::hook::InlineRequest request{"smith_instant_process_d72",DetourModKit::hook::Target{DetourModKit::Address{b+0x858D6C}},options};
    auto hook=DetourModKit::hook::inline_at(std::move(request),&SmithUpdateDetourD72);if(!hook)return false;
    g_smith_instant_hook_d72=std::move(*hook);g_smith_instant_original_d72=g_smith_instant_hook_d72->original<SmithUpdateD72>();
    HMODULE module=nullptr;
    if(!g_smith_instant_original_d72||!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&SmithUpdateDetourD72),&module))return false;
    const bool enabled=bool(g_smith_instant_hook_d72->enable());g_smith_instant_stop_d72.store(!enabled);si72::ready.store(enabled);
    return enabled;
}
static bool StopInstantSmithD72() noexcept {
    g_smith_instant_stop_d72.store(true);si72::ready.store(false);
    namespace a=kcd2vr::activity_input163;const auto& w=a::witness175;
    if(g_smith_pump_d72.owned&&w.ready.load()&&!w.transitionDepth175.load()&&
       (w.lifetime175.load()!=g_smith_pump_d72.lifetime||
        w.active[unsigned(a::Kind175::Smithing)].load()!=g_smith_pump_d72.instance)){
        g_smith_pump_d72={};g_smith_pump_pending_d72.store(false);
    }
    return !g_smith_pump_pending_d72.load()&&!g_smith_instant_inflight_d72.load();
}
static bool DisarmInstantSmithD72() noexcept {
    return (!g_smith_instant_hook_d72||!g_smith_instant_hook_d72->is_enabled()||
        (bool(g_smith_instant_hook_d72->disable())&&!g_smith_instant_hook_d72->is_enabled()))&&
        !g_smith_instant_inflight_d72.load();
}
