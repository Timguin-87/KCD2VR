#include "vrik/finger_tuning40.h"
#include <cassert>
#include "vrik/user_modes175.h"
#include <limits>
using namespace kcd2vr;
int main(){
 static_assert(user175::weapons(7,false,4)==user175::Mode::WeaponsHands40);
 static_assert(user175::weapons(7,false,5)==user175::Mode::Calibrated);
 static_assert(user175::configuration(user175::Mode::WeaponsHands40).pose==4);
static_assert(user175::default59(user175::Mode::Calibrated,true)==user175::Mode::CombatSynchronized);
 static_assert(user175::default59(user175::Mode::Off,true)==user175::Mode::Off);
 static_assert(user175::comparison59(true,true)==user175::Mode::CombatContactLab);
 static_assert(user175::comparison59(true,false)==user175::Mode::CombatSynchronized);
 namespace m=pose_core;const m::Quat open{},closed{0.7071067811865475,0,0,0.7071067811865475};
 const auto exact=finger40::tune(open,closed,0,1,0,{0,0,1},true);assert(m::angle(exact.open,open)<1e-6);assert(m::angle(exact.closed,closed)<1e-6);
 const auto softer=finger40::tune(open,closed,0,.7,0,{0,0,1},true);assert(m::angle(softer.closed,open)<m::angle(closed,open));
 const auto splay=finger40::tune(open,closed,0,1,10,{0,0,1},true);assert(m::angle(splay.open,open)<1e-6);assert(m::angle(splay.closed,closed)>.17);
 const auto distal=finger40::tune(open,closed,0,1,10,{0,0,1},false);assert(m::angle(distal.open,open)<1e-6);
 finger40::set(1,4,1,.75);assert(finger40::get(1,4,1)==.75f);assert(finger40::get(0,4,1)==1.f);
 finger40::set(1,4,2,99);assert(finger40::get(1,4,2)==20);assert(finger40::bounded(std::numeric_limits<float>::quiet_NaN(),1)==1);

 // C59 mirrored geometry: correction goes toward the neighbor on either hand,
 // leaves OPEN byte parity and rotates exactly three degrees at fixed length.
 for(double hand:{-1.,1.}){
  const m::Vec3 phalanx{0,1,0},neighbor{hand*.02,0,0};
  const auto fix=finger40::towardRing59(open,open,open,open,phalanx,{0,0,0},neighbor,3);
  assert(m::angle(fix.open,open)<1e-9);
  const auto tip=m::rotate(fix.closed,phalanx);assert(tip.x*hand>0);
  assert(std::abs(m::length(tip)-1)<1e-9);assert(std::abs(m::angle(fix.closed,open)-3*3.141592653589793/180)<1e-7);
 }
 const auto noAxis=finger40::towardRing59(open,closed,open,open,{0,1,0},{},{0,1,0},3);
 assert(m::angle(noAxis.closed,closed)<1e-9);
 assert(finger40::defaults59[0][0][1]==.58f&&finger40::defaults59[1][0][1]==.79f);
}
