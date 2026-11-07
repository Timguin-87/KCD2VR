// C55: include the actual publisher, grace, reader and validity functions.
// The native acquisition service is a copied input/output boundary here; its
// actual controller/Use/Attach implementation is tested by native_grasp_owner13.
#include "../src/frik_pose_probe.cpp"
#include <cassert>
#include <cstdio>
#include <limits>

namespace kcd2vr::diagnostic_trace { void Write(const char*) noexcept {} }
namespace kcd2vr::action_time_owner59c11 {extern bool pickupHoldRequested62,pickupExperimentRequested62;}
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
using namespace kcd2vr::frik_pose_probe;
namespace ng55=kcd2vr::grasp13;
namespace track55=kcd2vr::tracking59c6;
static ng55::AcquisitionInput55 received55{};
static ng55::Acquisition54 receipt55{};
static bool enabled55=true,live55=false,replaceBody55=false,f5OwnedCapabilityH17=false;
static unsigned publications55=0;
static bool enabledRead55() noexcept {
    // The native-owner leaf is substituted in this full-frik TU; its real
    // mode/fault/current-Use and Attach lifecycle are in native_grasp_owner13.
    // Keep this leaf connected to the actual public F5 request and wrist gate.
    if(!enabled55)return false;
    return !f5OwnedCapabilityH17||(kcd2vr::action_time_owner59c11::pickupExperimentRequested62&&
        kcd2vr::workstation178r20c1::experimentArmed36.load());
}
static void inputWrite55(const ng55::AcquisitionInput55& input) noexcept {
    received55=input;++publications55;
    if(replaceBody55)g_pose_epoch6689.fetch_add(1);
}
static bool acquisitionRead55(ng55::Acquisition54& out) noexcept {out=receipt55;return live55;}

int main() {
    hostTickCount64=10000;g_census_mode=3;g_mode=0;g_cal_state=2;
    // Actual UI/F5 setters publish requests to the native owner, rather than
    // just storing a menu enum or editing a CSV the owner does not consult.
    namespace owner62=kcd2vr::action_time_owner59c11;
    SetGrabMode60(2);assert(GrabMode60()==2&&owner62::pickupHoldRequested62);
    SetGrabMode60(0);assert(GrabMode60()==0&&!owner62::pickupHoldRequested62);
    SetGrabMode60(1);assert(GrabMode60()==0&&!owner62::pickupHoldRequested62); // unsafe instant mode is unavailable
    hostModuleHandleW=reinterpret_cast<void*>(0x180000000ull);g_wh_pinned104=0;
    kcd2vr::retail_build112::hostRetailVerified112=true;
    g_interaction_coverage175=0x3DF; // H15 observer failure cannot block the actual pickup owner
    const auto oldAttach62=g_item_attach_operation_original;
    g_item_attach_operation_original=reinterpret_cast<decltype(g_item_attach_operation_original)>(0x1234); // presence only; never called
    kcd2vr::combat_rules178r10::workstationEnabled=true;
    for(int rung:{1,2,3}){
      kcd2vr::combat_rules178r10::f5Rung=rung;
      const bool accepted=SetInteractionExperiment36(true);
      assert(accepted==(rung>=2)&&owner62::pickupExperimentRequested62==(rung>=2));
      assert(SetInteractionExperiment36(false)&&!owner62::pickupExperimentRequested62);
    }
    kcd2vr::combat_rules178r10::f5Rung=0;g_item_attach_operation_original=oldAttach62;
    g_interaction_coverage175=0;hostModuleHandleW=nullptr;
    kcd2vr::retail_build112::hostRetailVerified112=false;
    g_pose_epoch6689=7;track55::epoch=9;track55::enabled=false;
    g_committed_character6689=reinterpret_cast<void*>(0x1234);
    g_henry_char_attachment_manager=reinterpret_cast<void*>(0x123c);
    g_dialogue_body178r9=false;kcd2vr::reading_pose32::blocked=true;
    g_graspEnabled13=&enabledRead55;g_graspInput55=&inputWrite55;
    g_graspAcquisition54=&acquisitionRead55;
    GameStateContext context55{};PublishGameStateContext(context55);
    TrackingPose head{},left{},right{};
    head.py=1.7f;left.px=-.25f;left.py=1.2f;right.px=.25f;right.py=1.2f;
    head.qw=left.qw=right.qw=1.f;
    float leftGrip=.7f,rightGrip=.8f;
    bool hv=true,lv=true,rv=true;
    std::uint64_t tick=hostTickCount64,ns=track55::NowNs(),epoch=9;
    auto publish=[&](bool gameplay=false) {
        PublishTrackingSnapshot(head,hv,left,lv,right,rv,leftGrip,rightGrip,.2f,.3f,
            gameplay,tick,ns,epoch);
    };
    auto next=[&]() {++hostTickCount64;tick=hostTickCount64;ns=track55::NowNs();receipt55.tick=hostTickCount64;
        PublishGameStateContext(context55);};
    auto noPose=[&]() {
        g_committed_pose_valid6689=true;publish();
        assert(!g_committed_pose_valid6689.load()&&!g_head_valid_tick.load());
        assert(!g_gameplay_allowed.load()&&!kcd2vr::haptics101::allowed.load());
    };

    // H17 joined capability -> public native-owner request -> actual raw XR
    // producer. H15 mask0x3DF cannot deadlock acquisition behind a read-only
    // SetPhysicParams recorder failure. This is data, never a pose/hit grant.
    f5OwnedCapabilityH17=true;publications55=0;
    hostModuleHandleW=reinterpret_cast<void*>(0x180000000ull);g_wh_pinned104=0;
    kcd2vr::retail_build112::hostRetailVerified112=true;
    g_interaction_coverage175=0x3DF;
    g_item_attach_operation_original=reinterpret_cast<decltype(g_item_attach_operation_original)>(0x1234);
    kcd2vr::combat_rules178r10::f5Rung=2;
    assert(SetInteractionExperiment36(false));publish(false);assert(publications55==1&&!enabledRead55());
    assert(SetInteractionExperiment36(true));publish(false);
    assert(publications55==2&&received55.headTracked&&received55.tracked[0]&&received55.tick==tick&&received55.sample==ns);
    assert(!InteractionRecorderCompleteH17()&&!g_gameplay_allowed.load());
    // A lost required callback closes F5 ownership, not observation of raw input.
    g_interaction_coverage175=0x3DF&~1u;ServiceInteractionExperiment36();publish(false);
    assert(publications55==3&&!kcd2vr::action_time_owner59c11::pickupExperimentRequested62&&!enabledRead55());
    g_interaction_coverage175=0x3DF;assert(SetInteractionExperiment36(true));
    rightGrip=0;publish(false);assert(publications55==4&&received55.tracked[0]&&received55.grip[0]==0);rightGrip=.8f;
    assert(SetInteractionExperiment36(false));f5OwnedCapabilityH17=false;publications55=0;
    g_item_attach_operation_original=nullptr;g_interaction_coverage175=0;
    hostModuleHandleW=nullptr;kcd2vr::retail_build112::hostRetailVerified112=false;
    kcd2vr::combat_rules178r10::f5Rung=0;

    // Native Picking disables gameplay BEFORE a real attachment/held-prop
    // owner exists. Raw input must still reach the owner, with original stamps.
    kcd2vr::animation_filter::combatPoseTick112=9999;
    noPose();
    assert(publications55==1&&received55.headTracked&&received55.tracked[0]&&received55.tracked[1]);
    assert(received55.character==0x1234&&received55.epoch==7&&received55.trackingEpoch==9);
    assert(received55.tick==tick&&received55.sample==ns&&received55.grip[0]==rightGrip&&received55.grip[1]==leftGrip);
    assert(!kcd2vr::animation_filter::combatPoseTick112.load());
    TrackingSnapshot snapshot{};assert(ReadTrackingSnapshot(snapshot)&&!snapshot.allowed&&!snapshot.head_valid);

    // Repeating a source cannot fabricate a new acquisition sample or renew age.
    const auto original=received55;hostTickCount64+=20;publish();
    assert(received55.sample==original.sample&&received55.tick==original.tick);
    // Queued/pending ownership is data capture only, never visual permission.
    receipt55.character=0x1234;receipt55.epoch=7;receipt55.action=0x2345;
    receipt55.serial=18;receipt55.startedAt=10000;receipt55.tick=hostTickCount64;
    receipt55.nativeToken=23;receipt55.side=0;receipt55.generation55=2;receipt55.trackingEpoch55=9;
    live55=true;next();noPose();

    // Only the actual installed action receipt admits its current pose. The
    // GENERIC reader still has allowed=false; only the typed Picking consumer
    // can use these current coordinates. No held/grace input is manufactured.
    receipt55.active=true;receipt55.scope=0x3456;next();
    g_committed_pose_valid6689=true;publish();
    assert(g_committed_pose_valid6689.load()&&g_head_valid_tick.load()==tick);
    assert(g_right_valid_tick.load()==tick&&g_left_valid_tick.load()==tick);
    assert(TrackingHeadUsable111()&&TrackingHandUsable111(0));
    assert(ReadTrackingSnapshot(snapshot)&&!snapshot.allowed&&snapshot.head_valid&&snapshot.right_valid);
    assert(!snapshot.head_held&&!snapshot.right_held&&snapshot.head.y==head.py&&snapshot.right.x==right.px);
    assert(!g_gameplay_allowed.load()&&!kcd2vr::haptics101::allowed.load()&&!kcd2vr::animation_filter::combatPoseTick112.load());

    // Original physical release/tracking failure is visible before pose grace.
    next();rightGrip=0;noPose();assert(received55.tracked[0]&&received55.grip[0]==0);rightGrip=.8f;
    next();rv=false;noPose();assert(!received55.tracked[0]);rv=true;
    next();hv=false;noPose();assert(!received55.headTracked);hv=true;
    next();rightGrip=std::numeric_limits<float>::quiet_NaN();noPose();assert(!received55.tracked[0]);rightGrip=.8f;
    next();right.qw=0;noPose();assert(!received55.tracked[0]);right.qw=1;
    // An unrelated lost hand cannot manufacture a valid owned side.
    next();receipt55.side=1;lv=false;noPose();lv=true;receipt55.side=0;

    // Age comes from sourceTick, not the publisher's fallback-to-now. The head
    // keeps its 60 ms boundary and each hand its own 80 ms input boundary.
    next();tick=hostTickCount64-60;g_committed_pose_valid6689=true;publish();
    assert(received55.headTracked&&g_committed_pose_valid6689.load());
    tick=hostTickCount64-61;noPose();assert(!received55.headTracked&&received55.tracked[0]);
    tick=hostTickCount64-80;noPose();assert(received55.tracked[0]);
    tick=hostTickCount64-81;noPose();assert(!received55.tracked[0]);
    next();tick=0;noPose();assert(!received55.headTracked&&received55.tick==0);
    next();ns=0;noPose();assert(!received55.headTracked&&received55.sample==0);
    next();tick=hostTickCount64+1;noPose();assert(!received55.headTracked);
    next();ns=track55::NowNs()+1000000000ull;noPose();assert(!received55.headTracked);
    next();epoch=0;noPose();assert(!received55.headTracked);
    next();epoch=8;noPose();assert(!received55.headTracked);epoch=9;

    // The owner receipt cannot cross body/source sessions or become permission
    // for menus, reading, crafting, protected native body, calibration or OFF.
    next();receipt55.character=0x9999;noPose();receipt55.character=0x1234;
    next();receipt55.epoch=6;noPose();receipt55.epoch=7;
    next();receipt55.trackingEpoch55=8;noPose();receipt55.trackingEpoch55=9;
    next();receipt55.scope=0;noPose();receipt55.scope=0x3456;
    next();receipt55.side=-1;noPose();receipt55.side=0;
    next();receipt55.tick=hostTickCount64-81;noPose();
    next();g_henry_char_attachment_manager=reinterpret_cast<void*>(0x9999);noPose();assert(!received55.headTracked);
    g_henry_char_attachment_manager=reinterpret_cast<void*>(0x123c);
    next();replaceBody55=true;noPose();replaceBody55=false;g_pose_epoch6689=7;
    for(auto* context:{&g_ctx_menu,&g_ctx_dialogue,&g_ctx_cutscene,&g_ctx_reading}){
        next();*context=true;noPose();assert(!received55.headTracked);*context=false;
    }
    next();context55.tool_pose_ui_blocked=true;PublishGameStateContext(context55);
    assert(!g_ctx_menu.load());noPose();assert(!received55.headTracked);context55.tool_pose_ui_blocked=false;
    next();g_tool_context19c4r2=((hostTickCount64-81)<<8);noPose();assert(!received55.headTracked);
    next();kcd2vr::activity_input163::witness175.ready=true;
    kcd2vr::activity_input163::witness175.Begin(kcd2vr::activity_input163::Kind175::Alchemy,0x4567);
    noPose();assert(!received55.headTracked);kcd2vr::activity_input163::witness175.Clear();
    next();kcd2vr::animation_filter::nativeBodyOwned112=true;noPose();assert(!received55.headTracked);
    kcd2vr::animation_filter::nativeBodyOwned112=false;
    next();g_cal_state=0;noPose();g_cal_state=2;
    next();g_census_mode=0;noPose();g_census_mode=3;
    next();enabled55=false;const auto count=publications55;noPose();assert(publications55==count+1&&received55.tick==tick);enabled55=true;

    // Existing ordinary gameplay behavior remains on its original permission.
    next();live55=false;publish(true);
    assert(ReadTrackingSnapshot(snapshot)&&snapshot.allowed&&g_gameplay_allowed.load());
    assert(kcd2vr::haptics101::allowed.load());
    std::puts("PASS C55 actual tracking ingress: pre-Attach raw publication without a body frame; original source/sample/side/grip; exact installed Picking preserves current visual coordinates only; generic gameplay stays off; release, invalid/grace, source age/session, body replacement, modal/native owner and OFF refusal");
}
