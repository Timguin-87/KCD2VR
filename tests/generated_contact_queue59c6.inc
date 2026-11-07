struct Pending {std::uint64_t gesture59c3=0;std::uint32_t entityId=0;float pos[3]{};float energy=1.f;int kind=0;int part=-1;int surface=-1;int bodyMaterial=-1;std::uint64_t tick=0;std::uint32_t serial=0; // R59.16C7E: +bodyMaterial
    float powerRatio59c8=1.f;bool twoHanded59c8=false; // R59.16C8: strike energy / reference (unclamped) and support-hand grip at impact
    const void* action=nullptr; // V66.178R40 (direction review 4): the native action this contact was bound to on first sight; another action may not deliver it
    kcd2vr::contact47c2::Lease lease47c2{};
    kcd2vr::contact176::Ticket release51{}; // copied identity only, never a native pointer lease
    std::uint64_t recovery55c1=0;
    bool weaponItem=false; // V66.178R43: the struck entity is the opponent's WEAPON/SHIELD item (a block), not a body
    bool targetWait19c3=false; // bookkeeping only: same original contact waiting for the observed actor map
    unsigned targetWaits65h=0; // bounded missing-actor retries; does not refresh original impact
    std::uint64_t key=0,generation=0,epoch=0,request=0,sample=0,queued=0;
    kcd2vr::contact59c7g::Metadata metadata59c7g{};}; // C7G: immutable metadata belongs to this exact queued contact
// R59.16C7E: the impact's body material travels beside the unchanged submit signature, keyed by the impact sample id.
struct BodyMaterialNote {std::atomic<std::uint64_t> sample{0};std::atomic<int> material{-1};};
inline std::array<BodyMaterialNote,8> g_body_material59c7e{};
inline void NoteBodyMaterial59c7e(std::uint64_t sample,int material) noexcept {
    if(!sample)return;static std::atomic<unsigned> next{0};auto& n=g_body_material59c7e[next.fetch_add(1,std::memory_order_relaxed)%8];
    n.material.store(material,std::memory_order_relaxed);n.sample.store(sample,std::memory_order_release);}
std::mutex g_mutex;Pending g_pending{};bool g_has=false,g_draining59c3=false;std::uint64_t g_last_submit=0;std::uint32_t g_serial=0;
kcd2vr::contact_backlog59c6::Queue<Pending,8> g_backlog59c6;
std::atomic<std::uint64_t> g_backlogQueued59c6{0},g_backlogFull59c6{0},g_backlogExpired59c6{0};
// C56 review (Claude): per-site drop counters for the exits that used to return without a line
// (the 12-log analysis: 10-16 % of committed swings ended in "queued=1, no receipt"). Counters only
// here; the bounded per-site lines are written at the exits themselves. Never reset.
std::atomic<std::uint64_t> g_drop_recovery56{0},        // Drain entry / before the live handler: handling-recovery epoch changed
    g_drop_waiting_expired56{0},                            // contact expired (160 ms) while it was waiting for the strike window
    g_drop_flag220_56{0},                                   // phase 4/8 but [actor+0x220]+0x28 != 0 (C5235C precondition)
    g_drop_serial_claimed56{0},                             // release serial already claimed (earlier contact / native answer / admission)
    g_drop_duplicate56{0},                                  // hybrid attempt already consumed (same swing already answered) - expected
    g_drop_weapon_item_live56{0},                           // weapon/shield item contact on the live route (physical stop only)
    g_drop_nonordinary_phase56{0},                          // NO_RELEASE route refused by a protected native phase
    g_drop_outer_unavailable56{0};                          // direct transport preflight: reaction/notification transport missing
std::atomic<bool> g_hybrid_wait65h{true};
std::atomic<std::uint64_t> g_hybrid_waits65h{0},g_hybrid_resolved65h{0};
static void RecoveryRetired65h(const Pending& p,std::uint64_t epoch,std::uint64_t now,kcd2vr::recovery65h::Site site) noexcept {
 kcd2vr::recovery65h::retired(p,epoch,now,site,diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now));
}

bool Submit(const Pending& p) noexcept {
    // Construct before the queue lock: format only after that lock is released.
    struct QueueTraceC54R3 {const Pending& p;const char* why="UNOBSERVED";std::uint32_t serial=0;bool queued=false;
        ~QueueTraceC54R3(){const auto now=std::uint64_t(GetTickCount64());
            if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;char line[540]{};
            std::snprintf(line,sizeof(line),"C54R3 HIT_QUEUE gesture=%llu sample=%llu target=%u tick=%llu ageMs=%llu serial=%u queued=%d why=%s key=%016llX generation=%llu epoch=%llu request=%llu nativeAcceptance=NOT_INFERRED",
                (unsigned long long)p.gesture59c3,(unsigned long long)p.sample,p.entityId,(unsigned long long)p.tick,
                (unsigned long long)(now>=p.tick?now-p.tick:UINT64_MAX),serial,int(queued),why,(unsigned long long)p.key,(unsigned long long)p.generation,(unsigned long long)p.epoch,(unsigned long long)p.request);diagnostic_trace::Write(line);}
    } queueTrace{p};
    g_stats[0].fetch_add(1,std::memory_order_relaxed);
    std::lock_guard<std::mutex> lock(g_mutex);
    const auto now=std::uint64_t(GetTickCount64());
    const auto recovery=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
    if(!p.gesture59c3||!kcd2vr::contact176::fresh(p.tick,now,handoff59c3::maximumContactAge)){
        queueTrace.why=!p.gesture59c3?"NO_PHYSICAL_GESTURE":"IMPACT_EXPIRED_OR_FUTURE_160MS";
        // C67 review: enrich the existing bounded receipt from the original
        // impact. No extra actor reads, guessed speed, or contact-minted gesture.
        ++g_stats[5];if(Allow(4,16)){char line[480]{};std::snprintf(line,sizeof(line),
            "C54 HYBRID_ENQUEUE gesture=%llu sample=%llu target=%u kind=%d queued=0 reason=%s ageMs=%llu normalizedEnergy=%.3f powerRatio=%.3f part=%d surface=%d weaponItem=%d serial=%u",
            (unsigned long long)p.gesture59c3,(unsigned long long)p.sample,p.entityId,p.kind,
            !p.gesture59c3?"NO_PHYSICAL_GESTURE":"IMPACT_EXPIRED_OR_FUTURE",
            (unsigned long long)(now>=p.tick?now-p.tick:UINT64_MAX),double(p.energy),double(p.powerRatio59c8),
            p.part,p.surface,p.weaponItem?1:0,p.serial);diagnostic_trace::Write(line);}return false;
    }
    auto usable=[&](const Pending& x){const bool ok=x.recovery55c1==recovery&&
        kcd2vr::contact176::fresh(x.tick,now,handoff59c3::maximumContactAge);
        if(!ok)RecoveryRetired65h(x,recovery,now,kcd2vr::recovery65h::Site::SubmitPrune);return ok;};
    const auto pruned57=g_backlog59c6.prune(usable);
    g_backlogExpired59c6.fetch_add(pruned57,std::memory_order_relaxed);
    if(!g_draining59c3&&g_has&&!usable(g_pending)){
        g_drop_waiting_expired56.fetch_add(1,std::memory_order_relaxed);
        g_has=false;++g_backlogExpired59c6;}
    // No new formatting/trace queue work while the gameplay queue lock is held.
    if(!g_draining59c3&&!g_has)g_has=g_backlog59c6.pop(g_pending);
    Pending copy=p;copy.action=nullptr;copy.lease47c2={};copy.release51={};
    copy.recovery55c1=recovery;copy.queued=now;
    const auto same=kcd2vr::contact_backlog59c6::sameContact<Pending>;
    if((g_has&&same(g_pending,copy))||g_backlog59c6.contains(copy,same)){
        queueTrace.why="EXACT_CONTACT_ALREADY_QUEUED";
        ++g_coalesced59c3;return false; // duplicate, never refreshed or overwritten
    }
    if(++g_serial==0)++g_serial;copy.serial=g_serial;
    queueTrace.serial=copy.serial;
    if(!g_draining59c3&&!g_has){queueTrace.why="QUEUED_PRIMARY";queueTrace.queued=true;g_pending=copy;g_has=true;return true;}
    if(!g_backlog59c6.push(copy)){queueTrace.why="CONTACT_BACKLOG_FULL";++g_backlogFull59c6;++g_stats[5];return false;}
    queueTrace.why="QUEUED_BACKLOG";queueTrace.queued=true;++g_backlogQueued59c6;return true;
}
void Clear() noexcept {kcd2vr::hybrid_serial59::releases.clear();kcd2vr::motion59c4::hybridAttempts54.clear();kcd2vr::combo_lineage19::grace.clear();victim5916c3::Clear();vrstate596::Reset();std::lock_guard<std::mutex> lock(g_mutex);g_has=false;g_pending={};g_backlog59c6.clear();g_henry_seen178r31.store(0);g_henry_in_combat178r31.store(-1);g_native_owned_until178r33.store(0);g_tg_phase178r34.store(0);} // V66.178R30 (F03): a mode change drops queued work; R33: and the combat cache / owner latch

template<class Action> void DrainBooking59c6(Action action) noexcept {
    Pending p{};int counterOrigin36=-1;{std::lock_guard<std::mutex> lock(g_mutex);
        if(g_draining59c3)return;
        if(!g_has)g_has=g_backlog59c6.pop(g_pending);
        if(!g_has)return;p=g_pending;g_draining59c3=true;}
    // Reserve the slot across the native callback. A producer/reentrant drain
    // cannot overwrite or deliver a contact while its first consumer is active.
    struct DrainLease59c3 {
        std::uint32_t serial;bool keep=false;
        ~DrainLease59c3(){
            // C56 review (Claude): a waiting contact that expires here used to vanish without a receipt
            // (12-log analysis rank 3b). Collect under the lock, write after it.
            Pending expired{};bool expiredNow=false;std::size_t pruned=0;std::uint64_t now=0,recovery=0;
            {std::lock_guard<std::mutex> lock(g_mutex);
            if(!keep&&g_has&&g_pending.serial==serial)g_has=false;g_draining59c3=false;
            now=std::uint64_t(GetTickCount64());
            recovery=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
            const auto usable=[&](const Pending& x){return x.recovery55c1==recovery&&kcd2vr::contact176::fresh(x.tick,now,handoff59c3::maximumContactAge);};
            const auto prune65h=[&](const Pending& x){const bool ok=usable(x);if(!ok)RecoveryRetired65h(x,recovery,now,kcd2vr::recovery65h::Site::DrainPrune);return ok;};
            pruned=g_backlog59c6.prune(prune65h);g_backlogExpired59c6.fetch_add(pruned,std::memory_order_relaxed);
            if(g_has&&!usable(g_pending)){RecoveryRetired65h(g_pending,recovery,now,kcd2vr::recovery65h::Site::Waiting);expired=g_pending;expiredNow=true;g_has=false;++g_backlogExpired59c6;}
            if(!g_has)g_has=g_backlog59c6.pop(g_pending);}
            if(expiredNow){g_drop_waiting_expired56.fetch_add(1,std::memory_order_relaxed);
                if(Allow(4,4)){char line[280]{};std::snprintf(line,sizeof(line),"C56 GEOM_HIT serial=%u rejected=%s site=DRAIN_LEASE gesture=%llu release=%llu ageMs=%llu waitedMs=%llu boundAction=%p",
                    expired.serial,expired.recovery55c1!=recovery?"RECOVERY_EPOCH_CHANGED_WHILE_WAITING":"CONTACT_EXPIRED_WHILE_WAITING(160ms since physical impact)",
                    (unsigned long long)expired.gesture59c3,(unsigned long long)expired.release51.serial,
                    (unsigned long long)(now>=expired.tick?now-expired.tick:0),(unsigned long long)(now>=expired.queued?now-expired.queued:0),expired.action);diagnostic_trace::Write(line);}}
            if(pruned&&Allow(4,4)){char line[200]{};std::snprintf(line,sizeof(line),"C56 GEOM_HIT rejected=BACKLOG_EXPIRED_OR_RECOVERED count=%u site=DRAIN_LEASE",unsigned(pruned));diagnostic_trace::Write(line);}}
        void wait(const Pending& p){std::lock_guard<std::mutex> lock(g_mutex);
            if(g_has&&g_pending.serial==serial){g_pending=p;keep=true;}}
        void defer(const Pending& p){std::lock_guard<std::mutex> lock(g_mutex);
            if(!g_has||g_pending.serial!=serial)return;
            Pending next{};
            if(g_backlog59c6.pop(next)){
                // Popping made room: keep this exact old contact at the tail,
                // without renewing tick/queued or blocking another recipient.
                if(!g_backlog59c6.push(p))++g_stats[5];
                g_pending=next;
            }else g_pending=p;
            keep=true;
        }
    } pendingLease{p.serial};
    action(p,pendingLease);
}
