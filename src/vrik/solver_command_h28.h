#pragma once
#include "../native_weapon_body_t4.h"
namespace kcd2vr::solver_command_h28 {
namespace body=native_weapon_body_t4;
struct Stamp {
 body::Owner owner{};
 std::uint64_t worldGeneration=0,bodyGeneration=0,colliderLease=0,trackingEpoch=0,cancelGeneration=0,
  admissionSerial=0,sourceSample=0,sourceTick=0,observedTick=0;
 std::uintptr_t body=0,foreign=0;int bodyId=-1,foreignType=-1;
};
inline bool fresh(std::uint64_t tick,std::uint64_t now) noexcept {return tick&&now>=tick&&now-tick<=80;}
inline bool empty(const Stamp& s) noexcept {
 return s.owner==body::Owner{}&&!s.worldGeneration&&!s.bodyGeneration&&!s.colliderLease&&!s.trackingEpoch&&!s.cancelGeneration&&
  !s.admissionSerial&&!s.sourceSample&&!s.sourceTick&&!s.observedTick&&!s.body&&!s.foreign&&s.bodyId==-1&&s.foreignType==-1;
}
inline bool incarnation(const body::Achieved& a,const body::ContactOwnerH24& c) noexcept {
 return a.owner==c.owner&&a.colliderLease==c.colliderLease&&a.worldGeneration==c.worldGeneration&&
  a.bodyGeneration==c.bodyGeneration&&a.bodyId==c.bodyId&&a.trackingEpoch==c.trackingEpoch&&a.cancelGeneration==c.cancelGeneration;
}
inline bool live(const body::Achieved& a,const body::ContactOwnerH24& c,std::uint64_t now) noexcept {
 return a.state==body::State::Active&&a.poseValid&&!a.colliderRetired&&c.active&&incarnation(a,c)&&
  a.owner.key&&a.owner.generation&&a.owner.request&&a.owner.epoch&&a.owner.player&&a.owner.manager&&a.owner.character&&
  a.owner.side==0&&a.colliderLease&&a.worldGeneration&&a.bodyGeneration&&a.bodyId>=0&&a.trackingEpoch&&!(a.cancelGeneration&1)&&
  c.body&&c.foreign&&c.foreignType==0x5634&&a.sample&&c.sourceSample>=a.sample&&c.sourceTick>=a.sourceTick&&
  fresh(a.sourceTick,now)&&fresh(a.observedTick,now)&&fresh(c.sourceTick,now)&&fresh(c.observedTick,now)&&
  pose_core::finite(a.weaponWorld.p)&&pose_core::finite(a.linear)&&pose_core::finite(a.angular)&&
  bool(pose_core::unit(a.weaponWorld.q));
}
inline bool make(const body::Achieved& a,const body::ContactOwnerH24& c,std::uint64_t admission,std::uint64_t now,Stamp& out) noexcept {
 out={};if(!admission||!live(a,c,now))return false;
 out={a.owner,a.worldGeneration,a.bodyGeneration,a.colliderLease,a.trackingEpoch,a.cancelGeneration,
  admission,a.sample,a.sourceTick,a.observedTick,c.body,c.foreign,a.bodyId,c.foreignType};return true;
}
inline bool currentIncarnation(const Stamp& s,const body::Achieved& a,const body::ContactOwnerH24& c,std::uint64_t admission,std::uint64_t now) noexcept {
 return s.admissionSerial&&s.admissionSerial==admission&&s.owner==a.owner&&live(a,c,now)&&
  s.worldGeneration==a.worldGeneration&&s.bodyGeneration==a.bodyGeneration&&s.colliderLease==a.colliderLease&&
  s.trackingEpoch==a.trackingEpoch&&s.cancelGeneration==a.cancelGeneration&&s.bodyId==a.bodyId&&
  s.body==c.body&&s.foreign==c.foreign&&s.foreignType==c.foreignType;
}
inline bool current(const Stamp& s,const body::Achieved& a,const body::ContactOwnerH24& c,std::uint64_t admission,std::uint64_t now) noexcept {
 return s.admissionSerial&&s.admissionSerial==admission&&s.owner==a.owner&&live(a,c,now)&&
  s.worldGeneration==a.worldGeneration&&s.bodyGeneration==a.bodyGeneration&&s.colliderLease==a.colliderLease&&
  s.trackingEpoch==a.trackingEpoch&&s.cancelGeneration==a.cancelGeneration&&s.bodyId==a.bodyId&&
  s.body==c.body&&s.foreign==c.foreign&&s.foreignType==c.foreignType&&s.sourceSample&&s.sourceSample<=a.sample&&
  s.sourceTick<=a.sourceTick&&s.observedTick<=a.observedTick&&fresh(s.sourceTick,now)&&fresh(s.observedTick,now);
}
// Contact metadata retains the ORIGINAL occurrence clocks, never the latest
// achieved sample. Current data only proves that exact incarnation still lives.
inline bool makeContact(const body::ContactOwnerH24& occurrence,const body::Achieved& a,
 const body::ContactOwnerH24& liveOwner,std::uint64_t admission,std::uint64_t now,Stamp& out) noexcept {
 out={};Stamp candidate{};
 if(!make(a,liveOwner,admission,now,candidate)||!occurrence.active||!incarnation(a,occurrence)||
    occurrence.body!=liveOwner.body||occurrence.foreign!=liveOwner.foreign||occurrence.foreignType!=liveOwner.foreignType||
    !occurrence.sourceSample||occurrence.sourceSample>a.sample||occurrence.sourceTick>a.sourceTick||
    !fresh(occurrence.sourceTick,now)||!fresh(occurrence.observedTick,now))return false;
 candidate.sourceSample=occurrence.sourceSample;candidate.sourceTick=occurrence.sourceTick;candidate.observedTick=occurrence.observedTick;
 if(!current(candidate,a,liveOwner,admission,now))return false;out=candidate;return true;
}

}
