#include "../../../src/subsystems/alchemy/near_selection67.h"
#include <cassert>
#include <cstdio>
#include <limits>
namespace n=kcd2vr::subsystems::alchemy::near67;
static unsigned checks=0;static void check(bool b){++checks;if(!b)std::fprintf(stderr,"near67 check%u FAILED\n",checks);assert(b);}
static n::Context context(){return {1,2,3,4,5,6,7};}
static n::Completed frame(){n::Completed f{};f.context=context();f.tick=1000;f.sequence=10;f.complete=true;f.count=2;
 f.table[0]={0,100};f.table[1]={1,101};f.offers[0]={{0,100},{.09,0,0},1000,true};
 f.offers[1]={{1,101},{1,0,0},1000,true};return f;}
static n::Input input(){n::Input in{};in.context=context();in.now=1000;in.currentFrameSequence=11;
 in.palms[0]={{0,0,0},1000,4,5,6,0,0,true};in.palms[1]={{3,0,0},1000,4,5,6,1,0,true};return in;}
static void advance(n::Completed& f,n::Input& in,unsigned ms=10){in.now+=ms;++f.sequence;in.currentFrameSequence=f.sequence+1;
 f.tick=in.now;for(auto& p:in.palms)p.tick=in.now;for(auto& p:f.offers)if(p.valid)p.tick=in.now;}
static void native(n::Input& in,n::Ref ref,int side=-1){in.selected={in.context,ref,in.now,side,true};}
int main(){auto f=frame();auto in=input();n::State state{};auto d=state.step(f,in);
 check(d.source==n::Source::Right&&d.ref.entity==100&&d.hapticEdge&&!d.gripping&&!d.admissionReady);
 d=state.step(f,in);check(!d.hapticEdge&&d.ref.entity==100); // same completed frame is immutable/reusable
 in.palms[0].point.x=-.05;d=state.step(f,in);check(d.source==n::Source::Right); // retained at14cm
 in.palms[0].point.x=-.050001;d=state.step(f,in);check(d.source==n::Source::Gaze);
 state={};f=frame();in=input();f.offers[0].point.x=.1;d=state.step(f,in);check(d.source==n::Source::Right);
 state={};f.offers[0].point.x=.100001;d=state.step(f,in);check(d.source==n::Source::Gaze);
 // Hysteresis prevents enumeration order and a slightly nearer item stealing ownership.
 state={};f=frame();in=input();d=state.step(f,in);f.offers[1].point.x=.01;
 advance(f,in);d=state.step(f,in);check(d.ref.entity==100&&!d.hapticEdge);
 auto swap=f.offers[0];f.offers[0]=f.offers[1];f.offers[1]=swap;d=state.step(f,in);check(d.ref.entity==100&&!d.hapticEdge);
 in.palms[0].point.x=-.06;d=state.step(f,in);check(d.ref.entity==101&&d.hapticEdge);
 // Two hands and two targets: nearest initially wins, exact ties deterministic
 // by hand then native table slot, not which callback was observed last.
 state={};f=frame();in=input();in.palms[1].point={.05,0,0};in.palms[0].grip=in.palms[1].grip=1;
 d=state.step(f,in);check(d.source==n::Source::Left&&d.gripping&&d.ref.entity==100);
 state={};in.palms[0].point=in.palms[1].point={0,0,0};d=state.step(f,in);check(d.source==n::Source::Right&&d.gripping);
 state={};f.offers[1].point=f.offers[0].point;swap=f.offers[0];f.offers[0]=f.offers[1];f.offers[1]=swap;
 d=state.step(f,in);check(d.ref.slot==0);
 // Grip latches one exact native ref. Moving away or another nearer hand does
 // not redirect a held tool. Native final selection must still validate admission.
 state={};f=frame();in=input();in.palms[0].grip=1;d=state.step(f,in);
 check(d.gripping&&!d.admissionReady&&d.latchedTick==1000);
 in.selected={in.context,{0,100},999,-1,true};d=state.step(f,in);check(!d.admissionReady);
 native(in,{0,100});d=state.step(f,in);check(d.admissionReady);
 native(in,{1,101});d=state.step(f,in);check(!d.admissionReady);
 native(in,{0,100},1);d=state.step(f,in);check(!d.admissionReady);
 native(in,{0,100},0);d=state.step(f,in);check(d.admissionReady);
 advance(f,in);in.palms[0].point={2,2,2};in.palms[1].point={.09,0,0};native(in,{0,100});
 d=state.step(f,in);check(d.source==n::Source::Right&&d.gripping&&d.ref.entity==100&&d.admissionReady&&!d.hapticEdge);
 // New grip at12cm is not a10cm grab despite hover leave hysteresis.
 state={};f=frame();in=input();state.step(f,in);in.palms[0].point.x=-.03;in.palms[0].grip=1;
 d=state.step(f,in);check(d.source==n::Source::Right&&!d.gripping&&!d.admissionReady);
 in.palms[0].point.x=0;d=state.step(f,in);check(d.gripping);
 // Releasing clears latch; no fresh native admission is fabricated from the old selection.
 in.palms[0].grip=0;d=state.step(f,in);check(!d.gripping&&!d.admissionReady);
 // Missing current geometry is NOT replaced by an entity centre or an old point.
 state={};f=frame();in=input();f.offers[0].valid=false;d=state.step(f,in);check(d.source==n::Source::Gaze);
 state={};f=frame();f.offers[0].tick=919;d=state.step(f,in);check(d.source==n::Source::Gaze);
 state={};f=frame();f.offers[0].tick=920;d=state.step(f,in);check(d.source==n::Source::Right);
 for(unsigned fault=0;fault<13;++fault){state={};f=frame();in=input();
  switch(fault){case 0:f.complete=false;break;case 1:f.tick=919;break;case 2:f.tick=1001;break;case 3:f.sequence=0;break;
  case 4:f.count=22;break;case 5:++f.context.player;break;case 6:++f.context.instance;break;case 7:++f.context.lifetime;break;
  case 8:++f.context.poseEpoch;break;case 9:++f.context.sourceEpoch;break;case 10:++f.context.generation;break;
  case 11:++f.context.tableGeneration;break;case 12:in.currentFrameSequence=f.sequence;break;}
  d=state.step(f,in);check(d.source==n::Source::Gaze&&!d.gripping&&!d.admissionReady);
 }
 for(unsigned fault=0;fault<12;++fault){state={};f=frame();in=input();
  switch(fault){case 0:in.palms[0].valid=false;break;case 1:in.palms[0].side=1;break;case 2:in.palms[0].tick=919;break;
  case 3:in.palms[0].point.x=std::numeric_limits<double>::quiet_NaN();break;case 4:++in.palms[0].poseEpoch;break;
  case 5:++in.palms[0].sourceEpoch;break;case 6:++in.palms[0].generation;break;case 7:in.palms[0].grip=2;break;
  case 8:f.offers[0].ref={1,100};break;case 9:f.offers[0].point.x=std::numeric_limits<double>::infinity();break;
  case 10:f.offers[0].tick=1001;break;case 11:f.table[0].entity=999;break;}
  d=state.step(f,in);check(d.source==n::Source::Gaze&&!d.gripping);
 }
 // The held side's lost tracking and UNKNOWN neutral input do not permit a
 // fresh tool admission on recovery until a valid open-grip sample is observed.
 state={};f=frame();in=input();in.palms[0].grip=1;check(state.step(f,in).gripping);
 advance(f,in);in.palms[0].valid=false;in.palms[0].grip=0;d=state.step(f,in);check(d.source==n::Source::Gaze&&!d.gripping);
 advance(f,in);in.palms[0].valid=true;in.palms[0].grip=1;d=state.step(f,in);check(d.source==n::Source::Gaze&&!d.gripping);
 in.palms[0].grip=0;d=state.step(f,in);check(d.source==n::Source::Right&&!d.gripping);
 in.palms[0].grip=1;d=state.step(f,in);check(d.gripping);
 // Context/source change retires a held ref, even if the new object happens to
 // reuse the same entity/slot values. It requires current neutral then a new grip.
 for(unsigned fault=0;fault<7;++fault){state={};f=frame();in=input();in.palms[0].grip=1;check(state.step(f,in).gripping);
  switch(fault){case 0:++in.context.instance;break;case 1:++in.context.player;break;case 2:++in.context.lifetime;break;
  case 3:++in.context.poseEpoch;break;case 4:++in.context.sourceEpoch;break;case 5:++in.context.generation;break;case 6:++in.context.tableGeneration;break;}
  f.context=in.context;in.palms[0].poseEpoch=in.context.poseEpoch;in.palms[0].sourceEpoch=in.context.sourceEpoch;in.palms[0].generation=in.context.generation;
  d=state.step(f,in);check(!d.gripping&&!d.admissionReady);in.palms[0].grip=0;state.step(f,in);in.palms[0].grip=1;
  check(state.step(f,in).gripping);
 }
 state={};f=frame();in=input();in.palms[0].grip=1;check(state.step(f,in).gripping);advance(f,in);f.table[0].entity=999;
 check(!state.step(f,in).gripping); // native table removal wins over a held squeeze
 state={};f=frame();in=input();state.step(f,in);--f.sequence;check(state.step(f,in).source==n::Source::Gaze);
 // Offers are a bounded completed set. A21st native row is eligible; no lookup
 // outside its exact registered row is permitted.
 state={};f=frame();in=input();f.count=21;for(auto& o:f.offers)o={};f.table[20]={20,777};f.offers[20]={{20,777},{.02,0,0},1000,true};
 d=state.step(f,in);check(d.ref.slot==20&&d.ref.entity==777);
 state={};f=frame();in=input();f.table[0].entity=f.offers[0].ref.entity=0xF1234567u;
 d=state.step(f,in);check(d.ref.entity==0xF1234567u); // native registered32-bit ID, no invented24-bit cap
 state={};f=frame();in=input();in.palms[0].grip=1;check(state.step(f,in).gripping);
 advance(f,in);++f.table[1].entity;d=state.step(f,in);check(!d.gripping&&!d.admissionReady);
 d=state.step(f,in);check(!d.gripping); // unrelated row replacement still retires old held topology
 in.palms[0].grip=0;state.step(f,in);advance(f,in);in.palms[0].grip=1;check(state.step(f,in).gripping);
 // Real retail bounds are independent of the head-ray point: a hand on the
 // side of a large object is near even when its old point is far away.
 state={};f=frame();in=input();auto& o=f.offers[0];o.point={99,99,99};o.boundsValid=true;
 o.bounds.local[0]=o.bounds.local[1]=o.bounds.local[2]=-.5f;
 o.bounds.local[3]=o.bounds.local[4]=o.bounds.local[5]=.5f;
 o.bounds.world[0]=o.bounds.world[5]=o.bounds.world[10]=1;o.bounds.world[3]=.55f;
 d=state.step(f,in);check(d.source==n::Source::Right&&d.gap>.049999&&d.gap<.050001);
 check(d.palm.valid&&d.palm.sourceEpoch==5&&n::same(d.table[0],f.table[0]));
 const auto surface=n::closest(o,in.palms[0].point);check(surface.valid&&std::abs(surface.point.x-.05)<1e-6);
 check(n::distanceToOffer(o,in.palms[0].point)<.051);
 in.palms[0].grip=1;check(state.step(f,in).gripping);in.palms[0].point.x=-2;
 d=state.step(f,in);check(d.gripping&&d.gap>2&&d.palm.point.x==-2); // not the old misleading zero gap
 state={};o.bounds.world[0]=0;d=state.step(f,in);check(d.source==n::Source::Gaze); // invalid bounds never fall back to a point
 std::printf("near_selection67: %u checks PASS (native geometry/selection are explicit input evidence)\n",checks);
}
