#include "vrik/physical_weapon.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
int main(){
    namespace p=physical;
    p::Input in{};in.shoulders[0]={.2,0,1.5};in.shoulders[1]={-.2,0,1.5};
    for(bool oldPreference:{false,true}){
        p::Profile x{};x.legacyHolsters147=oldPreference;
        for(int c:{0,1,2,3,4,5,6,7,8,9,10,13,14,15,16,17}){
            x.classId=c;p::Vec3 point{};
            assert(p::holster_hand126(x)==((c==8||c==9||c==10||c==14||c==15||c==17)?1:0));
            if(c==0){assert(p::holster_zone126(x)==p::HolsterZone126::LeftChest45);assert(p::holster_mount126(in,x,point));assert(point.x<0&&point.y>0);}
            if(c==7){assert(p::holster_zone126(x)==p::HolsterZone126::RightShoulder);assert(p::holster_mount126(in,x,point));assert(point.x>0);}
            if(c==10||c==13||c==14||c==15){assert(p::holster_zone126(x)==p::HolsterZone126::LeftBack45);assert(p::holster_mount126(in,x,point));assert(point.x<0&&point.y<0&&point.z>1.5&&point.z<1.6);} // HS2: behind the TOP of the left shoulder (was 22 cm under it)
            if(c==3||c==5||c==6){assert(p::holster_zone126(x)==p::HolsterZone126::RightHip45);assert(p::holster_mount126(in,x,point));assert(point.x>0&&point.z<1.1);}
        }
    }
    equipment114::State hs{};equipment114::Input hi{};
    hi.key=12;hi.generation=hi.epoch=1;hi.tick=1000;hi.eligible=hi.drawn=true;
    hi.releaseAnywhere=true;hi.releaseDebounceMs=0;hi.grip=1;
    assert(equipment114::update(hs,hi)==equipment114::Request::None);
    ++hi.tick;hi.grip=0;
    assert(equipment114::update(hs,hi)==equipment114::Request::Holster);
    for(int i=0;i<30;++i){++hi.tick;assert(equipment114::update(hs,hi)==equipment114::Request::None);}
    hi.drawn=false;++hi.tick;assert(equipment114::update(hs,hi)==equipment114::Request::None);
    // Production release-presentation option: no sweep/contact/attack output,
    // exact current owner only, bounded and immediately revoked on tracking loss.
    p::State state{};state.epoch=1;state.bodyEpoch=2;
    auto& w=state.weapon[0];w.ready=true;w.key=12;w.generation=3;w.weapon.p={.2,.3,1};
    std::array<p::Profile,2> profiles{};auto& x=profiles[0];x.key=12;x.generation=3;x.side=0;x.primary=true;x.classId=1;x.weight=1;x.extent=1;
    in.epoch=1;in.bodyEpoch=2;in.tick=1000;in.sample=1;in.allowed=true;in.releasePresentation45=true;
    in.requirePrimaryGrip114=true;in.tracked[0]=in.intentTracked[0]=true;in.grip[0]=0;
    int queries=0;auto sweep=[&](const p::Profile&,p::Pose,p::Pose,bool){++queries;return p::Contact103{};};
    auto out=p::step(state,in,profiles,sweep);
    assert(out.active[0]&&out.owned[0]&&queries==0&&!out.strokeActive119&&!out.contact119[0].plane);
    in.tick=1200;++in.sample;out=p::step(state,in,profiles,sweep);assert(out.active[0]&&queries==0);
    in.tracked[0]=false;++in.tick;++in.sample;out=p::step(state,in,profiles,sweep);assert(!out.active[0]);
    state.weapon[0].ready=true;state.weapon[0].key=12;state.weapon[0].generation=3;
    in.tracked[0]=true;in.tick=2000;++in.sample;out=p::step(state,in,profiles,sweep);assert(out.active[0]);
    in.tick=2251;++in.sample;out=p::step(state,in,profiles,sweep);assert(!out.active[0]);
    std::puts("C45 fixed zones, native primary hands, one immediate sheath and bounded visual release PASS");
}
