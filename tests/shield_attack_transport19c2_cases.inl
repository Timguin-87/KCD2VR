// Actual completed physical producer + native queue consumer. The native leaves
// are the same counted OnAction/input-class model as the surrounding fixture.
{
 const auto savedFrame=frame;const auto savedCommand=command;
 ph::Input pi{};ph::State ps{};std::array<ph::Profile,2> pp{};ph::Output po{};
 pi.allowed=pi.tracked[0]=pi.tracked[1]=pi.intentTracked[0]=pi.intentTracked[1]=true;
 pi.epoch=pi.bodyEpoch=1;pi.sample=1;pi.tick=1000;pi.grip[0]=pi.grip[1]=1;
 pi.coherentMelee119=pi.naturalCombat117=pi.positionGuard176=pi.heldMotionAttacks=true;
 pi.dynamics=false;pi.contact=false;pi.controllerAnchor119=true;
 pi.shoulders[0]={.15,0,1.3};pi.shoulders[1]={-.15,0,1.3};pi.reach[0]=pi.reach[1]=1.3;
 pi.hands[0].p={-.25,.2,1.2};pi.hands[1].p={-.4,.3,1.4};
 pi.intentHands[0]=pi.hands[0];pi.intentHands[1]=pi.hands[1];
 pp[0]=frame.profiles[0];pp[0].tip={0,.6,0};pp[0].axis={0,1,0};pp[0].grip[0]={};
 pp[1]=pp[0];pp[1].key=88;pp[1].side=1;pp[1].classId=8;
 auto step19=[&](double travel){++pi.sample;pi.tick+=10;pi.hands[0].p.y+=travel;pi.intentHands[0]=pi.hands[0];
  po=ph::step(ps,pi,pp,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.;});};
 for(int i=0;i<20;++i)step19(0);
 bool produced19=false;
 for(int i=0;i<15&&!produced19;++i){step19(.02);for(unsigned j=0;j<po.commandCount;++j)
  if(po.commands[j].kind==ph::CommandKind::Prepare){command=po.commands[j];produced19=true;}}
 ck(produced19&&po.strokeActive119&&!po.guardBySide30c1[0]&&po.guardBySide30c1[1]&&po.guard117,
    "C19C2 actual producer recognizes primary forward attack while offhand shield keeps aggregate guard");
 frame.input.positionGuard176=true;frame.output.guard117=po.guard117;
 frame.output.guardBySide30c1[0]=po.guardBySide30c1[0];frame.output.guardBySide30c1[1]=po.guardBySide30c1[1];
 frame.output.strokeActive119=true;frame.output.active[0]=frame.output.owned[0]=true;
 frame.input.attacks=frame.input.allowed=frame.input.tracked[0]=true;frame.output.handlingOnly55c1[0]=false;
 // Isolate the command transport from the separate native guard-tick fixture:
 // exact physical decisions above feed the actual consumer, not a copied gate.
 for(auto kind:{ph::CommandKind::Slash,ph::CommandKind::Thrust}){
  PhysAbortAttack117(player.data());g_phys_native6698.attackPrepared=false;g_phys_input6698=true;
  g_phys_commands6698.head=g_phys_commands6698.tail=0;publish117();
  command.key=frame.profiles[0].key;command.generation=frame.profiles[0].generation;
  command.epoch=frame.request;command.bodyEpoch=frame.input.bodyEpoch;command.side=0;
  command.kind=ph::CommandKind::Prepare;command.stroke117=kind;command.zone117=kind==ph::CommandKind::Thrust?5:2;
  command.sample=frame.input.sample;command.tick=hostTickCount64;command.gesture59c3=frame.input.sample;
  const auto starts=begins117,ends=commits117;
  g_phys_commands6698.push(command);command.kind=kind;g_phys_commands6698.push(command);
  NativeFrameBegin104();PhysNativeTickImpl6698();
  ck(g_phys_native6698.attackPrepared&&begins117==starts+1&&commits117==ends,
     "C19C2 raised secondary shield cannot discard actual native cut/stab prepare");
  ++hostTickCount64;NativeFrameBegin104();PhysNativeTickImpl6698();
  ck(!g_phys_native6698.attackPrepared&&commits117==ends+1&&g_phys_commands6698.head==g_phys_commands6698.tail,
     "C19C2 primary cut/stab releases once after native frame boundary despite secondary shield guard");
  ++hostTickCount64;NativeFrameBegin104();PhysNativeTickImpl6698();
  ck(commits117==ends+1,"C19C2 consumed primary strike cannot replay through shield exception");
 }
 for(int bad=0;bad<5;++bad){
  PhysAbortAttack117(player.data());g_phys_native6698.attackPrepared=false;g_phys_input6698=true;
  frame.output.guardBySide30c1[0]=bad==0;frame.output.handlingOnly55c1[0]=bad==1;frame.input.tracked[0]=bad!=2;
  publish117();command.kind=ph::CommandKind::Prepare;command.stroke117=ph::CommandKind::Slash;
  command.sample=frame.input.sample;command.tick=hostTickCount64;command.generation=frame.profiles[0].generation+(bad==3);
  if(bad==4)command.tick-=101;
  const auto starts=begins117,ends=commits117;g_phys_commands6698.push(command);NativeFrameBegin104();PhysNativeTickImpl6698();
  ck(begins117==starts&&commits117==ends,
     "C19C2 same-side guard, handling-only, lost tracking, wrong item generation or stale command remains refused");
 }
 frame=savedFrame;command=savedCommand;PhysAbortAttack117(player.data());g_phys_native6698.attackPrepared=false;
 g_phys_commands6698.head=g_phys_commands6698.tail=0;publish117();
}
