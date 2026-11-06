            const auto liveRecovery65h=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
            if(p.recovery55c1!=liveRecovery65h){RecoveryRetired65h(p,liveRecovery65h,GetTickCount64(),kcd2vr::recovery65h::Site::BeforeLive);g_drop_recovery56.fetch_add(1,std::memory_order_relaxed);
                if(Allow(4,4)){char m[200]{};std::snprintf(m,sizeof(m),"C56 GEOM_HIT serial=%u rejected=RECOVERY_EPOCH_CHANGED site=BEFORE_LIVE_HANDLER gesture=%llu release=%llu ageMs=%llu",
                    p.serial,(unsigned long long)p.gesture59c3,(unsigned long long)p.release51.serial,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
                return;}
