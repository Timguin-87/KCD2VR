{
 model117=true;route117=true;specialAllowed117=true;put(control117.data()+0x18,actor.data());
 diagnostics::lab::Set(diagnostics::lab::Key::F3,0);animation_filter::nativeBodyOwned112=false;
 g_weapon_control_mode=6;g_phys_armed6698=true;g_phys_fault6698=false;g_native_fault104=false;
 g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;g_pose_epoch6689=g_weapon_request_epoch6694=1;
 animation_filter::combatPoseTick112=hostTickCount64;
 g_henry_item_attach_manager=manager.data();g_last_player_entity=entity.data();g_census_mode=3;
 kcd2vr::retail_build112::hostRetailVerified112=true;g_input_contract117=base;
 g_phys_native6698.player=player.data();g_phys_native6698.epoch=g_phys_native6698.request=1;g_phys_native6698.attackPrepared=false;
 static const char special[]="attack_special";g_phys_native6698.actionNames[4]=reinterpret_cast<std::uint64_t>(special);
 frame.input.coherentMelee119=true;frame.input.naturalCombat117=true;frame.input.requirePrimaryGrip114=true;frame.input.grip[0]=1;
 frame.input.allowed=frame.input.tracked[0]=frame.input.tracked[1]=true;frame.request=frame.input.epoch=frame.input.bodyEpoch=1;
 frame.player=player.data();frame.manager=manager.data();frame.output.active[0]=frame.output.owned[0]=true;frame.output.guard117=false;
 frame.output.guardBySide30c1[0]=frame.output.guardBySide30c1[1]=false; // completed non-guard sample, not a retained preceding parry fixture
 frame.profiles[0]=item.profile;frame.profiles[0].geometry=true;frame.profiles[0].tip={.6,0,0};
 command.key=77;command.generation=4;command.bodyEpoch=command.epoch=1;command.side=0;
 put(hands.data()+0x260,std::uint64_t(77));put(actor.data()+0x1F8,player.data());put(state.data()+0xCE0,actor.data());
 // The preceding contact fixture's stack collision owner has ended its scope.
 put(actor.data()+0x218,static_cast<void*>(nullptr));
 auto idle=[&](){put(state.data()+0x38,1);put(state.data()+0x218,-1);put(state.data()+0x2A8,std::uint8_t(0));put(state.data()+0x2D8,std::uint8_t(0));put(state.data()+0x308,std::uint8_t(0));};idle();
 const auto originalCommits=commits117;
 // R59.16C10R4F/R12: the fixture has no mutual combat opponent. R4F withheld the native input (NO_OPPONENT); R12 sends it - the 19:43
 // log withheld 10 strokes at the start of a fight because +0xCF8 was still empty while Henry's state already said in combat. The reach/
 // angle gate applies once an opponent is linked (not reachable from this fixture).
 {g_attack_require_opponent59c10=true;idle();command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Slash;command.zone117=2;
  const auto beginsBefore=begins117;
  ck(NativePhysicalAction117(player.data(),command,frame,true)&&begins117==beginsBefore+1,"R12: prepare without a linked opponent is sent natively (gate waits for the link)");
  PhysAbortAttack117(player.data());
  g_attack_require_opponent59c10=false;}
 primaryLatch117=1;PhysAbortAttack117(player.data());
 ck(primaryLatch117==0&&commits117==originalCommits,"abort then primary release clears cancelled latch without committing an attack");
 // Reproduce the old defect independently from the fixed helper.
 PhysCallAction6698(player.data(),2,1);PhysCallAction6698(player.data(),2,2);
 const auto oldBegins=begins117;PhysCallAction6698(player.data(),0,1);
 ck(primaryLatch117==2&&begins117==oldBegins,"old abort press/release leaves next native prepare ignored");PhysAbortAttack117(player.data());
 // R55: with VrThrustAsSwing (default) a thrust is the ORDINARY (heavy) input from its origin zone; the light/stab input only when it is off.
 for(const bool thrustAsSwing:{true,false})for(auto kind:{ph::CommandKind::Slash,ph::CommandKind::Thrust,ph::CommandKind::Pommel}){
  g_thrust_as_swing55=thrustAsSwing;
  idle();command.kind=ph::CommandKind::Prepare;command.stroke117=kind;command.zone117=kind==ph::CommandKind::Slash?2:kind==ph::CommandKind::Thrust?(thrustAsSwing?1:5):-1;
  const int expected=kind==ph::CommandKind::Slash?18:kind==ph::CommandKind::Thrust?(thrustAsSwing?18:17):19;
  ck(NativePhysicalAction117(player.data(),command,frame,true)&&beginInput117==expected,"production prepare crosses observed upstream leaf with correct live input class");
  command.kind=kind;
  ck(NativePhysicalAction117(player.data(),command,frame,false)&&releaseInput117==expected,"production release preserves prepared cut/thrust/special input class");
  ck(!g_input_scope117,"synthetic input scope ends at native return");
 }
 g_thrust_as_swing55=false; // the F11 guard-origin fixtures below exercise the native light/stab lifecycle
 // F11 submits four guard origins with the same native light-input lifecycle.
 // The host does not pretend to resolve master-strike eligibility.
 {
  g_weapon_control_mode=7;g_f11_test167=2;diagnostics::lab::Set(diagnostics::lab::Key::F3,3);
  diagnostics::lab::Set(diagnostics::lab::Key::F5,0);cc::selectSources(cc::F11Stabs167,hostTickCount64,true);
  cc::Event event{};while(cc::pop(event)){}
  for(int z:{0,1,2,5}){
   idle();command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Thrust;command.zone117=command.originZone167=z;
   ck(NativePhysicalAction117(player.data(),command,frame,true)&&lastZone==z&&beginInput117==17,"F11 four-way guard origin retains native light input on preparation");
   command.kind=ph::CommandKind::Thrust;
   ck(NativePhysicalAction117(player.data(),command,frame,false)&&releaseInput117==17,"F11 stab commits through same exact upstream controller path");
   int edges=0;while(cc::pop(event))if(event.kind==cc::Kind::F11Native167){++edges;
    ck(event.id==z&&event.layer==z&&event.current[1]==-1&&event.flags==7,"F11 diagnostic separates requested/observed guard from unforced native attack type");}
   ck(edges==2,"one F11 observation per prepare and commit edge");
  }
  for(int z:{3,4}){idle();command.zone117=z;const auto previous=zoneCalls;
   ck(!NativePhysicalAction117(player.data(),command,frame,true)&&zoneCalls==previous,"disabled guard zones never reach a native write");}
  command.originZone167=-1;g_weapon_control_mode=6;g_f11_test167=0;
  diagnostics::lab::Set(diagnostics::lab::Key::F3,0);cc::selectSources(0,hostTickCount64);
 }
 g_thrust_as_swing55=true;
 idle();specialAllowed117=false;command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Pommel;command.zone117=-1;
 const auto specialCommits=commits117;
 ck(!NativePhysicalAction117(player.data(),command,frame,true)&&commits117==specialCommits,"native special-context rejection is not replaced with a fabricated pommel or ordinary cut");specialAllowed117=true;
 NativeOwner104 testOwner{};ck(NativeResolve104(player.data(),testOwner),"input fixture retains exact Henry identity");
 NativeInputScope117 inputScope{testOwner,ph::CommandKind::Thrust,true};g_input_scope117=&inputScope;
 ck(NativeInputClass117(control117.data(),18,base+0x27C08E6,true)==18,"R55: a thrust as a swing keeps the ordinary input argument");
 g_thrust_as_swing55=false;inputScope.visited=false;
 ck(NativeInputClass117(control117.data(),18,base+0x27C08E6,true)==17,"qualified physical thrust changes only the upstream input argument");
 g_thrust_as_swing55=true;
 inputScope.visited=false;
 ck(NativeInputClass117(control117.data(),18,base+0x27C08E7,true)==18&&!inputScope.visited,"different native caller stays unchanged");
 put(control117.data()+0x18,static_cast<void*>(nullptr));
 ck(NativeInputClass117(control117.data(),18,base+0x27C08E6,true)==18&&!inputScope.visited,"NPC or mismatched receiver stays unchanged");put(control117.data()+0x18,actor.data());
 int threadInput=0;std::thread other([&](){threadInput=NativeInputClass117(control117.data(),18,base+0x27C08E6,true);});other.join();
 ck(threadInput==18,"another thread cannot inherit the physical input scope");g_input_scope117=nullptr;
 ck(NativeInputClass117(control117.data(),18,base+0x27C08E6,true)==18,"normal controls remain unchanged outside our OnAction call");
 command.stroke117=ph::CommandKind::Slash;command.zone117=1;idle();route117=false;
 ck(!NativePhysicalAction117(player.data(),command,frame,true),"OnAction return without matched handler is not acknowledged");route117=true;
 kcd2vr::retail_build112::hostRetailVerified112=false;const auto unverifiedCalls=actionCalls;
 ck(!NativePhysicalAction117(player.data(),command,frame,true)&&actionCalls==unverifiedCalls,"unverified SHA prevents typed input and all direction writes");
 kcd2vr::retail_build112::hostRetailVerified112=true;g_right_valid=false;
 ck(!NativePhysicalAction117(player.data(),command,frame,true)&&actionCalls==unverifiedCalls,"live tracking loss rejects a previously captured melee command");g_right_valid=true;
 animation_filter::nativeBodyOwned112=true;ck(!NativePhysicalAction117(player.data(),command,frame,true),"native reaction/paired ownership excludes typed melee input");animation_filter::nativeBodyOwned112=false;
 // V119: final F12 owns typed inputs without requiring combat/F3 selection.
 diagnostics::lab::Set(diagnostics::lab::Key::F3,1);const auto referenceCalls119=actionCalls;
 ck(!NativePhysicalAction117(player.data(),command,frame,true)&&actionCalls==referenceCalls119,"F3 reference still vetoes full F12 typed input");
 diagnostics::lab::Set(diagnostics::lab::Key::F3,0);
 g_phys_input6698=true;frame.input.tick=hostTickCount64;g_phys_frame6698.publish(frame);
 ck(WeaponMotionInput119(player.data(),manager.data(),hostTickCount64),"fresh live melee owner qualifies F12 raw-input ownership without F3");
 ck(!WeaponMotionInput119(player.data(),manager.data(),hostTickCount64+101),"stale F12 frame cannot steal native trigger");
 for(int cls:{10,11}){auto firearm119=frame;firearm119.profiles[0].classId=cls;g_phys_frame6698.publish(firearm119);
  ck(!WeaponMotionInput119(player.data(),manager.data(),hostTickCount64),"non-melee non-bow weapon retains native input");}
 g_phys_frame6698.publish(frame);
 // Actual production queue: cancellation followed by two complete strokes.
 NativeGuardRelease112();idle();g_phys_input6698=true;g_phys_abort6698=false;
 PhysAbortAttack117(player.data());g_phys_native6698.attackPrepared=false;
 frame.output.weapons[0]={{0,0,.7071067811865476,.7071067811865476},{0,.3,.2}};
 g_phys_commands6698.head=g_phys_commands6698.tail=0;
 const auto queuedCommits=commits117;
 for(int swing=0;swing<2;++swing){
  idle();++hostTickCount64;++frame.input.sample;frame.input.tick=hostTickCount64;animation_filter::combatPoseTick112=hostTickCount64;
  meta.metadataAt=hostTickCount64;PhysReadLive103(meta.last);g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
  command.kind=ph::CommandKind::Prepare;command.stroke117=swing?ph::CommandKind::Thrust:ph::CommandKind::Slash;command.zone117=swing?5:2;
  command.sample=frame.input.sample;command.tick=hostTickCount64;g_phys_commands6698.push(command);
  command.kind=command.stroke117;g_phys_commands6698.push(command);
  PhysicalNativeFrameTick6698();ck(g_phys_native6698.attackPrepared&&commits117==queuedCommits+unsigned(swing),"queue preserves separate native prepare frame after cancellation");
  ++hostTickCount64;PhysicalNativeFrameTick6698();
  ck(!g_phys_native6698.attackPrepared&&commits117==queuedCommits+unsigned(swing)+1&&releaseInput117==18,"both consecutive queued strokes commit with their correct native classes (R55: thrust = ordinary input)");
 }
 // R59C3: the real contact-mode consumer must recover an orphaned release.
 // The old R33 engagement switch is false in this fixture; contact mode still
 // requires native semantic input, never the retired telegraph-only shortcut.
 {const auto experiment=g_combat_experiment175.load();const bool route=kcd2vr::combat_diag_probe::GeometryLiveAction178r35();
  g_combat_experiment175=true;kcd2vr::combat_diag_probe::hostGeometry59c3=true;
  kcd2vr::combat_diag_probe::SetGeometryLiveAction178r35(true);g_latchedNative33=false;g_latchedKey33=0;
  for(auto kind:{ph::CommandKind::Slash,ph::CommandKind::Thrust}){
   idle();PhysAbortAttack117(player.data());g_phys_native6698.attackPrepared=false;
   ++hostTickCount64;++frame.input.sample;frame.input.tick=hostTickCount64;animation_filter::combatPoseTick112=hostTickCount64;
   meta.metadataAt=hostTickCount64;PhysReadLive103(meta.last);g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
   command.kind=command.stroke117=kind;command.zone117=kind==ph::CommandKind::Thrust?5:2;
   command.sample=frame.input.sample;command.tick=hostTickCount64;command.gesture59c3=frame.input.sample;
   const auto starts=begins117,ends=commits117;g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
   ck(g_phys_native6698.attackPrepared&&begins117==starts+1&&commits117==ends&&g_phys_commands6698.head!=g_phys_commands6698.tail,
      "R59C3 contact-mode orphaned cut/stab restores upstream preparation, without consuming release");
   ++hostTickCount64;PhysicalNativeFrameTick6698();
   ck(!g_phys_native6698.attackPrepared&&commits117==ends+1&&releaseInput117==18&&g_phys_commands6698.head==g_phys_commands6698.tail,
      "R59C3 contact-mode restored cut/stab delivers one release on the next native frame");
  }
  kcd2vr::combat_diag_probe::hostGeometry59c3=false;kcd2vr::combat_diag_probe::SetGeometryLiveAction178r35(route);g_combat_experiment175=experiment;
 }
 // R57C1: run held prepare -> feint -> physical thrust through actual consumer.
 {const bool oldExperiment=g_combat_experiment175.load();g_combat_experiment175=true;
  auto publish57=[&](){++hostTickCount64;++frame.input.sample;frame.input.tick=hostTickCount64;
   animation_filter::combatPoseTick112=hostTickCount64;meta.metadataAt=hostTickCount64;
   PhysReadLive103(meta.last);g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
   command.sample=frame.input.sample;command.tick=hostTickCount64;};
  idle();PhysAbortAttack117(player.data());g_phys_native6698.attackPrepared=false;
  command.held57=true;command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Slash;command.zone117=1;
  publish57();const auto begins57=begins117,commits57=commits117;
  g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
  ck(g_phys_native6698.attackPrepared&&g_phys_native6698.heldPrepare57&&begins117==begins57+1,"R57C1 consumer records the deliberate held preparation");
  hostTickCount64+=1000;publish57();command.kind=ph::CommandKind::Zone57;command.zone117=2;
  const auto calls57=actionCalls;g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
  ck(g_phys_native6698.attackPrepared&&g_phys_native6698.zone117==2&&lastZone==2&&actionCalls==calls57,
     "R57C1 held feint after one second updates exact native zone without a second input edge");
  for(int bad=0;bad<7;++bad){auto testFrame=frame;auto testCommand=command;
   const auto oldZone=zoneCalls;
   if(bad==0)testFrame.output.handlingOnly55c1[0]=true;
   if(bad==1)testCommand.generation++;
   if(bad==2)testCommand.bodyEpoch++;
   if(bad==3)testCommand.tick=hostTickCount64-101;
   if(bad==4)animation_filter::nativeBodyOwned112=true;
   if(bad==5)g_right_valid=false;
   if(bad==6)g_combat_experiment175=false;
   ck(!NativeZoneOnly57(player.data(),testCommand,testFrame)&&zoneCalls==oldZone,
      "R57C1 feint cannot write through display-only, generation, epoch, expiry, native-body, tracking or mode loss");
   animation_filter::nativeBodyOwned112=false;g_right_valid=true;g_combat_experiment175=true;
  }
  publish57();command.kind=command.stroke117=ph::CommandKind::Thrust;
  g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
  ck(!g_phys_native6698.attackPrepared&&begins117==begins57+1&&commits117==commits57+1&&releaseInput117==18,
     "R57C1 slash-held wind-up can commit an actual thrust without classification-mismatch cancellation");
  command.held57=false;g_combat_experiment175=oldExperiment;idle();
 }
 #include "forward_refinement19c4r2_cases.inl"
 #include "cut_feint62_native_cases.inl"
 // Expired commit must release its preparation without dropping F12 ownership.
 auto publish117=[&](){idle();++hostTickCount64;++frame.input.sample;frame.input.tick=hostTickCount64;
  animation_filter::combatPoseTick112=hostTickCount64;meta.metadataAt=hostTickCount64;
  PhysReadLive103(meta.last);g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;};
 publish117();command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Slash;command.zone117=2;
 command.sample=frame.input.sample;command.tick=hostTickCount64;g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(g_phys_native6698.attackPrepared&&primaryLatch117==1,"isolated prepare retains its native latch");
 const auto expiredCommits=commits117;command.kind=ph::CommandKind::Slash;hostTickCount64+=101;publish117();
 g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(!g_phys_native6698.attackPrepared&&primaryLatch117==0&&commits117==expiredCommits&&g_phys_input6698,
    "expired commit cancels its preparation without firing or dropping physical input ownership");
 // A different gesture cannot reuse the preceding gesture's preparation class.
 publish117();command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Slash;command.zone117=2;
 command.sample=frame.input.sample;command.tick=hostTickCount64;g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 const auto replacedBegins=begins117;
 publish117();command.stroke117=ph::CommandKind::Thrust;command.zone117=5;command.sample=frame.input.sample;command.tick=hostTickCount64;
 g_phys_commands6698.push(command);command.kind=ph::CommandKind::Thrust;g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(g_phys_native6698.attackPrepared&&begins117==replacedBegins+1&&beginInput117==18&&g_phys_input6698&&commits117==expiredCommits,
    "new preparation replaces the old gesture without committing it or releasing physical input ownership");
 ++hostTickCount64;PhysicalNativeFrameTick6698();
 ck(!g_phys_native6698.attackPrepared&&commits117==expiredCommits+1&&releaseInput117==18,"replacement gesture commits its own prepared class (R55: ordinary input)");
 idle();g_phys_native6698.attackPrepared=false;primaryLatch117=0;
 command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Slash;command.zone117=2;
 ck(NativePhysicalAction117(player.data(),command,frame,true),"fresh owned prepare before guard transition");
 g_phys_native6698.attackPrepared=true;g_phys_native6698.attackKey=77;
 frame.output.weapons[0]={{},{-.3,.4,.3}};frame.input.shoulders[0]=frame.input.shoulders[1]={};frame.output.guard117=true;
 NativeGuardRelease112();const auto guardCommits=commits117,presses=blockPresses117;
 for(int n=0;n<3;++n){hostTickCount64+=10;++frame.input.sample;frame.input.tick=hostTickCount64;
  animation_filter::combatPoseTick112=hostTickCount64;g_phys_frame6698.publish(frame);NativeGuardTick112();}
 ck(g_native_guard112.held&&!g_phys_native6698.attackPrepared&&primaryLatch117==0&&commits117==guardCommits,"guard cancels only our pending prepare, resets its latch, and does not fire");
 ck(blockPresses117==presses+1,"guard transition makes exactly one native press");
 for(int n=0;n<30;++n){hostTickCount64+=10;++frame.input.sample;frame.input.tick=hostTickCount64;
  animation_filter::combatPoseTick112=hostTickCount64;g_phys_frame6698.publish(frame);NativeGuardTick112();}
 ck(blockPresses117==presses+1,"holding guard sends holds, never repeated perfect-block presses");
 NativeGuardRelease112();idle();g_phys_native6698.attackPrepared=false;
 ck(NativePhysicalAction117(player.data(),command,frame,true),"capture prepared component identity for replacement test");
 g_phys_native6698.attackPrepared=true;g_phys_native6698.natural117=true;
 std::array<unsigned char,0xE00> replacementState117{};put(replacementState117.data()+0xCE0,actor.data());put(actor.data()+0x210,replacementState117.data());
 const auto replacedCalls=actionCalls;
 ck(!NativePhysicalAction117(player.data(),command,frame,false)&&actionCalls==replacedCalls,"same player with replacement combat state cannot receive the old commit");
 PhysCancelNative6698();ck(actionCalls==replacedCalls&&!g_phys_native6698.attackPrepared,"replacement state is not cancelled to clean up a retired component's request");
 put(actor.data()+0x210,state.data());PhysAbortAttack117(player.data());
 frame.output.strokeActive119=true;g_phys_frame6698.publish(frame);NativeGuardTick112();
 ck(!g_native_guard112.held,"active cut crossing guard space releases physical guard ownership");
 #include "shield_attack_transport19c2_cases.inl"
 // R59C4 executes the actual physical command consumer, with native leaves counted.
 {
  namespace mo=kcd2vr::motion59c4;mo::attempts.clear();mo::selected=true;
  g_phys_native6698.attackPrepared=false;idle();frame.output.guard117=false;
  frame.output.strokeActive119=true;frame.input.physicalCombat177=true;frame.input.attacks=true;
  frame.output.handlingOnly55c1[0]=false;frame.output.owned[0]=frame.output.active[0]=true;
  const auto begins=begins117,commits=commits117;
  for(auto kind:{ph::CommandKind::Slash,ph::CommandKind::Thrust}){
   hostTickCount64+=10;publish117();command.kind=kind;command.stroke117=kind;command.zone117=2;
   command.originZone167=-1;command.kick59=false;command.sample=frame.input.sample;command.tick=hostTickCount64;
   command.gesture59c3=command.sample;g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
   mo::Attempt result;const mo::Key key{command.gesture59c3,command.key,command.generation,command.bodyEpoch,command.epoch,
      kcd2vr::contact176::recoveryEpoch55c1.load(),reinterpret_cast<std::uintptr_t>(player.data())};
   ck(mo::attempts.claim(key,command.tick,hostTickCount64,55,result)==mo::Claim::Ready,
       "motion preview cut/thrust commits now without native preparation or phase");
   ck(result.technique==(kind==ph::CommandKind::Thrust?1:0)&&result.origin==2,"motion preview preserves technique separate from origin");
  }
  ck(begins117==begins&&commits117==commits&&!g_phys_native6698.attackPrepared,
      "motion preview emitted zero native ordinary begins/releases; no hidden reference fallback");
  mo::selected=false;mo::attempts.clear();
 }
 #include "native_block_metadata_h14_cases.inl"
 #include "native_preopen_stance_h14_cases.inl"
 #include "native_master23_cases.inl"
 #include "native_kick59c10_cases.inl"
 #include "native_owned_combo_h14_cases.inl"
 frame.input.coherentMelee119=false;frame.output.strokeActive119=false;
 NativeGuardRelease112();idle();frame.input.naturalCombat117=false;frame.output.guard117=false;model117=false;g_input_contract117=0;
}
