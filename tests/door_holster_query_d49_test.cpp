// Actual door ServiceState + actual holster producer. Native leaves are explicit test doubles.
#include "vrik/physical_weapon.h"
#include "subsystems/doors/hand_only_h40.h"
#include "vrik/holster_diag_r8d.h"
#include "vrik/bow_arm_aim_r9.h"
#include "diagnostics/capture168.h"
#include "subsystems/physics/corpse_grip_cg1.h"
#include "vrik/quick_talk_owner_h4.h"
#include "vrik/weapon_release_t1.h"
#include <cstdio>
#include <array>
#include <atomic>
#include <string>
#include <vector>
namespace ph=kcd2vr::physical;namespace pm=kcd2vr::pose_core;namespace equipment114=kcd2vr::equipment114;
static std::uint64_t tickD44=1000;static std::uint64_t GetTickCount64(){return tickD44;}
struct PhysEquipmentItem6698{ph::Profile profile{};bool held=false,dropUnitT1=false;int set=0,slot126=0;};
struct PhysEquipment6698{std::array<PhysEquipmentItem6698,8>items{};int count=0,selected=0;bool valid=true,handKnown[2]{true,true};std::uint64_t handKey[2]{};void*player=(void*)123;};
struct GripD44{std::uint64_t key=0,generation=0;bool mountReady=false,ready[2]{};struct{std::uint64_t tick=0,epoch=0;}mountStamp126;ph::Pose mount{},hand[2]{};};
struct PhysGripBank6698{std::array<GripD44,8>grip{};};
static kcd2vr::core::SnapshotMailbox<PhysGripBank6698>g_phys_grips6698;
static bool PhysSupplyHeldT2(const PhysEquipment6698&,const PhysEquipmentItem6698& item,int side){return side==1&&item.held&&item.profile.classId==0&&item.profile.key==777;}
static bool nativeBusyD48=false;static bool NativeHeldBusyT4(){return nativeBusyD48;}static bool PhysLeftHolsterRetiredH36(const ph::Input&,const PhysEquipment6698&){return true;}
static std::atomic<bool>g_native_held_release_inhibit_t4{false};static std::atomic<void*>g_committed_character6689{nullptr};
static bool claimD44[2]{};static bool HandClaimBlockedT24(int s,std::uintptr_t,std::uint64_t,std::uint64_t){return claimD44[s];}
namespace heldT4{struct Owner{std::uint64_t key=0,generation=0,epoch=0,request=0;int side=0;unsigned entityId=0;};static Owner ownerD48{};static bool ReadRetainedOwner(Owner& o){o=ownerD48;return nativeBusyD48;}}
namespace kcd2vr::haptics101{inline std::atomic<bool>enabled{false},allowed{false};inline std::atomic<unsigned>epoch{1};struct Pulse{std::uint64_t epoch,sample,tick,key;float strength;unsigned ms;};struct Queue{bool publish(Pulse){return true;}};inline Queue queue[2];}
namespace kcd2vr::diagnostic_trace{static std::vector<std::string> captured;static void Write(const char*line)noexcept{captured.emplace_back(line);}}
namespace kcd2vr::ammo_service_t22{inline bool integrated=true,leftChestKnivesT26=true;}
namespace kcd2vr::action_time_owner59c11{static bool ThrowingSupplyT2(){return false;}}
namespace kcd2vr::inventory_lifecycle_t8{inline std::atomic<int>daggers{0},axes{0};}
namespace diagnostic_trace=kcd2vr::diagnostic_trace;namespace melee118=kcd2vr::melee118;
#include "subsystems/throw_drop/holster_gestures.inl"
#include "subsystems/throw_drop/holster_commit_d48.inl"
static unsigned total=0,failed=0,badDrawClaims=0;static void check(bool b,const char*s){++total;if(!b){++failed;std::printf("FAIL %s\n",s);}}
struct Case{PhysEquipment6698 gear{};ph::Input in{};ph::Output out{};std::array<ph::HolsterState,8>state{};unsigned draws=0;Case(){gear.count=1;auto&w=gear.items[0];w.profile.key=1234;w.profile.generation=2;w.profile.classId=9;w.profile.side=1;w.set=1;in.bodyEpoch=in.epoch=1;in.tick=tickD44;in.requirePrimaryGrip114=in.allowed=true;in.tracked[0]=in.tracked[1]=in.intentTracked[0]=in.intentTracked[1]=true;in.shoulders[0]={.2,0,1.4};in.shoulders[1]={-.2,0,1.4};ph::holster_mount126(in,w.profile,in.hands[1].p);in.grip[1]=1;}
void seed(){auto&st=state[0].comparison114;st.initialized=st.latched=true;st.key=gear.items[0].profile.key;st.generation=gear.items[0].profile.generation;st.epoch=in.epoch;st.tick=st.started=in.tick;st.origin={};kcd2vr::corpse_grip_cg1::claims.capture(3,state[0].acquisitionCg1);}
void step(unsigned coord=3,bool full=false){tickD44+=16;in.tick=tickD44;++in.sample;out=ph::Output{};if(full)out.commandCount=out.commands.size();kcd2vr::quick_talk_owner_h4::mailbox.publish({in.tick,in.bodyEpoch,in.epoch,(std::uintptr_t)gear.player,0});PhysHolsterGestures6698(in,gear,out,state,3,coord);for(unsigned i=0;i<out.commandCount;++i)if(out.commands[i].kind==ph::CommandKind::Draw){++draws;if(out.commands[i].corpseClaimCg1.mask!=3)++badDrawClaims;}}
};

namespace hd=kcd2vr::hand_only_door_h40;
namespace kcd2vr::native_script_t2 {
struct Context {std::uint64_t now=0,generation=1;bool CallPacket(const char*,const char*,char*,std::size_t)const noexcept{return false;}};
static bool Register(void(*)(const Context&)noexcept,bool(*)()noexcept){return true;}
static bool Install(){return true;}
}
#include "subsystems/doors/hand_only_h40_service.inl"
static bool current(const hd::Frame&,int,std::uint64_t) noexcept{return true;}
struct ServiceFixture {
 enum Reply {NoDoor,RealDoor,Failure,Malformed,WrongIdentity,Rest};
 Case c;hd::Frame frame{};hd::ServiceState service{};Reply reply=NoDoor;
 bool observeInside=true;unsigned seenEntered=0,seenReserved=0,seenAcquired=0,feeds=0,clears=0,probes=0;bool rearmInside=false;
 ServiceFixture(){
  for(int i=0;i<2;++i){hd::claims[i]=hd::entered[i]=hd::acquiredD49[i]=hd::fallbackToken[i]=0;hd::neutral[i]=true;hd::pendingInput[i]={};
   kcd2vr::mutual_admission_t25::active[i]=kcd2vr::mutual_admission_t25::doorClaimsH41[i]=0;}
  hd::admissionBusy=hd::resting=0;hd::ready=true;hd::enabled=hd::transportInstalled=true;hd::vmQuarantine=false;hd::currentReader=&current;
  frame.tick=tickD44;frame.body=frame.request=frame.sample=frame.tracking=frame.policy=1;frame.actor=frame.character=frame.nativePlayer=frame.manager=1;frame.player=1;
  frame.permitted=frame.available[0]=frame.available[1]=true;frame.grip[1]=1;hd::completed.publish(frame);
  // Skip unrelated readiness probe. The production CandidatePress and feed/clear remain actual.
  service.vm=1;
  check(hd::CandidatePress(1,0,tickD44,true,1,0),"actual candidate reserves the same unknown-target Grip");
 }
 bool call(const char* input,char* output,std::size_t size) noexcept {
  unsigned long long serial=0;sscanf(input,"%llu|",&serial);
  if(std::strstr(input,"|probe|"))++probes;else if(std::strstr(input,"|feed|"))++feeds;else if(std::strstr(input,"|clear|"))++clears;
  seenEntered=hd::Entered();seenReserved=hd::Reserved();seenAcquired=hd::Acquired();
  if(observeInside){c.step(0);rearmInside=c.state[0].comparison114.requireReleaseR15;}
  if(reply==Failure)return false;
  if(reply==Malformed){std::snprintf(output,size,"bad");return true;}
  if(reply==WrongIdentity){std::snprintf(output,size,"OK|%llu|1|99|1|2|0|%llu|0|66",serial,serial);return true;}
  if(reply==RealDoor)std::snprintf(output,size,"OK|%llu|1|1|1|2|0|%llu|0|66",serial,serial);
  else if(reply==Rest)std::snprintf(output,size,"H40R1|%llu|1|1|1|0|0|%llu|0|0|1",serial,serial);
  else std::snprintf(output,size,"OK|%llu|1|1|1|0|0|%llu|0|0",serial,serial);
  return true;
 }
 void run(){service.tick(*this,tickD44,1);c.step();}
 void refresh(){frame.tick=tickD44;++frame.sample;hd::PublishCompleted(frame);}
};
int main(){
 {ServiceFixture x;x.observeInside=false;x.run();
  check(x.seenEntered==2&&x.seenReserved==2&&!x.seenAcquired,"entry debt is not a confirmed door");
  check(x.c.draws==1&&!hd::Reserved(),"nonoverlapping no-door response permits same Grip");}
 {ServiceFixture x;x.run();
  check(!x.rearmInside&&!x.seenAcquired,"in-flight unknown query does not consume valid Grip");
  // Successful dispatch intentionally sets requireRelease to prevent duplicate draws.
  check(x.c.draws==1&&x.c.state[0].comparison114.pending,"overlapping no-door response dispatches the same held Grip exactly once");
  check(x.feeds==1&&!x.clears&&!hd::Entered()&&!hd::Acquired(),"empty terminal ACK retires exact query once");}
 {ServiceFixture x;x.reply=ServiceFixture::RealDoor;x.run();
  check(!x.rearmInside&&x.seenAcquired==0,"unconfirmed first door query still defers without cancellation");
  check(hd::Acquired()==2&&hd::Entered()==2&&x.service.doors[1]==66,"validated concrete door acquires original token");
  check(!x.c.draws&&x.c.state[0].comparison114.requireReleaseR15,"real door cancels conflicting holster Grip");
  x.reply=ServiceFixture::Failure;x.refresh();x.run();
  check(hd::Acquired()==2&&hd::Reserved()==2&&!x.c.draws,"later failed callback cannot forget actual door owner");}
 for(auto reply:{ServiceFixture::Failure,ServiceFixture::Malformed,ServiceFixture::WrongIdentity}){
  ServiceFixture x;x.reply=reply;x.run();
  check(x.service.fault&&hd::Reserved()==2&&hd::Entered()==2&&!hd::Acquired(),"unknown first result retains native debt without inventing acquired door");
  check(!x.c.draws&&!x.c.state[0].comparison114.requireReleaseR15,"unknown callback debt defers draw without consuming Grip");
  x.reply=ServiceFixture::NoDoor;x.refresh();x.run();
  check(x.feeds==1&&x.clears==1&&!hd::Reserved()&&!hd::Entered(),"failure retries cleanup only and exact empty ACK retires debt");
  check(x.c.draws==1,"same valid Grip may dispatch after exact uncertain-debt cleanup");
 }
 {ServiceFixture x;x.reply=ServiceFixture::Failure;x.run();const auto token=hd::claims[1].load();
  x.service.tick(x,tickD44,2);x.c.step();
  check(hd::vmQuarantine&&hd::Reserved()==2&&hd::entered[1]==token&&!hd::Acquired(),"replacement VM cannot forgive uncertain original debt");
  check(x.feeds==1&&!x.clears&&!x.c.draws,"quarantine makes no second native call and dispatch stays deferred");}
 {ServiceFixture x;x.c.in.grip[1]=0;x.run();check(!x.c.draws,"Grip released during query cannot dispatch after no-door reply");}
 {ServiceFixture x;x.c.in.trigger[1]=1;x.run();check(!x.c.draws,"genuine Trigger plus Grip fist excludes holster");}
 {ServiceFixture x;x.c.gear.count=2;x.c.gear.items[1]=x.c.gear.items[0];auto& held=x.c.gear.items[1];held.profile.key=4321;held.profile.side=0;held.profile.classId=1;held.held=true;x.c.gear.handKey[0]=4321;
  x.run();check(!x.c.draws,"other held main weapon still excludes bow acquisition");}
 {ServiceFixture x;x.c.in.allowed=false;x.run();check(!x.c.draws,"gameplay revocation while query runs cannot replay draw");}
 {ServiceFixture x;x.c.in.intentTracked[1]=false;x.run();check(!x.c.draws,"tracking loss while query runs cannot replay draw");}
 {ServiceFixture x;x.c.seed();++x.c.in.epoch;x.run();check(!x.c.draws,"changed original request epoch cannot replay waiting draw");}
 {ServiceFixture x;x.c.seed();++x.c.gear.items[0].profile.generation;x.run();check(!x.c.draws,"changed item generation cannot replay waiting draw");}
 {ServiceFixture x;const auto token=hd::claims[1].load();hd::entered[1]=token;hd::acquiredD49[1]=token;
  check(!hd::RetireClaimH41(1,token+1)&&hd::Acquired()==2,"wrong terminal token cannot release a confirmed door");
  check(hd::RetireClaimH41(1,token)&&!hd::Acquired()&&!hd::acquiredD49[1],"matching terminal retires only its acquired token");
  hd::claims[1]=token+1;hd::entered[1]=token+1;hd::acquiredD49[1]=token;
  check(!hd::Acquired(),"stale acquired token cannot cancel a replacement request");}
 check(!badDrawClaims,"all permitted draws retain the real cross-feature occurrence qualification");
 {
  namespace cap=kcd2vr::diagnostics::capture168;
  using kcd2vr::diagnostic_trace::captured;
  const auto count=[](){unsigned n=0;for(const auto&line:captured)if(line.find("F5_HAND_ONLY_DOOR")!=std::string::npos)++n;return n;};
  captured.clear();hd::Diagnostic(tickD44);check(captured.empty(),"door diagnostics do no formatting/write while F5 and F2 are off");
  cap::gate.select(cap::Domain::Interaction,true,tickD44);hd::Diagnostic(tickD44);check(count()==1,"F5 records first actual receipt with acquisition distinction");
  hd::Diagnostic(tickD44);check(count()==1,"unchanged receipt does not repeat per frame");
  for(unsigned i=0;i<120;++i){hd::Receipt r{};r.stage=(i&1)?hd::Stage::Feed:hd::Stage::Retained;r.acquired=(i&1)?2u:0u;hd::receipts.publish(r);hd::Diagnostic(tickD44);}
  check(count()==96&&captured.back().find("detailBudgetExhausted=1")!=std::string::npos,"F5 receipt detail stops at explicit 96-edge budget");
  cap::gate.select(cap::Domain::Interaction,false,tickD44);cap::gate.select(cap::Domain::Bug,true,tickD44);captured.clear();hd::Diagnostic(tickD44);
  check(count()==1&&captured.size()==1,"F2 observes receipt without extra door-display snapshots");
  cap::gate.select(cap::Domain::Bug,false,tickD44);captured.clear();hd::Diagnostic(tickD44);check(captured.empty(),"ending F2 immediately silences these producers");
 }
 std::printf("D49 production door service + holster producer: %u/%u; native Lua leaves simulated; no headset execution\n",total-failed,total);
 return failed?1:0;
}
