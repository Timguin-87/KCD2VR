// D65: opt-in clip acceleration for a still-current transported request that
// has no hand owner. Release/retirement stays authoritative; no Use resurrection.
static std::atomic<bool> g_unowned_pickup_clock_d65{false};
static kcd2vr::transfer_clip_d65::PickupJoin g_unowned_pickup_join_d65;
static void RetireUnownedPickupClockD65(std::uint64_t serial) noexcept {
    if(serial)g_unowned_pickup_join_d65.retire(serial); // protected by existing pickup clip transaction
}
static void TickUnownedPickupClipD65(void* ctrl,std::uint64_t now) noexcept {
    if(!g_unowned_pickup_clock_d65.load(std::memory_order_acquire)||!OnPickupOwnerD44()||
       !NativeGraspAvailable63())return;
    PickupClipTransactionD44 transaction;if(!transaction||g_pickup_clock_h23.active||g_owner.state!=Idle||
       g_pick13.attachStarted||!g_context13.load(std::memory_order_acquire)||kcd2vr::activity_input163::witness175.Active())return;
    Use13 use{};grasp13::AcquisitionInput55 input{};
    if(!g_use13.read(use)||use.serial==g_consumed_use13||!g_acquisition_input55.read(input)||!CurrentUse55(use,input,now))return;
    CurrentRoot57 current{};const auto& before=use.before55;
    if(!ReadCurrentRootIdentity57(ctrl,true,current,before.character))return;
    const auto actionId=reinterpret_cast<std::uintptr_t>(current.action),scopeId=reinterpret_cast<std::uintptr_t>(current.scope);
    const bool newRoot=reinterpret_cast<std::uintptr_t>(ctrl)==before.controller&&
        (actionId!=before.action||current.token!=before.nativeToken);
    if(!g_unowned_pickup_join_d65.accept({use.serial,use.generation55,actionId,scopeId,current.token},true,newRoot))return;
    void* skel=nullptr;std::uintptr_t character=0;auto* scope=current.scope;auto* action=current.action;
    if(!PickupLayerSourceH23(skel,character)||character!=before.character||Rd<int>(scope,kScopeLayer,-1)!=0||
       Rd<unsigned>(scope,kScopeNumLayers,0)!=1||NumAnims(skel,0)!=1)return;
    const float time=AnimTime(skel,0,0);
    if(Rd<float>(action,kActSpeedBias,-1.f)!=1.f||Rd<float>(scope,kScopeSpeedBias,-1.f)!=1.f||
       Rd<float>(skel,0x50,-1.f)!=1.f||!std::isfinite(time)||time<0.f||time>=1.f)return;
    CurrentRoot57 again{};grasp13::AcquisitionInput55 latest{};
    if(!ReadCurrentRootIdentity57(ctrl,true,again)||again.action!=action||again.scope!=scope||again.token!=current.token||
       !g_acquisition_input55.read(latest)||!CurrentUse55(use,latest,std::uint64_t(GetTickCount64()))||
       !g_unowned_pickup_clock_d65.load(std::memory_order_acquire))return;
    g_pickup_clock_h23={ctrl,skel,character,0,1.f,8.f,true,use.serial};
    g_pickup_clock_controller_d44.store(ctrl,std::memory_order_release);g_pickup_clock_live_h23.store(true,std::memory_order_release);
    if(!SetPickupLayerScaleH23(skel,0,8.f))(void)RestorePickupClipH23();
    // The existing F5 action/clip receipts observe this native clip. No guessed
    // “Grip opened” message: this path explicitly requires a live request.
}
