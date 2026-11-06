static std::uint64_t g_native_held_pose_admission_t4=0;
static void NativeHeldRetirePoseAdmissionT4(CoherentTransaction6689& tx) noexcept {
    const auto serial=heldT4::AdmissionSerial();
    if(serial!=g_native_held_pose_admission_t4){
        PhysRetireMotion30c1("NATIVE_SOLVER_ADMISSION_CHANGED",__LINE__);tx.physicalState={};
        g_native_held_pose_admission_t4=serial;
    }
    tx.nativeHeldAdmissionT4=serial;
}
enum class NativeHeldStageResultT4 {Fallback,Waiting,Applied};
static NativeHeldStageResultT4 StageNativeHeldT4(CoherentTransaction6689& tx) noexcept {
    kcd2vr::diagnostics::physics_d42::Scope traceD42(tx.physicalInput.tick,tx.physicalInput.sample,[]()noexcept{return std::uint64_t(GetTickCount64());});
    auto& receiptD42=traceD42.receipt;
    using OutcomeD42=kcd2vr::diagnostics::physics_d42::Outcome;
    NativeHeldRetirePoseAdmissionT4(tx); // also before OFF/no-collider fallback
    if(!heldT4::Enabled()&&!heldT4::HasUnretiredCollider())return NativeHeldStageResultT4::Fallback;
    auto& pi=tx.physicalInput;const auto& ts=tx.tracking;
    NativeHeldSourceT4 nativeSourceT4{};heldT4::Achieved achievedT4{};
    const bool achievedReadT4=heldT4::Read(achievedT4);
    const bool colliderBusyT4=heldT4::HasUnretiredCollider();
    const auto& profileT4=tx.physicalProfiles[0];
    const bool eligibleT4=traceD42.check(0,heldT4::Enabled())&&traceD42.check(1,g_native_held_source_t4.read(nativeSourceT4))&&traceD42.check(2,nativeSourceT4.valid)&&
        traceD42.check(3,NativeHeldSourceCurrentT4(nativeSourceT4.input))&&traceD42.check(4,profileT4.key==nativeSourceT4.input.owner.key&&
        profileT4.generation==nativeSourceT4.input.owner.generation&&profileT4.entity==nativeSourceT4.input.owner.entityId&&
        tx.physicalCorrelation126[0].stamp.actor==nativeSourceT4.input.owner.character)&&
        traceD42.check(5,profileT4.primary&&profileT4.geometry)&&traceD42.check(6,pi.haveWorld&&pi.allowed)&&traceD42.check(7,pi.tracked[0]&&pi.intentTracked[0])&&
        traceD42.check(8,!ts.head_held&&!ts.right_held&&!ts.left_held)&&traceD42.check(9,!tx.acquisitionRequired57)&&
        traceD42.check(10,tx.physicalEquipment.handKnown[1]&&!tx.physicalEquipment.handKey[1])&&traceD42.check(11,!g_phys_state6698.weapon[0].impale178r15);
    if(traceD42.on()){
        const auto& s=nativeSourceT4.input;receiptD42.outcome=OutcomeD42::Fallback;
        receiptD42.enabled=(receiptD42.passed&1)!=0;receiptD42.sourceRead=(receiptD42.passed&2)!=0;
        receiptD42.busy=colliderBusyT4;receiptD42.achievedRead=achievedReadT4;receiptD42.admission=tx.nativeHeldAdmissionT4;
        receiptD42.key=s.owner.key;receiptD42.generation=s.owner.generation;receiptD42.entity=s.owner.entityId;
        receiptD42.request=s.owner.request;receiptD42.epoch=s.owner.epoch;receiptD42.lease=s.colliderLease;
        receiptD42.trackingEpoch=s.trackingEpoch;receiptD42.cancelGeneration=s.cancelGeneration;
        receiptD42.sourceTick=achievedT4.sourceTick;receiptD42.observedTick=achievedT4.observedTick;receiptD42.sourceSample=achievedT4.sample;
        receiptD42.bodyGeneration=achievedT4.bodyGeneration;receiptD42.worldGeneration=achievedT4.worldGeneration;receiptD42.bodyId=achievedT4.bodyId;
        receiptD42.state=unsigned(achievedT4.state);receiptD42.reason=unsigned(achievedT4.reason);receiptD42.poseValid=achievedT4.poseValid;
        receiptD42.ownerMatch=achievedReadT4&&achievedT4.owner==s.owner;receiptD42.leaseMatch=achievedReadT4&&achievedT4.colliderLease==s.colliderLease;
        receiptD42.trackingMatch=achievedReadT4&&achievedT4.trackingEpoch==ts.source_epoch;receiptD42.cancelMatch=achievedReadT4&&achievedT4.cancelGeneration==s.cancelGeneration;
        receiptD42.achieved[0]=achievedT4.weaponWorld.p.x;receiptD42.achieved[1]=achievedT4.weaponWorld.p.y;receiptD42.achieved[2]=achievedT4.weaponWorld.p.z;
        receiptD42.achievedValid=achievedReadT4&&achievedT4.poseValid&&ph::finite({achievedT4.weaponWorld.q,achievedT4.weaponWorld.p});
    }
    if(eligibleT4){
        auto inputT4=nativeSourceT4.input;inputT4.sample=pi.sample;inputT4.tick=pi.tick;
                inputT4.trackingEpoch=ts.source_epoch;inputT4.allowed=true;inputT4.tracked=true;
        const auto trackingFromModelH28=ph::compose(pi.intentHands[0],ph::inverse(pi.hands[0]));
        inputT4.intentFrameH28={{pi.world.q,pi.world.p},{trackingFromModelH28.q,trackingFromModelH28.p},pi.worldScale,
            pi.intentTracked[0]&&ph::finite(pi.intentHands[0])&&ph::finite(pi.hands[0])&&ph::finite(pi.world)};
        // The measured grip is in Henry model units; convert its offset
        // into the world metres consumed by the rigid solver.
        auto gripT4=profileT4.grip[0];gripT4.p=gripT4.p*pi.worldScale;
        inputT4.gripLocal={gripT4.q,gripT4.p};
        auto handT4=pi.hands[0];handT4.p=handT4.p*pi.worldScale;
        const auto targetT4=ph::compose(ph::compose(pi.world,handT4),ph::inverse(gripT4));
        inputT4.targetWorld={targetT4.q,targetT4.p};
        if(traceD42.on()){receiptD42.target[0]=targetT4.p.x;receiptD42.target[1]=targetT4.p.y;receiptD42.target[2]=targetT4.p.z;receiptD42.targetValid=ph::finite(targetT4);}
        g_native_held_attachment_self_t4.store(inputT4.owner.attachmentObject,std::memory_order_release);
        g_native_held_attachment_t4.store(inputT4.owner.attachment,std::memory_order_release);
        heldT4::Submit(inputT4);g_native_held_release_inhibit_t4.store(true,std::memory_order_release);
        tx.nativeHeldInputT4=inputT4;
                heldT4::ContactOwnerH24 contactT4{};const bool contactReadT4=heldT4::ReadContactOwnerH24(contactT4);
        if(traceD42.on()){receiptD42.contactRead=contactReadT4;receiptD42.contactActive=contactReadT4&&contactT4.active;}
        kcd2vr::native_held_pose_t4::ApplyReasonD42 applyReasonD42{};
        if(colliderBusyT4&&achievedReadT4&&kcd2vr::native_held_pose_t4::apply(inputT4,achievedT4,GetTickCount64(),pi,tx.physicalProfiles,tx.physicalOutput,tx.physicalState,
            contactReadT4?&contactT4:nullptr,heldT4::AdmissionSerial(),traceD42.on()?&applyReasonD42:nullptr)){
            receiptD42.outcome=OutcomeD42::Applied;receiptD42.applyReason=unsigned(applyReasonD42);
            tx.nativeHeldActiveT4=true;tx.nativeHeldReceiptT4=achievedT4;return NativeHeldStageResultT4::Applied;
        }else if(colliderBusyT4){
            // Creation spans several physics phases. Keep this exact
            // fresh intent alive while waiting, without a second writer.
            // Admission's authoritative control bit is published before its
            // receipt; a contended/previous receipt is also a wait, not cancel.
            const bool invalidActiveT4=achievedReadT4&&achievedT4.state==heldT4::State::Active&&achievedT4.poseValid&&
                achievedT4.owner==inputT4.owner&&achievedT4.colliderLease==inputT4.colliderLease&&
                achievedT4.trackingEpoch==inputT4.trackingEpoch&&achievedT4.cancelGeneration==inputT4.cancelGeneration;
            const bool pendingT4=!invalidActiveT4;
            receiptD42.outcome=pendingT4?OutcomeD42::WaitingCreate:OutcomeD42::WaitingInvalid;receiptD42.applyReason=unsigned(applyReasonD42);
            if(invalidActiveT4)heldT4::Retire(achievedT4.owner);
            tx.stage=pendingT4?"T4_WAIT_CURRENT_CREATE_CONFIG":"T4_WAIT_ACHIEVED_OR_RETIREMENT";return NativeHeldStageResultT4::Waiting;
        }
    }else if(colliderBusyT4){receiptD42.outcome=OutcomeD42::Revoked;heldT4::Cancel();tx.stage="T4_SOURCE_REVOKED_WAIT_COLLIDER_RETIREMENT";return NativeHeldStageResultT4::Waiting;}
    return NativeHeldStageResultT4::Fallback;
}
