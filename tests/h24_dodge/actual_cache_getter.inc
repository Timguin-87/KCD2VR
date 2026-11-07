bool IncomingHmdDodgeH24(std::uint64_t now) noexcept {
    kcd2vr::hmd_dodge_h24::Context c{};
    return kcd2vr::hmd_dodge_h24::published.read(c)&&kcd2vr::hmd_dodge_h24::current(c,
        reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity()),frik_pose_probe::PoseEpochH24(),now);
}
bool EngagedHmdDodgeD74(std::uint64_t now) noexcept {
    if(!retail_build112::Ready(nhp134::g_base))return false;
    kcd2vr::hmd_dodge_h24::Context c{};
    return kcd2vr::hmd_dodge_h24::published.read(c)&&kcd2vr::hmd_dodge_h24::currentEngaged(c,
        reinterpret_cast<std::uintptr_t>(frik_pose_probe::LastPlayerEntity()),frik_pose_probe::PoseEpochH24(),now);
}