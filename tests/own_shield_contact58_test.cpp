#include "vrik/own_shield_contact58.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;namespace c=own_shield_contact58;namespace m=pose_core;
static self_contact54::ShieldSample sample(unsigned n,double gap){self_contact54::ShieldSample x{};
 x.owner={1,2,3,4,5,6,7,1,0};x.shieldKey=8;x.shieldGeneration=9;x.shieldEntity=10;
 x.blade={{gap,.08,0},{gap,1,0},.006,true};x.box={{-.01,-.3,-.4},{.01,.3,.4},{},{},true};
 x.tick=1000+n*20;x.sample=n+1;x.timeNs=x.tick*1000000;x.eligible=true;return x;}
static auto step(c::State& s,self_contact54::ShieldSample x,bool current=true,std::uint64_t source=11,std::uint64_t now=0){
 return c::step(s,x,{now?now:x.tick,source,11,current});}
int main(){
 c::State s{};assert(!step(s,sample(0,.06)).tick);auto hit=step(s,sample(1,.016));
 assert(hit.tick&&hit.speed>1.2&&hit.approach>.35&&hit.weapon==0&&hit.shield==1);
 assert(!step(s,sample(1,.016)).tick);assert(!step(s,sample(2,.016)).tick);
 assert(!step(s,sample(3,.06)).tick);assert(!step(s,sample(4,.016)).tick); // 250 ms cooldown
 for(unsigned n=5;n<15;++n)step(s,sample(n,.06));assert(step(s,sample(15,.016)).tick);
 s={};assert(!step(s,sample(0,.016)).tick);assert(!step(s,sample(1,.016)).tick); // initial/held overlap
 s={};step(s,sample(0,.020));assert(!step(s,sample(1,.016)).tick); // slow inward contact
 // World locomotion is absent from these shared model-frame points. Common
 // model translation/rotation keeps the gap, so cannot become a clang either.
 s={};auto a=sample(0,.06);step(s,a);auto b=sample(1,.06);b.blade.a.x+=.2;b.blade.b.x+=.2;b.box.position.x+=.2;
 assert(!step(s,b).tick);b=sample(2,.06);const m::Quat q{0,0,std::sin(.5),std::cos(.5)};
 b.blade.a=m::rotate(q,b.blade.a);b.blade.b=m::rotate(q,b.blade.b);b.box.rotation=q;
 assert(!step(s,b).tick);
 // A high-speed tangential brush with tiny inward motion is not a bang.
 s={};step(s,sample(0,.020));b=sample(1,.016);b.blade.a.z+=.2;b.blade.b.z+=.2;assert(!step(s,b).tick);
 for(unsigned cause=0;cause<10;++cause){s={};step(s,sample(0,.06));b=sample(1,.016);
  switch(cause){case 0:++b.owner.epoch;break;case 1:++b.owner.request;break;case 2:++b.owner.generation;break;
   case 3:++b.shieldGeneration;break;case 4:++b.owner.player;break;case 5:++b.owner.character;break;
   case 6:b.scale=2;break;case 7:b.tick+=100;b.timeNs=b.tick*1000000;break;
   case 8:b.eligible=false;break;case 9:b.box.valid=false;break;}
  assert(!step(s,b).tick);
 }
 s={};step(s,sample(0,.06));assert(!step(s,sample(1,.016),false).tick&&!s.have); // menu/current owner revoked
 s={};step(s,sample(0,.06));assert(!step(s,sample(1,.016),true,12).tick&&!s.have); // original source changed
 s={};step(s,sample(0,.06));assert(!step(s,sample(1,.016),true,11,1101).tick&&!s.have); // stale frame
 std::puts("own_shield_contact58: committed native-bounds proxy, relative inward contact, locomotion/rotation/stationary/identity/source/scale/freshness/cooldown PASS");
}
