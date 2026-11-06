#include "vrik/smooth_lab_f2.h"
#include "diagnostics/capture168.h"
#include <cmath>
#include <cstdio>
namespace sl=kcd2vr::smooth_lab;namespace hs=kcd2vr::hand_smooth_h50;namespace h=kcd2vr::hj1;namespace cap=kcd2vr::diagnostics::capture168;
int n=0,failed=0;void ck(bool v,const char*name){++n;if(!v){++failed;printf("FAIL %s\n",name);}}
int main(){
 // Retained D45 assertions explicitly select the original early comparison.
 sl::SetTimingLocationD64(0);
 hs::enabled=false;hs::minCutoffHz=2.f;hs::beta=23.f;sl::ApplyDefaults(false);
 cap::Gate capture;for(auto d:{cap::Domain::Combat,cap::Domain::Interaction,cap::Domain::Vrik,cap::Domain::HandReference})capture.select(d,true,1000);
 unsigned mode=0;
 for(int lap=0;lap<8;++lap)for(unsigned expected:{1u,2u,3u,0u}){
  mode=sl::Next(mode);capture.select(cap::Domain::Bug,sl::BugRecordingRung(mode),1100+lap);sl::SetMode(mode);
  ck(mode==expected&&sl::Mode()==expected,"exact cycle");ck(capture.active(cap::Domain::Bug,1200)==(mode!=0),"D45: all three active rungs record");
  ck(capture.active(cap::Domain::Hud,1200)==(mode!=0),"retained D48 HUD reader follows the one F2 Bug recorder");
  for(auto d:{cap::Domain::Combat,cap::Domain::Interaction,cap::Domain::Vrik,cap::Domain::HandReference})ck(capture.active(d,1200),"other recorder preserved");
  ck((sl::NativeAnchorPolicy()!=0)==false,"D45 old anchor experiment retired");
  ck(sl::StaleContactFastRelease()==false,"D45 old contact experiment retired");
  ck(sl::AllFixesSelected()==false,"D45 old stacked fixes retired");
  ck(sl::NativePelvisYawSelected()==false,"D45 old pelvis experiment retired");
  ck((sl::NativeRejoinPolicyR28()!=0)==false,"D45 old native return experiment retired");
  ck(sl::EarlyPacingSelectedD45()==(mode==2),"D45 early timing only rung2");
  ck(sl::ElbowShoulderSelectedD45()==(mode==3),"D45 elbow shoulder only rung3");
  ck(!h::enabled,"J26: HJ1 retired from the ladder");ck(hs::enabled==false,"J26: no rung turns on the free-hand filter (baseline off here)");
  ck(sl::PacingWanted(sl::SteadyNs())==false,"D45 late pacing retired");ck(!sl::AdaptiveFilterSelected(),"adaptive fast option retired");
 }
 ck(hs::minCutoffHz==2.f&&hs::beta==23.f,"exact baseline restored");
 hs::enabled=true;hs::minCutoffHz=3.f;sl::ApplyDefaults(false);sl::SetMode(3);ck(!hs::enabled,"rung3 does not stack prior filter");hs::enabled=true;hs::minCutoffHz=3.f;sl::ApplyDefaults(false);ck(!h::enabled&&!hs::enabled&&sl::Mode()==3,"settings reload preserves explicit comparison");sl::SetMode(0);ck(hs::enabled&&hs::minCutoffHz==3.f,"explicit preference restored");
 ck(!sl::SetMode(0)&&sl::SetMode(3)&&!sl::SetMode(3),"J26: SetMode reports a real change only (INI start lease)");sl::SetMode(0);
 ck(!sl::BugRecordingRung(0)&&sl::BugRecordingRung(1)&&sl::BugRecordingRung(2)&&sl::BugRecordingRung(3)&&!sl::BugRecordingRung(4),"D45 recording map");
 sl::SetMode(2);hs::enabled=true; // J26: the ladder no longer turns the filter on; these lines test the filter itself
 hs::State s{};float p[3]{},q[4]{0,0,0,1},out[3]{},oq[4]{};
 ck(hs::Step(s,1,1000000000,p,q,out,oq,true),"first sample");p[0]=.002f;hs::Step(s,1,1011111111,p,q,out,oq,true);ck(out[0]<p[0],"small noise filtered");
 const auto prior=s;hs::Step(s,1,1011111111,p,q,out,oq,true);ck(s.out[0]==prior.out[0],"duplicate sample does not advance");
 p[0]=.03f;hs::Step(s,1,1022222222,p,q,out,oq,true);ck(out[0]<p[0]&&p[0]-out[0]<=hs::kMaxLagM+.00001f,"rung2 fast stroke remains filtered within existing cap");
 // Legacy callers still omit the option and retain the original fast-motion bypass.
 hs::State legacy{};float lp[3]{},lo[3]{},lq[4]{};
 hs::Step(legacy,1,1000000000,lp,q,lo,lq);lp[0]=.03f;
 hs::Step(legacy,1,1011111111,lp,q,lo,lq);
 ck(lo[0]==lp[0],"legacy/default fast stroke remains immediately raw");
 // Real pose call supplies the existing F2 mode explicitly; no other caller changes fast response.
 hs::State adaptive{};float ap[3]{},aq[4]{0,0,0,1},aout[3]{},aoq[4]{};
 ck(hs::Step(adaptive,1,1000000000,ap,aq,aout,aoq,true),"rung2 first current pose");
 ap[0]=.03f;aq[1]=std::sin(.1f);aq[3]=std::cos(.1f);
 ck(hs::Step(adaptive,1,1011111111,ap,aq,aout,aoq,true),"rung2 fast pose accepted");
 ck(aout[0]<ap[0]&&ap[0]-aout[0]<=hs::kMaxLagM+.00001f,"rung2 fast position remains adaptive and capped");
 ck(hs::QAngle(aoq,aq)>0&&hs::QAngle(aoq,aq)<=hs::kMaxLagRad+.00002f,"rung2 fast rotation remains adaptive and capped");
 const auto adaptivePrior=adaptive;hs::Step(adaptive,1,1011111111,ap,aq,aout,aoq,true);
 ck(adaptive.out[0]==adaptivePrior.out[0]&&adaptive.dw==adaptivePrior.dw,"rung2 duplicate fast source does not advance");
 p[0]=.031f;hs::Step(s,1,1033333333,p,q,out,oq,true);sl::SetMode(1);sl::SetMode(2);hs::enabled=true;hs::Step(s,1,1033333333,p,q,out,oq,true);ck(out[0]==p[0],"switch discards previous comparison history even at same timestamp");
 for(int rate:{45,75,80,90,105,120}){hs::State f{};float raw[3]{};auto t=2000000000ull;for(int i=0;i<100;++i){t+=1000000000ull/rate;raw[0]=.005f*std::sin(i*.1f);hs::Step(f,2,t,raw,q,out,oq,true);ck(std::fabs(raw[0]-out[0])<=hs::kMaxLagM+.00001f,"bounded lag across frame rates");}}
 sl::Pacing pacing{};auto now=1000000000ull;const auto period=11111111ull;
 for(int i=0;i<20;++i){now+=period;sl::ObserveGate(pacing,true,1000,now,period);}
 ck(pacing.gated==20&&pacing.timeouts==0&&pacing.slow==0,"normal pacing observations exact");
 for(int i=0;i<80;++i){now+=2*period;sl::ObserveGate(pacing,true,1000,now,period);}
 ck(pacing.gated==100&&pacing.slow==80,"long slow run retained as observations");
 ck(sl::EarlyPacingSelectedD45()&&sl::PacingWantedAtD45(now,true),"slow observations cannot withdraw selected timing");
 sl::Pacing timeout{};sl::ObserveGate(timeout,false,20000000,now,period);
 ck(timeout.gated==1&&timeout.timeouts==1&&timeout.waitNsMax==20000000,"timeout records exact bounded attempt");
 ck(sl::EarlyPacingSelectedD45()&&sl::PacingWantedAtD45(now+period,true),"timeout cannot withdraw next-frame timing");
 sl::pacingLock.test_and_set();ck(sl::PacingWantedAtD45(now+period,true),"diagnostic statistics contention cannot revoke requested owner");sl::pacingLock.clear();
 capture.poll(181500);ck(!capture.active(cap::Domain::Bug,181500),"capture expires independently");
 printf("F2 modes %d/%d PASS\n",n-failed,n);return failed?1:0;
}
