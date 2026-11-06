static bool ControllerWeaponTransform6694(AttachmentState6689& state,int mode,const float (&original)[8],float (&out)[8]) noexcept {
    QuatT28 nativeWorld{},requested{};kcd2vr::pose_core::Stamp source{};
    state.usedCommitted=mode==3;
    if(mode==3){
        QuatT28 native{},allowed{};float world[8]{};
        if(!ReadCommittedHandFrame6689(native,allowed,world,&source))return false;
        nativeWorld=HandWorld6694(native,world);requested=HandWorld6694(allowed,world);
    }else{
        WeaponTracking6694 raw{};if(!ReadWeaponTracking6694(raw))return false;
        nativeWorld=HandWorld6694(raw.nativeHand,raw.world);requested=HandWorld6694(raw.requestedHand,raw.world);
        source={raw.epoch,raw.sample,raw.tick,reinterpret_cast<std::uintptr_t>(raw.character)};
    }
    if(!state.gripValid){QuatT28 weapon{};std::copy_n(original,4,weapon.q);std::copy_n(original+4,3,weapon.t);
        state.grip=RelativeFromAbsolute(nativeWorld,weapon);state.gripValid=true;}
    const QuatT28 wanted=AbsoluteFromRelative(requested,state.grip);
    std::copy_n(original,8,out);std::copy_n(wanted.q,4,out);std::copy_n(wanted.t,3,out+4);
    for(float v:out)if(!std::isfinite(v))return false;
    state.lastSource=source;state.lastMode=mode;std::copy_n(out,8,state.lastWanted);return true;
}
// V66.132: atomic 3 -> physical takeover. While the new physical owner is
// WAITING for its coherent frame/grip, the already-qualified controller writer
// may remain the sole writer for the SAME right-hand attachment. The instant
// PhysicalAttachmentUpdate101 returns Placed, this bridge is no longer called.
static bool BridgeControllerToPhysical132(void* self,void* attachment,int mode) noexcept {
    AttachmentLock6689 guard;if(!guard.owned)return false;auto& state=g_attachment6689;
    const bool same=state.self==self&&state.attachment==attachment;
    if(!kcd2vr::physical_handoff132::allow_bridge(mode,same,state.gripValid,g_gameplay_allowed.load(),
        TrackingHeadUsable111(GetTickCount64()),TrackingHandUsable111(0,GetTickCount64()),double(g_right_grip.load())))return false;
    std::uint32_t bound=0,id=0;if(!Read(static_cast<unsigned char*>(self)+8,bound)||!bound||
       !NativeHeldItemId6694(0,id)||id!=bound)return false;
    void* entity=EntityById6694(id);if(!entity)return false;
    float original[8]{},wanted[8]{};if(!ReadAttachmentQs6689(attachment,original)||!ControllerWeaponTransform6694(state,3,original,wanted))return false;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());const auto request=g_weapon_request_epoch6694.load();
    if(mode!=g_weapon_control_mode.load()||!g_gameplay_allowed.load()||!TrackingHeadUsable111(now)||!TrackingHandUsable111(0,now)||
       (FullWeaponMode120(mode)&&g_right_grip.load()<.25f))return false;
    std::uint32_t live=0;if(!NativeHeldItemId6694(0,live)||live!=id)return false;
    bool child=false;if(!PlaceBothConsumers6689(self,entity,wanted,child))return false;
    const bool newRequest=state.requestEpoch!=request;state.changed=true;state.requestEpoch=request;MarkWeaponPending6695(self,attachment);
    if(newRequest){char m[256]{};std::snprintf(m,sizeof(m),"V66.132 PHYSICAL_HANDOFF BRIDGE key=%016llX entity=%u mode=%d soleWriter=CONTROLLER until=PHYSICAL_PLACED grip=%.3f",
        (unsigned long long)g_native_side_wuid0[0].load(),id,mode,double(g_right_grip.load()));diagnostic_trace::Write(m);}
    return true;
}
