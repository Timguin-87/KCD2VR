static xcXR34::Source CompletedReloadPoseSourceXR34(const xcXR34::Passenger& p,const CommittedFrame6695& packet,
    const QuatT28& weapon,const TrackingSnapshot& tracking,std::uint64_t now) noexcept {
    xcXR34::Source next{};
    if(p.workingHand<0||p.workingHand>1||p.frame.primary!=1-p.workingHand||packet.stamp.actor!=p.frame.character||packet.stamp.epoch!=p.frame.body||packet.stamp.sample!=tracking.sequence||
       packet.stamp.tick!=tracking.tick||tracking.source_epoch!=p.frame.tracking||!tracking.allowed||
       tracking.head_held||tracking.right_held||tracking.left_held||!packet.valid[p.workingHand]||!ValidPose6689(weapon))return next;
    next=p.owner.source;next.publication=0;next.sample=tracking.sequence;next.tick=tracking.tick;next.completedLease=p.owner.serial;
    next.nativePoseClockD78=p.clock.entry;next.nativePoseNormalizedD78=p.clock.normalized;next.nativePoseActiveTimeD78=p.clock.activeTime;
    ChestFlagsD66C(next,tracking);
    std::memcpy(next.world,packet.world,sizeof(next.world));std::memcpy(next.weaponModel.q,weapon.q,sizeof(weapon.q));
    std::memcpy(next.weaponModel.p,weapon.t,sizeof(weapon.t));
    next.nativeWeaponD72=next.weaponModel;
    if(xcXR34::handDrivenD81(p.owner.source)&&packet.valid[p.frame.primary]){
        const auto& g=p.owner.source.primaryGripD72;
        const ph::Pose grip{{g.q[0],g.q[1],g.q[2],g.q[3]},{g.p[0],g.p[1],g.p[2]}};
        const auto held=ph::compose(PhysPose6698(packet.allowed[p.frame.primary]),ph::inverse(grip));
        next.weaponModel={{float(held.q.x),float(held.q.y),float(held.q.z),float(held.q.w)},
            {float(held.p.x),float(held.p.y),float(held.p.z)}};
    }
    const auto& model=next.weaponModel;
    const auto inverse=ph::inverse(ph::Pose{{model.q[0],model.q[1],model.q[2],model.q[3]},
        {model.p[0],model.p[1],model.p[2]}});
    for(int side=0;side<2;++side){const auto& allowed=(side==p.workingHand||xcXR34::handDrivenD81(p.owner.source))?packet.allowed[side]:packet.native[side];
        std::memcpy(next.nativeWrist[side].q,packet.native[side].q,sizeof(packet.native[side].q));
        std::memcpy(next.nativeWrist[side].p,packet.native[side].t,sizeof(packet.native[side].t));
        std::memcpy(next.allowedWrist[side].q,allowed.q,sizeof(allowed.q));std::memcpy(next.allowedWrist[side].p,allowed.t,sizeof(allowed.t));
        const auto hand=ph::point(inverse,PhysPose6698(allowed).p);next.hand[side]={hand.x,hand.y,hand.z};
        next.grip[side]=side?tracking.left_grip:tracking.right_grip;}
    const auto grip=ph::point(inverse,PhysPose6698(packet.native[p.workingHand]).p);
    next.nativeGrip={grip.x,grip.y,grip.z};
    if(!RangedRawIntentD78(next,packet))return xcXR34::Source{};
    next.allowed=xcXR34::fresh(next,now);
    return next;
}
