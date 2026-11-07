// Generated complete production capture helper.
static void CaptureDoorRequest46(DoorRequestWitness38& witness,std::uint32_t target,int side,std::uint64_t now) noexcept {
    // Capture the exact producing family BEFORE the native accepted action
    // executes. A new Mod frame/global experiment after that call cannot
    // relabel an old physical Use as a fresh source.
    kcd2vr::door_hold65::Frame entry65{};kcd2vr::door_use46::Input input65{};
    if(side>=0&&side<2&&kcd2vr::door_hold65::frame.read(entry65)&&
       kcd2vr::door_use46::currentInput62.read(input65)&&kcd2vr::door_use46::inputCurrent62(input65,now)){
        const auto& h=entry65.hand[side];
        if(kcd2vr::door_hold65::current(h,now)&&h.side==side&&h.player==witness.player&&h.poseEpoch==witness.epoch&&
           h.grip>=.30f&&h.press&&h.press==input65.press&&h.side==input65.side&&h.player==input65.player&&
           h.poseEpoch==input65.poseEpoch&&h.trackingEpoch==input65.trackingEpoch&&h.sourceGeneration==input65.sourceGeneration&&
           h.experimentEpoch==input65.experimentEpoch64&&h.experimentEpoch==kcd2vr::workstation178r20c1::experimentEpoch36.load()&&
           h.settingsEpoch==kcd2vr::door_use46::settingsEpoch.load()&&interaction_alignment::DoorAcquisitionCurrent65(h.sourceGeneration)){
            witness.hand65=h;witness.handSource65=true;
        }
    }
    if(kcd2vr::diagnostics::door_receipt63::active(now)){
        witness.receipt63=true;
        (void)kcd2vr::door_use46::currentInput62.read(witness.receiptInput63);
        (void)kcd2vr::door_use46::physicalUse62.read(witness.receiptPhysical63);
        (void)interaction_alignment::LastHandCandidate(witness.receiptCandidate63);
        witness.receiptHeld63=kcd2vr::door_use46::inputCurrent62(witness.receiptInput63,now);
        witness.receiptPhysicalCurrent63=kcd2vr::door_use46::pulseCurrent62(witness.receiptPhysical63,witness.receiptInput63,now);
    }
    kcd2vr::door_use46::Intent i{};
    if(kcd2vr::door_use46::read(i,now)&&i.side==side&&i.player==witness.player&&i.poseEpoch==witness.epoch){
        witness.inputPresent46=true;
        if(i.target==target&&kcd2vr::door_use46::fresh(i.selectedTick,now,250)&&
           interaction_alignment::DoorAcquisitionCurrent65(i.sourceGeneration))witness.input46=i;
    }
    if(!witness.input46.token){
        const auto& c=witness.candidate;
        kcd2vr::door_use46::Selection exact{c.object&&!std::strcmp(c.cls,"AnimDoor"),c.entity,c.owner,c.tick,c.sourceTick,
            c.sourceEpoch,c.sourceGeneration,c.sourcePlayer,{c.x,c.y,c.z},c.distance};
        kcd2vr::door_hold65::Frame hands65{};kcd2vr::door_surface65::Witness surface65{};
        if(!kcd2vr::door_hold65::frame.read(hands65)||side<0||side>1||
           !kcd2vr::door_surface65::read(target,hands65.hand[side],now,surface65))return;
        exact.point=surface65.point;exact.distance=surface65.distance;
        if(c.entity==target&&c.owner==side&&c.sourcePlayer==witness.player&&c.sourceEpoch==witness.epoch&&
           (interaction_alignment::DoorAcquisitionCurrent65(c.sourceGeneration)||interaction_alignment::DoorSourceCurrent38(c.sourceGeneration))&&
           interaction_alignment::DoorAcquisitionCurrent65(hands65.hand[side].sourceGeneration)){
            // The retained native projection is eligibility only. Normalize its
            // input identity from the independently proven current SAME door
            // surface; a withdrawn generic ray is not the Door grasp source.
            exact.sourceGeneration=surface65.source.sourceGeneration;
            exact.poseEpoch=surface65.source.poseEpoch;exact.poseTick=surface65.source.poseTick;exact.player=surface65.source.player;
            (void)kcd2vr::door_use46::captureAccepted62(exact,now,witness.acceptedUse62);
        }
    }
}
