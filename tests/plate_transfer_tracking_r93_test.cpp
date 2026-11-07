#include "../src/vrik/static_grab_r89.h"
#define KCD2VR_ALCHEMY_TRANSFER_D65_NO_MAIN
#include "alchemy_transfer_runtime_d65_test.cpp"
int main(){
 using namespace kcd2vr::action_time_owner59c11;
 namespace ab=kcd2vr::subsystems::alchemy::bridge67;
 transferSetup65();ab::Control c{};ab::control.read(c);const auto tick=hostTickCount64;
 c.side=0;c.type=0;c.entity=100;c.admissionTickR22=tick-1;c.plateGestureSideR93=1;
 c.plateBindingR93={c.instance,2,888,777,100,0,0,0,0,666,c.lifetime};ab::control.publish(c);
 ab::NativeTable t{};t.instance=c.instance;t.player=2;t.playerEntity=55;t.lifetime=c.lifetime;t.tick=tick;
 t.refs[0]={0,100};t.handTypesKnown68=true;t.handType68={0,21};ab::table.publish(t);
 ab::NativeTool held{};held.instance=c.instance;held.character=888;held.lifetime=c.lifetime;held.tick=tick;held.poseEpoch=c.poseEpoch;
 held.entity=100;held.side=0;held.type=0;held.joined=true;held.binding=c.plateBindingR93;ab::tools[0].publish(held);ab::tools[1].publish({});
 transfer65::Key key{};key.instance=c.instance;key.lifetime=c.lifetime;key.parent=c.parent;key.mode=c.modeEpoch;
 testWorkstationFrame46.hand[0].valid=false;testWorkstationFrame46.hand[1].valid=true;
 ck65(AlchemyTransferInputD65(key,tick),"mapped plate transfer follows LEFT tracking, not native semantic side0");
 testWorkstationFrame46.hand[0].valid=true;testWorkstationFrame46.hand[1].valid=false;
 ck65(!AlchemyTransferInputD65(key,tick),"right tracking cannot substitute for mapped LEFT controller");
 testWorkstationFrame46.hand[1].valid=true;t.handType68[1]=13;ab::table.publish(t);
 ck65(!AlchemyTransferInputD65(key,tick),"other native hand conflict rejects transfer ownership");
 t.handType68[1]=21;ab::table.publish(t);held.binding.object++;ab::tools[0].publish(held);
 ck65(!AlchemyTransferInputD65(key,tick),"replacement native binding rejects transfer ownership");
 std::printf("plate_transfer_tracking_r93: %u checks PASS (actual full owner TU; native reads/calls mocked)\n",checks65);
}
