#include "../src/vrik/physical_weapon.h"
#include <cassert>
#include <cstdio>
int main(){namespace p=kcd2vr::physical;p::Attributes a{};
 assert(p::exhaustionScale59c7b(a)==1.0);                  // not live: unchanged
 a.live=true;a.stamina=1.0;assert(p::exhaustionScale59c7b(a)==1.0);
 a.stamina=0.15;assert(p::exhaustionScale59c7b(a)==1.0);
 a.stamina=0.0;assert(p::exhaustionScale59c7b(a)==5.0);
 a.stamina=0.02;assert(p::exhaustionScale59c7b(a)==5.0);
 a.stamina=0.085;const double mid=p::exhaustionScale59c7b(a);assert(mid>2.9&&mid<3.1);
 // Regression: C7C's 0.2 authority cannot support the weight of a horizontal polearm.
 namespace d=kcd2vr::dynamics177;namespace m=kcd2vr::pose_core;
 for(int cls:{4,7})for(bool support:{false,true}) {
  const auto body=d::body(cls,cls==4?3.4:9.6,cls==4?1.1:2.0);
  auto auth=d::fatiguedAuthority59c10r2(d::authority(10,10,0,true,support),.6,5);
  m::Vec3 pivot{},vel{},omega{};m::Quat q{};
  for(int n=0;n<2700;++n)d::advance(body,auth,{0,1,0},{0,0,-9.81},{},{},{},{},1./900,pivot,q,vel,omega);
  std::printf("class=%d support=%d stationary drift=%.4fm angle=%.3fdeg\n",cls,int(support),m::length(pivot),m::angle(q,{})*57.2958);
  assert(m::finite(pivot)&&m::length(pivot)<.02&&m::angle(q,{})<.035);
 }
 const auto one=d::authority(10,10,1,true,false),two=d::authority(10,10,1,true,true);
 assert(two.force>one.force&&two.torque>one.torque);
 std::puts("exhaustion59c7b: authority divisor 1 rested / 5 exhausted; not a fivefold displayed speed claim");}
