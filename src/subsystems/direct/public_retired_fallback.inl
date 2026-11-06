void SetGeometryDirectFallback178r37(bool on) noexcept {
    nhp134::geom27::g_direct_fallback178r37.store(false,std::memory_order_relaxed);
    if(on)diagnostic_trace::Write("V66.178R51C3 VrGeometryDirectFallback=1 ignored: absent native admission does not authorize direct damage");
}
