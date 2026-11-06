// Actual sound-consumer source; only native memory/registry/VM leaves substituted.
#include "subsystems/alchemy/instant_native_d72.h"
#include <atomic>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <unordered_map>
namespace kcd2vr::activity_input163 {enum class Kind175{Alchemy};struct Witness{std::atomic<bool>ready{true};std::atomic<unsigned>transitionDepth175{0};std::atomic<std::uint64_t>lifetime175{3};std::array<std::atomic<std::uintptr_t>,1>active{};}witness175;}
namespace kcd2vr::diagnostics::capture168 {enum class Domain{Interaction};bool active(Domain,std::uint64_t){return false;}}
namespace diagnostic_trace {void Write(const char*)noexcept{}}
namespace retail_build112 {bool valid=true;bool Ready(std::uintptr_t){return valid;}}
static std::atomic<std::uintptr_t>g_base{0x180000000};static std::atomic<bool>g_alchemy_instant_stop_d72{false};
static std::atomic<void*>g_henry_ctrl{reinterpret_cast<void*>(0x4000)};
static constexpr unsigned kCtrlEntityId=0x30;
static std::unordered_map<std::uintptr_t,unsigned char>memory;
template<class T>void put(std::uintptr_t at,T v){auto*p=reinterpret_cast<unsigned char*>(&v);for(unsigned i=0;i<sizeof(T);++i)memory[at+i]=p[i];}
template<class T>T Rd(const void*p,std::size_t off,T fallback){T v{};auto*b=reinterpret_cast<unsigned char*>(&v);auto at=reinterpret_cast<std::uintptr_t>(p)+off;for(unsigned i=0;i<sizeof(T);++i){auto x=memory.find(at+i);if(x==memory.end())return fallback;b[i]=x->second;}return v;}
static bool Vft13(const void*p,std::uintptr_t rva){return p&&Rd<std::uintptr_t>(p,0,0)==g_base+rva;}
static void*registry=reinterpret_cast<void*>(0x5000);
static void*Entity13(unsigned id){return id==42?registry:nullptr;}
static bool IsHenryController(void*p){return p==g_henry_ctrl;}
namespace kcd2vr::native_script_t2 {bool current=true;unsigned calls=0;bool result=true;char packet[192]{};bool Context::Current()const noexcept{return current;}bool Context::CallPacket(const char*n,const char*i,char*o,std::size_t cap)const noexcept{++calls;assert(!std::strcmp(n,"kcd2vr_alchemy_sound_d80"));std::snprintf(packet,sizeof(packet),"%s",i);std::snprintf(o,cap,"A80|1|DISPATCHED");return result;}}
using kcd2vr::native_script_t2::Context;
namespace native_script_t2=kcd2vr::native_script_t2;
#include "../src/subsystems/alchemy/sound_service_d80.inl"
namespace f=kcd2vr::subsystems::alchemy::feedback_d80;
static void reset(){memory.clear();registry=reinterpret_cast<void*>(0x5000);g_henry_ctrl=reinterpret_cast<void*>(0x4000);g_alchemy_instant_stop_d72=false;retail_build112::valid=true;native_script_t2::current=true;native_script_t2::result=true;native_script_t2::calls=0;auto&w=kcd2vr::activity_input163::witness175;w.ready=true;w.transitionDepth175=0;w.lifetime175=3;w.active[0]=0x1000;
 put(0x1000,g_base.load()+0x3F60B08);put(0x102C,(unsigned char)1);put(0x1018,std::uintptr_t(0x2000));put(0x2000,g_base.load()+0x3AA4B98);put(0x2038,std::uintptr_t(0x5000));put(0x2030,unsigned(42));put(0x5000,g_base.load()+0x4072C78);put(0x500C,unsigned(42));put(0x4030,unsigned(42));}
int main(){f::Event event{0x1000,0x2000,0x4000,3,1,1000,f::Cue::HerbToPot};Context ctx{1,2,1010,4};reset();AlchemyFeedbackServiceD80(ctx,event);assert(native_script_t2::calls==1&&!std::strcmp(native_script_t2::packet,"A80|4|3|1|42|5"));
 for(int fault=0;fault<13;++fault){reset();auto e=event;auto c=ctx;switch(fault){case 0:c.now=1251;break;case 1:c.now=999;break;case 2:native_script_t2::current=false;break;case 3:g_alchemy_instant_stop_d72=true;break;case 4:retail_build112::valid=false;break;case 5:kcd2vr::activity_input163::witness175.lifetime175=4;break;case 6:kcd2vr::activity_input163::witness175.active[0]=0x9999;break;case 7:g_henry_ctrl=reinterpret_cast<void*>(0x9000);break;case 8:put(0x1018,std::uintptr_t(0x9999));break;case 9:put(0x500C,unsigned(43));break;case 10:registry=reinterpret_cast<void*>(0x6000);break;case 11:put(0x4030,unsigned(43));break;case 12:kcd2vr::activity_input163::witness175.transitionDepth175=1;break;}AlchemyFeedbackServiceD80(c,e);assert(native_script_t2::calls==0);}
 reset();f::consumer=&AlchemyFeedbackServiceD80;f::transportReady=true;assert(f::Offer(event));native_script_t2::result=false;f::Service(ctx);f::Service(ctx);assert(native_script_t2::calls==1&&!f::Wanted());
 std::puts("D80 actual native sound receiver/lifetime/VM adapter PASS; audio/registry leaves substituted");}
