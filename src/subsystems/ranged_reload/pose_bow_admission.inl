    const auto& bowProfile169r2=tx.physicalProfiles[1];
    const auto& priorBow169r2=g_phys_state6698.weapon[1];
    const bool sameBow169r2=prepared169r2&&g_phys_state6698.epoch==preparedRequest169r2&&g_phys_state6698.bodyEpoch==tx.epoch&&
        priorBow169r2.key==bowProfile169r2.key&&priorBow169r2.generation==bowProfile169r2.generation&&
        ts.tick>=priorBow169r2.lastTick&&ts.tick-priorBow169r2.lastTick<=80;
    const bool manualRequested169r2=true; // V172: qualified physical bow keeps tracked string draw even with native auto-loading
    const bool manualPose169r2=full&&prepared169r2&&manualRequested169r2&&bowProfile169r2.bow()&&
        bowProfile169r2.primary&&tx.physicalProfiles[0].key==0&&valid[0]&&valid[1]&&
        ts.left_grip>.25f&&
        !(sameBow169r2&&priorBow169r2.bowHeld&&priorBow169r2.nativeBowMode146);
