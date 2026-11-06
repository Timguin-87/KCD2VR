// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 26
// src/mod.cpp: fillmenustate_01.inl
// M2_SLOT_BEGIN 26
    out.touch_attack_direction = enable_touch_attack_direction_.load(std::memory_order_acquire);
    out.npc_difficulty_percent = npc_difficulty::difficulty.load(std::memory_order_relaxed)*100.f;
    out.npc_defense_speed = npc_defense::speed.load(std::memory_order_relaxed);
    out.npc_difficulty_ready = npc_difficulty::Ready();
    out.npc_defense_ready = npc_defense::Ready();
    out.npc_unarmed_spacing_percent = npc_unarmed_spacing::scale.load(std::memory_order_relaxed)*100.f;
    out.npc_unarmed_spacing_ready = npc_unarmed_spacing::Ready();
    out.npc_unarmed_spacing_failure = npc_unarmed_spacing::Failure();
// M2_SLOT_END 26
#elif KCD2VR_M2_HOST_SLOT == 73
// src/mod.cpp: applymenustate_01.inl
// M2_SLOT_BEGIN 73
    enable_touch_attack_direction_.store(state.touch_attack_direction, std::memory_order_release);
    npc_difficulty::difficulty.store(std::isfinite(state.npc_difficulty_percent)?std::clamp(state.npc_difficulty_percent,0.f,100.f)*.01f:0.f,std::memory_order_relaxed);
    npc_defense::speed.store(std::isfinite(state.npc_defense_speed)?std::clamp(state.npc_defense_speed,1.f,3.f):1.f,std::memory_order_relaxed);
    npc_unarmed_spacing::scale.store(std::isfinite(state.npc_unarmed_spacing_percent)?std::clamp(state.npc_unarmed_spacing_percent,25.f,100.f)*.01f:1.f,std::memory_order_relaxed);
    {auto speed109=[](float v){return std::isfinite(v)?std::clamp(v,1.f,3.f):1.f;}; // R109 MISC 1: the D34 per-type NPC keys (same clamp as the INI load)
     npc_defense::blockSpeed.store(speed109(state.npc_block_anim_speed109),std::memory_order_relaxed);npc_defense::parrySpeed.store(speed109(state.npc_parry_anim_speed109),std::memory_order_relaxed);
     npc_defense::dodgeSpeed.store(speed109(state.npc_dodge_anim_speed109),std::memory_order_relaxed);npc_defense::raiseExperiment.store(state.npc_defense_raise109,std::memory_order_relaxed);}
// M2_SLOT_END 73
#elif KCD2VR_M2_HOST_SLOT == 81
// src/mod.cpp: applymenustate_02.inl
// M2_SLOT_BEGIN 81
    {const float pr=std::isfinite(state.exp_npc_proximity_radius36)?std::clamp(state.exp_npc_proximity_radius36,.2f,.6f):.4f;if(std::fabs(frik_pose_probe::NpcProximityRadius36()-pr)>1e-4f)frik_pose_probe::SetNpcProximityRadius36(pr);} // C49 first EXPERIMENTS page
// M2_SLOT_END 81
#elif KCD2VR_M2_HOST_SLOT == 88
// src/mod.cpp: applymenustate_03.inl
// M2_SLOT_BEGIN 88
    touch_bindings_.SetAttackDirectionMouseEmulation(state.touch_attack_direction);
// M2_SLOT_END 88
#elif KCD2VR_M2_HOST_SLOT == 108
// src/mod.cpp: savemenusettings_01.inl
// M2_SLOT_BEGIN 108
    ok = wb("EnableTouchAttackDirectionMouse", enable_touch_attack_direction_.load(std::memory_order_acquire)) && ok;
// M2_SLOT_END 108
#elif KCD2VR_M2_HOST_SLOT == 118
// src/mod.cpp: savemenusettings_02.inl
// M2_SLOT_BEGIN 118
    ok = wi("VrDirectNpcBlock",g_npc_block39.load()) && ok; // C39
    ok = wf("VrNpcDifficulty",npc_difficulty::difficulty.load()*100.f) && ok;
    ok = wf("VrNpcDefenseSpeed",npc_defense::speed.load()) && ok;
    ok = wi("VrNpcDefenseSpeedVersion",2) && ok; // I11: undo only the version-1 1.5 setting
    ok = wf("VrNpcBlockAnimSpeed",npc_defense::blockSpeed.load()) && ok;   // D34: used only with the raise experiment
    ok = wf("VrNpcParryAnimSpeed",npc_defense::parrySpeed.load()) && ok;   // D34: used only with the raise experiment
    ok = wf("VrNpcDodgeAnimSpeed",npc_defense::dodgeSpeed.load()) && ok;   // D34: NPC dodge clip rate
    ok = wi("VrNpcDefenseRaiseExperiment",npc_defense::raiseExperiment.load()?1:0) && ok; // D34: moves contact earlier
    ok = wf("VrNpcUnarmedSpacingPercent",npc_unarmed_spacing::scale.load()*100.f) && ok;
    ok = wi("VrNpcUnarmedSpacingVersion",npc_unarmed_spacing::settingsVersion) && ok;
    ok = wi("VrCombatRouter",kcd2vr::routerH3::mode.load()) && ok;
    ok = wb("VrHybridTimingAssist",false) && ok; // H18 retired auto-arm; F12 mode2 alone leases globals
    ok = wf("VrWin_PLSlotDurationMod",kcd2vr::routerH3::playerSlotModifier.load()) && ok;
    ok = wf("VrWin_BlockSpeedMod",kcd2vr::routerH3::blockSpeedModifier.load()) && ok;
    ok = wf("VrWin_ComboSlotDuration",kcd2vr::routerH3::comboSeconds.load()) && ok;
    ok = wf("VrWin_RiposteSlotDuration",kcd2vr::routerH3::riposteSeconds.load()) && ok;
    ok = wf("VrWin_StandardSlotDuration",kcd2vr::routerH3::standardSeconds.load()) && ok;
    ok = wf("VrWin_AISlotDurationMod",kcd2vr::routerH3::aiSlotModifier.load()) && ok;
// M2_SLOT_END 118
#elif KCD2VR_M2_HOST_SLOT == 136
// src/mod.cpp: loadconfig_01.inl
// M2_SLOT_BEGIN 136
    settings.bind("EnableTouchAttackDirectionMouse", enable_touch_attack_direction_, true);
// M2_SLOT_END 136
#elif KCD2VR_M2_HOST_SLOT == 155
// src/mod.cpp: loadconfig_02.inl
// M2_SLOT_BEGIN 155
    g_npc_block39.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrDirectNpcBlock",0),0,3)); // C39
    {const float v=ReadIniFloatExplicit(explicit_ini,"Settings","VrNpcDifficulty",43.f); // R124: default 43 (Miki's INI)
     npc_difficulty::difficulty.store(std::isfinite(v)?std::clamp(v,0.f,100.f)*.01f:0.f);}
    {const int version=ReadIniIntExplicit(explicit_ini,"Settings","VrNpcDefenseSpeedVersion",0);
     const float saved=ReadIniFloatExplicit(explicit_ini,"Settings","VrNpcDefenseSpeed",1.75f); // R124: default 1.75 (Miki's INI)
     const float v=version==1&&saved==1.5f?1.f:saved; // I11: version-2 custom values remain unchanged
     npc_defense::speed.store(std::isfinite(v)?std::clamp(v,1.f,3.f):1.f);}
    {auto load=[&](const char* key,std::atomic<float>& out,float fallback){
         const float v=ReadIniFloatExplicit(explicit_ini,"Settings",key,fallback);
         out.store(std::isfinite(v)?std::clamp(v,1.f,3.f):1.f);};
     load("VrNpcBlockAnimSpeed",npc_defense::blockSpeed,1.4f);
     load("VrNpcParryAnimSpeed",npc_defense::parrySpeed,1.33f);
     load("VrNpcDodgeAnimSpeed",npc_defense::dodgeSpeed,1.25f);
     npc_defense::raiseExperiment.store(ReadIniIntExplicit(explicit_ini,"Settings","VrNpcDefenseRaiseExperiment",0)!=0);}
    {const int version=ReadIniIntExplicit(explicit_ini,"Settings","VrNpcUnarmedSpacingVersion",0);
     const float saved=ReadIniFloatExplicit(explicit_ini,"Settings","VrNpcUnarmedSpacingPercent",npc_unarmed_spacing::defaultPercent);
     // Migrate only the old untouched native default. Preserve existing custom
     // percentages; after the normal settings save, explicit100 remains native.
     const float v=version<npc_unarmed_spacing::settingsVersion&&saved==100.f?npc_unarmed_spacing::defaultPercent:saved;
     npc_unarmed_spacing::scale.store(std::isfinite(v)?std::clamp(v,25.f,100.f)*.01f:1.f);}
    kcd2vr::routerH3::mode.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrCombatRouter",0),0,2));
    // H18 legacy VrHybridTimingAssist is intentionally not an activation grant.
    kcd2vr::routerH3::playerSlotModifier.store(kcd2vr::routerH3::setting(ReadIniFloatExplicit(explicit_ini,"Settings","VrWin_PLSlotDurationMod",.2f),0.f,1.f,.2f));
    kcd2vr::routerH3::blockSpeedModifier.store(kcd2vr::routerH3::setting(ReadIniFloatExplicit(explicit_ini,"Settings","VrWin_BlockSpeedMod",1.4f),.8f,2.4f,1.4f));
    kcd2vr::routerH3::comboSeconds.store(kcd2vr::routerH3::setting(ReadIniFloatExplicit(explicit_ini,"Settings","VrWin_ComboSlotDuration",3.f),.5f,3.f,3.f));
    kcd2vr::routerH3::riposteSeconds.store(kcd2vr::routerH3::setting(ReadIniFloatExplicit(explicit_ini,"Settings","VrWin_RiposteSlotDuration",1.f),.1f,1.5f,1.f));
    kcd2vr::routerH3::standardSeconds.store(kcd2vr::routerH3::setting(ReadIniFloatExplicit(explicit_ini,"Settings","VrWin_StandardSlotDuration",1.5f),.1f,2.f,1.5f));
    kcd2vr::routerH3::aiSlotModifier.store(kcd2vr::routerH3::setting(ReadIniFloatExplicit(explicit_ini,"Settings","VrWin_AISlotDurationMod",.2f),0.f,1.f,.2f));
// M2_SLOT_END 155
#elif KCD2VR_M2_HOST_SLOT == 175
// src/mod.cpp: loadconfig_03.inl
// M2_SLOT_BEGIN 175
    enable_touch_attack_direction_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableTouchAttackDirectionMouse", true), std::memory_order_relaxed);
// M2_SLOT_END 175
#elif KCD2VR_M2_HOST_SLOT == 194
// src/mod.cpp: loadconfig_04.inl
// M2_SLOT_BEGIN 194
    native_input_look_lab::SetStickDirectionKeys(ReadIniBoolExplicit(explicit_ini,"Settings","VrPadStickDirectionKeys",true));
// M2_SLOT_END 194
#elif KCD2VR_M2_HOST_SLOT == 195
// src/mod.cpp: loadconfig_05.inl
// M2_SLOT_BEGIN 195
    combat_diag_probe::SetGeometryTelegraph178r34(ReadIniBoolExplicit(explicit_ini,"Settings","VrGeometryTelegraph",false)); // V66.178R34; R36 default OFF (ChatGPT R34 review F02: raw property writes skip the native setters/notifications; the live action writes them itself)
// M2_SLOT_END 195
#elif KCD2VR_M2_HOST_SLOT == 196
// src/mod.cpp: loadconfig_06.inl
// M2_SLOT_BEGIN 196
    combat_diag_probe::SetGeometryDirectFallback178r37(ReadIniBoolExplicit(explicit_ini,"Settings","VrGeometryDirectFallback",false)); // V66.178R37
// M2_SLOT_END 196
#elif KCD2VR_M2_HOST_SLOT == 199
// src/mod.cpp: loadconfig_07.inl
// M2_SLOT_BEGIN 199
    frik_pose_probe::SetDirectionalStabs178r29(ReadIniBoolExplicit(explicit_ini,"Settings","VrDirectionalStabs",false)); // V66.178R29: thrust = stab zone
// M2_SLOT_END 199
#elif KCD2VR_M2_HOST_SLOT == 201
// src/mod.cpp: loadconfig_08.inl
// M2_SLOT_BEGIN 201
    {   // R59.16C10R12: rung 3 overdue contacts go direct without an admitted release; rung 4 may start a strike from the tail of its own
        // previous strike (native phase 16/32 within 1.5 s) and may publish a strike when the defense-row query returns no row.
        g_exp59c10.syncOverdueAny.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrSyncOverdueDirect",true));
        g_exp59c10.recoveryOverride.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDirectStrikeInRecovery",true));
        g_exp59c10.noRowFallback.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDirectHitWithoutDefenseRow",true));
        combat_diag_probe::SetRung3Switches59c12(g_exp59c10.syncOverdueAny.load(),g_exp59c10.recoveryOverride.load(),g_exp59c10.noRowFallback.load());
        g_exp59c10.reclaimGuard.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrReclaimPhaseFromOwnGuard",true)); // R59.16C10R16
        {int sr44=ReadIniIntExplicit(explicit_ini,"Settings","VrDirectStaggerRule",-1); // C44 (an old INI with VrStrikeDuringStagger=true maps to rule 0)
         if(sr44<0)sr44=ReadIniBoolExplicit(explicit_ini,"Settings","VrStrikeDuringStagger",false)?0:1;
         sr44=kcd2vr::direct_counter39::clampStaggerRule44(sr44);g_exp59c10.staggerRule44.store(sr44);kcd2vr::direct_counter39::staggerRule44.store(sr44,std::memory_order_relaxed);
         g_exp59c10.strikeDuringStagger.store(kcd2vr::direct_counter39::staggerStrikeAllowed44(sr44));}
        combat_diag_probe::SetRung4Switches59c16(g_exp59c10.reclaimGuard.load(),g_exp59c10.strikeDuringStagger.load());
    }
    g_exp59c10.telegraphWindup.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrTelegraphWindup",true));frik_pose_probe::SetTelegraphWindup59c10(g_exp59c10.telegraphWindup.load()); // R59.16C10
    g_exp59c10.telegraphStrike.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrTelegraphStrike",true)); // H21: optional readiness receipt only; R124 default ON (Miki's INI)
// M2_SLOT_END 201
#elif KCD2VR_M2_HOST_SLOT == 202
// src/mod.cpp: loadconfig_09.inl
// M2_SLOT_BEGIN 202
    if(ReadIniIntExplicit(explicit_ini,"Settings","VrRung4PreferenceVersion",0)<1){
        // H21: do not rewrite the legacy offer-observation preference.
        char eta[32]{};std::snprintf(eta,sizeof(eta),"%.2f",double(g_exp59c10.npcEtaPrepare.load()));(void)WritePrivateProfileStringA("Settings","VrNpcBlockEtaPrepare",eta,explicit_ini);
        std::snprintf(eta,sizeof(eta),"%.2f",double(g_exp59c10.npcEtaStrike.load()));(void)WritePrivateProfileStringA("Settings","VrNpcBlockEtaStrike",eta,explicit_ini);
        (void)WritePrivateProfileStringA("Settings","VrRung4PreferenceVersion","1",explicit_ini);}
// M2_SLOT_END 202
#elif KCD2VR_M2_HOST_SLOT == 203
// src/mod.cpp: loadconfig_10.inl
// M2_SLOT_BEGIN 203
    if(ReadIniIntExplicit(explicit_ini,"Settings","VrRung4PreferenceVersion",0)<2){
        // H21: retire the forced-true phase8 migration. Saved true observes
        // readiness only; motion Commit/contact still owns phase8.
        (void)WritePrivateProfileStringA("Settings","VrRung4PreferenceVersion","2",explicit_ini);}
// M2_SLOT_END 203
#elif KCD2VR_M2_HOST_SLOT == 204
// src/mod.cpp: loadconfig_11.inl
// M2_SLOT_BEGIN 204
    combat_diag_probe::SetTelegraphStrike59c10(g_exp59c10.telegraphStrike.load());
    g_exp59c10.directBlock39.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDirectNpcBlockBySkill",true)); // C39
// M2_SLOT_END 204
#elif KCD2VR_M2_HOST_SLOT == 205
// src/mod.cpp: loadconfig_12.inl
// M2_SLOT_BEGIN 205
    combat_diag_probe::SetDirectBlockTiming39(g_exp59c10.directBlock39.load(),g_exp59c10.directBlockLo39.load(),g_exp59c10.directBlockHi39.load());
// M2_SLOT_END 205
#elif KCD2VR_M2_HOST_SLOT == 236
// src/mod.h: mod_state_01.inl
// M2_SLOT_BEGIN 236
    std::atomic<bool> enable_touch_attack_direction_{true};
// M2_SLOT_END 236
#elif KCD2VR_M2_HOST_SLOT == 270
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 270
    bool touch_attack_direction = true;
// M2_SLOT_END 270
#elif KCD2VR_M2_HOST_SLOT == 281
// src/vr_menu.h: menu_state_02.inl
// M2_SLOT_BEGIN 281
    bool exp_telegraph_windup = true;       // VrTelegraphWindup
    bool exp_telegraph_strike = true;       // VrTelegraphStrike: bounded readiness receipt, no phase authority
// M2_SLOT_END 281
#elif KCD2VR_M2_HOST_SLOT == 283
// src/vr_menu.h: menu_state_03.inl
// M2_SLOT_BEGIN 283
    float exp_npc_eta_prepare = 0.25f;      // VrNpcBlockEtaPrepare (R59.16C10R5, S33)
// M2_SLOT_END 283
#elif KCD2VR_M2_HOST_SLOT == 285
// src/vr_menu.h: menu_state_04.inl
// M2_SLOT_BEGIN 285
    int   npc_block39 = 0;                  // C39 VrDirectNpcBlock: 0 native, 1 often, 2 always, 3 always perfect
    float exp_npc_proximity_radius36 = 0.40f; // NPC reference radius, meters; observation until defense boundary is proven
    float exp_npc_eta_strike = 0.12f;       // VrNpcBlockEtaStrike
    bool  exp_direct_block_timing39 = true; // C39 VrDirectNpcBlockBySkill: VR DIRECT announces a skill-shortened contact ETA to a waiting opponent
    float exp_direct_block_lo39 = 2.0f;     // C39 VrDirectNpcBlockSpeedLow: block-raise speed-up at NPC skill 0 (1..3)
    float exp_direct_block_hi39 = 3.0f;     // C39 VrDirectNpcBlockSpeedHigh: block-raise speed-up at NPC skill 30 (1..3)
    float npc_difficulty_percent = 43.f;   // VrNpcDifficulty: native current values -> exact C8 AI inputs
    float npc_defense_speed = 1.75f;       // VrNpcDefenseSpeed: NPC defenses, independent of Henry's windows
    bool npc_difficulty_ready = false, npc_defense_ready = false;
    float npc_unarmed_spacing_percent = 60.f; // Native movement base; lower asks unarmed NPC goals closer
    bool npc_unarmed_spacing_ready = false;
    const char* npc_unarmed_spacing_failure = "NOT_REQUESTED";
// M2_SLOT_END 285
#elif KCD2VR_M2_HOST_SLOT == 288
// src/vr_menu.h: menu_state_05.inl
// M2_SLOT_BEGIN 288
    bool  exp_sync_overdue_direct = true;   // VrSyncOverdueDirect (R59.16C10R12, COMBAT TUNING 2)
    bool  exp_strike_in_recovery = true;    // VrDirectStrikeInRecovery (R12)
    bool  exp_hit_without_row = true;       // VrDirectHitWithoutDefenseRow (R12)
    bool  exp_reclaim_guard = true;         // VrReclaimPhaseFromOwnGuard (R59.16C10R16, F4 press 4)
    int   exp_stagger_rule44 = 1;           // C44 VrDirectStaggerRule: 0 strike out of stagger, 1 no DIRECT hit while staggered, 2 half strength (replaces VrStrikeDuringStagger)
// M2_SLOT_END 288
#elif KCD2VR_M2_HOST_SLOT == 295
// src/vr_menu.h: menu_state_06.inl
// M2_SLOT_BEGIN 295
    bool  exp_direct_stagger_guard39 = false; // C39 VrDirectGuardOutOfStagger (F4 DIRECT)
// M2_SLOT_END 295
#elif KCD2VR_M2_HOST_SLOT == 300
// src/vr_menu.h: menu_state_07.inl
// M2_SLOT_BEGIN 300
    bool  exp_direct_dirblock39 = true; // C39 VrDirectNpcDirectionalBlock: waiting enemy is told the blade's real approach sector (F4 DIRECT)
// M2_SLOT_END 300
#elif KCD2VR_M2_HOST_SLOT == 304
// src/vr_menu.h: menu_state_08.inl
// M2_SLOT_BEGIN 304
    bool  exp_own_hits18 = true;            // VrDirectOwnsHits (C10R18, F4 press 4)
// M2_SLOT_END 304
#elif KCD2VR_M2_HOST_SLOT == 310
// src/vr_menu.h: menu_state_09.inl
// M2_SLOT_BEGIN 310
    float exp_npc_block_speed18 = 1.0f;     // VrNpcBlockSpeed (C10R19: 1.0 = native NPC guard timing, >1 experimental earlier guard; 0 = legacy ETA)
// M2_SLOT_END 310
#elif KCD2VR_M2_HOST_SLOT == 5008
// src/mod.cpp: npc_block_lease
// M2_SLOT_BEGIN 5008
static std::atomic<int> g_npc_block39{0};
struct NpcBlock39 {
    void Service(bool direct,int mode,std::uint64_t,RagdollConsole59&) noexcept {
        static int last=-1;const int state=direct?mode:0;if(state==last)return;last=state;
        if(state>0)diagnostic_trace::Write("C40 NPC_FORCE_BLOCK unavailable=GLOBAL_CVAR_AFFECTS_OTHER_FIGHTS requested=1 writes=NONE nativeDecision=UNCHANGED");
    }
};
static NpcBlock39 g_npc_block_state39;
struct CombatWindowsConsoleH3 : RagdollConsole59 {
    bool Qualified()const noexcept {return retail_build112::Ready(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")));}
    bool Read(const char* name,float& value)noexcept {return Qualified()&&RagdollConsole59::Read(name,value);}
    bool Write(const char* name,float value)noexcept {return Qualified()&&RagdollConsole59::Write(name,value);}
};
static kcd2vr::routerH3::Windows g_router_windows_h3;
static kcd2vr::dodge_window_d70::Lease g_dodge_window_d70;
// M2_SLOT_END 5008
#elif KCD2VR_M2_HOST_SLOT == 9010
// Existing Mod control thread: no F3/F4 dependency, no per-frame install retry.
// M2_SLOT_BEGIN 9010
    const float spacing=npc_unarmed_spacing::scale.load(std::memory_order_relaxed);
    const auto spacingModule=spacing<1.f&&!npc_unarmed_spacing::Ready()?
        reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll")):0;
    if(spacing!=npcSpacingAttempt||spacingModule!=npcSpacingModule){npcSpacingAttempt=spacing;npcSpacingModule=spacingModule;
        if(spacing<1.f&&!npc_unarmed_spacing::Ready()){
            const bool ok=npc_unarmed_spacing::Install(spacingModule);
            if(ok)diagnostic_trace::Write("NPC unarmed preferred-distance base control ready; native tactical overrides retained");
            else {char line[192]{};std::snprintf(line,sizeof(line),"NPC unarmed distance unavailable reason=%s; native movement retained",npc_unarmed_spacing::Failure());diagnostic_trace::Write(line);}}}
// M2_SLOT_END 9010
#else
#error Unknown M2 host contribution slot
#endif
