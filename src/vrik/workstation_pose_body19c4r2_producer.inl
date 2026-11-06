// Existing body admission, shared verbatim with the connected bootstrap fixture.
            const bool qualifiedWorkstationC1=activity_input163::TrackedHands59c7f();
            const bool workstationBody178r13=(kcd2vr::minigame_hands45::currentMode()!=kcd2vr::minigame_hands45::Native||kcd2vr::guided_crafting55::active(std::uint64_t(GetTickCount64())))&&
                (workstation178r20c1::ownsActivityWrist45(GetTickCount64())||hands59c7f::active())&&qualifiedWorkstationC1&&
                visual_selection59c7f::Selected(frik_pose_probe::CalibrationReady(),frik_pose_probe::CombatCensusMode(),frik_pose_probe::FinalSkinTestMode())&&
                !safe_ui_fit&&!matrix_active&&!mis_menu_after_frame&&!ui88_input_result.blocks_gameplay&&!dialogue_active;
            hands59c7f::activityVisual.store(workstationBody178r13,std::memory_order_release);
