// Connected D40R1: real producer -> immutable Use -> Queue/current root/Attach
// fallback -> real transfer detour. Only native leaves use the enclosing fixture.
{
 const auto requestD40R1=[&](std::uint32_t offered,int route){
  unknownPickingDefinition=false;beginUse55();
  SetPickupHold62(false);SetPickupExperiment62(false);
  pickup_use19c3::setNearOnlyD40(true);
  PickupProducer19 p;p.f5Mode=0;p.comparison114=false;
  for(const auto tick:{1000u,1020u,1040u}){
   candidate(tick,"PickableItem",.08f);interaction_alignment::testCandidate.entity=offered;
   p.tick(tick,tick==1000?0.f:1.f);
   // The pre-Use native controller observation belongs to the same current
   // preference generation as the physical request, after pending admission.
   if(tick==1020){frame(tick);UpdateDetour(controller.data(),.02f);}
  }
  Use13 use{};assert(g_use13.read(use)&&use.physicalPickup63&&use.expectedEntityD40==offered&&
      use.tick==1040&&p.touch_bindings_.pulses==1);
  // A later adjacent hover cannot change the transported target/serial.
  candidate(1060,"PickableItem",.08f);interaction_alignment::testCandidate.entity=offered+1;p.tick(1060,1);
  Use13 unchanged{};assert(g_use13.read(unchanged)&&unchanged.serial==use.serial&&unchanged.expectedEntityD40==offered);
  installPicking55();frame(1060);testAttachManager=manager.data();
  if(route==0)UpdateDetour(controller.data(),.02f);
  if(route==1){QueueDetour(controller.data(),action.data(),0);TickOwner(controller.data(),1060);}
  ng13::Acquisition54 pre{};
  // Current Picking still preserves its original HMD/tracking/reference
  // authority. Item presentation requires the separately proven held binding.
  if(route!=2)assert(g_owner.state==Playing&&g_owner.expectedEntityD40==offered&&ReadAcquisition54(pre)&&pre.active&&!pre.graspEntity&&!pre.graspToken);
  ng13::Binding held{};assert(!g_pick13.attachStarted&&!ReadGraspBinding(held)&&attaches==0&&transfers==0);
  return use;
 };
 for(int route=0;route<3;++route){
  // Offered adjacent A=73, accepted native B=72. Neither the Queue nor root
  // identity may grant B a controller hold on the strength of A's request.
  const auto wrong=requestD40R1(73,route);attach();
  ng13::Binding binding{};ng13::Acquisition54 acquisition{};
  assert(transfers==1&&notices==1&&!g_pick13.attachStarted&&!ReadGraspBinding(binding)&&
      !ReadAcquisition54(acquisition)&&g_owner.state==Idle&&!UseLive55(wrong)&&finishes==0);
  const auto right=requestD40R1(72,route);attach();
  assert(transfers==0&&notices==1&&attaches==1&&ReadGraspBinding(binding)&&binding.entity==72&&
      ReadAcquisition54(acquisition)&&acquisition.active&&acquisition.serial==right.serial&&
      acquisition.graspEntity==72&&g_owner.useInput55.epoch==right.input55.epoch&&
      g_owner.useInput55.trackingEpoch==right.input55.trackingEpoch);
  frame(1080,0);TickOwner(controller.data(),1080);assert(drops==1&&transfers==0&&!UseLive55(right));
 }
 // Exact native animation-job callback mismatch retires presentation before
 // returning to the controller, without replay or native cancellation.
 {
  const auto use=requestD40R1(73,0);
  std::thread job([]{pickupThread19c4=19740;attach();});job.join();
  ng13::Acquisition54 acquisition{};assert(transfers==1&&!g_pick13.attachStarted&&!ReadAcquisition54(acquisition));
  TickOwner(controller.data(),1060);assert(g_owner.state==Idle&&!UseLive55(use)&&finishes==0);
 }
 // The same entity cannot revive this serial across body/tracking/context
 // generations. The native transfer still runs once and owns its own effects.
 for(int change=0;change<3;++change){
  const auto use=requestD40R1(72,0);
  if(change==2){SetContextAvailable(false);SetContextAvailable(true);}
  else {ng13::AcquisitionInput55 input{};g_acquisition_input55.read(input);
   if(change==0)++input.epoch;else ++input.trackingEpoch;PublishAcquisitionInput55(input);}
  attach();assert(transfers==1&&!g_pick13.attachStarted&&!UseLive55(use)&&finishes==0);
 }
 // A native pickup can naturally lose its world link before this interception.
 // An unavailable identity means native fallback, never an inferred match.
 requestD40R1(72,0);put(item.data(),0x88,static_cast<void*>(nullptr));attach();
 assert(transfers==1&&!g_pick13.attachStarted&&g_owner.state==Idle&&finishes==0);
 // No target expectation cannot accidentally become a stamped physical Use.
 beginUse55();Use13 before{},after{};g_use13.read(before);NotePhysicalPickupUse63(0,1000,0);g_use13.read(after);
 assert(after.serial==before.serial&&!after.physicalPickup63&&!after.expectedEntityD40);
 pickup_use19c3::setNearOnlyD40(savedReachD40);
 std::puts("PASS D40R1 actual producer/Queue/root/Attach target join: adjacent mismatch native-once, current match, immutable serial, body/tracking/context retirement, missing world link");
}
