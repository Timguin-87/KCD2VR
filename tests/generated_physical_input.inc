const bool physical_rt_owned = !mis_menu_after_frame && !activity163.nativeControls && !ui89_policy.BlocksWorldInteraction() &&
            !immersive_cursor_menu && frik_pose_probe::PhysicalWeaponConsumesRightTrigger139();
        if(physical_rt_owned) routeR.trigger=0.0f;
        // Resolve F4 gesture-only melee and fist/physical ownership before BOTH transports.
        // Previously native XI saw raw RT/LT first, while only fallback saw this
        // filter: held grips/triggers could drive an extra stock attack/block.
        const bool fist_menu=mis_menu_after_frame||ui89_policy.BlocksWorldInteraction()||immersive_cursor_menu||activity163.nativeControls;
        routeR.trigger=frik_pose_probe::FistTransportTrigger101(0,routeR.trigger,routeR.active,fist_menu,routeR.grip);
        routeL.trigger=frik_pose_probe::FistTransportTrigger101(1,routeL.trigger,routeL.active,fist_menu,routeL.grip);
