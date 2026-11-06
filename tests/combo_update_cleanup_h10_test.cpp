// Actual physical recognizer -> completed H30 -> earned native OPEN ->
// original action cleanup/read continuation. Native memory/input counter leaves
// remain explicit, and the actual native machinecode composition is separate.
#define KCD2VR_COMBO_PUBLICATION_PREFIX_ONLY
#include "combo_publication_h8_test.cpp"
static unsigned checks10=0,failed10=0;
static void check10(bool ok,const char* label){++checks10;failed10+=!ok;std::printf("%s %s\n",ok?"PASS":"FAIL",label);}
static void rawValue10(const void* p,unsigned off,std::uintptr_t value){links[{reinterpret_cast<std::uintptr_t>(p),off}]=value;}
struct CleanupH10:PublicationH8 {
 CleanupH10(bool stab=false):PublicationH8(stab){publish(true);rawValue10(state,0x38,16);rawValue10(state,0x488,1);rawValue10(state,0x868,1);}
 void cleanup(bool remove=true){combo59c17::ObserveOrdinaryCleanupH10(state,now);rawValue10(state,0x488,0);rawValue10(state,0x38,2);if(remove)set(actionSlotsH8,0,nullptr);}
 bool keep(const void* whichTrigger=nullptr,const void* whichActor=nullptr,std::uintptr_t caller=0){
  return combo59c17::KeepUpdateHitReadH10(whichTrigger?whichTrigger:trigger.data(),whichActor?whichActor:actor,caller?caller:g_base+0x90874E,now);}
};
#ifndef KCD2VR_COMBO_CLEANUP_PREFIX_ONLY
int main(){
 for(bool stab:{false,true})for(bool remove:{false,true}){
  CleanupH10 f(stab);check10(combo59c17::g_update_continuation_h10.trigger==reinterpret_cast<std::uintptr_t>(f.trigger.data()),"actual native OPEN captures exact trigger/action/input identity");
  check10(!f.keep(),"a native OPEN alone cannot suppress a later failed hit read before ordinary cleanup");
  f.cleanup(remove);check10(f.keep()&&f.retained(),"observed original ordinary cleanup retains only original finite UPDATE hit read");
  check10(Rd<std::uint8_t>(state,0x488,1)==0,"read continuation never restores native480 property");
  check10(!f.keep(nullptr,nullptr,g_base+0x9111BB),"phase16 newOPEN eligibility cannot borrow UPDATE continuation");
  check10(!f.keep(reinterpret_cast<void*>(0x77000))&&!f.keep(nullptr,targetA),"another trigger or actor cannot borrow the receipt");
  for(unsigned i=0;i<100;++i){f.rig.step();now=f.rig.in.tick;f.next={f.rig.in,f.rig.out,f.rig.profiles,f.rig.in.epoch};f.publish(true);}
  check10(f.keep()&&f.retained()&&timers==1,"quiet completed poses preserve original clock without new input or rearm");
 }
 for(unsigned kind=0;kind<21;++kind){CleanupH10 f;f.cleanup();
  if(kind==0)++inputGenerationH10; // next real native begin/release, even if native rejects it
  if(kind==1)set(actionSlotsH8,0,reinterpret_cast<void*>(0x88000));
  if(kind==2)set(actor,0x200,reinterpret_cast<void*>(0x89000));
  if(kind==3)set(actionManagerH8,0x38,nullptr);
  if(kind==4)set(state,0xCF8,targetB);
  if(kind==5)++kcd2vr::contact176::recoveryEpoch55c1;
  if(kind==6)++currentIntentAdapterH8::g_pose_epoch6689;
  if(kind==7)++currentIntentAdapterH8::g_weapon_request_epoch6694;
  if(kind==8)currentIntentAdapterH8::g_ctx_menu=true;
  if(kind==9)currentIntentAdapterH8::g_ctx_dialogue=true;
  if(kind==10)currentIntentAdapterH8::g_ctx_cutscene=true;
  if(kind==11)currentIntentAdapterH8::g_ctx_scripted=true;
  if(kind==12)currentIntentAdapterH8::animation_filter::nativeBodyOwned112=true;
  if(kind==13)currentIntentAdapterH8::hand[0]=false;
  if(kind==14){++f.next.profiles[0].generation;f.publish(true);}
  if(kind==15){++f.next.profiles[0].key;f.publish(true);}
  if(kind==16)f.triggerPhase(4);
  if(kind==17)f.triggerPhase(64);
  if(kind==18)rawValue10(state,0x38,4);
  if(kind==19){while(f.rig.in.tick<=f.original.owner.tick+1500)f.rig.step();now=f.rig.in.tick;f.next={f.rig.in,f.rig.out,f.rig.profiles,f.rig.in.epoch};f.publish(true);}
  if(kind==20)std::memcpy(f.trigger.data()+0x38,&targetB,8);
  check10(!f.keep()&&timers==1,"new input/action/item/lifecycle/context/real interruption/original expiry cannot inherit UPDATE continuation");
 }
 for(unsigned kind=0;kind<7;++kind){CleanupH10 f;
  if(kind==0)rawValue10(state,0x488,0);
  if(kind==1)rawValue10(state,0x38,64);
  if(kind==2)rawValue10(state,0x38,4);
  if(kind==3)set(actionSlotsH8,0,reinterpret_cast<void*>(0x88000));
  if(kind==4)set(actionSlotsH8,0,nullptr);
  if(kind==5)++inputGenerationH10;
  if(kind==6)std::memcpy(f.trigger.data()+0x38,&targetB,8);
  f.cleanup();check10(!f.keep(),"cleanup without original live MAIN/hit/nonterminal/input identity cannot acquire continuation");
 }
 for(int phase:{1,128}){CleanupH10 f;rawValue10(state,0x38,phase);f.cleanup();rawValue10(state,0x38,phase);
  check10(f.keep(),"idle/guard phase alone does not cancel the exact earned native slot after ordinary cleanup");}
 {CleanupH10 f;f.cleanup();combo59c17::ClearLineage19();
  check10(combo59c17::g_update_continuation_h10.trigger!=0,"control-plane retirement does not write the native-thread receipt");
  check10(!f.keep()&&combo59c17::g_update_continuation_h10.trigger==0,"native read retires receipt after atomic control-plane lineage invalidation");}
 {CleanupH10 f;std::memcpy(f.trigger.data()+0x38,&targetA,8);f.cleanup();check10(f.keep(),"same explicit native trigger target may retain the original earned hit read");}
 {CleanupH10 f;f.cleanup();f.managerPhase(4);check10(!f.keep(),"manager's genuine next attack independently retires post-cleanup read ownership");}
 std::printf("%s combo cleanup %u/%u\n",failed10?"FAIL":"PASS",checks10-failed10,checks10);return failed10?1:0;
}
#endif
