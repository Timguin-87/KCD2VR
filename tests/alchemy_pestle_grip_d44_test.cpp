// Actual production tool discovery, ORIGINAL-joint builder and placement.
// Native memory and placement leaves are the existing retained D39 fixture.
#define KCD2VR_ALCHEMY_PRESENTATION_NO_MAIN
#include "alchemy_tool_presentation_d39_test.cpp"
static kcd2vr::alchemy_semi46::WorkWitness work44{};
static void setupPestle44(){
 setup39(3,1);Put(alchemy,0x2A0,std::uint32_t(21));Put(alchemy,0x150+4*3,std::uint32_t(42));
 const auto action=reinterpret_cast<std::uintptr_t>(alchemyAction.data());alchemySlots[1]=action;
 Put(alchemyAction,0,std::uintptr_t(0x3F60770));Put(alchemyAction,0x58,reinterpret_cast<std::uintptr_t>(alchemy.data()));
 Put(alchemyAction,0x40,reinterpret_cast<std::uintptr_t>(alchemy.data()+0x78));Put(alchemyAction,0x28,std::uint8_t(1));Put(alchemyAction,0x3C,int(1));
 require39(ReadAlchemyTool18(attachment.data(),42,frame.stamp.actor,tool39.binding),"actual native temporary mortar action binds right pestle");
 tool39.type=3;tool39.parent=action;table39.refs[0].entity=0;table39.refs[3].entity=42;
 ab39::table.publish(table39);ab39::tools[0].publish(tool39);
 work44={table39.instance,action,reinterpret_cast<std::uintptr_t>(alchemyChild19.data()),table39.lifetime,now,44,kcd2vr::alchemy_semi46::Mortar,1,3,false,false};
 kcd2vr::alchemy_semi46::work.publish(work44);
}
int main(){
 setupPestle44();commit39();
 require39(!frame.tools59c10[0].socket.gripReady67,"phase1 take keeps grip provisional instead of freezing low initial socket");
 const auto first=frame.tools59c10[0].socket.grip67;
 original39[0].t[2]+=.14f;++frame.stamp.sample;
 (void)ToolAttachment59c10(self.data(),attachment.data());BuildToolPoses59c10(frame,original39.data(),original39.size());
 require39(frame.tools59c10[0].valid&&!frame.tools59c10[0].socket.gripReady67&&
  frame.tools59c10[0].socket.grip67.t[2]>first.t[2]+.13f,"native take relationship can reach authored settled palm without stale frozen offset");
 for(int fault=0;fault<9;++fault){auto bad=work44;bad.phase=2;bad.working=true;
  if(fault==0)bad.instance++;if(fault==1)bad.lifetime++;if(fault==2)bad.parent++;if(fault==3)bad.child=0;
  if(fault==4)bad.kind=kcd2vr::alchemy_semi46::Resource;if(fault==5)bad.phase=1;if(fault==6)bad.working=false;
  if(fault==7)bad.tick=now-81;if(fault==8)bad.tick=now+1;
  require39(!kcd2vr::alchemy_tool18::pestleGripReadyD44(tool39.binding,bad,now),"wrong/stale/non-working parent cannot qualify canonical pestle");
 }
 work44.phase=2;work44.working=true;kcd2vr::alchemy_semi46::work.publish(work44);
 BuildToolPoses59c10(frame,original39.data(),original39.size());
 require39(frame.tools59c10[0].socket.gripReady67,"same exact working native mortar phase2 qualifies settled grip");
 const auto stable=frame.tools59c10[0].socket.grip67;
 (void)ToolAttachment59c10(self.data(),attachment.data());original39[0].t[2]-=.30f;
 work44.phase=3;kcd2vr::alchemy_semi46::work.publish(work44);BuildToolPoses59c10(frame,original39.data(),original39.size());
 require39(frame.tools59c10[0].socket.gripReady67&&std::memcmp(&stable,&frame.tools59c10[0].socket.grip67,sizeof(stable))==0,
  "working phase3 motion keeps settled grip and does not replay native pestle movement");
 work44.phase=4;work44.working=false;kcd2vr::alchemy_semi46::work.publish(work44);
 (void)ToolAttachment59c10(self.data(),attachment.data());BuildToolPoses59c10(frame,original39.data(),original39.size());
 require39(frame.tools59c10[0].socket.gripReady67&&std::memcmp(&stable,&frame.tools59c10[0].socket.grip67,sizeof(stable))==0,
  "same proven grip survives return phase without recapture");
 std::printf("D44 pestle grip: %u checks PASS; actual attachment/build, explicit native leaves\n",checks39);
}
