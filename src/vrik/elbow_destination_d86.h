#pragma once
// D86: one anatomical destination for tracked arms, including a bow draw.
// A controller does not measure an elbow. Infer a relaxed solution on the
// exact fixed-length circle, then let the existing pole-only clock approach it.
// No wrist, bone length, contact endpoint or engine/tracking state is written.
#include "arm_constraints.h"
namespace kcd2vr::elbow_destination_d86 {
namespace m=pose_core;
struct Input {
    m::Vec3 shoulder{},hand{},relaxed{},outward{},up{},backward{};
    m::Vec3 torsoA{},torsoB{},head{},forearmDirection{},bowAway{};
    double upper=0,fore=0,torsoRadius=0,drawWeight=0;
    bool haveHead=false,haveForearm=false;
    bool crossbowSupportR93=false; // right support elbow below the stock; never changes the wrist
    std::optional<m::Vec3> prior{};
};
struct Result {
    m::Vec3 pole{};
    double intrusion=0,objective=0;
    unsigned evaluations=0;
    bool valid=false;
};
// The neutral forearm axis is expressed in the HAND's local coordinates.
// Pure pronation about this axis changes hand roll but leaves this cue fixed.
// Missing/invalid reference geometry grants no guessed controller axis.
inline std::optional<m::Vec3> forearm_direction(m::Vec3 restForearm,m::Vec3 restHand,
        m::Quat restHandRotation,m::Quat currentHandRotation) noexcept {
    const auto axis=m::unit(restHand-restForearm);
    const auto rest=m::unit(restHandRotation),now=m::unit(currentHandRotation);
    if(!axis||!rest||!now)return {};
    return m::unit(m::rotate(*now,m::rotate(m::conjugate(*rest),*axis)));
}
struct Circle {
    m::Vec3 axis{},center{},reference{},tangent{};
    double radius=0;bool priorSeed=false;
};
inline std::optional<Circle> circle(const Input& in) noexcept {
    if(!m::finite(in.shoulder)||!m::finite(in.hand)||!m::finite(in.relaxed)||
       !m::finite(in.outward)||!m::finite(in.up)||!m::finite(in.backward)||
       !m::finite(in.torsoA)||!m::finite(in.torsoB)||
       !(in.upper>0&&in.fore>0&&in.torsoRadius>0))return {};
    const auto axis=m::unit(in.hand-in.shoulder);if(!axis)return {};
    // Frame selection is only a coordinate chart for this sample's search.
    // It is not an anatomical target and cannot drag a tracked wrist.
    auto reference=in.prior?m::unit(m::perpendicular(*in.prior,*axis)):std::optional<m::Vec3>{};
    const bool priorSeed=bool(reference);
    if(!reference){
        double best=-1;
        for(const auto v:{in.relaxed,in.outward,in.backward,in.up}){
            const auto p=m::perpendicular(v,*axis);const double n=m::dot(p,p);
            if(n>best&&n>1e-12){best=n;reference=m::unit(p);}
        }
    }
    if(!reference)return {};
    const auto arm=m::solve_arm(in.shoulder,in.hand,in.upper,in.fore,*reference,*reference);
    if(!arm||m::length(arm->hand-in.hand)>1e-5)return {};
    Circle c{};c.axis=*axis;c.reference=*reference;c.tangent=m::cross(*axis,*reference);
    c.center=in.shoulder+*axis*m::dot(arm->elbow-in.shoulder,*axis);
    c.radius=m::length(arm->elbow-c.center);c.priorSeed=priorSeed;return c;
}
inline m::Vec3 bend_at(const Circle& c,double angle) noexcept {
    return c.reference*std::cos(angle)+c.tangent*std::sin(angle);
}
inline double intrusion(const Input& in,const Circle& c,m::Vec3 bend) noexcept {
    const auto elbow=c.center+bend*c.radius;
    double total=0;
    for(int j=1;j<=4;++j){
        const double t=double(j)/4;
        for(const auto p:{in.shoulder+(elbow-in.shoulder)*t,elbow+(in.hand-elbow)*(t*.8)}){
            const double body=std::max(0.,in.torsoRadius+.01-m::capsule_distance(p,in.torsoA,in.torsoB));
            const double face=in.haveHead&&m::finite(in.head)?std::max(0.,.11-m::length(p-in.head)):0.;
            total+=body*body+face*face;
        }
    }
    return total;
}
inline double objective(const Input& in,const Circle& c,m::Vec3 bend,double* penetration=nullptr) noexcept {
    const auto elbow=c.center+bend*c.radius;
    const auto relaxed=m::unit(in.relaxed).value_or(m::Vec3{});
    const auto up=m::unit(in.up).value_or(m::Vec3{0,0,1});
    const auto outward=m::unit(in.outward).value_or(m::Vec3{});
    const double height=m::dot(in.hand-in.shoulder,up);
    const double raised=m::smooth01((height+.05)/.35);
    // Costs use UNNORMALIZED projections implicitly (dot against a unit
    // world/model vector). When a cue aligns with the reach axis its influence
    // tends to zero instead of normalizing tracking noise into a 180-deg pole.
    double cost=.15*(1-m::dot(bend,relaxed))+.035*raised*(1-m::dot(bend,outward));
    if(in.haveForearm)if(const auto cue=m::unit(in.forearmDirection)){
        const auto fore=m::unit(in.hand-elbow);
        if(fore)cost+=.20*(1-m::dot(*fore,*cue)); // soft wrist/forearm relationship, never a wrist clamp
    }
    if(const auto away=m::unit(in.bowAway))
        cost+=.22*std::clamp(in.drawWeight,0.,1.)*(1-m::dot(bend,*away));
    // Low hands should not lift the elbow into a chicken-wing posture simply
    // because two collision routes have almost equal cost.
    const double above=std::max(0.,m::dot(elbow-in.shoulder,up)-std::max(0.,height)-.02);
    cost+=12*above*above;
    if(in.crossbowSupportR93){
        // A braced crossbow has a tucked support elbow. This is a soft cost on
        // the fixed-length elbow circle, not a hand/socket offset. Torso and
        // face clearance keep their stronger cost; unreachable wrists stay put.
        const auto back=m::unit(in.backward).value_or(m::Vec3{});
        const auto tucked=m::unit(up*-1.+outward*.18+back*.20).value_or(relaxed);
        cost+=.70*(1-m::dot(bend,tucked));
        const double aboveWrist=std::max(0.,m::dot(elbow-in.hand,up)+.015);
        cost+=20*aboveWrist*aboveWrist;
    }
    const double pen=intrusion(in,c,bend);if(penetration)*penetration=pen;
    return cost+50000*pen;
}
inline Result choose(const Input& in) noexcept {
    Result result{};const auto geometry=circle(in);if(!geometry)return result;
    const auto& c=*geometry;
    double angle=0,value=0,currentPen=0;
    const auto cost=[&](double a,double* pen=nullptr){++result.evaluations;return objective(in,c,bend_at(c,a),pen);};
    value=cost(0,&currentPen);
    // Cold geometry has no previous branch. Pick a coarse seed once, then
    // refine continuously. Fresh samples never redo a global candidate vote.
    if(!c.priorSeed){
        for(int k=1;k<12;++k){const double a=2*m::armPi*k/12;double pen=0;const double v=cost(a,&pen);
            if(v<value-1e-12){angle=a;value=v;currentPen=pen;}}
    }
    constexpr double derivative=.002,maxStep=m::armPi/12;
    for(int iteration=0;iteration<8;++iteration){
        const double minus=cost(angle-derivative),plus=cost(angle+derivative);
        const double gradient=(plus-minus)/(2*derivative),curvature=(plus-2*value+minus)/(derivative*derivative);
        double step=std::clamp(-gradient/std::max(.05,curvature),-maxStep,maxStep);
        if(std::abs(step)<1e-7){
            if(minus>=value-1e-12&&plus>=value-1e-12)break;
            // An exact symmetric maximum must leave its old branch; history
            // may not become a self-following destination around an arm loop.
            if(std::abs(plus-minus)<1e-12){
                const auto lower=bend_at(c,angle+derivative)-bend_at(c,angle-derivative);
                step=m::dot(lower,in.up)<0?maxStep:-maxStep;
            }else step=plus<minus?maxStep:-maxStep;
        }
        bool advanced=false;
        for(int line=0;line<3;++line){
            const double next=angle+step;double nextPen=0;const double nextValue=cost(next,&nextPen);
            // A local branch cannot jump through a thin occupied arc just
            // because the opposite endpoint happens to have a lower cost.
            const double midPen=intrusion(in,c,bend_at(c,angle+step*.5));
            if(nextValue<value-1e-12&&midPen<=std::max(currentPen,nextPen)+1e-10){
                angle=next;value=nextValue;currentPen=nextPen;advanced=true;break;
            }
            step*=.5;
        }
        if(!advanced)break;
    }
    result.pole=bend_at(c,angle);result.objective=value;result.intrusion=currentPen;
    result.valid=m::finite(result.pole);return result;
}
}
