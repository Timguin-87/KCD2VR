#include "vrik/motion_owned59c4.h"
#include "vrik/swing_effort59c14.h"
#include "vrik/physical_weapon.h"
#include <cassert>
#include <cstdio>
#include <atomic>
#include <vector>
#include <string>
namespace mo=kcd2vr::motion59c4;namespace se=kcd2vr::swing_effort59c14;
namespace ph=kcd2vr::physical;
// Use the actual physical packet and actual publication helper. The complete
// production PhysPublish caller is independently exercised by physical_bridge.
struct PhysFrame6698 {ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};
 void* player=nullptr;std::uint64_t request=0;double publishWorldDeltaM122=0,publishWorldDeltaRad122=0;float world[8]{0,0,0,1,0,0,0,1};};
struct PhysEquipment6698 {bool valid=false;void* player=nullptr;std::uint64_t request=0,epoch=0,tick=0;
 int count=1;int heldIndex[2]{0,-1};bool handKnown[2]{true,true};std::uint64_t handKey[2]{};
 struct Item{ph::Profile profile{};bool held=true;};std::array<Item,2> items{};};
namespace kcd2vr::contact176 {inline std::atomic<std::uint64_t> recoveryEpoch55c1{6};}
#include "effort_impact_publisher19.inc"
static std::atomic<bool> g_swing_stamina{true},g_effort_fault{false};
static mo::Key g_key5912{};static std::uint64_t clockValue=1030;
static bool upOK=true,downOK=true,preflightOK=true,replaceKeyOnUp=false;static float stamina=100,raiseScaleSeen=0;
static std::size_t failAfterCalls=~std::size_t(0);
static std::vector<bool> calls;static std::vector<std::string> logs;
static std::uint64_t GetTickCount64(){return clockValue;}
static bool Allow(int,int){return true;}
namespace diagnostic_trace{static void Write(const char*s){logs.emplace_back(s);}}
static std::uint64_t EffortWuid5919(const void*,const mo::Key&){return preflightOK?123:0;}
static int EffortNative5916(const void*,bool up,float& before,float& after) noexcept{
 if(!preflightOK)return 0;
 if(calls.size()>=failAfterCalls){calls.push_back(up);return 2;}
 calls.push_back(up);before=stamina;
 if(up){float delta=-8.4f;
   assert(se::scaleOwnedEffect(123,0xC5BBEB,1,delta));
   raiseScaleSeen=delta/-8.4f;stamina+=delta;if(replaceKeyOnUp)++g_key5912.gesture;
 }
 after=stamina;return (up?upOK:downOK)?1:2;
}
#include "effort_transaction5916c1.inc"
static mo::Key key(unsigned id){return {id,12,3,4,5,6,7};}
static void prepare(mo::Key k,std::uint64_t tick=1020,double energy=1.){
 assert(mo::attempts.event(k,mo::Event::Prepare,tick-20,tick-20,2,0));
 assert(mo::attempts.event(k,mo::Event::Commit,tick,tick,2,0));
 assert(se::ledger.publish(k,energy,1.1,false,tick));
}
static void setup(unsigned id){
 mo::attempts.clear();se::ledger.clear();g_effort_fault=false;g_swing_stamina=true;calls.clear();logs.clear();
 se::scale.store(1.);raiseScaleSeen=0;stamina=100;upOK=downOK=preflightOK=true;replaceKeyOnUp=false;failAfterCalls=~std::size_t(0);
 g_key5912=key(id);clockValue=1030;
}
static bool near(double a,double b){return a>b-.01&&a<b+.01;}
static PhysFrame6698 contactPacket(mo::Key k,double speed=2.,std::uint64_t tick=1020){
 PhysFrame6698 f{};f.request=k.request;f.player=reinterpret_cast<void*>(k.player);
 f.input.epoch=k.request;f.input.bodyEpoch=k.body;f.input.tick=tick;f.input.sample=tick;
 f.input.motionOwned59c5=f.input.physicalCombat177=f.input.allowed=f.input.attacks=f.input.tracked[0]=true;
 f.output.active[0]=f.output.owned[0]=f.output.contactKnown[0]=true;
 auto& p=f.profiles[0];p.key=k.weapon;p.generation=k.generation;p.classId=4;p.tip={0,.8,0};p.weight=1.7;
 auto& r=f.output.impactRecord178r30[0];r.valid=true;r.tick=tick;r.sample=tick;r.key=k.weapon;r.generation=k.generation;
 r.epoch=k.body;r.request=k.request;r.gesture59c3=k.gesture;r.entityId=100;r.collider=200;r.part=3;r.speed=speed;r.kind=1;
 ph::impact_effort19(r,p,0,{}, {0,speed,0},{},1.);
 return f;
}
static void publishContact(const PhysFrame6698& f){
 PhysEquipment6698 equipment{true,f.player,f.request,f.input.bodyEpoch,f.input.tick};
 equipment.handKey[0]=f.profiles[0].key;equipment.items[0].profile=f.profiles[0];
 PhysPublishImpactEffort19(f,equipment);
}
int main(){
 setup(1);assert(mo::attempts.event(g_key5912,mo::Event::Prepare,1000,1000,2,0));
 assert(!ChargeEffort5916(nullptr)&&calls.empty()); // no energy/cost inferred from a held preparation
 assert(mo::attempts.event(g_key5912,mo::Event::Commit,1020,1020,2,0));
 assert(se::ledger.publish(g_key5912,1.,1.1,false,1020));
 assert(ChargeEffort5916(nullptr)&&calls==std::vector<bool>({true,false})&&near(raiseScaleSeen,1.02/.30));
 assert(ChargeEffort5916(nullptr)&&calls.size()==2);
 mo::attempts.event(g_key5912,mo::Event::Cancel,1030,1030,2,0);
 assert(ChargeEffort5916(nullptr)&&calls.size()==2); // no resurrection/retry after native debit
 setup(2);prepare(g_key5912);upOK=false;
 assert(!ChargeEffort5916(nullptr)&&calls==std::vector<bool>({true,false})&&g_effort_fault);
 assert(!ChargeEffort5916(nullptr)&&calls.size()==2);
 setup(3);prepare(g_key5912);downOK=false;
 assert(!ChargeEffort5916(nullptr)&&calls.size()==2&&g_effort_fault);
 setup(4);prepare(g_key5912);clockValue=1621;
 assert(!ChargeEffort5916(nullptr)&&calls.empty()); // motion ledger native strike lifetime remains narrower
 setup(5);g_swing_stamina=false;assert(ChargeEffort5916(nullptr)&&calls.empty());
 setup(6);prepare(g_key5912);preflightOK=false;
 assert(!ChargeEffort5916(nullptr)&&calls.empty()&&!g_effort_fault);
 preflightOK=true;assert(!ChargeEffort5916(nullptr)&&calls.empty()); // no cost success claimed; failed attempt terminal
 setup(8);prepare(g_key5912);stamina=0;
 assert(ChargeEffort5916(nullptr)&&calls.size()==2); // no invented stamina eligibility gate
 setup(10);prepare(g_key5912,1020,.4);
 assert(ChargeEffort5916(nullptr)&&near(raiseScaleSeen,(.30+.72*.4)/.30));
 setup(11);prepare(g_key5912,1020,1.25);assert(ChargeEffort5916(nullptr)&&near(raiseScaleSeen,1.20/.30));
 setup(12);se::scale=0.;prepare(g_key5912);assert(ChargeEffort5916(nullptr)&&raiseScaleSeen==0.f);
 setup(14);prepare(g_key5912);failAfterCalls=1;
 assert(!ChargeEffort5916(nullptr)&&g_effort_fault&&calls.size()==2);
 setup(15);prepare(g_key5912);replaceKeyOnUp=true;
 assert(!ChargeEffort5916(nullptr)&&calls==std::vector<bool>({true})&&g_effort_fault); // callback replacement: never lower B using A's cleanup
 // The old latest-slot failure: delayed A must debit A's immutable factor after a newer B publish.
 setup(20);auto a=g_key5912;auto b=key(21);prepare(a,1020,.4);prepare(b,1025,1.25);
 assert(ChargeEffort5916(nullptr)&&near(raiseScaleSeen,(.30+.72*.4)/.30));
 g_key5912=b;assert(ChargeEffort5916(nullptr)&&calls.size()==4&&near(raiseScaleSeen,1.20/.30));
 // Paying A cannot zero or credit an unrelated asynchronous native B. The actual policy forwards unchanged.
 float nativeB=-19.f;assert(!se::scaleOwnedEffect(123,0xC64DCA,2,nativeB)&&nativeB==-19.f);
 auto c=key(22);prepare(c,1028,.8);nativeB=-8.4f;
 assert(!se::scaleOwnedEffect(123,0xC5BBEB,1,nativeB)&&nativeB==-8.4f); // no scoped owner, even exact Henry/call site
 g_key5912=c;assert(ChargeEffort5916(nullptr)&&calls.size()==6); // native B did not mark C paid
 // Contact-led attempts: energy is copied before impact publication, but no
 // debit occurs until the exact attempt is recognized or contact-qualified.
 setup(30);mo::selected=true;auto early=g_key5912;
 assert(mo::attempts.event(early,mo::Event::Prepare,1000,1000,2,1));
 auto packet=contactPacket(early);publishContact(packet);assert(calls.empty());
 mo::Attempt contacted{};assert(mo::attempts.contact(early,1020,1030,100,1,contacted)==mo::Claim::Ready);
 assert(ChargeEffort5916(nullptr)&&calls.size()==2&&near(raiseScaleSeen,se::factor(2./3.)/.30));
 assert(!mo::attempts.event(early,mo::Event::Commit,1030,1030,2,1));
 assert(!se::ledger.publish(early,1.25,6.,true,1030));
 assert(ChargeEffort5916(nullptr)&&calls.size()==2); // late Commit cannot debit twice
 // Resting touches, held wind-up, retained old impact, foreign owner and native
 // comparison backend cannot populate the exact cost record.
 for(unsigned bad=0;bad<12;++bad){
  setup(40+bad);mo::selected=true;packet=contactPacket(g_key5912);
  switch(bad){case 0:packet.output.impactRecord178r30[0].speed=.2;break;
   case 1:packet.output.impactRecord178r30[0].gesture59c3=0;break;
   case 2:++packet.input.sample;break;case 3:++packet.input.tick;break;
   case 4:packet.output.handlingOnly55c1[0]=true;break;
   case 5:packet.output.impactRecord178r30[0].generation++;break;
   case 6:packet.input.allowed=false;break;case 7:packet.input.tracked[0]=false;break;
   case 8:packet.output.sweepDeferred115[0]=true;break;case 9:mo::selected=false;break;
   case 10:packet.input.haveWorld=true;packet.publishWorldDeltaM122=.01;break;
   case 11:packet.input.haveWorld=true;packet.world[7]=2;break;}
  publishContact(packet);se::Record cost{};assert(se::ledger.reserve(g_key5912,1030,cost)==se::Claim::Missing&&calls.empty());
 }
 for(unsigned bad=0;bad<6;++bad){
  setup(52+bad);mo::selected=true;packet=contactPacket(g_key5912);
  PhysEquipment6698 equipment{true,packet.player,packet.request,packet.input.bodyEpoch,packet.input.tick};
  equipment.handKey[0]=packet.profiles[0].key;equipment.items[0].profile=packet.profiles[0];
  switch(bad){case 0:equipment.tick-=101;break;case 1:equipment.handKey[0]++;break;
   case 2:equipment.items[0].profile.generation++;break;case 3:equipment.handKnown[0]=false;break;
   case 4:equipment.items[0].held=false;break;case 5:equipment.heldIndex[0]=2;break;}
  PhysPublishImpactEffort19(packet,equipment);se::Record cost{};
  assert(se::ledger.reserve(g_key5912,1030,cost)==se::Claim::Missing&&calls.empty());
 }
 setup(60);mo::selected=true;auto hard=g_key5912;
 assert(mo::attempts.event(hard,mo::Event::Prepare,1000,1000,2,1));publishContact(contactPacket(hard));
 assert(mo::attempts.event(hard,mo::Event::Cancel,1025,1025,2,1));se::ledger.cancel(hard);
 assert(mo::attempts.contact(hard,1020,1030,100,1,contacted)==mo::Claim::Cancelled);
 assert(!ChargeEffort5916(nullptr)&&calls.empty());
 setup(61);mo::selected=true;auto late=g_key5912;
 assert(mo::attempts.event(late,mo::Event::Prepare,1000,1000,2,1));publishContact(contactPacket(late));
 assert(mo::attempts.event(late,mo::Event::Cancel,1025,1025,2,1,true));
 assert(mo::attempts.contact(late,1020,1030,100,1,contacted)==mo::Claim::Ready&&contacted.late18);
 assert(ChargeEffort5916(nullptr)&&calls.size()==2);assert(ChargeEffort5916(nullptr)&&calls.size()==2);
 setup(62);mo::selected=true;a=g_key5912;b=key(63);
 assert(mo::attempts.event(a,mo::Event::Prepare,1000,1000,2,1));
 assert(mo::attempts.event(b,mo::Event::Prepare,1005,1005,1,1));
 publishContact(contactPacket(a,1.2,1020));publishContact(contactPacket(b,3.0,1025));
 assert(mo::attempts.contact(a,1020,1030,100,1,contacted)==mo::Claim::Ready);
 assert(ChargeEffort5916(nullptr)&&near(raiseScaleSeen,se::factor(.4)/.30));
 assert(mo::attempts.contact(b,1025,1030,101,1,contacted)==mo::Claim::Ready);g_key5912=b;
 assert(ChargeEffort5916(nullptr)&&calls.size()==4&&near(raiseScaleSeen,se::factor(1.)/.30));
 mo::selected=false;
 std::puts("PASS actual effort transaction: exact A/B factors, once-only native pairs, refusal vs indeterminate, vanilla unknown callbacks");
}
