#include "vrik/melee_stroke118.h"
#include <cassert>
using namespace kcd2vr::melee118;
int main(){
 State s{};s.kind=Kind::Cut;s.axis=s.modelAxis={0,1,0};
 Observation d{};d.progress=.15;d.fastTravel=.18;
 assert(!deliberate_cut42(s,d,{.2,.98,0},{.2,.98,0})); // short fast wrist adjustment
 s.hand=s.modelHand={0,.09,0};assert(deliberate_cut42(s,d,{0,1,0},{0,1,0}));
 s.hand=s.modelHand={};d.progress=.35;d.fastTravel=.4;
 assert(deliberate_cut42(s,d,{.6,.8,0},{.6,.8,0})); // genuine long wrist cut
 assert(!deliberate_cut42(s,d,{.1,.995,0},{.1,.995,0}));
 d.progress=.311;d.fastTravel=.314;s.hand=s.modelHand={0,.06,0};assert(!deliberate_cut42(s,d,{.6,.8,0},{.6,.8,0})); // logged-size flick, not enough travel
 s.kind=Kind::Thrust;assert(deliberate_cut42(s,d,{0,1,0},{0,1,0})); // thrust rules unchanged
 // Actual update replay: same close actor contact which formerly bypassed travel.
 State a{};a.ready=true;Policy p{true,true,true,false,true,true};p.deliberateCut42=true;
 Input in{};in.dt=.02;in.eligible=in.known=true;in.axis=in.modelAxis={1,0,0};
 in.forward=in.modelForward={0,1,0};in.tip=in.modelTip={0,.06,0};
 auto r=update(a,in,p);assert(!r.prepare&&!r.commit);
 in.blocked=true;in.contactEntity175=77;in.contactFraction175=.5;
 r=update(a,in,p);assert(!r.prepare&&!r.commit);
 // A translating cut reaches contact promptly and remains an eligible attack.
 a={};a.ready=true;in.blocked=false;in.contactEntity175=0;in.hand=in.modelHand={0,.03,0};
 in.tip=in.modelTip={0,.07,0};r=update(a,in,p);r=update(a,in,p);
 in.blocked=true;in.contactEntity175=77;in.contactFraction175=.8;
 r=update(a,in,p);assert(r.commit);
}
