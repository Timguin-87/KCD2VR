#pragma once
#include <atomic>
#include <cstdint>
// DS-F42 (after Codex F35/F36 repro): transactional bounded FIFO v2.
// v2 corrections:
// - occupancy counts CLAIMED slots too; a full ring never overwrites a
//   claimed head; admission writes only into a truly free slot;
// - take never scans PAST a claimed head (one consumer: later same-resource
//   work waits for the head's ACK);
// - settle returns a result and the drain retries a pending settle next tick
//   instead of giving up after a bounded spin;
// - cancellation (Remove cancels Create) happens transactionally: the freed
//   slot is only released after the Remove's own admission is decided.
namespace kcd2vr::constraint_op60 {
enum class Kind : unsigned char {None,Create,Drive,Remove,Cleanup};
struct Key {
    std::uint32_t id=0;std::uintptr_t buddy=0,world=0,sourceBody=0;std::uint64_t sourceGeneration=0;
    friend bool operator==(const Key& a,const Key& b) noexcept {
        return a.id==b.id&&a.buddy==b.buddy&&a.world==b.world&&a.sourceBody==b.sourceBody&&a.sourceGeneration==b.sourceGeneration;
    }
};
struct Op {
    Kind kind=Kind::None;Key key{};std::uint64_t tick=0;std::uint32_t serial=0;char reason[64]{};
};
inline const char* kindName(Kind k) noexcept {
    switch(k){case Kind::None:return "NONE";case Kind::Create:return "CREATE";case Kind::Drive:return "DRIVE";
              case Kind::Remove:return "REMOVE";default:return "CLEANUP";}
}
inline constexpr unsigned kFifo=16;
inline Op ring[kFifo]{};
inline std::atomic<bool> inFlight[kFifo]{{false},{false},{false},{false},{false},{false},{false},{false},
                                         {false},{false},{false},{false},{false},{false},{false},{false}};
inline std::atomic<std::uint32_t> head{0},tail{0};
inline std::atomic_flag gate=ATOMIC_FLAG_INIT; // bounded try-lock: never held over native calls
inline std::atomic<std::uint32_t> nextSerial{0};
inline std::atomic<std::uint64_t> opCount{0},dropCount{0},cancelCount{0},busyCount{0};
inline bool sameKey(const Op& a,const Op& b) noexcept {return a.key==b.key;}
// DS-F40: causal matching. A Create runs BEFORE the native id exists
// (key.id=0); its Remove later carries the latched id.
inline bool cancelMatch(const Op& a,const Op& b) noexcept {
    return a.key.buddy==b.key.buddy&&a.key.world==b.key.world&&a.key.sourceBody==b.key.sourceBody&&
        a.key.sourceGeneration==b.key.sourceGeneration&&(a.key.id==b.key.id||a.key.id==0||b.key.id==0);
}
inline bool occupiedAt(unsigned i) noexcept {return ring[i].kind!=Kind::None;} // claimed slots count too
inline bool admit(const Op& op) noexcept {
    if(op.kind==Kind::None||!op.tick||op.serial==0)return false;
    if(!op.key.buddy||!op.key.world)return false;
    if(!op.key.id&&op.kind!=Kind::Create)return false;
    if(gate.test_and_set(std::memory_order_acquire)){busyCount.fetch_add(1,std::memory_order_relaxed);return false;}
    bool result=false;
    unsigned cancelledSlot=kFifo;
    for(unsigned i=0;i<kFifo;++i){
        if(!occupiedAt(i)||inFlight[i].load(std::memory_order_acquire)||!cancelMatch(ring[i],op))continue;
        const Kind cur=ring[i].kind;
        if(op.kind==Kind::Remove){
            if(cur==Kind::Create)cancelledSlot=i; // decided AFTER the admission slot is secured
        }else if(op.kind==Kind::Create){
            if(cur==Kind::Remove||cur==Kind::Cleanup){cancelCount.fetch_add(1,std::memory_order_relaxed);gate.clear(std::memory_order_release);return false;}
        }else if(op.kind==Kind::Drive){
            if(cur==Kind::Create&&sameKey(ring[i],op)){dropCount.fetch_add(1,std::memory_order_relaxed);gate.clear(std::memory_order_release);return false;}
            if(cur==Kind::Remove||cur==Kind::Cleanup){gate.clear(std::memory_order_release);return false;}
        }else if(op.kind==Kind::Cleanup){
            if(cur==Kind::Create)cancelledSlot=i;
        }
    }
    unsigned slot=kFifo;
    if(!occupiedAt(tail.load(std::memory_order_relaxed)))slot=tail.load(std::memory_order_relaxed);
    else for(unsigned i=0;i<kFifo;++i)if(!occupiedAt(i)){slot=i;break;}
    if(slot>=kFifo){busyCount.fetch_add(1,std::memory_order_relaxed);gate.clear(std::memory_order_release);return false;} // full incl. claimed: explicit rejection
    ring[slot]=op;inFlight[slot].store(false,std::memory_order_release);
    if(cancelledSlot<kFifo){ring[cancelledSlot]={};cancelCount.fetch_add(1,std::memory_order_relaxed);} // transactional: the Remove is in, only now the Create dies
    tail.store((slot+1)%kFifo,std::memory_order_relaxed);
    result=true;
    gate.clear(std::memory_order_release);return result;
}
// take claims the head; it NEVER skips past a claimed head (the ACK order is
// the lifecycle order). Returns false while the head is claimed.
inline bool take(Op& out) noexcept {
    if(gate.test_and_set(std::memory_order_acquire)){busyCount.fetch_add(1,std::memory_order_relaxed);out={};return false;}
    bool result=false;
    const unsigned h=head.load(std::memory_order_relaxed);
    for(unsigned step=0;step<kFifo;++step){
        const unsigned i=(h+step)%kFifo;
        if(ring[i].kind==Kind::None)continue;
        if(inFlight[i].load(std::memory_order_acquire)){result=false;goto done;} // head claimed: wait for its ACK
        inFlight[i].store(true,std::memory_order_release);
        out=ring[i];result=true;break;
    }
done:
    gate.clear(std::memory_order_release);return result;
}
// Settle releases the claimed slot exactly once. Returns false when the gate
// stays busy (the caller keeps the pending settle and retries next tick).
inline bool settle(const Op& op,bool executed) noexcept {
    if(!op.serial||op.kind==Kind::None)return true;
    unsigned spins=0;
    while(gate.test_and_set(std::memory_order_acquire)){if(++spins>4096)return false;} // pending retry, never a lost disposition
    bool found=false;
    for(unsigned step=0;step<kFifo;++step){
        const unsigned i=(head.load(std::memory_order_relaxed)+step)%kFifo;
        if(inFlight[i].load(std::memory_order_acquire)&&ring[i].serial==op.serial&&sameKey(ring[i],op)){
            ring[i]={};inFlight[i].store(false,std::memory_order_release);
            // DS-F42: advance the head past freed tombstones with a FULL-cycle
            // stop (the old h!=tail stop stalls when the ring is full).
            std::uint32_t h=head.load(std::memory_order_relaxed);const std::uint32_t h0=h;
            while(ring[h].kind==Kind::None){h=(h+1)%kFifo;if(h==h0)break;}
            head.store(h,std::memory_order_relaxed);
            (executed?opCount:dropCount).fetch_add(1,std::memory_order_relaxed);found=true;break;
        }
    }
    gate.clear(std::memory_order_release);return found;
}
}
