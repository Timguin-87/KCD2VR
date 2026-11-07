// R38: reproduce the October 28 F7 hand displacement using production geometry.
// Engine writes, native physics and headset rendering are not simulated here.
#include "../src/vrik/upper_body.h"
#include "../src/vrik/arm_constraints.h"
#include <cstdio>
#include <limits>

namespace pc=kcd2vr::pose_core;
namespace fl=kcd2vr::frik_elbow_lab;
static int total=0,failed=0;
static void check(bool value,const char* name){++total;if(!value)++failed;std::printf("%s %s\n",value?"PASS":"FAIL",name);}
static bool close(pc::Vec3 a,pc::Vec3 b,double eps=1e-8){return pc::length(a-b)<eps;}
static bool lengths(const pc::ControllerShoulder103& root,const pc::ArmSolution& arm,double upper,double fore){
    return std::abs(pc::length(arm.elbow-root.shoulder)-upper)<1e-8&&
           std::abs(pc::length(arm.hand-arm.elbow)-fore)<1e-8;
}
static void RecordedPair(){
    // Exact F7 lines 473360 / 473409, samples 257594 / 257598. Their controller
    // delta is 0.115195 mm, requested model delta 0.139284 mm, shoulder delta
    // 47.717825 mm; the old published wrist delta is 28.887162 mm.
    const pc::Vec3 shoulder[2]{{-.38842,-.14563,.82463},{-.36642,-.18781,.82091}};
    const pc::Vec3 requested[2]{{-.50550,-.62006,.39583},{-.50537,-.62003,.39579}};
    const pc::Vec3 loggedWrist[2]{{-.48343,-.53060,.47669},{-.48427,-.55440,.46034}};
    const pc::Vec3 bend[2]{{.77216,.31051,-.55439},{.77285,.30103,-.55865}};
    constexpr double upper=.26628,fore=.26153;
    pc::Vec3 actual[2]{};
    bool ready=true,anatomy=true,endpoint=true;
    for(int i=0;i<2;++i){
        const auto root=pc::controller_shoulder103(shoulder[i],requested[i],upper,fore,{0,0,-1});
        const auto arm=root?pc::solve_arm(root->shoulder,requested[i],upper,fore,bend[i],bend[i]):std::nullopt;
        ready=ready&&root&&arm;
        if(!root||!arm)continue;
        actual[i]=arm->hand;anatomy=anatomy&&lengths(*root,*arm,upper,fore);
        endpoint=endpoint&&close(arm->hand,requested[i])&&!arm->reach_limited;
    }
    check(ready&&anatomy&&endpoint,"recorded shoulder jump cannot pull either authoritative wrist target");
    const double oldDelta=pc::length(loggedWrist[1]-loggedWrist[0]);
    const double inputDelta=pc::length(requested[1]-requested[0]);
    const double solvedDelta=pc::length(actual[1]-actual[0]);
    std::printf("F7_PAIR oldWristDeltaM=%.12f requestedDeltaM=%.12f correctedDeltaM=%.12f\n",oldDelta,inputDelta,solvedDelta);
    check(oldDelta>.028&&std::abs(solvedDelta-inputDelta)<1e-9,"F7 replay changes 28.9 mm skeletal jump to the actual 0.139 mm target motion");
}
static void ReachAndPoleSweep(){
    bool solved=true,anatomy=true,endpoints=true;
    const double upper=.26628,fore=.26153;
    double maximumError=0;
    for(int side=0;side<2;++side)for(bool lab:{false,true})
    for(double reach:{0.,.001,.005,.010,.030,.100,.300,.526,.528,.700,1.200})
    for(int step=0;step<72;++step){
        const double a=step*2*pc::armPi/72;
        const pc::Vec3 shoulder{side?-.18:.18,.03,1.40};
        const auto direction=*pc::unit(pc::Vec3{std::cos(a),std::sin(a),.4*std::sin(a*.5)});
        const pc::Vec3 target=shoulder+direction*reach;
        const auto clamp=fl::clamp_triangle(reach,upper,fore);
        const auto root=pc::controller_shoulder103(shoulder,target,upper,fore,direction,
            lab?clamp.distance:-1.,lab?clamp.reserve:-1.);
        if(!root){solved=false;continue;}
        const auto axis=*pc::unit(target-root->shoulder);
        const auto base=*pc::unit(pc::cross(axis,std::abs(axis.z)<.8?pc::Vec3{0,0,1}:pc::Vec3{0,1,0}));
        // Elbow turns through the entire swivel circle, including its other
        // hemisphere. No pole choice is allowed to move the endpoint.
        const auto pole=pc::rotate(pc::from_rotation_vector(axis*a),base);
        const auto arm=pc::solve_arm(root->shoulder,target,upper,fore,pole,pole,lab?clamp.reserve:-1.);
        if(!arm){solved=false;continue;}
        maximumError=std::max(maximumError,pc::length(arm->hand-target));
        anatomy=anatomy&&lengths(*root,*arm,upper,fore);
        endpoints=endpoints&&close(arm->hand,target)&&!arm->reach_limited;

    }
    check(solved&&anatomy,"both arms, close/coincident/far targets and both elbow policies preserve segment lengths");
    check(endpoints&&maximumError<1e-8,"controller wrist stays exact through every reach and 360-degree elbow pole sweep");
    std::printf("SWEEP maximumWristErrorM=%.12f\n",maximumError);
}
static void NativeAdmissionMargin(){
    const double upper=.26628,fore=.26153;
    const double reach=std::sqrt(upper*upper+fore*fore+2*upper*fore*std::cos(2*pc::armPi/180));
    const pc::Vec3 shoulder{},request{reach+.05,0,0};
    const auto root=pc::controller_shoulder103(shoulder,request,upper,fore,{1,0,0},-1.,upper+fore-reach+.002);
    const auto arm=root?pc::solve_arm(root->shoulder,request,upper,fore,{0,1,0},{0,1,0}):std::nullopt;
    check(root&&arm&&pc::length(request-root->shoulder)<=reach-.002&&close(arm->hand,request),
        "native held-body 2 mm admission margin is met by the shoulder without relaxing its gate or moving the wrist");
}
static void ContactAndCovariance(){
    const pc::Vec3 shoulder{},raw{.9,0,0};bool blocked=false;
    const auto allowed=pc::constrain_swept_point({.1,0,0},raw,.01,
        [](pc::Vec3,pc::Vec3)->std::optional<double>{return .30;},blocked);
    const auto root=pc::controller_shoulder103(shoulder,allowed,.25,.25,{1,0,0});
    const auto arm=root?pc::solve_arm(root->shoulder,allowed,.25,.25,{0,1,0},{0,1,0}):std::nullopt;
    check(blocked&&arm&&close(arm->hand,allowed)&&pc::length(arm->hand-raw)>.4,
        "world-contact endpoint remains authoritative; raw controller cannot bypass a surface");
    check(root&&!root->adjusted&&close(root->shoulder,shoulder),"reachable contact endpoint releases unnecessary shoulder correction");
    const pc::Vec3 s{.18,.04,1.4},target{.83,-.05,1.3},fallback{1,0,0};
    const auto first=pc::controller_shoulder103(s,target,.26628,.26153,fallback);
    bool covariant=bool(first);
    for(auto rv:{pc::Vec3{0,0,1.4},pc::Vec3{.2,.6,-.4},pc::Vec3{3.14,0,0}}){
        const auto q=pc::from_rotation_vector(rv);const pc::Vec3 t{10,-2,3};
        const auto moved=pc::controller_shoulder103(pc::rotate(q,s)+t,pc::rotate(q,target)+t,
            .26628,.26153,pc::rotate(q,fallback));
        covariant=covariant&&moved&&close(moved->shoulder,pc::rotate(q,first->shoulder)+t);
    }
    check(covariant,"root accommodation is invariant to world origin and tracking-frame rotation");
    const double nan=std::numeric_limits<double>::quiet_NaN();
    check(!pc::controller_shoulder103(s,{nan,0,0},.26,.26,fallback)&&
          !pc::controller_shoulder103(s,target,0,.26,fallback)&&
          !pc::controller_shoulder103(s,target,.26,.26,fallback,-1.,2.),
          "invalid tracking/anatomy cannot publish a fabricated shoulder or wrist");
}
int main(){RecordedPair();ReachAndPoleSweep();NativeAdmissionMargin();ContactAndCovariance();
    std::printf("controller_authority_r38 %d/%d; native_game_and_headset=NOT_EXECUTED\n",total-failed,total);return failed?1:0;}
