#include "ui_palm_alpha44c2.h"
#include "ui_native_canvas.h"
#include "ui_hud_experiment36c5.h"
#include <cassert>
#include <cstring>
using namespace kcd2vr;
struct IO {
 bool failOnce=false;unsigned calls=0;std::array<float,11> alpha{};std::array<bool,11> visible{};
 IO(){alpha.fill(100);visible.fill(true);visible[0]=false;}
 bool Acquire(){++calls;return true;}void Release(){++calls;}
 bool Resolve(unsigned,const char*){++calls;return true;}
 bool Read(unsigned i,float& a){++calls;a=alpha[i];return true;}
 bool Write(unsigned i,float a){++calls;alpha[i]=a;if(failOnce){failOnce=false;return false;}return true;}
};
struct Parents {
 std::array<ui_native_canvas::Geometry,11> g{};
 Parents(){const float xy[11][2]={{0,0},{960,0},{1920,0},{0,900},{1920,900},{960,900},{0,1800},{960,1800},{1920,1800},{960,900},{960,900}};
   for(unsigned i=0;i<11;++i)g[i]={xy[i][0],xy[i][1],166.66667f,166.66667f,0};}
 bool Resolve(unsigned i,const char*,std::uintptr_t& id){id=i+1;return true;}
 bool Get(unsigned i,ui_native_canvas::Geometry& out){out=g[i];return true;}
 bool Set(unsigned i,ui_native_canvas::Geometry out){g[i]=out;return true;}
};
int main(){ui_hud_experiment36c5::PalmReveal::dwellMs=0; // pre-C22b instant-reveal contract (dwell tested in hud_experiment36c5)
 IO failure;failure.failOnce=true;ui_palm_alpha44c2::Controller recovery;
 assert(recovery.Update(failure,{true,false,false,false,1}).failures==1);
 recovery.Update(failure,{false,false,false,false,1});for(float x:failure.alpha)assert(x==100);
 IO io;ui_palm_alpha44c2::Controller c;
 auto a=c.Update(io,{true,false,false,false,1});assert(a.changed&&a.failures==0&&io.alpha[0]==0&&io.alpha[1]==0&&io.alpha[6]==100);
 // Vitals follow the left palm; full-field vignette always remains native.
 assert(io.alpha[1]==0&&io.alpha[6]==100);
 const auto calls=io.calls;
 for(int i=0;i<10000;++i)assert(!c.Update(io,{true,false,false,false,1}).changed);
 assert(calls==io.calls);
 c.Update(io,{true,false,true,false,1});assert(io.alpha[0]==100&&io.alpha[1]==0&&!io.visible[0]);
 c.Update(io,{true,true,false,false,1});assert(io.alpha[0]==0&&io.alpha[1]==100&&io.alpha[2]==100&&io.alpha[7]==100); // C10: status (Buffs) follows the LEFT palm; GameLog is native
 c.Update(io,{true,false,true,true,1});assert(io.alpha[1]==0&&io.alpha[2]==0&&io.alpha[7]==100); // C10: the gated clip is the Stats CHILD, the selectors' parent is never touched (qam exception moot)
 c.Update(io,{false,false,false,false,1});for(float x:io.alpha)assert(x==100);
 c.Update(io,{true,false,false,false,1});io.alpha.fill(80); // new HUD lifetime never restores dead baselines
 c.Update(io,{true,true,true,false,2});assert(io.alpha[0]==100&&io.alpha[1]==100); // C54: a requested compass reveal is explicit full alpha, like vitals
 c.Update(io,{false,false,false,false,2});assert(io.alpha[1]==80); // new native baseline restored, not old100
 IO hidden;hidden.alpha[1]=0;ui_palm_alpha44c2::Controller hiddenStats;
 hiddenStats.Update(hidden,{true,true,false,false,3});assert(hidden.alpha[1]==100);
 hiddenStats.Update(hidden,{true,false,false,false,3});assert(hidden.alpha[1]==0);
 hiddenStats.Update(hidden,{false,false,false,false,3});assert(hidden.alpha[1]==0);
 IO nativeChange;nativeChange.alpha[1]=0;ui_palm_alpha44c2::Controller changedStats;
 changedStats.Update(nativeChange,{true,true,false,false,4});nativeChange.alpha[1]=42;
 changedStats.Update(nativeChange,{false,false,false,false,4});assert(nativeChange.alpha[1]==42);
 {// R59.16C10R16: hidden stays hidden - a palm-hidden gate the game made visible again is hidden again, the game's value is
  // adopted for the exit restore, revealed and native gates are never touched, and nothing is written when nothing changed.
  IO r;ui_palm_alpha44c2::Controller rc;rc.Update(r,{true,false,false,false,5});assert(r.alpha[0]==0&&r.alpha[1]==0&&r.alpha[6]==100);
  r.alpha[0]=70;r.alpha[1]=100;r.alpha[6]=55;
  auto rr=rc.Reassert(r);assert(rr.writes==2&&r.alpha[0]==0&&r.alpha[1]==0&&r.alpha[6]==55);
  const auto before=r.calls;assert(rc.Reassert(r).writes==0&&r.calls>before);
  rc.Update(r,{true,true,false,false,5});assert(r.alpha[1]==100&&rc.Reassert(r).writes==0&&r.alpha[1]==100);
  rc.Update(r,{false,false,false,false,5});assert(r.alpha[0]==100&&r.alpha[1]==100); // a mid-fade sample (70) is not adopted for the exit restore
  ui_palm_alpha44c2::Controller idle;assert(idle.Reassert(r).writes==0);}
 ui_surface_geometry::Hud hud{};hud.surface.valid=true;hud.surface.width=1920;hud.surface.height=1800;
 hud.safe={0,216,1920,864};
 auto fit=ui_native_canvas::ComputeReadable(hud,.8f,1,3,3);assert(fit.valid);
 auto lo=ui_native_canvas::Map({0,1080,100,100,0},fit),hi=ui_native_canvas::Map({1920,0,100,100,0},fit);
 assert(lo.y<hud.safe.bottom&&hi.y>hud.safe.top&&hi.x<hud.safe.right);
 assert(std::fabs((fit.ty+540*fit.sy)-540)<.01f);
 Parents parents;const auto original=parents.g;ui_native_canvas::Transaction transaction;
 auto result=transaction.Run(parents,1,true,fit);assert(result.complete&&result.written);
 const auto fittedBottom=parents.g[7];
 // Native child regeneration changes the child's data, not this ancestor transform.
 for(float childY:{-60.f,-85.f,-110.f}){
   const float inheritedY=fittedBottom.y+childY*fittedBottom.ys/100.f;
   assert(inheritedY<hud.safe.bottom/1080.f*1800.f&&inheritedY>hud.safe.top/1080.f*1800.f);
 }
 assert(transaction.Run(parents,1,false).complete);
 for(unsigned i=0;i<11;++i)assert(ui_native_canvas::Same(parents.g[i],original[i]));
 auto large=ui_native_canvas::ComputeAdjustableReadable49(hud,1,1,1,1,1,6);
 auto normal=ui_native_canvas::ComputeAdjustableReadable49(hud,1,1,1,1,1,1);
 const auto frame=ui_native_canvas::InferFrame(original);
 const auto lg=ui_native_canvas::MapLive(original[7],large,frame),ng=ui_native_canvas::MapLive(original[7],normal,frame);
 assert(large.valid&&lg.x==ng.x&&lg.y==ng.y&&std::fabs(lg.xs-ng.xs*6)<.01f&&std::fabs(lg.ys-ng.ys*6)<.01f);
 auto tiny=ui_native_canvas::ComputeReadable(hud,.25f,1,1,1);assert(tiny.valid&&tiny.sy<fit.sy);
 for(const char* excluded:{"fader","vignette","bbc","ratioStrips","popUp","cccursor"})
   for(auto* name:ui_native_canvas::Anchors)assert(std::strcmp(name,excluded));
 ui_hud_experiment36c5::PalmReveal palm;ui_contextual_hud162r1::Input in{};
 in.enabled=in.vrik=in.gameplay=in.headValid=true;in.now=100;
 in.palm[1].valid=true;in.palm[1].position={0,0,-.4f};in.palm[1].orientation={0,.70710678f,0,.70710678f};
 auto o=palm.Update(in,{false,true});assert(o.right&&!o.left);
 in.now=101;in.palm[1].position={0,0,.4f};o=palm.Update(in,{false,false});assert(o.right);
 in.now=10100;o=palm.Update(in,{false,false});assert(!o.right);
 in.vrik=false;assert(palm.Update(in,{true,true}).active); // R59.16C10R16: no VRIK requirement
 in.gameplay=false;assert(!palm.Update(in,{true,true}).active);
}
