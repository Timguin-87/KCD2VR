#pragma once
#include "contact_authority176.h"
#include <array>
#include <atomic>
#include <cstddef>

namespace kcd2vr::hybrid_serial59 {
// HYBRID contacts can wait for native cost at the tail of the contact queue.
// Ticket B entering before A is not evidence that A entered native code. Keep
// exact release tombstones; the legacy synchronous-sweep scalar stays intact.
enum class Result { Ready, Invalid, Expired, Duplicate, Alias, Busy, Full, Revoked };
inline const char* name(Result r) noexcept {
    switch(r){case Result::Ready:return "READY";case Result::Invalid:return "INVALID_RELEASE";
    case Result::Expired:return "EXPIRED_RELEASE";case Result::Duplicate:return "RELEASE_ALREADY_RESERVED_OR_ENTERED";
    case Result::Alias:return "RELEASE_SERIAL_IDENTITY_MISMATCH";case Result::Busy:return "RELEASE_LEDGER_BUSY";
    case Result::Full:return "RELEASE_LEDGER_FULL";case Result::Revoked:return "RELEASE_REVOKED_BEFORE_NATIVE_ENTRY";}return "UNKNOWN";
}
inline bool equal(const contact176::Ticket& a,const contact176::Ticket& b) noexcept {
    return a.player==b.player&&a.entity==b.entity&&a.serial==b.serial&&a.tick==b.tick&&a.sample==b.sample&&
        a.epoch==b.epoch&&a.request==b.request&&a.key==b.key&&a.generation==b.generation&&a.policy==b.policy&&
        a.gesture59c3==b.gesture59c3&&a.recovery59c3==b.recovery59c3;
}
inline bool valid(const contact176::Ticket& t) noexcept {
    return t.player&&t.entity&&t.serial&&t.tick&&t.sample&&t.epoch&&t.request&&t.key&&t.generation&&t.policy;
}
struct Token {std::size_t slot=0;std::uint64_t version=0,serial=0;};
template<std::size_t Capacity=64> class Ledger {
    // State low bits: 1 reserved (never evicted), 2 entered, 3 revoked tombstone.
    // Version remains unique across clear/reuse, so an old RAII lease cannot
    // erase a newer reservation, even during reentrant native work.
    struct Entry {contact176::Ticket ticket{};std::atomic<std::uint64_t> state{0};};
    std::array<Entry,Capacity> entries_{};
    std::atomic_flag busy_=ATOMIC_FLAG_INIT;
    std::uint64_t next_=0;std::atomic<std::uint64_t> revocation_{1};
public:
    Result reserve(const contact176::Ticket& t,std::uint64_t now,Token& out) noexcept {
        const auto revocation=revocation_.load(std::memory_order_acquire);
        out={};if(!valid(t))return Result::Invalid;
        if(!contact176::fresh(t.tick,now,contact176::kReleaseSafetyCapMs))return Result::Expired;
        if(busy_.test_and_set(std::memory_order_acquire))return Result::Busy;
        struct Unlock {std::atomic_flag& flag;~Unlock(){flag.clear(std::memory_order_release);}} unlock{busy_};
        std::size_t free=Capacity;
        for(std::size_t i=0;i<Capacity;++i){auto& e=entries_[i];const auto s=e.state.load(std::memory_order_acquire);
            if(!s){if(free==Capacity)free=i;continue;}
            // Immutable full ticket, not the current global policy (a newer
            // ordinary preparation can legitimately have advanced that).
            if(e.ticket.serial==t.serial)return equal(e.ticket,t)?Result::Duplicate:Result::Alias;
            if((s&3)>=2&&now>=e.ticket.tick&&now-e.ticket.tick>contact176::kReleaseSafetyCapMs&&free==Capacity)free=i;
        }
        if(free==Capacity||next_>=((~std::uint64_t(0))>>2))return Result::Full;
        auto& e=entries_[free];const auto version=++next_;e.ticket=t;
        e.state.store((version<<2)|1,std::memory_order_release);out={free,version,t.serial};
        if(revocation_.load(std::memory_order_acquire)!=revocation){rollback(out,false);out={};return Result::Busy;}
        return Result::Ready;
    }
    bool tryEnter(const Token& t) noexcept {
        if(!t.version||t.slot>=Capacity)return false;
        auto expected=(t.version<<2)|1;
        return entries_[t.slot].state.compare_exchange_strong(expected,(t.version<<2)|2,std::memory_order_acq_rel);
    }
    void rollback(const Token& t,bool mayRelease) noexcept {
        if(!t.version||t.slot>=Capacity)return;
        auto expected=(t.version<<2)|1;
        (void)entries_[t.slot].state.compare_exchange_strong(expected,mayRelease?0:((t.version<<2)|3),std::memory_order_acq_rel);
    }
    void clear() noexcept {
        // Revocation cannot erase fresh native-entry evidence. This is lock-free
        // and cannot invalidate a new slot that a concurrent reserve replaced.
        revocation_.fetch_add(1,std::memory_order_acq_rel);
        for(auto& e:entries_){auto s=e.state.load(std::memory_order_acquire);
            if((s&3)==1)(void)e.state.compare_exchange_strong(s,(s&~std::uint64_t(3))|3,std::memory_order_acq_rel);}
    }
};
inline Ledger<> releases;
class Reservation {
    Ledger<>* ledger_=nullptr;Token token_{};std::uint64_t policy_=0,recovery_=0;
    Result result_=Result::Invalid;
public:
    Reservation() noexcept=default;Reservation(const Reservation&)=delete;Reservation& operator=(const Reservation&)=delete;
    bool reserve(const contact176::Ticket& ticket,std::uint64_t now,Ledger<>& ledger=releases) noexcept {
        if(ledger_)return false;
        policy_=contact176::policy.load(std::memory_order_acquire);recovery_=contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
        result_=ledger.reserve(ticket,now,token_);if(result_!=Result::Ready)return false;ledger_=&ledger;return true;
    }
    Result result() const noexcept{return result_;}
    bool tryEnter() noexcept {
        if(!ledger_||!ledger_->tryEnter(token_)){result_=Result::Revoked;return false;}
        // Keep the old scalar as a conservative legacy-sweep high-water mark;
        // it is no longer the HYBRID contact's order-dependent eligibility gate.
        auto used=contact176::claimedSerial.load(std::memory_order_acquire);
        while(used<token_.serial&&!contact176::claimedSerial.compare_exchange_weak(used,token_.serial,std::memory_order_acq_rel)){}
        ledger_=nullptr;token_={};return true;
    }
    ~Reservation(){if(ledger_)ledger_->rollback(token_,contact176::policy.load(std::memory_order_acquire)==policy_&&
        contact176::recoveryEpoch55c1.load(std::memory_order_acquire)==recovery_);}
};
// The live comparison route retains its legacy scalar behavior. Only the
// synchronized HYBRID's full-release paths use the exact ledger above.
class RoutedReservation {
    contact176::SerialReservationC2 legacy_;Reservation hybrid_;bool hybridRoute_=false,attempted_=false,reserved_=false;
public:
    bool reserve(const contact176::Ticket& ticket,std::uint64_t now,bool hybrid) noexcept {
        if(attempted_)return false;attempted_=true;hybridRoute_=hybrid;
        reserved_=hybrid?hybrid_.reserve(ticket,now):legacy_.reserve(ticket.serial);return reserved_;
    }
    const char* reason() const noexcept{return hybridRoute_?name(hybrid_.result()):"LEGACY_RELEASE_ALREADY_CLAIMED";}
    bool tryEnter() noexcept{
        if(!attempted_)return true; // intentional verified physical attempt without an admitted release
        if(!reserved_)return false;reserved_=false;
        if(hybridRoute_)return hybrid_.tryEnter();legacy_.nativeCallStarted();return true;
    }
};
}
