static bool PhysBeltExperiment164(const PhysEquipment6698& e,int index,void* self,void* attachment,const float* natural,const CommittedFrame6695& body) noexcept {
    // D48: physical front holsters use the same body anchor as acquisition.
    // This is ordinary full-mode presentation, independent of diagnostic rungs.
    if(g_weapon_control_mode.load()<6||
       diagnostics::lab::NativeReference()||!g_gameplay_allowed.load()||animation_filter::nativeBodyOwned112.load()||
       index<0||index>=e.count)return false;
    const auto& item=e.items[index];
    if(item.held||item.set!=e.selected||!belt_layout164::eligible(item.profile.classId,item.profile.twoHanded)||
       ph::holster_zone126(item.profile)!=ph::HolsterZone126::RightHip45||
       !e.handKnown[0]||!e.handKnown[1]||e.handKey[0]==item.profile.key||e.handKey[1]==item.profile.key||
       !PhysEntityIdentity135(item.entity,item.profile.entity))return false;
    // One current selected small weapon. Never relocate inventory/NPC objects,
    // change equip_slot, unhide a native-hidden attachment, or create a model.
    for(int i=0;i<e.count;++i)if(i!=index&&!e.items[i].held&&e.items[i].set==e.selected&&
        belt_layout164::eligible(e.items[i].profile.classId,e.items[i].profile.twoHanded))return false;
    unsigned flags=0;if(!Read(static_cast<unsigned char*>(attachment)+8,flags)||(flags&0x70000u))return false;
    PhysFrame6698 frame{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_phys_frame6698.read(frame)||frame.request!=e.request||frame.input.bodyEpoch!=e.epoch||
       frame.player!=e.player||frame.manager!=e.manager||!frame.input.allowed||now<frame.input.tick||now-frame.input.tick>80)return false;
    ModelWorld6692 current{};if(!PhysAttachmentWorld164(attachment,body.stamp.actor,current))return false;
    const float world[8]{current.q.x,current.q.y,current.q.z,current.q.w,current.t.x,current.t.y,current.t.z,current.scale};
    const auto origin=PhysModel6698(natural,world);ph::Pose moved{},localGrip{};bool gripKnown=false;
    const int hand=ph::holster_hand126(item.profile);
    for(const auto& grip:g_phys_grip_bank6698.grip)if(grip.key==item.profile.key&&grip.generation==item.profile.generation){
        if(hand>=0&&hand<2&&grip.ready[hand]){localGrip=grip.hand[hand];gripKnown=true;}break;
    }
    if(!ph::front_stowed_pose_d48(frame.input,item.profile,origin,localGrip,gripKnown,moved))return false;
    const auto target=PhysCurrentWorld164(moved,current);
    const float wanted[8]{float(target.q.x),float(target.q.y),float(target.q.z),float(target.q.w),float(target.p.x),float(target.p.y),float(target.p.z),natural[7]};
    bool child=false;InvalidateReadCache101R2();
    if(!PlaceBothConsumers6689(self,item.entity,wanted,child)){
        if(g_entity_attachment_update_original)g_entity_attachment_update_original(self,attachment);
        // A front-only presentation failure cannot revoke both physical hands.
        if(PhysHolsterCaptureD48(now))diagnostic_trace::Write("D48 FRONT_HOLSTER placement=FAILED nativePresentation=RESTORED physicalOwner=UNCHANGED");
        return false;}
    // The gesture point and the rendered stored item use the same model pose.
    // Native grip capture remains native; only this current mount is overridden.
    for(auto& g:g_phys_grip_bank6698.grip)if(g.key==item.profile.key&&g.generation==item.profile.generation){
        g.mount=moved;g.mountReady=true;g.mountStamp126=body.stamp;break;}
    g_phys_grips6698.publish(g_phys_grip_bank6698);
    static std::uint64_t last=0;if(PhysHolsterCaptureD48(now)&&(now<last||now-last>=2000)){last=now;char m[420]{};
        std::snprintf(m,sizeof(m),"V66.164 RIGHT_BELT key=%016llX class=%d set=%d entity=%u from=(%.3f,%.3f,%.3f) to=(%.3f,%.3f,%.3f) flags=%X nativeEquipSlots=UNCHANGED visibility=RETAIN_NATIVE draw=EXISTING_NATIVE_REQUEST gripKnown=%d storedHiltGap=%.6f",
            (unsigned long long)item.profile.key,item.profile.classId,item.set,item.profile.entity,origin.p.x,origin.p.y,origin.p.z,moved.p.x,moved.p.y,moved.p.z,flags,gripKnown?1:0,pm::length(ph::point(moved,gripKnown?localGrip.p:pm::Vec3{})-(ph::body_shoulder_r38(frame.input,0)-ph::body_axes(frame.input).up*.50+ph::body_axes(frame.input).right*.03)));diagnostic_trace::Write(m);}
    return true;
}
