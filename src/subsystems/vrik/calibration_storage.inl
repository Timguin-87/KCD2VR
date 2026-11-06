void ResetCalibration() noexcept {
    animation_filter::combatPoseTick112.store(0,std::memory_order_release);
    g_pose_epoch6689.fetch_add(1,std::memory_order_acq_rel);
    kcd2vr::kick34::ledger.retire(kcd2vr::kick34::Why::EpochChanged); // C37
    g_committed_pose_valid6689.store(false,std::memory_order_release);
    // A failed native restoration requires restart; recalibration must not hide it.

    g_cal_state.store(0,std::memory_order_release);
    // A transient reset must not erase persistent reach or demand another T-pose.
    g_cal_reanchor66100.store(g_saved_arm_ready66100.load(),std::memory_order_release);
    g_cal_native_anchor_pending.store(false,std::memory_order_release);
    g_legacy_cal_state.store(0,std::memory_order_release);
    g_body_cal_ready.store(false,std::memory_order_release);
    while(g_guided_cal_lock.test_and_set(std::memory_order_acquire)){}
    for(auto& a:g_guided_pose_result) a=GuidedCalAccum{};
    g_guided_cal_accum=GuidedCalAccum{};g_guided_cal_pose.store(0,std::memory_order_release);
    g_guided_cal_lock.clear(std::memory_order_release);
    g_vrik_calibration_requested.store(false,std::memory_order_release);
    g_ctrl_cry_x.store(0);g_ctrl_cry_y.store(0);g_ctrl_cry_z.store(0);g_controller_delta_len.store(0);
    g_goal_r_x.store(0);g_goal_r_y.store(0);g_goal_r_z.store(0);g_goal_l_x.store(0);g_goal_l_y.store(0);g_goal_l_z.store(0);
    g_goal_err_r.store(0);g_goal_err_l.store(0);g_eff_r_x.store(0);g_eff_r_y.store(0);g_eff_r_z.store(0);g_eff_l_x.store(0);g_eff_l_y.store(0);g_eff_l_z.store(0);
    g_fail_stage.store(0);g_pose_baseline_state.store(0,std::memory_order_release);
    if(!g_cal_stability_lock.test_and_set(std::memory_order_acquire)){
        g_cal_stable_have_sample=false; g_cal_stable_since_ms=0;
        g_cal_stability_lock.clear(std::memory_order_release);
    }
}

void ConfigureCalibrationPersistence(const char*path) noexcept {
    ConfigurePersistence66100(path);if(!path||!*path)return;
    const auto read=[&](const char* key,float fallback){char text[32]{};char def[16]{};std::snprintf(def,sizeof(def),"%.3f",fallback);
        GetPrivateProfileStringA("VRIK.Presentation",key,def,text,sizeof(text),path);char* end=nullptr;
        const float value=std::strtof(text,&end);return end!=text&&end&&!*end&&std::isfinite(value)?std::clamp(value,0.f,6.f):fallback;};
    g_relaxed_middle101=read("RelaxedMiddleDegrees",1);g_relaxed_ring101=read("RelaxedRingDegrees",2);g_relaxed_pinky101=read("RelaxedPinkyDegrees",4);
    g_eye_forward115=std::min(read("EyePivotForwardMeters",.09f),.15f);
    g_eye_up115=std::min(read("EyePivotUpMeters",.08f),.15f);
}
