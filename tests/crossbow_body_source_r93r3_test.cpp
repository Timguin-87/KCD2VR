// Actual native clock/lease, preparation, completed-source and consumer joins.
// The attachment descriptor and retail memory are explicit fixture leaves.
#define KCD2VR_SCOPE_NO_MAIN
#include "crossbow_scope_d82_test.cpp"
#undef KCD2VR_SCOPE_NO_MAIN
#include "crossbow_source_route_d87_fixture.h"
namespace preparationR93R3 {
static std::array<unsigned char,512> model{},attachment{},manager{};
static std::array<unsigned char,32> weaponEntity{};
static const char* weaponName="weapon_left";
static bool mutateClock=false,republish=false,contract=true;
static int relativeReads=0;
static std::atomic<void*> g_henry_char_attachment_manager{};
template<class T> static bool Read(const void* at,T& out) noexcept {
 const auto address=reinterpret_cast<std::uintptr_t>(at);
 if(address==base+0x40AC2B0&&sizeof(T)==sizeof(weaponName)){std::memcpy(&out,&weaponName,sizeof(T));return true;}
 if(at==attachment.data()+0x9C&&++relativeReads==2){
  if(mutateClock)f->put(f->entry()+0x44,.71f);
  if(republish){XR34PublishNative();XR34PostControllerNative(f->controller.data());}
 }
 return ::Read(at,out);
}
static bool Read(const void* at,QuatT28& out) noexcept{return preparationR93R3::Read<QuatT28>(at,out);}
static bool PhysSocketContract101(std::uintptr_t b) noexcept{return b==base&&contract;}
static bool RangedItemEntityXR34(void* item,unsigned& id,void*& entity) noexcept {
 if(reinterpret_cast<std::uintptr_t>(item)!=f->source.weaponItem)return false;
 id=88;entity=weaponEntity.data();return true;
}
static void* ManualFindHandAttachment19c4r4(void* m,const char* n) noexcept{return m==model.data()&&n==weaponName?attachment.data():nullptr;}
static bool ManualBoundEntity19c4r4(void* a,unsigned id,void* e) noexcept{return a==attachment.data()&&id==88&&e==weaponEntity.data();}
static QuatT28 AbsoluteFromRelative(const QuatT28& a,const QuatT28& b) noexcept {
 const auto c=ph::compose(PhysPose6698(a),PhysPose6698(b));return {{float(c.q.x),float(c.q.y),float(c.q.z),float(c.q.w)},{float(c.p.x),float(c.p.y),float(c.p.z)}};
}
static void setup(Fixture& a){
 a.block(model);a.block(attachment);a.block(manager);a.block(weaponEntity);
 a.put(a.source.player+0x1B0,a.p(model));a.put(a.p(model)+0x20,a.source.player);
 a.put(a.p(attachment)+0x28,a.p(manager));a.put(a.p(manager)+0x70,a.source.character);
 a.put(a.p(attachment)+0x78,std::int16_t(0));a.put(a.p(attachment)+0x7A,(unsigned char)0);
 a.put(a.p(attachment)+0x9C,QuatT28{});g_henry_char_attachment_manager=manager.data();
 mutateClock=false;republish=false;contract=true;relativeReads=0;
}
#include "prepare_reload_body_r93r3_actual.inc"
}
static unsigned checked=0;
#define C93(c) do{++checked;if(!(c)){std::cerr<<"FAIL_R93R3 line="<<__LINE__<<" " #c "\n";return 1;}}while(0)
static CommittedFrame6695 bodyFor(const x::Source& s){
 CommittedFrame6695 p{};p.reloadSourceXR34=s;p.reloadSourceXR34.bodyPublicationR95=p.publicationR24;p.stamp={s.body,s.sample,s.tick,s.character};std::memcpy(p.world,s.world,sizeof(p.world));
 for(int side=0;side<2;++side){std::memcpy(p.native[side].q,s.nativeWrist[side].q,sizeof(p.native[side].q));std::memcpy(p.native[side].t,s.nativeWrist[side].p,sizeof(p.native[side].t));
  std::memcpy(p.allowed[side].q,s.allowedWrist[side].q,sizeof(p.allowed[side].q));std::memcpy(p.allowed[side].t,s.allowedWrist[side].p,sizeof(p.allowed[side].t));
  std::memcpy(p.propRequested5916c1[side].q,s.intentWristD78[side].q,sizeof(p.propRequested5916c1[side].q));std::memcpy(p.propRequested5916c1[side].t,s.intentWristD78[side].p,sizeof(p.propRequested5916c1[side].t));}
 return p;
}
#ifndef KCD2VR_BODY_SOURCE_NO_MAIN
int main(){
 for(int kind:{10,14,15}){
  Fixture a;a.source.classId=kind;a.source.physicalD81=true;a.source.intentKnownD78=true;a.source.chestKnown=true;
  a.spatial.toolBound=false;a.spatial.crankQualified=true;a.spatial.travel=.08;RetailScopeD82 scope(a,kind);
  start82(a);completeFirstPose82(a,admission82(a));XR34PostControllerNative(a.controller.data());
  x::Passenger passenger{};C93(ReadReloadPassengerXR34(passenger,a.source.character,a.source.body,a.now));
  preparationR93R3::setup(a);auto complete=bodyFor(a.source);sourceRouteD87::explicitBodyR93R3=&complete;
  sourceRouteD87::retained=sourceRouteD87::enabled=true;x::Source read{};
  C93(sourceRouteD87::RangedSourceXR34(read)&&read.sample==complete.stamp.sample);
  auto next=complete;const x::Source emptySourceR95{};next.reloadSourceXR34=emptySourceR95;++next.stamp.sample;++a.now;next.stamp.tick=a.now;
  next.allowed[0].t[0]+=.015f;next.propRequested5916c1[0].t[0]+=.015f;
  TrackingSnapshot tracking{};tracking.sequence=next.stamp.sample;tracking.tick=next.stamp.tick;tracking.source_epoch=a.source.tracking;
  tracking.right_grip=.8f;tracking.left_grip=1;QuatT28 original[1]{};
  C93(preparationR93R3::PrepareReloadPoseXR34(passenger,next,original,1,tracking));
  C93(next.reloadSourceXR34.sample==next.stamp.sample&&next.reloadSourceXR34.tick==next.stamp.tick);
  C93(sourceRouteD87::RangedSourceXR34(read)&&read.sample==complete.stamp.sample); // prepared but not committed
  auto mixed=next;mixed.reloadSourceXR34=complete.reloadSourceXR34;sourceRouteD87::explicitBodyR93R3=&mixed;
  C93(!sourceRouteD87::RangedSourceXR34(read)); // the former two-mailbox interleaving
  next.publicationR24=4;next.reloadSourceXR34.bodyPublicationR95=4;complete=next;sourceRouteD87::explicitBodyR93R3=&complete;
  C93(sourceRouteD87::RangedSourceXR34(read)&&read.sample==next.stamp.sample&&read.allowedWrist[0].p[0]==next.allowed[0].t[0]);
  C93(read.tick==next.stamp.tick&&read.completedLease==passenger.owner.serial);
  auto repeatedSample=complete;repeatedSample.publicationR24+=2;sourceRouteD87::explicitBodyR93R3=&repeatedSample;
  C93(!sourceRouteD87::RangedSourceXR34(read)); // same XR sample/tick, different body publication is still a mismatch
  sourceRouteD87::explicitBodyR93R3=&complete;
  // Reentrant native post-controller publication, same paused clock and
  // exact native binding: actual preparation retries rather than canceling.
  auto retried=next;preparationR93R3::relativeReads=0;preparationR93R3::republish=true;
  const auto beforePublication=passenger.publication;
  const bool retryOk=preparationR93R3::PrepareReloadPoseXR34(passenger,retried,original,1,tracking);
  if(!retryOk){x::Passenger debug{};const bool current=ReadReloadPassengerXR34(debug,a.source.character,a.source.body,a.now);std::cerr<<"retry current="<<current<<" pub="<<passenger.publication<<"/"<<debug.publication<<" clock="<<std::memcmp(&passenger.clock,&debug.clock,sizeof(debug.clock))<<" clip="<<std::memcmp(&passenger.clip,&debug.clip,sizeof(debug.clip))<<" binding="<<std::memcmp(&passenger.binding,&debug.binding,sizeof(debug.binding))<<" constraint="<<std::memcmp(&passenger.constraintD78,&debug.constraintD78,sizeof(debug.constraintD78))<<"\n";}
  C93(retryOk);
  // R95 keeps identical observed native state under the same lease instead
  // of forcing a spurious publication gap. Exact changed-pose retries below
  // still reject a different clock, binding, constraint or native owner.
  C93(x::passengerPublication.load()==beforePublication&&retried.reloadSourceXR34.sample==next.stamp.sample);
  C93(ReloadPassengerCurrentXR34(passenger,a.source.character,a.source.body,a.now)&&passenger.publication==x::passengerPublication.load());
  preparationR93R3::republish=false;
  auto changed=passenger;++changed.publication;
  C93(preparationR93R3::SameReloadPoseRetryR93R3(passenger,changed));
  changed.clock.normalized+=.01f;C93(!preparationR93R3::SameReloadPoseRetryR93R3(passenger,changed));
  changed=passenger;++changed.publication;++changed.binding.boltId;C93(!preparationR93R3::SameReloadPoseRetryR93R3(passenger,changed));
  changed=passenger;++changed.publication;changed.constraintD78.progress+=.01;C93(!preparationR93R3::SameReloadPoseRetryR93R3(passenger,changed));
  changed=passenger;++changed.publication;++changed.owner.serial;C93(!preparationR93R3::SameReloadPoseRetryR93R3(passenger,changed));
  // No global source/body is touched if the exact clock or native descriptor
  // changes while preparation is running. Reload remains empty; the caller
  // may still commit its independently valid ordinary body.
  const auto saved=complete;auto rejected=next;rejected.reloadSourceXR34=emptySourceR95;preparationR93R3::relativeReads=0;preparationR93R3::mutateClock=true;
  C93(!preparationR93R3::PrepareReloadPoseXR34(passenger,rejected,original,1,tracking)&&!rejected.reloadSourceXR34.allowed);
  C93(std::memcmp(&complete,&saved,sizeof(saved))==0);
  a.put(a.entry()+0x44,passenger.clock.normalized);preparationR93R3::mutateClock=false;preparationR93R3::relativeReads=0;
  a.put(a.p(preparationR93R3::attachment)+0x7A,(unsigned char)1);
  C93(!preparationR93R3::PrepareReloadPoseXR34(passenger,rejected,original,1,tracking));
  a.put(a.p(preparationR93R3::attachment)+0x7A,(unsigned char)0);
  for(int bad=0;bad<5;++bad){auto p=next;auto t=tracking;if(bad==0)p.stamp.actor++;if(bad==1)p.stamp.epoch++;if(bad==2)t.sequence++;if(bad==3)t.right_held=true;if(bad==4)t.allowed=false;
   C93(!preparationR93R3::PrepareReloadPoseXR34(passenger,p,original,1,t));}
  a.now=complete.stamp.tick+81;C93(!sourceRouteD87::RangedSourceXR34(read));
  sourceRouteD87::explicitBodyR93R3=nullptr;XR34BeforeStopNative(a.parent.data());
 }
 std::cout<<"R93R3 atomic body/source: "<<checked<<" source-connected checks passed; native socket/memory leaves explicit\n";return 0;
}
#endif
