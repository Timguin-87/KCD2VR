// Production NativeGuardTick112 + actual input hook; native leaves replaced.
// Current positional F4 contract supersedes the prior LT-driven V176 cases.
{
 namespace pt=kcd2vr::diagnostics::parry176;
 const auto savedFrame176=frame;const auto savedMode176=g_census_mode.load(),savedWeapon176=g_weapon_control_mode.load();
 const auto savedF3176=diagnostics::lab::Get(diagnostics::lab::Key::F3);const auto savedCapture176=diagnostics::capture168::gate.mask.load();
 const auto savedMotion57c1=g_guard_countermotion57c1.load();g_guard_countermotion57c1=false; // explicit historical assist comparison
 const auto savedAssist55c2=g_guard_incoming_repress45.load();const auto savedAutoWindup19c4r1=g_guard_auto_windup19c4r1.load();
 const auto savedExperiment176=g_combat_experiment175.load();const auto savedCal176=g_cal_state.load();
 const auto savedQx176=g_cal_frame_qx.load(),savedQy176=g_cal_frame_qy.load(),savedQz176=g_cal_frame_qz.load(),savedQw176=g_cal_frame_qw.load();
 std::array<unsigned char,0x500> foeActor176{};std::array<unsigned char,0xE00> foeState176{};std::array<unsigned char,0x100> foeEntity176{};
 TrackingSnapshot tracked176{};tracked176.sequence=20000;bool shieldCase176=false;float pull176=0.f;
 auto baseline176=[&](){
  g_guard_auto_windup19c4r1=false;blockCallback176={};g_guard_manual112=false;g_guard_manual_owner112=nullptr;NativeGuardRelease112();pt::Clear();
  g_census_mode=3;g_weapon_control_mode=6;g_combat_experiment175=true;diagnostics::lab::Set(diagnostics::lab::Key::F3,0);diagnostics::capture168::gate.mask=0;
  g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;animation_filter::nativeBodyOwned112=false;g_phys_fault6698=false;g_phys_armed6698=true;
  g_cal_state=2;g_cal_frame_qx=0;g_cal_frame_qy=0;g_cal_frame_qz=0;g_cal_frame_qw=1;
  put(player.data()+0x9D0,manager.data());put(manager.data()+0x38,player.data());put(hands.data()+0x260,std::uint64_t(77));put(hands.data()+0x268,std::uint64_t(0));
  put(state.data()+0xCE0,actor.data());put(state.data()+0xCF8,foeActor176.data());put(state.data()+8,std::uint8_t(1));
  put(state.data()+0x38,1);put(state.data()+0x218,-1);put(state.data()+0x248,-1);put(state.data()+0x278,-1);put(state.data()+0x1B8,1);
  put(state.data()+0x2A8,std::uint8_t(0));put(state.data()+0x2D8,std::uint8_t(0));put(state.data()+0x308,std::uint8_t(0));put(state.data()+0x648,std::uint8_t(0));put(state.data()+0x5D8,1);
  put(foeActor176.data(),base+0x3E3FAC8);put(foeActor176.data()+0x210,foeState176.data());put(foeActor176.data()+0x1F8,foeEntity176.data());
  put(foeState176.data()+0xCE0,foeActor176.data());put(foeState176.data()+0xCF8,actor.data());put(foeState176.data()+8,std::uint8_t(1));
  put(foeState176.data()+0x38,4);put(foeState176.data()+0x218,1);put(foeState176.data()+0x278,1);put(foeState176.data()+0x248,-1);
  put(foeState176.data()+0x158,2); // R59.16C10R17: the attacker's offered zone (+0x158, Henry's frame) is the incoming sector: Henry's right
  put(foeEntity176.data(),base+0x4072C78);put(foeEntity176.data()+0xC,std::uint32_t(456));
  frame=savedFrame176;frame.input.coherentMelee119=true;frame.input.positionGuard176=true;frame.input.allowed=true;frame.output.strokeActive119=false;
  frame.output.active[0]=frame.output.owned[0]=true;frame.output.active[1]=frame.output.owned[1]=false;
  frame.output.weapons[0]={{0,0,0,1},{-.3,.4,.3}};frame.profiles[0].key=77;frame.input.grip[0]=frame.input.grip[1]=1;frame.input.trigger[0]=frame.input.trigger[1]=0;
  tracked176.allowed=true;tracked176.head_valid=tracked176.right_valid=tracked176.left_valid=true;
  tracked176.head_held=tracked176.right_held=tracked176.left_held=false;tracked176.head={};tracked176.right={};tracked176.left={};
  g_guard_incoming_repress45=false;put(state.data()+0x778,0); // explicit legacy motion comparison; production default tested below
  blockModes176.clear();parryTraceLines176.clear();shieldCase176=false;pull176=0.f;
 };
 auto tick176=[&](float x){tracked176.tick=hostTickCount64+10;++tracked176.sequence;
  tracked176.head_good_tick=tracked176.left_good_tick=tracked176.right_good_tick=tracked176.tick;
  if(shieldCase176){tracked176.left.x=x;frame.output.weapons[1].p.x=x;}
  else {tracked176.right.x=x;tracked176.right.z=pull176;}
  // This bridge starts with a completed-pose fixture, not a physical solver.
  // Supply the same qualified per-side decision the real producer publishes.
  for(int side=0;side<2;++side)frame.output.guardBySide30c1[side]=frame.output.active[side]&&frame.output.owned[side]&&
      ph::natural_guard117(frame.input,frame.profiles[side],frame.output.weapons[side],g_native_guard112.held&&g_native_guard112.side==side);
  g_tracking_frames6695.publish(tracked176);guardFrame();};
 auto settle176=[&](){for(int i=0;i<16;++i)tick176(0.f);};
 auto motion176=[&](float direction=1.f){tick176(.03f*direction);tick176(.06f*direction);tick176(.09f*direction);};
 auto edges176=[](){std::vector<int> result;for(int mode:blockModes176)if(mode==1||mode==2)result.push_back(mode);return result;};
 auto pair176=[&](){return edges176()==std::vector<int>{2,1};};
 pt::Edge edge176{};
 baseline176();frame.input.positionGuard176=false;tick176(0);
 ck(edges176().empty()&&!g_guard_physical112,"stale baseline frame cannot acquire F4 positional guard before producer policy changes");
 baseline176();const auto firstTick176=hostTickCount64;tick176(0);
 ck(g_native_guard112.held&&hostTickCount64-firstTick176==10&&pt::Read(edge176)&&edge176.cause==pt::Cause::Initial&&!edge176.manualHeld,"F4 positional entry blocks on first qualified sample without Trigger or motion settle");
 ck(g_native_guard112.parry176.window.count==1,
    "C54 first defensive pose records motion history before the initial native block press");
 settle176();blockModes176.clear();motion176();
 ck(pair176()&&!g_guard_manual112&&g_native_guard112.held,"F4 positional guard counter-motion emits exactly release then press without LT");
 ck(pt::Read(edge176)&&edge176.cause==pt::Cause::Motion&&edge176.releaseReturned&&edge176.pressReturned&&!edge176.manualHeld&&edge176.henryId==123&&edge176.opponentId==456,"positional motion record proves native calls and exact pair without claiming parry outcome");
 ck(parryTraceLines176.empty(),"F3 OFF emits no edge formatting while positional guard and re-press work");
 blockModes176.clear();g_combat_experiment175=false;NativeGuardRelease112();
 ck(edges176()==std::vector<int>{2}&&!g_guard_physical112,"F4 exit releases its positional block exactly once");
 // C19C3 selective C20 repair: the native input is still owned by this
 // player after recenter. Only the old pose/parry authorization is invalid.
 for(int recenterCase=0;recenterCase<4;++recenterCase){
  baseline176();tick176(0);ck(g_native_guard112.held,"C19C3 recenter fixture starts with actual positional guard press");
  const auto oldEpoch=g_pose_epoch6689.load();++g_pose_epoch6689;
  g_guard_release_state59c7e=state.data();g_guard_release_tick59c7e=hostTickCount64;
  if(recenterCase==1){g_guard_manual_owner112=player.data();g_guard_manual112=true;}
  if(recenterCase==2)put(state.data()+0xCE0,static_cast<void*>(nullptr));
  if(recenterCase==3)put(actor.data()+0x1F8,static_cast<void*>(nullptr));
  blockModes176.clear();NativeGuardRelease112("RECENTER_CONTEXT_LOSS");NativeGuardRelease112("REPEATED_CONTEXT_LOSS");
  ck(edges176()==(recenterCase==0?std::vector<int>{2}:std::vector<int>{}),
     "C19C3 epoch change releases exactly once only for same native owner, never manual or broken backlinks");
  ck(!g_native_guard112.held&&!g_guard_physical112&&g_guard_release_tick59c7e==0&&g_guard_release_state59c7e==nullptr,
     "C19C3 recenter context cleanup grants no stale parry or strike witness");
  put(state.data()+0xCE0,actor.data());put(actor.data()+0x1F8,player.data());g_pose_epoch6689=oldEpoch;
  g_guard_manual112=false;g_guard_manual_owner112=nullptr;
 }
 baseline176();frame.output.weapons[0].p={3,3,3};
 CombatInputDetour101(input.data(),&blockName,1,1.f);tick176(0);
 ck(edges176().empty()&&!g_guard_manual112&&!g_guard_physical112,"F4 external LT cannot create block outside positional guard");
 CombatInputDetour101(input.data(),&blockName,2,0.f);
 baseline176();diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,GetTickCount64());settle176();blockModes176.clear();parryTraceLines176.clear();motion176();
 ck(pair176()&&parryTraceLines176.size()==1&&parryTraceLines176.back().find("cause=MOTION")!=std::string::npos,"F3 ON retains positional actions and records completed motion without collision");
 baseline176();settle176();frame.output.strokeActive119=true;blockModes176.clear();motion176();
 ck(pair176(),"held positional guard retains defensive counter-motion through stroke arbitration");
 baseline176();frame.output.strokeActive119=true;tick176(0);
 ck(edges176().empty(),"stroke begun outside guard is not converted into a block merely crossing guard region");
 baseline176();frame.output.weapons[0]={{0,-std::sqrt(.5),0,std::sqrt(.5)},{0,.4,0}};tick176(0);
 ck(g_guard_physical112,"upright blade in front of face now qualifies automatic F4 guard");
 // R40C1: torso entry and wider hold bounds, with real production input delivery.
 baseline176();frame.output.weapons[0].p.x=-.3+.75;tick176(0); // R45: 42 cm entry half width
 ck(!g_guard_physical112,"blade entirely outside the entry cone cannot start guard");
 frame.output.weapons[0].p.x=-.3;tick176(0);blockModes176.clear();
 frame.output.weapons[0].p.x=-.3+.78;tick176(0); // R45: 50 cm held half width
 ck(g_guard_physical112&&edges176().empty(),"existing guard retains a position inside the torso hold band");
 frame.output.weapons[0].p.x=-.3+.85;tick176(0);
 ck(!g_guard_physical112&&edges176()==std::vector<int>{2},"leaving positional cone releases block once");
 // C4R1 OPTIONAL assistance. Execute the real guard producer and native input
 // transport; native state changes here are explicit fake native leaves only.
 {
  GuardWindupKey19c4r1 full{player.data(),actor.data(),state.data(),entity.data(),1,77,4,1,0,
      foeActor176.data(),foeState176.data(),foeEntity176.data(),456};
  GuardWindupEdge19c4r1 edge{};
  ck(!edge.observe(full,1,100,false,true)&&!edge.observe(full,4,110,false,true),"C4R1 automatic windup helper is inert while disabled");
  ck(!edge.observe(full,4,120,true,true)&&!edge.observe(full,4,130,true,true),"C4R1 first observation inside windup is baseline, never an invented rising edge");
  ck(!edge.observe(full,8,140,true,true)&&edge.observe(full,4,150,true,true),"C4R1 one known non-4 to 4 transition produces one optional refresh");
  ck(!edge.observe({},0,160,true,false)&&!edge.observe(full,4,170,true,true),"C4R1 unknown opponent read does not reset and rearm the same attack");
  ck(!edge.observe(full,8,180,true,true)&&!edge.observe(full,4,300,true,true),"C4R1 long unknown observation gap establishes baseline without a delayed refresh");
  ck(!edge.observe(full,8,310,true,true)&&!edge.observe(full,4,305,true,true),"C4R1 backwards clock establishes baseline without an edge");
  for(int field=0;field<13;++field){
   auto changed=full;edge={};edge.observe(full,1,400,true,true);
   switch(field){case 0:changed.player=manager.data();break;case 1:changed.actor=manager.data();break;
    case 2:changed.state=manager.data();break;case 3:changed.entity=manager.data();break;case 4:++changed.epoch;break;
    case 5:++changed.key;break;case 6:++changed.generation;break;case 7:++changed.request;break;case 8:changed.side=1;break;
    case 9:changed.opponent=manager.data();break;case 10:changed.opponentState=manager.data();break;
    case 11:changed.opponentEntity=manager.data();break;case 12:++changed.opponentId;break;}
   ck(!edge.observe(changed,4,410,true,true),"C4R1 every owner/weapon/request/opponent identity replacement prevents a fabricated edge");
  }
 }
 baseline176();g_guard_auto_windup19c4r1=true;g_guard_countermotion57c1=true;put(foeState176.data()+0x38,1);
 settle176();blockModes176.clear();put(foeState176.data()+0x38,4);tick176(0);
 ck(pair176()&&g_native_guard112.held&&pt::Read(edge176)&&edge176.cause==pt::Cause::AutoWindup&&
    edge176.opponentId==456&&g_native_guard112.incomingEdgeTick45==0&&!g_native_guard112.window55c2.answered,
    "C4R1 opted-in held guard refreshes at actual windup edge without consuming deliberate parry timing");
 blockModes176.clear();tick176(0);put(foeActor176.data(),base+0x1234);tick176(0);
 put(foeActor176.data(),base+0x3E3FAC8);tick176(0);
 ck(edges176().empty(),"C4R1 continued windup and a transient unreadable opponent cannot repeat auto refresh");
 g_guard_countermotion57c1=false;g_guard_incoming_repress45=true;put(state.data()+0x778,1);tick176(0);
 ck(pair176()&&pt::Read(edge176)&&edge176.cause==pt::Cause::NativeWindow,
    "C4R1 native perfect-block opening remains available immediately after optional windup refresh");
 // An actual observed free-block phase is a held input, not a forbidden new owner.
 baseline176();g_guard_auto_windup19c4r1=true;put(foeState176.data()+0x38,1);settle176();
 put(state.data()+0x38,256);put(state.data()+0x218,1);blockModes176.clear();
 blockCallback176=[&](int mode){if(mode==2){put(state.data()+0x38,1);put(state.data()+0x218,-1);}};
 put(foeState176.data()+0x38,4);tick176(0);
 ck(pair176()&&pt::Read(edge176)&&edge176.cause==pt::Cause::AutoWindup,"C4R1 optional refresh retains native free-block release/acknowledgement semantics");
 // Unknown original phase/type never becomes a known idle -> windup edge.
 baseline176();g_guard_auto_windup19c4r1=true;settle176();blockModes176.clear();
 put(foeState176.data()+0x218,99);tick176(0);put(foeState176.data()+0x218,1);tick176(0);
 ck(edges176().empty(),"C4R1 invalid native opponent observation cannot rearm an already observed windup");
 // The queued transport carries the true cause, without sharing PB serial/cooldown.
 baseline176();g_guard_auto_windup19c4r1=true;put(foeState176.data()+0x38,1);settle176();
 {namespace xi=kcd2vr::attack_xi178r8;xi::reset();xi::enabled=true;xi::translate(3,1,hostTickCount64);
  float rt=0,lt=0;bool a=false;xi::apply(hostTickCount64,rt,lt,a);
  put(foeState176.data()+0x38,4);tick176(0);
  ck(g_native_guard112.pendingPress46&&g_native_guard112.pendingAutoWindup19c4r1&&!g_native_guard112.held,
     "C4R1 XI delivery retains the optional windup refresh as a pending transaction");
  xi::apply(hostTickCount64+1,rt,lt,a);put(state.data()+0x308,std::uint8_t(0));put(state.data()+0x648,std::uint8_t(0));tick176(0); // original native Update processes the LT-up packet
  ck(g_native_guard112.held&&!g_native_guard112.pendingPress46&&pt::Read(edge176)&&edge176.cause==pt::Cause::AutoWindup&&
     g_native_guard112.incomingEdgeTick45==0&&!g_native_guard112.window55c2.answered,
     "C4R1 asynchronous refresh retains auto cause and leaves later native parry timing untouched");
  xi::enabled=false;xi::reset();}
 for(int fault=0;fault<5;++fault){
  baseline176();g_guard_auto_windup19c4r1=true;put(foeState176.data()+0x38,1);settle176();blockModes176.clear();
  blockCallback176=[&,fault](int mode){if(mode!=2)return;
   if(fault==0)g_combat_experiment175=false;
   if(fault==1)g_right_valid=false;
   if(fault==2){++frame.profiles[0].generation;g_phys_frame6698.publish(frame);}
   if(fault==3){frame.output.guardBySide30c1[0]=false;g_phys_frame6698.publish(frame);}
   if(fault==4)put(foeState176.data()+0xCF8,static_cast<void*>(nullptr));
  };put(foeState176.data()+0x38,4);tick176(0);
  if(fault==4)ck(pair176()&&pt::Read(edge176)&&edge176.cause==pt::Cause::Initial,
    "C4R1 changed opponent after release restores ordinary guard without false auto attribution");
  else ck(edges176()==std::vector<int>{2}&&!g_native_guard112.held&&!g_native_guard112.pendingPress46,
    "C4R1 optional refresh revalidates mode, tracking, exact weapon generation and posture after native release");
 }
 baseline176();g_guard_auto_windup19c4r1=true;put(foeState176.data()+0x38,1);settle176();blockModes176.clear();
 tracked176.right_held=true;put(foeState176.data()+0x38,4);tick176(0);tracked176.right_held=false;tick176(0);
 ck(edges176().empty(),"C4R1 optional windup cannot use tracking grace or replay a missed stale edge later");
 g_guard_countermotion57c1=false;
 // R55C2 supersedes phase-episode assistance: native opening decides WHEN to ask.
 baseline176();g_guard_incoming_repress45=true;settle176();blockModes176.clear();
 for(int phase:{1,16,4,8}){put(foeState176.data()+0x38,phase);tick176(0);}
 ck(g_guard_physical112&&edges176().empty(),"closed native PB slot: enemy phases alone never re-press guard");
 put(state.data()+0x778,1);tick176(0);
 ck(g_guard_physical112&&pair176()&&pt::Read(edge176)&&edge176.cause==pt::Cause::NativeWindow,
    "guard raised early answers the later native PB opening exactly once without counter-motion");
 blockModes176.clear();put(state.data()+0x778,2);tick176(0);motion176();put(foeState176.data()+0x38,4);tick176(0);
 ck(edges176().empty(),"overlapping triggers, phase changes and motion cannot double-tap an open slot");
 put(state.data()+0x778,-1);tick176(0);put(state.data()+0x778,1);tick176(0);
 ck(edges176().empty(),"unknown counter does not rearm a previously answered native slot");
 put(state.data()+0x778,0);for(int i=0;i<16;++i)tick176(0);put(state.data()+0x778,1);tick176(0);
 ck(pair176(),"next native close/open earns one fresh block request");
 baseline176();g_guard_incoming_repress45=true;put(state.data()+0x778,1);tick176(0);blockModes176.clear();settle176();motion176();
 ck(edges176().empty()&&pt::Read(edge176)&&edge176.cause==pt::Cause::NativeWindow,
    "posture entry during native opening is the one timed press, never followed by automatic double-tap");
 for(int badCount:{-1,17}){baseline176();g_guard_incoming_repress45=true;settle176();blockModes176.clear();put(state.data()+0x778,badCount);tick176(0);
  ck(edges176().empty()&&g_guard_physical112,"unbounded PB count preserves ordinary held block without assistance");}
 baseline176();g_guard_incoming_repress45=true;settle176();blockModes176.clear();put(state.data()+0x778,1);tracked176.right_held=true;tick176(0);
 ck(edges176().empty()&&g_guard_physical112,"fresh native opening cannot trigger release using tracking grace");
 tracked176.right_held=false;tick176(0);ck(pair176(),"fresh tracking may answer still-open native window once");
 // Review: real XI queues release; retain the down half until native state acknowledges it.
 baseline176();g_guard_incoming_repress45=true;settle176();put(foeState176.data()+0x38,1);for(int i=0;i<16;++i)tick176(0);
 {namespace xi=kcd2vr::attack_xi178r8;
  xi::reset();xi::enabled=true;xi::translate(3,1,hostTickCount64);
  float rt=0,lt=0;bool a=false;xi::apply(hostTickCount64,rt,lt,a);
  put(state.data()+0x778,1);put(foeState176.data()+0x38,4);tick176(0);
  ck(g_native_guard112.pendingPress46&&!g_native_guard112.held,
     "REVIEW R46 queues pending press while XI release has not reached native state");
  rt=lt=0;a=false;xi::apply(hostTickCount64+1,rt,lt,a);
  ck(lt==0.f,"REVIEW R46 actual XI publisher exposes LT-up packet");
  put(state.data()+0x308,std::uint8_t(0));put(state.data()+0x648,std::uint8_t(0));tick176(0); // explicit original native Update ACK
  ck(xi::ltHeld.load()&&g_native_guard112.held&&!g_native_guard112.pendingPress46,
     "REVIEW R46 restores owned LT after native block clears");
  ck(pt::Read(edge176)&&edge176.cause==pt::Cause::NativeWindow&&edge176.opponentId==456,
     "REVIEW R46 asynchronous assisted press retains NATIVE_WINDOW cause and opponent identity");
  xi::enabled=false;xi::reset();}
 // R46C1: automatic assistance must revalidate after crossing the native release.
 for(int fault46c1=0;fault46c1<4;++fault46c1){
  baseline176();g_guard_incoming_repress45=true;settle176();put(foeState176.data()+0x38,1);for(int i=0;i<16;++i)tick176(0);
  blockModes176.clear();blockCallback176=[&,fault46c1](int mode){if(mode!=2)return;
   if(fault46c1==0)g_combat_experiment175=false;
   if(fault46c1==1)g_right_valid=false;
   if(fault46c1==2){frame.profiles[0].generation++;g_phys_frame6698.publish(frame);}
   if(fault46c1==3){frame.output.guardBySide30c1[0]=false;g_phys_frame6698.publish(frame);}
  };put(state.data()+0x778,1);put(foeState176.data()+0x38,4);tick176(0);
  ck(edges176()==std::vector<int>{2}&&!g_native_guard112.held&&!g_native_guard112.pendingPress46,
     "R46C1 automatic re-press rejects mode/tracking/generation/posture replacement inside native release");
 }
 baseline176();g_guard_incoming_repress45=true;settle176();blockModes176.clear();
 blockCallback176=[&](int mode){if(mode==2)put(state.data()+0x778,0);};put(state.data()+0x778,1);tick176(0);
 ck(pair176()&&pt::Read(edge176)&&edge176.cause==pt::Cause::Initial,"window closing during release restores ordinary block without timed attribution");
 baseline176();g_guard_incoming_repress45=true;settle176();blockModes176.clear();
 blockCallback176=[&](int mode){if(mode==2)put(foeState176.data()+0xCF8,static_cast<void*>(nullptr));};put(state.data()+0x778,1);tick176(0);
 ck(pair176()&&pt::Read(edge176)&&edge176.cause==pt::Cause::Initial,"pair invalidation during release restores ordinary block without timed attribution");
 for(int shieldClass:{8,17}){
  baseline176();g_guard_incoming_repress45=true;shieldCase176=true;put(hands.data()+0x268,std::uint64_t(88));
  frame.profiles[1]=frame.profiles[0];frame.profiles[1].key=88;frame.profiles[1].side=1;frame.profiles[1].classId=shieldClass;
  frame.profiles[1].tip={0,1,0};frame.output.weapons[1]={{0,0,0,1},{0,.4,.1}};
  frame.output.active[1]=frame.output.owned[1]=true;frame.output.weapons[0].p={3,3,3};settle176();blockModes176.clear();
  put(state.data()+0x778,1);tick176(0);
  ck(pair176()&&g_native_guard112.side==1&&pt::Read(edge176)&&edge176.cause==pt::Cause::NativeWindow,"held shield answers native PB opening without directional motion requirement");
 }
 // R59.16C10R17 (supersedes R59C3): counter-motion is required in EVERY F4 mode ("from block against the weapon = perfect block").
 baseline176();g_guard_countermotion57c1=true;g_guard_incoming_repress45=false;settle176();blockModes176.clear();
 put(state.data()+0x778,1);tick176(0);
 ck(edges176().empty()&&g_guard_physical112,"R17 a stationary held guard keeps the block but does not re-press in the native opening");
 motion176();
 ck(pair176()&&pt::Read(edge176)&&edge176.cause==pt::Cause::NativeWindow,"R57C1 counter-motion toward incoming sector requests once in native opening");
 blockModes176.clear();settle176();motion176();
 ck(edges176().empty(),"R57C1 one opening cannot be retriggered by repeated motion");
 baseline176();g_guard_countermotion57c1=true;g_guard_incoming_repress45=true;settle176();blockModes176.clear();motion176();
 ck(edges176().empty()&&g_guard_physical112,"R57C1 counter-motion outside native timing remains ordinary block");
 baseline176();g_guard_countermotion57c1=true;g_guard_incoming_repress45=true;settle176();blockModes176.clear();put(state.data()+0x778,1);motion176(-1);
 ck(pair176(),"R59C3 guard posture and native timing qualify without a fragile directional-motion threshold");
 baseline176();g_guard_countermotion57c1=true;g_guard_incoming_repress45=true;settle176();blockModes176.clear();
 put(state.data()+0x38,256);put(state.data()+0x778,1);
 blockCallback176=[&](int mode){if(mode==2)put(state.data()+0x38,1);};motion176();
 ck(pair176()&&g_guard_physical112,"R57C1 held phase 256 can produce motion and recover through the native release acknowledgement");
 baseline176();g_guard_countermotion57c1=true;g_guard_incoming_repress45=true;settle176();put(state.data()+0x778,1);
 {namespace xi=kcd2vr::attack_xi178r8;xi::reset();xi::enabled=true;xi::translate(3,1,hostTickCount64);
  float rt=0,lt=0;bool a=false;xi::apply(hostTickCount64,rt,lt,a);motion176();
  ck(g_native_guard112.pendingPress46,"R57C1 movement retains queued XI down until release is acknowledged");
  put(state.data()+0x308,std::uint8_t(0));put(state.data()+0x648,std::uint8_t(0));tick176(.09f); // explicit original native Update ACK
  ck(g_guard_physical112&&!g_native_guard112.pendingPress46&&pt::Read(edge176)&&edge176.cause==pt::Cause::NativeWindow,
     "R57C1 asynchronous press retains native window provenance without requiring movement twice");xi::enabled=false;xi::reset();}
 g_guard_countermotion57c1=false;
 // Block input can stay held through ordinary native progression without stance writes.
 baseline176();settle176();blockModes176.clear();const auto zones40c1=zoneCalls;
 for(int phase40c1:{4,8,16}){
  put(state.data()+0x38,phase40c1);put(state.data()+0x218,1);put(state.data()+0x2A8,std::uint8_t(1));put(state.data()+0x2D8,std::uint8_t(1));
  tick176(0);motion176();ck(g_guard_physical112&&edges176().empty()&&zoneCalls==zones40c1,"ordinary action and counter-motion keep block input without stance writes or an unsafe re-press");
 }
 animation_filter::nativeBodyOwned112=true;tick176(0);
 ck(edges176()==std::vector<int>{2}&&!g_guard_physical112,"protected native body ownership still releases guard immediately");
 baseline176();put(state.data()+0x38,16);put(state.data()+0x218,0);const auto recoveryZones40c1=zoneCalls;tick176(0);
 ck(edges176()==std::vector<int>{1}&&zoneCalls==recoveryZones40c1,"recovery accepts native block request while game retains eligibility and stance");
 baseline176();put(state.data()+0x38,32);put(state.data()+0x218,10);tick176(0);
 ck(edges176().empty(),"unclassified action type/phase cannot acquire assisted guard");
 for(int fault176=0;fault176<8;++fault176){
  baseline176();settle176();pt::Clear();
  if(fault176==0)put(foeState176.data()+0x38,1);
  if(fault176==1)put(foeState176.data()+0x218,-1);
  if(fault176==2)put(foeState176.data()+0xCF8,static_cast<void*>(nullptr));
  if(fault176==3)put(foeState176.data()+0x158,7);  // R59.16C10R17: no valid offered sector (Henry's +0x5D8 is a receipt only now)
  if(fault176==4)put(foeState176.data()+0x158,-1);
  if(fault176==5)put(foeActor176.data(),base+0x1234);
  if(fault176==6)tracked176.right_held=true;
  blockModes176.clear();
  if(fault176==7){tick176(0);pull176=.03f;tick176(0);pull176=.06f;tick176(0);pull176=.09f;tick176(0);} // pulled back toward Henry: never a parry
  else motion176(1.f);
  ck(edges176().empty()&&!pt::Read(edge176),"unqualified phase/type/pair/sector/vft/tracking/direction cannot re-press held guard");
 }
 baseline176();settle176();blockModes176.clear();g_combat_experiment175=false;frame.input.positionGuard176=false;motion176();
 ck(edges176().empty(),"F12 with F3 OFF does not inherit F4 counter-motion re-press");
 for(int fault176=0;fault176<7;++fault176){
  baseline176();settle176();pt::Clear();tick176(.03f);tick176(.06f);blockModes176.clear();
  blockCallback176=[&,fault176](int mode){if(mode!=2)return;
   if(fault176==0){g_combat_experiment175=false;pt::Clear();}
   if(fault176==1){frame.output.weapons[0].p={3,3,3};g_phys_frame6698.publish(frame);}
   if(fault176==2)put(foeState176.data()+0xCF8,static_cast<void*>(nullptr));
   if(fault176==3)put(state.data()+0xCE0,static_cast<void*>(nullptr));
   if(fault176==4)put(hands.data()+0x260,std::uint64_t(88));
   if(fault176==5)g_right_valid=false;
   if(fault176==6){frame.profiles[0].generation++;g_phys_frame6698.publish(frame);}
  };tick176(.09f);
  if(fault176==2)ck(pair176()&&g_guard_physical112,"opponent change restores ordinary positional block without motion attribution");
  else ck(edges176()==std::vector<int>{2},"mode/posture/owner/key/tracking/generation loss during release prevents stale press");
  ck(!pt::Read(edge176)||edge176.cause!=pt::Cause::Motion,"failed post-release qualification never publishes motion result");
 }
 baseline176();settle176();pt::Clear();tick176(.03f);tick176(.06f);blockModes176.clear();
 blockCallback176=[&](int mode){if(mode==2)put(state.data()+0x308,std::uint8_t(1));};tick176(.09f);
 ck(edges176()==std::vector<int>{2}&&!g_guard_physical112,"reentrant foreign block is not pressed twice or adopted");
 baseline176();settle176();++g_pose_epoch6689;NativeGuardTick112();
 ck(!pt::Read(edge176)&&!g_guard_physical112,"owner epoch loss invalidates trace and positional lease");--g_pose_epoch6689;
 // Inherited manual block is a native latch to drain, not new F4 permission.
 baseline176();g_combat_experiment175=false;frame.output.weapons[0].p={3,3,3};
 CombatInputDetour101(input.data(),&blockName,1,1.f);ck(g_guard_manual112,"baseline native manual LT still works outside F4");
 g_combat_experiment175=true;blockModes176.clear();tick176(0);tick176(0);
 ck(edges176()==std::vector<int>{2}&&!g_guard_manual112&&!g_guard_physical112,"F4 first guard pump drains inherited manual native lease once even outside posture");
 // Failure to invoke native release must retain the inherited manual latch.
 baseline176();g_combat_experiment175=false;CombatInputDetour101(input.data(),&blockName,1,1.f);g_combat_experiment175=true;
 frame.output.weapons[0].p={3,3,3};put(player.data()+0xB88,static_cast<void*>(nullptr));blockModes176.clear();tick176(0);
 ck(g_guard_manual112&&!g_guard_physical112&&edges176().empty()&&*reinterpret_cast<std::uint8_t*>(state.data()+0x308)==1,"failed exact-owner LT release invocation retains pending lease and cannot acquire positional block");
 put(player.data()+0xB88,input.data());tick176(0);tick176(0);
 ck(!g_guard_manual112&&edges176()==std::vector<int>{2},"restored native input owner drains inherited LT once, without repeating release");
 baseline176();g_combat_experiment175=false;CombatInputDetour101(input.data(),&blockName,1,1.f);g_combat_experiment175=true;
 frame.output.weapons[0].p={3,3,3};g_phys_fault6698=true;blockModes176.clear();tick176(0);tick176(0);
 ck(g_guard_manual112&&edges176().empty(),"native physical fault retains pending LT without retrying native calls");
 g_phys_fault6698=false;tick176(0);
 ck(!g_guard_manual112&&edges176()==std::vector<int>{2},"explicitly cleared physical fault allows the same qualified pending release");
 baseline176();g_combat_experiment175=false;CombatInputDetour101(input.data(),&blockName,1,1.f);g_combat_experiment175=true;
 put(state.data()+0xCE0,static_cast<void*>(nullptr));blockModes176.clear();tick176(0);
 ck(!g_guard_manual112&&edges176().empty(),"invalidated native owner retires stale LT identity without releasing a replacement owner");
 // Ranged and authored UI/activity input are exempt from gesture ownership.
 PhysEquipment6698 savedEquipment176{};g_phys_equipment6698.read(savedEquipment176);
 for(int rangedClass176:{9,10,13,14,15}){
  baseline176();PhysEquipment6698 ranged176{};ranged176.valid=true;ranged176.tick=hostTickCount64;ranged176.request=g_weapon_request_epoch6694.load();ranged176.epoch=g_pose_epoch6689.load();
  ranged176.manager=manager.data();ranged176.player=player.data();ranged176.count=1;ranged176.handKnown[0]=ranged176.handKnown[1]=true;ranged176.handKey[0]=77;ranged176.heldIndex[0]=0;
  ranged176.items[0].profile=frame.profiles[0];ranged176.items[0].profile.classId=rangedClass176;g_phys_equipment6698.publish(ranged176);
  frame.profiles[0].classId=rangedClass176;CombatInputDetour101(input.data(),&blockName,1,1.f);blockModes176.clear();tick176(0);
  ck(g_guard_manual112&&edges176().empty()&&!pt::Read(edge176)&&*reinterpret_cast<std::uint8_t*>(state.data()+0x308)==1,"F4 ranged native LT is not drained, traced as parry, or replaced by positional guard");
  CombatInputDetour101(input.data(),&blockName,2,0.f);ck(edges176()==std::vector<int>{2},"ranged native LT release remains intact in F4");
 }
 g_phys_equipment6698.publish(savedEquipment176);
 baseline176();g_gameplay_allowed=false;CombatInputDetour101(input.data(),&blockName,1,1.f);blockModes176.clear();tick176(0);
 ck(g_guard_manual112&&edges176().empty()&&!pt::Read(edge176),"authored UI/activity LT remains native and outside parry trace while F4 gameplay eligibility is false");
 CombatInputDetour101(input.data(),&blockName,2,0.f);ck(edges176()==std::vector<int>{2},"authored UI/activity native release survives guard pump");
 baseline176();g_combat_experiment175=false;CombatInputDetour101(input.data(),&blockName,1,1.f);g_combat_experiment175=true;
 frame.input.allowed=false;blockModes176.clear();tick176(0);tick176(0);
 ck(edges176()==std::vector<int>{2}&&!g_guard_manual112,"gesture-mode entry drains old manual latch once even before a usable physical frame");
 for(int shieldClass176:{8,17}){
  baseline176();shieldCase176=true;put(hands.data()+0x268,std::uint64_t(88));
  frame.profiles[1]=frame.profiles[0];frame.profiles[1].key=88;frame.profiles[1].side=1;frame.profiles[1].classId=shieldClass176;
  frame.profiles[1].tip={0,1,0};frame.output.weapons[1]={{0,0,0,1},{0,.4,.1}};
  frame.output.active[1]=frame.output.owned[1]=true;frame.output.weapons[0].p={3,3,3};tick176(0);
  ck(g_guard_physical112&&g_native_guard112.side==1,"known raised shield grip qualifies guard without treating bounds axis as face normal");
  settle176();blockModes176.clear();motion176();
  ck(pair176()&&pt::Read(edge176)&&edge176.cause==pt::Cause::Motion,"shield counter-motion uses original left-hand sample and exact live left key");
  blockModes176.clear();put(hands.data()+0x268,std::uint64_t(89));tick176(.09f);
  ck(edges176()==std::vector<int>{2}&&!g_guard_physical112,"shield identity replacement revokes its physical block lease");
 }
 // BC2 all four F4 policies share the native-window, deliberate-beat transaction.
 {const auto savedIntegrated=g_combat_integrated178r22c2.load(),savedSync=g_synchronized59c10r9.load(),savedIntent=g_intent59c16.load();
  const auto savedPhysical=g_physical_combat177.load(),savedMotion=g_guard_countermotion57c1.load();
  const auto savedPrepared=g_phys_native6698;g_guard_countermotion57c1=true;
  for(int rung=1;rung<=4;++rung){
   baseline176();g_phys_native6698.attackPrepared=false;g_combat_integrated178r22c2=rung==1||rung==3;
   g_synchronized59c10r9=rung==1||rung==3;g_intent59c16=rung==1;g_physical_combat177=true;
   frame.input.motionOwned59c5=rung==4;combat_diag_probe::hostMotionGuardOwned59c10=false;combat_diag_probe::hostMotionGuardUnknown59c10=false;
   tick176(0);blockModes176.clear();
   // A native window remains closed while the hand starts its beat; duplicate engine visits must not destroy XR history.
   for(int n=1;n<=5;++n){tick176(.02f*n);NativeGuardTick112();NativeGuardTick112();}
   ck(edges176().empty(),"all F4 rungs: a deliberate beat cannot re-press while native PB window is closed");
   put(state.data()+0x778,1);tick176(.12f);NativeGuardTick112();NativeGuardTick112();
   ck(pair176()&&g_guard_physical112&&pt::Read(edge176)&&edge176.cause==pt::Cause::NativeWindow,
      "all F4 rungs: repeated same-XR pump visits retain beat and deliver one native-window release/press");
   blockModes176.clear();for(int n=0;n<12;++n){tick176(.12f);NativeGuardTick112();}
   ck(edges176().empty(),"all F4 rungs: an answered opening cannot loop or replay the same beat");
  }
  for(int bad=0;bad<5;++bad){
   baseline176();g_phys_native6698.attackPrepared=false;g_combat_integrated178r22c2=false;g_synchronized59c10r9=g_intent59c16=false;
   tick176(0);blockModes176.clear();
   for(int n=1;n<=6;++n){
    if(bad==0)pull176=.02f*n; // pulling back is not a parry
    if(bad==1)tracked176.head.x=.02f*n; // stationary hand, moving head
    if(bad==2)put(foeState176.data()+0x38,1); // no incoming ordinary attack
    if(bad==3)tracked176.right_held=true; // tracked grace never triggers an edge
    if(bad==4)put(foeState176.data()+0xCF8,static_cast<void*>(nullptr)); // target changed
    if(n==6)put(state.data()+0x778,1);
    tick176(bad<2?0.f:.02f*n);NativeGuardTick112();NativeGuardTick112();
   }
   ck(edges176().empty(),"PB beat refuses wrong direction, head-only motion, no incoming, tracking grace, or changed pair");
  }
  g_combat_integrated178r22c2=savedIntegrated;g_synchronized59c10r9=savedSync;g_intent59c16=savedIntent;
  g_physical_combat177=savedPhysical;g_guard_countermotion57c1=savedMotion;g_phys_native6698=savedPrepared;
 }
 // C10R18 third mode: a forward input offer owns neither damage nor native phase,
 // but the same guard pump must not cancel it before the native selector runs.
 {const auto savedRung=g_combat_rung178r15.load();const auto savedPhysical=g_physical_combat177.load();
  const auto savedIntegrated=g_combat_integrated178r22c2.load(),savedSync=g_synchronized59c10r9.load();
  const auto savedPrepared=g_phys_native6698;const auto savedPreparedOwner=g_input_prepared_owner117;
  auto pending=[&](){g_combat_integrated178r22c2=g_synchronized59c10r9=true;g_physical_combat177=true;
   frame.output.strokeActive119=true;g_phys_native6698.player=player.data();g_phys_native6698.epoch=frame.input.bodyEpoch;
   g_phys_native6698.request=frame.request;g_phys_native6698.attackPrepared=true;
   g_phys_native6698.attackKey=frame.profiles[0].key;g_phys_native6698.attackGeneration117=frame.profiles[0].generation;
   g_phys_native6698.prepareTickC10R18=hostTickCount64;g_phys_native6698.prepareGestureC10R18=9001;frame.output.primaryStrokeGesture19=9001;
   frame.output.guardBySide30c1[0]=false;frame.input.tick=hostTickCount64;g_phys_ready_sample6698=frame.input.sample;
   NativeResolve104(player.data(),g_input_prepared_owner117);};
  for(bool shield:{false,true}){
   baseline176();g_phys_native6698.attackPrepared=false;
   if(shield){shieldCase176=true;put(hands.data()+0x268,std::uint64_t(88));
    frame.profiles[1]=frame.profiles[0];frame.profiles[1].key=88;frame.profiles[1].side=1;frame.profiles[1].classId=8;
    frame.profiles[1].tip={0,1,0};frame.output.weapons[1]={{0,0,0,1},{0,.4,.1}};
    frame.output.active[1]=frame.output.owned[1]=true;frame.output.weapons[0].p={3,3,3};}
   tick176(0);pending();blockModes176.clear();const auto starts=actionCalls;guardFrame();guardFrame();
   ck(g_phys_native6698.attackPrepared&&!g_guard_physical112&&edges176()==std::vector<int>{2}&&actionCalls==starts+1,
      "C10R18 forward pending sword/shield offer releases guard once without cancel or re-press");
   ck(g_phys_native6698.prepareTickC10R18==hostTickCount64-20,
      "C10R18 guard ticks cannot renew the original pending offer deadline");
   frame.output.strokeActive119=false;g_phys_native6698.attackPrepared=false;blockModes176.clear();tick176(0);
   ck(g_guard_physical112&&edges176()==std::vector<int>{1},"C10R18 ended pending offer returns positional guard immediately");
  }
  baseline176();pending();NativeOwner104 exact{};NativeResolve104(player.data(),exact);nc104::Observed observed{};NativeObserve104(exact,observed);
  ck(NativeSynchronizedPrepareOwnedC10R18(exact,frame,observed,hostTickCount64),"C10R18 exact bounded forward preparation qualifies");
  for(int bad=0;bad<19;++bad){auto testFrame=frame;auto testOwner=exact;auto testState=observed;auto tick=hostTickCount64;
   if(bad==0)testFrame.profiles[0].generation++;if(bad==1)testFrame.profiles[0].key++;
   if(bad==2)testFrame.request++;if(bad==3)testFrame.input.bodyEpoch++;if(bad==4)testFrame.output.strokeActive119=false;
   if(bad==5)testFrame.input.allowed=false;if(bad==6)testFrame.input.tracked[0]=false;if(bad==7){tick+=601;testFrame.input.tick=tick;}
   if(bad==8){testState.phase=32;testState.type=10;}if(bad==9)g_synchronized59c10r9=false;
   if(bad==10)testFrame.output.owned[0]=false;if(bad==11)testOwner.actor=nullptr;
   if(bad==12)++testFrame.output.primaryStrokeGesture19;
   if(bad==13)testFrame.output.guardBySide30c1[0]=true;if(bad==14)testFrame.input.sample++;
   if(bad==15)testFrame.player=foeEntity176.data();if(bad==16)testFrame.input.tick=tick+1;
   if(bad==17)testFrame.output.handlingOnly55c1[0]=true;if(bad==18)testFrame.profiles[0].primary=false;
   ck(!NativeSynchronizedPrepareOwnedC10R18(testOwner,testFrame,testState,tick),"C10R18 mismatch/ended/expired/protected forward offer cannot exclude guard");g_synchronized59c10r9=true;
  }
  // Newer XR may end the recognizer while the original release still waits
  // at the queue head. Only that exact bounded command may preserve Prepare.
  const auto savedQueue19=g_phys_commands6698.q;
  const auto savedHead19=g_phys_commands6698.head.load(),savedTail19=g_phys_commands6698.tail.load();
  baseline176();pending();g_phys_native6698.prepareSample117=frame.input.sample;
  g_phys_native6698.stroke117=ph::CommandKind::Slash;
  ph::Command queued19{};queued19.kind=queued19.stroke117=ph::CommandKind::Slash;
  queued19.side=0;queued19.key=frame.profiles[0].key;queued19.generation=frame.profiles[0].generation;
  queued19.epoch=frame.request;queued19.bodyEpoch=frame.input.bodyEpoch;queued19.sample=frame.input.sample;
  queued19.tick=hostTickCount64;queued19.gesture59c3=g_phys_native6698.prepareGestureC10R18;
  frame.output.strokeActive119=false;frame.output.primaryStrokeGesture19=0;
  ++frame.input.sample;g_phys_ready_sample6698=frame.input.sample;
  NativeResolve104(player.data(),exact);NativeObserve104(exact,observed);
  auto queue19=[&](const ph::Command& c){g_phys_commands6698.head=g_phys_commands6698.tail=0;g_phys_commands6698.push(c);};
  queue19(queued19);
  ck(NativeForwardPrepareOwned19c2(exact,frame,observed,hostTickCount64),
     "C19C2 original queued release survives newer inactive XR without renewing gesture or deadline");
  for(int bad=0;bad<17;++bad){auto c=queued19;auto testFrame=frame;auto now=hostTickCount64;
   if(bad==0)++c.gesture59c3;if(bad==1)++c.key;if(bad==2)++c.generation;if(bad==3)++c.epoch;
   if(bad==4)++c.bodyEpoch;if(bad==5)c.side=1;if(bad==6)c.kick59=true;if(bad==7)c.kind=ph::CommandKind::Abort;
   if(bad==8)c.stroke117=ph::CommandKind::Thrust;if(bad==9)c.sample=g_phys_native6698.prepareSample117-1;
   if(bad==10)c.sample=frame.input.sample+1;if(bad==11)c.tick=g_phys_native6698.prepareTickC10R18-1;
   if(bad==12)c.tick=now+1;if(bad==13){now+=101;testFrame.input.tick=now;}
   if(bad==14){now+=601;testFrame.input.tick=now;c.tick=now;}
   if(bad==15)testFrame.output.guardBySide30c1[0]=true;
   queue19(c);if(bad==16)g_phys_commands6698.pop(c);
   ck(!NativeForwardPrepareOwned19c2(exact,testFrame,observed,now),
      "C19C2 stale/foreign/cancelled/popped queued release or true primary guard cannot exclude guard");
  }
  // Cross the real guard producer, real typed Prepare, and actual queue
  // consumer after a newer XR frame has already ended its recognizer.
  const auto savedContract19=g_input_contract117.load();const auto savedModel19=model117;
  const auto savedMeta19=meta;const auto savedGeom19=combat_diag_probe::hostGeometry59c3;
  for(auto stroke:{ph::CommandKind::Slash,ph::CommandKind::Thrust}){
   baseline176();g_phys_native6698.attackPrepared=false;g_combat_integrated178r22c2=true;g_synchronized59c10r9=false;
   g_physical_combat177=true;g_phys_input6698=true;g_phys_abort6698=false;combat_diag_probe::hostGeometry59c3=false;
   model117=route117=true;primaryLatch117=specialLatch117=0;g_input_contract117=base;put(control117.data()+0x18,actor.data());
   frame.input.naturalCombat117=true;frame.input.attacks=true;frame.input.motionOwned59c5=false;
   shieldCase176=true;put(hands.data()+0x268,std::uint64_t(88));
   frame.profiles[1]=frame.profiles[0];frame.profiles[1].key=88;frame.profiles[1].side=1;frame.profiles[1].classId=8;
   frame.output.active[1]=frame.output.owned[1]=true;frame.output.weapons[1]={{0,0,0,1},{0,.4,.1}};
   frame.output.weapons[0].p={3,3,3};tick176(0);
   ck(g_guard_physical112&&g_native_guard112.side==1,"C19C2 delayed release fixture begins with real shield block");
   frame.output.guardBySide30c1[0]=false;frame.output.guard117=true;
   frame.output.strokeActive119=true;frame.output.primaryStrokeGesture19=9100+unsigned(stroke);guardFrame();
   meta.metadataAt=hostTickCount64;PhysReadLive103(meta.last);
   meta.profiles=savedEquipment176;meta.profiles.tick=hostTickCount64;meta.profiles.request=frame.request;meta.profiles.epoch=frame.input.bodyEpoch;
   meta.profiles.player=player.data();meta.profiles.manager=manager.data();meta.profiles.handKey[0]=77;meta.profiles.handKey[1]=88;
   meta.profiles.heldIndex[0]=0;meta.profiles.heldIndex[1]=1;meta.profiles.handKnown[0]=meta.profiles.handKnown[1]=true;meta.profiles.count=2;
   meta.profiles.items[0].profile=frame.profiles[0];meta.profiles.items[0].held=true;
   meta.profiles.items[1].profile=frame.profiles[1];meta.profiles.items[1].held=true;meta.profiles.valid=true;
   ph::Command c=queued19;c.kind=ph::CommandKind::Prepare;c.stroke117=stroke;c.zone117=stroke==ph::CommandKind::Thrust?5:2;
   c.gesture59c3=frame.output.primaryStrokeGesture19;c.sample=frame.input.sample;c.tick=hostTickCount64;
   c.key=frame.profiles[0].key;c.generation=frame.profiles[0].generation;c.epoch=frame.request;c.bodyEpoch=frame.input.bodyEpoch;
   g_phys_commands6698.head=g_phys_commands6698.tail=0;g_phys_commands6698.push(c);c.kind=stroke;g_phys_commands6698.push(c);
   const auto starts=begins117,ends=commits117;
   // This fixture models native input transport, not entity-world geometry.
   // No opponent link takes the already-supported native first-strike path.
   put(state.data()+0xCF8,static_cast<void*>(nullptr));
   NativeFrameBegin104();PhysNativeTickImpl6698();
   ck(g_phys_native6698.attackPrepared&&begins117==starts+1&&commits117==ends,
      "C19C2 delayed release fixture enters actual native typed Prepare and defers original release");
   const auto onset=g_phys_native6698.prepareTickC10R18;
   frame.output.strokeActive119=false;frame.output.primaryStrokeGesture19=0;blockModes176.clear();guardFrame();
   ck(g_phys_native6698.attackPrepared&&!g_guard_physical112&&edges176().empty()&&g_phys_native6698.prepareTickC10R18==onset,
      "C19C2 newer inactive XR cannot re-press shield or cancel exact still-queued release");
   meta.metadataAt=hostTickCount64;PhysReadLive103(meta.last);
   NativeFrameBegin104();PhysNativeTickImpl6698();
   ck(!g_phys_native6698.attackPrepared&&commits117==ends+1&&g_phys_commands6698.head==g_phys_commands6698.tail,
      "C19C2 preserved original cut/stab reaches actual native release exactly once on later frame");
   NativeFrameBegin104();PhysNativeTickImpl6698();
   ck(commits117==ends+1,"C19C2 consumed queued release grants no replay or continuing guard exclusion");
  }
  g_input_contract117=savedContract19;model117=savedModel19;meta=savedMeta19;combat_diag_probe::hostGeometry59c3=savedGeom19;
  g_phys_commands6698.q=savedQueue19;g_phys_commands6698.head=savedHead19;g_phys_commands6698.tail=savedTail19;
  // C19C2 intentionally supersedes the old comparison cancellation assertion:
  // the user requires qualified strikes FROM guard in every F4 native mode.
  baseline176();g_phys_native6698.attackPrepared=false;tick176(0);pending();g_synchronized59c10r9=false;blockModes176.clear();guardFrame();
  ck(g_phys_native6698.attackPrepared&&!g_guard_physical112&&edges176()==std::vector<int>{2},
     "C19C2 exact pending comparison offer releases guard instead of canceling the deliberate primary strike");
  // Earliest forward sample, before the consumer has issued Prepare: the
  // secondary shield cannot keep LT held or turn this back into parry motion.
  const auto oldIntent19=g_intent59c16.load();
  for(int mode=0;mode<3;++mode){
   baseline176();g_phys_native6698.attackPrepared=false;g_combat_integrated178r22c2=true;
   g_synchronized59c10r9=mode!=0;g_intent59c16=mode==2;g_physical_combat177=true;
   shieldCase176=true;put(hands.data()+0x268,std::uint64_t(88));
   frame.profiles[1]=frame.profiles[0];frame.profiles[1].key=88;frame.profiles[1].side=1;frame.profiles[1].classId=8;
   frame.profiles[1].tip={0,1,0};frame.output.weapons[1]={{0,0,0,1},{0,.4,.1}};
   frame.output.active[1]=frame.output.owned[1]=true;frame.output.weapons[0].p={3,3,3};tick176(0);
   ck(g_native_guard112.held&&g_native_guard112.side==1,"C19C2 native F4 mode begins with exact secondary shield guard");
   frame.output.strokeActive119=true;frame.output.primaryStrokeGesture19=9002;
   frame.output.guardBySide30c1[0]=false;blockModes176.clear();guardFrame();
   ck(!g_guard_physical112&&!g_phys_native6698.attackPrepared&&edges176()==std::vector<int>{2},
      "C19C2 comparison/synchronized/Intent primary forward onset releases shield before native Prepare");
   baseline176();g_phys_native6698.attackPrepared=false;g_combat_integrated178r22c2=true;
   g_synchronized59c10r9=mode!=0;g_intent59c16=mode==2;tick176(0);blockModes176.clear();
   frame.output.strokeActive119=true;frame.output.primaryStrokeGesture19=9003;tick176(0);
   ck(g_guard_physical112&&edges176().empty(),"C19C2 genuine primary guarding/counter-parry is not stolen by a stroke flag");
  }
  g_intent59c16=oldIntent19;
  baseline176();g_phys_native6698.attackPrepared=false;tick176(0);pending();animation_filter::nativeBodyOwned112=true;blockModes176.clear();tick176(0);
  ck(!g_guard_physical112&&edges176()==std::vector<int>{2},"C10R18 protected native body releases existing guard");
  baseline176();g_phys_native6698.attackPrepared=false;tick176(0);pending();g_right_valid=false;blockModes176.clear();tick176(0);
  ck(!g_guard_physical112&&edges176()==std::vector<int>{2},"C10R18 tracking loss releases existing guard");
  g_phys_native6698=savedPrepared;g_input_prepared_owner117=savedPreparedOwner;
  g_combat_rung178r15=savedRung;g_physical_combat177=savedPhysical;g_combat_integrated178r22c2=savedIntegrated;g_synchronized59c10r9=savedSync;
 }
 // Production guard arbitration consumes the exact driver's live lease rather
 // than guessing from a posture or a separate 400ms producer cooldown.
 for(bool shield59c10:{false,true}){
  baseline176();frame.input.motionOwned59c5=true;
  if(shield59c10){shieldCase176=true;put(hands.data()+0x268,std::uint64_t(88));
   frame.profiles[1]=frame.profiles[0];frame.profiles[1].key=88;frame.profiles[1].side=1;frame.profiles[1].classId=8;
   frame.profiles[1].tip={0,1,0};frame.output.weapons[1]={{0,0,0,1},{0,.4,.1}};
   frame.output.active[1]=frame.output.owned[1]=true;frame.output.weapons[0].p={3,3,3};}
  combat_diag_probe::hostMotionGuardOwned59c10=false;tick176(0);
  ck(g_guard_physical112,"C10 ordinary guard remains available before motion takes phase ownership");
  combat_diag_probe::hostMotionGuardOwned59c10=true;blockModes176.clear();
  // false also exercises the completed recognizer whose contact lease is still live.
  frame.output.strokeActive119=shield59c10;tick176(0);tick176(0);
  ck(!g_guard_physical112&&edges176()==std::vector<int>{2},
     "C10 live motion lease releases one owned block and prevents sword or shield re-press");
  const auto& queried59c10=combat_diag_probe::hostGuardQuery59c10;
  ck(queried59c10.gesture==0&&queried59c10.weapon==frame.profiles[0].key&&
     queried59c10.generation==frame.profiles[0].generation&&queried59c10.body==frame.input.bodyEpoch&&
     queried59c10.request==frame.input.epoch&&queried59c10.player==reinterpret_cast<std::uintptr_t>(player.data()),
     "C10 shield guard queries the attacking primary weapon's exact context");
  combat_diag_probe::hostMotionGuardOwned59c10=false;frame.output.strokeActive119=false;blockModes176.clear();tick176(0);
  ck(g_guard_physical112&&edges176()==std::vector<int>{1},
     "C10 ended driver lease returns qualified guard immediately without a synthetic cooldown");
  combat_diag_probe::hostMotionGuardUnknown59c10=true;blockModes176.clear();tick176(0);tick176(0);
  ck(!g_guard_physical112&&edges176()==std::vector<int>{2},
     "C10 unsettled motion publication releases once and cannot re-press sword or shield guard");
  combat_diag_probe::hostMotionGuardUnknown59c10=false;blockModes176.clear();tick176(0);
  ck(g_guard_physical112&&edges176()==std::vector<int>{1},
     "C10 stable no-owner publication restores guard immediately after a contested snapshot");
 }
 baseline176();combat_diag_probe::hostMotionGuardOwned59c10=true;combat_diag_probe::hostMotionGuardUnknown59c10=true;frame.input.motionOwned59c5=false;tick176(0);
 ck(g_guard_physical112,"C10 motion lease cannot change native-comparison guard ownership");
 combat_diag_probe::hostMotionGuardOwned59c10=false;combat_diag_probe::hostMotionGuardUnknown59c10=false;
 // C19C2: reproduce the real guard producer -> early strike release -> lingering
 // native block state. Generic context release used to erase this same owner's
 // witness before the queued motion-owned attempt could inspect phase128/256.
 auto handoff19c2=[&](int phase,bool capture=false){
  baseline176();guardHandoffTrace19.clear();frame.input.motionOwned59c5=true;tick176(0);
  ck(g_native_guard112.held,"C19C2 handoff fixture starts from the actual guard producer");
  put(state.data()+0x38,phase);put(state.data()+0x218,1);
  frame.output.strokeActive119=true;frame.output.guardBySide30c1[0]=false;
  if(capture)diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,GetTickCount64());
  blockModes176.clear();guardFrame();
 };
 // H20: the existing physical-contact fallback must recognize ONLY our quiet
 // lingering guard. Observation of128/256 by itself never authorizes a hit.
 for(int phase:{128,256}){
  handoff19c2(phase);
  ck(HybridQuietGuardOwnedH20(actor.data(),state.data()),"H20 actual guard handoff qualifies exact quiet128/256 for separate contact/cost checks");
  ck(!HybridQuietGuardOwnedH20(foeActor176.data(),state.data())&&!HybridQuietGuardOwnedH20(actor.data(),foeState176.data()),
     "H20 quiet guard never crosses actor/state identities");
  for(unsigned property:{0x2A8u,0x2D8u,0x308u}){
   put(state.data()+property,std::uint8_t(1));
   ck(!HybridQuietGuardOwnedH20(actor.data(),state.data()),"H20 pending request/candidate/active block cannot borrow quiet guard fallback");
   put(state.data()+property,std::uint8_t(0));
  }
  g_guard_manual_owner112=player.data();g_guard_manual112=true;
  ck(!HybridQuietGuardOwnedH20(actor.data(),state.data()),"H20 manual LT retains ownership over physical fallback");
  g_guard_manual112=false;g_guard_manual_owner112=nullptr;
  put(state.data()+0x218,5);ck(!HybridQuietGuardOwnedH20(actor.data(),state.data()),"H20 nonordinary action type stays protected");put(state.data()+0x218,1);
  put(state.data()+0x38,64);ck(!HybridQuietGuardOwnedH20(actor.data(),state.data()),"H20 native stagger wins over old guard witness");put(state.data()+0x38,phase);
  animation_filter::nativeBodyOwned112=true;ck(!HybridQuietGuardOwnedH20(actor.data(),state.data()),"H20 paired native body wins over guard witness");animation_filter::nativeBodyOwned112=false;
  hostTickCount64=g_guard_release_tick59c7e.load()+251;
  ck(!HybridQuietGuardOwnedH20(actor.data(),state.data()),"H20 expired guard ownership is not refreshed by contact admission");
 }
 for(int phase:{128,256}){
  handoff19c2(phase);const auto releasedAt=g_guard_release_tick59c7e.load();
  ck(edges176()==std::vector<int>{2}&&!g_guard_physical112&&releasedAt&&GuardOwnedByUs59c7e(state.data()),
     "C19C2 deliberate direct strike releases once and retains exact lingering-block ownership");
  ck(!GuardOwnedByUs59c7e(foeState176.data())&&!GuardOwnedByUs59c7e(nullptr),
     "C19C2 strike witness never authorizes a foreign or missing state");
  ck(guardHandoffTrace19.empty(),"C19C2 handoff produces no diagnostic strings with F3 recorder OFF");
  guardFrame();ck(g_guard_release_tick59c7e==releasedAt&&edges176()==std::vector<int>{2},
     "C19C2 repeated strike frames neither re-release block nor renew its witness");
  hostTickCount64=releasedAt+250;ck(GuardOwnedByUs59c7e(state.data()),"C19C2 existing 250 ms ownership boundary retained");
  ++hostTickCount64;ck(!GuardOwnedByUs59c7e(state.data()),"C19C2 strike witness expires without an extra frame");
 }
 for(int loss=0;loss<5;++loss){
  handoff19c2(128);ck(GuardOwnedByUs59c7e(state.data()),"C19C2 context-loss fixture has a released witness");
  if(loss==0)g_right_valid=false;if(loss==1)g_ctx_menu=true;if(loss==2)animation_filter::nativeBodyOwned112=true;
  if(loss==3)g_gameplay_allowed=false;if(loss==4)put(state.data()+0xCE0,static_cast<void*>(nullptr));
  NativeGuardTick112();ck(!GuardOwnedByUs59c7e(state.data())&&g_guard_release_tick59c7e==0,
     "C19C2 tracking/menu/paired/gameplay/native-owner loss revokes even an already-released witness");
  g_right_valid=true;g_ctx_menu=false;animation_filter::nativeBodyOwned112=false;g_gameplay_allowed=true;
  put(state.data()+0xCE0,actor.data());
 }
 for(int refusal=0;refusal<4;++refusal){
  baseline176();frame.input.motionOwned59c5=true;tick176(0);put(state.data()+0x38,128);put(state.data()+0x218,1);
  frame.output.strokeActive119=true;frame.output.guardBySide30c1[0]=false;
  if(refusal==0)put(input.data(),std::uintptr_t(0)); // input identity failure: native release was not called
  if(refusal==1)blockCallback176=[&](int){put(state.data()+0x38,64);}; // genuine reaction wins
  if(refusal==2)blockCallback176=[&](int){g_ctx_menu=true;}; // native callback transferred context
  if(refusal==3)blockCallback176=[&](int){put(state.data()+0xCE0,static_cast<void*>(nullptr));};
  guardFrame();ck(!GuardOwnedByUs59c7e(state.data())&&g_guard_release_tick59c7e==0,
     "C19C2 failed/partial native release or callback ownership takeover cannot mint a strike witness");
  put(input.data(),base+0x3EC99A8);put(state.data()+0xCE0,actor.data());g_ctx_menu=false;blockCallback176={};
 }
 baseline176();frame.input.motionOwned59c5=true;tick176(0);put(state.data()+0x38,128);put(state.data()+0x218,1);
 frame.output.strokeActive119=true;frame.output.guardBySide30c1[0]=false;
 blockCallback176=[&](int){put(state.data()+0x308,std::uint8_t(1));};guardFrame();
 ck(GuardOwnedByUs59c7e(state.data()),"C19C2 native blocking property is not guessed to be a synchronous release acknowledgement");
 baseline176();frame.input.motionOwned59c5=true;tick176(0);put(state.data()+0x38,128);put(state.data()+0x218,1);
 frame.output.strokeActive119=true;frame.output.guardBySide30c1[0]=false;
 kcd2vr::attack_xi178r8::enabled=true;guardFrame();
 ck(!GuardOwnedByUs59c7e(state.data()),"C19C2 queued XI release is not claimed as synchronous native handoff");
 kcd2vr::attack_xi178r8::enabled=false;kcd2vr::attack_xi178r8::reset();
 baseline176();frame.input.motionOwned59c5=true;tick176(0);put(state.data()+0x38,128);put(state.data()+0x218,1);
 combat_diag_probe::hostMotionGuardOwned59c10=true;guardFrame();
 ck(GuardOwnedByUs59c7e(state.data()),"C19C2 exact live motion owner also uses bounded guard handoff");
 combat_diag_probe::hostMotionGuardOwned59c10=false;combat_diag_probe::hostMotionGuardUnknown59c10=true;guardFrame();
 ck(!GuardOwnedByUs59c7e(state.data()),"C19C2 unknown motion publication revokes handoff rather than guessing ownership");
 combat_diag_probe::hostMotionGuardUnknown59c10=false;
 handoff19c2(128,true);guardFrame();
 ck(guardHandoffTrace19.size()==1&&guardHandoffTrace19[0].find("witness=GRANTED")!=std::string::npos&&
    guardHandoffTrace19[0].find("synchronous=1 livePhase=128 liveKnown=1")!=std::string::npos,
    "C19C2 finite Combat recorder reports one exact-state handoff, without repeated stroke-frame strings");
 #include "direct_guard_handoff19c4r2_cases.inl"
 #include "native_master_zone55_cases.inl"
 // H5: real guard pump/input transaction with explicit native leaves. No PB
 // result is synthesized. Missing +778 does not erase deliberate counter input.
 for(bool asynchronousH5:{false,true}){
  baseline176();g_guard_countermotion57c1=true;g_guard_incoming_repress45=true;settle176();
  put(foeState176.data()+0x38,1);tick176(0);
  put(foeState176.data()+0x38,4);tick176(0);blockModes176.clear();
  if(asynchronousH5)put(state.data()+0x308,std::uint8_t(1));
  for(int n=1;n<=4;++n){
   if(asynchronousH5)blockCallback176=[&](int mode){if(mode==2)put(state.data()+0x308,std::uint8_t(1));};
   tick176(.025f*n);
  }
  if(asynchronousH5){
   ck(g_native_guard112.pendingPress46&&g_native_guard112.pendingMotionH5&&!g_native_guard112.held,
      "H5 absent native slot retains same-attack motion while release awaits acknowledgement");
   put(state.data()+0x308,std::uint8_t(0));tick176(.10f);
  }
  ck(pair176()&&g_native_guard112.held&&pt::Read(edge176)&&edge176.cause==pt::Cause::Motion&&edge176.opponentId==456,
     "H5 fresh incoming attack plus deliberate movement delivers one exact-pair native block input without +778");
  for(int n=0;n<14;++n)tick176(.10f); // preserve existing native input cooldown
  blockModes176.clear();put(state.data()+0x778,1);tick176(.10f);
  ck(edges176().empty(),"H5 a later native window cannot replay the already-spent physical beat");
  // H20: after that old motion is refused, a fresh beat in the SAME attack's
  // newly opened native window must not be vetoed by the spent early beat.
  for(int n=1;n<=6;++n)tick176(.10f+.02f*n);
  ck(pair176()&&g_native_guard112.held&&pt::Read(edge176)&&edge176.cause==pt::Cause::NativeWindow,
     "H20 new deliberate motion in native PB opening survives earlier ordinary-block beat");
  blockModes176.clear();for(int n=0;n<8;++n)tick176(.22f);
  ck(edges176().empty(),"H20 native opening and consumed counter-motion cannot repeat");
  put(foeState176.data()+0x38,8);tick176(.22f);
  put(foeState176.data()+0x38,4);tick176(.22f);
  for(int n=0;n<5;++n)tick176(.22f);
  ck(edges176().empty(),"H5 old pre-attack movement cannot refresh block for the next combo strike");
 }
 #include "native_block_parry_ack_h14_cases.inl"
 g_guard_auto_windup19c4r1=savedAutoWindup19c4r1;g_guard_countermotion57c1=savedMotion57c1;g_guard_incoming_repress45=savedAssist55c2;put(state.data()+0x778,0);
 blockCallback176={};g_guard_manual112=false;g_guard_manual_owner112=nullptr;put(state.data()+0xCE0,actor.data());NativeGuardRelease112();
 put(state.data()+0xCF8,static_cast<void*>(nullptr));put(state.data()+0x308,std::uint8_t(0));put(state.data()+0x5D8,0);put(state.data()+8,std::uint8_t(0));
 put(hands.data()+0x260,std::uint64_t(77));put(hands.data()+0x268,std::uint64_t(0));frame=savedFrame176;g_phys_frame6698.publish(frame);g_census_mode=savedMode176;g_weapon_control_mode=savedWeapon176;
 g_combat_experiment175=savedExperiment176;g_cal_state=savedCal176;g_cal_frame_qx=savedQx176;g_cal_frame_qy=savedQy176;g_cal_frame_qz=savedQz176;g_cal_frame_qw=savedQw176;
 g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;diagnostics::lab::Set(diagnostics::lab::Key::F3,savedF3176);diagnostics::capture168::gate.mask=savedCapture176;pt::Clear();
}
