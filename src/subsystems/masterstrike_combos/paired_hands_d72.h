#pragma once
#include "native_pair_d72.h"
#include "../../core/snapshot_mailbox.h"
#include <cstdint>
namespace kcd2vr::paired_hands_d72 {
// Exact accepted Henry pair presentation only. No action or gameplay authority.
struct Owner {
    native_pair_d72::Pair pair{};
    std::uintptr_t controller=0,character=0,scope=0,clip=0;
    std::uint64_t serial=0,peerSerial=0,epoch=0,tracking=0,sample=0,sourceTick=0,tick=0;
    std::uint32_t token=0,entity=0;
    int animation=-1,layer=-1;
    unsigned hands=0;
};
inline core::SnapshotMailbox<Owner> witness;
// The paired-animation owner still owns the native held-item attachment. An
// arms-only override is therefore safe only when both native hand memberships
// are positively empty; an unreadable/unknown hand is not an empty hand.
// This is a copy-only presentation proof and grants no item/action mutation.
template<class Read> bool empty_native_hands(Read&& rd,std::uintptr_t base,std::uintptr_t manager,
    std::uint32_t expectedEntity) noexcept {
    if(!base||!manager||!expectedEntity)return false;
    const auto eq=[&](std::uintptr_t address,std::uintptr_t expected) noexcept {
        std::uintptr_t value=0;return rd(address,value)&&value==expected;
    };
    std::uintptr_t player=0,entity=0,slots=0;std::uint32_t id=0;
    std::uint64_t key[2]{},empty=0;
    if(!eq(manager,base+0x3EC9958)||!rd(manager+0x38,player)||!player||
       !eq(player,base+0x3AA4B98)||!eq(player+0x9D0,manager)||
       !rd(player+0x30,id)||id!=expectedEntity||!rd(player+0x38,entity)||!entity||
       !eq(entity,base+0x4072C78)||!rd(entity+0x0C,id)||id!=expectedEntity||
       !rd(player+0x990,slots)||!slots||!rd(slots+0x260,key[0])||!rd(slots+0x268,key[1]))return false;
    if((key[0]||key[1])&&!rd(base+0x5325B60,empty))return false;
    if((key[0]&&key[0]!=empty)||(key[1]&&key[1]!=empty))return false;
    // A concurrent owner/equipment transition must not reuse the first read.
    std::uint64_t after[2]{};
    return eq(manager,base+0x3EC9958)&&eq(manager+0x38,player)&&eq(player,base+0x3AA4B98)&&
        eq(player+0x9D0,manager)&&eq(player+0x38,entity)&&eq(player+0x990,slots)&&
        rd(player+0x30,id)&&id==expectedEntity&&rd(entity+0x0C,id)&&id==expectedEntity&&
        rd(slots+0x260,after[0])&&after[0]==key[0]&&rd(slots+0x268,after[1])&&after[1]==key[1];
}
inline bool current(const Owner& r,std::uintptr_t character,std::uint64_t epoch,std::uint64_t tracking,std::uint64_t now) noexcept {
    return r.controller&&r.character==character&&r.scope&&r.clip&&r.serial&&r.peerSerial&&
        r.epoch==epoch&&epoch&&r.tracking==tracking&&tracking&&r.sample&&r.entity&&
        r.pair.attackEntity==r.entity&&r.pair.attackFragment&&r.pair.hitFragment&&
        r.pair.attackAction&&r.pair.hitAction&&r.animation>=0&&r.layer>=0&&r.layer<=8&&r.hands&&
        r.sourceTick&&r.tick>=r.sourceTick&&r.tick-r.sourceTick<=80&&now>=r.tick&&now-r.tick<=80&&
        now>=r.sourceTick&&now-r.sourceTick<=80;
}
inline bool same(const Owner& a,const Owner& b) noexcept {
    return native_pair_d72::same(a.pair,b.pair)&&a.controller==b.controller&&a.character==b.character&&
        a.scope==b.scope&&a.clip==b.clip&&a.serial==b.serial&&a.peerSerial==b.peerSerial&&
        a.epoch==b.epoch&&a.tracking==b.tracking&&a.token==b.token&&a.entity==b.entity&&a.animation==b.animation&&a.layer==b.layer;
}
}
