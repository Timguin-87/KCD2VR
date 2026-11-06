#include "vrik/user_modes175.h"
#include "vrik/fist_motion.h"
#include "vrik/roomscale58.h"
#include "vrik/contact_projection44.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
static fists::Sample sample(){
    fists::Sample x{};x.epoch=x.sequence=1;x.tick=1000;x.actor=123;
    x.valid=x.empty[0]=x.empty[1]=true;x.head={0,1.6,0};
    x.controller[0]=x.allowed[0]={.2,1.25,-.2};
    x.controller[1]=x.allowed[1]={-.2,1.25,-.2};
    x.openPalm44=x.contactStop175=x.boxing167=true;return x;
}
static fists::Punch next(fists::State& s,fists::Sample& x){++x.tick;++x.sequence;x.tick+=9;return fists::step(s,x);}
static int slap(bool enabled,bool contact,double speed,bool occupied=false){
    fists::State s{};auto x=sample();x.openPalm44=enabled;
    if(occupied)x.empty[1]=false;
    for(int i=0;i<25;++i)assert(!next(s,x).valid);
    int events=0;
    for(int i=0;i<15;++i){
        x.controller[0].z-=speed*.01;x.allowed[0].z-=speed*.01;
        x.blocked[0]=x.freshContact174[0]=contact&&i>=7;
        events+=next(s,x).valid;
    }
    for(int i=0;i<20;++i)events+=next(s,x).valid;
    return events;
}
static bool dodge(double distance,int duration,bool combat=true){
    roomscale58::State s{};roomscale58::Config c{};
    for(int t=0;t<=duration;t+=10)roomscale58::observeHmd(s,1000+t,distance*t/duration,0);
    const auto d=roomscale58::drive(s,1000+duration,distance,0,false,combat,true,c);
    assert(!d.following&&d.lx==0&&d.ly==0&&s.anchorX==0&&s.anchorY==0);
    return d.dodge;
}
int main(){
    using namespace user175;
    const auto off=hybridCurrent19c4r2(true,false,true);
    assert(off==Mode::Weapons);
    const auto cfg=configuration(off);
    assert(!cfg.experiment&&cfg.pose==4&&cfg.weapon==7);
    assert(configuration(Mode::Off).pose==0); // explicit all-off is still possible
    assert(configuration(Mode::Calibrated).pose==4);
    assert(hybridCurrent19c4r2(false,false,false)==Mode::CombatSynchronized);
    assert(slap(true,true,2.0)==1);
    assert(slap(true,false,2.0)==0); // no air-slap attack
    assert(slap(false,true,2.0)==0);
    assert(slap(true,true,.8)==0); // touching/petting is not an attack
    assert(slap(true,true,2.0,true)==0); // held weapon/prop excludes fists
    assert(dodge(.105,200));
    assert(!dodge(.09,200));
    assert(dodge(.16,600));
    assert(dodge(.14,600)); // C54 user threshold:10cm within1s, not the old15cm/650ms
    assert(!dodge(.08,1000));
    assert(!dodge(.16,600,false));
    // Real corner constraints: projection is feasible, stable and independent
    // of ray order, including an oblique corner the old two-pass push violated.
    using contact_projection44::Plane;
    Plane planes[3]{{{1,0,0},.1},{{-.5,.866025403784,0},.1},{{0,0,1},.05}};
    pose_core::Vec3 p{},q{};
    assert(contact_projection44::project({-.2,-.2,-.2},planes,3,p));
    for(const auto& c:planes)assert(pose_core::dot(p,c.normal)>=c.minimum-1e-7);
    std::swap(planes[0],planes[2]);
    assert(contact_projection44::project({-.2,-.2,-.2},planes,3,q));
    assert(pose_core::length(p-q)<1e-7);
    for(int i=0;i<1000;++i){assert(contact_projection44::project(p,planes,3,q));assert(pose_core::length(p-q)<1e-7);}
    Plane impossible[2]{{{1,0,0},1},{{-1,0,0},1}};
    assert(!contact_projection44::project({},impossible,2,p));
    assert(!contact_projection44::project({},planes,8,p));
    std::puts("C44 ownership, contact-qualified slap and fast/slow dodge checks passed");
}
