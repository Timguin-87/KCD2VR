    { // C44 VrDirectStaggerRule 2: a DIRECT strike that started out of Henry's own hit reaction (64) lands at half strength (rule 1 never gets here: the phase publish yields)
     const int rule44=kcd2vr::direct_counter39::staggerRule44.load(std::memory_order_relaxed);const bool fromStagger44=vrDirect596&&apply&&vrstate596::FromStagger44();
     const float weak44=kcd2vr::direct_counter39::staggerStrength44(rule44,fromStagger44,strength);
     if(fromStagger44&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){char m[240]{};std::snprintf(m,sizeof(m),
      "C44 STAGGER_STRIKE serial=%u target=%u henryStartPhase=64 rule=%d strength=%.2f->%.2f staggerMs=%u nativePhase64=UNCHANGED writes=STRENGTH_ARGUMENT_ONLY",
      p.serial,p.entityId,rule44,double(strength),double(weak44),vrstate596::g_stagger_ms59c16.load(std::memory_order_relaxed));diagnostic_trace::Write(m);}
     strength=weak44;}
