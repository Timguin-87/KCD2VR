// Chronological production chain missing from the older observer-only fixture:
// generic hover -> squeeze -> actual point/entity lock -> native direct offer
// -> actual Use producer. Native leaves supply data, never action acceptance.
#define main prior_direct54r1_checks64
#include "door_direct54r1_test.cpp"
#undef main
#include "../src/activity_input163.h"
#include "../src/vrik/alchemy_palm_lock27.h"
#include "../src/vrik/hand_point123r1.h"
#include "../src/vrik/workstation178r20c1.h"
#undef check
#define check(expression64) do { ++checks; if(!(expression64)){std::fprintf(stderr,"FAIL line %u: %s\n",__LINE__,#expression64);std::abort();} } while(false)
namespace kcd2vr::diagnostics::lab {static bool NativeReference() noexcept{return false;}}
struct DoorFlow64 : Producer {
    hand_point123r1::State point123{};
    alchemy_input19c4::PalmLock27 alchemyPalmLock27{};
    bool pointerEnabled32=true;unsigned reserved114=0,gripMask116=0;
    int itemState116[2]{0,0};palm59c9::Pair published{},pointPublished64{};
    unsigned consumerFault64=0; // explicit race at the actual point/consumer seam
    bool actualRayPublication65=false; // C65 fixture executes native ray withdrawal leaf too
    struct Controller64 {bool active=true;float grip=0,trigger=0;struct{float pos_x=0,pos_y=1.2f,pos_z=0;}pose;};
    struct XR64 {Controller64 hands[2];std::uint64_t tick=0,epoch=3;
        const Controller64& RightController(){return hands[0];}const Controller64& LeftController(){return hands[1];}
        bool CopyInputSampleStamp59c6(tracking59c6::Stamp& s){s.tick=tick;s.epoch=epoch;return true;}
    } xr64;XR64* xr_manager_=&xr64;
    void frame(std::uint64_t ladder_now_ms,float grip,int hand=0){
        clock54=ladder_now_ms+frameWait54;xr64.tick=clock54-sourceAge;xr64.epoch=trackingEpoch;
        pi={};pi.enabled=true;pi.menu=ui;pi.now_ms=ladder_now_ms;
        for(int side=0;side<2;++side){xr64.hands[side].grip=side==hand?grip:0;
            pi.hand[side]={tracked,empty,true,xr64.hands[side].grip,false};}
        touch_bindings_.tick=ladder_now_ms;
        // Copied existing empty-hand arbitration permission; this fixture is
        // about target/transport ownership, not discovery of hand availability.
        grabMask59c7f=grip>=.30f?1u<<hand:0;
        const auto held114=held_interaction114::update(heldUse114,pi);
        physical_interaction106::Output po{};po.ray_owner=held114.owner;po.ray_active=held114.ray;
        interaction_alignment::HandCandidate candidate59c7f{};(void)LastHandCandidate(candidate59c7f);
        auto buildPalm59c9=[&](int side,interaction112::Ray& r){r={};r.hand=true;r.owner=side;
            r.tick=clock54-poseAge;r.sourceEpoch=poseEpoch;r.sourcePlayer=player;r.y=1;r.limit=2;r.radius=.2f;
            r.ox=float(side);return true;};
#include "../src/vrik/interaction_point32.inl"
        pointPublished64=palms59c9;
        SetPalmRays59c9(palms59c9);
        if(actualRayPublication65){
            rayPublished116=po.ray_active&&po.ray_owner>=0&&palms59c9.eligible[po.ray_owner];
            rayActive=rayPublished116;
            if(!rayPublished116)++g_door_generation38; // actual SetHandRayWorld(false) retires ONLY legacy ray source
        }
        if(consumerFault64==1)++xr64.epoch;
        if(consumerFault64==2){door_use46::SetEnabled(false);door_use46::SetEnabled(true);}
        if(consumerFault64==3)workstation178r20c1::experimentEpoch36.fetch_add(1);
        if(consumerFault64==4)++g_door_acquisition_generation65;
        bool heldNow114=false,useSent116=false;
#include "../src/vrik/pickup_use_producer19c3.inl"
        published=palms59c9;
    }
};
#ifndef KCD2VR_DOOR_FLOW_TYPES_ONLY65
int main(){
    using namespace DetourModKit::hook;
    alignas(8) std::uintptr_t entity[8]{};entity[0]=g_wh_base+0x4072C78;
    alignas(8) std::uintptr_t player[8]{};player[0]=g_wh_base+0x3AA4B98;player[7]=reinterpret_cast<std::uintptr_t>(entity);
    alignas(8) std::uintptr_t interactor[2]{g_wh_base+0x3A6A0C8,reinterpret_cast<std::uintptr_t>(player)};
    alignas(8) std::uintptr_t policy[2]{g_wh_base+kObjectPolicy,reinterpret_cast<std::uintptr_t>(player)};
    alignas(8) unsigned char payload[0x38]{};const std::uint32_t target=42;
    std::memcpy(payload+0x10,&target,4);expectedInteractor=reinterpret_cast<std::uintptr_t>(interactor);
    expectedPlayer=reinterpret_cast<std::uintptr_t>(player);
    MidContext ctx{};gpr(ctx,Gpr::R15)=expectedInteractor;gpr(ctx,Gpr::R12)=reinterpret_cast<std::uintptr_t>(policy);
    gpr(ctx,Gpr::Rdx)=reinterpret_cast<std::uintptr_t>(payload);ctx.ip=0x6404E1;
    auto hover=[&](std::uint64_t now,int side,const char* cls="PickableItem"){
        projected54r1={};projected54r1.entity=263342;projected54r1.owner=side;
        projected54r1.tick=now;projected54r1.object=true;projected54r1.distance=.12f;std::strcpy(projected54r1.cls,cls);};
    auto observe=[&](std::uint64_t now,int side){clock54=now;const float p[3]{float(side),.08f,0.f};
        std::memcpy(payload+0x28,p,sizeof(p));const auto before=ctx;
        NativeDirectDoor54r1(ctx);(void)physicalSurface65(now);NativeDirectDoor54r1(ctx);
        check(!std::memcmp(&before,&ctx,sizeof(ctx)));};
    hands59c7f::rung=2;hands59c7f::admitted=true;hands59c7f::pickupReady=false;
    action_time_owner59c11::nativeGraspAvailable64=true;
    for(int side=0;side<2;++side){
        (void)door_use46::intent.publish({});(void)door_use46::acceptedIntent62.publish({});g_door_candidates38.clear();
        {std::lock_guard<std::mutex> lock(g_hand_candidate_mutex178r14);g_direct_door54r1={};}
        DoorFlow64 f;f.player=reinterpret_cast<std::uintptr_t>(entity);
        const auto start=1000+side*1000;
        hover(start,side);f.frame(start,0,side); // actual open-hand hover, no prior cached door
        hover(start+20,side);f.frame(start+20,1,side);
        hover(start+40,side);f.frame(start+40,1,side); // first native held settle; pickup still unsent
        check(f.touch_bindings_.requests==0&&!f.touch_bindings_.held);
        // The completed native Door descriptor appears only AFTER the squeeze.
        // A generic nearby Pickable must not blind this same unsent palm.
        observe(start+45,side);HandCandidate direct{};
        const bool current=NativeDirectDoorCandidate54r1(side,start+45,direct);
        if(!current)std::fprintf(stderr,"FAIL C64 first native Door offer after unsent Grip was hidden: side=%d lockedEntity=%u requests=%u\n",side,f.published.lockedEntity,f.touch_bindings_.requests);
        check(current&&direct.entity==target);
        hover(start+60,side);f.frame(start+60,1,side);
        check(f.touch_bindings_.requests==1&&!f.touch_bindings_.held);
        door_use46::Intent request{};check(door_use46::read(request,start+60)&&request.target==target&&request.side==side);
        check(f.published.lockedEntity==target);
        // No native accepted action has run: request is not consumption.
        check(door_use46::consumed.load()<request.token);
        hover(start+80,side);f.frame(start+80,1,side);check(f.touch_bindings_.requests==1);
        f.frame(start+100,0,side);check(!door_use46::read(request,start+100));
    }
    auto reset=[&](){(void)door_use46::intent.publish({});(void)door_use46::acceptedIntent62.publish({});
        g_direct_door54r1={};g_door_candidates38.clear();palm59c9::ordinaryTransportLock64={};};
    // A genuine sent pickup is an object transaction, unlike its earlier
    // 150-ms qualification. Do not reopen it as a second Use for a later Door.
    for(int side=0;side<2;++side){reset();DoorFlow64 f;f.player=reinterpret_cast<std::uintptr_t>(entity);
        const auto start=4000+side*1000;hover(start,side);f.frame(start,0,side);
        for(unsigned step=20;step<=180;step+=20){hover(start+step,side);f.frame(start+step,1,side);
            if(step<180)check(!f.touch_bindings_.requests&&f.published.lockedEntity==0);}
        if(f.touch_bindings_.requests!=1||f.touch_bindings_.held||f.published.lockedEntity!=263342)
            std::fprintf(stderr,"pickup debug side=%d requests=%u held=%d entity=%u chosen=%d pickable=%d cancelled=%d sent=%d press=%llu at=%llu lockside=%d lockpress=%llu locktransport=%d\n",side,f.touch_bindings_.requests,int(f.touch_bindings_.held),f.published.lockedEntity,int(f.pickupUse19c3.chosen),int(f.pickupUse19c3.pickable63),int(f.pickupUse19c3.cancelled),int(f.pickupUse19c3.sent),f.pickupUse19c3.press,f.pickupUse19c3.selectedAt63,palm59c9::ordinaryTransportLock64.side,palm59c9::ordinaryTransportLock64.press,int(palm59c9::ordinaryTransportLock64.transported));
        check(f.touch_bindings_.requests==1&&!f.touch_bindings_.held&&f.published.lockedEntity==263342);
        observe(start+185,side);HandCandidate rejected{};check(!NativeDirectDoorCandidate54r1(side,start+185,rejected));
        hover(start+200,side);projected54r1.entity=999;f.frame(start+200,1,side);
        check(f.touch_bindings_.requests==1&&f.published.lockedEntity==263342&&!f.doorUse46.door);
        f.frame(start+220,0,side);check(f.published.lockedEntity==0);
    }
    // Contextual long-hold Use has already run too. Even if a direct offer is
    // independently fresh, it cannot create another action for that press.
    for(int side=0;side<2;++side){reset();DoorFlow64 f;f.player=reinterpret_cast<std::uintptr_t>(entity);
        const auto start=6000+side*1000;hover(start,side,"OtherNativeObject");f.frame(start,0,side);
        hover(start+20,side,"OtherNativeObject");f.frame(start+20,1,side);
        hover(start+40,side,"OtherNativeObject");f.frame(start+40,1,side);
        check(f.touch_bindings_.held&&!f.touch_bindings_.requests&&f.published.lockedEntity==263342);
        auto temporarilyUnlocked=f.published;temporarilyUnlocked.lockedEntity=0;SetPalmRays59c9(temporarilyUnlocked);
        observe(start+45,side);HandCandidate offered{};check(NativeDirectDoorCandidate54r1(side,start+45,offered));
        hover(start+60,side,"OtherNativeObject");f.frame(start+60,1,side);
        check(f.touch_bindings_.held&&!f.touch_bindings_.requests&&!f.doorUse46.door&&f.published.lockedEntity==263342);
        f.frame(start+80,0,side);check(!f.touch_bindings_.held);
    }
    // A refused queue has sent no native input and must not acquire an entity
    // lock. The still-held, untransported press can see a later exact Door.
    reset();DoorFlow64 refused;refused.player=reinterpret_cast<std::uintptr_t>(entity);refused.touch_bindings_.allow=false;
    hover(8000,0);refused.frame(8000,0);for(unsigned step=20;step<=180;step+=20){hover(8000+step,0);refused.frame(8000+step,1);}
    check(refused.touch_bindings_.requests==1&&!refused.published.lockedEntity&&!refused.doorUse46.transportedPress62);
    observe(8185,0);HandCandidate offered{};check(NativeDirectDoorCandidate54r1(0,8185,offered));
    refused.touch_bindings_.allow=true;hover(8200,0);refused.frame(8200,1);
    check(refused.touch_bindings_.requests==2&&refused.doorUse46.chosen.sent&&refused.published.lockedEntity==target);
    refused.frame(8220,0);
    // An exact target is not enough across player, ray-space, source-generation
    // or XR reference-space retirement. No old press gains a new Door intent.
    for(int change=0;change<4;++change){reset();DoorFlow64 f;f.player=reinterpret_cast<std::uintptr_t>(entity);
        const auto start=9000+change*1000;hover(start,0);f.frame(start,0);hover(start+20,0);f.frame(start+20,1);
        hover(start+40,0);f.frame(start+40,1);
        if(change==0)f.poseEpoch=8;if(change==1)++g_door_acquisition_generation65;if(change==2)f.trackingEpoch=4;
        // Player mismatch deliberately produces a refused exact native offer.
        if(change==3)f.player+=8;
        hover(start+60,0);f.frame(start+60,1);observe(start+65,0);
        hover(start+80,0);f.frame(start+80,1);
        check(!f.touch_bindings_.requests&&!f.doorUse46.door);
        f.frame(start+100,0);
    }
    // A short click still sends nothing for an ordinary Pickable. Its release
    // clears the hand/entity lock; a fresh Door hover never manufactures a press.
    reset();DoorFlow64 click;click.player=reinterpret_cast<std::uintptr_t>(entity);
    hover(14000,0);click.frame(14000,0);hover(14020,0);click.frame(14020,1);
    hover(14040,0);click.frame(14040,1);click.frame(14060,0);observe(14065,0);
    hover(14080,0);click.frame(14080,0);check(!click.touch_bindings_.requests&&!click.published.lockedEntity);
    // C62 accepted-door fallback is still native-owned. The explicit accepted
    // action leaf below supplies the exact copied Selection; production capture
    // and publish validate it against the real transported current squeeze.
    // Next point publication must already retain this ACTUAL target, with no
    // generic-Pickable -> Door cross-thread publication interval or second Use.
    reset();DoorFlow64 accepted;accepted.player=reinterpret_cast<std::uintptr_t>(entity);
    hover(15000,0,"OtherNativeObject");accepted.frame(15000,0);
    hover(15020,0,"OtherNativeObject");accepted.frame(15020,1);
    hover(15040,0,"OtherNativeObject");accepted.frame(15040,1);
    check(accepted.touch_bindings_.held&&accepted.published.lockedEntity==263342);
    door_use46::Selection nativeAccepted64{true,target,0,15040,15040,7,g_door_acquisition_generation65,accepted.player,{0,.08f,0},.08f};
    door_use46::AcceptedUse62 proof{};door_use46::Intent acceptedIntent{};
    check(door_use46::captureAccepted62(nativeAccepted64,15040,proof));
    check(door_use46::publishAccepted62(proof,15040,acceptedIntent)&&acceptedIntent.physicalUseSerial62);
    hover(15060,0,"OtherNativeObject");accepted.frame(15060,1);
    check(accepted.pointPublished64.lockedEntity==target&&accepted.published.lockedEntity==target);
    check(!accepted.touch_bindings_.requests&&accepted.doorUse46.chosen.physicalUseSerial62==acceptedIntent.physicalUseSerial62);
    hover(15080,0,"OtherNativeObject");accepted.frame(15080,1);
    check(accepted.pointPublished64.lockedEntity==target&&!accepted.touch_bindings_.requests);
    accepted.frame(15100,0);check(!accepted.published.lockedEntity);
    // A newer XR input/ray published after the loop began is current at the
    // actual acquisition clock, not falsely "future" against loop-start time.
    for(const unsigned wait64:{16u,32u}){reset();DoorFlow64 waited;waited.player=reinterpret_cast<std::uintptr_t>(entity);waited.frameWait54=wait64;
        const auto start=16000+wait64*10;hover(start,0,"OtherNativeObject");waited.frame(start,0);
        hover(start+20,0,"OtherNativeObject");waited.frame(start+20,1);
        hover(start+40,0,"OtherNativeObject");waited.frame(start+40,1);
        check(waited.touch_bindings_.held&&waited.published.lockedEntity==263342);
        hover(start+60,0,"OtherNativeObject");waited.frame(start+60,1);check(waited.pointPublished64.lockedEntity==263342);
        waited.frame(start+80,0);check(!waited.published.lockedEntity);
    }
    // Already transported contextual held Use must release and quarantine the
    // same press when its source/settings/mode is retired. Merely removing its
    // entity lock while leaving native A held would expose another target.
    for(int change=0;change<6;++change){reset();DoorFlow64 f;f.player=reinterpret_cast<std::uintptr_t>(entity);
        const auto start=17000+change*1000;hover(start,0,"OtherNativeObject");f.frame(start,0);
        hover(start+20,0,"OtherNativeObject");f.frame(start+20,1);
        hover(start+40,0,"OtherNativeObject");f.frame(start+40,1);check(f.touch_bindings_.held);
        const auto initialEpoch=f.poseEpoch,initialTracking=f.trackingEpoch,initialGen=g_door_acquisition_generation65,initialPlayer=f.player;
        if(change==0)++f.poseEpoch;if(change==1)++g_door_acquisition_generation65;if(change==2)++f.trackingEpoch;
        if(change==3)f.player+=8;if(change==4)door_use46::SetEnabled(false);if(change==5)f.f5Mode=0;
        hover(start+60,0,"OtherNativeObject");projected54r1.entity=999;f.frame(start+60,1);
        check(!f.touch_bindings_.held&&!f.touch_bindings_.requests&&f.doorUse46.transportRetired64);
        f.poseEpoch=initialEpoch;f.trackingEpoch=initialTracking;g_door_acquisition_generation65=initialGen;f.player=initialPlayer;
        door_use46::SetEnabled(true);f.f5Mode=2;
        hover(start+80,0,"OtherNativeObject");f.frame(start+80,1);
        check(!f.touch_bindings_.held&&!f.touch_bindings_.requests); // returning source is not a new squeeze
        f.frame(start+100,0);hover(start+120,0,"OtherNativeObject");f.frame(start+120,1);
        hover(start+140,0,"OtherNativeObject");f.frame(start+140,1);check(f.touch_bindings_.held); // real release/repress
        f.frame(start+160,0);
    }
    reset();door_use46::SetEnabled(false);DoorFlow64 nativeOff;nativeOff.player=reinterpret_cast<std::uintptr_t>(entity);
    hover(24000,0,"OtherNativeObject");nativeOff.frame(24000,0);hover(24020,0,"OtherNativeObject");nativeOff.frame(24020,1);
    hover(24040,0,"OtherNativeObject");nativeOff.frame(24040,1);
    check(nativeOff.touch_bindings_.held&&!nativeOff.doorUse46.transportedSource64.press); // never scoped, stays native
    nativeOff.frame(24060,0);door_use46::SetEnabled(true);
    // OFF -> ON between Mod updates is still a retired UNSENT squeeze. The
    // formerly non-door classification cannot be reauthorized by fresh data.
    for(int change=0;change<2;++change){reset();DoorFlow64 f;f.player=reinterpret_cast<std::uintptr_t>(entity);
        const auto start=25000+change*1000;hover(start,0);f.frame(start,0);hover(start+20,0);f.frame(start+20,1);
        hover(start+40,0);f.frame(start+40,1);check(f.doorUse46.classified&&!f.touch_bindings_.requests);
        if(change==0){door_use46::SetEnabled(false);door_use46::SetEnabled(true);}
        else workstation178r20c1::experimentEpoch36.fetch_add(1);
        hover(start+60,0);f.frame(start+60,1);observe(start+65,0);
        hover(start+80,0);f.frame(start+80,1);check(!f.touch_bindings_.requests&&f.doorUse46.transportRetired64);
        f.frame(start+100,0);
    }
    reset();DoorFlow64 offDuringQualification;offDuringQualification.player=reinterpret_cast<std::uintptr_t>(entity);
    hover(27500,0);offDuringQualification.frame(27500,0);hover(27520,0);offDuringQualification.frame(27520,1);
    hover(27540,0);offDuringQualification.frame(27540,1);check(!offDuringQualification.touch_bindings_.requests);
    door_use46::SetEnabled(false);hover(27560,0);offDuringQualification.frame(27560,1);
    check(offDuringQualification.doorUse46.nativeOnly54&&!offDuringQualification.doorUse46.transportedSource64.press&&
          !offDuringQualification.doorUse46.transportRetired64); // Pickable's separate 150-ms rule still applies
    // OFF controls stayed native, but ON cannot reclassify that old closed hand.
    door_use46::SetEnabled(true);observe(27565,0);hover(27580,0);offDuringQualification.frame(27580,1);
    check(!offDuringQualification.touch_bindings_.held&&!offDuringQualification.touch_bindings_.requests&&!offDuringQualification.doorUse46.door);
    offDuringQualification.frame(27600,0);
    // An epoch/settings retirement BETWEEN point publication and consume must
    // not queue the first held input from a mixed source family either.
    for(unsigned fault=1;fault<=4;++fault){reset();DoorFlow64 f;f.player=reinterpret_cast<std::uintptr_t>(entity);
        const auto start=27000+fault*1000;hover(start,0,"OtherNativeObject");f.frame(start,0);
        hover(start+20,0,"OtherNativeObject");f.frame(start+20,1);
        f.consumerFault64=fault;hover(start+40,0,"OtherNativeObject");f.frame(start+40,1);
        check(!f.touch_bindings_.held&&!f.touch_bindings_.requests);
        f.consumerFault64=0;hover(start+60,0,"OtherNativeObject");f.frame(start+60,1);
        check(!f.touch_bindings_.held&&!f.touch_bindings_.requests);f.frame(start+80,0);
    }
    // H12: before any transport, a changed Pickable restarts its fresh80-ms
    // qualification. Alternating candidates cannot dispatch; a later stable
    // target may qualify during the same continuous squeeze. Real transport,
    // unlike uncommitted hover, then freezes the exact entity.
    for(int side=0;side<2;++side){reset();DoorFlow64 f;f.player=reinterpret_cast<std::uintptr_t>(entity);
        const auto start=33000+side*1000;
        hover(start,side);f.frame(start,0,side);hover(start+20,side);f.frame(start+20,1,side);
        hover(start+40,side);f.frame(start+40,1,side);
        check(f.pickupUse19c3.pickable63&&!f.pickupUse19c3.cancelled&&f.pickupUse19c3.target63==263342&&
              !f.touch_bindings_.requests&&!f.touch_bindings_.held&&!f.published.lockedEntity);
        for(unsigned step=60;step<=200;step+=20){
            hover(start+step,side);projected54r1.entity=(step/20)%2?999u:263342u;
            f.frame(start+step,1,side);
            check(!f.pickupUse19c3.cancelled&&!f.pickupUse19c3.sent&&f.pickupUse19c3.target63==projected54r1.entity&&
                  !f.touch_bindings_.requests&&!f.touch_bindings_.held&&!f.published.lockedEntity);
        }
        for(unsigned step=220;step<=300;step+=20){hover(start+step,side);projected54r1.entity=999;
            f.frame(start+step,1,side);if(step<300)check(!f.touch_bindings_.requests&&!f.published.lockedEntity);}
        check(f.touch_bindings_.requests==1&&!f.touch_bindings_.held&&f.pickupUse19c3.sent&&
              !f.pickupUse19c3.cancelled&&f.pickupUse19c3.target63==999&&f.published.lockedEntity==999);
        hover(start+320,side);f.frame(start+320,1,side);
        check(f.touch_bindings_.requests==1&&f.pickupUse19c3.target63==999&&f.published.lockedEntity==999);
        f.frame(start+340,0,side);check(!f.published.lockedEntity);
        for(unsigned step=360;step<=520;step+=20){hover(start+step,side);projected54r1.entity=999;
            f.frame(start+step,1,side);if(step<520)check(f.touch_bindings_.requests==1&&!f.published.lockedEntity);}
        check(f.touch_bindings_.requests==2&&f.pickupUse19c3.sent&&f.published.lockedEntity==999);
        f.frame(start+540,0,side);check(!f.published.lockedEntity);
    }
    std::printf("door_flow64: %u checks PASS; actual point/direct/Use chronology, no fabricated native acceptance\n",checks);
}
#endif
