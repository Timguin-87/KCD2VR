    if(liveRoute39&&!vrDirect596){
        const handoff59c3::Key key{p.gesture59c3,p.key,p.generation,p.epoch,p.request,p.recovery55c1};
        kcd2vr::contact176::Ticket ticket=p.release51;
        if(!ticket.serial)(void)handoff59c3::releases.find(key,ticket);
        const auto result=handoff59c3::join(key,ticket,p.tick,now,kcd2vr::contact176::recoveryEpoch55c1.load());
        const auto rendezvous54=kcd2vr::hybrid54::rendezvous(result,
            g_sync59c10r9.load(std::memory_order_acquire),g_sync_overdue_any59c12.load(std::memory_order_relaxed),
            p.tick,now,g_sync_grace_ms59c10r9.load(std::memory_order_relaxed));
        if(rendezvous54==kcd2vr::hybrid54::Rendezvous::Wait){pendingLease.wait(p);return;}
        noRelease54=rendezvous54==kcd2vr::hybrid54::Rendezvous::Physical;
        if(!noRelease54&&(result!=handoff59c3::Join::Bound||ticket.entity!=reinterpret_cast<std::uintptr_t>(he))){
            ++g_stats[5];if(Allow(4,6)){char line[192]{};
                std::snprintf(line,sizeof(line),"R59C3 CONTACT_JOIN reject=%u gesture=%llu release=%llu ageMs=%llu",
                    unsigned(result),(unsigned long long)p.gesture59c3,(unsigned long long)ticket.serial,(unsigned long long)(now-p.tick));
                diagnostic_trace::Write(line);}return;
        }
        if(!noRelease54){p.release51=ticket;p.lease47c2={ticket.policy,ticket.serial};}
    }
