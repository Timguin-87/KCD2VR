// R160 UI: one icon size (VrUiUnifiedScaleR160) + tutorial box position (VrUiTutorialYR160).
// factor 1 / offset 0 must give EXACTLY the old values, the factor reaches every HUD element once (parents for the
// readable children, own scale for the cccursor clips), crafting keeps its own size, and only tl + tutorialInDialog move.
#include "ui_hud_r160.h"
#include "ui_native_canvas.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
using namespace kcd2vr;
namespace u=ui_hud_r160;namespace nc=ui_native_canvas;
struct Parents {
 std::array<nc::Geometry,11> g{};
 Parents(){const float xy[11][2]={{0,0},{960,0},{1920,0},{0,900},{1920,900},{960,900},{0,1800},{960,1800},{1920,1800},{960,900},{960,900}};
   for(unsigned i=0;i<11;++i)g[i]={xy[i][0],xy[i][1],166.66667f,166.66667f,0};}
 bool Resolve(unsigned i,const char*,std::uintptr_t& id){id=i+1;return true;}
 bool Get(unsigned i,nc::Geometry& out){out=g[i];return true;}
 bool Set(unsigned i,nc::Geometry out){g[i]=out;return true;}
};
static bool near(float a,float b,float e=1e-4f){return std::fabs(a-b)<=e;}
static std::string ini(const char* key){
 std::ifstream f("KCD2VR.ini",std::ios::binary);std::string all((std::istreambuf_iterator<char>(f)),{});
 const auto p=all.find(std::string("\n")+key);if(p==std::string::npos)return "";
 auto e=all.find('=',p);auto end=all.find_first_of("\r\n",e);std::string v=all.substr(e+1,end-e-1);
 while(!v.empty()&&v[0]==' ')v.erase(0,1);return v;
}
int main(){
 // 1) defaults and clamps
 assert(u::kUnifiedDefault==1.f&&u::kTutorialYDefault==0.f&&u::Unified()==1.f&&u::TutorialY()==0.f);
 assert(u::ClampUnified(NAN)==1.f&&u::ClampUnified(.1f)==.5f&&u::ClampUnified(9.f)==1.5f);
 assert(u::ClampTutorialY(NAN)==0.f&&u::ClampTutorialY(-999.f)==-540.f&&u::ClampTutorialY(999.f)==540.f);
 // 2) factor 1 = every consumer value unchanged (inside each consumer's own clamp)
 for(float icon:{.5f,1.f,1.67f,1.75f,6.f})assert(u::ParentIconScale(icon,1.f)==icon);
 for(float c:{.25f,.5f,1.5f})assert(u::OwnScale(c,1.f,.25f,1.5f)==c);
 for(float h:{.25f,.64f,.9f,1.f})assert(u::OwnScale(h,1.f,.25f,1.f)==h);
 for(float m:{.25f,.4f,.45f,1.f})assert(u::StationScale(m,1.f)==m);
 assert(std::isnan(u::ParentIconScale(NAN,1.2f))); // refused downstream exactly as before
 // 3) the factor is applied once: parent x factor, crafting children / factor -> crafting visible size unchanged
 for(float f:{.5f,.8f,1.2f,1.5f})for(float m:{.3f,.4f,.6f}){
   const float parent=u::ParentIconScale(1.67f,f),child=u::StationScale(m,f);
   if(m/f>=.25f&&m/f<=1.f)assert(near(parent*child,1.67f*m,1e-4f)); // = the crafting slider size on the unscaled parent
 }
 assert(near(u::ParentIconScale(1.67f,1.2f),2.004f)&&u::ParentIconScale(5.f,1.5f)==6.f&&u::ParentIconScale(.6f,.5f)==.5f);
 assert(near(u::OwnScale(.25f,1.2f,.25f,1.5f),.3f)&&u::OwnScale(.25f,.5f,.25f,1.5f)==.25f&&u::OwnScale(.9f,1.5f,.25f,1.f)==1.f);
 // 4) menu mapping (vr_menu.cpp UnifiedScaleR160 / TutorialYR160): t 0..1 -> raw inside the clamps, 5 % / 5 px steps
 for(int k=0;k<=100;++k){const float t=k/100.f;
   const float s=std::round((.5f+t)*20.f)/20.f;assert(s>=u::kUnifiedMin&&s<=u::kUnifiedMax&&u::ClampUnified(s)==s);
   const float y=std::round((-540.f+t*1080.f)/5.f)*5.f;assert(y>=u::kTutorialYMin&&y<=u::kTutorialYMax);}
 // 5) tutorial position: 0 = byte-identical transaction, non-zero moves only tl(0) + tutorialInDialog(10), no compounding
 ui_surface_geometry::Hud hud{};hud.surface.valid=true;hud.surface.width=1920;hud.surface.height=1800;hud.safe={0,216,1920,864};
 const auto base=nc::ComputeAdjustableReadable49(hud,1,1,1,1,1,1.67f);assert(base.valid);
 Parents a,b;nc::Transaction ta,tb;assert(ta.Run(a,1,true,base).complete);
 auto zero=base;zero.tutorialY160=0.f;assert(tb.Run(b,1,true,zero).complete);
 for(unsigned i=0;i<11;++i)assert(nc::Same(a.g[i],b.g[i]));
 auto down=base;down.tutorialY160=120.f;assert(tb.Run(b,1,true,down).complete);
 const float dy=120.f*base.sy*1800.f/1080.f;
 for(unsigned i=0;i<11;++i){
   if(i==0||i==10)assert(near(b.g[i].y-a.g[i].y,dy,.05f)&&near(b.g[i].x,a.g[i].x,.05f)&&near(b.g[i].xs,a.g[i].xs,.02f));
   else assert(nc::Same(b.g[i],a.g[i]));}
 assert(tb.Run(b,1,true,zero).complete);for(unsigned i=0;i<11;++i)assert(nc::Same(a.g[i],b.g[i]));
 // the dialogue offset and the tutorial offset are independent
 auto both=base;both.tutorialY160=-60.f;both.dialogueY160=40.f;Parents c;nc::Transaction tc;assert(tc.Run(c,1,true,both).complete);
 assert(near(c.g[0].y-a.g[0].y,-60.f*base.sy*1800.f/1080.f,.05f)&&near(c.g[3].y-a.g[3].y,40.f*base.sy*1800.f/1080.f,.05f));
 // 6) the shipped INI: new keys neutral, dialogue default = 80 % of the old size
 assert(ini("VrUiUnifiedScaleR160")=="1"&&ini("VrUiTutorialYR160")=="0");
 const float dlg=std::stof(ini("VrUiDialogueScale"));assert(near(nc::DialogueFactorR160(dlg),.80f,.01f));
 std::printf("PASS R160 UI: factor 1 / offset 0 = old values, factor once per element, crafting keeps its size, tutorial moves tl + tutorialInDialog only, dialogue default %.0f%%\n",double(nc::DialogueFactorR160(dlg)*100.f));
}
