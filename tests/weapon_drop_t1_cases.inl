// Actual production native adapter + existing native drop pose/dispatch hooks.
// Only native calling leaves and native memory are synthetic, never ownership.
{
 auto setupT1=[](){
  reset();g_owner={};g_pick13={};g_state_pub=Idle;g_mode=Off;g_installed=true;
  g_weapon_drop_fault_t1=false;for(auto& p:g_weapon_drop_t1)p={};
  for(auto& state:g_drop_inbox_state_t9)state=0;g_drop_context_t9.publish(DropContextT9{});
  for(auto& active:g_drop_native_active_t9)active=false;
  dropFixtureT1=true;inventoryAckT1=equipmentAckT1=true;bodyReadyT1=false;spinCallsT1=0;
  const auto b=WhBase();frameworkT1.fill(0);moduleT1.fill(0);inventoryManagerT1.fill(0);
  definitionT1.fill(0);weaponClassT1.fill(0);rigidT1.fill(0);
  put(image.data(),0x549D388,static_cast<void*>(frameworkT1.data()));
  put(frameworkT1.data(),0xE0,static_cast<void*>(moduleT1.data()));
  put(moduleT1.data(),0xE8,static_cast<void*>(inventoryManagerT1.data()));
  put(inventoryManagerT1.data()+0x100070,0,b+0x3A83DC0);put(image.data(),0x3A83DC0+8,b+0x1A74A40);
  put(item.data(),0x60,unsigned(1));put(item.data(),0x90,static_cast<void*>(&inventory));
  put(item.data(),0x48,static_cast<void*>(definitionT1.data()));put(definitionT1.data(),0,b+0x3A4E390);
  put(definitionT1.data(),0xE8,static_cast<void*>(weaponClassT1.data()));put(weaponClassT1.data(),8,int(3));
  put(manager.data(),0x48,key);put(slots.data(),0x260,key);put(attachment.data(),0x20,static_cast<void*>(attachmentObject.data()));
  put(rigidT1.data(),0,b+0x3A48DC0);put(rigidT1.data(),0x20,static_cast<void*>(entity.data()));put(rigidT1.data(),0x28,std::int16_t(1));
  put(image.data(),0x3A48DC0+0x30,b+0x8A6C90);put(image.data(),0x3A48DC0+0x38,b+0xAC0D18);
  const unsigned char ap[]{0x48,0x8b,0xc4,0x48,0x89,0x58,0x18,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56};
  const unsigned char sp[]{0x48,0x8b,0xc4,0x55,0x53,0x56,0x57,0x41,0x56,0x48,0x8d,0x68,0xc8,0x48,0x81,0xec};
  std::memcpy(image.data()+0xAC0D18,ap,sizeof(ap));std::memcpy(image.data()+0x8A6C90,sp,sizeof(sp));
  kcd2vr::collab_h21::setThrowing(true);
  wrt1::SetMode(1);wrt1::Request r{};r.throwEpochH21=kcd2vr::collab_h21::throwEpoch.load();r.physicsEpochH21=kcd2vr::collab_h21::epoch.load();r.manager=reinterpret_cast<std::uintptr_t>(manager.data());
  r.player=reinterpret_cast<std::uintptr_t>(player.data());r.character=reinterpret_cast<std::uintptr_t>(character.data());
  r.attachment=reinterpret_cast<std::uintptr_t>(attachment.data());r.key=key;r.generation=7;r.bodyEpoch=9;r.requestEpoch=10;
  r.sample=1000;r.tick=1000;r.motionTick=1000;r.entity=72;r.side=0;r.classId=3;r.motion=true;
  r.lastAllowed.p[0]=30;r.lastAllowed.p[1]=40;r.lastAllowed.p[2]=50;r.linear[0]=2;r.angular[2]=5;return r;
 };
 // T13: real inline drop completion must finish in this same native owner
 // transaction, even if the next animation Update arrives447ms later.
 auto inlineRequestT13=setupT1();dropAck=false;bodyReadyT1=true;
 kcd2vr::collab_h21::setThrowing(false);assert(!DropWeaponT1(inlineRequestT13).invoked&&drops==0);
 kcd2vr::collab_h21::setThrowing(true);assert(!DropWeaponT1(inlineRequestT13).invoked&&drops==0);
 inlineRequestT13.throwEpochH21=kcd2vr::collab_h21::throwEpoch.load(); // a NEW deliberate release only
 const auto transferBeforeT13=g_drop_transfer_serial_t13.load();
 auto inlineResultT13=DropWeaponT1(inlineRequestT13);
 assert(inlineResultT13.acknowledged()&&!inlineResultT13.queued&&spinCallsT1==1&&!g_weapon_drop_t1[0].active);
 assert(lastSpinT1.v[0]==2&&lastSpinT1.v[1]==0&&lastSpinT1.v[2]==0&&lastSpinT1.w[2]==5);
 DropTransferT13 transferT13{};assert(g_drop_transfer_t13[0].read(transferT13)&&transferT13.serial>transferBeforeT13&&transferT13.source.key==key);
 hostTickCount64=1447;TickWeaponDropT1(true,inlineRequestT13.player,9,10);
 assert(drops==1&&spinCallsT1==1&&g_drop_transfer_serial_t13.load()==transferT13.serial);hostTickCount64=1000;
 // A committed inventory release publishes stock advancement even when the
 // physics body is unavailable, without pretending it is a completed flight.
 auto inventoryOnlyT13=setupT1();dropAck=false;bodyReadyT1=false;
 const auto beforeInventoryT13=g_drop_transfer_serial_t13.load();
 assert(DropWeaponT1(inventoryOnlyT13).acknowledged()&&spinCallsT1==0);
 assert(g_drop_transfer_t13[0].read(transferT13)&&transferT13.serial>beforeInventoryT13);
 hostTickCount64=1447;TickWeaponDropT1(true,inventoryOnlyT13.player,9,10);assert(spinCallsT1==0&&!g_weapon_drop_t1[0].active);hostTickCount64=1000;
 auto r=setupT1();
 // Recorder/pickup mode OFF must still discover Henry's exact native owner.
 g_henry_ctrl=nullptr;g_owner_thread13=0;g_tapped_skel=nullptr;testAttachManager=manager.data();traceEnabled=false;
 UpdateDetour(controller.data(),.01f);
 assert(g_henry_ctrl.load()==controller.data()&&g_owner_thread13.load()==1&&g_mode==Off&&g_owner.state==Idle&&!NativeGraspEnabled());
 auto result=DropWeaponT1(r);
 assert(result.acknowledged()&&result.queued&&drops==1&&releasedVelocity[0]==2&&g_mode==Off);
 // Existing linked world body can predate queued physicalization. Never treat
 // positive mass/inventory ACK as a completion receipt or add spin too early.
 bodyReadyT1=true;TickWeaponDropT1(true,r.player,9,10);assert(spinCallsT1==0&&g_weapon_drop_t1[0].active);
 DropDispatchT2 dispatch{};assert(g_drop_dispatch_t2[0].read(dispatch)&&dispatch.serial==g_weapon_drop_t1[0].dispatchSerial);
 g_drop_consumer_original_t2=+[](const void*){};
 auto wrong=dispatch;wrong.record[8]^=1;DropConsumerDetourT2(wrong.record);
 TickWeaponDropT1(true,r.player,9,10);assert(spinCallsT1==0&&g_weapon_drop_t1[0].active);
 DropConsumerDetourT2(dispatch.record);TickWeaponDropT1(true,r.player,9,10);
 assert(spinCallsT1==1&&drops==1&&!g_weapon_drop_t1[0].active);
 wrt1::CommittedT2 committed{};assert(ReadCommittedDropT2(committed)&&committed.source.key==r.key&&committed.consumerSerial==dispatch.serial&&committed.body==reinterpret_cast<std::uintptr_t>(rigidT1.data()));
 DropConsumerDetourT2(dispatch.record);TickWeaponDropT1(true,r.player,9,10);assert(spinCallsT1==1); // no duplicate spin/lease from duplicate completion
 r=setupT1();dropAck=false;result=DropWeaponT1(r);assert(result.acknowledged()&&!result.queued&&drops==1);
 TickWeaponDropT1(true,r.player,9,10);assert(spinCallsT1==0&&g_weapon_drop_t1[0].active);
 bodyReadyT1=true;TickWeaponDropT1(true,r.player,9,10);
 assert(spinCallsT1==1&&spinThreadFlagT1==0&&lastSpinT1.type==10&&lastSpinT1.w[2]==5);
 assert(lastSpinT1.v[0]==2&&lastSpinT1.v[1]==.4f&&lastSpinT1.v[2]==-.2f); // preserve CURRENT COM v, no second actor/cross term
 TickWeaponDropT1(true,r.player,9,10);assert(spinCallsT1==1&&drops==1&&!DropWeaponT1(r).invoked);
 r=setupT1();put(item.data(),0x50,unsigned(5));assert(!DropWeaponT1(r).invoked&&drops==0&&Rd<unsigned>(item.data(),0x50,0)==5);
 r=setupT1();put(item.data(),0x60,unsigned(3));assert(!DropWeaponT1(r).invoked&&drops==0);
 r=setupT1();r.classId=9;put(weaponClassT1.data(),8,int(9));assert(!DropWeaponT1(r).invoked&&drops==0&&Rd<std::uint64_t>(slots.data(),0x260,0)==key);
 r=setupT1();put(attachmentObject.data(),8,unsigned(73));assert(!DropWeaponT1(r).invoked&&drops==0);
 r=setupT1();put(entity.data(),0x58+12,float(31));assert(!DropWeaponT1(r).invoked&&drops==0); // native owner moved it away
 r=setupT1();r.tick=900;assert(!DropWeaponT1(r).invoked&&drops==0);
 r=setupT1();g_owner_thread13=99;assert(!DropWeaponT1(r).invoked&&drops==0);g_owner_thread13=1;
 r=setupT1();inventoryAckT1=false;result=DropWeaponT1(r);assert(result.invoked&&!result.acknowledged()&&drops==1);
 bodyReadyT1=true;TickWeaponDropT1(true,r.player,9,10);assert(spinCallsT1==0);
 hostTickCount64=1181;TickWeaponDropT1(true,r.player,9,10);assert(g_weapon_drop_fault_t1&&!DropWeaponT1(r).invoked&&drops==1);
 r=setupT1();equipmentAckT1=false;result=DropWeaponT1(r);assert(result.invoked&&!result.equipmentClear&&drops==1);
 bodyReadyT1=true;TickWeaponDropT1(true,r.player,9,10);assert(spinCallsT1==0);
 r=setupT1();dropObserved=false;result=DropWeaponT1(r);assert(result.invoked&&!result.dispatched&&g_weapon_drop_fault_t1&&drops==1);
 r=setupT1();result=DropWeaponT1(r);bodyReadyT1=true;TickWeaponDropT1(true,r.player,99,10);assert(spinCallsT1==0&&!g_weapon_drop_t1[0].active);
 r=setupT1();result=DropWeaponT1(r);bodyReadyT1=true;TickWeaponDropT1(false,r.player,9,10);assert(spinCallsT1==0&&!g_weapon_drop_t1[0].active);
 r=setupT1();dropAck=false;result=DropWeaponT1(r);bodyReadyT1=true;put(rigidT1.data(),0x20,static_cast<void*>(player.data()));
 TickWeaponDropT1(true,r.player,9,10);assert(spinCallsT1==0); // same body class, WRONG current foreign owner
 // T9: actual public frame caller -> actual Henry Update -> original native
 // adapter. A foreign caller cannot call Drop, ACK or physics directly.
 r=setupT1();testAttachManager=manager.data();pickupThread19c4=77;
 TickWeaponDropT1(true,r.player,9,10);result=DropWeaponT1(r);
 assert(result.ownerQueuedT9&&!result.invoked&&!result.acknowledged()&&drops==0&&spinCallsT1==0);
 assert(!DropWeaponT1(r).ownerQueuedT9&&drops==0); // bounded, no overwrite/retry
 UpdateDetour(controller.data(),.01f);assert(drops==0); // foreign Update cannot drain
 pickupThread19c4=1;UpdateDetour(controller.data(),.01f);assert(drops==1&&g_weapon_drop_t1[0].active);
 DropDispatchT2 ownerDispatch{};assert(g_drop_dispatch_t2[0].read(ownerDispatch));
 DropConsumerDetourT2(ownerDispatch.record);bodyReadyT1=true;
 pickupThread19c4=77;TickWeaponDropT1(true,r.player,9,10);assert(spinCallsT1==0);
 pickupThread19c4=1;UpdateDetour(controller.data(),.01f);assert(spinCallsT1==1&&!g_weapon_drop_t1[0].active);
 UpdateDetour(controller.data(),.01f);assert(drops==1&&spinCallsT1==1);
 // Load/context replacement, stale context, stale request, lost gameplay and
 // re-equipment all refuse once, retaining inventory and never replaying later.
 for(int changed=0;changed<8;++changed){
  r=setupT1();testAttachManager=manager.data();pickupThread19c4=77;
  TickWeaponDropT1(true,r.player,9,10);assert(DropWeaponT1(r).ownerQueuedT9);
  if(changed==0)TickWeaponDropT1(true,r.player,99,10);
  if(changed==1)TickWeaponDropT1(true,r.player,9,99);
  if(changed==2)hostTickCount64=1081;
  if(changed==3){hostTickCount64=1081;TickWeaponDropT1(true,r.player,9,10);}
  if(changed==4)TickWeaponDropT1(false,r.player,9,10);
  if(changed==5)wrt1::SetMode(0);
  if(changed==6)put(slots.data(),0x260,std::uint64_t(0));
  if(changed==7){wrt1::SetMode(0);wrt1::SetMode(1);}
  pickupThread19c4=1;UpdateDetour(controller.data(),.01f);
  assert(drops==0&&g_drop_inbox_state_t9[0].load()==0&&!g_weapon_drop_t1[0].active);
  hostTickCount64=1000;wrt1::SetMode(1);put(slots.data(),0x260,key);TickWeaponDropT1(true,r.player,9,10);
  UpdateDetour(controller.data(),.01f);assert(drops==0);
 }
 // T2 exact registered explicit-hand boundary: native inventory/unit checks,
 // actual hand plus visual ACK, opaque native key and scoped actual-right lease.
 r=setupT1();put(slots.data(),0x260,std::uint64_t(0));put(item.data(),0x60,unsigned(0));
 put(weaponClassT1.data(),8,int(0));SetThrowingSupplyT2(true);
 st2::Input input{};input.manager=r.manager;input.player=r.player;input.character=r.character;input.entity=42;
 input.body=9;input.request=10;input.tick=input.sample=1000;input.allowed=input.tracked=input.bone=true;input.grip=1;
 // T12: same exact native owner TU emits a lossless token above Lua's 2^53.
 SupplyVisualRefreshT12 refreshT12{};st2::Input visualT12{};visualT12.beltKey=key;
 assert(refreshT12.due(visualT12,true,1000,2000,false));
 for(unsigned t=1001;t<1050;++t){visualT12.beltShow=!visualT12.beltShow;assert(!refreshT12.due(visualT12,true,t,2000,false));}
 visualT12.beltShow=true;assert(refreshT12.due(visualT12,true,1050,2000,false));
 assert(!refreshT12.due(visualT12,true,1500,2000,false)); // no idle per-frame inventory scan
 assert(refreshT12.due(visualT12,false,1051,2000,true)); // clear cannot wait for throttle
 assert(refreshT12.due(visualT12,true,1052,2000,true)); // real draw edge cannot be replay-delayed
 char beltPacketT12[512]{};auto packetInputT12=input;packetInputT12.beltKey=0x02ffffffffffffffull;
 packetInputT12.beltClass=3;packetInputT12.beltShow=true;
 assert(SupplyPacketT2(packetInputT12,1,"refresh",0,beltPacketT12,sizeof(beltPacketT12))>0);
 assert(std::strstr(beltPacketT12,"|[Item-72057594037927935]|1|3|"));
 packetInputT12.beltKey=0x0300000000000001ull;
 SupplyPacketT2(packetInputT12,2,"refresh",0,beltPacketT12,sizeof(beltPacketT12));
 assert(std::strstr(beltPacketT12,"|-|1|3|")); // wrong handle type never aliases an item
 auto mainInputT12=input;mainInputT12.beltKey=key;
 assert(!SupplyInventoryUnitT2(mainInputT12,key,0)); // the normal main belt is not pouch stock
 static int ownDrawCallsT2=0;ownDrawCallsT2=0;
 g_supply_draw_original_t2=+[](void*,std::uint64_t k,int side)->bool {++ownDrawCallsT2;put(slots.data(),0x260+side*8,k);return true;};
 SupplyDrawScopeT2 scope{input,1,1,0,0};g_supply_draw_scope_t2=&scope;hostCaller=WhBase()+0x2AB9CD1;
 scope.vm=0;assert(!SupplyDrawDetourT2(manager.data(),key,0)&&ownDrawCallsT2==0&&!scope.submitted);
 scope={input,2,1,0,0};put(item.data(),0x50,unsigned(5));assert(!SupplyDrawDetourT2(manager.data(),key,0)&&ownDrawCallsT2==0);put(item.data(),0x50,unsigned(1));
 scope={input,3,1,0,0};put(item.data(),0x90,static_cast<void*>(&worldContainer));assert(!SupplyDrawDetourT2(manager.data(),key,0)&&ownDrawCallsT2==0);put(item.data(),0x90,static_cast<void*>(&inventory));
 scope={input,4,1,0,0};scope.input.tick=899;assert(!SupplyDrawDetourT2(manager.data(),key,0)&&ownDrawCallsT2==0);
 scope={input,5,1,0,0};assert(SupplyDrawDetourT2(manager.data(),key,0)&&ownDrawCallsT2==1&&scope.key==key&&scope.submitted);
 assert(!SupplyDrawDetourT2(manager.data(),key,0)&&ownDrawCallsT2==1); // reservation/request is one-shot
 st2::Lease actual{};put(attachment.data(),0x20,static_cast<void*>(nullptr));assert(!SupplyVisualAckT2(input,key,0,5,actual));
 put(attachment.data(),0x20,static_cast<void*>(attachmentObject.data()));assert(SupplyVisualAckT2(input,key,0,5,actual)&&actual.side==0&&actual.classId==0);
 input.hand[0]=key;assert(st2::owns(actual,input,key,0,1000)&&!st2::owns(actual,input,key,-1,1000)&&!st2::owns(actual,input,key,2,1000));
 g_supply_lease_t2.publish(actual);r.classId=0;assert(SupplyLeaseOwnsT2(r));
 put(item.data(),0x60,unsigned(0));void* ownItem=nullptr;void* ownWorld=nullptr;assert(DropIdentityT1(r,ownItem,ownWorld)); // exact supply held unit need not replace native equipment slots
 g_supply_draw_scope_t2=nullptr;hostCaller=0;SetThrowingSupplyT2(false);g_supply_lease_t2.publish(st2::Lease{});
 dropFixtureT1=false;wrt1::SetMode(0);for(auto& p:g_weapon_drop_t1)p={};g_weapon_drop_fault_t1=false;
 std::puts("PASS T2 real unit drop, exact queued consumer receipt before spin/flight lease, opaque WUID draw, native owner/quantity/VM/attachment checks, actual-right Class0 profile lease; native leaves substituted");
}
