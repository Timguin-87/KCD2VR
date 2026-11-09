#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <cstddef>
namespace kcd2vr::diagnostic_trace {
// Bounded MPSC queue. Hot producers never wait, allocate, open a file or format time.
// Dropped records are observable. The single mod-worker consumer performs disk I/O.
template<std::size_t Capacity=256,std::size_t MessageSize=2048>
class RecordQueue {
    static_assert(Capacity>0&&MessageSize>=2,"Invalid diagnostic queue capacity");
public:
    struct Record {std::uint64_t filetime=0;char message[MessageSize]{};};
    bool Push(const char* text,std::uint64_t stamp) noexcept {
        if(!text||!text[0])return false;
        if(lock_.test_and_set(std::memory_order_acquire)){dropped_.fetch_add(1);return false;}
        if(size_==Capacity){lock_.clear(std::memory_order_release);dropped_.fetch_add(1);return false;}
        auto& r=records_[tail_];r.filetime=stamp;
        std::size_t n=0;while(n+1<MessageSize&&text[n]){r.message[n]=text[n];++n;}r.message[n]=0;
        if(text[n])truncated_.fetch_add(1,std::memory_order_relaxed);
        queued_.fetch_add(1,std::memory_order_relaxed);
        tail_=(tail_+1)%Capacity;++size_;lock_.clear(std::memory_order_release);return true;
    }
    // R154: status rows (WriteStatus: few, never per frame) must not vanish behind a producer or the worker that
    // holds the flag for the microseconds of one copy. The 8 Nov log lost its "R141 TUTORIALS" give-back row that
    // way (34 single-record drops in 20 min of F2, never a burst = contention, not a full queue). Bounded: at most
    // `spins` try-lock attempts, a full queue is still a drop. Hot producers keep the one-shot Push above.
    bool PushSpin(const char* text,std::uint64_t stamp,unsigned spins=64) noexcept {
        if(!text||!text[0])return false;
        for(unsigned i=0;i<spins;++i){
            if(lock_.test_and_set(std::memory_order_acquire))continue;
            if(size_==Capacity){lock_.clear(std::memory_order_release);dropped_.fetch_add(1);return false;}
            auto& r=records_[tail_];r.filetime=stamp;
            std::size_t n=0;while(n+1<MessageSize&&text[n]){r.message[n]=text[n];++n;}r.message[n]=0;
            if(text[n])truncated_.fetch_add(1,std::memory_order_relaxed);
            queued_.fetch_add(1,std::memory_order_relaxed);
            tail_=(tail_+1)%Capacity;++size_;lock_.clear(std::memory_order_release);return true;
        }
        dropped_.fetch_add(1);return false;
    }
    bool Pop(Record& out) noexcept {
        if(lock_.test_and_set(std::memory_order_acquire))return false;
        if(!size_){lock_.clear(std::memory_order_release);return false;}
        out=records_[head_];head_=(head_+1)%Capacity;--size_;lock_.clear(std::memory_order_release);return true;
    }
    std::uint64_t Dropped() const noexcept {return dropped_.load(std::memory_order_relaxed);}
    std::uint64_t Queued() const noexcept {return queued_.load(std::memory_order_relaxed);}
    std::uint64_t Truncated() const noexcept {return truncated_.load(std::memory_order_relaxed);}
private:
    std::array<Record,Capacity> records_{};
    std::size_t head_=0,tail_=0,size_=0;
    std::atomic_flag lock_=ATOMIC_FLAG_INIT;
    std::atomic<std::uint64_t> dropped_{0},queued_{0},truncated_{0};
};
}
