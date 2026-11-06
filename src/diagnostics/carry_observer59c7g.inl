// F5 observation of the native carry-state query. Never calls it proactively,
// changes its result, selects an NPC, requests carry, or writes physics.
using CarryQuery59c7g=bool(__fastcall*)(void*);
static CarryQuery59c7g g_carry_original59c7g=nullptr;
static std::optional<DetourModKit::hook::Hook> g_carry_hook59c7g;
static bool __fastcall CarryQueryDetour59c7g(void* actor) noexcept {
    const bool result=g_carry_original59c7g?g_carry_original59c7g(actor):false;
    if(!LifecycleActive165())return result;
    ReadScope101R2 scope;void* entity=nullptr;void* module=nullptr;void* back=nullptr;
    std::uint32_t actorId=0,targetId=0;int state=-1;std::uintptr_t actorVft=0,moduleVft=0;
    if(!actor||!Read(actor,actorVft)||actorVft!=WhBase()+0x3AA4B98||!Read(static_cast<unsigned char*>(actor)+0x38,entity)||
       entity!=g_last_player_entity.load()||!LifecycleEntity165(reinterpret_cast<std::uintptr_t>(entity),actorId)||
       !Read(static_cast<unsigned char*>(actor)+0x188,module)||!module||!Read(module,moduleVft)||moduleVft!=WhBase()+0x3A2F438||
       !Read(static_cast<unsigned char*>(module)+0xD8,back)||back!=actor||
       !Read(static_cast<unsigned char*>(module)+0x68,state)||state<0||state>8||
       !Read(static_cast<unsigned char*>(module)+0x80,targetId))return result;
    // Scope belongs to this native call/thread. No saved dereferenceable pointer.
    static thread_local kcd2vr::carry59c7g::Budget budget;
    auto e=LifecycleEvent165(il165::Kind::CarrySnapshot59c7g,0xB26BF0);
    if(!budget.accept(e.session,e.tick,reinterpret_cast<std::uintptr_t>(module),targetId,state))return result;
    e.actor=actorId;e.controller=reinterpret_cast<std::uintptr_t>(module);
    // +285E53C resolves this through the actor service, not IEntitySystem.
    // Preserve the raw native key until that mapping is qualified; never treat
    // it as an entity id and accidentally join an unrelated physics body.
    e.key=targetId;e.state=state;e.result=result?1:0;e.valid=result==(state==5)?1u:0u;
    LifecycleEmit165(e);return result;
}
static bool ArmCarryObserver59c7g() noexcept {
    if(g_carry_hook59c7g)return g_carry_original59c7g!=nullptr;
    if(!retail_build112::Ready(WhBase()))return false;
    struct Pin{std::uintptr_t rva;unsigned count;unsigned char bytes[16];};
    static constexpr Pin pins[]{
        {0xB26BF0,16,{0x48,0x8B,0x89,0x88,0x01,0x00,0x00,0x48,0x85,0xC9,0x74,0x07,0x48,0x8B,0x01,0x48}},
        {0xB26C08,8,{0x83,0x79,0x68,0x05,0x0F,0x94,0xC0,0xC3}},
        {0xDF4974,11,{0x83,0xB9,0x80,0x00,0x00,0x00,0x00,0x0F,0x95,0xC0,0xC3}},
        {0x285E568,14,{0x48,0x8B,0x81,0xD8,0x00,0x00,0x00,0x39,0x50,0x30,0x74,0x32,0x83,0x79}},
    };
    for(const auto& p:pins)for(unsigned i=0;i<p.count;++i){unsigned char value=0;
        if(!Read(reinterpret_cast<void*>(WhBase()+p.rva+i),value)||value!=p.bytes[i])return false;}
    std::uintptr_t method=0;
    if(!Read(reinterpret_cast<void*>(WhBase()+0x3A2F438+0x18),method)||method!=WhBase()+0xB26C08)return false;
    using namespace DetourModKit::hook;Options o{};o.fail_if_already_hooked=true;
    auto hook=inline_at(InlineRequest{"kcd2vr_carry_query59c7g",Target{DetourModKit::Address{WhBase()+0xB26BF0}},o},&CarryQueryDetour59c7g);
    if(!hook)return false;g_carry_hook59c7g=std::move(*hook);
    g_carry_original59c7g=g_carry_hook59c7g->original<CarryQuery59c7g>();
    if(g_carry_original59c7g&&g_carry_hook59c7g->enable())return true;
    g_carry_hook59c7g.reset();g_carry_original59c7g=nullptr;return false;
}
static void ShutdownCarryObserver59c7g() noexcept {
    if(g_carry_hook59c7g)(void)g_carry_hook59c7g->disable();
    g_carry_hook59c7g.reset();g_carry_original59c7g=nullptr;
}
