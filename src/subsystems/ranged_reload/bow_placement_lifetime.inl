static void PhysRetireBowGeometryLifetime59c10r2() noexcept {
    g_bow_placement59c10r2.publish(g_bow_lifetimes59c10r2.observe({},0,0,0));
}
static void PhysPublishBowPlacement59c10r2(const PhysEquipment6698& equipment,const PhysEquipmentItem6698& item,
        const PhysFrame6698& frame,const ModelWorld6692& body,const ph::Pose& expected,std::uint64_t now) noexcept {
    // This runs only after successful exact held-item placement, never on a
    // worker or anti-flicker replay. The existing CPoseData +0x8E0/+0x190 chain
    // is ALSO matched to the actual skinning callback before local geometry is
    // acquired; a merely plausible pointer/type cannot create that calibration.
    BowCharacterIdentity166 identity{};ModelWorld6692 actual{};bowgeom59::Identity live{};
    std::uintptr_t ownerType=0;
    bool valid=frame.input.allowed&&frame.input.tracked[0]&&frame.input.tracked[1]&&
        frame.input.tick&&now>=frame.input.tick&&now-frame.input.tick<=80&&
        frame.request==equipment.request&&frame.input.bodyEpoch==equipment.epoch&&
        item.profile.bow()&&item.held&&item.profile.side==1&&frame.profiles[1].key==item.profile.key&&
        frame.profiles[1].generation==item.profile.generation&&
        g_bow_identity166.read(identity)&&identity.character&&identity.entity==item.entity&&
        identity.tick&&now>=identity.tick&&now-identity.tick<=80&&
        std::isfinite(body.scale)&&body.scale>.1f&&body.scale<10.f;
    if(valid){
        live.key=item.profile.key;live.generation=item.profile.generation;live.request=frame.request;live.epoch=frame.input.bodyEpoch;
        live.entity=item.entity;live.character=identity.character;
        live.pose=static_cast<unsigned char*>(identity.character)+kPoseInCharInstanceOffset;
        valid=Read(live.pose,live.poseType)&&live.poseType&&
            Read(static_cast<unsigned char*>(live.pose)+kPoseDefaultSkeletonOffset,live.owner)&&live.owner&&
            Read(live.owner,ownerType)&&ownerType==WhBase()+kCDefaultSkeletonVftRva&&
            ReadModelWorld6692(identity.character,actual)&&
            pm::length(pm::Vec3{actual.t.x,actual.t.y,actual.t.z}-expected.p)<.03&&
            pm::angle(pm::Quat{actual.q.x,actual.q.y,actual.q.z,actual.q.w},expected.q)<.08;
        if(valid)live.scale=double(actual.scale)/body.scale;
    }
    if(!valid)live={};
    g_bow_placement59c10r2.publish(g_bow_lifetimes59c10r2.observe(live,now,frame.input.tick,frame.input.sample));
}
