// Executes the actual attachment callback adapter. Native memory/transform
// leaves are explicit doubles; this is scheduling-order proof, not pixel proof.
#define main ExistingHandoverMainD72
#include "item_handover_d71_test.cpp"
#undef main
static int nD72=0;
#define C72(x) do{++nD72;if(!(x)){std::printf("FAIL %d %s\n",__LINE__,#x);return 1;}}while(0)
static void PendingD72(bool gameplay=true,int side=0,bool legacyTrial=true){
 SetupD71(legacyTrial);graspBinding.attached=graspBinding.place=graspBinding.socketOnly28=false;
 graspBinding.acceptedAttachTickD72=now;graspBinding.side=side;
 acquisition55={};auto& a=acquisition55;a.character=graspBinding.character;a.epoch=graspBinding.epoch;a.side=side;
 a.action=101;a.scope=102;a.serial=103;a.startedAt=900;a.tick=now;a.active=true;
 a.graspToken=graspBinding.token;a.graspKey=graspBinding.key;a.graspItem=graspBinding.item;a.graspEntity=graspBinding.entity;
 graspBinding.acceptedAcquisitionD72=a;acquisitionAvailable55=true;
 if(side){frame.native[1]=frame.native[0];frame.allowed[1]=frame.allowed[0];frame.fistWristBoneH31[1]=0;
  frame.fistSlotBoneH31[1]=1;frame.originalToolSlotD71[1]=frame.originalToolSlotD71[0];frame.originalToolSlotValidD71[1]=true;}
 g_gameplay_allowed=gameplay;(void)ToolContext19c4r2();
}
int main(){
 for(int side=0;side<2;++side)for(bool gameplay:{false,true})for(bool legacyTrial:{false,true}){
  PendingD72(gameplay,side,legacyTrial);kcd2vr::grasp13::Binding b{};
  C72(!ReadGrasp(b)); // still no HELD or socket-only publication from owner
  C72(ToolAttachment59c10(self.data(),attachment.data())&&writes==1);
  C72(std::abs(lastWanted[4]-12.1f)<1e-5); // current controller, original native slot offset
  C72(!graspBinding.attached&&!graspBinding.place&&!graspBinding.socketOnly28);
  C72(nativePlacements==0&&socketProofs28==0&&readbacks==0);
  C72(g_native_wrist_retired107[side]==0&&g_native_use_until107[side]==0);
  C72(receipts.empty());
 }
 PendingD72();graspBinding.acceptedAttachTickD72=0;C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();kcd2vr::contact_handover_d71::SetItemHandover(false);C72(ToolAttachment59c10(self.data(),attachment.data())&&writes==1);
 PendingD72();now+=81;C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();++acquisition55.graspKey;C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();++acquisition55.graspEntity;C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();acquisitionAvailable55=false;C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();acquisition55.active=false;C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();Put(attachment,0x20,static_cast<void*>(nullptr));C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();Put(attachment,0x78,std::int16_t(2));C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();frame.originalToolSlotValidD71[0]=false;C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();completedPublicationD71=4;C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();tracking=false;C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();g_phys_relevant_id101[0]=42;C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();onAttachmentWorld55=[](){graspBinding.acceptedAttachTickD72=0;};C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();onAttachmentWorld55=[](){++graspBinding.token;};C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();onAttachmentWorld55=[](){acquisitionAvailable55=false;};C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();onAttachmentWorld55=[](){completedPublicationD71=4;};C72(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 PendingD72();onAttachmentWorld55=[](){graspBinding.socketOnly28=true;graspBinding.acceptedAttachTickD72=0;};
 C72(ToolAttachment59c10(self.data(),attachment.data())&&writes==1&&nativePlacements==0); // native owner advances, no borrowed HELD
 std::printf("D72 accepted attachment first callback: %d PASS\n",nD72);return 0;
}
