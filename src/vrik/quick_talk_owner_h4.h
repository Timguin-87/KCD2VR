#pragma once
#include <atomic>
#include <cstdint>
namespace kcd2vr::quick_talk_owner_h4 {
// Input arbitration before equipment admission; never a tracked-pose or native
// action lease. Equipment already admitted to its command ring keeps that owner.
struct Stamp {std::uint64_t tick=0,body=0,request=0;std::uintptr_t player=0;unsigned mask=0;};
class Mailbox {
 std::atomic_flag lock=ATOMIC_FLAG_INIT;Stamp value{};
public:
 bool publish(Stamp s) noexcept {if(lock.test_and_set(std::memory_order_acquire))return false;
  if(!s.body||!s.request||!s.player){const auto tick=s.tick;s=value;s.tick=tick;s.mask=0;}
  value=s;lock.clear(std::memory_order_release);return true;}
 bool read(Stamp& s) noexcept {if(lock.test_and_set(std::memory_order_acquire))return false;s=value;lock.clear(std::memory_order_release);return true;}
};
inline Mailbox mailbox;
inline bool same(const Stamp&s,std::uint64_t body,std::uint64_t request,std::uintptr_t player) noexcept{return s.body==body&&s.request==request&&s.player==player&&player&&body&&request;}
inline unsigned current(const Stamp&s,std::uint64_t now,std::uint64_t body,std::uint64_t request,std::uintptr_t player) noexcept{return same(s,body,request,player)&&s.tick&&now>=s.tick&&now-s.tick<=80?s.mask&3u:0u;}
}
