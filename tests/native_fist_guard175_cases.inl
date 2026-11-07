// Uses the same exact owner/backlink fixture and native block/zone test leaves.
{
 NativeGuardRelease112();
 const auto savedMode=g_census_mode.load(),savedWeapon=g_weapon_control_mode.load();
 const auto savedLab=diagnostics::lab::Snapshot();
 const auto savedCapture=diagnostics::capture168::gate.mask.load();
 g_census_mode=5;g_weapon_control_mode=0;diagnostics::lab::state=0;
 diagnostics::capture168::gate.mask=0;
 g_fist_fault101=false;g_fist_native101={};g_guard_manual112=false;g_guard_manual_owner112=nullptr;
 put(hands.data()+0x260,std::uint64_t(0));put(hands.data()+0x268,std::uint64_t(0));
 put(state.data()+0x38,1);put(state.data()+0x218,-1);put(state.data()+0x248,-1);
 put(state.data()+0x2A8,std::uint8_t(0));put(state.data()+0x2D8,std::uint8_t(0));put(state.data()+0x308,std::uint8_t(0));
 FistFrame101 fistFrame{};fistFrame.player=player.data();fistFrame.request=g_fist_epoch101.load();
 auto& sample=fistFrame.sample;sample.epoch=g_pose_epoch6689.load();sample.actor=reinterpret_cast<std::uintptr_t>(entity.data());
 sample.sequence=10000;sample.tick=hostTickCount64;
 sample.valid=sample.empty[0]=sample.empty[1]=true;sample.head={0,1.7,0};
 sample.grip[0]=sample.grip[1]=sample.trigger[0]=sample.trigger[1]=1;
 sample.controller[0]={.16,1.57,-.20};sample.controller[1]={-.16,1.57,-.20};
 auto fistGuardFrame=[&](){hostTickCount64+=10;++sample.sequence;sample.tick=hostTickCount64;
     g_fist_frame101.publish(fistFrame);NativeGuardTick112();};
 const auto beforeFist=actionCalls;for(int i=0;i<7;++i)fistGuardFrame();
 ck(actionCalls==beforeFist&&!g_guard_physical112,"F10 raised fists cannot acquire native guard without posture dwell");
 for(int i=0;i<6;++i)fistGuardFrame();
 ck(g_guard_physical112&&g_native_guard112.held&&lastAction=="block"&&lastZone==0,
    "F10 raised empty fists acquire native block with F3/F5 diagnostic recorders OFF");
 ck(diagnostics::capture168::gate.mask==0&&diagnostics::lab::Get(diagnostics::lab::Key::F3)==0,
    "fist guard neither arms capture nor changes internal motion configuration");
 for(int blockPhaseH14:{128,256}){
  put(state.data()+0x38,blockPhaseH14);put(state.data()+0x278,7);put(state.data()+0x1B8,1);put(state.data()+0x308,std::uint8_t(0));
  const auto beforeMetadataH14=blockModes176.size();const auto zonesMetadataH14=zoneCalls;
  for(int nH14=0;nH14<6;++nH14)fistGuardFrame();
  ck(g_guard_physical112&&g_native_guard112.held&&zoneCalls==zonesMetadataH14&&
     std::all_of(blockModes176.begin()+beforeMetadataH14,blockModes176.end(),[](int mode){return mode==4;}),
     "H14 exact existing empty-fist guard survives native block metadata without a new LT press or sector write");
 }
 put(state.data()+0x38,1);put(state.data()+0x278,0);put(state.data()+0x308,std::uint8_t(1));
 const auto beforeCandidate=blockModes176.size();
 fistFrame.phase[0]=fm::Phase::Moving;fistFrame.phase[1]=fm::Phase::Moving;
 for(int i=0;i<8;++i)fistGuardFrame();
 ck(g_guard_physical112&&std::all_of(blockModes176.begin()+beforeCandidate,blockModes176.end(),[](int mode){return mode==4;}),
    "raised-hand candidate movement without a qualified punch retains boxing guard");
 fistFrame.phase[0]=fm::Phase::Ready;fistFrame.phase[1]=fm::Phase::Ready;
 const auto beforePunch=actionCalls;g_fist_native101.prepared=true;fistGuardFrame();
 ck(!g_guard_physical112&&actionCalls==beforePunch+1&&lastActionMode==2,
    "deliberate pending punch releases only the owned fist guard");g_fist_native101.prepared=false;
 CombatInputDetour101(input.data(),&blockName,1,1.f);const auto fistManual=actionCalls;
 ck(lastAction=="block"&&lastActionMode==1&&g_guard_manual112,"F10 passes manual LT block press through native input");
 for(int i=0;i<13;++i)fistGuardFrame();
 ck(g_guard_physical112&&actionCalls==fistManual,"fist posture joins held manual LT without duplicate input");
 CombatInputDetour101(input.data(),&blockName,2,0.f);
 ck(actionCalls==fistManual&&!g_guard_manual112&&g_guard_physical112,
    "manual release preserves still-held physical fist block lease");
 sample.valid=false;fistGuardFrame();
 ck(actionCalls==fistManual+1&&lastActionMode==2&&!g_guard_physical112,
    "invalid committed fist pose releases final block source exactly once");
 fistGuardFrame();ck(actionCalls==fistManual+1,"repeated invalid fist pose cannot duplicate block release");
 sample.valid=true;for(int i=0;i<13;++i)fistGuardFrame();const auto beforeEquip=actionCalls;
 put(hands.data()+0x260,std::uint64_t(77));fistGuardFrame();
 ck(!g_guard_physical112&&actionCalls==beforeEquip+1,"new held item revokes fist guard at native ownership check");
 put(hands.data()+0x260,std::uint64_t(0));put(state.data()+0x38,128);put(state.data()+0x278,7);put(state.data()+0x308,std::uint8_t(0));
 const auto foreignMetadataH14=actionCalls;for(int nH14=0;nH14<13;++nH14)fistGuardFrame();
 ck(actionCalls==foreignMetadataH14&&!g_guard_physical112,"H14 foreign metadata cannot acquire a new empty-fist guard");
 put(state.data()+0x38,1);put(state.data()+0x278,0);put(hands.data()+0x260,std::uint64_t(77));
 NativeGuardRelease112();g_fist_native101={};
 g_census_mode=savedMode;g_weapon_control_mode=savedWeapon;diagnostics::lab::state=savedLab;
 diagnostics::capture168::gate.mask=savedCapture;
}
