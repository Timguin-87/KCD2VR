// Included after the private physical equipment implementation, in the enclosing namespace.
// All bodies exist only for this synchronous Henry pose/query pass. No physics
// body is created, forced into ragdoll mode, pushed, or retained for grabbing.
static bool NpcBodyRead172(void* entity,void*& body) noexcept {
    std::uintptr_t address=0;
    const bool ok=npc172::bodyFromEntity(WhBase(),reinterpret_cast<std::uintptr_t>(entity),
        [](std::uintptr_t p,auto& value) noexcept {return Read(reinterpret_cast<void*>(p),value);},address);
    body=reinterpret_cast<void*>(address);return ok;
}
// D53: dialogue/activity presentation has no gameplay equipment pump. Use
// the current coherent-pose observation, not an expired gameplay mailbox.
// Every native hand key AND entity binding is rejoined before/after building
// the pass-local exclusions. This grants no equipment or interaction writes.
static bool ContactExclusionsD53(npc172::Exclusions& skip,void* henry) noexcept {
    namespace co=kcd2vr::contact_occupancy_d52;
    const auto* observed=co::queryObservationD53;if(!observed||!observed->owner)return false;
    const auto& o=*observed;
    const auto rejoin=[&]() noexcept {
        if(!RejoinContactOccupancyD53(o))return false;
        void* entity=nullptr;
        return Read(reinterpret_cast<void*>(o.owner.player+0x38),entity)&&entity==henry;
    };
    if(!rejoin())return false;
    std::uint32_t weaponId[2]{},propId[2]{};
    const auto add=[&](std::uint32_t id) noexcept {
        void* entity=id?EntityById6694(id):nullptr;void* body=nullptr;
        return entity&&entity!=henry&&PhysEntityIdentity135(entity,id)&&NpcBodyRead172(entity,body)&&skip.add(entity,body);
    };
    for(int side=0;side<2;++side){const auto& h=o.hand[side];
        if(h.key&&(!NativeHeldItemId6694(side,weaponId[side])||!add(weaponId[side])))return false;
        if(h.object&&(!PhysVft6698(reinterpret_cast<void*>(h.object),0x3A68F68)||
            !Read(reinterpret_cast<void*>(h.object+8),propId[side])||!add(propId[side])))return false;
    }
    if(!rejoin())return false;
    for(int side=0;side<2;++side){std::uint32_t id=0;
        if(weaponId[side]&&(!NativeHeldItemId6694(side,id)||id!=weaponId[side]))return false;
        if(propId[side]&&(!Read(reinterpret_cast<void*>(o.hand[side].object+8),id)||id!=propId[side]))return false;
    }
    return true;
}
static bool PrepareNpcQuery172() noexcept {
    auto& q=g_npc_query172;
    if(q.prepared)return q.ready;
    q.prepared=true;
    q.gate=npc172::Gate::Contract;
    if(!q.requested||!NpcContract172(WhBase()))return false;
    void* henry=g_last_player_entity.load();void* body=nullptr;
    std::uint32_t id=0;
    q.gate=npc172::Gate::HenryIdentity;
    if(!PhysVft6698(henry,0x4072C78)||!Read(static_cast<unsigned char*>(henry)+0xC,id)||!id)return false;
    q.gate=npc172::Gate::HenryBody;
    if(!NpcBodyRead172(henry,body)||!body)return false;
    std::uintptr_t vf=0;
    if(!Read(body,vf)||(vf!=WhBase()+0x3a82570&&vf!=WhBase()+0x3a48b30))return false;
    if(!q.skip.add(henry,body))return false;
    // C42: this context is shared by BOTH hands/weapons. Never exclude an
    // impaled victim here; that also erased the other hand's contact/push.
    // A future blade-only exclusion needs a per-query source identity.
    PhysEquipment6698 e{};const auto now=std::uint64_t(GetTickCount64());
    if(kcd2vr::contact_occupancy_d52::queryObservationD53&&kcd2vr::contact_occupancy_d52::queryObservationD53->owner){
        q.gate=npc172::Gate::EquipmentIdentity;
        if(!ContactExclusionsD53(q.skip,henry))return false;
    }else if(g_phys_equipment6698.read(e)&&e.valid&&e.epoch==g_pose_epoch6689.load()&&
       e.request==g_weapon_request_epoch6694.load()&&now>=e.tick&&now-e.tick<=100&&
       e.count>=0&&e.count<=int(e.items.size())){
        q.gate=npc172::Gate::EquipmentIdentity;void* entity=nullptr;
        if(!e.player||!Read(static_cast<unsigned char*>(e.player)+0x38,entity)||entity!=henry)return false;
        for(int i=0;i<e.count;++i){const auto& item=e.items[i];
            if(!item.entity)continue;
            if(!PhysEntityIdentity135(item.entity,item.profile.entity)||!NpcBodyRead172(item.entity,body)||
               !q.skip.add(item.entity,body))return false;
        }
    }else{
        q.gate=npc172::Gate::Equipment;
        // Empty-hand mode may have no physical weapon service. Unknown hand
        // occupancy cannot enable a query that might collide with our own item.
        if(ExactHandItemState(0)!=0||ExactHandItemState(1)!=0)return false;
    }
    // R103: the small prop each hand holds (static_grab_r89) is part of that hand, not world to push it off.
    // Added last and optional: a full list keeps the old query (no refusal).
    for(int heldSide=0;heldSide<2;++heldSide){
        void* heldEntity=kcd2vr::static_grab_r89::heldEntityPtrR103[heldSide].load(std::memory_order_acquire);
        void* heldBody=kcd2vr::static_grab_r89::heldBodyR103[heldSide].load(std::memory_order_acquire);
        if(heldEntity&&heldBody&&kcd2vr::static_grab_r89::heldEntityR94[heldSide].load(std::memory_order_acquire))(void)q.skip.add(heldEntity,heldBody);
    }
    // R128 (Miki 6 Nov: blade "not deep enough"): the dead body hanging on our blade (VrImpaleFullDepthR128 ON) is
    // not a wall for our own blade queries, else the blade is pushed back out of him. Only a corpse hang (never the
    // living STUCK victim - C42), published within 200 ms, the same entity id still alive and its current physics
    // body is the hung one. Optional like the props: a full list keeps the old query.
    {ImpaleSkip40 hung{};
     if(kcd2vr::gore32::impaleFullDepthR128.load(std::memory_order_relaxed)&&g_impale_skip40.read(hung)&&hung.hangR128&&
        hung.entity&&hung.body&&hung.target&&now>=hung.tick&&now-hung.tick<=200){
      void* hungEntity=EntityById6694(hung.target);void* hungBody=nullptr;
      if(hungEntity&&hungEntity!=henry&&reinterpret_cast<std::uintptr_t>(hungEntity)==hung.entity&&
         NpcBodyRead172(hungEntity,hungBody)&&reinterpret_cast<std::uintptr_t>(hungBody)==hung.body)(void)q.skip.add(hungEntity,hungBody);}}
    q.gate=npc172::Gate::HenryIdentity;
    if(henry!=g_last_player_entity.load())return false;
    q.gate=npc172::Gate::Ready;q.ready=true;return true;
}
