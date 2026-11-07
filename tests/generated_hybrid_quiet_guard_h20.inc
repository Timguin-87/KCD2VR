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
