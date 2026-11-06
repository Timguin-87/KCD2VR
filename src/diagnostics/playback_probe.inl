// Included once inside animation_filter, after its validated name/identity helpers.
// Native callbacks only. No animation starts, seeks, root motion, health or physics writes.
namespace {
struct DiagnosticStream6690 {
    void* anim=nullptr; std::uint32_t entity=0;std::uint64_t relation=0,expiry=0,epoch=0;
    std::uint64_t lastSample=0,lastEmit=0,lastError=0,lastContact=0,emitThrottled=0,combatSession=0,handsSession28=0,handsAttemptSession28=0;
    int lifecycleId=-1;std::uint8_t lifecycleSegment=0xFF;std::uint64_t lifecycleTick=0;
    diagnostics::playback::Snapshot previous{};
    bool have=false;
    struct Name {int id=-1; char value[160]{};};
    std::array<Name,256> names{};std::size_t nextName=0;
};
static std::mutex g_playback_mutex6690;
static std::atomic<void*> g_selected_stream6694[3]{};
static DiagnosticStream6690 g_streams6690[3]{}; // Henry / requested action target / contact-associated entity
static std::atomic<std::uint64_t> g_playback_serial6690{0},g_contact_seq6690{0};
static std::atomic<std::uint64_t> g_stamp_seq6690{0},g_stamp_tick6690{0},g_stamp_serial6690{0},g_stamp_epoch6690{0};
static std::atomic<std::uint64_t> g_playback_contention6690{0};
static std::atomic<std::uint64_t> g_reload_diag_until136r3{0};
static std::atomic<int> g_reload_diag_weapon136r3{0}; // 1 bow / 2 crossbow / 3 rifle
static int ReloadWeaponCode136R3(const char* weapon) noexcept {
    if(!weapon)return 0;
    if(std::strcmp(weapon,"BOW")==0)return 1;
    if(std::strcmp(weapon,"CROSSBOW")==0)return 2;
    if(std::strcmp(weapon,"RIFLE")==0)return 3;
    return 0;
}
static const char* ReloadWeaponName136R3(int code) noexcept {
    return code==1?"BOW":code==2?"CROSSBOW":code==3?"RIFLE":"UNKNOWN";
}
static void ArmReloadDiagnostic136R3(const char* weapon,std::uint64_t now) noexcept {
    const int code=ReloadWeaponCode136R3(weapon);if(!code)return;
    g_reload_diag_weapon136r3.store(code,std::memory_order_release);
    g_reload_diag_until136r3.store(now+1800ull,std::memory_order_release);
}
static void ObserveReloadMilestones136R3(const char* name,const diagnostics::playback::Entry& e,
                                         const diagnostics::playback::Entry* old,std::uint64_t now,
                                         std::uint64_t serial) noexcept {
    namespace rm=kcd2vr::diagnostics::reload136r3;
    if(!name||!e.activated)return;
    for(std::size_t i=0;i<rm::kMilestoneCount;++i){
        const auto& m=rm::kMilestones[i];if(!rm::NameMatches(name,m.stem))continue;
        const float t=m.t;const float cur=e.segmentTime;
        bool crossed=false;const char* relation=nullptr;
        if(!old||!old->activated||old->segment!=e.segment){
            if(cur+1e-5f>=t){crossed=true;relation="FIRST_SAMPLE_AT_OR_AFTER";}
        }else if(cur+1e-5f>=t&&old->segmentTime+1e-5f<t){
            crossed=true;relation="CROSSED";
        }
        if(!crossed)continue;
        ArmReloadDiagnostic136R3(m.weapon,now);
        char msg[768]{};std::snprintf(msg,sizeof(msg),
            "V66.175-F11-RELOAD RELOAD_MILESTONE seq=%llu weapon=%s event=%s assetT=%.6f observedT=%.6f relation=%s id=%d layer=%u segment=%u alias=\"%s\" bone=\"%s\" source=ANIMEVENT_TIMELINE handlerCall=UNPROVEN ammoWrite=NONE",
            (unsigned long long)serial,m.weapon,m.event,double(t),double(cur),relation,e.id,e.layer,e.segment,name,m.bone?m.bone:"");
        diagnostic_trace::Write(msg);
    }
}
static const char* WeaponLifecycleClass135(const char* name) noexcept {
    if(!name)return nullptr;if(std::strstr(name,"crossbow")||std::strstr(name,"xbow"))return "CROSSBOW";
    if(std::strstr(name,"rifle"))return "RIFLE";
    if(std::strstr(name,"bow")||std::strstr(name,"arrow"))return "BOW";return nullptr;
}
static const char* WeaponLifecyclePhase135(const char* name) noexcept {
    if(!name)return nullptr;
    if(std::strstr(name,"unload"))return "UNLOAD";
    if(std::strstr(name,"holster"))return "HOLSTER";
    if(std::strstr(name,"charging_phase1")||std::strstr(name,"charge_phase1"))return "LOAD_OR_COCK_PHASE1";
    if(std::strstr(name,"charging_phase2")||std::strstr(name,"charge_phase2"))return "LOAD_OR_COCK_PHASE2";
    if(std::strstr(name,"cock"))return "COCK";
    if(std::strstr(name,"loaded"))return "LOADED";
    if(std::strstr(name,"reload")||std::strstr(name,"loading")||std::strstr(name,"_load"))return "LOAD";
    if(std::strstr(name,"powder"))return "POWDER";
    if(std::strstr(name,"goatsfoot"))return "GOATSFOOT";
    if(std::strstr(name,"lever"))return "LEVER";
    if(std::strstr(name,"charge"))return "CHARGE";
    if(std::strstr(name,"release")||std::strstr(name,"shot")||std::strstr(name,"fire"))return "FIRE_RELEASE";
    if(std::strstr(name,"draw"))return "DRAW";
    return nullptr;
}
static bool PlaybackRead6690(std::uintptr_t p,void* out,std::size_t n) noexcept {
    if(!Readable(reinterpret_cast<void*>(p),n))return false;
    __try {std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void RegisterStream6690(unsigned slot,void* anim,std::uint32_t entity,std::uint64_t relation) noexcept {
    if(slot==0||slot>2)return;
    std::unique_lock<std::mutex> lock(g_playback_mutex6690,std::try_to_lock);
    if(!lock.owns_lock()){g_playback_contention6690.fetch_add(1);return;}
    auto&s=g_streams6690[slot];const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(s.anim!=anim||s.entity!=entity||s.epoch!=(diagnostics::lab::Snapshot()>>4)){s={};s.anim=anim;s.entity=entity;}
    s.epoch=diagnostics::lab::Snapshot()>>4;
    s.relation=relation;s.expiry=now+(slot==1?10000ull:2500ull);
    g_selected_stream6694[slot].store(anim,std::memory_order_release);
}
static void SamplePlayback6690(void* anim,bool henry) noexcept {
    if(!anim||!g_playback_layout_ready6690.load(std::memory_order_acquire)||
       !(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64())||
         diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64())||
         diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,GetTickCount64())||
         (henry&&diagnostics::capture168::active(diagnostics::capture168::Domain::HandReference,GetTickCount64()))))return;
    if(!henry&&anim!=g_selected_stream6694[1].load(std::memory_order_acquire)&&anim!=g_selected_stream6694[2].load(std::memory_order_acquire))return;
    std::unique_lock<std::mutex> lock(g_playback_mutex6690,std::try_to_lock);
    if(!lock.owns_lock()){g_playback_contention6690.fetch_add(1);return;}
    const auto now=static_cast<std::uint64_t>(GetTickCount64());const auto epoch=diagnostics::lab::Snapshot()>>4;
    unsigned slot=3;
    if(henry)slot=0;
    else for(unsigned i=1;i<3;++i){auto&s=g_streams6690[i];
        if(s.epoch==epoch && s.anim==anim && now<s.expiry && (i==1?diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now):diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))){slot=i;break;}}
    if(slot==3)return;
    auto&s=g_streams6690[slot];
    if(s.anim!=anim||s.epoch!=epoch){const auto entity=s.entity;const auto rel=s.relation;const auto expiry=s.expiry;s={};s.anim=anim;s.epoch=epoch;s.entity=entity;s.relation=rel;s.expiry=expiry;}
    const bool detail=diagnostics::lab::AnyReference();
    const bool interaction=diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now);
    const bool otherCapture=diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)||diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now);
    const bool hands28=henry&&diagnostics::capture168::active(diagnostics::capture168::Domain::HandReference,now);
    const auto handsSession28=hands28?diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::HandReference)].load(std::memory_order_acquire):0;
    if(handsSession28!=s.handsAttemptSession28){s.lastSample=0;s.handsAttemptSession28=handsSession28;}
    const std::uint64_t samplePeriod=hands28||(interaction&&henry)?100:detail?100:otherCapture?250:500;
    const std::uint64_t emitPeriod=interaction||detail?500:1000;
    const auto interactionSession=interaction?diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load():0;
    if(s.lastSample&&now>=s.lastSample&&now-s.lastSample<samplePeriod)return;
    s.lastSample=now;
    const auto sampleCombatSession23=diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)?
        diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load(std::memory_order_acquire):0;
    diagnostics::playback::Snapshot snap{};
    const auto result=diagnostics::playback::Capture(reinterpret_cast<std::uintptr_t>(anim),PlaybackRead6690,snap);
    const char* role=slot==0?"HENRY":slot==1?"REQUEST_TARGET":"CONTACT_ENTITY";
    if(result!=diagnostics::playback::Result::Ok){s.have=false;
        if(henry){g_stamp_seq6690.fetch_add(1);g_stamp_tick6690.store(0);g_stamp_serial6690.store(0);g_stamp_epoch6690.store(0);g_stamp_seq6690.fetch_add(1,std::memory_order_release);}
        if(!s.lastError||now-s.lastError>=1000){s.lastError=now;char msg[384]{};
            std::snprintf(msg,sizeof(msg),"V66.90-PLAYBACK GAP tick=%llu epoch=%llu role=%s entity=%u reason=%s failureLayer=%d failureEntry=%d failureField=%d fieldLegend=1_layer/2_id/3_nonfinite_time/4_time_range priorSnapshotInvalidated=1",(unsigned long long)now,(unsigned long long)epoch,role,s.entity,diagnostics::playback::ResultName(result),snap.failureLayer,snap.failureEntry,snap.failureField);diagnostic_trace::Write(msg);}return;}
    snap.tick=now;snap.serial=g_playback_serial6690.fetch_add(1)+1;
    // V66.135 F3-only weapon lifecycle trace. This observes the real Henry FIFO;
    // it does not infer completion from a request and performs no reload/ammo writes.
    if(henry&&diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now)){
        for(std::size_t i=0;i<snap.count;++i){const auto&e=snap.entries[i];if(!e.activated)continue;
            const char* name=nullptr;for(auto&n:s.names)if(n.id==e.id&&n.value[0]!='<'){name=n.value;break;}
            if(!name){auto&n=s.names[s.nextName++%s.names.size()];n={};n.id=e.id;
                if(!NameForId6689(anim,e.id,n.value,sizeof(n.value)))std::snprintf(n.value,sizeof(n.value),"<UNRESOLVED_ID_%d>",e.id);name=n.value;}
            const char* weapon=WeaponLifecycleClass135(name);const char* phase=WeaponLifecyclePhase135(name);
            const auto* old=s.have?diagnostics::playback::Find(s.previous,e):nullptr;
            if(weapon)ObserveReloadMilestones136R3(name,e,old,now,snap.serial);
            if(!weapon||!phase)continue;
            ArmReloadDiagnostic136R3(weapon,now);
            const bool changed=s.lifecycleId!=e.id||s.lifecycleSegment!=e.segment;
            if(changed||!s.lifecycleTick||now-s.lifecycleTick>=1000){s.lifecycleId=e.id;s.lifecycleSegment=e.segment;s.lifecycleTick=now;
                char life[640]{};std::snprintf(life,sizeof(life),
                    "V66.175-F11-RELOAD WEAPON_LIFECYCLE weapon=%s phase=%s id=%d layer=%u segment=%u segmentTime=%.4f alias=\"%s\" completion=NATIVE_STATE_OR_FIFO_ACK_REQUIRED writes=NONE",
                    weapon,phase,e.id,e.layer,e.segment,double(e.segmentTime),name);diagnostic_trace::Write(life);}
        }
    }
    if(henry){g_stamp_seq6690.fetch_add(1,std::memory_order_acq_rel);g_stamp_tick6690.store(now,std::memory_order_relaxed);g_stamp_serial6690.store(snap.serial,std::memory_order_relaxed);g_stamp_epoch6690.store(epoch,std::memory_order_relaxed);g_stamp_seq6690.fetch_add(1,std::memory_order_release);}
    const auto contact=g_contact_seq6690.load(std::memory_order_acquire);
    const bool change=!s.have||!diagnostics::playback::SameMembership(s.previous,snap);
    const bool eventSample=contact!=s.lastContact;
    const bool needed=change||eventSample||!s.lastEmit||now-s.lastEmit>=emitPeriod;
    // F5 Henry membership edges may emit at10Hz; unchanged snapshots stay2Hz. Other streams retain their existing cadence.
    // Samples run at <=10Hz/reference or <=4Hz/live, never as a
    // claim of every frame/event or native queue locking.
    const bool emit=(interaction||otherCapture||detail)&&needed&&(!s.lastEmit||now-s.lastEmit>=((interaction&&henry&&change)?100:emitPeriod));
    if(needed&&!emit)++s.emitThrottled;
    if(emit){
        std::size_t active=0;for(std::size_t i=0;i<snap.count;++i)if(snap.entries[i].activated)++active;
        if(interaction&&henry){auto& w=activity_input163::witness175;
            const auto a=w.active[unsigned(activity_input163::Kind175::Alchemy)].load(),b=w.active[unsigned(activity_input163::Kind175::Smithing)].load();
            if(a||b){char line[512]{};std::snprintf(line,sizeof(line),"C71 F5_CRAFT_ANIMATION session=%llu tick=%llu instance=%llX life=%llu playbackSeq=%llu anim=%p entries=%zu phase=%d alchemyStage=%s source=EXISTING_NATIVE_FIFO exactBlendWeights=UNKNOWN",
                (unsigned long long)interactionSession,(unsigned long long)now,(unsigned long long)(a?a:b),(unsigned long long)w.lifetime175.load(),(unsigned long long)snap.serial,anim,snap.count,
                a?-1:w.SmithPhaseC1(),a?kcd2vr::alchemy_input19c4::currentStage37(a,now):"-");diagnostic_trace::Write(line);}}

        char msg[704]{};std::snprintf(msg,sizeof(msg),"V66.90-PLAYBACK SNAP tick=%llu epoch=%llu seq=%llu channels=%u F5session=%llu role=%s entity=%u relation=%llu contactSeq=%llu reason=%s entries=%zu activated=%zu deltaMs=%llu aliasSource=NATIVE_DICTIONARY_OR_EXACT_ADB_HASH coherentNativeLock=UNPROVEN clipWeights=UNKNOWN actionPhase=UNKNOWN dropContention=%llu throttledSnapshots=%llu F12RestorePending=%d",
            (unsigned long long)now,(unsigned long long)epoch,(unsigned long long)snap.serial,diagnostics::lab::Mask(),(unsigned long long)interactionSession,role,s.entity,(unsigned long long)s.relation,(unsigned long long)contact,change?"MEMBERSHIP":eventSample?"AFTER_CONTACT":"HEARTBEAT",snap.count,active,(unsigned long long)(s.have?now-s.previous.tick:0),(unsigned long long)g_playback_contention6690.load(),(unsigned long long)s.emitThrottled,diagnostics::lab::weaponRestorePending.load());diagnostic_trace::Write(msg);
        std::size_t emitted=0;
        // Active entries first; explicit truncation if the bounded record budget is exhausted.
        for(int pass=0;pass<2;++pass)for(std::size_t i=0;i<snap.count;++i){const auto&e=snap.entries[i];if(e.activated!=(pass==0))continue;if(emitted>=(detail?32u:16u))continue;
            const char* name=nullptr;
            for(auto&n:s.names)if(n.id==e.id&&n.value[0]!='<'){name=n.value;break;}
            if(!name){auto&n=s.names[s.nextName++%s.names.size()];n={};n.id=e.id;
                if(!NameForId6689(anim,e.id,n.value,sizeof(n.value)))std::snprintf(n.value,sizeof(n.value),"<UNRESOLVED_ID_%d>",e.id);
                for(char*p=n.value;*p;++p)if(*p=='\n'||*p=='\r'||*p=='\t'||*p=='\"')*p='_';name=n.value;}
            const auto*old=s.have?diagnostics::playback::Find(s.previous,e):nullptr;
            char line[1152]{};std::snprintf(line,sizeof(line),"V66.90-PLAYBACK CLIP tick=%llu epoch=%llu seq=%llu role=%s entity=%u relation=%llu layer=%u fifo=%u id=%d activated=%d segmentIndexByte=%u segmentTime=%.6f progression=%s layerField5C=%.5f clipWeight=UNKNOWN parametricSampler=%d flags=0x%08X dynamic=0x%08X raw18_34=%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X name=\"%s\"",
                (unsigned long long)now,(unsigned long long)epoch,(unsigned long long)snap.serial,role,s.entity,(unsigned long long)s.relation,e.layer,e.index,e.id,e.activated,e.segment,double(e.segmentTime),diagnostics::playback::PhaseRelation(e,old),double(e.layerWeight),e.sampler,e.flags,e.dynamicFlags,e.raw18To34[0],e.raw18To34[1],e.raw18To34[2],e.raw18To34[3],e.raw18To34[4],e.raw18To34[5],e.raw18To34[6],e.raw18To34[7],name);
            diagnostic_trace::Write(line);++emitted;
        }
        if(emitted<snap.count){char line[224]{};std::snprintf(line,sizeof(line),"V66.90-PLAYBACK TRUNCATED seq=%llu emitted=%zu total=%zu interpretation=INCOMPLETE",(unsigned long long)snap.serial,emitted,snap.count);diagnostic_trace::Write(line);}
        s.lastEmit=now;s.lastContact=contact;
    }
    if(hands28){unsigned resolved28=0;
        for(std::size_t i=0;i<snap.count&&resolved28<12;++i){const auto& e=snap.entries[i];if(!e.activated)continue;++resolved28;
            bool known=false;for(const auto& n:s.names)if(n.id==e.id&&n.value[0])known=true;
            if(!known){auto& n=s.names[s.nextName++%s.names.size()];n={};n.id=e.id;
                if(!NameForId6689(anim,e.id,n.value,sizeof(n.value)))std::snprintf(n.value,sizeof(n.value),"<UNRESOLVED_ID_%d>",e.id);
                for(char* p=n.value;*p;++p)if(*p=='\n'||*p=='\r'||*p=='\t'||*p=='\"')*p='_';}
        }
    }
    s.previous=snap;s.have=true;
    s.handsSession28=handsSession28&&diagnostics::capture168::active(diagnostics::capture168::Domain::HandReference,std::uint64_t(GetTickCount64()))&&
        handsSession28==diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::HandReference)].load(std::memory_order_acquire)?handsSession28:0;
    s.combatSession=sampleCombatSession23&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,std::uint64_t(GetTickCount64()))&&
        sampleCombatSession23==diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load(std::memory_order_acquire)?sampleCombatSession23:0;
}
}
namespace {
static void SampleKickPlayback101(void* anim) noexcept {
    namespace cap=diagnostics::combat_capture;
    const auto now=std::uint64_t(GetTickCount64());
    if(!cap::active(now)||anim!=g_player_skeleton_anim.load()||!g_playback_layout_ready6690.load())return;
    static std::mutex mutex;std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);if(!lock.owns_lock()){cap::dropped.fetch_add(1);return;}
    static std::uint64_t last=0,epoch=0;static void* previous=nullptr;
    static std::array<DiagnosticStream6690::Name,64> names{};static unsigned next=0;
    if(previous!=anim||epoch!=cap::session.load()){last=0;names={};next=0;epoch=cap::session.load();previous=anim;}
    if(last&&now-last<100)return;last=now;
    diagnostics::playback::Snapshot snapshot{};
    const auto status=diagnostics::playback::Capture(reinterpret_cast<std::uintptr_t>(anim),PlaybackRead6690,snapshot,1);
    if(status!=diagnostics::playback::Result::Ok){cap::Event e{};e.kind=cap::Kind::State;e.tick=now;e.id=int(status);cap::name(e,"KICK_FIFO_GAP");cap::push(e);return;}
    for(std::size_t i=0;i<snapshot.count;++i){const auto& row=snapshot.entries[i];if(!row.activated)continue;
        cap::Event e{};e.kind=cap::Kind::Clip;e.tick=now;e.actor=reinterpret_cast<std::uintptr_t>(anim);e.id=row.id;e.layer=row.layer;e.phase=row.segmentTime;e.flags=4|(row.sampler?8:0);
        const char* label=nullptr;for(const auto& n:names)if(n.id==row.id&&n.value[0]&&n.value[0]!='<'){label=n.value;break;}
        if(!label){auto& n=names[next++%names.size()];n={};n.id=row.id;if(!NameForId6689(anim,row.id,n.value,sizeof(n.value)))std::snprintf(n.value,sizeof(n.value),"<UNRESOLVED_ID_%d>",row.id);label=n.value;}
        cap::name(e,label);cap::push(e);
    }
}
}
void ObserveDiagnosticSkinning(void* anim,bool isHenry) noexcept {if(isHenry)SampleKickPlayback101(anim);SamplePlayback6690(anim,isHenry);}
void RegisterContactAnimation(void* anim,std::uint32_t id,std::uint64_t seq) noexcept {RegisterStream6690(2,anim,id,seq);}
void MarkContactForPlayback(std::uint64_t sequence) noexcept {g_contact_seq6690.store(sequence,std::memory_order_release);}
PlaybackStamp DiagnosticPlaybackStamp() noexcept {
    for(int i=0;i<3;++i){const auto q=g_stamp_seq6690.load(std::memory_order_acquire);if(q&1)continue;
        PlaybackStamp s{g_stamp_serial6690.load(std::memory_order_relaxed),g_stamp_tick6690.load(std::memory_order_relaxed)};
        const auto epoch=g_stamp_epoch6690.load(std::memory_order_relaxed);
        if(q==g_stamp_seq6690.load(std::memory_order_acquire))return epoch==(diagnostics::lab::Snapshot()>>4)?s:PlaybackStamp{};}
    return {};
}
bool ReadKickPlaybackOutput23(void* anim,KickPlaybackOutput23& out) noexcept {
    out={};const auto now=std::uint64_t(GetTickCount64());
    namespace cap=diagnostics::capture168;
    if(!cap::active(cap::Domain::Combat,now)||!anim||anim!=g_player_skeleton_anim.load(std::memory_order_acquire))return false;
    const auto session=cap::gate.session[unsigned(cap::Domain::Combat)].load(std::memory_order_acquire);
    std::unique_lock<std::mutex> lock(g_playback_mutex6690,std::try_to_lock);
    if(!lock.owns_lock())return false;
    const auto& s=g_streams6690[0];
    if(!s.have||s.anim!=anim||s.epoch!=(diagnostics::lab::Snapshot()>>4)||s.combatSession!=session||
       !s.previous.tick||now<s.previous.tick||now-s.previous.tick>500)return false;
    for(std::size_t i=0;i<s.previous.count;++i){const auto& e=s.previous.entries[i];
        if(!e.activated)continue;
        for(const auto& n:s.names)if(n.id==e.id&&kcd2vr::vr_combat111::NativeKickAlias175(n.value)){
            out.tick=s.previous.tick;out.serial=s.previous.serial;out.id=e.id;out.layer=e.layer;
            out.fifo=e.index;out.segment=e.segment;out.segmentTime=e.segmentTime;
            std::memcpy(out.name,n.value,sizeof(out.name));out.name[sizeof(out.name)-1]=0;
            if(cap::active(cap::Domain::Combat,std::uint64_t(GetTickCount64()))&&session==cap::gate.session[unsigned(cap::Domain::Combat)].load(std::memory_order_acquire))return true;
            out={};return false;
        }
    }
    return false;
}
bool ReadHandPlaybackOutput28(void* anim,HandPlaybackOutput28& out) noexcept {
    out={};namespace cap=diagnostics::capture168;const auto now=std::uint64_t(GetTickCount64());
    if(!cap::active(cap::Domain::HandReference,now)||!anim||anim!=g_player_skeleton_anim.load(std::memory_order_acquire))return false;
    const auto session=cap::gate.session[unsigned(cap::Domain::HandReference)].load(std::memory_order_acquire);
    std::unique_lock<std::mutex> guard(g_playback_mutex6690,std::try_to_lock);if(!guard.owns_lock())return false;
    const auto& s=g_streams6690[0];if(!s.have||s.anim!=anim||s.handsSession28!=session||s.epoch!=(diagnostics::lab::Snapshot()>>4)||
        !s.previous.tick||now<s.previous.tick||now-s.previous.tick>200)return false;
    out.tick=s.previous.tick;out.serial=s.previous.serial;out.signature=14695981039346656037ull;
    for(std::size_t i=0;i<s.previous.count;++i){const auto& e=s.previous.entries[i];if(!e.activated)continue;
        out.signature^=std::uint64_t(std::uint32_t(e.id))+std::uint64_t(e.layer)*65537+std::uint64_t(e.segment)*1048576+std::uint64_t(std::clamp(e.segmentTime,0.f,1.f)*10);out.signature*=1099511628211ull;
        if(out.count>=out.clips.size()){out.truncated=true;continue;}auto& v=out.clips[out.count++];
        v.tick=out.tick;v.serial=out.serial;v.id=e.id;v.layer=e.layer;v.fifo=e.index;v.segment=e.segment;v.segmentTime=e.segmentTime;
        for(const auto& n:s.names)if(n.id==e.id){std::memcpy(v.name,n.value,sizeof(v.name));v.name[sizeof(v.name)-1]=0;break;}
    }
    if(cap::active(cap::Domain::HandReference,std::uint64_t(GetTickCount64()))&&session==cap::gate.session[unsigned(cap::Domain::HandReference)].load(std::memory_order_acquire))return true;
    out={};return false;
}
bool ReloadDiagnosticWindow136R3(std::uint64_t now,int& weapon_code) noexcept {
    const auto until=g_reload_diag_until136r3.load(std::memory_order_acquire);
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now)||!until||now>until){weapon_code=0;return false;}
    weapon_code=g_reload_diag_weapon136r3.load(std::memory_order_acquire);
    return weapon_code>=1&&weapon_code<=3;
}
const char* ReloadDiagnosticWeaponName136R3(int weapon_code) noexcept {
    return ReloadWeaponName136R3(weapon_code);
}
