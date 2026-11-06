// Exact A7EA5E -> A7EA76 synchronous update envelope. These observers never
// change registers, pose memory, eligibility or native return values.
struct PredictionSource126 {
    void* action=nullptr;void* pose=nullptr;void* character=nullptr;
    std::uint64_t tick=0,epoch=0,request=0;int bone=-1;
    ph::Pose world{},joint{};bool captured=false;
    PoseGetFn wristGetterH31=nullptr;std::uintptr_t skeletonH31=0;
    ph::Pose wristH31{};int wristBoneH31=-1,handH31=-1;bool wristCapturedH31=false;
};
static thread_local PredictionSource126 g_prediction_source126;
static void PredictionSourceBegin126(void* driver,void* pose) noexcept {
    auto& s=g_prediction_source126;s={};
    if(g_weapon_control_mode.load()!=7||g_prediction_contract120.load()!=WhBase()||
       g_prediction_fault120.load()||!retail_build112::Ready(WhBase()))return;
    ReadScope101R2 reads;void* action=nullptr;void* player=nullptr;void* henry=nullptr;
    const auto manager=g_henry_item_attach_manager.load();
    if(!driver||!pose||!manager||!Read(static_cast<unsigned char*>(driver)+0x18,action)||!action||
       !Read(action,player)||!Read(static_cast<unsigned char*>(manager)+0x38,henry)||!henry||player!=henry)return;
    const auto address=reinterpret_cast<std::uintptr_t>(pose);
    if(address<std::uintptr_t(kPoseInCharInstanceOffset))return;
    auto* character=reinterpret_cast<void*>(address-kPoseInCharInstanceOffset);void* animation=nullptr;
    PoseGetFn reference=nullptr,current=nullptr;
    // Existing exact-retail character and getter identities, not SDK slots.
    if(!ValidateCharInstance(character,animation)||!ValidateRawPoseApi(pose,reference,current))return;
    s.action=action;s.pose=pose;s.character=character;s.tick=GetTickCount64();
    s.epoch=g_pose_epoch6689.load();s.request=g_weapon_request_epoch6694.load();
    s.wristGetterH31=current;
    void* skeletonH31=nullptr;
    if(Read(static_cast<unsigned char*>(pose)+kPoseDefaultSkeletonOffset,skeletonH31)&&PhysVft6698(skeletonH31,kCDefaultSkeletonVftRva))
        s.skeletonH31=reinterpret_cast<std::uintptr_t>(skeletonH31);
}
static void PredictionSourceCapture126(void* action,void* pose,const QuatT28* returned) noexcept {
    auto& s=g_prediction_source126;
    if(!s.action||s.action!=action||s.pose!=pose)return;
    ReadScope101R2 reads;std::int16_t bone=-1;QuatT28 joint{},world{};
    if(!Read(static_cast<unsigned char*>(action)+0xC,bone)||bone<0||
       !PhysCopySweep112(returned,joint)||
       !PhysCopySweep112(reinterpret_cast<QuatT28*>(static_cast<unsigned char*>(action)+0x20),world)){s={};return;}
    s.bone=bone;s.joint=PhysPose6698(joint);s.world=PhysPose6698(world);s.captured=true;
    // Read the native wrist from the SAME exact validated pose as its returned
    // collision bone. No bone index, bilateral offset or animation pose is
    // inferred from the selected native hand number.
    kcd2vr::fist_action_h12::Binding fistH31{};CommittedFrame6695 completedH31{};int handH31=-1,parentH31=-1;
    if(FistContactRequestedH31()&&kcd2vr::fist_action_h12::current(fistH31,GetTickCount64())&&
       Read(static_cast<unsigned char*>(action)+0x40C,handH31)&&handH31>=0&&handH31<2&&handH31==fistH31.nativeHand&&
       g_committed_frame6695.read(completedH31)&&completedH31.stamp.actor==reinterpret_cast<std::uintptr_t>(s.character)&&
       completedH31.stamp.epoch==s.epoch&&completedH31.fistSkeletonH31==s.skeletonH31&&s.skeletonH31&&
       completedH31.fistSlotBoneH31[handH31]==bone&&completedH31.fistSlotValidH31[handH31]){
        const int wristH31=completedH31.fistWristBoneH31[handH31];QuatT28 nativeWristH31{};
        if(wristH31>=0&&wristH31<int(completedH31.fistJointCountH31)&&
           ReadParentDirect(reinterpret_cast<void*>(s.skeletonH31),bone,parentH31)&&parentH31==wristH31&&
           ReadQuat(s.wristGetterH31,pose,wristH31,nativeWristH31)){
            s.wristH31=PhysPose6698(nativeWristH31);s.wristBoneH31=wristH31;s.handH31=handH31;s.wristCapturedH31=ph::finite(s.wristH31);
        }
    }
    if(fistH31.intent.serial&&PredictionCapture120()){
        static thread_local std::uint64_t nextH31=0;const auto nowH31=std::uint64_t(GetTickCount64());
        if(nowH31>=nextH31){nextH31=nowH31+250;char textH31[560]{};
            std::snprintf(textH31,sizeof(textH31),
                "H31 FIST_SOURCE operation=READ_CURRENT_NATIVE_WRIST captured=%d nativeHand=%d descriptorBone=%d actualParent=%d semanticWristR/L=%d/%d semanticItemR/L=%d/%d character=%p skeleton=%p completedSkeleton=%p sourceEpoch=%llu completedEpoch=%llu sourceSample=%llu writes=NONE",
                s.wristCapturedH31?1:0,handH31,bone,parentH31,completedH31.fistWristBoneH31[0],completedH31.fistWristBoneH31[1],
                completedH31.fistSlotBoneH31[0],completedH31.fistSlotBoneH31[1],s.character,reinterpret_cast<void*>(s.skeletonH31),
                reinterpret_cast<void*>(completedH31.fistSkeletonH31),(unsigned long long)s.epoch,(unsigned long long)completedH31.stamp.epoch,
                (unsigned long long)completedH31.stamp.sample);diagnostic_trace::Write(textH31);}
    }
}
static bool PredictionSourceMatches126(void* action,int bone,const PhysFrame6698& frame,
    const PhysSocket101& socket,ph::Pose authored,ph::Pose world,ph::Pose& expected) noexcept {
    const auto& s=g_prediction_source126;const auto now=std::uint64_t(GetTickCount64());
    if(!s.captured||s.action!=action||s.bone!=bone||s.character!=socket.character||
       s.epoch!=frame.input.bodyEpoch||s.request!=frame.request||now<s.tick||now-s.tick>50||
       pm::length(s.world.p-world.p)>.0001||pm::angle(s.world.q,world.q)>.0001)return false;
    // A7CEE0 composes this returned model-space bone with entity world, with
    // no guessed origin and no scale inserted into its native calculation.
    expected=ph::compose(s.world,s.joint);
    return ph::finite(expected)&&pm::length(expected.p-authored.p)<=.002&&pm::angle(expected.q,authored.q)<=.002;
}
static void PredictionSourceBeginMid126(DetourModKit::hook::MidContext& c) noexcept {
    using namespace DetourModKit::hook;
    PredictionSourceBegin126(reinterpret_cast<void*>(gpr(c,Gpr::Rbx)),reinterpret_cast<void*>(gpr(c,Gpr::Rdi)));
}
static void PredictionSourceReadMid126(DetourModKit::hook::MidContext& c) noexcept {
    using namespace DetourModKit::hook;
    PredictionSourceCapture126(reinterpret_cast<void*>(gpr(c,Gpr::Rdi)),reinterpret_cast<void*>(gpr(c,Gpr::R15)),
        reinterpret_cast<const QuatT28*>(gpr(c,Gpr::Rax)));
}
static void PredictionSourceEndMid126(DetourModKit::hook::MidContext&) noexcept {g_prediction_source126={};}
