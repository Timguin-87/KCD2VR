#include "vrik/direct_response36.h"
#include <cassert>
#include <limits>
using namespace kcd2vr::response36;
int main(){
 const m::Vec3 torso{0,0,1.1},r{1,0,0},f{0,1,0},u{0,0,1};
 auto near=proximity({-.5,.3,1.1},{.5,.3,1.1},torso,r,f,u,.4);assert(near.known&&near.inside&&near.zone==5);
 assert(!proximity({-.5,.3,1.1},{.5,.3,1.1},torso,r,f,u,.2).inside);
 assert(proximity({-.5,.3,1.1},{.5,.3,1.1},torso,r,f,u,.6).inside);
 assert(!proximity({-.5,-.3,1.1},{.5,-.3,1.1},torso,r,f,u,.6).inside);
 assert(proximity({.25,.1,.8},{.25,.1,1.5},torso,r,f,u,.4).zone==2);
 assert(proximity({-.25,.1,.8},{-.25,.1,1.5},torso,r,f,u,.4).zone==1);
 assert(proximity({-.1,.1,1.4},{.1,.1,1.4},torso,r,f,u,.4).zone==0);
 assert(proximity({-.5,.3,1.1},{.5,.3,1.1},torso,r,f,u,double(.6f)).known);
 assert(proximity({-.5,.1,1.1},{.5,.1,1.1},torso,r,f,u,double(.2f)).known);
 assert(!proximity({}, {},torso,r,f,u,.4).known);
 assert(!proximity({}, {0,1,0},torso,{},f,u,.4).known);
 assert(!proximity({}, {0,1,0},torso,r,f,u,.1).known);
 assert(!proximity({}, {0,1,0},torso,r,f,u,std::numeric_limits<double>::quiet_NaN()).known);
 // Current position sectors: independent of facing; rotate frame with NPC.
 assert(sector36(-.3,0)==Sector36::Left);assert(sector36(.3,0)==Sector36::Right);
 assert(sector36(0,.3)==Sector36::Up);assert(sector36(0,-.3)==Sector36::Down);
 assert(sector36(0,0)==Sector36::Center);
 assert(sector36(std::numeric_limits<double>::quiet_NaN(),0)==Sector36::Unknown);
 auto rear=proximity({-.1,-.3,1.4},{.1,-.3,1.4},torso,r,f,u,.6);
 assert(rear.known&&rear.withinRadius&&!rear.inside&&rear.sector==Sector36::Up&&rear.depth<0);
 auto side=proximity({.3,-.1,1.1},{.3,.1,1.1},torso,r,f,u,.4);
 assert(side.withinRadius&&side.sector==Sector36::Right);
 auto turned=proximity({.1,.3,1.1},{-.1,.3,1.1},torso,{0,1,0},{-1,0,0},u,.4);
 assert(turned.withinRadius&&turned.sector==Sector36::Right);
 assert(reactionTransition(8,64,1));assert(reactionTransition(4,64,1));
 assert(!reactionTransition(8,8,1));assert(!reactionTransition(8,64,2));assert(!reactionTransition(16,64,1));
 assert(comboWindowClosed(1,0));assert(!comboWindowClosed(-1,0));assert(!comboWindowClosed(0,0));
 assert(fresh(100,600));assert(!fresh(100,601));assert(!fresh(100,99));assert(!fresh(0,1));
 assert(opposite(1,2));assert(opposite(0,5));assert(!opposite(-1,2));assert(!opposite(1,1));
}
