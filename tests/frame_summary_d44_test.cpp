// Original D43 packets retained; D46 measures acceleration using source time.
// Historical D44 residual-quantity fixture remains in its frozen source archive.
#include "diagnostics/frame_summary_d30.h"
#include <cstdio>
#include <cmath>
#include <cstring>
namespace fd=kcd2vr::diagnostics::frame_d30;
unsigned checks=0,failed=0;
void check(bool ok,const char* name){++checks;if(!ok){++failed;std::printf("FAIL %s\n",name);}}
int main(){
 fd::Acceleration raw{},shown{};
 const std::uint64_t seq[]{221038,221042,221046},ns[]{1271361533576100ull,1271361544593300ull,1271361555606800ull};
 const double source[3][3]{{.08978076,-.2574857,-.4052716},{.09108017,-.2555134,-.4053468},{.09225403,-.2536808,-.4060905}};
 const double final[3][3]{{.09492579,.3584362,1.293758},{-.02416678,.332785,1.294756},{.09686951,.3594611,1.296622}};
 double r=-1,f=-1;
 for(int i=0;i<3;++i){r=raw.push(source[i][0],source[i][1],source[i][2],seq[i],ns[i]);f=shown.push(final[i][0],final[i][1],final[i][2],seq[i],ns[i]);}
 check(r>=0&&f>=0,"recorded publication stride produces an acceleration sample");
 check(r<10&&f>1900&&f/r>300,"recorded owner wrist jump distinguished from controller motion");
 std::printf("D43 packets rawAccelMps2=%.6f finalAccelMps2=%.6f\n",r,f);
 namespace cap=kcd2vr::diagnostics::capture168;
 const auto now=1000000ull;cap::gate.mask=0;fd::PoseSample p{};p.tick=now;p.sequence=100;p.timeNs=1000000000;p.motionContext.poseEpoch=8;p.motionContext.sourceEpoch=6;
 p.rawValid[1]=true;p.finalValid[0]=true;p.committed=true;
 fd::pose(p);check(fd::state.rawMotion[0].have==0,"off stays quiet");
 cap::gate.select(cap::Domain::Bug,true,now);
 for(int i=0;i<3;++i){p.sequence=seq[i];p.timeNs=ns[i];for(int j=0;j<3;++j){p.raw[1][j]=source[i][j];p.finalWrist[0][j]=final[i][j];}fd::pose(p);}
 check(fd::state.rawAccel[0].n==1&&fd::state.finalAccel[0].n==1,"actual pose route receives recorded source-clock triple");
 check(std::abs(fd::state.rawAccel[0].pct(.95)-r)<.001&&std::abs(fd::state.finalAccel[0].pct(.95)-f)<.001,"pose route and direct numerical replay agree");
 p.sequence+=4;p.timeNs+=11000000;++p.motionContext.poseEpoch;fd::pose(p);
 check(fd::state.rawMotion[0].have==1,"body epoch resets derivative");
 p.sequence=1;p.timeNs=1000000000;++p.motionContext.sourceEpoch;fd::pose(p);
 check(fd::state.rawMotion[0].have==1,"tracking reset accepts new clock and sequence");
 for(auto member:{&fd::MotionContext::character,&fd::MotionContext::policy,&fd::MotionContext::control,&fd::MotionContext::filterPolicy,&fd::MotionContext::elbowPolicy}){
  p.sequence+=4;p.timeNs+=11000000;++(p.motionContext.*member);fd::pose(p);
  check(fd::state.rawMotion[0].have==1,"context transition resets derivative without manufactured acceleration");
 }
 cap::gate.select(cap::Domain::Bug,false,now);cap::gate.select(cap::Domain::Bug,true,now+1);p.tick=now+1;p.sequence+=4;p.timeNs+=11000000;fd::pose(p);
 check(fd::state.rawAccel[0].n==0&&fd::state.rawMotion[0].have==1,"new recording inherits neither history nor measurements");
 fd::State empty{};fd::External ex{};char msg[1200]{};fd::formatF2(msg,sizeof(msg),1.,empty,ex);
 check(std::strstr(msg,"handRawAccelP95Mps2=N/A/N/A")&&std::strstr(msg,"finalSamplesRL=0/0"),"unavailable is never zero motion");
 std::printf("frame_summary_d44 recorded replay %u checks %u failures\n",checks,failed);return failed?1:0;
}
