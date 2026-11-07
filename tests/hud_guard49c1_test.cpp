#include "ui_native_canvas.h"
#include "vrik/guard_parry176.h"
#include "vrik/physical_weapon.h"
#include <cassert>
#include <limits>
using namespace kcd2vr;
int main(){
 namespace ui=ui_native_canvas;namespace gp=guard_parry176;
 ui_surface_geometry::Hud hud{};hud.surface.valid=true;hud.surface.width=1920;hud.surface.height=1800;hud.safe={0,216,1920,864};
 ui::NativeFrame native{0,0,1920,1800,1920,1800,true};
 ui::Geometry top{0,0,166.66667f,166.66667f,0},bottom{1920,1800,166.66667f,166.66667f,0},middle{960,900,166.66667f,166.66667f,0};
 auto fit=ui::ComputeAdjustableReadable49(hud,1,1,1,1,1);assert(fit.valid);
 auto t=ui::MapLive(top,fit,native),b=ui::MapLive(bottom,fit,native),c=ui::MapLive(middle,fit,native);
 auto upper=ui::ComputeAdjustableReadable49(hud,1,1,1,1.8f,1);
 assert(ui::MapLive(top,upper,native).y<t.y);assert(ui::Same(ui::MapLive(bottom,upper,native),b));
 assert(ui::Same(ui::MapLive(middle,upper,native),c));
 auto lower=ui::ComputeAdjustableReadable49(hud,1,1,1,1,1.8f);
 assert(ui::MapLive(bottom,lower,native).y>b.y);assert(ui::Same(ui::MapLive(top,lower,native),t));
 auto wider=ui::ComputeAdjustableReadable49(hud,1,1.4f,1,1,1);
 assert(ui::MapLive(top,wider,native).x<t.x);assert(ui::MapLive(top,wider,native).y==t.y);
 auto taller=ui::ComputeAdjustableReadable49(hud,1,1,1.4f,1,1);
 assert(ui::MapLive(top,taller,native).y<t.y);assert(ui::MapLive(top,taller,native).xs==t.xs&&ui::MapLive(top,taller,native).ys==t.ys);
 assert(!ui::ComputeAdjustableReadable49(hud,1,1,std::numeric_limits<float>::quiet_NaN(),1,1).valid);
 // Independent icon magnification changes neither border nor placement.
 for(float icon:{.5f,1.f,2.f,3.f}){
  auto larger=ui::ComputeAdjustableReadable49(hud,1,1,1,1,1,icon);assert(larger.valid);
  const auto x=ui::MapLive(top,larger,native);
  assert(x.x==t.x&&x.y==t.y);assert(std::fabs(x.xs-t.xs*icon)<.01f&&std::fabs(x.ys-t.ys*icon)<.01f);
  assert(std::fabs((x.xs/x.ys)-(t.xs/t.ys))<.001f);
 }
 assert(!ui::ComputeAdjustableReadable49(hud,1,1,1,1,1,std::numeric_limits<float>::quiet_NaN()).valid);
 // Deliberate diagonal forward jab lies inside the permissive guard cone.
 gp::ThrustExit49 motion{};gp::V axis{.6,.8,0};bool exit=false;
 for(int i=0;i<3;++i)exit=gp::thrustExit49(motion,true,.01,axis*.015,axis*.015,axis,axis,{0,1,0});
 assert(exit); // 1.5 m/s jab can exit without a 3.5 m/s slash speed.
 motion={};assert(!gp::thrustExit49(motion,true,.01,axis*.1,axis*.1,axis,axis,{0,1,0}));
 assert(!gp::thrustExit49(motion,true,.01,{},{},axis,axis,{0,1,0}));assert(motion.time==0);
 for(int i=0;i<10;++i)assert(!gp::thrustExit49(motion,true,.01,{.02,0,0},{.02,0,0},axis,axis,{0,1,0}));
 for(int i=0;i<10;++i)assert(!gp::thrustExit49(motion,true,.01,axis*.02,{},axis,axis,{0,1,0}));
 assert(!gp::thrustExit49(motion,true,.2,axis*.3,axis*.3,axis,axis,{0,1,0}));
 // Full producer: a moderate diagonal jab leaves guard and commits a lower-zone
 // stab with a separate physical starting side. No native hit is manufactured.
 namespace ph=physical;ph::Input in{};ph::State state{};std::array<ph::Profile,2> profiles{};
 in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;
 in.naturalCombat117=in.coherentMelee119=in.positionGuard176=in.controllerAnchor119=in.heldMotionAttacks=true;
 in.dynamics=in.contact=false;in.grip[0]=1;in.epoch=in.bodyEpoch=in.sample=1;in.tick=1000;
 in.shoulders[0]={.15,0,1.3};in.shoulders[1]={-.15,0,1.3};in.reach[0]=1.5;
 in.hands[0].p={-.18,.3,1.45};in.intentHands[0]=in.hands[0];
 auto& p=profiles[0];p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;p.axis=axis;p.tip=axis*.6;
 auto clear=[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.;};
 auto output=ph::step(state,in,profiles,clear);assert(output.guard117&&output.owned[0]);
 unsigned prepares=0,commits=0;
 for(int i=0;i<18;++i){++in.sample;in.tick+=10;in.hands[0].p=in.hands[0].p+axis*.015;in.intentHands[0]=in.hands[0];
  output=ph::step(state,in,profiles,clear);
  for(unsigned j=0;j<output.commandCount;++j){const auto& c=output.commands[j];
   if(c.kind==ph::CommandKind::Prepare){++prepares;assert(c.stroke117==ph::CommandKind::Thrust&&c.zone117==5&&c.originZone167>=0);}
   if(c.kind==ph::CommandKind::Thrust){++commits;assert(c.zone117==5&&c.originZone167>=0);}
  }
 }
 assert(prepares==1&&commits==1);
}
