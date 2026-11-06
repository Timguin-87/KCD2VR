// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 83
// src/mod.cpp: applymenustate_01.inl
// M2_SLOT_BEGIN 83
    {   // R59.16C8
        if(g_auto_palm59c8.exchange(state.auto_palm,std::memory_order_acq_rel)!=state.auto_palm)frik_pose_probe::SetAutoPalm59c8(state.auto_palm);
        if(g_riposte_native59c8.exchange(state.riposte_native,std::memory_order_acq_rel)!=state.riposte_native)combat_diag_probe::SetRiposteNative59c8(state.riposte_native);
        const float dwell=std::clamp(state.target_dwell_s,0.f,2.f);
        if(std::fabs(g_target_dwell59c8.exchange(dwell,std::memory_order_acq_rel)-dwell)>1e-4f){
            // The lease holds the old values: give them back, rebuild the list, apply again if a combat rung is live.
            const bool was=g_combat_cvars52.applied;char c[1024]{};
            if(was&&g_combat_cvars52.Apply(false,c,sizeof(c)))diagnostic_trace::Write(c);
            g_combat_cvars52.Load(g_player_rules52.load(),g_shared_rules52.load(),g_targeting_lease59c8,g_no_lunge594.load());
            if(was&&g_combat_cvars52.Apply(true,c,sizeof(c)))diagnostic_trace::Write(c);
            char m[120]{};std::snprintf(m,sizeof(m),"R59.16C8-IMMERSION targetSwitchDwell=%.2fs reapplied=%d",dwell,was?1:0);diagnostic_trace::Write(m);}
    }
// M2_SLOT_END 83
#elif KCD2VR_M2_HOST_SLOT == 117
// src/mod.cpp: savemenusettings_01.inl
// M2_SLOT_BEGIN 117
    for(int h=0;h<2;++h)for(int f=0;f<5;++f)for(int k=0;k<4;++k){char key[64];std::snprintf(key,sizeof(key),"VrFinger40_%c_%d_%d",h?'L':'R',f,k);ok=wf(key,finger40::get(h,f,k))&&ok;}
// M2_SLOT_END 117
#elif KCD2VR_M2_HOST_SLOT == 154
// src/mod.cpp: loadconfig_01.inl
// M2_SLOT_BEGIN 154
    frik_pose_probe::SetLab58(false,ReadIniBoolExplicit(explicit_ini,"Settings","VrLabCorpseDrag58",false),false,ReadIniFloatExplicit(explicit_ini,"Settings","VrLabLivingHitK58",6.f)); // D84: remember the explicit corpse choice; living impact/prop wake remain session trials.
    frik_pose_probe::SetLab58LivingTouch(false); // No saved living-NPC request at startup.
// M2_SLOT_END 154
#elif KCD2VR_M2_HOST_SLOT == 156
// src/mod.cpp: loadconfig_02.inl
// M2_SLOT_BEGIN 156
    g_touch43.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrNpcTouchReact",false));g_touch_mass43.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrNpcTouchMass",6.f),.5f,40.f));
    kcd2vr::bodycombo59c7g::npcReactsToBlade.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrNpcReactsToBlade",true)); // NR1
    kcd2vr::bodycombo59c7g::staminaHits.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrNpcReactsStaminaHits",true)); // I10: armour (stamina-only) hits react to the blade spot too
    kcd2vr::finisher_cb1::enabled.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrPhysicalComboFinisher",true)); // CB1 finisher = the table's physical move
    g_fallplay43.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrNpcFallAndPlay",false));g_fallplay_threshold43.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrFallAndPlayThreshold",15.f),1.f,200.f));
// M2_SLOT_END 156
#elif KCD2VR_M2_HOST_SLOT == 159
// src/mod.cpp: loadconfig_03.inl
// M2_SLOT_BEGIN 159
    for(int h=0;h<2;++h)for(int f=0;f<5;++f)for(int k=0;k<4;++k){char key[64];std::snprintf(key,sizeof(key),"VrFinger40_%c_%d_%d",h?'L':'R',f,k);// C42 migration: older lateral tuning also altered the open endpoint.
      // User requests native open hands; retain both saved side-specific closed/spread values.
      finger40::set(h,f,k,(k==0&&!ReadIniBoolExplicit(explicit_ini,"Settings","VrFinger42OpenNativeMigrated",false))?0.f:ReadIniFloatExplicit(explicit_ini,"Settings",key,finger40::defaults59[h][f][k]));}
    if(!ReadIniBoolExplicit(explicit_ini,"Settings","VrFinger47BothHandsMigrated",false)){
        // Never overwrite an independently saved left profile just because an
        // older migration marker is absent. The historical copy is only for
        // installations that have no LEFT closed/spread keys at all.
        bool explicitLeft55=false;char saved55[80]{};
        for(int f=0;f<5;++f)for(int k=1;k<3;++k){char key[64]{};
            std::snprintf(key,sizeof(key),"VrFinger40_L_%d_%d",f,k);
            explicitLeft55=GetPrivateProfileStringA("Settings",key,"",saved55,sizeof(saved55),explicit_ini)>0||explicitLeft55;}
        // C59 missing keys already loaded the separately approved left defaults.
        diagnostic_trace::Write(explicitLeft55?"C55 FINGER_PROFILE left=EXPLICIT_SETTINGS_PRESERVED right=EXPLICIT_SETTINGS open=UNCHANGED":
            "C59 FINGER_PROFILE left=APPROVED_LEFT_DEFAULT right=APPROVED_RIGHT_DEFAULT open=UNCHANGED rawBones=MIRROR_NOT_USED");
    }
    finger40::middleTowardRing59.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrFingerMiddleTowardRingDegrees",3.f),-8.f,8.f));
    for(int h=0;h<2;++h){char line55[640]{};int n=std::snprintf(line55,sizeof(line55),"C55 FINGER_ACTIVE side=%c section=Settings open/closed/spread/baseCurlDeg:",h?'L':'R');
        for(int f=0;f<5&&n>0&&n<int(sizeof(line55));++f)n+=std::snprintf(line55+n,sizeof(line55)-n," %d=%.2f/%.2f/%+.1f/%+.1f",f,
            finger40::get(h,f,0),finger40::get(h,f,1),finger40::get(h,f,2),finger40::get(h,f,3));
        diagnostic_trace::Write(line55);}
// M2_SLOT_END 159
#elif KCD2VR_M2_HOST_SLOT == 160
// src/mod.cpp: loadconfig_04.inl
// M2_SLOT_BEGIN 160
    {char line[192]{};std::snprintf(line,sizeof(line),"C32 CONFIG mapRegion=%s clinchInitiationMeters=%.3f (0=native; active paired actions unchanged)",
        printedMapRegion32==1?"Kuttenberg":"Trosky",std::isfinite(clinchRadius32)&&clinchRadius32>=.05f&&clinchRadius32<=2.f?clinchRadius32:0.f);
        diagnostic_trace::Write(line);}
// M2_SLOT_END 160
#elif KCD2VR_M2_HOST_SLOT == 161
// src/mod.cpp: loadconfig_05.inl
// M2_SLOT_BEGIN 161
    g_phys_hand_mass59c10r8.store(ReadIniFloatExplicit(explicit_ini,"Settings","VrPhysHandMass",3.f));
    g_phys_blade_mass59c10r8.store(ReadIniFloatExplicit(explicit_ini,"Settings","VrPhysBladeMass",4.f));
// M2_SLOT_END 161
#elif KCD2VR_M2_HOST_SLOT == 184
// src/mod.cpp: loadconfig_06.inl
// M2_SLOT_BEGIN 184
    g_exp59c10.strikeRefJ.store(ReadIniFloatExplicit(explicit_ini,"Settings","VrStrikeReferenceJ",75.f));frik_pose_probe::SetStrikeReferenceJ59c9(g_exp59c10.strikeRefJ.load()); // R59.16C9C
    g_exp59c10.headRaiseCm.store(ReadIniFloatExplicit(explicit_ini,"Settings","VrHeadRaiseMaxCm",18.f));frik_pose_probe::SetHeadRaiseMax59c9(g_exp59c10.headRaiseCm.load()); // R59.16C9C
// M2_SLOT_END 184
#elif KCD2VR_M2_HOST_SLOT == 185
// src/mod.cpp: loadconfig_07.inl
// M2_SLOT_BEGIN 185
    g_target_dwell59c8.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrTargetSwitchDwell",0.4f),0.f,2.f));
// M2_SLOT_END 185
#elif KCD2VR_M2_HOST_SLOT == 193
// src/mod.cpp: loadconfig_08.inl
// M2_SLOT_BEGIN 193
    g_exp59c10.strikeMinScale.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrStrikeMinScale",0.35f),0.f,1.f));
// M2_SLOT_END 193
#elif KCD2VR_M2_HOST_SLOT == 197
// src/mod.cpp: loadconfig_09.inl
// M2_SLOT_BEGIN 197
    { // V66.178R59.6: VrAttackBackend=vr selects the motion-owned backend WITH consequences (VR state driver + direct hit route)
        char backend[32]{};GetPrivateProfileStringA("Settings","VrAttackBackend","native",backend,DWORD(sizeof(backend)),explicit_ini);
        const bool vr=_stricmp(backend,"vr")==0;
        if(vr)motion59c4::previewRequested.store(true);
        combat_diag_probe::SetVrStateConsequence596(vr);
        g_contact_impulse596.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrContactImpulse",true));
        g_contact_impulse_scale596.store(ReadIniFloatExplicit(explicit_ini,"Settings","VrContactImpulseScale",1.f));
        g_contact_impulse_cap596.store(ReadIniFloatExplicit(explicit_ini,"Settings","VrContactImpulseCapNs",40.f));
    }
// M2_SLOT_END 197
#elif KCD2VR_M2_HOST_SLOT == 198
// src/mod.cpp: loadconfig_10.inl
// M2_SLOT_BEGIN 198
    {const float mh=ReadIniFloatExplicit(explicit_ini,"Settings","VrMasterHandoffForwardSpeed",3.5f);g_exp59c10.msHandoff23.store(mh<0.f||!std::isfinite(mh)?-1.f:std::clamp(mh,1.5f,4.5f));}
// M2_SLOT_END 198
#elif KCD2VR_M2_HOST_SLOT == 200
// src/mod.cpp: loadconfig_11.inl
// M2_SLOT_BEGIN 200
    frik_pose_probe::SetMasterStrikeAllWeapons59(ReadIniBoolExplicit(explicit_ini,"Settings","VrMasterStrikeAllWeapons",true)); // V66.178R59 (needs the companion pak rows)
// M2_SLOT_END 200
#elif KCD2VR_M2_HOST_SLOT == 290
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 290
    bool  exp_master_strike_game = true;    // VrMasterStrikeVeto=0: the game decides master strikes (no VR veto)
// M2_SLOT_END 290
#elif KCD2VR_M2_HOST_SLOT == 293
// src/vr_menu.h: menu_state_02.inl
// M2_SLOT_BEGIN 293
    bool  exp_ms_keep_stance = true;        // VrMasterStrikeKeepStance
// M2_SLOT_END 293
#elif KCD2VR_M2_HOST_SLOT == 296
// src/vr_menu.h: menu_state_03.inl
// M2_SLOT_BEGIN 296
    bool  exp_counter_stab40 = true;       // C40 VrDirectCounterStab: strong opposite-side stab into his attack start (F4 DIRECT); C44 default ON, perk-gated
    float exp_counter_stab_energy40 = 0.6f; // C40 VrDirectCounterStabEnergy: stroke energy / reference stroke (0.2..2.0)
    float exp_counter_stab_mul40 = 1.5f;    // C40 VrDirectCounterStabStrength: x stroke power, capped by the power rule (1.0..2.0)
    int   exp_counter_stab_window40 = 500;  // C40 VrDirectCounterStabWindowMs: after his attack start (150..1000)
    bool  exp_counter_stab_perk44 = true;   // C44 VrDirectCounterStabNeedsPerk: only with the game's Master strike perk (soul ability 73)
// M2_SLOT_END 296
#elif KCD2VR_M2_HOST_SLOT == 298
// src/vr_menu.h: menu_state_04.inl
// M2_SLOT_BEGIN 298
    int   exp_ms_stab_only47 = 1;           // 0 native, 1 opposite qualified CUT/STAB, 2 paired off, 3 R120 INSTANT (saved VrMasterStrikeModeR120)
    float exp_ms_stab_energy47 = .30f;      // Legacy saved value retained; no additional masterstrike energy veto
    bool  exp_ms_test_force = false;        // DS-F5 VrMasterStrikeTestForce: TEST receipts at every attack press (no forced window/outcome)
// M2_SLOT_END 298
#elif KCD2VR_M2_HOST_SLOT == 302
// src/vr_menu.h: menu_state_05.inl
// M2_SLOT_BEGIN 302
    bool  exp_combo_window_extend = true; // VrComboWindowExtend: qualified Henry timer extension
    int   exp_combo_window_ms = 1500;       // VrComboWindowMs (0 = native combos only)
    bool  exp_combo_recovery_hold = false;   // I12/N2 VrComboRecoveryHold: keep the attack tail alive while the slot is open
    bool  exp_combo_zone_assist = true;      // I12 VrComboZoneAssist: in an open native slot, announce the next recipe zone
    bool  exp_combo_test_force = false;     // DS-F6 VrComboTestForce: TEST - hold the combo slot 5 s + C69 receipts (game still decides)
// M2_SLOT_END 302
#elif KCD2VR_M2_HOST_SLOT == 309
// src/vr_menu.h: menu_state_06.inl
// M2_SLOT_BEGIN 309
    float exp_ms_handoff23 = 3.5f;          // C24b VrMasterHandoffForwardSpeed (-1 = guard exit speed; 1.5..4.5 m/s)
// M2_SLOT_END 309
#else
#error Unknown M2 host contribution slot
#endif
