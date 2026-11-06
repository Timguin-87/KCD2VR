// Actual extracted wrapper and transaction, native leaf calls doubled.
#include "ui_helmet_overlay63.h"
#include "ui_modal_overlay43.h"
#include <array>
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
namespace kcd2vr::retail_build112 {inline bool verified=true;bool Ready(std::uintptr_t){return verified;}}
namespace kcd2vr::flashui_probe {
static std::atomic<void*> g_wait_element_b2{nullptr};
static std::atomic<std::uint64_t> g_wait_generation_b2{0};
static bool g_display_verified170=true,acquire=true;
static int expireOnLeaf=0,writes=0,releases=0; // 1 Resolve, 2 Get, 3 Set
static ui_native_canvas::Geometry movie{960,540,100,180,0};
static void* element=reinterpret_cast<void*>(0x1000);
static std::uintptr_t WhBase(){return 1;}
struct HudHolder{};
static void retire(){g_wait_element_b2=nullptr;g_wait_generation_b2.fetch_add(1);}
static bool AcquireElementDirect(HudHolder&,const char* name,void*& out){
 assert(std::strcmp(name,"SkipTime")==0);if(!acquire)return false;
 if(g_wait_element_b2.exchange(element)!=element)g_wait_generation_b2.fetch_add(1);
 out=element;return true;
}
static void ReleaseHudHolder(HudHolder&){++releases;}
static bool FindMovieClip(void* el,const char* name,void*& clip,void*& get,void*& set){
 assert(el==element&&std::strcmp(name,"mc_Dialog")==0);
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
#include "wait_runtime_b2.inc"
}
int main(){using namespace kcd2vr::flashui_probe;
 kcd2vr::ui_surface_geometry::Hud h{};h.surface={4,2.25f,1.55f,0,0,true};h.safe={480,270,1440,810};
 assert(ApplyWaitOverlayB2(true,h)&&movie.x==960&&movie.y==540&&std::fabs(movie.xs-35)<.01&&writes==1);
 assert(ApplyWaitOverlayB2(true,h)&&writes==1); // no cumulative shrink
 assert(ApplyWaitOverlayB2(false,h)&&movie.xs==100&&movie.ys==180);
 for(int leaf:{1,2,3}){retire();movie={960,540,100,180,0};const int before=writes;expireOnLeaf=leaf;
  assert(!ApplyWaitOverlayB2(true,h));assert(writes==before+(leaf==3?1:0));}
 retire();movie={0,0,100,100,0};const int before=writes;assert(!ApplyWaitOverlayB2(true,h)&&writes==before);
 retire();movie={960,540,100,180,0};assert(ApplyWaitOverlayB2(true,h));assert(ApplyWaitOverlayB2(false,h));
 std::puts("PASS actual wait wrapper: named card only, smaller fit, no drift, native restore, lifetime refusal");
}
