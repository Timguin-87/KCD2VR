#include "../src/vrik/alchemy_resource_scope_d67r1.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <unordered_map>

namespace rs=kcd2vr::alchemy_resource_scope_d67r1;
static unsigned checks=0;
static void ck(bool b,const char* why){++checks;if(!b)std::fprintf(stderr,"FAIL: %s\n",why);assert(b);}
struct Fixture {
 std::unordered_map<std::uintptr_t,unsigned char> bytes;
 rs::Key key{0x180000000ull,0x10000,0x20000,0x1FB3F116DA0ull,0x1F85770F5C0ull,0x1F9B3350EF0ull,0x1F9B3351000ull,55432,48};
 rs::Rates rates{};
 static constexpr std::uintptr_t context=0x60000,foreignContext=0x61000,entries=0xB0000;
 template<class T> void put(std::uintptr_t at,T value){const auto* p=reinterpret_cast<const unsigned char*>(&value);for(unsigned i=0;i<sizeof(T);++i)bytes[at+i]=p[i];}
 template<class T> bool read(std::uintptr_t at,T& value) const {
  unsigned char data[sizeof(T)]{};for(unsigned i=0;i<sizeof(T);++i){const auto it=bytes.find(at+i);if(it==bytes.end())return false;data[i]=it->second;}
  std::memcpy(&value,data,sizeof(T));return true;
 }
 std::uintptr_t scope(unsigned i)const{return key.root+std::uintptr_t(i)*0x168;}
 void caf(std::uintptr_t at){put(at,key.base+0x3A70A60);put(at+8,std::uintptr_t(0));put(at+0x10,unsigned(key.animation));put(at+0x14,std::uint8_t(0));put(at+0x16,std::uint8_t(0));put(at+0x4C,unsigned(0x4104));}
 Fixture(){
  // Known log join: water animation48, token55432, same child in native scopes
  // 0/14/15, root0+3. Unlogged addresses/empty storage are explicit fixture
  // values, not a claim that the old run passed every new admission condition.
  put(key.controller+0x34,unsigned(20));put(key.controller+0x38,key.root);
  put(context+0x10,key.character);put(foreignContext+0x10,std::uintptr_t(0x90000));
  constexpr unsigned layer[]{0,3,4,4,6,8,9,9,10,11,12,13,14,14,15,15,0,0,0,0};
  constexpr unsigned count[]{3,1,2,0,2,1,1,0,1,1,1,1,1,0,1,0,3,3,3,2};
  for(unsigned i=0;i<20;++i){const auto s=scope(i);put(s,key.base+0x3AEA100);put(s+0x20,i<16?context:foreignContext);
   put(s+0x30,layer[i]);put(s+0x34,count[i]);put(s+0x148,(i==0||i==14||i==15)?key.child:std::uintptr_t(0));
   put(s+0x58,1.f);put(s+0x60,0.f);put(s+0x64,0.f);
  }
  put(key.skeleton,key.base+0x3A711E0);put(key.skeleton+0x4E8,key.character);
  put(key.child,key.base+0x3A47948);put(key.child+0x30,key.root);put(key.child+0x268,key.instance);
  put(key.child+0x54,key.token);put(key.child+0x28,int(2));put(key.child+0x5C,1.f);
  for(unsigned i=0;i<3;++i){const auto e=entries+std::uintptr_t(i)*0x1000;put(key.skeleton+0x48+std::uintptr_t(i)*0x40,e);put(e-4,unsigned(i?0:1));}
  put(key.skeleton+0x50,1.f);caf(entries);
 }
 rs::Result inspect(const rs::Proof* old=nullptr) const {return rs::inspect([this](auto at,auto& value){return read(at,value);},key,rates,old);}
};
int main(){
 {
  Fixture f;const auto r=f.inspect();ck(bool(r),"normal FullBody root3 with two empty slots qualifies");
  ck(r.proof.ownedScopes==0xC001u,"current root and LookPose/Looking action ownership joined");
  ck(bool(f.inspect(&r.proof)),"unchanged proof is usable at the next owner update");
  f.rates.last=0;f.put(f.key.child+0x5C,0.f);f.put(f.key.skeleton+0x50,0.f);
  ck(bool(f.inspect(&r.proof)),"own pause allows native scope caches to catch up without losing identity");
  for(unsigned i:{0u,14u,15u})f.put(f.scope(i)+0x58,0.f);
  ck(bool(f.inspect(&r.proof)),"native scopes acknowledged the exact owned zero rate");
 }
 for(unsigned i=1;i<3;++i){Fixture f;f.put(Fixture::entries+std::uintptr_t(i)*0x1000-4,unsigned(1));ck(f.inspect().reason==rs::Reason::ExtraCaf,"an extra active root CAF is never silently retimed");}
 for(unsigned count:{0u,2u,4u,16u}){Fixture f;f.put(f.key.root+0x34,count);ck(f.inspect().reason==rs::Reason::Profile,"unproven root shape is rejected");}
 {Fixture f;f.put(Fixture::entries-4,unsigned(2));ck(f.inspect().reason==rs::Reason::RootCaf,"root blend with two CAFs is rejected");}
 {Fixture f;f.put(Fixture::entries+0x10,unsigned(49));ck(f.inspect().reason==rs::Reason::RootCaf,"a different resource CAF cannot inherit this interval");}
 {Fixture f;f.put(Fixture::entries+8,std::uintptr_t(1));ck(f.inspect().reason==rs::Reason::RootCaf,"parametric sampler remains unsupported");}
 {Fixture f;f.put(Fixture::entries+0x16,std::uint8_t(1));ck(f.inspect().reason==rs::Reason::RootCaf,"different native segment is rejected");}
 {Fixture f;f.put(f.scope(1)+0x30,unsigned(1));ck(f.inspect().reason==rs::Reason::Overlap,"overlapping allocated writer on Henry is rejected");}
 {Fixture f;f.put(f.scope(1)+0x148,f.key.child);ck(f.inspect().reason==rs::Reason::ScopeOwner,"unexpected scope inheriting child speed is rejected");}
 {Fixture f;f.put(f.scope(16)+0x148,f.key.child);ck(f.inspect().reason==rs::Reason::ScopeOwner,"slave character may not inherit this resource clock");}
 {Fixture f;f.put(f.scope(0)+0x148,std::uintptr_t(1));ck(!f.inspect(),"root no longer owned by this child is rejected");}
 {Fixture f;f.put(f.scope(14)+0x60,.1f);ck(f.inspect().reason==rs::Reason::Pending,"pending procedural/look scope time is rejected");}
 {Fixture f;f.put(f.key.root+0x64,.1f);ck(f.inspect().reason==rs::Reason::Pending,"pending root time is rejected");}
 {Fixture f;f.put(f.scope(15)+0x58,.5f);ck(f.inspect().reason==rs::Reason::Rate,"foreign procedural scope rate writer is rejected");}
 {Fixture f;f.put(f.key.skeleton+0x50,.5f);ck(f.inspect().reason==rs::Reason::Rate,"foreign CAF-layer writer is rejected");}
 {Fixture f;f.rates.otherWriter=true;ck(f.inspect().reason==rs::Reason::Rate,"competing stage/pickup lease is rejected");}
 {Fixture f;f.rates.saved=2.f;ck(f.inspect().reason==rs::Reason::Rate,"non-native initial rate cannot be stolen");}
 {Fixture f;f.put(f.key.child+0x54,unsigned(55433));ck(f.inspect().reason==rs::Reason::Identity,"reused child pointer with new token is rejected");}
 {Fixture f;f.put(f.key.child+0x268,std::uintptr_t(1));ck(f.inspect().reason==rs::Reason::Identity,"different alchemy instance is rejected");}
 {Fixture f;f.put(f.key.child+0x30,f.key.root+0x168);ck(f.inspect().reason==rs::Reason::Identity,"child root migration is rejected");}
 {Fixture f;f.put(f.key.skeleton+0x4E8,std::uintptr_t(1));ck(f.inspect().reason==rs::Reason::Identity,"character/skeleton backlink mismatch is rejected");}
 {
  Fixture f;const auto old=f.inspect().proof;f.put(f.key.root+0x20,std::uintptr_t(0x62000));f.put(0x62010,f.key.character);
  ck(f.inspect(&old).reason==rs::Reason::Migration,"context allocation migration invalidates an existing lease");
 }
 {
  Fixture f;const auto old=f.inspect().proof;f.put(f.scope(15)+0x148,std::uintptr_t(0));
  ck(f.inspect(&old).reason==rs::Reason::Migration,"owned procedural scope migration invalidates an existing lease");
 }
 {
  Fixture f;const auto old=f.inspect().proof;const std::uintptr_t replacement=0xC0000;
  f.put(f.key.skeleton+0x48,replacement);f.put(replacement-4,unsigned(1));f.caf(replacement);
  ck(f.inspect(&old).reason==rs::Reason::Migration,"replaced FIFO cannot inherit an old event interval");
 }
 {Fixture f;f.bytes.erase(Fixture::entries-4);ck(f.inspect().reason==rs::Reason::Read,"unreadable FIFO fails closed");}
 {Fixture f;f.put(f.key.controller+0x34,unsigned(25));ck(f.inspect().reason==rs::Reason::Profile,"scope enumeration is bounded");}
 std::printf("D67R1 resource scope profile %u checks PASS; native events and writes are not simulated\n",checks);
}
