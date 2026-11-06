// Same pinned A7C624 input/interpolation boundary as weapons. The native
// predictor, shape setup, collision query, request and damage still run once.
// No controller-only candidate, new attack, deferred native pointer or hit call.
struct FistPredictionLeaseH31 {std::uint64_t generation=0,sample=0;};
static thread_local FistPredictionLeaseH31 g_fist_prediction_lease_h31;
static bool FistPredictionPrepareH31(void* wrapper,float dt,std::uintptr_t caller,PredictionScope120& s) noexcept {
    namespace fh=kcd2vr::fist_action_h12;fh::Binding binding{};const auto now=std::uint64_t(GetTickCount64()),base=WhBase();
    // R10: a refusal while the VR fist is valid also stops the native sweep (no animated-bone hit in its place).
    const auto no=[&](const char* reason){s.context121=reason;s.reason=2;
        s.fistSuppressR10=kcd2vr::fist_reach_r10::refused(reason,kcd2vr::fist_reach_r10::vrOnlyHits.load(std::memory_order_relaxed))==
            kcd2vr::fist_reach_r10::Sweep::Suppress;return false;};
    namespace fr10=kcd2vr::fist_reach_r10;
    if(caller!=base+0xA7CDEE||!std::isfinite(dt)||dt<=0||dt>.05f)return no("FIST_NATIVE_CALLER_DT");
    if(!base||g_prediction_contract120.load()!=base||g_prediction_fault120.load()||!retail_build112::Ready(base))return no("FIST_RETAIL_CONTRACT");
    if(!FistSelected101()||g_fist_fault101.load()||g_native_fault104.load()||diagnostics::lab::NativeReference()||
       !g_gameplay_allowed.load()||g_ctx_dialogue.load()||g_ctx_menu.load()||g_ctx_cutscene.load()||g_ctx_scripted.load()||
       animation_filter::nativeBodyOwned112.load()||animation_filter::PlayerNativeBodyRequiredForMotion122())return no("FIST_CURRENT_GAMEPLAY_OWNER");
    if(!fh::current(binding,now)||!fh::punch(binding.nativeType)||binding.intent.policy!=g_attack_timing_policy22.load()||
       binding.intent.epoch!=g_pose_epoch6689.load()||binding.intent.request!=g_fist_epoch101.load())return no("FIST_ADMITTED_ACTION");
    const int side=binding.intent.side,hand=binding.nativeHand;
    if(side<0||side>1||hand<0||hand>1)return no("FIST_HAND_IDENTITY");
    NativeOwner104 owner{};FistOwner101 empty{};TrackingSnapshot tracking{};
    if(!NativeResolve104(reinterpret_cast<void*>(binding.intent.player),owner)||
       reinterpret_cast<std::uintptr_t>(owner.actor)!=binding.intent.actor||reinterpret_cast<std::uintptr_t>(owner.state)!=binding.intent.state||
       owner.epoch!=binding.intent.epoch||!FistReadOwner101(empty)||!empty.empty||empty.player!=owner.player||
       empty.request!=binding.intent.request)return no("FIST_EMPTY_NATIVE_OWNER");
    if(!ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||!tracking.right_valid||!tracking.left_valid||
       tracking.head_held||tracking.right_held||tracking.left_held||now<tracking.tick||now-tracking.tick>60)return no("FIST_CURRENT_TRACKING");
    const float grip=side?tracking.left_grip:tracking.right_grip,trigger=side?tracking.left_trigger:tracking.right_trigger;
    if(!(grip>.60f&&trigger>.60f))return no("FIST_CURRENT_SQUEEZE");
    std::uintptr_t callback=0;fh::FailureH31 why{};
    if(!ClockRead50(binding.helper+0x18,&callback,8))return no("FIST_NATIVE_CALLBACK_READ");
    if(!fh::receiver(binding,callback,base,ClockRead50,&why))return no(fh::name(why));
    FistContactFrameH11 palms{};CommittedFrame6695 body{};
    // R10: this punch's farthest real segment (Henry's reach), replayed once if the live pair can't be used.
    FistPeakR10 peakR10{};const bool peakReadR10=FistPeakMailboxR10(side).read(peakR10);
    static thread_local std::uint64_t spentR10=0;bool replayR10=false;
    const auto structuralR10=[&](const FistContactFrameH11& c){return c.completedH31&&c.valid[side]&&c.pairH31[side]&&
       c.request==binding.intent.request&&c.epoch==binding.intent.epoch&&c.player==owner.player&&c.previousTickH31[side]&&
       c.tick>=c.previousTickH31[side]&&c.tick-c.previousTickH31[side]<=60&&c.sample>c.previousSampleH31[side]&&
       kcd2vr::motion_time::elapsed(c.previousTickH31[side],c.tick,c.previousTimeNsH31[side],c.timeNsH31)>0;};
    const bool peakUsableR10=fr10::peakReplay.load(std::memory_order_relaxed)&&peakReadR10&&peakR10.side==side&&
       fr10::usable(peakR10.peak,binding.intent.sample,now,spentR10)&&peakR10.palms.gestureSampleH31[side]==binding.intent.sample&&
       structuralR10(peakR10.palms);
    const auto takePeakR10=[&](){palms=peakR10.palms;replayR10=true;};
    if(!g_fist_contact_h11.read(palms)||!structuralR10(palms)||now<palms.tick||now-palms.tick>60){
        if(!peakUsableR10)return no("FIST_ORIGINAL_COMPLETED_PAIR");takePeakR10();}
    if(HandClaimBlockedT24(side,palms.character,palms.epoch,g_weapon_request_epoch6694.load()))return no("FIST_COMPETING_HAND_CLAIM_T24");
    if(!replayR10&&(palms.gestureSampleH31[side]!=binding.intent.sample||palms.sample<binding.intent.sample||palms.tick<binding.intent.tick)){
        if(!peakUsableR10)return no("FIST_ORIGINAL_GESTURE_PAIR");takePeakR10();}
    // H2H: the live pair is current but already going back behind this punch's peak -> sweep the real reach once.
    if(!replayR10&&peakUsableR10&&peakR10.palms.skeletonH31==palms.skeletonH31&&peakR10.palms.slotBoneH31[side]==palms.slotBoneH31[side]&&
       fr10::retracting(peakR10.peak,palms.previousSlotH31[side].p,palms.worldSlotH31[side].p,palms.gestureDirectionH31[side]))takePeakR10();
    if(replayR10&&(palms.sample<binding.intent.sample||palms.tick<binding.intent.tick))return no("FIST_ORIGINAL_GESTURE_PAIR");
    s.sample=palms.sample;s.frameTick176=palms.tick;s.epoch176=palms.epoch;s.request176=palms.request;
    if(!g_committed_frame6695.read(body)||body.stamp.epoch!=palms.epoch||body.stamp.actor!=palms.character||body.stamp.sample<palms.sample||
       !body.valid[side]||body.fistSkeletonH31!=palms.skeletonH31||body.fistJointCountH31!=palms.jointsH31||
       body.fistWristBoneH31[side]!=palms.wristBoneH31[side]||body.fistSlotBoneH31[side]!=palms.slotBoneH31[side])return no("FIST_COMPLETED_CHARACTER_TOPOLOGY");
    void* descriptor=nullptr;void* player=nullptr;void* collisionBack=nullptr;int descriptorHand=-1;std::int16_t bone=-1;
    unsigned char hit=1,admitted=0,enabled=0,mode=255;QuatT28 authored{},world{},inverse{};
    if(!Read(wrapper,descriptor)||!descriptor||!Read(descriptor,player)||player!=owner.player||
       !Read(static_cast<unsigned char*>(owner.actor)+0x218,s.collisions)||!PhysVft6698(s.collisions,0x3B91300)||
       !Read(static_cast<unsigned char*>(s.collisions)+8,collisionBack)||collisionBack!=owner.actor)return no("FIST_NATIVE_DESCRIPTOR_OWNER");
    auto* action=static_cast<unsigned char*>(descriptor);s.wrapper=wrapper;s.action=descriptor;
    if(!Read(action+0xC,bone)||!Read(action+0x40C,descriptorHand)||descriptorHand!=hand||
       !Read(action+0x308,hit)||!Read(static_cast<unsigned char*>(wrapper)+0x10,admitted)||!Read(action+0x411,enabled)||
       !Read(action+0x2D0,mode)||admitted!=1||enabled!=1||(mode!=0&&mode!=2)||hit)return no("FIST_NATIVE_PREDICTOR_ADMISSION");
    s.hand=hand;s.bone=bone;s.admitted147=admitted;s.enabled147=enabled;s.actionMode147=mode;s.entryTick147=now;
    const auto& source=g_prediction_source126;
    if(!source.captured||!source.wristCapturedH31||source.action!=descriptor||source.bone!=bone||source.handH31!=hand||
       source.character!=reinterpret_cast<void*>(palms.character)||source.epoch!=palms.epoch||source.request!=g_weapon_request_epoch6694.load()||
       source.skeletonH31!=palms.skeletonH31||source.wristBoneH31!=palms.wristBoneH31[hand]||
       bone!=palms.slotBoneH31[hand]||now<source.tick||now-source.tick>50)return no("FIST_SAME_UPDATE_NATIVE_WRIST_BONE");
    int sourceParent=-1,targetParent=-1;
    if(!palms.slotValidH31[side]||!ReadParentDirect(reinterpret_cast<void*>(palms.skeletonH31),bone,sourceParent)||sourceParent!=palms.wristBoneH31[hand]||
       !ReadParentDirect(reinterpret_cast<void*>(palms.skeletonH31),palms.slotBoneH31[side],targetParent)||targetParent!=palms.wristBoneH31[side])return no("FIST_SOURCE_TARGET_SLOT_PARENT");
    if(!PhysCopySweep112(reinterpret_cast<QuatT28*>(action+0xAC),authored)||!PhysCopySweep112(reinterpret_cast<QuatT28*>(action+0x20),world)||
       !PhysCopySweep112(reinterpret_cast<QuatT28*>(action+0x3C),inverse))return no("FIST_NATIVE_TRANSFORM_READ");
    const auto actual=PhysPose6698(authored),nativeWorld=PhysPose6698(world),expected=ph::compose(source.world,source.joint);
    s.inverse166=kcd2vr::native_transform166::InversePair(nativeWorld,PhysPose6698(inverse));
    if(!s.inverse166.valid||!ph::finite(expected)||pm::length(source.world.p-nativeWorld.p)>.0001||pm::angle(source.world.q,nativeWorld.q)>.0001||
       pm::length(expected.p-actual.p)>.002||pm::angle(expected.q,actual.q)>.002)return no("FIST_EXACT_SOURCE_PIVOT_TRANSFORM");
    // Separate per-side frames: the native RIGHT shape is not positioned by a
    // guessed RIGHT-wrist offset on the LEFT wrist. Its original setup receives
    // the admitted side's actual completed semantic slot transform, which owns
    // that side's axes, anatomy and current hand shape.
    const auto sourceOffset=ph::compose(ph::inverse(source.wristH31),source.joint);
    const auto targetOffset=ph::compose(ph::inverse(palms.modelWristH31[side]),palms.modelSlotH31[side]);
    if(!ph::finite(sourceOffset)||!ph::finite(targetOffset)||pm::length(sourceOffset.p)>.25||pm::length(targetOffset.p)>.25||
       !std::isfinite(palms.scaleH31)||palms.scaleH31<=.1||palms.scaleH31>=10)return no("FIST_BOUNDED_SOURCE_TARGET_OFFSETS");
    auto from=palms.previousSlotH31[side],to=palms.worldSlotH31[side];
    if(!ph::finite(from)||!ph::finite(to)||pm::length(to.p-from.p)>.25||pm::angle(from.q,to.q)>2.2||
       !pm::unit(palms.gestureDirectionH31[side]))return no("FIST_BOUNDED_ORIGINAL_WORLD_PAIR");
    auto& lease=g_fist_prediction_lease_h31;
    // R27: the original native action/phase above owns its collision window.
    // A current bounded VR segment is valid even when a hook curves away from
    // its onset direction. Rejecting it here silently swallowed native sweeps.
    // Keep monotonic sample ownership; never substitute an older segment.
    if(!replayR10&&lease.generation==binding.generation&&palms.sample<lease.sample){
        if(!peakUsableR10||peakR10.palms.skeletonH31!=palms.skeletonH31||peakR10.palms.slotBoneH31[side]!=palms.slotBoneH31[side])
            return no("FIST_OLD_PAIR_NOT_REPLAYED");
        takePeakR10();from=palms.previousSlotH31[side];to=palms.worldSlotH31[side];
        s.sample=palms.sample;s.frameTick176=palms.tick;s.epoch176=palms.epoch;s.request176=palms.request;
        if(!ph::finite(from)||!ph::finite(to)||pm::length(to.p-from.p)>.25||pm::angle(from.q,to.q)>2.2)return no("FIST_BOUNDED_ORIGINAL_WORLD_PAIR");}
    if(replayR10)to.p=fr10::replayEnd(from.p,to.p); // our hand collision stopped the fist on the skin
    s.input.player=owner.player;s.input.target=PhysQuat6698(to);
    s.input.previous=PhysQuat6698(!replayR10&&lease.generation==binding.generation&&palms.sample==lease.sample?to:from);
    const auto z=pm::rotate(to.q,{0,0,-1});const auto axis=pm::unit(pm::cross(z,std::abs(z.z)<.8?pm::Vec3{0,0,1}:pm::Vec3{0,1,0}));
    if(!axis)return no("FIST_SUBDIVISION_AXIS");
    const auto direction=pm::rotate(pm::from_rotation_vector(*axis*std::max(.01,pm::angle(PhysPose6698(s.input.previous).q,to.q))),z);
    s.input.direction[0]=float(direction.x);s.input.direction[1]=float(direction.y);s.input.direction[2]=float(direction.z);
    s.authored=actual;s.expected=expected;s.desired=to;s.sourceCorrelated126=true;
    s.pivotDistance=pm::length(expected.p-actual.p);s.pivotAngle=pm::angle(expected.q,actual.q);s.correlated122=2;
    s.fistH31=true;s.fistGenerationH31=binding.generation;s.fistGestureSampleH31=binding.intent.sample;
    s.fistContactH31=palms;
    s.fistPreviousTickH31=palms.previousTickH31[side];s.fistTickH31=palms.tick;
    s.fistReplayR10=replayR10;
    if(replayR10){spentR10=binding.intent.sample;fr10::replayed.fetch_add(1,std::memory_order_relaxed);
        s.context121="PASS_FIST_PEAK_REPLAY_R10";s.reason=0;
        lease={binding.generation,lease.generation==binding.generation?std::max(lease.sample,palms.sample):palms.sample};return true;}
    fr10::current.fetch_add(1,std::memory_order_relaxed);
    s.context121="PASS_FIST_ORIGINAL_WORLD_PAIR";s.reason=0;lease={binding.generation,palms.sample};return true;
}
