#include "../src/vrik/self_contact54.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;using namespace self_contact54;
static hand_shape59c7f::Shape hand(){hand_shape59c7f::Shape s{};s.valid=true;s.count=1;s.part[0]={{0,0,0},{0,.10,0},.006};return s;}
static Sample frame(unsigned n,double x,double bladeX=0){Sample s{};s.owner={1,2,3,4,5,6,7,1,0};
 s.shape=hand();s.blade={{bladeX,.08,0},{bladeX,1,0},.006,true};s.wrist={x,.4,0};s.tick=1000+n*20;s.sample=1+n;
 s.timeNs=1000000000ull+n*20000000ull;s.eligible=true;return s;}
int main(){
 const auto h=hand();const Blade b{{0,.08,0},{0,1,0},.006,true};
 const auto r=separate(h,{.010,.4,0},b,{1,0,0});assert(r.valid&&r.contact&&r.correction.x>0&&length(r.correction)<=.06);
 assert(nearest(h,Vec3{.010,.4,0}+r.correction,b).gap>=.001);
 const auto none=separate(h,{.1,.4,0},b,{1,0,0});assert(none.valid&&!none.contact&&length(none.correction)==0);
 auto many=h;many.count=26;for(auto& p:many.part)p=h.part[0];assert(separate(many,{.01,.4,0},b,{1,0,0}).valid);
 auto invalid=many;invalid.count=27;assert(!separate(invalid,{.01,.4,0},b,{1,0,0}).valid);
 // First/held contact never means a pat, and duplicates do not advance history.
 State st{};assert(!committed(st,frame(0,.014)).tick);assert(!committed(st,frame(1,.014)).tick);
 assert(!committed(st,frame(1,.001)).tick&&st.previous.wrist.x==.014);
 st={};assert(!committed(st,frame(0,.06)).tick);auto hit=committed(st,frame(1,.014));
 assert(hit.tick&&hit.key==5&&hit.player==2&&hit.hand==1&&hit.relativeSpeed>.25&&hit.approachSpeed>.15);
 assert(!committed(st,frame(2,.014)).tick); // held contact is quiet
 assert(!committed(st,frame(3,.06)).tick);assert(!committed(st,frame(4,.014)).tick); // cooldown
 // Moving blade into a stationary palm is equally valid, with material-point velocity.
 st={};assert(!committed(st,frame(0,.06)).tick);hit=committed(st,frame(1,.06,.046));assert(hit.tick);
 // Slow contact, co-translation, stale owner and changed equipment do not emit.
 st={};auto a=frame(0,.017);committed(st,a);assert(!committed(st,frame(1,.014)).tick);
 st={};a=frame(0,.06);committed(st,a);auto c=frame(1,.08,.02);assert(!committed(st,c).tick);
 st={};committed(st,frame(0,.06));c=frame(1,.014);c.tick+=100;c.timeNs+=100000000;assert(!committed(st,c).tick);
 st={};committed(st,frame(0,.06));c=frame(1,.014);++c.owner.generation;assert(!committed(st,c).tick);
 st={};committed(st,frame(0,.06));c=frame(1,.06);c.shape.part[0].a.x=c.shape.part[0].b.x=-.046;
 assert(!committed(st,c).tick); // curling fingers across a stationary blade is not a deliberate pat
 st={};committed(st,frame(0,.06));c=frame(1,.014);c.eligible=false;assert(!committed(st,c).tick&&!st.have);
 // Sword-to-held-shield gesture uses exact native-local AABB bounds, never a fake blade/box thickness.
 Box box{{-.01,-.3,-.4},{.01,.3,.4},{},{},true};
 Blade sword{{.016,.08,0},{.016,1,0},.006,true};
 auto bx=nearest(sword,box);assert(bx.valid&&std::abs(bx.gap)<1e-8&&bx.normal.x>.99);
 Box rotated=box;rotated.rotation={0,0,std::sqrt(.5),std::sqrt(.5)};
 Blade across{{-.4,.016,0},{.4,.016,0},.006,true};bx=nearest(across,rotated);
 assert(bx.valid&&std::abs(bx.gap)<1e-8&&bx.normal.y>.99);
 auto shieldFrame=[&](unsigned n,double x){ShieldSample f{};f.owner={1,2,3,4,5,6,7,1,0};f.blade=sword;f.blade.a.x=f.blade.b.x=x;
  f.box=box;f.shieldKey=8;f.shieldGeneration=9;f.shieldEntity=10;f.tick=1000+n*20;f.sample=1+n;
  f.timeNs=1000000000ull+n*20000000ull;f.eligible=true;return f;};
 ShieldState shield{};assert(!committed(shield,shieldFrame(0,.06)).tick);
 auto pat=committed(shield,shieldFrame(1,.016));assert(pat.tick&&pat.source==Source::VisualShieldBounds&&pat.shieldKey==8);
 assert(!committed(shield,shieldFrame(2,.016)).tick); // no repeated held-contact gesture
 shield={};assert(!committed(shield,shieldFrame(0,0)).tick);assert(!committed(shield,shieldFrame(1,0)).tick); // initial overlap is quiet
 shield={};committed(shield,shieldFrame(0,.06));auto different=shieldFrame(1,.016);++different.shieldGeneration;
 assert(!committed(shield,different).tick);
 box.hi.x=.5;assert(!valid(box)); // unproven fat bounds are not presented as a shield face
 std::puts("PASS C54 own-blade analytic separation and fresh relative-moving contact edges, both moving hands and moving swords, no proximity/hold awards");
}
