// The attachment producer records actual allowed WORLD object motion, once per
// completed source sample. Release never samples raw wrist or adds actor velocity.
static kcd2vr::weapon_release_t1::Sample g_weapon_release_history_t1[2];
static kcd2vr::core::SnapshotMailbox<kcd2vr::weapon_release_t1::Sample> g_weapon_release_sample_t1[2];
static std::atomic<std::uint64_t> g_weapon_release_pending_t1[2]{};
static void PhysRecordReleaseT1(const PhysEquipment6698& e,const PhysEquipmentItem6698& item,
    const PhysFrame6698& frame,void* attachment,const float (&wanted)[8]) noexcept {
    namespace w=kcd2vr::weapon_release_t1;const int side=item.profile.side;
    if(w::Mode()!=1||side<0||side>1||!frame.input.tracked[side]||!frame.input.intentTracked[side]||
       !std::isfinite(frame.input.grip[side])||frame.input.grip[side]<.25||!frame.output.owned[side])return;
    const auto invalidateD44=[&](){
        g_weapon_release_history_t1[side]={};
        (void)g_weapon_release_sample_t1[side].publish({});
    };
    if(frame.output.displayLatched[side]||frame.output.displayRejoinR13[side]||
       (side==1&&frame.output.bowHandlingOnlyR12)||(frame.output.handlingOnly55c1[side]&&!frame.output.solverOwnedT4[side])){
        invalidateD44();return;
    }
    const ph::Pose displayD44{{wanted[0],wanted[1],wanted[2],wanted[3]},{wanted[4],wanted[5],wanted[6]}};
    if(frame.output.solverOwnedT4[side]){
        kcd2vr::native_weapon_body_t4::Achieved achieved{};
        if(!w::TestEnabledH21()||!kcd2vr::native_weapon_body_t4::Read(achieved)||!achieved.poseValid||achieved.colliderRetired||
           achieved.owner.key!=item.profile.key||achieved.owner.generation!=item.profile.generation||achieved.owner.entityId!=item.profile.entity||
           achieved.owner.manager!=reinterpret_cast<std::uintptr_t>(e.manager)||achieved.owner.player!=reinterpret_cast<std::uintptr_t>(e.player)||
           achieved.owner.character!=frame.correlation126[side].stamp.actor||achieved.owner.epoch!=e.epoch||achieved.owner.request!=e.request||
           achieved.owner.side!=side||!kcd2vr::native_held_pose_t4::fresh(GetTickCount64(),achieved.sourceTick))return;
        if(pm::length(displayD44.p-achieved.weaponWorld.p)>.004||pm::angle(displayD44.q,achieved.weaponWorld.q)>.004){invalidateD44();return;}
    }
    // The mature non-T4 path intentionally retains actual placed WORLD motion.
    // The same controller pose may be transported by a newer LIVE actor root;
    // comparing it to the earlier skin sample here would starve throws while
    // walking. Do not add another collision/transport admission at this leaf.
    w::Sample& sample=g_weapon_release_history_t1[side];kcd2vr::grasp13::Binding identity{};
    identity.manager=reinterpret_cast<std::uintptr_t>(e.manager);identity.character=frame.correlation126[side].stamp.actor;
    identity.item=reinterpret_cast<std::uintptr_t>(item.item);identity.key=item.profile.key;identity.token=item.profile.generation;
    identity.epoch=e.epoch;identity.entity=item.profile.entity;identity.side=side;
    const auto throwEpochH21=kcd2vr::collab_h21::throwEpoch.load(std::memory_order_acquire),physicsEpochH21=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire);
    if(!kcd2vr::grasp13::same(identity,sample.identity)||sample.attachment!=reinterpret_cast<std::uintptr_t>(attachment)||
       sample.requestEpoch!=e.request||sample.throwEpochH21!=throwEpochH21||sample.physicsEpochH21!=physicsEpochH21)sample={};
    sample.identity=identity;sample.attachment=reinterpret_cast<std::uintptr_t>(attachment);
    sample.requestEpoch=e.request;sample.generation=item.profile.generation;
    sample.throwEpochH21=throwEpochH21;sample.physicsEpochH21=physicsEpochH21;
    kcd2vr::grasp13::Placement p{};p.binding=identity;p.attachment=sample.attachment;
    p.self=reinterpret_cast<std::uintptr_t>(item.entity);p.sample=frame.input.sample;p.tick=frame.input.tick;
    std::copy_n(wanted,4,p.world.q);std::copy_n(wanted+4,3,p.world.p);p.world.scale=wanted[7];
    if(sample.motion.add(p))(void)g_weapon_release_sample_t1[side].publish(sample);
}
