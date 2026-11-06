#pragma once
// Read-only T4 stage receipts. Producers copy existing values; only the worker
// formats. No physics/native calls, allocation, waiting or owner mutation here.
#include "capture168.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <type_traits>
namespace kcd2vr::diagnostics::physics_d42 {
namespace cap=capture168;
inline constexpr unsigned QueueSize=64,DetailBudget=2048,LaneCountD84=4;
// Preserve original F2/F7 lane indices; independent physics/combat keys add
// copied observations, never a gameplay mode or an extra native query.
inline constexpr cap::Domain DomainsD84[LaneCountD84]{cap::Domain::Bug,cap::Domain::Vrik,cap::Domain::Physics,cap::Domain::Combat};
inline constexpr unsigned CaptureMaskD84=(1u<<unsigned(cap::Domain::Bug))|(1u<<unsigned(cap::Domain::Vrik))|
    (1u<<unsigned(cap::Domain::Physics))|(1u<<unsigned(cap::Domain::Combat));
enum class Outcome:unsigned {Off,Fallback,WaitingCreate,WaitingInvalid,Revoked,Applied};
struct Receipt {
    std::uint64_t tick=0,inputTick=0,sample=0,sourceTick=0,observedTick=0,sourceSample=0;
    std::uint64_t request=0,epoch=0,key=0,generation=0,lease=0,bodyGeneration=0,worldGeneration=0;
    std::uint64_t trackingEpoch=0,cancelGeneration=0,admission=0;
    std::uint32_t entity=0;unsigned evaluated=0,passed=0,applyReason=0,state=0,reason=0;
    int bodyId=-1;Outcome outcome=Outcome::Off;
    bool enabled=false,busy=false,sourceRead=false,achievedRead=false,contactRead=false,contactActive=false;
    bool ownerMatch=false,leaseMatch=false,trackingMatch=false,cancelMatch=false,poseValid=false;
    double target[3]{},achieved[3]{};bool targetValid=false,achievedValid=false;
};
struct Lease {std::uint64_t serial[LaneCountD84]{};bool on()const noexcept{for(auto s:serial)if(s)return true;return false;}};
inline constexpr cap::Domain domain(unsigned lane) noexcept{return DomainsD84[lane];}
inline constexpr unsigned keyNumberD84(unsigned lane) noexcept{return lane==0?2u:lane==1?7u:lane==2?9u:3u;}
template<class Write>inline void emitD84(Write&write,unsigned lane,std::uint64_t serial,const char*line){
    if constexpr(std::is_invocable_v<Write&,cap::Domain,std::uint64_t,const char*>)write(domain(lane),serial,line);
    else if constexpr(std::is_invocable_v<Write&,cap::Domain,const char*>)write(domain(lane),line);
    else write(line); // compatibility for portable legacy consumers
}
inline Lease lease(std::uint64_t now) noexcept {
    Lease r{};const auto mask=cap::gate.mask.load(std::memory_order_relaxed);
    if(!(mask&CaptureMaskD84))return r;
    for(unsigned k=0;k<LaneCountD84;++k)if((mask&(1u<<unsigned(domain(k))))&&cap::active(domain(k),now))
        r.serial[k]=cap::gate.session[unsigned(domain(k))].load(std::memory_order_acquire);
    return r;
}
inline bool current(unsigned k,std::uint64_t serial,std::uint64_t now) noexcept {
    return serial&&cap::active(domain(k),now)&&serial==cap::gate.session[unsigned(domain(k))].load(std::memory_order_acquire);
}
struct Account {
    std::uint64_t serial=0,firstTick=0,lastTick=0,lastDetail=0,lastReport=0;
    std::uint64_t observed=0,queued=0,forwarded=0,throttled=0,budgetDropped=0,queueFull=0,discarded=0;
    std::uint64_t edges=0,suppressedEdges=0,duplicateSource=0,backwards=0,gap80=0;
    Receipt previous{},lastRefusal{};bool have=false,lastRefusalValid=false;
};
struct Entry {Receipt receipt{};std::uint64_t serial=0;unsigned lane=0;};
struct Storage {
    std::atomic_flag lock=ATOMIC_FLAG_INIT;
    std::atomic<bool> pending{false};
    std::array<std::atomic<std::uint64_t>,LaneCountD84> busy{};
    std::array<Account,LaneCountD84> accounts{};std::array<Entry,QueueSize> records{};
    unsigned head=0,size=0;
};
inline Storage storage;
struct Guard {bool ok;Guard()noexcept:ok(!storage.lock.test_and_set(std::memory_order_acquire)){}~Guard(){if(ok)storage.lock.clear(std::memory_order_release);}};
inline bool edge(const Receipt&a,const Receipt&b)noexcept {
    return a.outcome!=b.outcome||a.evaluated!=b.evaluated||a.passed!=b.passed||a.applyReason!=b.applyReason||
        a.key!=b.key||a.generation!=b.generation||a.request!=b.request||a.epoch!=b.epoch||a.lease!=b.lease||
        a.bodyGeneration!=b.bodyGeneration||a.worldGeneration!=b.worldGeneration||a.trackingEpoch!=b.trackingEpoch||
        a.cancelGeneration!=b.cancelGeneration||a.admission!=b.admission||a.state!=b.state||a.reason!=b.reason||
        a.enabled!=b.enabled||a.busy!=b.busy||a.sourceRead!=b.sourceRead||a.achievedRead!=b.achievedRead||
        a.contactRead!=b.contactRead||a.contactActive!=b.contactActive||a.ownerMatch!=b.ownerMatch||
        a.leaseMatch!=b.leaseMatch||a.trackingMatch!=b.trackingMatch||a.cancelMatch!=b.cancelMatch||a.poseValid!=b.poseValid;
}
// A session must be retired by drain before a new one takes its counter slot.
// This rare race is counted rather than silently replacing the old summary.
inline void observe(const Lease& capture,const Receipt&r) noexcept {
    if(!capture.on())return;
    Guard g;if(!g.ok){for(unsigned k=0;k<LaneCountD84;++k)if(capture.serial[k])++storage.busy[k];return;}
    for(unsigned k=0;k<LaneCountD84;++k){
        if(!current(k,capture.serial[k],r.tick))continue;
        auto& a=storage.accounts[k];
        if(a.serial&&a.serial!=capture.serial[k]){++storage.busy[k];continue;}
        if(!a.serial){a.serial=capture.serial[k];a.firstTick=r.tick;}
        storage.pending.store(true,std::memory_order_release);
        ++a.observed;const bool changed=!a.have||edge(r,a.previous);if(changed)++a.edges;
        if(a.have&&a.previous.epoch==r.epoch&&a.previous.request==r.request){
            if(r.sample==a.previous.sample)++a.duplicateSource;
            if(r.sample<a.previous.sample||r.inputTick<a.previous.inputTick)++a.backwards;
            if(r.inputTick>=a.previous.inputTick&&r.inputTick-a.previous.inputTick>80)++a.gap80;
        }
        a.previous=r;a.have=true;a.lastTick=r.tick;
        // A one-frame failure must survive detail throttling or exhaustion.
        // This is the last observed refusal, never evidence of a current pose.
        if(r.outcome==Outcome::WaitingCreate||r.outcome==Outcome::WaitingInvalid||r.outcome==Outcome::Revoked||
           (r.outcome==Outcome::Fallback&&r.enabled)){a.lastRefusal=r;a.lastRefusalValid=true;}
        if(a.lastDetail&&r.tick>=a.lastDetail&&r.tick-a.lastDetail<(changed?25u:500u)){
            ++a.throttled;if(changed)++a.suppressedEdges;continue;
        }
        if(a.queued>=DetailBudget){++a.budgetDropped;if(changed)++a.suppressedEdges;continue;}
        if(storage.size==QueueSize){++a.queueFull;if(changed)++a.suppressedEdges;continue;}
        storage.records[(storage.head+storage.size)%QueueSize]={r,a.serial,k};++storage.size;
        ++a.queued;a.lastDetail=r.tick;
    }
}
struct Scope {
    Lease capture{};Receipt receipt{};
    Scope(std::uint64_t tick,std::uint64_t sample)noexcept:capture(lease(tick)){receipt.tick=receipt.inputTick=tick;receipt.sample=sample;}
    template<class Clock>Scope(std::uint64_t inputTick,std::uint64_t sample,Clock&&clock)noexcept {
        // Off path does not read the clock. Input time and observation time are
        // separate so a stale publication cannot look freshly observed.
        const auto mask=cap::gate.mask.load(std::memory_order_relaxed);
        if(mask&CaptureMaskD84){
            receipt.tick=clock();capture=lease(receipt.tick);
        }
        receipt.inputTick=inputTick;receipt.sample=sample;
    }
    bool on()const noexcept{return capture.on();}
    bool check(unsigned bit,bool value)noexcept {if(on()){receipt.evaluated|=1u<<bit;if(value)receipt.passed|=1u<<bit;}return value;}
    ~Scope(){observe(capture,receipt);}
};
inline const char* outcome(Outcome o) noexcept {
    switch(o){case Outcome::Off:return "OFF";case Outcome::Fallback:return "FALLBACK";
        case Outcome::WaitingCreate:return "WAIT_CREATE_CONFIG";case Outcome::WaitingInvalid:return "WAIT_INVALID_ACHIEVED";
        case Outcome::Revoked:return "SOURCE_REVOKED";default:return "APPLIED";}
}
inline long long age(std::uint64_t now,std::uint64_t then) noexcept {return then&&now>=then?static_cast<long long>(now-then):-1;}
inline int format(char*line,std::size_t n,const Entry&e) noexcept {
    const auto&r=e.receipt;
    const double dx=r.target[0]-r.achieved[0],dy=r.target[1]-r.achieved[1],dz=r.target[2]-r.achieved[2];
    const double error=r.targetValid&&r.achievedValid?std::sqrt(dx*dx+dy*dy+dz*dz)*1000.:-1.;
    return std::snprintf(line,n,
        "D42_F%u T4_STAGE session=%llu tick=%llu inputTick=%llu inputAgeMs=%lld sample=%llu outcome=%s evaluated=0x%X passed=0x%X applyReason=%u "
        "enabled=%d colliderBusy=%d sourceRead=%d achievedRead=%d state=%u reason=%u poseValid=%d "
        "key=%llu generation=%llu entity=%u request=%llu epoch=%llu lease=%llu bodyId=%d bodyGen=%llu worldGen=%llu "
        "trackingEpoch=%llu cancelGen=%llu admission=%llu ownerMatch=%d leaseMatch=%d trackingMatch=%d cancelMatch=%d "
        "achievedSample=%llu sourceAgeMs=%lld observedAgeMs=%lld contactRead=%d contactActive=%d targetErrorMm=%.3f "
        "targetWorld=%.4f,%.4f,%.4f achievedWorld=%.4f,%.4f,%.4f geometry=WHOLE_CAPSULE_FEATURE_UNKNOWN impaleProvider=UNCONNECTED",
        keyNumberD84(e.lane),(unsigned long long)e.serial,(unsigned long long)r.tick,(unsigned long long)r.inputTick,age(r.tick,r.inputTick),(unsigned long long)r.sample,outcome(r.outcome),r.evaluated,r.passed,r.applyReason,
        int(r.enabled),int(r.busy),int(r.sourceRead),int(r.achievedRead),r.state,r.reason,int(r.poseValid),
        (unsigned long long)r.key,(unsigned long long)r.generation,r.entity,(unsigned long long)r.request,(unsigned long long)r.epoch,
        (unsigned long long)r.lease,r.bodyId,(unsigned long long)r.bodyGeneration,(unsigned long long)r.worldGeneration,
        (unsigned long long)r.trackingEpoch,(unsigned long long)r.cancelGeneration,(unsigned long long)r.admission,
        int(r.ownerMatch),int(r.leaseMatch),int(r.trackingMatch),int(r.cancelMatch),
        (unsigned long long)r.sourceSample,age(r.tick,r.sourceTick),age(r.tick,r.observedTick),int(r.contactRead),int(r.contactActive),error,
        r.target[0],r.target[1],r.target[2],r.achieved[0],r.achieved[1],r.achieved[2]);
}
struct Summary {Account a{};unsigned lane=0;bool ended=false;std::uint64_t busy=0;};
// Existing mod worker calls this. At most 16 details + four summaries per call;
// no synchronous native/game readback, and END survives capture expiry.
template<class Write>inline void drain(std::uint64_t now,Write&&write,bool closing=false) noexcept {
    if(!storage.pending.load(std::memory_order_acquire))return;
    std::array<Entry,16> details{};unsigned count=0;std::array<Summary,LaneCountD84> reports{};unsigned reportCount=0;
    {Guard g;if(!g.ok)return;
     for(unsigned k=0;k<LaneCountD84;++k){auto&a=storage.accounts[k];if(!a.serial)continue;
        if(closing||!current(k,a.serial,now)){
            // Count and retire old queued details before final summary. They
            // cannot leak into a new capture or be presented as current poses.
            for(unsigned j=0;j<storage.size;++j){auto&e=storage.records[(storage.head+j)%QueueSize];
                if(e.lane==k&&e.serial==a.serial){++a.discarded;e.serial=0;}}
            reports[reportCount++]={a,k,true,storage.busy[k].load(std::memory_order_relaxed)};a={};
        }
     }
     bool pending=storage.size!=0;for(const auto&a:storage.accounts)pending=pending||a.serial!=0;
     storage.pending.store(pending,std::memory_order_release);
     while(storage.size&&count<details.size()){
        auto e=storage.records[storage.head];storage.head=(storage.head+1)%QueueSize;--storage.size;
        if(!e.serial)continue;auto&a=storage.accounts[e.lane];
        if(a.serial==e.serial&&current(e.lane,e.serial,now)){details[count++]=e;++a.forwarded;}
        else if(a.serial==e.serial)++a.discarded;
     }
     for(unsigned k=0;k<LaneCountD84;++k){auto&a=storage.accounts[k];if(!a.serial)continue;
        if(!a.lastReport||now<a.lastReport||now-a.lastReport>=1000){
            a.lastReport=now;reports[reportCount++]={a,k,false,storage.busy[k].load(std::memory_order_relaxed)};
        }
     }
    }
    char line[1800]{};
    for(unsigned i=0;i<count;++i){if(!current(details[i].lane,details[i].serial,now))continue;
        if(format(line,sizeof(line),details[i])>0)emitD84(write,details[i].lane,details[i].serial,line);}
    for(unsigned i=0;i<reportCount;++i){const auto&s=reports[i];const auto&a=s.a;
        std::snprintf(line,sizeof(line),
            "D42_F%u%s T4_ACCOUNT session=%llu observed=%llu queued=%llu forwarded=%llu throttled=%llu budgetDropped=%llu "
            "queueFull=%llu discarded=%llu edges=%llu suppressedEdges=%llu duplicateSource=%llu backwards=%llu gapOver80ms=%llu "
            "firstTick=%llu lastTick=%llu detailBudget=%u busyProcessTotal=%llu lastRefusalKnown=%d lastRefusalTick=%llu "
            "lastRefusalSample=%llu lastRefusalOutcome=%s lastRefusalEvaluated=0x%X lastRefusalPassed=0x%X lastRefusalApplyReason=%u "
            "lastRefusalKey=%llu lastRefusalRequest=%llu lastRefusalEpoch=%llu lastRefusal=HISTORICAL_OBSERVATION "
            "coverage=STAGE_CALLS_ONLY downstreamQueueAck=NOT_CLAIMED",
            keyNumberD84(s.lane),s.ended?"_END":"",(unsigned long long)a.serial,(unsigned long long)a.observed,
            (unsigned long long)a.queued,(unsigned long long)a.forwarded,(unsigned long long)a.throttled,
            (unsigned long long)a.budgetDropped,(unsigned long long)a.queueFull,(unsigned long long)a.discarded,
            (unsigned long long)a.edges,(unsigned long long)a.suppressedEdges,(unsigned long long)a.duplicateSource,
            (unsigned long long)a.backwards,(unsigned long long)a.gap80,(unsigned long long)a.firstTick,
            (unsigned long long)a.lastTick,DetailBudget,(unsigned long long)s.busy,int(a.lastRefusalValid),
            (unsigned long long)a.lastRefusal.tick,(unsigned long long)a.lastRefusal.sample,outcome(a.lastRefusal.outcome),
            a.lastRefusal.evaluated,a.lastRefusal.passed,a.lastRefusal.applyReason,(unsigned long long)a.lastRefusal.key,
            (unsigned long long)a.lastRefusal.request,(unsigned long long)a.lastRefusal.epoch);emitD84(write,s.lane,a.serial,line);
    }
}
}
