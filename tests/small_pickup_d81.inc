static bool SmallInstantPickupAllowedD81(std::uint64_t now) noexcept {
    return kcd2vr::instant_d72::Interactions()&&g_instant_pickup_ready_d72.load(std::memory_order_acquire)&&
        PropInstantContextD78(now)&&!action_time_owner59c11::NativePickupOutstandingD72();
}
static std::uint32_t SmallPickupAdmissionR24(std::uint64_t key,bool contact) noexcept {
    if(contact)return SmallContactAdmissionR24(key);
    namespace L=kcd2vr::lab58;namespace P=kcd2vr::lab_prop58;
    auto now=std::uint64_t(GetTickCount64());const auto tick=g_stage_tick596.load();
    const bool captureD85R5R1=kcd2vr::diagnostics::capture168::active(
        kcd2vr::diagnostics::capture168::Domain::Interaction,now);
    const char* phaseD85R5R1="READINESS_OR_CONTEXT";
    std::uint32_t returnedGramsD85R5R1=0;
    const auto noteD85R5R1=[&]() noexcept {
        namespace cap=kcd2vr::diagnostics::capture168;
        if(!captureD85R5R1||!cap::active(cap::Domain::Interaction,now))return;
        static thread_local kcd2vr::diagnostics::interaction_receipt59c10::Gate gate;
        const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire);
        if(!gate.observe(session,now,{std::uintptr_t(key),0,-1,int(returnedGramsD85R5R1)},100,128,1000))return;
        char line[320]{};
        std::snprintf(line,sizeof(line),
            "D85R5R1 F5_PICKUP_PREPARE_NATIVE key=%016llX phase=%s grams=%u prepared=NOT_EXECUTED_BY_NATIVE_READER",
            (unsigned long long)key,phaseD85R5R1,returnedGramsD85R5R1);
        diagnostic_trace::Write(line);
    };
    struct ReceiptD85R5R1 {const decltype(noteD85R5R1)& note;~ReceiptD85R5R1(){note();}} receiptD85R5R1{noteD85R5R1};
    const bool instant=SmallInstantPickupAllowedD81(now);
    const bool physicsDriveD85R5R2=prop5916::enabled.load(std::memory_order_acquire);
    const auto allowed=[&](std::uint64_t at){return instant?SmallInstantPickupAllowedD81(at):
        L::active(L::Sub::PropWake)&&L::f9Rung.load()==3&&!L::uiBlocked.load()&&g_grab_mode60.load()==2&&
        kcd2vr::workstation178r20c1::ownsWrist(at);};
    // Static inventory items need a native rigid body in both hold modes.
    // The kinematic mode already uses PropWorldHand596 in BuildStagedPickup596;
    // requiring the velocity-drive option here prevented its preparation.
    // Keep the separate legacy F9 experiment's existing physics requirement.
    if(!allowed(now)||!retail_build112::Ready(WhBase())||g_prop_replay596||
       (physicsDriveD85R5R2&&g_prop_fault5916c1.load())||
       (!instant&&!physicsDriveD85R5R2)||!(g_lifecycle_coverage175.load()&1u))return 0;
    phaseD85R5R1="CALLBACK_THREAD_OR_FRAME";
    if(g_stage_thread596.load()!=GetCurrentThreadId()||!tick||now<tick||now-tick>80||
       (instant&&!g_committed_pose_valid6689.load()))return 0;
    phaseD85R5R1="PLAYER_IDENTITY";
    std::uint32_t actor=0;auto* henry=g_last_player_entity.load();
    if(!henry||!Read(static_cast<unsigned char*>(henry)+0xC,actor)||!LifecycleHenry165(actor))return 0;
    // Station/carry presentation has its own actual tool/resource owner.
    phaseD85R5R1="STATION_OR_EXISTING_HOLD";
    if(HeldInteractionVisual43())return 0;
    PropLock596 lock;if(!lock.owned||g_prop596.id)return 0;
    phaseD85R5R1="ITEM_INSTANCE";
    void* item=SmallPickupItemR24(key);void* definition=nullptr;void* link=nullptr;std::uint32_t id=0;
    unsigned char guid[16]{};
    if(!item||!Read(static_cast<unsigned char*>(item)+0x48,definition)||!definition||
       !Read(static_cast<unsigned char*>(definition)+8,guid)||
       !Read(static_cast<unsigned char*>(item)+0x88,link)||!link||
       !Read(static_cast<unsigned char*>(link)+0x30,id)||!id)return 0;
    // D85 interact: native definition Weight (S_PickableItemClass+0x58) for the
    // allowed pickable classes; the retained R24 rows stay as a fallback only.
    phaseD85R5R1="NATIVE_ITEM_CLASS_OR_WEIGHT";
    std::uintptr_t definitionVftD85=0;float weightD85=0;
    const auto base=WhBase();
    if(!Read(definition,definitionVftD85)||!Read(static_cast<unsigned char*>(definition)+0x58,weightD85))return 0;
    auto grams=P::pickupNativeGramsD85(definitionVftD85>=base?definitionVftD85-base:0,weightD85);
    if(!grams)grams=P::pickupMassGramsR24(guid);
    if(!grams)return 0;
    phaseD85R5R1="ENTITY_OR_STATIC_BODY";
    void* entity=EntityById6694(id);std::uint32_t liveId=0;
    if(!LifecycleEntity165(reinterpret_cast<std::uintptr_t>(entity),liveId)||liveId!=id||entity==henry)return 0;
    void* body=nullptr;std::uintptr_t vft=0;
    if(!SmallPickupBodyR24(entity,body)||(body&&!Read(body,vft)))return 0;
    // Existing rigid bodies stay on their original path. Never replace living,
    // articulated, unknown physics types or a different native proxy contract.
    if(body&&vft!=WhBase()+0x3A578C0)return 0;
    phaseD85R5R1="CURRENT_COMMITTED_HAND_FRAME";
    CommittedFrame6695 f{};TrackingSnapshot t{};Matrix34V6550 tm{};
    if(!ReadCommittedPacket6695(f)||!f.stamp.tick||
       f.stamp.actor!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())||
       f.stamp.epoch!=g_pose_epoch6689.load())return 0;
    if(instant){
        if(!f.mapFoldTrackedM39||!f.headEvidence122)return 0;
        t.tick=f.stamp.tick;t.allowed=t.head_valid=true;t.right_grip=f.mapGripM39[0];t.left_grip=f.mapGripM39[1];
    }else if(!ReadTrackingSnapshot(t))return 0;
    now=std::uint64_t(GetTickCount64());
    if(now<f.stamp.tick||now-f.stamp.tick>80)return 0;
    phaseD85R5R1="TRACKING_OR_RENDER_BOUNDS";
    if(!t.allowed||!t.head_valid||t.head_held||!t.tick||now<t.tick||now-t.tick>80||!GetEntityWorldTmV6552(entity,id,tm))return 0;
    void* renderProxy=nullptr;float localBounds[6]{};
    if(!SmallPickupBoundsR24(entity,renderProxy,localBounds)||!P::pickupBoundsSmallR24(localBounds,tm.m))return 0;
    const pm::Vec3 point{tm.m[3],tm.m[7],tm.m[11]};if(!pm::finite(point))return 0;
    phaseD85R5R1="CURRENT_HAND_ADMISSION";
    int requestedSideD85R5R2=-1;
    const bool requestedHandD85R5R2=instant&&
        action_time_owner59c11::RequestedPickupSideD85(id,requestedSideD85R5R2);
    unsigned pickupHandMask=0;for(int side=0;side<2;++side){ph::Pose hand{};
        if(requestedHandD85R5R2&&side!=requestedSideD85R5R2)continue;
        if(f.valid[side]&&(!physicsDriveD85R5R2||f.propRequestedValid5916c1[side])&&TrackingHandUsable111(side)&&
           !(side?t.left_held:t.right_held)&&PropHandFree596(side)&&(side?t.left_grip:t.right_grip)>=.58&&
           !pickup_input_d80::refusal(f,side,FistInputOwned101())&&
           PropPickupHandD81(f,side,physicsDriveD85R5R2,hand)&&pm::length(hand.p-point)<.25)pickupHandMask|=1u<<side;
    }
    phaseD85R5R1="CURRENT_IDENTITY_RECHECK";
    void* currentDefinition=nullptr;void* currentLink=nullptr;unsigned char currentGuid[16]{};
    if(!pickupHandMask||!allowed(now)||
       g_last_player_entity.load()!=henry||!LifecycleHenry165(actor)||SmallPickupItemR24(key)!=item||
       EntityById6694(id)!=entity||!Read(static_cast<unsigned char*>(item)+0x48,currentDefinition)||currentDefinition!=definition||
       !Read(static_cast<unsigned char*>(definition)+8,currentGuid)||std::memcmp(currentGuid,guid,16)||
       !Read(static_cast<unsigned char*>(item)+0x88,currentLink)||currentLink!=link)return 0;
    void* finalBody=nullptr;std::uintptr_t finalVft=0;
    if(!SmallPickupBodyR24(entity,finalBody)||finalBody!=body||
       (finalBody&&(!Read(finalBody,finalVft)||finalVft!=vft)))return 0;
    // Requalify the same current render box/world after all lookup/body leaves.
    // This is a bounded synchronous observation, not a retained slot/geometry lease.
    void* currentRenderProxy=nullptr;float currentBounds[6]{};Matrix34V6550 currentWorld{};
    if(!SmallPickupBoundsR24(entity,currentRenderProxy,currentBounds)||currentRenderProxy!=renderProxy||
        std::memcmp(currentBounds,localBounds,sizeof(localBounds))||
        !GetEntityWorldTmV6552(entity,id,currentWorld)||std::memcmp(currentWorld.m,tm.m,sizeof(tm.m))||
        EntityById6694(id)!=entity)return 0;
    // The new native render reads cannot authenticate an earlier item/body chain.
    if(SmallPickupItemR24(key)!=item||
        !Read(static_cast<unsigned char*>(item)+0x48,currentDefinition)||currentDefinition!=definition||
        !Read(static_cast<unsigned char*>(definition)+8,currentGuid)||std::memcmp(currentGuid,guid,16)||
        !Read(static_cast<unsigned char*>(item)+0x88,currentLink)||currentLink!=link||
        !SmallPickupBodyR24(entity,finalBody)||finalBody!=body||
        (finalBody&&(!Read(finalBody,finalVft)||finalVft!=vft))||
        !GetEntityWorldTmV6552(entity,id,currentWorld)||std::memcmp(currentWorld.m,tm.m,sizeof(tm.m))||
        EntityById6694(id)!=entity)return 0;
    phaseD85R5R1="LATEST_FRAME_AND_HAND_FRESHNESS";
    const auto finishNow=std::uint64_t(GetTickCount64());
    if(finishNow<now||finishNow-tick>80||finishNow-f.stamp.tick>80||finishNow-t.tick>80||
        prop5916::enabled.load(std::memory_order_acquire)!=physicsDriveD85R5R2||
        g_stage_thread596.load()!=GetCurrentThreadId()||g_stage_tick596.load()!=tick||
        !TrackingHeadUsable111(finishNow)||
        (t.source_epoch&&t.source_epoch!=tracking59c6::epoch.load(std::memory_order_acquire)))return 0;
    for(int side=0;side<2;++side)if((pickupHandMask&(1u<<side))&&
        (!TrackingHandUsable111(side,finishNow)||!PropHandFree596(side)))pickupHandMask&=~(1u<<side);
    if(!pickupHandMask)return 0;
    phaseD85R5R1="FINAL_NATIVE_ADMISSION";
    std::uint32_t currentId=0;
    returnedGramsD85R5R1=Read(static_cast<unsigned char*>(link)+0x30,currentId)&&currentId==id&&
        allowed(finishNow)&&
        g_last_player_entity.load()==henry&&LifecycleHenry165(actor)&&g_committed_pose_valid6689.load()&&
        f.stamp.actor==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())&&
        f.stamp.epoch==g_pose_epoch6689.load()?grams:0;
    return returnedGramsD85R5R1;
}
// Ordinary instant pickup needs the same narrow static-item preparation as
// the F9 experiment. Prepare the native interception on the current native
// frame BEFORE Lua PickableItem.Use: its preparation precedes item:OnUsed,
// so installing only from OnUsed made the first static pickup fail by design.
// Exact pin/partial-install failures remain terminal for that installation.
static void UpdateSmallPickupReaderR24(bool active) noexcept {
    const bool requested=kcd2vr::instant_d72::Interactions()&&
        !kcd2vr::instant_pickup_d72::stopping.load(std::memory_order_acquire);
    static bool nativeAttemptedD85R5R1=false;
    if(!requested&&!g_instant_pickup_ready_d72.load(std::memory_order_acquire))nativeAttemptedD85R5R1=false;
    if(requested&&!g_instant_pickup_ready_d72.load(std::memory_order_acquire)&&
       !nativeAttemptedD85R5R1&&retail_build112::Ready(WhBase())&&action_time_owner59c11::Installed()){
        nativeAttemptedD85R5R1=true;
        (void)InstallInstantPickupD72();
    }
    active=active||(requested&&g_instant_pickup_ready_d72.load(std::memory_order_acquire));
    static bool registered=false;
    static unsigned attempts=0;
    static std::uint64_t nextAttempt=0;
    if(active==registered){attempts=0;nextAttempt=0;return;}
    const auto now=std::uint64_t(GetTickCount64());
    // A busy installer may refuse a registration. Commit the edge only after
    // success; four bounded attempts avoid a silent permanent lost edge while
    // preserving the door installer's retained-failure policy. No frame scan.
    if(active&&(attempts>=4||(nextAttempt&&now<nextAttempt)))return;
    if(kcd2vr::door_signal19c4r4::SetSmallPickupReaderR24(active?&SmallPickupAdmissionR24:nullptr)){
        registered=active;attempts=0;nextAttempt=0;
    }else{++attempts;nextAttempt=now+500;}
}
