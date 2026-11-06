    const BlockClockReceipt19 victimClock19=vrDirect596?CopyBlockClockReceipt19(target,victimPhaseC10):BlockClockReceipt19{};
    const bool victimTargetsHenryC10=ts&&Rd<const void*>(ts,0xCF8,nullptr)==henryActor;
    const int victimZoneC10=ts?I(ts,kOpponentZone):-1,ourDefenseC10=hs?I(hs,kDefenseZone):-1;
    const bool defenseCaptureC10=(vrDirect596||syncDirect59c10r9)&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now);
    const char* victimBool300C10=defenseCaptureC10?vrstate596::ReadDefenseBoolDiagnostic(ts,0x300):"NOT_CAPTURED";
    const char* victimBool680C10=defenseCaptureC10?vrstate596::ReadDefenseBoolDiagnostic(ts,0x680):"NOT_CAPTURED";
    if(defenseCaptureC10&&Allow(4,20)){
        const void* committed=Rd<const void*>(hs,0xCE8,nullptr);
        // Exact retail C_CombatTarget (vft3A796F0, GetActor947210 = mov rax,[rcx+8];ret).
        // Other handle implementations are UNKNOWN; no virtual call or ownership change here.
        const bool committedKnown=committed&&Rd<std::uintptr_t>(committed,0,0)==g_base+0x3A796F0;
        const void* committedEntity=committedKnown?Rd<const void*>(committed,8,nullptr):nullptr;
        const char* committedMatch=!committedKnown?"UNKNOWN":committedEntity==te?"YES":"NO";
        const auto cf8=Rd<const void*>(hs,0xCF8,nullptr);
        char line[570]{};std::snprintf(line,sizeof(line),
        "C10R19C1 DEFENSE_INPUT serial=%u gesture=%llu CE8present=%d CE8kindKnown=%d CE8matchesContact=%s CF8matchesContact=%d attackerCE8=0x%llX attackerCF8=0x%llX contactActor=0x%llX reach4B0=%u reach4E0=%u victimPhase=%d victimZone5D0=%d attackZone150=%d logicalAttackType210=%d transportAttackType=%d guardReady=NOT_INFERRED",
        p.serial,(unsigned long long)p.gesture59c3,committed?1:0,committedKnown?1:0,committedMatch,cf8==target?1:0,
        (unsigned long long)reinterpret_cast<std::uintptr_t>(committed),(unsigned long long)reinterpret_cast<std::uintptr_t>(cf8),
        (unsigned long long)reinterpret_cast<std::uintptr_t>(target),unsigned(B(hs,0x4B0)),unsigned(B(hs,0x4E0)),victimPhaseC10,victimZoneC10,ourDefenseC10,I(hs,kAttackType),type);diagnostic_trace::Write(line);}
    if(defenseCaptureC10&&vrDirect596)vrstate596::EmitDefenseTimingContact39(p.serial,p.gesture59c3,target,ts,victimPhaseC10,now); // C39 edge-only
