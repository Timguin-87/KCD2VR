// Actual alchemy producer: withdrawing a still-unentered held tilt is not a
// native placement, Stop, Detach, inventory effect, or new acquisition.
#define KCD2VR_MORTAR69_NO_MAIN
#include "subsystems/alchemy/mortar_input69_test.cpp"
#include "vrik/semi_workstations46.h"
#include <cstdlib>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace m=kcd2vr::subsystems::alchemy::mortar69;
static unsigned r2Checks=0;
static void requireR2(bool value,const char* why){++r2Checks;if(!value){std::fprintf(stderr,"FAIL: %s\n",why);std::exit(1);}}
static void beginHeldR2(Fixture& q){
 pair69(q);q.advance();q.in.palm[1].point={.5,0,0};q.in.nativeFrame.offers[6].point={.52,0,0};q.selected(6);q.step();
 q.advance();q.f.hand[1].rotation[0]=std::sin(.6f);q.f.hand[1].rotation[3]=std::cos(.6f);
 const auto out=q.step();requireR2(out.mortarStop69.serial&&!out.use&&q.state.mortarStopping69&&!q.state.mortarReleaseIntent70,"held tilt creates one native stop request");
}
static void withdrawR2(Fixture& q){
 q.advance();q.in.palm[1].point={2,0,0};q.in.nativeSelectedKnown=false;
 const auto out=q.step();requireR2(out.mortarStop69.withdrawTickD78R2==q.in.now,"leaving recipient withdraws the original unspent tilt");
 requireR2(!out.use&&out.semiControl.manualUse&&q.state.mortarStopping69,"withdrawal waits for native no-entry acknowledgment");
}
int main(){
 {
  Fixture q;beginHeldR2(q);const auto original=q.state.mortarRequest69;withdrawR2(q);
  requireR2(m::sameRequestD78R2(original,q.state.mortarRequest69),"withdrawal preserves request, pair, parent and generation identity");
  const auto withdraw=q.state.mortarRequest69.withdrawTickD78R2;q.advance();auto out=q.step();
  requireR2(out.mortarStop69.withdrawTickD78R2==withdraw&&!out.use,"withdrawal is one sticky message, not a retry every frame");
  q.in.mortarResult69={original.serial,q.in.now,1,3,m::Status::WithdrawnD78R2,m::StopEntry72::NotEntered};out=q.step();
  requireR2(q.state.admitted&&q.state.mortarPair69&&!q.state.mortarStopping69&&!q.state.mortarAttempted69,"native no-entry acknowledgment releases only the waiting reservation");
  requireR2(!out.use&&!out.mortarStop69.serial&&!out.mortarPlaced69&&!q.state.mortarReturning69,"withdrawal is not native return or contents transfer");
  requireR2(!out.semiControl.manualUse&&out.control.held[0]&&out.control.held[1],"same actual tracked pair resumes without another pickup");
  q.in.palm[1].point={.5,0,0};q.selected(6);out=q.step();
  requireR2(out.mortarStop69.serial>original.serial,"a later live intent cannot reuse an already retired serial");
 }
 // Upright wrist or an opened hold also ends only a pending held-tilt intent.
 for(int kind=0;kind<2;++kind){Fixture q;beginHeldR2(q);q.advance();
  if(kind==0){q.f.hand[1].rotation[0]=0;q.f.hand[1].rotation[3]=1;}else q.grip(1,0);
  const auto out=q.step();requireR2(out.mortarStop69.withdrawTickD78R2&&!out.use,"no-longer-held tilt is withdrawn before native entry");
 }
 {
  Fixture q;beginHeldR2(q);q.advance();q.in.nativeSelectedKnown=false;
  const auto out=q.step();requireR2(q.state.mortarStopping69&&!out.mortarStop69.withdrawTickD78R2&&!out.use,"temporary native selection loss cannot withdraw a physically current held tilt");
 }
 // A release placement is already an explicit committed request; loss of
 // hover must not silently reinterpret it as an unspent held tilt.
 {
  Fixture q;pair69(q);q.advance();q.in.palm[1].point={.5,0,0};q.in.nativeFrame.offers[6].point={.52,0,0};q.selected(6);q.step();
  q.advance();q.grip(1,0);auto out=q.step();requireR2(q.state.mortarReleaseIntent70&&out.mortarStop69.serial,"release request established");
  q.advance();q.in.palm[1].point={2,0,0};q.in.nativeSelectedKnown=false;out=q.step();
  requireR2(!out.mortarStop69.withdrawTickD78R2&&q.state.mortarStopping69&&!out.use,"release placement retains its original lifecycle");
 }
 // Old, foreign, future, unknown and entered receipts cannot give an unspent
 // acknowledgment. An entered native action keeps the old safe completion.
 for(int fault=0;fault<8;++fault){Fixture q;beginHeldR2(q);withdrawR2(q);q.advance();
  auto& r=q.in.mortarResult69;r={q.state.mortarRequest69.serial,q.in.now,1,3,m::Status::WithdrawnD78R2,m::StopEntry72::NotEntered};
  switch(fault){case 0:++r.serial;break;case 1:++r.instance;break;case 2:++r.lifetime;break;case 3:r.tick=q.state.mortarRequest69.withdrawTickD78R2-1;break;
   case 4:r.tick=q.in.now+1;break;case 5:r.stopEntry72=m::StopEntry72::Entered;break;case 6:r.stopEntry72=m::StopEntry72::Unknown;break;case 7:r.status=m::Status::ReadyForNativePlace;break;}
  const auto out=q.step();requireR2(q.state.mortarStopping69&&!out.use&&!out.mortarPlaced69,"foreign or spent acknowledgment cannot resume an unspent tilt");
 }
 // Execute the actual Mod-facing pad adapter: a same-serial withdrawal must
 // reach the native mailbox once even though the original Stop was sent.
 {
  Fixture q;q.in.context.tableGeneration=3;q.in.proximityCraft1=true;beginHeldR2(q);withdrawR2(q);
  using namespace kcd2vr;
  semi_workstations46::State owner;owner.alchemy67=q.state;owner.mortarSent69=q.state.mortarRequest69.serial;
  semi_workstations46::Context context{};context.enabled=context.qualified=context.alchemyTracked=context.gestureAlchemy67=true;
  context.alchemy=1;context.lifetime=3;
  for(int h=0;h<2;++h){context.world67[h]=q.in.world[h];context.world67[h].point[0]=float(q.in.palm[h].point.x);context.world67[h].point[1]=float(q.in.palm[h].point.y);context.world67[h].point[2]=float(q.in.palm[h].point.z);context.worldPlayer67[h]=9;
   subsystems::alchemy::bridge67::tools[h].publish(q.in.tool[h]);}
  q.in.nativeFrame.table.actionKnown=true;q.in.nativeFrame.table.action=q.in.action.parent;
  subsystems::alchemy::bridge67::completed.publish(q.in.nativeFrame);alchemy_semi46::work.publish(q.in.work);
  minigame_hands45::alchemyMode67=minigame_hands45::Gesture;minigame_hands45::modeEpoch67=q.in.modeEpoch;
  alchemy_input19c4::clear();m::result.publish({});m::request.publish({});m::interested=false;
  struct Pad{bool a=false,b=false,x=false,lb=false,rb=false;float lx=0,ly=0,lt=0,rt=0;}pad;
  owner.apply(context,q.f,q.in.now,pad);m::Request sent{};
  requireR2(m::request.read(sent)&&m::sameRequestD78R2(sent,q.state.mortarRequest69)&&sent.withdrawTickD78R2,"connected pad publisher forwards withdrawal of the same serial");
  requireR2(m::interested.load()&&!pad.a,"withdrawal wakes only existing native service, not contextual A");
  auto sentinel=sent;sentinel.serial++;m::request.publish(sentinel);owner.apply(context,q.f,q.in.now,pad);m::request.read(sent);
  requireR2(sent.serial==sentinel.serial,"unchanged withdrawal is not republished over a newer mailbox owner");
 }
 std::printf("D78R2 mortar producer: %u assertions PASS\n",r2Checks);
}
