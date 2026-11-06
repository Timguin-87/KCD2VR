// C38: over-reach hard limit of the production VRIK arm chain + the pure reach/visibility receipt logic.
// The chain is exactly what coherent_pose.inl runs per arm: soft_reach_cap124 -> limit_close_reach103 -> solve_arm.
#include "../src/vrik/upper_body.h"
#include "../src/vrik/reach_view38.h"
#include <cmath>
#include <cstdio>
namespace pc=kcd2vr::pose_core;namespace rv=kcd2vr::reach_view38;
static int fails=0;
#define CHECK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
int main(){
    const double upper=.2663,fore=.2615,arm=upper+fore; // Henry's fixed arm (Miki's log: fixedArm=0.2663/0.2615)
    const pc::Vec3 shoulder{.18,-.01,1.26},pole{0,0,-1};
    // 1) Over-reach far ahead / over an edge / sideways / up / down: the hand never drops; it stays at full extension on the
    //    shoulder->controller line (hard limit), the solve reports reach_limited, and the arm is straight within 2 degrees.
    const pc::Vec3 dirs[]{{0,1,0},{0,1,-1},{0,1,.6},{1,1,0},{-.3,1,-.2},{0,.2,-1},{.9,.1,.3}};
    const double asks[]{1.001,1.05,1.3,2.0,5.0,40.0};
    for(const auto& d0:dirs)for(double ask:asks){
        const auto d=*pc::unit(d0);const pc::Vec3 controller=shoulder+d*(arm*ask);
        const auto goal=pc::limit_close_reach103(shoulder,pc::soft_reach_cap124(shoulder,controller,arm),upper,fore);
        CHECK(pc::finite(goal));
        const double g=pc::length(goal-shoulder);
        CHECK(g<=arm*.9995+1e-9&&g>=arm*.995-1e-9);                 // compressed into the last 0.5% of reach, never beyond
        CHECK(pc::dot(*pc::unit(goal-shoulder),d)>.999999);         // same direction as the controller
        const auto s=pc::solve_arm(shoulder,goal,upper,fore,pole,pole);
        CHECK(s.has_value());if(!s)continue;
        const double h=pc::length(s->hand-shoulder);
        CHECK(std::fabs(h-std::min(g,arm))<2e-3);                   // hand at the clamped point: full extension
        CHECK(pc::dot(*pc::unit(s->hand-shoulder),d)>.999999);
        CHECK(std::fabs(pc::length(s->elbow-shoulder)-upper)<1e-6&&std::fabs(pc::length(s->hand-s->elbow)-fore)<1e-6); // fixed bones
        CHECK(pc::finite(s->elbow)&&pc::finite(s->hand));
    }
    // A raw goal beyond reach (no soft cap) still clamps in solve_arm and flags reach_limited.
    {const auto s=pc::solve_arm(shoulder,shoulder+pc::Vec3{0,3,0},upper,fore,pole,pole);
     CHECK(s&&s->reach_limited&&std::fabs(pc::length(s->hand-shoulder)-arm)<2e-3&&s->hand.y>shoulder.y);}
    // 2) Inside the envelope the goal is untouched (1:1 controller) and solved exactly.
    for(double ask:{.40,.70,.90,.99}){
        const pc::Vec3 controller=shoulder+pc::Vec3{0,1,0}*(arm*ask);
        const auto goal=pc::limit_close_reach103(shoulder,pc::soft_reach_cap124(shoulder,controller,arm),upper,fore);
        CHECK(pc::length(goal-controller)<1e-12);
        const auto s=pc::solve_arm(shoulder,goal,upper,fore,pole,pole);
        CHECK(s&&!s->reach_limited&&pc::length(s->hand-controller)<1e-9);
    }
    // Continuity: the soft knee is monotonic and has no jump at 99.5%.
    {double prev=0;for(int i=0;i<=400;++i){const double ask=.98+i*.0001;
        const auto goal=pc::soft_reach_cap124(shoulder,shoulder+pc::Vec3{0,ask*arm,0},arm);const double g=pc::length(goal-shoulder);
        CHECK(g>=prev-1e-12);CHECK(i==0||g-prev<1.1e-4*arm);prev=g;}}
    // Degenerate inputs never produce NaN.
    CHECK(pc::finite(pc::soft_reach_cap124(shoulder,shoulder,arm)));
    CHECK(pc::finite(pc::soft_reach_cap124(shoulder,shoulder+pc::Vec3{0,1,0},0.)));
    // 3) Receipt geometry. Native standing body box; an eye leaned 35 cm forward is outside, hands ahead of it are visible.
    rv::Box b{};rv::grow(b,{-.25,-.15,0});rv::grow(b,{.25,.15,1.75});rv::grow(b,{NAN,0,0});
    CHECK(rv::valid(b)&&b.n==2);
    CHECK(rv::outside(b,{0,0,1.6})==0);
    CHECK(std::fabs(rv::outside(b,{0,.50,1.6})-.35)<1e-12);
    CHECK(std::fabs(rv::outside(b,{.55,.55,1.85})-std::sqrt(.09+.16+.01))<1e-12);
    CHECK(rv::outside(rv::Box{},{0,0,0})<0);
    const rv::V eye{0,.50,1.55},look{0,1,0};
    CHECK(rv::behindEye(b,eye,look));                               // whole native body behind the eye plane -> a frustum test fails
    CHECK(!rv::behindEye(b,eye,{0,.5,-1}));                          // looking down: the native body is in view again
    CHECK(!rv::behindEye(b,{0,0,1.6},look));                         // eye inside the body: never 'behind'
    CHECK(std::fabs(rv::ahead(eye,look,{.1,.95,1.3})-.45)<1e-12);
    CHECK(rv::ahead(eye,look,{.1,.40,1.3})<0);                       // a hand behind the eye plane can never be seen
    CHECK(rv::ahead(eye,{0,0,0},{1,1,1})==0);
    // 4) Hysteresis edges: one start, one end, nothing in between.
    rv::Flag f{};int starts=0,ends=0;const double seq[]{0,.05,.11,.12,.08,.07,.11,.05,.02,.2};
    for(double v:seq){const int e=rv::edge(f,v,.10,.06,1);starts+=e>0;ends+=e<0;}
    CHECK(starts==2&&ends==1&&f.on);
    CHECK(rv::edge(f,NAN,.10,.06,1)==0&&f.on);
    // 5) Skinning gap: starts once after the threshold, ends when skinning returns or VRIK stops being eligible.
    rv::Gap g{};std::uint64_t t=1000;int gs=0,ge=0;
    for(int i=0;i<10;++i){t+=11;const int e=rv::gap(g,t,t,true,120);gs+=e>0;ge+=e<0;} // skinned every frame
    CHECK(gs==0&&ge==0);
    const std::uint64_t lastSkin=t;
    for(int i=0;i<30;++i){t+=11;const int e=rv::gap(g,t,lastSkin,true,120);gs+=e>0;ge+=e<0;}
    CHECK(gs==1&&ge==0&&g.open&&g.since==lastSkin);
    t+=11;CHECK(rv::gap(g,t,t,true,120)==-1&&!g.open);
    const std::uint64_t fresh=t;                                                  // an older copy never rewinds the last skin
    for(int i=0;i<5;++i){t+=11;CHECK(rv::gap(g,t,lastSkin,true,120)==0);}
    for(int i=0;i<30;++i){t+=11;(void)rv::gap(g,t,lastSkin,true,120);}
    CHECK(g.open&&g.since==fresh&&g.lastSkin==fresh);
    rv::Gap h{};CHECK(rv::gap(h,5000,0,true,120)==0);                            // never skinned yet: no gap claim
    CHECK(rv::gap(h,5000,4000,false,120)==0);                                    // not eligible (menu): no gap
    CHECK(rv::gap(h,5000,4000,true,120)==1);CHECK(rv::gap(h,5010,0,false,120)==-1);
    if(fails)return 1;
    std::printf("reach_view38_test OK\n");return 0;
}
