#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>

namespace kcd2vr::motion59c4 {
// Explicit development backend. The native reference remains usable until the
// complete action-independent native services contract is qualified. No INI
// switch can turn missing contracts into permission to apply damage.
inline std::atomic<bool> previewRequested{false},selected{false};
inline std::atomic<std::uint64_t> nativeStarts{0};
// Match the VR state driver strike lifetime; contact freshness remains 160 ms.
inline constexpr std::uint64_t strikeLifetimeMs5910c1=600;
// C10R18 (Miki: "the swings should never cancel"): a contact that arrives within this window after the RECOGNIZER gave the
// stroke up (withdrawn / short / incoherent) still lands once - the blade was at the enemy. A native takeover's cancel never
// revives (it is not marked). 0 = off (VrDirectOwnsHits).
inline std::atomic<unsigned> lateContactMs18{200};
enum class Event { Prepare,Commit,Feint,Cancel };
enum class Stage { Empty,Prepared,Committed,Consumed,Cancelled,Miss };
enum class AttackOwnership { None,Owned,Unknown };
enum class EffortClaim { Ready,Duplicate,Pending,Failed,Invalid,Expired };
enum class Claim { Wait,Ready,Duplicate,Identity,Expired,Cancelled,InvalidContact };
struct Key {
    std::uint64_t gesture=0,weapon=0,generation=0,body=0,request=0,recovery=0;
    std::uintptr_t player=0;
};
inline bool valid(const Key& k) noexcept {
    return k.gesture&&k.weapon&&k.generation&&k.body&&k.request&&k.player;
}
inline bool same(const Key& a,const Key& b) noexcept {
    return a.gesture==b.gesture&&a.weapon==b.weapon&&a.generation==b.generation&&
        a.body==b.body&&a.request==b.request&&a.recovery==b.recovery&&a.player==b.player;
}
inline bool fresh(std::uint64_t t,std::uint64_t now,std::uint64_t cap) noexcept {
    return t&&now>=t&&now-t<=cap;
}
struct Attempt { Key key{}; Stage stage=Stage::Empty;
    std::uint64_t begin=0,commit=0; int origin=-1,technique=0; std::uint32_t target=0; bool effortAttempted=false;
    bool effortFinished=false,effortSucceeded=false;
    bool cancelWhileReserved=false; // R59.16C7A: a Cancel seen while the claim was reserved wins over release()
    std::uint64_t cancelledAt=0; bool recognizerCancel18=false,late18=false; // C10R18
    std::uint64_t terminalTick54=0; // native application/physical reservation time; never a contact lease
};
// Owns event timing and deduplication, never a native pointer/action lease.
// Eight bounded entries retain consumed/cancelled identities against replay.
// All calls are short; no native calls or diagnostic I/O while holding mutex.
class Attempts {
    std::array<Attempt,8> entries_{};
    std::mutex mutex_;
    std::uint64_t newest_=0,terminalRetentionMs_=300;
public:
    explicit Attempts(std::uint64_t terminalRetentionMs=300) noexcept:terminalRetentionMs_(terminalRetentionMs){}
    void clear() noexcept {std::lock_guard<std::mutex> l(mutex_);entries_={};newest_=0;}
    // Read-only provenance for a configured contact prediction. A commit or
    // early contact never substitutes its timestamp for the forward onset.
    // This grants no publication, claim, damage or lifetime extension.
    std::uint64_t beginForKey(const Key& key,std::uint64_t now) noexcept {
        if(!valid(key))return 0;
        std::lock_guard<std::mutex> l(mutex_);
        for(const auto& a:entries_)if(same(a.key,key)){
            if(a.cancelWhileReserved||!fresh(a.begin,now,3000))return 0;
            if(a.stage==Stage::Prepared)return a.begin;
            if((a.stage==Stage::Committed||a.stage==Stage::Consumed)&&fresh(a.commit,now,strikeLifetimeMs5910c1))return a.begin;
            return 0;
        }
        return 0;
    }
    bool event(const Key& key,Event e,std::uint64_t tick,std::uint64_t now,int origin,int technique,bool recognizerCancel=false) noexcept {
        if(!valid(key)||!fresh(tick,now,100)||technique<0||technique>1)return false;
        if(e!=Event::Cancel&&origin!=0&&origin!=1&&origin!=2&&origin!=5)return false;
        std::lock_guard<std::mutex> l(mutex_);Attempt* a=nullptr;Attempt* available=nullptr;
        for(auto& x:entries_){
            if(same(x.key,key)){a=&x;break;}
            // C10R18: a cancelled entry stays until its late-contact window (cancelledAt + 200 ms) is safely over.
            const auto settled=x.stage==Stage::Consumed&&x.terminalTick54?x.terminalTick54:
                x.stage==Stage::Cancelled&&x.cancelledAt?x.cancelledAt:(x.commit?x.commit:x.begin);
            if(x.stage==Stage::Empty||(!fresh(x.begin,now,3000)&&
                x.key.gesture<key.gesture&&!(x.stage==Stage::Consumed&&fresh(x.terminalTick54,now,terminalRetentionMs_)))||((x.stage==Stage::Consumed||x.stage==Stage::Cancelled||x.stage==Stage::Miss)&&
                !fresh(settled,now,terminalRetentionMs_)))available=&x;
        }
        if(!a){
            if(e==Event::Feint||e==Event::Cancel||!available||key.gesture<=newest_)return false;
            newest_=key.gesture;a=available;*a={key,Stage::Prepared,tick,0,origin,technique,0};
        }
        if(e==Event::Cancel){
            if(a->stage==Stage::Consumed){a->cancelWhileReserved=true;a->recognizerCancel18=false;return false;} // applied or reserved: not cancelled now
            if(a->stage==Stage::Cancelled){if(!recognizerCancel)a->recognizerCancel18=false;return false;}
            if(a->stage!=Stage::Prepared&&a->stage!=Stage::Committed)return false;
            a->stage=Stage::Cancelled;a->cancelledAt=tick;a->recognizerCancel18=recognizerCancel;return true;
        }
        if(a->stage!=Stage::Prepared)return false;
        a->origin=origin;a->technique=technique;
        if(e==Event::Commit){a->commit=tick;a->stage=Stage::Committed;}
        return true;
    }
    // The producer can enqueue Prepare and Commit before the game thread drains
    // either. A native takeover may cancel the ledger while handling Prepare;
    // queued successors must not recreate that cancelled native publication.
    bool publicationAllowed(const Key& key,Event event,std::uint64_t tick,std::uint64_t now) noexcept {
        if(!valid(key)||!fresh(tick,now,150)||event==Event::Cancel)return false;
        std::lock_guard<std::mutex> l(mutex_);
        for(const auto& a:entries_)if(same(a.key,key)){
            if(a.cancelWhileReserved||(a.stage!=Stage::Prepared&&a.stage!=Stage::Committed)||tick<a.begin)return false;
            if(event==Event::Commit)return a.stage==Stage::Committed&&a.commit==tick&&fresh(a.commit,now,strikeLifetimeMs5910c1);
            return fresh(a.begin,now,3000); // prepare/feint may precede an already-queued commit
        }
        return false;
    }
    // Cancellation provenance is recorded atomically in event(); native/context cancellations revoke late delivery.
    static bool late18(const Attempt& a,std::uint64_t impact) noexcept {
        const auto w=lateContactMs18.load(std::memory_order_relaxed);
        return w&&a.stage==Stage::Cancelled&&a.recognizerCancel18&&!a.cancelWhileReserved&&a.cancelledAt&&impact>=a.begin&&impact<=a.cancelledAt+w;
    }
    Claim claim(const Key& key,std::uint64_t impact,std::uint64_t now,std::uint32_t target,Attempt& out) noexcept {
        if(!valid(key))return Claim::Identity;
        if(!target)return Claim::InvalidContact;
        if(!fresh(impact,now,160))return Claim::Expired;
        std::lock_guard<std::mutex> l(mutex_);
        for(auto& a:entries_)if(same(a.key,key)){
            if(a.stage==Stage::Consumed)return Claim::Duplicate;
            if(late18(a,impact))return Claim::Wait; // C10R18: contact() finishes it once
            if(a.stage==Stage::Cancelled||a.stage==Stage::Miss)return Claim::Cancelled;
            if(impact<a.begin)return Claim::Identity;
            if(a.stage==Stage::Prepared)return Claim::Wait;
            // Early close contact may precede the recognizer's COMMIT, provided
            // it is the same still-fresh VR attempt. No native release involved.
            if(!fresh(a.commit,now,strikeLifetimeMs5910c1))return Claim::Expired;
            a.stage=Stage::Consumed;a.target=target;a.terminalTick54=now;out=a;return Claim::Ready;
        }
        return Claim::Wait;
    }
    // A real, checked early contact may finish an EXISTING preparation. It
    // never revives cancellation/expiry, invents an unknown attempt, changes
    // technique, or evicts replay tombstones. Callers supply current collision
    // evidence; this ledger owns only attempt identity and once-only delivery.
    Claim contact(const Key& key,std::uint64_t impact,std::uint64_t now,std::uint32_t target,int technique,Attempt& out) noexcept {
        if(!valid(key)||technique<0||technique>1)return Claim::Identity;
        if(!target)return Claim::InvalidContact;
        if(!fresh(impact,now,160))return Claim::Expired;
        std::lock_guard<std::mutex> l(mutex_);
        for(auto& a:entries_)if(same(a.key,key)){
            if(a.stage==Stage::Consumed)return Claim::Duplicate;
            if(late18(a,impact)&&fresh(a.begin,now,3000)){ // C10R18: the recognizer gave up, the blade did not
                a.technique=technique;a.stage=Stage::Consumed;a.late18=true;a.target=target;a.commit=impact;a.terminalTick54=now;out=a;return Claim::Ready;}
            if(a.stage==Stage::Cancelled||a.stage==Stage::Miss)return Claim::Cancelled;
            if(impact<a.begin)return Claim::Identity;
            if(a.stage!=Stage::Prepared)return Claim::Identity;
            a.technique=technique; // R59.16C7A: the contact decides cut vs thrust, not the wind-up guess
            if(!fresh(a.begin,now,3000))return Claim::Expired;
            a.stage=Stage::Consumed;a.target=target;a.commit=impact;a.terminalTick54=now;out=a;return Claim::Ready;
        }
        return Claim::Identity;
    }
    // C54: an exact admitted native application is another terminal answer to
    // this physical gesture, not another damage owner. Only an existing entry
    // can be reserved; this never creates a contact or extends its160ms lease.
    Claim application54(const Key& key,std::uint64_t now,Attempt& out) noexcept {
        if(!valid(key))return Claim::Identity;
        std::lock_guard<std::mutex> l(mutex_);
        for(auto& a:entries_)if(same(a.key,key)){
            if(a.stage==Stage::Consumed)return Claim::Duplicate;
            if(a.stage==Stage::Cancelled||a.stage==Stage::Miss)return Claim::Cancelled;
            if(!fresh(a.begin,now,3000))return Claim::Expired;
            if(a.stage!=Stage::Prepared&&a.stage!=Stage::Committed)return Claim::Identity;
            a.stage=Stage::Consumed;a.terminalTick54=now;out=a;return Claim::Ready;
        }
        return Claim::Identity;
    }
    // R59.16C7A: a claim that a later check could not deliver is given back, so the
    // next real contact of the SAME swing can still land (still once per swing).
    void release(const Key& key) noexcept {
        std::lock_guard<std::mutex> l(mutex_);
        for(auto& a:entries_)if(same(a.key,key)&&a.stage==Stage::Consumed){
            a.target=0;a.stage=(a.cancelWhileReserved||a.late18)?Stage::Cancelled:(a.commit?Stage::Committed:Stage::Prepared);
            a.terminalTick54=0;a.cancelWhileReserved=false;return;}
    }
    EffortClaim claimEffort(const Key& key,std::uint64_t now) noexcept {
        if(!valid(key))return EffortClaim::Invalid;
        std::lock_guard<std::mutex> l(mutex_);
        for(auto& a:entries_)if(same(a.key,key)){
            if(a.effortAttempted)return !a.effortFinished?EffortClaim::Pending:
                a.effortSucceeded?EffortClaim::Duplicate:EffortClaim::Failed;
            if(a.stage!=Stage::Committed&&a.stage!=Stage::Consumed)return EffortClaim::Invalid;
            if(!fresh(a.commit,now,strikeLifetimeMs5910c1))return EffortClaim::Expired;
            a.effortAttempted=true;return EffortClaim::Ready;
        }
        return EffortClaim::Invalid;
    }
    // Reservation is not successful payment. Never retry an ambiguous native debit.
    bool finishEffort(const Key& key,bool success) noexcept {
        std::lock_guard<std::mutex> l(mutex_);
        for(auto& a:entries_)if(same(a.key,key)&&a.effortAttempted&&!a.effortFinished){
            a.effortFinished=true;a.effortSucceeded=success;return true;}
        return false;
    }
    unsigned expire(std::uint64_t now) noexcept {
        unsigned missed=0;std::lock_guard<std::mutex> l(mutex_);
        for(auto& a:entries_){
            if(a.stage==Stage::Committed&&!fresh(a.commit,now,strikeLifetimeMs5910c1)){a.stage=Stage::Miss;++missed;}
            else if(a.stage==Stage::Prepared&&!fresh(a.begin,now,3000))a.stage=Stage::Cancelled;
        }
        return missed;
    }
};
// C2: a Ready claim is a reservation until the irreversible native call starts.
// Every earlier return gives it back; once the call starts even an exception
// must retain deduplication because native side effects may already have run.
class ClaimReservationC2 {
    Attempts* owner_=nullptr;Key key_{};
public:
    ClaimReservationC2() noexcept = default;
    ClaimReservationC2(const ClaimReservationC2&)=delete;
    ClaimReservationC2& operator=(const ClaimReservationC2&)=delete;
    ~ClaimReservationC2(){if(owner_)owner_->release(key_);}
    void hold(Attempts& owner,const Key& key) noexcept {owner_=&owner;key_=key;}
    void nativeCallStarted() noexcept {owner_=nullptr;}
};
inline Attempts attempts;
// C54: HYBRID physical identity is independent of native input admission.
// This ledger never publishes native phases or changes eligibility. The same
// entry guards BOTH live-action and direct transport, including native refusal
// and exceptions; an entered native call is terminal for this physical gesture.
inline Attempts hybridAttempts54{1500}; // retain terminal identity across the native release safety cap
// Do not confuse the identified D568B0 result routine with a complete combat
// transaction. These remain hard prerequisites, not configurable overrides.
enum Missing : unsigned { Effort=1,Threat=2,Hostility=4,Defense=8,Completion=16 };
inline constexpr unsigned missingNativeContracts=Effort|Threat|Hostility|Defense|Completion;
}
