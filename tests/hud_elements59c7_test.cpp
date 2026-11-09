#include "ui_hud_work59c7.h"
#include "ui_hud_elements171.h"
#include <array>
#include <atomic>
#include <mutex>
#include <cstring>
#include <cstdio>
#include <cassert>
using namespace kcd2vr;
std::uint64_t tick=1000;std::uint64_t GetTickCount64(){return tick;}
std::uint64_t generation=1;std::uint64_t HudGeneration(){return generation;}
std::recursive_mutex g_hud_draw_mutex162r1;
ui_hud_work59c7::Children g_children59c7;
ui_hud_work59c7::Retry g_elements_retry59c7;
ui_hud_elements171::Controller g_elements171;
std::atomic<std::uint64_t>g_canvas_lifetime{1};std::atomic<void*>g_canvas_live_hud{(void*)1};
bool g_display_verified170=true,g_canvas_requested=false,g_parent_ready59c7=false;
std::uint64_t g_parent_key59c7=0;
unsigned acquired=0,released=0,resolves=0,gets=0,sets=0,parentRuns=0;
struct HudHolder{};
bool AcquireHudDirect(HudHolder&,void*&h){++acquired;h=(void*)1;return true;}
void ReleaseHudHolder(HudHolder&){++released;}
namespace diagnostic_trace{void Write(const char*){}}
std::array<bool,4> present{{true,false,true,true}};
std::array<std::uintptr_t,4> ids{{1,2,3,4}};
std::array<ui_native_canvas::Geometry,4> values{{{0,0,100,100,0},{0,0,100,100,0},{0,0,100,100,0},{0,0,100,100,0}}};
struct NativeCanvasBackend {
 void*hud=nullptr;std::uint64_t generation=0;
 std::uint64_t ElementRevision59c7(unsigned i){return g_children59c7.Revision(i);}
 bool Resolve(unsigned i,const char*,std::uintptr_t&id){
  ++resolves;if(!present[i])return false;id=ids[i];return true;
 }
 bool Get(unsigned i,ui_native_canvas::Geometry&v){++gets;v=values[i];return true;}
 bool Set(unsigned i,ui_native_canvas::Geometry v){++sets;values[i]=v;return true;}
};
struct Canvas {ui_native_canvas::Report Run(NativeCanvasBackend&,std::uint64_t,bool,const ui_native_canvas::Fit&){
 ++parentRuns;ui_native_canvas::Report r{};r.complete=true;r.resolved=0x7ff;r.written=0x7ff;return r;
}}g_canvas;
ui_native_canvas::Report g_canvas_report{};
bool HudCaptureActive175(){return false;} // R160: stubs for the later D63/R12 notification calls in the parent function
void SetNotificationsRequestedD80(bool){}
bool ApplyNotificationScaleR12(){return true;}
#include "generated_hud_elements59c7.inc"
#include "generated_hud_parent59c7.inc"
int main(){
 auto apply=[](float stats=.5f){return ApplyUniformHudElements171(1,0,stats,.75f,0,0,20,30);};
 assert(!apply());auto r=g_elements171.LastReport();assert(r.pending==2&&!r.complete&&values[2].xs==75);
 const auto n=resolves,a=acquired;tick+=11;assert(!apply()&&resolves==n&&acquired==a);
 tick=1200;assert(!apply()&&resolves==n+1);tick=1400;assert(!apply()&&resolves==n+2);
 tick=10000;assert(!apply()&&resolves==n+2); // no permanent polling
 present[1]=true;g_children59c7.Observe(1,{2,12,22,1,generation});
 assert(apply()&&values[1].xs==50&&values[1].x==20&&values[1].y==-30);
 auto done=resolves;for(int i=0;i<10000;++i){++tick;assert(apply());}assert(resolves==done);
 const auto changed=apply(.6f);std::fprintf(stderr,"CHILD changed=%d xs=%.9f resolves=%u expected=%u\n",changed,values[1].xs,resolves,done+1);assert(changed&&std::fabs(values[1].xs-60)<.001f&&resolves==done+1); // only changed child
 // Same wrapper, replaced native object; capture new baseline, never compound old scale.
 values[1]={3,4,120,120,0};g_children59c7.Observe(1,{2,13,23,1,generation});
 assert(apply(.6f)&&values[1].xs==72&&values[1].x==23);
 ui_surface_geometry::Hud h{};h.surface.valid=true;h.surface.width=1.6f;h.surface.height=.9f;h.safe={0,0,1920,1080};
 assert(ApplyWholeReadableHud44c2(true,h,1,1,1,1,1,1,.71f,420.f,0.f)&&parentRuns==1);
 assert(apply(.7f));assert(ApplyWholeReadableHud44c2(true,h,1,1,1,1,1,1,.71f,420.f,0.f)&&parentRuns==1);
 ++generation;assert(ApplyWholeReadableHud44c2(true,h,1,1,1,1,1,1,.71f,420.f,0.f)&&parentRuns==2);
 assert(ApplyWholeReadableHud44c2(true,h,.8f,1,1,1,1,1,.71f,420.f,0.f)&&parentRuns==3);
 assert(ApplyWholeReadableHud44c2(true,h,.8f,1,1,1,1,1,.90f,420.f,0.f)&&parentRuns==4); // R160: dialogue size is part of the parent key
 assert(ApplyWholeReadableHud44c2(true,h,.8f,1,1,1,1,1,.90f,520.f,0.f)&&parentRuns==5); // R160: dialogue vertical too
 assert(ApplyWholeReadableHud44c2(true,h,.8f,1,1,1,1,1,.90f,520.f,0.f)&&parentRuns==5); // unchanged = no native call
 assert(ApplyWholeReadableHud44c2(true,h,.8f,1,1,1,1,1,.90f,520.f,60.f)&&parentRuns==6); // R160 UI: tutorial position is part of the parent key
 assert(ApplyWholeReadableHud44c2(true,h,.8f,1,1,1,1,1.2f,.90f,520.f,60.f)&&parentRuns==7); // R160 UI: one icon size (parent icon scale) too
 assert(ApplyWholeReadableHud44c2(true,h,.8f,1,1,1,1,1.2f,.90f,520.f,60.f)&&parentRuns==7); // unchanged = no native call
 puts("PASS actual wrappers: missing Stats pending, three retries, late binding, zero steady calls, one-child change, same-wrapper replacement, parent/child isolation");
}
