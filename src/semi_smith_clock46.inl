// Included inside action_time_owner59c11's native owner namespace. The existing
// Queue/Update hooks call the small public seams below; no world/game dt hook.
namespace smith46=kcd2vr::semi_smith46;
namespace smithNative46=kcd2vr::semi_smith46::native;
smithNative46::Lease g_semi_smith46{};
std::atomic<bool> g_semi_smith_pending46{false},g_semi_smith_stop46{true};
std::atomic<unsigned> g_semi_smith_inflight46{0};
std::optional<DetourModKit::hook::Hook> g_semi_smith_builder_hook46;
using SemiSmithBuilderFn46=void(__fastcall*)(void*,void*);
SemiSmithBuilderFn46 g_semi_smith_builder_original46=nullptr;
struct SemiSmithExecuting46 {smithNative46::Parent parent{};bool valid=false;};
static thread_local SemiSmithExecuting46 g_semi_smith_executing46{};
struct SemiSmithAbandoned46 {std::uintptr_t instance=0;std::uint64_t lifetime=0,request=0;} g_semi_smith_abandoned46;
template<class T> static bool SemiSmithRead46(std::uintptr_t p,T& value) noexcept {
    return p&&SehCopy(reinterpret_cast<void*>(p),&value,sizeof(value));
}
static bool SemiSmithWrite46(std::uintptr_t p,float value) noexcept {return Wr<float>(reinterpret_cast<void*>(p),0,value);}
static void SemiSmithReceipt46(const char* phase,const smithNative46::Parent& p,float rate=-1.f) noexcept {
    if(!DiagOn())return;
    namespace cap=kcd2vr::diagnostics::capture168;
    static kcd2vr::diagnostics::interaction_receipt59c10::Gate gate;
    unsigned key=0;for(const char* at=phase;*at;++at)key=key*33+unsigned(*at);
    const auto now=std::uint64_t(GetTickCount64());const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load();
    if(!gate.observe(session,now,{p.child,std::uint32_t(p.code),p.phase,int(key)},0,96))return;
    char line[440]{};std::snprintf(line,sizeof(line),
        "C46 SEMI_SMITH phase=%s nativePhase=%d actionCode=%d instance=%llX parent=%llX child=%llX lifetime=%llu request=%llu rate=%.3f clock=EXACT_CALLBACK_CHILD nativeHeat=UNCHANGED nativeImpact=UNCHANGED",
        phase,p.phase,p.code,(unsigned long long)p.instance,(unsigned long long)p.parent,(unsigned long long)p.child,
        (unsigned long long)p.lifetime,(unsigned long long)p.request,double(rate));diagnostic_trace::Write(line);
}
static bool SemiSmithPins46() noexcept {
    const auto b=g_base.load(std::memory_order_acquire);if(!b||!retail_build112::Ready(b))return false;
    static std::uintptr_t checked=0;static bool good=false;if(checked==b)return good;
    good=true;for(const auto& p:smith46::pins::all)good=good&&MatchBytes(b+p.rva,reinterpret_cast<const unsigned char*>(p.bytes),p.size);
    for(const auto& p:smith46::pins::slots)good=good&&Rd<std::uintptr_t>(reinterpret_cast<void*>(b+p.rva),0,0)==b+p.target;
    checked=b;return good;
}
static bool SemiSmithInput46(const smithNative46::Parent& p,std::uint64_t now,smith46::ClockControl& c) noexcept {
    namespace act=kcd2vr::activity_input163;const auto& w=act::witness175;
    if(g_semi_smith_stop46.load(std::memory_order_acquire)||!smith46::clockReady.load(std::memory_order_acquire)||
       (kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Smithing)==kcd2vr::minigame_hands45::Native&&!kcd2vr::guided_crafting55::active(now))||
       !kcd2vr::workstation178r20c1::ownsActivityWrist45(now)||!kcd2vr::workstation178r20c1::gesturesOn(now)||
       !kcd2vr::workstation178r20c1::smithAutoHold36.load(std::memory_order_acquire)||
       !act::trackedHands59c7f[unsigned(act::Kind175::Smithing)].load(std::memory_order_acquire)||
       !w.ready.load(std::memory_order_acquire)||w.transitionDepth175.load(std::memory_order_acquire)||
       w.active[unsigned(act::Kind175::Smithing)].load(std::memory_order_acquire)!=p.instance||
       w.active[unsigned(act::Kind175::Alchemy)].load(std::memory_order_acquire)||
       w.lifetime175.load(std::memory_order_acquire)!=p.lifetime||!smith46::clockControl.read(c)||
       !c.enabled||!c.request||!c.sequence||!c.poseEpoch||!c.sourceEpoch||!c.modeEpoch67||
       c.modeEpoch67!=kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire)||!c.tick||now<c.tick||now-c.tick>80||
       c.instance!=p.instance||c.lifetime!=p.lifetime||c.request!=p.request||
       (c.requestedPhase!=14&&c.requestedPhase!=16)||
       (g_semi_smith_abandoned46.instance==c.instance&&g_semi_smith_abandoned46.lifetime==c.lifetime&&
        g_semi_smith_abandoned46.request==c.request))return false;
    const auto packed=kcd2vr::minigame_hands45::witness.load(std::memory_order_acquire);
    if(kcd2vr::minigame_hands45::kindOf(packed)!=kcd2vr::minigame_hands45::Smithing)return false;
    kcd2vr::workstation_sample46::Frame frame{};
    // A copied producer packet cannot keep a native zero-speed owner after a
    // current tracking-loss, recenter, or menu/mode transition.
    return frik_pose_probe::WorkstationFrame46(frame)&&frame.allowed&&
        kcd2vr::workstation_sample46::fresh(frame,now)&&frame.hand[0].valid&&
        kcd2vr::workstation_sample46::finite(frame.hand[0])&&frame.poseEpoch==c.poseEpoch&&frame.sourceEpoch==c.sourceEpoch;
}
static bool SemiSmithParent46(void* ctrl,smithNative46::Parent& p,std::uintptr_t executing=0,std::uintptr_t child=0,bool seed=false) noexcept {
    namespace act=kcd2vr::activity_input163;const auto& w=act::witness175;
    p={};const auto b=g_base.load(std::memory_order_acquire);
    if(!b||!ctrl||ctrl!=g_henry_ctrl.load(std::memory_order_acquire)||!OnOwnerThread13()||!IsHenryController(ctrl)||
       !w.ready.load(std::memory_order_acquire)||w.transitionDepth175.load(std::memory_order_acquire))return false;
    const auto life=w.lifetime175.load(std::memory_order_acquire);
    const auto instance=w.active[unsigned(act::Kind175::Smithing)].load(std::memory_order_acquire);
    const auto rd=[](std::uintptr_t at,auto& v){return SemiSmithRead46(at,v);};
    if(!smithNative46::parent(rd,b,instance,executing,child,Rd<unsigned>(ctrl,kCtrlEntityId,0),seed,p))return false;
    if(!w.ready.load(std::memory_order_acquire)||w.transitionDepth175.load(std::memory_order_acquire)||
       w.lifetime175.load(std::memory_order_acquire)!=life||w.active[unsigned(act::Kind175::Smithing)].load(std::memory_order_acquire)!=instance)return false;
    smith46::ClockControl c{};if(!smith46::clockControl.read(c)||c.instance!=instance||c.lifetime!=life||!c.request)return false;
    p.lifetime=life;p.request=c.request;return true;
}
static bool RestoreSemiSmith46(const char* reason,bool abandon=false) noexcept {
    auto& l=g_semi_smith46;if(!l.written)return true;
    if(!OnOwnerThread13()||l.controller!=reinterpret_cast<std::uintptr_t>(g_henry_ctrl.load(std::memory_order_acquire))||
       !IsHenryController(reinterpret_cast<void*>(l.controller)))return false;
    const auto previous=l.owner;
    if(abandon){
        g_semi_smith_abandoned46={previous.instance,previous.lifetime,previous.request};
        // Revoke input authority even if a compare/restore is still pending.
        // A previous successful queue ACK is never a claim about unknown live memory.
        (void)smith46::clockStatus.publish({previous.instance,previous.lifetime,previous.request,std::uint64_t(GetTickCount64()),previous.phase,false});
    }
    const auto rd=[](std::uintptr_t at,auto& value){return SemiSmithRead46(at,value);};
    const auto result=smithNative46::restore(rd,SemiSmithWrite46,g_base.load(std::memory_order_acquire),l);
    if(result==smithNative46::Restore::Pending){SemiSmithReceipt46("RESTORE_PENDING",previous);return false;}
    g_semi_smith_pending46.store(false,std::memory_order_release);
    (void)smith46::clockStatus.publish({previous.instance,previous.lifetime,previous.request,std::uint64_t(GetTickCount64()),previous.phase,false});
    SemiSmithReceipt46(result==smithNative46::Restore::Ended?"CHILD_ENDED_NO_STALE_WRITE":
        result==smithNative46::Restore::Foreign?"NATIVE_SPEED_WRITER_WINS":reason,previous);
    return true;
}
// Called by the exact builder hook before its original; the native smart ptr
// still owns child here. No zero speed is written until the ordinary Queue
// returns with native membership, status and context established.
static SemiSmithExecuting46 BeginSemiSmithBuilder46(void* parent,void* smart) noexcept {
    SemiSmithExecuting46 out{};
    if(!smith46::clockReady.load(std::memory_order_acquire)||g_semi_smith_stop46.load(std::memory_order_acquire))return out;
    OwnerTransaction19c4 transaction;if(!transaction||g_owner.state!=Idle)return out;
    std::uintptr_t child=0;if(!SemiSmithRead46(reinterpret_cast<std::uintptr_t>(smart),child)||!child)return out;
    if(!SemiSmithParent46(g_henry_ctrl.load(std::memory_order_acquire),out.parent,reinterpret_cast<std::uintptr_t>(parent),child,true))return out;
    smith46::ClockControl c{};out.valid=SemiSmithInput46(out.parent,GetTickCount64(),c)&&c.requestedPhase==out.parent.phase;
    return out;
}
static void __fastcall SemiSmithBuilderDetour46(void* parent,void* smart) noexcept {
    g_semi_smith_inflight46.fetch_add(1,std::memory_order_acq_rel);
    const SemiSmithExecuting46 before=g_semi_smith_executing46;
    g_semi_smith_executing46=BeginSemiSmithBuilder46(parent,smart);
    // POD locals only: production SEH finally restores nested TLS on a native
    // exception as well. Native callbacks/results are never swallowed.
#if defined(_MSC_VER) && !defined(KCD2VR_GRASP_HOST_TEST)
    __try {if(g_semi_smith_builder_original46)g_semi_smith_builder_original46(parent,smart);}
    __finally {g_semi_smith_executing46=before;g_semi_smith_inflight46.fetch_sub(1,std::memory_order_acq_rel);}
#else
    if(g_semi_smith_builder_original46)g_semi_smith_builder_original46(parent,smart);
    g_semi_smith_executing46=before;g_semi_smith_inflight46.fetch_sub(1,std::memory_order_acq_rel);
#endif
}
static void SeedSemiSmithClock46(void* ctrl,void* action,std::uint64_t now) noexcept {
    if(!ctrl||!action||!g_semi_smith_executing46.valid||!smith46::clockReady.load(std::memory_order_acquire))return;
    OwnerTransaction19c4 transaction;if(!transaction||g_owner.state!=Idle)return;
    const auto executing=g_semi_smith_executing46.parent;
    if(executing.child!=reinterpret_cast<std::uintptr_t>(action))return;
    smithNative46::Parent p{};smith46::ClockControl c{};
    if(!SemiSmithParent46(ctrl,p,executing.parent,executing.child,true)||p.instance!=executing.instance||
       p.lifetime!=executing.lifetime||p.request!=executing.request||!SemiSmithInput46(p,now,c)||c.requestedPhase!=p.phase)return;
    auto& l=g_semi_smith46;
    const auto rd=[](std::uintptr_t at,auto& value){return SemiSmithRead46(at,value);};
    const auto b=g_base.load(std::memory_order_acquire),controller=reinterpret_cast<std::uintptr_t>(ctrl);
    const auto member=smithNative46::membership(rd,b,controller,p.child);smithNative46::Child native{};
    if(member.kind!=smithNative46::Membership::Queued||!smithNative46::child(rd,b,controller,p.child,member.kind,native))return;
    if(l.written){
        if(l.owner.code==p.code&&l.owner.instance==p.instance&&l.owner.lifetime==p.lifetime&&l.owner.request==p.request){
            // One native child per raise/finish phase. Native requeue/reset is
            // not permission to replay already-consumed hand credit.
            (void)RestoreSemiSmith46("SAME_PHASE_REQUEUE_NATIVE_RESUME",true);return;
        }
        if(!RestoreSemiSmith46("NEXT_NATIVE_WORK"))return;
    }
    // Allocation reuse at the same child address is a new token/code, never a
    // duplicate queue. The old lease is retired before seeding this allocation.
    if(native.activeTime!=0.f||native.speed<=0.f||native.speed>8.f)return;
    // Last permission read is adjacent to the only initial mutation.
    if(!SemiSmithInput46(p,now,c)||g_owner.state!=Idle)return;
    l={};l.owner=p;l.controller=controller;l.token=native.token;l.poseEpoch=c.poseEpoch;l.sourceEpoch=c.sourceEpoch;l.modeEpoch67=c.modeEpoch67;
    l.saved=native.speed;l.lastWritten=0.f;l.written=true;l.budget.begin(native.activeTime,now);
    g_semi_smith_pending46.store(true,std::memory_order_release);
    if(!SemiSmithWrite46(p.child+0x5C,0.f)||Rd<float>(action,0x5C,-1.f)!=0.f){(void)RestoreSemiSmith46("SEED_WRITE_REFUSED",true);return;}
    (void)smith46::clockStatus.publish({p.instance,p.lifetime,p.request,now,p.phase,true});
    SemiSmithReceipt46("WORK_QUEUED_PAUSED",p,0.f);
}
static void TickSemiSmithClock46(void* ctrl,float dt,std::uint64_t now) noexcept {
    auto& l=g_semi_smith46;if(!l.written)return;
    if(!ctrl||reinterpret_cast<std::uintptr_t>(ctrl)!=l.controller||ctrl!=g_henry_ctrl.load(std::memory_order_acquire)||
       !OnOwnerThread13()||!IsHenryController(ctrl))return;
    if(l.uncertainWrite){(void)RestoreSemiSmith46("UNCERTAIN_WRITE_NATIVE_RESUME",true);return;}
    smithNative46::Parent p{};smith46::ClockControl c{};
    const bool parent=SemiSmithParent46(ctrl,p,l.owner.parent,l.owner.child);
    if(!parent||p.instance!=l.owner.instance||p.lifetime!=l.owner.lifetime||p.request!=l.owner.request||
       !SemiSmithInput46(p,now,c)||c.poseEpoch!=l.poseEpoch||c.sourceEpoch!=l.sourceEpoch||c.modeEpoch67!=l.modeEpoch67||g_owner.state!=Idle){
        smith46::ClockControl transition{};
        const bool expectedFinish=l.owner.code==10&&SemiSmithInput46(l.owner,now,transition)&&transition.modeEpoch67==l.modeEpoch67&&transition.requestedPhase==16;
        (void)RestoreSemiSmith46("NATIVE_CONTINUATION",!expectedFinish);return;}
    const auto rd=[](std::uintptr_t at,auto& value){return SemiSmithRead46(at,value);};
    const auto b=g_base.load(std::memory_order_acquire);
    const auto member=smithNative46::membership(rd,b,l.controller,p.child);
    if(member.kind==smithNative46::Membership::Unknown){(void)RestoreSemiSmith46("MEMBERSHIP_UNREADABLE",true);return;}
    if(member.kind!=smithNative46::Membership::Queued&&member.kind!=smithNative46::Membership::Root){
        (void)RestoreSemiSmith46("NATIVE_WORK_ENDED");return;}
    smithNative46::Child native{};
    if(!smithNative46::child(rd,b,l.controller,p.child,member.kind,native)||native.token!=l.token||native.speed!=l.lastWritten){
        (void)RestoreSemiSmith46("NATIVE_IDENTITY_OR_SPEED_CHANGED",true);return;}
    const double work=p.code==10?c.raiseWork:c.finishWork;
    const float scale=Rd<float>(ctrl,0x9C,0.f);
    // During release acknowledgement the old raise clip is held at zero; only
    // the replacement code11 child consumes the stored downstroke work.
    const float speed=member.kind==smithNative46::Membership::Root&&c.requestedPhase==p.phase?
        l.budget.rate(work,native.activeTime,dt,scale,now):0.f;
    smith46::ClockControl finalControl{};
    if(!SemiSmithInput46(p,std::uint64_t(GetTickCount64()),finalControl)||
       finalControl.poseEpoch!=l.poseEpoch||finalControl.sourceEpoch!=l.sourceEpoch||
       finalControl.requestedPhase!=c.requestedPhase){(void)RestoreSemiSmith46("FINAL_PERMISSION_CHANGED",true);return;}
    if(speed!=l.lastWritten){const float previous=l.lastWritten;
        l.uncertainWrite=true;l.candidate=speed;
        if(!SemiSmithWrite46(p.child+0x5C,speed)||Rd<float>(reinterpret_cast<void*>(p.child),0x5C,-1.f)!=speed){
            (void)RestoreSemiSmith46("RATE_WRITE_REFUSED",true);return;}
        l.lastWritten=speed;l.uncertainWrite=false;if((previous==0.f)!=(speed==0.f))SemiSmithReceipt46(speed==0.f?"HAND_STOPPED":"HAND_ADVANCING",p,speed);
    }
    (void)smith46::clockStatus.publish({p.instance,p.lifetime,p.request,now,p.phase,true});
}
static bool InstallSemiSmithClock46() noexcept {
    if(g_semi_smith_builder_hook46)return smith46::clockReady.load(std::memory_order_acquire);
    if(!SemiSmithPins46())return false;
    DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
    DetourModKit::hook::InlineRequest request{"kcd2vr_semi_smith_child46",DetourModKit::hook::Target{DetourModKit::Address{g_base.load()+0x399AE0}},options};
    auto hook=DetourModKit::hook::inline_at(std::move(request),&SemiSmithBuilderDetour46);if(!hook)return false;
    g_semi_smith_builder_hook46=std::move(*hook);g_semi_smith_builder_original46=g_semi_smith_builder_hook46->original<SemiSmithBuilderFn46>();
    const bool good=g_semi_smith_builder_original46&&bool(g_semi_smith_builder_hook46->enable());
    g_semi_smith_stop46.store(!good,std::memory_order_release);smith46::clockReady.store(good,std::memory_order_release);
    diagnostic_trace::Write(good?"C46 SEMI_SMITH READY raise=CODE10 finish=CODE11 clock=HAND_WORK nativeHeat=UNCHANGED charge=NORMALIZED_NATIVE_ANIMATION":
        "C46 SEMI_SMITH INSTALL_REFUSED nativeFallback=1 originals=RETAINED");return good;
}
static bool StopSemiSmithClock46() noexcept {
    g_semi_smith_stop46.store(true,std::memory_order_release);smith46::clockReady.store(false,std::memory_order_release);
    if(g_semi_smith_pending46.load(std::memory_order_acquire)&&(!OnOwnerThread13()||!RestoreSemiSmith46("STOP_NATIVE_RESUME",true)))return false;
    return !g_semi_smith_pending46.load(std::memory_order_acquire)&&g_semi_smith_inflight46.load(std::memory_order_acquire)==0;
}
static bool DisarmSemiSmithClock46() noexcept {
    bool ok=true;
    if(g_semi_smith_builder_hook46&&g_semi_smith_builder_hook46->is_enabled()){
        const auto result=g_semi_smith_builder_hook46->disable();ok=bool(result)&&!g_semi_smith_builder_hook46->is_enabled();}
    // Keep the original and handle after disarm for in-flight native calls.
    // Hot reinstall is refused; physical DLL unload needs external quiescence.
    return ok&&g_semi_smith_inflight46.load(std::memory_order_acquire)==0;
}
