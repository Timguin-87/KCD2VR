#pragma once
#include "snapshot_mailbox.h" // DS-F32 (Codex 1350): synchronized publication instead of a plain shared array
#include <atomic>
#include <cmath> // DS-F53 (Codex F45): std::isfinite needs its own header, not transitive includes
#include <cstdint>
// DS-F16: two-stage door nomination, stage 1 - the native AnimDoor descriptor
// observed BEFORE the Grip press (a committed hand near the door, palm
// accepting the target, descriptor origin joining). A cached descriptor grants
// NOTHING by itself: the press-scoped 20 cm physics surface, the native Use and
// the admission all stay exactly as they are. It only lets the press edge issue
// the surface ray one pass earlier (stage 2, WarmDoorSurfaceOnPress66).
namespace kcd2vr::door_pre_press66 {
struct Entry {
    std::uint32_t target=0;int side=-1;
    std::uintptr_t entity=0,descriptorEntityH12=0;
    std::uint64_t tick=0,sourceGeneration=0,poseEpoch=0,clearGen=0; // DS-F41 (Codex): generation tag captured at publication
    float distance=1e9f; // DS-F47: palm-origin to bounds-centre squared - the NEAREST eligible door wins
    float point[3]{};char cls[32]{};
};
inline core::SnapshotMailbox<Entry> cache[2]{}; // DS-F32: one synchronized slot per side (no torn reads)
inline std::atomic<std::uint64_t> clearGen[2]{{0},{0}}; // DS-F41: invalidation is generation-only; a delayed stale publish can never resurrect the entry
inline constexpr std::uint64_t kFreshMs=500;
inline void store(int side,const Entry& e) noexcept {if(side>=0&&side<2){
    auto t=e;t.clearGen=clearGen[side].load(std::memory_order_acquire); // tag BEFORE publication
    (void)cache[side].publish(t);}}
inline void clear(int side) noexcept {if(side>=0&&side<2)clearGen[side].fetch_add(1,std::memory_order_acq_rel);} // DS-F29/F41: generation only
inline bool read(int side,std::uint64_t now,std::uint64_t generation,Entry& out) noexcept {
    if(side<0||side>1||!now||!generation)return false;
    const auto g=clearGen[side].load(std::memory_order_acquire);
    if(!cache[side].read(out))return false;
    if(out.clearGen!=g)return false; // published under an older generation: stale
    if(clearGen[side].load(std::memory_order_acquire)!=g)return false; // cleared mid-read
    return out.target&&out.target<=16777215u&&out.entity&&out.side==side&&out.sourceGeneration==generation&&
        out.tick&&now>=out.tick&&now-out.tick<=kFreshMs&&out.poseEpoch;
}
// DS-F50 (Codex F43 review): nearest-eligibility MUST be measured from the
// SAME current palm origin - an old entry's cached distance came from an OLD
// pose. Recompute the old nominee's distance to its point from the CURRENT
// origin; the new door wins only when it is strictly nearer NOW.
inline bool nearerNow(const Entry& old,float px,float py,float pz,float newD2) noexcept {
    const float odx=old.point[0]-px,ody=old.point[1]-py,odz=old.point[2]-pz;
    const float od2=odx*odx+ody*ody+odz*odz;
    return std::isfinite(od2)&&std::isfinite(newD2)&&newD2<od2;
}
// DS-F53 (Codex F45): a nominee with a nonfinite point or a stale owner join
// is INVALID, not merely "never nearer" - the producer clears it and admits
// the current candidate.
inline bool qualified(const Entry& old,std::uintptr_t currentPlayer,std::uint64_t currentPoseEpoch) noexcept {
    return std::isfinite(old.point[0])&&std::isfinite(old.point[1])&&std::isfinite(old.point[2])&&
        old.entity==currentPlayer&&old.poseEpoch==currentPoseEpoch;
}
}
