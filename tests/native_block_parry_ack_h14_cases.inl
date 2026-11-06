// Included in the existing actual NativeGuardTick112 fixture. Explicit native
// Update context is separate from the animation/blocking byte. No PB awarded.
{
 const bool savedUpdateAckH14=blockReleaseUpdateH14;
 auto pendingAckH14=[&](int phaseH14=128){
  blockReleaseUpdateH14=true;put(state.data()+0x648,std::uint8_t(0));baseline176();
  g_guard_countermotion57c1=true;g_guard_incoming_repress45=true;
  put(foeState176.data()+0x38,2);tick176(0); // native initial LT press
  put(state.data()+0x38,phaseH14);put(state.data()+0x218,-1);put(state.data()+0x248,-1);
  put(state.data()+0x278,7);put(state.data()+0x1B8,1);put(state.data()+0x308,std::uint8_t(0));
  tick176(0);blockModes176.clear();blockReleaseUpdateH14=false;
  put(foeState176.data()+0x38,4);tick176(0);
  for(int nH14=1;nH14<=4;++nH14)tick176(.025f*nH14);
 };
 for(int phaseH14:{128,256}){pendingAckH14(phaseH14);
  const auto deadlineH14=g_native_guard112.pendingSince46;
  ck(edges176()==std::vector<int>{2}&&!g_native_guard112.held&&g_native_guard112.pendingPress46&&
     g_native_guard112.pendingBlockMetadataH14&&g_native_guard112.pendingMotionH5,
     "H14 real countermotion from metadata guard emits LT-up then waits actual native context ACK, despite +308false");
  for(int nH14=0;nH14<3;++nH14)tick176(.10f);
  ck(edges176()==std::vector<int>{2}&&g_native_guard112.pendingSince46==deadlineH14,
     "H14 pending release wait neither presses LT early nor renews the original150ms deadline");
  put(state.data()+0x648,std::uint8_t(0));tick176(.10f);
  ck(pair176()&&g_native_guard112.held&&!g_native_guard112.pendingPress46&&!g_native_guard112.pendingBlockMetadataH14&&
     pt::Read(edge176)&&edge176.cause==pt::Cause::Motion,
     "H14 original native Update ACK permits exactly one same-pair motion block input without fabricating a PB result");
  int resultH14=-1;Read(state.data()+0x680,resultH14);
  ck(resultH14==0,"H14 native perfect-block result stays unchanged by a delivered parry input");
 }
 for(int refuseH14=0;refuseH14<6;++refuseH14){pendingAckH14();
  ck(g_native_guard112.pendingBlockMetadataH14,"H14 refusal fixture begins with a real motion-qualified pending metadata transaction");
  if(refuseH14==0)put(state.data()+0x648,std::uint8_t(2));
  if(refuseH14==1)put(state.data()+0x630,std::uintptr_t(0));
  if(refuseH14==2)put(foeState176.data()+0x158,1);
  if(refuseH14==3)put(hands.data()+0x260,std::uint64_t(991));
  if(refuseH14==4){hostTickCount64=g_native_guard112.pendingSince46+151;put(state.data()+0x648,std::uint8_t(0));}
  if(refuseH14==5)g_gameplay_allowed=false;
  if(refuseH14==2||refuseH14==3)put(state.data()+0x648,std::uint8_t(0));
  tick176(.10f);
  if(refuseH14==2)ck(pair176()&&g_native_guard112.held&&pt::Read(edge176)&&edge176.cause==pt::Cause::Initial,
     "H14 changed incoming side restores only ordinary same-owner guard after native ACK, without parry attribution");
  else ck(edges176()==std::vector<int>{2}&&!g_native_guard112.held&&!g_native_guard112.pendingBlockMetadataH14,
     "H14 unknown ACK, changed item, expired deadline and context loss cannot issue the delayed metadata press");
  put(state.data()+0x630,base+0x3B0A968);put(hands.data()+0x260,std::uint64_t(77));g_gameplay_allowed=true;
 }
 // Timing attribution is bounded150ms. An unacknowledged original up remains
 // release-only after expiry, even if the native animation has become neutral.
 pendingAckH14();hostTickCount64=g_native_guard112.pendingSince46+151;
 put(state.data()+0x38,1);put(state.data()+0x278,-1);tick176(.10f);tick176(.10f);
 ck(edges176()==std::vector<int>{2}&&!g_native_guard112.held&&!g_native_guard112.pendingPress46&&g_native_guard112.releaseWaitH14,
    "H14 expired parry attribution cannot become a fresh initial down while native up remains unacknowledged");
 put(state.data()+0x648,std::uint8_t(2));tick176(.10f);
 ck(edges176()==std::vector<int>{2}&&g_native_guard112.releaseWaitH14,
    "H14 unknown release context preserves only the no-input fence after timing expires");
 put(state.data()+0x648,std::uint8_t(0));tick176(.10f);
 ck(pair176()&&g_native_guard112.held&&!g_native_guard112.pendingPress46&&!g_native_guard112.releaseWaitH14&&
    pt::Read(edge176)&&edge176.cause==pt::Cause::Initial,
    "H14 native Update ACK permits ordinary fresh guard after expiry without reviving its old motion attribution");
 // Legacy pure-motion mode uses the same release ACK, not an immediate down.
 blockReleaseUpdateH14=true;baseline176();g_guard_countermotion57c1=false;g_guard_incoming_repress45=false;
 settle176();blockModes176.clear();blockReleaseUpdateH14=false;motion176();
 ck(edges176()==std::vector<int>{2}&&g_native_guard112.pendingPress46&&g_native_guard112.releaseWaitH14,
    "H14 legacy sector motion cannot cancel queued native LT-up with a same-callback LT-down");
 tick176(.09f);ck(edges176()==std::vector<int>{2},"H14 legacy sector motion waits across a native frame before ACK");
 put(state.data()+0x648,std::uint8_t(0));tick176(.09f);
 ck(pair176()&&g_native_guard112.held&&pt::Read(edge176)&&edge176.cause==pt::Cause::Motion,
    "H14 legacy sector motion restores once only after native context0 ACK");
 blockReleaseUpdateH14=true;baseline176();g_guard_countermotion57c1=false;g_guard_incoming_repress45=false;
 g_combat_experiment175=false;diagnostics::lab::Set(diagnostics::lab::Key::F3,3);
 settle176();blockModes176.clear();blockReleaseUpdateH14=false;motion176();
 ck(edges176()==std::vector<int>{2}&&g_native_guard112.pendingPress46&&g_native_guard112.releaseWaitH14,
    "H14 alternate F3 translation motion retains a bounded ACK wait instead of canceling native LT-up");
 tick176(.09f);ck(edges176()==std::vector<int>{2},"H14 alternate translation cannot re-enter initial guard before original Update ACK");
 put(state.data()+0x648,std::uint8_t(0));tick176(.09f);
 ck(pair176()&&g_native_guard112.held,"H14 alternate translation restores one ordinary guard after native ACK");
 for(int sourceLossH14=0;sourceLossH14<4;++sourceLossH14){
  blockReleaseUpdateH14=true;baseline176();g_guard_countermotion57c1=false;g_guard_incoming_repress45=false;
  g_combat_experiment175=false;diagnostics::lab::Set(diagnostics::lab::Key::F3,3);settle176();blockModes176.clear();
  blockCallback176=[&,sourceLossH14](int mode){if(mode!=2)return;
   if(sourceLossH14==0)g_ctx_menu=true;
   if(sourceLossH14==1)animation_filter::nativeBodyOwned112=true;
   if(sourceLossH14==2){frame.output.guardBySide30c1[0]=false;g_phys_frame6698.publish(frame);}
   if(sourceLossH14==3){auto lost=tracked176;lost.left_held=true;g_tracking_frames6695.publish(lost);}
  };motion176();
  ck(edges176()==std::vector<int>{2}&&!g_native_guard112.held&&!g_native_guard112.pendingPress46,
     "H14 alternate translation revalidates menu/native body/current posture/both hands after original LT-up");
  g_ctx_menu=false;animation_filter::nativeBodyOwned112=false;
 }
 // The fence itself cannot travel to a replacement owner/item/source epoch.
 for(int changedH14=0;changedH14<5;++changedH14){pendingAckH14();
  NativeOwner104 exactH14{};NativeResolve104(player.data(),exactH14);auto replacedH14=frame;
  if(changedH14==0)++replacedH14.request;
  if(changedH14==1)++replacedH14.profiles[0].generation;
  if(changedH14==2)++replacedH14.profiles[0].key;
  if(changedH14==3)++replacedH14.input.bodyEpoch;
  if(changedH14==4)replacedH14.input.sample=g_native_guard112.releaseSampleH14-1;
  const auto callsH14=actionCalls;
  ck(!NativeGuardReleaseFenceH14(exactH14,replacedH14)&&!g_native_guard112.releaseWaitH14&&actionCalls==callsH14,
     "H14 release fence drops on replaced epoch/item/sample and never authorizes an input");
 }
 for(int routeH14=0;routeH14<3;++routeH14){
  blockReleaseUpdateH14=true;baseline176();g_guard_countermotion57c1=routeH14==0;g_guard_incoming_repress45=false;
  if(routeH14==2){g_combat_experiment175=false;diagnostics::lab::Set(diagnostics::lab::Key::F3,3);}
  put(foeState176.data()+0x38,1);settle176();blockModes176.clear();const auto oldLeaseH14=g_native_guard112.leaseH14;
  blockCallback176=[&](int mode){if(mode!=2)return;NativeGuardMarkHeldH14();g_guard_physical112=true;g_guard_physical_owner112=player.data();
      put(state.data()+0x308,std::uint8_t(1));put(state.data()+0x648,std::uint8_t(1));};
  put(foeState176.data()+0x38,4);tick176(0);for(int nH14=1;nH14<=4;++nH14)tick176(.025f*nH14);
  ck(edges176()==std::vector<int>{2}&&g_native_guard112.held&&g_native_guard112.leaseH14>oldLeaseH14&&
     !g_native_guard112.releaseWaitH14&&!g_native_guard112.pendingPress46,
     "H14 reentrant same-address newer guard lease is never fenced, cleared or re-pressed by the original release callback");
 }
 // Read-contract negatives remain unknown; no getter or native setter executes.
 NativeOwner104 ownerH14{};NativeResolve104(player.data(),ownerH14);
 put(state.data()+0x648,std::uint8_t(0));ck(NativeGuardReleaseAckH14(ownerH14)==0,"H14 exact native context0 zero is release ACK");
 image[0x1A7DDE0]^=1;ck(NativeGuardReleaseAckH14(ownerH14)==-1,"H14 altered getter fingerprint cannot authorize a parry press");image[0x1A7DDE0]^=1;
 put(image.data()+0x3B0A970,base+0x1A7DDE1);ck(NativeGuardReleaseAckH14(ownerH14)==-1,"H14 wrong exact getter slot cannot authorize a parry press");
 put(image.data()+0x3B0A970,base+0x1A7DDE0);
 blockReleaseUpdateH14=savedUpdateAckH14;put(state.data()+0x630,base+0x3B0A968);put(state.data()+0x648,std::uint8_t(0));
 put(state.data()+0x278,-1);put(state.data()+0x1B8,1);
}
