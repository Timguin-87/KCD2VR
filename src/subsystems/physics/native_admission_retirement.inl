static bool NativeHeldObserveAdmissionsT4(std::uint64_t now) noexcept {
    // Only the serialized native frame owner consumes this monotonic epoch.
    // Central contact/action gates refuse an unobserved epoch meanwhile, even
    // if a body was created AND retired between two native-frame observations.
    const auto serial=heldT4::AdmissionSerial();
    const auto seen=g_native_held_admission_seen_t4.load(std::memory_order_acquire);
    if(serial==seen)return false;
    kcd2vr::attack_xi178r8::cancel(now); // Native abort is dispatched before RT-up.
    PhysCancelNative6698();
    kcd2vr::contact176::recoveryEpoch55c1.fetch_add(serial>seen?serial-seen:1,std::memory_order_acq_rel);
    // Preserve C59 ENTERED tombstones; revoke only reserved native entries.
    kcd2vr::hybrid_serial59::releases.clear();
    g_phys_ready_sample6698.store(0,std::memory_order_release);
    g_native_held_admission_seen_t4.store(serial,std::memory_order_release);
    return true;
}
