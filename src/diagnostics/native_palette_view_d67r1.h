#pragma once
#include "native_palette_d67r1.h"
#include <cstring>

namespace kcd2vr::diagnostics::native_palette_d67r1::view {
// Called only from the existing qualified main-view Standard entry. Native
// command 1DF78FA -> 7C2D40 -> 505DA8 uploads before 1DF7923 -> 864728.
// The extra eye invokes 864728 again, not the upload command. This receipt
// observes that live view's upload-pool identity, not a particular mesh draw.
struct Snapshot {
    std::uintptr_t rendererVt=0,viewVt=0,pipelineVt=0,viewPipeline=0,pipelineView=0,bulkOwner=0;
    std::uint32_t type=~0u,frame=0,selector=~0u;
    std::int32_t pool=-1,rendererPool=-1;
};
inline bool same(const Snapshot& a,const Snapshot& b) noexcept {
    return a.rendererVt==b.rendererVt&&a.viewVt==b.viewVt&&a.pipelineVt==b.pipelineVt&&
        a.viewPipeline==b.viewPipeline&&a.pipelineView==b.pipelineView&&a.bulkOwner==b.bulkOwner&&
        a.type==b.type&&a.frame==b.frame&&a.selector==b.selector&&a.pool==b.pool&&a.rendererPool==b.rendererPool;
}
template<class Reader,class T> bool field(Reader& read,std::uintptr_t base,std::uintptr_t off,T& value) noexcept {
    return base&&off<=UINTPTR_MAX-base&&read(base+off,&value,sizeof(value));
}
template<class Reader> bool abi(Reader& read,std::uintptr_t wh) noexcept {
    std::uint16_t mz=0,magic=0;std::uint32_t nt=0,pe=0,timestamp=0,size=0;
    if(!field(read,wh,0,mz)||mz!=0x5A4D||!field(read,wh,0x3C,nt)||nt<0x40||nt>0x1000||
       !field(read,wh,nt,pe)||pe!=0x4550||!field(read,wh,nt+8,timestamp)||timestamp!=0x6A350E20u||
       !field(read,wh,nt+0x18,magic)||magic!=0x20B||!field(read,wh,nt+0x50,size)||size!=0x5B2D000u)return false;
    const auto match=[&](std::uintptr_t rva,const auto& expected){
        unsigned char bytes[8]{};return sizeof(expected)<=sizeof(bytes)&&
            read(wh+rva,bytes,sizeof(expected))&&!std::memcmp(bytes,expected,sizeof(expected));
    };
    const unsigned char owner[]{0x48,0x8B,0x82,0x60,0x8D,0,0};
    const unsigned char bulk[]{0x48,0x8B,0x58,0x58};
    const unsigned char pool[]{0x48,0x63,0x8F,0x40,0x8D,0,0};
    const unsigned char prepare[]{0xE8,0x41,0xB4,0x9C,0xFE};
    const unsigned char pipeline[]{0xE8,0,0xCE,0xA6,0xFE};
    const unsigned char rendererPool[]{0x8B,0x81,0x4C,0x02,0,0,0xC3};
    return match(0x505DC0,owner)&&match(0x505DCD,bulk)&&match(0x505E61,pool)&&
        match(0x1DF78FA,prepare)&&match(0x1DF7923,pipeline)&&match(0x4A48F0,rendererPool);
}
template<class Reader> bool snapshot(Reader& read,std::uintptr_t renderer,std::uintptr_t crv,std::uintptr_t pipeline,Snapshot& s) noexcept {
    return field(read,renderer,0,s.rendererVt)&&field(read,crv,0,s.viewVt)&&field(read,pipeline,0,s.pipelineVt)&&
        field(read,crv,0x8D60,s.viewPipeline)&&field(read,pipeline,0x1978,s.pipelineView)&&
        field(read,pipeline,0x58,s.bulkOwner)&&field(read,crv,0x14,s.type)&&field(read,crv,0x2C,s.frame)&&
        field(read,crv,0x8D40,s.pool)&&field(read,crv,0x8EE0,s.selector)&&field(read,renderer,0x24C,s.rendererPool);
}
enum class Status : unsigned {Unknown,AbiRefused,Unreadable,IdentityRefused,Changed,Stable};
struct Record {
    std::uint64_t session=0,tick=0,cpuNs=0,readNs=0,epoch=0;
    std::uintptr_t renderer=0,crv=0,pipeline=0;
    std::uint32_t frame=0,eye=~0u,thread=0;Status status=Status::Unknown;Snapshot live{};
};
inline std::array<Record,128> queue{};inline unsigned head=0,tail=0;
inline std::atomic_flag queueLock=ATOMIC_FLAG_INIT,observerLock=ATOMIC_FLAG_INIT;
inline std::atomic<std::uint64_t> dropped{0},suppressed{0},observed{0},outsideBurst{0},expired{0},formatDropped{0};
inline Budget budget{};inline std::uintptr_t checkedWh=0;inline bool checkedAbi=false;
inline bool enabled() noexcept {return motion_d68::requested();}
inline bool push(const Record& r) noexcept {
    if(!current(r.session))return false;
    if(queueLock.test_and_set(std::memory_order_acquire)){dropped.fetch_add(1,std::memory_order_relaxed);return false;}
    const auto next=(head+1)%queue.size();
    if(next==tail){queueLock.clear(std::memory_order_release);dropped.fetch_add(1,std::memory_order_relaxed);return false;}
    queue[head]=r;head=unsigned(next);queueLock.clear(std::memory_order_release);return true;
}
inline bool pop(Record& r) noexcept {
    if(queueLock.test_and_set(std::memory_order_acquire))return false;
    const bool have=head!=tail;if(have){r=queue[tail];tail=unsigned((tail+1)%queue.size());}
    queueLock.clear(std::memory_order_release);return have;
}
template<class Reader> void observe(Reader read,std::uintptr_t wh,std::uintptr_t renderer,std::uintptr_t crv,
    std::uintptr_t pipeline,std::uint32_t frame,std::uint32_t eye,std::uint64_t epoch,std::uint64_t now,unsigned thread) noexcept {
    if(!enabled())return; // zero native reads/clock/queue when Shift+F2 is off
    motion_d68::Event capture{};
    if(!motion_d68::begin(capture,motion_d68::RenderRequest,false))return;
    observed.fetch_add(1,std::memory_order_relaxed);
    if(!motion_d68::inWindow(capture,false)){outsideBurst.fetch_add(1,std::memory_order_relaxed);return;}
    const auto session=capture.session;
    if(observerLock.test_and_set(std::memory_order_acquire)){dropped.fetch_add(1,std::memory_order_relaxed);return;}
    if(!budget.take(session,now)){suppressed.fetch_add(1,std::memory_order_relaxed);observerLock.clear(std::memory_order_release);return;}
    Record r{};r.session=session;r.tick=now;r.cpuNs=capture.ns;r.epoch=epoch;r.renderer=renderer;r.crv=crv;r.pipeline=pipeline;r.frame=frame;r.eye=eye;r.thread=thread;
    const bool ready=kcd2vr::retail_build112::Ready(wh);
    if(ready&&(!checkedWh||checkedWh!=wh)){checkedWh=wh;checkedAbi=abi(read,wh);}
    if(!ready||!checkedAbi)r.status=Status::AbiRefused;
    else {
        Snapshot second{};
        if(!snapshot(read,renderer,crv,pipeline,r.live)||!snapshot(read,renderer,crv,pipeline,second))r.status=Status::Unreadable;
        else if(!same(r.live,second))r.status=Status::Changed;
        else if(r.live.rendererVt!=wh+0x3DFC008||r.live.viewVt!=wh+0x3DF25C0||r.live.pipelineVt!=wh+0x3DFEBD0||
                r.live.viewPipeline!=pipeline||r.live.pipelineView!=crv||r.live.type!=0||r.live.frame!=frame||
                r.live.selector!=eye||eye>1||r.live.pool<0||!r.live.bulkOwner)r.status=Status::IdentityRefused;
        else r.status=Status::Stable;
    }
    r.readNs=clockNs()-r.cpuNs;observerLock.clear(std::memory_order_release);(void)push(r);
}
template<class Writer> void drain(std::uint64_t now,Writer&& write) noexcept {
    Record r{};unsigned count=32;
    while(count--&&pop(r)){
        if(!perf_d61::gate.receipt(r.session,now)){expired.fetch_add(1,std::memory_order_relaxed);continue;}
        char line[1000]{};
        const int n=std::snprintf(line,sizeof(line),
            "D68_MOTION session=%llu stage=PALETTE_VIEW tick=%llu ns=%llu readNs=%llu thread=%u epoch=%llu frame=%u eye=%u status=%u renderer=%llX crv=%llX pipeline=%llX liveFrame=%u liveEye=%u uploadPool=%d rendererPool=%d bulkOwner=%llX queueDropped=%llu suppressed=%llu maxPerSecond=240 queueCapacity=127 drainBudget=32 boundary=STANDARD_ENTER uploadBeforePipeline=STATIC_EXACT extraEyeUpload=NONE_STATIC henryDraw=UNOBSERVED gpuBinding=UNOBSERVED statusMeaning=UNKNOWN:0,ABI_REFUSED:1,UNREADABLE:2,IDENTITY_REFUSED:3,CHANGED:4,STABLE:5",
            (unsigned long long)r.session,(unsigned long long)r.tick,(unsigned long long)r.cpuNs,(unsigned long long)r.readNs,r.thread,
            (unsigned long long)r.epoch,r.frame,r.eye,unsigned(r.status),(unsigned long long)r.renderer,(unsigned long long)r.crv,
            (unsigned long long)r.pipeline,r.live.frame,r.live.selector,r.live.pool,r.live.rendererPool,(unsigned long long)r.live.bulkOwner,
            (unsigned long long)dropped.load(std::memory_order_relaxed),(unsigned long long)suppressed.load(std::memory_order_relaxed));
        if(n>0&&std::size_t(n)<sizeof(line))write(line);else formatDropped.fetch_add(1,std::memory_order_relaxed);
    }
    static std::uint64_t last=0,session=0;
    const auto currentSession=perf_d61::gate.serial.load(std::memory_order_acquire);
    if(perf_d61::gate.current(currentSession,now)&&(session!=currentSession||!last||now<last||now-last>=1000)){
        session=currentSession;last=now;char line[640]{};
        std::snprintf(line,sizeof(line),
            "D68_MOTION session=%llu stage=PALETTE_HEALTH family=VIEW observed=%llu outsideBurst=%llu suppressed=%llu queueDropped=%llu expired=%llu formatDropped=%llu burstMs=250 periodMs=2000 maxPerSecond=240 queueCapacity=127 drainBudget=32 counters=CUMULATIVE henryDraw=UNOBSERVED gpuBinding=UNOBSERVED",
            (unsigned long long)session,(unsigned long long)observed.load(std::memory_order_relaxed),
            (unsigned long long)outsideBurst.load(std::memory_order_relaxed),(unsigned long long)suppressed.load(std::memory_order_relaxed),
            (unsigned long long)dropped.load(std::memory_order_relaxed),(unsigned long long)expired.load(std::memory_order_relaxed),
            (unsigned long long)formatDropped.load(std::memory_order_relaxed));
        write(line);
    }
}
}
