// Production native adapter, source producer and pose preparation under an
// advancing native transition. Memory, scheduler and effects are explicit leaves.
#define main bodySourceLegacyMainH95
#include "crossbow_body_source_r93r3_test.cpp"
#undef main
namespace sourceRouteD87 {
static std::uint64_t currentPublicationR95=2;
static void RangedReloadEquipmentXR34() noexcept {}
static std::uint64_t ReadCompletedPublicationT23() noexcept{return currentPublicationR95;}
static bool RangedSpatialXR34(const x::Owner& o,const x::Source& accepted,x::Spatial& s) noexcept{return spatial82(o,accepted,s);}
#include "publish_source_r95_actual.inc"
}
static unsigned checksH95=0;
#define H95(c) do{++checksH95;if(!(c)){std::cerr<<"FAIL_HANDOVER_R95 line="<<__LINE__<<" " #c " reason="<<x::reasonName(g_crossbow_xr34.status().reason)<<"\n";return 1;}}while(0)
static int advancingTransition95(int kind){
 Fixture a;a.source.classId=kind;a.source.physicalD81=true;a.source.intentKnownD78=true;
 a.source.entryGripD72=3;a.source.quiverKnownD72=true;a.source.handInQuiverD72[0]=true;a.source.grip[0]=.9;
 a.spatial.toolBound=false;RetailScopeD82 profile(a,kind);
 start82(a,false,true);completeFirstPose82(a,admission82(a));XR34PostControllerNative(a.controller.data());
 preparationR93R3::setup(a);auto complete=bodyFor(a.source);
 sourceRouteD87::explicitBodyR93R3=&complete;sourceRouteD87::retained=sourceRouteD87::enabled=true;
 sourceRouteD87::currentPublicationR95=complete.publicationR24;
 QuatT28 original[1]{};x::Passenger passenger{};
 for(int i=0;i<8;++i){
  H95(ReadReloadPassengerXR34(passenger,a.source.character,a.source.body,a.now));
  // This is the former missed order: native time advanced after the last
  // post-controller witness but before the fresh tracked body callback.
  a.put(a.entry()+0x44,passenger.clock.normalized+.01f);
  a.put(a.p(a.child)+0x10,passenger.clock.activeTime+.05f);
  H95(!ReloadNativeClockCurrentD86(passenger,a.source.character)); // mechanism remains exact
  H95(ReloadNativeClockCurrentD86(passenger,a.source.character,true));
  auto next=complete;TrackingSnapshot tracking{};a.now+=20;
  tracking.sequence=complete.stamp.sample+1;tracking.tick=a.now;tracking.source_epoch=a.source.tracking;
  tracking.right_grip=.9;tracking.left_grip=1;next.stamp.sample=tracking.sequence;next.stamp.tick=a.now;
  next.allowed[0].t[0]+=.01f;next.propRequested5916c1[0].t[0]+=.01f;
  preparationR93R3::relativeReads=0;
  H95(preparationR93R3::PrepareReloadPoseXR34(passenger,next,original,1,tracking));
  next.publicationR24=complete.publicationR24+2;next.reloadSourceXR34.bodyPublicationR95=next.publicationR24;
  complete=next;sourceRouteD87::currentPublicationR95=complete.publicationR24;
  sourceRouteD87::RangedPublishXR34();H95(x::ReadSource(a.source)&&!a.source.auxiliaryUnavailableR95);
  XR34PreControllerNative(a.controller.data(),.02f);
  H95(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::NativeTransitionD80);
  H95(a.get<float>(a.p(a.child)+0x5C)==2.5f&&a.cocks==0&&a.loads==0);
  XR34PostControllerNative(a.controller.data());
  const auto publication=x::passengerPublication.load();XR34PublishNative();
  H95(x::passengerPublication.load()==publication); // an unchanged observation is not a revoke
  // An actual metadata/event boundary must revoke even with identical status.
  XR34PublishNative(true);H95(x::passengerPublication.load()==publication+1);
  XR34PostControllerNative(a.controller.data());
 }
 // Real source payload loss cannot restart the transition's pause/resume loop.
 const auto sourceTick=a.source.tick;const x::Source empty{};complete.reloadSourceXR34=empty;sourceRouteD87::RangedPublishXR34();
 x::Source gap{};H95(x::ReadSource(gap)&&gap.auxiliaryUnavailableR95&&gap.tick==sourceTick);
 XR34PreControllerNative(a.controller.data(),.02f);
 H95(g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);
 a.now=sourceTick+81;sourceRouteD87::RangedPublishXR34();XR34PreControllerNative(a.controller.data(),.02f);
 H95(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);
 sourceRouteD87::explicitBodyR93R3=nullptr;XR34BeforeStopNative(a.parent.data());return 0;
}
static int nativeEscape95(int kind){
 Fixture a;a.source.classId=kind;RetailScopeD82 profile(a,kind);
 g_crossbow_xr34=x::Client{};g_crossbow_base_xr34=0;g_henry_ctrl=a.controller.data();
 x::registrationReader=&FixtureRegistrationXR34;x::identityReader=&FixtureIdentityXR34;x::RefreshRegistration();
 a.source.entryGripD72=0;a.source.entryGripTickD72=0;x::PublishSource(a.source);
 const auto writes=a.writes;XR34PostStartNative(a.parent.data(),true);
 H95(!g_crossbow_xr34.pending()&&a.writes==writes&&a.get<float>(a.p(a.child)+0x5C)==2.5f);
 // A fresh deliberate physical request is admitted normally.
 a.source.entryGripD72=kind==10?1:2;a.source.entryGripTickD72=a.now;x::PublishSource(a.source);
 XR34PostStartNative(a.parent.data(),true);H95(g_crossbow_xr34.pending());
 const auto serial=g_crossbow_xr34.status().owner.serial;
 H95(g_crossbow_xr34.restore(x::Reason::InputLost));XR34PublishNative();
 a.source.entryGripTickD72=++a.now;a.source.tick=a.now;x::PublishSource(a.source);
 XR34PostStartNative(a.parent.data(),true);H95(!g_crossbow_xr34.pending()&&g_crossbow_xr34.status().owner.serial==serial);
 // Actual native Stop ends the latch; the next action/request may reuse memory.
 XR34BeforeStopNative(a.parent.data());a.source.entryGripTickD72=++a.now;a.source.tick=a.now;
 a.put(a.p(a.child)+0x54,18u);x::PublishSource(a.source);XR34PostStartNative(a.parent.data(),true);
 H95(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().owner.serial>serial);
 // Native Stop may precede a successful retry of our exact restore debt.
 // That debt still blocks admission, but cannot permanently blacklist reused
 // action storage after it is retired.
 a.writeFailure=true;XR34BeforeStopNative(a.parent.data());H95(g_crossbow_xr34.pending());
 a.writeFailure=false;XR34PreControllerNative(a.controller.data(),.02f);H95(!g_crossbow_xr34.pending());
 a.source.tick=++a.now;a.source.entryGripTickD72=a.now;x::PublishSource(a.source);
 XR34PostStartNative(a.parent.data(),true);H95(g_crossbow_xr34.pending()); // same token/storage, actual new lifetime
 XR34BeforeStopNative(a.parent.data());
 // A consumed Grip cannot auto-reacquire another action; world/body changes
 // invalidate previous source and continuation rather than reviving it.
 x::PublishSource(a.source);XR34PostStartNative(a.parent.data(),true);H95(!g_crossbow_xr34.pending());
 ++a.source.body;++a.source.tracking;++a.source.request;a.source.entryGripD72=0;x::PublishSource(a.source);
 H95(!g_crossbow_xr34.acceptsNewActionR95(a.source,a.now));
 return 0;
}
int main(){for(int kind:{10,14,15}){if(advancingTransition95(kind)||nativeEscape95(kind))return 1;}
 std::cout<<"R95 advancing transition/native escape: "<<checksH95<<" production checks passed; native memory, scheduling and effects remain explicit leaves\n";return 0;}
