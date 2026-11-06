#pragma once
#include "contact_metadata59c7g.h"
#include "contact_handoff59c3.h"
namespace kcd2vr::solver_rpg_h29 {
namespace s=solver_command_h28;
inline bool same(const s::Stamp& a,const s::Stamp& b) noexcept {
 return a.owner==b.owner&&a.worldGeneration==b.worldGeneration&&a.bodyGeneration==b.bodyGeneration&&
  a.colliderLease==b.colliderLease&&a.trackingEpoch==b.trackingEpoch&&a.cancelGeneration==b.cancelGeneration&&
  a.admissionSerial==b.admissionSerial&&a.sourceSample==b.sourceSample&&a.sourceTick==b.sourceTick&&
  a.observedTick==b.observedTick&&a.body==b.body&&a.foreign==b.foreign&&a.bodyId==b.bodyId&&a.foreignType==b.foreignType;
}
template<class Legacy,class Typed> bool contactOwned(const contact59c7g::Metadata& m,
 std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,
 std::uint64_t sample,std::uint64_t gesture,std::uint64_t impactTick,std::uint64_t now,Legacy&& legacy,Typed&& typed) noexcept {
 const auto& p=m.sourceSolverH28;
 if(!m.occurrenceSerialH28&&s::empty(p))return legacy(key,generation,epoch,request);
 // A malformed/retired physical occurrence never falls back to hand-ray ownership.
 if(!m.occurrenceSerialH28||s::empty(p)||!gesture||!sample||sample!=p.sourceSample||
    p.owner.key!=key||p.owner.generation!=generation||p.owner.epoch!=epoch||p.owner.request!=request||
    !s::fresh(impactTick,now)||p.sourceTick>impactTick||p.observedTick>impactTick)return false;
 return typed(p,key,generation,epoch,request,sample,gesture);
}
// H30: a previously produced checked contact can await its original native
// action while the display pose temporarily reacquires reach/collision. This
// function cannot offer a new contact: the caller owns an immutable queued
// event and later revalidates its exact struck body/part. Typed F9 occurrences
// retain all their existing body/lease checks and never use the normal proof.
template<class Legacy,class Original,class Typed> bool originalContactOwnedH30(const contact59c7g::Metadata& m,
 std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request,
 std::uint64_t sample,std::uint64_t gesture,std::uint64_t impactTick,std::uint64_t now,
 Legacy&& legacy,Original&& original,Typed&& typed) noexcept {
 if(m.occurrenceSerialH28||!s::empty(m.sourceSolverH28))
  return contactOwned(m,key,generation,epoch,request,sample,gesture,impactTick,now,legacy,typed);
 if(!m.collider||!key||!generation||!epoch||!request||!sample||!gesture||
    !contact176::fresh(impactTick,now,contact59c3::maximumContactAge))return false;
 return original(key,generation,epoch,request,sample,gesture,impactTick,now);
}
template<class Action,class Legacy,class Typed> bool costOwned(const Action& a,Legacy&& legacy,Typed&& typed) noexcept {
 const auto& p=a.solverH28;const auto& k=a.key;
 if(!k.gesture||!a.actor||!k.weapon||!k.generation||!k.body||!k.request||!k.player)return false;
 if(s::empty(p))return legacy(k.weapon,k.generation,k.body,k.request);
 if(!k.gesture||!a.actor||!p.sourceSample||!p.sourceTick||!p.observedTick||p.owner.key!=k.weapon||p.owner.generation!=k.generation||
    p.owner.epoch!=k.body||p.owner.request!=k.request||p.owner.player!=k.player)return false;
 // Original action ancestry is retained even after physical retirement. Only
 // the typed current-source check may validate it; legacy success cannot rescue it.
 return typed(p,k.gesture,k.weapon,k.generation,k.body,k.request,a.actor);
}
}
