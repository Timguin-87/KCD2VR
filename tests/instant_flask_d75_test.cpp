#include <atomic>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include <string>
#include "../src/subsystems/alchemy/instant_native_d72.h"
#include "../src/subsystems/alchemy/presentation_speed_d81.h"
#include "../src/subsystems/alchemy/bridge67.h"
#include "../src/vrik/instant_activities_d72.h"
static unsigned outcomeRowsD79=0;static std::string lastOutcomeD79;
namespace kcd2vr::diagnostic_trace {void Write(const char* p) noexcept {if(std::strstr(p,"ALCHEMY_INSTANT_OUTCOME_D79")){++outcomeRowsD79;lastOutcomeD79=p;}}}
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
static unsigned effects=0,completions=0,nativeStarts=0,actionIndex=0;
static bool directorReturned=false,accept=true,effectWorks=true,completionWorks=true,reentrant=false,physicalMatches=true;
static bool flaskHidden=true;
static unsigned heldEntity=0;
static bool __fastcall NativeStartD75(void*){++nativeStarts;put(parent+0x70,std::uintptr_t(0x999));return true;}
static bool __fastcall NativeDirectorD75(void*,void*,int,bool){
 put(parent+0x40,instance+0x78);put(parent+0x3C,1);
 const bool ok=actionIndex==0?AlchemyStartDetourD72<0>(reinterpret_cast<void*>(parent)):
  actionIndex==1?AlchemyStartDetourD72<1>(reinterpret_cast<void*>(parent)):
  actionIndex==2?AlchemyStartDetourD72<2>(reinterpret_cast<void*>(parent)):AlchemyStartDetourD72<3>(reinterpret_cast<void*>(parent));
 check(!effects&&!completions);
 if(accept){put(parent+0x28,static_cast<unsigned char>(1));put(slots+8,parent);}
 directorReturned=true;return accept&&ok;
}
static bool HostMortarHeldD75(const AlchemyInstantOwnerD72&) noexcept {return false;}
static bool HostOrdinaryHeldD75(const AlchemyInstantOwnerD72& s,bool held) noexcept {
 check(s.receipt.side==(s.receipt.source>=10?0:1)&&s.input.entity==unsigned(100+s.receipt.source));
 return physicalMatches&&(held?heldEntity==s.input.entity:heldEntity==0);
}
static bool HostSemanticD75(const AlchemyInstantOwnerD72& s) noexcept {
 check(directorReturned&&Rd<std::uintptr_t>(reinterpret_cast<void*>(slots),8,0)==parent);
 const bool special=actionIndex>=2,returning=(actionIndex&1)!=0;
 check((special?s.receipt.source>=10&&s.receipt.source<=12:s.receipt.source==5)&&
  s.receipt.side==(special?0:1)&&s.input.entity==unsigned(100+s.receipt.source));
 check(s.receipt.action==(special?(returning?ai72::Action::ReturnIngredient:ai72::Action::TakeIngredient):
  (returning?ai72::Action::ReturnObject:ai72::Action::TakeObject)));
 ++effects;if(reentrant)TickInstantAlchemyD72(reinterpret_cast<void*>(ctrl),now);
 if(effectWorks){
  // Native leaves are mocked; exact retail disassembly separately proves their
  // attach/membership and flask-specific hide/detach behavior.
  put(instance+0x2A0+4*s.receipt.side,returning?21:s.receipt.source);
  heldEntity=returning?0:s.input.entity;flaskHidden=returning;
  if(special){
   put(instance+0x470+24*s.receipt.source+8,std::uintptr_t(0x800000+(returning?0:8)));
   put(instance+0x380,returning?s.counter-1:s.counter+1);
  }
 }
 return effectWorks;
}
static bool HostCompleteD75(const AlchemyInstantOwnerD72&,bool success) noexcept {
 check(directorReturned);++completions;if(success)check(effects==1&&effectWorks&&physicalMatches);
 if(completionWorks){put(slots+8,std::uintptr_t(0));put(instance+0x668,0u);}return completionWorks;
}
static void reset(bool returning=false,int type=5,int side=1){
 memory.clear();effects=completions=nativeStarts=0;actionIndex=(type>=10&&type<=12?2:0)+(returning?1:0);
 accept=effectWorks=completionWorks=physicalMatches=true;directorReturned=reentrant=false;
 heldEntity=returning?unsigned(100+type):0;flaskHidden=!returning;
 g_alchemy_instant_d72={};g_alchemy_start_scope_d72=nullptr;g_alchemy_instant_stop_d72=false;g_alchemy_instant_ready_d72=true;
 g_alchemy_instant_serial_d72=0;g_semi_alchemy46.written=g_alchemy_stage19c4.written=false;ai72::receipt.publish({});
 kcd2vr::instant_d72::activities=kcd2vr::instant_d72::crafting=true;retail_build112::ready=true;kcd2vr::workstation178r20c1::enabled=true;
 auto& w=kcd2vr::activity_input163::witness175;w.ready=true;w.transitionDepth175=0;w.lifetime175=3;w.active[0]=instance;
 put(instance,g_base.load()+0x3F60B08);put(instance+0x2C,static_cast<unsigned char>(1));put(instance+0x18,player);
 put(player,g_base.load()+0x3AA4B98);put(player+0x30,55u);put(player+0x9D0,manager);
 put(ctrl+kCtrlEntityId,55u);put(manager,g_base.load()+0x3EC9958);put(manager+0x38,player);
 put(instance+0x78,g_base.load()+0x3BFA5B0);put(instance+0xB0,slots);put(instance+0xB8,slots+16);put(slots+8,std::uintptr_t(0));
 put(parent,g_base.load()+(actionIndex>=2?(returning?0x3B87A38:0x3A5C308):(returning?0x3C23010:0x3A5CCD0)));put(parent+0x58,instance);put(parent+0x70,std::uintptr_t(0));put(reference,parent);
 put(parent+0x50,side);put(parent+0x54,static_cast<unsigned char>(side==1));put(parent+0x55,static_cast<unsigned char>(side==0));
 put(instance+0x304,type);put(instance+0x2A0,21);put(instance+0x2A4,21);put(instance+0x2A0+4*side,returning?type:21);
 put(instance+0x150+4*type,unsigned(100+type));put(instance+0x668,0u);
 if(actionIndex>=2){put(instance+0x470+24*type,std::uintptr_t(0x800000));
  put(instance+0x470+24*type+8,std::uintptr_t(0x800000+(returning?8:0)));
  put(instance+0x470+24*type+16,std::uintptr_t(0x800100));put(instance+0x380,returning?1:0);}
 ab72::Control c{};c.instance=instance;c.lifetime=3;c.tick=now;c.poseEpoch=4;c.sourceEpoch=5;c.modeEpoch=kcd2vr::minigame_hands45::modeEpoch67.load();
 c.enabled=true;c.admissionTickR22=900;c.type=type;c.side=side;c.entity=100+type;ab72::control.publish(c);
 auto& f=kcd2vr::frik_pose_probe::hostFrame;const kcd2vr::workstation_sample46::Frame empty{};f=empty;f.sequence=8;f.tick=now;f.timeNs=1000000000;f.poseEpoch=4;f.sourceEpoch=5;f.allowed=f.headValid=true;f.hand[0].valid=f.hand[1].valid=true;
 g_alchemy_director_d72=NativeDirectorD75;for(unsigned i=0;i<4;++i)g_alchemy_start_d72[i]=NativeStartD75;
}
static bool admit(){return AlchemyDirectorDetourD72(reinterpret_cast<void*>(instance+0x78),reinterpret_cast<void*>(reference),1,false);}
static void tick(){OwnerTransaction19c4 tx;TickInstantAlchemyD72(reinterpret_cast<void*>(ctrl),now);}
int main(){
 for(bool returning:{false,true}){
  reset(returning);check(admit()&&g_alchemy_instant_d72.active&&!nativeStarts&&!effects);tick();
  ai72::Receipt r{};check(ai72::receipt.read(r)&&r.entered&&r.effect&&r.completed&&r.source==5&&r.side==1);
  check(effects==1&&completions==1&&!g_alchemy_instant_d72.active);
  check(Rd<int>(reinterpret_cast<void*>(instance),0x2A4,0)==(returning?21:5)&&flaskHidden==returning&&heldEntity==(returning?0u:105u));
  check(Rd<unsigned>(reinterpret_cast<void*>(instance),0x668,1)==0);tick();check(effects==1&&completions==1);
  check(ip72::Current(ip72::Family::Crafting)==ip72::State::Available);
  for(int fault=0;fault<8;++fault){reset(returning);ab72::Control c{};ab72::control.read(c);
   switch(fault){case 0:c.tick=now-81;break;case 1:if(returning)put(parent+0x55,static_cast<unsigned char>(1));else put(instance+0x304,4);break;
    case 2:put(parent+0x50,0);break;case 3:put(instance+0x164,999u);break;
    case 4:kcd2vr::instant_d72::crafting=false;break;case 5:g_semi_alchemy46.written=true;break;
    case 6:retail_build112::ready=false;break;case 7:put(instance+0x2A4,returning?21:5);break;}
   ab72::control.publish(c);check(admit()&&nativeStarts==1&&!g_alchemy_instant_d72.active&&!effects);tick();
   check(ip72::Current(ip72::Family::Crafting)==(fault==4?ip72::State::Native:ip72::State::Unsupported));
  }
  reset(returning);check(admit());physicalMatches=false;tick();tick();
  check(effects==1&&completions==0&&g_alchemy_instant_d72.active); // exact attachment is pending, not a failed take
  now+=126;tick();ai72::Receipt failed{};check(ai72::receipt.read(failed)&&!failed.effect&&failed.completed&&failed.semanticExecutedD80);check(effects==1&&completions==1);
  reset(returning);check(admit());physicalMatches=false;tick();check(effects==1&&!completions);
  now+=20;kcd2vr::frik_pose_probe::hostFrame.tick=now;physicalMatches=true;tick();tick();
  ai72::Receipt delayed{};check(ai72::receipt.read(delayed)&&delayed.effect&&delayed.completed&&delayed.attachmentWaitMsD80==20);
  check(effects==1&&completions==1&&!g_alchemy_instant_d72.active);
  reset(returning);check(admit());physicalMatches=false;tick();++kcd2vr::activity_input163::witness175.lifetime175;tick();
  check(effects==1&&!completions&&!g_alchemy_instant_d72.active); // never complete an ended/replaced station parent
  reset(returning);check(admit());physicalMatches=false;tick();g_alchemy_instant_stop_d72=true;tick();tick();
  check(effects==1&&completions==1&&!g_alchemy_instant_d72.active);
  reset(returning);check(admit());reentrant=true;tick();tick();check(effects==1&&completions==1&&!g_alchemy_instant_d72.active);
  reset(returning);check(admit());++kcd2vr::activity_input163::witness175.lifetime175;tick();check(!effects&&!completions&&!g_alchemy_instant_d72.active);
  reset(returning);check(admit());g_alchemy_instant_stop_d72=true;tick();tick();check(!effects&&completions==1);
  reset(returning);check(admit());kcd2vr::instant_d72::crafting=false;tick();check(effects==1&&completions==1&&!g_alchemy_instant_d72.active);
  reset(returning);check(admit());completionWorks=false;tick();tick();check(effects==1&&completions==1&&g_alchemy_instant_d72.active);
  reset(returning);accept=false;check(!admit()&&!g_alchemy_instant_d72.active&&!effects&&!completions);
 }
 for(int type:{2,3,5,13,14,15,16}){reset(false,type,0);check(admit()&&nativeStarts==1&&!g_alchemy_instant_d72.active&&!effects);tick();check(ip72::Current(ip72::Family::Crafting)==ip72::State::Unsupported);}
 reset(true);put(instance+0x304,4);check(admit()&&g_alchemy_instant_d72.active&&!nativeStarts);tick();check(effects==1&&completions==1);
 reset();memory.erase(slots+8);tick();check(ip72::Current(ip72::Family::Crafting)==ip72::State::Unsupported);
 reset(true);put(parent+0x54,static_cast<unsigned char>(0));check(admit()&&nativeStarts==1&&!g_alchemy_instant_d72.active);
 // Actual Tick completion boundary: F5 OFF, ON, reentrant, failed-source and
 // stopped sessions, with unchanged native call counts and no duplicate row.
 auto& gateD79=kcd2vr::diagnostics::capture168::gate;
 const auto domainD79=kcd2vr::diagnostics::capture168::Domain::Interaction;
 gateD79.select(domainD79,false,now);reset();check(admit());tick();check(outcomeRowsD79==0&&effects==1&&completions==1);
 gateD79.select(domainD79,true,now);reset();check(admit());reentrant=true;tick();tick();
 check(outcomeRowsD79==1&&effects==1&&completions==1&&lastOutcomeD79.find("semanticReadback=1 parentRetired=1")!=std::string::npos);
 reset();check(admit());g_alchemy_instant_stop_d72=true;tick();tick();
 check(outcomeRowsD79==2&&effects==0&&completions==1&&lastOutcomeD79.find("semanticEntered=0 semanticReadback=0 parentRetired=1")!=std::string::npos);
 gateD79.select(domainD79,false,now);reset();check(admit());tick();check(outcomeRowsD79==2&&effects==1&&completions==1);
 // D81: special ingredients use their actual Take/ReturnIngredient native
 // admission, quantities/counters and delayed attachment observation. The
 // semantic helper still runs exactly once; only engine leaves are spies.
 for(int type:{10,11,12})for(bool returning:{false,true}){
  reset(returning,type,0);check(admit()&&g_alchemy_instant_d72.active&&!nativeStarts);
  physicalMatches=false;tick();tick();check(effects==1&&!completions&&g_alchemy_instant_d72.active);
  now+=20;kcd2vr::frik_pose_probe::hostFrame.tick=now;physicalMatches=true;tick();tick();
  ai72::Receipt r{};check(ai72::receipt.read(r)&&r.effect&&r.completed&&r.attachmentWaitMsD80==20);
  check(effects==1&&completions==1&&!g_alchemy_instant_d72.active);
  reset(returning,type,0);check(admit());physicalMatches=false;tick();
  now+=126;kcd2vr::frik_pose_probe::hostFrame.tick=now;tick();
  check(ai72::receipt.read(r)&&!r.effect&&r.completed&&effects==1&&completions==1);
  reset(returning,type,0);check(admit());physicalMatches=false;tick();
  ++kcd2vr::frik_pose_probe::hostFrame.poseEpoch;tick();
  check(ai72::receipt.read(r)&&!r.effect&&r.completed&&effects==1&&completions==1);
 }
 std::printf("D75 actual native flask adapter: %u checks PASS; native calls mocked, exact retail side effects separately checked\n",checks);
}
