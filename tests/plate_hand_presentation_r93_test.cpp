#define KCD2VR_ALCHEMY_PRESENTATION_NO_MAIN
#include "alchemy_tool_presentation_d39_test.cpp"
static void mappedR93(){
 setup39(3,0);frame.valid[1]=true;frame.native[1].t[0]=8;frame.allowed[1].t[0]=4;frame.mapGripM39[1]=1;g_left_grip=1;
 table39.handTypesKnown68=true;table39.handType68={0,21};ab39::table.publish(table39);
 presentation39.passenger=false;presentation39.entity=42;presentation39.type=0;presentation39.side=0;
 presentation39.held[1]=true;presentation39.admissionTickR22=now-1;
 presentation39.plateBindingR93=tool39.binding;presentation39.plateGestureSideR93=1;ab39::control.publish(presentation39);
}
static void commitR93(){
 require39(!ToolAttachment59c10(self.data(),attachment.data())&&writes==0,"first native binding only publishes exact socket");
 BuildToolPoses59c10(frame,original39.data(),original39.size());
 require39(frame.tools59c10[1].valid&&!frame.tools59c10[0].valid,"native RIGHT plate is built on requested LEFT controller only");
 require39(frame.tools59c10[1].socket.alchemy18.side==0&&frame.tools59c10[1].socket.side==1,"native binding is retained exactly");
 require39(std::abs(frame.tools59c10[1].model.t[0]-4.1f)<.00001f,"grip is native RIGHT-relative, applied to LEFT wrist; no cross-hand 7m offset");
 require39(ToolAttachment59c10(self.data(),attachment.data())&&writes==1,"actual attachment callback writes mapped tracked plate");
 require39(std::abs(lastWanted[4]-14.1f)<.00001f,"placed world pose follows LEFT controller and original native grip");
}
int main(){
 mappedR93();commitR93();
 for(int fault=0;fault<9;++fault){mappedR93();commitR93();const auto count=writes;
  switch(fault){
   case 0:presentation39.plateBindingR93.attachment++;ab39::control.publish(presentation39);break;
   case 1:table39.handType68[1]=13;ab39::table.publish(table39);break;
   case 2:tool39.tick=now-81;ab39::tools[0].publish(tool39);break;
   case 3:onAttachmentWorld55=+[](){presentation39.plateBindingR93={};presentation39.plateGestureSideR93=-1;ab39::control.publish(presentation39);};break;
   case 4:onAttachmentWorld55=+[](){table39.handType68[1]=13;ab39::table.publish(table39);};break;
   case 5:frame.valid[1]=false;break;
   case 6:Put(alchemy,0x2A0,unsigned(21));break;
   case 7:Put(attachment,0x20,reinterpret_cast<std::uintptr_t>(entity.data()));break;
   case 8:presentation39.admissionTickR22=now;ab39::control.publish(presentation39);break;
  }
  require39(!ToolAttachment59c10(self.data(),attachment.data())&&writes==count,"mapping/hand/native-binding change revokes actual writer");
  onAttachmentWorld55=nullptr;
 }
 // Normal right-hand tools retain their existing world placement.
 setup39(3,0);commit39();require39(std::abs(lastWanted[4]-12.1f)<.00001f,"ordinary right-hand plate unchanged");
 std::printf("plate_hand_presentation_r93: %u checks PASS (full production attachment adapter)\n",checks39);
}
