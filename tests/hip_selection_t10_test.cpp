#include "../src/subsystems/throw_drop/hip_selection_t10.h"
#include <cassert>
#include <limits>
using namespace kcd2vr::hip_t10;
int main(){
 Input i{};i.key=55;i.generation=7;i.body=9;i.request=10;i.sample=1;i.tick=1000;
 i.allowed=i.equipment=i.stock=true;i.distance[0]=0;i.distance[1]=i.distance[2]=.13;
 State s{};assert(update(s,i).owner==Owner::Equipment);
 ++i.sample;i.tick+=10;i.grip=1;assert(update(s,i).owner==Owner::Equipment);
 i.distance[0]=.13;i.distance[1]=0;assert(update(s,i).owner==Owner::Equipment);
 ++i.sample;i.tick+=10;i.grip=0;assert(update(s,i).owner==Owner::Dagger);
 ++i.sample;i.tick+=10;i.grip=1;assert(update(s,i).owner==Owner::Dagger);
 i.distance[0]=0;i.distance[1]=.13;assert(update(s,i).owner==Owner::Dagger);
 for(int cause=0;cause<9;++cause){auto changed=i;auto state=s;
  if(cause==0)changed.allowed=false;
  if(cause==1)++changed.body;
  if(cause==2)++changed.request;
  if(cause==3)++changed.key;
  if(cause==4)++changed.generation;
  if(cause==5)changed.tick+=101;
  if(cause==6)changed.stock=false;
  if(cause==7)changed.grip=std::numeric_limits<double>::quiet_NaN();
  if(cause==8)changed.sample=0;
  assert(update(state,changed).owner==Owner::None);
 }
 s={};i.grip=1;assert(update(s,i).owner==Owner::None);
 i.grip=0;i.distance[0]=i.distance[1]=.065;i.distance[2]=.195;
 assert(update(s,i).owner==Owner::None);i.grip=1;assert(update(s,i).owner==Owner::None);
 s={};i.grip=0;i.distance[0]=.13;i.distance[1]=.26;i.distance[2]=0;
 assert(update(s,i).owner==Owner::Axe);
 i.grip=1;i.distance[0]=0;i.distance[2]=.13;assert(update(s,i).owner==Owner::None);
}
