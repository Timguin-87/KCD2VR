#pragma once
#include "draw_motion_d63.h"
#include "core/snapshot_mailbox.h"
#include <atomic>
namespace kcd2vr::draw_signal_d63 {
inline core::SnapshotMailbox<draw_motion_d63::Ticket> latest[2];
inline std::atomic<std::uint64_t> next{1};
inline void publish(draw_motion_d63::Ticket t) noexcept {
    if(t.side<0||t.side>1||!t.key||!t.generation||!t.body||!t.request||!t.sample||!t.tick||!draw_motion_d63::sourceFor(t))return;
    t.serial=next.fetch_add(1,std::memory_order_relaxed);latest[t.side].publish(t);
}
}
