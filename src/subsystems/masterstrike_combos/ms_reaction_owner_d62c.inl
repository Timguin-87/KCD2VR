// D62C: paired attack ownership + timing. Included by action_time_owner59c11.cpp inside its anonymous namespace, after
// FragmentCrc19c4 and before QueueDetour. Uses only that file's pinned Mannequin layout (kCtrl*, kAct*) and the
// semi_smith46::native::membership walk. Every speed write happens in the Update of the controller that owns the action,
// after a membership walk of that same Update proved the action is the root of a scope of that controller.
namespace msr62c=kcd2vr::ms_reaction_d62c;
namespace sig62c=kcd2vr::paired_signals_d62c;
static msr62c::Table<12> g_ms_table_d62c;
static std::atomic_flag g_ms_lock_d62c=ATOMIC_FLAG_INIT;
static std::atomic<bool> g_ms_live_d62c{false};
static std::atomic<std::uint64_t> g_ms_busy_d62c{0},g_ms_accels_d62c{0},g_ms_hits_d62c{0};
struct MsFragIdsD62C {std::uintptr_t definition=0;int id[8]{-1,-1,-1,-1,-1,-1,-1,-1};};
static MsFragIdsD62C g_ms_ids_d62c[8]{};static unsigned g_ms_ids_next_d62c=0;
static bool MsFragCrcD62C(void* ctrl,int fragId,std::uint32_t& crc) noexcept {
    const void* context=Rd<void*>(ctrl,kCtrlContext,nullptr);const auto definition=reinterpret_cast<std::uintptr_t>(Rd<void*>(context,0,nullptr));
    if(!definition||fragId<0)return false;
    for(auto& c:g_ms_ids_d62c)if(c.definition==definition){
        for(int k=0;k<8;++k)if(c.id[k]==fragId){crc=msr62c::kCrcs[k];return true;}
        return false;
    }
    MsFragIdsD62C fresh{};fresh.definition=definition;
    const void* fragments=Rd<void*>(reinterpret_cast<void*>(definition),0x18,nullptr);
    const auto* entries=Rd<const unsigned char*>(fragments,8,nullptr);const int count=Rd<int>(entries,-4,-1);
    if(!entries||count<1||count>65536)return false;
    for(int i=0;i<count;++i){std::uint32_t v=0;if(!SehCopy(entries+std::size_t(i)*0x20+0x10,&v,sizeof(v)))return false;
        for(int k=0;k<8;++k)if(v==msr62c::kCrcs[k])fresh.id[k]=i;}
    if(Rd<void*>(ctrl,kCtrlContext,nullptr)!=context||reinterpret_cast<std::uintptr_t>(Rd<void*>(context,0,nullptr))!=definition||
       Rd<void*>(reinterpret_cast<void*>(definition),0x18,nullptr)!=fragments||Rd<const unsigned char*>(fragments,8,nullptr)!=entries)return false;
    g_ms_ids_d62c[g_ms_ids_next_d62c++%8]=fresh;
    for(int k=0;k<8;++k)if(fresh.id[k]==fragId){crc=msr62c::kCrcs[k];return true;}
    return false;
}
struct MsLockD62C {bool held;MsLockD62C():held(!g_ms_lock_d62c.test_and_set(std::memory_order_acquire)){if(!held)g_ms_busy_d62c.fetch_add(1,std::memory_order_relaxed);}
    ~MsLockD62C(){if(held)g_ms_lock_d62c.clear(std::memory_order_release);} explicit operator bool() const noexcept {return held;}};
// D72 byte-closed native pair discovery. Original selection, construction,
// admission and teardown remain native; this adapter reads their result only.
static bool MsPairPinsD72() noexcept {
 static std::atomic<std::uintptr_t> checked{0};const auto b=g_base.load(std::memory_order_acquire);
 if(!b)return false;if(checked.load(std::memory_order_acquire)==b)return true;
 struct Pin{std::uintptr_t at;unsigned n;unsigned char bytes[32];};
 constexpr Pin pins[]{
  {0xD4B489,15,{0x49,0x8b,0x06,0x48,0x85,0xc0,0x74,0x07,0x48,0x89,0xb0,0xc0,0,0,0}},
  {0xD4B403,26,{0x4c,0x8b,0x46,0x60,0x48,0x8d,0x54,0x24,0x50,0xc1,0xe3,0x08,0x4c,0x8d,0xb6,0xa0,0,0,0,0x44,0x8a,0xc8,0x89,0x5c,0x24,0x20}},
  {0xD4BA51,27,{0xba,2,0,0,0,0x48,0x8b,0xcb,0xe8,0x52,0x17,0xbc,0xff,0x48,0x8b,0xce,0x48,0xc7,0x83,0xc0,0,0,0,0,0,0,0}},
  {0x10F30D3,25,{0x48,0x8b,0xd6,0x48,0x8b,0xcf,0xe8,0x72,0x97,0x81,0xff,0x84,0xc0,0x74,0x4f,0x48,0x8b,0x06,0x48,0x89,0x98,0xa0,0,0,0}},
  {0x9CCD78,16,{0x48,0x8b,0x46,0x38,0x49,0x8b,0xd6,0x4a,0x8d,0x0c,0xf8,0xe8,0x9c,0,0,0}},
  {0x90D06B,10,{0x48,0x8d,0x05,0xa6,0x29,0x53,0x03,0x48,0x89,0x03}},
  {0x90D0A3,13,{0x48,0x89,0x4b,0x40,0x48,0xc7,0x47,0x38,0,0,0,0,0x48}},
  {0x90D0AF,13,{0x48,0x8b,0x47,0x40,0x48,0x8b,0x74,0x24,0x38,0x48,0x89,0x43,0x48}}
 };
 if(!retail_build112::Ready(b))return false;
 for(const auto& pin:pins){unsigned char actual[32]{};
  if(!SehCopy(reinterpret_cast<void*>(b+pin.at),actual,pin.n)||std::memcmp(actual,pin.bytes,pin.n))return false;}
 checked.store(b,std::memory_order_release);return true;
}
static bool MsReadPairD72(const msr62c::Owner& r,kcd2vr::native_pair_d72::Pair& pair,kcd2vr::native_pair_d72::ReadTraceD84* trace=nullptr) noexcept {
 auto rd=[](std::uintptr_t a,auto& v) noexcept{return SehCopy(reinterpret_cast<void*>(a),&v,sizeof(v));};
 if(trace){*trace={};trace->gate=kcd2vr::native_pair_d72::ReadGateD84::Pins;}
 if(!MsPairPinsD72())return false;
 std::uint32_t token=0;const bool readable=rd(r.action+kActToken,token);
 if(trace){trace->gate=kcd2vr::native_pair_d72::ReadGateD84::Token;trace->at=r.action+kActToken;
  trace->observed=token;trace->expected=r.token;trace->readable=readable;}
 if(!readable||token!=r.token)return false;
 return kcd2vr::native_pair_d72::read(rd,g_base.load(std::memory_order_acquire),r.action,msr62c::combo(r.kind),msr62c::isAttacker(r.kind),pair,trace);
}
static void MsJoinPairD72(msr62c::Owner& row) noexcept {
 const bool capture=diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64());
 if(capture)row.readD84={};
 kcd2vr::native_pair_d72::Pair proof{};if(!MsReadPairD72(row,proof,capture?&row.readD84:nullptr))return;
 for(auto& a:g_ms_table_d62c.rows)if(a.live()&&a.henry&&msr62c::isAttacker(a.kind)&&a.action==proof.attackFragment&&a.entity==proof.attackEntity)
  for(auto& v:g_ms_table_d62c.rows)if(v.live()&&!v.henry&&v.action==proof.hitFragment&&v.entity==proof.hitEntity&&
   a.controller!=v.controller&&msr62c::pairs(a.kind,v.kind)){
   if((a.pair&&a.pair!=v.serial)||(v.pair&&v.pair!=a.serial))return;
   a.pair=v.serial;v.pair=a.serial;a.victim=v.entity;a.nativeD72=v.nativeD72=proof;a.nativeProofD72=v.nativeProofD72=true;
   return;
  }
}
static void MsPublishLiveD62C() noexcept {
    g_ms_live_d62c.store(g_ms_table_d62c.anyLive(),std::memory_order_release);
    if(!g_ms_table_d62c.henryAttackLive())(void)kcd2vr::paired_hands_d72::witness.publish({});
    // Entity/tick signals remain diagnostics; they do not authorize timing.
    sig62c::live.store(g_ms_table_d62c.henryAttackLive(),std::memory_order_release);
    bool active=false;for(const auto& r:g_ms_table_d62c.rows)active=active||r.leased;
    namespace instant=kcd2vr::instant_d72;
    instant::Publish(instant::Family::Combat,active?(instant::Combat()?instant::State::Active:instant::State::Retiring):
        !instant::Combat()?instant::State::Native:instant::State::Unsupported); // D84 native event/contact ownership is not implemented
}
static void MsTraceD62C(const char* what,const msr62c::Owner& o,std::uint64_t now) noexcept {
    using namespace diagnostics::capture168;if(!active(Domain::Combat,now))return;
    const msr62c::Owner* p=o.pair?g_ms_table_d62c.find(o.pair):nullptr;
    char tags[33]{};for(int i=0;i<16;++i)std::snprintf(tags+2*i,3,"%02X",o.tags[i]);
    char line[1200]{};std::snprintf(line,sizeof(line),
        "D42_F3 MS_PAIR_D62C %s serial=%llu kind=%s state=%s henry=%d entity=%u ctrl=%llX action=%llX token=%u frag=%d option=%d tags=%s "
        "queuedMs=%llu playMs=%llu exitMs=%llu endMs=%llu pair=%llu pairKind=%s pairEntity=%u phase=%s victim=%u contactAgeMs=%lld accelMs=%llu hitAfterAccelMs=%lld end=%s "
        "speed=%.2f saved=%.3f ours=%.3f leased=%d writes=%u restores=%u refused=%u queueCandidateD65=%llu queueCandidatesD65=%u pairProof=%s instantD72=%d "
        "readGateD84=%s readAt=%llX readValue=%llX readExpected=%llX readOK=%d requireNonzero=%d damageTiming=NATIVE_1X contactEventAuthority=UNPROVEN nativeOutcome=NATIVE",
        what,(unsigned long long)o.serial,msr62c::name(o.kind),msr62c::name(o.state),o.henry?1:0,o.entity,(unsigned long long)o.controller,(unsigned long long)o.action,
        o.token,o.fragId,o.option,tags,(unsigned long long)o.queued,(unsigned long long)(o.playing?o.playing-o.queued:0),
        (unsigned long long)(o.exiting?o.exiting-o.queued:0),(unsigned long long)(o.ended?o.ended-o.queued:0),
        (unsigned long long)o.pair,p?msr62c::name(p->kind):"NONE",p?p->entity:0u,msr62c::name(o.phase),o.victim,
        o.contactTick?(long long)(now-o.contactTick):-1LL,(unsigned long long)(o.accelStart?(o.accelEnd?o.accelEnd:now)-o.accelStart:0),
        o.hitTick&&o.accelStart?(long long)(o.hitTick-o.accelStart):-1LL,o.endWhy,double(sig62c::speed.load(std::memory_order_relaxed)),
        double(o.saved),double(o.ours),o.leased?1:0,o.writes,o.restores,o.refused,(unsigned long long)o.candidateD65,o.candidatesD65,o.nativeProofD72?"RECIPROCAL_NATIVE_MAIN":"UNVERIFIED",o.instantD72?1:0,
        kcd2vr::native_pair_d72::name(o.readD84.gate),(unsigned long long)o.readD84.at,(unsigned long long)o.readD84.observed,
        (unsigned long long)o.readD84.expected,o.readD84.readable?1:0,o.readD84.nonzero?1:0);
    diagnostic_trace::Write(line);
}
static void MsReactionQueueD62C(void* ctrl,void* action,bool henry) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!kcd2vr::instant_d72::Combat()&&!g_ms_live_d62c.load(std::memory_order_acquire)&&
       !diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    const auto b=g_base.load(std::memory_order_acquire);
    if(!b||!ctrl||!action||Rd<std::uintptr_t>(ctrl,0,0)!=b+kCtrlVft)return;
    const int fragId=Rd<int>(action,kActFragId,-1);std::uint32_t crc=0;
    MsLockD62C lock;if(!lock)return;
    if(!MsFragCrcD62C(ctrl,fragId,crc))return;
    unsigned char tags[16]{};(void)SehCopy(static_cast<unsigned char*>(action)+kActTags,tags,sizeof(tags));
    const auto newSerial=g_ms_table_d62c.next;
    auto* row=g_ms_table_d62c.offer(reinterpret_cast<std::uintptr_t>(ctrl),reinterpret_cast<std::uintptr_t>(action),
        Rd<std::uint32_t>(ctrl,kCtrlEntityId,0u),Rd<std::uint32_t>(action,kActToken,0u),crc,fragId,Rd<int>(action,kActOption,-1),tags,henry,now);
    if(row){if(row->serial==newSerial)row->instantD72=kcd2vr::instant_d72::Combat();MsJoinPairD72(*row);MsPublishLiveD62C();MsTraceD62C("QUEUE",*row,now);
        // D65: a newly arriving ambiguous row must be visible for the earlier
        // actor too. Finite F3 metadata only; no extra native reads/writes.
        for(const auto& observed:g_ms_table_d62c.rows)if(observed.serial!=row->serial&&observed.live()&&
            (observed.candidateD65==row->serial||observed.candidatesD65>1))MsTraceD62C("QUEUE_CANDIDATE_D65",observed,now);
        if(row->pair)if(const auto* p=g_ms_table_d62c.find(row->pair))MsTraceD62C("PAIRED",*p,now);}
}
// D76 review: an unreadable identity is not a positive replacement. Call only
// after native container membership, so an absent saved action is not followed.
enum class MsLeaseIdentityD76 {Unknown,Current,Replaced};
static MsLeaseIdentityD76 MsReadLeaseIdentityD76(const msr62c::Owner& r,std::uintptr_t b) noexcept {
    auto* ctrl=reinterpret_cast<unsigned char*>(r.controller);
    auto* action=reinterpret_cast<unsigned char*>(r.action);
    std::uint32_t entity=0,token=0;std::uintptr_t vt=0;int fragment=-1;
    if(!SehCopy(ctrl+kCtrlEntityId,&entity,sizeof(entity)))return MsLeaseIdentityD76::Unknown;
    if(entity!=r.entity)return MsLeaseIdentityD76::Replaced;
    if(!SehCopy(action,&vt,sizeof(vt)))return MsLeaseIdentityD76::Unknown;
    if(vt!=b+kcd2vr::native_pair_d72::fragmentVt)return MsLeaseIdentityD76::Replaced;
    if(!SehCopy(action+kActToken,&token,sizeof(token)))return MsLeaseIdentityD76::Unknown;
    if(token!=r.token)return MsLeaseIdentityD76::Replaced;
    if(!SehCopy(action+kActFragId,&fragment,sizeof(fragment)))return MsLeaseIdentityD76::Unknown;
    return fragment==r.fragId?MsLeaseIdentityD76::Current:MsLeaseIdentityD76::Replaced;
}
// Speed bias of THIS controller's action. The caller proved current root/exit
// membership. Recheck that membership and identity before an actual write.
static void MsApplyLeaseD62C(msr62c::Owner& r,float want,std::uint64_t now) noexcept {
    void* action=reinterpret_cast<void*>(r.action);float current=-1.f;
    if(!SehCopy(static_cast<unsigned char*>(action)+kActSpeedBias,&current,sizeof(current))){
        if(r.leased)++r.refused;return; // retain possible native write for cleanup
    }
    auto proposed=r;
    const float write=msr62c::lease(proposed,current,want,r.instantD72?msr62c::instantSpeedD72:8.f);
    if(!std::isfinite(write)){r=proposed;return;}
    auto rd=[](std::uintptr_t a,auto& v) noexcept{return SehCopy(reinterpret_cast<void*>(a),&v,sizeof(v));};
    const auto b=g_base.load(std::memory_order_acquire);
    using M=kcd2vr::semi_smith46::native::Membership;
    const auto member=kcd2vr::semi_smith46::native::membership(rd,b,r.controller,r.action);
    if(member.kind==M::Absent){r.leased=false;r.state=msr62c::State::Lost;r.ended=now;return;}
    if(member.kind!=M::Root&&member.kind!=M::Exiting){++r.refused;return;}
    const auto identity=MsReadLeaseIdentityD76(r,b);
    if(identity==MsLeaseIdentityD76::Replaced){r.leased=false;r.state=msr62c::State::Lost;r.ended=now;return;}
    if(identity!=MsLeaseIdentityD76::Current){++r.refused;return;}
    float beforeWrite=-1.f;
    if(!SehCopy(static_cast<unsigned char*>(action)+kActSpeedBias,&beforeWrite,sizeof(beforeWrite))){++r.refused;return;}
    if(beforeWrite!=current){
        if(r.leased&&beforeWrite!=r.ours)r.leased=false;
        ++r.refused;return; // another writer changed the sampled speed during proof
    }
    const auto restores=r.restores;r=proposed;
    float after=-1.f;
    const bool wrote=Wr<float>(action,kActSpeedBias,write);
    const bool readBack=wrote&&SehCopy(static_cast<unsigned char*>(action)+kActSpeedBias,&after,sizeof(after));
    if(!readBack||after!=write){
        // A failed write/readback may have left our value installed. Preserve
        // that debt until a later known owner/value can confirm or retire it.
        // A positively observed foreign value is never overwritten.
        r.leased=!readBack||after==r.ours;r.restores=restores;++r.refused;
        MsTraceD62C("SPEED_WRITE_REFUSED",r,now);return;
    }
    MsTraceD62C(r.leased?"SPEED_LEASED":"SPEED_RESTORED",r,now);
}
// D72: only the exact accepted Henry pair, its owned scope and sole active CAF
// may retain tracked wrists. Native pair events, torso/root and victim stay native.
static bool MsPublishPairHandsD72(const msr62c::Owner& a,const msr62c::Owner& v,void* ctrl,
    bool proof,bool roots,std::uint64_t now) noexcept {
    namespace hands=kcd2vr::paired_hands_d72;
    if(!proof||!roots||!a.henry||!a.instantD72||a.phase!=msr62c::Phase::Accel||
       a.controller!=reinterpret_cast<std::uintptr_t>(ctrl)||ctrl!=g_henry_ctrl.load()||
       !IsHenryController(ctrl)||a.entity!=HenryEntityId18())return false;
    grasp13::AcquisitionInput55 input{};
    if(!g_acquisition_input55.read(input)||!FreshAcquisitionInput55(input,now)||!input.headTracked)return false;
    const auto contexts=Rd<const void*>(ctrl,kCtrlScopeContexts,nullptr);
    if(!contexts||Rd<std::uintptr_t>(contexts,kScopeContextChar,0)!=input.character)return false;
    auto* action=reinterpret_cast<void*>(a.action);auto* scope=Rd<void*>(action,kActRootScope,nullptr);
    const int layer=Rd<int>(scope,kScopeLayer,-1);
    if(!scope||layer<0||layer>8||Rd<unsigned>(scope,kScopeNumLayers,0)!=1||
       Rd<std::uintptr_t>(scope,kScopeAction,0)!=a.action||Rd<unsigned>(action,kActToken,~a.token)!=a.token)return false;
    auto* anim=reinterpret_cast<void*>(input.character+kCharSkelAnim);
    auto* entries=Rd<unsigned char*>(anim,layer*0x40+0x48,nullptr);
    if(!entries||Rd<unsigned>(entries,-4,0)!=1||!(Rd<unsigned short>(entries,0x50,0)&1))return false;
    const int animation=Rd<int>(entries,0x10,-1);
    // Also rejects any other currently protected native body owner.
    if(!animation_filter::AdmittedNativeClipOnlyD72(anim,reinterpret_cast<std::uintptr_t>(entries),animation))return false;
    hands::Owner out{};out.pair=a.nativeD72;out.controller=a.controller;out.character=input.character;
    out.scope=reinterpret_cast<std::uintptr_t>(scope);out.clip=reinterpret_cast<std::uintptr_t>(entries);
    out.serial=a.serial;out.peerSerial=v.serial;out.epoch=input.epoch;out.tracking=input.trackingEpoch;
    out.sample=input.sample;out.sourceTick=input.tick;out.tick=now;out.token=a.token;out.entity=a.entity;
    out.animation=animation;out.layer=layer;out.hands=(input.tracked[0]?1u:0u)|(input.tracked[1]?2u:0u);
    return hands::current(out,input.character,input.epoch,input.trackingEpoch,now)&&hands::witness.publish(out);
}
static void MsReactionUpdateD62C(void* ctrl) noexcept {
    if(!g_ms_live_d62c.load(std::memory_order_acquire)||!ctrl)return;
    const auto b=g_base.load(std::memory_order_acquire);if(!b)return;
    MsLockD62C lock;if(!lock)return;
    const auto c=reinterpret_cast<std::uintptr_t>(ctrl);if(!g_ms_table_d62c.tracks(c))return;
    const auto now=std::uint64_t(GetTickCount64());
    const bool capture=diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now);
    bool pairHandsPublishedD72=false;
    auto rd=[](std::uintptr_t a,auto& v) noexcept{return SehCopy(reinterpret_cast<void*>(a),&v,sizeof(v));};
    using M=kcd2vr::semi_smith46::native::Membership;
    for(auto& r:g_ms_table_d62c.rows){
        if(r.controller!=c||(!r.live()&&!r.leased))continue;
        const auto m=kcd2vr::semi_smith46::native::membership(rd,b,c,r.action);
        if(m.kind==M::Unknown||m.kind==M::Absent){
            const auto edge=g_ms_table_d62c.observe(r,static_cast<msr62c::Seen>(int(m.kind)),now);
            if(edge.changed)MsTraceD62C("EDGE",r,now);
            if(m.kind==M::Absent)r.leased=false;
            // Unknown cleanup stays in the existing fixed 12-row table. No
            // timeout turns missing evidence into retirement or a new write.
            continue;
        }
        const auto identity=MsReadLeaseIdentityD76(r,b);
        if(identity==MsLeaseIdentityD76::Unknown){
            const auto edge=g_ms_table_d62c.observe(r,msr62c::Seen::Unknown,now);
            if(edge.changed)MsTraceD62C("EDGE",r,now);
            continue;
        }
        if(identity==MsLeaseIdentityD76::Replaced){r.leased=false;r.state=msr62c::State::Lost;r.ended=now;continue;}
        const bool ownsScope=m.kind==M::Root||m.kind==M::Exiting;
        if(!r.live()){
            if(ownsScope)MsApplyLeaseD62C(r,0.f,now);
            continue;
        }
        if(!capture&&!r.instantD72&&!r.pair&&!r.leased){r.state=msr62c::State::Retired;r.ended=now;continue;}
        const auto e=g_ms_table_d62c.observe(r,static_cast<msr62c::Seen>(int(m.kind)),now);
        if(e.changed)MsTraceD62C("EDGE",r,now);
        if(!r.pair)MsJoinPairD72(r);
        auto* a=g_ms_table_d62c.timingOwner(r);
        auto* v=a?g_ms_table_d62c.find(a->pair):nullptr;
        if(!a||!v||!a->instantD72){if(ownsScope&&r.leased)MsApplyLeaseD62C(r,0.f,now);continue;}
        // Both endpoints are sampled anew on this owning controller's Update.
        // The other controller is read only, never written from this callback.
        kcd2vr::native_pair_d72::Pair current{};
        const bool proof=MsReadPairD72(*a,current,capture?&a->readD84:nullptr)&&msr62c::ownsPairD72(*a,*v,current)&&
            Rd<std::uint32_t>(reinterpret_cast<void*>(v->action),kActToken,~v->token)==v->token;
        const auto other=kcd2vr::semi_smith46::native::membership(rd,b,r.serial==a->serial?v->controller:a->controller,
            r.serial==a->serial?v->action:a->action);
        const auto freeSpeed=[](const msr62c::Owner& x) noexcept {
            const float speed=Rd<float>(reinterpret_cast<void*>(x.action),kActSpeedBias,-1.f);
            return !x.refused&&std::isfinite(speed)&&speed>0.f&&
                (x.leased?speed==x.ours:!x.writes&&speed<=8.f);
        };
        const bool roots=m.kind==M::Root&&other.kind==M::Root&&freeSpeed(*a)&&freeSpeed(*v);
        if(a->phase==msr62c::Phase::Idle&&proof&&roots){
            const float as=Rd<float>(reinterpret_cast<void*>(a->action),kActSpeedBias,1.f);
            const float vs=Rd<float>(reinterpret_cast<void*>(v->action),kActSpeedBias,1.f);
            a->speedD72=std::min(msr62c::instantSpeedD72,msr62c::instantSpeedD72/std::max(as,vs));
        }
        if(msr62c::stepInstantD72(*a,v,proof,roots,now))MsTraceD62C("PAIR_CONTACT_AUTHORITY_MISSING_D84",*a,now);
        // The policy belongs to Henry's exact admitted pair, including the
        // victim row which may have been queued before Henry's own row.
        r.instantD72=a->instantD72;
        const float want=proof&&roots&&a->phase==msr62c::Phase::Accel?a->speedD72:0.f;
        if(r.henry)pairHandsPublishedD72=MsPublishPairHandsD72(*a,*v,ctrl,proof,roots,now)||pairHandsPublishedD72;
        if(ownsScope&&(want>0.f||r.leased))MsApplyLeaseD62C(r,want,now);
    }
    if(ctrl==g_henry_ctrl.load()&&!pairHandsPublishedD72)(void)kcd2vr::paired_hands_d72::witness.publish({});
    MsPublishLiveD62C();
}
