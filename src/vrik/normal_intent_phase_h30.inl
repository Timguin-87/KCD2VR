// The existing normal guard/gesture phase has one producer. It runs once for a
// fresh tracked source even when collision/reach can only publish a display pose.
// No contact can be synthesized by the soft path: its Output contact fields are
// empty, and checked collision authority remains attack_authority55c1.
inline void normal_intent_phase_h30(State& state,const Input& in,const Profile& p,
        WeaponState& s,Output& out,int side,double dt,Pose rawIntentWeapon104,
        Pose intentWeapon104,Pose relativeWeapon63,Pose final,bool stablePair63,
        Vec3 supportHandDelta63,Vec3 supportRelativeDelta63,bool softH30=false) {
    const auto commandBeginH30=out.commandCount;
    const Pose modelHandH30=p.melee()&&!p.shield176()?model_intent_hand_h30(in,side):in.hands[side];
    const bool trackedEffortH30=softH30||(model_intent_selected_h30(in,side)&&m::length(modelHandH30.p-in.hands[side].p)>.001);
    if(normal_intent_enabled_h30(in)&&s.sourcePoseH30&&s.intentReady&&s.relativeIntentReady&&
       (m::length(s.sourceGripH30.p-p.grip[side].p)>1e-7||m::angle(s.sourceGripH30.q,p.grip[side].q)>1e-7||
        m::length(s.sourceTipH30-p.tip)>1e-7)){
        Pose oldModelH30=compose(s.sourceModelH30,inverse(p.grip[side]));
        Pose oldTrackingH30=compose(s.sourceTrackingH30,inverse(p.grip[side]));
        const Vec3 leverH30=s.supportGrip169r2.p-p.grip[side].p;
        if(stablePair63&&s.supportGripReady169r2&&m::length(leverH30)>=.035&&
           finite(s.sourceOtherModelH30)&&finite(s.sourceOtherTrackingH30)){
            const double maximumH30=s.supportAxis59c16?3.0:std::clamp(in.supportConstraint130?std::max(in.supportMaxAngle,55.0*3.14159265358979323846/180.0):in.supportMaxAngle,0.0,1.05);
            const auto steerH30=[&](Pose root,Pose wrist,Pose other){
                const auto rawAxis=m::rotate(root.q,leverH30);
                const auto align=m::from_to(rawAxis,kcd2vr::two_hand_d65::target(rawAxis,other.p-wrist.p,s.supportPrimaryBiasD65));
                root.q=*m::unit(m::multiply(limit_toward({},align,maximumH30),root.q));
                root.p=wrist.p-m::rotate(root.q,p.grip[side].p);return root;};
            oldModelH30=steerH30(oldModelH30,s.sourceModelH30,s.sourceOtherModelH30);
            oldTrackingH30=steerH30(oldTrackingH30,s.sourceTrackingH30,s.sourceOtherTrackingH30);
        }
        s.intentTip=point(oldModelH30,p.tip);s.relativeIntentTip=point(oldTrackingH30,p.tip);
        s.intentHand=s.sourceModelH30.p;s.relativeIntentHand=s.sourceTrackingH30.p;
    }
    const double originalDtH30=kcd2vr::motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs);
    const bool sourceCurrentH30=s.intentReady&&s.relativeIntentReady&&originalDtH30>0&&originalDtH30<=.080000001&&
        finite(modelHandH30)&&finite(in.intentHands[side])&&finite(intentWeapon104)&&finite(relativeWeapon63);
    // D63: the observed held item must hand its extraction motion to a new
    // stroke explicitly. Neither a quiet grip before drawing nor the forward
    // pull out of a scabbard is a cut. Only the transported same-item Draw owns
    // this gate; generic acquisitions, contact corrections and reloads do not.
    if(p.primary&&p.side==side&&p.melee()&&!p.shield176()){
        const auto draw=state.drawDecisionD63[side];
        if(draw.blocked){
            if(s.prepared)out.add(CommandKind::Abort,in,p);
            s.prepared=false;s.waitingPrepare44c1=false;s.stroke118={};s.guardStroke36={};s.committedGestureH26={};
            s.stroke118.ready=draw.released;if(draw.released)state.drawMotionD63[side].handoffPending=false;s.impactRecord178r30={};out.impactRecord178r30[side]={};
            s.intentTip=point(intentWeapon104,p.tip);s.intentHand=modelHandH30.p;s.intentReady=true;
            s.relativeIntentTip=point(relativeWeapon63,p.tip);s.relativeIntentHand=in.intentHands[side].p;s.relativeIntentReady=true;
            s.lastTrackingRotationH29=in.intentHands[side].q;
            s.sourceModelH30=modelHandH30;s.sourceTrackingH30=in.intentHands[side];
            s.sourceOtherModelH30=model_intent_hand_h30(in,1-side);s.sourceOtherTrackingH30=in.intentHands[1-side];
            s.sourceGripH30=p.grip[side];s.sourceTipH30=p.tip;s.sourcePoseH30=true;
            s.sourceModelFrameH8=in.modelIntentFrameH8;s.sourceModelCalibrationH8=in.modelIntentCalibrationH8;
            for(auto n=commandBeginH30;n<out.commandCount;++n)out.commands[n].gesture59c3=s.gesture59c3;
            return;
        }
    }
    // Qualified tracked motion has no second maximum-speed veto. Sensitivity
    // remains in the recognizer; source identity and tracking freshness stay here.
    if(in.contactFaultH30||!model_intent_current_h30(in,side)||(normal_intent_enabled_h30(in)&&(!in.attacks||!in.intentTracked[side]||!finite(in.intentHands[side])||
       (s.intentReady&&s.relativeIntentReady&&(originalDtH30<0||originalDtH30>.080000001))))){
        if(s.prepared)out.add(CommandKind::Abort,in,p);
        s.prepared=false;s.stroke118={};s.guardStroke36={};s.committedGestureH26={};
        s.intentReady=s.relativeIntentReady=false;s.waitingPrepare44c1=false;
        s.impactRecord178r30={};s.lastImpactTick=0;s.sourcePoseH30=false;
        for(auto n=commandBeginH30;n<out.commandCount;++n)out.commands[n].gesture59c3=s.gesture59c3;
        return;
    }
    AdvanceCommittedGestureH8(in,p,s,side,intentWeapon104,relativeWeapon63,sourceCurrentH30);
    const auto& uncertainH8=s.committedGestureH26;
    if(normal_hybrid_intent_h31(in)&&uncertainH8.gesture&&uncertainH8.uncertainSampleH8==in.sample&&
       uncertainH8.uncertainFailedH8){
        out.intentQuarantineH8[side]={{p.key,p.generation,in.bodyEpoch,in.epoch,in.sample,in.tick,true},
            uncertainH8.gesture,uncertainH8.tick,uncertainH8.uncertainSinceH8,uncertainH8.uncertainFailedH8,
            originalDtH30,m::length(point(intentWeapon104,p.tip)-s.intentTip)/originalDtH30,
            m::length(point(relativeWeapon63,p.tip)-s.relativeIntentTip)/originalDtH30,
            m::length(modelHandH30.p-s.intentHand)/originalDtH30,
            m::length(in.intentHands[side].p-s.relativeIntentHand)/originalDtH30,uncertainH8.uncertainAfterSampleH8};
        // This unreadable increment advances no recognizer, native command,
        // forward-contact stamp or original attack clock. Seed only the finite
        // source poses so the next readable increment can resume the SAME cut.
        s.intentTip=point(intentWeapon104,p.tip);s.intentHand=modelHandH30.p;
        s.relativeIntentTip=point(relativeWeapon63,p.tip);s.relativeIntentHand=in.intentHands[side].p;
        s.lastTrackingRotationH29=in.intentHands[side].q;
        s.sourceModelH30=modelHandH30;s.sourceTrackingH30=in.intentHands[side];
        s.sourceOtherModelH30=model_intent_hand_h30(in,1-side);s.sourceOtherTrackingH30=in.intentHands[1-side];
        s.sourceGripH30=p.grip[side];s.sourceTipH30=p.tip;s.sourcePoseH30=true;
        s.sourceModelFrameH8=in.modelIntentFrameH8;s.sourceModelCalibrationH8=in.modelIntentCalibrationH8;
        // Final publication scope withholds semantic records/events; actual
        // checked geometry stop, recoil and physical haptic magnitude remain.
        out.impactRecord178r30[side]={};
        return;
    }
    if(s.committedGestureH26.gesture&&(in.tick<s.committedGestureH26.tick||
       (!normal_hybrid_intent_h31(in)&&in.tick-s.committedGestureH26.tick>160)))s.committedGestureH26={};
        const bool wasGuard117=s.guard117;
        // A deliberate stroke begun outside guard must survive crossing the
        // transverse guard region. A held guard still owns its entry posture.
        const bool crossingStroke119=in.coherentMelee119&&!s.guard117&&(s.stroke118.active||
            s.committedGestureH26.gesture);
        // V66.178R49 (19:05 log): the R45 guard box is wide enough that a blade STOPPED in the opponent, or paused right
        // after a cut, re-entered guard at once (entry ceiling 2.5 m/s, no dwell) and the native consumer then cancelled
        // the prepared attack (guard117 -> PhysCancelAttack117): 68 contacts without a release, 27 without an action.
        // A guard may not be ENTERED for guardAfterAttackMs after a commit, nor while the blade rests in a living actor.
        const bool recentAttack49=!s.guard117&&in.coherentMelee119&&s.lastAttack&&in.tick>=s.lastAttack&&in.tick-s.lastAttack<in.guardAfterAttackMs178r49;
        const bool restingInActor49=!s.guard117&&in.coherentMelee119&&out.blocked[side]&&out.contact119[side].native172.entityId&&
            (out.contact119[side].native172.body==kcd2vr::npc_contact172::Body::Living||out.contact119[side].native172.body==kcd2vr::npc_contact172::Body::Articulated);
        // R120 shield block box: a NEW shield block waits while the right-hand weapon prepares or strikes and for
        // VrShieldAfterAttackMsR120 after its attack, so a raised shield does not cancel the next combo attack
        // (native_combat104 cancels a prepared attack when the shield side presses block). A held block stays.
        const bool shieldBoxR120=in.positionGuard176&&side==1&&p.shield176()&&kcd2vr::shield_block_r120::on();
        const auto& primaryR120=state.weapon[0];
        const bool shieldWaitR120=shieldBoxR120&&!s.guard117&&(primaryR120.prepared||primaryR120.stroke118.active||
            (primaryR120.lastAttack&&in.tick>=primaryR120.lastAttack&&in.tick-primaryR120.lastAttack<kcd2vr::shield_block_r120::afterAttackMs()));
        const bool posture30c1=in.naturalCombat117&&!crossingStroke119&&!recentAttack49&&!restingInActor49&&!shieldWaitR120&&natural_guard117(in,p,in.positionGuard176?rawIntentWeapon104:final,s.guard117);
        // Use requested hand-relative motion, not the collision-stopped blade:
        // pressure against a wall must not look like a deliberate quiet guard.
        const Pose guardHand30c1=(in.intentTracked[side]&&finite(in.intentHands[side]))?in.intentHands[side]:modelHandH30;
        const Vec3 guardTip30c1=point(relativeWeapon63,p.tip);
        double guardSpeed30c1=dt>0&&s.relativeIntentReady?
            std::min(m::length(((guardHand30c1.p-s.relativeIntentHand)+(guardTip30c1-s.relativeIntentTip))*.5),
                     m::length(((modelHandH30.p-s.intentHand)+(point(intentWeapon104,p.tip)-s.intentTip))*.5))/dt:
            std::numeric_limits<double>::infinity();
        // R46C1: a fixed-wrist cut moves the tip twice as fast as the blade
        // midpoint. Both model and HMD-relative tip motion must agree before it
        // can leave/re-enter sword guard. Shields retain their centre policy.
        if(in.coherentMelee119&&p.melee()&&!p.shield176()&&dt>0&&s.intentReady&&s.relativeIntentReady){
            const double tipSpeed46c1=std::min(m::length(point(intentWeapon104,p.tip)-s.intentTip),
                m::length(guardTip30c1-s.relativeIntentTip))/dt;
            guardSpeed30c1=std::max(guardSpeed30c1,tipSpeed46c1);
        }
        const bool thrustLeaves49=in.positionGuard176&&p.melee()&&!p.shield176()&&s.intentReady&&s.relativeIntentReady&&
            guard_parry176::thrustExit49(s.guardThrust49,wasGuard117,dt,guardHand30c1.p-s.relativeIntentHand,
                modelHandH30.p-s.intentHand,m::rotate(compose(guardHand30c1,inverse(p.grip[side])).q,p.axis),
                m::rotate(intentWeapon104.q,p.axis),body_axes(in).forward);
        if(thrustLeaves49)s.guardAttackUntil49=in.tick+120;
        // R59.16C10e / R3B, restored in R59.16C10R16 (it was lost after R3B and never ran in any later log; rung-4 analysis: 17 of 43
        // R14B forward offers were cancelled by Henry's OWN block phase): a NEW guard cannot be entered while a preparation/strike
        // is announced; a guard already held is left to the exit speed.
        // R59.16C10R17 (Miki: "if we are blocking - no swings/stab movement against the enemy and proper position"): with
        // VrGuardStrictEntry a NEW guard is not entered (a) for guardFeintMs after a feint, a withdrawn/short/incoherent stroke
        // candidate or a wind-up (the R16 rung-3 log: 40 % of prepares started inside Henry's own block, 38 % of the guards ended
        // just before his own prepare), (b) while the guard hand moves TOWARD the opponent faster than 0.5 m/s in both motion
        // frames. A held guard leaves on forward TIP speed above the exit speed;
        // H5 keeps fast lateral/upward parries inside the defensive owner.
        double guardForward59c17=-1,guardHandForward59c17=0;
        // C19C4R1: permissive entry must not restore the old direction-blind
        // held exit. A lateral/upward parry keeps guard; a deliberate forward
        // stroke still hands ownership to the attack recognizer in both modes.
        if((in.guardStrict59c17||wasGuard117||(in.masterHandoff23&&in.masterWindowSerial23))&&in.positionGuard176&&p.melee()&&!p.shield176()&&dt>0&&s.intentReady&&s.relativeIntentReady){
            const Vec3 fwd=body_axes(in).forward;
            // The second family has the fixed tracking->model calibration
            // removed. Project it in its own axes, exactly as the recognizer
            // below does; model-forward is not tracking-forward after a turn.
            const Vec3 intentForward=m::rotate(m::multiply(guardHand30c1.q,m::conjugate(modelHandH30.q)),fwd);
            guardForward59c17=std::max(0.0,std::min(m::dot(point(intentWeapon104,p.tip)-s.intentTip,fwd),m::dot(guardTip30c1-s.relativeIntentTip,intentForward))/dt);
            guardHandForward59c17=std::min(m::dot(modelHandH30.p-s.intentHand,fwd),m::dot(guardHand30c1.p-s.relativeIntentHand,intentForward))/dt;
        }
        const bool feint59c17=in.guardStrict59c17&&!wasGuard117&&s.guardFeintUntil59c17&&in.tick<s.guardFeintUntil59c17;
        const bool forwardHand59c17=in.guardStrict59c17&&!wasGuard117&&guardHandForward59c17>.5;
        if(posture30c1&&!wasGuard117){if(feint59c17)++s.guardFeints59c17;if(forwardHand59c17)++s.guardForwardVetoes59c17;}
        const bool guardPosture49=posture30c1&&!thrustLeaves49&&(wasGuard117||!s.prepared)&&!feint59c17&&
            (wasGuard117||!s.guardAttackUntil49||in.tick>=s.guardAttackUntil49);
        // R120: the shield box has its own entry speed / dwell / release speed (VrShieldEnterMaxSpeedR120,
        // VrShieldEnterDwellMsR120, VrShieldExitSpeedR120); the weapon keeps VrGuardEnter*/VrGuardExitSpeed.
        const double guardEnterMaxR120=shieldBoxR120?double(kcd2vr::shield_block_r120::get(kcd2vr::shield_block_r120::EnterSpeed)):in.guardEnterMaxSpeed178r29;
        const unsigned guardDwellR120=shieldBoxR120?kcd2vr::shield_block_r120::dwellMs():release_combat_r92::guardDwell(std::max(100u,in.guardEnterDwellMs178r29),p.shield176());
        const double guardExitR120=shieldBoxR120?double(kcd2vr::shield_block_r120::get(kcd2vr::shield_block_r120::ExitSpeed)):in.guardExitSpeed178r45;
        s.guard117=in.positionGuard176?kcd2vr::guard_parry176::enter(s.guardEntry30c1,guardPosture49,wasGuard117,in.tick,
            forwardHand59c17?std::numeric_limits<double>::infinity():guardSpeed30c1,guardEnterMaxR120,guardDwellR120,
            guardExitR120,guardForward59c17):posture30c1;
        out.guardBySide30c1[side]=s.guard117;
        if(side==0){out.guardFeints59c17=s.guardFeints59c17;out.guardForwardVetoes59c17=s.guardForwardVetoes59c17;}
        const bool guardExit117=in.naturalCombat117&&wasGuard117&&!s.guard117;
        if(guardExit117)s.guardExitTick32=in.tick;
        if(side==0)out.guard117=s.guard117;else if(in.positionGuard176&&p.shield176())out.guard117=out.guard117||s.guard117;
        if(!in.attacks||!in.coherentMelee119||!in.forwardOnly19||!p.melee())s.guardStroke36={};
        if(in.attacks&&p.melee()&&dt>0){
            // V66.110 attack intent is common-motion invariant.  Henry-model motion
            // is still measured, but it is ANDed with an HMD-relative controller
            // trajectory. Walking/room translation/head-only motion therefore cannot
            // create a swing while real arm/wrist motion remains visible in both.
            const Vec3 requestedTip=point(intentWeapon104,p.tip);
            const Pose relativeHand=(in.intentTracked[side]&&finite(in.intentHands[side]))?in.intentHands[side]:modelHandH30;
            const Pose relativeWeapon=relativeWeapon63;
            const Vec3 relativeTip=point(relativeWeapon,p.tip);
            const Vec3 dtip=requestedTip-s.intentTip,dh=modelHandH30.p-s.intentHand;
            const Vec3 rtip=relativeTip-s.relativeIntentTip,rh=relativeHand.p-s.relativeIntentHand;
            const Vec3 globalVelocity=dtip*(1/dt),globalHandVelocity=dh*(1/dt);
            const Vec3 relativeVelocity=rtip*(1/dt),relativeHandVelocity=rh*(1/dt);
            const double globalSpeed=m::length(globalVelocity),globalHandSpeed=m::length(globalHandVelocity);
            const double relativeSpeed=m::length(relativeVelocity),relativeHandSpeed=m::length(relativeHandVelocity);
            const double speed=std::min(globalSpeed,relativeSpeed),handSpeed=std::min(globalHandSpeed,relativeHandSpeed);
            const bool attackIntent=(!in.requirePrimaryGrip114||in.grip[side]>.25)&&
                (in.heldMotionAttacks||(!s.gripRearm&&in.grip[side]>.55));
            const bool continuous=s.intentReady&&s.relativeIntentReady&&std::isfinite(globalSpeed)&&std::isfinite(globalHandSpeed)&&std::isfinite(relativeSpeed)&&std::isfinite(relativeHandSpeed); // finite fresh motion, no duplicate upper-speed veto
            const bool quiet=std::max(globalSpeed,relativeSpeed)<.25&&std::max(globalHandSpeed,relativeHandSpeed)<.25;
            const bool coherent=in.naturalCombat117||in.coherentMelee119;
            if(in.hitTraceC54R3&&side==0)out.hitTraceC54R3={true,attackIntent,continuous,coherent,dt,
                globalSpeed,relativeSpeed,globalHandSpeed,relativeHandSpeed};
            if(s.natural118!=coherent){
                if(s.prepared)out.add(CommandKind::Abort,in,p);
                s.prepared=false;s.stroke118={};s.guardStroke36={};s.spent=true;s.travel=s.thrustTravel=0;s.intentSamples=0;
                s.natural118=coherent;
            }
            if(coherent){
                melee118::Input sample{};
                sample.dt=kcd2vr::motion_time::elapsed(s.lastTick,in.tick,s.timeNs,in.timeNs);
                sample.modelTip=dtip;sample.tip=rtip;sample.modelHand=dh;sample.hand=rh;
                sample.supportMotion63=stablePair63;sample.supportModelHand63=supportHandDelta63;sample.supportHand63=supportRelativeDelta63;
                sample.modelAxis=m::rotate(intentWeapon104.q,p.axis);sample.axis=m::rotate(relativeWeapon.q,p.axis);
                const Vec3 outwardBasis=in.intentTracked[side]?relativeHand.p:(modelHandH30.p-body_shoulder_r38(in,side));
                sample.outward=m::dot(relativeHandVelocity,m::unit(outwardBasis).value_or(Vec3{}));
                sample.modelForward=body_axes(in).forward; // model-space wind-up direction
                // The second motion family is tracking-relative. Use the paired wrist orientations to map the
                // body forward into that family; raw tracking axes need not coincide with Henry's model axes.
                sample.forward=in.forwardOnly19?m::rotate(m::multiply(relativeHand.q,m::conjugate(modelHandH30.q)),sample.modelForward):sample.modelForward;
                sample.eligible=attackIntent&&continuous;sample.known=out.contactKnown[side];
                sample.guard=s.guard117||(guardExit117&&!thrustLeaves49);
                // Same bounded postguard interval as onset probation, but both
                // modes retain its origin for cut qualification. Once active,
                // the recognizer keeps this flag until that candidate retires.
                sample.guardedOrigin59=wasGuard117||(s.guardExitTick32&&in.tick>=s.guardExitTick32&&in.tick-s.guardExitTick32<=300);
                // A budget stop constrains presentation, not native attack intent.
                sample.blocked=out.contactKnown[side]&&out.blocked[side];
                // R32C1: do not pretend a stopped blade is still in free flight
                // while waiting for the native opponent to be selected. F4 may
                // admit its prepared gesture at the first fully qualified NPC
                // contact; continued pressure cannot accumulate a free-space hit.
                const bool actorApproach=in.physicalCombat177?actor_approach32c1(out.contact119[side]):
                    actor_contact175(out.contact119[side]);
                // Only the first qualified actor impact can complete an already
                // deliberate pre-contact stroke. Continued pressure, world hits,
                // UNKNOWN and diagnostic ray serials cannot buy attack intent.
                // V66.178R54: for the stroke INTENT a living/articulated actor body with an entity id is an actor contact.
                // The owner/part/plane/fraction/approach proofs stay where they belong - in the hit chain (R47C2).
                {namespace n=kcd2vr::npc_contact172;const auto& c=out.contact119[side];
                 const bool actorBody54=c.native172.entityId&&(c.native172.body==n::Body::Living||c.native172.body==n::Body::Articulated);
                 if(sample.blocked&&(in.motionOwned59c5?(!s.wasBlocked&&actorApproach&&out.contactMotion169r1[side].valid&&out.contactMotion169r1[side].incoming>.5):(actorBody54||(!s.wasBlocked&&actorApproach&&out.contactMotion169r1[side].valid&&out.contactMotion169r1[side].incoming>.5)))){
                    sample.contactEntity175=out.contact119[side].native172.entityId;sample.contactFraction175=out.contact119[side].fraction;}}
                const bool hadCandidate167=s.stroke118.active;
                // R57C1: moving a HELD wind-up across the rear shoulder region is
                // a feint, not a cut. Forward hand OR tip travel resumes the regular
                // recogniser. Both independent motion frames must stay rearward.
                const Vec3 chest57c1=body_center_r38(in)-body_axes(in).up*.18;
                const bool rearFeint57c1=s.cocked57.active&&in.windupHold57&&in.thrustAsSwing55&&
                    body_vector117(in,modelHandH30.p-chest57c1).y<.08&&body_vector117(in,modelHandH30.p-chest57c1).z>-.15&&
                    std::max(m::dot(dh,body_axes(in).forward),m::dot(rh,body_axes(in).forward))<=.20*dt&&
                    std::max(m::dot(dtip,body_axes(in).forward),m::dot(rtip,body_axes(in).forward))<=.50*dt;
                if(rearFeint57c1)melee118::end_candidate(s.stroke118); // retain the already-earned quiet rearm
                // The synchronized native opponent needs the same early, cancellable
                // announcement as DIRECT. Waiting for most of the stroke before
                // pressing attack gave it less time to enter defense. Full release
                // travel/contact and native acceptance remain independent.
                // C32: 33/49 logged HYBRID transient first-sample offers followed
                // guard release by <=300ms. A fast wrist recovery can satisfy
                // onset at 11ms but reverse before becoming a stroke. Only in
                // this bounded postguard interval use the existing normal
                // preparation travel/age evidence, not a new input cooldown.
                // Real cuts/stabs and actor-contact commitment remain eligible
                // immediately through those existing paths. DIRECT keeps its
                // early finite-threat onset, as do HYBRID attacks outside guard.
                const bool guardOnsetProbation32=!in.motionOwned59c5&&s.guardExitTick32&&
                    in.tick>=s.guardExitTick32&&in.tick-s.guardExitTick32<=300;
                const bool onsetPolicy32=in.motionOwned59c5||(in.forwardOnly19&&!normal_intent_enabled_h30(in));
                // An already announced onset may still refine cut->stab under
                // its same gesture. This never grants an onset to a new one.
                bool onsetIntent19c4r2=onsetPolicy32&&(!guardOnsetProbation32||
                    (s.prepared&&s.stroke118.earlyTelegraph59c10));
                melee118::Policy policy23{in.coherentMelee119,in.physicalCombat177||in.coherentMelee119,in.positionGuard176,in.motionOwned59c5,onsetIntent19c4r2&&!s.prepared,in.forwardOnly19};
                // Restore C54 cut admission once the CURRENT spatial guard has
                // released. Its old300ms origin tag must not require a second
                // translated-hand credential for an otherwise deliberate edge
                // sweep. Held-guard shadow/parry qualification stays unchanged.
                policy23.c54PostGuardH27=in.forwardOnly19&&!sample.guard;
                policy23.purposefulCuts52=in.forwardOnly19; // DIRECT uses the same deliberate cut-speed bank as HYBRID
                policy23.pommelForward69=in.forwardOnly19&&in.pommelStrike69; // C71
                const auto tuningH29=melee_tuning_c54::read(p.classId);policy23.cutSensitivityH29=tuningH29.cut*tuningH29.sensitivityH29;policy23.stabSensitivityH29=tuningH29.stab*tuningH29.sensitivityH29;policy23.intentSensitivityH29=tuningH29.intentH29;policy23.travelR8=tuningH29.travelR8;
                thrust_tuning_d67::apply(p.classId,policy23);
                policy23.bladeAxisCosineH41=melee_tuning_c54::bladeAxisH41();
                policy23.heavy60=p.classId==7; // H21: polearm-only stroke travel
                policy23.twoHand60=stablePair63; // H1: usable latched support in BOTH intent families, never retained identity alone
                policy23.deliberateCut42=in.forwardOnly19&&!in.motionOwned59c5; // headset-tested hybrid only; Direct comparison unchanged
                policy23.cutFeint62=in.forwardOnly19&&s.prepared&&!s.waitingPrepare44c1&&!s.cocked57.active&&!s.released44c1;
                // C23 guard->strike handoff (Codex C4R2 assignment item 2). A held guard wipes the recognizer above, so no
                // PREPARE can exist while guarding. ONLY while the live native master-strike window is published for this
                // exact weapon/body/request (and an opponent within the existing reach/70-degree gate), the FIRST forward
                // strike sample ends the guard through the ordinary exit (entry reset; the pump then performs its normal
                // release) and this same sample reaches the SAME recognizer. Qualification is the recognizer's own deliberate
                // cut/thrust start (dry run on a copy) plus forward tip travel in BOTH families of at least
                // VrMasterHandoffForwardSpeed (default = VrGuardExitSpeed) inside the same 70-degree cone the native request gate
                // uses for the opponent. One use per window serial; no extra Prepare, onset, deadline or window latch.
                // C54: a live native counter opportunity must not depend on an
                // automatic block having already pressed (that very press can
                // close the window). A fresh unprepared forward candidate may
                // take the SAME handoff too; no held input/result is fabricated.
                if(side==0&&(sample.guard||(!s.guard117&&!s.prepared))&&in.masterHandoff23&&in.positionGuard176&&in.masterWindowSerial23&&
                   in.masterWindowSerial23!=s.masterHandoffSerial23&&p.melee()&&!p.shield176()&&!rearFeint57c1&&
                   !s.cocked57.active&&!s.prepared&&p.key==in.masterWindowKey23&&p.generation==in.masterWindowGeneration23&&
                   in.bodyEpoch==in.masterWindowBody23&&in.epoch==in.masterWindowRequest23&&guardForward59c17>0&&
                   std::isfinite(in.masterHandoffSpeed23)&&guardForward59c17>=std::max(0.0,in.masterHandoffSpeed23)){
                    constexpr double kOpponentCone23=.342; // cos 70 deg: NativeStrokeTowardOpponent117's existing opponent cone
                    const double tipSpeed23=std::max(m::length(dtip),m::length(rtip))/dt;
                    auto trialState23=s.stroke118;auto trialSample23=sample;trialSample23.guard=false;
                    const auto trial23=melee118::update(trialState23,trialSample23,policy23);
                    using R23=melee118::Reason;const auto r23=trial23.observation.reason;
                    if(std::isfinite(tipSpeed23)&&guardForward59c17>=kOpponentCone23*tipSpeed23&&!trial23.abort&&trialState23.active&&
                       (trialState23.kind==melee118::Kind::Cut||trialState23.kind==melee118::Kind::Thrust)&&
                       (r23==R23::Candidate||r23==R23::Prepared)){
                        auto& h=out.masterHandoff23;h={};h.serial=in.masterWindowSerial23;h.tick=in.tick;h.sample=in.sample;
                        h.forward=guardForward59c17;h.tipSpeed=tipSpeed23;h.threshold=in.masterHandoffSpeed23;
                        h.kind=int(trialState23.kind);h.heldGuard=s.guard117;
                        s.masterHandoffSerial23=in.masterWindowSerial23;s.masterHandoffGesture23=0;
                        s.guard117=false;s.guardEntry30c1={};out.guardBySide30c1[side]=false;out.guard117=false;
                        sample.guard=false;
                    }
                }
                const bool handoff23=out.masterHandoff23.tick==in.tick&&out.masterHandoff23.serial==s.masterHandoffSerial23&&side==0&&!sample.guard;
                // A genuine live-window handoff already passed the exact item,
                // epoch, one-use, two-family direction/speed and recognizer
                // checks above. Preserve its proved immediate onset; merely
                // possessing a window serial cannot bypass guard probation.
                if(handoff23&&onsetPolicy32){onsetIntent19c4r2=true;policy23.telegraphStart59c10=!s.prepared;}
                // C36 ordinary guard->strike: preserve guard during a parry or
                // short recovery, but stop resetting a deliberate slower cut
                // before it can accumulate normal travel. C23 above keeps its
                // own exact native-window first-sample priority.
                melee118::Result guarded36{};bool transferred36=false;
                Vec3 transferredOrigin36{};
                const bool firstActor36=sample.blocked&&!s.wasBlocked&&actor_approach32c1(out.contact119[side])&&sample.contactEntity175&&
                    out.contactMotion169r1[side].valid&&out.contactMotion169r1[side].incoming>.5;
                const bool observeGuard36=in.forwardOnly19&&in.positionGuard176&&
                    wasGuard117&&(s.guard117||(guardExit117&&firstActor36))&&sample.guard&&p.melee()&&!p.shield176()&&
                    !rearFeint57c1&&!s.cocked57.active&&!s.prepared&&sample.eligible&&
                    sample.known&&(!sample.blocked||firstActor36)&&s.intentReady&&s.relativeIntentReady;
                if(observeGuard36){
                    if(s.guardDirect36!=in.motionOwned59c5)s.guardStroke36={};
                    s.guardDirect36=in.motionOwned59c5;
                    if(!s.guardStroke36.ready&&!s.guardStroke36.active){
                        s.guardStroke36=s.stroke118;
                        s.guardStroke36.ready=true;
                    }
                    const bool hadGuardCandidate36=s.guardStroke36.active;
                    const auto oldModelTip36=hadGuardCandidate36?s.guardStroke36.modelTip:Vec3{};
                    const auto oldTip36=hadGuardCandidate36?s.guardStroke36.tip:Vec3{};
                    auto trialSample36=sample;trialSample36.guard=false;
                    auto trialPolicy36=policy23;trialPolicy36.telegraphStart59c10=false;
                    // The same checked first-body-impact qualification as
                    // DIRECT may request HYBRID input too. This changes no
                    // native backend or hit grant; world/resting contacts fail.
                    if(firstActor36)trialPolicy36.motionOwned59c5=true;
                    guarded36=melee118::update(s.guardStroke36,trialSample36,trialPolicy36);
                    if(!hadGuardCandidate36&&(s.guardStroke36.active||guarded36.commit)){
                        const auto chest36=body_center_r38(in)-body_axes(in).up*.18;
                        s.guardOrigin36=body_vector117(in,modelHandH30.p-dh-chest36);
                    }
                    if(guarded36.prepare){
                        const auto mf36=m::unit(sample.modelForward),rf36=m::unit(sample.forward);
                        const auto fraction36=firstActor36?sample.contactFraction175:1.0;
                        const auto md36=m::unit(oldModelTip36+sample.modelTip*fraction36);
                        const auto rd36=m::unit(oldTip36+sample.tip*fraction36);
                        constexpr double cone36=.342; // existing body-forward70deg input cone
                        const bool forward36=mf36&&rf36&&md36&&rd36&&
                            m::dot(*mf36,*md36)>=cone36&&m::dot(*rf36,*rd36)>=cone36;
                        // An actual approached actor body may already be at the end
                        // of a deliberate side cut. Its swept contact replaces
                        // only this free-space forward cone, never the gesture,
                        // hand travel, actor identity or native hit adjudication.
                        // Lateral parries/world/UNKNOWN still cannot leave guard.
                        if((forward36||firstActor36)&&(s.guardStroke36.active||guarded36.commit)&&!guarded36.abort&&
                           (guarded36.kind==melee118::Kind::Cut||guarded36.kind==melee118::Kind::Thrust)){
                            s.stroke118=s.guardStroke36;transferredOrigin36=s.guardOrigin36;transferred36=true;
                            s.guard117=false;s.guardEntry30c1={};s.guardExitTick32=in.tick;
                            out.guardBySide30c1[side]=false;if(side==0)out.guard117=false;
                            sample.guard=false;
                        }
                        // Off-cone parry preparation cannot be banked for a
                        // subsequent forward movement with a different origin.
                        s.guardStroke36={};
                    }
                }else s.guardStroke36={};
                // Transfer the SAME state/result: do not replay this increment
                // or create a second onset/native attempt for shadowed travel.
                const auto stroke=transferred36?guarded36:
                    rearFeint57c1?melee118::Result{}:melee118::update(s.stroke118,sample,policy23);
                if(in.guardStrict59c17&&in.positionGuard176){ // R59.16C10R17: a feint / abandoned candidate / wind-up is not a guard
                    using R59c17=melee118::Reason;const auto r=stroke.observation.reason;
                    const bool ended=hadCandidate167&&(r==R59c17::Withdrawn||r==R59c17::Short||r==R59c17::Incoherent||r==R59c17::Timeout);
                    const bool windup=r==R59c17::Withdrawn||rearFeint57c1||s.cocked57.active;
                    const std::uint64_t hold=ended?in.guardFeintMs59c17:windup?std::min<unsigned>(150,in.guardFeintMs59c17):0;
                    if(hold&&in.tick+hold>s.guardFeintUntil59c17)s.guardFeintUntil59c17=in.tick+hold;
                }
                if(!hadCandidate167&&(s.stroke118.active||((in.motionOwned59c5||transferred36)&&stroke.commit))){
                    if(normal_hybrid_intent_h31(in))s.committedGestureH26={};
                    if(!s.cocked57.active)s.gesture59c3=in.sample;
                    s.released44c1=false;s.waitingPrepare44c1=false;
                    // Capture before the first deliberate increment, not at release
                    // after the blade/hand may already have crossed Henry's midline.
                    const auto chest=body_center_r38(in)-body_axes(in).up*.18;
                    s.origin167=transferred36?transferredOrigin36:body_vector117(in,modelHandH30.p-dh-chest);
                    // R49C1: F4 master qualification always needs the physical origin.
                    // Ordinary stabs remain native lower-zone input unless the old directional experiment is explicit.
                    s.originZone167=(in.positionGuard176||in.directionalStabs167)?thrust_origin167::zone(s.origin167):-1;
                }
                if(handoff23&&s.stroke118.active)s.masterHandoffGesture23=s.gesture59c3; // the gesture this handoff started
                if(side==0){out.melee118=stroke.observation;
                    if(observeGuard36&&!transferred36){out.melee118.guardHandTravel59=guarded36.observation.guardHandTravel59;
                        out.melee118.guardedOrigin59=true;out.melee118.guardCutPass59=guarded36.observation.guardCutPass59;}
                    out.strokeActive119=in.coherentMelee119&&(s.stroke118.active||stroke.commit);
                    out.primaryStrokeGesture19=out.strokeActive119?s.gesture59c3:0;}
                // C39 DIRBLOCK: where the committed blade comes FROM (tip velocity v_grip + w x r_tip, turned by the blade's own
                // angular velocity for 50 ms), body frame. Pose cadence, existing state only; the game thread reads it only for
                // a waiting reciprocal opponent of a VR DIRECT attempt.
                if(side==0&&out.strokeActive119&&stroke.kind==melee118::Kind::Cut&&s.gesture59c3){
                    // Use this completed physical pose; recovery retains its stored checked pose instead of a requested fallback.
                    const Pose& arrivalPoseH30=softH30?s.weapon:final;
                    const double sc39=in.haveWorld?in.worldScale:1.0;const Vec3 r39=point(arrivalPoseH30,p.tip)-point(arrivalPoseH30,p.grip[side].p);
                    const Vec3 v39=body_vector117(in,(s.linearVelocity+m::cross(s.angularVelocity,r39))*sc39),w39=body_vector117(in,s.angularVelocity);
                    const auto a39=kcd2vr::dirblock39::predictCut({v39.x,v39.y,v39.z},{w39.x,w39.y,w39.z});
                    if(a39.valid)out.arrival39={s.gesture59c3,in.tick,a39.zone,a39.angle,a39.nearBoundary};
                }
                // D34 FE VrFeintHybrid (feint_hybrid_fe.h): HYBRID only; DIRECT keeps C39/C62. HYBRID commits in the prepare
                // sample, so after the commit of THIS cut, while the blade still moves, a clear and stable new C39 sector asks the
                // game thread for one native feint (936A28) of the stroke. No zone/state of this gesture changes here.
                if(side==0&&in.forwardOnly19&&!in.motionOwned59c5&&s.released44c1&&!s.cocked57.active&&s.gesture59c3){
                    const auto& cgFE=s.committedGestureH26;
                    const bool cutFE=cgFE.gesture==s.gesture59c3&&cgFE.kind==0&&s.stroke117==CommandKind::Slash&&
                        cgFE.key==p.key&&cgFE.generation==p.generation&&cgFE.epoch==in.bodyEpoch&&cgFE.request==in.epoch;
                    // The C39 sector math on the live blade of THIS pose sample. The recognizer stroke is spent at the commit
                    // (no C39 arrival/mailbox after it), so FE measures it here; the C39 mailbox is not written.
                    kcd2vr::feint_fe::Live liveFE{};
                    if(cutFE&&in.tick>=cgFE.tick&&in.tick-cgFE.tick<=kcd2vr::feint_fe::kWindowMs){
                        const Pose& poseFE=softH30?s.weapon:final;const double scFE=in.haveWorld?in.worldScale:1.0;
                        const Vec3 rFE=point(poseFE,p.tip)-point(poseFE,p.grip[side].p);
                        const Vec3 vFE=body_vector117(in,(s.linearVelocity+m::cross(s.angularVelocity,rFE))*scFE),wFE=body_vector117(in,s.angularVelocity);
                        const auto aFE=kcd2vr::dirblock39::predictCut({vFE.x,vFE.y,vFE.z},{wFE.x,wFE.y,wFE.z});
                        liveFE={aFE.valid,aFE.nearBoundary,s.gesture59c3,in.sample,aFE.zone,aFE.angle};
                    }
                    int zoneFE=-1;
                    if(kcd2vr::feint_fe::consider(s.feintFE,kcd2vr::feint_fe::enabled.load(std::memory_order_relaxed),s.gesture59c3,
                           cutFE,cutFE?cgFE.tick:0,s.zone117,liveFE,in.tick,zoneFE)==kcd2vr::feint_fe::Why::Ask){
                        const auto beforeFE=out.commandCount;
                        out.add(CommandKind::Zone57,in,p,0,{},zoneFE,CommandKind::Slash,-1,{});
                        if(out.commandCount>beforeFE){out.commands[out.commandCount-1].feintFE=true;
                            kcd2vr::feint_fe::asked.fetch_add(1,std::memory_order_relaxed);}
                    }
                }
                if(stroke.abort){
                    if(s.prepared&&!s.released44c1){const auto beforeAbort=out.commandCount;out.add(CommandKind::Abort,in,p);
                        if(out.commandCount>beforeAbort){using R=melee118::Reason;const auto reason=stroke.observation.reason;
                            out.commands[out.commandCount-1].recognizerAbort18=reason==R::Withdrawn||reason==R::Short||reason==R::Incoherent||reason==R::Timeout;}}
                    s.prepared=false;s.waitingPrepare44c1=false;}
                // R44C1: a temporary input-owner/cooldown gate must not discard
                // the rest of a still-live gesture. Reconsider preparation when
                // that gate clears; never replay an ended/aborted old gesture.
                // R59.16C10R16 VR INTENT: "the swings should never cancel" - no rhythm gate; the native machinery queues/chains.
                const bool rhythmBlocked178=in.physicalCombat177&&!normal_intent_enabled_h30(in)&&(in.nativeAttackBusy178||
                    (s.lastStrike178&&in.tick>=s.lastStrike178&&in.tick-s.lastStrike178<in.strikeMinIntervalMs178));
                if(stroke.prepare&&rhythmBlocked178&&!s.prepared){
                    s.waitingPrepare44c1=true;
                    auto& k=out.strike178;k.serial=++s.strikeSerial178;k.tick=in.tick;k.accepted=false;
                    k.reason=in.nativeAttackBusy178?1:2;k.kind=int(stroke.kind);
                }
                // R57C1: only deliberate backward travel followed by a body-relative
                // settled hold can advertise preparation. Resting near a shoulder is idle.
                // HYBRID held prepare uses the ordinary heavy input; the light-stab
                // comparison cannot change input class after RT-down.
                // C28: DIRECT's finite threat/contact prediction starts at Prepare.
                // A rear cue may be held for seconds and is not a forward onset:
                // announcing it here expires the NPC offer before the real stroke.
                // Keep HYBRID's native held preparation; DIRECT instead reaches the
                // existing qualified forward Prepare below, with its original tick.
                // C35: the C34 rear hint reused the real motion attempt and
                // exhausted its immutable contact prediction before forward
                // motion (11/17 joined rear-cue first STRIKE clocks were zero).
                // Preserve the classifier's windup observation, never admit it
                // as DIRECT Prepare. Only the qualified forward stroke owns
                // the finite attempt/clock; HYBRID held preparation is separate.
                if(!in.motionOwned59c5&&in.windupHold57&&in.thrustAsSwing55&&in.physicalCombat177&&side==0&&p.melee()){
                    auto& ck=s.cocked57;
                    const auto chest=body_center_r38(in)-body_axes(in).up*.18;
                    const Vec3 hp=body_vector117(in,modelHandH30.p-chest);
                    const double hs=std::max(globalHandSpeed,relativeHandSpeed);
                    // Explicit manual HEAVY preparation only. Stable two-hand idle
                    // cannot auto-prepare: incoming-counter yield is not proven.
                    // Trigger-up aborts this hold; only a real stroke releases it.
                    if(std::isfinite(in.trigger[0])&&in.trigger[0]<.20)s.holdRearmH26=false;
                    const bool manualH26=std::isfinite(in.trigger[0])&&in.trigger[0]>.55;
                    const int zoneH26=windup57c1::zone(hp);
                    const bool quietH26=hs<.35&&quiet&&!sample.guard&&attackIntent&&continuous&&!rhythmBlocked178;
                    const bool heldStartH26=!s.holdRearmH26&&quietH26&&!s.prepared&&!s.stroke118.active&&
                        manualH26;
                    if(ck.active&&ck.controlledH26&&!manualH26){
                        out.add(CommandKind::Abort,in,p);s.prepared=false;ck={};s.holdRearmH26=true;
                    }else if(heldStartH26){
                        s.gesture59c3=in.sample;s.stroke117=CommandKind::Slash;s.zone117=zoneH26;s.origin167=hp;s.originZone167=-1;
                        const auto countH26=out.commandCount;out.add(CommandKind::Prepare,in,p,0,relativeVelocity,s.zone117,s.stroke117,-1,hp);
                        if(out.commandCount>countH26){auto& c=out.commands[out.commandCount-1];c.held57=c.controlledHoldH26=true;
                            s.prepared=true;s.prepareTick=in.tick;s.released44c1=false;s.waitingPrepare44c1=false;
                            ck.active=ck.controlledH26=true;ck.zone=zoneH26;ck.originH26=hp;
                            ck.since=ck.lastZoneTick=in.tick;s.holdRearmH26=true;}
                    }else if(!ck.active){
                        const bool armed=in.windupHold57&&ck.arm.update({in.tick,dt,hp,body_vector117(in,dh),body_vector117(in,rh),
                            !s.prepared&&!sample.guard&&attackIntent&&continuous&&!rhythmBlocked178});
                        if(armed&&!s.stroke118.active){
                            s.gesture59c3=in.sample;
                            s.stroke117=CommandKind::Slash;s.zone117=windup57c1::zone(hp);s.origin167=hp;s.originZone167=-1;
                            const auto before=out.commandCount;
                            out.add(CommandKind::Prepare,in,p,0,relativeVelocity,s.zone117,s.stroke117,-1,hp);
                            if(out.commandCount>before){
                                out.commands[out.commandCount-1].held57=true;
                                s.prepared=true;s.prepareTick=in.tick;s.released44c1=false;s.waitingPrepare44c1=false;
                                ck.active=true;ck.zone=s.zone117;ck.since=ck.lastZoneTick=in.tick;ck.windup59c10=false;
                            }
                        }
                    }else if(!s.prepared){ck={};}
                    else if(!attackIntent||!continuous||sample.guard||in.tick<ck.since||in.tick-ck.since>(ck.windup59c10?600u:3000u)||
                            (!ck.controlledH26&&!ck.windup59c10&&!s.stroke118.active&&(hp.y>=.08||hp.z<=-.15)&&hs<1.0)){
                        out.add(CommandKind::Abort,in,p);s.prepared=false;ck={};
                    }else if(!s.stroke118.active&&(ck.controlledH26||windup57c1::region(hp))&&hs<.55&&
                             (!ck.controlledH26||(!ck.feintH26&&m::length(hp-ck.originH26)>=.12))&&
                             windup57c1::zone(hp)!=ck.zone&&in.tick-ck.lastZoneTick>=300){
                        ck.zone=windup57c1::zone(hp);s.zone117=ck.zone;s.origin167=hp;ck.lastZoneTick=in.tick;
                        const auto countH26=out.commandCount;out.add(CommandKind::Zone57,in,p,0,relativeVelocity,s.zone117,s.stroke117,-1,hp);
                        if(ck.controlledH26&&out.commandCount>countH26){auto& c=out.commands[out.commandCount-1];
                            c.held57=c.controlledHoldH26=c.cutFeint62=true;ck.feintH26=true;}
                    }
                }else s.cocked57={};
                // V66.178R59 KICK: R3 while armed = the companion pak's zone-3 kick row (attack_heavy at the unused
                // lower-left zone, the retail unarmed kick fragment under that weapon's tags). It is announced through
                // the same prepare / release pair as a tracked stroke, one native frame apart; the enemy reads zone 3
                // and the companion's zone-distance rows let his low guard block it like the retail kick.
                // R59.16C2 KICK: release first, so a pending new request can never starve a prepared kick.
                if(s.kickRelease59&&side==0){
                    if(s.prepared&&in.tick>s.kickRelease59){
                        const auto beforeKick=out.commandCount;
                        out.add(CommandKind::Slash,in,p,1.0,relativeVelocity,3,CommandKind::Slash,-1,{});
                        if(out.commandCount>beforeKick){
                            out.commands[out.commandCount-1].kick59=true;
                            out.commands[out.commandCount-1].kickTicket59c3=s.kickTicket59c3;
                            s.prepared=false;s.released44c1=true;s.lastStrike178=in.tick;s.kickRelease59=0;
                        }
                    }else if(!s.prepared)s.kickRelease59=0; // aborted elsewhere: nothing to release
                }else if(in.kickRequest59&&in.kickRequest59!=s.kickSeen59&&in.kickRequest59!=state.kickSeen59c3&&side==0&&p.melee()&&in.physicalCombat177){
                    // R59.16C2: a raised guard no longer swallows the kick (native KCD2 also kicks out of a
                    // block). A held wind-up, a running stroke or the rhythm gate DEFER the request for up to
                    // kKickPendingMs59c2 instead of silently consuming it; an expired request reports why.
                    const std::uint64_t age=in.tick>=in.kickRequest59?in.tick-in.kickRequest59:0;
                    // A press made while a native body action owned Henry (e.g. a clinch) is not replayed late.
                    const bool wrongOwner=in.kickTicket59c3.id&&
                        (in.kickTicket59c3.owner.key!=p.key||in.kickTicket59c3.owner.generation!=p.generation||
                         in.kickTicket59c3.owner.body!=in.bodyEpoch);
                    // R59.16C4: owner.request is the WEAPON request epoch (g_weapon_request_epoch6694), a different
                    // counter from in.epoch (pose epoch); C3 compared the two and dropped every kick after a VR mode
                    // switch as OWNER_REPLACED. The kernel has no weapon-request epoch; the receiver checks it against
                    // the live frame.request instead.
                    // R59.16C4 (user decision): R3 is independent of the sword. A HELD wind-up is cancelled for the
                    // kick (Abort + kick prepare in the same frame, so the cocked pose cannot re-arm in between);
                    // only a swing in flight or the native attack/rhythm gate defer it, up to the 600 ms deadline.
                    // R59.16C7E-DECAP1 (13 Oct log: 15 of 16 R3 presses expired as STALE_PRESS while the blade rested in guard):
                    // ANY held preparation gives way to the kick, cocked or not - the pose cannot re-arm in the same frame.
                    if(!wrongOwner&&in.tick>=in.kickRequest59&&age<=kKickPendingMs59c2&&s.prepared&&
                       !s.stroke118.active&&!s.released44c1&&!s.kickRelease59){
                        out.add(CommandKind::Abort,in,p);s.prepared=false;s.cocked57={};s.waitingPrepare44c1=false;
                    }
                    const int blocker=(in.tick<in.kickRequest59||age>kKickPendingMs59c2)?5:
                        wrongOwner?6:s.stroke118.active?2:s.prepared?1:rhythmBlocked178?3:0;
                    if(!blocker){
                        s.kickSeen59=state.kickSeen59c3=in.kickRequest59;
                        const auto before=out.commandCount;
                        s.stroke117=CommandKind::Slash;s.zone117=3;s.origin167={};s.originZone167=-1;
                        out.add(CommandKind::Prepare,in,p,0,relativeVelocity,3,CommandKind::Slash,-1,{});
                        if(out.commandCount>before){
                            out.commands[out.commandCount-1].kick59=true;
                            s.kickTicket59c3=in.kickTicket59c3;
                            out.commands[out.commandCount-1].kickTicket59c3=s.kickTicket59c3;
                            s.prepared=true;s.prepareTick=in.tick;s.released44c1=false;s.waitingPrepare44c1=false;s.kickRelease59=in.tick;
                        }else{s.kickDrop59c2=4;s.kickDropTick59c2=in.tick;} // command buffer full
                    }else if(blocker==5||blocker==6||age>=kKickPendingMs59c2){
                        s.kickSeen59=state.kickSeen59c3=in.kickRequest59;s.kickDrop59c2=blocker;s.kickDropTick59c2=in.tick;
                    }
                }
                if(side==0){out.kickDrop59c2=s.kickDrop59c2;out.kickDropTick59c2=s.kickDropTick59c2;}
                // H29: announce the recognizer's qualified coherent preparation
                // before commitment so native defense can see the incoming zone.
                // First-sample telegraphs are still excluded in VR INTENT. An
                // abandoned preparation follows the existing cancellable Abort;
                // only a committed gesture releases the ordinary attack input.
                const bool intentHeld59c16=normal_intent_enabled_h30(in)&&s.cocked57.active&&s.prepared;
                const bool qualifiedPrepareH29=stroke.prepare&&!s.stroke118.earlyTelegraph59c10;
                const bool prepare44=normal_intent_enabled_h30(in)?((intentHeld59c16&&(stroke.prepare||stroke.commit))||(!s.prepared&&(qualifiedPrepareH29||stroke.commit)&&!stroke.abort)):
                    (stroke.prepare||(s.waitingPrepare44c1&&!stroke.abort&&!s.released44c1&&
                    (stroke.commit||(s.stroke118.active&&s.stroke118.prepared))));
                if(prepare44&&s.cocked57.active&&s.prepared){
                    // R57: the swing leaves a HELD preparation: keep the announced zone, take the recogniser's classification.
                    s.waitingPrepare44c1=false;
                    s.stroke117=stroke.kind==melee118::Kind::Thrust?CommandKind::Thrust:
                        stroke.kind==melee118::Kind::Pommel?CommandKind::Pommel:CommandKind::Slash;
                    // D84 blade zone: the attack direction is where the BLADE goes, not where the hand was cocked.
                    // The held preparation announced the cocked-hand zone; when the actual blade stroke starts from
                    // another side, re-announce once through the existing Zone57 call (no press, no new action).
                    // A controlled (heavy RT) hold may refine only once (native cutFeintUsed62), so skip it if spent.
                    if(bladeZoneD84Enabled()&&(s.stroke117==CommandKind::Slash||(s.stroke117==CommandKind::Thrust&&in.thrustAsSwing55))&&
                       (!s.cocked57.controlledH26||!s.cocked57.feintH26)){
                        // D84: a held stab takes its side from where the stab starts (stabStartZoneD84), a cut from the blade.
                        const int bladeD84=s.stroke117==CommandKind::Thrust?stabStartZoneD84(s.origin167,s.zone117):
                            stroke_zone117(CommandKind::Slash,body_vector117(in,stroke.modelDirection));
                        // a controlled (heavy RT) hold may only refine to 0/1/2 natively; 5 stays for normal holds.
                        const bool allowedD84=bladeD84==0||bladeD84==1||bladeD84==2||(bladeD84==5&&!s.cocked57.controlledH26);
                        if(allowedD84&&bladeD84!=s.zone117){
                            const int handD84=s.zone117; // Keep original zone until the native consumer accepts.
                            const auto beforeD84=out.commandCount;
                            out.add(CommandKind::Zone57,in,p,0,relativeVelocity,bladeD84,s.stroke117,
                                (in.positionGuard176||in.directionalStabs167)?s.originZone167:-1,s.origin167);
                            if(out.commandCount>beforeD84){auto& cD84=out.commands[out.commandCount-1];
                                cD84.optionalBladeZoneD84=true;
                                if(s.cocked57.controlledH26){cD84.held57=cD84.controlledHoldH26=cD84.cutFeint62=true;s.cocked57.feintH26=true;}
                                bladeZoneNoteD84(in.tick,handD84,bladeD84);}
                        }
                    }
                }else if(prepare44&&!(rhythmBlocked178&&!s.prepared)){
                    s.waitingPrepare44c1=false;
                    s.stroke117=stroke.kind==melee118::Kind::Thrust?CommandKind::Thrust:
                        stroke.kind==melee118::Kind::Pommel?CommandKind::Pommel:CommandKind::Slash;
                    s.zone117=stroke_zone117(s.stroke117,body_vector117(in,stroke.modelDirection));
                    if(in.directionalStabs167&&s.stroke117==CommandKind::Thrust&&s.originZone167>=0)s.zone117=s.originZone167;
                    // R55: the thrust keeps its classification (timing, diagnostics) but asks for the ordinary attack ZONE of its
                    // origin: above the chest = overhead, else the side it starts from. The native input class follows in native_input117.
                    if(in.thrustAsSwing55&&s.stroke117==CommandKind::Thrust)s.zone117=stabStartZoneD84(s.origin167,s.origin167.z>.20?0:(s.origin167.x<0?1:2)); // D84 stab start // R14: start side = zone (stroke_zone117 convention)
                    const auto beforePrepare23=out.commandCount;
                    out.add(CommandKind::Prepare,in,p,0,relativeVelocity,s.zone117,s.stroke117,
                        (in.positionGuard176||in.directionalStabs167)?s.originZone167:-1,s.origin167);s.prepared=true;s.prepareTick=in.tick;
                    if(out.commandCount>beforePrepare23&&s.masterHandoffGesture23&&s.masterHandoffGesture23==s.gesture59c3){
                        out.commands[out.commandCount-1].masterHandoff23=s.masterHandoffSerial23;s.masterHandoffGesture23=0;} // one tagged Prepare per handoff

                }else if(stroke.refineCut62&&!stroke.abort&&s.prepared&&!s.released44c1&&!s.waitingPrepare44c1&&
                         !s.cocked57.active&&s.stroke117==CommandKind::Slash){
                    const int zone62=stroke_zone117(CommandKind::Slash,body_vector117(in,stroke.modelDirection));
                    if(zone62!=s.zone117&&(zone62==0||zone62==1||zone62==2)){
                        s.zone117=zone62;const auto before62=out.commandCount;
                        out.add(CommandKind::Zone57,in,p,0,relativeVelocity,zone62,CommandKind::Slash,
                            (in.positionGuard176||in.directionalStabs167)?s.originZone167:-1,s.origin167);
                        if(out.commandCount>before62)out.commands[out.commandCount-1].cutFeint62=true;
                    }
                }else if(stroke.refine59c10&&!stroke.abort&&onsetIntent19c4r2&&s.prepared&&!s.released44c1&&!s.waitingPrepare44c1){
                    // A slightly lateral first sample must not lock a real stab
                    // into cut classification. Update the same preparation; do
                    // not reset its gesture, issue a second prepare or spend it.
                    s.stroke117=CommandKind::Thrust;s.zone117=5;
                    if(in.directionalStabs167&&s.originZone167>=0)s.zone117=s.originZone167;
                    if(in.thrustAsSwing55)s.zone117=stabStartZoneD84(s.origin167,s.origin167.z>.20?0:(s.origin167.x<0?1:2)); // D84 stab start // R14: start side = zone
                    out.add(CommandKind::Zone57,in,p,0,relativeVelocity,s.zone117,s.stroke117,
                        (in.positionGuard176||in.directionalStabs167)?s.originZone167:-1,s.origin167);
                }
                // V66.178 STRIKE QUALITY (F4 rung 2): measured from the force-limited weapon
                // body, not from the controller. Tip velocity v = v_grip + w x r_tip; kinetic
                // energy E = 1/2 m |v_com|^2 + 1/2 I_t |w|^2; reference = a full committed
                // stroke of THIS weapon (cut: w_ref 12 rad/s about the grip; thrust: 4.5 m/s
                // linear). scale = sqrt(E/E_ref) ~ speed ratio. Below strikeMinScale178 the
                // stroke is a touch, not an attack: the prepared native attack is aborted.
                const bool early44=!normal_intent_enabled_h30(in)&&in.physicalCombat177&&in.attackAtPrepare178r42&&s.prepared&&
                    !s.released44c1&&!stroke.abort&&melee118::early_release44c1(s.stroke118,stroke.observation)&&
                    (!policy23.deliberateCut42||melee118::deliberate_cut42(s.stroke118,stroke.observation,sample.axis,sample.modelAxis));
                const bool release44=stroke.commit||early44;
                bool commitAllowed178=true;
                if(release44&&in.physicalCombat177&&s.prepared){
                    const double sc=in.haveWorld?in.worldScale:1.0;
                    const auto b=kcd2vr::dynamics177::body(p.classId,p.weight,m::length(p.tip-p.grip[side].p)*sc);
                    const Pose effortPoseH30=trackedEffortH30?intentWeapon104:s.weapon;
                    const Vec3 grip=point(effortPoseH30,p.grip[side].p),tipW=point(effortPoseH30,p.tip);
                    const Vec3 rTip=(tipW-grip)*sc,axisU=m::unit(rTip).value_or(Vec3{0,1,0});
                    // V66.178R5: a blade stopped by the enemy's body has had its velocity zeroed by
                    // contact resolution above; score the approach (last free-flight sample, <=120 ms old).
                    Vec3 vBody=trackedEffortH30?globalHandVelocity:s.linearVelocity;
                    Vec3 wBody=s.angularVelocity;
                    if(trackedEffortH30){
                        const Vec3 trackingOmegaH30=m::rotation_vector(m::multiply(in.intentHands[side].q,m::conjugate(s.lastTrackingRotationH29)))*(1/dt);
                        wBody=m::rotate(m::multiply(modelHandH30.q,m::conjugate(in.intentHands[side].q)),trackingOmegaH30);
                    }
                    {namespace n=kcd2vr::npc_contact172;const auto& c=out.contact119[side];
                     if(out.blocked[side]&&(void(c),true)&& // R54: any obstruction - the energy is the pre-impact approach, for a wall as for a body
                        s.freeTick178r5&&in.tick>=s.freeTick178r5&&in.tick-s.freeTick178r5<=120){vBody=s.freeLinear178r5;wBody=s.freeAngular178r5;}}
                    const Vec3 v=vBody*sc,w=wBody;
                    const Vec3 vTip=v+m::cross(w,rTip),vCom=v+m::cross(w,axisU*b.com);
                    const bool thrust=stroke.kind==melee118::Kind::Thrust,pommel=stroke.kind==melee118::Kind::Pommel;
                    // V66.178R4 (independent review): only technique-USEFUL motion buys energy.
                    // Spin about the blade's own long axis (wrist roll) moves no edge and no point:
                    // w = w_par + w_perp, E_rot = 1/2 I_perp |w_perp|^2 + 1/2 I_par w_par^2, and
                    // the I_par term is excluded from attack energy because it does no work on a target.
                    //  thrust: forward point motion along the blade, 1/2 m max(0, v_com . a)^2
                    //  pommel: backward motion along the blade,     1/2 m max(0,-v_grip . a)^2
                    //  cut:    edge motion across the blade,        1/2 m |v_com_perp|^2 + 1/2 I_perp |w_perp|^2
                    const Vec3 wPar=axisU*m::dot(w,axisU),wPerp=w-wPar;
                    const Vec3 vComPerp=vCom-axisU*m::dot(vCom,axisU);
                    double E=0;
                    if(thrust){const double f=std::max(0.0,m::dot(vCom,axisU));E=.5*b.mass*f*f;}
                    else if(pommel){const double f=std::max(0.0,-m::dot(v,axisU));E=.5*b.mass*f*f;}
                    else E=.5*b.mass*m::dot(vComPerp,vComPerp)+.5*b.transverse*m::dot(wPerp,wPerp);
                    // V66.178R30 (R28 review F05): a deliberate 1.5 m/s stab scored .33 against the 4.5 m/s reference and fell under the
                    // .35 gate. KCD2 stabs are pushes, not whip cuts: the thrust/pommel reference is 3.0 m/s (a 1.5 m/s stab = .5).
                    const double Eref=(thrust||pommel)?.5*b.mass*3.0*3.0:.5*b.gripInertia*12.0*12.0;
                    auto& k=out.strike178;k.serial=++s.strikeSerial178;k.tick=in.tick;k.kind=int(stroke.kind);k.zone=s.zone117;
                    k.tipSpeed=thrust?std::max(0.0,m::dot(vTip,axisU)):pommel?std::max(0.0,-m::dot(v,axisU)):m::length(vTip-axisU*m::dot(vTip,axisU));
                    k.energy=E;k.reference=Eref;
                    k.scale=(Eref>1e-6&&std::isfinite(E))?std::clamp(std::sqrt(std::max(0.0,E)/Eref)*tuningH29.sensitivityH29,0.0,1.25):0.0;
                    // R59.13 (user): a light poke is NOT discarded - it stays a real attack so the NPC can
                    // see its zone and try to block it; its strength is carried by k.scale and ends up in the
                    // native hit ENERGY (small damage). Only motion below strikeMinScale178 (now a floor, not a
                    // veto) is reported as WEAK; the attack still commits.
                    commitAllowed178=true;k.accepted=true;k.reason=k.scale>=in.strikeMinScale178?0:3;
                    s.weakStrike178=k.scale<in.strikeMinScale178;
                    // R59.16C10R14 (Miki: "native strikes are firing by every small movement"): with the gate on, a
                    // WEAK stroke is not an attack - the pending prepare is aborted and the hand must swing again.
                    if(in.strikeMinScaleGate59c14&&s.weakStrike178){commitAllowed178=false;k.accepted=false;}
                    if(commitAllowed178){s.lastStrike178=in.tick;}
                    else{out.add(CommandKind::Abort,in,p);s.prepared=false;}

                }
                if(release44&&!s.released44c1&&commitAllowed178&&(!in.physicalCombat177||s.prepared)){ /* no prepare (rhythm) = no commit */ const auto count22=out.commandCount;out.add(s.stroke117,in,p,0,relativeVelocity,s.zone117,s.stroke117,
                    (in.positionGuard176||in.directionalStabs167)?s.originZone167:-1,s.origin167);
                    if(out.commandCount>count22){out.commands[out.commandCount-1].held57=s.cocked57.active;
                        out.commands[out.commandCount-1].cutFeint62=stroke.cutSegment62;}
                    if(out.commandCount>count22&&out.strike178.accepted&&out.strike178.tick==in.tick){auto& effort=out.commands[out.commandCount-1];
                        const double sc=in.haveWorld?in.worldScale:1.;
                        effort.effortScale19=out.strike178.scale;effort.effortMass19=kcd2vr::dynamics177::body(p.classId,p.weight,m::length(p.tip-p.grip[side].p)*sc).mass;
                        effort.effortSupport19=out.support[side];effort.effortKnown19=!trackedEffortH30;}
                    if(out.commandCount>count22&&out.strike178.accepted&&out.strike178.tick==in.tick)out.commands[out.commandCount-1].timingSpeed178r22c2=std::max(out.strike178.tipSpeed,stroke.observation.peak);
                    else if(out.commandCount>count22&&!in.physicalCombat177)
                        out.commands[out.commandCount-1].timingSpeed178r22c2=stroke.kind==melee118::Kind::Thrust?
                            std::max(0.,std::min(m::dot(globalHandVelocity,m::unit(sample.modelAxis).value_or(Vec3{})),
                                                m::dot(relativeHandVelocity,m::unit(sample.axis).value_or(Vec3{})))):speed;
                    if(out.commandCount>count22){
                        s.committedGestureH26={s.gesture59c3,in.tick,p.key,p.generation,in.bodyEpoch,in.epoch,
                            m::unit(globalVelocity).value_or(Vec3{}),m::unit(relativeVelocity).value_or(Vec3{}),
                            s.stroke117==CommandKind::Thrust?1:0,s.originZone167};
                        s.committedGestureH26.goreNetTravelD56=stroke.observation.progress;
                        s.committedGestureH26.goreFastTravelD56=stroke.observation.fastTravel;
                        if(normal_hybrid_intent_h31(in)){s.committedGestureH26.forwardTickH8=in.tick;s.committedGestureH26.forwardSampleH8=in.sample;}
                        s.prepared=false;s.released44c1=true;s.lastAttack=in.tick;}}
            }else if(s.guard117||guardExit117){
                if(s.prepared)out.add(CommandKind::Abort,in,p);
                s.prepared=false;s.spent=false;s.travel=s.thrustTravel=s.reversalTravel=0;
                s.intentSamples=0;s.quietSince=0;s.lastStrokeKind=CommandKind::None;
            }else if(!attackIntent||!continuous||!out.contactKnown[side]){
                if(s.prepared)out.add(CommandKind::Abort,in,p);
                s.prepared=false;s.travel=s.thrustTravel=0;s.quietSince=0;s.spent=true;s.intentSamples=0;
            }else{
                if(quiet){
                    if(!s.quietSince)s.quietSince=in.tick;
                    if(in.tick-s.quietSince>=80){s.spent=false;s.travel=s.thrustTravel=0;s.intentSamples=0;s.reversalTravel=0;
                        if(s.prepared)out.add(CommandKind::Abort,in,p);
                        s.prepared=false;}
                }else s.quietSince=0;
                if(s.spent&&s.lastStrokeKind==CommandKind::Slash&&s.lastAttack&&in.tick-s.lastAttack>=220&&!s.wasBlocked){
                    const auto direction=m::unit(relativeVelocity);
                    if(direction&&m::dot(*direction,s.lastStrokeDirection)<-.75&&speed>=.55){
                        s.reversalTravel+=std::min(m::length(dtip),m::length(rtip));
                        if(s.reversalTravel>=.055){s.spent=false;s.travel=s.reversalTravel;s.intentSamples=3;s.prepared=false;s.reversalTravel=0;}
                    }else s.reversalTravel=0;
                }
                // Deliberate stroke gate.  The thresholds are intentionally above
                // controller micro-jitter, but ordinary arm cuts/thrusts clear them.
                if(!s.spent&&std::max(speed,handSpeed)>.40&&(!s.wasBlocked||(!out.blocked[side]&&s.prepared))){
                    ++s.intentSamples;s.travel+=std::min(.20,std::min(m::length(dtip),m::length(rtip)));
                    const auto axis=m::unit(m::rotate(relativeWeapon.q,p.axis)).value_or(Vec3{0,1,0});
                    s.thrustTravel+=std::max(0.0,m::dot(rh,axis));
                    const double along=m::dot(relativeHandVelocity,axis);
                    const Vec3 outwardBasis=in.intentTracked[side]?relativeHand.p:(modelHandH30.p-body_shoulder_r38(in,side));
                    const double outward=m::dot(relativeHandVelocity,m::unit(outwardBasis).value_or(Vec3{}));
                    const bool axial=handSpeed>.50&&std::abs(along)>handSpeed*.72&&handSpeed>speed*.60;
                    const auto stroke=axial&&along>0?CommandKind::Thrust:
                        axial&&along<0&&outward>.18?CommandKind::Pommel:CommandKind::Slash;
                    if(!s.prepared&&s.intentSamples>=2&&s.travel>.025&&(!in.naturalCombat117||!axial||outward>.08)){
                        s.zone117=stroke_zone117(stroke,body_vector117(in,handSpeed>speed*.8?globalHandVelocity:globalVelocity));
                        s.stroke117=stroke;
                        out.add(CommandKind::Prepare,in,p,0,relativeVelocity,in.naturalCombat117?s.zone117:-1,in.naturalCombat117?stroke:CommandKind::None);
                        s.prepared=true;s.prepareTick=in.tick;
                    }
                    if(s.prepared&&s.intentSamples>=3&&s.travel>=.060&&
                       (!in.naturalCombat117||s.stroke117!=CommandKind::Thrust||s.thrustTravel>=.060)&&
                       std::max(speed,handSpeed)>=.55&&(!s.lastAttack||in.tick-s.lastAttack>=220)){
                        if(axial&&outward<=.08){out.add(CommandKind::Abort,in,p);s.prepared=false;s.spent=true;s.reversalTravel=0;}
                        else{
                            const bool thrust=axial&&along>0;
                            const bool pommel=axial&&along<0&&outward>.18;
                            const auto kind=in.naturalCombat117?s.stroke117:pommel?CommandKind::Pommel:thrust?CommandKind::Thrust:CommandKind::Slash;
                            const Vec3 chosen=handSpeed>speed*.8?relativeHandVelocity:relativeVelocity;
                            out.add(kind,in,p,0,chosen,in.naturalCombat117?s.zone117:-1,in.naturalCombat117?kind:CommandKind::None);
                            s.spent=true;s.prepared=false;s.lastAttack=in.tick;s.reversalTravel=0;
                            s.lastStrokeKind=kind;s.lastStrokeDirection=m::unit(relativeVelocity).value_or(Vec3{});
                        }
                    }
                }
                if(s.prepared&&in.tick-s.prepareTick>500){out.add(CommandKind::Abort,in,p);s.prepared=false;s.spent=true;}
            }
            s.intentTip=requestedTip;s.intentHand=modelHandH30.p;s.intentReady=true;
            s.relativeIntentTip=relativeTip;s.relativeIntentHand=relativeHand.p;s.relativeIntentReady=true;
        }else{
            s.intentTip=point(intentWeapon104,p.tip);s.intentHand=modelHandH30.p;s.intentReady=true;
            const Pose relativeHand=(in.intentTracked[side]&&finite(in.intentHands[side]))?in.intentHands[side]:modelHandH30;
            const Pose relativeWeapon=relativeWeapon63;
            s.relativeIntentTip=point(relativeWeapon,p.tip);s.relativeIntentHand=relativeHand.p;s.relativeIntentReady=true;
        }

    // Existing recognizer quiet/reversal rearm ends the source; ordinary REARM
    // while spent and a brief speed dip do not. Actual contact age is unchanged.
    if(normal_hybrid_intent_h31(in)&&s.committedGestureH26.gesture&&
       (s.guard117||(!s.stroke118.spent&&!s.stroke118.active)||s.gesture59c3!=s.committedGestureH26.gesture))s.committedGestureH26={};
    s.lastTrackingRotationH29=in.intentHands[side].q;
    s.sourceModelH30=modelHandH30;s.sourceTrackingH30=in.intentHands[side];
    s.sourceOtherModelH30=model_intent_hand_h30(in,1-side);s.sourceOtherTrackingH30=in.intentHands[1-side];
    s.sourceGripH30=p.grip[side];s.sourceTipH30=p.tip;s.sourcePoseH30=true;
    s.sourceModelFrameH8=in.modelIntentFrameH8;s.sourceModelCalibrationH8=in.modelIntentCalibrationH8;
    if(normal_intent_enabled_h30(in)&&in.allowed&&in.attacks&&in.coherentMelee119&&in.tracked[side]&&in.intentTracked[side]&&
       p.key&&p.generation&&p.primary&&p.side==side&&p.melee()&&!p.shield176()&&
       finite(modelHandH30)&&finite(in.intentHands[side])&&sourceCurrentH30&&
       !out.solverOwnedT4[side]&&!out.ownShieldBlocked60[side])
        out.normalIntentH30[side]={p.key,p.generation,in.bodyEpoch,in.epoch,in.sample,in.tick,true};
    for(auto n=commandBeginH30;n<out.commandCount;++n){auto& cmd=out.commands[n];
        if(cmd.kind==CommandKind::Prepare||cmd.kind==CommandKind::Slash||cmd.kind==CommandKind::Thrust||
           cmd.kind==CommandKind::Pommel||cmd.kind==CommandKind::Zone57||cmd.kind==CommandKind::Abort)
            cmd.gesture59c3=cmd.kick59?0:s.gesture59c3;
    }
}
