// Generated verbatim current production intent-owner predicate.
unsigned GeometryIntentOwnerStateH30(std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept {
    PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    const auto now=std::uint64_t(GetTickCount64());
    const auto fail=[](int s)->unsigned{g_geometry_owner_stage49.store(s,std::memory_order_relaxed);return 0;};
    // F9 keeps its typed body/lease path. A raw tracked owner can never borrow
    // an unretired physical body or an unobserved admission generation.
    if(!NativeHeldContactsCurrentT4())return fail(10);
    if(!key||!generation||!ready||!g_phys_frame6698.read(f)||f.input.sample<ready)return fail(1);
    if(!kcd2vr::contact176::fresh(f.input.tick,now,80)||!TrackingHeadUsable111(now))return fail(2);
    if(epoch!=g_pose_epoch6689.load()||f.input.bodyEpoch!=epoch)return fail(3);
    if(request!=g_weapon_request_epoch6694.load()||f.request!=request)return fail(4);
    if(!f.input.allowed||!f.input.physicalCombat177||!g_gameplay_allowed.load())return fail(5);
    if(animation_filter::nativeBodyOwned112.load())return fail(6);
    if(g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_scripted.load()||g_ctx_reading.load())return fail(7);
    for(int side=0;side<2;++side)if(f.profiles[side].key==key&&f.profiles[side].generation==generation){
        // The H30 proof is emitted by the qualified once-per-sample recognizer,
        // independently of reach/collision presentation. No proof is inferred
        // from handlingOnly/active/owned, and checked-contact authority remains
        // the separate attack_authority55c1 predicate above.
        if(!TrackingHandUsable111(side,now)||!ph::command_authority_h30(f.input,f.output,f.profiles[side],side))return fail(9);
        // PhysPublish6698 publishes the new frame before completing its command
        // queue and ready-sample store. That same-owner interval is not owner
        // loss. It grants no input/contact authority: only an already-earned
        // native combo slot may wait on its original finite clock.
        const auto completed=g_phys_ready_sample6698.load(std::memory_order_acquire);
        if(completed!=ready&&completed!=f.input.sample)return fail(1);
        if(completed!=f.input.sample){g_geometry_owner_stage49.store(1,std::memory_order_relaxed);return 2;}
        g_geometry_owner_stage49.store(0,std::memory_order_relaxed);return 1;
    }
    return fail(8);
}
bool GeometryIntentOwnerH30(std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept {
    return GeometryIntentOwnerStateH30(key,generation,epoch,request)==1;
}
