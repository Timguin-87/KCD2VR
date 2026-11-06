// Connected producer/consumer regression: actual State::step and selectionOwner68.
// Native observations are fixture inputs; no native semantic completion is faked.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
#include <cstring>
#include <utility>

namespace b75=kcd2vr::subsystems::alchemy::bridge67;
namespace kcd2vr::frik_pose_probe {
static workstation_sample46::Frame returnFrame75{};
bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept {out=returnFrame75;return true;}
}
static unsigned checks75=0,failures75=0;
static void expect75(bool ok,const char* message){++checks75;if(!ok){++failures75;std::fprintf(stderr,"D75 FAIL %s\n",message);}}
static void bounds75(Fixture& q,int type){
 auto& o=q.in.nativeFrame.offers[type];o.boundsValid=true;
 for(int j=0;j<3;++j){o.bounds.local[j]=-.01f;o.bounds.local[j+3]=.01f;o.bounds.world[j*4+j]=1;}
 o.bounds.world[3]=.08f;
}
static void attached75(Fixture& q,int type,int side){
 q.tool(side,type);q.in.tool[side].binding={1,2,888,std::uintptr_t(700+side),unsigned(100+type),side,type,0,0,std::uintptr_t(800+side),3};
 q.in.nativeFrame.table.handTypesKnown68=true;q.in.nativeFrame.table.handType68[side]=unsigned(type);
}
static void setup75(Fixture& q,int type,int side,bool attached=true){
 q.in.proximityCraft1=q.in.handsOnlyD53=true;q.in.context.tableGeneration=3;
 q.in.nativeFrame.table.actionKnown=true;q.in.nativeFrame.offers[6].valid=false;
 q.admit(type,side);bounds75(q,type);
 if(attached)attached75(q,type,side);
 q.step();q.advance(90);q.step();q.advance();q.in.nativeSelectedKnown=false;
 // Returning a held source must not require the acquisition ray to point at
 // that source; it can lie beside a turned palm while still within reach.
 q.in.palmRayD70=true;
 q.in.aimH21[side]={{-1,0,0},q.in.now,q.f.poseEpoch,q.f.sourceEpoch,q.in.modeEpoch,side,true};
}
static bool consume75(Fixture& q,const a::Output& out,unsigned expected){
 b75::enabled.store(true);kcd2vr::minigame_hands45::alchemyMode67.store(kcd2vr::minigame_hands45::SixDofVrik);
 kcd2vr::minigame_hands45::modeEpoch67.store(q.in.modeEpoch);kcd2vr::tracking59c6::epoch.store(q.f.sourceEpoch);
 kcd2vr::frik_pose_probe::returnFrame75=q.f;
 b75::table.publish(q.in.nativeFrame.table);b75::nomination.publish(out.nomination);
 for(int h=0;h<2;++h)b75::tools[h].publish(q.in.tool[h]);
 for(unsigned i=0;i<n::Capacity;++i)b75::offers[i].publish({q.in.context.instance,q.in.context.player,q.in.context.lifetime,q.in.nativeFrame.offers[i]});
 unsigned entity=0;return b75::selectionNear68(q.in.nativeFrame.table,entity,q.in.now)&&entity==expected;
}
static void ordinary75(int type,int side){
 Fixture q;setup75(q,type,side);q.grip(side,0);auto out=q.step();
 expect75(q.state.ordinaryReleasePending&&!out.use,"release waits for native selection");
 expect75(out.nomination.decision.ref.slot==type&&out.nomination.decision.selfReturnRecipientD74,"held tool source stays nominated after open Grip");
 expect75(consume75(q,out,unsigned(100+type)),"consumer accepts exact current ordinary source return");
 q.advance();q.selected(6);out=q.step();expect75(!out.use,"wrong native acknowledgement cannot dispatch return");
 q.advance();q.selected(type);out=q.step();expect75(out.use&&out.ordinaryReturnRequested,"matching new native selection emits one Return press");
 expect75(out.nomination.decision.ref.slot==type&&out.nomination.decision.selfReturnRecipientD74,"accepted return target persists through input pulse");
 out=q.step();expect75(out.use&&!out.ordinaryReturnRequested,"same XR callback does not create another Return request");
 q.advance(81);out=q.step();expect75(!out.use,"Return input pulse ends");
 q.advance(300);out=q.step();expect75(!out.use&&!out.ordinaryReturnRequested,"refused Return is never automatically retried");
}
static void delayedAttach75(){
 Fixture q;setup75(q,0,0,false);q.grip(0,0);q.step();q.advance();attached75(q,0,0);auto out=q.step();
 expect75(!out.use&&out.nomination.decision.ref.slot==0&&out.nomination.decision.selfReturnRecipientD74,"release before delayed Attach promotes same proven source once attachment arrives");
 q.advance();q.selected(0);out=q.step();expect75(out.use&&out.ordinaryReturnRequested,"delayed Attach returns without requiring another squeeze");
}
static void nativeWait75(){
 Fixture q;setup75(q,5,1);q.in.action.parent=q.in.nativeFrame.table.action=900;q.step();
 q.grip(1,0);auto out=q.step();expect75(!out.use&&out.transferStateD59==2,"source release waits for its still-running Take action");
 q.advance(40);q.selected(5);out=q.step();expect75(!out.use&&out.nomination.decision.ref.slot==5,"native selection does not spend the pulse while director is busy");
 q.advance();q.in.action.parent=q.in.nativeFrame.table.action=0;
 q.in.nativeSelectionTick=q.state.ordinaryIntentTickD59-1;out=q.step();expect75(!out.use,"old selection timestamp cannot acknowledge a new release");
 q.advance();q.in.nativeSelectionTick=q.in.now;out=q.step();expect75(out.use&&out.ordinaryReturnRequested,"idle native director and fresh exact selection dispatch once");
 q.advance();q.in.action.parent=q.in.nativeFrame.table.action=901;out=q.step();
 expect75(q.state.ordinaryReturnParent==901&&out.passenger&&!out.nomination.decision.selfReturnRecipientD74,"accepted native Return owns completion and ends source nomination");
 q.advance();q.in.action.parent=q.in.nativeFrame.table.action=0;out=q.step();
 expect75(!out.admitted&&out.ordinaryReturnObserved&&!out.use,"actual native Return end retires admission without another press");
}
static void cancellations75(){
 for(int fault=0;fault<12;++fault){
  Fixture q;setup75(q,5,1);q.grip(1,0);auto out=q.step();q.advance();
  switch(fault){
   case 0:q.grip(1,1);break;
   case 1:q.in.tool[1].tick=q.in.now-81;break;
   case 2:++q.in.tool[1].binding.player;break;
   case 3:++q.in.tool[1].binding.character;break;
   case 4:++q.in.tool[1].binding.instance;break;
   case 5:++q.in.tool[1].binding.lifetime;break;
   case 6:++q.in.tool[1].binding.entity;break;
   case 7:q.in.tool[1].binding.action=999;break;
   case 8:q.in.nativeFrame.table.handType68[1]=21;break;
   case 9:q.in.tool[1].binding.object=0;break;
   case 10:q.in.palm[1].point={1,0,0};break;
   case 11:++q.in.context.lifetime;break;
  }
  q.in.nativeSelectedKnown=false;out=q.step();
  expect75(!out.use&&!out.ordinaryReturnRequested&&!out.nomination.decision.selfReturnRecipientD74,"stale/foreign binding, regrip, distance or context change revokes source-return exception");
 }
}
static void currentConsumer75(){
 for(int fault=0;fault<15;++fault){
  Fixture q;setup75(q,5,1);q.grip(1,0);auto out=q.step();
  // Supply the producer's valid proposal even to the unmodified D74 consumer;
  // the independent current ownership check is the subject of these cases.
  auto& d=out.nomination.decision;d.source=n::Source::Left;d.ref=q.in.nativeFrame.table.refs[5];d.palm=q.in.palm[1];
  d.table=q.in.nativeFrame.table.refs;d.proximityCraft1=d.handsOnlyD53=d.selfReturnRecipientD74=true;
  d.sourceAcquisitionD39=d.gripSelectionOwnedCraft1=false;d.heldSourceH21=d.ref;
  switch(fault){
   case 1:q.f.hand[1].grip=1;break;
   case 2:q.in.tool[1].tick=q.in.now-81;break;
   case 3:++q.in.tool[1].binding.player;break;
   case 4:++q.in.tool[1].binding.character;break;
   case 5:++q.in.tool[1].binding.instance;break;
   case 6:++q.in.tool[1].binding.lifetime;break;
   case 7:++q.in.tool[1].binding.entity;break;
   case 8:q.in.tool[1].binding.action=999;break;
   case 9:q.in.nativeFrame.table.handType68[1]=21;break;
   case 10:q.in.tool[1].binding.object=0;break;
   case 11:q.in.tool[1].binding.attachment=0;break;
   case 12:q.in.tool[1].binding.side=0;break;
   case 13:q.in.tool[1].binding.type=0;break;
   case 14:d.sourceAcquisitionD39=true;break;
  }
  expect75(consume75(q,out,105)==(fault==0),"consumer independently checks current native source binding and open raw Grip");
 }
 // These sources require their specialized native work/fixture paths even if
 // a forged ordinary binding and current hand-type snapshot are presented.
 for(int type:{2,3,4,13,14,15,16,18}){
  Fixture q;setup75(q,5,1);q.grip(1,0);auto out=q.step();attached75(q,type,1);bounds75(q,type);
  auto& offer=q.in.nativeFrame.offers[type];offer.ref={type,unsigned(100+type)};offer.valid=true;offer.tick=q.in.now;
  auto& d=out.nomination.decision;d.ref=d.heldSourceH21=q.in.nativeFrame.table.refs[type];
  d.selfReturnRecipientD74=true;d.source=n::Source::Left;d.palm=q.in.palm[1];
  expect75(!consume75(q,out,unsigned(100+type)),"specialized mortar/resource/retort/bellows source never borrows ordinary return");
 }
}
int main(){
 for(auto item:{std::pair<int,int>{0,0},{0,1},{1,0},{5,0},{5,1},{19,0},{19,1}})ordinary75(item.first,item.second);
 delayedAttach75();nativeWait75();cancellations75();currentConsumer75();
 std::printf("D75 ordinary source return: %u checks, %u failures; actual input/bridge headers, native observations simulated\n",checks75,failures75);
 return failures75?1:0;
}
