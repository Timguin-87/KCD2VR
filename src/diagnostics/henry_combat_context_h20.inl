// H20: gameplay combat-context cache, independent of geometry-hit/recorder modes.
// Called only for the exact C_CombatPlayer from its original-first Update.
// H24 also copies the exact reciprocal opponent attack as a bounded read-only
// dodge prerequisite. No actor census, attributes, telegraph, physics or writes.
void PublishHenryCombatContextH20(const void* actor) noexcept {
    const void *entity=nullptr,*hs=nullptr;
    if(!CombatActor(actor,entity,hs,false)||!hs||!IsPlayer(entity)){
        g_henry_in_combat178r31.store(-1,std::memory_order_relaxed);
        g_henry_seen178r31.store(0,std::memory_order_release);
        (void)kcd2vr::hmd_dodge_h24::published.publish({});
        (void)kcd2vr::hmd_dodge_h24::blockPublished.publish({});return;
    }
    const auto now=std::uint64_t(GetTickCount64());
    const auto epochH24=frik_pose_probe::PoseEpochH24();
    const auto blockH24=kcd2vr::hmd_dodge_h24::observeBlock(g_base,reinterpret_cast<std::uintptr_t>(actor),
        reinterpret_cast<std::uintptr_t>(entity),reinterpret_cast<std::uintptr_t>(hs),epochH24,now,
        [](std::uintptr_t address,auto& value){return SehCopy146(reinterpret_cast<const void*>(address),&value,sizeof(value));});
    (void)kcd2vr::hmd_dodge_h24::blockPublished.publish(
        epochH24==frik_pose_probe::PoseEpochH24()&&entity==frik_pose_probe::LastPlayerEntity()?blockH24:kcd2vr::hmd_dodge_h24::BlockContext{});
    const auto dodgeH24=kcd2vr::hmd_dodge_h24::observe(g_base,reinterpret_cast<std::uintptr_t>(actor),
        reinterpret_cast<std::uintptr_t>(entity),reinterpret_cast<std::uintptr_t>(hs),epochH24,now,
        [](std::uintptr_t address,auto& value){return SehCopy146(reinterpret_cast<const void*>(address),&value,sizeof(value));});
    (void)kcd2vr::hmd_dodge_h24::published.publish(
        epochH24==frik_pose_probe::PoseEpochH24()&&entity==frik_pose_probe::LastPlayerEntity()?dodgeH24:kcd2vr::hmd_dodge_h24::Context{});
    {   // R133: FREE PLUS holds the view only while Henry has a combat opponent (S_CombatActorState+0xCF8, the
        // same field H24/D74 read). one bounded copy on this original-first update, no other native access.
        std::uintptr_t opponentR133=0;
        if(SehCopy146(static_cast<const unsigned char*>(hs)+0xCF8,&opponentR133,sizeof(opponentR133))&&opponentR133&&
           opponentR133!=reinterpret_cast<std::uintptr_t>(actor)&&entity==frik_pose_probe::LastPlayerEntity())
            kcd2vr::free_targeting_r133::opponentMs.store(now,std::memory_order_release);
    }
    const int b=B(hs,kInCombat),v=b==0xFF?-1:(b?1:0);
    const int prev=g_henry_in_combat178r31.exchange(v,std::memory_order_relaxed);
    g_henry_seen178r31.store(now,std::memory_order_release);
    if(prev!=v&&Allow(4,30)){g_henry_in_combat_edges178r33.fetch_add(1,std::memory_order_relaxed);char m[200]{};
        std::snprintf(m,sizeof(m),"V66.178R33 HENRY_COMBAT_EDGE inCombat=%d (was %d) actor=%p state=%p edges=%llu",v,prev,actor,hs,(unsigned long long)g_henry_in_combat_edges178r33.load());diagnostic_trace::Write(m);}
}
