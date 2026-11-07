// Actual production direct-candidate mid observer. Native entity lookup is a
// bounded leaf fixture; register bank, native payload and source lifetime are
// explicit inputs. No game ABI or simulated native acceptance is claimed.
#define KCD2VR_GRASP_HOST_TEST // stateful hook lifetime leaf; no native patch
#include <windows.h>
#include <DetourModKit/hook.hpp>
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>
#include "vrik/pickup_use19c3.h"
#include "vrik/held_interaction114.h"
#include "vrik/hands_mode59c7f.h"
#include "interaction_alignment.h"
#include "tracking_sample59c6.h"
#include "core/door_candidates38.h"
#include "core/snapshot_mailbox.h"
#include "core/door_use46.h"
#include "core/door_origin_h12.h"
static std::vector<std::string> doorReceipts63;
namespace kcd2vr::diagnostic_trace {void Write(const char*);}
namespace kcd2vr::interaction_alignment {
static constexpr std::uintptr_t kObjectPolicy=0x3A69D90;
static std::uintptr_t g_wh_base=0x10000000;
static std::atomic<bool> g_enabled{true},g_split_ready57c1{true},g_visual_fallback_enabled{false};
static std::uint64_t g_door_generation38=1;
static std::uint64_t g_door_acquisition_generation65=(std::uint64_t(1)<<63)|1;
static bool rayActive=true;
static std::mutex g_hand_candidate_mutex178r14;
static std::optional<DetourModKit::hook::Hook> g_direct_door_hook54r1;
static door_candidates38::Records<HandCandidate> g_door_candidates38;
static core::SnapshotMailbox<palm59c9::Pair> g_palm_packets59c9;
static const char* nativeClass="AnimDoor";
static std::uintptr_t expectedInteractor=0,expectedPlayer=0;
static HandCandidate projected54r1{};
static int lastReceipt54r1=-1;
static void SelectionReceipt59c10(std::uint64_t,std::uint32_t,int,int reason,float,float,std::uint32_t) noexcept{lastReceipt54r1=reason;}
static interaction57c1::Kind CandidateKind57c1(std::uintptr_t interactor,std::uint32_t id,std::uintptr_t& player,char* cls,std::uintptr_t* entityOut=nullptr) noexcept {
    player=0;if(entityOut)*entityOut=0;if(interactor!=expectedInteractor||(id!=42&&id!=263342))return interaction57c1::Kind::Unknown;
    player=expectedPlayer;std::snprintf(cls,32,"%s",nativeClass);return interaction57c1::Kind::Object;
}
bool HandRayActive() noexcept{return rayActive;}
std::uint64_t DoorSourceGeneration38() noexcept{return g_enabled&&g_split_ready57c1&&!g_visual_fallback_enabled&&rayActive?g_door_generation38:0;}
bool DoorSourceCurrent38(std::uint64_t generation) noexcept{return generation&&generation==DoorSourceGeneration38();}
std::uint64_t DoorAcquisitionGeneration65() noexcept{return g_enabled&&g_split_ready57c1&&!g_visual_fallback_enabled?g_door_acquisition_generation65:0;}
bool DoorAcquisitionCurrent65(std::uint64_t generation) noexcept{return generation&&generation==DoorAcquisitionGeneration65();}
bool LastHandCandidate(HandCandidate& out) noexcept{out=projected54r1;return out.entity!=0;}
void SetPalmRays59c9(const palm59c9::Pair& pair) noexcept{(void)g_palm_packets59c9.publish(pair);}
void SetHandRayWorld(bool,float,float,float,float,float,float,float,float,int) noexcept{}
#include "../src/interaction_direct_door54r1.inl"
bool NativeDirectDoorCandidate54r1(int side,std::uint64_t now,HandCandidate& out) noexcept{return ReadDirectDoor54r1(side,now,out);}
bool ExactDoorCandidate38(std::uint32_t target,int side,std::uint64_t now,HandCandidate& out) noexcept{return ReadExactDoor54r1(target,side,now,out);}
}
namespace kcd2vr::action_time_owner59c11 {
static bool nativeGraspAvailable64=false;
bool NativeGraspEnabled() noexcept{return false;}
bool NativeGraspAvailable63() noexcept {return nativeGraspAvailable64;}
void PreparePhysicalPickup63(bool,int,std::uint64_t) noexcept {}
void NotePhysicalPickupUse63(int,std::uint64_t,std::uint32_t) noexcept {}
void NoteUseInput(bool,bool,int,std::uint64_t) noexcept{}
}
namespace kcd2vr::frik_pose_probe {
void NoteVrPickupUse43(int,std::uint32_t,bool,std::uint64_t) noexcept{}
void SetInteractionHandContext158(int,bool,float,float,float,float) noexcept{}
}
namespace kcd2vr::diagnostic_trace {void Write(const char* s){doorReceipts63.emplace_back(s);}}
using namespace kcd2vr;
using namespace kcd2vr::interaction_alignment;
static std::uint64_t& clock54=hostTickCount64;
#include "door_producer_fixture46.h"
static unsigned checks=0;
static void checkAt65(bool good,unsigned line){++checks;if(!good)std::fprintf(stderr,"direct check%u line%u failed\n",checks,line);assert(good);}
#define check(x) checkAt65((x),__LINE__)
// Explicit synchronous native-physics leaf, between two actual descriptor
// callbacks. The native descriptor centre deliberately remains one metre away.
static bool physicalSurface65(std::uint64_t now,float distance=.08f,std::uint32_t returnedEntity=42){
    door_hold65::Frame frame{};if(!door_hold65::frame.read(frame))return false;bool any=false;
    for(const auto& h:frame.hand){std::array<door_surface65::Request,8> requests{};
        const auto count=door_surface65::requests(now,h.player,h.poseEpoch,h.side,requests);
        (void)door_surface65::withdraw(h.side,now,now);
        for(unsigned i=0;i<count;++i){door_surface65::Witness w{};w.source=requests[i];w.sample=now;w.queryTick=now;
            w.actualPalm=h.point;w.point={h.point.x,h.point.y+distance,h.point.z};w.normal={0,-1,0};
            w.entityId=returnedEntity;w.opaqueOwner=0x5678;w.opaqueCollider=0x6789;w.distance=distance;w.valid=true;
            any=door_surface65::publish(w)||any;}}
    return any;
}
int main(){
    using namespace DetourModKit::hook;
    StopDirectDoor54r1();check(!g_direct_door_retired54r1);
    alignas(8) std::uintptr_t entity[8]{};entity[0]=g_wh_base+0x4072C78;
    alignas(8) std::uintptr_t player[8]{};player[0]=g_wh_base+0x3AA4B98;player[7]=reinterpret_cast<std::uintptr_t>(entity);
    alignas(8) std::uintptr_t interactor[2]{g_wh_base+0x3A6A0C8,reinterpret_cast<std::uintptr_t>(player)};
    alignas(8) std::uintptr_t policy[2]{g_wh_base+kObjectPolicy,reinterpret_cast<std::uintptr_t>(player)};
    alignas(8) unsigned char payload[0x38]{};
    const std::uint32_t target=42,priority=99999;
    std::memcpy(payload,&priority,4);std::memcpy(payload+0x10,&target,4);
    expectedInteractor=reinterpret_cast<std::uintptr_t>(interactor);expectedPlayer=reinterpret_cast<std::uintptr_t>(player);
    MidContext ctx{};gpr(ctx,Gpr::R15)=expectedInteractor;gpr(ctx,Gpr::R12)=reinterpret_cast<std::uintptr_t>(policy);
    gpr(ctx,Gpr::Rdx)=reinterpret_cast<std::uintptr_t>(payload);ctx.ip=0x6404E1;
    palm59c9::Pair palms{};
    auto publish=[&](std::uint64_t now,int side){hostTickCount64=now;palms=palm59c9::Pair{};palms.locked=side;
        for(int i=0;i<2;++i){auto& r=palms.ray[i];r.hand=true;r.owner=i;r.tick=now;r.y=1;r.limit=2;r.radius=.2;
            r.sourceEpoch=7;r.sourcePlayer=reinterpret_cast<std::uintptr_t>(entity);r.ox=float(i);palms.eligible[i]=true;}
        const float p[3]{float(side),1.f,0.f};std::memcpy(payload+0x28,p,sizeof(p));g_palm_packets59c9.publish(palms);
        door_hold65::Frame old{},frame{};(void)door_hold65::frame.read(old);
        auto& h=frame.hand[side];h.available=true;h.side=side;h.grip=1;h.tick=h.poseTick=now;h.poseEpoch=7;h.trackingEpoch=3;
        h.sourceGeneration=g_door_acquisition_generation65;h.settingsEpoch=door_use46::settingsEpoch.load();h.experimentEpoch=workstation178r20c1::experimentEpoch36.load();
        h.player=reinterpret_cast<std::uintptr_t>(entity);h.point={float(side),0,0};
        h.press=door_hold65::current(old.hand[side],now)&&old.hand[side].grip>=.30f&&old.hand[side].press?old.hand[side].press:now-100;
        (void)door_hold65::frame.publish(frame);};
    auto observe=[&](){const auto before=ctx;unsigned char nativeBefore[sizeof(payload)];std::memcpy(nativeBefore,payload,sizeof(payload));
        NativeDirectDoor54r1(ctx);(void)physicalSurface65(hostTickCount64);NativeDirectDoor54r1(ctx);
        check(!std::memcmp(&ctx,&before,sizeof(ctx)));check(!std::memcmp(nativeBefore,payload,sizeof(payload)));};
    HandCandidate c{};
    for(int side=0;side<2;++side){publish(1000,side);observe();
        check(lastReceipt54r1==11);
        check(NativeDirectDoorCandidate54r1(side,1000,c)&&c.entity==42&&c.owner==side&&c.sourcePlayer==reinterpret_cast<std::uintptr_t>(entity));
        check(c.x==float(side)&&c.y==.08f&&c.z==0.f&&c.distance==.08f&&c.sourceTick==1000&&c.sourceEpoch==7);
        check(!NativeDirectDoorCandidate54r1(1-side,1000,c));check(!NativeDirectDoorCandidate54r1(side,999,c));
        check(NativeDirectDoorCandidate54r1(side,1080,c));check(!NativeDirectDoorCandidate54r1(side,1081,c));
        check(ExactDoorCandidate38(42,side,1000,c));
        check(!g_door_candidates38.find(42,side,1000,c));
    }
    // H23: the BEFORE-Grip descriptor cache must really be populated by
    // an open hand. This remains nomination only: no query/Use/door lease.
    for(int side=0;side<2;++side){
        publish(1000,side);door_hold65::Frame frame{};check(door_hold65::frame.read(frame));
        frame.hand[side].press=0;frame.hand[side].grip=0;check(door_hold65::frame.publish(frame));
        door_surface65::requestMailbox.publish({});door_surface65::witnessMailbox.publish({});
        door_pre_press66::clear(side);NativeDirectDoor54r1(ctx);
        door_pre_press66::Entry cached{};check(door_pre_press66::read(side,1000,DoorAcquisitionGeneration65(),cached)&&cached.target==42);
        std::array<door_surface65::Request,8> requests{};
        check(door_surface65::requests(1000,frame.hand[side].player,7,side,requests)==0);
        check(!NativeDirectDoorCandidate54r1(side,1000,c));
        door_pre_press66::clear(side);
    }
    // Connect the actual new observer to the actual Mod Use producer. The
    // logged near PickableItem stays the secondary projected candidate. Native
    // direct offer alone grants neither an input intent nor action consumption.
    hands59c7f::rung=2;hands59c7f::admitted=true;hands59c7f::pickupReady=false;
    for(int side=0;side<2;++side){
        (void)door_use46::intent.publish({});const auto consumed=door_use46::consumed.load();
        Producer producer;producer.player=reinterpret_cast<std::uintptr_t>(entity);producer.handWorld65[side].x=float(side);
        auto offer=[&](std::uint64_t tick){publish(tick,side);observe();
            // The real native scan continues through unrelated offered items
            // before the Mod thread consumes the door candidate. This used to
            // erase every door offer and produce NO_EXPLICIT_SEMI_INPUT.
            const std::uint32_t other=263342;std::memcpy(payload+0x10,&other,4);
            nativeClass="PickableItem";observe();nativeClass="AnimDoor";
            std::memcpy(payload+0x10,&target,4);
            projected54r1={};projected54r1.entity=263342;projected54r1.owner=side;
            projected54r1.tick=tick;projected54r1.object=true;projected54r1.distance=.12f;
            std::strcpy(projected54r1.cls,"PickableItem");};
        offer(1000);door_use46::Intent intent{};check(!door_use46::read(intent,1000));
        producer.tick(1000,0,side);offer(1020);producer.tick(1020,1,side);
        check(producer.touch_bindings_.requests==0&&!producer.touch_bindings_.held);
        offer(1040);producer.tick(1040,1,side);
        check(producer.touch_bindings_.requests==1&&!producer.touch_bindings_.held);
        check(door_use46::read(intent,1040)&&intent.target==42&&intent.side==side&&intent.player==producer.player);
        check(door_use46::consumed.load()==consumed); // actual accepted action is still absent
        offer(1060);producer.tick(1060,1,side);check(producer.touch_bindings_.requests==1&&door_use46::current(intent,1060));
        producer.tick(1080,0,side);check(!door_use46::current(intent,1080));
    }
    // H23 regression: an already transported apple hover must not prevent
    // observing the SAME native nominated door surface. This observer remains
    // read-only and does not retarget the held transaction or claim native Use.
    for(int side=0;side<2;++side){
        publish(1090,side);palms.lockedEntity=263342;g_palm_packets59c9.publish(palms);
        const auto claimed=door_use46::claimedPhysical62.load();
        const auto consumed=door_use46::consumed.load();
        observe();
        check(NativeDirectDoorCandidate54r1(side,1090,c)&&c.entity==42&&c.owner==side);
        check(ExactDoorCandidate38(42,side,1090,c));
        palm59c9::Pair retained{};check(g_palm_packets59c9.read(retained)&&retained.lockedEntity==263342);
        check(door_use46::claimedPhysical62.load()==claimed&&door_use46::consumed.load()==consumed);
        // No physical surface -> no admission, even though the native nominee
        // and the conflicting hover lock remain present.
        door_surface65::witnessMailbox.publish({});NativeDirectDoor54r1(ctx);
        check(!NativeDirectDoorCandidate54r1(side,1090,c));
    }
    // A held native Pickable already transported by this press stays owned
    // by that transaction. Geometric observation is not a second input edge.
    {door_use46::Controller owner;door_use46::Input in{};
     in.available=in.held=in.scoped54=true;in.side=0;in.now=in.poseTick=1090;in.press=1000;
     in.poseEpoch=7;in.sourceGeneration=DoorAcquisitionGeneration65();in.trackingEpoch=3;in.player=reinterpret_cast<std::uintptr_t>(entity);
     door_use46::Selection apple{};apple.target=263342;apple.side=0;apple.tick=1090;
     check(!owner.update(in,apple).owns);owner.transport62(in,false,true);
     door_use46::PhysicalUse62 before{};check(door_use46::physicalUse62.read(before)&&before.serial);
     door_use46::Selection door{true,42,0,1090,1090,7,in.sourceGeneration,in.player,{0,.08f,0},.08f,true};
     const auto plan=owner.update(in,door);check(!plan.pulse&&!plan.owns);
     door_use46::PhysicalUse62 after{};check(door_use46::physicalUse62.read(after)&&after.serial==before.serial&&after.target==before.target);
    }
    projected54r1={};
    // A rejected observation of the SAME native target withdraws its offer;
    // an arbitrary native sort key cannot become a target.
    publish(1100,0);observe();nativeClass="PickableItem";observe();check(!NativeDirectDoorCandidate54r1(0,1100,c));
    check(!ExactDoorCandidate38(42,0,1100,c));nativeClass="AnimDoor";
    // An independently observed projection is not withdrawn by a direct
    // rejection. Conversely, a direct-only witness is immediately gone.
    publish(1110,0);observe();check(ExactDoorCandidate38(42,0,1110,c));
    auto projection=c;projection.sourceGeneration=g_door_generation38;g_door_candidates38.record(projection);
    nativeClass="PickableItem";observe();check(!NativeDirectDoorCandidate54r1(0,1110,c));
    check(ExactDoorCandidate38(42,0,1110,c)&&c.sourceTick==projection.sourceTick);
    g_door_candidates38.clear();check(!ExactDoorCandidate38(42,0,1110,c));nativeClass="AnimDoor";
    // Current palm geometry/source, not HMD proximity or entity root, is needed.
    for(int bad=0;bad<8;++bad){publish(1200,0);door_hold65::Frame frame{};door_hold65::frame.read(frame);
        if(bad==0)palms.ray[0].tick=1119;
        if(bad==1)palms.ray[0].tick=1201;
        if(bad==2)palms.ray[0].sourceEpoch=0;
        if(bad==3)palms.ray[0].sourcePlayer=123;
        if(bad==4)palms.eligible[0]=false;
        if(bad==5)frame.hand[0].sourceGeneration++;
        if(bad==6)frame.hand[0].grip=0;
        if(bad==7){const float p[3]{0,std::nanf(""),0};std::memcpy(payload+0x28,p,sizeof(p));}
        if(bad==0)frame.hand[0].poseTick=1119;if(bad==1)frame.hand[0].poseTick=1201;
        if(bad==2)frame.hand[0].poseEpoch=0;if(bad==3)frame.hand[0].player=123;if(bad==4)frame.hand[0].available=false;
        door_hold65::frame.publish(frame);g_palm_packets59c9.publish(palms);observe();check(!NativeDirectDoorCandidate54r1(0,1200,c));
        check(!ExactDoorCandidate38(42,0,1200,c));
        check(lastReceipt54r1==12);
    }
    publish(1300,0);observe();check(NativeDirectDoorCandidate54r1(0,1300,c));
    const auto revoked=c;++g_door_acquisition_generation65;check(!NativeDirectDoorCandidate54r1(0,1300,c));
    PublishDirectDoor54r1(revoked,g_door_acquisition_generation65-1);check(!NativeDirectDoorCandidate54r1(0,1300,c));
    publish(1400,0);observe();rayActive=false;check(NativeDirectDoorCandidate54r1(0,1400,c));rayActive=true;
    g_enabled=false;check(!NativeDirectDoorCandidate54r1(0,1400,c));g_enabled=true;
    g_visual_fallback_enabled=true;check(!NativeDirectDoorCandidate54r1(0,1400,c));g_visual_fallback_enabled=false;
    // No object-policy or correct native-player witness: no new observation.
    ++g_door_acquisition_generation65;policy[0]=g_wh_base+0x3A69E48;observe();check(!NativeDirectDoorCandidate54r1(0,1400,c));policy[0]=g_wh_base+kObjectPolicy;
    policy[1]=0;observe();check(!NativeDirectDoorCandidate54r1(0,1400,c));policy[1]=expectedPlayer;
    player[7]=0;observe();check(!NativeDirectDoorCandidate54r1(0,1400,c));player[7]=reinterpret_cast<std::uintptr_t>(entity);
    // Door-only receipt has its own bounded gate, independent of the shared
    // projection receipt. It changes neither descriptor nor input authority.
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,true,2000);
    publish(2000,0);diagnostics::door_receipt63::scope(true,2000);doorReceipts63.clear();observe();
    check(doorReceipts63.size()==1&&doorReceipts63.back().find("NO_CURRENT_20CM_DOOR_SURFACE")!=std::string::npos);
    observe();check(doorReceipts63.size()==1); // <=4Hz even with many native candidates
    publish(2250,0);diagnostics::door_receipt63::scope(true,2250);palms.lockedEntity=99;g_palm_packets59c9.publish(palms);observe();
    check(doorReceipts63.size()==2&&doorReceipts63.back().find("NO_CURRENT_20CM_DOOR_SURFACE")!=std::string::npos);
    publish(2500,0);diagnostics::door_receipt63::scope(false,2500);observe();check(doorReceipts63.size()==2);
    diagnostics::door_receipt63::scope(true,2500);check(!diagnostics::door_receipt63::active(2581));
    diagnostics::door_receipt63::Gate bounded63;
    for(unsigned n=0;n<128;++n){const auto t=3000+250*n;diagnostics::door_receipt63::scope(true,t);check(diagnostics::door_receipt63::allow(bounded63,t));}
    diagnostics::door_receipt63::scope(true,35000);check(!diagnostics::door_receipt63::allow(bounded63,35000));
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,false,2500);
    diagnostics::door_receipt63::scope(true,2500);check(!diagnostics::door_receipt63::active(2500));
    // Teardown does not destroy a trampoline that may still contain a return
    // address. A failed disable retains the same handle and revokes all offers.
    publish(1500,0);observe();check(NativeDirectDoorCandidate54r1(0,1500,c));
    g_direct_door_hook54r1.emplace();g_direct_door_hook54r1->failDisable=true;
    const auto* retainedHook=&*g_direct_door_hook54r1;StopDirectDoor54r1();
    check(g_direct_door_retired54r1&&g_direct_door_hook54r1&& &*g_direct_door_hook54r1==retainedHook&&g_direct_door_hook54r1->is_enabled());
    observe();check(!NativeDirectDoorCandidate54r1(0,1500,c));check(!ExactDoorCandidate38(42,0,1500,c));
    g_direct_door_hook54r1->failDisable=false;StopDirectDoor54r1();
    check(g_direct_door_hook54r1&& &*g_direct_door_hook54r1==retainedHook&&!g_direct_door_hook54r1->is_enabled());
    std::printf("door_direct54r1: %u checks PASS; production observer preserves native registers/payload and joins direct target to exact fresh palm\n",checks);
    return 0;
}
