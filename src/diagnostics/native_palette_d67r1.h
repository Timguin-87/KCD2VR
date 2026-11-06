#pragma once
#include "motion_trace_d68.h"
#include "../retail_build112.h"
#include <array>
#include <atomic>
#include <cstdio>
#include <cstdint>
#include <limits>

namespace kcd2vr::diagnostics::native_palette_d67r1 {
// Values copied while the exact Henry skinning callback owns its arguments.
// The worker never dereferences these addresses. Pool IDs come from native
// GetSkinningData's three-slot cache, NOT an XR/eye/present frame counter.
struct Slot {
    std::uintptr_t data=0,output=0,previousData=0;
    std::int32_t pool=-1;bool readable=false;
    std::uintptr_t transfer=0; // native bulk-palette transfer entry, not a GPU resource
};
enum class Match : unsigned {Unknown,Unique,SelfPrevious,PreviousOutsideRing,Ambiguous};
struct Ring {
    Match match=Match::Unknown;int slot=-1,previousSlot=-1;
    std::int32_t pool=-1,previousPool=-1;
    std::uintptr_t data=0,previousData=0,transfer=0;
};
// Equality requires values from the same captured native ring. An address
// reused in multiple slots must not acquire whichever pool counter is newest.
inline Ring identify(const std::array<Slot,3>& slots,std::uintptr_t output,std::uintptr_t previousOutput) noexcept {
    Ring r{};if(!output||!previousOutput)return r;unsigned matches=0;
    for(unsigned i=0;i<slots.size();++i)if(slots[i].readable&&slots[i].data&&slots[i].output==output){
        ++matches;r.slot=int(i);r.pool=slots[i].pool;r.data=slots[i].data;r.previousData=slots[i].previousData;r.transfer=slots[i].transfer;
    }
    if(matches!=1){r={};if(matches>1)r.match=Match::Ambiguous;return r;}
    if(r.previousData==r.data&&previousOutput==output){
        r.match=Match::SelfPrevious;r.previousSlot=r.slot;r.previousPool=r.pool;return r;
    }
    unsigned previous=0;
    for(unsigned i=0;i<slots.size();++i){const auto& s=slots[i];
        if(s.readable&&s.data==r.previousData&&s.output==previousOutput&&
           std::int64_t(s.pool)+1==std::int64_t(r.pool)){
            ++previous;r.previousSlot=int(i);r.previousPool=s.pool;
        }
    }
    if(previous==1)r.match=Match::Unique;
    else if(previous>1){r={};r.match=Match::Ambiguous;}
    else {r.match=Match::PreviousOutsideRing;r.previousSlot=-1;r.previousPool=-1;}
    return r;
}
inline bool same(const Ring& a,const Ring& b) noexcept {
    return a.match==b.match&&a.slot==b.slot&&a.previousSlot==b.previousSlot&&a.pool==b.pool&&
        a.previousPool==b.previousPool&&a.data==b.data&&a.previousData==b.previousData&&a.transfer==b.transfer;
}
inline std::uint64_t hash32(const std::array<unsigned char,32>& bytes) noexcept {
    std::uint64_t h=14695981039346656037ull;for(auto b:bytes){h^=b;h*=1099511628211ull;}return h;
}
enum Flags : unsigned {
    Abi=1u,IdentityPre=2u,DescriptorPre=4u,RingPreStable=8u,IdentityPost=16u,
    DescriptorPost=32u,RingPostStable=64u,RingUnchanged=128u,NativeReturned=256u,
    ScopedPose=512u,PairedPublication=1024u,PublicationCurrent=2048u
};
struct Record {
    std::uint64_t session=0,cycle=0,serial=0,tick=0,sample=0,publication=0,epoch=0,trackingEpoch=0,sourceNs=0;
    std::int64_t locateTarget=0;
    std::uint64_t beforeNativeNs=0,afterNativeNs=0,readNs=0,finishNs=0;
    std::uintptr_t character=0,pose=0,skeleton=0,descriptor=0,output=0,previousOutput=0;
    Ring before{},after{};
    std::uint64_t hashes[2][2][2]{}; // phase(pre/post), output(current/previous), side(R/L)
    unsigned masks[2]{}; // bit (output * 2 + side); zero hash is not a failed read
    unsigned flags=0,thread=0;int wrists[2]{-1,-1};
};
// The existing pose mutex serializes admission. Never used to infer display
// cadence: omitted entries and queue drops are explicit in the health receipt.
struct Budget {
    std::uint64_t session=0,second=0;unsigned count=0;
    bool take(std::uint64_t serial,std::uint64_t now) noexcept {
        if(!serial||!now)return false;
        if(session!=serial||second!=now/1000){session=serial;second=now/1000;count=0;}
        if(count>=240)return false;
        ++count;return true;
    }
};
// Match the D68 capture clock; native GetTickCount64 remains a separate identity clock.
inline std::uint64_t clockNs() noexcept {return perf_d61::nowNs();}
inline bool current(std::uint64_t session) noexcept {return motion_d68::current(session);}
inline std::array<Record,256> queue{};inline unsigned head=0,tail=0;
inline std::atomic_flag lock=ATOMIC_FLAG_INIT;
inline std::atomic<std::uint64_t> dropped{0},suppressed{0},ordinal{0},expired{0},formatDropped{0};
inline bool push(const Record& r) noexcept {
    if(!current(r.session))return false;
    if(lock.test_and_set(std::memory_order_acquire)){dropped.fetch_add(1,std::memory_order_relaxed);return false;}
    const auto next=(head+1)%queue.size();
    if(next==tail){lock.clear(std::memory_order_release);dropped.fetch_add(1,std::memory_order_relaxed);return false;}
    queue[head]=r;head=unsigned(next);lock.clear(std::memory_order_release);return true;
}
inline bool pop(Record& r) noexcept {
    if(lock.test_and_set(std::memory_order_acquire))return false;
    const bool have=head!=tail;if(have){r=queue[tail];tail=unsigned((tail+1)%queue.size());}
    lock.clear(std::memory_order_release);return have;
}
template<class Writer> inline void drain(std::uint64_t now,Writer&& write) noexcept {
    Record r{};unsigned budget=32;
    while(budget--&&pop(r)){
        if(!perf_d61::gate.receipt(r.session,now)){expired.fetch_add(1,std::memory_order_relaxed);continue;}
        char line[1800]{};
        const int n=std::snprintf(line,sizeof(line),
            "D68_MOTION session=%llu stage=PALETTE_LINEAGE cycle=%llu serial=%llu tick=%llu thread=%u sample=%llu publication=%llu poseEpoch=%llu trackingEpoch=%llu sourceNs=%llu targetXrNs=%lld ns=%llu endNs=%llu finishNs=%llu readNs=%llu actor=%llX pose=%llX skeleton=%llX descriptor=%llX output=%llX previousOutput=%llX flags=%X masks=%X/%X ring=%u/%u slots=%d/%d pools=%d/%d afterPools=%d/%d data=%llX previousData=%llX transfer=%llX/%llX wrists=%d/%d currentR=%016llX/%016llX currentL=%016llX/%016llX previousR=%016llX/%016llX previousL=%016llX/%016llX outputFormat=OPAQUE_32_BYTES poolCounter=NOT_XR_FRAME meshSubmission=UNOBSERVED",
            (unsigned long long)r.session,(unsigned long long)r.cycle,(unsigned long long)r.serial,(unsigned long long)r.tick,r.thread,
            (unsigned long long)r.sample,(unsigned long long)r.publication,(unsigned long long)r.epoch,
            (unsigned long long)r.trackingEpoch,(unsigned long long)r.sourceNs,(long long)r.locateTarget,
            (unsigned long long)r.beforeNativeNs,(unsigned long long)r.afterNativeNs,(unsigned long long)r.finishNs,(unsigned long long)r.readNs,
            (unsigned long long)r.character,(unsigned long long)r.pose,(unsigned long long)r.skeleton,
            (unsigned long long)r.descriptor,(unsigned long long)r.output,(unsigned long long)r.previousOutput,
            r.flags,r.masks[0],r.masks[1],unsigned(r.before.match),unsigned(r.after.match),r.before.slot,r.before.previousSlot,
            r.before.pool,r.before.previousPool,r.after.pool,r.after.previousPool,
            (unsigned long long)r.before.data,(unsigned long long)r.before.previousData,
            (unsigned long long)r.before.transfer,(unsigned long long)r.after.transfer,r.wrists[0],r.wrists[1],
            (unsigned long long)r.hashes[0][0][0],(unsigned long long)r.hashes[1][0][0],
            (unsigned long long)r.hashes[0][0][1],(unsigned long long)r.hashes[1][0][1],
            (unsigned long long)r.hashes[0][1][0],(unsigned long long)r.hashes[1][1][0],
            (unsigned long long)r.hashes[0][1][1],(unsigned long long)r.hashes[1][1][1]);
        if(n>0&&std::size_t(n)<sizeof(line))write(line);else formatDropped.fetch_add(1,std::memory_order_relaxed);
    }
    static std::uint64_t last=0,session=0;
    const auto currentSession=perf_d61::gate.serial.load(std::memory_order_acquire);
    if(perf_d61::gate.current(currentSession,now)&&(session!=currentSession||!last||now<last||now-last>=1000)){
        session=currentSession;last=now;char line[1000]{};
        std::snprintf(line,sizeof(line),
            "D68_MOTION session=%llu stage=PALETTE_HEALTH family=LINEAGE observed=%llu suppressed=%llu queueDropped=%llu expired=%llu formatDropped=%llu capture=SHIFT_F2_CORE_BURST maxPerSecond=240 queueCapacity=255 drainBudget=32 ringMeaning=UNKNOWN:0,EXACT_PREVIOUS:1,SELF_PREVIOUS:2,PREVIOUS_OUTSIDE_RING:3,AMBIGUOUS:4 flags=ABI:1,ID_PRE:2,DESC_PRE:4,RING_PRE:8,ID_POST:10,DESC_POST:20,RING_POST:40,RING_SAME:80,NATIVE_RETURN:100,SCOPED:200,PAIRED:400,CURRENT:800 masks=CUR_R:1,CUR_L:2,PREV_R:4,PREV_L:8 idsHex=FLAGS_MASKS hashes=PRE_POST typedPaletteDecode=NONE finalDraw=UNOBSERVED",
            (unsigned long long)session,(unsigned long long)ordinal.load(std::memory_order_relaxed),
            (unsigned long long)suppressed.load(std::memory_order_relaxed),(unsigned long long)dropped.load(std::memory_order_relaxed),
            (unsigned long long)expired.load(std::memory_order_relaxed),(unsigned long long)formatDropped.load(std::memory_order_relaxed));
        write(line);
    }
}
}
