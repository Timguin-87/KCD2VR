#include "subsystems/hud_ui_controls/runtime/flashui_probe.h"
#include "ui_modal_overlay43.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <array>
using namespace kcd2vr;
namespace kcd2vr::retail_build112 {inline bool Ready(std::uintptr_t){return true;}}
namespace kcd2vr::diagnostic_trace {inline void Write(const char*){}}
namespace kcd2vr::flashui_probe {
std::array<std::atomic<void*>,2> g_activity_element_s1{};
std::array<std::atomic<std::uint64_t>,2> g_activity_generation_s1{};
bool g_display_verified170=true;
std::uintptr_t WhBase(){return 1;}
struct HudHolder{};
unsigned reads=0,writes=0,lookups=0;bool refuse=false,destroyDuringSet=false;
int objects[2]{},clips[2]{};
ui_native_canvas::Geometry displayGeometry[2]{{960.2f,540,100,100,0},{959.8f,540,100,100,0}};
bool AcquireElementDirect(HudHolder&,const char* name,void*& element){++lookups;element=&objects[std::strcmp(name,"LockPicking")!=0];return true;}
void ReleaseHudHolder(HudHolder&){}
bool FindMovieClip(void* e,const char* name,void*& clip,void*& get,void*& set){
 unsigned i=e==&objects[0]?0:1;assert(!std::strcmp(name,i==0?"mc_LockPick":"dialog"));
 clip=&clips[i];get=(void*)1;set=(void*)2;return !refuse;
}
bool CallFlashDisplayFnSEH(void* fn,void* c,void* bytes){unsigned i=c==&clips[0]?0:1;
 if(fn==(void*)1){++reads;auto p=ui_native_canvas::Pack(displayGeometry[i]);std::memcpy(bytes,p.data(),p.size());return true;}
 ++writes;std::array<unsigned char,0x38> b{};std::memcpy(b.data(),bytes,b.size());displayGeometry[i]=ui_native_canvas::Unpack(b);
 if(destroyDuringSet){g_activity_element_s1[i]=nullptr;++g_activity_generation_s1[i];}return true;
}
#include "subsystems/hud_ui_controls/activity_overlays_s1.inl"
}
int main(){using namespace flashui_probe;ui_surface_geometry::Hud hud{};
 hud.surface={1.92f,1.08f,1.55f,0,0,true};hud.safe={96,54,1824,1026};
 ui_native_state::Snapshot snapshot{};snapshot.known=snapshot.visible=ui_native_state::Bit(ui_native_state::LockPicking)|ui_native_state::Bit(ui_native_state::Pickpocketing);
 UpdateActivityOverlaysS1(snapshot,true,true,hud,1,.8f,100);
 assert(writes==2&&reads==4&&displayGeometry[0].xs<100&&displayGeometry[1].xs<100);
 const auto n=lookups;const auto w=writes;
 for(unsigned t=300;t<30000;t+=100)UpdateActivityOverlaysS1(snapshot,true,true,hud,1,.8f,t);
 assert(lookups==n+2&&writes==w); // one identity reconciliation, then no Flash polling
 const auto stable=lookups;
 for(unsigned t=30000;t<60000;t+=100)UpdateActivityOverlaysS1(snapshot,true,true,hud,1,.8f,t);
 assert(lookups==stable);
 snapshot.visible=0;UpdateActivityOverlaysS1(snapshot,true,true,hud,1,.8f,60100);
 assert(std::fabs(displayGeometry[0].xs-100)<.01&&std::fabs(displayGeometry[1].xs-100)<.01);
 snapshot.visible=snapshot.known;refuse=true;++g_activity_generation_s1[0];++g_activity_generation_s1[1];
 const auto start=lookups;
 for(unsigned t=61000;t<90000;t+=200)UpdateActivityOverlaysS1(snapshot,true,true,hud,1,.8f,t);
 assert(lookups==start+16);refuse=false;
 ++g_activity_generation_s1[0];++g_activity_generation_s1[1];destroyDuringSet=true;
 UpdateActivityOverlaysS1(snapshot,true,true,hud,1,.7f,90100);
 assert(!g_activity_element_s1[0].load()&&!g_activity_element_s1[1].load());
 std::puts("PASS actual S1 parent fit, readback, restoration, bounded retries, no steady Flash polling and destruction guards");
}
