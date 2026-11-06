#pragma once
// Finite F3/F5 collection only. Fixed data queue, try-lock/drop, no formatting
// or allocation on pose/physics hooks. No capture flag authorizes motion.
#include "capture168.h"
#include "../vrik/pose_math.h"
#include "../vrik/sweep_evidence169r2.h"
#include <array>
#include <atomic>
#include <cstdio>
namespace kcd2vr::diagnostics::bow169r2 {
namespace m=pose_core;
inline bool recording(std::uint64_t now) noexcept {return capture168::active(capture168::Domain::Combat,now)||capture168::active(capture168::Domain::Interaction,now);}
struct Record {
    unsigned kind=0,stage=0,line=0;int side=0;
    std::uint64_t tick=0,sample=0,epoch=0,request=0,key=0,generation=0,actor=0,combat=0,interaction=0;
    m::Vec3 target{},allowed{},final{},shoulder{},elbow{};
    double chestYaw=0,pull=0,armGap=0,reachError=0,wristError=0;
    bool fresh=false,geometryReady=false,latched=false,owned=false,carry=false,committed=false,posePacketPublished=false,physicalPublishAttempted=false,worldSame=false;
    sweep169r2::Evidence sweep{};bool acquire=false,known=false,limited=false;
    double fraction=0;unsigned raysUsed=0,raysRemaining=0;std::uint64_t workUs=0,totalUs=0;
};
inline std::array<Record,128> queue{};inline unsigned head=0,tail=0;
inline std::atomic_flag lock=ATOMIC_FLAG_INIT;
inline std::atomic<std::uint64_t> drops{0},limited{0};
inline std::array<std::atomic<std::uint64_t>,4> lastTick{};
inline std::array<std::atomic<std::uint64_t>,unsigned(sweep169r2::Reason::Count)> sweepCounts{};
inline bool push(Record r) noexcept {
    if(!recording(r.tick))return false;
    r.combat=capture168::active(capture168::Domain::Combat,r.tick)?capture168::gate.session[0].load():0;
    r.interaction=capture168::active(capture168::Domain::Interaction,r.tick)?capture168::gate.session[1].load():0;
    const unsigned slot=r.kind?2u+unsigned(r.side&1):unsigned(r.stage?1:0);
    if(r.kind){const auto reason=unsigned(r.sweep.reason);if(reason<sweepCounts.size())++sweepCounts[reason];}
    auto prior=lastTick[slot].load(std::memory_order_relaxed);
    if(prior&&r.tick>=prior&&r.tick-prior<100){++limited;return false;}
    if(!lastTick[slot].compare_exchange_strong(prior,r.tick,std::memory_order_relaxed)){++limited;return false;}
    if(lock.test_and_set(std::memory_order_acquire)){++drops;return false;}
    const unsigned next=(head+1)%queue.size();if(next==tail){lock.clear(std::memory_order_release);++drops;return false;}
    queue[head]=r;head=next;lock.clear(std::memory_order_release);return true;
}
inline bool pop(Record& r) noexcept {
    if(lock.test_and_set(std::memory_order_acquire))return false;
    const bool ok=tail!=head;if(ok){r=queue[tail];tail=(tail+1)%queue.size();}
    lock.clear(std::memory_order_release);return ok;
}
template<class Write> inline void drain(std::uint64_t now,Write&& write) noexcept {
    Record r{};unsigned budget=32;
    while(budget--&&pop(r)){
        char b[1800]{};
        if(!r.kind){
            std::snprintf(b,sizeof(b),
                "V66.169R2 BOW_POSE tick=%llu sample=%llu epoch=%llu request=%llu key=%016llX generation=%llu actorKind=CHARACTER actor=%llX combatSession=%llu interactionSession=%llu stage=%u rejectLine=%u geometryReady=%d intentFresh=%d latched=%d carry=%d rightOwned=%d committed=%d posePacketPublished=%d physicalPublishAttempted=%d worldSame=%d chestYawDeg=%.3f pullM=%.5f target=(%.5f,%.5f,%.5f) allowed=(%.5f,%.5f,%.5f) final=(%.5f,%.5f,%.5f) shoulder=(%.5f,%.5f,%.5f) elbow=(%.5f,%.5f,%.5f) wristErrorM=%.6f reachExcessM=%.6f armTorsoProxyGapM=%.6f cameraWrites=NONE headPositionOwnedBy=HMD elbow=INFERRED_NOT_TRACKED",
                (unsigned long long)r.tick,(unsigned long long)r.sample,(unsigned long long)r.epoch,(unsigned long long)r.request,(unsigned long long)r.key,(unsigned long long)r.generation,
                (unsigned long long)r.actor,(unsigned long long)r.combat,(unsigned long long)r.interaction,r.stage,r.line,r.geometryReady,r.fresh,r.latched,r.carry,r.owned,r.committed,r.posePacketPublished,r.physicalPublishAttempted,r.worldSame,
                r.chestYaw*57.295779513,r.pull,r.target.x,r.target.y,r.target.z,r.allowed.x,r.allowed.y,r.allowed.z,r.final.x,r.final.y,r.final.z,
                r.shoulder.x,r.shoulder.y,r.shoulder.z,r.elbow.x,r.elbow.y,r.elbow.z,r.wristError,r.reachError,r.armGap);
        }else{
            const auto& e=r.sweep;
            std::snprintf(b,sizeof(b),
                "V66.169R2 SWEEP_COVERAGE tick=%llu sample=%llu epoch=%llu request=%llu key=%016llX generation=%llu actorKind=CHARACTER actor=%llX side=%c acquire=%d combatSession=%llu interactionSession=%llu reason=%s segments=%u/%u raysCompleted=%u raysAttempted=%u requestedDeg=%.4f checkedDeg=%.4f known=%d limited=%d fraction=%.6f passUsed=%u passRemaining=%u chargedQueryUs=%llu captureWallUs=%llu budgetUnchanged=18_RAYS_1000US chordTargetM=0.004 motionThroughUnknown=NONE",
                (unsigned long long)r.tick,(unsigned long long)r.sample,(unsigned long long)r.epoch,(unsigned long long)r.request,(unsigned long long)r.key,(unsigned long long)r.generation,(unsigned long long)r.actor,
                r.side?'L':'R',r.acquire,(unsigned long long)r.combat,(unsigned long long)r.interaction,sweep169r2::name(e.reason),e.completed,e.planned,e.completedRays,e.attemptedRays,
                e.requestedAngle*57.295779513,e.checkedAngle*57.295779513,r.known,r.limited,r.fraction,r.raysUsed,r.raysRemaining,(unsigned long long)r.workUs,(unsigned long long)r.totalUs);
        }
        write(b);
    }
    static std::uint64_t last=0;static bool was=false;
    const bool active=recording(now);
    if((active&&(!last||now<last||now-last>=2000))||(!active&&was)){
        last=now;char b[1100]{};int n=std::snprintf(b,sizeof(b),"V66.169R2 SWEEP_SUMMARY active=%d countBasis=CAPTURED_QUERY_CALLS_NOT_FRAMES deltasSinceLastDrainWindow=1",active);
        for(unsigned i=0;i<sweepCounts.size();++i)if(n>0&&n<int(sizeof(b))-80)n+=std::snprintf(b+n,sizeof(b)-n," %s=%llu",sweep169r2::name(sweep169r2::Reason(i)),(unsigned long long)sweepCounts[i].exchange(0));
        if(n>0&&n<int(sizeof(b))-140)std::snprintf(b+n,sizeof(b)-n," droppedTotal=%llu detailRateLimitedTotal=%llu queueCapacity=127",(unsigned long long)drops.load(),(unsigned long long)limited.load());
        write(b);
    }
    was=active;
}
}
