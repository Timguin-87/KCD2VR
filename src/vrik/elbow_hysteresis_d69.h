#pragma once
// Optional Claude destination trial: hysteresis + pole-only rate bound.
// No wrist filtering and no bone-length changes. OFF does not invoke this path.
#include "arm_constraints.h"
namespace kcd2vr::pose_core {
constexpr double kBranchMarginD67C=.15*(1-0.9659258262890683); // .15*(1-cos 15 deg)
constexpr std::uint64_t kBranchDwellMsD67C=100;
constexpr double kDestinationRateD67C=armPi; // 180 deg/s
inline Vec3 torso_elbow_destination_d67c(ElbowContinuity& s,Vec3 shoulder,Vec3 hand,double upper,double fore,
        Vec3 preference,Vec3 torsoA,Vec3 torsoB,double radius,
        std::uint64_t epoch,std::uint64_t sample,std::uint64_t tick,std::uint64_t timeNs) {
    auto arm=solve_arm(shoulder,hand,upper,fore,preference,preference);
    auto axis=unit(hand-shoulder);
    const bool history=s.destinationReadyD67C&&s.destEpochD67C==epoch&&tick>=s.destTickD67C&&
        tick-s.destTickD67C<=500&&sample>=s.destSampleD67C;
    if(!arm||!axis||!std::isfinite(radius)||radius<=0||!sample||!tick)
        return torso_elbow_preference(shoulder,hand,upper,fore,preference,torsoA,torsoB,radius,std::nullopt,false,false);
    std::optional<Vec3> previous{};
    if(history)previous=unit(perpendicular(rotate(arm_from_to(s.destAxisD67C,*axis,s.destinationD67C),s.destinationD67C),*axis));
    if(!history){s.alternativeSinceD67C=0;s.alternativeSideD69=0;}
    if(history&&sample==s.destSampleD67C&&previous)return *previous; // duplicate consumer: no temporal advance
    const Vec3 center=shoulder+*axis*dot(arm->elbow-shoulder,*axis);
    const double circle=length(arm->elbow-center);
    const auto commit=[&](Vec3 destination){
        if(previous){
            const double elapsed=motion_time::elapsed(s.destTickD67C,tick,s.destNsD67C,timeNs);
            const double dt=std::clamp(elapsed,0.0,.05);
            destination=turn_pole(*previous,destination,*axis,kDestinationRateD67C*dt);
        }
        s.destAxisD67C=*axis;s.destinationD67C=destination;s.destEpochD67C=epoch;s.destSampleD67C=sample;
        s.destTickD67C=tick;s.destNsD67C=timeNs;s.destinationReadyD67C=true;
        return destination;
    };
    if(circle<.003){s.alternativeSinceD67C=0;return commit(arm->bend);}
    const auto score=[&](Vec3 bend){
        const Vec3 elbow=center+bend*circle;
        double intrusion=0;
        for(int k=1;k<=4;++k){
            const double t=double(k)/4;
            for(Vec3 p:{shoulder+(elbow-shoulder)*t,elbow+(arm->hand-elbow)*(t*.8)}){
                const double v=std::max(0.0,radius-capsule_distance(p,torsoA,torsoB));
                intrusion+=v*v;
            }
        }
        return intrusion;
    };
    const auto up=unit(torsoB-torsoA).value_or(Vec3{0,0,1});
    const double handHeight=dot(arm->hand-shoulder,up);
    const bool levelReach=std::abs(handHeight)<.10;
    const bool hanging=handHeight<-.20;
    const int maxStep=hanging?3:6;
    // Candidate table: the same 10-degree bins, costs and skips as the default search.
    struct Candidate {Vec3 bend{};double cost=0,height=0;bool usable=false;};
    Candidate candidates[13]{};
    for(int k=-maxStep;k<=maxStep;++k){
        auto& c=candidates[k+6];
        const double angle=(armPi/18)*k;
        c.bend=arm->bend*std::cos(angle)+cross(*axis,arm->bend)*std::sin(angle);
        const Vec3 candidateElbow=center+c.bend*circle;
        c.height=dot(candidateElbow-shoulder,up);
        if(levelReach&&c.height>std::max(0.0,handHeight)+.002)continue;
        if(hanging&&previous&&dot(c.bend,*previous)<std::cos(armPi/6)-1e-9)continue; // cone about the PREVIOUS pole
        c.cost=score(c.bend)*5000+.15*(1-std::cos(angle));c.usable=true;
    }
    const double baseCost=score(arm->bend)*5000;
    int global=-1;double globalCost=baseCost;double globalHeight=dot(center+arm->bend*circle-shoulder,up);Vec3 globalBend=arm->bend;
    for(int i=0;i<13;++i){const auto& c=candidates[i];if(!c.usable)continue;
        if(c.cost<globalCost-1e-12||(std::abs(c.cost-globalCost)<=1e-12&&c.height<globalHeight-1e-9)){globalCost=c.cost;globalHeight=c.height;globalBend=c.bend;global=i;}
    }
    if(global<0&&candidates[6].usable)global=6; // the unrotated bend is the k=0 bin
    if(!previous){s.alternativeSinceD67C=0;return commit(globalBend);}
    // The branch the previous destination sits in: nearest usable bin, then downhill.
    int local=-1;double nearest=-2;
    for(int i=0;i<13;++i){const auto& c=candidates[i];if(!c.usable)continue;
        const double d=dot(c.bend,*previous);if(d>nearest){nearest=d;local=i;}
    }
    if(local<0){s.alternativeSinceD67C=0;return commit(globalBend);}
    for(;;){
        int next=local;
        if(local>0&&candidates[local-1].usable&&candidates[local-1].cost<candidates[next].cost-1e-12)next=local-1;
        if(local<12&&candidates[local+1].usable&&candidates[local+1].cost<candidates[next].cost-1e-12)next=local+1;
        if(next==local)break;local=next;
    }
    const double localCost=candidates[local].cost;
    Vec3 destination=candidates[local].bend;
    if(global<0||global==local||std::abs(globalCost-localCost)<=1e-12){s.alternativeSinceD67C=0;}
    else if(globalCost<localCost-kBranchMarginD67C){
        const int side=global<local?-1:1;
        if(!s.alternativeSinceD67C||tick<s.alternativeSinceD67C||side!=s.alternativeSideD69)s.alternativeSinceD67C=tick;
        s.alternativeSideD69=side;
        if(tick-s.alternativeSinceD67C>=kBranchDwellMsD67C){destination=globalBend;s.alternativeSinceD67C=0;}
    }else s.alternativeSinceD67C=0;
    return commit(destination);
}

}

#include "elbow_motion_d67.h"
namespace kcd2vr::elbow_motion_d69 {
inline pose_core::Vec3 preference(bool enabled,pose_core::Vec3 shoulder,pose_core::Vec3 hand,double upper,double fore,
 pose_core::Vec3 raw,pose_core::Vec3 torsoA,pose_core::Vec3 torsoB,double radius,pose_core::ElbowContinuity& history,
 std::uint64_t epoch,std::uint64_t sample,std::uint64_t tick,std::uint64_t ns,bool continuous,bool lab){
 if(enabled)return pose_core::torso_elbow_destination_d67c(history,shoulder,hand,upper,fore,raw,torsoA,torsoB,radius,epoch,sample,tick,ns);
 return elbow_motion_d67::preference(shoulder,hand,upper,fore,raw,torsoA,torsoB,radius,history,epoch,sample,tick,continuous,lab);
}
}
