// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 10
// src/mod.cpp: fillmenustate_01.inl
// M2_SLOT_BEGIN 10
    out.door_gestures46=door_use46::enabled.load(std::memory_order_acquire);
// M2_SLOT_END 10
#elif KCD2VR_M2_HOST_SLOT == 269
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 269
    bool door_gestures46=true; // VrDoorGestures: a squeeze owns one native door transaction.
// M2_SLOT_END 269
#else
#error Unknown M2 host contribution slot
#endif
