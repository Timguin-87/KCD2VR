// Actual Mod ordinary hand/transaction lock; never used for Alchemy native-selected locking.
if(alchemyPointer59c2){palm59c9::ordinaryTransportLock64={};}
else {
    tracking59c6::Stamp palmStamp64{};
    const bool sampledPalm64=point123.owner>=0&&xr_manager_&&xr_manager_->CopyInputSampleStamp59c6(palmStamp64);
    const auto palmNow64=std::uint64_t(GetTickCount64()); // acquisition time, not the pre-RunFrame loop clock
    const bool freshPalm64=sampledPalm64&&door_use46::fresh(palmStamp64.tick,palmNow64);
    auto& transaction64=palm59c9::ordinaryTransportLock64;
    palms59c9.lockedEntity=transaction64.observe(palms59c9,point123.owner,heldUse114.pressed,
        door_use46::inputMode(f5Mode)?interaction_alignment::DoorAcquisitionGeneration65():interaction_alignment::DoorSourceGeneration38(),freshPalm64?palmStamp64.epoch:0,
        door_use46::settingsEpoch.load(std::memory_order_acquire),
        workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire),f5Mode,palmNow64);
    door_use46::Intent accepted64{};
    if(door_use46::inputMode(f5Mode)&&transaction64.transported&&door_use46::read(accepted64,palmNow64)&&accepted64.physicalUseSerial62&&
       accepted64.side==transaction64.side&&accepted64.press==transaction64.press&&
       accepted64.player==transaction64.player&&accepted64.poseEpoch==transaction64.epoch&&
       accepted64.sourceGeneration==transaction64.generation&&accepted64.trackingEpoch==transaction64.tracking&&
       accepted64.settingsEpoch==transaction64.settings){
        transaction64.adoptAccepted(accepted64.target,accepted64.press,accepted64.physicalUseSerial62);
        palms59c9.lockedEntity=transaction64.target;
    }
    door_hold65::Operation operation65{};
    if(door_use46::inputMode(f5Mode)&&point123.owner>=0&&door_hold65::operation.read(operation65)&&door_hold65::current(operation65,palmNow64)&&
       !operation65.paused&&operation65.source.press==heldUse114.pressed&&operation65.source.side==point123.owner&&
       door_hold65::sameSource(operation65.source,doorFrame65.hand[point123.owner]))
        palms59c9.lockedEntity=operation65.request.id; // already accepted native operation, no new Use
}
