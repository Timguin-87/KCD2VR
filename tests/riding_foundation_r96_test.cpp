#include "vrik/tracked_frame51.h"
#include <cstdio>

namespace f=kcd2vr::tracked_frame51;
namespace m=kcd2vr::pose_core;
int main() {
 unsigned checks=0,failed=0;
 const auto check=[&](bool value,const char* why){++checks;if(!value){++failed;std::printf("FAIL %s\n",why);}};
 for(bool smooth:{false,true}) {
  f::State state{};f::Frame out{};f::Input in{};
  in.allowed=true;in.smooth=smooth;in.stamp={8,100,1000,0x100};in.timeNs=1000000000;in.control=smooth?3:2;
  in.restHead={0,.004,1.554};in.restPelvis={0,0,.95};in.calibratedHead={-.005,-.011,1.516};
  in.nativePelvis={0,0,.086};in.nativeHead={0,0,.711};
  in.referenceDomainD86=f::ReferenceDomainD86::Mounted;in.referenceLifetimeD86=10;
  check(f::advance(state,in,out),"first admitted mounted frame uses the current seat");
  const auto anchor=out.anchor;const auto calibration=in.calibratedHead;
  const m::Vec3 controller{.22,.30,-.35};
  for(int n=1;n<=32;++n){
   ++in.stamp.sample;in.stamp.tick+=11;in.timeNs+=11000000;
   in.nativePelvis={.03*std::sin(n*.2),.04*std::cos(n*.2),.086+.10*std::sin(n*.2)};
   in.nativeHead={.10,.20,in.nativePelvis.z+.51};
   const bool valid=f::advance(state,in,out);
   check(valid&&m::length(out.anchor-anchor)<1e-10,"authored seated gait/head sway cannot move the admitted controller foundation");
   check(valid&&m::length(in.nativePelvis+out.bodyOffset-(anchor-(in.restHead-in.restPelvis)))<1e-10,
    "pelvis gets the same current correction as upper body and controller endpoints");
   check(valid&&m::length((out.anchor+controller)-(anchor+controller))<1e-10,
    "tracked controller offset stays direct in the shared seat frame");
  }
  in.control+=2;in.hmdHeight=true;in.nativePelvis.z=.16;
  check(f::advance(state,in,out)&&m::length(out.anchor-anchor)<1e-10,
   "body-height preference change keeps this mounted visit without importing standing height");
  in.control+=2;in.hmdHeight=false;in.smooth=!smooth;
  check(f::advance(state,in,out)&&m::length(out.anchor-anchor)<1e-10,
   "body mode and smoothing toggles cannot silently recapture a gait pose as the seat");
  // New native visit at a model origin two metres away, within the same XR sample.
  in.referenceLifetimeD86=11;in.nativePelvis={0,0,.086-2};in.nativeHead={0,0,.711-2};
  check(f::advance(state,in,out)&&std::fabs(out.anchor.z-(anchor.z-2))<1e-10,
   "remount at a different model-local seat discards the old visit even with the same XR stamp");
  check(m::length(in.calibratedHead-calibration)<1e-10,"replacing the mounted visit does not rewrite calibration");
  in.referenceDomainD86=f::ReferenceDomainD86::Ordinary;in.referenceLifetimeD86=0;
  in.hmdHeight=true;in.nativePelvis=in.restPelvis;in.nativeHead=in.restHead;
  f::State ordinary{};f::Frame ordinaryOut{};
  check(f::advance(state,in,out)&&f::advance(ordinary,in,ordinaryOut)&&m::length(out.anchor-ordinaryOut.anchor)<1e-10,
   "dismount restores the unchanged ordinary frame with no seat/filter carryover");
  in.referenceDomainD86=f::ReferenceDomainD86::Mounted;in.referenceLifetimeD86=12;in.hmdHeight=false;
  in.nativePelvis={0,0,-.5};in.nativeHead={0,0,.1};
  check(f::advance(state,in,out)&&std::fabs(out.anchor.z-(-.5+.604))<1e-10,
   "next mount anchors from its own current native model pose");
  in.allowed=false;
  check(!f::advance(state,in,out)&&!state.ready,"ownership loss retires only presentation state");
  in.allowed=true;in.nativePelvis.z=-.25;in.nativeHead.z=.35;
  check(f::advance(state,in,out)&&std::fabs(out.anchor.z-(-.25+.604))<1e-10,
   "ownership return acquires a current seat and never leaves a permanent failed state");
 }
 std::printf("R96 mounted foundation %u/%u; production header; synthetic poses; game/HMD NOT_RUN\n",checks-failed,checks);
 return failed?1:0;
}
