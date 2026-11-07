#include "vrik/hand_separation_ds1.h"
#include <cstdio>
#include <cmath>
int main(){
 using namespace kcd2vr::hand_separation_ds1;namespace m=kcd2vr::pose_core;int n=0,f=0;
 auto ck=[&](bool b,const char* w){++n;if(!b){++f;std::printf("FAIL %s\n",w);}};
 Limits lim{};
 // far apart: no push
 {auto r=separateHands({0,0,0},{0.5,0,0},lim);ck(!r.pushedHands&&m::length(r.delta[0])==0&&m::length(r.delta[1])==0,"far hands untouched");}
 // exact touch distance (2*0.06 = 0.12): no push
 {auto r=separateHands({0,0,0},{0.12,0,0},lim);ck(!r.pushedHands,"touch distance no push");}
 // overlap: 0.04 apart -> overlap 0.08 -> push 0.04 each, halves outward
 {auto r=separateHands({0,0,0},{0.04,0,0},lim);ck(r.pushedHands,"overlap pushes");
  ck(std::fabs(r.delta[0].x+0.04)<1e-9&&std::fabs(r.delta[1].x-0.04)<1e-9,"halves split");}
 // same point fallback axis +X
 {auto r=separateHands({1,1,1},{1,1,1},lim);ck(r.pushedHands&&r.delta[1].x>0&&r.delta[0].x<0,"same point fallback");}
 // maxPush cap: huge overlap cannot exceed 0.05 per hand
 {auto r=separateHands({0,0,0},{0,0,0},lim);ck(m::length(r.delta[0])<=0.05+1e-9&&m::length(r.delta[1])<=0.05+1e-9,"push capped");}
 // non-finite input refused
 {auto r=separateHands({0,0,0},{NAN,0,0},lim);ck(!r.pushedHands,"nan refused");}
 // weapon: palm on the segment -> pushed weaponSkin away along a normal
 {auto r=separateWeapon({0,0,0},{0,0,-1},{0,0,1},lim);ck(r.pushedWeapon,"on-segment pushed");
  ck(m::length(r.delta[0])>=0.02-1e-9&&m::length(r.delta[0])<=0.05+1e-9,"push is the skin");}
 // weapon: palm already far -> untouched
 {auto r=separateWeapon({0.5,0,0},{0,0,-1},{0,0,1},lim);ck(!r.pushedWeapon,"far palm untouched");}
 // weapon: degenerate segment refused
 {auto r=separateWeapon({0,0,0},{1,1,1},{1,1,1},lim);ck(!r.pushedWeapon,"degenerate segment refused");}
 // weapon: clamp to the segment end (the palm 1.5cm past the tip -> pushed)
 {auto r=separateWeapon({0,0,1.015},{0,0,-1},{0,0,1},lim);ck(r.pushedWeapon,"past-tip pushed outward");}
 std::printf("hand_separation_ds1 %d checks, %d failures\n",n,f);
 return f?1:0;
}
