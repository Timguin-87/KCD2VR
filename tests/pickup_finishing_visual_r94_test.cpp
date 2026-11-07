// Exact ordinary attachment adapter during the post-cleanup/pre-adoption gap.
// Native memory and owner-reader leaves are controlled; lifecycle is checked by
// the separate actual action-owner fixture. No engine/DLL/headset execution.
#define main ExistingItemHandoverR94
#include "item_handover_d71_test.cpp"
#undef main
static std::array<unsigned char,0x100> finishItem{},finishLink{},finishPlayer{};
static int nFinishR94=0;
#define F94(x) do{++nFinishR94;if(!(x)){std::printf("FAIL FINISH R94 line=%d %s\n",__LINE__,#x);return 1;}}while(0)
static void SetupFinishR94(int side=0,bool gameplay=false){
 SetupD71(false);diagnostics::lab::reference=false;nativePassengerR94=false;finishItem={};finishLink={};finishPlayer={};
 auto b=graspBinding;b.manager=reinterpret_cast<std::uintptr_t>(manager.data());
 b.item=reinterpret_cast<std::uintptr_t>(finishItem.data());b.logic=0;b.side=side;
 b.attached=b.place=b.socketOnly28=false;b.gripReady55=true;b.grip55.p[0]=.1f;
 Put(finishItem,0,std::uintptr_t(0x3A2DA88));Put(finishItem,0x30,b.key);
 Put(finishItem,0x88,static_cast<void*>(finishLink.data()));Put(finishLink,0x30,b.entity);
 Put(manager,0x38,static_cast<void*>(finishPlayer.data()));Put(finishPlayer,0x30,b.actor);
 finishingHandKeysR94[side]=b.key;finishingReceiptR94={b,now,500,600,700,now};finishingAvailableR94=true;
 graspBinding={}; // regular semantic binding is deliberately unavailable
 if(side){frame.native[1]=frame.native[0];frame.allowed[1]=frame.allowed[0];frame.fistWristBoneH31[1]=0;
  frame.fistSlotBoneH31[1]=1;frame.originalToolSlotD71[1]=frame.originalToolSlotD71[0];frame.originalToolSlotValidD71[1]=true;}
 g_gameplay_allowed=gameplay;(void)ToolContext19c4r2();
}
int main(){
 for(int side=0;side<2;++side)for(bool gameplay:{false,true}){
  SetupFinishR94(side,gameplay);F94(ToolAttachment59c10(self.data(),attachment.data())&&writes==1);
  F94(std::abs(lastWanted[4]-12.1f)<1e-5);
  F94(nativePlacements==0&&socketProofs28==0&&readbacks==0);
  F94(g_native_wrist_retired107[side]==0&&g_native_use_until107[side]==0&&receipts.empty());
  F94(!graspBinding.attached&&!graspBinding.place&&finishingReceiptR94.binding.logic==0);
  F94(ToolAttachment59c10(self.data(),attachment.data())&&writes==2); // same fresh receipt, no frozen world pose
 }
 SetupFinishR94();finishingAvailableR94=false;F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();finishingHandKeysR94[0]=999;F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();Put(finishItem,0x30,std::uint64_t(999));F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();Put(finishLink,0x30,std::uint32_t(999));F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();Put(finishPlayer,0x30,std::uint32_t(999));F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();Put(finishItem,0,std::uintptr_t(123));F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();g_phys_relevant_id101[0]=42;F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();completedPublicationD71=4;F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();g_ctx_menu=true;F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();g_ctx_cutscene=true;F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();animation_filter::nativeBodyOwned112=true;F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();tracking=false;F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();now+=81;F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();onAttachmentWorld55=[](){finishingAvailableR94=false;};F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();onAttachmentWorld55=[](){finishingHandKeysR94[0]=999;};F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();onAttachmentWorld55=[](){Put(finishLink,0x30,std::uint32_t(999));};F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();onAttachmentWorld55=[](){QuatT28 q{};q.t[2]=.1f;Put(attachment,0x9C,q);};F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();onAttachmentWorld55=[](){Put(attachment,0x20,static_cast<void*>(nullptr));};F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();onAttachmentWorld55=[](){completedPublicationD71=4;};F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();onAttachmentWorld55=[](){finishingReceiptR94.binding.grip55.p[0]=.2f;};F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 // A new native/reference/fault owner wins even after initial admission.
 SetupFinishR94();onAttachmentWorld55=[](){g_pose_restore_fault6689=true;};F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();onAttachmentWorld55=[](){diagnostics::lab::reference=true;};F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();onAttachmentWorld55=[](){nativePassengerR94=true;};F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 // Adoption is compatible only for this same resource and canonical grip;
 // it still cannot turn this in-flight visual callback into a semantic receipt.
 SetupFinishR94();onAttachmentWorld55=[](){graspBinding=finishingReceiptR94.binding;graspBinding.attached=graspBinding.place=true;finishingAvailableR94=false;};
 F94(ToolAttachment59c10(self.data(),attachment.data())&&writes==1&&nativePlacements==0&&socketProofs28==0);
 SetupFinishR94();onAttachmentWorld55=[](){graspBinding=finishingReceiptR94.binding;graspBinding.attached=graspBinding.place=true;++graspBinding.token;finishingAvailableR94=false;};
 F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupFinishR94();onAttachmentWorld55=[](){graspBinding=finishingReceiptR94.binding;graspBinding.attached=graspBinding.place=true;graspBinding.grip55.p[0]=.2f;finishingAvailableR94=false;};
 F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 std::printf("R94 post-cleanup ordinary presentation: %d PASS\n",nFinishR94);return 0;
}
