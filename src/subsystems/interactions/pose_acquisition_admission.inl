    const auto acquisitionNow54=std::uint64_t(GetTickCount64());
    // Save anatomy at the actual Use edge. A later installed Picking observer
    // cannot recover it from ordinary frames already authored during the reach.
    // This pending record never grants pose/input/item authority by itself.
    kcd2vr::grasp13::PendingUse55 pending55{};
    if(g_graspPendingUse55&&g_graspPendingUse55(pending55)&&pending55.serial&&pending55.requestedAt&&
       pending55.character==reinterpret_cast<std::uintptr_t>(character)&&pending55.epoch==tx.epoch&&
       pending55.side>=0&&pending55.side<2&&acquisitionNow54>=pending55.requestedAt&&
       acquisitionNow54-pending55.requestedAt<=1500){
        if(s.pendingUse55.serial!=pending55.serial||s.pendingUse55.character!=pending55.character||
           s.pendingUse55.epoch!=pending55.epoch||s.pendingUse55.requestedAt!=pending55.requestedAt){
            s.pendingUse55=pending55;s.pendingReference55={};
            const auto& reference55=s.ordinaryReference54;
            if(kcd2vr::tracked_frame51::ordinary_reference_d86(reference55)&&
               reference55.stamp.actor==pending55.character&&reference55.stamp.epoch==pending55.epoch&&
               reference55.stamp.tick<=pending55.requestedAt&&pending55.requestedAt-reference55.stamp.tick<=80)
                s.pendingReference55=reference55;
        }
    }else if(s.pendingUse55.character!=reinterpret_cast<std::uintptr_t>(character)||s.pendingUse55.epoch!=tx.epoch||
             acquisitionNow54<s.pendingUse55.requestedAt||acquisitionNow54-s.pendingUse55.requestedAt>1500){
        s.pendingUse55={};s.pendingReference55={};
    }
    const bool acquisitionCurrent54=kcd2vr::grasp13::readFoundationD56(g_graspAcquisition54,tx.acquisition54)&&
        !HandClaimBlockedT24(tx.acquisition54.side,reinterpret_cast<std::uintptr_t>(character),tx.epoch,g_weapon_request_epoch6694.load())&&
        kcd2vr::grasp13::freshAcquisition54(tx.acquisition54,acquisitionNow54)&&tx.acquisition54.active&&
        tx.acquisition54.character==reinterpret_cast<std::uintptr_t>(character)&&tx.acquisition54.epoch==tx.epoch&&
        tx.acquisition54.trackingEpoch55&&tx.acquisition54.trackingEpoch55==ts.source_epoch&&
        tx.acquisition54.trackingEpoch55==kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire)&&
        !ts.head_held&&!ts.right_held&&!ts.left_held&&
        (tx.acquisition54.side?ts.left_valid:ts.right_valid)&&
        (tx.acquisition54.side?ts.left_grip:ts.right_grip)>.25f&&
        !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_reading.load()&&
        WorkstationContextRefusal19c4r2(acquisitionNow54,true)==nullptr&&
        !animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175();
    // A completed ordinary held item has no live Picking action. Its typed
    // visual receipt may keep producing a current body while the generic
    // gameplay cache catches up; it cannot reopen native interaction rights.
    const bool adoptedVisualCurrentR95=!acquisitionCurrent54&&
        (!g_gameplay_allowed.load(std::memory_order_acquire)||g_ctx_scripted.load(std::memory_order_acquire))&&
        action_time_owner59c11::ReadAdoptedAttachmentPresentationR95(tx.adoptedVisualR95)&&
        tx.adoptedVisualR95.binding.character==reinterpret_cast<std::uintptr_t>(character)&&
        tx.adoptedVisualR95.binding.epoch==tx.epoch&&tx.adoptedVisualR95.trackingEpoch==ts.source_epoch&&
        ts.source_epoch==kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire)&&
        !ts.head_held&&(tx.adoptedVisualR95.binding.side?!ts.left_held:!ts.right_held)&&
        (tx.adoptedVisualR95.binding.side?ts.left_valid:ts.right_valid)&&
        (tx.adoptedVisualR95.binding.side?ts.left_grip:ts.right_grip)>.25f&&
        !HandClaimBlockedT24(tx.adoptedVisualR95.binding.side,reinterpret_cast<std::uintptr_t>(character),tx.epoch,g_weapon_request_epoch6694.load())&&
        !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_reading.load()&&
        WorkstationContextRefusal19c4r2(acquisitionNow54,true)==nullptr&&
        !animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175()&&
        !g_kick_busy176.load()&&!diagnostics::lab::NativeReference();
    tx.adoptedVisualRequiredR95=adoptedVisualCurrentR95;
    tx.acquisitionRequired57=acquisitionCurrent54;
    if(!ts.allowed&&!acquisitionCurrent54&&!adoptedVisualCurrentR95&&!tx.instantPairHandsD72){tx.stage="GAMEPLAY_DISALLOWED";return RejectCoherent6693(tx,__LINE__);}
    if(!ts.head_valid){tx.stage="HEAD_TRACKING_INVALID";return RejectCoherent6693(tx,__LINE__);}
    if(!ts.left_valid&&!ts.right_valid){tx.stage="BOTH_HANDS_INVALID";return RejectCoherent6693(tx,__LINE__);}
    if(!ValidateBuffer1Setter(pose,tx.setter)){tx.stage="SETTER_CONTRACT";return RejectCoherent6693(tx,__LINE__);}
