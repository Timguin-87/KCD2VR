#include "../src/diagnostics/punch_path_c54r3.h"
#include <cmath>
#include <cstring>
#include <limits>
#include <cstdio>
namespace fp=kcd2vr::diagnostics::punch_c54r3;
namespace fm=kcd2vr::fists;
namespace cap=kcd2vr::diagnostics::capture168;
static int total=0,failed=0;
static void check(bool ok,const char* text){++total;failed+=!ok;std::printf("%s %s\n",ok?"PASS":"FAIL",text);}
static fm::Sample input(){fm::Sample s{};s.epoch=s.sequence=1;s.tick=1000;s.actor=123;s.valid=s.empty[0]=s.empty[1]=true;s.boxing167=true;s.head={0,1.6,0};s.controller[0]=s.allowed[0]={.2,1.25,-.2};s.controller[1]=s.allowed[1]={-.2,1.25,-.2};return s;}
static fm::Punch next(fm::State& state,fm::Sample& s,fm::Trace& trace,int dt=10){++s.sequence;s.tick+=dt;return fm::step(state,s,&trace);}
static void ready(fm::State& state,fm::Sample& s,fm::Trace& t){fm::step(state,s,&t);s.grip[0]=1;s.trigger[0]=1;for(int i=0;i<20;++i)next(state,s,t);}
static bool vectorEqual(fm::pc::Vec3 a,fm::pc::Vec3 b){return a.x==b.x&&a.y==b.y&&a.z==b.z;}
static bool stateEqual(const fm::State& a,const fm::State& b){if(a.actor!=b.actor||a.epoch!=b.epoch||a.sequence!=b.sequence||a.tick!=b.tick||a.timeNs!=b.timeNs)return false;for(int i=0;i<2;++i){const auto& x=a.hand[i];const auto& y=b.hand[i];if(x.phase!=y.phase||x.released!=y.released||x.quiet!=y.quiet||x.start!=y.start||x.contactTick174!=y.contactTick174||!vectorEqual(x.xr,y.xr)||!vectorEqual(x.relative,y.relative)||!vectorEqual(x.allowed,y.allowed)||!vectorEqual(x.origin,y.origin)||!vectorEqual(x.allowedOrigin,y.allowedOrigin)||!vectorEqual(x.direction,y.direction)||!vectorEqual(x.strike,y.strike))return false;}return true;}
int main(){fm::State state{};auto s=input();fm::Trace t{};
 fm::step(state,s,&t);check((t.hand[0].reject&fm::GripOpen)&&!t.hand[0].closed,"open hand reports actual Grip readiness exclusion");
 s=input();s.grip[0]=1;state={};fm::step(state,s,&t);check((t.hand[0].reject&fm::NeedRelease)&&!t.hand[0].released,"held entry names required real Grip release");
 s=input();state={};ready(state,s,t);next(state,s,t);check(t.hand[0].after==fm::Phase::Ready&&(t.hand[0].reject&fm::NoOnset)&&t.hand[0].speed==0&&std::abs(t.hand[0].onsetMin-.4)<1e-8,"no onset reports actual speed and threshold before any contact");
 s.blocked[0]=true;next(state,s,t);check((t.hand[0].reject&fm::BlockedOnset)!=0,"already blocked hand explains why stroke cannot start");s.blocked[0]=false;
 s.controller[0].z-=.012;s.allowed[0].z-=.012;next(state,s,t);check(t.hand[0].after==fm::Phase::Moving&&(t.hand[0].reject&fm::Travel)&&std::abs(t.hand[0].travelMin-.10)<1e-8,"motion onset captures actual travel failure and boxing threshold");
 bool accepted=false;for(int i=0;i<12;++i){s.controller[0].z-=.012;s.allowed[0].z-=.012;accepted|=next(state,s,t).valid;}
 check(accepted&&s.trigger[0]>.60,"traced production recognizer accepts intentional Trigger-and-Grip-qualified punch");
 next(state,s,t);check((t.hand[0].reject&fm::SpentRearm)!=0,"follow-through names retraction rearm requirement");
 check(t.hand[0].retractMin==-.07&&t.hand[0].rearmQuietMin==60,"boxing retraction and quiet thresholds come from current sample");
 s.boxing167=false;next(state,s,t);check(t.hand[0].retractMin==-.055&&t.hand[0].rearmQuietMin==120,"legacy retraction and quiet thresholds are not mislabelled as boxing");
 s=input();state={};ready(state,s,t);s.controller[0].z-=1;s.allowed[0].z-=1;next(state,s,t);check(t.hand[0].reject&fm::Jump,"tracking discontinuity is recorded at rejection");
 s.valid=false;next(state,s,t);check(!t.valid&&(t.hand[0].reject&fm::Invalid)&&t.invalidInputs==1&&state.sequence==0,"ownership/tracking invalidation is explicit and state still retires");
 s=input();state={};ready(state,s,t);fm::step(state,s,&t);check(t.hand[0].reject==fm::Duplicate,"duplicate committed pose is distinguished from real missing motion");
 ++s.sequence;fm::step(state,s,&t);check(t.hand[0].reject==fm::ZeroDt,"quantized zero elapsed time is explicit");
 // Compare every gameplay state/event for trace ON and OFF, including invalidity,
 // epoch changes, open slaps, blockers, jumps, quantized clocks and both hands.
 fm::State off{},on{};s=input();unsigned random=1234567;bool equivalent=true;auto rng=[&](){random^=random<<13;random^=random>>17;random^=random<<5;return random;};
 for(int i=0;i<60000;++i){++s.sequence;s.tick+=rng()%14;s.valid=(rng()%200)!=0;if(rng()%700==0)++s.epoch;s.openPalm44=rng()%2;s.contactStop175=true;
  for(int side=0;side<2;++side){s.grip[side]=(rng()%100)/100.;s.trigger[side]=(rng()%100)/100.;s.empty[side]=rng()%120!=0;s.blocked[side]=rng()%3==0;s.freshContact174[side]=rng()%2;const double dz=(int(rng()%100)-50)*.002;s.controller[side].z+=dz;s.allowed[side].z+=dz*(rng()%2);}
  const auto a=fm::step(off,s),b=fm::step(on,s,&t);equivalent&=stateEqual(off,on)&&a.valid==b.valid&&a.side==b.side&&a.sequence==b.sequence&&a.tick==b.tick&&a.epoch==b.epoch&&a.actor==b.actor&&a.speed==b.speed&&a.travel==b.travel&&a.motion167==b.motion167;
 }
 check(equivalent,"60,000 production traces preserve every state field and punch event with diagnostics ON/OFF");
 fp::Record record{};record.tick=20000;cap::gate.mask=0;check(!fp::push(record),"capture OFF queues nothing");cap::gate.select(cap::Domain::Interaction,true,20000);check(!fp::push(record),"F5 alone cannot enable new punch recorder");cap::gate.select(cap::Domain::Combat,true,20000);
 check(fp::push(record),"F3 permits bounded POD producer");fp::Record out{};check(fp::pop(out)&&out.session==fp::session(),"queued event carries exact F3 session");
 fp::lock.test_and_set();const auto dropped=fp::dropped.load();check(!fp::push(record)&&fp::dropped.load()==dropped+1,"producer lock contention drops and accounts instead of blocking");fp::lock.clear();
 unsigned count=0;while(fp::push(record))++count;check(count==255,"bounded queue capacity is exactly255 and overflow reported");while(fp::pop(out)){}
 fp::Join join{1,2,3,4,5,6,0};check(fp::publishContext(join,20000,fp::Stage::Prepare,7),"native transport context publishes no game reads");fp::NativeContext context{};
 check(fp::readContext(context,20500)&&context.join.sample==5&&context.nativeActor==7,"native sweep can copy fixed-lived transport context");check(!fp::readContext(context,20501),"context expires after500ms without renewal");cap::gate.select(cap::Domain::Combat,true,20501);check(!fp::readContext(context,20501),"a new F3 session cannot borrow prior transport context");
 fp::Observer observer;s=input();s.tick=21000;state={};ready(state,s,t);observer.observe(s,t,{},99,12);while(fp::pop(out)){};next(state,s,t);observer.observe(s,t,{},99,12);
 char formatted[1900]{};record={};record.tick=21100;record.join=join;record.hand=t.hand[0];const int size=fp::format(record,formatted,sizeof(formatted));check(size>0&&size<int(sizeof(formatted))&&std::strstr(formatted,"triggerGate=GT_0_60")&&std::strstr(formatted,"nativeAdmission=NOT_INFERRED"),"normal formatter fits logger limit and separates transport from admission");
 record.hand.speed=std::numeric_limits<double>::max();char tiny[32]{};check(fp::format(record,tiny,sizeof(tiny))>=int(sizeof(tiny)),"formatter reports truncation length for worker loss accounting");
 cap::gate.mask=0;const auto produced=fp::accepted.load();observer.observe(s,t,{},99,12);check(fp::accepted.load()==produced,"producer observer is quiet when F3 ends");
 std::printf("punch_path_c54r3=%d/%d native_GAME_calls=NONE randomizedSteps=60000\n",total-failed,total);return failed?1:0;
}
