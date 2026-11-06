#include "../src/vrik/archer_pose169r2.h"
#include <cstdio>
using namespace kcd2vr::archer169r2;
static int checks=0,failures=0;static void ck(bool v,const char* n){++checks;if(!v)++failures;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
int main(){
    auto goal=retarget({.2,0,1.4},{0,0,1.7},{.01,.02,1.68},{.22,-.10,-.16},{.2,-.1,-.25},.9);
    ck(goal&&m::length(*goal-m::Vec3{.228,.02,1.461})<1e-10,"retarget uses fixed neutral shoulder plus HMD translation, not rotated shoulder");
    ck(!retarget({0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},0),"invalid arm scale cannot create a wrist");
    State s{};advance(s,true,1,2,3,4,1,1000,0,0,-1,0);
    for(int i=1;i<20;++i)advance(s,true,1,2,3,4,i+1,1000+i*10,0,0,-1,0);
    ck(s.correction==0,"stationary trigger/no pull adds no archer yaw");
    auto a=advance(s,true,1,2,3,4,21,1200,0,0,-1,1);
    ck(a<0&&a>=-.018000001,"first physical draw is rate limited rather than chest snap");
    const auto copy=s;auto duplicate=advance(s,true,1,2,3,4,21,1201,0,0,1,1);
    ck(duplicate==a&&s.tick==copy.tick,"duplicate pose consumer does not advance yaw or its clock");
    for(int i=22;i<150;++i)advance(s,true,1,2,3,4,i,1000+i*10,0,0,-1,1);
    ck(std::abs(s.correction+1)<.001,"sustained draw approaches side-on torso");
    const auto still=s.correction;advance(s,true,1,2,3,4,150,2500,0,0,1,1,false);
    ck(s.correction==still,"held tracking sample may keep visualization but not advance torso intent");
    auto lost=advance(s,false,1,2,3,4,151,2510,0,0,-1,1);
    ck(lost==0&&!s.ready,"invalid ownership/tracking grants no stale archer state");
    advance(s,true,1,2,3,4,200,3000,0,0,-1,1);advance(s,true,1,2,3,4,201,3010,0,0,-1,1);
    ck(advance(s,true,9,2,3,4,202,3020,0,0,-1,1)==0,"new item starts new posture state");
    ck(std::abs(yaw_goal({1,0,0},{.2,-.2,1.5},{-.2,.4,1.5},0))<=75*pi/180,"absolute shoulder-line target bounded to 75 degrees");
    ck(std::abs(yaw_goal({1,0,0},{1,-1,1.5},{0,0,1.5},0)+.4*pi/4)<1e-9,"moderate body participation is 40 percent of measured draw-plane yaw");
    ck(std::abs(yaw_goal({1,0,0},{-1,0,1.5},{0,0,1.5},0))<=30*pi/180+1e-9,"bow torso remains capped at 30 degrees, no wrist/camera write");
    const m::Vec3 shoulder{.2,0,1.4},hand{.15,-.12,1.59},bow{-.2,.4,1.4},fallback{1,0,-1};
    const auto pole=elbow_preference(shoulder,hand,bow,.27,.26,fallback,{0,0,.8},{0,0,1.4},.11,{0,0,1.65});
    const auto arm=m::solve_arm(shoulder,hand,.27,.26,pole,pole);
    ck(arm&&m::length(arm->hand-hand)<1e-8,"archer elbow choice cannot drag a reachable wrist");
    ck(arm&&std::abs(m::length(arm->elbow-shoulder)-.27)<1e-8&&std::abs(m::length(arm->hand-arm->elbow)-.26)<1e-8,"both arm lengths remain exact");
    std::printf("archer169r2=%d/%d\n",checks-failures,checks);return failures?1:0;
}
