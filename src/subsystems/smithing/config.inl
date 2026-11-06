// M1: exact host config/state contributions at their original callsites.
#if defined(KCD2VR_CONFIG_SAVE_SMITHING0)
    ok = wb("VrSmithingAutoHold",workstation178r20c1::smithAutoHold36.load(std::memory_order_acquire)) && ok; // C37: menu-owned
#endif
#if defined(KCD2VR_CONFIG_LOAD_SMITHING0)
    workstation178r20c1::setSmithAutoHold36(ReadIniBoolExplicit(explicit_ini,"Settings","VrSmithingAutoHold",true)); // C37
#endif
