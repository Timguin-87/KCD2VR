#pragma once
#include "snapshot_mailbox.h"
#include <cmath>
namespace kcd2vr::pickup_grip_d64 {
// One native pickup terminal consumes only the physical Grip occurrence which
// carried it. No timeout, global fresh-edge acquisition rule or zone cooldown.
struct Occurrence {
    std::uintptr_t character=0;
    std::uint64_t body=0,tracking=0,sample=0,tick=0,token=0;
};
inline bool sameOwner(const Occurrence& a,const Occurrence& b) noexcept {
    return a.character&&a.character==b.character&&a.body&&a.body==b.body;
}
inline bool released(const Occurrence& use,const Occurrence& open) noexcept {
    return sameOwner(use,open)&&open.tick>=use.tick&&
        (open.tracking!=use.tracking||open.sample>=use.sample);
}
class Retirement {
    core::SnapshotMailbox<Occurrence> used_[2],open_[2];
public:
    template<class Input> void observe(const Input& in) noexcept {
        if(!in.character||!in.epoch||!in.sample||!in.tick)return;
        for(int side=0;side<2;++side)if(in.tracked[side]&&std::isfinite(in.grip[side])&&in.grip[side]<.25f){
            const Occurrence next{in.character,in.epoch,in.trackingEpoch,in.sample,in.tick,0};
            Occurrence previous{};
            // The raw input producer is serialized. An out-of-order source
            // must not erase a later physical opening and re-block a used Grip.
            if(open_[side].read(previous)&&sameOwner(previous,next)&&
                (next.tick<previous.tick||(next.tracking==previous.tracking&&next.sample<previous.sample)))continue;
            (void)open_[side].publish(next);
        }
    }
    template<class Input> bool consume(std::uintptr_t character,std::uint64_t body,
        std::uint64_t token,int side,const Input& in) noexcept {
        if(side<0||side>1||!token||!body||!character||in.character!=character||in.epoch!=body||
           !in.trackingEpoch||!in.sample||!in.tick||!in.tracked[side]||
           !std::isfinite(in.grip[side])||in.grip[side]<.25f)return false;
        return used_[side].publish({character,body,in.trackingEpoch,in.sample,in.tick,token});
    }
    bool blocked(int side,std::uintptr_t character,std::uint64_t body) const noexcept {
        if(side<0||side>1||!character||!body)return false;
        Occurrence use{},open{};
        if(!used_[side].read(use)||!use.token||use.character!=character||use.body!=body)return false;
        return !open_[side].read(open)||!released(use,open);
    }
};
inline Retirement retirement;
}
