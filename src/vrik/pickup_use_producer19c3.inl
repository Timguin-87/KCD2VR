// Production native Use dispatch; shared with the connected ownership fixture.
// C46 ordinary doors use a short native press/release while physical Grip is
// still held. The nearby Picking adapter and its attach experiment do not own
// this input. The native interactor still chooses and accepts the exact action.
// RunFrame may block and publishes input AFTER the loop-start clock. Capture
// a real current consumption clock here; never compare that newer XR sample to
// ladder_now_ms or promote a future stamp with max(now, stamp).
const auto consumeNow54=std::uint64_t(GetTickCount64());
door_hold65::Frame holdFrame65{};(void)door_hold65::frame.read(holdFrame65);
kcd2vr::diagnostics::door_receipt63::scope(door_use46::inputAvailable(f5Mode,comparison114)&&!alchemyPointer59c2&&pi.enabled&&!pi.menu,consumeNow54);
door_use46::Selection doorSelection46{};
interaction_alignment::HandCandidate doorCandidate46{};
// Direct native ray/AABB doors bypass the projection-stage hover publication.
// Use the matching native direct offer before a nearby secondary PickableItem;
// this still only arms a request and never substitutes native Use acceptance.
const bool directDoor64=interaction_alignment::NativeDirectDoorCandidate54r1(held114.owner,consumeNow54,doorCandidate46);
if(directDoor64||
   interaction_alignment::LastHandCandidate(doorCandidate46)){
    doorSelection46.ordinary=std::strcmp(doorCandidate46.cls,"AnimDoor")==0;
    doorSelection46.target=doorCandidate46.entity;doorSelection46.side=doorCandidate46.owner;doorSelection46.tick=doorCandidate46.tick;
    if(doorSelection46.ordinary){
        interaction_alignment::HandCandidate exact46{};
        if(interaction_alignment::ExactDoorCandidate38(doorCandidate46.entity,doorCandidate46.owner,consumeNow54,exact46)){
            doorSelection46={true,exact46.entity,exact46.owner,exact46.tick,exact46.sourceTick,exact46.sourceEpoch,
                exact46.sourceGeneration,exact46.sourcePlayer,{exact46.x,exact46.y,exact46.z},exact46.distance};
            doorSelection46.direct64=directDoor64;
        }else doorSelection46={}; // a physical surface cannot revive a refused exact native candidate
    }
}
if(doorSelection46.ordinary){
    door_surface65::Witness surface65{};const int side65=doorSelection46.side;
    if(side65<0||side65>1||!door_surface65::read(doorSelection46.target,holdFrame65.hand[side65],consumeNow54,surface65))
        doorSelection46={}; // known Door, but no physical near-surface permission; wait, never generic Use
    else {doorSelection46.point=surface65.point;doorSelection46.distance=surface65.distance;
        doorSelection46.sourceGeneration=surface65.source.sourceGeneration;doorSelection46.poseEpoch=surface65.source.poseEpoch;
        doorSelection46.poseTick=surface65.source.poseTick;doorSelection46.player=surface65.source.player;}
}
door_use46::Input doorInput46{};doorInput46.now=consumeNow54;doorInput46.press=heldUse114.pressed;
doorInput46.side=held114.owner;doorInput46.held=held114.hold;
if(held114.owner>=0&&(kcd2vr::hand_only_door_h40::Reserved()&(1u<<held114.owner)))doorInput46.held=false;
doorInput46.scoped54=door_use46::inputAvailable(f5Mode,comparison114)&&!alchemyPointer59c2&&pi.enabled&&!pi.menu;
doorInput46.experimentEpoch64=kcd2vr::workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire);
doorInput46.interactionMode64=f5Mode;
if(doorInput46.side>=0&&doorInput46.side<2&&xr_manager_){
    const int side46=doorInput46.side;const auto& hand46=pi.hand[side46];const auto& ray46=palms59c9.ray[side46];
    const auto& controller46=side46?xr_manager_->LeftController():xr_manager_->RightController();
    tracking59c6::Stamp stamp46{};const bool sampled46=xr_manager_->CopyInputSampleStamp59c6(stamp46);
    doorInput46.available=door_use46::inputAvailable(f5Mode,comparison114)&&!alchemyPointer59c2&&pi.enabled&&!pi.menu&&
        rayPublished116&&po.ray_owner==side46&&palms59c9.eligible[side46]&&hand46.tracked&&hand46.empty&&
        std::isfinite(controller46.grip)&&controller46.grip>=.30f&&sampled46&&door_use46::fresh(stamp46.tick,consumeNow54);
    doorInput46.waitingForRay49=door_use46::inputAvailable(f5Mode,comparison114)&&!alchemyPointer59c2&&pi.enabled&&!pi.menu&&
        hand46.tracked&&hand46.empty&&std::isfinite(controller46.grip)&&controller46.grip>=.30f&&
        sampled46&&door_use46::fresh(stamp46.tick,consumeNow54)&&!rayPublished116;
    doorInput46.poseTick=ray46.tick;doorInput46.poseEpoch=ray46.sourceEpoch;doorInput46.player=ray46.sourcePlayer;
    doorInput46.sourceGeneration=door_use46::inputMode(f5Mode)?interaction_alignment::DoorAcquisitionGeneration65():interaction_alignment::DoorSourceGeneration38();doorInput46.trackingEpoch=stamp46.epoch;
    doorInput46.motion={controller46.pose.pos_x,controller46.pose.pos_y,controller46.pose.pos_z};
    const auto& source65=holdFrame65.hand[side46];
    const bool raw65=doorInput46.scoped54&&door_hold65::current(source65,consumeNow54)&&source65.side==side46&&
        source65.press==heldUse114.pressed&&source65.grip>=.30f&&sampled46&&
        door_use46::fresh(stamp46.tick,consumeNow54)&&source65.trackingEpoch==stamp46.epoch&&
        source65.sourceGeneration==doorInput46.sourceGeneration&&source65.settingsEpoch==door_use46::settingsEpoch.load(std::memory_order_acquire)&&
        source65.experimentEpoch==doorInput46.experimentEpoch64&&source65.poseEpoch==ray46.sourceEpoch&&source65.player==ray46.sourcePlayer&&
        std::isfinite(controller46.grip)&&controller46.grip>=.30f;
    if(raw65){doorInput46.poseTick=source65.poseTick;doorInput46.poseEpoch=source65.poseEpoch;doorInput46.player=source65.player;
        doorInput46.sourceGeneration=source65.sourceGeneration;doorInput46.trackingEpoch=source65.trackingEpoch;}
    const bool point65=raw65&&rayPublished116&&po.ray_owner==side46&&
        palm59c9::ordinaryTransportLock64.current(palms59c9,side46,heldUse114.pressed,source65.sourceGeneration,
            source65.trackingEpoch,source65.settingsEpoch,source65.experimentEpoch,f5Mode,consumeNow54);
    doorInput46.available=point65;
    doorInput46.waitingForPoint65=raw65&&!point65; // current raw source, not a tracking/camera grace
}
if(doorInput46.available&&!palm59c9::ordinaryTransportLock64.current(palms59c9,held114.owner,heldUse114.pressed,
    doorInput46.sourceGeneration,doorInput46.trackingEpoch,door_use46::settingsEpoch.load(std::memory_order_acquire),
    doorInput46.experimentEpoch64,f5Mode,consumeNow54)){
    // The reference/settings family can retire between point publication and
    // this consumer. Do not queue Use from a mixed old-palm/new-input family.
    doorInput46.available=false;doorInput46.waitingForRay49=false;
}
door_hold65::Operation operation65{};
if(doorInput46.available&&door_hold65::operation.read(operation65)&&door_hold65::current(operation65,consumeNow54)&&
   operation65.paused&&doorInput46.side>=0&&doorInput46.side<2){
    const auto& hand65=holdFrame65.hand[doorInput46.side];door_surface65::Witness surface65{};
    if(hand65.press&&hand65.press!=operation65.request.inputPress65&&door_hold65::sameSource(operation65.source,hand65)&&
       door_surface65::read(operation65.request.id,hand65,consumeNow54,surface65)){
        const door_hold65::Resume resume65{operation65.request.serial,operation65.request.id,operation65.request.entity,hand65};
        if(door_hold65::resume.publish(resume65))doorInput46.resumeTarget65=operation65.request.id;
    }
}
const auto doorPlan46=doorUse46.update(doorInput46,doorSelection46);
// F5 copies the real admission/transport decisions, without changing their owner.
const auto observeCycleH4=[&](bool transport) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,consumeNow54))return;
    diagnostics::interaction_cycle_h4::ProducerExtra extra{};
    extra.contextKnown=true;extra.enabled=pi.enabled;extra.menu=pi.menu;extra.reserved=reserved114;
    extra.gripBlocked=gripMask116;extra.mode=f5Mode;extra.rayPublished=rayPublished116;
    extra.sentPulse=useSent116;extra.held=heldNow114;
    if(doorInput46.side>=0&&doorInput46.side<2){
        extra.tracked=pi.hand[doorInput46.side].tracked;extra.empty=pi.hand[doorInput46.side].empty;
        extra.grip=pi.hand[doorInput46.side].grip;
        if(xr_manager_)extra.trigger=(doorInput46.side?xr_manager_->LeftController():xr_manager_->RightController()).trigger;
    }
    diagnostics::interaction_cycle_h4::observeProducer(doorInput46,doorSelection46,doorPlan46,doorUse46,extra,transport);
};
observeCycleH4(false);
if(doorPlan46.owns&&doorPlan46.target&&doorInput46.available){
    // Publish the exact target lock BEFORE queuing the first pulse. Following
    // frames retain it in interaction_point32 while this same squeeze is live.
    palms59c9.locked=doorPlan46.side;palms59c9.lockedEntity=doorPlan46.target;
    interaction_alignment::SetPalmRays59c9(palms59c9);
}
interaction_alignment::HandCandidate pickupCandidate19c3{};
const bool pickupPulseEnabled19c3=!doorPlan46.owns&&(f5Mode==0||f5Mode==2)&&rayPublished116&&!alchemyPointer59c2&&
    po.ray_owner==held114.owner&&
    action_time_owner59c11::NativeGraspAvailable63();
if((f5Mode==0||f5Mode==2)&&rayPublished116&&!alchemyPointer59c2)(void)interaction_alignment::LastHandCandidate(pickupCandidate19c3);
// Direct pickup has its own two-sample admission below. The generic held-Use
// 20ms classification must not discard the first selected sample of that same
// squeeze; doors and contextual held actions keep the original classification.
const bool nearSqueezeD40=held114.ray&&pickupCandidate19c3.object&&pickupCandidate19c3.owner==held114.owner&&
    std::strcmp(pickupCandidate19c3.cls,"PickableItem")==0&&std::isfinite(pickupCandidate19c3.distance)&&
    pickupCandidate19c3.distance>=0&&pickupCandidate19c3.distance<=pickup_use19c3::kDirectReachD40&&
    pickupCandidate19c3.tick>=heldUse114.pressed&&door_use46::fresh(pickupCandidate19c3.tick,consumeNow54);
const bool heldEligible19c3=(comparison114||f5Mode==0)&&(held114.hold||nearSqueezeD40)&&
    (!hands59c7f::active()||(po.ray_owner>=0&&(grabMask59c7f&(1u<<po.ray_owner))));
// D40: publish each hand independently. Existing physical owners keep their
// exact recipient outside the selection ray. The ray still nominates only one
// NEW prop at a time; continuation is no longer truncated at the first hand.
// Native inventory items keep Picking and can never enter this constraint lane.
// A captured physical squeeze stays reserved through feature/context loss and
// release. It cannot turn into a new native A/E press on a mid-grip toggle.
bool looseGripOwnerD40=pickupUse19c3.loosePressD40&&pickupUse19c3.loosePressD40==heldUse114.pressed;
{
    tracking59c6::Stamp sourceD40{};
    const bool freshInputD40=xr_manager_&&xr_manager_->CopyInputSampleStamp59c6(sourceD40)&&
        door_use46::fresh(sourceD40.tick,consumeNow54);
    for(int side60=0;side60<2;++side60){
        kcd2vr::physics::grab_input60::GripEvidence ev{};ev.side=side60;ev.tick=consumeNow54;
        kcd2vr::physics::grab_producer60::Status status60{};
        const bool continuingD40=kcd2vr::physics::grab_producer60::ReadStatus(side60,status60)&&
            status60.owner.side==side60&&status60.owner.press&&status60.owner.entity&&status60.tick&&
            consumeNow54>=status60.tick&&consumeNow54-status60.tick<=80&&
            (status60.state==kcd2vr::physics::grab_producer60::State::Preparing||
             status60.state==kcd2vr::physics::grab_producer60::State::WaitingPresent||
             status60.state==kcd2vr::physics::grab_producer60::State::Held);
        const bool selectedD40=side60==po.ray_owner&&pickupCandidate19c3.object&&
            pickupCandidate19c3.owner==side60&&pickupCandidate19c3.entity&&
            kcd2vr::physics::grab_input60::nearSelectionD40(pickupCandidate19c3.cls,pickupCandidate19c3.distance,
                pickupCandidate19c3.tick,consumeNow54);
        if(continuingD40||selectedD40){
            ev.entity=continuingD40?status60.owner.entity:pickupCandidate19c3.entity;
            ev.press=continuingD40?status60.owner.press:heldUse114.pressed;ev.object=true;
            ev.eligible=kcd2vr::collab_h21::enabled.load(std::memory_order_acquire)&&freshInputD40&&pi.enabled&&!pi.menu&&!alchemyPointer59c2&&pi.hand[side60].tracked&&
                pi.hand[side60].empty&&!(reserved114&(1u<<side60))&&!(gripMask116&(side60?1u:2u))&&
                !(doorPlan46.owns&&doorPlan46.side==side60)&&(comparison114||f5Mode==0);
            if(xr_manager_){const auto& controller60=side60?xr_manager_->LeftController():xr_manager_->RightController();
                ev.held=ev.eligible&&std::isfinite(controller60.grip)&&controller60.grip>=.30f&&
                    (continuingD40||heldEligible19c3)&&ev.press;
            }
        }
        if(ev.held){
            float palm[3]{},quat[4]{0,0,0,1};std::uint64_t poseEpoch=0,trackingEpoch=0,ptick=0;std::uintptr_t player=0;
            if(frik_pose_probe::ReadGrabHandWorld60(side60,palm,quat,poseEpoch,trackingEpoch,player,ptick)&&
               trackingEpoch==sourceD40.epoch&&door_use46::fresh(ptick,consumeNow54)){
                ev.poseEpoch=poseEpoch;ev.trackingEpoch=trackingEpoch;ev.player=player;ev.tick=ptick;
                for(int j=0;j<3;++j)ev.palm[j]=palm[j];for(int j=0;j<4;++j)ev.quat[j]=quat[j];
            }else {ev.held=false;ev.eligible=false;}
        }
        // A class name alone grants no physical ownership. OFF, missing poses
        // and unavailable sources retain the ordinary native contextual route.
        const bool queuedD48=kcd2vr::physics::grab_input60::PublishGripEvidence(ev);
        if(side60==po.ray_owner&&ev.held&&ev.eligible&&queuedD48)looseGripOwnerD40=true;
    }
}
const auto pickupPlan19c3=pickup_use19c3::update(pickupUse19c3,pickupPulseEnabled19c3,
    po.ray_owner,heldUse114.pressed,heldEligible19c3,
    {pickupCandidate19c3.tick,pickupCandidate19c3.entity,pickupCandidate19c3.owner,pickupCandidate19c3.object,
     pickupCandidate19c3.distance,pickupCandidate19c3.cls},consumeNow54);
if(looseGripOwnerD40&&heldUse114.pressed)pickupUse19c3.loosePressD40=heldUse114.pressed;
action_time_owner59c11::PreparePhysicalPickup63(pickupPlan19c3.pickable63&&!pickupUse19c3.cancelled&&!pickupUse19c3.sent&&!pickupPlan19c3.pulse,
    po.ray_owner,consumeNow54);
// Suppress the ordinary F5=0 delayed tap as well: a released Pickable
// squeeze cannot become native Use after the Grip has already opened.
const bool pickupClick63=pickupCandidate19c3.object&&pickupCandidate19c3.owner==po.ray_owner&&
    std::strcmp(pickupCandidate19c3.cls,"PickableItem")==0;
if(looseGripOwnerD40||pickupPlan19c3.suppress63||((f5Mode==0||f5Mode==2)&&pickupClick63&&!heldEligible19c3))po.fire_use=false;
touch_bindings_.ValidatePhysicalInteractionPressD48(heldUse114.pressed,heldEligible19c3);
if(rayPublished116){
    f5OwnedRay=true;
    heldNow114=touch_bindings_.SetPhysicalInteractionHeld114(!doorPlan46.owns&&!looseGripOwnerD40&&(comparison114||f5Mode==0)&&pickupPlan19c3.nativeHeld);
    if(doorPlan46.pulse||(!doorPlan46.owns&&(po.fire_use||pickupPlan19c3.pulse))){
        const bool sent=touch_bindings_.RequestPhysicalInteractionUse(heldUse114.pressed);
        useSent116=sent;
        pickup_use19c3::complete(pickupUse19c3,pickupPlan19c3,sent);
        if(pickupPlan19c3.pulse&&pickupPlan19c3.pickable63&&sent)
            action_time_owner59c11::NotePhysicalPickupUse63(po.ray_owner,consumeNow54,pickupPlan19c3.target);
        if(pickupPlan19c3.pulse&&pickupPlan19c3.pickable63&&sent) // R120: the frame tick runs the item Use if the game does not
            kcd2vr::pickup_use_fallback_r120::note(pickupPlan19c3.target,po.ray_owner,consumeNow54);
        doorUse46.complete(doorPlan46,sent);
        if(doorPlan46.pulse&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,consumeNow54)){char m46[240]{};std::snprintf(m46,sizeof(m46),
            "C46 DOOR_USE target=%u hand=%d queued=%d physicalGrip=HELD nativeUse=SHORT_PULSE selection=NATIVE_OFFERED motion=CONTROLLER_LOCAL",
            doorPlan46.target,doorPlan46.side,sent?1:0);diagnostic_trace::Write(m46);}
        // A sent qualified pickup opens only its hand's visual window. Another
        // contextual Use on that hand retires the old request (doors stay native).
        if(sent)frik_pose_probe::NoteVrPickupUse43(doorPlan46.pulse?doorPlan46.side:po.ray_owner,pickupPlan19c3.target,
            pickupPlan19c3.pulse&&pickupPlan19c3.pickable63,consumeNow54);
        // R90 (R89 pickup): this squeeze belongs to the native item pickup, the scenery grab steps back on that hand.
        // (no prep here: static item preparation stays with R2's TryPreparePickupR24.)
        if(sent&&pickupPlan19c3.pulse&&pickupPlan19c3.pickable63&&pickupPlan19c3.target&&po.ray_owner>=0&&po.ray_owner<2&&
           static_grab_r89::enabled.load(std::memory_order_acquire))
            static_grab_r89::pickupUseTick[po.ray_owner].store(consumeNow54,std::memory_order_release);
        if(pickupPlan19c3.pulse&&diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,consumeNow54)){char m[240]{};std::snprintf(m,sizeof(m),
            "C19C3 F5_PICKUP_USE target=%u hand=%d queued=%d physicalGrip=HELD nativeUse=SHORT_PULSE authority=NATIVE_INTERACTOR",
            pickupPlan19c3.target,po.ray_owner,sent?1:0);diagnostic_trace::Write(m);}
        // V66.178R7: re-wired (dead since V66.123R1): the game's own finger pose on the
        // using hand for 2.5 s after a hand-aimed Use (doors, pickups).
        // V66.178R10: no finger ownership on the sent pulse any more (it reset the empty hand on
        // every Grip squeeze); the actor interactive-action hook grants it when KCD2 starts the action.
        // Input pulse is not native action acceptance: no speculative hand ownership.
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,consumeNow54)){
            char m[384]{};std::snprintf(m,sizeof(m),"C10 HAND_USE hand=%s queuedUsePulse=%d transport=%s source=%s semantics=KCD2_INTERACTOR_OWNS_TARGET_ACTION",po.ray_owner?"LEFT":"RIGHT",sent?1:0,native_xi_full?"XI_A":"KEY_E",f5Mode==2?"F5_DIAGNOSTIC":"GAMEPLAY");diagnostic_trace::Write(m);
        }
    }
} else {
    touch_bindings_.SetPhysicalInteractionHeld114(false);
    if(f5OwnedRay){interaction_alignment::SetHandRayWorld(false,0,0,0,0,1,0);f5OwnedRay=false;}
    frik_pose_probe::SetInteractionHandContext158(-1,false,0,0,0,0);
}
// C10R13: arm from the transport that actually ran. Normal calibrated
// modes use held XI-A/E, not po.fire_use; do not require the tap branch.
// Publication is an input request, never native action acceptance.
action_time_owner59c11::NoteUseInput(useSent116&&!pickupPlan19c3.pickable63,!pickupPlan19c3.pickable63&&(heldNow114||
    pickup_use19c3::physicalHeld(pickupUse19c3,po.ray_owner,heldUse114.pressed,heldEligible19c3)),
    po.ray_owner,consumeNow54);
// Only the transport that actually succeeded can witness a physical request.
// It grants no door target/action; the native accepted-door callback must join
// the same original squeeze to a fresh exact target before worker adoption.
if(useSent116||(heldNow114&&!doorUse46.nativeHeld62)){
    const auto target64=doorPlan46.owns?doorPlan46.target:
        pickupPlan19c3.pulse?pickupPlan19c3.target:
        pickupCandidate19c3.object&&pickupCandidate19c3.owner==held114.owner&&
        door_use46::fresh(pickupCandidate19c3.tick,consumeNow54)?pickupCandidate19c3.entity:0;
    palm59c9::ordinaryTransportLock64.claim(palms59c9,held114.owner,target64,heldUse114.pressed,
        doorInput46.sourceGeneration,doorInput46.trackingEpoch,door_use46::settingsEpoch.load(std::memory_order_acquire),
        doorInput46.experimentEpoch64,f5Mode,consumeNow54);
    if(palm59c9::ordinaryTransportLock64.transported&&palm59c9::ordinaryTransportLock64.target){
        palms59c9.lockedEntity=palm59c9::ordinaryTransportLock64.target;
        interaction_alignment::SetPalmRays59c9(palms59c9);
    }
}
doorUse46.transport62(doorInput46,useSent116,heldNow114);
observeCycleH4(true);
