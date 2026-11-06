#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace kcd2vr::pose_restore_debt_r96 {
// C37 is the positively qualified native player lifetime. Pose/calibration
// epochs, menu preferences and an observed loading screen are not substitutes.
struct Owner {
 std::uintptr_t character=0,skeleton=0,pose=0;
 std::uint64_t playerLifetime=0;
 std::uint32_t count=0;
};
inline bool valid(const Owner& o) noexcept {
 return o.character&&o.skeleton&&o.pose&&o.playerLifetime&&o.count;
}
inline bool sameNativeOwner(const Owner& a,const Owner& b) noexcept {
 return a.character==b.character&&a.playerLifetime==b.playerLifetime;
}
inline bool sameBuffer(const Owner& a,const Owner& b) noexcept {
 return sameNativeOwner(a,b)&&a.skeleton==b.skeleton&&a.pose==b.pose&&a.count==b.count;
}
enum class Resolution : unsigned { NoDebt, Pending, RestoredReadback, ReplacedNativeOwner };

template<class Value,std::size_t Capacity> struct Debt {
 Owner owner{};
 std::array<Value,Capacity> original{};
 std::array<bool,Capacity> failed{};
 bool active=false;

 void remember(const Owner& who,const Value* before,const std::array<bool,Capacity>& unsuccessful) noexcept {
  // An active obligation is never overwritten by a setting/reset request.
  if(active)return;
  owner=who;failed=unsuccessful;active=true;
  if(!before||who.count>Capacity)return;
  for(std::size_t j=0;j<who.count;++j)if(failed[j])original[j]=before[j];
 }
 Resolution observe(const Owner& now,const Value* untouched,bool qualified) noexcept {
  if(!active)return Resolution::NoDebt;
  if(!qualified||!untouched||!valid(owner)||!valid(now)||owner.count>Capacity||now.count>Capacity)
   return Resolution::Pending;
  // CPoseData addresses rotate within one actor. A different pose buffer is
  // not evidence that the previous buffer's failed restoration disappeared.
  if(!sameNativeOwner(owner,now)){
   *this={};return Resolution::ReplacedNativeOwner;
  }
  if(!sameBuffer(owner,now))return Resolution::Pending;
  bool any=false;
  for(std::size_t j=0;j<owner.count;++j)if(failed[j]){
   any=true;
   if(std::memcmp(&original[j],&untouched[j],sizeof(Value))!=0)return Resolution::Pending;
  }
  if(!any)return Resolution::Pending;
  *this={};return Resolution::RestoredReadback;
 }
};
}
