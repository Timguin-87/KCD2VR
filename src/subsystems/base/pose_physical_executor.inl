    if(physicalMode){
        // C55: rung 4 is direct handling/support with bounded contact; 5 adds compliance.
        // Nocking remains independent of optional shoulder/quiver gestures.
        auto& pi=tx.physicalInput;pi.hitTraceC54R3=diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,ts.tick);pi.dynamics=g_weapon_control_mode.load()>=5;pi.contact=true; // C55: already inside qualified physical ownership, independent of optional free-hand contacts
        pi.attacks=FullWeaponMode120(g_weapon_control_mode.load());pi.bowActions=BowHandlingMode59c9(g_weapon_control_mode.load());
        // Grace is visual continuity only. Do not synthesize intent from a held
        // controller pose or its recovery jump; the kernel rearms after abort.
        pi.attacks=pi.attacks&&!ts.head_held&&!ts.right_held;
        pi.bowActions=pi.bowActions&&!ts.head_held&&!ts.right_held&&!ts.left_held;
        pi.requirePrimaryGrip114=FullWeaponMode120(g_weapon_control_mode.load())&&retail_build112::Ready(WhBase());
        const auto combatRung117=diagnostics::lab::Get(diagnostics::lab::Key::F3);
        pi.naturalCombat117=(combatRung117==2||combatRung117==3||FullWeaponMode120(g_weapon_control_mode.load()))&&retail_build112::Ready(WhBase())&&pi.attacks;
        pi.coherentMelee119=pi.attacks&&!diagnostics::lab::NativeReference()&&retail_build112::Ready(WhBase());
        pi.positionGuard176=pi.coherentMelee119&&CombatExperiment175Enabled();
        pi.guardEnterMaxSpeed178r29=double(g_guard_enter_speed178r29.load(std::memory_order_relaxed));pi.guardEnterDwellMs178r29=g_guard_enter_dwell178r29.load(std::memory_order_relaxed); // V66.178R29
        pi.guardExitSpeed178r45=double(g_guard_exit_speed178r45.load(std::memory_order_relaxed)); // V66.178R45
        {   // C23: copy (never latch) the guard pump's live +7D8 window publication; fresh <= 100 ms, toward the opponent.
            const float hs23=g_master_handoff_speed23.load(std::memory_order_relaxed);
            pi.masterHandoff23=g_master_handoff_enabled23.load(std::memory_order_relaxed);
            pi.masterHandoffSpeed23=hs23<0.f?pi.guardExitSpeed178r45:double(hs23);
            kcd2vr::native_master23::Published ms23{};
            if(pi.masterHandoff23&&pi.positionGuard176&&g_master_published23.read(ms23)&&ms23.available&&ms23.toward&&ms23.serial&&
               ms23.count>0&&ts.tick>=ms23.tick&&ts.tick-ms23.tick<=100){
                pi.masterWindowSerial23=ms23.serial;pi.masterWindowKey23=ms23.key;pi.masterWindowGeneration23=ms23.generation;
                pi.masterWindowBody23=ms23.body;pi.masterWindowRequest23=ms23.request;}
        }
        pi.guardAfterAttackMs178r49=g_guard_after_attack_ms49.load(std::memory_order_relaxed); // V66.178R49
        pi.guardStrict59c17=g_guard_strict59c17.load(std::memory_order_relaxed); // R59.16C10R17
        pi.statEffect59c17=double(g_weapon_stat_effect59c17.load(std::memory_order_relaxed)); // R59.16C10R17
        pi.statSim26=kcd2vr::dynamics177::statSim26.load(std::memory_order_relaxed); // C26c SIMULATE HENRY
        pi.physicalCombat177=pi.coherentMelee119&&PhysicalCombat177Enabled();
        ph::select_forward_motion36(pi,kcd2vr::motion59c4::selected.load(std::memory_order_acquire),CombatSynchronized59c10r9());
        pi.contactStartsAttack178r41=pi.physicalCombat177&&g_contact_starts_attack178r41.load(std::memory_order_relaxed); // V66.178R41
        pi.attackAtPrepare178r42=pi.physicalCombat177&&g_attack_at_prepare178r42.load(std::memory_order_relaxed); // V66.178R42
        // V66.178R15 impale latch (F4 rung 5): pin published by the game thread, fresh <= 150 ms.
        // C32 (Miki): every killing stab impales, in every F4 combat mode. The old F4 rung-3 visual latch stays as it was
        // when VrImpaleHang is off. C40's optional armour-depth display does not require corpse HANG.
        // R93R3: the user-facing corpse impale option owns its proven servo.
        // Combat/F9 labs and the legacy display latch are separate opt-ins.
        pi.impaleHang32=pi.coherentMelee119&&kcd2vr::gore32::impaleHang.load(std::memory_order_relaxed);
        const bool armourDisplay40=pi.coherentMelee119&&CombatExperiment175Enabled()&&kcd2vr::impale40::enabled(); // C40 default OFF; native result/depth still required
        pi.impaleEnabled178r15=pi.impaleHang32||(g_impale_enabled178r15.load(std::memory_order_relaxed)&&((pi.physicalCombat177&&F4Rung178r10()==3)||armourDisplay40));
        pi.impale40=g_impale_enabled178r15.load(std::memory_order_relaxed)&&armourDisplay40;
        if(pi.impaleEnabled178r15)(void)kcd2vr::contact58c1::latest.read(pi.receipt58c1);
        if(pi.impaleEnabled178r15&&pi.impaleHang32)(void)kcd2vr::impale_corpse_r15::latest.read(pi.corpseStabR15); // I10/N2 corpse stab (game thread qualified)
        // R93: the game thread can publish a current pin AFTER this tracking
        // sample was captured. Judge publication age at consumption, not against
        // the older sample clock; the original source still has its 80 ms limit.
        {ImpalePin178r15 pin{};if(pi.impaleEnabled178r15&&g_impale_pin178r15.read(pin)&&pin.valid){
         const auto pinReadNowR93=std::uint64_t(GetTickCount64()); // after read: concurrent publication cannot postdate the check clock
         if(kcd2vr::contact176::fresh(ts.tick,pinReadNowR93,80)&&kcd2vr::contact176::fresh(pin.tick,pinReadNowR93,150)){
            pi.impalePinValid178r15=true;pi.impalePinWorld178r15={pin.x,pin.y,pin.z};pi.impalePinSerial178r19=pin.serial;pi.impalePinMode40=pin.mode40;pi.impalePinDepth40=pin.depth40;}}}
        if(pi.impaleEnabled178r15){const auto rq26=kcd2vr::kick26::requestTick.load(std::memory_order_acquire),rc26=kcd2vr::kick26::reachTick.load(std::memory_order_acquire); // C26 kick frees the blade
            pi.kickHoldImpale26=kcd2vr::kick26::HoldImpale(ts.tick,rq26,rc26);pi.kickReleaseImpale26=kcd2vr::kick26::ReleaseImpale(ts.tick,rc26);}
        {const auto bt178=g_native_busy_tick178.load(std::memory_order_acquire);
         // V66.178: stale or unknown native state counts as BUSY (fail closed: no new native attack).
         pi.nativeStateKnown178=bt178&&ts.tick>=bt178&&ts.tick-bt178<=150&&g_native_state_ok178.load(std::memory_order_acquire)&&
             g_native_state_epoch178.load(std::memory_order_acquire)==tx.epoch; // V66.178R4: failed/foreign reads are unknown
         // V66.178R3: unknown native state no longer blocks (R2 log: the only stroke was rejected
         // as NATIVE_STATE_UNKNOWN); the 550 ms minimum interval still applies.
         pi.nativeAttackBusy178=pi.physicalCombat177&&pi.nativeStateKnown178&&g_strike_block_active178.load(std::memory_order_relaxed)&&
             g_native_busy178.load(std::memory_order_acquire);
         if(kcd2vr::motion59c4::selected.load())pi.nativeAttackBusy178=false; // Preview timing comes from VR; protected-body guards stay intact.
         pi.weaponAuthorityScale178r44=std::clamp(double(g_weapon_authority_scale178r44.load()),0.2,2.0); // V66.178R44
         pi.twoHandWeightFeelR90=kcd2vr::two_hand_weight_r90::enabled.load(std::memory_order_acquire); // R90 option
         pi.strikeMinScale178=std::clamp(double(g_strike_min_scale178.load()),0.0,1.0);
         pi.strikeMinIntervalMs178=std::clamp(g_strike_min_interval178.load(),0u,3000u);
         pi.strikeMinScaleGate59c14=g_strike_min_scale_gate59c14.load(std::memory_order_relaxed);}
        if(kcd2vr::melee_tuning_c54::bypassExtraAttackVetoesH41.load()){
            // Experimental bypass of redundant MOD rhythm/busy checks only.
            // Native action/state eligibility still receives and judges input.
            pi.nativeAttackBusy178=false;pi.strikeMinIntervalMs178=0;
        }
        pi.intent59c16=pi.physicalCombat177&&g_intent_pi59c16.load(std::memory_order_acquire); // legacy R16 native route only
        pi.supportAxis59c16=g_two_hand_axis59c16.load(std::memory_order_relaxed); // R59.16C10R16 two-handed axis grip (every weapon mode)
        pi.fatigueRamp59c14=std::clamp(double(g_fatigue_ramp59c14.load(std::memory_order_relaxed)),0.0,1.0);   // R59.16C10R14
        pi.poleOneHand59c14=std::clamp(double(g_pole_one_hand59c14.load(std::memory_order_relaxed)),0.15,1.0); // R59.16C10R14
        pi.directionalStabs167=pi.coherentMelee119&&CombatExperiment175Enabled()&&g_directional_stabs178r29.load(std::memory_order_relaxed);
        pi.thrustAsSwing55=g_thrust_as_swing55.load(std::memory_order_relaxed); // V66.178R55
        if(kcd2vr::kick5916c3::requests.read(pi.kickTicket59c3))
            pi.kickRequest59=pi.kickTicket59c3.tick; // immutable request owner is checked in kernel AND receiver
        pi.windupHold57=g_windup_hold57.load(std::memory_order_relaxed);
        pi.telegraphWindup59c10=g_telegraph_windup59c10.load(std::memory_order_relaxed); // R59.16C10
        pi.earlyWarning33=pi.telegraphWindup59c10; // C35: compatibility hint only; DIRECT admission starts at qualified forward motion.
        pi.wallSlide58=g_wall_slide59c10.load(std::memory_order_relaxed);
        pi.wallBackoutSpringH31=kcd2vr::deepseek_h29::wallBackoutSpringH31.load(std::memory_order_acquire); // session-only Shift+F12 opt-in
    pi.wallFriction57=kcd2vr::physical::g_wall_friction57.load(std::memory_order_relaxed); // R59.16C10d // V66.178R57 // V66.178R29: default off - a thrust is the stab zone (5), not the origin guard
        pi.controllerAnchor119=true;
        pi.releasePresentation45=true;
        pi.constraintDrive129=g_weapon_constraint_drive134.load(std::memory_order_relaxed); // V66.129 bounded force/torque compliance; replaces perceptual low-pass drag in controller-anchor mode
        pi.supportV120_178r27=g_two_hand_v120_178r27.load(std::memory_order_relaxed); // V66.178R27: V66.120/123 two-hand handling
        {const bool nudge120=g_two_hand_constraint_v120_178r29.load(std::memory_order_relaxed); // V66.178R29: socket (V120) and constraint (V130) are separate choices
         pi.supportConstraint130=g_weapon_two_hand_constraint134.load(std::memory_order_relaxed)&&!nudge120;pi.supportMaxAngle=(nudge120?8.0:55.0)*3.14159265358979323846/180.0;} // V66.130 true secondary rigid-weapon constraint (V120 option: 8 deg nudge, primary hand owns the weapon)
        pi.bowTrigger=true;pi.trigger[0]=ts.right_trigger;pi.trigger[1]=ts.left_trigger;pi.autoSupport=false;pi.arrowFromShoulder=false; /* V66.109: explicit Grip support + RT bow lifecycle; pseudo-quiver retired */pi.bowRetention=true;pi.heldMotionAttacks=true;pi.sweepGate=!g_npc_query172.requested; /* Moving bodies cannot reuse static-world clearance. */pi.timeNs=ts.timeNs;pi.poseTimeEpoch=ts.source_epoch;pi.handPredictionPolicyD69=ts.handPolicyD69;pi.allowed=ts.allowed&&ts.head_valid;pi.epoch=tx.epoch;pi.bodyEpoch=tx.epoch;pi.sample=ts.sequence;pi.tick=ts.tick;
        // Accepted physical Grip draw is a normal option; F11=2 still overrides it.
        // Native automatic load remains separate from string geometry and recording.
        const bool gripDraw596c1=BowGripDrawEnabled596C1();
        ph::select_bow_policy175(pi,tx.physicalProfiles[1],BowHandlingMode59c9(g_weapon_control_mode.load()),
            valid[0],valid[1],!gripDraw596c1&&!g_combat_experiment175.load()&&g_bow_manual_reload146.load(std::memory_order_relaxed), // Grip policy preserves native automatic loading.
            !g_combat_experiment175.load()&&g_bow_native_grab146.load(std::memory_order_relaxed),gripDraw596c1);
        if(ph::physical_bow175(pi)&&tx.physicalProfiles[1].bow()&&!tx.physicalProfiles[1].bowNockQualified40)pi.bowActions=false; // C40: wait for current measured rest-string nock before first draw
        if(StagedBowEnabled59()&&!ManualReadyFor596(tx.physicalProfiles[1].key,tx.epoch,GetTickCount64(),tx.physicalProfiles[1].generation))pi.bowActions=false;
        pi.quiverRadius146=std::clamp(double(g_holster_radius136.load(std::memory_order_relaxed)),.18,.35);
        pi.guardHead=PV(tx.headTarget122);pi.guardHeadKnown=pc::finite(pi.guardHead);
        pi.modelIntentFrameH8=tx.frame51;pi.modelIntentCalibrationH8=PQ(frame);
        pi.torsoShouldersR38[0]=PV(gestureShouldersR38[0]);pi.torsoShouldersR38[1]=PV(gestureShouldersR38[1]);
        pi.torsoShouldersKnownR38=true;
        // R1: subtract common translation in tracking axes. Rotating by the
        // instantaneous HMD orientation invents a swing when only the head turns.
        for(int side=0;side<2;++side){const int u=side?rig.upper_l:rig.upper_r;
            // R32C1: the grip is native wrist-local, not controller-local. Use
            // the same anatomical wrist basis in both motion checks, removing
            // only the fixed tracking->model calibration from intent orientation.
            const Q4 modelHand=DesiredHand6689(s,side,frame,side?ts.lq:ts.rq);
            const Q4 intentHand=QNormalize(QMul(QConj(frame),modelHand));
            const V3 rawP=XrToCryV(Sub(side?ts.left:ts.right,ts.head));
            pi.hands[side]={PQ(modelHand),PV(rawGoalsH50[side])}; // H50: physical targets stay raw even on the first acquired-weapon frame
            pi.modelIntentHandsH30[side]={PQ(modelHand),PV(modelIntentGoalsH30[side])};
            pi.modelIntentSourceH30[side]=true;
            pi.modelIntentTrackedH30[side]=valid[side]&&ts.head_valid&&!ts.head_held&&!(side?ts.left_held:ts.right_held);
            pi.intentHands[side]={PQ(intentHand),PV(rawP)};pi.intentTracked[side]=valid[side]&&ts.head_valid&&!ts.head_held&&!(side?ts.left_held:ts.right_held);
            // A grace-held coordinate is not a new locate at the family's target.
            // H17 also uses the head in its desired basis; retain delivery timing
            // if that component is held. Fresh opposite-hand timing is independent.
            pi.poseTime[side]=(ts.head_held||(side?ts.left_held:ts.right_held))?0:(ts.handTargetD69[side?0:1]?ts.handTargetD69[side?0:1]:ts.poseTime);
            pi.shoulders[side]=PV(FromT(s.work[u]));pi.reach[side]=ph::reach_max(s.upper[side],s.fore[side]);
            pi.grip[side]=side?ts.left_grip:ts.right_grip;pi.tracked[side]=valid[side];}
        PhysInputWorld6698(pi,character);
        if(prepared169r2){
            pi.epoch=preparedRequest169r2;
            ManualNockInput19c4r4(pi,tx.physicalProfiles[1],g_phys_state6698.sample);
            ph::resolve_bow_mode146(pi,tx.physicalProfiles[1],&g_phys_state6698);
            NativeHeldRetirePoseAdmissionT4(tx);
            tx.physicalState=g_phys_state6698;tx.physicalHolsters=g_phys_holsters6698;
            ph::RejectedBowRelease166 rejected166{};
            if(g_bow_release_rejected166.read(rejected166))
                ph::restore_rejected_bow166(tx.physicalState,rejected166.key,rejected166.generation,rejected166.epoch,rejected166.bodyEpoch,rejected166.sample);
            kcd2vr::passive169r1::Sample rayOwner169r1{};rayOwner169r1.epoch=tx.epoch;rayOwner169r1.request=pi.epoch;
            rayOwner169r1.sequence=pi.sample;rayOwner169r1.tick=pi.tick;
            kcd2vr::diagnostics::passive169r1::RayScope rayScope169r1(rayOwner169r1);
            pi.contactSuspended59c1=ContactSuspended59c1();
            pi.contactFaultH30=ContactFaultH30();
            const auto stageT4=StageNativeHeldT4(tx);
            if(stageT4==NativeHeldStageResultT4::Waiting)return RejectCoherent6693(tx,__LINE__);
            const bool solverT4=stageT4==NativeHeldStageResultT4::Applied;
            if(!solverT4){
                PhysClaimNativeRejoinR28(pi,tx.physicalState,tx.physicalProfiles,tx.physicalCorrelation126,
                    character,ts,std::uint64_t(GetTickCount64()));
                tx.physicalOutput=ph::step(tx.physicalState,pi,tx.physicalProfiles,[&](const ph::Profile& p,ph::Pose a,ph::Pose b,bool acquire){return PhysSweep6698(character,p,a,b,acquire);});
            }
            tx.physicalActive=true;
            if(!solverT4&&pi.sample!=g_phys_state6698.sample)(void)ConstrainOwnShield60(tx);
            kcd2vr::gesture_recovery_h30::MaskRecoveredContacts(tx.physicalOutput);
            if(pi.sample!=g_phys_state6698.sample){
                unsigned coordinationCg1=0;
                for(int side=0;side<2;++side)if(tx.doorHolsterAdmissionH40[side].take(1u<<side,true))coordinationCg1|=1u<<side;
                // Busy-bit coordination covers both hands; persistent door policy
                // remains the original per-hand eligibility, never a global veto.
                const unsigned doorAdmissionH40=coordinationCg1&~kcd2vr::hand_only_door_h40::Reserved();
                // Retained through completed publication/queue push; no staged
                // DRAW can appear after H40 passed a supposedly empty queue.
                auto holsterInput=ph::holster_tracking_input_h43(pi,PV(modelIntentGoalsH30[0]),PV(modelIntentGoalsH30[1]));
                // R38: per-arm reach accommodation cannot drag a holster with
                // the reaching hand. Zones retain the pre-reach torso anchors.
                holsterInput.shoulders[0]=PV(gestureShouldersR38[0]);
                holsterInput.shoulders[1]=PV(gestureShouldersR38[1]);
                std::array<pc::Vec3,2> holsterShouldersD70{};
                if(ts.head_valid&&!ts.head_held&&kcd2vr::holster_body_d70::shoulders(
                    PV(FromT(s.rest[rig.head])),PV(FromT(s.rest[rig.upper_r])),PV(FromT(s.rest[rig.upper_l])),
                    PV(headTarget),PQ(delta),holsterShouldersD70)){
                    for(int h=0;h<2;++h)holsterInput.shoulders[h]=holsterInput.torsoShouldersR38[h]=holsterShouldersD70[h];
                    holsterInput.torsoShouldersKnownR38=true;
                }
                // D85 holster: bow/shield/back zones hang from the PLAYER's head pivot (headset eye point,
                // same anchor+frame as the requested hands, minus the calibrated eye offset turned with the head).
                // D70 hung them from the eye point itself = ~9 cm in front of / 8 cm above his real shoulders.
                {std::array<pc::Vec3,2> playerShouldersD85{};
                 const Q4 neutralChestD85=QFromQuatT(s.neutral[rig.spine3]);
                 const auto axesD85=pc::chest_axes103(PV(QRotateV(neutralChestD85,s.chestRightLocal)),PV(QRotateV(neutralChestD85,s.chestUpLocal)));
                 if(ts.head_valid&&!ts.head_held&&axesD85&&kcd2vr::holster_body_d85::shoulders(
                     PV(FromT(s.rest[rig.head])),PV(FromT(s.rest[rig.upper_r])),PV(FromT(s.rest[rig.upper_l])),
                     PV(Add(headAnchor123,QRotateV(frame,XrToCryV(Sub(ts.head,trackedBase))))),PQ(delta),
                     axesD85->forward*double(g_eye_forward115.load())+axesD85->up*double(g_eye_up115.load()),playerShouldersD85)){
                     holsterInput.playerShouldersD85[0]=playerShouldersD85[0];holsterInput.playerShouldersD85[1]=playerShouldersD85[1];
                     holsterInput.playerShouldersKnownD85=true;
                 }}
                PhysHolsterGestures6698(holsterInput,tx.physicalEquipment,tx.physicalOutput,tx.physicalHolsters,doorAdmissionH40,coordinationCg1);
                // HS2 may stage only prerequisite stows this frame. They retain
                // both leases through the same successful publication and queue push.
                if(!kcd2vr::corpse_grip_cg1::stagedAcquisition(tx.physicalOutput))
                    kcd2vr::hand_only_door_h40::ReleaseUnstagedAdmissions(tx.physicalOutput,ph::CommandKind::Draw,tx.doorHolsterAdmissionH40);
            }
            tx.physicalActive=true;
            for(int side=0;side<2;++side)if(tx.physicalOutput.owned[side])goals[side]=VP(((tx.physicalOutput.displayLatched[side]||tx.physicalOutput.displayRejoinR13[side])?tx.physicalOutput.displayHands[side]:tx.physicalOutput.hands[side]).p); // R19 display packet
        }
    }
