struct StagedProp596 {
    void* entity=nullptr;void* controller=nullptr;void* item=nullptr;
    std::uint32_t id=0,actor=0;std::uint64_t key=0,epoch=0,sample=0;
    int side=-1;ph::Pose offset{};float scale[3]{};stage596::Hold hold{};
    void* body=nullptr;std::uintptr_t character=0;std::uint64_t bodyEpoch=0;
    bool physics=false;prop5916::Controller drive{};std::uint64_t reportTick=0;
    std::uint64_t sourceTickR96=0,loadEpisodeR96=0; // original qualified source; never refreshed during an observation gap
    ph::Pose observed{};bool observedValid=false;
    prop5916::KinematicTrailD85 kinematicD85{}; // D85 interact: legacy kinematic throw
    // Only populated by the exact native immediate transfer boundary. The
    // accepted request is retained once; no animation/action pointer is held.
    bool instantD72=false,noticeD72=false;
    std::uint64_t instantTokenD72=0;
    void* managerD72=nullptr;void* playerD72=nullptr;
    void* containerD72=nullptr;void* ownerD72=nullptr;void* inventoryD72=nullptr;
    std::uint32_t countD72=0;
    alignas(8) unsigned char acceptedD72[0xC8]{};
    kcd2vr::instant_pickup_d72::Transfer transferD72=nullptr;
};
static StagedProp596 g_prop596;
static bool PropCommitInstantD72(const StagedProp596&) noexcept;
static void PropRetireInstantD72(const StagedProp596&) noexcept;
static kcd2vr::load_fault_recovery_r96::Fault g_prop_fault5916c1;
static std::atomic<DWORD> g_stage_thread596{0};
static std::atomic<std::uint64_t> g_stage_tick596{0};
static std::atomic_flag g_prop_lock596=ATOMIC_FLAG_INIT;
static thread_local bool g_prop_replay596=false;
struct PropLock596 {bool owned=!g_prop_lock596.test_and_set(std::memory_order_acquire);
    ~PropLock596(){if(owned)g_prop_lock596.clear(std::memory_order_release);}};

static bool PropInstantContextD78(std::uint64_t now) noexcept {
    return g_cal_state.load(std::memory_order_acquire)==2&&g_gameplay_allowed.load(std::memory_order_acquire)&&
        !kcd2vr::instant_pickup_d72::stopping.load(std::memory_order_acquire)&&
        !diagnostics::lab::NativeReference()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
        !g_ctx_cutscene.load()&&!g_ctx_reading.load()&&!g_ctx_scripted.load()&&
        !animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175()&&
        WorkstationContextRefusal19c4r2(now,true)==nullptr;
}

static bool PropPickupHandD81(const CommittedFrame6695& f,int side,bool physics,ph::Pose& out) noexcept {
    if(side<0||side>1||!f.valid[side]||(physics&&!f.propRequestedValid5916c1[side]))return false;
    out=physics?PropRequestedWorld5916C1(f,side):PropWorldHand596(f,side);
    return ph::finite(out);
}

static void StagedPropTick596() noexcept {
    PropLock596 lock;if(!lock.owned||!g_prop596.id)return;
    auto& p=g_prop596;auto now=std::uint64_t(GetTickCount64());
    // This world-item request owns no constraint. A positively observed load
    // retires its private claim, without dereferencing/replaying the old item.
    if(p.loadEpisodeR96&&p.loadEpisodeR96!=kcd2vr::inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire)){
        PropRetireInstantD72(p);p={};prop5916::clearPresentation();return;
    }
    if(!PropIdentity596(p)){PropNote596("IDENTITY_ENDED",p,false);PropRetireInstantD72(p);p={};prop5916::clearPresentation();return;}
    CommittedFrame6695 f{};TrackingSnapshot t{};
    // Keep the exact short-circuit order of the ownership contract. These
    // receipts use the observations already required by the consumer.
    const auto eligibility=[&]() -> const char* {
        if(p.instantD72&&kcd2vr::instant_pickup_d72::stopping.load(std::memory_order_acquire))return "INSTANT_SHUTDOWN";
        if(p.instantD72){
            if(!PropInstantContextD78(now))return "INSTANT_GAMEPLAY_OWNER_ENDED";
            if(p.side<0||p.side>1||!p.instantTokenD72||
               kcd2vr::mutual_admission_t25::pickupClaimsD72[p.side].load(std::memory_order_acquire)!=p.instantTokenD72)
                return "INSTANT_HAND_CLAIM_ENDED";
        }else if(!kcd2vr::workstation178r20c1::ownsWrist(now))return "WRIST_OWNER_ENDED";
        const bool haveFrameR96=ReadCommittedPacket6695(f);now=std::uint64_t(GetTickCount64());
        if(!haveFrameR96)return "COMMITTED_WRIST_UNAVAILABLE";
        if(!f.valid[p.side])return "COMMITTED_HAND_INVALID";
        if(f.stamp.epoch!=p.bodyEpoch||f.stamp.actor!=p.character)return "BODY_IDENTITY_CHANGED";
        if(const char* why=pickup_input_d80::refusal(f,p.side,FistInputOwned101()))return why;
        if(p.instantD72){
            if(!f.mapFoldTrackedM39||!f.headEvidence122)return "INSTANT_COMPLETED_TRACKING_UNAVAILABLE";
            t.tick=f.stamp.tick;t.allowed=true;t.head_valid=true;
            t.right_grip=f.mapGripM39[0];t.left_grip=f.mapGripM39[1];
        }else{const bool haveTrackingR96=ReadTrackingSnapshot(t);now=std::uint64_t(GetTickCount64());
            if(!haveTrackingR96)return "TRACKING_SNAPSHOT_UNAVAILABLE";}
        if(!t.allowed||!t.head_valid)return "TRACKING_CONTEXT_DISALLOWED";
        if(!TrackingHandUsable111(p.side))return "HAND_TRACKING_UNUSABLE";

        if(t.head_held||(p.side?t.left_held:t.right_held))return "TRACKING_GRACE_NOT_FRESH";
        if(!PropHandFree596(p.side))return "NATIVE_HAND_ITEM_CHANGED";
        if(p.physics!=prop5916::enabled.load())return "PHYSICS_MODE_CHANGED";
        if(p.physics&&!f.propRequestedValid5916c1[p.side])return "CONTROLLER_PROP_POSE_UNAVAILABLE";
        now=std::uint64_t(GetTickCount64());
        if(!t.tick||now<t.tick||now-t.tick>80)return "TRACKING_AGE";
        if(!f.stamp.tick||now<f.stamp.tick||now-f.stamp.tick>80)return "COMMITTED_POSE_AGE";
        if(p.instantD72&&!PropInstantContextD78(now))return "INSTANT_GAMEPLAY_OWNER_ENDED";
        return nullptr;
    };
    const char* endReason=eligibility();const bool eligible=endReason==nullptr;
    // A missing publication/temporarily untracked hand is not a native owner
    // replacement. Keep this exact claim only inside its ORIGINAL source age.
    // No drive, stow progress, presentation publication or lease renewal occurs.
    const bool observationGapR96=endReason&&(
        std::strcmp(endReason,"COMMITTED_WRIST_UNAVAILABLE")==0||std::strcmp(endReason,"COMMITTED_HAND_INVALID")==0||
        std::strcmp(endReason,"HAND_TRACKING_UNUSABLE")==0||std::strcmp(endReason,"TRACKING_SNAPSHOT_UNAVAILABLE")==0||
        std::strcmp(endReason,"INSTANT_COMPLETED_TRACKING_UNAVAILABLE")==0||std::strcmp(endReason,"TRACKING_GRACE_NOT_FRESH")==0||
        std::strcmp(endReason,"CONTROLLER_PROP_POSE_UNAVAILABLE")==0);
    if(observationGapR96&&p.sourceTickR96&&now>=p.sourceTickR96&&now-p.sourceTickR96<=80)return;
    // Stow is a gesture of the tracked hand. Collision/prop presentation may
    // keep the displayed wrist outside the chest; that correction must not
    // veto the same deliberate gesture in a different collision mode. Both
    // poses and the chest come from this one completed publication. Legacy
    // kinematic packets without requested-pose evidence keep their old source.
    const pm::Vec3 chest{f.solvedHead122.x,f.solvedHead122.y,f.solvedHead122.z-.40};
    const double distance=eligible&&f.headEvidence122?pm::length(PhysPose6698(
        f.propRequestedValid5916c1[p.side]?f.propRequested5916c1[p.side]:f.allowed[p.side]).p-chest):100.;
    const auto decision=p.hold.update(p.instantD72?p.bodyEpoch:kcd2vr::workstation178r20c1::experimentEpoch36.load(),now,eligible,
        p.side?t.left_grip:t.right_grip,distance);
    if(decision!=stage596::Decision::Hold){
        const auto ended=p;p={};prop5916::clearPresentation(); // clear BEFORE native removal/reentry
        const bool stow=decision==stage596::Decision::Stow;
        const bool intentionalRelease=eligible&&std::isfinite(ended.side?t.left_grip:t.right_grip)&&
            (ended.side?t.left_grip:t.right_grip)<.30;
        // Intentional let-go keeps achieved body momentum. Loss/menu/epoch exits
        // stop our drive safely; no new controller-velocity throw is synthesized.
        // D85 interact: the legacy kinematic hold keeps its hand-carried velocity
        // on an intentional open-hand release; the same native SetVelocity.
        pm::Vec3 throwD85{};
        const bool kinematicThrowD85=!ended.physics&&intentionalRelease&&!stow&&ended.kinematicD85.release(now,throwD85);
        const bool settled=ended.physics&&intentionalRelease&&!stow?true:
            kinematicThrowD85?PropSetVelocity5916C1(PropBody596(ended.entity),throwD85):PropVelocity596(ended.entity);
        const bool ok=stow?(settled&&(ended.instantD72?PropCommitInstantD72(ended):PropCommit596(ended.controller,ended.actor))):settled;
        // The immediate commit re-reads the item's real inventory membership.
        // Only that successful transfer confirms "taken". The older animated
        // request merely admits an action and is not a completion receipt.
        // This user cue is independent of capture, and follows the held hand.
        const bool confirmedD84=stow&&ok&&ended.instantD72;
        const bool hapticD84=confirmedD84&&kcd2vr::haptics101::publish_event(
            ended.side,ended.key,.65f,65,now);
        if(stow&&kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,now)){
            char line[320]{};std::snprintf(line,sizeof(line),
                "D84 F5_STOW_CONFIRM side=%d entity=%u key=%016llX token=%llu nativeInventoryVerified=%d hapticQueued=%d gesture=COMPLETED_REQUESTED_WRIST",
                ended.side,ended.id,(unsigned long long)ended.key,(unsigned long long)ended.instantTokenD72,
                confirmedD84,hapticD84);diagnostic_trace::Write(line);
        }
        PropRetireInstantD72(ended);
        PropNote596(stow?"NATIVE_STOW_RETURN":(intentionalRelease?"DROP_RETAIN_BODY_VELOCITY":"CANCEL_TO_NATIVE_PHYSICS"),ended,ok,
            endReason?endReason:stow?"CHEST_STOW":intentionalRelease?"GRIP_RELEASE":"EXPERIMENT_EPOCH_OR_HOLD_TIMEOUT");return;
    }
    p.sourceTickR96=f.stamp.tick;
    if(p.sample==f.stamp.sample&&!p.physics)return;
    if(!p.physics){p.sample=f.stamp.sample;
        const auto placedD85=ph::compose(PropWorldHand596(f,p.side),p.offset);
        if(!PropPlace596(p,placedD85)){PropNote596("PLACEMENT_STOPPED",p,false);PropRetireInstantD72(p);p={};return;}
        p.kinematicD85.add(placedD85.p,f.stamp.tick);
        return;
    }
    prop5916::Dynamics d{};ph::Pose actual{};
    if(!PropDynamics5916C1(p.body,d)||!PropObjectPose5916C1(p,actual)){
        PropNote596("BODY_STATUS_ENDED",p,false);(void)PropSetVelocity5916C1(p.body);PropRetireInstantD72(p);p={};prop5916::clearPresentation();return;
    }
    const auto target=ph::compose(PropRequestedWorld5916C1(f,p.side),p.offset);
    now=std::uint64_t(GetTickCount64());
    auto staged=p.drive;const auto command=staged.update(target,actual,d,f.stamp.sample,f.stamp.tick,now);
    if(command.decision==prop5916::Decision::End){
        // Existing F5 end receipt only: distinguish the incoming source from
        // the last successful drive instead of logging its sample as the cause.
        if(kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,now)){
            char reason[224]{};std::snprintf(reason,sizeof(reason),
                "CONTINUITY inSample=%llu priorSample=%llu inTick=%llu priorTick=%llu ageMs=%llu separation=%.4f",
                (unsigned long long)f.stamp.sample,(unsigned long long)p.drive.sequence,
                (unsigned long long)f.stamp.tick,(unsigned long long)p.drive.tick,
                (unsigned long long)(now>=f.stamp.tick?now-f.stamp.tick:~std::uint64_t{}),command.separation);
            PropNote596("DRIVE_CONTINUITY_ENDED",p,false,reason);
        }
        (void)PropSetVelocity5916C1(p.body);PropRetireInstantD72(p);p={};prop5916::clearPresentation();return;
    }
    if(command.decision==prop5916::Decision::Duplicate){
        // The native body was already observed this callback. Publish that
        // result even when its controller clock has no new integration time;
        // PropPublish keeps the original source tick and never invents motion.
        PropPublish5916C1(p,f,actual,now);return;
    }
    const auto driveLoadR96=kcd2vr::load_fault_recovery_r96::Fault::capture();
    if(!PropSetVelocity5916C1(p.body,command.linear,command.angular)){
        // May have partially applied. Never retry it on a timer.
        g_prop_fault5916c1.record(driveLoadR96);PropNote596("BODY_DRIVE_FAULT",p,false);PropRetireInstantD72(p);p={};prop5916::clearPresentation();return;
    }
    p.drive=staged;p.sample=f.stamp.sample;
    PropPublish5916C1(p,f,actual,now); // observed pose, NOT extrapolated commanded position
    if(kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,now)&&
       (!p.reportTick||now-p.reportTick>=200)){
        const auto moved=p.observedValid?pm::length(actual.p-p.observed.p):0.;
        char line[512]{};std::snprintf(line,sizeof(line),
            "R59.16C1 F5_PROP_BODY entity=%u body=%p sample=%llu sourceAgeMs=%llu mass=%.3f observedV=%.3f observedW=%.3f deltaPosition=%.5f separation=%.4f commandV=%.3f nativeActionReturned=1 motion=CURRENT_BODY_OBSERVATION constraint=NONE damage=NONE",
            p.id,p.body,(unsigned long long)f.stamp.sample,(unsigned long long)(now-f.stamp.tick),d.mass,
            pm::length(d.linear),pm::length(d.angular),moved,command.separation,pm::length(command.linear));
        diagnostic_trace::Write(line);p.reportTick=now;p.observed=actual;p.observedValid=true;
    }
}
