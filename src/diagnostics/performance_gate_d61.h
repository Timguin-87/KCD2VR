#pragma once
// Shift+F2 owns ONLY this finite metrics lease; never a capture168 recorder bit.
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <cstdlib>
namespace kcd2vr::diagnostics::perf_d61 {
inline std::uint64_t nowNs() noexcept {
#ifdef KCD2VR_PERF_D61_TEST_CLOCK
    return KCD2VR_PERF_D61_TEST_CLOCK();
#else
    return std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
#endif
}
inline std::uint64_t nowMs() noexcept {return nowNs()/1000000ull;}
inline constexpr std::uint64_t DurationMs=180000,ReportMs=2000;
struct Gate {
    std::atomic<bool> requested{false};
    std::atomic<std::uint64_t> serial{0},until{0},closed{0};
    bool active(std::uint64_t now) const noexcept {
        if(!requested.load(std::memory_order_acquire))return false;
        const auto end=until.load(std::memory_order_acquire);
        return now&&end&&now<end&&end-now<=DurationMs;
    }
    bool current(std::uint64_t id,std::uint64_t now) const noexcept {
        return id&&id==serial.load(std::memory_order_acquire)&&active(now);
    }
    bool select(bool on,std::uint64_t now) noexcept {
        requested.store(false,std::memory_order_release);
        if(on&&now&&now<=UINT64_MAX-DurationMs){
            auto id=serial.load(std::memory_order_relaxed)+1;if(!id)id=1;
            serial.store(id,std::memory_order_release);closed.store(0,std::memory_order_release);
            until.store(now+DurationMs,std::memory_order_release);requested.store(true,std::memory_order_release);return true;
        }
        until.store(0,std::memory_order_release);closed.store(now,std::memory_order_release);return false;
    }
    bool poll(std::uint64_t now) noexcept {
        if(!requested.load(std::memory_order_acquire)||active(now))return false;
        select(false,now);return true;
    }
    bool receipt(std::uint64_t id,std::uint64_t now) const noexcept {
        if(!id||id!=serial.load(std::memory_order_acquire))return false;
        if(active(now))return true;const auto at=closed.load(std::memory_order_acquire);
        return at&&now>=at&&now-at<=5000;
    }
};
inline Gate gate;
inline bool requested() noexcept {return gate.requested.load(std::memory_order_relaxed);}
inline bool family(const char* s) noexcept {
    return s&&(std::strncmp(s,"D61_PERF CONTROL ",17)==0||std::strncmp(s,"D61_PERF SUMMARY ",17)==0||std::strncmp(s,"D61_PERF STAGES ",16)==0);
}
inline bool accept(const char* s,std::uint64_t now) noexcept {
    if(!family(s))return false;
    const char* p=std::strstr(s,"session=");if(!p)return false;p+=8;
    if(*p<'0'||*p>'9')return false;char* end=nullptr;const auto id=std::strtoull(p,&end,10);
    if(!end||(*end&&*end!=' '))return false;
    return std::strncmp(s,"D61_PERF CONTROL ",17)==0?gate.receipt(id,now):gate.current(id,now);
}
}
