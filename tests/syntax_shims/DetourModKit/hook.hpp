#pragma once
#include <optional>
#include <cstdint>
#include <string>
#include <array>
#include <cstring>
#include <vector>
namespace DetourModKit {struct Address{std::uintptr_t value;}; namespace hook {
struct Target{Address address;};struct Options{bool fail_if_already_hooked=false;};struct InlineRequest{std::string name;Target target;Options options;};
#if defined(KCD2VR_GRASP_HOST_TEST)
// Grasp owner teardown fixture only: model a failed disarm that remains live.
// No native patch/trampoline is made by these host objects.
struct Hook{
 bool live=true,failDisable=false,released=false,retainedLive=false;
 template<class T>T original(){return nullptr;}
 bool enable(){live=true;return true;}
 bool disable(){if(failDisable)return false;live=false;return true;}
 bool is_enabled()const{return live;}
 void release() noexcept {retainedLive=live;released=true;live=false;}
};
template<class F>std::optional<Hook> inline_at(InlineRequest,F){return Hook{};}
#elif defined(KCD2VR_TEST_HOOK_SPY115)
// Test-owned memory patch/original counters. No instructions are executed.
inline std::uintptr_t (*hostOriginal115)(const InlineRequest&)=nullptr;
inline unsigned hostCollisionInstalls115=0;
#if defined(KCD2VR_H36R3_HOOK_TEST)
inline bool hostFailCreateH36r3=false,hostFailEnableH36r3=false,hostFailDisableH36r3=false,hostNoOriginalH36r3=false;
inline bool hostFailEnableMidH36r3=false,hostFailDisableMidH36r3=false;
inline std::uintptr_t hostAddressH36r3=0,hostMidAddressH36r3=0;
#endif
#if defined(KCD2VR_RENDER_BOUNDS_TEST63)
inline bool hostFailEnableBounds63=false,hostFailDisableBounds63=false;
#endif
#if defined(KCD2VR_NPC_LOOK_TEST63)
inline bool hostFailDisable63=false,hostFailEnableNpc63=false;
#endif
struct RetainedHook115{std::uintptr_t address=0,fn=0;unsigned char saved=0;bool live=false;};
inline std::vector<RetainedHook115> hostRetainedHooks115;
struct Hook{
 std::uintptr_t address=0,fn=0;unsigned char saved=0;bool live=false;
 template<class T>T original(){return reinterpret_cast<T>(fn);}
 bool enable(){if(!address)return false;if(!live){auto*p=reinterpret_cast<unsigned char*>(address);saved=*p;*p=0xCC;live=true;}
#if defined(KCD2VR_H36R3_HOOK_TEST)
  if((address==hostAddressH36r3&&hostFailEnableH36r3)||(address==hostMidAddressH36r3&&hostFailEnableMidH36r3))return false;
#endif
#if defined(KCD2VR_RENDER_BOUNDS_TEST63)
  if(hostFailEnableBounds63)return false;
#endif
#if defined(KCD2VR_NPC_LOOK_TEST63)
  if(hostFailEnableNpc63)return false;
#endif
  return true;}
 bool disable(){
#if defined(KCD2VR_H36R3_HOOK_TEST)
  if((address==hostAddressH36r3&&hostFailDisableH36r3)||(address==hostMidAddressH36r3&&hostFailDisableMidH36r3))return false;
#endif
#if defined(KCD2VR_RENDER_BOUNDS_TEST63)
  if(hostFailDisableBounds63)return false;
#endif
#if defined(KCD2VR_NPC_LOOK_TEST63)
  if(hostFailDisable63)return false;
#endif
  if(live){*reinterpret_cast<unsigned char*>(address)=saved;live=false;}return true;}
 bool is_enabled()const{return live;}
 // Installed DMK release disengages the handle without unpatching or freeing
 // its backend. Preserve the test-owned patch/original in a process-live
 // record; resetting the detached handle must not discard that record.
 void release(){if(address)hostRetainedHooks115.push_back({address,fn,saved,live});address=fn=0;live=false;}
};
template<class F>std::optional<Hook> inline_at(InlineRequest r,F){
 if(r.name=="kcd2vr_v6689_combat_collision")++hostCollisionInstalls115;
#if defined(KCD2VR_H36R3_HOOK_TEST)
 if(r.name=="kcd2vr_h36r3_original_draw_token")hostMidAddressH36r3=r.target.address.value;
 if(r.name=="kcd2vr_h36r3_original_left_draw_completion"){
  hostAddressH36r3=r.target.address.value;
  if(hostFailCreateH36r3)return std::nullopt;
  if(hostNoOriginalH36r3)return Hook{r.target.address.value,0};
 }
#endif
 return Hook{r.target.address.value,hostOriginal115?hostOriginal115(r):0};
}
#else
struct Hook{template<class T>T original(){return nullptr;}bool enable(){return true;}bool disable(){return true;}bool is_enabled()const{return false;}void release() noexcept {}};
template<class F>std::optional<Hook> inline_at(InlineRequest,F){return Hook{};}
#endif
}}

namespace DetourModKit::hook {
// Host-only synthetic register bank, NOT the opaque production DMK layout.
enum class Gpr {Rdi,R12,Rax,Rbx,R15,Rcx,Rdx,R8,R9,Rsi,Rbp,R14};
struct XmmView{std::array<unsigned char,16> bytes{};template<class T>T lane(std::size_t i)const noexcept{T v{};if(i<16/sizeof(T))std::memcpy(&v,bytes.data()+i*sizeof(T),sizeof(T));return v;}};
struct MidContext{std::uintptr_t registers[12]{},stack=0,ip=0;XmmView vectors[16]{};};
inline std::uintptr_t& instruction_pointer(MidContext& c) noexcept{return c.ip;}
inline XmmView xmm(const MidContext& c,std::size_t i) noexcept{return i<16?c.vectors[i]:XmmView{};}
inline std::uintptr_t& gpr(MidContext& c,Gpr r) noexcept{return c.registers[static_cast<unsigned>(r)];}
inline std::uintptr_t stack_pointer(const MidContext& c) noexcept{return c.stack;}
using MidRequest=InlineRequest;
template<class F>std::optional<Hook> mid_at(MidRequest r,F f){return inline_at(r,f);}
}
