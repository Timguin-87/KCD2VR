// Actual extracted wrapper and transaction, native leaf calls doubled.
#include "ui_helmet_overlay63.h"
#include <array>
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
namespace kcd2vr::retail_build112 {inline bool verified=true;bool Ready(std::uintptr_t){return verified;}}
namespace kcd2vr::flashui_probe {
static std::atomic<void*> g_helmet_element63{nullptr};
static std::atomic<std::uint64_t> g_helmet_generation63{0};
static bool g_display_verified170=true,acquire=true;
static int expireOnLeaf=0,writes=0,releases=0; // 1 Resolve, 2 Get, 3 Set
static ui_native_canvas::Geometry movie{960,540,100,180,0};
static void* element=reinterpret_cast<void*>(0x1000);
static std::uintptr_t WhBase(){return 1;}
struct HudHolder{};
static void retire(){g_helmet_element63=nullptr;g_helmet_generation63.fetch_add(1);}
static bool AcquireElementDirect(HudHolder&,const char* name,void*& out){
 assert(std::strcmp(name,"Overlay")==0);if(!acquire)return false;
 if(g_helmet_element63.exchange(element)!=element)g_helmet_generation63.fetch_add(1);
 out=element;return true;
}
static void ReleaseHudHolder(HudHolder&){++releases;}
static bool FindMovieClip(void* el,const char* name,void*& clip,void*& get,void*& set){
 assert(el==element&&std::strcmp(name,"overlay0")==0);
 clip=reinterpret_cast<void*>(0x2000);get=reinterpret_cast<void*>(1);set=reinterpret_cast<void*>(2);
 if(expireOnLeaf==1){expireOnLeaf=0;retire();}return true;
}
static bool CallFlashDisplayFnSEH(void* fn,void* clip,unsigned char* bytes){
 assert(clip==reinterpret_cast<void*>(0x2000));
 if(fn==reinterpret_cast<void*>(1)){
  std::memcpy(bytes,&movie.x,4);std::memcpy(bytes+4,&movie.y,4);std::memcpy(bytes+8,&movie.rotation,4);
  std::memcpy(bytes+12,&movie.xs,4);std::memcpy(bytes+16,&movie.ys,4);
  if(expireOnLeaf==2){expireOnLeaf=0;retire();}return true;
 }
 assert(fn==reinterpret_cast<void*>(2));std::uint16_t flags=0;std::memcpy(&flags,bytes+0x34,2);assert(flags==0x1B);
 std::memcpy(&movie.x,bytes,4);std::memcpy(&movie.y,bytes+4,4);
 std::memcpy(&movie.xs,bytes+12,4);std::memcpy(&movie.ys,bytes+16,4);++writes;
 if(expireOnLeaf==3){expireOnLeaf=0;retire();}return true;
}
#include "helmet_runtime63.inc"
}
int main(){using namespace kcd2vr::flashui_probe;
 // First GetElement can itself publish the generation. This must not become
 // two separate 10cm offsets when the worker observes that generation later.
 assert(ApplyHelmetOverlay63(true,4)&&movie.x==960&&std::fabs(movie.xs-95.04f)<.01&&writes==1);
 assert(ApplyHelmetOverlay63(true,4)&&movie.x==960&&std::fabs(movie.xs-95.04f)<.01&&writes==1);
 assert(ApplyHelmetOverlay63(false,4)&&movie.x==960&&movie.xs==100&&writes==2);
 for(int leaf:{1,2,3}){
  retire();movie={960,540,100,180,0};const int before=writes;expireOnLeaf=leaf;
  assert(!ApplyHelmetOverlay63(true,4));
  assert(writes==before+(leaf==3?1:0)); // no stale write or rollback after retirement
 }
 retire();movie={960,540,100,180,0};assert(ApplyHelmetOverlay63(true,4));
 assert(movie.y==540&&std::fabs(movie.xs-95.04f)<.01&&std::fabs(movie.ys-142.56f)<.01);
 assert(ApplyHelmetOverlay63(false,4)&&movie.x==960&&movie.xs==100&&movie.ys==180);
 assert(releases==8);
 std::puts("PASS actual helmet wrapper: self-acquire generation, no double shift, retirement during Resolve/Get/Set, X-only restore");
}
