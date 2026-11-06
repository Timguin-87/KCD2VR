// C62 actual native transport, not a mirrored zone model. Native acknowledgment
// is a zone readback only; it is never called an accepted block/hit or AI result.
{
 const auto feintBindingH20=g_cut_feint_h20;
 for(const auto& pin:kCutFeintPinsH20)std::memcpy(image.data()+pin.rva,pin.bytes,pin.size);
 g_cut_feint_h20={base,&cutFeintH20};
 ck(NativeCutFeintContractH20(base),"H20 redirect contract accepts exact retail entry and notification pins");
 for(const auto& pin:kCutFeintPinsH20){image[pin.rva+pin.size-1]^=1;
  ck(!NativeCutFeintContractH20(base),"H20 changed native redirect pin refuses call");image[pin.rva+pin.size-1]^=1;}
 const auto saved62=command;
 const bool ex62=g_combat_experiment175.load(),integrated62=g_combat_integrated178r22c2.load();
 const bool sync62=g_synchronized59c10r9.load(),intent62=g_intent59c16.load();
 const bool forward62=frame.input.forwardOnly19,physical62=frame.input.physicalCombat177;
 const bool rawTracked62=frame.input.intentTracked[0],motionOwned62=frame.input.motionOwned59c5,pommel62=frame.input.pommelStrike69;
 const auto normalOwner62=frame.output.normalIntentH30[0];
 frame.input.forwardOnly19=frame.input.physicalCombat177=true;
 g_combat_experiment175=g_combat_integrated178r22c2=g_synchronized59c10r9=true;g_intent59c16=false;
 auto publish62=[&](){++hostTickCount64;++frame.input.sample;frame.input.tick=hostTickCount64;
  animation_filter::combatPoseTick112=hostTickCount64;meta.metadataAt=hostTickCount64;
  // This native-transport fixture substitutes a completed recognizer source,
  // not its authority checks. Current HYBRID requires an exact current proof;
  // DIRECT retains its separate existing ownership policy. Production kernel
  // emission is exercised by public_hybrid_h31_bridge/gesture_recovery_h30.
  ph::select_forward_motion36(frame.input,kcd2vr::motion59c4::selected.load(),CombatSynchronized59c10r9());
  frame.input.intentTracked[0]=frame.input.tracked[0];frame.output.normalIntentH30[0]={};
  const auto& p62=frame.profiles[0];
  if(ph::normal_hybrid_intent_h31(frame.input)&&frame.input.allowed&&frame.input.attacks&&
     frame.input.tracked[0]&&p62.primary&&p62.melee()&&!p62.shield176()&&
     ph::finite(frame.input.hands[0])&&ph::finite(frame.input.intentHands[0]))
   frame.output.normalIntentH30[0]={p62.key,p62.generation,frame.input.bodyEpoch,frame.input.epoch,frame.input.sample,frame.input.tick,true};
  PhysReadLive103(meta.last);g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
  command.sample=frame.input.sample;command.tick=hostTickCount64;
  // Actual PhysPublish6698 producer enqueue stamps this immutable owner.
  command.recoveryEpochH30=kcd2vr::contact176::recoveryEpoch55c1.load();};
 auto hybrid62=[&](){const auto count62=frame.output.commandCount;frame.output.commandCount=1;
  frame.output.commands[0]=command;PhysPublishHybrid54(frame);frame.output.commandCount=count62;};
 idle();PhysAbortAttack117(player.data());g_phys_native6698.attackPrepared=false;
 command.held57=command.cutFeint62=false;command.kind=ph::CommandKind::Prepare;
 command.stroke117=ph::CommandKind::Slash;command.zone117=2;command.gesture59c3=0xC6200;
 publish62();hybrid62();const auto begins62=begins117,ends62=commits117;
 const auto currentProof62=frame.output.normalIntentH30[0];
 for(bool missingProof62:{true,false}){
  frame.output.normalIntentH30[0]=currentProof62;
  if(missingProof62)frame.output.normalIntentH30[0]={};else ++frame.output.normalIntentH30[0].sample;
  const auto forbiddenActionsH31=actionCalls,forbiddenZonesH31=zoneCalls;
  g_phys_frame6698.publish(frame);g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
  ck(!g_phys_native6698.attackPrepared&&begins117==begins62&&actionCalls==forbiddenActionsH31&&zoneCalls==forbiddenZonesH31,
     "H31 C62 actual queue refuses absent or wrong-sample normal source proof before native preparation");
 }
 frame.output.normalIntentH30[0]=currentProof62;g_phys_frame6698.publish(frame);
 g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 const auto onset62=g_phys_native6698.prepareTickC10R18,prepareSample62=g_phys_native6698.prepareSample117;
 ck(g_phys_native6698.attackPrepared&&begins117==begins62+1,"C62 cut starts exactly one native preparation");
 publish62();command.kind=ph::CommandKind::Zone57;command.zone117=0;command.cutFeint62=true;
 const auto owner62=g_phys_native6698;
 for(int bad62=0;bad62<21;++bad62){auto c62=command;auto f62=frame;g_phys_native6698=owner62;
  if(bad62==0)++c62.gesture59c3;
  if(bad62==1)++c62.key;
  if(bad62==2)++c62.generation;
  if(bad62==3)++c62.bodyEpoch;
  if(bad62==4)++c62.epoch;
  if(bad62==5)c62.sample=prepareSample62-1;
  if(bad62==6)c62.sample=frame.input.sample+1;
  if(bad62==7)c62.tick=onset62+101;
  if(bad62==8)c62.tick=hostTickCount64-101;
  if(bad62==9)c62.cutFeint62=false;
  if(bad62==10)c62.zone117=2;
  if(bad62==11)g_phys_native6698.cutFeintUsed62=true;
  if(bad62==12){f62.output.handlingOnly55c1[0]=true;f62.output.normalIntentH30[0]={};} // display-only, no qualified source
  if(bad62==13)animation_filter::nativeBodyOwned112=true;
  if(bad62==14)g_right_valid=false;
  if(bad62==15)g_phys_native6698.attackPrepared=false;
  if(bad62==16)c62.held57=true;
  if(bad62==17)f62.input.forwardOnly19=false;
  if(bad62==18)f62.input.coherentMelee119=false;
  if(bad62==19)g_phys_native6698.heldPrepare57=true;
  if(bad62==20)g_synchronized59c10r9=false;
  const auto writes62=zoneCalls,inputs62=actionCalls,redirectsH20=cutFeintCallsH20;
  ck(!NativeZoneOnly57(player.data(),c62,f62)&&zoneCalls==writes62&&actionCalls==inputs62&&cutFeintCallsH20==redirectsH20,
     "C62 cut feint refuses foreign, stale, untagged, repeated or protected-owner input without writes");
  animation_filter::nativeBodyOwned112=false;g_right_valid=true;g_synchronized59c10r9=true;
 }
 // A soft checked-pose recovery with an independently current tracked source
 // is deliberately different from the no-proof display-only negative above.
 // It may refine the SAME native preparation, never invent contact or new input.
 g_phys_native6698=owner62;put(state.data()+0xF8,2);put(state.data()+0x188,2);put(state.data()+0xC8,3);
 auto softFeintH31=frame;softFeintH31.output.handlingOnly55c1[0]=true;
 const auto softInputsH31=actionCalls,softBeginsH31=begins117,softCommitsH31=commits117,softRedirectsH31=cutFeintCallsH20;
 ck(NativeZoneOnly57(player.data(),command,softFeintH31)&&g_phys_native6698.cutFeintUsed62&&
    cutFeintCallsH20==softRedirectsH31+1&&actionCalls==softInputsH31&&begins117==softBeginsH31&&commits117==softCommitsH31&&
    g_phys_native6698.prepareTickC10R18==onset62&&g_phys_native6698.prepareSample117==prepareSample62,
    "H31 qualified soft-recovery feint retains original native preparation/onset without new input edges");
 // Actual adapter/caller transaction: preserve the OLD zone and native guard,
 // consume before callbacks, and refuse post-call owner changes without retry.
 for(int badH20=0;badH20<6;++badH20){
  g_phys_native6698=owner62;put(state.data()+0xF8,2);put(state.data()+0x188,2);put(state.data()+0xC8,3);
  const auto redirectsH20=cutFeintCallsH20,writesH20=zoneCalls;
  if(badH20==0)zoneAck62=false;
  if(badH20==1)cutFeintCallbackH20=[&](){put(state.data()+0xF8,1);};
  if(badH20==2)cutFeintCallbackH20=[&](){put(hands.data()+0x260,std::uint64_t(999));};
  if(badH20==3)cutFeintCallbackH20=[&](){g_phys_native6698.attackPrepared=false;};
  if(badH20==4)cutFeintCallbackH20=[&](){++g_weapon_request_epoch6694;};
  if(badH20==5)cutFeintCallbackH20=[&](){put(actor.data()+0x210,static_cast<void*>(nullptr));};
  ck(!NativeZoneOnly57(player.data(),command,frame)&&cutFeintCallsH20==redirectsH20+1&&zoneCalls==writesH20&&
     g_phys_native6698.cutFeintUsed62,"H20 refusal/substitution/reentrant owner loss spends one redirect without fallback setter");
  ck(!NativeZoneOnly57(player.data(),command,frame)&&cutFeintCallsH20==redirectsH20+1,
     "H20 same failed refinement cannot replay across callbacks");
  zoneAck62=true;put(hands.data()+0x260,std::uint64_t(77));g_weapon_request_epoch6694=command.epoch;
  put(actor.data()+0x210,state.data());
 }
 g_phys_native6698=owner62;put(state.data()+0xF8,2);put(state.data()+0x188,2);
 bool recursiveDeniedH20=false;const auto recursionCallsH20=cutFeintCallsH20;
 cutFeintCallbackH20=[&](){recursiveDeniedH20=!NativeZoneOnly57(player.data(),command,frame);};
 ck(NativeZoneOnly57(player.data(),command,frame)&&recursiveDeniedH20&&cutFeintCallsH20==recursionCallsH20+1,
    "H20 synchronous notification cannot recursively replay the same feint");
 // Exercise the actual running ordinary-cut state, not only the idle fixture.
 g_phys_native6698=owner62;put(state.data()+0xF8,2);put(state.data()+0x188,2);put(state.data()+0xC8,3);
 put(state.data()+0x38,4);put(state.data()+0x218,0);
 const auto inputs62=actionCalls,writes62=zoneCalls,redirectsH20=cutFeintCallsH20;hybrid62();
 g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(g_phys_native6698.attackPrepared&&g_phys_native6698.cutFeintUsed62&&
    g_phys_native6698.zone117==0&&lastZone==0&&zoneCalls==writes62&&cutFeintCallsH20==redirectsH20+1&&
    cutFeintGuardH20==3&&cutFeintOldZoneH20==2&&actionCalls==inputs62&&
    g_phys_native6698.prepareTickC10R18==onset62&&g_phys_native6698.prepareSample117==prepareSample62,
    "H20 native redirect sees old zone and current guard before setter, without another input/cost edge or renewed onset");
 auto duplicate62=command;duplicate62.zone117=1;
 ck(!NativeZoneOnly57(player.data(),duplicate62,frame)&&actionCalls==inputs62&&zoneCalls==writes62&&cutFeintCallsH20==redirectsH20+1,
    "C62 second cut refinement cannot mutate or renew the same preparation");
 idle();
 publish62();command.kind=ph::CommandKind::Slash;hybrid62();
 g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(!g_phys_native6698.attackPrepared&&begins117==begins62+1&&commits117==ends62+1,
    "C62 refined cut releases original native input exactly once");
 namespace mo62=kcd2vr::motion59c4;
 const mo62::Key hybridKey62{command.gesture59c3,command.key,command.generation,command.bodyEpoch,command.epoch,
     kcd2vr::contact176::recoveryEpoch55c1.load(),reinterpret_cast<std::uintptr_t>(player.data())};
 mo62::Attempt hybridAttempt62{};
 ck(mo62::hybridAttempts54.claim(hybridKey62,command.tick,hostTickCount64,777,hybridAttempt62)==mo62::Claim::Ready&&
    hybridAttempt62.origin==0&&hybridAttempt62.begin==onset62,
    "C62 HYBRID physical fallback stores new travel zone and keeps original source onset");
 // DIRECT receives the same new cut sector through its existing Feint event;
 // stale original hand-origin metadata must not overwrite it at Commit.
 const auto selected62=mo62::selected.load();
 mo62::selected=true;mo62::attempts.clear();
 command.gesture59c3=0xC6201;command.kind=ph::CommandKind::Prepare;command.zone117=command.originZone167=2;
 command.cutFeint62=false;publish62();g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 const auto directOnset62=command.tick;
 command.kind=ph::CommandKind::Zone57;command.zone117=0;command.cutFeint62=true;
 publish62();g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 command.kind=ph::CommandKind::Slash;publish62();g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 const mo62::Key key62{command.gesture59c3,command.key,command.generation,command.bodyEpoch,command.epoch,
     kcd2vr::contact176::recoveryEpoch55c1.load(),reinterpret_cast<std::uintptr_t>(player.data())};
 mo62::Attempt attempt62{};
 ck(mo62::attempts.claim(key62,command.tick,hostTickCount64,777,attempt62)==mo62::Claim::Ready&&
    attempt62.origin==0&&attempt62.begin==directOnset62&&attempt62.commit==command.tick&&attempt62.technique==0,
    "C62 DIRECT actual queue preserves refined cut zone, original onset and once-only native-rule attempt");
 ck(mo62::attempts.claim(key62,command.tick,hostTickCount64,777,attempt62)==mo62::Claim::Duplicate,
    "C62 cut feint cannot create a second contact attempt");
 mo62::attempts.clear();mo62::selected=selected62;
 // A native zone call without exact readback is not a successful feint.
 // The actual physical publication and actual queue share hard cancellation,
 // so later commit cannot restore an orphan input or apply fallback contact.
 command.gesture59c3=0xC6202;command.kind=ph::CommandKind::Prepare;command.zone117=2;command.cutFeint62=false;
 publish62();hybrid62();g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 const auto failBegins62=begins117,failEnds62=commits117;const auto failCount62=g_zone_feints57.load();
 const auto releaseSerial62=kcd2vr::contact176::nextSerial.load();
 command.kind=ph::CommandKind::Zone57;command.zone117=0;command.cutFeint62=true;zoneAck62=false;
 publish62();hybrid62();g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();zoneAck62=true;
 ck(!g_phys_native6698.attackPrepared&&g_zone_feints57.load()==failCount62,
    "C62 missing native zone ACK does not count as successful refinement and cancels held input");
 command.kind=ph::CommandKind::Slash;publish62();hybrid62();g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 const mo62::Key failedKey62{command.gesture59c3,command.key,command.generation,command.bodyEpoch,command.epoch,
     kcd2vr::contact176::recoveryEpoch55c1.load(),reinterpret_cast<std::uintptr_t>(player.data())};
 const auto failedClaimH20=mo62::hybridAttempts54.claim(failedKey62,command.tick,hostTickCount64,777,hybridAttempt62);
 ck(begins117==failBegins62&&commits117==failEnds62&&!g_phys_native6698.attackPrepared&&
    kcd2vr::contact176::nextSerial.load()==releaseSerial62&&
    failedClaimH20==mo62::Claim::Cancelled,
    "C62 refused feint cannot restore old/new native input, mint release, or reuse physical hit authority");
 // A native notification may synchronously replace the original preparation.
 // Its failure path must not cancel the newer source in the actual queue.
 command.gesture59c3=0xC6209;command.kind=ph::CommandKind::Prepare;command.zone117=2;command.cutFeint62=false;
 publish62();hybrid62();g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 const auto replacementOwnerH20=g_phys_native6698;
 command.kind=ph::CommandKind::Zone57;command.zone117=0;command.cutFeint62=true;publish62();
 cutFeintCallbackH20=[&](){++g_phys_native6698.prepareGestureC10R18;++g_phys_native6698.prepareSample117;
  ++g_phys_native6698.prepareTickC10R18;g_phys_native6698.cutFeintUsed62=false;};
 g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(g_phys_native6698.attackPrepared&&g_phys_native6698.prepareGestureC10R18==replacementOwnerH20.prepareGestureC10R18+1&&
    !g_phys_native6698.cutFeintUsed62,"H20 native feint refusal cannot cancel a newer synchronous preparation");
 PhysCancelAttack117();idle();

 // H26 actual original begin observer -> retained held proof -> native redirect.
 const auto beginLeafH26=g_input_begin117;const auto realBeginH26=realBeginR18;
 g_input_begin117=&BeginLeafR18;realBeginR18=true;reenterBeginR18=false;
 command=saved62;command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Slash;
 command.held57=command.controlledHoldH26=true;command.cutFeint62=false;command.zone117=2;command.gesture59c3=0x260001;
 primaryLatch117=0;g_phys_native6698.attackPrepared=false;idle();
 publish62();hybrid62();g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(g_phys_native6698.attackPrepared&&g_phys_native6698.heldPrepare57&&g_prepared_input_r18.controlledHoldH26&&g_prepared_input_r18.source,
    "H26 controlled hold retains ACTUAL stable original native HEAVY preparation source");
 hostTickCount64+=400;publish62();command.kind=ph::CommandKind::Zone57;command.zone117=0;command.cutFeint62=true;
 put(state.data()+0x188,2);put(state.data()+0xF8,2);put(state.data()+0xC8,3);
 const auto heldOwnerH26=g_phys_native6698;const auto redirectsH26=cutFeintCallsH20,settersH26=zoneCalls,inputsH26=actionCalls;
 g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(g_phys_native6698.attackPrepared&&g_phys_native6698.zone117==command.zone117&&cutFeintCallsH20==redirectsH26+1&&zoneCalls==settersH26&&actionCalls==inputsH26,
    "H26 actual queue held feint crosses original936A28 exactly once without setter or new input");
 ck(!NativeZoneOnly57(player.data(),command,frame)&&cutFeintCallsH20==redirectsH26+1,
    "H26 held source one-feint permission cannot replay");
 const auto commitsHeldH26=commits117;
 command.kind=command.stroke117=ph::CommandKind::Slash;command.cutFeint62=false;publish62();hybrid62();
 g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(!g_phys_native6698.attackPrepared&&commits117==commitsHeldH26+1&&releaseInput117==18,
    "H26 actual qualified stroke releases SAME native held HEAVY input exactly once");
 command.kind=ph::CommandKind::Zone57;command.cutFeint62=true;
 g_phys_native6698=heldOwnerH26;g_phys_native6698.cutFeintUsed62=false;
 const auto heldProofH26=g_prepared_input_r18;g_prepared_input_r18={};
 ck(!NativeZoneOnly57(player.data(),command,frame)&&cutFeintCallsH20==redirectsH26+1,
    "H26 unobserved asynchronous XI held source cannot redirect");
 g_prepared_input_r18=heldProofH26;cutFeintCallbackH20=[&](){NativeInvalidateReleasedInputH14();put(state.data()+0xF8,command.zone117);};
 put(state.data()+0x188,2);put(state.data()+0xF8,2);
 ck(!NativeZoneOnly57(player.data(),command,frame),"H26 native redirect callback source replacement cannot retain held authority");
 cutFeintCallbackH20={};PhysCancelAttack117();idle();
 g_input_begin117=beginLeafH26;realBeginR18=realBeginH26;
 command=saved62;g_combat_experiment175=ex62;g_combat_integrated178r22c2=integrated62;
 g_synchronized59c10r9=sync62;g_intent59c16=intent62;frame.input.forwardOnly19=forward62;
 frame.input.physicalCombat177=physical62;frame.input.intentTracked[0]=rawTracked62;
 frame.input.motionOwned59c5=motionOwned62;frame.input.pommelStrike69=pommel62;frame.output.normalIntentH30[0]=normalOwner62;idle();
 g_cut_feint_h20=feintBindingH20;put(state.data()+0xC8,0);
}
