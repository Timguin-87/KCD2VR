// Actual retail adapter/client with explicit native memory/clock leaves only.
// Prepared during source hold: not compiled or executed by this change.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "crossbow_native_xr34_test.cpp"
static void startTransitionD80(Fixture& a){
 g_crossbow_xr34=x::Client{};g_crossbow_base_xr34=0;x::revokedNativeLease=0;
 g_henry_ctrl.store(a.controller.data());x::registrationReader=&FixtureRegistrationXR34;
 x::identityReader=&FixtureIdentityXR34;x::RefreshRegistration();x::SetSpatialReader(&spatial);x::PublishSource(a.source);
 a.put(a.p(a.eventStorage)+12,2u);
 a.event(0,.2990909f,0x12345678u);a.event(1,.3743182f,0x12345679u); // harmless native audio leaves
 XR34PostStartNative(a.parent.data(),true);assert(g_crossbow_xr34.pending());a.activate();
 XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
 assert(g_crossbow_xr34.status().phase==x::Phase::NativeTransitionD80);
 assert(g_crossbow_xr34.clip().nativeTransitionD80);
 assert(!g_crossbow_xr34.status().placeTool&&!g_crossbow_xr34.status().placeBolt);
}
int main(){
 for(int kind:{10,14,15}){
  Fixture a;a.source.classId=kind;startTransitionD80(a);
  const auto serial=g_crossbow_xr34.status().owner.serial;
  const auto deadline=g_crossbow_xr34.status().firstPoseDeadline;
  tick(a);assert(a.get<float>(a.p(a.child)+0x5C)==0); // bounded first wrist, no invented completion
  a.source.completedLease=serial;x::PublishSource(a.source);a.input(.8);tick(a);
  assert(a.get<float>(a.p(a.child)+0x5C)==2.5f); // original stat speed, never instant transport speed
  x::Passenger p{};assert(x::ReadPassenger(p,a.now));assert(!p.constraintD78.active);
  a.put(a.entry()+0x44,.29f);nativeFrame(a);
  assert(a.originalEvents==1&&a.cocks==0&&a.loads==0&&a.spawns==0);
  // More than the original 80ms is fine only with fresh completed source,
  // not an extended/rebased first-pose deadline.
  for(int i=0;i<6;++i){a.input(.8);tick(a);}
  assert(g_crossbow_xr34.status().firstPoseDeadline==deadline);
  a.events(false);a.put(a.entry()+0x44,0.f);a.put(a.p(a.child)+0x10,0.f);
  XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
  assert(g_crossbow_xr34.status().phase==x::Phase::Setup&&!g_crossbow_xr34.clip().nativeTransitionD80);
  assert(a.get<float>(a.p(a.child)+0x5C)==0); // no old transition-rate leak during same Update
  assert(g_crossbow_xr34.status().owner.serial==serial);
  assert(g_crossbow_xr34.status().firstPoseDeadline==deadline);
  a.input(.8);tick(a);assert(a.cocks==0&&a.loads==0); // gesture/native events still required
  XR34BeforeStopNative(a.parent.data());assert(!g_crossbow_xr34.pending());
  assert(a.get<float>(a.p(a.child)+0x5C)==2.5f);
 }
 {Fixture a;startTransitionD80(a);a.now+=81;tick(a);
  assert(g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==0); // D85 first-pose pause is bounded separately from live tracking
  a.now=g_crossbow_xr34.status().firstPoseDeadline+1;tick(a);
  assert(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);}
 {Fixture a;startTransitionD80(a);a.source.completedLease=g_crossbow_xr34.status().owner.serial;x::PublishSource(a.source);tick(a);
  a.source.allowed=false;x::PublishSource(a.source);tick(a);assert(!g_crossbow_xr34.pending());}
 {Fixture a;startTransitionD80(a);a.put(a.p(a.child)+0x5C,.75f);XR34BeforeStopNative(a.parent.data());
  assert(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==.75f);}
 {Fixture a;startTransitionD80(a);a.put(a.p(a.child)+0x54,999u);tick(a);assert(!g_crossbow_xr34.pending());}
 {Fixture a;startTransitionD80(a);a.event(0,.2f,0xAFE0B3FFu);
  XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
  assert(!g_crossbow_xr34.pending());assert(!std::strcmp(x::lastClipRefusalD72.load(),"CLIP_SEMANTIC_MARKERS_WITHOUT_ENDPOINT"));}
 {Fixture a;startTransitionD80(a);a.event(0,.2f,0x8ABB129Au);a.event(1,.8f,0xEF667DEAu);
  XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);assert(!g_crossbow_xr34.pending());}
 {Fixture a;startTransitionD80(a);a.source.completedLease=g_crossbow_xr34.status().owner.serial;x::PublishSource(a.source);tick(a);
  a.events();a.put(a.entry()+0x44,0.f);XR34PostMetadataNative(a.parent.data(),a.child.data(),5,0);
  assert(g_crossbow_xr34.interval(a.p(a.caf)+0x48,5,0,0,.1f)); // exact effect-free prefix of the new CAF
  assert(!g_crossbow_xr34.interval(a.p(a.caf)+0x48,5,0,.1f,.20001f)); // SpawnLever cannot borrow transition credit
  assert(!g_crossbow_xr34.pending()&&a.cocks==0&&a.loads==0);}
 {Fixture a;startTransitionD80(a);XR34BeforeCompleteNative(a.parent.data(),a.child.data());
  assert(!g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::NativeComplete);}
 std::puts("D80 actual reload transition adapter: native clock/events preserved; headset NOT_RUN");
}
