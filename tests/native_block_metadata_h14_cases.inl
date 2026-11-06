// Actual NativeObserve104/NativeRhythmTick178/NativeGuardTick112/input callers.
// The owned native block is a memory/native-leaf double, not an awarded block.
{
 const auto frameSavedBM14=frame;const auto commandSavedBM14=command;const auto stateSavedBM14=state;
 const auto captureSavedBM14=diagnostics::capture168::gate.mask.load();
 const bool experimentSavedBM14=g_combat_experiment175.load(),requireSavedBM14=g_attack_require_opponent59c10.load();
 const bool physicalSavedBM14=g_physical_combat177.load();
 const bool integratedSavedBM14=g_combat_integrated178r22c2.load();
 const bool keepSavedBM14=g_ms_keep_stance59c17.load();const int masterSavedBM14=mg47::mode.load();
 const auto readySavedBM14=g_phys_ready_sample6698.load();
 auto resetBM14=[&](bool owned=true){
  blockCallback176={};NativeGuardRelease112();PhysAbortAttack117(player.data());idle();
  g_phys_native6698.attackPrepared=false;g_guard_manual112=false;g_guard_manual_owner112=nullptr;
  g_combat_experiment175=true;g_physical_combat177=true;g_combat_integrated178r22c2=true;
  g_attack_require_opponent59c10=false;g_ms_keep_stance59c17=false;mg47::mode=0;
  g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=false;
  g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;animation_filter::nativeBodyOwned112=false;
  ++hostTickCount64;animation_filter::combatPoseTick112=hostTickCount64;
  frame=frameSavedBM14;frame.player=player.data();frame.manager=manager.data();
  frame.input.coherentMelee119=frame.input.naturalCombat117=frame.input.allowed=true;
  frame.input.tracked[0]=frame.input.tracked[1]=true;frame.input.positionGuard176=true;frame.input.attacks=true;
  frame.input.tick=hostTickCount64;++frame.input.sample;frame.request=frame.input.epoch=frame.input.bodyEpoch=1;
  frame.output.active[0]=frame.output.owned[0]=true;frame.output.handlingOnly55c1[0]=false;
  frame.output.guard117=frame.output.guardBySide30c1[0]=true;frame.output.guardBySide30c1[1]=false;
  frame.output.strokeActive119=false;frame.output.primaryStrokeGesture19=0;
  frame.output.weapons[0]={{0,0,0,1},{-.3,.4,.3}};frame.profiles[0].key=77;frame.profiles[0].generation=4;
  frame.profiles[0].primary=true;frame.profiles[0].side=0;
  frame.input.shoulders[0]=frame.input.shoulders[1]={};frame.input.grip[0]=1;
  command=commandSavedBM14;command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Slash;
  command.key=77;command.generation=4;command.side=0;command.bodyEpoch=command.epoch=1;
  command.zone117=command.originZone167=2;command.sample=frame.input.sample;command.tick=hostTickCount64;
  command.gesture59c3=frame.input.sample;command.held57=command.kick59=false;command.masterHandoff23=0;
  put(state.data()+0xCF8,static_cast<void*>(nullptr));put(state.data()+0x7D8,0);put(state.data()+0x778,0);
  put(state.data()+0x38,128);put(state.data()+0x218,-1);put(state.data()+0x248,-1);
  put(state.data()+0x278,7);put(state.data()+0x1B8,1);put(state.data()+0x2A8,std::uint8_t(0));
  put(state.data()+0x2D8,std::uint8_t(0));put(state.data()+0x308,std::uint8_t(0));
  put(hands.data()+0x260,std::uint64_t(77));put(actor.data()+0x210,state.data());put(state.data()+0xCE0,actor.data());
  NativeOwner104 ownerBM14{};(void)NativeResolve104(player.data(),ownerBM14);
  if(owned){g_native_guard112.owner=ownerBM14;g_native_guard112.key=77;g_native_guard112.generation=4;
   g_native_guard112.side=0;NativeGuardMarkHeldH14();g_native_guard112.lastHold=hostTickCount64;
   g_guard_physical_owner112=player.data();g_guard_physical112=true;}
  primaryLatch117=0;g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
 };
 for(int phaseBM14:{128,256})for(bool capturedBM14:{false,true}){
  resetBM14();diagnostics::capture168::gate.mask=capturedBM14?captureSavedBM14:0;
  put(state.data()+0x38,phaseBM14);
  NativeOwner104 currentBM14{};NativeResolve104(player.data(),currentBM14);nc104::Observed observedBM14{};nc104::ReadReceipt59 receiptBM14{};
  ck(NativeObserve104(currentBM14,observedBM14,capturedBM14?&receiptBM14:nullptr)&&
     observedBM14.zone==7&&!nc104::valid(observedBM14)&&!kcd2vr::native_admission147::guardWritable(observedBM14),
     "H14 actual reader distinguishes native block metadata7 from a writable attack sector with F3 off/on");
  const auto pressesBM14=blockPresses117,releasesBM14=blockReleases117;
  for(int nBM14=0;nBM14<3;++nBM14){++hostTickCount64;frame.input.tick=hostTickCount64;++frame.input.sample;
   animation_filter::combatPoseTick112=hostTickCount64;g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
   NativeGuardTick112();}
  ck(g_native_guard112.held&&blockPresses117==pressesBM14&&blockReleases117==releasesBM14,
     "H14 repeated metadata preparation retains only the existing guard without new LT press or release");
  NativeRhythmTick178();
  ck(g_native_state_ok178.load()&&!g_native_busy178.load(),
     "H14 actual rhythm publisher allows original input from exact currently held native block metadata");
  command.sample=frame.input.sample;command.tick=hostTickCount64;
  frame.output.guard117=frame.output.guardBySide30c1[0]=false;
  frame.output.strokeActive119=true;frame.output.primaryStrokeGesture19=command.gesture59c3;
  blockCallback176=[&](int modeBM14){if(modeBM14==2){put(state.data()+0x278,-1);put(state.data()+0x38,2);}};
  const auto beginsBM14=begins117,zonesBM14=zoneCalls,releasesBeforeBM14=blockReleases117;
  ck(NativePhysicalAction117(player.data(),command,frame,true)&&begins117==beginsBM14+1,
     "H14 actual forward prepare leaves guard posture and crosses native input once from the existing owned block");
  ck(blockReleases117==releasesBeforeBM14+1&&!g_native_guard112.held&&zoneCalls==zonesBM14+1&&lastZone==2,
     "H14 owned block handoff releases only its LT and sends an ordinary physical sector, never7");
  command.kind=ph::CommandKind::Slash;
  ck(NativePhysicalAction117(player.data(),command,frame,false),
     "H14 exact prepared owner keeps the original native release lifecycle after block cleanup");
 }
 for(int denyBM14=0;denyBM14<17;++denyBM14){resetBM14();
  switch(denyBM14){
   case 0:g_native_guard112.held=false;g_guard_physical112=false;break;
   case 1:g_native_guard112.owner.state=nullptr;break;
   case 2:++g_native_guard112.owner.epoch;break;
   case 3:g_guard_physical_owner112=nullptr;break;
   case 4:g_guard_manual_owner112=player.data();g_guard_manual112=true;break;
   case 5:command.tick=hostTickCount64-101;break;
   case 6:frame.input.tick=hostTickCount64-81;break;
   case 7:++g_native_guard112.key;break;
   case 8:++g_native_guard112.generation;break;
   case 9:frame.input.tracked[1]=false;break;
   case 10:frame.input.allowed=false;break;
   case 11:++command.sample;break;
   case 12:put(state.data()+0x278,6);break;
   case 13:put(state.data()+0x38,64);break;
   case 14:put(state.data()+0x218,1);break;
   case 15:put(state.data()+0x248,7);break;
   case 16:put(state.data()+0x2D8,std::uint8_t(1));break;
  }
  const auto beginsBM14=begins117,zonesBM14=zoneCalls,releasesBM14=blockReleases117;
  ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==beginsBM14&&zoneCalls==zonesBM14&&
     blockReleases117==releasesBM14,"H14 foreign, stale, changed, protected or unclassified block metadata cannot borrow input or direction writes");
  g_guard_manual112=false;g_guard_manual_owner112=nullptr;
 }
 for(int changedBM14=0;changedBM14<4;++changedBM14){resetBM14();
  blockCallback176=[&,changedBM14](int modeBM14){if(modeBM14!=2)return;
   if(changedBM14==0)put(state.data()+0x38,64);
   if(changedBM14==1)put(actor.data()+0x210,static_cast<void*>(nullptr));
   if(changedBM14==2)put(hands.data()+0x260,std::uint64_t(991));
   if(changedBM14==3)g_ctx_menu=true;
  };
  const auto beginsBM14=begins117;
  ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==beginsBM14,
     "H14 a native guard-release callback cannot dispatch under replaced item/state, protected reaction or menu ownership");
  put(actor.data()+0x210,state.data());put(hands.data()+0x260,std::uint64_t(77));g_ctx_menu=false;
 }
 resetBM14(false);const auto pressesBM14=blockPresses117;NativeGuardTick112();NativeRhythmTick178();
 ck(!g_native_guard112.held&&blockPresses117==pressesBM14&&g_native_busy178.load(),
    "H14 observable foreign block metadata cannot acquire LT or publish input readiness");
 resetBM14();g_guard_physical112=false;g_native_guard112.held=false;
 g_guard_release_tick59c7e=hostTickCount64;g_guard_release_state59c7e=state.data();NativeRhythmTick178();
 ck(g_native_busy178.load(),"H14 the old250ms release witness alone cannot authorize block-metadata input");
 auto queueForwardBM14=[&](ph::CommandKind family=ph::CommandKind::Prepare){
  resetBM14();frame.output.guard117=frame.output.guardBySide30c1[0]=false;
  frame.output.strokeActive119=true;frame.output.primaryStrokeGesture19=command.gesture59c3;
  frame.input.motionOwned59c5=false;g_phys_input6698=true;g_phys_abort6698=false;
  g_phys_commands6698.head=g_phys_commands6698.tail=0;command.kind=family;
  meta.metadataAt=hostTickCount64;PhysReadLive103(meta.last);
  g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;g_phys_commands6698.push(command);
 };
 for(auto familyBM14:{ph::CommandKind::Prepare,ph::CommandKind::Slash}){
  queueForwardBM14(familyBM14);const auto beginsBM14=begins117,releasesBM14=blockReleases117;
  // Leave metadata7 in place until a later native Update. This proves the
  // real ordered pump, rather than hiding a release race with synchronous cleanup.
  PhysicalNativeFrameTick6698();
  ck(g_phys_native6698.attackPrepared&&begins117==beginsBM14+1&&blockReleases117==releasesBM14+1&&
     !g_native_guard112.held&&!g_block_guard_frame_h14,
     "H14 actual Rhythm->GuardTick->consumer order preserves current LT until exact queued prepare/restore-commit consumes it once");
 }
 queueForwardBM14();command.zone117=3;g_phys_commands6698.head=g_phys_commands6698.tail=0;g_phys_commands6698.push(command);
 {const auto beginsBM14=begins117,releasesBM14=blockReleases117;PhysicalNativeFrameTick6698();
  ck(begins117==beginsBM14&&blockReleases117==releasesBM14+1&&!g_native_guard112.held&&!g_block_guard_frame_h14,
     "H14 actual consumer refusal releases the original deferred guard at the same native-frame end");}
 for(int noQueueBM14=0;noQueueBM14<3;++noQueueBM14){queueForwardBM14();
  g_phys_commands6698.head=g_phys_commands6698.tail=0;
  if(noQueueBM14==1){++command.gesture59c3;g_phys_commands6698.push(command);}
  if(noQueueBM14==2){command.tick=hostTickCount64-101;g_phys_commands6698.push(command);}
  const auto releasesBM14=blockReleases117;
  {NativeBlockGuardFrameH14 scopeBM14;NativeGuardTick112();ck(!scopeBM14.deferred,
      "H14 no queued gesture, wrong gesture and stale queued input cannot defer LT cleanup");}
  ck(!g_native_guard112.held&&blockReleases117==releasesBM14+1,"H14 absent exact queued input leaves no deferred guard owner");
 }
 queueForwardBM14();
 {const auto releasesBM14=blockReleases117;std::uint64_t replacementLeaseBM14=0;
  {NativeBlockGuardFrameH14 scopeBM14;NativeGuardTick112();ck(scopeBM14.deferred&&g_native_guard112.held,
      "H14 stack scope binds the original current held-input publication");
   NativeGuardRelease112("TEST_NATIVE_REPLACEMENT");NativeOwner104 ownerBM14{};NativeResolve104(player.data(),ownerBM14);
   g_native_guard112.owner=ownerBM14;g_native_guard112.key=77;g_native_guard112.generation=4;g_native_guard112.side=0;
   NativeGuardMarkHeldH14();replacementLeaseBM14=g_native_guard112.leaseH14;
   g_guard_physical_owner112=player.data();g_guard_physical112=true;
  }
  ck(g_native_guard112.held&&g_native_guard112.leaseH14==replacementLeaseBM14&&blockReleases117==releasesBM14+1,
     "H14 same actor/item replacement input has a new lease and is never released by old frame cleanup");
 }
 queueForwardBM14();
 {const auto beginsBM14=begins117,releasesBM14=blockReleases117;
  {NativeBlockGuardFrameH14 scopeBM14;NativeGuardTick112();ck(scopeBM14.deferred,"H14 busy-consumer fixture starts after actual exact guard deferral");
   std::atomic<bool> lockedBM14{false},leaveBM14{false};
   std::thread holderBM14([&](){std::lock_guard<std::recursive_mutex> holdBM14(g_phys_native_mutex6698);lockedBM14=true;
       while(!leaveBM14.load())std::this_thread::yield();});
   while(!lockedBM14.load())std::this_thread::yield();PhysNativeTickImpl6698();leaveBM14=true;holderBM14.join();
  }
  ck(!g_native_guard112.held&&begins117==beginsBM14&&blockReleases117==releasesBM14+1,
     "H14 lock-busy actual consumer cannot retain deferred LT beyond the same stack frame");
 }
 g_phys_commands6698.head=g_phys_commands6698.tail=0;
 blockCallback176={};NativeGuardRelease112();PhysAbortAttack117(player.data());state=stateSavedBM14;
 diagnostics::capture168::gate.mask=captureSavedBM14;g_combat_experiment175=experimentSavedBM14;
 g_physical_combat177=physicalSavedBM14;
 g_combat_integrated178r22c2=integratedSavedBM14;
 g_attack_require_opponent59c10=requireSavedBM14;g_ms_keep_stance59c17=keepSavedBM14;mg47::mode=masterSavedBM14;
 frame=frameSavedBM14;command=commandSavedBM14;g_phys_frame6698.publish(frame);g_phys_ready_sample6698=readySavedBM14;
}
