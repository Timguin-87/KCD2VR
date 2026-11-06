#pragma once
// Event-owned presentation leases only: no inventory, damage or corpse-physics authority.
#include <array>
#include <cstdint>
#include <cstring>
namespace kcd2vr::gore_visibility59 {
struct Key {
 std::uint32_t target=0;std::uintptr_t entity=0,character=0,attachment=0;
 char name[64]{};
};
inline bool valid(const Key& k) noexcept {
 return k.target&&k.entity&&k.character&&k.attachment&&k.name[0]&&
  std::memchr(k.name,0,sizeof(k.name));
}
inline bool same(const Key& a,const Key& b) noexcept {
 return a.target==b.target&&a.entity==b.entity&&a.character==b.character&&
  a.attachment==b.attachment&&!std::strcmp(a.name,b.name);
}
enum class Observation { Unknown, Retired, Current };
struct Lease {
 Key key{};std::uint64_t started=0,batch=0;std::uint32_t before=0,owned=0;
 bool active=false,restorePending=false;std::uint64_t checkedAt=0;
 // A cut copy is presentation owned by this exact original lease. Cleanup of
 // the copy must be acknowledged before ANY path can restore the original.
 Key replacement{};std::uint64_t reassertUntil=0;
};
template<std::size_t Count=128> struct Owner {
 std::array<Lease,Count> leases{};unsigned activeCount=0;
 // A cosmetic timeout must never put the original head back on a live owner.
 // Inspect retirement infrequently; failed explicit cleanup keeps its own debt.
 static constexpr std::uint64_t inspectMs=30000,retryMs=250;
 static bool due(const Lease& x,std::uint64_t now,std::uint64_t period) noexcept {
  return !x.checkedAt||now<x.checkedAt||now-x.checkedAt>=period;
 }
 template<class IO> bool hide(IO& io,const Key& key,std::uint64_t now,std::uint64_t batch=0) noexcept {
  if(!valid(key)||!now)return false;
  Lease* free=nullptr;
  for(auto& x:leases){if(x.active&&same(x.key,key))return false;if(!x.active&&!free)free=&x;}
  std::uint32_t flags=0;if(!free||!io.flags(key,flags)||(flags&0x70000u))return false;
  // Exact retail 5AF5E8 / 5ADA68 sets ALL three bits0x70000, not only
  // the observed hide bit0x10000. Its false branch clears all three. Require
  // all originally clear so restoring false reproduces the exact native flags.
  // Reserve cleanup BEFORE the native setter, including applied-then-fault cases.
  *free={key,now,batch,flags,flags|0x70000u,true,true,0};++activeCount;
  const bool returned=io.visibility(key,true);std::uint32_t after=0;
  const bool acquired=returned&&io.flags(key,after)&&after==free->owned;
  if(acquired){free->restorePending=false;free->checkedAt=now;}
  return acquired;
 }
 Lease* find(const Key& key,std::uint64_t batch) noexcept {
  for(auto& x:leases)if(x.active&&x.batch==batch&&same(x.key,key))return &x;
  return nullptr;
 }
 // Reserve the original's cleanup slot before CreateAttachment/AddBinding.
 // A partially completed bind therefore never relies on a transient local.
 template<class IO> bool reserveCut(IO& io,const Key& key,std::uint64_t now,std::uint64_t batch) noexcept {
  if(!valid(key)||!now||!batch)return false;
  Lease* free=nullptr;
  for(auto& x:leases){if(x.active&&same(x.key,key))return false;if(!x.active&&!free)free=&x;}
  std::uint32_t flags=0;if(!free||!io.flags(key,flags)||(flags&0x70000u))return false;
  *free={key,now,batch,flags,flags|0x70000u,true,true,0};++activeCount;return true;
 }
 template<class IO> bool linkCut(IO& io,const Key& key,std::uint64_t batch,const Key& copy) noexcept {
  auto* x=find(key,batch);std::uint32_t flags=0;
  if(!x||!x->restorePending||valid(x->replacement)||!valid(copy)||same(key,copy)||
     key.target!=copy.target||key.entity!=copy.entity||key.character!=copy.character||
     !io.flags(copy,flags))return false;
  x->replacement=copy;return true; // captured BEFORE binding or revealing it
 }
 template<class IO> bool activateCut(IO& io,const Key& key,std::uint64_t batch,std::uint64_t now) noexcept {
  auto* x=find(key,batch);std::uint32_t original=0,copy=0;
  if(!x||!x->restorePending||!valid(x->replacement)||!io.flags(key,original)||original!=x->before||
     !io.flags(x->replacement,copy)||(copy&0x70000u)!=0x70000u)return false;
  // Both pieces were collected and bound while the replacement stayed hidden.
  // Hide the original before revealing the copy. Failures retain cleanup debt.
  if(!io.visibility(key,true)||!io.flags(key,original)||original!=x->owned)return false;
  if(!io.visibility(x->replacement,false)||!io.flags(x->replacement,copy)||(copy&0x70000u))return false;
  x->restorePending=false;x->checkedAt=now;
  x->reassertUntil=now>UINT64_MAX-10000?UINT64_MAX:now+10000;return true;
 }
 // This is a reassertion of OUR SAME lease, never a fresh acquisition. Native
 // equipment may clear the hide bits; preserve the original flags and batch.
 template<class IO> bool reassert(IO& io,const Key& key,std::uint64_t batch,std::uint64_t now) noexcept {
  auto* x=find(key,batch);std::uint32_t flags=0;
  if(!x||x->restorePending)return false;
  if(valid(x->replacement)){
   if(now<x->started||now>x->reassertUntil)return false;
   std::uint32_t copy=0;if(!io.flags(x->replacement,copy)||(copy&0x70000u))return false;
  }
  const auto seen=io.observe(key,flags);
  if(seen==Observation::Retired){
   if(valid(x->replacement)){x->restorePending=true;x->checkedAt=now;retire(io,*x);}
   else release(io,*x,false);
   return false;
  }
  if(seen!=Observation::Current)return false;
  if(flags==x->owned){x->checkedAt=now;return true;}
  if(flags!=x->before)return false; // another flags owner, do not overwrite it
  (void)io.visibility(key,true);
  if(!io.flags(key,flags)||flags!=x->owned)return false;
  x->checkedAt=now;return true; // observed applied-then-fault is acknowledged
 }
 template<class IO> bool retireCopy(IO& io,Lease& x) noexcept {
  if(!valid(x.replacement))return true;
  std::uint32_t flags=0;auto seen=io.observe(x.replacement,flags);
  if(seen==Observation::Unknown)return false;
  if(seen==Observation::Retired)return true;
  if((flags&0x70000u)==0x70000u)return true;
  (void)io.visibility(x.replacement,true);
  seen=io.observe(x.replacement,flags);
  return seen==Observation::Retired||(seen==Observation::Current&&(flags&0x70000u)==0x70000u);
 }
 template<class IO> void service(IO& io,std::uint64_t now,bool force=false) noexcept {
  if(!activeCount)return; // ordinary gameplay performs no scan or native reads
  for(auto& x:leases){
   if(!x.active)continue;
   const bool newRestore=force&&!x.restorePending;
   if(force)x.restorePending=true;
   if(!newRestore&&!due(x,now,x.restorePending?retryMs:inspectMs))continue;
   x.checkedAt=now;
   if(x.restorePending)retire(io,x);
   else {
    std::uint32_t flags=0;const auto observed=io.observe(x.key,flags);
    // Maintenance runs BEFORE the cut watcher. A native clear-to-before
    // during its exact active watch is a reassert request, not hand-back.
    // Unknown replacement reads preserve the active owner for that watcher;
    // foreign flags, a hidden/retired copy, force, and expired watches do not.
    if(observed==Observation::Current&&flags==x.before&&valid(x.replacement)&&
       now>=x.started&&now<=x.reassertUntil){
     std::uint32_t copy=0;const auto copyState=io.observe(x.replacement,copy);
     if(copyState==Observation::Unknown)continue;
     if(copyState==Observation::Current&&!(copy&0x70000u)){
      (void)reassert(io,x.key,x.batch,now);continue;
     }
    }
    if(observed==Observation::Retired||(observed==Observation::Current&&flags!=x.owned)){
     if(valid(x.replacement)){x.restorePending=true;retire(io,x);}
     else release(io,x,false);
    }
   }
  }
 }
 template<class IO> void restoreKey(IO& io,const Key& key,std::uint64_t batch,std::uint64_t now) noexcept {
  auto* x=find(key,batch);if(!x)return;
  const bool newlyPending=!x->restorePending;x->restorePending=true;
  if(newlyPending||due(*x,now,retryMs)){x->checkedAt=now;retire(io,*x);}
 }
 // A refused event can undo only the leases it acquired in that exact batch.
 // Every native restore still revalidates the saved complete attachment key.
 template<class IO> void restoreBatch(IO& io,std::uint32_t target,std::uintptr_t entity,
        std::uintptr_t character,std::uint64_t batch,std::uint64_t now) noexcept {
  if(!batch)return; // lifetime timestamps never identify an acquisition batch
  for(auto& x:leases)if(x.active&&x.batch==batch&&x.key.target==target&&
      x.key.entity==entity&&x.key.character==character){
   const bool newRestore=!x.restorePending;x.restorePending=true;
   if(newRestore||due(x,now,retryMs)){x.checkedAt=now;retire(io,x);}
  }
 }
 template<class IO> void retire(IO& io,Lease& x) noexcept {
  if(!retireCopy(io,x))return; // unknown/failed copy hide keeps exact cleanup debt
  // An unavailable native observation is not proof the object disappeared.
  std::uint32_t flags=0;auto observed=io.observe(x.key,flags);
  if(observed==Observation::Unknown)return;
  if(observed==Observation::Retired){release(io,x,false);return;}
  if(flags==x.before){release(io,x,true);return;}
  if(flags!=x.owned){release(io,x,false);return;}
  io.visibility(x.key,false);
  observed=io.observe(x.key,flags);
  // Actual original flags acknowledge even an applied-then-fault setter.
  if(observed==Observation::Retired)release(io,x,false);
  else if(observed==Observation::Current&&flags!=x.owned)release(io,x,flags==x.before);
 }
 template<class IO> void release(IO& io,Lease& x,bool restored) noexcept {
  io.receipt(x.key,restored);x={};--activeCount;
 }
};
}
