// Runs in the already-admitted coherent pose callback only. In particular this
// does not refresh the gameplay equipment mailbox or service its commands.
static kcd2vr::contact_occupancy_d52::Owner ContactOwnerD52(void* character,
    std::uint64_t epoch,std::uint64_t tracking,std::uint64_t policy,std::uint64_t now) noexcept {
    namespace co=kcd2vr::contact_occupancy_d52;co::Owner o{};
    // R95 reloads keep ordinary weapon commands off. Their free working
    // hand still needs live native occupancy/exclusions after the gameplay
    // equipment mailbox ages out; this read-only owner borrows no commands.
    if(g_gameplay_allowed.load(std::memory_order_acquire)&&xcXR34::activeNativeLease.load(std::memory_order_acquire)&&
       character&&epoch&&tracking&&policy&&!(policy&1)&&policy==g_pose_policy_generation57.load()){
        xcXR34::Passenger reload{};
        if(ReadReloadPassengerXR34(reload,reinterpret_cast<std::uintptr_t>(character),epoch,now)&&
           xcXR34::handDrivenD81(reload.owner.source)&&reload.frame.tracking==tracking){
            const auto& source=reload.frame;
            o.character=source.character;o.manager=source.manager;o.player=source.player;
            o.scene=reload.owner.child;o.body=epoch;o.request=source.request;o.tracking=tracking;o.policy=policy;
            o.kind=4;o.lifetime=reload.owner.serial;return o;
        }
    }
    if(g_gameplay_allowed.load(std::memory_order_acquire)||!character||!epoch||!tracking||!policy||(policy&1)||
       !g_dialogue_body178r9.load(std::memory_order_acquire)||g_pose_restore_fault6689.load()||
       diagnostics::lab::NativeReference()||!TrackingHeadUsable111(now)||
       epoch!=g_pose_epoch6689.load()||tracking!=kcd2vr::tracking59c6::epoch.load()||
       policy!=g_pose_policy_generation57.load()||character!=g_committed_character6689.load()||
       !retail_build112::Ready(WhBase()))return o;
    // Real UI/cutscene excludes; the dialogue/native-context bits themselves do
    // not. Those same bits intentionally disable ordinary gameplay input.
    const auto context=g_tool_context19c4r2.load(std::memory_order_acquire);
    const auto tick=context>>8;const unsigned flags=unsigned(context&255);
    if(!tick||now<tick||now-tick>80||(flags&4)||(flags&16)||((flags&1)&&!(flags&8)))return o;
    o.character=reinterpret_cast<std::uintptr_t>(character);o.body=epoch;o.tracking=tracking;o.policy=policy;
    o.manager=reinterpret_cast<std::uintptr_t>(g_henry_item_attach_manager.load(std::memory_order_acquire));
    o.request=g_weapon_request_epoch6694.load(std::memory_order_acquire);
    if(!o.manager||!Read(reinterpret_cast<void*>(o.manager+0x38),o.player))return {};
    if(g_ctx_dialogue.load(std::memory_order_acquire)){
        kcd2vr::physics::dialogue_hand_h24::Witness w{};
        if(!g_dialogue_hand_presence_enabled_h24.load()||!g_dialogue_hand_presence_h24.read(w)||
           !w.presentation||w.policy!=policy||!kcd2vr::dialogue_state59c10r2::embodiedOwner(w.native,now)||
           w.native.player!=o.player||w.native.entity!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load()))return {};
        o.kind=1;o.scene=w.native.instance;
    }else{
        using namespace kcd2vr::activity_input163;
        if(WorkstationToolReason19c4r2(now)||!witness175.ready.load()||witness175.transitionDepth175.load())return {};
        const auto alchemy=witness175.active[unsigned(Kind175::Alchemy)].load();
        const auto smith=witness175.active[unsigned(Kind175::Smithing)].load();
        if(bool(alchemy)==bool(smith))return {};
        o.kind=alchemy?2:3;o.scene=alchemy?alchemy:smith;o.lifetime=witness175.lifetime175.load();
        o.mode=kcd2vr::minigame_hands45::modeEpoch67.load();
    }
    return o;
}
static kcd2vr::contact_occupancy_d52::Observation ReadContactOccupancyD52(void* character,
    std::uint64_t epoch,std::uint64_t tracking,std::uint64_t policy,std::uint64_t now) noexcept {
    namespace co=kcd2vr::contact_occupancy_d52;co::Observation out{};
    const auto& activity=kcd2vr::activity_input163::witness175;
    const bool activityScene=activity.active[unsigned(kcd2vr::activity_input163::Kind175::Alchemy)].load()||
        activity.active[unsigned(kcd2vr::activity_input163::Kind175::Smithing)].load();
    const auto owner=ContactOwnerD52(character,epoch,tracking,policy,now);
    if(!owner&&g_gameplay_allowed.load(std::memory_order_acquire)&&
       xcXR34::activeNativeLease.load(std::memory_order_acquire))return out; // reload loss is UNKNOWN, never old gameplay equipment
    if(!owner&&(g_gameplay_allowed.load(std::memory_order_acquire)||(!g_ctx_dialogue.load()&&!activityScene))){
        for(int side=0;side<2;++side){out.hand[side].state=kcd2vr::frik_pose_probe::ExactHandItemState(side);out.hand[side].reason=co::Reason::Legacy;}
        return out;
    }
    if(!owner)return out;
    const auto read=[](std::uintptr_t p,auto& v){return Read(reinterpret_cast<const void*>(p),v);};
    const auto keys=[&](int side,std::uint64_t& key){if(!NativeHandKey6697(reinterpret_cast<void*>(owner.manager),side,key))return false;
        if(PhysEmpty6698(key))key=0;return true;};
    // Reuse the exact existing qualified name-lookup leaf. A missing/failed
    // lookup stays UNKNOWN; never interpret its nullptr as an empty binding.
    const auto find=[&](const char* name){return reinterpret_cast<std::uintptr_t>(CharAttachment59c15(character,name));};
    const auto current=[&](const co::Owner& o){return co::same(o,ContactOwnerD52(character,epoch,tracking,policy,std::uint64_t(GetTickCount64())));};
    return co::observe(WhBase(),owner,read,keys,find,current);
}
static bool RejoinContactOccupancyD53(const kcd2vr::contact_occupancy_d52::Observation& o) noexcept {
    namespace co=kcd2vr::contact_occupancy_d52;if(!o.owner)return false;
    const auto read=[](std::uintptr_t p,auto& v){return Read(reinterpret_cast<const void*>(p),v);};
    const auto keys=[&](int side,std::uint64_t& key){if(!NativeHandKey6697(reinterpret_cast<void*>(o.owner.manager),side,key))return false;
        if(PhysEmpty6698(key))key=0;return true;};
    // Revalidate the already observed exact slots; no additional native name
    // lookups inside the shared ray budget. observe checks class/name/backlink.
    const auto find=[&](const char* name){return o.hand[std::strcmp(name,"entity_right")==0?0:1].attachment;};
    const auto current=[&](const co::Owner& owner){return co::same(owner,ContactOwnerD52(
        reinterpret_cast<void*>(owner.character),owner.body,owner.tracking,owner.policy,std::uint64_t(GetTickCount64())));};
    const auto joined=co::observe(WhBase(),o.owner,read,keys,find,current);
    if(!co::same(o.owner,joined.owner))return false;
    for(int side=0;side<2;++side){const auto& a=o.hand[side];const auto& b=joined.hand[side];
        if(a.state<0||b.state!=a.state||a.key!=b.key||a.attachment!=b.attachment||a.object!=b.object)return false;}
    return true;
}
static void TraceContactOccupancyD52(const kcd2vr::contact_occupancy_d52::Observation& o,std::uint64_t now) noexcept {
    namespace cap=diagnostics::capture168;
    if(!cap::active(cap::Domain::Interaction,now)&&!cap::active(cap::Domain::Bug,now))return;
    const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load()^
        (cap::gate.session[unsigned(cap::Domain::Bug)].load()<<1);
    static std::uint64_t lastSession=0,lastKey[2]{};static std::uintptr_t lastObject[2]{};
    static unsigned count=0,last[2]{~0u,~0u};static kcd2vr::contact_occupancy_d52::Owner previous{};
    if(lastSession!=session){lastSession=session;count=0;last[0]=last[1]=~0u;previous={};lastKey[0]=lastKey[1]=0;lastObject[0]=lastObject[1]=0;}
    const unsigned state[2]{unsigned(o.hand[0].reason),unsigned(o.hand[1].reason)};
    if(count>=96||(last[0]==state[0]&&last[1]==state[1]&&kcd2vr::contact_occupancy_d52::same(previous,o.owner)&&
        lastKey[0]==o.hand[0].key&&lastKey[1]==o.hand[1].key&&lastObject[0]==o.hand[0].object&&lastObject[1]==o.hand[1].object))return;
    last[0]=state[0];last[1]=state[1];previous=o.owner;++count;
    for(int side=0;side<2;++side){lastKey[side]=o.hand[side].key;lastObject[side]=o.hand[side].object;}
    char line[560]{};std::snprintf(line,sizeof(line),
        "D52 CONTACT_OCCUPANCY session=%llu sample=%u/96 kind=%u scene=%llX body=%llu request=%llu R=%d/%s L=%d/%s key=%llX/%llX binding=%llX/%llX source=CONTACT_ONLY weaponCommands=UNCHANGED budgetExhausted=%d",
        (unsigned long long)session,count,o.owner.kind,(unsigned long long)o.owner.scene,(unsigned long long)o.owner.body,(unsigned long long)o.owner.request,
        o.hand[0].state,kcd2vr::contact_occupancy_d52::name(o.hand[0].reason),o.hand[1].state,kcd2vr::contact_occupancy_d52::name(o.hand[1].reason),
        (unsigned long long)o.hand[0].key,(unsigned long long)o.hand[1].key,(unsigned long long)o.hand[0].object,(unsigned long long)o.hand[1].object,count==96);
    diagnostic_trace::Write(line);
}
