{
 namespace mo=kcd2vr::motion59c4;
 const auto oldSelected=mo::selected.load(),oldInput=g_phys_input6698.load();
 const auto oldNative=g_phys_native6698;const auto oldMeta=meta;
 const auto oldQueue=g_phys_commands6698.q;const auto oldHead=g_phys_commands6698.head.load(),oldTail=g_phys_commands6698.tail.load();
 auto forwardAfterGuard=[&](){
  baseline176();mo::selected=true;frame.input.motionOwned59c5=true;frame.input.attacks=true;frame.input.naturalCombat117=true;
  frame.input.physicalCombat177=true;frame.output.handlingOnly55c1[0]=false;tick176(0);
  put(state.data()+0x38,128);put(state.data()+0x218,1);blockModes176.clear();
  // A new completed XR frame arrives AFTER the actual guard tick; do not run
  // GuardTick again before the real command consumer sees this Prepare.
  ++hostTickCount64;++frame.input.sample;frame.input.tick=hostTickCount64;
  frame.output.guardBySide30c1[0]=frame.output.guard117=false;
  frame.output.strokeActive119=true;frame.output.primaryStrokeGesture19=0xD1EC7400+frame.input.sample;
  animation_filter::combatPoseTick112=hostTickCount64;g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;
  ph::Command c{};c.kind=ph::CommandKind::Prepare;c.stroke117=ph::CommandKind::Slash;c.zone117=1;
  c.gesture59c3=frame.output.primaryStrokeGesture19;c.key=frame.profiles[0].key;c.generation=frame.profiles[0].generation;
  c.side=0;c.sample=frame.input.sample;c.tick=hostTickCount64;c.bodyEpoch=frame.input.bodyEpoch;c.epoch=frame.request;
  return c;
 };
 for(int bad=0;bad<19;++bad){auto c=forwardAfterGuard();auto f=frame;
  if(bad==0)g_guard_manual112=true,g_guard_manual_owner112=player.data();
  if(bad==1)g_native_guard112.owner.state=nullptr;
  if(bad==2)++g_native_guard112.generation;
  if(bad==3)f.output.guardBySide30c1[0]=true;
  if(bad==4)f.output.strokeActive119=false;
  if(bad==5)++c.gesture59c3;
  if(bad==6)++c.key;
  if(bad==7)++c.generation;
  if(bad==8)++c.bodyEpoch;
  if(bad==9)++c.epoch;
  if(bad==10)c.tick=hostTickCount64-101;
  if(bad==11)f.input.tick=hostTickCount64-81;
  if(bad==12)++g_phys_ready_sample6698;
  if(bad==13)g_right_valid=false;
  if(bad==14)g_ctx_menu=true;
  if(bad==15)animation_filter::nativeBodyOwned112=true;
  if(bad==16)c.kind=ph::CommandKind::Thrust;
  if(bad==17)c.held57=true;
  if(bad==18)f.output.handlingOnly55c1[0]=true;
  NativeDirectPrepareGuardHandoff19c4r2(c,f,hostTickCount64);
  ck(edges176().empty()&&g_guard_release_tick59c7e.load()==0,
     "C4R2 DIRECT Prepare handoff refuses manual/foreign/stale/guarding/protected/non-onset ownership");
  g_ctx_menu=false;g_right_valid=true;animation_filter::nativeBodyOwned112=false;
 }
 auto c=forwardAfterGuard();ck(g_native_guard112.held,"C4R2 publication race begins with actual previous-frame held guard");
 g_phys_native6698.attackPrepared=false;g_phys_input6698=true;g_phys_abort6698=false;
 meta.metadataAt=hostTickCount64;PhysReadLive103(meta.last);meta.profiles=savedEquipment176;
 meta.profiles.tick=hostTickCount64;meta.profiles.request=frame.request;meta.profiles.epoch=frame.input.bodyEpoch;
 meta.profiles.player=player.data();meta.profiles.manager=manager.data();meta.profiles.handKey[0]=77;meta.profiles.handKey[1]=0;
 meta.profiles.heldIndex[0]=0;meta.profiles.heldIndex[1]=-1;meta.profiles.handKnown[0]=meta.profiles.handKnown[1]=true;
 meta.profiles.count=1;meta.profiles.items[0].profile=frame.profiles[0];meta.profiles.items[0].held=true;meta.profiles.valid=true;
 mo::attempts.clear();g_phys_commands6698.head=g_phys_commands6698.tail=0;g_phys_commands6698.push(c);
 static unsigned observedBeforeEvent=0;observedBeforeEvent=0;
 combat_diag_probe::hostVrStateEventObserver19c4r2=[](){
  if(!g_native_guard112.held&&g_guard_release_state59c7e.load()&&g_guard_release_tick59c7e.load())++observedBeforeEvent;
 };
 const auto begins=begins117,commits=commits117;NativeFrameBegin104();PhysNativeTickImpl6698();
 ck(edges176()==std::vector<int>{2}&&GuardOwnedByUs59c7e(state.data())&&observedBeforeEvent==1&&
    begins117==begins&&commits117==commits,
    "C4R2 actual DIRECT consumer releases old-frame guard before VR phase event, without native ordinary attack");
 const auto witness=g_guard_release_tick59c7e.load();hostTickCount64+=10;
 NativeDirectPrepareGuardHandoff19c4r2(c,frame,hostTickCount64);NativeFrameBegin104();PhysNativeTickImpl6698();
 ck(g_guard_release_tick59c7e.load()==witness&&edges176()==std::vector<int>{2}&&observedBeforeEvent==1,
    "C4R2 repeated consumer/Prepare cannot renew guard witness or repeat input/event");
 combat_diag_probe::hostVrStateEventObserver19c4r2=nullptr;
 NativeGuardRelease112();mo::attempts.clear();mo::selected=oldSelected;g_phys_input6698=oldInput;
 g_phys_native6698=oldNative;meta=oldMeta;g_phys_commands6698.q=oldQueue;g_phys_commands6698.head=oldHead;g_phys_commands6698.tail=oldTail;
}
