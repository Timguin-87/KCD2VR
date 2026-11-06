static kcd2vr::core::SnapshotMailbox<TrackingSnapshot> g_tracking_frames6695;
static bool TrackingHeadUsable111(std::uint64_t now=0) noexcept {
    if(!now)now=static_cast<std::uint64_t>(GetTickCount64());
    return tracking_grace111::KeepLastGood(g_head_valid.load(std::memory_order_acquire),now,g_head_valid_tick.load(std::memory_order_relaxed),60);
}
static bool TrackingHandUsable111(int side,std::uint64_t now=0) noexcept {
    if(side<0||side>1)return false;if(!now)now=static_cast<std::uint64_t>(GetTickCount64());
    const bool live=side?g_left_valid.load(std::memory_order_acquire):g_right_valid.load(std::memory_order_acquire);
    const auto tick=side?g_left_valid_tick.load(std::memory_order_relaxed):g_right_valid_tick.load(std::memory_order_relaxed);
    return tracking_grace111::KeepLastGood(live,now,tick,80);
}
// Scoped only around the existing final skin-callback read. Generic readers
// must not borrow its cycle, and cannot consume its diagnostic cadence.
struct MotionReadContextD68 {std::uint64_t session=0,cycle=0;std::uintptr_t actor=0,descriptor=0,pose=0,output=0;};
static thread_local MotionReadContextD68 g_motionReadD68;
struct MotionReadScopeD68 {
    MotionReadContextD68 previous;
    MotionReadScopeD68(std::uint64_t session,std::uint64_t cycle,std::uintptr_t actor,std::uintptr_t descriptor,std::uintptr_t pose,std::uintptr_t output) noexcept:previous(g_motionReadD68){
        g_motionReadD68={session,cycle,actor,descriptor,pose,output};
    }
    ~MotionReadScopeD68(){g_motionReadD68=previous;}
    MotionReadScopeD68(const MotionReadScopeD68&)=delete;
};
// Read-only choice receipt. prior IDs belong to permission packet before current-family substitution.
static void TraceTrackingReadD68(const TrackingSnapshot& out,unsigned reason,std::uint64_t priorSample,std::uint64_t priorNs,std::uint64_t readStart) noexcept {
    namespace mt=kcd2vr::diagnostics::motion_d68;mt::Event e{};
    if(!readStart||!mt::begin(e,mt::TrackingRead))return;
    e.endNs=e.ns;e.ns=readStart;
    const bool finalRead=g_motionReadD68.session==e.session&&g_motionReadD68.cycle;
    if(!finalRead&&!mt::inWindow(e))return;
    static thread_local mt::Cadence cadence;
    if(!finalRead&&!cadence.due(e.session,e.ns)){mt::noteCadenceSkip(mt::TrackingRead);return;}
    if(finalRead){e.cycle=g_motionReadD68.cycle;e.actor=g_motionReadD68.actor;e.descriptor=g_motionReadD68.descriptor;
        e.pose=g_motionReadD68.pose;e.output=g_motionReadD68.output;}
    e.sourceNs=out.timeNs;e.sourceTick=out.tick;e.inputTick=out.input_tick;e.sample=out.sequence;
    e.leftTargetD69=out.handTargetD69[0];e.rightTargetD69=out.handTargetD69[1];e.handPolicyD69=out.handPolicyD69;e.leftLeadD69=out.handLeadSourceD69[0];e.rightLeadD69=out.handLeadSourceD69[1];
    e.priorSample=priorSample;e.priorSourceNs=priorNs;e.trackingEpoch=out.source_epoch;e.targetXrNs=out.poseTime;e.thread=GetCurrentThreadId();
    e.flags=reason|(finalRead?0x80000000u:0u)|(unsigned(out.allowed)<<8)|(unsigned(out.head_valid)<<9)|(unsigned(out.left_valid)<<10)|(unsigned(out.right_valid)<<11)|
        (unsigned(out.head_held)<<12)|(unsigned(out.left_held)<<13)|(unsigned(out.right_held)<<14);
    const float v[]{out.head.x,out.head.y,out.head.z,out.hq.x,out.hq.y,out.hq.z,out.hq.w,
        out.left.x,out.left.y,out.left.z,out.lq.x,out.lq.y,out.lq.z,out.lq.w,
        out.right.x,out.right.y,out.right.z,out.rq.x,out.rq.y,out.rq.z,out.rq.w};
    std::copy(std::begin(v),std::end(v),e.v);e.count=21;mt::push(e);
}
bool ReadTrackingSnapshot(TrackingSnapshot& out) noexcept {
    // INVESTIGATE_D67R1[FREE_HAND_FAMILY]: this reads the latest complete XR
    // family; it does not reserve that family for a native render view/palette.
    // D66 free-hand commits 27774 -> 27774 -> 27782 show a source hold followed
    // by a two-period catch-up with no finger contact. Join this sourceEpoch /
    // sample / poseTime to F2_PALETTE_D67R1 and the actual upload/eye pool before
    // changing cadence. Preserve fresh semantic input for gestures and combat;
    // a visual late refresh must not replay their state machines. HJ1's private
    // queue is not proof of a native job callback/owner and is not enabled here.
    const auto readStartD68=kcd2vr::diagnostics::motion_d68::requested()?kcd2vr::diagnostics::perf_d61::nowNs():0;
    if(!g_tracking_frames6695.read(out)){if(kcd2vr::diagnostics::motion_d68::requested())TraceTrackingReadD68(TrackingSnapshot{},1,0,0,readStartD68);return false;}
    const auto priorSampleD68=out.sequence,priorNsD68=out.timeNs;
    unsigned reasonD68=0;
    if(!kcd2vr::reading_pose32::poseAllowed(g_gameplay_allowed.load(std::memory_order_acquire),g_dialogue_body178r9.load(std::memory_order_relaxed),GetTickCount64()))out.allowed=false;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(out.source_epoch&&out.source_epoch!=tracking59c6::epoch.load(std::memory_order_acquire)){
        out.head_valid=out.left_valid=out.right_valid=false;TraceTrackingReadD68(out,2,priorSampleD68,priorNsD68,readStartD68);return true;
    }
    tracking59c6::Frame current59c6{};
    if(tracking59c6::Read(current59c6)){
        const auto choice=tracking59c6::Compose(out,current59c6,now,tracking59c6::NowNs());
        reasonD68=choice==tracking59c6::Choice::Current?3u:choice==tracking59c6::Choice::Invalidated?4u:0u;
        if(choice==tracking59c6::Choice::Current){
            // Optional HJ1 family is joined to the exact native frame and source
            // epoch. A failed/estimated locate keeps the entire ordinary family.
            kcd2vr::hj1::Sample hj1{};
            if(!motion_trials_d67::UncappedSelectedAt(0)&&kcd2vr::hj1::Current(hj1)&&hj1.sourceEpoch==tracking59c6::epoch.load(std::memory_order_acquire))
                if(kcd2vr::hj1::ApplyCompleteFamily(out,hj1))reasonD68=5;
        }
        if(choice!=tracking59c6::Choice::Original){TraceTrackingReadD68(out,reasonD68,priorSampleD68,priorNsD68,readStartD68);return true;}
    }
    if(tracking59c6::enabled.load(std::memory_order_acquire)){
        // No mixed-clock fallback: a busy/uninitialized family is not permission
        // to jump back to the predicted worker packet with a newer sequence.
        out.head_valid=out.left_valid=out.right_valid=false;
        out.head_held=out.left_held=out.right_held=true;TraceTrackingReadD68(out,6,priorSampleD68,priorNsD68,readStartD68);return true;
    }
    // Coordinates and last-good ages belong to the same published sample.
    // Consumers never reconstruct a pose from independently changing atomics.
    out.head_valid=out.head_valid&&tracking_grace111::KeepLastGood(false,now,out.head_good_tick,60); // match producer and gameplay HMD age
    out.left_valid=out.left_valid&&tracking_grace111::KeepLastGood(false,now,out.left_good_tick,80);
    out.right_valid=out.right_valid&&tracking_grace111::KeepLastGood(false,now,out.right_good_tick,80);
    // DS-FIX-20261022 (the H18-onward hit-loss root): the held flags must follow
    // the SAME grace as the valid flags - the raw g_head_valid read here flipped
    // head_held instantly on a one-frame HMD dropout, killing both hands'
    // intentTracked and firing the handlingRecovery that orphaned the stroke.
    out.head_held=out.head_held||!out.head_valid;
    out.left_held=out.left_held||!out.left_valid;
    out.right_held=out.right_held||!out.right_valid;
    TraceTrackingReadD68(out,7,priorSampleD68,priorNsD68,readStartD68);return true;
}
