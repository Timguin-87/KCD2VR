// Current full attachment writer, native-hand observer and ORIGINAL pose
// builder. Native memory/placement leaves remain explicit in shared fixture.
#define KCD2VR_ALCHEMY_PRESENTATION_NO_MAIN
#include "alchemy_tool_presentation_d39_test.cpp"
static std::array<unsigned char,0x400> smithModelD56{};
static std::array<unsigned char,0x40> smithWorkpieceD56{};
static void supportD56(int phase=7){
 setup39(3);alchemyActive=false;smithActive37=true;
 auto& w=kcd2vr::activity_input163::witness175;w.Clear();w.ready=true;w.transitionDepth175=0;
 w.Begin(kcd2vr::activity_input163::Kind175::Smithing,0x1100);
 kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Smithing,3);
 kcd2vr::minigame_hands45::publish(kcd2vr::minigame_hands45::Smithing,now);
 workstation178r20c1::setSmithAutoHold36(true);
 workstation178r20c1::heldToolActivity59c10=int(workstation178r20c1::Kind::SmithingOther);
 workstation178r20c1::heldToolActivityTick59c10=now;
 (void)ToolContext19c4r2(true);
 smithBinding37={};auto& b=smithBinding37;
 b.instance=0x1100;b.player=0x1200;b.character=frame.stamp.actor;
 b.model=reinterpret_cast<std::uintptr_t>(smithModelD56.data());b.holder=0x1300;
 b.attachment=reinterpret_cast<std::uintptr_t>(attachment.data());b.object=reinterpret_cast<std::uintptr_t>(self.data());
 b.sourceEntity=b.entity=42;b.side=1;b.tool=int(kcd2vr::smith_tool36::Tool::Tongs);b.phase=phase;b.lifetime=w.lifetime175.load();b.key=123;b.host=77;
 Put(attachment,0x10,reinterpret_cast<std::uintptr_t>("hand_left"));
 smithModelD56.fill(0);smithWorkpieceD56.fill(0);
 namespace st=kcd2vr::smith_tool36;
 Put(smithModelD56,0,std::uintptr_t(st::kModelVft));Put(smithModelD56,st::kWorkpieceSlot,reinterpret_cast<std::uintptr_t>(smithWorkpieceD56.data()));
 Put(smithWorkpieceD56,0,std::uintptr_t(st::kWorkpieceVft));Put(smithWorkpieceD56,st::kHand,int(1));
 Put(smithWorkpieceD56,st::kEntity,std::uint32_t(44));Put(smithWorkpieceD56,st::kHost,std::uint32_t(0));
 Put(smithWorkpieceD56,st::kAttached,std::uint8_t(0));Put(smithWorkpieceD56,st::kAssetSlot,std::uint8_t(0));
 frame.valid[0]=false;frame.valid[1]=true;frame.native[1].t[0]=1;frame.allowed[1].t[0]=2;original39[0].t[0]=1.1f;
 frame.tools59c10[0]={};frame.tools59c10[1]={};
}
static void buildD56(){frame.tools59c10[0]={};frame.tools59c10[1]={};BuildToolPoses59c10(frame,original39.data(),original39.size());}
static void attachedD56(){namespace st=kcd2vr::smith_tool36;
 Put(smithWorkpieceD56,st::kHost,std::uint32_t(42));Put(smithWorkpieceD56,st::kAttached,std::uint8_t(1));Put(smithWorkpieceD56,st::kAssetSlot,std::uint8_t(1));}
int main(){
 supportD56();require39(!ToolAttachment59c10(self.data(),attachment.data()),"uncommitted tongs socket cannot place a tool");buildD56();
 require39(frame.tools59c10[1].valid&&!frame.tools59c10[1].socket.gripReady67,"transfer tongs grip remains provisional instead of freezing an entry pose");
 require39(ToolAttachment59c10(self.data(),attachment.data()),"current provisional tongs remains controller placed");
 original39[0].t[0]=1.24f;buildD56();
 require39(!frame.tools59c10[1].socket.gripReady67&&std::fabs(frame.tools59c10[1].model.t[0]-2.24f)<1.e-5,
  "unsettled transfer follows current untouched native relationship");
 smithBinding37.phase=8;
 // The detached blank stays at its native forge fixture. It does not own
 // Henry's whole left wrist: D73 retained the attached tongs as a tracked tool.
 require39(!ObserveNativeActivityHand46(self.data(),attachment.data()),"detached forge blank does not grant native wrist ownership");
 kcd2vr::smith_tool36::Binding current{};
 require39(NativeActivityHandOwned46(false,frame.stamp.actor,frame.stamp.epoch,1,now,&current,false)&&SmithNativeHeatingD56(current),
  "exact native membership still proves the detached blank independently of wrist ownership");
 require39(!NativeActivityHandOwned46(false,frame.stamp.actor,frame.stamp.epoch,1,now),
  "6DOF heating cannot acquire the lower-mode whole-wrist fixture");
 const auto detachedBlank=smithWorkpieceD56;
 const int before=writes;require39(!ToolAttachment59c10(self.data(),attachment.data())&&writes==before,
  "new heating stage needs its own current original tool relation");buildD56();
 require39(frame.tools59c10[1].valid&&frame.tools59c10[1].socket.gripReady67&&
  frame.tools59c10[1].socket.id==42,"native detached blank leaves the exact held tongs controller-placeable");
 const auto forgeGrip=frame.tools59c10[1].socket.grip67;
 require39(ToolAttachment59c10(self.data(),attachment.data()),"tracked tongs place during heating without moving detached blank");
 const float firstHeatingPlacement=lastWanted[4];
 frame.allowed[1].t[0]+=.2f;buildD56();
 require39(ToolAttachment59c10(self.data(),attachment.data())&&std::fabs(lastWanted[4]-firstHeatingPlacement-.2f)<1.e-5,
  "moving controller moves tongs during heating instead of freezing the left wrist");
 require39(smithWorkpieceD56==detachedBlank,"tracked placement does not mutate native detached workpiece binding");
 Put(self,8,std::uint32_t(44));const int beforeBlank=writes;
 require39(!ToolAttachment59c10(self.data(),attachment.data())&&writes==beforeBlank,
  "detached blank entity cannot masquerade as the attached tongs");Put(self,8,std::uint32_t(42));
 attachedD56();require39(!ObserveNativeActivityHand46(self.data(),attachment.data()),"reattaching the blank does not seize the tracked wrist");
 require39(ToolAttachment59c10(self.data(),attachment.data()),"same tongs stay continuous across native blank reattachment");buildD56();
 require39(frame.tools59c10[1].valid&&frame.tools59c10[1].socket.gripReady67&&
  std::memcmp(&forgeGrip,&frame.tools59c10[1].socket.grip67,sizeof(forgeGrip))==0,
  "same heating tongs keep their current canonical grip across blank reattachment");
 smithBinding37.phase=11;original39[0].t[0]=1.38f;
 require39(!ToolAttachment59c10(self.data(),attachment.data()),"anvil stage cannot borrow the heating grip");buildD56();
 const auto anvilGrip=frame.tools59c10[1].socket.grip67;
 require39(frame.tools59c10[1].socket.gripReady67&&std::fabs(anvilGrip.t[0]-.38f)<1.e-5&&
  std::memcmp(&anvilGrip,&forgeGrip,sizeof(anvilGrip)),"settled anvil recaptures actual native tongs relation");
 require39(ToolAttachment59c10(self.data(),attachment.data()),"new anvil support grip writes only after correlation");
 for(int phase:{14,16,15,17,11}){smithBinding37.phase=phase;original39[0].t[0]+=.1f;
  require39(ToolAttachment59c10(self.data(),attachment.data()),"raise/fall remains continuous on exact anvil support");buildD56();
  require39(std::memcmp(&anvilGrip,&frame.tools59c10[1].socket.grip67,sizeof(anvilGrip))==0,"native stroke cannot animate canonical anvil grip");}
 for(int fault=0;fault<7;++fault){supportD56(8);
  switch(fault){case 0:smithBinding37.phase=11;break;case 1:smithBinding37.side=0;break;
   case 2:attachedD56();break;case 3:Put(smithWorkpieceD56,0,std::uintptr_t(kcd2vr::smith_tool36::kSlotToolVft));break;
   case 4:Put(smithWorkpieceD56,kcd2vr::smith_tool36::kHost,std::uint32_t(42));break;
   case 5:Put(smithWorkpieceD56,kcd2vr::smith_tool36::kEntity,std::uint32_t(0));break;
   case 6:kcd2vr::minigame_hands45::smithingMode67=2;break;}
  require39(!SmithNativeHeatingD56(smithBinding37),"wrong phase/side/attachment/holder/mode cannot borrow native heating ownership");}
 auto hammer=smithBinding37;hammer.side=0;hammer.tool=int(kcd2vr::smith_tool36::Tool::Hammer);hammer.phase=11;
 auto raised=hammer;raised.phase=14;
 require39(kcd2vr::smith_tool36::sameCanonicalGripD56(hammer,raised),"right hammer retains settled anvil grip through raise");
 hammer.phase=6;
 require39(!kcd2vr::smith_tool36::sameCanonicalGripD56(hammer,raised),"right hammer transfer cannot borrow another visit's settled grip");
 std::printf("D56 support lifecycle: %u checks PASS; actual attachment/observer/builder, native leaves explicit\n",checks39);
}
