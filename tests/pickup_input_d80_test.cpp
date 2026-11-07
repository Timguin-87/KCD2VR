#include <cmath>
#include <limits>
#include <cassert>
#include <initializer_list>
#include "../src/vrik/pickup_input_d80.h"
struct Frame {float mapGripM39[2]{1,1},pickupTriggerD80[2]{};};
int main(){
 for(int side=0;side<2;++side){
  Frame f;f.pickupTriggerD80[1-side]=1;
  assert(!pickup_input_d80::refusal(f,side,true));
  f.pickupTriggerD80[side]=.60f;assert(!pickup_input_d80::refusal(f,side,true));
  f.pickupTriggerD80[side]=.61f;assert(pickup_input_d80::refusal(f,side,true));
  assert(!pickup_input_d80::refusal(f,side,false));
  for(float bad:{-1.f,2.f,std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity()}){
   f.pickupTriggerD80[side]=bad;assert(pickup_input_d80::refusal(f,side,true));
  }
 }
 Frame f;assert(pickup_input_d80::refusal(f,-1,true));assert(pickup_input_d80::refusal(f,2,true));
}
