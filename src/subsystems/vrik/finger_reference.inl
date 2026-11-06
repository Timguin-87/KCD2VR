// V66.94: deterministic native-curve reference acquisition. No animation start,
// no user squeeze detector, no polling of the running FIFO. This is included after
// CoherentState and anatomy helpers, on Henry's validated natural pose callback.
struct FingerFile6694 {
    std::uint64_t magic=0x34393636474E4946ull,rig=0,checksum=0;
    std::uint32_t version=3,count[2]{};
    std::uint32_t crcs[2][32]{};
    QuatT28 relaxed[2][32]{},fist[2][32]{};
};
// Numeric evidence is copied once; only the worker formats/writes the report.
// Actual authored mesh clearance remains a headset/asset validation requirement.
struct FingerAudit6697 {
    FingerFile6694 file{};
    int ids[2][32]{},parents[2][32]{},selected[2]{-1,-1};
    std::uint32_t flags[5]{};
    QuatT28 phases[2][5][32]{};
    bool havePhases=false;
};
static FingerAudit6697 g_finger_audit6697{};
static bool g_finger_audit_pending6697=false;
static void WriteFingerAudit6697(const FingerAudit6697& a) noexcept {
    using Domain=kcd2vr::diagnostics::capture168::Domain;
    if(!diagnostic_trace::EnabledFor(Domain::HandReference))return;
    char line[1200]{};
    std::snprintf(line,sizeof(line),"D81 FINGER_REFERENCE rig=%016llX phases=%d selected=%d,%d counts=%u,%u flags=%u,%u,%u,%u,%u cache=MEMORY_ONLY",
        (unsigned long long)a.file.rig,int(a.havePhases),a.selected[0],a.selected[1],a.file.count[0],a.file.count[1],
        a.flags[0],a.flags[1],a.flags[2],a.flags[3],a.flags[4]);
    diagnostic_trace::WriteFor(Domain::HandReference,line);
    for(int side=0;side<2;++side)for(unsigned j=0;j<a.file.count[side]&&j<32;++j){
        const auto& o=a.file.relaxed[side][j];const auto& f=a.file.fist[side][j];
        std::snprintf(line,sizeof(line),"D81 FINGER_REFERENCE_JOINT rig=%016llX side=%d joint=%d parent=%d crc=%u offset=(%.9g,%.9g,%.9g) relaxed=(%.9g,%.9g,%.9g,%.9g) fist=(%.9g,%.9g,%.9g,%.9g)",
            (unsigned long long)a.file.rig,side,a.ids[side][j],a.parents[side][j],a.file.crcs[side][j],
            o.t[0],o.t[1],o.t[2],o.q[0],o.q[1],o.q[2],o.q[3],f.q[0],f.q[1],f.q[2],f.q[3]);
        diagnostic_trace::WriteFor(Domain::HandReference,line);
        if(a.havePhases)for(int k=0;k<5;++k){const auto& q=a.phases[side][k][j];
            std::snprintf(line,sizeof(line),"D81 FINGER_REFERENCE_PHASE rig=%016llX side=%d joint=%d phase=%d q=(%.9g,%.9g,%.9g,%.9g)",
                (unsigned long long)a.file.rig,side,a.ids[side][j],k,q.q[0],q.q[1],q.q[2],q.q[3]);
            diagnostic_trace::WriteFor(Domain::HandReference,line);
        }
    }
}
static std::mutex g_finger_io_mutex6694;
static FingerFile6694 g_finger_file6694{};
static std::atomic<std::uint64_t> g_finger_loaded6694{0};
static std::uint64_t FingerHash6694(const void* data,std::size_t size,std::uint64_t h=1469598103934665603ull) noexcept {
    auto* b=static_cast<const unsigned char*>(data);for(std::size_t i=0;i<size;++i){h^=b[i];h*=1099511628211ull;}return h;
}
// Called ONLY by the existing mod worker. No file access in the skeleton callback.
static void ServiceFingerReferenceIo6694() noexcept {
    if(kcd2vr::diagnostics::hands28::selected())kcd2vr::diagnostics::hands28::drain(GetTickCount64(),[](const char* line){diagnostic_trace::Write(line);});
    // Authored references are rebuilt once per rig and reused in memory.
    // User calibration remains in KCD2VR.ini; no loose BIN/JSON dump is needed.
    FingerAudit6697 audit{};bool haveAudit=false;
    {std::unique_lock<std::mutex> l(g_finger_io_mutex6694,std::try_to_lock);
     if(l.owns_lock()&&g_finger_audit_pending6697){audit=g_finger_audit6697;g_finger_audit_pending6697=false;haveAudit=true;}}
    if(haveAudit)WriteFingerAudit6697(audit);

}
struct FingerAcquire6694 {
    void* owner=nullptr;void* anim=nullptr;void* character=nullptr;std::uint32_t count=0;
    std::uint64_t initSerial59=0,bodyEpoch59=0,rig=0,retry=0;
    std::uint32_t crc[2][32]{};animation_filter::CurveReferenceInfo clip[5]{};
    QuatT28 open[2][32]{},candidate[2][32]{},best[2][32]{},phases[2][5][32]{};
    int selected[2]{-1,-1};
    int side=0,index=0,phase=0;unsigned attempts=0;bool initialized=false,resolved=false,done=false,fault=false;
    bool pendingPublish=false; // Retry only a numeric try-lock handoff, never native sampling or I/O.
    bool loadedCacheH7=false; // A file is not engine-native final-blend evidence.
    int animated[2]{};std::uint64_t start=0;
};
static FingerAcquire6694 g_finger_acquire6694;
// CombatClenchedHand is authored as a LOOP (retained combat ADB). Select
// one whole representative phase, not the phase that pushes tips closest to the
// wrist. Include the thumb, never compose per-joint extrema or mirror a hand.
// This is a deterministic candidate recipe, NOT proof of mesh-safe maximum curl.
static int FingerClenchPhase6697(const QuatT28 phases[5][32],int count) noexcept {
    if(count<=0||count>32)return -1;
    double scores[5]{};
    for(int p=0;p<4;++p)for(int q=0;q<4;++q)for(int j=0;j<count;++j){
        if(!ValidPose6689(phases[p][j])||!ValidPose6689(phases[q][j]))return -1;
        scores[p]+=pc::angle(PQ(QFromQuatT(phases[p][j])),PQ(QFromQuatT(phases[q][j])));
    }
    // Phase 1.0 is the duplicated loop endpoint; retain it for the audit,
    // but give phases 0/.25/.5/.75 equal weight. Mid-loop wins exact ties.
    int best=2;for(int p:{1,3,0})if(scores[p]<scores[best]-1e-8)best=p;
    return best;
}
static void QueueFingerAudit6697(const CoherentState6689& s,const FingerFile6694& f,
                                const FingerAcquire6694& a,bool havePhases) noexcept {
    // Caller owns g_finger_io_mutex6694. Never format or write here.
    auto& d=g_finger_audit6697;d={};d.file=f;d.havePhases=havePhases;
    for(int k=0;k<5;++k)d.flags[k]=a.clip[k].flags;
    for(int side=0;side<2;++side){d.selected[side]=havePhases?a.selected[side]:-1;
        for(int j=0;j<s.fingerCount[side];++j){d.ids[side][j]=s.fingerIds[side][j];d.parents[side][j]=s.parent[d.ids[side][j]];
            if(havePhases)for(int k=0;k<5;++k)d.phases[side][k][j]=a.phases[side][k][j];}}
    g_finger_audit_pending6697=true;
}
static void PublishCompletedReferences6697(const CoherentState6689& s,FingerAcquire6694& a) noexcept {
    if(!a.pendingPublish)return;
    std::unique_lock<std::mutex> lock(g_finger_io_mutex6694,std::try_to_lock);
    if(!lock.owns_lock())return;
    FingerFile6694 f{};f.rig=a.rig;
    for(int h=0;h<2;++h){f.count[h]=s.fingerCount[h];for(int j=0;j<s.fingerCount[h];++j){
        f.crcs[h][j]=a.crc[h][j];f.relaxed[h][j]=s.open[h].local[j];f.fist[h][j]=s.closed[h].local[j];}}
    f.checksum=0;f.checksum=FingerHash6694(&f,sizeof(f));
    g_finger_file6694=f;g_finger_loaded6694.store(f.rig,std::memory_order_release); // exact complete in-memory pair is immediately reusable
    QueueFingerAudit6697(s,f,a,true);a.pendingPublish=false;
}
static void AcquireCurveReferences6694(CoherentState6689& s,void* anim,std::uint64_t now,bool referenceOnlyH7=false) noexcept {
    if(!anim||!s.fingerMap[0]||!s.fingerMap[1])return;
    auto& a=g_finger_acquire6694;
    const auto bodyEpoch59=g_pose_epoch6689.load(std::memory_order_acquire);
    // C59: shared skeleton/animation owners can survive a character replacement.
    // A prior done/fault/pendingPublish must never strand the new hand with no
    // endpoints. Rebind immutable exact-rig cache; no sampling of live attacks.
    if(a.owner!=s.owner||a.anim!=anim||a.character!=s.character||a.count!=s.count||
       a.initSerial59!=s.initRuns137||a.bodyEpoch59!=bodyEpoch59||(referenceOnlyH7&&a.loadedCacheH7)){
        a={};a.owner=s.owner;a.anim=anim;a.character=s.character;a.count=s.count;
        a.initSerial59=s.initRuns137;a.bodyEpoch59=bodyEpoch59;a.start=now;
    }
    if(a.pendingPublish){PublishCompletedReferences6697(s,a);return;}
    if(referenceOnlyH7?(s.nativeCurveClosedH7[0].ready&&s.nativeCurveClosedH7[1].ready):
        (s.open[0].ready&&s.closed[0].ready&&s.open[1].ready&&s.closed[1].ready))return;
    if(a.fault||a.done)return;
    if(!a.initialized){
        void* joints=nullptr;if(!Read(static_cast<unsigned char*>(s.owner)+8,joints)||!joints)return;
        std::uint64_t h=FingerHash6694("V66.97/player-neutral-clench-loop-v3/bdf8f9e4",sizeof("V66.97/player-neutral-clench-loop-v3/bdf8f9e4")-1);
        for(int side=0;side<2;++side)for(int i=0;i<s.fingerCount[side];++i){
            const int id=s.fingerIds[side][i];
            // Exact native GetJointCRC32ByID +84415C: record size 0x30, CRC +0x08.
            if(!Read(static_cast<unsigned char*>(joints)+std::size_t(id)*0x30+8,a.crc[side][i]))return;
            h=FingerHash6694(&a.crc[side][i],sizeof(std::uint32_t),h);
            h=FingerHash6694(&s.parent[id],sizeof(int),h);h=FingerHash6694(&s.restLocal[id],sizeof(QuatT28),h);
        }
        a.rig=h;a.initialized=true;
    }
    if(!referenceOnlyH7&&g_finger_loaded6694.load(std::memory_order_acquire)==a.rig){
        std::unique_lock<std::mutex> lock(g_finger_io_mutex6694,std::try_to_lock);
        if(lock.owns_lock()&&g_finger_file6694.rig==a.rig){const auto& f=g_finger_file6694;bool ok=f.version==3;
            for(int side=0;side<2;++side){ok=ok&&f.count[side]==unsigned(s.fingerCount[side]);
                for(int i=0;i<s.fingerCount[side];++i)ok=ok&&f.crcs[side][i]==a.crc[side][i]&&ValidPose6689(f.relaxed[side][i])&&ValidPose6689(f.fist[side][i]);}
            if(ok){for(int side=0;side<2;++side){for(int i=0;i<s.fingerCount[side];++i){s.open[side].local[i]=f.relaxed[side][i];s.closed[side].local[i]=f.fist[side][i];}s.open[side].ready=s.closed[side].ready=true;}
                QueueFingerAudit6697(s,f,a,false);a.done=true;a.loadedCacheH7=true;diagnostic_trace::Write("V66.97-FINGER REFERENCES_LOADED source=NATIVE_CURVE_CACHE liveFifoRequired=0 visualAcceptance=UNVERIFIED");return;}
        }
    }
    if(now<a.retry)return;
    constexpr const char* names[]{"relaxed_idle_both_player","combat_lg_sz2_idle_nwpn_player","combat_rg_sz1_idle_nwpn_player","combat_attack_lg_z2_nwpn_add","combat_attack_rg_z1_nwpn_add"};
    if(!a.resolved){bool ok=true;for(int k=0;k<5;++k)ok=animation_filter::ResolveCurveReference(anim,names[k],a.clip[k])&&ok;
        if(!ok){a.retry=now+2000;if(++a.attempts>=12)a.done=true;
            if(a.attempts==1||a.done){char m[256]{};std::snprintf(m,sizeof(m),"V66.94-FINGER CURVE_LOOKUP_UNAVAILABLE attempts=%u finite=1 liveFifoRequired=0",a.attempts);diagnostic_trace::Write(m);}return;}
        a.resolved=true;
    }
    // At most four joints (twelve native curve samples) per natural pose callback;
    // five whole authored phases are compared, never component-wise extrema.
    constexpr float phases[]{0.0f,0.25f,0.5f,0.75f,1.0f};
    for(int budget=0;budget<4&&!a.done;++budget){
        const int side=a.side,i=a.index,id=s.fingerIds[side][i];QuatT28 open{},base{},add{};
        const int ro=animation_filter::SampleCurveReference(anim,a.clip[0].id,a.crc[side][i],.5f,open.q);
        const int rb=animation_filter::SampleCurveReference(anim,a.clip[1+side].id,a.crc[side][i],.5f,base.q);
        const int rc=animation_filter::SampleCurveReference(anim,a.clip[3+side].id,a.crc[side][i],phases[a.phase],add.q);
        const auto unavailable=[](int r){return r!=0&&r!=4;};
        if(unavailable(ro)||unavailable(rb)||unavailable(rc)){
            a.retry=now+2000;if(ro<0||rb<0||rc<0)a.fault=true;
            if(++a.attempts>=12)a.done=true;
            if(a.attempts==1||a.done||a.fault){char m[320]{};std::snprintf(m,sizeof(m),"V66.94-FINGER CURVE_SAMPLE_UNAVAILABLE side=%c joint=%d status=%d/%d/%d tries=%u nativeFault=%d writes=NONE",side?'L':'R',id,ro,rb,rc,a.attempts,a.fault);diagnostic_trace::Write(m);}return;
        }
        // Unanimated bones retain their base local transform, as native layering
        // does. Asset additive bit is read from the native CAF header (not '_add').
        const auto apply=[&](int status,const QuatT28& v,const QuatT28& reference,std::uint32_t flags){QuatT28 out=reference;
            if(status==0)QToQuatT(out,(flags&1u)?QNormalize(QMul(QFromQuatT(reference),QFromQuatT(v))):QNormalize(QFromQuatT(v)));
            ToT(out,FromT(s.restLocal[id]));return out;};
        a.open[side][i]=apply(ro,open,s.restLocal[id],a.clip[0].flags);
        base=apply(rb,base,s.restLocal[id],a.clip[1+side].flags);
        a.candidate[side][i]=apply(rc,add,base,a.clip[3+side].flags);
        if(!ValidPose6689(a.open[side][i])||!ValidPose6689(a.candidate[side][i])){a.fault=true;return;}
        if(a.phase==0&&rc==0)++a.animated[side];
        if(++a.index<s.fingerCount[side])continue;
        a.index=0;std::copy_n(a.candidate[side],32,a.phases[side][a.phase]);
        if(++a.phase<5)continue;a.phase=0;
        a.selected[side]=FingerClenchPhase6697(a.phases[side],s.fingerCount[side]);
        if(a.selected[side]<0){a.fault=true;return;}
        std::copy_n(a.phases[side][a.selected[side]],32,a.best[side]);
        // Require real clench curves, not an all-rest hand sold as a fist.
        if(a.animated[side]<5){a.done=true;diagnostic_trace::Write("V66.94-FINGER INCOMPLETE_CLENCH_CURVES nativeFallback=1");return;}
        for(int j=0;j<s.fingerCount[side];++j){
            s.nativeCurveClosedH7[side].local[j]=a.best[side][j];
            if(!referenceOnlyH7){s.open[side].local[j]=a.open[side][j];s.closed[side].local[j]=a.best[side][j];}
        }
        auto& nativeH7=s.nativeCurveClosedH7[side];nativeH7.ready=true;nativeH7.sequence=a.rig;
        nativeH7.issue="ASSET_DECODED_BASE_ADDITIVE_REFERENCE_NATIVE_BLEND_UNPROVEN";
        if(!referenceOnlyH7)s.open[side].ready=s.closed[side].ready=true;
        char m[448]{};std::snprintf(m,sizeof(m),"V66.94-FINGER CURVE_REFERENCE_READY side=%c count=%d clenchControllers=%d flags=%X/%X/%X phases=5 neutral=PLAYER_IDLE recipe=3 selection=WHOLE_AUTHORED_LOOP_MEDOID parentLocal=1 index=TRIGGER others=GRIP source=AUTHORED_CURVE_COMPOSITION visualAcceptance=UNVERIFIED",side?'L':'R',s.fingerCount[side],a.animated[side],a.clip[0].flags,a.clip[1+side].flags,a.clip[3+side].flags);diagnostic_trace::Write(m);
        if(++a.side<2)continue;a.done=true;
        // The optional bank is presentation-only. It must never replace the
        // default cache/publication, including after a later body reinitializes.
        if(!referenceOnlyH7){a.pendingPublish=true;PublishCompletedReferences6697(s,a);}
    }
}
