#pragma once
// Independent F2 comparisons. The existing capture owner controls recording;
// this owner changes only the selected visual/timing experiment.
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include "vrik/hand_same_frame_hj1.h"
#include "vrik/visual_selection59c7f.h"
#include "vrik/visible_failure.h"
#include "vrik/motion_trials_d67.h"
#include "vrik/elbow_motion_d67.h"
namespace kcd2vr::smooth_lab {
inline std::atomic<unsigned> selected{0};
// D64: only F2/2 uses this choice. Default is the accepted I10 late boundary;
// the early D45 location remains an explicit independent comparison.
inline std::atomic<unsigned> timingLocationD64{1};
inline unsigned TimingLocationD64() noexcept {return timingLocationD64.load(std::memory_order_acquire);}

// FRIK retention per 90 Hz frame -> one-euro min cutoff with the same time constant.
constexpr float kFrikRetention90=.7f;
inline float CutoffHzForRetention(float r) noexcept { // tau = -1/(90 ln r), fc = 1/(2 pi tau)
    r=std::clamp(r,.01f,.999f);const float tau=-1.f/(90.f*std::log(r));return 1.f/(6.2831853f*tau);
}
// FRIK smoothingMoveFraction: frame-rate independent move toward the target, dt clamped to 3 nominal frames.
inline std::uint64_t SteadyNs() noexcept {return std::uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());}

// --- c) pacing policy (pure) --------------------------------------------------------------------------------------
struct Pacing {
    std::uint64_t suspendedUntilNs=0,lastPassNs=0;int slowRun=0;
    std::uint64_t gated=0,timeouts=0,slow=0,fallbacks=0,waitNsSum=0,waitNsMax=0;
};
constexpr int kSlowRunLimit=8;constexpr std::uint64_t kSuspendNs=5000000000ull;
inline bool PacingActive(const Pacing& p,std::uint64_t nowNs) noexcept {return nowNs>=p.suspendedUntilNs;}
// one gated game frame. returns true when pacing just fell back (caller may say so).
inline bool ObserveGate(Pacing& p,bool ok,std::uint64_t waitNs,std::uint64_t nowNs,std::uint64_t periodNs) noexcept {
    if(!periodNs)periodNs=11111111ull;
    ++p.gated;p.waitNsSum+=waitNs;p.waitNsMax=std::max(p.waitNsMax,waitNs);
    const std::uint64_t interval=p.lastPassNs&&nowNs>p.lastPassNs?nowNs-p.lastPassNs:0;p.lastPassNs=nowNs;
    bool bad=false;
    if(!ok){++p.timeouts;bad=true;p.slowRun=kSlowRunLimit;}
    else if(interval>periodNs*16/10){++p.slow;++p.slowRun;}
    else p.slowRun=0;
    if(bad||p.slowRun>=kSlowRunLimit){p.slowRun=0;p.lastPassNs=0;p.suspendedUntilNs=nowNs+kSuspendNs;++p.fallbacks;return true;}
    return false;
}
// D72 accepted timing is a persistent gameplay preference, not a finite trial.
// Record a slow/missed publication but never turn timing off for five seconds.
// The caller still uses the same 20 ms timeout and one-wait-per-native-frame
// claim; a missed token is NOT permission to reuse or freeze a stale pose.
inline void ObserveAcceptedTimingD72(Pacing& p,bool ok,std::uint64_t waitNs,std::uint64_t nowNs,std::uint64_t periodNs) noexcept {
    if(!periodNs)periodNs=11111111ull;
    ++p.gated;p.waitNsSum+=waitNs;p.waitNsMax=std::max(p.waitNsMax,waitNs);
    const auto interval=p.lastPassNs&&nowNs>p.lastPassNs?nowNs-p.lastPassNs:0;p.lastPassNs=nowNs;
    if(!ok)++p.timeouts;
    if(interval>periodNs*16/10)++p.slow;
    p.slowRun=0;p.suspendedUntilNs=0;
}
inline Pacing pacing{};inline std::atomic_flag pacingLock=ATOMIC_FLAG_INIT;
inline bool SetTimingLocationD64(unsigned location) noexcept {
    location=std::min(location,1u);
    if(timingLocationD64.exchange(location,std::memory_order_acq_rel)==location)return false;
    while(pacingLock.test_and_set(std::memory_order_acquire)){}
    pacing=Pacing{};pacingLock.clear(std::memory_order_release);
    return true;
}


struct Saved {bool have=false,on=false;float minHz=1.5f,beta=20.f;};
inline Saved baseline{};
// D39: the saved preference is independent of the effective F2 comparison.
// Only the Run owner mutates baseline/filter policy; UI publishes one atomic
// preference and requests its application on that owner's next iteration.
inline std::atomic<bool> baselineOnD39{false},baselineRefreshD39{false};
inline unsigned Mode() noexcept {return selected.load(std::memory_order_acquire);}
inline bool BaselineSmoothingD39() noexcept {return baselineOnD39.load(std::memory_order_acquire);}
inline void SetBaselineSmoothingD39(bool on) noexcept {
    if(baselineOnD39.exchange(on,std::memory_order_acq_rel)!=on)baselineRefreshD39.store(true,std::memory_order_release);
}
inline void RefreshBaselineSmoothingD39() noexcept {
    if(!baselineRefreshD39.exchange(false,std::memory_order_acq_rel))return;
    const bool effective=Mode()<2&&BaselineSmoothingD39();
    if(hand_smooth_h50::enabled.exchange(effective,std::memory_order_acq_rel)!=effective)
        hand_smooth_h50::policyEpoch.fetch_add(1,std::memory_order_acq_rel);
}
inline bool Enabled() noexcept {return Mode()>=2;}
inline unsigned Next(unsigned rung) noexcept {return rung<3?rung+1:0;}
// D45 user-selected ladder: 0 baseline/off, 1 baseline recording,
// 2 recording + selected timing location, 3 recording + elbow/shoulder experiment.
// Old comparisons remain source history only; no rung stacks their policies.
inline constexpr bool AllFixesRung(unsigned) noexcept {return false;}
inline bool AllFixesSelected() noexcept {return false;}
inline constexpr bool BugRecordingRung(unsigned rung) noexcept {return rung>=1&&rung<=3;}
inline bool PacingRung(unsigned rung) noexcept {return rung==2&&TimingLocationD64()==1;}
inline constexpr bool NativeAnchorRung(unsigned) noexcept {return false;}
inline bool PacingSelected() noexcept {return PacingRung(Mode());}
inline bool EarlyPacingRungD45(unsigned rung) noexcept {return rung==2&&TimingLocationD64()==0;}
inline bool EarlyPacingSelectedD45() noexcept {return EarlyPacingRungD45(Mode());}
inline bool AnyPacingSelectedD45() noexcept {return Mode()==2;}
inline bool ElbowShoulderSelectedD45() noexcept {return Mode()==3;}
// no rung selects the old adaptive free-hand filter any more (hand_smooth_h50 fast-stroke option).
inline constexpr bool AdaptiveFilterSelected() noexcept {return false;}
// a free hand whose contact has no live plane and no hit lets go in ~70 ms instead of a 1 m/s glide.
inline bool StaleContactFastRelease() noexcept {return AllFixesSelected();}
// Independent ordinary-body comparison; native semantic actions remain active.
inline bool NativePelvisYawSelected() noexcept {return false;}
inline std::uint64_t NativeRejoinPolicyR28() noexcept {
    return 0;
}
// Existing comparison policy epoch retires only the per-body anchor experiment.
inline std::uint64_t NativeAnchorPolicy() noexcept {
    return NativeAnchorRung(Mode())?hand_smooth_h50::policyEpoch.load(std::memory_order_acquire):0;
}
// Preserve explicit baseline filtering preferences; HJ1 stays retired.
inline void ApplyDefaults(bool on) noexcept {
    namespace hs=kcd2vr::hand_smooth_h50;
    baseline={true,on||hs::enabled.load(),on?CutoffHzForRetention(kFrikRetention90):hs::minCutoffHz.load(),hs::beta.load()};
    baselineOnD39.store(baseline.on,std::memory_order_release);baselineRefreshD39.store(false,std::memory_order_release);
    const auto mode=Mode();
    // J26: every comparison rung runs without the free-hand filter; 0/1 keep the user's own preference.
    hs::enabled.store(mode>=2?false:BaselineSmoothingD39());
    hs::minCutoffHz.store(baseline.minHz);
    hs::beta.store(baseline.beta);
    hs::policyEpoch.fetch_add(1,std::memory_order_release);
    kcd2vr::hj1::enabled.store(false);
}
inline const char* RungText(unsigned rung) noexcept {
    switch(rung){
    case 1:return "F2 1: bug recording; baseline motion";
    case 2:return TimingLocationD64()==1?"F2 2: bug recording + late-publish timing experiment":
        "F2 2: bug recording + early-frame timing experiment";
    case 3:return "F2 3: bug recording + elbow/shoulder experiment";
    default:return "F2 off: baseline motion; bug recording off";
    }
}
inline bool SetMode(unsigned mode) noexcept {
    if(mode>3)mode=0;
    if(selected.exchange(mode,std::memory_order_acq_rel)==mode)return false;
    namespace hs=kcd2vr::hand_smooth_h50;
    if(!baseline.have){baseline={true,hs::enabled.load(),hs::minCutoffHz.load(),hs::beta.load()};baselineOnD39.store(baseline.on,std::memory_order_release);}
    hs::enabled.store(mode>=2?false:BaselineSmoothingD39());
    hs::minCutoffHz.store(baseline.minHz);
    hs::beta.store(baseline.beta);
    hs::policyEpoch.fetch_add(1,std::memory_order_release); // no carry-over from another comparison
    kcd2vr::hj1::enabled.store(false,std::memory_order_release);
    while(pacingLock.test_and_set(std::memory_order_acquire)){}
    pacing=Pacing{};pacingLock.clear(std::memory_order_release);
    return true;
}
// This is a claim on the existing native frame, never a timer/burst guess.
// Keep it thread-local with the same owner as CurrentNativeFrameHJ1(). Mode
// changes do not permit a second wait in an already claimed native frame.
struct PacingFrameGate {std::uint64_t attempted=0;};
// Both comparison locations share one claim even if F2 changes during a frame.
inline thread_local PacingFrameGate pacingFrameGateD45{};
// Switching locations cannot reset consumption of the immutable XR token.
inline thread_local std::uint64_t pacingTokenD64=0;
inline std::atomic<std::uint64_t> pacingOutsideScope{0},pacingRepeatedFrame{0};
inline bool ClaimPacingFrame(PacingFrameGate& gate,std::uint64_t nativeFrame) noexcept {
    if(!nativeFrame){pacingOutsideScope.fetch_add(1,std::memory_order_relaxed);return false;}
    if(gate.attempted==nativeFrame){pacingRepeatedFrame.fetch_add(1,std::memory_order_relaxed);return false;}
    gate.attempted=nativeFrame;return true;
}
inline bool PacingWantedAtD45(std::uint64_t nowNs,bool early) noexcept {
    if(early?!EarlyPacingSelectedD45():!PacingSelected())return false;
    if(pacingLock.test_and_set(std::memory_order_acquire))return false;
    const bool a=PacingActive(pacing,nowNs);pacingLock.clear(std::memory_order_release);return a;
}
inline bool PacingWanted(std::uint64_t nowNs) noexcept {return PacingWantedAtD45(nowNs,false);}
inline void PacingObserved(bool ok,std::uint64_t waitNs,std::uint64_t nowNs,std::uint64_t periodNs,std::uint64_t tickMs) noexcept {
    if(pacingLock.test_and_set(std::memory_order_acquire))return;
    const bool fell=ObserveGate(pacing,ok,waitNs,nowNs,periodNs);pacingLock.clear(std::memory_order_release);
    if(fell)(void)kcd2vr::visible_failure::publishDiagnosticD74("F2 2: timing trial suspended for 5 s after slow frame or timeout",tickMs,diagnostics::capture168::Domain::Bug);
}
inline Pacing PacingSnapshot() noexcept {
    Pacing p{};if(pacingLock.test_and_set(std::memory_order_acquire))return p;p=pacing;pacingLock.clear(std::memory_order_release);return p;
}
} // namespace kcd2vr::smooth_lab
