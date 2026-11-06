#pragma once
#include <atomic>
#include <cstdint>
#include "../core/snapshot_mailbox.h"
namespace kcd2vr::kick5916c3 {
inline constexpr std::uint64_t lifetime=600;
struct Owner {
    std::uintptr_t player=0;std::uint64_t key=0,generation=0,body=0,request=0;
};
inline bool same(const Owner&a,const Owner&b) noexcept {
    return a.player==b.player&&a.key==b.key&&a.generation==b.generation&&a.body==b.body&&a.request==b.request;
}
struct Ticket {std::uint64_t id=0,tick=0;Owner owner{};};
inline bool fresh(const Ticket&t,const Owner&o,std::uint64_t now) noexcept {
    return t.id&&t.tick&&t.owner.player&&t.owner.key&&same(t.owner,o)&&now>=t.tick&&now-t.tick<=lifetime;
}
inline core::SnapshotMailbox<Ticket> requests;
inline std::atomic<std::uint64_t> nextId{0};
inline bool publish(const Owner&owner,std::uint64_t now) noexcept {
    return owner.player&&owner.key&&now&&requests.publish(Ticket{++nextId,now,owner});
}
// Native-frame consumer only, under the existing physical-owner mutex. The
// watermarks do not reset with a weapon or pose, so an old press cannot replay.
struct Delivery {
    Ticket current{};std::uint64_t highest=0;bool prepared=false;
    bool begin(const Ticket&t,const Owner&o,std::uint64_t now) noexcept {
        if(!fresh(t,o,now)||t.id<=highest||prepared)return false;
        highest=t.id;current=t;prepared=true;return true;
    }
    bool release(const Ticket&t,const Owner&o,std::uint64_t now) noexcept {
        if(!prepared||t.id!=current.id||t.tick!=current.tick||!fresh(t,o,now)||!same(t.owner,current.owner))return false;
        prepared=false;return true;
    }
    bool active(const Owner&o,std::uint64_t now) const noexcept {return prepared&&fresh(current,o,now);}
    void cancel() noexcept {prepared=false;current={};} // retain watermark
};
inline Delivery delivery;
inline bool guardReject(bool natural,bool held,bool kick) noexcept {return natural&&held&&!kick;}
}
