#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <type_traits>

namespace kcd2vr::mutual_admission_t25 {
// A transient, metadata-only admission transaction. It never grants native
// eligibility and never retires another subsystem's persistent native debt.
enum class Kind : unsigned char { Throw=1, Reload=2, Door=3 };
struct Token {std::uint64_t value=0;int side=-1;};
static_assert(std::is_trivially_copyable_v<Token> && std::is_trivially_destructible_v<Token>);
inline std::array<std::atomic<std::uint64_t>,2> active{};
// H41 mirrors only the ORIGINAL H40 claim serial. A transient transaction may
// finish while Lua still owns a clip; that outstanding hand is not free.
inline std::array<std::atomic<std::uint64_t>,2> doorClaimsH41{};
// D72 real world-pickup hold, retained until actual stow/drop retirement. This
// is a separate family from ammunition; no fake ammo claim is published.
inline std::array<std::atomic<std::uint64_t>,2> pickupClaimsD72{};
inline std::atomic<std::uint64_t> next{0};
inline constexpr std::uint64_t finishing=std::uint64_t(1)<<63;
inline constexpr std::uint64_t maxSerial=(finishing-1)>>2;
using Publisher=void(*)(void*) noexcept;
inline bool valid(const Token& t) noexcept {
 return t.side>=0&&t.side<2&&t.value&&!(t.value&finishing)&&(t.value&3);
}
inline Token TryBegin(int side,Kind kind) noexcept {
 if(side<0||side>1||(kind!=Kind::Throw&&kind!=Kind::Reload&&kind!=Kind::Door)||
    doorClaimsH41[side].load(std::memory_order_acquire)||pickupClaimsD72[side].load(std::memory_order_acquire))return {};
 auto prior=next.load(std::memory_order_acquire);
 // Exactly one allocation CAS and one slot CAS: contention refuses rather
 // than retrying/spinning. Burned serials are never reused or wrapped.
 if(prior>=maxSerial||!next.compare_exchange_strong(prior,prior+1,std::memory_order_acq_rel))return {};
 const auto value=((prior+1)<<2)|static_cast<unsigned>(kind);std::uint64_t empty=0;
 if(!active[side].compare_exchange_strong(empty,value,std::memory_order_acq_rel))return {};
 if(doorClaimsH41[side].load(std::memory_order_acquire)||pickupClaimsD72[side].load(std::memory_order_acquire)){
    auto own=value;(void)active[side].compare_exchange_strong(own,0,std::memory_order_acq_rel);return {};
 }
 return {value,side};
}
inline bool RetainPickupD72(int side,std::uint64_t original) noexcept {
 if(side<0||side>1||!original||active[side].load(std::memory_order_acquire)||
    doorClaimsH41[side].load(std::memory_order_acquire))return false;
 std::uint64_t empty=0;if(!pickupClaimsD72[side].compare_exchange_strong(empty,original,std::memory_order_acq_rel))return false;
 // Pair the post-CAS check with TryBegin's own post-CAS check. A concurrent
 // peer can make both admissions refuse, never make both mutate the hand.
 if(active[side].load(std::memory_order_acquire)||doorClaimsH41[side].load(std::memory_order_acquire)){
    (void)pickupClaimsD72[side].compare_exchange_strong(original,0,std::memory_order_acq_rel);return false;
 }
 return true;
}
inline bool RetirePickupD72(int side,std::uint64_t original) noexcept {
 return side>=0&&side<2&&original&&pickupClaimsD72[side].compare_exchange_strong(original,0,std::memory_order_acq_rel);
}
// Called inside the Door transaction after H40 has reserved this exact serial.
// No native or Lua operation runs while this metadata transaction is held.
inline bool RetainDoorH41(const Token& t,std::uint64_t original) noexcept {
 if(!valid(t)||(t.value&3)!=static_cast<unsigned>(Kind::Door)||!original||
    active[t.side].load(std::memory_order_acquire)!=t.value)return false;
 std::uint64_t empty=0;return doorClaimsH41[t.side].compare_exchange_strong(empty,original,std::memory_order_acq_rel);
}
inline bool RetireDoorH41(int side,std::uint64_t original) noexcept {
 if(side<0||side>1||!original)return false;
 return doorClaimsH41[side].compare_exchange_strong(original,0,std::memory_order_acq_rel);
}
// Every normal, refused and partially-mutated exit publishes OWN persistent
// exclusion before unlocking. Publisher must be bounded atomic/POD work only:
// no native call/read, Lua, waiting, destruction, new admission or cleanup.
// Failed mailbox delivery MUST already have a conservative raw busy/debt bit.
// Token is POD so a real Windows __finally can call Finish after native SEH.
// C++ RAII alone under /EHsc is NOT a substitute for that native fault boundary.
inline bool Finish(Token& t,Publisher publish,void* context) noexcept {
 if(!valid(t)||!publish)return false;
 auto expected=t.value;
 if(!active[t.side].compare_exchange_strong(expected,t.value|finishing,std::memory_order_acq_rel))return false;
 publish(context);
 active[t.side].store(0,std::memory_order_release);t={};return true;
}
}
