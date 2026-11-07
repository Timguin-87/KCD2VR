// Real attachment discovery/builder/writer with the existing native memory
// fixture. Native read-enable publications are explicit inputs; no opening
// animation, rendered palm clearance, or game action success is fabricated.
#define KCD2VR_ALCHEMY_PRESENTATION_NO_MAIN
#include "alchemy_tool_presentation_d39_test.cpp"
namespace ai53=kcd2vr::activity_input163;
namespace as53=kcd2vr::alchemy_semi46;
static ai53::AlchemyBook45 book53{};
static as53::BookEnable enabled53{};
static void setupBook53(int mode=3){
 setup39(mode);as53::hooksReady=true;as53::bookGeneration=11;
 Put(alchemy,0x2A0,std::uint32_t(19));Put(alchemy,0x150+4*19,std::uint32_t(42));
 kcd2vr::alchemy_tool18::Binding actual{};
 require39(ReadAlchemyTool18(attachment.data(),42,frame.stamp.actor,actual)&&actual.type==19,
  "current native membership proves the book rather than a selection hint");
 tool39.type=19;tool39.binding=actual;ab39::tools[0].publish(tool39);
 table39.refs[0]={0,0};table39.refs[19]={19,42};ab39::table.publish(table39);
 book53={actual.instance,0x3500,actual.lifetime,now,11,42};
 enabled53={};enabled53.instance=actual.instance;enabled53.action=book53.action;
 enabled53.lifetime=actual.lifetime;enabled53.generation=11;
 ai53::alchemyBook45.publish(book53);as53::bookEnable.publish(enabled53);
 original39[0]=QuatT28{};original39[0].t[0]=1.02f;
 require39(!ToolAttachment59c10(self.data(),attachment.data()),"first native callback only publishes the uncorrelated book socket");
 BuildToolPoses59c10(frame,original39.data(),original39.size());
 require39(frame.tools59c10[0].valid,"entry book remains attached and renderable before read-enable");
}
static void rebuild53(float originalX){
 require39(ToolAttachment59c10(self.data(),attachment.data()),"actual writer revalidates the same live book before next original-joint build");
 original39[0].t[0]=originalX;
 BuildToolPoses59c10(frame,original39.data(),original39.size());
}
int main(){
 setupBook53();require39(!frame.tools59c10[0].socket.gripReady67,"entry pose does not freeze the persistent book grip");
 rebuild53(1.20f);
 require39(!frame.tools59c10[0].socket.gripReady67&&std::fabs(frame.tools59c10[0].model.t[0]-2.20f)<1e-5,
  "provisional relation follows the new untouched native grip during entry");
 enabled53.enabled=true;as53::bookEnable.publish(enabled53);rebuild53(1.24f);
 require39(frame.tools59c10[0].socket.gripReady67&&std::fabs(frame.tools59c10[0].model.t[0]-2.24f)<1e-5,
  "same entity and current native read-enable generation capture the settled book grip");
 const auto canonical=frame.tools59c10[0].socket.grip67;rebuild53(1.5f);
 require39(std::memcmp(&canonical,&frame.tools59c10[0].socket.grip67,sizeof(canonical))==0&&
  std::fabs(frame.tools59c10[0].model.t[0]-2.24f)<1e-5,"later native page animation cannot move the captured controller grip");
 const int before=writes;Put(attachment,0x20,reinterpret_cast<std::uintptr_t>(entity.data()));
 require39(!ToolAttachment59c10(self.data(),attachment.data())&&writes==before,"detach revokes the writer even with a settled book grip");
 for(int fault=0;fault<7;++fault){
  setupBook53();enabled53.enabled=true;
  switch(fault){
   case 0:book53.entity=43;break;
   case 1:book53.action=0x3600;break;
   case 2:book53.lifetime++;break;
   case 3:book53.tick=now-121;break;
   case 4:enabled53.generation++;break;
   case 5:as53::hooksReady=false;break;
   case 6:as53::bookGeneration++;break;
  }
  ai53::alchemyBook45.publish(book53);as53::bookEnable.publish(enabled53);rebuild53(1.22f);
  require39(!frame.tools59c10[0].socket.gripReady67,"wrong entity/action/lifetime/generation or stale unavailable read-enable cannot certify the grip");
 }
 setupBook53(2);require39(frame.tools59c10[0].socket.gripReady67,"mode2 preserves its existing immediate native-grip policy");
 std::printf("alchemy_book_grip_d53: %u checks PASS; provisional entry, exact native read-enable, no predicted pose\n",checks39);
}
