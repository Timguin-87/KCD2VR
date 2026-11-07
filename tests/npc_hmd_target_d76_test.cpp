// Exercise the actual LookAt detour with the retained native leaves.
#define main retained_npc_look63_main
#include "npc_look63_test.cpp"
#undef main
static std::string packetD76;
namespace kcd2vr {bool CameraHook::GetLatestVisualForwardWorld(float& x,float& y,float& z,std::uint64_t){x=0;y=1;z=0;return true;}}
namespace kcd2vr::native_script_t2 {
bool Context::CallPacket(const char*,const char* input,char* output,std::size_t capacity)const noexcept {packetD76=input;if(capacity>5)std::strcpy(output,"D76|0");return true;}
bool Register(Client,Wanted)noexcept{return true;}bool Install()noexcept{return true;}
}
static bool subtitleResultD81=true,clearResultD81=true;
static unsigned subtitleCallsD81=0,clearCallsD81=0;
static bool subtitleD76(void*,void*,void*,void*,const bool*){++subtitleCallsD81;return subtitleResultD81;}
static bool clearD81(void*,void*){++clearCallsD81;return clearResultD81;}
int main(){
 std::vector<unsigned char> image(0x4080000);hostModuleHandleW=image.data();
 const auto base=reinterpret_cast<std::uintptr_t>(image.data());
 // Actual startup: the saved ON setting prepares both optional native observers
 // after the exact Henry look hook is established. Hook/native calls are leaves.
 auto* dos=reinterpret_cast<IMAGE_DOS_HEADER*>(image.data());dos->e_magic=IMAGE_DOS_SIGNATURE;dos->e_lfanew=0x100;
 auto* nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(image.data()+0x100);nt->Signature=IMAGE_NT_SIGNATURE;nt->FileHeader.TimeDateStamp=0x6A350E20;
 const unsigned char look[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x30};
 const unsigned char getter[]{0x48,0x8B,0x41,0x38,0xC3};
 const unsigned char show[]{0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x48,0x89,0x7C,0x24,0x20};
 const unsigned char clear[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x60};
 std::memcpy(image.data()+0x59B0F0,look,sizeof look);std::memcpy(image.data()+0x602360,getter,sizeof getter);
 std::memcpy(image.data()+0x1457A7C,show,sizeof show);std::memcpy(image.data()+0x566BCC,clear,sizeof clear);
 DetourModKit::hook::hostOriginal115=[](const auto& r)->std::uintptr_t{
  if(r.name=="kcd2vr_v6510_dialogue_player_look_point")return reinterpret_cast<std::uintptr_t>(&original63);
  if(r.name=="kcd2vr_d73_subtitle_speaker")return reinterpret_cast<std::uintptr_t>(&subtitleD76);
  if(r.name=="kcd2vr_d73_subtitle_clear")return reinterpret_cast<std::uintptr_t>(&clearD81);
  return 0;
 };
 lively_npc_d73::SetEnabled(true);SetEnabled(Install());
 ck63(Enabled()&&lively_npc_d73::ready&&lively_npc_d73::subtitleOriginal==&subtitleD76&&lively_npc_d73::clearOriginal==&clearD81);
 ck63(image[0x59B0F0]==0xCC&&image[0x1457A7C]==0xCC&&image[0x566BCC]==0xCC);
 alignas(8)std::array<unsigned char,0x40> entity{},other{},puppet{},wrapper{};
 put63(entity.data(),base+0x4072C78);put63(other.data(),base+0x4072C78);player63=entity.data();
 put63(puppet.data(),base+npc_target47c1::AiPuppet);put63(puppet.data()+0x38,player63);
 put63(image.data()+npc_target47c1::AiPuppet+0x18,base+0x602360);
 void* payload=puppet.data();put63(wrapper.data()+0x18,payload);
 npc_look_policy63::setFollowHmd(true);g_original=&original63;g_base63=base;g_enabled=true;g_installed=true;g_retired63=false;
 PublishDialogueOwner63({},false);lively_npc_d73::SetEnabled(true);lively_npc_d73::ready=true;
 lively_npc_d73::Frame f{hostTickCount64,lively_npc_d73::mode.load(),reinterpret_cast<std::uintptr_t>(player63),23,false,true,true,0,0,1.6f,0,1,0};
 auto run=[&](bool expected){float out[3]{};const auto before=originalCalls63;
  ck63(ResolveLookTargetWorldPosDetour(nullptr,wrapper.data(),out));ck63(originalCalls63==before+1);
  ck63((out[0]==final63[0]&&out[1]==final63[1]&&out[2]==final63[2])==expected);};
 lively_npc_d73::frames.publish(f);run(true); // no Henry-body/input admission exists
 ck63(!g_dialogue_active.load());
 auto bad=f;bad.tick-=81;lively_npc_d73::frames.publish(bad);run(false);
 bad=f;bad.dialogue=false;lively_npc_d73::frames.publish(bad);run(false);
 lively_npc_d73::frames.publish(f);put63(puppet.data()+0x38,static_cast<void*>(other.data()));run(false);put63(puppet.data()+0x38,player63);
 hmd63=false;run(false);hmd63=true;run(true);
 lively_npc_d73::SetEnabled(false);run(false);lively_npc_d73::SetEnabled(true);run(false);
 f.generation=lively_npc_d73::mode.load();lively_npc_d73::frames.publish(f);run(true);
 lively_npc_d73::SetListeningGestures(true);run(false); // any presentation setting change retires same-generation witness
 f.generation=lively_npc_d73::mode.load();lively_npc_d73::frames.publish(f);run(true);
 npc_look_policy63::setFollowHmd(false);auto before=hmdCalls63;run(false);ck63(before==hmdCalls63);
 // Actual subtitle and script service use the same native-scene identity, not a proximity candidate.
 npc_look_policy63::setFollowHmd(true);
 std::array<unsigned char,0x300> instance{};std::array<unsigned char,0x100> manager{},scene{};std::array<unsigned char,0x40> people{};
 f.scene=reinterpret_cast<std::uintptr_t>(scene.data());f.dialogueInstance=reinterpret_cast<std::uintptr_t>(instance.data());f.dialogueManager=reinterpret_cast<std::uintptr_t>(manager.data());f.playerSoul=123;
 put63(manager.data(),base+0x3E64C90);put63(manager.data()+0x90,f.dialogueInstance);
 put63(instance.data(),base+0x3E63A40);put63(instance.data()+0x18,base+0x3E63B60);put63(instance.data()+0x158,f.scene);put63(instance.data()+0x278,std::uint32_t(1));
 put63(scene.data(),reinterpret_cast<std::uintptr_t>(people.data()));put63(scene.data()+8,reinterpret_cast<std::uintptr_t>(people.data()+0x40));
 put63(people.data(),std::uint64_t(123));put63(people.data()+0x18,static_cast<unsigned char>(1));
 put63(people.data()+0x20,std::uint64_t(0xFEDCBA9876543211ull));put63(people.data()+0x38,static_cast<unsigned char>(1));
 lively_npc_d73::base=base;lively_npc_d73::frames.publish(f);
 lively_npc_d73::dialogueInterest=true; // exact worker scene is an explicit fixture input
 auto serviceD81=[&](){hostTickCount64+=100;f.tick=hostTickCount64;lively_npc_d73::frames.publish(f);
  lively_npc_d73::Service(native_script_t2::Context{1,2,hostTickCount64,1});};
 serviceD81();ck63(packetD76.find("|0|1|0|")!=std::string::npos); // no observed speaker is not quiet evidence
 callerD57=reinterpret_cast<void*>(base+0x1457A49);std::uintptr_t name=base+0x3B8DBB8;bool henry=true;
 ck63(lively_npc_d73::Subtitle(reinterpret_cast<void*>(41),&name,nullptr,nullptr,&henry));
 serviceD81();
 ck63(packetD76.starts_with("D76|")&&packetD76.find("|0|1|1|")!=std::string::npos);
 ck63(packetD76.ends_with("|1|18364758544493064721")); // exact >2^53 WUID, body enabled
 henry=false;ck63(lively_npc_d73::Subtitle(reinterpret_cast<void*>(41),&name,nullptr,nullptr,&henry));
 hostTickCount64+=100;f.tick=hostTickCount64;lively_npc_d73::frames.publish(f);
 lively_npc_d73::Service(native_script_t2::Context{1,2,hostTickCount64,1});
 ck63(packetD76.find("|0|1|0|")!=std::string::npos&&packetD76.ends_with("|0|18364758544493064721"));
 // A native Clear for this exact HUD/current scene permits listening while
 // choosing the next response. An NPC subtitle immediately suppresses it again.
 std::uintptr_t clearName=base+0x3C147C0;callerD57=reinterpret_cast<void*>(base+0x17990F4);
 auto clears=clearCallsD81;ck63(lively_npc_d73::Clear(reinterpret_cast<void*>(41),&clearName));
 ck63(clearCallsD81==clears+1);serviceD81();
 ck63(packetD76.find("|0|1|1|")!=std::string::npos&&packetD76.ends_with("|1|18364758544493064721"));
 lively_npc_d73::Speaker quiet{};ck63(lively_npc_d73::speakers.read(quiet)&&quiet.clearedD81&&!lively_npc_d73::Speaking(quiet,f,hostTickCount64));
 callerD57=reinterpret_cast<void*>(base+0x1457A49);henry=false;
 ck63(lively_npc_d73::Subtitle(reinterpret_cast<void*>(41),&name,nullptr,nullptr,&henry));serviceD81();
 ck63(packetD76.find("|0|1|0|")!=std::string::npos);
 callerD57=reinterpret_cast<void*>(base+0x17990F4);
 ck63(lively_npc_d73::Clear(reinterpret_cast<void*>(42),&clearName));serviceD81();
 ck63(packetD76.find("|0|1|0|")!=std::string::npos); // foreign HUD cannot lend a quiet phase
 clearResultD81=false;ck63(!lively_npc_d73::Clear(reinterpret_cast<void*>(41),&clearName));clearResultD81=true;serviceD81();
 ck63(packetD76.find("|0|1|0|")!=std::string::npos); // native failure is unknown, not silence
 callerD57=reinterpret_cast<void*>(base+0x1457A49);henry=true;
 ck63(lively_npc_d73::Subtitle(reinterpret_cast<void*>(41),&name,nullptr,nullptr,&henry));
 const auto calls=subtitleCallsD81;unsigned char invalidPlayer=2;
 ck63(lively_npc_d73::Subtitle(reinterpret_cast<void*>(41),&name,nullptr,nullptr,reinterpret_cast<const bool*>(&invalidPlayer)));
 ck63(subtitleCallsD81==calls+1);serviceD81();ck63(packetD76.find("|0|1|0|")!=std::string::npos);
 ck63(lively_npc_d73::Subtitle(reinterpret_cast<void*>(41),&name,nullptr,nullptr,&henry));
 subtitleResultD81=false;ck63(!lively_npc_d73::Subtitle(reinterpret_cast<void*>(41),&name,nullptr,nullptr,&henry));subtitleResultD81=true;
 serviceD81();ck63(packetD76.find("|0|1|0|")!=std::string::npos);
 put63(manager.data()+0x90,std::uintptr_t(0));hostTickCount64+=100;f.tick=hostTickCount64;lively_npc_d73::frames.publish(f);
 lively_npc_d73::Service(native_script_t2::Context{1,2,hostTickCount64,1});
 ck63(packetD76.find("|0|0|0|")!=std::string::npos&&packetD76.ends_with("|0|0|0"));
 std::printf("D81 actual startup, HMD target, subtitle/clear and service %u checks PASS; native engine mocked, final NPC pose NOT_OBSERVED\n",checks63);
}
