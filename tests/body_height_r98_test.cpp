// R98 body height modes: HMD HEIGHT / NATIVE HEIGHT / HMD + NATIVE (RIDE/CROUCH).
// Production header only; synthetic poses. Game/HMD NOT_RUN.
#include "vrik/tracked_frame51.h"
#include <cstdio>
#include <cmath>

namespace f=kcd2vr::tracked_frame51;
namespace m=kcd2vr::pose_core;
static unsigned checks=0,failed=0;
static void check(bool ok,const char* why){++checks;if(!ok){++failed;std::printf("FAIL %s\n",why);}}
static f::Input standing(){
 f::Input in{};in.allowed=true;in.stamp={8,100,1000,0x100};in.timeNs=1000000000;in.control=2;
 in.restHead={0,.004,1.554};in.restPelvis={0,0,.95};in.calibratedHead={-.005,-.011,1.516};
 in.nativePelvis=in.restPelvis;in.nativeHead=in.restHead;in.hmdHeight=true;return in;
}
static void next(f::Input& in,unsigned ms=11){++in.stamp.sample;in.stamp.tick+=ms;in.timeNs+=std::uint64_t(ms)*1000000;}
int main(){
 const double crouchDrop=.40; // retail crouch pelvis drop seen in R97 log (anchor 1.554 -> 1.154)
 // 1. COMBINED standing is exactly HMD height.
 {f::State a{},b{};f::Frame fa{},fb{};auto hmd=standing();auto comb=hmd;comb.combinedHeightR98=true;
  for(int n=0;n<20;++n){hmd.nativePelvis.z=comb.nativePelvis.z=.95+.04*std::sin(n*.9); // walk bob
   check(f::advance(a,hmd,fa)&&f::advance(b,comb,fb)&&std::fabs(fa.anchor.z-fb.anchor.z)<1e-12,"combined on foot = HMD height, walk bob removed");
   check(std::fabs(fb.anchor.z-1.554)<1e-12&&fb.nativeHeightWeightR98==0,"combined standing weight 0");next(hmd);next(comb);}}
 // 2. COMBINED crouch blends to native height in ~0.25 s, never jumps; stand blends back.
 {f::State s{};f::Frame out{};auto in=standing();in.combinedHeightR98=true;in.nativeStanceR93=true;
  check(f::advance(s,in,out)&&std::fabs(out.anchor.z-1.554)<1e-12,"standing receipt keeps HMD height");
  double last=out.anchor.z,maxStep=0;in.nativeCrouchedR93=true;
  for(int n=1;n<=40;++n){next(in);in.nativePelvis.z=.95-crouchDrop*std::min(1.,n/30.);
   const bool ok=f::advance(s,in,out);check(ok,"crouch blend frame admitted (no residual rejection)");
   maxStep=std::max(maxStep,std::fabs(out.anchor.z-last));last=out.anchor.z;}
  check(std::fabs(out.anchor.z-(1.554-crouchDrop))<1e-9&&out.nativeHeightWeightR98==1,"crouched: native height");
  check(maxStep<.03,"crouch edge has no jump (per 11 ms frame < 3 cm)");
  check(out.heightDrivenR98&&out.combinedHeightR98&&out.nativeCrouchOffsetR93==0,"combined crouch is native height, not the -0.5 m button offset");
  in.nativeCrouchedR93=false;maxStep=0;
  for(int n=1;n<=40;++n){next(in);in.nativePelvis.z=.95-crouchDrop*std::max(0.,1.-n/30.);
   check(f::advance(s,in,out),"stand blend frame admitted");maxStep=std::max(maxStep,std::fabs(out.anchor.z-last));last=out.anchor.z;}
  check(std::fabs(out.anchor.z-1.554)<1e-9&&out.nativeHeightWeightR98==0,"stood up: HMD height again");
  // Standing up moves both the native pelvis (0.4 m over 30 frames) and the
  // blend weight (0.25 s): bound = both per-frame steps added, ~3.1 cm.
  check(maxStep<crouchDrop*(11./250.)+crouchDrop/30.+1e-6,"stand edge has no jump (only the 0.25 s blend + native rise)");}
 // 3. A missing receipt holds the last answer up to 0.5 s, then returns to HMD (no latch).
 {f::State s{};f::Frame out{};auto in=standing();in.combinedHeightR98=true;in.nativeStanceR93=true;in.nativeCrouchedR93=true;
  in.nativePelvis.z=.95-crouchDrop;f::advance(s,in,out);check(std::fabs(out.anchor.z-1.154)<1e-9,"fresh crouched frame takes native height directly");
  in.nativeStanceR93=false;in.nativeCrouchedR93=false;
  for(int n=0;n<40;++n){next(in);f::advance(s,in,out);} // 440 ms
  check(std::fabs(out.anchor.z-1.154)<1e-9,"brief receipt gap keeps crouch height");
  for(int n=0;n<40;++n){next(in);f::advance(s,in,out);}
  check(std::fabs(out.anchor.z-1.554)<1e-9,"long receipt gap returns to HMD height; nothing is latched");}
 // 4. Rider evidence (seated/mount/dismount clip) = native height while on foot frames occur.
 {f::State s{};f::Frame out{};auto in=standing();in.combinedHeightR98=true;in.nativeRiderR98=true;in.nativePelvis.z=.95+.30;
  check(f::advance(s,in,out)&&std::fabs(out.anchor.z-1.854)<1e-9,"rider gap frame uses native height immediately (no 2 m HMD drop)");
  in.nativeRiderR98=false;double last=out.anchor.z,maxStep=0;
  for(int n=0;n<40;++n){next(in);in.nativePelvis.z=.95+.30*std::max(0.,1.-n/20.);check(f::advance(s,in,out),"dismount blend admitted");
   maxStep=std::max(maxStep,std::fabs(out.anchor.z-last));last=out.anchor.z;}
  check(std::fabs(out.anchor.z-1.554)<1e-9&&maxStep<.03,"after riding: smooth return to HMD height");}
 // 5. Mounted seat foundation unchanged by COMBINED (R96).
 {f::State a{},b{};f::Frame fa{},fb{};auto in=standing();in.referenceDomainD86=f::ReferenceDomainD86::Mounted;in.referenceLifetimeD86=10;
  in.hmdHeight=false;in.nativePelvis={0,0,.086};auto comb=in;comb.hmdHeight=true;comb.combinedHeightR98=true;
  check(f::advance(a,in,fa)&&f::advance(b,comb,fb)&&std::fabs(fa.anchor.z-fb.anchor.z)<1e-12&&!fb.heightDrivenR98,"mounted seat identical in every mode");}
 // 6. Weapon same frame: NATIVE height shares the anchor delta with the hands.
 for(int mode=0;mode<3;++mode){
  f::State s{};f::Frame prev{},cur{};auto in=standing();in.hmdHeight=mode!=1;in.combinedHeightR98=mode==2;in.control=2+4*mode;
  f::advance(s,in,prev);bool same=true,bob=false,r97Missed=false;
  for(int n=1;n<30;++n){next(in);in.nativePelvis.z=.95+.04*std::sin(n*.9);f::advance(s,in,cur);
   const auto d=f::shared_frame_delta_r98(cur,prev);
   const auto hands=(cur.anchor-prev.anchor); // hand goals = anchor + fixed controller offset
   if(std::fabs(d.z-hands.z)>1e-9)same=false;
   if(std::fabs(hands.z)>1e-4)bob=true;
   if(std::fabs(hands.z)>1e-4&&f::native_stance_delta_r93(cur,prev).z==0)r97Missed=true;
   prev=cur;}
  check(same,mode==0?"HMD: weapon delta = hand delta":mode==1?"NATIVE: weapon motor moves with the walk-bob frame like the hands":"COMBINED: weapon delta = hand delta");
  check(bob==(mode==1),"only NATIVE height carries the walk bob");
  check(r97Missed==(mode==1),"R97 cause: the stance-only delta gave the weapon zero of the native bob the hands got");}
 // 7. HMD mode keeps the exact R93 behaviour (no new shared offset).
 {f::State s{};f::Frame out{};auto in=standing();in.nativePelvis.z=.99;f::advance(s,in,out);
  check(!out.heightDrivenR98&&out.heightOffsetR98==0&&f::shared_height_offset_r98(out)==out.nativeCrouchOffsetR93,"HMD mode offset unchanged");}
 // 8. Mode switches are generation changes: every mode works after any other.
 {f::State s{};f::Frame out{};auto in=standing();in.nativeStanceR93=true;in.nativeCrouchedR93=true;in.nativePelvis.z=.95-crouchDrop;
  const int modes[]{2,0,1,2,1,0,2};for(int k=0;k<7;++k){in.control+=2;in.hmdHeight=modes[k]!=1;in.combinedHeightR98=modes[k]==2;next(in);
   const bool ok=f::advance(s,in,out);
   if(modes[k]==0)check(ok&&std::fabs(out.anchor.z-(1.554-.5))<1e-9&&out.nativeCrouchOffsetR93==-.5,"HMD after switch keeps the exact R93 button stance");
   else check(ok&&std::fabs(out.anchor.z-1.154)<1e-9,"native/combined crouch after any switch");}}
 // 9. COMBINED pickup while crouched keeps the reference (native) height; no 0.4 m pop.
 {f::State s{};f::Frame ref{},out{};auto in=standing();in.combinedHeightR98=true;in.nativeStanceR93=true;in.nativeCrouchedR93=true;
  in.nativePelvis.z=.95-crouchDrop;check(f::advance(s,in,ref)&&std::fabs(ref.anchor.z-1.154)<1e-9,"crouched reference");
  next(in);in.acquisitionSerial=7;in.acquisitionAction=1;in.acquisitionScope=1;in.acquisitionAnchor=ref.anchor;
  in.acquisitionCrouchOffsetR93=ref.nativeCrouchOffsetR93;
  check(f::advance(s,in,out)&&std::fabs(out.anchor.z-1.154)<1e-9,"pickup keeps crouched native height");
  next(in);in.acquisitionSerial=0;in.acquisitionAction=0;in.acquisitionScope=0;
  check(f::advance(s,in,out)&&std::fabs(out.anchor.z-1.154)<1e-9,"after pickup still crouched, no jump");}
 std::printf("R98 body height %u/%u; production header; synthetic poses; game/HMD NOT_RUN\n",checks-failed,checks);
 return failed?1:0;
}
