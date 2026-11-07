static bool RangedRegistrationReadyXR34() noexcept {
    return retail_build112::Ready(WhBase())&&g_reload_pose_ready_xr34.load(std::memory_order_acquire)&&
        g_reload_ready36.load(std::memory_order_acquire)&&action_time_owner59c11::CrossbowNativeReadyXR34()&&
        g_attachment_helper_transform_original&&g_attachment_helper_transform_hook&&g_attachment_helper_transform_hook->is_enabled();
}
// D78 raw controller goals belong to the same completed transaction as the
// solved wrists. Constraining a wrist must never create its own pull input.
static bool RangedRawIntentD78(xcXR34::Source& out,const CommittedFrame6695& body) noexcept {
    out.intentKnownD78=false;
    if(!xcXR34::handDrivenD81(out))return true;
    if(out.primary<0||out.primary>1||!xcXR34::wristValidD78(out.primaryGripD72))return false;
    for(int side=0;side<2;++side){
        if(!body.propRequestedValid5916c1[side]||!ValidPose6689(body.propRequested5916c1[side]))return false;
        std::memcpy(out.intentWristD78[side].q,body.propRequested5916c1[side].q,sizeof(out.intentWristD78[side].q));
        std::memcpy(out.intentWristD78[side].p,body.propRequested5916c1[side].t,sizeof(out.intentWristD78[side].p));
    }
    const auto weapon=xcXR34::composeWristD78(out.intentWristD78[out.primary],xcXR34::inverseWristD78(out.primaryGripD72));
    const auto inverse=xcXR34::inverseWristD78(weapon);
    for(int side=0;side<2;++side){const auto local=xcXR34::composeWristD78(inverse,out.intentWristD78[side]);
        out.intentHandD78[side]={local.p[0],local.p[1],local.p[2]};}
    out.intentKnownD78=true;return true;
}
static void RangedPublishXR34() noexcept {
    xcXR34::RefreshRegistration();
    RangedReloadEquipmentXR34();
    xcXR34::SetSpatialReader(&RangedSpatialXR34);xcXR34::Source source{};(void)RangedSourceXR34(source);xcXR34::PublishSource(source);
}
