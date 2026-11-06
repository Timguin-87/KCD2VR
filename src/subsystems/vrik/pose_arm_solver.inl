        pc::Vec3 elbowPreference=torsoPreference,elbowFallback=destinationD86.valid?torsoPreference:PV(sideDown);
        // D86 supplies a continuous anatomy destination for tracked/bow
        // arms. Legacy passenger fallback retains its existing conditioner.
        // No destination path may replace the admitted wrist endpoint.
        const bool bowPoleH15=prepared169r2&&(tx.physicalProfiles[0].bow()||tx.physicalProfiles[1].bow());
        if(tx.traceH29){auto& d=tx.traceArmH29[side];const auto& prior=stagedElbow[side];
            d.entered=true;d.bowPole=bowPoleH15;d.requested=PV(requestedGoals[side]);d.goal=PV(goals[side]);
            d.shoulder=PV(shoulder);d.rawPole=PV(sideDown);d.torsoPole=torsoPreference;
            d.priorSample=prior.sample;d.priorTick=prior.tick;d.priorNs=prior.timeNs;d.priorAxis=prior.axis;d.priorBend=prior.bend;d.priorReady=prior.ready;d.priorEpoch=prior.epoch;
            d.historyCurrent=pc::elbow_history_current(prior,tx.epoch,ts.sequence,ts.tick);
            const auto axis=pc::unit(PV(Sub(goals[side],shoulder)));
            d.conditioning=axis?pc::length(pc::perpendicular(PV(sideDown),*axis)):-1.;
            d.reach=Len(Sub(goals[side],shoulder))/(s.upper[side]+s.fore[side]);
            d.handover=stagedHandoverD47[side];d.hover=s.hoverW178r14[side];}
        const auto bodyBackward=pc::cross(PV(QRotateV(chest,s.chestUpLocal)),PV(QRotateV(chest,s.chestRightLocal)))*-1;
        if(tx.traceH29){tx.traceArmH29[side].bodyBackward=bodyBackward;tx.traceArmH29[side].axisInput=PV(Sub(goals[side],shoulder));}
        if(!bowPoleH15&&!destinationD86.valid&&!elbowR89){ // R90: the R89 option builds its own target below
            if(tx.traceH29)tx.traceArmH29[side].conditionAttemptedD44=true;
            const auto conditioned=pc::condition_elbow_preference(stagedElbow[side],PV(Sub(goals[side],shoulder)),
                torsoPreference,PV(sideDown),bodyBackward,tx.epoch,ts.sequence,ts.tick);
            if(tx.traceH29)tx.traceArmH29[side].conditionedOk=bool(conditioned);
            if(!conditioned)return RejectCoherent6693(tx,__LINE__);
            elbowPreference=conditioned->anatomical;elbowFallback=conditioned->fallback;
        }
        if(tx.traceH29){tx.traceArmH29[side].conditioned=elbowPreference;tx.traceArmH29[side].fallback=elbowFallback;}
        // Existing optional pole/flexion response remains; D86's destination
        // is shared. F7 only records and never selects a solver.
        const bool frikLabR8c=torsoSearchLabD45&&!bowPoleH15&&!elbowExperimentD45;
        if(tx.traceH29){tx.traceArmH29[side].frikLab=frikLabR8c;tx.traceArmH29[side].elbowShoulderD45=elbowExperimentD45;}
        // R90 (R89 option): hand-guided swivel for every arm (ordinary, bow, D45/D67 trials alike). body part:
        // W1 side-down + .6 backward, bow arms their D86/archer preference; hand forward/thumb and torso push the same.
        kcd2vr::elbow_r89::Input r89{};
        if(elbowR89){
            r89.shoulder=PV(shoulder);r89.wrist=PV(goals[side]);r89.upper=s.upper[side];r89.fore=s.fore[side];
            r89.body=bowPoleH15?torsoPreference:PV(sideDown)+bodyBackward*.6;
            r89.forward=PV(QRotateV(handDesired,s.handForwardLocalR89[side]));
            r89.thumb=PV(QRotateV(handDesired,s.thumbLocalR89[side]));
            if(crossbowSupportR93&&destinationD86.valid){
                // The support wrist is already fixed by the retained native
                // crossbow grip. Hand roll must not lift the elbow over its stock.
                // Keep the same pole clock/history; only its destination changes.
                r89.body=torsoPreference;r89.forward={};r89.thumb={};
            }
            r89.torsoA=torsoA_D45;r89.torsoB=torsoB_D45;r89.torsoRadius=torsoRadius;
            r89.seedElbow=PV(FromT(s.work[f]));
        }
        if(tx.traceH29){auto& d=tx.traceArmH29[side];
            using Solver=kcd2vr::diagnostics::vrik_h29::ElbowSolverR94;
            d.selectedSolverR94=elbowR89?Solver::HandR89:elbowExperimentD45?Solver::ShoulderD45:frikLabR8c?Solver::FrikLab:Solver::Continuous;
            if(elbowR89)d.inputR89=r89;
        }
        const auto filtered=elbowR89
            ?kcd2vr::elbow_r89::elbow(stagedElbow[side],r89,tx.epoch,ts.sequence,ts.tick,ts.timeNs,tx.traceH29?&tx.traceArmH29[side].traceR89:nullptr)
            :elbowExperimentD45
            ?kcd2vr::elbow_shoulder_d45::elbow(stagedElbow[side],PV(Sub(goals[side],shoulder)),elbowPreference,elbowFallback,
                Len(Sub(goals[side],shoulder))/(s.upper[side]+s.fore[side]),tx.epoch,ts.sequence,ts.tick,ts.timeNs)
            :frikLabR8c
            ?kcd2vr::frik_elbow_lab::continuous_elbow_lab(stagedElbow[side],PV(Sub(goals[side],shoulder)),elbowPreference,elbowFallback,
                Len(Sub(goals[side],shoulder))/(s.upper[side]+s.fore[side]),tx.epoch,ts.sequence,ts.tick,ts.timeNs)
            :pc::continuous_elbow(stagedElbow[side],PV(Sub(goals[side],shoulder)),elbowPreference,elbowFallback,
                Len(Sub(goals[side],shoulder))/(s.upper[side]+s.fore[side]),tx.epoch,ts.sequence,ts.tick,ts.timeNs);
        if(tx.traceH29)tx.traceArmH29[side].continuityOk=bool(filtered);
        if(!filtered)return RejectCoherent6693(tx,__LINE__);
        const V3 pole=VP(*filtered);
        if(tx.traceH29){tx.traceArmH29[side].filteredPole=*filtered;tx.traceArmH29[side].solvePole=PV(pole);
            if(destinationD86.valid)if(const auto c=kcd2vr::elbow_destination_d86::circle(anatomicalD86))
                tx.traceArmH29[side].filteredIntrusionD86=kcd2vr::elbow_destination_d86::intrusion(anatomicalD86,*c,*filtered);}
        // R38: ordinary and laboratory elbows solve the SAME admitted wrist.
        // Any flexion/reach accommodation already moved the shoulder root. Do
        // not feed clamp_triangle's virtual hand into the visible arm solver.
        const double armReserveR38=elbowExperimentD45?kcd2vr::elbow_shoulder_d45::reserve(s.upper[side],s.fore[side]):frikLabR8c?s.upper[side]+s.fore[side]-
            kcd2vr::frik_elbow_lab::reach_at_flexion(s.upper[side],s.fore[side],kcd2vr::frik_elbow_lab::kMinFlexionDeg):-1.;
        auto arm=pc::solve_arm(PV(shoulder),PV(goals[side]),s.upper[side],s.fore[side],PV(pole),PV(pole),armReserveR38);
        if(tx.traceH29)tx.traceArmH29[side].solveOk=bool(arm);
        if(!arm)return RejectCoherent6693(tx,__LINE__);
        s.reachLimited[side]=arm->reach_limited; // shoulder accommodation is not a wrist reach clamp
        // These rays only annotate HAND_CONTACT; they never correct the arm.
        // Use the same finite recorder and 2 s sample as that diagnostic. Normal
        // gameplay spends no native queries on an obstruction it cannot resolve.
        s.armBlocked[side]=s.armSaturated[side]=false;
        const bool armProbeDueR13=tx.traceH29&&rung>=2&&ts.tick>=s.lastInvariantLog&&
            ts.tick-s.lastInvariantLog>=2000&&
            diagnostics::capture168::handCapture(GetTickCount64());
        if(armProbeDueR13&&freeContact164R1[side]&&g_contact_pass6695&&s.allowedReady[side]){
            float a=0,b=0;
            const bool ub=SegmentBlockedWorld6692(character,shoulder,VP(arm->elbow),0.f,a);
            const bool fb=SegmentBlockedWorld6692(character,VP(arm->elbow),VP(arm->hand),0.f,b);
            if(ub||fb)s.armBlocked[side]=s.armSaturated[side]=true;
        }
        if(freeContact164R1[side]){
            if(s.allowedReady[side])s.allowedGoal[side]=VP(arm->hand);
            // This remains staged with the pose and is published only if the
            // full native write/restore transaction accepts it. Unavailable
            // physics does not move the next sweep origin to the animated hand.
            ContactCommit6695(stagedContact[side],character,VP(arm->hand),ts.tick);
        }
        ikAllowed113[side]=VP(arm->hand);
        stagedBend[side]=VP(arm->bend);
        const auto upperRotation=(elbowExperimentD45?kcd2vr::elbow_shoulder_d45::orient_upper:frikLabR8c?kcd2vr::frik_elbow_lab::orient_upper_arm_lab:pc::orient_upper_arm)(PQ(QFromQuatT(s.work[u])),
            PV(Sub(FromT(s.work[f]),shoulder)),PV(QRotateV(QFromQuatT(s.work[u]),s.hingeNormalLocal[side])),
            arm->elbow-PV(shoulder),pc::cross(arm->hand-PV(shoulder),arm->bend));
        if(!upperRotation)return RejectCoherent6693(tx,__LINE__);
        SetWorldRotation6689(s,u,QP(*upperRotation));
        if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);
        const V3 ef=Sub(VP(arm->hand),FromT(s.work[f]));
        const Q4 foreSwingD45=QP(pc::arm_from_to(
            PV(Sub(FromT(s.work[h]),FromT(s.work[f]))),PV(ef),
            pc::cross(arm->hand-PV(shoulder),arm->bend)));
        SetWorldRotation6689(s,f,QMul(foreSwingD45,QFromQuatT(s.work[f])));if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);
        // D52: handDesired was resolved before contact/shoulder admission.
        // Reapplying native guidance here would rotate the displayed tool
        // through the surface after the contact solver accepted its pose.
        const Q4 residual=QMul(handDesired,QConj(QFromQuatT(s.work[h])));
        const auto conditionedTwist=pc::continuous_pronation(stagedPronation[side],PQ(residual),PV(ef),tx.epoch,ts.sequence,ts.tick,ts.timeNs);
        if(!conditionedTwist)return RejectCoherent6693(tx,__LINE__);
        const Q4 twist=QP(*conditionedTwist);
        SetWorldRotation6689(s,f,QMul(twist,QFromQuatT(s.work[f])));if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);
        SetWorldRotation6689(s,h,handDesired);
