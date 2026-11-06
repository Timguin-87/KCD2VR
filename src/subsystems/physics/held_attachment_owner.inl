static PhysicalAttachmentResult101 NativeHeldAttachmentT4(void* self,void* attachment) noexcept {
    using R=PhysicalAttachmentResult101;heldT4::Achieved a{};heldT4::Owner retained{};
    if(!heldT4::HasUnretiredCollider())return R::NotMine;
    // Positive policy changes retire the competing writer even if its newest
    // receipt is unavailable. Busy lasts until the real collider is retired.
    if(!heldT4::Enabled()||g_weapon_control_mode.load(std::memory_order_acquire)<4)NativeHeldInvalidateT4();
    if(!heldT4::ReadRetainedOwner(retained)){
        return g_native_held_attachment_self_t4.load(std::memory_order_acquire)==reinterpret_cast<std::uintptr_t>(self)||
            g_native_held_attachment_t4.load(std::memory_order_acquire)==reinterpret_cast<std::uintptr_t>(attachment)?R::Busy:R::NotMine;
    }
    const bool receipt=heldT4::Read(a)&&a.owner==retained&&!a.colliderRetired;
    if(!receipt)a.owner=retained;
    const bool exact=a.owner.attachment==reinterpret_cast<std::uintptr_t>(attachment)&&a.owner.attachmentObject==reinterpret_cast<std::uintptr_t>(self);
    std::uint32_t id=0;const bool sameEntity=Read(static_cast<unsigned char*>(self)+8,id)&&id==a.owner.entityId;
    if(!exact&&!sameEntity)return R::NotMine;
    if(!exact){heldT4::Retire(a.owner);return R::Busy;}
    if(a.owner.request!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||
       a.owner.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)){heldT4::Retire(a.owner);return R::Busy;}
    if(!receipt)return R::Busy;
    PhysTryLock6698 lock;if(!lock.owned)return R::Busy;
    // Only the completed coherent frame is presented; the newer native body
    // receipt cannot move the object independently of its wrist/skin packet.
    PhysFrame6698 f{};NativeHeldSourceT4 s{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if(!g_native_held_source_t4.read(s))return R::Busy;
    if(!s.valid||s.input.owner!=a.owner||
       s.input.colliderLease!=a.colliderLease||!NativeHeldSourceCurrentT4(s.input)||
       !PhysVft6698(self,0x3A68F68)||!PhysVft6698(attachment,0x3A657C0)){
        NativeHeldInvalidateT4();return R::Busy;}
    // A newly created/configured body must reach its first coherent publication;
    // attachment callbacks in between phases cannot cancel that admission.
    if(a.state!=heldT4::State::Active||!a.poseValid||!ready||!g_phys_frame6698.read(f)||
       f.input.sample!=ready||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||!f.output.solverOwnedT4[0])return R::Busy;
    const auto now=std::uint64_t(GetTickCount64()); // validate this completed frame after reading it
    if(
       f.request!=a.owner.request||f.input.bodyEpoch!=a.owner.epoch||f.profiles[0].key!=a.owner.key||
       f.profiles[0].generation!=a.owner.generation||f.correlation126[0].stamp.actor!=a.owner.character||
       f.output.solverLeaseT4!=a.colliderLease||f.output.solverWorldGenT4!=a.worldGeneration||f.output.solverBodyGenT4!=a.bodyGeneration||
       !kcd2vr::native_held_pose_t4::fresh(now,f.output.solverSourceTickT4)||
       now<f.input.tick||now-f.input.tick>80||!PhysVft6698(reinterpret_cast<void*>(a.owner.entityPointer),0x4072C78)||
       !PhysVft6698(attachment,0x3A657C0)){heldT4::Retire(a.owner);return R::Busy;}
    void* object=nullptr;
    if(!Read(static_cast<unsigned char*>(attachment)+0x20,object)||object!=self||!sameEntity){heldT4::Retire(a.owner);return R::Busy;}
    // Original model->world from THIS packet, never rebased to a later actor.
    const auto world=f.output.solverWorldT4;
    // PlaceBothConsumers multiplies object+0C model scale by QS[7]. Do not
    // multiply the actual entity-world scale twice for non-unit asset scales.
    float wanted[8]{float(world.q.x),float(world.q.y),float(world.q.z),float(world.q.w),float(world.p.x),float(world.p.y),float(world.p.z),s.attachmentScale};
    bool child=false;if(!PlaceBothConsumers6689(self,reinterpret_cast<void*>(a.owner.entityPointer),wanted,child)){heldT4::Retire(a.owner);return R::Busy;}
    const rdrawD72::Identity drawBindingD75{a.owner.entityPointer,reinterpret_cast<std::uintptr_t>(self),
        reinterpret_cast<std::uintptr_t>(attachment),a.owner.key,a.owner.generation,a.owner.epoch,a.owner.request,a.owner.entityId,0};
    if(!WeaponDrawAcceptedForD75(drawBindingD75)||RangedDrawPendingForD72(drawBindingD75)){
        bool checked=false;
        const bool verified=PlacementReadback6689(self,reinterpret_cast<void*>(a.owner.entityPointer),a.owner.entityId,wanted,checked);
        const bool current=f.publicationT23==ReadCompletedPublicationT23()&&g_committed_pose_valid6689.load(std::memory_order_acquire)&&
            f.trackingEpochT23==tracking59c6::epoch.load(std::memory_order_acquire);
        RangedDrawPlacedD72(drawBindingD75,true,verified&&current);
    }
    // F9 release is intentionally not fed to T1 until a native solver-velocity
    // handoff is qualified. Old controller samples cannot masquerade as throws.
    return R::Placed;
}
