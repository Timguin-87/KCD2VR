// Actual physical recognizer -> actual H30 frame owner -> actual original-first
// combo callbacks. Pause the pose producer AFTER its mailbox publication and
// BEFORE its command-completion sample store, in the real publication order.
// Native OPEN/APPLIED/timer and retail memory are explicit host leaves.
#define KCD2VR_PUBLIC_REARM_PREFIX_ONLY
#include "combo_public_rearm_h8_test.cpp"

static unsigned checks=0,failed=0;
static void check(bool ok,const char* label){++checks;failed+=!ok;std::printf("%s %s\n",ok?"PASS":"FAIL",label);}
static const void* actionManagerH8=reinterpret_cast<void*>(0x31000);
static const void* actionSlotsH8=reinterpret_cast<void*>(0x32000);
static std::uint64_t readyPublishValueH8=0;
static void storeReadyDuringReadH8(){currentIntentAdapterH8::g_phys_ready_sample6698=readyPublishValueH8;}
struct PublicationH8 {
 RigH30 rig;cg::Owner paid{};cg::SlotLeaseF1 original{};
 currentIntentAdapterH8::PhysFrame6698 next{};
 alignas(16) std::array<unsigned char,128> trigger{};
 PublicationH8(bool stab=false):rig(false,stab?ph::Vec3{0,1,0}:ph::Vec3{1,0,0}){
  setup();combo59c17::g_teardown_pending19=false;combo59c17::g_ready=true;
  combo59c17::g_timer_fault_h19=false;combo59c17::g_window_ms=1500;
  kcd2vr::routerH3::mode=0;expectedTimerH3=1.5f;
  currentIntentAdapterH8::g_pose_epoch6689=4;currentIntentAdapterH8::g_weapon_request_epoch6694=5;
  currentIntentAdapterH8::g_gameplay_allowed=true;
  currentIntentAdapterH8::g_ctx_menu=currentIntentAdapterH8::g_ctx_dialogue=false;
  currentIntentAdapterH8::g_ctx_cutscene=currentIntentAdapterH8::g_ctx_scripted=currentIntentAdapterH8::g_ctx_reading=false;
  currentIntentAdapterH8::animation_filter::nativeBodyOwned112=false;
  currentIntentAdapterH8::head=currentIntentAdapterH8::hand[0]=currentIntentAdapterH8::hand[1]=currentIntentAdapterH8::held=true;
  currentIntentFrameH30=&currentIntentAdapterH8::GeometryIntentOwnerH30;
  currentIntentStateFrameH30=&currentIntentAdapterH8::GeometryIntentOwnerStateH30;
  rig.targetId=11;rig.state={};rig.in.bodyEpoch=4;rig.in.epoch=5;
  rig.profiles[0].key=2;rig.profiles[0].generation=3;
  for(unsigned i=0;i<30;++i)rig.step();rig.commands.clear();moveH30(rig,14);
  const auto* release=firstH30(rig,stab?ph::CommandKind::Thrust:ph::CommandKind::Slash);assert(release);
  currentIntentAdapterH8::publish(rig);assert(frik_pose_probe::GeometryIntentOwnerH30(2,3,4,5));
  paid=publicOwnerH8(rig,*release);
  set(actor,0x200,actionManagerH8);set(actionManagerH8,0x38,actionSlotsH8);set(actionSlotsH8,0,actionA);
  triggerMemory=trigger.data();std::memcpy(trigger.data()+8,&actor,sizeof(actor));
  trigger[0x18]=trigger[0x19]=trigger[0x1B]=1;
  const float duration=.3f;std::memcpy(trigger.data()+0x1C,&duration,4);
  {cg::OfferScope offer(&paid);kcd2vr::geometry40c1::Scope originalHit(actor,reinterpret_cast<void*>(1),targetA);
   geom27::g_inject_result178r35={true,true,true,1,11};
   combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,16);}
  std::uint64_t consumed=0;assert(cg::nativeSlot.read(original,consumed)&&timers==1);
  rig.step();now=rig.in.tick;next={rig.in,rig.out,rig.profiles,rig.in.epoch};
 }
 ~PublicationH8(){currentIntentFrameH30=nullptr;currentIntentStateFrameH30=nullptr;currentIntentAdapterH8::onHeldReadH8=nullptr;triggerMemory=nullptr;}
 void publish(bool complete=false){assert(currentIntentAdapterH8::g_phys_frame6698.publish(next));
  if(complete)currentIntentAdapterH8::g_phys_ready_sample6698=next.input.sample;}
 void complete(){currentIntentAdapterH8::g_phys_ready_sample6698=next.input.sample;}
 bool retained(){cg::SlotLeaseF1 after{};std::uint64_t consumed=0;
  return cg::nativeSlot.read(after,consumed)&&after.owner.tick==original.owner.tick&&after.window==original.window&&
    after.owner.release==original.owner.release&&after.owner.action==original.owner.action&&timers==1;}
 void managerPhase(int phase){combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,phase);}
 void triggerPhase(int phase){combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,phase);}
};
#ifndef KCD2VR_COMBO_PUBLICATION_PREFIX_ONLY
int main(){
 for(bool stab:{false,true}){
  {PublicationH8 f(stab);f.publish(true);f.managerPhase(1);
   check(managerCalls.empty()&&f.retained(),"completed-frame control keeps original earned slot");}
  {PublicationH8 f(stab);f.publish();
   check(frik_pose_probe::GeometryIntentOwnerStateH30(2,3,4,5)==2,"same-owner new frame is publication-in-progress");
   check(!frik_pose_probe::GeometryIntentOwnerH30(2,3,4,5),"incomplete frame grants no ordinary input/contact owner");
   check(!combo59c17::Active(actor,now)&&f.retained(),"ordinary Active query grants nothing but preserves earned slot");
   f.managerPhase(1);f.managerPhase(2);f.triggerPhase(1);
   check(managerCalls.empty()&&triggerCalls==std::vector<int>({16}),"idle/guard reset and idle close wait on original finite slot");
   check(f.retained(),"waiting preserves exact original anchor action release and one timer call");
   f.complete();check(frik_pose_probe::GeometryIntentOwnerH30(2,3,4,5),"completed sample restores current ordinary intent owner");
   f.managerPhase(1);check(managerCalls.empty()&&f.retained(),"completion restores existing continuation without rearming");
   // Several normal publish/complete cycles cannot refresh the initial clock.
   for(unsigned i=0;i<12;++i){f.rig.step();now=f.rig.in.tick;f.next={f.rig.in,f.rig.out,f.rig.profiles,f.rig.in.epoch};
    f.publish();f.managerPhase(1);f.complete();}
   check(managerCalls.empty()&&f.retained(),"repeated in-progress publications never refresh original slot");
  }
 }
 for(unsigned value:{0u,1u,2u}){PublicationH8 f;f.publish();
  // Inject the atomic store from the explicit native-held read leaf AFTER the
  // actual owner function's first ready load, before its final consistency read.
  readyPublishValueH8=value==0?0:value==1?f.next.input.sample:f.next.input.sample+1;
  currentIntentAdapterH8::onHeldReadH8=&storeReadyDuringReadH8;
  const auto state=frik_pose_probe::GeometryIntentOwnerStateH30(2,3,4,5);
  check(state==(value==1?1u:0u),"exact completion may become current; zero or third ready value remains lost");
  f.managerPhase(1);
  check(value==1?(managerCalls.empty()&&f.retained()):managerCalls==std::vector<int>({1}),
        "ready changes during witness read do not borrow incomplete or third-frame authority");
 }
 // Losing the actual owner, a completed false witness or a conflicting newer
 // frame still retires it immediately. No previous proof is replayed.
 for(unsigned kind=0;kind<29;++kind){PublicationH8 f;
  bool complete=false;
  if(kind==0){f.next.output.normalIntentH30[0].current=false;complete=true;}
  if(kind==1)f.next.output.normalIntentH30[0].current=false;
  if(kind==2)++f.next.profiles[0].key;
  if(kind==3)++f.next.profiles[0].generation;
  if(kind==4)++f.next.input.bodyEpoch;
  if(kind==5)++f.next.request;
  if(kind==6)++currentIntentAdapterH8::g_pose_epoch6689;
  if(kind==7)++currentIntentAdapterH8::g_weapon_request_epoch6694;
  if(kind==8)currentIntentAdapterH8::g_ctx_menu=true;
  if(kind==9)currentIntentAdapterH8::g_ctx_dialogue=true;
  if(kind==10)currentIntentAdapterH8::g_ctx_cutscene=true;
  if(kind==11)currentIntentAdapterH8::g_ctx_scripted=true;
  if(kind==12)currentIntentAdapterH8::g_ctx_reading=true;
  if(kind==13)currentIntentAdapterH8::g_gameplay_allowed=false;
  if(kind==14)currentIntentAdapterH8::animation_filter::nativeBodyOwned112=true;
  if(kind==15)currentIntentAdapterH8::head=false;
  if(kind==16)currentIntentAdapterH8::hand[0]=false;
  if(kind==17)currentIntentAdapterH8::held=false;
  if(kind==18)currentIntentAdapterH8::g_phys_ready_sample6698=0;
  if(kind==19)now+=81;
  if(kind==20)f.next.input.tick=now+1;
  if(kind==21)f.next.input.sample=currentIntentAdapterH8::g_phys_ready_sample6698-1;
  if(kind==22)currentIntentAdapterH8::g_phys_ready_sample6698=f.next.input.sample+1;
  if(kind==23)set(state,0xCF8,targetB);
  if(kind==24)set(actor,0x1F8,reinterpret_cast<void*>(0x33000));
  if(kind==25)set(actor,0x210,reinterpret_cast<void*>(0x34000));
  if(kind==26)set(actor,0x3B0,reinterpret_cast<void*>(0x35000));
  if(kind==27)++kcd2vr::contact176::recoveryEpoch55c1;
  if(kind==28)mode=1;
  f.publish(complete);f.managerPhase(1);
  cg::SlotLeaseF1 after{};std::uint64_t consumed=0;
  check(managerCalls==std::vector<int>({1})&&!cg::nativeSlot.read(after,consumed),"actual lost/conflicting owner forwards native reset exactly once and retires slot");
  check(timers==1,"owner loss cannot invent or refresh native OPEN timer");
 }
 for(unsigned kind=0;kind<4;++kind){PublicationH8 f;
  if(kind==0)set(actionSlotsH8,0,reinterpret_cast<void*>(0x36000));
  if(kind==1)set(actionManagerH8,0x38,nullptr);
  if(kind==2)set(actor,0x200,nullptr);
  if(kind==3)geom27::vrstate596::g_henry_actor=reinterpret_cast<void*>(0x37000);
  f.publish();f.managerPhase(1);
  check(managerCalls==std::vector<int>({1})&&timers==1,"replacement native action manager slots or actor cannot defer reset");
  geom27::vrstate596::g_henry_actor=actor;
 }
 {PublicationH8 f;cg::nativeSlot.retire();f.publish();f.managerPhase(1);
  check(managerCalls==std::vector<int>({1})&&timers==1,"publication gap alone cannot create an earned native slot");}
 {PublicationH8 f;cg::nativeSlot.retire();assert(cg::grace.publish(f.paid));f.publish();f.managerPhase(1);
  check(managerCalls==std::vector<int>({1})&&timers==1,"accepted-hit grace alone cannot borrow publication wait authority");}
 {PublicationH8 f;
  // Obtain genuinely fresh later recognizer frames while leaving the original
  // lease tick untouched. The original 1500ms boundary remains inclusive.
  while(f.rig.in.tick<f.original.owner.tick+1500)f.rig.step();now=f.rig.in.tick;
  f.next={f.rig.in,f.rig.out,f.rig.profiles,f.rig.in.epoch};f.publish();f.managerPhase(1);
  check(managerCalls.empty()&&f.retained(),"original finite deadline is inclusive while publication is in progress");
  f.complete();f.rig.step();now=f.rig.in.tick;f.next={f.rig.in,f.rig.out,f.rig.profiles,f.rig.in.epoch};f.publish();f.managerPhase(1);
  check(managerCalls==std::vector<int>({1})&&timers==1,"original expiry forwards reset once despite fresh same-owner pending frame");
 }
 for(int phase:{4,64}){PublicationH8 f;f.publish();f.managerPhase(phase);f.triggerPhase(phase);
  check(managerCalls==std::vector<int>({phase})&&triggerCalls==std::vector<int>({16,phase})&&timers==1,
        "native terminal4/64 callbacks remain original once without new timer");
  if(phase==64){cg::SlotLeaseF1 after{};std::uint64_t consumed=0;check(!cg::nativeSlot.read(after,consumed),"native64 retires original lease");}
 }
 {PublicationH8 f;f.publish();f.triggerPhase(16);
  check(timers==1&&f.retained(),"incomplete frame cannot start or extend another native OPEN");}
 std::printf("%s combo publication %u/%u checks; native OPEN/APPLIED/timer are host leaves, no runtime finisher claim\n",
   failed?"FAIL":"PASS",checks-failed,checks);
 return failed?1:0;
}
#endif
