// Exact production attachment adapter, with the existing explicit engine leaves.
// No engine, display-frame, real-thread scheduling or headset emulation claim.
#define KCD2VR_TOOL_ATTACHMENT_NO_MAIN
#include "tool_attachment59c10_test.cpp"
#include <limits>
static int checksD71=0;
#define CHECK71(x) do{++checksD71;if(!(x)){std::printf("FAIL line=%d %s\n",__LINE__,#x);return 1;}}while(0)
static void SetupD71(bool on=true,bool pending=false){
 using namespace kcd2vr::contact_handover_d71;
 self={};attachment={};manager={};character={};entity={};frame=CommittedFrame6695{};graspBinding={};
 g_tool_socket59c10[0].publish({});g_tool_socket59c10[1].publish({});
 now=1000;frame.stamp={7,100,now,reinterpret_cast<std::uintptr_t>(character.data())};
 frame.valid[0]=frame.valid[1]=true;frame.native[0].t[0]=1;frame.allowed[0].t[0]=2;
 frame.fistJointCountH31=5;frame.fistWristBoneH31[0]=0;frame.fistSlotBoneH31[0]=1;
 frame.originalToolSlotD71[0].t[0]=1.1f;frame.originalToolSlotValidD71[0]=true;
 frame.publicationR24=completedPublicationD71=2;SetItemHandover(on);
 frame.itemHandoverRevisionD71=on?ItemRevision():0;
 Put(self,0,std::uintptr_t(0x3A68F68));Put(self,8,std::uint32_t(42));
 Put(attachment,0,std::uintptr_t(0x3A657C0));Put(attachment,0x20,static_cast<void*>(self.data()));
 Put(attachment,0x28,static_cast<void*>(manager.data()));Put(manager,0x70,static_cast<void*>(character.data()));
 Put(character,0,std::uintptr_t(0x40602F8));Put(attachment,0x78,std::int16_t(1));
 Put(attachment,0x7A,std::uint8_t(1));Put(attachment,0x9C,QuatT28{});
 graspBinding={frame.stamp.actor,11,12,13,700,42,7,42,99,0,!pending,!pending};graspBinding.socketOnly28=pending;
 g_graspBinding13=&ReadGrasp;g_graspPlaced13=&ReceivePlacement;g_graspSocket28=&ReceiveSocket28;
 socketProof28={};socketProofs28=nativePlacements=0;writes=readbacks=0;receipts.clear();
 g_gameplay_allowed=true;g_dialogue_body178r9=false;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_reading=false;
 g_pose_restore_fault6689=false;g_pose_epoch6689=7;animation_filter::nativeBodyOwned112=false;animation_filter::kick175=false;
 g_phys_relevant_id101[0]=g_phys_relevant_id101[1]=0;g_left_grip=g_right_grip=1;
 for(int side=0;side<2;++side){g_native_use_until107[side]=0;g_native_wrist_retired107[side]=0;g_native_use_serial107[side]=17;}
 tracking=placement=identity=true;onAttachmentWorld55=nullptr;placementNotify107=nullptr;acceptVisualPacket55=true;
 completedPublicationD71=2;worldScale35=1;attachmentRootD47={10,20,30};committedReadOverrideD47=nullptr;
 alchemyActive=smithActive37=false;held[0]=held[1]=0;g_henry_char_attachment_manager=nullptr;
 kcd2vr::workstation178r20c1::setExperiment36(false);kcd2vr::workstation178r20c1::smithAutoHold36=false;
 kcd2vr::combat_rules178r10::f5Rung=0;kcd2vr::workstation178r20c1::heldToolActivity59c10=0;
 kcd2vr::activity_input163::witness175.ready=true;
 for(auto& v:kcd2vr::activity_input163::witness175.active)v=false;
 finishingAvailableR94=false;finishingReceiptR94={};finishingHandKeysR94[0]=finishingHandKeysR94[1]=0;
 acquisitionAvailable55=false;acquisition55={};(void)ToolContext19c4r2();
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Interaction,false,now);
}
static void PairedFingersD71(){
 auto& ref=frame.tools59c10[0].fingers55;ref.sample=frame.stamp.sample;ref.tick=frame.stamp.tick;ref.count=2;
 ref.mapping.skeleton=0x1234;ref.mapping.generation=1;ref.mapping.joints=5;ref.mapping.wrist=0;ref.mapping.count=2;
 ref.mapping.ids[0]=3;ref.mapping.ids[1]=4;ref.mapping.parents[0]=0;ref.mapping.parents[1]=3;
 ref.rotations[0]={0,0,0,1};ref.rotations[1]={0,0,0,1};
 auto& a=ref.acquisition;a.character=graspBinding.character;a.epoch=graspBinding.epoch;a.side=graspBinding.side;
 a.action=101;a.scope=102;a.serial=103;a.startedAt=900;a.tick=now;a.active=true;
 a.graspToken=graspBinding.token;a.graspKey=graspBinding.key;a.graspItem=graspBinding.item;a.graspEntity=graspBinding.entity;
}
static std::array<unsigned char,0x400> smithModelD71{};
static std::array<unsigned char,0x80> smithBlankD71{};
static void SetupSmithD71(){
 SetupD71();g_graspBinding13=nullptr;g_graspPlaced13=nullptr;
 namespace ai=kcd2vr::activity_input163;namespace ws=kcd2vr::workstation178r20c1;
 ai::witness175.Begin(ai::Kind175::Smithing,999);
 ws::heldToolActivity59c10=int(ws::Kind::Anvil);ws::heldToolActivityTick59c10=now;
 ws::setSmithAutoHold36(true);ws::setExperiment36(true);kcd2vr::combat_rules178r10::f5Rung=2;
 kcd2vr::combat_rules178r10::workstationEnabled=true;
 (void)ToolContext19c4r2(true);g_gameplay_allowed=false;g_dialogue_body178r9=true;
 g_henry_char_attachment_manager=manager.data();g_right_grip=0;
 smithBinding37={};smithBinding37.instance=999;smithBinding37.player=888;smithBinding37.character=frame.stamp.actor;
 smithBinding37.attachment=reinterpret_cast<std::uintptr_t>(attachment.data());
 smithBinding37.object=reinterpret_cast<std::uintptr_t>(self.data());smithBinding37.holder=777;
 smithBinding37.entity=42;smithBinding37.side=0;smithBinding37.tool=0;smithBinding37.phase=11;smithBinding37.lifetime=88;
 smithActive37=true;g_phys_relevant_id101[0]=42;
}
static void SetupAlchemyD71(){
 SetupD71();g_graspBinding13=nullptr;g_graspPlaced13=nullptr;
 namespace ai=kcd2vr::activity_input163;namespace ws=kcd2vr::workstation178r20c1;
 kcd2vr::combat_rules178r10::f5Rung=2;kcd2vr::combat_rules178r10::workstationEnabled=true;ws::setExperiment36(true);
 ws::heldToolActivity59c10=1;ws::heldToolActivityTick59c10=now;alchemyActive=true;
 alchemy={};alchemyPlayer={};alchemyAction={};alchemyChild19={};alchemySlots={};
 Put(alchemy,0,std::uintptr_t(0x3F60B08));Put(alchemy,0x18,reinterpret_cast<std::uintptr_t>(alchemyPlayer.data()));
 Put(alchemy,0x2C,std::uint8_t(1));Put(alchemyPlayer,0,std::uintptr_t(0x3AA4B98));
 Put(alchemy,0x78,std::uintptr_t(0x3BFA5B0));alchemySlots[1]=reinterpret_cast<std::uintptr_t>(alchemyAction.data());
 Put(alchemy,0xB0,reinterpret_cast<std::uintptr_t>(alchemySlots.data()));Put(alchemy,0xB8,reinterpret_cast<std::uintptr_t>(alchemySlots.data()+2));
 Put(alchemyAction,0,std::uintptr_t(0x3A5C670));Put(alchemyAction,0x28,std::uint8_t(1));Put(alchemyAction,0x3C,int(1));
 Put(alchemyAction,0x40,reinterpret_cast<std::uintptr_t>(alchemy.data()+0x78));Put(alchemyAction,0x58,reinterpret_cast<std::uintptr_t>(alchemy.data()));
 Put(alchemyAction,0x78,reinterpret_cast<std::uintptr_t>(alchemyChild19.data()));
 Put(alchemyChild19,0,std::uintptr_t(0x3A47948));Put(alchemyChild19,0x268,reinterpret_cast<std::uintptr_t>(alchemy.data()));
 Put(alchemyChild19,0x28,int(2));Put(alchemyChild19,0x54,std::uint32_t(55));
 Put(alchemy,0x304,std::uint32_t(14));Put(alchemy,0x150+4*14,std::uint32_t(42));
 Put(alchemy,0x2A0,std::uint32_t(21));Put(alchemy,0x2A4,std::uint32_t(21));
 Put(attachment,0x10,reinterpret_cast<std::uintptr_t>("entity_left"));
 frame.native[1]=frame.native[0];frame.allowed[1]=frame.allowed[0];
 frame.fistWristBoneH31[1]=0;frame.fistSlotBoneH31[1]=1;
 frame.originalToolSlotD71[1]=frame.originalToolSlotD71[0];frame.originalToolSlotValidD71[1]=true;
 ai::witness175.Begin(ai::Kind175::Alchemy,reinterpret_cast<std::uintptr_t>(alchemy.data()));
 g_gameplay_allowed=false;g_dialogue_body178r9=true;(void)ToolContext19c4r2(true);
}
int main(){
 using namespace kcd2vr::contact_handover_d71;
 SetupD71(false);CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&writes==1); // production ordinary first callback
 std::array<QuatT28,5> original{};original[1].t[0]=1.1f;
 BuildToolPoses59c10(frame,original.data(),original.size());
 CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&writes==2); // next packet retains the same ordinary owner
 SetupD71();CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&writes==1);
 CHECK71(std::abs(lastWanted[4]-12.1f)<1e-5&&nativePlacements==1&&readbacks==1);
 CHECK71(socketProofs28==0&&!graspBinding.gripReady55); // no poisoned canonical finger pair
 PairedFingersD71();CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&socketProofs28==1);
 CHECK71(socketProof28.gripReady55&&socketProof28.fingers55.count==2&&std::abs(socketProof28.grip55.p[0]-.1f)<1e-5);
 SetupD71(true,true);CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&writes==1);
 CHECK71(!graspBinding.attached&&!graspBinding.place&&graspBinding.socketOnly28&&nativePlacements==0&&readbacks==0);
 CHECK71(g_native_wrist_retired107[0]==0&&g_native_use_until107[0]==0&&socketProofs28==0);
 PairedFingersD71();CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&socketProofs28==1&&nativePlacements==0);
 BuildToolPoses59c10(frame,original.data(),original.size());
 CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&writes==3&&nativePlacements==0); // pending never flashes back after next pose
 CHECK71(receipts.empty()); // no recorder-off output/readback
 SetupD71();Put(attachment,0x78,std::int16_t(0));QuatT28 rel{};rel.t[2]=.1f;Put(attachment,0x9C,rel);
 CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&std::abs(lastWanted[4]-12)<1e-5&&std::abs(lastWanted[6]-30.1f)<1e-5);
 SetupD71();Put(attachment,0x78,std::int16_t(2));CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0); // unknown original joint
 SetupD71();frame.originalToolSlotValidD71[0]=false;CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();frame.originalToolSlotD71[0].t[0]=2;CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0); // implausible grip
 SetupD71();frame.originalToolSlotD71[0].t[0]=std::numeric_limits<float>::quiet_NaN();CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();frame.stamp.sample=0;CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();frame.stamp.tick=now-81;CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();frame.stamp.epoch=8;CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();frame.publicationR24=0;CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();SetItemHandover(false);SetItemHandover(true);CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&writes==1); // ordinary owner does not depend on legacy trial revision
 SetupD71();completedPublicationD71=4;CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();Put(attachment,0x20,static_cast<void*>(nullptr));CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();onAttachmentWorld55=[](){QuatT28 changed{};changed.t[0]=.05f;Put(attachment,0x9C,changed);};CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();onAttachmentWorld55=[](){Put(attachment,0x78,std::int16_t(0));};CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();onAttachmentWorld55=[](){completedPublicationD71=4;};CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();onAttachmentWorld55=[](){SetItemHandover(false);};CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&writes==1);
 SetupD71();onAttachmentWorld55=[](){SetItemHandover(false);SetItemHandover(true);};CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&writes==1);
 SetupD71();onAttachmentWorld55=[](){Put(attachment,0x20,static_cast<void*>(nullptr));};CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();onAttachmentWorld55=[](){now+=81;};CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();onAttachmentWorld55=[](){tracking=false;};CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();onAttachmentWorld55=[](){g_phys_relevant_id101[0]=42;};CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();onAttachmentWorld55=[](){++graspBinding.token;};CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71(true,true);onAttachmentWorld55=[](){graspBinding.socketOnly28=false;};CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();graspBinding.place=false;CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0); // cleanup retains native owner
 SetupD71();tracking=false;CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupD71();retail_build112::ready=false;CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);retail_build112::ready=true;
 SetupD71();placement=false;CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&writes==0&&nativePlacements==0); // exact owner consumes possible partial native write (R91R3)
 SetupD71();g_phys_relevant_id101[0]=42;CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0); // weapon owner unchanged
 SetupD71();(void)ToolContext19c4r2(false,true);CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupSmithD71();CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&writes==1&&std::abs(lastWanted[4]-12.1f)<1e-5);
 // D72: current native FREE smith membership uses original transaction on
 // its very first placement even with the unrelated pickup experiment off.
 SetupSmithD71();kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Smithing,kcd2vr::minigame_hands45::SixDofVrik);
 SetItemHandover(false);frame.itemHandoverRevisionD71=0;
 CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&writes==1&&std::abs(lastWanted[4]-12.1f)<1e-5);
 SetupSmithD71();kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Smithing,kcd2vr::minigame_hands45::SixDofVrik);
 SetItemHandover(false);frame.itemHandoverRevisionD71=0;completedPublicationD71=4;
 CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupSmithD71();kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Smithing,kcd2vr::minigame_hands45::SixDofVrik);
 SetItemHandover(false);frame.itemHandoverRevisionD71=0;onAttachmentWorld55=[](){++smithBinding37.lifetime;};
 CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupSmithD71();onAttachmentWorld55=[](){++smithBinding37.lifetime;};CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 SetupSmithD71();kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Smithing,kcd2vr::minigame_hands45::SixDofVrik);
 smithModelD71={};smithBlankD71={};Put(smithModelD71,0,std::uintptr_t(kcd2vr::smith_tool36::kModelVft));
 Put(smithModelD71,kcd2vr::smith_tool36::kWorkpieceSlot,reinterpret_cast<std::uintptr_t>(smithBlankD71.data()));
 Put(smithBlankD71,0,std::uintptr_t(kcd2vr::smith_tool36::kWorkpieceVft));Put(smithBlankD71,kcd2vr::smith_tool36::kHand,int(1));
 Put(smithBlankD71,kcd2vr::smith_tool36::kEntity,std::uint32_t(77));
 smithBinding37.model=reinterpret_cast<std::uintptr_t>(smithModelD71.data());smithBinding37.side=1;smithBinding37.tool=1;smithBinding37.phase=8;
 CHECK71(SmithNativeHeatingD56(smithBinding37));
 CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0); // explicit heating owns its native placement
 SetupAlchemyD71();CHECK71(ToolAttachment59c10(self.data(),attachment.data())&&writes==1&&std::abs(lastWanted[4]-12.1f)<1e-5);
 SetupAlchemyD71();onAttachmentWorld55=[](){Put(alchemyAction,0x28,std::uint8_t(0));alchemySlots[1]=0;};CHECK71(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0);
 std::printf("D71 item handover actual-adapter checks: %d PASS\n",checksD71);return 0;
}
