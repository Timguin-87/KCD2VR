#pragma once
#include "../vrik/contact_authority176.h"
namespace kcd2vr::contact47c2 {
struct Lease {std::uint64_t policy=0,serial=0;};
enum class Result {Wait,Bound,Reject};
// V66.178R49: a contact may wait up to 300 ms (was 100) for the release the recogniser commits at/after the contact.
inline constexpr std::uint64_t kJoinMs49=300;
// A pre-commit contact can bind only the immediately following release within
// 100 ms. Once bound, PREPARE/cancel/new release cannot transfer it to a new attack.
inline Result bind(Lease& l,const contact176::Ticket& t,std::uint64_t policy,
    std::uint64_t tick,std::uint64_t now,std::uint64_t key,std::uint64_t generation,
    std::uint64_t epoch,std::uint64_t request) noexcept {
 if(!l.policy||!tick||now<tick)return Result::Reject;
 if(l.serial&&(policy!=l.policy||t.serial!=l.serial))return Result::Reject;
 if(!l.serial&&policy!=l.policy&&(l.policy==UINT64_MAX||policy!=l.policy+1))return Result::Reject;
 const bool same=t.serial&&t.policy==policy&&t.key==key&&t.generation==generation&&
    t.epoch==epoch&&t.request==request&&t.player&&t.entity;
 if(!same)return policy==l.policy&&!l.serial&&now-tick<=kJoinMs49?Result::Wait:Result::Reject;
 if(!l.serial){
    // Early telegraph/release may precede impact by the native stroke's life.
    // Only a release AFTER this contact has the short join deadline.
    if(!t.tick||(t.tick>tick&&t.tick-tick>kJoinMs49))return Result::Reject;
    l.policy=policy;l.serial=t.serial;
 }
 return contact176::fresh(t.tick,now,contact176::kReleaseSafetyCapMs)?Result::Bound:Result::Reject;
}
}
