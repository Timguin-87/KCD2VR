// Complete actual attachment + committed pose adapter. World placement and
// native bindings are explicit leaves. Release must not introduce free-space parking.
#define KCD2VR_ALCHEMY_PRESENTATION_NO_MAIN
#include "alchemy_tool_presentation_d39_test.cpp"
static CommittedFrame6695 priorD47{};
// D55: native release-presentation remains the mode2 baseline; mode3 has its
// separate tracked-presentation fixture, with actual detach still authoritative.
static void setupParkD47(int mode=2){
 setup39(mode,1);committedReadOverrideD47=nullptr;attachmentRootD47={10,20,30};presentation39.passenger=false;
 Put(alchemy,0x2A0,std::uint32_t(21));Put(alchemy,0x150+4*3,std::uint32_t(42));
 const auto action=reinterpret_cast<std::uintptr_t>(alchemyAction.data());alchemySlots[1]=action;
 Put(alchemyAction,0,std::uintptr_t(0x3F60770));Put(alchemyAction,0x58,reinterpret_cast<std::uintptr_t>(alchemy.data()));
 Put(alchemyAction,0x40,reinterpret_cast<std::uintptr_t>(alchemy.data()+0x78));Put(alchemyAction,0x28,std::uint8_t(1));Put(alchemyAction,0x3C,int(1));
 require39(ReadAlchemyTool18(attachment.data(),42,frame.stamp.actor,tool39.binding),"exact temporary native pestle binding");
 tool39.type=3;tool39.parent=action;table39.refs[0].entity=0;table39.refs[3].entity=42;
 presentation39.type=3;presentation39.entity=42;presentation39.side=0;presentation39.parent=action;
 presentation39.held[0]=true;ab39::table.publish(table39);ab39::tools[0].publish(tool39);ab39::control.publish(presentation39);
 frame.world[4]=10;frame.world[5]=20;frame.world[6]=30;frame.mapGripM39[0]=1;frame.doorTrackingEpoch66=9;
 kcd2vr::alchemy_semi46::WorkWitness w{table39.instance,action,reinterpret_cast<std::uintptr_t>(alchemyChild19.data()),table39.lifetime,now,44,kcd2vr::alchemy_semi46::Mortar,3,3,true,false};
 kcd2vr::alchemy_semi46::work.publish(w);commit39();
 require39(!frame.tools59c10[0].parkedD47,"closed native-bound pestle follows tracked wrist");
}
static bool nextParkD47(float grip,float wrist,float root=10){
 priorD47=frame;now+=16;++frame.stamp.sample;frame.stamp.tick=now;
 mh39::publish(mh39::Alchemy,now);workstation178r20c1::heldToolActivityTick59c10=now;(void)ToolContext19c4r2(true);
 frame.mapGripM39[0]=grip;g_right_grip=grip;frame.allowed[0].t[0]=wrist;frame.world[4]=root;attachmentRootD47.x=root;
 presentation39.tick=tool39.tick=table39.tick=now;presentation39.held[0]=grip>=.35f;
 ab39::control.publish(presentation39);ab39::tools[0].publish(tool39);ab39::table.publish(table39);
 committedReadOverrideD47=&priorD47;BuildToolPoses59c10(frame,original39.data(),original39.size());committedReadOverrideD47=nullptr;
 return ToolAttachment59c10(self.data(),attachment.data());
}
int main(){
 setupParkD47();const int initialWrites=writes;
 require39(!nextParkD47(0,2.4f)&&writes==initialWrites,"release yields actual placement to native without any parked-world write");
 require39(!frame.tools59c10[0].parkedD47,"released pestle is never frozen in world space");
 require39(!nextParkD47(0,2.7f,11)&&writes==initialWrites,"body motion does not make a released tool a VR-owned floating prop");
 // The same exact native binding and canonical grip survive the release.
 // Regrip does not require an invented 15 cm world-park radius or another edge.
 require39(nextParkD47(.8f,2.7f,11)&&writes==initialWrites+1,"regrip resumes the same canonical controller attachment");
 require39(!frame.tools59c10[0].parkedD47,"regripped pestle has no parked owner");
 require39(!nextParkD47(0,1.4f,11),"second right release yields independently");
 presentation39.passenger=true;
 require39(nextParkD47(0,1.8f,11),"accepted native passenger return retains its existing presentation policy");
 presentation39.passenger=false;presentation39.controllerMortar70=true;
 require39(nextParkD47(0,1.8f,11),"qualified controller mortar placement keeps its accepted native route");
 std::printf("D49 pestle release: %u checks PASS; native fallback, no invented independent right-tool home\n",checks39);
 return 0;
}
