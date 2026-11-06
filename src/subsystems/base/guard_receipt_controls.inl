void SetGuardAfterAttackMs49(unsigned ms) noexcept {g_guard_after_attack_ms49.store(ms,std::memory_order_relaxed);}
void SetGuardStrict59c17(bool on) noexcept {g_guard_strict59c17.store(on,std::memory_order_relaxed); // R59.16C10R17
    diagnostic_trace::Write(on?"R59.16C10R17 GUARD strictEntry=1 (blade in front, no guard after a feint/wind-up or with the hand moving at the enemy, forward-only exit)":"R59.16C10R17 GUARD strictEntry=0 (R45 box)");}
void SetGuardCounterMotion57C1(bool on) noexcept {g_guard_countermotion57c1.store(on);}
void SetGuardAutoWindup19c4r1(bool on) noexcept {g_guard_auto_windup19c4r1.store(on);}
void SetBlockCooldownLease59c17(bool leased) noexcept {g_block_cooldown_lease59c17.store(leased,std::memory_order_relaxed);} // R59.16C10R17 receipt only
// R59.16C10R17: cumulative pose-side receipts for mod.cpp's 5 s NORMAL-log COMBAT_FLOW line (no F3 recording needed).
void CombatFlowReceipts59c17(char* out,std::size_t cap) noexcept {
    if(!out||!cap)return;
    std::snprintf(out,cap,"guard{strict=%d feints=%llu forwardVetoes=%llu} parry{incomingReads=%llu againstEdges=%llu lateralToward=%llu lateralMirror=%llu cooldownLease=%d} "
        "npcReaction{targetS=%.2f applied=%llu lastHitS=%.3f catchUps=%llu} master{windowSeen=%llu keptStance=%llu}",
        g_guard_strict59c17.load()?1:0,(unsigned long long)g_guard_feints_total59c17.load(),(unsigned long long)g_guard_forward_total59c17.load(),
        (unsigned long long)g_guard_reads_total59c17.load(),(unsigned long long)g_guard_against_total59c17.load(),(unsigned long long)g_guard_toward_total59c17.load(),
        (unsigned long long)g_guard_mirror_total59c17.load(),g_block_cooldown_lease59c17.load()?1:0,
        double(g_native_hit_time59c17.load()),(unsigned long long)g_native_hit_applied59c17.load(),double(g_native_hit_after59c17.load()),
        (unsigned long long)g_motion_clock_contacts50.load(),(unsigned long long)g_ms_window_seen59c17.load(),(unsigned long long)g_ms_window_presses59c17.load());
    const std::size_t used23=std::strlen(out); // C23b: cumulative OUTCOME_CHAIN counters (receipts only)
    if(used23+1<cap){out[used23]=' ';out[used23+1]=0;kcd2vr::outcome23::formatCounters(kcd2vr::outcome23::counters,out+used23+1,cap-used23-1);}
}
