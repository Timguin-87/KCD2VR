#include <atomic>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include "../src/subsystems/alchemy/instant_native_d72.h"
#include "../src/subsystems/alchemy/presentation_speed_d81.h"
#include "../src/subsystems/alchemy/bridge67.h"
#include "../src/vrik/instant_activities_d72.h"
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::frik_pose_probe {
inline workstation_sample46::Frame hostFrame;
bool WorkstationFrame46(workstation_sample46::Frame& f) noexcept {f=hostFrame;return true;}
}
namespace kcd2vr::workstation178r20c1 {inline bool enabled=true;bool fullVrAlchemy63(std::uint64_t){return enabled;}}
namespace kcd2vr::activity_input163 {
struct Witness {std::atomic<bool> ready{true};std::atomic<unsigned> transitionDepth175{0};
 std::atomic<std::uint64_t> lifetime175{3};std::array<std::atomic<std::uintptr_t>,1> active{};} witness175;
enum class Kind175 {Alchemy};
}
namespace retail_build112 {inline bool ready=true;bool Ready(std::uintptr_t){return ready;}}
constexpr std::uintptr_t instance=0x100000,parent=0x200000,player=0x300000,manager=0x400000,ctrl=0x500000,slots=0x600000,reference=0x700000;
static std::uint64_t now=1000;
static unsigned checks=0;
static void checkAt(bool b,int line){++checks;if(!b){std::fprintf(stderr,"D75 native check %u line %d failed\n",checks,line);std::abort();}}
#define check(x) checkAt((x),__LINE__)
static std::uint64_t GetTickCount64(){return now;}
static std::unordered_map<std::uintptr_t,unsigned char> memory;
template<class T>static void put(std::uintptr_t at,T value){const auto* b=reinterpret_cast<const unsigned char*>(&value);for(unsigned i=0;i<sizeof(T);++i)memory[at+i]=b[i];}
static bool SehCopy(const void* src,void* out,std::size_t n){auto at=reinterpret_cast<std::uintptr_t>(src);auto* b=static_cast<unsigned char*>(out);for(std::size_t i=0;i<n;++i){auto p=memory.find(at+i);if(p==memory.end())return false;b[i]=p->second;}return true;}
template<class T>static T Rd(const void* p,std::size_t offset,T fallback){T out{};return SehCopy(static_cast<const unsigned char*>(p)+offset,&out,sizeof(out))?out:fallback;}
static std::atomic<std::uintptr_t> g_base{0x180000000ull};
static std::atomic<void*> g_henry_ctrl{reinterpret_cast<void*>(ctrl)};
static bool IsHenryController(void* p){return p==g_henry_ctrl.load();}
enum OwnerState{Idle};static struct {OwnerState state=Idle;} g_owner;
static struct {bool written=false;} g_semi_alchemy46,g_alchemy_stage19c4;
static unsigned g_owner_transaction_depth19c4=0;
struct OwnerTransaction19c4 {OwnerTransaction19c4(){++g_owner_transaction_depth19c4;}~OwnerTransaction19c4(){--g_owner_transaction_depth19c4;}explicit operator bool()const{return true;}};
static bool OnAlchemyUpdateOwnerR27(){return true;}
constexpr unsigned kCtrlEntityId=0x38;
struct AlchemyInstantOwnerD72;
static bool HostMortarHeldD75(const AlchemyInstantOwnerD72&) noexcept;
static bool HostSemanticD75(const AlchemyInstantOwnerD72&) noexcept;
static bool HostOrdinaryHeldD75(const AlchemyInstantOwnerD72&,bool) noexcept;
static bool HostCompleteD75(const AlchemyInstantOwnerD72&,bool) noexcept;
namespace native_script_t2=kcd2vr::native_script_t2;
#include "generated/instant_mortar_d75.inc"
static unsigned effects=0,completions=0,nativeStarts=0;
static bool heldPair=true,directorReturned=false,accept=true,effectWorks=true,completionWorks=true,reentrant=false;
static bool __fastcall NativeStartD75(void*){++nativeStarts;put(parent+0x70,std::uintptr_t(0x999));return true;}
static bool __fastcall NativeDirectorD75(void*,void*,int,bool){
 put(parent+0x40,instance+0x78);put(parent+0x3C,1);
 const bool ok=AlchemyStartDetourD72<6>(reinterpret_cast<void*>(parent));
 check(!effects&&!completions); // no helper or completion inside native Start
 if(accept){put(parent+0x28,static_cast<unsigned char>(1));put(slots+8,parent);}
 directorReturned=true;return accept&&ok;
}
static bool HostMortarHeldD75(const AlchemyInstantOwnerD72& s) noexcept {return heldPair&&s.mortarEntityD75==102&&s.pestleEntityD75==103;}
static bool HostOrdinaryHeldD75(const AlchemyInstantOwnerD72&,bool) noexcept {return true;}
static bool HostSemanticD75(const AlchemyInstantOwnerD72& s) noexcept {
 check(directorReturned&&Rd<std::uintptr_t>(reinterpret_cast<void*>(slots),8,0)==parent);
 check(s.receipt.action==ai72::Action::PourMortarD75&&s.receipt.source==2&&s.receipt.destination==6&&s.receipt.side==1);
 ++effects;
 if(reentrant)TickInstantAlchemyD72(reinterpret_cast<void*>(ctrl),now);
 if(effectWorks){put(instance+0x470+24*2+8,std::uintptr_t(0x800000));put(instance+0x470+24*6+8,std::uintptr_t(0x900018));}
 return effectWorks;
}
static bool HostCompleteD75(const AlchemyInstantOwnerD72&,bool success) noexcept {
 check(directorReturned);++completions;
 if(success)check(effects==1&&effectWorks);
 if(completionWorks)put(slots+8,std::uintptr_t(0));return completionWorks;
}
static void reset(int admittedType=2){
 memory.clear();effects=completions=nativeStarts=0;heldPair=accept=effectWorks=completionWorks=true;directorReturned=reentrant=false;
 g_alchemy_instant_d72={};g_alchemy_start_scope_d72=nullptr;g_alchemy_instant_stop_d72=false;g_alchemy_instant_ready_d72=true;
 g_alchemy_instant_serial_d72=0;g_semi_alchemy46.written=g_alchemy_stage19c4.written=false;ai72::receipt.publish({});
 kcd2vr::instant_d72::crafting=true;retail_build112::ready=true;kcd2vr::workstation178r20c1::enabled=true;
 auto& w=kcd2vr::activity_input163::witness175;w.ready=true;w.transitionDepth175=0;w.lifetime175=3;w.active[0]=instance;
 put(instance,g_base.load()+0x3F60B08);put(instance+0x2C,static_cast<unsigned char>(1));put(instance+0x18,player);
 put(player,g_base.load()+0x3AA4B98);put(player+0x30,55u);put(player+0x9D0,manager);
 put(ctrl+kCtrlEntityId,55u);put(manager,g_base.load()+0x3EC9958);put(manager+0x38,player);
 put(instance+0x78,g_base.load()+0x3BFA5B0);put(instance+0xB0,slots);put(instance+0xB8,slots+16);put(slots+8,std::uintptr_t(0));
 put(parent,g_base.load()+0x3A5BB90);put(parent+0x58,instance);put(parent+0x70,std::uintptr_t(0));put(reference,parent);
 put(instance+0x304,6);put(instance+0x2A0,2);put(instance+0x2A4,21);put(instance+0x158,102u);put(instance+0x15C,103u);
 for(int t:{2,6}){const auto base=t==2?0x800000u:0x900000u;put(instance+0x470+24*t,std::uintptr_t(base));put(instance+0x470+24*t+8,std::uintptr_t(base+(t==2?16:8)));put(instance+0x470+24*t+16,std::uintptr_t(base+128));}
 ab72::Control c{};c.instance=instance;c.lifetime=3;c.tick=now;c.poseEpoch=4;c.sourceEpoch=5;c.modeEpoch=kcd2vr::minigame_hands45::modeEpoch67.load();
 c.enabled=true;c.admissionTickR22=900;c.type=admittedType;c.side=admittedType==2?1:0;c.entity=100+admittedType;c.heldMortarPourD75=true;
 c.passenger=true;ab72::control.publish(c);
 auto& f=kcd2vr::frik_pose_probe::hostFrame;const kcd2vr::workstation_sample46::Frame empty{};f=empty;f.sequence=8;f.tick=now;f.timeNs=1000000000;f.poseEpoch=4;f.sourceEpoch=5;f.allowed=f.headValid=true;f.hand[0].valid=f.hand[1].valid=true;
 g_alchemy_director_d72=NativeDirectorD75;g_alchemy_start_d72[6]=NativeStartD75;
}
static bool admit(){return AlchemyDirectorDetourD72(reinterpret_cast<void*>(instance+0x78),reinterpret_cast<void*>(reference),1,false);}
static void tick(){OwnerTransaction19c4 tx;TickInstantAlchemyD72(reinterpret_cast<void*>(ctrl),now);}
int main(){
 for(int source:{2,3}){reset(source);check(admit()&&g_alchemy_instant_d72.active&&!nativeStarts&&!effects);tick();
  ai72::Receipt r{};check(ai72::receipt.read(r)&&r.effect&&r.completed&&r.source==2&&r.side==1);
  check(effects==1&&completions==1&&Rd<int>(reinterpret_cast<void*>(instance),0x2A0,0)==2&&heldPair);tick();check(effects==1&&completions==1);}
 for(int fault=0;fault<9;++fault){reset();ab72::Control c{};ab72::control.read(c);
  switch(fault){case 0:c.heldMortarPourD75=false;break;case 1:c.tick=now-81;break;case 2:heldPair=false;break;
   case 3:put(instance+0x304,0);break;case 4:put(parent+0x70,std::uintptr_t(9));break;
   case 5:kcd2vr::instant_d72::crafting=false;break;case 6:g_semi_alchemy46.written=true;break;
   case 7:put(instance+0x470+24*2+8,std::uintptr_t(0x800000));break;case 8:retail_build112::ready=false;break;}
  ab72::control.publish(c);check(admit());check(nativeStarts==1&&!g_alchemy_instant_d72.active&&!effects);}
 reset();accept=false;check(!admit()&&!g_alchemy_instant_d72.active&&!effects&&!completions);
 reset();check(admit());ab72::Control c{};ab72::control.read(c);c.heldMortarPourD75=false;ab72::control.publish(c);tick();check(!effects&&completions==1);
 reset();check(admit());++kcd2vr::activity_input163::witness175.lifetime175;tick();check(!effects&&!completions&&!g_alchemy_instant_d72.active);
 reset();check(admit());heldPair=false;tick();check(!effects&&completions==1);
 reset();check(admit());effectWorks=false;tick();tick();check(effects==1&&completions==1);
 reset();check(admit());completionWorks=false;tick();tick();check(effects==1&&completions==1&&g_alchemy_instant_d72.active);
 reset();check(admit());reentrant=true;tick();tick();check(effects==1&&completions==1&&!g_alchemy_instant_d72.active);
 std::printf("D75 actual native mortar adapter: %u checks PASS; native calls mocked, exact retail pins separately checked\n",checks);
}
