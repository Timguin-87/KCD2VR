// Actual native-return adapter and owner admission; retail execution substituted.
#define NOMINMAX
#include <windows.h>
#include <array>
#include <atomic>
#include <optional>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include "core/player_window_h26.h"
static unsigned checks=0,calls=0;
static void ck(bool b,const char* why){++checks;if(!b){std::fprintf(stderr,"FAIL %u %s\n",checks,why);std::exit(2);}}
static std::uintptr_t base=0x180000000ull;static std::uint64_t now=1000;
static bool retail=true,experiment=true,reference=false,resolve=true;
static std::atomic<bool> g_gameplay_allowed{true},g_head_valid{true},g_left_valid{true},g_right_valid{true};
static std::atomic<bool> g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false};
static std::atomic<bool> g_ctx_reading{false},g_phys_fault6698{false},g_fist_fault101{false};
static std::atomic<std::uint64_t> g_fist_epoch101{3},g_weapon_request_epoch6694{3};
static bool fist=false,activity=false;
namespace kcd2vr::workstation178r20c1{static bool ownsActivityWrist45(std::uint64_t){return activity;}}
struct FistFrame101{void* player=nullptr;std::uint64_t request=3;struct Sample{std::uint64_t epoch=7,tick=1000;bool valid=true;}sample;};
struct PhysFrame6698{void* player=nullptr;std::uint64_t request=3;struct Input{bool allowed=true;std::uint64_t epoch=3,bodyEpoch=7,tick=1000;}input;
 struct Profile{bool isMelee=true;std::uint64_t key=123,generation=4;bool melee()const{return isMelee;}};std::array<Profile,2> profiles;};
template<class T>struct Mailbox{T value{};bool readable=true;bool read(T& out){out=value;return readable;}};
static Mailbox<PhysFrame6698> g_phys_frame6698;static Mailbox<FistFrame101> g_fist_frame101;
static bool FistSelected101(){return fist;}
static std::atomic<std::uint64_t> g_head_valid_tick{1000},g_left_valid_tick{1000},g_right_valid_tick{1000},g_pose_epoch6689{7};
namespace animation_filter{static std::atomic<bool> nativeBodyOwned112{false};}
namespace retail_build112{static bool Ready(std::uintptr_t){return retail;}static bool Verify(std::uintptr_t){return retail;}}
namespace diagnostics{namespace lab{static bool NativeReference(){return reference;}}
namespace capture168{enum class Domain{Combat};static bool active(Domain,std::uint64_t){return false;}}}
namespace diagnostic_trace{static void Write(const char*){}}
static std::uintptr_t WhBase(){return base;}static bool CombatExperiment175Enabled(){return experiment;}
struct NativeOwner104{void* player=nullptr;void* entity=nullptr;void* actor=nullptr;void* state=nullptr;void* soul=nullptr;std::uint64_t epoch=0;};
static NativeOwner104 live{};
static std::array<unsigned char,0x40> handler{};
static std::array<unsigned char,0x80> manager{};
static std::array<unsigned char,0x220> actor{},enemy{};
static std::array<unsigned char,0xD10> state{},enemyState{};
static std::array<unsigned char,0x10> player{};
static std::atomic<void*> g_henry_item_attach_manager{nullptr};
template<class T>static void put(void* p,unsigned off,T value){std::memcpy(static_cast<unsigned char*>(p)+off,&value,sizeof(value));}
template<class T>static bool Read(const void* p,T& out){if(!p)return false;std::memcpy(&out,p,sizeof(out));return true;}
static bool PhysVft6698(void* p,std::uintptr_t r){std::uintptr_t v=0;return Read(p,v)&&v==base+r;}
static bool NativeResolve104(void* p,NativeOwner104& out){out={};if(!resolve||p!=live.player)return false;out=live;return true;}
static float nativeDuration=.2f;static void(*during)()=nullptr;static bool throws=false;
static float __fastcall Original(void* h,void* a){++calls;ck(h==handler.data()&&a==enemy.data(),"original receives unchanged native args");if(during){auto f=during;during=nullptr;f();}if(throws)throw std::runtime_error("native-leaf-substitute");return nativeDuration;}
namespace DetourModKit{struct Address{std::uintptr_t value;};namespace hook{
struct Options{bool fail_if_already_hooked=false;};struct Target{Address address;};struct InlineRequest{const char* name;Target target;Options options;};
struct Hook{bool enabled=true;template<class T>T original(){return reinterpret_cast<T>(&Original);}bool enable(){enabled=true;return true;}bool is_enabled()const{return enabled;}bool disable(){enabled=false;return true;}};
template<class F>static std::optional<Hook> inline_at(InlineRequest,F){return Hook{};}
}}
static std::uint64_t Clock(){return now;}
#define GetTickCount64 Clock
#include "../src/vrik/player_window_h26.inl"
#undef GetTickCount64
static void reset(unsigned kind=1){
 handler={};manager={};actor={};enemy={};state={};enemyState={};player={};live={};
 calls=0;now=1000;nativeDuration=.2f;during=nullptr;throws=false;retail=experiment=resolve=true;reference=false;
 g_gameplay_allowed=g_head_valid=g_left_valid=g_right_valid=true;
 g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=false;animation_filter::nativeBodyOwned112=false;
 g_ctx_reading=g_phys_fault6698=g_fist_fault101=false;fist=activity=false;g_fist_epoch101=g_weapon_request_epoch6694=3;
 g_head_valid_tick=g_left_valid_tick=g_right_valid_tick=1000;g_pose_epoch6689=7;
 live.player=player.data();live.actor=actor.data();live.state=state.data();live.epoch=7;
 g_phys_frame6698={};g_fist_frame101={};g_phys_frame6698.value.player=g_fist_frame101.value.player=player.data();
 put(handler.data(),0,base+(kind==1?0x3AECEA0:0x3AECE20));put(handler.data(),8,static_cast<void*>(actor.data()));
 put(manager.data(),0,base+0x3EC9958);put(manager.data(),0x38,static_cast<void*>(player.data()));g_henry_item_attach_manager=manager.data();
 put(state.data(),0xCF8,static_cast<void*>(enemy.data()));put(enemy.data(),0,base+0x3E3FAC8);
 put(enemy.data(),0x210,static_cast<void*>(enemyState.data()));put(enemyState.data(),0xCE0,static_cast<void*>(enemy.data()));
 put(enemyState.data(),0xCF8,static_cast<void*>(actor.data()));g_player_window_readyH26=true;g_player_window_retiredH26=false;
 g_player_window_originalH26=&Original;g_player_window_hookH26.reset();kcd2vr::player_window_h26::multiplier=1.5f;kcd2vr::player_window_h26::perfectBlockMultiplier=1.5f;
}
static float invoke(){return PlayerWindowDetourH26(handler.data(),enemy.data());}
int main(){
 for(unsigned kind:{1u,5u}){reset(kind);ck(std::fabs(invoke()-.3f)<.000001f&&calls==1,"exact Henry PB/MS gets150% allowance, original once");
  nativeDuration=2;ck(invoke()==(kind==1?3.f:2.5f),"PB uses requested exact scale; MS retains existing extra500ms policy");
  for(float value:{0.f,-1.f,-.1f,INFINITY,-INFINITY,NAN}){nativeDuration=value;const float actual=invoke();ck((std::isnan(value)&&std::isnan(actual))||actual==value,"zero/negative/sentinel/nonfinite native duration preserved");}}
 reset(1);kcd2vr::player_window_h26::perfectBlockMultiplier=2.f;
 ck(std::fabs(invoke()-.4f)<.000001f&&calls==1,"native Henry PB allowance doubles with original called once");
 reset(5);kcd2vr::player_window_h26::perfectBlockMultiplier=2.f;
 ck(std::fabs(invoke()-.3f)<.000001f&&calls==1,"PB setting never changes native MS allowance");
 for(unsigned bad=0;bad<38;++bad){reset();
  if(bad==0)retail=false;if(bad==1)experiment=false;if(bad==2)reference=true;if(bad==3)g_gameplay_allowed=false;
  if(bad==4)g_head_valid=false;if(bad==5)g_left_valid=false;if(bad==6)g_right_valid=false;
  if(bad==7)g_ctx_menu=true;if(bad==8)g_ctx_dialogue=true;if(bad==9)g_ctx_cutscene=true;if(bad==10)g_ctx_scripted=true;
  if(bad==11)animation_filter::nativeBodyOwned112=true;if(bad==12)g_head_valid_tick=939;if(bad==13)g_left_valid_tick=919;
  if(bad==14)g_right_valid_tick=1001;if(bad==15)put(handler.data(),0,base+0x3AECD20); // ordinary block
  if(bad==16)put(handler.data(),8,static_cast<void*>(enemy.data())); // NPC-owned handler
  if(bad==17)g_henry_item_attach_manager=nullptr;if(bad==18)put(manager.data(),0,base+123);
  if(bad==19)resolve=false;if(bad==20)++live.epoch;if(bad==21)put(state.data(),0xCF8,static_cast<void*>(actor.data()));
  if(bad==22)put(enemy.data(),0,base+123);if(bad==23)put(enemyState.data(),0xCE0,static_cast<void*>(actor.data()));
  if(bad==24)put(enemyState.data(),0xCF8,static_cast<void*>(enemy.data()));
  if(bad==25)g_player_window_readyH26=false;if(bad==26)g_player_window_retiredH26=true;if(bad==27){kcd2vr::player_window_h26::multiplier=3;kcd2vr::player_window_h26::perfectBlockMultiplier=3;}
  if(bad==28)g_phys_frame6698.value.profiles[0].isMelee=false; // bow/ranged/unknown
  if(bad==29)g_phys_frame6698.value.input.allowed=false;if(bad==30)g_phys_frame6698.value.input.tick=919;
  if(bad==31)++g_weapon_request_epoch6694;if(bad==32)g_phys_frame6698.value.player=enemy.data();
  if(bad==33)activity=true;if(bad==34)g_ctx_reading=true;if(bad==35)g_phys_fault6698=true;
  if(bad==36){fist=true;g_fist_frame101.value.sample.valid=false;}if(bad==37){fist=true;++g_fist_epoch101;}
  ck(invoke()==.2f&&calls==1,"unknown/reference/NPC/foreign-kind/stale/ownership-loss remains original once");
 }
 reset();fist=true;ck(std::fabs(invoke()-.3f)<.000001f&&calls==1,"current owned fist frame also receives native allowance adaptation");
 for(unsigned revoke=0;revoke<9;++revoke){reset();
  if(revoke==0)during=[](){++g_pose_epoch6689;};if(revoke==1)during=[](){g_ctx_dialogue=true;};
  if(revoke==2)during=[](){put(handler.data(),8,static_cast<void*>(enemy.data()));};
  if(revoke==3)during=[](){put(handler.data(),0,base+0x3AECE20);};
  if(revoke==4)during=[](){live.state=enemyState.data();};if(revoke==5)during=[](){g_player_window_readyH26=false;};
  if(revoke==6)during=[](){++g_weapon_request_epoch6694;++g_phys_frame6698.value.request;++g_phys_frame6698.value.input.epoch;};
  if(revoke==7)during=[](){++g_phys_frame6698.value.profiles[0].key;};
  if(revoke==8)during=[](){++g_phys_frame6698.value.profiles[0].generation;};
  ck(invoke()==.2f&&calls==1,"native synchronous takeover before return prevents widening");
 }
 reset();throws=true;bool caught=false;try{(void)invoke();}catch(const std::runtime_error&){caught=true;}
 ck(caught&&calls==1,"native exception propagates without repeat or invented result");
 reset();g_player_window_hookH26.emplace();ShutdownPlayerWindowH26();ck(!g_player_window_readyH26&&g_player_window_retiredH26&&g_player_window_hookH26.has_value(),"shutdown retains callback trampoline and stops modification");
 ck(invoke()==.2f&&calls==1,"retired callback safely forwards original once");
 ck(!ArmPlayerWindowH26(),"retired hook cannot accidentally reinstall");
 std::printf("player_window_h26: %u checks PASS; actual return adapter/owner admission; native acceptance UNCLAIMED\n",checks);
}
