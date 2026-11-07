// Generated exact hook entry and capture-off path; do not edit.
    const auto captureNow175=std::uint64_t(GetTickCount64());
    const bool interactionCapture175=diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,captureNow175);
    // Snapshot source while native owns the arguments. There is no worker polling.
    void* actorEntity=nullptr;bool henry=actor&&Read(static_cast<unsigned char*>(actor)+0x38,actorEntity)&&actorEntity&&actorEntity==g_last_player_entity.load(std::memory_order_acquire);
    // C38: bind the source before the synchronous native call. Its nested work
    // may publish another hand's nearer candidate before returning acceptance.
    InteractionHandContext158 requestHand38{};
    DoorRequestWitness38 requestDoor38{};
    requestDoor38.epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    requestDoor38.player=reinterpret_cast<std::uintptr_t>(actorEntity);
    if(henry&&g_interaction_hand_context158.read(requestHand38)&&requestHand38.valid&&
       captureNow175>=requestHand38.tick&&captureNow175-requestHand38.tick<=80)
        (void)interaction_alignment::ExactDoorCandidate38(targetId,requestHand38.side,captureNow175,requestDoor38.candidate);
    if(henry&&requestHand38.valid&&captureNow175>=requestHand38.tick&&captureNow175-requestHand38.tick<=80)
        CaptureDoorRequest46(requestDoor38,targetId,requestHand38.side,captureNow175);
    // V66.178R10/R13: KCD2's own finger pose owns the using hand only once the game ACCEPTED Henry's
    // interactive action (non-zero return = the action object; the R7 log shows 0x18A57CE7A00 for doors),
    // for the hand that published the hover/use ray just before. A rejected request grants nothing.
    // (V66.178R7-R9 granted it on every sent Use pulse, i.e. on every Grip squeeze of an empty hand.)
    auto grantHandUse178r13=[&](std::uint64_t returned){
        if(!henry||!returned)return;const auto& hc=requestHand38;const auto hcNow=std::uint64_t(GetTickCount64());
        if(hc.valid&&hcNow>=hc.tick&&hcNow-hc.tick<=80){
            NotifyNativeHandUse(hc.side,1800);
            ArmDoor19c4(targetId,returned,name,hc.side,hcNow,&requestDoor38);
            if(hc.side>=0&&hc.side<2)g_native_use_target59c10[hc.side].store(targetId,std::memory_order_release);
            if(interactionCapture175&&kcd2vr::workstation178r20c1::ownsWrist(hcNow)){
                char label[96]{};if(name)for(unsigned i=0;i<sizeof(label)-1;++i){if(!Read(name+i,label[i])||!label[i])break;}
                char line[304]{};std::snprintf(line,sizeof(line),
                    "C10 F5_HAND_HOLD_ACTION side=%d object=%u action=%s nativeReturn=%p fingers=NATIVE wrist=CONTROLLER elementDrive=NATIVE_AUTHORED",
                    hc.side,targetId,label,reinterpret_cast<void*>(returned));diagnostic_trace::Write(line);
            }
        }else if(interactionCapture175){
            // Accepted native door, but no fresh hand context: receipt only.
            ArmDoor19c4(targetId,returned,name,-1,hcNow);
        }};
    if(!interactionCapture175&&!ReloadDiagnostics175()){
        const auto fast=DoorNativeCall66(g_interaction_original6689,actor,name,targetId,force,speed,requestDoor38,requestHand38.side,captureNow175);
        grantHandUse178r13(fast);return fast;}
