    // V66.178R35: the live attack action route first.
    bool syncDirect59c10r9=false; // R59.16C10R9: set only by the SYNCHRONIZED branches below
    const bool syncAged59c12=g_sync59c10r9.load(std::memory_order_acquire)&&p.tick&&now>=p.tick&&now-p.tick>=g_sync_grace_ms59c10r9.load(std::memory_order_relaxed);
    // R159 (directOkR159, tutorial_native_r159.h): in a tutorial every direct branch below is off - the overdue routes, the VR
    // intent direct hit and the retired direct fallback; the contact waits for the live strike window like VrSyncOverdueDirect=0.
    const bool syncOverdue59c10r9=syncAged59c12&&p.release51.serial&&directOkR159;
    // R59.16C10R12: the overdue direct route no longer needs an admitted native release - a withheld stroke, an older action still
    // alive or a non-ordinary native phase all left the contact waiting until it expired (11 CONTACT_EXPIRED in the 19:43 log).
    const bool syncOverdueAny59c12=syncAged59c12&&g_sync_overdue_any59c12.load(std::memory_order_relaxed)&&directOkR159;
    const char* syncReason59c12="STRIKE_WINDOW_NOT_OPEN";
    if(g_live178r35.load(std::memory_order_relaxed)&&!vrDirect596){
        // Accept only the verified unpatched entry or this owner's compared
        // patch/trampoline; never cache a generic patched-entry success.
        const int verified=FistReceiverCallableH12()?1:0;
        const void* action=(verified==1&&!g_live_fault178r35.load(std::memory_order_relaxed))?LiveAttackAction178r35(henryActor):nullptr;
        if(action&&!g_apply178r30.load(std::memory_order_relaxed)){ // V66.178R37 (F05): the live handler cannot evaluate-only
            if(Allow(4,4)){char m[160]{};std::snprintf(m,sizeof(m),"V66.178R37 GEOM_SKIP serial=%u reason=APPLY_OFF_LIVE_ROUTE (VrGeometryApply=false: no live-action delivery)",p.serial);diagnostic_trace::Write(m);}return;}
        if(p.action&&!action){
            g_stats[5].fetch_add(1,std::memory_order_relaxed);
            if(Allow(4,6))diagnostic_trace::Write("V66.178R40C1 GEOM_SKIP reason=BOUND_ACTION_ENDED (never rebind a held contact after disappearance)");
            return;
        }
        // R59.16C10R16 VR INTENT: the game ADMITTED this stroke's attack (its live action owns this release - Henry is not staggered,
        // the NPC sees the attack): the contact is the hit now, energy-scaled, through the direct native hit. Not admitted yet: the
        // ordinary SYNCHRONIZED wait below (and its overdue direct route) still applies.
        // R59.16C10R17: if the contact catch-up has opened the strike window (phase 8, receiver armed) the hit goes through the LIVE
        // chain below (native block/parry/clash, combo flag; our strike power leased onto +0x1E0). Still in phase 4: wait up to
        // VrIntentLiveMs for the catch-up, then the direct hit (R16). VrIntentLiveRoute off = R16 (direct at once).
        if(directOkR159&&g_intent59c16.load(std::memory_order_acquire)&&g_sync59c10r9.load(std::memory_order_acquire)&&p.lease47c2.serial&&
           action&&(!p.action||p.action==action)&&frik_pose_probe::GeometryAdmittedRelease51(p.release51,action)){
            const int phase17=I(hs,kActionPhaseProp39);const void* a220=Rd<const void*>(henryActor,0x220,nullptr);const int f220=a220?int(Rd<std::uint8_t>(a220,0x28,0xFF)):-1;
            const bool live17=g_intent_live59c17.load(std::memory_order_relaxed)&&phase17==g_strike_phase178r39.load(std::memory_order_relaxed)&&f220==0;
            const bool wait17=g_intent_live59c17.load(std::memory_order_relaxed)&&phase17==4&&p.tick&&now>=p.tick&&now-p.tick<g_intent_live_ms59c17.load(std::memory_order_relaxed);
            if(!live17){
                if(wait17){pendingLease.wait(p);return;}
                g_intent_hits59c16.fetch_add(1,std::memory_order_relaxed);g_intent_direct_hits59c17.fetch_add(1,std::memory_order_relaxed);
                if(!p.action)p.action=action; // carry this already-admitted action into the direct lineage bridge
                syncDirect59c10r9=true;syncReason59c12=phase17==4?"VR_INTENT_PHASE8_NOT_OPEN_IN_TIME":"VR_INTENT_ADMITTED_CONTACT_NOW";goto sync_direct59c10r9;}
            g_intent_live_hits59c17.fetch_add(1,std::memory_order_relaxed); // falls through to the live route
        }
        if(action&&p.action&&p.action!=action){ // V66.178R40: contact bound to action A cannot be delivered through action B
            g_stats[5].fetch_add(1,std::memory_order_relaxed);
            if(Allow(4,6)){char m[200]{};std::snprintf(m,sizeof(m),"V66.178R40 GEOM_SKIP serial=%u reason=ACTION_REPLACED bound=%p live=%p",p.serial,p.action,action);diagnostic_trace::Write(m);}return;}
        if(action&&!frik_pose_probe::GeometryAdmittedRelease51(p.release51,action)){
            // A previous native action cannot consume the new gesture's contact.
            // Wait for THIS release's admitted receiver, within the impact's age.
            if(syncOverdueAny59c12){syncDirect59c10r9=true;syncReason59c12="ACTION_NOT_ADMITTED_FOR_THIS_RELEASE";goto sync_direct59c10r9;} // R59.16C10R12
            pendingLease.wait(p);return;
        }
        if(action){
            if(!p.action)p.action=action; // bind on first sight
            // V66.178R39: C5235C's own preconditions, read the way it reads them: phase (state prop +0x30) == 8, [actor+0x220]+0x28 == 0.
            const int phase=I(hs,kActionPhaseProp39);const void* a220=Rd<const void*>(henryActor,0x220,nullptr);const int f220=a220?int(Rd<std::uint8_t>(a220,0x28,0xFF)):-1; // native phase only
            // Phase 16 is a spent/withdrawn ordinary strike, not wind-up.
            // Do not carry its old contact into another attack using this receiver.
            if(phase!=4&&phase!=8){
                if(syncOverdueAny59c12){syncDirect59c10r9=true;syncReason59c12="NONORDINARY_NATIVE_PHASE";goto sync_direct59c10r9;} // R59.16C10R12
                if(directOkR159&&g_sync59c10r9.load(std::memory_order_acquire)&&g_sync_overdue_any59c12.load(std::memory_order_relaxed)){pendingLease.wait(p);return;} // not yet overdue: wait for the grace (R159: never in a tutorial)
                ++g_stats[5];
                if(Allow(4,6))diagnostic_trace::Write("V66.178R47C2 GEOM_SKIP reason=STRIKE_ENDED_OR_NONORDINARY");
                return;
            }
            if(f220!=0){++g_stats[5];g_drop_flag220_56.fetch_add(1,std::memory_order_relaxed);
                // C56 review (Claude): a dropped (not waited) contact; it used to leave no line.
                if(Allow(4,4)){char m[240]{};std::snprintf(m,sizeof(m),"C56 GEOM_SKIP serial=%u reason=NATIVE_FLAG220_SET phase=%d flag220=%d action=%p gesture=%llu release=%llu ageMs=%llu dropped=1",
                    p.serial,phase,f220,action,(unsigned long long)p.gesture59c3,(unsigned long long)p.release51.serial,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
                return;}
            const char* transition48=nullptr;const int phaseBefore48=phase;const float remaining48=F(hs,0x420),strength48=F(hs,0x1E0);
            const int phase48=phase;
            if(phase48!=g_strike_phase178r39.load(std::memory_order_relaxed)&&syncOverdue59c10r9){syncDirect59c10r9=true;goto sync_direct59c10r9;}
            if(phase48!=g_strike_phase178r39.load(std::memory_order_relaxed)){
                pendingLease.wait(p); // original age and gesture remain unchanged
                static std::uint64_t lastWait=0;if(now-lastWait>=250&&Allow(4,4)){lastWait=now;char m[240]{};std::snprintf(m,sizeof(m),"V66.178R39 GEOM_LIVE_WAIT serial=%u action=%p phase=%d (need %d) flag220=%d heldMs=%llu (contact held for the native strike window)",p.serial,action,phase,g_strike_phase178r39.load(),f220,(unsigned long long)(now-p.queued));diagnostic_trace::Write(m);}
                return;}
            {std::lock_guard<std::mutex> lock(g_mutex);g_last_submit=now;} // F05: the submit cooldown covers the live path too
            // Claim before the native handler: rejection/reentrancy is not a
            // licence to replay the same physical strike against another target.
            if(!frik_pose_probe::GeometryAdmittedRelease51(p.release51,action)){
                if(Allow(4,4)){char m[220]{};std::snprintf(m,sizeof(m),"C56 GEOM_SKIP serial=%u reason=RELEASE_NOT_ADMITTED_AT_SUBMIT release=%llu gesture=%llu ageMs=%llu dropped=1",
                    p.serial,(unsigned long long)p.release51.serial,(unsigned long long)p.gesture59c3,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
                return;}
            if(!serialReservationC2.reserve(p.release51,now,g_sync59c10r9.load(std::memory_order_acquire))){g_drop_serial_claimed56.fetch_add(1,std::memory_order_relaxed);
                if(Allow(4,4)){char m[240]{};std::snprintf(m,sizeof(m),"C56 GEOM_SKIP serial=%u reason=%s release=%llu claimed=%llu gesture=%llu ageMs=%llu dropped=1",
                    p.serial,serialReservationC2.reason(),(unsigned long long)p.lease47c2.serial,(unsigned long long)kcd2vr::contact176::claimedSerial.load(std::memory_order_acquire),(unsigned long long)p.gesture59c3,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
                return;}
            if(!initializeDetails59c3())return;
            g_inject_result178r35={};
            const auto liveRecovery65h=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
            if(p.recovery55c1!=liveRecovery65h){RecoveryRetired65h(p,liveRecovery65h,GetTickCount64(),kcd2vr::recovery65h::Site::BeforeLive);g_drop_recovery56.fetch_add(1,std::memory_order_relaxed);
                if(Allow(4,4)){char m[200]{};std::snprintf(m,sizeof(m),"C56 GEOM_HIT serial=%u rejected=RECOVERY_EPOCH_CHANGED site=BEFORE_LIVE_HANDLER gesture=%llu release=%llu ageMs=%llu",
                    p.serial,(unsigned long long)p.gesture59c3,(unsigned long long)p.release51.serial,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
                return;}
            bool survived=false,serialEntered59=false;
            // R59.16C10R17 (the user's damage rule: native damage x the stroke's energy, 30..120 %): in F4 press 1/3 the live route
            // carried the native AttackStrength (+0x1E0, normally 1.0) - the VR energy never reached it. Lease our strike power onto
            // +0x1E0 for this one call through the native float setter; if the handler did not consume it, restore the native value.
            float leased17=-1.f,prior17=0.f;bool riposte17=false;std::uint64_t riposteToken17=0;
            if(g_sync59c10r9.load(std::memory_order_acquire)&&vrstate596::Verified()){
                prior17=F(hs,kAttackStrength);riposteToken17=riposte59c8::TokenForC2(now,target);riposte17=riposteToken17!=0; // consumed only if the hit lands (below)
                const auto pw17=power59c8::Compute(p,now,riposte17);
                const auto counterH11=counter40::Decide(p,henryActor,ts,p.metadata59c7g.originZone47,pw17.power,now);
                const float contactStrengthH11=counterH11.after;
                if(std::isfinite(prior17)&&prior17>0.f&&std::isfinite(contactStrengthH11)&&contactStrengthH11>.01f){
                    if(vrstate596::SetFloat(hs,kAttackStrength,contactStrengthH11)){leased17=contactStrengthH11;g_strength_leases59c17.fetch_add(1,std::memory_order_relaxed);}
                    else if(F(hs,kAttackStrength)!=prior17)(void)vrstate596::SetFloat(hs,kAttackStrength,prior17);}}
#include "../base/live_contact_executor.inl" // M2_OWNED_SOURCE
            if(leased17>0.f&&receipt.accepted&&receipt.fields&&receipt.applied){
                power59c8::RecordDelivered(p,completedNowC2);
                if(riposte17)(void)riposte59c8::TakeTokenC2(completedNowC2,target,riposteToken17);
            }
            if(Active()||kcd2vr::contact58c1::experiment.load(std::memory_order_acquire)){char m[832]{};
                std::snprintf(m,sizeof(m),"V66.178R58C1 CONTACT_RECEIPT serial=%llu gesture=%llu release=%llu impactAgeMs=%llu sample=%llu target=%u kind=%d bodyContact=%d nativeExecutedZone=%d rawCollisionPart=%d nativeHitPart=%d material=%d submitted=%d applied=%d fields=%d damage=%.3f stamina=%.3f blocked=%d perfect=%d combo=%d/%d death=%d stabbed=%d reactionDispatch=%d regionJoin=UNPROVEN ragdollImpulse=NONE",
                    (unsigned long long)receipt.serial,(unsigned long long)p.gesture59c3,(unsigned long long)p.release51.serial,(unsigned long long)(now-p.tick),(unsigned long long)receipt.sample,receipt.target,receipt.kind,receipt.bodyContact,receipt.origin,receipt.rawPart,receipt.nativePart,receipt.material,
                    receipt.accepted,receipt.applied,receipt.fields,receipt.damage,receipt.stamina,receipt.blocked,receipt.perfect,receipt.combo,receipt.comboEnd,receipt.death,receipt.stabbed,receipt.dispatch);
                diagnostic_trace::Write(m);}
            if(Allow(4,20)){char s[400]{};if(hs)StateText(hs,s,sizeof(s));char m[1500]{};
                char v48[220]{};if(transition48){std::snprintf(v48,sizeof(v48),"%s via %s (phase %d->%d, remaining420=%.3f strength1E0=%.3f)",transition48,via,phaseBefore48,phase48,double(remaining48),double(strength48));via=v48;} // R48
                std::snprintf(m,sizeof(m),"V66.178R35 GEOM_LIVE_HIT serial=%u entity=%u target=%u via=%s kind=%s energy=%.2f pos=(%.2f,%.2f,%.2f) part=%d surface=%d action=%p route=C5235C(live attack action: full native chain) phase=%d flag220=%d heldMs=%llu submit=%s returned=%d post=%d dmg=%.2f stam=%.2f blocked=%d perfectBlock=%d parryInPlace=%d death=%d processReturned=%d resultFields=%d masterStrike=%d reactionCreated=%d reactionKind=%d reactionDispatch=%d targetPhaseAfter=%d bodyMaterial=%d(%s) henry{%s}",
                    p.serial,p.entityId,te?EntityId(te):0u,via,p.kind==1?"THRUST":"CUT",p.energy,p.pos[0],p.pos[1],p.pos[2],p.part,p.surface,action,phase,f220,(unsigned long long)(now-p.queued),r.seen?"REACHED_D568B0":"NOT_REACHED(rejected before the builder: target map/eligibility/phase)",
                    r.ok?1:0,r.post?1:0,r.dmg,r.stam,r.blocked,r.perfect,r.parry,r.death,r.process,r.fields,r.master,r.reactionCreated,r.reactionKind,r.dispatch,I(ts,kActionPhaseProp39),p.bodyMaterial,decap59c7e::MaterialName(p.bodyMaterial),s);diagnostic_trace::Write(m);}
            return;
        }
        // V66.178R37 (R36 review F04): with the live route selected, a contact without a live action is PASSIVE (visual/haptic
        // only) - never a direct-damage fallback, whether the latch expired or the live route faulted. INI VrGeometryDirectFallback
        // re-enables the R30-R34 direct route for comparison.
        // V66.178R39: no action yet - hold the contact while the engagement latch says a native swing was requested (the
        // stroke commits at/after the contact; the action appears a few frames later).
        if(!action&&verified==1&&!g_live_fault178r35.load(std::memory_order_relaxed)&&(p.release51.serial||g_sync_overdue_any59c12.load(std::memory_order_relaxed))){
            // R59.16C10R17 (review): in F4 press 1 the strike receiver (the live action) exists only from phase 8 on; before it, an
            // admitted release waits VrIntentLiveWaitMs (not the 70 ms SYNC grace) for the catch-up to open the strike window.
            if(directOkR159&&g_intent59c16.load(std::memory_order_acquire)&&g_intent_live59c17.load(std::memory_order_relaxed)&&p.release51.serial&&g_sync59c10r9.load(std::memory_order_acquire)){
                if(p.tick&&now>=p.tick&&now-p.tick<g_intent_live_ms59c17.load(std::memory_order_relaxed)){pendingLease.wait(p);return;}
                g_intent_direct_hits59c17.fetch_add(1,std::memory_order_relaxed);syncDirect59c10r9=true;syncReason59c12="VR_INTENT_STRIKE_WINDOW_NOT_OPEN_IN_TIME";goto sync_direct59c10r9;}
            if(syncOverdue59c10r9||syncOverdueAny59c12){syncDirect59c10r9=true;if(!p.release51.serial)syncReason59c12="NO_LIVE_ACTION_NO_RELEASE";goto sync_direct59c10r9;}
            if(p.release51.serial||g_sync59c10r9.load(std::memory_order_acquire)){pendingLease.wait(p);return;}}
        if(frik_pose_probe::CombatExperiment175Enabled()||!g_direct_fallback178r37.load(std::memory_order_relaxed)||!directOkR159){g_stats[5].fetch_add(1,std::memory_order_relaxed);
            if(Allow(4,6)){char m[220]{};std::snprintf(m,sizeof(m),"V66.178R37 GEOM_SKIP serial=%u reason=%s (live route selected: no direct-damage authority without Henry's live attack action)",p.serial,
                g_live_fault178r35.load()?"LIVE_ROUTE_FAULTED":verified!=1?"LIVE_HANDLER_PROLOGUE_MISMATCH":NativeOwned178r33(now)?"ENGAGEMENT_LATCHED_NO_LIVE_ACTION":"NO_LIVE_ACTION");diagnostic_trace::Write(m);}
            return;}
    }
sync_direct59c10r9:
    // R159 safety net: everything after this label is the mod's direct transport (BuildAndSubmitMeleeHit). in a tutorial no
    // branch above may get here (they are gated); if one ever does, the contact is not delivered instead of going direct.
    if(!directOkR159){g_stats[5].fetch_add(1,std::memory_order_relaxed);
        if(Allow(4,4)){char m[200]{};std::snprintf(m,sizeof(m),"R159 GEOM_SKIP serial=%u target=%u reason=TUTORIAL_NATIVE_ONLY (direct route off in a tutorial, no live strike window reached)",p.serial,p.entityId);diagnostic_trace::Write(m);}
        return;}
    if(syncDirect59c10r9){ // R59.16C10R9: exactly-once claim, then the ordinary direct transport below
        if(p.lease47c2.serial&&!serialReservationC2.reserve(p.release51,now,g_sync59c10r9.load(std::memory_order_acquire))){g_drop_serial_claimed56.fetch_add(1,std::memory_order_relaxed);
            if(Allow(4,4)){char m[260]{};std::snprintf(m,sizeof(m),"C56 SYNC_CONTACT serial=%u target=%u route=DIRECT_NATIVE_HIT refused=%s release=%llu claimed=%llu reason=%s ageMs=%llu",
                p.serial,p.entityId,serialReservationC2.reason(),(unsigned long long)p.lease47c2.serial,(unsigned long long)kcd2vr::contact176::claimedSerial.load(std::memory_order_acquire),syncReason59c12,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
            return;}
        if(!p.lease47c2.serial&&!noRelease54){
            if(Allow(4,4)){char m[220]{};std::snprintf(m,sizeof(m),"C56 SYNC_CONTACT serial=%u target=%u route=DIRECT_NATIVE_HIT refused=NO_RELEASE_AND_NO_PHYSICAL_ATTEMPT reason=%s ageMs=%llu",
                p.serial,p.entityId,syncReason59c12,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
            return;}
        const auto n=g_sync_direct59c10r9.fetch_add(1,std::memory_order_relaxed)+1;
        if(Allow(4,8)){char m[240]{};std::snprintf(m,sizeof(m),"R59.16C10R9 SYNC_CONTACT serial=%u target=%u ageMs=%llu route=DIRECT_NATIVE_HIT reason=%s direct=%llu live=%llu",
            p.serial,p.entityId,(unsigned long long)(now-p.tick),syncReason59c12,(unsigned long long)n,(unsigned long long)g_sync_live59c10r9.load());diagnostic_trace::Write(m);}
    }
