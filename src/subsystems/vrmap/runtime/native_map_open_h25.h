#pragma once
#include "core/snapshot_mailbox.h"
#include <atomic>
#include <cstdint>
namespace kcd2vr::native_map_open_h25 {
// One deliberate shortcut, one ordinary native inventory item-use request.
// No animation, world placement, equipment, inventory or camera mutation here.
struct Ticket {std::uint64_t serial=0,started=0;};
struct Ui {std::uint64_t serial=0,tick=0;bool inventory=false,allowed=false;};
inline core::SnapshotMailbox<Ticket> ticket;
inline core::SnapshotMailbox<Ui> ui;
inline std::atomic<std::uint64_t> next{0},pending{0};
inline std::atomic<bool> chordClaimed{false};
inline void ObserveChord(bool held) noexcept {if(!held)chordClaimed.store(false,std::memory_order_release);}
inline bool Begin(std::uint64_t now,bool allowed) noexcept {
 if(chordClaimed.exchange(true)||!allowed||!now||pending.load())return false;
 const auto serial=next.fetch_add(1)+1;
 if(!ticket.publish({serial,now}))return false;
 pending.store(serial,std::memory_order_release);return true;
}
inline void Finish(std::uint64_t serial) noexcept {pending.compare_exchange_strong(serial,0,std::memory_order_acq_rel);}
inline bool Pending() noexcept {return pending.load(std::memory_order_acquire)!=0;}
inline void PublishUi(std::uint64_t now,bool inventory,bool allowed,bool cancel) noexcept {
 const auto serial=pending.load(std::memory_order_acquire);if(!serial)return;
 Ticket t{};if(cancel||!ticket.read(t)||t.serial!=serial||now<t.started||now-t.started>3000){Finish(serial);return;}
 (void)ui.publish({serial,now,inventory,allowed});
}
inline bool Current(std::uint64_t now,Ticket& t) noexcept {
 Ui u{};const auto serial=pending.load(std::memory_order_acquire);
 if(serial&&ticket.read(t)&&t.serial==serial&&(now<t.started||now-t.started>3000)){Finish(serial);return false;}
 return serial&&ticket.read(t)&&t.serial==serial&&now>=t.started&&now-t.started<=3000&&
  ui.read(u)&&u.serial==serial&&u.inventory&&u.allowed&&u.tick&&now>=u.tick&&now-u.tick<=100;
}
}
