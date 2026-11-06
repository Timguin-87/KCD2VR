// M1: exact host config/state contributions at their original callsites.
#if defined(KCD2VR_CONFIG_LOAD_HYBRID0)
    combat_diag_probe::SetHybridTargetWait58(ReadIniBoolExplicit(explicit_ini,"Settings","VrHybridTargetWait58",true));
#endif
