bool InteractionLifecycleActive168() noexcept {
    return diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64());
}
static bool ArmInteractionBody122() noexcept;
static bool InteractionActionReady175() noexcept;
static std::atomic<unsigned> g_interaction_coverage175{0};
unsigned InteractionCoverage175() noexcept {return g_interaction_coverage175.load(std::memory_order_acquire);}
// H17: recorder coverage and mutation capability are different contracts.
// Only the PICKUP entry group can intercept native pickup; the other lifecycle
// groups/body/input callbacks are read-only. Native grasp has its own exact-pin
// all-or-nothing install/cleanup hooks and each physical writer rechecks its own
// body/leaf/owner proof. An optional SetPhysicParams observer cannot revoke them.
static constexpr unsigned kInteractionRequiredH17=0x101; // PICKUP + native Use/action
static constexpr unsigned kInteractionRecorderH17=0x17F; // historical full capture
static bool InteractionRecorderCompleteH17() noexcept {
    return retail_build112::Ready(WhBase())&&
        (g_interaction_coverage175.load(std::memory_order_acquire)&kInteractionRecorderH17)==kInteractionRecorderH17&&
        g_item_attach_operation_original;
}
bool InteractionLifecycleReady36() noexcept {
    return retail_build112::Ready(WhBase())&&
        (g_interaction_coverage175.load(std::memory_order_acquire)&kInteractionRequiredH17)==kInteractionRequiredH17&&
        g_item_attach_operation_original;
}
static const char* InteractionExperimentBlock36() noexcept {
    // Recording expiry must not revoke an explicitly armed gameplay experiment.
    if(!InteractionLifecycleReady36())return "REQUIRED_CAPABILITY_UNAVAILABLE";
    if(!CalibrationReady())return "CALIBRATION_REQUIRED";
    if(CombatCensusMode()<3)return "VRIK_OFF";
    if(!kcd2vr::combat_rules178r10::workstationEnabled.load())return "INI_DISABLED";
    return nullptr;
}
bool SetInteractionExperiment36(bool on) noexcept {
    const char* reason=on?InteractionExperimentBlock36():nullptr;
    const auto rung=kcd2vr::combat_rules178r10::f5Rung.load();
    const bool admitted=on&&!reason&&rung>=2&&rung<=3;
    if(on&&!admitted){char visible[160]{};std::snprintf(visible,sizeof(visible),"Interactions blocked: %s",reason?reason:"F5_RUNG_REQUIRED");
        frik_pose_probe::PublishVisibleFailure(visible);} // DS-F12: Miki's one visible line
    kcd2vr::workstation178r20c1::setExperiment36(admitted);
    action_time_owner59c11::SetPickupExperiment62(admitted);
    const auto coverage=g_interaction_coverage175.load(std::memory_order_acquire);
    char m[400]{};std::snprintf(m,sizeof(m),
        "V66.178R36C4 F5_GATE requested=%d armed=%d reason=%s capabilityMask=%u requiredMask=%u missingRequired=%u recorderComplete=%d missingRecorder=%u policy=H17 modeWrites=NONE",
        on,admitted,reason?reason:admitted?"READY":"OFF",coverage,kInteractionRequiredH17,
        kInteractionRequiredH17&~coverage,InteractionRecorderCompleteH17()?1:0,kInteractionRecorderH17&~coverage);diagnostic_trace::Write(m);
    return !on||admitted;
}
void ServiceInteractionExperiment36() noexcept {
#include "vrik/door_service46.inl"
    if(kcd2vr::combat_rules178r10::f5Rung.load()==1&&!InteractionLifecycleActive168()){
        kcd2vr::combat_rules178r10::f5Rung=0;
        (void)SetInteractionLifecycle165(false);
        diagnostic_trace::Write("V66.178R36C4 F5_GATE rung=0 reason=CAPTURE_EXPIRED");
    }else if(kcd2vr::workstation178r20c1::experimentArmed36.load()&&InteractionExperimentBlock36()){
        (void)SetInteractionExperiment36(false);
        diagnostic_trace::Write("V66.178R36C4 F5_GATE armed=0 reason=PREREQUISITE_LOST rearm=EXPLICIT_F5_CYCLE");
    }
}
static bool PrepareInteractionResources59c7f() noexcept;
bool PrepareGameplayResourcesD84() noexcept {
    if(!CalibrationReady()||!retail_build112::Ready(WhBase()))return false;
    const bool wanted=kcd2vr::instant_d72::Interactions();
    const bool pickup=!wanted||ArmLifecycle165(1u);
    const unsigned gameplayCoverage=g_lifecycle_coverage175.load()|(InteractionActionReady175()?256u:0u);
    g_interaction_coverage175.fetch_or(gameplayCoverage,std::memory_order_release);
    // Reload preparation independently considers both saved ranged switches.
    (void)PrepareStagedBowDefault59();
    return pickup;
}
bool SetInteractionLifecycle165(bool on) noexcept {
    (void)SetInteractionExperiment36(false);
    EndLifecycle165(on?"RESTART":"USER_STOP");
    const auto now=std::uint64_t(GetTickCount64());
    diagnostics::capture168::select(diagnostics::capture168::Domain::Interaction,on,now,"F5");
    if(!on){
        // Recorder OFF must not revoke installed gameplay capability.
        g_interaction_coverage175=g_lifecycle_coverage175.load()|(InteractionActionReady175()?256u:0u);
        return true;
    }
    (void)PrepareInteractionResources59c7f();
    const bool retail=retail_build112::Ready(WhBase());
    const auto coverage=g_interaction_coverage175.load();
    const bool bodyProbe=(coverage&64)!=0,inputProbe=(coverage&128)!=0,animationProbe=(coverage&512)!=0;
    cc::interactionOnlyUntil168r1=now+diagnostics::capture168::Duration;cc::session.fetch_add(1);
    g_lifecycle_ledger165={};g_lifecycle_session165.fetch_add(1);g_lifecycle_budget36=0;
    g_lifecycle_until165=now+diagnostics::capture168::Duration;
    char m[560]{};std::snprintf(m,sizeof(m),
        "V66.175 INTERACTION_COVERAGE state=BEGIN mask=%u optionalMask=%u action=%d body=%d input=%d animation=%d partialCapture=ALLOWED failedGroups=NO_RETRY_THIS_INSTALL durationMs=180000 gameplayModes=UNCHANGED writes=NONE",
        coverage,g_lifecycle_coverage175.load(),InteractionActionReady175(),bodyProbe,inputProbe,animationProbe);
    diagnostic_trace::Write(m);
    for(unsigned gi=0;gi<6;++gi){char group[256]{};
        const bool ready=(g_lifecycle_coverage175.load()&(1u<<gi))!=0;
        std::snprintf(group,sizeof(group),"V66.178R36C3 F5_GROUP session=%llu group=%s ready=%d reason=%u failedRva=%llX failedStage=%s gameplayRequired=%d",
            (unsigned long long)g_lifecycle_session165.load(),diagnostics::optional175::lifecycle[gi].name,ready,retail?g_lifecycle_failure36[gi]:1,
            (unsigned long long)g_lifecycle_failure_siteH17[gi],g_lifecycle_failure_stageH17[gi],gi==0?1:0);
        diagnostic_trace::Write(group);
    }
    const bool attach=g_item_attach_operation_original!=nullptr;
    const bool capable=InteractionLifecycleReady36(),complete=InteractionRecorderCompleteH17();
    char detail[700]{};std::snprintf(detail,sizeof(detail),
        "V66.178R59.6C2 F5_CAPTURE session=%llu ready=%d gameplayReady=%d attach=%d requiredMask=%u recorderMask=%u missingRecorder=%u xrEdges=INSTRUMENTED handCensus=INSTRUMENTED capacity=4096 durationMs=180000 stageEvidence=NATIVE_EVENTS physicalHold=RUNG2_RIGID_PICKUP_EXPERIMENT cleanup=RETURN_NOT_DESTRUCTION instruction=%s",
        (unsigned long long)g_lifecycle_session165.load(),complete,capable,attach,kInteractionRequiredH17,kInteractionRecorderH17,
        kInteractionRecorderH17&~coverage,complete?"ONE_OBJECT_LIFECYCLE":"PARTIAL_RECORDER_FAILED_GROUPS_EXPLICIT");
    diagnostic_trace::Write(detail);return capable;
}
static bool PrepareInteractionResources59c7f() noexcept {
    const bool retail=WhBase()&&(retail_build112::Ready(WhBase())||retail_build112::Verify(WhBase()));
    if(retail)(void)ArmLifecycle165();
    const bool bodyProbe=retail&&ArmInteractionBody122();
    const bool inputProbe=retail&&ArmCombatInput101(false);
    const bool animationProbe=retail&&(animation_filter::Ready()||animation_filter::Install());
    const auto coverage=g_lifecycle_coverage175.load()|(bodyProbe?64u:0u)|(inputProbe?128u:0u)|
        (InteractionActionReady175()?256u:0u)|(animationProbe?512u:0u);
    g_interaction_coverage175=coverage;
    return InteractionLifecycleReady36();
}
int HandsMode59c7f() noexcept {return kcd2vr::hands59c7f::rung.load();}
bool SetHandsMode59c7f(int rung) noexcept {
    using namespace kcd2vr;
    hands59c7f::clear();hands59c7f::rung=std::clamp(rung,0,2);
    // D48: F10 selects hand behavior; F5/F7 select interaction/VRIK recording.
    bool ok=true;
    if(rung==2){
        // Select only the extra hand arbitration; preserve normal pickup,
        // holster and combat ownership when entering or leaving this mode.
        ok=ApplyUserMode175(user175::Mode::Hands);
        hands59c7f::admitted=ok;
        hands59c7f::pickupReady=ok&&PrepareInteractionResources59c7f();
    } // OFF clears this hand experiment only; normal handling remains active.
    char m[320]{};std::snprintf(m,sizeof(m),"C7F F10_MODE rung=%d admitted=%d pickupReady=%d recorder=%s fingers=SURFACE_PROBES rigidFingerBodies=UNIMPLEMENTED",rung,ok,hands59c7f::pickupReady.load(),rung==1?"HANDS_ONLY_180S":"OFF");
    diagnostic_trace::Write(m);return ok;
}
