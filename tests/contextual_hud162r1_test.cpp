#include "ui_contextual_hud162r1.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::ui_contextual_hud162r1;
struct Mock {
 std::array<float,Clips.size()> x{},y{};int fail=-1,missing=-1;bool failed=false;int sets=0;
 Mock(){for(unsigned i=0;i<Clips.size();++i){x[i]=80.f+i;y[i]=110.f+i;}}
 bool Resolve(unsigned i,const char*){return int(i)!=missing;}
 bool Get(unsigned i,float&a,float&b){a=x[i];b=y[i];return true;}
 bool Set(unsigned i,float a,float b){x[i]=a;y[i]=b;++sets;if(int(i)==fail&&!failed){failed=true;return false;}return true;}
};
int main(){
 Input in{};in.enabled=in.vrik=in.gameplay=in.headValid=true;
 in.palm[0]={{0,0,-.45f},{0,-.70710678f,0,.70710678f},true};
 in.palm[1]={{0,0,-.45f},{0,.70710678f,0,.70710678f},true};
 Gaze gaze;assert(!gaze.Update(in).left);in.now=99;assert(!gaze.Update(in).left);in.now=100;
 auto out=gaze.Update(in);assert(out.left&&out.right&&out.active);assert(Hidden(out)==Other);
 // Back-of-hand presentation is accepted too; an edge-on hand leaves after hysteresis.
 in.palm[1].orientation={0,-.70710678f,0,.70710678f};assert(Looking(in,1,false));
 in.palm[0].orientation={0,.70710678f,0,.70710678f};assert(Looking(in,0,false));
 in.palm[1].orientation={0,0,0,1};in.now=120;assert(gaze.Update(in).right);
 in.now=279;assert(gaze.Update(in).right);in.now=280;assert(!gaze.Update(in).right);
 in.palm[0].valid=false;assert(!gaze.Update(in).left);
 in.vrik=false;assert(gaze.Update(in).active&&Hidden(gaze.Update(in))!=0); // R59.16C10R16: the palm HUD no longer needs VRIK
 in.vrik=true;in.gameplay=false;assert(!gaze.Update(in).active);
 in.gameplay=true;in.headValid=false;assert(Hidden(gaze.Update(in))==7);
 in.headValid=true;in.palm[0].valid=true;in.palm[0].position={0,0,.45f};assert(!Looking(in,0,false));
 in.palm[0].position={0,0,-4};assert(!Looking(in,0,false));
 in.palm[0].position={0,0,-.45f};in.headOrientation.w=2;assert(!Looking(in,0,false));
 Mock m;const auto originalX=m.x,originalY=m.y;
 {DrawMask<Mock> mask(m);assert(mask.Begin(Left));assert(m.x[0]==0);assert(m.x[1]==originalX[1]&&m.y[2]==originalY[2]);assert(mask.Restore());assert(m.x==originalX&&m.y==originalY);}
 {DrawMask<Mock> mask(m);assert(mask.Begin(7));for(unsigned i=0;i<16;++i)assert(m.x[i]==0);assert(m.x[16]==originalX[16]);}
 assert(m.x==originalX&&m.y==originalY);
 m.fail=3;{DrawMask<Mock> mask(m);assert(!mask.Begin(7));}assert(m.x==originalX&&m.y==originalY);
 {DrawMask<Mock> mask(m);assert(mask.Begin(0,.5f,.25f,.7f));assert(m.x[16]==originalX[16]*.5f&&m.y[17]==originalY[17]*.25f);}
 assert(m.x==originalX&&m.y==originalY);
 m.x[16]=150.f;{DrawMask<Mock> mask(m);assert(mask.Begin(0,.5f));assert(m.x[16]==75.f);}assert(m.x[16]==150.f);
 m.missing=16;{DrawMask<Mock> mask(m);assert(mask.Begin(0,.5f));}assert(m.x[16]==150.f);
 {DrawMask<Mock> mask(m);assert(mask.Begin(HideCrosshair163));
  assert(m.x[19]==0&&m.x[13]==0&&m.x[0]==originalX[0]);}
 assert(m.x[19]==originalX[19]);
 {DrawMask<Mock> mask(m);assert(mask.Begin(HideCompass163));assert(m.x[0]==0&&m.x[1]==originalX[1]);}
 {DrawMask<Mock> mask(m);assert(mask.Begin(HideStatus163));assert(m.x[1]==originalX[1]&&m.x[2]==0&&m.x[0]==originalX[0]);}
 {DrawMask<Mock> mask(m);assert(!mask.Begin(7,0.f));}assert(m.x[0]==originalX[0]);
 std::puts("PASS palm dwell/hysteresis/orientation/tracking/native gates and scoped mask rollback/restoration");
}
