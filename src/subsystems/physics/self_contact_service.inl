bool ReadSelfPat54(kcd2vr::self_contact54::Event& out,std::uint64_t now) noexcept {
    out={};if(!g_selfPat54.read(out)||!out.serial||!out.tick||now<out.tick||now-out.tick>120||
       out.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       out.request!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||
       out.character!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire)))return false;
    PhysEquipment6698 e{};
    if(!g_phys_equipment6698.read(e)||!e.valid||!e.tick||now<e.tick||now-e.tick>100||
       e.epoch!=out.epoch||e.request!=out.request||reinterpret_cast<std::uintptr_t>(e.player)!=out.player||
       out.hand<0||out.hand>1||out.weapon!=1-out.hand||!e.handKnown[out.hand]||
       !e.handKnown[out.weapon]||e.handKey[out.weapon]!=out.key)return false;
    if(out.source==kcd2vr::self_contact54::Source::EmptyHandSword){if(e.handKey[out.hand])return false;}
    else if(out.source==kcd2vr::self_contact54::Source::VisualShieldBounds){
        const int shield=e.heldIndex[out.hand];
        if(!out.shieldKey||!out.shieldGeneration||!out.shieldEntity||e.handKey[out.hand]!=out.shieldKey||
           shield<0||shield>=e.count||e.count>int(e.items.size())||!e.items[shield].held||
           !e.items[shield].profile.shield176()||e.items[shield].profile.key!=out.shieldKey||
           e.items[shield].profile.generation!=out.shieldGeneration||e.items[shield].profile.entity!=out.shieldEntity)return false;
    }else return false;
    const int held=e.heldIndex[out.weapon];
    return held>=0&&held<e.count&&e.count<=int(e.items.size())&&e.items[held].held&&
        e.items[held].profile.key==out.key&&e.items[held].profile.generation==out.generation&&e.items[held].profile.entity==out.entity;
}
void SetChestDagger54(bool enabled) noexcept {g_chest_dagger54.store(enabled,std::memory_order_release);}
bool ChestDagger54() noexcept {return g_chest_dagger54.load(std::memory_order_acquire);}
void* HenryItemAttachManager59c15() noexcept {return g_henry_item_attach_manager.load(std::memory_order_acquire);}
// R59.16C10R14 stowed bow hide (see physical_native.inl StowedBowHide59c14)
