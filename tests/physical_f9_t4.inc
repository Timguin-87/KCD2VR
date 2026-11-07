static std::atomic<int> g_physical_held_f9_t4{0};
static std::atomic<int> g_f9_saved_ragdoll64{-1};
static std::atomic<bool> g_f9_failed_request_h6{false};
static int NextPhysicalHeldF9H6() noexcept {return g_f9_failed_request_h6.load()?0:(g_physical_held_f9_t4.load()+1)%4;}
static bool RagdollPreference64() noexcept {const int saved=g_f9_saved_ragdoll64.load();return saved>=0?saved==1:g_ragdoll_wanted59.load();}
static std::atomic<bool> g_legacy_phys_suspended_t4{false};
static void ApplyLegacyPhysBodiesT4() noexcept {
    frik_pose_probe::SetPhysBodies59c10r8(g_legacy_phys_suspended_t4.load()?0:g_phys_bodies59c10r8.load(),
        g_phys_hand_mass59c10r8.load(),g_phys_blade_mass59c10r8.load());
}
static void ApplyPhysicalHeldF9T4(int rung) noexcept {
    rung=std::clamp(rung,0,3);
    // Installation is admission, not an ownership transfer. A refusal must not
    // turn off the previously selected hand/blade proxies or ragdoll preference.
    const bool installed=rung<2||native_weapon_body_t4::Install();
    if(!installed){
        g_f9_failed_request_h6.store(true,std::memory_order_release);
        diagnostic_trace::Write("B3 F9 requested=PHYSICAL_HELD status=NATIVE_UNAVAILABLE previousMode=RETAINED ownershipTransfer=NONE");
        return;
    }
    g_f9_failed_request_h6.store(false,std::memory_order_release);
    const int previous=g_physical_held_f9_t4.exchange(rung);
    // Save the user's preference once for the entire session ladder. Rung 1
    // requests bounded corpse push; 2/3 suspend it for the native weapon owner.
    // Visiting 2 -> 1 must not consume the saved preference before 1 -> 0.
    if(previous==0&&rung>0)g_f9_saved_ragdoll64.store(g_ragdoll_wanted59.load()?1:0);
    if(rung>=2){g_ragdoll_wanted59.store(false,std::memory_order_release);if(g_ragdoll_rung19c4r4.load()!=0)ApplyRagdollRung19c4r4(0);}
    else if(rung==1)g_ragdoll_wanted59.store(true,std::memory_order_release);
    else if(previous>0){const int saved=g_f9_saved_ragdoll64.exchange(-1);if(saved>=0)g_ragdoll_wanted59.store(saved==1,std::memory_order_release);}
    kcd2vr::lab58::rung.store(rung==3?3:0,std::memory_order_release);
    combat_diag_probe::SetContactImpulseDiagnostics19c4r4(rung==1);
    if(rung>=2){
        g_legacy_phys_suspended_t4.store(true);
        ApplyLegacyPhysBodiesT4();
        native_weapon_body_t4::SetEnabled(true);
        diagnostic_trace::Write(rung==3?
            "C61 F9 rung=3 PHYSICAL_HELD + CLAUDE_LAB gated_by_individual_INI feature_status=EXPERIMENTAL nativeRpgResult=UNCHANGED":
            "C61 F9 rung=2 PHYSICAL_HELD nativeRpgResult=UNCHANGED");
    }else{
        native_weapon_body_t4::SetEnabled(false);
        native_weapon_body_t4::Cancel();
        diagnostic_trace::Write(rung==1?"T4 F9 rung=1 mode=OBSERVE_AND_CORPSE_PUSH_REQUESTED":"T4 F9 rung=0 mode=OFF retirement=PENDING_ACK");
    }
}
static void ServiceRagdollPushH6(bool consoleReady) noexcept {
    if(!consoleReady||g_physical_held_f9_t4.load()>=2)return;
    const bool suspended=g_legacy_phys_suspended_t4.load();
    const bool wanted=g_ragdoll_wanted59.load();
    const bool run=wanted&&!suspended;
    if(run!=g_ragdoll59.On()){
        ApplyRagdoll59(run);
        combat_diag_probe::SetContactImpulseDiagnostics19c4r4(g_physical_held_f9_t4.load()==1);
        // Only retirement suspension preserves the request. An install refusal
        // must retain ApplyRagdoll59's OFF result, not retry every frame.
        if(suspended)g_ragdoll_wanted59.store(wanted,std::memory_order_release);
    }
}
static void ServicePhysicalHeldF9T4() noexcept {
    native_weapon_body_t4::Achieved a{};
    const bool receiptRead=native_weapon_body_t4::Read(a);
    if(!receiptRead){
        a.state=native_weapon_body_t4::State::Waiting;
        a.reason=native_weapon_body_t4::Reason::NoInput;
        a.safeNativeRestore=false;a.colliderRetired=false;a.poseValid=false;
    }
    if(g_physical_held_f9_t4.load()<2&&a.safeNativeRestore&&g_legacy_phys_suspended_t4.exchange(false)){
        ApplyLegacyPhysBodiesT4();
        diagnostic_trace::Write("T4 F9 retired=ACK legacyProxyPreference=RESTORED savedSettings=UNCHANGED");
    }
    // Only transitions are formatted on the mod thread, never native callbacks.
    static unsigned priorState=~0u,priorReason=~0u,priorSource=~0u;
    static int priorRung=-1;
    const auto source=frik_pose_probe::HeldPhysicsSourceReasonT4();
    const auto state=unsigned(a.state),reason=unsigned(a.reason);const int rung=g_physical_held_f9_t4.load();
    if(state!=priorState||reason!=priorReason||rung!=priorRung||unsigned(source)!=priorSource){
        priorState=state;priorReason=reason;priorRung=rung;priorSource=unsigned(source);
        const auto phase=native_physics_phase_t4::ReadReceipt();
        char m[768]{};std::snprintf(m,sizeof(m),
            "T4 F9 rung=%d bodyState=%u reason=%s source=%s bodyId=%d bodyGen=%llu key=%llu epoch=%llu sample=%llu sourceTick=%llu achieved=%d world=%llu legacyRestoreSafe=%d receiptRead=%d phaseSteps=%llu phaseRejected=%llu modMeleeContact=DISABLED_WHILE_OWNED nativeRpgResult=UNCHANGED",
            rung,state,native_weapon_body_t4::ReasonName(a.reason),frik_pose_probe::HeldPhysicsSourceNameT4(source),a.bodyId,
            static_cast<unsigned long long>(a.bodyGeneration),static_cast<unsigned long long>(a.owner.key),
            static_cast<unsigned long long>(a.owner.epoch),static_cast<unsigned long long>(a.sample),
            static_cast<unsigned long long>(a.sourceTick),a.poseValid?1:0,
            static_cast<unsigned long long>(a.worldGeneration),a.safeNativeRestore?1:0,receiptRead?1:0,
            static_cast<unsigned long long>(phase.steps),static_cast<unsigned long long>(phase.rejectedContexts));
        diagnostic_trace::Write(m);
    }
}
