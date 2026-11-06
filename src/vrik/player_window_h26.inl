// Native float B27958(handler,attacker), exact retail only. The two supported
// handler vfts are proved by retail RTTI and native +68 category getters.
using PlayerWindowFnH26=float(__fastcall*)(void*,void*);
static PlayerWindowFnH26 g_player_window_originalH26=nullptr;
static std::optional<DetourModKit::hook::Hook> g_player_window_hookH26;
static std::atomic<bool> g_player_window_readyH26{false},g_player_window_retiredH26{false};
struct PlayerWindowSourceH26 {
    std::uint64_t request=0,tick=0,key=0,generation=0;bool fist=false;
    bool same(const PlayerWindowSourceH26& o)const noexcept {
        return request==o.request&&tick==o.tick&&key==o.key&&generation==o.generation&&fist==o.fist;
    }
};
static bool PlayerWindowMotionH26(const NativeOwner104& owner,std::uint64_t now,PlayerWindowSourceH26& source) noexcept {
    source={};
    if(FistSelected101()){
        FistFrame101 f{};
        if(!g_fist_fault101.load()&&g_fist_frame101.read(f)&&f.player==owner.player&&
            f.request==g_fist_epoch101.load()&&f.sample.epoch==owner.epoch&&f.sample.valid&&
            kcd2vr::player_window_h26::fresh(f.sample.tick,now,80)){
            source={f.request,f.sample.tick,0,0,true};return true;
        }return false;
    }
    PhysFrame6698 f{};
    if(!g_phys_fault6698.load()&&g_phys_frame6698.read(f)&&f.player==owner.player&&f.input.allowed&&
        f.profiles[0].melee()&&f.profiles[0].key&&f.profiles[0].generation&&
        f.input.bodyEpoch==owner.epoch&&f.request==g_weapon_request_epoch6694.load()&&
        f.input.epoch==f.request&&kcd2vr::player_window_h26::fresh(f.input.tick,now,80)){
        source={f.request,f.input.tick,f.profiles[0].key,f.profiles[0].generation,false};return true;
    }return false;
}
static bool PlayerWindowOwnerH26(void* handler,void* attacker,NativeOwner104& owner,std::uintptr_t& kind,PlayerWindowSourceH26& source) noexcept {
    const auto base=WhBase();const auto now=std::uint64_t(GetTickCount64());kind=0;owner={};
    if(!g_player_window_readyH26.load(std::memory_order_acquire)||g_player_window_retiredH26.load(std::memory_order_acquire)||
       !retail_build112::Ready(base)||!CombatExperiment175Enabled()||diagnostics::lab::NativeReference()||
       !g_gameplay_allowed.load()||!g_head_valid.load()||!g_left_valid.load()||!g_right_valid.load()||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_scripted.load()||g_ctx_reading.load()||
       kcd2vr::workstation178r20c1::ownsActivityWrist45(now)||
       animation_filter::nativeBodyOwned112.load()||!handler||!attacker||
       !kcd2vr::player_window_h26::fresh(g_head_valid_tick.load(),now,60)||
       !kcd2vr::player_window_h26::fresh(g_left_valid_tick.load(),now,80)||
       !kcd2vr::player_window_h26::fresh(g_right_valid_tick.load(),now,80))return false;
    std::uintptr_t vft=0;void* actor=nullptr;void* player=nullptr;void* manager=g_henry_item_attach_manager.load();
    if(!Read(handler,vft)||(vft!=base+0x3AECEA0&&vft!=base+0x3AECE20)||
       !Read(static_cast<unsigned char*>(handler)+8,actor)||!actor||!manager||
       !PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,player)||
       !NativeResolve104(player,owner)||owner.actor!=actor||owner.epoch!=g_pose_epoch6689.load()||!PlayerWindowMotionH26(owner,now,source))return false;
    void* opponent=nullptr;void* state=nullptr;void* back=nullptr;
    if(!Read(static_cast<unsigned char*>(owner.state)+0xCF8,opponent)||opponent!=attacker||opponent==actor||
       (!PhysVft6698(opponent,0x3E3FAC8)&&!PhysVft6698(opponent,0x3A80610))||
       !Read(static_cast<unsigned char*>(opponent)+0x210,state)||!state||
       !Read(static_cast<unsigned char*>(state)+0xCE0,back)||back!=opponent||
       !Read(static_cast<unsigned char*>(state)+0xCF8,back)||back!=actor)return false;
    kind=vft==base+0x3AECEA0?1:5;return true;
}
static float __fastcall PlayerWindowDetourH26(void* handler,void* attacker) {
    NativeOwner104 before{};std::uintptr_t beforeKind=0;PlayerWindowSourceH26 beforeSource{};
    const bool qualified=PlayerWindowOwnerH26(handler,attacker,before,beforeKind,beforeSource);
    const float original=g_player_window_originalH26(handler,attacker); // exactly once, exceptions propagate
    if(!qualified)return original;
    NativeOwner104 after{};std::uintptr_t afterKind=0;PlayerWindowSourceH26 afterSource{};
    const bool current=PlayerWindowOwnerH26(handler,attacker,after,afterKind,afterSource)&&beforeSource.same(afterSource)&&afterKind==beforeKind&&
        after.player==before.player&&after.actor==before.actor&&after.state==before.state&&after.epoch==before.epoch;
    const float adjusted=kcd2vr::player_window_h26::widen(original,current,(beforeKind==1?kcd2vr::player_window_h26::perfectBlockMultiplier:kcd2vr::player_window_h26::multiplier).load(std::memory_order_acquire),beforeKind==1);
    static std::atomic<unsigned> receipts{0};
    if(std::isfinite(original)&&adjusted>original&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64())&&
       receipts.fetch_add(1,std::memory_order_relaxed)<48){char line[280]{};
        std::snprintf(line,sizeof(line),"H26 PLAYER_WINDOW_ALLOWANCE kind=%s native=%.4f adjusted=%.4f scope=EXACT_HENRY nativeEligibilityAndImpactCap=PRESERVED windowOpened=UNCLAIMED",
            beforeKind==1?"PERFECT_BLOCK":"MASTER_STRIKE",double(original),double(adjusted));diagnostic_trace::Write(line);}
    return adjusted;
}
static void ShutdownPlayerWindowH26() noexcept {
    g_player_window_readyH26.store(false,std::memory_order_release);
    if(g_player_window_hookH26){g_player_window_retiredH26.store(true,std::memory_order_release);
        try{(void)g_player_window_hookH26->disable();}catch(...){} }
    // Retain executable handle/original even after uncertain disable.
}
static bool ArmPlayerWindowH26() noexcept {
    if(g_player_window_readyH26.load(std::memory_order_acquire))return true;
    const auto base=WhBase();if(g_player_window_retiredH26.load()||g_player_window_hookH26||!base||!retail_build112::Verify(base))return false;
    struct Pin{std::uintptr_t rva;const char* bytes;std::size_t n;};
    constexpr Pin pins[]{
        {0xB27958,"\x48\x8b\xc4\x48\x89\x58\x08\x48\x89\x70\x10\x57\x48\x83\xec\x40",16},
        {0xB27976,"\xe8\x9d\x00\x00\x00\x0f\x28\xf8",8},
        {0xB27997,"\x48\x8b\x98\x88\x03\x00\x00",7},
        {0xB27A26,"\xff\x50\x68\x0f\xb6\xc8\x83\xe9\x01",9},
        {0xB279D3,"\xe8\xbc\xdf\xea\xff",5},
        {0xB279F7,"\xf3\x0f\x58\xc6",4},
        {0xB27A83,"\xe8\x40\xdb\xde\xff\xe8\x37\xd3\x44\x00",10},
        {0xB27A6D,"\xe8\x56\xdb\xde\xff\xe8\x4d\xd3\x44\x00",10},
        {0xB27654,"\xe8\xff\x02\x00\x00",5},
        {0x9142B4,"\xe8\x9f\x36\x21\x00",5},
        {0x41A6A0,"\xb0\x01\xc3",3},{0x1A78ED0,"\xb0\x05\xc3",3}};
    for(const auto& p:pins){unsigned char actual[16]{};
        if(!Read(reinterpret_cast<void*>(base+p.rva),actual)||std::memcmp(actual,p.bytes,p.n))return false;}
    std::uintptr_t category=0;
    if(!Read(reinterpret_cast<void*>(base+0x3AECEA0+0x68),category)||category!=base+0x41A6A0||
       !Read(reinterpret_cast<void*>(base+0x3AECE20+0x68),category)||category!=base+0x1A78ED0)return false;
    HMODULE module=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&PlayerWindowDetourH26),&module)||!module)return false;
    try{using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
        auto h=inline_at(InlineRequest{"kcd2vr_player_window_h26",Target{DetourModKit::Address{base+0xB27958}},options},&PlayerWindowDetourH26);
        if(!h)return false;g_player_window_hookH26=std::move(*h);g_player_window_originalH26=g_player_window_hookH26->original<PlayerWindowFnH26>();
        if(!g_player_window_originalH26||!g_player_window_hookH26->enable()||!g_player_window_hookH26->is_enabled()){
            ShutdownPlayerWindowH26();return false;}
        g_player_window_readyH26.store(true,std::memory_order_release);return true;
    }catch(...){ShutdownPlayerWindowH26();return false;}
}
