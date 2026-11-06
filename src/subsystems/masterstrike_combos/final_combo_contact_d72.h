#pragma once
// D72 final-move policy. Only the native recipe's LAST input is translated.
// Prefix credit, live slot, original selector, cost and outcomes remain native.
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include "../../core/snapshot_mailbox.h"
#include "../../vrik/instant_activities_d72.h"
#include "../../vrik/motion_owned59c4.h"
#include "../../diagnostics/combo_snapshot.h"
#include "physical_finisher_cb1.h"

namespace kcd2vr::final_combo_d72 {
namespace bc=kcd2vr::bodycombo59c7g;
struct Contact {
    std::uint64_t tick=0,sample=0;
    std::uintptr_t collider=0;
    std::uint32_t target=0;
    int body=0,part=-1;
    bc::Feature feature=bc::Feature::Unknown;
    double height=0;
    bool actual=false,heightKnown=false,ordinaryWeapon=false;
};
// This producer owns the right weapon's immutable body contact. Separate
// fist/shield owners, trips and method-only weapon-head rows retain their
// original native path; do not hold an input for evidence we cannot produce.
inline bool weaponEndpoint(const finisher_cb1::Row& row) noexcept {
    if(row.motion==bc::Motion::Trip)return false;
    switch(row.feature){
    case bc::Feature::Blade:case bc::Feature::Tip:case bc::Feature::Any:
    case bc::Feature::Pommel:case bc::Feature::Handle:return true;
    case bc::Feature::WeaponHead:return row.region!=bc::Region::Any;
    default:return false;
    }
}
// Blade-versus-tip and slash-versus-stab are transport details for a target
// location. The generated WeaponHead marker on a face/knee/abdomen row does
// not create an additional contact-surface test. Pommel/handle/shield/fist
// really name an implement and still require their measured feature.
inline bool method(const finisher_cb1::Row& row,const Contact& c) noexcept {
    if(!c.actual||!c.collider||!c.target||(c.body!=2&&c.body!=3))return false;
    switch(row.feature){
    case bc::Feature::Blade:case bc::Feature::Tip:case bc::Feature::Any:
        return c.ordinaryWeapon;
    case bc::Feature::Pommel:return c.feature==bc::Feature::Pommel;
    case bc::Feature::Handle:return c.feature==bc::Feature::Handle;
    case bc::Feature::WeaponHead:
        return row.region!=bc::Region::Any&&row.motion!=bc::Motion::Trip&&c.ordinaryWeapon;
    case bc::Feature::Shield:return c.feature==bc::Feature::Shield||c.feature==bc::Feature::ShieldEdge;
    case bc::Feature::ShieldEdge:return c.feature==bc::Feature::ShieldEdge;
    case bc::Feature::Fist:return c.feature==bc::Feature::Fist;
    default:return false;
    }
}
inline bool matches(const finisher_cb1::Row& row,const Contact& c) noexcept {
    return method(row,c)&&(row.region==bc::Region::Any||
        (c.heightKnown&&finisher_cb1::regionOk(row.region,c.height)));
}
struct Permit {
    std::uint64_t serial=0,gesture=0,key=0,generation=0,body=0,request=0;
    std::uint64_t started=0,sourceSample=0;
    // D82: a measured contact can start a NEW input transaction for the same
    // still-owned final intent. The original request remains immutable above.
    std::uint64_t contactClockD82=0;
    std::uintptr_t player=0,entity=0,actor=0,state=0,manager=0,target=0,targetEntity=0;
    std::uintptr_t recipe=0,nativeStep=0;
    std::uint32_t targetId=0,cursor=0,required=0,xiSerial=0;
    std::uint8_t technique[16]{};
    int input=-1,zone=-1,physicalKind=0;
    Contact contact{};
};
enum class QueueD82 {Ordinary,Wait,Contact,Refused};
inline bool fresh(const Permit& p,std::uint64_t now) noexcept {
    const auto clock=p.contactClockD82?p.contactClockD82:p.started;
    return p.serial&&p.gesture&&p.key&&p.generation&&p.body&&p.request&&p.player&&p.entity&&
        p.actor&&p.state&&p.manager&&p.target&&p.targetEntity&&p.targetId&&p.recipe&&p.nativeStep&&
        p.started&&clock>=p.started&&now>=clock&&now-clock<=100&&p.sourceSample&&
        (!p.contactClockD82||(p.contactClockD82==p.contact.tick&&
            p.contactClockD82-p.started<=motion59c4::strikeLifetimeMs5910c1))&&
        p.contact.actual&&p.contact.tick>=p.started&&p.contact.tick<=now&&now-p.contact.tick<=100&&p.contact.target==p.targetId&&
        p.cursor&&p.cursor+1==p.required&&p.input>=0;
}
inline std::atomic<bool> hookReady{false};
inline core::SnapshotMailbox<Permit> mailbox;
inline std::atomic<std::uint64_t> next{1},live{0},selected{0};
inline std::uint64_t mint() noexcept {return next.fetch_add(1,std::memory_order_relaxed);}
inline void revoke() noexcept {live.store(0,std::memory_order_release);}
inline bool publish(const Permit& p) noexcept {
    revoke();if(!p.serial||!mailbox.publish(p))return false;
    live.store(p.serial,std::memory_order_release);return true;
}
inline bool readIdentity(Permit& p) noexcept {
    const auto serial=live.load(std::memory_order_acquire);
    return serial&&mailbox.read(p)&&p.serial==serial&&live.load(std::memory_order_acquire)==serial;
}
inline bool read(Permit& p,std::uint64_t now) noexcept {return readIdentity(p)&&fresh(p,now);}
// A delayed XI edge retains its exact captured identity for rejection, even if
// its permission has expired. That cannot fall through to a different finisher.
inline thread_local const Permit* dispatched=nullptr;
struct DispatchScope {
    const Permit* before=nullptr;
    explicit DispatchScope(const Permit* p) noexcept:before(dispatched){dispatched=p&&p->serial?p:nullptr;}
    ~DispatchScope(){dispatched=before;}
    DispatchScope(const DispatchScope&)=delete;
};
inline bool sameGesture(const Permit& p,std::uintptr_t player,std::uint64_t gesture,
    std::uint64_t key,std::uint64_t generation,std::uint64_t body,std::uint64_t request) noexcept {
    return p.gesture&&p.player==player&&p.gesture==gesture&&p.key==key&&p.generation==generation&&
        p.body==body&&p.request==request;
}
// A live candidate is read again at 93999B. A different recipe cannot borrow
// the same input family and turn a face strike into a knee/pommel finisher.
inline bool candidate(const Permit& p,const diagnostics::combo::Candidate& c,unsigned cursor) noexcept {
    return c.recipe==p.recipe&&c.requiredSteps==p.required&&cursor==p.cursor&&
        !std::memcmp(c.techniqueRaw.data(),p.technique,16);
}
}
