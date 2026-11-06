#pragma once
// Optional HJ1 prediction experiment. One complete pose family per verified
// native gameplay frame. Camera notes only measure prediction; they never
// renew the cache. Unassociated workers retain the normal tracking family.
#include <atomic>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <mutex>
namespace kcd2vr::hj1 {
inline std::atomic<bool> enabled{false}; // Selected explicitly by the HJ1 comparison rung only.
constexpr std::int64_t kMaxLeadNs=50000000;   // camera apply later than the hand read by more = not the same frame
constexpr std::int64_t kCacheNs=25000000; // stale sample falls back; never resample within the same native frame
// pure rule: how many xrWaitFrame returns will happen between now and the camera apply.
inline int Ahead(std::int64_t phaseNs,std::int64_t leadNs,std::int64_t periodNs) noexcept {
    if(periodNs<=0)return 0;if(phaseNs<0)phaseNs=0;if(leadNs<0)leadNs=0;
    const std::int64_t k=(phaseNs+leadNs)/periodNs;return k<0?0:k>2?2:int(k);
}
inline std::int64_t Target(std::int64_t base,int ahead,std::int64_t periodNs) noexcept {return base+std::int64_t(ahead)*periodNs;}
// lead EMA (alpha 0.1). samples outside 0..50 ms are ignored. first accepted sample seeds it.
inline std::int64_t UpdateLead(std::int64_t prev,bool havePrev,std::int64_t sample) noexcept {
    if(sample<0||sample>kMaxLeadNs)return prev;
    if(!havePrev)return sample;
    return prev+(sample-prev)/10;
}
struct Pose7 {float p[3]{};float q[4]{0,0,0,1};};
struct Sample {
    bool ok=false;bool valid[2]{},tracked[2]{};Pose7 hand[2]{}; // LEFT, RIGHT
    bool headValid=false,headTracked=false;Pose7 head{};
    std::uint64_t sourceEpoch=0,tick=0,sequence=0,nativeFrame=0;
    std::uint64_t comparisonEpoch=0,cameraGeneration=0; // Existing F2 policy and camera-origin generations.
    std::int64_t target=0,readXrNow=0;int ahead=0;std::uint64_t steadyNs=0;bool cameraNoted=false;
    std::uint64_t cameraFrame=0; // W3 (Codex review): the camera pose frame that consumed this sample, 0 = not yet
    std::int64_t handTargetD69[2]{};unsigned handLeadSourceD69[2]{};std::uint64_t handPolicyD69=0;

};
using LocateFn=bool(*)(Sample&) noexcept; // Complete head + grip family, one XR target.
using NativeFrameFn=std::uint64_t(*)() noexcept;
inline std::atomic<NativeFrameFn> nativeFrame{nullptr};
inline std::atomic<LocateFn> locate{nullptr};
inline std::atomic<std::int64_t> leadNs{0};inline std::atomic<bool> leadKnown{false};
inline std::atomic<std::uint64_t> matched{0},early{0},late{0},fallback{0},unnoted{0},ahead0{0},ahead1{0},ahead2{0};
// W3 frame identity: samples made between two camera notes. >1 = readers of one game frame saw different hands.
inline std::atomic<std::uint64_t> samplesSinceNote{0},splitFrames{0},cameraNotes{0},lastCameraFrame{0};
inline std::atomic<std::uint64_t> outsideScope{0},used{0},lastNativeFrame{0},lastSourceEpoch{0};
// Thread-local: an unassociated worker cannot borrow another thread's frame.
inline thread_local Sample cache{};
// Private proposal: no native hook uses this transport yet. The caller must
// qualify the retail manager/callback/descriptor. A tagged carrier registers
// every dispatched tagged incarnation; untagged callbacks mask UNKNOWN without
// registry lookup. An observer-only adapter instead must record unknown submits
// too. Keys contain values only;
// they never retain or dereference native arena, batch or heap-state storage.
namespace jobs {
struct Key {
    std::array<std::uintptr_t,7> payload{}; // exact copied 0x38-byte payload
    std::uintptr_t handle=0,state=0;
};
inline bool Same(const Key& a,const Key& b) noexcept {return a.payload==b.payload&&a.handle==b.handle&&a.state==b.state;}
using ValidateFn=bool(*)(const Sample&) noexcept; // Reads current existing policy/source/origin owners only.
struct Reservation {std::uint64_t serial=0;std::size_t slot=0;bool reserved=false;};
struct Claim {
    Sample family{};ValidateFn validate=nullptr;std::uint64_t serial=0;std::size_t slot=0;
    const std::atomic<bool>* fault=nullptr;bool counted=false;
};
template<std::size_t Capacity> class Registry {
    static_assert(Capacity>0);
    // Bounded set-associative lookup: at most eight exact-key comparisons,
    // with independent locks. A full bucket fails closed, never evicts a job.
    static constexpr std::size_t kWays=Capacity<8?Capacity:8;
    static constexpr std::size_t kBuckets=(Capacity+kWays-1)/kWays;
    struct Entry {Key key{};Sample family{};std::uint64_t serial=0;std::uint32_t pending=0,active=0;bool poison=false;};
    struct Bucket {std::array<Entry,kWays> entries{};std::mutex mutex;std::uint64_t serial=0;};
    std::array<Bucket,kBuckets> buckets_{};
    std::atomic<bool> fault_{false};
    static std::size_t BucketFor(const Key& key) noexcept {
        std::uint64_t hash=14695981039346656037ull;
        for(auto word:key.payload){hash^=std::uint64_t(word);hash*=1099511628211ull;}
        hash^=std::uint64_t(key.handle);hash*=1099511628211ull;
        hash^=std::uint64_t(key.state);hash*=1099511628211ull;
        hash^=hash>>33;hash*=0xff51afd7ed558ccdull;
        hash^=hash>>33;hash*=0xc4ceb9fe1a85ec53ull;hash^=hash>>33;
        return std::size_t(hash%kBuckets);
    }
    static constexpr std::size_t Ways(std::size_t bucket) noexcept {
        const auto remaining=Capacity-bucket*kWays;return remaining<kWays?remaining:kWays;
    }
public:
    Registry()=default;Registry(const Registry&)=delete;Registry& operator=(const Registry&)=delete;
    bool Failed() const noexcept {return fault_.load(std::memory_order_acquire);}
    // Missing an exact submit cannot be repaired by borrowing a later family.
    // This failure persists for the registry lifetime; there is no mode reset.
    void Unrecordable() noexcept {fault_.store(true,std::memory_order_release);}
    bool Submit(const Key& key,const Sample& family,Reservation* reservation=nullptr) noexcept {
        if(reservation)*reservation={};
        if(Failed())return false;
        const auto index=BucketFor(key);auto& bucket=buckets_[index];
        std::lock_guard<std::mutex> lock(bucket.mutex);if(Failed())return false;
        Entry* empty=nullptr;
        for(std::size_t i=0;i<Ways(index);++i){auto& e=bucket.entries[i];
            if(!e.pending){if(!empty)empty=&e;continue;}
            if(!Same(e.key,key))continue;
            e.poison=true;e.family={};
            if(e.pending==UINT32_MAX){Unrecordable();return false;}
            ++e.pending;
            if(reservation)*reservation={e.serial,index*kWays+i,true};
            return true; // count every queued or active incarnation
        }
        if(!empty||bucket.serial==UINT64_MAX){Unrecordable();return false;}
        *empty={};empty->key=key;empty->family=family;empty->serial=++bucket.serial;empty->pending=1;
        if(reservation)*reservation={empty->serial,index*kWays+std::size_t(empty-bucket.entries.data()),true};
        return true;
    }
    // Submit may throw AFTER enqueue. Never erase a pending reservation or touch
    // a reused slot/newer serial; synchronous completion may already have retired it.
    // An already admitted active Claim keeps its copied family (as on duplicate
    // Submit); poisoning affects future claims, not past unambiguous admission.
    Claim Enter(const Key& key) noexcept {
        Claim out{};out.fault=&fault_;
        if(Failed())return out;
        const auto index=BucketFor(key);auto& bucket=buckets_[index];
        std::lock_guard<std::mutex> lock(bucket.mutex);
        if(Failed())return out;
        for(std::size_t i=0;i<Ways(index);++i){auto& e=bucket.entries[i];
            if(!e.pending||!Same(e.key,key))continue;
            if(e.active>=e.pending){e.poison=true;e.family={};Unrecordable();return out;}
            ++e.active;out.slot=index*kWays+i;out.serial=e.serial;out.counted=true;
            if(!e.poison)out.family=e.family;
            return out;
        }
        return out; // unknown callback explicitly masks an outer valid family
    }
    void Leave(Claim& claim) noexcept {
        if(!claim.counted)return;
        if(claim.slot>=Capacity){Unrecordable();claim.counted=false;return;}
        auto& bucket=buckets_[claim.slot/kWays];
        std::lock_guard<std::mutex> lock(bucket.mutex);
        auto& e=bucket.entries[claim.slot%kWays];
        if(e.serial!=claim.serial||!e.pending||!e.active){Unrecordable();claim.counted=false;return;}
        --e.active;--e.pending;if(!e.pending)e={};claim.counted=false;
    }
};
struct ScopeValue {Sample family{};ValidateFn validate=nullptr;const std::atomic<bool>* fault=nullptr;std::uint64_t serial=0;bool active=false;};
struct ScopeMark {ScopeValue previous{};std::uint64_t entered=0;};
inline thread_local ScopeValue scope{};
inline thread_local std::uint64_t scopeSerial=0;
// Native integration must call End and Leave in __finally, after original
// callback returns. No registry lock spans that original invocation. Unknown
// nested callbacks install an unknown scope instead of inheriting outer data.
inline ScopeMark Begin(const Claim& claim) noexcept {
    ScopeMark mark{scope,++scopeSerial};scope={claim.family,claim.validate,claim.fault,mark.entered,true};
    if(!mark.entered)scope.family={};return mark;
}
inline void End(const ScopeMark& mark) noexcept {
    if(scope.serial==mark.entered)scope=mark.previous;
    else scope={Sample{},nullptr,nullptr,0,true}; // malformed unwind must not reuse a family
}
inline bool Read(Sample& out,std::uint64_t now) noexcept {
    if(!scope.active||(scope.fault&&scope.fault->load(std::memory_order_acquire))||
       (scope.validate&&!scope.validate(scope.family))||!scope.family.ok||!scope.family.steadyNs||now<scope.family.steadyNs||
       now-scope.family.steadyNs>std::uint64_t(kCacheNs))return false;
    out=scope.family;return true;
}
} // namespace jobs
inline std::uint64_t SteadyNs() noexcept {
    return std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}
// Game-side read. Failure keeps the normal complete family.
inline bool Current(Sample& out) noexcept {
    if(!enabled.load(std::memory_order_acquire))return false;
    if(jobs::scope.active)return jobs::Read(out,SteadyNs());
    const auto frameFn=nativeFrame.load(std::memory_order_acquire);
    const auto frame=frameFn?frameFn():0;if(!frame){outsideScope.fetch_add(1,std::memory_order_relaxed);return false;}
    const auto fn=locate.load(std::memory_order_acquire);if(!fn)return false;
    const auto now=SteadyNs();
    if(cache.nativeFrame!=frame){
        Sample s{};s.steadyNs=now;s.nativeFrame=frame;
        if(!fn(s)||!s.ok){s.ok=false;fallback.fetch_add(1,std::memory_order_relaxed);}
        else (s.ahead==0?ahead0:s.ahead==1?ahead1:ahead2).fetch_add(1,std::memory_order_relaxed);
        if(cache.ok&&!cache.cameraNoted)unnoted.fetch_add(1,std::memory_order_relaxed);
        samplesSinceNote.store(1,std::memory_order_relaxed);
        cache=s;
    }
    const auto validatedNow=SteadyNs(); // LocateFn stamps completion after the pre-locate clock read.
    if(!cache.steadyNs||validatedNow<cache.steadyNs||validatedNow-cache.steadyNs>std::uint64_t(kCacheNs))return false;
    out=cache;return out.ok;
}
// Keep the existing input permission/edge age, validity, held flags and last-good
// ages. Estimated/invalid late samples fall back as a WHOLE family; they do not
// introduce a new gameplay tracking veto or extend the existing tracking grace.
template<class S> bool ApplyCompleteFamily(S& out,const Sample& s) noexcept {
    const auto validPose=[](const Pose7& p){
        const double n=double(p.q[0])*p.q[0]+double(p.q[1])*p.q[1]+double(p.q[2])*p.q[2]+double(p.q[3])*p.q[3];
        return std::isfinite(p.p[0])&&std::isfinite(p.p[1])&&std::isfinite(p.p[2])&&std::isfinite(n)&&n>.90&&n<1.10;
    };
    if(!s.ok||!s.sourceEpoch||s.sourceEpoch!=out.source_epoch||!s.tick||!s.sequence||!s.steadyNs||s.target<=0||
       !s.headValid||!s.headTracked||!s.valid[0]||!s.tracked[0]||!s.valid[1]||!s.tracked[1]||!out.head_valid||
       !validPose(s.head)||!validPose(s.hand[0])||!validPose(s.hand[1]))return false;
    out.head={s.head.p[0],s.head.p[1],s.head.p[2]};out.hq={s.head.q[0],s.head.q[1],s.head.q[2],s.head.q[3]};
    out.left={s.hand[0].p[0],s.hand[0].p[1],s.hand[0].p[2]};out.lq={s.hand[0].q[0],s.hand[0].q[1],s.hand[0].q[2],s.hand[0].q[3]};
    out.right={s.hand[1].p[0],s.hand[1].p[1],s.hand[1].p[2]};out.rq={s.hand[1].q[0],s.hand[1].q[1],s.hand[1].q[2],s.hand[1].q[3]};
    out.sequence=s.sequence;out.tick=s.tick;out.timeNs=s.steadyNs;out.poseTime=s.target;
    if constexpr(requires{out.handPolicyD69;out.handTargetD69[0];out.handLeadSourceD69[0];}){
        out.handPolicyD69=s.handPolicyD69;for(unsigned h=0;h<2;++h){out.handTargetD69[h]=s.handTargetD69[h];out.handLeadSourceD69[h]=s.handLeadSourceD69[h];}}
    used.fetch_add(1,std::memory_order_relaxed);lastNativeFrame.store(s.nativeFrame,std::memory_order_relaxed);lastSourceEpoch.store(s.sourceEpoch,std::memory_order_relaxed);
    return true;
}
// render camera apply: the head target it really used. proof receipt + lead learning. one note per cached sample.
inline void NoteCamera(std::int64_t cameraTarget,std::int64_t cameraXrNow,std::int64_t periodNs,std::uint64_t cameraFrame=0) noexcept {
    if(!enabled.load(std::memory_order_acquire))return;
    const auto frameFn=nativeFrame.load(std::memory_order_acquire);
    const auto frame=frameFn?frameFn():0;if(!frame||cache.nativeFrame!=frame)return;
    if(!cache.ok||cache.cameraNoted||!cameraTarget||!cache.readXrNow)return;
    if(samplesSinceNote.exchange(0,std::memory_order_relaxed)>1)splitFrames.fetch_add(1,std::memory_order_relaxed);
    cameraNotes.fetch_add(1,std::memory_order_relaxed);cache.cameraFrame=cameraFrame;lastCameraFrame.store(cameraFrame,std::memory_order_relaxed);
    cache.cameraNoted=true;
    const auto sample=cameraXrNow-cache.readXrNow;if(sample<0||sample>kMaxLeadNs)return;
    const bool known=leadKnown.load(std::memory_order_relaxed);
    leadNs.store(UpdateLead(leadNs.load(std::memory_order_relaxed),known,sample),std::memory_order_relaxed);leadKnown.store(true,std::memory_order_relaxed);
    const auto d=cache.target-cameraTarget,half=periodNs>0?periodNs/2:1000000; // runtime display times are not always exact multiples
    (d>-half&&d<half?matched:d<0?early:late).fetch_add(1,std::memory_order_relaxed);
}
} // namespace kcd2vr::hj1
