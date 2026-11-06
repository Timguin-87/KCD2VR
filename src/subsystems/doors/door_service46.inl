// The existing worker remains the sole scheduler. Native hooks/cleanup are host
// owned; the Doors module owns only this bridge-service lifecycle state.
{
    const auto now46=std::uint64_t(GetTickCount64());
    const bool wanted46=kcd2vr::door_use46::enabled.load(std::memory_order_acquire)&&
        kcd2vr::door_use46::inputMode(unsigned(MiscInteractionDiagMode()))&&CalibrationReady()&&InteractionActionReady175()&&
        retail_build112::Ready(WhBase())&&action_time_owner59c11::Installed();
    static constexpr kcd2vr::subsystems::doors::ServiceLeaves leaves46{
        &kcd2vr::door_signal19c4r4::Ready,&kcd2vr::door_signal19c4r4::SetEnabled,
        &StartNativeDoor19c4,&StopNativeDoor19c4,&diagnostic_trace::Write};
    (void)kcd2vr::subsystems::doors::TickService({wanted46},now46,leaves46);
}
