// R160: dialogue text size / vertical position for the readable-parent HUD owner (always on since 44c2).
// the shipped defaults (VrUiDialogueScale 0.71, VrUiDialogueY 420) must give EXACTLY the old placement, other
// sliders move/size only the dialogue parents cl/cr, and the INI clamp + slider range stay consistent.
#include "ui_native_canvas.h"
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace kcd2vr;
namespace nc=ui_native_canvas;
struct Parents {
 std::array<nc::Geometry,11> g{};
 Parents(){const float xy[11][2]={{0,0},{960,0},{1920,0},{0,900},{1920,900},{960,900},{0,1800},{960,1800},{1920,1800},{960,900},{960,900}};
   for(unsigned i=0;i<11;++i)g[i]={xy[i][0],xy[i][1],166.66667f,166.66667f,0};}
 bool Resolve(unsigned i,const char*,std::uintptr_t& id){id=i+1;return true;}
 bool Get(unsigned i,nc::Geometry& out){out=g[i];return true;}
 bool Set(unsigned i,nc::Geometry out){g[i]=out;return true;}
};
static bool near(float a,float b,float e=.01f){return std::fabs(a-b)<=e;}
int main(){
 // neutral point = shipped defaults
 assert(near(nc::DialogueFactorR160(.71f),1.f,1e-6f));
 assert(near(nc::DialogueOffsetR160(420.f),0.f,1e-6f));
 // clamps (INI clamp is .30..1.50 and -720..720; the usable vertical range is -540..+300 around the neutral)
 assert(near(nc::DialogueFactorR160(.30f),.30f/.71f,1e-5f)&&near(nc::DialogueFactorR160(5.f),1.50f/.71f,1e-5f));
 assert(near(nc::DialogueOffsetR160(-720.f),-540.f)&&near(nc::DialogueOffsetR160(720.f),300.f)&&near(nc::DialogueOffsetR160(-120.f),-540.f));
 assert(nc::DialogueFactorR160(NAN)==1.f&&nc::DialogueOffsetR160(NAN)==0.f);
 assert(nc::kDialogueYMinR160>=-720.f&&nc::kDialogueYMaxR160<=720.f); // slider range inside the INI clamp
 // the menu slider mapping (vr_menu.cpp UiDialogueY on INTERACTION / PROMPTS): t=0..1 -> raw, rounded to 5
 for(int k=0;k<=100;++k){const float t=k/100.f;float raw=nc::kDialogueYMinR160+t*(nc::kDialogueYMaxR160-nc::kDialogueYMinR160);raw=std::round(raw/5.f)*5.f;
   assert(raw>=-720.f&&raw<=720.f);const float off=nc::DialogueOffsetR160(raw);assert(off>=-540.f&&off<=300.f);}

 ui_surface_geometry::Hud hud{};hud.surface.valid=true;hud.surface.width=1920;hud.surface.height=1800;hud.safe={0,216,1920,864};
 const auto base=nc::ComputeAdjustableReadable49(hud,1,1,1,1,1,1);assert(base.valid);
 // 1) defaults: identical to the pre-R160 result (Fit without dialogue fields)
 Parents a,b;nc::Transaction ta,tb;
 assert(ta.Run(a,1,true,base).complete);
 auto def=base;def.dialogue160=nc::DialogueFactorR160(.71f);def.dialogueY160=nc::DialogueOffsetR160(420.f);
 assert(tb.Run(b,1,true,def).complete);
 for(unsigned i=0;i<11;++i)assert(nc::Same(a.g[i],b.g[i]));
 // 2) bigger + lower: only cl(3)/cr(4) change; x stays, scale x factor, y moves down by offset*box scale
 Parents c;nc::Transaction tc;auto mod=base;mod.dialogue160=nc::DialogueFactorR160(1.065f);mod.dialogueY160=nc::DialogueOffsetR160(620.f);
 assert(near(mod.dialogue160,1.5f,1e-4f)&&near(mod.dialogueY160,200.f));
 assert(tc.Run(c,1,true,mod).complete);
 for(unsigned i=0;i<11;++i){
   if(i==3||i==4){
     assert(near(c.g[i].x,a.g[i].x)&&near(c.g[i].xs,a.g[i].xs*1.5f,.05f)&&near(c.g[i].ys,a.g[i].ys*1.5f,.05f));
     const float expectDy=200.f*base.sy*1800.f/1080.f;
     assert(near(c.g[i].y-a.g[i].y,expectDy,.05f)&&c.g[i].y>a.g[i].y);
   }else assert(nc::Same(c.g[i],a.g[i]));
 }
 // 3) slider change on the same HUD: no compounding, back to defaults = exact old placement again
 assert(tc.Run(c,1,true,def).complete);for(unsigned i=0;i<11;++i)assert(nc::Same(c.g[i],a.g[i]));
 // 4) restore gives the native geometry back
 const Parents fresh;assert(tc.Run(c,1,false).complete);for(unsigned i=0;i<11;++i)assert(nc::Same(c.g[i],fresh.g[i]));
 std::puts("PASS R160 dialogue size/vertical: defaults = old placement, only cl/cr move/scale, no compounding, restore native");
}
