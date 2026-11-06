// Two exact Henry joints at the EXISTING qualified native palette callback.
// Every read is inside a finite Shift+F2 burst. Raw palette bytes are not pixels.
static bool MotionDescriptorD68(const CoherentTransaction6689& tx,void* desc,void* pose,void* owner,void* character,void* output) noexcept {
    void *p=nullptr,*o=nullptr,*c=nullptr,*palette=nullptr;
    return desc&&pose&&owner&&character&&output&&tx.motionDescriptorD68==reinterpret_cast<std::uintptr_t>(desc)&&
        Read(static_cast<unsigned char*>(desc),p)&&Read(static_cast<unsigned char*>(desc)+8,o)&&
        Read(static_cast<unsigned char*>(desc)+0x10,c)&&Read(static_cast<unsigned char*>(desc)+0x20,palette)&&
        p==pose&&o==owner&&c==static_cast<unsigned char*>(character)+8&&palette==output;
}
static void MotionCoreBeforeD68(CoherentTransaction6689& tx,void* desc,void* pose,void* owner,void* character,
    void* output,const RigMap& rig,std::uint32_t count,PoseGetFn get,const TrackingSnapshot& ts,bool committed) noexcept {
    namespace md=kcd2vr::diagnostics::motion_d68;
    if(!tx.motionSessionD68||!md::current(tx.motionSessionD68))return;
    tx.motionCommittedD68=committed;tx.motionSourceNsD68=ts.timeNs;tx.motionSourceTickD68=ts.tick;tx.motionSampleD68=ts.sequence;
    tx.motionTrackingEpochD68=ts.source_epoch;tx.motionInputTickD68=ts.input_tick;tx.motionTargetXrNsD68=ts.poseTime;
    const bool same=MotionDescriptorD68(tx,desc,pose,owner,character,output);
    const auto& s=g_pose6689;
    const bool staged=s.ready&&s.owner==owner&&s.character==character&&s.count==count&&count<=kPoseCapacity6689;
    for(int side=0;side<2;++side){md::Event e{};if(!MotionEventD68(tx,e,md::CoreBefore))return;e.side=side;
        const int joint=side?rig.hand_l:rig.hand_r;const bool valid=joint>=0&&unsigned(joint)<count&&count<=kPoseCapacity6689;
        QuatT28 actual{};const bool have=same&&valid&&get&&ReadQuat(get,pose,joint,actual);
        const bool haveStaged=staged&&valid;
        e.flags=(have?1u:0u)|(haveStaged?2u:0u)|(committed?4u:0u)|
            (haveStaged&&tx.selected[joint]?8u:0u)|(haveStaged&&tx.attempted[joint]?16u:0u)|(get?32u:0u)|(same?64u:0u)|tx.motionHandFlagsD68[side];
        unsigned n=0;auto add=[&](const QuatT28& value){for(float v:value.q)e.v[n++]=v;for(float v:value.t)e.v[n++]=v;};
        add(actual);add(haveStaged?s.work[joint]:QuatT28{});add(haveStaged?s.original[joint]:QuatT28{});e.v[n++]=float(joint);e.count=n;
        (void)md::push(e);
    }
}
static void MotionCoreAfterD68(const CoherentTransaction6689& tx,void* desc,void* pose,void* owner,void* character,
    void* output,const RigMap& rig,std::uint32_t count,bool committed) noexcept {
    namespace md=kcd2vr::diagnostics::motion_d68;
    if(!tx.motionSessionD68||!md::current(tx.motionSessionD68))return;
    const bool same=MotionDescriptorD68(tx,desc,pose,owner,character,output);
    const auto& s=g_pose6689;
    const bool staged=s.ready&&s.owner==owner&&s.character==character&&s.count==count&&count<=kPoseCapacity6689;
    for(int side=0;side<2;++side){md::Event e{};if(!MotionEventD68(tx,e,md::CoreAfter))return;e.side=side;
        const int joint=side?rig.hand_l:rig.hand_r;const bool valid=joint>=0&&unsigned(joint)<count&&count<=kPoseCapacity6689;
        Dq32 raw{};const bool have=tx.traceNativeReturnedH29&&same&&valid&&Read(static_cast<unsigned char*>(output)+std::size_t(joint)*sizeof(Dq32),raw);
        const bool haveStaged=staged&&valid;
        e.flags=(have?1u:0u)|(haveStaged?2u:0u)|(committed?4u:0u)|(tx.traceNativeReturnedH29?8u:0u)|(same?16u:0u)|tx.motionHandFlagsD68[side];
        unsigned n=0;for(float f:raw.a)e.v[n++]=f;for(float f:raw.b)e.v[n++]=f;
        const auto expected=haveStaged?s.work[joint]:QuatT28{};for(float f:expected.q)e.v[n++]=f;for(float f:expected.t)e.v[n++]=f;
        e.v[n++]=float(joint);e.count=n;(void)md::push(e);
    }
}
