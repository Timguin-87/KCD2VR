
#include "diagnostics/capture168.h"
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cstring>
namespace diagnostics=kcd2vr::diagnostics;
static unsigned checks=0;void check(bool v){++checks;if(!v){std::printf("FAIL %u\n",checks);std::exit(1);}}
static unsigned snapshots=0,cues=0;static std::uint64_t nowTest=1000;
std::uint64_t GetTickCount64(){return nowTest;}
namespace flashui_probe{void WriteHudCoverage168r1(const char*){++snapshots;}}
namespace hud_layer_v6664{void WriteSnapshot(){++snapshots;}}
void LabLadderCue(unsigned c){cues=c;}
namespace kcd2vr::diagnostics::capture168{void select(Domain d,bool on,std::uint64_t now,const char*){gate.select(d,on,now);}}
struct Press{bool hud=true;};void key(Press capturePress){
        if(capturePress.hud){
            const auto now=GetTickCount64();const bool was=diagnostics::capture168::active(diagnostics::capture168::Domain::Hud,now);
            if(was){flashui_probe::WriteHudCoverage168r1("END_CAPTURE_ONLY");hud_layer_v6664::WriteSnapshot();}
            diagnostics::capture168::select(diagnostics::capture168::Domain::Hud,!was,now,"F2_CAUSAL_CAPTURE_ONLY");
            if(!was){flashui_probe::WriteHudCoverage168r1("BEGIN_CAPTURE_ONLY");hud_layer_v6664::WriteSnapshot();}
            LabLadderCue(was?0:1);
        }

}
int main(){namespace oc=diagnostics::ownership48;using diagnostics::capture168::Domain;
 oc::Event e{};e.tick=nowTest;e.stage=oc::Pose;e.v[0]=3.25f;oc::name(e,"same source");
 check(!oc::active(nowTest));check(!oc::push(e));check(oc::order.load()==0);
 key({});check(oc::active(nowTest));check(cues==1);check(!diagnostics::capture168::active(Domain::Combat,nowTest));check(!diagnostics::capture168::active(Domain::Vrik,nowTest));
 check(oc::push(e));oc::Event out{};check(oc::pop(out));check(out.session==1);check(out.v[0]==3.25f);check(out.order==1);check(out.cpuNs>0);check(!std::strcmp(out.reason,"same source"));
 key({});check(!oc::active(nowTest));check(cues==0);check(!oc::push(e));check(!oc::pop(out));
 key({});e.tick=nowTest;check(oc::push(e));check(oc::pop(out));check(out.session==2);
 nowTest+=diagnostics::capture168::Duration;check(!oc::active(nowTest));e.tick=nowTest;check(!oc::push(e));diagnostics::capture168::gate.poll(nowTest);
 nowTest+=1;key({});e.tick=nowTest;unsigned accepted=0;while(oc::push(e))++accepted;check(accepted==2047);check(oc::dropped.load()==1);
 unsigned got=0;while(oc::pop(out))++got;check(got==accepted);
 auto send=[&](int side){for(unsigned i=0;i<1000;++i){auto t=e;t.side=side;t.sample=i;t.v[0]=float(i);oc::push(t);}};
 std::thread a(send,0),b(send,1);a.join();b.join();std::uint64_t order=0;while(oc::pop(out)){check(out.order>order);check(out.v[0]==float(out.sample));order=out.order;}
 key({});check(!oc::push(e));check(snapshots==10);std::printf("PASS %u actual F2/capture queue checks; bounded drops=%llu\n",checks,(unsigned long long)oc::dropped.load());
}
