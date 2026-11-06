// Generated actual Mod semantic alchemy pointer permission.
        const auto& alchemyWitness59c2=activity_input163::witness175;
        const bool alchemyAvailable63=interaction_alignment_installed&&
            activity_input163::trackedHands59c7f[unsigned(activity_input163::Kind175::Alchemy)].load()&&
            activity163.nativeControls&&alchemyWitness59c2.ready.load(std::memory_order_acquire)&&
            !alchemyWitness59c2.transitionDepth175.load(std::memory_order_acquire)&&
            alchemyWitness59c2.active[unsigned(activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire)&&
            !safe_ui_fit&&!f2_ui_menu_active&&!matrix_active&&!diagnostics::lab::NativeReference()&&
            (!ui89_policy.BlocksWorldInteraction()||ui89_policy.NativeContextControls());
        const bool alchemyPalmExperiment63=combat_rules178r10::f5Rung.load(std::memory_order_acquire)==2&&
            workstation178r20c1::experimentArmed36.load(std::memory_order_acquire);
        const bool alchemyGesture67=alchemyAvailable63&&minigame_hands45::modeFor(minigame_hands45::Alchemy)!=minigame_hands45::Native&&
            (minigame_hands45::freeTrackedMode(minigame_hands45::modeFor(minigame_hands45::Alchemy))||alchemyPalmExperiment63);
        // H18: the obsolete whole-hand ray branch was reachable only in NATIVE
        // mode while F5=2, stealing gaze from the native fallback. Free Alchemy
        // already nominates ONE qualified nearby hand in its OnScreen bridge;
        // everywhere else (including Native/Semi and book) keep HMD gaze.
        const bool alchemyPointer59c2=false;
        const bool alchemyHeadSelection63=alchemyAvailable63;
        subsystems::alchemy::bridge67::enabled.store(alchemyGesture67,std::memory_order_release);
