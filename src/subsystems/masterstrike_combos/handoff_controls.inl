void SetMasterHandoff23(bool enabled,float forwardSpeed) noexcept {g_master_handoff_enabled23.store(enabled,std::memory_order_relaxed);
    g_master_handoff_speed23.store(std::isfinite(forwardSpeed)?std::min(forwardSpeed,6.f):-1.f,std::memory_order_relaxed);}
