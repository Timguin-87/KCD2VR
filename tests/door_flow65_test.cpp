// Actual point -> completed native descriptor -> copied pose-owner physical
// hit -> next point -> actual Use producer. Only native query/transport leaves
// are supplied; no native accepted Door or Lua progress is invented here.
#define KCD2VR_DOOR_FLOW_TYPES_ONLY65
#include "door_flow64_test.cpp"
int main(){
    using namespace DetourModKit::hook;
    // Real F5 admission increments this source-family epoch. A default-zero
    // synthetic epoch is deliberately not a usable door source in production.
    kcd2vr::combat_rules178r10::f5Rung=2;
    kcd2vr::workstation178r20c1::setExperiment36(true);
    alignas(8) std::uintptr_t entity[8]{};entity[0]=g_wh_base+0x4072C78;
    alignas(8) std::uintptr_t player[8]{};player[0]=g_wh_base+0x3AA4B98;player[7]=reinterpret_cast<std::uintptr_t>(entity);
    alignas(8) std::uintptr_t interactor[2]{g_wh_base+0x3A6A0C8,reinterpret_cast<std::uintptr_t>(player)};
    alignas(8) std::uintptr_t policy[2]{g_wh_base+kObjectPolicy,reinterpret_cast<std::uintptr_t>(player)};
    alignas(8) unsigned char payload[0x38]{};const std::uint32_t target=42;
    std::memcpy(payload+0x10,&target,4);expectedInteractor=reinterpret_cast<std::uintptr_t>(interactor);
    expectedPlayer=reinterpret_cast<std::uintptr_t>(player);
    MidContext ctx{};gpr(ctx,Gpr::R15)=expectedInteractor;gpr(ctx,Gpr::R12)=reinterpret_cast<std::uintptr_t>(policy);
    gpr(ctx,Gpr::Rdx)=reinterpret_cast<std::uintptr_t>(payload);ctx.ip=0x6404E1;
    // Native descriptor centre is neither the palm nor the surface: it can be
    // 1.4 m away while the existing committed-palm query hits its near edge.
    const float centre[3]{0,1.4f,0};std::memcpy(payload+0x28,centre,sizeof(centre));
    auto reset=[&](){(void)door_use46::intent.publish({});(void)door_use46::acceptedIntent62.publish({});
        (void)door_hold65::frame.publish({});(void)door_hold65::operation.publish({});(void)door_hold65::resume.publish({});
        (void)door_surface65::requestMailbox.publish({});(void)door_surface65::witnessMailbox.publish({});
        g_direct_door54r1={};g_door_candidates38.clear();projected54r1={};palm59c9::ordinaryTransportLock64={};};
    auto native=[&](std::uint64_t now,float distance=.08f,std::uint32_t returned=42){clock54=now;
        const auto before=ctx;NativeDirectDoor54r1(ctx);(void)physicalSurface65(now,distance,returned);NativeDirectDoor54r1(ctx);
        check(!std::memcmp(&before,&ctx,sizeof(ctx)));};
    // Construct at the call site: DoorFlow64 owns xr_manager_=&xr64.
    // Returning a named local can copy that pointer to a dead factory stack
    // when Debug does not perform optional NRVO (reproduced on frozen M1).
    auto fixture=[&](DoorFlow64& f){f.player=reinterpret_cast<std::uintptr_t>(entity);f.actualRayPublication65=true;
        check(f.xr_manager_==&f.xr64);};
    hands59c7f::rung=2;hands59c7f::admitted=true;hands59c7f::pickupReady=false;
    action_time_owner59c11::nativeGraspAvailable64=true;
    // Grip is already held for400ms. Trigger's heuristic cannot withdraw a
    // current verified door-only palm; no genuine weapon/fist owner is active.
    for(unsigned mode:{0u,2u})for(int side=0;side<2;++side){reset();DoorFlow64 f;fixture(f);f.f5Mode=mode;f.comparison114=mode!=0;const auto start=1000+side*1000;
        f.xr64.hands[side].trigger=.20f;f.frame(start,0,side);
        for(unsigned t=20;t<=420;t+=20){f.frame(start+t,1,side);
            check(!f.touch_bindings_.requests&&!f.touch_bindings_.held&&!f.doorUse46.cancelled&&f.point123.owner==-1);
            check(!f.rayPublished116&&!HandRayActive()&&!DoorSourceGeneration38()&&DoorAcquisitionGeneration65()!=0);}
        native(start+425);HandCandidate candidate{};check(NativeDirectDoorCandidate54r1(side,start+425,candidate));
        check(candidate.entity==target&&candidate.distance==.08f&&candidate.y==.08f);
        f.frame(start+440,1,side);check(f.touch_bindings_.requests==1&&!f.touch_bindings_.held&&f.point123.owner==side);
        door_use46::Intent intent{};check(door_use46::read(intent,start+440)&&intent.target==target&&intent.press==start+20);
        f.frame(start+460,0,side);door_hold65::Frame released{};check(door_hold65::frame.read(released));
        check(released.hand[side].available&&released.hand[side].grip==0&&released.hand[side].press==0);
    }
    // H12: F5=3 must publish the same exact fresh hand/acquisition family as
    // mode2 before the native acceptance callback selects its LEGACY owner.
    // The old fixture assigned f5Mode2 while claiming to exercise rung3.
    for(int side=0;side<2;++side){reset();combat_rules178r10::f5Rung=3;
        workstation178r20c1::setExperiment36(true);
        DoorFlow64 f;fixture(f);f.f5Mode=3;const auto start=20000+side*1000;
        f.xr64.hands[side].trigger=.20f;f.frame(start,0,side);
        for(unsigned t=20;t<=420;t+=20){f.frame(start+t,1,side);
            door_hold65::Frame actual{};check(door_hold65::frame.read(actual));
            check(door_hold65::current(actual.hand[side],clock54)&&actual.hand[side].press==start+20);
            check(!f.touch_bindings_.requests&&!f.touch_bindings_.held&&!f.doorUse46.cancelled);}
        native(start+425);HandCandidate c{};check(NativeDirectDoorCandidate54r1(side,start+425,c)&&c.entity==target);
        f.frame(start+440,1,side);check(f.touch_bindings_.requests==1&&!f.touch_bindings_.held);
        door_use46::Input input{};check(door_use46::currentInput62.read(input)&&input.interactionMode64==3&&
            door_use46::inputCurrent62(input,start+440)&&input.press==start+20);
        door_use46::Intent intent{};check(door_use46::read(intent,start+440)&&intent.target==target&&intent.side==side);
        f.frame(start+460,0,side);check(!door_use46::read(intent,start+460));
    }
    // Mode/context changes before transport cannot turn the old squeeze into
    // a new door Use. Both directions require a real release and new squeeze.
    for(unsigned initial:{2u,3u})for(unsigned change=0;change<5;++change){reset();
        combat_rules178r10::f5Rung=initial;workstation178r20c1::setExperiment36(true);
        DoorFlow64 f;fixture(f);f.f5Mode=initial;const auto start=23000+initial*1000+change*1000;
        f.xr64.hands[0].trigger=.20f;f.frame(start,0);f.frame(start+20,1);
        check(f.doorUse46.classifiedSource64.press==start+20);
        if(change==0){f.f5Mode=initial==2?3:2;combat_rules178r10::f5Rung=f.f5Mode;workstation178r20c1::setExperiment36(true);}
        if(change==1)f.ui=true;
        if(change==2)f.itemState116[0]=1;
        if(change==3)f.reserved114=1;
        if(change==4)f.poseAge=81;
        f.frame(start+40,1);native(start+45);f.frame(start+60,1);
        check(!f.touch_bindings_.requests&&!f.touch_bindings_.held);
        f.ui=false;f.itemState116[0]=0;f.reserved114=0;f.poseAge=0;
        f.frame(start+80,1);native(start+85);f.frame(start+100,1);
        check(!f.touch_bindings_.requests&&!f.touch_bindings_.held);
        f.frame(start+120,0);f.frame(start+140,1);f.frame(start+160,1);
        native(start+165);f.frame(start+180,1);
        check(f.touch_bindings_.requests==1&&!f.touch_bindings_.held);
        f.frame(start+200,0);
    }
    combat_rules178r10::f5Rung=2;workstation178r20c1::setExperiment36(true);
    // Actual surface distance/identity, never the descriptor centre or the
    // unrelated other hand, grants the door-only pointer capability.
    for(int bad=0;bad<4;++bad){reset();DoorFlow64 f;fixture(f);const auto start=4000+bad*1000;
        f.xr64.hands[0].trigger=.20f;f.frame(start,0);f.frame(start+20,1);f.frame(start+40,1);
        if(bad==2)f.reserved114=1;if(bad==3)f.itemState116[0]=1;
        f.frame(start+60,1);native(start+65,bad==0?.201f:.08f,bad==1?99u:42u);
        f.frame(start+80,1);check(!f.touch_bindings_.requests&&!f.touch_bindings_.held&&f.point123.owner==-1);
        door_hold65::Frame h{};check(door_hold65::frame.read(h));if(bad>=2)check(!h.hand[0].available);
    }
    // An indefinitely waiting raw source is still ONE press/family. A later
    // valid20cm target after recenter/player/settings changes cannot revive it.
    for(int change=0;change<6;++change){reset();DoorFlow64 f;fixture(f);const auto start=9000+change*1000;
        f.xr64.hands[0].trigger=.20f;f.frame(start,0);f.frame(start+20,1);
        check(f.doorUse46.classifiedSource64.press==start+20);
        if(change==0)++f.poseEpoch;if(change==1)++f.trackingEpoch;if(change==2)++g_door_acquisition_generation65;
        if(change==3){door_use46::SetEnabled(false);door_use46::SetEnabled(true);}
        if(change==4)workstation178r20c1::experimentEpoch36.fetch_add(1);if(change==5)f.player+=8;
        for(unsigned t=40;t<=420;t+=20)f.frame(start+t,1);
        native(start+425);f.frame(start+440,1);
        check(!f.touch_bindings_.requests&&!f.touch_bindings_.held&&f.doorUse46.transportRetired64);
    }
    // The separate accepted-Use deadline and tiny tracking grace remain real.
    {reset();DoorFlow64 f;fixture(f);f.xr64.hands[0].trigger=.20f;f.frame(16000,0);f.frame(16020,1);
        f.poseAge=81;f.frame(16040,1);f.poseAge=0;f.frame(16060,1);native(16065);f.frame(16080,1);
        check(!f.touch_bindings_.requests&&!f.touch_bindings_.held);}
    // Explicit already-native-accepted retained operation leaf. New Grip near
    // that SAME door publishes only Resume; it cannot queue another Use, even
    // if the worker completes the operation while this Grip stays held.
    {reset();DoorFlow64 f;fixture(f);f.xr64.hands[0].trigger=.20f;f.frame(17000,0);f.frame(17020,1);
        native(17025);f.frame(17040,1);check(f.touch_bindings_.requests==1);
        f.frame(17060,0);door_hold65::Frame frame{};check(door_hold65::frame.read(frame));
        door_hold65::Operation op{};op.tick=17060;op.source=frame.hand[0];op.paused=true;
        op.request.serial=71;op.request.id=target;op.request.entity=0x5678;op.request.side=0;op.request.player=op.source.player;
        op.request.epoch=op.source.poseEpoch;op.request.sourceGeneration=op.source.sourceGeneration;
        op.request.trackingEpoch46=op.source.trackingEpoch;op.request.inputSettingsEpoch46=op.source.settingsEpoch;
        op.request.inputExperimentEpoch65=op.source.experimentEpoch;op.request.inputPress65=17020;
        check(door_hold65::current(op,17060));(void)door_hold65::operation.publish(op);
        f.frame(17080,1);check(!physicalSurface65(17080,.201f)); // too far cannot regrip
        f.frame(17100,1);check(f.touch_bindings_.requests==1&&!f.doorUse46.resumedPress65);
        check(physicalSurface65(17100));f.frame(17120,1);
        door_hold65::Resume resume{};check(door_hold65::resume.read(resume)&&resume.target==target&&resume.serial==71&&resume.hand.press==17080);
        check(f.touch_bindings_.requests==1&&f.doorUse46.resumedPress65==17080&&!f.touch_bindings_.held);
        (void)door_hold65::operation.publish({});f.frame(17140,1);check(f.touch_bindings_.requests==1&&!f.touch_bindings_.held);
        f.frame(17160,0);}
    std::printf("door_flow65: %u checks PASS; actual delayed squeeze/trigger/surface/Use/regrip source chronology\n",checks);
}
