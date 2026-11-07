#include "../src/vrik/physical_weapon.h"
#include "../src/vrik/equipment_gesture114.h"
#include <cstdio>
using namespace kcd2vr; using namespace kcd2vr::physical; using namespace kcd2vr::equipment114; namespace m=kcd2vr::pose_core;
static unsigned checks=0,failures=0; static void check(bool b,const char*n){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
int main(){
 equipment114::State s{}; equipment114::Input i{};i.key=1;i.generation=1;i.epoch=1;i.tick=1000;i.eligible=true;i.drawn=true;i.mountKnown=true;i.mount={0,0,1};i.hand={.8,0,1};i.grip=1;
 update(s,i);i.tick+=70;i.grip=0;check(update(s,i)==Request::None,"ordinary Grip release away from holster does not stow");
 i.tick+=10;i.grip=1;i.hand=i.mount;update(s,i);i.tick+=60;update(s,i);i.tick+=10;i.grip=0;check(update(s,i)==Request::Holster,"return to own zone, dwell and release requests one holster");
 i.drawn=false;i.tick+=20;update(s,i); // acknowledge
 // Draw path remains pull gesture.
 i.grip=0;i.tick+=20;update(s,i);i.grip=1;i.hand=i.mount;i.tick+=10;update(s,i);i.hand.x+=.30;i.tick+=90;check(update(s,i)==Request::Draw,"grip and pull from zone still requests draw");
 // Shoulder closest-point helper makes a short vertical capsule.
 physical::Input pi{};pi.shoulders[0]={.2,0,1.3};pi.shoulders[1]={-.2,0,1.3};physical::Profile bow{};bow.classId=9;Vec3 base{},closest{};physical::holster_mount126(pi,bow,base);Vec3 hand=base+m::Vec3{0,0,.12};check(physical::shoulder_holster_closest131(pi,bow,hand,closest)&&m::length(closest-hand)<1e-9,"shoulder zone follows hand along vertical capsule segment");
 hand=base+m::Vec3{0,0,.30};physical::shoulder_holster_closest131(pi,bow,hand,closest);check(std::abs((closest-base).z-.14)<1e-9,"HS bow capsule has bounded vertical extent (+14 cm over its own point, H3 value)");
 std::printf("%u checks, %u failures\n",checks,failures);return failures?1:0;}
