// Actual D78 socket reader, untouched skinning observer and mounted/hand
// attachment writers. Retail lookup/API/pose-job scheduling are explicit leaves.
#include <iostream>
#define KCD2VR_XR34_POSE_MEMORY_SIZE 0x1600
#define KCD2VR_XR34_POSE_NO_MAIN
#define KCD2VR_XR34_POSE_EXTERNAL_RELOAD
#define KCD2VR_XR34_ACTUAL_INCLUDE "crossbow_pose_d78_actual.inc"
#include "crossbow_pose_xr34_test.cpp"
static bool liveContextD78=true,poseApiD78=true,unstablePoseD78=false,weaponBindingD78=true;
static void RangedReloadEquipmentXR34() noexcept{}
// These leaves model the same live context for both passenger and retained
// placement paths. Exact owner/native binding checks below remain production.
static bool ReloadContextXR34(const xcXR34::Owner& o,std::uint64_t) noexcept {
    return liveContextD78&&o.source.tracking==tracking59c6::epoch.load();
}
static bool ReloadNativeOwnerXR34(const xcXR34::Owner& o,bool) noexcept {return RangedOwnerXR34(o);}
static bool ReloadPoseWriterCurrentXR34(const xcXR34::Owner& o,std::uint64_t) noexcept {return RangedOwnerXR34(o);}

static unsigned poseReadsD78=0;
static xcXR34::Clock liveClockD78{};
static QuatT28 nativePoseD78{},nativeStringD78{};
static void* weaponCharD78(){return other.data()+0x200;}
static void* weaponPoseD78(){return other.data()+0xAE0;}
static void* weaponSkeletonD78(){return other.data()+0x1000;}
static void* weaponManagerD78(){return other.data()+0x1100;}
static void* nockD78(){return other.data()+0x1200;}
static void* ManualFindNockAttachment19c4r4(void*) noexcept{return nockD78();}
using PoseGetFn=const QuatT28*(*)(void*,int);
static const QuatT28* nativeGetterD78(void*,int joint){return joint==1?&nativeStringD78:&nativePoseD78;}
static bool ReadValidatedCharInstanceFromSkinning(void* plus,void*& ci,void*& animation) noexcept {
    ci=weaponCharD78();animation=static_cast<unsigned char*>(ci)+0x110;
    return plus==static_cast<unsigned char*>(ci)+8&&PhysVft6698(ci,0x40602F8);
}
static bool ReadValidatedDefaultSkeletonFromPose(void* p,void*& skel) noexcept {skel=weaponSkeletonD78();return p==weaponPoseD78();}
static bool ReadJointCount(void* s,std::uint32_t& count) noexcept {count=12;return s==weaponSkeletonD78();}
static bool ValidateRawPoseApi(void* p,PoseGetFn& reference,PoseGetFn& current) noexcept {
    reference=current=&nativeGetterD78;return poseApiD78&&p==weaponPoseD78();
}
static bool ReadQuat(PoseGetFn fn,void* p,int joint,QuatT28& out) noexcept {
    ++poseReadsD78;out=*fn(p,joint);if(unstablePoseD78&&!(poseReadsD78%2))out.t[0]+=.02f;return true;
}
static bool ReloadPassengerCurrentXR34(const xcXR34::Passenger& p,std::uintptr_t c,std::uint64_t epoch,std::uint64_t tick) noexcept {
    return liveContextD78&&c==source.character&&epoch==source.body&&tick==now&&xcXR34::sameMechanismClockD78(p.clock,liveClockD78);
}
static bool ReadReloadPassengerXR34(xcXR34::Passenger& p,std::uintptr_t c,std::uint64_t epoch,std::uint64_t tick) noexcept {
    return xcXR34::ReadPoseLeaseD87(p,tick)&&ReloadPassengerCurrentXR34(p,c,epoch,tick);
}
// Native current-clock/identity is the only fixture leaf for D86 observation.
// The production mailbox and exact Source/Passenger joins execute unchanged.
static bool ReloadMechanismCurrentD86(const xcXR34::Passenger& p,std::uint64_t tick) noexcept {
    xcXR34::Passenger current{};
    return liveContextD78&&tick==now&&xcXR34::ReadMechanismWitnessD86(current,tick)&&
        current.publication==p.publication&&xcXR34::sameOwner(current.owner,p.owner)&&
        xcXR34::sameMechanismClockD78(p.clock,liveClockD78);
}
#include "crossbow_mechanism_d78_actual.inc"
static unsigned socketChecksD78=0,socketFailsD78=0;
#define S78(c) do{++socketChecksD78;if(!(c)){++socketFailsD78;std::cerr<<"FAIL_SOCKET_D78 line="<<__LINE__<<" " #c "\n";}}while(0)
static void publish78(){xcXR34::statusMailbox.publish(state);xcXR34::PublishSource(source);
    xcXR34::ReadSource(source);xcXR34::Passenger p{};p.owner=state.owner;p.frame=source;p.workingHand=0;p.childStatus=2;
    p.root=reinterpret_cast<std::uintptr_t>(scope.data());p.publication=xcXR34::passengerPublication.fetch_add(1)+1;
    p.clip.qualified=true;p.clip.cocking=true;p.clip.spawn=.2f;p.clock=liveClockD78;
    S78(RangedNativeBindingXR34(state.owner,p.binding));p.binding.sample=source.sample;p.binding.tick=source.tick;
    p.valid=true;xcXR34::passengerMailbox.publish(p);
}
static void heavy78(){completedPublicationR93R2=2;g_committed_pose_valid6689=true;tracking59c6::epoch=1;worldReadReentryR93R2=nullptr;placementReentryXR34=nullptr;reset(false);liveContextD78=poseApiD78=d78BindingOK=weaponBindingD78=contract=true;unstablePoseD78=false;poseReadsD78=0;
    source.instantD72=true;source.intentKnownD78=true;source.classId=15;source.hand[0]={1,2,3};
    state.owner.source=source;state.toolTakenD72=true;state.toolMountedD78=false;
    put(other,0x200,base+0x40602F8);put(other,0xC70,weaponSkeletonD78());put(other,0x1000,base+0x406B930);
    put(other,0x1170,weaponCharD78());put(attachment,0x28,weaponManagerD78());
    nativePoseD78={};nativePoseD78.t[0]=.1f;nativePoseD78.t[1]=.2f;nativePoseD78.t[2]=.3f;
    nativeStringD78={};nativeStringD78.t[1]=.31f;nativeStringD78.t[2]=.06f;
    QuatT28 rel{};Read(attachment.data()+0x9C,rel);const auto nativeTool=PhysPose6698(AbsoluteFromRelative(nativePoseD78,rel));
    const ph::Pose grip{{},{.05,.02,.01}};const auto wrist=ph::compose(nativeTool,grip);
    source.nativeWrist[0]={{float(wrist.q.x),float(wrist.q.y),float(wrist.q.z),float(wrist.q.w)},
        {float(wrist.p.x),float(wrist.p.y),float(wrist.p.z)}};
    liveClockD78={};liveClockD78.qualified=true;liveClockD78.entry=reinterpret_cast<std::uintptr_t>(other.data()+0x1400);
    liveClockD78.segment=0;liveClockD78.normalized=.2f;liveClockD78.activeTime=.1f;
    liveClockD78.playbackScale=liveClockD78.controllerScale=liveClockD78.characterScale=1;
    source.nativePoseClockD78=liveClockD78.entry;source.nativePoseNormalizedD78=liveClockD78.normalized;source.nativePoseActiveTimeD78=liveClockD78.activeTime;
    source.completedLease=state.owner.serial;
    g_ranged_heavy_grip_d72={};g_ranged_mechanism_d78.publish({});publish78();
}
static void observe78(){CrossbowMechanismSkinningD78(weaponPoseD78(),weaponSkeletonD78(),static_cast<unsigned char*>(weaponCharD78())+8);}
static bool place78(){const auto captured=state;return CrossbowHeavyToolAttachmentD72(captured,object.data(),attachment.data(),77,toolItem.data(),entity.data());}
static RangedPlacedXR34 receipt78(){RangedPlacedXR34 r{};g_ranged_placed_xr34.read(r);return r;}
int main(){
    // Actual heavy attachment writer after Henry completed N+1 while the
    // native clock remains fixed. Both unmounted hand and mounted tool use
    // the new committed body. A stale body is denied; an unchanged achieved
    // unmounted tool may remain bounded while its new clock witness is pending.
    for(bool mounted:{false,true})for(int fault=-1;fault<3;++fault){
        heavy78();observe78();S78(place78());state.toolMountedD78=mounted;publish78();
        ++now;++source.sample;source.tick=now;body.stamp.sample=source.sample;body.stamp.tick=now;
        body.allowed[0].t[0]=1.2f;xcXR34::PublishSource(source);observe78();
        if(fault==0)body.stamp.sample--;
        if(fault==1)liveClockD78.normalized+=.01f;
        if(fault==2){source.completedLease=0;xcXR34::PublishSource(source);}
        xcXR34::Passenger strict{};S78(!xcXR34::ReadPassenger(strict,now));
        const auto writes=placed;const auto oldReceipt=receipt78();
        const bool retainOnly=!mounted&&(fault==1||fault==2);
        S78(place78()==(fault==-1||retainOnly));
        if(retainOnly)S78(placed==writes&&receipt78().valid&&receipt78().sample==oldReceipt.sample&&receipt78().tick==oldReceipt.tick);
        if(fault==-1)S78(receipt78().valid&&receipt78().sample==source.sample&&receipt78().mountedD78==mounted);
    }
    // D86 ordering: controller publishes P(N), Henry commits/publishes N+1,
    // then the weapon skinning job runs while the exact native clock is held.
    // Old code used ReadPassenger and rejected every such string observation.
    for(int kind:{10,14,15}){
        heavy78();source.classId=kind;state.owner.source=source;
        if(kind==10)put(mainAction,0x80,static_cast<void*>(nullptr));
        publish78();
        ++now;++source.sample;source.tick=now;body.stamp.sample=source.sample;body.stamp.tick=now;
        xcXR34::PublishSource(source);
        xcXR34::Passenger exact{},native{};
        S78(!xcXR34::ReadPassenger(exact,now)); // old wrist frame stays forbidden
        S78(xcXR34::ReadMechanismWitnessD86(native,now));
        observe78();RangedMechanismD78 observed{};
        S78(g_ranged_mechanism_d78.read(observed)&&observed.valid&&observed.sample==native.frame.sample&&observed.tick==native.frame.tick&&observed.stringValid);
        // Changed native clock cannot borrow the old observation publication.
        liveClockD78.normalized+=.01f;observe78();
        S78(g_ranged_mechanism_d78.read(observed)&&!observed.valid);
    }
    for(int fault=0;fault<6;++fault){
        heavy78();publish78();++source.sample;++now;source.tick=now;xcXR34::PublishSource(source);
        if(fault==0){source.allowed=false;xcXR34::PublishSource(source);}
        if(fault==1){++source.request;xcXR34::PublishSource(source);}
        if(fault==2)++xcXR34::passengerPublication;
        if(fault==3)xcXR34::revokedNativeLease=state.owner.serial;
        if(fault==4){now+=81;source.tick=now;xcXR34::PublishSource(source);}
        if(fault==5){state.sourceEstablished=false;xcXR34::statusMailbox.publish(state);}
        xcXR34::Passenger p{};S78(!xcXR34::ReadMechanismWitnessD86(p,now));
    }
    // A body publication in flight must not prevent a read-only weapon job
    // from producing the original-frame string receipt the controller awaits.
    for(int kind:{10,14,15}){
        heavy78();source.classId=kind;state.owner.source=source;
        if(kind==10)put(mainAction,0x80,static_cast<void*>(nullptr));
        publish78();currentSourceAvailableR92=false;observe78();RangedMechanismD78 saved{};
        S78(g_ranged_mechanism_d78.read(saved)&&saved.valid&&saved.stringValid);
        S78(saved.sample==source.sample&&saved.tick==source.tick&&saved.observed==now);
        // A transient publication mismatch retains, but never renews, this
        // exact observation. The existing 80-ms budget still expires it.
        const auto publication=xcXR34::passengerPublication.load();++xcXR34::passengerPublication;++now;
        observe78();RangedMechanismD78 retained{};S78(g_ranged_mechanism_d78.read(retained)&&retained.valid);
        S78(retained.tick==saved.tick&&retained.observed==saved.observed&&retained.sample==saved.sample);
        xcXR34::passengerPublication=publication;
        now+=80;source.tick=now;++source.sample;xcXR34::PublishSource(source);
        S78(!RangedReadStringD78(state.owner,source,retained));
    }
    // Source grace and observation grace cannot add together into 160 ms.
    heavy78();observe78();RangedMechanismD78 originalAge{};
    S78(g_ranged_mechanism_d78.read(originalAge)&&originalAge.valid);
    originalAge.tick=now-40;originalAge.observed=now;
    g_ranged_mechanism_d78.publish(originalAge);now+=50; // source 50ms, receipt 90ms
    xcXR34::Spatial ageBinding{};S78(RangedNativeBindingXR34(state.owner,ageBinding));
    S78(!RangedReadStringD78(state.owner,source,originalAge));
    S78(!RangedReadMechanismD78(state.owner,source,ageBinding,originalAge));
    heavy78();const auto untouched=nativePoseD78;observe78();RangedMechanismD78 observed{};
    S78(g_ranged_mechanism_d78.read(observed)&&observed.valid&&observed.pose==weaponPoseD78());
    S78(!std::memcmp(&untouched,&nativePoseD78,sizeof(untouched))&&poseReadsD78==4);
    S78(place78());auto r=receipt78();S78(r.valid&&!r.mountedD78&&near(r.worldGrip,{1,2,3}));
    const auto grip=g_ranged_heavy_grip_d72.grip;S78(near(grip.p,{.05,.02,.01}));
    // Native socket data are untouched by the previous free-hand entity write.
    xcXR34::Spatial binding{};S78(RangedNativeBindingXR34(state.owner,binding));
    S78(RangedReadMechanismD78(state.owner,source,binding,observed)&&std::abs(observed.toolModel.t[0]-.11f)<1e-5f);
    state.toolMountedD78=true;publish78();S78(place78());r=receipt78();
    S78(r.valid&&r.mountedD78&&near(r.worldGrip,{.16,.22,.31}));
    // Rotating around the grip leaves the point readback unchanged. The exact
    // achieved basis is a separate requirement for a mounted-mechanism proof.
    const auto achieved=entityWorld;Matrix34V6550 spun{};spun.m[1]=-1;spun.m[4]=1;spun.m[10]=1;
    spun.m[3]=float(r.worldGrip.x+r.localGrip.y);spun.m[7]=float(r.worldGrip.y-r.localGrip.x);spun.m[11]=float(r.worldGrip.z-r.localGrip.z);
    entityWorld=spun;S78(!RangedPlacementStillCurrentXR34(r));entityWorld=achieved;
    RangedMechanismSpatialD78(state.owner,source,binding,&r);
    S78(binding.toolMechanismD72&&binding.toolMountedPoseD78&&near({binding.toolHandleD72.x,binding.toolHandleD72.y,binding.toolHandleD72.z},{.16,.22,.31}));
    // Grip release frees the hand, but the same lever stays on the weapon.
    state.toolEngagedD72=false;body.allowed[0].t[0]=9;body.allowed[0].t[1]=8;body.allowed[0].t[2]=7;publish78();
    S78(place78()&&near(receipt78().worldGrip,{.16,.22,.31}));
    nativePoseD78.q[2]=.5f;nativePoseD78.q[3]=float(std::sqrt(.75));nativePoseD78.t[1]=.28f;
    liveClockD78.normalized=.4f;liveClockD78.activeTime=.2f;publish78();observe78();
    S78(place78());r=receipt78();
    g_ranged_mechanism_d78.read(observed);const auto moved=ph::compose(PhysPose6698(observed.toolModel),grip);
    S78(r.valid&&r.mountedD78&&near(r.worldGrip,moved.p)&&!near(r.worldGrip,{9,8,7}));
    S78(std::abs(entityWorld.m[0]-.5)<1e-5&&std::abs(entityWorld.m[4]-std::sqrt(.75))<1e-5);
    // Old tool destination is a distraction, never another native pose sample.
    entityWorld.m[3]=98;entityWorld.m[7]=97;entityWorld.m[11]=96;
    S78(RangedReadMechanismD78(state.owner,source,binding,observed)&&std::abs(observed.toolModel.t[1]-.28866025f)<1e-5f);
    S78(place78()&&near(receipt78().worldGrip,moved.p));
    // A newly observed animated relative offset of the SAME socket keeps the
    // original grip after Spawn; an unobserved/stale descriptor still refuses.
    QuatT28 changed{};Read(attachment.data()+0x9C,changed);changed.t[0]+=.03f;put(attachment,0x9C,changed);
    S78(!RangedReadMechanismD78(state.owner,source,binding,observed));observe78();S78(place78());
    S78(receipt78().valid&&near(g_ranged_heavy_grip_d72.grip.p,grip.p));
    for(bool mounted:{false,true}){
        heavy78();observe78();S78(place78());const auto capturedGrip=g_ranged_heavy_grip_d72.grip;
        state.toolMountedD78=mounted;liveClockD78.normalized=.4f;liveClockD78.activeTime=.2f;
        QuatT28 relative{};Read(attachment.data()+0x9C,relative);relative.t[0]+=.03f;put(attachment,0x9C,relative);
        body.allowed[0].t[0]=9;body.allowed[0].t[1]=8;body.allowed[0].t[2]=7;
        publish78();observe78();S78(place78());auto placedNow=receipt78();RangedMechanismD78 current{};
        S78(RangedNativeBindingXR34(state.owner,binding)&&RangedReadMechanismD78(state.owner,source,binding,current));
        const auto nativeGrip=ph::compose(PhysPose6698(current.toolModel),capturedGrip);
        S78(placedNow.valid&&placedNow.mountedD78==mounted&&near(placedNow.worldGrip,mounted?nativeGrip.p:pm::Vec3{9,8,7}));
        S78(near(g_ranged_heavy_grip_d72.grip.p,capturedGrip.p));
        RangedMechanismSpatialD78(state.owner,source,binding,&placedNow);S78(binding.toolMechanismD72&&binding.toolMountedPoseD78==mounted);
    }
    // Post-Spawn native identity replacement can never borrow an old grip.
    for(int fault=0;fault<6;++fault){
        heavy78();observe78();S78(place78());liveClockD78.normalized=.4f;liveClockD78.activeTime=.2f;publish78();
        if(fault==0)put(toolItem,0x30,std::uint64_t(124));
        if(fault==1)put(attachment,0x78,std::int16_t(2));
        if(fault==2){state.owner.serial++;xcXR34::statusMailbox.publish(state);}
        if(fault==3)entityLookup=other.data();
        if(fault==4)put(attachment,0x20,static_cast<void*>(other.data()));
        if(fault==5){std::memcpy(other.data()+0x1500,toolItem.data(),0x100);put(mainAction,0x80,static_cast<void*>(other.data()+0x1500));}
        observe78();const auto count=placed;const bool consumed=place78();xcXR34::Spatial currentBinding{};
        const bool bindingCurrent=RangedNativeBindingXR34(state.owner,currentBinding);
        // A rejected native lookup may return before consuming the callback;
        // its preceding receipt has no credit without that same current binding.
        S78(!consumed&&placed==count&&(!receipt78().valid||!bindingCurrent));
    }
    // Publication/liveness/identity invalidation before a write refuses it;
    // invalidation inside native placement consumes the callback, without a
    // valid receipt or a second original/native placement in this callback.
    for(bool during:{false,true})for(int fault=0;fault<8;++fault){
        heavy78();observe78();S78(place78());
        void (*invalidate)()=nullptr;
        if(fault==0)invalidate=+[](){completedPublicationR93R2+=2;};
        if(fault==1)invalidate=+[](){g_committed_pose_valid6689=false;};
        if(fault==2)invalidate=+[](){tracking59c6::epoch++;};
        if(fault==3)invalidate=+[](){QuatT28 r{};Read(attachment.data()+0x9C,r);r.t[0]+=.02f;put(attachment,0x9C,r);};
        if(fault==4)invalidate=+[](){put(toolItem,0x30,std::uint64_t(124));};
        if(fault==5)invalidate=+[](){state.owner.serial++;xcXR34::statusMailbox.publish(state);};
        if(fault==6)invalidate=+[](){put(attachment,0x78,std::int16_t(2));};
        if(fault==7)invalidate=+[](){now+=81;};
        if(during)placementReentryXR34=invalidate;else worldReadReentryR93R2=invalidate;
        const auto count=placed;
        // An animated descriptor alone does not replace this native entity or
        // its already achieved matrix. Await its new mechanism observation.
        const bool retainedPublicationR95=!during&&(fault==0||fault==3);
        S78(place78()==(during||retainedPublicationR95));S78(placed==count+unsigned(during)&&receipt78().valid==retainedPublicationR95);
        placementReentryXR34=nullptr;worldReadReentryR93R2=nullptr;
    }
    heavy78();observe78();S78(place78());state.toolMountedD78=true;publish78();
    readbackOK=false;const auto before=placed;S78(place78()&&placed==before+1&&!receipt78().valid);readbackOK=true;
    // Failure still consumes the callback, so a second native writer cannot
    // overwrite a partial PRS update in the same callback.
    heavy78();observe78();S78(place78());state.toolMountedD78=true;publish78();
    placementReentryXR34=+[](){state.toolMountedD78=false;xcXR34::statusMailbox.publish(state);};
    S78(place78()&&!receipt78().valid);placementReentryXR34=nullptr;
    for(int fault=0;fault<7;++fault){heavy78();observe78();S78(place78());
        if(fault==0)unstablePoseD78=true;
        if(fault==1)poseApiD78=false;
        if(fault==2)liveContextD78=false;
        if(fault==3)d78BindingOK=false;
        if(fault==4)put(attachment,0x7A,static_cast<unsigned char>(1));
        if(fault==5)contract=false;
        if(fault==6)weaponBindingD78=false;
        observe78();S78(g_ranged_mechanism_d78.read(observed)&&!observed.valid);
    }
    heavy78();observe78();S78(place78());now+=81;source.tick=now;++source.sample;body.stamp.tick=now;body.stamp.sample=source.sample;publish78();
    S78(!RangedReadMechanismD78(state.owner,source,binding,observed));
    heavy78();source.nativePoseNormalizedD78=.1f;publish78();observe78();
    S78(!place78()&&!g_ranged_heavy_grip_d72.ready);
    heavy78();observe78();CrossbowMechanismSkinningD78(entity.data(),weaponSkeletonD78(),static_cast<unsigned char*>(weaponCharD78())+8);
    S78(g_ranged_mechanism_d78.read(observed)&&!observed.valid);
    for(int kind:{10,14}){heavy78();source.classId=kind;state.owner.source=source;
        if(kind==10)put(mainAction,0x80,static_cast<void*>(nullptr));
        publish78();nativeStringD78.t[0]=.013f;nativeStringD78.t[1]=.19f;observe78();
        xcXR34::Spatial actual{};RangedMechanismSpatialD78(state.owner,source,actual,nullptr);
        S78(actual.stringMechanismD78&&near({actual.stringCurrentD78.x,actual.stringCurrentD78.y,actual.stringCurrentD78.z},{.013,.19,.06}));
        entityWorld.m[3]=99;entityWorld.m[7]=98;entityWorld.m[11]=97;
        RangedMechanismSpatialD78(state.owner,source,actual,nullptr);
        S78(actual.stringMechanismD78&&near({actual.stringCurrentD78.x,actual.stringCurrentD78.y,actual.stringCurrentD78.z},{.013,.19,.06}));
        now+=81;source.tick=now;++source.sample;body.stamp.tick=now;body.stamp.sample=source.sample;publish78();
        RangedMechanismSpatialD78(state.owner,source,actual,nullptr);S78(!actual.stringMechanismD78);
    }
    // A native-created item already present at pose-build time needs no
    // earlier attachment callback to become controller-owned. Both reloads
    // publish their own owner/identity; no previous cycle target is reused.
    for(bool bolt:{false,true}){
        reset(bolt);source.instantD72=true;
        for(unsigned cycle=1;cycle<=2;++cycle){
            // Keep previous socket/target mailboxes alive into the next native
            // action. The new owner must recapture its own original grip.
            if(cycle==2){original[0].q[2]=original[0].q[3]=float(std::sqrt(.5));
                ++now;++source.sample;source.tick=now;body.stamp.sample=source.sample;body.stamp.tick=now;}
            state.owner.source=source;state.owner.serial=cycle;
            xcXR34::activeNativeLease=xcXR34::completedNativeLease=cycle;
            xcXR34::statusMailbox.publish(state);xcXR34::PublishSource(source);
            BuildCrossbowPosesXR34(body,original,1);CommitReloadBodyFixtureR95(body);
            S78(CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==cycle);
            const auto r=receipt78();S78(r.valid&&r.owner.serial==cycle&&near(r.worldGrip,{1,2,3}));
            if(cycle==2)S78(std::abs(entityWorld.m[0])<1e-6&&std::abs(entityWorld.m[4]-1)<1e-6);
        }
    }
    // R93: the engine advances its relative slot AFTER the target body was
    // committed. Both the right-hand bolt and medium lever stay at the current
    // controller; no new body frame or recapture is required for that offset.
    for(bool bolt:{false,true}){
        reset(bolt);source.physicalD81=true;state.owner.source=source;
        xcXR34::statusMailbox.publish(state);xcXR34::PublishSource(source);
        BuildCrossbowPosesXR34(body,original,1);CommitReloadBodyFixtureR95(body);
        QuatT28 animated{};Read(attachment.data()+0x9C,animated);
        animated.t[0]+=.25f;animated.q[2]=animated.q[3]=float(std::sqrt(.5));put(attachment,0x9C,animated);
        S78(CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==1);
        const auto achievedR93=receipt78();S78(achievedR93.valid&&near(achievedR93.worldGrip,{1,2,3}));
        // A different actual socket joint still cannot borrow the target.
        put(attachment,0x78,std::int16_t(1));
        S78(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==1);
    }
    // Instant hand-ammunition grip is captured once from untouched native
    // slot/wrist data. Native slot animation cannot rotate it in the VR hand.
    reset(true);source.instantD72=true;state.owner.source=source;xcXR34::statusMailbox.publish(state);xcXR34::PublishSource(source);bootstrap();
    const auto firstEntity=entityWorld;original[0].q[2]=original[0].q[3]=float(std::sqrt(.5));
    BuildCrossbowPosesXR34(body,original,1);CommitReloadBodyFixtureR95(body);S78(CrossbowAttachmentXR34(object.data(),attachment.data()));
    S78(std::abs(entityWorld.m[0]-firstEntity.m[0])<1e-6&&std::abs(entityWorld.m[4]-firstEntity.m[4])<1e-6);
    // Losing a destination for one frame does not change the same item's
    // rigid calibration. The invalid frame grants no stale destination write.
    body.valid[0]=false;BuildCrossbowPosesXR34(body,original,1);CommitReloadBodyFixtureR95(body);const auto beforeGap=placed;
    S78(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==beforeGap);
    body.valid[0]=true;BuildCrossbowPosesXR34(body,original,1);CommitReloadBodyFixtureR95(body);
    S78(CrossbowAttachmentXR34(object.data(),attachment.data()));
    S78(std::abs(entityWorld.m[0]-firstEntity.m[0])<1e-6&&std::abs(entityWorld.m[4]-firstEntity.m[4])<1e-6);
    // The same real bolt stays at the controller even if the native slot
    // itself animates during the hold. A changed owner/helper is still a new
    // calibration, and placement requires the exact current descriptor.
    QuatT28 slotAnimation{};Read(attachment.data()+0x9C,slotAnimation);
    slotAnimation.q[2]=slotAnimation.q[3]=float(std::sqrt(.5));slotAnimation.t[0]+=.1f;
    put(attachment,0x9C,slotAnimation);BuildCrossbowPosesXR34(body,original,1);CommitReloadBodyFixtureR95(body);
    S78(CrossbowAttachmentXR34(object.data(),attachment.data()));
    S78(std::abs(entityWorld.m[0]-firstEntity.m[0])<1e-6&&std::abs(entityWorld.m[4]-firstEntity.m[4])<1e-6);
    r=receipt78();S78(r.valid&&r.helperFrameD78);
    // Only an actual current arrow socket with inverse(slt_0) establishes the
    // frame convention. Unknown geometry retains compatibility and is labeled.
    put(other,0x1200,base+0x3A657C0);put(other,0x1228,static_cast<void*>(manager.data()));put(other,0x1278,std::int16_t(3));
    const auto inverseHelper=qt(ph::inverse(PhysPose6698(helper)));put(other,0x129C,inverseHelper);
    ManualItems596 items{};items.visual=other.data();Matrix34V6550 weapon{};weapon.m[0]=weapon.m[5]=weapon.m[10]=1;
    xcXR34::Spatial result{};result.handBoltPlaced=true;
    RangedBoltAlignmentD78(state.owner,items,character.data(),3,{},weapon,r,result);
    S78(result.boltAlignmentKnownD78&&result.boltAlignedD78&&result.handBoltPlaced);
    // Rotate the achieved bolt 90 degrees about its unchanged nock point.
    Matrix34V6550 rotated{};rotated.m[1]=-1;rotated.m[4]=1;rotated.m[10]=1;
    rotated.m[3]=float(r.worldGrip.x-r.localGrip.y*-1);rotated.m[7]=float(r.worldGrip.y-r.localGrip.x);rotated.m[11]=float(r.worldGrip.z-r.localGrip.z);
    entityWorld=rotated;S78(!RangedPlacementStillCurrentXR34(r));r.achievedD78=rotated;
    result={};result.handBoltPlaced=true;RangedBoltAlignmentD78(state.owner,items,character.data(),3,{},weapon,r,result);
    S78(result.boltAlignmentKnownD78&&!result.boltAlignedD78&&!result.handBoltPlaced);
    put(other,0x129C,QuatT28{});result={};result.handBoltPlaced=true;
    RangedBoltAlignmentD78(state.owner,items,character.data(),3,{},weapon,r,result);
    S78(!result.boltAlignmentKnownD78&&result.handBoltPlaced);
    std::cout<<"D86 native mechanism ordering / socket observer / mounted writer: "<<(socketChecksD78-socketFailsD78)<<"/"<<socketChecksD78<<" PASS; native API/pose jobs mocked\n";
    return socketFailsD78?1:0;
}
