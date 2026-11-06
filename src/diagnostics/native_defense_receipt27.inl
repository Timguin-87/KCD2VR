// C27: F3-only native NPC selection receipts. No gameplay ownership or credit.
// Included after native actor readers, before native-services callbacks.
namespace defense27 {
namespace mo=kcd2vr::motion59c4;
bool CopyDirectKey(const void* attacker,mo::Key& key) noexcept; // defined after the state driver
using CandidateFn=void*(__fastcall*)(const void*,void**,const void*,int,int);
static CandidateFn candidateOriginal=nullptr;
static std::atomic<bool> enabled{false};
static std::atomic<std::uint64_t> sequence{0},dropped{0};
struct Receipt {
    std::uint64_t outer=0,candidate=0,tick=0,session=0;unsigned thread=0,stage=0,selector=0;
    std::uintptr_t recipient=0,attacker=0,action=0,vft=0,row=0;mo::Key key{};
    unsigned candidates=0,evaluations=0,evaluationOk=0;int nativeKind=-1,nativeDefenseZone=-1,predictedBlock=-1,predictedPerfect=-1,predictedMaster=-1;
    bool keyKnown=false,typeKnown=false,managerKnown=false,dispatchReturned=false,ambiguous=false;
};
struct Selection {std::uintptr_t action=0;Receipt receipt{};};
struct CounterFrame {std::uint64_t id=0;const void* recipient=nullptr;std::array<Selection,4> selected{};unsigned count=0,attempted=0;bool overflow=false;Receipt receipt{};};
struct CandidateFrame {Receipt receipt{};CounterFrame* outer=nullptr;};
static thread_local CounterFrame* counterFrame=nullptr;
static thread_local CandidateFrame* candidateFrame=nullptr;
static std::mutex journalMutex;static std::array<Receipt,64> journal{};static unsigned head=0,count=0;
static void Queue(const Receipt& r) noexcept {
    std::unique_lock<std::mutex> lock(journalMutex,std::try_to_lock);
    if(!lock.owns_lock()||count==journal.size()){dropped.fetch_add(1,std::memory_order_relaxed);return;}
    journal[(head+count)%journal.size()]=r;++count;
}
static bool Pop(Receipt& out) noexcept {
    std::unique_lock<std::mutex> lock(journalMutex,std::try_to_lock);
    if(!lock.owns_lock()||!count)return false;out=journal[head];head=(head+1)%journal.size();--count;return true;
}
static bool Npc(const void* actor) noexcept {
    const void *entity=nullptr,*state=nullptr;
    return g_base&&Rd<std::uintptr_t>(actor,0,0)==g_base+kCombatActorVft&&CombatActor(actor,entity,state,false)&&!IsPlayer(entity);
}
static bool EnterCounter(const void* actor,unsigned selector,CounterFrame& frame) noexcept {
    counterFrame=nullptr;
    if(!enabled.load(std::memory_order_acquire)||!Active()||selector!=6||!Npc(actor))return false;
    frame.id=sequence.fetch_add(1,std::memory_order_relaxed)+1;frame.recipient=actor;
    frame.receipt.outer=frame.id;frame.receipt.tick=GetTickCount64();frame.receipt.thread=GetCurrentThreadId();
    frame.receipt.recipient=reinterpret_cast<std::uintptr_t>(actor);frame.receipt.selector=selector;
    frame.receipt.session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
    counterFrame=&frame;return true;
}
static void LeaveCounter(CounterFrame& frame,CounterFrame* saved,bool entered,bool returned) noexcept {
    if(entered){frame.receipt.candidates=frame.attempted;frame.receipt.ambiguous=frame.overflow||!returned;Queue(frame.receipt);}
    counterFrame=saved;
}
struct CounterScope {
    CounterFrame frame{};CounterFrame* saved=nullptr;bool entered=false;
    CounterScope(const void* actor,unsigned selector) noexcept:saved(counterFrame){entered=EnterCounter(actor,selector,frame);}
    ~CounterScope(){LeaveCounter(frame,saved,entered,true);}
};
using CounterBodyFn=void*(__fastcall*)(void*,void**,unsigned char,int,bool,int);
// POD-only wrapper: native SEH propagates, while stack TLS is always restored.
static void* CounterInvoke(CounterBodyFn body,void* actor,void** out,unsigned char selector,int arg4,bool option,int arg6) noexcept {
    // Quiet common path allocates/zeros no receipt frame. Nested scopes still
    // take the finally path so a foreign callback cannot inherit their lineage.
    if(!counterFrame&&!candidateFrame&&(!enabled.load(std::memory_order_acquire)||selector!=6||!Active()))
        return body?body(actor,out,selector,arg4,option,arg6):nullptr;
#if defined(_MSC_VER) && !defined(__try) // host syntax shim maps __try to C++ try
    CounterFrame frame{};auto* saved=counterFrame;const bool entered=EnterCounter(actor,selector,frame);
    void* result=nullptr;bool returned=false;
    __try{result=body?body(actor,out,selector,arg4,option,arg6):nullptr;returned=true;}
    __finally{LeaveCounter(frame,saved,entered,returned);}
    return result;
#else
    CounterScope scope(actor,selector);return body?body(actor,out,selector,arg4,option,arg6):nullptr;
#endif
}
static bool CandidateOwner(const void* rpg,const void* attacker,Receipt& r) noexcept {
    if(!counterFrame||!counterFrame->recipient||!rpg||!attacker)return false;
    const auto defender=counterFrame->recipient;const void *ae=nullptr,*as=nullptr;
    if(Rd<const void*>(rpg,8,nullptr)!=defender||Rd<const void*>(defender,0x3A8,nullptr)!=rpg||!Npc(defender)||
       Rd<std::uintptr_t>(attacker,0,0)!=g_base+kCombatPlayerVft||!CombatActor(attacker,ae,as,false)||!IsPlayer(ae))return false;
    r.recipient=reinterpret_cast<std::uintptr_t>(defender);r.attacker=reinterpret_cast<std::uintptr_t>(attacker);
    return true;
}
static void CandidateSelected(CandidateFrame& frame,void** out) noexcept {
    auto& r=frame.receipt;
    // Holder belongs to the still-entered native caller. Copy now, never after dispatch.
    r.action=Rd<std::uintptr_t>(out,0,0);r.vft=Rd<std::uintptr_t>(reinterpret_cast<const void*>(r.action),0,0);
    r.typeKnown=r.vft==g_base+0x3A89688;
    if(r.typeKnown){const auto manager=Rd<std::uintptr_t>(reinterpret_cast<const void*>(r.recipient),0x200,0);
        r.managerKnown=manager&&Rd<std::uintptr_t>(reinterpret_cast<const void*>(r.action),0x40,0)==manager;
        r.row=Rd<std::uintptr_t>(reinterpret_cast<const void*>(r.action),0x58,0);}
    r.stage=1;Queue(r);
    // Multiple candidates remain independent; overflow disables an exact dispatch join.
    if(r.action&&frame.outer){auto& outer=*frame.outer;
        if(outer.count<outer.selected.size())outer.selected[outer.count++]={r.action,r};else outer.overflow=true;}
}
void* __fastcall CandidateDetour(const void* rpg,void** out,const void* attacker,int kind,int zone) noexcept {
    const auto original=candidateOriginal;
    if(!candidateFrame&&!counterFrame)return original?original(rpg,out,attacker,kind,zone):nullptr;
    const auto caller=KCD2VR_COMBO_CALLER();
    CandidateFrame frame{};auto* saved=candidateFrame;candidateFrame=nullptr;
    const bool qualified=enabled.load(std::memory_order_acquire)&&Active()&&g_base&&caller==g_base+0x14837B4&&CandidateOwner(rpg,attacker,frame.receipt);
    if(qualified){++counterFrame->attempted;auto& r=frame.receipt;r.outer=counterFrame->id;r.candidate=sequence.fetch_add(1,std::memory_order_relaxed)+1;
        r.tick=GetTickCount64();r.thread=GetCurrentThreadId();r.selector=6;r.nativeKind=kind;r.nativeDefenseZone=zone;
        r.session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
        r.keyKnown=CopyDirectKey(attacker,r.key);frame.outer=counterFrame;candidateFrame=&frame;}
    void* result=nullptr;
#if defined(_MSC_VER) && !defined(__try) // host syntax shim maps __try to C++ try
    __try{result=original?original(rpg,out,attacker,kind,zone):nullptr;}
    __finally{candidateFrame=saved;}
#else
    struct Reset {CandidateFrame* prior;~Reset(){candidateFrame=prior;}};
    {Reset reset{saved};result=original?original(rpg,out,attacker,kind,zone):nullptr;}
#endif
    if(qualified&&Active()){
        const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
        if(enabled.load(std::memory_order_acquire)&&result==out&&session==frame.receipt.session)CandidateSelected(frame,out);
        else {frame.receipt.stage=1;frame.receipt.ambiguous=true;frame.receipt.keyKnown=false;frame.receipt.key={};Queue(frame.receipt);}
    }
    return result;
}
struct EvaluationScope {
    CandidateFrame* frame=nullptr;
    EvaluationScope(const void* rpg,const void* request,bool apply,std::uintptr_t caller) noexcept {
        if(!enabled.load(std::memory_order_acquire)||!Active()||apply||!counterFrame||!candidateFrame||candidateFrame->outer!=counterFrame||!g_base||caller!=g_base+0x173BF58||
           candidateFrame->receipt.session!=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load())return;
        const auto& r=candidateFrame->receipt;
        if(Rd<std::uintptr_t>(rpg,8,0)==r.attacker&&Rd<std::uintptr_t>(request,0x10,0)==r.recipient)frame=candidateFrame;
    }
    void Finish(bool ok,const void* details) noexcept {
        if(!frame||!enabled.load(std::memory_order_acquire)||!Active()||candidateFrame!=frame||
           frame->receipt.session!=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load())return;
        auto& r=frame->receipt;++r.evaluations;r.evaluationOk+=ok;
        if(r.evaluations>1){r.ambiguous=true;r.predictedBlock=r.predictedPerfect=r.predictedMaster=-1;return;}
        if(ok&&details){r.predictedBlock=int(Rd<std::uint8_t>(details,1,255));
            r.predictedPerfect=int(Rd<std::uint8_t>(details,3,255));r.predictedMaster=int(Rd<std::uint8_t>(details,7,255));}
    }
};
struct DispatchScope {
    Receipt receipt{};bool qualified=false;
    DispatchScope(const void* recipient,void* holder,std::uintptr_t caller) noexcept {
        if(!enabled.load(std::memory_order_acquire)||!Active()||!counterFrame||!g_base||caller!=g_base+0x1483924||recipient!=counterFrame->recipient)return;
        receipt.outer=counterFrame->id;receipt.recipient=reinterpret_cast<std::uintptr_t>(recipient);
        receipt.tick=GetTickCount64();receipt.thread=GetCurrentThreadId();receipt.stage=2;
        receipt.session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
        receipt.action=Rd<std::uintptr_t>(holder,0,0);unsigned matches=0;
        for(unsigned i=0;i<counterFrame->count;++i)if(receipt.action&&counterFrame->selected[i].action==receipt.action){
            ++matches;receipt=counterFrame->selected[i].receipt;}
        receipt.tick=GetTickCount64();receipt.thread=GetCurrentThreadId();receipt.stage=2;const auto currentSession=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
        receipt.ambiguous=receipt.ambiguous||counterFrame->overflow||matches!=1||receipt.session!=currentSession;
        if(matches!=1||counterFrame->overflow||receipt.session!=currentSession){receipt.candidate=0;receipt.keyKnown=false;receipt.key={};}
        qualified=true;
    }
    void Finish(bool returned) noexcept {
        // Native may have destroyed/replaced its holder/action; only copied POD is used here.
        if(!qualified)return;
        const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
        if(!enabled.load(std::memory_order_acquire)||!Active()||session!=receipt.session){
            receipt.ambiguous=true;receipt.candidate=0;receipt.keyKnown=false;receipt.key={};
        }
        receipt.dispatchReturned=returned;Queue(receipt);
    }
};
static void Drain() noexcept {
    Receipt r{};unsigned budget=32;char line[1400]{};
    while(budget--&&Pop(r)){
        std::snprintf(line,sizeof(line),"C27 NPC_DEFENSE outer=%llu candidate=%llu stage=%s tick=%llu session=%llu thread=%u recipient=0x%llX attacker=0x%llX action=0x%llX vft=0x%llX row=0x%llX typeKnown=%d managerKnown=%d keyKnown=%d keyJoin=%s nativeKind=%d nativeDefenseZone=%d gesture=%llu weapon=%llu generation=%llu body=%llu request=%llu recovery=%llu player=0x%llX candidates=%u evaluations=%u evaluationOk=%u predictedBlock=%d predictedPerfect=%d predictedMaster=%d ambiguous=%d dispatchReturned=%d join=%s completion=UNOBSERVED writes=NONE",
            (unsigned long long)r.outer,(unsigned long long)r.candidate,r.stage==0?"COUNTER_RETURN":r.stage==1?"CANDIDATE_RETURN":"DISPATCH_RETURN",
            (unsigned long long)r.tick,(unsigned long long)r.session,r.thread,(unsigned long long)r.recipient,(unsigned long long)r.attacker,
            (unsigned long long)r.action,(unsigned long long)r.vft,(unsigned long long)r.row,r.typeKnown,r.managerKnown,r.keyKnown,r.keyKnown?"ENTRY_CONTEXT_ONLY":"UNKNOWN",r.nativeKind,r.nativeDefenseZone,
            (unsigned long long)r.key.gesture,(unsigned long long)r.key.weapon,(unsigned long long)r.key.generation,(unsigned long long)r.key.body,
            (unsigned long long)r.key.request,(unsigned long long)r.key.recovery,(unsigned long long)r.key.player,r.candidates,r.evaluations,r.evaluationOk,
            r.predictedBlock,r.predictedPerfect,r.predictedMaster,r.ambiguous,r.dispatchReturned,
            r.candidate&&!r.ambiguous?"SAME_CANDIDATE_INVOCATION":"UNKNOWN");
        diagnostic_trace::Write(line);
    }
    static std::uint64_t previous=0;const auto lost=dropped.load(std::memory_order_relaxed);
    if(lost!=previous){previous=lost;std::snprintf(line,sizeof(line),"C27 NPC_DEFENSE_DROPPED total=%llu queueCapacity=64 drainLimit=32 writes=NONE",(unsigned long long)lost);diagnostic_trace::Write(line);}
}
}
