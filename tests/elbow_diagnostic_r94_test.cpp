// Execute actual solver with/without observation, then emit the production F7
// serialization for the independent row-replay program. Engine leaves absent.
#include "diagnostics/vrik_trace_h29.h"
#include <cstdio>
namespace pc=kcd2vr::pose_core;
int main(){
    namespace d=kcd2vr::diagnostics::vrik_h29;namespace r89=kcd2vr::elbow_r89;
    using namespace kcd2vr;pc::ElbowContinuity state{};unsigned checked=0;
    diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Vrik,true,1000);
    for(unsigned n=0;n<18;++n){
        d::Record r{};r.kind=d::Kind::Cycle;r.tick=r.sourceTick=1000+n*30;r.timeNs=r.tick*1000000;
        r.sample=n==6?6:n+1;r.epoch=n>10?3:2;r.trackingEpoch=4;r.publication=n+1;r.poseData=r.committed=r.published=true;
        r.line=int(n+1); // distinct fixture events pass the production 25 ms event throttle
        auto& a=r.arms[0];auto& c=a.context;c.entered=true;c.selectedSolverR94=d::ElbowSolverR94::HandR89;
        c.priorReady=state.ready;c.priorEpoch=state.epoch;c.priorSample=state.sample;c.priorTick=state.tick;c.priorNs=state.timeNs;c.priorAxis=state.axis;c.priorBend=state.bend;
        r89::Input in{};in.shoulder={.2,0,1.5};in.wrist={.35+.07*std::sin(n),.22+.1*std::cos(n),1.65};
        in.upper=.31;in.fore=.29;in.body={.4,-.6,-1};in.forward={.1,1,-.1};in.thumb={-1,.2,.1};
        if(n==9){in.forward={};in.thumb={};} // actual crossbow-support cue suppression
        in.torsoA={0,0,.8};in.torsoB={0,0,1.6};in.torsoRadius=.15;in.seedElbow=pc::Vec3{.45,-.1,1.2};
        c.inputR89=in;c.shoulder=in.shoulder;c.goal=in.wrist;c.axisInput=in.wrist-in.shoulder;
        auto control=state;const auto expected=r89::elbow(control,in,r.epoch,r.sample,r.sourceTick,r.timeNs);
        const auto observed=r89::elbow(state,in,r.epoch,r.sample,r.sourceTick,r.timeNs,&c.traceR89);
        if(bool(expected)!=bool(observed)||(observed&&pc::length(*expected-*observed)!=0))return 1;
        if(observed)c.filteredPole=c.solvePole=*observed;c.continuityOk=bool(observed);
        a.upperLength=in.upper;a.foreLength=in.fore;
        if(!d::push(r))return 2;
        d::drain(r.tick,[](const char* line){std::puts(line);});++checked;
    }
    std::printf("R94 observational invariance: %u checks PASS\n",checked);return 0;
}
