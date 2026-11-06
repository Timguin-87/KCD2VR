    bool doorPoseH40[2]{};
    bool doorPose66[2]{};pc::LocalPose doorWrist66[2]{};
    for(int side=0;side<2;++side)if(valid[side]&&!nativeFingerCapture&&!propHeld5916c1[side]&&
        !(tx.physicalActive&&tx.physicalOutput.owned[side])&&g_doorActive66){
        const auto now66=std::uint64_t(GetTickCount64());
        if(!g_doorActive66(side,tx.epoch,now66,tx.doorSerial66[side]))continue;
        pc::LocalPose worldWrist{};pc::Vec3 handle{};std::uint64_t serial=0;int nativeSide=-1;ModelWorld6692 mw{};
        if(g_doorGoal66&&g_doorGoal66(side,tx.epoch,now66,worldWrist,handle,serial,nativeSide)&&
           serial==tx.doorSerial66[side]&&ReadModelWorld6692(character,mw)){
            const auto modelQ=pc::multiply(pc::conjugate(PQ(mw.q)),worldWrist.rotation);
            const auto modelP=WorldToModel6692(mw,VP(worldWrist.fixed_offset));
            const auto shoulder=FromT(s.work[side?rig.upper_l:rig.upper_r]);
            if(pc::finite(PV(modelP))&&Len(Sub(modelP,rawGoalsH50[side]))<=.35f&&
               Len(Sub(modelP,shoulder))<=ph::reach_max(s.upper[side],s.fore[side])){
                doorPose66[side]=true;doorWrist66[side]={modelQ,PV(modelP)};goals[side]=modelP;
            }
        }
    }
    // H40 display owns only the visible palm endpoint; the raw controller
    // request was already staged and remains the door/combat intent source.
    for(int side=0;side<2;++side)if(valid[side]&&!nativeFingerCapture&&!propHeld5916c1[side]&&!doorPose66[side]&&
        !(tx.physicalActive&&tx.physicalOutput.owned[side])&&(side?ts.left_grip:ts.right_grip)>=.30f){
        kcd2vr::hand_only_door_h40::Display shown{};ModelWorld6692 mw{};
        if(!kcd2vr::hand_only_door_h40::ReadDisplay(side,reinterpret_cast<std::uintptr_t>(character),tx.epoch,
            tx.doorRequestH40,ts.source_epoch,tx.policyGeneration57,GetTickCount64(),shown)||!ReadModelWorld6692(character,mw))continue;
        // Same four metacarpal bases as PublishCommitted6689; no universal
        // palm offset, finger endpoint substitution or new bone lengths.
        V3 centre{};bool mapped=s.fingerMap[side];
        for(int finger=1;finger<5&&mapped;++finger){const auto&chain=s.fingers[side][finger];
            if(chain.count<1||chain.id[0]<0||chain.id[0]>=s.count)mapped=false;
            else centre=Add(centre,FromT(s.restLocal[chain.id[0]]));}
        centre=Mul(centre,.125f); // average four bases, halfway to palm centre
        if(!mapped||!std::isfinite(Len(centre))||Len(centre)<=.0075f||Len(centre)>=.10f)continue;
        const auto orientation=DesiredHand6689(s,side,frame,side?ts.lq:ts.rq);
        const V3 palmWorld{shown.point[side][0],shown.point[side][1],shown.point[side][2]};
        const auto palmModel=WorldToModel6692(mw,palmWorld);
        goals[side]=Sub(palmModel,QRotateV(orientation,centre));
        doorPoseH40[side]=true;tx.doorDisplayH40[side]=shown.token[side];
    }
