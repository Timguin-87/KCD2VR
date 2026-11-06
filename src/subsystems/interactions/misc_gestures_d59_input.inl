// Lexical input adapter: uses completed palms, cached HMD view and the existing
// native candidate. Does not consume buttons, claim hands or alter ray choice.
if(misc_gestures_d59::Any()){
    namespace misc59=misc_gestures_d59;
    const auto now59=std::uint64_t(GetTickCount64());
    misc59::Input in59{};in59.tick=now59;in59.policy=misc59::policy.load(std::memory_order_acquire);
    in59.permitted=!uiBlocked&&!diagnostics::lab::NativeReference()&&xr_head_valid&&xr_manager_&&
        misc59::Ensure(now59)&&frik_pose_probe::ReadMiscGestureBodyD59(in59.body);
    tracking59c6::Stamp stamp59{};
    in59.permitted=in59.permitted&&xr_manager_->CopyInputSampleStamp59c6(stamp59)&&
        stamp59.epoch==in59.body.tracking&&petting_d59::Fresh(stamp59.tick,now59);
    in59.inputTick=stamp59.tick;
    petting_d59::Frame pet59{};pet59.tick=now59;pet59.policy=in59.policy;
    pet59.player=in59.body.playerId;pet59.playerIdentity=in59.body.player;
    pet59.body=in59.body.body;pet59.tracking=in59.body.tracking;pet59.permitted=in59.permitted;
    washing_d59::Frame wash59{};wash59.tick=now59;wash59.policy=in59.policy;
    wash59.player=in59.body.playerId;wash59.actor=in59.body.player;wash59.character=in59.body.character;
    wash59.body=in59.body.body;wash59.tracking=in59.body.tracking;
    wash59.allowed=in59.permitted&&CameraHook::GetLatestVisualPositionWorld(wash59.hmd[0],wash59.hmd[1],wash59.hmd[2],60)&&
        CameraHook::GetLatestVisualForwardWorld(wash59.forward[0],wash59.forward[1],wash59.forward[2],60);
    if(wash59.allowed)wash59.hmdTick=now59;
    const auto& candidate59=candidate59c7f;
    const bool joined59=candidate59.object&&candidate59.sourcePlayer==in59.body.player&&
        candidate59.sourceEpoch==in59.body.body&&petting_d59::Fresh(candidate59.tick,now59)&&
        petting_d59::Fresh(candidate59.sourceTick,now59);
    if(joined59&&!std::strcmp(candidate59.cls,"WaterTubeActionTrigger")){
        wash59.target=candidate59.entity;wash59.targetTick=candidate59.tick;
    }
    for(int side59=0;xr_manager_&&side59<2;++side59){
        const auto& control59=side59?xr_manager_->LeftController():xr_manager_->RightController();
        auto& hand59=pet59.hand[side59];
        hand59.tracked=in59.permitted&&control59.active&&control59.grip_valid&&control59.trigger_valid&&in59.body.valid[side59];
        hand59.empty=in59.empty[side59]=itemState116[side59]==0;
        hand59.free=in59.free[side59]=hand59.tracked&&!(reserved114&(1u<<side59))&&!(gripMask116&(side59?1u:2u));
        hand59.grip=in59.grip[side59]=control59.grip;hand59.trigger=in59.trigger[side59]=control59.trigger;
        for(int k59=0;k59<3;++k59)hand59.palm[k59]=wash59.palm[side59][k59]=in59.body.palm[side59][k59];
        interaction_alignment::DogHandCandidateD59R1 dog59{};
        if(hand59.free&&hand59.empty&&petting_d59::Enabled()&&
           interaction_alignment::ReadDogHandCandidateD59R1(side59,now59,dog59)&&
           dog59.owner==side59&&dog59.sourcePlayer==in59.body.player&&dog59.sourceEpoch==in59.body.body&&
           petting_d59::Fresh(dog59.tick,now59)&&petting_d59::Fresh(dog59.sourceTick,now59)){
            hand59.target=dog59.entity;hand59.candidateTick=dog59.tick;
            hand59.candidateGeneration=dog59.source.generation;hand59.candidateContext=dog59.source.context;
            hand59.distance=dog59.distance;hand59.dogCandidate=true;
        }
        if(hand59.tracked&&hand59.empty&&hand59.free&&std::isfinite(hand59.grip)&&std::isfinite(hand59.trigger)&&
           hand59.grip>=0&&hand59.grip<.35f&&hand59.trigger>=0&&hand59.trigger<.35f){
            wash59.openEmptyMask|=1u<<side59;wash59.handTick[side59]=in59.body.tick;
        }
    }
    (void)misc59::current.publish(in59);
    petting_d59::Publish(pet59);washing_d59::Publish(wash59);
}
