#include <atomic>
#include <array>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "vrik/npc_contact172.h"
#include "vrik/contact_authority176.h"
#include "vrik/contact_receipt58c1.h"
#include "diagnostics/capture168.h"
#include "core/collab_test_h21.h"
static std::uintptr_t g_base=0x180000000;static std::uint64_t fixtureNow=1000;
static constexpr std::uintptr_t kEntityVft=0x4072C78;
static std::uint64_t GetTickCount64(){return fixtureNow;}
static std::map<std::uintptr_t,std::vector<unsigned char>> mem;
static bool SehCopy146(const void* p,void* o,std::size_t n){auto it=mem.find((std::uintptr_t)p);if(it==mem.end()||it->second.size()!=n)return false;std::memcpy(o,it->second.data(),n);return true;}
template<class T>static T Rd(const void* p,std::uintptr_t off,T fallback){T v{};return SehCopy146((const void*)((std::uintptr_t)p+off),&v,sizeof(v))?v:fallback;}
template<class T>static void put(std::uintptr_t p,T v){auto& b=mem[p];b.resize(sizeof(v));std::memcpy(b.data(),&v,sizeof(v));}
namespace diagnostics=kcd2vr::diagnostics;
static std::vector<std::string> receipts;
namespace diagnostic_trace {static void Write(const char*s){receipts.emplace_back(s);}}
static bool owner=true,contract=true;static unsigned ownerReads=0,entityReads=0,nativeCalls=0;
static std::function<void()> ownerHook,entityHook;
namespace frik_pose_probe {
static bool currentMainH7=false,statusFaultH7=false;static unsigned currentCallsH7=0;static std::function<void()> queryHookH7;
static bool GeometryCorpseBodyH7(std::uint32_t,std::uintptr_t,std::uintptr_t,int rawPart,bool dead)noexcept{++currentCallsH7;if(queryHookH7)queryHookH7();return currentMainH7&&rawPart>=0&&dead&&!statusFaultH7;}
static bool GeometryCorpseContactFaultH7()noexcept{return statusFaultH7;}
static bool GeometryOwnerValid178r30(std::uint64_t k,std::uint64_t g,std::uint64_t e,std::uint64_t q){++ownerReads;if(ownerHook)ownerHook();return owner&&k==11&&g==12&&e==13&&q==14;}}
static std::map<std::uint32_t,std::uintptr_t> entities;
struct Req;static void* EntityById(std::uint32_t id){++entityReads;if(entityHook)entityHook();return (void*)entities[id];}
static bool Contract19c4r4(){return contract;}
static bool NativeImpulse19c4r4(std::uintptr_t,const Req&,const float*,bool,const char*&)noexcept;
static int colliderImpulses37c4=0;static bool colliderFault57=false,colliderRefused57=false;static std::uintptr_t lastCollider57=0;static int lastPart57=-1;
static bool NativeColliderImpulse37c4(std::uintptr_t collider,void* action,const char*& why)noexcept{
 ++colliderImpulses37c4;lastCollider57=collider;std::memcpy(&lastPart57,static_cast<unsigned char*>(action)+40,sizeof(lastPart57));
 if(colliderFault57){why="COLLIDER_ACTION_EXCEPTION";return false;}return !colliderRefused57;} // exact caught-exception result, native leaf mocked
#include "generated_impulse5916c3.inc"
static std::array<float,3> lastImpulse{};static bool lastPoint=true,nativeFails=false;
static std::function<void()> nativeHook;
static bool NativeImpulse19c4r4(std::uintptr_t,const Req&,const float* p,bool point,const char*&why)noexcept{
 ++nativeCalls;lastPoint=point;for(int i=0;i<3;++i)lastImpulse[i]=p[i];if(nativeHook)nativeHook();if(nativeFails){why="ADDIMPULSE_EXCEPTION";return false;}return true;
}
static unsigned checks=0;static void ck(bool b,const char*s){++checks;if(!b){std::fprintf(stderr,"FAIL %s\n",s);std::exit(1);}}
constexpr std::uintptr_t ent=0x1000,fixtureHead=0x2000,fixtureNode=0x3000,proxy=0x4000,body=0x5000;
static kcd2vr::contact58c1::Receipt deathReceipt56(){kcd2vr::contact58c1::Receipt r{};r.serial=51;r.tick=fixtureNow;r.target=2;r.targetIdentity=ent;r.epoch=13;r.seen=r.accepted=r.applied=r.fields=r.bodyContact=true;r.death=1;return r;}
static void death56(){NoteNativeDeath56(deathReceipt56(),13,kcd2vr::contact176::recoveryEpoch55c1.load(),fixtureNow);}
static void setup(bool observedDeath=false){kcd2vr::collab_h21::setPhysicsMode(3);mem.clear();entities={{2,ent}};put(ent,g_base+kEntityVft);put(ent+0xC,std::uint32_t(2));put(ent+0xa8,fixtureHead);put(fixtureHead+8,fixtureNode);put(fixtureNode+0x19,(unsigned char)0);put(fixtureNode+0x20,1u);put(fixtureNode+0x28,proxy);
 put(proxy,g_base+kPhysicalProxyVft5916c3);put(proxy+0x30,ent);put(proxy+0x38,body);put(g_base+kPhysicalProxyVft5916c3+0xd0,g_base+kProxyImpulse5916c3);put(ent+0x98,std::uintptr_t(0));put(body,g_base+kArticulatedVft19c4r4);put(g_base+kArticulatedVft19c4r4+8,g_base+0x1A72E50);put(g_base+kArticulatedVft19c4r4+0x30,g_base+0x8A9348);put(g_base+kArticulatedVft19c4r4+0x38,g_base+0xA99DF0);
 frik_pose_probe::currentMainH7=frik_pose_probe::statusFaultH7=false;frik_pose_probe::currentCallsH7=0;frik_pose_probe::queryHookH7={};
 owner=contract=true;ownerReads=entityReads=nativeCalls=0;ownerHook=entityHook=nativeHook={};g_fault=false;g_enabled=true;g_observe=false;g_scale=1;g_mass=1.6f;g_cap=40;g_session=1;g_sent=g_dropped=0;g_count=0;g_budget19c4r4={};fixtureNow=1000;nativeFails=false;colliderImpulses37c4=0;colliderFault57=colliderRefused57=false;lastCollider57=0;lastPart57=-1;g_deaths56={};g_death_next56=0;if(observedDeath)death56();receipts.clear();diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,false,fixtureNow);
}
static void request(std::uint64_t tick=0,int type=3,float speed=2,float n=1){float p[3]{1,2,3},normal[3]{0,0,n};Request(2,3,type,p,normal,speed,tick?tick:fixtureNow,body,11,12,13,14);}
static void direct57(){put(proxy+0x38,body+1);put(body+0x28,std::int16_t(1));put(body+0x20,ent);put(g_base+kArticulatedVft19c4r4+0x50,g_base+0x5D6F44);}
static void secondRecipient57(){constexpr std::uintptr_t e=0x10000,h=0x11000,n=0x12000,p=0x13000,b=0x14000;
 entities[3]=e;put(e,g_base+kEntityVft);put(e+0xC,std::uint32_t(3));put(e+0x98,std::uintptr_t(0));put(e+0xA8,h);put(h+8,n);put(n+0x19,std::uint8_t(0));put(n+0x20,1u);put(n+0x28,p);
 put(p,g_base+kPhysicalProxyVft5916c3);put(p+0x30,e);put(p+0x38,b);put(b,g_base+kArticulatedVft19c4r4);
 auto d=deathReceipt56();d.serial=52;d.target=3;d.targetIdentity=e;NoteNativeDeath56(d,13,kcd2vr::contact176::recoveryEpoch55c1.load(),fixtureNow);
}
static void requestSecond57(){const float p[3]{1,2,3},normal[3]{0,0,1};Request(3,7,3,p,normal,2.f,fixtureNow,0x14000,11,12,13,14);}
int main(){
 for(int mode:{0,1,2}){setup(true);request();kcd2vr::collab_h21::setPhysicsMode(mode);Drain(fixtureNow);ck(!nativeCalls&&!entityReads,"NPC impulse requires F9 third mode even with pending enabled request");}
 setup(true);request();kcd2vr::collab_h21::setPhysicsMode(0);kcd2vr::collab_h21::setPhysicsMode(3);Drain(fixtureNow);ck(!nativeCalls&&!entityReads,"OFF/ON never retags an old NPC impulse to the new F9 profile");
 // Independent-review reproductions execute the same extracted production routes.
 unsigned parentCalls57[2]{};
 for(int route=0;route<2;++route){setup(true);if(route)direct57();put(ent+0x98,std::uintptr_t(0x6000));put(0x6018,std::uintptr_t(0x7000));request();Drain(fixtureNow);parentCalls57[route]=nativeCalls+colliderImpulses37c4;}
 setup(true);direct57();colliderFault57=true;request();Drain(fixtureNow);const bool directLatched57=g_fault.load();
 const unsigned firstDirect57=colliderImpulses37c4;fixtureNow+=600;request();Drain(fixtureNow);const bool directRetried57=colliderImpulses37c4!=firstDirect57;
 setup(true);secondRecipient57();request();requestSecond57();Drain(fixtureNow);const unsigned positiveTwo57=nativeCalls;
 setup(true);secondRecipient57();nativeFails=true;request();requestSecond57();nativeHook=[]{requestSecond57();};Drain(fixtureNow);
 const unsigned proxyFaultCalls57=nativeCalls;const bool proxyLatched57=g_fault.load();const auto proxyPending57=g_count;
 setup(true);direct57();secondRecipient57();colliderFault57=true;request();requestSecond57();Drain(fixtureNow);
 const unsigned directBatchCalls57=nativeCalls+colliderImpulses37c4;const bool directBatchLatched57=g_fault.load();
 std::printf("C57R1 REPRO parentCalls(same/replaced)=%u/%u directLatched=%d retried=%d positiveTwo=%u proxyFaultCalls=%u proxyLatched=%d pending=%u directBatchCalls=%u directBatchLatched=%d\n",
  parentCalls57[0],parentCalls57[1],directLatched57,directRetried57,positiveTwo57,proxyFaultCalls57,proxyLatched57,proxyPending57,directBatchCalls57,directBatchLatched57);std::fflush(stdout);
 ck(!parentCalls57[0]&&!parentCalls57[1],"C57R1 identical parent ownership refuses both same-body and body-replaced routes");
 ck(directLatched57&&!directRetried57,"C57R1 exact direct native exception latches and never retries after cooldown");
 ck(positiveTwo57==2,"C57R1 batch-stop comparison has two independently eligible recipients");
 ck(proxyLatched57&&proxyFaultCalls57==1&&!proxyPending57,"C57R1 proxy exception stops remaining eligible batch and purges queued callback work");
 ck(directBatchLatched57&&directBatchCalls57==1,"C57R1 direct exception stops remaining independently eligible recipient");
 setup(true);direct57();request();ownerHook=[]{if(ownerReads==2){put(ent+0x98,std::uintptr_t(0x6000));put(0x6018,std::uintptr_t(0x7000));}};Drain(fixtureNow);
 ck(!nativeCalls&&!colliderImpulses37c4,"C57R1 parent transfer during final owner callback refuses direct native leaf");
 for(int field=0;field<2;++field){setup(true);direct57();request();ownerHook=[field](){if(ownerReads==2){if(field)put(ent+0xC,std::uint32_t(99));else put(ent,g_base+kEntityVft+8);}};Drain(fixtureNow);
  ck(!nativeCalls&&!colliderImpulses37c4,"C57R1 changed current entity identity cannot pass direct fallback after initial lookup");}
 setup(true);direct57();mem.erase(ent+0x98);request();Drain(fixtureNow);ck(!nativeCalls&&!colliderImpulses37c4,"C57R1 unreadable parent data is not an unparented direct recipient");
 setup(true);direct57();colliderRefused57=true;request();Drain(fixtureNow);ck(colliderImpulses37c4==1&&!g_fault,"C57R1 ordinary native Action refusal is not mislabeled as a caught native fault");
 setup(true);direct57();request();Drain(fixtureNow);ck(colliderImpulses37c4==1&&lastCollider57==body&&lastPart57==3,"C57R1 valid unparented direct route preserves exact collider and raw part");
 // Actual native-death receipt admission. Legacy identity cases below explicitly
 // start with that receipt; an articulated collider alone grants no impulse.
 setup();request();Drain(fixtureNow);ck(!nativeCalls&&!colliderImpulses37c4,"C56 living/unknown articulated target has no native death proof");
 setup();death56();request();Drain(fixtureNow);ck(nativeCalls==1,"C56 same exact native lethal receipt admits existing corpse impulse");
 for(int invalid=0;invalid<8;++invalid){setup();auto d=deathReceipt56();auto epoch=std::uint64_t(13),recovery=kcd2vr::contact176::recoveryEpoch55c1.load();
  if(invalid==0)d.accepted=false;if(invalid==1)d.applied=false;if(invalid==2)d.fields=false;if(invalid==3)d.bodyContact=false;
  if(invalid==4)d.death=0;if(invalid==5)d.tick=fixtureNow+1;if(invalid==6)epoch=14;if(invalid==7)++recovery;
  NoteNativeDeath56(d,epoch,recovery,fixtureNow);request();Drain(fixtureNow);ck(!nativeCalls,"C56 unaccepted/nonlethal/mixed lifetime/future receipt cannot create death proof");}
 setup();auto d56=deathReceipt56();d56.target=3;NoteNativeDeath56(d56,13,kcd2vr::contact176::recoveryEpoch55c1.load(),fixtureNow);request();Drain(fixtureNow);ck(!nativeCalls,"C56 death of another entity is not death of this collider");
 setup(true);request();entities[2]=ent+0x100;Drain(fixtureNow);ck(!nativeCalls,"C56 replacement entity pointer cannot borrow old death proof");
 setup();d56=deathReceipt56();d56.epoch=14;NoteNativeDeath56(d56,14,kcd2vr::contact176::recoveryEpoch55c1.load(),fixtureNow);request();Drain(fixtureNow);ck(!nativeCalls,"C56 different body epoch cannot donate native death");
 setup(true);++g_deaths56[0].recovery;request();Drain(fixtureNow);ck(!nativeCalls,"C56 different recovery lifetime cannot donate native death");
 setup(true);fixtureNow+=kNativeDeathAge56+1;request();Drain(fixtureNow);ck(!nativeCalls,"C56 expired death evidence refuses even a fresh physical contact");
 setup(true);const auto originalDeathTick56=g_deaths56[0].tick;fixtureNow+=500;death56();ck(g_deaths56[0].tick==originalDeathTick56,"C56 repeated same-lifetime observation cannot refresh death proof lease");
 setup(true);request();ownerHook=[]{if(ownerReads==2)++kcd2vr::contact176::recoveryEpoch55c1;};Drain(fixtureNow);ck(!nativeCalls,"C56 recovery revoked during final owner callback is rechecked before proxy impulse");
 auto direct56=[](){put(proxy+0x38,body+1);put(body+0x28,std::int16_t(1));put(body+0x20,ent);put(g_base+kArticulatedVft19c4r4+0x50,g_base+0x5D6F44);};
 setup();direct56();request();Drain(fixtureNow);ck(!colliderImpulses37c4&&!nativeCalls,"C56 direct articulated limb fallback also refuses living/unknown target");
 setup(true);direct56();request();Drain(fixtureNow);ck(colliderImpulses37c4==1&&!nativeCalls,"C56 exact observed corpse retains existing same-entity raw-part direct route");
 setup(true);direct56();request();ownerHook=[]{if(ownerReads==2)++kcd2vr::contact176::recoveryEpoch55c1;};Drain(fixtureNow);ck(!colliderImpulses37c4&&!nativeCalls,"C56 recovery revoked during final owner callback is rechecked before direct limb impulse");

 setup(true);request(900);ck(g_count==1&&g_queue[0].tick==900,"queue original time");fixtureNow=1100;request(900);ck(g_count==1,"expired contact not refreshed");Drain(fixtureNow);ck(!nativeCalls,"expired queue no impulse");
 setup(true);request();Drain(fixtureNow);ck(nativeCalls==1&&g_sent==1&&!lastPoint,"complete qualified adapter invokes native no-point exactly once");ck(std::fabs(lastImpulse[2]+3.2f)<.001f,"bounded momentum from normalized direction");ck(receipts.empty(),"off recorder formats no impulse receipt");
 setup(true);request(0,3,40,1.49f);Drain(fixtureNow);ck(nativeCalls==1&&std::fabs(lastImpulse[2]+40)<.001f,"C33 configured request cap (40) and normal normalization");
 setup(true);g_cap=500;g_mass=6;request(0,3,40,1.f);Drain(fixtureNow);ck(nativeCalls==1&&std::fabs(lastImpulse[2]+80)<.001f,"C33 hard request ceiling 80 N*s");
 for(int type:{0,1,2,4}){setup(true);request(0,type);Drain(fixtureNow);ck(!nativeCalls,"all non-articulated contact classes refused");}
 for(auto vf:{0x3A578C0ull,0x3A82570ull,0x3A48DC0ull,0ull}){setup(true);request();put(body,g_base+vf);Drain(fixtureNow);ck(!nativeCalls,"current changed/non-articulated body refused");}
 for(auto off:{8,0x30,0x38}){setup(true);request();put(g_base+kArticulatedVft19c4r4+off,g_base+0x1234);Drain(fixtureNow);ck(!nativeCalls,"articulated virtual contract mismatch");}
 setup(true);request();put(proxy+0x38,body+1);Drain(fixtureNow);ck(!nativeCalls,"body replaced between contact and drain");
 setup(true);request();put(proxy+0x30,ent+1);Drain(fixtureNow);ck(!nativeCalls,"wrong proxy entity");
 setup(true);request();put(proxy,g_base+kPhysicalProxyVft5916c3+8);Drain(fixtureNow);ck(!nativeCalls,"wrong proxy class");
 setup(true);request();put(ent+0x98,std::uintptr_t(0x6000));put(0x6018,std::uintptr_t(0x7000));Drain(fixtureNow);ck(!nativeCalls,"native parent forwarding refused");
 setup(true);request();entities.clear();Drain(fixtureNow);ck(!nativeCalls,"missing entity");
 setup(true);request();contract=false;Drain(fixtureNow);ck(!nativeCalls&&!entityReads,"fingerprint mismatch before entity lookup");
 setup(true);request();owner=false;Drain(fixtureNow);ck(!nativeCalls&&!entityReads,"menu/tracking/context disallow before native lookup");
 for(int field=0;field<4;++field){setup(true);request();auto&r=g_queue[0];if(field==0)++r.key;if(field==1)++r.generation;if(field==2)++r.epoch;if(field==3)++r.request;Drain(fixtureNow);ck(!nativeCalls&&!entityReads,"full weapon/generation/body/request owner mismatch");}
 setup(true);request();++g_session;Drain(fixtureNow);ck(!nativeCalls,"mode transition revokes original queue");
 setup(true);request();++kcd2vr::contact176::recoveryEpoch55c1;Drain(fixtureNow);ck(!nativeCalls,"recovery change revokes queue");
 setup(true);request();entityHook=[]{owner=false;};Drain(fixtureNow);ck(!nativeCalls,"owner changed during native entity lookup refused");
 setup(true);request();ownerHook=[]{if(ownerReads==2)put(proxy+0x38,body+1);};Drain(fixtureNow);ck(!nativeCalls,"body rechecked after final owner read");
 setup(true);request();entityHook=[]{g_enabled=false;};Drain(fixtureNow);ck(!nativeCalls,"off during lookup cannot dispatch");
 setup(true);request();request();Drain(fixtureNow);ck(nativeCalls==1&&g_dropped==1,"duplicate/two-hand same-target contacts consume one budget");
 fixtureNow=1100;request();Drain(fixtureNow);ck(nativeCalls==1,"per-body cooldown stops sustained contact pumping");fixtureNow=1500;request();Drain(fixtureNow);ck(nativeCalls==2,"new qualified contact after500ms works");
 setup(true);request();Req r=g_queue[0];Drain(fixtureNow);const char*why="";fixtureNow=1500;ck(!Reserve19c4r4(r,fixtureNow,why)&&std::strcmp(why,"DUPLICATE_CONTACT")==0,"budget never reuses same original contact");
 setup(true);request();nativeHook=[]{request();Drain(fixtureNow);};Drain(fixtureNow);ck(nativeCalls==1,"reservation precedes native leaf, reentrant duplicate refused");
 setup(true);request();nativeFails=true;Drain(fixtureNow);ck(nativeCalls==1&&g_fault,"native exception latches closed");fixtureNow+=600;request();Drain(fixtureNow);ck(nativeCalls==1,"fault does not retry even after cooldown");
 setup(true);g_enabled=false;g_observe=true;diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,fixtureNow);request();Drain(fixtureNow);ck(!nativeCalls&&!entityReads&&receipts.size()==1&&receipts[0].find("OBSERVE_ONLY")!=std::string::npos,"rung1 observation no entity/native calls");
 setup(true);g_enabled=false;request();ck(!g_count,"OFF produces nothing");g_enabled=true;request();g_enabled=false;Drain(fixtureNow);ck(!g_count&&!nativeCalls,"OFF purges pending queue");
 for(float scale:{0.f,-1.f,std::numeric_limits<float>::quiet_NaN()}){setup(true);g_scale=scale;request();Drain(fixtureNow);ck(!nativeCalls,"invalid tuning refuses");}
 setup(true);for(int i=0;i<9;++i)request();ck(g_count==8&&g_dropped==1,"queue bounded");Drain(fixtureNow);ck(nativeCalls==1,"full duplicate queue produces one impulse");
 setup(true);request();r=g_queue[0];for(unsigned i=0;i<32;++i){r.entity=i+10;r.collider=i+100;ck(Reserve19c4r4(r,fixtureNow,why),"bounded table accepts distinct body");}r.entity=99;ck(!Reserve19c4r4(r,fixtureNow,why),"full live budget table refuses, no eviction bypass");fixtureNow+=2000;ck(Reserve19c4r4(r,fixtureNow,why),"expired budget slot reusable");

 // H7 executes the actual new fallback caller; current-body/status bridge is
 // mocked here and separately exercised from actual production in its fixture.
 const auto skeleton57=[](){direct57();put(body+0x28,std::int16_t(99));put(body+0x20,std::uintptr_t(0));frik_pose_probe::currentMainH7=true;};
 setup(true);skeleton57();request();Drain(fixtureNow);ck(colliderImpulses37c4==1&&lastCollider57==body&&lastPart57==3,"new corpse contact uses exact current main and raw part");
 setup(false);skeleton57();request();Drain(fixtureNow);ck(!colliderImpulses37c4&&!frik_pose_probe::currentCallsH7,"no native death proof means no new corpse query/action");
 setup(true);skeleton57();request();g_queue[0].part=-1;Drain(fixtureNow);ck(!colliderImpulses37c4,"new route cannot use all-body sentinel");
 setup(true);skeleton57();request();frik_pose_probe::queryHookH7=[](){if(frik_pose_probe::currentCallsH7==2)owner=false;};Drain(fixtureNow);ck(!colliderImpulses37c4,"owner rechecked after current-body status recapture");
 setup(true);skeleton57();request();frik_pose_probe::queryHookH7=[](){g_deaths56={};};Drain(fixtureNow);ck(!colliderImpulses37c4,"death proof loss during query prevents mutation");
 setup(true);skeleton57();request();frik_pose_probe::queryHookH7=[](){put(ent+0x98,std::uintptr_t(0x6000));put(0x6018,std::uintptr_t(0x7000));};Drain(fixtureNow);ck(!colliderImpulses37c4,"parent transfer during query prevents mutation");
 setup(true);skeleton57();secondRecipient57();request();requestSecond57();frik_pose_probe::statusFaultH7=true;Drain(fixtureNow);ck(g_fault&&!colliderImpulses37c4&&!nativeCalls&&!g_count,"corpse status fault stops remaining native batch");
 std::printf("%u actual F9 request/ownership/dispatch/cleanup checks passed; native leaf mocked\n",checks);
}
