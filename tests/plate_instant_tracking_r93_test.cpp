// Current generated native adapter: only four native-call leaves are mocked.
#define main mortarAdapterOriginalMainR93
#include "instant_mortar_d75_test.cpp"
#undef main
static AlchemyInstantOwnerD72 plateInstantR93(){
 reset(0);put(instance+0x150,100u);put(instance+0x304,0);
 AlchemyInstantOwnerD72 s{};ab72::control.read(s.input);auto& c=s.input;
 c.passenger=false;c.type=0;c.side=0;c.entity=100;c.heldMortarPourD75=false;c.held[1]=true;
 c.plateBindingR93={instance,player,888,777,100,0,0,0,0,666,3};c.plateGestureSideR93=1;
 ab72::control.publish(c);auto& r=s.receipt;r.instance=instance;r.parent=parent;r.lifetime=3;r.admission=c.admissionTickR22;
 r.poseEpoch=c.poseEpoch;r.sourceEpoch=c.sourceEpoch;r.modeEpoch=c.modeEpoch;r.source=0;r.side=0;r.action=ai72::Action::ReturnObject;
 s.selected=0;
 ab72::NativeTable t{};t.instance=instance;t.player=player;t.playerEntity=55;t.lifetime=3;t.tick=now;t.refs[0]={0,100};t.handTypesKnown68=true;t.handType68={0,21};ab72::table.publish(t);
 ab72::NativeTool held{};held.instance=instance;held.character=888;held.lifetime=3;held.tick=now;held.poseEpoch=4;
 held.entity=100;held.side=0;held.type=0;held.joined=true;held.binding=c.plateBindingR93;ab72::tools[0].publish(held);ab72::tools[1].publish({});
 auto& f=kcd2vr::frik_pose_probe::hostFrame;f.hand[0].valid=false;f.hand[1].valid=true;
 return s;
}
int main(){
 auto s=plateInstantR93();check(AlchemyInstantSourceD72(s,now));
 auto& f=kcd2vr::frik_pose_probe::hostFrame;f.hand[1].valid=false;f.hand[0].valid=true;check(!AlchemyInstantSourceD72(s,now));
 s=plateInstantR93();ab72::Control c{};ab72::control.read(c);c.plateBindingR93.object++;ab72::control.publish(c);check(!AlchemyInstantSourceD72(s,now));
 s=plateInstantR93();ab72::NativeTable t{};ab72::table.read(t);t.handType68[1]=13;ab72::table.publish(t);check(!AlchemyInstantSourceD72(s,now));
 s=plateInstantR93();s.semanticReturnedD80=true;s.semanticTickD80=now;
 // A completed semantic Return may have already detached; its pending observation follows original LEFT tracking.
 ab72::tools[0].publish({});check(AlchemyAttachmentPendingCurrentD80(s,now));
 f.hand[0].valid=true;f.hand[1].valid=false;check(!AlchemyAttachmentPendingCurrentD80(s,now));
 s=plateInstantR93();s.semanticReturnedD80=true;s.semanticTickD80=now;check(!AlchemyAttachmentPendingCurrentD80(s,now+126));
 std::printf("plate_instant_tracking_r93: %u checks PASS (native side0; actual left controller validation)\n",checks);
}
