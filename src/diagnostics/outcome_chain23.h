#pragma once
// C23b OUTCOME_CHAIN: RECEIPTS ONLY. One joined, bounded, edge-only record per VR
// gesture (ph::Command::gesture59c3) telling where a master strike / combo / kill
// stopped. Every field is copied from a site the mod ALREADY hooks or pumps:
//   prepare/commit edge   NativePhysicalAction117 (ms7D8 live read, existing C23 receipt)
//   XI handler return     XiOwnedCall178r13       (ms7D8 live read, existing C23 receipt)
//   D4AD94                AdmissionGateHook51     (inline detour, R51C1): payload pre-read of
//                         state+838/+7D8/+868 in D4AD94's own branch order, plus its return
//   1483580 kind 5        services59c5::Counter   (inline detour, R59C5): same stack only
//   1483A90 factory       MasterFactoryGate168    (mid-hook, V168): same stack only
//   90C850 @148391F       ReactionDispatchDetour50 (inline detour, R50C1): caller 1483924 + same stack
//   91115C phase 16       combo59c17::TriggerPhaseDetour (inline detour, R59.16C10R17)
//   D568B0 results        SubmitHitDetour / direct CallSubmit: ok && apply==1 ONLY
// Nothing here calls the game, writes game memory, or changes a native argument/result.
// Not observable without a new hook (deliberately absent): 1483540's own return,
// 909A90/9D15D0/909EA0 predicate results, C5B404/C5C52C advance, C5AC2C result,
// the factory's selected row (55 vs 62 etc.) and the action entry of the dispatched reference.
#include <array>
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <mutex>
namespace kcd2vr::outcome23 {
// D4AD94 branch order (WHGame 1.5.6, 6A350E20):
//   D4AE58 (state+830 property) && input==heavy -> 1899D6C, return 1
//   state+7D8 > 0                                -> 22B780B -> 1483540 (kind5), return 1
//   D4AE70 (state+860 combo slot)                -> C5AC2C(actor+3B0), return 1
//   D4AE88 (actor+3F0 special input) / opponent-less path -> may return 1; otherwise 0
// The +838/+868 bytes are the property PAYLOADS; D4AD94 calls the property's getter.
// A predicted consuming route that returned 0 is reported as UNKNOWN (pre-read mismatch).
enum class Route : signed char {Unseen=-2,Unknown=-1,Ordinary=0,Special830=1,Master=2,Combo=3,OtherConsumed=4};
inline const char* name(Route r) noexcept {
    switch(r){case Route::Unseen:return "UNSEEN";case Route::Unknown:return "UNKNOWN";case Route::Ordinary:return "ORDINARY";
        case Route::Special830:return "SPECIAL_830";case Route::Master:return "MASTER_7D8";case Route::Combo:return "COMBO_SLOT";
        case Route::OtherConsumed:return "OTHER_CONSUMED";}
    return "?";
}
inline Route classify(bool known,bool r830,bool heavyInput,int ms7D8,bool slot868,bool returned) noexcept {
    if(!known||ms7D8<0||ms7D8>=64)return Route::Unknown;
    Route r=Route::Ordinary;
    if(r830&&heavyInput)r=Route::Special830;
    else if(ms7D8>0)r=Route::Master;
    else if(slot868)r=Route::Combo;
    if(r!=Route::Ordinary)return returned?r:Route::Unknown;
    return returned?Route::OtherConsumed:Route::Ordinary;
}
// One D568B0 result. Only ok && apply are outcomes; apply==0 is an evaluation.
struct Hit {
    bool ok=false,apply=false,flagEdge=false;
    int parry=-1,blocked=-1,perfect=-1,combo=-1,comboEnd=-1,master=-1,death=-1,ko=-1,reaction=-1,deathKind=-1;
    std::uint32_t target=0;
};
// C59 finite F3-only evidence. A requested/ACKed Henry zone is not an NPC
// defense. NPC fields below are copied only from the already-budgeted F3 state
// census; matching actor/id and chronology prove observation, not causation.
struct Owner59 {
    std::uintptr_t player=0;std::uint64_t key=0,generation=0,body=0,request=0;
};
inline bool equal59(const Owner59& a,const Owner59& b) noexcept {
    return a.player&&a.key&&a.generation&&a.body&&a.request&&a.player==b.player&&a.key==b.key&&
        a.generation==b.generation&&a.body==b.body&&a.request==b.request;
}
struct Announcement59 {
    std::uint64_t tick=0,sample=0;int kind=0,requested=-1,announced=-1,ack=-1,after=-1;
    bool dispatched=false;unsigned npcObservations=0;
    std::uint64_t firstNpcTick=0,lastNpcTick=0;
    int firstDefense=-1,lastDefense=-1,firstBlocking=-1,lastBlocking=-1,lastOpponent=-1;
};
struct Feint59 {
    Owner59 owner{};std::uintptr_t actor=0;std::uint32_t target=0;
    std::array<Announcement59,4> announcements{};unsigned count=0,dropped=0;
    unsigned applied=0,blocked=0,perfect=0,parry=0,master=0,combo=0,comboEnd=0,exact=0,temporal=0;
};
struct Chain {
    std::uint64_t gesture=0,openTick=0,lastTick=0;int stroke=-1;
    int msPrepare=-2,msCommit=-2,msXiPrepare=-2,msXiRelease=-2; // -2 not seen, -1 unreadable
    Route prepareRoute=Route::Unseen,releaseRoute=Route::Unseen;int prepareSlot=-1,releaseSlot=-1;
    unsigned admissions=0;bool factoryObservable=false,slotObservable=false,msCounted=false,comboRouteCounted=false;
    unsigned kind5=0,kind5Ref=0,factoryQ=0,factoryNative=0,factoryPermit=0,dispatch=0,dispatchOk=0;
    unsigned slot16=0,slotRequested=0;
    unsigned applied=0,exact=0,temporal=0,evalSkipped=0,blocked=0,perfect=0,parry=0,master=0,combo=0,comboEnd=0,death=0,ko=0,flagEdges=0,deathRows=0;
    int lastReaction=-1;Feint59 feint59{};
};
inline bool msOpened(const Chain& c) noexcept {
    return c.msPrepare>0||c.msCommit>0||c.msXiPrepare>0||c.msXiRelease>0||c.prepareRoute==Route::Master||c.releaseRoute==Route::Master;
}
// First missing link of the native master-strike chain (contract C23 §1). A later link
// observed without an earlier one is reported as the earlier link's gap, never as success.
inline const char* masterVerdict(const Chain& c) noexcept {
    if(!msOpened(c))return c.master?"MASTER_FLAG_WITHOUT_OBSERVED_WINDOW":"NO_WINDOW";
    if(!c.admissions)return "STOP_BEFORE_D4AD94";
    if(c.prepareRoute!=Route::Master&&c.releaseRoute!=Route::Master)return "STOP_AT_D4AD94_ROUTE";
    if(!c.kind5)return "STOP_KIND5_UNOBSERVED";
    if(!c.factoryQ)return c.factoryObservable?"STOP_BEFORE_FACTORY":"FACTORY_GATE_UNARMED";
    if(!c.factoryNative)return "STOP_FACTORY_GROUP_FALSE";
    if(!c.factoryPermit)return "STOP_VR_VETO";
    if(!c.dispatch)return "STOP_BEFORE_DISPATCH_148391F";
    if(!c.dispatchOk)return "STOP_DISPATCH_REFUSED";
    if(!c.kind5Ref)return "STOP_NO_REFERENCE";
    if(!c.applied)return "DISPATCHED_NO_APPLIED_HIT";
    return c.master?"APPLIED_MASTER_FLAG":"APPLIED_WITHOUT_MASTER_FLAG";
}
// This gesture's press inside a slot opened by an EARLIER hit.
inline const char* comboInVerdict(const Chain& c) noexcept {
    if(c.prepareRoute==Route::Combo||c.releaseRoute==Route::Combo)return "C5AC2C_ROUTE_ADVANCE_UNOBSERVED";
    if(c.prepareSlot==1||c.releaseSlot==1)return "SLOT_PAYLOAD_SET_EARLIER_ROUTE_WON";
    if(!c.admissions)return "D4AD94_UNOBSERVED";
    return "NO_SLOT_AT_PRESS";
}
// This gesture's own hit opening the next slot (91115C phase 16 -> 909A90).
inline const char* comboOutVerdict(const Chain& c) noexcept {
    if(!c.slotObservable)return "PHASE16_OBSERVER_UNARMED";
    if(!c.slot16)return c.applied?"APPLIED_HIT_NO_PHASE16":"NO_PHASE16";
    if(!c.slotRequested)return "SLOT_NOT_REQUESTED_909A90_FALSE";
    return "SLOT_REQUESTED";
}
inline const char* killVerdict(const Chain& c) noexcept {
    if(!c.applied)return "NO_APPLIED_HIT";
    if(c.death)return c.deathRows?"DEATH_FLAG_DEATH_REACTION":"DEATH_FLAG";
    return c.ko?"KO_FLAG":"ALIVE";
}
struct Counters {
    std::atomic<std::uint64_t> msOpenAtPrepare{0},msFactoryQ{0},msRef{0},msAccepted{0},comboSlotOpen{0},comboRoute{0},
        comboAdvance{0},kills{0},deathRows{0},evalSkipped{0},unjoined{0},lines{0},dropped{0},busy{0};
};
inline int formatCounters(const Counters& k,char* out,std::size_t cap) noexcept {
    if(!out||!cap)return 0;
    return std::snprintf(out,cap,"chain{msOpenAtPrepare=%llu msFactoryQ=%llu msRef=%llu msAccepted=%llu comboSlotOpen=%llu comboRoute=%llu comboAdvance=%llu kills=%llu deathRows=%llu evalSkipped=%llu unjoined=%llu lines=%llu dropped=%llu}",
        (unsigned long long)k.msOpenAtPrepare.load(),(unsigned long long)k.msFactoryQ.load(),(unsigned long long)k.msRef.load(),
        (unsigned long long)k.msAccepted.load(),(unsigned long long)k.comboSlotOpen.load(),(unsigned long long)k.comboRoute.load(),
        (unsigned long long)k.comboAdvance.load(),(unsigned long long)k.kills.load(),(unsigned long long)k.deathRows.load(),
        (unsigned long long)k.evalSkipped.load(),(unsigned long long)k.unjoined.load(),(unsigned long long)k.lines.load(),
        (unsigned long long)(k.dropped.load()+k.busy.load()));
}
inline int format(const Chain& c,std::uint64_t now,char* out,std::size_t cap) noexcept {
    if(!out||!cap)return 0;
    return std::snprintf(out,cap,
        "C23 OUTCOME_CHAIN gesture=%llu stroke=%d spanMs=%llu idleMs=%llu ms7D8{prepare=%d commit=%d xiPrepare=%d xiRelease=%d} "
        "d4ad94{calls=%u prepare=%s release=%s slotPayload=%d/%d} kind5{returns=%u ref=%u} factory1483A90{observable=%d q=%u group=%u permit=%u} "
        "dispatch148391F{calls=%u ok=%u} phase16{observable=%d seen=%u requested=%u} "
        "applied{n=%u exact=%u temporal=%u blocked=%u perfect=%u parry=%u master=%u combo=%u comboEnd=%u death=%u ko=%u flagEdge=%u deathRows=%u reaction=%d} evalSkipped=%u "
        "master=%s comboIn=%s comboOut=%s kill=%s join=D4AD94_SAME_STACK,HITS_EXACT_OR_TEMPORAL,PHASE16_TEMPORAL row=UNOBSERVED actionEntry=UNOBSERVED writes=NONE",
        (unsigned long long)c.gesture,c.stroke,(unsigned long long)(c.lastTick>=c.openTick?c.lastTick-c.openTick:0),
        (unsigned long long)(now>=c.lastTick?now-c.lastTick:0),c.msPrepare,c.msCommit,c.msXiPrepare,c.msXiRelease,
        c.admissions,name(c.prepareRoute),name(c.releaseRoute),c.prepareSlot,c.releaseSlot,c.kind5,c.kind5Ref,
        c.factoryObservable?1:0,c.factoryQ,c.factoryNative,c.factoryPermit,c.dispatch,c.dispatchOk,
        c.slotObservable?1:0,c.slot16,c.slotRequested,
        c.applied,c.exact,c.temporal,c.blocked,c.perfect,c.parry,c.master,c.combo,c.comboEnd,c.death,c.ko,c.flagEdges,c.deathRows,c.lastReaction,
        c.evalSkipped,masterVerdict(c),comboInVerdict(c),comboOutVerdict(c),killVerdict(c));
}
inline int formatFeint59(const Chain& c,char* out,std::size_t cap) noexcept {
    if(!out||!cap||!c.feint59.count)return 0;
    const auto& f=c.feint59;std::size_t used=0;
    const auto append=[&](const char* text){if(used+1>=cap)return;const auto n=std::snprintf(out+used,cap-used,"%s",text);
        if(n>0)used=std::min(cap-1,used+std::size_t(n));};
    char b[700]{};std::snprintf(b,sizeof(b),
        "C59 F3_FEINT_CHAIN gesture=%llu player=%llX key=%llX gen=%llu body=%llu request=%llu npc=%u actor=%llX entries=%u dropped=%u ",
        (unsigned long long)c.gesture,(unsigned long long)f.owner.player,(unsigned long long)f.owner.key,
        (unsigned long long)f.owner.generation,(unsigned long long)f.owner.body,(unsigned long long)f.owner.request,
        f.target,(unsigned long long)f.actor,f.count,f.dropped);append(b);
    for(unsigned i=0;i<f.count;++i){const auto& e=f.announcements[i];std::snprintf(b,sizeof(b),
        "e%u{kind=%d tick=%llu sample=%llu requested=%d announced=%d ACK=%d dispatched=%d after=%d npcN=%u firstTick=%llu lastTick=%llu defense=%d/%d blocking=%d/%d opponentZone=%d} ",
        i,e.kind,(unsigned long long)e.tick,(unsigned long long)e.sample,e.requested,e.announced,e.ack,int(e.dispatched),e.after,e.npcObservations,
        (unsigned long long)e.firstNpcTick,(unsigned long long)e.lastNpcTick,e.firstDefense,e.lastDefense,e.firstBlocking,e.lastBlocking,e.lastOpponent);append(b);}
    std::snprintf(b,sizeof(b),"targetApplied{n=%u exact=%u temporal=%u blocked=%u perfect=%u parry=%u master=%u comboFlag=%u comboEndFlag=%u} join=EXACT_OWNER_TARGET_AND_TIME,HITS_EXACT_OR_TEMPORAL causalNpcReaction=UNPROVEN npcCoverage=EXISTING_F3_BUDGETED_STATE_EDGES cutToCutRoute=ONE_SEGMENT_ZONE_ONLY_KIND5_ACK writes=NONE extraReads=0",
        f.applied,f.exact,f.temporal,f.blocked,f.perfect,f.parry,f.master,f.combo,f.comboEnd);append(b);return int(used);
}
// Pure joiner. One open chain (the latest gesture); closed chains wait in a bounded ring.
struct Core {
    static constexpr std::uint64_t idleMs=2500,temporalMs=2000;static constexpr unsigned ringSize=8;
    Chain cur{};bool has=false;std::array<Chain,ringSize> ring{};unsigned head=0,count=0;
    void close(Counters& k) noexcept {
        if(!has)return;
        has=false;
        if(count==ringSize){k.dropped.fetch_add(1,std::memory_order_relaxed);return;} // bounded: newest dropped, counted
        ring[(head+count)%ringSize]=cur;++count;
    }
    bool pop(Chain& out) noexcept {if(!count)return false;out=ring[head];head=(head+1)%ringSize;--count;return true;}
    void pump(std::uint64_t now,Counters& k) noexcept {if(has&&now>=cur.lastTick&&now-cur.lastTick>idleMs)close(k);}
    Chain* current(std::uint64_t now) noexcept {
        return has&&now>=cur.lastTick&&now-cur.lastTick<=temporalMs?&cur:nullptr;
    }
    // Prepare/commit edge of OUR owned input. The edge is the only thing that opens a chain.
    void edge(std::uint64_t gesture,int stroke,bool prepare,int ms7D8,bool factoryObservable,bool slotObservable,std::uint64_t now,Counters& k) noexcept {
        if(!gesture)return;
        if(!has||cur.gesture!=gesture){close(k);cur={};has=true;cur.gesture=gesture;cur.openTick=now;cur.stroke=stroke;}
        cur.lastTick=now;cur.factoryObservable=cur.factoryObservable||factoryObservable;cur.slotObservable=cur.slotObservable||slotObservable;
        (prepare?cur.msPrepare:cur.msCommit)=ms7D8;
        if(prepare&&ms7D8>0&&!cur.msCounted){cur.msCounted=true;k.msOpenAtPrepare.fetch_add(1,std::memory_order_relaxed);}
    }
    void announce59(std::uint64_t gesture,const Owner59& owner,std::uintptr_t actor,std::uint32_t target,
        int kind,int requested,int announced,int ack,std::uint64_t sample,std::uint64_t now) noexcept {
        auto* c=current(now);if(!c||c->gesture!=gesture||!sample||!owner.player||!owner.key||!owner.generation||!owner.body||!owner.request)return;
        auto& f=c->feint59;
        if(f.count&&!equal59(f.owner,owner))return;
        if(!f.count)f.owner=owner;
        if(actor&&target){if(f.actor&&(f.actor!=actor||f.target!=target))return;f.actor=actor;f.target=target;}
        if(f.count==f.announcements.size()){++f.dropped;return;}
        auto& e=f.announcements[f.count++];e.tick=now;e.sample=sample;e.kind=kind;e.requested=requested;e.announced=announced;e.ack=ack;
    }
    void announcedDispatch59(std::uint64_t gesture,const Owner59& owner,std::uint64_t sample,bool dispatched,int after,std::uint64_t now) noexcept {
        auto* c=current(now);if(!c||c->gesture!=gesture||!equal59(c->feint59.owner,owner))return;
        auto& f=c->feint59;for(unsigned n=f.count;n>0;--n){auto& e=f.announcements[n-1];
            if(e.sample==sample){e.dispatched=dispatched;e.after=after;return;}}
    }
    void npc59(std::uintptr_t actor,std::uint32_t target,int defense,int blocking,int opponent,std::uint64_t now) noexcept {
        auto* c=current(now);if(!c)return;auto& f=c->feint59;
        if(!f.count||!actor||!target||f.actor!=actor||f.target!=target)return;
        auto& e=f.announcements[f.count-1];if(now<e.tick)return;
        if(!e.npcObservations){e.firstNpcTick=now;e.firstDefense=defense;e.firstBlocking=blocking;}
        ++e.npcObservations;e.lastNpcTick=now;e.lastDefense=defense;e.lastBlocking=blocking;e.lastOpponent=opponent;
    }
    void xi(bool prepare,int ms7D8,std::uint64_t now,Counters& k) noexcept {
        auto* c=current(now);if(!c){k.unjoined.fetch_add(1,std::memory_order_relaxed);return;}
        (prepare?c->msXiPrepare:c->msXiRelease)=ms7D8;c->lastTick=now;
        if(prepare&&ms7D8>0&&!c->msCounted){c->msCounted=true;k.msOpenAtPrepare.fetch_add(1,std::memory_order_relaxed);}
    }
    void admission(bool prepare,Route route,int slot,std::uint64_t now,Counters& k) noexcept {
        auto* c=current(now);if(!c){k.unjoined.fetch_add(1,std::memory_order_relaxed);return;}
        ++c->admissions;c->lastTick=now;
        if(prepare){c->prepareRoute=route;c->prepareSlot=slot;}else {c->releaseRoute=route;c->releaseSlot=slot;}
        if(route==Route::Combo&&!c->comboRouteCounted){c->comboRouteCounted=true;k.comboRoute.fetch_add(1,std::memory_order_relaxed);}
    }
    void kind5(bool ref,std::uint64_t now,Counters& k) noexcept {
        auto* c=current(now);if(!c){k.unjoined.fetch_add(1,std::memory_order_relaxed);return;}
        ++c->kind5;if(ref){++c->kind5Ref;k.msRef.fetch_add(1,std::memory_order_relaxed);}c->lastTick=now;
    }
    void factory(bool native,bool permit,std::uint64_t now,Counters& k) noexcept {
        k.msFactoryQ.fetch_add(1,std::memory_order_relaxed);
        auto* c=current(now);if(!c){k.unjoined.fetch_add(1,std::memory_order_relaxed);return;}
        ++c->factoryQ;if(native)++c->factoryNative;if(permit)++c->factoryPermit;c->lastTick=now;
    }
    void dispatch(bool ok,std::uint64_t now,Counters& k) noexcept {
        if(ok)k.msAccepted.fetch_add(1,std::memory_order_relaxed);
        auto* c=current(now);if(!c){k.unjoined.fetch_add(1,std::memory_order_relaxed);return;}
        ++c->dispatch;if(ok)++c->dispatchOk;c->lastTick=now;
    }
    void slot16(bool requested,std::uint64_t now,Counters& k) noexcept {
        if(requested)k.comboSlotOpen.fetch_add(1,std::memory_order_relaxed);
        auto* c=current(now);if(!c)return; // a phase 16 without a VR gesture is native-only, not unjoined evidence
        ++c->slot16;if(requested)++c->slotRequested;
    }
    // exactGesture: the submission's own gesture (direct route); 0 = native producer (temporal join).
    void hit(std::uint64_t exactGesture,const Hit& h,std::uint64_t now,Counters& k) noexcept {
        if(!h.ok)return;
        Chain* c=nullptr;bool exact=false;
        if(exactGesture){if(has&&cur.gesture==exactGesture){c=&cur;exact=true;}}
        else c=current(now);
        if(!h.apply){k.evalSkipped.fetch_add(1,std::memory_order_relaxed);if(c)++c->evalSkipped;return;} // evaluation, never an outcome
        const bool deathRow=h.reaction>=0&&h.deathKind>=0&&h.reaction==h.deathKind;
        if(h.death==1)k.kills.fetch_add(1,std::memory_order_relaxed);
        if(h.combo>0)k.comboAdvance.fetch_add(1,std::memory_order_relaxed);
        if(deathRow)k.deathRows.fetch_add(1,std::memory_order_relaxed);
        if(!c){k.unjoined.fetch_add(1,std::memory_order_relaxed);return;}
        if(c->feint59.target&&h.target==c->feint59.target){auto& f=c->feint59;++f.applied;
            ++(exact?f.exact:f.temporal);f.blocked+=h.blocked>0;f.perfect+=h.perfect>0;f.parry+=h.parry>0;f.master+=h.master>0;f.combo+=h.combo>0;f.comboEnd+=h.comboEnd>0;}
        ++c->applied;++(exact?c->exact:c->temporal);c->lastTick=now;
        c->blocked+=h.blocked>0;c->perfect+=h.perfect>0;c->parry+=h.parry>0;c->master+=h.master>0;
        c->combo+=h.combo>0;c->comboEnd+=h.comboEnd>0;c->death+=h.death==1;c->ko+=h.ko==1;c->flagEdges+=h.flagEdge;
        c->deathRows+=deathRow;if(h.reaction>=0)c->lastReaction=h.reaction;
    }
};
inline Counters counters;
inline Core core;
// Which optional existing observers are armed (set by their installers; read by the edge note).
inline std::atomic<bool> factoryArmed{false},slotArmed{false};
inline std::mutex mutex;
// Hooks never wait: a contended update is dropped and counted (busy), never blocks the game.
template<class F> inline bool with(F&& f) noexcept {
    std::unique_lock<std::mutex> lock(mutex,std::try_to_lock);
    if(!lock.owns_lock()){counters.busy.fetch_add(1,std::memory_order_relaxed);return false;}
    f(core);return true;
}
// Same-stack join for the D4AD94 owned call: 1483580/1483A90/90C850 nested inside it.
struct Stack {bool active=false;std::uintptr_t actor=0;};
inline thread_local Stack stack{};
struct StackScope {
    Stack saved;
    StackScope(bool owned,std::uintptr_t actor) noexcept:saved(stack){if(owned)stack={true,actor};}
    ~StackScope(){stack=saved;}
};
inline bool onStack(std::uintptr_t actor) noexcept {return stack.active&&actor&&stack.actor==actor;}
}
