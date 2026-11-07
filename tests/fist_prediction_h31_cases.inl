    {
    const auto savedCommitted=committed;const auto savedTracking=t;const auto savedPalms=palms;const auto savedNowH31=hostTickCount64;
    const auto modeH31=kcd2vr::user175::configuration(kcd2vr::user175::Mode::CombatSynchronized);
    g_census_mode=modeH31.pose;g_weapon_control_mode=modeH31.weapon;g_combat_experiment175=modeH31.experiment;
    g_combat_integrated178r22c2=true;g_synchronized59c10r9=true;g_intent59c16=g_intent_pi59c16=false;
    g_physical_combat177=modeH31.physical177;
    IntegratedFistSelection178r22c2();assert(FistSelected101()&&CombatSynchronized59c10r9()&&!CombatIntent59c16());
    assert(hit(2)==0&&h::receiptH31.failure==h::FailureH31::NoTrackedNativeSweep); // no armed predictor => no canned fallback
    auto* dosH31=reinterpret_cast<IMAGE_DOS_HEADER*>(image.data());dosH31->e_magic=IMAGE_DOS_SIGNATURE;dosH31->e_lfanew=0x100;
    auto* ntH31=reinterpret_cast<IMAGE_NT_HEADERS64*>(image.data()+0x100);ntH31->Signature=IMAGE_NT_SIGNATURE;ntH31->FileHeader.TimeDateStamp=kSupportedWhTimestamp;
    std::vector<unsigned char> characterH31(0x1800),skeletonH31(0x1000),parentsH31(512*0x30);
    std::array<std::uintptr_t,12> poseVftH31{};std::array<unsigned char,0x420> descriptorH31{},driverH31{};
    putH(characterH31.data(),base+kCCharInstanceVftRva);putH(characterH31.data()+kSkeletonAnimOffset,base+kCSkeletonAnimVftRva);
    putH(skeletonH31.data(),base+kCDefaultSkeletonVftRva);putH(skeletonH31.data()+8,parentsH31.data());
    putH(parentsH31.data()+0x16+417*0x30,std::int16_t(408));putH(parentsH31.data()+0x16+452*0x30,std::int16_t(443));
    auto* poseH31=characterH31.data()+kPoseInCharInstanceOffset;putH(poseH31,poseVftH31.data());
    poseVftH31[2]=base+kPoseGetterBuffer0Rva;poseVftH31[10]=base+kPoseGetterBuffer1Rva;
    putH(poseH31+kPoseDefaultSkeletonOffset,skeletonH31.data());
    putH(actor.data()+0x218,collisions.data());putH(collisions.data(),base+0x3B91300);
    putH(row.data()+0x224,0);putH(state.data()+0x1B8,0);
    t.sequence=9100;t.left_grip=t.left_trigger=t.right_grip=t.right_trigger=1;
    g_tracking_frames6695.publish(t);committed.stamp.actor=reinterpret_cast<std::uintptr_t>(characterH31.data());committed.stamp.sample=9100;
    committed.fistSkeletonH31=reinterpret_cast<std::uintptr_t>(skeletonH31.data());committed.fistJointCountH31=512;
    committed.fistWristBoneH31[0]=408;committed.fistWristBoneH31[1]=443;committed.fistSlotBoneH31[0]=417;committed.fistSlotBoneH31[1]=452;
    committed.fistSlotValidH31[0]=committed.fistSlotValidH31[1]=true;
    committed.allowed[0]=nativeWristH31;committed.fistSlotH31[0]={{0,0,0,1},{.15f,0,1.2f}};
    committed.allowed[1]={{0,0,0,1},{-.2f,.2f,1.3f}};
    committed.fistSlotH31[1]={{0,0,.24740396f,.96891242f},{-.2f,.23f,1.32f}};
    committed.palm59c9[0]={.12f,0,1.2f};committed.palm59c9[1]={-.2f,.23f,1.32f};
    g_committed_frame6695.publish(committed);FistReadOwner101(ownerH31);g_fist_owner101.publish(ownerH31);FistPublish101(t,committed,blockedH31);
    hostTickCount64+=10;t.tick=hostTickCount64;++t.sequence;committed.stamp.sample=t.sequence;
    committed.allowed[1].t[1]+=.1f;committed.fistSlotH31[1].t[1]+=.1f;committed.palm59c9[1].y+=.1f;
    g_tracking_frames6695.publish(t);g_committed_frame6695.publish(committed);FistReadOwner101(ownerH31);g_fist_owner101.publish(ownerH31);FistPublish101(t,committed,blockedH31);
    assert(g_fist_contact_h11.read(palms)&&palms.pairH31[1]);
    // The recognizer's immutable admitted stroke identity and forward direction.
    palms.gestureSampleH31[1]=t.sequence;palms.gestureDirectionH31[1]={0,1,0};g_fist_contact_h11.publish(palms);
    NativeTimingFist178r22c2(player.data(),t.sequence,t.tick,1,2.5);assert(NativeAttackStart22(action.data(),nullptr));
    putH(descriptorH31.data(),player.data());putH(descriptorH31.data()+0xC,std::int16_t(417));
    putH(descriptorH31.data()+0x40C,0);putH(descriptorH31.data()+0x411,std::uint8_t(1));
    QuatT28 identityH31{{0,0,0,1},{0,0,0}},sourceBoneH31{{0,0,0,1},{.15f,0,1.2f}};
    putH(descriptorH31.data()+0x20,identityH31);putH(descriptorH31.data()+0x3C,identityH31);putH(descriptorH31.data()+0xAC,sourceBoneH31);
    putH(driverH31.data()+0x18,descriptorH31.data());
    struct WrapperH31 {void* action=nullptr;unsigned char reserved[8]{};unsigned char admitted=1;} wrapperH31{descriptorH31.data()};
    g_prediction_contract120=base;g_prediction_fault120=false;
    const auto sourceH31=[&](){PredictionSourceBegin126(driverH31.data(),poseH31);
        assert(g_prediction_source126.action==descriptorH31.data());g_prediction_source126.wristGetterH31=&wristGetterH31;
        PredictionSourceCapture126(descriptorH31.data(),poseH31,&sourceBoneH31);};
    sourceH31();PredictionScope120 proofH31{};
    const bool readyH31=PredictionPrepare120(&wrapperH31,.01f,base+0xA7CDEE,proofH31);
    std::printf("H31 fist predictor ready=%d context=%s wristReads=%u\n",readyH31,proofH31.context121,wristReadsH31);
    std::fflush(stdout);
    assert(readyH31); // baseline is RED: PHYSICAL_OWNER excludes every fist
    g_prediction_original120=&nativePredictionH31;g_interpolate_original120=&interpolateH31;g_phys_native_sweep_original110=&nativeSweepH31;
    sweepReceiverH31=receiver.data();sweepDetailsH31=collisions.data()+0x170;putH(request.data()+0x18,sweepDetailsH31);
    const auto runH31=[&](bool accepted){sourceH31();const auto np=predictionsH31,ns=sweepsH31,nr=processesH,nstart=startsH;
        PredictionRun120(&wrapperH31,.01f,base+0xA7CDEE);
        assert(predictionsH31==np+1&&startsH==nstart); // original once; no second attack/cost producer
        assert(sweepsH31==ns+(accepted?1:0)&&processesH==nr+(accepted?1:0));
        assert(!g_prediction_scope120&&!h::Scope::live);
    };
    g_fist_prediction_lease_h31={};runH31(true);
    {
    // The native punch above is already admitted. These checks execute the
    // actual completed-pose publisher and public predictor while preserving its
    // ORIGINAL gesture/attack/cost identity, including two-eye consumption.
    const auto savedClockTrackingH31=t;const auto savedClockBodyH31=committed;const auto savedClockPalmsH31=palms;
    const auto savedClockNowH31=hostTickCount64;const auto savedClockLeaseH31=g_fist_prediction_lease_h31;
    const auto savedClockSelectedTickH31=g_f4_fists_tick178r22c2.load(),savedClockSelectedEpochH31=g_f4_fists_epoch178r22c2.load();
    const auto savedClockFromH31=sweptFromH31,savedClockToH31=sweptToH31;
    const auto savedClockMotionH31=g_fist_motion101;const auto savedClockPendingH31=g_fist_pending101;
    const auto savedClockRequestH31=g_fist_motion_request101;FistFrame101 savedClockFrameH31{};g_fist_frame101.read(savedClockFrameH31);
    const auto attackStartsClockH31=startsH;
    unsigned clockChecksH31=0,clockFailuresH31=0;
    const auto clockCheckH31=[&](bool pass,const char* label){++clockChecksH31;if(!pass){++clockFailuresH31;std::printf("FAIL H31CLOCK %s\n",label);}};
    const auto clockPublishH31=[&](){committed.stamp.sample=t.sequence;t.head_good_tick=t.right_good_tick=t.left_good_tick=t.tick;
        g_tracking_frames6695.publish(t);g_committed_frame6695.publish(committed);
        IntegratedFistSelection178r22c2(); // normal native pump, not a granted pose/contact
        FistReadOwner101(ownerH31);g_fist_owner101.publish(ownerH31);FistPublish101(t,committed,blockedH31);
        assert(g_fist_contact_h11.read(palms));};
    const char* clockContextH31=nullptr;
    const auto clockPredictH31=[&](){sourceH31();PredictionScope120 s{};
        const bool ready=PredictionPrepare120(&wrapperH31,.01f,base+0xA7CDEE,s);
        clockContextH31=s.context121;
        std::printf("H31CLOCK tick=%llu ns=%llu sample=%llu pair=%d gesture=%llu ready=%d context=%s\n",
            (unsigned long long)t.tick,(unsigned long long)t.timeNs,(unsigned long long)t.sequence,palms.pairH31[1],
            (unsigned long long)palms.gestureSampleH31[1],ready,s.context121);return ready;};
    // Seed real completed publications in one original nanosecond domain. The
    // existing fixture's already-earned gesture is carried, never re-stamped.
    for(unsigned i=0;i<2;++i){hostTickCount64+=5;t.tick=hostTickCount64;t.timeNs=1000000000+i*5000000;++t.sequence;clockPublishH31();}
    palms.gestureSampleH31[1]=savedClockPalmsH31.gestureSampleH31[1];palms.gestureDirectionH31[1]=savedClockPalmsH31.gestureDirectionH31[1];
    g_fist_contact_h11.publish(palms);
    for(unsigned i=0;i<3;++i){if(i==2){hostTickCount64+=16;t.tick=hostTickCount64;}
        t.timeNs+=11111111;++t.sequence;committed.allowed[1].t[1]+=.003f;committed.fistSlotH31[1].t[1]+=.003f;committed.palm59c9[1].y+=.003f;
        clockPublishH31();const bool ready=clockPredictH31();
        clockCheckH31(palms.pairH31[1],"new completed sample retains original pair despite equal coarse raw tick");
        clockCheckH31(palms.gestureSampleH31[1]==savedClockPalmsH31.gestureSampleH31[1],"original admitted gesture survives, no fresh onset");
        clockCheckH31(ready,"public native predictor accepts current original world pair");
        const auto eyeBeforeH31=palms;FistPublish101(t,committed,blockedH31);assert(g_fist_contact_h11.read(palms));
        clockCheckH31(palms.sample==eyeBeforeH31.sample&&palms.pairH31[1]==eyeBeforeH31.pairH31[1]&&
            palms.previousSampleH31[1]==eyeBeforeH31.previousSampleH31[1]&&palms.gestureSampleH31[1]==eyeBeforeH31.gestureSampleH31[1],"same original two-eye publication is idempotent");
        runH31(ready);clockCheckH31(startsH==attackStartsClockH31,"native prediction never requests a second attack/cost");
    }
    const auto baseClockTrackingH31=t;const auto baseClockBodyH31=committed;const auto baseClockPalmsH31=palms;const auto baseClockNowH31=hostTickCount64;
    const auto clockRestoreH31=[&](){t=baseClockTrackingH31;committed=baseClockBodyH31;palms=baseClockPalmsH31;hostTickCount64=baseClockNowH31;
        g_tracking_frames6695.publish(t);g_committed_frame6695.publish(committed);g_fist_contact_h11.publish(palms);g_fist_prediction_lease_h31={};IntegratedFistSelection178r22c2();};
    for(unsigned control=0;control<8;++control){clockRestoreH31();++t.sequence;t.timeNs+=11111111;
        if(control==0)t.timeNs=baseClockTrackingH31.timeNs-1;
        if(control==1)t.timeNs=0;
        if(control==2)t.sequence=baseClockTrackingH31.sequence;
        if(control==3)t.sequence=baseClockTrackingH31.sequence-1;
        if(control==4){--hostTickCount64;t.tick=hostTickCount64;}
        if(control==5){hostTickCount64+=61;t.tick=hostTickCount64;t.timeNs+=61000000;}
        if(control==6)++committed.stamp.actor;
        if(control==7)++committed.fistJointCountH31;
        clockPublishH31();clockCheckH31(!palms.pairH31[1],"reset/backwards/duplicate/old source, raw expiry or changed body cannot borrow pair");
        clockCheckH31(!clockPredictH31(),"public native predictor retains original negative ownership/chronology controls");
    }
    clockRestoreH31();hostTickCount64+=61;IntegratedFistSelection178r22c2();
    clockCheckH31(!clockPredictH31()&&!std::strcmp(clockContextH31,"FIST_CURRENT_TRACKING"),"actual raw60ms expiry still refuses stale contact and tracking");
    clockRestoreH31();++palms.gestureSampleH31[1];g_fist_contact_h11.publish(palms);
    clockCheckH31(!clockPredictH31()&&!std::strcmp(clockContextH31,"FIST_ORIGINAL_GESTURE_PAIR"),"different punch cannot borrow an admitted gesture");
    clockRestoreH31();const auto oldEpochClockH31=g_fist_epoch101.load();++g_fist_epoch101;
    clockCheckH31(!clockPredictH31()&&!std::strcmp(clockContextH31,"FIST_ADMITTED_ACTION"),"different current native request cannot borrow old punch");g_fist_epoch101=oldEpochClockH31;
    clockRestoreH31();
    for(unsigned i=0;i<2;++i){hostTickCount64+=5;t.tick=hostTickCount64;t.timeNs=0;++t.sequence;clockPublishH31();}
    palms.gestureSampleH31[1]=savedClockPalmsH31.gestureSampleH31[1];palms.gestureDirectionH31[1]=savedClockPalmsH31.gestureDirectionH31[1];g_fist_contact_h11.publish(palms);
    clockCheckH31(palms.pairH31[1]&&clockPredictH31(),"unavailable nanosecond clocks retain original positive raw-time path");
    clockCheckH31(startsH==attackStartsClockH31,"all controls preserve once-only original native attack/cost count");
    t=savedClockTrackingH31;committed=savedClockBodyH31;palms=savedClockPalmsH31;hostTickCount64=savedClockNowH31;
    g_tracking_frames6695.publish(t);g_committed_frame6695.publish(committed);g_fist_contact_h11.publish(palms);
    g_fist_prediction_lease_h31=savedClockLeaseH31;g_fist_motion101=savedClockMotionH31;g_fist_pending101=savedClockPendingH31;g_fist_motion_request101=savedClockRequestH31;g_fist_frame101.publish(savedClockFrameH31);
    sweptFromH31=savedClockFromH31;sweptToH31=savedClockToH31;
    g_f4_fists_epoch178r22c2=savedClockSelectedEpochH31;g_f4_fists_tick178r22c2=savedClockSelectedTickH31;
    FistReadOwner101(ownerH31);g_fist_owner101.publish(ownerH31);
    std::printf("H31CLOCK %u/%u checks PASS\n",clockChecksH31-clockFailuresH31,clockChecksH31);std::fflush(stdout);assert(!clockFailuresH31);
    }
    assert(pm::length(PhysPose6698(sweptFromH31).p-palms.previousSlotH31[1].p)<1.e-6);
    assert(pm::length(PhysPose6698(sweptToH31).p-palms.worldSlotH31[1].p)<1.e-6);
    assert(pm::angle(PhysPose6698(sweptToH31).q,palms.worldSlotH31[1].q)<1.e-6); // LEFT's independent axes, not native RIGHT offset
    runH31(true);assert(pm::length(PhysPose6698(sweptToH31).p-PhysPose6698(sweptFromH31).p)<1.e-6); // no replayed interval
    publishNewerH31=true;runH31(true);publishNewerH31=false;
    assert(h::receiptH31.palmSample==palms.sample&&h::receiptH31.committedSample==palms.sample+1);
    g_committed_frame6695.publish(committed);g_fist_contact_h11.publish(palms);
    for(int typeH31:{6,7,5}){putH(type.data()+8,typeH31);putH(state.data()+0x218,typeH31);
        NativeTimingFist178r22c2(player.data(),t.sequence,t.tick,1,2.5);assert(NativeAttackStart22(action.data(),nullptr));runH31(true);}
    // Reverse the asymmetric tuple too: native LEFT slot452, VR RIGHT side0.
    const auto leftGesturePalmsH31=palms;
    putH(row.data()+0x224,1);putH(state.data()+0x1B8,1);putH(descriptorH31.data()+0x40C,1);putH(descriptorH31.data()+0xC,std::int16_t(452));
    sourceBoneH31.t[0]=-.15f;putH(descriptorH31.data()+0xAC,sourceBoneH31);
    palms.pairH31[0]=true;palms.previousSlotH31[0]=palms.worldSlotH31[0];palms.previousSlotH31[0].p.y-=.08;
    palms.previous[0]=palms.palm[0];palms.previous[0].y-=.08;palms.gestureSampleH31[0]=t.sequence;palms.gestureDirectionH31[0]={0,1,0};
    g_fist_contact_h11.publish(palms);NativeTimingFist178r22c2(player.data(),t.sequence,t.tick,0,2.5);assert(NativeAttackStart22(action.data(),nullptr));runH31(true);
    assert(pm::length(PhysPose6698(sweptToH31).p-palms.worldSlotH31[0].p)<1.e-6);
    palms=leftGesturePalmsH31;g_fist_contact_h11.publish(palms);putH(row.data()+0x224,0);putH(state.data()+0x1B8,0);
    putH(descriptorH31.data()+0x40C,0);putH(descriptorH31.data()+0xC,std::int16_t(417));sourceBoneH31.t[0]=.15f;putH(descriptorH31.data()+0xAC,sourceBoneH31);
    NativeTimingFist178r22c2(player.data(),t.sequence,t.tick,1,2.5);assert(NativeAttackStart22(action.data(),nullptr));
    auto savedSourceH31=g_prediction_source126;
    wrapperH31.admitted=0;runH31(false);wrapperH31.admitted=1;
    t.left_trigger=.60f;g_tracking_frames6695.publish(t);runH31(false);t.left_trigger=1;g_tracking_frames6695.publish(t);
    t.left_grip=.60f;g_tracking_frames6695.publish(t);runH31(false);t.left_grip=1;g_tracking_frames6695.publish(t);
    t.left_held=true;g_tracking_frames6695.publish(t);runH31(false);t.left_held=false;g_tracking_frames6695.publish(t);
    g_ctx_menu=true;runH31(false);g_ctx_menu=false;
    ++g_fist_epoch101;runH31(false);--g_fist_epoch101;
    putH(parentsH31.data()+0x16+417*0x30,std::int16_t(443));runH31(false);putH(parentsH31.data()+0x16+417*0x30,std::int16_t(408));
    const auto qualifiedPalmsH31=palms;
    palms.gestureDirectionH31[1]={0,-1,0};g_fist_contact_h11.publish(palms);runH31(false);palms=qualifiedPalmsH31;g_fist_contact_h11.publish(palms);
    // An armed public F4 request still goes through the existing WEAPON path;
    // the mere experiment flag must not select the empty-hand producer.
    putH(hands.data()+0x260,std::uint64_t(77));IntegratedFistSelection178r22c2();
    PredictionScope120 armedH31{};assert(!PredictionPrepare120(&wrapperH31,.01f,base+0xA7CDEE,armedH31)&&!std::strcmp(armedH31.context121,"PHYSICAL_OWNER"));
    putH(hands.data()+0x260,std::uint64_t(0));IntegratedFistSelection178r22c2();
    // Original WORLD endpoints survive root movement and nonunit model scale.
    const auto publishH31=[&](){++t.sequence;hostTickCount64+=10;t.tick=hostTickCount64;committed.stamp.sample=t.sequence;
        g_tracking_frames6695.publish(t);g_committed_frame6695.publish(committed);FistReadOwner101(ownerH31);g_fist_owner101.publish(ownerH31);FistPublish101(t,committed,blockedH31);
        assert(g_fist_contact_h11.read(palms));};
    committed.world[7]=1.2f;committed.world[4]=1;publishH31();assert(!palms.pairH31[1]); // new scale seeds a new pair
    const auto originalWorldSlotH31=palms.worldSlotH31[1];
    committed.world[5]=.01f;committed.allowed[1].t[1]+=.02f;committed.fistSlotH31[1].t[1]+=.02f;committed.palm59c9[1].y+=.02f;publishH31();
    assert(palms.pairH31[1]&&pm::length(palms.previousSlotH31[1].p-originalWorldSlotH31.p)<1.e-9);
    palms.gestureSampleH31[1]=t.sequence;palms.gestureDirectionH31[1]={0,1,0};g_fist_contact_h11.publish(palms);
    NativeTimingFist178r22c2(player.data(),t.sequence,t.tick,1,2.5);assert(NativeAttackStart22(action.data(),nullptr));
    runH31(true);assert(pm::length(PhysPose6698(sweptFromH31).p-originalWorldSlotH31.p)<1.e-6);
    assert(std::abs(PhysPose6698(sweptToH31).p.y-PhysPose6698(sweptFromH31).p.y-(.01+.02*1.2))<1.e-6);
    // Only an already prepared/released exact punch gets150ms START time.
    at22::Intent pendingH31{};assert(g_attack_timing_intent22.read(pendingH31));
    const auto savedNativeH31=g_fist_native101;g_fist_native101.player=player.data();g_fist_native101.request=pendingH31.request;
    g_fist_native101.epoch=pendingH31.epoch;g_fist_native101.side=pendingH31.side;g_fist_native101.pendingSample=pendingH31.sample;
    g_fist_native101.pendingTick=pendingH31.tick+66;g_fist_native101.lastAttack=pendingH31.tick+93;
    for(unsigned ageH31:{100u,109u,150u})assert(NativeStartIntentEligibleH31(pendingH31,pendingH31.tick+ageH31,0));
    assert(!NativeStartIntentEligibleH31(pendingH31,pendingH31.tick+151,0));
    auto weaponH31=pendingH31;weaponH31.kind=at22::Kind::Slash;assert(!NativeStartIntentEligibleH31(weaponH31,weaponH31.tick+109,0));
    ++pendingH31.request;assert(!NativeStartIntentEligibleH31(pendingH31,pendingH31.tick+109,0));--pendingH31.request;
    g_fist_native101.lastAttack=0;g_fist_native101.prepared=true;g_fist_release_call_h31=true;
    assert(NativeStartIntentEligibleH31(pendingH31,pendingH31.tick+109,0));g_fist_release_call_h31=false;
    assert(!NativeStartIntentEligibleH31(pendingH31,pendingH31.tick+109,0));
    // Reproduce the actual109ms delayed native START with current tracking;
    // neither native input nor the original occurrence clock is refreshed.
    NativeTimingFist178r22c2(player.data(),t.sequence,t.tick,1,2.5);assert(g_attack_timing_intent22.read(pendingH31));
    g_fist_native101.pendingSample=pendingH31.sample;g_fist_native101.pendingTick=pendingH31.tick+66;g_fist_native101.lastAttack=pendingH31.tick+93;
    // Ten continuously published physical samples remain forward while the
    // separate native action admission is delayed. No source timestamp moves.
    for(int frameH31=0;frameH31<10;++frameH31){IntegratedFistSelection178r22c2();
        committed.allowed[1].t[1]+=.003f;committed.fistSlotH31[1].t[1]+=.003f;committed.palm59c9[1].y+=.003f;publishH31();}
    hostTickCount64=pendingH31.tick+109;t.tick=t.head_good_tick=t.right_good_tick=t.left_good_tick=hostTickCount64;
    g_tracking_frames6695.publish(t);IntegratedFistSelection178r22c2();
    TrackingSnapshot lateTrackingH31{};ReadTrackingSnapshot(lateTrackingH31);
    std::printf("H31 late eligible=%d selected=%d nativeSelected=%d tracking=%d%d%d tick=%llu now=%llu serial=%llu consumed=%llu\n",
        NativeStartIntentEligibleH31(pendingH31,hostTickCount64,g_attack_timing_consumed22.load()),FistSelected101(),NativeTimingSelected22(),lateTrackingH31.head_valid,lateTrackingH31.right_valid,lateTrackingH31.left_valid,
        (unsigned long long)lateTrackingH31.tick,(unsigned long long)hostTickCount64,(unsigned long long)pendingH31.serial,(unsigned long long)g_attack_timing_consumed22.load());std::fflush(stdout);
    assert(NativeAttackStart22(action.data(),nullptr));h::Binding delayedH31{};
    assert(h::current(delayedH31,hostTickCount64)&&delayedH31.intent.tick==pendingH31.tick&&delayedH31.intent.sample==pendingH31.sample);
    runH31(true);assert(h::receiptH31.gestureSample==pendingH31.sample&&h::receiptH31.palmSample>pendingH31.sample);
    // The physical hand has retracted by the time of a later native query:
    // current backward motion cannot be turned into a new or replayed hit.
    committed.allowed[1].t[1]-=.02f;committed.fistSlotH31[1].t[1]-=.02f;committed.palm59c9[1].y-=.02f;publishH31();
    const auto lateBeforeH31=processesH;runH31(false);assert(processesH==lateBeforeH31);
    g_fist_native101=savedNativeH31;
    std::puts("H31 public HYBRID fist predictor: types5/6/7 original input/interpolation/sweep/receiver once; original world/scale, exact native wrist/topology, >60%, lifetime/retraction/armed-owner negatives PASS");
    putH(request.data()+0x18,contact.data());
    g_prediction_contract120=0; // restore the isolated receiver fixture below
    g_census_mode=5;g_weapon_control_mode=0;g_combat_experiment175=false;g_combat_integrated178r22c2=false;g_synchronized59c10r9=false;g_physical_combat177=false;
    committed=savedCommitted;t=savedTracking;palms=savedPalms;hostTickCount64=savedNowH31;
    g_committed_frame6695.publish(committed);g_tracking_frames6695.publish(t);g_fist_contact_h11.publish(palms);
    putH(row.data()+0x224,1);putH(state.data()+0x1B8,1);assert(start());
    }
