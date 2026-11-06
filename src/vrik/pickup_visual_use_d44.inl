void NoteVrPickupUse43(int side,std::uint32_t entity,bool pickableItem,std::uint64_t nowMs) noexcept {
    // D44: Use precedes Attach. The exact native grasp capability must be ready,
    // but its live item owner need not exist yet. This holds only our wrist
    // presentation; Picking, prop attachment and inventory stay native-owned.
    const bool available=action_time_owner59c11::NativeGraspAvailable63();
    if(!pickableItem||!available){kcd2vr::grab43::clear(side);return;}
    const kcd2vr::grab43::Context context{
        reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire)),
        g_pose_epoch6689.load(std::memory_order_acquire),tracking59c6::epoch.load(std::memory_order_acquire)};
    if(!context.actor||!context.pose||!context.tracking){kcd2vr::grab43::clear(side);return;}
    kcd2vr::grab43::Use u{};if(!kcd2vr::grab43::note(side,entity,nowMs,&u,context))return;
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,nowMs))return;
    char m[360]{};std::snprintf(m,sizeof(m),"C43 GRAB_ALIGN edge=VR_PICKUP_USE side=%d target=%u serial=%u setting=%s graspAvailable=ON itemAuthority=UNCHANGED windowMs=%llu requester=VR_HAND_USE(PickableItem) nativePicking=UNCHANGED handover=%s",
        side,entity,u.serial,kcd2vr::grab43::handStays.load(std::memory_order_relaxed)?"ON":"OFF",(unsigned long long)kcd2vr::grab43::kWindowMs,
        kcd2vr::grab43::handStays.load(std::memory_order_relaxed)?"WITHHELD_UNTIL_ATTACH":"NATIVE_REACH_BLEND");
    diagnostic_trace::Write(m);
}
