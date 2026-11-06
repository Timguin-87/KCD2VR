// Actual native queue/zone-only transport: first forward cut samples can become
// one stab, without restarting the native input or renewing the attempt onset.
{
 const bool oldExperiment=g_combat_experiment175.load(),oldIntegrated=g_combat_integrated178r22c2.load();
 const bool oldSync=g_synchronized59c10r9.load(),oldIntent=g_intent59c16.load(),oldHeavy=g_thrust_as_swing55.load();
 const auto savedCommand=command;
 auto& gate59=kcd2vr::diagnostics::capture168::gate;
 const auto mask59=gate59.mask.load(),source59=cc::sourceMask.load();
 const auto until59=gate59.until[0].load(),session59=gate59.session[0].load();
 const auto ccUntil59=cc::until.load(),ccSession59=cc::session.load();
 gate59.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,hostTickCount64);
 cc::selectSources(cc::CaptureOnly168R1,hostTickCount64,true);
 kcd2vr::outcome23::with([](kcd2vr::outcome23::Core& k){k={};});
 g_combat_experiment175=g_combat_integrated178r22c2=g_synchronized59c10r9=true;
 g_intent59c16=false;g_thrust_as_swing55=true;
 auto publishRefinement=[&](){++hostTickCount64;++frame.input.sample;frame.input.tick=hostTickCount64;
  animation_filter::combatPoseTick112=hostTickCount64;meta.metadataAt=hostTickCount64;
  PhysReadLive103(meta.last);g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
  command.sample=frame.input.sample;command.tick=hostTickCount64;};
 idle();PhysAbortAttack117(player.data());g_phys_native6698.attackPrepared=false;
 command.held57=false;command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Slash;
 command.zone117=2;command.gesture59c3=0xC41942;
 publishRefinement();const auto starts=begins117,ends=commits117;
 g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 const auto onset=g_phys_native6698.prepareTickC10R18;
 ck(g_phys_native6698.attackPrepared&&!g_phys_native6698.heldPrepare57&&begins117==starts+1&&onset==command.tick,
    "C4R2 forward SYNC onset prepares exactly one ordinary heavy input");
 publishRefinement();command.kind=ph::CommandKind::Zone57;command.stroke117=ph::CommandKind::Thrust;command.zone117=5;
 const auto originalOwner=g_phys_native6698;
 for(int bad=0;bad<17;++bad){auto testCommand=command;auto testFrame=frame;g_phys_native6698=originalOwner;
  if(bad==0)++testCommand.gesture59c3;
  if(bad==1)++testCommand.key;
  if(bad==2)++testCommand.generation;
  if(bad==3)++testCommand.bodyEpoch;
  if(bad==4)++testCommand.epoch;
  if(bad==5)testCommand.sample=originalOwner.prepareSample117-1;
  if(bad==6)testCommand.sample=frame.input.sample+1;
  if(bad==7)testCommand.tick=onset+101;
  if(bad==8)testCommand.tick=hostTickCount64-101;
  if(bad==9)g_thrust_as_swing55=false;
  if(bad==10)g_synchronized59c10r9=false;
  if(bad==11)g_intent59c16=true;
  if(bad==12)testFrame.output.handlingOnly55c1[0]=true;
  if(bad==13)animation_filter::nativeBodyOwned112=true;
  if(bad==14)g_right_valid=false;
  if(bad==15)g_phys_native6698.attackPrepared=false;
  if(bad==16)testCommand.held57=true;
  const auto writes=zoneCalls,inputs=actionCalls;
  ck(!NativeZoneOnly57(player.data(),testCommand,testFrame)&&zoneCalls==writes&&actionCalls==inputs,
     "C4R2 forward refinement refuses stale/foreign/light-class/non-SYNC/protected ownership without native writes");
  g_thrust_as_swing55=g_synchronized59c10r9=true;g_intent59c16=false;
  animation_filter::nativeBodyOwned112=false;g_right_valid=true;
 }
 g_phys_native6698=originalOwner;
 const auto inputs=actionCalls;g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(g_phys_native6698.attackPrepared&&g_phys_native6698.stroke117==ph::CommandKind::Thrust&&
    g_phys_native6698.zone117==5&&lastZone==5&&actionCalls==inputs&&begins117==starts+1&&
    g_phys_native6698.prepareTickC10R18==onset&&g_phys_native6698.prepareGestureC10R18==command.gesture59c3,
    "C4R2 same-gesture cut-to-stab refinement changes zone/classification without a second input or renewed onset");
 kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){
  ck(k.cur.gesture==command.gesture59c3&&k.cur.feint59.count==2&&
     k.cur.feint59.announcements[0].requested==2&&k.cur.feint59.announcements[0].ack==2&&
     k.cur.feint59.announcements[1].kind==4&&k.cur.feint59.announcements[1].ack==5&&
     k.cur.feint59.announcements[1].dispatched&&!k.cur.feint59.applied,
     "C59 actual native prepare/refinement copies requested and ACKed zones, never assumes NPC reaction or damage");});
 publishRefinement();command.kind=ph::CommandKind::Thrust;
 g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(!g_phys_native6698.attackPrepared&&begins117==starts+1&&commits117==ends+1&&releaseInput117==18,
    "C4R2 refined forward stab commits the original heavy preparation exactly once");
 gate59.mask=mask59;gate59.until[0]=until59;gate59.session[0]=session59;
 cc::sourceMask=source59;cc::until=ccUntil59;cc::session=ccSession59;
 command=savedCommand;g_combat_experiment175=oldExperiment;g_combat_integrated178r22c2=oldIntegrated;
 g_synchronized59c10r9=oldSync;g_intent59c16=oldIntent;g_thrust_as_swing55=oldHeavy;idle();
}
