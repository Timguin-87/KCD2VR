#include "subsystems/base/linked_fov_h26.h"
#include <cassert>
#include <limits>
#include <cstdio>
using namespace kcd2vr::linked_fov_h26;
int main(){
 assert(Value(95)==95&&Value(40)==95&&Value(180)==95);
 assert(Value(std::numeric_limits<float>::quiet_NaN())==95);
 Startup s;assert(!s.Poll(100,0,false));assert(s.Poll(1000,1,false));
 assert(!s.Poll(1499,2,false)&&s.Poll(1500,2,false));
 assert(!s.Poll(11000,2,false)); // no ongoing native-camera writer
 assert(s.Poll(120000,2,true)); // load longer than10s: first gameplay rearms ONCE
 assert(!s.Poll(120001,2,true)&&s.Poll(120500,3,true));
 assert(!s.Poll(130000,3,true));assert(!s.Poll(200000,3,false));assert(!s.Poll(210000,3,true));
 // Simulate a profile overwrite during startup. Native setter owns writes,
 // selected value remains95; later camera values never overwrite the setting.
 Startup late;float native=95,selected=95;int writes=0,reads=0;
 for(unsigned now=0;now<=15000;++now){if(now==2000)native=110;
  if(late.Poll(now,1,false)){++reads;if(std::fabs(native-selected)>=.25f){native=selected;++writes;}}}
 assert(selected==95&&native==95&&writes==1&&reads==20);
 std::puts("H26 shared FOV selection, late-profile correction and bounded startup/gameplay lease PASS");
}
