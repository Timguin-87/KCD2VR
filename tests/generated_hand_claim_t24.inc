static bool HandClaimBlockedT24(int side,std::uintptr_t character,std::uint64_t body,std::uint64_t request) noexcept {
    // Normal OFF/free path uses only the existing copied claim publication.
    // Busy/partial/unknown observations still require the qualified owner join.
    if(kcd2vr::hand_claim_t24::observe(side,{}).state==kcd2vr::hand_claim_t24::State::Free)return false;
    kcd2vr::hand_claim_t24::Owner owner{};owner.character=character;owner.body=body;owner.request=request;
    void* manager=g_henry_item_attach_manager.load(std::memory_order_acquire);void* player=nullptr;void* back=nullptr;
    if(PhysVft6698(manager,0x3EC9958)&&Read(static_cast<unsigned char*>(manager)+0x38,player)&&PhysVft6698(player,0x3AA4B98)&&Read(static_cast<unsigned char*>(player)+0x9D0,back)&&back==manager&&manager==g_henry_item_attach_manager.load(std::memory_order_acquire)){
        owner.player=reinterpret_cast<std::uintptr_t>(player);owner.manager=reinterpret_cast<std::uintptr_t>(manager);
    }
    return kcd2vr::hand_claim_t24::competing(side,owner);
}
