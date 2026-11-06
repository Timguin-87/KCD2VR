    if(liveRoute39&&!vrDirect596&&g_sync59c10r9.load(std::memory_order_acquire)){
        namespace mo=kcd2vr::motion59c4;
        const mo::Key key{p.gesture59c3,p.key,p.generation,p.epoch,p.request,p.recovery55c1,
            reinterpret_cast<std::uintptr_t>(Rd<const void*>(henryActor,0x1F8,nullptr))};
        mo::Attempt attempt{};
        auto result=mo::hybridAttempts54.claim(key,p.tick,now,p.entityId,attempt);
        if(result==mo::Claim::Wait)result=mo::hybridAttempts54.contact(key,p.tick,now,p.entityId,p.kind==1?1:0,attempt);
        if(result==mo::Claim::Wait){pendingLease.wait(p);return;}
        if(result!=mo::Claim::Ready){
            if(result==mo::Claim::Duplicate)g_drop_duplicate56.fetch_add(1,std::memory_order_relaxed); // expected: same swing already answered (counter only)
            if(result!=mo::Claim::Duplicate&&Allow(4,12)){char m[240]{};std::snprintf(m,sizeof(m),
                "C54 HYBRID_CONTACT serial=%u gesture=%llu target=%u claim=%u nativeRelease=%llu noRelease=%d delivered=0",
                p.serial,(unsigned long long)key.gesture,p.entityId,unsigned(result),(unsigned long long)p.release51.serial,int(noRelease54));diagnostic_trace::Write(m);}
            return;
        }
        hybridReservation54.hold(mo::hybridAttempts54,key);
        if(noRelease54){
            // Preserve all real hit/clinch/paired ownership. A missing release
            // cannot create an attack opportunity from a protected native phase.
            const int phase=I(hs,kActionPhaseProp39);
            const bool quietGuardH20=(phase==128||phase==256)&&frik_pose_probe::HybridQuietGuardOwnedH20(henryActor,hs);
            if(!kcd2vr::hybrid54::ordinaryPhase(phase,quietGuardH20)){g_drop_nonordinary_phase56.fetch_add(1,std::memory_order_relaxed);
                if(Allow(4,4)){char m[240]{};std::snprintf(m,sizeof(m),"C56 HYBRID_CONTACT serial=%u gesture=%llu target=%u route=NO_RELEASE reason=PROTECTED_NATIVE_PHASE phase=%d applied=0 ageMs=%llu",
                    p.serial,(unsigned long long)key.gesture,p.entityId,phase,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
                return;}
            // A dispatched/absent release does not establish whether native
            // preparation already charged. Do not create free hits, debit a
            // guessed fixed cost, or charge an unjoined asynchronous effect
            // twice. Only this full gesture's completed native proof counts.
            const auto cost54=kcd2vr::hybrid54::costProof54(kcd2vr::swing_effort59c14::ledger,key,p.tick,now,
                kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire));
            if(cost54==kcd2vr::hybrid54::CostProof54::Wait){
                pendingLease.defer(p); // one bounded yield; original160ms contact lease and all owner checks remain
                if(Allow(4,12)){char m[240]{};std::snprintf(m,sizeof(m),
                    "C54 HYBRID_CONTACT serial=%u gesture=%llu target=%u route=NO_RELEASE reason=EXACT_NATIVE_COST_PENDING applied=0 ageMs=%llu",
                    p.serial,(unsigned long long)key.gesture,p.entityId,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
                return;
            }
            if(cost54!=kcd2vr::hybrid54::CostProof54::Ready){
                if(Allow(4,12)){char m[260]{};std::snprintf(m,sizeof(m),
                    "C54 HYBRID_CONTACT serial=%u gesture=%llu target=%u route=NO_RELEASE reason=NO_NATIVE_COST_WITNESS applied=0 nativeCalls=0 ageMs=%llu",
                    p.serial,(unsigned long long)key.gesture,p.entityId,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
                return;
            }
        }
    }
    if(liveRoute39&&!vrDirect596&&p.lease47c2.serial&&kcd2vr::attackclock50::enabled.load()){
        kcd2vr::attackclock50::Contact c{};c.actor=reinterpret_cast<std::uintptr_t>(henryActor);
        c.serial=p.lease47c2.serial;c.policy=p.lease47c2.policy;c.key=p.key;c.generation=p.generation;
        c.epoch=p.epoch;c.request=p.request;c.tick=p.tick;c.sample=p.release51.sample;c.releaseTick=p.release51.tick;
        c.gesture59c17=p.gesture59c3?p.gesture59c3:p.release51.gesture59c3; // R59.16C10R17
        (void)kcd2vr::attackclock50::contact.publish(c);
    }
