// M1: exact host config/state contributions at their original callsites.
#if defined(KCD2VR_CONFIG_SAVE_THROW_DROP0)
    ok = wi("VrWeaponReleaseMode", kcd2vr::weapon_release_t1::Mode()) && ok;
#endif
#if defined(KCD2VR_CONFIG_SAVE_THROW_DROP1)
    ok = wb("VrLeftChestKnives", action_time_owner59c11::ThrowingSupplyT2()) && ok;
#endif
#if defined(KCD2VR_CONFIG_LOAD_THROW_DROP0)
    kcd2vr::weapon_release_t1::SetMode(ReadIniIntExplicit(explicit_ini,"Settings","VrWeaponReleaseMode",0));
#endif
#if defined(KCD2VR_CONFIG_LOAD_THROW_DROP1)
    action_time_owner59c11::SetThrowingSupplyT2(ReadIniBoolExplicit(explicit_ini,"Settings","VrLeftChestKnives",false)); // R124: default OFF (Miki's INI, 6 Nov); was ON since R109
#endif
