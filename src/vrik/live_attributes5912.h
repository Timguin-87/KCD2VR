#pragma once
#include "../core/snapshot_mailbox.h"
#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace kcd2vr::live5912 {
// Values copied on Henry's native update; never dereference Soul from the pose
// thread. Strength/agility are CURRENT BASE LEVELS, not buff-adjusted levels.
struct Snapshot {
    std::uintptr_t player=0,entity=0,soul=0;
    std::uint64_t tick=0,recovery=0;
    double strength=10,agility=10,stamina=1,current=0,maximum=0;
    double strengthEffective=-1,agilityEffective=-1; // R59.16C10R17: the game's effective levels (buffs, drink, exhaustion); -1 = unread
    int skills26[5]{-1,-1,-1,-1,-1}; // C26c base weapon skills: sword 16, heavy 17, large 23, dagger 22, unarmed 24 (soul+0x5E8+8*id)
    bool valid=false;
};
inline core::SnapshotMailbox<Snapshot> latest;
inline std::atomic<bool> enabled{true};
// R59.16C10R17: the producer published only for F4 press 4 (motion59c4::selected); press 1/2/3 drove the weapon with neutral stats,
// no fatigue ramp and no exhaustion. The consumer's VrLiveAttributesAllRungs now gates the producer too.
inline std::atomic<bool> allRungs{true};
inline bool usable(const Snapshot& s,std::uintptr_t player,std::uintptr_t entity,
                   std::uint64_t now,std::uint64_t recovery) noexcept {
    return enabled.load()&&s.valid&&s.player==player&&s.entity==entity&&s.soul&&
        s.recovery==recovery&&s.tick&&now>=s.tick&&now-s.tick<=100&&
        std::isfinite(s.strength)&&s.strength>=0&&s.strength<=1000&&
        std::isfinite(s.agility)&&s.agility>=0&&s.agility<=1000&&
        std::isfinite(s.stamina)&&s.stamina>=0&&s.stamina<=1&&
        std::isfinite(s.maximum)&&s.maximum>0&&std::isfinite(s.current)&&s.current>=0;
}
// Mailbox contention is not recovery to full stamina. Reuse only the original
// still-fresh observation; an explicit invalid observation revokes it at once.
inline bool readBounded(Snapshot& cache,bool received,const Snapshot& observation,
        std::uintptr_t player,std::uintptr_t entity,std::uint64_t now,std::uint64_t recovery) noexcept {
    if(received)cache=observation;
    if(!usable(cache,player,entity,now,recovery)){cache={};return false;}
    return true;
}

}
