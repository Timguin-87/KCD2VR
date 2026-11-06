// Generated from mod.cpp by tools/extract_r3_route176.py; do not edit.
        if(touch_events.special_attack_requested){
            // U16: honor the KICKS page. Exactly one semantic producer per R3.
            const bool procedural=frik_pose_probe::CombatExperiment175Enabled()&&
                kcd2vr::kick26::Current()==kcd2vr::kick26::Style::VrLegOnly;
            // H35: with EMPTY hands the kick is the fist lane's own prepare/release with zone 5 (the proven 14 Oct chain); the
            // special-attack ingress stays the route for everything else (armed R3 unchanged).
            const int kickHandH35=kcd2vr::kick26::ForcedSide()<0?0:kcd2vr::kick26::ForcedSide();
            const auto kickResultH35=procedural?frik_pose_probe::FistKickResultH35::NotThisLane:frik_pose_probe::RequestFistKickStatusH35(kickHandH35);
            const bool fistKickH35=kickResultH35==frik_pose_probe::FistKickResultH35::Queued;
            const bool queued=fistKickH35?true:kickResultH35==frik_pose_probe::FistKickResultH35::Refused?false:procedural?frik_pose_probe::RequestVrLegKick34():frik_pose_probe::RequestNativeSpecial168();
            if(queued&&!fistKickH35)frik_pose_probe::NoteKickRequest26(!procedural);
            diagnostic_trace::Write(fistKickH35?"U16 R3_KICK route=FIST_ZONE5_NATIVE (H35: empty hands, prepare/release lane, game selects the kick row)":procedural?(queued?
                "U16 R3_KICK route=VR_LEG_CONTACT applications<=1 cost=NATIVE_IF_TEST_ENABLED":
                "U16 R3_KICK route=VR_LEG_CONTACT state=REFUSED context_or_busy=1"):
                (queued?"U16 R3_KICK route=NATIVE_SPECIAL selection=NATIVE_NOT_INFERRED":"U16 R3_KICK route=NATIVE_SPECIAL state=REFUSED"));
        }
