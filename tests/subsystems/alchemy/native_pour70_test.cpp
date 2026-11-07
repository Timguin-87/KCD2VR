#define KCD2VR_ALCHEMY_SEMI46_NO_MAIN
#include "../../alchemy_semi46_native_test.cpp"
namespace kcd2vr::action_time_owner59c11 { namespace {
static unsigned nativePlaceCalls70=0;
static void NativePlaceEvent70(void*,void*,const void*){++nativePlaceCalls70;}
static int Scenario70(){
 Prepare46(semi46::Mortar,2);g_mortar_place_events70={};
 auto pin=[](unsigned at,std::initializer_list<unsigned char> bytes){std::copy(bytes.begin(),bytes.end(),image.begin()+at);};
 pin(0x6C4A34,{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x40,0x41,0x8b,0x40,0x1c});
 pin(0x6C3B16,{0x48,0x8d,0x7b,0x70});put(image.data(),0x3A5BB90+0xF8,g_base.load()+0x6C3AC0);
 ck46(MortarPlacePins70());semi46::mortarReady70=true;
 put(parent46.data(),0,g_base.load()+0x3A5BB90);put(parent46.data(),0x70,reinterpret_cast<std::uintptr_t>(action.data()));
 put(parent46.data(),0x68,unsigned(101));put(parent46.data(),0x6C,unsigned(102));
 minigame_hands45::setMode(minigame_hands45::Alchemy,minigame_hands45::Gesture);
 kcd2vr::combat_rules178r10::workstationEnabled=true;
 namespace br=kcd2vr::subsystems::alchemy::bridge67;br::Control c{};
 c.instance=input46.instance;c.parent=reinterpret_cast<std::uintptr_t>(parent46.data());c.lifetime=input46.lifetime;
 c.tick=1000;c.poseEpoch=8;c.sourceEpoch=9;c.modeEpoch=minigame_hands45::modeEpoch67.load();c.enabled=true;c.controllerMortar70=true;c.entity=72;c.type=2;c.side=1;
 put(alchemy46.data(),0x158,unsigned(72));input46.hand=1;semi46::control.publish(input46);br::control.publish(c);
 QueueDetour(controller.data(),action.data(),0);ck46(g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==0);
 InstallQueued46();semi46::WorkWitness w{};ck46(SemiAlchemyParent46(controller.data(),w)&&w.kind==semi46::MortarPlace70&&w.phase==0);
 // No earned gesture work means no animation advance, even though native input
 // admitted the placement. The real original controller still executes once.
 UpdateDetour(controller.data(),.02f);ck46(Rd<float>(action.data(),kActActiveTime,-1)==0);
 Sample46(1020,.02);c.tick=1020;br::control.publish(c);UpdateDetour(controller.data(),.02f);
 ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.02f)<1.e-6);
 std::array<unsigned char,0x50> event{};put(event.data(),0x1C,unsigned(102));g_mortar_place_original70=NativePlaceEvent70;
 MortarPlaceEventDetour70(parent46.data(),nullptr,event.data());ck46(nativePlaceCalls70==1);
 ck46(SemiAlchemyParent46(controller.data(),w)&&w.phase==1);
 // An unrelated event forwards once but cannot claim Use/Detach completion.
 put(event.data(),0x1C,unsigned(999));MortarPlaceEventDetour70(parent46.data(),nullptr,event.data());ck46(nativePlaceCalls70==2);
 ck46(SemiAlchemyParent46(controller.data(),w)&&w.phase==1);
 put(event.data(),0x1C,unsigned(101));MortarPlaceEventDetour70(parent46.data(),nullptr,event.data());ck46(nativePlaceCalls70==3);
 ck46(SemiAlchemyParent46(controller.data(),w)&&w.phase==2);
 // Same allocation with a different token cannot inherit native completion.
 input46.manualUse=true;semi46::control.publish(input46);UpdateDetour(controller.data(),.02f);ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
 put(action.data(),kActToken,unsigned(56));ck46(SemiAlchemyParent46(controller.data(),w)&&w.phase==0);
 c.controllerMortar70=false;br::control.publish(c);ck46(!SemiAlchemyParent46(controller.data(),w));
 std::printf("native mortar70: %u checks PASS\n",checks46);return 0;
}
}}
int main(){return kcd2vr::action_time_owner59c11::Scenario70();}
