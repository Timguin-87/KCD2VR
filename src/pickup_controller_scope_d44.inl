// The engine may schedule Henry's verified Mannequin Update on a different
// worker each frame. A cached OS thread id is not the lifetime of that Update.
// This scope is created only inside its acquired owner transaction, and ends
// BEFORE the original Update (which may join native animation jobs). It grants
// pickup observation/clock/adoption only; other native services keep their
// existing OnOwnerThread13 restriction.
struct PickupControllerWitnessD44 {
    void* controller=nullptr;
    std::uint32_t actor=0;
};
static thread_local PickupControllerWitnessD44 g_pickup_controller_d44;
struct PickupControllerScopeD44 {
    PickupControllerWitnessD44 previous{};
    explicit PickupControllerScopeD44(void* ctrl) noexcept : previous(g_pickup_controller_d44) {
        g_pickup_controller_d44={};
        if(g_owner_transaction_depth19c4&&ctrl&&
           ctrl==g_henry_ctrl.load(std::memory_order_acquire)&&IsHenryController(ctrl)&&
           retail_build112::Ready(WhBase())){
            const auto actor=Rd<std::uint32_t>(ctrl,kCtrlEntityId,0);
            if(actor)g_pickup_controller_d44={ctrl,actor};
        }
    }
    ~PickupControllerScopeD44(){g_pickup_controller_d44=previous;}
    PickupControllerScopeD44(const PickupControllerScopeD44&)=delete;
    PickupControllerScopeD44& operator=(const PickupControllerScopeD44&)=delete;
};
static bool OnPickupControllerD44() noexcept {
    const auto& witness=g_pickup_controller_d44;
    return g_owner_transaction_depth19c4&&witness.controller&&witness.actor&&
        witness.controller==g_henry_ctrl.load(std::memory_order_acquire)&&
        IsHenryController(witness.controller)&&retail_build112::Ready(WhBase())&&
        Rd<std::uint32_t>(witness.controller,kCtrlEntityId,0)==witness.actor;
}
static bool OnPickupOwnerD44() noexcept {
    return OnOwnerThread13()||OnPickupControllerD44();
}
