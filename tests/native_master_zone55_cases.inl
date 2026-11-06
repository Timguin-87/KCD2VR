// Complete production MasterGateEdge47 + reciprocal NativeParryOpponentRead176.
// Native action leaves are never invoked; this verifies only the extra VR veto.
{
 const auto savedReady55=g_master_gate_ready47.load();const int savedMode55=mg47::mode.load();const float savedEnergy55=mg47::minEnergy.load();
 void* savedSoul55=nullptr;(void)Read(player.data()+0x668,savedSoul55);
 std::array<unsigned char,0x400> soul55{};mg47::AbilityRow learned55{mg47::kMasterStrikeAbility,0,1};
 put(soul55.data(),base+0x3F44F28);put(soul55.data()+0x320,&learned55);put(soul55.data()+0x328,std::uint64_t(1));
 put(player.data()+0x668,soul55.data());
 auto attempt55=[&](int mode=1,ph::CommandKind kind=ph::CommandKind::Thrust,double speed=3.){
  mg47::mode=mode;mg47::minEnergy=.30f;g_master_gate_ready47=true;
  NativeOwner104 owner55{};ck(NativeResolve104(player.data(),owner55),"C55 master zone fixture resolves current Henry owner");
  ph::Command c55{};c55.originZone167=1;c55.zone117=1;c55.direction={0,0,speed};
  const bool denied=MasterGateEdge47(owner55,c55,kind,true);MasterAttempt47 a55{};
  ck(g_master_attempt47.read(a55)&&a55.actor==reinterpret_cast<std::uintptr_t>(actor.data()),"C55 actual master edge publishes its own current attempt");
  return std::pair{denied,a55};
 };
 baseline176();put(foeState176.data()+0x158,2);put(foeState176.data()+0x278,1);
 auto result55=attempt55();
 ck(!result55.first&&result55.second.enemyZone==2&&result55.second.reason==mg47::Reason::Qualified,
    "C55 actual master gate uses offered2 rather than executed1 for Henry's opposite origin1");
 put(foeState176.data()+0x278,5);result55=attempt55();
 ck(!result55.first&&result55.second.enemyZone==2,"C55 unrelated executed-zone changes do not change the extra master veto");
 put(foeState176.data()+0x158,1);put(foeState176.data()+0x278,2);result55=attempt55();
 ck(result55.first&&result55.second.reason==mg47::Reason::NotOpposite,"C55 same-side offered1 refuses even when executed2 would look opposite");
 for(int invalid55:{-1,3,4,9}){put(foeState176.data()+0x158,invalid55);result55=attempt55();
  ck(result55.first&&result55.second.enemyZone==-1&&result55.second.reason==mg47::Reason::SideUnknown,
     "C55 unknown/unoffered sector never falls back to a valid executed zone");}
 put(foeState176.data()+0x158,2);
 for(int inactive55:{1,16,64}){put(foeState176.data()+0x38,inactive55);result55=attempt55();
  ck(result55.first&&result55.second.enemyZone==-1,"C55 nonattacking native phase cannot qualify the offered-zone veto");}
 put(foeState176.data()+0x38,8);result55=attempt55();
 ck(!result55.first&&result55.second.enemyZone==2,"C55 actual strike phase8 retains the same offered-zone rule as windup4");
 put(foeState176.data()+0xCF8,static_cast<void*>(nullptr));result55=attempt55();
 ck(result55.first&&result55.second.enemyZone==-1,"C55 opponent without reciprocal Henry link leaves a normal attack");
 put(foeState176.data()+0xCF8,actor.data());put(state.data()+0xCF8,manager.data());result55=attempt55();
 ck(result55.first&&result55.second.enemyZone==-1,"C55 changed current opponent cannot borrow the previous offered zone");
 put(state.data()+0xCF8,foeActor176.data());put(foeState176.data()+0x218,-1);result55=attempt55();
 ck(result55.first&&result55.second.enemyZone==-1,"C55 unknown native attack type grants no master side");
 put(foeState176.data()+0x218,1);result55=attempt55(2);
 ck(result55.first&&result55.second.reason==mg47::Reason::PairedOff,"C55 mode2 still keeps the ordinary native attack path");
 result55=attempt55(1,ph::CommandKind::Slash);
 ck(!result55.first&&result55.second.reason==mg47::Reason::Qualified,"REPAIR2 cut uses the same opposite offered zone as a stab");
 result55=attempt55(1,ph::CommandKind::Thrust,.1);
 ck(!result55.first&&result55.second.reason==mg47::Reason::Qualified,"REPAIR2 qualified thrust is not vetoed by a second master energy threshold");
 put(soul55.data()+0x328,std::uint64_t(0));result55=attempt55();
 ck(result55.first&&result55.second.reason==mg47::Reason::NoPerk,"C55 corrected offered zone does not grant an unlearned master strike");
 // H21 actual cold-block hook must never apply an old weapon veto to fists.
 int savedCountH21=-1;(void)Read(state.data()+0x7D8,savedCountH21);MasterAttempt47 savedAttemptH21{};(void)g_master_attempt47.read(savedAttemptH21);
 const auto savedReceiptH21=g_master_receipted47.load(),savedDeniedH21=g_master_denied47.load();
 baseline176();g_master_gate_ready47=true;mg47::mode=1;put(state.data()+0x7D8,1);
 NativeOwner104 ownerH21{};ck(NativeResolve104(player.data(),ownerH21),"H21 gate current owner resolved");
 MasterAttempt47 vetoH21{};vetoH21.nativePolicyD51=kcd2vr::native_kill_nf::generation.load();vetoH21.serial=999;vetoH21.tick=hostTickCount64;
 vetoH21.player=reinterpret_cast<std::uintptr_t>(player.data());vetoH21.actor=reinterpret_cast<std::uintptr_t>(actor.data());
 vetoH21.state=reinterpret_cast<std::uintptr_t>(state.data());vetoH21.keyH21=77;vetoH21.generationH21=4;
 vetoH21.bodyH21=g_pose_epoch6689.load();vetoH21.requestH21=g_weapon_request_epoch6694.load();vetoH21.reason=mg47::Reason::NotOpposite;
 auto hookH21=[&](){g_master_attempt47.publish(vetoH21);DetourModKit::hook::MidContext ctx{};
  using namespace DetourModKit::hook;gpr(ctx,Gpr::Rcx)=vetoH21.actor;gpr(ctx,Gpr::Rax)=vetoH21.state;
  instruction_pointer(ctx)=base+0x22B780B;MasterGateHook47(ctx);return instruction_pointer(ctx);};
 ck(hookH21()==base+0xD4ADCC,"H21 actual hook still enforces same-current-weapon opposite-zone veto");
 put(hands.data()+0x260,std::uint64_t(0));
 ck(hookH21()==base+0x22B780B,"H21 fresh old weapon veto does not deny native unarmed master/disarm");
 put(hands.data()+0x260,std::uint64_t(88));
 ck(hookH21()==base+0x22B780B,"H21 another current weapon cannot inherit old opposite-zone veto");
 put(hands.data()+0x260,std::uint64_t(77));
 NativeInputScope117 fistH21{};fistH21.owner=ownerH21;fistH21.fistPunchR19=true;
 auto* previousScopeH21=g_input_scope117;g_input_scope117=&fistH21;
 ck(hookH21()==base+0x22B780B,"H21 exact synchronous fist native input ignores weapon-only policy");
 g_input_scope117=previousScopeH21;
 ++vetoH21.requestH21;ck(hookH21()==base+0x22B780B,"H21 old request veto is untouched native input");
 --vetoH21.requestH21;++vetoH21.bodyH21;ck(hookH21()==base+0x22B780B,"H21 old body epoch veto is untouched native input");
 put(state.data()+0x7D8,savedCountH21);g_master_attempt47.publish(savedAttemptH21);g_master_receipted47=savedReceiptH21;g_master_denied47=savedDeniedH21;
 put(player.data()+0x668,savedSoul55);
 g_master_gate_ready47=savedReady55;mg47::mode=savedMode55;mg47::minEnergy=savedEnergy55;
 baseline176();
}
