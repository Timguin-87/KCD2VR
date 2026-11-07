#include "../src/vrik/native_weapon_parry_join_h24.h"
#include <unordered_map>
#include <cstring>
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr;namespace wp=weapon_parry_h24;namespace join=native_weapon_parry_join_h24;namespace ne=native_weapon_contact_h24;
static unsigned checks=0;static void check(bool b){++checks;if(!b){std::printf("FAIL %u\n",checks);std::exit(1);}}
int main(){
 constexpr std::uintptr_t base=0x180000000,state=0x10000,item=0x20000,link=0x30000,ent=0x40000,head=0x50000,node=0x60000,proxy=0x70000,body=0x80000;
 constexpr std::uint64_t key=0x0200000000018E22;
 std::unordered_map<std::uintptr_t,std::uint64_t> m{{state+0x8C0,base+0x3B0A5E8},{state+0x8C8,key},{item,base+0x3A2DA88},{item+0x30,key},{item+0x88,link},{link+0x30,7},{ent,base+0x4072C78},{ent+0xC,7},{ent+0xA8,head},{head+8,node},{node+0x19,0},{node+0x20,1},{node+0x28,proxy},{proxy,base+0x3AC3478},{proxy+0x38,body}};
 auto read=[&](std::uintptr_t at,auto& v){auto i=m.find(at);if(i==m.end())return false;std::memcpy(&v,&i->second,sizeof(v));return true;};
 auto lookup=[&](std::uint64_t k){return k==key?item:0;};auto entity=[&](std::uint32_t id){return id==7?ent:0;};

 constexpr std::uintptr_t parts=0xB0000;
 m[body]=base+0x3A48DC0;m[body+0x20]=ent;m[body+0x28]=2;m[body+0x120]=1;m[body+0x118]=parts;m[parts+0x48]=3;
 native_weapon_body_t4::ContactOwnerH24 live{};live.active=true;live.owner.key=77;live.owner.generation=4;
 live.owner.player=10;live.owner.manager=20;live.owner.character=30;live.owner.entityId=42;live.owner.request=6;live.owner.epoch=5;
 live.body=0x90000;live.foreign=0xA0000;live.foreignType=0x5634;live.bodyId=1;
 live.worldGeneration=1;live.bodyGeneration=2;live.colliderLease=3;live.trackingEpoch=4;
 live.sourceSample=12;live.sourceTick=live.observedTick=100;
 join::Frame f{10,20,30,42,77,4,6,5,15,12,100,3,1,2,4,0};
 ne::Event e{};e.serial=1;e.tick=110;e.source=live;e.otherForeign=ent;e.otherBody=body;e.otherForeignType=2;e.otherPart=3;
 wp::Pair pair{1,2,3,state,5,6};wp::Contact out{};
 auto qualify=[&](){return join::join(base,e,live,f,pair,9,11,0,120,read,lookup,entity,out);};
 check(qualify());check(out.nativeEvent==1&&out.serial==9&&out.sample==12&&out.tick==110&&out.enemy.key==key&&out.part==3);
 for(unsigned cause=0;cause<22;++cause){auto saved=e;auto original=live;auto savedFrame=f;auto savedMem=m;
  switch(cause){case 0:e.tick=39;break;case 1:e.source.sourceSample=10;break;case 2:e.source.sourceSample=13;break;
   case 3:e.source.sourceTick=101;break;case 4:e.source.foreign++;break;case 5:e.source.bodyGeneration++;break;
   case 6:live.owner.player++;break;case 7:live.colliderLease++;break;case 8:f.body++;break;case 9:f.tracking++;break;
   case 10:e.otherForeignType=0;break;case 11:e.otherPart=-1;break;case 12:e.otherPart=4;break;
   case 13:m[body+0x120]=2;break;case 14:m[body+0x28]=0;break;case 15:m[body+0x20]=ent+1;break;
   case 16:m[body]=base+0x3A82570;break;case 17:m[state+0x8C8]=key+1;break;case 18:m[item+0x30]=key+1;break;
   case 19:m[proxy+0x38]=body+1;break;case 20:e.serial=0;break;case 21:live.cancelGeneration++;break;}
  check(!qualify());check(out.nativeEvent==0);e=saved;live=original;f=savedFrame;m=savedMem;
 }
 e.selfSide=1;e.otherPhysicsIdKnown=false;e.otherPhysicsId=-1;check(qualify());
 check(out.enemy.collider==body); // Independent current item/body proof, no invented side1otherID.
 wp::Episode episode;check(episode.observe(pair,4,11)&&episode.onsetSample==11);
 check(episode.observe(pair,8,12)&&episode.onsetSample==11);check(!episode.observe(pair,16,13));
 check(episode.observe(pair,4,14)&&episode.onsetSample==14);
 check(!join::join(base,e,live,f,pair,episode.serial,episode.onsetSample,0,120,read,lookup,entity,out));
 std::printf("PASS %u native occurrence/current enemy weapon join checks\n",checks);
}
