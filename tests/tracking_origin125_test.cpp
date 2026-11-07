#include "vrik/tracking_origin125.h"
#include <cstdio>
#include <cstdlib>
namespace m=kcd2vr::pose_core;
namespace t=kcd2vr::tracking125;
static unsigned checks=0;
static void ck(bool value,const char* why){++checks;if(!value){std::fprintf(stderr,"FAIL %s\n",why);std::exit(1);}}
int main(){
 t::Origin o{};
 for(int n=0;n<100;++n){
  const double yaw=.071*n;
  const auto heading=m::from_rotation_vector({0,yaw,0});
  const auto pitched=m::multiply(heading,m::from_rotation_vector({-.5,0,.15}));
  const m::Vec3 position{n*.04,1.7-n*.002,-n*.08};
  o=t::recenter(position,pitched,o.yaw);
  ck(m::length(t::displacement(o,position))<1e-9,"repeated moved recenter has zero accumulated displacement");
  ck(o.yaw.x==0&&o.yaw.z==0,"recenter never bakes pitch or roll into horizontal origin");
  const auto neutral=t::model_vector(m::rotate(o.yaw,{.2,0,-.3}),o.yaw);
  ck(m::length(neutral-m::Vec3{.2,.3,0})<1e-9,"translation has the same meaning at every heading");
 }
 const auto prior=m::from_rotation_vector({0,.8,0});
 const auto vertical=m::multiply(prior,m::from_rotation_vector({1.57079632679,0,0}));
 ck(m::angle(t::yaw_only(vertical,prior),prior)<1e-8,"vertical recenter retains horizontal heading instead of flipping");
 std::printf("PASS %u tracking-origin checks\n",checks);
}
