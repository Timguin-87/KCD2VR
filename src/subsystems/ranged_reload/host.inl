// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 36
// src/mod.cpp: fillmenustate_01.inl
// M2_SLOT_BEGIN 36
    out.bow_manual_reload = frik_pose_probe::StagedBowDefault59();
    out.crossbow_no_zoom32 = ranged_options_xr32::crossbowNoZoom.load(std::memory_order_acquire);
// M2_SLOT_END 36
#elif KCD2VR_M2_HOST_SLOT == 85
// src/mod.cpp: applymenustate_01.inl
// M2_SLOT_BEGIN 85
    ranged_options_xr32::crossbowNoZoom.store(state.crossbow_no_zoom32,std::memory_order_release);
    bow_manual_reload_.store(false,std::memory_order_release);
    frik_pose_probe::SetBowReload146(false,bow_native_grab_at_quiver_.load(std::memory_order_acquire));
// M2_SLOT_END 85
#elif KCD2VR_M2_HOST_SLOT == 86
// src/mod.cpp: applymenustate_02.inl
// M2_SLOT_BEGIN 86
    if(state.bow_manual_reload)(void)frik_pose_probe::PrepareStagedBowDefault59();
// M2_SLOT_END 86
#elif KCD2VR_M2_HOST_SLOT == 102
// src/mod.cpp: savemenusettings_01.inl
// M2_SLOT_BEGIN 102
    ok = wb("VrBowPhysicalAimD65C", ranged_options_d65c::bowPhysicalAim.load(std::memory_order_acquire)) && ok;
    ok = wb("VrCrossbowReloadD65C", ranged_options_d65c::crossbowReload.load(std::memory_order_acquire)) && ok;
    ok = wf("VrCrankTravelD66C", ranged_options_d66c::crankTravelSetting(ranged_options_d66c::crankTravel.load(std::memory_order_acquire))) && ok;
    ok = wb("VrCrossbowNoZoom", ranged_options_xr32::crossbowNoZoom.load(std::memory_order_acquire)) && ok;
    ok = wb("VrManualBowDefault59", frik_pose_probe::StagedBowDefault59()) && ok;
    ok = wb("BowNativeGrabAtQuiver", bow_native_grab_at_quiver_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 102
#elif KCD2VR_M2_HOST_SLOT == 120
// src/mod.cpp: savemenusettings_02.inl
// M2_SLOT_BEGIN 120
    ok = wb("EnableBowZoomGuard", enable_bow_zoom_guard_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 120
#elif KCD2VR_M2_HOST_SLOT == 140
// src/mod.cpp: loadconfig_01.inl
// M2_SLOT_BEGIN 140
    settings.bind("EnableBowZoomGuard", enable_bow_zoom_guard_, false);
// M2_SLOT_END 140
#elif KCD2VR_M2_HOST_SLOT == 146
// src/mod.cpp: loadconfig_02.inl
// M2_SLOT_BEGIN 146
    future.bind("EnablePhysicalBow", enable_physical_bow_, false);
// M2_SLOT_END 146
#elif KCD2VR_M2_HOST_SLOT == 187
// src/mod.cpp: loadconfig_03.inl
// M2_SLOT_BEGIN 187
    frik_pose_probe::SetBowStringFollow128(ReadIniBoolExplicit(explicit_ini,"Settings","BowStringFollowsHand",true));
// M2_SLOT_END 187
#elif KCD2VR_M2_HOST_SLOT == 191
// src/mod.cpp: loadconfig_04.inl
// M2_SLOT_BEGIN 191
    bow_manual_reload_.store(ReadIniBoolExplicit(explicit_ini,"Settings","BowManualReload",false),std::memory_order_relaxed);
    if(ReadIniIntExplicit(explicit_ini,"Settings","BowTriggerPreference163",0)<1){
        bow_manual_reload_.store(false,std::memory_order_relaxed);
        WritePrivateProfileStringA("Settings","BowManualReload","false",explicit_ini);
        WritePrivateProfileStringA("Settings","BowTriggerPreference163","1",explicit_ini);
    }
    bow_native_grab_at_quiver_.store(ReadIniBoolExplicit(explicit_ini,"Settings","BowNativeGrabAtQuiver",true),std::memory_order_relaxed);
    frik_pose_probe::SetBowReload146(bow_manual_reload_.load(std::memory_order_relaxed),
        bow_native_grab_at_quiver_.load(std::memory_order_relaxed));
// M2_SLOT_END 191
#elif KCD2VR_M2_HOST_SLOT == 206
// src/mod.cpp: loadconfig_05.inl
// M2_SLOT_BEGIN 206
    frik_pose_probe::SetBowStringGlue178r9(ReadIniBoolExplicit(explicit_ini,"Settings","VrBowStringGlue",true));
// M2_SLOT_END 206
#elif KCD2VR_M2_HOST_SLOT == 212
// src/mod.cpp: loadconfig_06.inl
// M2_SLOT_BEGIN 212
    frik_pose_probe::SetBowStringTrigger160(ReadIniBoolExplicit(explicit_ini,"Settings","BowStringTrigger",true)); // Legacy/native comparison policy.
    frik_pose_probe::SetBowGripDrawDefault596C1(ReadIniBoolExplicit(explicit_ini,"Settings","BowGripDraw",true));
// M2_SLOT_END 212
#elif KCD2VR_M2_HOST_SLOT == 213
// src/mod.cpp: loadconfig_07.inl
// M2_SLOT_BEGIN 213
    bow_manual_reload_.store(false);
    frik_pose_probe::SetBowReload146(false,bow_native_grab_at_quiver_.load());
// M2_SLOT_END 213
#elif KCD2VR_M2_HOST_SLOT == 215
// src/mod.cpp: loadconfig_08.inl
// M2_SLOT_BEGIN 215
    enable_bow_zoom_guard_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableBowZoomGuard", false), std::memory_order_relaxed);
// M2_SLOT_END 215
#elif KCD2VR_M2_HOST_SLOT == 220
// src/mod.h: mod_state_01.inl
// M2_SLOT_BEGIN 220
    std::atomic<bool> enable_bow_zoom_guard_{false};
// M2_SLOT_END 220
#elif KCD2VR_M2_HOST_SLOT == 239
// src/mod.h: mod_state_02.inl
// M2_SLOT_BEGIN 239
    std::atomic<bool> bow_manual_reload_{false}; // V66.149 live Immersion toggle
    std::atomic<bool> bow_native_grab_at_quiver_{false}; // retained INI comparison option
// M2_SLOT_END 239
#elif KCD2VR_M2_HOST_SLOT == 247
// src/mod.h: mod_state_03.inl
// M2_SLOT_BEGIN 247
    std::atomic<bool> enable_physical_bow_{false};
// M2_SLOT_END 247
#elif KCD2VR_M2_HOST_SLOT == 291
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 291
    int   exp_hide_stowed_bow = 2;          // VrHideStowedBow 0 off / 1 when looking up / 2 always while stowed
// M2_SLOT_END 291
#elif KCD2VR_M2_HOST_SLOT == 312
// src/vr_menu.h: menu_state_02.inl
// M2_SLOT_BEGIN 312
    bool crossbow_no_zoom32 = true; // camera-only, saved VrCrossbowNoZoom
    bool bow_manual_reload = true;          // effective saved native staged bow policy (VrManualBowDefault59)
// M2_SLOT_END 312
#elif KCD2VR_M2_HOST_SLOT == 5019
// src/mod.cpp: bow_zoom_stage
// M2_SLOT_BEGIN 5019
        // V61.32 bow-zoom guard. We do not yet have a verified semantic "bow
        // equipped" action owner, so this deliberately keys off the already-used
        // Right-Grip directional/aim gesture and only reacts when KCD2's exact
        // gameplay VFOV actually collapses by >2 degrees. Melee grip use with a
        // stable FOV performs zero writes. F8's own projection feedback supersedes
        // this guard when F8 AUTO is active.
        {
            static bool guard_grip_prev=false;
            static bool guard_adjusted=false;
            static float guard_baseline_actual=0.0f;
            static float guard_baseline_game=0.0f;
            static std::uint64_t guard_last_adjust_ms=0;
            const auto& r=xr_manager_->RightController();
            const bool guard_grip=r.active && r.grip>0.55f && !safe_ui_fit && !(vr_menu_ && vr_menu_->IsVisible());
            const bool guard_enabled=false; // V61.33 superseded by generic immersive transient-zoom reducer
            if(guard_enabled && guard_grip && !guard_grip_prev){
                guard_baseline_actual=CameraHook::LatestGameplayFovDegrees();
                guard_baseline_game=ReadCurrentGameFovValue();
                guard_adjusted=false; guard_last_adjust_ms=0;
                if(guard_baseline_actual>30.0f && guard_baseline_actual<150.0f){
                    char bm[240]{}; std::snprintf(bm,sizeof(bm),
                        "V61.32-BOW-ZOOM-GUARD-LEGACY armed grip=true baselineActual=%.3f baselineGame=%.3f heuristic=right-grip+FOV-drop",
                        double(guard_baseline_actual),double(guard_baseline_game)); debug_trace::Write(bm);
                }
            }
            if(guard_enabled && guard_grip && !f8_auto_projection_active && guard_baseline_actual>30.0f){
                const float actual=CameraHook::LatestGameplayFovDegrees();
                const std::uint64_t gnow=static_cast<std::uint64_t>(GetTickCount64());
                if(actual>30.0f && actual < guard_baseline_actual-2.0f && gnow-guard_last_adjust_ms>=100ull){
                    guard_last_adjust_ms=gnow;
                    const float gameNow=ReadCurrentGameFovValue();
                    const float correction=std::clamp(guard_baseline_actual-actual,1.0f,6.0f);
                    if(gameNow>1.0f && gameNow<149.5f){
                        const float request=std::min(150.0f,gameNow+correction);
                        if(ApplyNativeGameFovV6128Test(request)){
                            guard_adjusted=true;
                            char bm[280]{}; std::snprintf(bm,sizeof(bm),
                                "V61.32-BOW-ZOOM-GUARD-LEGACY suppress actual=%.3f baseline=%.3f game=%.3f request=%.3f",
                                double(actual),double(guard_baseline_actual),double(gameNow),double(request)); debug_trace::Write(bm);
                        }
                    }
                }
            }
            if((!guard_grip || !guard_enabled) && guard_grip_prev){
                if(guard_adjusted && guard_baseline_game>1.0f && !f8_auto_projection_active){
                    (void)ApplyNativeGameFovV6128Test(guard_baseline_game);
                    char bm[224]{}; std::snprintf(bm,sizeof(bm),
                        "V61.32-BOW-ZOOM-GUARD-LEGACY release restoreGame=%.3f",double(guard_baseline_game)); debug_trace::Write(bm);
                }
                guard_adjusted=false; guard_baseline_actual=0.0f; guard_baseline_game=0.0f;
            }
            guard_grip_prev=guard_grip;
        }


// M2_SLOT_END 5019
#else
#error Unknown M2 host contribution slot
#endif
