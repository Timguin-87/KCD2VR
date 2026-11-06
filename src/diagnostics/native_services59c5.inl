// Read-only exact-retail service observations. This is NOT the missing combat
// backend. Never call these detours as an alternate attack/PB driver.
namespace services59c5 {
void Install() noexcept;void Uninstall() noexcept;
namespace s=kcd2vr::diagnostics::services59c5;
using RaisedFn=void(__fastcall*)(void*,bool);
using CommitFn=void(__fastcall*)(void*);
using EffectFn=void*(__fastcall*)(void*,void*,std::uint64_t,float,unsigned char);
using CounterFn=void*(__fastcall*)(void*,void**,unsigned char,int,bool,int);
RaisedFn raised=nullptr;CommitFn commit=nullptr;EffectFn effect=nullptr;CounterFn counter=nullptr;
std::optional<DetourModKit::hook::Hook> raisedHook,commitHook,effectHook,counterHook;
thread_local s::Record* current=nullptr;
// R59.16C10R17 swing stamina (see swing_effort59c14.h). Henry's soul WUID is published by geom27 every Henry update.
std::atomic<std::uint64_t> henryWuid59c17{0},strikesScaled59c17{0},ownScaled59c17{0},strikesNative59c17{0},strikesZeroed59c17{0};
std::atomic<float> lastNative59c17{0},lastScaled59c17{0},lastFactor59c17{0};
// C10R19C1: the synchronous keyed OwnRaiseScope lives in swing_effort59c14.h.
std::atomic<bool> staminaScaling59c17{true}; // mirrors VrSwingStamina && !session fault (geom27 StaminaFallback59c17, every Henry update)
std::atomic<std::uint64_t> overheadNs{0},overheadMaxNs{0},observedCalls{0};
static std::uint64_t Clock() noexcept {
    return std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
}
static std::uint64_t Ns(std::uint64_t ticks) noexcept {return ticks;}
static void Cost(std::uint64_t ticks) noexcept {
    const auto ns=Ns(ticks);overheadNs.fetch_add(ns);++observedCalls;
    auto max=overheadMaxNs.load();while(max<ns&&!overheadMaxNs.compare_exchange_weak(max,ns)){}
}
static bool PlayerRecord(void* actor,s::Record& r) noexcept {
    if(!Active()||Rd<std::uintptr_t>(actor,0,0)!=g_base+kCombatPlayerVft)return false;
    const void *entity=nullptr,*state=nullptr;
    if(!CombatActor(actor,entity,state,false)||!IsPlayer(entity))return false;
    r.session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
    r.span=s::journal.span();r.tick=GetTickCount64();r.thread=GetCurrentThreadId();
    r.actor=reinterpret_cast<std::uintptr_t>(actor);r.state=reinterpret_cast<std::uintptr_t>(state);
    r.actorId=EntityId(entity);r.other=Rd<std::uintptr_t>(state,0xCF8,0);
    r.phase=I(state,0x30);r.type=I(state,kAttackType);r.zone=I(state,kCommittedZone);
    r.strength=F(state,kAttackStrength);return true;
}
static bool RpgRecord(void* rpg,s::Record& r) noexcept {
    void* actor=Rd<void*>(rpg,8,nullptr);
    return actor&&Rd<void*>(actor,0x388,nullptr)==rpg&&PlayerRecord(actor,r);
}
struct Scope {s::Record* old;explicit Scope(s::Record* r):old(current){current=r;}~Scope(){current=old;}};
// C54: complete native MainAttackStart ancestry, independent of F3 recording.
// No latest-slot join, added debit or altered effect value.
static bool NativeCostLink54(void* rpg,kcd2vr::swing_effort59c14::NativeCost54* cost,std::uint64_t& wuid,float& stamina) noexcept {
    if(!cost||!cost->valid()||!retail_build112::Ready(g_base)||!rpg)return false;
    const void* actor=Rd<const void*>(rpg,8,nullptr);const void *entity=nullptr,*state=nullptr;
    if(reinterpret_cast<std::uintptr_t>(actor)!=cost->actor||Rd<const void*>(actor,0x388,nullptr)!=rpg||
       !CombatActor(actor,entity,state,false)||!IsPlayer(entity)||reinterpret_cast<std::uintptr_t>(state)!=cost->state||
       I(state,kAttackType)!=cost->type||I(state,kAttackingHand)!=cost->hand||
       Rd<std::uintptr_t>(state,0x8C0,0)!=g_base+0x3B0A5E8)return false;
    std::uint64_t nativeWeapon=0;
    if(!SehCopy146(static_cast<const unsigned char*>(state)+0x8C8+8*unsigned(cost->hand),&nativeWeapon,sizeof(nativeWeapon))||
       nativeWeapon!=(cost->preparationOnly54?cost->nativeWeapon54:cost->key.weapon))return false;
    const void* player=Rd<const void*>(actor,0x1F8,nullptr);const void* soul=player?Rd<const void*>(player,0x668,nullptr):nullptr;
    if(reinterpret_cast<std::uintptr_t>(player)!=cost->key.player||!soul||Rd<std::uintptr_t>(soul,0,0)!=g_base+0x3F44F28)return false;
    wuid=Rd<std::uint64_t>(soul,0x30,0);if(!wuid)return false;
    __try{stamina=reinterpret_cast<float(__fastcall*)(const void*,int)>(g_base+0x642B30)(soul,1);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    return std::isfinite(stamina);
}
struct NativeCostWrapper54 {
    kcd2vr::swing_effort59c14::NativeWrapper54 value{};
    kcd2vr::swing_effort59c14::NativeWrapper54* old=nullptr;void* rpg=nullptr;bool called=false;
    explicit NativeCostWrapper54(void* p,unsigned kind) noexcept:rpg(p){namespace se=kcd2vr::swing_effort59c14;
        old=se::nativeWrapper54;float before=0;auto* owner=se::nativeCost54;
        if(kind&&owner&&!se::ownRaise&&!kcd2vr::kick34::kickRaise&&NativeCostLink54(p,owner,value.wuid,before)){
            value.owner=owner;value.kind=kind;value.valid=true;
            if(owner->wuid&&owner->wuid!=value.wuid)owner->invalid=true;else owner->wuid=value.wuid;}
        se::nativeWrapper54=value.valid?&value:nullptr;
    }
    ~NativeCostWrapper54(){namespace se=kcd2vr::swing_effort59c14;
        if(value.owner){std::uint64_t wuid=0;float after=0;
            if(!called||value.effects!=1||!NativeCostLink54(rpg,value.owner,wuid,after)||wuid!=value.wuid)value.owner->invalid=true;
            else if(value.kind==1)++value.owner->raised;else ++value.owner->struck;}
        se::nativeWrapper54=old;
    }
};
void __fastcall Raised(void* rpg,bool up) noexcept {
    NativeCostWrapper54 native54(rpg,up?1u:0u);
    if(!Active()){Scope scope(nullptr);if(raised){native54.called=true;raised(rpg,up);}return;}
    const auto begin=Clock();s::Record r{};const bool trace=RpgRecord(rpg,r);
    if(!trace){Scope scope(nullptr);if(raised){native54.called=true;raised(rpg,up);}return;}
    if(trace){r.kind=s::Kind::RaisedBegin;r.flags=up?1:0;s::journal.push(r);}
    const auto entered=Clock();
    {Scope scope(trace?&r:nullptr);if(raised){native54.called=true;raised(rpg,up);}}
    const auto returned=Clock();
    if(trace){r.kind=s::Kind::RaisedEnd;r.tick=GetTickCount64();r.elapsedNs=Ns(returned-entered);s::journal.push(r);Cost(entered-begin+Clock()-returned);}
}
void __fastcall Commit(void* rpg) noexcept {
    const auto caller54=KCD2VR_COMBO_CALLER();
    NativeCostWrapper54 native54(rpg,caller54==g_base+0x93921C?2u:0u);
    if(!Active()){Scope scope(nullptr);if(commit){native54.called=true;commit(rpg);}return;}
    const auto begin=Clock();s::Record r{};const bool trace=RpgRecord(rpg,r);
    if(!trace){Scope scope(nullptr);if(commit){native54.called=true;commit(rpg);}return;}
    if(trace){r.kind=s::Kind::CommitBegin;s::journal.push(r);}
    const auto entered=Clock();
    {Scope scope(trace?&r:nullptr);if(commit){native54.called=true;commit(rpg);}}
    const auto returned=Clock();
    if(trace){
        r.kind=s::Kind::CommitEnd;r.tick=GetTickCount64();r.elapsedNs=Ns(returned-entered);
        // Read after the real wrapper set the native strength. Revalidate all
        // links after re-entry; a replaced state is unavailable, never reused.
        s::Record after{};
        if(RpgRecord(rpg,after)&&after.actor==r.actor&&after.state==r.state)r.strength=after.strength;
        else r.flags|=0x100;
        s::journal.push(r);Cost(entered-begin+Clock()-returned);
    }
}
void* __fastcall Effect(void* context,void* transaction,std::uint64_t wuid,float delta,unsigned char flag) noexcept {
    // C10R19C1: an asynchronous native strike cannot be associated with the latest VR gesture.
    // Preserve its original delta and give it no attempt credit. Only our exact synchronous
    // raise scope (key + expected soul + native call site) may scale this one effect.
    // C37C2: only while KickApply37c2 holds its exact KickRaiseScope (opt-in real-hit test): its one effect = native kick cost.
    bool kick37c2=false;
    if(kcd2vr::kick34::kickRaise&&g_base&&std::isfinite(delta)&&delta<=0.f&&wuid){
        const auto ret=KCD2VR_COMBO_CALLER();kick37c2=kcd2vr::kick34::scaleKickEffect(wuid,ret>=g_base?ret-g_base:0,flag,delta);}
    if(!kick37c2&&staminaScaling59c17.load(std::memory_order_relaxed)&&g_base&&std::isfinite(delta)&&delta<=0.f&&wuid&&
       wuid==henryWuid59c17.load(std::memory_order_relaxed)&&frik_pose_probe::CombatExperiment175Enabled()){
        const auto ret=KCD2VR_COMBO_CALLER();const auto caller=ret>=g_base?ret-g_base:0;
        const float original=delta;
        if(kcd2vr::swing_effort59c14::scaleOwnedEffect(wuid,caller,flag,delta)){
            lastNative59c17.store(original,std::memory_order_relaxed);lastScaled59c17.store(delta,std::memory_order_relaxed);
            lastFactor59c17.store(kcd2vr::swing_effort59c14::ownRaise->multiplier*.30f,std::memory_order_relaxed);
            ownScaled59c17.fetch_add(1,std::memory_order_relaxed);
        }else if(caller==0xC64DCA&&flag==2)strikesNative59c17.fetch_add(1,std::memory_order_relaxed);
    }
    // Factory use outside a proven Henry effort stack is an immediate forward.
    const auto nativeCaller54=KCD2VR_COMBO_CALLER();
    if(!current){void* result=effect?effect(context,transaction,wuid,delta,flag):nullptr;
        (void)kcd2vr::swing_effort59c14::nativeEffect54(wuid,nativeCaller54>=g_base?nativeCaller54-g_base:0,flag,delta,result!=nullptr);return result;}
    const auto begin=Clock();auto r=*current;const auto entered=Clock();
    void* result=effect?effect(context,transaction,wuid,delta,flag):nullptr;
    (void)kcd2vr::swing_effort59c14::nativeEffect54(wuid,nativeCaller54>=g_base?nativeCaller54-g_base:0,flag,delta,result!=nullptr);
    const auto returned=Clock();r.kind=s::Kind::EffectBuilt;r.tick=GetTickCount64();r.elapsedNs=Ns(returned-entered);
    r.actorWuid=wuid;r.delta=delta;r.flags=flag;r.transaction=reinterpret_cast<std::uintptr_t>(transaction);
    r.object=reinterpret_cast<std::uintptr_t>(result);r.vtable=Rd<std::uintptr_t>(result,0,0);
    // Construction is NOT submission/application. The enclosing wrapper's end
    // proves return through its submit/release path, not a measured stamina delta.
    s::journal.push(r);Cost(entered-begin+Clock()-returned);return result;
}
// C23b OUTCOME_CHAIN: kind 5 nested in OUR owned D4AD94 call (same stack, same actor). The returned
// reference is only tested for null after the original; its ownership and lifetime stay native.
static void OutcomeKind5_23(void* actor,void** out,unsigned char selector) noexcept {
    if(selector!=5||!kcd2vr::outcome23::onStack(reinterpret_cast<std::uintptr_t>(actor)))return;
    const bool ref=Rd<std::uintptr_t>(out,0,0)!=0;const auto now=std::uint64_t(GetTickCount64());
    kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){k.kind5(ref,now,kcd2vr::outcome23::counters);});
}
static void* __fastcall CounterBody27(void* actor,void** out,unsigned char selector,int arg4,bool option,int arg6) noexcept {
    if(!Active()){void* r=counter?counter(actor,out,selector,arg4,option,arg6):nullptr;OutcomeKind5_23(actor,out,selector);return r;}
    const auto begin=Clock();s::Record r{};const bool trace=PlayerRecord(actor,r);
    if(trace){r.kind=s::Kind::CounterBegin;r.selector=selector;r.argument4=arg4;r.argument6=arg6;r.flags=option?1:0;s::journal.push(r);}
    const auto entered=Clock();
    void* result=counter?counter(actor,out,selector,arg4,option,arg6):nullptr;
    const auto returned=Clock();
    if(trace){r.kind=s::Kind::CounterEnd;r.tick=GetTickCount64();r.elapsedNs=Ns(returned-entered);
        r.object=Rd<std::uintptr_t>(out,0,0);r.vtable=Rd<std::uintptr_t>(reinterpret_cast<const void*>(r.object),0,0);
        s::journal.push(r);Cost(entered-begin+Clock()-returned);}
    OutcomeKind5_23(actor,out,selector);
    return result; // native return object and reference ownership unchanged
}
void* __fastcall Counter(void* actor,void** out,unsigned char selector,int arg4,bool option,int arg6) noexcept {
    return defense27::CounterInvoke(CounterBody27,actor,out,selector,arg4,option,arg6);
}
void Hit(const void* attacker,const void* target,const void* ae,const void* te,const void* hi,const void* details,bool ok,std::uintptr_t caller,bool applyNative) noexcept {
    if(!Active()||(!IsPlayer(ae)&&!IsPlayer(te)))return;
    const auto begin=Clock();s::Record r{};r.kind=s::Kind::HitReturn;r.tick=GetTickCount64();r.thread=GetCurrentThreadId();
    r.session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
    r.actor=reinterpret_cast<std::uintptr_t>(attacker);r.other=reinterpret_cast<std::uintptr_t>(target);
    r.actorId=EntityId(ae);r.targetId=EntityId(te);r.caller=caller;
    r.health=Rd<float>(hi,0x30,0);r.victimStamina=Rd<float>(hi,0x2C,0);
    r.flags=(ok?1:0)|(Rd<unsigned char>(details,1,0)?2:0)|(Rd<unsigned char>(details,3,0)?4:0)|(Rd<unsigned char>(details,7,0)?8:0)|(applyNative?0x200:0);
    // D568B0's sixth argument selects evaluation versus application. HitInfo
    // contains calculated damage in BOTH cases, not a measured actor HP delta.
    // Deliberately no invented join from an earlier Counter to an asynchronous hit.
    s::journal.push(r);Cost(Clock()-begin);
}
void Drain() noexcept {
    defense27::Drain();
    s::Record r{};unsigned budget=64;char line[1100]{};
    while(budget--&&s::journal.pop(r)){
        const char* hitEffect=r.kind!=s::Kind::HitReturn?"NOT_HIT":(r.flags&0x200)?"APPLICATION_REQUEST":"EVALUATION";
        std::snprintf(line,sizeof(line),"R59C5 NATIVE_SERVICE seq=%llu session=%llu span=%llu stage=%u tick=%llu thread=%u nativeCallNs=%llu actorId=%u targetId=%u actor=0x%llX state=0x%llX other=0x%llX phase=%d type=%d zone=%d strength=%.5g selector=%d argument4=%d argument6=%d flags=0x%X actorWuid=0x%llX requestedEffectDelta=%.5g transaction=0x%llX object=0x%llX vtable=0x%llX callerRva=0x%llX hitEffect=%s hitInfoHealthDamage=%.5g hitInfoStaminaDamage=%.5g writes=NONE",
          (unsigned long long)r.sequence,(unsigned long long)r.session,(unsigned long long)r.span,unsigned(r.kind),(unsigned long long)r.tick,r.thread,(unsigned long long)r.elapsedNs,r.actorId,r.targetId,(unsigned long long)r.actor,(unsigned long long)r.state,(unsigned long long)r.other,r.phase,r.type,r.zone,r.strength,r.selector,r.argument4,r.argument6,r.flags,(unsigned long long)r.actorWuid,r.delta,(unsigned long long)r.transaction,(unsigned long long)r.object,(unsigned long long)r.vtable,(unsigned long long)r.caller,hitEffect,r.health,r.victimStamina);
        diagnostic_trace::Write(line);
    }
    static std::uint64_t previous=0,lastLost=0,next=0;const auto now=GetTickCount64();
    if(now>=next){next=now+2000;const auto calls=observedCalls.load(),lost=s::journal.dropped();
        if(calls!=previous||lost!=lastLost){previous=calls;lastLost=lost;std::snprintf(line,sizeof(line),"R59C5 NATIVE_SERVICE_BUDGET calls=%llu observerNsTotal=%llu observerNsMax=%llu dropped=%llu queueCapacity=512 drainLimit=64 includesNativeTime=0",
            (unsigned long long)calls,(unsigned long long)overheadNs.load(),(unsigned long long)overheadMaxNs.load(),(unsigned long long)lost);diagnostic_trace::Write(line);}}
}
}
