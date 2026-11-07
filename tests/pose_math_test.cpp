#include "../src/vrik/pose_math.h"
#include <cstdio>
#include <limits>
using namespace kcd2vr::pose_core;
static int failures=0,tests=0;
void check(bool ok,const char* name) { ++tests; if(!ok) ++failures; std::printf("%s %s\n",ok?"PASS":"FAIL",name); }
bool near(double a,double b,double tolerance=1e-9) { return std::abs(a-b)<tolerance; }
int main() {
    auto a=solve_arm({0,0,0},{.4,0,0},.25,.25,{0,1,0},{0,1,0});
    check(a&&near(length(a->elbow),.25)&&near(length(a->hand-a->elbow),.25),"two-bone lengths preserved");
    auto b=solve_arm({-.1,0,0},{.8,0,0},.25,.25,{0,1,0},{0,1,0});
    check(b&&near(length(b->elbow-Vec3{-.1,0,0}),.25)&&near(length(b->hand-b->elbow),.25)&&near(length(b->hand-Vec3{-.1,0,0}),.5*std::cos(3.14159265358979323846/180)),"moved shoulder does not change anatomy or reach");
    auto c=solve_arm({-.16,0,0},{.8,0,0},.25,.25,{0,1,0},{0,1,0});
    check(c.has_value(),"previous false rejection avoided");
    auto same=solve_arm({0,0,0},{.4,0,0},.25,.25,{0,1,0},{0,1,0});
    check(a&&same&&near(length(a->elbow-same->elbow),0),"fixed target and fixed pole are deterministic; no native elbow input");
    auto singular=solve_arm({0,0,0},{.4,0,0},.25,.25,{1,0,0},{0,1,0});
    check(singular&&near(length(singular->elbow-a->elbow),0),"degenerate pole uses previous valid bend");
    check(!solve_arm({0,0,0},{.4,0,0},.25,.25,{1,0,0},{1,0,0}),"ambiguous bend fails without arbitrary pole flip");
    check(!solve_arm({0,0,0},{.4,0,0},std::numeric_limits<double>::quiet_NaN(),.25,{0,1,0},{0,1,0}),"nonfinite anatomy rejected");
    // Three child links are rebuilt under their current parents; rotation changes,
    // but each local translation remains the same fixed three centimetres.
    bool chain_ok=true;
    for(int step=0;step<=100;++step) {
        Vec3 p{}; Quat parent{};
        for(int joint=0;joint<3;++joint) {
            auto local=blend_finger({}, {0,0,std::sqrt(.5),std::sqrt(.5)}, {.03,0,0},step/100.0);
            Vec3 child=p+rotate(parent,local->fixed_offset);
            chain_ok=chain_ok&&near(length(child-p),.03);
            p=child; parent=multiply(parent,local->rotation);
        }
    }
    check(chain_ok,"finger parent-first reconstruction preserves all segment lengths across 101 curl values");
    auto q=slerp({0,0,0,1},{0,0,0,-1},.5);
    check(q&&near(std::abs(q->w),1),"opposite quaternion signs do not corkscrew");
    check(!blend_finger({0,0,0,0},{0,0,0,1},{.03,0,0},.5),"missing or zero quaternion donor rejected");
    bool split=true;
    for(auto f:{Finger::Thumb,Finger::Middle,Finger::Ring,Finger::Pinky}) split=split&&near(finger_input(f,1,0),1)&&near(finger_input(f,0,1),0);
    split=split&&near(finger_input(Finger::Index,1,0),0)&&near(finger_input(Finger::Index,0,1),1);
    check(split,"grip controls thumb and 3-5; trigger controls index");
    // Identity-frame example of H + (C-H): translating the HMD alone cannot move C.
    Vec3 C{.2,-.4,-.1},H1{0,0,0},H2{.1,.2,.3};
    check(near(length((H1+(C-H1))-(H2+(C-H2))),0),"head-relative decomposition does not turn a fixed controller into a head-owned target");
    const double pi=3.14159265358979323846;
    Quat yaw=from_rotation_vector({0,0,pi/2});
    Quat chest=axis_weighted_delta(yaw,{.1,.1,.4},{pi,pi,pi});
    Vec3 cv=rotation_vector(chest);
    check(std::abs(cv.x)<1e-8&&std::abs(cv.y)<1e-8&&near(cv.z,.4*pi/2,1e-7),"axis-weighted trunk preserves yaw axis and applies yaw share");
    Quat pitch=from_rotation_vector({pi/2,0,0});
    Quat limited=axis_weighted_delta(pitch,{1,1,1},{10*pi/180,pi,pi});
    check(near(std::abs(rotation_vector(limited).x),10*pi/180,1e-7),"axis-specific trunk limit caps pitch without Euler coupling");
    bool blocked=false;
    Vec3 clamped=constrain_swept_point({0,0,0},{1,0,0},.05,
        [](Vec3,Vec3)->std::optional<double>{return .40;},blocked);
    check(blocked&&near(clamped.x,.35)&&near(clamped.y,0)&&near(clamped.z,0),"collision clamp stops allowed endpoint before surface while retaining skin");
    Vec3 free=constrain_swept_point({0,0,0},{1,0,0},.05,
        [](Vec3,Vec3)->std::optional<double>{return std::nullopt;},blocked);
    check(!blocked&&near(free.x,1),"collision miss preserves requested controller endpoint");
    std::printf("active_math=%d/%d; retail_ABI=NOT_CALLED\n",tests-failures,tests);
    return failures?1:0;
}
