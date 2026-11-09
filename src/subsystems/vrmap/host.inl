// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 33
// src/mod.cpp: fillmenustate_01.inl
// M2_SLOT_BEGIN 33
    out.held_map_region32=int(held_map_m1::GetPrintedRegion());
// M2_SLOT_END 33
#elif KCD2VR_M2_HOST_SLOT == 79
// src/mod.cpp: applymenustate_01.inl
// M2_SLOT_BEGIN 79
    held_map_m1::SetPrintedRegion(state.held_map_region32==1?held_map_m1::PrintedRegion::Kuttenberg:held_map_m1::PrintedRegion::Trosky);
// M2_SLOT_END 79
#elif KCD2VR_M2_HOST_SLOT == 273
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 273
    int held_map_region32 = 0;             // native printed map:0 Trosky,1 Kuttenberg
// M2_SLOT_END 273
#elif KCD2VR_M2_HOST_SLOT == 5020
// src/mod.cpp: map_allowed_and_back_stage
// M2_SLOT_BEGIN 5020
        // Same admission gates, with a copied-state refusal mask. No new scene,
        // script, camera or loading-time queries are introduced by this recorder.
        const unsigned mapRefusalH24=(!held_map_m1::Selected()?1u:0u)|(matrix_active?2u:0u)|
            (mis_menu_after_frame?4u:0u)|(dialogue_active?8u:0u)|(theater_presented?16u:0u)|
            (activity163.nativeControls?32u:0u)|(activity_input163::witness175.Active()?64u:0u)|
            (animation_filter::nativeBodyOwned112.load()?128u:0u)|(frik_pose_probe::NativeHeadAimEquipment120R3()==1?256u:0u)|
            (native_ui_snapshot.Open(ui_native_state::Loading)?512u:0u)|(native_ui_snapshot.Open(ui_native_state::Progress)?1024u:0u)|
            (ui89_policy.owner!=ui_menu_router::Owner::None?2048u:0u)|(ui89_policy.uncertain!=0?4096u:0u);
        held_map_m1::PublishAllowed(mapRefusalH24==0,GetTickCount64());
        const bool leftModifierMapH24=kcd2vr::quick_access_r112::LeftThumbrest();
        const float modifierMapXH24=leftModifierMapH24?gpR.thumb_x:gpL.thumb_x;
        const float modifierMapYH24=leftModifierMapH24?gpR.thumb_y:gpL.thumb_y;
        const bool rawMapChordH24=gpR.quick_access_touch&&modifierMapYH24>=.62f&&std::fabs(modifierMapXH24)<=modifierMapYH24;
        static bool rawMapPreviousH24=false;static unsigned rawMapReceiptsH24=0;
        if(rawMapChordH24!=rawMapPreviousH24&&rawMapReceiptsH24<32){++rawMapReceiptsH24;char m[256]{};
            std::snprintf(m,sizeof(m),"H24 MAP_INPUT edge=%s selected=%d requested=%d refusalMask=%u blocksGameplay=%d activeL=%d activeR=%d",
                rawMapChordH24?"BEGIN":"END",held_map_m1::Selected(),held_map_m1::Requested(),mapRefusalH24,
                ui88_input_result.blocks_gameplay,gpL.active,gpR.active);diagnostic_trace::Write(m);}
        rawMapPreviousH24=rawMapChordH24;
        native_map_open_h25::ObserveChord(rawMapChordH24);
        native_map_open_h25::PublishUi(ladder_now_ms,
            native_ui_snapshot.Known(ui_native_state::Inventory)&&native_ui_snapshot.Open(ui_native_state::Inventory),
            ui89_policy.owner==ui_menu_router::Owner::InventoryMap&&ui89_policy.uncertain==0&&
            !native_ui_snapshot.Open(ui_native_state::Modal)&&!native_ui_snapshot.Open(ui_native_state::Shop)&&
            !native_ui_snapshot.Open(ui_native_state::ItemTransfer)&&!native_ui_snapshot.Open(ui_native_state::ItemSelection),
            !held_map_m1::Selected()||matrix_active||mis_menu_after_frame||dialogue_active||
            native_ui_snapshot.Open(ui_native_state::Loading)||native_ui_snapshot.Open(ui_native_state::Progress)||
            native_ui_snapshot.Open(ui_native_state::GameOver)||gpR.b_down||gpR.a_down||gpL.menu_down||
            gpL.x_down||gpL.y_down||gpR.thumbstick_down||gpL.thumbstick_down||gpR.trigger>.25f||gpL.trigger>.25f);

        const auto readingH28=activity_input163::witness175.active[unsigned(activity_input163::Kind175::Reading)].load(std::memory_order_acquire);
        held_map_feed_t2::NativeBookTick(ladder_now_ms,readingH28,
            activity_input163::witness175.lifetime175.load(std::memory_order_acquire),
            held_map_m1::Selected()&&activity_input163::witness175.ready.load(std::memory_order_acquire)&&
            !activity_input163::witness175.transitionDepth175.load(std::memory_order_acquire)&&
            !matrix_active&&!mis_menu_after_frame&&!dialogue_active&&ui89_policy.uncertain==0&&
            (ui89_policy.owner==ui_menu_router::Owner::NativeContext||ui89_policy.owner==ui_menu_router::Owner::None)&&
            !native_ui_snapshot.Open(ui_native_state::Loading)&&!native_ui_snapshot.Open(ui_native_state::Progress));

        if(held_map_m1::Selected()){
            static std::uint64_t lastMapLog=0;if(ladder_now_ms-lastMapLog>=5000){lastMapLog=ladder_now_ms;const auto c=held_map_m1::Snapshot();
                char m[304]{};std::snprintf(m,sizeof(m),"C7FM1 HELD_MAP requested=%d placed=%llu shows=%llu hides=%llu missing=%llu stale=%llu poseRefusal=%u lastHideM4=%u refusalMask=%u",held_map_m1::Requested(),
                    (unsigned long long)c.placements,(unsigned long long)c.shows,(unsigned long long)c.hides,(unsigned long long)c.missing,(unsigned long long)c.stale,c.poseRefusal,c.lastHideReasonM4,mapRefusalH24);diagnostic_trace::Write(m);}}
// M2_SLOT_END 5020
#elif KCD2VR_M2_HOST_SLOT == 5021
// src/mod.cpp: map_open_loop_stage
// M2_SLOT_BEGIN 5021
        if(touch_events.toggle_map_comfort) {
            // D44: an explicitly disabled letter may use the ordinary map UI.
            // The retired cropped-XR/reading-animation experiment never runs.
            physical_map_active=!physical_map_active;
            if(physical_map_active){
                ui88_router.Request(CleanMenuOwner::InventoryMap);clean_menu_owner=ui88_router.Current();map_flat_ui=true;inventory_flat_ui=false;pause_flat_ui=false;
                if(xr_manager_) xr_manager_->SetPhysicalMapMode(false);
                physical_map_open_ms=ladder_now_ms; physical_map_loop_requested=false;
                physical_map_read_start_base=animation_filter::PhysicalMapReadingAnimationStarts();
                diagnostic_trace::Write("D44 MAP_NATIVE_MENU open=1 heldLetter=DISABLED readingAnimation=NONE");
            } else {
                if(xr_manager_) xr_manager_->SetPhysicalMapMode(false);
                physical_map_open_ms=0; physical_map_loop_requested=false;
                inventory_close_pending=true;map_flat_ui=false;inventory_flat_ui=false;pause_flat_ui=false;
                diagnostic_trace::Write("D44 MAP_NATIVE_MENU open=0 readingAnimation=NONE");
            }
        }

// M2_SLOT_END 5021
#elif KCD2VR_M2_HOST_SLOT == 6001
// src/mod.cpp: map_back_filter
// M2_SLOT_BEGIN 6001
        // M35: retain the raw LEFT direction until TouchBindings owns the hold.
        // Native-XI consumes its separate pad copy; fallback consumes the
        // quick-access modifier inside TouchBindings. Clearing routeL here
        // prevented the actual map recognizer from ever observing Up.
        static held_map_m1::BackEdge mapBack;
        if(mapBack.Filter(gpR.b_down,held_map_m1::Requested()&&held_map_m1::Allowed(GetTickCount64()),GetTickCount64())){routeR.b_down=false;routeR.b_pressed=false;}
// M2_SLOT_END 6001
#else
#error Unknown M2 host contribution slot
#endif
