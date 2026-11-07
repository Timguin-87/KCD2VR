// Executes the complete production attachment adapter via the retained native
// memory fixture, including socket discovery, committed tool build and final
// placement. Native attachments and the placement call are explicit host leaves.
// No native Use, ingredient result, detach or free-drop is fabricated.
#define KCD2VR_TOOL_ATTACHMENT_NO_MAIN
#include "tool_attachment59c10_test.cpp"
#include <limits>
namespace ab39=kcd2vr::subsystems::alchemy::bridge67;
namespace mh39=kcd2vr::minigame_hands45;
static unsigned checks39=0;
static void require39(bool value,const char* reason){++checks39;if(!value){std::fprintf(stderr,"FAIL: %s\n",reason);std::exit(1);}}
static ab39::Control presentation39{};
static ab39::NativeTable table39{};
static ab39::NativeTool tool39{};
static std::array<QuatT28,1> original39{};
static void setup39(int mode,float grip=0){
 now=1000;writes=readbacks=0;onAttachmentWorld55=nullptr;receipts.clear();
 self.fill(0);attachment.fill(0);manager.fill(0);character.fill(0);entity.fill(0);
 alchemy.fill(0);alchemyPlayer.fill(0);alchemyAction.fill(0);alchemyChild19.fill(0);alchemySlots.fill(0);
 frame=CommittedFrame6695{};for(auto& mb:g_tool_socket59c10)mb.publish({});
 held[0]=held[1]=0;g_graspBinding13=nullptr;g_phys_relevant_id101[0]=g_phys_relevant_id101[1]=0;
 g_gameplay_allowed=false;g_dialogue_body178r9=true;g_pose_restore_fault6689=false;g_pose_epoch6689=7;
 tracking=placement=identity=childPresent=true;alchemyActive=true;smithActive37=false;
 g_right_grip=grip;g_left_grip=0;retail_build112::ready=true;diagnostics::lab::reference=false;
 auto& w=kcd2vr::activity_input163::witness175;for(auto& active:w.active)active=0;
 w.ready=true;w.transitionDepth175=0;w.Begin(kcd2vr::activity_input163::Kind175::Alchemy,reinterpret_cast<std::uintptr_t>(alchemy.data()));
 mh39::setMode(mh39::Alchemy,mode);mh39::publish(mh39::Alchemy,now);
 kcd2vr::combat_rules178r10::workstationEnabled=true;kcd2vr::combat_rules178r10::f5Rung=mode<2?2:0;
 workstation178r20c1::setExperiment36(mode<2);
 workstation178r20c1::heldToolActivity59c10=1;workstation178r20c1::heldToolActivityTick59c10=now;
 Put(self,0,std::uintptr_t(0x3A68F68));Put(self,8,std::uint32_t(42));
 Put(attachment,0,std::uintptr_t(0x3A657C0));Put(attachment,0x10,reinterpret_cast<std::uintptr_t>("entity_right"));
 Put(attachment,0x20,reinterpret_cast<std::uintptr_t>(self.data()));Put(attachment,0x28,static_cast<void*>(manager.data()));
 Put(manager,0x70,static_cast<void*>(character.data()));Put(character,0,std::uintptr_t(0x40602F8));
 Put(attachment,0x78,std::int16_t(0));Put(attachment,0x7A,std::uint8_t(0));Put(attachment,0x9C,QuatT28{});
 Put(alchemy,0,std::uintptr_t(0x3F60B08));Put(alchemy,0x18,reinterpret_cast<std::uintptr_t>(alchemyPlayer.data()));
 Put(alchemy,0x2C,std::uint8_t(1));Put(alchemyPlayer,0,std::uintptr_t(0x3AA4B98));
 Put(alchemy,0x78,std::uintptr_t(0x3BFA5B0));Put(alchemy,0xB0,reinterpret_cast<std::uintptr_t>(alchemySlots.data()));
 Put(alchemy,0xB8,reinterpret_cast<std::uintptr_t>(alchemySlots.data()+2));
 Put(alchemy,0x2A0,std::uint32_t(0));Put(alchemy,0x2A4,std::uint32_t(21));Put(alchemy,0x150,std::uint32_t(42));
 frame.stamp.actor=reinterpret_cast<std::uintptr_t>(character.data());frame.stamp.epoch=7;frame.stamp.tick=now;frame.stamp.sample=10;
 frame.mapGripM39[0]=grip;frame.doorTrackingEpoch66=9;
 frame.valid[0]=true;frame.native[0].t[0]=1;frame.allowed[0].t[0]=2;original39[0].t[0]=1.1f;
 g_henry_char_attachment_manager=manager.data();(void)ToolContext19c4r2(true);
 kcd2vr::tracking59c6::epoch=9;
 kcd2vr::alchemy_tool18::Binding actual{};
 require39(ReadAlchemyTool18(attachment.data(),42,frame.stamp.actor,actual),"actual native ordinary-tool reader proves current attachment");
 table39={};table39.instance=actual.instance;table39.player=actual.player;table39.playerEntity=77;
 table39.lifetime=actual.lifetime;table39.tick=now;table39.sequence=20;
 for(int i=0;i<21;++i)table39.refs[i]={i,i==0?42u:0u};
 presentation39={};presentation39.enabled=presentation39.passenger=presentation39.trackedPresentation71=true;
 presentation39.instance=actual.instance;presentation39.lifetime=actual.lifetime;presentation39.tick=now;
 presentation39.poseEpoch=7;presentation39.sourceEpoch=9;presentation39.modeEpoch=mh39::modeEpoch67.load();
 tool39={};tool39.instance=actual.instance;tool39.character=actual.character;tool39.lifetime=actual.lifetime;
 tool39.tick=now;tool39.poseEpoch=7;tool39.entity=42;tool39.side=0;tool39.type=0;tool39.joined=true;tool39.binding=actual;
 ab39::enabled=true;ab39::table.publish(table39);ab39::control.publish(presentation39);ab39::tools[0].publish(tool39);ab39::tools[1].publish({});
}
static void commit39(){
 require39(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0,"uncommitted native socket discovery cannot place a tool");
 BuildToolPoses59c10(frame,original39.data(),original39.size());
 require39(frame.tools59c10[0].valid,"an exact current Free/6DOF attachment remains buildable after release or delayed Attach");
 require39(ToolAttachment59c10(self.data(),attachment.data())&&writes==1,"actual attachment callback places the still-native tool on the committed wrist with an open hand");
}
#ifndef KCD2VR_ALCHEMY_PRESENTATION_NO_MAIN
int main(){
 for(int mode:{2,3})for(float grip:{0.f,.45f}){setup39(mode,grip);commit39();}
 for(int mode:{0,1}){setup39(mode);require39(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0,
   "Native/Semi cannot borrow the Free/6DOF low-grip presentation exception");}
 for(int fault=0;fault<11;++fault){setup39(3);commit39();const int before=writes;
  switch(fault){
   case 0:tool39.joined=false;ab39::tools[0].publish(tool39);break;
   case 1:Put(attachment,0x20,reinterpret_cast<std::uintptr_t>(entity.data()));break;
   case 2:Put(alchemy,0x2A0,std::uint32_t(21));break;
   case 3:presentation39.poseEpoch=8;ab39::control.publish(presentation39);break;
   case 4:presentation39.tick=now-81;ab39::control.publish(presentation39);break;
   case 5:table39.refs[0].entity=43;ab39::table.publish(table39);break;
   case 6:kcd2vr::tracking59c6::epoch=10;break;
   case 7:g_right_grip=std::numeric_limits<float>::quiet_NaN();break;
   case 8:presentation39.trackedPresentation71=false;ab39::control.publish(presentation39);break;
   case 9:onAttachmentWorld55=+[](){ab39::control.publish({});};break;
   case 10:onAttachmentWorld55=+[](){
    // Change the independent saved-mode witness while keeping the old packet
    // epoch usable. Legacy F5 may still own tools, but it cannot borrow the
    // stronger saved-Free open-Grip exception at the final writer.
    mh39::alchemyMode67=mh39::SemiPassenger;kcd2vr::combat_rules178r10::f5Rung=2;
    workstation178r20c1::experimentArmed36=true;
   };break;
  }
  require39(!ToolAttachment59c10(self.data(),attachment.data())&&writes==before,
    "detach, replacement, stale/retired presentation or invalid input stops the actual writer immediately");
  onAttachmentWorld55=nullptr;
 }
 std::printf("alchemy_tool_presentation_d39: %u checks PASS (actual full attachment adapter; native memory/placement leaves only)\n",checks39);
}

#endif
