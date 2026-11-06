void SetGeometryRejectNativeSweep178r40(bool on) noexcept {
    nhp134::geom27::g_reject_native_sweep178r40.store(true,std::memory_order_relaxed);
    if(!on)diagnostic_trace::Write("V66.178R51C3 VrGeometryRejectNativeSweep=0 ignored in contact-only mode: authored hits cannot be restored by missing VR input");
}
