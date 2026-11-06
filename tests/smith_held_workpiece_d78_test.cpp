// Real D56 nested attachment reader + real current/held iron source functions.
#define KCD2VR_SMITH_FLOOR_FIXTURE_ONLY_D62
#include "smith_native_floor_d56_test.cpp"
namespace retail_build112 {static bool Ready(std::uintptr_t){return pins;}}
static void (*duringWorldD78)()=nullptr;
template<class T>static bool ReadHeldD78(const void* address,T& out){
 const bool read=Read(address,out);
 if(read&&reinterpret_cast<std::uintptr_t>(address)==reinterpret_cast<std::uintptr_t>(EntityById6694(44))+0x58&&duringWorldD78){
  auto callback=duringWorldD78;duringWorldD78=nullptr;callback();
 }
 return read;
}
#define Read ReadHeldD78
#ifndef SMITH_HELD_D78_INC
#define SMITH_HELD_D78_INC "smith_held_d78.inc"
#endif
#include SMITH_HELD_D78_INC
#undef Read
static void prepared(){
 setup(true);duringWorldD78=nullptr;
 const auto entity=reinterpret_cast<std::uintptr_t>(EntityById6694(44));
 put(entity,base+0x4072C78);float world[12]{1,0,0,10,0,1,0,20,0,0,1,30};put(entity+0x58,world);
}
int main(){
 std::uint32_t id=0;float world[12]{};prepared();
 check(SmithHeldIronA2(current,id,world)&&id==44,"exact nested workpiece qualifies held-tongs contact");
 for(int fault=0;fault<8;++fault){prepared();switch(fault){
  case 0:put(holder+st::kAttached,std::uint8_t(0));break;
  case 1:put(holder+st::kHost,std::uint32_t(99));break;
  case 2:put(holder+st::kAssetSlot,std::uint8_t(0));break;
  case 3:put(assetObject+8,std::uint32_t(45));break;
  case 4:put(asset+0x20,std::uintptr_t(0));break;
  case 5:put(object+0x0C,2.f);break;
  case 6:live=false;break;case 7:pins=false;break;
 }
 check(!SmithHeldIronA2(current,id,world)&&id==0,"detached, foreign, unknown or unsupported nested owner cannot clamp tongs");
 }
 prepared();duringWorldD78=+[](){put(holder+st::kAttached,std::uint8_t(0));};
 check(!SmithHeldIronA2(current,id,world)&&id==0,"native Detach during world read revokes the same held proof");
 prepared();auto old=current;duringWorldD78=+[](){++current.lifetime;};
 check(!SmithHeldIronA2(old,id,world),"same-address station lifetime turnover revokes in-flight proxy");
 prepared();put(holder+st::kAttached,std::uint8_t(0));current.tool=int(st::Tool::Hammer);
 check(SmithIronA2(current,id,world)&&id==44,"independent hammer thickness reader remains available for forge workpiece");
 std::printf("PASS actual held-workpiece readers: %u checks, native leaves substituted\n",checks);
}
