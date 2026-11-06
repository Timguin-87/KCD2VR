// Actual Mod producer: activity controls remain menu-owned for gameplay.
// Only the reason for that block is shared with the exact workstation tool owner.
            gs.dialogue=dialogue_active;
            gs.cutscene=cutscene_flat_ui;
            gs.menu=ui89_policy.BlocksWorldInteraction()||mis_menu_after_frame||activity163.nativeControls;
            gs.tool_pose_ui_blocked=ui89_policy.BlocksWorldInteraction()||mis_menu_after_frame||safe_ui_fit||matrix_active||
                ui88_input_result.blocks_gameplay;
            gs.workstation_controls_only=activity163.nativeControls&&!gs.tool_pose_ui_blocked&&!dialogue_active&&!cutscene_flat_ui;
