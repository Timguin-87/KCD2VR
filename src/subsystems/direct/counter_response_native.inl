// F3 observation of the SAME applied contact; never another hit or native write.
namespace counter36 {
struct State {int phase=-1,type=-99,combo=-1;};
struct Before {State npc{};int window=-1,origin=-1,executed=-1,offered=-1;std::uint64_t session=0;bool captured=false;};
struct Watch {const void* actor=nullptr;const void* state=nullptr;std::uint32_t serial=0,target=0;std::uint64_t start=0;Before before{};State last{};int dispatch=-1;unsigned samples=0,emitted=0;
    std::uintptr_t henry37=0;bool track37=false;unsigned emitted37=0;kcd2vr::defense37::Npc last37{};}; // C37: NPC defense/action-slot edges
static std::array<Watch,8> watches{};static std::mutex mutex;
static State Snapshot(const void* state) noexcept {
    State s{};s.phase=Rd<int>(state,0x38,-1);s.type=Rd<int>(state,0x218,-99);
    if(Rd<std::uintptr_t>(state,0x860,0)==g_base+0x3B0A6A0){const auto b=Rd<std::uint8_t>(state,0x868,255);if(b<=1)s.combo=b;}
    return s;
}
static Before Capture(const void* hs,const void* ts,int origin,std::uint64_t now) noexcept {
    Before b{};if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return b;
    b.captured=true;b.session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();b.npc=Snapshot(ts);b.origin=origin;b.executed=Rd<int>(ts,0x278,-1);b.offered=Rd<int>(ts,0x158,-1);
    const auto count=Rd<int>(hs,0x7D8,-1);b.window=count>=0&&count<64?count:-1;return b;
}
static void Emit(const Watch& w,const State& s,const char* stage,std::uint64_t now) noexcept {
    using namespace kcd2vr::response36;char line[1000]{};
    std::snprintf(line,sizeof(line),
        "C36 CONTACT_REACTION serial=%u target=%u stage=%s ageMs=%llu counterWindow7D8=%d origin=%d enemyExecuted=%d enemyOffered=%d oppositeExecuted=%d phaseBefore=%d phaseNow=%d typeBefore=%d typeNow=%d comboWindowBefore=%d comboWindowNow=%d reactionDispatch=%d hitPhaseTransition=%d comboWindowClosed=%d samples=%u interruptCause=UNPROVEN masterAward=NONE extraHit=NONE session=%llu tick=%llu actor=%llX state=%llX observedTransitions=%u sameContactObservation=1",
        w.serial,w.target,stage,(unsigned long long)(now>=w.start?now-w.start:0),w.before.window,w.before.origin,w.before.executed,w.before.offered,
        int(opposite(w.before.origin,w.before.executed)),w.before.npc.phase,s.phase,w.before.npc.type,s.type,w.before.npc.combo,s.combo,w.dispatch,
        int(reactionTransition(w.before.npc.phase,s.phase,w.dispatch)),int(comboWindowClosed(w.before.npc.combo,s.combo)),w.samples,
        (unsigned long long)w.before.session,(unsigned long long)now,(unsigned long long)reinterpret_cast<std::uintptr_t>(w.actor),
        (unsigned long long)reinterpret_cast<std::uintptr_t>(w.state),w.emitted);
    diagnostic_trace::Write(line);
}
// C37: the exact 90C850 bracket captured inside CallOuter for THIS target (F3 only). Landing is a slot comparison,
// not a claim that the NPC's attack was or was not interrupted (the attack can live in another slot).
static void EmitDispatch37(const Pending& p,const kcd2vr::defense37::Dispatch& d,std::uint64_t session,std::uint64_t now) noexcept {
    namespace d37=kcd2vr::defense37;int index=-1;const auto landing=d37::Classify(d,&index);
    char before[560]{},after[560]{};d37::FormatNpc(before,sizeof(before),d.before);d37::FormatNpc(after,sizeof(after),d.after);
    char line[1700]{};std::snprintf(line,sizeof(line),
        "C37 REACTION_DISPATCH serial=%u target=%u dispatch=%d henryPhase=%d henryReach4B0=%d reaction=%X/p%d/s%d/a%d landing=%s landedSlot=%d before{%s} after{%s} interruptCause=UNPROVEN writes=NONE session=%llu tick=%llu actor=%llX henry=%llX",
        p.serial,p.entityId,d.result,d.henry.phase,d.henry.reach,d.reaction.vft,d.reaction.priority,d.reaction.slot,d.reaction.active,
        d37::LandingName(landing),index,before,after,(unsigned long long)session,(unsigned long long)now,
        (unsigned long long)d.target,(unsigned long long)d.henryActor);
    diagnostic_trace::Write(line);
}
static void Begin(const Pending& p,const void* actor,const void* state,const Before& before,int dispatch,std::uint64_t now,const kcd2vr::defense37::Dispatch* d37=nullptr) noexcept {
    if(!before.captured||before.session!=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load()||!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    std::unique_lock<std::mutex> l(mutex,std::try_to_lock);if(!l.owns_lock())return;
    Watch* slot=nullptr;for(auto& w:watches)if(!w.actor||!kcd2vr::response36::fresh(w.start,now)){slot=&w;break;}
    if(!slot){diagnostic_trace::Write("C36 CONTACT_REACTION watch=CAPACITY_8 noOverwrite=1");return;}
    *slot={actor,state,p.serial,p.entityId,now,before,Snapshot(state),dispatch,0,0};Emit(*slot,slot->last,"AFTER_CONTINUATION",now);
    {   // C39: was this SAME applied contact a counter into the enemy's attack (opposite/master side)? Receipt only: the native master
        // strike is a paired sync action (1483A90->1483B04) with its own native hits, so DIRECT never requests it.
        namespace dc=kcd2vr::direct_counter39;const auto st=dc::openState.load(std::memory_order_acquire);
        const bool open=st&&st==reinterpret_cast<std::uintptr_t>(state);
        if(open||kcd2vr::response36::attackPhase(before.npc.phase)){
            const auto ot=open?dc::openTick.load(std::memory_order_relaxed):0;const int startOffered=open?dc::openOffered.load(std::memory_order_relaxed):-1;
            const int offered=before.offered>=0?before.offered:startOffered;char line[600]{};std::snprintf(line,sizeof(line),
                "C39 DIRECT_COUNTER edge=CANDIDATE serial=%u target=%u window=%llu sinceStartMs=%lld npcPhase=%d offered158=%d startOffered158=%d msStance=%d origin=%d counterSide=%d ms7D8=%d reactionDispatch=%d admission=NOT_REQUESTED refusal=%s damage=ONE_COLLISION_DIRECT_SUBMIT result=SEE_C36_SAME_SERIAL",
                p.serial,p.entityId,open?(unsigned long long)dc::openSerial.load(std::memory_order_relaxed):0ull,ot&&now>=ot?(long long)(now-ot):-1LL,
                before.npc.phase,offered,startOffered,dc::msZone(offered),before.origin,int(dc::counterSide(before.origin,offered)),before.window,dispatch,dc::kRefusal);
            diagnostic_trace::Write(line);
        }
    }
    if(d37&&d37->captured&&d37->target==reinterpret_cast<std::uintptr_t>(actor)){
        EmitDispatch37(p,*d37,before.session,now);slot->henry37=d37->henryActor;slot->track37=true;slot->last37=d37->after;}
}
static void ActorUpdated(const void* actor,std::uint64_t now) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    std::unique_lock<std::mutex> l(mutex,std::try_to_lock);if(!l.owns_lock())return;
    for(auto& w:watches){
        if(!w.actor)continue;
        if(w.before.session!=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load()){w={};continue;}
        if(!kcd2vr::response36::fresh(w.start,now)){Emit(w,w.last,"EXPIRED_LAST_OBSERVATION",now);w={};continue;}
        if(w.actor!=actor)continue;
        const void *entity=nullptr,*state=nullptr;
        if(!CombatActor(actor,entity,state,false)||state!=w.state||EntityId(entity)!=w.target||Rd<std::uintptr_t>(actor,0,0)!=g_base+kCombatActorVft){
            Emit(w,w.last,"IDENTITY_LOST",now);w={};continue;}
        const auto s=Snapshot(state);++w.samples;
        if(s.phase!=w.last.phase||s.type!=w.last.type||s.combo!=w.last.combo){
            if(w.emitted++<8)Emit(w,s,"NATIVE_UPDATE_CHANGED",now);w.last=s;
        }
        if(w.track37&&w.emitted37<8){ // C37: same bounded watch; automation/block/slot edges only
            const auto n=kcd2vr::defense37::ReadNpc(Copy37,g_base,reinterpret_cast<std::uintptr_t>(actor),reinterpret_cast<std::uintptr_t>(state),w.henry37);
            if(!kcd2vr::defense37::Same(n,w.last37)){++w.emitted37;w.last37=n;char text[560]{};kcd2vr::defense37::FormatNpc(text,sizeof(text),n);
                char line[760]{};std::snprintf(line,sizeof(line),"C37 REACTION_DEFENSE serial=%u target=%u ageMs=%llu %s writes=NONE",
                    w.serial,w.target,(unsigned long long)(now>=w.start?now-w.start:0),text);diagnostic_trace::Write(line);}
        }
        // At most500ms per exact contact; no raw pointer retained as a lifetime lease.
    }
}
}
