#pragma once
// CPU elapsed time at existing boundaries only. No GPU queries or new waits.
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include "diagnostic_trace.h"
#include "capture168.h"
#include "performance_metrics_d61.h"
#include "tracking_sample59c6.h"
#include "core/render_pacing59c6.h"
#include "vrik/smooth_lab_f2.h"
#include "vrik/contact_glide_w5.h"
#include <windows.h>
namespace kcd2vr::diagnostics::xr165 {
enum Slot {Frame,WaitFrame,PredictedPose,SyncPose,LatePose,Input,SwapchainWait,Handoff,Bridge,FenceWait,EndFrame,BeginFrame,ActionSync,GripLocate,HandoffPickup,HandoffComplete,AcquireImage,Count};
inline constexpr const char* names[]{"frame","waitFrame","predictedPose","syncPose","latePose","input","swapchainWait","handoff","bridge","fenceWait","endFrame","beginFrame","actionSync","gripLocate","handoffPickup","handoffComplete","acquireImage"};
struct Metric {std::atomic<std::uint64_t> calls{0},ns{0},max{0};};
inline std::array<Metric,Count> metrics{};
inline std::atomic<std::uint64_t> frameCount{0}; // V66.178R29
inline std::atomic<std::uint64_t> predictedPeriodNs{0},predictedStepNs{0},waitReturnStepNs{0},waitReturns{0};
// W3 F2 rung 1 cadence (Codex list): counted only while the Bug recorder is on, formatted in Report().
// Desktop PRESENT serial is a count; certified-pair identity is a packed token, never a count. Pickups = XR copies,
// wait = request->pickup, latch = the 24 ms culling-pose reuse the render camera actually took (frame_sync).
inline std::atomic<std::uint64_t> lastPresentSerial{0},pairCompleteCount{0},pickupWaitMsSum{0},pickupWaitMsMax{0},pickupCount{0};
inline std::atomic<std::uint64_t> latchUsed{0},latchMissed{0},latchAgeMsSum{0},latchAgeMsMax{0};
inline bool BugOn() noexcept {return capture168::active(capture168::Domain::Bug,GetTickCount64());}
inline void AtomicMax(std::atomic<std::uint64_t>& a,std::uint64_t v) noexcept {auto o=a.load(std::memory_order_relaxed);while(o<v&&!a.compare_exchange_weak(o,v,std::memory_order_relaxed)){}}
inline void PickupObserved(std::uint64_t serial,std::uint64_t pairs,std::uint64_t waitMs,bool paired) noexcept {
    if(!BugOn())return;
    lastPresentSerial.store(paired?0:serial,std::memory_order_relaxed);pairCompleteCount.store(pairs,std::memory_order_relaxed);
    pickupWaitMsSum.fetch_add(waitMs,std::memory_order_relaxed);AtomicMax(pickupWaitMsMax,waitMs);pickupCount.fetch_add(1,std::memory_order_relaxed);
}
inline void LatchObserved(bool used,std::uint64_t ageMs) noexcept {
    if(!BugOn())return;
    if(!used){latchMissed.fetch_add(1,std::memory_order_relaxed);return;}
    latchUsed.fetch_add(1,std::memory_order_relaxed);latchAgeMsSum.fetch_add(ageMs,std::memory_order_relaxed);AtomicMax(latchAgeMsMax,ageMs);
}
// one SMOOTH_LAB line per 5 s while F2 rung 2 is on (and nothing while it is off).
inline void ReportSmoothLab(std::uint64_t now) noexcept {
    namespace sl=kcd2vr::smooth_lab;namespace h=kcd2vr::hj1;
    static std::uint64_t last=0;if(!sl::Enabled()||!BugOn()){last=0;return;}if(last&&now-last<5000)return;last=now;
    const auto p=sl::PacingSnapshot();
    char line[1800]{};std::snprintf(line,sizeof(line),"W3 XR_TIMING SMOOTH_COMPARE rung=%u hj1=%d matched=%llu early=%llu late=%llu fallback=%llu unnoted=%llu splitFrames=%llu nativeFrame=%llu sourceEpoch=%llu familyUses=%llu outsideNativeScope=%llu cameraNotes=%llu lastCameraFrame=%llu leadMs=%.2f ahead=0:%llu,1:%llu,2:%llu pacing=%d persistentTiming=%d outsidePacingScope=%llu repeatedNativeFrame=%llu gated=%llu timeouts=%llu slow=%llu waitAvgMs=%.2f waitMaxMs=%.2f damping=%d minHz=%.2f beta=%.1f cameraPolicy=UNCHANGED inputEdges=ORIGINAL family=HEAD_AND_HANDS",
        sl::Mode(),h::enabled.load()?1:0,(unsigned long long)h::matched.load(),(unsigned long long)h::early.load(),(unsigned long long)h::late.load(),
        (unsigned long long)h::fallback.load(),(unsigned long long)h::unnoted.load(),(unsigned long long)h::splitFrames.load(),(unsigned long long)h::lastNativeFrame.load(),(unsigned long long)h::lastSourceEpoch.load(),(unsigned long long)h::used.load(),(unsigned long long)h::outsideScope.load(),(unsigned long long)h::cameraNotes.load(),
        (unsigned long long)h::lastCameraFrame.load(),double(h::leadNs.load())/1e6,(unsigned long long)h::ahead0.load(),(unsigned long long)h::ahead1.load(),(unsigned long long)h::ahead2.load(),
        sl::AnyPacingSelectedD45()?1:0,kcd2vr::motion_trials_d67::TimingSelectedAt(now)?1:0,(unsigned long long)sl::pacingOutsideScope.load(),(unsigned long long)sl::pacingRepeatedFrame.load(),(unsigned long long)p.gated,(unsigned long long)p.timeouts,(unsigned long long)p.slow,
        p.gated?double(p.waitNsSum)/double(p.gated)/1e6:0.,double(p.waitNsMax)/1e6,
        kcd2vr::hand_smooth_h50::enabled.load()?1:0,double(kcd2vr::hand_smooth_h50::minCutoffHz.load()),double(kcd2vr::hand_smooth_h50::beta.load()));
    diagnostic_trace::Write(line);
}
inline void WaitReceipt(std::int64_t predicted,std::int64_t period) noexcept {
    // One XR worker; aggregate metadata is independent of GPU/presentation rate.
    static std::int64_t lastPredicted=0;static std::uint64_t lastReturn=0;
    const auto now=tracking59c6::NowNs();
    predictedPeriodNs.store(period>0?std::uint64_t(period):0,std::memory_order_relaxed);
    predictedStepNs.store(lastPredicted>0&&predicted>lastPredicted?std::uint64_t(predicted-lastPredicted):0,std::memory_order_relaxed);
    waitReturnStepNs.store(lastReturn&&now>=lastReturn?now-lastReturn:0,std::memory_order_relaxed);
    lastPredicted=predicted;lastReturn=now;waitReturns.fetch_add(1,std::memory_order_relaxed);
}
struct Timer {
    Slot slot;
    capture168::Domain domain=capture168::Domain::Hud;bool enabled=false;std::uint64_t session=0;std::chrono::steady_clock::time_point start{};
    perf_d61::Scope lightMetricsD61;
    explicit Timer(Slot s) noexcept:slot(s),lightMetricsD61(perf_d61::NativeStages+unsigned(s)){
        if(s==Frame)frameCount.fetch_add(1,std::memory_order_relaxed); // V66.178R29: always-on frame counter (one increment), no timing unless recording
        const auto now=GetTickCount64();
        domain=capture168::active(capture168::Domain::Bug,now)?capture168::Domain::Bug:capture168::Domain::Hud;
        enabled=capture168::active(domain,now);
        if(enabled){session=capture168::gate.session[unsigned(domain)].load();start=std::chrono::steady_clock::now();}
    }
    ~Timer(){if(!enabled||session!=capture168::gate.session[unsigned(domain)].load()||!capture168::active(domain,GetTickCount64()))return;const auto elapsed=static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-start).count());
        auto& m=metrics[slot];m.ns.fetch_add(elapsed,std::memory_order_relaxed);m.calls.fetch_add(1,std::memory_order_relaxed);
        auto old=m.max.load(std::memory_order_relaxed);while(old<elapsed&&!m.max.compare_exchange_weak(old,elapsed,std::memory_order_relaxed)){}
    }
};
// J26: one line per 5 s while F2 rung 2/3 (all hand fixes) is selected - also in rung 3, it is the fps watch for
// the pacing wait. xrFps = headset frames, gameFps = fresh game images that reached the headset.
inline void ReportPacingJ26(std::uint64_t elapsedMs,std::uint64_t frames,std::uint64_t fresh,std::uint64_t reused) noexcept {
    namespace sl=kcd2vr::smooth_lab;
    static std::uint64_t lGated=0,lWait=0,lSlow=0,lFast=0;static bool was=false;
    const bool on=sl::AllFixesSelected();
    const auto p=sl::PacingSnapshot();const std::uint64_t fast=kcd2vr::contact_glide_w5::fastReleases.load(std::memory_order_relaxed);
    if(!on||!was||!elapsedMs||p.gated<lGated){lGated=p.gated;lWait=p.waitNsSum;lSlow=p.slow;lFast=fast;was=on;return;} // first window primes
    const std::uint64_t g=p.gated-lGated,w=p.waitNsSum-lWait,sl5=p.slow-lSlow,fr=std::uint64_t(fast)-lFast;lGated=p.gated;lWait=p.waitNsSum;lSlow=p.slow;lFast=fast;
    const double sec=double(elapsedMs)/1000.;
    char line[400]{};std::snprintf(line,sizeof(line),"J26 PACING rung=%u fps=%.1f xrFps=%.1f reusePct=%.1f waitAvgMs=%.2f waitMaxMs=%.2f slow=%llu timeouts=%llu contactFastReleases=%llu",
        sl::Mode(),double(fresh)/sec,double(frames)/sec,(fresh+reused)?100.0*double(reused)/double(fresh+reused):0.,g?double(w)/double(g)/1e6:0.,double(p.waitNsMax)/1e6,
        (unsigned long long)sl5,(unsigned long long)p.timeouts,(unsigned long long)fr);
    diagnostic_trace::Write(line);
}
// Existing mod worker only. Snapshot deltas; max is explicitly lifetime-wide.
inline void Report(std::uint64_t now) noexcept {
    ReportSmoothLab(now);
    static std::uint64_t last=0,lastFrames=0;static std::array<std::uint64_t,Count> calls{},ns{};
    if(now-last<5000)return;const auto elapsed=last?now-last:0;last=now;
    const auto fc=frameCount.load(std::memory_order_relaxed);const auto frames=fc-lastFrames;lastFrames=fc;
    // R59.16C7A: the always-on line also says WHY: headset period and fresh/reused game images (both counters are always counted).
    static std::uint64_t lastFresh=0,lastReuse=0;std::uint64_t fresh=0,reuse=0;
    for(unsigned source=0;source<2;++source){fresh+=pacing59c6::counts[source][0].load(std::memory_order_relaxed);reuse+=pacing59c6::counts[source][1].load(std::memory_order_relaxed);}
    const auto df=fresh-lastFresh,dr=reuse-lastReuse;lastFresh=fresh;lastReuse=reuse;
    ReportPacingJ26(elapsed,frames,df,dr);
    if(!capture168::active(capture168::Domain::Hud,now)&&!capture168::active(capture168::Domain::Bug,now)){ // V66.178R29: frame rate only (essential), the per-boundary timing stays a HUD-recorder line
        // Do not divide a previous capture's totals by the next short window.
        for(unsigned i=0;i<Count;++i){calls[i]=metrics[i].calls.load();ns[i]=metrics[i].ns.load();}
        return; }
    // Three bounded lines: the journal's per-record capacity is 2048 bytes.
    char header[768]{};std::snprintf(header,sizeof(header),"R59.16C6 XR_TIMING part=SUMMARY windowMs=%llu xrFramesPerSec=%.1f CPU_elapsed_including_waits=1 nestedDoNotSum=1 runtimePeriodMs=%.3f predictedStepMs=%.3f lastWaitReturnIntervalMs=%.3f poseFamilies=%llu poseReads=%llu synchronousHands=%d clock=STEADY_NS",
        (unsigned long long)elapsed,elapsed?double(frames)*1000.0/double(elapsed):0.0,
        double(predictedPeriodNs.load())/1e6,double(predictedStepNs.load())/1e6,double(waitReturnStepNs.load())/1e6,
        (unsigned long long)tracking59c6::published.load(),(unsigned long long)tracking59c6::consumed.load(),tracking59c6::enabled.load());
    diagnostic_trace::Write(header);
    for(unsigned source=0;source<2;++source){const auto& c=pacing59c6::counts[source];char pace[600]{};
      std::snprintf(pace,sizeof(pace),"R59.16C6 XR_TIMING part=PACING source=%s counters=CUMULATIVE fresh=%llu reuse=%llu graceWait=%llu futureWait=%llu identityBusy=%llu clockReject=%llu strictWait=%llu",
        source?"CERTIFIED_PAIR":"DESKTOP_PRESENT",(unsigned long long)c[0].load(),(unsigned long long)c[1].load(),(unsigned long long)c[2].load(),(unsigned long long)c[3].load(),(unsigned long long)c[4].load(),(unsigned long long)c[5].load(),(unsigned long long)c[6].load());diagnostic_trace::Write(pace);
    }
    if(capture168::active(capture168::Domain::Bug,now)){ // CPU receipts only; no cadence readback while the Bug recorder is off.
      static std::uint64_t lSerial=0,lPairs=0,lWait=0,lPick=0,lUsed=0,lMiss=0,lAge=0,lSession=0,lTick=0;
      const auto session=capture168::gate.session[unsigned(capture168::Domain::Bug)].load();
      const bool complete=lSession==session&&lTick&&now>lTick;
      const auto cadenceElapsed=complete?now-lTick:0;
      const auto serial=lastPresentSerial.load(),pairs=pairCompleteCount.load(),wait=pickupWaitMsSum.load(),pick=pickupCount.load();
      const auto used=latchUsed.load(),miss=latchMissed.load(),age=latchAgeMsSum.load();
      const bool presentRateAvailable=lSerial&&serial>=lSerial;
      const auto dSerial=presentRateAvailable?serial-lSerial:0,dPairs=lPairs&&pairs>=lPairs?pairs-lPairs:0,dPick=pick-lPick,dWait=wait-lWait,dUsed=used-lUsed,dMiss=miss-lMiss,dAge=age-lAge;
      lSerial=serial;lPairs=pairs;lWait=wait;lPick=pick;lUsed=used;lMiss=miss;lAge=age;
      lSession=session;lTick=now; // first report primes this capture; never divide an older capture by a new window
      const double sec=cadenceElapsed?double(cadenceElapsed)/1000.:0.;
      if(complete&&session==capture168::gate.session[unsigned(capture168::Domain::Bug)].load()){
      char cad[700]{};std::snprintf(cad,sizeof(cad),"W3 XR_TIMING part=CADENCE windowMs=%llu xrFramesPerSec=%.1f producedPresentsPerSec=%.1f completedPairsPerSec=%.1f pickupsPerSec=%.1f freshPerSec=%.1f reusedPerSec=%.1f reusePct=%.1f pickupWaitAvgMs=%.2f pickupWaitMaxEverMs=%llu cameraLatchUsed=%llu cameraLatchMissed=%llu latchAgeAvgMs=%.1f latchAgeMaxEverMs=%llu poseSourceMode=%d smoothingRung=%u presentRateAvailable=%d",
        (unsigned long long)cadenceElapsed,sec>0?double(frames)/sec:0.,presentRateAvailable&&sec>0?double(dSerial)/sec:-1.,sec>0?double(dPairs)/sec:0.,sec>0?double(dPick)/sec:0.,sec>0?double(df)/sec:0.,sec>0?double(dr)/sec:0.,
        (df+dr)?100.0*double(dr)/double(df+dr):0.,dPick?double(dWait)/double(dPick):0.,(unsigned long long)pickupWaitMsMax.load(),
        (unsigned long long)dUsed,(unsigned long long)dMiss,dUsed?double(dAge)/double(dUsed):0.,(unsigned long long)latchAgeMsMax.load(),
        tracking59c6::cameraSource.load(),kcd2vr::smooth_lab::Mode(),presentRateAvailable?1:0);
      diagnostic_trace::Write(cad);
      }
    }
    for(unsigned group=0;group<3;++group){char text[1800]{};int n=std::snprintf(text,sizeof(text),"R59.16C6 XR_TIMING part=STAGES_%u windowMs=%llu",group+1,(unsigned long long)elapsed);
      for(unsigned i=group*6;i<Count&&i<(group+1)*6;++i){auto& m=metrics[i];const auto c=m.calls.load(std::memory_order_relaxed),t=m.ns.load(std::memory_order_relaxed);
        const auto dc=c-calls[i],dt=t-ns[i];calls[i]=c;ns[i]=t;
        if(n>0&&n<static_cast<int>(sizeof(text)))n+=std::snprintf(text+n,sizeof(text)-n," %s={n:%llu avgMs:%.3f maxEverMs:%.3f}",names[i],(unsigned long long)dc,dc?double(dt)/double(dc)/1e6:0.,double(m.max.load(std::memory_order_relaxed))/1e6);
      }
      diagnostic_trace::Write(text);
    }
}
} // namespace
