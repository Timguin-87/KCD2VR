// Complete actual producer publication + actual key/attempt/release/cost policy.
// Native action completion is an explicit fixture witness; no game code runs.
#define ArmF4ContactGate176 FixtureContactGateH8
#include "frik_pose_probe.cpp"
#undef ArmF4ContactGate176
#include "gesture_recovery_h30_fixture.h"
#include "subsystems/hybrid/contact_policy.h"
namespace kcd2vr::combat_diag_probe {bool FixtureContactGateH8() noexcept{return true;}}
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::animation_filter {void SetMode(SuppressionMode) noexcept {}bool Install() noexcept{return false;}bool Ready() noexcept{return false;}}
namespace kcd2vr::native_stereo_outer_lab {bool EnsurePhysicalFramePump6698() noexcept{return true;}}
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
using namespace kcd2vr::frik_pose_probe;
namespace mo=kcd2vr::motion59c4;namespace se=kcd2vr::swing_effort59c14;namespace h=kcd2vr::hybrid54;namespace ct=kcd2vr::contact59c3;
static std::vector<unsigned char> playerH8(0xB00);
struct PublisherH8 {
 RigH30 r{false};PhysEquipment6698 gear{};std::array<ph::HolsterState,8> holsters{};float world[8]{0,0,0,1,0,0,0,1};
 ph::Command commit{};mo::Key committed{};kcd2vr::contact176::Ticket ticket{};unsigned nativeCompletions=0;bool deferNative=false;
 PublisherH8(bool defer=false):deferNative(defer){r.in.hitTraceC54R3=true;gear.valid=true;gear.player=playerH8.data();gear.manager=reinterpret_cast<void*>(0x1234);gear.request=r.in.epoch;gear.epoch=r.in.bodyEpoch;
  gear.count=1;gear.heldIndex[0]=0;gear.handKnown[0]=gear.handKnown[1]=true;gear.handKey[0]=r.profiles[0].key;
  gear.items[0].held=true;gear.items[0].profile=r.profiles[0];g_phys_ready_sample6698=0;g_phys_commands6698.head=g_phys_commands6698.tail=0;
  g_weapon_request_epoch6694=r.in.epoch;g_pose_epoch6689=r.in.bodyEpoch;mo::hybridAttempts54.clear();se::ledger.clear();
  kcd2vr::contact176::recoveryEpoch55c1=10;kcd2vr::contact176::nextSerial=0;
  publish();for(unsigned n=0;n<20&&!commit.gesture59c3;++n){r.step({0,.035,0});publish();}
 }
 void publish(){gear.tick=r.in.tick;hostTickCount64=r.in.tick;
  PhysPublish6698(r.in,r.out,r.profiles,gear,world,r.state,holsters);
  if(deferNative){const auto tail=g_phys_commands6698.tail.load(),head=g_phys_commands6698.head.load();
   for(auto slot=tail;slot!=head;slot=(slot+1)%g_phys_commands6698.q.size()){
    const auto c=g_phys_commands6698.q[slot];if(c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust){
     commit=c;committed={c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch,c.recoveryEpochH30,reinterpret_cast<std::uintptr_t>(gear.player)};break;}}
   return;
  }
  ph::Command c{};while(g_phys_commands6698.pop(c)){
   if((c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust)&&!commit.gesture59c3){commit=c;
    committed={c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch,c.recoveryEpochH30,reinterpret_cast<std::uintptr_t>(gear.player)};
    PhysFrame6698 f{};f.input=r.in;f.output=r.out;f.profiles=r.profiles;f.player=gear.player;f.request=r.in.epoch;
    // Actual publication after a successful native edge; native completion
    // itself is a fixture leaf and does not debit health/stamina or run retail.
    PublishPhysicalStrike176(c,f);se::ledger.nativeObserved54(committed,c.tick);se::ledger.nativeComplete54(committed,c.tick);++nativeCompletions;
    ct::releases.find({c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch,c.recoveryEpochH30},ticket);
   }
  }
 }
};

namespace drainFixtureH8 {
using namespace kcd2vr;namespace frik_pose_probe=kcd2vr::frik_pose_probe;
std::atomic<bool> g_sync59c10r9{true};std::atomic<unsigned> g_drop_duplicate56{0},g_drop_nonordinary_phase56{0};
static bool Allow(int,int){return false;}
static constexpr int kActionPhaseProp39=0;static int I(const void* p,int){return *static_cast<const int*>(p);}
template<class T> static T Rd(const void* p,std::size_t offset,T fallback){T out{};return Read(static_cast<const unsigned char*>(p)+offset,out)?out:fallback;}
struct Pending {std::uint64_t gesture59c3=0,key=0,generation=0,epoch=0,request=0,recovery55c1=0,tick=0;
 unsigned entityId=0,serial=1;int kind=0;contact176::Ticket release51{};
 std::uint64_t sample=0,queued=0;contact59c7g::Metadata metadata59c7g{};bool weaponItem=false,targetWait19c3=false;int part=1;};
struct Lease {unsigned waitCount=0,deferCount=0;void wait(const Pending&){++waitCount;}void defer(const Pending&){++deferCount;}};
std::array<std::atomic<unsigned>,12> g_stats{};
namespace handoff59c3=kcd2vr::contact59c3;
#include "generated_quarantine_owner_h8.inc"
std::uintptr_t g_base=0;constexpr std::uintptr_t kEntityVft=0x4072C78;
namespace impulse596 {constexpr std::uintptr_t kPhysicalProxyVft5916c3=0x3AC3478;}
static bool SehCopy146(const void* p,void* out,std::size_t n){if(!p||!out)return false;std::memcpy(out,p,n);return true;}
#include "generated_quarantine_body_h8.inc"
struct Actor {const void* entity=nullptr;bool coherent=true;};
static bool CombatActor(const void* a,const void*& entity,const void*& state,bool){
 if(!a)return false;const auto& value=*static_cast<const Actor*>(a);entity=value.entity;state=a;return value.coherent;}
static unsigned EntityId(const void* e){return Rd<unsigned>(e,0xc,0);}
static bool targetCurrent(Pending& p,const void* target,const void* henryActor){const void* te=nullptr,*ts=nullptr;
#include "generated_quarantine_target_h8.inc"
 return target!=nullptr;
}
unsigned outerContinued=0;
static void outer(Pending& p,Lease& pendingLease,const void* henryActor,const void* he,std::uint64_t now){
 const bool liveRoute39=true;const auto maxAge39=handoff59c3::maximumContactAge;
#include "generated_quarantine_outer_h8.inc"
 ++outerContinued; // next production stage must still validate exact struck target/body
}
unsigned entered=0;
static void drain(Pending& p,const void* henryActor,std::uint64_t now,bool noRelease54=false,int phase=8){
 const bool liveRoute39=true,vrDirect596=false;const void* hs=&phase;Lease pendingLease{};
 motion59c4::ClaimReservationC2 hybridReservation54;
#include "generated_quarantine_drain_h8.inc"
 ++entered;hybridReservation54.nativeCallStarted(); // counted native-entry leaf; no game call/damage
}
}
static void outlierH8(PublisherH8& p,int family=3,double angle=.75){
 const auto q=pm::from_rotation_vector({0,0,angle});
 if(family&1)p.r.in.hands[0].q=q;if(family&2)p.r.in.intentHands[0].q=q;
 p.r.step({0,.025,0});p.publish();
}
static PhysFrame6698 frameH8(const PublisherH8& p){PhysFrame6698 f{};f.input=p.r.in;f.output=p.r.out;f.profiles=p.r.profiles;f.player=p.gear.player;return f;}

static drainFixtureH8::Pending pendingH8(const PublisherH8& p,const ph::ImpactRecord178r30& a){
 drainFixtureH8::Pending pending{a.gesture59c3,a.key,a.generation,a.epoch,a.request,kcd2vr::contact176::recoveryEpoch55c1.load(),a.tick,a.entityId};
 pending.release51=p.ticket;pending.sample=a.sample;pending.queued=a.tick;pending.metadata59c7g.collider=a.collider;return pending;
}
static void trackingLiveH8(const PublisherH8& p){g_gameplay_allowed=true;g_head_valid=g_left_valid=g_right_valid=true;
 g_head_valid_tick=g_left_valid_tick=g_right_valid_tick=p.r.in.tick;}
static void outerH8(){
 for(unsigned bad=0;bad<5;++bad){PublisherH8 p(true);p.r.actor=p.r.blocked=true;p.r.step({0,.035,0});p.publish();
  const auto impact=p.r.out.impactRecord178r30[0];auto pending=pendingH8(p,impact);
  // Additional good source tick makes command expiry independent of the latest
  // frame-age80ms check in the delayed-native callback negative.
  p.r.actor=p.r.blocked=false;p.r.step({0,.035,0});p.publish();outlierH8(p);trackingLiveH8(p);
  std::array<unsigned char,0x220> actor{};void* player=p.gear.player;std::memcpy(actor.data()+0x1f8,&player,sizeof player);
  ckH30(!p.nativeCompletions&&!pending.release51.serial&&se::ledger.contactProof54(p.committed,p.r.in.tick)==se::ContactProof54::Refuse,
    "pre-dispatch fixture has actual queued COMMIT and checked contact but no native cost/release witness yet");
  if(bad==1)g_phys_commands6698.tail=g_phys_commands6698.head.load();
  if(bad==2)for(auto& c:g_phys_commands6698.q)if(c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust)++c.gesture59c3;
  if(bad==3){pending.tick=p.r.in.tick;pending.sample=p.r.in.sample;}
  const auto now=bad==4?p.commit.tick+101:p.r.in.tick;
  drainFixtureH8::Lease lease{};const auto passed=drainFixtureH8::outerContinued;
  drainFixtureH8::outer(pending,lease,actor.data(),g_last_player_entity.load(),now);
  ckH30(lease.waitCount==(bad==0)&&drainFixtureH8::outerContinued==passed,
    "outer WAIT retains exact queued pre-dispatch attack only; missing/different/expired command or current-q contact never qualifies");
  if(!bad){
   auto frame=frameH8(p);auto c=p.commit;PhysNativeOwner6698 native{};
   ckH30(PhysCommandAuthorityH8(frame,c,native,p.r.in.tick,true,false,false,false)==PhysCommandDecisionH8::Wait,
     "same actual not-yet-native COMMIT also waits at current motion command gate");
   p.r.step({0,.035,0});p.deferNative=false;p.commit={};p.publish();trackingLiveH8(p);
   drainFixtureH8::Lease resumed{};drainFixtureH8::outer(pending,resumed,actor.data(),g_last_player_entity.load(),p.r.in.tick);
   ckH30(!resumed.waitCount&&drainFixtureH8::outerContinued==passed+1&&p.nativeCompletions==1&&p.ticket.serial,
     "readable resumption dispatches original command once before normal owner/cost admission");
   const auto entered=drainFixtureH8::entered;drainFixtureH8::drain(pending,actor.data(),p.r.in.tick);drainFixtureH8::drain(pending,actor.data(),p.r.in.tick);
   ckH30(drainFixtureH8::entered==entered+1,"pre-dispatch contact subsequently reaches unchanged exact native-cost/once-only Drain leaf once");
  }
 }

 {PublisherH8 p(true);p.r.actor=p.r.blocked=true;p.r.step({0,.035,0});p.publish();const auto impact=p.r.out.impactRecord178r30[0];
  auto pending=pendingH8(p,impact);p.r.actor=p.r.blocked=false;p.r.step({0,.035,0});p.publish();outlierH8(p);trackingLiveH8(p);
  std::array<unsigned char,0x220> actor{};void* player=p.gear.player;std::memcpy(actor.data()+0x1f8,&player,sizeof player);
  const auto frame=frameH8(p);const auto tail=g_phys_commands6698.tail.load(),head=g_phys_commands6698.head.load();
  const kcd2vr::original_contact_h8::Source original{pending.gesture59c3,pending.key,pending.generation,pending.epoch,pending.request,pending.recovery55c1,
   pending.sample,pending.tick,reinterpret_cast<std::uintptr_t>(player),reinterpret_cast<std::uintptr_t>(g_last_player_entity.load()),pending.metadata59c7g.collider};
  std::atomic<bool> locked{false},release{false};std::thread consumer([&]{std::lock_guard<std::recursive_mutex> lock(g_phys_native_mutex6698);locked=true;while(!release.load())std::this_thread::yield();});
  while(!locked.load())std::this_thread::yield();
  ckH30(PhysQueuedOriginalCommitH8(original,frame,p.r.in.tick)==PhysQueuedCommitH8::Busy,
    "existing native consumer lock contention returns Busy before any plain ring-slot read");
  drainFixtureH8::Lease lease{};const auto passed=drainFixtureH8::outerContinued;drainFixtureH8::outer(pending,lease,actor.data(),g_last_player_entity.load(),p.r.in.tick);
  ckH30(lease.waitCount==1&&drainFixtureH8::outerContinued==passed&&g_phys_commands6698.tail.load()==tail&&g_phys_commands6698.head.load()==head,
    "contended inspection retains only original contact without popping queue or entering native code");
  auto invalid=pending;++invalid.recovery55c1;drainFixtureH8::Lease refused{};drainFixtureH8::outer(invalid,refused,actor.data(),g_last_player_entity.load(),p.r.in.tick);
  ckH30(!refused.waitCount,"hard recovery mismatch rejects before contended queue inspection can retain anything");
  drainFixtureH8::Lease expired{};drainFixtureH8::outer(pending,expired,actor.data(),g_last_player_entity.load(),p.commit.tick+101);
  ckH30(!expired.waitCount,"Busy cannot extend original command100ms deadline");
  release=true;consumer.join();
  ckH30(PhysQueuedOriginalCommitH8(original,frame,p.r.in.tick)==PhysQueuedCommitH8::ExactQueued,
    "uncontended retry inspects actual exact queued original command without side effects");
 }
 for(unsigned bad=0;bad<18;++bad){PublisherH8 p;p.r.actor=p.r.blocked=true;p.r.step({0,.035,0});p.publish();
  const auto impact=p.r.out.impactRecord178r30[0];auto pending=pendingH8(p,impact);
  std::array<unsigned char,0x220> actor{};void* owner=p.gear.player;std::memcpy(actor.data()+0x1f8,&owner,sizeof owner);
  trackingLiveH8(p);const bool before=drainFixtureH8::OriginalContactOwned19c3(pending,p.r.in.tick);
  outlierH8(p);trackingLiveH8(p);const auto prior=pending;
  switch(bad){case 1:++pending.gesture59c3;break;case 2:++pending.key;break;case 3:++pending.generation;break;case 4:++pending.epoch;break;
   case 5:++pending.request;break;case 6:++pending.recovery55c1;break;case 7:pending.tick=p.r.in.tick;pending.sample=p.r.in.sample;break;
   case 8:pending.metadata59c7g.occurrenceSerialH28=1;break;case 9:pending.metadata59c7g.collider=0;break;case 10:pending.weaponItem=true;break;
   case 11:g_ctx_menu=true;break;case 12:g_gameplay_allowed=false;break;case 13:kcd2vr::animation_filter::nativeBodyOwned112=true;break;
   case 14:g_head_valid=false;g_head_valid_tick=0;break;case 15:se::ledger.clear();break;
   case 16:pending.release51.player=999;break;case 17:pending.tick=p.r.in.tick-161;break;}
  drainFixtureH8::Lease lease{};const auto passed=drainFixtureH8::outerContinued;
  drainFixtureH8::outer(pending,lease,actor.data(),g_last_player_entity.load(),p.r.in.tick);
  ckH30(before&&lease.waitCount==(bad==0)&&drainFixtureH8::outerContinued==passed,
    "actual outer Drain prefix+actual owner callback WAIT only original pre-quarantine same native ancestry; negatives never enter");
  if(!bad){
   ckH30(pending.tick==prior.tick&&pending.sample==prior.sample&&pending.queued==prior.queued&&pending.gesture59c3==prior.gesture59c3&&pending.targetWait19c3,
     "outer WAIT preserves original occurrence clocks and requires existing exact struck-body validation on resume");
   p.r.actor=p.r.blocked=false;p.r.step({0,.035,0});p.publish();trackingLiveH8(p);drainFixtureH8::Lease resumed{};
   drainFixtureH8::outer(pending,resumed,actor.data(),g_last_player_entity.load(),p.r.in.tick);
   ckH30(!resumed.waitCount&&drainFixtureH8::outerContinued==passed+1,
     "readable source resumes queued original contact through unchanged owner predicate without new clocks");

   std::array<unsigned char,0x100> entity{},head{},node{},proxy{};
   const auto ptr=[](const auto& x){return reinterpret_cast<std::uintptr_t>(x.data());};
   const auto put=[](auto& x,std::size_t n,auto v){std::memcpy(x.data()+n,&v,sizeof v);};
   drainFixtureH8::g_base=WhBase();put(entity,0,WhBase()+drainFixtureH8::kEntityVft);put(entity,0xc,pending.entityId);
   put(entity,0xa8,ptr(head));put(head,8,ptr(node));put(node,0x19,std::uint8_t(0));put(node,0x20,unsigned(1));put(node,0x28,ptr(proxy));
   put(proxy,0,WhBase()+drainFixtureH8::impulse596::kPhysicalProxyVft5916c3);put(proxy,0x30,ptr(entity));put(proxy,0x38,pending.metadata59c7g.collider);
   drainFixtureH8::Actor npc{entity.data(),true};
   ckH30(drainFixtureH8::targetCurrent(pending,&npc,actor.data()),"resumed target uses actual unchanged exact entity/proxy/body ownership check");
   put(proxy,0x38,pending.metadata59c7g.collider+1);
   ckH30(!drainFixtureH8::targetCurrent(pending,&npc,actor.data()),"body replaced while waiting cannot substitute current body for original collider");
   put(proxy,0x38,pending.metadata59c7g.collider);put(entity,0xc,pending.entityId+1);
   ckH30(!drainFixtureH8::targetCurrent(pending,&npc,actor.data()),"changed struck entity cannot resume original queued occurrence");
   put(entity,0xc,pending.entityId);npc.coherent=false;
   ckH30(!drainFixtureH8::targetCurrent(pending,&npc,actor.data())&&!drainFixtureH8::targetCurrent(pending,nullptr,actor.data()),"invalid/unknown native target remains refused after source resumption");
   npc.coherent=true;
   const auto native=drainFixtureH8::entered;drainFixtureH8::drain(pending,actor.data(),p.r.in.tick);drainFixtureH8::drain(pending,actor.data(),p.r.in.tick);
   ckH30(drainFixtureH8::entered==native+1,"resumed original contact still consumes unchanged Drain claim only once");
  }
  g_ctx_menu=false;g_gameplay_allowed=true;kcd2vr::animation_filter::nativeBodyOwned112=false;g_head_valid=true;
 }
}
static void extraH8(){
 for(int family:{1,2,3}){PublisherH8 p;const auto recovery=kcd2vr::contact176::recoveryEpoch55c1.load();outlierH8(p,family);
  const auto& q=p.r.out.intentQuarantineH8[0];
  ckH30(q.gesture==p.commit.gesture59c3&&q.failed&&ph::quarantined_intent_h8(p.r.in,p.r.out,p.r.profiles[0],0),"actual kernel finite outlier stamps exact committed ancestry and failed predicate");
  ckH30((!(family&1)||q.modelTip>=40)&&(!(family&2)||q.trackingTip>=40),"diagnostic speeds describe actual failing model/tracked tip cap");
  ckH30(!p.r.out.commandCount&&!p.r.out.impactRecord178r30[0].valid&&!ph::command_authority_h30(p.r.in,p.r.out,p.r.profiles[0],0),"quarantine issues neither new command nor contact nor command authority");
  ckH30(kcd2vr::contact176::recoveryEpoch55c1.load()==recovery,"either-family finite outlier does not roll native cost ancestry");
  const auto since=q.since;const auto committed=p.r.state.weapon[0].committedGestureH26;
  const auto duplicate=ph::step(p.r.state,p.r.in,p.r.profiles,[&](const ph::Profile&,ph::Pose,ph::Pose,bool){return ph::Contact103{};});
  ckH30(duplicate.intentQuarantineH8[0].since==since&&p.r.state.weapon[0].committedGestureH26.tick==committed.tick,"duplicate XR sample neither advances quarantine nor renews original commit");
 }


 for(int family:{1,2,3}){PublisherH8 p;p.r.actor=p.r.blocked=true;outlierH8(p,family);
  ckH30(p.r.out.intentQuarantineH8[0].gesture&&!p.r.out.impactRecord178r30[0].valid&&!p.r.state.weapon[0].impactRecord178r30.valid,
    "colliding uncertain increment cannot export or retain a NEW semantic impact after final publication");
  ckH30(p.r.out.blocked[0]&&p.r.out.contact119[0].known&&p.r.out.contact119[0].plane,
    "uncertain semantic impact suppression preserves actual checked collision stop");
  p.r.actor=p.r.blocked=false;p.r.step({0,.035,0});p.publish();
  ckH30(!p.r.out.impactRecord178r30[0].valid&&!p.r.state.weapon[0].impactRecord178r30.valid,
    "following clear readable sample cannot replay the uncertain contact record");
  ckH30(!p.r.out.impactEventTick178r24[0],"following clear sample cannot replay an uncertain latched semantic event");
 }

 {PublisherH8 p;p.r.in.dynamics=true;p.r.actor=p.r.blocked=true;outlierH8(p);
  ckH30(p.r.out.intentQuarantineH8[0].gesture&&p.r.out.impactSpeed[0]>.25&&p.r.state.weapon[0].lastImpactTick==p.r.in.tick,
    "real checked collision preserves physical haptic magnitude and recoil clock during semantic quarantine");
  ckH30(!p.r.out.impactRecord178r30[0].valid&&!p.r.out.impactEventTick178r24[0]&&p.r.state.weapon[0].unpublishedImpactTickH8==p.r.in.tick,
    "new uncertain recoil event is withheld without rewinding actual physical recoil state");
 }
 {PublisherH8 p;p.r.actor=p.r.blocked=true;p.r.step({0,.035,0});p.publish();const auto original=p.r.out.impactRecord178r30[0];
  outlierH8(p);const auto retained=p.r.state.weapon[0].impactRecord178r30;
  ckH30(original.valid&&sameImpactH30(original,retained)&&std::memcmp(&original,&retained,sizeof original)==0,
    "colliding outlier preserves earlier genuine checked occurrence internally byte-for-byte");
  ckH30(!p.r.out.impactRecord178r30[0].valid,"quarantine does not expose the retained earlier occurrence as current authority");
  p.r.actor=p.r.blocked=false;p.r.step({0,.035,0});p.publish();
  const auto resumed=p.r.out.impactRecord178r30[0];
  ckH30(sameImpactH30(original,resumed)&&std::memcmp(&original,&resumed,sizeof original)==0,
    "clear readable resumption exposes only the prior genuine occurrence with original tick/sample");
 }
 for(int family:{1,2}){PublisherH8 p;
  if(family==1)p.r.in.hands[0].p.y+=.25;else p.r.in.intentHands[0].p.y+=.25;
  p.r.step();p.publish();const auto& q=p.r.out.intentQuarantineH8[0];
  ckH30((q.failed&(family==1?4u:8u))&&q.gesture==p.commit.gesture59c3,"hand-speed sanity outlier is measured and ancestry-only quarantined");
 }
 {RigH30 r(false);for(unsigned i=0;i<10;++i)r.step({0,.025,0});
  ckH30(!r.hits(),"preparation-only negative has not committed any native attack");
  r.in.hands[0].q=r.in.intentHands[0].q=pm::from_rotation_vector({0,0,.75});r.step({0,.025,0});
  ckH30(!r.out.intentQuarantineH8[0].gesture&&!r.state.weapon[0].committedGestureH26.gesture,"motion uncertainty cannot create ancestry before a real COMMIT");
 }
 {PublisherH8 p;p.r.in.modelIntentFrameH8.valid=true;p.r.in.modelIntentFrameH8.stamp.actor=123;
  outlierH8(p);ckH30(!p.r.out.intentQuarantineH8[0].gesture,"metadata-family acquisition cannot be disguised as finite motion uncertainty");
 }

 for(unsigned bad=0;bad<12;++bad){PublisherH8 p;auto& a=p.r.in.modelIntentFrameH8;auto& b=p.r.state.weapon[0].sourceModelFrameH8;
  a.valid=b.valid=true;a.stamp={p.r.in.bodyEpoch,p.r.in.sample+1,p.r.in.tick+10,123};b.stamp={p.r.in.bodyEpoch,p.r.in.sample,p.r.in.tick,123};a.control=b.control=4;
  switch(bad){case 1:++a.stamp.actor;break;case 2:++a.control;break;case 3:++a.stamp.epoch;break;case 4:++a.stamp.sample;break;
   case 5:a.scale=2;break;case 6:a.smooth=true;break;case 7:a.acquisitionSerial=1;break;case 8:a.restOffset.y=.1;break;
   case 9:p.r.in.modelIntentCalibrationH8=pm::from_rotation_vector({0,0,.2});break;case 10:a.calibratedHead.x=std::numeric_limits<double>::quiet_NaN();break;
   case 11:b.stamp.tick-=10;break;}
  outlierH8(p);
  ckH30(bool(p.r.out.intentQuarantineH8[0].gesture)==(bad==0),"quarantine accepts same-source frame metadata only; actor/calibration/owner/family/nonfinite/stale changes cannot donate ancestry");
 }
 for(unsigned bad=0;bad<11;++bad){PublisherH8 p;outlierH8(p);const auto recovery=kcd2vr::contact176::recoveryEpoch55c1.load();
  switch(bad){case 0:p.r.in.allowed=false;break;case 1:p.r.in.tracked[0]=false;break;case 2:p.r.in.intentTracked[0]=false;break;
   case 3:p.r.in.attacks=false;break;case 4:p.r.in.contactFaultH30=true;break;case 5:++p.r.profiles[0].key;break;
   case 6:++p.r.profiles[0].generation;break;case 7:++p.r.in.bodyEpoch;break;case 8:++p.r.in.epoch;break;
   case 9:p.r.in.grip[0]=0;break;case 10:p.r.in.tick+=90;p.r.in.timeNs+=90000000;break;}
  // Publish requires the actual current equipment owner; ownership mismatches
  // still reach recovery retirement because global request remains explicit.
  if(bad==7){g_pose_epoch6689=p.r.in.bodyEpoch;p.gear.epoch=p.r.in.bodyEpoch;}
  if(bad==8){g_weapon_request_epoch6694=p.r.in.epoch;p.gear.request=p.r.in.epoch;}
  p.r.step({0,.025,0});p.publish();
  ckH30(!p.r.state.weapon[0].committedGestureH26.gesture&&!ph::quarantined_intent_h8(p.r.in,p.r.out,p.r.profiles[0],0),"tracking/menu/item/grip/clock hard loss after quarantine clears ancestry");
  ckH30(kcd2vr::contact176::recoveryEpoch55c1.load()!=recovery,"hard loss AFTER quarantine still retires original recovery identity");
 }
 {PublisherH8 p;const auto recovery=kcd2vr::contact176::recoveryEpoch55c1.load();std::uint64_t since=0;
  for(unsigned i=1;i<=9;++i){outlierH8(p,3,.75*i);if(i==1)since=p.r.out.intentQuarantineH8[0].since;
   if(i<=8)ckH30(p.r.out.intentQuarantineH8[0].since==since&&p.r.state.weapon[0].committedGestureH26.gesture==p.commit.gesture59c3,"repeated finite outliers retain first uncertainty clock without renewal");
  }
  ckH30(!p.r.state.weapon[0].committedGestureH26.gesture&&kcd2vr::contact176::recoveryEpoch55c1.load()!=recovery,"uncertainty beyond original80ms source boundary retires rather than freezes stroke");
 }
 {PublisherH8 p;outlierH8(p);auto frame=frameH8(p);auto c=p.commit;PhysNativeOwner6698 native{};
  g_phys_commands6698.head=g_phys_commands6698.tail=0;g_phys_commands6698.push(c);
  ckH30(PhysCommandAuthorityH8(frame,c,native,p.r.in.tick,true,false,false,false)==PhysCommandDecisionH8::Wait,"actual native consumer gate waits on queued original commit during uncertain current frame");
  ph::Command same{};ckH30(g_phys_commands6698.peek(same)&&same.tick==p.commit.tick&&same.sample==p.commit.sample,"actual WAIT leaves original queued input and source clocks unchanged");
  c=p.commit;c.kind=ph::CommandKind::Prepare;
  ckH30(PhysCommandAuthorityH8(frame,c,native,p.r.in.tick,true,false,false,false)==PhysCommandDecisionH8::Wait,"actual native consumer waits for same-source PREPARE without dropping already-proven ancestry");

  p.r.step({0,.035,0});p.publish();frame=frameH8(p);c=p.commit;
  ckH30(PhysCommandAuthorityH8(frame,c,native,p.r.in.tick,true,false,false,false)==PhysCommandDecisionH8::Continue,"actual native consumer continues same original input only after readable current frame");
 }
 {PublisherH8 p;outlierH8(p);auto frame=frameH8(p);PhysNativeOwner6698 native{};
  for(int bad=0;bad<5;++bad){auto c=p.commit;if(bad==0)++c.gesture59c3;if(bad==1)++c.generation;if(bad==2)++c.bodyEpoch;if(bad==3)++c.epoch;
   const auto now=bad==4?p.commit.tick+101:p.r.in.tick;
   g_phys_commands6698.head=g_phys_commands6698.tail=0;g_phys_commands6698.push(c);
   ckH30(PhysCommandAuthorityH8(frame,c,native,now,true,false,false,false)==PhysCommandDecisionH8::Discard&&!g_phys_commands6698.peek(c),"actual native gate refuses changed command ancestry and never waits beyond original100ms");
  }
  auto c=p.commit;c.kind=ph::CommandKind::Abort;
  ckH30(PhysCommandAuthorityH8(frame,c,native,p.r.in.tick,true,true,false,false)==PhysCommandDecisionH8::Continue,"native cancellation is never deferred by quarantine");
 }
 {PublisherH8 p;outlierH8(p);p.r.step({0,.035,0});p.publish();p.r.actor=p.r.blocked=true;p.r.step({0,.035,0});p.publish();
  const auto& a=p.r.out.impactRecord178r30[0];auto recovery=kcd2vr::contact176::recoveryEpoch55c1.load();
  drainFixtureH8::Pending pending{a.gesture59c3,a.key,a.generation,a.epoch,a.request,recovery,a.tick,a.entityId};pending.release51=p.ticket;
  std::array<unsigned char,0x220> actor{};void* owner=p.gear.player;std::memcpy(actor.data()+0x1f8,&owner,sizeof owner);
  const auto count=drainFixtureH8::entered;drainFixtureH8::drain(pending,actor.data(),p.r.in.tick);
  ckH30(a.valid&&drainFixtureH8::entered==count+1,"actual unchanged Drain cost/attempt block accepts restored exact-key checked contact");
  drainFixtureH8::drain(pending,actor.data(),p.r.in.tick);
  ckH30(drainFixtureH8::entered==count+1,"actual Drain once-only reservation refuses a second native-entry leaf for same stroke");
  ckH30(h::costProof54(se::ledger,p.committed,a.tick,a.tick+161,recovery)==h::CostProof54::Refuse,"original contact freshness remains160ms, never extended by uncertainty");
 }
 {PublisherH8 p;outlierH8(p);p.r.step({0,.035,0});p.publish();const auto old=p.commit.gesture59c3;
  for(unsigned i=0;i<30;++i){p.r.step();p.publish();}
  const auto oldHits=p.r.hits();for(unsigned i=0;i<20;++i){p.r.step({0,.035,0});p.publish();}
  ckH30(p.r.hits()>oldHits&&p.r.state.weapon[0].committedGestureH26.gesture!=old,"real quiet/rearm then new forward swing gets a new gesture, not consumed old identity");
 }
 {PublisherH8 p;outlierH8(p);p.r.step({0,.035,0});p.publish();p.r.step({0,-.12,0});p.publish();
  ckH30(!p.r.state.weapon[0].committedGestureH26.gesture&&!p.r.out.intentQuarantineH8[0].gesture,"genuine backwards windup retires forward ancestry instead of being quarantined");
 }
}
int main(){
 std::vector<unsigned char> image(0x5400000);hostModuleHandleW=image.data();g_wh_pinned104=0;hostTickCount64=1000;
 const auto base=WhBase();kcd2vr::retail_build112::hostRetailVerified112=true;g_saved_arm_ready66100=true;g_arm_capture_active66100=false;
 g_native_contract104=g_input_contract117=g_combat_contract101=base;g_phys_armed6698=true;
 g_prediction_contract120=g_bow_zoom_contract121=g_attack_timing_contract22=base;g_close_combat_ready175=g_master_gate_ready168=true;
 g_combat_input_original101=+[](void*,const std::uint64_t*,int,float){};SetStagedBowDefault59(false);
 g_last_player_entity=reinterpret_cast<void*>(0x789);
 ckH30(ApplyUserMode175(kcd2vr::user175::Mode::CombatSynchronized),"actual public HYBRID mode is installed with synthetic native installer witnesses");
 PublisherH8 p;ckH30(p.commit.gesture59c3&&p.r.hits()==1&&p.ticket.serial,"actual producer publishes one committed attempt and exact native-dispatch release ticket");
 const auto originalRecovery=kcd2vr::contact176::recoveryEpoch55c1.load();const auto originalCommit=p.commit;
 const auto before=p.r.state.weapon[0].committedGestureH26;
 p.r.in.hands[0].q=p.r.in.intentHands[0].q=pm::from_rotation_vector({0,0,.75});
 p.r.step({0,.025,0});p.publish();
 std::printf("OUTLIER sourceCurrent=%d beforeRecovery=%llu afterRecovery=%llu gestureBefore=%llu gestureAfter=%llu modelTip=%.3f trackingTip=%.3f\n",p.r.out.normalIntentH30[0].current,(unsigned long long)originalRecovery,(unsigned long long)kcd2vr::contact176::recoveryEpoch55c1.load(),(unsigned long long)before.gesture,(unsigned long long)p.r.state.weapon[0].committedGestureH26.gesture,p.r.out.intentQuarantineH8[0].modelTip,p.r.out.intentQuarantineH8[0].trackingTip);
 ckH30(!p.r.out.normalIntentH30[0].current&&!ph::command_authority_h30(p.r.in,p.r.out,p.r.profiles[0],0),"uncertain motion frame receives no current command authority");
 ckH30(kcd2vr::contact176::recoveryEpoch55c1.load()==originalRecovery,"one finite speed outlier does not retire admitted native cost/release ancestry");
 p.r.step({0,.035,0});p.publish();p.r.actor=p.r.blocked=true;p.r.step({0,.035,0});p.publish();
 const auto& impact=p.r.out.impactRecord178r30[0];
 ckH30(impact.valid&&impact.gesture59c3==p.committed.gesture,"next fresh checked contact retains the original committed gesture");
 const auto now=p.r.in.tick;const auto recovery=kcd2vr::contact176::recoveryEpoch55c1.load();
 const ct::Key joined{p.committed.gesture,p.committed.weapon,p.committed.generation,p.committed.body,p.committed.request,recovery};
 kcd2vr::contact176::Ticket exact{};const bool found=ct::releases.find(joined,exact);
 ckH30(found&&ct::join(joined,exact,impact.tick,now,recovery)==ct::Join::Bound,"actual contact key joins original release instead of NO_RELEASE after global recovery-key rollover");
 mo::Attempt claim{};auto key=p.committed;key.recovery=recovery;const auto claimed=mo::hybridAttempts54.claim(key,impact.tick,now,13,claim);
 ckH30(claimed==mo::Claim::Ready,"actual producer attempt ledger admits one same-owner restored contact");
 ckH30(h::costProof54(se::ledger,key,impact.tick,now,recovery)==h::CostProof54::Ready,"actual effort publisher joins original native completion without another debit");
 ckH30(p.nativeCompletions==1&&p.commit.tick==originalCommit.tick&&exact.tick==p.ticket.tick,"original native cost and release clocks are never renewed");
 extraH8();outerH8();std::printf("SUMMARY checks=%u failures=%u native_calls=NONE\n",checksH30,failuresH30);return failuresH30?1:0;
}
