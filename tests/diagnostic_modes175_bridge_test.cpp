// Execute current public recorder/mode APIs from the complete production TU.
// Native installers are explicit fail-closed external substitutes; no retail
// code, loaded-image offsets, or game objects are executed in this fixture.
#include "../src/frik_pose_probe.cpp"
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace {
std::string snapshot175;
unsigned snapshots175=0;std::string f5Coverage175,f5GateH17;
void Write(const char* s) noexcept {
    if(s&&std::strstr(s,"F5_CAPTURE"))f5Coverage175=s;
    if(s&&std::strstr(s,"F5_GATE"))f5GateH17=s;
    if(s&&std::strstr(s,"F11-RELOAD SNAPSHOT")){snapshot175=s;++snapshots175;}
}
}
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
using namespace kcd2vr::frik_pose_probe;
namespace cap175=kcd2vr::diagnostics::capture168;
static auto Modes175() { return std::array<int,4>{HolsterCombatDiagMode(),MiscInteractionDiagMode(),CombatCensusMode(),WeaponControlMode()}; }
int main() {
    hostModuleHandleW=nullptr;
    g_saved_arm_ready66100=true;
    g_census_mode=5;
    kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,3);
    kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F5,2);
    const auto original=Modes175();
    const auto poseEpoch=g_pose_epoch6689.load();
    const auto weaponEpoch=g_weapon_request_epoch6694.load();
    assert(SetReloadMode47C2(1)&&ReloadMode47C2()==1&&ReloadDiagnostics175());
    assert(!SetReloadMode47C2(2)&&ReloadMode47C2()==1); // no supported module: fail closed
    assert(SetReloadMode47C2(0)&&ReloadMode47C2()==0&&!ReloadDiagnostics175());
    assert(g_weapon_request_epoch6694.load()==weaponEpoch); // observer and refused staging do not reseed the bow
    assert(Modes175()==original&&g_pose_epoch6689.load()==poseEpoch);
    assert(SetHandsMode59c7f(1)&&HandsMode59c7f()==1);
    assert(cap175::active(cap175::Domain::Hands,hostTickCount64)&&Modes175()==original);
    assert(!cap175::active(cap175::Domain::Combat,hostTickCount64));
    assert(SetHandsMode59c7f(0)&&Modes175()==original);
    for (unsigned repeat=0;repeat<3;++repeat) {
        assert(SetReloadDiagnostics175(true)&&ReloadDiagnostics175());
        assert(Modes175()==original&&g_pose_epoch6689.load()==poseEpoch&&CalibrationReady());
        assert(!SetInteractionLifecycle165(true)&&InteractionLifecycleActive168());
        assert(kcd2vr::diagnostic_trace::f5Coverage175.find("ready=0")!=std::string::npos);
        assert(kcd2vr::diagnostic_trace::f5Coverage175.find("PARTIAL_RECORDER_FAILED_GROUPS_EXPLICIT")!=std::string::npos);
        assert(InteractionCoverage175()==0); // unavailable native groups keep partial recorder alive
        assert(Modes175()==original&&g_pose_epoch6689.load()==poseEpoch);
        assert(SetReloadDiagnostics175(false)&&!ReloadDiagnostics175());
        assert(InteractionLifecycleActive168()&&Modes175()==original);
        assert(SetInteractionLifecycle165(false)&&!InteractionLifecycleActive168());
        assert(Modes175()==original&&CalibrationReady());
    }
    // Execute the real F11 worker against copied publications: no native pointer
    // reads or mutation of the native-owner complete-frame cache is permitted.
    assert(SetReloadDiagnostics175(true));
    PhysFrame6698 frame{};frame.input.sample=7;frame.input.bodyEpoch=g_pose_epoch6689.load();
    frame.request=g_weapon_request_epoch6694.load();frame.profiles[0].key=123;
    frame.output.bowHeld=true;frame.output.bowFraction=.6;
    g_phys_native_complete175.input.sample=991;
    auto snapshot=[&](std::uint64_t ready,std::int64_t age=0) {
        hostTickCount64+=200;frame.input.tick=hostTickCount64-age;
        g_phys_frame6698.publish(frame);g_phys_ready_sample6698=ready;
        kcd2vr::diagnostic_trace::snapshot175.clear();ServiceReloadDiagnostics175();
        assert(g_phys_native_complete175.input.sample==991);
        return kcd2vr::diagnostic_trace::snapshot175;
    };
    auto uncommitted=snapshot(6);
    assert(uncommitted.find("frameFresh=0 physSample=0")!=std::string::npos);
    auto committed=snapshot(7);
    assert(committed.find("frameFresh=1 physSample=7")!=std::string::npos);
    assert(committed.find("ammoPresence=UNKNOWN_UNTIL_NATIVE_EVIDENCE")!=std::string::npos);
    auto count=kcd2vr::diagnostic_trace::snapshots175;
    hostTickCount64+=50;ServiceReloadDiagnostics175();
    assert(kcd2vr::diagnostic_trace::snapshots175==count); // real 5 Hz bound
    assert(snapshot(7,101).find("frameFresh=0")!=std::string::npos);
    assert(snapshot(7,-1).find("frameFresh=0")!=std::string::npos);
    ++frame.request;assert(snapshot(7).find("frameFresh=0")!=std::string::npos);
    --frame.request;++frame.input.bodyEpoch;
    assert(snapshot(7).find("frameFresh=0")!=std::string::npos);
    --frame.input.bodyEpoch;assert(snapshot(0).find("frameFresh=0")!=std::string::npos);
    assert(SetReloadDiagnostics175(false));count=kcd2vr::diagnostic_trace::snapshots175;
    hostTickCount64+=200;ServiceReloadDiagnostics175();
    assert(kcd2vr::diagnostic_trace::snapshots175==count);
    assert(Modes175()==original&&g_pose_epoch6689.load()==poseEpoch&&CalibrationReady());
    // Actual production F5 admission: no installation, calibration or VRIK
    // enablement as a side effect. Supply only availability leaves for success.
    namespace ws=kcd2vr::workstation178r20c1;
    namespace rules=kcd2vr::combat_rules178r10;
    assert(!SetInteractionLifecycle165(true));rules::f5Rung=2;
    assert(!SetInteractionExperiment36(true)&&!ws::ownsWrist(hostTickCount64));
    const auto oldModuleH17=hostModuleHandleW;
    hostModuleHandleW=reinterpret_cast<void*>(0x180000000ull);g_wh_pinned104=0;
    kcd2vr::retail_build112::hostRetailVerified112=true;
    g_interaction_coverage175=0x17F;
    const auto oldAttach=g_item_attach_operation_original;
    g_item_attach_operation_original=+[](void*,void*,int)->bool{return false;};
    // H17: actual H15 failed only the optional SetPhysicParams observer. F5
    // must admit its proven pickup/Use capability without inventing coverage.
    for(unsigned coverageH17:{0x17Fu,0x3DFu,0x101u}){
        g_interaction_coverage175=coverageH17;
        assert(SetInteractionExperiment36(true)&&ws::ownsWrist(hostTickCount64));
        assert(InteractionRecorderCompleteH17()==((coverageH17&0x17F)==0x17F));
        assert(kcd2vr::diagnostic_trace::f5GateH17.find("armed=1")!=std::string::npos);
        assert(kcd2vr::diagnostic_trace::f5GateH17.find("missingRequired=0")!=std::string::npos);
        assert(SetInteractionExperiment36(false));
    }
    for(unsigned missingH17:{1u,256u}){
        g_interaction_coverage175=0x3FFu&~missingH17;
        assert(!SetInteractionExperiment36(true)&&!ws::experimentArmed36);
    }
    // Native installer execution is intentionally absent from this bridge.
    // The real H15 failed-observer mask remains visible and unmodified.
    g_interaction_coverage175=0x3DF;
    g_item_attach_operation_original=nullptr;
    assert(!SetInteractionExperiment36(true)&&!InteractionRecorderCompleteH17());
    g_item_attach_operation_original=+[](void*,void*,int)->bool{return false;};
    kcd2vr::retail_build112::hostRetailVerified112=false;
    assert(!SetInteractionExperiment36(true)&&!InteractionRecorderCompleteH17()); // stale coverage never grants a retail mismatch
    kcd2vr::retail_build112::hostRetailVerified112=true;
    assert(SetInteractionExperiment36(true)&&ws::ownsWrist(hostTickCount64));
    rules::useHeld=true;rules::strokeUntil=hostTickCount64+140;
    g_census_mode=0;ServiceInteractionExperiment36();
    assert(rules::f5Rung==2&&!ws::experimentArmed36&&!rules::useHeld&&!rules::strokeUntil);
    g_census_mode=5;ServiceInteractionExperiment36();assert(!ws::experimentArmed36); // explicit re-arm only
    assert(SetInteractionExperiment36(true));g_saved_arm_ready66100=false;g_cal_state=0;
    ServiceInteractionExperiment36();assert(!ws::experimentArmed36);
    g_saved_arm_ready66100=true;assert(SetInteractionExperiment36(true));
    hostTickCount64=cap175::gate.until[unsigned(cap175::Domain::Interaction)].load();
    assert(ws::ownsWrist(hostTickCount64));ServiceInteractionExperiment36();
    assert(rules::f5Rung==2&&ws::experimentArmed36&&!InteractionLifecycleActive168());
    assert(SetInteractionExperiment36(false)&&!ws::ownsWrist(hostTickCount64));
    assert(!rules::useHeld&&!rules::strokeUntil);
    // Recorder-only F5 still returns to OFF at its deadline.
    rules::f5Rung=1;ServiceInteractionExperiment36();
    assert(rules::f5Rung==0&&!ws::experimentArmed36);
    g_item_attach_operation_original=oldAttach;
    hostModuleHandleW=oldModuleH17;kcd2vr::retail_build112::hostRetailVerified112=false;
    for(unsigned i=0;i<cap175::DomainCount;++i)
        cap175::gate.select(cap175::Domain(i),true,hostTickCount64);
    const auto captureMask=cap175::gate.mask.load();
    g_combat_experiment175=true;
    g_combat_integrated178r22c2=true;g_physical_combat177=true;g_combat_full178r10=true;g_combat_rung178r15=6;
    kcd2vr::vr_combat111::hideNativeClipsActive178r28=true;
    ws::setExperiment36(true);rules::f5Rung=2;rules::useHeld=true;rules::strokeUntil=hostTickCount64+140;
    g_saved_arm_ready66100=false;g_cal_state=0; // OFF must work even after calibration becomes invalid
    g_special_pending168=true;
    g_phys_input6698=true;
    assert(ApplyUserMode175(kcd2vr::user175::Mode::Off));
    assert((Modes175()==std::array<int,4>{0,0,0,0}));
    assert(!CombatExperiment175Enabled()&&!g_special_pending168&&!g_phys_input6698);
    assert(!g_combat_integrated178r22c2&&!g_physical_combat177&&!g_combat_full178r10&&g_combat_rung178r15==0);
    assert(!kcd2vr::vr_combat111::hideNativeClipsActive178r28&&!ws::experimentArmed36&&!rules::useHeld&&!rules::strokeUntil);
    assert(cap175::gate.mask.load()==captureMask&&!CalibrationReady());
    // Even without calibration, OFF succeeds; a new gameplay mode is rejected
    // before hooks/native prerequisites and cannot disturb live recorders.
    g_saved_arm_ready66100=false;g_cal_state=0;
    assert(ApplyUserMode175(kcd2vr::user175::Mode::Off));
    assert(!ApplyUserMode175(kcd2vr::user175::Mode::Hands));
    assert((Modes175()==std::array<int,4>{0,0,0,0}));
    assert(cap175::gate.mask.load()==captureMask);
    // C59 actual public policy API: resources below represent an independently
    // qualified installation. No native hook/function is executed by this host.
    SetStagedBowDefault59(false);g_staged_bow_ready59=false;g_reload_ready36=false;
    const auto unpreparedEpoch59=g_weapon_request_epoch6694.load();
    SetStagedBowDefault59(true);
    assert(!PrepareStagedBowDefault59()&&!StagedBowEnabled59()&&g_weapon_request_epoch6694.load()==unpreparedEpoch59);
    cap175::gate.select(cap175::Domain::Reload,false,hostTickCount64);
    g_reload_mode47c2=0;g_reload_ready36=true;g_staged_bow_ready59=true;
    assert(StagedBowEnabled59()&&!ReloadDiagnostics175()&&BowGripDrawEnabled596C1());
    const auto beforePreference59=g_weapon_request_epoch6694.load();
    SetBowGripDrawDefault596C1(false);
    assert(BowGripDrawEnabled596C1()&&g_weapon_request_epoch6694.load()==beforePreference59);
    SetBowGripDrawDefault596C1(true);
    assert(g_weapon_request_epoch6694.load()==beforePreference59);
    const auto stagedEpoch59=g_weapon_request_epoch6694.load();
    assert(SetReloadMode47C2(1)&&SetReloadMode47C2(0));
    assert(StagedBowEnabled59()&&!ReloadDiagnostics175()&&g_weapon_request_epoch6694.load()==stagedEpoch59);
    SetStagedBowDefault59(false);assert(!StagedBowEnabled59()&&g_weapon_request_epoch6694.load()==stagedEpoch59+1);
    SetStagedBowDefault59(false);assert(g_weapon_request_epoch6694.load()==stagedEpoch59+1);
    SetStagedBowDefault59(true);assert(StagedBowEnabled59()&&g_weapon_request_epoch6694.load()==stagedEpoch59+2);
    assert(!ReloadDiagnostics175());
    std::puts("PASS complete production recorder APIs and C59 independent staged default/opt-out/epoch policy; native leaves synthetic");
}
