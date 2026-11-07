// Actual complete production input consumer and begin observer; native bodies
// explicitly replaced. H15 sample124714 prepare ->124718 pending block release.
{
 g_combat_experiment175=true;g_combat_integrated178r22c2=true;
 g_guard_manual112=g_guard_physical112=false;g_native_guard112={};
 g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;
 g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=false;
 animation_filter::nativeBodyOwned112=false;
 g_weapon_control_mode=7;g_census_mode=3;g_phys_armed6698=true;g_phys_fault6698=false;
 g_pose_epoch6689=g_weapon_request_epoch6694=1;
 kcd2vr::retail_build112::hostRetailVerified112=true;
 diagnostics::lab::Set(diagnostics::lab::Key::F3,0);
 g_input_contract117=base;g_native_contract104=base;g_attack_require_opponent59c10=false;
 g_henry_item_attach_manager=manager.data();g_last_player_entity=entity.data();
 g_input_begin117=&BeginLeafR18;model117=route117=realBeginR18=true;reenterBeginR18=false;
 g_combat_bypass101=&action;g_combat_input_original101=&action;
 put(control117.data()+0x18,actor.data());put(hands.data()+0x260,std::uint64_t(77));
 frame.player=player.data();frame.manager=manager.data();frame.request=1;
 frame.input.allowed=frame.input.coherentMelee119=frame.input.naturalCombat117=true;
 frame.input.bodyEpoch=1;frame.input.attacks=true;frame.input.tracked[0]=frame.input.tracked[1]=true;
 frame.profiles[0].key=77;frame.profiles[0].generation=4;frame.output.active[0]=frame.output.owned[0]=true;
 command={};command.side=0;command.key=77;command.generation=4;command.epoch=command.bodyEpoch=1;
 command.stroke117=ph::CommandKind::Slash;command.zone117=1;command.direction={1,0,0};
 auto beginR18=[&](){
  command.generation=4;command.held57=false;
  ++hostTickCount64;command.tick=hostTickCount64;command.gesture59c3=++frame.input.sample;
  command.sample=frame.input.sample;frame.input.tick=hostTickCount64;
  animation_filter::combatPoseTick112=hostTickCount64;
  put(state.data()+0x38,1);put(state.data()+0x218,-1);put(state.data()+0x248,-1);
  put(state.data()+0x278,0);put(state.data()+0x1B8,1);
  put(state.data()+0x2A8,std::uint8_t(0));put(state.data()+0x2D8,std::uint8_t(0));put(state.data()+0x308,std::uint8_t(0));
  g_phys_native6698={};g_phys_native6698.player=player.data();g_phys_native6698.epoch=g_phys_native6698.request=1;
  g_phys_native6698.actionNames[0]=reinterpret_cast<std::uint64_t>(prep);
  g_phys_native6698.actionNames[1]=reinterpret_cast<std::uint64_t>(primary);
  g_phys_native6698.actionNames[2]=reinterpret_cast<std::uint64_t>(abort);
  primaryLatch117=0;command.kind=ph::CommandKind::Prepare;
  const auto old=realBeginCallsR18;
  ck(NativePhysicalAction117(player.data(),command,frame,true)&&realBeginCallsR18==old+1,
     "R18 real original begin observer sees one owned preparation");
  auto& n=g_phys_native6698;n.attackPrepared=true;n.attackKey=77;n.attackGeneration117=4;
  n.stroke117=command.stroke117;n.zone117=command.zone117;n.natural117=true;RememberPrepareC10R18(n,command);
  command.kind=ph::CommandKind::Slash;
  put(state.data()+0x38,128);put(state.data()+0x218,-1);put(state.data()+0x248,18);
  put(state.data()+0x278,7);put(state.data()+0x1B8,1);put(state.data()+0x2A8,std::uint8_t(1));
  put(state.data()+0x2D8,std::uint8_t(0));put(state.data()+0x308,std::uint8_t(0));
 };
 beginR18();nc104::Observed pendingR18{};NativeOwner104 ownerR18{};
 ck(NativeResolve104(player.data(),ownerR18)&&NativeObserve104(ownerR18,pendingR18)&&
    nc104::pendingBlockInputR18(pendingR18)&&!nc104::valid(pendingR18)&&!nc104::blockMetadataH14(pendingR18),
    "R18 pending metadata is readable context, never an attack sector or held guard grant");
 const auto first=commits117;
 ck(NativePhysicalAction117(player.data(),command,frame,false)&&commits117==first+1,
    "R18 exact original owned release reaches original selector from pending native block");
 g_phys_native6698.attackPrepared=false;
 ck(!NativePhysicalAction117(player.data(),command,frame,false)&&commits117==first+1,
    "R18 no prepared owner means no duplicate release");
 for(int bad=0;bad<8;++bad){
  beginR18();const auto old=commits117;
  if(bad==0)++command.gesture59c3;
  if(bad==1)++g_input_source_generation_h14;
  if(bad==2)++command.generation;
  if(bad==3)hostTickCount64+=101;
  if(bad==4)command.kind=ph::CommandKind::Prepare;
  if(bad==5)put(state.data()+0x38,32);
  if(bad==6)g_phys_native6698.heldPrepare57=true;
  if(bad==7)put(state.data()+0x248,17);
  ck(!NativePhysicalAction117(player.data(),command,frame,bad==4)&&commits117==old,
     "R18 mismatched gesture/source/item, stale, prepare, protected32, held or other input refuses");
 }
 reenterBeginR18=true;beginR18();reenterBeginR18=false;
 ck(!g_prepared_input_r18.source&&!NativePhysicalAction117(player.data(),command,frame,false),
    "R18 reentrant original begin cannot publish stable preparation ownership");
 // Exercise the production ordering, not just its final input leaf. A newer
 // completed XR frame may end the recognizer while the SAME original release
 // waits behind the preparing block row; an offhand shield must not abort it.
 const auto savedMetaH17=meta;
 const bool savedGeometryH17=combat_diag_probe::hostGeometry59c3;
 auto queuedShieldH17=[&](){
  g_guard_manual112=g_guard_physical112=false;g_native_guard112={};
  g_f4_fists_tick178r22c2=0;
  g_synchronized59c10r9=true;g_intent59c16=false;g_physical_combat177=true;
  g_phys_input6698=true;g_phys_abort6698=false;combat_diag_probe::hostGeometry59c3=false;
  frame.input.positionGuard176=true;frame.input.motionOwned59c5=false;
  frame.input.grip[0]=frame.input.grip[1]=1;
  frame.input.epoch=frame.request=1;frame.output.handlingOnly55c1[0]=false;
  frame.profiles[0].geometry=true;frame.profiles[0].primary=true;frame.profiles[0].side=0;frame.profiles[0].classId=4;
  frame.profiles[1]=frame.profiles[0];frame.profiles[1].key=88;frame.profiles[1].side=1;frame.profiles[1].classId=8;
  frame.profiles[1].tip={0,1,0};frame.output.weapons[1]={{0,0,0,1},{0,.4,.1}};
  frame.profiles[1].grip[1]={};frame.input.shoulders[0]=frame.input.shoulders[1]={};
  frame.output.active[1]=frame.output.owned[1]=true;
  frame.output.strokeActive119=false;frame.output.primaryStrokeGesture19=0;
  frame.output.guardBySide30c1[0]=false;frame.output.guardBySide30c1[1]=true;frame.output.guard117=true;
  put(hands.data()+0x268,std::uint64_t(88));put(state.data()+0xCF8,static_cast<void*>(nullptr));
  put(manager.data()+0x40,0);put(manager.data()+0x48,std::uint64_t(77));
  beginR18();
  g_phys_native6698.prepareSample117=command.sample;
  ++hostTickCount64;++frame.input.sample;frame.input.tick=hostTickCount64;
  animation_filter::combatPoseTick112=hostTickCount64;
  g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
  meta.metadataAt=hostTickCount64;PhysReadLive103(meta.last);
  meta.profiles={};meta.profiles.valid=true;meta.profiles.player=player.data();meta.profiles.manager=manager.data();
  meta.profiles.tick=hostTickCount64;meta.profiles.request=meta.profiles.epoch=1;meta.profiles.selected=0;
  meta.profiles.handKey[0]=77;meta.profiles.handKey[1]=88;
  meta.profiles.handKnown[0]=meta.profiles.handKnown[1]=true;
  meta.profiles.heldIndex[0]=0;meta.profiles.heldIndex[1]=1;meta.profiles.count=2;
  for(int side=0;side<2;++side){meta.profiles.items[side].profile=frame.profiles[side];meta.profiles.items[side].held=true;}
  g_phys_commands6698.head=g_phys_commands6698.tail=0;g_phys_commands6698.push(command);
 };
 queuedShieldH17();const auto shieldEndsH17=commits117;
 ck(ph::natural_guard117(frame.input,frame.profiles[1],frame.output.weapons[1],false)&&!FistSelected101(),
    "H17 ordered handoff fixture has an actual raised shield and weapon rather than fist ownership");
 NativeFrameBegin104();
 {NativeBlockGuardFrameH14 sameFrameH17;NativeGuardTick112();PhysNativeTickImpl6698();}
 ck(commits117==shieldEndsH17+1&&!g_phys_native6698.attackPrepared&&
    g_phys_commands6698.head==g_phys_commands6698.tail&&!g_guard_physical112,
    "H17 GuardTick then real consumer preserves exact queued release past raised offhand shield");
 NativeFrameBegin104();
 {NativeBlockGuardFrameH14 sameFrameH17;NativeGuardTick112();PhysNativeTickImpl6698();}
 ck(commits117==shieldEndsH17+1,"H17 ordered shield handoff cannot replay the consumed release");
 for(int badH17=0;badH17<4;++badH17){
  queuedShieldH17();const auto endsH17=commits117;
  if(badH17==0)g_prepared_input_r18.owner.actor=nullptr;
  if(badH17==1)++g_phys_commands6698.q[g_phys_commands6698.tail.load()].gesture59c3;
  if(badH17==2)g_prepared_input_r18.tick=hostTickCount64-101; // command/frame remain fresh
  if(badH17==3)++g_input_source_generation_h14;
  NativeFrameBegin104();
  {NativeBlockGuardFrameH14 sameFrameH17;NativeGuardTick112();PhysNativeTickImpl6698();}
  ck(commits117==endsH17&&!g_phys_native6698.attackPrepared,
     "H17 ordered pending handoff refuses wrong owner/new gesture/expired original/source replacement");
 }
 meta=savedMetaH17;combat_diag_probe::hostGeometry59c3=savedGeometryH17;
 realBeginR18=false;g_prepared_input_r18={};
}
