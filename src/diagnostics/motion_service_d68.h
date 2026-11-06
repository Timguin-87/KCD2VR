#pragma once
#include "motion_trace_d68.h"
namespace kcd2vr::diagnostics::motion_d68 {
// Mod worker only. This does not change any capture bit, experiment or pose.
template<class Writer> inline void service(Writer&& write,unsigned f2,bool h50,bool presentLead,bool hj1,int cameraSource) noexcept {
    // A nonempty queue is bounded; draining after stop emits only same-session
    // receipts in the existing five-second close grace, never renews the lease.
    if(!requested()&&copied.load(std::memory_order_relaxed)==drained.load(std::memory_order_relaxed))return;
    const auto now=perf_d61::nowMs();drain(now,write,64);
    static std::uint64_t session=0,last=0;
    const auto id=perf_d61::gate.serial.load(std::memory_order_acquire);
    if(!perf_d61::gate.current(id,now))return;
    if(session==id&&now>=last&&now-last<2000)return;
    session=id;last=now;
    char line[2048]{};
    int n=std::snprintf(line,sizeof(line),"D68_MOTION session=%llu stage=HEALTH ns=%llu burstMs=250 periodMs=2000 cadenceFloorMs=3 policyChange=0 fullSessionMetrics=1 meshSubmission=UNOBSERVED counters=PROCESS_TOTALS snapshots=NONATOMIC copied=%llu drained=%llu dropped=%llu expired=%llu formatDropped=%llu f2Policy=%u H50=%d presentLead=%d HJ1=%d cameraSource=%d stages=observed/outsideBurst/cadenceSkip/pushed/dropped",
        (unsigned long long)id,(unsigned long long)(now*1000000ull),
        (unsigned long long)copied.load(),(unsigned long long)drained.load(),(unsigned long long)dropped.load(),
        (unsigned long long)expired.load(),(unsigned long long)formatDropped.load(),f2,h50,presentLead,hj1,cameraSource);
    for(unsigned i=1;i<observed.size()&&n>0&&n<int(sizeof(line))-160;++i)
        n+=std::snprintf(line+n,sizeof(line)-n," %u=%llu/%llu/%llu/%llu/%llu",i,
            (unsigned long long)observed[i].load(),(unsigned long long)outsideWindow[i].load(),
            (unsigned long long)cadenceSkipped[i].load(),(unsigned long long)pushed[i].load(),(unsigned long long)stageDropped[i].load());
    if(n>0&&n<int(sizeof(line)))write(line);
}
}
