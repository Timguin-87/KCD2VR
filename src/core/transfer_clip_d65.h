#pragma once
#include <cstdint>
#include <cmath>
#include <algorithm>
namespace kcd2vr::transfer_clip_d65 {
// Exact action identity is copied only from an owner-thread membership proof.
struct Key {
    std::uintptr_t controller=0,character=0,skeleton=0,instance=0,parent=0,child=0,scope=0;
    std::uint64_t lifetime=0,mode=0;std::uint32_t token=0;int layer=-1;
};
inline bool sameAction(const Key& a,const Key& b) noexcept {
    return a.child&&a.controller==b.controller&&a.character==b.character&&a.skeleton==b.skeleton&&
        a.instance==b.instance&&a.lifetime==b.lifetime&&a.child==b.child&&a.token==b.token;
}
inline bool same(const Key& a,const Key& b) noexcept {
    return sameAction(a,b)&&a.parent==b.parent&&a.scope==b.scope&&a.mode==b.mode&&a.layer==b.layer;
}
inline float rate(float v) noexcept {return std::isfinite(v)?std::clamp(v,1.f,8.f):1.f;}
struct Admission {bool exactParent=false,fullVr=false,currentInput=false,root=false,singleClip=false,foreign=false;};
inline bool eligible(const Admission& a,float requested) noexcept {
    return rate(requested)>1.f&&a.exactParent&&a.fullVr&&a.currentInput&&a.root&&a.singleClip&&!a.foreign;
}
enum class Restore {Pending,GiveBack,Foreign,AlreadyNative,Gone};
struct Ledger {
    Key key{},retired{};float saved=1.f,written=1.f;bool active=false,uncertain=false;
    bool begin(const Key& k,float current,float requested) noexcept {
        if(active||sameAction(k,retired)||!k.controller||!k.character||!k.skeleton||!k.child||!k.scope||
            k.layer<0||k.layer>15||current!=1.f||rate(requested)<=1.f)return false;
        key=k;saved=current;written=rate(requested);active=true;uncertain=true;return true;
    }
    void confirmed() noexcept {uncertain=false;}
    Restore restore(bool readable,bool sameSkeleton,float current) const noexcept {
        if(!active)return Restore::Gone;
        if(!readable)return Restore::Pending;
        if(!sameSkeleton)return Restore::Gone;
        if(!std::isfinite(current)||current<0.f)return Restore::Pending;
        if(current==written)return Restore::GiveBack;
        if(uncertain&&current==saved)return Restore::AlreadyNative;
        return Restore::Foreign;
    }
    // Caller clears only after a successful setter+readback or proven loss.
    void settled(bool foreign=false) noexcept {
        if(foreign)retired=key;
        key={};saved=written=1.f;active=uncertain=false;
    }
};
// An unowned request may name only one native Picking action. A later action
// cannot reuse the same transported Use even within its freshness window.
struct PickupKey {std::uint64_t serial=0,generation=0;std::uintptr_t action=0,scope=0;std::uint32_t token=0;};
struct PickupJoin {
    PickupKey key{};std::uint64_t retired=0;
    void retire(std::uint64_t serial) noexcept {retired=std::max(retired,serial);}
    bool accept(const PickupKey& k,bool currentUse,bool changedRoot) noexcept {
        if(!currentUse||!changedRoot||!k.serial||k.serial<=retired||!k.generation||!k.action||!k.scope)return false;
        if(key.serial==k.serial)return key.generation==k.generation&&key.action==k.action&&key.scope==k.scope&&key.token==k.token;
        if(key.serial>k.serial)return false;
        key=k;return true;
    }
};
}
