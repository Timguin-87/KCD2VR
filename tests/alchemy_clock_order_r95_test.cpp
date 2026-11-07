// Complete current ToolAttachment59c10 + real presentation mailbox adapter.
// Only native-memory/placement leaves and a controlled scheduling point are doubles.
#define KCD2VR_ALCHEMY_PRESENTATION_NO_MAIN
#include "alchemy_tool_presentation_d39_test.cpp"
static bool injectedR95=false,lateCopyR95=false;
static void publishCurrentControlR95(const void* p){
 if(injectedR95||p!=(lateCopyR95?alchemy.data()+0x150:self.data()+8))return;
 injectedR95=true;++now;if(!lateCopyR95)kcd2vr::workstation178r20c1::heldToolActivityTick59c10=now;presentation39.tick=now;ab39::control.publish(presentation39);
}
int main(){
 setup39(3,1);commit39();const int before=writes;
 beforeNativeFixtureReadR95=publishCurrentControlR95;
 const bool consumed=ToolAttachment59c10(self.data(),attachment.data());
 beforeNativeFixtureReadR95=nullptr;
 require39(injectedR95&&now==1001,"same-owner valid producer published during native attachment read");
 require39(consumed&&writes==before+1,"actual attachment accepts valid post-entry-clock publication");
 setup39(3,1);commit39();injectedR95=false;lateCopyR95=true;
 beforeNativeFixtureReadR95=publishCurrentControlR95;
 const int lateBefore=writes;const bool lateConsumed=ToolAttachment59c10(self.data(),attachment.data());
 beforeNativeFixtureReadR95=nullptr;lateCopyR95=false;
 require39(injectedR95&&lateConsumed&&writes==lateBefore+1,"control published during later native binding read uses post-copy validation clock");
 for(int bad=0;bad<12;++bad){
  setup39(3,1);commit39();const int prior=writes;
  switch(bad){
   case 0:presentation39.tick=now+1;ab39::control.publish(presentation39);break;
   case 1:table39.tick=now+1;ab39::table.publish(table39);break;
   case 2:tool39.tick=now+1;ab39::tools[0].publish(tool39);break;
   case 3:presentation39.tick=now-81;ab39::control.publish(presentation39);break;
   case 4:table39.tick=now-81;ab39::table.publish(table39);break;
   case 5:tool39.tick=now-81;ab39::tools[0].publish(tool39);break;
   case 6:++presentation39.sourceEpoch;ab39::control.publish(presentation39);break;
   case 7:++presentation39.modeEpoch;ab39::control.publish(presentation39);break;
   case 8:++table39.lifetime;ab39::table.publish(table39);break;
   case 9:++tool39.binding.object;ab39::tools[0].publish(tool39);break;
   case 10:onAttachmentWorld55=[](){ab39::control.publish({});};break;
   case 11:onAttachmentWorld55=[](){++tool39.binding.entity;ab39::tools[0].publish(tool39);};break;
  }
  require39(!ToolAttachment59c10(self.data(),attachment.data())&&writes==prior,
   "future/stale publication, changed ownership and late retirement remain refusals");
  onAttachmentWorld55=nullptr;
 }
 setup39(3,1);
 // Clock callback publishes different data AFTER the adapter copied the mailbox.
 // The accepted output must be those original copies, never a reread from clock time.
 ab39::Control accepted{};const auto original=presentation39;unsigned clockCalls=0;
 const bool sameCopies=ab39::presentationOwnsCurrentR95(tool39.binding,frame.stamp.epoch,[&](){
  ++clockCalls;++presentation39.instance;ab39::control.publish(presentation39);
  ++table39.lifetime;ab39::table.publish(table39);return now;
 },&accepted);
 require39(sameCopies&&clockCalls==1&&accepted.instance==original.instance&&accepted.tick==original.tick,
  "clock is sampled after the exact copies that validation and output consume");
 require39(!ab39::presentationOwnsCurrentR95(tool39.binding,frame.stamp.epoch,[&](){return now;}),
  "the next call refuses replacements published after the previous copy");
 setup39(3,1);presentation39.tick=table39.tick=tool39.tick=now+1;
 ab39::control.publish(presentation39);ab39::table.publish(table39);ab39::tools[0].publish(tool39);
 require39(ab39::presentationOwnsCurrentR95(tool39.binding,frame.stamp.epoch,[&](){return ++now;}),
  "all three valid next-millisecond copies are judged at the later sampled clock");
 std::printf("R95 alchemy clock order: %u checks PASS (actual attachment and snapshot adapter)\n",checks39);
}
