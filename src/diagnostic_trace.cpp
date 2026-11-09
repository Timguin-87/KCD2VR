#include "diagnostics/capture168.h"
#include "diagnostic_trace.h"
#include "diagnostics/performance_gate_d61.h"
#include "diagnostics/map_performance_d72.h"
#include "diagnostics/motion_trace_d68.h"
#include "core/diagnostic_queue.h"
#include "vrik/visible_failure.h"
#include <DetourModKit/logger.hpp>
#include <windows.h>
#include <atomic>
#include <algorithm>
#include <cstdio>
#include <cwchar>
#include <string>
#include <array>
#include <cstring>
namespace kcd2vr::diagnostic_trace {
namespace {
// Worker-only byte batching. Record content/order/timestamps are unchanged.
// A failed batch counts no records as forwarded. Logger admission is not disk durability.
template<class Sink,std::size_t Capacity=65536> class FileBatchH47 {
    Sink& sink_;std::array<char,Capacity> bytes_;std::size_t used_=0,records_=0;
    std::uint64_t& written_;
public:
    FileBatchH47(Sink& sink,std::uint64_t& written) noexcept:sink_(sink),written_(written){}
    bool Flush() noexcept {
        if(!used_)return true;
        const bool ok=sink_(bytes_.data(),used_);if(ok)written_+=records_;
        used_=records_=0;return ok;
    }
    bool Append(const char* text,std::size_t size) noexcept {
        if(!size)return true;
        bool prior=true;if(size>Capacity){prior=Flush();const bool ok=sink_(text,size);if(ok)++written_;return prior&&ok;}
        if(used_+size>Capacity)prior=Flush();
        std::memcpy(bytes_.data()+used_,text,size);used_+=size;++records_;return prior;
    }
};

std::atomic<bool> g_enabled{false};
std::atomic<unsigned long long> g_accepted_d30{0};
RecordQueue<> g_records;
std::uint64_t g_reported_drop=0;
std::uint64_t g_forwarded=0,g_sinkRejected=0,g_filtered=0,g_lastHealth=0;
unsigned g_lastMask=0;
std::array<std::uint64_t,diagnostics::capture168::DomainCount> g_lastSession{};
// This scope runs on the mod worker. Explicit captures and finite map receipts
// must not disappear behind ordinary WARNING/ERROR verbosity. Restore only our
// own threshold; never force synchronous flushes from a native callback.
class CaptureVerbosityD81 {
    DetourModKit::Logger& logger_=DetourModKit::log();
    DetourModKit::LogLevel saved_=logger_.get_log_level();bool changed_=false;
public:
    CaptureVerbosityD81(){if(saved_>DetourModKit::LogLevel::Info){
        logger_.set_log_level(DetourModKit::LogLevel::Info);changed_=true;}}
    ~CaptureVerbosityD81()noexcept{if(changed_&&logger_.get_log_level()==DetourModKit::LogLevel::Info){
        try{logger_.set_log_level(saved_);}catch(...){}}}
};
bool Output(const char* line,std::size_t n) noexcept {
    if(!line||!n)return true;
    // Existing bounded producer queue and worker batching are retained.
    try{
        CaptureVerbosityD81 capture;
        if(!DetourModKit::log().is_enabled(DetourModKit::LogLevel::Info)){++g_filtered;return false;}
        const bool ok=DetourModKit::log().log_noexcept(DetourModKit::LogLevel::Info,std::string_view(line,n));
        if(!ok)++g_sinkRejected;return ok;
    }catch(...){++g_sinkRejected;return false;}
}
// Worker-only receipt covers the producer queue and shared logger separately.
// Forwarding is queue admission; sharedLoggerDropped also includes sink losses.
void CaptureHealth(bool ending) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    const unsigned mask=diagnostics::capture168::gate.mask.load(std::memory_order_acquire);
    bool changed=mask!=g_lastMask;std::array<std::uint64_t,diagnostics::capture168::DomainCount> sessions{};
    for(unsigned i=0;i<sessions.size();++i){sessions[i]=diagnostics::capture168::gate.session[i].load(std::memory_order_acquire);changed=changed||sessions[i]!=g_lastSession[i];}
    // No logger-end or periodic record without an explicitly opened capture.
    if(!mask&&!g_lastMask&&!changed)return;
    if(!ending&&!changed&&(!mask||(now>=g_lastHealth&&now-g_lastHealth<5000)))return;
    char line[896]{};
    const int n=std::snprintf(line,sizeof(line),"C54R3 CAPTURE_HEALTH tick=%llu state=%s activeMask=%u sessions{F2:%llu F3:%llu F5:%llu F7:%llu HUD:%llu F11:%llu F10:%llu HAND_REFERENCE:%llu F9:%llu SF5:%llu} queue{queued:%llu dropped:%llu truncated:%llu forwarded:%llu sinkRejectedBatches:%llu filteredBatches:%llu sharedLoggerDropped:%llu} scope=SHARED_PROCESS_TOTALS snapshot=NONATOMIC writes=KCD2VR_LOG forwarded=LOGGER_ADMISSION_NOT_DURABILITY completeness=CHECK_QUEUE_AND_LOGGER_DELTA\r\n",
        (unsigned long long)now,ending?"LOGGER_END":changed?"CAPTURE_CHANGE":"PERIODIC",mask,
        (unsigned long long)sessions[unsigned(diagnostics::capture168::Domain::Bug)],
        (unsigned long long)sessions[unsigned(diagnostics::capture168::Domain::Combat)],
        (unsigned long long)sessions[unsigned(diagnostics::capture168::Domain::Interaction)],
        (unsigned long long)sessions[unsigned(diagnostics::capture168::Domain::Vrik)],
        (unsigned long long)sessions[unsigned(diagnostics::capture168::Domain::Hud)],
        (unsigned long long)sessions[unsigned(diagnostics::capture168::Domain::Reload)],
        (unsigned long long)sessions[unsigned(diagnostics::capture168::Domain::Hands)],
        (unsigned long long)sessions[unsigned(diagnostics::capture168::Domain::HandReference)],
        (unsigned long long)sessions[unsigned(diagnostics::capture168::Domain::Physics)],
        (unsigned long long)sessions[unsigned(diagnostics::capture168::Domain::Quest)], // R158 Shift+F5 quest recorder
        (unsigned long long)g_records.Queued(),(unsigned long long)g_records.Dropped(),(unsigned long long)g_records.Truncated(),
        (unsigned long long)g_forwarded,(unsigned long long)g_sinkRejected,(unsigned long long)g_filtered,(unsigned long long)DetourModKit::log().dropped_count());
    if(n>0)Output(line,std::min<std::size_t>(std::size_t(n),sizeof(line)-1));
    g_lastHealth=now;g_lastMask=mask;g_lastSession=sessions;
}
}
void SetEnabled(bool enabled) noexcept {
    if(enabled==g_enabled.load(std::memory_order_acquire))return;
    g_enabled.store(enabled,std::memory_order_release);
    if(!enabled)Drain(true); // recorder state never opens, truncates or closes a file
}
bool Enabled() noexcept {return g_enabled.load(std::memory_order_acquire);}
bool EnabledFor(diagnostics::capture168::Domain d) noexcept {
    if(!Enabled())return false;
    using namespace diagnostics::capture168;
    if(!(gate.mask.load(std::memory_order_relaxed)&members(d)))return false;
    return active(d,GetTickCount64());
}
namespace {
void Enqueue(const char* message) noexcept {
    FILETIME ft{};GetSystemTimeAsFileTime(&ft);
    if(g_records.Push(message,(std::uint64_t(ft.dwHighDateTime)<<32)|ft.dwLowDateTime))
        g_accepted_d30.fetch_add(1,std::memory_order_relaxed);
}
}
void WriteFor(diagnostics::capture168::Domain d,const char* message) noexcept {
    if(message&&EnabledFor(d))Enqueue(message);
}
void WriteReceiptFor(diagnostics::capture168::Domain d,std::uint64_t session,const char* message) noexcept {
    if(Enabled()&&message&&diagnostics::capture168::gate.receipt(d,session,GetTickCount64()))Enqueue(message);
}
void Write(const char* message) noexcept {
    if(!Enabled()||!message)return;
    using namespace diagnostics::capture168;
    const auto now=std::uint64_t(GetTickCount64());
    // R146: no recorder and no 5 s end-receipt grace = accept() refuses everything;
    // skip its prefix/strstr scan of the text (the common public-player case).
    if(gate.quiet(now))return;
    // End receipts are explicitly session-bound; FAULT/mode/startup text has no
    // exemption. Previously the essential() whitelist bypassed every F key.
    if(!gate.mask.load(std::memory_order_relaxed)&&receiptDomain(message)<0)return;
    if(!accept(message,now))return;
    Enqueue(message);
}
void WriteStatus(const char* message) noexcept {
    if(!Enabled()||!message)return;
    // R154: a status row is rare and the only trace of a decision in KCD2VR.log; it waits out a held queue flag
    // (bounded spin, see RecordQueue::PushSpin) instead of counting as a one-shot drop like per-frame producers.
    FILETIME ft{};GetSystemTimeAsFileTime(&ft);
    if(g_records.PushSpin(message,(std::uint64_t(ft.dwHighDateTime)<<32)|ft.dwLowDateTime))
        g_accepted_d30.fetch_add(1,std::memory_order_relaxed);
}
void WritePerformanceD61(const char* message) noexcept {
    if(!Enabled()||!diagnostics::perf_d61::family(message))return;
    if(diagnostics::perf_d61::accept(message,diagnostics::perf_d61::nowMs()))Enqueue(message);
}
void WritePerformanceD68(const char* message) noexcept {
    if(!Enabled()||!message)return;
    if(diagnostics::motion_d68::accept(message,diagnostics::perf_d61::nowMs()))Enqueue(message);
}
void WriteMapPerformanceD72(const char* message) noexcept {
    if(!Enabled()||!diagnostics::map_d72::accept(message,diagnostics::perf_d61::nowMs()))return;
    // D84: the same sink, but only while an explicit F2/Shift+F2 lease is active.
    (void)Output(message,std::strlen(message));
}
unsigned long long Accepted() noexcept {return g_accepted_d30.load(std::memory_order_relaxed);}
void Drain(bool all) noexcept {
    // Status is independent of recorder state and never opens another file.
    // Repeated identical faults are coalesced on this worker, not in a hook.
    static std::uint32_t statusSerial=0;static std::uint64_t statusAt=0;
    static char statusText[visible_failure::kMaxText+1]{};
    visible_failure::Message status{};
    if(visible_failure::logMailboxD80.read(status)&&status.serial&&status.serial!=statusSerial){
        statusSerial=status.serial;
        if(std::strcmp(statusText,status.text)||!statusAt||status.tick<statusAt||status.tick-statusAt>=30000){
            char line[160]{};std::snprintf(line,sizeof(line),"VR status: %s",status.text);
            (void)DetourModKit::log().log_noexcept(DetourModKit::LogLevel::Info,line);
            std::memcpy(statusText,status.text,sizeof(statusText));statusAt=status.tick;
        }
    }
    RecordQueue<>::Record r{};char line[2304]{};std::size_t remaining=all?4096:128;
    auto sinkH47=[](const char* bytes,std::size_t size) noexcept{return Output(bytes,size);};
    FileBatchH47<decltype(sinkH47)> batchH47(sinkH47,g_forwarded);
    while(remaining--&&g_records.Pop(r)){
        FILETIME utc{static_cast<DWORD>(r.filetime),static_cast<DWORD>(r.filetime>>32)},local{};SYSTEMTIME st{};
        FileTimeToLocalFileTime(&utc,&local);FileTimeToSystemTime(&local,&st);
        const int n=std::snprintf(line,sizeof(line),"[%04u-%02u-%02u %02u:%02u:%02u.%03u] %s\r\n",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds,r.message);
        if(n>0)batchH47.Append(line,std::min<std::size_t>(std::size_t(n),sizeof(line)-1));
    }
    batchH47.Flush(); // preserve record order before health/drop receipts
    const auto dropped=g_records.Dropped();if(dropped!=g_reported_drop){g_reported_drop=dropped;const int n=std::snprintf(line,sizeof(line),"V66.89-DIAGNOSTICS droppedTotal=%llu queue=BOUNDED nonblockingHotProducer=1\r\n",(unsigned long long)dropped);if(n>0)Output(line,std::size_t(n));}
    CaptureHealth(all);
}
}
