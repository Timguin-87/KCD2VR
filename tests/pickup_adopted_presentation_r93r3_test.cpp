// Actual ordinary attachment adapter. The native leaf completes Picking and
// republishes the same held resource during the current placement callback.
#define main ExistingHandoverMainR93R3
#include "item_handover_d71_test.cpp"
#undef main
static unsigned checkedR93R3=0;
#define P93(x) do{++checkedR93R3;if(!(x)){std::printf("FAIL R93R3 line=%d %s\n",__LINE__,#x);return 1;}}while(0)
static void HeldR93R3(int side=0){
 SetupD71(false);graspBinding.gripReady55=true;graspBinding.grip55.p[0]=.1f;graspBinding.side=side;
 if(side){frame.native[1]=frame.native[0];frame.allowed[1]=frame.allowed[0];
  frame.fistWristBoneH31[1]=0;frame.fistSlotBoneH31[1]=1;
  frame.originalToolSlotD71[1]=frame.originalToolSlotD71[0];frame.originalToolSlotValidD71[1]=true;}
}
int main(){
 for(int side=0;side<2;++side){
  HeldR93R3(side);onAttachmentWorld55=[](){graspBinding.logic=0;};
  P93(ToolAttachment59c10(self.data(),attachment.data())&&writes==1);
  P93(std::abs(lastWanted[4]-12.1f)<1e-5&&graspBinding.attached&&graspBinding.place);
  P93(nativePlacements==1&&receivedPlacement.binding.logic==0&&socketProofs28==0);
  P93(ToolAttachment59c10(self.data(),attachment.data())&&writes==2);
 }
 // This is only retirement of the old unique Picking logic, not replacement.
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=14;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();graspBinding.logic=0;onAttachmentWorld55=[](){graspBinding.logic=13;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();graspBinding.gripReady55=false;onAttachmentWorld55=[](){graspBinding.logic=0;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;graspBinding.grip55.p[0]=.2f;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;graspBinding.gripReady55=false;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;graspBinding.place=false;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;graspBinding.attached=false;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;++graspBinding.token;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;++graspBinding.key;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;++graspBinding.item;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;++graspBinding.manager;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;++graspBinding.entity;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;++graspBinding.actor;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;++graspBinding.character;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;++graspBinding.epoch;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;graspBinding.side=1;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;g_phys_relevant_id101[0]=42;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 HeldR93R3();onAttachmentWorld55=[](){graspBinding.logic=0;nativePassengerR94=true;};P93(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);nativePassengerR94=false;
 std::printf("R93R3 same held resource after Picking retirement: %u PASS\n",checkedR93R3);return 0;
}
