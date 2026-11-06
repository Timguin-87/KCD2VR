// Reconstruct each recorded prior state independently: sparse F7 cadence is
// not treated as a complete sequence. Executes the shipped R89 solver.
#include "vrik/elbow_r89.h"
#include <iostream>
#include <iomanip>
#include <cmath>
namespace pc=kcd2vr::pose_core;
static bool vec(std::istream& s,pc::Vec3& v){return bool(s>>v.x>>v.y>>v.z);}
int main(){
    unsigned rows=0,bad=0,policyMismatch=0;double maximum=0;
    std::uint64_t epoch=0,sample=0,tick=0,ns=0;
    while(std::cin>>epoch>>sample>>tick>>ns){
        pc::ElbowContinuity before{};kcd2vr::elbow_r89::Input in{};int ready=0,seed=0,wantedOk=0;pc::Vec3 seedValue{},wanted{};
        if(!(std::cin>>ready>>before.epoch>>before.sample>>before.tick>>before.timeNs)||!vec(std::cin,before.axis)||!vec(std::cin,before.bend)||
           !vec(std::cin,in.shoulder)||!vec(std::cin,in.wrist)||!(std::cin>>in.upper>>in.fore)||!vec(std::cin,in.body)||
           !vec(std::cin,in.forward)||!vec(std::cin,in.thumb)||!vec(std::cin,in.torsoA)||!vec(std::cin,in.torsoB)||
           !(std::cin>>in.torsoRadius>>seed)||!vec(std::cin,seedValue))return 2;
        namespace r89=kcd2vr::elbow_r89;
        const double constants[]{r89::kBody,r89::kForward,r89::kThumb,r89::kTorso,r89::kTau,r89::kRate,r89::kConfidence,r89::kTorsoMargin,r89::kTorsoBand};
        bool policyOk=true;for(const auto expected:constants){double observed=0;if(!(std::cin>>observed))return 2;policyOk=policyOk&&observed==expected;}
        if(!(std::cin>>wantedOk)||!vec(std::cin,wanted))return 2;
        if(!policyOk)++policyMismatch;
        before.ready=ready!=0;if(seed)in.seedElbow=seedValue;
        const auto actual=kcd2vr::elbow_r89::elbow(before,in,epoch,sample,tick,ns);
        double error=actual&&wantedOk?pc::length(*actual-wanted):0;
        const bool ok=policyOk&&bool(actual)==bool(wantedOk)&&std::isfinite(error)&&error<=1e-10;
        maximum=std::max(maximum,error);++rows;if(!ok){++bad;std::cout<<"MISMATCH "<<rows<<" error="<<std::setprecision(17)<<error<<'\n';}
    }
    std::cout<<"rows="<<rows<<" mismatches="<<bad<<" policy_mismatches="<<policyMismatch<<" maximum_error="<<std::setprecision(17)<<maximum<<'\n';
    return rows&&bad==0?0:1;
}
