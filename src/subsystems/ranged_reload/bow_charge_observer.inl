static bool PhysChargeOwnedAction175(void* action,bool canRemember) noexcept {
    void* player=g_phys_charge_player175.load(std::memory_order_acquire);
    if(!player||!action||!PhysVft6698(action,0x47083C0))return false;
    if(g_phys_charge_action175.load(std::memory_order_acquire)==action)return true;
    // Same pinned owner-resolution leaf already used by ChargePrepare. It is
    // read-only; a different actor keeps its original native charge callback.
    __try {
        void* actor=reinterpret_cast<void*(__fastcall*)(void*)>(WhBase()+0x9115E0)(static_cast<unsigned char*>(action)+0x58);
        if(actor!=player||g_phys_charge_player175.load(std::memory_order_acquire)!=player)return false;
        if(canRemember)g_phys_charge_action175.store(action,std::memory_order_release);
        return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static std::uint32_t __fastcall PhysChargeDetour6698(void* action,float dt) noexcept {
    std::unique_lock<std::recursive_mutex> ownerLock(g_phys_native_mutex6698,std::try_to_lock);
    const bool physicalOwner=PhysChargeOwnedAction175(action,ownerLock.owns_lock());
    const bool manual=ownerLock.owns_lock()&&PhysChargePrepareLeaf6698(action,dt);
    if(physicalOwner&&!manual){
        // A lost frame/geometry/ACK path cannot quietly accumulate time. Keep
        // native publication/eligibility with zero charge increment and arrange
        // native uncharge on the frame owner. Never manufacture release-to-fire.
        if(ownerLock.owns_lock())g_phys_native6698.bowChargeObserved114=false;
        g_phys_abort6698.store(true,std::memory_order_release);
    }
    return g_phys_charge_original6698?g_phys_charge_original6698(action,(manual||physicalOwner)?0.f:dt):0;
}
