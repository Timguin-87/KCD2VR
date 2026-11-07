bool NativeBlockIndicatorH24(std::uint64_t now) noexcept {
    kcd2vr::hmd_dodge_h24::BlockContext c{};
    return retail_build112::Ready(nhp134::g_base)&&frik_pose_probe::BlockIndicatorGameplayH24(now)&&
        kcd2vr::hmd_dodge_h24::blockPublished.read(c)&&
        kcd2vr::hmd_dodge_h24::blockCurrent(c,reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity()),
            frik_pose_probe::PoseEpochH24(),now)&&c.maximum!=0;
}