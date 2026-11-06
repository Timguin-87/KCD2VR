// Exact ordinary attachment adapter across cleanup, adoption and policy lag.
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

// Review reproducer: same native item, same Grip, same completed pose; only
// Picking cleanup/adoption changes. No production source is modified.
int main(){
 for(int side=0;side<2;++side){
  SetupFinishR94(side,false);
  bool before=ToolAttachment59c10(self.data(),attachment.data());
  onAttachmentWorld55=[](){graspBinding=finishingReceiptR94.binding;graspBinding.attached=graspBinding.place=true;finishingAvailableR94=false;};
  bool adopting=ToolAttachment59c10(self.data(),attachment.data());
  onAttachmentWorld55=nullptr;
  bool next=ToolAttachment59c10(self.data(),attachment.data());
  const char* gate=ToolExperimentReason59c10(now);
  g_gameplay_allowed=true;
  bool resumed=ToolAttachment59c10(self.data(),attachment.data());
  std::printf("side=%d preAdoption=%d adoptionCallback=%d nextCallbackBeforeGameplayRefresh=%d gate=%s afterGameplayRefresh=%d writes=%d\n",side,before,adopting,next,gate?gate:"NONE",resumed,writes);
  F94(before&&adopting&&next&&resumed&&writes==4);
 }
 // Every ordinary writer refusal remains effective while the adopted receipt
 // provides the narrow policy-lag exception. No unrelated resource can borrow it.
 for(int side=0;side<2;++side)for(int fault=0;fault<23;++fault){
  SetupFinishR94(side,false);
  graspBinding=finishingReceiptR94.binding;graspBinding.attached=graspBinding.place=true;finishingAvailableR94=false;
  F94(ToolAttachment59c10(self.data(),attachment.data())&&writes==1);
  switch(fault){
   case 0:++graspBinding.entity;break;
   case 1:++graspBinding.token;break;
   case 2:graspBinding.grip55.p[0]+=.01f;break;
   case 3:graspBinding.place=false;break;
   case 4:graspBinding.attached=false;break;
   case 5:++finishingHandKeysR94[side];break;
   case 6:Put(finishItem,0x30,finishingReceiptR94.binding.key+1);break;
   case 7:Put(finishLink,0x30,finishingReceiptR94.binding.entity+1);break;
   case 8:Put(finishPlayer,0x30,finishingReceiptR94.binding.actor+1);break;
   case 9:tracking=false;break;
   case 10:frame.stamp.tick=now-81;break;
   case 11:frame.stamp.epoch++;break;
   case 12:acceptVisualPacket55=false;break;
   case 13:g_ctx_menu=true;break;
   case 14:g_ctx_dialogue=true;break;
   case 15:g_ctx_cutscene=true;break;
   case 16:g_ctx_reading=true;break;
   case 17:g_pose_restore_fault6689=true;break;
   case 18:animation_filter::nativeBodyOwned112=true;break;
   case 19:onAttachmentWorld55=[](){++graspBinding.token;};break;
   case 20:onAttachmentWorld55=[](){g_ctx_menu=true;};break;
   case 21:kcd2vr::static_grab_r89::heldEntityR94[side]=42;break;
   case 22:onAttachmentWorld55=[](){kcd2vr::static_grab_r89::heldEntityR94[0]=42;};break;
  }
  F94(!ToolAttachment59c10(self.data(),attachment.data())&&writes==1);
  onAttachmentWorld55=nullptr;kcd2vr::static_grab_r89::heldEntityR94[0]=kcd2vr::static_grab_r89::heldEntityR94[1]=0;
 }
 std::printf("PASS R95: adopted policy-lag adapter: %d assertions, both hands and 23 refusal conditions.\n",nFinishR94);
 return 0;
}
