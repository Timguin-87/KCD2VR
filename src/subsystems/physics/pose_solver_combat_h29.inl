// One game-thread consumer of the physics producer's separate RPG queue.
// Parry retains its own queue; no producer callback dispatches native combat.
{
 namespace ne=kcd2vr::native_weapon_contact_h24;namespace bridge=kcd2vr::native_weapon_combat_h29;
 static ne::Event pendingH29{};static bool haveH29=false;PhysFrame6698 fH29{};
 const auto nowH29=std::uint64_t(GetTickCount64());
 const bool enabledH29=combat_diag_probe::GeometryHitsEnabled178r27()&&CombatSynchronized59c10r9()&&
     !kcd2vr::motion59c4::selected.load(std::memory_order_acquire)&&heldT4::Enabled()&&
     retail_build112::Ready(WhBase())&&g_phys_frame6698.read(fH29)&&fH29.output.solverOwnedT4[0]&&
     fH29.input.sample==g_phys_ready_sample6698.load(std::memory_order_acquire)&&fH29.input.allowed&&
     fH29.input.attacks&&fH29.input.bodyEpoch==g_pose_epoch6689.load()&&fH29.request==g_weapon_request_epoch6694.load()&&
     g_gameplay_allowed.load()&&!animation_filter::nativeBodyOwned112.load()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
     ph::attack_authority55c1(fH29.input,fH29.output,0);
 if(!enabledH29)haveH29=false;
 else for(unsigned countH29=0;countH29<8;++countH29){
     if(!haveH29){if(!ne::ReadContactEventH24(pendingH29,nowH29))break;haveH29=true;}
     if(!ne::fresh(pendingH29.tick,nowH29)){haveH29=false;continue;}
     // Wait for display publication of this ORIGINAL source, never re-date it.
     if(pendingH29.source.sourceSample>fH29.output.solverSourceSampleT4||
        pendingH29.source.sourceTick>fH29.output.solverSourceTickT4)break;
     const auto eventH29=pendingH29;haveH29=false;heldT4::ContactOwnerH24 liveH29{};heldT4::Achieved achievedH29{};
     kcd2vr::native_weapon_actor_join_h28::Contact joinedH29{};solverH28::Stamp proofH29{};bridge::Contact hitH29{};
     const auto& gestureH29=fH29.output.solverGestureH29;
     bool acceptedH29=false;const char* reasonH29="NO_COMMITTED_ACHIEVED_GESTURE";
     if(gestureH29.gesture){reasonH29="OCCURRENCE_BODY_OR_PART_JOIN";
      if(heldT4::ReadContactOwnerH24(liveH29)&&heldT4::Read(achievedH29)&&
        kcd2vr::native_weapon_actor_join_h28::join(WhBase(),eventH29,liveH29,WeaponParryFrameH24(fH29),
          gestureH29.onsetSample,nowH29,[](std::uintptr_t at,auto& value){return Read(reinterpret_cast<const void*>(at),value);},
          [](std::uint32_t id){return reinterpret_cast<std::uintptr_t>(EntityById6694(id));},joinedH29)){
       reasonH29="ORIGINAL_SOLVER_STAMP_OR_FORWARD_MOTION";
       if(solverH28::makeContact(eventH29.source,achievedH29,liveH29,heldT4::AdmissionSerial(),nowH29,proofH29)&&
          bridge::bind(joinedH29,proofH29,gestureH29,fH29.profiles[0],nowH29,hitH29)&&
          GeometrySolverOwnerValidH28(proofH29,hitH29.impact.key,hitH29.impact.generation,hitH29.impact.epoch,
            hitH29.impact.request,hitH29.impact.sample,hitH29.impact.gesture59c3)){
        const auto& r=hitH29.impact;const float pointH29[]{float(r.point.x),float(r.point.y),float(r.point.z)};
        const auto weightH29=std::clamp(fH29.profiles[0].weight,.3,6.0);
        const auto joulesH29=.5*weightH29*(r.kind==1?.9:.55)*r.speed*r.speed;
        combat_diag_probe::NoteImpactPower59c8(r.sample,float(joulesH29/double(g_strike_reference_j59c9.load())),false);
        acceptedH29=combat_diag_probe::GeometryHitSubmit178r27(r.entityId,pointH29,float(std::clamp(joulesH29/21.6,.05,1.0)),
            r.kind,r.part,-1,r.tick,r.key,r.generation,r.epoch,r.request,r.sample,r.gesture59c3,&hitH29.metadata);
        reasonH29=acceptedH29?"QUEUED_FOR_NATIVE_COST_AND_ADJUDICATION":"GAMEPLAY_QUEUE_REFUSED";
       }
      }
     }
     if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,nowH29)){
       char lineH29[460]{};std::snprintf(lineH29,sizeof(lineH29),
         "H29 F9_COMBAT occurrence=%llu gesture=%llu sourceSample=%llu impactTick=%llu origin=%d part=%d body=0x%llX queued=%d reason=%s nativeResult=UNCLAIMED",
         (unsigned long long)eventH29.serial,(unsigned long long)gestureH29.gesture,(unsigned long long)eventH29.source.sourceSample,
         (unsigned long long)eventH29.tick,gestureH29.origin,eventH29.otherPart,(unsigned long long)eventH29.otherBody,int(acceptedH29),reasonH29);
       diagnostic_trace::Write(lineH29);
     }
 }
}
