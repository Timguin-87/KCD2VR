#define main original_guard_cut36_main_h5
#include "guard_cut36_test.cpp"
#undef main
#include "vrik/guard_beat_h5.h"
#include <cassert>
#include <memory>

struct KeyH5 {unsigned item=1,opponent=1;bool valid()const{return item&&opponent;}
 bool same(const KeyH5& b)const{return item==b.item&&opponent==b.opponent;}};
int main(){
 namespace g=kcd2vr::guardH5;
 {g::Beat<KeyH5> b;KeyH5 k;b.observe(k,4,1000,true);assert(!b.ready()); // no adopted mid-attack
  b.observe(k,1,1010,true);b.observe(k,4,1020,true);assert(b.ready()&&b.onset==1020);
  const auto serial=b.serial;b.consume();b.observe(k,8,1030,true);assert(!b.ready()&&b.serial==serial);
  b.observe(k,4,1040,true);assert(b.ready()&&b.serial!=serial&&b.onset==1040);
  b.consume();b.observe(k,8,1050,true);assert(!b.ready());
  b.observe(k,1,1060,true);b.observe(k,4,1070,true);assert(b.ready());
  b.observe(k,8,1200,true);assert(!b.ready()); // tracking gap cannot inherit beat
  b.observe(k,1,1210,true);++k.opponent;b.observe(k,4,1220,true);assert(!b.ready());
  b.observe(k,1,1230,true);b.observe(k,4,1240,true);assert(b.ready());
  b.observe(k,4,1250,false);assert(!b.ready());
 }
 { // A previous parry cannot be recycled by the next native attack serial.
  kcd2vr::guard_parry176::State s;using S=kcd2vr::guard_motion168::Sample;
  for(unsigned i=0;i<12;++i)s.update(S{1000+i*10,1+i,1,77,{double(i)*.01,0,0},{},true});
  assert(s.window.recent(160,40).valid);
  assert(!s.window.recent(160,40,1100).valid);
  assert(!kcd2vr::guard_parry176::againstRecent(s,1,{}, {1,0,0},.03,.5,1100).ok);
 }
 { // Actual production recognizer: continuous motion is still one stroke.
  namespace me=kcd2vr::melee118;
  for(bool selected:{false,true}){me::State s;s.ready=true;me::Policy p;
   p.deliberate119=p.handRearm178=p.responsive44c2=true;p.forwardOnly19=selected;
   me::Input i;i.dt=.01;i.eligible=i.known=true;i.outward=.4;
   i.axis=i.modelAxis=i.forward=i.modelForward={0,1,0};i.tip=i.modelTip=i.hand=i.modelHand={0,.025,0};
   unsigned commits=0;for(int n=0;n<30&&!commits;++n){auto o=me::update(s,i,p);commits+=o.commit;}
   assert(commits==1&&s.spent);assert(selected?s.cooldown==0:s.cooldown>0);
   for(int n=0;n<10;++n)assert(!me::update(s,i,p).commit);
   if(selected){i.tip=i.modelTip=i.hand=i.modelHand={};for(int n=0;n<7;++n)me::update(s,i,p);
    i.tip=i.modelTip=i.hand=i.modelHand={0,.025,0};unsigned next=0;
    for(int n=0;n<20;++n)next+=me::update(s,i,p).commit;assert(next==1);}
  }
 }
 { // Actual weapon producer, synthetic native-contact query only.
  auto r=std::make_unique<Rig36>(true,ph::Vec3{0,1,0});
  for(unsigned n=0;n<20&&!r->hits();++n)r->step({0,.025,0});
  assert(r->hits()==1);auto& s=r->state.weapon[0];const auto gesture=s.gesture59c3;
  assert(gesture&&s.released44c1&&s.stroke117==ph::CommandKind::Thrust);
  s.lastImpactTick=r->in.tick;r->blocked=r->actor=true;
  r->step({0,.02,0});auto hit=r->out.impactRecord178r30[0];
  assert(hit.valid&&hit.sample==r->in.sample&&hit.gesture59c3==gesture&&hit.kind==1);
  const auto first=hit.tick;assert(s.lastImpactTick<first); // cosmetic recoil untouched with dynamics off
  r->step({0,.02,0});hit=r->out.impactRecord178r30[0];
  assert(hit.valid&&hit.tick>first&&hit.tick-first<120&&hit.gesture59c3==gesture&&hit.kind==1);
  // A recognizer ending cannot erase its still-current committed source. Keep
  // the NEW actual contact rather than replaying a previous target/part/tick.
  const auto saved=hit;s.released44c1=false;s.stroke118={};s.stroke118.ready=false;
  s.guard117=false;r->step({0,.02,0});hit=r->out.impactRecord178r30[0];
  assert(hit.gesture59c3==saved.gesture59c3&&hit.tick>saved.tick&&hit.sample==r->in.sample&&hit.kind==1);
  r->in.bodyEpoch++;r->step({0,.02,0});
  assert(!r->out.impactRecord178r30[0].gesture59c3); // no old owner replay
 }
 std::puts("combat_h5: actual recognizer/producer and once-per-attack input policies passed; native outcomes unclaimed");
}
