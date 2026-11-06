#pragma once
#include <atomic>
#include <cstdint>
#include <cstddef>

// These addresses are for the SHA-pinned retail image, not a generic CryEngine ABI.
// Three native callers of +4846E4 load RCX from the same +492D830 pointer slot.
namespace kcd2vr::world_query_contract {
inline constexpr std::uintptr_t kWorldSlotRva = 0x492D830ull;
inline constexpr std::uintptr_t kHelperRva = 0x4846E4ull;
inline constexpr std::uintptr_t kWorldVtableRva = 0x407FC80ull;
inline constexpr std::uintptr_t kQueryMethodRva = 0x50A3E8ull;
inline constexpr std::size_t kQueryVtableOffset = 0x118;
inline constexpr std::uintptr_t kNativeWorldLoadRvas[]{0x4838C6ull,0xD54AE9ull,0x2F364F4ull};

// A failed query is NOT an empty/clear scene. A permanent fault must stop all
// further calls this process; changing F10 modes cannot re-arm a fault storm.
enum class Result : int { Clear, Hit, Busy, Fault, Invalid, Disabled };
class LatencyBackoff59c1 {
    std::atomic<std::uint64_t> until_{0}, events_{0};
    unsigned consecutiveSlow_=0; // observed only inside Circuit's serialized leaf
public:
    bool ready(std::uint64_t now) const noexcept { return now>=until_.load(std::memory_order_acquire); }
    bool pending() const noexcept { return until_.load(std::memory_order_acquire)!=0; }
    std::uint64_t events() const noexcept { return events_.load(std::memory_order_relaxed); }
    Result observe(Result result,std::uint64_t elapsed,std::uint64_t now) noexcept {
        // Invalid data and faults must reach Circuit even if the call was slow.
        if(result!=Result::Clear&&result!=Result::Hit) return result;
        if(elapsed>8000) {
            const unsigned shift=consecutiveSlow_<3?consecutiveSlow_:3;
            if(consecutiveSlow_<3) ++consecutiveSlow_;
            until_.store(now+(500000ull<<shift),std::memory_order_release);
            events_.fetch_add(1,std::memory_order_relaxed);
            return Result::Busy; // latency is UNKNOWN coverage, never a fault or clearance
        }
        consecutiveSlow_=0;until_.store(0,std::memory_order_release);
        return result;
    }
};
class Circuit {
    std::atomic<int> blocked_{0}; // 1 native fault, 2 invalid result/type
    std::atomic_flag busy_ = ATOMIC_FLAG_INIT;
    std::atomic<std::uint64_t> calls_{0}, faults_{0};
public:
    bool enabled() const noexcept { return blocked_.load(std::memory_order_acquire)==0; }
    int reason() const noexcept { return blocked_.load(std::memory_order_acquire); }
    std::uint64_t calls() const noexcept { return calls_.load(std::memory_order_relaxed); }
    std::uint64_t faults() const noexcept { return faults_.load(std::memory_order_relaxed); }
    void invalidate() noexcept { int expected=0; blocked_.compare_exchange_strong(expected,2,std::memory_order_acq_rel); }
    template<class NativeLeaf>
    Result invoke(NativeLeaf&& leaf) noexcept {
        if(!enabled()) return Result::Disabled;
        if(busy_.test_and_set(std::memory_order_acquire)) return Result::Busy;
        if(!enabled()) { busy_.clear(std::memory_order_release); return Result::Disabled; }
        calls_.fetch_add(1,std::memory_order_relaxed);
        // The native leaf catches Windows SEH and returns Fault. No native call
        // can unwind through this portable lock owner.
        const Result result=leaf();
        if(result==Result::Fault) {
            faults_.fetch_add(1,std::memory_order_relaxed);
            int expected=0; blocked_.compare_exchange_strong(expected,1,std::memory_order_acq_rel);
        } else if(result==Result::Invalid) invalidate();
        busy_.clear(std::memory_order_release);
        return result;
    }
};
} // namespace kcd2vr::world_query_contract
