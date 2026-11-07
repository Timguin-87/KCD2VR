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
    }
