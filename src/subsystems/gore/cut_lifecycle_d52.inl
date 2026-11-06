// Included inside the existing native gore owner; production watch and gated receipts.
// Bounded F3 receipts only. Formatting is outside ordinary gameplay.
static void TraceCutLifecycleD52(std::uint64_t now,const char* phase,std::uint32_t target,
    std::uint64_t batch,const char* layer,bool acknowledged) noexcept {
    namespace cap=diagnostics::capture168;if(!cap::active(cap::Domain::Combat,now))return;
    const auto session=cap::gate.session[unsigned(cap::Domain::Combat)].load();
    static std::uint64_t previous=0;static unsigned count=0;
    if(previous!=session){previous=session;count=0;}if(count>=96)return;++count;
    unsigned batchLeases=0,cleanupDebt=0;
    for(const auto& lease:g_head_visibility59.leases)if(lease.active&&lease.batch==batch&&lease.key.target==target){++batchLeases;if(lease.restorePending)++cleanupDebt;}
    char line[340]{};std::snprintf(line,sizeof(line),
        "D52 GORE_LIFECYCLE session=%llu sample=%u/96 phase=%s target=%u batch=%llu layer=%s acknowledged=%d batchLeases=%u cleanupDebt=%u totalLeases=%u",
        (unsigned long long)session,count,phase,target,(unsigned long long)batch,layer,acknowledged?1:0,batchLeases,cleanupDebt,g_head_visibility59.activeCount);
    diagnostic_trace::Write(line);
}
// Keep the exact acquired keys. An attachment found later by the same name is
// not automatically ours. Original visibility flags/batch remain unchanged.
struct CutWatch43 {visibility59::Key key{};std::uint64_t until=0,batch=0,next=0;};
static CutWatch43 g_cut_watch43[128]{};
static void TickCut43(std::uint64_t now) noexcept {
    for(auto& w:g_cut_watch43){if(!w.key.target)continue;if(now>w.until){w={};continue;}
        if(now<w.next)continue;w.next=now+100; // event-local watch, bounded reads
        auto* lease=g_head_visibility59.find(w.key,w.batch);
        if(!lease||lease->restorePending){w={};continue;}
        HeadVisibilityIO59 io{};std::uint32_t copy=0,original=0;
        const auto copyState=io.observe(lease->replacement,copy);
        if(copyState==visibility59::Observation::Retired||
           (copyState==visibility59::Observation::Current&&(copy&0x10000u))){w={};continue;}
        if(copyState!=visibility59::Observation::Current)continue;
        const auto originalState=io.observe(w.key,original);
        if(originalState==visibility59::Observation::Retired){g_head_visibility59.restoreKey(io,w.key,w.batch,now);w={};continue;}
        if(originalState!=visibility59::Observation::Current||original==lease->owned)continue;
        const bool hidden=g_head_visibility59.reassert(io,w.key,w.batch,now);
        TraceCutLifecycleD52(now,"REASSERT",w.key.target,w.batch,w.key.name,hidden);
    }
}
// Existing event paths only: no additional native scan or material lookup.
static void TraceHeadPartsD52(const gp50::Event& e,std::uint64_t now) noexcept {
    namespace cap=diagnostics::capture168;if(!cap::active(cap::Domain::Combat,now))return;
    const auto session=cap::gate.session[unsigned(cap::Domain::Combat)].load();
    static std::uint64_t previous=0;static unsigned count=0;
    if(previous!=session){previous=session;count=0;}if(count>=12)return;++count;
    for(int i=0;i<e.nAtt&&i<24;++i){
        char line[640]{};std::snprintf(line,sizeof(line),
            "D52 GORE_HEAD_PARTS session=%llu event=%u/12 target=%u part=%d/%d attachment=%s model=%s primary=%s materialWitness=MODEL_PATH_ONLY",
            (unsigned long long)session,count,e.target,i+1,e.nAtt,e.attName[i]?e.attName[i]:"-",
            e.attSkin[i]?e.attSkin[i]:"-",e.headSkin?e.headSkin:"-");
        diagnostic_trace::Write(line);
    }
}
