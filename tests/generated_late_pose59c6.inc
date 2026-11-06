// Verbatim source bodies/statements; test-only class/platform wrappers are identified below.
#if defined(KCD2VR_HF1_PIPELINE_TEST)
struct V3 { float x{},y{},z{}; };
struct Q4 { float x{},y{},z{},w{1.0f}; };
struct TrackingSnapshot {
    std::uint64_t sequence=0,tick=0,timeNs=0;
    std::int64_t poseTime=0; // XR locate target; never used as input/freshness time
    std::uint64_t input_tick=0,source_epoch=0;
    std::uint64_t head_good_tick=0,left_good_tick=0,right_good_tick=0;
    V3 head{}, left{}, right{};
    Q4 hq{}, lq{}, rq{};
    float left_grip=0.0f,right_grip=0.0f;
    float left_trigger=0.0f,right_trigger=0.0f;
    kcd2vr::dialogue_fingers_d78::VisualInput dialogueFingersD78{}; // presentation only
    bool head_valid=false,left_valid=false,right_valid=false,allowed=false;
    bool head_held=false,left_held=false,right_held=false; // V66.86 VRIK-only short dropout hold
    std::int64_t handTargetD69[2]{};unsigned handLeadSourceD69[2]{};std::uint64_t handPolicyD69=0;

};

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
            if(kcd2vr::hj1::Current(hj1)&&hj1.sourceEpoch==tracking59c6::epoch.load(std::memory_order_acquire))
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

static void PublishActualPacket(TrackingPose head,bool hv,TrackingPose left,bool lv,TrackingPose right,bool rv,std::uint64_t poseNow,std::uint64_t sourceNs,std::int64_t poseTime){
 const bool allowed=true,poseAllowed=true;const auto sourceEpoch=tracking59c6::epoch.load();
    TrackingSnapshot ready{};
    ready.head={head.px,head.py,head.pz};ready.left={left.px,left.py,left.pz};ready.right={right.px,right.py,right.pz};
    ready.hq={head.qx,head.qy,head.qz,head.qw};ready.lq={left.qx,left.qy,left.qz,left.qw};ready.rq={right.qx,right.qy,right.qz,right.qw};
    ready.left_grip=g_left_grip.load();ready.right_grip=g_right_grip.load();
    ready.left_trigger=g_left_trigger.load();ready.right_trigger=g_right_trigger.load();
    ready.dialogueFingersD78=kcd2vr::dialogue_fingers_d78::capture(
        dialogueFingersD78&&sourceTick&&sourceNs&&sourceNs<=tracking59c6::NowNs()&&sourceEpoch&&
        sourceEpoch==tracking59c6::epoch.load(std::memory_order_acquire)&&!allowed&&
        g_ctx_dialogue.load(std::memory_order_acquire)&&g_dialogue_body178r9.load(std::memory_order_acquire),
        lv,rv,leftVisualTriggerD78,rightVisualTriggerD78);
    ready.head_valid=hv;ready.left_valid=lv;ready.right_valid=rv;ready.allowed=poseAllowed; // V66.178R9: pose path may run in dialogue
    ready.timeNs=sourceNs?sourceNs:tracking59c6::NowNs();ready.poseTime=poseTime;
    ready.tick=poseNow;ready.sequence=tracking59c6::Next();
    ready.input_tick=poseNow;ready.source_epoch=sourceEpoch?sourceEpoch:tracking59c6::epoch.load(std::memory_order_acquire);
    LifecycleTracking36(ready,allowed); // raw validity/input before presentation grace
    TrackingSnapshot previous{};(void)g_tracking_frames6695.read(previous);
    tracking_grace111::HoldCoherent112(ready,previous);
    const bool publishedD68=g_tracking_frames6695.publish(ready);
    // The permission/button packet is separate from the latest pose family.
    // bit16 distinguishes it; its original input_tick is never refreshed here.
    if(kcd2vr::diagnostics::motion_d68::requested()){
        namespace mt=kcd2vr::diagnostics::motion_d68;mt::Event e{};
        static thread_local mt::Cadence cadence;
        if(mt::begin(e,mt::InputPublished)&&mt::inWindow(e)){
            if(cadence.due(e.session,e.ns)){
                e.flags=0x10000u|unsigned(publishedD68)|(unsigned(ready.allowed)<<17)|
                    (unsigned(ready.head_valid)<<1)|(unsigned(ready.left_valid)<<2)|(unsigned(ready.right_valid)<<3)|
                    (unsigned(ready.head_held)<<10)|(unsigned(ready.left_held)<<11)|(unsigned(ready.right_held)<<12);
                e.sourceNs=ready.timeNs;e.sourceTick=ready.tick;e.inputTick=ready.input_tick;e.sample=ready.sequence;
                e.trackingEpoch=ready.source_epoch;e.targetXrNs=ready.poseTime;e.thread=GetCurrentThreadId();e.endNs=e.ns;
                const float v[]{ready.head.x,ready.head.y,ready.head.z,ready.hq.x,ready.hq.y,ready.hq.z,ready.hq.w,
                    ready.left.x,ready.left.y,ready.left.z,ready.lq.x,ready.lq.y,ready.lq.z,ready.lq.w,
                    ready.right.x,ready.right.y,ready.right.z,ready.rq.x,ready.rq.y,ready.rq.z,ready.rq.w};
                std::copy(std::begin(v),std::end(v),e.v);e.count=21;mt::push(e);
            }else mt::noteCadenceSkip(mt::InputPublished);
        }
    }
}
static void CopyCurrentPoseAndOrigin(std::mutex& mutex, const XrPose& src_pose, const XrPose& src_origin,
                                      std::atomic<float>& scale_atomic,
                                      XrPose& out_pose, XrPose& out_origin, float& out_scale,
                                      const std::atomic<std::uint64_t>* frame_source=nullptr,
                                      std::uint64_t* out_frame=nullptr,
                                      const hj1::Sample* family_source=nullptr,hj1::Sample* out_family=nullptr,
                                      const std::atomic<std::uint64_t>* generation_source=nullptr,std::uint64_t* out_generation=nullptr,
                                      native_roomscale_d76::Origin* out_room_origin=nullptr,std::uint64_t* out_room_revision=nullptr,
                                      const std::uint64_t* source_epoch=nullptr,std::uint64_t* out_source_epoch=nullptr)
{
    std::lock_guard<std::mutex> lock(mutex);
    out_pose = src_pose;
    out_origin = src_origin;
    if(out_room_origin)(void)native_roomscale_d76::ReadTrackingOriginD76(*out_room_origin);
    if(out_room_revision)*out_room_revision=roomscale_d76::originRevision.load(std::memory_order_acquire);
    out_scale = scale_atomic.load(std::memory_order_relaxed);
    if(frame_source&&out_frame)*out_frame=frame_source->load(std::memory_order_relaxed);
    if(family_source&&out_family)*out_family=*family_source;
    if(generation_source&&out_generation)*out_generation=generation_source->load(std::memory_order_relaxed);
    if(source_epoch&&out_source_epoch)*out_source_epoch=*source_epoch;
}

struct CameraMini {std::mutex s_pose_mutex;XrPose s_current_pose{},s_origin_pose{};std::atomic<float>s_current_scale{1};std::atomic<std::uint64_t>s_current_pose_frame_index{0};
void Publish(const XrPose& pose,float scale,std::uint64_t frame_index){
    std::lock_guard<std::mutex> lock(s_pose_mutex);
    const auto sourceEpochD75=tracking59c6::epoch.load(std::memory_order_acquire);
    s_current_pose = pose;
    if (frame_index != 0) s_current_pose_frame_index.store(frame_index, std::memory_order_release);
    s_current_scale.store(scale, std::memory_order_relaxed);

}
 XrPose Consume(){XrPose p{},o{};float scale=1;CopyCurrentPoseAndOrigin(s_pose_mutex,s_current_pose,s_origin_pose,s_current_scale,p,o,scale);return p;} };
static ph::Input Physical(const TrackingSnapshot& ts){ph::Input pi{};struct{std::uint64_t epoch=11;}tx;
pi.timeNs=ts.timeNs;pi.poseTimeEpoch=ts.source_epoch;pi.handPredictionPolicyD69=ts.handPolicyD69;pi.allowed=ts.allowed&&ts.head_valid;pi.epoch=tx.epoch;pi.bodyEpoch=tx.epoch;pi.sample=ts.sequence;pi.tick=ts.tick;
const bool valid[2]{ts.right_valid,ts.left_valid};for(int side=0;side<2;++side){const V3 rawP=side?ts.left:ts.right;pi.hands[side].p={rawP.x-ts.head.x,0,0};pi.intentHands[side]=pi.hands[side];
pi.intentTracked[side]=valid[side]&&ts.head_valid&&!ts.head_held&&!(side?ts.left_held:ts.right_held);
            pi.poseTime[side]=(ts.head_held||(side?ts.left_held:ts.right_held))?0:(ts.handTargetD69[side?0:1]?ts.handTargetD69[side?0:1]:ts.poseTime);
 pi.tracked[side]=valid[side];pi.grip[side]=1;pi.shoulders[side]={side?-.2:.2,0,0};}pi.attacks=false;pi.contact=false;pi.dynamics=false;return pi;}

#else
bool XrManager::SampleHeadPoseAtTime(XrTime target_time, XrPose& out_pose, hj1::Sample* out_family)
{
    if(out_family)*out_family={};
    const auto familyPolicyR24=out_family&&hj1::enabled.load(std::memory_order_acquire)?hand_smooth_h50::policyEpoch.load(std::memory_order_acquire):0;
    tracking59c6::Frame completedR24{};
    diagnostics::xr165::Timer timing165(diagnostics::xr165::SyncPose);
    if (!session_running_.load(std::memory_order_acquire) || target_time <= 0 ||
        view_space_ == XR_NULL_HANDLE || local_space_ == XR_NULL_HANDLE) return false;
    // Synchronous camera routes preserve their chosen predicted/NOW target.
    // Preserve its head sampling semantics. Pose-only controller reads use the
    // same target time, never xrSyncActions or a second button/haptic producer.
    std::lock_guard<std::mutex> lock(pose_locate_mutex_);
    tracking59c6::Frame family{};family.epoch=tracking59c6::epoch.load(std::memory_order_acquire);
    family.tick=std::uint64_t(GetTickCount64());family.ns=tracking59c6::NowNs();family.locateTime=target_time;
    XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
    const auto result=xrLocateSpace(view_space_, local_space_, target_time, &location);
    const XrSpaceLocationFlags need=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
    const XrSpaceLocationFlags tracked=XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
    auto fill=[&](unsigned i,const XrPosef& p,XrSpaceLocationFlags f){
        family.poses[i]={p.position.x,p.position.y,p.position.z,p.orientation.x,p.orientation.y,p.orientation.z,p.orientation.w};
        family.valid[i]=(f&need)==need;family.tracked[i]=(f&tracked)==tracked;
    };
    fill(0,location.pose,XR_SUCCEEDED(result)?location.locationFlags:0);
    if(tracking59c6::enabled.load(std::memory_order_acquire)&&tracking59c6::cameraSource.load(std::memory_order_acquire)!=0){
        XrPosef poses[2]{};XrSpaceLocationFlags flags[2]{};
        const auto extraD69=claude_motion_d69::ExtraNs();XrSpaceVelocity velocityD69[2]{};
        family.controllersSampled=input_.TryLocateGripPair(target_time,poses,flags,extraD69>0?velocityD69:nullptr);
        fill(1,poses[0],flags[0]);fill(2,poses[1],flags[1]);
        if(extraD69>0)ApplyExtraHandsD69(input_,family,velocityD69,extraD69);
        (void)tracking59c6::Publish(family,familyPolicyR24?&completedR24:nullptr);
    }
    tracking59c6::PublishHead(family);
    if(XR_FAILED(result)||(location.locationFlags&need)!=need)return false;
    out_pose.pos_x=location.pose.position.x;out_pose.pos_y=location.pose.position.y;out_pose.pos_z=location.pose.position.z;
    out_pose.qx=location.pose.orientation.x;out_pose.qy=location.pose.orientation.y;out_pose.qz=location.pose.orientation.z;out_pose.qw=location.pose.orientation.w;
    if(familyPolicyR24&&hj1::enabled.load(std::memory_order_acquire)&&familyPolicyR24==hand_smooth_h50::policyEpoch.load(std::memory_order_acquire)&&
       completedR24.epoch==tracking59c6::epoch.load(std::memory_order_acquire))
        *out_family=tracking59c6::CompletedCameraFamilyR24(completedR24,familyPolicyR24);
    return true;
}

bool XrManager::SampleLateHeadPose(XrPose& out_pose)
{
    diagnostics::xr165::Timer timing165(diagnostics::xr165::LatePose);
    if (!late_pose_enabled_.load(std::memory_order_acquire) ||
        !session_running_.load(std::memory_order_acquire) ||
        !qpc_time_extension_enabled_ || !qpc_to_xr_time_ ||
        view_space_ == XR_NULL_HANDLE || local_space_ == XR_NULL_HANDLE) return false;
    // Do not make a camera callback wait behind a duplicate locate. The cached
    // completed family is used at the caller, with its original sample age.
    std::unique_lock<std::mutex> lock(pose_locate_mutex_,std::try_to_lock);
    if(!lock.owns_lock())return false;
    tracking59c6::Frame frame{};frame.epoch=tracking59c6::epoch.load(std::memory_order_acquire);
    frame.tick=std::uint64_t(GetTickCount64());frame.ns=tracking59c6::NowNs();
    LARGE_INTEGER qpc{};if(!QueryPerformanceCounter(&qpc))return false;
    XrTime now=0;
    if(XR_FAILED(qpc_to_xr_time_(instance_,&qpc,&now)))return false;
    frame.locateTime=now;
    XrSpaceLocation head{XR_TYPE_SPACE_LOCATION};
    const auto headResult=xrLocateSpace(view_space_,local_space_,now,&head);
    XrPosef grips[2]{};XrSpaceLocationFlags flags[2]{};
    const auto extraD69=claude_motion_d69::ExtraNs();XrSpaceVelocity velocityD69[2]{};
    frame.controllersSampled=input_.TryLocateGripPair(now,grips,flags,extraD69>0?velocityD69:nullptr);
    const XrSpaceLocationFlags valid=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
    const XrSpaceLocationFlags tracked=XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
    auto fill=[&](unsigned index,const XrPosef& pose,XrSpaceLocationFlags f){
        frame.poses[index]={pose.position.x,pose.position.y,pose.position.z,pose.orientation.x,pose.orientation.y,pose.orientation.z,pose.orientation.w};
        frame.valid[index]=(f&valid)==valid;frame.tracked[index]=(f&tracked)==tracked;
    };
    fill(0,head.pose,XR_SUCCEEDED(headResult)?head.locationFlags:0);
    fill(1,grips[0],flags[0]);fill(2,grips[1],flags[1]);
    if(extraD69>0&&tracking59c6::cameraNow.load(std::memory_order_acquire))ApplyExtraHandsD69(input_,frame,velocityD69,extraD69);
    tracking59c6::PublishHead(frame);
    // Late frustum comfort remains head-owned. Its NOW target must not replace
    // the predicted gameplay family selected by camera modes 0/1.
    if(tracking59c6::cameraNow.load(std::memory_order_acquire))(void)tracking59c6::Publish(frame);
    if(!frame.valid[0]||!tracking59c6::Valid(frame.poses[0])){
        late_pose_failures_.fetch_add(1,std::memory_order_relaxed);return false;
    }
    out_pose.pos_x=head.pose.position.x;out_pose.pos_y=head.pose.position.y;out_pose.pos_z=head.pose.position.z;
    out_pose.qx=head.pose.orientation.x;out_pose.qy=head.pose.orientation.y;out_pose.qz=head.pose.orientation.z;out_pose.qw=head.pose.orientation.w;
    late_pose_samples_.fetch_add(1,std::memory_order_relaxed);
    return true;
}

void XrManager::PublishPredictedForTest(const tracking59c6::Stamp& source59c6,const XrPose& out_head_pose,XrSpaceLocationFlags headFlags59c6,bool inputFresh51){
    if(tracking59c6::enabled.load(std::memory_order_acquire)&&!tracking59c6::cameraNow.load(std::memory_order_acquire)){
        std::lock_guard<std::mutex> pose_lock(pose_locate_mutex_);
        tracking59c6::Frame family{};family.epoch=source59c6.epoch;family.tick=source59c6.tick;
        family.ns=source59c6.ns;family.locateTime=source59c6.locateTime;family.controllersSampled=true;
        const XrSpaceLocationFlags valid=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
        const XrSpaceLocationFlags tracked=XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
        auto fill=[&](unsigned i,const XrPose& p,XrSpaceLocationFlags flags){
            family.poses[i]={p.pos_x,p.pos_y,p.pos_z,p.qx,p.qy,p.qz,p.qw};
            family.valid[i]=(flags&valid)==valid;family.tracked[i]=(flags&tracked)==tracked;
        };
        fill(0,out_head_pose,headFlags59c6);
        fill(1,input_.Left().pose,inputFresh51?input_.Left().pose_flags:0);
        fill(2,input_.Right().pose,inputFresh51?input_.Right().pose_flags:0);
        if(handExtraD69>0){const XrSpaceVelocity velocity[2]{input_.Left().velocityD69,input_.Right().velocityD69};ApplyExtraHandsD69(input_,family,velocity,handExtraD69);}
        // D62: locate optional display data before publishing the raw family.
        // The final tracking prepare assigns its exact sequence/held flags,
        // seals the optional receipt, then exposes raw tracking atomically.
        // A missing/failed visual receipt is a sealed baseline choice, not a
        // reason to delay a consumer or later replace the same raw family.
        tracking59c6::Frame completedR24{};
        bool familyPublishedD61=false;
        if(handPresentD61){
        hand_present_d61::Packet displayD62{};bool locatedDisplayD62=false;
        const auto leadD61=hand_present_lead_d61_.leadNs();
        if(inputFresh51&&leadD61>0&&
           hand_present_d61::WantsSample(source59c6.epoch,source59c6.tick)&&
           tracking59c6::cameraSource.load(std::memory_order_acquire)==0&&
           family.valid[0]&&family.tracked[0]&&
           source59c6.epoch==tracking59c6::epoch.load(std::memory_order_acquire)&&
           hand_present_d61::CurrentPolicy(handPolicyD61)&&
           family.locateTime<=std::numeric_limits<XrTime>::max()-leadD61){
            XrPosef gripsD61[2]{};XrSpaceLocationFlags flagsD61[2]{};
            const auto targetD61=family.locateTime+leadD61;
            if(input_.TryLocateGripPair(targetD61,gripsD61,flagsD61)){
                displayD62.policyEpoch=handPolicyD61;
                displayD62.headTime=displayD62.baseHandTime=family.locateTime;
                displayD62.presentHandTime=targetD61;
                for(unsigned i=0;i<2;++i){const auto& g=gripsD61[i];
                    displayD62.tracked[i]=(flagsD61[i]&(valid|tracked))==(valid|tracked);
                    displayD62.hand[i]={{g.position.x,g.position.y,g.position.z},{g.orientation.x,g.orientation.y,g.orientation.z,g.orientation.w}};
                }
                locatedDisplayD62=true;
            }
        }
        familyPublishedD61=tracking59c6::PublishPrepared(family,&completedR24,
            [&](const tracking59c6::Frame& sealed) noexcept {
                if(!locatedDisplayD62||!sealed.tracked[0]||sealed.held[0]||
                   source59c6.epoch!=tracking59c6::epoch.load(std::memory_order_acquire))return;
                displayD62.sourceEpoch=sealed.epoch;displayD62.sequence=sealed.sequence;
                displayD62.tick=sealed.tick;displayD62.ns=sealed.ns;
                for(unsigned i=0;i<2;++i)displayD62.tracked[i]=displayD62.tracked[i]&&sealed.tracked[i+1]&&!sealed.held[i+1];
                (void)hand_present_d61::Publish(displayD62);
            });
        }else{
            // OFF takes the ordinary publication path without an optional
            // packet, callback capture, extra pose copies or runtime locates.
            familyPublishedD61=tracking59c6::Publish(family,familyPolicyR24?&completedR24:nullptr);
        }
        if(familyPublishedD61&&familyPolicyR24&&hj1::enabled.load(std::memory_order_acquire)&&
           familyPolicyR24==hand_smooth_h50::policyEpoch.load(std::memory_order_acquire))
            cameraFamilyR24=tracking59c6::CompletedCameraFamilyR24(completedR24,familyPolicyR24);
    }
}

#endif
