#pragma once
#include <array>
#include <cstddef>
// Caller owns the mutex, including the active native transaction. No allocation,
// expiry rejuvenation, gameplay admission or native pointers created here.
namespace kcd2vr::contact_backlog59c6 {
template<class T,std::size_t N=8> struct Queue {
 std::array<T,N> values{};std::size_t count=0;
 bool push(const T& x) noexcept {if(count==N)return false;values[count++]=x;return true;}
 bool pop(T& x) noexcept {if(!count)return false;x=values[0];for(std::size_t i=1;i<count;++i)values[i-1]=values[i];values[--count]=T{};return true;}
 template<class Same> bool contains(const T& x,Same same) const noexcept {for(std::size_t i=0;i<count;++i)if(same(values[i],x))return true;return false;}
 template<class Valid> std::size_t prune(Valid valid) noexcept {std::size_t n=0,old=count;for(std::size_t i=0;i<old;++i)if(valid(values[i]))values[n++]=values[i];for(std::size_t i=n;i<old;++i)values[i]=T{};count=n;return old-n;}
 void clear() noexcept {for(std::size_t i=0;i<count;++i)values[i]=T{};count=0;}
};
template<class T> bool sameContact(const T& a,const T& b) noexcept {
 // Retain the first recipient/attempt contact; repeated part samples do not buy
 // another slot. Different attempts or recipients can wait independently.
 return a.gesture59c3==b.gesture59c3&&a.key==b.key&&a.generation==b.generation&&
  a.epoch==b.epoch&&a.request==b.request&&a.recovery55c1==b.recovery55c1&&
  a.entityId==b.entityId&&a.weaponItem==b.weaponItem;
}
}
