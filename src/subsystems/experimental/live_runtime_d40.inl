// D40 shared live owner wiring. Included at the four Mod ownership boundaries;
// the portable fixture executes these same contributions with explicit IO leaves.
#if defined(KCD2VR_D40_LIVE_READ)
    {
        auto& d40=out.live_d39;
        d40.instantActivitiesD72=instant_d72::activities.load();
        d40.instantInteractionsD72=instant_d72::interactions.load();
        d40.instantCraftingD72=instant_d72::crafting.load();
        d40.instantReloadsD72=instant_d72::reloads.load();
        d40.instantCombatD72=instant_d72::combat.load();
        for(unsigned i=0;i<4;++i)d40.instantStateD72[i]=unsigned(instant_d72::Current(instant_d72::Family(i)));
        d40.contactContinuityD71=contact_handover_d71::ContactContinuity();
        d40.itemHandoverD71=contact_handover_d71::ItemHandover();
        d40.stabFixD77=claude_d70::StabFix();
        d40.unarmedFixD77=claude_d70::UnarmedFix();
        d40.masterstrikeFixD77=claude_d70::MasterstrikeFix();
        d40.finisherUpperPlayerD83=finisher_upper_d83::player.load(std::memory_order_acquire); // D83 finisher
        d40.finisherUpperNpcD83=finisher_upper_d83::npc.load(std::memory_order_acquire);
        d40.bladeZoneD84=blade_zone_d84::enabled.load(std::memory_order_acquire); // D84 blade zone
        d40.upCutLowD84=blade_zone_d84::upCutLow.load(std::memory_order_acquire);
        d40.stabStartD84=blade_zone_d84::stabStart.load(std::memory_order_acquire);
        d40.diceHandsD85=dice_hands_d85::enabled.load(std::memory_order_acquire); // D85 dice
        d40.staticGrabR89=static_grab_r89::enabled.load(std::memory_order_acquire); // R90 (R89 pickup)
        d40.diceHandsStatusD85=dice_hands_d85::status.load(std::memory_order_acquire);
        d40.crossbowHandOnlyR90=ranged_options_d65c::crossbowHandOnlyR90.load(std::memory_order_acquire); // R90
        d40.heavyCrossbowHandPullR111=ranged_options_d65c::heavyCrossbowHandPullR111.load(std::memory_order_acquire); // R111
        d40.mediumCrossbowHandPullR117=ranged_options_d65c::mediumCrossbowHandPullR117.load(std::memory_order_acquire); // R117
        d40.controllerLayoutR118=kcd2vr::controller_layout_r118::Get(); // R118
        d40.quickSlotFadeR112=kcd2vr::ui_hud_r112::quickSlotFade.load(std::memory_order_acquire); // R112
        d40.alchemyViewSelectR116=kcd2vr::subsystems::alchemy::head_select_r116::enabled.load(std::memory_order_acquire); // R116
        d40.swingWhooshR116=kcd2vr::swing_whoosh_r116::enabled.load(std::memory_order_acquire); // R116
        d40.corpseSqueezeSearchR90=lab58::corpseSqueezeSearchR90.load(std::memory_order_acquire); // R90
        d40.twoHandWeightR90=kcd2vr::two_hand_weight_r90::enabled.load(std::memory_order_acquire); // R90
        d40.weaponKickR96=kcd2vr::bridges168::weaponKickR96.load(std::memory_order_acquire); // R96
        d40.sceneryPropSwapR100=kcd2vr::brush_swap_r100::enabled.load(std::memory_order_acquire); // R100
        d40.propFirmHoldR116=kcd2vr::prop5916c1::firmHoldR116.load(std::memory_order_acquire); // R116
        d40.shieldBlockBoxR120=kcd2vr::shield_block_r120::enabled.load(std::memory_order_acquire); // R120
        d40.hideBackWeaponsR121=kcd2vr::stowed_hide_hw::hideBackSetting.load(std::memory_order_acquire); // R121
        d40.hideAtStationsR121=kcd2vr::stowed_hide_hw::hideAtStationsSetting.load(std::memory_order_acquire); // R121
        d40.eyeRayR121=kcd2vr::eye_ray_r121::enabled.load(std::memory_order_acquire); // R121
        d40.meleeGripR123=kcd2vr::melee_grip_r123::enabled.load(std::memory_order_acquire); // R123
        d40.propImpactSoundR124=kcd2vr::prop_impact_sound_r124::enabled.load(std::memory_order_acquire); // R124
        d40.impaleFastDeathR106=kcd2vr::impale_death_speed_r106::enabled.load(std::memory_order_acquire); // R106
        d40.cinematicBarsR107=kcd2vr::cinematic_bars_r107::enabled.load(std::memory_order_acquire); // R107
        d40.dodgeWindowD70=dodge_window_d70::Factor();
        d40.hudWalkoffD70=claude_d70::HudWalkoff();
        d40.nativeRoomscaleD76=roomscale_d76::Enabled();
        d40.ridingVrikD76=riding_vrik_d76::Get();
        d40.npcListeningD76=lively_npc_d73::ListeningGestures();
        d40.leanHeightD70=roomscale58::LeanHeightD70();
        d40.handLeadD69=claude_motion_d69::HandLead();
        d40.handLeadMsD69=claude_motion_d69::HandLeadMs();
        d40.elbowHysteresisD69=claude_motion_d69::ElbowHysteresis();
        d40.weaponSameFrameD69=weapon_trials_d69::sameFrame();
        d40.rigidGripD69=weapon_trials_d69::rigidGrip();
        d40.gripAnchorD67=held_motion_d67::gripAnchored();
        d40.arcCoverageD67=held_motion_d67::currentArcCoverage();
        d40.supportReleaseD67=held_motion_d67::supportRelease();
        d40.contactFairnessD67=held_motion_d67::contactFairness();
        d40.elbowContinuityD67=motion_trials_d67::ContinuityRepair();
        d40.elbowOutwardD67=motion_trials_d67::ElbowOutward();
        d40.elbowBackD67=motion_trials_d67::ElbowBack();
        d40.smithRecoveryD67=crafting_d67::smithRecovery.load();
        d40.smithSemiD67=crafting_d67::smithSemiAnchors.load();
        d40.motionTrialD67=int(motion_trials_d67::MenuMode());
        d40.elbowPreferenceR92=motion_trials_d67::ElbowPreferenceR92();

        out.native_kill_nf=native_kill_nf::enabled.load(std::memory_order_acquire);
        d40.frikElbowsD48=frik_elbow_lab::enabled.load(std::memory_order_acquire);
        d40.elbowSolverR89=elbow_r89::enabled.load(std::memory_order_acquire); // R90
        d40.elbowF2OverrideD48=smooth_lab::ElbowShoulderSelectedD45();
        d40.timingLocationD64=int(smooth_lab::TimingLocationD64());
        d40.mapSizeD64=held_map_m1::paperSizeD64.load();
        d40.mapSupportD64=held_map_m1::twoHandD64.load();
        d40.mapNativePaperM43=held_map_m1::NativePaperSelectedM43();
        d40.twoHandBiasD65=two_hand_d65::enabled.load();
        d40.knifeRightHipD66=knife_pouch_d66::rightHipSetting();
        d40.knifeChestZoneR109=knife_pouch_d66::chestZoneSetting(); // R109
        d40.eyeSrgbR114=kcd2vr::xr_eye_srgb_r114::eyeSrgb.load(std::memory_order_acquire); // R114
        d40.vrikOnStartR111=start_r111::vrikOnStart.load(std::memory_order_acquire); // R111
        d40.startHenryEyesR111=start_r111::henryEyes.load(std::memory_order_acquire); // R111
        d40.bowAimD65=ranged_options_d65c::bowPhysicalAim.load();
        d40.crossbowReloadD65=ranged_options_d65c::crossbowReload.load();
        d40.crankTravelD66=ranged_options_d66c::crankTravelSetting(ranged_options_d66c::crankTravel.load());
        d40.alchemySpeedD65=action_time_owner59c11::AlchemyTransferSpeedD65();
        d40.pickupClockD65=action_time_owner59c11::UnownedPickupClockD65();
        d40.twoHandPrimaryBiasD65=two_hand_d65::setting(two_hand_d65::primaryBias.load());
        d40.arrowNativeTurnD48=arrow_hold_d48::nativeTurn.load(std::memory_order_acquire);
        d40.fistStrokeD49=fist_stroke_d49::enabled.load(std::memory_order_acquire);
        d40.pickupCameraBridgeD55=camera_hold_e1::pickupBridgeD55.load(std::memory_order_acquire);
        d40.alchemyToolContactD55=alchemy_tool_contact_d55::enabled.load(std::memory_order_acquire);
        d40.npcLifeD73=lively_npc_d73::Enabled();
        d40.npcGazeHmdD52=npc_look_policy63::followHmd();
        d40.npcGazeReadyD52=npc_look_policy63::hookReadyD52.load(std::memory_order_acquire);
        d40.npcGazeSceneD52=npc_look_policy63::sceneAdmittedD52.load(std::memory_order_acquire)&&dialogue_state59c10r2::age(npc_look_policy63::sceneTickD52.load(std::memory_order_acquire),GetTickCount64(),80);
        d40.pickupNearOnlyD40=pickup_use19c3::nearOnlyD40.load(std::memory_order_acquire);
        d40.combatNoStepD40=frik_pose_probe::CombatNoStep55();
        d40.combatNoTurnD40=frik_pose_probe::CombatNoTurnD40();
        d40.freeTargetingD42=free_targeting_d42::enabled.load(std::memory_order_acquire);
        d40.freeTargetingContextD42=free_targeting_d42::contextReady.load(std::memory_order_acquire);
        d40.freeTargetingLeaseD42=free_targeting_d42::cameraLeased.load(std::memory_order_acquire);
        d40.combatNoTurnReadyD40=frik_pose_probe::CombatNoTurnReadyD40();
        d40.clinchModeD40=frik_pose_probe::ClinchModeD40();
        d40.clinchReadyD40=frik_pose_probe::ClinchGateReadyD40();
        d40.labLivingTouchD40=lab58::livingTouch.load(std::memory_order_acquire);
        d40.labLivingImpactD40=lab58::livingHitImpact.load(std::memory_order_acquire);
        d40.labCorpseDragD40=lab58::corpseDrag.load(std::memory_order_acquire);
        d40.npcContactReactionD41=bodycombo59c7g::npcReactsToBlade.load(std::memory_order_acquire);
        d40.npcStaminaReactionD41=bodycombo59c7g::staminaHits.load(std::memory_order_acquire);
        d40.finisherCandidateD41=finisher_cb1::enabled.load(std::memory_order_acquire);
        const auto nowD40=std::uint64_t(GetTickCount64());
        for(int sideD40=0;sideD40<2;++sideD40){
            physics::grab_producer60::Status actualD40{};
            const bool readD40=physics::grab_producer60::ReadStatus(sideD40,actualD40);
            d40.propStatusKnownD40[sideD40]=readD40&&actualD40.tick&&nowD40>=actualD40.tick&&nowD40-actualD40.tick<=500;
            d40.propStateD40[sideD40]=readD40?unsigned(actualD40.state):0;
        }
    }
#elif defined(KCD2VR_D40_LIVE_APPLY)
    {
        const auto& d40=state.live_d39;
        instant_d72::activities.store(d40.instantActivitiesD72);
        instant_d72::interactions.store(d40.instantInteractionsD72);
        instant_d72::crafting.store(d40.instantCraftingD72);
        instant_d72::reloads.store(d40.instantReloadsD72);
        instant_d72::combat.store(d40.instantCombatD72);
        contact_handover_d71::SetContactContinuity(d40.contactContinuityD71);
        contact_handover_d71::SetItemHandover(d40.itemHandoverD71);
        claude_d70::SetD77(d40.stabFixD77,d40.unarmedFixD77,d40.masterstrikeFixD77);
        finisher_upper_d83::player.store(d40.finisherUpperPlayerD83,std::memory_order_release); // D83 finisher
        finisher_upper_d83::npc.store(d40.finisherUpperNpcD83,std::memory_order_release);
        blade_zone_d84::enabled.store(d40.bladeZoneD84,std::memory_order_release); // D84 blade zone
        blade_zone_d84::upCutLow.store(d40.upCutLowD84,std::memory_order_release);
        blade_zone_d84::stabStart.store(d40.stabStartD84,std::memory_order_release);
        dice_hands_d85::enabled.store(d40.diceHandsD85,std::memory_order_release); // D85 dice
        static_grab_r89::enabled.store(d40.staticGrabR89,std::memory_order_release); // R90 (R89 pickup)
        ranged_options_d65c::crossbowHandOnlyR90.store(d40.crossbowHandOnlyR90,std::memory_order_release); // R96 route switch
        ranged_options_d65c::heavyCrossbowHandPullR111.store(d40.heavyCrossbowHandPullR111,std::memory_order_release); // R111
        ranged_options_d65c::mediumCrossbowHandPullR117.store(d40.mediumCrossbowHandPullR117,std::memory_order_release); // R117
        kcd2vr::controller_layout_r118::Set(d40.controllerLayoutR118); // R118: live, read on the next X press
        kcd2vr::ui_hud_r112::quickSlotFade.store(d40.quickSlotFadeR112,std::memory_order_release); // R112
        kcd2vr::subsystems::alchemy::head_select_r116::enabled.store(d40.alchemyViewSelectR116,std::memory_order_release); // R116: live
        kcd2vr::swing_whoosh_r116::enabled.store(d40.swingWhooshR116,std::memory_order_release); // R116: live, next swing
        lab58::corpseSqueezeSearchR90.store(d40.corpseSqueezeSearchR90,std::memory_order_release); // R90
        kcd2vr::two_hand_weight_r90::enabled.store(d40.twoHandWeightR90,std::memory_order_release); // R90
        kcd2vr::bridges168::weaponKickR96.store(d40.weaponKickR96,std::memory_order_release); // R96
        kcd2vr::brush_swap_r100::enabled.store(d40.sceneryPropSwapR100,std::memory_order_release); // R100
        kcd2vr::prop5916c1::firmHoldR116.store(d40.propFirmHoldR116,std::memory_order_release); // R116
        kcd2vr::shield_block_r120::enabled.store(d40.shieldBlockBoxR120,std::memory_order_release); // R120: live, next frame
        kcd2vr::stowed_hide_hw::hideBackSetting.store(d40.hideBackWeaponsR121,std::memory_order_release); // R121: live, next stowed tick
        kcd2vr::stowed_hide_hw::hideAtStationsSetting.store(d40.hideAtStationsR121,std::memory_order_release); // R121: live, next stowed tick
        kcd2vr::eye_ray_r121::enabled.store(d40.eyeRayR121,std::memory_order_release); // R121: live, next look ray
        kcd2vr::melee_grip_r123::enabled.store(d40.meleeGripR123,std::memory_order_release); // R123: live, next grip sample
        kcd2vr::prop_impact_sound_r124::enabled.store(d40.propImpactSoundR124,std::memory_order_release); // R124: live, next impact
        kcd2vr::impale_death_speed_r106::enabled.store(d40.impaleFastDeathR106,std::memory_order_release); // R106
        kcd2vr::cinematic_bars_r107::enabled.store(d40.cinematicBarsR107,std::memory_order_release); // R107
        dodge_window_d70::SetFactor(d40.dodgeWindowD70);
        claude_d70::SetHudWalkoff(d40.hudWalkoffD70);
        roomscale_d76::SetEnabled(d40.nativeRoomscaleD76);
        riding_vrik_d76::Set(d40.ridingVrikD76);
        lively_npc_d73::SetListeningGestures(d40.npcListeningD76);
        roomscale58::SetLeanHeightD70(d40.leanHeightD70);
        claude_motion_d69::SetHandLeadMs(d40.handLeadMsD69);
        claude_motion_d69::SetHandLead(d40.handLeadD69);
        claude_motion_d69::SetElbowHysteresis(d40.elbowHysteresisD69);
        weapon_trials_d69::setSameFrame(d40.weaponSameFrameD69);
        weapon_trials_d69::setRigidGrip(d40.rigidGripD69);
        held_motion_d67::setGripAnchored(d40.gripAnchorD67);
        held_motion_d67::setCurrentArcCoverage(d40.arcCoverageD67);
        held_motion_d67::setSupportRelease(d40.supportReleaseD67);
        held_motion_d67::setContactFairness(d40.contactFairnessD67);
        motion_trials_d67::SetContinuityRepair(d40.elbowContinuityD67);
        motion_trials_d67::SetElbowOutward(d40.elbowOutwardD67);
        motion_trials_d67::SetElbowBack(d40.elbowBackD67);
        crafting_d67::setSmithRecovery(d40.smithRecoveryD67);
        crafting_d67::setSmithSemiAnchors(d40.smithSemiD67);
        motion_trials_d67::SetMenuMode(unsigned(d40.motionTrialD67));
        motion_trials_d67::SetElbowPreferenceR92(d40.elbowPreferenceR92);

        smooth_lab::SetTimingLocationD64(unsigned(d40.timingLocationD64));
        held_map_m1::paperSizeD64.store(std::clamp(d40.mapSizeD64,.40f,1.5f));
        held_map_m1::twoHandD64.store(d40.mapSupportD64);
        held_map_m1::SetNativePaperM43(d40.mapNativePaperM43);
        two_hand_d65::primaryBias.store(two_hand_d65::setting(d40.twoHandPrimaryBiasD65));
        two_hand_d65::enabled.store(d40.twoHandBiasD65);
        knife_pouch_d66::setRightHip(d40.knifeRightHipD66);
        knife_pouch_d66::setChestZone(d40.knifeChestZoneR109); // R109
        kcd2vr::xr_eye_srgb_r114::eyeSrgb.store(d40.eyeSrgbR114,std::memory_order_release); // R114: next session
        start_r111::vrikOnStart.store(d40.vrikOnStartR111,std::memory_order_release); // R111
        start_r111::henryEyes.store(d40.startHenryEyesR111,std::memory_order_release); // R111: the next recenter/load uses it
        ranged_options_d65c::SetBowPhysicalAim(d40.bowAimD65);
        ranged_options_d65c::SetCrossbowReload(d40.crossbowReloadD65);
        (void)frik_pose_probe::PrepareGameplayResourcesD84();
        ranged_options_d66c::crankTravel.store(ranged_options_d66c::crankTravelSetting(d40.crankTravelD66));
        action_time_owner59c11::SetAlchemyTransferSpeedD65(d40.alchemySpeedD65);
        action_time_owner59c11::SetUnownedPickupClockD65(d40.pickupClockD65);
        native_kill_nf::SetEnabled(state.native_kill_nf);
        frik_elbow_lab::enabled.store(d40.frikElbowsD48,std::memory_order_release);
        elbow_r89::enabled.store(d40.elbowSolverR89,std::memory_order_release); // R90
        arrow_hold_d48::nativeTurn.store(d40.arrowNativeTurnD48,std::memory_order_release);
        fist_stroke_d49::enabled.store(d40.fistStrokeD49,std::memory_order_release);
        camera_hold_e1::pickupBridgeD55.store(d40.pickupCameraBridgeD55,std::memory_order_release);
        alchemy_tool_contact_d55::enabled.store(d40.alchemyToolContactD55,std::memory_order_release);
        if(d40.npcLifeD73!=lively_npc_d73::Enabled())
            lively_npc_d73::SetEnabled(d40.npcLifeD73&&lively_npc_d73::Prepare());
        npc_look_policy63::setFollowHmd(d40.npcGazeHmdD52);
        pickup_use19c3::setNearOnlyD40(d40.pickupNearOnlyD40);
        frik_pose_probe::SetCombatNoStep55(d40.combatNoStepD40);
        frik_pose_probe::SetCombatNoTurnD40(d40.combatNoTurnD40);
        free_targeting_d42::set(d40.freeTargetingD42);
        frik_pose_probe::SetClinchModeD40(d40.clinchModeD40);
        bodycombo59c7g::npcReactsToBlade.store(d40.npcContactReactionD41,std::memory_order_release);
        bodycombo59c7g::staminaHits.store(d40.npcStaminaReactionD41,std::memory_order_release);
        finisher_cb1::enabled.store(d40.finisherCandidateD41,std::memory_order_release);
        if(lab58::livingTouch.load()!=d40.labLivingTouchD40)
            frik_pose_probe::SetLab58LivingTouch(d40.labLivingTouchD40);
        if(lab58::livingHitImpact.load()!=d40.labLivingImpactD40||lab58::corpseDrag.load()!=d40.labCorpseDragD40||
           lab58::propWake.load()!=d40.labPropWake)
            frik_pose_probe::SetLab58(d40.labLivingImpactD40,d40.labCorpseDragD40,d40.labPropWake,lab58::livingHitK.load());
    }
#elif defined(KCD2VR_D40_LIVE_SAVE)
    ok=wb("VrInstantActivitiesD72",instant_d72::activities.load())&&ok;
    ok=wb("VrInstantInteractionsD72",instant_d72::interactions.load())&&ok;
    ok=wb("VrInstantCraftingD72",instant_d72::crafting.load())&&ok;
    ok=wb("VrInstantReloadsD72",instant_d72::reloads.load())&&ok;
    ok=wb("VrInstantCombatD72",instant_d72::combat.load())&&ok;
    ok=wb("VrContactContinuityD71",contact_handover_d71::ContactContinuity())&&ok;
    ok=wb("VrItemHandoverD71",contact_handover_d71::ItemHandover())&&ok;
    ok=wf("VrDodgeWindowScaleD70",dodge_window_d70::Factor())&&ok;
    ok=claude_d70::SaveD77(wb)&&ok;
    ok=wb("VrFinisherUpperBodyPlayerD83",finisher_upper_d83::player.load(std::memory_order_acquire))&&ok; // D83 finisher
    ok=wb("VrFinisherUpperBodyNpcD83",finisher_upper_d83::npc.load(std::memory_order_acquire))&&ok;
    ok=wb("VrBladeZoneD84",blade_zone_d84::enabled.load(std::memory_order_acquire))&&ok; // D84 blade zone
    ok=wb("VrUpCutLowD84",blade_zone_d84::upCutLow.load(std::memory_order_acquire))&&ok;
    ok=wb("VrStabStartZoneD84",blade_zone_d84::stabStart.load(std::memory_order_acquire))&&ok;
    ok=wb("VrDiceHandsD85",dice_hands_d85::enabled.load(std::memory_order_acquire))&&ok; // D85 dice
    ok=wb("VrStaticPropGrabR89",static_grab_r89::enabled.load(std::memory_order_acquire))&&ok; // R90 (R89 pickup)
    ok=wb("VrCrossbowReloadRouteR96",ranged_options_d65c::crossbowHandOnlyR90.load(std::memory_order_acquire))&&ok; // R96 (true = HAND ONLY)
    ok=wb("VrHeavyCrossbowHandPullR111",ranged_options_d65c::heavyCrossbowHandPullR111.load(std::memory_order_acquire))&&ok; // R111
    ok=wb("VrMediumCrossbowHandPullR117",ranged_options_d65c::mediumCrossbowHandPullR117.load(std::memory_order_acquire))&&ok; // R117
    ok=wi("VrControllerLayoutR118",kcd2vr::controller_layout_r118::Get())&&ok; // R118: 0 OPENXR, 1 STEAMVR, 2 PS VR2
    ok=wb("VrHudQuickSlotFadeR112",kcd2vr::ui_hud_r112::quickSlotFade.load(std::memory_order_acquire))&&ok; // R112
    ok=wb("VrAlchemyViewSelectR116",kcd2vr::subsystems::alchemy::head_select_r116::enabled.load(std::memory_order_acquire))&&ok; // R116
    ok=wb("VrSwingWhooshR116",kcd2vr::swing_whoosh_r116::enabled.load(std::memory_order_acquire))&&ok; // R116
    ok=wb("VrCorpseSearchSqueezeR89",lab58::corpseSqueezeSearchR90.load(std::memory_order_acquire))&&ok; // R90
    ok=wb("VrTwoHandWeightFeelR90",kcd2vr::two_hand_weight_r90::enabled.load(std::memory_order_acquire))&&ok; // R90
    ok=wb("VrWeaponKickR96",kcd2vr::bridges168::weaponKickR96.load(std::memory_order_acquire))&&ok; // R96
    ok=wb("VrSceneryPropSwapR100",kcd2vr::brush_swap_r100::enabled.load(std::memory_order_acquire))&&ok; // R100
    ok=wb("VrPropFirmHoldR116",kcd2vr::prop5916c1::firmHoldR116.load(std::memory_order_acquire))&&ok; // R116
    ok=wb(kcd2vr::shield_block_r120::kEnabledKey,kcd2vr::shield_block_r120::enabled.load(std::memory_order_acquire))&&ok; // R120
    ok=wb(kcd2vr::stowed_hide_hw::kHideBackKey,kcd2vr::stowed_hide_hw::hideBackSetting.load(std::memory_order_acquire))&&ok; // R121
    ok=wb(kcd2vr::stowed_hide_hw::kHideAtStationsKey,kcd2vr::stowed_hide_hw::hideAtStationsSetting.load(std::memory_order_acquire))&&ok; // R121
    ok=wb("VrInteractionEyeRayR121",kcd2vr::eye_ray_r121::enabled.load(std::memory_order_acquire))&&ok; // R121
    ok=wb(kcd2vr::melee_grip_r123::kKey,kcd2vr::melee_grip_r123::enabled.load(std::memory_order_acquire))&&ok; // R123
    ok=wb(kcd2vr::prop_impact_sound_r124::kKey,kcd2vr::prop_impact_sound_r124::enabled.load(std::memory_order_acquire))&&ok; // R124
    ok=wb("VrImpaleFastDeathR106",kcd2vr::impale_death_speed_r106::enabled.load(std::memory_order_acquire))&&ok; // R106
    ok=wb("VrCinematicBarsR107",kcd2vr::cinematic_bars_r107::enabled.load(std::memory_order_acquire))&&ok; // R107
    ok=wb("VrHudWalkoffD70",claude_d70::HudWalkoff())&&ok;
    ok=wb("VrNativeRoomscale1to1D76",roomscale_d76::Enabled())&&ok;
    ok=wb("VrRidingVrikD76",riding_vrik_d76::Get())&&ok;
    ok=wb("VrNpcListeningGesturesD76",lively_npc_d73::ListeningGestures())&&ok;
    ok=wf("VrRoomscaleLeanHeightD70",roomscale58::LeanHeightD70())&&ok;
    ok=wb("VrHandLeadD67C",claude_motion_d69::HandLead())&&ok;
    ok=wf("VrHandLeadMsD69",claude_motion_d69::HandLeadMs())&&ok;
    ok=wb("VrElbowContinuityD67C",claude_motion_d69::ElbowHysteresis())&&ok;
    ok=wb("VrWeaponSameFrameD67C",weapon_trials_d69::sameFrame())&&ok;
    ok=wb("VrRigidGripD67C",weapon_trials_d69::rigidGrip())&&ok;
    ok=wb("VrGripAnchorD67",held_motion_d67::gripAnchored())&&ok;
    ok=wb("VrCurrentArcCoverageD67",held_motion_d67::currentArcCoverage())&&ok;
    ok=wb("VrSupportReleaseD67",held_motion_d67::supportRelease())&&ok;
    ok=wb("VrContactFairnessD67",held_motion_d67::contactFairness())&&ok;
    ok=wb("VrElbowContinuityD67",motion_trials_d67::ContinuityRepair())&&ok;
    ok=wf("VrElbowOutwardD67",motion_trials_d67::ElbowOutward())&&ok;
    ok=wf("VrElbowBackD67",motion_trials_d67::ElbowBack())&&ok;
    ok=wb("VrSmithRecoveryD67",crafting_d67::smithRecovery.load())&&ok;
    ok=wb("VrSmithSemiAnchorsD67",crafting_d67::smithSemiAnchors.load())&&ok;
    ok=wi("VrAlchemyTransferTuningD67Adopted",1)&&ok;

    ok=wb("VrKnifeRightHipD66",knife_pouch_d66::rightHipSetting())&&ok;
    ok=wb("VrKnifeChestZoneR109",knife_pouch_d66::chestZoneSetting())&&ok; // R109
    ok=wb("VrEyeSrgbR114",kcd2vr::xr_eye_srgb_r114::eyeSrgb.load(std::memory_order_acquire))&&ok; // R114
    ok=wb("VrVrikOnStartR111",start_r111::vrikOnStart.load(std::memory_order_acquire))&&ok; // R111
    ok=wb("VrStartAtHenryEyesR111",start_r111::henryEyes.load(std::memory_order_acquire))&&ok; // R111
    ok=wi("VrTimingLocationD64",int(smooth_lab::TimingLocationD64()))&&ok;
    ok=wf("VrMapPaperSizeD64",held_map_m1::paperSizeD64.load())&&ok;
    ok=wb("VrMapTwoHandD64",held_map_m1::twoHandD64.load())&&ok;
    ok=wb("VrMapNativePaperM43",held_map_m1::NativePaperSelectedM43())&&ok;
    ok=wf("VrAlchemyTransferSpeedD65",action_time_owner59c11::AlchemyTransferSpeedD65())&&ok;
    ok=wb("VrUnownedPickupClockD65",action_time_owner59c11::UnownedPickupClockD65())&&ok;
    ok=wb("VrTwoHandPrimaryBiasExperimentD65",two_hand_d65::enabled.load())&&ok;
    ok=wf("VrTwoHandPrimaryBiasD65",two_hand_d65::setting(two_hand_d65::primaryBias.load()))&&ok;
    ok=wb("VrNativeKillAnimations",native_kill_nf::enabled.load(std::memory_order_acquire))&&ok;
    ok=wb("VrPairedHitTimingD62C",paired_signals_d62c::enabled.load(std::memory_order_acquire))&&ok;
    ok=wf("VrPairedHitSpeedD62C",paired_signals_d62c::speed.load(std::memory_order_acquire))&&ok;
    ok=wb("VrFrikElbows",frik_elbow_lab::enabled.load(std::memory_order_acquire))&&ok;
    ok=wb("VrElbowSolverR89",elbow_r89::enabled.load(std::memory_order_acquire))&&ok; // R90
    ok=wb("VrArrowHoldNativeTurn",arrow_hold_d48::nativeTurn.load(std::memory_order_acquire))&&ok;
    ok=wi("VrArrowHoldNativeTurnD56Adopted",1)&&ok;
    ok=wb("VrFistStrokeTrialD49",fist_stroke_d49::enabled.load(std::memory_order_acquire))&&ok;
    ok=wb("VrPickupCameraBridgeD55",camera_hold_e1::pickupBridgeD55.load(std::memory_order_acquire))&&ok;
    ok=wb("VrAlchemyToolContactD55",alchemy_tool_contact_d55::enabled.load(std::memory_order_acquire))&&ok;
    ok=wb("VrLivelyNpcD73",lively_npc_d73::Enabled())&&ok;
    ok=wb("VrDialogueNpcLookHmd",npc_look_policy63::followHmd())&&ok;
    ok=wb("VrPickupNearOnlyD40",pickup_use19c3::nearOnlyD40.load(std::memory_order_acquire))&&ok;
    ok=wb("VrCombatNoStep",frik_pose_probe::CombatNoStep55())&&ok;
    ok=wb("VrCombatNoTurnD40",frik_pose_probe::CombatNoTurnD40())&&ok;
    ok=wb("VrFreeTargeting",free_targeting_d42::enabled.load(std::memory_order_acquire))&&ok;
    ok=wi("VrClinchModeD40",frik_pose_probe::ClinchModeD40())&&ok;
    // This existing preference loaded from INI but was missing from SaveMenuSettings.
    ok=wb("VrLabLivingTouch58",false)&&ok;
    // D41: these existing runtime owners loaded from INI but could not be
    // changed or saved from Experiments. Keep their canonical keys/defaults.
    ok=wb("VrNpcReactsToBlade",bodycombo59c7g::npcReactsToBlade.load(std::memory_order_acquire))&&ok;
    ok=wb("VrNpcReactsStaminaHits",bodycombo59c7g::staminaHits.load(std::memory_order_acquire))&&ok;
    ok=wb("VrPhysicalComboFinisher",finisher_cb1::enabled.load(std::memory_order_acquire))&&ok;
#elif defined(KCD2VR_D40_LIVE_LOAD)
    instant_d72::activities.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrInstantActivitiesD72",true));
    instant_d72::interactions.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrInstantInteractionsD72",false));
    instant_d72::crafting.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrInstantCraftingD72",false));
    instant_d72::reloads.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrInstantReloadsD72",false));
    instant_d72::combat.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrInstantCombatD72",true));
    contact_handover_d71::SetContactContinuity(ReadIniBoolExplicit(explicit_ini,"Settings","VrContactContinuityD71",true));
    contact_handover_d71::SetItemHandover(ReadIniBoolExplicit(explicit_ini,"Settings","VrItemHandoverD71",true));
    dodge_window_d70::SetFactor(ReadIniFloatExplicit(explicit_ini,"Settings","VrDodgeWindowScaleD70",1.25f));
    claude_d70::LoadD77([&](const char* key,bool fallback){return ReadIniBoolExplicit(explicit_ini,"Settings",key,fallback);});
    // D83R1: ON = limited tracked head turns during native finishers; OFF = native pose.
    finisher_upper_d83::player.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrFinisherUpperBodyPlayerD83",false),std::memory_order_release);
    finisher_upper_d83::npc.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrFinisherUpperBodyNpcD83",false),std::memory_order_release);
    // D84: ON = native attack side from the blade stroke; OFF = old cocked-hand zone.
    blade_zone_d84::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrBladeZoneD84",true),std::memory_order_release);
    // D84: ON = an upward cut is the native low attack (5); OFF = old left/right side.
    blade_zone_d84::upCutLow.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrUpCutLowD84",true),std::memory_order_release);
    // D84: ON = stab side from its start (above 0, left 1, right 2, chest/below 5); needs VrThrustAsSwing.
    blade_zone_d84::stabStart.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrStabStartZoneD84",true),std::memory_order_release);
    // D85 dice: ON = point the right hand at a die to move the native dice cursor; OFF = gamepad only.
    dice_hands_d85::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDiceHandsD85",false),std::memory_order_release);
    // R90 (R89 pickup): ON = small scenery props (mugs, plates, candles) can be carried, placed and thrown.
    // R124: ON by default (Miki's INI). OFF = nothing is read.
    static_grab_r89::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrStaticPropGrabR89",true),std::memory_order_release);
    // R96 crossbow reload route: true = HAND ONLY (bow pattern, default), false = NATIVE-DRIVEN (XR34).
    // New key: R92-R95 force-wrote VrCrossbowHandOnlyR89=false on every exit, so that old
    // value is not the player's choice and is no longer read.
    ranged_options_d65c::crossbowHandOnlyR90.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrCrossbowReloadRouteR96",true),std::memory_order_release);
    // R111: heavy crossbow string pulled by hand like the light one (true, default) or the old cranequin route (false).
    ranged_options_d65c::heavyCrossbowHandPullR111.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrHeavyCrossbowHandPullR111",true),std::memory_order_release);
    // R117: goat's-foot crossbow string pulled by hand like the light one (true, default) or the old spanner route (false).
    ranged_options_d65c::mediumCrossbowHandPullR117.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrMediumCrossbowHandPullR117",true),std::memory_order_release);
    // R118: controller layout from the start setup / VR CONFIG > CONTROLS / Experiments GEAR page 2. 0 (default) =
    // OPENXR native (Left Menu = inventory, X hold = torch); 1 STEAMVR and 2 PS VR2: X/Square hold = inventory.
    kcd2vr::controller_layout_r118::Set(ReadIniIntExplicit(explicit_ini,"Settings","VrControllerLayoutR118",0));
    // R112: quick weapon/potion selectors fade 3 s after the last quick-access input (true, default) or stay shown (false).
    kcd2vr::ui_hud_r112::quickSlotFade.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrHudQuickSlotFadeR112",true),std::memory_order_release);
    // R116: alchemy selection follows the headset view (idle hands) and the reaching hand (true, default); false = R115 selection.
    kcd2vr::subsystems::alchemy::head_select_r116::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrAlchemyViewSelectR116",true),std::memory_order_release);
    // R116: weapon whoosh at the physical release, the native clip's later whoosh skipped (true, default); false = clip whoosh only.
    kcd2vr::swing_whoosh_r116::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrSwingWhooshR116",true),std::memory_order_release);
    // R90: corpse search. false = always, 10/s per idle hand (R2 budgeted); true = only within 700 ms of a starting squeeze (R89).
    lab58::corpseSqueezeSearchR90.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrCorpseSearchSqueezeR89",false),std::memory_order_release);
    // R90: ON = VrWeaponWeightFeel also scales the two-hand hybrid rotation drive; OFF = R2 heft.
    kcd2vr::two_hand_weight_r90::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrTwoHandWeightFeelR90",false),std::memory_order_release);
    // R96: ON (default) = R3 with a melee weapon against an enemy sends the SPECIAL kick input (companion kick rows); OFF = HEAVY zone 5.
    kcd2vr::bridges168::weaponKickR96.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrWeaponKickR96",true),std::memory_order_release);
    // R100: ON (default) = Grip on a small level-brush prop (<= 65 cm) hides the brush, suspends its physics and
    // spawns a NOT SAVED loose copy you carry on the small-prop path; OFF = brushes stay level geometry.
    kcd2vr::brush_swap_r100::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrSceneryPropSwapR100",true),std::memory_order_release);
    // R116: ON (default) = a held small prop follows the hand every frame (hand velocity + half the gap, no force
    // budget, release = hand velocity); OFF = the R103 servo (80 N, commands only when the coarse tick advanced).
    kcd2vr::prop5916c1::firmHoldR116.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrPropFirmHoldR116",true),std::memory_order_release);
    // R120: ON (default) = the shield presses the native block only inside the shield box (VrShieldBox*R120) with its
    // own entry/release speeds; OFF = the old wide R40C1 shield rule (block held almost whenever a shield is in hand).
    kcd2vr::shield_block_r120::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings",kcd2vr::shield_block_r120::kEnabledKey,true),std::memory_order_release);
    // R121 (was a D33 INI-only read in mod.cpp): VrHideBackWeapons ON (default) = bow, crossbow, shield and quiver on the
    // back are hidden in normal play, whatever "Stowed bow visibility" says; OFF = the stowed bow row decides for the bow.
    // VrHideStowedAtStations ON (default) = every stowed weapon hidden at the alchemy/smithing table. GEAR page 2.
    kcd2vr::stowed_hide_hw::hideBackSetting.store(ReadIniBoolExplicit(explicit_ini,"Settings",kcd2vr::stowed_hide_hw::kHideBackKey,true),std::memory_order_release);
    kcd2vr::stowed_hide_hw::hideAtStationsSetting.store(ReadIniBoolExplicit(explicit_ini,"Settings",kcd2vr::stowed_hide_hw::kHideAtStationsKey,true),std::memory_order_release);
    // R121: the head look/selection ray starts at the rendered eye (true, default); false = Henry's game eye (R120).
    kcd2vr::eye_ray_r121::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrInteractionEyeRayR121",true),std::memory_order_release);
    // R123: true (default) = a melee weapon sits at its own hand socket (authored wrist relation); false = learned from the animated hand.
    kcd2vr::melee_grip_r123::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings",kcd2vr::melee_grip_r123::kKey,true),std::memory_order_release);
    // R124: ON (default) = a released/knocked loose prop of ours plays the game's authored put-down sound (clay, wood,
    // metal, glass ...) when its velocity changes >= 1.2 m/s in one frame; OFF = silent props (R123). GEAR page 2.
    kcd2vr::prop_impact_sound_r124::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings",kcd2vr::prop_impact_sound_r124::kKey,true),std::memory_order_release);
    kcd2vr::impale_death_speed_r106::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrImpaleFastDeathR106",true),std::memory_order_release); // R106
    kcd2vr::cinematic_bars_r107::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrCinematicBarsR107",true),std::memory_order_release); // R107
    // R91 dlss: ON (default) = DLSS/FSR stereo start fallback to the D85 eye boundary when R4's
    // final-main transaction gives no pair for 1 s (D85R5: the only path that ever gave DLSS stereo).
    claude_d70::SetHudWalkoff(ReadIniBoolExplicit(explicit_ini,"Settings","VrHudWalkoffD70",false));
    roomscale_d76::SetEnabled(ReadIniBoolExplicit(explicit_ini,"Settings","VrNativeRoomscale1to1D76",false));
    riding_vrik_d76::Set(ReadIniBoolExplicit(explicit_ini,"Settings","VrRidingVrikD76",true));
    lively_npc_d73::SetListeningGestures(ReadIniBoolExplicit(explicit_ini,"Settings","VrNpcListeningGesturesD76",true));
    roomscale58::SetLeanHeightD70(ReadIniFloatExplicit(explicit_ini,"Settings","VrRoomscaleLeanHeightD70",.12f));
    claude_motion_d69::SetHandLeadMs(ReadIniFloatExplicit(explicit_ini,"Settings","VrHandLeadMsD69",11.f));
    claude_motion_d69::SetHandLead(ReadIniBoolExplicit(explicit_ini,"Settings","VrHandLeadD67C",false));
    claude_motion_d69::SetElbowHysteresis(ReadIniBoolExplicit(explicit_ini,"Settings","VrElbowContinuityD67C",false));
    weapon_trials_d69::setSameFrame(ReadIniBoolExplicit(explicit_ini,"Settings","VrWeaponSameFrameD67C",false));
    weapon_trials_d69::setRigidGrip(ReadIniBoolExplicit(explicit_ini,"Settings","VrRigidGripD67C",true));
    held_motion_d67::setGripAnchored(ReadIniBoolExplicit(explicit_ini,"Settings","VrGripAnchorD67",true));
    held_motion_d67::setCurrentArcCoverage(ReadIniBoolExplicit(explicit_ini,"Settings","VrCurrentArcCoverageD67",true));
    held_motion_d67::setSupportRelease(ReadIniBoolExplicit(explicit_ini,"Settings","VrSupportReleaseD67",false));
    held_motion_d67::setContactFairness(ReadIniBoolExplicit(explicit_ini,"Settings","VrContactFairnessD67",false));
    motion_trials_d67::SetContinuityRepair(ReadIniBoolExplicit(explicit_ini,"Settings","VrElbowContinuityD67",true));
    motion_trials_d67::SetElbowOutward(ReadIniFloatExplicit(explicit_ini,"Settings","VrElbowOutwardD67",.20f));
    motion_trials_d67::SetElbowBack(ReadIniFloatExplicit(explicit_ini,"Settings","VrElbowBackD67",.25f));
    crafting_d67::setSmithRecovery(ReadIniBoolExplicit(explicit_ini,"Settings","VrSmithRecoveryD67",true));
    crafting_d67::setSmithSemiAnchors(ReadIniBoolExplicit(explicit_ini,"Settings","VrSmithSemiAnchorsD67",true));

    knife_pouch_d66::setRightHip(ReadIniBoolExplicit(explicit_ini,"Settings","VrKnifeRightHipD66",false));
    knife_pouch_d66::setChestZone(ReadIniBoolExplicit(explicit_ini,"Settings","VrKnifeChestZoneR109",false)); // R109
    kcd2vr::xr_eye_srgb_r114::eyeSrgb.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrEyeSrgbR114",true),std::memory_order_release); // R114
    start_r111::vrikOnStart.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrVrikOnStartR111",true),std::memory_order_release); // R111: calibrated = VRIK at start/load
    start_r111::henryEyes.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrStartAtHenryEyesR111",true),std::memory_order_release); // R111: recenter = Henry's eyes
    smooth_lab::SetTimingLocationD64(unsigned(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrTimingLocationD64",1),0,1)));
    held_map_m1::paperSizeD64.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrMapPaperSizeD64",.80f),.40f,1.5f));
    held_map_m1::twoHandD64.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrMapTwoHandD64",true));
    held_map_m1::SetNativePaperM43(ReadIniBoolExplicit(explicit_ini,"Settings","VrMapNativePaperM43",false));
    native_kill_nf::SetEnabled(ReadIniBoolExplicit(explicit_ini,"Settings","VrNativeKillAnimations",false));
    // D63: retain compatibility preferences, default OFF. Runtime timing is quarantined until the native pair join is proven.
    paired_signals_d62c::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrPairedHitTimingD62C",false),std::memory_order_release);
    paired_signals_d62c::speed.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrPairedHitSpeedD62C",4.f),1.f,8.f),std::memory_order_release);
    arrow_hold_d48::nativeTurn.store(arrow_hold_d48::LoadNativeTurnD56(
        ReadIniIntExplicit(explicit_ini,"Settings","VrArrowHoldNativeTurnD56Adopted",0),
        ReadIniBoolExplicit(explicit_ini,"Settings","VrArrowHoldNativeTurn",true)),std::memory_order_release);
    fist_stroke_d49::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrFistStrokeTrialD49",true),std::memory_order_release);
    {float rate=ReadIniFloatExplicit(explicit_ini,"Settings","VrAlchemyTransferSpeedD65",8.f);
     // User-requested D67 fast default: adopt only the inherited exact1x default once.
     // Custom rates survive; after a D67 save, selecting1x remains native speed.
     if(ReadIniIntExplicit(explicit_ini,"Settings","VrAlchemyTransferTuningD67Adopted",0)==0&&rate==1.f)rate=4.f;
     action_time_owner59c11::SetAlchemyTransferSpeedD65(rate);}
    action_time_owner59c11::SetUnownedPickupClockD65(ReadIniBoolExplicit(explicit_ini,"Settings","VrUnownedPickupClockD65",true));
    two_hand_d65::primaryBias.store(two_hand_d65::setting(ReadIniFloatExplicit(explicit_ini,"Settings","VrTwoHandPrimaryBiasD65",.4f)));
    two_hand_d65::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrTwoHandPrimaryBiasExperimentD65",true));
    camera_hold_e1::pickupBridgeD55.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrPickupCameraBridgeD55",true),std::memory_order_release);
    alchemy_tool_contact_d55::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrAlchemyToolContactD55",true),std::memory_order_release);
    lively_npc_d73::SetEnabled(ReadIniBoolExplicit(explicit_ini,"Settings","VrLivelyNpcD73",true));
    npc_look_policy63::setFollowHmd(ReadIniBoolExplicit(explicit_ini,"Settings","VrDialogueNpcLookHmd",true));
    pickup_use19c3::setNearOnlyD40(ReadIniBoolExplicit(explicit_ini,"Settings","VrPickupNearOnlyD40",false));
    frik_pose_probe::SetCombatNoTurnD40(ReadIniBoolExplicit(explicit_ini,"Settings","VrCombatNoTurnD40",true));
    free_targeting_d42::set(ReadIniBoolExplicit(explicit_ini,"Settings","VrFreeTargeting",true));
    frik_pose_probe::SetClinchModeD40(ReadIniIntExplicit(explicit_ini,"Settings","VrClinchModeD40",2));
#else
#error D40 live runtime requires an explicit owner contribution
#endif
