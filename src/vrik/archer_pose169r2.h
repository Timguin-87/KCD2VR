#pragma once
// Model-space bow posture only. No camera, XR, native state, bone length or
// collision-permission writes. Temporal state belongs to the pose transaction.
#include "upper_body.h"
#include "arm_constraints.h"
namespace kcd2vr::archer169r2 {
namespace m=pose_core;
constexpr double pi=3.14159265358979323846;
struct State {
    std::uint64_t key=0,generation=0,epoch=0,request=0,sample=0,tick=0,timeNs=0;
    double correction=0; bool ready=false;
};
// Use the unturned skeleton as the retarget reference. Re-adding a solved,
// turned shoulder would move both wrists when chest participation changes.
inline std::optional<m::Vec3> retarget(m::Vec3 neutralShoulder,m::Vec3 neutralHead,
        m::Vec3 translatedHead,m::Vec3 controllerFromHead,m::Vec3 userShoulder,double scale) noexcept {
    if(!m::finite(neutralShoulder)||!m::finite(neutralHead)||!m::finite(translatedHead)||
       !m::finite(controllerFromHead)||!m::finite(userShoulder)||!std::isfinite(scale)||scale<.3||scale>2.5)return {};
    return neutralShoulder+(translatedHead-neutralHead)+(controllerFromHead-userShoulder)*scale;
}
inline double yaw_goal(m::Vec3 neutralRight,m::Vec3 drawHand,m::Vec3 bowHand,double fallback) noexcept {
    neutralRight.z=0;auto span=drawHand-bowHand;span.z=0;
    const auto a=m::unit(neutralRight),b=m::unit(span);
    if(!a||!b||m::length(span)<.12)return fallback;
    // Shoulder participation, not a side-on native shooting stance. The
    // dominant left wrist and the glued right string hand remain unchanged.
    const double relative=std::remainder(std::atan2(m::cross(*a,*b).z,m::dot(*a,*b))-fallback,2*pi);
    return fallback+std::clamp(relative*.40,-30*pi/180,30*pi/180);
}
inline double advance(State& s,bool valid,std::uint64_t key,std::uint64_t generation,
        std::uint64_t epoch,std::uint64_t request,std::uint64_t sample,std::uint64_t tick,
        std::uint64_t timeNs,double baseYaw,double targetYaw,double pullWeight,bool fresh=true) noexcept {
    if(!valid||!key||!sample||!tick||!std::isfinite(baseYaw)||!std::isfinite(targetYaw)||!std::isfinite(pullWeight)){
        s={};return 0;
    }
    const bool same=s.ready&&s.key==key&&s.generation==generation&&s.epoch==epoch&&s.request==request;
    if(same&&(sample<s.sample||tick<s.tick))return s.correction;
    if(same&&sample==s.sample)return s.correction; // second eye never advances posture
    const double elapsed=same?motion_time::elapsed(s.tick,tick,s.timeNs,timeNs):0;
    const bool history=same&&elapsed>=0&&elapsed<=.080000001;
    if(!history){s={};s.ready=true;s.key=key;s.generation=generation;s.epoch=epoch;s.request=request;}
    const double weight=std::clamp(pullWeight,0.0,1.0);
    const double finalYaw=std::clamp(baseYaw+std::remainder(targetYaw-baseYaw,2*pi)*weight,-75*pi/180,75*pi/180);
    const double desired=finalYaw-baseYaw;
    const double dt=history&&fresh?std::min(elapsed,.05):0; // existing visual grace never creates posture intent
    const double step=(desired-s.correction)*(-std::expm1(-dt/(weight>0?.09:.14)));
    s.correction+=std::clamp(step,-1.8*dt,1.8*dt); // visual shoulder participation, not wrist lag
    s.sample=sample;s.tick=tick;s.timeNs=timeNs;
    return s.correction;
}
// The draw elbow is inferred, not measured by the controller. Search its
// fixed-length solution circle; keep the wrist fixed and use the existing
// temporal pole filter after this preference. These are torso/head proxies,
// not a claim about armor mesh or simultaneous world/body clearance.
inline m::Vec3 elbow_preference(m::Vec3 shoulder,m::Vec3 hand,m::Vec3 bowHand,
        double upper,double fore,m::Vec3 fallback,m::Vec3 torsoA,m::Vec3 torsoB,
        double torsoRadius,m::Vec3 head,double headRadius=.09) noexcept {
    const auto axis=m::unit(hand-shoulder);
    auto wanted=axis?m::unit(m::perpendicular(hand-bowHand,*axis)):std::optional<m::Vec3>{};
    if(!axis||!wanted||!m::finite(head)||!std::isfinite(torsoRadius)||torsoRadius<=0)return fallback;
    const auto arm=m::solve_arm(shoulder,hand,upper,fore,*wanted,fallback);
    if(!arm)return fallback;
    const auto center=shoulder+*axis*m::dot(arm->elbow-shoulder,*axis);
    const double radius=m::length(arm->elbow-center);
    if(radius<.003)return arm->bend;
    auto chosen=arm->bend;double best=1e30;
    for(int k=-12;k<=12;++k){
        const double a=k*pi/12;
        const auto bend=arm->bend*std::cos(a)+m::cross(*axis,arm->bend)*std::sin(a);
        const auto elbow=center+bend*radius;double intrusion=0;
        for(int j=1;j<=5;++j){
            const double t=double(j)/5;
            for(const auto p:{shoulder+(elbow-shoulder)*t,elbow+(hand-elbow)*(t*.8)}){
                const double body=std::max(0.0,torsoRadius+.02-m::capsule_distance(p,torsoA,torsoB));
                const double face=std::max(0.0,headRadius+.02-m::length(p-head));
                intrusion+=body*body+face*face;
            }
        }
        const double score=intrusion*5000+.15*(1-m::dot(bend,*wanted));
        if(score<best){best=score;chosen=bend;}
    }
    return chosen;
}
inline double arm_proxy_gap(m::Vec3 shoulder,m::Vec3 elbow,m::Vec3 hand,
        m::Vec3 torsoA,m::Vec3 torsoB,double radius) noexcept {
    double gap=1e9;
    for(int j=1;j<=8;++j){const double t=double(j)/8;
        for(auto p:{shoulder+(elbow-shoulder)*t,elbow+(hand-elbow)*(t*.875)})
            gap=std::min(gap,m::capsule_distance(p,torsoA,torsoB)-radius-.02);
    }
    return gap;
}
}
