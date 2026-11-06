#pragma once
#include <atomic>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <cmath>
#include "riding_aliases_r95.h"
namespace kcd2vr::riding_vrik_d76 {
// Native mount/dismount clips own their transfer. A positive seated pose after
// transfer admits tracked upper body on that same scan (R97: no settle wait; the
// R94R1 log shows the native seat already final when the mount clip leaves).
// Native clips/events still run beneath that presentation owner.
inline std::atomic<std::uint64_t> mode{3};
// Separate from saved preferences/calibration. Retires only mounted presentation caches.
inline std::atomic<std::uint64_t> lifetimeR95{1};
inline bool Get() noexcept {return (mode.load(std::memory_order_acquire)&1)!=0;}
// R98: last tick a seated rider, mount or dismount clip was current on Henry
// (player animation scan, any riding option). Body height COMBINED uses native
// height for this short window only; it is evidence with an age, not a latch.
inline std::atomic<std::uint64_t> riderSeenTickR98{0};
inline constexpr std::uint64_t kRiderRecentMsR98=750;
inline bool RiderRecentR98(std::uint64_t now) noexcept {
 const auto seen=riderSeenTickR98.load(std::memory_order_acquire);
 return seen&&now>=seen&&now-seen<=kRiderRecentMsR98;
}
inline void Set(bool on) noexcept {
 auto old=mode.load(std::memory_order_acquire);
 for(;;){if(bool(old&1)==on)return;
  if(mode.compare_exchange_weak(old,((old+2)&~std::uint64_t(1))|std::uint64_t(on),std::memory_order_acq_rel))return;}
}
// The standing calibration anchor is in the former native/model posture.
// A mounted model-space head may legitimately be more than75cm from it. Use
// only the current untouched native head in that exact gait, never its old
// calibration fallback. This helper has no camera or tracking publication.
template<class V> inline bool LiveHeadUsableD80(bool mounted,const V& head,const V& calibrated) noexcept {
 if(!std::isfinite(head.x)||!std::isfinite(head.y)||!std::isfinite(head.z))return false;
 if(mounted)return true;
 const auto x=head.x-calibrated.x,y=head.y-calibrated.y,z=head.z-calibrated.z;
 return std::sqrt(x*x+y*y+z*z)<.75;
}
inline bool UpperBodyD80(int rung,bool alchemyPassenger,bool reloadHand,bool pairHands) noexcept {
 return rung>=3&&!alchemyPassenger&&!reloadHand&&!pairHands;
}
inline bool Gait(const char* name,int layer) noexcept {
 if(!name||layer!=0)return false;
 // D78 records the horse idle, but the same retail male_horse database also
 // supplies neutral, uphill and variation stationary riders. None needs a
 // joystick/gait transition to establish the mounted coordinate context.
 // Exact layer-zero aliases only; reading, attacks and transfers stay native.
 for(const auto* n:{"relaxed_idle_rider_01","relaxed_idle_rider_01_horse","relaxed_idle_rider_01_uphill",
                    "relaxed_idle_rider_01_var1","relaxed_idle_rider_01_var1_horse",
                    "relaxed_idle_rider_01.caf","relaxed_idle_rider_01_horse.caf","relaxed_idle_rider_01_uphill.caf",
                    "relaxed_idle_rider_01_var1.caf","relaxed_idle_rider_01_var1_horse.caf",
                    "2d_horse_walk","2d_horse_trot","2d_horse_canter","2d_horse_gallop",
                    "2d_horse_walk.bspace","2d_horse_trot.bspace","2d_horse_canter.bspace","2d_horse_gallop.bspace"})
  if(!std::strcmp(name,n))return true;
 return false;
}
inline bool SeatedR95(const char* name,int layer) noexcept {
 return layer>=0&&layer<=8&&(Gait(name,layer)||AliasR95(name,seatedAliasesR95));
}
inline bool MountD81(const char* name) noexcept {return AliasR95(name,mountAliasesR95);}
inline bool DismountD81(const char* name) noexcept {return AliasR95(name,dismountAliasesR95);}
// R96: Henry getting up from the ground (thrown off the horse at 18:13:21 in the R94R1 log, then
// getup_ground_front_player 18:13:25 ran under VRIK with the camera 0.6-0.7 m off his head). Like mount and
// dismount this is a whole-body transfer: native only, VRIK returns on the next ordinary pose. Exact aliases.
inline bool GetupR96(const char* name) noexcept {
 if(!name)return false;
 for(const auto* n:{"getup_ground_back_player","getup_ground_front_player","getup_ground_left_player","getup_ground_right_player",
                    "getup_ground_back","getup_ground_front","getup_ground_left","getup_ground_right"})
  if(!std::strcmp(name,n))return true;
 return false;
}
struct SessionD81 {
 std::uintptr_t actor=0;std::uint64_t policy=0,completedAt=0,lastObserved=0,bodyEpochR95=0,loadEpisodeR96=0;
 bool completed=false;
};
struct ObservationD81 {
 std::uintptr_t actor=0;std::uint64_t policy=0,now=0;
 bool qualified=false,mount=false,dismount=false,gait=false,protectedAction=false;
 std::uint64_t bodyEpochR95=0,loadEpisodeR96=0;
};
// Every admission requires a current positive seated alias. No stale session can
// make a standing/unknown body mounted. Actor, pose/load epoch, option and
// transfer changes retire the mounted frame without changing calibration or
// preferences. A new seat lifetime starts only AFTER mount/alignment/dismount
// disappear; ordinary seated gait changes keep it.
inline bool AdvanceD81(SessionD81& s,const ObservationD81& i) noexcept {
 const auto retire=[&]() noexcept {if(s.actor||s.completed)lifetimeR95.fetch_add(1,std::memory_order_acq_rel);s={};};
 if(!i.actor||!(i.policy&1)||!i.qualified){retire();return false;}
 if(s.actor!=i.actor||s.policy!=i.policy||s.bodyEpochR95!=i.bodyEpochR95||
    s.loadEpisodeR96!=i.loadEpisodeR96||i.now<s.lastObserved){
  retire();s.actor=i.actor;s.policy=i.policy;s.bodyEpochR95=i.bodyEpochR95;s.loadEpisodeR96=i.loadEpisodeR96;
 }
 s.lastObserved=i.now;
 if(i.mount||i.dismount||!i.gait){
  if(s.completed)lifetimeR95.fetch_add(1,std::memory_order_acq_rel);
  s.completed=false;s.completedAt=0;return false;
 }
 if(!s.completed){s.completed=true;s.completedAt=i.now;lifetimeR95.fetch_add(1,std::memory_order_acq_rel);}
 return !i.protectedAction;
}
}
