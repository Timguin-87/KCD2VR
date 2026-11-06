// Actual production ContactHand and final caller extraction; native geometry is an explicit synthetic leaf.
#define main retained_world_main_d45
#include "contact_release_world_d45.inc"
#undef main
int main(){
 Init(); std::uint64_t previous=0; HandContact6695 c{}; V3 point{},offsets[5]{};
 mockMode=7;thinCenterH24={.25f,0,0};
 auto run=[&](unsigned tick,unsigned sample,V3 requested){const int before=mockCalls;bool ok=false;
  {ContactPass6695 pass(true,tick,previous,sample,7);ok=ContactHand6695(c,reinterpret_cast<void*>(character),1,{0,.0049f,0},requested,point,offsets);}
  Check(ok&&mockCalls-before<=5,"actual release uses existing mandatory feature budget only");return point;};
 const auto stopped=run(1000,1,{.325f,.0049f,0});
 Check(c.blocked&&std::abs(stopped.x-.242f)<.00001f,"finite shelf fixture creates measured 83 mm contact correction");
 const auto clear=run(1011,2,{.325f,.006f,0});
 const float firstStep=Len(Sub(clear,stopped));
 std::printf("D45_FINITE_EDGE rawStepMm=1.100000 oldCorrectionMm=83.000000 firstClearStepMm=%.6f planes=%d blocked=%d\n",firstStep*1000,c.planeCount,c.blocked);
 Check(!c.blocked&&c.planeCount==0&&firstStep<.020f&&clear.x>.25f,"first clear shelf edge releases correction without 83 mm snap or retained surface");
 const auto duplicate=run(1011,2,{.325f,.006f,0});
 Check(Len(Sub(clear,duplicate))<1e-7f,"duplicate source sample does not advance contact spring");
 // The released candidate is checked against newly encountered geometry.
 mockMode=4;wallPoint={.27f,0,0};wallNormal={-1,0,0};
 const auto nextWall=run(1022,3,{.325f,.006f,0});
 Check(c.blocked&&nextWall.x<=.26201f,"new geometry constrains release candidate before it is presented");
 const auto retreat=run(1033,4,{.10f,.006f,0});
 Check(!c.blocked&&std::abs(retreat.x-.10f)<1e-6f,"fast withdrawal beyond correction reaches controller immediately");
 mockMode=0;const auto free=run(1044,5,{.65f,.20f,.10f});
 Check(Len(Sub(free,{.65f,.20f,.10f}))<1e-6f,"ordinary fast free hand is not filtered or velocity capped");
 Init();previous=0;c={};mockMode=7;thinCenterH24={.25f,0,0};
 run(1000,1,{.325f,.0049f,0});run(1011,2,{.325f,.006f,0});
 float lastX=point.x;
 for(unsigned sample=3;sample<=10;++sample){const auto v=run(1000+(sample-1)*11,sample,{.325f,.006f,0});
  Check(v.x>=lastX-1e-6f&&v.x-lastX<=.02001f,"clear release progresses monotonically without hand overshoot");lastX=v.x;}
 Check(Len(Sub(point,{.325f,.006f,0}))<1e-6f&&!c.releaseActiveD45,"bounded shelf release finishes at exact current controller");
 Init();previous=0;c={};mockMode=7;thinCenterH24={.25f,0,0};
 run(1000,1,{.325f,.0049f,0});
 mockMode=0;const auto afterGap=run(1090,2,{.325f,.006f,0});
 Check(Len(Sub(afterGap,{.325f,.006f,0}))<1e-6f&&!c.releaseActiveD45,
       "absent-query interval beyond80ms cannot revive stale contact correction on reacquisition");
 Init();previous=0;c={};mockMode=7;thinCenterH24={.25f,0,0};
 run(1000,1,{.325f,.0049f,0});mockMode=0;
 {ContactPass6695 pass(true,1011,previous,2);bool ok=ContactHand6695(c,reinterpret_cast<void*>(character),2,{0,0,0},{.325f,.006f,0},point,offsets);
  Check(ok&&Len(Sub(point,{.325f,.006f,0}))<1e-6f&&!c.releaseActiveD45,"epoch change retires release correction before current queries");}
 Init();previous=0;c={};mockMode=7;thinCenterH24={.25f,0,0};
 run(1000,1,{.325f,.0049f,0});
 {ContactPass6695 pass(true,1090,previous,2);g_contact_budget6695.remaining=0;
  const bool ok=ContactHand6695(c,reinterpret_cast<void*>(character),1,{0,0,0},{.325f,.006f,0},point,offsets);
  Check(!ok&&!c.glideTickR8b&&!c.releaseActiveD45&&!c.planeCount&&Len(Sub(point,{.325f,.006f,0}))<.000001f,"expired unknown query releases current wrist and retires contact correction");}
 std::printf("corner_release_d45=%d/%d\n",assertions-failures,assertions);return failures?1:0;
}
