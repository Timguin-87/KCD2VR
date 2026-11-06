    if(full) {
        tx.stage="TORSO_RECONSTRUCTION";
        // Use this transaction's neutral pose (the supplied patch read the last
        // transaction's neutral cache before Compose). Eye rotation and translation
        // are separate: retain 1:1 HMD translation, subtract only the eye-pivot arc.
        const Q4 neutralChest=QFromQuatT(s.neutral[rig.spine3]);
        const auto neutralAxes=pc::chest_axes103(PV(QRotateV(neutralChest,s.chestRightLocal)),PV(QRotateV(neutralChest,s.chestUpLocal)));
        if(!neutralAxes)return RejectCoherent6693(tx,__LINE__);
        const V3 eyeOffset=VP(neutralAxes->forward*g_eye_forward115.load()+neutralAxes->up*g_eye_up115.load());
        kickInput176.forward=PV(QRotateV(delta,VP(neutralAxes->forward)));
        tx.kickFrame176=kick176::advance(stagedKick176,kickInput176);
        if(tx.kickFrame176.serial){tx.kickSerial176=tx.kickFrame176.serial;tx.kickCancel176=kickInput176.cancel;}
        if(tx.kickFrame176.active){
            const int hip=s.leg[tx.kickFrame176.side].hip;
            for(std::uint32_t j=0;j<count;++j)if(Descendant6689(s,int(j),hip))tx.selected[j]=true;
        }
        headTarget=Sub(headTarget,Sub(QRotateV(delta,eyeOffset),eyeOffset));
        // Orient the connected trunk first, then solve positions with fixed offsets.
        const int ids[6]{rig.spine1,rig.spine2,rig.spine3,rig.neck,rig.head,rig.spine};
        // Model-axis rotation-vector distribution. X behaves pitch-like, Y roll-like,
        // Z yaw-like after the established OpenXR->Cry basis conversion. Neck/head
        // absorb most pitch/roll; chest follows yaw more than head tilt.
        const double d2r=3.14159265358979323846/180.0;
        // CL-B (Claude, Miki 3 Oct: 'the chest should move with me when I look down, neck rigid, bend in the abdomen'): the PITCH
        // share of the trunk is one knob. 0 = the old table (neck 65 %, chest 18 % capped 12 deg: the chest stays up and hides the
        // belt). 1 = neck 90 %, chest 60 % / 40 deg, spine2 30 %, spine1 12 %. INI VrChestPitchShare, default 0 (trial OFF). yaw/roll unchanged.
        const double pitchShare=std::clamp(double(ChestPitchShareCB()),0.0,1.0);
        auto mixW=[&](double a,double b){return a+(b-a)*pitchShare;};
        const pc::Vec3 weights[6]{{mixW(.05,.12),.02,.10},{mixW(.10,.30),.03,.22},{mixW(.18,.60),.06,.40},{mixW(.65,.90),.45,.75},{1,1,1},{mixW(.02,.04),.01,.04}};
        const pc::Vec3 limits[6]{{mixW(4,15)*d2r,2*d2r,10*d2r},{mixW(8,25)*d2r,3*d2r,20*d2r},{mixW(12,40)*d2r,5*d2r,35*d2r},
                                 {mixW(45,55)*d2r,30*d2r,65*d2r},{179*d2r,179*d2r,179*d2r},{mixW(2,5)*d2r,1*d2r,5*d2r}};
        auto yawQ128=[](double a){return QP(kcd2vr::body_follow_r38::yaw(a));};
        Q4 swing128=delta;float headYaw128=0.f,chestYaw128=0.f,handBias128=0.f;
        {
            const float tl=std::sqrt(delta.z*delta.z+delta.w*delta.w);
            if(tl>1e-6f){const Q4 twist{0.f,0.f,delta.z/tl,delta.w/tl};
                headYaw128=float(kcd2vr::body_follow_r38::wrapped(2.*std::atan2(twist.z,twist.w)));
                swing128=QNormalize(QMul(delta,QConj(twist)));}
        }
        // R38: infer a continuous hip heading from this exact HMD sample. The
        // helper stores a model-local delta, so entity/native world turn is not
        // added a second time. Recenter/tracking/basis/station changes retire it.
        kcd2vr::body_follow_r38::Input headingInputR38{};
        headingInputR38.owner.character=reinterpret_cast<std::uintptr_t>(character);
        headingInputR38.owner.pose=tx.epoch;headingInputR38.owner.tracking=ts.source_epoch;
        headingInputR38.owner.control=tx.control51;
        headingInputR38.owner.station=tx.frame51.workstationInstanceH51;
        headingInputR38.owner.mode=tx.frame51.workstationModeEpochH51;
        headingInputR38.owner.lifetime=tx.frame51.workstationLifetimeH51;
        headingInputR38.owner.kind=tx.frame51.workstationKindH51;
        headingInputR38.trackingBasis=PQ(frame);headingInputR38.sample=ts.sequence;
        headingInputR38.tick=ts.tick;headingInputR38.timeNs=ts.timeNs;headingInputR38.now=GetTickCount64();
        headingInputR38.headYaw=headYaw128;headingInputR38.active=follow128&&tx.frame51.valid&&!ts.head_held;
        const auto headingR38=kcd2vr::body_follow_r38::advance(tx.bodyFollowR38,headingInputR38);
        const double bodyYawR38=headingR38.active?headingR38.body:0.;
        const double headRelativeR38=headingR38.active?
            kcd2vr::body_follow_r38::wrapped(headingR38.head-bodyYawR38):double(headYaw128);
        // V169R2: current tracked hand geometry, not old trigger/fraction, owns
        // bounded archer participation. A stationary Trigger press adds no yaw.
        float archerYaw127=0.f;
        // CL-C (Claude, 23 Oct 2026): the V66.127/128 ARCHER STANCE is available as an optional trial. Miki: 'VRIK was better before the
        // correction of the bow body position (we had a working left chest rotation)' = V66.169R1, the last build with it.
        // 169R2 replaced it with a 40 % / 30 deg 'shoulder participation' that only grows with the string pull, so in play the
        // chest hardly turns. Here, like V127/128: while a bow is in hand (held, or the string trigger pressed) the chest turns so
        // the LEFT SHOULDER points along the aim line (draw hand -> bow hand), bounded to 75 deg, scaled by the draw
        // (fraction x 1.5, +.35 on the trigger). INI VrArcherStance: 1 = trial, 0 = the 169R2 rule.
        const bool legacyArcherCB1=ArcherStanceCB();
        if(legacyArcherCB1){
            // Use this transaction's qualified bow/reference, not a previous
            // physical frame from a possibly replaced item or calibration.
            if(manualPose169r2&&sameBow169r2&&stringLatched169r2&&valid[0]&&valid[1]&&
               !ts.head_held&&!ts.right_held&&!ts.left_held&&bowProfile169r2.bow()){
                const double draw=std::clamp(archerWeight169r2,0.0,1.0);
                V3 aim=Sub(requestedGoalsRawH50[1],requestedGoalsRawH50[0]);aim.z=0;
                V3 cr=QRotateV(QFromQuatT(s.neutral[rig.spine3]),s.chestRightLocal);cr.z=0;
                if(Len(aim)>.08f&&Len(cr)>.2f){
                    aim=Normalize(aim);cr=Normalize(cr);const V3 want=Mul(aim,-1.f); // left shoulder toward the target
                    const double ang=kcd2vr::body_follow_r38::wrapped(std::atan2(cr.x*want.y-cr.y*want.x,Dot(cr,want))-bodyYawR38);
                    archerYaw127=float(std::clamp(ang,-1.309,1.309)*draw);
                }
            }
        }
        // V66.128 (b) NATURAL CHEST TWIST. Split the head delta into yaw about model up
        // (twist) and the remaining pitch/roll (swing). Pitch/roll keep the established
        // per-segment weights. Yaw: chest = smooth saturation of head yaw (tanh, no hard
        // clamp kink), optionally pulled toward where both hands work (FRIK-style bias,
        // 10 deg dead zone, +-40 deg cap, never while drawing a bow), distributed
        // CUMULATIVELY spine 12% / spine1 35% / spine2 68% / spine3 100%; the neck takes
        // 55% of the remaining head-vs-chest yaw so it never twists back behind the chest.
        const bool natural128=g_chest_natural128.load(std::memory_order_relaxed);
        {
            const float maxChest=float(std::clamp(double(g_chest_max_deg128.load()),10.0,80.0)*d2r);
            const float gain=std::clamp(g_chest_gain128.load(),.1f,1.f);
            chestYaw128=float(maxChest*std::tanh(gain*headRelativeR38/maxChest));
            if(natural128&&g_chest_hand_bias128.load(std::memory_order_relaxed)&&valid[0]&&valid[1]){
                // D86: keep this anatomical base through bow admission. The
                // archer correction below blends from the SAME base; removing
                // it on item acquisition made the shoulders step in one frame.
                // The targets already use the shared tracked frame. Compare
                // against the same chest basis, not the still-animated neutral
                // pelvis before its bodyOffset is applied later in this stage.
                const V3 chestPos=Add(FromT(s.neutral[rig.spine3]),
                    tx.frame51.valid&&(follow128||frameFollow51||stationTorsoJ27)?VP(tx.frame51.bodyOffset):V3{});
                V3 mid=Sub(Mul(Add(requestedGoalsRawH50[0],requestedGoalsRawH50[1]),.5f),chestPos);
                const float midUp=mid.z;mid.z=0;V3 fwd=QRotateV(yawQ128(bodyYawR38),VP(neutralAxes->forward));fwd.z=0;
                const float sideSpan=std::fabs(Dot(Sub(requestedGoalsRawH50[0],requestedGoalsRawH50[1]),Normalize(Cross(fwd,V3{0,0,1}))));
                const double midLength=Len(mid);
                // The old .15 m / .30 m / .05 m binary gates toggled a whole
                // chest correction in one frame. Fade the posture influence;
                // tracked wrist positions are never filtered or retargeted.
                const double participation=pc::smooth_range103(.08,.22,midLength)*
                    (1.-pc::smooth_range103(.20,.35,midUp))*pc::smooth_range103(.03,.08,sideSpan);
                if(midLength>1e-6&&Len(fwd)>.2f&&participation>0){
                    mid=Normalize(mid);fwd=Normalize(fwd);
                    const float handYaw=std::atan2(fwd.x*mid.y-fwd.y*mid.x,Dot(fwd,mid));
                    float diff=float(kcd2vr::body_follow_r38::wrapped(handYaw-chestYaw128));const float dead=float(10*d2r);
                    diff=std::fabs(diff)<dead?0.f:(diff>0?diff-dead:diff+dead);
                    handBias128=float(participation)*std::clamp(g_chest_hand_weight128.load(),0.f,.8f)*std::clamp(diff,float(-40*d2r),float(40*d2r));
                }
            }
        }
        const double baseChest169r2=natural128?chestYaw128+handBias128:0.f;
        // Bow participation remains bounded relative to the hips. Its old
        // absolute +/-75 degree cap must not unwind a whole-body physical turn.
        const double goalYaw169r2=kcd2vr::archer169r2::yaw_goal(
            pc::rotate(kcd2vr::body_follow_r38::yaw(bodyYawR38),neutralAxes->right),
            PV(bowGoals169r2[0]),PV(bowGoals169r2[1]),baseChest169r2);
        if(!legacyArcherCB1)archerYaw127=float(kcd2vr::archer169r2::advance(stagedArcher169r2,manualPose169r2,bowProfile169r2.key,bowProfile169r2.generation,
            tx.epoch,preparedRequest169r2,ts.sequence,ts.tick,ts.timeNs,baseChest169r2,goalYaw169r2,archerWeight169r2,
            !ts.head_held&&!ts.right_held&&!ts.left_held));
        else stagedArcher169r2={}; // CL-C: the 169R2 filter state stays empty while the V127 stance owns the yaw
        const double chestRelativeR38=baseChest169r2+archerYaw127;
        const double chestTotal128=bodyYawR38+chestRelativeR38;
        tx.bowEvidence169r2.chestYaw=chestTotal128;
        // HMD translation beyond the lean allowance moves the visible pelvis.
        // Head-led yaw recruits hips and each native-ground foot through a short
        // step. Fixed leg lengths and native locomotion references are retained.
        // Native capsule movement remains owned by the existing roomscale path.
        if(follow128||frameFollow51||stationTorsoJ27){
            const V3 off=Sub(headTarget,headAnchor123);
            V3 h{off.x,off.y,0.f};const float hl=Len(h);
            const float lean=std::clamp(g_follow_lean128.load(),0.f,.40f),cap=std::clamp(g_follow_max128.load(),0.f,1.2f);
            if(station6DofH41){
                kcd2vr::station_body_h41::Input in{};
                in.owner={tx.frame51.stamp.actor,tx.epoch,ts.source_epoch,tx.control51,tx.frame51.workstationModeEpochH51,
                    workstationOwnerH51.stationLifetimeH46,tx.frame51.workstationInstanceH51,tx.frame51.workstationKindH51};
                in.sample=ts.sequence;in.tick=ts.tick;in.now=GetTickCount64();in.active=true;
                in.x=stationHeadH41.x;in.y=stationHeadH41.y;in.yaw=headYaw128;
                const auto root=kcd2vr::station_body_h41::advance(tx.stationBodyH41,in);
                if(root.active){follow128Offset.x=float(root.x);follow128Offset.y=float(root.y);}
            }else if(follow128&&hl>lean)follow128Offset=Mul(h,(std::min(hl,lean+cap)-lean)/hl);
            const double legMin=std::min(s.leg[0].thigh+s.leg[0].shin,s.leg[1].thigh+s.leg[1].shin);
            const float drop=std::max(0.f,headAnchor123.z-headTarget.z);
            if(tx.frame51.valid){
                // Tracked crouching owns the whole vertical displacement. A
                // fractional pelvis gain leaves a straight fixed-length spine
                // unable to reach the HMD, even though every joint solve passes.
                follow128Offset.z=-std::min(drop,float(legMin*.45));
            }else if(follow128)follow128Offset.z=-std::min(drop*std::clamp(g_follow_crouch128.load(),0.f,1.f),float(legMin*.45));
            if(stationTorsoJ27){
                // J27: real VRIK at the station. hips stay behind/under the head, the spine pivots over the table,
                // only the head drop the lean can't explain is crouch. camera and hands are not touched.
                namespace ws=kcd2vr::station_ws_j27;
                const Q4 restChestQ=QFromQuatT(s.rest[rig.spine3]);
                const auto restAxes=pc::chest_axes103(PV(QRotateV(restChestQ,s.chestRightLocal)),PV(QRotateV(restChestQ,s.chestUpLocal)));
                V3 fwd=QRotateV(yawQ128(bodyYawR38),VP(neutralAxes->forward));fwd.z=0;
                const V3 restHead=FromT(s.rest[rig.head]),restPelvis=FromT(s.rest[rig.pelvis]),restChest=FromT(s.rest[rig.spine3]);
                if(restAxes&&Len(fwd)>.2f){
                    V3 rf=VP(restAxes->forward);rf.z=0;const float rfl=Len(rf);
                    ws::TorsoIn in{};
                    in.fx=fwd.x;in.fy=fwd.y;
                    in.hx=station6DofH41?follow128Offset.x:off.x;in.hy=station6DofH41?follow128Offset.y:off.y;
                    in.drop=drop;in.spine=Len(Sub(restHead,restPelvis));
                    in.restHeadFwd=rfl>.2f?Dot(Sub(restHead,restPelvis),Mul(rf,1.f/rfl)):0.f;
                    in.restChestFwd=rfl>.2f?Dot(Sub(restChest,restPelvis),Mul(rf,1.f/rfl)):0.f;
                    in.chestFraction=in.spine>.2?Len(Sub(restChest,restPelvis))/in.spine:.6;
                    in.eyeFwd=g_eye_forward115.load();in.clear=std::clamp(double(ws::clearCm.load()),0.0,30.0)*.01;
                    in.follow=station6DofH41;
                    const auto t=ws::torso(in);
                    if(t.valid){
                        follow128Offset={float(t.px),float(t.py),-std::min(float(-t.pz),float(legMin*.45))};
                        static std::uintptr_t lastInstanceJ27=0;static std::uint64_t lastLifetimeJ27=0;
                        if(tx.traceH29&&(lastInstanceJ27!=tx.frame51.workstationInstanceH51||lastLifetimeJ27!=tx.frame51.workstationLifetimeH51)){
                            lastInstanceJ27=tx.frame51.workstationInstanceH51;lastLifetimeJ27=tx.frame51.workstationLifetimeH51;
                            char wm[240]{};std::snprintf(wm,sizeof(wm),"WS TORSO station=%s mode=%s pelvisBehindEyeCm=%.1f chestClearCm=%.1f backShiftCm=%.1f spineM=%.3f",
                                tx.frame51.workstationKindH51==1?"ALCHEMY":tx.frame51.workstationKindH51==2?"SMITHING":"OTHER",
                                station6DofH41?"6DOF":"FREE",t.eyeAheadOfPelvis*100.,t.chestClear*100.,t.backShift*100.,in.spine);
                            diagnostic_trace::Write(wm);
                        }
                    }
                }
            }
            const int pel=rig.pelvis;
            const V3 shared51=tx.frame51.valid?VP(tx.frame51.bodyOffset):V3{};
            SetWorldPosition128(s,pel,Add(FromT(s.work[pel]),Add(follow128Offset,shared51)));
            if(follow128)SetWorldRotation6689(s,pel,QMul(yawQ128(bodyYawR38),QFromQuatT(s.work[pel])));
            if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);
            const float trigger=std::clamp(g_step_trigger128.load(),.05f,.5f);
            for(int side=0;side<2;++side){
                const auto& L=s.leg[side];
                const V3 Hn=FromT(s.original[L.hip]),Kn=FromT(s.original[L.knee]),An=FromT(s.original[L.ankle]);
                const auto desired=kcd2vr::body_follow_r38::foot_offset(PV(An),PV(FromT(s.original[pel])),PV(follow128Offset),bodyYawR38);
                const auto foot=kcd2vr::body_follow_r38::step_foot(tx.bodyFollowR38.foot[side],desired,bodyYawR38,
                    ts.sequence,ts.tick,headingR38.active&&headingR38.continuous,tx.bodyFollowR38.foot[1-side].stepping,trigger);
                const V3 axis=Normalize(Sub(An,Hn));V3 pole=Sub(Sub(Kn,Hn),Mul(axis,Dot(Sub(Kn,Hn),axis)));
                if(Len(pole)<.01f)pole=VP(neutralAxes->forward);
                if(follow128)pole=QRotateV(yawQ128(bodyYawR38),pole);
                const Q4 footQ=QMul(yawQ128(foot.currentYaw),QFromQuatT(s.original[L.ankle]));
                float reachErr=0.f;
                // D53: the shared virtual station displacement moves fixed-length
                // feet with the pelvis; it is separate from the HMD step/lean.
                if(!SolveLeg128(s,L,Add(Add(An,VP(foot.current)),VP(tx.frame51.stationOffsetD53)),pole,footQ,reachErr))return RejectCoherent6693(tx,__LINE__);
                follow128Reach[side]=reachErr;
            }
        }
        if(tx.kickFrame176.active){
            const auto& f=tx.kickFrame176;const auto& leg=s.leg[f.side];float reachError=0;
            const auto hipShift=PV(FromT(s.work[leg.hip]))-kickInput176.hip[f.side];
            if(!SolveLeg128(s,leg,VP(f.target+hipShift),VP(f.pole),QFromQuatT(s.original[leg.ankle]),reachError))
                return RejectCoherent6693(tx,__LINE__);
        }
        for(std::uint32_t k=0;k<count;++k)for(int n=0;n<6;++n)if(s.order[k]==ids[n]) {
            Q4 distributed=QP(pc::axis_weighted_delta(PQ(delta),weights[n],limits[n]));
            if((natural128||headingR38.active)&&n!=4){
                static const float yawShare[6]{.35f,.68f,1.f,0.f,0.f,.12f};
                const double relative=natural128?(n==3?chestRelativeR38+.55*(headRelativeR38-chestRelativeR38):chestRelativeR38*yawShare[n]):
                    std::clamp(headRelativeR38*weights[n].z,-limits[n].z,limits[n].z)+(n<3?archerYaw127*yawShare[n]:0.);
                distributed=QMul(yawQ128(bodyYawR38+relative),QP(pc::axis_weighted_delta(PQ(swing128),weights[n],limits[n])));
            }else if(archerYaw127!=0.f&&n<3){
                static const float share[3]{.20f,.50f,1.f};
                distributed=QMul(yawQ128(archerYaw127*share[n]),distributed);
            }
            if(tx.kickFrame176.active&&(n<3||n==5)){
                static constexpr double share176[6]{.35,.68,1,0,0,.12};
                const auto axis=pc::cross(tx.kickFrame176.forward,pc::Vec3{0,0,1});
                const double half=tx.kickFrame176.leanRadians*share176[n]*.5;
                const auto v=axis*std::sin(half);
                distributed=QMul(QP({v.x,v.y,v.z,std::cos(half)}),distributed);
            }
            SetWorldRotation6689(s,ids[n],QMul(distributed,QFromQuatT(s.neutral[ids[n]])));if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);
        }
        if(tx.traceH29&&ts.tick>=s.lastFollowLog128&&ts.tick-s.lastFollowLog128>=2000){s.lastFollowLog128=ts.tick;char fm[800]{};
            std::snprintf(fm,sizeof(fm),"V66.128-BODY follow=%d legs=%d offset=(%.3f,%.3f,%.3f) headYawDeg=%.1f bodyYawDeg=%.1f chestYawDeg=%.1f handBiasDeg=%.1f archerDeg=%.1f naturalTwist=%d stepR/L=%d/%d legReachErrR/L=%.3f/%.3f frame51=%d smooth51=%d rawPelvisZ=%.4f rawAnchorZ=%.4f filteredAnchorZ=%.4f correctionZ=%.4f bodyPelvisZ=%.4f targetHeadZ=%.4f",
                follow128?1:0,s.legsReady128?1:0,follow128Offset.x,follow128Offset.y,follow128Offset.z,headYaw128/d2r,bodyYawR38/d2r,chestTotal128/d2r,handBias128/d2r,archerYaw127/d2r,
                natural128?1:0,tx.bodyFollowR38.foot[0].stepping?1:0,tx.bodyFollowR38.foot[1].stepping?1:0,follow128Reach[0],follow128Reach[1],
                tx.frame51.valid?1:0,tx.frame51.smooth?1:0,double(FromT(s.original[rig.pelvis]).z),tx.frame51.rawAnchor.z,tx.frame51.anchor.z,
                tx.frame51.bodyOffset.z,double(FromT(s.work[rig.pelvis]).z),double(headTarget.z));diagnostic_trace::Write(fm);}
        // V66.112R1: hands never drive the torso/head foundation.  The previous
        // reach-assist rotated spine2 from controller extension, which made Henry's
        // rendered head/body shadow slide when the player moved a hand sideways even
        // though the HMD pose was stationary.  HMD tracking owns the connected trunk;
        // excessive hand reach is handled downstream by clavicle/shoulder/elbow reach
        // retargeting instead of feeding controller motion back into the spine.
        std::array<int,32> spine{};std::size_t n=0;int j=rig.head;
        while(j>=0&&n<spine.size()){spine[n++]=j;if(j==rig.pelvis)break;j=s.parent[j];}
        if(n<2||spine[n-1]!=rig.pelvis)return RejectCoherent6693(tx,__LINE__);std::reverse(spine.begin(),spine.begin()+n);
        std::array<pc::Vec3,32> points{};std::array<double,32> lengths{};
        for(std::size_t k=0;k<n;++k){points[k]=PV(FromT(s.work[spine[k]]));if(k)lengths[k]=Len(FromT(s.local[spine[k]]));}
        // Preserve the actual direct neck->head link, rather than bending it back
        // toward a fixed head point after pitching it. The abdomen/spine reaches
        // the resulting neck base; pelvis, legs and all segment lengths stay native.
        const bool neckLink=n>=4&&spine[n-2]==rig.neck&&s.parent[rig.head]==rig.neck;
        const float neckGainR38=float(.85+.12*pitchShare);
        const Q4 neckTurn=headingR38.active?
            QMul(yawQ128(bodyYawR38+headRelativeR38*neckGainR38),QNlerpIdentity(swing128,neckGainR38)):
            QNlerpIdentity(delta,neckGainR38); // full body yaw plus bounded neck-relative turn; no +/-pi wrap jump
        const V3 neckToHead=Sub(FromT(s.neutral[rig.head]),FromT(s.neutral[rig.neck]));
        const V3 chainTarget=neckLink?Sub(headTarget,QRotateV(neckTurn,neckToHead)):headTarget;
        const auto chainCount=neckLink?n-1:n;
        // The pelvis is not written. Its first child is therefore fixed as well;
        // solve from that actual pivot, not an imaginary movable pelvis->spine link.
        std::array<pc::Vec3,32> ownedPoints{};std::array<double,32> ownedLengths{};
        for(std::size_t k=1;k<chainCount;++k){ownedPoints[k-1]=points[k];if(k>1)ownedLengths[k-1]=lengths[k];}
        if(!pc::solve_spine(ownedPoints,ownedLengths,chainCount-1,PV(chainTarget)))return RejectCoherent6693(tx,__LINE__);
        for(std::size_t k=1;k<chainCount;++k)points[k]=ownedPoints[k-1];
        for(std::size_t k=1;k+1<chainCount;++k) {
            const int id=spine[k],child=spine[k+1];
            const V3 from=Sub(FromT(s.work[child]),FromT(s.work[id]));
            SetWorldRotation6689(s,id,QMul(DeltaRotation6689(from,VP(points[k+1]-points[k])),QFromQuatT(s.work[id])));if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);
        }
        if(neckLink){SetWorldRotation6689(s,rig.neck,QMul(neckTurn,QFromQuatT(s.neutral[rig.neck])));if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);}
        SetWorldRotation6689(s,rig.head,QMul(delta,QFromQuatT(s.neutral[rig.head])));if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);
        const Q4 chestOrientation=QFromQuatT(s.work[rig.spine3]);
        const auto chestAxes=pc::chest_axes103(PV(QRotateV(chestOrientation,s.chestRightLocal)),PV(QRotateV(chestOrientation,s.chestUpLocal)));
        if(!chestAxes)return RejectCoherent6693(tx,__LINE__);
        // Compute BOTH shoulder goals against the identical chest snapshot before
        // either clavicle is changed. No first-arm -> second-arm frame bias.
        Q4 shoulderRotation[2]{};
        for(int side=0;side<2;++side)if(valid[side]){
            const int u=side?rig.upper_l:rig.upper_r,c=side?rig.clav_l:rig.clav_r;
            shoulderRotation[side]=QFromQuatT(s.work[c]);
            if(s.parent[u]!=c)continue;
            const auto turn=pc::shoulder_delta103(stagedShoulder[side],*chestAxes,side,PV(FromT(s.work[u])),PV(manualPose169r2?bowGoals169r2[side]:requestedGoalsRawH50[side]),
                s.upper[side]+s.fore[side],tx.epoch,ts.sequence,ts.tick,ts.timeNs,PV(FromT(s.work[c])));
            if(!turn)return RejectCoherent6693(tx,__LINE__);
            shoulderRotation[side]=QMul(QP(*turn),shoulderRotation[side]);
        }
        for(int side=0;side<2;++side)if(valid[side])SetWorldRotation6689(s,side?rig.clav_l:rig.clav_r,shoulderRotation[side]);
        if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);

    }
