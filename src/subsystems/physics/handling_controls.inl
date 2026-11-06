void SetTwoHandAxis59c16(bool on) noexcept {g_two_hand_axis59c16.store(on,std::memory_order_relaxed); // R59.16C10R16
    diagnostic_trace::Write(on?"R59.16C10R16 TWO_HAND grip=AXIS(latch where the off hand closes, fixed grasp until release, both hands steer the axis, any weapon mode)":"R59.16C10R16 TWO_HAND grip=SOCKET(V120/V164 fixed socket, 55 deg)");}
void SetWeaponFeel59c14(float fatigueRamp,float poleOneHand,bool strikeMinScaleGate,bool liveAttributesAllRungs) noexcept {
    g_fatigue_ramp59c14.store(std::clamp(fatigueRamp,0.f,1.f),std::memory_order_relaxed);
    g_pole_one_hand59c14.store(std::clamp(poleOneHand,.15f,1.f),std::memory_order_relaxed);
    g_strike_min_scale_gate59c14.store(strikeMinScaleGate,std::memory_order_relaxed);
    g_live_attributes_all_rungs59c14.store(liveAttributesAllRungs,std::memory_order_relaxed);
    kcd2vr::live5912::allRungs.store(liveAttributesAllRungs,std::memory_order_relaxed); // R59.16C10R17: the producer follows it too
    char m[220]{};std::snprintf(m,sizeof(m),"R59.16C10R14 WEAPON_FEEL fatigueRamp=%.2f poleOneHand=%.2f strikeMinScaleGate=%d liveAttributesAllRungs=%d weightFeel=%.2f",
        double(g_fatigue_ramp59c14.load()),double(g_pole_one_hand59c14.load()),strikeMinScaleGate?1:0,liveAttributesAllRungs?1:0,double(g_weapon_authority_scale178r44.load()));
    diagnostic_trace::Write(m);
}
