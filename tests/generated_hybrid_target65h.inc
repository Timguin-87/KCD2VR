    // C65H1 TARGET_STAGE_BEGIN (extracted unchanged for connected host tests)
    const void *target=nullptr,*te=nullptr,*ts=nullptr;const char* via="MAP";
    if(liveRoute39&&!vrDirect596&&g_sync59c10r9.load(std::memory_order_acquire)&&g_hybrid_wait65h.load(std::memory_order_acquire)){
        if(!AwaitHybridTarget65h(p,pendingLease,henryActor,hs,now,target,te,ts,via))return;
    }else{
    target=vrDirect596?directTarget19c3:ActorForEntity(p.entityId,now);
    if(!target&&!vrDirect596){ // the current opponent qualifies ONLY if it IS the struck entity (F03: no redirection to another NPC)
        const void* opp=Rd<const void*>(hs,0xCF8,nullptr);const void *oe=nullptr,*os=nullptr;
        if(opp&&opp!=henryActor&&CombatActor(opp,oe,os,false)&&oe&&EntityId(oe)==p.entityId){target=opp;via="OPPONENT_SAME_ENTITY";g_stats[6].fetch_add(1,std::memory_order_relaxed);}
    }
    if(target&&(target==henryActor||!CombatActor(target,te,ts,false)||!te||EntityId(te)!=p.entityId||
       ((p.targetWait19c3||p.metadata59c7g.occurrenceSerialH28)&&!RetainedContactBody19c3(p,te))))target=nullptr; // exact original actor/body or reject
    if(!target){g_stats[3].fetch_add(1,std::memory_order_relaxed);
        if(Allow(4,4)){char m[220]{};std::snprintf(m,sizeof(m),"V66.178R30 GEOM_HIT serial=%u rejected=TARGET_UNKNOWN entity=%u (no combat actor with this entity id seen within 500 ms)",p.serial,p.entityId);diagnostic_trace::Write(m);}return;}
    }

    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
        char lineH29[1100]{};std::snprintf(lineH29,sizeof(lineH29),"H29 STRIKE_NPC tick=%llu gesture=%llu sample=%llu serial=%u target=%u kind=%s originZone=%d nativeAttackZone=%d rawPart=%d bodyMaterial=%d surface=%d collider=%p contact=(%.5f,%.5f,%.5f) npcPhase=%d npcAttackType=%d npcPendingZone=%d npcOpponentZone=%d npcTargetsHenry=%d playerPhase=%d observation=BEFORE_NATIVE_CONTACT_ADMISSION decision=NOT_INFERRED",
            (unsigned long long)now,(unsigned long long)p.gesture59c3,(unsigned long long)p.sample,p.serial,p.entityId,p.kind?"STAB":"CUT",p.metadata59c7g.originZone47,I(hs,kAttackZone),p.part,p.bodyMaterial,p.surface,reinterpret_cast<void*>(p.metadata59c7g.collider),double(p.pos[0]),double(p.pos[1]),double(p.pos[2]),ts?I(ts,kActionPhaseProp39):-1,ts?I(ts,kAttackType):-1,ts?I(ts,kPendingZone):-1,ts?I(ts,kOpponentZone):-1,ts&&Rd<const void*>(ts,0xCF8,nullptr)==henryActor,I(hs,kActionPhaseProp39));diagnostic_trace::Write(lineH29);
    }
