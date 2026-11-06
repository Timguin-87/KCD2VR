    tx.headTarget122=headTarget;tx.fullHead122=full;
    tx.stage="ARMS_REACH_AND_FINGERS";
    // R38: controller coordinates use the calibrated tracking frame even when
    // the shared full-body frame is unavailable. A shoulder-relative span map
    // feeds torso/elbow reconstruction back into a stationary wrist.
    const bool bowOwnsReachH17=prepared169r2&&(tx.physicalProfiles[0].bow()||tx.physicalProfiles[1].bow());
    double reachAsk38[2]{};
    // Gesture zones use the torso-derived shoulders, before per-arm reach
    // accommodation. Moving an arm root must never drag its holster along.
    V3 gestureShouldersR38[2]{FromT(s.work[rig.upper_r]),FromT(s.work[rig.upper_l])};
    auto accommodateShoulderR38=[&](int side,V3& endpoint,bool elbowLab,bool physicalAdmission=false,bool experimentD45=false,const V3* collisionRawD74=nullptr)->bool {
        const int u=side?rig.upper_l:rig.upper_r,h=side?rig.hand_l:rig.hand_r;
        const auto natural=PV(gestureShouldersR38[side]);
        const auto& history=stagedElbow[side];
        const auto fallback=pc::elbow_history_current(history,tx.epoch,ts.sequence,ts.tick)?
            history.axis:PV(Sub(FromT(s.work[h]),gestureShouldersR38[side]));
        const auto previousFitD74=stagedShoulderFitD45[side];
        const auto fitEndpointD74=[&](V3 endpoint){
        double wanted=-1,reserve=-1;
        if(elbowLab){const auto c=kcd2vr::frik_elbow_lab::clamp_triangle(pc::length(PV(endpoint)-natural),s.upper[side],s.fore[side]);
            wanted=c.distance;reserve=c.reserve;}
        else if(physicalAdmission){
            // T4 keeps a strict 2 mm achieved-wrist admission margin. Give its
            // exact tracked request that shoulder room; do not relax its gate.
            reserve=s.upper[side]+s.fore[side]-ph::reach_max(s.upper[side],s.fore[side])+.002;
        }
        return experimentD45?
            kcd2vr::elbow_shoulder_d45::shoulder(stagedShoulderFitD45[side],natural,PV(endpoint),
                PQ(QFromQuatT(s.work[rig.spine3])),s.upper[side],s.fore[side],fallback,
                tx.epoch,elbowPolicyD45,ts.sequence,ts.tick,ts.timeNs):
            pc::controller_shoulder103(natural,PV(endpoint),s.upper[side],s.fore[side],fallback,wanted,reserve);
        };
        auto fit=fitEndpointD74(endpoint);
        if(!fit)return false;
        if(collisionRawD74&&!kcd2vr::contact_admission_d74::shoulderAllowed(natural,PV(*collisionRawD74),
                fit->shoulder,s.upper[side],s.fore[side],fallback)){
            // This FREE-HAND world endpoint demanded remote body motion. Retire
            // its collision authority and solve the exact current controller
            // request. Never clamp every wrist, stretch a bone, or reject the
            // whole VRIK transaction into an authored/native hand pose.
            endpoint=*collisionRawD74;stagedShoulderFitD45[side]=previousFitD74;
            ResetHandGeometryD56(stagedContact[side]);
            s.allowedReady[side]=false;s.handBlocked[side]=s.armBlocked[side]=s.armSaturated[side]=false;
            fit=fitEndpointD74(endpoint);if(!fit)return false;
        }
        // D39: observe large anatomical travel; never feed a shoulder cap back into the wrist.
        const V3 next=VP(fit->shoulder);
        if(tx.traceH29)(void)kcd2vr::shoulder_reach_d38::observe(natural,fit->shoulder,
            kcd2vr::shoulder_reach_d38::maxShiftM.load(std::memory_order_relaxed));
        if(Len(Sub(next,FromT(s.work[u])))>1e-7f){
            SetWorldPosition128(s,u,next);if(!Compose6689(s))return false;
        }
        return true;
    };
    V3 modelIntentGoalsH30[2]{goals[0],goals[1]};
    V3 rawGoalsH50[2]{goals[0],goals[1]}; // H50R2c: the same mapping fed by the RAW requested goal / RAW controller
    for(int side=0;side<2;++side)if(valid[side]){
        const int u=side?rig.upper_l:rig.upper_r;
        const V3 shoulder=FromT(s.work[u]);
        const double henryArm=s.upper[side]+s.fore[side];
      for(int passH50=0;passH50<2;++passH50){ // 0 = visual (smoothed when free), 1 = raw for intent/propRequested
        if(passH50==1&&requestedGoalsRawH50[side].x==requestedGoals[side].x&&requestedGoalsRawH50[side].y==requestedGoals[side].y&&
           requestedGoalsRawH50[side].z==requestedGoals[side].z&&
           (side?ts.left:ts.right).x==visPH50[side].x&&(side?ts.left:ts.right).y==visPH50[side].y&&
           (side?ts.left:ts.right).z==visPH50[side].z){rawGoalsH50[side]=goals[side];break;}
        V3& goalH50=passH50?rawGoalsH50[side]:goals[side];
        goalH50=manualPose169r2?bowGoals169r2[side]:(passH50?requestedGoalsRawH50[side]:requestedGoals[side]);
        // R38: calibrated controller intent stays exact through anatomical
        // reach admission. The shoulder moves to fit the hand below. Contact
        // owners may still select an achieved endpoint later in this frame.
        if(passH50==0)reachAsk38[side]=henryArm>.1?Len(Sub(goalH50,shoulder))/henryArm:0.; // C38 receipt: requested reach / Henry arm
        modelIntentGoalsH30[side]=goalH50; // pass 1 (raw) overwrites pass 0 when it runs: H30 intent = raw pre-reach goal
        if(passH50==0)rawGoalsH50[side]=goals[side];
      }
    }
    // The physical kernel must receive a reachable controller target too;
    // otherwise its own reach cap would reintroduce the same wrist pull before
    // the final IK. The final contact-selected endpoint is accommodated again.
    for(int side=0;side<2;++side)if(valid[side]&&!accommodateShoulderR38(side,rawGoalsH50[side],false,prepared169r2))
        return RejectCoherent6693(tx,__LINE__);
    for(int side=0;side<2;++side){
        tx.propRequestedValid5916c1[side]=valid[side]&&!ts.head_held&&!(side?ts.left_held:ts.right_held);
        tx.propRequested5916c1[side]={PQ(DesiredHand6689(s,side,frame,side?ts.lq:ts.rq)),PV(rawGoalsH50[side])}; // H50R2c: raw requested boundary (H40/grab)
    }
