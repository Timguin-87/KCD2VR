// Executes the real producer, exact preparation, source publication, native
// clock client and adapter. Retail memory/scheduling/context remain explicit
// leaves inherited from the supplied source-connected fixture.
#define main bodySourceLegacyMainR95
#include "crossbow_body_source_r93r3_test.cpp"
#undef main
namespace sourceRouteD87 {
static std::uint64_t currentPublicationR95=2;
static void RangedReloadEquipmentXR34() noexcept {}
static std::uint64_t ReadCompletedPublicationT23() noexcept {return currentPublicationR95;}
static bool RangedSpatialXR34(const x::Owner& o,const x::Source& accepted,x::Spatial& s) noexcept {return spatial82(o,accepted,s);}
#include "publish_source_r95_actual.inc"
}
static unsigned checksR95=0;
#define C95(c) do{++checksR95;if(!(c)){std::cerr<<"FAIL_R95 line="<<__LINE__<<" " #c " reason="<<x::reasonName(g_crossbow_xr34.status().reason)<<"\n";return 1;}}while(0)
static bool bindingR95(const x::Owner& o,x::Spatial& b) noexcept {b=f->spatial;b.serial=o.serial;b.qualified=XR34Current(o);return b.qualified;}
static void beginGapR95(Fixture& a,int kind){
 a.source.classId=kind;a.source.physicalD81=true;a.source.intentKnownD78=true;a.source.chestKnown=true;
 a.spatial.toolBound=false;a.spatial.crankQualified=true;a.spatial.travel=.08;
 start82(a);completeFirstPose82(a,admission82(a));a.source.bodyPublicationR95=2;x::PublishSource(a.source);XR34PostControllerNative(a.controller.data());
 sourceRouteD87::retained=sourceRouteD87::enabled=true;sourceRouteD87::currentPublicationR95=2;
 x::bindingReader=&bindingR95;preparationR93R3::setup(a);
}
static int auxiliaryGapR95(int kind,bool expire){
 Fixture a;a.source.classId=kind;RetailScopeD82 profile(a,kind);beginGapR95(a,kind);
 x::Passenger passenger{};C95(ReadReloadPassengerXR34(passenger,a.source.character,a.source.body,a.now));
 auto complete=bodyFor(a.source);sourceRouteD87::explicitBodyR93R3=&complete;
 const auto original=a.source;const auto cancellation=x::sourceCancellationR93.load();
 auto next=complete;const x::Source empty{};next.reloadSourceXR34=empty;++next.stamp.sample;++a.now;next.stamp.tick=a.now;next.publicationR24=4;
 TrackingSnapshot tracking{};tracking.sequence=next.stamp.sample;tracking.tick=a.now;tracking.source_epoch=a.source.tracking;
 tracking.right_grip=.9f;tracking.left_grip=1;QuatT28 native[1]{};
 a.put(a.p(preparationR93R3::attachment)+0x7A,(unsigned char)1);
 C95(!preparationR93R3::PrepareReloadPoseXR34(passenger,next,native,1,tracking));
 C95(!next.reloadSourceXR34.allowed&&ReloadPassengerCurrentXR34(passenger,a.source.character,a.source.body,a.now));
 complete=next;sourceRouteD87::currentPublicationR95=4;
 x::Source read{};C95(!sourceRouteD87::RangedSourceXR34(read));sourceRouteD87::RangedPublishXR34();
 C95(x::sourceCancellationR93.load()==cancellation&&x::ReadSource(read)&&read.auxiliaryUnavailableR95);
 C95(read.sample==original.sample&&read.tick==original.tick&&read.bodyPublicationR95==original.bodyPublicationR95);
 const auto publication=read.publication;const auto events=a.originalEvents;const auto progress=g_crossbow_xr34.status().progress;
 const auto clock=a.get<float>(a.entry()+0x44);nativeFrame(a);
 C95(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::WaitingSourceR95);
 C95(a.get<float>(a.p(a.child)+0x5C)==0&&a.get<float>(a.entry()+0x44)==clock);
 C95(!g_crossbow_xr34.status().constraintD78.active&&g_crossbow_xr34.status().progress==progress&&a.originalEvents==events&&a.cocks==0&&a.loads==0);
 // Failed ordinary body publications do not refresh the original source age,
 // source publication, sample, or body serial. A release in that body is not
 // a reload Grip edge or ammunition permission.
 for(int elapsed:{20,40,60,80}){
  a.now=original.tick+elapsed;++complete.stamp.sample;complete.stamp.tick=a.now;complete.publicationR24+=2;sourceRouteD87::currentPublicationR95=complete.publicationR24;
  sourceRouteD87::RangedPublishXR34();C95(x::ReadSource(read)&&read.publication==publication&&read.tick==original.tick&&read.sample==original.sample);
  nativeFrame(a);C95(g_crossbow_xr34.pending()&&a.originalEvents==events&&a.get<float>(a.entry()+0x44)==clock);
  if(!expire)break;
 }
 if(expire){
  a.now=original.tick+81;sourceRouteD87::RangedPublishXR34();C95(!x::ReadSource(read)&&x::sourceCancellationR93.load()!=cancellation);
  tick(a);C95(!g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::InputLost);
 }else{
  a.put(a.p(preparationR93R3::attachment)+0x7A,(unsigned char)0);preparationR93R3::relativeReads=0;
  C95(ReadReloadPassengerXR34(passenger,a.source.character,a.source.body,a.now));
  tracking.sequence=complete.stamp.sample+1;tracking.tick=++a.now;complete.stamp.sample=tracking.sequence;complete.stamp.tick=tracking.tick;
  C95(preparationR93R3::PrepareReloadPoseXR34(passenger,complete,native,1,tracking));
  complete.publicationR24+=2;complete.reloadSourceXR34.bodyPublicationR95=complete.publicationR24;sourceRouteD87::currentPublicationR95=complete.publicationR24;
  sourceRouteD87::RangedPublishXR34();C95(x::ReadSource(read)&&!read.auxiliaryUnavailableR95&&read.tick==a.now&&x::sourceCancellationR93.load()==cancellation);
  a.source=read;nativeFrame(a);C95(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().owner.serial==passenger.owner.serial&&g_crossbow_xr34.status().reason!=x::Reason::WaitingSourceR95);
 }
 sourceRouteD87::explicitBodyR93R3=nullptr;XR34BeforeStopNative(a.parent.data());return 0;
}
static int trueRevokeR95(int defect,bool odd){
 Fixture a;RetailScopeD82 profile(a,14);beginGapR95(a,14);auto complete=bodyFor(a.source);const x::Source empty{};complete.reloadSourceXR34=empty;
 sourceRouteD87::explicitBodyR93R3=&complete;sourceRouteD87::currentPublicationR95=odd?3:2;sourceRouteD87::retained=!odd;
 if(defect==0)++a.source.tracking;
 if(defect==1)++a.source.body;
 if(defect==2)++a.source.request;
 if(defect==3)a.put(a.p(a.child)+0x54,77u);
 if(defect==4)a.put(a.p(a.equipment)+0x40,a.source.weaponItem+8);
 if(defect==5)sourceRouteD87::enabled=false;
 if(defect==6)a.current=false; // current/protected-context identity leaf
 if(defect==7)kcd2vr::ammo_carry_t18::hands=1u;
 if(defect==8){complete.stamp.actor++;if(odd){sourceRouteD87::retained=true;sourceRouteD87::currentPublicationR95=2;}}
 if(defect==9){complete.valid[0]=false;if(odd){sourceRouteD87::retained=true;sourceRouteD87::currentPublicationR95=2;}}
 if(defect==10){complete.reloadSourceXR34=a.source;complete.reloadSourceXR34.bodyPublicationR95=99;
  sourceRouteD87::retained=true;sourceRouteD87::currentPublicationR95=2;}
 const auto cancel=x::sourceCancellationR93.load();sourceRouteD87::RangedPublishXR34();x::Source read{};
 C95(x::sourceCancellationR93.load()!=cancel&&!x::ReadSource(read));
 a.current=true;tick(a);C95(!g_crossbow_xr34.pending());sourceRouteD87::retained=sourceRouteD87::enabled=true;
 sourceRouteD87::explicitBodyR93R3=nullptr;kcd2vr::ammo_carry_t18::hands=0;XR34BeforeStopNative(a.parent.data());return 0;
}
static int movingGapR95(bool bindingReplacement){
 Fixture a;RetailScopeD82 profile(a,14);a.source.physicalD81=true;a.source.intentKnownD78=true;a.source.chestKnown=true;
 animateMechanismD87=true;start82(a);completeFirstPose82(a,admission82(a));x::bindingReader=&bindingR95;
 input82(a,0);nativeFrame(a);a.source.handInChest[0]=true;input82(a,.9);nativeFrame(a);a.source.handInChest[0]=false;
 for(int n=0;n<3;++n){input82(a,.9,{.3,0,0});nativeFrame(a);}
 input82(a,0);nativeFrame(a);input82(a,.9);nativeFrame(a);input82(a,.9,{0,-a.spatial.travel*.5,0});nativeFrame(a);
 C95(a.get<float>(a.p(a.child)+0x5C)>0&&g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);
 auto complete=bodyFor(a.source);const x::Source empty{};complete.reloadSourceXR34=empty;sourceRouteD87::explicitBodyR93R3=&complete;
 sourceRouteD87::currentPublicationR95=2;sourceRouteD87::retained=sourceRouteD87::enabled=true;
 const auto events=a.originalEvents;const auto clock=a.get<float>(a.entry()+0x44);const auto progress=g_crossbow_xr34.status().progress;
 sourceRouteD87::RangedPublishXR34();if(bindingReplacement)++a.spatial.toolEntity;
 if(bindingReplacement){tick(a);C95(!g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::PhysicalBindingLost);}
 else{nativeFrame(a);C95(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::WaitingSourceR95);
  C95(a.get<float>(a.p(a.child)+0x5C)==0&&a.get<float>(a.entry()+0x44)==clock&&g_crossbow_xr34.status().progress==progress&&a.originalEvents==events);}
 sourceRouteD87::explicitBodyR93R3=nullptr;XR34BeforeStopNative(a.parent.data());return 0;
}
// A release while the auxiliary body payload is unavailable must be retired,
// not replayed as the release-to-nock when a later placement becomes valid.
static int releaseDuringGapR94(int kind){
 Fixture a;a.source.classId=kind;RetailScopeD82 profile(a,kind);
 a.source.physicalD81=true;a.source.intentKnownD78=true;a.source.chestKnown=a.source.quiverKnownD72=true;
 a.spatial.toolBound=false;a.spatial.boltId=77;a.spatial.boltKey=a.source.ammoKey;
 a.spatial.boltEntity=a.p(a.player)+0x80;a.spatial.boltAttachment=a.p(a.item)+0x100;
 start82(a,true);completeFirstPose82(a,admission82(a));x::bindingReader=&bindingR95;
 input82(a,0);nativeFrame(a);a.source.handInQuiverD72[0]=true;input82(a,.9);nativeFrame(a);
 a.source.handInQuiverD72[0]=false;
 for(int n=0;n<3;++n){input82(a,.9,{.3,0,0});nativeFrame(a);}
 C95(g_crossbow_xr34.status().phase==x::Phase::PlaceBolt&&g_crossbow_xr34.pending()&&a.loads==0);
 const auto events=a.originalEvents;const auto original=a.source;
 auto complete=bodyFor(a.source);const x::Source empty{};complete.reloadSourceXR34=empty;
 sourceRouteD87::explicitBodyR93R3=&complete;sourceRouteD87::retained=sourceRouteD87::enabled=true;
 sourceRouteD87::currentPublicationR95=complete.publicationR24;
 TrackingSnapshot raw{};raw.sequence=original.sample+1;raw.tick=++a.now;raw.source_epoch=original.tracking;
 raw.right_grip=0;raw.left_grip=1;sourceRouteD87::rawTrackingOverrideR95=&raw;
 sourceRouteD87::RangedPublishXR34();x::Source paused{};
 C95(x::ReadSource(paused)&&paused.auxiliaryUnavailableR95&&paused.auxiliaryReleasedR95);
 C95(paused.sample==original.sample&&paused.tick==original.tick&&paused.grip[0]==original.grip[0]);
 nativeFrame(a);C95(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().reason==x::Reason::WaitingSourceR95&&a.loads==0&&a.originalEvents==events);
 // Repeated observation, including a reclosed hand, cannot unconsume release
 // or refresh source/pose age. No placement/event is granted while paused.
 const auto publication=paused.publication;raw.right_grip=.9f;raw.tick=++a.now;++raw.sequence;
 sourceRouteD87::RangedPublishXR34();C95(x::ReadSource(paused)&&paused.auxiliaryReleasedR95&&paused.publication==publication&&paused.tick==original.tick);
 sourceRouteD87::rawTrackingOverrideR95=nullptr;
 a.spatial.handBoltPlaced=true;input82(a,0,{});complete=bodyFor(a.source);
 sourceRouteD87::RangedPublishXR34();nativeFrame(a);C95(g_crossbow_xr34.pending()&&a.loads==0&&a.originalEvents==events);
 input82(a,.9,{});nativeFrame(a);input82(a,0,{});nativeFrame(a);C95(a.loads==1&&a.spatial.nockBound);
 for(int i=0;i<3;++i){input82(a,0,{});nativeFrame(a);}C95(a.loads==1);
 sourceRouteD87::explicitBodyR93R3=nullptr;XR34BeforeStopNative(a.parent.data());return 0;
}
static int rawGapLossR94(int defect){
 Fixture a;RetailScopeD82 profile(a,14);beginGapR95(a,14);auto complete=bodyFor(a.source);
 const x::Source empty{};complete.reloadSourceXR34=empty;sourceRouteD87::explicitBodyR93R3=&complete;
 TrackingSnapshot raw{};raw.sequence=a.source.sample;raw.tick=a.now;raw.source_epoch=a.source.tracking;raw.right_grip=.9;raw.left_grip=1;
 if(defect==0)raw.allowed=false;if(defect==1)raw.head_valid=false;if(defect==2)raw.right_held=true;
 if(defect==3)++raw.source_epoch;if(defect==4)raw.sequence--;
 if(defect==5)raw.tick++;if(defect==6)raw.tick=a.now-81;
 if(defect==7)raw.right_grip=std::numeric_limits<float>::quiet_NaN();
 if(defect==8)sourceRouteD87::rawTrackingAvailableR95=false;
 sourceRouteD87::rawTrackingOverrideR95=&raw;
 const auto cancellation=x::sourceCancellationR93.load();sourceRouteD87::RangedPublishXR34();x::Source out{};
 C95(!x::ReadSource(out)&&x::sourceCancellationR93.load()!=cancellation);tick(a);C95(!g_crossbow_xr34.pending());
 sourceRouteD87::rawTrackingAvailableR95=true;sourceRouteD87::rawTrackingOverrideR95=nullptr;
 sourceRouteD87::explicitBodyR93R3=nullptr;XR34BeforeStopNative(a.parent.data());return 0;
}
static int handPolicyR95(){
 for(int kind:{10,14,15})for(int primary:{0,1}){
  x::Passenger p{};p.valid=true;p.frame.primary=primary;p.workingHand=1-primary;p.owner.source.classId=kind;p.owner.source.physicalD81=true;
  const int working=1-primary;auto policy=x::handPolicyR95(p);
  C95(policy.tracked[0]&&policy.tracked[1]&&policy.owner[primary]==x::HandOwnerR95::PrimaryGrip&&!policy.worldContact[primary]&&policy.nativeFingers[primary]);
  C95(policy.owner[working]==x::HandOwnerR95::FreeWorking&&policy.worldContact[working]&&!policy.nativeFingers[working]);
  p.binding.handBoltBound=true;policy=x::handPolicyR95(p);C95(policy.owner[working]==x::HandOwnerR95::HandBolt&&!policy.worldContact[working]&&policy.nativeFingers[working]);
  p.binding.handBoltBound=false;p.binding.toolBound=true;policy=x::handPolicyR95(p);C95(policy.owner[working]==x::HandOwnerR95::HandTool&&!policy.worldContact[working]&&policy.nativeFingers[working]);
  p.binding.toolWeaponBoundD72=true;policy=x::handPolicyR95(p);C95(policy.owner[working]==x::HandOwnerR95::FreeWorking&&policy.worldContact[working]);
  p.constraintD78.active=true;policy=x::handPolicyR95(p);C95(policy.owner[working]==x::HandOwnerR95::String&&!policy.worldContact[working]);
  p.constraintD78.mounted=true;policy=x::handPolicyR95(p);C95(policy.owner[working]==x::HandOwnerR95::MountedMechanism&&!policy.worldContact[working]);
  p.frame.auxiliaryUnavailableR95=true;policy=x::handPolicyR95(p);C95(policy.owner[working]==x::HandOwnerR95::FreeWorking);
  p.valid=false;policy=x::handPolicyR95(p);C95(!policy.tracked[0]&&!policy.tracked[1]&&!policy.worldContact[0]&&!policy.worldContact[1]);
 }
 return 0;
}
int main(){
 for(int kind:{10,14,15})for(bool expire:{false,true})if(auxiliaryGapR95(kind,expire))return 1;
 for(bool odd:{false,true})for(int bad=0;bad<11;++bad)if(trueRevokeR95(bad,odd))return 1;
 for(bool replacement:{false,true})if(movingGapR95(replacement))return 1;
 for(int kind:{10,14,15})if(releaseDuringGapR94(kind))return 1;
 for(int bad=0;bad<9;++bad)if(rawGapLossR94(bad))return 1;
 if(handPolicyR95())return 1;
 std::cout<<"R95 auxiliary source/hand policy: "<<checksR95<<" production checks passed; native memory, scheduling and context leaves explicit\n";
}
