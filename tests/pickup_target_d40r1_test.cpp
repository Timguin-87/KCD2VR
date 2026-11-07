// Actual production owner and Grip producer through the retained native fixture.
// This entry isolates target/lifetime cases from unrelated historical scenarios.
#include <windows.h>
#if defined(_MSC_VER)
// The legacy OS shim maps these to C++ try/catch, which cannot accompany newer
// production __finally wrappers. MSVC supports the actual native SEH keywords.
#undef __try
#undef __except
#endif
#define main inherited_native_grasp_owner_d40r1
#include "native_grasp_owner13_test.cpp"
#undef main
namespace kcd2vr::action_time_owner59c11 { namespace {
static int TargetScenarioD40R1(){
 const auto beginUse55=[](){
  reset();ResetOwner();g_consumed_use13=0;
  put(action.data(),0,WhBase()+0x3B13F38);
  put(action.data(),kActContext,static_cast<void*>(actionContext.data()));
  put(action.data(),kActFragId,int(0));frame(980);UpdateDetour(controller.data(),.02f);
  frame(1000);NoteUseSent(0,1000);
 };
 const auto installPicking55=[](){
  put(action.data(),0,WhBase()+0x3A450B0);put(action.data(),kActFragId,int(1));
  put(action.data(),kActToken,unsigned(56));
 };
 const auto candidate=[](std::uint64_t tick,const char* cls,float distance){
  auto& c=interaction_alignment::testCandidate;c={};c.tick=tick;c.entity=72;c.owner=0;c.object=true;c.distance=distance;
  c.sourceTick=tick;c.sourceGeneration=1;c.sourceEpoch=7;c.sourcePlayer=0x1234;std::snprintf(c.cls,sizeof(c.cls),"%s",cls);
 };
 const bool savedReachD40=pickup_use19c3::nearOnlyD40.load();
#include "pickup_target_d40r1_cases.inl"
 return 0;
}
}}
int main(){return kcd2vr::action_time_owner59c11::TargetScenarioD40R1();}
