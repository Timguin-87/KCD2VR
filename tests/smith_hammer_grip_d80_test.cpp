// Executes the current native-tool adapter and original-pose builder.
// Authored RightHandSlot samples: retail ToAnvil AttachHammer -> anvil idle.
// Native memory/calls remain explicit leaves in the shared adapter fixture.
#define KCD2VR_ALCHEMY_PRESENTATION_NO_MAIN
#include "alchemy_tool_presentation_d39_test.cpp"
static QuatT28 authoredD80(bool settled,bool axe=false){
 if(axe)return settled?QuatT28{{-.664972f,.723370f,.007961f,-.185699f},{-.074014f,.019727f,.006589f}}:
  QuatT28{{.959321f,-.203453f,-.144929f,.131549f},{-.118789f,.025253f,-.011781f}};
 return settled?QuatT28{{-.557186f,.791042f,.138628f,-.211135f},{-.052518f,.019352f,.009951f}}:
  QuatT28{{.908601f,-.361286f,-.197324f,.070565f},{-.098332f,.023027f,.034142f}};
}
static void hammerD80(bool axe=false){
 setup39(3);alchemyActive=false;smithActive37=true;
 auto& w=kcd2vr::activity_input163::witness175;w.Clear();w.ready=true;w.transitionDepth175=0;
 w.Begin(kcd2vr::activity_input163::Kind175::Smithing,0x1100);
 mh39::setMode(mh39::Smithing,3);mh39::publish(mh39::Smithing,now);
 workstation178r20c1::setSmithAutoHold36(true);
 workstation178r20c1::heldToolActivity59c10=int(workstation178r20c1::Kind::SmithingOther);
 workstation178r20c1::heldToolActivityTick59c10=now;(void)ToolContext19c4r2(true);
 smithBinding37={};auto& b=smithBinding37;
 b.instance=0x1100;b.player=0x1200;b.character=frame.stamp.actor;b.model=0x1300;b.holder=0x1400;
 b.attachment=reinterpret_cast<std::uintptr_t>(attachment.data());b.object=reinterpret_cast<std::uintptr_t>(self.data());
 b.sourceEntity=b.entity=42;b.side=0;b.tool=int(kcd2vr::smith_tool36::Tool::Hammer);b.phase=6;
 b.lifetime=w.lifetime175.load();b.key=123;b.host=77;Put(attachment,0x10,reinterpret_cast<std::uintptr_t>("hand_right"));
 frame.native[0]={};frame.allowed[0].t[0]=2;original39[0]=authoredD80(false,axe);
 frame.tools59c10[0]={};frame.tools59c10[1]={};
}
static void buildHammerD80(){frame.tools59c10[0]={};BuildToolPoses59c10(frame,original39.data(),original39.size());}
static bool samePoseD80(const QuatT28&a,const QuatT28&b){return Len(FromT(a)-FromT(b))<1.e-5f&&
 std::fabs(a.q[0]-b.q[0])+std::fabs(a.q[1]-b.q[1])+std::fabs(a.q[2]-b.q[2])+std::fabs(a.q[3]-b.q[3])<1.e-5f;}
int main(){
 for(bool axe:{false,true}){
  hammerD80(axe);require39(!ToolAttachment59c10(self.data(),attachment.data()),"initial discovery waits for original joint correlation");
  buildHammerD80();require39(frame.tools59c10[0].valid,"phase6 hammer already has live controller placement");
  require39(!frame.tools59c10[0].socket.gripReady67,"phase6 cannot permanently capture an unfinished hammer grasp");
  require39(ToolAttachment59c10(self.data(),attachment.data()),"provisional phase6 hammer remains visible on controller");
  original39[0]=authoredD80(true,axe);buildHammerD80();
  require39(frame.tools59c10[0].valid&&!frame.tools59c10[0].socket.gripReady67&&samePoseD80(frame.tools59c10[0].socket.grip67,original39[0]),
   "current original native relation continues to update before phase11");
  require39(ToolAttachment59c10(self.data(),attachment.data()),"transition frame does not disable hammer placement");
  smithBinding37.phase=11;(void)ToolAttachment59c10(self.data(),attachment.data());buildHammerD80();
  const auto canonical=frame.tools59c10[0].socket.grip67;
  require39(frame.tools59c10[0].valid&&frame.tools59c10[0].socket.gripReady67&&samePoseD80(canonical,authoredD80(true,axe)),
   "phase11 freezes the actual settled native grasp");
  require39(ToolAttachment59c10(self.data(),attachment.data()),"settled hammer remains controller owned");
  for(int phase:{14,16,17,11}){smithBinding37.phase=phase;original39[0]=authoredD80(false,axe);
   require39(ToolAttachment59c10(self.data(),attachment.data()),"same visit stroke preserves attachment ownership");buildHammerD80();
   require39(samePoseD80(frame.tools59c10[0].socket.grip67,canonical),"native stroke cannot wobble settled controller grip");}
  // Same exact tool and holder, but another transfer: the old visit is no authority.
  smithBinding37.phase=6;original39[0]=authoredD80(false,!axe);
  (void)ToolAttachment59c10(self.data(),attachment.data());buildHammerD80();
  require39(frame.tools59c10[0].valid&&!frame.tools59c10[0].socket.gripReady67&&samePoseD80(frame.tools59c10[0].socket.grip67,original39[0]),
   "new ToAnvil visit revokes previous canonical grip without blocking provisional presentation");
  require39(ToolAttachment59c10(self.data(),attachment.data()),"new visit provisional tool places immediately after correlation");
  smithBinding37.phase=11;original39[0]=authoredD80(true,!axe);(void)ToolAttachment59c10(self.data(),attachment.data());buildHammerD80();
  require39(frame.tools59c10[0].valid&&frame.tools59c10[0].socket.gripReady67&&samePoseD80(frame.tools59c10[0].socket.grip67,original39[0]),"new visit learns new exact native grasp");
  auto old=smithBinding37,changed=old;++changed.key;
  require39(!kcd2vr::smith_tool36::sameCanonicalGripD56(old,changed),"different tool identity cannot borrow hammer grip");
  tracking=false;const auto before=writes;require39(!ToolAttachment59c10(self.data(),attachment.data())&&writes==before,"tracking loss does not retain a frozen hammer");
 }
 namespace st=kcd2vr::smith_tool36;
 st::Binding b{};b.tool=int(st::Tool::Tongs);b.phase=8;require39(st::gripCanSettleD56(b),"tongs heating policy unchanged");
 b.tool=int(st::Tool::Workpiece);b.phase=11;require39(st::gripCanSettleD56(b),"workpiece anvil policy unchanged");
 std::printf("Hammer native-grip lifecycle: %u checks PASS; actual adapter, two retail slot tracks; native leaves explicit\n",checks39);
}
