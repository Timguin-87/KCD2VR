static bool BuildStagedPickup596(void* controller,std::uint32_t actor,bool prepareInstantD72,StagedProp596* preparedD72=nullptr) noexcept {
    // Native grasp must receive its own Attach event; do not intercept the same
    // pickup first with the older kinematic/rigid-body F5 experiment.
    if(!prepareInstantD72&&g_graspEnabled13&&g_graspEnabled13()&&g_grab_mode60.load(std::memory_order_acquire)!=2)return false; // C61 staged hold owns this pickup in explicit PHYSICAL mode
    auto now=std::uint64_t(GetTickCount64());const auto frameTick=g_stage_tick596.load();
    const auto loadEpisodeR96=kcd2vr::inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire);
    StagedProp596 p{};p.controller=controller;p.actor=actor;
    CommittedFrame6695 f{};TrackingSnapshot t{};Matrix34V6550 tm{};
    std::uintptr_t observedBody=0,observedVft=0;
    unsigned validMask=0,trackingMask=0,freeMask=0,gripMask=0;
    double reach[2]{-1,-1};
    const auto reject=[&](const char* reason){
        if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return false;
        using namespace kcd2vr::diagnostics::interaction_receipt59c10;
        static thread_local Gate receipt;
        const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
        // Literal refusal reasons and the copied controller are diagnostic keys;
        // never dereference a failed identity just to give a refusal a target ID.
        if(!receipt.observe(session,now,{reinterpret_cast<std::uintptr_t>(controller)^reinterpret_cast<std::uintptr_t>(reason),p.id,p.side,0},0))return false;
        const auto base=WhBase();
        const auto rva=observedVft>=base&&observedVft-base<0x6000000?observedVft-base:0;
        char line[880]{};std::snprintf(line,sizeof(line),
            "C10 F5_PICKUP_REFUSED session=%llu reason=%s actor=%u target=%u controller=%p wuid=%016llX side=%d epoch=%llu thread=%lu stageThread=%lu frameAge=%llu committedAge=%llu trackingAge=%llu body=%p bodyVftRva=%llX validMask=%u trackingMask=%u freeMask=%u gripMask=%u reachR=%.3f reachL=%.3f observed=%llu changes=%llu coalesced=%llu",
            (unsigned long long)session,reason,actor,p.id,controller,(unsigned long long)p.key,p.side,
            (unsigned long long)kcd2vr::workstation178r20c1::experimentEpoch36.load(),
            static_cast<unsigned long>(GetCurrentThreadId()),static_cast<unsigned long>(g_stage_thread596.load()),
            (unsigned long long)age(now,frameTick),(unsigned long long)age(now,f.stamp.tick),(unsigned long long)age(now,t.tick),
            reinterpret_cast<void*>(observedBody),(unsigned long long)rva,validMask,trackingMask,freeMask,gripMask,reach[0],reach[1],
            (unsigned long long)receipt.seen,(unsigned long long)receipt.changes,(unsigned long long)receipt.coalesced);
        diagnostic_trace::Write(line);return false;
    };
    if(g_prop_replay596)return reject("NATIVE_STOW_REENTRY");
    if(prop5916::enabled.load()&&g_prop_fault5916c1.load())return reject("PHYSICS_DRIVE_FAULT_LATCH");
    if(prepareInstantD72?!PropInstantContextD78(now):!kcd2vr::workstation178r20c1::ownsWrist(now))
        return reject(prepareInstantD72?"INSTANT_GAMEPLAY_OWNER_NOT_ADMITTED":"WRIST_OWNER_NOT_ADMITTED");
    if(!retail_build112::Ready(WhBase()))return reject("RETAIL_FINGERPRINT");
    if(!LifecycleHenry165(actor))return reject("ACTOR_NOT_EXACT_HENRY");
    if(!prepareInstantD72&&g_stage_thread596.load()!=GetCurrentThreadId())return reject("WRONG_GAME_FRAME_THREAD");
    if(!frameTick)return reject("GAME_FRAME_NOT_PUBLISHED");
    if(now<frameTick)return reject("GAME_FRAME_FUTURE");
    if(now-frameTick>80)return reject("GAME_FRAME_STALE");
    PropLock596 lock;if(!lock.owned)return reject("LOCK_BUSY");
    // Repeated Use must not commit the held item, or pick a second one.
    if(g_prop596.id)return !prepareInstantD72;
    if(!PhysVft6698(controller,0x3EC85E0)||
       !Read(static_cast<unsigned char*>(controller)+0x38,p.entity)||
       !LifecycleEntity165(reinterpret_cast<std::uintptr_t>(p.entity),p.id)||
       !Read(static_cast<unsigned char*>(controller)+0x58,p.item)||!PhysVft6698(p.item,0x3A2DA88)||
       !Read(static_cast<unsigned char*>(p.item)+0x30,p.key)||PhysEmpty6698(p.key))return reject("NATIVE_ITEM_IDENTITY");
    // These are distinct contracts. The C10 test produced four ambiguous
    // POSE_OR_RIGID_BODY refusals: another pickup cannot distinguish a missing
    // committed wrist from a static/non-rigid object without this receipt.
    if(!ReadCommittedPacket6695(f))return reject("COMMITTED_WRIST_UNAVAILABLE");
    if(prepareInstantD72){
        // Grip, body, hand poses and head validity are one completed semantic
        // publication. A newer live tracking read cannot complete this packet.
        if(!f.mapFoldTrackedM39||!f.headEvidence122)
            return reject("INSTANT_COMPLETED_TRACKING_UNAVAILABLE");
        t.tick=f.stamp.tick;t.allowed=true;t.head_valid=true;
        t.right_grip=f.mapGripM39[0];t.left_grip=f.mapGripM39[1];
    }else if(!ReadTrackingSnapshot(t))return reject("TRACKING_SNAPSHOT_UNAVAILABLE");
    if(!t.allowed||!t.head_valid)return reject("TRACKING_CONTEXT_DISALLOWED");
    if(!GetEntityWorldTmV6552(p.entity,p.id,tm))return reject("OBJECT_WORLD_POSE_UNAVAILABLE");
    if(!PropBody596(p.entity,&observedBody,&observedVft))return reject("OBJECT_NOT_VERIFIED_RIGID_BODY");
    now=std::uint64_t(GetTickCount64()); // sample AFTER these exact copied sources/native reads
    if(!f.stamp.tick||now<f.stamp.tick||now-f.stamp.tick>80)return reject("COMMITTED_POSE_AGE");
    if(!t.tick||now<t.tick||now-t.tick>80)return reject("TRACKING_AGE");
    ph::Pose object{};const V3 x{tm.m[0],tm.m[4],tm.m[8]},y{tm.m[1],tm.m[5],tm.m[9]},z{tm.m[2],tm.m[6],tm.m[10]};
    p.scale[0]=Len(x);p.scale[1]=Len(y);p.scale[2]=Len(z);
    for(float v:p.scale)if(!std::isfinite(v)||v<.001f||v>100.f)return reject("OBJECT_SCALE");
    const Q4 q=QFromBasis(Normalize(x),Normalize(y),Normalize(z));
    object={{q.x,q.y,q.z,q.w},{tm.m[3],tm.m[7],tm.m[11]}};if(!ph::finite(object))return reject("OBJECT_POSE");
    double best=.25; // local reach, even when a distant ray offered native pickup
    p.physics=prop5916::enabled.load(std::memory_order_acquire);
    ph::Pose pickupHand{};
    const bool selectedFistsD80=FistInputOwned101();const char* inputRefusalD80=nullptr;
    // The transported Use identifies the acquiring hand. A second closed hand
    // being closer must not steal that item from the hand that requested it.
    // Older native/F5 acquisitions without that receipt retain nearest-hand
    // selection; a qualified request never falls across to the other hand.
    int requestedSideD85R5R2=-1;
    const bool requestedHandD85R5R2=prepareInstantD72&&
        action_time_owner59c11::RequestedPickupSideD85(p.id,requestedSideD85R5R2);
    for(int side=0;side<2;++side){
        if(requestedHandD85R5R2&&side!=requestedSideD85R5R2)continue;
        const double grip=side?t.left_grip:t.right_grip;
        const unsigned bit=1u<<side;
        if(!f.valid[side])continue;validMask|=bit;
        if(!TrackingHandUsable111(side))continue;trackingMask|=bit;
        if(!PropHandFree596(side))continue;freeMask|=bit;
        if(grip<.58)continue;gripMask|=bit;
        ph::Pose hand{};if(!PropPickupHandD81(f,side,p.physics,hand))continue;
        const double distance=pm::length(hand.p-object.p);reach[side]=distance;
        if(distance<best){
            if(const char* why=pickup_input_d80::refusal(f,side,selectedFistsD80)){inputRefusalD80=why;continue;}
            best=distance;p.side=side;pickupHand=hand;
        }
    }
    if(p.side<0)return reject(inputRefusalD80?inputRefusalD80:
        requestedHandD85R5R2?"REQUESTING_HAND_NOT_QUALIFIED":"NO_LOCAL_GRIP_WITHIN_25CM");
    if(!StageLeafPins596(kPropLeaves596))return reject("NATIVE_LEAF_PINS");
    // Physics drive consumes the requested wrist. Using a collision-corrected
    // visible wrist here would add that correction a second time at acquisition
    // and move the object even though the controller had not moved.
    p.offset=ph::compose(ph::inverse(pickupHand),object);
    p.epoch=prepareInstantD72?f.stamp.epoch:kcd2vr::workstation178r20c1::experimentEpoch36.load();p.sample=f.stamp.sample;
    p.body=PropBody596(p.entity);
    p.character=f.stamp.actor;p.bodyEpoch=f.stamp.epoch;
    if(p.physics){
        prop5916::Dynamics d{};
        if(!f.propRequestedValid5916c1[p.side])return reject("CONTROLLER_PROP_POSE_UNAVAILABLE");
        if(!PropPhysicsPins5916C1())return reject("PHYSICS_LEAF_PINS");
        if(!PropDynamics5916C1(p.body,d))return reject("RIGID_DYNAMICS_UNAVAILABLE");
        // A world item retains its actual momentum on acquisition. No SetWorldTM,
        // rigid-body replacement or unconditional zero-velocity write.
        p.drive.seed(ph::compose(PropRequestedWorld5916C1(f,p.side),p.offset),f.stamp.sample,f.stamp.tick);
        p.observed=object;p.observedValid=true;
    }
    now=std::uint64_t(GetTickCount64());
    if(now<f.stamp.tick||now-f.stamp.tick>80||now<t.tick||now-t.tick>80||
       loadEpisodeR96!=kcd2vr::inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire)||
       kcd2vr::inventory_lifecycle_t8::loadingObserved18.load(std::memory_order_acquire)||
       (prepareInstantD72?!PropInstantContextD78(now):!kcd2vr::workstation178r20c1::ownsWrist(now)))
        return reject("FINAL_SOURCE_OR_OWNER_CHANGED");
    p.hold.begin(p.epoch,now);p.sourceTickR96=f.stamp.tick;p.loadEpisodeR96=loadEpisodeR96;
    if(p.physics&&!prepareInstantD72)PropPublish5916C1(p,f,object,now);
    else if(!p.physics&&!prepareInstantD72&&!PropVelocity596(p.entity))return reject("NATIVE_VELOCITY_CLEAR");
    if(prepareInstantD72){if(preparedD72)*preparedD72=p;return preparedD72!=nullptr;}
    // The original pickup is deferred: do not pretend an authored native grip
    // was accepted. Existing calibrated free-hand fingers follow the real Grip.
    g_prop596=p;PropNote596("ACQUIRED_WORLD_ITEM",p,true);return true;
}
