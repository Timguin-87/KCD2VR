#pragma once
#include "core/snapshot_mailbox.h"
#include <atomic>
#include <cstdint>
namespace kcd2vr::native_map_live_h28 {
struct Arm {std::uint64_t serial=0,tick=0,vm=0;};
struct Lease {std::uint64_t serial=0,tick=0,vm=0,lifetime=0;std::uintptr_t reading=0;};
inline core::SnapshotMailbox<Arm> arms;
inline core::SnapshotMailbox<Lease> leases;
inline std::atomic<std::uint64_t> armed{0},until{0};
inline std::atomic<bool> clear{false};
inline void Request(std::uint64_t serial,std::uint64_t now,std::uint64_t vm) noexcept {
 if(serial&&now&&vm&&arms.publish({serial,now,vm}))armed.store(serial,std::memory_order_release);
}
inline void Finish(std::uint64_t serial) noexcept {armed.compare_exchange_strong(serial,0);}
inline void Publish(std::uint64_t now,std::uintptr_t reading,std::uint64_t lifetime,bool allowed) noexcept {
 // Sole policy producer; script owner writes only the immutable Arm mailbox.
 static std::uint64_t serial=0,boundEpoch=0;static std::uintptr_t bound=0;
 const auto id=armed.load(std::memory_order_acquire);Arm a{};
 bool active=id&&arms.read(a)&&a.serial==id&&now>=a.tick;
 if(serial!=id){serial=id;bound=0;boundEpoch=0;}
 if(active&&bound&&(reading!=bound||lifetime!=boundEpoch)){Finish(id);active=false;}
 if(active&&!bound){if(now-a.tick>5000){Finish(id);active=false;}else if(reading){bound=reading;boundEpoch=lifetime;}}
 active=active&&bound&&allowed&&lifetime;
 const auto previous=until.exchange(active?now+150:0);
 if(active){if(!leases.publish({id,now,a.vm,lifetime,reading})){until.store(0);clear.store(true);}}
 else if(previous)clear.store(true);
}
inline bool Current(std::uint64_t now,std::uint64_t vm,Lease& out) noexcept {
 const auto end=until.load(std::memory_order_acquire);
 if(!end||now>end||!leases.read(out)||!out.serial||armed.load()!=out.serial||now<out.tick||now-out.tick>150)return false;
 if(out.vm!=vm){Finish(out.serial);return false;}return out.reading&&out.lifetime;
}
inline bool Wanted() noexcept {return until.load(std::memory_order_acquire)||clear.load(std::memory_order_acquire);}
}
