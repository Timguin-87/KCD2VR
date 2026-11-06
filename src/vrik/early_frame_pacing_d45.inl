// D64: two explicitly selected locations share one immutable-token consumer.
// Existing paced modes do not mutate poses or stereo. Mode4 adds a pose-only
// pre-update refresh. Included at namespace kcd2vr scope.
static void PaceGameplayAtD64(bool early) noexcept {
    namespace sl=kcd2vr::smooth_lab;
    const auto controlNowD67=std::uint64_t(GetTickCount64());
    // Persistent mode4 has priority over every finite F2/F7 timing comparison.
    // Both entry points return here: no fallthrough to a token wait is possible.
    if(kcd2vr::motion_trials_d67::UncappedSelectedAt(controlNowD67)){
        if(!early||CameraHook::GetInjectionMode()!=1)return;
        const auto nativeFrame=native_stereo_outer_lab::CurrentNativeFrameHJ1();
        static thread_local sl::PacingFrameGate uncappedFrame{};
        if(!sl::ClaimPacingFrame(uncappedFrame,nativeFrame))return;
        namespace oc=kcd2vr::diagnostics::ownership48;
        const bool traceF2=oc::active(controlNowD67);
        const bool traceF7=diagnostics::capture168::active(diagnostics::capture168::Domain::Vrik,controlNowD67);
        const auto before=traceF2||traceF7?sl::SteadyNs():0;
        const auto receipt=XrManager::Instance().RefreshUncappedPoseFamily();
        // The pose refresh is gameplay work; diagnostic clocks/formatting are
        // absent when neither recorder is active.
        if(!traceF2&&!traceF7)return;
        const auto after=sl::SteadyNs();
        const auto now=std::uint64_t(GetTickCount64());
        if(traceF2&&oc::active(now)){
            oc::Event e{};e.stage=15;e.tick=now;e.thread=GetCurrentThreadId();
            e.sample=receipt.sequence;e.publication=nativeFrame;e.sourceNs=before;e.generation=after;
            e.item=receipt.sourceNs;e.code=int(receipt.result);e.flags=16u;
            oc::name(e,"UNCAPPED_POSE_LOCATE_NO_TOKEN_WAIT");(void)oc::push(e);
        }
        if(traceF7&&diagnostics::capture168::active(diagnostics::capture168::Domain::Vrik,now)){
            static thread_local std::uint64_t lastReceipt=0;
            if(!lastReceipt||now<lastReceipt||now-lastReceipt>=500){lastReceipt=now;
                char m[768]{};std::snprintf(m,sizeof(m),
                    "VRIK_F7 FRESH_POSE_UNCAPPED nativeFrame=%llu poseSource=%d result=%u reason=%s effective=%s published=%d waitNs=0 locateNs=%llu sample=%llu sourceNs=%llu sourceTick=%llu sampleTarget=%lld requestedTarget=%lld validMask=%u trackedMask=%u lateHead=UNCHANGED cameraHeadWrites=0 stereoSchedule=UNCHANGED meshSubmission=UNOBSERVED",
                    (unsigned long long)nativeFrame,receipt.source,unsigned(receipt.result),receipt.Reason(),receipt.Effective(),receipt.published?1:0,
                    (unsigned long long)(after>=before?after-before:0),(unsigned long long)receipt.sequence,
                    (unsigned long long)receipt.sourceNs,(unsigned long long)receipt.sourceTick,
                    (long long)receipt.target,(long long)receipt.requestedTarget,receipt.validMask,receipt.trackedMask);
                diagnostic_trace::Write(m);
            }
        }
        return;
    }
    const auto trialD67=kcd2vr::motion_trials_d67::ModeAt(controlNowD67);
    const bool freshD67=kcd2vr::motion_trials_d67::TimingSelectedAt(controlNowD67);
    if(freshD67?early:trialD67?true:(early?!sl::EarlyPacingSelectedD45():!sl::PacingSelected()))return;
    const auto nativeFrame=native_stereo_outer_lab::CurrentNativeFrameHJ1();
    if(!nativeFrame||XrManager::Instance().GetXrRenderGateMode()!=0||CameraHook::GetInjectionMode()!=1)return;
    const auto before=sl::SteadyNs();
    bool wantedD67=false;
    if(freshD67)wantedD67=true; // D72: accepted preference has no automatic five-second OFF period.
    else wantedD67=sl::PacingWantedAtD45(before,early);
    if(!wantedD67||!sl::ClaimPacingFrame(sl::pacingFrameGateD45,nativeFrame))return;
    // Consume an available unconsumed token FIRST. Waiting for a token newer
    // than arrival can wait on XR while XR waits for this native render pickup.
    // Same token API and20ms timeout as accepted I10 rung4. D72 keeps
    // accepted fresh timing active; only legacy comparisons retain5s suspension.
    // One native-frame claim spans both locations even if the menu switches
    // between them. No pose mutex or physics/command lease is acquired here.
    auto& token=sl::pacingTokenD64;
    const auto oldToken=token;
    const bool ok=XrManager::Instance().WaitForNextRenderFrameToken(token,20,true,false);
    const auto after=sl::SteadyNs();
    const auto period=XrManager::Instance().Hj1PeriodNs();
    const auto now=std::uint64_t(GetTickCount64());
    if(freshD67){
        if(!sl::pacingLock.test_and_set(std::memory_order_acquire)){
            sl::ObserveAcceptedTimingD72(sl::pacing,ok,after>=before?after-before:0,after,period>0?std::uint64_t(period):0ull);
            sl::pacingLock.clear(std::memory_order_release);
        }
    }else sl::PacingObserved(ok,after>=before?after-before:0,after,period>0?std::uint64_t(period):0ull,now);
    // CPU wait endpoints and XR token remain separate clocks, not scanout.
    namespace oc=kcd2vr::diagnostics::ownership48;
    if(oc::active(now)){
        oc::Event e{};e.stage=15;e.tick=now;e.thread=GetCurrentThreadId();
        e.sample=token;e.publication=nativeFrame;e.sourceNs=before;e.generation=after;
        e.item=oldToken;e.code=ok?1:0;e.flags=freshD67?8u:early?2u:4u;
        oc::name(e,freshD67?"AVAILABLE_UNCONSUMED_PUBLICATION_D72":early?"EARLY_FRAME_WAIT_BEFORE_NATIVE_UPDATE_D45":"LATE_WAIT_BEFORE_CAMERA_PUBLISH_D64");(void)oc::push(e);
    }
    // Bounded existing F7 recorder only. These are CPU/token receipts, not pixels.
    if(freshD67&&diagnostics::capture168::active(diagnostics::capture168::Domain::Vrik,now)){
        static thread_local std::uint64_t lastReceiptD67=0;
        if(!lastReceiptD67||now<lastReceiptD67||now-lastReceiptD67>=500){lastReceiptD67=now;
            char m[448]{};std::snprintf(m,sizeof(m),"VRIK_F7 TIMING_D67 nativeFrame=%llu oldConsumed=%llu returned=%llu waitNs=%llu ok=%d freshAfterArrival=0 availableUnconsumed=1 cameraTransform=UNCHANGED stereoSchedule=UNCHANGED meshSubmission=UNOBSERVED",
                (unsigned long long)nativeFrame,(unsigned long long)oldToken,(unsigned long long)token,(unsigned long long)(after>=before?after-before:0),ok?1:0);
            diagnostic_trace::Write(m);
        }
    }
}
void FrameSync::PaceGameplayBeforeUpdateD45() noexcept {PaceGameplayAtD64(true);}
