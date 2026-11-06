    // D62: this point follows the ordinary collision/IK solve and its passive
    // evidence. Only the skinning pose below may use future display data.
    // Gameplay packets, native contacts and every temporal semantic history
    // retain the baseline result. OFF does no reads, copies, rays or extra FK.
    if(kcd2vr::hand_present_d61::Enabled()&&!kcd2vr::claude_motion_d69::ExtraActive()){
        namespace hd=kcd2vr::hand_present_d62;
        namespace hp=kcd2vr::hand_present_d61;
        const auto displayNowD62=std::uint64_t(GetTickCount64());
        const auto displayPolicyD62=hp::policy.load(std::memory_order_acquire);
        const bool displayContextD62=HandPresentContextD62()&&!smoothEnabledH50&&
            hd::IdlePhysical(tx.physicalActive,tx.physicalProfiles,tx.physicalState,tx.physicalOutput)&&
            !(collisionMode&&!perfContactModeDs2&&ContactSuspended59c1())&&
            !tx.kickFrame176.active&&!readingVisual32&&!nativeFingerCapture&&
            !tx.reloadWorkingHandXR34&&!tx.acquisitionRequired57&&!s.pendingUse55.serial&&
            ts.allowed&&ts.head_valid&&!ts.head_held&&ts.source_epoch==tracking59c6::epoch.load(std::memory_order_acquire)&&
            ts.tick&&displayNowD62>=ts.tick&&displayNowD62-ts.tick<=80;
        unsigned freeDisplayD62=0;
        for(int side=0;side<2&&displayContextD62;++side){
            const bool owned=heldFingerKey[side]||propHeld5916c1[side]||presenceOwnedH24[side]||
                nativePassengerHand46[side]||doorPose66[side]||doorPoseH40[side]||tx.doorSerial66[side]||
                stagedHandoverD47[side]>.001f||
                displayNowD62<g_native_use_until107[side].load(std::memory_order_acquire)||
                (g_wristOwnedByController59c12&&g_wristOwnedByController59c12(side))||
                HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(s.character),tx.epoch,g_weapon_request_epoch6694.load());
            const bool empty=kcd2vr::frik_pose_probe::ExactHandItemState(side)==0;
            if(hd::Free(displayContextD62,valid[side]&&!(side?ts.left_held:ts.right_held),empty,owned,
                        side?ts.left_grip:ts.right_grip,side?ts.left_trigger:ts.right_trigger))freeDisplayD62|=1u<<side;
        }
        unsigned sampleInterestD62=freeDisplayD62;
        if(collisionMode&&!perfContactModeDs2)for(int side=0;side<2;++side){
            const auto& rawContact=stagedContact[side];
            if(!rawContact.fresh169r1||rawContact.blocked||!rawContact.completeClear49||rawContact.planeCount||
               !g_contact_pass6695||!g_contact_budget_active6695||
               g_contact_budget6695.remaining<(balancedContactD79?7u:preciseContactModeR24?6u:5u))sampleInterestD62&=~(1u<<side);
        }
        const hd::Key keyD62{ts.source_epoch,ts.sequence,ts.tick,ts.timeNs,displayPolicyD62,
            tx.epoch,tx.policyGeneration57,tx.control51,contactControl};
        if(!freeDisplayD62){
            bool first=false;hd::Basis empty{};
            hd::Revoke(hd::Begin(s.handDisplayD62,keyD62,empty,first),0);
            hp::NoteConsumer(ts.source_epoch,ts.tick,0);
        }else{
        // Snapshot the ACTUAL collision-qualified baseline wrists/slots before
        // even reading future data. No unconstrained propRequested substitute.
        tx.handSemanticReadyD62=true;
        const int displayJointsD62[2][3]{{rig.upper_r,rig.fore_r,rig.hand_r},{rig.upper_l,rig.fore_l,rig.hand_l}};
        hd::Basis basisD62{};
        V3 rawPalmD62[2][7]{};
        basisD62.geometryPolicy=(collisionMode?1u:0u)|(perfContactModeDs2?2u:0u)|
            (preciseContactModeR24?4u:0u)|(ph::handSeparationDs1.load(std::memory_order_relaxed)?8u:0u)|
            (g_auto_palm59c8.load(std::memory_order_relaxed)?16u:0u)|(balancedContactD79?32u:0u);
        const auto copyPoseD62=[](const QuatT28& p){return pc::LocalPose{PQ(QFromQuatT(p)),PV(FromT(p))};};
        for(int side=0;side<2;++side){
            tx.handSemanticD62[side]=s.work[displayJointsD62[side][2]];
            for(int j=0;j<3;++j)basisD62.arm[side][j]=copyPoseD62(s.work[displayJointsD62[side][j]]);
            const int slot=side?rig.item_l:rig.item_r;
            if(slot>=0&&slot<int(s.count)&&s.parent[slot]==displayJointsD62[side][2]&&ValidPose6689(s.work[slot])){
                tx.handSemanticSlotD62[side]=s.work[slot];tx.handSemanticSlotValidD62[side]=true;
            }
            // Revalidate the copied shape on EVERY callback, even a cache hit.
            // No rays are repeated. A changed finger/curve/hover pose or contact
            // classification retires the previously checked decision forever.
            bool fingersKnownD62=s.fingerMap[side]&&s.fingerCount[side]>0&&s.fingerCount[side]<=32;
            if(fingersKnownD62)for(int j=0;j<s.fingerCount[side];++j){
                const int id=s.fingerIds[side][j];
                if(id<0||id>=int(s.count)||!ValidPose6689(s.local[id])){fingersKnownD62=false;break;}
                basisD62.fingerId[side][j]=id;
                for(unsigned n=0;n<3;++n)basisD62.finger[side][j].p[n]=s.local[id].t[n];
                for(unsigned n=0;n<4;++n)basisD62.finger[side][j].q[n]=s.local[id].q[n];
            }
            if(fingersKnownD62)basisD62.fingerCount[side]=unsigned(s.fingerCount[side]);
            FingerContactGeometry103 rawGeometryD62{};
            const bool rawGeometryOkD62=fingersKnownD62&&ContactPalmGeometryD79(s,side,
                QFromQuatT(tx.handSemanticD62[side]),side?ts.left_grip:ts.right_grip,
                side?ts.left_trigger:ts.right_trigger,0,rawPalmD62[side],rawGeometryD62,
                preciseContactModeR24,balancedContactD79,
                tx.fingerTrialH7[side]?tx.fingerOpenBiasH7[side]:0.f,tx.fingerTrialH7[side]);
            if(rawGeometryOkD62){
                basisD62.palmCount[side]=balancedContactD79?7u:preciseContactModeR24?6u:5u;
                for(unsigned j=0;j<basisD62.palmCount[side];++j)basisD62.palm[side][j]=PV(rawPalmD62[side][j]);
            }else freeDisplayD62&=~(1u<<side);
            // fresh169r1 is an initial-query requirement only: repeated raw
            // callbacks legitimately retain complete clearance without rays.
            const auto& contactD62=stagedContact[side];
            basisD62.clear[side]=!collisionMode||perfContactModeDs2||
                (!contactD62.blocked&&contactD62.completeClear49&&!contactD62.planeCount);
        }
        basisD62.pelvis=copyPoseD62(s.work[rig.pelvis]);basisD62.chest=copyPoseD62(s.work[rig.spine3]);
        basisD62.head=copyPoseD62(s.work[rig.head]);
        ModelWorld6692 displayWorldD62{};
        const bool displayWorldKnownD62=ReadModelWorld6692(character,displayWorldD62)&&
            std::isfinite(displayWorldD62.scale)&&displayWorldD62.scale>.1f;
        if(displayWorldKnownD62){basisD62.world={PQ(displayWorldD62.q),PV(displayWorldD62.t)};basisD62.scale=displayWorldD62.scale;}
        bool firstDisplayD62=false;
        auto& decisionD62=hd::Begin(s.handDisplayD62,keyD62,basisD62,firstDisplayD62);
        if(firstDisplayD62)decisionD62.interest=displayWorldKnownD62?sampleInterestD62&freeDisplayD62:0;
        hd::Revoke(decisionD62,displayWorldKnownD62?freeDisplayD62:0);
        // Interest shares the initial family decision too. A normal repeated
        // skin callback has no fresh query/pass and must not erase interest
        // for the next XR frame solely because those credits were consumed.
        // Missing/changed geometry and real owners still revoke it immediately.
        hp::NoteConsumer(ts.source_epoch,ts.tick,decisionD62.interest);
        hp::Packet displayPacketD62{};
        if(firstDisplayD62&&freeDisplayD62&&displayWorldKnownD62&&
           hp::Read(ts.source_epoch,ts.sequence,ts.tick,ts.timeNs,ts.poseTime,displayNowD62,displayPacketD62)){
            V3 displayGoalD62[2]{FromT(tx.handSemanticD62[0]),FromT(tx.handSemanticD62[1])};
            Q4 displayRotationD62[2]{QFromQuatT(tx.handSemanticD62[0]),QFromQuatT(tx.handSemanticD62[1])};
            hd::Arm displayArmD62[2]{};unsigned candidatesD62=0;
            V3 displayPalmD62[2][7]{};unsigned featuresD62[2]{};
            double radiusD62[2]{};
            for(int side=0;side<2;++side){
                V3 p=side?ts.left:ts.right;Q4 q=side?ts.lq:ts.rq;
                const bool future=(freeDisplayD62&(1u<<side))&&hp::Select(displayPacketD62,side,true,p,q);
                if(future){
                    const V3 xr=wristBack59c7f!=0.f?Add(p,QRotateV(q,V3{0,0,wristBack59c7f})):p;
                    displayGoalD62[side]=Add(headAnchor123,QRotateV(frame,XrToCryV(Sub(xr,trackedBase))));
                    displayRotationD62[side]=DesiredHand6689(s,side,frame,q);
                    if(g_auto_palm59c8.load(std::memory_order_relaxed)){
                        V3 palm[6]{};FingerContactGeometry103 geometry{};
                        if(!PalmGeometry103(s,side,displayRotationD62[side],0.f,0.f,0,palm,geometry))continue;
                        const V3 back=Mul(palm[0],-.5f);const float cm=Len(back)*100.f;
                        if(!std::isfinite(cm)||cm>12.f)continue;
                        displayGoalD62[side]=Add(displayGoalD62[side],back);
                    }
                    displayArmD62[side]=hd::Solve(basisD62.arm[side],PV(displayGoalD62[side]),PQ(displayRotationD62[side]),
                        s.upper[side],s.fore[side],basisD62.scale);
                }
                // The same existing palm/finger recipe supplies the sparse
                // world rays. A conservative radius only REFUSES optional
                // display near torso/opposite hand; it never moves either hand.
                FingerContactGeometry103 futureGeometry{};
                const bool futureGeometryOk=ContactPalmGeometryD79(s,side,displayRotationD62[side],
                    side?ts.left_grip:ts.right_grip,side?ts.left_trigger:ts.right_trigger,0,
                    displayPalmD62[side],futureGeometry,preciseContactModeR24,balancedContactD79,
                    tx.fingerTrialH7[side]?tx.fingerOpenBiasH7[side]:0.f,tx.fingerTrialH7[side]);
                if(!basisD62.palmCount[side]||!futureGeometryOk)continue;
                featuresD62[side]=basisD62.palmCount[side];
                for(unsigned j=0;j<featuresD62[side];++j)
                    radiusD62[side]=std::max(radiusD62[side],double(std::max(Len(rawPalmD62[side][j]),Len(displayPalmD62[side][j]))));
                radiusD62[side]+=.016; // include bounded finger thickness and skin
                if(future&&displayArmD62[side].valid)candidatesD62|=1u<<side;
            }
            for(int side=0;side<2;++side)if(candidatesD62&(1u<<side)){
                const auto raw=basisD62.arm[side][2].fixed_offset,target=PV(displayGoalD62[side]);
                if(collisionMode){
                    const auto body=kcd2vr::hand_shape59c7f::closest(raw,target,basisD62.pelvis.fixed_offset,basisD62.chest.fixed_offset);
                    const double torso=pc::torso_radius103(PV(gestureShouldersR38[0]),PV(gestureShouldersR38[1]));
                    if(pc::length(body.a-body.b)<=torso+radiusD62[side]){candidatesD62&=~(1u<<side);continue;}
                    if(ph::handSeparationDs1.load(std::memory_order_relaxed)){
                        if(!featuresD62[1-side]){candidatesD62&=~(1u<<side);continue;}
                        // Test relative travel against both the raw opposite
                        // hand and its proposed motion. Either hand may yield.
                        const auto other=basisD62.arm[1-side][2].fixed_offset;
                        const auto otherTarget=(candidatesD62&(1u<<(1-side)))?PV(displayGoalD62[1-side]):other;
                        const auto one=kcd2vr::hand_shape59c7f::closest(raw-other,target-other,{},{ });
                        const auto both=kcd2vr::hand_shape59c7f::closest(raw-other,target-otherTarget,{},{ });
                        if(std::min(pc::length(one.a),pc::length(both.a))<=radiusD62[0]+radiusD62[1]){
                            candidatesD62=0;break;
                        }
                    }
                }
                if(collisionMode&&!perfContactModeDs2){
                    // Mandatory raw queries already ran for BOTH hands. No
                    // separate budget, retry, clearance grace or native hit.
                    const auto& baseline=stagedContact[side];
                    if(!baseline.fresh169r1||baseline.blocked||!baseline.completeClear49||baseline.planeCount||
                       !g_contact_pass6695||!g_contact_budget_active6695||
                       g_contact_budget6695.remaining<featuresD62[side]){candidatesD62&=~(1u<<side);continue;}
                    bool clear=true;
                    kcd2vr::touch_react_service60::ScopedNpcQueryH9 displayQueryD62(g_npc_query172,diagnostics::lab::NativeReference());
                    for(unsigned j=0;j<featuresD62[side];++j){
                        const auto start=ModelToWorld6692(displayWorldD62,Add(VP(raw),rawPalmD62[side][j]));
                        const auto finish=ModelToWorld6692(displayWorldD62,Add(displayGoalD62[side],displayPalmD62[side][j]));
                        const auto motion=Sub(finish,start);
                        if(!std::isfinite(Len(motion))){clear=false;break;}
                        if(Len(motion)<.0001f)continue;
                        if(WorldRayDistance6692(start,motion).state!=wc::Result::Clear){clear=false;break;}
                    }
                    if(!clear){candidatesD62&=~(1u<<side);continue;}
                }
                for(int j=0;j<3;++j)decisionD62.rotation[side][j]=displayArmD62[side].rotation[j];
                decisionD62.target[side]=displayArmD62[side].hand;decisionD62.mask|=1u<<side;
            }
            // Pair rejection can happen after the other side passed its rays.
            decisionD62.mask&=candidatesD62;
            decisionD62.displayTime=displayPacketD62.presentHandTime;
        }
        if(!displayWorldKnownD62||!hp::CurrentPolicy(displayPolicyD62)||
           (collisionMode&&!perfContactModeDs2&&ContactSuspended59c1()))decisionD62.mask=0;
        for(int side=0;side<2;++side)if(decisionD62.mask&(1u<<side)){
            QuatT28 baselineLocalD62[3]{};
            for(int j=0;j<3;++j)baselineLocalD62[j]=s.local[displayJointsD62[side][j]];
            bool appliedD62=true;
            for(int j=0;j<3;++j){
                SetWorldRotation6689(s,displayJointsD62[side][j],QP(decisionD62.rotation[side][j]));
                if(!Compose6689(s)){appliedD62=false;break;}
            }
            if(appliedD62&&s.deferPassengers137)appliedD62=ComposeFull137(s);
            if(appliedD62){
                const auto u=FromT(s.work[displayJointsD62[side][0]]),f=FromT(s.work[displayJointsD62[side][1]]),h=FromT(s.work[displayJointsD62[side][2]]);
                appliedD62=ValidPose6689(s.work[displayJointsD62[side][2]])&&
                    Len(Sub(h,VP(decisionD62.target[side])))<=.002f&&
                    std::fabs(Len(Sub(f,u))-s.upper[side])<=.002&&std::fabs(Len(Sub(h,f))-s.fore[side])<=.002;
            }
            if(!appliedD62){
                decisionD62.mask&=~(1u<<side);
                for(int j=0;j<3;++j)s.local[displayJointsD62[side][j]]=baselineLocalD62[j];
                s.dirtyAll=true;
                if(!ComposeFull137(s))return RejectCoherent6693(tx,__LINE__);
                continue;
            }
            tx.handPresentOnlyD61[side]=true;tx.handPresentPolicyD61=displayPolicyD62;
            tx.handPresentTimeD62=decisionD62.displayTime;
        }
        } // at least one current free hand
    }
