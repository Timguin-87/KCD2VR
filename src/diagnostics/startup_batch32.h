#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
namespace kcd2vr::diagnostics {
// Single startup-worker owner. No allocation/file IO, no repeated plugin scan.
// Disabled/failed logger admission must not consume the pending batch.
template<std::size_t Capacity=32,std::size_t Size=512> struct StartupBatch32 {
    static_assert(Capacity>0&&Size>1);
    std::array<std::array<char,Size>,Capacity> records{};
    std::size_t count=0,dropped=0;bool flushed=false;
    bool Add(const char* message) noexcept {
        if(flushed||!message||!*message)return false;
        if(count==Capacity){++dropped;return false;}
        auto& line=records[count++];std::size_t n=0;
        while(n+1<Size&&message[n]){line[n]=message[n];++n;}line[n]=0;return true;
    }
    template<class Sink> bool Flush(bool loggerReady,Sink&& sink) noexcept {
        if(!loggerReady||flushed)return false;
        for(std::size_t i=0;i<count;++i)sink(records[i].data());
        char summary[160]{};std::snprintf(summary,sizeof(summary),
            "C32 PROXY_STARTUP BUFFER emitted=%llu omitted=%llu bounded=1 scanRepeated=0",
            static_cast<unsigned long long>(count),static_cast<unsigned long long>(dropped));
        sink(summary);flushed=true;return true;
    }
};
// At most three worker-owned logger-admission retries,500ms apart. Inactive
// after completion/exhaustion; never scans plugins or touches native game state.
struct StartupRetry32 {
    unsigned attempts=0;std::uint64_t due=0;bool done=false;
    template<class Ready,class Enable,class Flush> void Tick(std::uint64_t now,bool pending,
        Ready&& ready,Enable&& enable,Flush&& flush) noexcept {
        if(done)return;
        if(!pending){done=true;return;}
        if(!now||now<due)return;
        if(ready()){flush();done=true;return;}
        if(attempts==3){done=true;return;}
        ++attempts;due=now+500;enable();
        if(ready()){flush();done=true;}
    }
};
}
