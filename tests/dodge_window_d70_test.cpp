#include "vrik/dodge_window_d70.h"
#include <cstdio>
#include <limits>
using namespace kcd2vr::dodge_window_d70;
struct IO {float value=.65f;int reads=0,writes=0;bool readable=true,writable=true,sideEffect=false;bool failReadAfterWrite=false;
 bool Read(const char*,float&v){++reads;if(!readable)return false;v=value;return true;}
 bool Write(const char*,float v){++writes;if(writable||sideEffect)value=v;if(failReadAfterWrite)readable=false;return writable;}};
static int checks=0,failed=0;static void ck(bool v,const char*n){++checks;if(!v){++failed;std::printf("FAIL %s\n",n);}}
int main(){auto log=[](const char*,const char*,float,float){};
 ck(Factor()==0,"OFF default native untouched");Lease l{};IO io{};l.service(true,1000,io,log);ck(io.reads==0&&io.writes==0,"OFF does not read native CVar");
 SetFactor(2);l.service(true,1100,io,log);ck(Near(io.value,1.3f)&&l.owns(),"2x observednative0.65 window");const int reads=io.reads,writes=io.writes;l.service(true,5000,io,log);ck(io.reads==reads&&io.writes==writes,"stable ownership no polling");
 l.service(false,5100,io,log);ck(Near(io.value,.65f)&&!l.owns(),"UI/context exit restores original");l.service(true,5200,io,log);ck(Near(io.value,1.3f),"context reentry reapplies same option");SetFactor(3);l.service(true,5300,io,log);ck(Near(io.value,1.95f)&&Near(l.original,.65f),"factor change restoresoldthenusesoriginal notcompounded");
 io.value=.9f;SetFactor(0);l.service(false,5400,io,log);ck(Near(io.value,.9f)&&!l.owns()&&l.foreign,"foreign owner not overwritten atretirement");l.service(true,5700,io,log);ck(Near(io.value,.9f),"context toggle doesnot clearforeignstanddown");SetFactor(2);l.service(true,5800,io,log);ck(Near(io.value,1.8f)&&Near(l.original,.9f),"explicitnew userfactor acquires currentnativevalue");
 SetFactor(0);io.readable=false;l.service(false,5900,io,log);ck(l.owns(),"unreadable restore retainsdebt");io.readable=true;io.writable=false;l.service(false,6200,io,log);ck(l.owns()&&Near(io.value,1.8f),"failedrestore retainsdebt");io.writable=true;io.failReadAfterWrite=true;l.service(false,6500,io,log);ck(l.owns()&&Near(io.value,.9f),"uncertain readback retainsdebt evenwrite returned");io.readable=true;io.failReadAfterWrite=false;l.service(false,6800,io,log);ck(!l.owns()&&Near(io.value,.9f),"next readconfirm recognizes actualrestore");
 for(bool side:{false,true}){Lease q{};IO a{};SetFactor(2);a.writable=false;a.sideEffect=side;q.service(true,7000,a,log);ck(q.owns(),"acquisition failure holdspossiblewriteobligation");SetFactor(0);a.writable=true;q.service(false,7300,a,log);ck(!q.owns()&&Near(a.value,.65f),"OFF cleanspossible acquisition sideeffect");}
 {Lease q{};IO a{};SetFactor(2);q.service(true,8000,a,log);a.value=.8f;q.rearmRestore();q.service(true,8300,a,log);ck(q.foreign&&!q.owns()&&Near(a.value,.8f),"samefactor foreign owner retireslease");q.service(false,8600,a,log);q.service(true,8900,a,log);ck(q.foreign&&Near(a.value,.8f),"menu exit/reentry cannot fight foreignwriter");}
 {Lease q{};IO a{};SetFactor(1);q.service(true,9000,a,log);ck(!q.owns()&&a.writes==0,"1x doesnotclaim anotherowner nativevalue");}
 SetFactor(std::numeric_limits<float>::quiet_NaN());ck(Factor()==0,"NaN off");SetFactor(9);ck(Factor()==3,"maximum3x");SetFactor(.5);ck(Factor()==1,"positivebelowoneclamps1x");SetFactor(0);
 std::printf("D70 native dodge CVar lease %d/%d; explicit mocks for qualifiednativeconsole\n",checks-failed,checks);return failed?1:0;
}
