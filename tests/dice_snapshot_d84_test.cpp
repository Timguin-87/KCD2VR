// Executes the production activity TU and its completed-frame callback. Native
// heap bytes, retail readiness and OS/hook leaves are explicit host fixtures.
#ifndef _MSC_VER
#define __fastcall
#endif
#include "../src/activity_input163.cpp"
#include <vector>
#include <string>
#include <cstdlib>
namespace {
std::vector<std::string> diceLinesD84;
bool retailReadyDiceD84=true;
unsigned diceChecksD84=0;
void checkDiceD84(bool ok,const char* why){++diceChecksD84;if(!ok){std::fprintf(stderr,"D84 dice snapshot: %s\n",why);std::abort();}}
template<class T>void dicePutD84(void* p,unsigned offset,T value){std::memcpy(static_cast<unsigned char*>(p)+offset,&value,sizeof(value));}
}
namespace kcd2vr::diagnostic_trace {void Write(const char* line)noexcept{if(std::string(line).find("D84 F5_DICE_NATIVE")==0)diceLinesD84.emplace_back(line);}}
namespace kcd2vr::retail_build112 {bool Ready(std::uintptr_t)noexcept{return retailReadyDiceD84;}bool Verify(std::uintptr_t)noexcept{return false;}}
namespace kcd2vr::frik_pose_probe {
bool ReadSmithContactHammer68(kcd2vr::subsystems::smithing::contact68::HammerFrame&,std::uint64_t)noexcept{return false;}
std::uintptr_t FindSmithContactEntity68(std::uint32_t)noexcept{return 0;}
}

int main(){
    namespace ai=kcd2vr::activity_input163;namespace cap=kcd2vr::diagnostics::capture168;
    // Invalid-pointer F5-off controls call the exact diagnostic observer directly;
    // the completed-frame callback also owns independent native die selection.
    auto& w=ai::witness175;w.Clear();w.ready=true;w.dice=1;ai::base175=1;hostTickCount64=1000;
    cap::gate.select(cap::Domain::Interaction,false,hostTickCount64);
    ai::DiceNativeFrameEndD84();
    checkDiceD84(diceLinesD84.empty(),"F5 off never dereferences deliberately invalid game pointers");
    cap::gate.select(cap::Domain::Vrik,true,hostTickCount64);ai::DiceNativeFrameEndD84();
    checkDiceD84(diceLinesD84.empty(),"F7 alone never activates F5 native reads");
    cap::gate.select(cap::Domain::Vrik,false,hostTickCount64);
    std::vector<unsigned char> image(0x3000000);
    const auto base=reinterpret_cast<std::uintptr_t>(image.data());
    const unsigned char mapRead[]{0x48,0x8B,0x49,0x30},indexRead[]{0x83,0xB9,0xF0,0x04,0,0,0},
        indexClear[]{0x89,0x99,0xF0,0x04,0,0},stateRead[]{0x48,0x8B,0x83,0xA0,0x04,0,0};
    std::memcpy(image.data()+0x2E92D60,mapRead,sizeof(mapRead));std::memcpy(image.data()+0x2E8B8D6,indexRead,sizeof(indexRead));
    std::memcpy(image.data()+0x2E951F1,indexClear,sizeof(indexClear));std::memcpy(image.data()+0x2E8B06C,stateRead,sizeof(stateRead));
    std::array<unsigned char,0x600> instance{};std::array<unsigned char,0x70> player{};
    char waiting[]="dice_waiting",interaction[]="dice_interaction",unbounded[48];std::memset(unbounded,'x',sizeof(unbounded));
    const auto self=reinterpret_cast<std::uintptr_t>(instance.data()),p=reinterpret_cast<std::uintptr_t>(player.data());
    auto prepare=[&](){
        w.Clear();w.ready=true;w.transitionDepth175=0;ai::base175=base;
        dicePutD84(instance.data(),0,base+0x3F72098);dicePutD84(instance.data(),0x18,p);
        dicePutD84(instance.data(),0x28,std::uint32_t(77));dicePutD84(instance.data(),0x2C,std::uint8_t(1));
        dicePutD84(instance.data(),0x30,reinterpret_cast<std::uintptr_t>(waiting));
        dicePutD84(instance.data(),0x4A0,std::uintptr_t(0x1234));dicePutD84(instance.data(),0x4F0,2);
        dicePutD84(player.data(),0,base+0x3AA4B98);dicePutD84(player.data(),0x38,std::uintptr_t(0x5678));
        w.Begin(ai::Kind175::Dice,self,hostTickCount64);diceLinesD84.clear();retailReadyDiceD84=true;
        cap::gate.select(cap::Domain::Interaction,false,hostTickCount64);cap::gate.select(cap::Domain::Interaction,true,hostTickCount64);
    };
    prepare();const auto originalInstance=instance;const auto originalPlayer=player;
    ai::AlchemySelectionNativeFrameEnd38();
    checkDiceD84(diceLinesD84.size()==1&&diceLinesD84[0].find("qualified=1")!=std::string::npos&&
        diceLinesD84[0].find("nativeMap=dice_waiting")!=std::string::npos&&diceLinesD84[0].find("nativeCursorIndex=2")!=std::string::npos,"native frame-end integration reports exact current map/index");
    checkDiceD84(instance==originalInstance&&player==originalPlayer,"observer never mutates native bytes");
    checkDiceD84(diceLinesD84[0].find("nativeKeepState=UNJOINED")!=std::string::npos&&
        diceLinesD84[0].find("handCupControl=NOT_IMPLEMENTED")!=std::string::npos,"unproven keep/cup semantics never claim completion");
    checkDiceD84(diceLinesD84[0].find("cupGate=NATIVE_CUP_CONTRACT_UNQUALIFIED")!=std::string::npos&&
        diceLinesD84[0].find("cupAttachment=UNJOINED cupManipulationInterval=UNJOINED cupThrowCompletion=UNJOINED")!=std::string::npos,
        "missing cup attachment, manipulation interval and native completion remain explicit bounded diagnostics");
    hostTickCount64+=10;ai::AlchemySelectionNativeFrameEnd38();checkDiceD84(diceLinesD84.size()==1,"native sampling cadence bounded");
    hostTickCount64+=40;ai::AlchemySelectionNativeFrameEnd38();checkDiceD84(diceLinesD84.size()==1,"unchanged state coalesced");
    dicePutD84(instance.data(),0x30,reinterpret_cast<std::uintptr_t>(interaction));dicePutD84(instance.data(),0x4F0,-1);
    hostTickCount64+=50;ai::AlchemySelectionNativeFrameEnd38();
    checkDiceD84(diceLinesD84.size()==2&&diceLinesD84.back().find("nativeMap=dice_interaction")!=std::string::npos&&
        diceLinesD84.back().find("nativeCursorIndex=-1")!=std::string::npos,"native map/index changes produce edge receipt");
    ai::DiceSnapshotD84 sample{};
    for(int refusal=0;refusal<7;++refusal){prepare();
        if(refusal==0)w.transitionDepth175=1;
        if(refusal==1)retailReadyDiceD84=false;
        if(refusal==2)dicePutD84(instance.data(),0,base+0x3F72099);
        if(refusal==3)dicePutD84(player.data(),0,base+0x3AA4B99);
        if(refusal==4)dicePutD84(instance.data(),0x2C,std::uint8_t(0));
        if(refusal==5)dicePutD84(instance.data(),0x30,std::uintptr_t(0));
        if(refusal==6)dicePutD84(instance.data(),0x30,reinterpret_cast<std::uintptr_t>(unbounded));
        checkDiceD84(!ai::ReadDiceSnapshotD84(sample),"transition/build/class/player/running/map qualification refuses invalid native state");
    }
    prepare();cap::gate.select(cap::Domain::Vrik,true,hostTickCount64);ai::AlchemySelectionNativeFrameEnd38();
    checkDiceD84(diceLinesD84.size()==1,"F5+F7 operate together");
    cap::gate.select(cap::Domain::Interaction,false,hostTickCount64);w.dice=1;ai::base175=1;
    ai::DiceNativeFrameEndD84();checkDiceD84(diceLinesD84.size()==1,"turning F5 off stops reads despite F7 staying on");
    cap::gate.select(cap::Domain::Vrik,false,hostTickCount64);w.Clear();w.ready=false;
    std::printf("D84 production completed-frame dice diagnostic: %u checks PASS; native memory/OS substituted; headset NOT_RUN\n",diceChecksD84);
}
