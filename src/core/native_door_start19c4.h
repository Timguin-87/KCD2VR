#pragma once
#include "snapshot_mailbox.h"
#include <cstdint>
namespace kcd2vr::door_start19c4 {
struct Watch {std::uintptr_t anim=0;std::uint64_t serial=0,tick=0;};
struct Receipt {std::uintptr_t anim=0;std::uint64_t serial=0,tick=0;int id=-1;};
inline core::SnapshotMailbox<Watch> watched;
inline core::SnapshotMailbox<Receipt> accepted;
inline void arm(std::uintptr_t anim,std::uint64_t serial,std::uint64_t now) noexcept {watched.publish({anim,serial,now});}
// Observation AFTER the real native animation start succeeded. All actors are
// pass-through; only the exact pending door/layer is recorded. No name heuristic.
inline void started(std::uintptr_t anim,int layer,int id,std::uint64_t now) noexcept {
    if(layer!=0)return;Watch w{};
    if(watched.read(w)&&w.anim==anim&&w.serial&&w.tick&&now>=w.tick&&now-w.tick<=250)
        accepted.publish({anim,w.serial,now,id});
}
inline bool matches(std::uintptr_t anim,std::uint64_t serial,std::uint64_t now,int id) noexcept {
    Receipt r{};return accepted.read(r)&&r.anim==anim&&r.serial==serial&&r.tick&&now>=r.tick&&now-r.tick<=250&&
        (r.id<0||r.id==id);
}
}
