// Whole production action-owner TU. Native memory/calls are existing doubles.
// Inspect before any controller adoption: that is the missing handover interval.
#define KCD2VR_RESOURCE_D60_NO_MAIN
#include "alchemy_resource_receipt_d60_test.cpp"
static unsigned checksR92=0;
#define R92(x) do{++checksR92;if(!(x)){std::printf("FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(){
 using namespace kcd2vr::action_time_owner59c11;
 const auto begin=[](int side,bool armInJob){
  reset();testAttachManager=manager.data();
  ng13::AcquisitionInput55 raw{};g_acquisition_input55.read(raw);
  raw.tracked[side]=true;raw.grip[side]=1;PublishAcquisitionInput55(raw);
  ng13::HandFrame hands{};g_hands13.read(hands);hands.tracked[side]=true;hands.grip[side]=1;PublishHands(hands);
  if(side){g_owner.side=side;g_owner.useInput55=raw;put(logic.data(),0x30,side);}
  if(armInJob){
   g_owner={};g_consumed_use13=0;g_acquisition54.publish({});g_acquisition_foundation_d56.publish({});
   NoteUseSent(side,1000);
  }
  std::thread job([]{pickupThread19c4=19728;attach(false);});job.join();
 };
 for(int side=0;side<2;++side)for(bool armInJob:{false,true}){
  begin(side,armInJob);ng13::Binding binding{};ng13::Acquisition54 basis{},acquisition{};
  R92(attaches==1&&notices==1&&transfers==0&&finishes==0);
  R92(g_owner.state==PendingAttach&&g_pick13.controllerPausePending19c4&&!g_pick13.attached);
  R92(!ReadGraspBinding(binding)&&ReadPendingAttachmentPresentationD72(binding));
  R92(ReadAcquisitionFoundationD56(basis));
  R92(ReadAcquisition54(acquisition)&&acquisition.active&&basis.active);
  R92(basis.side==side&&acquisition.side==side&&ng13::sameAcquisition55(basis,binding.acceptedAcquisitionD72));
  R92(basis.graspToken==0&&basis.graspKey==0&&basis.graspItem==0&&basis.graspEntity==0);
  R92(ng13::acquisitionOwns55(binding,acquisition)&&basis.tick==binding.acceptedAttachTickD72);
  R92(Rd<float>(action.data(),kActSpeedBias,-1)==1.f); // job gained no action-clock authority
  R92(!binding.attached&&!binding.place&&!binding.socketOnly28); // no semantic HELD promotion
  const auto originalTick=basis.tick;hostTickCount64+=30;
  R92(ReadAcquisitionFoundationD56(basis)&&basis.tick==originalTick); // no timestamp renewal
  hostTickCount64+=51;R92(!ReadAcquisitionFoundationD56(basis)&&!ReadPendingAttachmentPresentationD72(binding));
 }
 begin(0,false);g_context13=false;ng13::Acquisition54 a{};ng13::Binding b{};
 R92(!ReadAcquisition54(a)&&!ReadAcquisitionFoundationD56(a)&&!ReadPendingAttachmentPresentationD72(b));
 begin(0,false);++g_use_generation55;
 R92(!ReadAcquisition54(a)&&!ReadAcquisitionFoundationD56(a)&&!ReadPendingAttachmentPresentationD72(b));
 begin(0,false);RetireUse55(g_owner.useSerial54);
 R92(!ReadAcquisition54(a)&&!ReadAcquisitionFoundationD56(a)&&!ReadPendingAttachmentPresentationD72(b));
 begin(0,false);frame(1001,0);
 R92(!ReadAcquisition54(a)&&!ReadPendingAttachmentPresentationD72(b));
 begin(0,false);g_pick13.pendingTerminal=ng13::Terminal::Drop;PublishBinding13();
 {std::thread job([]{pickupThread19c4=19728;Publish();});job.join();}
 R92(!ReadAcquisition54(a)&&!ReadAcquisitionFoundationD56(a)&&!ReadPendingAttachmentPresentationD72(b));
 begin(0,false);g_cleanup_claim19c4=g_pick13.bind.token;
 {std::thread job([]{pickupThread19c4=19728;Publish();});job.join();}
 R92(!ReadAcquisition54(a)&&!ReadAcquisitionFoundationD56(a)&&!ReadPendingAttachmentPresentationD72(b));
 // An unrelated publication has no exact Attach callback privilege.
 begin(0,false);{std::thread job([]{pickupThread19c4=19728;Publish();});job.join();}
 R92(!ReadAcquisition54(a)&&!ReadAcquisitionFoundationD56(a));
 // Existing same-item controller adoption and native finish remain once-only.
 begin(0,false);frame(1020);UpdateDetour(controller.data(),.02f);
 R92(g_owner.state==Held&&ReadGraspBinding(b)&&attaches==1&&notices==1&&transfers==0);
 poseProof55();frame(1040);UpdateDetour(controller.data(),.02f);
 R92(finishes==1&&g_pick13.cleanupSeen&&g_pick13.bind.gripReady55&&transfers==0);
 std::printf("R92 accepted native pickup presentation handover: %u PASS\n",checksR92);return 0;
}
