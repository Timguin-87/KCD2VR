#include "vrik/tracked_frame51.h"
#include <cstdio>
using namespace kcd2vr::tracked_frame51;
int main(){
 int checks=0,fail=0;const auto ck=[&](bool ok,const char* n){++checks;if(!ok){++fail;std::printf("FAIL %s\n",n);}};
 State s{};Frame out{};Input in{};in.allowed=true;in.smooth=true;in.stamp={1,1,1000,99};in.control=7;in.timeNs=1000000000;
 in.restHead={0,0,1.7};in.restPelvis={0,0,1.};in.nativeHead=in.restHead;in.nativePelvis=in.restPelvis;in.calibratedHead=in.restHead;in.eyeNeutral=true;
 ck(advance(s,in,out)&&out.neutral&&ordinary_reference_d86(out),"ordinary head/body may teach neutral and donate reference");
 // The dice scene stays on the same current actor, body epoch and controller
 // calibration. Only the activity domain/lifetime changes.
 ++in.stamp.sample;in.stamp.tick+=11;in.timeNs+=11000000;in.referenceDomainD86=ReferenceDomainD86::Dice;in.referenceInstanceD86=123;in.referenceLifetimeD86=10;
 in.nativePelvis.z=.7;in.nativeHead.z=1.4;
 ck(advance(s,in,out)&&out.valid&&!out.neutral&&!ordinary_reference_d86(out),"dice retains tracked frame without neutral/pickup authority");
 ck(std::abs(out.anchor.z-1.4)<1e-12,"entry seeds current native-height presentation without prior ordinary filter");
 for(int n=0;n<20;++n){++in.stamp.sample;in.stamp.tick+=11;in.timeNs+=11000000;ck(advance(s,in,out)&&!out.neutral,"dice cannot become neutral even if caller requests neutral");}
 ++in.stamp.sample;in.stamp.tick+=11;in.timeNs+=11000000;in.referenceDomainD86=ReferenceDomainD86::Ordinary;in.referenceInstanceD86=0;in.referenceLifetimeD86=0;
 in.nativePelvis=in.restPelvis;in.nativeHead=in.restHead;
 ck(advance(s,in,out)&&out.neutral&&ordinary_reference_d86(out),"leaving dice restores ordinary reference eligibility");
 ck(std::abs(out.anchor.z-1.7)<1e-12,"dice native-height history cannot persist into ordinary pose");
 ck(m::length(s.calibratedHead-in.restHead)<1e-12&&in.control==7,"lifecycle retirement does not recenter or change XR calibration");
 for(auto domain:{ReferenceDomainD86::Mounted,ReferenceDomainD86::Workstation,ReferenceDomainD86::Other}){
  ++in.stamp.sample;in.stamp.tick+=11;in.timeNs+=11000000;in.referenceDomainD86=domain;
  ck(advance(s,in,out)&&out.valid&&!out.neutral&&!ordinary_reference_d86(out),"other body owner cannot train ordinary eye or pickup reference");
 }
 // A new dice visit with the same pointers must also retire the previous
 // session's state; same-source stamp cannot bypass a changed lifetime.
 in.referenceDomainD86=ReferenceDomainD86::Dice;in.referenceInstanceD86=123;in.referenceLifetimeD86=11;in.nativePelvis.z=.7;in.nativeHead.z=1.4;
 ck(advance(s,in,out)&&std::abs(out.anchor.z-1.4)<1e-12,"first dice visit gets its own frame");
 in.referenceLifetimeD86=12;in.nativePelvis.z=.9;in.nativeHead.z=1.6;
 ck(advance(s,in,out)&&std::abs(out.anchor.z-1.6)<1e-12,"new visit replaces same-stamp activity cache");
 std::printf("tracked_reference_d86=%d/%d\n",checks-fail,checks);return fail?1:0;
}
