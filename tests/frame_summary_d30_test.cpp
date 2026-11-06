// D30: F2 FRAME / F7 SUMMARY aggregation and the "all logs gated behind F binds" routes.
#include "diagnostics/frame_summary_d30.h"
#include <cstdio>
#include <limits>
#include <cmath>
#include <cstring>
#include <string>
#include <vector>
using namespace kcd2vr::diagnostics;
static unsigned checks=0,failures=0;
static void check(bool ok,const char* what){++checks;if(!ok){++failures;std::printf("FAIL %s\n",what);}}
int main(){
    namespace cap=capture168;namespace fd=frame_d30;
    // window order statistics
    fd::Window w{};for(int i=1;i<=100;++i)w.add(float(i));
    check(w.pct(.5)>=50&&w.pct(.5)<=51,"p50");check(w.pct(.95)>=95&&w.pct(.95)<=96,"p95");check(w.mx==100.f,"max");
    w.add(std::numeric_limits<float>::quiet_NaN());check(w.total==100,"nan ignored");
    // D45: source sequences in the capture advance by four, not by one.
    // Unequal intervals at constant velocity must still produce zero acceleration.
    fd::Acceleration j{};check(j.push(0,0,0,100,1'000'000'000)<0,"needs triple");
    check(j.push(.01,0,0,104,1'010'000'000)<0,"needs triple 2");
    check(std::abs(j.push(.025,0,0,108,1'025'000'000))<1e-9,"nonuniform constant velocity, stride four");
    check(j.push(999,0,0,108,1'025'000'000)<0,"duplicate ignored");
    check(j.push(999,0,0,104,1'010'000'000)<0,"old sample ignored");
    check(std::abs(j.push(.04,0,0,112,1'040'000'000))<1e-9,"ignored samples do not corrupt history");
    fd::Acceleration k{};k.push(0,0,0,1,1'000'000'000);k.push(0,0,0,5,1'010'000'000);
    check(std::abs(k.push(.01,0,0,9,1'020'000'000)-100.)<1e-6,"10mm step over10ms is100mps2");
    check(k.push(.01,0,0,13,1'200'000'000)<0,"time gap resets even when sequence stride is stable");
    check(k.push(.01,0,0,17,1'210'000'000)<0,"gap requires new triple");
    check(k.push(.01,0,0,21,1'220'000'000)==0,"new triple after gap");
    check(k.push(.01,0,0,25,1'219'000'000)<0,"regressed source time rejected");
    check(k.push(.01,0,0,29,1'230'000'000)<0,"bad clock resets history");
    fd::Acceleration a{};a.push(0,0,0,10,1'000'000'000);a.push(.0001,0,0,14,1'010'000'000);
    check(std::abs(a.push(.000625,0,0,18,1'025'000'000)-2.)<1e-9,"known quadratic trajectory with unequal intervals");
    check(a.push(std::numeric_limits<double>::quiet_NaN(),0,0,22,1'035'000'000)<0,"invalid point resets motion");
    check(a.push(0,0,0,26,0)<0,"missing source time is unavailable");
    // lease gating: nothing recorded while F2/F7 are off
    const std::uint64_t now=1000000;
    fd::gameFrame(1'000'000'000ull,now);fd::gameFrame(1'011'000'000ull,now);check(fd::state.frames==0,"off: no frames");
    cap::gate.select(cap::Domain::Bug,true,now);
    fd::gameFrame(2'000'000'000ull,now);fd::gameFrame(2'011'000'000ull,now);fd::gameFrame(2'022'000'000ull,now);
    check(fd::state.frames==3,"F2 frames counted");check(fd::state.frameMs.n==2,"two intervals");
    fd::PoseSample p{};p.tick=now;p.committed=true;
    for(int i=0;i<5;++i){p.sequence=10+4*i;p.timeNs=3'000'000'000ull+10'000'000ull*i;p.rawValid[1]=true;p.raw[1][0]=.001*i;p.finalValid[0]=true;p.finalWrist[0][0]=(i%2)*.004;fd::pose(p);}
    check(fd::state.rawAccel[0].n==3,"raw acceleration samples with captured stride");check(fd::state.finalAccel[0].n==3,"final acceleration samples");
    check(fd::state.finalAccel[0].pct(.95)>79.9f,"final zigzag is measured");check(fd::state.rawAccel[0].pct(.95)<1e-3f,"raw linear has zero acceleration");
    const auto rawBefore=fd::state.rawAccel[0].n;
    p.motionContext.control=1;p.sequence+=4;p.timeNs+=10'000'000;p.raw[1][0]=100;fd::pose(p);
    check(fd::state.rawAccel[0].n==rawBefore&&fd::state.rawMotion[0].have==1,"control change breaks derivative history");
    p.motionContext.sourceEpoch=1;p.sequence=2;p.timeNs=1'000'000'000;fd::pose(p);
    check(fd::state.rawMotion[0].have==1,"new source epoch accepts reset sequence and clock");
    p.rawValid[0]=true;p.sequence+=4;p.timeNs+=10'000'000;fd::pose(p);
    p.rawValid[0]=false;fd::pose(p);check(fd::state.headMotion.have==0,"invalid head breaks history");
    char formatted[1024]{};fd::External noExternal{};
    fd::formatF2(formatted,sizeof(formatted),1.,fd::state,noExternal);
    check(std::strstr(formatted,"handRawAccelP95Mps2=0.000/N/A")&&std::strstr(formatted,"headSamples=0"),"available zero differs from missing data");
    check(!std::strstr(formatted,"Jerk"),"second derivative is not mislabeled jerk");
    cap::gate.select(cap::Domain::Bug,false,now);
    cap::gate.select(cap::Domain::Bug,true,now);
    p.sequence+=4;p.timeNs+=10'000'000;fd::pose(p);
    check(fd::state.rawAccel[0].n==0&&fd::state.finalAccel[0].n==0,"new session discards old capture window");
    check(fd::state.rawMotion[0].have==1,"new session starts a fresh motion triple");
    check(fd::state.poseFrames==0,"F7 off: no F7 stats");
    std::vector<std::string> lines;auto write=[&](const char* l){lines.push_back(l);};
    fd::External e{};e.xrFrames=90;e.fresh=80;e.reused=10;e.logLines=5;
    fd::report(now,e,write);check(lines.empty(),"first report primes");
    e.xrFrames=180;e.fresh=160;e.reused=20;e.ourUs=3000;e.logLines=25;e.handoffNs=4'000'000;e.handoffCalls=2;
    fd::report(now+1000,e,write);
    check(lines.size()==1&&std::strncmp(lines[0].c_str(),"F2 FRAME ",9)==0,"one F2 line");
    check(lines.size()==1&&std::strstr(lines[0].c_str(),"xrFps=90.0")&&std::strstr(lines[0].c_str(),"reusePct=11.1")&&std::strstr(lines[0].c_str(),"handoffMs=2.00"),"F2 fields");
    check(fd::state.frames==0&&fd::state.finalAccel[0].n==0,"window reset after report");
    check(lines.size()==1&&std::strstr(lines[0].c_str(),"handFinalAccelP95Mps2=N/A/N/A")&&std::strstr(lines[0].c_str(),"finalSamplesRL=0/0"),"empty reporting window explicitly unavailable");
    // F7
    cap::gate.select(cap::Domain::Vrik,true,now+1000);
    fd::PoseSample q{};q.tick=now+1000;q.traced=true;q.transactionUs=800;q.lockWaitUs=50;q.prepareUs=200;q.frameValid=true;q.goalValid[0]=true;q.goalOffsetMm[0]=12;q.planes[1]=2;q.elbowOk[1]=false;
    fd::pose(q);q.frameValid=false;fd::pose(q);
    lines.clear();fd::report(now+2100,e,write);
    bool f7=false;for(auto& l:lines)if(std::strncmp(l.c_str(),"F7 SUMMARY ",11)==0){f7=true;
        check(std::strstr(l.c_str(),"poseFrames=2")&&std::strstr(l.c_str(),"anchorSourceSwitches=1")&&std::strstr(l.c_str(),"frame51ValidPct=50.0")&&std::strstr(l.c_str(),"elbowRefusedRL=0/2"),"F7 fields");}
    check(f7,"F7 line present");
    // routes: quiet mode drops formerly essential per-event lines, F binds bring them back
    cap::gate.select(cap::Domain::Bug,false,now);cap::gate.select(cap::Domain::Vrik,false,now);
    check(!cap::essential("W3 XR_TIMING part=CADENCE x")&&!cap::essential("C47 IMPALE stage=REQUESTED")&&!cap::essential("R59.16C10R15 STOWED_BOW hidden")&&
          !cap::essential("R47C1 DIALOGUE_POSE active=1")&&!cap::essential("T8 INVENTORY gate")&&!cap::essential("R59C5 NATIVE_SERVICE seq=1"),"quiet mode no longer essential");
    check(!cap::essential("KCD2VR BUILD X")&&!cap::essential("V66.168 CAPTURE domain=BUG")&&!cap::essential("R59C5 NATIVE_SERVICE_INSTALL raised=1")&&!cap::essential("X FAULT y"),"identity/capture/fault have no unconditional bypass");
    const std::uint64_t t=now+5000;
    cap::gate.select(cap::Domain::Combat,true,t);
    check(cap::accept("C47 IMPALE stage=REQUESTED",t)&&cap::accept("C32 GORE x",t)&&!cap::accept("F2 FRAME x",t)&&!cap::accept("F5 HELD x",t),"F3 routes");
    cap::gate.select(cap::Domain::Combat,false,t);cap::gate.select(cap::Domain::Interaction,true,t);
    check(cap::accept("F5 HELD side=0",t)&&cap::accept("F5 ANIM actor=HENRY",t)&&cap::accept("R47C1 DIALOGUE_POSE a",t)&&!cap::accept("C47 IMPALE stage=x",t),"F5 routes");
    cap::gate.select(cap::Domain::Interaction,false,t);cap::gate.select(cap::Domain::Reload,true,t);
    check(cap::accept("F11 RANGED event=X",t)&&cap::accept("F11 RANGED_STATE stage=EMPTY",t)&&cap::accept("XR32 CROSSBOW_ZOOM a",t)&&cap::accept("R59.16C10R15 STOWED_BOW x",t),"F11 routes");
    cap::gate.select(cap::Domain::Reload,false,t);cap::gate.select(cap::Domain::Vrik,true,t);
    check(cap::accept("F7 SUMMARY x",t)&&!cap::accept("F2 FRAME x",t),"F7 routes");
    cap::gate.select(cap::Domain::Vrik,false,t);cap::gate.select(cap::Domain::Bug,true,t);
    check(cap::accept("F2 FRAME x",t)&&cap::accept("W3 XR_TIMING part=CADENCE",t)&&cap::accept("J26 PACING rung=3",t),"F2 routes");
    std::printf("frame_summary_d30 %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
