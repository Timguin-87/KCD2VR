#pragma once
#include "core/snapshot_mailbox.h"
#include <cstdint>
#include <atomic>
#include <cstring>
namespace kcd2vr::native_passenger_a34 {
// Presentation only. An installed native action owns its target, native UseHand
// events, costs/resources and alignment. This witness never starts an action,
// changes gameplay permission or classifies a camera/cutscene.
enum class Kind : unsigned { None, Wash, SoupEat, SoupPoison };
inline Kind classify(std::uint32_t crc) noexcept {
    switch(crc){
    case 0xeed2e729u:return Kind::Wash; // WashFace
    case 0x591f4107u:case 0x735721aau:return Kind::SoupEat; // BehaviorEatingBoiler/Pot
    case 0x04ab0eefu:case 0xc630055bu:case 0xd860d990u:return Kind::SoupPoison;
    default:return Kind::None;
    }
}
struct Root {
    std::uintptr_t controller=0,character=0,action=0,scope=0,context=0;
    std::uint64_t epoch=0,sourceTick=0,tick=0,serial=0;
    std::uint32_t token=0,crc=0;int fragment=-1,status=-1,layer=-1;
};
inline core::SnapshotMailbox<Root> witness;
// Only the exact WashFace passenger may retain controller-owned visible arms.
// Semantic current() stays unchanged: weapon/pickup owners still yield.
inline std::atomic<bool> trackedWashD72{false};
inline bool valid(const Root& r) noexcept {
    return r.controller&&r.character&&r.action&&r.scope&&r.context&&r.epoch&&r.serial&&
        r.sourceTick&&r.tick>=r.sourceTick&&r.tick-r.sourceTick<=80&&r.status==2&&r.layer==0&&
        r.fragment>=0&&classify(r.crc)!=Kind::None;
}
inline bool sameAction(const Root& a,const Root& b) noexcept {
    return a.controller==b.controller&&a.character==b.character&&a.action==b.action&&a.scope==b.scope&&
        a.context==b.context&&a.epoch==b.epoch&&a.token==b.token&&a.fragment==b.fragment&&a.crc==b.crc;
}
inline bool current(const Root& r,std::uintptr_t character,std::uint64_t epoch,std::uint64_t now) noexcept {
    return valid(r)&&r.character==character&&r.epoch==epoch&&now>=r.tick&&now-r.tick<=80&&
        now>=r.sourceTick&&now-r.sourceTick<=80;
}
inline bool current(std::uintptr_t character,std::uint64_t epoch,std::uint64_t now) noexcept {
    Root r{};return witness.read(r)&&current(r,character,epoch,now);
}
inline bool trackedWash(const Root& r,std::uintptr_t character,std::uint64_t epoch,std::uint64_t now) noexcept {
    return trackedWashD72.load(std::memory_order_acquire)&&classify(r.crc)==Kind::Wash&&current(r,character,epoch,now);
}
inline bool trackedWash(std::uintptr_t character,std::uint64_t epoch,std::uint64_t now) noexcept {
    Root r{};return witness.read(r)&&trackedWash(r,character,epoch,now);
}
inline bool bodyCurrent(std::uintptr_t character,std::uint64_t epoch,std::uint64_t now) noexcept {
    Root r{};return witness.read(r)&&current(r,character,epoch,now)&&!trackedWash(r,character,epoch,now);
}
inline bool trackedWashAlias(const char* name,int layer,std::uintptr_t character,std::uint64_t epoch,std::uint64_t now) noexcept {
    return name&&layer==0&&std::strcmp(name,"player_washing_watertube")==0&&trackedWash(character,epoch,now);
}

}
