// R59C4: compatibility helper only. Native call sites use fault latches again.
#pragma once
// V66.178R59.2 FAULT PAUSE - the end of the process-lifetime kill switches.
//
// Until R59.1 a single fault (an SEH exception in a native leaf, an impossible native result, a slow query) set a
// std::atomic<bool> for the rest of the process: the subsystem went dark, nothing said which one, and neither F4
// nor a save load brought it back ("rearm=PROCESS_RESTART_ONLY"). Those guards were right while the native
// contracts were unproven (V66.92-98); they are wrong now that every one of them has thousands of clean calls per
// session. A FaultPause keeps the exact same call sites (it quacks like std::atomic<bool>: load / store / exchange /
// assignment) but a trip is a PAUSE: 2 s, doubling per repeat up to 60 s, the series forgotten after 120 s of quiet,
// every begin / end written to the diagnostics log with the subsystem's name, and the only thing that stays off for
// good is a retail-image mismatch at startup (retail_build112), which is not a fault but a wrong game version.
//
// Pure header: the clock is std::chrono::steady_clock; the log sink is a function pointer the mod sets at startup
// (kcd2vr::fault59::sink = &diagnostic_trace::Write); host fixtures set their own or leave it null.
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <mutex>

namespace kcd2vr::fault59 {
inline void (*sink)(const char*) noexcept = nullptr;
inline void log(const char* m) noexcept { if(sink)sink(m); }
inline std::uint64_t now_ms() noexcept {
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}
inline constexpr std::uint64_t kFirstPauseMs=2000,kMaxPauseMs=60000,kForgetMs=120000;
inline std::uint64_t pause_for(unsigned strike) noexcept {
    std::uint64_t p=kFirstPauseMs;for(unsigned i=1;i<strike&&p<kMaxPauseMs;++i)p*=2;return p<kMaxPauseMs?p:kMaxPauseMs;
}
class FaultPause {
    const char* name_;
    std::recursive_mutex mutex_;
    std::atomic<std::uint64_t> until_{0},last_{0};
    std::atomic<unsigned> strikes_{0},total_{0};
    std::atomic<bool> announcedEnd_{true};
public:
    explicit FaultPause(const char* name) noexcept : name_(name) {}
    FaultPause(const FaultPause&)=delete;FaultPause& operator=(const FaultPause&)=delete;
    const char* name() const noexcept {return name_;}
    unsigned total() const noexcept {return total_.load(std::memory_order_relaxed);}
    unsigned strikes() const noexcept {return strikes_.load(std::memory_order_relaxed);}
    std::uint64_t until() const noexcept {return until_.load(std::memory_order_acquire);}
    // Paused right now? Expiry is lazy: the first query after the deadline re-arms and logs the end.
    bool active(std::uint64_t now) noexcept {std::lock_guard<std::recursive_mutex> lock(mutex_);
        auto u=until_.load(std::memory_order_acquire);
        if(!u)return false;
        if(now<u)return true;
        if(until_.compare_exchange_strong(u,0,std::memory_order_acq_rel)&&!announcedEnd_.exchange(true)){
            char m[160]{};std::snprintf(m,sizeof(m),"V66.178R59 FAULT_PAUSE end subsystem=%s rearm=AUTOMATIC",name_);log(m);}
        return false;
    }
    // A fault happened: pause with back-off and say so. Returns the pause length.
    std::uint64_t trip(std::uint64_t now,const char* why=nullptr) noexcept {std::lock_guard<std::recursive_mutex> lock(mutex_);
        const auto last=last_.load(std::memory_order_relaxed);
        const unsigned strike=(last&&now>=last&&now-last<=kForgetMs)?strikes_.load(std::memory_order_relaxed)+1:1;
        strikes_.store(strike,std::memory_order_relaxed);last_.store(now,std::memory_order_relaxed);total_.fetch_add(1,std::memory_order_relaxed);
        const auto pause=pause_for(strike);until_.store(now+pause,std::memory_order_release);announcedEnd_.store(false,std::memory_order_release);
        char m[320]{};std::snprintf(m,sizeof(m),"V66.178R59 FAULT_PAUSE begin subsystem=%s pauseMs=%llu strike=%u total=%u rearm=AUTOMATIC%s%s",
            name_,(unsigned long long)pause,strike,total_.load(std::memory_order_relaxed),why?" why=":"",why?why:"");log(m);
        return pause;
    }
    void clear() noexcept {std::lock_guard<std::recursive_mutex> lock(mutex_);until_.store(0,std::memory_order_release);announcedEnd_.store(true,std::memory_order_release);}
    // std::atomic<bool> look-alikes so the existing call sites compile unchanged.
    bool load(std::memory_order=std::memory_order_seq_cst) noexcept {return active(now_ms());}
    void store(bool v,std::memory_order=std::memory_order_seq_cst) noexcept {if(v)(void)trip(now_ms());else clear();}
    bool exchange(bool v,std::memory_order=std::memory_order_seq_cst) noexcept {std::lock_guard<std::recursive_mutex> lock(mutex_);const bool was=active(now_ms());if(v){if(!was)(void)trip(now_ms());}else clear();return was;}
    FaultPause& operator=(bool v) noexcept {store(v);return *this;}
    explicit operator bool() noexcept {return load();}
};
}
