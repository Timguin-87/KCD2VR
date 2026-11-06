// C23 production bridge (included inside native_input117_cases.inl): master-window pins, the
// edge-only +7D8 observer in the exact-owner guard pump, the live PREPARE recheck of a
// handoff-tagged command and the live count at XI consumption. Native leaves are replaced;
// nothing here claims a master strike: D4AD94 is not executed.
{
 namespace nm=kcd2vr::native_master23;
 auto has=[&](const char* needle){for(const auto& l:masterTrace23)if(l.find(needle)!=std::string::npos)return true;return false;};
 auto msContract=[&](){return nm::contract(base,[](std::uintptr_t at,unsigned char* out,std::size_t n){std::memcpy(out,reinterpret_cast<void*>(at),n);return true;});};
 g_master_checked23=0;
 NativeOwner104 owner23{};ck(NativeResolve104(player.data(),owner23),"C23 fixture resolves exact Henry owner");
 int count23=-9;put(state.data()+0x7D8,1);
 ck(!msContract()&&!NativeMasterCount23(owner23,count23)&&count23==-1,"C23 an image without the D4ADBF/1899D0B/1899D33 pins is never read (fails closed)");
 for(const auto& pin:nm::pins)std::memcpy(image.data()+pin.rva,pin.bytes,pin.size);
 ck(msContract(),"C23 reader accepts the exact retail consumer/open/close pins");
 for(const auto& pin:nm::pins){image[pin.rva+pin.size-1]^=1;ck(!msContract(),"C23 each changed master-window pin fails closed");image[pin.rva+pin.size-1]^=1;}
 g_master_checked23=0;
 ck(NativeMasterCount23(owner23,count23)&&count23==1,"C23 live state+0x7D8 is read only under the verified contract");
 put(state.data()+0x7D8,70);ck(!NativeMasterCount23(owner23,count23),"C23 out-of-bound count is unreadable, not a window");
 // Mutually linked opponents (same proof NativeParryOpponentRead176 uses for the guard).
 std::array<unsigned char,0x500> foeA{},foeB{};std::array<unsigned char,0xE00> foeAS{},foeBS{};std::array<unsigned char,0x100> foeAE{},foeBE{};
 auto link=[&](unsigned char* a,unsigned char* s,unsigned char* e,std::uint32_t id){
  put(a,base+0x3E3FAC8);put(a+0x210,s);put(a+0x1F8,e);put(s+0xCE0,a);put(s+0xCF8,actor.data());put(s+8,std::uint8_t(1));
  put(s+0x38,1);put(s+0x218,-1);put(e,base+0x4072C78);put(e+0xC,id);put(state.data()+0xCF8,a);put(state.data()+8,std::uint8_t(1));};
 link(foeA.data(),foeAS.data(),foeAE.data(),456);
 masterTrace23.clear();g_master_window23={};g_master_published23.publish({});
 auto observe=[&](int c){hostTickCount64+=10;put(state.data()+0x7D8,c);NativeMasterObserve23(owner23,frame,hostTickCount64);
  nm::Published p{};(void)g_master_published23.read(p);return p;};
 auto p23=observe(1);
 ck(!p23.available&&!has("MS7D8_EDGE"),"C23 a count already positive when first seen is not adopted as a new window");
 p23=observe(0);p23=observe(1);
 ck(p23.available&&p23.serial==1&&p23.count==1&&p23.opponentId==456&&p23.key==frame.profiles[0].key&&p23.body==frame.input.bodyEpoch&&
    has("C23 MS7D8_EDGE edge=OPEN serial=1 count=1")&&has("oppId=456"),"C23 OPEN edge copies owner/opponent/item/body/request and tick; publication available");
 // C50: actual observer publishes for BOTH contact consumers; never enable DIRECT globally in HYBRID.
 {namespace dc=kcd2vr::direct_counter39;
  const auto experiment50=g_combat_experiment175.load(),integrated50=g_combat_integrated178r22c2.load(),direct50=kcd2vr::contact58c1::experiment.load();
  const auto before50=actionCalls;
  g_combat_experiment175=true;g_combat_integrated178r22c2=true;kcd2vr::contact58c1::experiment=false;
  put(foeAS.data()+0x218,0);put(foeAS.data()+0x158,1);put(foeAS.data()+0x38,2);observe(1);put(foeAS.data()+0x38,4);observe(1);
  ck(dc::openState.load()!=0&&dc::openOffered.load()==1&&!kcd2vr::contact58c1::experiment.load(),"C50 HYBRID publishes same-opponent counter window without DIRECT mode takeover");
  const auto hybridSerial50=dc::openSerial.load();
  kcd2vr::contact58c1::experiment=true;put(foeAS.data()+0x38,2);observe(1);put(foeAS.data()+0x38,4);observe(1);
  ck(dc::openState.load()!=0&&dc::openSerial.load()>hybridSerial50,"C50 DIRECT transition resets and rearms counter observer");
  kcd2vr::contact58c1::experiment=false;g_combat_integrated178r22c2=false;observe(1);
  ck(!dc::openState.load()&&actionCalls==before50,"C50 leaving both modes withdraws counter window and observer makes no native requests");
  g_combat_experiment175=experiment50;g_combat_integrated178r22c2=integrated50;kcd2vr::contact58c1::experiment=direct50;
  put(foeAS.data()+0x38,1);put(foeAS.data()+0x218,-1);
 }
 // C50: actual native geometry reader still measures a valid opponent when ordinary attacks are unrestricted.
 {const auto oldRequire50=g_attack_require_opponent59c10.load();
  std::uintptr_t oldId50=0,oldTm50=0;std::memcpy(&oldId50,image.data()+0x4072C78+8,8);std::memcpy(&oldTm50,image.data()+0x4072C78+0x100,8);
  put(image.data()+0x4072C78+8,&EntityId50);put(image.data()+0x4072C78+0x100,&WorldMatrix50);
  Matrix34V6550 h50{},t50{};h50.m[0]=h50.m[5]=h50.m[10]=1;t50=h50;t50.m[7]=1;
  std::memcpy(entity.data()+0x58,&h50,sizeof(h50));std::memcpy(foeAE.data()+0x58,&t50,sizeof(t50));
  g_attack_require_opponent59c10=false;auto measured50=observe(1);
  ck(measured50.available&&measured50.toward,"C50 unrestricted ordinary attacks retain measured native-master toward permission");
  t50.m[7]=100;std::memcpy(foeAE.data()+0x58,&t50,sizeof(t50));measured50=observe(1);
  ck(measured50.available&&!measured50.toward,"C50 unrestricted ordinary attacks cannot fabricate master geometry out of reach");
  put(image.data()+0x4072C78+8,oldId50);put(image.data()+0x4072C78+0x100,oldTm50);g_attack_require_opponent59c10=oldRequire50;
 }
 const auto opens=masterTrace23.size();p23=observe(2);
 ck(p23.available&&p23.serial==1&&masterTrace23.size()==opens,"C23 a changed positive count is not another edge (edge-only, no per-frame receipt)");
 put(state.data()+0xCF8,static_cast<void*>(nullptr));hostTickCount64+=10;NativeMasterObserve23(owner23,frame,hostTickCount64);(void)g_master_published23.read(p23);
 ck(!p23.available&&g_master_window23.serial==1&&has("edge=UNKNOWN serial=1 count=-1"),"C23 unreadable opponent/count = UNKNOWN, never CLOSE, never a serial, nothing published");
 put(state.data()+0xCF8,foeA.data());p23=observe(1);
 ck(p23.available&&p23.serial==1&&has("edge=RESUMED serial=1"),"C23 the same window resumes with its original serial");
 link(foeB.data(),foeBS.data(),foeBE.data(),789);p23=observe(1);
 ck(!p23.available&&g_master_window23.serial==1&&has("edge=OWNER_CHANGED serial=1")&&has("oppId=789"),"C23 opponent change retires the window and publishes nothing");
 p23=observe(1);ck(!p23.available,"C23 a positive count for the new opponent still needs its own native opening");
 p23=observe(0);p23=observe(1);ck(p23.available&&p23.serial==2&&p23.opponentId==789,"C23 genuine reopening for the new opponent = serial 2");
 p23=observe(0);ck(!p23.available&&has("edge=CLOSE serial=2 count=0"),"C23 CLOSE edge withdraws the publication");
 // Wired into the existing exact-owner guard pump (no new hook): one pump tick publishes this tick's observation.
 {hostTickCount64+=10;++frame.input.sample;frame.input.tick=hostTickCount64;animation_filter::combatPoseTick112=hostTickCount64;
  g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;g_master_published23.publish({});
  NativeGuardTick112();nm::Published pump{};(void)g_master_published23.read(pump);
  ck(pump.tick==hostTickCount64&&pump.count==0,"C23 NativeGuardTick112 itself performs the +7D8 observation on its exact-owner path");}
 // PREPARE: a handoff-tagged command is rechecked against the LIVE count and the same published window/opponent.
 link(foeA.data(),foeAS.data(),foeAE.data(),456);g_attack_require_opponent59c10=false;g_master_handoff_spent23=0;
 auto publishWindow=[&](std::uint64_t serial,std::uint32_t opp,std::uint64_t tick){nm::Published p{};p.serial=serial;p.tick=tick;
  p.key=command.key;p.generation=command.generation;p.body=command.bodyEpoch;p.request=command.epoch;p.opponentId=opp;p.count=1;p.available=p.toward=true;
  g_master_published23.publish(p);};
 auto prepare23=[&](std::uint64_t tag,int live){idle();put(state.data()+0x7D8,live);animation_filter::combatPoseTick112=hostTickCount64;
  command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Slash;command.zone117=2;command.kick59=false;command.held57=false;
  command.masterHandoff23=tag;masterTrace23.clear();const bool ok=NativePhysicalAction117(player.data(),command,frame,true);
  PhysAbortAttack117(player.data());return ok;};
 publishWindow(7,456,hostTickCount64);
 ck(prepare23(7,1)&&has("MELEE_INPUT edge=PREPARE")&&has("ms7D8=1 msHandoff=7")&&has("MS_WINDOW_OPEN count=1"),
    "C23 tagged PREPARE inside the live window reaches the native input with ms7D8 receipt; existing keep-stance path unchanged");
 ck(!prepare23(7,1)&&has("rejected=MS_HANDOFF_SERIAL_SPENT"),"C23 one handoff PREPARE per window serial");
 publishWindow(8,456,hostTickCount64);
 ck(!prepare23(8,0)&&has("rejected=MS_HANDOFF_WINDOW_CLOSED")&&has("ms7D8=0 msHandoff=8"),"C23 live count 0 at PREPARE: no stale-window latch");
 ck(!prepare23(8,-1)&&has("rejected=MS_HANDOFF_WINDOW_UNKNOWN"),"C23 unreadable live count refuses the handoff PREPARE");
 publishWindow(9,999,hostTickCount64);
 ck(!prepare23(9,1)&&has("rejected=MS_HANDOFF_OPPONENT_CHANGED"),"C23 opponent changed since the window opened");
 publishWindow(11,456,hostTickCount64);
 ck(!prepare23(10,1)&&has("rejected=MS_HANDOFF_WINDOW_CHANGED"),"C23 a different window serial cannot be borrowed");
 publishWindow(12,456,hostTickCount64-150);
 ck(!prepare23(12,1)&&has("rejected=MS_HANDOFF_WINDOW_CHANGED"),"C23 a stale publication cannot be extended");
 ck(prepare23(0,0)&&has("ms7D8=0 msHandoff=0"),"C23 untagged ordinary PREPARE outside the window is unchanged (receipt only)");
 ck(prepare23(0,1)&&has("ms7D8=1 msHandoff=0")&&has("MS_WINDOW_OPEN"),"C23 untagged PREPARE inside a window keeps the existing NativeMasterWindow59c17 stance behaviour");
 // XI consumption: the live count immediately before the retail handler (where D4AD94 runs) is receipted.
 {static const char ap[]="attack_primary";const std::uint64_t apn=reinterpret_cast<std::uint64_t>(ap);
  const int savedPending=kcd2vr::attack_xi178r8::pendingStroke.load();kcd2vr::attack_xi178r8::pendingStroke=1;
  put(state.data()+0x7D8,1);masterTrace23.clear();XiOwnedCall178r13(input.data(),&apn,1,1.f,player.data(),1,"attack_primary",hostTickCount64);
  ck(has("XI_RECEIPT action=attack_primary mode=1")&&has("ms7D8=1"),"C23 XI consumption receipt carries the live +7D8 count");
  put(state.data()+0x7D8,0);masterTrace23.clear();XiOwnedCall178r13(input.data(),&apn,2,0.f,player.data(),1,"attack_primary",hostTickCount64);
  ck(!has("XI_RECEIPT")||has("ms7D8=0"),"C23 closed window at XI is receipted as 0 (only when capture is active)");
  kcd2vr::attack_xi178r8::pendingStroke=savedPending;}
 command.masterHandoff23=0;g_master_handoff_spent23=0;g_master_window23={};g_master_published23.publish({});
 put(state.data()+0x7D8,0);put(state.data()+0xCF8,static_cast<void*>(nullptr));g_attack_require_opponent59c10=false;idle();
}
