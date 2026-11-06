void SetGuardIncoming178r45(bool repress,int phaseMask,unsigned cooldownMs) noexcept {
    g_guard_incoming_repress45.store(repress,std::memory_order_relaxed);
    (void)phaseMask; // R55C2: legacy INI key cannot substitute phases for the native PerfectBlock slot.
    g_guard_incoming_cooldown45.store(cooldownMs,std::memory_order_relaxed);}
void SetDirectionalStabs178r29(bool on) noexcept {g_directional_stabs178r29.store(on,std::memory_order_relaxed);}
void SetThrustAsSwing55(bool on) noexcept {g_thrust_as_swing55.store(on,std::memory_order_relaxed);}
void SetWindupHold57(bool on) noexcept {g_windup_hold57.store(on,std::memory_order_relaxed);}
void SetTelegraphWindup59c10(bool on) noexcept {g_telegraph_windup59c10.store(on,std::memory_order_relaxed);}
void SetWallSlide59c10(bool on) noexcept {g_wall_slide59c10.store(on,std::memory_order_relaxed);}
void SetWallFriction57(float friction) noexcept { physical::g_wall_friction57.store(std::isfinite(friction)?std::clamp(double(friction),0.0,.95):0.0,std::memory_order_relaxed); }
void SetAttackOpponentGate59c10(bool requireOpponent,float reachMeters) noexcept {g_attack_require_opponent59c10.store(requireOpponent,std::memory_order_relaxed);g_attack_reach_m59c10.store(std::isfinite(reachMeters)?reachMeters:2.6f,std::memory_order_relaxed);}
// R59C1: retain the selected R59 experimental zone-3 request with the corrected
// companion. Native admission still decides; armed-kick acceptance is unverified.
