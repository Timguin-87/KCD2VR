#include "subsystems/throw_drop/pending_reach_t23.h"
#include <cassert>
#include <cstdio>
namespace t=kcd2vr::throw_admission_t23;
int main(){
 t::Source a{};auto& f=a.frame;
 f.player=1;f.manager=2;f.character=3;f.entity=4;f.body=5;f.request=6;f.sample=7;f.tick=1000;
 f.allowed=true;f.chestKnivesT26=true;f.pose18[1]=true;f.poseTick18[1]=1000;f.wrist18[1][3]=1;f.wrist18[1][4]=.2f;
 a.tracking=kcd2vr::tracking59c6::epoch.load();a.publication=2;a.complete=true;a.handKnown[1]=a.handEmpty[1]=true;
 t::Live live{};live.player=1;live.manager=2;live.character=3;live.entity=4;live.body=5;live.request=6;
 live.tracking=a.tracking;live.observed=1000;live.allowed=true;live.handKnown[1]=live.handEmpty[1]=live.tracked[1]=true;
 assert(t::PublishLive(live)&&t::PublishSource(a));assert(t::CurrentSource(f,1000));
 auto b=a;b.frame.sample++;b.frame.tick++;b.publication+=2;b.frame.wrist18[1][4]=.4f;b.frame.poseTick18[1]++;
 assert(t::PublishSource(b));assert(!t::CurrentSource(f,1001));
 {t::SourceScopeD72 scope(a);assert(t::CurrentSource(f,1001));assert(t::CurrentSource(f,1080));assert(!t::CurrentSource(f,1081));
  auto corrupt=f;corrupt.wrist18[1][4]=9;assert(!t::CurrentSource(corrupt,1001));
  t::Pending p{f,101,1,0};assert(t::PublishPending(p,1001));assert(t::ClearPending(p));
  {t::SourceScopeD72 nested(b);assert(t::CurrentSource(b.frame,1001));assert(!t::CurrentSource(f,1001));}
  assert(t::CurrentSource(f,1001));
  auto stopped=live;stopped.allowed=false;assert(t::PublishLive(stopped));assert(!t::CurrentSource(f,1001));
  assert(t::PublishLive(live));assert(t::CurrentSource(f,1001));
  ++kcd2vr::tracking59c6::epoch;assert(!t::CurrentSource(f,1001));--kcd2vr::tracking59c6::epoch;
 }
 assert(!t::CurrentSource(f,1001)&&t::CurrentSource(b.frame,1001));
 assert(!t::CurrentSource(b.frame,1082));
 std::puts("D72 actual source admission: concurrent publication, original deadline, corruption, live cancellation, tracking epoch, nested scope, pending lifetime PASS");
}
