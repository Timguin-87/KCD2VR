#include "../src/vrik/passive_contact169r1.h"
#include "../src/diagnostics/passive_contact169r1.h"
#include "../src/vrik/physical_weapon.h"
#include <cstdio>
#include <limits>
using namespace kcd2vr;
namespace p=passive169r1;namespace tr=diagnostics::passive169r1;namespace cap=diagnostics::capture168;
static int checks=0,failed=0;
static void ck(bool v,const char* n){++checks;if(!v)++failed;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
static p::Sample sample(){p::Sample x{};x.actor=1;x.key=11;x.generation=x.epoch=x.request=x.sequence=1;x.tick=1000;
 x.eligible=x.worldCoherent=true;x.normal={-1,0,0};x.status=p::Status::Clear;return x;}
static void next(p::Sample& x,int ms=10){++x.sequence;x.tick+=ms;}
int main(){
 {p::State s{};auto x=sample();auto d=p::update(s,x);ck(!d.amplitude,"initial clear sample produces no contact cue");
  next(x);x.status=p::Status::Contact;x.requested.x=.01;d=p::update(s,x);ck(d.phase==p::Phase::Begin&&d.amplitude>0,"fresh ordinary touch needs no attack/damage result");
  const auto episode=d.episode;d=p::update(s,x);ck(d.phase==p::Phase::None&&d.amplitude==0,"same sample/second eye does not duplicate feedback");
  for(int i=0;i<100;++i){next(x);d=p::update(s,x);ck(d.amplitude==0,"stationary pressure stays quiet");}
  for(int i=0;i<10;++i){next(x);x.requested.x+=.01;d=p::update(s,x);ck(d.amplitude==0&&d.motion.slipDistance==0,"pushing deeper without allowed travel cannot become scrape");}
  int cues=0;for(int i=0;i<50;++i){next(x);x.requested.z+=.01;x.allowed.z+=.01;d=p::update(s,x);if(d.amplitude)cues++;}
  ck(cues>=3&&cues<=6&&s.episode==episode,"real permitted sliding yields bounded texture in one contact episode");
  next(x);x.status=p::Status::Unknown;d=p::update(s,x);ck(d.phase==p::Phase::Suspended&&!d.amplitude,"UNKNOWN stops feedback, not a synthetic release/hit");
  next(x);x.status=p::Status::Contact;x.requested.x+=.02;d=p::update(s,x);ck(d.phase==p::Phase::Resumed&&!d.amplitude,"recovery from UNKNOWN does not invent impact");
  next(x);x.status=p::Status::Clear;d=p::update(s,x);ck(s.contact&&!d.amplitude,"one clear flicker does not end/re-arm contact");
  next(x);x.status=p::Status::Contact;d=p::update(s,x);ck(d.phase!=p::Phase::Begin&&!d.amplitude,"query chatter cannot repeatedly tap");
  next(x);x.status=p::Status::Clear;(void)p::update(s,x);next(x,40);d=p::update(s,x);ck(d.phase==p::Phase::End&&!s.contact,"verified separation ends contact after hysteresis");
 }
 {p::State s{};auto x=sample();(void)p::update(s,x);next(x);x.status=p::Status::Contact;x.requested.x=.01;(void)p::update(s,x);
  next(x);x.worldCoherent=false;auto d=p::update(s,x);ck(!d.amplitude&&d.phase==p::Phase::Suspended,"changed world/pose composition cannot trigger feedback");
  next(x);x.worldCoherent=true;x.normal={};d=p::update(s,x);ck(!d.amplitude,"invalid contact normal cannot trigger feedback");
  next(x);x.eligible=false;d=p::update(s,x);ck(d.phase==p::Phase::End&&!d.amplitude,"ownership or tracking loss clears contact");
  next(x);x.eligible=true;x.normal={-1,0,0};x.actor=2;x.requested.x=5;d=p::update(s,x);ck(!d.amplitude,"new actor cannot inherit old velocity");
  next(x,200);x.requested.x+=1;d=p::update(s,x);ck(!d.amplitude&&!d.motion.valid,"long tracking gaps do not become velocity");
  next(x);x.requested.x=std::numeric_limits<double>::quiet_NaN();d=p::update(s,x);ck(!d.amplitude,"NaN cannot produce a pulse");
 }
 {p::State s{};auto x=sample();x.source=p::Source::SelfTorso;(void)p::update(s,x);next(x);x.status=p::Status::Contact;x.requested.x=.01;auto d=p::update(s,x);
  ck(d.amplitude>0,"verified final self-proxy touch can give a soft cue");
  for(int i=0;i<30;++i){next(x);x.point.x+=.04;d=p::update(s,x);}
  ck(!d.amplitude&&d.motion.slipDistance==0,"moving Henry/world point alone cannot generate self friction");
 }
 {p::State s{};auto x=sample();x.source=p::Source::HandWorld;x.feature=0;(void)p::update(s,x);
  next(x);x.feature=5;x.status=p::Status::Contact;x.allowed.z=.1;auto d=p::update(s,x);
  ck(!d.amplitude&&!d.motion.valid,"switching fingertip identity is not contact velocity");
 }
 {cap::gate.select(cap::Domain::Combat,false,1000);cap::gate.select(cap::Domain::Interaction,false,1000);
  tr::Record r{};while(tr::pop(r)){}tr::monitors={};haptics101::select(true,1000);haptics101::allowed=true;
  auto x=sample();tr::observe(x);next(x);x.status=p::Status::Contact;x.requested.x=.01;tr::observe(x);
  haptics101::Pulse pulse{};ck(haptics101::queue[0].read(pulse)&&pulse.key==11&&pulse.amplitude>0,"contact feedback works with all diagnostics OFF");
  ck(!tr::pop(r),"capture OFF enqueues no contact records");
  haptics101::publish_event(0,99,.7f,60,x.tick);ck(!tr::cue(0,11,.1f,20,x.tick+1),"passive scrape cannot overwrite stronger native/holster feedback");
  cap::gate.select(cap::Domain::Combat,true,x.tick);next(x);tr::observe(x);ck(tr::pop(r)&&r.kind==0&&r.combatSession>0,"new capture snapshots current contact without mode change");
  unsigned char raw[80]{};raw[3]=42;x.source=p::Source::WeaponWorld;
  {tr::RayScope scope(x);const auto id=tr::ray(raw,{0,0,0},{1,0,0},.3f);ck(id>0,"raw witness ID is published for an existing validated ray");
   ck(tr::ray(raw,{},{1,0,0},.3f)==0,"raw witnesses are bounded, not every ray every frame");}
  ck(tr::pop(r)&&r.kind==1&&r.raw[3]==42&&r.serial>0,"witness copies 0x50 raw bytes without dereferencing hit pointers");
  cap::gate.select(cap::Domain::Combat,false,x.tick);next(x);{tr::RayScope scope(x);ck(tr::ray(raw,{},{1,0,0},.3f)==0,"raw capture off adds no records");}
 }
 {using namespace physical;std::array<Profile,2> profiles{};auto& w=profiles[0];w.key=w.generation=1;w.side=0;w.classId=4;w.primary=w.geometry=true;w.tip={0,.5,0};w.axis={0,1,0};
  Input i{};i.allowed=true;i.epoch=i.bodyEpoch=i.sample=1;i.tick=1000;i.tracked[0]=true;i.hands[0].p={.2,.2,1.2};i.shoulders[0]={.2,0,1.2};i.contact=true;i.dynamics=i.attacks=false;i.experiments169=true;
  State s{};auto clear=[](const Profile&,Pose,Pose,bool){return Contact103{};};auto o=step(s,i,profiles,clear);const auto before=o.weapons[0];
  // Establish fresh CLEAR, then change the path while the query is UNKNOWN.
  ++i.sample;i.tick+=10;o=step(s,i,profiles,clear);
  auto unknown=[](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=false;return c;};
  i.hands[0].p.x+=.2;for(int k=0;k<10;++k){++i.sample;i.tick+=10;o=step(s,i,profiles,unknown);}
  ck(pose_core::length(o.weapons[0].p-before.p)<1e-8,"EXPERIMENTS ON cannot creep through UNKNOWN after CLEAR");
  i.wallSlide58=true;i.hands[0].p=before.p;i.hands[0].p.x+=.05;i.hands[0].p.z+=.02;++i.sample;i.tick+=10;
  auto wall=[](const Profile&,Pose a,Pose b,bool){Contact103 c{};if(b.p.x>a.p.x+1e-6){c.fraction=0;c.plane=true;c.normal={-1,0,0};c.point={.2,0,0};c.feature={0,.5,0};c.witness169r1=77;}return c;};
  o=step(s,i,profiles,wall);ck(o.active[0]&&o.blocked[0]&&o.contactMotion169r1[0].valid,"committed wall contact has geometry-only motion evidence");
  ck(o.contactMotion169r1[0].slipDistance>.015&&o.contact119[0].witness169r1==77,"checked tangent slide survives and carries its exact witness");
  ck(o.commandCount==0,"wall slide is not a synthetic native attack");
 }
 std::printf("passive169r1=%d/%d\n",checks-failed,checks);return failed?1:0;
}
