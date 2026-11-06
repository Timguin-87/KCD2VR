// Execute the production Mod Use adapter with the retained XR/selection/transport seams.
#include "../src/subsystems/doors/hand_only_h40.h"
#define main inherited_door_producer_d40
#include "door_producer46_test.cpp"
#undef main
static unsigned failuresD40=0,checksD40=0;
static void expectD40(bool value,const char* why){++checksD40;if(!value){++failuresD40;std::printf("FAIL %s\n",why);}}
int main(){
 action_time_owner59c11::grasp=true;
 door_use46::SetEnabled(false);
#ifdef KCD2VR_PICKUP_NEAR_D40
 pickup_use19c3::setNearOnlyD40(true);
#endif
 auto sample=[](Producer& p,std::uint64_t t,float grip,float d=.08f){candidate(t,0,"PickableItem",d);p.tick(t,grip);};
 {Producer p;p.f5Mode=0;p.comparison114=false;sample(p,1000,0);sample(p,1020,1);
  expectD40(p.touch_bindings_.requests==0,"first current close sample prepares; never fabricates native use");
  sample(p,1040,1);expectD40(p.touch_bindings_.requests==1,"close pickup transports on second fresh selection sample (20ms after grip)");
  for(unsigned t=1060;t<=1220;t+=20)sample(p,t,1);
  expectD40(p.touch_bindings_.requests==1&&!p.touch_bindings_.held,"one held squeeze transports once");}
 {Producer p;sample(p,2000,0);sample(p,2020,1);p.tick(2040,1);p.tick(2060,1);
  expectD40(p.touch_bindings_.requests==0,"duplicate candidate timestamp cannot qualify close pickup");
  sample(p,2080,1);expectD40(p.touch_bindings_.requests==1,"later monotonic selection sample qualifies original target");}
 {Producer p;sample(p,3000,0);sample(p,3020,1);sample(p,3040,0);
  expectD40(p.touch_bindings_.requests==0,"release before second selected sample never transports delayed pickup");}
 for(float distance:{.11f,.13f,.15f,.20f}){
  Producer p;sample(p,4000,0,distance);sample(p,4020,1,distance);
  expectD40(p.touch_bindings_.requests==0,"D49 expanded native reach still requires two selected samples");
  sample(p,4040,1,distance);
  expectD40(p.touch_bindings_.requests==1,"D49 admits recorded inside-mesh food selection points up to20cm");}
 {Producer p;sample(p,4500,0,.20001f);for(unsigned t=4520;t<=4800;t+=20)sample(p,t,1,.20001f);
  expectD40(p.touch_bindings_.requests==0,"Direct mode refuses beyond20cm without extending assisted reach");}
 {Producer p;sample(p,5000,0);sample(p,5020,1);candidate(5040,0,"PickableItem",.08f);interaction_alignment::candidate.entity=43;p.tick(5040,1);
  expectD40(p.touch_bindings_.requests==0,"replacement target cannot borrow prior qualification");
  candidate(5060,0,"PickableItem",.08f);interaction_alignment::candidate.entity=43;p.tick(5060,1);
  expectD40(p.touch_bindings_.requests==1&&p.pickupUse19c3.target63==43,"replacement requires its own two fresh samples");}
 {Producer p;sample(p,6000,0);sample(p,6020,1);p.rayPublished116=false;sample(p,6040,1);p.rayPublished116=true;sample(p,6060,1);sample(p,6080,1);
  expectD40(p.touch_bindings_.requests==0,"lost context irreversibly aborts this squeeze");}
#ifdef KCD2VR_PICKUP_NEAR_D40
 {Producer p;sample(p,6500,0);sample(p,6520,1);candidate(6510,0,"PickableItem",.08f);p.tick(6540,1);sample(p,6560,1);sample(p,6580,1);
  expectD40(p.touch_bindings_.requests==0,"reversed candidate source retires the squeeze instead of backdating qualification");}
 {Producer p;sample(p,6600,0);sample(p,6620,1);pickup_use19c3::setNearOnlyD40(false);sample(p,6640,1);sample(p,6660,1);
  expectD40(p.touch_bindings_.requests==0,"reach mode change requires a new squeeze");pickup_use19c3::setNearOnlyD40(true);}
 {pickup_use19c3::setNearOnlyD40(false);Producer p;sample(p,7000,0,.24f);sample(p,7020,1,.24f);
  for(unsigned t=7040;t<7180;t+=20)sample(p,t,1,.24f);
  expectD40(p.touch_bindings_.requests==0,"Assisted retains deliberate distant hold delay");
  sample(p,7180,1,.24f);expectD40(p.touch_bindings_.requests==1,"Assisted retains legacy25cm route");pickup_use19c3::setNearOnlyD40(true);}
#endif
 std::printf("D40 production pickup adapter checks=%u failures=%u; native game and headset NOT executed\n",checksD40,failuresD40);
 return failuresD40?1:0;
}
