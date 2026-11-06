bool BlockIndicatorGameplayH24(std::uint64_t now) noexcept {
    return g_gameplay_allowed.load(std::memory_order_acquire)&&TrackingHeadUsable111(now)&&
        !g_ctx_menu.load(std::memory_order_acquire)&&!g_ctx_dialogue.load(std::memory_order_acquire)&&
        !g_ctx_cutscene.load(std::memory_order_acquire)&&!g_ctx_reading.load(std::memory_order_acquire)&&
        !g_vrik_calibration_requested.load(std::memory_order_acquire)&&
        !g_arm_capture_active66100.load(std::memory_order_acquire)&&g_guided_cal_pose.load(std::memory_order_acquire)==0&&
        !animation_filter::nativeBodyOwned112.load(std::memory_order_acquire);
}
