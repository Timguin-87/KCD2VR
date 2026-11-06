#include "../src/vrik/weapon_parry_contact_h24.h"
#include <unordered_map>
#include <cstring>
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr;namespace wp=weapon_parry_h24;
static unsigned checks=0;static void check(bool b){++checks;if(!b){std::printf("FAIL %u\n",checks);std::exit(1);}}
int main(){
 constexpr std::uintptr_t base=0x180000000,state=0x10000,item=0x20000,link=0x30000,ent=0x40000,head=0x50000,node=0x60000,proxy=0x70000,body=0x80000;
 constexpr std::uint64_t key=0x0200000000018E22;
 std::unordered_map<std::uintptr_t,std::uint64_t> m{{state+0x8C0,base+0x3B0A5E8},{state+0x8C8,key},{item,base+0x3A2DA88},{item+0x30,key},{item+0x88,link},{link+0x30,7},{ent,base+0x4072C78},{ent+0xC,7},{ent+0xA8,head},{head+8,node},{node+0x19,0},{node+0x20,1},{node+0x28,proxy},{proxy,base+0x3AC3478},{proxy+0x38,body}};
 auto read=[&](std::uintptr_t at,auto& v){auto i=m.find(at);if(i==m.end())return false;std::memcpy(&v,&i->second,sizeof(v));return true;};
 auto lookup=[&](std::uint64_t k){return k==key?item:0;};auto entity=[&](std::uint32_t id){return id==7?ent:0;};
 npc_contact172::Witness h{};h.ownerRead=true;h.owner=ent;h.entityId=7;h.collider=body;h.part=3;
 wp::Held held{};auto qualify=[&](){return wp::held(base,state,0,h,read,lookup,entity,held);};
 check(qualify());check(held.key==key&&held.entity==ent&&held.collider==body&&held.hand==0);
 h.owner=ent+1;check(!qualify());check(!held.key);h.owner=ent;
 h.collider=body+1;check(!qualify());h.collider=body;
 h.ownerRead=false;check(!qualify());h.ownerRead=true;
 h.entityId=8;check(!qualify());h.entityId=7;
 m[item+0x30]=key+1;check(!qualify());m[item+0x30]=key;
 m[state+0x8C8]=key+1;check(!qualify());m[state+0x8C8]=key;
 m[proxy]=base+1;check(!qualify());m[proxy]=base+0x3AC3478;
 check(!wp::held(base,state,2,h,read,lookup,entity,held));
 check(qualify());
 wp::Pair pair{1,2,3,4,5,6};wp::Episode ep;
 check(ep.observe(pair,4,1));check(ep.serial==1);check(!ep.observe(pair,4,1));check(ep.observe(pair,8,2));check(ep.serial==1);
 check(!ep.observe(pair,16,3));check(ep.observe(pair,4,4));check(ep.serial==2);
 check(!ep.observe(pair,-1,5));check(!ep.observe(pair,4,6));check(ep.serial==2);check(!ep.observe(pair,16,7));check(ep.observe(pair,4,8));check(ep.serial==3);
 wp::Contact c{pair,held,3,100,8,key,12,0,3};check(wp::current(c,pair,180,key,12,0));check(!wp::current(c,pair,181,key,12,0));check(!wp::current(c,pair,99,key,12,0));
 auto other=pair;other.opponent=9;check(!wp::current(c,other,110,key,12,0));check(!wp::current(c,pair,110,key+1,12,0));check(!wp::current(c,pair,110,key,13,0));check(!wp::current(c,pair,110,key,12,1));
 std::printf("PASS %u actual production helper checks\n",checks);
}

