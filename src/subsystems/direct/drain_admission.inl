    if(kcd2vr::motion59c4::selected.load(std::memory_order_acquire)&&directOkR159){ // R159: no VR-state-driver direct hit in a tutorial
        namespace mo=kcd2vr::motion59c4;
        const mo::Key key{p.gesture59c3,p.key,p.generation,p.epoch,p.request,p.recovery55c1,
            reinterpret_cast<std::uintptr_t>(Rd<const void*>(henryActor,0x1F8,nullptr))};
        mo::Attempt attempt{};
        if(p.weaponItem){if(Allow(4,2)){char m[180]{};std::snprintf(m,sizeof(m),"R59.16C7A VR_HIT_SKIPPED serial=%u reason=CONTACT_ON_WEAPON_OR_SHIELD_ITEM (no parry/block route yet)",p.serial);diagnostic_trace::Write(m);}return;} // Item-to-owner defense contract is not established.
        // Do not consume an attempt merely because a queued integer ID exists.
        // Resolve and revalidate the exact struck actor before the once-only claim.
        const void* target=nullptr;const void *entity=nullptr,*state=nullptr;
        if(!AwaitDirectTarget19c3(p,pendingLease,henryActor,hs,now,target,entity,state))return;
        directTarget19c3=target;
        // R59.16C7A: every check that can refuse runs BEFORE the once-per-swing claim, so a refused
        // contact never uses up the swing (C7 claimed first and then refused).
        // R59.8 preflight: never apply damage when the reaction/notification continuation is not available.
        if(!(g_outer178r33.load()&&!g_outer_fault178r33.load()&&OuterVerified178r33()&&
             (gameRules5910c1=HitEventReceiver5910c1())!=nullptr)){
            char m[160]{};std::snprintf(m,sizeof(m),"V66.178R59.6 VR_HIT_SKIPPED serial=%u reason=HIT_CONTINUATION_UNAVAILABLE (no apply without reaction and game-rule notification transport)",p.serial);diagnostic_trace::Write(m);return;}
        // R59.13b: our own per-target spacing + hostility gate (the native 1.5 s repeat rule is not
        // reached on this route). A neutral NPC is only struck when VrHitHostileOnly=0.
        {std::uint64_t age=0;
         if(!vrstate596::TargetReady(p.entityId,now,age)){
            if(Allow(4,4)){char m[220]{};std::snprintf(m,sizeof(m),"V66.178R59.13 VR_HIT_TOO_SOON serial=%u target=%u ageMs=%llu windowMs=%u (our spacing, not the native 1.5 s repeat rule)",
                p.serial,p.entityId,(unsigned long long)age,vrstate596::g_target_repeat_ms.load());diagnostic_trace::Write(m);}return;}
         if(vrstate596::g_hostile_only.load()){
            const int inCombat=state?B(state,kInCombat):0xFF;const void* opponent=Rd<const void*>(hs,0xCF8,nullptr);
            const float minEnergy=vrstate596::g_neutral_min_energy.load();
            if(inCombat!=1&&opponent!=target){
                if(p.energy<minEnergy){
                char m[260]{};std::snprintf(m,sizeof(m),"V66.178R59.13 VR_HIT_SKIPPED serial=%u target=%u reason=NOT_HOSTILE_LIGHT_CONTACT energy=%.2f min=%.2f inCombat=%d (VrHitHostileOnly=1 accidental-contact protection)",
                    p.serial,p.entityId,p.energy,minEnergy,inCombat);diagnostic_trace::Write(m);return;}
                char m[230]{};std::snprintf(m,sizeof(m),"R59.16C4 VR_HIT_NEUTRAL_FIRST_STRIKE serial=%u target=%u energy=%.2f inCombat=%d continues=NATIVE_CHECKS (not proof of application)",
                    p.serial,p.entityId,p.energy,inCombat);diagnostic_trace::Write(m);}}}
        auto result=mo::attempts.claim(key,p.tick,now,p.entityId,attempt);
        // R59.12C1: checked early contact can commit an existing preparation.
        // Cancellation/expiry and unknown attempts cannot be revived.
        if(vrstate596::g_consequence.load()&&!vrstate596::g_fault.load()&&
           (result==mo::Claim::Wait)){
            attempt={};result=mo::attempts.contact(key,p.tick,now,p.entityId,p.kind==1?1:0,attempt);
            if(result==mo::Claim::Ready){char m[200]{};std::snprintf(m,sizeof(m),"V66.178R59.6 VR_CONTACT_COMMIT serial=%u gesture=%llu target=%u kind=%s (contact-led attack, consumed once)",p.serial,(unsigned long long)key.gesture,p.entityId,p.kind==1?"THRUST":"CUT");diagnostic_trace::Write(m);}}
        if(result==mo::Claim::Wait){pendingLease.wait(p);return;}
        if(result==mo::Claim::Ready){counterOrigin36=attempt.origin;char line[480]{};std::snprintf(line,sizeof(line),
            "R59C4 MOTION_OWNED attempt=%llu contact=%u target=%u ageMs=%llu origin=%d technique=%d timing=VR_CONTACT nativeRelease=NOT_REQUIRED nativePhase=NOT_REQUIRED nativeOrdinaryStarts=%llu apply=PENDING_NATIVE_ADJUDICATION missingNativeContracts=0x%X",
            (unsigned long long)key.gesture,p.serial,p.entityId,(unsigned long long)(now-p.tick),attempt.origin,attempt.technique,
            (unsigned long long)mo::nativeStarts.load(),mo::missingNativeContracts);diagnostic_trace::Write(line);}
        if(result!=mo::Claim::Ready||!vrstate596::g_consequence.load()||vrstate596::g_fault.load()){ // preview only: no consequence
            // R59.16C7A: say why (C7 returned silently). Duplicate = same swing already hit: expected, not logged.
            if(result!=mo::Claim::Duplicate&&Allow(4,4)){static const char* kClaim[]{"WAIT","READY","DUPLICATE","IDENTITY(unknown attempt or contact before prepare)","EXPIRED","CANCELLED","INVALID_CONTACT"};
                char m[260]{};std::snprintf(m,sizeof(m),"R59.16C7A VR_HIT_SKIPPED serial=%u gesture=%llu target=%u claim=%s consequence=%d fault=%d",p.serial,(unsigned long long)key.gesture,p.entityId,
                    kClaim[unsigned(result)<7?unsigned(result):0],vrstate596::g_consequence.load()?1:0,vrstate596::g_fault.load()?1:0);diagnostic_trace::Write(m);}
            if(result==mo::Claim::Ready)mo::attempts.release(key);
            return;}
        claimReservationC2.hold(mo::attempts,key); // every pre-submit exit returns this undelivered reservation
        // R59.6: the blade is at the enemy NOW. Publish the strike phase through the native notifying setter (what
        // D4D34C does for an ordinary action, minus the action/animation/sweep) and deliver the hit directly.
        // R59.10C1: a contact cannot steal a protected native phase.
        if(!contactContext5912.begin(hs,attempt)){
            char m[240]{};std::snprintf(m,sizeof(m),"R59.12C1 VR_HIT_SKIPPED serial=%u gesture=%llu reason=CONTEXT_NOT_OWNED phase=%d applied=0 claim=RELEASE_ON_EXIT",
                p.serial,(unsigned long long)attempt.key.gesture,I(hs,kActionPhaseProp39));diagnostic_trace::Write(m);return;
        }
        vrDirect596=true;
    }
