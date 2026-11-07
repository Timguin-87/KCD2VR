// Generated from ACTUAL current mod.cpp settings blocks. Regenerate; do not edit.
float ReadIniFloatExplicit(const char* path, const char* section, const char* key, float fallback) noexcept
{
    if (!path || !section || !key)
        return fallback;
    char fb[64]{};
    std::snprintf(fb, sizeof(fb), "%.9g", static_cast<double>(fallback));
    char raw[64]{};
    GetPrivateProfileStringA(section, key, fb, raw, static_cast<DWORD>(sizeof(raw)), path);
    char* end = nullptr;
    const float parsed = std::strtof(raw, &end);
    const float value = (end && end != raw) ? parsed : fallback;
    char trace[256]{};
    std::snprintf(trace, sizeof(trace), "explicit INI float: [%s] %s raw='%s' parsed=%.6f", section, key, raw, value);
    debug_trace::Write(trace);
    return value;
}
void H29Fill(State& out){
    for(unsigned c=0;c<melee_tuning_c54::classCount;++c)out.melee_types_c54[c]=melee_tuning_c54::read(int(c));
    out.blade_axis_cosine_h41=melee_tuning_c54::bladeAxisSettingH41(melee_tuning_c54::bladeAxisCosineH41.load());
    out.punch_sensitivity_c54=melee_tuning_c54::punch();out.block_sensitivity_h29=melee_tuning_c54::blockH29();out.craft_trial_h41=subsystems::alchemy::reach_craft1::enabled.load();out.bypass_mod_vetoes_h41=melee_tuning_c54::bypassExtraAttackVetoesH41.load();out.chest_trial_h41=frik_pose_probe::ChestPitchShareCB();out.archer_trial_h41=frik_pose_probe::ArcherStanceCB();out.punch_travel_h41=melee_tuning_c54::punchTravel.load();out.guard_width_h41=melee_tuning_c54::guardHalfWidth.load();out.guard_height_h41=melee_tuning_c54::guardHalfHeight.load();out.pb_window_h41=player_window_h26::perfectBlockMultiplier.load();out.ms_window_h41=player_window_h26::multiplier.load();
    for(int h=0;h<2;++h)for(int f=0;f<5;++f)out.finger_middle_lateral_c54[h][f]=finger40::getMiddleLateralC54(h,f);
    for(int h=0;h<2;++h){out.finger_middle_shift_mm_h31[h]=finger40::getShiftMmH31(h);out.finger_middle_distal_medial_h31[h]=finger40::getDistalMedialDegH31(h);} // H31_FINGER_FILL
}
void H29Apply(const State& state){
        for(unsigned c=0;c<melee_tuning_c54::classCount;++c)melee_tuning_c54::set(int(c),state.melee_types_c54[c]);
        melee_tuning_c54::bladeAxisCosineH41.store(melee_tuning_c54::bladeAxisSettingH41(state.blade_axis_cosine_h41));
        melee_tuning_c54::punchSensitivity.store(melee_tuning_c54::bound(state.punch_sensitivity_c54,.5f,2.f,1.f));
        melee_tuning_c54::blockSensitivityH29.store(melee_tuning_c54::bound(state.block_sensitivity_h29,.5f,1.5f,1.f));subsystems::alchemy::reach_craft1::enabled.store(state.craft_trial_h41);melee_tuning_c54::bypassExtraAttackVetoesH41.store(state.bypass_mod_vetoes_h41);frik_pose_probe::SetChestPitchShareCB(state.chest_trial_h41);frik_pose_probe::SetArcherStanceCB(state.archer_trial_h41);melee_tuning_c54::punchTravel.store(melee_tuning_c54::bound(state.punch_travel_h41,.4f,2.f,1.f));melee_tuning_c54::guardHalfWidth.store(melee_tuning_c54::bound(state.guard_width_h41,.25f,.75f,.5f));melee_tuning_c54::guardHalfHeight.store(melee_tuning_c54::bound(state.guard_height_h41,.1f,.35f,.2f));player_window_h26::perfectBlockMultiplier.store(melee_tuning_c54::bound(state.pb_window_h41,1.f,2.f,2.f));player_window_h26::multiplier.store(melee_tuning_c54::bound(state.ms_window_h41,1.f,2.f,1.5f));
        for(int h=0;h<2;++h)for(int f=0;f<5;++f)finger40::setMiddleLateralC54(h,f,state.finger_middle_lateral_c54[h][f]);
        for(int h=0;h<2;++h){finger40::setShiftMmH31(h,state.finger_middle_shift_mm_h31[h]);finger40::setDistalMedialDegH31(h,state.finger_middle_distal_medial_h31[h]);} // H31_FINGER_APPLY
}
bool H29Save(){bool ok=true;
    for(unsigned c=0;c<17;++c){const auto v=melee_tuning_c54::read(int(c));char key[64]{};std::snprintf(key,sizeof(key),"VrMeleeTypeC54_%u_Cut",c);ok=wf(key,v.cut)&&ok;std::snprintf(key,sizeof(key),"VrMeleeTypeC54_%u_Stab",c);ok=wf(key,v.stab)&&ok;std::snprintf(key,sizeof(key),"VrMeleeTypeH29_%u_Intent",c);ok=wf(key,v.intentH29)&&ok;std::snprintf(key,sizeof(key),"VrMeleeTypeH29_%u_Sensitivity",c);ok=wf(key,v.sensitivityH29)&&ok;std::snprintf(key,sizeof(key),"VrMeleeTypeR8_%u_MinTravel",c);ok=wf(key,v.travelR8)&&ok;}
    ok=wf("VrBladeAxisAlignmentH41",melee_tuning_c54::bladeAxisSettingH41(melee_tuning_c54::bladeAxisCosineH41.load()))&&ok;
    ok=wf("VrPunchSensitivityC54",melee_tuning_c54::punch())&&ok;ok=wf("VrFistBlockSensitivityH29",melee_tuning_c54::blockH29())&&ok;ok=wb("VrCraftProximityTrialH41",subsystems::alchemy::reach_craft1::enabled.load())&&ok;ok=wb("VrDisableExtraModAttackVetoesH41",melee_tuning_c54::bypassExtraAttackVetoesH41.load())&&ok;ok=wf("VrChestPitchShareH41Trial",frik_pose_probe::ChestPitchShareCB())&&ok;ok=wb("VrArcherStanceH41Trial",frik_pose_probe::ArcherStanceCB())&&ok;ok=wf("VrPunchTravelH41",melee_tuning_c54::punchTravel.load())&&ok;ok=wf("VrGuardPlaneHalfWidthH41",melee_tuning_c54::guardHalfWidth.load())&&ok;ok=wf("VrGuardPlaneHalfHeightH41",melee_tuning_c54::guardHalfHeight.load())&&ok;ok=wf("VrPerfectBlockScaleH41",player_window_h26::perfectBlockMultiplier.load())&&ok;ok=wf("VrMasterstrikeScaleH41",player_window_h26::multiplier.load())&&ok;
    for(int h=0;h<2;++h)for(int f=0;f<5;++f){char key[64]{};std::snprintf(key,sizeof(key),"VrFingerMiddleLateralC54_%c_%d",h?'L':'R',f);ok=wf(key,finger40::getMiddleLateralC54(h,f))&&ok;}
    for(int h=0;h<2;++h){char key[80]{};std::snprintf(key,sizeof(key),"VrFingerMiddleBaseShiftMmH31_%c",h?'L':'R');ok=wf(key,finger40::getShiftMmH31(h))&&ok;std::snprintf(key,sizeof(key),"VrFingerMiddleDistalMedialDegH31_%c",h?'L':'R');ok=wf(key,finger40::getDistalMedialDegH31(h))&&ok;} // H31_FINGER_SAVE
return ok;}
void H29Load(){const char* explicit_ini="fixture";
    for(unsigned c=0;c<17;++c){auto v=melee_tuning_c54::read(int(c));char key[64]{};std::snprintf(key,sizeof(key),"VrMeleeTypeC54_%u_Cut",c);v.cut=ReadIniFloatExplicit(explicit_ini,"Settings",key,1.f);std::snprintf(key,sizeof(key),"VrMeleeTypeC54_%u_Stab",c);v.stab=ReadIniFloatExplicit(explicit_ini,"Settings",key,1.f);std::snprintf(key,sizeof(key),"VrMeleeTypeH29_%u_Intent",c);v.intentH29=ReadIniFloatExplicit(explicit_ini,"Settings",key,1.f);std::snprintf(key,sizeof(key),"VrMeleeTypeH29_%u_Sensitivity",c);v.sensitivityH29=ReadIniFloatExplicit(explicit_ini,"Settings",key,1.f);std::snprintf(key,sizeof(key),"VrMeleeTypeR8_%u_MinTravel",c);v.travelR8=ReadIniFloatExplicit(explicit_ini,"Settings",key,1.f);melee_tuning_c54::set(int(c),v);}
    melee_tuning_c54::bladeAxisCosineH41.store(melee_tuning_c54::bladeAxisSettingH41(ReadIniFloatExplicit(explicit_ini,"Settings","VrBladeAxisAlignmentH41",.80f)));
    melee_tuning_c54::punchSensitivity.store(melee_tuning_c54::bound(ReadIniFloatExplicit(explicit_ini,"Settings","VrPunchSensitivityC54",1.f),.5f,2.f,1.f));
    melee_tuning_c54::blockSensitivityH29.store(melee_tuning_c54::bound(ReadIniFloatExplicit(explicit_ini,"Settings","VrFistBlockSensitivityH29",1.f),.5f,1.5f,1.f));
    for(int h=0;h<2;++h)for(int f=0;f<5;++f){char key[64]{};std::snprintf(key,sizeof(key),"VrFingerMiddleLateralC54_%c_%d",h?'L':'R',f);finger40::setMiddleLateralC54(h,f,ReadIniFloatExplicit(explicit_ini,"Settings",key,0.f));}
    for(int h=0;h<2;++h){char key[80]{};std::snprintf(key,sizeof(key),"VrFingerMiddleBaseShiftMmH31_%c",h?'L':'R');finger40::setShiftMmH31(h,ReadIniFloatExplicit(explicit_ini,"Settings",key,15.f));std::snprintf(key,sizeof(key),"VrFingerMiddleDistalMedialDegH31_%c",h?'L':'R');finger40::setDistalMedialDegH31(h,ReadIniFloatExplicit(explicit_ini,"Settings",key,10.f));} // H31_FINGER_LOAD
}
