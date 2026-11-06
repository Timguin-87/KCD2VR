// DS-2026-10-23 (Miki: "if we are near someone, QUICK TALK IS NOT WORKING - it is
// ONLY FOR the distance"): the quick-talk point gesture must never fire while the
// same hand touches/holds a living NPC. Also carries the FREE-HAND touch feed for
// the F9 rung-3 physics lab (the lab58's living-impact/corpse-drag consumers) - the
// lab's own switches gate the reactions; this header only transports the contact.
#pragma once
#include <atomic>
#include <cstdint>
#include "core/snapshot_mailbox.h"
namespace kcd2vr::grab_over_talk_ds {
inline std::atomic<std::uint64_t> livingContactTick[2]{0,0};
inline void PublishLivingContact(int side,std::uint64_t now) noexcept {
    if(side>=0&&side<2)livingContactTick[side].store(now,std::memory_order_release);
}
inline bool LivingNear(int side,std::uint64_t now,std::uint64_t windowMs=250) noexcept {
    if(side<0||side>1||!now)return false;
    const auto t=livingContactTick[side].load(std::memory_order_acquire);
    return t&&now>=t&&now-t<=windowMs;
}
// The free-hand touch feed: the coherent pose publishes each fresh living contact;
// the lab58 native tick consumes it behind its own F9 rung-3 sub switches.
struct HandTouch {std::uint32_t entity=0;int part=-1;int side=-1;std::uint64_t tick=0,sample=0;
    float point[3]{};float normal[3]{};float speed=0;};
inline core::SnapshotMailbox<HandTouch> handTouch;
}
