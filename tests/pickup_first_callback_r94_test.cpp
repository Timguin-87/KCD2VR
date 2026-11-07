// Production ordinary attachment callback, immutable original-pose payload,
// and explicit engine leaves. This proves source behavior, not displayed pixels.
#define main ExistingHandoverMainR94
#include "item_handover_d71_test.cpp"
#undef main
static int checksR94=0;
#define R94(x) do{++checksR94;if(!(x)){std::printf("FAIL R94 line=%d %s\n",__LINE__,#x);return 1;}}while(0)
static void CopyOriginalFingerCarryR94(CommittedFrame6695& packet,
    const kcd2vr::grasp13::FingerReference55 (&graspFingers55)[2]) {
#include "pickup_original_finger_carry_r94.inc"
}
static void SideR94(int side){
 graspBinding.side=side;
 if(side){frame.native[1]=frame.native[0];frame.allowed[1]=frame.allowed[0];
  frame.fistWristBoneH31[1]=0;frame.fistSlotBoneH31[1]=1;
  frame.originalToolSlotD71[1]=frame.originalToolSlotD71[0];frame.originalToolSlotValidD71[1]=true;}
}
static void AcceptedR94(int side,bool gameplay){
 SetupD71(false);SideR94(side);graspBinding.attached=graspBinding.place=false;
 graspBinding.acceptedAttachTickD72=now;auto& a=acquisition55;
 a.character=graspBinding.character;a.epoch=graspBinding.epoch;a.side=side;
 a.action=101;a.scope=102;a.serial=103;a.startedAt=900;a.tick=now;a.active=true;
 a.graspToken=graspBinding.token;a.graspKey=graspBinding.key;a.graspItem=graspBinding.item;a.graspEntity=graspBinding.entity;
 graspBinding.acceptedAcquisitionD72=a;acquisitionAvailable55=true;
 g_gameplay_allowed=gameplay;(void)ToolContext19c4r2();
}
int main(){
 using namespace kcd2vr::contact_handover_d71;
 for(int side=0;side<2;++side)for(bool recorder:{false,true}){
  SetupD71(false);SideR94(side);
  diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,recorder,now);
  R94(!ItemHandover()&&frame.itemHandoverRevisionD71==0&&!frame.tools59c10[side].valid);
  R94(ToolAttachment59c10(self.data(),attachment.data())&&writes==1);
  R94(std::abs(lastWanted[4]-12.1f)<1e-5&&nativePlacements==1&&socketProofs28==0);
  // No new body or control callback is required for the next placement.
  R94(ToolAttachment59c10(self.data(),attachment.data())&&writes==2);
  R94(std::abs(lastWanted[4]-12.1f)<1e-5&&nativePlacements==2);
  if(!recorder)R94(receipts.empty());
 }
 for(int side=0;side<2;++side)for(bool gameplay:{false,true}){
  AcceptedR94(side,gameplay);
  R94(ToolAttachment59c10(self.data(),attachment.data())&&writes==1);
  R94(std::abs(lastWanted[4]-12.1f)<1e-5&&!graspBinding.attached&&!graspBinding.place);
  R94(nativePlacements==0&&socketProofs28==0&&readbacks==0&&g_native_use_until107[side]==0);
  R94(ToolAttachment59c10(self.data(),attachment.data())&&writes==2&&nativePlacements==0);
  // Native acknowledgement alone cannot survive a replaced action/request.
  ++acquisition55.graspKey;
  R94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==2);
 }
 // Original fingers are carried with their transaction even before a mailbox socket.
 SetupD71(false);PairedFingersD71();
 kcd2vr::grasp13::FingerReference55 original[2]{};original[0]=frame.tools59c10[0].fingers55;
 frame.tools59c10[0]={};CopyOriginalFingerCarryR94(frame,original);
 R94(frame.itemHandoverRevisionD71==0&&frame.tools59c10[0].fingers55.sample==frame.stamp.sample);
 R94(ToolAttachment59c10(self.data(),attachment.data())&&writes==1&&socketProofs28==1);
 R94(socketProof28.gripReady55&&socketProof28.fingers55.count==2);
 R94(std::abs(socketProof28.grip55.p[0]-.1f)<1e-5);
 original[0]={};CopyOriginalFingerCarryR94(frame,original);
 R94(frame.tools59c10[0].fingers55.count==0&&frame.tools59c10[1].fingers55.count==0);
 // Production exception must not promote an unrelated legacy item or native mode.
 SetupD71(false);g_graspBinding13=nullptr;held[0]=42;
 kcd2vr::combat_rules178r10::f5Rung=2;kcd2vr::combat_rules178r10::workstationEnabled=true;
 kcd2vr::workstation178r20c1::setExperiment36(true);
 kcd2vr::workstation178r20c1::heldToolActivity59c10=1;kcd2vr::workstation178r20c1::heldToolActivityTick59c10=now;
 R94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 ToolSocket59c10 legacy{};R94(g_tool_socket59c10[0].read(legacy)&&!legacy.nativeGrasp13);
 QuatT28 grip{};R94(!OriginalToolGripD71(frame,legacy,grip));
 SetItemHandover(true);frame.itemHandoverRevisionD71=ItemRevision();
 R94(OriginalToolGripD71(frame,legacy,grip));
 SetItemHandover(false);SetItemHandover(true);R94(!OriginalToolGripD71(frame,legacy,grip));
 // Exact current publication/identity/tracking remain mandatory with trial OFF.
 SetupD71(false);completedPublicationD71=4;R94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71(false);onAttachmentWorld55=[](){completedPublicationD71=4;};R94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71(false);onAttachmentWorld55=[](){++graspBinding.token;};R94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71(false);onAttachmentWorld55=[](){g_phys_relevant_id101[0]=42;};R94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71(false);tracking=false;R94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 AcceptedR94(0,false);g_ctx_menu=true;R94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 AcceptedR94(0,false);g_ctx_cutscene=true;R94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 AcceptedR94(0,false);graspBinding.acceptedAttachTickD72=0;R94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 AcceptedR94(0,false);now+=81;R94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 std::printf("R94 production first-callback pickup: %d PASS\n",checksR94);return 0;
}
