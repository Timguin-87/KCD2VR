#include "../src/diagnostics/hit_chain_c54r3.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <limits>
namespace hc=kcd2vr::diagnostics::hit_chain_c54r3;
namespace ph=kcd2vr::physical;
namespace cap=kcd2vr::diagnostics::capture168;
static int checks=0,failures=0;
static void check(bool b,const char* why){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",why);}
static auto clear=[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.;};
static bool same(const ph::Pose& a,const ph::Pose& b){return ph::m::length(a.p-b.p)<1e-12&&ph::m::angle(a.q,b.q)<1e-12;}
static void drain(hc::Recorder& r){hc::Event e{};while(r.pop(e)){};}
int main(){
 ph::Input in{};in.allowed=true;in.epoch=3;in.bodyEpoch=5;in.tick=1000;in.sample=1;in.grip[0]=1;
 in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.hands[0].p={.2,.3,1.3};in.hands[1].p={-.2,.3,1.3};
 in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};in.intentHands[0]=in.hands[0];
 in.dynamics=false;in.contact=false;in.haveWorld=true;in.world.p={10,20,30};in.worldScale=1.2;
 in.requirePrimaryGrip114=true;in.coherentMelee119=in.naturalCombat117=true;
 std::array<ph::Profile,2> profiles{};auto& p=profiles[0];p.side=0;p.key=77;p.generation=8;p.classId=4;
 p.primary=p.geometry=true;p.tip={0,.9,0};p.axis={0,1,0};p.extent=1;p.weight=1;
 ph::State off{},on{};hc::Recorder observer;hc::Event e{};
 auto out=ph::step(off,in,profiles,clear);on=off;
 auto s=hc::snapshot(in,out,profiles,on,0,2);observer.observe(s,false);
 check(!observer.pop(e)&&observer.dropped.load()==0,"capture off takes no recorder state or event");
 bool sawMoving=false;for(int n=0;n<100;++n){++in.sample;in.tick+=11;
  if(n>10){in.hands[0].p.y+=n<45?.01:-.006;in.intentHands[0]=in.hands[0];}
  auto diagnosticInput=in;diagnosticInput.hitTraceC54R3=true;
  const auto a=ph::step(off,in,profiles,clear),b=ph::step(on,diagnosticInput,profiles,clear);
  observer.observe(hc::snapshot(diagnosticInput,b,profiles,on,0,2),true);
  check(a.commandCount==b.commandCount&&same(a.weapons[0],b.weapons[0])&&a.owned[0]==b.owned[0]&&
    off.weapon[0].gesture59c3==on.weapon[0].gesture59c3&&off.weapon[0].prepared==on.weapon[0].prepared&&
    off.weapon[0].stroke118.active==on.weapon[0].stroke118.active&&off.weapon[0].stroke118.age==on.weapon[0].stroke118.age&&
    a.melee118.reason==b.melee118.reason&&a.strike178.accepted==b.strike178.accepted,"actual production kernel capture off/on has identical gameplay pose/recognizer/command/strike result");
  while(observer.pop(e)){sawMoving|=e.moving>0;check(e.sample.key==77&&e.sample.generation==8&&e.sample.epoch==5&&e.sample.request==3,"copied diagnostic preserves actual producer identity");}
 }
 check(sawMoving,"moving samples without required deliberate onset remain counted");
 hc::Recorder exact;s.session=9;s.tick=3000;s.sample=200;s.motion.reason=kcd2vr::melee118::Reason::Candidate;s.motion.tipSpeed=4;s.liveGesture=true;s.gesture=199;
 exact.observe(s,true);check(exact.pop(e)&&e.onsets==1&&e.sample.gesture==199,"first live production gesture counted once");
 exact.observe(s,true);check(!exact.pop(e),"same sample re-evaluation does not invent a second source observation");
 ++s.sample;s.tick+=11;s.liveGesture=false;s.motion.reason=kcd2vr::melee118::Reason::Withdrawn;
 exact.observe(s,true);check(exact.pop(e)&&e.endedGesture==199&&e.endedWithoutCandidate,"no-contact recognizer withdrawal carries original gesture identity");
 ++s.sample;s.tick+=11;s.liveGesture=true;s.gesture=201;exact.observe(s,true);drain(exact);
 ++s.sample;s.tick+=11;s.liveGesture=false;s.impact.valid=true;s.impact.gesture59c3=201;s.impact.tick=s.tick;s.impact.sample=s.sample;
 exact.observe(s,true);check(exact.pop(e)&&!e.endedWithoutCandidate,"same-sample terminal impact cannot be labelled no surface candidate");
 ++s.sample;s.tick+=1100;s.impact={};s.motion.reason=kcd2vr::melee118::Reason::Slow;exact.observe(s,true);
 check(exact.pop(e)&&e.summary&&e.reasons[unsigned(kcd2vr::melee118::Reason::Slow)]==1,"one-second summary includes actual below-speed reasons rather than only successful commands");
 s.session=10;s.sample=1;s.tick=5000;exact.observe(s,true);check(exact.pop(e)&&e.steps==1,"new capture session resets observer counts without changing source state");
 s.flags=hc::Allowed|hc::Tracked|hc::Active|hc::Owned|hc::Attacks|hc::IntentTracked|hc::HandlingOnly;
 check(!std::strcmp(hc::gate(s),"HANDLING_RECOVERY_NO_ATTACK_AUTHORITY"),"handling-only display is identified separately from damage authority");
 s.normalOwnerCurrent=true;
 check(!std::strcmp(hc::gate(s),"POSE_RECOVERY_INTENT_OWNED_NEW_CONTACT_DISABLED"),"retained normal intent is reported without claiming new collision permission");
 s.normalHybridH31=true;s.legacyIntentR16=false;s.normalIntentEnabled=true;
 hc::Recorder full;for(unsigned n=0;n<100;++n){s.sample=100+n;s.tick=6000+n*11;s.commands=1;full.observe(s,true);}
 check(full.dropped.load()==37,"bounded 63-event queue reports every overflow rather than claiming no event");
 cap::gate.select(cap::Domain::Combat,true,1000);const auto session=cap::gate.session[0].load();
 s.session=session;s.tick=1100;s.sample=999;s.commands=1;s.side=0;s.contact.known=true;s.contact.plane=true;s.flags|=hc::World;
 hc::recorder.observe(s,true);std::vector<std::string> lines;hc::drain(1101,[&](const char* p){lines.emplace_back(p);});
 const auto hasFamily=[&](const char* family){for(const auto& row:lines)if(row.find(family)!=std::string::npos)return true;return false;};
 check(hasFamily("stage=PRODUCER")&&hasFamily("stage=BEFORE_RECOGNIZER")&&hasFamily("stage=GEOMETRY")&&hasFamily("stage=BLADE_ENDPOINTS")&&hasFamily("H29 STRIKE_ORIGIN"),"actual production formatter exposes classifier, origin, and requested/allowed blade geometry");
 check(hasFamily("normalHybridH31=1 legacyIntentR16=0 normalIntentEnabled=1 normalOwnerCurrent=1"),"F3 reports the actual public HYBRID producer separately from the retired R16 route");
 bool bounded=true;for(const auto& line:lines)bounded&=line.size()<2047;check(bounded,"full records fit their bounded formatter buffer");
 ++s.sample;hc::recorder.observe(s,true);cap::gate.select(cap::Domain::Combat,false,1200);lines.clear();hc::drain(1201,[&](const char* p){lines.emplace_back(p);});
 check(lines.size()>=1&&lines[0].find("detail=HIT_CHAIN_HEALTH")!=std::string::npos&&hc::recorder.discarded.load()>0,"stopping F3 discards queued detail and emits only bounded essential end health");
 ++s.sample;s.session=session;hc::recorder.observe(s,true);cap::gate.select(cap::Domain::Combat,true,1300);lines.clear();hc::drain(1301,[&](const char* p){lines.emplace_back(p);});
 check(lines.empty(),"new capture never receives queued old-session detail");
 s.session=cap::gate.session[0].load();++s.sample;s.tick=1400;s.motion.handSpeed=s.motion.tipSpeed=s.motion.rotSpeed=s.motion.peak=s.motion.progress=s.motion.fastTravel=s.motion.rotTravel=s.motion.coherence=s.motion.age=std::numeric_limits<double>::max();
 const auto clipped=hc::recorder.formatterClipped.load();hc::recorder.observe(s,true);hc::drain(1401,[](const char*){});
 check(hc::recorder.formatterClipped.load()>clipped,"formatter clipping is explicitly counted for extreme finite diagnostic values");
 ph::State retired{};retired.sample=501;retired.bodyEpoch=22;retired.epoch=23;
 auto& rw=retired.weapon[1];rw.gesture59c3=902;rw.key=77;rw.generation=8;rw.committedGestureH26.gesture=902;rw.committedGestureH26.tick=1380;
 lines.clear();hc::recorder.retire(retired,"NATIVE_SOLVER_ADMISSION_CHANGED",321,1402);hc::drain(1403,[&](const char* p){lines.emplace_back(p);});
 check(lines.size()==1&&hasFamily("side=1 gesture=902 committedGesture=902")&&hasFamily("reason=NATIVE_SOLVER_ADMISSION_CHANGED sourceLine=321")&&hasFamily("epoch=22 request=23"),"hard retirement records exact hand, immutable stroke and failing ownership boundary");
 check(rw.gesture59c3==902&&rw.committedGestureH26.tick==1380,"retirement observation cannot clear or refresh gameplay state");
 cap::gate.select(cap::Domain::Combat,false,1404);hc::recorder.retire(retired,"OFF",0,1405);check(!hc::recorder.pop(e),"retirement recorder remains inert with F3 off");
 {
  hc::Recorder empty;hc::Snapshot a{};a.session=21;a.side=0;a.sample=1;a.tick=10000;
  a.flags=hc::Allowed|hc::Tracked;a.invalid=1;a.reset.reason=1;
  unsigned emitted=0;for(unsigned n=0;n<90;++n){a.sample=n+1;a.tick=10000+n*11;a.flags^=hc::NativeKnown;empty.observe(a,true);while(empty.pop(e))++emitted;}
  std::array<hc::Lane,2> stats{};check(empty.stats(stats)&&stats[0].steps==90&&stats[0].suppressed==89&&emitted==1,
   "unchanged missing profile counts every sample but emits once despite unrelated native-observation flag changes");
  const auto edge=[&](const char* why){++a.sample;a.tick+=11;empty.observe(a,true);check(empty.pop(e)&&!empty.pop(e),why);};
  ++a.generation;edge("missing-profile generation replacement keeps its first reset");
  ++a.epoch;edge("missing-profile body replacement keeps its first reset");
  ++a.request;edge("missing-profile request replacement keeps its first reset");
  ++a.invalid;edge("changed missing-profile invalid reason remains an immediate edge");
  a.flags|=hc::NativeBusy;++a.sample;a.tick+=11;empty.observe(a,true);check(!empty.pop(e),"unrelated native-observation flags do not restart the missing-profile reset occurrence");
  a.motion.reason=kcd2vr::melee118::Reason::Unavailable;edge("changed actual classifier reason remains an immediate edge");
  ++a.reset.reason;edge("changed actual kernel reset mask remains an immediate edge");
  ++a.reset.priorGesture;edge("retirement of a different original gesture remains an immediate edge");
  ++a.reset.priorCommit;edge("retirement of a different original commit remains an immediate edge");
  a.tick+=1000;edge("unchanged empty profile still emits a periodic summary");
  a.commands=1;edge("actual command edge is never coalesced with an empty-profile reset");
  edge("another actual command edge remains visible on the next sample");a.commands=0;
  ++a.session;edge("new capture session retains its first empty-profile reset");
  a.key=77;edge("a real held profile keeps its actual reset edge");
  edge("repeated real held-profile resets retain existing per-occurrence details");
  a.key=0;edge("returning to an empty hand starts a new empty-profile occurrence");
  ++a.sample;a.tick+=11;empty.observe(a,false);check(!empty.pop(e),"capture-off does not advance or enqueue empty-profile observer state");
 }
 std::printf("checks=%d failures=%d\n",checks,failures);return failures?1:0;
}
