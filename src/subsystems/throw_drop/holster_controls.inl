void SetLegacyHolsterLayout147(bool legacy) noexcept {
    if(g_legacy_holsters147.exchange(legacy)!=legacy)g_weapon_request_epoch6694.fetch_add(1);
}
void SetHolsterPolicy136(float radius,bool releaseAnywhere) noexcept {
    const float r=std::isfinite(radius)?std::clamp(radius,.10f,.40f):.24f;
    g_holster_radius136.store(r);kcd2vr::haptics101::zoneRadius.store(r);g_holster_release_anywhere136.store(releaseAnywhere);
    char m[256]{};std::snprintf(m,sizeof(m),"V66.164 HOLSTER_POLICY zoneRadius=%.2f releaseGripAnywhere=%d userLayout={bow:LEFT_HAND_RIGHT_SHOULDER shield:LEFT_HAND_LEFT_SHOULDER otherBack:RIGHT_HAND_RIGHT_SHOULDER} oneHanded=NATIVE_MOUNT_OR_F3_3_RIGHT_BELT",r,releaseAnywhere?1:0);
    diagnostic_trace::Write(m);}
