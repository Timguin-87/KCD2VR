std::atomic<std::uint64_t> g_coalesced59c3{0},g_sweep_rejected178r40{0};std::atomic<bool> g_reject_native_sweep178r40{true};
// V66.178R40 (direction review 6): ONE contact producer per owned stroke. In rung 2 with the live route, Henry's own canned
// weapon sweep never hits - only the VR blade's contact (the injection) does. Called from the ProcessCollisionHit detour.
bool InjectedContact40(const void* rpg,const void* request) noexcept {
    return kcd2vr::geometry40c1::Scope::matches(Rd<const void*>(rpg,8,nullptr),
        Rd<const void*>(request,0x18,nullptr),Rd<const void*>(request,0x10,nullptr));
}
bool NativeSweepRejected178r40(std::uintptr_t caller,void* rpg,const void* request) noexcept {
    // Selected ownership survives a producer fault or missing/expired ticket.
    // Those conditions must never re-enable an authored invisible blade.
    if(!frik_pose_probe::PhysicalCombat177Enabled()||
       !g_live178r35.load(std::memory_order_relaxed)||InjectedContact40(rpg,request))return false;
    const void* actor=Rd<const void*>(rpg,8,nullptr);const void *e=nullptr,*s=nullptr;if(!CombatActor(actor,e,s,false)||!IsPlayer(e))return false;
    // Same retail ordinary producer as the original F4 gate. Preserve kicks,
    // fists, native paired actions, incoming NPC hits and unrelated producers.
    if(!kcd2vr::geometry40c1::rejectAuthored51c3(caller,I(s,kAttackType),
       frik_pose_probe::CombatExperiment175Enabled(),true,false))return false;
    g_sweep_rejected178r40.fetch_add(1,std::memory_order_relaxed);
    static std::uint64_t last=0;const auto now=std::uint64_t(GetTickCount64());if(now-last>=2000){last=now;char m[200]{};std::snprintf(m,sizeof(m),"V66.178R40 NATIVE_SWEEP_REJECTED count=%llu (F4 contact-owned route: Henry's canned sweep does not hit; the VR blade's contact does)",(unsigned long long)g_sweep_rejected178r40.load());diagnostic_trace::Write(m);}
    return true;}

constexpr std::uint32_t kActionPhaseProp39=0x30;
