// M1: exact host config/state contributions at their original callsites.
#if defined(KCD2VR_CONFIG_APPLY_RANGED_RELOAD0)
    frik_pose_probe::SetStagedBowDefault59(state.bow_manual_reload);
#endif
#if defined(KCD2VR_CONFIG_LOAD_RANGED_RELOAD)
    ranged_options_d66c::crankTravel.store(ranged_options_d66c::crankTravelSetting(ReadIniFloatExplicit(explicit_ini,"Settings","VrCrankTravelD66C",.08f)));
    ranged_options_d65c::SetBowPhysicalAim(ReadIniBoolExplicit(explicit_ini,"Settings","VrBowPhysicalAimD65C",true));
    ranged_options_d65c::SetCrossbowReload(ReadIniBoolExplicit(explicit_ini,"Settings","VrCrossbowReloadD65C",true));
    ranged_options_xr32::crossbowNoZoom.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrCrossbowNoZoom",true),std::memory_order_release);
    frik_pose_probe::SetStagedBowDefault59(ReadIniBoolExplicit(explicit_ini,"Settings","VrManualBowDefault59",true));
    diagnostic_trace::Write(frik_pose_probe::StagedBowDefault59()?
        "BOW_RELOAD_POLICY savedSection=Settings manualQuiver=ON qualification=CALIBRATION_AND_NATIVE_HOOKS_REQUIRED F11=INDEPENDENT":
        "BOW_RELOAD_POLICY savedSection=Settings manualQuiver=OFF loading=NATIVE_AUTOMATIC F11_RUNG2=EXPLICIT_OVERRIDE");
#endif
