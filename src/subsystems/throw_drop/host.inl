// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 29
// src/mod.cpp: fillmenustate_01.inl
// M2_SLOT_BEGIN 29
    out.weapon_release_mode_t1 = kcd2vr::weapon_release_t1::Mode();
    out.throwing_supply_t2 = action_time_owner59c11::ThrowingSupplyT2();
// M2_SLOT_END 29
#elif KCD2VR_M2_HOST_SLOT == 76
// src/mod.cpp: applymenustate_01.inl
// M2_SLOT_BEGIN 76
    kcd2vr::weapon_release_t1::SetMode(state.weapon_release_mode_t1);
    if(state.throwing_supply_t2&&!action_time_owner59c11::ThrowingSupplyT2())kcd2vr::inventory_lifecycle_t8::Recheck();
    action_time_owner59c11::SetThrowingSupplyT2(state.throwing_supply_t2);
// M2_SLOT_END 76
#elif KCD2VR_M2_HOST_SLOT == 272
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 272
    int weapon_release_mode_t1 = 0;         // 0 automatic holster, 1 qualified native drop/throw
    bool throwing_supply_t2 = true;         // T26 LEFT knife pouch (hip / chest); independent of right release mode. R109: default ON
// M2_SLOT_END 272
#else
#error Unknown M2 host contribution slot
#endif
