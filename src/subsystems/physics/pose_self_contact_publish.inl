static void PublishShieldPat54(CoherentTransaction6689& tx,const CommittedFrame6695& packet,bool physicalPublished) noexcept {
    (void)PublishOwnShieldClang58(tx,packet,physicalPublished,std::uint64_t(GetTickCount64()));
    namespace sc=kcd2vr::self_contact54;const auto& sample=tx.shieldContact54;
    if(!physicalPublished||!sample.eligible||sample.owner.character!=packet.stamp.actor||sample.owner.epoch!=packet.stamp.epoch||
       sample.sample!=packet.stamp.sample||sample.tick!=packet.stamp.tick){sc::retire(g_pose6689.shieldPat54);return;}
    auto edge=sc::committed(g_pose6689.shieldPat54,sample);
    if(edge.tick){edge.serial=g_selfPatSerial54.fetch_add(1,std::memory_order_relaxed)+1;g_selfPat54.publish(edge);}
}
// Only a successfully published body+physical packet advances the pat history.
// World/arm constraints may move the wrist after the analytic correction; use
// that actual final wrist and reject a changed native/weapon orientation.
static void PublishOwnBladeContact54(CoherentTransaction6689& tx,const CommittedFrame6695& packet,bool physicalPublished) noexcept {
    namespace sc=kcd2vr::self_contact54;auto sample=tx.selfContact54;
    if(!physicalPublished||!sample.eligible||sample.owner.character!=packet.stamp.actor||sample.owner.epoch!=packet.stamp.epoch||
       sample.sample!=packet.stamp.sample||sample.tick!=packet.stamp.tick||sample.owner.hand<0||sample.owner.hand>1){
        sc::retire(g_pose6689.selfPat54);return;}
    const int side=sample.owner.hand;
    if(!packet.valid[side]||pc::angle(sample.orientation,PQ(QFromQuatT(packet.allowed[side])))>.01){
        sc::retire(g_pose6689.selfPat54);return;}
    sample.wrist=PV(FromT(packet.allowed[side]));
    auto edge=sc::committed(g_pose6689.selfPat54,sample);
    if(edge.tick){edge.serial=g_selfPatSerial54.fetch_add(1,std::memory_order_relaxed)+1;g_selfPat54.publish(edge);}
}

// V66.105 finger diagnostics.
//  F10 rung 1: read-only. Every 4 s per hand and finger: how far (degrees, summed over the
//              joints) the native OPEN endpoint, the bind pose and the final open pose are from
//              the FIST endpoint (bigger = more open), plus where the endpoints came from.
//              Also asks the worker to explain every VRIK.Fingers.* INI section.
//  F10 rung 2+: once per rig/session, the exact per-joint OPEN and FIST locals that are being
//              written ("V66.105-FINGER-HARDCODE"), so they can be compiled in.
