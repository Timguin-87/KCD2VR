        // Resolve every existing wrist owner once, after native fixture position
        // guidance and before contact/arm admission. DesiredHand6689 reads only
        // the calibrated hand basis and tracking frame, never mutable arm FK.
        const Q4 wristFrame52=directHandsR135?directHandFrameR142:frame;
        Q4 handDesired=(tx.physicalActive&&tx.physicalOutput.owned[side])?QP(((tx.physicalOutput.displayLatched[side]||tx.physicalOutput.displayRejoinR13[side])?tx.physicalOutput.displayHands[side]:tx.physicalOutput.hands[side]).q):DesiredHand6689(s,side,wristFrame52,visQH50[side]);
        handDesired=QP(kcd2vr::physics::hand_presence_h24::wristRotation(presenceOwnedH24[side],{presenceWristH24[side].q,presenceWristH24[side].p},PQ(handDesired)));
        if(propHeld5916c1[side])handDesired=QP(propWrist5916c1[side].q);
        if(doorPose66[side])handDesired=QP(doorWrist66[side].rotation);
        if(!directHandsR135&&stagedHandoverD47[side]>.001f){const auto q=pc::slerp(PQ(handDesired),PQ(QFromQuatT(s.original[h])),stagedHandoverD47[side]);if(q)handDesired=QP(*q);}
        // Smithing's exact current tool/socket owns its dedicated collision
        // setting independently of the free-hand F10/performance switches.
        // Position, rotation, tool placement and staged history now agree on
        // this one allowed pose. The adapter retires missing/changed owners.
        if(nativeHeatingHandD56[side]){
            tx.smithContactR38.nativeFloorD56={};tx.smithContactR38.world[side]={};tx.smithContactR38.anvil[side]={};
        }
        const bool nativeSmithFloorD56=!nativeFingerCapture&&!freeContact164R1[side]&&!nativeHeatingHandD56[side]&&
            SmithNativeFloorD56(character,tx.epoch,side,s,ts.sequence,ts.source_epoch,ts.tick,
                goals[side],handDesired,tx.smithContactR38,
                ts.head_valid&&ts.left_valid&&!ts.head_held&&!ts.left_held?ts.left_trigger:0.f,&handDesired);
        if(!nativeFingerCapture&&!freeContact164R1[side]&&!nativeHeatingHandD56[side]&&!nativeSmithFloorD56)
            SmithAnvilContactA2(character,tx.epoch,side,s,frame,visQH50[side],goals[side],&tx.smithContactR38,&handDesired);
        // D53: a current held alchemy prop uses native local bounds and the
        // last committed tool pose; the native recipe/action clocks are separate.
        if(!nativeFingerCapture&&!freeContact164R1[side]&&!nativeHeatingHandD56[side])
            AlchemyToolContactD53(character,tx.epoch,side,ts.sequence,ts.source_epoch,goals[side],handDesired,tx.alchemyContactD80[side]);
