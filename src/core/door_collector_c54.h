#pragma once
#include <atomic>
#include <cstdint>
namespace kcd2vr::door_collector_c54 {
// One exact native 63F9AC invocation, not the last candidate in its loop.
struct Scope {std::uintptr_t interactor=0,nativePlayer=0,policy=0;std::uint64_t serial=0;std::uint32_t target=0;std::uintptr_t frame=0;};
inline thread_local Scope current{};
inline std::atomic<std::uint64_t> next{0};
inline std::uint64_t allocate() noexcept {
 auto n=next.load(std::memory_order_relaxed);
 for(unsigned i=0;i<4;++i){if(n==UINT64_MAX)return 0;
  if(next.compare_exchange_weak(n,n+1,std::memory_order_acq_rel))return n+1;}
 return 0;
}
inline bool matches(std::uintptr_t interactor,std::uintptr_t player,std::uintptr_t policy,std::uint64_t serial=0) noexcept {
 return current.serial&&current.interactor==interactor&&current.nativePlayer==player&&current.policy==policy&&(!serial||current.serial==serial);
}
}
