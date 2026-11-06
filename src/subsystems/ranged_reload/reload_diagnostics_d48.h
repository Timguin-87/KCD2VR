#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include "../../core/snapshot_mailbox.h"
#include "../../diagnostics/capture168.h"
#include "../../diagnostics/reload_capture_d84.h"
#include "crossbow_clock_entry_d87.h"

namespace kcd2vr::reload_diagnostics_d48 {
namespace cap=diagnostics::capture168;
namespace capture=diagnostics::reload_capture_d84;
struct Source {std::uint64_t session=0,tick=0;const char* why=nullptr;cap::Domain domain=cap::Domain::Reload;unsigned activeMask=0;};
struct Event {
    cap::Domain domain=cap::Domain::Reload;unsigned activeMask=0;
    std::uint64_t session=0,order=0,tick=0,sourceTick=0,serial=0,sample=0;
    std::uintptr_t parent=0,child=0,equipment=0;
    const char* why=nullptr;const char* sourceWhy=nullptr;
    unsigned stage=0,phase=0,reason=0;int primary=-1,classId=-1;
    float progress=0,speed=0;bool owned=false;
    std::uint32_t thread=0,ownerThread=0;unsigned char requestA=0,requestB=0;
    std::uint64_t sourcePublication=0,passengerFramePublication=0,nativePublication=0;
    crossbow_clock_entry_d87::SelectionObservation clockD87{};
    std::uintptr_t observedListD87=0,expectedListD87=0;
    int animationD87=-1,segmentD87=-1;float fromD87=-1,toD87=-1,permittedD87=-1;unsigned factsD87=0;
};
inline core::SnapshotMailbox<Source> source;
inline std::array<Event,64> events{};
inline unsigned head=0,tail=0;
inline std::atomic_flag lock=ATOMIC_FLAG_INIT;
inline std::atomic<std::uint64_t> next{0},dropped{0};
inline bool requested() noexcept {return capture::requested();}
inline std::uint64_t session(std::uint64_t now) noexcept {return capture::select(now).session;}
inline void recordSource(const char* why,std::uint64_t now) noexcept {
    const auto ticket=capture::select(now);if(ticket.session)(void)source.publish({ticket.session,now,why,ticket.domain,ticket.activeMask});
}
inline bool push(Event e) noexcept {
    if(!capture::current({e.domain,e.session,e.activeMask},e.tick))return false;
    e.order=next.fetch_add(1,std::memory_order_relaxed)+1;
    if(lock.test_and_set(std::memory_order_acquire)){dropped.fetch_add(1,std::memory_order_relaxed);return false;}
    const unsigned end=(head+1)%events.size();
    if(end==tail){lock.clear(std::memory_order_release);dropped.fetch_add(1,std::memory_order_relaxed);return false;}
    events[head]=e;head=end;lock.clear(std::memory_order_release);return true;
}
inline void recordStart(const char* why,std::uint64_t now,std::uintptr_t parent) noexcept {
    const auto ticket=capture::select(now);const auto s=ticket.session;if(!s)return;
    Event e{};e.stage=1;e.session=s;e.domain=ticket.domain;e.activeMask=ticket.activeMask;e.tick=now;e.parent=parent;e.why=why;
    Source observed{};if(source.read(observed)&&observed.domain==ticket.domain&&observed.session==s&&observed.tick<=now){e.sourceWhy=observed.why;e.sourceTick=observed.tick;}
    (void)push(e);
}
// D57: the native request can pass through before the old frame-thread logger.
// Retain only arguments already supplied by that callback. No native reads,
// formatting, clock call, new gameplay ownership, or capture lease refresh.
inline void recordEarlyPass(bool replay,std::uint64_t now,std::uintptr_t equipment,
        std::uint32_t thread,std::uint32_t ownerThread,unsigned char a,unsigned char b) noexcept {
    const auto ticket=capture::select(now);const auto s=ticket.session;if(!s)return;
    Event e{};e.stage=3;e.session=s;e.domain=ticket.domain;e.activeMask=ticket.activeMask;e.tick=now;e.equipment=equipment;
    e.thread=thread;e.ownerThread=ownerThread;e.requestA=a;e.requestB=b;
    e.why=replay?"REPLAY_NATIVE_PASS":"NON_FRAME_THREAD_NATIVE_PASS";
    (void)push(e);
}
template<class Status> inline void recordState(const Status& st,std::uint64_t now) noexcept {
    const auto ticket=capture::select(now);const auto s=ticket.session;if(!s)return;
    static thread_local std::uint64_t priorSession=0,serial=0;
    static thread_local cap::Domain priorDomain=cap::Domain::Reload;
    static thread_local unsigned phase=~0u,reason=~0u,priorMask=0;
    static thread_local bool owned=false;
    if(priorDomain==ticket.domain&&priorMask==ticket.activeMask&&priorSession==s&&serial==st.owner.serial&&phase==unsigned(st.phase)&&reason==unsigned(st.reason)&&owned==st.clockOwned)return;
    Event e{};e.stage=2;e.session=s;e.domain=ticket.domain;e.activeMask=ticket.activeMask;e.tick=now;e.serial=st.owner.serial;e.sample=st.owner.source.sample;
    // Admission identity stays fixed for the whole reload, not the current motion source.
    e.sourceTick=st.owner.source.tick;e.parent=st.owner.parent;e.child=st.owner.child;
    e.phase=unsigned(st.phase);e.reason=unsigned(st.reason);e.owned=st.clockOwned;
    e.primary=st.owner.source.primary;e.classId=st.owner.source.classId;e.progress=st.progress;e.speed=st.speed;
    if(push(e)){priorDomain=ticket.domain;priorMask=ticket.activeMask;priorSession=s;serial=e.serial;phase=e.phase;reason=e.reason;owned=e.owned;}
}
// D86: actual native weapon-skinning observer outcome. This records an
// existing branch, never polls native memory or grants presentation authority.
// Capture OFF returns before any queue or deduplication work. A stable reason
// does not emit once per source publication; at most 24 edges per capture/thread.
template<class Status> inline void recordMechanismD86(const Status& st,const char* why,std::uint64_t now,
        std::uint64_t sourcePublication=0,std::uint64_t passengerFramePublication=0,std::uint64_t nativePublication=0) noexcept {
    const auto ticket=capture::select(now);if(!ticket.session)return;
    static thread_local std::uint64_t session=0,serial=0;
    static thread_local cap::Domain domain=cap::Domain::Reload;
    static thread_local const char* lastWhy=nullptr;
    static thread_local unsigned mask=0,count=0;
    if(session!=ticket.session||domain!=ticket.domain||mask!=ticket.activeMask){
        session=ticket.session;domain=ticket.domain;mask=ticket.activeMask;serial=0;lastWhy=nullptr;count=0;}
    if(count>=24||(serial==st.owner.serial&&lastWhy==why))return;
    Event e{};e.stage=4;e.session=ticket.session;e.domain=ticket.domain;e.activeMask=ticket.activeMask;
    e.tick=now;e.serial=st.owner.serial;e.sample=st.owner.source.sample;e.sourceTick=st.owner.source.tick;
    e.parent=st.owner.parent;e.child=st.owner.child;e.phase=unsigned(st.phase);e.reason=unsigned(st.reason);
    e.owned=st.clockOwned;e.primary=st.owner.source.primary;e.classId=st.owner.source.classId;e.why=why;
    e.sourcePublication=sourcePublication;e.passengerFramePublication=passengerFramePublication;e.nativePublication=nativePublication;
    if(push(e)){serial=e.serial;lastWhy=why;++count;}
}
// D87 captures existing failed clock/scan decisions. OFF returns before
// dedup/queue work. No native reads or formatting occur in these producers.
// Each domain/session/thread has a fixed 32-edge budget; records cannot renew it.
template<class Status> inline void recordNativeFaultD87(const Status& st,const char* why,std::uint64_t now,
        const crossbow_clock_entry_d87::SelectionObservation& clock={},
        std::uintptr_t observedList=0,std::uintptr_t expectedList=0,int animation=-1,int segment=-1,
        float from=-1,float to=-1,float permitted=-1,unsigned facts=0) noexcept {
    const auto ticket=capture::select(now);if(!ticket.session)return;
    struct Limit {std::uint64_t session=0,serial=0;unsigned count=0;const char* previous=nullptr;};
    static thread_local std::array<Limit,cap::DomainCount> limits{};
    auto& limit=limits[unsigned(ticket.domain)];
    if(limit.session!=ticket.session){limit={};limit.session=ticket.session;}
    if(limit.count>=32||(limit.previous==why&&limit.serial==st.owner.serial))return;
    Event e{};e.stage=5;e.session=ticket.session;e.domain=ticket.domain;e.activeMask=ticket.activeMask;
    e.tick=now;e.serial=st.owner.serial;e.parent=st.owner.parent;e.child=st.owner.child;e.phase=unsigned(st.phase);
    e.reason=unsigned(st.reason);e.owned=st.clockOwned;e.primary=st.owner.source.primary;e.classId=st.owner.source.classId;
    e.why=why;e.clockD87=clock;e.observedListD87=observedList;e.expectedListD87=expectedList;
    e.animationD87=animation;e.segmentD87=segment;e.fromD87=from;e.toD87=to;e.permittedD87=permitted;e.factsD87=facts;
    if(push(e)){limit.previous=why;limit.serial=st.owner.serial;++limit.count;}
}
inline bool pop(Event& e) noexcept {
    if(lock.test_and_set(std::memory_order_acquire))return false;
    const bool have=head!=tail;if(have){e=events[tail];tail=(tail+1)%events.size();}
    lock.clear(std::memory_order_release);return have;
}
// Control worker only: no formatting, clock read or queue write in OFF native
// producers. Starts have an order ID (two starts in one millisecond stay distinct)
// and retain the source reason observed at the start, not a later mutable reason.
template<class Sink> inline void drain(std::uint64_t now,Sink sink) noexcept {
    Event e{};unsigned budget=64;
    while(budget--&&pop(e)){
        if(!capture::receipt({e.domain,e.session,e.activeMask},now))continue;
        if(e.stage==5){
            char line[1536]{};const auto& c=e.clockD87;const auto& a=c.earlier;const auto& b=c.later;
            std::snprintf(line,sizeof(line),"D87 RELOAD_NATIVE_DECISION session=%llu order=%llu serial=%llu tick=%llu result=%s phase=%u clockOwned=%d class=%d rootLayer=%u scopeLayers=%u fifo=%llX fifoCount=%u selected=%llX duplicate=%d first=%llX firstKnown=0x%X firstSampler=%llX firstTimes=%.6f,%.6f firstSegments=%u,%u firstFlags=0x%X firstDynamic=0x%X second=%llX secondKnown=0x%X secondSampler=%llX secondTimes=%.6f,%.6f secondSegments=%u,%u secondFlags=0x%X secondDynamic=0x%X list=%llX expectedList=%llX animation=%d segment=%d from=%.7f to=%.7f permitted=%.7f facts=0x%X observedOnly=1 nativeSuccess=NOT_INFERRED recorder=%s captureMask=0x%X",
                (unsigned long long)e.session,(unsigned long long)e.order,(unsigned long long)e.serial,(unsigned long long)e.tick,e.why?e.why:"UNKNOWN",e.phase,e.owned?1:0,e.classId,
                c.layer,c.layers,(unsigned long long)c.entries,c.count,(unsigned long long)c.selected,c.duplicate?1:0,
                (unsigned long long)a.entry,a.known,(unsigned long long)a.sampler,double(a.previous),double(a.current),unsigned(a.previousSegment),unsigned(a.currentSegment),a.flags,unsigned(a.dynamic),
                (unsigned long long)b.entry,b.known,(unsigned long long)b.sampler,double(b.previous),double(b.current),unsigned(b.previousSegment),unsigned(b.currentSegment),b.flags,unsigned(b.dynamic),
                (unsigned long long)e.observedListD87,(unsigned long long)e.expectedListD87,e.animationD87,e.segmentD87,double(e.fromD87),double(e.toD87),double(e.permittedD87),e.factsD87,cap::name(e.domain),e.activeMask);
            capture::emit(sink,{e.domain,e.session,e.activeMask},line);continue;
        }
        char line[768]{};
        if(e.stage==4){
            std::snprintf(line,sizeof(line),"D86 RELOAD_MECHANISM session=%llu order=%llu serial=%llu tick=%llu result=%s sourcePublication=%llu passengerFramePublication=%llu nativePublication=%llu primary=%d class=%d phase=%u clockOwned=%d pose=ORIGINAL_WEAPON_SKINNING nativeSuccess=NOT_INFERRED recorder=%s captureMask=0x%X",
                (unsigned long long)e.session,(unsigned long long)e.order,(unsigned long long)e.serial,(unsigned long long)e.tick,e.why?e.why:"UNKNOWN",
                (unsigned long long)e.sourcePublication,(unsigned long long)e.passengerFramePublication,(unsigned long long)e.nativePublication,
                e.primary,e.classId,e.phase,e.owned?1:0,cap::name(e.domain),e.activeMask);
            capture::emit(sink,{e.domain,e.session,e.activeMask},line);continue;
        }
        const auto age=e.sourceTick&&e.tick>=e.sourceTick?static_cast<long long>(e.tick-e.sourceTick):-1;
        std::snprintf(line,sizeof(line),"D48_F11 RELOAD session=%llu order=%llu stage=%s tick=%llu parent=%llX child=%llX serial=%llu %s=%llu %s=%lld poseAge=NOT_RECORDED startReason=%s sourceReason=%s phase=%u reason=%u clockOwned=%d primary=%d class=%d progress=%.4f speed=%.4f queueDropped=%llu nativeSuccess=NOT_INFERRED equipment=%llX thread=%u ownerThread=%u requestA=%u requestB=%u recorder=%s captureMask=0x%X",
            (unsigned long long)e.session,(unsigned long long)e.order,e.stage==1?"START":e.stage==3?"REQUEST":"STATE",(unsigned long long)e.tick,
            (unsigned long long)e.parent,(unsigned long long)e.child,(unsigned long long)e.serial,
            e.stage==2?"admissionSample":"sample",(unsigned long long)e.sample,
            e.stage==2?"admissionAgeMs":"sourceObservationAgeMs",age,
            e.why?e.why:"NA",e.sourceTick?(e.sourceWhy?e.sourceWhy:"SOURCE_OK"):"UNOBSERVED",
            e.phase,e.reason,e.owned?1:0,e.primary,e.classId,double(e.progress),double(e.speed),(unsigned long long)dropped.load(std::memory_order_relaxed),
            (unsigned long long)e.equipment,unsigned(e.thread),unsigned(e.ownerThread),unsigned(e.requestA),unsigned(e.requestB),cap::name(e.domain),e.activeMask);
        capture::emit(sink,{e.domain,e.session,e.activeMask},line);
    }
}
}
