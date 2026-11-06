static std::atomic<bool> g_bow_grip_default596c1{true};
static std::atomic<bool> g_staged_bow_default59{true},g_staged_bow_ready59{false};
static std::atomic<int> g_reload_mode47c2{0};
static int g_reload_prior_weapon47c2=-1,g_reload_prior_pose47c2=0;
static std::uint64_t g_reload_weapon_epoch47c2=0;
static std::uint64_t g_reload_last175=0,g_reload_session175=0;
int ReloadMode47C2() noexcept {return g_reload_mode47c2.load(std::memory_order_acquire);}
bool ReloadExperiment47C2() noexcept {return ReloadMode47C2()==2;}
bool StagedBowEnabled59() noexcept {
    return ReloadExperiment47C2()||(g_staged_bow_default59.load(std::memory_order_acquire)&&
        g_staged_bow_ready59.load(std::memory_order_acquire)&&g_reload_ready36.load(std::memory_order_acquire));
}
bool StagedBowDefault59() noexcept {return g_staged_bow_default59.load(std::memory_order_acquire);}
void SetStagedBowDefault59(bool on) noexcept {
    const bool before=StagedBowEnabled59();
    g_staged_bow_default59.store(on,std::memory_order_release);
    if(before!=StagedBowEnabled59())g_weapon_request_epoch6694.fetch_add(1,std::memory_order_acq_rel);
}
bool PrepareStagedBowDefault59() noexcept {
    if(!g_staged_bow_default59.load(std::memory_order_acquire)&&!CrossbowReloadEnabledD65C())return false;
    if(g_staged_bow_ready59.load(std::memory_order_acquire))return g_reload_ready36.load(std::memory_order_acquire);
    if(!CalibrationReady()||!retail_build112::Ready(WhBase())||!StageLeafPins596(kReloadLeaves596)||!ArmReloadNative36())return false;
    const bool before=StagedBowEnabled59();
    g_staged_bow_ready59.store(true,std::memory_order_release);
    if(before!=StagedBowEnabled59())g_weapon_request_epoch6694.fetch_add(1,std::memory_order_acq_rel);
    diagnostic_trace::Write("C59 BOW_DEFAULT staged=READY draw=GRIP_PHYSICAL_PULL_RELEASE ammo=NATIVE_QUIVER_NOCK recorder=UNCHANGED");
    return true;
}
bool BowGripDrawEnabled596C1() noexcept {
    return g_bow_grip_default596c1.load(std::memory_order_acquire)||StagedBowEnabled59();
}
void SetBowGripDrawDefault596C1(bool on) noexcept {
    const bool before=BowGripDrawEnabled596C1();
    g_bow_grip_default596c1.store(on,std::memory_order_release);
    if(before!=BowGripDrawEnabled596C1())g_weapon_request_epoch6694.fetch_add(1,std::memory_order_acq_rel);
    char line[256]{};std::snprintf(line,sizeof(line),
        "C59 BOW_DEFAULT preferredDraw=%s effectiveDraw=%s ammo=%s recorder=UNCHANGED",
        on?"GRIP":"TRIGGER",BowGripDrawEnabled596C1()?"GRIP_PHYSICAL_PULL_RELEASE":"TRIGGER_PHYSICAL_PULL_RELEASE",
        StagedBowEnabled59()?"NATIVE_QUIVER_NOCK":"NATIVE_AUTOMATIC");diagnostic_trace::Write(line);
}
bool SetReloadMode47C2(int mode) noexcept {
    mode=std::clamp(mode,0,2);
    const int requested=mode;
    const bool stagedBefore=StagedBowEnabled59();
    // An already qualified default owns staging in every ordinary bow-handling
    // mode. F11 recording must not upgrade that mode or reseed a held bow.
    const bool keepBowMode59=stagedBefore&&BowHandlingMode59c9(g_weapon_control_mode.load());
    if(mode==2&&(!CalibrationReady()||!retail_build112::Ready(WhBase())||!StageLeafPins596(kReloadLeaves596))){
        diagnostic_trace::Write("V66.178R47C2 F11-RELOAD MODE blocked=CALIBRATION_OR_RETAIL_REQUIRED");return false;
    }
    bool recorderStarted=false;
    if(mode==2){
        (void)SetReloadDiagnostics175(true);recorderStarted=true;
        if(!g_reload_ready36.load()){
            diagnostic_trace::Write("R59.6C2 F11-RELOAD MODE blocked=STAGE_HOOKS_UNAVAILABLE gameplayModes=UNCHANGED");return false;
        }
    }
    // Input policy changes invalidate pending native commands. The game-thread
    // pump performs cancellation; this hotkey worker never calls a bow method.
    if(mode!=2&&g_reload_prior_weapon47c2>=0){
        if(g_weapon_request_epoch6694.load()==g_reload_weapon_epoch47c2&&g_weapon_control_mode.load()==6){
            SetWeaponControlMode(g_reload_prior_weapon47c2);
            if(g_census_mode.load()==3)SetCombatCensusMode(g_reload_prior_pose47c2);
        }
        g_reload_prior_weapon47c2=-1;g_reload_weapon_epoch47c2=0;
    }
    g_reload_mode47c2.store(mode,std::memory_order_release);
    // C59: recorder OFF/ON is observational. Only an actual change of staged
    // gameplay ownership cancels its native operation, including rapid edges.
    if(stagedBefore!=StagedBowEnabled59())g_weapon_request_epoch6694.fetch_add(1,std::memory_order_acq_rel);
    if(mode==2&&!FullWeaponMode120(g_weapon_control_mode.load())&&!keepBowMode59){
        g_reload_prior_weapon47c2=g_weapon_control_mode.load();g_reload_prior_pose47c2=g_census_mode.load();
        SetWeaponControlMode(6);g_reload_weapon_epoch47c2=g_weapon_request_epoch6694.load();
    }
    if(mode==2&&!FullWeaponMode120(g_weapon_control_mode.load())&&!keepBowMode59){g_reload_mode47c2=1;mode=1;}
    if(!recorderStarted)(void)SetReloadDiagnostics175(mode!=0);
    char line[360]{};std::snprintf(line,sizeof(line),
        "H21 F11-RELOAD MODE rung=%d draw=%s ammo=NATIVE rung1=DIAGNOSTICS rung2=RANGED_STAGED_INPUT_THEN_NATIVE_RELOAD bow=EXISTING_QUIVER_NOCK experimentOffRestores=%s",
        mode,mode==2?"GRIP_PHYSICAL_PULL_RELEASE":"UNCHANGED",g_bow_grip_default596c1.load()?"GRIP_PHYSICAL_DRAW":"TRIGGER_PHYSICAL_DRAW");diagnostic_trace::Write(line);
    return requested==g_reload_mode47c2.load();
}
bool ReloadDiagnostics175() noexcept {return diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,GetTickCount64());}
bool SetReloadDiagnostics175(bool on) noexcept {
    EndReloadNative36(on?"RESTART":"OFF");
    diagnostics::capture168::select(diagnostics::capture168::Domain::Reload,on,GetTickCount64(),"F11");
    g_reload_last175=0;g_reload_session175=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Reload)].load();
    if(on){g_arrow_client_logs128=0;g_missile_helper_logs136r3=0;g_reload_attach_event_logs136r3=0;}
    const bool playback=on&&(animation_filter::Ready()||animation_filter::Install());
    if(on)(void)BeginReloadNative36(g_reload_session175);
    char m[512]{};std::snprintf(m,sizeof(m),
        "V66.175 RELOAD_COVERAGE state=%s session=%llu playback=%d attachment=%d tracking=COPIED_SNAPSHOTS ammoPresence=NATIVE_EVIDENCE_ONLY recovery=PICKUP_REQUEST_NOT_INVENTORY_PROOF durationMs=%u gameplayModes=UNCHANGED writes=NONE",
        on?"BEGIN":"END",(unsigned long long)g_reload_session175,playback,bool(g_attachment_helper_transform_original),on?180000u:0u);
    diagnostic_trace::Write(m);return true;
}
static void ServiceReloadDiagnostics175() noexcept {
    ServiceReloadNative36();
    const auto now=std::uint64_t(GetTickCount64());if(!ReloadDiagnostics175()){EndReloadNative36("EXPIRED_OR_DOMAIN_CHANGED");return;}
    if(g_reload_last175&&now>=g_reload_last175&&now-g_reload_last175<200)return;
    g_reload_last175=now;
    TrackingSnapshot tracking{};const bool haveTracking=ReadTrackingSnapshot(tracking);
    // Observe only a completed current publication. This worker never touches the
    // native-owner cache/mutex and never makes an in-flight frame look fresh.
    const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    PhysFrame6698 frame{};const bool haveFrame=ready&&g_phys_frame6698.read(frame)&&frame.input.sample==ready&&
        ready==g_phys_ready_sample6698.load(std::memory_order_acquire)&&
        frame.request==g_weapon_request_epoch6694.load()&&frame.input.bodyEpoch==g_pose_epoch6689.load()&&
        frame.input.tick&&now>=frame.input.tick&&now-frame.input.tick<=100;
    int weapon=0;const bool lifecycle=animation_filter::ReloadDiagnosticWindow136R3(now,weapon);
    char m[1280]{};std::snprintf(m,sizeof(m),
        "V66.175 F11-RELOAD SNAPSHOT session=%llu tick=%llu nativeWeapon=%s phaseWindow=%d xr=%llu sourceTick=%llu trackingRead=%d H/R/L=%d/%d/%d right=(%.4f,%.4f,%.4f) left=(%.4f,%.4f,%.4f) grip=%.3f/%.3f trigger=%.3f/%.3f frameFresh=%d physSample=%llu keyR=%016llX keyL=%016llX bowHeld=%d drawFraction=%.4f nativeCharge=%.4f nativeChargeTick=%llu gripDraw=%d couplingHand=%d reason=%d along=%.4f lateral=%.4f nockGap=%.4f ammoPresence=UNKNOWN_UNTIL_NATIVE_EVIDENCE",
        (unsigned long long)g_reload_session175,(unsigned long long)now,animation_filter::ReloadDiagnosticWeaponName136R3(weapon),lifecycle,
        (unsigned long long)tracking.sequence,(unsigned long long)tracking.tick,haveTracking,tracking.head_valid,tracking.right_valid,tracking.left_valid,
        double(tracking.right.x),double(tracking.right.y),double(tracking.right.z),double(tracking.left.x),double(tracking.left.y),double(tracking.left.z),
        double(tracking.right_grip),double(tracking.left_grip),double(tracking.right_trigger),double(tracking.left_trigger),haveFrame,
        (unsigned long long)(haveFrame?frame.input.sample:0),(unsigned long long)(haveFrame?frame.profiles[0].key:0),(unsigned long long)(haveFrame?frame.profiles[1].key:0),
        haveFrame&&frame.output.bowHeld,haveFrame?frame.output.bowFraction:0.,double(g_bow_charge128.load()),(unsigned long long)g_bow_charge_tick128.load(),
        haveFrame&&frame.input.bowGripDraw47c2,haveFrame&&frame.output.bowHand169r2,haveFrame?int(frame.output.bow116.reason):-1,
        haveFrame?frame.output.bow116.along:0.,haveFrame?frame.output.bow116.lateral:0.,haveFrame?frame.output.bow116.gap:0.);
    diagnostic_trace::Write(m);
}

