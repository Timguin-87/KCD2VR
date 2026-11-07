void __fastcall ActorUpdateDetour(void* actor,float frameTime) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::ActorUpdate};
    if(g_update_original){kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::ActorUpdate};g_update_original(actor,frameTime);}
    // H20: HMD dodge needs native combat context in fists/default modes too.
    // Geometry fallback and recorder lifetimes must not own this read-only cache.
    if(Rd<std::uintptr_t>(actor,0,0)==g_base+kCombatPlayerVft)geom27::PublishHenryCombatContextH20(actor);
    if(geom27::victim5916c3::count.load(std::memory_order_relaxed))geom27::victim5916c3::Observe(actor,std::uint64_t(GetTickCount64()));
    if(geom27::Enabled()){geom27::ActorSeen27(actor);if(Rd<std::uintptr_t>(actor,0,0)==g_base+kCombatPlayerVft){geom27::ObserveHenry178r31(actor);geom27::Drain(actor);geom27::KickDrain37(actor);}}
    else{if(Rd<std::uintptr_t>(actor,0,0)==g_base+kCombatPlayerVft)services59c5::staminaScaling59c17.store(false,std::memory_order_relaxed); // R59.16C10R17: no fallback -> no discount
        if(geom27::TelegraphDirty178r34()&&Rd<std::uintptr_t>(actor,0,0)==g_base+kCombatPlayerVft)geom27::TelegraphRestore178r34(actor);
        if(geom27::vrstate596::g_stage.load()!=0&&Rd<std::uintptr_t>(actor,0,0)==g_base+kCombatPlayerVft){const void *he=nullptr,*hs=nullptr; // R59.8: cleanup outside the rung
            if(CombatActor(actor,he,hs,false))geom27::vrstate596::Tick(hs,std::uint64_t(GetTickCount64()));}} // V66.178R34 // V66.178R27 (game thread, after the native update)
}
