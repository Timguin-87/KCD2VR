#define KCD2VR_NATIVE_MORTAR69_NO_MAIN
#include "subsystems/alchemy/native_mortar69_test.cpp"
int main(){
 prepare69();nativeClockR96=1000;frameReadClockR96=1001;kcd2vr::frik_pose_probe::frame.tick=1001;
 actualMortarTickR96(memory_controller.data(),1000);
 ck(cancels==1&&result69().status==mortar69::Status::ReadyForNativePlace,"post-read clock permits the actual native Stop and retained pair");
 prepare69();kcd2vr::frik_pose_probe::frame.tick=1001;tick69();
 ck(!cancels&&result69().status==mortar69::Status::Revoked,"future copied pose still refuses native entry");
 prepare69();kcd2vr::frik_pose_probe::frame.tick=919;tick69();
 ck(!cancels&&result69().status==mortar69::Status::Revoked,"stale copied pose still refuses native entry");
 prepare69();kcd2vr::frik_pose_probe::sourceOk=false;tick69();
 ck(!cancels&&g_mortar_stop69.active&&!g_mortar_stop69.attempted,"missing publication retains original unentered request only");
 TickMortarStop69(memory_controller.data(),1040);
 ck(!cancels&&g_mortar_stop69.sourceTickR96==1000,"gap cannot renew the request source");
 kcd2vr::frik_pose_probe::sourceOk=true;kcd2vr::frik_pose_probe::frame.tick=1040;
 TickMortarStop69(memory_controller.data(),1040);
 ck(cancels==1&&result69().status==mortar69::Status::ReadyForNativePlace,"recovered same request enters native Stop exactly once");
 TickMortarStop69(memory_controller.data(),1041);ck(cancels==1,"terminal request never replays");
 // Repeated station visit without resetting ServiceState/retired serial. New
 // native lifetime/parent/pair observation, new explicit request, actual Stop.
 const auto firstSerial=g_mortar_retired69;
 slots[1]=req69.parent;req69.serial++;req69.tick=1060;req69.left.lifetime++;req69.right.lifetime++;
 kcd2vr::activity_input163::witness175.lifetime175=req69.left.lifetime;
 kcd2vr::frik_pose_probe::frame.tick=1060;mortar69::request.publish(req69);mortar69::interested=true;
 TickMortarStop69(memory_controller.data(),1060);
 ck(cancels==2&&g_mortar_retired69>firstSerial&&result69().status==mortar69::Status::ReadyForNativePlace,"fresh station lifetime performs a second real Stop without process/owner reset");
 prepare69();kcd2vr::frik_pose_probe::sourceOk=false;tick69();TickMortarStop69(memory_controller.data(),1081);
 ck(!cancels&&result69().status==mortar69::Status::Revoked,"missing source cannot hold a request beyond original age");
 prepare69();put(memory_parent.data(),0x70,3);tick69();kcd2vr::activity_input163::witness175.lifetime175++;
 TickMortarStop69(memory_controller.data(),1020);ck(!cancels&&result69().status==mortar69::Status::Revoked,"positive station replacement retires old unspent request");
 prepare69();cancelEffectH21=[](){kcd2vr::frik_pose_probe::sourceOk=false;};tick69();
 ck(cancels==1&&g_mortar_stop69.active&&g_mortar_stop69.attempted,"post-Stop observation gap retains entered debt without cancellation replay");
 cancelEffectH21=nullptr;kcd2vr::frik_pose_probe::sourceOk=true;kcd2vr::frik_pose_probe::frame.tick=1020;
 TickMortarStop69(memory_controller.data(),1020);
 ck(cancels==1&&result69().status==mortar69::Status::ReadyForNativePlace&&result69().stopEntry72==mortar69::StopEntry72::Entered,"native parent retirement completes after visibility recovers, without another Stop");
 ck(!detaches&&!homes,"mortar readiness never fakes contents transfer or attachment destruction");
 prepare69();put(memory_parent.data(),0x70,3);put(memory_parent.data(),0x90,req69.child);tick69();
 kcd2vr::frik_pose_probe::frame.tick=7500;put(memory_child.data(),kActActiveTime,1.f);
 TickMortarStop69(memory_controller.data(),7500);ck(!cancels&&result69().status==mortar69::Status::WaitingNativeCycle,"long native cycle remains live only with actual child progress");
 put(memory_parent.data(),0x70,2);TickMortarStop69(memory_controller.data(),7500);
 ck(cancels==1&&result69().status==mortar69::Status::ReadyForNativePlace,"long progressing cycle reaches actual Stop after native READY");
 prepare69();put(memory_parent.data(),0x70,3);put(memory_parent.data(),0x90,req69.child);tick69();
 kcd2vr::frik_pose_probe::frame.tick=7500;TickMortarStop69(memory_controller.data(),7500);
 ck(!cancels&&result69().status==mortar69::Status::Revoked,"stalled native child is not made live by clock-only renewal");
 std::printf("R96 mortar production Stop recovery: %u checks PASS; native cancel/memory leaves substituted\n",checks);
}
