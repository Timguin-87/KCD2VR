#pragma once
#include "module.h"
#include "../../vrik/motion_owned59c4.h"
#include <atomic>
#include <cstdint>
#include <mutex>
namespace kcd2vr::combo_lineage19 {
struct Owner {
    motion59c4::Key key{};
    std::uintptr_t actor=0,state=0,manager=0,target=0,targetEntity=0,action=0;
    std::uint32_t targetId=0;std::uint64_t release=0,modeGeneration=0,tick=0;unsigned mode=0;
};
inline bool valid(const Owner& r) noexcept {return motion59c4::valid(r.key)&&r.actor&&r.state&&r.manager&&r.target&&r.targetEntity&&r.targetId&&r.action&&r.release&&r.modeGeneration&&r.tick&&r.mode;}
// A combo may continue with a NEW gesture against the same exact owner/target.
inline bool sameLineage(const Owner& a,const Owner& b) noexcept {
    return valid(a)&&a.key.player==b.key.player&&a.key.weapon==b.key.weapon&&a.key.generation==b.key.generation&&
        a.key.body==b.key.body&&a.key.request==b.key.request&&a.key.recovery==b.key.recovery&&a.actor==b.actor&&a.state==b.state&&
        a.manager==b.manager&&a.target==b.target&&a.targetEntity==b.targetEntity&&a.targetId==b.targetId&&
        a.modeGeneration==b.modeGeneration&&a.mode==b.mode;
}
class Grace {
    mutable std::mutex mutex_;Owner owner_{};std::atomic<std::uint64_t> generation_{1};
public:
    std::uint64_t generation() const noexcept{return generation_.load(std::memory_order_acquire);}
    void clear() noexcept {generation_.fetch_add(1,std::memory_order_acq_rel);std::lock_guard<std::mutex> lock(mutex_);owner_={};}
    bool publish(const Owner& owner) noexcept {
        if(!valid(owner))return false;std::lock_guard<std::mutex> lock(mutex_);
        if(owner.modeGeneration!=generation())return false;owner_=owner;return true;
    }
    bool read(Owner& out) const noexcept {std::lock_guard<std::mutex> lock(mutex_);out=owner_;return valid(out)&&out.modeGeneration==generation();}
    bool active(const Owner& current,std::uint64_t now,unsigned window) const noexcept {
        Owner saved{};return window&&read(saved)&&sameLineage(saved,current)&&motion59c4::fresh(saved.tick,now,window);
    }
};
inline Grace grace;
// Native-open timing has a different anchor from an eventual accepted-hit
// receipt. nextSerial is process-monotonic; retain the consumed high-water even
// when a lease is retired so an old native release cannot be rearmed after a
// target/mode/recovery excursion or a late accepted-hit publication.
struct SlotLeaseF1 {Owner owner{};unsigned window=0;};
inline bool windowAllowedF6(unsigned window,bool explicitFiveSecondTest) noexcept {
    return window&&(window<=3000||(explicitFiveSecondTest&&window==5000));
}
class SlotWindowF1 {
    mutable std::mutex mutex_;SlotLeaseF1 lease_{};std::uint64_t consumed_=0;
public:
    bool consume(const Owner& owner,unsigned window,bool explicitFiveSecondTest=false) noexcept {
        // Ordinary leases retain their 3s ceiling; only the explicit 5s test may use exactly 5000ms.
        if(!valid(owner)||!windowAllowedF6(window,explicitFiveSecondTest))return false;
        std::lock_guard<std::mutex> lock(mutex_);
        if(owner.release<=consumed_||owner.modeGeneration!=grace.generation())return false;
        consumed_=owner.release;lease_={owner,window};return true;
    }
    void retire() noexcept {std::lock_guard<std::mutex> lock(mutex_);lease_={};}
    void spend(std::uint64_t release) noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        if(release>consumed_){consumed_=release;lease_={};}
    }
    bool read(SlotLeaseF1& out,std::uint64_t& consumed) const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);out=lease_;consumed=consumed_;
        return valid(out.owner)&&out.owner.modeGeneration==grace.generation();
    }
};
inline SlotWindowF1 nativeSlot;
// A provisional offer is usable only by the enclosing exact native hit stack
// AFTER that stack has an observed applied hit result. It is never a recent-time guess.
inline thread_local const Owner* offer=nullptr;
struct OfferScope {const Owner* old;explicit OfferScope(const Owner* owner) noexcept:old(offer){offer=owner;}~OfferScope(){offer=old;}
    OfferScope(const OfferScope&)=delete;OfferScope& operator=(const OfferScope&)=delete;};
}
