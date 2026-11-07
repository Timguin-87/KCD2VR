// Actual staging/attachment/mechanism implementations with fixture native
// memory and PRS leaves. Checks the real native grip point across body scale,
// handed socket identity, animated descriptors, gaps and heavy mount handover.
#define main socketLegacyMainR95R2
#include "crossbow_socket_d78_test.cpp"
#undef main
static unsigned gripChecksR95R2=0;
#define G95(c) do{++gripChecksR95R2;if(!(c)){std::cerr<<"FAIL_GRIP_R95R2 line="<<__LINE__<<" " #c "\n";return 1;}}while(0)
static void scaledWorldR95R2(float scale) {
    const double q=std::sqrt(.5);world.q={0,0,q,q};world.t={10,20,30};world.scale=scale;naturalScale=scale;
    put(object,0x0C,1.2f);put(object,0x10,.8f);put(object,0x14,.9f);
}
static pm::Vec3 currentWristR95R2(){return PhysCurrentWorld164(PhysPose6698(body.allowed[0]),world).p;}
static void handSourceR95R2(int kind) {
    source.classId=kind;source.physicalD81=true;state.owner.source=source;
    xcXR34::statusMailbox.publish(state);xcXR34::PublishSource(source);
}
static int mediumGripR95R2(float scale) {
    reset(false);completedPublicationR93R2=2;g_committed_pose_valid6689=true;tracking59c6::epoch=1;
    scaledWorldR95R2(scale);handSourceR95R2(14);bootstrap();const auto first=receipt78();
    // Native wrist was zero and untouched socket origin was +0.11 on X.
    // With natural entity scale == body scale, the corresponding asset grip
    // is -0.11 / its native X scale. This is independent of the receipt math.
    const pm::Vec3 nativeAssetGrip{-.11/1.2,0,0};
    G95(first.valid&&near(RangedPointMatrixXR34(entityWorld,nativeAssetGrip),currentWristR95R2()));
    G95(near(first.worldGrip,currentWristR95R2()));
    const auto captured=g_ranged_grip_xr34.grip;const auto firstTick=first.tick;const auto writes=placed;
    body.reloadTargetXR34.valid=false;
    G95(CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==writes);
    G95(receipt78().valid&&receipt78().tick==firstTick);
    // Recover with a new real wrist and animated native attachment descriptor.
    // The same item-to-hand grip must remain fixed through this native motion.
    now+=20;source.tick=now;++source.sample;body.stamp.tick=now;body.stamp.sample=source.sample;
    body.allowed[0].t[0]=.4f;body.allowed[0].t[1]=.8f;body.allowed[0].t[2]=1.2f;
    const float q=float(std::sqrt(.5));body.allowed[0].q[2]=q;body.allowed[0].q[3]=q;
    QuatT28 animated{};animated.t[0]=.31f;animated.t[1]=.2f;put(attachment,0x9C,animated);
    body.publicationR24=completedPublicationR93R2=4;xcXR34::PublishSource(source);
    BuildCrossbowPosesXR34(body,original,1);CommitReloadBodyFixtureR95(body);
    G95(body.reloadTargetXR34.valid&&std::memcmp(&captured,&body.reloadTargetXR34.grip,sizeof(captured))==0);
    G95(CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==writes+1);
    const auto recovered=receipt78();G95(recovered.valid&&recovered.tick==now);
    G95(near(RangedPointMatrixXR34(entityWorld,nativeAssetGrip),currentWristR95R2())&&near(recovered.worldGrip,currentWristR95R2()));
    // A real child-token replacement cannot use the retained placement/grip.
    put(nativeChild,0x54,18u);G95(!CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==writes+1);
    return 0;
}
// This suite uses the physical switch with Instant disabled. The legacy
// socket fixture also exercises Instant; both reach the same native writer.
static void physicalHeavyR95R2() {
    heavy78();source.instantD72=false;source.physicalD81=true;state.owner.source=source;publish78();
}
static int heavyGripR95R2(float scale) {
    physicalHeavyR95R2();scaledWorldR95R2(scale);observe78();G95(place78());
    auto r=receipt78();G95(r.valid&&near(r.worldGrip,currentWristR95R2()));
    const auto captured=g_ranged_heavy_grip_d72.grip;
    state.toolMountedD78=true;publish78();G95(place78());r=receipt78();
    RangedMechanismD78 observation{};G95(g_ranged_mechanism_d78.read(observation)&&observation.valid);
    const auto nativeGrip=ph::compose(PhysPose6698(observation.toolModel),captured);
    const auto expected=PhysCurrentWorld164(ph::compose(RangedWristPoseD72(source.weaponModel),nativeGrip),world);
    G95(r.valid&&r.mountedD78&&near(r.worldGrip,expected.p));
    body.allowed[0].t[0]+=1;G95(place78()&&near(receipt78().worldGrip,expected.p)); // released hand cannot drag mounted lever
    return 0;
}
static int heavyPublicationGapR95R2(bool mounted) {
    physicalHeavyR95R2();observe78();G95(place78());state.toolMountedD78=mounted;publish78();G95(place78());
    const auto first=receipt78();const auto writes=placed;
    // Native pre-update invalidates the passenger before post-update supplies
    // its successor. The completed body and actual achieved tool are unchanged.
    xcXR34::passengerPublication.fetch_add(1);
    G95(place78()&&placed==writes&&receipt78().valid&&receipt78().tick==first.tick);
    now+=20;source.tick=now;++source.sample;body.stamp.tick=now;body.stamp.sample=source.sample;
    body.allowed[0].t[0]+=.1f;publish78();G95(place78()&&placed==writes+1&&receipt78().tick==now);
    if(!mounted)G95(near(receipt78().worldGrip,currentWristR95R2()));
    // The same exact binding has a missing auxiliary weapon-pose publication.
    // Consume without a write until the unchanged native clock is observed.
    g_ranged_mechanism_d78.publish({});const auto pausedWrites=placed;
    G95(place78()&&placed==pausedWrites&&receipt78().valid);
    observe78();G95(place78()&&placed==pausedWrites+1&&receipt78().valid);
    xcXR34::passengerPublication.fetch_add(1);now=source.tick+81;
    G95(!place78()&&placed==pausedWrites+1); // no deadline renewal from repeated callbacks
    physicalHeavyR95R2();observe78();G95(place78());state.placeTool=false;xcXR34::statusMailbox.publish(state);
    G95(!place78());return 0;
}
static int heavyTrueRevocationR95R2() {
    for(int fault=0;fault<8;++fault){
        physicalHeavyR95R2();observe78();G95(place78());const auto captured=state;const auto writes=placed;
        xcXR34::passengerPublication.fetch_add(1);
        if(fault==0)liveContextD78=false;
        if(fault==1){state.placeTool=false;xcXR34::statusMailbox.publish(state);}
        if(fault==2){state.toolMountedD78=true;xcXR34::statusMailbox.publish(state);}
        if(fault==3){state.owner.serial++;xcXR34::statusMailbox.publish(state);}
        if(fault==4)put(nativeChild,0x54,18u);
        if(fault==5)entityWorld.m[3]+=.01f;
        if(fault==6)tracking59c6::epoch++;
        if(fault==7){std::memcpy(other.data()+0x1500,toolItem.data(),0x100);put(mainAction,0x80,static_cast<void*>(other.data()+0x1500));}
        G95(!CrossbowHeavyToolAttachmentD72(captured,object.data(),attachment.data(),77,toolItem.data(),entity.data())&&placed==writes);
    }
    for(bool during:{false,true}){
        physicalHeavyR95R2();observe78();G95(place78());const auto writes=placed;
        auto revoke=+[](){state.placeTool=false;xcXR34::statusMailbox.publish(state);};
        if(during)placementReentryXR34=revoke;else worldReadReentryR93R2=revoke;
        G95(place78()==during&&placed==writes+unsigned(during)&&!receipt78().valid);
    }
    return 0;
}
int main() {
    for(float scale:{.8f,1.f,1.35f,2.f})if(mediumGripR95R2(scale)||heavyGripR95R2(scale))return 1;
    for(bool mounted:{false,true})if(heavyPublicationGapR95R2(mounted))return 1;
    if(heavyTrueRevocationR95R2())return 1;
    for(int kind:{10,14,15}){
        reset(true);completedPublicationR93R2=2;g_committed_pose_valid6689=true;tracking59c6::epoch=1;
        scaledWorldR95R2(1.35f);handSourceR95R2(kind);bootstrap();const auto placedBolt=receipt78();
        G95(placedBolt.valid&&placedBolt.bolt&&near(placedBolt.worldGrip,currentWristR95R2()));
        G95(boltShowCallsR95==1&&placedBolt.socketR95.side==0&&placedBolt.id==77);
        const auto writes=placed;body.reloadTargetXR34.valid=false;
        G95(CrossbowAttachmentXR34(object.data(),attachment.data())&&placed==writes&&receipt78().tick==placedBolt.tick);
    }
    std::cout<<"PASS_GRIP_R95R2 checks="<<gripChecksR95R2<<" native medium/heavy grips and all-class right-hand bolt; fixture PRS/context leaves\n";
    return 0;
}
