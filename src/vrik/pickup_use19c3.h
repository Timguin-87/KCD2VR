#pragma once
#include <cmath>
#include <cstdint>
#include <cstring>
#include <atomic>
#define KCD2VR_PICKUP_NEAR_D40 1
#include "../core/door_use46.h"

namespace kcd2vr::pickup_use19c3 {
// D40: direct pickup is deliberately close to the native selection point. This
// is not a collision/contact claim. Assisted retains the older 25cm reach and
// its deliberate hold delay; the live Experiments choice needs no restart.
inline std::atomic<bool> nearOnlyD40{true};
inline std::atomic<std::uint64_t> reachEpochD40{1};
inline void setNearOnlyD40(bool direct) noexcept {
    if(nearOnlyD40.exchange(direct,std::memory_order_acq_rel)!=direct)
        reachEpochD40.fetch_add(1,std::memory_order_acq_rel);
}
// D49 / Claude M57: native selection points can sit inside the food mesh.
// Recorded cheese/chicken candidates at 11-20 cm were denied by the 10 cm
// radius despite an actual selected PickableItem. Keep native eligibility,
// target identity and both fresh samples; widen only this direct transport.
inline constexpr float kDirectReachD40=.20f,kAssistedReachD40=.25f;
// Native XI-A short Use starts on release. The physical Grip must remain closed
// until the native Picking Attach event. Only the proven nearby PickableItem
// route and the retail AnimDoor short use/unlock route get this split;
// contextual selection/locks remain native and use_other/long-hold is unchanged.
struct Candidate {
    std::uint64_t tick=0;
    std::uint32_t entity=0;
    int side=-1;
    bool object=false;
    float distance=0;
    const char* scriptClass=nullptr;
};
struct State {
    std::uint64_t press=0;
    int side=-1;
    bool chosen=false,pulseGesture=false,sent=false,cancelled=false,pickable63=false,waitingH12=false;
    std::uint32_t target63=0;std::uint64_t selectedAt63=0;
    std::uint64_t firstSelectionD40=0,lastSelectionD40=0,reachEpochD40=0;
    bool nearD40=false;std::uint64_t loosePressD40=0;
};
struct Plan {bool nativeHeld=false,pulse=false;std::uint32_t target=0;bool pickable63=false,suppress63=false;};
inline Plan update(State& s,bool enabled,int side,std::uint64_t press,bool held,
                   const Candidate& candidate,std::uint64_t now) noexcept {
    Plan out{};out.nativeHeld=held;
    const auto scoped=[&]() noexcept {out.pickable63=s.pickable63;out.suppress63=s.pulseGesture||s.waitingH12;out.nativeHeld=false;};
    if(!enabled||side<0||side>1||!press||!now||now<press){
        if(s.pulseGesture||s.waitingH12){s.cancelled=true;s.sent=false;scoped();}return out;
    }
    const auto reachEpoch=reachEpochD40.load(std::memory_order_acquire);
    if(s.side!=side||s.press!=press){s={};s.side=side;s.press=press;s.reachEpochD40=reachEpoch;}
    if(s.reachEpochD40!=reachEpoch){s.cancelled=true;scoped();return out;}
    if(!held){if(s.pulseGesture||s.waitingH12){s.cancelled=true;s.sent=false;scoped();}return out;}
    if(s.cancelled){if(s.pulseGesture||s.waitingH12)scoped();return out;}
    const bool current=candidate.object&&candidate.entity&&candidate.side==side&&candidate.tick&&now>=candidate.tick&&now-candidate.tick<=80&&
        std::isfinite(candidate.distance)&&candidate.distance>=0&&candidate.scriptClass&&candidate.scriptClass[0];
    // H12: selection can arrive after Grip. Before any transport, an absent,
    // wrong-hand or stale candidate owns only waiting/suppression, never a
    // generic held Use. Reaching a fresh item later is the same deliberate
    // squeeze, with a NEW qualification time rather than a backdated pulse.
    if(!s.sent&&(!s.chosen||s.pulseGesture||s.waitingH12)){
        const bool pickable=current&&std::strcmp(candidate.scriptClass,"PickableItem")==0;
        const bool door=current&&std::strcmp(candidate.scriptClass,"AnimDoor")==0;
        const float reach=pickable&&nearOnlyD40.load(std::memory_order_acquire)?kDirectReachD40:kAssistedReachD40;
        if(!current||((pickable||door)&&candidate.distance>reach)){
            s.chosen=false;s.pulseGesture=false;s.pickable63=false;s.waitingH12=true;
            s.target63=0;s.selectedAt63=0;s.firstSelectionD40=s.lastSelectionD40=0;s.nearD40=false;scoped();return out;
        }
        const bool pulse=pickable||door;
        const bool isNearPickupD40=pickable&&candidate.distance<=kDirectReachD40;
        if(isNearPickupD40&&s.target63==candidate.entity&&s.lastSelectionD40&&candidate.tick<s.lastSelectionD40){
            s.cancelled=true;scoped();return out;
        }
        if(!s.chosen||s.waitingH12||s.target63!=candidate.entity||s.pickable63!=pickable||s.pulseGesture!=pulse||
           s.nearD40!=isNearPickupD40){
            s.selectedAt63=now;s.firstSelectionD40=s.lastSelectionD40=0;
        }
        // Two distinct monotonic native selection samples while this squeeze
        // is live replace the unconditional 150ms delay for a nearby item.
        // Pre-press hover, repeated cached packets and another target earn none.
        if(isNearPickupD40&&candidate.tick>=press){
            if(!s.firstSelectionD40)s.firstSelectionD40=candidate.tick;
            s.lastSelectionD40=candidate.tick;
        }
        s.chosen=true;s.waitingH12=false;s.pickable63=pickable;s.pulseGesture=pulse;s.target63=candidate.entity;s.nearD40=isNearPickupD40;
    }
    if(!s.pulseGesture)return out;scoped();
    if(s.pickable63&&!s.sent){
        // Selection qualifies only transport; native Use/Attach and inventory
        // ownership remain untouched. Releasing before qualification cancels.
        if(s.nearD40){
            if(!s.firstSelectionD40||s.lastSelectionD40<=s.firstSelectionD40)return out;
        }else if(now-press<150||now-s.selectedAt63<80)return out;
    }
    if(!s.sent){out.pulse=true;out.target=s.target63;}
    return out;
}

inline void complete(State& s,const Plan& plan,bool sent) noexcept {
    if(plan.pulse){s.sent=sent;if(!sent)s.cancelled=true;}
}
inline bool physicalHeld(const State& s,int side,std::uint64_t press,bool held) noexcept {
    return held&&s.pulseGesture&&s.sent&&!s.cancelled&&s.side==side&&s.press==press;
}
}
