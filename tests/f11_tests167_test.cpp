#include "vrik/f11_tests167.h"
#include "vrik/fist_motion.h"
#include "vrik/physical_weapon.h"
#include "diagnostics/combat_capture.h"
#include <cassert>
#include <cstdio>
#include <limits>
#include <vector>
namespace ft=kcd2vr::f11_tests167;
namespace fm=kcd2vr::fists;
namespace ph=kcd2vr::physical;
namespace to=kcd2vr::thrust_origin167;
namespace cc=kcd2vr::diagnostics::combat_capture;
struct Boxing {
 fm::State state{};fm::Sample in{};
 Boxing(){in.epoch=in.sequence=1;in.tick=1000;in.actor=1;in.valid=in.empty[0]=in.empty[1]=in.boxing167=true;
  in.head={0,1.6,0};in.controller[0]=in.allowed[0]={.2,1.3,-.2};in.controller[1]=in.allowed[1]={-.2,1.3,-.2};fm::step(state,in);
  for(int i=0;i<2;++i)in.grip[i]=in.trigger[i]=1;for(int i=0;i<20;++i)move(0,{});
 }
 fm::Punch move(int side,ph::Vec3 delta,bool allowed=true){in.tick+=10;++in.sequence;in.controller[side]=in.controller[side]+delta;
  if(allowed)in.allowed[side]=in.allowed[side]+delta;return fm::step(state,in);}
};
struct Stab {
 ph::Input in{};ph::State state{};std::array<ph::Profile,2> p{};std::vector<ph::Command> commands;
 Stab(ph::Vec3 start,bool enabled=true,double yaw=0){in.epoch=in.bodyEpoch=in.sample=1;in.tick=1000;
  in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.grip[0]=1;in.contact=in.naturalCombat117=in.heldMotionAttacks=in.requirePrimaryGrip114=true;
  in.coherentMelee119=in.controllerAnchor119=true;in.directionalStabs167=enabled;in.dynamics=false;
  auto q=ph::m::from_rotation_vector({0,0,yaw});in.shoulders[0]=ph::m::rotate(q,{.2,0,1.3});in.shoulders[1]=ph::m::rotate(q,{-.2,0,1.3});in.reach[0]=1.5;
  in.hands[0]={q,ph::m::rotate(q,start)};in.intentHands[0]=in.hands[0];
  p[0].key=p[0].generation=1;p[0].classId=4;p[0].primary=p[0].geometry=true;p[0].axis={0,1,0};p[0].tip={0,.6,0};
  for(int n=0;n<15;++n)move({});
 }
 void move(ph::Vec3 d){in.tick+=10;++in.sample;in.hands[0].p=in.hands[0].p+d;in.intentHands[0].p=in.intentHands[0].p+d;
  auto out=ph::step(state,in,p,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return ph::Contact103{};});
  for(unsigned i=0;i<out.commandCount;++i)commands.push_back(out.commands[i]);
 }
 void check(int z){int prepared=0,committed=0;for(auto c:commands){if(c.kind==ph::CommandKind::Prepare){++prepared;assert(c.stroke117==ph::CommandKind::Thrust);assert(c.zone117==z);}
  if(c.kind==ph::CommandKind::Thrust){++committed;assert(c.zone117==z);assert(c.originZone167==(in.directionalStabs167?z:-1));}}
  assert(prepared==1&&committed==1);
 }
};
int main(){
 ft::Lease lease;ft::Modes base{1,1,0,0},restored{};lease.begin(base);lease.selected=ft::preset(1);lease.begin(ft::preset(1));lease.selected=ft::preset(2);
 assert(lease.end(ft::preset(2),restored)&&ft::same(restored,base));assert(!lease.owned);
 lease.begin(base);lease.selected=ft::preset(1);assert(!lease.end(ft::Modes{2,3,5,0},restored));
 assert(ft::same(ft::preset(1),{2,0,5,0})&&ft::same(ft::preset(2),{3,0,3,7}));
 assert(to::zone({.3,.5,0})==2&&to::zone({-.3,.5,0})==1&&to::zone({0,.5,.3})==0&&to::zone({0,.5,-.3})==5);
 assert(to::zone({.01,1,.01})==-1&&to::zone({std::numeric_limits<double>::quiet_NaN(),0,0})==-1);
 for(int z:{0,1,2,5})assert(to::opposite(to::opposite(z))==z);
 ft::Touch touch;assert(!touch.update(0,true,false,1,1,1000));assert(touch.update(0,true,true,1,2,1010));
 for(int n=0;n<100;++n)assert(!touch.update(0,true,true,1,n+3,1020+n*10));
 assert(!touch.update(0,false,true,1,104,2100));assert(!touch.update(0,true,false,1,105,2110));assert(touch.update(0,true,true,1,106,2120));
 assert(touch.update(1,true,true,1,106,2120));assert(!touch.update(1,true,false,1,107,2130));assert(!touch.update(1,true,true,1,108,2140));
 for(int side:{0,1})for(int kind=0;kind<3;++kind){Boxing b;ph::Vec3 d=kind==0?ph::Vec3{0,0,-.02}:kind==1?ph::Vec3{.02,0,0}:ph::Vec3{0,.02,0};int hits=0;
  for(int n=0;n<12;++n){auto hit=b.move(side,d);if(hit.valid){++hits;assert(hit.side==side&&hit.motion167==kind);}}assert(hits==1);
  for(int n=0;n<12;++n)assert(!b.move(side,d).valid);
  for(int n=0;n<12;++n)assert(!b.move(side,d*-1).valid);for(int n=0;n<9;++n)assert(!b.move(side,{}).valid);
  hits=0;for(int n=0;n<12;++n)hits+=b.move(side,d).valid;assert(hits==1);
 }
 {Boxing b;for(int n=0;n<200;++n)assert(!b.move(0,{n%2?.02:-.02,0,0}).valid);}
 {Boxing b;for(int n=0;n<100;++n)assert(!b.move(0,{0,0,-.003}).valid);}
 {Boxing b;for(int n=0;n<12;++n)assert(!b.move(0,{0,0,-.02},false).valid);}
 {Boxing b;b.in.valid=false;assert(!b.move(0,{}).valid);b.in.valid=true;for(int n=0;n<12;++n)assert(!b.move(0,{0,0,-.02}).valid);}
 {Boxing b;b.in.empty[1]=false;for(int n=0;n<12;++n)assert(!b.move(0,{0,0,-.02}).valid);}
 for(auto pair:std::vector<std::pair<ph::Vec3,int>>{{{.3,.2,1.12},2},{{-.3,.2,1.12},1},{{0,.2,1.50},0},{{0,.2,.82},5}}){
  for(double yaw:{0.,1.5707963267948966}){Stab s(pair.first,true,yaw);auto d=ph::m::rotate(ph::m::from_rotation_vector({0,0,yaw}),{0,.025,0});for(int n=0;n<16;++n)s.move(d);s.check(pair.second);}
  Stab legacy(pair.first,false);for(int n=0;n<16;++n)legacy.move({0,.025,0});legacy.check(5);
 }
 for(auto pair:std::vector<std::pair<ph::Vec3,int>>{{{.3,.2,1.12},2},{{-.3,.2,1.12},1},{{0,.2,1.50},0},{{0,.2,.82},5}}){
  Stab s(pair.first,false);s.in.positionGuard176=true;
  for(int n=0;n<16;++n)s.move({0,.025,0});int prepares=0,commits=0;
  for(auto c:s.commands){if(c.kind==ph::CommandKind::Prepare||c.kind==ph::CommandKind::Thrust){
    assert(c.stroke117==ph::CommandKind::Thrust&&c.zone117==5&&c.originZone167==pair.second);
    prepares+=c.kind==ph::CommandKind::Prepare;commits+=c.kind==ph::CommandKind::Thrust;}}
  assert(prepares==1&&commits==1);
 }
 // A diagonal forward jab crosses the centre after its start. Origin stays LEFT.
 {Stab s({-.11,.2,1.12});for(int n=0;n<16;++n)s.move({.008,.025,0});s.check(1);assert(s.in.hands[0].p.x>0);}
 // Retired preset mathematics/provenance remains tested, but it cannot enable a recorder.
 cc::selectSources(cc::F11Hands167,1000,true);assert(!cc::active(1001));
 namespace cap=kcd2vr::diagnostics::capture168;cap::gate.select(cap::Domain::Combat,true,1000);
 assert(cc::active(180999)&&!cc::active(181000));
 cc::Event e{};e.kind=cc::Kind::F11Native167;e.tick=1001;assert(cc::push(e));assert(cc::pop(e)&&cc::f11Rung167(e.source)==1);
 cc::selectSources(cc::F11Stabs167,2000,true);cap::gate.select(cap::Domain::Combat,true,2000);e.tick=2001;assert(cc::push(e));assert(cc::pop(e)&&cc::f11Rung167(e.source)==2);
 std::puts("Historical preset mathematics/provenance retained; explicit Combat recorder authority, boxing/retraction, touch cues and thrust origins PASS (current hotkeys tested separately)");
}
