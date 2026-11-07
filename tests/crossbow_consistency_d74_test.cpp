// Actual client/native adapter plus extracted production spatial policy.
// Native side effects, attachment reads and rail/rest geometry are fixture leaves.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "crossbow_native_xr34_test.cpp"
#include "../src/subsystems/ranged_reload/options_d65c.h"
namespace xcXR34=kcd2vr::crossbow_native_xr34;
static void spatialPolicy(Fixture& a){
 const auto& source=a.source;const struct{x::Source source;}o{source};auto& result=a.spatial;
 // Synthetic exact model-space rail/rest inputs, upstream of actual producer.
 result.axis={0,-1,0};result.stringRest={0,0,0};result.travel=source.classId==15?.1743:.1315;
 result.crankQualified=false;
#include "generated_crossbow_spatial_d74.inc"
}
int main(){
 for(bool instant:{false,true}){Fixture a;a.source.classId=15;a.source.instantD72=instant;
  kcd2vr::ranged_options_d66c::crankTravel=.08f;spatialPolicy(a);
  assert(a.spatial.crankQualified&&a.spatial.axis.y==1&&std::abs(a.spatial.travel-.08)<1e-6);
 }
 for(int cls:{14,15}){Fixture a;a.source.instantD72=true;a.source.classId=cls;
  kcd2vr::ranged_options_d66c::crankTravel=.08f;spatialPolicy(a);
  assert(a.spatial.axis.y==(cls==15?1:-1));
  a.source.entryGripD72=2;a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;
  a.source.chestKnown=true;a.source.handInChest[0]=true;setup(a);
  for(int i=0;i<5&&!g_crossbow_xr34.status().toolTakenD72;++i){a.input(.9);nativeFrame(a);}
  assert(a.spawns==1&&a.cocks==0&&g_crossbow_xr34.status().toolTakenD72);
  a.source.handInChest[0]=false;a.input(.9);nativeFrame(a);assert(g_crossbow_xr34.status().toolEngagedD72);
  if(cls==15){
   a.input(.9,{0,-.1,0});nativeFrame(a);assert(a.cocks==0&&g_crossbow_xr34.status().progress==0);
   a.input(.9,{0,.04,0});nativeFrame(a);assert(a.cocks==0&&std::abs(g_crossbow_xr34.status().progress-.5)<.0001);
   kcd2vr::ranged_options_d66c::crankTravel=.04f;spatialPolicy(a);
   a.input(.9,{0,.04,0});nativeFrame(a);assert(a.cocks==0&&std::abs(g_crossbow_xr34.status().progress-.5)<.0001);
   a.input(.9,{0,.081,0});nativeFrame(a);
  }else{
   a.input(.9,{0,.15,0});nativeFrame(a);assert(a.cocks==0&&g_crossbow_xr34.status().progress==0);
   a.input(.9,{0,-.15,0});nativeFrame(a);
  }
  assert(a.spawns==1&&a.cocks==1&&a.loads==0&&g_crossbow_xr34.status().phase==x::Phase::Tail);
  a.input(.9,cls==15?x::Vec{0,.081,0}:x::Vec{0,-.15,0});nativeFrame(a);assert(a.cocks==1);
  XR34BeforeStopNative(a.parent.data());assert(!g_crossbow_xr34.pending());
 }
 std::cout<<"D74 class15 Instant/normal producer agreement, push-vs-pull and frozen travel PASS; ergonomics NOT_PROVEN\n";
}
