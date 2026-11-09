#pragma once
// R103: a held small prop or a dragged corpse must not collide with Henry.
// R101 log (/tmp/r103/KCD2VR.log 09:53:43-10:00:53): every held prop is a real
// rigid body driven by the velocity servo toward the hand. When the hand comes
// near the body the prop hits Henry's living capsule; the servo keeps pushing
// it into him and he is pushed away and the prop spins.
// The fix uses the retail collision class (CPhysicalEntity +0xEC type,
// +0xF0 ignore; setter +850AA2: new = (old & AND) | OR). Henry's living body
// has type bit 4 (living, read by native_physics_phase_t4 ObserveLivingExcluded).
// A body ignores a pair when (its ignore & other type) or (other ignore & its type).
// - held prop: ignore |= living (it passes through Henry and NPC capsules) and
//   type |= living, so the mod's own hand proxies / weapon bodies (ignore=4,
//   native_hand_proxy_owner60, native_weapon_body_t4) do not hit it either.
// - dragged corpse: ignore |= living only.
// Only the bits that were NOT already set are added, and only those are removed
// again. Restore after release: when the object is clear of Henry or at the
// latest after kMaxMs. Never a latch: no lease outlives kMaxMs after release.
#include <cstdint>
#include <cmath>
#include "brush_swap_r100.h" // R158: VrSceneryBigPropsR158 (bigger props need a bigger clear radius)
namespace kcd2vr::self_collision_r103 {
inline constexpr std::uint32_t kLiving=4u;
enum class Kind : unsigned char {None,Prop,Corpse};
// one owner bit per hand and route; two hands can share one body
// R155: the instant-held pickup item (staged_pickup596c2 / instant_pickup_native_d72) is its own route: it was the
// one carried rigid body WITHOUT this exclusion, so it pushed Henry around ("the objects are pushing me back").
enum Owner : unsigned {PropR=1u<<0,PropL=1u<<1,CorpseLabR=1u<<2,CorpseLabL=1u<<3,CorpseSavedR=1u<<4,CorpseSavedL=1u<<5,InstantR=1u<<6,InstantL=1u<<7};
inline unsigned propOwner(int side) noexcept {return side?PropL:PropR;}
inline unsigned instantOwner(int side) noexcept {return side?InstantL:InstantR;}
inline unsigned corpseLabOwner(int side) noexcept {return side?CorpseLabL:CorpseLabR;}
inline unsigned corpseSavedOwner(int side) noexcept {return side?CorpseSavedL:CorpseSavedR;}
struct Bits {std::uint32_t type=0,ignore=0;bool any() const noexcept {return type||ignore;}};
// bits this lease still has to add, from the CURRENT class of the body
inline Bits missing(Kind k,std::uint32_t type,std::uint32_t ignore) noexcept {
    Bits b{};if(k==Kind::None)return b;
    b.ignore=kLiving&~ignore;
    if(k==Kind::Prop)b.type=kLiving&~type;
    return b;
}
struct Lease {
    Kind kind=Kind::None;std::uint32_t id=0;std::uintptr_t entity=0,body=0;int part=-1;
    Bits added{};unsigned owners=0;std::uint64_t released=0,applyTick=0;bool used=false;
};
inline constexpr unsigned kMax=8;
inline constexpr std::uint64_t kMinMs=400;   // a dropped prop first falls away from the hand
inline constexpr std::uint64_t kMaxMs=4000;  // hard end: the body collides with Henry again
inline constexpr std::uint64_t kReapplyMs=250; // a queued/refused native write is retried at this rate while held
// horizontal metres from Henry's root. R158: with "Scenery props: size" UP TO 1 M a dropped prop may be 1 m across, so
// its centre at 0.6 m can still overlap Henry's capsule: props then wait for 1.0 m like a corpse (or the 4 s end).
inline double clearRadius(Kind k) noexcept {
    return k==Kind::Corpse||(k==Kind::Prop&&brush_swap_r100::bigPropsR158.load(std::memory_order_acquire))?1.0:.6;
}
enum class Due {Wait,Restore};
inline Due due(const Lease& l,std::uint64_t now,bool haveDistance,double horizontal) noexcept {
    if(!l.used||l.owners)return Due::Wait;
    if(now<l.released)return Due::Restore; // clock reset: never keep a lease
    const auto age=now-l.released;
    if(age>=kMaxMs)return Due::Restore;
    if(age<kMinMs)return Due::Wait;
    return haveDistance&&std::isfinite(horizontal)&&horizontal>=clearRadius(l.kind)?Due::Restore:Due::Wait;
}
struct Table {
    Lease e[kMax]{};
    Lease* byBody(std::uintptr_t body) noexcept {
        if(!body)return nullptr;for(auto& l:e)if(l.used&&l.body==body)return &l;return nullptr;}
    Lease* byOwner(unsigned owner) noexcept {
        if(!owner)return nullptr;for(auto& l:e)if(l.used&&(l.owners&owner))return &l;return nullptr;}
    Lease* take(Kind k,std::uint32_t id,std::uintptr_t entity,std::uintptr_t body,int part,unsigned owner) noexcept {
        if(k==Kind::None||!body||!owner)return nullptr;
        if(auto* l=byBody(body)){l->owners|=owner;l->released=0;return l;}
        for(auto& l:e)if(!l.used){l={};l.used=true;l.kind=k;l.id=id;l.entity=entity;l.body=body;l.part=part;l.owners=owner;return &l;}
        return nullptr; // table full: this hold runs without the exclusion (one log row)
    }
    // the owner lets go; returns the lease that is now waiting for its restore
    Lease* release(unsigned owner,std::uint64_t now) noexcept {
        Lease* last=nullptr;
        for(auto& l:e)if(l.used&&(l.owners&owner)){l.owners&=~owner;if(!l.owners)l.released=now?now:1;last=&l;}
        return last;
    }
    unsigned count() const noexcept {unsigned n=0;for(const auto& l:e)n+=l.used?1u:0u;return n;}
};
}
