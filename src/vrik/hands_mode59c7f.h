#pragma once
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::hands59c7f {
// Requested rung is not proof of hook readiness or ownership. Only admitted
// rung 2 may arbitrate hands. Native acquisition still validates the object.
inline std::atomic<int> rung{0};
inline std::atomic<bool> admitted{false},pickupReady{false},activityVisual{false};
inline std::atomic<unsigned> grabMask{0};
inline std::atomic<std::uint64_t> sampleTick{0},epoch{1};
inline bool active() noexcept {return rung.load()==2&&admitted.load();}
inline unsigned grabs(std::uint64_t now) noexcept {
    const auto t=sampleTick.load(std::memory_order_acquire);
    return active()&&t&&now>=t&&now-t<=80?grabMask.load(std::memory_order_acquire):0;
}
inline void clear() noexcept {
    admitted=false;pickupReady=false;activityVisual=false;grabMask=0;sampleTick=0;epoch.fetch_add(1);
}
struct Hand {bool tracked=false,empty=false;float grip=0,trigger=0;};
struct Candidate {bool object=false;int side=-1;std::uint64_t tick=0;float distance=0;};
struct Arbitration {
    bool open[2]{};std::uint64_t pendingPressD48[2]{};int owner=-1;std::uint64_t tick=0,releaseTick=0,generation=0;
    unsigned update(bool enabled,std::uint64_t generationNow,std::uint64_t now,
                    const Hand (&h)[2],Candidate c) noexcept {
        if(!enabled||!now||generation!=generationNow||(tick&&(now<tick||now-tick>80))){
            *this={};generation=generationNow;
            // An entry or lost sample requires a fresh open hand before a grab.
            if(!enabled||!now)return 0;
        }
        tick=now;bool edge[2]{};
        for(int i=0;i<2;++i){
            const bool valid=h[i].tracked&&h[i].empty&&std::isfinite(h[i].grip)&&std::isfinite(h[i].trigger);
            if(!valid){open[i]=false;pendingPressD48[i]=0;if(owner==i){owner=-1;releaseTick=0;}continue;}
            if(h[i].grip<.30f){open[i]=true;pendingPressD48[i]=0;if(owner==i&&!releaseTick)releaseTick=now;}
            else if(h[i].grip>=.58f&&open[i]){edge[i]=true;open[i]=false;pendingPressD48[i]=now;}
        }
        // One short input-release interval lets the game-frame prop consumer
        // observe the real grip release and retain achieved body momentum.
        // This never holds a tracking pose; missing tracking releases immediately.
        if(owner>=0&&releaseTick&&now-releaseTick>=60){owner=-1;releaseTick=0;}
        const bool withinReach=c.object&&c.side>=0&&c.side<2&&c.tick&&now>=c.tick&&now-c.tick<=80&&
            std::isfinite(c.distance)&&c.distance>=0&&c.distance<=.25f;
        // Native selection can arrive after Grip. Preserve the same unconsumed
        // squeeze briefly; release, trigger/combat intent and loss cancel it.
        if(edge[0]&&edge[1])pendingPressD48[0]=pendingPressD48[1]=0;
        for(int i=0;i<2;++i)if(pendingPressD48[i]&&
            (now<pendingPressD48[i]||now-pendingPressD48[i]>300||h[i].trigger>=.30f))pendingPressD48[i]=0;
        if(owner<0&&withinReach&&pendingPressD48[c.side]&&!pendingPressD48[1-c.side]&&h[c.side].grip>=.30f){
            owner=c.side;releaseTick=0;pendingPressD48[c.side]=0;
        }
        return owner>=0?1u<<owner:0;
    }
};
}
