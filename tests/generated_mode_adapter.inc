static const char* WeaponControlModeNameImpl(int mode) noexcept;
static void SetWeaponControlModeImpl(int mode) noexcept {
    mode=std::clamp(mode,0,7);
    // Failure must never trap Cycle() at 3 -> failed 4 -> 3 forever.
    // Use the ordinary OFF path so epochs, pending restoration and native
    // cancellation see the same mode transition as a deliberate OFF request.
    if((mode>=4&&!ArmPhysicalWeapons6698())||(FullWeaponMode120(mode)&&!ArmMotionCombatInput101())||(mode==7&&!ArmPrediction120())){
        diagnostic_trace::Write("V66.135R1 PHYSICAL enable rejected: returning F12 to NATIVE/OFF; next USER press=BASELINE_3; native guards retained");
        mode=0;
    }
    if(BowHandlingMode59c9(mode)&&!ArmBowZoom121())diagnostic_trace::Write("V66.121 BOW_ZOOM unavailable: native camera value retained");
    g_contact_control_epoch6695.fetch_add(1,std::memory_order_acq_rel);
    if(g_weapon_control_mode.load()!=0){char end[448]{};
        std::snprintf(end,sizeof(end),"V66.95-F12 SESSION_END rung=%d next=%d applied=%llu contentionSkips=%llu sourceLosses=%llu nativeRestores=%llu restorePending=%d finalVisibility=USER_REPORT",
            g_weapon_control_mode.load(),mode,(unsigned long long)g_weapon_applied6694.load(),(unsigned long long)g_weapon_contended6695.load(),(unsigned long long)g_weapon_lost6695.load(),(unsigned long long)g_weapon_restored6695.load(),diagnostics::lab::weaponRestorePending.load());diagnostic_trace::Write(end);}
    g_weapon_contended6695.store(0);g_weapon_lost6695.store(0);g_weapon_restored6695.store(0);
    g_weapon_request_epoch6694.fetch_add(1,std::memory_order_acq_rel);
    g_weapon_fault6694.store(false,std::memory_order_release);g_weapon_applied6694.store(0);
    if(mode==7&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64()))StartPredictionCapture120();
    g_weapon_reason6694.store(mode?1:0);g_weapon_control_mode.store(mode,std::memory_order_release);kcd2vr::haptics101::select(mode>=4||g_census_mode.load()>=4,std::uint64_t(GetTickCount64()));
    g_f12_attachment_proof_until_ms.store(0);g_f12_follow_requested6689.store(mode>=2);
    if(mode>0&&g_census_mode.load()==5)SetCombatCensusMode(3);
    if(mode>=3&&g_census_mode.load()<3)SetCombatCensusMode(3); // one deliberate combined-mode request, not a repeated override
    char m[512]{};std::snprintf(m,sizeof(m),"V66.94-F12 rung=%d %s previousProofRequired=0 fingerReferenceRequired=0 weaponOnlyNeedsVrik=0 nativeOwnershipRequired=1",mode,WeaponControlModeNameImpl(mode));diagnostic_trace::Write(m);
}
static void CycleWeaponControlModeImpl() noexcept {SetWeaponControlModeImpl((g_weapon_control_mode.load()+1)%8);}
static int WeaponControlModeImpl() noexcept {return g_weapon_control_mode.load(std::memory_order_acquire);}
static const char* WeaponControlModeNameImpl(int mode) noexcept {switch(mode){case 0:return "NATIVE / OFF";case 1:return "NATIVE ITEM/ATTACHMENT DIAGNOSTIC";case 2:return "CONTROLLER WEAPON ONLY / SKELETON SUSPENDED";case 3:return "CONTROLLER WEAPON + HANDS + UPPER BODY";case 4:return "PHYSICAL HANDLING / TWO HANDS / MOUNTS (NO ATTACKS)";case 5:return "TRACKED WEAPON + STATIC CONTACT (NO ATTACKS)";case 6:return "FULL WEAPON TEST / HOLD GRIP / SWINGS / GUARD / BOW / HOLSTERS";case 7:return "TRACKED NATIVE SWEEP EXPERIMENT + FULL WEAPON HANDLING";default:return "INVALID";}}
