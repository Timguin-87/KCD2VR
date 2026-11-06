#include "../src/vrik/native_weapon_actor_join_h28.h"
#include <unordered_map>
#include <cstring>
#include <cstdio>
using namespace kcd2vr;namespace j=native_weapon_actor_join_h28;namespace ne=native_weapon_contact_h24;
static unsigned checks=0,failures=0;void ck(bool b){++checks;if(!b){++failures;std::printf("FAIL %u\n",checks);}}
int main(){
 constexpr std::uintptr_t base=0x180000000,ent=0x40000,head=0x50000,node=0x60000,proxy=0x70000,body=0x80000;
 std::unordered_map<std::uintptr_t,std::uint64_t> m{{ent,base+0x4072C78},{ent+0xC,7},{ent+0xA8,head},{head+8,node},{node+0x19,0},{node+0x20,1},{node+0x28,proxy},{proxy,base+0x3AC3478},{proxy+0x38,body},{body,base+0x3A82570},{body+0x20,ent},{body+0x28,2}};
 auto read=[&](std::uintptr_t at,auto& v){auto it=m.find(at);if(it==m.end())return false;std::memcpy(&v,&it->second,sizeof(v));return true;};
 unsigned lookups=0;bool replace=false;auto entity=[&](std::uint32_t id){++lookups;if(replace&&lookups==2)m[proxy+0x38]=body+1;return id==7?ent:0;};
 native_weapon_body_t4::ContactOwnerH24 live{};live.active=true;live.owner.key=77;live.owner.generation=4;
 live.owner.player=10;live.owner.manager=20;live.owner.character=30;live.owner.entityId=42;live.owner.request=6;live.owner.epoch=5;
 live.body=0x90000;live.foreign=0xA0000;live.foreignType=0x5634;live.bodyId=1;live.worldGeneration=1;live.bodyGeneration=2;live.colliderLease=3;live.trackingEpoch=4;live.sourceSample=12;live.sourceTick=live.observedTick=100;
 native_weapon_parry_join_h24::Frame f{10,20,30,42,77,4,6,5,15,12,100,3,1,2,4,0};
 ne::Event e{};e.serial=1;e.tick=110;e.source=live;e.otherForeign=ent;e.otherBody=body;e.otherForeignType=2;e.otherPart=3;e.otherMaterial=116;
 j::Contact out{};auto qualify=[&](){lookups=0;return j::join(base,e,live,f,11,120,read,entity,out);};
 ck(qualify());ck(out.witness.entityId==7&&out.witness.part==3&&out.occurrence.serial==1&&out.occurrence.otherMaterial==116);
 ck(out.witness.bodyMaterial==-1&&out.witness.surfaceIdx==-1);
 m[body]=base+0x3A48B30;ck(qualify()&&out.witness.body==npc_contact172::Body::Articulated);m[body]=base+0x3A82570;
 for(unsigned cause=0;cause<19;++cause){const auto se=e;const auto sl=live;const auto sf=f;const auto sm=m;
  switch(cause){case 0:e.serial=0;break;case 1:e.tick=39;break;case 2:e.source.sourceSample=10;break;case 3:e.source.sourceSample=13;break;case 4:e.source.sourceTick=101;break;case 5:live.bodyGeneration++;break;case 6:live.colliderLease++;break;case 7:f.tracking++;break;case 8:f.cancel++;break;case 9:e.otherForeignType=0;break;case 10:e.otherPart=-1;break;case 11:m[body]=base+0x3A48DC0;break;case 12:m[proxy+0x38]=body+1;break;case 13:m[body+0x20]=ent+1;break;case 14:m[body+0x28]=0;break;case 15:m[ent+0xC]=0;break;case 16:m[ent+0xC]=42;break;case 17:e.normal.x=std::numeric_limits<float>::infinity();break;case 18:replace=true;break;}
  ck(!qualify());ck(out.occurrence.serial==0);e=se;live=sl;f=sf;m=sm;replace=false;
 }
 std::printf("actor_join_h28 %u checks %u failures; native damage NOT invoked\n",checks,failures);return failures?1:0;
}
