#pragma once
// A completed head/grip pose family, separate from action/button synchronization.
// No rendering, native mutation, input edge generation, or extrapolation here.
#include "core/snapshot_mailbox.h"
#include "diagnostics/motion_trace_d68.h"
#include "vrik/hand_same_frame_hj1.h" // W3: the smooth lab hand sample, read by subsystems/vrik/tracking_reader.inl
#include "vrik/motion_trials_d67.h"
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
namespace kcd2vr::tracking59c6 {
struct Pose { float x=0,y=0,z=0,qx=0,qy=0,qz=0,qw=1; };
struct Stamp { std::uint64_t tick=0,ns=0,epoch=0;std::int64_t locateTime=0; };
struct Frame {
    // Order is HEAD, LEFT, RIGHT (not the physical kernel's right/left order).
    std::array<Pose,3> poses{};
    std::array<bool,3> valid{},tracked{},held{};
    // Fresh runtime-estimated hand behind the HMD-facing plane. This is an
    // explicit position-dependent continuation, never a TRACKED measurement.
    std::array<bool,3> estimatedBehindHead{};
    std::array<std::uint64_t,3> goodTick{};
    std::uint64_t sequence=0,epoch=0,tick=0,ns=0;
    std::int64_t locateTime=0;
    std::int64_t handTargetD69[2]{}; // LEFT, RIGHT; zero means original locateTime
    unsigned handLeadSourceD69[2]{}; // 0 baseline, 1 runtime velocity, 2 future locate
    std::uint64_t handPolicyD69=0;
    // Independent pre-update hands. The worker/camera lane keeps its own
    // history and can never replace this sample with a different time target.
    // The head member is a coordinate reference for these hands, not a camera publication.
    std::uint64_t refreshGenerationR94=0;
    bool controllersSampled=false;
};
inline std::atomic<bool> enabled{false},requested{true},cameraNow{false};
inline std::atomic<int> cameraSource{0};
inline std::atomic<std::uint64_t> epoch{1},serial{0},published{0},consumed{0},rejected{0},estimated{0}; // R59.16C7A: estimated = hand valid but not camera-tracked
inline core::SnapshotMailbox<Frame,4> frames,heads;
inline core::SnapshotMailbox<Frame,4> handFramesR94;
inline std::atomic<std::uint64_t> activeHandGenerationR94{0};
inline std::uint64_t NowNs() noexcept {
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
}
inline std::uint64_t Next() noexcept {return serial.fetch_add(2,std::memory_order_relaxed)+2;}
inline bool Valid(const Pose& p) noexcept {
    const double n=double(p.qx)*p.qx+double(p.qy)*p.qy+double(p.qz)*p.qz+double(p.qw)*p.qw;
    return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z)&&std::isfinite(n)&&n>.90&&n<1.10;
}
inline bool Age(std::uint64_t source,std::uint64_t now,std::uint64_t max) noexcept {
    return source&&now>=source&&now-source<=max;
}
inline bool BehindHead(const Frame& f,unsigned i) noexcept {
    if(!i||i>=3||!f.valid[0]||!f.tracked[0]||!f.valid[i]||f.tracked[i]||
       !Valid(f.poses[0])||!Valid(f.poses[i]))return false;
    const auto& h=f.poses[0];const auto& p=f.poses[i];
    // OpenXR forward is -Z, rotated by the CURRENT HMD quaternion.
    const double x=h.qx,y=h.qy,z=h.qz,w=h.qw;
    const double n=x*x+y*y+z*z+w*w;
    const double fx=-2*(x*z+w*y)/n,fy=-2*(y*z-w*x)/n,fz=-1+2*(x*x+y*y)/n;
    return (double(p.x)-h.x)*fx+(double(p.y)-h.y)*fy+(double(p.z)-h.z)*fz<0;
}
inline void Reset() noexcept {epoch.fetch_add(1,std::memory_order_acq_rel);}
inline void UpdateSelection() noexcept {
    const int mode=cameraSource.load(std::memory_order_acquire);
    const bool wanted=requested.load(std::memory_order_acquire)&&mode>=0&&mode<=2;
    if(enabled.exchange(wanted,std::memory_order_acq_rel)!=wanted)Reset();
}
inline void SetCameraSource(int mode) noexcept {
    const int previous=cameraSource.exchange(mode,std::memory_order_acq_rel);
    cameraNow.store(mode==2,std::memory_order_release);
    if(previous!=mode)Reset();
    UpdateSelection();
}
inline void Configure(bool on) noexcept {requested.store(on,std::memory_order_release);UpdateSelection();}
// R96: the independent uncapped family is no longer a session latch. It keeps
// priority while it is CURRENT (same epoch + generation and younger than the
// 80 ms family age that Compose accepts anyway). A contended mailbox read still
// returns false (the next read succeeds). But a family that stopped publishing,
// or one left in an older tracking epoch after Reset(), no longer blocks the
// worker family forever: R94R1 KCD2VR1.log 17:44:15-17:45:28 rejected every
// pose (HEAD_TRACKING_INVALID / no hands) until restart because of that latch.
inline constexpr std::uint64_t kIndependentFamilyFreshNsR96=80000000ull;
template<class HandReader,class WorkerReader> inline bool ReadFamilyR94(Frame& f,
    std::uint64_t current,std::uint64_t generation,bool uncapped,HandReader&& hands,WorkerReader&& worker,
    std::uint64_t nowNs=0) noexcept {
    if(uncapped){
        Frame h{};
        const bool read=hands(h);
        const bool mine=read&&h.epoch==current&&h.refreshGenerationR94==generation;
        if(!nowNs)nowNs=NowNs();
        if(mine&&Age(h.ns,nowNs,kIndependentFamilyFreshNsR96)){f=h;return true;}
        const bool owned=activeHandGenerationR94.load(std::memory_order_acquire)==generation;
        // Transient contention: keep the original no-mix rule for this one read.
        if(owned&&!read)return false;
        // Old/dead independent family: use the worker family when it is from
        // the current epoch and NEWER than the independent sample (never an
        // older worker pose, never a renewed age).
        Frame w{};
        if(worker(w)&&w.epoch==current&&(!mine||w.ns>h.ns)){f=w;return true;}
        // Otherwise hand back the old independent sample so Compose
        // invalidates it by age exactly as before (never renewed).
        if(mine){f=h;return true;}
        return false;
    }
    return worker(f)&&f.epoch==current;
}
inline bool Read(Frame& f) noexcept {
    if(!enabled.load(std::memory_order_acquire))return false;
    return ReadFamilyR94(f,epoch.load(std::memory_order_acquire),
        motion_trials_d67::handRefreshGenerationR94.load(std::memory_order_acquire),
        motion_trials_d67::UncappedSelectedAt(0),
        [](Frame& out){return handFramesR94.read(out);},[](Frame& out){return frames.read(out);});
}
inline bool ReadHead(Frame& f) noexcept {return heads.read(f)&&f.epoch==epoch.load(std::memory_order_acquire);}
inline void PublishHead(const Frame& f) noexcept {
    // Invalid locates cannot renew last-good display age. The camera's separate
    // 20ms limit and the gameplay family's invalid/held flags still apply.
    if(f.epoch==epoch.load(std::memory_order_acquire)&&f.ns&&f.tick&&f.valid[0]&&Valid(f.poses[0]))(void)heads.publish(f);
}
// Serialized by the manager's pose-locate lock. Even a failed locate publishes
// its invalidity; only a busy caller skips a publication. Original good ages persist.
// The optional display receipt is sealed before this exact raw family becomes
// visible. The callback cannot change raw coordinates or their source clocks.
// Existing callers retain the same no-op publication path below.
template<class BeforePublish> inline bool PublishPrepared(Frame f,Frame* completed,BeforePublish&& beforePublish) noexcept {
    diagnostics::motion_d68::Event traceD68{};
    static thread_local diagnostics::motion_d68::Cadence cadenceD68;
    bool traceOnD68=diagnostics::motion_d68::begin(traceD68,diagnostics::motion_d68::InputPublished)&&diagnostics::motion_d68::inWindow(traceD68);
    if(traceOnD68&&!cadenceD68.due(traceD68.session,traceD68.ns)){diagnostics::motion_d68::noteCadenceSkip(diagnostics::motion_d68::InputPublished);traceOnD68=false;}
    if(traceOnD68){
        traceD68.sourceNs=f.ns;traceD68.sourceTick=f.tick;traceD68.trackingEpoch=f.epoch;traceD68.targetXrNs=f.locateTime;
        traceD68.leftTargetD69=f.handTargetD69[0];traceD68.rightTargetD69=f.handTargetD69[1];traceD68.handPolicyD69=f.handPolicyD69;traceD68.leftLeadD69=f.handLeadSourceD69[0];traceD68.rightLeadD69=f.handLeadSourceD69[1];
        for(unsigned i=0;i<3;++i){const auto&p=f.poses[i];const float values[]{p.x,p.y,p.z,p.qx,p.qy,p.qz,p.qw};
            for(unsigned j=0;j<7;++j)traceD68.v[i*7+j]=values[j];
            traceD68.flags|=(unsigned(f.valid[i])<<(i+1))|(unsigned(f.tracked[i])<<(i+4));}
        traceD68.count=21;
    }
    // flags bit0=published,1..3 raw valid,4..6 raw tracked,7..9 prepared
    // valid,10..12 prepared held,13..15 estimated. Values: raw H/L/R P3Q4,
    // then prepared H/L/R P3Q4. Record failure without altering grace/selection.
    const auto recordD68=[&](bool ok) noexcept {
        if(!traceOnD68)return;traceD68.endNs=diagnostics::perf_d61::nowNs();traceD68.sample=f.sequence;traceD68.flags|=unsigned(ok);
        for(unsigned i=0;i<3;++i){const auto&p=f.poses[i];const float values[]{p.x,p.y,p.z,p.qx,p.qy,p.qz,p.qw};
            for(unsigned j=0;j<7;++j)traceD68.v[21+i*7+j]=values[j];
            traceD68.flags|=(unsigned(f.valid[i])<<(i+7))|(unsigned(f.held[i])<<(i+10))|(unsigned(f.estimatedBehindHead[i])<<(i+13));}
        traceD68.count=42;diagnostics::motion_d68::push(traceD68);
    };
    if(completed)*completed={};
    if(!f.controllersSampled||!f.ns||!f.tick||f.epoch!=epoch.load(std::memory_order_acquire)){recordD68(false);return false;}
    const bool independentR94=f.refreshGenerationR94!=0;
    if(independentR94&&(!motion_trials_d67::UncappedSelectedAt(0)||
       f.refreshGenerationR94!=motion_trials_d67::handRefreshGenerationR94.load(std::memory_order_acquire))){recordD68(false);return false;}
    Frame prev{};
    bool previousR94=independentR94?handFramesR94.read(prev):frames.read(prev);
    if(independentR94&&(!previousR94||prev.refreshGenerationR94!=f.refreshGenerationR94))previousR94=frames.read(prev);
    const bool same=previousR94&&prev.epoch==f.epoch;
    if(same&&f.ns<=prev.ns){recordD68(false);return false;}
    // A new XR-frame anchor can shorten its prediction horizon. Do not feed
    // a backwards time target into the already selected hand history or call
    // it a new sample; retain the original clocks until the target catches up.
    if(independentR94&&same&&prev.refreshGenerationR94==f.refreshGenerationR94&&
       f.locateTime<=prev.locateTime){recordD68(false);return false;}
    for(unsigned i=0;i<3;++i){
        f.valid[i]=f.valid[i]&&Valid(f.poses[i]);f.tracked[i]=f.tracked[i]&&f.valid[i];
        // R59.16C7A: measurement only (semantics unchanged after Claude/Codex review): count HAND samples the runtime
        // reports valid but not camera-tracked, to decide with evidence whether a short motion bridge is needed.
        if(i&&f.valid[i]&&!f.tracked[i])estimated.fetch_add(1,std::memory_order_relaxed);
        f.goodTick[i]=f.tracked[i]?f.tick:(same?prev.goodTick[i]:0);
        f.estimatedBehindHead[i]=!cameraNow.load(std::memory_order_relaxed)&&BehindHead(f,i);
        f.held[i]=!f.tracked[i]||!same||!prev.tracked[i]; // reacquisition cannot become a new stroke
        if(f.estimatedBehindHead[i]){
            // Keep the current moving pose, not a frozen last-good hand. A
            // first/reacquired sample cannot create a stroke by itself.
            f.held[i]=!same||(!prev.tracked[i]&&!prev.estimatedBehindHead[i]);
        }else if(!f.tracked[i]){
            // Grace is display-only; never renew it from an inferred/repeated pose.
            const bool recent=same&&prev.valid[i]&&Age(prev.goodTick[i],f.tick,i?80:60);
            if(!f.valid[i]&&recent)f.poses[i]=prev.poses[i];
            f.valid[i]=recent&&Valid(f.poses[i]);
        }
    }
    f.sequence=Next();
    if(f.epoch!=epoch.load(std::memory_order_acquire)){recordD68(false);return false;}
    beforePublish(static_cast<const Frame&>(f));
    if(f.epoch!=epoch.load(std::memory_order_acquire)){recordD68(false);return false;}
    if(independentR94&&(!motion_trials_d67::UncappedSelectedAt(0)||
       f.refreshGenerationR94!=motion_trials_d67::handRefreshGenerationR94.load(std::memory_order_acquire))){recordD68(false);return false;}
    // Mark the source before exposing the packet, including publication
    // contention. No reader can see the independent family and then fall back
    // to a worker pose solely because its next bounded read was contended.
    if(independentR94)activeHandGenerationR94.store(f.refreshGenerationR94,std::memory_order_release);
    const bool ok=independentR94?handFramesR94.publish(f):frames.publish(f);
    if(ok){published.fetch_add(1,std::memory_order_relaxed);if(completed)*completed=f;}
    recordD68(ok);return ok;
}
inline bool Publish(Frame f,Frame* completed=nullptr) noexcept {
    return PublishPrepared(f,completed,[](const Frame&) noexcept {});
}
// R24: by-value receipt of the exact successful publication, never a later
// mailbox read. Tracking grace/estimated hands cannot qualify a camera family.
inline hj1::Sample CompletedCameraFamilyR24(const Frame& f,std::uint64_t comparisonEpoch) noexcept {
    hj1::Sample out{};
    if(!comparisonEpoch||!f.controllersSampled||!f.epoch||!f.sequence||!f.tick||!f.ns||f.locateTime<=0)return out;
    for(unsigned i=0;i<3;++i)if(!f.valid[i]||!f.tracked[i]||!Valid(f.poses[i]))return out;
    const auto pose=[](const Pose& p){return hj1::Pose7{{p.x,p.y,p.z},{p.qx,p.qy,p.qz,p.qw}};};
    out.ok=true;out.headValid=out.headTracked=true;out.head=pose(f.poses[0]);
    out.handPolicyD69=f.handPolicyD69;for(unsigned h=0;h<2;++h){out.handTargetD69[h]=f.handTargetD69[h];out.handLeadSourceD69[h]=f.handLeadSourceD69[h];}
    for(unsigned i=0;i<2;++i){out.valid[i]=out.tracked[i]=true;out.hand[i]=pose(f.poses[i+1]);}
    out.sourceEpoch=f.epoch;out.tick=f.tick;out.sequence=f.sequence;out.target=f.locateTime;out.steadyNs=f.ns;
    out.comparisonEpoch=comparisonEpoch;
    return out;
}
inline bool CameraFamilyCurrentR24(const hj1::Sample& s,std::uint64_t comparisonEpoch,std::uint64_t nowNs) noexcept {
    const int source=cameraSource.load(std::memory_order_acquire);
    if(!hj1::enabled.load(std::memory_order_acquire)||!enabled.load(std::memory_order_acquire)||source<0||source>2||
       !comparisonEpoch||s.comparisonEpoch!=comparisonEpoch||!s.ok||!s.sourceEpoch||s.sourceEpoch!=epoch.load(std::memory_order_acquire)||
       !s.sequence||!s.tick||s.target<=0||!Age(s.steadyNs,nowNs,std::uint64_t(hj1::kCacheNs))||
       !s.headValid||!s.headTracked||!s.valid[0]||!s.tracked[0]||!s.valid[1]||!s.tracked[1])return false;
    const auto valid=[](const hj1::Pose7& p){return Valid({p.p[0],p.p[1],p.p[2],p.q[0],p.q[1],p.q[2],p.q[3]});};
    return valid(s.head)&&valid(s.hand[0])&&valid(s.hand[1]);
}

enum class Choice { Original, Current, Invalidated };
// A fresh locate never renews the age of the button/permission packet. A stale
// permission remains stale, and an old epoch can never regain a current body.
template<class S> Choice Compose(S& out,const Frame& f,std::uint64_t nowTick,std::uint64_t nowNs) noexcept {
    if(!enabled.load(std::memory_order_acquire)||!f.controllersSampled||!f.sequence||f.epoch!=out.source_epoch||f.epoch!=epoch.load(std::memory_order_acquire))return Choice::Original;
    if(!Age(out.input_tick,nowTick,100)||!Age(f.ns,nowNs,80000000ull)){
        out.head_valid=out.left_valid=out.right_valid=false;
        out.head_held=out.left_held=out.right_held=true;
        rejected.fetch_add(1,std::memory_order_relaxed);return Choice::Invalidated;
    }
    auto set=[&](unsigned i,auto& pos,auto& q,bool& valid,bool& held,std::uint64_t& good){
        const auto& p=f.poses[i];pos={p.x,p.y,p.z};q={p.qx,p.qy,p.qz,p.qw};
        valid=f.valid[i]&&(Age(f.goodTick[i],nowTick,i?80:60)||f.estimatedBehindHead[i]);held=f.held[i];good=f.goodTick[i];
    };
    set(0,out.head,out.hq,out.head_valid,out.head_held,out.head_good_tick);
    set(1,out.left,out.lq,out.left_valid,out.left_held,out.left_good_tick);
    set(2,out.right,out.rq,out.right_valid,out.right_held,out.right_good_tick);
    out.sequence=f.sequence;out.tick=f.tick;out.timeNs=f.ns;out.poseTime=f.locateTime;
    if constexpr(requires{out.handPolicyD69;out.handTargetD69[0];out.handLeadSourceD69[0];}){
        out.handPolicyD69=f.handPolicyD69;for(unsigned h=0;h<2;++h){out.handTargetD69[h]=f.handTargetD69[h];out.handLeadSourceD69[h]=f.handLeadSourceD69[h];}}
    consumed.fetch_add(1,std::memory_order_relaxed);return Choice::Current;
}
} // namespace kcd2vr::tracking59c6
