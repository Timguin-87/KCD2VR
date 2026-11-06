// Executes the production Draw dispatch block and PhysPrepare profile-ingress
// prefix verbatim, then the complete physical kernel. Native equipment calls,
// grip geometry and platform context are explicit modeled leaves; no game ACK
// is inferred from native return. The later positive held snapshot is supplied.
#include "gesture_recovery_h30_fixture.h"
#include "subsystems/masterstrike_combos/draw_signal_d63.h"
#include <atomic>
namespace dm=kcd2vr::draw_motion_d63;
namespace ds=kcd2vr::draw_signal_d63;
struct PhysEquipmentItem6698 {ph::Profile profile{};bool held=false;};
struct PhysEquipment6698 {
    std::array<PhysEquipmentItem6698,8> items{};int count=1,heldIndex[2]{-1,-1};
    std::uint64_t request=1,epoch=1,tick=0,handKey[2]{},leftNativeMembershipH36=0;
    bool valid=true,handKnown[2]{true,true};
    void* player=reinterpret_cast<void*>(0x300);void* manager=reinterpret_cast<void*>(0x400);
};
struct PhysGrip6698 {std::uint64_t key=0,generation=0;unsigned nativeStamp=0,nativeSocket126=0;};
struct PhysGripBank6698 {std::array<PhysGrip6698,8> grip{};};
struct PhysCorrelation126 {unsigned stamp=0,socket=0;};
static kcd2vr::core::SnapshotMailbox<PhysEquipment6698> g_phys_equipment6698;
static kcd2vr::core::SnapshotMailbox<PhysGripBank6698> g_phys_grips6698;
static std::atomic<bool> g_phys_armed6698{true},g_phys_fault6698{false},g_gameplay_allowed{true};
static std::atomic<int> g_weapon_control_mode{6};
static std::atomic<unsigned> g_physical_gates101R2[2]{},g_phys_relevant_id101[8]{};
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1},g_pose_epoch6689{1},g_left_native_membership_h36{0};
static std::atomic<void*> g_henry_item_attach_manager{reinterpret_cast<void*>(0x400)};
static std::uint64_t nowD63=1000;
static std::uint64_t GetTickCount64(){return nowD63;}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
namespace diagnostics::lab {static bool NativeReference(){return false;}}
namespace diagnostic_trace {static void Write(const char*){}}
static bool NativePassengerCurrentA34(std::uint64_t){return false;}
static bool TrackingHeadUsable111(std::uint64_t){return true;}
static unsigned popsD63=0,enteredCallsD63=0,cancelsD63=0;
static bool enteredD63=true,returnedD63=true;
struct QueueD63 {void pop(ph::Command&){++popsD63;}};
static QueueD63 g_phys_commands6698;
static void PhysCancelNative6698(){++cancelsD63;}
static bool PhysEquipmentCommand6698(const PhysEquipment6698&,const ph::Command&,bool* entered){++enteredCallsD63;*entered=enteredD63;return returnedD63;}
static bool PhysCancelUnenteredEquipmentH36(const PhysEquipment6698&,const ph::Command&){return true;}
static void PhysRejectUnenteredEquipmentD48(const ph::Command&){}
static bool PhysRefreshEquipment103(PhysEquipment6698&,std::uint64_t){return false;}
static bool PhysHolsterCaptureD48(std::uint64_t){return false;}
static void PhysFillProfile6698(const PhysEquipmentItem6698& item,const PhysGrip6698&,ph::Profile& p){p=item.profile;}
#include "draw_ingress_d63.inc"
#include "legacy_draw_transport_d63.inc"
static void dispatch(PhysEquipment6698& e,ph::Command c){const auto now=nowD63;while(true){
#include "draw_dispatch_d63.inc"
break;}}
struct BridgeRig:RigH30 {
    PhysEquipment6698 gear{};
    BridgeRig():RigH30(false){
        gear.items[0].profile=profiles[0];gear.tick=in.tick;nowD63=in.tick;
        PhysGripBank6698 bank{};bank.grip[0].key=profiles[0].key;bank.grip[0].generation=profiles[0].generation;
        g_phys_grips6698.publish(bank);g_phys_equipment6698.publish(gear);
        ds::latest[0].publish({});ds::latest[1].publish({});
        enteredD63=returnedD63=true;popsD63=enteredCallsD63=cancelsD63=0;
    }
    ph::Command command(){ph::Command c{};c.kind=ph::CommandKind::Draw;c.side=0;c.key=gear.items[0].profile.key;
        c.generation=gear.items[0].profile.generation;c.bodyEpoch=in.bodyEpoch;c.epoch=in.epoch;c.sample=in.sample;c.tick=in.tick;
#ifdef KCD2VR_PHYSICAL_DRAW_MOTION_D63
        c.drawSourceD63=ph::draw_source_d63(in,0);
#endif
        return c;}
    void held(){gear.handKey[0]=gear.items[0].profile.key;gear.heldIndex[0]=0;gear.items[0].held=true;}
    bool prepare(){gear.tick=nowD63=in.tick;g_phys_equipment6698.publish(gear);PhysEquipment6698 copy{};return PhysPrepare6698(in,profiles,copy);}
    dm::Ticket ticket(){dm::Ticket t{};(void)ds::latest[0].read(t);return t;}
};
// Replay actual legacy holster admission, original raw latch and extraction,
// then carry its command through actual native dispatch and held ingress.
static ph::Command legacyCommand(BridgeRig& r){
    ph::HolsterState state{};const auto mount=r.in.hands[0].p;
    auto update=[&](){const auto before=state;
        const auto kind=ph::holster(state,r.profiles[0].key,r.in.hands[0].p,mount,r.in.grip[0],false,true,r.in.tick);
        return std::pair{kind,PhysLegacyDrawSourceD63(r.in,0,kind,before,state)};};
    r.in.grip[0]=0;update();r.in.grip[0]=1;update();
    for(unsigned n=0;n<16;++n){
        ++r.in.sample;r.in.tick+=10;r.in.timeNs+=10000000;
        r.in.hands[0].p.y+=.025;r.in.intentHands[0].p.y+=.025;
        const auto [kind,source]=update();
        if(kind==ph::CommandKind::Draw){auto c=r.command();c.drawSourceD63=source;nowD63=r.in.tick;return c;}
    }
    ckH30(false,"legacy fixture must enter real Draw once after a real pull");return {};
}
int main(){
    {BridgeRig r;const auto c=legacyCommand(r);dispatch(r.gear,c);
        ckH30(c.kind==ph::CommandKind::Draw&&c.drawSourceD63.extracted&&enteredCallsD63==1,
            "actual legacy Draw preserves completed extraction through one native dispatch");
        r.held();r.prepare();r.step();
        for(unsigned n=0;n<18;++n)r.step({0,.03,0});
        ckH30(r.hits()+r.count(ph::CommandKind::Prepare)==0,
            "continued legacy extraction cannot become an attack after held ACK");
        for(unsigned n=0;n<6;++n)r.step({0,-.015,0});r.commands.clear();
        for(unsigned n=0;n<14;++n)r.step({0,.03,0});
        ckH30(r.hits()==1,"legacy extracted=true accepts next deliberate stroke after nonquiet reversal without forced pause");}

    {BridgeRig r;const auto c=r.command();dispatch(r.gear,c);const auto t=r.ticket();
        ckH30(popsD63==1&&enteredCallsD63==1&&cancelsD63==1,"actual Draw dispatch remains one native request with the original cancellation boundary");
        ckH30(t.serial&&t.key==c.key&&t.generation==c.generation&&t.body==c.bodyEpoch&&t.request==c.epoch&&t.sample==c.sample&&t.tick==c.tick&&t.side==0,"entered known-empty Draw publishes exact immutable occurrence");
        ckH30(r.prepare()&&!r.profiles[0].primary&&!r.profiles[0].key,"native return does not manufacture held-item geometry or ACK");
        for(unsigned n=0;n<12;++n)r.step();ckH30(r.hits()==0,"unacknowledged Draw cannot produce an attack through empty equipment");
        r.held();ckH30(r.prepare()&&r.profiles[0].primary&&r.profiles[0].key==c.key,"later positively held same-item snapshot admits current primary geometry");
        r.step();for(unsigned n=0;n<20;++n)r.step();for(unsigned n=0;n<18;++n)r.step({0,.03,0});
        ckH30(r.hits()+r.count(ph::CommandKind::Prepare)==0,"actual dispatch to current held ingress to kernel keeps extraction handling-only");
        for(unsigned n=0;n<8;++n)r.step();r.commands.clear();for(unsigned n=0;n<14;++n)r.step({0,.03,0});
        ckH30(r.hits()==1,"same bridge permits next deliberate stroke after distinct settle");}
    {BridgeRig r;enteredD63=returnedD63=false;dispatch(r.gear,r.command());
        ckH30(!r.ticket().serial,"unentered native refusal publishes no draw owner");
        r.held();r.prepare();for(unsigned n=0;n<14;++n)r.step({0,.03,0});
        ckH30(r.hits()==1,"a refused draw cannot permanently disarm an independently held weapon");}
    {BridgeRig r;r.held();dispatch(r.gear,r.command());
        ckH30(!r.ticket().serial,"a redundant request for the already held same item cannot create a draw owner");
        r.prepare();for(unsigned n=0;n<14;++n)r.step({0,.03,0});ckH30(r.hits()==1,"already-held positive control preserves ordinary combat");}
    {BridgeRig r;r.gear.handKnown[0]=false;dispatch(r.gear,r.command());ckH30(!r.ticket().serial,"unknown prior hand is not evidence of extraction");}
    {BridgeRig r;auto c=r.command();c.kind=ph::CommandKind::Holster;dispatch(r.gear,c);ckH30(!r.ticket().serial,"holstering cannot lend a Draw occurrence");}
    {BridgeRig r;returnedD63=false;dispatch(r.gear,r.command());ckH30(r.ticket().serial!=0,"entered native operation retains only input context when return is uncertain");
        ckH30(r.prepare()&&!r.profiles[0].primary,"uncertain return still grants no held profile or attack authority");}
    {BridgeRig r;dispatch(r.gear,r.command());r.held();++r.gear.items[0].profile.generation;PhysGripBank6698 bank{};bank.grip[0].key=r.gear.items[0].profile.key;bank.grip[0].generation=r.gear.items[0].profile.generation;g_phys_grips6698.publish(bank);r.prepare();
        for(unsigned n=0;n<30;++n)r.step();r.commands.clear();for(unsigned n=0;n<14;++n)r.step({0,.03,0});
        ckH30(r.hits()==1,"old draw cannot disarm a later binding generation of the same key");}
    {BridgeRig r;dispatch(r.gear,r.command());r.prepare();
        for(unsigned n=0;n<18;++n)r.step({0,.03,0});for(unsigned n=0;n<8;++n)r.step();
#ifdef KCD2VR_PHYSICAL_DRAW_MOTION_D63
        ckH30(r.state.drawMotionD63[0].qualified&&!r.state.drawMotionD63[0].armed,"actual empty-profile path remembers extraction/settle without combat permission");
#endif
        r.held();r.prepare();r.step();r.commands.clear();for(unsigned n=0;n<14;++n)r.step({0,.03,0});
        ckH30(r.hits()==1,"delayed held ACK after completed extraction preserves the FIRST subsequent attack");}
    {BridgeRig r;dispatch(r.gear,r.command());r.prepare();for(unsigned n=0;n<40;++n)r.step();
        r.held();r.prepare();r.step();for(unsigned n=0;n<18;++n)r.step({0,.03,0});
        ckH30(r.hits()+r.count(ph::CommandKind::Prepare)==0,"quiet hilt before delayed ACK does not arm the following extraction");}
    {BridgeRig r;dispatch(r.gear,r.command());r.prepare();for(unsigned n=0;n<18;++n)r.step({0,.03,0});for(unsigned n=0;n<8;++n)r.step();
        // The entity-world root may translate/rotate independently of these
        // already model-local and HMD-relative controller sources.
        r.in.haveWorld=true;r.in.world.p={7,11,2};r.in.world.q={0,0,std::sin(.8),std::cos(.8)};
        r.held();r.prepare();r.step();r.commands.clear();for(unsigned n=0;n<14;++n)r.step({0,.03,0});
        ckH30(r.hits()==1,"late ACK across entity-world root motion preserves the first new controller stroke");}
    std::printf("D63 actual Draw dispatch/profile ingress: %u/%u; native leaves modeled, game NOT_RUN\n",checksH30-failuresH30,checksH30);
    return failuresH30?1:0;
}
