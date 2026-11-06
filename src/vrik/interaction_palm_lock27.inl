// Actual Mod palm-lock producer, shared with the connected host fixture.
if(alchemyPointer59c2){
    const auto& witness27=activity_input163::witness175;
    const auto life27=witness27.lifetime175.load(std::memory_order_acquire);
    const auto instance27=witness27.active[unsigned(activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire);
    const auto epoch27=workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire);
    const bool allowed27=(f5Mode==0||f5Mode==2||(f5Mode==3&&guided_crafting55::active(std::uint64_t(GetTickCount64()))))&&witness27.ready.load(std::memory_order_acquire)&&!witness27.transitionDepth175.load(std::memory_order_acquire)&&
        instance27; // C31: normal0 and experiment2 retain C29 selected-target locking.
        // Diagnostic/reference1 must not retain a live override. Epoch still resets on F5 toggles.
    alchemy_input19c4::Selection selected27{};
    // C37I1: native frame completion can publish after this Mod loop began.
    // Judge the copied selection at acquisition time; the old loop-start clock
    // could falsely label a fresh target "future" and consume the Grip edge.
    if(allowed27)(void)alchemy_input19c4::selectionSnapshot(instance27,life27,std::uint64_t(GetTickCount64()),selected27);
    const bool stable27=allowed27&&witness27.ready.load(std::memory_order_acquire)&&
        !witness27.transitionDepth175.load(std::memory_order_acquire)&&
        witness27.lifetime175.load(std::memory_order_acquire)==life27&&
        witness27.active[unsigned(activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire)==instance27&&
        workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire)==epoch27;
    palms59c9.lockedEntity=alchemyPalmLock27.update(stable27,instance27,life27,epoch27,palms59c9.locked,selected27);
}else{
    (void)alchemyPalmLock27.update(false,0,0,0,-1,{});
    palms59c9.lockedEntity=0; // hand only; successful transport lock is applied by interaction_ordinary_lock64
}
