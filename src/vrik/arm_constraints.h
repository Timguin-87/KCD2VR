#pragma once
// V66.101R1: temporal state must be staged with the pose transaction.
#include "elbow_continuity.h"
#include "frik_elbow_lab.h" // explicit FRIK elbow experiment (live menu / VrFrikElbows)
namespace kcd2vr::pose_core {
constexpr double armPi=3.14159265358979323846;
struct ForearmPronation {
    std::uint64_t epoch=0,sample=0,tick=0,timeNs=0;
    double previousRaw=0,unwrapped=0,allowed=0;
    bool ready=false,conditioned=false;
};
inline std::optional<Quat> continuous_pronation(ForearmPronation& s,Quat relative,Vec3 direction,
        std::uint64_t epoch,std::uint64_t sample,std::uint64_t tick,std::uint64_t timeNs=0) {
    const auto q=unit(relative);const auto a=unit(direction);
    if(!q||!a||!sample||!tick)return std::nullopt;
    constexpr double forearmShare116=.65;
    if(s.ready&&s.epoch==epoch&&(sample<s.sample||tick<s.tick))return std::nullopt;
    const bool history=s.ready&&s.epoch==epoch&&tick>=s.tick&&tick-s.tick<=500;
    if(history&&sample==s.sample)return from_rotation_vector(*a*s.allowed);
    const double projection=q->x*a->x+q->y*a->y+q->z*a->z;
    const double conditioning=std::hypot(q->w,projection);
    // q and -q have the same wrapped angle. Unwrap against the previous sample,
    // NOT against a stateless w>=0 choice before an anatomical clamp.
    const double raw=std::remainder(2*std::atan2(projection,q->w),2*armPi);
    const bool stable=conditioning>(history&&s.conditioned?.06:.12);
    if(!history){s={};s.ready=true;s.epoch=epoch;s.previousRaw=raw;s.unwrapped=stable?raw:0;}
    else if(stable){
        if(s.conditioned)s.unwrapped+=std::remainder(raw-s.previousRaw,2*armPi);
        else s.unwrapped=s.allowed/forearmShare116; // preserve allowed roll when re-entering from degeneracy
    }
    // Returning the controller to neutral must unwind accumulated full turns.
    // Preserve the continuous branch around 180 degrees; rebase only near neutral.
    if(stable&&std::abs(raw)<20*armPi/180&&std::abs(s.unwrapped-raw)>armPi)s.unwrapped=raw;
    const double elapsed=history?motion_time::elapsed(s.tick,tick,s.timeNs,timeNs):0;
    if(history&&elapsed<0)return std::nullopt;
    const double dt=history?std::clamp(elapsed,0.0,.05):0;
    const double limit=80*armPi/180;
    // Share pronation with the wrist on both sides. The final wrist quaternion
    // remains controller-owned; this changes skin roll, never either endpoint.
    const double desired=stable?std::clamp(s.unwrapped*forearmShare116,-limit,limit):s.allowed;
    s.allowed+=std::clamp(desired-s.allowed,-5.0*dt,5.0*dt);
    s.previousRaw=raw;s.conditioned=stable;s.sample=sample;s.tick=tick;s.timeNs=timeNs;
    return from_rotation_vector(*a*s.allowed);
}
inline double capsule_distance(Vec3 point,Vec3 a,Vec3 b) {
    const Vec3 ab=b-a;const double d=dot(ab,ab);
    const double t=d>1e-12?std::clamp(dot(point-a,ab)/d,0.0,1.0):0;
    return length(point-(a+ab*t));
}
// A geometric preference on the fixed-length solution circle. D45 follows a
// continuous local clearance branch inside a smooth anatomical window; the
// legacy finite-candidate policy remains explicit for bow handling. The existing
// temporal filter approaches the selected pole. No bone stretching, scene
// queries, wrist filtering, or instant post-filter pole replacement.
inline Vec3 torso_elbow_preference(Vec3 shoulder,Vec3 hand,double upper,double fore,
        Vec3 preference,Vec3 torsoA,Vec3 torsoB,double radius,
        std::optional<Vec3> priorBendD45=std::nullopt,bool continuousBranchD45=false,
        bool labModeD45=kcd2vr::frik_elbow_lab::Active()) {
    auto arm=solve_arm(shoulder,hand,upper,fore,preference,preference);
    auto axis=unit(hand-shoulder);
    if(!arm||!axis||!std::isfinite(radius)||radius<=0)return preference;
    const Vec3 center=shoulder+*axis*dot(arm->elbow-shoulder,*axis);
    const double circle=length(arm->elbow-center);
    if(circle<.003)return arm->bend;
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
    Vec3 chosen=arm->bend;double best=continuousBranchD45?0.0:score(chosen);
    if(best<1e-7&&!continuousBranchD45)return chosen;
    double bestCost=best*5000;
    const auto up=unit(torsoB-torsoA).value_or(Vec3{0,0,1});
    const double handHeight=dot(arm->hand-shoulder,up);
    if(continuousBranchD45){
        // D45: the old global minimum over10-degree bins could alternate
        // between opposite, equally usable clearance routes (+30/-30 in the
        // supplied trace). Follow a continuous local minimum of the same
        // geometric cost. Committed history selects a branch only; the cost
        // still attracts the elbow to its chest-relative anatomical posture.
        // Fixed cost budget: one initial and at most8*(2+3) evaluations, each
        // with8 capsule distances. No scene queries or temporal wrist filter.
        const auto tangent=cross(*axis,arm->bend);
        const auto bendAt=[&](double a){return arm->bend*std::cos(a)+tangent*std::sin(a);};
        const auto cost=[&](double a){
            const auto b=bendAt(a);
            return score(b)*5000+.15*(1-std::cos(a))+
                kcd2vr::frik_elbow_lab::window_cost(a,handHeight)+
                kcd2vr::frik_elbow_lab::level_cost(dot(center+b*circle-shoulder,up),handHeight);
        };
        const double window=kcd2vr::frik_elbow_lab::window_half_deg(handHeight)*armPi/180;
        double angle=0;
        if(priorBendD45){
            if(const auto p=unit(perpendicular(*priorBendD45,*axis))){
                // A sine coordinate keeps the branch hint continuous even at
                // an opposite old pole. It is only a seed inside this frame's
                // anatomical window, never a clamp of the committed elbow.
                angle=window*std::clamp(dot(*p,tangent),-1.0,1.0);
            }
        }
        double value=cost(angle);
        constexpr double derivativeStep=.002,maxStep=armPi/12;
        for(int iteration=0;iteration<8;++iteration){
            const double minus=cost(angle-derivativeStep),plus=cost(angle+derivativeStep);
            const double gradient=(plus-minus)/(2*derivativeStep);
            const double curvature=(plus-2*value+minus)/(derivativeStep*derivativeStep);
            double step=std::clamp(-gradient/std::max(.05,curvature),-maxStep,maxStep);
            if(std::abs(step)<1e-7){
                if(minus>=value-1e-12&&plus>=value-1e-12)break;
                // A cold, exactly symmetric local maximum has two equal
                // routes. Use the lower elbow, on either arm, deterministically.
                if(std::abs(plus-minus)<1e-12)
                    step=dot(bendAt(angle+derivativeStep)-bendAt(angle-derivativeStep),up)<0?derivativeStep:-derivativeStep;
                else step=plus<minus?derivativeStep:-derivativeStep;
            }
            bool advanced=false;
            for(int line=0;line<3;++line){
                const double next=std::clamp(angle+step,-window,window),nextValue=cost(next);
                if(nextValue<value-1e-12){angle=next;value=nextValue;advanced=true;break;}
                step*=.5;
            }
            if(!advanced)break;
        }
        return bendAt(angle);
    }
    if(labModeD45){
        // R8c lab: same candidates/costs, but the hanging cap and the level-reach skip are smooth priced walls
        // (frik_elbow_lab::window_cost/level_cost), 5 deg candidates, window never under 15 deg. tie-break unchanged.
        namespace fl=kcd2vr::frik_elbow_lab;
        bestCost+=fl::level_cost(dot(chosen*circle+center-shoulder,up),handHeight);
        double chosenHeightLab=dot(center+chosen*circle-shoulder,up);
        for(int k=-12;k<=12;++k){
            const double angle=(armPi/36)*k;
            const Vec3 candidate=arm->bend*std::cos(angle)+cross(*axis,arm->bend)*std::sin(angle);
            const Vec3 candidateElbow=center+candidate*circle;
            const double height=dot(candidateElbow-shoulder,up);
            const double cost=score(candidate)*5000+.15*(1-std::cos(angle))+fl::window_cost(angle,handHeight)+fl::level_cost(height,handHeight);
            if(cost<bestCost-1e-12||(std::abs(cost-bestCost)<=1e-12&&height<chosenHeightLab-1e-9)){bestCost=cost;chosen=candidate;chosenHeightLab=height;}
        }
        return chosen;
    }
    // Below-hand preference is only appropriate around shoulder/chest height,
    // not an impossible universal rule for hands hanging below the elbow.
    const bool levelReach=std::abs(handHeight)<.10;
    // W1: a relaxed hanging arm (hand >20 cm under the shoulder) turns its elbow at most 30 deg for torso
    // clearance, so the elbow cannot swing out sideways around the vertical arm.
    const int maxStep=handHeight<-.20?3:6;
    double chosenHeight=dot(center+chosen*circle-shoulder,up);
    for(int k=-maxStep;k<=maxStep;++k){
        const double angle=(armPi/18)*k; // +/-60 degrees, 10-degree candidates
        const Vec3 candidate=arm->bend*std::cos(angle)+cross(*axis,arm->bend)*std::sin(angle);
        const Vec3 candidateElbow=center+candidate*circle;
        if(levelReach&&dot(candidateElbow-shoulder,up)>std::max(0.0,handHeight)+.002)continue;
        // Rotation preserves the bend length: its angular penalty is exactly
        // 1-cos(angle). Recomputing that dot can give equal +/- rotations
        // different roundoff costs and flip a collision-clear elbow branch.
        const double cost=score(candidate)*5000+.15*(1-std::cos(angle));
        // W1: equal cost (mirrored +/- candidates) -> the LOWER elbow wins on both arms. before, the scan order
        // (always -60 first) picked opposite branches on the left and right arm.
        const double height=dot(candidateElbow-shoulder,up);
        if(cost<bestCost-1e-12||(std::abs(cost-bestCost)<=1e-12&&height<chosenHeight-1e-9)){bestCost=cost;chosen=candidate;chosenHeight=height;}
    }
    return chosen;
}
} // namespace
