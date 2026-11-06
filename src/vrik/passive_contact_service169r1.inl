// Runs in the existing diagnostic-service path, never a physics/pose producer.
static void ServicePassiveContact169R1() noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    namespace trace=kcd2vr::diagnostics::passive169r1;
    namespace p=kcd2vr::passive169r1;
    const auto now=std::uint64_t(GetTickCount64());
    kcd2vr::diagnostics::bow169r2::drain(now,[](const char* line){diagnostic_trace::Write(line);});
    static std::uint64_t sessions[2]{},lastSummary=0;static bool wasActive=false;
    static trace::captureD84::Ticket previousCapture{};
    const auto capture=trace::captureD84::select(now);const bool active=capture.session!=0;
    const std::uint64_t current[2]{cap::active(cap::Domain::Combat,now)?cap::gate.session[0].load():0,
        cap::active(cap::Domain::Interaction,now)?cap::gate.session[1].load():0};
    if(active&&(!trace::captureD84::same(capture,previousCapture)||current[0]!=sessions[0]||current[1]!=sessions[1])){
        sessions[0]=current[0];sessions[1]=current[1];
        char m[1024]{};std::snprintf(m,sizeof(m),
            "V66.169R1 CONTACT_COVERAGE recorder=%s captureSession=%llu captureMask=0x%X bodyExtension=174 combatSession=%llu interactionSession=%llu bodyQueriesSelected=%d hand=FINAL_COMPOSED_POINTS self=EXACT_BODY_AND_EQUIPMENT_SKIP npcShape=NATIVE_PHYSICS_REPRESENTATION nativeNpcDamage=UNCHANGED constraints=NONE material=UNDECODED haptics=INDEPENDENT_OF_CAPTURE",
            cap::name(capture.domain),(unsigned long long)capture.session,capture.activeMask,
            (unsigned long long)sessions[0],(unsigned long long)sessions[1],NpcQuerySelected172());diagnostic_trace::WriteReceiptFor(capture.domain,capture.session,m);
    }
    trace::Record r{};int budget=48;
    while(budget--&&trace::pop(r)){
        if(!trace::captureD84::receipt(r.capture,now))continue; // no formatting of expired/replaced records
        const auto& s=r.sample;char m[1664]{};
        if(r.kind==1){
            char raw[0x50*2+1]{};constexpr char digits[]="0123456789abcdef";
            for(std::size_t i=0;i<r.raw.size();++i){raw[i*2]=digits[r.raw[i]>>4];raw[i*2+1]=digits[r.raw[i]&15];}
            std::snprintf(m,sizeof(m),
                "V66.169R1 CONTACT_RAY recorder=%s captureSession=%llu captureMask=0x%X serial=%llu tick=%llu sample=%llu combatSession=%llu interactionSession=%llu source=%s side=%c actorKind=CHARACTER actor=%llX key=%016llX generation=%llu epoch=%llu request=%llu origin=(%.5f,%.5f,%.5f) motion=(%.5f,%.5f,%.5f) distance=%.5f raw80=%s geometry=POINT_NORMAL_VALIDATED queryMask=0x%X collider=%llX bodyVftRva=%llX bodyClass=%s owner=%llX entityId=%u part=%d ownerRead=%d rigidCurrentSampleD67R1=%llu rigidCurrentTickD67R1=%llu materialDecoded=0 damageRequested=0 addedQueries=0",
                cap::name(r.capture.domain),(unsigned long long)r.capture.session,r.capture.activeMask,
                (unsigned long long)r.serial,(unsigned long long)s.tick,(unsigned long long)s.sequence,(unsigned long long)r.combatSession,(unsigned long long)r.interactionSession,
                p::name(s.source),s.side?'L':'R',(unsigned long long)s.actor,(unsigned long long)s.key,(unsigned long long)s.generation,(unsigned long long)s.epoch,(unsigned long long)s.request,
                r.rayOrigin.x,r.rayOrigin.y,r.rayOrigin.z,r.rayMotion.x,r.rayMotion.y,r.rayMotion.z,double(r.distance),raw,
                r.native172.mask,(unsigned long long)r.native172.collider,(unsigned long long)r.native172.vft,npc172::name(r.native172.body),
                (unsigned long long)r.native172.owner,r.native172.entityId,r.native172.part,r.native172.ownerRead,
                (unsigned long long)r.native172.rigidCurrentSampleD67R1,(unsigned long long)r.native172.rigidCurrentTickD67R1);
        }else{
            const auto& d=r.decision;
            std::snprintf(m,sizeof(m),
                "V66.169R1 CONTACT_EVENT recorder=%s captureSession=%llu captureMask=0x%X tick=%llu sample=%llu combatSession=%llu interactionSession=%llu source=%s side=%c actorKind=%s actor=%llX key=%016llX generation=%llu epoch=%llu request=%llu episode=%llu status=%s phase=%s eligible=%d worldCoherent=%d witness=%llu motionValid=%d inwardMps=%.4f slipMps=%.4f slipM=%.5f gapM=%.5f pointWorld=(%.5f,%.5f,%.5f) normalMotionFrame=(%.5f,%.5f,%.5f) feature=%d flags=0x%X hapticQueued=%d nativeHit=NOT_REQUESTED_BY_PASSIVE material=UNRESOLVED fx=NOT_REQUESTED",
                cap::name(r.capture.domain),(unsigned long long)r.capture.session,r.capture.activeMask,
                (unsigned long long)s.tick,(unsigned long long)s.sequence,(unsigned long long)r.combatSession,(unsigned long long)r.interactionSession,
                p::name(s.source),s.side?'L':'R',s.source==p::Source::WeaponWorld?"C_PLAYER":"CHARACTER",(unsigned long long)s.actor,(unsigned long long)s.key,(unsigned long long)s.generation,
                (unsigned long long)s.epoch,(unsigned long long)s.request,(unsigned long long)d.episode,p::name(s.status),p::name(d.phase),s.eligible,s.worldCoherent,
                (unsigned long long)s.witness,d.motion.valid,d.motion.incoming,d.motion.slipSpeed,d.motion.slipDistance,s.gap,s.point.x,s.point.y,s.point.z,
                s.normal.x,s.normal.y,s.normal.z,s.feature,s.flags,r.hapticQueued);
        }
        diagnostic_trace::WriteReceiptFor(r.capture.domain,r.capture.session,m);
    }
    const auto summaryCapture=active?capture:previousCapture;
    if(trace::captureD84::receipt(summaryCapture,now)&&((active&&(!lastSummary||now<lastSummary||now-lastSummary>=5000))||(!active&&wasActive))){
        lastSummary=now;char m[1024]{};std::snprintf(m,sizeof(m),
            "V66.169R1 CONTACT_SUMMARY recorder=%s captureSession=%llu captureMask=0x%X active=%d dropped=%llu rawRateLimited=%llu bodySelected=%d gate=%s bodyQueries=%llu blockedQueries=%llu selfRejected=%llu hitsOther=%llu hitsBase=%llu hitsLiving=%llu hitsArticulated=%llu hitsRigid=%llu queueCapacity=255 npcAbsence=NOT_A_NEGATIVE_RESULT ragdollState=NOT_INFERRED queryBudget=EXISTING_18_RAYS_1000_US",
            cap::name(summaryCapture.domain),(unsigned long long)summaryCapture.session,summaryCapture.activeMask,
            active,(unsigned long long)trace::dropped.load(),(unsigned long long)trace::limited.load(),NpcQuerySelected172(),npc172::name(g_npc_gate172.load()),
            (unsigned long long)g_npc_queries172.load(),(unsigned long long)g_npc_blocked172.load(),(unsigned long long)g_npc_self172.load(),
            (unsigned long long)g_npc_hits172[0].load(),(unsigned long long)g_npc_hits172[1].load(),
            (unsigned long long)g_npc_hits172[2].load(),(unsigned long long)g_npc_hits172[3].load(),
            (unsigned long long)g_npc_hits172[4].load());diagnostic_trace::WriteReceiptFor(summaryCapture.domain,summaryCapture.session,m);
    }
    if(active)previousCapture=capture;
    wasActive=active;
}
