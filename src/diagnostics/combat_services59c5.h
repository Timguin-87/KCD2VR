#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <chrono>
namespace kcd2vr::diagnostics::services59c5 {
// Copied scalar observations only. No worker may dereference an address here.
enum class Kind : unsigned { RaisedBegin,RaisedEnd,CommitBegin,CommitEnd,EffectBuilt,CounterBegin,CounterEnd,HitReturn };
struct Record {
    std::uint64_t sequence=0,session=0,span=0,tick=0,elapsedNs=0;
    std::uintptr_t actor=0,state=0,other=0,object=0,transaction=0,vtable=0,caller=0;
    std::uint64_t actorWuid=0;
    std::uint32_t actorId=0,targetId=0,thread=0;
    Kind kind{};int phase=-99,type=-99,zone=-99,selector=-1,flags=0,argument4=0,argument6=0;
    float strength=0,delta=0,health=0,victimStamina=0;
};
// No allocations, waiting or formatting at native boundaries. A busy/full
// journal rejects an observation and exposes the loss; gameplay never waits.
template<std::size_t N=512> class Journal {
    std::array<Record,N> data_{};
    std::atomic_flag lock_=ATOMIC_FLAG_INIT;
    std::atomic<unsigned> count_{0};
    unsigned head_=0,tail_=0;
    std::uint64_t sequence_=0;
    std::atomic<std::uint64_t> dropped_{0},nextSpan_{0};
public:
    std::uint64_t span() noexcept { return ++nextSpan_; }
    bool push(Record r) noexcept {
        if(lock_.test_and_set(std::memory_order_acquire)){++dropped_;return false;}
        bool ok=count_.load(std::memory_order_relaxed)<N;
        if(ok){r.sequence=++sequence_;data_[tail_]=r;tail_=(tail_+1)%N;count_.fetch_add(1,std::memory_order_relaxed);}
        else ++dropped_;
        lock_.clear(std::memory_order_release);return ok;
    }
    bool pop(Record& r) noexcept {
        if(!count_.load(std::memory_order_relaxed)||lock_.test_and_set(std::memory_order_acquire))return false;
        const bool ok=count_.load(std::memory_order_relaxed)>0;
        if(ok){r=data_[head_];head_=(head_+1)%N;count_.fetch_sub(1,std::memory_order_relaxed);}
        lock_.clear(std::memory_order_release);return ok;
    }
    std::uint64_t dropped() const noexcept {return dropped_.load();}
};
inline Journal<> journal;
}
