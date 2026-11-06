#pragma once
// D77: copied lifecycle evidence. No native reads/writes, engine calls, heap,
// formatting or I/O in native producers. F5/F7/F11 record independently;
// each row carries its original domain/session and the active recorder mask.
#include "capture168.h"
#include "reload_capture_d84.h"
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
namespace kcd2vr::diagnostics::activity_links_d77 {
namespace cap=capture168;
namespace capture=reload_capture_d84;
struct ReloadEvent {
    cap::Domain domain=cap::Domain::Reload;unsigned activeMask=0;
    std::uint64_t session=0,tick=0,serial=0,request=0,sourceTick=0,sample=0,generation=0,publication=0;
    std::uintptr_t player=0,parent=0,child=0,toolEntity=0,boltEntity=0,toolAttachment=0,boltAttachment=0;
    std::uint64_t weaponKey=0,toolKey=0,boltKey=0;
    unsigned phase=0,reason=0,flags=0,channel=0;int classId=-1,side=-1,childStatus=-1,progressBucket=-1;
    float progress=0,clock=0,spawn=-1,ammo=-1,cock=-1,load=-1;
};
// flags: VALID=1, EXACT_OWNER_JOIN=2, BINDING_QUALIFIED=4, TOOL_BOUND=8,
// TOOL_VISIBLE=16, BOLT_IN_HAND=32, BOLT_PLACED=64, NATIVE_COCKED=128,
// NATIVE_LOADED=256, NOCK_BOUND=512, TOOL_POSE=1024, BOLT_POSE=2048,
// TOOL_WEAPON_BOUND=4096, TOOL_PENDING=8192, CLOCK_OWNED=16384,
// TOOL_TAKEN=32768, TOOL_ENGAGED=65536, BOLT_TAKEN=131072.
class Recorder {
    std::atomic_flag lock=ATOMIC_FLAG_INIT;
    std::array<ReloadEvent,128> queue{};
    std::array<ReloadEvent,2> previous{};
    unsigned head=0,tail=0;
    std::array<unsigned,cap::DomainCount> used{};std::array<std::uint64_t,cap::DomainCount> budgetSession{};
public:
    std::atomic<std::uint64_t> dropped{0},coalesced{0},budgetLost{0},expired{0};
    bool offer(ReloadEvent e,std::uint64_t now) noexcept {
        if(!e.tick||e.tick>now||e.channel>1)return false;
        // A prepared event keeps its original lease even if another recorder
        // becomes preferred between copy and queue. Never relabel old data.
        const auto ticket=e.session?capture::Ticket{e.domain,e.session,e.activeMask}:capture::select(now);
        if(!capture::current(ticket,now))return false;
        e.domain=ticket.domain;e.session=ticket.session;e.activeMask=ticket.activeMask;
        const auto session=e.session;const auto index=unsigned(e.domain);
        if(lock.test_and_set(std::memory_order_acquire)){++dropped;return false;}
        if(budgetSession[index]!=session){budgetSession[index]=session;used[index]=0;}
        auto& p=previous[e.channel];
        const bool same=p.domain==e.domain&&p.activeMask==e.activeMask&&p.session==session&&p.serial==e.serial&&p.request==e.request&&p.player==e.player&&
            p.parent==e.parent&&p.child==e.child&&p.generation==e.generation&&p.weaponKey==e.weaponKey&&
            p.phase==e.phase&&p.reason==e.reason&&p.flags==e.flags&&p.classId==e.classId&&p.side==e.side&&
            p.childStatus==e.childStatus&&p.toolKey==e.toolKey&&p.boltKey==e.boltKey&&
            p.toolEntity==e.toolEntity&&p.boltEntity==e.boltEntity&&p.toolAttachment==e.toolAttachment&&p.boltAttachment==e.boltAttachment;
        // Identity/binding/semantic edges are immediate. Progress-only samples
        // are 10% buckets, at most 4 Hz. No periodic unchanged heartbeat.
        if(same&&(e.progressBucket==p.progressBucket||now<p.tick||now-p.tick<250)){++coalesced;lock.clear(std::memory_order_release);return false;}
        const auto next=(head+1)%queue.size();bool ok=false;
        if(used[index]>=2048)++budgetLost;else if(next==tail)++dropped;
        else {queue[head]=e;head=unsigned(next);p=e;++used[index];ok=true;}
        lock.clear(std::memory_order_release);return ok;
    }
    bool pop(ReloadEvent& e) noexcept {
        if(lock.test_and_set(std::memory_order_acquire))return false;
        const bool ok=head!=tail;if(ok){e=queue[tail];tail=unsigned((tail+1)%queue.size());}
        lock.clear(std::memory_order_release);return ok;
    }
};
inline Recorder recorder;
template<class Status,class Packet>
inline void observeReload(const Status& s,const Packet& p,unsigned channel,std::uint64_t now) noexcept {
    const auto ticket=capture::select(now);if(!ticket.session)return;
    ReloadEvent e{};e.domain=ticket.domain;e.session=ticket.session;e.activeMask=ticket.activeMask;const auto& o=s.owner;e.tick=now;e.channel=channel;e.serial=o.serial;
    e.request=o.source.request;e.player=o.source.player;e.parent=o.parent;e.child=o.child;
    e.generation=o.source.generation;e.weaponKey=o.source.key;e.classId=o.source.classId;e.side=o.source.primary>=0&&o.source.primary<2?1-o.source.primary:-1;
    e.phase=unsigned(s.phase);e.reason=unsigned(s.reason);e.progress=s.progress;
    e.progressBucket=std::isfinite(s.progress)?int(std::fmax(0.f,std::fmin(1.f,s.progress))*10.f):-1;
    e.publication=p.publication;e.childStatus=p.childStatus;
    // ADL reuses the production XR34 full owner/source equality, including
    // player, hand, class, controller/context/token and ammo identity.
    const bool joined=p.valid&&p.binding.qualified&&p.binding.serial==o.serial&&sameOwner(o,p.owner);
    e.flags=(p.valid?1u:0u)|(joined?2u:0u)|(s.clockOwned?16384u:0u)|
        (s.toolTakenD72?32768u:0u)|(s.toolEngagedD72?65536u:0u)|(s.boltTakenD72?131072u:0u);
    // Invalid/mismatched packets never supply a purported tool/bolt result.
    if(joined){const auto& b=p.binding;e.sourceTick=b.tick;e.sample=b.sample;
        e.flags|=(b.qualified?4u:0u)|(b.toolBound?8u:0u)|(b.toolVisible?16u:0u)|(b.handBoltBound?32u:0u)|
            (b.handBoltPlaced?64u:0u)|(b.nativeCocked?128u:0u)|(b.nativeLoaded?256u:0u)|(b.nockBound?512u:0u)|
            (b.toolPoseReady?1024u:0u)|(b.boltPoseReady?2048u:0u)|(b.toolWeaponBoundD72?4096u:0u)|(b.toolPendingD72?8192u:0u);
        e.toolEntity=b.toolEntity;e.boltEntity=b.boltEntity;e.toolAttachment=b.toolAttachment;e.boltAttachment=b.boltAttachment;
        e.toolKey=b.toolKey;e.boltKey=b.boltKey;e.clock=p.clock.normalized;
        e.spawn=p.clip.spawn;e.ammo=p.clip.ammo;e.cock=p.clip.cock;e.load=p.clip.load;
    }
    (void)recorder.offer(e,now);
}
template<class Sink> inline void drain(std::uint64_t now,Sink sink) noexcept {
    ReloadEvent e{};unsigned budget=128;
    while(budget--&&recorder.pop(e)){
        if(!capture::receipt({e.domain,e.session,e.activeMask},now)){++recorder.expired;continue;}
        char row[1600]{};
        std::snprintf(row,sizeof(row),
            "D77 F11_BINDING session=%llu tick=%llu channel=%s serial=%llu request=%llu player=%llX parent=%llX child=%llX generation=%llu weapon=%llX class=%d workingSide=%d phase=%u reason=%u childStatus=%d flags=0x%X progress=%.3f publication=%llu sample=%llu sourceAgeMs=%lld toolKey=%llX toolEntity=%llX toolAttachment=%llX boltKey=%llX boltEntity=%llX boltAttachment=%llX clock=%.4f markers=%.4f,%.4f,%.4f,%.4f join=%s nativeCocked=%d nativeLoaded=%d actualRenderedPose=NOT_OBSERVED fireOrInventory=NOT_INFERRED extraNativeReads=0 queueLost=%llu budgetLost=%llu recorder=%s captureMask=0x%X",
            (unsigned long long)e.session,(unsigned long long)e.tick,e.channel?"FIRST_POSE_ADMISSION":"POST_UPDATE_PASSENGER",
            (unsigned long long)e.serial,(unsigned long long)e.request,(unsigned long long)e.player,(unsigned long long)e.parent,(unsigned long long)e.child,
            (unsigned long long)e.generation,(unsigned long long)e.weaponKey,e.classId,e.side,e.phase,e.reason,e.childStatus,e.flags,double(e.progress),
            (unsigned long long)e.publication,(unsigned long long)e.sample,e.sourceTick&&e.tick>=e.sourceTick?static_cast<long long>(e.tick-e.sourceTick):-1LL,
            (unsigned long long)e.toolKey,(unsigned long long)e.toolEntity,(unsigned long long)e.toolAttachment,
            (unsigned long long)e.boltKey,(unsigned long long)e.boltEntity,(unsigned long long)e.boltAttachment,double(e.clock),double(e.spawn),double(e.ammo),double(e.cock),double(e.load),
            e.flags&2u?"EXACT_RELOAD_OWNER":"UNAVAILABLE",e.flags&2u?int((e.flags&128u)!=0):-1,e.flags&2u?int((e.flags&256u)!=0):-1,
            (unsigned long long)recorder.dropped.load(std::memory_order_relaxed),(unsigned long long)recorder.budgetLost.load(std::memory_order_relaxed),cap::name(e.domain),e.activeMask);
        capture::emit(sink,{e.domain,e.session,e.activeMask},row);
    }
}
}
