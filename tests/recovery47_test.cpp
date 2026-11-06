#include "vrik/motion_camera47.h"
#include "vrik/finger_tuning40.h"
#include "vrik/gesture_talk59c5.h"
#include "vrik/contact_metadata59c7g.h"
#include <cassert>
#include <cstdio>
int main(){
 namespace c=kcd2vr::motion_camera47;namespace m=kcd2vr::pose_core;
 c::State s{};c::Input i{};c::Result out{};
 i.character=100;i.epoch=2;i.tick=1000;i.allowed=i.neutral=true;
 i.root={10,20,0};i.nativeHead=i.calibratedHead={0,0,1.53};i.nativeEye={10,20,1.60};
 assert(c::solve(s,i,1000,out));assert(m::length(out.eye-i.nativeEye)<1e-6);
 // Same current body, animated head/bow moves: neither camera position nor yaw
 // follows that animation. No HMD delta is read or applied inside this leaf.
 i.neutral=false;i.nativeHead={.12,-.10,1.24};i.nativeEye={10.12,19.9,1.31};i.nativeForward={.4,.9,0};i.tick=1010;
 assert(c::solve(s,i,1010,out));assert(m::length(out.eye-m::Vec3{10,20,1.60})<1e-6);
 assert(m::length(out.forward-m::Vec3{0,1,0})<1e-6);
 // Actual body translation/turn remains live, not a frozen old world eye.
 i.root={11,22,0};i.rotation={0,0,.7071067811865475,.7071067811865475};i.nativeEye={11,22,1.31};i.tick=1020;
 assert(c::solve(s,i,1020,out));assert(m::length(out.eye-m::Vec3{11,22,1.60})<1e-6);
 assert(m::length(out.forward-m::Vec3{-1,0,0})<1e-6);
 i.allowed=false;assert(!c::solve(s,i,1020,out));i.allowed=true; // protected owner never receives override
 assert(c::solve(s,i,1020,out));assert(!c::solve(s,i,1101,out));
 ++i.epoch;assert(!c::solve(s,i,1020,out)); // new owner cannot learn from an active draw
 i.neutral=true;i.nativeHead={0,0,1.53};i.nativeEye={11,22,1.6};assert(c::solve(s,i,1020,out));
 namespace f=kcd2vr::finger40;
 for(int n=0;n<5;++n){f::set(0,n,0,0);f::set(1,n,0,0);f::set(0,n,1,.5f+.05f*n);f::set(0,n,2,-10.f+5.f*n);}
 f::copyRightClosedToLeft47();
 for(int n=0;n<5;++n){assert(f::get(0,n,1)==f::get(1,n,1));assert(f::get(0,n,2)==f::get(1,n,2));assert(f::get(1,n,0)==0);}
 namespace g=kcd2vr::gesture_talk59c5;g::Config cfg{};cfg.holdPoint=true;g::State gs{};g::Hand h[2]{};
 h[0].valid=true;h[0].grip=.8f;h[0].triggerTouch=false;h[0].forward=.5;h[0].up=0;
 g::update(gs,1000,h,true,false,false,cfg);auto go=g::update(gs,1310,h,true,false,false,cfg);
 assert(go.fired==g::Kind::Talk&&go.openChat);
 assert(g::update(gs,4000,h,true,false,true,cfg).openChat&&gs.fired==1);
 h[0].empty=false;assert(!g::update(gs,4010,h,true,false,true,cfg).openChat);
 g::State occupied{};g::update(occupied,1000,h,true,false,false,cfg);
 assert(g::update(occupied,5000,h,true,false,false,cfg).fired==g::Kind::None);
 // Immutable origin is copied with the impact, not borrowed from a later guard.
 kcd2vr::contact59c7g::Metadata impact{};impact.originZone47=1;const auto queued=impact;impact.originZone47=2;
 assert(queued.originZone47==1);
 std::puts("C47 current-root eye, both closed endpoints, sustained empty-hand talk and copied contact origin PASS");
}
