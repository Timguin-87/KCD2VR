#pragma once
#include "core/snapshot_mailbox.h"
#include "vrik/passive_contact169r1.h"
#include "vrik/npc_contact172.h"
#include <atomic>
#include <cmath>
#include <cstdint>

// Deliberate admission to an EXISTING native automatic-clinch candidate.
// This owns neither a living constraint nor a paired animation. Native false
// must remain false. A grip is consumed at the candidate boundary, not reported
// as an accepted action or an achieved hold.
namespace kcd2vr::clinch_intent_d40 {
enum class Mode : int { Distance=0, ContactGrip=1, Disabled=2 };
inline std::atomic<int> mode{int(Mode::Disabled)};
inline std::atomic<std::uint64_t> generation{2},nextSerial{0};
inline int getMode() noexcept {return mode.load(std::memory_order_acquire);}
inline void setMode(int value) noexcept {
    value=value>=0&&value<=2?value:int(Mode::Disabled);
    if(mode.exchange(value,std::memory_order_acq_rel)!=value)
        generation.fetch_add(2,std::memory_order_acq_rel);
}
inline bool wantsContact() noexcept {return getMode()==int(Mode::ContactGrip);}
struct Ticket {
    std::uint64_t serial=0,pressTick=0,tick=0,sequence=0,publication=0,policy=0;
    std::uint64_t actor=0,epoch=0,request=0;
    std::uintptr_t owner=0,collider=0;
    std::uint32_t entity=0;int side=-1,part=-1;
};
struct Producer { // one committed-pose producer; never accessed by native hooks
    std::uint64_t policy=0,actor=0,epoch=0,request=0,sequence=0;
    bool armed=false;Ticket pending{};
};
inline Producer producer[2];
inline core::SnapshotMailbox<Ticket> tickets[2];
inline std::atomic<std::uint64_t> used[2]{};
inline bool contact(const passive169r1::Sample& s,const npc_contact172::Witness& w) noexcept {
    return s.actor&&s.epoch&&s.request&&s.sequence&&s.tick&&s.eligible&&s.worldCoherent&&
        s.source==passive169r1::Source::HandWorld&&s.status==passive169r1::Status::Contact&&
        s.key==0x48414e4400000000ull+unsigned(s.side)&&s.generation==0&&
        w.ownerRead&&w.owner&&w.collider&&w.entityId&&w.part>=0&&
        (w.body==npc_contact172::Body::Living||w.body==npc_contact172::Body::Articulated)&&
        pose_core::finite(s.point)&&bool(pose_core::unit(s.normal));
}
inline void publish(const passive169r1::Sample& s,const npc_contact172::Witness& w,
                    float grip,std::uint64_t publication,bool closedFist=false) noexcept {
    if(s.side<0||s.side>1||s.source!=passive169r1::Source::HandWorld)return;
    auto& p=producer[s.side];const auto policy=generation.load(std::memory_order_acquire);
    if(p.policy!=policy||p.actor!=s.actor||p.epoch!=s.epoch||p.request!=s.request){
        p={};p.policy=policy;p.actor=s.actor;p.epoch=s.epoch;p.request=s.request;
    }
    if(!wantsContact()||!s.sequence||s.sequence<=p.sequence){
        if(!wantsContact())(void)tickets[s.side].publish({});return;
    }
    p.sequence=s.sequence;
    if(closedFist||!std::isfinite(grip)||grip<0.f||grip>1.f||!s.tick||!publication||(publication&1)){
        p.armed=false;p.pending={};(void)tickets[s.side].publish({});return;
    }
    if(grip<.30f){p.armed=true;p.pending={};(void)tickets[s.side].publish({});return;}
    const bool touching=contact(s,w);
    if(p.armed&&grip>.58f){
        p.armed=false;p.pending={};
        if(touching){
            auto& t=p.pending;t.serial=nextSerial.fetch_add(1,std::memory_order_acq_rel)+1;
            t.pressTick=s.tick;t.policy=policy;t.actor=s.actor;t.epoch=s.epoch;t.request=s.request;
            t.owner=w.owner;t.collider=w.collider;t.entity=w.entityId;t.side=s.side;t.part=w.part;
        }
    }
    auto& t=p.pending;
    // Preserve the original press time. A held grip cannot perpetually refresh
    // an unconsumed ticket, move it to another NPC, or start another clinch.
    if(!touching||grip<.35f||!t.serial||s.tick<t.pressTick||s.tick-t.pressTick>250||
       t.owner!=w.owner||t.entity!=w.entityId||t.collider!=w.collider||t.part!=w.part){
        t={};(void)tickets[s.side].publish({});return;
    }
    t.tick=s.tick;t.sequence=s.sequence;t.publication=publication;
    if(policy!=generation.load(std::memory_order_acquire)||!tickets[s.side].publish(t))t={};
}
inline bool current(const Ticket& t,std::uint64_t now,std::uint64_t actor,
                    std::uint64_t epoch,std::uint64_t request,std::uintptr_t target,
                    std::uint32_t entity,std::uint64_t publication) noexcept {
    return wantsContact()&&t.side>=0&&t.side<2&&t.serial&&t.pressTick&&t.tick&&t.sequence&&
        now>=t.tick&&now-t.tick<=80&&now>=t.pressTick&&now-t.pressTick<=250&&
        t.policy==generation.load(std::memory_order_acquire)&&t.actor==actor&&t.epoch==epoch&&
        t.request==request&&t.owner==target&&t.entity==entity&&t.collider&&t.part>=0&&
        publication&&!(publication&1)&&t.publication==publication&&
        used[t.side].load(std::memory_order_acquire)<t.serial;
}
inline bool consume(const Ticket& t) noexcept {
    if(t.side<0||t.side>1||!t.serial||!wantsContact()||t.policy!=generation.load(std::memory_order_acquire))return false;
    auto previous=used[t.side].load(std::memory_order_acquire);
    while(previous<t.serial){if(used[t.side].compare_exchange_weak(previous,t.serial,std::memory_order_acq_rel))return true;}
    return false;
}
} // namespace kcd2vr::clinch_intent_d40
