#include "vrik/physical_weapon.h"
#include <cassert>
#include <cstdio>
namespace p=kcd2vr::physical;namespace c=kcd2vr::own_shield_constraint60;namespace m=kcd2vr::pose_core;
static std::array<p::Profile,2> profiles(){std::array<p::Profile,2> a{};
 for(int s=0;s<2;++s){auto& x=a[s];x.key=10+s;x.generation=20+s;x.entity=30+s;x.side=s;x.primary=x.geometry=x.diagnosticBounds59c6=true;}
 a[0].classId=4;a[0].tip={0,1,0};a[0].extent=1;
 a[1].classId=8;a[1].diagnosticBoundsMin59c6={-.01,.1,-.4};a[1].diagnosticBoundsMax59c6={.01,.9,.4};a[1].tip={0,.9,0};
 return a;
}
static p::Pose pose(double x){p::Pose a{};a.p.x=x;return a;}
static std::array<p::CheckedPath60,2> paths(double a=.2,double b=-.2){std::array<p::CheckedPath60,2> x{};
 x[0].begin(pose(a),pose(b),true);x[1].begin(pose(0),pose(0),true);return x;}
template<class Profiles,class Paths> static void assertSafe(const Profiles& ps,const Paths& path,const c::Result& r){
 assert(r.usable&&r.limited&&r.fraction>=0&&r.fraction<1&&r.queries<=65);
 for(unsigned i=0;i<=100;++i){const double t=r.fraction*i/100.;
  const auto z=kcd2vr::self_contact54::nearest(c::blade(ps[0],c::at(ps[0],path[0],t)),c::box(ps[1],c::at(ps[1],path[1],t)));
  assert(z.valid&&z.gap>=.00199);
 }
}
int main(){auto ps=profiles();auto path=paths();
 auto r=c::sweep(ps,path,0,1,1.);assertSafe(ps,path,r);assert(r.fraction>.40&&r.fraction<.48);
 // Both separated endpoints cross the complete thin shield. A mere endpoint
 // overlap test would miss this; conservative advancement stops before entry.
 assert(kcd2vr::self_contact54::nearest(c::blade(ps[0],path[0].point[1]),c::box(ps[1],path[1].point[1])).gap>.1);
 // Shield motion alone against a stationary sword, and simultaneous approach.
 path=paths(.2,.2);path[1].begin(pose(0),pose(.4),true);r=c::sweep(ps,path,0,1,1.);assertSafe(ps,path,r);
 path=paths(.2,0);path[1].begin(pose(0),pose(.2),true);r=c::sweep(ps,path,0,1,1.);assertSafe(ps,path,r);
 // A rotating shield sweeps through a stationary blade although its end poses
 // are clear. Whole rigid pose interpolation, not the tip alone, is tested.
 path=paths(.2,.2);path[1].point[0].q=m::from_rotation_vector({0,0,-1.5707963267948966});path[1].point[1].q=m::from_rotation_vector({0,0,1.5707963267948966});
 assert(kcd2vr::self_contact54::nearest(c::blade(ps[0],path[0].point[0]),c::box(ps[1],path[1].point[0])).gap>.002);
 assert(kcd2vr::self_contact54::nearest(c::blade(ps[0],path[0].point[1]),c::box(ps[1],path[1].point[1])).gap>.002);
 r=c::sweep(ps,path,0,1,1.);assert(r.usable&&r.limited&&r.fraction<1);
 // Tangential translation / withdrawal from contact are certified by a common
 // separating plane. Initial overlap permits only non-deepening backout.
 path=paths(.018,.118);r=c::sweep(ps,path,0,1,1.);assert(r.usable&&!r.limited&&r.escaping);
 path=paths(.018,.018);path[0].point[1].p.z=.2;r=c::sweep(ps,path,0,1,1.);assert(r.usable&&!r.limited&&r.escaping);
 path=paths(.005,.105);r=c::sweep(ps,path,0,1,1.);assert(r.usable&&!r.limited&&r.escaping);
 path=paths(.005,-.005);r=c::sweep(ps,path,0,1,1.);assert(r.usable&&r.limited&&r.fraction==0);
 path=paths(.005,.005);path[1].point[1].p.x=.01;r=c::sweep(ps,path,0,1,1.);assert(r.usable&&r.limited&&r.fraction==0);
 path=paths(.005,.005);path[1].point[1].p.x=-.1;r=c::sweep(ps,path,0,1,1.);assert(r.usable&&!r.limited&&r.escaping);
 path=paths(.005,.005);path[1].point[1].q=m::from_rotation_vector({0,0,.1});r=c::sweep(ps,path,0,1,1.);assert(r.usable&&r.limited&&r.fraction==0);
 // Common translation does not produce relative contact or freeze a touch.
 path=paths(.018,.218);path[1].point[1].p.x=.2;r=c::sweep(ps,path,0,1,1.);assert(r.usable&&!r.limited);
 // Piecewise checked tangent stays piecewise. No straight chord from the first
 // to last endpoint may cut through a world corner avoided by the kernel.
 path=paths(.2,.2);path[0].point[0].p.z=.6;path[0].append(pose(-.2));r=c::sweep(ps,path,0,1,1.);assertSafe(ps,path,r);assert(r.fraction>.5);
 assert(std::abs(c::at(ps[0],path[0],r.fraction).p.z)<1e-10);
 path[0].valid=false;assert(!c::sweep(ps,path,0,1,1.).usable);path[0].valid=true;
 ps[0].classId=3;assert(!c::sweep(ps,path,0,1,1.).usable);ps=profiles();
 // Run the actual physical kernel: clear native-world leaves record the real
 // accepted acquisition paths. The pair helper issues ZERO extra native rays.
 for(int mode=0;mode<3;++mode){p::State state{};p::Input in{};in.allowed=in.contact=true;in.sample=1;in.tick=1000;in.timeNs=1000000000;
  in.epoch=3;in.bodyEpoch=4;in.dynamics=mode!=0;in.sweepGate=true;in.requirePrimaryGrip114=true;in.positionGuard176=mode!=0;in.attacks=mode!=0;in.physicalCombat177=mode!=0;
  for(int s=0;s<2;++s){in.tracked[s]=in.intentTracked[s]=true;in.grip[s]=1;in.reach[s]=1.8;in.shoulders[s]={0,0,0};}
  in.hands[0]=pose(-.2);in.hands[1]=pose(0);ps[0].nativeWeapon=pose(.2);ps[1].nativeWeapon=pose(0);ps[0].nativeSeed=ps[1].nativeSeed=true;
  unsigned native=0;auto out=p::step(state,in,ps,[&](const auto&,auto,auto,bool){++native;return p::Contact103{};});
  assert(out.active[0]&&out.active[1]&&out.checkedPath60[0].valid&&out.checkedPath60[1].valid);
  const auto before=native;out.add(p::CommandKind::Prepare,in,ps[0]);out.add(p::CommandKind::Slash,in,ps[0]);state.weapon[0].prepared=true;
  out.impactRecord178r30[0].valid=true;out.impactSpeed[0]=4;
  r=c::apply(in,ps,state,out,0,1);assert(r.usable&&r.limited&&native==before);
  assert(out.weapons[0].p.x>.01799&&out.weapons[0].p.x<.0181&&out.ownShieldBlocked60[0]);
  assert(!p::attack_authority55c1(in,out,0)&&!out.impactRecord178r30[0].valid&&out.impactSpeed[0]==0);
  assert(m::length(out.weapons[0].p-state.weapon[0].weapon.p)<1e-10&&m::length(out.weapons[0].p-state.cached.weapons[0].p)<1e-10);
  bool abort=false;for(unsigned j=0;j<out.commandCount;++j){assert(out.commands[j].kind!=p::CommandKind::Prepare&&out.commands[j].kind!=p::CommandKind::Slash);abort|=out.commands[j].kind==p::CommandKind::Abort;}assert(abort);
  if(mode==0){
   ++in.sample;in.tick+=20;in.timeNs+=20000000;out=p::step(state,in,ps,[&](const auto&,auto,auto,bool){++native;return p::Contact103{};});c::apply(in,ps,state,out,0,1);
   const auto cached=native;++in.sample;in.tick+=10;in.timeNs+=10000000;
   out=p::step(state,in,ps,[&](const auto&,auto,auto,bool){++native;return p::Contact103{};});
   assert(native==cached);r=c::apply(in,ps,state,out,0,1);assert(r.usable&&r.limited&&native==cached);
  }
 }
 std::puts("own_shield_constraint60: continuous moving-pair checked prefixes, whole pose/cache/attack retirement, zero extra native rays PASS");
}
