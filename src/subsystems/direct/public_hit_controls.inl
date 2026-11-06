void SetVrDirect18(bool ownHits,float npcBlockSpeed) noexcept { // C10R18 F4 press 4
    if(nhp134::geom27::vrstate596::g_own_hits18.load()!=ownHits||nhp134::geom27::vrstate596::g_npc_block_speed18.load()!=npcBlockSpeed)nhp134::combo59c17::ClearLineage19();
    nhp134::geom27::vrstate596::g_own_hits18.store(ownHits,std::memory_order_relaxed);
    kcd2vr::motion59c4::lateContactMs18.store(ownHits?200u:0u,std::memory_order_relaxed);
    nhp134::geom27::vrstate596::g_npc_block_speed18.store(npcBlockSpeed,std::memory_order_relaxed);
    const bool native19=std::isfinite(npcBlockSpeed)&&npcBlockSpeed>0.f&&npcBlockSpeed<=1.0001f;
    char m[300]{};std::snprintf(m,sizeof(m),"C10R19 VR_DIRECT ownsHits=%d (no native parry-in-place without a contact, contacts never dropped for Henry's own guard, late contact %u ms) npcGuardTiming=%.2fx %s",
        ownHits?1:0,ownHits?200u:0u,double(npcBlockSpeed),native19?"(NATIVE: Henry +0x450 untouched, as C10R3)":npcBlockSpeed>0.f?"(EXPERIMENTAL: constant +0x450 - his guard counts as up earlier, the animation does not speed up)":"(legacy ETA seconds)");diagnostic_trace::Write(m);}
void SetRung4Switches59c16(bool reclaimFromOwnGuard,bool strikeDuringStagger) noexcept { // R59.16C10R16
    nhp134::geom27::vrstate596::g_reclaim_guard59c16.store(reclaimFromOwnGuard,std::memory_order_release);
    nhp134::geom27::vrstate596::g_accept_stagger59c16.store(strikeDuringStagger,std::memory_order_release);}
void SetVrNeutralHitMinEnergy59c4(float energy) noexcept { // R59.16C4
    const float e=std::isfinite(energy)?std::clamp(energy,0.f,1.f):.10f;
    nhp134::geom27::vrstate596::g_neutral_min_energy.store(e);
    char m[200]{};std::snprintf(m,sizeof(m),"R59.16C4 HIT_POLICY neutralMinEnergy=%.2f appliesOnlyWith=VrHitHostileOnly=1 (accidental-contact protection; deliberate strikes land)",e);diagnostic_trace::Write(m);
}
void SetVrHitPolicy5913(unsigned targetRepeatMs,bool hostileOnly) noexcept {
    nhp134::geom27::vrstate596::g_target_repeat_ms.store(targetRepeatMs?targetRepeatMs:1);
    nhp134::geom27::vrstate596::g_hostile_only.store(hostileOnly);
    char m[280]{};std::snprintf(m,sizeof(m),"R59.16C3 HIT_POLICY targetRepeatMs=%u hostileOnly=%d neutralFirstHit=%s nativeOffense=QUEUED_TRANSACTION combatFlags=NOT_FORCED (target repeat is a mod limiter, not combo timing)",targetRepeatMs,hostileOnly?1:0,hostileOnly?"LIGHT_CONTACT_PROTECTED":"ELIGIBLE_WITH_NATIVE_TARGET_CHECKS");
    diagnostic_trace::Write(m);
}
