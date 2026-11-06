// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 34
// src/mod.cpp: fillmenustate_01.inl
// M2_SLOT_BEGIN 34
    out.pommel_strike_h8=kcd2vr::physical::pommelStrikeDefault69.load();out.kick_assist_h8=kcd2vr::kick26::assistCm72.load();out.impale_main_probe_h8=kcd2vr::claude_h8::impaleMainProbe.load();
    out.clinch_distance32=frik_pose_probe::ClinchDistanceMeters32();
// M2_SLOT_END 34
#elif KCD2VR_M2_HOST_SLOT == 80
// src/mod.cpp: applymenustate_01.inl
// M2_SLOT_BEGIN 80
    kcd2vr::physical::pommelStrikeDefault69.store(state.pommel_strike_h8);kcd2vr::kick26::assistCm72.store(std::clamp(state.kick_assist_h8,0,30));kcd2vr::claude_h8::impaleMainProbe.store(state.impale_main_probe_h8);
    frik_pose_probe::SetClinchDistanceMeters32(state.clinch_distance32);
// M2_SLOT_END 80
#elif KCD2VR_M2_HOST_SLOT == 116
// src/mod.cpp: savemenusettings_01.inl
// M2_SLOT_BEGIN 116
    ok = wb("VrPommelStrike",kcd2vr::physical::pommelStrikeDefault69.load()) && ok;ok = wi("VrKickAssist",kcd2vr::kick26::assistCm72.load()) && ok;ok = wb("VrImpaleMainProbeH8",kcd2vr::claude_h8::impaleMainProbe.load()) && ok;
    ok = wf("VrClinchDistanceMeters32",frik_pose_probe::ClinchDistanceMeters32()) && ok;
// M2_SLOT_END 116
#elif KCD2VR_M2_HOST_SLOT == 158
// src/mod.cpp: loadconfig_01.inl
// M2_SLOT_BEGIN 158
    kcd2vr::physical::pommelStrikeDefault69.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrPommelStrike",true));
    kcd2vr::kick26::assistCm72.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrKickAssist",0),0,30));
    kcd2vr::claude_h8::impaleMainProbe.store(kcd2vr::developer_mode::Enabled()&&ReadIniBoolExplicit(explicit_ini,"Settings","VrImpaleMainProbeH8",false));
    float clinchRadius32=ReadIniFloatExplicit(explicit_ini,"Settings","VrClinchDistanceMeters32",.40f);
    if(ReadIniIntExplicit(explicit_ini,"Settings","VrClinchDefaultD56",0)<1){
        // Only the inherited old default migrates. Native(0) and custom choices survive.
        bool savedD56=true;
        if(std::isfinite(clinchRadius32)&&std::fabs(clinchRadius32-.55f)<.0001f){
            clinchRadius32=.40f;savedD56=WritePrivateProfileStringA("Settings","VrClinchDistanceMeters32","0.40",explicit_ini)!=FALSE;
        }
        if(savedD56)(void)WritePrivateProfileStringA("Settings","VrClinchDefaultD56","1",explicit_ini);
    }
    frik_pose_probe::SetClinchDistanceMeters32(clinchRadius32);
// M2_SLOT_END 158
#elif KCD2VR_M2_HOST_SLOT == 210
// src/mod.cpp: loadconfig_02.inl
// M2_SLOT_BEGIN 210
    combat_rules178r10::kickTransport.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrKickTransport",3),0,3));
// M2_SLOT_END 210
#elif KCD2VR_M2_HOST_SLOT == 274
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 274
    bool pommel_strike_h8=true,impale_main_probe_h8=false;int kick_assist_h8=0;
    float clinch_distance32 = .40f;        // new current-pair initiation limit;0 native
// M2_SLOT_END 274
#elif KCD2VR_M2_HOST_SLOT == 284
// src/vr_menu.h: menu_state_02.inl
// M2_SLOT_BEGIN 284
    bool  npc_touch43 = false;              // C43T VrNpcTouchReact
    bool  npc_fallplay43 = false;           // C43T VrNpcFallAndPlay
// M2_SLOT_END 284
#elif KCD2VR_M2_HOST_SLOT == 306
// src/vr_menu.h: menu_state_03.inl
// M2_SLOT_BEGIN 306
    int   kick_style26 = 0;                 // C26 VrKickStyle: 0 game kick, 1 VR leg + game rules, 2 VR leg only (no effect)
    int   kick_leg26 = 2;                   // C26 VrKickLeg: 0 right, 1 left, 2 alternate
    int   kick_reach_ms26 = 730;            // C26 VrKickReachMs: native foot reach after the kick clip starts
// M2_SLOT_END 306
#elif KCD2VR_M2_HOST_SLOT == 308
// src/vr_menu.h: menu_state_04.inl
// M2_SLOT_BEGIN 308
    bool  kick_special33 = true;            // C33 VrKickSpecialInput: armed R3 = the game special attack (zone-5 kick rows)
    bool  kick_fallback33 = true;           // H26 VrKickFallbackLeg: explicit VR LEG style1 only
    bool  kick_real_hit37c2 = false;        // C37C2 VrKickRealHitTest: VR LEG CONTACT PROBE applies one native kick hit (test)
    int kick_profile52 = 0; // Normal-kick table reference profile; 1 = old 520-ms visual
    int kick_reach52 = 92; // Visual leg reach % at fixed anatomical lengths
// M2_SLOT_END 308
#else
#error Unknown M2 host contribution slot
#endif
