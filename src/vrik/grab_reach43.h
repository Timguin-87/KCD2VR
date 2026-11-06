#pragma once
// C43/D44 GRAB REACH: a VR hand Use sent for a PickableItem while the qualified native grasp capability is ready opens one
// bounded window per Use in which the using wrist stays on the controller. The R14 "natural hands" blend toward Henry's
// authored 3d_pick reach does not run inside it; since C10R18 arms the grasp only at the native Attach event the R12B
// controller-wrist rule began ~0.9 s late (Use -> Attach), and that 0.9 s reach was the "grabbing animation toward the
// object". Doors, chests and every other usable never open a window (they keep the native approach). Native pickup rules,
// the Picking action, the Attach event and the item transfer are untouched: only our own hand blend is withheld.
// Pure data + decisions, no native reads; host-testable (grab_reach43_test).
#include "../core/snapshot_mailbox.h"
#include <atomic>
#include <cmath>
#include <cstdint>
namespace kcd2vr::grab43 {
inline constexpr std::uint64_t kWindowMs=4000; // same bound as the native-attach arm (NO_FRESH_VR_HAND_USE)
struct Context {std::uintptr_t actor=0;std::uint64_t pose=0,tracking=0;};
struct Use { std::uint64_t tick=0; std::uint32_t entity=0,serial=0;Context context{};std::uint64_t cancellation=0; };
inline std::atomic<bool> handStays{true}; // INI VrGrabHandStaysOnController
inline core::SnapshotMailbox<Use> use[2]{};
inline std::atomic<std::uint64_t> cancellation[2]{};
inline std::atomic<std::uint32_t> useSerial{0};
inline bool note(int side,std::uint32_t entity,std::uint64_t now,Use* out=nullptr,Context context={}) noexcept {
    if(side<0||side>1||!entity||!now)return false;
    Use next{now,entity,useSerial.fetch_add(1,std::memory_order_acq_rel)+1,context};
    next.cancellation=cancellation[side].load(std::memory_order_acquire);
    if(!use[side].publish(next))return false;
    if(out)*out=next;
    return true;
}
inline void clear(int side) noexcept {
    // Cancellation cannot be lost to mailbox contention. A later note starts
    // a new generation; an in-flight old publication cannot revive this one.
    if(side>=0&&side<2)cancellation[side].fetch_add(1,std::memory_order_acq_rel);
}
inline bool read(int side,Use& u) noexcept {
    u={};return side>=0&&side<2&&use[side].read(u)&&u.tick&&u.entity&&u.serial&&
        u.cancellation==cancellation[side].load(std::memory_order_acquire);
}
inline bool current(const Use& u,Context context) noexcept {
    return context.actor&&context.pose&&context.tracking&&u.context.actor==context.actor&&
        u.context.pose==context.pose&&u.context.tracking==context.tracking;
}
inline bool fresh(const Use& u,std::uint64_t now) noexcept {return u.tick&&u.serial&&now>=u.tick&&now-u.tick<=kWindowMs;}
// The ready native pickup capability qualifies the request; the live prop owner
// is deliberately not required before Attach. A wrist already owned by a
// tracked interaction needs no veto. Native item/action authority is unchanged.
inline bool veto(bool setting,bool graspAvailable,bool freshWindow,bool wristOwned) noexcept {
    return setting&&graspAvailable&&freshWindow&&!wristOwned;
}
// Edge bookkeeping for the F5 rung 1 receipts (one line per edge per Use serial).
struct Tracker {
    std::uint32_t serial=0;bool open=false,reachEdge=false,vetoed=false,nativeSeen=false,haveStart=false;
    float maxPull=0,start[3]{};std::uint64_t openedAt=0;
    // Returns true when this is the first lease-eligible frame of the window (one REACH_* line).
    bool reach(const Use& u,bool vetoedNow,float pullM) noexcept {
        if(serial!=u.serial){*this=Tracker{};serial=u.serial;}
        nativeSeen=true;if(std::isfinite(pullM)&&pullM>maxPull)maxPull=pullM;
        if(vetoedNow)vetoed=true;
        const bool first=!reachEdge;reachEdge=true;return first;
    }
    // Window open/close on the committed-frame clock. Returns 1 on open, 2 on close, 0 otherwise.
    int frame(const Use& u,bool freshWindow,bool wristOwned,const float (&henry)[3],std::uint64_t now,float& movedM,const char*& end) noexcept {
        if(serial!=u.serial){*this=Tracker{};serial=u.serial;}
        movedM=0;end="";
        if(!open){
            if(!freshWindow||wristOwned)return 0;
            open=true;openedAt=now;haveStart=true;for(int i=0;i<3;++i)start[i]=henry[i];return 1;
        }
        if(freshWindow&&!wristOwned)return 0;
        open=false;end=wristOwned?"ATTACH_ADOPTED":"EXPIRED";
        if(haveStart){double d=0;for(int i=0;i<3;++i){const double v=double(henry[i])-start[i];d+=v*v;}movedM=float(std::sqrt(d));}
        return 2;
    }
};
}
