// Shared actual Mod producer. Camera intent must not wait for calibration or
// for the separate optional tracked-body/hand subsystem to be installed.
const dialogue_pose47c1::Context dialoguePolicy28{
    .enabled=vrik_in_dialogue_,
    .vrik=vrik_probe_installed&&visual_selection59c7f::Selected(frik_pose_probe::CalibrationReady(),
        frik_pose_probe::CombatCensusMode(),frik_pose_probe::FinalSkinTestMode()),
    .dialogue=true,.cameraMode=dialogue_mode_now,.theater=safe_ui_fit,
    .matrix=matrix_active,.vrMenu=mis_menu_after_frame,.nativeReference=diagnostics::lab::NativeReference(),
    .activity=activity_input163::witness175.Active(),.ui=ui89_policy};
activity_lens59c10r1::RequestDialogueFirstPerson19c2(
    dialogue_pose47c1::CameraRequested28(dialoguePolicy28),GetTickCount64());
