#pragma once
#include "../src/vrik/workstation178r20c1.h"
#include "../src/diagnostics/interaction_cycle_h4.h"
#include "../src/subsystems/physics/native_grab_input60.h"
// Explicit native/committed-pose leaves. Default is no physical ownership;
// the D40 producer fixture also supplies independent existing hand owners.
namespace fixture_grab_d40 {
inline bool pose[2]{false,false},statusReady[2]{false,false};
inline std::uint64_t poseEpochD41[2]{7,7},trackingEpochD41[2]{3,3};
inline kcd2vr::physics::grab_producer60::Status status[2]{};
}
namespace kcd2vr::physics::grab_producer60 {
inline bool ReadStatus(int side,Status& out) noexcept {
    out={};if(side<0||side>1||!fixture_grab_d40::statusReady[side])return false;
    out=fixture_grab_d40::status[side];return true;
}
}
namespace kcd2vr::frik_pose_probe {
inline bool ReadGrabHandWorld60(int side,float p[3],float q[4],std::uint64_t& pe,std::uint64_t& te,std::uintptr_t& player,std::uint64_t& tick) noexcept {
    if(side<0||side>1||!fixture_grab_d40::pose[side])return false;
    p[0]=float(side);p[1]=p[2]=0;q[0]=q[1]=q[2]=0;q[3]=1;pe=fixture_grab_d40::poseEpochD41[side];te=fixture_grab_d40::trackingEpochD41[side];player=0x1234;tick=clock54;return true;
}
}
// Shared copied XR, pose, selection and input-transport leaves. The production
// Use producer is included verbatim; no native action outcome is simulated here.
struct Producer {
    pickup_use19c3::State pickupUse19c3{};door_use46::Controller doorUse46;
    held_interaction114::State heldUse114{};
    physical_interaction106::Input pi{};
    unsigned f5Mode=2,grabMask59c7f=0;
    bool comparison114=true,rayPublished116=true,alchemyPointer59c2=false,native_xi_full=true,f5OwnedRay=false;
    std::uint64_t poseEpoch=7,trackingEpoch=3,poseAge=0,sourceAge=0,frameWait54=0;std::uintptr_t player=0x1234;
    bool empty=true,tracked=true,ui=false,forceGenericPulseH31=false;
    bool retainOtherGripD40=false;
    bool surfaceEnabled65=true;float surfaceDistance65=.08f;std::uint32_t surfaceEntity65=0;
    kcd2vr::door19c4::Point handWorld65[2]{};
    bool surfaceHit65(std::uint32_t target,std::uint64_t now,int side=0){
        kcd2vr::door_hold65::Frame frame{};if(!surfaceEnabled65||side<0||side>1||!kcd2vr::door_hold65::frame.read(frame))return false;
        const auto& h=frame.hand[side];
        const auto point=kcd2vr::door19c4::Point{h.point.x,h.point.y+surfaceDistance65,h.point.z};
        const auto request=kcd2vr::door_surface65::request(target,h,now,point);
        if(!kcd2vr::door_surface65::offer(request,now))return false;
        kcd2vr::door_surface65::Witness w{};w.source=request;w.queryTick=now;w.sample=now;w.actualPalm=h.point;w.point=point;w.normal={0,-1,0};
        w.entityId=surfaceEntity65?surfaceEntity65:target;w.opaqueOwner=0x5678;w.opaqueCollider=0x6789;
        w.distance=surfaceDistance65;w.valid=true;return kcd2vr::door_surface65::publish(w);
    }
    struct Hand {float grip=0,trigger=0;struct {float pos_x=0,pos_y=1.2f,pos_z=0;}pose;};
    struct XR {Hand hands[2];std::uint64_t tick=0,epoch=3;
        const Hand& RightController(){return hands[0];}const Hand& LeftController(){return hands[1];}
        bool CopyInputSampleStamp59c6(tracking59c6::Stamp& s){s.tick=tick;s.epoch=epoch;return true;}
    } xr;
    XR* xr_manager_=&xr;
    struct Transport {
        bool held=false,allow=true;unsigned requests=0,risingHeldEdgesH31=0;std::uint64_t tick=0,pulseEnd=0,queuedPressD48=0;
        bool SetPhysicalInteractionHeld114(bool on){const bool next=allow&&on;if(next&&!held)++risingHeldEdgesH31;held=next;return held;}
        bool RequestPhysicalInteractionUse(std::uint64_t originalPress=0){++requests;if(allow){pulseEnd=tick+80;queuedPressD48=originalPress;}return allow;}
        void ValidatePhysicalInteractionPressD48(std::uint64_t press,bool eligible){
            if(pulseEnd&&queuedPressD48&&(!eligible||press!=queuedPressD48)){pulseEnd=0;queuedPressD48=0;}}
        bool value() const{return held||(pulseEnd&&tick<pulseEnd);}
    } touch_bindings_;
    void tick(std::uint64_t ladder_now_ms,float grip,int hand=0){
        clock54=ladder_now_ms+frameWait54;
        pi={};pi.enabled=true;pi.menu=ui;pi.now_ms=ladder_now_ms;
        for(int side=0;side<2;++side){if(!retainOtherGripD40||side==hand)xr.hands[side].grip=side==hand?grip:0;
            pi.hand[side]={tracked,empty,true,xr.hands[side].grip,false};}
        xr.tick=clock54-sourceAge;xr.epoch=trackingEpoch;touch_bindings_.tick=ladder_now_ms;
        const auto held114=held_interaction114::update(heldUse114,pi);
        physical_interaction106::Output po{};po.ray_owner=held114.owner;po.ray_active=held114.ray;
        po.fire_use=forceGenericPulseH31; // explicit upstream pulse to verify downstream suppression
        palm59c9::Pair palms59c9{};
        for(int side=0;side<2;++side){palms59c9.eligible[side]=tracked&&empty;
            auto& ray=palms59c9.ray[side];ray.hand=true;ray.owner=side;ray.tick=clock54-poseAge;ray.sourceEpoch=poseEpoch;ray.sourcePlayer=player;
            ray.ox=handWorld65[side].x;ray.oy=handWorld65[side].y;ray.oz=handWorld65[side].z;}
        // Explicit copied upstream point/lock source for this isolated consumer
        // fixture. door_flow64 executes the real point producer instead.
        palms59c9.locked=held114.owner;
        palms59c9.lockedEntity=palm59c9::ordinaryTransportLock64.observe(palms59c9,held114.owner,heldUse114.pressed,
            door_use46::inputMode(f5Mode)?interaction_alignment::DoorAcquisitionGeneration65():interaction_alignment::DoorSourceGeneration38(),trackingEpoch,door_use46::settingsEpoch.load(),
            kcd2vr::workstation178r20c1::experimentEpoch36.load(),f5Mode,clock54);
        kcd2vr::door_hold65::Frame hold65{};
        for(int side=0;side<2;++side){auto& h=hold65.hand[side];h.available=door_use46::inputAvailable(f5Mode,comparison114)&&!ui&&tracked&&empty;
            h.side=side;h.grip=xr.hands[side].grip;h.tick=clock54;h.poseTick=clock54-poseAge;h.player=player;
            h.poseEpoch=poseEpoch;h.trackingEpoch=trackingEpoch;h.sourceGeneration=interaction_alignment::DoorAcquisitionGeneration65();
            h.settingsEpoch=door_use46::settingsEpoch.load();h.experimentEpoch=kcd2vr::workstation178r20c1::experimentEpoch36.load();
            h.press=held114.owner==side?heldUse114.pressed:0;h.point=handWorld65[side];}
        (void)kcd2vr::door_hold65::frame.publish(hold65);
        if(held114.owner>=0){
            const int side=held114.owner;
            interaction_alignment::HandCandidate native65{};
            const bool nominated65=(interaction_alignment::NativeDirectDoorCandidate54r1(side,clock54,native65)||interaction_alignment::LastHandCandidate(native65))&&
               native65.owner==side&&!std::strcmp(native65.cls,"AnimDoor");
            (void)kcd2vr::door_surface65::withdraw(side,clock54,clock54);
            if(nominated65)
                (void)surfaceHit65(native65.entity,clock54,side); // explicit native physical-hit leaf
        }
        bool heldNow114=false,useSent116=false;
        // This isolated door fixture has no upstream weapon/UI Grip reservation.
        constexpr unsigned reserved114=0,gripMask116=0;
#include "../src/vrik/pickup_use_producer19c3.inl"
    }
};
