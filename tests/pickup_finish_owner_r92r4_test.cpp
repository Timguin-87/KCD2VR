// Real production owner + native End/Cleanup callback doubles. No DLL or pixels.
#define KCD2VR_RESOURCE_D60_NO_MAIN
#include "alchemy_resource_receipt_d60_test.cpp"
static unsigned checksFinishR94=0;
#define F94(x) do{++checksFinishR94;if(!(x)){std::printf("FAIL R94 %d: %s\n",__LINE__,#x);return 1;}}while(0)
static void BeginFinishR94(int side=0){
 using namespace kcd2vr::action_time_owner59c11;
 g_finishing_visual_token_r94=0;reset();testAttachManager=manager.data();
 if(side){
  ng13::AcquisitionInput55 raw{};g_acquisition_input55.read(raw);raw.tracked[side]=true;raw.grip[side]=1;PublishAcquisitionInput55(raw);
  ng13::HandFrame hands{};g_hands13.read(hands);hands.tracked[side]=true;hands.grip[side]=1;PublishHands(hands);
  g_owner.side=side;g_owner.useInput55=raw;put(logic.data(),0x30,side);
 }
 attach();
 // Keep the actual native outer End continuation alive after Cleanup returned.
 outerBlocked19c4=true;finishUnjoined19c4=true;
 if(side){hostTickCount64=1020;ng13::AcquisitionInput55 raw{};g_acquisition_input55.read(raw);raw.tick=1020;raw.sample=1020;PublishAcquisitionInput55(raw);
  ng13::HandFrame hands{};g_hands13.read(hands);hands.tick=1020;hands.sample=1020;PublishHands(hands);}
 else frame(1020);
 TickOwner(controller.data(),1020);outerEntered19c4.wait(false);
}
static void CompleteNativeR94(){using namespace kcd2vr::action_time_owner59c11;outerRelease19c4.store(true);unjoinedCleanup19c4.join();}
int main(){
 using namespace kcd2vr::action_time_owner59c11;ng13::FinishingPresentationR94 visual{};
 reset();F94(!ReadFinishingAttachmentPresentationR94(visual));
 for(int side=0;side<2;++side){
  BeginFinishR94(side);const auto token=g_pick13.bind.token;
  F94(finishes==1&&cleanup==1&&attaches==1&&transfers==0&&drops==0);
  F94(g_finishing_visual_token_r94==token&&!g_pick13.cleanupSeen);
  F94(g_cleanup_original19c4==token&&g_cleanup_done19c4!=token);
  F94(!ReadFinishingAttachmentPresentationR94(visual)&&!HoldPublication13(token));
  CompleteNativeR94();
  F94(g_cleanup_done19c4==token&&g_outer_callbacks19c4==(token<<16));
  F94(ReadFinishingAttachmentPresentationR94(visual)&&visual.binding.side==side&&visual.requestedAt==1020);
  F94(!visual.binding.attached&&!visual.binding.place&&!visual.binding.socketOnly28&&!visual.binding.logic);
  F94(visual.binding.gripReady55&&visual.binding.grip55.p[0]==.07f&&visual.binding.acceptedAttachTickD72==0);
  F94(visual.binding.acceptedAcquisitionD72.action==0&&visual.binding.fingers55.count==0);
  F94(!HoldPublication13(token)&&!g_pick13.cleanupSeen&&g_free_hold19c4r2!=token);
  ng13::Binding semantic{};F94(!ReadGraspBinding(semantic));
  const auto first=visual;
  hostTickCount64=1030;ng13::AcquisitionInput55 current{};g_acquisition_input55.read(current);current.tick=1030;current.sample=1030;PublishAcquisitionInput55(current);
  F94(ReadFinishingAttachmentPresentationR94(visual)&&visual.requestedAt==first.requestedAt&&visual.sourceTick==1030);
  F94(ng13::sameFinishingPresentationR94(first,visual));
  // Old logic/action contents are not read; allocator reuse cannot be authority.
  logic.fill(0xdd);action.fill(0xdd);forbidRetiredRead19c4=true;
  F94(ReadFinishingAttachmentPresentationR94(visual)&&retiredReads19c4==0);
  F94(AdoptNativeCleanup19c4()&&g_pick13.cleanupSeen&&g_free_hold19c4r2==token);
  F94(FinishingOrHeldPresentationCurrentR94(first));
  F94(!ReadFinishingAttachmentPresentationR94(visual)&&ReadGraspBinding(semantic)&&semantic.place);
  F94(ReadAdoptedAttachmentPresentationR95(visual)&&visual.requestedAt==first.requestedAt);
  // A genuine held item can outlive the brief pre-adoption bridge. Only fresh
  // raw grip input keeps this presentation valid; the cleanup time is immutable.
  hostTickCount64=1150;current.tick=1150;current.sample=1150;PublishAcquisitionInput55(current);
  F94(ReadAdoptedAttachmentPresentationR95(visual)&&visual.requestedAt==1020&&visual.sourceTick==1150);
  F94(FinishingOrHeldPresentationCurrentR94(first)&&retiredReads19c4==0);
  hostTickCount64=1231;F94(!ReadAdoptedAttachmentPresentationR95(visual));
  hostTickCount64=1150;F94(ReadAdoptedAttachmentPresentationR95(visual));
  auto changed=first;changed.binding.grip55.p[0]+=.01f;F94(!FinishingOrHeldPresentationCurrentR94(changed));
  F94(attaches==1&&finishes==1&&cleanup==1&&transfers==0&&drops==0&&retiredReads19c4==0);
  g_inside_terminal13=true;Terminal13(ng13::Terminal::Drop,true,"adopted_release");
  F94(!FinishingOrHeldPresentationCurrentR94(first));
 }
 // The adopted-reader exception never transfers to a changed resource or
 // stale/cancelled input, even after its original 80-ms cleanup window.
 for(int failure=0;failure<18;++failure){
  BeginFinishR94();CompleteNativeR94();F94(AdoptNativeCleanup19c4());
  hostTickCount64=1150;ng13::AcquisitionInput55 input{};g_acquisition_input55.read(input);
  input.tick=1150;input.sample=1150;PublishAcquisitionInput55(input);
  F94(ReadAdoptedAttachmentPresentationR95(visual));
  if(failure==0)g_context13=false;
  if(failure==1)g_native_fault13=true;
  if(failure==2)g_foreign_cancel13=true;
  if(failure==3)++g_use_generation55;
  if(failure==4)RetireUse55(g_owner.useSerial54);
  if(failure==5){auto b=g_pick13.bind;++b.entity;g_binding13.publish(b);}
  if(failure==6){auto b=g_pick13.bind;b.grip55.p[0]+=.01f;g_binding13.publish(b);}
  if(failure==7){auto b=g_pick13.bind;b.attached=false;g_binding13.publish(b);}
  if(failure==8){auto b=g_pick13.bind;b.place=false;g_binding13.publish(b);}
  if(failure==9)g_free_hold19c4r2=0;
  if(failure==10)g_cleanup_done19c4=0;
  if(failure==11)hostTickCount64=1231;
  if(failure>=12){
   if(failure==12)input.grip[0]=0;
   if(failure==13)input.headTracked=false;
   if(failure==14)input.tracked[0]=false;
   if(failure==15)++input.trackingEpoch;
   if(failure==16)++input.character;
   if(failure==17)++input.epoch;
   PublishAcquisitionInput55(input);
  }
  F94(!ReadAdoptedAttachmentPresentationR95(visual));
  F94(attaches==1&&finishes==1&&cleanup==1&&transfers==0&&drops==0);
 }
 // Every condition is checked after genuine native outer completion.
 for(int failure=0;failure<15;++failure){
  BeginFinishR94();CompleteNativeR94();F94(ReadFinishingAttachmentPresentationR94(visual));
  if(failure==0)g_context13=false;
  if(failure==1)g_native_fault13=true;
  if(failure==2)g_foreign_cancel13=true;
  if(failure==3)++g_use_generation55;
  if(failure==4)RetireUse55(g_owner.useSerial54);
  if(failure==5){auto b=g_pick13.bind;++b.entity;g_binding13.publish(b);}
  if(failure==6){auto b=g_pick13.bind;b.grip55.p[0]+=.01f;g_binding13.publish(b);}
  if(failure==7){auto t=g_pick13.bind.token;g_outer_callbacks19c4=(t<<16)|1;}
  if(failure==8)g_outer_unobserved19c4=g_pick13.bind.token;
  if(failure==9)g_cleanup_done19c4=0;
  if(failure==10){frame(1101);}
  if(failure>=11){ng13::AcquisitionInput55 input{};g_acquisition_input55.read(input);
   if(failure==11)input.grip[0]=0;
   if(failure==12)input.headTracked=false;
   if(failure==13)input.tracked[0]=false;
   if(failure==14)++input.trackingEpoch;
   PublishAcquisitionInput55(input);
  }
  F94(!ReadFinishingAttachmentPresentationR94(visual));F94(attaches==1&&finishes==1&&cleanup==1&&transfers==0&&drops==0);
 }
 // A terminal request revokes even when its existing early-return guard fires.
 BeginFinishR94();CompleteNativeR94();g_inside_terminal13=true;
 Terminal13(ng13::Terminal::Drop,true,"fixture_release");F94(!ReadFinishingAttachmentPresentationR94(visual)&&g_finishing_visual_token_r94==0);
 BeginFinishR94();CompleteNativeR94();GraspFault13("fixture_fault");F94(!ReadFinishingAttachmentPresentationR94(visual)&&g_finishing_visual_token_r94==0);
 BeginFinishR94();CompleteNativeR94();ResetOwner();F94(!ReadFinishingAttachmentPresentationR94(visual)&&g_finishing_visual_token_r94==0);
 // Ordinary terminal Finish never publishes a keep-holding visual bridge.
 g_finishing_visual_token_r94=0;reset();attach();finishNoCleanup19c4=true;
 Terminal13(ng13::Terminal::Stow,false,"fixture_stow");PickupEndDetour19c4(logic.data(),0,0);
 F94(!ReadFinishingAttachmentPresentationR94(visual)&&g_finishing_visual_token_r94==0);
 std::printf("R94 native completed-cleanup visual owner: %u PASS\n",checksFinishR94);return 0;
}
