#include "subsystems/interactions/washing_d59.h"
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
namespace wash=kcd2vr::washing_d59;
static std::vector<std::string> calls;
static bool probeFails=false,commitUnknown=false,readerCurrent=true;
static unsigned probeSmartObject=900;
bool kcd2vr::native_script_t2::Context::Current() const noexcept {return true;}
bool kcd2vr::native_script_t2::Context::CallPacket(const char* global,const char* in,char*out,std::size_t cap)const noexcept {
    assert(std::string(global)=="kcd2vr_wash_d59");calls.emplace_back(in);
    char op[16]{};unsigned long long s=0;unsigned p=0,t=0;
    assert(std::sscanf(in,"%15[^|]|%llu|%u|%u",op,&s,&p,&t)==4);
    if(std::string(op)=="PROBE") {
        if(probeFails){std::snprintf(out,cap,"OK|%llu|NATIVE_REFUSED",s);return true;}
        std::snprintf(out,cap,"OK|%llu|READY|%u|%u|-.3|-.3|.5|.3|.3|1.0",s,t,probeSmartObject);return true;
    }
    if(std::string(op)=="COMMIT"&&commitUnknown){out[0]=0;return false;}
    std::snprintf(out,cap,"OK|%llu|%s",s,std::string(op)=="COMMIT"?"DISPATCHED":"CANCELLED");return true;
}
void kcd2vr::diagnostic_trace::Write(const char*) noexcept {}
static bool reader(const wash::Frame&,std::uint64_t)noexcept{return readerCurrent;}
static wash::Frame frame(std::uint64_t n){
    wash::Frame f{};f.allowed=true;f.player=42;f.target=99;f.actor=100;f.character=101;
    f.body=1;f.tracking=2;f.policy=3;f.tick=f.hmdTick=f.handTick[0]=f.handTick[1]=f.targetTick=n;
    f.openEmptyMask=3;f.hmd[1]=-.5f;f.hmd[2]=1.7f;f.forward[1]=1;
    for(int i=0;i<2;++i){f.palm[i][0]=i?.12f:-.12f;f.palm[i][1]=0;f.palm[i][2]=.9f;}return f;
}
static wash::Volume volume(std::uint64_t n){wash::Volume v{};v.target=99;v.smartObject=900;v.tick=n;
    v.lo[0]=v.lo[1]=-.3f;v.lo[2]=.5f;v.hi[0]=v.hi[1]=.3f;v.hi[2]=1.f;return v;}
static void atFace(wash::Frame&f){for(auto&p:f.palm){p[1]=-.38f;p[2]=1.64f;}f.target=0;f.targetTick=0;}
static int count(const char*op){int n=0;for(auto&s:calls)if(s.rfind(op,0)==0)++n;return n;}
static void service(wash::Frame f){wash::Publish(f);kcd2vr::native_script_t2::Context c{1,2,f.tick,1};if(wash::Wanted())wash::Service(c);}
static void reset(std::uint64_t now){wash::SetEnabled(false);service(frame(now));wash::Publish({});wash::SetEnabled(true);calls.clear();probeFails=false;commitUnknown=false;readerCurrent=true;probeSmartObject=900;}
int main(){
    wash::Gesture g{};auto f=frame(1000);assert(g.Begin(f,volume(1000),1000));
    for(unsigned n=1000;n<=1140;n+=20){f=frame(n);assert(g.Advance(f,volume(n),n)!=wash::Step::Face);}
    int fired=0;for(unsigned n=1160;n<=1380;n+=20){f=frame(n);atFace(f);if(g.Advance(f,volume(n),n)==wash::Step::Face)++fired;}assert(fired==1);
    g.Reset();f=frame(2000);atFace(f);f.target=99;f.targetTick=2000;assert(g.Begin(f,volume(2000),2000));
    for(unsigned n=2000;n<2300;n+=20){f=frame(n);atFace(f);assert(g.Advance(f,volume(n),n)!=wash::Step::Face);}
    g.Reset();f=frame(3000);assert(g.Begin(f,volume(3000),3000));f=frame(3100);assert(g.Advance(f,volume(3100),3100)==wash::Step::Cancelled);
    f=frame(4000);assert(g.Begin(f,volume(4000),4000));f.openEmptyMask=1;assert(g.Advance(f,volume(4000),4000)==wash::Step::Cancelled);
    f=frame(5000);assert(g.Begin(f,volume(5000),5000));f.tracking=999;assert(g.Advance(f,volume(5000),5000)==wash::Step::Cancelled);
    f=frame(6000);assert(g.Begin(f,volume(6000),6000));auto v=volume(6000);v.smartObject=901;assert(g.Advance(f,v,6000)==wash::Step::Cancelled);
    wash::SetCurrentReader(reader);reset(10000);
    service(frame(10020));assert(wash::Wanted());wash::SetEnabled(false);assert(wash::Wanted());service(frame(10040));assert(count("CANCEL")==1);assert(!wash::Wanted());
    reset(11000);probeFails=true;
    for(unsigned n=11020;n<=14500;n+=20)service(frame(n));assert(count("PROBE")==8);assert(count("COMMIT")==0);
    // A leave/re-enter starts a fresh eligibility episode; no permanent block.
    auto none=frame(14520);none.target=0;service(none);service(frame(14540));assert(count("PROBE")==9);
    reset(16000);commitUnknown=true;
    for(unsigned n=16020;n<=16160;n+=20)service(frame(n));
    for(unsigned n=16180;n<18000;n+=20){f=frame(n);atFace(f);service(f);}assert(count("COMMIT")==1);
    const int probesBeforeWithdraw=count("PROBE");
    for(unsigned n=18000;n<=18300;n+=20)service(frame(n));assert(count("PROBE")==probesBeforeWithdraw);
    // Mere eligibility return does not rearm. Both hands positively withdraw.
    for(unsigned n=18320;n<=18380;n+=20){f=frame(n);f.target=0;for(auto&p:f.palm){p[1]=-.6f;p[2]=.3f;}service(f);}
    // Even a short off/on transition discards prior withdrawal progress.
    wash::SetEnabled(false);f=frame(18400);f.target=0;service(f);wash::SetEnabled(true);
    for(unsigned n=18420;n<=18600;n+=20){f=frame(n);f.target=0;for(auto&p:f.palm){p[1]=-.6f;p[2]=.3f;}service(f);}
    assert(wash::latched.load());assert(count("COMMIT")==1);
    // A suspended service/invalid tracking gap cannot count as withdrawal.
    for(unsigned n=18620;n<=19000;n+=20){f=frame(n);f.allowed=false;f.target=0;service(f);}
    for(unsigned n=19020;n<=19200;n+=20){f=frame(n);f.target=0;for(auto&p:f.palm){p[1]=-.6f;p[2]=.3f;}service(f);}
    assert(wash::latched.load());assert(count("COMMIT")==1);
    for(unsigned n=19220;n<=19300;n+=20){f=frame(n);f.target=0;for(auto&p:f.palm){p[1]=-.6f;p[2]=.3f;}service(f);}
    assert(!wash::Wanted());assert(count("COMMIT")==1);
    reset(20000);service(frame(20020));readerCurrent=false;service(frame(20040));assert(count("COMMIT")==0);assert(count("CANCEL")==1);
    // D63: native Animation triggers may explicitly have no smart-object link.
    // A link appearing/disappearing during an armed gesture must still cancel.
    reset(21000);probeSmartObject=0;
    for(unsigned n=21020;n<=21120;n+=20)service(frame(n));
    assert(wash::active.load()&&count("COMMIT")==0);probeSmartObject=900;
    service(frame(21140));assert(count("CANCEL")==1&&count("COMMIT")==0);
    reset(22000);
    for(unsigned n=22020;n<=22120;n+=20)service(frame(n));
    probeSmartObject=0;service(frame(22140));assert(count("CANCEL")==1&&count("COMMIT")==0);
    reset(23000);probeSmartObject=0;
    for(unsigned n=23020;n<=23160;n+=20)service(frame(n));
    for(unsigned n=23180;n<24000;n+=20){f=frame(n);atFace(f);service(f);}
    assert(count("COMMIT")==1&&wash::latched.load());
    std::cout<<"washing_d59: core + actual Service transport tests PASS; linked/unlinked native leases and once-only dispatch\n";
}
