#include "../src/vrik/knuckle_contact174.h"
#include "../src/vrik/fist_motion.h"
#include <cassert>
#include <iostream>
using namespace kcd2vr;
int stoppedPunch(bool fresh,int late=0){
    fists::State state{};fists::Sample s{};
    s.actor=s.epoch=s.sequence=1;s.tick=1000;s.valid=s.empty[0]=s.empty[1]=s.boxing167=true;
    s.head={0,1.6,0};s.controller[0]=s.allowed[0]={.2,1.2,-.2};s.controller[1]=s.allowed[1]={-.2,1.2,-.2};
    auto step=[&](){++s.sequence;s.tick+=10;return fists::step(state,s);};
    step();s.grip[0]=s.trigger[0]=1;for(int i=0;i<20;++i)step();
    int events=0;
    for(int i=0;i<2;++i){s.controller[0].z-=.03;s.allowed[0].z-=.03;events+=step().valid;}
    s.blocked[0]=true;s.freshContact174[0]=fresh;
    for(int i=0;i<late;++i)step();
    for(int i=0;i<5;++i){s.controller[0].z-=.03;events+=step().valid;}
    return events;
}
int main(){
    pose_core::FingerCurve103 c[5]{};float input[5]{1,1,1,1,1};
    for(auto& curve:c){curve.count=3;curve.offset[0]={.04,0,0};curve.offset[1]={.05,0,0};curve.offset[2]={.04,0,0};
        curve.closed[1]=pose_core::from_rotation_vector({0,0,3.141592653589793});}
    const auto knuckle=knuckle174::offset(c,input,{});
    assert(knuckle&&std::abs(knuckle->x-.09)<1e-8);
    const auto tip=pose_core::finger_clearance103(c[1],1,{}, {},{{0,0,0},{1,0,0}},0);
    assert(tip.valid&&tip.tip.x<knuckle->x-.03);
    c[2].count=0;assert(!knuckle174::offset(c,input,{}));
    assert(stoppedPunch(true)==1); // one native input request, not claimed damage
    assert(stoppedPunch(false)==0); // UNKNOWN/cached obstruction cannot qualify it
    assert(stoppedPunch(true,5)==0); // pressing through a wall later is not an impact
    std::cout<<"contact174: knuckle geometry and contact-qualified punch intent passed\n";
}
