// D42 worker contribution. Replaces only the existing combat-camera lease
// Apply line; native renderer/stereo/camera transforms are untouched.
{
    namespace ft=kcd2vr::free_targeting_d42;
    const bool selected=ft::enabled.load(std::memory_order_acquire);
    const bool current=selected&&frik_pose_probe::CombatFreeViewContextD42();
    ft::contextReady.store(current,std::memory_order_release);
    // OFF retains the previous F4 preference. The opt-in mode extends the
    // same camera-only CVar lease to F4 OFF and yields protected scene owners.
    const bool want=((rung>0&&cameraOnly50)||selected)&&(!selected||current);
    char change[240]{};
    if(g_combat_camera50.Apply(want,change,sizeof(change))){
        diagnostic_trace::Write(change);KCD2VR_DIAG_LOG(Bug,warning,"{}",change);
    }
    ft::cameraLeased.store(selected&&current&&g_combat_camera50.applied,std::memory_order_release);
    const auto now=std::uint64_t(GetTickCount64());
    const bool capture=diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now);
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load(std::memory_order_acquire);
    static bool previousCapture=false;
    static std::uint64_t previousSession=0,lastLine=0,baseCalls=0,baseVetoes=0,baseFinishSeen=0,baseFinishWritten=0,baseFinishSuppressed=0;
    static std::array<std::uint64_t,64> baseReasons{};
    const auto emit=[&](const char* prefix) {
        char line[2048]{};
        int n=std::snprintf(line,sizeof(line),
            "%s FREE_TARGETING session=%llu selected=%d context=%d cameraLease=%d rotationHook=%d legacyNoTurn=%d calls=%llu vetoed=%llu scope=PINNED_ROTATION_CALLER opponentSelection=NATIVE targetWrites=NONE forcedOutcomes=NONE finisherSeen=%llu finisherWritten=%llu finisherSuppressed=%llu ",
            prefix,(unsigned long long)previousSession,selected,current,
            ft::cameraLeased.load(std::memory_order_acquire),frik_pose_probe::CombatNoTurnReadyD40(),frik_pose_probe::CombatNoTurnD40(),
            (unsigned long long)(ft::rotationCalls.load(std::memory_order_relaxed)-baseCalls),
            (unsigned long long)(ft::rotationVetoes.load(std::memory_order_relaxed)-baseVetoes),
            (unsigned long long)(kcd2vr::finisherdiag_d42::seen.load()-baseFinishSeen),
            (unsigned long long)(kcd2vr::finisherdiag_d42::written.load()-baseFinishWritten),
            (unsigned long long)(kcd2vr::finisherdiag_d42::suppressed.load()-baseFinishSuppressed));
        // R103: NPC managers use the same native builder; CONTACT_IDENTITY is
        // "not Henry" there (D84 review), so those calls stay native and are
        // listed apart. henryCalls/henryVetoed is the free-targeting result.
        const auto npc=ft::rotationRefused[kcd2vr::align24::ContactIdentity].load(std::memory_order_relaxed)-baseReasons[kcd2vr::align24::ContactIdentity];
        const auto calls=ft::rotationCalls.load(std::memory_order_relaxed)-baseCalls;
        if(n>0&&n<int(sizeof(line))-200)
            n+=std::snprintf(line+n,sizeof(line)-n,"henryCalls=%llu henryVetoed=%llu npcNative=%llu keptPhases=0x%X ",
                (unsigned long long)(calls>=npc?calls-npc:0),
                (unsigned long long)(ft::rotationVetoes.load(std::memory_order_relaxed)-baseVetoes),
                (unsigned long long)npc,ft::keptPhaseMask.load(std::memory_order_relaxed));
        if(n>0&&n<int(sizeof(line)))n+=std::snprintf(line+n,sizeof(line)-n,"refusalCounts=");
        for(unsigned i=0;i<baseReasons.size()&&n>0&&n<int(sizeof(line))-100;++i){
            const auto count=ft::rotationRefused[i].load(std::memory_order_relaxed)-baseReasons[i];
            if(count)n+=std::snprintf(line+n,sizeof(line)-n,"%s:%llu,",kcd2vr::align24::RefusalName(i),(unsigned long long)count);
        }
        diagnostic_trace::Write(line);lastLine=now;
    };
    // Retire the old session before rebasing a repeated F3 start as well as an
    // explicit stop/expiry. END remains routable after the finite lease closes.
    if(previousCapture&&(!capture||session!=previousSession))emit("D42_F3_END");
    const bool started=capture&&(!previousCapture||session!=previousSession);
    if(started){
        baseCalls=ft::rotationCalls.load(std::memory_order_relaxed);baseVetoes=ft::rotationVetoes.load(std::memory_order_relaxed);
        for(unsigned i=0;i<baseReasons.size();++i)baseReasons[i]=ft::rotationRefused[i].load(std::memory_order_relaxed);
        ft::keptPhaseMask.store(0,std::memory_order_relaxed);
        baseFinishSeen=kcd2vr::finisherdiag_d42::seen.load();baseFinishWritten=kcd2vr::finisherdiag_d42::written.load();baseFinishSuppressed=kcd2vr::finisherdiag_d42::suppressed.load();
        previousSession=session;lastLine=0;
    }
    if(started||(capture&&now>=lastLine&&now-lastLine>=1000))emit("D42_F3");
    previousCapture=capture;
}
