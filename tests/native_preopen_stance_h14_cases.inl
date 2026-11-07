// Pure policy plus the actual production NativePhysicalAction117 caller. Native
// input leaves are counted, not a model of accepted master strikes or damage.
{
 namespace ps14=kcd2vr::preopen_stance_h14;
 ps14::In policy14{};policy14.keepStance=policy14.current=policy14.writable=true;
 policy14.count=0;policy14.stance=policy14.origin=5;policy14.offered=0;
 policy14.mode=policy14.perk=1;policy14.stroke=mg47::Stroke::Thrust;policy14.energy=.6f;
 ck(ps14::preserve(policy14),"H14 zero-count physical lower counter retains an already correct native stance without opening a window");
 {auto owned14=policy14;owned14.writable=false;owned14.ownedMetadata=true;
  ck(ps14::preserve(owned14),"H14 exact caller-owned metadata may retain a measured stance without granting native direction-write permission");
  owned14.ownedMetadata=false;ck(!ps14::preserve(owned14),"H14 foreign metadata without a current stack permit cannot retain a counter stance");}
 for(int deny14=0;deny14<15;++deny14){auto bad14=policy14;
  switch(deny14){case 0:bad14.keepStance=false;break;case 1:bad14.current=false;break;
   case 2:bad14.writable=false;break;case 3:bad14.held=true;break;case 4:bad14.kick=true;break;
   case 5:bad14.count=-1;break;case 6:bad14.count=1;break;case 7:bad14.mode=0;break;
   case 8:bad14.mode=2;break;case 9:bad14.perk=0;break;case 10:bad14.perk=-1;break;
   case 11:bad14.stroke=mg47::Stroke::Unknown;break;case 12:bad14.stroke=mg47::Stroke::Pommel;break;
   case 13:bad14.stance=2;break;case 14:bad14.offered=2;break;}
  ck(!ps14::preserve(bad14),"H14 pre-open policy never invents missing current ownership, side, supported technique, perk or count");
 }
 const auto savedFrame14=frame;const auto savedCommand14=command;
 const auto savedCapture14=diagnostics::capture168::gate.mask.load();
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,GetTickCount64());
 const auto savedReadySample14=g_phys_ready_sample6698.load();
 std::array<std::array<unsigned char,16>,3> savedPins14{};unsigned pinIndex14=0;
 for(const auto& pin14:kcd2vr::native_master23::pins){
  std::memcpy(savedPins14[pinIndex14++].data(),image.data()+pin14.rva,pin14.size);
  std::memcpy(image.data()+pin14.rva,pin14.bytes,pin14.size);
 }
 g_master_checked23=0;
 const bool savedKeep14=g_ms_keep_stance59c17.load(),savedGate14=g_master_gate_ready47.load(),savedRequire14=g_attack_require_opponent59c10.load();
 const int savedMode14=mg47::mode.load();const float savedEnergy14=mg47::minEnergy.load();
 void* savedSoul14=nullptr;void* savedOpponent14=nullptr;
 (void)Read(player.data()+0x668,savedSoul14);(void)Read(state.data()+0xCF8,savedOpponent14);
 std::array<unsigned char,0x400> soul14{};mg47::AbilityRow ability14{mg47::kMasterStrikeAbility,0,1};
 std::array<unsigned char,0x500> foeActor14{};std::array<unsigned char,0xE00> foeState14{};
 std::array<unsigned char,0x100> foeEntity14{};
 put(soul14.data(),base+0x3F44F28);put(soul14.data()+0x320,&ability14);put(soul14.data()+0x328,std::uint64_t(1));
 put(player.data()+0x668,soul14.data());put(state.data()+0xCF8,foeActor14.data());
 put(foeActor14.data(),base+0x3E3FAC8);put(foeActor14.data()+0x210,foeState14.data());put(foeActor14.data()+0x1F8,foeEntity14.data());
 put(foeState14.data()+0xCE0,foeActor14.data());put(foeState14.data()+0xCF8,actor.data());
 put(foeState14.data()+8,std::uint8_t(1));put(foeState14.data()+0x38,4);put(foeState14.data()+0x218,1);
 put(foeState14.data()+0x278,1);put(foeState14.data()+0x248,-1);
 put(foeEntity14.data(),base+0x4072C78);put(foeEntity14.data()+0xC,std::uint32_t(854));
 auto reset14=[&](int origin14=5){
  idle();PhysAbortAttack117(player.data());primaryLatch117=0;g_phys_native6698.attackPrepared=false;
  put(state.data()+0x278,-1);put(state.data()+0x248,-1);put(state.data()+0x1B8,0);
  g_ms_keep_stance59c17=true;mg47::mode=1;mg47::minEnergy=.30f;g_master_gate_ready47=true;g_attack_require_opponent59c10=false;
  put(state.data()+0xF8,origin14);put(state.data()+0x7D8,0);
  put(soul14.data()+0x328,std::uint64_t(1));put(foeState14.data()+0xCF8,actor.data());
  put(foeState14.data()+0x38,4);put(foeState14.data()+0x158,mg47::masterZone(origin14));
  ++hostTickCount64;animation_filter::combatPoseTick112=hostTickCount64;
  frame=savedFrame14;frame.input.coherentMelee119=frame.input.naturalCombat117=true;
  frame.input.allowed=frame.input.tracked[0]=frame.input.tracked[1]=true;
  frame.input.attacks=true;frame.input.requirePrimaryGrip114=true;frame.input.grip[0]=1;
  frame.output.active[0]=frame.output.owned[0]=true;frame.output.handlingOnly55c1[0]=frame.output.handlingOnly55c1[1]=false;
  frame.output.solverOwnedT4[0]=frame.output.ownShieldBlocked60[0]=false;
  frame.output.invalidMask55c1[0]=0;frame.input.tick=hostTickCount64;++frame.input.sample;
  g_phys_ready_sample6698=frame.input.sample;
  command=savedCommand14;command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Thrust;
  command.zone117=2;command.originZone167=origin14;command.direction={0,0,1.8};
  command.held57=command.kick59=false;command.masterHandoff23=0;command.gesture59c3=frame.input.sample;
  command.sample=frame.input.sample;command.tick=hostTickCount64;command.bodyEpoch=frame.input.bodyEpoch;command.epoch=frame.request;
 };
 for(int origin14:{5,0}){reset14(origin14);const auto zones14=zoneCalls,begins14=begins117;masterTrace23.clear();
  ck(NativePhysicalAction117(player.data(),command,frame,true)&&begins117==begins14+1,
     "H14 actual caller dispatches the deliberate pre-open thrust exactly once through original native input");
  int stance14=-1,count14=-1;(void)Read(state.data()+0xF8,stance14);(void)Read(state.data()+0x7D8,count14);
  ck(zoneCalls==zones14&&stance14==origin14&&count14==0,
     "H14 actual caller preserves measured lower/high stance instead of lateral ordinary thrust announcement; native window remains zero");
  bool receipt14=false;for(const auto& line14:masterTrace23)if(line14.find("H14 PREOPEN_STANCE")!=std::string::npos&&
      line14.find("kept=1 before{count=0")!=std::string::npos&&line14.find("after{owner=1 count=0")!=std::string::npos&&
      line14.find("nativeAdmission=UNCLAIMED windowWrite=NONE")!=std::string::npos)receipt14=true;
  ck(receipt14,"H14 F3 receipt joins actual before/after native count and stance without declaring an admitted counter");
 }
 // Production caller, both techniques: preserve only the already measured opposite
 // origin; never write or open a native masterstrike window.
 for(auto kindR2:{ph::CommandKind::Slash,ph::CommandKind::Thrust}){
  reset14(1);command.stroke117=kindR2;command.direction={0,0,.1};
  const auto zonesR2=zoneCalls,beginsR2=begins117;
  ck(NativePhysicalAction117(player.data(),command,frame,true)&&begins117==beginsR2+1&&zoneCalls==zonesR2,
     "REPAIR2 cut and stab keep actual opposite origin with one native input and no secondary energy veto");
  int countR2=-1;Read(state.data()+0x7D8,countR2);
  ck(countR2==0,"REPAIR2 origin preservation never manufactures native masterstrike opening");
 }
 for(int deny14=0;deny14<9;++deny14){reset14();
  switch(deny14){case 0:g_ms_keep_stance59c17=false;break;case 1:mg47::mode=0;break;
   case 2:command.held57=true;break;case 3:command.originZone167=-1;break;
   case 4:put(soul14.data()+0x328,std::uint64_t(0));break;case 5:put(foeState14.data()+0xCF8,static_cast<void*>(nullptr));break;
   case 6:put(state.data()+0xF8,1);break;case 7:command.tick=hostTickCount64-101;break;
   case 8:put(state.data()+0x7D8,-1);break;}
  const auto zones14=zoneCalls;
  ck(NativePhysicalAction117(player.data(),command,frame,true)&&zoneCalls==zones14+1&&lastZone==2,
     "H14 actual caller leaves ordinary zone routing unchanged when a pre-open counter condition is absent");
 }
 auto ownGuard14=[&](){NativeOwner104 owner14{};(void)NativeResolve104(player.data(),owner14);
  g_native_guard112.owner=owner14;NativeGuardMarkHeldH14();g_native_guard112.key=command.key;
  g_native_guard112.side=0;g_native_guard112.generation=command.generation;
  g_guard_physical_owner112=player.data();g_guard_physical112=true;};
 auto ownedMetadata14=[&](int phase14=128){reset14();ownGuard14();
  put(state.data()+0x38,phase14);put(state.data()+0x218,-1);put(state.data()+0x248,-1);
  put(state.data()+0x278,7);put(state.data()+0x1B8,1);put(state.data()+0x2A8,std::uint8_t(0));
  put(state.data()+0x2D8,std::uint8_t(0));put(state.data()+0x308,std::uint8_t(0));};
 for(int phase14:{128,256}){ownedMetadata14(phase14);const auto zones14=zoneCalls,begins14=begins117,releases14=blockReleases117;
  const bool sent14=NativePhysicalAction117(player.data(),command,frame,true);int stance14=-1,count14=-1;
  Read(state.data()+0xF8,stance14);Read(state.data()+0x7D8,count14);
  ck(sent14&&begins117==begins14+1&&blockReleases117==releases14+1&&zoneCalls==zones14&&stance14==5&&count14==0,
     "H14 current metadata guard releases its LT and retains matching measured pre-open stance through the actual caller without a zone write or window award");
 }
 for(int changed14=0;changed14<6;++changed14){ownedMetadata14();
  blockCallback176=[&,changed14](int mode14){if(mode14!=2)return;
   if(changed14==0)NativeGuardMarkHeldH14(); // a re-entrant acquisition, even with the same native addresses
   if(changed14==1)put(foeState14.data()+0x158,2);
   if(changed14==2)put(hands.data()+0x260,std::uint64_t(997));
   if(changed14==3)put(actor.data()+0x210,static_cast<void*>(nullptr));
   if(changed14==4)g_weapon_request_epoch6694.fetch_add(1);
   if(changed14==5)g_phys_ready_sample6698.fetch_add(1);
  };
  const auto zones14=zoneCalls,begins14=begins117;const auto request14=g_weapon_request_epoch6694.load();
  ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==begins14&&zoneCalls==zones14,
     "H14 metadata no-write retention cannot cross another held lease, changed offered side, item, actor, request or current frame");
  put(hands.data()+0x260,command.key);put(actor.data()+0x210,state.data());g_weapon_request_epoch6694=request14;
 }
 reset14();ownGuard14();blockCallback176=[&](int mode14){if(mode14==2)put(foeState14.data()+0x158,2);};
 {const auto zones14=zoneCalls,begins14=begins117;
  ck(NativePhysicalAction117(player.data(),command,frame,true)&&zoneCalls==zones14+1&&lastZone==2&&begins117==begins14+1,
     "H14 guard-release callback changing offered side restores ordinary routing once, without retaining an old counter stance");}
 reset14();ownGuard14();blockCallback176=[&](int mode14){if(mode14==2)put(hands.data()+0x260,std::uint64_t(997));};
 {const auto begins14=begins117;
  ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==begins14,
     "H14 item replacement inside guard-release callback cannot dispatch under the old pre-open identity");}
 put(hands.data()+0x260,command.key);
 reset14();ownGuard14();blockCallback176=[&](int mode14){if(mode14==2)put(actor.data()+0x210,static_cast<void*>(nullptr));};
 {const auto begins14=begins117;
  ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==begins14,
     "H14 combat state replacement inside guard-release callback cannot dispatch under the old pre-open identity");}
 put(actor.data()+0x210,state.data());
 reset14();ownGuard14();blockCallback176=[&](int mode14){if(mode14==2)put(state.data()+0x38,64);};
 {const auto begins14=begins117,zones14=zoneCalls;
  ck(!NativePhysicalAction117(player.data(),command,frame,true)&&begins117==begins14&&zoneCalls==zones14,
     "H14 reaction entered during guard-release callback grants neither a new stance write nor native attack dispatch");}
 reset14();ownGuard14();blockCallback176=[&](int mode14){if(mode14==2)put(state.data()+0x7D8,1);};
 {const auto zones14=zoneCalls,begins14=begins117;int count14=-1;
  const bool sent14=NativePhysicalAction117(player.data(),command,frame,true);(void)Read(state.data()+0x7D8,count14);
  ck(sent14&&zoneCalls==zones14&&begins117==begins14+1&&count14==1,
     "H14 actual native opening during guard-release callback reaches the existing selector without replay or counter writes");}
 PhysAbortAttack117(player.data());idle();put(state.data()+0x7D8,0);put(state.data()+0xCF8,savedOpponent14);put(player.data()+0x668,savedSoul14);
 g_ms_keep_stance59c17=savedKeep14;g_master_gate_ready47=savedGate14;g_attack_require_opponent59c10=savedRequire14;
 mg47::mode=savedMode14;mg47::minEnergy=savedEnergy14;frame=savedFrame14;command=savedCommand14;
 g_phys_ready_sample6698=savedReadySample14;pinIndex14=0;
 diagnostics::capture168::gate.mask=savedCapture14;
 for(const auto& pin14:kcd2vr::native_master23::pins)std::memcpy(image.data()+pin14.rva,savedPins14[pinIndex14++].data(),pin14.size);
 g_master_checked23=0;
}
