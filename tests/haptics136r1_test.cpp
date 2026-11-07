#include "../src/core/haptics.h"
#include <cstdio>
using namespace kcd2vr::haptics101;
static int n=0,f=0;static void ck(bool b,const char*s){++n;if(!b)++f;std::printf("%s %s\n",b?"PASS":"FAIL",s);}
int main(){
  allowed.store(false);select(true,1000);service_test(1000);Pulse p{};ck(!queue[0].read(p)||!p.sequence,"self-test waits for tracking/allowed");
  allowed.store(true);service_test(1010);Pulse r{},l{};ck(queue[0].read(r)&&r.sequence&&queue[1].read(l)&&l.sequence,"self-test publishes both hands once allowed");
  ck(eligible(r,1010)&&eligible(l,1010),"self-test pulses satisfy XR consumer contract");
  Cue c{};zoneRadius.store(.24f);observe(c,0,77,.20,true,2000,2000);observe(c,0,77,.20,true,2001,2040);Pulse z{};
  ck(queue[0].read(z)&&z.key==77&&z.amplitude>=.5f,"zone dwell publishes clear proximity cue");
  ck(publish_event(0,88,.44f,36,2100),"accepted/native event publishes explicit pulse");Pulse e{};ck(queue[0].read(e)&&e.key==88&&e.sequence>(1ull<<63)&&eligible(e,2100),"event pulse has unique XR-consumable sequence");
  std::printf("haptics136r2=%d/%d\n",n-f,n);return f?1:0;
}
