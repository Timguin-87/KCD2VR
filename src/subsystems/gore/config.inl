// M1: exact host config/state contributions at their original callsites.
#if defined(KCD2VR_CONFIG_SAVE_GORE0)
    ok = wb("VrBloodyFights", frik_pose_probe::BloodyFights49()) && ok; // C49: menu-owned
#endif
#if defined(KCD2VR_CONFIG_LOAD_GORE0)
    frik_pose_probe::SetBloodyFights49(ReadIniBoolExplicit(explicit_ini,"Settings","VrBloodyFights",true)); // C49 BLOODY FIGHTS (default ON, Miki)
#endif
