#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <type_traits>

namespace kcd2vr::core {
// Fixed-capacity, race-free publication. Readers pin a slot before copying;
// writers never modify the published slot or a pinned older slot. A busy
// producer cannot turn an already published coherent value into an empty read.
// No heap allocation, OS mutex, unbounded spin, or non-atomic seqlock payload.
template<class T, unsigned N=3> class SnapshotMailbox {
    static_assert(N>=3 && std::is_trivially_copyable<T>::value, "POD snapshots and >=3 slots required");
    static constexpr std::uint32_t writing=0x80000000u;
    struct Slot { mutable std::atomic<std::uint32_t> users{0}; T value{}; };
    std::array<Slot,N> slots_{};
    std::atomic<unsigned> published_{N};
    std::atomic_flag producer_=ATOMIC_FLAG_INIT;
public:
    bool publish(const T& value) noexcept {
        if(producer_.test_and_set(std::memory_order_acquire))return false;
        const unsigned current=published_.load(std::memory_order_acquire);
        bool done=false;
        for(unsigned d=1;d<=N;++d){
            const unsigned i=(current+d)%N; if(i==current)continue;
            std::uint32_t zero=0;
            if(!slots_[i].users.compare_exchange_strong(zero,writing,std::memory_order_acq_rel))continue;
            slots_[i].value=value;
            slots_[i].users.store(0,std::memory_order_release);
            published_.store(i,std::memory_order_release);done=true;break;
        }
        producer_.clear(std::memory_order_release);return done;
    }
    bool read(T& out) const noexcept {
        for(unsigned attempt=0;attempt<4;++attempt){
            const unsigned i=published_.load(std::memory_order_acquire);if(i>=N)return false;
            auto n=slots_[i].users.load(std::memory_order_acquire);
            if(n>=writing-1)continue;
            if(!slots_[i].users.compare_exchange_strong(n,n+1,std::memory_order_acq_rel))continue;
            if(published_.load(std::memory_order_acquire)!=i){slots_[i].users.fetch_sub(1,std::memory_order_release);continue;}
            out=slots_[i].value;slots_[i].users.fetch_sub(1,std::memory_order_release);return true;
        }
        return false;
    }
};
}
