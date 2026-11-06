// V66.178R59.2 fault pause (pure header). Log sink and checks only.
#include <string>
#include <vector>
#include <cstdio>
static std::vector<std::string> logs;
#include "../src/vrik/fault_pause59.h"
static void capture(const char* m) noexcept {logs.emplace_back(m);}
static int fails=0;static void ck(bool ok,const char* w){std::printf("%s %s\n",ok?"PASS":"FAIL",w);if(!ok)++fails;}
int main(){
    using namespace kcd2vr::fault59;sink=&capture;
    FaultPause p{"unit"};
    ck(!p.active(1000),"fresh: not paused");
    ck(p.trip(1000,"SEH")==2000&&p.active(1000)&&p.active(2999),"first fault pauses 2 s");
    ck(!p.active(3000)&&logs.size()==2&&logs[0].find("FAULT_PAUSE begin subsystem=unit pauseMs=2000 strike=1")!=std::string::npos&&logs[1].find("FAULT_PAUSE end subsystem=unit")!=std::string::npos,"the pause ends by itself and both edges are logged with the name");
    ck(p.trip(3000)==4000&&p.trip(7000)==8000&&p.trip(15000)==16000&&p.trip(31000)==32000&&p.trip(63000)==60000&&p.trip(123000)==60000,"repeats double to the 60 s cap");
    ck(p.trip(123000+60000+kForgetMs+1)==2000,"a quiet 120 s forgets the series");
    ck(p.total()==8,"every fault is counted");
    FaultPause q{"atomic-lookalike"};
    ck(!q.exchange(true)&&q.exchange(true)&&q.load(),"exchange(true) trips once and reports the previous state");
    q.store(false);ck(!q.load(),"store(false) clears");
    q=true;ck(q.load()&&q.strikes()==2,"assignment trips and the strike count carries on inside the forget window");
    std::printf("%s\n",fails?"FAILED":"ALL PASSED");return fails?1:0;
}
