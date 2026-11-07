// Real production mid callback/install/read view. Native actor/class getters,
// the continuation of the pinned three-load sequence and native movement
// sampler/cache are explicit host services, not an emulated game acceptance.
#define KCD2VR_TEST_HOOK_SPY115
#define KCD2VR_NPC_LOOK_TEST63
#define KCD2VR_NPC_SPACING_HOST_TEST
#define _ReturnAddress HostReturnAddress
#include <cstdint>
static std::uintptr_t HostSpacingCombatActor(std::uintptr_t) noexcept;
static std::uintptr_t HostSpacingWeaponClass(std::uintptr_t) noexcept;
#include "../src/subsystems/direct/npc_tuning.cpp"
#include <cstdio>
#include <algorithm>
#include <limits>
#include <map>
#include <string>
#include <thread>
#include "subsystems/direct/npc_defense.h"
#include "subsystems/direct/combat_router_h3.h"
extern "C" __declspec(dllimport) void* __stdcall VirtualAlloc(void*,std::size_t,unsigned long,unsigned long);
namespace ns=kcd2vr::npc_unarmed_spacing;
using Address=std::uintptr_t;
static Address image=0,currentEntity=0,currentActor=0,currentEquipment=0,currentRow=0,currentModule=0,alternateVerifiedModule=0;
static bool verified=true;
static std::atomic<unsigned> actorCalls{0},classCalls{0},nativeCalls{0};
static unsigned checks=0,failed=0;
static void check(bool ok,const char* why){++checks;if(!ok){++failed;std::printf("FAIL %s\n",why);}}
static void checkClose(float actual,float expected,const char* why){check(std::fabs(actual-expected)<1e-5f,why);}
namespace kcd2vr::retail_build112 {
bool Verify(std::uintptr_t base) noexcept {return verified&&(base==image||(alternateVerifiedModule&&base==alternateVerifiedModule));}
bool Ready(std::uintptr_t base) noexcept {return Verify(base);}
}
static Address HostSpacingCombatActor(Address entity) noexcept {++actorCalls;return entity==currentEntity?currentActor:0;}
static Address HostSpacingWeaponClass(Address equipment) noexcept {++classCalls;return equipment==currentEquipment+0x18?currentRow:0;}
template<class T> static void write(Address at,T value){std::memcpy(reinterpret_cast<void*>(at),&value,sizeof(value));}
template<class T> static T read(Address at){T value{};std::memcpy(&value,reinterpret_cast<const void*>(at),sizeof(value));return value;}
struct Fixture {
 alignas(16) std::array<unsigned char,0x700> npc{};
 alignas(16) std::array<unsigned char,0x98> move{};
 alignas(16) std::array<unsigned char,0x400> actor{};
 alignas(16) std::array<unsigned char,0x40> equipment{},entity{},row{};
 std::array<Address,2> capture{};
 Address n()const{return reinterpret_cast<Address>(npc.data());}
 Address m()const{return reinterpret_cast<Address>(move.data());}
 Address a()const{return reinterpret_cast<Address>(actor.data());}
 Address e()const{return reinterpret_cast<Address>(equipment.data());}
 Address c()const{return reinterpret_cast<Address>(capture.data());}
 void reset(){
  npc.fill(0);move.fill(0);actor.fill(0);equipment.fill(0);entity.fill(0);row.fill(0);
  currentEntity=reinterpret_cast<Address>(entity.data());currentActor=a();currentEquipment=e();currentRow=reinterpret_cast<Address>(row.data());
  write(n(),image+0x3FF1B18);write(n()+0x638,currentEntity);write(n()+0x640,m());
  write(m(),image+0x3A836A0);write(m()+0x20,n());profile(1.5f,3.f,2.f);
  write(a(),image+0x3E3FAC8);write(a()+0x3E8,e());write(e()+8,a());write(currentRow+8,std::uint32_t{12});
  capture={m(),n()};
 }
 void profile(float inner,float outer,float activity){write(m()+0x28,activity);write(m()+0x2C,inner);write(m()+0x30,outer);}
 DetourModKit::hook::MidContext context() const {
  using namespace DetourModKit::hook;MidContext ctx{};
  for(unsigned i=0;i<12;++i)ctx.registers[i]=0x700000+i;
  gpr(ctx,Gpr::Rax)=m();gpr(ctx,Gpr::Rdi)=c();gpr(ctx,Gpr::Rcx)=n();return ctx;
 }
};
struct Output {float inner=91.f,outer=92.f,activity=93.f;Address loadedFrom=0;};
static Output continuation(Fixture& f,float terrain=0,float tactical=0,bool override=false,bool declined=false){
 ++nativeCalls;Output out{};
 // Exact207964B/2079661 decline branches precede the mid site entirely.
 if(declined)return out;
 auto ctx=f.context();const auto before=ctx;
 ns::Sample(ctx);
 using namespace DetourModKit::hook;
 for(unsigned i=0;i<12;++i)if(i!=static_cast<unsigned>(Gpr::Rax))check(ctx.registers[i]==before.registers[i],"mid callback preserves every unrelated native register");
 const Address source=gpr(ctx,Gpr::Rax);out.loadedFrom=source;
 // Pinned native207967C/688/694 scalar loads; source overwritten207969A.
 out.inner=read<float>(source+0x2C);out.activity=read<float>(source+0x28);out.outer=read<float>(source+0x30);
 // Explicit native continuation fixture: terrain and tactical offsets are
 // added AFTER the view; an original native keep-distance override wins.
 out.activity+=terrain;out.inner+=terrain;out.outer+=terrain;
 if(override){out.inner=4.f;out.outer=5.f;}
 out.inner+=tactical;out.outer+=tactical;
 return out;
}
// Actual UI host slots and actual lazy Mod activation block compose with the
// actual native tuning install above. Only INI storage/module lookup/logging
// are host services; no duplicate apply/load/activation model is used.
namespace kcd2vr::npc_defense {bool Ready() noexcept {return false;}}
namespace kcd2vr {
struct SpacingSettingsState {
 bool touch_attack_direction=true;
#define KCD2VR_M2_HOST_SLOT 285
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
};
std::atomic<bool> enable_touch_attack_direction_{true};
std::atomic<int> g_npc_block39{0};
std::map<std::string,float> spacingIni;
namespace diagnostic_trace {std::vector<std::string> spacingMessages;void Write(const char* line){spacingMessages.emplace_back(line);}}
static void* HostSpacingModule(const wchar_t*){return reinterpret_cast<void*>(::currentModule);}
static void TickSpacingControl(float& npcSpacingAttempt,Address& npcSpacingModule){
#define GetModuleHandleW HostSpacingModule
#define KCD2VR_M2_HOST_SLOT 9010
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
#undef GetModuleHandleW
}
static void ApplySpacingSettings(const SpacingSettingsState& state){
#define KCD2VR_M2_HOST_SLOT 73
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
}
static SpacingSettingsState ReadSpacingSettings(){SpacingSettingsState out;
#define KCD2VR_M2_HOST_SLOT 26
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
 return out;
}
static void SaveSpacingSettings(){bool ok=true;
 auto wf=[](const char* key,float value){spacingIni[key]=value;return true;};auto wi=wf;auto wb=wf;
#define KCD2VR_M2_HOST_SLOT 118
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
 check(ok,"actual INI save accepts host-owned storage");
}
static float ReadIniFloatExplicit(const char*,const char*,const char* key,float fallback){const auto it=spacingIni.find(key);return it==spacingIni.end()?fallback:it->second;}
static int ReadIniIntExplicit(const char* file,const char* section,const char* key,int fallback){return int(ReadIniFloatExplicit(file,section,key,float(fallback)));}
static void LoadSpacingSettings(){const char* explicit_ini="host.ini";
#define KCD2VR_M2_HOST_SLOT 155
#include "subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
}
}
static void SettingsAndActivation(Fixture& f){using namespace kcd2vr;
 float npcSpacingAttempt=1.f;Address npcSpacingModule=0;currentModule=::image;
 auto tick=[&]{TickSpacingControl(npcSpacingAttempt,npcSpacingModule);};
 spacingIni.clear();LoadSpacingSettings();auto state=ReadSpacingSettings();
 checkClose(state.npc_unarmed_spacing_percent,60.f,"missing setting selects new boxing preference");
 check(spacingIni.empty(),"load migration does not write external INI");
 for(float oldPercent:{25.f,42.f,60.f,75.f,99.f,100.f}){
  spacingIni.clear();spacingIni["VrNpcUnarmedSpacingPercent"]=oldPercent;LoadSpacingSettings();
  const float expected=oldPercent==100.f?60.f:oldPercent;
  checkClose(ns::scale,expected*.01f,"only legacy100 migrates; custom settings retain exact value");
  check(spacingIni.count("VrNpcUnarmedSpacingVersion")==0,"migration itself remains in memory until normal save");
  SaveSpacingSettings();check(spacingIni.at("VrNpcUnarmedSpacingVersion")==1.f,"normal save records migration version");
  auto desired=ReadSpacingSettings();desired.npc_unarmed_spacing_percent=100.f;ApplySpacingSettings(desired);SaveSpacingSettings();LoadSpacingSettings();
  check(ns::scale==1.f,"explicit native100 after migration survives reload");
 }
 for(float version:{1.f,2.f,20.f}){spacingIni["VrNpcUnarmedSpacingVersion"]=version;spacingIni["VrNpcUnarmedSpacingPercent"]=100.f;LoadSpacingSettings();check(ns::scale==1.f,"current/future version preserves explicit native100");}
 spacingIni["VrNpcUnarmedSpacingVersion"]=1.f;spacingIni["VrNpcUnarmedSpacingPercent"]=100.f;LoadSpacingSettings();state=ReadSpacingSettings();
 check(state.npc_unarmed_spacing_percent==100.f&&ns::scale==1.f,"explicit versioned100 preserves native opt-out");
 const auto emptyMessages=diagnostic_trace::spacingMessages.size();tick();
 check(!ns::Ready()&&diagnostic_trace::spacingMessages.size()==emptyMessages,"default production control tick installs no hook or periodic log");
 for(float percent:{25.f,42.f,75.f,100.f})for(float difficulty:{0.f,60.f,100.f})for(float defense:{1.f,1.5f,3.f}){
  state.npc_unarmed_spacing_percent=percent;state.npc_difficulty_percent=difficulty;state.npc_defense_speed=defense;
  ApplySpacingSettings(state);SaveSpacingSettings();checkClose(spacingIni.at("VrNpcUnarmedSpacingPercent"),percent,"actual persisted key uses UI percent units");
  ns::scale=1.f;npc_difficulty::difficulty=0.f;npc_defense::speed=1.f;LoadSpacingSettings();auto loaded=ReadSpacingSettings();
  checkClose(loaded.npc_unarmed_spacing_percent,percent,"actual UI Apply/INI Save/INI Load/UI Read roundtrip");
  checkClose(loaded.npc_difficulty_percent,difficulty,"spacing preserves independent difficulty setting");
  checkClose(loaded.npc_defense_speed,defense,"spacing preserves independent defense-speed setting");
 }
 for(float bad:{std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity()}){
  state.npc_unarmed_spacing_percent=bad;ApplySpacingSettings(state);check(ns::scale==1.f,"invalid UI percent restores native rather than a poisoned setting");
  spacingIni["VrNpcUnarmedSpacingPercent"]=bad;LoadSpacingSettings();check(ns::scale==1.f,"invalid saved percent restores native");
 }
 for(auto pair:{std::pair{10.f,.25f},std::pair{200.f,1.f}}){state.npc_unarmed_spacing_percent=pair.first;ApplySpacingSettings(state);check(ns::scale==pair.second,"UI percent obeys explicit25 to100 percent range");spacingIni["VrNpcUnarmedSpacingPercent"]=pair.first;LoadSpacingSettings();check(ns::scale==pair.second,"INI percent uses the same bounded range");}
 state.npc_unarmed_spacing_percent=100.f;ApplySpacingSettings(state);tick();
 state.npc_unarmed_spacing_percent=50.f;ApplySpacingSettings(state);verified=false;tick();
 check(!ns::Ready()&&std::strcmp(ReadSpacingSettings().npc_unarmed_spacing_failure,"RETAIL_MISMATCH")==0,"actual installation refusal surfaces exact current failure in menu state");
 check(diagnostic_trace::spacingMessages.size()==emptyMessages+1&&diagnostic_trace::spacingMessages.back().find("RETAIL_MISMATCH")!=std::string::npos,"production activation reports concrete failure once");
 tick();check(diagnostic_trace::spacingMessages.size()==emptyMessages+1,"unchanged failure does not reinstall/log every frame");
 state.npc_unarmed_spacing_percent=25.f;ApplySpacingSettings(state);tick();check(diagnostic_trace::spacingMessages.size()==emptyMessages+2,"changed explicit request permits one new activation attempt");
 verified=true;tick();check(!ns::Ready()&&diagnostic_trace::spacingMessages.size()==emptyMessages+2,"external recovery alone does not silently repeat an unchanged failed install");
 state.npc_unarmed_spacing_percent=50.f;ApplySpacingSettings(state);tick();auto active=ReadSpacingSettings();
 check(active.npc_unarmed_spacing_ready&&std::strcmp(active.npc_unarmed_spacing_failure,"ACTIVE")==0,"actual saved/UI setting activates actual native consumer through production lifecycle block");
 auto applied=continuation(f);checkClose(applied.inner,.45f,"UI requested50 percent reaches actual production inner read view");checkClose(applied.outer,.85f,"same requested percent reaches outer read view coherently");
 state.npc_unarmed_spacing_percent=100.f;ApplySpacingSettings(state);SaveSpacingSettings();tick();
 const auto actors=actorCalls.load();check(continuation(f).loadedFrom==f.m()&&actorCalls==actors,"UI restoring100 percent immediately returns exact native pointer and fast path");
 ns::Shutdown();
 // A real new Run resets its local attempt state, while the saved setting and
 // the retained native hook backend may persist across Shutdown.
 state.npc_unarmed_spacing_percent=50.f;ApplySpacingSettings(state);SaveSpacingSettings();ns::scale=1.f;LoadSpacingSettings();
 npcSpacingAttempt=1.f;npcSpacingModule=0;tick();check(ns::Ready()&&ns::scale==.5f,"persisted nonnative50 activates at the first tick of a Run");
 ns::Shutdown();npcSpacingAttempt=1.f;npcSpacingModule=0;LoadSpacingSettings();tick();
 check(ns::Ready()&&ns::scale==.5f,"Shutdown then new Run with the same saved50 reactivates the retained hook");
 ns::Shutdown();state.npc_unarmed_spacing_percent=100.f;ApplySpacingSettings(state);npcSpacingAttempt=1.f;npcSpacingModule=0;
 const auto nativeRestartMessages=diagnostic_trace::spacingMessages.size();tick();
 check(!ns::Ready()&&diagnostic_trace::spacingMessages.size()==nativeRestartMessages,"new native100 Run leaves its hook disabled and does not log activation");
 state.npc_unarmed_spacing_percent=50.f;ApplySpacingSettings(state);npcSpacingAttempt=1.f;npcSpacingModule=0;currentModule=0;tick();
 check(!ns::Ready()&&std::strcmp(ns::Failure(),"MODULE_UNAVAILABLE")==0,"first request without WHGame reports actual module unavailability");
 const auto missingMessages=diagnostic_trace::spacingMessages.size();tick();check(diagnostic_trace::spacingMessages.size()==missingMessages,"unchanged absent module does not retry activation");
 currentModule=::image;tick();check(ns::Ready()&&diagnostic_trace::spacingMessages.size()==missingMessages+1,"real module-owner availability change retries once without altering saved scale");
 ns::Shutdown();currentModule=::image+0x1000;npcSpacingAttempt=1.f;npcSpacingModule=0;tick();
 check(!ns::Ready()&&std::strcmp(ns::Failure(),"RETAIL_MISMATCH")==0,"different unverified module owner cannot inherit a retained sampler hook");
 const auto foreignMessages=diagnostic_trace::spacingMessages.size();tick();check(diagnostic_trace::spacingMessages.size()==foreignMessages,"unchanged foreign owner remains quiet and native");
 alternateVerifiedModule=currentModule;npcSpacingAttempt=1.f;npcSpacingModule=0;tick();
 check(!ns::Ready()&&std::strcmp(ns::Failure(),"DIFFERENT_MODULE")==0,"even a separately fingerprint-verified new base cannot inherit the old retained hook backend");
 check(*reinterpret_cast<unsigned char*>(::image+ns::sampleLoads)==static_cast<unsigned char>(ns::pins[0].bytes[0]),"different verified module attempt never re-enables the previous module's hook");
 alternateVerifiedModule=0;
 currentModule=::image;tick();check(ns::Ready(),"return to the actual verified module is one distinct valid activation request");ns::Shutdown();
}
int main(){
 image=reinterpret_cast<Address>(VirtualAlloc(nullptr,0x5600000,0x3000,0x04));if(!image)return 2;
 for(const auto& pin:ns::pins)std::memcpy(reinterpret_cast<void*>(image+pin.rva),pin.bytes,pin.size);
 write(image+0x3FF1B18+0x250,image+0x730B30);write(image+0x3FE3F70+0x10,image+0x207C4A0);
 auto* entry=reinterpret_cast<unsigned char*>(image+ns::sampleLoads);const auto first=*entry;
 Fixture f{};f.reset();
 ns::scale=1.f;check(!ns::Install(image)&&!ns::Ready()&&*entry==first&&!ns::hook&&std::strcmp(ns::Failure(),"NATIVE_NO_HOOK")==0,"default native1 consumes no hook adapter and reports inactive native route");
 ns::scale=.5f;verified=false;check(!ns::Install(image)&&!ns::Ready()&&*entry==first,"retail mismatch never installs movement tuning");verified=true;
 for(const auto& pin:ns::pins){auto* byte=reinterpret_cast<unsigned char*>(image+pin.rva);const auto saved=*byte;*byte^=0xFF;check(!ns::Install(image)&&!ns::Ready()&&*entry==(pin.rva==ns::sampleLoads?static_cast<unsigned char>(first^0xFF):first),"each exact native instruction pin must match before installation");*byte=saved;}
 write(image+0x3FE3F70+0x10,Address{0});check(!ns::Install(image)&&!ns::Ready(),"foreign callback vtable cannot borrow the proven sampler");write(image+0x3FE3F70+0x10,image+0x207C4A0);
 DetourModKit::hook::hostFailEnableNpc63=true;check(!ns::Install(image)&&!ns::Ready(),"failed hook enable keeps ready false and native passthrough");DetourModKit::hook::hostFailEnableNpc63=false;
 check(ns::Install(image)&&ns::Ready()&&*entry==0xCC,"actual production install enables one pinned mid callback");
 ns::scale=1.f;auto beforeActor=actorCalls.load(),beforeClass=classCalls.load();auto native=continuation(f);
 check(native.loadedFrom==f.m()&&native.inner==1.5f&&native.outer==3.f&&native.activity==2.f,"native1 preserves original source and all loaded floats exactly");
 check(actorCalls==beforeActor&&classCalls==beforeClass,"native1 returns before any owner or native class getter work");
 auto originalMove=f.move;ns::scale=.5f;auto tuned=continuation(f);
 check(tuned.loadedFrom!=f.m()&&tuned.activity==2.f,"only selected NPC unarmed base pair uses a read view; activity stays native");
 checkClose(tuned.inner,.75f,"inner native base scaled once");checkClose(tuned.outer,1.5f,"outer native base scaled coherently");
 check(f.move==originalMove,"native profile, identity, target, activity and every other move byte remain unchanged");
 for(int i=0;i<8;++i){auto again=continuation(f);check(again.inner==.75f&&again.outer==1.5f&&f.move==originalMove,"native profile early-return/repeated sampler reads cannot compound tuning");}
 f.profile(.9f,1.7f,6.f);auto fresh=continuation(f,.2f,.3f);checkClose(fresh.inner,.95f,"fresh native RPG/weapon/perk inner is used and native terrain/tactical deltas remain unscaled");checkClose(fresh.outer,1.35f,"fresh outer with same native deltas preserves coherent native ring");checkClose(fresh.activity,6.2f,"fresh native activity unaffected by scale");
 auto overridden=continuation(f,.2f,.3f,true);checkClose(overridden.inner,4.3f,"native tactical range override remains authoritative");checkClose(overridden.outer,5.3f,"native outer override is not secretly scaled afterward");
 auto priorCalls=nativeCalls.load();auto declined=continuation(f,0,0,false,true);check(declined.inner==91.f&&declined.outer==92.f&&nativeCalls==priorCalls+1,"native refused sampler never enters tuning or replaces another callback's outputs");
 for(float factor:{.25f,.6f,.7f,1.5f,2.f}){ns::scale=factor;auto value=continuation(f);checkClose(value.inner,.9f*factor,"positive scale always uses original current inner");checkClose(value.outer,1.7f*factor,"inner/outer ordering scales coherently without compounding");check(value.inner<=value.outer,"ordered native ring remains ordered");}
 ns::scale=.5f;
 write(currentRow+8,std::uint32_t{4});check(continuation(f).loadedFrom==f.m(),"same current NPC switching to an armed effective class is immediately native");write(currentRow+8,std::uint32_t{12});check(continuation(f).loadedFrom!=f.m(),"current native unarmed class switching back needs no stale per-actor lease");
 write(f.a(),image+0x3A80610);check(continuation(f).loadedFrom==f.m(),"Henry CombatPlayer never receives NPC spacing");write(f.a(),image+0x3E3FAC8);
 write(f.n(),image+0x3FF1B20);check(continuation(f).loadedFrom==f.m(),"foreign XGen subtype uses native movement");write(f.n(),image+0x3FF1B18);
 write(f.n()+0x640,Address{0});check(continuation(f).loadedFrom==f.m(),"retired move not current owner cannot reuse NPC identity");write(f.n()+0x640,f.m());
 f.capture[1]=f.n()+16;check(continuation(f).loadedFrom==f.m(),"controlled/alternate captured actor is not misidentified as owner NPC");f.capture[1]=f.n();
 f.capture[0]=0;check(continuation(f).loadedFrom==f.m(),"foreign capture cannot borrow current source");f.capture[0]=f.m();
 write(f.e()+8,Address{0});check(continuation(f).loadedFrom==f.m(),"stale equipment owner returns native; it never suppresses the sampler");write(f.e()+8,f.a());
 auto ctx=f.context();DetourModKit::hook::gpr(ctx,DetourModKit::hook::Gpr::Rax)=0;ns::Sample(ctx);check(DetourModKit::hook::gpr(ctx,DetourModKit::hook::Gpr::Rax)==0,"invalid tuning source is not replaced with invented native motion");
 for(float factor:{0.f,-1.f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}){ns::scale=factor;check(continuation(f).loadedFrom==f.m(),"invalid setting is native passthrough without poisoning game floats");}
 ns::scale=.5f;f.profile(0.f,0.f,2.f);auto zero=continuation(f);check(zero.inner==0.f&&zero.outer==0.f,"native zero ring readiness remains zero");
 f.profile(std::numeric_limits<float>::infinity(),3.f,2.f);check(continuation(f).loadedFrom==f.m(),"nonfinite native profile remains exactly native");f.profile(.9f,1.7f,6.f);
 std::atomic<unsigned> arrived{0};Address threadView[2]{};float threadInner[2]{};
 auto threaded=[&](unsigned side){auto context=f.context();ns::Sample(context);threadView[side]=DetourModKit::hook::gpr(context,DetourModKit::hook::Gpr::Rax);arrived.fetch_add(1);while(arrived.load()<2)std::this_thread::yield();threadInner[side]=read<float>(threadView[side]+0x2C);};
 std::thread one([&]{threaded(0);}),two([&]{threaded(1);});one.join();two.join();
 check(threadView[0]!=f.m()&&threadView[1]!=f.m()&&threadView[0]!=threadView[1]&&threadInner[0]==.45f&&threadInner[1]==.45f,"native readers on separate threads have independent immediate read views");
 auto inFlight=f.context();ns::Sample(inFlight);const Address retained=DetourModKit::hook::gpr(inFlight,DetourModKit::hook::Gpr::Rax);
 ns::Shutdown();check(!ns::Ready()&&*entry==first,"shutdown restores entry before further tuning callbacks");check(read<float>(retained+0x2C)==.45f,"already-entered native scalar loads retain a valid view through shutdown");check(continuation(f).loadedFrom==f.m(),"callback retained after shutdown is native passthrough");
 check(ns::Install(image)&&ns::Ready(),"same pinned retained hook re-enables without replacing its backend");ns::scale=1.f;check(continuation(f).loadedFrom==f.m(),"setting1 restores native route without constructor/default replay");
 DetourModKit::hook::hostFailDisable63=true;ns::Shutdown();check(!ns::Ready()&&continuation(f).loadedFrom==f.m(),"failed native disarm still leaves retained callback native passthrough");DetourModKit::hook::hostFailDisable63=false;ns::Shutdown();
 SettingsAndActivation(f);
 std::printf("npc_unarmed_spacing=%u/%u actualProduction=MID_INSTALL_CURRENT_OWNER_FRESH_READVIEW_UI_INI_LAZY_ACTIVATION nativeContinuation=EXPLICIT_HOST nativeProfileWrites=NONE headset=NOT_RUN\n",checks-failed,checks);
 return failed?1:0;
}
