// Execute the actual production native-frame consumer, not just Delivery.
{
 namespace kr=kcd2vr::kick5916c3;
 const auto savedFrame=frame;const auto savedCommand=command;
 const bool savedExperiment=g_combat_experiment175.load();g_combat_experiment175=true;
 NativeGuardRelease112();PhysCancelNative6698();kr::delivery.cancel();
 frame.output.guard117=false;frame.output.strokeActive119=false;
 frame.input.heldMotionAttacks=false;frame.input.requirePrimaryGrip114=true;
 frame.input.attacks=true;frame.input.naturalCombat117=frame.input.coherentMelee119=true;
 command.kick59=true;command.held57=false;command.zone117=3;command.stroke117=ph::CommandKind::Slash;
 auto publishKick=[&](){
  ++hostTickCount64;++frame.input.sample;frame.input.tick=hostTickCount64;
  animation_filter::combatPoseTick112=hostTickCount64;meta.metadataAt=hostTickCount64;
  PhysReadLive103(meta.last);g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
  command.sample=frame.input.sample;command.tick=hostTickCount64;
 };
 auto prepareKick=[&](){
  idle();PhysAbortAttack117(player.data());g_phys_native6698.attackPrepared=false;kr::delivery.cancel();
  command.kind=ph::CommandKind::Prepare;publishKick();
  command.kickTicket59c3={kr::delivery.highest+1,hostTickCount64,
   {reinterpret_cast<std::uintptr_t>(player.data()),command.key,command.generation,command.bodyEpoch,frame.request}};
  g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 };
 for(bool held:{false,true}){
  frame.input.grip[0]=held?1.f:0.f;frame.output.active[0]=frame.output.owned[0]=held;
  const auto starts=begins117,ends=commits117;prepareKick();
  ck(g_phys_native6698.attackPrepared&&kr::delivery.prepared&&begins117==starts+1&&commits117==ends,
     "R3 preparation survives one native frame with Grip either open or held");
  command.kind=ph::CommandKind::Slash;publishKick();g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
  ck(!g_phys_native6698.attackPrepared&&!kr::delivery.prepared&&commits117==ends+1,
     "R3 sends exactly one next-frame release without requiring primary Grip");
  publishKick();g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
  ck(commits117==ends+1&&!g_phys_native6698.attackPrepared,
     "duplicate R3 ticket cannot restore preparation or produce a second release");
 }
 frame.input.grip[0]=0;frame.output.active[0]=frame.output.owned[0]=false;
 for(int invalid=0;invalid<3;++invalid){
  prepareKick();const auto ends=commits117;
  if(invalid==0)hostTickCount64+=kr::lifetime+1;
  if(invalid==1)frame.input.tracked[0]=false;
  if(invalid==2)++frame.profiles[0].generation;
  command.kind=ph::CommandKind::Slash;publishKick();g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
  ck(!g_phys_native6698.attackPrepared&&!kr::delivery.prepared&&commits117==ends,
     "R3 open-Grip exception still cancels on deadline, tracking loss or owner generation change");
  frame.input.tracked[0]=true;frame.profiles[0]=savedFrame.profiles[0];
 }
 // Native selection is not emulated by this fixture. Explicitly seed the
 // observed property to exercise honest reporting of both possible results.
 for(int type:{-1,1,4}){
  idle();PhysAbortAttack117(player.data());command.kind=ph::CommandKind::Prepare;publishKick();
  command.kickTicket59c3.id=10000+unsigned(type+1);put(state.data()+0x218,type);
  const auto count=kickTraceLines59c10.size();
  ck(NativePhysicalAction117(player.data(),command,frame,true),"R3 receipt fixture dispatches through the typed native input adapter");
  const char* expected=type<0?"NO_TYPE_OBSERVED":type==4?"KICK_TYPE_OBSERVED":"OTHER_TYPE_OBSERVED";
  ck(kickTraceLines59c10.size()==count+1&&kickTraceLines59c10.back().find(expected)!=std::string::npos&&
     kickTraceLines59c10.back().find("request="+std::to_string(command.kickTicket59c3.id))!=std::string::npos&&
     kickTraceLines59c10.back().find("nativeActionOrHit=NOT_INFERRED")!=std::string::npos,
     "per-ticket receipt separates native type observation from action/hit success");
 }
 PhysAbortAttack117(player.data());g_phys_native6698.attackPrepared=false;kr::delivery.cancel();
 frame=savedFrame;command=savedCommand;g_combat_experiment175=savedExperiment;idle();publish117();
}
