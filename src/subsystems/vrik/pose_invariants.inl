    if(s.deferPassengers137&&!ComposeFull137(s))return RejectCoherent6693(tx,__LINE__);
    for(std::uint32_t j=0;j<count;++j)if(s.pending137[j])return RejectCoherent6693(tx,__LINE__);
    // Sparse invariants: requested controller target, allowed/reachable endpoint,
    // solved wrist and hierarchy composition. No per-joint/frame dump.
    if(tx.traceH29&&rung>=2&&ts.tick>=s.lastInvariantLog&&ts.tick-s.lastInvariantLog>=2000) {
        s.lastInvariantLog=ts.tick;
        if(full){const V3 actual=FromT(s.work[rig.head]),chest=FromT(s.work[rig.spine3]);char b[640]{};
            std::snprintf(b,sizeof(b),"V66.115R1 HEAD_BODY sample=%llu target=(%.4f,%.4f,%.4f) solved=(%.4f,%.4f,%.4f) errorM=%.4f chest=(%.4f,%.4f,%.4f) hmdDelta=(%.4f,%.4f,%.4f) eyeTuning=%.3f/%.3f anchorZ=%.4f pelvisZ=%.4f neutralHeadZ=%.4f floorEye=%.4f standingEye=%.4f cameraCoincidence=UNVERIFIED",
                (unsigned long long)ts.sequence,headTarget.x,headTarget.y,headTarget.z,actual.x,actual.y,actual.z,Len(Sub(actual,headTarget)),chest.x,chest.y,chest.z,
                ts.head.x-trackedBase.x,ts.head.y-trackedBase.y,ts.head.z-trackedBase.z,double(g_eye_forward115.load()),double(g_eye_up115.load()),double(baseHead.z),double(s.original[rig.pelvis].t[2]),double(s.neutral[rig.head].t[2]),double(g_eye_floor_m107.load()),double(g_standing_eye_floor_m107.load()));diagnostic_trace::Write(b);}
        for(int side=0;side<2;++side)if(valid[side]) {
            const int u=side?rig.upper_l:rig.upper_r,h=side?rig.hand_l:rig.hand_r,c=side?rig.clav_l:rig.clav_r;
            const V3 solved=FromT(s.work[h]);const float reqErr=Len(Sub(solved,requestedGoals[side]));
            const float preClampErr=Len(Sub(solved,goals[side]));
            const float allowedErr=Len(Sub(solved,ikAllowed113[side]));
            const Q4 desired=DesiredHand6689(s,side,frame,side?ts.lq:ts.rq);
            const Q4 allowedWrist=(tx.physicalActive&&tx.physicalOutput.owned[side])?QP(((tx.physicalOutput.displayLatched[side]||tx.physicalOutput.displayRejoinR13[side])?tx.physicalOutput.displayHands[side]:tx.physicalOutput.hands[side]).q):desired;
            const double wristErr=pc::angle(PQ(allowedWrist),PQ(QFromQuatT(s.work[h])));
            const double controllerDelta=pc::angle(PQ(desired),PQ(allowedWrist));
            unsigned fingerMask159=0;for(int finger=0;finger<5;++finger)if(stagedContact[side].fingerContact[finger])fingerMask159|=1u<<finger;
            auto pointSegmentGap159=[](V3 p,V3 a,V3 b,double radius)->double{const V3 ab=Sub(b,a);const double d2=double(Dot(ab,ab));double t=d2>1e-9?double(Dot(Sub(p,a),ab))/d2:0.0;t=std::clamp(t,0.0,1.0);return double(Len(Sub(p,Add(a,Mul(ab,float(t))))))-radius;};
            const V3 pelvis159=FromT(s.work[rig.pelvis]),chest159=FromT(s.work[rig.spine3]);
            const double torsoRadius159=std::clamp(double(Len(Sub(FromT(s.work[rig.upper_r]),FromT(s.work[rig.upper_l]))))*.32,.08,.16);
            const double torsoGap159=pointSegmentGap159(solved,pelvis159,chest159,torsoRadius159);
            double otherHandGap159=-1.0;if(valid[1-side]){const int oh=side?rig.hand_r:rig.hand_l;otherHandGap159=double(Len(Sub(solved,FromT(s.work[oh]))))-.10;}
            char d[1180]{};std::snprintf(d,sizeof(d),
                "V66.159 HAND_CONTACT side=%c sample=%llu epoch=%llu requested=(%.4f,%.4f,%.4f) allowed=(%.4f,%.4f,%.4f) solved=(%.4f,%.4f,%.4f) reqErr=%.4f preClampErr=%.4f allowedErr=%.4f solverWristErrDeg=%.2f controllerToAllowedDeg=%.2f fixedArm=%.4f/%.4f reachLimited=%d handBlocked=%d armBlocked=%d armSaturated=%d planes=%d fingerMask=0x%02X features=%d torsoGapM=%.4f otherHandGapM=%.4f worldProxy=PALM_PLUS_FIVE_FINGERS worldScope=SEE_NPC_QUERY_WITNESS selfBodyCollision=TORSO_PROXY otherHandCollision=RUNG_DEPENDENT_VISUAL_PROXY dynamicEntityCollision=QUALIFIED_QUERY_ONLY shoulder=(%.4f,%.4f,%.4f) clavicle=(%.4f,%.4f,%.4f)",
                side?'L':'R',(unsigned long long)ts.sequence,(unsigned long long)tx.epoch,
                requestedGoals[side].x,requestedGoals[side].y,requestedGoals[side].z,ikAllowed113[side].x,ikAllowed113[side].y,ikAllowed113[side].z,
                solved.x,solved.y,solved.z,reqErr,preClampErr,allowedErr,float(wristErr*57.295779513),float(controllerDelta*57.295779513),s.upper[side],s.fore[side],
                s.reachLimited[side]?1:0,s.handBlocked[side]?1:0,s.armBlocked[side]?1:0,s.armSaturated[side]?1:0,
                stagedContact[side].planeCount,fingerMask159,stagedContact[side].featureCount,torsoGap159,otherHandGap159,
                FromT(s.work[u]).x,FromT(s.work[u]).y,FromT(s.work[u]).z,FromT(s.work[c]).x,FromT(s.work[c]).y,FromT(s.work[c]).z);
            diagnostic_trace::Write(d);
            if(collisionMode){char owner[256]{};std::snprintf(owner,sizeof(owner),
                "V66.164R1 HAND_CONTACT_OWNER side=%c free=%d itemState=%d yieldMask=%u maskBits=ITEM:1,WEAPON:2,NATIVE_USE:4,CAPTURE:8,UNKNOWN:16,BOW_STRING:32",
                side?'L':'R',freeContact164R1[side],contactItemState164R1[side],contactYield164R1[side]);diagnostic_trace::Write(owner);}
            if(collisionMode&&selfContact167){
                char proxy[320]{};std::snprintf(proxy,sizeof(proxy),
                    "V66.164 SELF_CONTACT side=%c mask=%u correctionM=%.4f resolved=%d stage=BEFORE_WORLD_SWEEP proxy=TORSO_CAPSULE_OTHER_PALM fixedBones=1 rigidBody=0 meshContact=0",
                    side?'L':'R',selfMask164[side],selfCorrection164[side],selfResolved164[side]);diagnostic_trace::Write(proxy);}
        }
    }
    if(tx.traceH29&&rung>=2&&ts.tick>=s.lastHierarchyLog&&ts.tick-s.lastHierarchyLog>=5000) {
        s.lastHierarchyLog=ts.tick;float maxCompose=0,maxOffsetDrift=0,maxShoulderRootShift=0;int checked=0;
        const int parents[6]{rig.head,rig.neck,rig.clav_r,rig.clav_l,rig.upper_r,rig.upper_l};
        for(int p:parents)for(std::uint32_t j=0;j<count;++j)if(s.parent[j]==p){
            const auto expected=AbsoluteFromRelative(s.work[p],s.local[j]);
            maxCompose=std::max(maxCompose,Len(Sub(FromT(expected),FromT(s.work[j]))));
            // R38 scapular root translation is an explicit DOF. Keep it visible
            // separately; humerus/forearm/finger offsets still must be fixed.
            if(int(j)==rig.upper_r||int(j)==rig.upper_l)
                maxShoulderRootShift=std::max(maxShoulderRootShift,Len(Sub(FromT(s.local[j]),FromT(s.restLocal[j]))));
            else maxOffsetDrift=std::max(maxOffsetDrift,std::fabs(Len(FromT(s.local[j]))-Len(FromT(s.restLocal[j]))));
            ++checked;
        }
        char h[480]{};std::snprintf(h,sizeof(h),"V66.92-HIERARCHY INHERIT checked=%d maxComposePosErr=%.6f maxLocalOffsetDrift=%.6f shoulderRootShiftM=%.6f shoulderWarningsD39=%llu wristAuthority=TRACKING_OR_CONTACT fullUpper=1 measurement=STAGED_BUFFER_ONLY nativeConsumerChecked=0 interpretation=%s",
            checked,maxCompose,maxOffsetDrift,maxShoulderRootShift,(unsigned long long)kcd2vr::shoulder_reach_d38::capped.load(std::memory_order_relaxed),(maxCompose<.0005f&&maxOffsetDrift<.0025f)?"STAGED_HIERARCHY_CONSISTENT":"STAGED_OFFSET_ALERT");diagnostic_trace::Write(h);
    }
    if(full&&tx.traceH29) {
        const V3 ctrl[2]{ts.right,ts.left};const Q4 ctrlQ[2]{ts.rq,ts.lq};
        auto establish=[&](){
            s.stableWindow=true;s.stableSince=ts.tick;s.stableHmd=ts.hq;
            for(int side=0;side<2;++side){s.stableCtrl[side]=ctrl[side];s.stableCtrlQ[side]=ctrlQ[side];s.stableReq[side]=requestedGoals[side];s.stableSolved[side]=FromT(s.work[side?rig.hand_l:rig.hand_r]);}
        };
        if(!s.stableWindow)establish();
        else {
            bool moved=false;
            for(int side=0;side<2;++side)if(valid[side]){
                if(Len(Sub(ctrl[side],s.stableCtrl[side]))>.008f||pc::angle(PQ(ctrlQ[side]),PQ(s.stableCtrlQ[side]))>.05236)moved=true;
            }
            if(moved)establish();
            else {
                const double hmdAngle=pc::angle(PQ(ts.hq),PQ(s.stableHmd));
                if(ts.tick>=s.stableSince+120&&hmdAngle>.30&&ts.tick-s.lastHmdInvariantLog>=400) {
                    float maxReq=0,maxSolved=0;
                    for(int side=0;side<2;++side)if(valid[side]){
                        maxReq=std::max(maxReq,Len(Sub(requestedGoals[side],s.stableReq[side])));
                        maxSolved=std::max(maxSolved,Len(Sub(FromT(s.work[side?rig.hand_l:rig.hand_r]),s.stableSolved[side])));
                    }
                    char d[512]{};std::snprintf(d,sizeof(d),
                        "V66.92-VRIK-HMD-INVARIANCE sample=%llu stableMs=%llu hmdDeltaDeg=%.2f controllerPosMax=0.008 controllerRotMaxDeg=3.0 requestedHandDelta=%.4f solvedHandDelta=%.4f frameLeak=%d interpretation=%s",
                        (unsigned long long)ts.sequence,(unsigned long long)(ts.tick-s.stableSince),hmdAngle*57.295779513,maxReq,maxSolved,maxReq>.015f?1:0,
                        maxReq>.015f?"TRACKING_FRAME_ALERT":(maxSolved>.030f?"BODY_OR_CONSTRAINT_MOVED_HAND":"PASS_REQUESTED_AND_SOLVED_STABLE"));
                    diagnostic_trace::Write(d);s.lastHmdInvariantLog=ts.tick;establish();
                }
            }
        }
    } else s.stableWindow=false; // No hidden diagnostic history/work outside F7.
