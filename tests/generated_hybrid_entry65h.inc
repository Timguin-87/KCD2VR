    const auto drainRecovery65h=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
    if(p.recovery55c1!=drainRecovery65h){RecoveryRetired65h(p,drainRecovery65h,GetTickCount64(),kcd2vr::recovery65h::Site::DrainEntry);++g_stats[5];
        // C56 review (Claude): the handling-recovery drop had no receipt (C54 15:56:16.161 serial 94: queued=1, 17.5 m/s,
        // release 81 admitted, phase 8 open, nothing logged; the next contact of the same swing carried gesture 0).
        g_drop_recovery56.fetch_add(1,std::memory_order_relaxed);
        if(Allow(4,4)){const auto nowR=std::uint64_t(GetTickCount64());char m[260]{};std::snprintf(m,sizeof(m),"C56 GEOM_HIT serial=%u rejected=RECOVERY_EPOCH_CHANGED site=DRAIN_ENTRY gesture=%llu release=%llu ageMs=%llu contactRecovery=%llu currentRecovery=%llu (handling recovery invalidates queued contacts by design)",
            p.serial,(unsigned long long)p.gesture59c3,(unsigned long long)p.release51.serial,(unsigned long long)(nowR>=p.tick?nowR-p.tick:0),(unsigned long long)p.recovery55c1,(unsigned long long)kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire));diagnostic_trace::Write(m);}
        return;}
