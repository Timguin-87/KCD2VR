    // C51R1: own the ordinary native locomotion frame, then add actual HMD and
    // controller offsets once. Rest pelvis/head anatomy is the same fixed-length
    // reference as this solver; no current attack/bow head is learned as standing.
    tx.frameState51=s.trackedFrame51;
    ModelWorld6692 frameWorld51{};
    bool acquisitionFrame54=false;
    if(full&&acquisitionCurrent54&&s.legsReady128){
        const bool retained=s.acquisitionSerial54==tx.acquisition54.serial&&s.acquisitionAction54==tx.acquisition54.action;
        const bool pending=s.pendingUse55.serial==tx.acquisition54.serial&&
            s.pendingUse55.requestedAt==tx.acquisition54.startedAt&&s.pendingUse55.side==tx.acquisition54.side;
        const auto reference=retained?s.acquisitionReference54:(pending?s.pendingReference55:s.ordinaryReference54);
        const bool preQueue=kcd2vr::tracked_frame51::ordinary_reference_d86(reference)&&
            reference.stamp.actor==tx.acquisition54.character&&reference.stamp.epoch==tx.epoch&&reference.control==tx.control51&&
            pc::length(reference.calibratedHead-PV(baseHead))<1e-6&&
            pc::length(reference.restOffset-(PV(FromT(s.rest[rig.head]))-PV(FromT(s.rest[rig.pelvis]))))<1e-6&&
            reference.stamp.tick<=tx.acquisition54.startedAt&&tx.acquisition54.startedAt-reference.stamp.tick<=80;
        if(preQueue){tx.acquisitionReference54=reference;acquisitionFrame54=ReadModelWorld6692(character,frameWorld51)&&
            std::fabs(frameWorld51.scale-reference.scale)<1e-6;}
    }
    // D39: an occluded drawing hand does not own the headset. BeginCoherent
    // already requires a fresh head and at least one usable hand; keep that
    // same body frame alive while the opposite controller remains usable.
    // A missing hand gets no valid wrist, contact or neutral-learning authority.
    const bool ordinaryFrame51=full&&(g_gameplay_allowed.load(std::memory_order_acquire)||adoptedVisualCurrentR95)&&
        !readingVisual32&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&(!g_ctx_scripted.load()||adoptedVisualCurrentR95)&&!g_ctx_reading.load()&&
        !animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175()&&
        ts.head_valid&&!ts.head_held&&(ts.right_valid||ts.left_valid)&&s.legsReady128&&ReadModelWorld6692(character,frameWorld51);
    // A still-valid hand's original display grace must not switch the shared
    // coordinate basis. Action/input owners keep their original held checks;
    // neutral measurement still requires every component to be freshly tracked.
    const bool ordinaryFresh51=ordinaryFrame51&&g_gameplay_allowed.load(std::memory_order_acquire)&&ts.right_valid&&ts.left_valid&&!ts.right_held&&!ts.left_held;
    // Saved FREE VRIK alchemy/smithing uses the same completed C51 body/camera
    // frame: current native pelvis plus rest head offset, then current HMD once.
    // Keep its exact native identity even when ordinary gameplay is admitted;
    // an authored workstation eye must never train neutral anatomy or heading.
    kcd2vr::workstation178r20c1::WorkstationOwnerH51 workstationOwnerH51{};
    const bool workstationOwnedH51=full&&workstation178r20c1::workstationOwnerH51(GetTickCount64(),workstationOwnerH51);
    // AS1: the nested alchemy recipe book is the same FREE workstation (see workstation178r20c1.h), not a Reading body.
    const bool alchemyBookAS1=workstationOwnedH51&&workstationOwnerH51.alchemyBookAS1;
    const bool workstationFrameH51=workstationOwnedH51&&
        (!readingVisual32||alchemyBookAS1)&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&(!g_ctx_reading.load()||alchemyBookAS1)&&
        !animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175()&&
        !ts.head_held&&!ts.right_held&&!ts.left_held&&s.legsReady128&&(ordinaryFrame51||ReadModelWorld6692(character,frameWorld51));
    kcd2vr::tracked_frame51::Input fi51{};
    fi51.stamp={tx.epoch,ts.sequence,ts.tick,reinterpret_cast<std::uintptr_t>(character)};
    fi51.control=tx.control51;fi51.timeNs=ts.timeNs;fi51.scale=frameWorld51.scale;
    fi51.nativeAnchorPolicyR14=kcd2vr::smooth_lab::NativeAnchorPolicy();
    fi51.nativeHead=PV(liveNativeHead123);fi51.nativePelvis=PV(FromT(s.original[rig.pelvis]));
    fi51.restHead=PV(FromT(s.rest[rig.head]));fi51.restPelvis=PV(FromT(s.rest[rig.pelvis]));fi51.calibratedHead=PV(baseHead);
    fi51.allowed=ordinaryFrame51||acquisitionFrame54||workstationFrameH51;fi51.workstationKindH51=workstationFrameH51&&!acquisitionFrame54?workstationOwnerH51.kind:0;fi51.smooth=smoothBody51;fi51.hmdHeight=(tx.control51&kNativeBodyHeightBit54)==0;
    // R98: COMBINED = HMD frame plus native height while crouched or on/at a horse.
    fi51.combinedHeightR98=fi51.hmdHeight&&(tx.control51&kCombinedBodyHeightBitR98)!=0;
    if(fi51.workstationKindH51){fi51.workstationModeEpochH51=workstationOwnerH51.modeEpoch;
        fi51.workstationLifetimeH51=workstationOwnerH51.lifetime;fi51.workstationInstanceH51=workstationOwnerH51.instance;}
    if(acquisitionFrame54){fi51.acquisitionSerial=tx.acquisition54.serial;fi51.acquisitionAction=tx.acquisition54.action;
        fi51.acquisitionScope=tx.acquisition54.scope;fi51.acquisitionToken=tx.acquisition54.nativeToken;
        fi51.acquisitionAnchor=tx.acquisitionReference54.anchor;
        fi51.acquisitionCrouchOffsetR93=tx.acquisitionReference54.nativeCrouchOffsetR93;}
    // Saved Free/6DOF stations own visible motion; native actions still run.
    // Keep ordinary native crouch-height preference outside this exact current
    // workstation frame. Animated local pelvis Z otherwise moves head, hands
    // and body together even though the station/entity world has not moved.
    const bool freeStationFoundationR13=fi51.workstationKindH51&&
        kcd2vr::minigame_hands45::freeTrackedMode(kcd2vr::minigame_hands45::modeFor(fi51.workstationKindH51));
    if(freeStationFoundationR13){fi51.hmdHeight=true;fi51.combinedHeightR98=false;}
    if(tx.ridingGaitD76)fi51.hmdHeight=false; // use this rider's current native pelvis foundation
    // D86: dice/rider/station poses still use the shared frame, but never
    // train ordinary eye anatomy or carry their native-height filter outside
    // that exact activity lifetime. Leaving the activity retires only this
    // presentation cache; XR calibration and physical HMD movement are untouched.
    using ReferenceDomainD86=kcd2vr::tracked_frame51::ReferenceDomainD86;
    fi51.referenceDomainD86=tx.diceBodyD81?ReferenceDomainD86::Dice:tx.ridingGaitD76?ReferenceDomainD86::Mounted:
        fi51.workstationKindH51?ReferenceDomainD86::Workstation:ordinaryFrame51?ReferenceDomainD86::Ordinary:ReferenceDomainD86::Other;
    if(tx.diceBodyD81){fi51.referenceInstanceD86=tx.diceLeaseD81.instance;fi51.referenceLifetimeD86=tx.diceLeaseD81.lifetime;}
    else if(tx.ridingGaitD76)fi51.referenceLifetimeD86=tx.ridingLifetimeR95;
    else if(fi51.workstationKindH51){fi51.referenceInstanceD86=fi51.workstationInstanceH51;fi51.referenceLifetimeD86=fi51.workstationLifetimeH51;}
    // R98: recent seated/mount/dismount clip on Henry (animation scan, <=750 ms).
    fi51.nativeRiderR98=fi51.combinedHeightR98&&kcd2vr::riding_vrik_d76::RiderRecentR98(std::uint64_t(GetTickCount64()));
    if(fi51.hmdHeight&&fi51.referenceDomainD86==ReferenceDomainD86::Ordinary&&
       !acquisitionFrame54&&!fi51.workstationKindH51&&
       (kcd2vr::native_crouch_r93::enabled.load(std::memory_order_acquire)||fi51.combinedHeightR98)){
        namespace crouch=kcd2vr::native_crouch_r93;crouch::Receipt receipt{};
        const auto token=crouch::generation.load(std::memory_order_acquire);
        const crouch::Identity identity{WhBase(),reinterpret_cast<std::uintptr_t>(g_player_module_d76.load()),
            reinterpret_cast<std::uintptr_t>(g_last_player_entity.load()),fi51.stamp.actor,tx.epoch,ts.source_epoch,
            tx.control51,g_player_lifetime_d44.load(),std::uint64_t(GetTickCount64())};
        if(crouch::receipt.read(receipt)&&crouch::current(receipt,identity,token)&&token==crouch::generation.load()){
            fi51.nativeStanceR93=true;fi51.nativeCrouchedR93=receipt.crouched;
        }
    }
    fi51.eyeNeutral=ordinaryFresh51&&fi51.referenceDomainD86==ReferenceDomainD86::Ordinary&&g_frame_neutral51&&g_frame_neutral51(GetTickCount64());
    // D37B: exact native held-bow identity owns only the shared camera/body
    // foundation. Missing grip geometry must not relabel this bow as ordinary
    // gameplay and restore native animated aim yaw to the headset.
    const auto cameraBowD37B=kcd2vr::bow_camera_identity_d37b::qualify(tx.physicalEquipment,
        {g_henry_item_attach_manager.load(std::memory_order_acquire),
         g_weapon_request_epoch6694.load(std::memory_order_acquire),g_pose_epoch6689.load(std::memory_order_acquire),
         preparedRequest169r2,tx.epoch,std::uint64_t(GetTickCount64()),prepared169r2});
    fi51.bow=bool(cameraBowD37B);
    fi51.bowKey=cameraBowD37B.key;fi51.bowGeneration=cameraBowD37B.generation;fi51.bowRequest=cameraBowD37B.request;
    fi51.drawing=fi51.bow&&((sameBow169r2&&priorBow169r2.bowHeld)||
        (BowGripDrawEnabled596C1()?ts.right_grip:ts.right_trigger)>.5f);
    kcd2vr::station_reposition_d53::Input moveD53{};
    moveD53.owner={reinterpret_cast<std::uintptr_t>(character),workstationOwnerH51.instance,
        tx.epoch,ts.source_epoch,tx.control51,workstationOwnerH51.modeEpoch,
        workstationOwnerH51.stationLifetimeH46,workstationOwnerH51.kind};
    moveD53.sample=ts.sequence;moveD53.tick=ts.tick;moveD53.now=GetTickCount64();
    moveD53.trackingBasis=PQ(frame);moveD53.scale=frameWorld51.scale;
    moveD53.forward=PV(QRotateV(frame,XrToCryV(QRotateV(ts.hq,V3{0,0,-1}))));
    moveD53.active=workstationFrameH51&&fi51.workstationKindH51&&!acquisitionFrame54&&
        !fi51.bow&&kcd2vr::minigame_hands45::modeFor(fi51.workstationKindH51)==kcd2vr::minigame_hands45::SixDofVrik;
    (void)kcd2vr::station_reposition_d53::command.read(moveD53.command);
    kcd2vr::station_reposition_d53::Output movedD53{};
    if(kcd2vr::tracked_frame51::advance_station_d53(tx.frameState51,fi51,moveD53,tx.frame51,&movedD53)){
        if(movedD53.moved&&kcd2vr::station_ws_j27::captureD47(moveD53.now)){
            static std::uint64_t nextMoveD53=0;
            if(moveD53.now>=nextMoveD53){nextMoveD53=moveD53.now+1000;char lineD53[420]{};
                std::snprintf(lineD53,sizeof(lineD53),"D53 STATION_REPOSITION phase=STAGED display=UNOBSERVED kind=%u instance=%p sample=%llu offsetModel=(%.4f,%.4f,%.4f) radiusM=0.40 speedMps=0.30 owner=SHARED_CAMERA_BODY_HANDS nativeCapsule=UNCHANGED",
                    moveD53.owner.kind,reinterpret_cast<void*>(moveD53.owner.instance),(unsigned long long)ts.sequence,
                    movedD53.offset.x,movedD53.offset.y,movedD53.offset.z);diagnostic_trace::Write(lineD53);}
        }
        const V3 change=Sub(VP(tx.frame51.anchor),headAnchor123);
        headAnchor123=VP(tx.frame51.anchor);headTarget=Add(headTarget,change);
        if(!directHandsR135)for(int side=0;side<2;++side){requestedGoals[side]=Add(requestedGoals[side],change);requestedGoalsRawH50[side]=Add(requestedGoalsRawH50[side],change);goals[side]=Add(goals[side],change);}
        tx.frame51.root=PV(frameWorld51.t);tx.frame51.rotation=PQ(frameWorld51.q);
    }
    // Mounted upper-body goals must never commit on an unjoined standing/native
    // mixture. A refusal is transaction-local; the next fresh seat pose retries.
    if(tx.ridingGaitD76&&!tx.frame51.valid){tx.stage="MOUNTED_FRAME_UNAVAILABLE_R95";return RejectCoherent6693(tx,__LINE__);}
    // C54: the ordinary completed tracked frame also owns the upper-body
    // foundation. Native pelvis pitch/roll otherwise survives the neutral-local
    // reconstruction, tipping the first spine pivot and neck link beneath a
    // stationary HMD. Keep native pelvis translation; retain original ankle
    // targets with the existing fixed-length leg solve below. A protected
    // reaction/activity has no frame51 and never enters this presentation path.
    bool framePelvis54=false;
    if(tx.frame51.valid){
        // D56: native local yaw also rotates both shoulder roots. Tracked body
        // follow and chest participation below already supply the visible yaw;
        // retaining the authored attack/gait yaw stacks a second moving basis
        // under stationary controllers. Entity-world turn and native ankle
        // targets remain current. Protected native owners have no frame51.
        const bool isolateNativePelvisYawR25=ordinaryFrame51||acquisitionFrame54;
        const auto foundation54=kcd2vr::tracked_frame51::pelvis_foundation54(
            PQ(QFromQuatT(s.original[rig.pelvis])),PQ(QFromQuatT(s.rest[rig.pelvis])),freeStationFoundationR13||isolateNativePelvisYawR25);
        if(!foundation54)return RejectCoherent6693(tx,__LINE__);
        framePelvis54=pc::angle(*foundation54,PQ(QFromQuatT(s.work[rig.pelvis])))>1e-6;
        if(framePelvis54){
            SetWorldRotation6689(s,rig.pelvis,QP(*foundation54));
            if(!Compose6689(s))return RejectCoherent6693(tx,__LINE__);
            s.neutral=s.work;
        }
    }
