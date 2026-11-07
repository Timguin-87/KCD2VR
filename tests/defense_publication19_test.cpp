#include "diagnostics/defense_publication19.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
namespace dp=kcd2vr::defense_publication19;
static std::map<std::uintptr_t,std::vector<unsigned char>> memory;
template<class T>void put(std::uintptr_t a,T value){auto& b=memory[a];b.resize(sizeof(T));std::memcpy(b.data(),&value,sizeof(T));}
static bool read(std::uintptr_t a,void* p,std::size_t n){auto it=memory.find(a);if(it==memory.end()||it->second.size()!=n)return false;std::memcpy(p,it->second.data(),n);return true;}
int main(){
 constexpr std::uintptr_t base=0x180000000ull,state=0x10000,actor=0x20000,handle=0x30000,npc=0x40000,npcState=0x50000,entity=0x60000,automation=0x70000,models=0x80000;
 auto setup=[&]{memory.clear();put(state+0xCE8,handle);put(state+0xCF8,npc);put(handle,base+0x3A796F0);put(handle+8,entity);
  for(unsigned p:{0x4B0,0x4E0}){put(state+p,base+0x3B0A6A0);put(state+p+8,(unsigned char)1);}
  put(npc,base+0x3E3FAC8);put(npc+0x210,npcState);put(npcState+0xCE0,npc);put(npc+0x1F8,entity);put(entity,base+0x4072C78);
  put(npcState+0x680,base+0x3B0A6A0);put(npcState+0x688,(unsigned char)0);
  put(npc+0x248,automation);put(automation,base+0x3B0A480);put(automation+8,npc);put(automation+0x48,models);
  put(automation+0x118,base+0x3B0A390);put(automation+0x120,npc);put(automation+0x140,(std::uintptr_t)0);
  put(models+0x1A8,npc);put(models+0x108,(std::uintptr_t)0);put(automation+0x108,(std::uintptr_t)999);};
 setup();auto r=dp::Copy(read,base,state,actor,1);assert(r.ce8Present==1&&r.ce8Kind==1&&r.ce8MatchesCf8==1&&r.reach4B0==1&&r.victim680==0&&r.modelsPending108==0&&r.oldPhase==1);
 put(models+0x108,(std::uintptr_t)123);r=dp::Copy(read,base,state,actor,4);assert(r.modelsPending108==1&&r.oldPhase==4); // same-phase write cannot notify a new native roll
 put(state+0xCE8,(std::uintptr_t)0);r=dp::Copy(read,base,state,actor,1);assert(r.ce8Present==0&&r.ce8MatchesCf8==-1); // absence isn't a fabricated target
 for(int failure=0;failure<5;++failure){setup();
  if(failure==0)put(automation,base+0x123);
  if(failure==1)put(automation+8,actor);
  if(failure==2)put(models+0x1A8,actor);
  if(failure==3)put(automation+0x140,models+32);
  if(failure==4)memory.erase(models+0x108);
  r=dp::Copy(read,base,state,actor,1);assert(r.modelsPending108==-1);
 }
 setup();put(state+0x4B0,base+0x123);r=dp::Copy(read,base,state,actor,1);assert(r.reach4B0==-1);
 put(handle+8,entity+32);r=dp::Copy(read,base,state,actor,1);assert(r.ce8MatchesCf8==0);
 setup();memory.erase(npc+0x1F8+0x38); // a failed copy may partially touch its destination
 auto partialRead=[&](std::uintptr_t a,void* p,std::size_t n){
     if(a==entity){if(n)std::memset(p,0x7f,n);return false;}return read(a,p,n);};
 r=dp::Copy(partialRead,base,state,actor,1);assert(r.ce8MatchesCf8==-1); // failed owner-vft read cannot become a known entity match
 setup();put(npc+0x1F8,entity+0x100);put(entity+0x100,(std::uintptr_t)0x1234);
 auto partialOwnerRead=[&](std::uintptr_t a,void* p,std::size_t n){
     if(a==entity+0x100+0x38){std::memcpy(p,&entity,n);return false;}return read(a,p,n);};
 r=dp::Copy(partialOwnerRead,base,state,actor,1);assert(r.ce8MatchesCf8==-1); // pointer staging discards even a plausible partial CEntity pointer
 std::puts("PASS pre-publication snapshot: exact target/entity, typed reach bool, MODELS+108 vs wrong automation+108, stale/cache/owner/unreadable UNKNOWN");
}
