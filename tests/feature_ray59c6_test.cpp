// Complete production read-only ray decoder, with explicit memory/query spies.
#include "vrik/npc_contact172.h"
#include "vrik/world_query_contract.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <cassert>
#include <cstdio>
namespace npc172=kcd2vr::npc_contact172;namespace wc=kcd2vr::world_query_contract;
struct V3{float x=0,y=0,z=0;};
V3 Add(V3 a,V3 b){return {a.x+b.x,a.y+b.y,a.z+b.z};}V3 Sub(V3 a,V3 b){return {a.x-b.x,a.y-b.y,a.z-b.z};}V3 Mul(V3 a,float s){return {a.x*s,a.y*s,a.z*s};}float Len(V3 a){return std::sqrt(a.x*a.x+a.y*a.y+a.z*a.z);}
static std::map<std::uintptr_t,std::vector<unsigned char>> memory;
template<class T> void put(std::uintptr_t addr,T v){auto& b=memory[addr];b.resize(sizeof(v));std::memcpy(b.data(),&v,sizeof(v));}
template<class T> bool Read(void* p,T& v){auto i=memory.find(reinterpret_cast<std::uintptr_t>(p));if(i==memory.end()||i->second.size()!=sizeof(v))return false;std::memcpy(&v,i->second.data(),sizeof(v));return true;}
std::uintptr_t WhBase(){return 0x10000000;}std::atomic<int> g_collision_binding6692{1};
using RayWorldIntersectionFn6692=void*;
static unsigned nativeCalls=0;static int returned=1;static bool invalidPlane=false;static std::vector<std::string> logs;
namespace diagnostic_trace{void Write(const char* s)noexcept{logs.emplace_back(s);}}
int RayWorldGuarded6692(RayWorldIntersectionFn6692,void*,const V3* from,const V3* delta,void* dst,bool bodies,void** skip,int count)noexcept{
 ++nativeCalls;assert(bodies&&count==1&&skip[0]==reinterpret_cast<void*>(0x999));if(returned!=1)return returned;
 auto bytes=static_cast<unsigned char*>(dst);const float distance=Len(*delta)*.5f;const auto point=invalidPlane?V3{100,100,100}:Add(*from,Mul(*delta,.5f));const V3 normal{0,0,1};
 const std::uintptr_t body=0x30000000;const int part=2;const std::int16_t surface=104;
 std::memcpy(bytes,&distance,4);std::memcpy(bytes+8,&body,8);std::memcpy(bytes+0x14,&part,4);std::memcpy(bytes+0x18,&surface,2);std::memcpy(bytes+0x24,&point,12);std::memcpy(bytes+0x30,&normal,12);return 1;
}
#include "generated_feature_ray59c6.inc"
int main(){unsigned n=0;auto ck=[&](bool b,const char*t){++n;std::printf("%s %s\n",b?"PASS":"FAIL",t);assert(b);};
 const auto base=WhBase();put(base+wc::kWorldSlotRva,reinterpret_cast<void*>(0x20000000));put(0x20000000,base+wc::kWorldVtableRva);put(base+wc::kWorldVtableRva+wc::kQueryVtableOffset,base+wc::kQueryMethodRva);
 put(0x30000000,base+0x3a48b30);put(base+0x3a48b30+0x50,base+0x5D6F44);put(0x30000020,reinterpret_cast<void*>(0x40000000));put(0x40000000,base+0x4072C78);put(0x4000000C,std::uint32_t(42));
 npc172::Exclusions skip;skip.add(reinterpret_cast<void*>(0x888),reinterpret_cast<void*>(0x999));
 auto q=Ray({0,0,0},{.1f,0,0},skip);ck(!std::strcmp(q.status,"HIT")&&q.witness.entityId==42&&q.witness.part==2&&q.witness.body==npc172::Body::Articulated&&q.witness.surfaceIdx==104,"exact ray decoder retains part/surface/owner");
 ck(std::abs(q.point.x-.05)<1e-6&&q.witness.collider==0x30000000,"contact point checked against THIS source segment");
 returned=0;q=Ray({},{.1f,0,0},skip);ck(!std::strcmp(q.status,"CLEAR")&&q.witness.entityId==0,"empty native ray is not an invented hit");
 returned=1;invalidPlane=true;q=Ray({},{.1f,0,0},skip);ck(!std::strcmp(q.status,"INVALID_RAY"),"out-of-segment data rejected");invalidPlane=false;
 put(0x30000020,reinterpret_cast<void*>(0x888));put(0x888,base+0x4072C78);put(0x894,std::uint32_t(7));q=Ray({},{.1f,0,0},skip);ck(!std::strcmp(q.status,"SELF_EXCLUDED"),"foreign owner cannot bypass self exclusion");
 put(0x30000020,reinterpret_cast<void*>(0x40000000));
 g_collision_binding6692=0;auto calls=nativeCalls;q=Ray({},{.1f,0,0},skip);ck(!std::strcmp(q.status,"UNAVAILABLE")&&nativeCalls==calls,"no new native query without existing validated binding");g_collision_binding6692=1;
 returned=-1;q=Ray({},{.1f,0,0},skip);ck(g_fault&&!std::strcmp(q.status,"NATIVE_FAULT")&&logs.size()==1,"query exception stops PRIVATE probe once");
 calls=nativeCalls;returned=1;(void)Ray({},{.1f,0,0},skip);ck(nativeCalls==calls&&g_collision_binding6692==1,"probe fault neither retries nor disables shared binding");
 std::printf("feature_ray59c6=%u native leaves are spies, not KCD2 calls\n",n);
}
