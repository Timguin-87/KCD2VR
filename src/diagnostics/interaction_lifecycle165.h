#pragma once
// Diagnostic identities only. Never grants gameplay/physics ownership.
#include <array>
#include <atomic>
#include <cstdint>
namespace kcd2vr::diagnostics::lifecycle165 {
enum class Kind {PickupRequest, PickupReturn, AttachResult, HandEnter, HandLeave,
    AlchemyStart, AlchemyState, AlchemyUse, AlchemyTarget, RemoveRequest, RemoveAcceptedPath,
    ConstraintRequest, ConstraintReturn, PhysicParamRequest168, PhysicParamReturn168, ActionRequest, ActionReturn, HandSample, GripSample, BodySample, PhysicSubmit, PhysicActionReturn, AttachRequest, ObjectPose, CarrySnapshot59c7g};
inline const char* name(Kind k) noexcept {
    constexpr const char* names[]{"PICKUP_REQUEST","PICKUP_RETURN","ATTACH_RESULT","HAND_ENTER","HAND_LEAVE",
        "ALCHEMY_START","ALCHEMY_STATE","ALCHEMY_USE","ALCHEMY_TARGET","REMOVE_REQUEST","REMOVE_ACCEPTED_PATH",
        "CONSTRAINT_REQUEST","CONSTRAINT_RETURN","SET_PHYSIC_PARAMS_REQUEST","SET_PHYSIC_PARAMS_RETURN","ACTION_REQUEST","ACTION_RETURN","HAND_SAMPLE","GRIP_SAMPLE","BODY_SAMPLE","PHYSICS_SUBMIT","PHYSICS_ACTION_RETURN","ATTACH_REQUEST","OBJECT_POSE","CARRY_SNAPSHOT"};
    return names[static_cast<unsigned>(k)];
}
struct Event {
    Kind kind{};
    std::uint64_t session=0,tick=0,call=0,key=0,meta=0;
    std::uintptr_t entity=0,item=0,controller=0,body=0,bodyVft=0;
    std::uint32_t actor=0,target=0,rva=0,eventHash=0;
    int side=-1,result=-1,state=-1,previousState=-1,action=-1;
    std::uint64_t related=0,sourceTick=0,returnBits=0;
    int constraintId=-1,part0=-1,part1=-1,reason=0;
    unsigned valid=0; // bit 0: native/copy payload; bit 1: live hand/tracking
    float grip=0,point[3]{},orientation[4]{},worldMatrix59c1[12]{};
    std::uintptr_t buddy=0;
    char label[64]{};
};
// Nonblocking bounded producers; a missed record is counted, never hidden.
struct Queue {
    std::atomic_flag lock=ATOMIC_FLAG_INIT;
    std::atomic<std::uint64_t> dropped{0};
    std::array<Event,512> events{};
    unsigned head=0,tail=0;
    bool push(const Event& e) noexcept {
        if(lock.test_and_set(std::memory_order_acquire)){++dropped;return false;}
        const auto next=(head+1)%events.size();
        const bool ok=next!=tail;
        if(ok){events[head]=e;head=static_cast<unsigned>(next);}else ++dropped;
        lock.clear(std::memory_order_release);return ok;
    }
    bool pop(Event& e) noexcept {
        if(lock.test_and_set(std::memory_order_acquire))return false;
        const bool ok=head!=tail;
        if(ok){e=events[tail];tail=(tail+1)%events.size();}
        lock.clear(std::memory_order_release);return ok;
    }
};
struct Episode {
    std::uint64_t session=0,id=0,key=0,enterTick=0,heldMs=0,handVisits=0;
    std::uint32_t target=0;
    std::uintptr_t entity=0;
    bool requested=false,returned=false,accepted=false,attached=false,held=false,released=false,removeRequested=false,removeAccepted=false;
    bool constraintRequest=false,constraintReturn=false;
    std::uint64_t objectPoses59c1=0,objectAbsent59c1=0;
    std::uint64_t eventMask58c2=0,handSamples58c2=0,bodySamples58c2=0;
    int lastConstraintResult58c2=-1,lastConstraintId58c2=-1;
    std::uintptr_t observedBody58c2=0;
    bool paramAddRequest168=false,paramAddReturn168=false,paramRemoveRequest168=false,paramRemoveReturn168=false;
};
struct Ledger {
    std::array<Episode,64> episodes{};
    std::uint64_t serial=0,overflow=0,conflicts=0;
    Episode* accept(const Event& e) noexcept {
        if(!e.session||(!e.key&&!e.target))return nullptr;
        Episode* match=nullptr;
        for(auto& p:episodes){
            if(p.session!=e.session)continue;
            const bool sameKey=e.key&&p.key==e.key;
            const bool sameEntity=e.target&&p.target==e.target&&e.entity&&p.entity==e.entity;
            if(!sameKey&&!sameEntity)continue;
            // Entity IDs/pointers can be recycled. Never join contradictory keys.
            if((sameEntity&&e.key&&p.key&&e.key!=p.key)||(sameKey&&e.target&&p.target&&(e.target!=p.target||e.entity!=p.entity))){++conflicts;return nullptr;}
            if(match&&match!=&p){++conflicts;return nullptr;}
            match=&p;
        }
        if(!match){
            for(auto& p:episodes)if(!p.session){match=&p;break;}
            if(!match){++overflow;return nullptr;}
            match->session=e.session;match->id=++serial;
        }
        auto& p=*match;
        p.eventMask58c2|=1ull<<static_cast<unsigned>(e.kind);
        if(e.kind==Kind::ObjectPose){if(e.valid&4)++p.objectPoses59c1;else if(e.result==0)++p.objectAbsent59c1;}
        if(e.kind==Kind::HandSample)++p.handSamples58c2;
        if(e.kind==Kind::BodySample){++p.bodySamples58c2;p.observedBody58c2=e.body;}
        if(e.kind==Kind::ConstraintReturn)p.lastConstraintResult58c2=e.result; // raw return, not accepted physics
        if(e.constraintId>=0)p.lastConstraintId58c2=e.constraintId;
        if(e.key)p.key=e.key;
        if(e.target){p.target=e.target;p.entity=e.entity;}
        switch(e.kind){
        case Kind::PickupRequest:p.requested=true;break;
        case Kind::PickupReturn:p.returned=true;p.accepted=e.result==1;break;
        case Kind::AttachResult:p.attached|=e.result==1&&e.reason!=1;break;
        case Kind::HandEnter:if(!p.held){p.enterTick=e.tick;++p.handVisits;}p.held=true;break;
        case Kind::HandLeave:if(p.held&&e.tick>=p.enterTick)p.heldMs+=e.tick-p.enterTick;p.released=true;p.held=false;break;
        case Kind::RemoveRequest:p.removeRequested=true;break;
        case Kind::RemoveAcceptedPath:p.removeAccepted=true;break;
        case Kind::ConstraintRequest:p.constraintRequest=true;break;
        case Kind::ConstraintReturn:p.constraintReturn=true;break;
        case Kind::PhysicParamRequest168:if(e.action==10)p.paramAddRequest168=true;else if(e.action==11)p.paramRemoveRequest168=true;break;
        case Kind::PhysicParamReturn168:if(e.action==10)p.paramAddReturn168=true;else if(e.action==11)p.paramRemoveReturn168=true;break;
        default:break;
        }
        return match;
    }
};
struct WatchIndex {
    // Coarse rejection only. A hit still needs exact session/pointer/entity ID.
    // Avoid native memory reads for the many unrelated physics getter calls.
    std::atomic<std::uint64_t> hints{0};
    static std::uint64_t bit(std::uintptr_t p) noexcept {return 1ull<<(((p>>4)^(p>>11))&63);}
    bool possible(std::uintptr_t p) const noexcept {return (hints.load(std::memory_order_acquire)&bit(p))!=0;}
    std::atomic_flag lock=ATOMIC_FLAG_INIT;
    std::atomic<std::uint64_t> lost{0};
    std::array<Event,64> entries{};
    unsigned cursor59c1=0; // native-frame consumer only
    std::size_t snapshot59c1(std::uint64_t session,std::uint64_t now,std::array<Event,4>& out) noexcept {
        if(lock.test_and_set(std::memory_order_acquire)){++lost;return 0;}
        std::size_t count=0;
        for(unsigned n=0;n<entries.size()&&count<out.size();++n){
            const auto& e=entries[cursor59c1++%entries.size()];
            if(e.session==session&&e.entity&&e.target&&now>=e.tick&&now-e.tick<=15000)out[count++]=e;
        }
        lock.clear(std::memory_order_release);return count;
    }
    bool access(Event& e,bool insert) noexcept {
        if(!e.session||!e.entity||!e.target)return false;
        if(lock.test_and_set(std::memory_order_acquire)){++lost;return false;}
        Event* found=nullptr;Event* free=nullptr;
        for(auto& w:entries){
            if(w.session!=e.session){if(!free)free=&w;continue;}
            if(w.entity==e.entity&&w.target==e.target){found=&w;break;}
        }
        bool ok=false;
        if(found){
            if(e.key&&found->key&&e.key!=found->key){++lost;}
            else {if(insert)found->tick=e.tick;
                if(insert&&e.key){found->key=e.key;found->item=e.item;found->meta=e.meta;}
                if(!e.key){e.key=found->key;e.item=found->item;e.meta=found->meta;}ok=true;}
        }else if(insert&&free){*free=e;ok=true;}
        else if(insert)++lost;
        if(ok&&insert)hints.fetch_or(bit(e.entity),std::memory_order_release);
        lock.clear(std::memory_order_release);return ok;
    }
};
// Stack address + capture generation prevents a later callback from borrowing
// the identity of a previous call on the same thread. Bounded nesting, no heap.
template<std::size_t N=8> struct Scopes {
    struct Slot {std::uintptr_t stack=0;Event event{};};
    std::array<Slot,N> slots{};
    bool put(std::uintptr_t stack,Event e) noexcept {
        erase(stack);
        for(auto& s:slots)if(!s.stack||s.event.session!=e.session){s={stack,e};return true;}
        return false;
    }
    Event* find(std::uintptr_t stack,std::uint64_t session) noexcept {
        for(auto& s:slots)if(s.stack==stack&&s.event.session==session)return &s.event;
        return nullptr;
    }
    void erase(std::uintptr_t stack) noexcept {for(auto& s:slots)if(s.stack==stack)s={};}
};
} // namespace
