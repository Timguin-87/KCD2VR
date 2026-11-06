bool CombatExperiment175Enabled() noexcept {return g_combat_experiment175.load(std::memory_order_acquire);}
void SetPlayerCombatWindowScaleH26(float scale) noexcept {
    kcd2vr::player_window_h26::multiplier.store(std::isfinite(scale)?std::clamp(scale,1.f,2.f):1.f,std::memory_order_release);
}
bool CombatIntegrated178r22c2Enabled() noexcept {return CombatExperiment175Enabled()&&g_combat_integrated178r22c2.load(std::memory_order_acquire);} // V66.178R28: F4 rung 1
int F4ContactGateMode178r26() noexcept {return g_f4_contact_gate178r26.load(std::memory_order_relaxed);}
bool PhysicalCombat177Enabled() noexcept {return CombatExperiment175Enabled()&&g_physical_combat177.load(std::memory_order_acquire);}
void SetContactStartsAttack178r41(bool on) noexcept {g_contact_starts_attack178r41.store(on,std::memory_order_relaxed);}
void SetAttackAtPrepare178r42(bool on) noexcept {g_attack_at_prepare178r42.store(on,std::memory_order_relaxed);}
void SetWeaponAuthorityScale178r44(float k) noexcept {g_weapon_authority_scale178r44.store(std::clamp(k,0.2f,2.0f),std::memory_order_relaxed);}
// R59.16C10R14 weapon weight / fatigue feel (see weapon_dynamics177.h handling59c14 / fatigueRamp59c14).
