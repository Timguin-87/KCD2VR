// F3-only post-hit observations using the EXISTING combat actor update hook.
// No combat setters, damage calls, extra hook, worker dereference or full NPC census.
namespace victim5916c3 {
struct Watch {
    const void* actor=nullptr;const void* entity=nullptr;const void* state=nullptr;
    std::uint64_t started=0,session=0,recovery=0;std::uint32_t id=0,serial=0;
    int combat=-1,lastCombat=-1,phase=-1;unsigned observed=0,emitted=0;bool lethal=false;
};
inline std::array<Watch,8> watches{};inline std::mutex mutex;inline std::atomic<unsigned> count{0};
static std::uint64_t Session(std::uint64_t now) noexcept {
    namespace c=kcd2vr::diagnostics::capture168;
    return c::active(c::Domain::Combat,now)?c::gate.session[unsigned(c::Domain::Combat)].load():0;
}
static void Note(const Watch& w,const char* stage,std::uint64_t now,int combat,int phase) noexcept {
    if(!w.session||Session(now)!=w.session)return;
    char m[400]{};std::snprintf(m,sizeof(m),
        "R59.16C3 NPC_HIT_RESPONSE serial=%u target=%u stage=%s elapsedMs=%llu updates=%u combatAtContinuation=%d observedCombat=%d observedPhase=%d lethalResult=%d consumer=UNOBSERVED nativeStateWrites=NONE",
        w.serial,w.id,stage,(unsigned long long)(now>=w.started?now-w.started:0),w.observed,w.combat,combat,phase,w.lethal?1:0);
    diagnostic_trace::Write(m);
}
static void Clear() noexcept {std::lock_guard<std::mutex> l(mutex);watches={};count=0;}
static void Start(const void* actor,std::uint32_t id,std::uint32_t serial,std::uint64_t now,int atContinuation,bool lethal) noexcept {
    const auto session=Session(now);if(!session)return;
    const void *entity=nullptr,*state=nullptr;
    if(!CombatActor(actor,entity,state,false)||EntityId(entity)!=id)return;
    Watch w{};w.actor=actor;w.entity=entity;w.state=state;w.id=id;w.serial=serial;w.started=now;w.session=session;
    w.recovery=kcd2vr::contact176::recoveryEpoch55c1.load();w.combat=atContinuation;w.lastCombat=atContinuation;w.phase=I(state,0x30);w.lethal=lethal;
    Watch retired{};
    {std::lock_guard<std::mutex> l(mutex);Watch* slot=nullptr;auto* oldest=&watches[0];
     for(auto& x:watches){if(x.actor==actor){slot=&x;break;}if(!x.actor&&!slot)slot=&x;if(x.started<oldest->started)oldest=&x;}
     if(!slot)slot=oldest;retired=*slot;*slot=w;unsigned n=0;for(const auto& x:watches)if(x.actor)++n;count=n;}
    if(retired.actor)Note(retired,"REPLACED_BY_NEW_RECEIPT",now,-1,-1);
    Note(w,"QUEUED_WATCH",now,B(state,kInCombat),w.phase);
}
static void Expire(std::uint64_t now) noexcept {
    if(!count.load(std::memory_order_relaxed))return;
    const auto session=Session(now),recovery=kcd2vr::contact176::recoveryEpoch55c1.load();
    std::array<Watch,8> ended{};unsigned n=0;
    {std::lock_guard<std::mutex> l(mutex);unsigned remaining=0;for(auto& w:watches){
        if(w.actor&&(w.session!=session||w.recovery!=recovery||now<w.started||now-w.started>=1200)){ended[n++]=w;w={};}
        if(w.actor)++remaining;}count=remaining;}
    for(unsigned i=0;i<n;++i)Note(ended[i],ended[i].observed?"WINDOW_END":"NO_MATCHED_ACTOR_UPDATE",now,-1,-1);
}
static void Observe(const void* actor,std::uint64_t now) noexcept {
    if(!count.load(std::memory_order_relaxed))return;
    Watch copy{};{std::unique_lock<std::mutex> l(mutex,std::try_to_lock);if(!l.owns_lock())return;
        for(const auto& w:watches)if(w.actor==actor){copy=w;break;}}
    if(!copy.actor||copy.session!=Session(now)||copy.recovery!=kcd2vr::contact176::recoveryEpoch55c1.load()||now<copy.started||now-copy.started>=1200)return;
    // Only the current original-update argument is dereferenced, not an old
    // saved pointer from the queue. Recycled entity/state identities retire it.
    const void *entity=nullptr,*state=nullptr;
    const bool valid=CombatActor(actor,entity,state,false)&&entity==copy.entity&&state==copy.state&&EntityId(entity)==copy.id;
    const int combat=valid?B(state,kInCombat):-1,phase=valid?I(state,0x30):-1;
    bool emit=false;
    {std::lock_guard<std::mutex> l(mutex);for(auto& w:watches)if(w.actor==copy.actor&&w.serial==copy.serial&&w.session==copy.session){
        if(!valid){w={};count.fetch_sub(1);emit=true;break;}
        ++w.observed;emit=w.emitted<4&&(w.observed==1||w.phase!=phase||w.lastCombat!=combat);
        // Keep baseline combatAtContinuation; phase changes provide a bounded
        // native-state observation, not proof of the queue consumer's execution.
        if(emit)++w.emitted;w.phase=phase;w.lastCombat=combat;copy.observed=w.observed;break;}}
    if(emit)Note(copy,valid?"AFTER_NATIVE_ACTOR_UPDATE":"OWNER_REPLACED",now,combat,phase);
}
} // namespace victim5916c3
