#include "../src/vrik/dialogue_native_head28.h"
#include <map>
#include <vector>
#include <cstring>
#include <cassert>
#include <cstdio>
namespace dn=kcd2vr::dialogue_native_head28;
namespace ds=kcd2vr::dialogue_state59c10r2;
namespace {
constexpr std::uintptr_t b=0x180000000ull,player=0x10000,actor=0x20000,entity=0x30000,animated=0x40000,go=0x50000,
 game=0x60000,module=0x70000,manager=0x80000,instance=0x90000,scene=0xA0000,cameras=0xB0000,
 inputManager=0xC0000,inputMap=0xD0000,fixtureCharacter=0xE0000,fixtureOwner=0xF0000,joints=0x110000;
std::map<std::uintptr_t,std::vector<unsigned char>> memory;
unsigned reads=0,getCalls=0,poseCalls=0,mutation=0;
std::uintptr_t failed=0;std::uint64_t liveEpoch=7;
bool originReadable=true,originStable=true;unsigned originReads=0,originChecks=0;
std::array<float,8> fixtureWorld{0,0,0,1,10,20,0,1};
template<class T>void put(std::uintptr_t a,T value){auto& data=memory[a];data.resize(sizeof(T));std::memcpy(data.data(),&value,sizeof(T));}
void q(std::uintptr_t a,std::uintptr_t value){put(a,value);}
void u(std::uintptr_t a,std::uint32_t value){put(a,value);}
void bit(std::uintptr_t a,unsigned char value){put(a,value);}
template<class T>bool read(std::uintptr_t a,T& value){++reads;const auto at=memory.find(a);
 if(a==failed||at==memory.end()||at->second.size()!=sizeof(T))return false;
 std::memcpy(&value,at->second.data(),sizeof(T));return true;}
ds::State fixtureState;
void reset(){originReadable=originStable=true;originReads=originChecks=0;memory.clear();reads=getCalls=poseCalls=mutation=0;failed=0;liveEpoch=7;fixtureWorld={0,0,0,1,10,20,0,1};
 fixtureState={};fixtureState.tick=100;fixtureState.entity=entity;fixtureState.player=player;fixtureState.instance=instance;fixtureState.known=fixtureState.active=true;
 fixtureState.camera=ds::Camera::FirstPerson;fixtureState.membership.known=fixtureState.membership.active=true;
 fixtureState.membership.participant=fixtureState.membership.listed=true;fixtureState.membership.state=1;
 fixtureState.inputOwner.known=fixtureState.inputOwner.enabled=true;fixtureState.inputOwner.manager=inputManager;fixtureState.inputOwner.map=inputMap;
 q(player,b+0x3AA4B98);q(player+0x40,b+0x3A81028);q(player+0x278,actor);q(actor,b+0x3A80610);
 q(actor+0x1F8,player);q(player+0x38,entity);q(entity,b+0x4072C78);q(b+0x4072C78+0x328,b+0x6411AC);
 q(player+0x268,animated);q(animated,b+0x3A49468);q(animated+0x18,go);q(go+0x18,entity);
 q(b+0x549D388,game);q(game+0x150,module);q(module,b+0x3E64C38);q(module+0x10,manager);
 q(manager,b+0x3E64C90);q(manager+0x90,instance);q(instance,b+0x3E63A40);q(instance+0x18,b+0x3E63B60);
 u(instance+0x278,1);q(instance+0x158,scene);u(scene+0xBC,0);q(game+0x38,cameras);q(cameras,b+0x408E560);
 bit(cameras+0x38,0);bit(cameras+0x3B,0);q(cameras+0x30,cameras+0x1E8);q(cameras+0x1E8,b+0x3A5A0F8);
 q(inputManager,b+0x4048018);q(inputMap,b+0x4048530);q(inputMap+0x10,inputManager);bit(inputMap+9,1);
 q(fixtureCharacter,b+0x40602F8);q(b+0x40602F8+0x38,b+0x1AA6BC0);q(fixtureCharacter+0x110,b+0x3A711E0);
 q(fixtureCharacter+0x698,b+0x3A71028);q(b+0x3A71028+0xD8,b+0x641250);q(fixtureCharacter+0x8E0,b+0x3A71510);
 q(b+0x3A71510+8,b+0x1A74B40);q(b+0x3A71510+0x10,b+0x53F550);q(b+0x3A71510+0x50,b+0x53D830);
 u(fixtureCharacter+0x8E0+0x60,450);q(fixtureCharacter+0x8E0+0x190,fixtureOwner);q(fixtureCharacter+0xA70,fixtureOwner);
 q(fixtureOwner,b+0x406B930);q(fixtureOwner+8,joints);u(joints-4,450);put(fixtureOwner+0x17C+20,std::uint16_t(12));put(fixtureCharacter+0xA08,fixtureWorld);
}
bool get(std::uintptr_t fn,std::uintptr_t ent,std::uintptr_t& out){++getCalls;assert(fn==b+0x6411AC&&ent==entity);
 out=fixtureCharacter;
 if(mutation==1&&getCalls==1)q(player+0x38,entity+8);
 if(mutation==2&&getCalls==2)out=fixtureCharacter+8;
 if(mutation==3&&getCalls==1)++liveEpoch;
 if(mutation==4&&getCalls==2){auto changed=fixtureWorld;changed[4]+=1;put(fixtureCharacter+0xA08,changed);}
 return mutation!=5;
}
bool pose(std::uintptr_t fn,std::uintptr_t p,int id,ds::V& out){++poseCalls;
 assert(fn==b+0x53D830&&p==fixtureCharacter+0x8E0&&id==12);out={0,0,1.7f};
 if(mutation==6&&poseCalls==2)out.z+=.1f;return mutation!=7;
}
bool copy(ds::NativeHead& out,bool scope=true){return dn::copy(b,fixtureState,100,7,scope,
 [](std::uintptr_t a,auto& v){return read(a,v);},get,pose,[]{return liveEpoch;},
 [](std::uintptr_t c,kcd2vr::native_roomscale_d76::Origin& out){++originReads;assert(c==fixtureCharacter);out.serial=6;out.consumedTracking={.2f,.3f,0};return originReadable;},
 [](std::uintptr_t c,std::uint64_t serial){++originChecks;assert(c==fixtureCharacter&&serial==6);return originStable;},out);}
}
#ifndef DIALOGUE_NATIVE_HEAD28_MEMORY_ONLY
int main(){unsigned checks=0;const auto check=[&](bool value){++checks;assert(value);};ds::NativeHead out{};
 reset();check(copy(out));check(out.tick==100&&out.epoch==7&&out.entity==entity&&out.character==fixtureCharacter);
 check(out.world.x==10&&out.world.y==20&&std::fabs(out.world.z-1.7f)<1e-5f&&out.modelOriginKnown);
 check(getCalls==2&&poseCalls==2);
 reset();const auto saved=out;check(!copy(out,false));check(reads==0&&getCalls==0&&poseCalls==0&&out.tick==saved.tick);
 reset();fixtureState.tick=19;check(!copy(out));check(reads==0&&getCalls==0);
 reset();fixtureState.camera=ds::Camera::Dialogue;check(!copy(out));check(reads==0&&getCalls==0);
 reset();fixtureState.membership.participant=false;check(!copy(out));check(reads==0&&getCalls==0);
 reset();fixtureState.inputOwner.enabled=false;check(!copy(out));check(reads==0&&getCalls==0);
 reset();check(copy(out));std::vector<std::uintptr_t> required;for(const auto& m:memory)required.push_back(m.first);
 for(auto a:required){reset();failed=a;const auto before=out;check(!copy(out));check(out.tick==before.tick&&out.character==before.character);}
 for(unsigned m=1;m<=7;++m){reset();mutation=m;const auto before=out;check(!copy(out));check(out.tick==before.tick&&out.character==before.character);}
 reset();q(go+0x18,entity+0x100);check(!copy(out));check(getCalls==0); // original actor cannot be replaced by a twin
 reset();put(fixtureOwner+0x17C+20,std::uint16_t(0xFFFF));check(!copy(out));check(poseCalls==0);
 reset();u(fixtureCharacter+0x8E0+0x60,451);check(!copy(out));check(poseCalls==0);
 reset();fixtureWorld[7]=0;put(fixtureCharacter+0xA08,fixtureWorld);check(!copy(out));
 reset();fixtureWorld[3]=0;put(fixtureCharacter+0xA08,fixtureWorld);check(!copy(out));
 reset();check(copy(out));check(originReads==1&&originChecks==1&&out.roomOriginD76.serial==6&&out.roomOriginD76.consumedTracking.x==.2f);
 reset();originReadable=false;check(!copy(out));check(poseCalls==0&&originChecks==0);
 reset();originStable=false;check(!copy(out));check(originReads==1&&originChecks==1);
 std::printf("dialogue_native_head28: %u original-native-view producer checks PASS\n",checks);
}
#endif
