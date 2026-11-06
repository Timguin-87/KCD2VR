#include "subsystems/throw_drop/projectile_flight_t17.h"
#include "vrik/native_supply_t2.h"
#include "core/native_grasp13.h"
#include "core/snapshot_mailbox.h"
#include "old_policy_generated.hpp"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <limits>
#define EXCEPTION_EXECUTE_HANDLER 1
namespace flight17=kcd2vr::throw_projectile_t17;
namespace ng13=kcd2vr::grasp13;
namespace st2=kcd2vr::supply_t2;
namespace core=kcd2vr::core;
namespace inventory_lifecycle_t8 {static std::atomic<std::uint64_t> loadEpisode18{1};}
namespace native_script_t2 {struct Context {std::uint64_t now=0;bool valid=true;bool Current()const noexcept{return valid;}};}
template<class Policy>struct FlightRecord {
 std::uintptr_t arrow=0,entity=0;std::uint32_t id=0;
 std::uint64_t key=0,serial=0,load=0,tick=0,born=0,ageMs=0;int kind=0;
 Policy policy{};float slot[12]{};bool slotKnown=false;
};
struct ContactIdentity {std::uintptr_t arrow=0;std::uint32_t entity=0;std::uint64_t key=0,serial=0;};
static unsigned checks=0,identityCalls=0,statusCalls=0,velocityCalls=0,slotWrites=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL line%d %s\n",__LINE__,#x);std::abort();}}while(0)
alignas(16)static unsigned char entity[160]{},arrow[160]{};
static int body=1,otherBody=2;
static bool identityKnown=true,bodyKnown=true,poseKnown=true,statusKnown=true;
static bool contactAtStatus=false,replaceAtStatus=false,invalidateAtStatus=false;
static std::atomic<std::uint64_t>* activeContact=nullptr;
static std::uint64_t hostNow=2000;
static void* manager=reinterpret_cast<void*>(0x1000);static void* world=reinterpret_cast<void*>(0x2000);
static float measured[3]{24,0,-3},applied[3]{};
struct DropDynamicsT1 {float v[3]{};};
static std::uint64_t GetTickCount64(){return hostNow;}
static bool AmmoTransientScopeT20(const native_script_t2::Context& ctx){return ctx.Current();}
static void* AmmoTransientItemManagerT20(){return manager;}
static void* WorldInventoryT1(){return world;}
static bool Vft13(void* p,std::uintptr_t v){return v==0x3B0C698?bodyKnown&&p==&body:v==0x3A83DC0&&p==world;}
template<class T>static T Rd(void* p,std::size_t o,T fallback){if(p!=arrow||o!=0x70)return fallback;T out{};std::memcpy(&out,arrow+o,sizeof(out));return out;}
static bool SehCopy(const void* p,void* out,std::size_t n){if(!poseKnown)return false;std::memcpy(out,p,n);return true;}
static bool AmmoSlotT18(void*,const float*){++slotWrites;return true;}
static void AmmoNoteT18(const char*,int,std::uint64_t,std::uint64_t,const char*){}
static int __fastcall Status(void*,void* out){++statusCalls;
 if(contactAtStatus&&activeContact)activeContact->store(1);
 if(replaceAtStatus){void* p=&otherBody;std::memcpy(arrow+0x70,&p,sizeof(p));}
 if(invalidateAtStatus)identityKnown=false;
 if(!statusKnown)return 0;std::memcpy(static_cast<DropDynamicsT1*>(out)->v,measured,sizeof(measured));return 1;
}
static void __fastcall Velocity(void*,void*,const float* v,float,void*,int){++velocityCalls;std::memcpy(applied,v,sizeof(applied));}
template<class T>static T Native13(std::uintptr_t rva){return reinterpret_cast<T>(rva==0xD85828?reinterpret_cast<void*>(&Status):reinterpret_cast<void*>(&Velocity));}
template<class F>static bool Identity(const F&,void*& a,void*& e){++identityCalls;a=arrow;e=entity;return identityKnown;}
namespace old_tick {
 using AmmoFlightT18=FlightRecord<old_policy::Flight>;
 namespace flight17=old_policy;
 namespace ac18 {static std::atomic<unsigned> flights{0};}
 static AmmoFlightT18 g_ammo_flight18[16];
 static core::SnapshotMailbox<ContactIdentity> g_ammo_contact_id18[16];
 static std::atomic<std::uint64_t> g_ammo_contact18[16]{};
 static bool AmmoFlightIdentityT18(const AmmoFlightT18& f,void*& a,void*& e){return Identity(f,a,e);}
 #include "old_tick.inc"
}
namespace current {
 using AmmoFlightT18=FlightRecord<flight17::Flight>;
 namespace ac18 {static std::atomic<unsigned> flights{0};}
 static AmmoFlightT18 g_ammo_flight18[16];
 static core::SnapshotMailbox<ContactIdentity> g_ammo_contact_id18[16];
 static std::atomic<std::uint64_t> g_ammo_contact18[16]{};
 static bool AmmoFlightIdentityT18(const AmmoFlightT18& f,void*& a,void*& e){return Identity(f,a,e);}
 #include "new_tick.inc"
 struct AmmoFlightScopeT22 {std::uint64_t serial=0;void* manager=nullptr;void* world=nullptr;bool retired=false;};
 static AmmoFlightScopeT22 g_ammo_flight_scope22[16];
 #include "service_tick.inc"
}
static void resetNative(){identityCalls=statusCalls=velocityCalls=slotWrites=0;identityKnown=bodyKnown=poseKnown=statusKnown=true;
 contactAtStatus=replaceAtStatus=invalidateAtStatus=false;hostNow=2000;inventory_lifecycle_t8::loadEpisode18.store(1);
 measured[0]=24;measured[1]=0;measured[2]=-3;
 void* p=&body;std::memcpy(arrow+0x70,&p,sizeof(p));std::memset(applied,0,sizeof(applied));}
static void position(float x){const float matrix[12]{1,0,0,x,0,1,0,0,0,0,1,2};std::memcpy(entity+0x58,matrix,sizeof(matrix));}
template<class F>static void seed(F& f){f={};f.arrow=reinterpret_cast<std::uintptr_t>(arrow);f.entity=reinterpret_cast<std::uintptr_t>(entity);f.id=7;f.key=8;f.serial=1;f.load=1;f.born=1000;f.tick=2000;f.policy.begin({0,0,2});}
static void setup(){resetNative();for(auto& f:current::g_ammo_flight18)f={};seed(current::g_ammo_flight18[0]);current::g_ammo_contact18[0].store(0);current::ac18::flights.store(1);activeContact=&current::g_ammo_contact18[0];position(21);}
static void tick(std::uint64_t t,bool allowed=true){hostNow=t;current::AmmoTickFlightsT18(t,allowed);}
int main(){
 old_policy::Flight old{};flight17::Flight fixed{};flight17::Vec3 v{};old_policy::Vec3 ov{};
 CHECK(old.begin({0,0,0})&&fixed.begin({0,0,0}));CHECK(old.observe({19.8f,0,0})&&fixed.observe({19.8f,0,0}));
 CHECK(!fixed.tailVelocity({24,0,-3},.05f,v));CHECK(old.observe({21,0,0})&&fixed.observe({21,0,0}));
 CHECK(old.tailVelocity({24,0,-3},.05f,ov)&&fixed.tailVelocity({24,0,-3},.05f,v));
 CHECK(ov.x>16.08f&&ov.x<16.09f&&v.x>20.31f&&v.x<20.32f&&v.z==-3); // frozen endpoint RED / integrated crossing GREEN
 CHECK(!fixed.tailVelocity({24,0,-3},.06f,v));
 fixed.breakInterval();CHECK(fixed.observe({22,0,0})&&!fixed.tailVelocity({24,0,-3},.02f,v));
 CHECK(fixed.observe({22.1f,0,0})&&fixed.tailVelocity({24,0,3},.02f,v)&&v.x<24&&v.z>0&&v.z<3);
 CHECK(fixed.observe({18,0,0})&&fixed.furthestHorizontal==22.1f); // rebound never grants a fresh20m

 setup();auto& f=current::g_ammo_flight18[0];f.policy.observe({19.8f,0,2});tick(2050);CHECK(velocityCalls==1&&applied[0]>20.31f&&applied[0]<20.32f&&applied[2]==-3&&slotWrites==0);
 // The frozen full old Tick never accumulates age at recurring60ms updates.
 resetNative();for(auto& x:old_tick::g_ammo_flight18)x={};seed(old_tick::g_ammo_flight18[0]);old_tick::g_ammo_flight18[0].tick=1000;old_tick::ac18::flights.store(1);old_tick::g_ammo_contact18[0].store(0);
 setup();current::g_ammo_flight18[0].tick=1000;
 for(std::uint64_t t=1060;t<=16000;t+=60){hostNow=t;old_tick::AmmoTickFlightsT18(t,false);current::AmmoTickFlightsT18(t,false);}
 CHECK(old_tick::g_ammo_flight18[0].serial&&current::g_ammo_flight18[0].serial);identityCalls=0;tick(16001,false);
 CHECK(!current::g_ammo_flight18[0].serial&&identityCalls==0&&velocityCalls==0);old_tick::AmmoTickFlightsT18(16001,false);CHECK(old_tick::g_ammo_flight18[0].serial); // frozen lifetime RED
 setup();tick(999);CHECK(!current::g_ammo_flight18[0].serial&&identityCalls==0); // future born before historical lookup
 setup();inventory_lifecycle_t8::loadEpisode18.store(2);tick(2010);CHECK(!current::g_ammo_flight18[0].serial&&identityCalls==0);

 for(int gap=0;gap<6;++gap){setup();current::g_ammo_flight18[0].policy.observe({19.8f,0,2});
  if(gap==0)tick(2100); // >50ms gap
  if(gap==1)tick(2020,false);
  if(gap==2){poseKnown=false;tick(2020);}
  if(gap==3){bodyKnown=false;tick(2020);}
  if(gap==4){statusKnown=false;tick(2020);}
  if(gap==5){measured[0]=std::numeric_limits<float>::quiet_NaN();tick(2020);}
  CHECK(velocityCalls==0);poseKnown=bodyKnown=statusKnown=true;measured[0]=24;position(21.2f);
  const auto next=current::g_ammo_flight18[0].tick+20;tick(next);CHECK(velocityCalls==0); // positive rebaseline only
  position(21.4f);tick(next+20);CHECK(velocityCalls==1&&applied[2]==-3);CHECK(current::g_ammo_flight18[0].policy.origin.x==0);
 }
 setup();contactAtStatus=true;tick(2020);CHECK(velocityCalls==0);tick(2040);CHECK(!current::g_ammo_flight18[0].serial&&slotWrites==0);
 setup();replaceAtStatus=true;tick(2020);CHECK(velocityCalls==0);
 setup();invalidateAtStatus=true;tick(2020);CHECK(velocityCalls==0);
 setup();identityKnown=false;tick(2020);CHECK(!current::g_ammo_flight18[0].serial&&velocityCalls==0);
 // Current T22 manager/world/load guards retire locally before old Arrow reads.
 for(int change=0;change<3;++change){setup();current::g_ammo_flight_scope22[0]={1,manager,world,false};
  if(change==0)current::g_ammo_flight_scope22[0].manager=reinterpret_cast<void*>(0x9990);
  if(change==1)current::g_ammo_flight_scope22[0].world=reinterpret_cast<void*>(0x9990);
  if(change==2)inventory_lifecycle_t8::loadEpisode18.store(2);
  current::g_ammo_flight18[0].arrow=1;native_script_t2::Context ctx{2020,true};st2::Input input{};input.allowed=true;input.tick=2020;hostNow=2020;
  current::AmmoServiceTickFlightsT22(ctx,input);CHECK(!current::g_ammo_flight18[0].serial&&identityCalls==0&&velocityCalls==0&&slotWrites==0);
 }
 std::printf("{\"checks\":%u,\"passed\":true,\"frozen_endpoint_red\":true,\"frozen_wall_ttl_red\":true,\"native_execution\":false}\n",checks);
}
