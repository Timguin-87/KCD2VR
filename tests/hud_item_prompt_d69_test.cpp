// Executes the extracted current ApplyReadablePromptScale and production transactions.
// Native Flash acquisition/read/write/identity leaves are explicit; no live UI claim.
#include "ui_prompt_scale120r3.h"
#include "ui_hud_work59c7.h"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <limits>
namespace kcd2vr::flashui_probe {
using G=ui_native_canvas::Geometry;
struct ClipD69 {G value{};std::uintptr_t object=0,handle=0;bool present=true;int failSet=0,wrongRead=0,wrongAfterSet=0;};
static ClipD69 clipsD69[4];
static unsigned checksD69=0,setsD69=0,acquiresD69=0,releasesD69=0;
static bool availableD69=true,animateD69=false;static std::uint64_t hudGenerationD69=7;
static void ckD69(bool ok,const char* why){++checksD69;if(!ok){std::fprintf(stderr,"FAIL item prompt %s\n",why);std::exit(1);}}
static bool nearD69(float a,float b){return std::fabs(a-b)<.02f;}
std::recursive_mutex g_hud_draw_mutex162r1;
std::atomic<void*> g_canvas_live_hud{nullptr};std::atomic<std::uint64_t> g_canvas_lifetime{1};
ui_prompt_scale120r3::Transaction g_prompt162r1;
ui_prompt_scale120r3::ActionCursorD69 g_action_cursor_d69;
ui_hud_work59c7::SizedChildren<2> g_action_cursor_children_d69;
ui_hud_work59c7::SizedChildren<2> g_prompt_children_d74;
struct HudHolder{};
static void* currentHudD69=reinterpret_cast<void*>(0x1000);
static bool AcquireHudDirect(HudHolder&,void*& out){++acquiresD69;out=availableD69?currentHudD69:nullptr;return availableD69;}
static void ReleaseHudHolder(HudHolder&){++releasesD69;}
static std::uint64_t HudGeneration(){return hudGenerationD69;}
static bool ReadHudKey175(void* clip,void* hud,std::uint64_t generation,ui_hud_visibility175::Key& key){
 auto* c=static_cast<ClipD69*>(clip);if(!c||!c->present||!c->object||!c->handle)return false;
 key={reinterpret_cast<std::uintptr_t>(clip),c->handle,c->object,reinterpret_cast<std::uintptr_t>(hud),generation};return true;
}
static bool CallFlashDisplayFnSEH(void*,void* clip,void* bytes){
 auto& c=*static_cast<ClipD69*>(clip);++setsD69;
 std::uint16_t flags=0;std::memcpy(&flags,static_cast<unsigned char*>(bytes)+0x34,2);
 ckD69(flags==0x18,"actual owner sends scale-only flags");
 if(c.failSet){--c.failSet;return false;}
 std::memcpy(&c.value.xs,static_cast<unsigned char*>(bytes)+12,4);std::memcpy(&c.value.ys,static_cast<unsigned char*>(bytes)+16,4);if(c.wrongAfterSet){--c.wrongAfterSet;c.wrongRead=1;}return true;
}
struct NativeCanvasBackend {
 void* hud=nullptr;std::uint64_t generation=0;
 struct Access{void* clip=nullptr;void* get=nullptr;void* set=nullptr;};std::array<Access,11> handles{};
 bool Alive()const{return hud==g_canvas_live_hud.load()&&generation==g_canvas_lifetime.load();}
 bool Resolve(std::size_t i,const char* name,std::uintptr_t& id){
  static constexpr const char* names[]{"Hints","TutorialMessage","CursorOutline","CursorCross"};
  for(unsigned j=0;j<4;++j)if(!std::strcmp(name,names[j])){auto& c=clipsD69[j];if(!c.present)return false;
   handles[i].clip=&c;id=reinterpret_cast<std::uintptr_t>(&c);return Alive();}return false;
 }
 bool Get(std::size_t i,G& out){auto& c=*static_cast<ClipD69*>(handles[i].clip);if(animateD69){c.value.x+=3;c.value.y-=2;}
  out=c.value;if(c.wrongRead){--c.wrongRead;out.xs+=5;}return Alive();}
 bool Set(std::size_t i,G value){clipsD69[static_cast<ClipD69*>(handles[i].clip)-clipsD69].value=value;return Alive();}
};
#include "generated/item_prompt_owner_d69.inl"
static void nativeD69(float x=100,float y=150){for(unsigned i=0;i<4;++i){clipsD69[i]={};clipsD69[i].value={float(i*10),float(i*20),x,y,0};clipsD69[i].object=100+i;clipsD69[i].handle=200+i;}}
static int runD69(){
 nativeD69();ckD69(ApplyReadablePromptScale(true,.75f,.5f),"actual owner enabled");
 ckD69(nearD69(clipsD69[0].value.xs,75)&&nearD69(clipsD69[1].value.xs,50),"existing hint/control scales retained");
 for(unsigned i=2;i<4;++i)ckD69(nearD69(clipsD69[i].value.xs,50)&&nearD69(clipsD69[i].value.ys,75)&&nearD69(clipsD69[i].value.x,float(i*10)),"both item children follow existing Controls50%, position native");
 const auto initial=setsD69;ckD69(ApplyReadablePromptScale(true,.75f,.5f)&&setsD69==initial,"same settings do not compound or rewrite");
 animateD69=true;clipsD69[2].value.xs=120;ckD69(ApplyReadablePromptScale(true,.75f,.5f),"one native scale axis and animated position");
 ckD69(nearD69(clipsD69[2].value.xs,60)&&nearD69(clipsD69[2].value.ys,75)&&clipsD69[2].value.x>20,"single-axis repair preserves other axis and native movement");
 animateD69=false;ckD69(ApplyReadablePromptScale(false,.75f,.5f),"disabled actual owner restores");
 ckD69(nearD69(clipsD69[2].value.xs,120)&&nearD69(clipsD69[2].value.ys,150)&&!g_action_cursor_d69.Active(),"captured native baselines restored, lease released");
 ckD69(ApplyReadablePromptScale(true,.75f,1.5f),"existing slider upper range retained");
 ckD69(nearD69(clipsD69[2].value.xs,180)&&nearD69(clipsD69[3].value.ys,225),"150% custom scale no imposed50% default");
 ckD69(ApplyReadablePromptScale(true,.75f,.3f),"existing slider lower range");
 ckD69(nearD69(clipsD69[2].value.xs,36),"30% uses original native baseline");
 clipsD69[2].present=false;ckD69(ApplyReadablePromptScale(true,.75f,.5f),"temporary absent child tolerated");
 clipsD69[2].present=true;ckD69(ApplyReadablePromptScale(true,.75f,.5f)&&nearD69(clipsD69[2].value.xs,60),"same child retains native baseline across absence");
 // Exact native wrapper destruction/rebinding revision; no old baseline on reuse.
 g_action_cursor_children_d69.Remove(reinterpret_cast<std::uintptr_t>(&clipsD69[2]));
 clipsD69[2].value.xs=160;clipsD69[2].value.ys=180;
 ckD69(ApplyReadablePromptScale(true,.75f,.5f)&&nearD69(clipsD69[2].value.xs,80)&&nearD69(clipsD69[2].value.ys,90),"reused native wrapper starts a new baseline");
 ++clipsD69[3].object;++clipsD69[3].handle;clipsD69[3].value.xs=140;clipsD69[3].value.ys=160;
 ckD69(ApplyReadablePromptScale(true,.75f,.5f)&&nearD69(clipsD69[3].value.xs,70),"changed native object/handle identity rebinds");
 currentHudD69=reinterpret_cast<void*>(0x2000);++hudGenerationD69;nativeD69(80,120);
 ckD69(ApplyReadablePromptScale(true,.75f,.5f)&&nearD69(clipsD69[2].value.xs,40)&&nearD69(clipsD69[3].value.ys,60),"new HUD generation never borrows old scale");
 ckD69(ApplyReadablePromptScale(false,1,1),"restore before write failure tests");
 clipsD69[2].failSet=2;ckD69(!ApplyReadablePromptScale(true,.75f,.5f)&&g_action_cursor_d69.Active(),"failed write and rollback preserve repair debt");
 ckD69(ApplyReadablePromptScale(true,.75f,.5f)&&nearD69(clipsD69[2].value.xs,40),"next owner event repairs exact child");
 ckD69(ApplyReadablePromptScale(false,1,1),"restore before false-readback case");
 clipsD69[2].wrongAfterSet=1;
 ckD69(!ApplyReadablePromptScale(true,.75f,.5f)&&nearD69(clipsD69[2].value.xs,80),"false scale readback rolls back original native scale");
 ckD69(ApplyReadablePromptScale(true,.75f,.5f)&&nearD69(clipsD69[2].value.xs,40),"next owner event retries failed readback without compounding");
 const auto before=setsD69;clipsD69[2].value.xs=std::numeric_limits<float>::quiet_NaN();
 ckD69(!ApplyReadablePromptScale(true,.75f,.5f)&&setsD69==before,"invalid geometry is never written");
 availableD69=false;ckD69(!ApplyReadablePromptScale(true,.75f,.5f),"missing native HUD does not claim enabled success");
 ckD69(acquiresD69==releasesD69,"every actual owner call releases retained HUD holder");
 std::printf("D69 actual prompt owner %u checks PASS; native Flash leaves mocked; headset NOT_RUN\n",checksD69);return 0;
}
}
#ifndef KCD2VR_D69_NO_MAIN
int main(){return kcd2vr::flashui_probe::runD69();}
#endif
