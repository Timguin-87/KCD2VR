#include "vrik/environment_fx178r22c1.h"
#include "core/haptics.h"
#include <cassert>
#include <cstdio>
namespace ph=kcd2vr::physical;
struct PhysFrame6698 {ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};
 void* player=nullptr;void* manager=nullptr;std::uint64_t request=0;float world[8]{};
 double publishWorldDeltaM122=0,publishWorldDeltaRad122=0;};
struct PhysLive103 {void* player=nullptr;void* manager=nullptr;std::uint64_t request=0,epoch=0,hand[2]{};};
static std::uint64_t clock58=1000;static auto GetTickCount64(){return clock58;}
static std::atomic<bool> g_contact_fx178r12{true},g_contact_fx_fault178r12{false},g_gameplay_allowed{true};
static bool full58=true;static bool CombatFull178r10Enabled(){return full58;}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
static std::atomic<std::uint64_t> g_phys_ready_sample6698{0};
static kcd2vr::core::SnapshotMailbox<PhysFrame6698> g_phys_frame6698;
static PhysLive103 owner58{};static bool liveOwner58=true;
static bool PhysReadLive103(PhysLive103& out){out=owner58;return liveOwner58;}
static unsigned fxCalls58=0;static double scale58=0;static bool scraped58=false;
static void ContactFxRequest178r12(const PhysFrame6698&,int,int,double scale,bool scrape){++fxCalls58;scale58=scale;scraped58=scrape;}
#include "environment_delivery58.inc"
static PhysFrame6698 frame(){PhysFrame6698 f{};f.player=reinterpret_cast<void*>(1);f.manager=reinterpret_cast<void*>(2);f.request=5;f.world[7]=1;
 auto&i=f.input;i.allowed=i.contact=i.haveWorld=i.physicalCombat177=true;i.epoch=5;i.bodyEpoch=2;i.tick=1000;i.sample=10;
 i.tracked[0]=i.intentTracked[0]=true;i.wallSlide58=true;auto&o=f.output;o.active[0]=o.owned[0]=o.blocked[0]=true;
 o.wallScraped58[0]=true;o.contactMotion169r1[0]={0,.4,.008,true};
 auto&p=f.profiles[0];p.key=12;p.generation=3;p.entity=91;p.classId=1;
 auto&c=o.contact119[0];c.known=c.plane=true;c.normal={0,1,0};c.native172.collider=100;c.native172.surfaceIdx=104;return f;}
static void publish(PhysFrame6698& f,std::uint64_t t){clock58=t;f.input.tick=t;++f.input.sample;
 owner58={f.player,f.manager,f.request,f.input.bodyEpoch,{f.profiles[0].key,0}};
 g_phys_frame6698.publish(f);g_phys_ready_sample6698=f.input.sample;PublishEnvironmentFx178r22c1(f);}
int main(){auto f=frame();kcd2vr::haptics101::enabled=kcd2vr::haptics101::allowed=true;
 auto h=kcd2vr::haptics101::eventSequence.load();publish(f,1000);ConsumeEnvironmentFx178r22c1();
 assert(fxCalls58==1&&scraped58&&scale58==.30&&kcd2vr::haptics101::eventSequence.load()==h+1);
 ConsumeEnvironmentFx178r22c1();assert(fxCalls58==1);
 for(unsigned t=1020;t<=1140;t+=20){publish(f,t);ConsumeEnvironmentFx178r22c1();}
 assert(fxCalls58==2);f.output.contactMotion169r1[0].slipDistance=f.output.contactMotion169r1[0].slipSpeed=0;
 publish(f,1160);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==2); // stationary flag is quiet
 f.output.contactMotion169r1[0]={0,.4,.008,true};f.output.wallBackout58[0]=true;
 publish(f,1180);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==2);f.output.wallBackout58[0]=false;
 g_contact_fx178r12=false;publish(f,1200);ConsumeEnvironmentFx178r22c1();assert(!g_environment_fx_pending178r22c1&&fxCalls58==2);
 g_contact_fx178r12=true;publish(f,1220);clock58=1301;ConsumeEnvironmentFx178r22c1();assert(fxCalls58==2); // stale original event
 ++f.profiles[0].generation;publish(f,1320);++owner58.hand[0];ConsumeEnvironmentFx178r22c1();assert(fxCalls58==2);
 ++f.profiles[0].generation;publish(f,1340);PhysFrame6698 changed=f;changed.output.wallScraped58[0]=false;
 g_phys_frame6698.publish(changed);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==2); // no current same-plane slide
 ++f.profiles[0].generation;kcd2vr::haptics101::enabled=false;h=kcd2vr::haptics101::eventSequence.load();
 publish(f,1360);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==3&&kcd2vr::haptics101::eventSequence.load()==h);
 full58=false;f.input.physicalCombat177=false;++f.profiles[0].generation;publish(f,1380);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==4); // ordinary VRIK world FX works without F4
 g_gameplay_allowed=false;publish(f,1390);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==4);g_gameplay_allowed=true;
 full58=true;f.output.wallScraped58[0]=false;f.output.impactSpeed[0]=2;
 publish(f,1400);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==4); // continuous contact: new inward pressure is not another impact
 f.output.blocked[0]=false;f.output.contact119[0].known=false;f.output.impactSpeed[0]=0;
 publish(f,1410);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==4); // unknown is not evidence that the blade left the wall
 f.output.contact119[0].known=true;f.output.sweepDeferred115[0]=true;
 publish(f,1420);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==4); // nor is an untested deferred clear path
 f.output.blocked[0]=true;f.output.sweepDeferred115[0]=false;f.output.impactSpeed[0]=2;
 publish(f,1430);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==4); // no replay after those non-evidence gaps
 f.output.blocked[0]=false;f.output.impactSpeed[0]=0;
 publish(f,1440);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==4); // a checked clear path rearms normal impact
 f.output.blocked[0]=true;f.output.impactSpeed[0]=2;
 publish(f,1450);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==5&&!scraped58&&scale58>0.6);
 assert(kcd2vr::haptics101::eventSequence.load()==h); // existing impact haptics not duplicated
 // H18: turning scrape off suppresses continued wall particles without
 // disabling the first real impact, even with leftover scrape metadata.
 f.input.wallSlide58=false;f.output.wallScraped58[0]=true;f.output.impactSpeed[0]=0;
 publish(f,1460);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==5);
 f.output.blocked[0]=false;f.output.contact119[0].known=true;
 publish(f,1470);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==5);
 f.output.blocked[0]=true;f.output.impactSpeed[0]=2;
 publish(f,1480);ConsumeEnvironmentFx178r22c1();assert(fxCalls58==6&&!scraped58);
 std::puts("environment_delivery58: actual publisher/current-owner consumer, scrape cadence, normal-contact episode, checked reentry, stale/owner/mode/haptics-option PASS");
}
