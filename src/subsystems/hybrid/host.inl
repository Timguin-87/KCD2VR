// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 157
// src/mod.cpp: loadconfig_01.inl
// M2_SLOT_BEGIN 157
    g_hybrid_default59.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrHybridDefault59",true));
// M2_SLOT_END 157
#elif KCD2VR_M2_HOST_SLOT == 286
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 286
    bool exp_attack_require_opponent = true; // VrAttackRequireOpponent (R59.16C10R4F)
    float exp_attack_reach_m = 2.6f;        // VrAttackReachMeters
// M2_SLOT_END 286
#elif KCD2VR_M2_HOST_SLOT == 294
// src/vr_menu.h: menu_state_02.inl
// M2_SLOT_BEGIN 294
    bool  exp_instant_free_swing38 = true;  // C38 VrInstantFreeSwing
// M2_SLOT_END 294
#elif KCD2VR_M2_HOST_SLOT == 301
// src/vr_menu.h: menu_state_03.inl
// M2_SLOT_BEGIN 301
    bool  exp_intent_live = true;           // VrIntentLiveRoute
    float exp_native_hit_time = 0.30f;      // VrNativeHitTime (s, < 0.10 = native timing)
// M2_SLOT_END 301
#elif KCD2VR_M2_HOST_SLOT == 303
// src/vr_menu.h: menu_state_04.inl
// M2_SLOT_BEGIN 303
    int   exp_intent_live_ms = 60;          // VrIntentLiveWaitMs
// M2_SLOT_END 303
#else
#error Unknown M2 host contribution slot
#endif
