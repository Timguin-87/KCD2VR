// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 13
// src/mod.cpp: fillmenustate_01.inl
// M2_SLOT_BEGIN 13
    out.vrik_census_mode = frik_pose_probe::CombatCensusMode(); // V65.09 F10 right-arm attachment-order ladder
    out.vrik_calibration_ui_state = vrik_calibration_ui_state_.load(std::memory_order_acquire);
    out.vrik_finger_capture_state=frik_pose_probe::FingerCaptureStatus();
    out.vrik_calibration_persistence_state=frik_pose_probe::CalibrationPersistenceStatus();
    out.vrik_finger_capture_active=frik_pose_probe::FingerCaptureActive();
// M2_SLOT_END 13
#elif KCD2VR_M2_HOST_SLOT == 28
// src/mod.cpp: fillmenustate_02.inl
// M2_SLOT_BEGIN 28
    out.vrik_body_follow = vrik_body_follow_.load(std::memory_order_acquire);
// M2_SLOT_END 28
#elif KCD2VR_M2_HOST_SLOT == 35
// src/mod.cpp: fillmenustate_03.inl
// M2_SLOT_BEGIN 35
    out.user_shoulder_holsters = user_shoulder_holsters_.load(std::memory_order_acquire);
// M2_SLOT_END 35
#elif KCD2VR_M2_HOST_SLOT == 38
// src/mod.cpp: fillmenustate_04.inl
// M2_SLOT_BEGIN 38
    out.vrik_probe_enabled = enable_vrik_ownership_test_.load(std::memory_order_acquire);
    {
        const auto vs = frik_pose_probe::GetStatus();
        out.vrik_mode = vs.mode>0 ? vs.mode : (out.vrik_census_mode>0 ? 1 : 0);
        out.vrik_rig_valid = vs.rig_valid;
        out.vrik_joint_count = vs.joint_count;
        out.vrik_hand_r = vs.hand_r;
        out.vrik_correlated = vs.correlated_calls;
        out.vrik_final_writes = vs.final_output_writes;
        out.vrik_prebuild_writes = vs.prebuild_writes;
        out.vrik_rejects = vs.rejects;
    }
// M2_SLOT_END 38
#elif KCD2VR_M2_HOST_SLOT == 75
// src/mod.cpp: applymenustate_01.inl
// M2_SLOT_BEGIN 75
    { // D84: explicit physical-walking choices use the existing finite D76 native step owner.
        // D40 LIVE APPLY runs first, so compare against the captured previous native choice.
        // An unrelated menu apply preserves an old INI's explicit OFF or legacy FOLLOW.
        const int roomscale=kcd2vr::roomscale58::bodySelection(vrik_body_follow_.load(),state.vrik_body_follow,state.roomscale_mode);
        const bool walking=roomscale==1||roomscale==2,dodge=roomscale>=2;
        const bool wasWalking=nativeRoomscaleWasEnabledD84||g_roomscale_follow58.load(std::memory_order_acquire);
        const bool nativeChoiceChanged=state.live_d39.nativeRoomscaleD76!=nativeRoomscaleWasEnabledD84;
        if(walking!=wasWalking||nativeChoiceChanged){
            kcd2vr::roomscale_d76::SetEnabled(walking!=wasWalking?walking:state.live_d39.nativeRoomscaleD76);
            g_roomscale_follow58.store(false,std::memory_order_release); // no second offset-to-stick owner
        }else if(kcd2vr::roomscale_d76::Enabled())g_roomscale_follow58.store(false,std::memory_order_release);
        g_roomscale_dodge58.store(dodge,std::memory_order_release);
        const bool walkingNow=kcd2vr::roomscale_d76::Enabled()||g_roomscale_follow58.load(std::memory_order_acquire);
        CameraHook::SetRoomscaleFollow((walkingNow&&state.vrik_body_follow)||dodge||frik_pose_probe::CombatExperiment175Enabled());
    }
// M2_SLOT_END 75
#elif KCD2VR_M2_HOST_SLOT == 84
// src/mod.cpp: applymenustate_02.inl
// M2_SLOT_BEGIN 84
    if(vrik_body_follow_.exchange(state.vrik_body_follow,std::memory_order_acq_rel)!=state.vrik_body_follow){frik_pose_probe::SetBodyFollow128(state.vrik_body_follow);CameraHook::SetRoomscaleFollow(((kcd2vr::roomscale_d76::Enabled()||g_roomscale_follow58.load())&&state.vrik_body_follow)||g_roomscale_dodge58.load()||frik_pose_probe::CombatExperiment175Enabled());} // R58; R59.16C5 dodge alone
    if(user_shoulder_holsters_.exchange(state.user_shoulder_holsters,std::memory_order_acq_rel)!=state.user_shoulder_holsters)
        frik_pose_probe::SetLegacyHolsterLayout147(state.user_shoulder_holsters);
// M2_SLOT_END 84
#elif KCD2VR_M2_HOST_SLOT == 101
// src/mod.cpp: savemenusettings_01.inl
// M2_SLOT_BEGIN 101
    ok = wb("UserShoulderHolsters", user_shoulder_holsters_.load(std::memory_order_acquire)) && ok;
    ok = wb("LegacyShoulderHolsters", user_shoulder_holsters_.load(std::memory_order_acquire)) && ok; // backward compatibility only
// M2_SLOT_END 101
#elif KCD2VR_M2_HOST_SLOT == 110
// src/mod.cpp: savemenusettings_02.inl
// M2_SLOT_BEGIN 110
    ok = wb("VrikBodyFollow6DOF", vrik_body_follow_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrikBodyFollow6DOFVersion", 1) && ok; // D38: the one-time ON below is done
// M2_SLOT_END 110
#elif KCD2VR_M2_HOST_SLOT == 138
// src/mod.cpp: loadconfig_01.inl
// M2_SLOT_BEGIN 138
    settings.bind("VrikBodyFollow6DOF", vrik_body_follow_, false);
// M2_SLOT_END 138
#elif KCD2VR_M2_HOST_SLOT == 144
// src/mod.cpp: loadconfig_02.inl
// M2_SLOT_BEGIN 144
    future.bind("EnableBodyIK", enable_body_ik_, false);
    future.bind("EnableVirtualHolsters", enable_virtual_holsters_, false);
    future.bind("EnableMotionControls", enable_motion_controls_, false);
    future.bind("EnableNativeMelee", enable_native_melee_, false);
    future.bind("EnableStaminaWeaponHandling", enable_stamina_weapon_handling_, false);
// M2_SLOT_END 144
#elif KCD2VR_M2_HOST_SLOT == 148
// src/mod.cpp: loadconfig_03.inl
// M2_SLOT_BEGIN 148
    experimental.bind("EnableVrikSemanticOwnershipTest", enable_vrik_ownership_test_, false);
// M2_SLOT_END 148
#elif KCD2VR_M2_HOST_SLOT == 177
// src/mod.cpp: loadconfig_04.inl
// M2_SLOT_BEGIN 177
    // R124 (Miki: "my current .ini settings as default"): 6DOF BODY default OFF, the saved choice always wins.
    // The D38 one-time ON for an INI without VrikBodyFollow6DOFVersion is retired (it overwrote a saved OFF).
    vrik_body_follow_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrikBodyFollow6DOF",false),std::memory_order_relaxed);
    frik_pose_probe::SetBodyFollow128(vrik_body_follow_.load(std::memory_order_relaxed));
// M2_SLOT_END 177
#elif KCD2VR_M2_HOST_SLOT == 186
// src/mod.cpp: loadconfig_05.inl
// M2_SLOT_BEGIN 186
    CameraHook::SetRoomscaleFollow(((kcd2vr::roomscale_d76::Enabled()||g_roomscale_follow58.load())&&vrik_body_follow_.load(std::memory_order_relaxed))||g_roomscale_dodge58.load()||frik_pose_probe::CombatExperiment175Enabled()); // R59.16C5
    frik_pose_probe::SetChestTwist128(ReadIniBoolExplicit(explicit_ini,"Settings","VrikChestNaturalTwist",true),ReadIniBoolExplicit(explicit_ini,"Settings","VrikChestHandBias",true));
// M2_SLOT_END 186
#elif KCD2VR_M2_HOST_SLOT == 188
// src/mod.cpp: loadconfig_06.inl
// M2_SLOT_BEGIN 188
    user_shoulder_holsters_.store(userHolsters149,std::memory_order_relaxed);
    frik_pose_probe::SetLegacyHolsterLayout147(userHolsters149); // internal legacy flag == requested user layout
    frik_pose_probe::SetHolsterPolicy136(ReadIniFloatExplicit(explicit_ini,"Settings","HolsterZoneRadius",0.28f),
        migrateHolster146||ReadIniBoolExplicit(explicit_ini,"Settings","HolsterOnGripRelease",true));
// M2_SLOT_END 188
#elif KCD2VR_M2_HOST_SLOT == 192
// src/mod.cpp: loadconfig_07.inl
// M2_SLOT_BEGIN 192
    if(ReadIniBoolExplicit(explicit_ini,"Settings","VrikHandAimPitch",false))
        diagnostic_trace::Write("V66.162 VrikHandAimPitch ignored: retired unsafe native-look owner; hand ray retained");
// M2_SLOT_END 192
#elif KCD2VR_M2_HOST_SLOT == 209
// src/mod.cpp: loadconfig_08.inl
// M2_SLOT_BEGIN 209
    kcd2vr::vr_combat111::attackConnectKeepsVrik50.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrAttackConnectKeepsVrik",true)); // V66.178R50
    kcd2vr::vr_combat111::hitReactionsKeepVrik178r31.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrHitReactionKeepsVrik",true)); // V66.178R31
// M2_SLOT_END 209
#elif KCD2VR_M2_HOST_SLOT == 219
// src/mod.cpp: loadconfig_09.inl
// M2_SLOT_BEGIN 219
    enable_vrik_ownership_test_.store(ReadIniBoolExplicit(explicit_ini, "Experimental", "EnableVrikSemanticOwnershipTest", false), std::memory_order_relaxed);
// M2_SLOT_END 219
#elif KCD2VR_M2_HOST_SLOT == 238
// src/mod.h: mod_state_01.inl
// M2_SLOT_BEGIN 238
    std::atomic<bool> vrik_body_follow_{false};
    std::atomic<bool> user_shoulder_holsters_{true}; // V66.149 user layout (R14B): bow right shoulder/left hand only; shield left shoulder/left hand; polearm right shoulder/right hand
// M2_SLOT_END 238
#elif KCD2VR_M2_HOST_SLOT == 244
// src/mod.h: mod_state_02.inl
// M2_SLOT_BEGIN 244
    std::atomic<bool> enable_vrik_ownership_test_{false}; // V61.43 opt-in semantic +0x5FE2F0 ownership ladder
    std::atomic<bool> vrik_cycle_requested_{false}; // V63.0 reliable WndProc/Async hotkey handoff
// M2_SLOT_END 244
#elif KCD2VR_M2_HOST_SLOT == 245
// src/mod.h: mod_state_03.inl
// M2_SLOT_BEGIN 245
    std::atomic<int> vrik_menu_mode_requested_{-1};
    std::atomic<int> vrik_calibration_ui_state_{0}; // 0 idle, 3/2/1 countdown, 4 NOW/capture, 5 complete
    std::atomic<bool> vrik_recalibrate_requested_{false};
    std::atomic<bool> vrik_restore_requested_{false}; // C43 menu RESTORE CALIBRATION
    std::atomic<bool> enable_body_ik_{false};
    std::atomic<bool> enable_virtual_holsters_{false};
    std::atomic<bool> enable_motion_controls_{false};
    std::atomic<bool> enable_native_melee_{false};
    std::atomic<bool> enable_stamina_weapon_handling_{false};
// M2_SLOT_END 245
#elif KCD2VR_M2_HOST_SLOT == 249
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 249
    float fingers40[2][5][4] = {{{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},{{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}};
// M2_SLOT_END 249
#elif KCD2VR_M2_HOST_SLOT == 256
// src/vr_menu.h: menu_state_02.inl
// M2_SLOT_BEGIN 256
    int vrik_census_mode = 0;              // V65.09 F10 right-arm attachment-order rung
// M2_SLOT_END 256
#elif KCD2VR_M2_HOST_SLOT == 271
// src/vr_menu.h: menu_state_03.inl
// M2_SLOT_BEGIN 271
    bool vrik_body_follow = false;          // V66.128: pelvis follow + leg IK (F1)
// M2_SLOT_END 271
#elif KCD2VR_M2_HOST_SLOT == 279
// src/vr_menu.h: menu_state_04.inl
// M2_SLOT_BEGIN 279
    bool user_shoulder_holsters = true;    // V66.149: USER SHOULDER LAYOUT (R14B: bow right shoulder/left hand; shield left shoulder/left hand; polearm/crossbow right shoulder/right hand)
// M2_SLOT_END 279
#elif KCD2VR_M2_HOST_SLOT == 282
// src/vr_menu.h: menu_state_05.inl
// M2_SLOT_BEGIN 282
    float exp_head_raise_cm = 18.f;         // VrHeadRaiseMaxCm
// M2_SLOT_END 282
#elif KCD2VR_M2_HOST_SLOT == 314
// src/vr_menu.h: menu_state_06.inl
// M2_SLOT_BEGIN 314
    bool vrik_probe_enabled = false;
    int vrik_mode = 0;
    int vrik_calibration_ui_state = 0;
    int vrik_finger_capture_state = 0;
    int vrik_calibration_persistence_state = 0;
    bool vrik_finger_capture_active = false;
    bool vrik_rig_valid = false;
    std::uint32_t vrik_joint_count = 0;
    int vrik_hand_r = -1;
    std::uint64_t vrik_correlated = 0;
    std::uint64_t vrik_final_writes = 0;
    std::uint64_t vrik_prebuild_writes = 0;
    std::uint64_t vrik_rejects = 0;
// M2_SLOT_END 314
#elif KCD2VR_M2_HOST_SLOT == 5017
// src/mod.cpp: roomscale_pad
// M2_SLOT_BEGIN 5017
static void ApplyRoomscalePad58(native_vr_pad_premap::State& pad,const native_vr_pad_premap::Context& padCtx,bool trackedGameplay) noexcept
{
    static kcd2vr::hmd_dodge_h24::Request dodgeRequestH24{};
    // R59.16C5: FOLLOW and DODGE are separate. Before, the dodge needed FOLLOW + 6DOF BODY, the F4 experiment, a
    // guard/parry-resolved opponent, AND an untouched left stick - so moving with the stick in a fight disabled it.
    const bool context=!padCtx.vr_menu&&!padCtx.theater&&!padCtx.system_menu&&!padCtx.dialogue_navigation&&!padCtx.semantic_minigame&&
        !padCtx.apse_ui&&!padCtx.back_context&&trackedGameplay&&!animation_filter::nativeBodyOwned112.load();
    roomscale_d76::Context(context&&frik_pose_probe::BodyFollow128());
    // One small receipt on the control thread, only during an existing capture
    // or Shift+F2 performance session. No extra native reads or visible messages.
    if(roomscale_d76::Enabled()){
        static std::uint64_t nextD76=0;const auto nowD76=std::uint64_t(GetTickCount64());
        if(nowD76>=nextD76&&(diagnostics::capture168::any(nowD76)||diagnostics::perf_d61::gate.active(nowD76))){
            nextD76=nowD76+1000;const auto c=native_roomscale_d76::ReadCounters();char line[480]{};
            std::snprintf(line,sizeof(line),"D76_ROOMSCALE context=%d ready=%d submitted=%llu applied=%llu queued=%llu entity=%llu character=%llu stale=%llu rejected=%llu faults=%llu lostReceipt=%llu units=METRES joystick=INDEPENDENT",
                context?1:0,native_roomscale_d76::Ready()?1:0,(unsigned long long)c.submitted,(unsigned long long)c.applied,
                (unsigned long long)c.queued,(unsigned long long)c.entityAdopted,(unsigned long long)c.characterAdopted,
                (unsigned long long)c.stale,(unsigned long long)c.rejected,(unsigned long long)c.faults,(unsigned long long)c.lostReceipt);
            diagnostic_trace::Write(line);
        }
    }
    const bool followOn=!roomscale_d76::Enabled()&&g_roomscale_follow58.load(std::memory_order_relaxed)&&frik_pose_probe::BodyFollow128();
    const bool dodgeOn=g_roomscale_dodge58.load(std::memory_order_relaxed); // C54: explicit Immersion switch owns dodge; F4 cannot override OFF
    const bool stickFree=std::abs(pad.lx)<.01f&&std::abs(pad.ly)<.01f; // the stick wins over FOLLOW, never over a dodge
    if(!context||(!followOn&&!dodgeOn)){dodgeRequestH24={};kcd2vr::hmd_dodge_h24::cancelRequest();(void)CameraHook::RoomscaleDrive(GetTickCount64(),false,false,false);return;}
    const auto rsNow=std::uint64_t(GetTickCount64());std::uint64_t rsTick=0;float rx=0,ry=0;bool leanHeldD70=false;
    static std::uint64_t dodgeEpochH24=0;static void* dodgeEntityH24=nullptr;
    const auto currentEpochH24=frik_pose_probe::PoseEpochH24();void* currentEntityH24=frik_pose_probe::LastPlayerEntity();
    if(dodgeEpochH24!=currentEpochH24||dodgeEntityH24!=currentEntityH24){
        dodgeRequestH24={};kcd2vr::hmd_dodge_h24::cancelRequest();
        (void)CameraHook::RoomscaleDrive(rsNow,false,false,false);
        dodgeEpochH24=currentEpochH24;dodgeEntityH24=currentEntityH24;
    }
    if(CameraHook::RoomscaleRemaining(rx,ry,rsTick)&&rsTick&&rsNow>=rsTick&&rsNow-rsTick<=60){
        // D74: request side/back dodge in an exact ordinary reciprocal pair.
        // The game decides acceptance/perfect timing; waiting for an enemy's
        // attack here used to discard pre-emptive physical dodge gestures.
        const bool combatLocked=combat_diag_probe::EngagedHmdDodgeD74(rsNow);
        const auto d=CameraHook::RoomscaleDrive(rsNow,followOn&&stickFree,combatLocked,dodgeOn);
        leanHeldD70=d.forwardLeanHeldD70;
        if(d.dodge){
            namespace hd=kcd2vr::hmd_dodge_h24;
            hd::Request live{};
            if(!dodgeRequestH24.serial){
                if(!hd::beginRequest(reinterpret_cast<std::uintptr_t>(currentEntityH24),currentEpochH24,rsNow,rsTick,dodgeRequestH24))return;
            }else if(!hd::currentRequest(reinterpret_cast<std::uintptr_t>(currentEntityH24),
                dodgeRequestH24.owner.actor,dodgeRequestH24.owner.state,currentEpochH24,rsNow,live)){
                hd::cancelRequest();return; // never renew a failed/expired gesture into a new held request
            }else{dodgeRequestH24.poseTick=rsTick;(void)hd::requestPublished.publish(dodgeRequestH24);}
            if(!hd::releaseReady(dodgeRequestH24)){
                if(rsNow-dodgeRequestH24.started>60)hd::cancelRequest();
                return; // our native LT owner must acknowledge actual release first
            }
            pad.b=true;pad.lx=d.dodgeLx;pad.ly=d.dodgeLy;
            static std::uint64_t lastDodgeLog58=0;if(rsNow-lastDodgeLog58>=500&&diagnostics::capture168::any(rsNow)){lastDodgeLog58=rsNow;char m[220]{};std::snprintf(m,sizeof(m),"R59.16C5 ROOMSCALE_DODGE attemptedDirection=(%.2f,%.2f) transport=XI_B_PLUS_STICK follow=%d nativeAccepted=UNOBSERVED perfectDodge=UNOBSERVED",double(d.dodgeLx),double(d.dodgeLy),followOn?1:0);diagnostic_trace::Write(m);}}
        else {dodgeRequestH24={};kcd2vr::hmd_dodge_h24::cancelRequest();}
        if(!d.dodge&&d.following){pad.lx=std::clamp(pad.lx+d.lx,-1.f,1.f);pad.ly=std::clamp(pad.ly+d.ly,-1.f,1.f);}
    }
    else {dodgeRequestH24={};kcd2vr::hmd_dodge_h24::cancelRequest();(void)CameraHook::RoomscaleDrive(rsNow,false,false,false);}
    static std::uint64_t lastRoomLog58=0;
    if(rsNow-lastRoomLog58>=5000&&diagnostics::capture168::any(rsNow)){lastRoomLog58=rsNow;std::uint64_t consumed=0,dodges=0,injected=0;float ax=0,ay=0;
        CameraHook::RoomscaleCounters(consumed,dodges,injected,ax,ay);char m[300]{};
        std::snprintf(m,sizeof(m),"V66.178R58 ROOMSCALE remaining=(%.2f,%.2f) anchor=(%.2f,%.2f) consumedFrames=%llu injectedFrames=%llu dodges=%llu follow=%d forwardLeanHeld=%d leanHeightCm=%.1f",double(rx),double(ry),double(ax),double(ay),(unsigned long long)consumed,(unsigned long long)injected,(unsigned long long)dodges,followOn?1:0,leanHeldD70?1:0,100.*kcd2vr::roomscale58::LeanHeightD70());diagnostic_trace::Write(m);}
}

// M2_SLOT_END 5017
#else
#error Unknown M2 host contribution slot
#endif
