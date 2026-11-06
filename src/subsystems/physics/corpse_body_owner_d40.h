#pragma once
#include "core/snapshot_mailbox.h"
#include <atomic>
#include <cstdint>
#include <limits>

// Shared ownership for the existing corpse velocity and typed-constraint
// writers. A reservation is NOT native body/part qualification: each caller
// must still prove its original current entity, body and part before a write.
// Hand and impale lifetimes use separate slots so queued constraint cleanup
// cannot be released by the game-thread impale display ending.
namespace kcd2vr::corpse_body_owner_d40 {
enum class Slot : unsigned { RightHand, LeftHand, ImpaleServo, ImpaleConstraint, Count };
struct Key {
 Slot slot=Slot::Count;
 std::uintptr_t entity=0,body=0;
 std::uint64_t serial=0,epoch=0;
 int part=-1;
 bool wholeEntity=false; // D72 native Entity:SetVelocity has one owner for the entire corpse, not a guessed part.
};
inline bool valid(const Key& k) noexcept {
 return unsigned(k.slot)<unsigned(Slot::Count)&&k.entity&&k.body&&k.serial&&k.epoch&&(k.wholeEntity?k.part==-1:k.part>=0);
}
inline bool same(const Key& a,const Key& b) noexcept {
 return a.slot==b.slot&&a.entity==b.entity&&a.body==b.body&&a.serial==b.serial&&a.epoch==b.epoch&&a.part==b.part&&a.wholeEntity==b.wholeEntity;
}
inline bool hand(Slot s) noexcept {return s==Slot::RightHand||s==Slot::LeftHand;}
inline bool conflicts(const Key& a,const Key& b) noexcept {
 if(a.entity!=b.entity&&a.body!=b.body)return false;
 if(a.wholeEntity||b.wholeEntity)return true;
 // An entity changing bodies is a transition, not another independent victim.
 if(a.entity!=b.entity||a.body!=b.body||a.epoch!=b.epoch)return true;
 if(hand(a.slot)&&hand(b.slot))return a.slot==b.slot||a.part==b.part;
 if(hand(a.slot)!=hand(b.slot))return true;
 // The same original impale may transfer from its servo to its typed joint.
 return a.serial!=b.serial||a.part!=b.part;
}
struct Lease {Key key{};std::uint64_t incarnation=0;};
class Claims {
 struct Entry {core::SnapshotMailbox<Key> key;std::atomic<std::uint64_t> active{0};};
 Entry entries_[unsigned(Slot::Count)];
 std::atomic_flag admitting_=ATOMIC_FLAG_INIT;
 std::uint64_t next_=0;
 bool compatible(const Key& key) const noexcept {
  for(unsigned i=0;i<unsigned(Slot::Count);++i){
   const auto before=entries_[i].active.load(std::memory_order_acquire);
   if(!before)continue;
   Key other{};
   if(!entries_[i].key.read(other))return false;
   const auto after=entries_[i].active.load(std::memory_order_acquire);
   if(before!=after)return false; // retry on another frame, never guess empty
   if(conflicts(key,other))return false;
  }
  return true;
 }
public:
 bool current(const Lease& lease) const noexcept {
  return lease.incarnation&&valid(lease.key)&&
   entries_[unsigned(lease.key.slot)].active.load(std::memory_order_acquire)==lease.incarnation;
 }
 bool acquire(Lease& lease,const Key& key) noexcept {
  if(!valid(key))return false;
  if(lease.incarnation)return same(lease.key,key)&&current(lease);
  if(admitting_.test_and_set(std::memory_order_acquire))return false;
  struct Unlock {std::atomic_flag& flag;~Unlock(){flag.clear(std::memory_order_release);}} unlock{admitting_};
  auto& entry=entries_[unsigned(key.slot)];
  if(entry.active.load(std::memory_order_acquire)||!compatible(key)||next_==std::numeric_limits<std::uint64_t>::max())return false;
  if(!entry.key.publish(key))return false;
  const auto token=++next_;
  lease={key,token};entry.active.store(token,std::memory_order_release);return true;
 }
 // No admission lock is required to retire one's original incarnation. A stale
 // release cannot clear a later slot user, and cleanup cannot wait on a reader.
 void release(Lease& lease) noexcept {
  if(lease.incarnation&&valid(lease.key)){
   auto expected=lease.incarnation;
   (void)entries_[unsigned(lease.key.slot)].active.compare_exchange_strong(expected,0,std::memory_order_acq_rel);
  }
  lease={};
 }
 bool handAvailable(int side,std::uintptr_t entity,std::uintptr_t body,int part,std::uint64_t epoch) noexcept {
  if(side<0||side>1||!entity||!body||part<0||!epoch)return false;
  if(admitting_.test_and_set(std::memory_order_acquire))return false;
  struct Unlock {std::atomic_flag& flag;~Unlock(){flag.clear(std::memory_order_release);}} unlock{admitting_};
  return compatible({side?Slot::LeftHand:Slot::RightHand,entity,body,1,epoch,part});
 }
};
inline Claims claims;
}
