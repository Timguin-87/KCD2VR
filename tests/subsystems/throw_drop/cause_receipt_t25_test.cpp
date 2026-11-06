#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef near
#undef far
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "core/snapshot_mailbox.h"
#include "core/native_grasp13.h"
#include "vrik/native_supply_t2.h"
#include "subsystems/throw_drop/projectile_flight_t17.h"
namespace ng13=kcd2vr::grasp13;namespace st2=kcd2vr::supply_t2;namespace flight17=kcd2vr::throw_projectile_t17;
// Explicit mailbox busy/read-failure leaf; normal operations use the REAL mailbox.
namespace core {template<class T>struct SnapshotMailbox {kcd2vr::core::SnapshotMailbox<T> actual;bool busy=false,unknown=false;unsigned writes=0;
 bool publish(const T& x)noexcept{++writes;return !busy&&actual.publish(x);}bool read(T& x)const noexcept{return !unknown&&actual.read(x);}};}
namespace inventory_lifecycle_t8 {static std::atomic<std::uint64_t> loadEpisode18{1};}
namespace native_script_t2 {struct Context {std::uint64_t now=2000;bool valid=true;bool Current()const noexcept{return valid;}};}
namespace ac18 {static std::atomic<unsigned> flights{0};}
static unsigned checks=0,originalCalls=0,identityCalls=0,causeReads=0,payloadReads=0,oldReads=0,nativeCalls=0,damageCalls=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL line%d %s\n",__LINE__,#x);std::abort();}}while(0)
alignas(16)static unsigned char arrow[512]{},otherArrow[512]{},payload[160]{},entity[192]{};
static int managerObject=0,worldObject=0,foreignManager=0,foreignWorld=0,body=0;
static bool worldChanged=false,identityKnown=true,changeDuringIdentity=false,forbidOld=false,failActor=false,failSource=false,failPayload=false;
static bool originalValue=true,reenterOriginal=false,originalFault=false;
static void* originalArrow=nullptr;static void* originalEvent=nullptr;
static std::vector<std::string> notes;static std::uint64_t now=2000;
static bool SehCopy(const void* p,void* out,std::size_t size)noexcept{
 auto a=reinterpret_cast<std::uintptr_t>(p),b=reinterpret_cast<std::uintptr_t>(arrow);
 if(forbidOld&&a>=b&&a<b+sizeof(arrow)){++oldReads;return false;}
 if(p==arrow+0x108){++causeReads;if(failActor)return false;}
 if(p==arrow+0x110){++causeReads;if(failSource)return false;}
 if(p==arrow+0x120||p==payload+0x30){++payloadReads;if(failPayload)return false;}
 if(!p)return false;std::memcpy(out,p,size);return true;
}
template<class T>static T Rd(void* p,std::size_t off,T fallback){T v{};return p&&SehCopy(static_cast<unsigned char*>(p)+off,&v,sizeof(v))?v:fallback;}
static void* Entity13(unsigned id){++nativeCalls;return id==7?entity:nullptr;}
static void* Item13(std::uint64_t key){++nativeCalls;return key==8?payload:nullptr;}
static bool AmmoEntityT18(void* a,void* e,unsigned id){return a==arrow&&e==entity&&id==7;}
static bool Vft13(void* p,std::uintptr_t value){return value==0x3A83DC0&&(p==&worldObject||p==&foreignWorld);}
static bool AmmoTransientScopeT20(const native_script_t2::Context& c){return c.Current();}
static void* AmmoTransientItemManagerT20(){return worldChanged?&foreignManager:&managerObject;}
static void* WorldInventoryT1(){return worldChanged?&foreignWorld:&worldObject;}
static bool AmmoSlotT18(void*,const float*){++nativeCalls;return true;}
static void AmmoNoteT18(const char* stage,int,std::uint64_t,std::uint64_t,const char* detail){notes.emplace_back(std::string(stage)+" "+detail);}
struct DropDynamicsT1{float v[3]{};};
static const float* __fastcall Slot(void*,int,bool){++nativeCalls;return nullptr;}
static int __fastcall Status(void*,void*){++nativeCalls;return 0;}
static void __fastcall Velocity(void*,void*,const float*,float,void*,int){++nativeCalls;}
template<class T>static T Native13(std::uintptr_t rva){return reinterpret_cast<T>(rva==0x421528?reinterpret_cast<void*>(&Slot):rva==0xD85828?reinterpret_cast<void*>(&Status):reinterpret_cast<void*>(&Velocity));}
struct AmmoCommandT18{st2::Input owner{};std::uint64_t serial=0,ammo=0,launcher=0,reserved=0;int side=-1,kind=-1;};
struct AmmoHandT18{AmmoCommandT18 command{};struct{ng13::Pose last{};}motion;std::uint64_t load=1;};
struct AmmoLaunchResultT17{void* projectile=nullptr;unsigned entity=0;std::uint64_t payloadKey=0;};
#include "types.inc"
static bool __fastcall Original(void* a,void* e);
#include "capture.inc"
#include "observer.inc"
static bool __fastcall Original(void* a,void* e){++originalCalls;originalArrow=a;originalEvent=e;
 if(reenterOriginal){reenterOriginal=false;CHECK(ArrowContactT18(a,e)==originalValue);}
 if(originalFault)RaiseException(0xE0250001,0,0,nullptr);return originalValue;}
#include "track.inc"
static bool AmmoFlightIdentityT18(const AmmoFlightT18& f,void*& a,void*& e){++identityCalls;
 if(forbidOld){++oldReads;return false;}a=reinterpret_cast<void*>(f.arrow);e=reinterpret_cast<void*>(f.entity);
 const bool result=identityKnown&&a==arrow&&e==entity&&f.id==7&&f.key==8;
 if(changeDuringIdentity)worldChanged=true;return result;}
static thread_local const native_script_t2::Context* g_ammo_context22=nullptr;
#include "scope.inc"
#include "drain.inc"
#include "tick.inc"
#include "service_tick.inc"
template<class T>static void put(unsigned char* at,const T& v){std::memcpy(at,&v,sizeof(v));}
static native_script_t2::Context ctx;
static void reset(){worldChanged=changeDuringIdentity=forbidOld=failActor=failSource=failPayload=false;identityKnown=true;
 originalValue=true;reenterOriginal=originalFault=false;notes.clear();ctx={2000,true};now=2000;g_ammo_context22=&ctx;
 originalCalls=identityCalls=causeReads=payloadReads=oldReads=nativeCalls=damageCalls=0;g_arrow_contact_original18=&Original;
 inventory_lifecycle_t8::loadEpisode18=1;ac18::flights=0;
 for(int n=0;n<16;++n){g_ammo_flight18[n]={};g_ammo_flight_scope22[n]={};g_ammo_contact18[n]=0;g_ammo_cause_attempt25[n]=0;
  g_ammo_contact_id18[n].busy=g_ammo_contact_id18[n].unknown=false;(void)g_ammo_contact_id18[n].publish({});
  g_ammo_cause_contact25[n].busy=g_ammo_cause_contact25[n].unknown=false;(void)g_ammo_cause_contact25[n].publish({});g_ammo_cause_contact25[n].writes=0;}
 std::memset(arrow,0,sizeof(arrow));std::memset(payload,0,sizeof(payload));unsigned id=7,actor=42;std::uint64_t key=8,source=99;void* p=payload;
 put(arrow+0x30,id);put(arrow+0x108,actor);put(arrow+0x110,source);put(arrow+0x120,p);put(payload+0x30,key);
 AmmoHandT18 h{};h.command.serial=101;h.command.launcher=99;h.command.owner.entity=42;h.load=1;
 AmmoLaunchResultT17 r{arrow,7,8};AmmoTrackFlightT18(r,h,now);g_ammo_flight_scope22[0]={101,&managerObject,&worldObject,false,42,99,false};
 causeReads=payloadReads=nativeCalls=0;
}
static unsigned causeNotes(){unsigned n=0;for(const auto& x:notes)if(x.starts_with("CAUSE_ENTRY_RECEIPT"))++n;return n;}
static void drain(){st2::Input in{};in.allowed=false;in.tick=now;AmmoServiceTickFlightsT22(ctx,in);}
static int callFault(){__try{ArrowContactT18(arrow,reinterpret_cast<void*>(2));return 0;}__except(EXCEPTION_EXECUTE_HANDLER){return 1;}}
int main(){
 for(bool value:{false,true}){reset();originalValue=value;void* event=reinterpret_cast<void*>(3);
  CHECK(ArrowContactT18(arrow,event)==value);CHECK(originalCalls==1&&originalArrow==arrow&&originalEvent==event);CHECK(notes.empty()&&damageCalls==0&&nativeCalls==0);
  CHECK(causeReads==2&&payloadReads==2&&g_ammo_cause_contact25[0].writes==2); // registration reset + occurrence
  now+=60;ctx.now=now;drain();CHECK(causeNotes()==1&&notes[0].find("actor=MATCH")!=std::string::npos&&notes[0].find("source=MATCH")!=std::string::npos&&notes[0].find("nativeOutcome=UNOBSERVED")!=std::string::npos);
  CHECK(!g_ammo_flight18[0].serial&&ac18::flights==0&&damageCalls==0);drain();CHECK(causeNotes()==1);
 }
 for(int mismatch=0;mismatch<3;++mismatch){reset();if(mismatch==0){unsigned id=9;put(arrow+0x30,id);}if(mismatch==1){std::uint64_t key=9;put(payload+0x30,key);}
  void* receiver=mismatch==2?otherArrow:arrow;CHECK(ArrowContactT18(receiver,nullptr));CHECK(originalCalls==1&&originalArrow==receiver&&causeReads==0&&notes.empty());
  AmmoCauseReceiptT25 receipt{};CHECK(g_ammo_cause_contact25[0].read(receipt)&&!receipt.serial);
 }
 for(int variant=0;variant<5;++variant){reset();if(variant==0){unsigned a=0;std::uint64_t s=0;put(arrow+0x108,a);put(arrow+0x110,s);}
  if(variant==1){unsigned a=77;put(arrow+0x108,a);}if(variant==2){std::uint64_t s=55;put(arrow+0x110,s);}if(variant==3)failActor=true;if(variant==4)failSource=true;
  CHECK(ArrowContactT18(arrow,nullptr));drain();CHECK(causeNotes()==1&&originalCalls==1);
  CHECK(notes[0].find(variant==0?"actor=ZERO":variant==1?"actor=MISMATCH":variant==2?"source=MISMATCH":variant==3?"actor=UNKNOWN":"source=UNKNOWN")!=std::string::npos);
 }
 reset();g_ammo_cause_contact25[0].busy=true;CHECK(ArrowContactT18(arrow,nullptr));auto reads=causeReads;
 CHECK(ArrowContactT18(arrow,nullptr));CHECK(originalCalls==2&&causeReads==reads);g_ammo_cause_contact25[0].busy=false;drain();CHECK(causeNotes()==0&&!g_ammo_flight18[0].serial); // missed diagnostic never holds flight
 reset();failPayload=true;CHECK(ArrowContactT18(arrow,nullptr));failPayload=false;CHECK(ArrowContactT18(arrow,nullptr));CHECK(causeReads==0&&originalCalls==2);drain();CHECK(causeNotes()==0&&!g_ammo_flight18[0].serial);
 reset();reenterOriginal=true;CHECK(ArrowContactT18(arrow,nullptr));CHECK(originalCalls==2&&causeReads==2);drain();CHECK(causeNotes()==1);
 reset();originalFault=true;CHECK(callFault()==1&&originalCalls==1&&notes.empty());originalFault=false;drain();CHECK(causeNotes()==1&&notes[0].find("nativeOutcome=UNOBSERVED")!=std::string::npos);
 for(int change=0;change<5;++change){reset();CHECK(ArrowContactT18(arrow,nullptr));
  if(change==0)worldChanged=true;if(change==1)g_ammo_flight_scope22[0].manager=&foreignManager;if(change==2)g_ammo_flight_scope22[0].world=&foreignWorld;
  if(change==3)inventory_lifecycle_t8::loadEpisode18=2;if(change==4)ctx.valid=false;
  forbidOld=true;drain();CHECK(causeNotes()==0&&identityCalls==0&&oldReads==0&&damageCalls==0);
 }
 reset();changeDuringIdentity=true;CHECK(ArrowContactT18(arrow,nullptr));drain();CHECK(causeNotes()==0&&identityCalls==1); // identity lookup reentry: pair rechecked before diagnostic
 for(int field=0;field<4;++field){reset();CHECK(ArrowContactT18(arrow,nullptr));AmmoCauseReceiptT25 receipt{};CHECK(g_ammo_cause_contact25[0].read(receipt));
  if(field==0)++receipt.serial;if(field==1)++receipt.key;if(field==2)++receipt.entity;if(field==3)++receipt.arrow;
  (void)g_ammo_cause_contact25[0].publish(receipt);drain();CHECK(causeNotes()==0&&!g_ammo_flight18[0].serial);
 }
 reset();CHECK(ArrowContactT18(arrow,nullptr));g_ammo_cause_contact25[0].unknown=true;drain();CHECK(causeNotes()==0&&!g_ammo_flight18[0].serial);
 reset();CHECK(ArrowContactT18(arrow,nullptr));g_ammo_context22=nullptr;drain();CHECK(causeNotes()==0&&!g_ammo_flight18[0].serial);
 reset();for(int n=0;n<100;++n)CHECK(ArrowContactT18(arrow,nullptr));CHECK(causeReads==2&&originalCalls==100&&g_ammo_cause_contact25[0].writes==2);drain();CHECK(causeNotes()==1&&nativeCalls==0&&damageCalls==0);
 reset();(void)g_ammo_contact_id18[0].publish({});CHECK(ArrowContactT18(arrow,nullptr));CHECK(originalCalls==1&&causeReads==0);drain();CHECK(causeNotes()==0); // early/native contact not registered: absence is not a hit refusal
 std::printf("{\"checks\":%u,\"actual_observer_tick_drain\":true,\"native_execution\":false,\"nativeOutcome\":\"UNOBSERVED\"}\n",checks);
}
