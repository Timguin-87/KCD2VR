// Actual native binding reader + actual resource/local-stop adapters. Only
// native cancel and memory/tracking providers are mocked, never state logic.
#define main resourceReturn67Main
#include "native_return67_test.cpp"
#undef main
#include "../../../src/subsystems/alchemy/mortar_handoff69.h"
#ifndef KCD2VR_NATIVE_MORTAR69_HEADER
#define KCD2VR_NATIVE_MORTAR69_HEADER "../../../src/subsystems/alchemy/native_mortar69.inl"
#endif
#include KCD2VR_NATIVE_MORTAR69_HEADER
static constexpr auto actualMortarTickR96=&TickMortarStop69;
static void MortarTickAtR96(void* ctrl,std::uint64_t at) noexcept {nativeClockR96=at;actualMortarTickR96(ctrl,at);}
#define TickMortarStop69 MortarTickAtR96
static std::array<unsigned char,0x1000> rightAtt{},rightObj{};
static const char rightName[]="entity_right";
static mortar69::Request req69;
static void prepare69(){
 prepare();g_mortar_stop69={};g_mortar_retired69=0;
 rightAtt.fill(0);rightObj.fill(0);regions.push_back({rightAtt.data(),rightAtt.size()});regions.push_back({rightObj.data(),rightObj.size()});regions.push_back({rightName,sizeof(rightName)});
 const auto b=g_base.load(),instance=reinterpret_cast<std::uintptr_t>(memory_instance.data());
 put(memory_instance.data(),0x2A0,std::uint32_t(2));put(memory_instance.data(),0x2A4,std::uint32_t(21));put(memory_instance.data(),0x300,(unsigned char)2);
 put(memory_instance.data(),0x158,std::uint32_t(500));put(memory_instance.data(),0x15C,std::uint32_t(501));
 put(memory_parent.data(),0,b+0x3F60770);put(memory_parent.data(),0x70,2);
 put(rightAtt.data(),0,b+0x3A657C0);put(rightAtt.data(),0x28,reinterpret_cast<std::uintptr_t>(memory_attachmentManager.data()));
 put(rightAtt.data(),0x10,reinterpret_cast<std::uintptr_t>(rightName));put(rightAtt.data(),0x20,reinterpret_cast<std::uintptr_t>(rightObj.data()));
 put(rightObj.data(),0,b+0x3A68F68);put(rightObj.data(),8,std::uint32_t(501));
 put(memory_scope.data(),kScopeAction,std::uintptr_t(0));g_semi_alchemy46.written=false;
 req69={};req69.left=request67.binding;req69.left.type=2;req69.left.actionClass=b+0x3F60770;
 req69.right=req69.left;req69.right.side=0;req69.right.type=3;req69.right.entity=501;req69.right.attachment=reinterpret_cast<std::uintptr_t>(rightAtt.data());req69.right.object=reinterpret_cast<std::uintptr_t>(rightObj.data());
 req69.parent=req69.left.action;req69.child=request67.child;req69.serial=201;req69.tick=1000;req69.poseEpoch=20;req69.sourceEpoch=30;req69.modeEpoch=1;
 kcd2vr::frik_pose_probe::frame.hand[0].valid=true;kcd2vr::frik_pose_probe::frame.hand[1].valid=true;
 mortar69::request.publish(req69);mortar69::result.publish({});mortar69::interested=true;
}
static mortar69::Result result69(){mortar69::Result r{};mortar69::result.read(r);return r;}
static void tick69(){TickMortarStop69(memory_controller.data(),1000);}
#ifndef KCD2VR_NATIVE_MORTAR69_NO_MAIN
int main(){
 ck(mortar69::Result{}.stopEntry72==mortar69::StopEntry72::Unknown,"default or older receipt never claims no native entry");
 prepare69();tick69();ck(cancels==1&&detaches==0&&homes==0,"mortar Stop only, no resource Detach/home transplant");
 ck(result69().status==mortar69::Status::ReadyForNativePlace,"actual empty parent/retired child acknowledged");
 ck(result69().stopEntry72==mortar69::StopEntry72::Entered,"native completion records actual Stop entry");tick69();ck(cancels==1,"one request cannot replay");
 const auto rd=[](std::uintptr_t at,auto& v){return SehCopy(reinterpret_cast<void*>(at),&v,sizeof(v));};kcd2vr::alchemy_tool18::Binding retained{};
 ck(kcd2vr::alchemy_tool18::read(rd,g_base.load(),req69.left.instance,req69.left.player,req69.left.character,req69.left.attachment,500,retained)&&!retained.action&&retained.type==2,"real left mortar remains joined after parent end");
 ck(kcd2vr::alchemy_tool18::read(rd,g_base.load(),req69.right.instance,req69.right.player,req69.right.character,req69.right.attachment,501,retained)&&!retained.action&&retained.type==3,"real right pestle remains joined after parent end");
 prepare69();put(memory_parent.data(),0x70,3);tick69();ck(!cancels&&result69().status==mortar69::Status::WaitingNativeCycle,"working cycle is not canceled mid Use marker");
 put(memory_parent.data(),0x70,2);tick69();ck(cancels==1,"READY can retire local parent");
 prepare69();put(memory_parent.data(),0x90,req69.child);put(memory_scope.data(),kScopeAction,req69.child);tick69();ck(cancels==1,"READY child is retired by native Stop, not awaited beforehand");
 prepare69();behavior=2;tick69();ck(cancels==1&&result69().status==mortar69::Status::Refused,"native refused cancel is not a transfer");tick69();ck(cancels==1,"no repeated cancel");
 ck(result69().stopEntry72==mortar69::StopEntry72::Entered,"refused native cancellation is still entered and cannot safely replay");
 prepare69();behavior=1;tick69();ck(cancels==1&&result69().status==mortar69::Status::Refused&&result69().stopEntry72==mortar69::StopEntry72::Entered,"fault after entering Stop remains uncertain");
 prepare69();kcd2vr::frik_pose_probe::frame.sourceEpoch++;tick69();ck(!cancels&&result69().status==mortar69::Status::Revoked,"source change cannot borrow native Stop");
 ck(result69().stopEntry72==mortar69::StopEntry72::NotEntered,"pre-call rejection explicitly proves no native Stop entry");
 prepare69();put(memory_parent.data(),0x70,3);tick69();ck(result69().stopEntry72==mortar69::StopEntry72::Unknown,"waiting cycle is not a terminal no-entry acknowledgment");
 prepare69();behavior=6;tick69();ck(cancels==1&&result69().status==mortar69::Status::Revoked&&result69().stopEntry72==mortar69::StopEntry72::Entered,"post-call lifetime revocation does not claim no entry");
 prepare69();put(rightObj.data(),8,std::uint32_t(999));tick69();ck(!cancels,"other hand attachment mismatch fails closed");
 prepare69();behavior=3;tick69();ck(cancels==1&&result69().status==mortar69::Status::Revoked&&result69().stopEntry72==mortar69::StopEntry72::Entered,"Stop detaching original mortar cannot publish retained-pair Ready");tick69();ck(cancels==1,"entered detach refusal never retries Stop");
 prepare69();cancelEffectH21=[](){put(rightAtt.data(),0x20,std::uintptr_t(0));TickMortarStop69(memory_controller.data(),1000);};
 tick69();ck(cancels==1&&result69().status==mortar69::Status::Revoked,"synchronous pestle detach plus recursive Stop loses pair without duplicate entry");
 prepare69();cancelEffectH21=[](){put(rightObj.data(),8,std::uint32_t(999));};tick69();
 ck(cancels==1&&result69().status==mortar69::Status::Revoked,"synchronous right object identity swap cannot publish Ready");
 std::printf("native mortar69: %u assertions PASS\n",checks);
}

#endif
