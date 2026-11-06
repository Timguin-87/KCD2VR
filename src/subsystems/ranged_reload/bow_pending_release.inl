static bool PhysPendingBowRelease103(const PhysNativeOwner6698& owner,const ph::Command& command,const PhysFrame6698& frame,std::uint64_t now) noexcept {
    return command.kind==ph::CommandKind::BowFire&&command.key==owner.key&&command.epoch==owner.request&&
        command.bodyEpoch==owner.epoch&&command.sample<=frame.input.sample&&now>=command.tick&&now-command.tick<=100;
}
static bool PhysBowReleaseReady175(const PhysNativeOwner6698& n,const ph::Command& c,
        const PhysFrame6698& f,std::uint64_t now,bool right,bool left) noexcept {
    if(c.kind!=ph::CommandKind::BowFire||!n.bowHeld||n.key!=c.key||n.bowGeneration175!=c.generation||
       c.epoch!=n.request||c.bodyEpoch!=n.epoch||!n.bowBeginSample175||c.sample<=n.bowBeginSample175||
       !f.input.sample||c.sample>f.input.sample||now<c.tick||now-c.tick>100||
       f.request!=n.request||f.input.bodyEpoch!=n.epoch||f.profiles[1].key!=n.key||
       f.profiles[1].generation!=n.bowGeneration175||!f.profiles[1].bow()||
       !f.input.allowed||!f.input.bowActions||!f.input.tracked[0]||!f.input.tracked[1]||!right||!left||
       f.output.handlingOnly55c1[1]||f.output.displayRejoinR13[1])return false;
    if((n.physicalBow175||ph::bow_requires_charge_ack175(f.input))&&!n.bowChargeObserved114)return false;
    if(n.physicalBow175)return !f.input.nativeBowTrigger126&&ph::manual_bow_geometry146(f.profiles[1])&&
        std::isfinite(c.fraction)&&c.fraction>.12&&c.fraction<=1;
    return f.input.nativeBowTrigger126||c.fraction>.12;
}
// R30C1 compact causal ledger. Called by the native frame owner; no new polling.
