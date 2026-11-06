// (door_pre_press66.h is included at FILE SCOPE in frik_pose_probe.cpp - it
//  declares namespace kcd2vr and must never be included inside a namespace)
void StartNativeDoor19c4() noexcept {g_door_stop19c4.store(false,std::memory_order_release);}
bool StopNativeDoor19c4() noexcept {
    g_door_stop19c4.store(true,std::memory_order_release);
    kcd2vr::door_signal19c4r4::Stop();
    return !g_door_active19c4.load(std::memory_order_acquire)&&!kcd2vr::door_signal19c4r4::HasCleanupLease();
}
// DS-F16 stage 2: on the press edge, offer the press-scoped surface request from
// the cached pre-press descriptor so the very next pose pass rays it (the
// admission is unchanged: the 20 cm witness and the native Use still decide).
static void WarmDoorSurfaceOnPress66(std::uint64_t now) noexcept {
    const auto generation=interaction_alignment::DoorAcquisitionGeneration65();
    if(!generation||!kcd2vr::door_use46::enabled.load(std::memory_order_acquire))return;
    kcd2vr::door_hold65::Frame frame{};
    if(!kcd2vr::door_hold65::frame.read(frame))return;
    for(int side=0;side<2;++side){
        const auto& h=frame.hand[side];
        if(!kcd2vr::door_hold65::current(h,now)||h.side!=side||!h.press||h.grip<.30f||h.sourceGeneration!=generation)continue;
        kcd2vr::door_pre_press66::Entry e{};
        if(!kcd2vr::door_pre_press66::read(side,now,generation,e))continue;
        (void)door_surface65::offer(door_surface65::request(e.target,h,now,{e.point[0],e.point[1],e.point[2]}),now);
    }
}
void TickNativeDoor19c4() noexcept {
    // Called only by the exact Henry controller hook on its verified thread.
    WarmDoorSurfaceOnPress66(std::uint64_t(GetTickCount64())); // DS-F16 stage 2 (bounded, fail-closed)
    TickDoorScript19c4r4();
}
bool DoorNativeMotionOwned66(std::uintptr_t actor,std::uint32_t target,std::uintptr_t entity,std::uint64_t now,std::uint64_t* serial) noexcept {
    return OwnedDoorMotion66(actor,target,entity,now,serial);
}
