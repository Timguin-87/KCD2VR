#pragma once
#include "temporal_compose_publication_d82.h"
#include <atomic>
#include <cstdint>
#include <cstdio>

// Bounded F2 diagnostic state for temporal publication. The D85R6 final-main
// recording/queue transaction is the primary temporal publisher.
// Timeout is diagnostic only. SDK evaluation is not proof of final main-target
// contents; Native0 retains its established first-writer boundary.
namespace kcd2vr::temporal_pair_d85 {

inline constexpr std::uint64_t kStarvationMs=1000;
inline constexpr std::uint64_t kRepeatReportMs=10000;
// A menu/theater pause makes no pair attempts; it must not latch the fallback.
inline constexpr std::uint64_t kMinFailedAttempts=45;

enum class Check : std::uint8_t {
    None=0,
    ComposeQualification, // exact RIGHT FinalCompose call never asked for a scope
    ScopeInactive,         // census/queue-copy owner inactive at compose
    ScopeHooks,            // Reset/ClearState/Draw observers not ready
    ScopePairFrame,        // compose frame is not the locked pair frame
    ScopeSdkReceipt,       // no successful L+R SDK receipt at compose time
    ScopeMainTarget,       // no observed engine main target
    NoScopedMainDraw,      // scope opened, no draw with the main RTV bound
    Superseded,            // later unscoped main-target draw cleared the stamp
    StampNotSubmitted,     // stamped list not seen at submission for this frame
    OutputMismatch,        // submitted stamp output is not the current main target
    SdkReceipt,            // receipt missing at submission
    LeftCopy,              // LEFT preservation copy failed
    RightCopy,             // RIGHT preservation copy failed
    FallbackSdkReceipt,    // retained diagnostic enum value
    FinalMainBoundary, FinalMainCopy, FinalMainSubmitted, CaptureLifetimePressure
};

inline const char* CheckName(Check c) noexcept {
    switch(c) {
    case Check::None: return "none";
    case Check::ComposeQualification: return "compose-qualification";
    case Check::ScopeInactive: return "scope-inactive";
    case Check::ScopeHooks: return "scope-hooks";
    case Check::ScopePairFrame: return "scope-pair-frame";
    case Check::ScopeSdkReceipt: return "scope-sdk-receipt";
    case Check::ScopeMainTarget: return "scope-main-target";
    case Check::NoScopedMainDraw: return "no-scoped-main-draw";
    case Check::Superseded: return "later-main-draw";
    case Check::StampNotSubmitted: return "stamp-not-submitted";
    case Check::OutputMismatch: return "output-mismatch";
    case Check::SdkReceipt: return "sdk-receipt";
    case Check::LeftCopy: return "left-copy";
    case Check::RightCopy: return "right-copy";
    case Check::FallbackSdkReceipt: return "fallback-sdk-receipt";
    case Check::FinalMainBoundary:return "final-main-boundary-not-observed";
    case Check::FinalMainCopy:return "final-main-copy-not-recorded";
    case Check::FinalMainSubmitted:return "final-main-copy-not-submitted";
    case Check::CaptureLifetimePressure:return "capture-lifetime-slots-busy";
    }
    return "unknown";
}

enum Stage : std::uint32_t { ScopeOpened=1u, DrawRecorded=2u, StampSubmitted=4u };

// Bootstrap keeps one rolling LEFT window until a RIGHT phase locks a frame.
// A locked frame that has passed without publication is stale: start again at
// this LEFT phase instead of rejecting every later frame for the whole run.
inline bool ShouldRearmBootstrap(bool liveStarted,std::uint32_t lockedFrame,std::uint32_t frame,
                                 bool pairPublished,bool drainRequested) noexcept {
    return !liveStarted && lockedFrame!=0 && frame!=lockedFrame && !pairPublished && !drainRequested;
}

// This legacy claim is used only by Native0. Temporal copies and publication
// belong to temporal_capture_d85::Owner at the final-main boundary, including
// when the watchdog has expired and both vendor evaluations already succeeded.
inline bool ClaimRightCopy(std::atomic<bool>& consumed,bool temporal,bool nativeRightMain,
                           const temporal_compose_d82::Stamp& composition,std::uint32_t frame,
                           std::uintptr_t currentOutput,bool samePairSdkReady,
                           bool starvationFallback,bool fallbackSdkReady,bool& viaFallback) noexcept {
    (void)starvationFallback;(void)fallbackSdkReady;
    viaFallback=false;
    if(temporal)return false;
    return temporal_compose_d82::ClaimRightCopy(consumed,false,nativeRightMain,composition,
                                                frame,currentOutput,samePairSdkReady);
}

struct Watchdog {
    std::atomic<bool> armed{false},fallback{false};
    std::atomic<std::uint64_t> sinceMs{0};       // run start or last temporal pair
    std::atomic<std::uint64_t> lastAnyPairMs{0};
    std::atomic<std::uint64_t> lastReportMs{0};
    std::atomic<std::uint32_t> progress{0},attemptFrame{0};
    std::atomic<std::uint8_t> detail{0},lastFailed{0};
    std::atomic<std::uint64_t> attempts{0},temporalPairs{0},fallbackPairs{0},attemptsSinceTemporal{0};

    void Arm(std::uint64_t now) noexcept {
        fallback.store(false,std::memory_order_release);
        sinceMs.store(now,std::memory_order_release);lastAnyPairMs.store(now,std::memory_order_release);
        lastReportMs.store(0,std::memory_order_release);
        progress.store(0,std::memory_order_release);attemptFrame.store(0,std::memory_order_release);
        detail.store(0,std::memory_order_release);lastFailed.store(0,std::memory_order_release);
        attempts.store(0,std::memory_order_release);temporalPairs.store(0,std::memory_order_release);
        fallbackPairs.store(0,std::memory_order_release);attemptsSinceTemporal.store(0,std::memory_order_release);
        armed.store(true,std::memory_order_release);
    }
    void Disarm() noexcept {armed.store(false,std::memory_order_release);fallback.store(false,std::memory_order_release);}

    // A new pair frame is being attempted; keep the previous attempt's verdict.
    void BeginAttempt(std::uint32_t frame) noexcept {
        if(attemptFrame.exchange(frame,std::memory_order_acq_rel)==frame)return;
        const auto previous=Failed();
        if(previous!=Check::None)lastFailed.store(static_cast<std::uint8_t>(previous),std::memory_order_release);
        progress.store(0,std::memory_order_release);detail.store(0,std::memory_order_release);
        attempts.fetch_add(1,std::memory_order_relaxed);attemptsSinceTemporal.fetch_add(1,std::memory_order_acq_rel);
    }
    void Reach(Stage s) noexcept {progress.fetch_or(s,std::memory_order_acq_rel);}
    void Refuse(Check c) noexcept {
        std::uint8_t none=0;(void)detail.compare_exchange_strong(none,static_cast<std::uint8_t>(c),std::memory_order_acq_rel);
    }
    // First failed check of the current attempt (the earliest refusal, else the
    // first stage of the certificate chain that was not reached).
    Check Failed() const noexcept {
        const auto d=detail.load(std::memory_order_acquire);
        if(d)return static_cast<Check>(d);
        const auto p=progress.load(std::memory_order_acquire);
        if(!attemptFrame.load(std::memory_order_acquire))return Check::None;
        if(!(p&ScopeOpened))return Check::FinalMainBoundary;
        if(!(p&DrawRecorded))return Check::FinalMainCopy;
        if(!(p&StampSubmitted))return Check::FinalMainSubmitted;
        return Check::None;
    }
    // Reported check: the current attempt, or the last finished one.
    Check ReportCheck() const noexcept {
        const auto c=Failed();
        return c!=Check::None?c:static_cast<Check>(lastFailed.load(std::memory_order_acquire));
    }
    void Published(bool viaFallback,std::uint64_t now) noexcept {
        lastAnyPairMs.store(now,std::memory_order_release);
        if(viaFallback){fallbackPairs.fetch_add(1,std::memory_order_relaxed);return;}
        temporalPairs.fetch_add(1,std::memory_order_relaxed);sinceMs.store(now,std::memory_order_release);
        fallback.store(false,std::memory_order_release); // A published final-main pair resets the starvation episode.
        attemptsSinceTemporal.store(0,std::memory_order_release);
        progress.store(0,std::memory_order_release);detail.store(0,std::memory_order_release);
    }
    // Starvation latch: true once 1 s and at least 45 pair attempts passed
    // without a certified temporal pair. This never authorizes an early copy.
    bool Tick(std::uint64_t now) noexcept {
        if(!armed.load(std::memory_order_acquire)||fallback.load(std::memory_order_acquire))return false;
        const auto since=sinceMs.load(std::memory_order_acquire);
        if(now<since||now-since<kStarvationMs)return false;
        if(attemptsSinceTemporal.load(std::memory_order_acquire)<kMinFailedAttempts)return false;
        bool expected=false;
        return fallback.compare_exchange_strong(expected,true,std::memory_order_acq_rel);
    }
    bool FallbackActive() const noexcept {return armed.load(std::memory_order_acquire)&&fallback.load(std::memory_order_acquire);}
    // One short F2-only status line (the producer gates this call): at the latch, then only while no pair at all
    // reaches the headset for >1 s (at most every 10 s). Max 96 ASCII chars.
    bool Report(std::uint64_t now,bool latchedNow,const char* tech,char* out,std::size_t size) noexcept {
        if(!out||!size)return false;
        out[0]=0;
        if(!armed.load(std::memory_order_acquire))return false;
        const auto last=lastReportMs.load(std::memory_order_acquire);
        const auto anyPair=lastAnyPairMs.load(std::memory_order_acquire);
        const bool starving=FallbackActive()&&now>=anyPair&&now-anyPair>=kStarvationMs;
        if(!latchedNow&&!(starving&&(!last||now<last||now-last>=kRepeatReportMs)))return false;
        lastReportMs.store(now?now:1,std::memory_order_release);
        const char* name=CheckName(ReportCheck());
        const int n=latchedNow
            ?std::snprintf(out,size,"%s stereo: no certified pair for 1s (%s); waiting for final-main submission",tech?tech:"Upscaler",name)
            :std::snprintf(out,size,"%s stereo: no pair for 1s (%s)",tech?tech:"Upscaler",name);
        return n>0;
    }
};

}
