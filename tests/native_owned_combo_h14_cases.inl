// Execute the full production NativePhysicalAction117 and the REAL typed
// ReadNativeComboSlotF1, with copied native memory and counted input leaves.
// No helper returning "eligible" is mocked; no accepted hit/recipe is claimed.
{
 namespace cg14=kcd2vr::combo_lineage19;
 const auto savedFrameC14=frame;const auto savedCommandC14=command;const auto savedNativeC14=g_phys_native6698;
 const auto savedPreparedC14=g_input_prepared_owner117;
 const auto savedReleasedC14=g_released_input_h14;const auto savedSourceGenerationC14=g_input_source_generation_h14;
 const auto savedSyncC14=g_synchronized59c10r9.load(),savedIntegratedC14=g_combat_integrated178r22c2.load();
 const auto savedExperimentC14=g_combat_experiment175.load(),savedPhysicalC14=g_physical_combat177.load();
 const auto savedReadyC14=g_motion_clock_ready50.load(),savedFaultC14=g_motion_clock_fault50.load();
 const auto savedClockEnabledC14=ac50::enabled.load(),savedDirectC14=kcd2vr::motion59c4::selected.load();
 const auto savedClockActorC14=g_motion_clock_actor50.load();const bool savedThrustC14=g_thrust_as_swing55.load();
 const bool savedMenuC14=g_ctx_menu.load(),savedDialogueC14=g_ctx_dialogue.load(),savedCutsceneC14=g_ctx_cutscene.load(),savedScriptedC14=g_ctx_scripted.load();
 const bool savedGameplayC14=g_gameplay_allowed.load(),savedHeadC14=g_head_valid.load(),savedRightC14=g_right_valid.load(),savedLeftC14=g_left_valid.load();
 const bool savedNativeBodyC14=animation_filter::nativeBodyOwned112.load();
 const auto savedTimingContractC14=g_attack_timing_contract22.load(),savedTimingPolicyC14=g_attack_timing_policy22.load();
 const auto savedBodyC14=g_pose_epoch6689.load(),savedRequestC14=g_weapon_request_epoch6694.load();
 const auto savedRecoveryC14=kcd2vr::contact176::recoveryEpoch55c1.load(),savedReadySampleC14=g_phys_ready_sample6698.load();
 const auto savedCaptureC14=diagnostics::capture168::gate.mask.load();const auto savedClaimedC14=kcd2vr::contact176::claimedSerial.load();
 const auto savedBypassC14=g_combat_bypass101.load();const auto savedInputOriginalC14=g_combat_input_original101;
 const auto savedAdmissionC14=g_admission_gate51;const bool savedRequireC14=g_attack_require_opponent59c10.load();
 const bool savedDiagSyncC14=combat_diag_probe::Synchronized59c10r9(),savedDiagIntentC14=combat_diag_probe::Intent59c16();
 const bool savedDiagLiveC14=combat_diag_probe::GeometryLiveAction178r35();
 ac50::Binding savedBindingC14{};(void)g_motion_clock_binding50.read(savedBindingC14);
 TrackingSnapshot savedTrackingC14{};(void)ReadTrackingSnapshot(savedTrackingC14);
 std::array<unsigned char,0x500> savedActorC14=actor;std::array<unsigned char,0xE00> savedStateC14=state;
 std::array<unsigned char,0x100> actionManagerC14{},helperC14{},clockC14{},typeC14{},gateC14{},comboManagerC14{};
 std::array<unsigned char,0xB0> actionC14{};std::array<unsigned char,0x150> rowC14{};
 std::array<unsigned char,0x500> targetC14{};std::array<unsigned char,0xE00> targetStateC14{};std::array<unsigned char,0x100> targetEntityC14{};
 void* mainSlotC14=actionC14.data();ac50::Binding bindingC14{};kcd2vr::contact176::Ticket releaseC14{};
 cg14::Owner leaseOwnerC14{};unsigned serialC14=120000;
 auto observeReleaseC14=[&](bool retainedC14=true,bool nestedC14=false,bool returnsLaterH19=false){
  const auto savedReturnC14=hostReturnAddressValue;const auto savedLeafC14=g_input_release117,savedBeginC14=g_input_begin117;
  auto* savedScopeC14=g_input_scope117;NativeInputScope117 releasedScopeC14{};NativeResolve104(player.data(),releasedScopeC14.owner);
  releasedScopeC14.stroke=ph::CommandKind::Slash;releasedScopeC14.prepare=false;
  releasedScopeC14.costInput54={leaseOwnerC14.key,leaseOwnerC14.actor,leaseOwnerC14.state,releaseC14.tick};
  g_input_scope117=&releasedScopeC14;hostReturnAddressValue=reinterpret_cast<void*>(base+0x27C0916);
  g_input_release117=returnsLaterH19?+[](void* self,int input){ComboReleaseLeafH14(self,input);++hostTickCount64;}:&ComboReleaseLeafH14;g_input_begin117=nullptr;
  releaseRetainsPendingH14=retainedC14;releaseNestedForeignH14=nestedC14;
  // The native original must install these values. Capturing its preimage
  // instead of its returned state would make the positive caller case fail.
  put(state.data()+0x2A8,std::uint8_t(0));put(state.data()+0x248,-1);
  NativeInputRelease117(control117.data(),18);
  g_input_release117=savedLeafC14;g_input_begin117=savedBeginC14;g_input_scope117=savedScopeC14;hostReturnAddressValue=savedReturnC14;
 };
 auto resetC14=[&](unsigned windowH19=1500u){
  NativeGuardRelease112();PhysAbortAttack117(player.data());idle();primaryLatch117=0;g_phys_native6698.attackPrepared=false;
  g_synchronized59c10r9=g_combat_integrated178r22c2=g_combat_experiment175=g_physical_combat177=true;
  g_motion_clock_ready50=ac50::enabled=true;g_motion_clock_fault50=false;kcd2vr::motion59c4::selected=false;
  combat_diag_probe::SetSynchronized59c10r9(true);combat_diag_probe::SetIntent59c16(false);combat_diag_probe::SetGeometryLiveAction178r35(true);
  g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=false;g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;
  animation_filter::nativeBodyOwned112=false;g_attack_timing_contract22=base;g_attack_timing_policy22=31;
  g_pose_epoch6689=g_weapon_request_epoch6694=1;kcd2vr::contact176::recoveryEpoch55c1=61;
  g_attack_require_opponent59c10=false;g_thrust_as_swing55=true;
  ++hostTickCount64;++serialC14;animation_filter::combatPoseTick112=hostTickCount64;
  frame=savedFrameC14;frame.player=player.data();frame.manager=manager.data();frame.request=frame.input.bodyEpoch=frame.input.epoch=1;
  frame.input.sample=serialC14+100;frame.input.tick=hostTickCount64;
  frame.input.allowed=frame.input.attacks=frame.input.coherentMelee119=frame.input.naturalCombat117=frame.input.physicalCombat177=true;
  frame.input.tracked[0]=frame.input.tracked[1]=true;frame.input.requirePrimaryGrip114=true;frame.input.grip[0]=1;
  frame.output.active[0]=frame.output.owned[0]=true;frame.output.handlingOnly55c1[0]=frame.output.handlingOnly55c1[1]=false;
  frame.output.solverOwnedT4[0]=frame.output.ownShieldBlocked60[0]=false;frame.output.invalidMask55c1[0]=0;
  frame.output.guard117=false;frame.output.guardBySide30c1[0]=frame.output.guardBySide30c1[1]=false;
  frame.profiles[0]=savedFrameC14.profiles[0];frame.profiles[0].primary=true;
  g_phys_ready_sample6698=frame.input.sample;g_phys_frame6698.publish(frame);
  TrackingSnapshot trackingC14{};trackingC14.allowed=trackingC14.head_valid=trackingC14.right_valid=trackingC14.left_valid=true;
  trackingC14.tick=trackingC14.head_good_tick=trackingC14.right_good_tick=trackingC14.left_good_tick=hostTickCount64;
  trackingC14.sequence=serialC14;g_tracking_frames6695.publish(trackingC14);
  command=savedCommandC14;command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Slash;
  command.zone117=2;command.originZone167=2;command.direction={2,0,0};command.key=frame.profiles[0].key;
  command.generation=frame.profiles[0].generation;command.epoch=command.bodyEpoch=1;command.side=0;
  command.sample=frame.input.sample;command.tick=hostTickCount64;command.gesture59c3=serialC14+1000;
  command.kick59=command.held57=false;command.masterHandoff23=0;
  bindingC14={};auto& inC14=bindingC14.intent;
  inC14.player=reinterpret_cast<std::uintptr_t>(player.data());inC14.actor=reinterpret_cast<std::uintptr_t>(actor.data());
  inC14.state=reinterpret_cast<std::uintptr_t>(state.data());inC14.manager=reinterpret_cast<std::uintptr_t>(manager.data());
  inC14.serial=serialC14+2000;inC14.policy=31;inC14.epoch=inC14.request=1;inC14.sample=command.sample-1;inC14.tick=hostTickCount64-1;
  inC14.key=command.key;inC14.generation=command.generation;inC14.kind=at22::Kind::Slash;inC14.side=0;
  inC14.speed=4;inC14.gesture59c3=serialC14;inC14.recovery54=61;
  bindingC14.action=reinterpret_cast<std::uintptr_t>(actionC14.data());bindingC14.helper=reinterpret_cast<std::uintptr_t>(helperC14.data());
  bindingC14.clock=reinterpret_cast<std::uintptr_t>(clockC14.data());bindingC14.manager=reinterpret_cast<std::uintptr_t>(actionManagerC14.data());
  bindingC14.row=reinterpret_cast<std::uintptr_t>(rowC14.data());bindingC14.entity=reinterpret_cast<std::uintptr_t>(entity.data());
  g_motion_clock_actor50=inC14.actor;g_motion_clock_binding50.publish(bindingC14);
  releaseC14={};releaseC14.player=inC14.player;releaseC14.entity=bindingC14.entity;releaseC14.serial=serialC14;
  releaseC14.tick=inC14.tick;releaseC14.sample=inC14.sample;releaseC14.epoch=releaseC14.request=1;
  releaseC14.key=inC14.key;releaseC14.generation=inC14.generation;releaseC14.policy=31;
  releaseC14.gesture59c3=inC14.gesture59c3;releaseC14.recovery59c3=61;kcd2vr::contact59c3::releases.publish(releaseC14);
  put(actor.data()+0x200,actionManagerC14.data());put(actor.data()+0x3C0,clockC14.data());put(clockC14.data()+8,actor.data());
  put(actionManagerC14.data()+0x38,&mainSlotC14);mainSlotC14=actionC14.data();
  put(actionC14.data(),base+0x3A61C58);put(actionC14.data()+0x3C,0);put(actionC14.data()+0x40,actionManagerC14.data());
  put(actionC14.data()+0x28,std::uint8_t(1));put(actionC14.data()+0x70,actor.data());put(actionC14.data()+0xA0,helperC14.data());
  put(actionC14.data()+0x58,rowC14.data());put(helperC14.data(),base+0x3A61040);put(helperC14.data()+8,actor.data());
  put(helperC14.data()+0x20,actionC14.data()+0x88);put(actionC14.data()+0x88,base+0x3A61440);put(clockC14.data()+0x14,std::uint8_t(1));
  put(rowC14.data()+0x148,typeC14.data());put(typeC14.data()+8,0);put(actor.data()+0x220,gateC14.data());put(gateC14.data()+0x28,std::uint8_t(0));
  put(actor.data()+0x3B0,comboManagerC14.data());put(comboManagerC14.data()+8,actor.data());
  put(state.data()+0x38,16);put(state.data()+0x218,1);put(state.data()+0x248,18);put(state.data()+0x1B8,0);
  put(state.data()+0x2A8,std::uint8_t(1));put(state.data()+0x868,std::uint8_t(1));put(state.data()+0xCF8,targetC14.data());
  put(targetC14.data(),base+0x3E3FAC8);put(targetC14.data()+0x210,targetStateC14.data());put(targetC14.data()+0x1F8,targetEntityC14.data());
  put(targetStateC14.data()+0xCE0,targetC14.data());put(targetStateC14.data()+0xCF8,actor.data());
  put(targetEntityC14.data(),base+0x4072C78);put(targetEntityC14.data()+0xC,std::uint32_t(1993));
  cg14::grace.clear();cg14::nativeSlot.retire();leaseOwnerC14={};
  leaseOwnerC14.key={releaseC14.gesture59c3,command.key,command.generation,1,1,61,inC14.player};
  leaseOwnerC14.actor=inC14.actor;leaseOwnerC14.state=inC14.state;leaseOwnerC14.manager=reinterpret_cast<std::uintptr_t>(comboManagerC14.data());
  leaseOwnerC14.target=reinterpret_cast<std::uintptr_t>(targetC14.data());leaseOwnerC14.targetEntity=reinterpret_cast<std::uintptr_t>(targetEntityC14.data());
  leaseOwnerC14.targetId=1993;leaseOwnerC14.action=bindingC14.action;leaseOwnerC14.release=releaseC14.serial;
  leaseOwnerC14.modeGeneration=cg14::grace.generation();leaseOwnerC14.mode=9;leaseOwnerC14.tick=hostTickCount64;
  ck(cg14::nativeSlot.consume(leaseOwnerC14,windowH19),"H14 fixture publishes one finite lease from an observed native-open transition");
  g_phys_native6698.player=player.data();g_phys_native6698.epoch=g_phys_native6698.request=1;
  g_phys_native6698.natural117=true;g_phys_native6698.stroke117=ph::CommandKind::Slash;
  g_phys_native6698.attackKey=command.key;g_phys_native6698.attackGeneration117=command.generation;
  g_phys_native6698.prepareGestureC10R18=releaseC14.gesture59c3;g_phys_native6698.prepareSample117=releaseC14.sample;
  NativeResolve104(player.data(),g_input_prepared_owner117);
  observeReleaseC14();
  g_combat_bypass101=&ComboAdmissionActionH14;g_combat_input_original101=&ComboAdmissionActionH14;g_admission_gate51=&AdmissionLeafH12;
  comboGateCallsH14=admissionCallsH12=0;comboGateObservedH14=false;admissionReturnH12=false;
 };
 resetC14();observeReleaseC14();
 ck(!releaseSawPriorSourceH14&&g_released_input_h14.key.gesture==releaseC14.gesture59c3,
    "H14 repeated original release sees no prior source proof while nested native code runs, and republishes only after returning");
 kcd2vr::subsystems::masterstrike_combos::NativeSlotF1 observedC14{};
 ck(ReadNativeComboSlotF1(actor.data(),observedC14)&&observedC14.release.serial==releaseC14.serial,
    "H14 actual typed reader proves the same admitted/released ordinary main action before the next input");
 cg14::SlotLeaseF1 leaseBeforeC14{};std::uint64_t consumedBeforeC14=0;cg14::nativeSlot.read(leaseBeforeC14,consumedBeforeC14);
 const auto beginsBeforeC14=begins117;const auto claimedBeforeC14=kcd2vr::contact176::claimedSerial.load();
 ck(NativePhysicalAction117(player.data(),command,frame,true)&&begins117==beginsBeforeC14+1&&comboGateCallsH14==1&&admissionCallsH12==1,
    "H14 same-owned native-open combo continuation reaches original input and native arbiter exactly once");
 cg14::SlotLeaseF1 leaseAfterC14{};std::uint64_t consumedAfterC14=0;cg14::nativeSlot.read(leaseAfterC14,consumedAfterC14);
 ck(consumedAfterC14==consumedBeforeC14&&leaseAfterC14.owner.tick==leaseBeforeC14.owner.tick&&
    !comboGateObservedH14&&kcd2vr::contact176::claimedSerial==claimedBeforeC14,
    "H14 native false result remains false; input validation neither refreshes/consumes the slot nor claims damage");
 // H19: the native slot already opened and received a finite3000ms lease.
 // Only current frame/tracking/NEW swing advance; never retimestamp the old
 // binding, release source, release ticket or native opening lease.
 auto currentAfterH19=[&](unsigned delay){
  hostTickCount64+=delay;animation_filter::combatPoseTick112=hostTickCount64;
  frame.input.tick=command.tick=hostTickCount64;command.sample=++frame.input.sample;
  g_phys_ready_sample6698=frame.input.sample;g_phys_frame6698.publish(frame);
  TrackingSnapshot fresh{};fresh.allowed=fresh.head_valid=fresh.right_valid=fresh.left_valid=true;fresh.tick=fresh.head_good_tick=fresh.right_good_tick=fresh.left_good_tick=hostTickCount64;
  ++fresh.sequence;g_tracking_frames6695.publish(fresh);
 };
 resetC14(3000);currentAfterH19(1600);
 ck(!ReadNativeComboSlotF1(actor.data(),observedC14),"H19 initial native-slot opening reader remains capped at fresh1500ms");
 cg14::SlotLeaseF1 retainedH19{};std::uint64_t spentH19=0;cg14::nativeSlot.read(retainedH19,spentH19);
 const auto priorBeginsH19=begins117;
 ck(NativePhysicalAction117(player.data(),command,frame,true)&&begins117==priorBeginsH19+1&&comboGateCallsH14==1,
    "H19 exact existing native slot permits current follow-up input after1500ms within its original3000ms lease");
 cg14::SlotLeaseF1 afterH19{};std::uint64_t spentAfterH19=0;cg14::nativeSlot.read(afterH19,spentAfterH19);
 ck(afterH19.owner.tick==retainedH19.owner.tick&&afterH19.window==retainedH19.window&&spentAfterH19==spentH19,
    "H19 continued original input cannot rearm or refresh native opening/lease/release consumption");
 resetC14();put(state.data()+0x38,int(32));
 const auto withdrawalBeginsH32=begins117;
 ck(NativePhysicalAction117(player.data(),command,frame,true)&&begins117==withdrawalBeginsH32+1&&comboGateCallsH14==1&&admissionCallsH12==1&&!comboGateObservedH14,
    "H32 same-action open-slot withdrawal reaches original combo input without a new window or phase write");
 resetC14(3000);const auto openedBeforeReturnH19=leaseOwnerC14.tick;observeReleaseC14(true,false,true);
 ck(g_released_input_h14.tick==openedBeforeReturnH19+1,"H19 actual native release source timestamp may follow synchronous slot opening by1ms");
 currentAfterH19(1600);const auto delayedReturnBeginsH19=begins117;
 ck(NativePhysicalAction117(player.data(),command,frame,true)&&begins117==delayedReturnBeginsH19+1,
    "H19 synchronous opening inside original release still continues the same finite lease after1500ms");
 resetC14(3000);currentAfterH19(1600);g_released_input_h14.tick=leaseOwnerC14.tick+1501;const auto unrelatedLateSourceH19=begins117;
 ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==unrelatedLateSourceH19,
    "H19 arbitrarily late same-key release cannot borrow an old native opening grant");
 resetC14(3000);currentAfterH19(3001);const auto lateH19=begins117;
 ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==lateH19,"H19 expired native lease cannot continue even with a live-looking slot byte");
 resetC14(3000);currentAfterH19(1600);cg14::nativeSlot.retire();const auto retiredH19=begins117;
 ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==retiredH19,"H19 retired original slot lease cannot be reconstructed from stale release/binding");
 resetC14(3000);currentAfterH19(1600);mainSlotC14=nullptr;const auto replacedH19=begins117;
 ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==replacedH19,"H19 long native continuation still requires current manager membership before action dereference");
 resetC14(3000);currentAfterH19(1600);put(targetEntityC14.data()+0xC,std::uint32_t(1994));const auto targetH19=begins117;
 ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==targetH19,"H19 long native continuation cannot change the original target identity");
 for(int denyC14=0;denyC14<34;++denyC14){resetC14();
  switch(denyC14){
   case 0:put(state.data()+0x868,std::uint8_t(0));break;
   case 1:put(state.data()+0x868,std::uint8_t(2));break;
   case 2:mainSlotC14=nullptr;break;
   case 3:bindingC14.action+=8;g_motion_clock_binding50.publish(bindingC14);break;
   case 4:g_motion_clock_actor50=0;break;
   case 5:g_input_prepared_owner117={};break;
   case 6:++g_phys_native6698.prepareGestureC10R18;break;
   case 7:put(state.data()+0x248,19);break;
   case 8:command.gesture59c3=releaseC14.gesture59c3;break;
   case 9:command.sample=releaseC14.sample;break;
   case 10:++bindingC14.intent.recovery54;g_motion_clock_binding50.publish(bindingC14);break;
   case 11:++bindingC14.intent.policy;g_motion_clock_binding50.publish(bindingC14);break;
   case 12:put(targetEntityC14.data()+0xC,std::uint32_t(1994));break;
   case 13:put(targetStateC14.data()+0xCF8,static_cast<void*>(nullptr));break;
   case 14:cg14::grace.clear();break;
   case 15:cg14::nativeSlot.retire();break;
   case 16:combat_diag_probe::SetIntent59c16(true);break;
   case 17:++bindingC14.intent.generation;g_motion_clock_binding50.publish(bindingC14);break;
   case 18:put(state.data()+0x38,8);break;
   case 19:command.kick59=true;break;
   case 20:command.held57=true;break;
   case 21:g_phys_native6698.natural117=false;break;
   case 22:put(gateC14.data()+0x28,std::uint8_t(1));break;
   case 23:bindingC14.intent.tick=hostTickCount64-1501;g_motion_clock_binding50.publish(bindingC14);break;
   case 24:++command.generation;break;
   case 25:command.tick=hostTickCount64-101;break;
   case 26:put(state.data()+0x1B8,1);break;
   case 27:NativeInvalidateReleasedInputH14();break;
   case 28:observeReleaseC14(false);put(state.data()+0x2A8,std::uint8_t(1));break;
   case 29:observeReleaseC14(true,true);break;
   case 30:{auto* oldScopeC14=g_input_scope117;g_input_scope117=nullptr;
     NativeInputBegin117(control117.data(),18);g_input_scope117=oldScopeC14;break;}
   case 31:{auto* oldScopeC14=g_input_scope117;g_input_scope117=nullptr;
     NativeInputRelease117(control117.data(),18);g_input_scope117=oldScopeC14;break;}
   case 32:++g_released_input_h14.owner.epoch;break;
   case 33:g_released_input_h14.owner.entity=nullptr;break;
  }
  const auto beginsC14=begins117;
  ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==beginsC14&&comboGateCallsH14==0,
     "H14 foreign pending request stays closed for stale source/action/slot/target/epochs/mode or non-ordinary input");
 }
 for(int callbackC14=0;callbackC14<5;++callbackC14){resetC14();NativeOwner104 guardOwnerC14{};NativeResolve104(player.data(),guardOwnerC14);
  g_native_guard112.owner=guardOwnerC14;g_native_guard112.held=true;g_native_guard112.key=command.key;
  g_native_guard112.generation=command.generation;g_guard_physical_owner112=player.data();g_guard_physical112=true;
  blockCallback176=[&,callbackC14](int modeC14){if(modeC14!=2)return;
   if(callbackC14==0)put(state.data()+0x868,std::uint8_t(0));
   if(callbackC14==1)mainSlotC14=nullptr;
   if(callbackC14==2)put(targetEntityC14.data()+0xC,std::uint32_t(788));
   if(callbackC14==3)cg14::grace.clear();};
  if(callbackC14==4)blockCallback176=[&](int modeC14){if(modeC14==2){auto* savedScopeC14=g_input_scope117;
    g_input_scope117=nullptr;NativeInputBegin117(control117.data(),18);g_input_scope117=savedScopeC14;}};
  const auto beginsC14=begins117;
  ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==beginsC14&&comboGateCallsH14==0,
     "H14 native callback changing slot/action/target/ownership cannot replay the pre-release continuation permission");
 }
 resetC14();g_ctx_menu=true;
 {const auto beginsC14=begins117;ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==beginsC14,
     "H14 menu ownership cancels same-owned combo continuation before native input");}g_ctx_menu=false;
 resetC14();g_left_valid=false;
 {const auto beginsC14=begins117;ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==beginsC14,
     "H14 tracked two-hand owner is required for the existing native combo reader");}g_left_valid=true;
 // Actual direct fallback post-native-call seam: proving damage alone must
 // not mark an unrelated action as hit. The native admitted-release reader
 // remains the full production frik function, with its phase4/8 root checks.
 std::array<ac50::Record,3> clockRowsC14{};std::array<unsigned char,16> receiverC14{};
 auto directSetupC14=[&](){resetC14();put(state.data()+0x38,8);put(state.data()+0x488,std::uint8_t(0));
  for(unsigned nC14=0;nC14<3;++nC14)clockRowsC14[nC14]={nC14,1,1,.5f,.5f};
  put(clockC14.data()+0x58,clockRowsC14.data());put(clockC14.data()+0x60,clockRowsC14.data()+3);
  put(clockC14.data()+0x10,1.f);put(clockC14.data()+0x50,0.f);put(clockC14.data()+0x54,0.f);
  put(helperC14.data()+0x18,receiverC14.data());directComboH14::g_enabled=directComboH14::g_sync59c10r9=directComboH14::g_live178r35=true;
  directComboH14::g_intent59c16=false;directComboH14::g_combo_flag_serial59c17=0;
  directComboH14::Pending pendingC14{};pendingC14.gesture59c3=releaseC14.gesture59c3;pendingC14.key=releaseC14.key;
  pendingC14.generation=releaseC14.generation;pendingC14.epoch=releaseC14.epoch;pendingC14.request=releaseC14.request;
  pendingC14.recovery55c1=releaseC14.recovery59c3;pendingC14.release51=releaseC14;
  pendingC14.lease47c2.serial=releaseC14.serial;pendingC14.serial=serialC14;pendingC14.entityId=1993;
  pendingC14.action=receiverC14.data();return pendingC14;};
 {auto pendingC14=directSetupC14();const auto setsC14=directComboH14::setterCalls,callsC14=directComboH14::submitCalls;
  ck(GeometryAdmittedRelease51(releaseC14,receiverC14.data()),"H14 direct flag fixture uses the real current native admitted-release reader");
  ck(directComboH14::run(pendingC14,actor.data(),state.data(),targetC14.data(),hostTickCount64)&&
     directComboH14::setterCalls==setsC14+1&&directComboH14::submitCalls==callsC14+1,
     "H14 qualified same-current admitted direct contact retains the existing one native hit-flag bridge");}
#ifdef KCD2VR_R17_ROUTE_EXTRACT
 const auto beforeChecksR17=total,beforeFailuresR17=failed;
 for(int routeCaseR17=0;routeCaseR17<12;++routeCaseR17){
  auto pendingR17=directSetupC14();pendingR17.action=nullptr;pendingR17.tick=hostTickCount64-100;
  directComboH14::g_intent59c16=true;directComboH14::g_intent_live59c17=true;
  directComboH14::g_apply178r30=true;directComboH14::g_live_fault178r35=false;directComboH14::callableR17=true;
  directComboH14::currentActionR17=receiverC14.data();put(state.data()+0x38,4);
  if(routeCaseR17==1)directComboH14::g_intent_live59c17=false;
  if(routeCaseR17==2)pendingR17.tick=hostTickCount64-5; // original waiting interval
  if(routeCaseR17==3)put(state.data()+0x38,8); // native live window
  if(routeCaseR17==4)++pendingR17.release51.gesture59c3; // previous unrelated native action
  if(routeCaseR17==5)pendingR17.action=actionC14.data(); // already bound elsewhere
  if(routeCaseR17==6)directComboH14::currentActionR17=nullptr;
  if(routeCaseR17==7)directComboH14::g_intent59c16=false;
  if(routeCaseR17==8)directComboH14::g_apply178r30=false;
  if(routeCaseR17==9)directComboH14::g_live_fault178r35=true;
  if(routeCaseR17==10)pendingR17.lease47c2.serial=0;
  if(routeCaseR17==11)directComboH14::callableR17=false;
  const auto beforeR17=pendingR17;const auto setsR17=directComboH14::setterCalls,callsR17=directComboH14::submitCalls;
  directComboH14::routeR17(pendingR17,actor.data(),state.data(),hostTickCount64);
  if(routeCaseR17<=1){
   ck(directComboH14::directR17&&!directComboH14::waitR17&&pendingR17.action==receiverC14.data(),
      "R17 admitted intent direct route carries its current native action instead of losing lineage before goto");
   ck(directComboH14::run(pendingR17,actor.data(),state.data(),targetC14.data(),hostTickCount64)&&
      directComboH14::setterCalls==setsR17+1&&directComboH14::submitCalls==callsR17+1,
      "R17 actual early route reaches existing native480 bridge once through real admitted-release/lineage readers");
   ck(!directComboH14::run(pendingR17,actor.data(),state.data(),targetC14.data(),hostTickCount64)&&
      directComboH14::setterCalls==setsR17+1,
      "R17 same release cannot gain a second native flag edge or setter call");
  }else{
   ck(pendingR17.action==beforeR17.action,
      "R17 wait/live/unadmitted/foreign/disabled/refused route preserves its original action binding");
   ck(directComboH14::setterCalls==setsR17&&directComboH14::submitCalls==callsR17,
      "R17 routing alone cannot submit native hit or write combo flag");
   if(routeCaseR17==2)ck(directComboH14::waitR17&&!directComboH14::directR17&&pendingR17.tick==beforeR17.tick,
      "R17 waiting intent retains original unbound pending contact and occurrence clock");
   if(routeCaseR17==4)ck(directComboH14::directR17&&!pendingR17.action&&
       !directComboH14::run(pendingR17,actor.data(),state.data(),targetC14.data(),hostTickCount64),
      "R17 unadmitted fallback stays unbound and cannot lend another action combo credit");
  }
  ck(pendingR17.tick==beforeR17.tick&&pendingR17.release51.serial==beforeR17.release51.serial&&
     pendingR17.gesture59c3==beforeR17.gesture59c3&&pendingR17.key==beforeR17.key&&
     pendingR17.generation==beforeR17.generation&&pendingR17.epoch==beforeR17.epoch&&
     pendingR17.request==beforeR17.request&&pendingR17.recovery55c1==beforeR17.recovery55c1,
     "R17 binding does not retag gesture/release/item/body/request/recovery or renew contact clock");
 }
 directComboH14::checksR17=total-beforeChecksR17;directComboH14::failuresR17=failed-beforeFailuresR17;
#endif
 for(int refuseC14=0;refuseC14<11;++refuseC14){auto pendingC14=directSetupC14();
  if(refuseC14==0)pendingC14.action=nullptr;
  if(refuseC14==1)++pendingC14.release51.gesture59c3;
  const auto setsC14=directComboH14::setterCalls,callsC14=directComboH14::submitCalls;
  auto callbackC14=[&](){
   if(refuseC14==2)put(actor.data()+0x210,static_cast<void*>(nullptr));
   if(refuseC14==3)put(state.data()+0xCF8,static_cast<void*>(nullptr));
   if(refuseC14==4)cg14::grace.clear();
   if(refuseC14==5)mainSlotC14=nullptr;
   if(refuseC14==6){++bindingC14.intent.policy;g_motion_clock_binding50.publish(bindingC14);}
   if(refuseC14==7)put(state.data()+0x38,16);
   if(refuseC14==8)put(helperC14.data()+0x18,static_cast<void*>(nullptr));
  };
  const bool bridgedC14=directComboH14::run(pendingC14,actor.data(),state.data(),targetC14.data(),hostTickCount64,
      callbackC14,refuseC14!=9,refuseC14!=10);
  ck(!bridgedC14&&directComboH14::setterCalls==setsC14&&directComboH14::submitCalls==callsC14+1,
     "H14 unbound/replaced/cancelled/refused direct contact preserves native submit once without lending another action combo credit");
  put(actor.data()+0x210,state.data());
 }
 // H24: execute the actual completed pose producer -> actual native guard pump
 // while an existing native combo lease is open. A ready pose cannot steal it;
 // an intentional front broad-face guard remains a legitimate native input.
 for(int poseH24=0;poseH24<4;++poseH24){
  resetC14();put(state.data()+0x2A8,std::uint8_t(0));
  frame.input.positionGuard176=true;frame.input.motionOwned59c5=false;
  frame.input.shoulders[0]={.15,0,1.3};frame.input.shoulders[1]={-.15,0,1.3};
  frame.input.grip[0]=1;frame.input.controllerAnchor119=true;frame.input.dynamics=false;frame.input.contact=false;
  frame.input.heldMotionAttacks=true;frame.input.guardEnterDwellMs178r29=0;frame.input.guardEnterMaxSpeed178r29=2.5;
  auto& profileH24=frame.profiles[0];profileH24.classId=4;profileH24.geometry=true;profileH24.primary=true;
  profileH24.tip={0,0,1};profileH24.axis={0,0,1};profileH24.grip[0].p={};
  profileH24.guardFaceReadyH24=true;profileH24.guardFaceNormalH24={0,1,0};
  ph::Pose pose{};pose.p={0,.35,1.1};
  if(poseH24==0)pose.q={0,0,.7071067811865475,.7071067811865475};
  if(poseH24==1)pose.p={.8,.35,1.1};if(poseH24==2)pose.p={0,.35,.2};
  ph::State producerH24{};ph::Output producedH24{};std::array<ph::Profile,2> profilesH24{};profilesH24[0]=profileH24;
  for(int stepH24=0;stepH24<24;++stepH24){++hostTickCount64;frame.input.tick=hostTickCount64;++frame.input.sample;
   frame.input.hands[0]=frame.input.intentHands[0]=pose;frame.input.intentTracked[0]=true;
   producedH24=ph::step(producerH24,frame.input,profilesH24,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.;});}
  frame.output.guardBySide30c1[0]=producedH24.guardBySide30c1[0];frame.output.guardBySide30c1[1]=false;
  frame.output.strokeActive119=false;frame.output.primaryStrokeGesture19=0;frame.output.weapons[0]=pose;
  frame.output.active[0]=frame.output.owned[0]=true;frame.output.active[1]=false;
  frame.output.invalidMask55c1[0]=0;frame.output.handlingOnly55c1[0]=frame.output.solverOwnedT4[0]=false;
  animation_filter::combatPoseTick112=hostTickCount64;g_phys_ready_sample6698=frame.input.sample;g_phys_frame6698.publish(frame);
  TrackingSnapshot trackingH24{};trackingH24.allowed=trackingH24.head_valid=trackingH24.right_valid=trackingH24.left_valid=true;
  trackingH24.tick=trackingH24.head_good_tick=trackingH24.right_good_tick=trackingH24.left_good_tick=hostTickCount64;
  trackingH24.sequence=frame.input.sample;g_tracking_frames6695.publish(trackingH24);
  g_combat_bypass101=&action;g_combat_input_original101=&action;g_native_guard112={};
  const auto beforeH24=blockModes176.size();NativeGuardTick112();
  if(poseH24<3){
   ck(blockModes176.size()==beforeH24&&!g_native_guard112.held,"H24 actual full-TU guard pump cannot turn edge-on/side/low ready pose into LT during finite combo lease");
   kcd2vr::subsystems::masterstrike_combos::NativeSlotF1 retainedH24{};cg14::SlotLeaseF1 leaseH24{};std::uint64_t consumedH24=0;
   ck(cg14::nativeSlot.read(leaseH24,consumedH24)&&ReadNativeComboContinuationH19(actor.data(),retainedH24,leaseH24),
      "H24 ready pose retains exact native phase16/action/finite combo continuation without fabricated slot");
  }else ck(blockModes176.size()==beforeH24+1&&blockModes176.back()==1&&g_native_guard112.held,
      "H24 intentional front broad-face guard may send original native LT while combo exists; game remains cancellation authority");
 }
 // Candidate H24 native solver receipt -> completed frame -> real native LT.
 resetC14();put(state.data()+0x2A8,std::uint8_t(0));g_native_guard112={};
 frame.input.positionGuard176=true;frame.input.haveWorld=true;frame.input.world={};frame.input.worldScale=1;frame.input.reach[0]=3;
 frame.input.shoulders[0]={.15,0,1.3};frame.input.shoulders[1]={-.15,0,1.3};frame.input.grip[0]=1;frame.input.intentTracked[0]=true;
 frame.input.guardEnterDwellMs178r29=0;frame.input.guardEnterMaxSpeed178r29=2.5;frame.input.guardExitSpeed178r45=2;
 auto& solverProfile=frame.profiles[0];solverProfile.classId=4;solverProfile.geometry=solverProfile.primary=true;
 solverProfile.tip={0,0,1};solverProfile.axis={0,0,1};solverProfile.grip[0].p={};solverProfile.guardFaceReadyH24=true;solverProfile.guardFaceNormalH24={0,1,0};
 heldT4::Input nativeSourceH24{};nativeSourceH24.owner.key=solverProfile.key;nativeSourceH24.owner.generation=solverProfile.generation;
 nativeSourceH24.owner.entityId=solverProfile.entity;nativeSourceH24.owner.request=frame.input.epoch;nativeSourceH24.owner.epoch=frame.input.bodyEpoch;
 nativeSourceH24.colliderLease=1;nativeSourceH24.trackingEpoch=1;nativeSourceH24.cancelGeneration=1;
 heldT4::Achieved achievedH24{};achievedH24.owner=nativeSourceH24.owner;achievedH24.colliderLease=1;achievedH24.trackingEpoch=1;achievedH24.cancelGeneration=1;
 achievedH24.worldGeneration=achievedH24.bodyGeneration=1;achievedH24.bodyId=3;achievedH24.state=heldT4::State::Active;
 achievedH24.poseValid=true;achievedH24.colliderRetired=false;achievedH24.weaponWorld.p={0,.35,1.1};ph::State solverStateH24{};
 const auto initialSolverBlocks=blockModes176.size();
 for(int sampleH24=0;sampleH24<3;++sampleH24){++hostTickCount64;++frame.input.sample;frame.input.tick=hostTickCount64;
  achievedH24.sample=frame.input.sample;achievedH24.sourceTick=achievedH24.observedTick=hostTickCount64;
  frame.input.hands[0]=frame.input.intentHands[0]={achievedH24.weaponWorld.q,achievedH24.weaponWorld.p};
  ck(kcd2vr::native_held_pose_t4::apply(nativeSourceH24,achievedH24,hostTickCount64,frame.input,frame.profiles,frame.output,solverStateH24),
     "H24 native solver receipt closes actual coherent pose without local integrator");
  animation_filter::combatPoseTick112=hostTickCount64;g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
  g_combat_bypass101=&action;g_combat_input_original101=&action;NativeGuardTick112();
 }
 ck(blockModes176.size()>initialSolverBlocks&&g_native_guard112.held&&frame.output.solverOwnedT4[0],
    "H24 actual achieved native body posture reaches original native LT transport");
 ck(!frame.input.attacks&&!frame.output.contactKnown[0]&&frame.output.commandCount==0,
    "H24 native solver posture cannot fabricate collision witness, attack or damage");
 // Physics body remains stopped at the SAME receipt; fresh raw stroke exits LT.
 ++frame.input.sample;hostTickCount64+=10;frame.input.tick=hostTickCount64;achievedH24.observedTick=hostTickCount64;
 frame.input.hands[0].p.y+=.08;frame.input.intentHands[0].p.y+=.08;
 ck(kcd2vr::native_held_pose_t4::apply(nativeSourceH24,achievedH24,hostTickCount64,frame.input,frame.profiles,frame.output,solverStateH24)&&!frame.output.guard117,
    "H24 duplicate achieved receipt still exits posture on fresh tracked forward stroke");
 animation_filter::combatPoseTick112=hostTickCount64;g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
 const auto beforeDuplicateExitH24=blockModes176.size();NativeGuardTick112();
 ck(!g_native_guard112.held&&blockModes176.size()==beforeDuplicateExitH24+1&&blockModes176.back()==2,
    "H24 actual native LT-up follows intended stroke even before a new physics receipt");

 NativeGuardRelease112();PhysAbortAttack117(player.data());cg14::grace.clear();cg14::nativeSlot.retire();
 actor=savedActorC14;state=savedStateC14;frame=savedFrameC14;command=savedCommandC14;g_phys_native6698=savedNativeC14;
 g_input_prepared_owner117=savedPreparedC14;g_synchronized59c10r9=savedSyncC14;g_combat_integrated178r22c2=savedIntegratedC14;
 g_released_input_h14=savedReleasedC14;g_input_source_generation_h14=savedSourceGenerationC14;
 g_combat_experiment175=savedExperimentC14;g_physical_combat177=savedPhysicalC14;g_motion_clock_ready50=savedReadyC14;
 g_motion_clock_fault50=savedFaultC14;ac50::enabled=savedClockEnabledC14;kcd2vr::motion59c4::selected=savedDirectC14;
 g_attack_timing_contract22=savedTimingContractC14;g_attack_timing_policy22=savedTimingPolicyC14;
 g_pose_epoch6689=savedBodyC14;g_weapon_request_epoch6694=savedRequestC14;kcd2vr::contact176::recoveryEpoch55c1=savedRecoveryC14;
 g_phys_ready_sample6698=savedReadySampleC14;g_phys_frame6698.publish(frame);g_tracking_frames6695.publish(savedTrackingC14);
 g_motion_clock_binding50.publish(savedBindingC14);g_motion_clock_actor50=savedClockActorC14;g_thrust_as_swing55=savedThrustC14;
 g_ctx_menu=savedMenuC14;g_ctx_dialogue=savedDialogueC14;g_ctx_cutscene=savedCutsceneC14;g_ctx_scripted=savedScriptedC14;
 g_gameplay_allowed=savedGameplayC14;g_head_valid=savedHeadC14;g_right_valid=savedRightC14;g_left_valid=savedLeftC14;
 animation_filter::nativeBodyOwned112=savedNativeBodyC14;
 g_combat_bypass101=savedBypassC14;g_combat_input_original101=savedInputOriginalC14;g_admission_gate51=savedAdmissionC14;
 g_attack_require_opponent59c10=savedRequireC14;diagnostics::capture168::gate.mask=savedCaptureC14;
 combat_diag_probe::SetSynchronized59c10r9(savedDiagSyncC14);combat_diag_probe::SetIntent59c16(savedDiagIntentC14);
 combat_diag_probe::SetGeometryLiveAction178r35(savedDiagLiveC14);
 ck(kcd2vr::contact176::claimedSerial==savedClaimedC14,"H14 continuation fixture never awards a hit or consumes a damage ticket");
}
