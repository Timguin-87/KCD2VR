#pragma once
#include "../../core/snapshot_mailbox.h"
#include "options_d65c.h"
#include <atomic>
#include <cmath>
#include <cstdint>
// D65: no frozen tracking. A release vector belongs to ONE constructed native
// firing action; it is exposed to Shoot only on that action's callback stack.
namespace kcd2vr::bow_physical_aim_d65c {
inline constexpr std::uint64_t kMaxReleaseAgeMs=1500; // action lifetime, NOT tracking grace
struct Snapshot {
    double dir[3]{},up[3]{},nock[3]{};
    void* player=nullptr;void* ownerInterface=nullptr;void* launcher=nullptr;void* action=nullptr;
    std::uint64_t key=0,generation=0,body=0,request=0,tracking=0,sample=0,tick=0,serial=0,revision=0;
};
using Current=bool(*)(const Snapshot&) noexcept;
inline std::atomic<Current> current{nullptr};
inline core::SnapshotMailbox<Snapshot> mailbox;
inline std::atomic<std::uint64_t> lifetime{1},consumed{0};
inline std::atomic<bool> hooksReady{false};
inline thread_local Snapshot* releaseDraft=nullptr;
inline thread_local const Snapshot* shotScope=nullptr;
enum class Refusal : unsigned char { None, Disabled, NoRelease, WrongAction, ActorMismatch, LauncherMismatch, Stale, ContextLost, Consumed, Invalid };
inline bool finite3(const double* p) noexcept {return std::isfinite(p[0])&&std::isfinite(p[1])&&std::isfinite(p[2]);}
inline bool unit3(const double* p) noexcept {if(!finite3(p))return false;const double n=p[0]*p[0]+p[1]*p[1]+p[2]*p[2];return n>.98&&n<1.02;}
inline void retire() noexcept {lifetime.fetch_add(1,std::memory_order_acq_rel);}
inline void retire(std::uint64_t serial) noexcept {if(serial)lifetime.compare_exchange_strong(serial,serial+1,std::memory_order_acq_rel);}
inline bool live(const Snapshot& s) noexcept {
    return s.serial&&s.serial==lifetime.load(std::memory_order_acquire)&&
        s.revision==ranged_options_d65c::bowAimRevision.load(std::memory_order_acquire)&&
        ranged_options_d65c::bowPhysicalAim.load(std::memory_order_acquire)&&hooksReady.load(std::memory_order_acquire);
}
struct ReleaseScope {
    Snapshot value{};Snapshot* prior=nullptr;bool finished=false;
    explicit ReleaseScope(Snapshot s) noexcept: value(s),prior(releaseDraft) {
        // A new release, including a rejected/invalid replacement, retires old aim.
        value.serial=lifetime.fetch_add(1,std::memory_order_acq_rel)+1;
        value.revision=ranged_options_d65c::bowAimRevision.load(std::memory_order_acquire);
        const bool valid=value.player&&value.ownerInterface&&value.key&&value.generation&&value.body&&value.request&&value.tracking&&
            value.sample&&value.tick&&unit3(value.dir)&&finite3(value.up)&&finite3(value.nock)&&live(value);
        releaseDraft=valid?&value:nullptr;
    }
    bool finish(bool nativeCallReturned) noexcept {
        finished=true;releaseDraft=prior;
        if(nativeCallReturned&&value.action&&consumed.load(std::memory_order_acquire)==value.serial)return true;
        if(!nativeCallReturned||!value.action||!live(value)){retire(value.serial);return false;}
        return true;
    }
    ~ReleaseScope(){if(!finished)finish(false);}
};
inline void retireReusedAction(void* action) noexcept {
    Snapshot old{};if(action&&mailbox.read(old)&&old.action==action)retire(old.serial);
}
inline void bindConstructed(void* action,void* ownerInterface) noexcept {
    // Also guard synthetic/alternate callers of the completed-construction hook.
    retireReusedAction(action);
    auto* s=releaseDraft;
    if(!s||!action||s->ownerInterface!=ownerInterface||!s->launcher||!live(*s))return;
    if(s->action){retire(s->serial);return;} // ambiguous second native firing action is never guessed
    s->action=action;if(!mailbox.publish(*s)){s->action=nullptr;retire();}
}
inline Refusal enter(void* action,std::uint64_t now,Snapshot& out) noexcept {
    if(!ranged_options_d65c::bowPhysicalAim.load())return Refusal::Disabled;
    Snapshot s{};if(!mailbox.read(s)||!s.serial)return Refusal::NoRelease;
    if(!live(s))return Refusal::NoRelease;
    if(s.action!=action)return Refusal::WrongAction;
    if(!s.tick||now<s.tick||now-s.tick>kMaxReleaseAgeMs){retire(s.serial);return Refusal::Stale;}
    const auto validate=current.load(std::memory_order_acquire);
    if(!validate||!validate(s)){retire(s.serial);return Refusal::ContextLost;}
    out=s;return Refusal::None;
}
struct ShotScope {
    const Snapshot* previous=nullptr;
    explicit ShotScope(const Snapshot* s) noexcept:previous(shotScope){shotScope=s;}
    ~ShotScope(){shotScope=previous;}
};
inline Refusal take(void* actor,void* launcher,Snapshot& out) noexcept {
    const auto* s=shotScope;if(!s||!live(*s))return Refusal::NoRelease;
    if(s->player!=actor)return Refusal::ActorMismatch;
    if(s->launcher!=launcher)return Refusal::LauncherMismatch;
    if(!unit3(s->dir)||!finite3(s->up)||!finite3(s->nock))return Refusal::Invalid;
    auto seen=consumed.load(std::memory_order_acquire);
    for(;;){if(seen>=s->serial)return Refusal::Consumed;if(consumed.compare_exchange_weak(seen,s->serial,std::memory_order_acq_rel))break;}
    out=*s;return Refusal::None;
}
inline const char* name(Refusal r) noexcept {
    switch(r){case Refusal::None:return "NONE";case Refusal::Disabled:return "DISABLED";case Refusal::NoRelease:return "NO_MATCHED_RELEASE";
    case Refusal::WrongAction:return "DIFFERENT_NATIVE_ACTION";case Refusal::ActorMismatch:return "ACTOR_MISMATCH";case Refusal::LauncherMismatch:return "LAUNCHER_MISMATCH";
    case Refusal::Stale:return "RELEASE_EXPIRED";case Refusal::ContextLost:return "OWNER_CONTEXT_CHANGED";case Refusal::Consumed:return "ALREADY_CONSUMED";default:return "INVALID_VECTOR";}
}
}
