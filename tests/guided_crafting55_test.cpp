// Production final input adapter, actual selected-entity/hint mailboxes and
// committed native attachment witness, rather than Boolean "touch succeeds".
#include "vrik/semi_workstations46.h"
#include "vrik/workstation178r20c1.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <mutex>
namespace kcd2vr::interaction_alignment {
using RaySample=interaction112::Ray;
static std::uint64_t clock55=0,g_door_generation38=8;
static std::uint64_t GetTickCount64() noexcept {return clock55;}
static std::atomic<bool> g_enabled{true},g_split_ready57c1{true},g_hand_ray_active{true},g_visual_fallback_enabled{false};
static std::mutex g_hand_candidate_mutex178r14;static HandCandidate g_hand_candidate178r14;
static core::SnapshotMailbox<RaySample> g_hand_packets112;
static core::SnapshotMailbox<palm59c9::Pair> g_palm_packets59c9;
struct Candidate57c1 {std::uint32_t id=0;char cls[32]{};};
#include "generated_guided_candidate55.inc"
static HandCandidate offer55(std::uint64_t now,int side,std::uint32_t entity,float distance,std::uint64_t epoch){
 clock55=now;RaySample ray{};ray.tick=now;ray.hand=true;ray.owner=side;ray.sourcePlayer=55;ray.sourceEpoch=epoch;
 ray.oz=-.3f;ray.y=1;g_hand_packets112.publish(ray);palm59c9::Pair pair{};pair.eligible[side]=true;pair.ray[side]=ray;
 g_palm_packets59c9.publish(pair);Candidate57c1 native{};native.id=entity;std::strcpy(native.cls,"AlchemyTable");
 const float point[4]{0,0,0,distance};ProductionOffer55(true,now,point,ray,true,true,false,native,g_door_generation38);
 HandCandidate out{};(void)LastHandCandidate(out);return out;
}
}
using namespace kcd2vr;
namespace g=guided_crafting55;namespace sw=semi_workstations46;namespace a=alchemy_semi46;
struct Pad {bool a=false,b=false,x=false;float lx=0,ly=0,rt=0,lt=0;};
static unsigned checks=0;static void ck(bool v){++checks;if(!v)std::fprintf(stderr,"guided check %u failed\n",checks);assert(v);}
static workstation_sample46::Frame frame(std::uint64_t t,std::uint64_t s){
 workstation_sample46::Frame f{};f.sequence=s;f.tick=t;f.timeNs=t*1000000;f.poseEpoch=7;f.sourceEpoch=9;
 f.allowed=f.headValid=true;f.head[1]=1.7f;for(auto& h:f.hand){h.valid=true;h.position[1]=1.2f;h.position[2]=-.3f;}return f;
}
static void arm(unsigned kind,std::uint64_t t){
 combat_rules178r10::workstationEnabled=true;combat_rules178r10::f5Rung=3;workstation178r20c1::setExperiment36(true);
 minigame_hands45::mode=minigame_hands45::Native;minigame_hands45::publish(kind,t);
}
static void current(sw::Context& c,workstation_sample46::Frame& f,a::WorkWitness& w,std::uint64_t t,std::uint64_t s){
 f.tick=t;f.timeNs=t*1000000;f.sequence=s;w.tick=t;a::work.publish(w);minigame_hands45::publish(c.alchemy?1:2,t);
 if(c.alchemy)c.touch=interaction_alignment::offer55(t,c.touch.owner,c.touch.entity,c.touch.distance,f.poseEpoch);
}
static void nativeDock(const sw::Context& c,const workstation_sample46::Frame& f,int side,int type,int kind,float gap=0){
 g::Dock d{};d.instance=c.alchemy?c.alchemy:c.smith;d.lifetime=c.lifetime;d.character=55;d.attachment=66;d.object=77;
 d.entity=side?101:102;d.side=side;d.type=type;d.kind=kind;d.phase=c.smithPhase;d.gap=gap;
 d.tick=f.tick;d.sequence=f.sequence;d.poseEpoch=f.poseEpoch;d.sourceEpoch=f.sourceEpoch;g::dock[side].publish(d);
}
// Every sample goes through the actual current Dock -> guided Smith -> final pad
// bridge. A request remains a native X input; the fixture never fabricates ACK.
struct SmithRetryFixture57 {
 sw::State state;sw::Context context{};workstation_sample46::Frame f=frame(4000,1);
 a::WorkWitness work{};std::uint64_t tick=4000,seq=1;Pad pad{};
 SmithRetryFixture57(){context.enabled=context.qualified=context.guided=context.smithTracked=true;
  context.smith=11;context.lifetime=21;context.smithPhase=11;}
 Pad sample(float grip,float roll=0,int phase=11,bool valid=true,std::uint64_t advance=20,bool newSequence=true,float pitch=0){
  tick+=advance;if(newSequence)++seq;f.hand[1].valid=valid;f.hand[1].grip=grip;
  f.hand[1].rotation[0]=std::sin(pitch*.5f)*std::cos(roll*.5f);
  f.hand[1].rotation[1]=-std::sin(pitch*.5f)*std::sin(roll*.5f);
  f.hand[1].rotation[2]=std::cos(pitch*.5f)*std::sin(roll*.5f);
  f.hand[1].rotation[3]=std::cos(pitch*.5f)*std::cos(roll*.5f);
  context.smithPhase=phase;arm(minigame_hands45::Smithing,tick);current(context,f,work,tick,seq);
  nativeDock(context,f,1,1,2);pad={};state.apply(context,f,tick,pad);return pad;
 }
 void request(){ck(!sample(0).x);ck(!sample(1).x);
  for(int i=0;i<=5;++i)ck(sample(1,.8f).x==(i==5));
  ck(state.guidedSmith.flipConsumed&&!state.guidedSmith.flipAcknowledged);
 }
 void freshTwist(bool requestExpected){for(int i=0;i<=5;++i){const auto p=sample(1,.8f);
  if(requestExpected&&i==5&&!p.x)std::fprintf(stderr,"SMITH_RETRY57: fresh explicit second twist refused; consumed=%d ack=%d tick=%llu seq=%llu\n",
    state.guidedSmith.flipConsumed,state.guidedSmith.flipAcknowledged,
    static_cast<unsigned long long>(tick),static_cast<unsigned long long>(seq));
  ck(p.x==(requestExpected&&i==5));}}
};
int main(){
 a::hooksReady=true;a::producer.reset();a::work.publish({});a::control.publish({});
 {
  sw::State s;sw::Context c{};c.enabled=c.qualified=c.guided=c.alchemyTracked=true;c.alchemy=10;c.lifetime=20;
  auto f=frame(1000,1);arm(minigame_hands45::Alchemy,1000);a::WorkWitness w{};
  c.touch.object=true;c.touch.owner=1;c.touch.entity=101;c.touch.distance=.06f;
  c.touch.sourceGeneration=8;c.touch.sourceEpoch=7;c.touch.sourcePlayer=55;
  current(c,f,w,1000,1);ck(c.touch.sourceGeneration==8&&c.touch.sourceEpoch==7&&c.touch.sourcePlayer==55&&c.touch.sourceTick==1000);
  Pad p;s.apply(c,f,1000,p);ck(!p.a&&s.lastGuided==g::Status::NeutralRequired);
  // Native selected object and a current native hold/release hint are BOTH required.
  alchemy_input19c4::selected.publish({10,20,1020,101});
  alchemy_input19c4::observed[13].publish({10,20,1020,101,13,false});
  f.hand[1].grip=1;c.touch.entity=999;current(c,f,w,1020,2);p={};s.apply(c,f,1020,p);ck(!p.a);
  c.touch.entity=101;current(c,f,w,1040,3);p={};s.apply(c,f,1040,p);ck(!p.a); // no retry in one squeeze
  f.hand[1].grip=0;current(c,f,w,1060,4);p={};s.apply(c,f,1060,p);
  f.hand[1].grip=1;current(c,f,w,1080,5);
  alchemy_input19c4::selected.publish({10,20,1080,101});alchemy_input19c4::observed[13].publish({10,20,1080,101,13,false});
  p={};s.apply(c,f,1080,p);ck(p.a&&s.lastGuided==g::Status::NativeActionRequested);
  a::Control control{};ck(a::control.read(control)&&control.enabled&&control.hand==1&&control.totalSeconds==0);
  w.instance=10;w.lifetime=20;w.parent=30;w.child=40;w.token=2;w.kind=a::Resource;w.phase=1;w.working=true;w.hands=2;
  current(c,f,w,1100,6);p={};s.apply(c,f,1100,p);ck(s.lastGuided==g::Status::WaitingNativeGrip);
  ck(a::control.read(control)&&control.totalSeconds==0); // selection alone earns no work
  current(c,f,w,1120,7);nativeDock(c,f,1,13,1);p={};s.apply(c,f,1120,p);
  ck(s.lastGuided==g::Status::GuidedWork&&a::nativeHandOwned(1120,1));
  f.hand[1].rotation[0]=std::sin(.10f);f.hand[1].rotation[3]=std::cos(.10f);
  current(c,f,w,1140,8);nativeDock(c,f,1,13,1);p={};s.apply(c,f,1140,p);
  ck(a::control.read(control)&&control.totalSeconds>.1);const auto earned=control.totalSeconds;
  p={};s.apply(c,f,1140,p);ck(a::control.read(control)&&control.totalSeconds==earned); // no duplicate XR credit
  f.hand[1].rotation[0]=std::sin(.15f);f.hand[1].rotation[3]=std::cos(.15f);
  current(c,f,w,1160,9);nativeDock(c,f,1,13,1,.2f);p={};s.apply(c,f,1160,p);
  ck(a::control.read(control)&&control.totalSeconds==earned&&s.lastGuided==g::Status::WaitingNativeGrip);
  // A from its FIRST press hands the complete action back to native playback.
  current(c,f,w,1180,10);nativeDock(c,f,1,13,1);p={};p.a=true;s.apply(c,f,1180,p);
  ck(p.a&&s.lastGuided==g::Status::NativeButtonPassenger&&a::control.read(control)&&!control.enabled);
  current(c,f,w,1200,11);p={};s.apply(c,f,1200,p);ck(!p.a&&s.lastGuided==g::Status::NativeButtonPassenger);
  w={};current(c,f,w,1220,12);p={};s.apply(c,f,1220,p);
  current(c,f,w,1240,13);p={};s.apply(c,f,1240,p);ck(!p.a); // new neutral required, no stale re-admission
  // No observed native parent must not strand Guided forever after a refused A.
  current(c,f,w,1250,14);p={};p.a=true;s.apply(c,f,1250,p);ck(p.a&&s.lastGuided==g::Status::NativeButtonPassenger);
  f.hand[0].grip=f.hand[1].grip=0;current(c,f,w,2760,15);p={};s.apply(c,f,2760,p);
  ck(!p.a&&s.lastGuided==g::Status::NeutralRequired);
  // A future native selection, missing hint, wrong native hand or stale source fails closed.
  f.hand[1].grip=0;current(c,f,w,2780,16);p={};s.apply(c,f,2780,p);
  f.hand[1].grip=1;current(c,f,w,2800,17);alchemy_input19c4::selected.publish({10,20,2820,101});
  p={};s.apply(c,f,2800,p);ck(!p.a);
  workstation178r20c1::setExperiment36(false);current(c,f,w,2820,18);p={};s.apply(c,f,2820,p);
  ck(!p.a&&a::control.read(control)&&!control.enabled);
 }
 {
  // Invalid/default grip0 cannot count as a physical release after a lost hand.
  sw::State s;sw::Context c{};c.enabled=c.qualified=c.guided=c.alchemyTracked=true;c.alchemy=12;c.lifetime=22;
  c.touch.owner=1;c.touch.entity=101;c.touch.distance=.04f;
  auto f=frame(1800,1);f.hand[1].grip=1;arm(minigame_hands45::Alchemy,1800);a::WorkWitness w{};
  current(c,f,w,1800,1);Pad p;s.apply(c,f,1800,p);ck(!p.a);
  f.hand[1]={};current(c,f,w,1820,2);p={};s.apply(c,f,1820,p);ck(!p.a);
  f.hand[1].valid=true;f.hand[1].grip=1;current(c,f,w,1840,3);
  alchemy_input19c4::selected.publish({12,22,1840,101});alchemy_input19c4::observed[13].publish({12,22,1840,101,13,false});
  p={};s.apply(c,f,1840,p);ck(!p.a);
  f.hand[1].grip=0;current(c,f,w,1860,4);p={};s.apply(c,f,1860,p);ck(!p.a);
  f.hand[1].grip=1;current(c,f,w,1880,5);
  alchemy_input19c4::selected.publish({12,22,1880,101});alchemy_input19c4::observed[13].publish({12,22,1880,101,13,false});
  p={};s.apply(c,f,1880,p);ck(p.a);
  w.instance=999;w.lifetime=666;w.parent=888;current(c,f,w,1900,6);p={};s.apply(c,f,1900,p);
  ck(!s.guidedAlchemy.sawNativeParent); // a global previous station cannot claim this session
 }
 {
  sw::State s;sw::Context c{};c.enabled=c.qualified=c.guided=c.smithTracked=true;c.smith=11;c.lifetime=21;c.smithPhase=11;
  auto f=frame(2000,1);arm(minigame_hands45::Smithing,2000);a::WorkWitness w{};
  current(c,f,w,2000,1);nativeDock(c,f,1,1,2);Pad p;s.apply(c,f,2000,p);ck(!p.lx&&!p.ly&&!p.x);
  f.hand[1].grip=1;current(c,f,w,2020,2);nativeDock(c,f,1,1,2);p={};s.apply(c,f,2020,p);
  f.hand[1].position[0]+=.02f;f.hand[1].position[2]-=.004f;
  current(c,f,w,2040,3);nativeDock(c,f,1,1,2);p={};s.apply(c,f,2040,p);ck(p.lx==0&&p.ly>.6f&&!p.x);
  current(c,f,w,2060,4);nativeDock(c,f,1,1,2);p={};p.lx=.5f;p.ly=-.4f;s.apply(c,f,2060,p);ck(p.lx==.5f&&p.ly==-.4f);
  f.hand[1].rotation[3]=-1;
  for(int i=0;i<7;++i){const auto t=2080+20*i;current(c,f,w,t,5+i);nativeDock(c,f,1,1,2);p={};s.apply(c,f,t,p);ck(!p.x);}
  f.hand[1].rotation[3]=1;current(c,f,w,2220,12);nativeDock(c,f,1,1,2);p={};s.apply(c,f,2220,p);ck(!p.x);
  f.hand[1].rotation[2]=std::sin(.4f);f.hand[1].rotation[3]=std::cos(.4f);
  for(int i=0;i<=5;++i){const auto t=2240+20*i;current(c,f,w,t,13+i);nativeDock(c,f,1,1,2);p={};s.apply(c,f,t,p);ck(p.x==(i==5));}
  current(c,f,w,2360,19);nativeDock(c,f,1,1,2);p={};s.apply(c,f,2360,p);ck(p.x); // same bounded native down pulse, not a second press
  c.smithPhase=12;current(c,f,w,2380,20);nativeDock(c,f,1,1,2);p={};s.apply(c,f,2380,p);ck(!p.x);
  c.smithPhase=11;current(c,f,w,2400,21);nativeDock(c,f,1,1,2);p={};s.apply(c,f,2400,p);ck(!p.x); // acknowledgement alone cannot retrigger
  f.hand[1].rotation[2]=0;f.hand[1].rotation[3]=1;current(c,f,w,2420,22);nativeDock(c,f,1,1,2);p={};s.apply(c,f,2420,p);ck(!p.x);
  f.hand[1].grip=0;f.hand[1].position[2]-=.02f;current(c,f,w,2440,23);nativeDock(c,f,1,1,2);p={};s.apply(c,f,2440,p);ck(!p.ly&&!p.rt);
  c.smithPhase=8;f.hand[1].grip=1;f.hand[1].rotation[2]=std::sin(.4f);f.hand[1].rotation[3]=std::cos(.4f);
  for(int i=0;i<8;++i){const auto t=2460+20*i;current(c,f,w,t,24+i);nativeDock(c,f,1,1,2);p={};s.apply(c,f,t,p);ck(!p.x);}
  p={};p.a=true;p.b=true;p.x=true;p.rt=.8f;p.lx=.6f;p.ly=-.3f;s.apply(c,f,f.tick,p);
  ck(p.a&&p.b&&p.x&&p.rt==.8f&&p.lx==.6f&&p.ly==-.3f);
  c.qualified=false;p={};s.apply(c,f,f.tick,p);ck(!p.a&&!p.x&&!p.rt&&!p.lx&&!p.ly);
 }
 {
  SmithRetryFixture57 h;h.request();
  // A rejected first X must not strand every future explicit wrist gesture.
  for(int i=0;i<10;++i)ck(h.sample(1,.8f).x==(i<3));
  for(int i=0;i<4;++i)ck(!h.sample(1,0).x); // neutral while held cannot retry
  ck(!h.sample(0,0).x);ck(!h.sample(1,0).x); // explicit release/new clutch is not X
  h.freshTwist(true);
 }
 {
  SmithRetryFixture57 h;h.request();
  ck(!h.sample(0,0).x);ck(!h.sample(1,0).x); // re-grip before old 70 ms pulse ends
  for(int i=0;i<10;++i)h.sample(1,0);
  h.freshTwist(false); // old held squeeze never automatically replays
  ck(!h.sample(0,0).x);ck(!h.sample(1,0).x);h.freshTwist(true);
 }
 {
  SmithRetryFixture57 h;h.request();for(int i=0;i<6;++i)h.sample(1,.8f);
  ck(!h.sample(0,0,11,false).x); // invalid hand's default Grip0 is not release
  ck(!h.sample(1,0).x);h.freshTwist(false);
  ck(!h.sample(0,0).x);ck(!h.sample(1,0).x);h.freshTwist(true);
 }
 {
  SmithRetryFixture57 h;h.request();for(int i=0;i<6;++i)h.sample(1,.8f);
  ck(!h.sample(0,.8f).x);ck(!h.sample(1,0).x);h.freshTwist(false); // no neutral release
  ck(!h.sample(0,0).x);ck(!h.sample(1,0,11,true,1020).x);h.freshTwist(false); // expired proof
  ck(!h.sample(0,0).x);ck(!h.sample(1,0).x);h.freshTwist(true);
 }
 {
  SmithRetryFixture57 h;h.request();for(int i=0;i<6;++i)h.sample(1,.8f);
  ck(!h.sample(0,0).x);ck(!h.sample(1,0,11,true,0,false).x);h.freshTwist(false); // duplicate XR cannot re-grip
  ck(!h.sample(0,0).x);ck(!h.sample(1,0).x);h.freshTwist(true);
 }
 {
  SmithRetryFixture57 h;h.request();for(int i=0;i<6;++i)h.sample(1,.8f);
  ck(!h.sample(0,0,8).x);ck(!h.sample(1,0).x);h.freshTwist(false); // not the anvil phase
  ck(!h.sample(0,0).x);ck(!h.sample(1,0,8).x);h.freshTwist(false); // no re-clutch in another phase
  ck(!h.sample(0,0).x);ck(!h.sample(1,0).x);h.freshTwist(true);
 }
 {
  SmithRetryFixture57 h;h.request();for(int i=0;i<6;++i)h.sample(1,.8f);
  ck(!h.sample(0,0).x);ck(!h.sample(1,0,11,true,1000).x);h.freshTwist(true); // inclusive bound
 }
 {
  SmithRetryFixture57 h;h.request();for(int i=0;i<6;++i)h.sample(1,.8f);
  ck(!h.sample(0,0).x); // valid pitch is not a neutral wrist
  ck(!h.sample(1,0,11,true,20,true,.8f).x);h.freshTwist(false);
  ck(!h.sample(0,0).x);ck(!h.sample(1,0).x);h.freshTwist(true);
 }
 {
  // Normal native acknowledgement still requires phase return plus neutral,
  // and cannot be converted into a refused-attempt retry by release/re-grip.
  SmithRetryFixture57 h;h.request();ck(!h.sample(1,.8f,12).x);
  ck(h.state.guidedSmith.flipAcknowledged);
  ck(!h.sample(1,.8f).x);ck(h.state.guidedSmith.flipConsumed);
  ck(!h.sample(1,0).x);
  ck(!h.state.guidedSmith.flipConsumed);h.freshTwist(true);
 }
 {
  auto candidate=interaction_alignment::offer55(3000,0,77,.04f,7);ck(candidate.sourceEpoch==7);
  ++interaction_alignment::g_door_generation38;ck(!interaction_alignment::LastHandCandidate(candidate)&&!candidate.tick);
  candidate=interaction_alignment::offer55(3020,0,77,.04f,7);ck(candidate.tick==3020);
  interaction_alignment::g_enabled=false;ck(!interaction_alignment::LastHandCandidate(candidate));interaction_alignment::g_enabled=true;
  candidate=interaction_alignment::offer55(3040,0,77,.04f,7);ck(candidate.tick==3040);
  interaction_alignment::clock55=3121;ck(!interaction_alignment::LastHandCandidate(candidate));
 }
 std::printf("guided_crafting55: %u actual selected/hint/attachment-to-pad, clocks, Grip/phase/manual/expiry checks PASS\n",checks);
}
