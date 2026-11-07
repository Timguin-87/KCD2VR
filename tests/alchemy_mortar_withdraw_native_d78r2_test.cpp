// Actual native owner and actual native attachment reader, with explicit
// native-call/memory doubles inherited from the existing adapter fixture.
#define KCD2VR_NATIVE_MORTAR69_NO_MAIN
#include "subsystems/alchemy/native_mortar69_test.cpp"
static void waitingR2(){prepare69();put(memory_parent.data(),0x70,3);put(memory_parent.data(),0x90,req69.child);tick69();ck(!cancels&&g_mortar_stop69.active,"native work still owns the current cycle");}
static void unreadableContextR2(std::array<unsigned char,0x1000>& memory,std::size_t offset){
 // Remove only the context field from readable memory, retaining the action
 // class, owner, status and all other native validation bytes.
 for(auto it=regions.begin();it!=regions.end();++it)if(it->first==memory.data()&&it->second==memory.size()){
  regions.erase(it);regions.push_back({memory.data(),offset});
  regions.push_back({memory.data()+offset+sizeof(void*),memory.size()-offset-sizeof(void*)});return;
 }
 ck(false,"context-read fixture region must exist");
}
static void withdrawalCasesR2(){
 waitingR2();auto withdrawal=req69;withdrawal.withdrawTickD78R2=1000;mortar69::request.publish(withdrawal);tick69();
 ck(!cancels&&result69().status==mortar69::Status::WithdrawnD78R2,"unspent exact tilt withdrawal never enters native Stop");
 ck(result69().stopEntry72==mortar69::StopEntry72::NotEntered&&!g_mortar_stop69.active,"withdrawal acknowledges actual no entry");
 ck(!detaches&&!homes&&slots[1]==req69.parent,"withdrawal does not mutate pair, native parent or contents");
 put(memory_parent.data(),0x70,2);tick69();ck(!cancels,"withdrawn serial cannot later Stop at native READY");
 // A delayed message can retire an unspent request before it starts; it
 // cannot gain permission for a stale native call.
 prepare69();withdrawal=req69;withdrawal.withdrawTickD78R2=1100;mortar69::request.publish(withdrawal);
 TickMortarStop69(memory_controller.data(),1100);ck(!cancels&&result69().status==mortar69::Status::WithdrawnD78R2,"withdrawal of a never-entered stale request is harmless retirement");
 for(int fault=0;fault<15;++fault){waitingR2();withdrawal=req69;withdrawal.withdrawTickD78R2=1000;
  switch(fault){case 0:++withdrawal.serial;break;case 1:++withdrawal.tick;break;case 2:++withdrawal.poseEpoch;break;case 3:++withdrawal.sourceEpoch;break;
   case 4:++withdrawal.modeEpoch;break;case 5:++withdrawal.parent;break;case 6:++withdrawal.child;break;case 7:++withdrawal.left.object;break;
   case 8:++withdrawal.right.attachment;break;case 9:++withdrawal.left.lifetime;break;case 10:++withdrawal.right.entity;break;
   case 11:++withdrawal.left.action;break;case 12:++withdrawal.right.actionClass;break;case 13:withdrawal.withdrawTickD78R2=999;break;case 14:withdrawal.withdrawTickD78R2=1001;break;}
  mortar69::request.publish(withdrawal);tick69();ck(!cancels&&result69().status==mortar69::Status::WaitingNativeCycle,"foreign or future withdrawal cannot overwrite original owner");
 }
 // Native Stop has already entered but is still waiting on child retirement.
 // A late withdrawal cannot reinterpret that operation as unspent.
 prepare69();behavior=7;put(memory_parent.data(),0x90,req69.child);put(memory_scope.data(),kScopeAction,req69.child);tick69();
 ck(cancels==1&&g_mortar_stop69.active&&g_mortar_stop69.attempted,"entered native Stop awaits its child retirement");
 withdrawal=req69;withdrawal.withdrawTickD78R2=1000;mortar69::request.publish(withdrawal);tick69();
 ck(cancels==1&&g_mortar_stop69.active&&result69().status!=mortar69::Status::WithdrawnD78R2,"late withdrawal cannot revoke entered native Stop");
 put(memory_scope.data(),kScopeAction,std::uintptr_t(0));tick69();
 ck(cancels==1&&result69().status==mortar69::Status::ReadyForNativePlace&&result69().stopEntry72==mortar69::StopEntry72::Entered,"native completion remains authoritative after late withdrawal");
}
static void waitCasesR2(){
 // Runtime D78 phase3 outlived the old 5s phase watchdog. A still-current
 // native cycle remains eligible while its actual child active-time progresses.
 // R96: retain the D85 production progress requirement in this older fixture.
 waitingR2();auto& frame=kcd2vr::frik_pose_probe::frame;
 frame.tick=7500;frame.timeNs=7500000000ull;++frame.sequence;put(memory_child.data(),kActActiveTime,1.f);
 TickMortarStop69(memory_controller.data(),7500);
 ck(!cancels&&result69().status==mortar69::Status::WaitingNativeCycle,"native working cycle beyond five seconds retains its original request");
 put(memory_parent.data(),0x70,2);TickMortarStop69(memory_controller.data(),7500);
 ck(cancels==1&&result69().status==mortar69::Status::ReadyForNativePlace,"same long native cycle can complete through one exact Stop at READY");
 for(int fault=0;fault<7;++fault){waitingR2();
  switch(fault){case 0:put(memory_parent.data(),0x70,1);break;case 1:put(memory_parent.data(),0x90,std::uintptr_t(0));break;
   case 2:put(memory_child.data(),0,std::uintptr_t(0));break;case 3:put(memory_child.data(),0x268,std::uintptr_t(1));break;
   case 4:put(memory_child.data(),kActStatus,1);break;case 5:put(memory_child.data(),kActContext,static_cast<void*>(nullptr));break;
   case 6:kcd2vr::activity_input163::witness175.lifetime175++;break;}
  if(fault<2)tick69(); // changed phase/child is first observed at the original sample
  frame.tick=7500;frame.timeNs=7500000000ull;++frame.sequence;put(memory_child.data(),kActActiveTime,1.f);
  TickMortarStop69(memory_controller.data(),7500);
  ck(!cancels&&result69().status==mortar69::Status::Revoked,"unqualified current native work cannot receive the longer wait");
 }
 waitingR2();frame.tick=16001;frame.timeNs=16001000000ull;++frame.sequence;
 TickMortarStop69(memory_controller.data(),16001);
 ck(!cancels&&result69().status==mortar69::Status::Revoked&&result69().stopEntry72==mortar69::StopEntry72::NotEntered,"stalled native child still refuses an uncompleted cycle without progress");
}
static void contextCasesR2(bool readsOnly=false){
 for(int fault=readsOnly?1:0;fault<5;++fault){waitingR2();
  switch(fault){
  case 0:put(memory_child.data(),kActContext,static_cast<void*>(nullptr));put(memory_controller.data(),kCtrlContext,static_cast<void*>(nullptr));break;
  case 1:unreadableContextR2(memory_child,kActContext);unreadableContextR2(memory_controller,kCtrlContext);break;
  case 2:unreadableContextR2(memory_child,kActContext);put(memory_controller.data(),kCtrlContext,static_cast<void*>(nullptr));break;
  case 3:put(memory_child.data(),kActContext,static_cast<void*>(nullptr));unreadableContextR2(memory_controller,kCtrlContext);break;
  case 4:unreadableContextR2(memory_controller,kCtrlContext);break;
  }
  auto& frame=kcd2vr::frik_pose_probe::frame;frame.tick=7500;frame.timeNs=7500000000ull;++frame.sequence;put(memory_child.data(),kActActiveTime,1.f);
  TickMortarStop69(memory_controller.data(),7500);
  ck(!cancels&&result69().status==mortar69::Status::Revoked&&result69().stopEntry72==mortar69::StopEntry72::NotEntered,
     "null or unreadable action/controller contexts cannot qualify a longer native wait");
 }
}
int main(int argc,char** argv){
 const bool contextOnly=argc>1&&(!std::strcmp(argv[1],"context")||!std::strcmp(argv[1],"context_reads"));
 if(!contextOnly){if(argc<2||std::strcmp(argv[1],"wait"))withdrawalCasesR2();waitCasesR2();}
 contextCasesR2(argc>1&&!std::strcmp(argv[1],"context_reads"));
 std::printf("D78R2 mortar native withdrawal: %u assertions PASS\n",checks);
}
