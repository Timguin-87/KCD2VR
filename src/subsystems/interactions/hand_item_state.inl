bool ReadHolsterGripOwnerR15(int side,std::uint64_t now) noexcept {
    if(side<0||side>1||g_weapon_control_mode.load(std::memory_order_acquire)<4||
       !g_phys_armed6698.load(std::memory_order_acquire)||g_phys_fault6698.load(std::memory_order_acquire)||
       !g_gameplay_allowed.load(std::memory_order_acquire))return false;
    const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    PhysFrame6698 f{};PhysEquipment6698 e{};
    if(!ready||!g_phys_frame6698.read(f)||!g_phys_equipment6698.read(e)||
       ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       e.manager!=g_henry_item_attach_manager.load(std::memory_order_acquire))return false;
    return kcd2vr::physical_hand_owner55::holster(f,e,side,now,ready,
        g_weapon_request_epoch6694.load(std::memory_order_acquire),g_pose_epoch6689.load(std::memory_order_acquire),
        reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire)));
}
int ReadPhysicalHandHeld55(int side,std::uint64_t now) noexcept {
    if(side<0||side>1)return -1;
    if(HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),
        g_pose_epoch6689.load(),g_weapon_request_epoch6694.load()))return 1;
    if(g_weapon_control_mode.load(std::memory_order_acquire)<4)return 0;
    if(!g_phys_armed6698.load(std::memory_order_acquire)||g_phys_fault6698.load(std::memory_order_acquire)||
       !g_gameplay_allowed.load(std::memory_order_acquire))return -1;
    const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    PhysFrame6698 f{};PhysEquipment6698 e{};
    if(!g_phys_frame6698.read(f)||!g_phys_equipment6698.read(e)||
       ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       e.manager!=g_henry_item_attach_manager.load(std::memory_order_acquire))return -1;
    return kcd2vr::physical_hand_owner55::read(f,e,side,now,ready,
        g_weapon_request_epoch6694.load(std::memory_order_acquire),
        g_pose_epoch6689.load(std::memory_order_acquire),
        reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire)));
}
int ExactHandItemState(int side) noexcept {
    if(side<0||side>1)return -1;
    if(g_weapon_control_mode.load()>=4){
        // The native-frame equipment reader already validates the exact Henry
        // manager and both live hand keys. Reuse it; no extra native polling.
        // An empty hand need not have emitted an attach-null event after startup.
        PhysEquipment6698 e{};const auto now=std::uint64_t(GetTickCount64());
        if(!g_phys_equipment6698.read(e)||!e.valid||!e.handKnown[side]||!e.tick||now<e.tick||now-e.tick>150|| // V66.178R10: 80 -> 150 ms (one stalled frame kept flagging the hand UNKNOWN)
           e.request!=g_weapon_request_epoch6694.load()||e.epoch!=g_pose_epoch6689.load()||
           !e.manager||e.manager!=g_henry_item_attach_manager.load())return -1;
        return e.handKey[side]?1:0;
    }
    // F10 can run without the F12 equipment reader. Attach-null observations
    // are not authoritative after the separate native hand-slot clear route.
    // Read only the already-proven Henry slot chain; no gameplay getter/poll.
    const auto manager=g_henry_item_attach_manager.load(std::memory_order_acquire);
    std::uint64_t key=0;
    if(!retail_build112::Ready(WhBase())||!NativeHandKey6697(manager,side,key)||
       manager!=g_henry_item_attach_manager.load(std::memory_order_acquire))return -1;
    return PhysEmpty6698(key)?0:1;
}
