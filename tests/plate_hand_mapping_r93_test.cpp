// Captured R92R1: LEFT admission, retail entity_right plate attachment, then idle.
// Run the real input owner and selector; native table/attachment reads are supplied evidence leaves.
#define KCD2VR_INPUT67_NO_MAIN
#include "subsystems/alchemy/input67_test.cpp"
namespace ab=kcd2vr::subsystems::alchemy::bridge67;
namespace mh=kcd2vr::minigame_hands45;
namespace kcd2vr::frik_pose_probe {
static workstation_sample46::Frame plateFrameR93{};
bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept {out=plateFrameR93;return true;}
}
static void beginR93(Fixture& q,bool empty=true){
 mh::alchemyMode67=mh::SixDofVrik;mh::modeEpoch67=6;kcd2vr::tracking59c6::epoch=5;
 q.in.context.tableGeneration=3;q.in.proximityCraft1=q.in.handsOnlyD53=true;
 q.in.nativeFrame.table.actionKnown=q.in.nativeFrame.table.handTypesKnown68=true;
 q.in.nativeFrame.table.handType68={empty?21u:0u,21u};q.admit(0,1);
 check(q.step().control.side==1);
 check(q.state.plateAdmissionR93==empty);check(!q.state.plateBindingR93);
 q.advance();q.in.action.parent=999;q.in.nativeFrame.table.action=999;q.step();
}
static a::Output attachR93(Fixture& q){
 q.advance();q.tool(0,0);q.in.tool[0].binding={1,2,888,777,100,0,0,0,0,666,3};
 q.in.nativeFrame.table.handType68={0,21};
 // The old left resource mailbox may persist; its original stale tick is not a competing hand owner.
 q.in.tool[1]={1,0,888,3,q.in.now-100,4,113,1,13,true};
 return q.step();
}
static a::Output idleR93(Fixture& q){
 q.advance(81);q.in.tool[1]={};q.in.action.parent=0;q.in.nativeFrame.table.action=0;
 return q.step();
}
static void potR93(Fixture& q,int slot=6){
 auto& o=q.in.nativeFrame.offers[slot];o={{slot,unsigned(100+slot)},{.05,0,0},q.in.now,true};o.boundsValid=true;
 for(int j=0;j<3;++j){o.bounds.local[j]=-.01f;o.bounds.local[j+3]=.01f;o.bounds.world[j*4+j]=1;}o.bounds.world[3]=.05f;
 q.selected(slot);
}
static void publishR93(Fixture& q,const a::Output& o){
 ab::enabled=true;ab::table.publish(q.in.nativeFrame.table);ab::control.publish(o.control);ab::nomination.publish(o.nomination);
 for(int h=0;h<2;++h)ab::tools[h].publish(q.in.tool[h]);
 for(unsigned i=0;i<q.in.nativeFrame.offers.size();++i)ab::offers[i].publish({1,2,3,q.in.nativeFrame.offers[i]});
 kcd2vr::frik_pose_probe::plateFrameR93=q.f;
}
static void tiltR93(Fixture& q){q.f.hand[1].rotation[0]=std::sin(.6f);q.f.hand[1].rotation[3]=std::cos(.6f);}
int main(){
 // Same-step admission publishes the newly selected physical side for all ordinary sources.
 for(auto item:{std::pair{0,0},std::pair{1,0},std::pair{0,1},std::pair{1,5},std::pair{1,14}}){
  const auto [side,type]=item;
  Fixture q;mh::alchemyMode67=mh::SixDofVrik;mh::modeEpoch67=6;
  q.in.nativeFrame.offers[14].valid=false;q.in.nativeFrame.offers[type]={{type,unsigned(100+type)},{0,0,0},q.in.now,true};
  q.in.palm[side].point={0,0,0};q.in.palm[1-side].point={2,0,0};q.grip(side,1);q.step();q.advance();q.selected(type);
  auto o=q.step();check(o.admitted&&o.control.side==side);
 }
 for(int destination:{2,6}){
  Fixture q;beginR93(q);auto o=attachR93(q);
  check(o.admitted&&o.ordinaryHeld&&o.control.held[1]&&!o.control.held[0]);
  check(o.control.side==0&&o.control.plateGestureSideR93==1&&o.control.plateBindingR93);
  check(q.state.admittedSide==1&&q.state.operationParent==999);
  o=idleR93(q);check(o.admitted&&o.ordinaryHeld&&!o.use); // parent end must not retire this LEFT occurrence
  potR93(q,destination);q.step();q.advance();tiltR93(q);o=q.step();
  check(o.use&&o.ordinaryReturnRequested&&q.state.ordinaryReturning&&o.control.side==0);
  check(o.control.plateGestureSideR93==1&&o.nomination.decision.source==n::Source::Left);
  const auto serial=q.state.ordinaryReturnTick;auto repeated=q.step();
  check(repeated.use&&!repeated.ordinaryReturnRequested&&q.state.ordinaryReturnTick==serial);
  q.advance(81);o=q.step();check(!o.use);q.in.action.parent=1001;q.in.nativeFrame.table.action=1001;
  q.step();q.advance();q.in.action.parent=0;q.in.nativeFrame.table.action=0;q.in.tool[0]={};q.in.nativeFrame.table.handType68[0]=21;
  o=q.step();check(!o.admitted&&o.ordinaryReturnObserved&&!o.use);
 }
 // The exact producer nomination can reach the native selector despite native side0 / physical side1.
 {Fixture q;beginR93(q);attachR93(q);idleR93(q);q.in.palmRayD70=true;potR93(q);q.step();q.advance();tiltR93(q);auto o=q.step();
  check(o.nomination.decision.pourRecipientD72);publishR93(q,o);ab::Nomination nout{};
  check(ab::selectionOwner68(q.in.nativeFrame.table,nout,q.in.now)==1&&nout.decision.ref.slot==6);
  int side=-1;check(ab::presentationOwns71(q.in.tool[0].binding,q.in.now,4,nullptr,&side)&&side==1);
  // A current other native tool cannot be displaced by the same plate intent.
  q.in.tool[1].joined=true;q.in.tool[1].tick=q.in.now;q.in.tool[1].side=1;q.in.tool[1].entity=113;
  q.in.tool[1].instance=1;q.in.tool[1].lifetime=3;q.in.tool[1].poseEpoch=4;ab::tools[1].publish(q.in.tool[1]);
  check(!ab::presentationOwns71(q.in.tool[0].binding,q.in.now,4,nullptr,&side));
  check(ab::selectionOwner68(q.in.nativeFrame.table,nout,q.in.now)!=1||nout.decision.ref.slot!=6);
 }
 // Neither a pre-existing plate nor an unobserved pickup action creates a cross-hand lease.
 {Fixture q;beginR93(q,false);auto o=attachR93(q);check(!o.control.plateBindingR93&&!o.ordinaryHeld);}
 {Fixture q;beginR93(q);q.state.operationParent=0;q.in.action.parent=0;auto o=attachR93(q);check(!o.control.plateBindingR93&&!o.ordinaryHeld);}
 // Every native identity/freshness and real left-hand ownership requirement remains required.
 for(int fault=0;fault<18;++fault){Fixture q;beginR93(q);attachR93(q);idleR93(q);q.advance();
  switch(fault){case 0:q.in.tool[0].binding.entity++;break;case 1:q.in.tool[0].binding.attachment++;break;
   case 2:q.in.tool[0].binding.object++;break;case 3:q.in.tool[0].binding.character++;break;
   case 4:q.in.tool[0].binding.lifetime++;break;case 5:q.in.tool[0].joined=false;break;
   case 6:q.in.tool[0].tick=q.in.now-81;break;case 7:q.in.tool[0].poseEpoch++;break;
   case 8:q.in.nativeFrame.table.handType68[1]=13;break;case 9:q.in.nativeFrame.table.refs[0].entity++;break;
   case 10:q.in.nativeFrame.table.handTypesKnown68=false;break;case 11:q.f.hand[1].valid=false;break;
   case 12:q.in.nativeFrame.table.tick=q.in.now-81;break;case 13:mh::alchemyMode67=mh::Native;break;
   case 14:mh::alchemyMode67=mh::SemiPassenger;break;case 15:q.in.tool[0].binding.action=999;break;
   case 16:q.in.tool[0].binding.side=1;break;case 17:q.in.modeEpoch++;break;}
  auto o=q.step();check(!o.control.held[1]&&!o.ordinaryHeld&&!o.ordinaryReturnRequested);
 }
 std::printf("plate_hand_mapping_r93: %u checks PASS (actual State::step + bridge selector)\n",checks);
}
