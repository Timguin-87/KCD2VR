// Production receiver/pin preflight, with explicit read-only memory substitutes.
// No native code or the positive queue leaf executes in this host test.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <map>
#include <vector>
#include <cassert>
#ifndef _MSC_VER
#define __fastcall
#define __try try
#define __except(x) catch(...)
#endif
#define EXCEPTION_EXECUTE_HANDLER 1
static std::uintptr_t g_base=0x180000000ull;
static bool fingerprint=true;
namespace retail_build112 { bool Ready(std::uintptr_t){return fingerprint;} }
static std::map<std::uintptr_t,std::vector<unsigned char>> memory;
static bool SehCopy146(const void* p,void* out,std::size_t n){
 auto it=memory.find(reinterpret_cast<std::uintptr_t>(p));
 if(it==memory.end()||it->second.size()<n)return false;
 std::memcpy(out,it->second.data(),n);return true;
}
template<class T> T Rd(const void* p,std::size_t o,T fallback){
 T v{};return p&&SehCopy146(reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(p)+o),&v,sizeof(v))?v:fallback;
}
static bool EntryIs(std::uintptr_t r,const unsigned char* wanted,std::size_t n){
 std::vector<unsigned char> b(n);return SehCopy146(reinterpret_cast<const void*>(g_base+r),b.data(),n)&&!std::memcmp(b.data(),wanted,n);
}
#include "../src/diagnostics/game_hit_event5910c1.inl"
template<class T> void put(std::uintptr_t at,T value){
 auto& b=memory[at];b.resize(sizeof(value));std::memcpy(b.data(),&value,sizeof(value));
}
int main(){
 auto pin=[](std::uintptr_t r,const unsigned char* p,std::size_t n){memory[g_base+r]={p,p+n};};
 pin(kQueueHit5910c1,kQueueHitEntry5910c1,sizeof(kQueueHitEntry5910c1));
 pin(0x9155F1,kHitGlobalRead5910c1,sizeof(kHitGlobalRead5910c1));
 pin(0xC526BC,kNativeHitEventCall5910c1,sizeof(kNativeHitEventCall5910c1));
 constexpr std::uintptr_t service=0x10000,rules=0x20000;
 put(g_base+kGameGlobal5910c1,service);put(service+0x1A0,rules);
 put(rules,g_base+kGameRulesVft5910c1);put(g_base+kGameRulesVft5910c1+0x528,g_base+kQueueHit5910c1);
 std::uintptr_t v[3]{0x30000,0x300A0,0x30140};
 memory[rules+0x1D8].resize(sizeof(v));std::memcpy(memory[rules+0x1D8].data(),v,sizeof(v));
 assert(HitEventReceiver5910c1()==reinterpret_cast<void*>(rules));
 fingerprint=false;assert(!HitEventReceiver5910c1());fingerprint=true;
 put(rules,g_base+kGameRulesVft5910c1+0x40);assert(!HitEventReceiver5910c1());put(rules,g_base+kGameRulesVft5910c1);
 put(g_base+kGameRulesVft5910c1+0x528,g_base+kQueueHit5910c1+1);assert(!HitEventReceiver5910c1());
 put(g_base+kGameRulesVft5910c1+0x528,g_base+kQueueHit5910c1);
 memory[rules+0x1D8][8]++;assert(!HitEventReceiver5910c1());memory[rules+0x1D8][8]--;
 memory.erase(rules+0x1D8);assert(!HitEventReceiver5910c1());
 assert(!QueueHitEvent5910c1(nullptr,nullptr));
 put(service+0x1A0,std::uintptr_t(0));assert(!HitEventReceiver5910c1());
}
