void SetMiscInteractionDiagMode(int mode) noexcept {
    // Compatibility adapter for explicit hand-interaction policy; F5 records only.
    if(mode==3&&!retail_build112::Verify(WhBase()))
        diagnostic_trace::Write("V66.114 F5=3 retail SHA unavailable: held contextual hand use disabled");
    SetDiagnosticLab6690(diagnostics::lab::Key::F5,mode);
    const int current=MiscInteractionDiagMode();
    if(current==1){
        const bool inputReady=retail_build112::Verify(WhBase())&&ArmCombatInput101(false);
        diagnostic_trace::Write(ArmInteractionBody122()?"V66.122 F5_BODY_OBSERVER ready targetOnly=1 durationAfterRequestMs=3000 mutation=NONE":
            "V66.122 F5_BODY_OBSERVER unavailable noConstraintCalls=1");
        if(!animation_filter::Ready())animation_filter::Install();
        char status[256]{};std::snprintf(status,sizeof(status),
            "V66.113 REFERENCE_ARM source=F5 input=%d durationMs=180000 contextualActions=OBSERVE_ONLY mutation=NONE",inputReady?1:0);
        diagnostic_trace::Write(status);
    }
    cc::selectSources(cc::sources(HolsterCombatDiagMode(),current,g_census_mode.load()),GetTickCount64(),current==1);
}
void CycleMiscInteractionDiagMode() noexcept {SetMiscInteractionDiagMode((MiscInteractionDiagMode()+1)%4);}
int MiscInteractionDiagMode() noexcept {return int(diagnostics::lab::Get(diagnostics::lab::Key::F5));}
void MiscInteractionDiagTick() noexcept {
    WeaponControlTick(); // bounded alias hashing and reference file I/O on the existing worker
    // Worker performs no game reads/calls. Only resume the mod's own pose mode.

    static std::uint64_t last=0;const auto now=static_cast<std::uint64_t>(GetTickCount64());
    diagnostics::interaction_d42::capture.tick(now,[]() noexcept {
        diagnostics::interaction_d42::Snapshot s{};
        s.character=g_player_char37.load(std::memory_order_acquire);
        s.epoch=PoseEpochH24();
        s.actionRead=native_passenger_a34::witness.read(s.action);
        s.propEnabled=physics::grab_producer60::Enabled();
        for(int h=0;h<2;++h)s.handRead[h]=physics::grab_producer60::ReadStatus(h,s.hand[h]);
        return s;
    },diagnostic_trace::Write);
    static std::uint64_t lastAuthority157=0;
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)&&
       (g_weapon_control_mode.load(std::memory_order_relaxed)>=4||g_census_mode.load(std::memory_order_relaxed)>0)&&
       (now<lastAuthority157||now-lastAuthority157>=2000)){
        lastAuthority157=now;
        char authority[1024]{};std::snprintf(authority,sizeof(authority),
            "V66.157 AUTHORITY weaponMode=%d vrikMode=%d weapon{grace=%llu expired=%llu nativeRestores=%llu waitingNative=%llu drop[1..10]=%llu/%llu/%llu/%llu/%llu/%llu/%llu/%llu/%llu/%llu} poseFallback{rig=%llu raw=%llu noSnapshot=%llu age=%llu nativeBody=%llu transaction=%llu} rebaseR98=%llu policy=ONE_WRITER_NO_CONTROLLER_BRIDGE",
            g_weapon_control_mode.load(std::memory_order_relaxed),g_census_mode.load(std::memory_order_relaxed),
            (unsigned long long)g_phys_grace135.load(),(unsigned long long)g_phys_grace_expired135.load(),
            (unsigned long long)g_phys_native_restore157.load(),(unsigned long long)g_phys_waiting_native157.load(),
            (unsigned long long)g_phys_authority_drop157[1].load(),(unsigned long long)g_phys_authority_drop157[2].load(),
            (unsigned long long)g_phys_authority_drop157[3].load(),(unsigned long long)g_phys_authority_drop157[4].load(),
            (unsigned long long)g_phys_authority_drop157[5].load(),(unsigned long long)g_phys_authority_drop157[6].load(),
            (unsigned long long)g_phys_authority_drop157[7].load(),(unsigned long long)g_phys_authority_drop157[8].load(),
            (unsigned long long)g_phys_authority_drop157[9].load(),(unsigned long long)g_phys_authority_drop157[10].load(),
            (unsigned long long)g_pose_authority_drop157[0].load(),(unsigned long long)g_pose_authority_drop157[1].load(),
            (unsigned long long)g_pose_authority_drop157[2].load(),(unsigned long long)g_pose_authority_drop157[3].load(),
            (unsigned long long)g_pose_authority_drop157[4].load(),(unsigned long long)g_pose_authority_drop157[5].load(),
            (unsigned long long)g_phys_rebase_r98.load());
        diagnostic_trace::Write(authority);
    }
    if(!cc::active(now)||now-last<2000)return;last=now;
    if(const int f11=FocusedTestMode167()){
        char status[256]{};std::snprintf(status,sizeof(status),
            "V66.167 F11_STATUS rung=%d session=%llu detailedCapture=%d dropped=%llu nativeOutcome=OBSERVE_ONLY handContact=BOUNDED_BODY_WORLD_SELF_QUERIES",
            f11,(unsigned long long)cc::session.load(),F11Capture167(f11),(unsigned long long)cc::dropped.load());
        diagnostic_trace::Write(status);
    }
    if(HolsterCombatDiagMode()==1||HolsterCombatDiagMode()==3){
        char material[320]{};std::snprintf(material,sizeof(material),
            "V66.116 MATERIAL_STATUS contract=%d qualifiedDispatches=%llu rateLimited=%llu queueDropped=%llu rateCap=50_DISPATCH_PER_SECOND writes=NONE",
            g_material_contract116.load()==WhBase()&&retail_build112::Ready(WhBase()),
            (unsigned long long)g_material_serial116.load(),(unsigned long long)g_material_limited116.load(),
            (unsigned long long)cc::dropped.load());
        diagnostic_trace::Write(material);
    }
    if(HolsterCombatDiagMode()>=1){
        char weapon135[512]{};std::snprintf(weapon135,sizeof(weapon135),
            "V66.135-F3 WEAPON_STATUS mode=%d nativeSweepCalls=%llu localSweeps=%llu nativeSweepHits=%llu physicalGrace=%llu graceExpired=%llu verifyDeferred=%llu diagnosticsDomain=WEAPON_COMBAT_COLLISION",
            HolsterCombatDiagMode(),(unsigned long long)g_phys_native_sweep_calls110.load(),(unsigned long long)g_phys_native_sweep_local110.load(),
            (unsigned long long)g_phys_native_sweep_hits110.load(),(unsigned long long)g_phys_grace135.load(),
            (unsigned long long)g_phys_grace_expired135.load(),(unsigned long long)g_phys_verify_deferred135.load());diagnostic_trace::Write(weapon135);
    }
    if(MiscInteractionDiagMode()>=1){
        char interact135[448]{};std::snprintf(interact135,sizeof(interact135),
            "V66.135-F5 INTERACTION_STATUS mode=%d physicsGetterContract=%d watchedBodyReturns=%llu nonnullBodies=%llu backend=%s scope=LEGACY_BODY_GETTER_ONLY nativeGrabStatus=D42_F5_PROP_OWNER",
            MiscInteractionDiagMode(),g_interaction_body_contract122.load()==WhBase()&&retail_build112::Ready(WhBase()),
            (unsigned long long)g_interaction_body_seen135.load(),(unsigned long long)g_interaction_body_nonnull135.load(),
            MiscInteractionDiagMode()==3?"HELD_NATIVE_CONTEXT":MiscInteractionDiagMode()==2?"HAND_RAY_NATIVE_USE":"TARGET_BODY_REFERENCE");diagnostic_trace::Write(interact135);
    }
    if(HolsterCombatDiagMode()==3){
        char motion[640]{};std::snprintf(motion,sizeof(motion),
            "V66.112-MOTION STATUS contract=%d alignmentSkips=%llu trajectoryWrites=%llu rejects(caller/context/producer/coherence/socket/pivot/discontinuity/warmup)=%llu/%llu/%llu/%llu/%llu/%llu/%llu/%llu acceptedDamage=UNCLAIMED",
            g_motion_contract112.load()==WhBase()&&retail_build112::Ready(WhBase()),
            (unsigned long long)g_alignment_skips112.load(),(unsigned long long)g_trajectory_writes112.load(),
            (unsigned long long)g_trajectory_reject112[0].load(),(unsigned long long)g_trajectory_reject112[1].load(),
            (unsigned long long)g_trajectory_reject112[2].load(),(unsigned long long)g_trajectory_reject112[3].load(),
            (unsigned long long)g_trajectory_reject112[4].load(),(unsigned long long)g_trajectory_reject112[5].load(),
            (unsigned long long)g_trajectory_reject112[6].load(),(unsigned long long)g_trajectory_reject112[7].load());
        diagnostic_trace::Write(motion);
        char contact[256]{};std::snprintf(contact,sizeof(contact),
            "V66.114 CONTACT_STATUS kept=%llu rateLimited=%llu queueDropped=%llu rateCap=50_IN_50_OUT_PER_SECOND geometry=RAW_UNQUALIFIED parry=NOT_CLASSIFIED",
            (unsigned long long)g_contact_kept114.load(),(unsigned long long)g_contact_limited114.load(),(unsigned long long)cc::dropped.load());
        diagnostic_trace::Write(contact);
    }
    const auto stamp=animation_filter::DiagnosticPlaybackStamp();
    char m[600]{};std::snprintf(m,sizeof(m),"V66.135-LAB STATUS tick=%llu epoch=%llu F3=%d F5=%d calibration=%d requestedVrik=%d census=%d combatReference=%d interactionReference=%d committedPose=%d latestPlaybackSeq=%llu playbackAgeMs=%llu visibility=UNVERIFIED runtimeBackend=%s",
        (unsigned long long)now,(unsigned long long)(diagnostics::lab::Snapshot()>>4),HolsterCombatDiagMode(),MiscInteractionDiagMode(),CalibrationReady(),diagnostics::lab::WantsVrik(),g_census_mode.load(),diagnostics::lab::CombatReference(),diagnostics::lab::InteractionReference(),g_committed_pose_valid6689.load(),(unsigned long long)stamp.serial,(unsigned long long)(stamp.tick&&now>=stamp.tick?now-stamp.tick:UINT64_MAX),MiscInteractionDiagMode()==3?"HELD_CONTEXTUAL_HAND_USE":MiscInteractionDiagMode()==2?"HAND_RAY_NATIVE_USE":"OBSERVATION_ONLY");diagnostic_trace::Write(m);
}
void ObserveCollisionAnimationCharacter(void* character,std::uint32_t id,std::uint64_t contactSeq) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64())||!character||!id)return;
    // Natural GetCharacter result only. Save a verified character association;
    // animation queues are sampled later on its OWN validated skinning callback.
    void* anim=nullptr;
    if(character&&ValidateCharInstance(character,anim))animation_filter::RegisterContactAnimation(anim,id,contactSeq);
}


