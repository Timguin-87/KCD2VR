#pragma once
#include "../activity_eye59c19.h"
#include "../activity_input163.h"
#include "../core/snapshot_mailbox.h"

namespace kcd2vr::dice_body_d81 {
// A copied presentation lease from the camera's already accepted native dice
// owner. It never authorizes a roll, a result, inventory or world interaction.
struct Lease {
    std::uint64_t tick=0,lifetime=0,enteredAt=0,bodyEpoch=0;
    std::uintptr_t instance=0,entity=0,character=0;
    dialogue_state59c10r2::V headLocal{};
    bool allowed=false;
};
inline core::SnapshotMailbox<Lease> visual;
inline std::atomic<bool> blocked{true};
inline bool matches(const Lease& s) noexcept {
    const auto& w=activity_input163::witness175;
    if(!s.allowed||!s.instance||!s.character||!s.bodyEpoch||!w.ready.load(std::memory_order_acquire)||
       w.transitionDepth175.load(std::memory_order_acquire)||
       w.dice.load(std::memory_order_acquire)!=s.instance||w.diceStart.load(std::memory_order_acquire)!=s.enteredAt||
       w.lifetime175.load(std::memory_order_acquire)!=s.lifetime||w.forgeBuilder.load(std::memory_order_acquire))return false;
    for(const auto& other:w.active)if(other.load(std::memory_order_acquire))return false;
    return dialogue_state59c10r2::finite(s.headLocal);
}
inline Lease qualify(const activity_eye59c19::State& owner,const dialogue_state59c10r2::NativeHead& head,
                     const activity_eye59c19::Anchor& anchor,std::uint64_t now,bool applied) noexcept {
    Lease s{};
    if(!applied||owner.kindMask!=32u||!activity_eye59c19::fresh(owner,now)||!owner.active||!owner.eligible||
       !activity_eye59c19::body(head,now)||owner.entity!=head.entity||!activity_eye59c19::sameOwner(anchor.owner,owner)||
       !activity_eye59c19::sameBody(anchor.eye,head)||!anchor.eye.headKnownD81)return s;
    s.tick=now;s.lifetime=owner.lifetime;s.enteredAt=owner.enteredAt;s.bodyEpoch=head.epoch;
    s.instance=activity_input163::witness175.dice.load(std::memory_order_acquire);
    s.entity=head.entity;s.character=head.character;s.headLocal=anchor.eye.headLocalD81;s.allowed=true;
    if(!matches(s))return {};
    return s;
}
inline void publish(const activity_eye59c19::State& owner,const dialogue_state59c10r2::NativeHead& head,
                    const activity_eye59c19::Anchor& anchor,std::uint64_t now,bool applied) noexcept {
    const auto s=qualify(owner,head,anchor,now,applied);
    if(!s.allowed)blocked.store(true,std::memory_order_release);
    const bool written=visual.publish(s);
    blocked.store(!written||!s.allowed,std::memory_order_release);
}
inline void retire() noexcept {
    blocked.store(true,std::memory_order_release);(void)visual.publish({});
}
inline bool read(Lease& s,std::uint64_t now) noexcept {
    return !blocked.load(std::memory_order_acquire)&&visual.read(s)&&
        dialogue_state59c10r2::age(s.tick,now,80)&&matches(s)&&!blocked.load(std::memory_order_acquire);
}
inline bool active(std::uint64_t now) noexcept {Lease s{};return read(s,now);}
inline bool current(const Lease& s,std::uint64_t now) noexcept {
    Lease v{};return read(v,now)&&s.allowed&&v.instance==s.instance&&v.lifetime==s.lifetime&&
        v.enteredAt==s.enteredAt&&v.character==s.character&&v.entity==s.entity&&v.bodyEpoch==s.bodyEpoch;
}
}
