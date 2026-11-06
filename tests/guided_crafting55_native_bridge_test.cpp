// Connected mode -> actual Mod pointer/lock -> actual offered-candidate
// publisher/getter -> current native attachment/body -> committed Dock ->
// production final pad/work ledger. Native memory leaves are explicit providers;
// actual child scan/Attach/clock/Use is independently exercised by alchemy_semi46_native.
#define KCD2VR_NATIVE_ACTIVITY_NO_MAIN
#include "native_activity_hands46_test.cpp"
#include "../src/vrik/semi_workstations46.h"
#include "../src/vrik/hand_point123r1.h"
#include "../src/vrik/alchemy_palm_lock27.h"
#include "../src/core/door_use46.h"
#include <mutex>
namespace kcd2vr::interaction_alignment {
using RaySample=interaction112::Ray;
static std::uint64_t g_door_generation38=8;
static std::atomic<bool> g_enabled{true},g_split_ready57c1{true},g_hand_ray_active{true},g_visual_fallback_enabled{false};
static std::mutex g_hand_candidate_mutex178r14;static HandCandidate g_hand_candidate178r14;
static core::SnapshotMailbox<RaySample> g_hand_packets112;
static core::SnapshotMailbox<palm59c9::Pair> g_palm_packets59c9;
struct Candidate57c1 {std::uint32_t id=0;char cls[32]{};};
#include "generated_guided_candidate55.inc"
bool DoorSourceCurrent38(std::uint64_t) noexcept{return true;} // no ordinary door branch in this alchemy test
}
int main(){
    NativeActivityScenario46();
    namespace ai=kcd2vr::activity_input163;namespace ws=kcd2vr::workstation178r20c1;
    namespace mh=kcd2vr::minigame_hands45;namespace gc=kcd2vr::guided_crafting55;
    namespace ia=kcd2vr::interaction_alignment;namespace as=kcd2vr::alchemy_semi46;
    using namespace kcd2vr;
    now=4000;auto& w=ai::witness175;w.Clear();w.ready=true;w.Begin(ai::Kind175::Alchemy,reinterpret_cast<std::uintptr_t>(alchemy.data()),now);
    ai::trackedHands59c7f[0]=true;alchemyActive=true;smithActive37=false;
    combat_rules178r10::workstationEnabled=true;combat_rules178r10::f5Rung=3;ws::setExperiment36(true);
    ws::alchemyPassenger42=true;ws::alchemyHandRay59c2=true;mh::mode=mh::Native;mh::publish(mh::Alchemy,now);
    ws::heldToolActivity59c10=int(ws::Kind::Alchemy);ws::heldToolActivityTick59c10=now;
    bool body=false;(void)ToolContext19c4r2(true,false,false,false,false,false,false,false,&body);
    assert(body);g_dialogue_body178r9=body;
    assert(ObserveNativeActivityHand46(self.data(),attachment.data()));
    const auto instance=reinterpret_cast<std::uintptr_t>(alchemy.data()),ch=reinterpret_cast<std::uintptr_t>(character.data());
    workstation_sample46::Frame f{};f.allowed=f.headValid=true;f.tick=now;f.sequence=1;f.timeNs=now*1000000;f.poseEpoch=7;f.sourceEpoch=9;f.head[1]=1.7f;
    for(auto& h:f.hand){h.valid=true;h.position[1]=1.2f;}
    CommittedFrame6695 packet{};packet.stamp={7,1,now,ch};packet.valid[0]=packet.propRequestedValid5916c1[0]=true;
    packet.native[0].t[0]=.2f;packet.propRequested5916c1[0].t[0]=.25f;
    PublishGuidedTools55(packet,9);gc::Dock dock{};
    assert(gc::dock[0].read(dock)&&dock.entity==42&&dock.object==reinterpret_cast<std::uintptr_t>(self.data()));
    bool interaction_alignment_installed=true,safe_ui_fit=false,f2_ui_menu_active=false,matrix_active=false;
    struct {bool nativeControls=true;} activity163;
    struct {bool BlocksWorldInteraction()const{return false;}bool NativeContextControls()const{return true;}} ui89_policy;
    struct Controller {bool active=true;float grip=0,trigger=0;};
    struct XR {Controller hands[2];const Controller& RightController(){return hands[0];}const Controller& LeftController(){return hands[1];}} xr;
    auto* xr_manager_=&xr;bool pointerEnabled32=true;
    unsigned f5Mode=3,reserved114=0,gripMask116=0;int itemState116[2]{0,0};
    std::uint64_t ladder_now_ms=now;hand_point123r1::State point123;
    alchemy_input19c4::PalmLock27 alchemyPalmLock27;
    struct {bool ray_active=false;int ray_owner=-1;} po;
    interaction_alignment::HandCandidate candidate59c7f{};
    auto buildPalm59c9=[&](int side,interaction112::Ray& ray){ray={};ray.owner=side;ray.hand=true;ray.tick=now;
        ray.limit=2;ray.radius=.1f;ray.y=1;ray.ox=side?-1.f:0.f;ray.sourceEpoch=7;ray.sourcePlayer=reinterpret_cast<std::uintptr_t>(alchemyPlayer.data());return true;};
    auto ray=[&](){
        namespace diagnostics=::diagnostics;
#include "generated_guided_pointer55.inc"
        assert(!alchemyPointer59c2&&alchemyHeadSelection63);
        // C63 guided Alchemy selects by gaze, so no palm candidate is published.
        return interaction_alignment::HandCandidate{};

    };
    semi_workstations46::Context context{};context.enabled=context.qualified=context.guided=context.alchemyTracked=true;
    context.alchemy=instance;context.lifetime=w.lifetime175.load();context.alchemyGaze63=true;
    as::hooksReady=true;as::producer.reset();as::work.publish({});
    semi_workstations46::State state;struct Pad {bool a=false,b=false,x=false;float lx=0,ly=0,rt=0,lt=0;} pad;
    const auto sample=[&](bool held,float x){
        now+=20;ladder_now_ms=now;f.tick=now;++f.sequence;f.timeNs=now*1000000;f.hand[0].grip=held?1.f:0.f;f.hand[0].position[0]=x;xr.hands[0].grip=f.hand[0].grip;
        mh::publish(mh::Alchemy,now);ws::heldToolActivityTick59c10=now;(void)ToolContext19c4r2(true);
        assert(ObserveNativeActivityHand46(self.data(),attachment.data()));
        packet.stamp={7,f.sequence,now,ch};PublishGuidedTools55(packet,9);
        alchemy_input19c4::selected.publish({instance,context.lifetime,now,42});
        alchemy_input19c4::observed[3].publish({instance,context.lifetime,now,42,3,false});
        context.touch=ray();pad={};state.apply(context,f,now,pad);
    };
    sample(false,0);assert(!pad.a);
    sample(true,0);assert(pad.a&&state.lastGuided==gc::Status::NativeActionRequested);
    as::WorkWitness work{};work.instance=instance;work.lifetime=context.lifetime;work.parent=30;work.child=40;work.token=2;
    work.kind=as::Mortar;work.phase=3;work.working=true;work.hands=3;work.tick=now;as::work.publish(work);
    sample(true,0);assert(state.lastGuided==gc::Status::GuidedWork);
    work.tick=now;as::work.publish(work);sample(true,.0032f);
    as::Control control{};assert(as::control.read(control)&&control.enabled&&control.hand==0&&control.totalSeconds>.019);
    // OFF with Grip still held revokes the exact clock and native tool owner;
    // no current mode/selection can silently re-arm it after prerequisite loss.
    ws::setExperiment36(false);pad={};state.apply(context,f,now,pad);
    assert(as::control.read(control)&&!control.enabled&&!pad.a);
    (void)ToolContext19c4r2(true,false,false,false,false,false,false,false,&body);assert(!body);
    PublishGuidedTools55(packet,9);assert(gc::dock[0].read(dock)&&!dock.object);
    std::puts("guided_crafting55_native_bridge: actual Native-setting mode/gaze-selection/body/attachment/Dock/final-pad/work/OFF graph PASS");
}
