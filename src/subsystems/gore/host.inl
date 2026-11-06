// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 8
// src/mod.cpp: fillmenustate_01.inl
// M2_SLOT_BEGIN 8
    out.bloody_fights49=frik_pose_probe::BloodyFights49(); // C49
// M2_SLOT_END 8
#elif KCD2VR_M2_HOST_SLOT == 30
// src/mod.cpp: fillmenustate_02.inl
// M2_SLOT_BEGIN 30
    out.decapitation = g_decapitation59c7e.load(std::memory_order_acquire); // R59.16C7E
    out.dismemberment_d80 = kcd2vr::gorepak50::dismember.load(std::memory_order_acquire);
    out.decap_style44 = kcd2vr::gore32::decapStyle44.load(); // C44
    out.impale_hang = g_impale_hang32.load(); // C32
    out.impale_hang_on45 = g_impale_hang_on45.load(); // C45
    out.exp_impale40 = kcd2vr::impale40::mode.load(); // C40
// M2_SLOT_END 30
#elif KCD2VR_M2_HOST_SLOT == 71
// src/mod.cpp: applymenustate_01.inl
// M2_SLOT_BEGIN 71
    if(state.bloody_fights49!=frik_pose_probe::BloodyFights49())frik_pose_probe::SetBloodyFights49(state.bloody_fights49); // C49 BLOODY FIGHTS
// M2_SLOT_END 71
#elif KCD2VR_M2_HOST_SLOT == 78
// src/mod.cpp: applymenustate_02.inl
// M2_SLOT_BEGIN 78
    ApplyImpaleHang32(state.impale_hang);kcd2vr::gore32::decap.store(state.decapitation); // C32
    kcd2vr::gorepak50::dismember.store(state.dismemberment_d80,std::memory_order_release); // D80: explicit saved limb choice, no migration
    kcd2vr::gore32::decapStyle44.store(kcd2vr::gore32::clampDecapStyle46(state.decap_style44)); // C44/C46: 0 hide, 1 loose neck, 2 stump + flying head
    kcd2vr::impale40::mode.store(std::clamp(state.exp_impale40,0,2)); // C40
// M2_SLOT_END 78
#elif KCD2VR_M2_HOST_SLOT == 82
// src/mod.cpp: applymenustate_03.inl
// M2_SLOT_BEGIN 82
    g_decapitation59c7e.store(state.decapitation,std::memory_order_release);combat_diag_probe::SetDecapitation59c7e(state.decapitation,g_decapitation_min_energy59c7e); // R59.16C7E
// M2_SLOT_END 82
#elif KCD2VR_M2_HOST_SLOT == 112
// src/mod.cpp: savemenusettings_01.inl
// M2_SLOT_BEGIN 112
    ok = wb("VrDecapitation", g_decapitation59c7e.load(std::memory_order_acquire)) && ok; // R59.16C7E: menu-owned
// M2_SLOT_END 112
#elif KCD2VR_M2_HOST_SLOT == 113
// src/mod.cpp: savemenusettings_02.inl
// M2_SLOT_BEGIN 113
    ok = wi("VrDecapitationStyle", kcd2vr::gore32::decapStyle44.load()) && ok; // C44: menu-owned (impulse/wait are INI only, written back so they stay visible)
    ok = wf("VrDecapitationHeadImpulse", kcd2vr::gore32::decapHeadImpulse44.load()) && ok; ok = wi("VrDecapitationLooseNeckWaitMs", kcd2vr::gore32::decapLooseNeckWaitMs44.load()) && ok;
    ok = wi("VrImpaleHang", g_impale_hang32.load()) && ok; // C32
    ok = wf("VrImpaleHangMassKg", kcd2vr::gore32::hangMassKg.load()) && ok; ok = wf("VrImpaleKillEnergy", kcd2vr::contact58c1::impaleKillEnergy44.load()) && ok; // C44
    ok = wb("VrImpaleEntityRoot", kcd2vr::impale47::entityRoot.load()) && ok; // I10: impale root accepts KCD2 entity foreign type 2
    {namespace i40=kcd2vr::impale40; // C40: menu-owned switch + INI-only depth tuning (written back so it stays visible)
     ok = wi("VrImpaleByArmour", i40::mode.load()) && ok; ok = wf("VrImpaleLightArmour", i40::lightArmour.load()) && ok; ok = wf("VrImpaleHardArmour", i40::hardArmour.load()) && ok;
     ok = wf("VrImpaleMinEnergy", i40::minEnergy.load()) && ok; ok = wf("VrImpaleThroughEnergy", i40::throughEnergy.load()) && ok;
     ok = wf("VrImpalePartialCm", i40::partialCm.load()) && ok; ok = wf("VrImpaleThroughCm", i40::throughCm.load()) && ok;}
// M2_SLOT_END 113
#elif KCD2VR_M2_HOST_SLOT == 178
// src/mod.cpp: loadconfig_01.inl
// M2_SLOT_BEGIN 178
    g_decapitation59c7e.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDecapitation",true)); // R59.16C7E; R124 default ON (Miki's INI)
// M2_SLOT_END 178
#elif KCD2VR_M2_HOST_SLOT == 179
// src/mod.cpp: loadconfig_02.inl
// M2_SLOT_BEGIN 179
    combat_diag_probe::SetDecapitation59c7e(g_decapitation59c7e.load(),g_decapitation_min_energy59c7e);
// M2_SLOT_END 179
#elif KCD2VR_M2_HOST_SLOT == 180
// src/mod.cpp: loadconfig_03.inl
// M2_SLOT_BEGIN 180
    kcd2vr::gore32::decapMinPower.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrDecapitationMinPower",0.9f),0.9f,1.5f));
    kcd2vr::gore32::decapAllowBand.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrDecapitationAllowBand",false)); // H18: living-kill neck band opt-in (user-tunable thresholds stay on their original keys)
    kcd2vr::gore32::limbCorpseMinSpeed.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrLimbCorpseMinSpeed",5.0f),1.f,12.f)); // C60: limb-cut receipt threshold
// M2_SLOT_END 180
#elif KCD2VR_M2_HOST_SLOT == 181
// src/mod.cpp: loadconfig_04.inl
// M2_SLOT_BEGIN 181
    kcd2vr::gore32::decapStyle44.store(kcd2vr::gore32::clampDecapStyle46(ReadIniIntExplicit(explicit_ini,"Settings","VrDecapitationStyle",2))); // C44/C46
// M2_SLOT_END 181
#elif KCD2VR_M2_HOST_SLOT == 182
// src/mod.cpp: loadconfig_05.inl
// M2_SLOT_BEGIN 182
    kcd2vr::gore32::decapLooseNeckWaitMs44.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrDecapitationLooseNeckWaitMs",3000),200,10000));
    ApplyImpaleHang32(ReadIniIntExplicit(explicit_ini,"Settings","VrImpaleHang",2)); // C32
// M2_SLOT_END 182
#elif KCD2VR_M2_HOST_SLOT == 211
// src/mod.cpp: loadconfig_06.inl
// M2_SLOT_BEGIN 211
    frik_pose_probe::SetImpaleLatch178r15(ReadIniBoolExplicit(explicit_ini,"Settings","VrImpaleLatch",true)); // V66.178R15
// M2_SLOT_END 211
#elif KCD2VR_M2_HOST_SLOT == 268
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 268
    bool bloody_fights49=true; // C49 VrBloodyFights: 2x blood on hits by Henry + 4 s gush on kill moves
// M2_SLOT_END 268
#elif KCD2VR_M2_HOST_SLOT == 277
// src/vr_menu.h: menu_state_02.inl
// M2_SLOT_BEGIN 277
    int   impale_hang = 2;                  // C32 VrImpaleHang: 0 off, 1 light, 2 normal, 3 heavy (body hangs on the blade)
    int   impale_hang_on45 = 2;             // C45 read-only: the last non-zero VrImpaleHang, what the EXPERIMENTS ON/OFF toggle restores
    bool decapitation = false;              // Existing user setting remains authoritative; new delivery does not enable decapitation
    bool dismemberment_d80 = false;          // VrDismemberment remains opt-in; the gore selector exposes the saved choice
    int   decap_style44 = 2;                // C44/C46 VrDecapitationStyle: 0 hide the head parts, 1 loose neck + head kick (head stays on the ragdoll), 2 stump + flying head (C43D Lua)
// M2_SLOT_END 277
#elif KCD2VR_M2_HOST_SLOT == 297
// src/vr_menu.h: menu_state_03.inl
// M2_SLOT_BEGIN 297
    float impale_hang_mass44 = 40.f;        // C44 VrImpaleHangMassKg 20..80: felt weight of the impaled body on the blade
// M2_SLOT_END 297
#elif KCD2VR_M2_HOST_SLOT == 299
// src/vr_menu.h: menu_state_04.inl
// M2_SLOT_BEGIN 299
    float impale_kill_energy44 = .3f;       // C44 VrImpaleKillEnergy 0..1: a killing thrust impales from this stroke energy (0 = every one)
    int   exp_impale40 = 0;             // C40 optional armour-depth experiment OFF; existing lethal-only path unchanged
// M2_SLOT_END 299
#elif KCD2VR_M2_HOST_SLOT == 307
// src/vr_menu.h: menu_state_05.inl
// M2_SLOT_BEGIN 307
    bool  kick_frees_impale26 = true;       // C26 VrKickFreesImpale
// M2_SLOT_END 307
#elif KCD2VR_M2_HOST_SLOT == 5010
// src/mod.cpp: impale_decap_state
// M2_SLOT_BEGIN 5010
static std::atomic<int> g_impale_hang32{2}; // C32: 0 off, 1 light 1.5 kg, 2 normal 3 kg, 3 heavy 6 kg at the stuck point (INI VrImpaleHang)
static std::atomic<int> g_impale_hang_on45{2}; // C45: the last non-zero VrImpaleHang (1..3), restored by the EXPERIMENTS IMPALE ON KILL STAB toggle (session only, default 2 normal)
static void ApplyImpaleHang32(int v) noexcept {v=std::clamp(v,0,3);g_impale_hang32.store(v);if(v)g_impale_hang_on45.store(v);kcd2vr::gore32::impaleHang.store(v>0);constexpr float kg[]{0.f,1.5f,3.f,6.f};kcd2vr::gore32::impaleWeightKg.store(kg[v]);}
static std::atomic<bool> g_decapitation59c7e{false}; // R59.16C7E: Immersion page, INI VrDecapitation (phase 1 log only)
// M2_SLOT_END 5010
#elif KCD2VR_M2_HOST_SLOT == 5022
// src/vr_menu.cpp: menu_decap_labels
// M2_SLOT_BEGIN 5022
// D80: one explicit gore cycle on every existing alias. Keep the saved head
// presentation style; selecting HEADS ONLY must not silently switch to HIDE HEAD.
void CycleDecap44(VrMenuState& s) {
    if(!s.decapitation){s.decapitation=true;s.dismemberment_d80=false;}
    else if(!s.dismemberment_d80){s.dismemberment_d80=true;}
    else {s.decapitation=false;s.dismemberment_d80=false;}
}
const wchar_t* DecapLabel44(const VrMenuState& s) {
    const bool missing=kcd2vr::gorepak50::state.load()==2;
    // Independent legacy INI values remain authoritative until an explicit click.
    if(!s.decapitation&&s.dismemberment_d80)return missing?L"LIMBS ONLY: NO GORE PAK":L"LIMBS ONLY (SAVED)";
    if(!s.decapitation)return L"GORE: OFF";
    if(s.dismemberment_d80)return missing?L"HEADS + LIMBS: NO GORE PAK":L"GORE: HEADS + LIMBS";
    return missing?L"HEADS ONLY: NO GORE PAK":L"GORE: HEADS ONLY";
}
const char* DecapTrace46(const VrMenuState& s,const char* page) {
    static char b[200];
    std::snprintf(b,sizeof(b),"%s VrDecapitation=%s VrDismemberment=%s VrDecapitationStyle=%d",
        page,s.decapitation?"ON":"OFF",s.dismemberment_d80?"ON":"OFF",s.decap_style44);
    return b;
}

// M2_SLOT_END 5022
#else
#error Unknown M2 host contribution slot
#endif
