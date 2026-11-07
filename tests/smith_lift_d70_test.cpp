#ifndef D70_ANVIL_HEADER
#define D70_ANVIL_HEADER "subsystems/smithing/anvil_a2.h"
#endif
#include D70_ANVIL_HEADER
#include <cstdio>
#include <cstdlib>
namespace av=kcd2vr::subsystems::smithing::anvil_a2;
#define CHECK(x) do{++checks;if(!(x)){std::printf("FAIL line %d\n",__LINE__);return 1;}}while(0)
int main(){unsigned checks=0;av::Anvil a{};a.locked=a.box=true;a.boxMin={-.5,-.5,0};a.boxMax={.5,.5,.8};a.top=.8;
 const av::Probe tip{{.4,0,0},.02};const av::Pose from{{0,0,.8190001},{0,0,0,1}};
 // Translation rises20mm while rotation lowers the tip14mm: every true point
 // rises. Old independent minima falsely clamp this whole move at fraction0.
 const double angle=.03490658503988659;
 const av::Pose to{{0,0,.8390001},{0,std::sin(angle*.5),0,std::cos(angle*.5)}};
 auto s=av::solve(a,from,to,&tip,1);CHECK(!s.corrected&&s.fraction==1);
 for(int i=0;i<=1000;++i)CHECK(av::penetration(a,av::lerp(from,to,double(i)/1000),&tip,1)<=av::kClear);
 // Actual incoming motion still stops:3mm rise cannot outrun14mm tip dip.
 auto into=to;into.p.z=from.p.z+.003;s=av::solve(a,from,into,&tip,1);CHECK(s.corrected&&s.fraction<1);
 CHECK(av::penetration(a,s.pose,&tip,1)<=av::kClear+1.e-9);
 // General separating-plane claims must survive an independent dense path.
 for(int ai=-18;ai<=18;++ai)for(int zi=0;zi<=12;++zi){
  const double t=ai*.015;av::Pose end{{0,0,from.p.z+zi*.003},{0,std::sin(t*.5),0,std::cos(t*.5)}};
  if(av::staysAboveTopD39(a,from,end,tip))for(int j=0;j<=100;++j)
   CHECK(av::penetration(a,av::lerp(from,end,double(j)/100),&tip,1)<=av::kClear+1.e-9);
 }
 std::printf("D70 smith separating lift: %u checks PASS\n",checks);
}
