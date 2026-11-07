// Actual production holster producer, explicit host/native leaves, no game calls.
#include "../src/diagnostics/interaction_cycle_h4.h"
#include "../src/frik_pose_probe.cpp"
#include <cstdio>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::frik_pose_probe;
static unsigned checks=0,failures=0;
static void check(bool ok,const char* name){++checks;if(!ok){++failures;std::printf("FAIL %s\n",name);}}
struct RigH39 {
 int side;PhysEquipment6698 gear{};ph::Input in{};ph::Output out{};
 std::array<ph::HolsterState,8> states{};ph::Vec3 mount{};unsigned draws=0,stows=0;
 explicit RigH39(int hand):side(hand){
  g_pose_epoch6689=1;g_weapon_request_epoch6694=1;g_gameplay_allowed=true;
  g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=false;
  gear.valid=true;gear.epoch=gear.request=1;gear.player=reinterpret_cast<void*>(123);gear.count=1;gear.selected=1;
  gear.handKnown[0]=gear.handKnown[1]=true;
  auto& item=gear.items[0];item.profile.side=side;item.profile.classId=side?9:7;
  item.profile.key=901;item.profile.generation=2;item.set=1;item.slot126=0;
  in.epoch=in.bodyEpoch=1;in.tick=1000;in.sample=1;in.requirePrimaryGrip114=in.allowed=true;
  in.tracked[0]=in.tracked[1]=in.intentTracked[0]=in.intentTracked[1]=true;
  in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};
  ph::holster_mount126(in,item.profile,mount);in.hands[side].p=mount;
 }
 void step(unsigned talk=0){
  in.tick+=20;++in.sample;hostTickCount64=in.tick;gear.tick=in.tick;
  g_phys_equipment6698.publish(gear);PublishQuickTalkOwnerH4(talk,in.tick);
  out={};PhysHolsterGestures6698(in,gear,out,states);
  for(unsigned i=0;i<out.commandCount;++i){draws+=out.commands[i].kind==ph::CommandKind::Draw;stows+=out.commands[i].kind==ph::CommandKind::Holster;}
 }
 void pull(double grip=1,double trigger=0){step();in.grip[side]=grip;in.trigger[side]=trigger;step();in.hands[side].p.y+=.45;for(int n=0;n<4;++n)step();}
};
int main(){
 for(int side=0;side<2;++side){
  {RigH39 r(side);r.in.grip[1-side]=1;r.pull();check(r.draws==1,"opposite Grip alone does not block actual own-hand draw");}
  {RigH39 r(side);r.in.grip[1-side]=r.in.trigger[1-side]=1;r.pull();check(r.draws==1,"opposite qualified fist preserves own Grip-only draw");}
  {RigH39 r(side);r.pull(1,1);check(!r.draws&&!r.states[0].comparison114.latched,"single qualified fist never acquires its own holstered item");}
  {RigH39 r(side);r.in.grip[1-side]=r.in.trigger[1-side]=1;r.pull(1,1);check(!r.draws,"two qualified fists cannot draw equipment");}
  {RigH39 r(side);r.pull(1,double(.60f));check(r.draws==1,"Trigger exactly sixty percent is not a qualified fist");}
  {RigH39 r(side);r.pull(double(.60f),1);check(r.draws==1,"Grip exactly sixty percent is not a qualified fist");}
  {RigH39 r(side);r.pull(.6001,.6001);check(!r.draws,"both controls above sixty percent reserve this hand");}
  {RigH39 r(side);r.step();r.in.grip[side]=1;r.step();check(r.states[0].comparison114.latched,"ordinary Grip acquisition begins before fist");
   r.in.trigger[side]=1;r.step();check(!r.states[0].comparison114.latched,"fist cancels actual in-flight acquisition latch");
   r.in.trigger[side]=0;r.in.hands[side].p.y+=.45;for(int n=0;n<4;++n)r.step();check(!r.draws,"Trigger release alone cannot turn held Grip into delayed draw");
   r.in.hands[side].p=r.mount;r.in.grip[side]=0;r.pull();check(r.draws==1,"real neutral Grip then fresh pull rearms after combat ownership");}
  {RigH39 r(side);auto& item=r.gear.items[0];item.held=true;r.gear.handKey[side]=item.profile.key;
   r.in.grip[side]=r.in.trigger[side]=1;r.step();r.in.grip[side]=0;r.step();check(r.stows==1,"held weapon still holsters normally despite Trigger held");}
  {RigH39 r(side);r.step();r.in.grip[side]=1;r.step(1u<<side);r.in.hands[side].p.y+=.45;for(int n=0;n<4;++n)r.step(1u<<side);check(!r.draws,"same-hand quick talk retains utility ownership");}
  {RigH39 r(side);r.gear.handKnown[side]=false;r.pull();check(!r.draws,"unknown equipment hand does not acquire");}
  {RigH39 r(side);r.in.intentTracked[side]=false;r.pull();check(!r.draws,"lost original intent tracking does not acquire");}
  {RigH39 r(side);r.pull();const auto sent=r.states[0].comparison114.sent;
   check(r.draws==1&&r.states[0].comparison114.pending,"fixture emitted original draw awaiting native acknowledgement");
   r.in.trigger[side]=1;for(int n=0;n<10;++n)r.step();
   check(r.states[0].comparison114.pending&&r.states[0].comparison114.sent==sent,"closing fist retains issued draw and original occurrence time");
   check(kcd2vr::equipment114::PendingDrawBlocks(r.states[0].comparison114,901,2,r.in.epoch,r.in.tick,false),"fist keeps original cross-item pending exclusion fresh past hundred milliseconds");
   r.in.trigger[side]=0;r.in.grip[side]=0;r.in.hands[side].p=r.mount;r.pull();check(r.draws==1,"fresh Grip cannot repeat native-unacknowledged draw");
   r.gear.items[0].held=true;r.gear.handKey[side]=901;r.step();check(!r.states[0].comparison114.pending,"actual current held acknowledgement completes original draw normally");}
  {RigH39 r(side);r.pull();const auto sent=r.states[0].comparison114.sent;r.in.trigger[side]=1;
   while(r.in.tick<sent+1980)r.step();check(r.states[0].comparison114.pending,"fist observation retains original request until existing timeout");
   r.step();check(!r.states[0].comparison114.pending&&r.draws==1,"existing timeout expires without repeating a draw or changing native state");}
 }
 std::printf("H39 actual holster producer %u/%u; native_calls=NONE\n",checks-failures,checks);
 return failures?1:0;
}
