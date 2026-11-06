    // R59.16C10R17: the NPC already parried / blocked this stroke natively (FOLLOWUP_C at the phase-8 edge, or the collision path):
    // the VR contact is the clash, not a second hit. Claimed once so no later contact of the same stroke lands either.
    if(!vrDirect596&&p.lease47c2.serial&&g_sync59c10r9.load(std::memory_order_acquire)){
        if(ConsumeNativeAnswer19c4r2(p,henryActor,target,now)){hybridReservation54.nativeCallStarted();return;}
    }
