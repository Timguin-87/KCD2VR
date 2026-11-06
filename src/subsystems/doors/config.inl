// M1: exact host config/state contributions at their original callsites.
#if defined(KCD2VR_CONFIG_APPLY_DOORS0)
    (void)state;door_use46::SetEnabled(false); // CL-D: legacy door routes are off for good; hand doors (H40) are always on
#endif
#if defined(KCD2VR_CONFIG_SAVE_DOORS0)
    ok = wb("VrDoorGestures",door_use46::enabled.load()) && ok;
#endif
#if defined(KCD2VR_CONFIG_LOAD_DOORS0)
    door_use46::SetEnabled(false); // CL-D: VrDoorGestures no longer switches anything - hand doors always on
#endif
